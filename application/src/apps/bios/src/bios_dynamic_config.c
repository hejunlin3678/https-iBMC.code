
#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "bios.h"
#include <string.h>






#define DELAY_INTERVAL_TIME 100
#define DELAY_TIMES 10

#define HUAWEI_MANU_OFFSET_BYTE 8
#define HUAWEI_MANU_OFFSET_UINT16 16
#define HUAWEI_MANU_AND_OPER_VALUE 0xFF

#define BIOS_SMI_CMD_LENGTH_OFFSET 0
#define BIOS_SMI_CMD_DATA_OFFSET 4

#define BIOS_CFG_DATA_LEN_OFFSET 0x1
#define BIOS_CFG_DATA_OFFSET 0x2
#define BIOS_CFG_DATA_LEN 3

#define BIOS_CFG_ITEM_DATA_RESULT_OFFSET 0x0
#define BIOS_CFG_ITEM_DATA_VALUE_OFFSET 0x1

#define BIOS_CFG_INFO_SAME_FLAG 0x1
#define BIOS_CFG_RESULT_FLAG_LEN 0x1
#define BIOS_CFG_TOTAL_NUM 0x9
#define METHOD_SET_CONFIG_INPUT_LEN 0x2

#define BIOS_CFG_NUM_VALUE_OFFSET 0x7F
#define BIOS_CFG_SAME_BIT_OFFSET 0x7

#define BIOS_CFG_OUT_SIZE_TIMES 2

#define BIOS_CFG_INFO_MAX_LEN 1024

#define BIOS_CFG_CONDITION_VALID 0x0
#define BIOS_CFG_CONDITION_INVALID 0x1

#define BIOS_CFG_DEMT_ENABLED "Enabled"
#define BIOS_CFG_DEMT_DISABLED "Disabled"

#pragma pack(1)
typedef struct _tag_bios_conf_ipmi_query_req {
    guint8 huawei_manu_id[MANUFAC_ID_LEN];
    guint8 sub_cmd;
    guint8 reserved;
    guint16 offset;
    guint8 length;
} BIOS_CONFIG_IPMI_QUERY_REQ;

typedef struct _tag_bios_conf_ipmi_query_rsp {
    guint8 completion_code;
    guint8 huawei_manu_id[MANUFAC_ID_LEN];
#ifdef BD_BIG_ENDIAN
    guint8 reserved : 7;
    guint8 cascading : 1;
#else
    guint8 cascading : 1;
    guint8 reserved : 7;
#endif
    guint8 data[BIOS_CFG_ITEM_DATA_MAX_LEN];
} BIOS_CONFIG_IPMI_QUERY_RSP;
#define BIOS_CONFIG_IPMI_QUERY_RSP_HEADER_SIZE (sizeof(BIOS_CONFIG_IPMI_QUERY_RSP) - BIOS_CFG_ITEM_DATA_MAX_LEN)

typedef struct _tag_bios_conf_ipmi_set_req {
    guint8 huawei_manu_id[MANUFAC_ID_LEN];
    guint8 sub_cmd;
    guint8 reserved;
#ifdef BD_BIG_ENDIAN
    guint8 reserved2 : 7;
    guint8 cascading : 1;
#else
    guint8 cascading : 1;
    guint8 reserved2 : 7;
#endif
    guint16 offset;
    guint8 length;
    guint8 data[0];
} BIOS_CONFIG_IPMI_SET_REQ;
#define BIOS_CONFIG_SET_REQ_HEADER_SIZE sizeof(BIOS_CONFIG_IPMI_SET_REQ)

typedef struct _tag_bios_conf_ipmi_set_rsp {
    guint8 completion_code;
    guint8 huawei_manu_id[MANUFAC_ID_LEN];
} BIOS_CONFIG_IPMI_SET_RSP;
#define BIOS_CONFIG_IPMI_SET_RSP_SIZE sizeof(BIOS_CONFIG_IPMI_SET_RSP)
typedef struct _tag_bios_dynamic_cfg_item_info {
    guint8 cfg_item_number;
    gchar cfg_prop_name[BIOS_CFG_ITEM_DATA_MAX_LEN];
    gchar cfg_deso_name[BIOS_CFG_ITEM_DATA_MAX_LEN];
} BIOS_DYNAMIC_CFG_ITEM_INFO;
#pragma pack()

LOCAL guint8 *g_smi_command_data = NULL;
LOCAL pthread_mutex_t g_smi_command_mutex = PTHREAD_MUTEX_INITIALIZER;

// 此处配置项有新增时，需要添加相应的属性以及用于记录操作日志的描述信息
// 此处配置项id与与数组索引一致
/*lint -save -e651 -e64 初始化误告警屏蔽*/
BIOS_DYNAMIC_CFG_ITEM_INFO g_dynamic_cfg_item_info_tbl[] = {
    { BIOS_DYNAMIC_INVALID_ITEM,         BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_DEMT,                 PROPERTY_BIOS_DEMT_CONFIG_ITEM, BIOS_DEMT_DESP },
    { BIOS_DYNAMIC_UFS,                  BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_ENERGY_PERF_BIAS_CFG, BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_PL1_POWER_LIMIT,      BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_PL2_POWER_LIMIT,      BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_PL1_TIME_WINDOWS,     BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_PL2_TIME_WINDOWS,     BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL },
    { BIOS_DYNAMIC_LOADLINE_SWITCH,      BIOS_CFG_DESP_NULL,             BIOS_CFG_DESP_NULL }
}; /*lint -restore */
#define BIOS_CFG_ARRAY_SIZE (sizeof(g_dynamic_cfg_item_info_tbl) / sizeof(BIOS_DYNAMIC_CFG_ITEM_INFO))






LOCAL guint8 _bios_integrate_request_data(const BIOS_CONFIG_IPMI_SET_REQ *pReq, guint8 req_data_length,
    BIOS_CONFIG_IPMI_SET_REQ **integrated, guint8 **raw_data, guint32 *received_length);
LOCAL gint32 _bios_get_ipmi_rsp_data(const guint8 *data_buf, guint32 buf_len, guint8 *resp, guint16 offset,
    guint32 response_length);
LOCAL gint32 _bios_trigger_smi(void);
LOCAL gint32 _bios_trigger_smi_for_read_dynamic_config(const guchar *cmd_data, guint32 cmd_data_len);
LOCAL gint32 _bios_check_dynamic_conf_result(guint8 cfg_item_key, guint8 cfg_item_len, guint8 *cfg_item_data);
LOCAL gint32 _bios_get_cfg_result_output(const guint8 *input_data, guint32 input_data_len, guint8 *output_data,
    guint32 cfg_item_num, guint8 condition_flag);
LOCAL gint32 _bios_update_dynamic_cfg(OBJ_HANDLE obj_handle, guint8 *cfg_result, guint32 cfg_data_length);
LOCAL gint32 _bios_check_cfg_key_valid(guint8 cfg_item_key, gchar *cfg_prop_name, guint32 prop_name_size);
LOCAL void _bios_record_operation(GSList *caller_info, guint8 *cfg_result_array, guint32 cfg_result_len);
LOCAL void _bios_sync_demt_cfg(json_object *result_jso);
LOCAL gint32 _bios_read_cfg_from_current(gchar *fileshortname, json_object **out_data_json);
LOCAL void _bios_get_cfg_success_info(gchar *desp_array, guint32 desp_size, guint8 cfg_item_id);


LOCAL gint32 _bios_check_cfg_key_valid(guint8 cfg_item_key, gchar *cfg_prop_name, guint32 prop_name_size)
{
    guint8 iter_index = 0;
    guint8 iter_index_key = 0;
    guint32 cfg_prop_name_len = 0;

    if (cfg_prop_name == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return RET_ERR;
    }

    
    for (iter_index = 0; iter_index < BIOS_CFG_ARRAY_SIZE; iter_index++) {
        iter_index_key = g_dynamic_cfg_item_info_tbl[iter_index].cfg_item_number;
        if (cfg_item_key != iter_index_key) {
            continue;
        }
        if (g_strcmp0(g_dynamic_cfg_item_info_tbl[iter_index].cfg_prop_name, BIOS_CFG_DESP_NULL) == 0) {
            continue;
        }

        cfg_prop_name_len = strlen(g_dynamic_cfg_item_info_tbl[iter_index].cfg_prop_name);
        if (prop_name_size < cfg_prop_name_len) {
            debug_log(DLOG_ERROR, "%s: cfg key buff size ess than cfg prop name len", __FUNCTION__);
            return RET_ERR;
        }

        if (strncpy_s(cfg_prop_name, prop_name_size, g_dynamic_cfg_item_info_tbl[iter_index].cfg_prop_name,
            cfg_prop_name_len) != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s failed", __FUNCTION__);
            return RET_ERR;
        }

        return RET_OK;
    }

    return RET_ERR;
}


LOCAL guint8 _bios_integrate_request_data(const BIOS_CONFIG_IPMI_SET_REQ *pReq, guint8 req_data_length,
    BIOS_CONFIG_IPMI_SET_REQ **integrated, guint8 **raw_data, guint32 *received_length)
{
    errno_t safe_fun_ret = EOK;
    guint8 ipmi_cc = COMP_CODE_SUCCESS;
    guint8 *back_up = NULL;
    guint32 received_data_length = 0;

    if (pReq == NULL || integrated == NULL || raw_data == NULL || received_length == NULL) {
        return COMP_CODE_UNKNOWN;
    }

    
    if ((pReq->offset != 0 && *raw_data == NULL) ||
        (pReq->length + BIOS_CONFIG_SET_REQ_HEADER_SIZE) != req_data_length || pReq->length == 0) {
        ipmi_cc = COMP_CODE_INVALID_FIELD;
        goto quit;
    }

    if (pReq->offset == 0 && *raw_data != NULL) {
        g_free(*raw_data);
        *raw_data = NULL;
        *received_length = 0;
    }

    if (*raw_data == NULL) {
        *raw_data = (guint8 *)g_malloc0(sizeof(pReq->length));

        if (*raw_data != NULL) {
            safe_fun_ret = memcpy_s(*raw_data, pReq->length, pReq->data, pReq->length);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            ipmi_cc = COMP_CODE_MEMOUT;
            goto quit;
        }
    } else {
        // 数据连续判断
        if (pReq->offset != *received_length) {
            ipmi_cc = COMP_CODE_INVALID_FIELD;
            goto quit;
        }

        
        if ((pReq->offset + pReq->length) >= BIOS_CFG_INFO_MAX_LEN) {
            ipmi_cc = COMP_CODE_DATA_LEN_LARGE;
            goto quit;
        }

        // 重新分配内存进行增量存储
        back_up = *raw_data;
        *raw_data = (guint8 *)g_malloc0(pReq->offset + pReq->length);

        // 先拷贝之前接收的数据，然后拷贝当次接收的数据
        if (*raw_data != NULL) {
            safe_fun_ret = memcpy_s(*raw_data, pReq->offset + pReq->length, back_up, pReq->offset);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = memcpy_s(*raw_data + pReq->offset, pReq->length, pReq->data, pReq->length);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            g_free(back_up);
        } else {
            g_free(back_up);
            ipmi_cc = COMP_CODE_MEMOUT;
            goto quit;
        }
    }

    *received_length = pReq->offset + pReq->length;

    // 最后一帧数据
    if (pReq->cascading == 0) {
        received_data_length = BIOS_CONFIG_SET_REQ_HEADER_SIZE + *received_length;

        
        if (received_data_length >= BIOS_CFG_INFO_MAX_LEN) {
            ipmi_cc = COMP_CODE_DATA_LEN_LARGE;
            goto quit;
        }

        *integrated = (BIOS_CONFIG_IPMI_SET_REQ *)g_malloc0(received_data_length);
        if (*integrated == NULL) {
            ipmi_cc = COMP_CODE_MEMOUT;
            goto quit;
        }

        (void)memcpy_s(*integrated, BIOS_CONFIG_SET_REQ_HEADER_SIZE, pReq, BIOS_CONFIG_SET_REQ_HEADER_SIZE);
        safe_fun_ret = memcpy_s((*integrated)->data, *received_length, *raw_data, *received_length);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }

        // 拷贝完成，数据释放
        g_free(*raw_data);
        *raw_data = NULL;
    }

    return COMP_CODE_SUCCESS;

quit:

    if (*raw_data != NULL) {
        g_free(*raw_data);
        *raw_data = NULL;
    }

    if (*integrated != NULL) {
        g_free(*integrated);
        *integrated = NULL;
    }

    *received_length = 0;
    return ipmi_cc;
}


LOCAL gint32 _bios_get_ipmi_rsp_data(const guint8 *data_buf, guint32 buf_len, guint8 *resp, guint16 offset,
    guint32 response_length)
{
    BIOS_CONFIG_IPMI_QUERY_RSP *bios_cfg_rsp = NULL;
    errno_t safe_fun_ret;
    size_t resp_len;

    if (resp == NULL || data_buf == NULL) {
        return RET_ERR;
    }
    bios_cfg_rsp = (BIOS_CONFIG_IPMI_QUERY_RSP *)resp;

    if (offset < buf_len) {
        if (offset + response_length < buf_len) {
            bios_cfg_rsp->cascading = 1;
        } else {
            bios_cfg_rsp->cascading = 0;
        }

        resp_len = sizeof(bios_cfg_rsp->data);
        if (response_length != 0 && response_length <= resp_len) {
            safe_fun_ret = memcpy_s(bios_cfg_rsp->data, resp_len, data_buf + offset, response_length);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }
    } else {
        debug_log(DLOG_ERROR, "%s: offset(%d) exceed buf length(%d)", __FUNCTION__, offset, buf_len);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 _bios_trigger_smi(void)
{
    gint32 ret;
    OBJ_HANDLE bios_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get object handle for class [%s] failed", __FUNCTION__, CLASS_NAME_BIOS);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: begin to trigger smi", __FUNCTION__);

    
    ret = dal_set_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_INTERRUPT, TRUE, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value for Bios.%s failed, ret is %d", __FUNCTION__,
            PROPERTY_BIOS_SMI_INTERRUPT, ret);
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_INTERRUPT, FALSE, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value for Bios.%s failed, ret is %d", __FUNCTION__,
            PROPERTY_BIOS_SMI_INTERRUPT, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 _bios_trigger_smi_for_read_dynamic_config(const guchar *cmd_data, guint32 cmd_data_len)
{
    gint32 ret = 0;
    guchar smi_data_state = BIOS_SMI_DATA_EMPTY;
    OBJ_HANDLE bios_handle = 0;
    guint8 delay_times = DELAY_TIMES;

    debug_log(DLOG_INFO, "%s: begin to write smi command data", __FUNCTION__);

    if (cmd_data == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return RET_ERR;
    }

    (void)pthread_mutex_lock(&g_smi_command_mutex);

    
    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    (void)dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_COMMAND_DATA_STATE, &smi_data_state);
    if (smi_data_state == BIOS_SMI_DATA_READY) {
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
        debug_log(DLOG_ERROR, "%s failed: smi command data state invalid, current state is %d", __FUNCTION__,
            smi_data_state);
        return BIOS_ERROR_STATUS_UNSUPPORTED;
    }
    
    if ((cmd_data_len + BIOS_SMI_CMD_DATA_OFFSET) >= BIOS_FILE_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s:cmd data len invalid", __FUNCTION__);
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
        return RET_ERR;
    }
    g_smi_command_data = (guint8 *)g_malloc0(sizeof(guint8) * (cmd_data_len + BIOS_SMI_CMD_DATA_OFFSET));
    if (g_smi_command_data == NULL) {
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
        debug_log(DLOG_ERROR, "%s:g_malloc0 failed", __FUNCTION__);
        return RET_ERR;
    }

    errno_t securec_rv =
        memcpy_s(&g_smi_command_data[BIOS_SMI_CMD_LENGTH_OFFSET], sizeof(guint32), &cmd_data_len, sizeof(guint32));
    if (securec_rv != EOK) {
        g_free(g_smi_command_data);
        g_smi_command_data = NULL;
        debug_log(DLOG_ERROR, "memcpy_s fail, securec_rv = %d", securec_rv);
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
        return RET_ERR;
    }

    if (cmd_data_len > 0) {
        securec_rv = memcpy_s(g_smi_command_data + BIOS_SMI_CMD_DATA_OFFSET, cmd_data_len, cmd_data, cmd_data_len);
        if (securec_rv != EOK) {
            g_free(g_smi_command_data);
            g_smi_command_data = NULL;
            debug_log(DLOG_ERROR, "memcpy_s fail, securec_rv = %d", securec_rv);
            (void)pthread_mutex_unlock(&g_smi_command_mutex);
            return RET_ERR;
        }
    }

    (void)dal_set_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_COMMAND_DATA_STATE, BIOS_SMI_DATA_READY, DF_NONE);

    (void)pthread_mutex_unlock(&g_smi_command_mutex);

    
    ret = _bios_trigger_smi();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: trigger smi interrupt failed, ret is %d", __FUNCTION__, ret);
        goto out;
    }

    
    while (delay_times > 0) {
        vos_task_delay(DELAY_INTERVAL_TIME);
        delay_times--;
        (void)dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_COMMAND_DATA_STATE, &smi_data_state);

        if (smi_data_state == BIOS_REPORT_CONF_RESULT) {
            break;
        }
    }

    debug_log(DLOG_INFO, "%s: wait times is %d, smi data state is %d", __FUNCTION__, delay_times, smi_data_state);
    ret = (BIOS_REPORT_CONF_RESULT == smi_data_state) ? RET_OK : BIOS_ERROR_BIOS_STATUS_UNSUPPORTED;

out:

    // 退出前删除SMI中断触发的数据
    (void)pthread_mutex_lock(&g_smi_command_mutex);
    if (g_smi_command_data != NULL) {
        g_free(g_smi_command_data);
        g_smi_command_data = NULL;
        (void)dal_set_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_COMMAND_DATA_STATE, BIOS_SMI_DATA_EMPTY,
            DF_NONE);
    }
    (void)pthread_mutex_unlock(&g_smi_command_mutex);

    return ret;
}


LOCAL gint32 _bios_check_dynamic_conf_result(guint8 cfg_item_key, guint8 cfg_item_len, guint8 *cfg_item_data)
{
    gint32 ret = RET_OK;
    gchar cfg_prop_name[BIOS_CFG_ITEM_DATA_MAX_LEN] = { 0 };
    gchar cfg_item_type = 0;
    guint8 cfg_item_byte_value = 0;
    OBJ_HANDLE bios_handle = 0;

    if (cfg_item_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input para error", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = _bios_check_cfg_key_valid(cfg_item_key, cfg_prop_name, sizeof(cfg_prop_name));
    if (ret == RET_ERR || strlen(cfg_prop_name) == 0) {
        debug_log(DLOG_ERROR, "%s: cfg item key %d is not supported", __FUNCTION__, cfg_item_key);
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: cann't get object handle for class [%s], ret is %d", __FUNCTION__,
            CLASS_NAME_BIOS, ret);
        return RET_ERR;
    }

    
    (void)dal_get_property_type(bios_handle, cfg_prop_name, &cfg_item_type);

    switch (cfg_item_type) {
        case G_VARIANT_CLASS_BYTE:
            (void)dal_get_property_value_byte(bios_handle, cfg_prop_name, &cfg_item_byte_value);
            ret = (*cfg_item_data == cfg_item_byte_value) ? RET_OK : RET_ERR;
            break;

        default:
            ret = RET_ERR;
            break;
    }

    return ret;
}


LOCAL gint32 _bios_get_cfg_result_output(const guint8 *input_data, guint32 input_data_len, guint8 *output_data,
    guint32 cfg_item_num, guint8 condition_flag)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = RET_OK;
    guint8 cfg_item_index = 0;
    guint8 cfg_item_key = 0;
    guint8 cfg_item_length = 0;
    guint8 cfg_item_data[BIOS_CFG_ITEM_DATA_MAX_LEN] = { 0 };
    guint32 cfg_item_offset = 0;
    guint32 cfg_position = 0;

    if (input_data == NULL || output_data == NULL) {
        debug_log(DLOG_ERROR, "%s: pointer NULL", __FUNCTION__);
        return RET_ERR;
    }

    while (cfg_item_offset < input_data_len) {
        
        if (cfg_item_offset + BIOS_CFG_DATA_LEN_OFFSET >= input_data_len) {
            debug_log(DLOG_ERROR, "%s: cfg item index %d excceed cfg info len %d", __FUNCTION__, cfg_item_index,
                input_data_len);
            return RET_ERR;
        }

        if (cfg_item_index >= cfg_item_num) {
            debug_log(DLOG_ERROR, "%s: cfg item index %d, excceed cfg num %d", __FUNCTION__, cfg_item_index,
                cfg_item_num);
            return RET_ERR;
        }

        
        cfg_item_key = input_data[cfg_item_offset];
        cfg_item_length = input_data[cfg_item_offset + BIOS_CFG_DATA_LEN_OFFSET];

        cfg_position = cfg_item_offset + BIOS_CFG_DATA_LEN_OFFSET + cfg_item_length;
        if (cfg_position >= input_data_len) {
            debug_log(DLOG_ERROR, "%s: cfg item num 0x%x excceed array max size %d", __FUNCTION__, cfg_item_key,
                input_data_len);
            return RET_ERR;
        }

        
        if (cfg_item_length > 0) {
            (void)memset_s(cfg_item_data, sizeof(cfg_item_data), 0, sizeof(cfg_item_data));
            safe_fun_ret = memcpy_s(cfg_item_data, sizeof(cfg_item_data),
                &input_data[cfg_item_offset + BIOS_CFG_DATA_OFFSET], cfg_item_length);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }

            ret = _bios_check_dynamic_conf_result(cfg_item_key, cfg_item_length, cfg_item_data);
            output_data[cfg_item_index] = cfg_item_key;
            output_data[cfg_item_index + 1] = ((ret == RET_OK) && (condition_flag == BIOS_CFG_CONDITION_VALID)) ?
                BIOS_CFG_ITEM_SUCCESS :
                BIOS_CFG_ITEM_FAILED;
        }

        cfg_item_offset = cfg_position + 1;
        cfg_item_index++;
    }

    return RET_OK;
}


LOCAL void _bios_get_cfg_success_info(gchar *desp_array, guint32 desp_size, guint8 cfg_item_id)
{
    errno_t safe_fun_ret = EOK;
    guint8 demt_cfg_value = 0;
    OBJ_HANDLE bios_handle = 0;
    gint32 ret = RET_OK;
    gchar *cfg_desp = NULL;

    if (desp_array == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: cann't get object handle for class [%s], ret is %d", __FUNCTION__,
            CLASS_NAME_BIOS, ret);
        return;
    }

    // 此处后期配置项扩展需要新增分支处理
    switch (cfg_item_id) {
        case BIOS_DYNAMIC_DEMT:
            (void)dal_get_property_value_byte(bios_handle, g_dynamic_cfg_item_info_tbl[cfg_item_id].cfg_prop_name,
                &demt_cfg_value);
            cfg_desp = (demt_cfg_value == BIOS_DEMT_CFG_DISABLED) ? BIOS_CFG_DEMT_DISABLED : BIOS_CFG_DEMT_ENABLED;
            safe_fun_ret = strncat_s(desp_array, desp_size, " to ", strlen(" to "));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            safe_fun_ret = strncat_s(desp_array, desp_size, cfg_desp, strlen(cfg_desp));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            break;

        default:
            break;
    }

    return;
}


LOCAL void _bios_record_operation(GSList *caller_info, guint8 *cfg_result_array, guint32 cfg_result_len)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret = EOK;
    gchar log_success_string[MAX_LOG_ARR_LEN] = { 0 }; // 此处日志长度暂定512，可根据配置项优化
    gchar log_failed_string[MAX_LOG_ARR_LEN] = { 0 };
    guint32 cfg_iter_index = 0;
    guint8 cfg_item_result = 0;
    guint8 cfg_item_id = 0;
    gchar tmp_desp[MAX_LOG_ARR_LEN] = { 0 };
    guint8 cfg_success_num = 0;
    guint8 cfg_failed_num = 0;

    (void)snprintf_s(log_success_string, sizeof(log_success_string), sizeof(log_success_string) - 1,
        "Config BIOS dynamic energy saving parameters (");
    (void)snprintf_s(log_failed_string, sizeof(log_failed_string), sizeof(log_failed_string) - 1,
        "Config BIOS dynamic energy saving parameters (");

    for (cfg_iter_index = 0; cfg_iter_index < cfg_result_len; cfg_iter_index += BIOS_INDEX_INTERVAL_LEN) {
        cfg_item_id = cfg_result_array[cfg_iter_index];
        cfg_item_result = cfg_result_array[cfg_iter_index + 1];

        (void)memset_s(tmp_desp, sizeof(tmp_desp), 0, sizeof(tmp_desp));
        iRet = snprintf_s(tmp_desp, sizeof(tmp_desp), sizeof(tmp_desp) - 1, "%s",
            g_dynamic_cfg_item_info_tbl[cfg_item_id].cfg_deso_name);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }

        if (cfg_item_result == BIOS_CFG_ITEM_SUCCESS) {
            _bios_get_cfg_success_info(tmp_desp, sizeof(tmp_desp), cfg_item_id);
        }

        if (cfg_iter_index == cfg_result_len - BIOS_INDEX_INTERVAL_LEN) {
            safe_fun_ret = strncat_s(tmp_desp, sizeof(tmp_desp), ") ", strlen(") "));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            safe_fun_ret = strncat_s(tmp_desp, sizeof(tmp_desp), ", ", strlen(", "));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }

        if (cfg_item_result == BIOS_CFG_ITEM_FAILED) {
            safe_fun_ret = strncat_s(log_failed_string, sizeof(log_failed_string), tmp_desp, strlen(tmp_desp));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            cfg_failed_num++;
        } else {
            safe_fun_ret = strncat_s(log_success_string, sizeof(log_success_string), tmp_desp, strlen(tmp_desp));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            cfg_success_num++;
        }
    }

    safe_fun_ret = strncat_s(log_failed_string, sizeof(log_failed_string), "failed", strlen("failed"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = strncat_s(log_success_string, sizeof(log_success_string), "successfully", strlen("successfully"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    if (cfg_failed_num > 0) {
        method_operation_log(caller_info, NULL, "%s", log_failed_string);
    }

    if (cfg_success_num > 0) {
        method_operation_log(caller_info, NULL, "%s", log_success_string);
    }

    return;
}


LOCAL gint32 _bios_update_dynamic_cfg(OBJ_HANDLE obj_handle, guint8 *cfg_result, guint32 cfg_data_length)
{
    errno_t safe_fun_ret = EOK;
    guint8 cfg_item_index = 0;
    guint8 cfg_item_key = 0;
    guint8 cfg_item_length = 0;
    guint8 cfg_item_data[BIOS_CFG_ITEM_DATA_MAX_LEN] = { 0 };
    guint8 cfg_item_offset = 0;
    guint8 cfg_position = 0;
    guint8 cfg_item_result = 0;
    gchar cfg_item_type = 0;

    gchar cfg_prop_name[BIOS_CFG_ITEM_DATA_MAX_LEN] = { 0 };
    gint32 ret_val = RET_ERR;

    if (obj_handle == 0 || cfg_result == NULL || cfg_data_length <= 0 || cfg_data_length > UINT8_MAX) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return RET_ERR;
    }

    while (cfg_item_offset < cfg_data_length) {
        
        if (cfg_item_offset + BIOS_CFG_DATA_LEN_OFFSET >= cfg_data_length) {
            debug_log(DLOG_ERROR, "%s: cfg item index %d excceed max size %d", __FUNCTION__, cfg_item_index,
                cfg_data_length);
            return RET_ERR;
        }

        cfg_item_key = cfg_result[cfg_item_offset];
        cfg_item_length = cfg_result[cfg_item_offset + BIOS_CFG_DATA_LEN_OFFSET];

        cfg_position = cfg_item_offset + BIOS_CFG_DATA_LEN_OFFSET + BIOS_CFG_RESULT_FLAG_LEN + cfg_item_length;
        if (cfg_position >= cfg_data_length) {
            debug_log(DLOG_ERROR, "%s: cfg item num(0x%x) excceed cfg info length %d", __FUNCTION__, cfg_item_key,
                cfg_data_length);
            return RET_ERR;
        }

        
        ret_val = _bios_check_cfg_key_valid(cfg_item_key, cfg_prop_name, sizeof(cfg_prop_name));
        if ((ret_val != RET_OK) || (strlen(cfg_prop_name) == 0)) {
            debug_log(DLOG_DEBUG, "%s: bios dynamic cfg key %d not supported", __FUNCTION__, cfg_item_key);
            goto next;
        }

        if (cfg_item_length > 0) {
            (void)memset_s(cfg_item_data, sizeof(cfg_item_data), 0, sizeof(cfg_item_data));
            safe_fun_ret = memcpy_s(cfg_item_data, sizeof(cfg_item_data),
                &cfg_result[cfg_item_offset + BIOS_CFG_DATA_OFFSET], cfg_item_length + BIOS_CFG_RESULT_FLAG_LEN);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                return RET_ERR;
            }
            cfg_item_result = cfg_item_data[BIOS_CFG_ITEM_DATA_RESULT_OFFSET];

            if (cfg_item_result == BIOS_CFG_ITEM_SUCCESS) {
                
                (void)dal_get_property_type(obj_handle, cfg_prop_name, &cfg_item_type);

                switch (cfg_item_type) {
                    case G_VARIANT_CLASS_BYTE:
                        ret_val = dal_set_property_value_byte(obj_handle, cfg_prop_name,
                            cfg_item_data[BIOS_CFG_ITEM_DATA_VALUE_OFFSET], DF_SAVE_TEMPORARY);
                        if (ret_val != RET_OK) {
                            debug_log(DLOG_ERROR, "%s: update bios cfg item %s value %d failed, ret is %d",
                                __FUNCTION__, cfg_prop_name, cfg_item_data[BIOS_CFG_ITEM_DATA_VALUE_OFFSET], ret_val);
                        }
                        break;

                    default:
                        break;
                }
            }
        }

    next:
        cfg_item_offset = cfg_position + 1;
        cfg_item_index++;
    }

    return RET_OK;
}


LOCAL gint32 _bios_read_cfg_from_current(gchar *fileshortname, json_object **out_data_json)
{
    FM_FILE_S *file_handle = NULL;
    gchar *data_buf = NULL;
    gint32 fm_fseek_back = 1;
    gint32 file_size = 0;
    guint32 file_actual_size = 0;

    if (fileshortname == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter error", __FUNCTION__);
        return RET_ERR;
    }

    
    file_handle = fm_fopen(fileshortname, "r+b");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    file_size = fm_ftell(file_handle);
    if (file_size < 0) {
        debug_log(DLOG_ERROR, "%s: file tell %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    
    data_buf = (gchar *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 len:%d fail", __FUNCTION__, file_size + 1);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, fileshortname);
        goto err_out;
    }

    file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
    if (file_size != file_actual_size) {
        debug_log(DLOG_ERROR, "%s: read file(InLen:%d,OutLen:%d) failed", __FUNCTION__, file_size, file_actual_size);
        goto err_out;
    }

    
    *out_data_json = json_tokener_parse(data_buf);
    if (*out_data_json == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        goto err_out;
    }

    
    fm_fclose(file_handle);
    g_free(data_buf);

    return RET_OK;

err_out:
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }

    return RET_ERR;
}


LOCAL void _bios_sync_demt_cfg(json_object *result_jso)
{
    json_object *demt_jso = NULL;
    gint32 ret = RET_OK;
    guint8 demt_cfg_new_value = 0;
    OBJ_HANDLE bios_handle = 0;
    guint8 demt_cfg_old_value = 0;
    const gchar *demt_cfg_cur_desp = NULL;

    json_bool jso_ret = FALSE;

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle for %s fail", __FUNCTION__, BIOS_CLASS_NAME);
        return;
    }

    
    jso_ret = json_object_object_get_ex(result_jso, BIOS_JSON_DEMT, &demt_jso);
    if (jso_ret != TRUE) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_object_get_ex fail", __FUNCTION__, __LINE__);
        return;
    }

    demt_cfg_cur_desp = dal_json_object_get_str(demt_jso);
    if (demt_cfg_cur_desp == NULL) {
        debug_log(DLOG_DEBUG, "%s: demt desp from currentvalue.json invalid", __FUNCTION__);
        return;
    }

    if (g_strcmp0(BIOS_DEMT_DISABLED, demt_cfg_cur_desp) == 0) {
        demt_cfg_new_value = BIOS_DEMT_CFG_DISABLED;
    } else if (g_strcmp0(BIOS_DEMT_ENABLED, demt_cfg_cur_desp) == 0) {
        demt_cfg_new_value = BIOS_DEMT_CFG_ENABLED;
    }

    
    ret = dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_DEMT_CONFIG_ITEM, &demt_cfg_old_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get demt prop failed, ret is %d", __FUNCTION__, ret);
        return;
    }

    if (demt_cfg_new_value != demt_cfg_old_value) {
        ret = dal_set_property_value_byte(bios_handle, PROPERTY_BIOS_DEMT_CONFIG_ITEM, demt_cfg_new_value,
            DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get demt prop failed, ret is %d", __FUNCTION__, ret);
            return;
        }
    }

    debug_log(DLOG_INFO, "%s: bios sync demt cfg from currentvalue.json succeed, old value is %d, new value is %d",
        __FUNCTION__, demt_cfg_old_value, demt_cfg_new_value);

    return;
}


void bios_dynamic_config_sync(void)
{
    gint32 ret = 0;
    json_object *result_jso = NULL;

    
    ret = _bios_read_cfg_from_current(BIOS_FILE_CURRENT_VALUE_NAME, &result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: read demt cfg from currentvalue.json failed, ret is %d", __FUNCTION__, ret);
        return;
    }

    
    (void)_bios_sync_demt_cfg(result_jso);

    if (result_jso != NULL) {
        json_object_put(result_jso);
    }

    return;
}


gint32 bios_set_dynamic_configuration(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guint8 smbios_status = SMBIOS_WRITE_NOT_START;
    const guint8 *input_data = NULL;
    gsize input_data_size = 0;
    guint8 bios_cfg_item_num = 0;
    guint8 input_list_len = 0;
    guint8 *bios_cfg_result = NULL;
    guint8 energy_saving_mode = 0;
    OBJ_HANDLE smbios_handle = 0;
    guint8 bios_cfg_result_size = 0;
    gint32 method_ret = RET_OK;
    guint8 condition_flag = BIOS_CFG_CONDITION_INVALID; 
    guint8 energy_saving_support = 0;
    guint8 same_cfg_set_flag = 0;
    guint8 cfg_total_num = 0;

    
    if (input_list == NULL || output_list == NULL) {
        method_ret = BIOS_ERROR_INVALID_INPUT_PARA;
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        goto error_out1;
    }

    input_list_len = g_slist_length(input_list);
    if (input_list_len != METHOD_SET_CONFIG_INPUT_LEN) {
        method_ret = BIOS_ERROR_INVALID_INPUT_PARA;
        debug_log(DLOG_ERROR, "%s:input_list len %d is invalid", __FUNCTION__, input_list_len);
        goto error_out1;
    }

    bios_cfg_item_num = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 0)));
    input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 1)),
        &input_data_size, sizeof(guint8));

    
    cfg_total_num = bios_cfg_item_num & BIOS_CFG_NUM_VALUE_OFFSET; // 低7位取配置项个数
    
    same_cfg_set_flag = (bios_cfg_item_num & BIOS_CFG_ITEM_SAME_FLAG_OFFSET) >> BIOS_CFG_SAME_BIT_OFFSET;

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        &energy_saving_support);
    if (energy_saving_support != 1) {
        method_ret = BIOS_ERROR_LICENSE_NOT_INSTALLED;
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__);
        goto out;
    }

    
    (void)dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &smbios_handle);
    (void)dal_get_property_value_byte(smbios_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_status);
    if (smbios_status != SMBIOS_WRITE_FINISH) {
        method_ret = BIOS_ERROR_BIOS_STATUS_UNSUPPORTED;
        debug_log(DLOG_ERROR, "%s: bios status is not supported, status is %d", __FUNCTION__, smbios_status);
        goto out;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_ENERGY_MODE_ENABLE, &energy_saving_mode);
    if (ret != RET_OK) {
        method_ret = BIOS_ERROR_MATHOD_OPERATION;
        debug_log(DLOG_ERROR, "%s: get bios energy saving mode failed, ret is %d", __FUNCTION__, ret);
        goto out;
    }
    if (energy_saving_mode != BIOS_ENERGY_MODE_ENABLED) {
        method_ret = BIOS_ERROR_ENERGY_SAVING_DISABLED;
        debug_log(DLOG_ERROR, "%s: energy saving mode %d not supported", __FUNCTION__, energy_saving_mode);
        goto out;
    }

    
    if (same_cfg_set_flag == BIOS_CFG_INFO_SAME_FLAG) {
        condition_flag = BIOS_CFG_CONDITION_VALID;
        debug_log(DLOG_DEBUG, "%s: all cfg parameters is the same as saved cfg parameters", __FUNCTION__);
        goto out;
    }

    
    ret = _bios_trigger_smi_for_read_dynamic_config(input_data, input_data_size);
    if (ret != RET_OK) {
        method_ret = ret;
        debug_log(DLOG_ERROR, "%s: trigger smi for read dynamic config failed, ret is %d", __FUNCTION__, ret);
        goto out;
    }

    
    condition_flag = BIOS_CFG_CONDITION_VALID;

    
out:
    debug_log(DLOG_DEBUG, "%s: cfg item total num is %d, same cfg flag is %d", __FUNCTION__, cfg_total_num,
        same_cfg_set_flag);
    bios_cfg_result_size = BIOS_CFG_OUT_SIZE_TIMES * cfg_total_num;
    bios_cfg_result = (guint8 *)g_malloc0(sizeof(guint8) * bios_cfg_result_size);
    if (bios_cfg_result == NULL) {
        method_ret = BIOS_ERROR_MATHOD_OPERATION;
        debug_log(DLOG_ERROR, "%s:g_malloc0 failed", __FUNCTION__);
        goto error_out1;
    }

    ret = _bios_get_cfg_result_output(input_data, input_data_size, bios_cfg_result, (guint32)cfg_total_num,
        condition_flag);
    if (ret != RET_OK) {
        g_free(bios_cfg_result);
        debug_log(DLOG_ERROR, "%s: call _bios_get_config_output failed, ret is %d", __FUNCTION__, ret);
        goto error_out1;
    }

    *output_list = g_slist_append(*output_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
        bios_cfg_result, bios_cfg_result_size, sizeof(guint8)));

    _bios_record_operation(caller_info, bios_cfg_result, bios_cfg_result_size);

    g_free(bios_cfg_result);

    return method_ret;

error_out1:
    method_operation_log(caller_info, NULL, "Config BIOS dynamic energy saving parameters failed");
    return method_ret;
}

gint32 bios_set_demt_state(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    gint32 method_ret;
    guint8 smbios_status = SMBIOS_WRITE_NOT_START;
    guint8 input_data[BIOS_CFG_DATA_LEN] = {0};
    guint8 demt_cfg_value;
    guint8 energy_saving_support = 0;
    OBJ_HANDLE smbios_handle = 0;

    
    if (input_list == NULL) {
        method_ret = BIOS_ERROR_INVALID_INPUT_PARA;
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        goto ERR_EXIT;
    }

    demt_cfg_value = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 0)));
    if ((demt_cfg_value != BIOS_DEMT_CFG_DISABLED && demt_cfg_value != BIOS_DEMT_CFG_ENABLED)) {
        method_ret = BIOS_ERROR_INVALID_INPUT_PARA;
        debug_log(DLOG_ERROR, "%s:param err", __FUNCTION__);
        goto ERR_EXIT;
    }

    
    input_data[0] = BIOS_DYNAMIC_DEMT;
    input_data[BIOS_CFG_DATA_LEN_OFFSET] = (guint8)sizeof(guint8);
    input_data[BIOS_CFG_DATA_OFFSET] = demt_cfg_value;

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        &energy_saving_support);
    if (energy_saving_support != 1) {
        method_ret = BIOS_ERROR_LICENSE_NOT_INSTALLED;
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__);
        goto ERR_EXIT;
    }

    
    (void)dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &smbios_handle);
    (void)dal_get_property_value_byte(smbios_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_status);
    if (smbios_status != SMBIOS_WRITE_FINISH) {
        method_ret = BIOS_ERROR_BIOS_STATUS_UNSUPPORTED;
        debug_log(DLOG_ERROR, "%s: bios status is not supported, status is %d", __FUNCTION__, smbios_status);
        goto ERR_EXIT;
    }

    
    ret = _bios_trigger_smi_for_read_dynamic_config(input_data, sizeof(input_data));
    if (ret != RET_OK) {
        method_ret = ret;
        debug_log(DLOG_ERROR, "%s: trigger smi for read dynamic config failed, ret is %d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    
    ret = _bios_check_dynamic_conf_result(BIOS_DYNAMIC_DEMT, 1, &demt_cfg_value);
    if (ret != RET_OK) {
        method_ret = ret;
        debug_log(DLOG_ERROR, "%s: check dynamic config result failed, ret is %d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    method_operation_log(caller_info, NULL, "Set BIOS DEMT status (%s) successfully",
        (demt_cfg_value == BIOS_DEMT_CFG_DISABLED) ? BIOS_CFG_DEMT_DISABLED : BIOS_CFG_DEMT_ENABLED);

    return RET_OK;

ERR_EXIT:
    method_operation_log(caller_info, NULL, "Failed to set the BIOS DEMT status");
    return method_ret;
}


gint32 bios_set_energy_mode_enable(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = RET_OK;
    guint8 energy_mode_val = 0;
    gchar *energy_mode_desp[] = { "Disable", "Enable" };
    guint8 energy_saving_support = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Failed to set the intelligent energy saving mode");
        return RET_ERR;
    }

    energy_mode_val = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (energy_mode_val >= BIOS_ENERGY_MODE_INVALID) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Failed to set the intelligent energy saving mode");
        return RET_ERR;
    }

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        &energy_saving_support);
    if (energy_saving_support != 1) {
        method_operation_log(caller_info, NULL, "%s intelligent energy saving failed",
            energy_mode_desp[energy_mode_val]);
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__);
        return BIOS_ERROR_LICENSE_NOT_INSTALLED;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_ENERGY_MODE_ENABLE, energy_mode_val, DF_SAVE_TEMPORARY);
    if (ret == RET_OK) {
        method_operation_log(caller_info, NULL, "%s intelligent energy saving successfully",
            energy_mode_desp[energy_mode_val]);
    } else {
        debug_log(DLOG_ERROR, "%s:call dal_set_property_value_byte set bios energy mode failed, ret is %d",
            __FUNCTION__, ret);
        method_operation_log(caller_info, NULL, "%s intelligent energy saving failed",
            energy_mode_desp[energy_mode_val]);
    }

    return ret;
}


gint32 bios_set_dynamic_config_result(const void *msg_data, gpointer user_data)
{
    gint32 ret_val = RET_OK;
    OBJ_HANDLE bios_handle = 0;
    const guchar *req_data = NULL;
    const BIOS_CONFIG_IPMI_SET_REQ *bios_cfg_req = NULL;
    BIOS_CONFIG_IPMI_SET_REQ *bios_cfg_result_integrated = NULL;
    guint8 ipmi_code = COMP_CODE_SUCCESS;
    BIOS_CONFIG_IPMI_SET_RSP bios_cfg_rsp;

    guint8 req_data_len = 0;
    static guint8 *cfg_result_data = NULL;
    static guint32 received_length = 0;

    
    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "%s: ipmi request data is null", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get ipmi request data failed", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    req_data_len = get_ipmi_src_data_len(msg_data);
    bios_cfg_req = (const BIOS_CONFIG_IPMI_SET_REQ *)req_data;

    // 厂商ID判断
    if (dal_check_ipmi_req_huawei_iana(bios_cfg_req->huawei_manu_id, MANUFAC_ID_LEN) != TRUE) {
        debug_log(DLOG_ERROR, "%s: manufacture id match fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ret_val = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cann't get object handle for class [%s]", __FUNCTION__, CLASS_NAME_BIOS);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    (void)memset_s(&bios_cfg_rsp, sizeof(BIOS_CONFIG_IPMI_SET_RSP), 0, sizeof(BIOS_CONFIG_IPMI_SET_RSP));

    ipmi_code = _bios_integrate_request_data(bios_cfg_req, req_data_len, &bios_cfg_result_integrated, &cfg_result_data,
        &received_length);

    if (bios_cfg_result_integrated != NULL) {
        // 解析处理BIOS上报的结果，更新BMC配置结果
        ret_val = _bios_update_dynamic_cfg(bios_handle, bios_cfg_result_integrated->data, received_length);
        if (ret_val != RET_OK) {
            ipmi_code = COMP_CODE_INVALID_FIELD;
        }

        received_length = 0;
        g_free(bios_cfg_result_integrated);

        
        (void)pthread_mutex_lock(&g_smi_command_mutex);
        (void)dal_set_property_value_byte(bios_handle, PROPERTY_BIOS_SMI_COMMAND_DATA_STATE, BIOS_REPORT_CONF_RESULT,
            DF_NONE);
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
    }

    bios_cfg_rsp.completion_code = ipmi_code;
    bios_cfg_rsp.huawei_manu_id[0] = OEM_MANUFACTURE_ID & HUAWEI_MANU_AND_OPER_VALUE;
    bios_cfg_rsp.huawei_manu_id[1] = (OEM_MANUFACTURE_ID >> HUAWEI_MANU_OFFSET_BYTE) & HUAWEI_MANU_AND_OPER_VALUE;
    bios_cfg_rsp.huawei_manu_id[2] = (OEM_MANUFACTURE_ID >> HUAWEI_MANU_OFFSET_UINT16) & HUAWEI_MANU_AND_OPER_VALUE;

    return ipmi_send_response(msg_data, BIOS_CONFIG_IPMI_SET_RSP_SIZE, (guint8 *)&bios_cfg_rsp);
}


gint32 bios_get_dynamic_configuration(const void *msg_data, gpointer user_data)
{
    BIOS_CONFIG_IPMI_QUERY_REQ bios_cfg_req;
    BIOS_CONFIG_IPMI_QUERY_RSP bios_cfg_rsp;
    guint32 cfg_data_len = 0;

    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input param pointer NULL", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    const guchar *req_data = get_ipmi_src_data(msg_data);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "%s: get ipmi request data failed", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 req_data_len = get_ipmi_src_data_len(msg_data);

    (void)memset_s(&bios_cfg_rsp, sizeof(BIOS_CONFIG_IPMI_QUERY_RSP), 0, sizeof(BIOS_CONFIG_IPMI_QUERY_RSP));
    (void)memset_s(&bios_cfg_req, sizeof(BIOS_CONFIG_IPMI_QUERY_REQ), 0, sizeof(BIOS_CONFIG_IPMI_QUERY_REQ));
    errno_t securec_rv = memcpy_s(&bios_cfg_req, sizeof(BIOS_CONFIG_IPMI_QUERY_REQ), req_data, req_data_len);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, securec_rv);
    }

    // 厂商ID判断
    if (dal_check_ipmi_req_huawei_iana(bios_cfg_req.huawei_manu_id, MANUFAC_ID_LEN) != TRUE) {
        debug_log(DLOG_ERROR, "%s: manufacture id match fail", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    (void)pthread_mutex_lock(&g_smi_command_mutex);
    if (g_smi_command_data == NULL) {
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
        debug_log(DLOG_ERROR, "%s: g_smi_command_data is NULL", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    securec_rv =
        memcpy_s(&cfg_data_len, sizeof(guint32), &g_smi_command_data[BIOS_SMI_CMD_LENGTH_OFFSET], sizeof(guint32));
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s fail, securec_rv = %d", securec_rv);
    }

    // offset有效性判断
    if (bios_cfg_req.offset >= cfg_data_len) {
        (void)pthread_mutex_unlock(&g_smi_command_mutex);
        debug_log(DLOG_ERROR, "%s: offset(%d) exceed revoke code length(%d)", __FUNCTION__, bios_cfg_req.offset,
            cfg_data_len);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint32 response_length = MIN(bios_cfg_req.length, cfg_data_len - bios_cfg_req.offset);
    /*lint -save -e648 内部数据不会截断 */
    response_length =
        MIN(response_length, IPMI_MSG_MAX_PAYLOAD_LEN - 1 - BIOS_CONFIG_IPMI_QUERY_RSP_HEADER_SIZE); /*lint -restore */

    // 根据偏移和长度读取数据
    gint32 ret_val = _bios_get_ipmi_rsp_data(&g_smi_command_data[BIOS_SMI_CMD_DATA_OFFSET], cfg_data_len,
        (guint8 *)&bios_cfg_rsp, bios_cfg_req.offset, response_length);
    (void)pthread_mutex_unlock(&g_smi_command_mutex);

    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get smi data failed, ret is %d", __FUNCTION__, ret_val);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
    }

    bios_cfg_rsp.completion_code = COMP_CODE_SUCCESS;
    bios_cfg_rsp.huawei_manu_id[0] = OEM_MANUFACTURE_ID & HUAWEI_MANU_AND_OPER_VALUE;
    bios_cfg_rsp.huawei_manu_id[1] = (OEM_MANUFACTURE_ID >> HUAWEI_MANU_OFFSET_BYTE) & HUAWEI_MANU_AND_OPER_VALUE;
    bios_cfg_rsp.huawei_manu_id[2] = (OEM_MANUFACTURE_ID >> HUAWEI_MANU_OFFSET_UINT16) & HUAWEI_MANU_AND_OPER_VALUE;

    if (G_MAXUINT8 - BIOS_CONFIG_IPMI_QUERY_RSP_HEADER_SIZE < response_length) {
        debug_log(DLOG_ERROR, "%s: exceeded array length", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    return ipmi_send_response(msg_data, BIOS_CONFIG_IPMI_QUERY_RSP_HEADER_SIZE + response_length,
        (guint8 *)&bios_cfg_rsp);
}