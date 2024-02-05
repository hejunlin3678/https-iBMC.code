

#include "redfish_provider.h"
#include "redfish_provider_update.h"

LOCAL gint32 get_smm_firmwareinventory_count(json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_smm_firmwareinventory_object(json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_firmwareinventory_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 resources_obj_get_correspond_component_obj(OBJ_HANDLE obj_handle, const gchar *slot_name,
    gint32 i_device_type, OBJ_HANDLE *component_handle);
LOCAL gint32 get_firmwareinventory_object(json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_bios(json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_imu(json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_bmc(json_object **o_result_jso);
LOCAL void get_firmwareinventory_object_cpld(json_object **o_result_jso);
LOCAL void get_firmwareinventory_object_hwsr(json_object **o_result_jso);
LOCAL void get_firmwareinventory_object_ioctrl(json_object **o_result_jso);
LOCAL void get_cpld_bmc(json_object **o_result_jso);
LOCAL void get_cpld_listuri(json_object **o_result_jso, const gchar *class_name, const gchar *member_id);
LOCAL gint32 add_uri_to_members(json_object **o_result_jso, const gchar *member_id);
LOCAL void achieve_count(gchar *class_name, gchar *class_property, gint32 *count);
LOCAL gint32 get_firmwareinventory_object_sdcard_controller(json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_ps(json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_vrd(json_object **o_result_jso);
LOCAL void get_firmwareinventory_object_bbu(json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_lcd(json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_object_switch_fabric(json_object **o_result_jso);
LOCAL void get_firmwareinventory_object_retimer_type(json_object **o_result_jso);

LOCAL void __add_peripheral_fw_object(json_object *o_result_jso);

LOCAL void get_firmwareinventory_object_socboard(json_object **o_result_jso);
LOCAL gint32 get_board_firm_count(const gchar *class_name, const guint32 comp_idx);
LOCAL void get_firmwareinventory_object_io_board(json_object **o_result_jso);
LOCAL void get_firmwareinventory_object_mcu_board(json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_firmwareinventories_provider[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, BMC_CLASEE_NAME, BMC_MANA_VER_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_members, NULL, NULL, ETAG_FLAG_ENABLE},
};

LOCAL INT2STR_MAP_S component_type[] = {
    {COMPONENT_TYPE_HDD_BACKPLANE,          CLASS_HDDBACKPLANE_NAME},
    {COMPONENT_TYPE_RAID_CARD,              CLASS_RAIDCARD_NAME},
    {COMPONENT_TYPE_PCIE_CARD,              CLASS_PCIECARD_NAME},
    {COMPONENT_TYPE_NIC_CARD,               CLASS_NETCARD_NAME},
    {COMPONENT_TYPE_MEMORY_RISER,           CLASS_MEMBOARD_NAME},
    {COMPONENT_TYPE_PCIE_RISER,             CLASS_RISERPCIECARD_NAME},
    {COMPONENT_TYPE_FAN_BACKPLANE,          CLASS_FANBOARD_NAME},
    {COMPONENT_TYPE_IO_BOARD,               CLASS_IOBOARD_NAME},
    {COMPONENT_TYPE_BOARD_CPU,              CLASS_CPUBOARD_NAME},
    {COMPONENT_TYPE_HDD_BACKPLANE,          CLASS_EXPBOARD_NAME},
    {COMPONENT_TYPE_FABRIC_PLANE,           CLASS_DFT_VERSION},
    
    {COMPONENT_TYPE_GPU_BOARD,              CLASS_GPUBOARD_NAME},
    
};

typedef struct ac_cnt_params {
    gchar *class_name;
    gchar *class_property;
} ACHIEVE_CNT_PARAMS;

LOCAL ACHIEVE_CNT_PARAMS g_ac_cnt_params[] = {
    
    {BMC_CLASEE_NAME,                   BMC_MANA_VER_NAME},
    {BMC_CLASEE_NAME,                   BMC_BACKUP_MANA_VER_NAME},
#ifdef ARM64_HI1711_ENABLED
    {BMC_CLASEE_NAME,               PROPERTY_BMC_AVAILABLE_VER},
#endif
    {BMC_CLASEE_NAME,                   BMC_UBOOT_VER_NAME},
    {BMC_CLASEE_NAME,                   PROPERTY_UBOOT_BACKUP_VER},
    
    {BMC_CLASEE_NAME,                   BMC_CPLD_VER_NAME},
    
    
    {CLASS_LCD,                         PROPERTY_LCD_VERSION},
    
    {CLASS_NAME_PS,                     PROTERY_PS_VERSION},
    
    {CLASS_BBU_MODULE_NAME,             PROPERTY_BBU_MODULE_FIRMWARE_VER},
    
    {CLASS_RAID_CHIP_NAME,              PROPETRY_FX3S_VERSION},
    
    
    {CLASS_CPUBOARD_NAME,               PROPERTY_CPUBOARD_LOGICVER},
    {CLASS_CPUBOARD_NAME,               PROPERTY_HWSR_VERSION},
    
    {CLASS_MEMBOARD_NAME,               PROPERTY_MEMBOARD_LOGICVER},
    
    {CLASS_HDDBACKPLANE_NAME,           PROPERTY_HDDBACKPLANE_LOGICVER},
    {CLASS_HDDBACKPLANE_NAME,           PROPERTY_HWSR_VERSION},
    
    {CLASS_IOBOARD_NAME,                PROPERTY_IOBOARD_LOGICVER},
    
    {CLASS_RISERPCIECARD_NAME,          PROPERTY_RISERPCIECARD_LOGICVER},
    {CLASS_RISERPCIECARD_NAME,          PROPERTY_HWSR_VERSION},
    
    {CLASS_FANBOARD_NAME,               PROPERTY_FANBOARD_LOGICVER},
    {CLASS_FANBOARD_NAME,               PROPERTY_HWSR_VERSION},
    
    {CLASS_EXPBOARD_NAME,               PROPERTY_MEMBOARD_LOGICVER},
    {CLASS_EXPBOARD_NAME,               PROPERTY_HWSR_VERSION},
    
    {CLASS_RAIDCARD_NAME,               "NULL"},
    {CLASS_PCIECARD_NAME,               "NULL"},
    
    {CLASS_PERIPHERAL_FIRMWARE_NAME,    PROPERTY_SOFTWARE_VERSIONSTRING},
    
    {CLASS_GPUBOARD_NAME,               PROPERTY_GPUBOARD_LOGICVER},
    
    {CLASS_DFT_VERSION,                 "NULL"},
    {CLASS_CPUBOARD_NAME,               PROPERTY_CPUBOARD_BIOSVER},
    {CLASS_UPGRADE_MCU_SOFTWARE_NAME,   PROPERTY_SOFTWARE_VERSIONSTRING}
};


LOCAL void achieve_count(gchar *class_name, gchar *class_property, gint32 *count)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list fail, class_name is %s, ret=%d", __FUNCTION__, class_name, ret);
        g_slist_free(obj_list);
        return;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        ret = redfish_get_object_verison(class_name, (OBJ_HANDLE)obj_node->data, class_property, NULL, 0);
        do_val_if_expr(VOS_OK == ret, (*count)++);
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
}

LOCAL void achieve_count_retimer(gint32 *count)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        return;
    }
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        guint8 type = 0xff;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RETIMER_CHIP_TYPE, &type);
        if (type == RETIMER_TYPE_5902L || type == RETIMER_TYPE_5902H) {
            ret = redfish_get_object_verison(CLASS_RETIMER_NAME, (OBJ_HANDLE)obj_node->data, PROPERTY_RETIMER_SOFT_VER,
                                             NULL, 0);
            if (ret == RET_OK) {
                (*count)++;
            }
        }
    }
    g_slist_free(obj_list);
}


LOCAL gint32 get_smm_firmwareinventory_count(json_object **o_message_jso, json_object **o_result_jso)
{
    guint32 presence = 0;
    guint8 lcd_presence = 0;
    guint8 fan_presence = 0;
    guint32 fan_count = 0;
    guint32 count = 0;
    guint32 obj_index;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    // 本板部件数目
    count += SMM_BOARDFW_CNT;
    // 对板部件数目
    (void)get_other_board_present(&presence);
    if (presence == BLADE_PRESENCE) {
        count += SMM_BOARDFW_CNT;
    }
    // LCD
    ret = dfl_get_object_handle(OBJECT_LCD, &handle);
    if (ret == VOS_OK) {
        (void)dal_get_property_value_byte(handle, PROPERTY_PRESENCE, &lcd_presence);
        if (lcd_presence == BLADE_PRESENCE) {
            count++;
        }
    }
    // 风扇部件数目
    (void)dfl_get_object_count(CLASS_NAME_SMM_FAN, &fan_count);
    for (obj_index = 0; obj_index < fan_count; obj_index++) {
        ret = dal_get_object_handle_nth(CLASS_NAME_SMM_FAN, obj_index, &handle);
        if (ret != VOS_OK) {
            continue;
        }
        (void)dal_get_property_value_byte(handle, PROPERTY_FAN_PRESENT, &fan_presence);
        if (fan_presence == BLADE_PRESENCE) {
            count++;
        }
    }
    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}

LOCAL gint32 get_board_firm_count(const gchar *class_name, const guint32 comp_idx)
{
    gint32 count = 0;
    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list fail, ret=%d", __FUNCTION__, ret);
        return count;
    }
    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE board_handle = (OBJ_HANDLE)node->data;
        GSList *firm_list = NULL;
        (void)dal_get_object_list_position(board_handle, CLASS_UPGRADE_SOFTWARE_NAME, &firm_list);
        for (GSList *firm_node = firm_list; firm_node != NULL; firm_node = firm_node->next) {
            OBJ_HANDLE firm_handle = (OBJ_HANDLE)firm_node->data;
            if (!is_valid_board_firmver(firm_handle, comp_idx)) {
                debug_log(DLOG_DEBUG, "%s: is_valid_board_firmver=FALSE", __FUNCTION__);
                continue;
            }
            count++;
        }
        g_slist_free(firm_list);
    }
    g_slist_free(obj_list);
    return count;
}


gboolean is_mainboard_bios_supported(void)
{
    guint8 board_type = DISABLE;
    guint8 host_num = 0;
    gint32 ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &board_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get X86Enable fail, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }
    ret = dal_get_func_ability(OBJ_PRODUCT_COMPONENT, PROPERTY_COMPONENT_HOST_NUM, &host_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get HostNum fail, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }
    return (board_type != DISABLE && host_num == 0);
}


gboolean is_imu_valid(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar version_num[FWINV_STR_MAX_LEN] = {0};
    
    if (dal_match_product_id(PRODUCT_ID_TIANCHI) != TRUE) {
        return FALSE;
    }
    
    ret = dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d:bmc dal_get_object_handle_nth fail.", __FUNCTION__, __LINE__));

    
    ret = redfish_get_object_verison(CLASS_ME_INFO, obj_handle, PROPERTY_MEINFO_FIRM_VER, version_num,
        sizeof(version_num));
    return_val_do_info_if_expr(VOS_OK != ret, FALSE,
        debug_log(DLOG_DEBUG, "%s, %d: redfish_get_object_verison.", __FUNCTION__, __LINE__));
    
    if (!strncmp(version_num, INVALID_IMU_MAJOR_VERSION, IMU_MAJOR_VERSION_LEN)) {
        return FALSE;
    }
    return TRUE;
}

LOCAL gint32 get_bios_imu_count(gint32 *count)
{
    gint32 ret;
    OBJ_HANDLE object_handle;

    
    ret = dfl_get_object_handle(BIOS_CLASS_NAME, &object_handle);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_ERROR, "%s, %d: get object_handle fail.", __FUNCTION__, __LINE__));
    ret = redfish_get_object_verison(BIOS_CLASS_NAME, object_handle, BMC_BIOS_VER_NAME, NULL, 0);
    do_val_if_expr(VOS_OK == ret, (*count)++);

    
    if (is_imu_valid() == TRUE) {
        (*count)++;
    }
    return ret;
}

LOCAL void get_vrd_count(gint32* count)
{
    GSList* handle_list = NULL;
    GSList* list_item = NULL;
    OBJ_HANDLE object_handle;
    OBJ_HANDLE obj_conponent_handle;
    gchar vrd_version[VRD_VERSION_LENGTH] = {0};
    gint32 ret;
    
    ret = dfl_get_object_list(CLASS_VRD_UPGRADE_MGNT, &handle_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get VRDUpgradeMgnt object list failed. ret = %d.", ret);
        return;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        gint32 ret1 = dal_get_property_value_string(object_handle, PROPERTY_VRDVERSION, vrd_version,
            sizeof(vrd_version));
        gint32 ret2 = dfl_get_referenced_object(object_handle, PROPERTY_COMPONENT_OBJECT,
            &obj_conponent_handle);
        if (ret1 == RET_OK && ret2 == RET_OK) {
            *count += 1;
        } else {
            debug_log(DLOG_ERROR,
                "get property vrd_version value failed, ret = %d, or get referenced component object failed, ret = %d",
                ret1, ret2);
        }
    }
    g_slist_free(handle_list);
}


LOCAL gint32 get_firmwareinventory_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 0;
    OBJ_HANDLE object_handle;
    OBJ_HANDLE chip_handle = 0;
    guint8 software_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer1.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        gint32 ret = get_smm_firmwareinventory_count(o_message_jso, o_result_jso);
        return ret;
    }

    for (guint32 idx = 0; idx < G_N_ELEMENTS(g_ac_cnt_params); idx++) {
        (void)achieve_count(g_ac_cnt_params[idx].class_name, g_ac_cnt_params[idx].class_property, &count);
    }

    
    if (is_mainboard_bios_supported() && get_bios_imu_count(&count) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    
    gint32 ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &object_handle);
    if (VOS_OK == ret) {
        ret = dfl_get_referenced_object(object_handle, PROPERTY_VRD_UPGRADE_REFER_CHIP, &chip_handle);
        do_val_if_expr(VOS_OK == ret, count++);
    }
    
    
    get_vrd_count(&count);

    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6) == TRUE) {
        achieve_count(CLASS_RETIMER_NAME, PROPERTY_RETIMER_SOFT_VER, &count);
    } else {
        achieve_count_retimer(&count);
    }
    count += get_board_firm_count(CLASS_SOC_BOARD, BIOS_COMP_ID_EX_ATLAS);
    count += get_board_firm_count(CLASS_IOBOARD_NAME, BIOS_COMP_ID_EX_1601);

    
    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}


LOCAL gint32 get_firmwareinventory_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 software_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n.", __FUNCTION__, __LINE__);

        return HTTP_INTERNAL_SERVER_ERROR;
    }


    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) { // MM920可升级部件有区别
        ret = get_smm_firmwareinventory_object(o_message_jso, o_result_jso);
    } else {
        ret = get_firmwareinventory_object(o_message_jso, o_result_jso);
    }

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get_firmwareinventory_object fail\n.", __FUNCTION__, __LINE__);

        if (NULL != *o_result_jso) {
            (void)json_object_put(*o_result_jso);
            *o_result_jso = NULL;
        }

        return ret;
    }

    return HTTP_OK;
}


LOCAL void common_get_smm_firmware_inventory_object(json_object **result_jso, gchar *member_id, guint8 smm_index,
    guint32 member_id_size)
{
    return_if_expr(result_jso == NULL || member_id == NULL || member_id_size == 0);

    // Uboot
    (void)snprintf_s(member_id, member_id_size, member_id_size - 1, "HMM%uActiveUboot", smm_index);
    (void)add_uri_to_members(result_jso, member_id);
    (void)snprintf_s(member_id, member_id_size, member_id_size - 1, "HMM%uBackupUboot", smm_index);
    (void)add_uri_to_members(result_jso, member_id);
    // Software
    (void)snprintf_s(member_id, member_id_size, member_id_size - 1, "HMM%uActiveSoftware", smm_index);
    (void)add_uri_to_members(result_jso, member_id);
    (void)snprintf_s(member_id, member_id_size, member_id_size - 1, "HMM%uBackupSoftware", smm_index);
    (void)add_uri_to_members(result_jso, member_id);
#ifdef ARM64_HI1711_ENABLED
    (void)snprintf_s(member_id, member_id_size, member_id_size - 1, "HMM%uAvailableBMC", smm_index);
    (void)add_uri_to_members(result_jso, member_id);
#endif

    // CPLD
    (void)snprintf_s(member_id, member_id_size, member_id_size - 1, "HMM%uCPLD", smm_index);
    (void)add_uri_to_members(result_jso, member_id);
}


LOCAL gint32 get_smm_firmwareinventory_object(json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 smm_index = 0;
    guint32 fan_index;
    gchar member_id[MAX_MEM_ID_LEN] = { 0 };
    guint32 presence = 0;
    guint8 fan_presence = 0;
    guint8 lcd_presence = 0;
    guint32 count = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    // 获取本板可升级资源
    (void)get_smm_index(&smm_index);
    (void)common_get_smm_firmware_inventory_object(o_result_jso, member_id, smm_index, sizeof(member_id));
    // 获取对板可升级资源
    (void)get_other_board_present(&presence);
    if (presence == BLADE_PRESENCE) {
        smm_index = smm_index == 1 ? 2 : 1;
        (void)common_get_smm_firmware_inventory_object(o_result_jso, member_id, smm_index, sizeof(member_id));
    }

    // LCD
    ret = dfl_get_object_handle(OBJECT_LCD, &handle);
    if (ret == VOS_OK) {
        (void)dal_get_property_value_byte(handle, PROPERTY_PRESENCE, &lcd_presence);
        if (lcd_presence == BLADE_PRESENCE) {
            (void)snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "Lcd");
            (void)add_uri_to_members(o_result_jso, member_id);
        }
    }
    // 获取风扇可升级资源
    (void)dfl_get_object_count(CLASS_NAME_SMM_FAN, &count);
    for (fan_index = 0; fan_index < count; fan_index++) {
        ret = dal_get_object_handle_nth(CLASS_NAME_SMM_FAN, fan_index, &handle);
        if (ret != VOS_OK) {
            continue;
        }
        (void)dal_get_property_value_byte(handle, PROPERTY_FAN_PRESENT, &fan_presence);
        if (fan_presence == BLADE_PRESENCE) {
            (void)snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "Fantray%u", fan_index + 1);
            (void)add_uri_to_members(o_result_jso, member_id);
        }
    }
    return VOS_OK;
}

LOCAL gint32 get_firmwareinventory_object(json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    
    ret = get_firmwareinventory_object_bmc(o_result_jso);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_handle fail.\n", __FUNCTION__, __LINE__));

    
    if (is_mainboard_bios_supported()) {
        
        ret = get_firmwareinventory_object_bios(o_result_jso);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_handle fail.\n", __FUNCTION__, __LINE__));
    }
    
    (void)get_firmwareinventory_object_imu(o_result_jso);
    

    
    
    (void)get_firmwareinventory_object_lcd(o_result_jso);

    
    (void)get_firmwareinventory_object_ps(o_result_jso);

    get_firmwareinventory_object_bbu(o_result_jso);

    
    
    (void)get_firmwareinventory_object_sdcard_controller(o_result_jso);
    
    
    (void)get_firmwareinventory_object_switch_fabric(o_result_jso);
    
    get_firmwareinventory_object_cpld(o_result_jso);

    
    get_firmwareinventory_object_hwsr(o_result_jso);

    
    get_firmwareinventory_object_ioctrl(o_result_jso);

    
    get_firmwareinventory_object_retimer_type(o_result_jso);

    
    (void)get_firmwareinventory_object_vrd(o_result_jso);
    
    
    __add_peripheral_fw_object(*o_result_jso);
    
    
    get_firmwareinventory_object_socboard(o_result_jso);

    
    get_firmwareinventory_object_io_board(o_result_jso);

    
    get_firmwareinventory_object_mcu_board(o_result_jso);
 
    return VOS_OK;
}

LOCAL void get_hwsr_component_info(OBJ_HANDLE component_handle, gchar *member_id, guint8 memid_len)
{
    gint32 ret;
    gchar device_name[MAX_PROP_NAME] = {0};
    gchar location[MAX_PROP_NAME] = {0};

    (void)dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
        sizeof(device_name));
    (void)dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));
    dal_clear_string_blank(device_name, sizeof(device_name));
    dal_clear_string_blank(location, sizeof(location));

    // location + devicename
    ret = snprintf_s(member_id, memid_len, memid_len - 1, "%s%s%s", location, device_name, FI_HWSR_MEMBERID);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
}

LOCAL void get_hwsr_listuri(json_object **o_result_jso, const gchar *class_name, const gchar *version_name)
{
    gint32 ret;
    OBJ_HANDLE component_handle = 0;
    gchar member_id[MAX_MEM_ID_LEN] = {0};
    gint32 device_type = -1;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    for (guint32 i = 0; i < G_N_ELEMENTS(component_type); i++) {
        if (g_strcmp0(component_type[i].str_val, class_name) == 0) {
            device_type = component_type[i].int_val;
            break;
        }
    }

    if (device_type == -1) {
        debug_log(DLOG_ERROR, "%s: not find matched device", __FUNCTION__);
        return;
    }

    ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s get %s object handle failed, ret = %d", __FUNCTION__, class_name, ret);
        return;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        ret = redfish_get_object_verison(class_name, (OBJ_HANDLE)obj_node->data, version_name, NULL, 0);
        if (ret != RET_OK) {
            continue;
        }

        ret = resources_obj_get_correspond_component_obj((OBJ_HANDLE)obj_node->data, PROPERTY_FIRMWARE_SLOT,
            device_type, &component_handle);
        if (ret != RET_OK) {
            continue;
        }

        (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));
        get_hwsr_component_info(component_handle, member_id, MAX_MEM_ID_LEN);

        ret = add_uri_to_members(o_result_jso, member_id);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            return;
        }
    }

    g_slist_free(obj_list);
    return;
}


LOCAL void get_firmwareinventory_object_hwsr(json_object **o_result_jso)
{
    
    get_hwsr_listuri(o_result_jso, CLASS_EXPBOARD_NAME, PROPERTY_HWSR_VERSION);

    
    get_hwsr_listuri(o_result_jso, CLASS_CPUBOARD_NAME, PROPERTY_HWSR_VERSION);

    
    get_hwsr_listuri(o_result_jso, CLASS_HDDBACKPLANE_NAME, PROPERTY_HWSR_VERSION);

    
    get_hwsr_listuri(o_result_jso, CLASS_FANBOARD_NAME, PROPERTY_HWSR_VERSION);

    
    get_hwsr_listuri(o_result_jso, CLASS_RISERPCIECARD_NAME, PROPERTY_HWSR_VERSION);
}

gint32 get_io_board_firm_memberid(OBJ_HANDLE firm_handle, gchar *buf, guint32 buf_len)
{
    OBJ_HANDLE comp_handle = 0;
    gint32 ret = dal_get_specific_position_object_byte(firm_handle, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
        COMPONENT_TYPE_IO_BOARD, &comp_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_specific_position_object_byte fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    gchar loc_dev[FI_URI_LEN] = { 0 };
    ret = get_object_obj_location_devicename(comp_handle, loc_dev, FI_URI_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get location devicename fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = sprintf_s(buf, buf_len, "%sBios", loc_dev);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}

gint32 get_mcu_board_firm_memberid(OBJ_HANDLE firm_handle, gchar *buf, guint32 buf_len)
{
    gint32 ret;
    gchar location[MAX_STRBUF_LEN + 1] = {0};
    gchar name[MAX_STRBUF_LEN + 1] = {0};
    gchar ver_str[MAX_STRBUF_LEN + 1] = {0};

    ret = dal_get_property_value_string(firm_handle, PROPERTY_SOFTWARE_LOCATION, location, sizeof(location));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get mcuboard location fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    // 清除空格
    (void)dal_clear_string_blank(location, sizeof(location));
    (void)dal_get_property_value_string(firm_handle, PROPERTY_SOFTWARE_NAME, name, sizeof(name));
    ret = dal_get_property_value_string(firm_handle, PROPERTY_SOFTWARE_VERSIONSTRING, ver_str, sizeof(ver_str));
    
    if (ret != RET_OK || strlen(ver_str) == 0) {
        debug_log(DLOG_ERROR, "%s: get version fail, ver_str=%s, ret=%d", __FUNCTION__, ver_str, ret);
        return RET_ERR;
    }

    ret = sprintf_s(buf, buf_len, "%s%s", location, name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}
 
LOCAL void get_firmwareinventory_object_io_board(json_object **o_result_jso)
{
    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_IOBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list fail, ret=%d", __FUNCTION__, ret);
        return;
    }
    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE io_board_handle = (OBJ_HANDLE)node->data;
        OBJ_HANDLE firm_handle = 0;
        ret = dal_get_specific_object_position(io_board_handle, CLASS_UPGRADE_SOFTWARE_NAME, &firm_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: dal_get_specific_object_position fail, ret=%d", __FUNCTION__, ret);
            continue;
        }
        if (!is_valid_board_firmver(firm_handle, BIOS_COMP_ID_EX_1601)) {
            debug_log(DLOG_DEBUG, "%s: is_valid_io_board_firmver=FALSE", __FUNCTION__);
            continue;
        }
        gchar member_id[FI_URI_LEN] = { 0 };
        ret = get_io_board_firm_memberid(firm_handle, member_id, FI_URI_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get_io_board_firm_memberid fail. ret=%d", __FUNCTION__, ret);
            continue;
        }
        ret = add_uri_to_members(o_result_jso, member_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: add_uri_to_members fail. ret=%d", __FUNCTION__, ret);
        }
    }
    g_slist_free(obj_list);
}

LOCAL void get_firmwareinventory_object_mcu_board(json_object **o_result_jso)
{
    GSList *obj_list = NULL;
 
    gint32 ret = dfl_get_object_list(CLASS_UPGRADE_MCU_SOFTWARE_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list fail, ret=%d", __FUNCTION__, ret);
        return;
    }
    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE mcubrd_handle = (OBJ_HANDLE)node->data;
        gchar member_id[FI_URI_LEN] = { 0 };
        ret = get_mcu_board_firm_memberid(mcubrd_handle, member_id, FI_URI_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: get mcuboard member_id fail, ret=%d", __FUNCTION__, ret);
            continue;
        }
        ret = add_uri_to_members(o_result_jso, member_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: add_uri_to_members fail. ret=%d", __FUNCTION__, ret);
        }
    }
    g_slist_free(obj_list);
}
 

LOCAL void get_firmwareinventory_object_cpld(json_object **o_result_jso)
{
    
    
    get_cpld_bmc(o_result_jso);

    
    get_cpld_listuri(o_result_jso, CLASS_CPUBOARD_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    get_cpld_listuri(o_result_jso, CLASS_MEMBOARD_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    get_cpld_listuri(o_result_jso, CLASS_HDDBACKPLANE_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    get_cpld_listuri(o_result_jso, CLASS_IOBOARD_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    get_cpld_listuri(o_result_jso, CLASS_RISERPCIECARD_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    get_cpld_listuri(o_result_jso, CLASS_FANBOARD_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    get_cpld_listuri(o_result_jso, CLASS_EXPBOARD_NAME, PROPERTY_FIRMWARE_LOGICVER);

    
    
    get_cpld_listuri(o_result_jso, CLASS_RAIDCARD_NAME, PROPERTY_FIRMWARE_LOGICVER);
    get_cpld_listuri(o_result_jso, CLASS_PCIECARD_NAME, PROPERTY_FIRMWARE_LOGICVER);
    

    
    get_cpld_listuri(o_result_jso, CLASS_GPUBOARD_NAME, PROPERTY_GPUBOARD_LOGICVER);
    
}


LOCAL gint32 resources_obj_get_correspond_component_obj(OBJ_HANDLE obj_handle, const gchar *slot_name,
    gint32 i_device_type, OBJ_HANDLE *component_handle)
{
    gint32 ret;
    guint8 slot = 0;
    guint8 com_slot = 0;
    guint8 com_device_type = 0;
    guint8 com_fruid = 0;
    guint8 res_fruid = 0;
    gchar class_name[MAX_NAME_SIZE] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    // 通过RefComponent 获取资源对应的部件类
    
    if (i_device_type == COMPONENT_TYPE_HDD_BACKPLANE || i_device_type == COMPONENT_TYPE_IO_BOARD ||
        i_device_type == COMPONENT_TYPE_BOARD_CPU) {
        

        ret = dfl_get_referenced_object(obj_handle, PROPERTY_IOBOARD_REFCOMPONENT, component_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s, %d: dfl_get_referenced_object fail object: %s property: %s.\n", __FUNCTION__,
                __LINE__, dfl_get_object_name(obj_handle), PROPERTY_IOBOARD_REFCOMPONENT);
            return VOS_ERR;
        }

        debug_log(DLOG_DEBUG, "%s, %d: classname: %s componentname: %s.\n", __FUNCTION__, __LINE__,
            dfl_get_object_name(obj_handle), dfl_get_object_name(*component_handle));

        return VOS_OK;
    }
    

    (void)dal_get_property_value_byte(obj_handle, slot_name, &slot);

    ret = dal_get_object_list_position(obj_handle, CLASS_COMPONENT, &obj_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: dal_get_object_list_position fail", __FUNCTION__, __LINE__);

        return VOS_ERR;
    }

    (void)dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        // 匹配槽位号和部件编号
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICENUM, &com_slot);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &com_device_type);

        // FruId为255 用于告警优化
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_FRUID, &com_fruid);

        
        
        if (0 == g_strcmp0(class_name, CLASS_RISERPCIECARD_NAME)) {
            return_val_do_info_if_expr(
                (i_device_type == com_device_type || COMPONENT_TYPE_BOARD_PCIE_ADAPTER == com_device_type) &&
                slot == com_slot && 255 != com_fruid,
                VOS_OK, *component_handle = (OBJ_HANDLE)obj_node->data;
                g_slist_free(obj_list));
            continue;
        }
        

        // Fabric Plane 通过 fruid匹配
        if (com_device_type == COMPONENT_TYPE_FABRIC_PLANE) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_FRU_ID, &res_fruid);

            if (com_device_type == i_device_type && com_fruid == res_fruid) {
                *component_handle = (OBJ_HANDLE)obj_node->data;
                do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

                return VOS_OK;
            }
        } else {
            
            if (com_device_type == i_device_type && slot == com_slot && 255 != com_fruid) {
                *component_handle = (OBJ_HANDLE)obj_node->data;
                do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

                return VOS_OK;
            }
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return VOS_ERR;
}

LOCAL gint32 get_cpuboard_ioctrl_foreach(OBJ_HANDLE obj_cpuboard, gpointer user_data)
{
    gint32 ret;
    json_object** o_result_jso = (json_object **)user_data;
    OBJ_HANDLE component_handle = OBJ_HANDLE_COMMON;
    gchar member_id[64] = {0};
    gchar device_name[32] = {0};
    gchar location[32] = {0};

    ret = redfish_get_object_verison(CLASS_CPUBOARD_NAME, obj_cpuboard, PROPERTY_CPUBOARD_BIOSVER, NULL, 0);
    if (ret != RET_OK) {
        return RET_OK;
    }

    // ��ȡ���������Ĳ���Component�����
    ret = resources_obj_get_correspond_component_obj(obj_cpuboard, NULL, COMPONENT_TYPE_BOARD_CPU, &component_handle);
    if (ret != RET_OK) {
        return RET_OK;
    }

    (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
    (void)memset_s(location, sizeof(location), 0, sizeof(location));
    (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));

    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
        sizeof(device_name));
    if (ret != RET_OK) {
        return RET_OK;
    }

    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));
    if (ret != RET_OK) {
        return RET_OK;
    }

    dal_clear_string_blank(device_name, sizeof(device_name));
    dal_clear_string_blank(location, sizeof(location));

    // location + devicename
    ret = snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s%s%s", location, device_name,
        FI_IOCTRL_MEMBERID);
    if (ret <= 0) {
        return RET_OK;
    }

    ret = add_uri_to_members(o_result_jso, member_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add uri fail, member_id=%s, ret=%d.", __FUNCTION__, member_id, ret);
    }
    return RET_OK;
}

LOCAL void get_firmwareinventory_object_ioctrl(json_object **o_result_jso)
{
    (void)dfl_foreach_object(CLASS_CPUBOARD_NAME, get_cpuboard_ioctrl_foreach, o_result_jso, NULL);
}


LOCAL void get_cpld_listuri(json_object **o_result_jso, const gchar *class_name, const gchar *version_name)
{
    int iRet = -1;
    gint32 ret;
    guint32 i;
    OBJ_HANDLE component_handle = 0;
    gchar member_id[64] = {0};
    gchar device_name[32] = {0};
    gchar location[32] = {0};
    gchar slot_name[FI_NAME_LEN] = {0};

    gint32 i_device_type = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    // 槽位号名称
    if (0 == g_strcmp0(class_name, CLASS_RAIDCARD_NAME)) {
        (void)strncpy_s(slot_name, sizeof(slot_name), PROPERTY_RAIDCARD_SLOT, strlen(PROPERTY_RAIDCARD_SLOT));
    } else if (0 == g_strcmp0(class_name, CLASS_PCIECARD_NAME)) {
        (void)strncpy_s(slot_name, sizeof(slot_name), PROPETRY_PCIECARD_SLOT, strlen(PROPETRY_PCIECARD_SLOT));
    } else {
        (void)strncpy_s(slot_name, sizeof(slot_name), PROPERTY_FIRMWARE_SLOT, strlen(PROPERTY_FIRMWARE_SLOT));
    }

    // 获取部件编号
    for (i = 0; i < sizeof(component_type) / sizeof(component_type[0]); i++) {
        if (0 == g_strcmp0(component_type[i].str_val, class_name)) {
            i_device_type = component_type[i].int_val;
        }
    }

    ret = dfl_get_object_list(class_name, &obj_list);
    return_if_expr(VOS_OK != ret);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = redfish_get_object_verison(class_name, (OBJ_HANDLE)obj_node->data, version_name, NULL, 0);
        do_if_expr(VOS_OK != ret, continue);

        // 获取软件归属的部件Component类对象
        ret = resources_obj_get_correspond_component_obj((OBJ_HANDLE)obj_node->data, slot_name, i_device_type,
            &component_handle);
        do_if_expr(VOS_OK != ret, continue);

        (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
        (void)memset_s(location, sizeof(location), 0, sizeof(location));
        (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));

        ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        check_success_do_continue(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_string fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));
        check_success_do_continue(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_string fail.\n", __FUNCTION__, __LINE__));

        dal_clear_string_blank(device_name, sizeof(device_name));
        dal_clear_string_blank(location, sizeof(location));

        // location + devicename
        iRet = snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s%s%s", location, device_name,
            FI_CPLD_MEMBERID);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        ret = add_uri_to_members(o_result_jso, member_id);
        if (VOS_OK != ret) {
            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

            return;
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return;
}


LOCAL void get_cpld_bmc(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE chip_handle = 0;

    ret = dfl_get_object_handle(BMC_CLASEE_NAME, &obj_handle);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_handle bmc fail.\n", __FUNCTION__, __LINE__));

    ret = redfish_get_object_verison(BMC_CLASEE_NAME, obj_handle, BMC_CPLD_VER_NAME, NULL, 0);
    return_if_expr(VOS_OK != ret);

    
    ret = add_uri_to_members(o_result_jso, FI_MAINCPLD_MEMBERID);
    return_if_expr(VOS_OK != ret);

    
    obj_handle = 0;
    ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &obj_handle);
    return_if_expr(VOS_OK != ret);

    ret = dfl_get_referenced_object(obj_handle, PROPERTY_VRD_UPGRADE_REFER_CHIP, &chip_handle);
    return_if_expr(VOS_OK != ret);

    ret = add_uri_to_members(o_result_jso, FI_BASICCPLD_MEMBERID);
    return_if_expr(VOS_OK != ret);
    

    return;
}


LOCAL gint32 get_firmwareinventory_object_bios(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:bmc dfl_get_object_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = redfish_get_object_verison(BIOS_CLASS_NAME, obj_handle, BMC_BIOS_VER_NAME, NULL, 0);
    
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d: redfish_get_object_verison.\n", __FUNCTION__, __LINE__));
    

    
    ret = add_uri_to_members(o_result_jso, FI_BIOS_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:Bios add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}

LOCAL gint32 get_firmwareinventory_object_imu(json_object **o_result_jso)
{
    gint32 ret;
    if (is_imu_valid() != TRUE) {
        return VOS_ERR;
    }
    
    ret = add_uri_to_members(o_result_jso, FI_IMU_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:IMU add_uri_to_members fail.", __FUNCTION__, __LINE__));
    return VOS_OK;
}

gint32 gen_firmwareinventory_retimer_rsc_id(OBJ_HANDLE object_handle, gchar *prefix, gchar *output_str,
                                             guint32 str_len, guint8 type)
{
#define LOCATION_NAME_LEN 32
    guint8 retimer_type = 0XFF;
    OBJ_HANDLE comp_handle = 0;
    gchar device_name[LOCATION_NAME_LEN] = {0};
    gchar location[LOCATION_NAME_LEN] = {0};
    gchar chip_location[LOCATION_NAME_LEN] = {0};
    gchar *id_type = NULL;

    if (output_str == NULL || str_len == 0) {
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_RETIMER_CHIP_TYPE, &retimer_type);
    if (retimer_type != type) {
        return RET_ERR;
    }

    gint32 ret = redfish_get_object_verison(CLASS_RETIMER_NAME, object_handle, PROPERTY_RETIMER_SOFT_VER, NULL, 0);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    if (dfl_get_referenced_object(object_handle, PROPERTY_IOBOARD_REFCOMPONENT, &comp_handle) != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_referenced_object failed, ret=%d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    (void)dal_get_property_value_string(comp_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    (void)dal_get_property_value_string(comp_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));
    (void)dal_get_property_value_string(object_handle, PROPERTY_RETIMER_LOCATION, chip_location, sizeof(chip_location));

    dal_clear_string_blank(device_name, sizeof(device_name));
    dal_clear_string_blank(location, sizeof(location));
    dal_clear_string_blank(chip_location, sizeof(chip_location));
    if (type == RETIMER_TYPE_5902L) {
        id_type = RETIMER_TYPE_5902L_NAME;
    } else if (type == RETIMER_TYPE_5902H) {
        id_type = RETIMER_TYPE_5902H_NAME;
    } else {
        debug_log(DLOG_ERROR, "%s: retimer type check failed, type=%d.", __FUNCTION__, type);
        return RET_ERR;
    }
    if (prefix) {
        ret = snprintf_s(output_str, str_len, str_len - 1, "%s%s%s%s%s", prefix, location, device_name, id_type,
                         chip_location);
    } else {
        ret = snprintf_s(output_str, str_len, str_len - 1, "%s%s%s%s", location, device_name, id_type, chip_location);
    }
    if (ret <= 0) {
        return RET_ERR;
    }
    return RET_OK;
}
LOCAL void get_firmwareinventory_object_retimer(json_object** o_result_jso, guint8 type)
{
    gint32 ret;
    gchar member_id[MAX_MEM_ID_LEN] = {0};
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;

    ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        return;
    }
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = gen_firmwareinventory_retimer_rsc_id((OBJ_HANDLE)obj_node->data, NULL, member_id, MAX_MEM_ID_LEN, type);
        if (ret != RET_OK) {
            continue;
        }
        ret = add_uri_to_members(o_result_jso, member_id);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: add_uri_to_members failed, ret=%d.", __FUNCTION__, ret);
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
}

LOCAL void get_firmwareinventory_object_retimer_type(json_object** o_result_jso)
{
    get_firmwareinventory_object_retimer(o_result_jso, RETIMER_TYPE_5902L);
    get_firmwareinventory_object_retimer(o_result_jso, RETIMER_TYPE_5902H);
}



LOCAL gint32 get_firmwareinventory_object_bmc(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dfl_get_object_handle(BMC_CLASEE_NAME, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:bmc dfl_get_object_handle fail.\n", __FUNCTION__, __LINE__));

    
    
    ret = redfish_get_object_verison(BMC_CLASEE_NAME, obj_handle, BMC_MANA_VER_NAME, NULL, 0);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:bmc redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));

    
    ret = redfish_get_object_verison(BMC_CLASEE_NAME, obj_handle, BMC_BACKUP_MANA_VER_NAME, NULL, 0);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:bmc redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));

#ifdef ARM64_HI1711_ENABLED
    
    ret = redfish_get_object_verison(BMC_CLASEE_NAME, obj_handle, PROPERTY_BMC_AVAILABLE_VER, NULL, 0);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:bmc redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));
#endif
    

    
    ret = redfish_get_object_verison(BMC_CLASEE_NAME, obj_handle, BMC_UBOOT_VER_NAME, NULL, 0);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:Uboot redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));

    
    ret = redfish_get_object_verison(BMC_CLASEE_NAME, obj_handle, PROPERTY_UBOOT_BACKUP_VER, NULL, 0);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:Uboot redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));
    

    
    ret = add_uri_to_members(o_result_jso, FI_ABMC_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:bmc add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    
    ret = add_uri_to_members(o_result_jso, FI_BBMC_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:bmc add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    
#ifdef ARM64_HI1711_ENABLED
    
    ret = add_uri_to_members(o_result_jso, FI_AVAILBMC_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:bmc add_uri_to_members fail.\n", __FUNCTION__, __LINE__));
#endif
    

    
    ret = add_uri_to_members(o_result_jso, FI_AUBOOT_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:Uboot add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    
    ret = add_uri_to_members(o_result_jso, FI_BUBOOT_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:Uboot add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_firmwareinventory_object_sdcard_controller(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(CLASS_RAID_CHIP_NAME, 0, &obj_handle);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    
    ret = redfish_get_object_verison(CLASS_RAID_CHIP_NAME, obj_handle, PROPETRY_FX3S_VERSION, NULL, 0);
    
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d: redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));
    

    
    ret = add_uri_to_members(o_result_jso, FI_SDCARD_CONTROLLER_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:sd card add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_firmwareinventory_object_switch_fabric(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    gchar string_value[32] = {0};

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(CLASS_DFT_VERSION, &obj_list);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = redfish_get_object_verison(CLASS_DFT_VERSION, (OBJ_HANDLE)obj_node->data, PROPERTY_VERSION_STR, NULL, 0);
        check_success_continue(VOS_OK != ret);

        ret = dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_FABRIC_PLANE,
            &object_handle);
        if (ret == RET_OK) {
            (void)get_location_devicename(object_handle, string_value, sizeof(string_value));

            (void)add_uri_to_members(o_result_jso, string_value);
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return VOS_OK;
}


LOCAL gint32 get_firmwareinventory_object_ps(json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar ps_name[FI_NAME_LEN] = {0};
    gchar ps_location[FI_NAME_LEN] = {0};
    gchar member_id[FI_URI_LEN] = {0};

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = dfl_get_object_list(CLASS_NAME_PS, &obj_list);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        ret = redfish_get_object_verison(CLASS_NAME_PS, (OBJ_HANDLE)obj_node->data, PROTERY_PS_VERSION, NULL, 0);
        check_success_continue(VOS_OK != ret);

        (void)memset_s(ps_name, sizeof(ps_name), 0, sizeof(ps_name));
        (void)memset_s(ps_location, sizeof(ps_location), 0, sizeof(ps_location));
        (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));

        ret =
            dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROTERY_PS_DEVICENAME, ps_name, sizeof(ps_name));
        check_success_do_continue(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_string fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROTERY_PS_LOCATION, ps_location,
            sizeof(ps_location));
        check_success_do_continue(VOS_OK != ret,
            debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_string fail.\n", __FUNCTION__, __LINE__));

        dal_clear_string_blank(ps_name, sizeof(ps_name));
        dal_clear_string_blank(ps_location, sizeof(ps_location));

        // location + devicename
        iRet = snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s%s", ps_location, ps_name);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        ret = add_uri_to_members(o_result_jso, member_id);
        if (VOS_OK != ret) {
            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

            return VOS_ERR;
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return VOS_OK;
}
LOCAL gint32 get_firmwareinventory_object_vrd(json_object **o_result_jso)
{
    GSList* handle_list = NULL;
    GSList* list_item = NULL;
    OBJ_HANDLE obj_handle;
    gchar device_name[NAME_LEN] = {0};
    gchar member_id[FI_URI_LEN] = {0};
    gint32 already_success = RET_ERR;

    gint32 ret = dfl_get_object_list(CLASS_VRD_UPGRADE_MGNT, &handle_list);
    if (ret != RET_OK || handle_list == NULL) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed. ret = %d.", ret);
        return RET_ERR;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        ret = dfl_get_referenced_object((OBJ_HANDLE)list_item->data, PROPERTY_COMPONENT_OBJECT, &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object failed! obj name: %s, ret = %d.",
                dfl_get_object_name((OBJ_HANDLE)list_item->data), ret);
            continue;
        }
        (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        dal_clear_string_blank(device_name, sizeof(device_name));
        gint32 ret1 = sprintf_s(member_id, sizeof(member_id), "%sVRD", device_name);
        if (ret1 <= 0) {
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "sprintf_s fail, ret = %d.", ret1);
            continue;
        }
        ret = add_uri_to_members(o_result_jso, member_id);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "add_uri_to_members fail, ret = %d.", ret);
            continue;
        }
        already_success = RET_OK;
    }
    g_slist_free(handle_list);
    return (already_success == RET_OK) ? already_success : ret;
}

LOCAL void get_firmwareinventory_object_bbu(json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE object_bbu = 0;
    gchar member_id[FI_URI_LEN] = {0};

    ret = dfl_get_object_list(CLASS_BBU_MODULE_NAME, &obj_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: Get list of %s failed.", __FUNCTION__, CLASS_BBU_MODULE_NAME);
        return;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        object_bbu = (OBJ_HANDLE)obj_node->data;

        ret = redfish_get_object_verison(CLASS_BBU_MODULE_NAME, object_bbu, PROPERTY_BBU_MODULE_FIRMWARE_VER, NULL, 0);
        if (ret != VOS_OK) {
            continue;
        }

        ret = rf_gen_rsc_id_with_location_and_devicename(object_bbu, member_id, sizeof(member_id));
        if (ret != VOS_OK) {
            continue;
        }

        ret = add_uri_to_members(o_result_jso, member_id);
        if (ret != VOS_OK) {
            continue;
        }
    }

    g_slist_free(obj_list);

    return;
}



LOCAL gint32 get_firmwareinventory_object_lcd(json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(CLASS_LCD, 0, &obj_handle);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    
    ret = redfish_get_object_verison(CLASS_LCD, obj_handle, PROPERTY_LCD_VERSION, NULL, 0);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: redfish_get_object_verison fail.\n", __FUNCTION__, __LINE__));

    
    ret = add_uri_to_members(o_result_jso, FI_LCD_MEMBERID);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: lcd add_uri_to_members fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL void __add_peripheral_fw_object(json_object *o_result_jso)
{
    errno_t safe_fun_ret = EOK;
    GSList *peripheral_fw_list = NULL;
    GSList *peripheral_fw_iter = NULL;
    OBJ_HANDLE peripheral_fw_handle;
    gint32 ret;
    gchar               rsc_id[MAX_RSC_ID_LEN] = {0};
    gchar               fw_version[MAX_VER_INFO_LEN] = {0};
    const gchar *obj_name = NULL;
    gchar               rsc_uri[MAX_URI_LENGTH] = {0};

    ret = dfl_get_object_list(CLASS_PERIPHERAL_FIRMWARE_NAME, &peripheral_fw_list);
    return_if_fail(VOS_OK == ret);

    for (peripheral_fw_iter = peripheral_fw_list; NULL != peripheral_fw_iter;
        peripheral_fw_iter = peripheral_fw_iter->next) {
        peripheral_fw_handle = (OBJ_HANDLE)peripheral_fw_iter->data;

        obj_name = dfl_get_object_name(peripheral_fw_handle);
        continue_do_info_if_fail(NULL != obj_name, debug_log(DLOG_ERROR,
            "%s: cann't get object name for handle [%" OBJ_HANDLE_FORMAT "]", __FUNCTION__, peripheral_fw_handle));

        (void)memset_s(fw_version, sizeof(fw_version), 0, sizeof(fw_version));
        ret = dal_get_property_value_string(peripheral_fw_handle, PROPERTY_SOFTWARE_VERSIONSTRING, fw_version,
            sizeof(fw_version));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get version string failed for [%s]", __FUNCTION__, obj_name));

        
        ret = rf_string_check(fw_version);
        continue_if_fail(VOS_OK == ret);

        safe_fun_ret = memmove_s((void *)rsc_id, sizeof(rsc_id), RF_PREFIX_PERIPHERAL, strlen(RF_PREFIX_PERIPHERAL));
        do_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        (void)dal_get_property_value_string(peripheral_fw_handle, PROPERTY_SOFTWARE_NAME,
            rsc_id + strlen(RF_PREFIX_PERIPHERAL), sizeof(rsc_id) - strlen(RF_PREFIX_PERIPHERAL));

        (void)dal_clear_string_blank(rsc_id, sizeof(rsc_id));

        (void)memset_s((void *)rsc_uri, sizeof(rsc_uri), 0, sizeof(rsc_uri));
        ret = snprintf_s(rsc_uri, sizeof(rsc_uri), sizeof(rsc_uri) - 1, URI_FORMAT_FIRMWARE_INVENTORY, rsc_id);
        continue_do_info_if_fail(ret > 0,
            debug_log(DLOG_ERROR, "%s: format peripheral firmware uri failed, ret is %d", __FUNCTION__, ret));

        RF_ADD_ODATA_ID_MEMBER(o_result_jso, rsc_uri);
    }

    g_slist_free(peripheral_fw_list);

    return;
}


LOCAL void get_firmwareinventory_object_socboard(json_object **o_result_jso)
{
    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_SOC_BOARD, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_list fail, ret=%d", __FUNCTION__, ret);
        return;
    }
    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE soc_handle = (OBJ_HANDLE)node->data;
        GSList *firm_list = NULL;
        (void)dal_get_object_list_position(soc_handle, CLASS_UPGRADE_SOFTWARE_NAME, &firm_list);
        for (GSList *firm_node = firm_list; firm_node != NULL; firm_node = firm_node->next) {
            OBJ_HANDLE firm_handle = (OBJ_HANDLE)firm_node->data;
            if (!is_valid_board_firmver(firm_handle, BIOS_COMP_ID_EX_ATLAS)) {
                debug_log(DLOG_DEBUG, "%s: is_valid_board_firmver=FALSE", __FUNCTION__);
                continue;
            }
            gchar member_id[FI_URI_LEN] = {0};
            gint32 ret = get_soc_board_firm_memberid(firm_handle, member_id, FI_URI_LEN);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: get_soc_board_firm_memberid fail. ret=%d", __FUNCTION__, ret);
                continue;
            }
            ret = add_uri_to_members(o_result_jso, member_id);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: add_uri_to_members fail. ret=%d", __FUNCTION__, ret);
            }
        }
        g_slist_free(firm_list);
    }
    g_slist_free(obj_list);
}


LOCAL gint32 add_uri_to_members(json_object **o_result_jso, const gchar *member_id)
{
    int iRet;
    gint32 ret;
    json_object *obj_jso = NULL;
    json_object *obj_str = NULL;
    gchar fi_uri[FI_URI_LEN] = {0};

    iRet = snprintf_s(fi_uri, sizeof(fi_uri), sizeof(fi_uri) - 1, "%s%s", FI_CONST_URI, member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    obj_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == obj_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));
    obj_str = json_object_new_string(fi_uri);
    return_val_do_info_if_expr(NULL == obj_str, VOS_ERR, (void)json_object_put(obj_jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail.\n", __FUNCTION__, __LINE__));
    json_object_object_add(obj_jso, RFPROP_ODATA_ID, obj_str);
    ret = json_object_array_add(*o_result_jso, obj_jso);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR, (void)json_object_put(obj_jso); (void)json_object_put(obj_str);
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


gint32 get_soc_board_firm_memberid(OBJ_HANDLE firm_handle, gchar *buf, guint32 buf_len)
{
    OBJ_HANDLE comp_handle = 0;
    gint32 ret = dal_get_specific_position_object_byte(firm_handle, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
        COMPONENT_TYPE_SOC_BOARD, &comp_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    gchar loc_dev[FI_URI_LEN] = { 0 };
    ret = get_object_obj_location_devicename(comp_handle, loc_dev, FI_URI_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get location devicename fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
 
    guint16 instance_id = 0;
    (void)dal_get_property_value_uint16(firm_handle, PROPERTY_SOFTWARE_INSTANCEID, &instance_id);
    ret = sprintf_s(buf, buf_len, "%sBios%u", loc_dev, instance_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}
 

gboolean is_valid_board_firmver(OBJ_HANDLE obj_handle, const guint32 comp_idx)
{
    guint8 classification = 0;
    guint32 component_idex = 0;
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOFTWARE_CLASSIFICATIONS, &classification);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte fail, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SOFTWARE_COMPONENTIDEX, &component_idex);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32 fail, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }
    if (classification != COMP_BIOS || component_idex != comp_idx) {
        return FALSE;
    }
    gchar version[FWINV_STR_MAX_LEN] = { 0 };
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SOFTWARE_VERSIONSTRING, version, sizeof(version));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string fail, ret=%d", __FUNCTION__, ret);
        return FALSE;
    }
    dal_trim_string(version, FWINV_STR_MAX_LEN);
    return (check_string_val_effective(version) == RET_OK);
}


gint32 firmwareinventories_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    *prop_provider = g_firmwareinventories_provider;
    *count = sizeof(g_firmwareinventories_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 check_vrd_updateable(const gchar *member_id, gchar *class_name, guchar *out_val)
{
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    OBJ_HANDLE object_handle;
    OBJ_HANDLE component_object_handle = 0;
    gchar component_device_name[MAX_PROP_NAME] = {0};
    gchar resource_id[FI_URI_LEN] = {0};
    guchar byte_value = 0;
    gint32 ret;

    
    ret = dfl_get_object_list(CLASS_CHIP_UPGRADE_CFG, &handle_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, " Failed for dfl_get_object_list.");
        return ret;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        ret = dfl_get_referenced_object(object_handle, PROPERTY_FIRMWARE_COMPONENT_OBJECT, &component_object_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object failed! obj name: %s, ret = %d.",
                dfl_get_object_name(object_handle), ret);
            return ret;
        }
        (void)dal_get_property_value_string(component_object_handle, PROPERTY_COMPONENT_DEVICE_NAME,
            component_device_name, MAX_NAME_SIZE);
        dal_clear_string_blank(component_device_name, sizeof(component_device_name));
        ret = sprintf_s(resource_id, sizeof(resource_id), "%sVRD", component_device_name);
        if (ret <= 0) {
            g_slist_free(handle_list);
            debug_log(DLOG_ERROR, "sprintf_s fail, ret = %d.", ret);
            return ret;
        }
        if (g_ascii_strcasecmp(member_id, resource_id) == 0) {
            // memberid校验成功 获取当前vrd对象的 UpdateFlag 标志
            ret = dal_get_property_value_byte(object_handle, PROPERTY_FIRMWARE_UPDATE_FLAG, &byte_value);
            if (ret != RET_OK) {
                g_slist_free(handle_list);
                debug_log(DLOG_ERROR, "dal_get_property_value_byte fail! object: %s property: %s. ret = %d.",
                    dfl_get_object_name(object_handle), PROPERTY_FIRMWARE_UPDATE_FLAG, ret);
                return ret;
            }
            if (byte_value == 1) {
                *out_val = byte_value;
                g_slist_free(handle_list);
                return RET_OK; 
            }
        }
    }
    g_slist_free(handle_list);
    *out_val = byte_value; 
    return RET_OK;
}

gint32 get_power_updatebale(const gchar *member_id, gchar *class_name,
    OBJ_HANDLE resource_obj, guchar *byte_value)
{
    gint32 ret = 0;
    // 电源
    if (g_strcmp0(class_name, CLASS_NAME_PS) == 0) {
        
        ret = dal_get_property_value_byte(resource_obj, PROPETRY_PS_UPDATEFLAG, byte_value);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "dal_get_property_value_byte fail! object: %s property: %s. ret = %d.",
                dfl_get_object_name(resource_obj), PROPETRY_PS_UPDATEFLAG, ret);
        }
        
    }

    if (g_strcmp0(class_name, CLASS_VRD_UPGRADE_MGNT) == 0) {
        ret = check_vrd_updateable(member_id, class_name, byte_value); // 全部vrd升级才算升级成功
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "Not all VRDs are successfully updated.");
        }
    }
    return ret;
}
