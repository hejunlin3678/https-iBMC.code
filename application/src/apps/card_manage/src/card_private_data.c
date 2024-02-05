

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "share_card.h"





LOCAL GMutex g_pm_ref_mutex; // Pcie卡私有数据引用计数锁
LOCAL guint8 g_access_hardware_err_cnt[MAX_CARD_CNT] = {0};

void pm_ref_mutex_init(void)
{
    g_mutex_init(&g_pm_ref_mutex);
}


gint32 pm_get_binded_data(OBJ_HANDLE obj_handle, gpointer *pointer)
{
    gint32 ret;
    PM_CHIP_INFO *pm_chip_info = NULL;

    if (pointer == NULL) {
        return RET_ERR;
    }

    g_mutex_lock(&g_pm_ref_mutex);
    ret = dfl_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (ret != DFL_OK || pm_chip_info == NULL) {
        *pointer = NULL;
        g_mutex_unlock(&g_pm_ref_mutex);
        return ret;
    }
    if (pm_chip_info->ref_cnt == 0) {
        *pointer = NULL;
        g_mutex_unlock(&g_pm_ref_mutex);
        return RET_ERR;
    }
    pm_chip_info->ref_cnt++;
    *pointer = pm_chip_info;
    g_mutex_unlock(&g_pm_ref_mutex);
    return RET_OK;
}


void pm_unref_binded_data(gpointer pointer)
{
    PM_CHIP_INFO *pm_chip_info = NULL;
    if (pointer == NULL) {
        return;
    }
    g_mutex_lock(&g_pm_ref_mutex);
    pm_chip_info = (PM_CHIP_INFO *)pointer;
    if (pm_chip_info->ref_cnt == 0) {
        debug_log(DLOG_ERROR, "pm_chip_info double-freed!");
        g_mutex_unlock(&g_pm_ref_mutex);
        return;
    }
    pm_chip_info->ref_cnt--;
    if (pm_chip_info->ref_cnt != 0) {
        g_mutex_unlock(&g_pm_ref_mutex);
        return;
    }
    pm_clear(pointer);

    g_mutex_unlock(&g_pm_ref_mutex);
}


LOCAL void pm_chip_info_init_basic(PM_CHIP_INFO *pm_chip_info, guint8 protocol)
{
    g_mutex_init(&pm_chip_info->rw_mutex);
    g_mutex_init(&pm_chip_info->key_mutex);

    pm_chip_info->pf_info_list = NULL;
    pm_chip_info->pf_update_delay_cnt = 0;
    pm_chip_info->inf_protocol = protocol;
    pm_chip_info->log_delay_time_for_rw = LOG_DELAY_TIME_FOR_RW;
    pm_chip_info->common_delay_time_for_rw = COMMON_DELAY_TIME_FOR_RW;
    pm_chip_info->collect_log_task_id = 0;
    pm_chip_info->last_fault_code_len = 0;
    pm_chip_info->log_dump_trigger = LOG_DUMP_TRIGGER_RESET_BMC;
    pm_chip_info->ref_cnt = 1;
}


LOCAL void pm_chip_info_clear_recv_data(PM_CHIP_INFO *pm_chip_info)
{
    switch (pm_chip_info->inf_protocol) {
        case PCIE_MGNT_PROTOCOL_I2C:
            g_mutex_lock(&pm_chip_info->key_mutex);

            if (pm_chip_info->data_region_table.data_region != NULL) {
                g_free(pm_chip_info->data_region_table.data_region);
                pm_chip_info->data_region_table.data_region = NULL;
            }

            g_mutex_unlock(&pm_chip_info->key_mutex);
            break;
        case PCIE_MGNT_PROTOCOL_SMBUS:
        case PCIE_MGNT_PROTOCOL_STD_SMBUS:
            g_mutex_lock(&pm_chip_info->key_mutex);

            if (pm_chip_info->receive_data_table.opcode != NULL) {
                g_free(pm_chip_info->receive_data_table.opcode);
                pm_chip_info->receive_data_table.opcode = NULL;
            }

            g_mutex_unlock(&pm_chip_info->key_mutex);
            break;
        default:
            // do nothing
            break;
    }
}


LOCAL gboolean __is_support_oob_over_i2c(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gchar card_desc[MAX_PCIE_CARD_DESC_LEN] = {0};

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_DESC, card_desc, sizeof(card_desc));
    if (ret != RET_OK) {
        return FALSE;
    }

    if (strcmp(card_desc, VIRTUAL_FPGA_CARD_DES) == 0) {
        return FALSE;
    }
    

    return TRUE;
}


LOCAL gboolean __is_valid_oob_protocol(guint8 protocol)
{
    if (protocol == PCIE_MGNT_PROTOCOL_I2C || protocol == PCIE_MGNT_PROTOCOL_SMBUS ||
        protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS || protocol == PCIE_MGNT_PROTOCOL_SDI_CPLD ||
        protocol == PCIE_MGNT_PROTOCOL_SDI_MULTI_HOST_CPLD || protocol == PCIE_MGNT_PROTOCOL_SMBUS_REG ||
        protocol == PCIE_MGNT_PROTOCOL_EMPTY) {
        return TRUE;
    }

    return FALSE;
}


LOCAL void __reset_failed_opeation_timestamp(PM_CHIP_INFO *pm_chip_info)
{
    pm_chip_info->temp_read_fail_timestamp = 0;
    pm_chip_info->ddr_temp_read_fail_timestamp = 0;
    pm_chip_info->power_read_fail_timestamp = 0;
    pm_chip_info->optical_temp_read_fail_timestamp = 0;
    pm_chip_info->optical_current_read_fail_timestamp = 0;

    return;
}

LOCAL gboolean is_no_process_protocol(guint8 inf_protocol)
{
    if (inf_protocol == PCIE_MGNT_PROTOCOL_SDI_CPLD ||
        inf_protocol == PCIE_MGNT_PROTOCOL_SDI_MULTI_HOST_CPLD ||
        inf_protocol == PCIE_MGNT_PROTOCOL_EMPTY) {
        return TRUE;
    }
    return FALSE;
}


LOCAL void __reset_log_dump_trigger(PM_CHIP_INFO *pm_chip_info, gpointer user_data)
{
    gint32 init_flag = 0;
    if (user_data != NULL) {
        init_flag = *(gint32 *)user_data;
    }
    
    if (init_flag != PCIE_INIT_WITHOUT_LOG_DUMP && pm_chip_info->log_dump_trigger == LOG_DUMP_TRIGGER_UNRESET) {
        pm_chip_info->log_dump_trigger =
            LOG_DUMP_TRIGGER_RESET_OS; 
    }
    

    return;
}


LOCAL gint32 __oob_mgnt_init(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = 0;
    // i2coverlan 需要通过rpc访问,mm板重启时间较长，增加到30次
    const guint8 MAX_TIMES = 30;
    guint8 retry_time;
    gint32 init_result = RET_OK;

    // 初始化失败则重试，最多重试30次
    for (retry_time = 0; retry_time < MAX_TIMES; retry_time++) {
        if (PCIE_MGNT_PROTOCOL_I2C == pm_chip_info->inf_protocol) {
            ret = pcie_mgnt_init_info_by_i2c(obj_handle, pm_chip_info);
        } else if (PCIE_MGNT_PROTOCOL_SMBUS == pm_chip_info->inf_protocol) {
            ret = pcie_mgnt_init_info_by_smbus(obj_handle, pm_chip_info);
        } else if (is_no_process_protocol(pm_chip_info->inf_protocol) == TRUE) {
            ret = RET_OK;
        } else {
            ret = pcie_mgnt_init_info_by_std_smbus(obj_handle, pm_chip_info);
        }

        if (ret == RET_OK) {
            pm_chip_info->init_state = INIT_STATE_SUCCESS;
            break;
        }

        debug_log(DLOG_ERROR, "%s: init PCIe device OOB over i2c/smbus failed, protocol = %d", __FUNCTION__,
            pm_chip_info->inf_protocol);
        pm_chip_info_clear_recv_data(pm_chip_info);

        vos_task_delay(6000);
    }

    if ((retry_time >= MAX_TIMES) && (ret != RET_OK)) {
        init_result = RET_ERR;
        debug_log(DLOG_ERROR, "%s: init PCIe device (%s) OOB failed retried (%u) > MAX_TIMES(%u)", __FUNCTION__,
            dfl_get_object_name(obj_handle), retry_time, MAX_TIMES);
        pm_chip_info->init_state = INIT_STATE_FAIL;
    }

    return init_result;
}


LOCAL gboolean is_support_extend_pciecard_extra_init(OBJ_HANDLE obj_handle)
{
    gchar class_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE basecard_obj_handle = 0;
    gint32 ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        return FALSE;
    }
    // 获取类对象为pciecard
    if (g_strcmp0(class_name, CLASS_EXTEND_PCIECARD_NAME) != 0) {
        return FALSE;
    }
    ret = dfl_get_referenced_object(obj_handle, PROPETRY_EXTEND_PCIECARD_BASECARD, &basecard_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get extend pcie card base, object: %s, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return FALSE;
    }
    debug_log(DLOG_INFO, "%s: get extend pcie card base, extend: %s, object_basecard : %s", __FUNCTION__,
        dfl_get_object_name(obj_handle), dfl_get_object_name(basecard_obj_handle));

    if (is_sdi_support_mcu(basecard_obj_handle) == TRUE) {
        return TRUE;
    }
    return FALSE;
}


LOCAL void __oob_extra_init(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gboolean is_support_extra = is_support_extend_pciecard_extra_init(obj_handle);
    // smbus协议初始化时还需手动触发电子标签初始化流程
    if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS || is_support_extra == TRUE) {
        // 手动触发FRU电子标签的初始化流程
        init_elabel_info(obj_handle);
    }

    return;
}


gint32 pcie_mgnt_init_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    PM_CHIP_INFO *pm_chip_info = NULL;
    guint8 protocol = 0;
    gint32 init_result = RET_OK;
    gint32 ret_get_binded = FALSE;

    
    if (__is_support_oob_over_i2c(obj_handle) == FALSE) {
        return RET_OK;
    }
    

    // 获取芯片支持协议
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_PROTOCOL, &protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIe card OOB protocol failed, return %d", __FUNCTION__, ret);
        return RET_OK;
    }

    if (__is_valid_oob_protocol(protocol) == FALSE) {
        debug_log(DLOG_DEBUG, "%s: protocol %d is invalid", __FUNCTION__, protocol);
        return RET_OK;
    }

    ret = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (ret == RET_OK && pm_chip_info != NULL) {
        ret_get_binded = TRUE;
    }

    if (ret_get_binded == FALSE || pm_chip_info == NULL) {
        pm_chip_info = (PM_CHIP_INFO *)g_malloc0(sizeof(PM_CHIP_INFO));
        if (pm_chip_info == NULL) {
            debug_log(DLOG_ERROR, "%s: g_malloc failed", __FUNCTION__);
            return RET_OK;
        }

        pm_chip_info_init_basic(pm_chip_info, protocol);
        pm_chip_info->reset_mcu_state = RESET_MCU_IDLE;
    }

    __reset_failed_opeation_timestamp(pm_chip_info);
    __reset_log_dump_trigger(pm_chip_info, user_data);

    init_result = __oob_mgnt_init(obj_handle, pm_chip_info);
    if (ret_get_binded == FALSE) {
        ret = dfl_bind_object_data(obj_handle, (gpointer)pm_chip_info, pm_unref_binded_data);
        if (ret != DFL_OK) {
            pm_clear((gpointer)pm_chip_info);
            debug_log(DLOG_ERROR, "%s: dfl_bind_object_data fail, return is %d", __FUNCTION__, ret);
            return init_result;
        }
    }

    __oob_extra_init(obj_handle, pm_chip_info);

    if (ret_get_binded == TRUE) {
        pm_unref_binded_data((gpointer)pm_chip_info);
        pm_chip_info = NULL;
    }

    return init_result;
}


gint32 pcie_mgnt_init_virtual_card_info(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    PM_CHIP_INFO *pm_chip_info = NULL;
    guint8 protocol = 0;
    guint16 *opcode_restore = NULL;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_PROTOCOL, &protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): get interface_spec failed!", __FUNCTION__, __LINE__);
        return RET_OK;
    }

    if (protocol == PCIE_MGNT_INVALID_PROTOCOL) {
        return RET_OK;
    }

    pm_chip_info = (PM_CHIP_INFO *)g_malloc0(sizeof(PM_CHIP_INFO));
    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): pm_chip_info g_malloc failed!", __FUNCTION__, __LINE__);
        return RET_OK;
    }

    pm_chip_info_init_basic(pm_chip_info, protocol);

    // support 2 opcode at least!
    opcode_restore = (guint16 *)g_malloc0(sizeof(guint16) * 2);
    if (opcode_restore == NULL) {
        g_free(pm_chip_info);
        debug_log(DLOG_ERROR, "%s: malloc memory fail.", __FUNCTION__);
        return RET_ERR;
    }
    opcode_restore[0] = REGISTER_KEY_FPGA_SELFHEALING;              // start
    opcode_restore[1] = REGISTER_KEY_FPGA_QUERY_SELFHEALING_STATUS; // get status
    pm_chip_info->receive_data_table.opcode = opcode_restore;
    pm_chip_info->receive_data_table.opcode_num = 2;

    const gchar *ptr_obj_name = NULL;
    OBJ_HANDLE ref_handle = 0;

    // 获取refChip对象即mcu地址信息
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REFCHIP, &ref_handle);
    if (ret == DFL_ERR) {
        g_free(opcode_restore);
        g_free(pm_chip_info);
        debug_log(DLOG_ERROR, "%s: get chip handle failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 获取MCU对象
    ptr_obj_name = dfl_get_object_name(ref_handle);
    if (ptr_obj_name == NULL) {
        g_free(opcode_restore);
        g_free(pm_chip_info);
        debug_log(DLOG_ERROR, "%s: failed to get object name", __FUNCTION__);
        return RET_ERR;
    }
    (void)strncpy_s(pm_chip_info->chip_name, sizeof(pm_chip_info->chip_name), ptr_obj_name,
        sizeof(pm_chip_info->chip_name) - 1);

    // 获取单帧长度
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_MAXFRAMELEN, &pm_chip_info->framelen);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: get pcie card frame len failed, max frame len is %d", __FUNCTION__,
            pm_chip_info->framelen);
        g_free(opcode_restore);
        g_free(pm_chip_info);
        return ret;
    }

    // binded data will be freed by pme!
    ret = dfl_bind_object_data(obj_handle, (gpointer)pm_chip_info, pm_unref_binded_data);
    if (ret == DFL_ERR) {
        g_free(opcode_restore);
        g_free(pm_chip_info);
        debug_log(DLOG_ERROR, "%s : dfl_bind_object_data fail, result is %d", __FUNCTION__, ret);
        return ret;
    }
    
    return RET_OK;
}


void _update_pcie_connector_binded_data(OBJ_HANDLE connector_handle, const NODE_PCIE_INFO_S *node_pcie_info)
{
    NODE_PCIE_INFO_S *data_dump = NULL;
    errno_t ptr_ret = EOK;
    gint32 ret;
    gpointer user_data = NULL;

    if (node_pcie_info == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return;
    }

    data_dump = (NODE_PCIE_INFO_S *)g_malloc0(sizeof(NODE_PCIE_INFO_S));
    if (data_dump == NULL) {
        debug_log(DLOG_ERROR, "%s failed:alloc new memroy failed, size (%zu)", __FUNCTION__, sizeof(NODE_PCIE_INFO_S));
        return;
    }

    ptr_ret =
        memmove_s((void *)data_dump, sizeof(NODE_PCIE_INFO_S), (const void *)node_pcie_info, sizeof(NODE_PCIE_INFO_S));
    if (ptr_ret != EOK) {
        debug_log(DLOG_ERROR, "copy node pcie info failed");
        g_free(data_dump);
        return;
    }

    (void)dfl_get_binded_data(connector_handle, &user_data);
    if (user_data != NULL) {
        debug_log(DLOG_INFO, "Warning:object (%s) has been binded with data", dfl_get_object_name(connector_handle));
    }

    ret = dfl_bind_object_data(connector_handle, data_dump, (GDestroyNotify)g_free);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "bind pcie info to object (%s) failed, ret is %d", dfl_get_object_name(connector_handle),
            ret);
        g_free(data_dump);
    }

    return;
}

gint32 card_action_add_object_callback(OBJ_HANDLE object_handle)
{
    guint8 *priv_data = NULL;
    gint32 result = RET_OK;

    priv_data = (guint8 *)g_malloc0(sizeof(PCIE_ACTION_PRIV_DATA_S));
    if (priv_data == NULL) {
        debug_log(DLOG_ERROR, "priv_data malloc is NULL");
        return RET_ERR;
    }

    (void)memset_s(priv_data, sizeof(PCIE_ACTION_PRIV_DATA_S), 0xff, sizeof(PCIE_ACTION_PRIV_DATA_S));

    
    result = dfl_bind_object_data(object_handle, priv_data, g_free);
    if (result != DFL_OK) {
        g_free(priv_data);
        debug_log(DLOG_ERROR, "dfl_bind_object_data fail!result = %d", result);
        return result;
    }
    (void)pangea_card_action_add_object_callback(object_handle);
    return RET_OK;
}

gint32 pcie_init_action_foreach(OBJ_HANDLE handle, gpointer user_data)
{
    guint8 *priv_data = NULL;
    gint32 result = RET_OK;

    priv_data = (guint8 *)g_malloc0(sizeof(PCIE_ACTION_PRIV_DATA_S));
    if (priv_data == NULL) {
        debug_log(DLOG_ERROR, "priv_data malloc is NULL");
        return RET_OK;
    }

    (void)memset_s(priv_data, sizeof(PCIE_ACTION_PRIV_DATA_S), 0xff, sizeof(PCIE_ACTION_PRIV_DATA_S));

    
    result = dfl_bind_object_data(handle, priv_data, g_free);
    if (result != DFL_OK) {
        g_free(priv_data);
        debug_log(DLOG_ERROR, "dfl_bind_object_data for %s fail! ret = %d", dfl_get_object_name(handle), result);
        return result;
    }
    (void)init_pangea_card_action(handle, user_data);
    return RET_OK;
}


LOCAL void card_manage_init_head_para(PROTOCOL_HEAD_PARA *phead, gint32 size, GSList *input_list)
{
    (void)memset_s(phead, size, 0, size);
    phead->opcode = g_variant_get_uint16((GVariant *)(g_slist_nth_data(input_list, 0)));
    ;
    phead->data_object_index = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 1)));
    phead->cmd_arg = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 2)));

    debug_log(DLOG_DEBUG, "%s: opcode is %d, cmd_arg is 0x%x, data_object_index is %d", __FUNCTION__, phead->opcode,
        phead->cmd_arg, phead->data_object_index);

    return;
}


gint32 card_manage_mcu_get_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
#define INPUT_PARA_VALID_NUM 4

    gint32 ret = 0;
    const guint8 *input_data = NULL;
    guint32 input_data_size = 0;
    PM_CHIP_INFO *chip_info = NULL;
    PROTOCOL_HEAD_PARA head_para;
    guint8 *buf = NULL;
    guint32 buf_len = 0;
    guint8 list_length = 0;
    gsize parm_temp = 0;

    if (object_handle == 0 || input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    list_length = g_slist_length(input_list);
    // 第4个参数可选，因此input_list为3合法
    if (list_length != INPUT_PARA_VALID_NUM && list_length != INPUT_PARA_VALID_NUM - 1) {
        debug_log(DLOG_ERROR, "%s: input parameter cnt error, list len is %d", __FUNCTION__, list_length);
        return RET_ERR;
    }

    // 入参链表数据解析并初始化
    card_manage_init_head_para(&head_para, sizeof(head_para), input_list);

    // 请求数据不为空时，获取第4个参数
    if (list_length == INPUT_PARA_VALID_NUM) {
        input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 3)),
            &parm_temp, sizeof(guint8));
        input_data_size = (guint32)parm_temp;
    }

    // 获取PCIE卡绑定的私有对象
    ret = pm_get_binded_data(object_handle, (gpointer *)&chip_info);
    if (ret != RET_OK || chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: get binded data failed. The error code is %d, object name is %s", __FUNCTION__, ret,
            dfl_get_object_name(object_handle));
        return ret;
    }

    if ((input_data != NULL) && (input_data_size > 0)) {
        errno_t securec_rv = memcpy_s(head_para.input_data, HEAD_PARA_MAX_LEN, input_data, input_data_size);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, securec_rv);
        }
        head_para.input_data_len = input_data_size;
    }

    ret = pm_get_random_len_data_by_std_smbus(chip_info, (guint8 *)&head_para, &buf, &buf_len);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get data failed, ret is %d", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)chip_info);
        return ret;
    }

    // 读取的数据封装
    *output_list =
        g_slist_append(*output_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, buf, buf_len, sizeof(guint8)));

    g_free(buf);
    pm_unref_binded_data((gpointer)chip_info);

    return RET_OK;
}

gint32 card_manage_mcu_set_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
#define INPUT_PARA_NUM 4

    gint32 ret = 0;
    guint16 opcode = 0;
    const guint8 *buf = NULL;
    guint32 buf_size = 0;
    PM_CHIP_INFO *chip_info = NULL;
    PROTOCOL_HEAD_PARA head_para;
    guint8 cmd_arg = 0;
    guint8 data_obj_index = 0;
    guint8 list_length = 0;
    gsize parm_temp = 0;

    if (object_handle == 0 || input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    list_length = g_slist_length(input_list);
    if (list_length != INPUT_PARA_NUM) {
        debug_log(DLOG_ERROR, "%s: input parameter cnt error, list len is %d", __FUNCTION__, list_length);
        return RET_ERR;
    }

    // 入参链表数据解析
    opcode = g_variant_get_uint16((GVariant *)(g_slist_nth_data(input_list, 0)));
    data_obj_index = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 1)));
    cmd_arg = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 2)));
    buf = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 3)), &parm_temp,
        sizeof(guint8));
    buf_size = (guint32)parm_temp;

    if ((buf == NULL) || (buf_size == 0)) {
        debug_log(DLOG_ERROR, "%s: data to write is null, object name is %s", __FUNCTION__,
            dfl_get_object_name(object_handle));
        return ret;
    }

    // 获取PCIE卡绑定的私有对象
    ret = pm_get_binded_data(object_handle, (gpointer *)&chip_info);
    if (ret != RET_OK || chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: get binded data failed, error code is %d, object name is %s", __FUNCTION__, ret,
            dfl_get_object_name(object_handle));
        return ret;
    }

    // 请求数据封装
    (void)memset_s(&head_para, sizeof(head_para), 0, sizeof(head_para));
    head_para.opcode = opcode;
    head_para.data_object_index = data_obj_index;
    head_para.input_data_len = 0;
    head_para.cmd_arg = cmd_arg;

    ret = pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, buf, buf_size);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set data to mcu failed, ret is %d", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)chip_info);
        return ret;
    }

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}


LOCAL gint32 init_pfport_info(GSList **pfport_list, guint16 physical_port, guint16 pf, gchar *mac_addr)
{
    errno_t safe_fun_ret = EOK;
    // 此处申请的内存，保存到私有数据中，不释放，直到card对象被注销；
    PFPORT_INFO_S *p_pfport_info_new = (PFPORT_INFO_S *)g_malloc0(sizeof(PFPORT_INFO_S));
    if (p_pfport_info_new == NULL) {
        debug_log(DLOG_ERROR, "%s: p_pfport_info_new malloc memory failed ", __FUNCTION__);
        return RET_ERR;
    }

    p_pfport_info_new->pf = pf;
    p_pfport_info_new->physical_port = physical_port;
    safe_fun_ret = strncpy_s(p_pfport_info_new->mac_addr, MACADDRESS_LEN + 1, mac_addr, strlen(mac_addr));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    debug_log(DLOG_INFO, "%s, init_pfport_info physical_port %d, pf = %d ", __FUNCTION__, physical_port, pf);
    // 添加元素到链表 并排序
    *pfport_list = g_slist_insert_sorted(*pfport_list, (gpointer)p_pfport_info_new, _pf_port_compare);
    return safe_fun_ret;
}

gint32 pcie_card_set_pfinfo(OBJ_HANDLE extend_card_handle, PM_CHIP_INFO *pm_chip_info, guint16 pf,
    guint16 physical_port, gchar *mac_addr)
{
    gint32 ret = RET_OK;
    GSList *node = NULL;
    errno_t safe_fun_ret = EOK;
    gint32 pf_exist = FALSE;
    PFPORT_INFO_S *p_pfport_info = NULL;
    const gchar *handle_name = dfl_get_object_name(extend_card_handle);

    if (pm_chip_info == NULL || mac_addr == NULL) {
        debug_log(DLOG_ERROR, "%s: input error", __FUNCTION__);
        return RET_ERR;
    }

    // 调用此函数的时候，pm_chip_info肯定不为空；
    // 如果链表为空，则是第一次查询到pf信息，保存到链表中；
    if (pm_chip_info->pf_info_list == NULL) {
        ret = init_pfport_info(&pm_chip_info->pf_info_list, physical_port, pf, mac_addr);
        debug_log(DLOG_INFO, "%s, extend_card_handle %s, pf = %d pf_info_list = NULL", __FUNCTION__, handle_name, pf);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, init_pfport_info failed, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } else { // 如果链表不为空，遍历链表；修改，新增；
        for (node = pm_chip_info->pf_info_list; node; node = g_slist_next(node)) {
            p_pfport_info = (PFPORT_INFO_S *)node->data;
            if (p_pfport_info == NULL) {
                debug_log(DLOG_ERROR, "%s, p_pfport_private = NULL", __FUNCTION__);
                break;
            }
            // 如果找到对应的pf，则修改
            if (p_pfport_info->pf == pf) {
                p_pfport_info->physical_port = physical_port;
                safe_fun_ret =
                    strncpy_s(p_pfport_info->mac_addr, MACADDRESS_LEN + 1, mac_addr, NETCARD_MAC_ADDRESS_LENGTH);
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
                }
                debug_log(DLOG_INFO, "%s, extend_card_handle %s modify exist pfport_info", __FUNCTION__, handle_name);
                pf_exist = TRUE;
                break;
            }
        }
        // 链表不为空，但是没找到对应的pf，则新增；
        if (pf_exist == FALSE) {
            ret = init_pfport_info(&pm_chip_info->pf_info_list, physical_port, pf, mac_addr);
            debug_log(DLOG_INFO, "%s, extend_card_handle %s new pfport_info", __FUNCTION__, handle_name);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s, init_pfport_info failed, ret = %d", __FUNCTION__, ret);
                return RET_ERR;
            }
        }
    }
    return ret;
}


LOCAL gint32 get_pfport_info(PFPORT_INFO_S *p_pfport_info_tmp, PFPORT_INFO_S *p_pfport_info, gint32 pf_count)
{
    errno_t safe_fun_ret = EOK;

    // 获取PF，MAC地址等信息，添加到output_list
    if ((p_pfport_info == NULL) || (pf_count >= BUSSINESS_PORT_PFINFO_MAX_CNT)) {
        debug_log(DLOG_ERROR, "%s, pfport_private = NULL, pf_count %d >= BUSSINESS_PORT_PFINFO_MAX_CNT", __FUNCTION__,
            pf_count);
        return RET_ERR;
    }

    p_pfport_info_tmp[pf_count].pf = p_pfport_info->pf;
    p_pfport_info_tmp[pf_count].physical_port = p_pfport_info->physical_port;
    safe_fun_ret = strncpy_s(p_pfport_info_tmp[pf_count].mac_addr, MACADDRESS_LEN + 1, p_pfport_info->mac_addr,
        strlen(p_pfport_info->mac_addr));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    debug_log(DLOG_INFO, "%s, pf is %d, mac_addr %s, pf_count: %d", __FUNCTION__, p_pfport_info->pf,
        p_pfport_info->mac_addr, pf_count);

    return RET_OK;
}


gint32 method_get_pfinfo(OBJ_HANDLE extend_card_obj, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = RET_ERR;
    gint32 pf_count = 0;
    GSList *obj_node = NULL;
    guint32 pfport_info_size = 0;
    PFPORT_INFO_S *p_pfport_info_tmp = NULL;
    PM_CHIP_INFO *pm_chip_info = NULL;

    // 获取绑定信息；
    ret = pm_get_binded_data(extend_card_obj, (gpointer *)&pm_chip_info);
    if (output_list == NULL || ret != RET_OK || pm_chip_info == NULL) {
        debug_log(DLOG_INFO, "%s, pm_get_binded_data failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }
    // 共16组pf信息；
    p_pfport_info_tmp = (PFPORT_INFO_S *)g_malloc0(sizeof(PFPORT_INFO_S) * BUSSINESS_PORT_PFINFO_MAX_CNT);
    if (p_pfport_info_tmp == NULL) {
        debug_log(DLOG_ERROR, "malloc failed");
        pm_unref_binded_data((gpointer)pm_chip_info);
        return RET_ERR;
    }

    if ((ret == ERRCODE_OBJECT_NOT_EXIST) || (pm_chip_info->pf_info_list == NULL)) {
        // 私有数据不存在，或者pflist为空，表示没有pf信息，不退出，填充无效值；
        debug_log(DLOG_ERROR, "%s, get extend_card_obj bind_object_data failed result is %d ", __FUNCTION__, ret);
    } else {
        for (obj_node = pm_chip_info->pf_info_list; obj_node; obj_node = g_slist_next(obj_node)) {
            // 获取PF，MAC地址等信息，添加到output_list
            if (get_pfport_info(p_pfport_info_tmp, (PFPORT_INFO_S *)obj_node->data, pf_count) == RET_ERR) {
                break;
            }
            pf_count++;
        }
    }
    // 目前最多16个pf，如果获取的pf不够16个，就添加无效值
    while (pf_count < BUSSINESS_PORT_PFINFO_MAX_CNT) {
        p_pfport_info_tmp[pf_count].pf = 0xffff;
        p_pfport_info_tmp[pf_count].physical_port = 0xffff;
        (void)strncpy_s(p_pfport_info_tmp[pf_count].mac_addr, MACADDRESS_LEN + 1, "00:00:00:00:00:00\0",
            strlen("00:00:00:00:00:00\0"));
        debug_log(DLOG_INFO, "%s, pf_count: %d", __FUNCTION__, pf_count);
        pf_count++;
    }
    pfport_info_size = sizeof(PFPORT_INFO_S) * BUSSINESS_PORT_PFINFO_MAX_CNT;
    debug_log(DLOG_INFO, "%s, pfport_info_size: %d", __FUNCTION__, pfport_info_size);

    // 为避免class里面方法定义太多的输出参数，此处用数组方式；
    *output_list = g_slist_append(*output_list,
        g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, (guint8 *)p_pfport_info_tmp, pfport_info_size, sizeof(guint8)));
    g_free(p_pfport_info_tmp);

    pm_unref_binded_data((gpointer)pm_chip_info);
    return RET_OK;
}

LOCAL void pcie_card_get_netcard_pf_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = RET_OK;
    guint8 *netcard_mac_adress = NULL;
    guint32 netcard_mac_address_total_len = 0;
    gint32 need_update_pf = TRUE;
    guint8 channal_index = 0;
    guint16 physical_port = 0xffff;
    guint16 pf = 0xffff;
    gchar    mac_addr[NETCARD_MAC_ADDRESS_LENGTH] = {0};
    gint32 mac_i = 0;
    guint32 mac_num = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }
    // 检查此次是否需要去查询pfinfo，如果不需要，就退出，部分场景不需要获取pf信息；
    need_update_pf = check_update_pfinfo_condition(obj_handle, pm_chip_info);
    if (need_update_pf == FALSE) {
        return;
    }

    // 调用I2C接口去固件获取pf，MAC地址信息；
    ret = pm_get_netcard_mac_address(pm_chip_info, &netcard_mac_adress, &netcard_mac_address_total_len);
    if ((ret != RET_OK) || (netcard_mac_adress == NULL)) {
        debug_log(DLOG_ERROR, "%s Get netcard MAC address failed, ret = 0x%4x", __FUNCTION__, ret);
        if (netcard_mac_adress != NULL) {
            g_free(netcard_mac_adress);
        }
        return;
    }

    // 校验通过SMBus获取的MAC地址长度参数，避免出现异常情况下因无符号数翻转导致死循环
    mac_num = netcard_mac_address_total_len / NETCARD_MAC_ADDRESS_RESPONSE_LENGTH;
    if (mac_num > UCHAR_MAX) {
        debug_log(DLOG_ERROR, "%s: invalid mac addr total len 0x%4x", __FUNCTION__, netcard_mac_address_total_len);
        g_free(netcard_mac_adress);
        return;
    }

    // 获取MAC地址成功
    for (channal_index = 0; channal_index < (guint8)mac_num; channal_index++) {
        // 解析pfvf，port，将mac地址拼接成为字符串
        // 根据接口定义:命令字返回信息中，第一个字节是物理端口号1~N
        physical_port = netcard_mac_adress[9 * channal_index];
        // 第2，3个字节是PF/VF，最高位是0代表pf，最高位是1代表vf，当前只用pf；
        pf = (netcard_mac_adress[1 + 9 * channal_index] << 16) + netcard_mac_adress[2 + 9 * channal_index];

        (void)memset_s(mac_addr, NETCARD_MAC_ADDRESS_LENGTH, 0, NETCARD_MAC_ADDRESS_LENGTH);
        // 第三个开始是MAC地址，小字节序；有6组数据，加5个:字符，加结束符18个；
        for (mac_i = 0; mac_i < MAC_ADDRESS_LENGTH; ++mac_i) {
            ret = snprintf_truncated_s((mac_addr + 3 * mac_i), (NETCARD_MAC_ADDRESS_LENGTH - 3 * mac_i),
                "%02x:", netcard_mac_adress[3 + mac_i + 9 * channal_index]);
            if (ret <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_truncated_s fail, ret = %d", __FUNCTION__, ret);
            }
        }
        mac_addr[NETCARD_MAC_ADDRESS_LENGTH - 1] = '\0';

        debug_log(DLOG_INFO, "%s i = %d: port = %02x, pf = %04x, mac_addr = %s ", __FUNCTION__, channal_index,
            physical_port, pf, mac_addr);
        // 将pf信息写入到私有数据中；
        ret = pcie_card_set_pfinfo(obj_handle, pm_chip_info, pf, physical_port, mac_addr);
        debug_log(DLOG_INFO, "%s channal_index %d pcie_card_set_pfinfo, ret = %d ", __FUNCTION__, channal_index, ret);

        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s:Set channal_index %d pfinfo  fail, ret %d.", __FUNCTION__, channal_index, ret);
            continue;
        }
    }
    g_free(netcard_mac_adress);
    return;
}


LOCAL gint32 update_pcie_card_info_by_i2c(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret;
    gint32 firmware_state = FIRMWARE_UNKNOW;
    OBJ_HANDLE slave_obj_handle = 0;
    gchar slave_card_class_name[CLASSE_NAME_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: pm_chip_info is null", __FUNCTION__);
        return RET_ERR;
    }

    // 对单卡上下电进行判断，如果下电则清理数据，并返回OK
    if (PCIE_POWER_ON != pcie_card_get_power_state(obj_handle)) {
        // 设成未初始化，下面重新初始化下
        pm_chip_info->init_state = INIT_STATE_INCOMPLETE;
        update_temp_read_state(obj_handle, SENSOR_NA_READING);
        update_ddr_temp_read_state(obj_handle, SENSOR_NA_READING);
        // 再清除故障码
        pcie_card_clear_fault_code(obj_handle, pm_chip_info);

        return RET_ERR;
    }

    
    if (INIT_STATE_FAIL == pm_chip_info->init_state && (PCIE_POWER_ON == pcie_card_get_power_state(obj_handle))) {
        
        update_temp_read_state(obj_handle, SENSOR_INVALID_READING);
        update_ddr_temp_read_state(obj_handle, SENSOR_INVALID_READING);
        // 信息获取不到置位产生告警事件
        pcie_card_get_info_fail(obj_handle);
        return RET_ERR;
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strlen(slave_card_class_name) != 0) {
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get pcie card slave card handle failed!");
            return RET_ERR;
        }

        if (ret == RET_OK) {
            firmware_state = check_firmware_info(slave_obj_handle, pm_chip_info);
            if (firmware_state == RET_ERR || pm_chip_info->init_state == INIT_STATE_INCOMPLETE) {
                pcie_mgnt_init_info(obj_handle, NULL);
                return RET_ERR;
            }

            pcie_card_update_fault_code(slave_obj_handle, pm_chip_info);
            pcie_card_update_ddr_temp(slave_obj_handle, pm_chip_info);
        }
    }

    
    pcie_card_update_temperature(obj_handle, pm_chip_info);
    
    return RET_OK;
}


LOCAL gint32 update_pcie_card_info_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret;
    OBJ_HANDLE port_handle = 0;
    guchar link_status_src = 0;
    guint8 portnum = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: pm_chip_info is null", __FUNCTION__);
        return RET_ERR;
    }

    pcie_card_update_temperature_by_smbus(obj_handle, pm_chip_info);
    pcie_card_update_optical_module_temp_by_smbus(obj_handle, pm_chip_info);
    pcie_card_update_fault_code_by_smbus(obj_handle, pm_chip_info);
    pcie_card_collect_log(obj_handle, pm_chip_info);
    
    // 从SMbus获取mac，pf，port信息，绑定到扩展卡的私有数据中；
    pcie_card_get_netcard_pf_by_smbus(obj_handle, pm_chip_info);
    
    
    // 获取businessPort端口句柄
    ret = pcie_card_get_business_port_handle_by_channel(obj_handle, 0, &port_handle);
    // 支持SMBus访问的PCIe网卡不一定配置有BusinessPort对象（如，SDI的扩展网卡），优化该场景日志打印
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        return ret;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get business port handle by channel fail, ret = %d.", __FUNCTION__, ret);
        return ret;
    }
    (void)dal_get_property_value_byte(port_handle, BUSY_ETH_ATTRIBUTE_LINK_SRC, &link_status_src);

    // 支持从带外的方式获取网卡的mac地址和link状态
    if (GET_LINK_STATUS_FROM_OUT_BAND == link_status_src) {
        get_pcie_netcard_port_num(obj_handle, &portnum);

        // 若为与BMC芯片通过PCIe链路直连的PCIe卡，则从对应的EthGroup对象同步MAC地址
        ret = dfl_get_class_name(obj_handle, class_name, sizeof(class_name));
        if ((ret == RET_OK) && (g_strcmp0(class_name, CLASS_LOCAL_PCIECARD_NAME) == 0)) {
            sync_local_pcie_card_mac_addr_by_port(obj_handle, 0);
            sync_local_pcie_card_mac_addr_by_port(obj_handle, 1);
        } else {
            pcie_card_update_netcard_mac_address_by_smbus(obj_handle, pm_chip_info, portnum);
        }
        pcie_card_update_netcard_link_status_by_smbus(obj_handle, pm_chip_info, portnum);
    }
    return RET_OK;
}

LOCAL gint32 check_is_cpld_core_err(void)
{
    GSList *obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE card_obj;
    guint8 slot;
    gint32 ret;
    gint32 need_repair_cpld = TRUE;
    ret = dfl_get_object_list(CLASS_PCIE_CARD, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get pcie card obj list failed, ret=%d.", ret);
        return FALSE;
    }
    for (node = obj_list; node; node = node->next) {
        card_obj = (OBJ_HANDLE)node->data;
        ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s get slot id failed, ret=%d.", dfl_get_object_name(card_obj), ret);
            need_repair_cpld = FALSE;
            break;
        }
        
        if ((slot <= MAX_CARD_CNT) && (g_access_hardware_err_cnt[slot - 1] < MAX_ACCESS_ERR_CNT)) {
            need_repair_cpld = FALSE;
            break;
        }
    }
    g_slist_free(obj_list);
    return need_repair_cpld;
}


LOCAL void statistic_access_err_cnt(OBJ_HANDLE card_obj)
{
    gint32 ret;
    guint8 slot;
    guint16 chip_temp;
    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get slot id failed, ret=%d.", dfl_get_object_name(card_obj), ret);
        return;
    }
    ret = dal_get_property_value_uint16(card_obj, PROPERTY_PCIE_CARD_CHIPTEMP, &chip_temp);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get chip temp failed, ret=%d.", dfl_get_object_name(card_obj), ret);
        return;
    }
    if ((chip_temp & SENSOR_INVALID_READING) && (slot <= MAX_CARD_CNT)) {
        g_access_hardware_err_cnt[slot - 1]++; 
    } else if (((chip_temp & SENSOR_INVALID_READING) == 0) && (slot <= MAX_CARD_CNT)) {
        g_access_hardware_err_cnt[slot - 1] = 0;
    }
}

LOCAL gint32 card_reset_cpld_core(OBJ_HANDLE card_obj)
{
    OBJ_HANDLE local_bus = INVALID_OBJ_HANDLE;
    GSList *input_list = NULL;
    gint32 ret;
    ret = pm_get_virtual_local_bus(card_obj, PROPERTY_PCIE_CARD_REFCHIP, CLASS_VIRTUALI2C, &local_bus);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get local bus obj failed.");
        return ret;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(0x9)); 
    input_list = g_slist_append(input_list, g_variant_new_byte(0x6)); 
    ret = dfl_call_class_method(local_bus, METHOD_RESET_CPLD_CORE, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Reset %s cpld core failed, ret=%d", dfl_get_object_name(local_bus), ret);
    }
    return ret;
}

LOCAL void repair_cpld_core(OBJ_HANDLE card_obj)
{
    gint32 need_repair_cpld;
    gint32 ret;
    statistic_access_err_cnt(card_obj);
    need_repair_cpld = check_is_cpld_core_err();
    if (need_repair_cpld == FALSE) {
        return;
    }
    ret = card_reset_cpld_core(card_obj);
    if (ret != RET_OK) {
        return;
    }
    (void)memset_s(g_access_hardware_err_cnt, MAX_CARD_CNT, 0, MAX_CARD_CNT);
    debug_log(DLOG_DEBUG, "%s repair cpld core, repair cpld core.", dfl_get_object_name(card_obj));
}


LOCAL gint32 update_pcie_card_info_by_protocol(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 inf_protocol)
{
    gint32 ret;
    if (inf_protocol == PCIE_MGNT_PROTOCOL_I2C) {
        ret = update_pcie_card_info_by_i2c(obj_handle, pm_chip_info);
        if (ret != RET_OK) {
            return RET_ERR;
        }
    } else if (inf_protocol == PCIE_MGNT_PROTOCOL_SMBUS) {
        ret = update_pcie_card_info_by_smbus(obj_handle, pm_chip_info);
        if (ret != RET_OK) {
            return RET_ERR;
        }
    } else if (inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS) {
        update_pcie_card_info_by_std_smbus(obj_handle, pm_chip_info);
    } else if (inf_protocol == PCIE_MGNT_PROTOCOL_SMBUS_REG) {
        update_pcie_card_info_by_smbus_reg(obj_handle, pm_chip_info);
    }
    return RET_OK;
}


gint32 pcie_card_update_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret = 0;
    guint8 inf_protocol = 0xff;
    guint8 *card_fault_state = NULL;
    PM_CHIP_INFO *pm_chip_info = NULL;

    if (user_data != NULL) {
        card_fault_state = (guint8 *)user_data;
    }
    repair_cpld_core(obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_PROTOCOL, &inf_protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get pcie card interface_spec failed!", __FUNCTION__);
        return RET_OK;
    }
    if (inf_protocol == 0xff) {
        return RET_OK;
    }

    ret = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get object %s binded data failed. The error code is %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return RET_OK;
    }

    ret = get_pcie_mgnt_init_state(pm_chip_info);
    if (ret == RET_ERR) {
        debug_log(DLOG_DEBUG, "%s: Binded data is NULL, ret = %d. ", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)pm_chip_info);
        return RET_OK;
    }

    ret = update_pcie_card_info_by_protocol(obj_handle, pm_chip_info, inf_protocol);
    if (ret != RET_OK) {
        pm_unref_binded_data((gpointer)pm_chip_info);
        return RET_OK;
    }

    
    if (card_fault_state != NULL) {
        
        
        // 更新卡的过温过流异常状态
        (*card_fault_state) = (*card_fault_state) | pm_chip_info->pcieslot_fault_state;
        
    }

    

    pm_unref_binded_data((gpointer)pm_chip_info);
    return RET_OK;
}

gint32 pcie_hotplug_status_foreach(OBJ_HANDLE handle, gpointer user_data)
{
    guint8 present_status = 0x00;
    PCIE_ACTION_PRIV_DATA_S *priv_data = NULL;
    gint32 result = 0;
    guint8 type = 0;

    
    result = dal_get_extern_value_byte(handle, PROPERTY_CARD_ACTION_DESTINATION, &present_status, DF_AUTO);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "Get CardAction property Destination for %s fail", dfl_get_object_name(handle));
        return RET_OK;
    }

    
    result = dfl_get_binded_data(handle, (gpointer *)&priv_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "Get private property for %s fail, result is %d", dfl_get_object_name(handle), result);
        return RET_OK;
    }

    result = dal_get_property_value_byte(handle, PROPERTY_CARD_ACTION_TYPE, &type);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %s propetry type failed, result is %d", dfl_get_object_name(handle), result);
        return RET_OK;
    }

    if (priv_data->present == PCIE_INVALID_PRESENT) {
        debug_log(DLOG_DEBUG, "PcieCard(%s) present status updated from %d to %d", dfl_get_object_name(handle),
            priv_data->present, present_status);

        
        if (pcie_card_get_vmd_state() == ENABLE && type != OCPCARD_CONNECTOR) {
            if (present_status == 1) {
                priv_data->present = VPD_FIRST_TIME_LOAD;
            } else {
                priv_data->present = 0;
            }
        } else {
            priv_data->present = present_status;
        }

        
    } else if (present_status != priv_data->present) {
        debug_log(DLOG_DEBUG, "PcieCard(%s) present status will switch from %d to %d", dfl_get_object_name(handle),
            priv_data->present, present_status);

        result = pcie_card_present_status_switch(handle, present_status, priv_data);
        if (result != RET_OK) {
            debug_log(DLOG_DEBUG, "PcieCard(%s) present status switch from %d to %d failed. ret=%d",
                dfl_get_object_name(handle), priv_data->present, present_status, result);
        }
    }

    
    if ((pcie_card_get_vmd_state() == ENABLE) && (type != OCPCARD_CONNECTOR) && (type != NVME_RAID_CONNECTOR)) {
        if (priv_data->present == 1 && VPD_UNLOAD == priv_data->vpd_load) {
            // 根据根据在位加载对应协议xml配置
            pcie_card_load_vpd_xml_via_protocol(handle, &priv_data->vpd_load);
        }
    }

    
    return RET_OK;
}

gint32 update_smm_pcie_info(OBJ_HANDLE obj_handle, guint16 *vendor_id, guint16 *device_id, guint16 *sub_vendor_id,
    guint16 *sub_device_id)
{
    OBJ_HANDLE anchor_handle = 0;
    OBJ_HANDLE connector_handle = 0;
    gint32 ret;
    NODE_PCIE_INFO_S *pcie_info = NULL;

    if (obj_handle == 0 || vendor_id == NULL || device_id == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_specific_object_position(obj_handle, CLASS_ANCHOR, &anchor_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get anchor handle for %s failed, ret is %d", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    ret = dal_get_anchor_related_connector(anchor_handle, &connector_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get connector_handle for anchor (%s) failed, ret is %d",
            dfl_get_object_name(anchor_handle), ret);
        return RET_ERR;
    }

    (void)dfl_get_binded_data(connector_handle, (void **)&pcie_info);
    if (pcie_info == NULL) {
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "update pcie info for %s", dfl_get_object_name(obj_handle));

    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_VID, pcie_info->vendor_id, DF_NONE);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_DID, pcie_info->device_id, DF_NONE);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_SUBVID, pcie_info->sub_vendor_id, DF_NONE);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIECARD_SUBDID, pcie_info->sub_device_id, DF_NONE);

    *vendor_id = pcie_info->vendor_id;
    *device_id = pcie_info->device_id;
    *sub_vendor_id = pcie_info->sub_vendor_id;
    *sub_device_id = pcie_info->sub_device_id;

    return RET_OK;
}

gint32 ipmi_mcu_set_fpga_golden_fw_restore(OBJ_HANDLE obj_handle)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    PM_CHIP_INFO *chip_info = NULL;
    gint32 ret = 0;

    ret = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj bund data failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }

    head_para.cmd_arg = 0;
    head_para.opcode = REGISTER_KEY_FPGA_SELFHEALING;
    head_para.data_object_index = 0;
    head_para.input_data_len = 0;

    ret = pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, head_para.input_data, head_para.input_data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set data to mcu failed, ret is %d", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)chip_info);
        return ret;
    }

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}

gint32 ipmi_mcu_get_fpga_golden_fw_restore_result(OBJ_HANDLE obj_handle, guint8 *result_data, guint32 result_data_len)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    PM_CHIP_INFO *chip_info = NULL;
    gint32 ret = 0;

    if (result_data == NULL || result_data_len == 0) {
        return RET_ERR;
    }

    
    ret = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (ret != RET_OK) {
        return ret;
    }

    head_para.cmd_arg = 0;
    head_para.opcode = REGISTER_KEY_FPGA_QUERY_SELFHEALING_STATUS;
    head_para.data_object_index = 0;
    head_para.input_data_len = 0;

    ret = pm_get_fixed_len_data_by_std_smbus(chip_info, (guint8 *)&head_para, result_data, result_data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get data from mcu failed, ret is %d", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)chip_info);
        return ret;
    }

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}


LOCAL gint32 upgread_mcu_get_binfile(const gchar *binfile, guchar **p_out_buf, gint32 *p_file_len)
{
    gint32 ret = RET_ERR;
    gchar *resolved_path = NULL;
    FILE *fp = NULL;
    guchar *buf = NULL;
    gint32 file_len = 0;

    resolved_path = realpath(binfile, NULL);
    if (resolved_path == NULL) {
        debug_printf("bin file path error\n");
        return RET_ERR;
    }

    
    fp = fopen(resolved_path, "rb");
    if (fp == NULL) {
        debug_printf("fail to open file");
        goto EXIT;
    }

    if (fseek(fp, 0L, SEEK_END) < 0) {
        goto EXIT;
    }

    file_len = ftell(fp);
    if (file_len <= 0) {
        goto EXIT;
    }

    if (fseek(fp, 0L, SEEK_SET) < 0) {
        goto EXIT;
    }

    buf = (guchar *)g_malloc(file_len);
    if (buf == NULL) {
        debug_printf("fail to malloc\n");
        goto EXIT;
    }

    if (fread(buf, 1, file_len, fp) != file_len) {
        g_free(buf);
        buf = NULL;
        debug_printf("fail to read file\n");
        goto EXIT;
    }

    *p_out_buf = buf;
    *p_file_len = file_len;
    ret = RET_OK;
    debug_printf("file len%d \n", file_len);

EXIT:
    if (fp != NULL) {
        (void)fclose(fp);
    }
    if (resolved_path != NULL) {
        g_free(resolved_path);
    }

    return ret;
}


gint32 upgrade_mcu_send_file(GSList *input_list)
{
    debug_printf("upgrade_mcu_debug");
    gint32 ret = 0;
    gchar slotid = 0;
    guchar soltid_get = 0;
    const gchar *binfile = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar *buf = NULL;
    gint32 file_len = 0;
    PM_CHIP_INFO *pm_chip_info = NULL;
    PM_CHIP_INFO chip_info;
    PROTOCOL_HEAD_PARA head_para;

    if ((input_list == NULL) || (g_slist_length(input_list) < 2)) {
        debug_printf("invalid arguments\n");
        return RET_ERR;
    }

    slotid = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    binfile = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    if (binfile == NULL) {
        debug_printf("invalid arguments\n");
        return RET_ERR;
    }

    if (upgread_mcu_get_binfile(binfile, &buf, &file_len) == RET_ERR) {
        return RET_ERR;
    }

    if (dfl_get_object_list(CLASS_PCIECARD_NAME, &obj_list) != DFL_OK) {
        g_free(buf);
        buf = NULL;
        debug_printf("No card to upgrade\n");
        return RET_OK;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &soltid_get);

        if (soltid_get == slotid) {
            debug_printf("slot%d, %s \n", slotid, dfl_get_object_name(obj_handle));

            ret = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
            if (ret != RET_OK || pm_chip_info == NULL) {
                debug_log(DLOG_ERROR,
                    "%s: invalid arguments: pm_chip_info is NULL or get %s binded data failed, ret: %d",
                    __FUNCTION__, dfl_get_object_name(obj_handle), ret);
                g_free(buf);
                buf = NULL;
                g_slist_free(obj_list);
                return RET_ERR;
            }

            g_mutex_lock(&pm_chip_info->rw_mutex);
            (void)memcpy_s(&chip_info, sizeof(PM_CHIP_INFO), pm_chip_info, sizeof(PM_CHIP_INFO));
            g_mutex_unlock(&pm_chip_info->rw_mutex);

            (void)memset_s(&head_para, sizeof(head_para), 0, sizeof(head_para));
            head_para.opcode = 0x18;
            head_para.data_object_index = 0;
            head_para.input_data_len = 0;

            
            debug_printf("Send firmware...\n");
            ret = pm_send_data_by_std_smbus(&chip_info, (guint8 *)&head_para, buf, file_len);
            if (ret != RET_OK) {
                debug_printf("Fail to Send firmware\n");
                g_free(buf);
                buf = NULL;
                g_slist_free(obj_list);
                pm_unref_binded_data((gpointer)pm_chip_info);
                return RET_ERR;
            }

            debug_printf("Send firmware complete\n");

            pm_unref_binded_data((gpointer)pm_chip_info);
            break;
        }
    }

    g_free(buf);
    buf = NULL;
    g_slist_free(obj_list);
    return RET_OK;
}

gint32 ipmi_mcu_reset_chip(OBJ_HANDLE obj_handle, guint8 device_num, guint8 oprate_type)
{
    gint32 ret = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    PM_CHIP_INFO *chip_info = NULL;
    guint8 chip_number = 0;
    guint8 num_index = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_CHIP_NUMBER, &chip_number);
    if (ret != DFL_OK || chip_number == 0 || chip_number == INVALID_DATA_BYTE) {
        debug_log(DLOG_ERROR, "%s : get property failed, ret = %d, chip_number = %d", __FUNCTION__, ret, chip_number);
        return RET_ERR;
    }

    
    ret = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get chip_info failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    if (device_num != 0) {
        if (device_num > chip_number) {
            pm_unref_binded_data((gpointer)chip_info);
            return RET_ERR;
        }

        head_para.data_object_index = device_num;
        head_para.cmd_arg = 0;
        if (oprate_type == MCU_ACTION_RESET_CHIP) {
            head_para.opcode = REGISTER_KEY_CHIP_RESET;
        }
        if (oprate_type == MCU_ACTION_RESET_MCU_GPIO) {
            head_para.opcode = REGISTER_KEY_PCIE_MCU_GPIO_RESET;
        }
        head_para.input_data_len = 0;

        ret =
            pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, head_para.input_data, head_para.input_data_len);
    } else {
        head_para.cmd_arg = 0;
        if (oprate_type == MCU_ACTION_RESET_CHIP) {
            head_para.opcode = REGISTER_KEY_CHIP_RESET;
        }
        if (oprate_type == MCU_ACTION_RESET_MCU_GPIO) {
            head_para.opcode = REGISTER_KEY_PCIE_MCU_GPIO_RESET;
        }
        head_para.input_data_len = 0;

        for (num_index = 1; num_index <= chip_number; num_index++) {
            head_para.data_object_index = num_index;
            ret += pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, head_para.input_data,
                head_para.input_data_len);
        }
    }

    pm_unref_binded_data((gpointer)chip_info);
    return ret;
}


gint32 ipmi_mcu_reset_mcu(OBJ_HANDLE obj_handle, guint8 device_num)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    PM_CHIP_INFO *chip_info = NULL;
    gint32 retval;

    if (device_num != 0) {
        return RET_ERR;
    }

    head_para.opcode = REGISTER_KEY_UPGRADE_CONTROL_CMD;
    head_para.input_data_len = 1;
    head_para.input_data[0] = MCU_RESET;

    
    retval = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (retval != RET_OK || chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid arguments: chip_info is NULL or get %s binded data failed,  ret: %d",
            __FUNCTION__, dfl_get_object_name(obj_handle), retval);
        return RET_ERR;
    }

    if (chip_info->reset_mcu_task_id == 0) {
        retval =
            pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, head_para.input_data, head_para.input_data_len);
        if (retval != DFL_OK) {
            chip_info->reset_mcu_state = RESET_MCU_FAILED;
            debug_log(DLOG_ERROR, "%s: send data failed, obj_name: %s, ret: %d", __FUNCTION__,
                dfl_get_object_name(obj_handle), retval);

            pm_unref_binded_data((gpointer)chip_info);
            return RET_ERR;
        }

        retval = vos_task_create(&(chip_info->reset_mcu_task_id), "reset mcu", (TASK_ENTRY)update_reset_mcu_state,
            chip_info, DEFAULT_PRIORITY);
        if (retval != RET_OK) {
            chip_info->reset_mcu_task_id = 0;
            chip_info->reset_mcu_state = RESET_MCU_FAILED;
            debug_log(DLOG_ERROR, "%s: create reset mcu task exist, obj_name: %s, ret: %d", __FUNCTION__,
                dfl_get_object_name(obj_handle), retval);
            pm_unref_binded_data((gpointer)chip_info);
            return RET_ERR;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: reset mcu task exist, obj_name: %s", __FUNCTION__, dfl_get_object_name(obj_handle));
        pm_unref_binded_data((gpointer)chip_info);
        return RET_ERR;
    }
    return RET_OK;
}

gint32 pcie_card_mcu_get_reset_result(OBJ_HANDLE obj_handle, guint8 device_num, guint8 chip_number, guint8 *result)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    PM_CHIP_INFO *chip_info = NULL;
    gint32 ret = 0;
    guint8 num_index = 0;

    if (result == NULL) {
        return RET_ERR;
    }

    ret = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get binded data fail, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    head_para.cmd_arg = 0;
    head_para.opcode = REGISTER_KEY_GET_CHIP_RESET_RESULT;
    head_para.input_data_len = 0;

    if (device_num != 0) {
        if (device_num > chip_number) {
            debug_log(DLOG_ERROR, "%s : device number out of range, device_num = %d, chip_number = %d", __FUNCTION__,
                device_num, chip_number);
            pm_unref_binded_data((gpointer)chip_info);
            return RET_ERR;
        }
        head_para.data_object_index = device_num;

        ret = pm_get_fixed_len_data_by_std_smbus(chip_info, (guint8 *)&head_para, &result[0], sizeof(guint8));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : get chip_info fail, ret = %d", __FUNCTION__, ret);
            pm_unref_binded_data((gpointer)chip_info);
            return ret;
        }
    } else {
        for (num_index = 1; num_index <= chip_number; num_index++) {
            head_para.data_object_index = num_index;
            ret += pm_get_fixed_len_data_by_std_smbus(chip_info, (guint8 *)&head_para, &result[num_index - 1],
                sizeof(guint8));
        }

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : get chip_info fail, ret = %d", __FUNCTION__, ret);
            pm_unref_binded_data((gpointer)chip_info);
            return ret;
        }
    }

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}

gint32 ipmi_mcu_get_reset_mcu_result(OBJ_HANDLE obj_handle, guint8 device_num, guint8 *result_data, guint32 *length,
    guint32 manufactureid)
{
    gint32 retval;
    PM_CHIP_INFO *chip_info = NULL;

    if (device_num != 0) {
        return RET_ERR;
    }

    *length = 5; // 获取mcu复位的响应长度是5
    result_data[0] = COMP_CODE_SUCCESS;
    result_data[1] = LONGB0(manufactureid);
    result_data[2] = LONGB1(manufactureid); // byte2 表示厂商的次高字节
    result_data[3] = LONGB2(manufactureid); // byte3 表示厂商的最高字节

    retval = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (retval != RET_OK || chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s:  invalid arguments: chip_info is NULL or get %s bind data failed, ret: %d",
            __FUNCTION__, dfl_get_object_name(obj_handle), retval);
        return RET_ERR;
    }

    result_data[4] = chip_info->reset_mcu_state; // byte4 表示重启MCU的状态

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}


LOCAL gint32 pcie_card_powre_on(OBJ_HANDLE obj, gpointer data)
{
    gint32 ret;
    gchar name_buf[MAX_NAME_SIZE] = {0};

    ret = dal_get_property_value_string(obj, PROPERTY_P_DEVICE_SENSOR_NAME, name_buf, sizeof(name_buf));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s obj failed.", CLASS_NAME_PERIPHERAL_DEVICE_SENSOR);
        return RET_OK;
    }

    if (g_strcmp0(name_buf, CARD0_PWR_ON_SENSOR_NAME) && g_strcmp0(name_buf, CARD1_PWR_ON_SENSOR_NAME)) {
        return RET_OK;
    }

    
    ret = dal_set_property_value_uint16(obj, PROPERTY_P_DEVICE_SENSOR_VALUE, 0x20, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set %s %s value failed, ret=%d.", name_buf, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR, ret);
    }

    return RET_OK;
}


void pciecard_power_on(void)
{
    gint32 ret;
    ret = dfl_foreach_object(CLASS_NAME_PERIPHERAL_DEVICE_SENSOR, pcie_card_powre_on, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Pcie card power on failed, ret = %d.", ret);
    }
}

LOCAL gint32 chip_reset_action(OBJ_HANDLE obj_resetchip, guint8 chip_id)
{
    gint32 ret = 0;
    // 0表示芯片复位
    ret = dal_set_property_value_byte(obj_resetchip, PROPERTY_CHIP_ACTION_DESTINATION, 0, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set ResetChipAction Accessor failed, ret %d", ret);
        return RET_ERR;
    }
    vos_task_delay(60); // 复位后延时60ms解复位
    // 1表示解复位
    ret = dal_set_property_value_byte(obj_resetchip, PROPERTY_CHIP_ACTION_DESTINATION, 1, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set ResetChipAction Accessor failed, ret %d", ret);
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "Reset chip%u successfully.", chip_id);
    return RET_OK;
}

LOCAL gint32 traverse_reset_chip(GSList *resetchip_list, guint8 device_num)
{
    guint8 chip_id = 0;
    OBJ_HANDLE obj_resetchip = 0;
    gint32 ret;
    GSList *node = NULL;

    for (node = resetchip_list; node != NULL; node = g_slist_next(node)) {
        obj_resetchip = (OBJ_HANDLE)node->data;
        ret = dal_get_property_value_byte(obj_resetchip, PROPERTY_CHIP_ACTION_RESETTRAGET, &chip_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get chipid failed,device_num %u, ret %d", device_num, ret);
            return RET_ERR;
        }
        if (device_num == 0) {
            ret = chip_reset_action(obj_resetchip, chip_id);
            if (ret != RET_OK) {
                return RET_ERR;
            }
        } else if (chip_id == device_num) {
            ret = chip_reset_action(obj_resetchip, chip_id);
            if (ret != RET_OK) {
                return RET_ERR;
            }
            break;
        }
    }
    
    return RET_OK;
}


LOCAL gint32 ipmi_reset_chip(OBJ_HANDLE obj_handle, guint8 device_num)
{
    gint32 ret;
    guint8 chip_number = 0;
    GSList *resetchip_list = NULL;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_CHIP_NUMBER, &chip_number);
    if (ret != DFL_OK || chip_number == 0 || chip_number == INVALID_DATA_BYTE) {
        debug_log(DLOG_ERROR, "get property chip_number failed, ret = %d, chip_number = %u", ret, chip_number);
        return RET_ERR;
    }

    if (device_num > chip_number) {
        debug_log(DLOG_ERROR, "device_num %u > chip_number %u ", device_num, chip_number);
        return RET_ERR;
    }
    ret = dal_get_object_list_position(obj_handle, CLASS_RESET_CHIP_ACTION, &resetchip_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s ResetChipAction object list failed, ret %d",
            dfl_get_object_name(obj_handle), ret);
        g_slist_free(resetchip_list);
        return RET_ERR;
    }

    ret = traverse_reset_chip(resetchip_list, device_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "write accessor failed, ret = %d, chip_number = %u", ret, chip_number);
        g_slist_free(resetchip_list);
        return RET_ERR;
    }
    g_slist_free(resetchip_list);
    return RET_OK;
}

gint32 ipmi_set_device_action(guint8 *resp_buf, gsize resp_buf_len, guint32 manufactureid,
    OBJ_HANDLE obj_handle, guint8 device_num)
{
    gint32 ret;
    // 若resp_buf_len小于4，数组会访问越界
    if (resp_buf_len < 4) {
        debug_log(DLOG_ERROR, "The response length is too short");
        return RET_ERR;
    }
    ret = ipmi_reset_chip(obj_handle, device_num);
    if (ret == RET_OK) {
        resp_buf[0] = COMP_CODE_SUCCESS;
        resp_buf[1] = LONGB0(manufactureid); // 1表示第二位响应0xdb
        resp_buf[2] = LONGB1(manufactureid); // 2表示第三位响应0x07
        resp_buf[3] = LONGB2(manufactureid); // 3表示第四位响应0x00
    }

    return ret;
}