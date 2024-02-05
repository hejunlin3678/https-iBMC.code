
#include "redfish_provider.h"
#include "redfish_provider_chassis.h"
#include "redfish_provider_update.h"

LOCAL gint32 get_firmwareinventory_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_updateable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_softwareid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_firmwareinventory_relateditem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_board_softwareid(OBJ_HANDLE firm_handle, json_object **o_result_jso,
    const gchar *class_name, const gchar *prop);
LOCAL gint32 get_board_mcu_softwareid(OBJ_HANDLE firm_handle, json_object **o_result_jso,
    const gchar *class_name, const gchar *prop);
LOCAL gint32 check_memberid_get_property(const gchar *member_id, gchar *class_name, gint32 name_len,
    OBJ_HANDLE *component_handle, OBJ_HANDLE *resource_obj);
LOCAL gint32 get_bbu_handle_by_memberid(const gchar *member_id, OBJ_HANDLE *bbu_handle);
LOCAL gint32 memberid_get_ps_handle(const gchar *member_id, OBJ_HANDLE *ps_handle);
LOCAL gint32 memberid_get_retimer_handle(const gchar *member_id, OBJ_HANDLE *retimer_handle);
LOCAL gint32 memberid_get_vrd_handle(const gchar *member_id, OBJ_HANDLE *vrd_handle);

LOCAL gint32 get_oem_manufacturer(PROVIDER_PARAS_S *i_paras, json_object *obj_jso_huawei);
LOCAL gint32 get_oem_bios_component(PROVIDER_PARAS_S *i_paras, json_object *obj_jso_huawei);
LOCAL gint32 get_oem_active_mode(PROVIDER_PARAS_S *i_paras, json_object *obj_jso);

typedef struct gethandle {
    gchar* class_name;
    gchar* property_name;
    gint32 (*get_handle_by_memberid)(const gchar *data, OBJ_HANDLE* i_resource_obj);
} GetHandleById;

typedef gint32 (*GetOemFun)(PROVIDER_PARAS_S *i_paras, json_object *obj_jso_huawei);

LOCAL GetOemFun g_get_oem_functions[] = {
    get_oem_manufacturer,
    get_oem_bios_component,
    get_oem_active_mode
};

LOCAL GetHandleById g_handle_by_member[] = {
    {CLASS_NAME_PS, PROPERTY_FIRMWARE_LOGICVER, memberid_get_ps_handle},
    {CLASS_VRD_UPGRADE_MGNT, PROPERTY_VRDVERSION, memberid_get_vrd_handle},
    {CLASS_BBU_MODULE_NAME, PROPERTY_BBU_MODULE_FIRMWARE_VER, get_bbu_handle_by_memberid},
    {CLASS_RETIMER_NAME, PROPERTY_RETIMER_SOFT_VER, memberid_get_retimer_handle},
};

LOCAL PROPERTY_PROVIDER_S g_firmwareinventory_provider[] = {
    {RFPROP_FWINV_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_version, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_UPDATEABLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_updateable, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_SOFTWAREID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_softwareid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_FWINV_RELATEDITEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_firmwareinventory_relateditem, NULL, NULL, ETAG_FLAG_ENABLE}
};

typedef enum {
    VAL_UINT32 = 0,
    VAL_STRING
} PROP_VAL_TYPE;

typedef struct ver_obj_set {
    const gchar *obj_property;
    const gchar *obj_uri;
} VER_OBJ_SET;




char* classinfo_map[][CLS_INFO_MAX_SIZE] = {
    {URI_OBJ_ACTIVEBMC,             OBJ_NAME_BMC,                  SW_ID_BMC,               SW_ID_NULL,                  URI_SUFFIX_NULL,                      NULL},
    {URI_OBJ_BACKUPBMC,             OBJ_NAME_BMC,                  SW_ID_BMC,               SW_ID_NULL,                  URI_SUFFIX_NULL,                      NULL},
    {URI_OBJ_AVAILABLEBMC,          OBJ_NAME_BMC,                  SW_ID_BMC,               SW_ID_NULL,                  URI_SUFFIX_NULL,                      NULL},
    {URI_OBJ_ACTIVEUBOOT,           OBJ_NAME_BMC,                  SW_ID_UBOOT,             SW_ID_NULL,                  URI_SUFFIX_NULL,                      NULL},
    {URI_OBJ_BACKUPUBOOT,           OBJ_NAME_BMC,                  SW_ID_UBOOT,             SW_ID_NULL,                  URI_SUFFIX_NULL,                      NULL},
    {URI_OBJ_MAINBOARDCPLD,         CLASS_MAINBOARD_NAME,          SW_ID_CPLD,              PROPERTY_MAINBOARD_NAME,     URI_SUFFIX_NULL,  MAINBOARDCPLD_DEVICETYPE},
    {NULL,                          CLASS_VRD_UPGRADE_MGNT,        SW_ID_VRD,               PROPERTY_COMPONENT_DEVICE_NAME, URI_SUFFIX_NULL,                   NULL},
    {URI_OBJ_MAINBOARDCPLD2,        CLASS_VRD_UPGRADE_MGNT,        SW_ID_CPLD2,             SW_ID_NULL,                  URI_SUFFIX_NULL,  MAINBOARDCPLD_DEVICETYPE},
    {URI_OBJ_BIOS,                  OBJ_NAME_BIOS,                 SW_ID_BIOS,              SW_ID_NULL,                  URI_SUFFIX_NULL,                      NULL},
    {URI_OBJ_IMU,                   OBJ_NAME_BIOS,                 SW_ID_IMU,               SW_ID_NULL,
        URI_SUFFIX_NULL,  NULL},
    {URI_OBJ_LCD,                   OBJECT_LCD,                    SW_ID_LCD,               SW_ID_NA,                    URI_SUFFIX_NULL,                      NULL},
    {NULL,                          CLASS_NAME_PS,                 SW_ID_PSU,               PROTERY_PS_RIMM_MODEL,       URI_SUFFIX_NULL,             PS_DEVICETYPE},
    {URI_OBJ_SDCARD_CONTROLLER,     CLASS_RAID_CHIP_NAME,          SW_ID_SDCARD_CONTROLLER, SW_ID_NA,                    URI_SUFFIX_NULL,                      NULL},
    {NULL,                          CLASS_CPUBOARD_NAME,           SW_ID_CPLD,              PROPERTY_CPUBOARD_NAME,      URI_SUFFIX_CPLD,       CPUBOARD_DEVICETYPE},
    {NULL,                          CLASS_CPUBOARD_NAME,           SW_ID_IOCTRL,            PROPERTY_CPUBOARD_NAME,      URI_SUFFIX_IOCTRL,     CPUBOARD_DEVICETYPE},
    {NULL,                          CLASS_MEMBOARD_NAME,           SW_ID_CPLD,              PROPERTY_MEMBOARD_NAME,      URI_SUFFIX_CPLD,       MEMBOARD_DEVICETYPE},
    {NULL,                          CLASS_HDDBACKPLANE_NAME,       SW_ID_CPLD,              PROPERTY_HDDBACKPLANE_NAME,  URI_SUFFIX_CPLD,   HDDBACKPLANE_DEVICETYPE},
    {NULL,                          CLASS_IOBOARD_NAME,            SW_ID_CPLD,              PROPERTY_IOBOARD_NAME,       URI_SUFFIX_CPLD,        IOBOARD_DEVICETYPE},
    {NULL,                          CLASS_RISERPCIECARD_NAME,      SW_ID_CPLD,              PROPERTY_RISERPCIECARD_NAME, URI_SUFFIX_CPLD,      PCIERISER_DEVICETYPE},
    {NULL,                          CLASS_FANBOARD_NAME,           SW_ID_CPLD,              PROPERTY_FANBOARD_NAME,      URI_SUFFIX_CPLD,       FANBOARD_DEVICETYPE},
    {NULL,                          CLASS_EXPBOARD_NAME,           SW_ID_CPLD,              PROPERTY_EXPBOARD_NAME,      URI_SUFFIX_CPLD,       EXPBOARD_DEVICETYPE},
    {NULL,                          CLASS_FANBOARD_NAME,           SW_ID_CPLD,              PROPERTY_FANBOARD_NAME,      URI_SUFFIX_CPLD,  EXPENSION_MOD_DEVICETYPE},
    
    
    {NULL,                          CLASS_PCIECARD_NAME,           SW_ID_CPLD,            PROPERTY_CARD_BOARDNAME,       URI_SUFFIX_CPLD,       PCIERAID_DEVICETYPE},
    
    {NULL,                          CLASS_RAIDCARD_NAME,           SW_ID_CPLD,            PROPERTY_CARD_BOARDNAME,       URI_SUFFIX_CPLD,           RAID_DEVICETYPE},
    {NULL,                          CLASS_DFT_VERSION,             SW_ID_FABRIC_PLANE,    SW_ID_NULL,                    URI_SUFFIX_CPLD,     SWITCHPLANE_DEVICETYPE},
    
    
    {NULL,                          CLASS_GPUBOARD_NAME,           SW_ID_CPLD,              PROPERTY_GPUBOARD_NAME,      URI_SUFFIX_CPLD,       GPUBOARD_DEVICETYPE},
    
    {URI_OBJ_CDR,                   CLASS_RETIMER_NAME,            SW_ID_CDR,               SW_ID_NULL,                  URI_SUFFIX_CDR,                       NULL},
    {URI_OBJ_RETIMER,               CLASS_RETIMER_NAME,            SW_ID_RETIMER,           PROPERTY_RETIMER_LOCATION,   URI_SUFFIX_RETIMER,                   NULL},
    {URI_OBJ_HWSR,                  CLASS_CPUBOARD_NAME,           SW_ID_HWSR,              PROPERTY_CPUBOARD_NAME,      URI_SUFFIX_HWSR,       CPUBOARD_DEVICETYPE},
    {URI_OBJ_HWSR,                  CLASS_HDDBACKPLANE_NAME,       SW_ID_HWSR,              PROPERTY_HDDBACKPLANE_NAME,  URI_SUFFIX_HWSR,   HDDBACKPLANE_DEVICETYPE},
    {URI_OBJ_HWSR,                  CLASS_RISERPCIECARD_NAME,      SW_ID_HWSR,              PROPERTY_RISERPCIECARD_NAME, URI_SUFFIX_HWSR,      PCIERISER_DEVICETYPE},
    {URI_OBJ_HWSR,                  CLASS_FANBOARD_NAME,           SW_ID_HWSR,              PROPERTY_FANBOARD_NAME,      URI_SUFFIX_HWSR,       FANBOARD_DEVICETYPE},
    {URI_OBJ_HWSR,                  CLASS_EXPBOARD_NAME,           SW_ID_HWSR,              PROPERTY_EXPBOARD_NAME,      URI_SUFFIX_HWSR,       EXPBOARD_DEVICETYPE},
    {NULL,                          CLASS_BBU_MODULE_NAME,         SW_ID_BBU,        PROPERTY_BBU_MODULE_BOARDNAME,       URI_SUFFIX_NULL,            BBU_DEVICETYPE},
    
    { URI_OBJ_HMM1_AC_UBOOT,            OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM1_BK_UBOOT,            OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM1_AC_SOFTWARE,         OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM1_BK_SOFTWARE,         OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
#ifdef ARM64_HI1711_ENABLED
    { URI_OBJ_HMM1_AV_SOFTWARE,         OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
#endif
    { URI_OBJ_HMM1_CPLD,                OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM2_AC_UBOOT,            OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM2_BK_UBOOT,            OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM2_AC_SOFTWARE,         OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
    { URI_OBJ_HMM2_BK_SOFTWARE,         OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
#ifdef ARM64_HI1711_ENABLED
    { URI_OBJ_HMM2_AV_SOFTWARE,         OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },
#endif
    { URI_OBJ_HMM2_CPLD,                OBJ_NAME_BMC,             NULL,    NULL,                    NULL,     NULL },

};


#define CHIP_TYPE_NORMAL 0
#define CHIP_TYPE_CPLD 1
#define CHIP_TYPE_CDR 2
#define CHIP_TYPE_RETIMER 3
#define CHIP_TYPE_HWSR 4
#define CHIP_TYPE_IOCTRL 5

typedef gint32 (*GET_FIRMWARE_SOFTWAREID_FUNC)(OBJ_HANDLE firm_handle, json_object **o_result_jso,
    const gchar *class_name, const gchar *prop);

typedef struct {
    gchar firm_handle[MAX_CLASS_NAME];
    gchar prop[MAX_CLASS_NAME];
    GET_FIRMWARE_SOFTWAREID_FUNC func;
} GET_FIRMWARE_SOFTWAREID_MAP;

// 不同类型softwareid获取函数列表
LOCAL const GET_FIRMWARE_SOFTWAREID_MAP get_softwareid_map[] = {
    {CLASS_SOC_BOARD, PROPERTY_SOC_BOARD_BOARDNAME, get_board_softwareid},
    {CLASS_IOBOARD_NAME, PROPERTY_IOBOARD_NAME, get_board_softwareid},
    {CLASS_UPGRADE_MCU_SOFTWARE_NAME, PROPERTY_SOFTWARE_NAME, get_board_mcu_softwareid}
};


LOCAL gboolean is_in_set_case(const gchar *str, const gchar *set[], guint32 len)
{
    for (guint32 i = 0; i < len; i++) {
        if (g_ascii_strcasecmp(str, set[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL gint32 get_obj_prop_val(const gchar *obj_name, const gchar *prop_name, void *val, gint32 size,
    PROP_VAL_TYPE val_type)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    
    // MeInfo类可能有后缀，取唯一的对象
    if (!g_ascii_strcasecmp(obj_name, CLASS_ME_INFO)) {
        ret = dal_get_object_handle_nth(obj_name, 0, &obj_handle);
    } else {
        ret = dfl_get_object_handle(obj_name, &obj_handle);
    }
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_handle %s fail.\n", __FUNCTION__, __LINE__, obj_name));

    switch (val_type) {
        case VAL_UINT32:
            ret = dal_get_property_value_uint32(obj_handle, prop_name, (guint32 *)val);
            break;

        case VAL_STRING:
            ret = dal_get_property_value_string(obj_handle, prop_name, (gchar *)val, size);
            break;

        default:
            ret = VOS_ERR;
            break;
    }

    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d: get property %s fail.\n", __FUNCTION__, __LINE__, prop_name));
    

    return VOS_OK;
}


LOCAL gint32 get_pcie_firm_unit_num(OBJ_HANDLE obj_handle, guint32 *val)
{
    OBJ_HANDLE refer_handle = 0;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_referenced_object fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_uint32(refer_handle, PROPERTY_UNIT_NUM, val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32 fail, ret=%d", __FUNCTION__, ret);
    }
    return ret;
}

LOCAL gint32 get_other_firmware_property_logic_unit(OBJ_HANDLE resource_handle, void *val, const gchar* class_name)
{
    gint32 ret = 0;
    
    
    if (0 == g_strcmp0(class_name, CLASS_RAIDCARD_NAME) || 0 == g_strcmp0(class_name, CLASS_PCIECARD_NAME)) {
        if (get_pcie_firm_unit_num(resource_handle, (guint32 *)val) != RET_OK) {
            return RET_ERR;
        }
    }
    
    else if (0 == g_strcmp0(class_name, CLASS_DFT_VERSION)) {
        ret = dal_get_property_value_uint32(resource_handle, PROPERTY_UNIT_NUM, (guint32 *)val);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s:  dal_get_property_value_uint32 fail", __FUNCTION__));
    } else if (0 == g_strcmp0(class_name, CLASS_NAME_PS)) {
        // 电源填null
        return VOS_ERR;
    } else {
        
        ret = dal_get_property_value_uint32(resource_handle, PROPERTY_FIRMWARE_LOGICUNIT, (guint32 *)val);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %s, get prop error.", __FUNCTION__, PROPERTY_FIRMWARE_LOGICUNIT));
    }
    return VOS_OK;
}


LOCAL gint32 get_other_firmware_property(const gchar *str_member_id, const gchar *prop_name, void *val, guint32 size)
{
    gchar class_name[FWINV_STR_MAX_LEN] = {0};
    OBJ_HANDLE resource_handle = 0;

    return_val_do_info_if_fail(NULL != str_member_id && NULL != prop_name && NULL != val, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: parameter is NULL.", __FUNCTION__, __LINE__));

    gint32 ret = check_memberid_get_property(str_member_id, class_name, sizeof(class_name), NULL, &resource_handle);
    if (ret != RET_OK) {
        
        debug_log(DLOG_DEBUG, "%s, %d, check_memberid_get_property fail.", __FUNCTION__, __LINE__);
        
        return VOS_ERR;
    }
    if (g_strcmp0(class_name, CLASS_SOC_BOARD) == 0 || g_strcmp0(class_name, CLASS_IOBOARD_NAME) == 0) {
        ret = dal_get_property_value_uint32(resource_handle, PROPERTY_UNIT_NUM, (guint32 *)val);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint32 fail, ret=%d", __FUNCTION__, ret);
        }
        return ret;
    }

    
    if (!g_strcmp0(prop_name, PROPERTY_FIRMWARE_LOGICUNIT)) {
        return get_other_firmware_property_logic_unit(resource_handle, val, class_name);
    }

    if (!g_strcmp0(prop_name, PROTERY_PS_VERSION)) {
        ret = redfish_get_ps_verison(resource_handle, (gchar *)val, size);
        return_val_if_fail(VOS_OK == ret, VOS_ERR);
        return VOS_OK;
    }

    
    
    if (0 == g_strcmp0(prop_name, URI_CLASS_RAID) &&
        (0 == g_strcmp0(class_name, CLASS_RAIDCARD_NAME) || 0 == g_strcmp0(class_name, CLASS_PCIECARD_NAME))) {
        ret = redfish_get_firmware_version(resource_handle, (gchar *)val, size);
        return_val_if_fail(VOS_OK == ret, VOS_ERR);
        return VOS_OK;
    }

    

    ret = dal_get_property_value_string(resource_handle, prop_name, (gchar *)val, size);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: %s, get prop error.", __FUNCTION__, __LINE__, prop_name));

    return VOS_OK;
}


LOCAL gint32 format_str_opt_prefix(const gchar *id_prefix, const gchar *id, gchar *str_val, gint32 str_len)
{
    gint32 ret;
    if (id_prefix != NULL) {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s%s", id_prefix, id);
    } else {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s", id);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 get_firmware_info_by_splice_location_and_devicename(const gchar *id_prefix, OBJ_HANDLE object_handle,
    gchar *output_str, guint32 str_len)
{
    gchar resource_id[MAX_SOFTWARE_ID_LEN] = {0};

    gint32 ret = rf_gen_rsc_id_with_location_and_devicename(object_handle, resource_id, sizeof(resource_id));
    if (ret != VOS_OK) {
        return RET_ERR;
    }

    if (str_len < 1) {
        debug_log(DLOG_ERROR, "%s: str_len is error, str_len = %u", __FUNCTION__, str_len);
        return RET_ERR;
    }

    return format_str_opt_prefix(id_prefix, resource_id, output_str, str_len);
}

LOCAL gint32 joint_assemble_id_get_str_val(gchar *class_name, const gchar *id_prefix, const gchar *str_member_id,
    OBJ_HANDLE component_handle, gchar *str_val, gint32 str_len)
{
    gint32 ret;
    gchar device_name[MAX_PROP_NAME] = {0};
    gchar location[MAX_PROP_NAME] = {0};

    (void)dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
        sizeof(device_name));
    (void)dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_LOCATION, location, sizeof(location));

    dal_clear_string_blank(device_name, sizeof(device_name));
    dal_clear_string_blank(location, sizeof(location));

    if (dal_str_has_casesuffix(str_member_id, URI_SUFFIX_HWSR) != NULL) {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s%sHWSR", location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        return RET_OK;
    } else if (g_strcmp0(class_name, CLASS_CPUBOARD_NAME) == 0 &&
               dal_str_has_casesuffix(str_member_id, FI_IOCTRL_MEMBERID) != NULL) {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s%s%s"URI_SUFFIX_IOCTRL,
            id_prefix ? id_prefix : "", location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        return RET_OK;
    }

    if (id_prefix != NULL) {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s%s%sCPLD", id_prefix, location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    } else {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s%sCPLD", location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    }

    return RET_OK;
}

LOCAL gint32 get_io_board_firm_id(const gchar *id_prefix, OBJ_HANDLE firm_handle, gchar *str_val, gint32 str_len)
{
    gchar buf[FI_URI_LEN] = {0};
    gint32 ret = get_io_board_firm_memberid(firm_handle, buf, FI_URI_LEN);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return format_str_opt_prefix(id_prefix, buf, str_val, str_len);
}

LOCAL gint32 get_mcu_board_firm_id(const gchar *id_prefix, OBJ_HANDLE firm_handle, gchar *str_val, gint32 str_len)
{
    gchar buf[FI_URI_LEN] = {0};
    gint32 ret = get_mcu_board_firm_memberid(firm_handle, buf, FI_URI_LEN);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return format_str_opt_prefix(id_prefix, buf, str_val, str_len);
}
 

LOCAL gint32 get_soc_board_firm_id(const gchar *id_prefix, OBJ_HANDLE firm_handle, gchar *str_val, gint32 str_len)
{
    gchar buf[FI_URI_LEN] = {0};
    gint32 ret = get_soc_board_firm_memberid(firm_handle, buf, FI_URI_LEN);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return format_str_opt_prefix(id_prefix, buf, str_val, str_len);
}

LOCAL gint32 get_smm_fan_memberid(OBJ_HANDLE fan_class, gchar *memberid, gint32 len)
{
    guchar slot = 0;
    gint32 ret = dal_get_property_value_byte(fan_class, PROPERTY_FAN_SLOT, &slot);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = snprintf_s(memberid, len, len - 1, "Fantray%u", slot);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 get_smm_fan_id(const gchar *id_prefix, OBJ_HANDLE fan_class, gchar *str_val, gint32 str_len)
{
    gchar memberid[FI_URI_LEN] = {0};
    gint32 ret = get_smm_fan_memberid(fan_class, memberid, FI_URI_LEN);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return format_str_opt_prefix(id_prefix, memberid, str_val, str_len);
}


LOCAL gint32 memberid_get_vrd_handle(const gchar *member_id, OBJ_HANDLE *vrd_handle)
{
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar device_name[MAX_PROP_NAME] = {0};
    gchar resource_id[FI_URI_LEN] = {0};

    
    gint32 ret = dfl_get_object_list(CLASS_VRD_UPGRADE_MGNT, &handle_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed for dfl_get_object_list. class name:%s. ret=%d.", CLASS_VRD_UPGRADE_MGNT, ret);
        return ret;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        ret = dfl_get_referenced_object((OBJ_HANDLE)list_item->data, PROPERTY_COMPONENT_OBJECT, &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object failed. obj name:%s. ret=%d.",
                dfl_get_object_name((OBJ_HANDLE)list_item->data), ret);
            continue;
        }
        (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME,
            device_name, sizeof(device_name));

        dal_clear_string_blank(device_name, sizeof(device_name));
        ret = sprintf_s(resource_id, sizeof(resource_id), "%sVRD", device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "sprintf_s fail, ret = %d.", ret);
            continue;
        }
        if (g_ascii_strcasecmp(member_id, resource_id) == 0) {
            *vrd_handle = (OBJ_HANDLE)list_item->data;
            g_slist_free(handle_list);
            return RET_OK;
        }
    }
    g_slist_free(handle_list);
    return RET_ERR;
}

LOCAL gint32 get_vrd_assemble_id(const gchar *id_prefix, const gchar *str_member_id, gchar *str_val, gint32 str_len)
{
    gint32 ret;
    if (id_prefix != NULL) {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s%s", id_prefix, str_member_id);
    } else {
        ret = snprintf_s(str_val, str_len, str_len - 1, "%s", str_member_id);
    }
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "sprintf_s fail, ret = %d.", ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 assemble_id(const gchar *id_prefix, const gchar *str_member_id, gchar *str_val, gint32 str_len)
{
    gchar class_name[MAX_PROP_NAME] = {0};
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE resource_obj = 0;

    do_val_if_expr(NULL == str_member_id || NULL == str_val || str_len < 1, return VOS_ERR);

    // memberid为固定值 从classinfo_map中取值
    for (guint32 i = 0; i < G_N_ELEMENTS(classinfo_map); i++) {
        if (NULL != classinfo_map[i][0] && 0 == g_ascii_strcasecmp(str_member_id, classinfo_map[i][0])) {
            (void)format_str_opt_prefix(id_prefix, classinfo_map[i][0], str_val, str_len);
            return VOS_OK;
        }
    }
    // memberid为拼接值
    if (check_memberid_get_property(str_member_id, class_name, sizeof(class_name),
        &component_handle, &resource_obj) == RET_ERR) {
        
        debug_log(DLOG_DEBUG, "%s, %d, check_memberid_get_property fail.", __FUNCTION__, __LINE__);
        
        return VOS_ERR;
    }

    // 电源
    if (g_strcmp0(class_name, CLASS_NAME_PS) == 0 || g_strcmp0(class_name, CLASS_BBU_MODULE_NAME) == 0) {
        (void)get_firmware_info_by_splice_location_and_devicename(id_prefix, resource_obj, str_val, str_len);
    } else if (g_strcmp0(class_name, CLASS_RETIMER_NAME) == 0) {
        guint8 retimer_type = 0xff;
        (void)dal_get_property_value_byte(resource_obj, PROPERTY_RETIMER_CHIP_TYPE, &retimer_type);
        (void)gen_firmwareinventory_retimer_rsc_id(resource_obj, NULL, str_val, str_len, retimer_type);
    }
    // Fabric Plane
    else if (g_strcmp0(class_name, CLASS_DFT_VERSION) == 0) {
        (void)get_firmware_info_by_splice_location_and_devicename(id_prefix, component_handle, str_val, str_len);
    }
    
    // Fantray
    else if (0 == g_strcmp0(class_name, CLASS_NAME_SMM_FAN)) {
        return get_smm_fan_id(id_prefix, resource_obj, str_val, str_len);
    } else if (g_strcmp0(class_name, CLASS_SOC_BOARD) == 0) {
        return get_soc_board_firm_id(id_prefix, resource_obj, str_val, str_len);
    } else if (g_strcmp0(class_name, CLASS_IOBOARD_NAME) == 0) {
        return get_io_board_firm_id(id_prefix, resource_obj, str_val, str_len);
    } else if (g_strcmp0(class_name, CLASS_UPGRADE_MCU_SOFTWARE_NAME) == 0) {
        return get_mcu_board_firm_id(id_prefix, resource_obj, str_val, str_len);
    }
    
    else if (g_strcmp0(class_name, CLASS_VRD_UPGRADE_MGNT) == 0) {
        return get_vrd_assemble_id(id_prefix, str_member_id, str_val, str_len);
    } else {
        return joint_assemble_id_get_str_val(class_name, id_prefix, str_member_id, component_handle, str_val, str_len);
    }
    return VOS_OK;
}


LOCAL gint32 get_handle_if_match_io_board(const gchar *member_id, OBJ_HANDLE *out_fw_hd, OBJ_HANDLE *out_comp_hd)
{
    GSList *firm_list = NULL;
    (void)dfl_get_object_list(CLASS_UPGRADE_SOFTWARE_NAME, &firm_list);
    for (GSList *node = firm_list; node != NULL; node = node->next) {
        OBJ_HANDLE firm_handle = (OBJ_HANDLE)node->data;
        if (!is_valid_board_firmver(firm_handle, BIOS_COMP_ID_EX_1601)) {
            continue;
        }
        gchar firm_id[FI_URI_LEN] = {0};
        OBJ_HANDLE comp_handle = 0;
        gint32 ret = get_io_board_firm_memberid(firm_handle, firm_id, FI_URI_LEN);
        if (ret != RET_OK || g_ascii_strcasecmp(member_id, firm_id) != 0) {
            continue;
        }
        ret = dal_get_specific_position_object_byte(firm_handle, CLASS_COMPONENT,
            PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_IO_BOARD, &comp_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_specific_position_object_byte fail, ret=%d", __FUNCTION__, ret);
            break;
        }
        if (out_fw_hd != NULL) {
            *out_fw_hd = firm_handle;
        }
        if (out_comp_hd != NULL) {
            *out_comp_hd = comp_handle;
        }
        g_slist_free(firm_list);
        return RET_OK;
    }
    g_slist_free(firm_list);
    return RET_ERR;
}

LOCAL gint32 get_vrd_component_name(const gchar *memberid, gchar *device_name, guint32 prop_name_len)
{
    OBJ_HANDLE object_handle;
    OBJ_HANDLE obj_component_handle;

    gint32 ret = memberid_get_vrd_handle(memberid, &object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "memberid_get_vrd_handle failed! ret = %d.", ret);
    }
    ret = dfl_get_referenced_object(object_handle, PROPERTY_COMPONENT_OBJECT, &obj_component_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object failed! obj name: %s, ret = %d.",
            dfl_get_object_name(object_handle), ret);
        return ret;
    }
    (void)dal_get_property_value_string(obj_component_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
        prop_name_len);
    return RET_OK;
}

LOCAL gint32 get_handle_if_match_mcu_board(const gchar *member_id, OBJ_HANDLE *out_fw_hd)
{
    GSList *mcu_firm_list = NULL;
    gint32 ret;
 
    (void)dfl_get_object_list(CLASS_UPGRADE_MCU_SOFTWARE_NAME, &mcu_firm_list);
    for (GSList *node = mcu_firm_list; node != NULL; node = node->next) {
        OBJ_HANDLE mcubrd_handle = (OBJ_HANDLE)node->data;
        gchar firm_id[FI_URI_LEN] = { 0 };
        ret = get_mcu_board_firm_memberid(mcubrd_handle, firm_id, FI_URI_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: get mcuboard firm_id fail, ret=%d", __FUNCTION__, ret);
            continue;
        }
        if (g_ascii_strcasecmp(member_id, firm_id) != 0) {
            continue;
        }
        if (out_fw_hd != NULL) {
            *out_fw_hd = mcubrd_handle;
        }
        g_slist_free(mcu_firm_list);
        return RET_OK;
    }
    g_slist_free(mcu_firm_list);
    return RET_ERR;
}
 

LOCAL gint32 get_softwareid_info(const gchar *rsc_id, const gchar *class_name, gchar *software_id_pattern,
    guint32 software_id_pattern_len, gchar *software_id_prop_name, guint32 prop_name_len)
{
    errno_t safe_fun_ret = EOK;
    guint32 array_size = G_N_ELEMENTS(classinfo_map);

    // 固定值获取 固定值class_name有重复
    for (guint32 i = 0; i < array_size; i++) {
        
        if (g_str_case_rstr(rsc_id, classinfo_map[i][CLS_INFO_NAME])) {
            safe_fun_ret = strncpy_s(software_id_pattern, software_id_pattern_len,
                classinfo_map[i][CLS_INFO_SW_ID_PATTERN], strlen(classinfo_map[i][CLS_INFO_SW_ID_PATTERN]));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            safe_fun_ret = strncpy_s(software_id_prop_name, prop_name_len, classinfo_map[i][CLS_INFO_SW_ID_PROP_NAME],
                strlen(classinfo_map[i][CLS_INFO_SW_ID_PROP_NAME]));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            return VOS_OK;
        }
    }

    // 拼接值获取
    for (guint32 i = 0; i < array_size; i++) {
        
        if (g_str_case_rstr(class_name, classinfo_map[i][CLS_INFO_REF_NAME])
            && (g_strcmp0(classinfo_map[i][CLS_INFO_URI_SUFFIX], URI_SUFFIX_NULL) == 0
            || dal_str_has_casesuffix(rsc_id, classinfo_map[i][CLS_INFO_URI_SUFFIX]) != NULL)) {
            safe_fun_ret = strncpy_s(software_id_pattern, software_id_pattern_len,
                classinfo_map[i][CLS_INFO_SW_ID_PATTERN], strlen(classinfo_map[i][CLS_INFO_SW_ID_PATTERN]));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            safe_fun_ret = strncpy_s(software_id_prop_name, prop_name_len, classinfo_map[i][CLS_INFO_SW_ID_PROP_NAME],
                strlen(classinfo_map[i][CLS_INFO_SW_ID_PROP_NAME]));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

            return VOS_OK;
        }
    }

    return VOS_ERR;
}


LOCAL gint32 get_firmwareinventory_id_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar *id_prefix)
{
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    
    if (o_message_jso == NULL || o_result_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = assemble_id(id_prefix, i_paras->member_id, string_value, sizeof(string_value));
    if (VOS_OK != ret) {
        
        debug_log(DLOG_DEBUG, "%s, %d: assemble_id fail.", __FUNCTION__, __LINE__);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return HTTP_OK;
}


LOCAL gint32 get_firmwareinventory_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_firmwareinventory_id_odataid(i_paras, o_message_jso, o_result_jso, FI_CONST_URI);
    }


LOCAL gint32 get_firmwareinventory_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_firmwareinventory_id_odataid(i_paras, o_message_jso, o_result_jso, NULL);
}

LOCAL gboolean is_manu_from_fru0(PROVIDER_PARAS_S *i_paras)
{
static const gchar* suffix_arr[] = {URI_CLASS_CPLD, URI_OBJ_CDR, URI_OBJ_RETIMER, URI_SUFFIX_IOCTRL};
static const gchar* uri_obj_arr[] = {URI_OBJ_ACTIVEBMC, URI_OBJ_BACKUPBMC, URI_OBJ_AVAILABLEBMC, URI_OBJ_ACTIVEUBOOT,
    URI_OBJ_BACKUPUBOOT, URI_OBJ_MAINBOARDCPLD, URI_OBJ_BASICBOARDCPLD, OBJ_NAME_BIOS, URI_OBJ_HMM1_CPLD,
    URI_OBJ_HMM2_CPLD, URI_OBJ_IMU};
    for (guint32 i = 0; i < G_N_ELEMENTS(suffix_arr); i++) {
        if (dal_str_has_casesuffix(i_paras->member_id, suffix_arr[i]) != NULL) {
            return TRUE;
        }
    }
    for (guint32 i = 0; i < G_N_ELEMENTS(uri_obj_arr); i++) {
        if (g_ascii_strcasecmp(i_paras->member_id, uri_obj_arr[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

LOCAL gint32 get_oem_manufacturer(PROVIDER_PARAS_S *i_paras, json_object *obj_jso_huawei)
{
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gchar fru0_manu[FRU_MAX_ELABEL_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = OBJ_HANDLE_COMMON;
    gchar class_name[MAX_NAME_SIZE] = {0};

    if (g_str_case_rstr(i_paras->member_id, RF_PREFIX_PERIPHERAL) != NULL) {
        (void)dal_get_property_value_string(i_paras->obj_handle, PROPETRY_MANUFACTURER, string_value,
            sizeof(string_value));
    } else if (is_manu_from_fru0(i_paras)) {
        
        (void)dfl_get_object_handle(OBJ_FRU0_ELABEL, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_MFG_NAME, fru0_manu, sizeof(fru0_manu));
        if (strncpy_s(string_value, sizeof(string_value), fru0_manu, strlen(fru0_manu)) != EOK) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else if (g_str_case_rstr(i_paras->member_id, RF_MCU_FIRMWARE) != NULL) {
        if (get_handle_if_match_mcu_board(i_paras->member_id, &obj_handle) == RET_OK) {
            (void)dal_get_property_value_string(obj_handle, PROPETRY_MANUFACTURER, string_value,
                sizeof(string_value));
        }
    } else {
        if (dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE) == DFL_OK) {
            const gchar *prop_name = (g_strcmp0(class_name, CLASS_UPGRADE_SOFTWARE_NAME) == 0) ? PROPETRY_MANUFACTURER :
                ((g_strcmp0(class_name, CLASS_BBU_MODULE_NAME) == 0) ?
                    PROPERTY_BBU_MODULE_BATT_MANU : PROTERY_PS_MANUFACTURER);
            (void)dal_get_property_value_string(i_paras->obj_handle, prop_name, string_value, sizeof(string_value));
        }
    }

    json_object_object_add(obj_jso_huawei, RFPROP_MANUFACTURE,
        (rf_string_check(string_value) == RET_OK ? json_object_new_string((const gchar *)string_value) : NULL));
    return HTTP_OK;
}

LOCAL gint32 get_oem_bios_component(PROVIDER_PARAS_S *i_paras, json_object *obj_jso_huawei)
{
    // 如果不是BIOS，跳过即可，无需返回错误
    if (g_ascii_strcasecmp(i_paras->member_id, OBJ_NAME_BIOS)) {
        debug_log(DLOG_DEBUG, "%s: not bios resource.", __FUNCTION__);
        return HTTP_OK;
    }

    json_object *components = json_object_new_array();
    (void)json_object_object_add(obj_jso_huawei, RFPROP_FWINV_COMPONENT_INFO, components);

    GSList *output_list = NULL;
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_COMPONENTVERSION_NAME));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        BIOS_CLASS_NAME, METHOD_BIOS_GET_JSON_FILE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: call method %s failed, ret:%d", __FUNCTION__, METHOD_BIOS_GET_JSON_FILE, ret);
        uip_free_gvariant_list(output_list);
        return HTTP_OK;
    }

    const gchar *file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (file_data == NULL || strlen(file_data) == 0) {
        debug_log(DLOG_ERROR, "%s: file_data is NULL or empty.", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        return HTTP_OK;
    }
    json_object *component_info = json_tokener_parse(file_data);
    json_object *tmp_com_info = NULL;
    if (json_object_object_get_ex(component_info, RFPROP_FWINV_COMPONENT_INFO, &tmp_com_info) != TRUE) {
        debug_log(DLOG_ERROR, "%s: can not find component info.", __FUNCTION__);
        json_object_put(component_info);
        uip_free_gvariant_list(output_list);
        return HTTP_OK;
    }

    size_t tmp_com_size = json_object_array_length(tmp_com_info);
    for (size_t idx = 0; idx < tmp_com_size; idx++) {
        json_object *item = json_object_array_get_idx(tmp_com_info, idx);
        json_object_object_del(item, COMPONENT_BITMAP);
        json_object_array_add(components, json_object_get(item));
    }
    json_object_put(component_info);
    uip_free_gvariant_list(output_list);
    return HTTP_OK;
}


LOCAL gint32 get_oem_active_mode(PROVIDER_PARAS_S *i_paras, json_object *obj_jso)
{
    guint8 effective_mode = BMC_AUTO_VALID;
    guint8 current_active_mode = BMC_AUTO_VALID;

    if (obj_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_oem_active_mode NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_ACTIVEBMC)) {
        (void)dal_get_func_ability(CLASS_NAME_PME_PRODUCT, PROTERY_PRODUCT_BMC_VALID_MODE, &effective_mode);
        json_object_object_add(obj_jso, RF_IS_SUPPORT_OPTIONAL_UPGRADE_EFECTIVE_MODE,
            json_object_new_boolean(effective_mode == BMC_MANUAL_VALID));

        (void)dal_get_func_ability(PFN_CLASS_NAME, RF_BMC_CURRENT_ACTIVE_MODE, &current_active_mode);
        debug_log(DLOG_DEBUG, "%s: get current active mode from property is: %u\r\n", __FUNCTION__,
            current_active_mode);

        json_object_object_add(obj_jso, RF_BMC_CURRENT_ACTIVE_MODE, json_object_new_string(
            (current_active_mode == BMC_ACTIVE_MODE_RESET) ?
            RFACTION_BMC_ACTIVE_MODE_RESET : RFACTION_BMC_ACTIVE_MODE_IMMEDIATELY));
    } else {
        json_object_object_add(obj_jso, RF_IS_SUPPORT_OPTIONAL_UPGRADE_EFECTIVE_MODE, NULL);
        json_object_object_add(obj_jso, RF_BMC_CURRENT_ACTIVE_MODE, NULL);
    }

    return HTTP_OK;
}


LOCAL void add_oem_property_value(PROVIDER_PARAS_S* i_paras, json_object* obj_jso_huawei, gchar* property_name)
{
    gchar class_name[MAX_NAME_SIZE] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    gint32 ret = dfl_get_class_name(i_paras->obj_handle, class_name, MAX_NAME_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get class name fail. ret = %d", ret);
        return;
    }

    if (g_strcmp0(class_name, CLASS_NAME_PS) == 0) {
        ret = dal_get_property_value_string(i_paras->obj_handle, property_name, string_value, sizeof(string_value));
        if (ret == RET_OK) {
            json_object_object_add(obj_jso_huawei, RFPROP_POWER_TYPE,
                json_object_new_string((const gchar*)string_value));
        }
    }
}


LOCAL gint32 get_tmpfs_remaining_size(gulong *size)
{
#define TMP_RET_LEN 20
    const gchar *cmd = "du -sm /tmp | cut -d '/' -f 1"; 
    gchar stdout_data[TMP_RET_LEN] = {0}; 
    FILE *fp = popen(cmd, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s, Popen error.", __FUNCTION__);
        return RET_ERR;
    }

    gchar *data_ptr = fgets(stdout_data, TMP_RET_LEN, fp);
    pclose(fp);
    
    fp = NULL;

    if (data_ptr == NULL) { 
        return RET_ERR;
    }

    data_ptr = NULL;
    gulong value = strtoul(stdout_data, &data_ptr, DECIMAL_NUM);
    if (data_ptr == stdout_data) {
        debug_log(DLOG_ERROR, "data[%s]:strtoul failed", stdout_data);
        return RET_ERR;
    }

    if (errno == ERANGE) {
        debug_log(DLOG_ERROR, "value is out of range, value is %lu", value);
        return RET_ERR;
    }

    *size = value;
    return RET_OK;
}


LOCAL void add_upgrade_enough_property(PROVIDER_PARAS_S *i_paras, json_object* obj_jso_huawei)
{
    gulong size = 0;

    if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_ACTIVEBMC)) {
        if (get_tmpfs_remaining_size(&size) == RET_OK) {
            json_object_object_add(obj_jso_huawei, RFPROP_UPGRADE_ENOUGH,
                ((size > (RFPROP_TMP_SIZE - UPGRADE_THRESHOLD)) ?
                 json_object_new_string("false") : json_object_new_string("true")));
        } else {
            debug_log(DLOG_ERROR, "%s: get /tmp size fail.", __FUNCTION__);
        }
    }
}

LOCAL gint32 get_object_position_id(const gchar *member_id, gint32 null_flag, guint32 val_uint32,
    json_object **obj_jso_huawei)
{
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    json_object *obj_jso_pos = NULL;

    if (!null_flag) {
        
        
        if (!g_ascii_strcasecmp(member_id, OBJECT_LCD)) {
            (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "J%u", val_uint32);
        } else if (dal_str_has_casesuffix(member_id, URI_SUFFIX_HWSR) != NULL) {
            (void)memset_s(string_value, sizeof(string_value), 0, sizeof(string_value));
        
        } else if (dal_str_has_casesuffix(member_id, URI_SUFFIX_CDR) == NULL &&
            dal_str_has_casesuffix(member_id, URI_SUFFIX_RETIMER) == NULL) {
            (void)snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, "U%u", val_uint32);
        }

        obj_jso_pos = json_object_new_string((const gchar *)string_value);
        if (obj_jso_pos == NULL) {
            debug_log(DLOG_ERROR, "%s: json_object_new_string error.", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    json_object_object_add(*obj_jso_huawei, RFPROP_FWINV_POSITION_ID, obj_jso_pos);
    return RET_OK;
}


LOCAL gint32 get_firmwareinventory_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = VOS_OK;
    gint32 null_flag = 0;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    guint32 val_uint32 = 0;
    OBJ_HANDLE obj_handle;

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    const gchar *unit_flash_set[] = {
        URI_OBJ_ACTIVEBMC, URI_OBJ_BACKUPBMC, URI_OBJ_AVAILABLEBMC,
        URI_OBJ_HMM1_AC_SOFTWARE, URI_OBJ_HMM1_BK_SOFTWARE, URI_OBJ_HMM2_AC_SOFTWARE, URI_OBJ_HMM2_BK_SOFTWARE,
#ifdef ARM64_HI1711_ENABLED
        URI_OBJ_HMM1_AV_SOFTWARE, URI_OBJ_HMM2_AV_SOFTWARE
#endif
    };
    const gchar *unit_cpld_set[] = {URI_OBJ_MAINBOARDCPLD, URI_OBJ_HMM1_CPLD, URI_OBJ_HMM2_CPLD};
    if (is_in_set_case(i_paras->member_id, unit_flash_set, G_N_ELEMENTS(unit_flash_set))) {
        ret = get_obj_prop_val(OBJ_NAME_BMC, PROPERTY_BMC_FLASHUNITNUM, &val_uint32, sizeof(val_uint32), VAL_UINT32);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, OBJ_NAME_BIOS)) {
        ret = get_obj_prop_val(OBJ_NAME_BIOS, PROTERY_BIOS_UNITNUM, &val_uint32, sizeof(val_uint32), VAL_UINT32);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_IMU)) {
        ret = get_obj_prop_val(OBJ_NAME_BIOS, PROTERY_BIOS_UNITNUM, &val_uint32, sizeof(val_uint32), VAL_UINT32);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, OBJECT_LCD)) {
        ret = get_obj_prop_val(OBJECT_LCD, PROPERTY_LCD_UNITNUM, &val_uint32, sizeof(val_uint32), VAL_UINT32);
    } else if (is_in_set_case(i_paras->member_id, unit_cpld_set, G_N_ELEMENTS(unit_cpld_set))) {
        ret = get_obj_prop_val(OBJ_NAME_BMC, PROPERTY_BMC_CPLDUNITNUM, &val_uint32, sizeof(val_uint32), VAL_UINT32);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_BASICBOARDCPLD)) {
        ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &obj_handle);
        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_VRD_CPLD2_UNIT_NUMBER, &val_uint32);
    } else if (NULL != dal_str_has_casesuffix(i_paras->member_id, URI_CLASS_CPLD)) {
        ret = get_other_firmware_property(i_paras->member_id, PROPERTY_FIRMWARE_LOGICUNIT, &val_uint32,
            sizeof(val_uint32));
    } else if (NULL != g_str_case_rstr(i_paras->member_id, RF_MCU_FIRMWARE)) {
        ;
    } else if (dal_str_has_casesuffix(i_paras->member_id, URI_SUFFIX_CDR) != NULL) {
        ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_CHIP_TYPE, RETIMER_TYPE_5902L,
            &obj_handle);
        if (ret == VOS_OK) {
            (void)dal_get_property_value_string(obj_handle, PROPERTY_RETIMER_LOCATION, string_value,
                sizeof(string_value));
        }
    } else if (dal_str_has_casesuffix(i_paras->member_id, URI_SUFFIX_RETIMER) != NULL) {
        ret = dal_get_specific_object_byte(CLASS_RETIMER_NAME, PROPERTY_RETIMER_CHIP_TYPE, RETIMER_TYPE_5902H,
            &obj_handle);
        if (ret == VOS_OK) {
            dal_get_property_value_string(obj_handle, PROPERTY_RETIMER_LOCATION, string_value, sizeof(string_value));
        }
    } else if (g_str_case_rstr(i_paras->member_id, RF_PREFIX_PERIPHERAL) != NULL) {
        
        ret = dal_get_property_value_uint32(i_paras->obj_handle, PROPERTY_SOFTWARE_UNITNUM, &val_uint32);
        do_val_if_expr(0 == val_uint32, (null_flag = 1));
        
    } else {
        
        do_val_if_expr(get_other_firmware_property(i_paras->member_id, PROPERTY_FIRMWARE_LOGICUNIT, &val_uint32,
            sizeof(val_uint32)) != RET_OK, (null_flag = VOS_ERR));
    }
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: cann't get unit number info, ret is %d", __FUNCTION__, ret));

    json_object *obj_jso_huawei = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_jso_huawei, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object error.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_jso_huawei);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object error.", __FUNCTION__, __LINE__));

    

    if (get_object_position_id(i_paras->member_id, null_flag, val_uint32, &obj_jso_huawei) != RET_OK) {
        json_object_put(obj_jso_huawei);
        json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        return ret;
    }

    for (guint32 i = 0; i < sizeof(g_get_oem_functions) / sizeof(GetOemFun); i++) {
        if (g_get_oem_functions[i](i_paras, obj_jso_huawei) != HTTP_OK) {
            json_object_put(obj_jso_huawei);
            json_object_put(*o_result_jso);
            *o_result_jso = NULL;
            return HTTP_BAD_REQUEST;
        }
    }

    add_oem_property_value(i_paras, obj_jso_huawei, PROPETRY_PS_POWER_TYPE);
    
    
    
    add_upgrade_enough_property(i_paras, obj_jso_huawei);
    json_object_object_add(*o_result_jso, RFPROP_FWINV_HUAWEI, obj_jso_huawei);

    return HTTP_OK;
}


LOCAL gint32 get_firmwareinventory_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 health = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    const gchar *standby_set[] = {
        URI_OBJ_BACKUPBMC, URI_OBJ_BACKUPUBOOT, URI_OBJ_AVAILABLEBMC, URI_OBJ_HMM1_BK_UBOOT, URI_OBJ_HMM1_BK_SOFTWARE,
        URI_OBJ_HMM2_BK_UBOOT, URI_OBJ_HMM2_BK_SOFTWARE,
#ifdef ARM64_HI1711_ENABLED
        URI_OBJ_HMM1_AV_SOFTWARE, URI_OBJ_HMM2_AV_SOFTWARE
#endif
    };

    
    if (is_in_set_case(i_paras->member_id, standby_set, G_N_ELEMENTS(standby_set))) {
        (void)get_resource_status_property(&health, NULL, "StandbySpare", o_result_jso, FALSE);
        return HTTP_OK;
    } else if (!g_ascii_strcasecmp(i_paras->member_id, OBJECT_LCD)) {
        OBJ_HANDLE handle = 0;
        guint8 lcd_presence = 0;
        gchar *string_val = RF_STATE_ENABLED;

        gint32 ret = dfl_get_object_handle(OBJECT_LCD, &handle);
        if (ret != VOS_OK) { // Get handle failed
            health = 0xff;
            string_val = RF_STATE_ABSENT;
            (void)get_resource_status_property(&health, NULL, string_val, o_result_jso, FALSE);
            return HTTP_OK;
        }

        (void)dal_get_property_value_byte(handle, PROPERTY_PRESENCE, &lcd_presence);
        if (lcd_presence != BLADE_PRESENCE) { // Lcd is not present
            health = 0xff;
            string_val = RF_STATE_ABSENT;
        }

        (void)get_resource_status_property(&health, NULL, string_val, o_result_jso, FALSE);
        return HTTP_OK;
    } else {
        (void)get_resource_status_property(&health, NULL, "Enabled", o_result_jso, FALSE);
        return HTTP_OK;
    }
}


#ifdef ARM64_HI1711_ENABLED
LOCAL gint32 get_bmc_otherfirmwareinventory_version(PROVIDER_PARAS_S *i_paras, gchar *string_value, gint32 len,
    gint32 *result)
{
    gint32 ret = VOS_OK;
    gint32 cmp_result = VOS_OK;
    const gchar *avail_set[] = {FI_AVAILBMC_MEMBERID, URI_OBJ_HMM1_AV_SOFTWARE, URI_OBJ_HMM2_AV_SOFTWARE};
    
    if (is_in_set_case(i_paras->member_id, avail_set, G_N_ELEMENTS(avail_set))) {
        ret = get_obj_prop_val(OBJ_NAME_BMC, PROPERTY_BMC_AVAILABLE_VER, string_value, len, VAL_STRING);
    } else {
        cmp_result = VOS_ERR;
    }
    *result = ret;
    return cmp_result;
}
#endif


LOCAL gint32 get_joint_memberid_firmware_version(const gchar *member_id, gchar *firm_ver, guint32 firm_len)
{
    guint8 version_uint8 = 0;
    gchar class_name[MAX_PROP_NAME] = {0};
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE resource_obj = 0;

    // memberid为拼接值
    gint32 ret = check_memberid_get_property(member_id, class_name, sizeof(class_name), &component_handle,
        &resource_obj);
    if (ret != RET_OK) {
        
        debug_log(DLOG_DEBUG, "%s, %d, check_memberid_get_property fail.", __FUNCTION__, __LINE__);
        
        return RET_ERR;
    }
    // 电源
    if (g_strcmp0(class_name, CLASS_NAME_PS) == 0) {
        ret = dal_get_property_value_string(resource_obj, PROTERY_PS_VERSION, firm_ver, firm_len);
        if ((strlen(firm_ver) == 0) || (g_strcmp0(firm_ver, NA_STR) == 0)) {
            ret = dal_get_property_value_string(resource_obj, PROTERY_PS_FWVERSION, firm_ver, firm_len);
        }
    } else if (g_strcmp0(class_name, CLASS_BBU_MODULE_NAME) == 0) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_BBU_MODULE_FIRMWARE_VER, firm_ver, firm_len);
    } else if (g_strcmp0(class_name, CLASS_RAIDCARD_NAME) == 0 || g_strcmp0(class_name, CLASS_PCIECARD_NAME) == 0) {
        ret = redfish_get_firmware_version(resource_obj, firm_ver, firm_len); // Raid 和 Pcie Card
    } else if (g_strcmp0(class_name, CLASS_DFT_VERSION) == 0) { // Fabric Plane
        ret = dal_get_property_value_string(resource_obj, PROPERTY_VERSION_STR, firm_ver, firm_len);
        if (ret != RET_OK) {
            ret = check_string_val_effective(firm_ver);
        }
    } else if (g_strcmp0(class_name, CLASS_NAME_SMM_FAN) == 0) { 
        ret = dal_get_property_value_byte(resource_obj, PROPERTY_FAN_SOFT_VERSION, &version_uint8);
        (void)snprintf_s(firm_ver, firm_len, firm_len - 1, "%u", version_uint8);
    } else if (g_strcmp0(class_name, CLASS_RETIMER_NAME) == 0) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_RETIMER_SOFT_VER, firm_ver, firm_len);
    } else if (g_strcmp0(class_name, CLASS_VRD_UPGRADE_MGNT) == 0) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_VRDVERSION, firm_ver, firm_len);
    } else if (g_strcmp0(class_name, CLASS_CPUBOARD_NAME) == 0
        && dal_str_has_casesuffix(member_id, FI_IOCTRL_MEMBERID) != NULL) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_CPUBOARD_BIOSVER, firm_ver, firm_len);
    } else if (g_strcmp0(class_name, CLASS_SOC_BOARD) == 0 || g_strcmp0(class_name, CLASS_IOBOARD_NAME) == 0) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_SOFTWARE_VERSIONSTRING, firm_ver,
            firm_len);
    } else if (g_strcmp0(class_name, CLASS_UPGRADE_MCU_SOFTWARE_NAME) == 0) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_SOFTWARE_VERSIONSTRING, firm_ver,
            firm_len);
    } else if (dal_str_has_casesuffix(member_id, URI_SUFFIX_HWSR) != NULL) {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_HWSR_VERSION, firm_ver, firm_len);
    } else {
        ret = dal_get_property_value_string(resource_obj, PROPERTY_FIRMWARE_LOGICVER, firm_ver, firm_len);
    }
    return ret;
}
LOCAL gint32 get_bmc_class_set_firmware_version(const gchar *memberid, gchar *str_val, gint32 str_len)
{
    VER_OBJ_SET ver_set[] = {
        {BMC_MANA_VER_NAME,             URI_OBJ_ACTIVEBMC},
        {BMC_MANA_VER_NAME,             URI_OBJ_HMM1_AC_SOFTWARE},
        {BMC_MANA_VER_NAME,             URI_OBJ_HMM2_AC_SOFTWARE},
        {PROPERTY_BMC_BAKPMEVER,        URI_OBJ_BACKUPBMC},
        {PROPERTY_BMC_BAKPMEVER,        URI_OBJ_HMM1_BK_SOFTWARE},
        {PROPERTY_BMC_BAKPMEVER,        URI_OBJ_HMM2_BK_SOFTWARE},
        {PROPERTY_BMC_UOOTVER,          URI_OBJ_ACTIVEUBOOT},
        {PROPERTY_BMC_UOOTVER,          URI_OBJ_HMM1_AC_UBOOT},
        {PROPERTY_BMC_UOOTVER,          URI_OBJ_HMM2_AC_UBOOT},
        {PROPERTY_BMC_UOOTBACKUPVER,    URI_OBJ_BACKUPUBOOT},
        {PROPERTY_BMC_UOOTBACKUPVER,    URI_OBJ_HMM1_BK_UBOOT},
        {PROPERTY_BMC_UOOTBACKUPVER,    URI_OBJ_HMM2_BK_UBOOT},
        {PROPERTY_BMC_CPLDVER,          URI_OBJ_MAINBOARDCPLD},
        {PROPERTY_BMC_CPLDVER,          URI_OBJ_HMM1_CPLD},
        {PROPERTY_BMC_CPLDVER,          URI_OBJ_HMM2_CPLD},
    };
    for (guint32 idx = 0; idx < G_N_ELEMENTS(ver_set); idx++) {
        if (g_ascii_strcasecmp(memberid, ver_set[idx].obj_uri) == 0) {
            return get_obj_prop_val(OBJ_NAME_BMC, ver_set[idx].obj_property, str_val, str_len, VAL_STRING);
        }
    }
    return RET_ERR;
}


LOCAL gint32 get_firmwareinventory_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = VOS_OK;
    OBJ_HANDLE obj_handle = 0;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // memberid为固定值
    
    if (get_bmc_class_set_firmware_version(i_paras->member_id, string_value, sizeof(string_value)) == RET_OK) {
        ;
    }
    
#ifdef ARM64_HI1711_ENABLED
    else if (!get_bmc_otherfirmwareinventory_version(i_paras, string_value, sizeof(string_value), &ret)) {
        ;
    } else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_AVAILABLEBMC)) {
        ret =
            get_obj_prop_val(OBJ_NAME_BMC, PROPERTY_BMC_AVAILABLE_VER, string_value, sizeof(string_value), VAL_STRING);
    }
#endif
    

    else if (!g_ascii_strcasecmp(i_paras->member_id, OBJ_NAME_BIOS)) {
        ret = get_obj_prop_val(OBJ_NAME_BIOS, PROTERY_BIOS_VERSION, string_value, sizeof(string_value), VAL_STRING);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_IMU)) {
        ret = get_obj_prop_val(CLASS_ME_INFO, PROPERTY_MEINFO_FIRM_VER, string_value, sizeof(string_value), VAL_STRING);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, OBJECT_LCD)) {
        ret = get_obj_prop_val(OBJECT_LCD, PROPERTY_LCD_VERSION, string_value, sizeof(string_value), VAL_STRING);
    }
    
    else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_BASICBOARDCPLD)) {
        ret = dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_VRD_CPLD2_VERSION, string_value, sizeof(string_value));
    } else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_SDCARD_CONTROLLER)) {
        ret = dal_get_object_handle_nth(CLASS_RAID_CHIP_NAME, 0, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR,
            "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_RAID_CHIP_NAME));
        ret = redfish_get_object_verison(CLASS_RAID_CHIP_NAME, obj_handle, PROPETRY_FX3S_VERSION, string_value,
            sizeof(string_value));
    } else {
        ret = get_joint_memberid_firmware_version(i_paras->member_id, string_value, sizeof(string_value));
    }

    do_val_if_expr(VOS_OK != ret, return HTTP_INTERNAL_SERVER_ERROR);

    dal_trim_string(string_value, FWINV_STR_MAX_LEN);
    *o_result_jso = json_object_new_string((const gchar *)string_value);

    return HTTP_OK;
}

LOCAL gboolean is_memberid_related_to_chassis_board(const gchar *member_id, const gchar* class_name)
{
static const gchar *suffix_arr[] = {URI_CLASS_CPLD, URI_OBJ_CDR, URI_OBJ_RETIMER, URI_SUFFIX_IOCTRL};
    for (guint32 i = 0; i < G_N_ELEMENTS(suffix_arr); i++) {
        if (dal_str_has_casesuffix(member_id, suffix_arr[i]) != NULL) {
            return TRUE;
        }
    }
    const gchar *class_arr[] = {CLASS_UPGRADE_SOFTWARE_NAME, CLASS_SOC_BOARD, CLASS_IOBOARD_NAME};
    for (guint32 i = 0; i < G_N_ELEMENTS(class_arr); i++) {
        if (g_strcmp0(class_name, class_arr[i]) == 0) {
            return TRUE;
        }
    }
    
    return FALSE;
}

typedef gint32 (*splice_url_handler)(const gchar *slot, gchar *url, guint32 url_len);
typedef struct menberid_map {
    const gchar member_id[MAX_MEM_ID_LEN];
    splice_url_handler pfn_splice_url;
} MENBERID_MAP;

LOCAL gint32 splice_url_managers(const gchar *slot, gchar *url, guint32 url_len)
{
    return sprintf_s(url, url_len, "%s%s", RF_MANAGERS_URI, slot);
}
LOCAL gint32 splice_url_bios(const gchar *slot, gchar *url, guint32 url_len)
{
    guint32 product_ver = 0;
    
    gint32 ret = dal_get_product_version_num(&product_ver);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (product_ver > PRODUCT_VERSION_V3) {
        ret = sprintf_s(url, url_len, SYSTEMS_STORAGE_BIOS, slot);
    }
    
    return ret;
}
LOCAL gint32 splice_url_sdcard(const gchar *slot, gchar *url, guint32 url_len)
{
    return sprintf_s(url, url_len, STORAGE_SDCARD_CONTROLLERS_ODATA_ID, slot, 0);
}
LOCAL gint32 splice_url_main_fmboards(const gchar *slot, gchar *url, guint32 url_len)
{
    OBJ_HANDLE component_handle = 0;
    gchar board_name[MAX_PROP_NAME] = {0};
    (void)dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, MAINBOARD_DEVICETYPE,
        &component_handle);
    (void)get_location_devicename(component_handle, board_name, sizeof(board_name));
    return sprintf_s(url, url_len, URI_FORMAT_CHASSIS_FM_BOARDS, slot, board_name);
}

LOCAL MENBERID_MAP g_menberids_map[] = {
    {URI_OBJ_ACTIVEBMC, splice_url_managers}, {URI_OBJ_BACKUPBMC, splice_url_managers},
    {URI_OBJ_AVAILABLEBMC, splice_url_managers}, {OBJ_NAME_BIOS, splice_url_bios},
    {URI_OBJ_SDCARD_CONTROLLER, splice_url_sdcard}, {URI_OBJ_MAINBOARDCPLD, splice_url_main_fmboards},
    {URI_OBJ_MAINBOARDCPLD2, splice_url_main_fmboards}
};

LOCAL gint32 get_power_relateditem(OBJ_HANDLE resource_obj, const gchar *slot_id, gchar *relateditem,
    guint32 item_len)
{
#define MIN_DEVICE_POWER 1
    gint32 ret;
    guint8 ps_num = 1;
    gchar device_name[MAX_PROP_NAME] = {0};

    (void)dal_get_property_value_string(resource_obj, PROTERY_PS_DEVICENAME, device_name,
        sizeof(device_name));
    dal_clear_string_blank(device_name, sizeof(device_name));
    if (strlen(device_name) >= strlen(POWER_SUPPLY_UNIT_NAME) + 1 &&
        g_strstr_len(device_name, sizeof(device_name), POWER_SUPPLY_UNIT_NAME) != NULL &&
        vos_str2int(device_name + strlen(POWER_SUPPLY_UNIT_NAME), BASE_10, &ps_num, NUM_TPYE_UCHAR) ==
        RET_OK && ps_num >= MIN_DEVICE_POWER) {
        ret = snprintf_s(relateditem, item_len, item_len - 1,
            "%s%s/%s#/PowerSupplies/%u", RF_CHASSIS_UI, slot_id, RFPROP_CASSIS_POWER,
            dal_is_pangea_pacific_series() ? ps_num : (ps_num - 1));
    } else {
        ret = snprintf_s(relateditem, item_len, item_len - 1, "%s%s/%s", RF_CHASSIS_UI,
            slot_id, RFPROP_CASSIS_POWER);
    }

    return ret;
}

LOCAL gint32 get_mcu_version_relateditem(OBJ_HANDLE resource_obj, const gchar *slot_id, gchar *relateditem,
    guint32 item_len)
{
    gint32 ret;
    GSList *component_list = NULL;
    GSList *component_node = NULL;
    gchar device_name[MAX_PROP_NAME] = {0};
    gchar component_location[MAX_PROP_NAME] = {0};
    gchar firmware_location[MAX_PROP_NAME] = {0};
    gchar buf_str[FWINV_STR_MAX_LEN] = {0};

    ret = dal_get_object_list_position(resource_obj, CLASS_COMPONENT, &component_list);
    debug_log(DLOG_ERROR, "%s: get dal_get_object_list_position fail, ret is %d.", __FUNCTION__, ret);
    if (ret != RET_OK) {
        return ret;
    }
    (void)dal_get_property_value_string(resource_obj, PROPERTY_COMPONENT_LOCATION, firmware_location,
        sizeof(firmware_location));
    for (component_node = component_list; component_node; component_node = g_slist_next(component_node)) {
        (void)dal_get_property_value_string((OBJ_HANDLE)component_node->data, PROPERTY_COMPONENT_LOCATION,
            component_location, sizeof(component_location));
        if (g_strcmp0(component_location, firmware_location) != 0) {
            continue;
        }
        (void)dal_get_property_value_string((OBJ_HANDLE)component_node->data, PROPERTY_COMPONENT_DEVICE_NAME,
            device_name, sizeof(device_name));
        dal_clear_string_blank(device_name, sizeof(device_name));
        dal_clear_string_blank(component_location, sizeof(component_location));
        ret = snprintf_s(buf_str, FWINV_STR_MAX_LEN, FWINV_STR_MAX_LEN - 1, "%s%s", component_location,
            device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            goto exit;
        }
        ret = snprintf_s(relateditem, item_len, item_len - 1, URI_FORMAT_CHASSIS_BOARD, slot_id,
            buf_str);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
            goto exit;
        }
        goto exit;
    }
    ret = RET_ERR;
exit:
    g_slist_free(component_list);
    return ret;
}

LOCAL gint32 get_pcie_uri_msg(OBJ_HANDLE component_handle, gchar *mainboard_chassis_slot, gchar *msg, gint32 msg_len)
{
    OBJ_HANDLE pciecard_handle = 0;
    guchar pcie_card_slot = 0;
    gchar pcie_device_id[MAX_STRBUF_LEN] = {0};

    
    gint32 ret = dal_get_specific_object_string(CLASS_PCIECARD_NAME, PROPERTY_PCIE_CARD_REF_COMPONENT,
        dfl_get_object_name(component_handle), &pciecard_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: get object prop %s with string %s fail, ret is %d.", __FUNCTION__,
                  PROPERTY_PCIE_CARD_REF_COMPONENT, dfl_get_object_name(component_handle), ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, &pcie_card_slot);
    if (ret != RET_OK || pcie_card_slot == INVALID_VAL) {
        ret = dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIE_CARD_SLOT_ID, &pcie_card_slot);
    }
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s of pciecard fail, ret is %d.", __FUNCTION__, PROPERTY_PCIE_CARD_SLOT_ID,
                  ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = sprintf_s(pcie_device_id, MAX_STRBUF_LEN, "%s%u", RF_PCIECARD_PREFIX, pcie_card_slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = snprintf_s(msg, msg_len, msg_len - 1, URI_FORMAT_CHASSIS_FM_PCIEDEVICES, mainboard_chassis_slot,
                     pcie_device_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    
    return ret;
}

LOCAL gint32 get_other_uri_msg(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE component_handle, OBJ_HANDLE resource_obj,
                               gchar *class_name, gchar *string_len32, gint32 string_len32_len, gchar *enc_slot_id,
                               gchar *mainboard_chassis_slot, gchar *msg, gint32 msg_len)
{
    guint8 bbu_slot = 0;
    gint32 ret = RF_ERROR;
    // Power Supply
    if (!g_strcmp0(class_name, CLASS_NAME_PS)) {
        ret = get_power_relateditem(resource_obj, enc_slot_id, msg, msg_len);
    }
    if (!g_strcmp0(class_name, CLASS_UPGRADE_MCU_SOFTWARE_NAME)) {
        ret = get_mcu_version_relateditem(resource_obj, enc_slot_id, msg, msg_len);
        return ret;
    }
    if (g_strcmp0(class_name, CLASS_BBU_MODULE_NAME) == 0) {
        (void)dal_get_property_value_byte(resource_obj, PROPERTY_BBU_MODULE_ID, &bbu_slot);
        ret = snprintf_s(msg, msg_len, msg_len - 1, "%s%s/%s/%u", RF_CHASSIS_UI,
            enc_slot_id, RFPROP_CHASSIS_BBU, bbu_slot);
    }

    (void)get_location_devicename(component_handle, string_len32, string_len32_len);

    // PcieRaid
    if (!g_strcmp0(class_name, CLASS_PCIECARD_NAME)) {
        ret = get_pcie_uri_msg(component_handle, mainboard_chassis_slot, msg, msg_len);
    }
    // Fabric Plane
    else if (!g_strcmp0(class_name, CLASS_DFT_VERSION)) {
        ret = RF_ERROR;
    } else if (is_memberid_related_to_chassis_board(i_paras->member_id, class_name)) {
        ret = snprintf_s(msg, msg_len, msg_len - 1, URI_FORMAT_CHASSIS_FM_BOARDS, enc_slot_id, string_len32);
    }
    return ret;
}

LOCAL gboolean firmwareinventory_relateditem_is_valid(const gchar *class_name)
{
    if (dal_is_pangea_pacific_series() == FALSE) {
        return TRUE;
    }

    
    if (g_strcmp0(class_name, CLASS_NAME_PS) == 0) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 get_firmwareinventory_relateditem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar mainboard_chassis_slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gchar class_name[MAX_PROP_NAME] = {0};
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE resource_obj = 0;
    gchar string_len32[MAX_PROP_NAME] = {0};
    gchar enc_slot_id[MAX_RSC_ID_LEN + 1] = {0};

    
    return_val_do_info_if_fail(o_result_jso && o_message_jso && provider_paras_check(i_paras) == VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_EM)) {
        *o_result_jso = json_object_new_array();
        return HTTP_OK;
    }

    gint32 ret = rf_gen_enc_contained_component_chassis_slot(enc_slot_id, MAX_RSC_ID_LEN + 1);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get enclosure chassis component"));

    
    ret = redfish_get_board_slot(mainboard_chassis_slot, sizeof(mainboard_chassis_slot));
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));
    
    
    *o_result_jso = json_object_new_array();

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (guint8 i = 0; i < G_N_ELEMENTS(g_menberids_map); i++) {
        if (g_ascii_strcasecmp(i_paras->member_id, g_menberids_map[i].member_id) != 0) {
            continue;
        }
        ret = g_menberids_map[i].pfn_splice_url(mainboard_chassis_slot, string_value, sizeof(string_value));
        goto GOTURI;
    }

    // memberid为拼接值
    gint32 iRet = check_memberid_get_property(i_paras->member_id, class_name, sizeof(class_name), &component_handle,
        &resource_obj);
    if (iRet == RET_ERR) {
        
        debug_log(DLOG_DEBUG, "%s, %d, check_memberid_get_property fail.", __FUNCTION__, __LINE__);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_if_expr(firmwareinventory_relateditem_is_valid(class_name) == FALSE, HTTP_OK);

    ret = get_other_uri_msg(i_paras, component_handle, resource_obj, class_name, string_len32, sizeof(string_len32),
        enc_slot_id, mainboard_chassis_slot, string_value, sizeof(string_value));

GOTURI:
    if (ret == HTTP_INTERNAL_SERVER_ERROR) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (ret > 0) {
        json_object *obj_jso = json_object_new_object();
        return_val_do_info_if_expr(!obj_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)string_value));

        ret = json_object_array_add(*o_result_jso, obj_jso);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(obj_jso);
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.\n", __FUNCTION__, __LINE__));
    }

    return HTTP_OK;
}

LOCAL gboolean is_updateable_firmware_by_memberid(const char *member_id)
{
static const gchar *uri_obj_arr[] = {
    URI_OBJ_ACTIVEBMC, URI_OBJ_BACKUPBMC, URI_OBJ_AVAILABLEBMC, OBJ_NAME_BIOS, URI_OBJ_IMU, OBJECT_LCD};
static const gchar *suffix_arr[] = {URI_CLASS_CPLD, URI_SUFFIX_IOCTRL};
static const gchar *prefix_arr[] = {URI_CLASS_DISK, URI_OBJ_SDCARD_CONTROLLER};
    for (guint32 i = 0; i < G_N_ELEMENTS(uri_obj_arr); i++) {
        if (g_ascii_strcasecmp(member_id, uri_obj_arr[i]) == 0) {
            return TRUE;
        }
    }
    for (guint32 i = 0; i < G_N_ELEMENTS(suffix_arr); i++) {
        if (dal_str_has_casesuffix(member_id, suffix_arr[i]) != NULL) {
            return TRUE;
        }
    }
    for (guint32 i = 0; i < G_N_ELEMENTS(prefix_arr); i++) {
        if (g_ascii_strncasecmp(member_id, prefix_arr[i], strlen(prefix_arr[i])) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}


LOCAL gint32 get_firmwareinventory_updateable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE resource_obj = 0;
    guchar byte_value = 0;
    gchar class_name[MAX_PROP_NAME] = {0};
    guint8 software_type = 0;

    
    return_val_do_info_if_fail(NULL != o_message_jso && NULL != o_result_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    
    (void)dal_get_software_type(&software_type);

    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        const gchar *hmm_set[] = {
            URI_OBJ_HMM1_AC_UBOOT, URI_OBJ_HMM1_BK_UBOOT, URI_OBJ_HMM2_AC_UBOOT, URI_OBJ_HMM2_BK_UBOOT
        };
        *o_result_jso = json_object_new_boolean(is_in_set_case(i_paras->member_id, hmm_set, G_N_ELEMENTS(hmm_set)));
        return HTTP_OK;
        return HTTP_OK;
    }
    

    
    
    if (is_updateable_firmware_by_memberid(i_paras->member_id)) {
        *o_result_jso = json_object_new_boolean(1);
    } else if (!g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_ACTIVEUBOOT) ||
        !g_ascii_strcasecmp(i_paras->member_id, URI_OBJ_BACKUPUBOOT)) {
        *o_result_jso = json_object_new_boolean(0);
    } else {
        if (check_memberid_get_property(i_paras->member_id, class_name,
            sizeof(class_name), NULL, &resource_obj) == RET_ERR) {
            
            debug_log(DLOG_DEBUG, "check_memberid_get_property fail.");
            
            return VOS_ERR;
        }
        if (g_strcmp0(class_name, CLASS_NAME_PS) == 0 || g_strcmp0(class_name, CLASS_VRD_UPGRADE_MGNT) == 0) {
            // 电源、VRD
            (void)get_power_updatebale(i_paras->member_id, class_name, resource_obj, &byte_value);
            *o_result_jso = (byte_value == 1) ? json_object_new_boolean(1) : json_object_new_boolean(0);
        }
        const char* board_set[] = {CLASS_SOC_BOARD, CLASS_IOBOARD_NAME};
        if (is_in_set_case(class_name, board_set, G_N_ELEMENTS(board_set))) {
            *o_result_jso = json_object_new_boolean(TRUE);
            return HTTP_OK;
        }
    }
    
    return HTTP_OK;
}

LOCAL gint32 get_board_softwareid(OBJ_HANDLE firm_handle, json_object **o_result_jso, const gchar *class_name,
    const gchar *prop)
{
    OBJ_HANDLE board_handle = 0;
    gchar softwareid[FWINV_STR_MAX_LEN] = {0};
    if (dal_get_specific_object_position(firm_handle, class_name, &board_handle) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gchar board_name[MAX_STRBUF_LEN] = {0};
    gint32 ret = dal_get_property_value_string(board_handle, prop, board_name, sizeof(board_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if ((ret = sprintf_s(softwareid, FWINV_STR_MAX_LEN, "BIOS-%s", board_name)) <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(softwareid);
    return HTTP_OK;
}

LOCAL gint32 get_board_mcu_softwareid(OBJ_HANDLE firm_handle, json_object **o_result_jso, const gchar *class_name,
    const gchar *prop)
{
    OBJ_HANDLE board_handle = 0;
    gchar softwareid[FWINV_STR_MAX_LEN] = {0};
    if (dal_get_specific_object_position(firm_handle, class_name, &board_handle) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gchar board_name[MAX_STRBUF_LEN] = {0};
    gint32 ret = dal_get_property_value_string(board_handle, prop, board_name, sizeof(board_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = sprintf_s(softwareid, FWINV_STR_MAX_LEN, "MCU-%s", board_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(softwareid);
    return HTTP_OK;
}


LOCAL gint32 get_firmwareinventory_softwareid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gchar class_name[MAX_CLASS_NAME] = {0};
    gchar sw_id_pattern[MAX_SOFTWARE_ID_LEN] = {0};
    gchar sw_id_prop_name[MAX_PROP_NAME] = {0};
    gchar sw_id_prop_val[FWINV_STR_MAX_LEN] = {0};

    gint32 ret = check_param_and_privi_validity(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        return ret;
    } else if (dal_match_software_type(MGMT_SOFTWARE_TYPE_EM)) {
        return HTTP_OK;
    }

    ret = check_memberid_get_property(i_paras->member_id, class_name, sizeof(class_name), NULL, &obj_handle);
    if (VOS_ERR == ret) {
        
        debug_log(DLOG_DEBUG, "%s, %d, check_memberid_get_property fail.", __FUNCTION__, __LINE__);
        
        return VOS_ERR;
    }

    for (gint32 i = 0; i < G_N_ELEMENTS(get_softwareid_map); i++) {
        if (g_strcmp0(class_name, get_softwareid_map[i].firm_handle) == 0) {
            return get_softwareid_map[i].func(obj_handle, o_result_jso, get_softwareid_map[i].firm_handle,
                get_softwareid_map[i].prop);
        }
    }

    ret = get_softwareid_info(i_paras->member_id, class_name, sw_id_pattern, MAX_SOFTWARE_ID_LEN, sw_id_prop_name,
        MAX_PROP_NAME);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get software id info for %s", i_paras->member_id));

    
    if ((0 != g_strcmp0(sw_id_prop_name, SW_ID_NA)) && (0 != g_strcmp0(sw_id_prop_name, SW_ID_NULL))) {
        if (g_str_case_rstr(i_paras->member_id, "VRD") != NULL) {
            (void)get_vrd_component_name(i_paras->member_id, sw_id_prop_val, sizeof(sw_id_prop_val));
        } else {
            (void)dal_get_property_value_string(obj_handle, sw_id_prop_name, sw_id_prop_val, FWINV_STR_MAX_LEN);
        }
        dal_trim_string(sw_id_prop_val, FWINV_STR_MAX_LEN);
        ret = sprintf_s(sw_id_prop_name, MAX_PROP_NAME, "%s", (strlen(sw_id_prop_val) == 0 ? NA_STR : sw_id_prop_val));
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret);
        }
    }
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(string_value, FWINV_STR_MAX_LEN, FWINV_STR_MAX_LEN - 1, sw_id_pattern, sw_id_prop_name);
#pragma GCC diagnostic pop
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", ret);
    }
    *o_result_jso = json_object_new_string((const gchar *)string_value);
    return HTTP_OK;
}


LOCAL gint32 CheckVersionMmemberid(const char *member_id, gchar *class_name, gint32 name_len)
{
    
    // 版本信息始终有效
    // bmc uboot
    const gchar *bmc_ver_set[] = {
        URI_OBJ_ACTIVEBMC, URI_OBJ_BACKUPBMC, URI_OBJ_ACTIVEUBOOT, URI_OBJ_BACKUPUBOOT,
#ifdef ARM64_HI1711_ENABLED
        FI_AVAILBMC_MEMBERID
#endif
    };
    if (is_in_set_case(member_id, bmc_ver_set, G_N_ELEMENTS(bmc_ver_set))) {
        gint32 safe_fun_ret = strncpy_s(class_name, name_len, BMC_CLASEE_NAME, strlen(BMC_CLASEE_NAME));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        return VOS_OK;
    }

    return VOS_ERR;
}


LOCAL gint32 check_bios_imu(const char *member_id, gchar *class_name, gint32 name_len)
{
    errno_t safe_fun_ret = EOK;
    // bios
    if (!g_ascii_strcasecmp(member_id, BIOS_OBJECT_NAME)) {
        safe_fun_ret = strncpy_s(class_name, name_len, BIOS_CLASS_NAME, strlen(BIOS_CLASS_NAME));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));

        return RET_OK;
    }
    // IMU
    if (!g_ascii_strcasecmp(member_id, URI_OBJ_IMU) && is_imu_valid()) {
        safe_fun_ret = strncpy_s(class_name, name_len, URI_OBJ_IMU, strlen(URI_OBJ_IMU));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));

        return RET_OK;
    }
    return RET_ERR;
}


LOCAL gint32 check_constant_memberid(const char *member_id, gchar *class_name, gint32 name_len,
    OBJ_HANDLE *resource_handle)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;

    
    // 版本信息始终有效
    // bmc uboot
    if (CheckVersionMmemberid(member_id, class_name, name_len) == VOS_OK) {
        return VOS_OK;
    }

    if (check_bios_imu(member_id, class_name, name_len) == RET_OK) {
        return VOS_OK;
    }

    // mainboard
    if (!g_ascii_strcasecmp(member_id, URI_OBJ_MAINBOARDCPLD)) {
        safe_fun_ret = strncpy_s(class_name, name_len, CLASS_MAINBOARD_NAME, strlen(CLASS_MAINBOARD_NAME));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        ret = dal_get_object_handle_nth(CLASS_MAINBOARD_NAME, 0, resource_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d:  get obj_handle for %s fail\n", __FUNCTION__, __LINE__,
            CLASS_MAINBOARD_NAME));
        return VOS_OK;
    }

    // mainboardback
    if (!g_ascii_strcasecmp(member_id, URI_OBJ_BASICBOARDCPLD)) {
        
        return_val_do_info_if_fail(VOS_OK == dal_get_object_handle_nth(CLASS_VRD_UPGRADE_MGNT, 0, &obj_handle),
            VOS_ERR, debug_log(DLOG_DEBUG, "%s:  get obj_handle for %s fail",
                __FUNCTION__, CLASS_VRD_UPGRADE_MGNT));
        
        ret = redfish_get_object_verison(CLASS_VRD_UPGRADE_MGNT, obj_handle, PROPERTY_VRD_CPLD2_VERSION, NULL, 0);
        return_val_if_fail(VOS_OK == ret, VOS_ERR);

        safe_fun_ret = strncpy_s(class_name, name_len, CLASS_VRD_UPGRADE_MGNT, strlen(CLASS_VRD_UPGRADE_MGNT));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        return VOS_OK;
    }

    
    if (!g_ascii_strcasecmp(member_id, OBJECT_LCD)) {
        return_val_do_info_if_fail(VOS_OK == dfl_get_object_handle(OBJECT_LCD, &obj_handle), VOS_ERR,
            debug_log(DLOG_ERROR, "%s:  get obj_handle fail, %s", __FUNCTION__, OBJECT_LCD));

        ret = redfish_get_object_verison(CLASS_LCD, obj_handle, PROPERTY_LCD_VERSION, NULL, 0);
        return_val_if_fail(VOS_OK == ret, VOS_ERR);

        safe_fun_ret = strncpy_s(class_name, name_len, OBJECT_LCD, strlen(OBJECT_LCD));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        return VOS_OK;
    }

    
    if (!g_ascii_strcasecmp(member_id, URI_OBJ_SDCARD_CONTROLLER)) {
        ret = dal_get_object_handle_nth(CLASS_RAID_CHIP_NAME, 0, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d:  get obj_handle for %s fail\n", __FUNCTION__, __LINE__,
            URI_OBJ_SDCARD_CONTROLLER));

        ret = redfish_get_object_verison(CLASS_RAID_CHIP_NAME, obj_handle, PROPETRY_FX3S_VERSION, NULL, 0);
        
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "get property %s for %s failed", PROPETRY_FX3S_VERSION, CLASS_RAID_CHIP_NAME));
        
        safe_fun_ret = strncpy_s(class_name, name_len, OBJECT_LCD, strlen(OBJECT_LCD));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        return VOS_OK;
    }

    

    return VOS_ERR;
}


LOCAL gint32 memberid_get_component_handle(const char *member_id, OBJ_HANDLE *component_handle, guint8 *member_type)
{
    guint8 com_fruid = 0;
    guint8 com_device_type = 0;
    const char *chip_arr[] = {"", "CPLD", "Cdr", "Retimer", "HWSR", FI_IOCTRL_MEMBERID};
    
    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: dfl_get_object_list fail, ret=%d.", __FUNCTION__, ret));

    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        // 匹配槽位号和部件编号
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_TYPE, &com_device_type);

        // FruId为255 用于告警优化
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_FRUID, &com_fruid);
        check_success_continue(255 == com_fruid);

        gchar device_name[MAX_PROP_NAME] = {0};
        gchar location[MAX_PROP_NAME] = {0};
        gchar resource_id[MAX_SOFTWARE_ID_LEN] = {0};

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_LOCATION, location,
            sizeof(location));

        dal_clear_string_blank(device_name, sizeof(device_name));
        dal_clear_string_blank(location, sizeof(location));

        ret = snprintf_s(resource_id, sizeof(resource_id), sizeof(resource_id) - 1, "%s%s", location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret=%d", __FUNCTION__, ret);
        }

        gint32 resource_id_len = strlen(resource_id);

        for (gsize i = 0; i < sizeof(chip_arr) / sizeof(chip_arr[0]); i++) {
            errno_t safe_fun_ret = strcat_s(resource_id, sizeof(resource_id), chip_arr[i]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strcat_s failrd, ret=%d.", __FUNCTION__, safe_fun_ret);

                g_slist_free(obj_list);
                return VOS_ERR;
            }
            if (g_ascii_strcasecmp(member_id, resource_id) == 0) {
                // 匹配成功
                *component_handle = (OBJ_HANDLE)obj_node->data;
                *member_type = i;

                g_slist_free(obj_list);
                return VOS_OK;
            }
            resource_id[resource_id_len] = '\0';
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return VOS_ERR;
}


LOCAL gint32 component_obj_get_correspond_resource_obj(OBJ_HANDLE component_handle, gchar *class_name, gint32 name_len,
    const gchar *slot_name, OBJ_HANDLE *resource_obj)
{
    gint32 ret;
    guint8 device_num = 0;

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    guchar version_type = 0;
    guchar fru_id = 0;
    guchar com_fruid = 0;

    OBJ_HANDLE ref_handle = 0;
    const gchar *name_str = NULL;

    
    // ExpBoard和硬盘背板部件编号一样 后置硬盘背板槽位号通过计算得到 通过对象名进行匹配
    if (0 == g_strcmp0(class_name, CLASS_HDDBACKPLANE_NAME) || 0 == g_strcmp0(class_name, CLASS_EXPBOARD_NAME)) {
        name_str = dfl_get_object_name(component_handle);
        // 在硬盘背板中查找
        if (dfl_get_object_list(CLASS_HDDBACKPLANE_NAME, &obj_list) == DFL_OK) {
            for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
                ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, PROPERTY_HDDBACKPLANE_REF_COMPONENT,
                    &ref_handle);
                if (ret != VOS_OK) {
                    debug_log(DLOG_ERROR, "%s, %d: dfl_get_referenced_object fail object: %s property: %s.\n",
                        __FUNCTION__, __LINE__, dfl_get_object_name((OBJ_HANDLE)obj_node->data),
                        PROPERTY_HDDBACKPLANE_REF_COMPONENT);

                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_ERR;
                }

                debug_log(DLOG_DEBUG, "%s, %d: refname: %s componentname: %s.\n", __FUNCTION__, __LINE__,
                    dfl_get_object_name(ref_handle), dfl_get_object_name(component_handle));

                if (0 == g_strcmp0(dfl_get_object_name(ref_handle), name_str)) {
                    // 匹配成功
                    if (EOK !=
                        strncpy_s(class_name, name_len, CLASS_HDDBACKPLANE_NAME, strlen(CLASS_HDDBACKPLANE_NAME))) {
                        debug_log(DLOG_ERROR, "%s, %d:  strncpy_s fail \n", __FUNCTION__, __LINE__);
                    }

                    *resource_obj = (OBJ_HANDLE)obj_node->data;

                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_OK;
                }
            }

            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
        }

        obj_list = NULL;

        // 在ExpBoard中查找
        if (dfl_get_object_list(CLASS_EXPBOARD_NAME, &obj_list) == DFL_OK) {
            for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
                ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, PROPERTY_HDDBACKPLANE_REF_COMPONENT,
                    &ref_handle);
                if (ret != VOS_OK) {
                    debug_log(DLOG_ERROR, "%s, %d: dfl_get_referenced_object fail object: %s property: %s.\n",
                        __FUNCTION__, __LINE__, dfl_get_object_name((OBJ_HANDLE)obj_node->data),
                        PROPERTY_HDDBACKPLANE_REF_COMPONENT);

                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_ERR;
                }

                debug_log(DLOG_DEBUG, "%s, %d: refname: %s componentname: %s.\n", __FUNCTION__, __LINE__,
                    dfl_get_object_name(ref_handle), dfl_get_object_name(component_handle));

                if (0 == g_strcmp0(dfl_get_object_name(ref_handle), name_str)) {
                    // 匹配成功
                    if (EOK != strncpy_s(class_name, name_len, CLASS_EXPBOARD_NAME, strlen(CLASS_EXPBOARD_NAME))) {
                        debug_log(DLOG_ERROR, "%s, %d:  strncpy_s fail \n", __FUNCTION__, __LINE__);
                    }

                    *resource_obj = (OBJ_HANDLE)obj_node->data;

                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_OK;
                }
            }

            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
        }

        return VOS_ERR;
    }

    

    
    // FruId关联
    if (0 == g_strcmp0(class_name, CLASS_DFT_VERSION)) {
        ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_FRUID, &com_fruid);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte fail.", __FUNCTION__, __LINE__));

        if (dfl_get_object_list(CLASS_DFT_VERSION, &obj_list) == DFL_OK) {
            for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
                ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_VERSION_TYPE, &version_type);
                do_val_if_expr(VOS_OK != ret,
                    debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte.\n", __FUNCTION__, __LINE__));

                ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_FRU_ID, &fru_id);
                do_val_if_expr(VOS_OK != ret,
                    debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte.\n", __FUNCTION__, __LINE__));

                // version_type为24 fru_id为关联
                if (VERSION_TYPE_FABRIC == version_type && com_fruid == fru_id) {
                    // 匹配成功
                    *resource_obj = (OBJ_HANDLE)obj_node->data;
                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_OK;
                }
            }

            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
            return VOS_ERR;
        }
    }

    
    if (g_strcmp0(class_name, CLASS_CPUBOARD_NAME) == 0 || g_strcmp0(class_name, CLASS_IOBOARD_NAME) == 0 ||
        g_strcmp0(class_name, CLASS_RETIMER_NAME) == 0) {
        if (dfl_get_object_list(class_name, &obj_list) == DFL_OK) {
            name_str = dfl_get_object_name(component_handle);

            for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
                ret =
                    dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, PROPERTY_CPUBOARD_REFCOMPONENT, &ref_handle);
                if (ret != VOS_OK) {
                    debug_log(DLOG_ERROR, "%s, %d: dfl_get_referenced_object fail object: %s property: %s.\n",
                        __FUNCTION__, __LINE__, dfl_get_object_name((OBJ_HANDLE)obj_node->data),
                        PROPERTY_HDDBACKPLANE_REF_COMPONENT);

                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_ERR;
                }

                debug_log(DLOG_DEBUG, "%s, %d: refname: %s componentname: %s.\n", __FUNCTION__, __LINE__,
                    dfl_get_object_name(ref_handle), dfl_get_object_name(component_handle));

                if (0 == g_strcmp0(dfl_get_object_name(ref_handle), name_str)) {
                    *resource_obj = (OBJ_HANDLE)obj_node->data;

                    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
                    return VOS_OK;
                }
            }

            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
        }
    }

    

    // 其他通过槽位号匹配
    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &device_num);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: dal_get_property_value_byte fail.", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_byte(class_name, slot_name, device_num, resource_obj);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:  get object fail, %s, %s, %d\n", __FUNCTION__, __LINE__, class_name, slot_name,
        device_num));
    
    return VOS_OK;
}


LOCAL gint32 memberid_get_ps_handle(const gchar *member_id, OBJ_HANDLE *ps_handle)
{
    int iRet = -1;
    GSList *obj_list = NULL;
    
    return_val_do_info_if_fail(dfl_get_object_list(CLASS_NAME_PS, &obj_list) == DFL_OK, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:  dfl_get_object_list fail.", __FUNCTION__, __LINE__));
    
    for (GSList *obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        gchar device_name[MAX_PROP_NAME] = {0};
        gchar location[MAX_PROP_NAME] = {0};
        gchar resource_id[MAX_SOFTWARE_ID_LEN] = {0};

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROTERY_PS_DEVICENAME, device_name,
            sizeof(device_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROTERY_PS_LOCATION, location,
            sizeof(location));

        dal_clear_string_blank(device_name, sizeof(device_name));
        dal_clear_string_blank(location, sizeof(location));

        iRet = snprintf_s(resource_id, sizeof(resource_id), sizeof(resource_id) - 1, "%s%s", location, device_name);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        if (0 == g_ascii_strcasecmp(member_id, resource_id)) {
            // 匹配成功
            *ps_handle = (OBJ_HANDLE)obj_node->data;

            do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

            return VOS_OK;
        }
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    return VOS_ERR;
}


LOCAL gint32 memberid_get_retimer_handle(const gchar *member_id, OBJ_HANDLE* retimer_handle)
{
    GSList* obj_list = NULL;
    guint8 retimer_id = 0;
    gchar *pattern_lower = NULL;
    gchar retimer[MAX_MEM_ID_LEN] = {0};
    OBJ_HANDLE obj_handle;

    gint32 ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %s object failed, ret:%d", CLASS_RETIMER_NAME, ret);
        return RET_ERR;
    }

    for (GSList* obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        ret = redfish_get_object_verison(CLASS_RETIMER_NAME, obj_handle, PROPERTY_RETIMER_SOFT_VER, NULL, 0);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Redfish_get_object_verison fail. ret = %d", ret);
            continue;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get %s.%s failed, ret:%d", dfl_get_object_name(obj_handle),
                      PROPERTY_RETIMER_ID, ret);
            continue;
        }

        (void)memset_s(retimer, sizeof(retimer), 0, sizeof(retimer));
        ret = snprintf_s(retimer, sizeof(retimer), sizeof(retimer) - 1, "%s%u", CLASS_RETIMER_NAME, retimer_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s snprintf_s fail, ret = %d", dfl_get_object_name(obj_handle), ret);
            continue;
        }

        pattern_lower = g_ascii_strdown(retimer, strlen(retimer));
        if (pattern_lower == NULL) {
            debug_log(DLOG_ERROR, "[%s]: g_ascii_strdown fail, retimer is %s", __FUNCTION__, retimer);
            continue;
        }

        if (g_ascii_strcasecmp(member_id, pattern_lower) == 0) {
            
            *retimer_handle = (OBJ_HANDLE)obj_node->data;
            if (obj_list != NULL) {
                g_slist_free(obj_list);
                g_free(pattern_lower);
            }
            return RET_OK;
        }
        g_free(pattern_lower);
    }
    if (obj_list != NULL) {
        g_slist_free(obj_list);
    }
    return RET_ERR;
}


LOCAL void _get_device_type_0x26_type(OBJ_HANDLE com_obj, guint8 *i_device_type)
{
    OBJ_HANDLE obj_handle_tmp = 0;

    return_if_expr(0 == com_obj || NULL == i_device_type);

    gint32 ret = dal_get_specific_object_position(com_obj, CLASS_PCIETRANSFORMCARD, &obj_handle_tmp);
    return_do_info_if_expr(DFL_OK == ret, (*i_device_type = COMPONENT_TYPE_BOARD_PCIE_ADAPTER));

    ret = dal_get_specific_object_position(com_obj, CLASS_RISERPCIECARD_NAME, &obj_handle_tmp);
    return_do_info_if_expr(DFL_OK == ret, (*i_device_type = COMPONENT_TYPE_PCIE_RISER));

    ret = dal_get_specific_object_position(com_obj, CLASS_IOBOARD_NAME, &obj_handle_tmp);
    return_do_info_if_expr(DFL_OK == ret, (*i_device_type = COMPONENT_TYPE_IO_BOARD));

    debug_log(DLOG_INFO, "%s: get device type 0x26 class failed", __FUNCTION__);
}


LOCAL gint32 check_smm_software_version(const char *member_id, gchar *class_name, gint32 name_len,
    OBJ_HANDLE *resource_handle, guint8 smm_index, gboolean *is_software_member)
{
    const gchar *hmm_set[] = {
        URI_OBJ_HMM1_AC_SOFTWARE, URI_OBJ_HMM1_BK_SOFTWARE, URI_OBJ_HMM2_AC_SOFTWARE, URI_OBJ_HMM2_BK_SOFTWARE,
#ifdef ARM64_HI1711_ENABLED
        URI_OBJ_HMM1_AV_SOFTWARE, URI_OBJ_HMM2_AV_SOFTWARE
#endif
    };
    if (is_in_set_case(member_id, hmm_set, G_N_ELEMENTS(hmm_set))) {
        
        *is_software_member = TRUE;
        if (smm_index == member_id[SMM_NUM_INDEX] - '0') { // 访问本板可升级资源
            errno_t safe_fun_ret = strncpy_s(class_name, name_len, CLASS_BMC, sizeof(CLASS_BMC));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            (void)dal_get_object_handle_nth(CLASS_BMC, 0, resource_handle);
            return VOS_OK;
        }

        // 对板资源已透传处理
        return VOS_ERR;
    }
    // No match
    return VOS_ERR;
}


LOCAL gint32 check_smm_memberid(const char *member_id, gchar *class_name, gint32 name_len, OBJ_HANDLE *resource_handle)
{
    errno_t safe_fun_ret = EOK;
    guint8 smm_index = 0;
    guint32 fan_index = 0;
    guint32 fan_count = 0;
    gchar fantray_name[MAX_NAME_SIZE] = { 0 };
    OBJ_HANDLE handle = 0;
    guint8 fan_presence = 0;
    gboolean is_software_member = FALSE;

    (void)get_smm_index(&smm_index);

    // UBoot
    const gchar *uboot_set[] = {
        URI_OBJ_HMM1_AC_UBOOT, URI_OBJ_HMM1_BK_UBOOT, URI_OBJ_HMM2_AC_UBOOT, URI_OBJ_HMM2_BK_UBOOT
    };
    if (is_in_set_case(member_id, uboot_set, G_N_ELEMENTS(uboot_set))) {
        if (smm_index == member_id[3] - '0') { // 访问本板可升级资源
            safe_fun_ret = strncpy_s(class_name, name_len, CLASS_BMC, sizeof(CLASS_BMC));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            (void)dal_get_object_handle_nth(CLASS_BMC, 0, resource_handle);
            return VOS_OK;
        }

        // 对板资源已透传处理
        return VOS_ERR;
    }
    gint32 ret = check_smm_software_version(member_id, class_name, name_len,
        resource_handle, smm_index, &is_software_member);
    
    if (is_software_member == TRUE) {
        return ret;
    }
    // CPLD
    if (g_ascii_strcasecmp(member_id, "HMM1CPLD") == 0 || g_ascii_strcasecmp(member_id, "HMM2CPLD") == 0) {
        if (smm_index == member_id[3] - '0') { // 访问本板可升级资源
            safe_fun_ret = strncpy_s(class_name, name_len, CLASS_BMC, sizeof(CLASS_BMC));
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            (void)dal_get_object_handle_nth(CLASS_BMC, 0, resource_handle);
            return VOS_OK;
        }

        // 对板资源已透传处理
        return VOS_ERR;
    }

    // LCD
    if (g_ascii_strcasecmp(member_id, "Lcd") == 0) {
        if (dfl_get_object_handle(OBJECT_LCD, &handle) != RET_OK) { // Get handle failed
            return VOS_ERR;
        }

        return VOS_OK;
    }

    // Fantray
    if (dfl_get_object_count(CLASS_NAME_SMM_FAN, &fan_count) == RET_OK) {
        for (fan_index = 0; fan_index < fan_count; fan_index++) { // Check all fantray
            (void)snprintf_s(fantray_name, sizeof(fantray_name), sizeof(fantray_name) - 1, "Fantray%u", fan_index + 1);

            if (g_ascii_strcasecmp(member_id, fantray_name) == 0) { // Find fantray, check fantray presence
                safe_fun_ret = strncpy_s(class_name, name_len, CLASS_NAME_SMM_FAN, sizeof(CLASS_NAME_SMM_FAN));
                do_val_if_expr(safe_fun_ret != EOK,
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                if (dal_get_object_handle_nth(CLASS_NAME_SMM_FAN, fan_index, resource_handle) != RET_OK) {
                    return VOS_ERR;
                }

                (void)dal_get_property_value_byte(*resource_handle, PROPERTY_FAN_PRESENT, &fan_presence);

                return (fan_presence == BLADE_PRESENCE) ? VOS_OK : VOS_ERR;
            }
        }
    }

    // No match
    return VOS_ERR;
}


LOCAL void memberid_class_name_copy_and_check_return(gchar *class_name, gint32 name_len, gchar *i_class_name)
{
    if (NULL != class_name) {
        errno_t safe_fun_ret = strncpy_s(class_name, name_len, i_class_name, name_len - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
}

LOCAL gint32 get_bbu_handle_by_memberid(const gchar *member_id, OBJ_HANDLE *bbu_handle)
{
    GSList *obj_list = NULL;
    gchar resource_id[MAX_SOFTWARE_ID_LEN] = {0};

    gint32 ret = dfl_get_object_list(CLASS_BBU_MODULE_NAME, &obj_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: Get list of %s failed, ret=%d", __FUNCTION__, CLASS_BBU_MODULE_NAME, ret);
        return VOS_ERR;
    }

    for (GSList *obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        OBJ_HANDLE object_bbu = (OBJ_HANDLE)obj_node->data;

        ret = rf_gen_rsc_id_with_location_and_devicename(object_bbu, resource_id, sizeof(resource_id));
        if (ret != VOS_OK) {
            continue;
        }

        if (g_ascii_strcasecmp(member_id, resource_id) == 0) {
            *bbu_handle = object_bbu;

            g_slist_free(obj_list);

            return VOS_OK;
        }
    }

    g_slist_free(obj_list);

    return VOS_ERR;
}

LOCAL gint32 get_object_present_or_verion_validity(
    guint8 member_type, const gchar *member_id, const gchar *class_name, OBJ_HANDLE resource_obj)
{
    gint32 ret;
    gchar ver_prop_name[FWINV_STR_MAX_LEN] = PROPERTY_FIRMWARE_LOGICVER;

    
    if (dal_str_has_casesuffix(member_id, URI_SUFFIX_HWSR) != NULL) {
        ret = redfish_get_object_verison(class_name, resource_obj, PROPERTY_HWSR_VERSION, NULL, 0);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: redfish_get_object_verison fail, %s", __FUNCTION__, class_name);
            return RET_ERR;
        }
    } else {
        // raid在接口中单独处理 其他版本属性名取"LogicVer"
        if (g_strcmp0(class_name, CLASS_CPUBOARD_NAME) == 0 && member_type == CHIP_TYPE_IOCTRL) {
            (void)strcpy_s(ver_prop_name, sizeof(ver_prop_name), PROPERTY_CPUBOARD_BIOSVER);
        }
        ret = redfish_get_object_verison(class_name, resource_obj, ver_prop_name, NULL, 0);
        if (ret != RET_OK) {
            
            debug_log(DLOG_DEBUG, "%s: redfish_get_object_verison fail, %s", __FUNCTION__, class_name);
            
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL gint32 get_object_class_name(
    guint8 *member_type, const gchar *member_id, OBJ_HANDLE *component_handle, gchar *class_name, guint8 class_len)
{
    guint8 device_type = 0;
    guint8 tmp_value = 0;
    guint32 i;
    guint32 len = G_N_ELEMENTS(classinfo_map);

    
    if (memberid_get_component_handle(member_id, component_handle, member_type) != RET_OK) {
        
        debug_log(DLOG_ERROR, "%s, check memberid fail, %s", __FUNCTION__, member_id);
        
        return RET_ERR;
    }

    if (*member_type != CHIP_TYPE_NORMAL && *member_type != CHIP_TYPE_CPLD && *member_type != CHIP_TYPE_HWSR &&
        *member_type != CHIP_TYPE_IOCTRL) {
        debug_log(DLOG_ERROR, "%s: member_type %d not found", __FUNCTION__, *member_type);
        return RET_ERR;
    }

    
    (void)dal_get_property_value_byte(*component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &device_type);
    
    if (device_type == COMPONENT_TYPE_BOARD_PCIE_ADAPTER) {
        _get_device_type_0x26_type(*component_handle, &device_type);
    }
    
    for (i = 0; i < len; i++) {
        (void)vos_str2int(classinfo_map[i][5], 10, &tmp_value, NUM_TPYE_UCHAR);
        if (tmp_value == device_type) {
            errno_t safe_fun_ret = strcpy_s(class_name, class_len, classinfo_map[i][1]);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            }
            break;
        }
    }

    if (i == len) {
        // 没有数据
        debug_log(DLOG_ERROR, "%s:no data in classinfo_map. device_type %d ", __FUNCTION__, device_type);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_handle_if_match_soc_board(const char *member_id, OBJ_HANDLE *out_fw_hd, OBJ_HANDLE *out_comp_hd)
{
    GSList *firm_list = NULL;
    (void)dfl_get_object_list(CLASS_UPGRADE_SOFTWARE_NAME, &firm_list);
    for (GSList *firm = firm_list; firm != NULL; firm = firm->next) {
        OBJ_HANDLE firm_handle = (OBJ_HANDLE)firm->data;
        if (!is_valid_board_firmver(firm_handle, BIOS_COMP_ID_EX_ATLAS)) {
            continue;
        }
        gchar firm_id[FI_URI_LEN] = {0};
        OBJ_HANDLE comp_handle = 0;
        gint32 ret = get_soc_board_firm_memberid(firm_handle, firm_id, FI_URI_LEN);
        if (ret != RET_OK || g_ascii_strcasecmp(member_id, firm_id) != 0) {
            continue;
        }
        ret = dal_get_specific_position_object_byte(firm_handle, CLASS_COMPONENT,
            PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_SOC_BOARD, &comp_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_specific_position_object_byte fail, ret=%d", __FUNCTION__, ret);
            break;
        }
        if (out_fw_hd != NULL) {
            *out_fw_hd = firm_handle;
        }
        if (out_comp_hd != NULL) {
            *out_comp_hd = comp_handle;
        }
        g_slist_free(firm_list);
        return RET_OK;
    }
    g_slist_free(firm_list);
    return RET_ERR;
}

LOCAL const gchar* get_class_slot_name(const gchar *class_name)
{
    const gchar *slot_name = PROPERTY_FIRMWARE_SLOT;
    if (g_strcmp0(CLASS_NAME_PS, class_name) == 0) {
        slot_name = PROTERY_PS_SLOT_ID;
    } else if (g_strcmp0(CLASS_BBU_MODULE_NAME, class_name) == 0) {
        slot_name = PROPERTY_BBU_MODULE_ID;
    } else if (g_strcmp0(CLASS_RAIDCARD_NAME, class_name) == 0) {
        slot_name = PROPERTY_RAIDCARD_SLOT;
    } else if (g_strcmp0(CLASS_PCIECARD_NAME, class_name) == 0) {
        slot_name = PROPETRY_PCIECARD_SLOT;
    } else if (g_strcmp0(CLASS_RISERPCIECARD_NAME, class_name) == 0) {
        slot_name = PROPETRY_RISERPCIECARD_SLOT;
    } else if (g_strcmp0(CLASS_FANBOARD_NAME, class_name) == 0) {
        slot_name = PROPERTY_FANBOARD_SLOT;
    }
    return slot_name;
}

LOCAL gint32 __get_retimer_handle_by_memberid(const char *member_id, OBJ_HANDLE *retimer_handle)
{
    GSList *obj_list = NULL;
    gchar resource_id[MAX_SOFTWARE_ID_LEN] = {0};

    gint32 ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list) != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: Get list of %s failed, ret=%d", __FUNCTION__, CLASS_RETIMER_NAME, ret);
        return RET_ERR;
    }

    for (GSList *obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        guint8 retimer_type = 0xff;
        OBJ_HANDLE object_retimer = (OBJ_HANDLE)obj_node->data;
        if (dal_get_property_value_byte(object_retimer, PROPERTY_RETIMER_CHIP_TYPE, &retimer_type) != RET_OK) {
            continue;
        }
        ret = gen_firmwareinventory_retimer_rsc_id(object_retimer, NULL, resource_id, MAX_SOFTWARE_ID_LEN,
                                                   retimer_type);
        if (ret != RET_OK) {
            continue;
        }
        if (g_ascii_strcasecmp(member_id, resource_id) == 0) {
            *retimer_handle = object_retimer;
            g_slist_free(obj_list);
            return VOS_OK;
        }
    }
    g_slist_free(obj_list);
    return VOS_ERR;
}

LOCAL gint32 get_handle_if_match(
    gchar *class_name, gint32 name_len, const char *member_id, OBJ_HANDLE *out_fw_hd, OBJ_HANDLE *out_comp_hd)
{
    if (get_handle_if_match_soc_board(member_id, out_fw_hd, out_comp_hd) == RET_OK) {
        memberid_class_name_copy_and_check_return(class_name, name_len, CLASS_SOC_BOARD);
        return RET_OK;
    }

    if (get_handle_if_match_io_board(member_id, out_fw_hd, out_comp_hd) == RET_OK) {
        memberid_class_name_copy_and_check_return(class_name, name_len, CLASS_IOBOARD_NAME);
        return RET_OK;
    }
    if (get_handle_if_match_mcu_board(member_id, out_fw_hd) == RET_OK) {
        memberid_class_name_copy_and_check_return(class_name, name_len, CLASS_UPGRADE_MCU_SOFTWARE_NAME);
        return RET_OK;
    }
    return RET_ERR;
}


LOCAL gint32 check_memberid_get_property(const gchar *member_id, gchar *class_name, gint32 name_len,
    OBJ_HANDLE *component_handle, OBJ_HANDLE *resource_obj)
{
    OBJ_HANDLE i_component_handle = 0;
    OBJ_HANDLE i_resource_obj = 0;
    gchar i_class_name[MAX_PROP_NAME] = {0};
    guint8 member_type = 0;
    
    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_EM)) {
        if (check_smm_memberid(member_id, i_class_name, sizeof(i_class_name), &i_resource_obj) == RET_OK) {
            memberid_class_name_copy_and_check_return(class_name, name_len, i_class_name);
            do_if_expr(NULL != resource_obj, (*resource_obj = i_resource_obj));
            return VOS_OK;
        }
        return VOS_ERR;
    }
    

    // memberid为固定值
    if (check_constant_memberid(member_id, i_class_name, sizeof(i_class_name), &i_resource_obj) == VOS_OK) {
        memberid_class_name_copy_and_check_return(class_name, name_len, i_class_name);
        do_if_expr(NULL != resource_obj, (*resource_obj = i_resource_obj));
        return VOS_OK;
    }

    for (guint32 i = 0; i < sizeof(g_handle_by_member) / sizeof(g_handle_by_member[0]); i++) {
        if (g_handle_by_member[i].get_handle_by_memberid(member_id, &i_resource_obj) == RET_OK) {
            (void)strncpy_s(i_class_name, sizeof(i_class_name), g_handle_by_member[i].class_name,
                strlen(g_handle_by_member[i].class_name));
            return_val_if_expr(RET_OK != redfish_get_object_verison(i_class_name, i_resource_obj,
                g_handle_by_member[i].property_name, NULL, 0), RET_ERR);
            memberid_class_name_copy_and_check_return(class_name, name_len, i_class_name);
            do_if_expr(NULL != resource_obj, (*resource_obj = i_resource_obj));
            return RET_OK;
        }
    }

    if (__get_retimer_handle_by_memberid(member_id, &i_resource_obj) == RET_OK) {
        (void)strcpy_s(i_class_name, sizeof(i_class_name), CLASS_RETIMER_NAME);
        if (redfish_get_object_verison(i_class_name, i_resource_obj, PROPERTY_RETIMER_SOFT_VER, NULL, 0) != RET_OK) {
            return RET_ERR;
        }
        memberid_class_name_copy_and_check_return(class_name, name_len, i_class_name);
        do_if_expr(NULL != resource_obj, (*resource_obj = i_resource_obj));
        return RET_OK;
    }
    if (get_handle_if_match(class_name, name_len, member_id, resource_obj, component_handle) == RET_OK) {
        return RET_OK;
    }

    if (get_object_class_name(&member_type, member_id, &i_component_handle, i_class_name, sizeof(i_class_name)) !=
        RET_OK) {
        return RET_ERR;
    }

    // 获取资源句柄
    
    if (component_obj_get_correspond_resource_obj(i_component_handle, i_class_name, sizeof(i_class_name),
        get_class_slot_name(i_class_name), &i_resource_obj) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d:  check memberid fail, %s", __FUNCTION__, __LINE__, member_id);
        return VOS_ERR;
    }

    if (get_object_present_or_verion_validity(member_type, member_id, i_class_name, i_resource_obj) != RET_OK) {
        return RET_ERR;
    }

    memberid_class_name_copy_and_check_return(class_name, name_len, i_class_name);
    do_if_expr(NULL != component_handle, (*component_handle = i_component_handle));

    do_if_expr(NULL != resource_obj, (*resource_obj = i_resource_obj));

    return VOS_OK;
}


gint32 firmwareinventory_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    guchar board_type = 0;

    
    return_val_do_info_if_expr(redfish_get_x86_enable_type(&board_type) != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get object_handle fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr(!is_mainboard_bios_supported() && (0 == g_ascii_strcasecmp(i_paras->member_id, BIOS_CLASS_NAME)),
        HTTP_NOT_FOUND);
    

    
    
    if (check_memberid_get_property(i_paras->member_id, NULL, 0, NULL, &i_paras->obj_handle) != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_firmwareinventory_provider;
    *count = sizeof(g_firmwareinventory_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 __check_peripheral_fw_inventory_uri_valid(const gchar *rsc_id, OBJ_HANDLE *peripheral_fw_handle)
{
    GSList *prop_list = NULL;
    gchar               string_value[FWINV_STR_MAX_LEN] = {0};

    const gchar *fw_name = g_str_case_rstr(rsc_id, RF_PREFIX_PERIPHERAL);
    return_val_do_info_if_fail(NULL != fw_name, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: cann't find [%s] from %s", __FUNCTION__, RF_PREFIX_PERIPHERAL, rsc_id));

    prop_list = g_slist_append(prop_list, g_variant_new_string(PROPERTY_SOFTWARE_NAME));
    gint32 ret = rf_check_rsc_uri_valid_with_property_list(rsc_id + strlen(RF_PREFIX_PERIPHERAL),
        CLASS_PERIPHERAL_FIRMWARE_NAME, prop_list, peripheral_fw_handle);
    g_slist_free_full(prop_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s failed: cann't match any obj handle for [%s] with [%s]", __FUNCTION__,
        CLASS_PERIPHERAL_FIRMWARE_NAME, rsc_id));

    
    (void)dal_get_property_value_string(*peripheral_fw_handle, PROPERTY_SOFTWARE_VERSIONSTRING, string_value,
        sizeof(string_value));
    return_val_do_info_if_fail(0 != strlen(string_value), VOS_ERR,
        debug_log(DLOG_DEBUG, "%s failed: version string for %s is empty", __FUNCTION__,
        dfl_get_object_name(*peripheral_fw_handle)));
    

    return VOS_OK;
}


gint32 get_peripheral_fw_inventory_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gchar               rsc_id[MAX_RSC_ID_LEN] = {0};
    gchar               rsc_uri[MAX_URI_LENGTH] = {0};
    gchar               version_str[MAX_VER_INFO_LEN] = {0};
    json_object *oem_jso = NULL;
    json_object *o_message_jso = NULL;

    if (i_paras == NULL || o_rsc_jso == NULL || o_err_array_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RF_FAILED;
    }

    errno_t ptr_ret = memmove_s(rsc_id, sizeof(rsc_id), RF_PREFIX_PERIPHERAL, strlen(RF_PREFIX_PERIPHERAL));
    return_val_do_info_if_fail(EOK == ptr_ret, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: copy string %s to buffer failed, ret = %d", __FUNCTION__,
        RF_PREFIX_PERIPHERAL, ptr_ret));

    const gchar *obj_name = dfl_get_object_name(i_paras->obj_handle);
    return_val_do_info_if_fail(NULL != obj_name, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: cann't get object name for handle [%" OBJ_HANDLE_FORMAT "]", __FUNCTION__,
        i_paras->obj_handle));

    gint32 ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_SOFTWARE_NAME,
        rsc_id + strlen(RF_PREFIX_PERIPHERAL), sizeof(rsc_id) - strlen(RF_PREFIX_PERIPHERAL));
    return_val_do_info_if_fail(VOS_OK == ret, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s]", __FUNCTION__, obj_name,
        PROPERTY_SOFTWARE_NAME));

    (void)dal_clear_string_blank(rsc_id, sizeof(rsc_id));

    ret = snprintf_s(rsc_uri, sizeof(rsc_uri), sizeof(rsc_uri) - 1, URI_FORMAT_FIRMWARE_INVENTORY, rsc_id);
    return_val_do_info_if_fail(ret > 0, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: format firmware uri for [%s] failed, ret is %d", __FUNCTION__, obj_name,
        ret));

    
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)rsc_uri));
    json_object_object_add(o_rsc_jso, RFPROP_COMMON_ID, json_object_new_string((const gchar *)rsc_id));
    json_object_object_add(o_rsc_jso, RFPROP_COMMON_NAME, json_object_new_string((const gchar *)rsc_id));

    
    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_SOFTWARE_VERSIONSTRING, version_str,
        sizeof(version_str));
    return_val_do_info_if_fail(VOS_OK == ret, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed: cann't get property value for [%s:%s], ret is %d", __FUNCTION__, obj_name,
        PROPERTY_SOFTWARE_VERSIONSTRING, ret));

    dal_trim_string(version_str, MAX_VER_INFO_LEN);
    json_object_object_add(o_rsc_jso, RFPROP_FWINV_VERSION, json_object_new_string((const gchar *)version_str));

    
    i_paras->is_satisfy_privi = TRUE;
    if ((ret = get_firmwareinventory_oem(i_paras, &o_message_jso, &oem_jso)) != HTTP_OK) {
        (void)json_object_put(o_message_jso);
        (void)json_object_put(oem_jso);
        debug_log(DLOG_ERROR, "%s failed: cann't get oem property, ret is %d", __FUNCTION__, ret);
        return RF_OK;
    }

    json_object_object_add(o_rsc_jso, RFPROP_COMMON_OEM, oem_jso);

    
    json_object_object_add(o_rsc_jso, RFPROP_FWINV_RELATEDITEM, json_object_new_array());

    return RF_OK;
}


LOCAL PROPERTY_PROVIDER_S g_peripheral_fw_inventory_provider[] = {
};


gint32 peripheral_fw_inventory_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    gint32 ret = __check_peripheral_fw_inventory_uri_valid(i_paras->member_id, &i_paras->obj_handle);
    if (VOS_ERR == ret) {
        debug_log(DLOG_ERROR, "%s failed: check uri valid failed for [%s]", __FUNCTION__, i_paras->member_id);

        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_peripheral_fw_inventory_provider;
    
    *count = 0;

    return VOS_OK;
}
