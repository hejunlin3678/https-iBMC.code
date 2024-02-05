

#include "component/base.h"
#include "common.h"

#define DEVICE_NAME_SPLIT_FLAG ("$")
#define DEVICE_NAME_ADD_LOCATION_FLAG ("#")
#define COM_PS_REDUNDANCY_DEVICE_NUMBER 0xfe

typedef struct tag_object_descp_s {
    OBJ_HANDLE object_handle; 
} OBJECT_DESCP_S;

LOCAL SEMID g_connects_hot = 0;

GSList *g_connector_list = NULL;

LOCAL GAsyncQueue *g_component_obj_add_msg_queue = NULL;

LOCAL TASKID g_frudata_new_obj_add_task_id = 0;

LOCAL guint32 g_manufacture_id_bmc = 0;

LOCAL void init_bmc_manu_id(void)
{
    OBJ_HANDLE obj_handle = 0;
    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);

    (void)dal_get_property_value_uint32(obj_handle, BMC_CLASEE_NAME, &g_manufacture_id_bmc);

    return;
}
typedef gint32 (*get_slot_handler)(OBJ_HANDLE obj_handle, guint8 *component_slot, guint8 comp_type);
typedef struct compslot_map {
    guint8 comp_type;
    get_slot_handler pfn_get_slot;
} COMPSLOT_MAP;

LOCAL gint32 get_slot_memory(OBJ_HANDLE obj_handle, guint8 *component_slot, guint8 comp_type)
{
    
    if (component_slot == NULL || *component_slot != 0xff) {
        return RET_OK;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, component_slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s Groupid failed!", dfl_get_object_name(obj_handle));
    }
    return ret;
}

LOCAL gint32 get_slot_harddisk(OBJ_HANDLE obj_handle, guint8 *component_slot, guint8 comp_type)
{
    gchar device_name[COMPONENT_DEVICE_NAME_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    if ((strstr(device_name, DEVICE_NAME_PREFIX_M2) == NULL)) {
        return RET_OK;
    }

    OBJ_HANDLE anchor_handle = INVALID_OBJ_HANDLE;
    gint32 ret = dal_get_specific_object_position(obj_handle, CLASS_ANCHOR, &anchor_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get %s related anchor obj failed, result is %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return ret;
    }
    (void)dal_get_property_value_byte(anchor_handle, PROPERTY_ANCHOR_SLOT_ID, component_slot);
    return RET_OK;
}


LOCAL gint32 get_slot_pciecard(OBJ_HANDLE obj_handle, guint8 *component_slot, guint8 comp_type)
{
    gchar *card_class_name = NULL;
    OBJ_HANDLE card_handle;
    switch (comp_type) {
        case COMPONENT_TYPE_PCIE_CARD:
            card_class_name = CLASS_PCIE_CARD;
            break;
        case COMPONENT_TYPE_OCP:
            card_class_name = CLASS_OCP_CARD;
            break;
        default:
            return RET_OK;
    }

    gint32 ret = dal_get_specific_object_byte(card_class_name, PROPETRY_PCIECARD_SLOT, *component_slot, &card_handle);
    if (ret == RET_OK) {
        (void)dal_get_silk_id(card_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, PROPETRY_PCIECARD_SLOT, component_slot);
    }
    return RET_OK;
}

LOCAL COMPSLOT_MAP g_component_slot_map[] = {
    {COMPONENT_TYPE_MEMORY, get_slot_memory},
    {COMPONENT_TYPE_HARDDISK, get_slot_harddisk},
    {COMPONENT_TYPE_PCIE_CARD, get_slot_pciecard},
    {COMPONENT_TYPE_OCP, get_slot_pciecard}
};

LOCAL void __rename_device_name_by_slot_id(OBJ_HANDLE obj_handle, const component_union *alias_id,
    gchar *device_name, gsize len)
{
    gint32 ret;
    gchar **str_arr = g_strsplit(device_name, DEVICE_NAME_SPLIT_FLAG, 2); // device_name由$分隔2段
    if (str_arr == NULL) {
        return;
    }

    if (str_arr[1] == NULL) {
        g_strfreev(str_arr);
        return;
    }

    // 3、获取component的slot号
    guint8 component_slot = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &component_slot);

    
    guint8 count = sizeof(g_component_slot_map) / sizeof(COMPSLOT_MAP);
    for (guint8 i = 0; i < count; i++) {
        if (alias_id->device_info[0] == g_component_slot_map[i].comp_type) {
            ret = g_component_slot_map[i].pfn_get_slot(obj_handle, &component_slot, g_component_slot_map[i].comp_type);
            if (ret != RET_OK) {
                g_strfreev(str_arr);
                return;
            }
            break;
        }
    }

    
    ret = snprintf_s(device_name, len, len - 1, "%s%u%s", str_arr[0], component_slot, str_arr[1]);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    debug_log(DLOG_DEBUG, "device name is %s", device_name);

    // 4、更新设备名称
    ret = dal_set_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_set_property_value_string fail, ret = %d", __FUNCTION__, ret);
    }

    g_strfreev(str_arr);
}

LOCAL void __rename_device_name_by_location(OBJ_HANDLE obj_handle, const gchar *device_name)
{
    if (strstr(device_name, DEVICE_NAME_ADD_LOCATION_FLAG) == NULL) {
        return;
    }

    gchar location[COMPONENT_LOCATION_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));
    if ((strstr(location, DEVICE_NAME_SPLIT_FLAG) != NULL) ||
        (strstr(location, DEVICE_NAME_ADD_LOCATION_FLAG) != NULL)) {
        debug_log(DLOG_ERROR, "%s location has", dfl_get_object_name(obj_handle));
        (void)send_component_obj_add_msg_to_queue(obj_handle, 0);
        return;
    }

    gchar **str_arr = g_strsplit(device_name, DEVICE_NAME_ADD_LOCATION_FLAG, 2);
    if (str_arr == NULL) {
        return;
    }
    if (str_arr[1] == NULL) {
        g_strfreev(str_arr);
        return;
    }

    debug_log(DLOG_DEBUG, "str[0] is %s, str[1] is %s", str_arr[0], str_arr[1]);

    gchar new_device_name[COMPONENT_DEVICE_NAME_LEN] = {0};
    gint32 ret = snprintf_s(new_device_name, sizeof(new_device_name), sizeof(new_device_name) - 1, "%s%s%s", str_arr[0],
        location, str_arr[1]);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    debug_log(DLOG_DEBUG, "new device name is %s", new_device_name);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, new_device_name, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        g_strfreev(str_arr);
        debug_log(DLOG_ERROR, "Set %s Device Name failed, result is %d", dfl_get_object_name(obj_handle), ret);
        return;
    }

    g_strfreev(str_arr);
}


LOCAL gint32 __rename_component_device_name(OBJ_HANDLE obj_handle, const component_union *alias_id, gchar *device_name,
    gsize len)
{
    // 此处device_name为传入传出参数，数据会被修改，作为后续调用的入参
    __rename_device_name_by_slot_id(obj_handle, alias_id, device_name, len);

    __rename_device_name_by_location(obj_handle, device_name);

    return RET_OK;
}

LOCAL void __on_component_obj_add_event(void *param)
{
    if (g_component_obj_add_msg_queue == NULL) {
        return;
    }

    (void)prctl(PR_SET_NAME, (gulong) "FruAddObjTask");

    while (TRUE) {
        OBJECT_DESCP_S *msg = NULL;
        gint32 ret = vos_queue_receive(g_component_obj_add_msg_queue, (gpointer *)(&msg), QUE_WAIT_FOREVER);
        if ((ret != RET_OK) || (msg == NULL)) {
            debug_log(DLOG_ERROR, "%s Queue received failed, ret = %d", __FUNCTION__, ret);
            vos_task_delay(1000);
            continue;
        }

        (void)init_component(msg->object_handle, NULL);

        g_free(msg);
        msg = NULL;
        vos_task_delay(100);
    }
}

void init_component_base(void)
{
    (void)vos_thread_mutex_sem4_create(&g_connects_hot, (gchar *)"connector");

    init_bmc_manu_id();
}

void lock_connector(void)
{
    (void)vos_thread_sem4_p(g_connects_hot, SEM_WAIT_FOREVER);
}

void unlock_connector(void)
{
    (void)vos_thread_sem4_v(g_connects_hot);
}


gint32 position_compare(gconstpointer a, gconstpointer b)
{
    const DFT_COMPONENT_POSN_INFO_S *connector_a = (const DFT_COMPONENT_POSN_INFO_S *)a;
    const DFT_COMPONENT_POSN_INFO_S *connector_b = (const DFT_COMPONENT_POSN_INFO_S *)b;

    return (connector_a->position_id > connector_b->position_id) ? TRUE : FALSE;
}

GSList *get_connectors(void)
{
    return g_connector_list;
}

GSList **get_connectors_addr(void)
{
    return &g_connector_list;
}

void free_connectors(void)
{
    g_slist_free_full(g_connector_list, g_free);
    g_connector_list = NULL;
}


void snprintf_with_unknown(gchar *puc_manu_info, gint32 info_len, gint32 value, const char *str)
{
    if (puc_manu_info == NULL || info_len <= 0) {
        debug_log(DLOG_ERROR, "%s: puc_manu_info is null.", __FUNCTION__);
        return;
    }

    if (value == 0) {
        (void)snprintf_s(puc_manu_info, info_len, info_len - 1, ",Unknown");
        return;
    }

    gint32 ret = snprintf_s(puc_manu_info, info_len, info_len - 1, ",%d%s", value, str);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
}


void snprintf_with_guint32(gchar *puc_manu_info, gint32 info_len, guint32 value, const char *str)
{
    if (puc_manu_info == NULL || info_len <= 0) {
        debug_log(DLOG_ERROR, "%s: puc_manu_info is null.", __FUNCTION__);
        return;
    }

    if (value == 0) {
        if (snprintf_s(puc_manu_info, info_len, info_len - 1, ",Unknown") <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
        }

        return;
    }

    if (snprintf_s(puc_manu_info, info_len, info_len - 1, ",%u%s", value, str) <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
    }
}


gboolean is_device_type_valid(guint8 device_type)
{
    if ((device_type != DEVICE_TYPE_FAN) && (device_type != DEVICE_TYPE_PS)) {
        return TRUE;
    }

    
    guint8 server_type = 0;
    (void)dal_get_server_type(&server_type);
    // x6000 v6是刀片类型，但是没有管理板，也需要支持查询。
    if (server_type == SERVER_TYPE_BLADE && dal_is_aggregation_management_chassis_product() != TRUE) {
        debug_log(DLOG_INFO, "%s blade not support", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}

void frudata_convert_mac_hex2string(guchar *input, gchar *output, gint32 out_size, guint8 data_len)
{
    // 6 is the data lengths specified in the protocol.
    if (data_len == 6) {
        (void)snprintf_s(output, out_size, out_size - 1, "%02X:%02X:%02X:%02X:%02X:%02X", input[0], input[1], input[2],
            input[3], input[4], input[5]);
    }

    (void)snprintf_s(output, out_size, out_size - 1, "%02X%02X%02X%02X%02X%02X%02X%02X", input[0], input[1], input[2],
        input[3], input[4], input[5], input[6], input[7]);

    return;
}

void frudata_mac_str2int(gchar *input, guchar *output, guint32 out_size, guint8 *output_len)
{
#define MAC_TEMP_LEN 3
#define MAX_MAC_ADDR_LEN 10

    // 17 and 16 are the data lengths specified in the protocol.
    guint8 input_len = strlen(input);
    if (input_len != 17 && input_len != 16) {
        debug_log(DLOG_ERROR, "%s:input len = %d ,input = %s", __FUNCTION__, input_len, input);
        return;
    }

    
    guint8 scanf_len = (input_len == 17) ? 6 : 8;

    
    gint32 offset_per = (input_len == 17) ? 3 : 2;

    errno_t safe_fun_ret;
    gchar mac_temp[MAC_TEMP_LEN] = {0};
    guchar temp[MAX_MAC_ADDR_LEN] = {0};
    gint32 offset = 0;
    for (gint32 i = 0; i < scanf_len; i++) {
        safe_fun_ret = memmove_s(mac_temp, sizeof(mac_temp), input + offset, MAC_TEMP_LEN - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }
        temp[i] = strtol(mac_temp, NULL, 16);
        offset += offset_per;
    }

    *output_len = scanf_len;
    safe_fun_ret = memmove_s(output, out_size, temp, scanf_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    return;
}


gint32 component_device_num_compare_fun(gconstpointer a, gconstpointer b)
{
    guint8 deivce_num_a = 0;
    guint8 deivce_num_b = 0;
    (void)dal_get_property_value_byte((OBJ_HANDLE)a, PROPERTY_COMPONENT_DEVICENUM, &deivce_num_a);
    (void)dal_get_property_value_byte((OBJ_HANDLE)b, PROPERTY_COMPONENT_DEVICENUM, &deivce_num_b);

    if (deivce_num_a > deivce_num_b) {
        return 1;
    }
    if (deivce_num_a < deivce_num_b) {
        return -1;
    }
    return 0;
}

gint32 get_sdi_card_handle_by_logic_num(guint8 logicalNum, OBJ_HANDLE *obj_handle)
{
    if (obj_handle == NULL) {
        debug_log(DLOG_ERROR, "%s:obj_handle is null!", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    guint32 sd100_max_num = 0;
    (void)dfl_get_object_count(CLASS_PCIE_SDI_CARD, &sd100_max_num);

    if (logicalNum == 0 || logicalNum > sd100_max_num) {
        debug_log(DLOG_ERROR, "%s:logicalNum is out of range, logicalNum = %d, sd100_max_num = %d", __FUNCTION__,
            logicalNum, sd100_max_num);
        return COMP_CODE_OUTOF_RANGE;
    }

    gint32 ret = dal_get_object_handle_nth(CLASS_PCIE_SDI_CARD, logicalNum - 1, obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:(%s)dal_get_object_handle_nth(%d)!", __FUNCTION__, CLASS_PCIE_SDI_CARD,
            logicalNum - 1);
        return COMP_CODE_UNKNOWN;
    }

    return RET_OK;
}


guint8 get_blade_location(FRU_PRIV_PROPERTY_S *fru_priv)
{
#define PRODUCT_NAME_9008 "9008"
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "%s fru_priv is null.", __FUNCTION__);
        return 0;
    }

    guint8 slot_id = fru_priv->fru_property.slot_id;

    OBJ_HANDLE obj_handle = 0;
    gint32 result = dfl_get_object_handle(BMC_PRODUCT_OBJ_NAME_APP, &obj_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s %d: dfl_get_object_handle fail!result = %d", __FUNCTION__, __LINE__, result);
        return slot_id;
    }

    gchar product_name[NAME_LEN] = {0};
    (void)dal_get_property_value_string(obj_handle, BMC_PRODUCT_PRO_NAME, product_name, sizeof(product_name));
    if ((strncmp(product_name, PRODUCT_NAME_9008, strlen(PRODUCT_NAME_9008)) != 0) ||
        (dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &obj_handle) == RET_OK)) {
        return slot_id;
    }

    result = dfl_get_object_handle(PARTITION_OBJ_NAME, &obj_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_object_handle fail, result = %d", __FUNCTION__, result);
        return slot_id;
    }

    guint8 local_node = 0; 
    result = dal_get_property_value_byte(obj_handle, NODE_MODE, &local_node);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_property_value_byte %s fail! result = %d", __FUNCTION__, NODE_MODE, result);
        return slot_id;
    }

    return local_node + 1;
}

gint32 fill_device_physical_num(guint8 mezz_location, guint8 device_num, guint8 slot_id, guint8 physical_num,
    guint8 *id)
{
    switch (mezz_location) {
        case 0: 
            id[0] = slot_id;
            id[1] = physical_num;
            break;
        case 1: 
            id[0] = slot_id;
            id[1] = physical_num;
            break;
        case 2: 
            id[0] = slot_id + 8;
            id[1] = physical_num;
            break;
        case 3: 
            if (device_num < 3) { 
                id[0] = slot_id;
                id[1] = physical_num;
            }
            if (device_num >= 3) {        
                id[0] = slot_id + 8;      
                id[1] = physical_num - 2; 
            }
            break;
        case 4: 
            id[0] = slot_id - 8;
            id[1] = physical_num;
            break;
        case 5: 
            id[0] = slot_id;
            id[1] = physical_num;
            break;
        case 6: 
            if (device_num < 3) {    
                id[0] = slot_id - 8; 
                id[1] = physical_num;
            }
            if (device_num >= 3) { 
                id[0] = slot_id;
                id[1] = physical_num - 2; 
            }
            break;
        default:
            return RET_ERR;
    }

    return RET_OK;
}


gint32 check_device_type(guint8 comp_device_type)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get product handle fail.");
        return RET_ERR;
    }

    guchar mgnt_software_type;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MGMT_SOFTWARE_TYPE, &mgnt_software_type);

    
    if (mgnt_software_type != MGMT_SOFTWARE_TYPE_FRAME_PERIPHERAL_MGMT) {
        return RET_OK;
    }

    if (comp_device_type == DEVICE_TYPE_PCIE_CARD) {
        debug_log(DLOG_ERROR, "The soft_ware_type is 0x%02X. Don't support device_type = %d", mgnt_software_type,
            DEVICE_TYPE_PCIE_CARD);
        return RET_ERR;
    }

    return RET_OK;
}

gboolean check_device_num_range(guint8 device_type, guint8 device_num)
{
    OBJ_HANDLE object_handle = 0;

    if (device_type == DEVICE_TYPE_SDI_CARD && device_num == INVALID_UINT8) {
        
        return FALSE;
    }
    if (dfl_get_object_handle(XMLPARTITION_OBJ_NAME, &object_handle) == DFL_OK) {
        guint8 node_mode = 0;
        gint32 ret = dal_get_property_value_byte(object_handle, NODE_MODE, &node_mode);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s dal_get_property_value_byte failed!", __FUNCTION__);
            return FALSE;
        }

        
        if (node_mode == NODE_SLAVE) {
            
            return FALSE;
        }
    }

    guint8 arm_enable = 0;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (arm_enable != DISABLE) {
        return FALSE;
    }

    return TRUE;
}

gboolean is_manu_id_valid(const guint8 *manufacture_id)
{
    if (manufacture_id == NULL) {
        return FALSE;
    }

    guint32 manu_id = MAKE_DWORD(0, manufacture_id[2], manufacture_id[1], manufacture_id[0]);
    if (manu_id != g_manufacture_id_bmc) {
        debug_log(DLOG_ERROR, "ManufactureId:%d(ShouldBe:%d) is invalid!", manu_id, g_manufacture_id_bmc);
        return FALSE;
    }

    return TRUE;
}

gint32 is_component_type_support_fru(guint8 component_device_type)
{
    guint8 type_arr[CM_DEVICE_NUM_ALL + 1] = {0};
    type_arr[CM_DEVICE_TYPE_PS] = TRUE;
    type_arr[CM_DEVICE_TYPE_HDD_BACKPLANE] = TRUE;
    type_arr[CM_DEVICE_TYPE_RAID_CARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_PCIE_CARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_AMC] = TRUE;
    type_arr[CM_DEVICE_TYPE_MEZZ_CARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_NIC_CARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_MEMORY_BOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_PCIE_RISER] = TRUE;
    type_arr[CM_DEVICE_TYPE_MAINBOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_CHASSIS_BACKPLANE] = TRUE;
    type_arr[CM_DEVICE_TYPE_FANBOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_BMC] = TRUE;
    type_arr[CM_DEVICE_TYPE_MMC] = TRUE;
    type_arr[CM_DEVICE_TYPE_BASEBOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_FABRIC] = TRUE;
    type_arr[CM_DEVICE_TYPE_SWITCH_MEZZ] = TRUE;
    type_arr[CM_DEVICE_TYPE_IOBOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_CPUBOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_PCIE_ADAPTER] = TRUE;
    type_arr[CM_DEVICE_TYPE_EXPAND_BOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_GPU_BOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_CONVERGE_BOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_SOC_BOARD] = TRUE;
    type_arr[CM_DEVICE_TYPE_ALL] = TRUE;
    type_arr[CM_DEVICE_TYPE_TIANCHI_EXPBOARD] = TRUE;
    if (type_arr[component_device_type] != TRUE) {
        debug_log(DLOG_ERROR, "%s: component_device_type(%d)", __FUNCTION__, component_device_type);
        return COMP_CODE_OUTOF_RANGE;
    }

    return RET_OK;
}


void update_ps_ref_component_device_name(OBJ_HANDLE obj_handle)
{
    guchar device_type = COMPONENT_TYPE_INVALID;
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &device_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]get device_type failed.", __FUNCTION__);
        return;
    }

    if (device_type != COMPONENT_TYPE_PS) {
        return;
    }

    guchar device_num = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]get device_num failed.", __FUNCTION__);
        return;
    }

    gchar ps_device_name[COMPONENT_DEVICE_NAME_LEN] = {0};
    if (device_num == COM_PS_REDUNDANCY_DEVICE_NUMBER) {
        ret = snprintf_s(ps_device_name, sizeof(ps_device_name), sizeof(ps_device_name) - 1, POWER_SUPPLY_UNIT_NAME);
    } else {
        ret = snprintf_s(ps_device_name, sizeof(ps_device_name), sizeof(ps_device_name) - 1,
            POWER_SUPPLY_UNIT_NAME "%d", device_num);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return;
    }

    ret = dal_set_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, ps_device_name, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Init PS (%s) failed!", dfl_get_object_name(obj_handle));
        return;
    }

    return;
}

gint32 create_component_obj_add_task(void)
{
    gint32 ret = vos_queue_create(&g_component_obj_add_msg_queue);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s create queue failed, ret = %d", __FUNCTION__, ret);
        g_component_obj_add_msg_queue = NULL;
        return ret;
    }

    debug_log(DLOG_DEBUG, "%s Create new_obj_add_task task.", __FUNCTION__);

    
    ret = vos_task_create(&g_frudata_new_obj_add_task_id, "AddComp", __on_component_obj_add_event, NULL,
        DEFAULT_PRIORITY);
    if (ret == RET_OK) {
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "%s Create new_obj_add_task task fail, ret = %d", __FUNCTION__, ret);
    g_frudata_new_obj_add_task_id = 0;
    (void)vos_queue_delete(&g_component_obj_add_msg_queue);
    g_component_obj_add_msg_queue = NULL;
    return ret;
}

gint32 send_component_obj_add_msg_to_queue(OBJ_HANDLE obj_handle, guint8 option)
{
    OBJECT_DESCP_S *obj_descp_new = (OBJECT_DESCP_S *)g_malloc0(sizeof(OBJECT_DESCP_S));
    if (obj_descp_new != NULL) {
        obj_descp_new->object_handle = obj_handle;
        gint32 ret = vos_queue_send(g_component_obj_add_msg_queue, (gpointer)obj_descp_new);
        if (ret == RET_OK) {
            return RET_OK;
        }

        g_free(obj_descp_new);
    } else {
        debug_log(DLOG_INFO, "g_malloc0 failed, option is %u", option);
    }

    
    if (option <= 0) {
        return RET_ERR;
    }

    return init_component(obj_handle, NULL);
}


gint32 init_component(OBJ_HANDLE obj_handle, gpointer user_data)
{
    component_union alias_id;
    alias_id.component_alias_id = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &alias_id.device_info[0]);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_DEVICENUM, &alias_id.device_info[1]);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_GROUPID, &alias_id.device_info[2]);

    guint8 virtual_flag = 0;
    (void)dal_get_property_value_byte(obj_handle, METHOD_COMPONENT_VIRTUALFLAG, &virtual_flag);

    // 物理的对象就行别名获取
    if (virtual_flag == COMPONENT_PHYSICS) {
        gint32 ret = dfl_bind_object_alias(obj_handle, alias_id.component_alias_id);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Bind %s alias failed!", dfl_get_object_name(obj_handle));
            return RET_OK;
        }
    }

    // 初始化时候给电源devicename命名
    update_ps_ref_component_device_name(obj_handle);

    
    // 1、获取设备名称
    gchar device_name[COMPONENT_DEVICE_NAME_LEN] = {0};
    gint32 ret =
        dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s DeviceName failed!", dfl_get_object_name(obj_handle));
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "device_name=%s", device_name);

    if (strlen(device_name) == 0) {
        return RET_OK;
    }

    return __rename_component_device_name(obj_handle, &alias_id, device_name, COMPONENT_DEVICE_NAME_LEN);
}