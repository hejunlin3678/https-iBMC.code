
#include "redfish_provider.h"

#define LIC_SHORT_STREAM_LEN 136
#define LICENSE_MAX_ESN_LEN 2048
#define LICENSE_UNINSTALLED "NotInstalled"
#define LICENSE_INSTALLED "Installed"
#define LICENSE_INSTALLING "Installing"
#define LICENSE_UNINSTALLED_INDEX 0
#define LICENSE_INSTALLED_INDEX 1
#define LICENSE_INSTALLING_INDEX 0x80

#define LICENSE_INSTALL_SUCCESS_INDEX 0
#define LICENSE_INSTALL_FAIL_INDEX 1
#define LICENSE_INSTALL_SUCCESS "Success"
#define LICENSE_INSTALL_FAIL "Fail"
#define LICENSE_INSTALL_INVALID "Invalid"

#define LICENSE_CLASS_UNINSTALL_INDEX 0
#define LICENSE_CLASS_ADVANCED_INDEX 1
#define LICENSE_CLASS_UNINSTALL "NotInstalled"
#define LICENSE_CLASS_ADVANCED "Advanced"

#define LICENSE_STATUS_UNDEFINED "N/A"
#define LICENSE_STATUS_DEFAULT "Default"
#define LICENSE_STATUS_GRACE_PERIOD "GracePeriod"
#define LICENSE_STATUS_NORMAL "Normal"
#define LICENSE_STATUS_EMERGENCY "Emergency"
#define LICENSE_STATUS_COMMISSIONING "Commissioning"
#define LICENSE_STATUS_UNKOWN "Unknown"
#define LICENSE_STATUS_UNKOWN_INDEX 0xff
#define LICENSE_STATUS_LEN 30
#define LICENSE_STATUS_MAX_ITEM 6

#define LICENSE_FEATURE_NAME_MAX_LEN 0x48
#define LICENSE_INFO_MAX_LEN 64

#define LICENSE_FEATURE_NOCONTROL_INDEX 1
#define LICENSE_FEATURE_CONTROL_INDEX 0
#define LICENSE_FEATURE_NO_CONTROL "Uncontrolled"
#define LICENSE_FEATURE_CONTROL "Controlled"
#define LICENSE_FEATURE_UNDEFINE "N/A"
#define LICENSE_FEATURE_DEFAULT "Default"
#define LICENSE_FEATURE_GRACE_PERIOD "GracePeriod"
#define LICENSE_FEATURE_NORMAL "Normal"
#define LICENSE_FEATURE_EMERGENCY "Emergency"
#define LICENSE_FEATURE_COMMISSIONING "Commissioning"

#define LICENSE_FILE_SOURCE_BMC "iBMC"
#define LICENSE_FILE_SOURCE_FD "FusionDirector"
#define LICENSE_FILE_SOURCE_ESIGHT "eSight"
#define LICENSE_FILE_SOURCE_BMC_INDEX 0
#define LICENSE_FILE_SOURCE_FD_INDEX 1
#define LICENSE_FILE_SOURCE_ESIGHT_INDEX 2

#define LICENSE_CAP_LOCAL "Local"
#define LICENSE_CAP_REMOTE "Remote"

typedef enum tag_LM_bit {
    LICENSE_CAP_ZERO = 0x01,
    LICENSE_CAP_ONE = 0x02
} LICENSE_CAP_BIT;

#define LICENSE_ALARM_MAX_INDEX 2
#define LICENSE_ALARM_MATCH "Matched"
#define LICENSE_ALARM_NOT_MATCH "Unmatched"
#define LICENSE_ALARM_ESN_INVALID "Invalid"
#define LICENSE_ALARM_ESN_VALID "Valid"
#define LICENSE_ALARM_FILE_NORMAL "Normal"
#define LICENSE_ALARM_FILE_INCORRECT "Incorrect"
#define LICENSE_ALARM_UNDEFINED "N/A"
#define LICENSE_ALARM_UNDEFINED_INDEX 0xff


#define LICENSE_MAX_LEN 2097152
#define LICENSE_TMP_FILE_PATH "/tmp/license.xml"

#define MAX_FEAT_KEY_STATE 6


typedef void (*GetInfoFun)(GVariant *array_item, json_object **o_result_jso);

LOCAL gint32 act_install_license_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso);
LOCAL gint32 act_export_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_revoke_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_delete_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_license_capability(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_license_device_ESN(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_license_installed_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_license_revoket_ticket(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_license_class(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_license_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_license_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_alarm_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_manager_license_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_licsrv_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_license_service_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL const gchar*       g_manager_licsrv_action_array[] = {
    RFACTION_LICENSE_SERVICE_INSTALL,
    RFACTION_LICENSE_SERVICE_EXPORT,
    RFACTION_LICENSE_SERVICE_REVOKE,
    RFACTION_LICENSE_SERVICE_DELETE
};

LOCAL PROPERTY_PROVIDER_S g_manager_licsrv_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_service_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_CAPABILITY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_capability, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_DEVICE_ESN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_device_ESN, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_INSTALLED_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_installed_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_REVOKE_TICKET, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_revoket_ticket, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_CLASS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_class, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_LICENSE_INFO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_license_info, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICSVC_ALARM_INFO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_alarm_info, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_license_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LICENSE_SERVICE_INSTALL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_install_license_entry, ETAG_FLAG_ENABLE},
    {RFPROP_LICENSE_SERVICE_EXPORT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_license, ETAG_FLAG_ENABLE},
    {RFPROP_LICENSE_SERVICE_REVOKE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_revoke_license, ETAG_FLAG_ENABLE},
    {RFPROP_LICENSE_SERVICE_DELETE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_delete_license, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S g_manager_licsrv_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_licsrv_actioninfo, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_license_service_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, "/redfish/v1/Managers/%s/LicenseService");
}


LOCAL gint32 get_license_capability(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *cap_str = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 cap = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_DEVICE_ESN, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_MANAGE));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LM_LIC_CAPABILITY, &cap);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get license device ESN failed.\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();

    // 是否支持管理本地license
    if (cap & LICENSE_CAP_ZERO) {
        cap_str = json_object_new_string(LICENSE_CAP_LOCAL);
        ret = json_object_array_add(*o_result_jso, cap_str);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(cap_str);
            debug_log(DLOG_ERROR, "%s: add to object array failed", __FUNCTION__));
    }
    // 是否支持管理远程license
    if (cap & LICENSE_CAP_ONE) {
        cap_str = json_object_new_string(LICENSE_CAP_REMOTE);
        ret = json_object_array_add(*o_result_jso, cap_str);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(cap_str);
            debug_log(DLOG_ERROR, "%s: add to object array failed", __FUNCTION__));
    }

    return HTTP_OK;
}


LOCAL gint32 get_license_device_ESN(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar device_ESN[LICENSE_MAX_ESN_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_DEVICE_ESN, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_MANAGE));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LM_PRODUCT_ESN, device_ESN, sizeof(device_ESN));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get license device ESN failed.\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)device_ESN);

    return HTTP_OK;
}


LOCAL gint32 get_license_installed_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 status = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_INSTALLED_STATUS, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_MANAGE));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LM_INSTALL_STATUS, &status);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LM_INSTALL_STATUS));

    switch (status) {
        case LICENSE_UNINSTALLED_INDEX:
            *o_result_jso = json_object_new_string(LICENSE_UNINSTALLED);
            break;
        case LICENSE_INSTALLED_INDEX:
            *o_result_jso = json_object_new_string(LICENSE_INSTALLED);
            break;
        case LICENSE_INSTALLING_INDEX:
            *o_result_jso = json_object_new_string(LICENSE_INSTALLING);
            break;
        default:
            
            debug_log(DLOG_DEBUG, "%s, undefined status = %u", __FUNCTION__, status);
            
            break;
    }

    return HTTP_OK;
}


LOCAL gint32 get_license_revoket_ticket(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar revoke_ticket[LM_SHORT_STREAM_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_REVOKE_TICKET, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_MANAGE));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LM_REVOKE_TICKET, revoke_ticket, sizeof(revoke_ticket));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get license revoke failed.\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)revoke_ticket);

    return HTTP_OK;
}


LOCAL gint32 get_license_class(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 lic_class = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_CLASS, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_MANAGE));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LM_LIC_CLASS, &lic_class);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LM_LIC_CLASS));

    switch (lic_class) {
        case LICENSE_CLASS_UNINSTALL_INDEX:
            *o_result_jso = json_object_new_string(LICENSE_CLASS_UNINSTALL);
            break;
        case LICENSE_CLASS_ADVANCED_INDEX:
            *o_result_jso = json_object_new_string(LICENSE_CLASS_ADVANCED);
            break;
        default:
            
            debug_log(DLOG_DEBUG, "%s, undefined license class = %u", __FUNCTION__, lic_class);
            
            break;
    }

    return HTTP_OK;
}


LOCAL gint32 get_license_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 status = 0;

    gchar status_table[][LICENSE_STATUS_LEN] = {LICENSE_STATUS_UNDEFINED, LICENSE_STATUS_DEFAULT, LICENSE_STATUS_GRACE_PERIOD,
                                                LICENSE_STATUS_NORMAL, LICENSE_STATUS_EMERGENCY, LICENSE_STATUS_COMMISSIONING};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_STATUS, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_MANAGE));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LM_LIC_STATUS, &status);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LM_LIC_STATUS));

    if (status == LICENSE_STATUS_UNKOWN_INDEX) {
        *o_result_jso = json_object_new_string(LICENSE_STATUS_UNKOWN);
    } else if (status < LICENSE_STATUS_MAX_ITEM) {
        *o_result_jso = json_object_new_string(status_table[status]);
    } else {
        
        debug_log(DLOG_DEBUG, "%s: get license status invalid, status = %u", __FUNCTION__, status);
        
    }

    return HTTP_OK;
}


LOCAL gint32 get_general_info(json_object **o_result_jso)
{
    json_object *general_info_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *prop_name_list = NULL;
    GSList *prop_val_list = NULL;
    const gchar *copy_right = NULL;
    const gchar *lic_SN = NULL;
    const gchar *lic_type = NULL;
    guint32 grace_day;
    const gchar *lic_creator = NULL;
    const gchar *lic_issuer = NULL;
    const gchar *lic_create_time = NULL;

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_GENERAL_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_GENERAL_INFO));

    
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_COPY_RIGHT);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_SN);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_TYPE);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_GRACE_DAY);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_CREATOR);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ISSUER);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_CREATE_TIME);

    ret = dfl_multiget_property_value(obj_handle, prop_name_list, &prop_val_list);
    g_slist_free(prop_name_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s failed:cann't get mutiple properties", __FUNCTION__));
    
    general_info_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != general_info_jso, VOS_ERR, uip_free_gvariant_list(prop_val_list);
        debug_log(DLOG_ERROR, "%s failed:alloc a new jso failed", __FUNCTION__));

    copy_right = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 0), NULL);
    lic_SN = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 1), NULL);
    lic_type = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 2), NULL);
    grace_day = g_variant_get_uint32((GVariant *)g_slist_nth_data(prop_val_list, 3));
    lic_creator = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 4), NULL);
    lic_issuer = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 5), NULL);
    lic_create_time = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 6), NULL);

    json_object_object_add(general_info_jso, RFPROP_LICSVC_COPY_RIGHT, json_object_new_string(copy_right));
    json_object_object_add(general_info_jso, RFPROP_LICSVC_LSN, json_object_new_string(lic_SN));
    json_object_object_add(general_info_jso, RFPROP_LICSVC_LIC_TYPE, json_object_new_string(lic_type));
    json_object_object_add(general_info_jso, RFPROP_LICSVC_GRACE_DAY, json_object_new_int64((gint64)grace_day));
    json_object_object_add(general_info_jso, RFPROP_LICSVC_CREATOR, json_object_new_string(lic_creator));
    json_object_object_add(general_info_jso, RFPROP_LICSVC_ISSUER, json_object_new_string(lic_issuer));
    json_object_object_add(general_info_jso, RFPROP_LICSVC_CREATE_TIME, json_object_new_string(lic_create_time));

    uip_free_gvariant_list(prop_val_list);

    *o_result_jso = general_info_jso;

    return VOS_OK;
}


LOCAL gint32 get_customer_info(json_object **o_result_jso)
{
    json_object *customer_info_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar customer_name[LICENSE_INFO_MAX_LEN] = {0};
    gchar customer_country[LICENSE_INFO_MAX_LEN] = {0};
    gchar customer_office[LICENSE_INFO_MAX_LEN] = {0};

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_GENERAL_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_GENERAL_INFO));

    customer_info_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == customer_info_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LIC_CUSTOMER, customer_name, sizeof(customer_name));
    if (VOS_OK == ret) {
        json_object_object_add(customer_info_jso, RFPROP_LICSVC_CUSTOMER,
            json_object_new_string((const gchar *)customer_name));
    } else {
        debug_log(DLOG_ERROR, "%s, %d: get property %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LIC_CUSTOMER);
        json_object_object_add(customer_info_jso, RFPROP_LICSVC_CUSTOMER, NULL);
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LIC_COUNTRY, customer_country, sizeof(customer_country));
    if (VOS_OK == ret) {
        json_object_object_add(customer_info_jso, RFPROP_LICSVC_COUNTRY,
            json_object_new_string((const gchar *)customer_country));
    } else {
        debug_log(DLOG_ERROR, "%s, %d: get property %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LIC_COUNTRY);
        json_object_object_add(customer_info_jso, RFPROP_LICSVC_COUNTRY, NULL);
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LIC_OFFICE, customer_office, sizeof(customer_office));
    if (VOS_OK == ret) {
        json_object_object_add(customer_info_jso, RFPROP_LICSVC_OFFICE,
            json_object_new_string((const gchar *)customer_office));
    } else {
        debug_log(DLOG_ERROR, "%s, %d: get property %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LIC_OFFICE);
        json_object_object_add(customer_info_jso, RFPROP_LICSVC_OFFICE, NULL);
    }

    *o_result_jso = customer_info_jso;

    return VOS_OK;
}


LOCAL gint32 get_node_info(json_object **o_result_jso)
{
    json_object *node_info_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar node_name[LICENSE_INFO_MAX_LEN] = {0};
    gchar node_ESN[LICENSE_MAX_ESN_LEN] = {0};

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_GENERAL_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_GENERAL_INFO));

    node_info_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == node_info_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LIC_NODE_NAME, node_name, sizeof(node_name));
    if (VOS_OK == ret) {
        json_object_object_add(node_info_jso, RFPROP_LICSVC_NODE_NAME,
            json_object_new_string((const gchar *)node_name));
    } else {
        debug_log(DLOG_ERROR, "%s, %d: get property %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LIC_NODE_NAME);
        json_object_object_add(node_info_jso, RFPROP_LICSVC_NODE_NAME, NULL);
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LIC_NODE_ESN, node_ESN, sizeof(node_ESN));
    if (VOS_OK == ret) {
        json_object_object_add(node_info_jso, RFPROP_LICSVC_NODE_ESN, json_object_new_string((const gchar *)node_ESN));
    } else {
        debug_log(DLOG_ERROR, "%s, %d: get property %s failed.\n", __FUNCTION__, __LINE__, PROPERTY_LIC_NODE_ESN);
        json_object_object_add(node_info_jso, RFPROP_LICSVC_NODE_ESN, NULL);
    }

    *o_result_jso = node_info_jso;

    return VOS_OK;
}


LOCAL gint32 get_one_feat_key(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    GSList *prop_name_list = NULL;
    GSList *prop_val_list = NULL;
    guint8 feature_value;
    gint32 ret;  

    gchar status_table[][LICENSE_STATUS_LEN] = {
        LICENSE_FEATURE_UNDEFINE, LICENSE_FEATURE_DEFAULT, LICENSE_FEATURE_GRACE_PERIOD,
        LICENSE_FEATURE_NORMAL, LICENSE_FEATURE_EMERGENCY, LICENSE_FEATURE_COMMISSIONING
    };

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_FEATURE_NAME);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_GROUP_NAME);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_CONTROL_VALUE);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_VALID_DATE);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_IS_NOCONTROL);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_CURRENT_STATE);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_FK_REMOTE_CONTROL_VALUE);

    ret = dfl_multiget_property_value(obj_handle, prop_name_list, &prop_val_list);
    g_slist_free(prop_name_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s failed:cann't get mutiple properties", __FUNCTION__));
    
    json_object *info_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != info_jso, VOS_ERR, uip_free_gvariant_list(prop_val_list);
        debug_log(DLOG_ERROR, "%s failed:alloc a new jso failed", __FUNCTION__));

    const gchar *feature_name = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 0), NULL);
    const gchar *group_name = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 1), NULL);
    guint8 control_val = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 2));
    const gchar *valid_date = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 3), NULL);
    guint8 control = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 4));
    guint8 current_state = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 5));
    guint8 remote_control = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 6));
    if (remote_control != 0 || control_val != 0) {
        feature_value = 1; 
    } else {
        feature_value = 0;    
    }
    json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_NAME, json_object_new_string(feature_name));
    json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_GROUP, json_object_new_string(group_name));
    json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_VALUE, json_object_new_int(feature_value));
    json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_VALID_DATE, json_object_new_string(valid_date));

    switch (control) {
        case LICENSE_FEATURE_NOCONTROL_INDEX:
            json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_NO_CONTROL,
                json_object_new_string(LICENSE_FEATURE_NO_CONTROL));
            break;
        case LICENSE_FEATURE_CONTROL_INDEX:
            json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_NO_CONTROL,
                json_object_new_string(LICENSE_FEATURE_CONTROL));
            break;
        default:
            json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_NO_CONTROL, NULL);
            
            debug_log(DLOG_DEBUG, "%s: invalid feature control = %u.", __FUNCTION__, control);
            
            break;
    }

    if (current_state < MAX_FEAT_KEY_STATE) {
        json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_STATE,
            json_object_new_string(status_table[current_state]));
    } else {
        json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_STATE, NULL);
        
        debug_log(DLOG_DEBUG, "invalid value of property %s", PROPERTY_LIC_FK_CURRENT_STATE);
        
    }

    json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_REMOTE_CONTROL_VALUE, json_object_new_int(remote_control));

    uip_free_gvariant_list(prop_val_list);

    *o_result_jso = info_jso;

    return VOS_OK;
}


LOCAL gint32 get_one_sale_item_info(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    GSList *prop_name_list = NULL;
    GSList *prop_val_list = NULL;
    gint32 ret;
    json_object *info_jso = NULL;
    const gchar *sale_name = NULL;
    guint32 sale_value;
    const gchar *valid_date = NULL;
    const gchar *sale_desc = NULL;
    GVariant *array_feat_key_gvar = NULL;
    json_object *feat_key_jso = NULL;
    GVariant *feature_gvar = NULL;
    GVariantIter iter;
    const gchar *feature_string = NULL;
    json_object *one_feat_key_jso = NULL;
    guint8 control;

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_SALE_ITEM_NAME);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_SALE_ITEM_VALUE);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_SALE_ITEM_VALID_DATE);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_SALE_ITEM_DESC);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_SALE_ITEM_IS_NOCONTROL);

    ret = dfl_multiget_property_value(obj_handle, prop_name_list, &prop_val_list);
    g_slist_free(prop_name_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s failed:cann't get mutiple properties", __FUNCTION__));
    
    info_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != info_jso, VOS_ERR, uip_free_gvariant_list(prop_val_list);
        debug_log(DLOG_ERROR, "%s failed:alloc a new jso failed", __FUNCTION__));

    sale_name = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 0), NULL);
    sale_value = g_variant_get_uint32((GVariant *)g_slist_nth_data(prop_val_list, 1));
    valid_date = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 2), NULL);
    sale_desc = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 3), NULL);
    control = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 4));

    json_object_object_add(info_jso, RFPROP_LICSVC_SALE_NAME, json_object_new_string(sale_name));
    json_object_object_add(info_jso, RFPROP_LICSVC_SALE_VALUE, json_object_new_int64((int64_t)sale_value));
    json_object_object_add(info_jso, RFPROP_LICSVC_SALE_VALID_DATE, json_object_new_string(valid_date));

    switch (control) {
        case LICENSE_FEATURE_NOCONTROL_INDEX:
            json_object_object_add(info_jso, RFPROP_LICSVC_SALE_CONTROL_VALUE,
                json_object_new_string(LICENSE_FEATURE_NO_CONTROL));
            break;
        case LICENSE_FEATURE_CONTROL_INDEX:
            json_object_object_add(info_jso, RFPROP_LICSVC_SALE_CONTROL_VALUE,
                json_object_new_string(LICENSE_FEATURE_CONTROL));
            break;
        default:
            json_object_object_add(info_jso, RFPROP_LICSVC_SALE_CONTROL_VALUE, NULL);
            
            debug_log(DLOG_DEBUG, "%s: invalid item control = %u.", __FUNCTION__, control);
            
            break;
    }

    json_object_object_add(info_jso, RFPROP_LICSVC_SALE_DESCRIPTION, json_object_new_string(sale_desc));

    uip_free_gvariant_list(prop_val_list);

    
    ret = dfl_get_property_value(obj_handle, PROPERTY_LIC_SALE_ITEM_FK, &array_feat_key_gvar);
    return_val_do_info_if_fail(VOS_OK == ret, ret, json_object_put(info_jso);
        debug_log(DLOG_ERROR, "%s: get array %s failed.\n", __FUNCTION__, PROPERTY_LIC_SALE_ITEM_FK));

    feat_key_jso = json_object_new_array();
    if (array_feat_key_gvar != NULL) {
        g_variant_iter_init(&iter, array_feat_key_gvar);

        while (NULL != (feature_gvar = g_variant_iter_next_value(&iter))) {
            feature_string = g_variant_get_string(feature_gvar, NULL);
            obj_handle = 0;
            ret = dfl_get_object_handle(feature_string, &obj_handle);
            goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
                debug_log(DLOG_ERROR, "%s: get the obj handle %s failed.", __FUNCTION__, feature_string));

            ret = get_one_feat_key(obj_handle, &one_feat_key_jso);
            goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
                
                debug_log(DLOG_DEBUG, "%s: get one sale item %s failed.", __FUNCTION__, feature_string));
            
            ret = json_object_array_add(feat_key_jso, one_feat_key_jso);
            continue_do_info_if_fail(VOS_OK == ret, (void)json_object_put(one_feat_key_jso);
                g_variant_unref(feature_gvar); debug_log(DLOG_ERROR, "%s: add to object array failed", __FUNCTION__));
            g_variant_unref(feature_gvar);
        }
    }
    g_variant_unref(array_feat_key_gvar);

    json_object_object_add(info_jso, RFPROP_LICSVC_FEATURE_KEY, feat_key_jso);

    *o_result_jso = info_jso;

    return VOS_OK;

err_exit:
    g_variant_unref(feature_gvar);
    g_variant_unref(array_feat_key_gvar);
    json_object_put(info_jso);
    json_object_put(feat_key_jso);
    return ret;
}


LOCAL gint32 get_sale_info(json_object **o_result_jso)
{
    json_object *product_jso = NULL;
    json_object *sale_item_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar product_name[LICENSE_INFO_MAX_LEN] = {0};
    json_object *one_item_jso = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_SALE_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_SALE_INFO));

    product_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == product_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_LIC_SALE_PRODUCT_NAME, product_name, sizeof(product_name));
    if (VOS_OK == ret) {
        json_object_object_add(product_jso, RFPROP_LICSVC_PRODUCT_NAME,
            json_object_new_string((const gchar *)product_name));
    } else {
        debug_log(DLOG_ERROR, "%s: get property %s failed.\n", __FUNCTION__, PROPERTY_LIC_SALE_PRODUCT_NAME);
        json_object_object_add(product_jso, RFPROP_LICSVC_PRODUCT_NAME, NULL);
    }

    
    ret = dfl_get_object_list(CLASS_NAME_LICENSE_SALE_ITEM, &obj_list);
    return_val_do_info_if_expr(obj_list == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(product_jso);
        debug_log(DLOG_ERROR, "%s: get sale item object list fail", __FUNCTION__));

    sale_item_jso = json_object_new_array();
    return_val_do_info_if_expr(sale_item_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(product_jso);
        g_slist_free(obj_list); debug_log(DLOG_ERROR, "%s: json_object_new_array list fail", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        one_item_jso = NULL;
        ret = get_one_sale_item_info(obj_handle, &one_item_jso);
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
            
            debug_log(DLOG_DEBUG, "%s: get one sale item %s failed.\n", __FUNCTION__, dfl_get_object_name(obj_handle)));
        
        ret = json_object_array_add(sale_item_jso, one_item_jso);
        continue_do_info_if_fail(ret == VOS_OK, (void)json_object_put(one_item_jso);
            debug_log(DLOG_ERROR, "%s: add to object array failed", __FUNCTION__));
    }
    g_slist_free(obj_list);

    json_object_object_add(product_jso, RFPROP_LICSVC_SALE_ITEM, sale_item_jso);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(product_jso);
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));
    json_object_object_add(*o_result_jso, RFPROP_LICSVC_OFFER_PRODUCT, product_jso);

    return VOS_OK;

err_exit:
    g_slist_free(obj_list);
    json_object_put(product_jso);
    json_object_put(sale_item_jso);
    return ret;
}


LOCAL gint32 get_key_info(json_object **o_result_jso)
{
    json_object *key_info_jso = NULL;
    json_object *key_item_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *one_feat_key_jso = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    return_val_do_info_if_fail(o_result_jso != NULL, VOS_ERR, debug_log(DLOG_ERROR, "input o_result_jso is NULL"));

    key_info_jso = json_object_new_object();
    return_val_do_info_if_expr(key_info_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

    
    ret = dfl_get_object_list(CLASS_NAME_LICENSE_FEATURE_KEY, &obj_list);
    return_val_do_info_if_expr(obj_list == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(key_info_jso);
        debug_log(DLOG_ERROR, "%s: get FeatureKye object list fail", __FUNCTION__));

    key_item_jso = json_object_new_array();
    return_val_do_info_if_expr(key_item_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(key_info_jso);
        g_slist_free(obj_list); debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        one_feat_key_jso = NULL;
        ret = get_one_feat_key(obj_handle, &one_feat_key_jso);
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
            debug_log(DLOG_DEBUG, "%s: get one FeatureKey %s failed.\n", __FUNCTION__,
            dfl_get_object_name(obj_handle)));

        ret = json_object_array_add(key_item_jso, one_feat_key_jso);
        continue_do_info_if_fail(ret == VOS_OK, (void)json_object_put(one_feat_key_jso);
            debug_log(DLOG_ERROR, "%s: add to object array failed", __FUNCTION__));
    }
    g_slist_free(obj_list);

    json_object_object_add(key_info_jso, RFPROP_LICSVC_FEATURE_KEY, key_item_jso);

    *o_result_jso = key_info_jso;

    return VOS_OK;

err_exit:
    g_slist_free(obj_list);
    json_object_put(key_info_jso);
    json_object_put(key_item_jso);
    return ret;
}


LOCAL gint32 get_license_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar format_version[LICENSE_INFO_MAX_LEN] = {0};
    json_object *lic_info_jso = NULL;
    json_object *general_info_jso = NULL;
    json_object *customer_info_jso = NULL;
    json_object *node_info_jso = NULL;
    json_object *sale_info_jso = NULL;
    json_object *key_info_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_LICENSE_INFO, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    lic_info_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == lic_info_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new object fail", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_GENERAL_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(lic_info_jso);
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_GENERAL_INFO));

    
    ret =
        dal_get_property_value_string(obj_handle, PROPERTY_LIC_FORMAT_VERSION, format_version, sizeof(format_version));
    if (VOS_OK == ret) {
        json_object_object_add(lic_info_jso, RFPROP_LICSVC_FORMAT_VERSION,
            json_object_new_string((const gchar *)format_version));
    } else {
        debug_log(DLOG_ERROR, "%s: get license format version failed.\n", __FUNCTION__);
        json_object_object_add(lic_info_jso, RFPROP_LICSVC_FORMAT_VERSION, NULL);
    }

    
    
    ret = get_general_info(&general_info_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(lic_info_jso);
        debug_log(DLOG_DEBUG, "%s: get license general info failed.\n", __FUNCTION__));
    json_object_object_add(lic_info_jso, RFPROP_LICSVC_GENERAL_INFO, general_info_jso);

    
    ret = get_customer_info(&customer_info_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(lic_info_jso);
        debug_log(DLOG_DEBUG, "%s: get license general info failed.\n", __FUNCTION__));
    json_object_object_add(lic_info_jso, RFPROP_LICSVC_CUSTOMER_INFO, customer_info_jso);

    
    ret = get_node_info(&node_info_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(lic_info_jso);
        debug_log(DLOG_DEBUG, "%s: get license general info failed.\n", __FUNCTION__));
    json_object_object_add(lic_info_jso, RFPROP_LICSVC_NODE_INFO, node_info_jso);

    
    ret = get_sale_info(&sale_info_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(lic_info_jso);
        debug_log(DLOG_DEBUG, "%s: get license general info failed.\n", __FUNCTION__));
    
    json_object_object_add(lic_info_jso, RFPROP_LICSVC_SALE_INFO, sale_info_jso);

    
    ret = get_key_info(&key_info_jso);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR, json_object_put(lic_info_jso);
        debug_log(DLOG_DEBUG, "%s: get license general info failed.\n", __FUNCTION__));
    
    json_object_object_add(lic_info_jso, RFPROP_LICSVC_KEY_INFO, key_info_jso);
    *o_result_jso = lic_info_jso;

    return HTTP_OK;
}


LOCAL gint32 get_alarm_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *alarm_info = NULL;
    GSList *prop_name_list = NULL;
    GSList *prop_val_list = NULL;
    guint32 remain_grace_day;
    guint32 remain_comm_day;
    guint8 ESN_valid;
    guint8 file_error;
    guint8 ESN_match;
    guint8 version_match;

    gchar ESN_valid_str[][LICENSE_STATUS_LEN] = {LICENSE_ALARM_ESN_VALID, LICENSE_ALARM_ESN_INVALID};
    gchar file_err_str[][LICENSE_STATUS_LEN] = {LICENSE_ALARM_FILE_NORMAL, LICENSE_ALARM_FILE_INCORRECT};
    gchar match_str[][LICENSE_STATUS_LEN] = {LICENSE_ALARM_MATCH, LICENSE_ALARM_NOT_MATCH};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (0 == (i_paras->user_role_privilege & USERROLE_READONLY)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_LICSVC_LICENSE_INFO, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_ALARM_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "cann't get object handle for %s", CLASS_NAME_LICENSE_ALARM_INFO));

    
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ALARM_REMAIN_GRACEDAYS);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ALARM_REMAIN_COMMISSIONINGDAYS);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ALARM_PRODUCT_ESN_INVALID);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ALARM_FILE_ERROR);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ALARM_ESN_NOT_MATCH);
    prop_name_list = g_slist_append(prop_name_list, PROPERTY_LIC_ALARM_VER_NOT_MATCH);

    ret = dfl_multiget_property_value(obj_handle, prop_name_list, &prop_val_list);
    g_slist_free(prop_name_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s failed:cann't get mutiple properties, ret = %d", __FUNCTION__, ret));
    
    alarm_info = json_object_new_object();
    return_val_do_info_if_fail(NULL != alarm_info, HTTP_INTERNAL_SERVER_ERROR, uip_free_gvariant_list(prop_val_list);
        debug_log(DLOG_ERROR, "%s failed:alloc a new jso failed", __FUNCTION__));

    remain_grace_day = g_variant_get_uint32((GVariant *)g_slist_nth_data(prop_val_list, 0));
    remain_comm_day = g_variant_get_uint32((GVariant *)g_slist_nth_data(prop_val_list, 1));
    ESN_valid = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 2));
    file_error = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 3));
    ESN_match = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 4));
    version_match = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_val_list, 5));

    json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_REMAIN_GRACE_DAY,
        json_object_new_int64((gint64)remain_grace_day));
    json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_REMAIN_COMM_DAY,
        json_object_new_int64((gint64)remain_comm_day));

    if (ESN_valid < LICENSE_ALARM_MAX_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_ESN_VALID,
            json_object_new_string(ESN_valid_str[ESN_valid]));
    } else if (ESN_valid == LICENSE_ALARM_UNDEFINED_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_ESN_VALID,
            json_object_new_string(LICENSE_ALARM_UNDEFINED));
    } else {
        
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_ESN_VALID, NULL);
        debug_log(DLOG_DEBUG, "%s: invalid ESN valide state = %u.", __FUNCTION__, ESN_valid);
    }

    if (file_error < LICENSE_ALARM_MAX_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_FILE_SATE,
            json_object_new_string(file_err_str[file_error]));
    } else if (file_error == LICENSE_ALARM_UNDEFINED_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_FILE_SATE,
            json_object_new_string(LICENSE_ALARM_UNDEFINED));
    } else {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_FILE_SATE, NULL);
        debug_log(DLOG_DEBUG, "%s: invalid file error state = %u.", __FUNCTION__, file_error);
    }

    if (ESN_match < LICENSE_ALARM_MAX_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_ESN_MATCH,
            json_object_new_string(match_str[ESN_match]));
    } else if (ESN_match == LICENSE_ALARM_UNDEFINED_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_ESN_MATCH,
            json_object_new_string(LICENSE_ALARM_UNDEFINED));
    } else {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_ESN_MATCH, NULL);
        debug_log(DLOG_DEBUG, "%s: invalid ESN match state = %u.", __FUNCTION__, ESN_match);
    }

    if (version_match < LICENSE_ALARM_MAX_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_VERSION_MATCH,
            json_object_new_string(match_str[version_match]));
    } else if (version_match == LICENSE_ALARM_UNDEFINED_INDEX) {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_VERSION_MATCH,
            json_object_new_string(LICENSE_ALARM_UNDEFINED));
    } else {
        json_object_object_add(alarm_info, RFPROP_LICSVC_ALARM_PRODUCT_VERSION_MATCH, NULL);
        debug_log(DLOG_DEBUG, "%s: invalid version match state = %u.", __FUNCTION__, version_match);
    }
    

    uip_free_gvariant_list(prop_val_list);

    *o_result_jso = alarm_info;

    return HTTP_OK;
}


LOCAL gint32 set_install_err_code(gint32 result_code, json_object **o_message_jso)
{
    gint32 ret = 0;

    debug_log(DLOG_DEBUG, "result_code = %d", result_code);

    switch (result_code) {
        case LM_OK:
            ret = HTTP_OK;
            (void)create_message_info(MSGID_INSTALL_LICENSE_SUCCESS, NULL, o_message_jso);
            break;
        case LM_ERR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_INSTALL_LICENSE_FAILED, NULL, o_message_jso);
            break;
        case LM_ERR_VERIFY:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_VERIFY_LICENSE_FAILED, NULL, o_message_jso);
            break;
        case LM_ERR_ACTIVATE:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_ACTIVATE_LICENSE_FAILED, NULL, o_message_jso);
            break;
        case LM_ERR_PARAMETER:
        case LM_ERR_BUSY:
        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }
    return ret;
}


LOCAL void exec_install_license(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj, 
    gchar* file_path, guint8 source)
{
    GSList *input_list = NULL;
    const gchar *user_name = NULL;
    const gchar *client_ip = NULL;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    gint32 ret;

    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP, &client_ip);
    
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    // 调用方法开始安装license
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_path));
    input_list = g_slist_append(input_list, g_variant_new_byte(source));

    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client_ip, OBJ_HANDLE_COMMON,
        CLASS_NAME_LICENSE_MANAGE, METHOD_LM_INSTALL_LIC, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    

    ret = set_install_err_code(ret, message_obj);
    if (ret != HTTP_OK) {
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        debug_log(DLOG_ERROR, "%s: call class method %s failed!", __FUNCTION__, METHOD_LM_INSTALL_LIC);
        return;
    }

    monitor_fn_data->task_state = RF_TASK_COMPLETED;
    monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
}


LOCAL gint32 license_install_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 file_trans_progress = 0;
    gchar file_path[MAX_URL_LEN] = {0};
    const gchar *content = NULL;
    const gchar *source_para = NULL;
    guint8 source = 0;
    gchar *filename = NULL;
 
    return_val_do_info_if_expr((NULL == origin_obj_path) || (NULL == body_jso) || (NULL == monitor_fn_data) ||
        (NULL == message_obj),
        RF_OK, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    monitor_fn_data->task_progress = RF_TASK_RUNNING;
    ret = get_file_transfer_progress(&file_trans_progress);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "get upgrade packege download progress failed"));

    if (file_trans_progress < 0) {
        
        (void)parse_file_transfer_err_code(file_trans_progress, message_obj);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = 0;
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        return RF_OK;
    }

    monitor_fn_data->task_progress = file_trans_progress;

    
    return_val_do_info_if_fail(RF_FINISH_PERCENTAGE == file_trans_progress, RF_OK,
        (monitor_fn_data->task_state = RF_TASK_RUNNING));

    
    if (FALSE == get_element_str(body_jso, RFPROP_LICSVC_PARAM_FILE_SOURCE, &source_para)) {
        
        debug_log(DLOG_DEBUG, "can't get install parameter %s, set for default", RFPROP_LICSVC_PARAM_FILE_SOURCE);
        source_para = LICENSE_FILE_SOURCE_BMC;
    }
    do_info_if_true(!strcmp(LICENSE_FILE_SOURCE_BMC, source_para), (source = LICENSE_FILE_SOURCE_BMC_INDEX));
    do_info_if_true(!strcmp(LICENSE_FILE_SOURCE_FD, source_para), (source = LICENSE_FILE_SOURCE_FD_INDEX));
    do_info_if_true(!strcmp(LICENSE_FILE_SOURCE_ESIGHT, source_para), (source = LICENSE_FILE_SOURCE_ESIGHT_INDEX));

    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &content);
    filename = g_strrstr(content, SLASH_FLAG_STR);
    goto_label_do_info_if_fail(NULL != filename, err_exit, debug_log(DLOG_ERROR, "get filename from url_path failed"));
    ret = snprintf_s(file_path, MAX_URL_LEN, MAX_URL_LEN - 1, "/tmp%s", filename);
    goto_label_do_info_if_fail(ret > 0, err_exit,
        debug_log(DLOG_ERROR, "format license file path failed, ret = %d", ret));

    exec_install_license(monitor_fn_data, message_obj, file_path, source);
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    return RF_OK;

err_exit:
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_OK;
}

LOCAL gint32 get_tmp_file_name(gchar* file_path, size_t path_len)
{
    // 循环5次，生成临时文件名
    for (gint32 i = 0; i < 5; i++) {
        gulong cur_tick = vos_tick_get();
        gint32 ret = snprintf_s(file_path, path_len, path_len - 1, "/tmp/license_%lu.xml", cur_tick);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: format fail %d", __FUNCTION__, ret);
            return RET_ERR;
        }

        // 如果临时文件不存在，则使用当前文件名，作为临时文件
        if (vos_get_file_accessible(file_path) != TRUE) {
            return RET_OK;
        }
        vos_task_delay(10); // delay 10 ms, 保证 tick 变化
    }

    debug_log(DLOG_ERROR, "%s: get_tmp_file_name fail", __FUNCTION__);
    return RET_ERR;
}


LOCAL gint32 act_install_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *type = NULL;
    const gchar *content = NULL;
    GSList *input_list = NULL;
    GSList *download_input_list = NULL;
    gchar download_url_buf[MAX_URL_LEN] = {0};
    gchar file_path[MAX_URL_LEN] = {0};
    const gchar *source_para = NULL;
    guint8 source = 0;
    gboolean b_ret;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));
    
    return_val_do_info_if_expr(0 == (USERROLE_USERMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (FALSE == get_element_str(i_paras->val_jso, RFPROP_LICSVC_PARAM_FILE_SOURCE, &source_para)) {
        
        debug_log(DLOG_DEBUG, "can't get install parameter %s, set for default", RFPROP_LICSVC_PARAM_FILE_SOURCE);
        source_para = LICENSE_FILE_SOURCE_BMC;
    }

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_TYPE, &type);
    if (type == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFACTION_PARAM_TYPE);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, &content);
    if (content == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFACTION_PARAM_CONTENT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    do_info_if_true(!strcmp(LICENSE_FILE_SOURCE_BMC, source_para), (source = LICENSE_FILE_SOURCE_BMC_INDEX));
    do_info_if_true(!strcmp(LICENSE_FILE_SOURCE_FD, source_para), (source = LICENSE_FILE_SOURCE_FD_INDEX));
    do_info_if_true(!strcmp(LICENSE_FILE_SOURCE_ESIGHT, source_para), (source = LICENSE_FILE_SOURCE_ESIGHT_INDEX));
    // 查询是否已经有安装的任务
    b_ret = check_redfish_running_task_exist(license_install_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "install license task already exist");
        (void)create_message_info(MSGID_LICENSE_INSTALLING, NULL, o_message_jso));

    if (0 == strcmp(RFACTION_PARAM_VALUE_TEXT, type)) {
        // license内容超过最大长度
        if (strlen(content) > LICENSE_MAX_LEN) {
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        if (get_tmp_file_name(file_path, sizeof(file_path)) != RET_OK) {
            goto err_exit;
        }

        // 将license内容保存到临时文件/tmp/license_*.xml
        ret = g_file_set_contents(file_path, content, -1, NULL);
        goto_label_do_info_if_fail(ret, err_exit,
            debug_log(DLOG_ERROR, "%s: save license to tmp path failed, ret = %d!", __FUNCTION__, ret));
        (void)chmod(file_path, S_IRUSR);
    } else {
        // URL格式校验
        b_ret = g_regex_match_simple(LICENSE_FILE_REGEX, content, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_ILLEGAL_URI_PATH, NULL, o_message_jso));

        // 判断输入为本地路径
        if (content[0] == '/') {
            // 检查绝对路径是否在/tmp目录下
            ret = dal_check_real_path(content);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
                (void)create_message_info(MSGID_ILLEGAL_URI_PATH, NULL, o_message_jso));

            if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, content,
                (const gchar *)i_paras->user_roleid)) {
                (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }
            errno_t safe_fun_ret = strncpy_s(file_path, MAX_URL_LEN, content, MAX_URL_LEN - 1);
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        } else {
            // 下载远程文件到本地/tmp目录下
            ret =
                snprintf_s(download_url_buf, MAX_URL_LEN, MAX_URL_LEN - 1, "download;%d;%s", FILE_ID_LICENSE, content);
            goto_label_do_info_if_fail(ret > 0, err_exit,
                debug_log(DLOG_ERROR, "format license download url failed. ret = %d", ret));

            download_input_list =
                g_slist_append(download_input_list, g_variant_new_string((const gchar *)download_url_buf));
            download_input_list = g_slist_append(download_input_list, g_variant_new_string(" "));

            memset_s(download_url_buf, sizeof(download_url_buf), 0, sizeof(download_url_buf));
            ret =
                uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                (i_paras->auth_type == LOG_TYPE_LOCAL), (const gchar *)i_paras->user_roleid, OBJ_HANDLE_COMMON,
                CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, 0, download_input_list, NULL);
            uip_free_gvariant_list(download_input_list);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
                debug_log(DLOG_ERROR, "save file to local failed, ret = %d.", ret);
                (void)parse_file_transfer_err_code(ret, o_message_jso));

            // 远程下载，需做成任务
            TASK_MONITOR_INFO_S *monitor_fn_data = task_monitor_info_new((GDestroyNotify)json_object_put);
            goto_label_do_info_if_fail(NULL != monitor_fn_data, err_exit,
                debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed.", __FUNCTION__));

            monitor_fn_data->user_data = json_object_new_object();
            goto_label_do_info_if_fail(NULL != monitor_fn_data->user_data, err_exit,
                debug_log(DLOG_ERROR, "dup file path failed");
                task_monitor_info_free(monitor_fn_data));

            json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME,
                json_object_new_string(i_paras->user_name));
            json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP,
                json_object_new_string(i_paras->client));
            
            json_object_object_add((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG,
                json_object_new_int((int32_t)i_paras->is_from_webui));
            

            monitor_fn_data->task_progress = TASK_NO_PROGRESS;
            monitor_fn_data->rsc_privilege = USERROLE_USERMGNT;

            ret = create_new_task("license install task", license_install_status_monitor, monitor_fn_data,
                i_paras->val_jso, i_paras->uri, o_result_jso);
            goto_label_do_info_if_fail(VOS_OK == ret, err_exit, task_monitor_info_free(monitor_fn_data);
                debug_log(DLOG_ERROR, "%s: create new task failed.", __FUNCTION__));

            return HTTP_ACCEPTED;
        }
    }

    // 调用方法开始安装license
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_path));
    input_list = g_slist_append(input_list, g_variant_new_byte(source));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        CLASS_NAME_LICENSE_MANAGE, METHOD_LM_INSTALL_LIC, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    ret = set_install_err_code(ret, o_message_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: call class method %s failed!", __FUNCTION__, METHOD_LM_INSTALL_LIC));

    return HTTP_OK;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 act_install_license_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso)
{
    gint32 ret = act_install_license(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL && i_paras->val_jso != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 license_export_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 trans_status = 0;
    GSList *caller_info = NULL;
    const gchar *user_name = NULL;
    const gchar *user_ip = NULL;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    return_val_do_info_if_expr((NULL == origin_obj_path) || (NULL == body_jso) || (NULL == monitor_fn_data) ||
        (NULL == message_obj),
        RF_OK, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP, &user_ip);
    (void)get_element_int((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name((guchar)from_webui_flag)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_ip));

    ret = get_file_transfer_progress(&trans_status);
    goto_label_do_info_if_fail(VOS_OK == ret, err_quit,
        debug_log(DLOG_ERROR, "get file transfer progress failed, ret is %d", ret));

    if (trans_status < 0) {
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        
        (void)parse_file_transfer_err_code(trans_status, message_obj);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export license failed");
        uip_free_gvariant_list(caller_info);
        
        return RF_OK;
    } else {
        monitor_fn_data->task_progress = trans_status;
        monitor_fn_data->task_state = (RF_FINISH_PERCENTAGE == trans_status) ? RF_TASK_COMPLETED : RF_TASK_RUNNING;
        do_val_if_expr(RF_FINISH_PERCENTAGE == trans_status,
            proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export license successfully"));
        uip_free_gvariant_list(caller_info);

        return RF_OK;
    }

err_quit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Export license failed");
    uip_free_gvariant_list(caller_info);

    
    return RF_OK;
}


LOCAL gint32 set_export_err_code(gint32 result_code, json_object **o_message_jso)
{
    gint32 ret = 0;

    debug_log(DLOG_DEBUG, "result_code = %d", result_code);

    switch (result_code) {
        case LM_OK:
            ret = HTTP_OK;
            break;
        case LM_ERR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_EXPORT_LICENSE_FAILED, NULL, o_message_jso);
            break;
        case LM_ERR_NOT_INSTALLED:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_LICENSE_NOT_INSTALL, NULL, o_message_jso);
            break;
        case LM_ERR_PARAMETER:
        case LM_ERR_BUSY:
        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }
    return ret;
}


LOCAL gint32 get_export_license_url(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar **url_path)
{
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((USERROLE_USERMGNT & i_paras->user_role_privilege) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_CONTENT, url_path);
    if (*url_path == NULL) {
        debug_log(DLOG_ERROR, "%s input %s param error.", __FUNCTION__, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (strstr(*url_path, "/../") != NULL) {
        debug_log(DLOG_ERROR, "%s input %s param error.", __FUNCTION__, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_EXPORT_LICENSE_FAILED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}

LOCAL gint32 act_export_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *url_path = NULL;
    gboolean b_ret;

    
    ret = get_export_license_url(i_paras, o_message_jso, o_result_jso, &url_path);
    if (ret != RET_OK) {
        return ret;
    }

    // 任务重复性检查
    b_ret = check_redfish_running_task_exist(license_export_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(b_ret == FALSE, HTTP_BAD_REQUEST, debug_log(DLOG_ERROR, "export task already exist");
        (void)create_message_info(MSGID_LICENSE_EXPORTING, NULL, o_message_jso));

    // URL格式校验
    b_ret = g_regex_match_simple(LICENSE_FILE_REGEX, url_path, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
    return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ILLEGAL_URI_PATH, NULL, o_message_jso));

    // 判断URI是否为目录名
    if (url_path[0] == '/' && g_file_test(url_path, G_FILE_TEST_IS_DIR) == TRUE) {
        (void)create_message_info(MSGID_NOT_ALLOW_OVERWRITE_DIR, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(url_path));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        CLASS_NAME_LICENSE_MANAGE, METHOD_LM_EXPORT_LIC, AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (('/' == url_path[0]) && (VOS_OK == ret)) {
        ret = dal_change_file_owner(i_paras->auth_type, url_path, i_paras->user_name, S_IRUSR | S_IWUSR);
        if (ret == VOS_OK) {
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;
        } else {
            (void)proxy_delete_file(0, NULL, NULL, url_path);
            goto err_exit;
        }
    }

    ret = set_export_err_code(ret, o_message_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret, debug_log(DLOG_ERROR, "export license failed, ret = %d", ret));

    TASK_MONITOR_INFO_S *monitor_fn_data = task_monitor_info_new((GDestroyNotify)json_object_put);
    goto_label_do_info_if_fail(NULL != monitor_fn_data, err_exit,
        debug_log(DLOG_ERROR, "%s: create new task failed.", __FUNCTION__));

    monitor_fn_data->user_data = json_object_new_object();
    goto_label_do_info_if_fail(NULL != monitor_fn_data->user_data, err_exit,
        debug_log(DLOG_ERROR, "dup file path failed"); task_monitor_info_free(monitor_fn_data));

    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_NAME,
        json_object_new_string(i_paras->user_name));
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_LOG_USER_IP,
        json_object_new_string(i_paras->client));
    
    json_object_object_add((json_object *)monitor_fn_data->user_data, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int((int32_t)i_paras->is_from_webui));
    

    monitor_fn_data->task_progress = TASK_NO_PROGRESS;
    monitor_fn_data->rsc_privilege = USERROLE_USERMGNT;

    ret = create_new_task("license export task", license_export_status_monitor, monitor_fn_data, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, task_monitor_info_free(monitor_fn_data);
        debug_log(DLOG_ERROR, "%s: create new task failed.", __FUNCTION__));

    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    return HTTP_ACCEPTED;

err_exit:
    json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_revoke_err_code(gint32 result_code, json_object **o_message_jso)
{
    gint32 retVal = 0;

    debug_log(DLOG_DEBUG, "result_code = %d", result_code);

    switch (result_code) {
        case LM_OK:
            retVal = HTTP_OK;
            break;

        case LM_ERR:
            retVal = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_REVOKE_LICENSE_FAILED, NULL, o_message_jso);
            break;

        case LM_ERR_NOT_INSTALLED:
            retVal = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_LICENSE_NOT_INSTALL, NULL, o_message_jso);
            break;

        case LM_ERR_BUSY:
        default:
            retVal = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }

    return retVal;
}


LOCAL gint32 act_revoke_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    gint32 ret;
    GSList *output_list = NULL;
    gchar revoke_code[LM_SHORT_STREAM_LEN] = {0};
    const gchar *revoke_code_tmp = NULL;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));
    
    return_val_do_info_if_expr(0 == (USERROLE_USERMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        CLASS_NAME_LICENSE_MANAGE, METHOD_LM_REVOKE_LIC, AUTH_DISABLE, 0, NULL, &output_list);
    ret = set_revoke_err_code(ret, o_message_jso);
    return_val_do_info_if_fail(HTTP_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: call class method %s failed!", __FUNCTION__, METHOD_LM_REVOKE_LIC));

    revoke_code_tmp = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    goto_label_do_info_if_fail(NULL != revoke_code_tmp, err_exit,
        debug_log(DLOG_ERROR, "%s: get output string failed.", __FUNCTION__);
        uip_free_gvariant_list(output_list));
    safe_fun_ret = strncpy_s(revoke_code, LM_SHORT_STREAM_LEN, revoke_code_tmp, LM_SHORT_STREAM_LEN - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    uip_free_gvariant_list(output_list);
    (void)create_message_info(MSGID_REVOKE_LICENSE_SUCCESS, NULL, o_message_jso, (const gchar *)revoke_code);
    return HTTP_OK;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_delete_err_code(gint32 result_code, json_object **o_message_jso)
{
    gint32 ret = 0;

    debug_log(DLOG_DEBUG, "result_code = %d", result_code);

    switch (result_code) {
        case LM_OK:
            ret = HTTP_OK;
            break;
        case LM_ERR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_DELETE_LICENSE_FAILED, NULL, o_message_jso);
            break;
        case LM_ERR_NOT_INSTALLED:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_LICENSE_NOT_INSTALL, NULL, o_message_jso);
            break;
        case LM_ERR_BUSY:
        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }
    return ret;
}


LOCAL gint32 act_delete_license(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));
    
    return_val_do_info_if_expr(0 == (USERROLE_USERMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        CLASS_NAME_LICENSE_MANAGE, METHOD_LM_DELETE_LIC, AUTH_DISABLE, 0, NULL, NULL);
    ret = set_delete_err_code(ret, o_message_jso);
    return ret;
}


LOCAL gint32 get_manager_license_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr((NULL == o_result_jso) || (NULL == o_message_jso) ||
        (VOS_OK != provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: new action object fail", __FUNCTION__));

    
    ret = redfish_get_board_slot(slot, PROP_VAL_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get slot fail", __FUNCTION__));

    // 生成安装license的action的对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_LICENSE_SERVICE_INSTALL, (const gchar *)slot,
        RFACTION_LICENSE_SERVICE_INSTALL, RFACTION_INFO_LICENSE_SERVICE_INSTALL);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add root cert object fail", __FUNCTION__));

    // 生成导出license的action的对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_LICENSE_SERVICE_EXPORT, (const gchar *)slot,
        RFACTION_LICENSE_SERVICE_EXPORT, RFACTION_INFO_LICENSE_SERVICE_EXPORT);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add client cert object fail", __FUNCTION__));

    // 生成失效license的action对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_LICENSE_SERVICE_REVOKE, (const gchar *)slot,
        RFACTION_LICENSE_SERVICE_REVOKE, RFACTION_INFO_LICENSE_SERVICE_REVOKE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add test object fail", __FUNCTION__));

    // 生成删除license的action对象
    ret = add_all_action_prop(*o_result_jso, RFPROP_LICENSE_SERVICE_DELETE, (const gchar *)slot,
        RFACTION_LICENSE_SERVICE_DELETE, RFACTION_INFO_LICENSE_SERVICE_DELETE);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: add test object fail", __FUNCTION__));

    return HTTP_OK;
}


gint32 manager_licsrv_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "this board doesn't support license."));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (TRUE == b_ret) {
        *prop_provider = g_manager_licsrv_provider;
        *count = sizeof(g_manager_licsrv_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}


LOCAL gint32 get_licsrv_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar               uri[MAX_URI_LENGTH] = {0};
    gchar               action_info_uri[MAX_URI_LENGTH] = {0};
    gchar               slot_id[MAX_RSC_ID_LEN] = {0};
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)redfish_get_board_slot(slot_id, MAX_RSC_ID_LEN);

    ret = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_LICENSE_SERVICE_URI, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    ret = rf_update_rsc_actioninfo_uri(g_manager_licsrv_action_array, G_N_ELEMENTS(g_manager_licsrv_action_array),
        i_paras->uri, uri, action_info_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "update action info uri (%s) failed", i_paras->uri));

    *o_result_jso = json_object_new_string((const gchar *)action_info_uri);

    return HTTP_OK;
}


gint32 licsrv_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "this board doesn't support license."));

    b_ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (TRUE == b_ret) {
        *prop_provider = g_manager_licsrv_actioninfo_provider;
        *count = sizeof(g_manager_licsrv_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}
