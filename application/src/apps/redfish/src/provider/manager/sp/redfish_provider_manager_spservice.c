
#include "redfish_provider.h"
#include <dirent.h>

#define MAX_SCHAME_FILEPATH_LENGTH 266


extern guint32 g_sp_timeout;
extern guint8 g_sp_finished;

typedef struct {
    gchar ps_v[MAX_CHARACTER_NUM];
    gchar os_v[MAX_CHARACTER_NUM];
    gchar dr_v[MAX_CHARACTER_NUM];
    gchar euler_v[MAX_CHARACTER_NUM];
    gchar major_v[MAX_CHARACTER_NUM];
    gchar component_messages[MAX_CHARACTER_NUM];
}SP_VERSION_INFO;

LOCAL gint32 _get_spservice_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_spservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 _get_spservice_raids(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_spservice_raids_current_cfgs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_spservice_oss(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_spservice_fwupdates(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_spservice_result(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_spservice_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_spservice_device(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_spservice_diagnose(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_spservice_drive_erase(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_config_list(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_start_flag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_time_delay(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_spservice_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_finished(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_device_info_collect_enabled(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 _get_spservice_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 _set_start_flag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_restart_delay(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _act_spservice_delfile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_finished(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_spraid_exportconfiguration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_device_info_collect_enabled(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);

LOCAL gint32 act_spservice_update_schema(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 verify_file_checksum(const gchar *src_filepath, const gchar *sha256_checksum);
LOCAL gint32 get_update_schema_info(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, 
                                    UPDATE_SP_SCHEMA_NEED_INFO* update_schema_info);

LOCAL gint32 get_write_ums_file_result(gint32 ret, json_object **o_message_jso);
LOCAL gint32 get_delete_usm_file_result(gint32 ret, json_object **o_message_jso);
LOCAL gint32 get_spservice_signature(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S  g_manager_spservice_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_SPSERVICE_STARTFLAG, SP_UMS_CLASS_NAME, UMS_PROPERTY_SP_CFG_READY, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_start_flag, _set_start_flag, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_TIMEDELY, SP_UMS_CLASS_NAME, UMS_PROPERTY_SYS_RESTART_DELAY, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_time_delay, _set_restart_delay, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_UP_VERSION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_version, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_TIMEOUT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_timeout, _set_timeout, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_FINISHED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_finished, _set_finished, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_DEVICE_INFO_COLLECT_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, _get_device_info_collect_enabled,
        _set_device_info_collect_enabled, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_SPSERVICE_RAID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_raids, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_RAID_CURRENT_CFG, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_raids_current_cfgs, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_OS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_oss, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_FWUPDATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_fwupdates, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_SPRESULT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_result, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_SPCFG, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_cfg, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_SPSERVICE_DEVICEINFO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_device, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_DIAGNOSE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_diagnose, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_DRIVE_ERASE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_drive_erase, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_action, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_SPSERVICE_FILE_LIST, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_config_list, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SPSERVICE_SIGNATURE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_spservice_signature, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_MANAGER_SP_POST_ACTION_NAME, CLASS_SMTP_CONFIG, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, _act_spservice_delfile, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SPRAID_EXPORT_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_spraid_exportconfiguration, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_SP_COPY_SCHEMA_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_spservice_update_schema, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_spservice_signature(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object* jso_result_file = json_object_new_object();
    if (jso_result_file == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_CFG, RFPROP_MANAGER_SP_SUPPORT_NAME, jso_result_file);
    if (ret != RET_OK) {
        json_object_put(jso_result_file);
        debug_log(DLOG_ERROR, "%s: get_sp_file_data fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object* jso_update = NULL;
    json_bool ret_bool = json_object_object_get_ex(jso_result_file, "SPFWUpdate", &jso_update);
    if (ret_bool != TRUE || jso_update == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex SPFWUpdate fail", __FUNCTION__);
        goto exit;
    }

    json_object* jso_signature = NULL;
    ret_bool = json_object_object_get_ex(jso_update, "Signature", &jso_signature);
    if (ret_bool != TRUE || jso_update == NULL) {
        debug_log(DLOG_DEBUG, "%s: json_object_object_get_ex Signature fail", __FUNCTION__);
        goto exit;
    }

    *o_result_jso = json_object_get(jso_signature);
    json_object_put(jso_result_file);
    return HTTP_OK;

exit:
    json_object_put(jso_result_file);
    *o_result_jso = json_tokener_parse("{\"PGP\":{\"Suffix\": \"asc\"}}");
    return HTTP_OK;
}


LOCAL gint32 _get_spservice_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGERS_SPSERVICE_METADATA, slot);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_SPSERVICE_URI, slot);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_raids(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_RAID, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_raids_current_cfgs(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_RAID_CURRNT_CFG, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_oss(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 spservice_os_enable = TRUE;

    
    return_val_do_info_if_expr(NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SP_SERVICE_OS_ENABLE,
        &spservice_os_enable);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: Get SP service for OS enable state failed.", __FUNCTION__, __LINE__));

    return_val_do_info_if_expr(FALSE == spservice_os_enable, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: The resouce is not enabled by license.", __FUNCTION__, __LINE__));
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_OS, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_fwupdates(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_FWUPDATE, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_result(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_SPRESULT, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_SPCFG, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_device(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_DEVICEINFO, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_diagnose(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_DIAGNOSE, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_drive_erase(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RF_UIR_NEXT_SPSERVICE_DRIVE_ERASE, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gchar *_rf_sp_pathid_to_type(guchar pathid)
{
    if (UMS_TRANS_FILE_ID_SP_FWUPDATE == pathid) {
        return SP_FILE_TYPE_FWUPDATE;
    } else if (UMS_TRANS_FILE_ID_SP_RAID == pathid) {
        return SP_FILE_TYPE_RAID;
    } else if (UMS_TRANS_FILE_ID_SP_CFG == pathid) {
        return SP_FILE_TYPE_SPCFG;
    } else if (UMS_TRANS_FILE_ID_SP_OSINSTALL == pathid) {
        return SP_FILE_TYPE_SPOSINSTALL;
    }
    return NULL;
}


LOCAL gint32 rf_sp_type_to_pathid(const gchar *type)
{
    if (0 == g_strcmp0(SP_FILE_TYPE_FWUPDATE, type)) {
        return UMS_TRANS_FILE_ID_SP_FWUPDATE;
    } else if (0 == g_strcmp0(SP_FILE_TYPE_RAID, type)) {
        return UMS_TRANS_FILE_ID_SP_RAID;
    } else if (0 == g_strcmp0(SP_FILE_TYPE_SPDRIVEERASE, type)) {
        return UMS_TRANS_FILE_ID_SP_DRIVE_ERASE;
    }

    return -1;
}


LOCAL gint32 _get_sp_filename_by_path(PROVIDER_PARAS_S *i_paras, guchar pathid, json_object *json_obj)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *file_data = NULL;
    gchar **split_array = NULL;
    gchar *filetype = NULL;
    gint32 length = 0;
    gint32 index_id = 0;
    json_object *tmp_json = NULL;

    filetype = _rf_sp_pathid_to_type(pathid);
    return_val_do_info_if_expr(NULL == filetype, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: type error.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(pathid));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_GET_FILE_LIST, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);

    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        debug_log(DLOG_DEBUG, "%s, %d:  get file list fail, ret is %d.", __FUNCTION__, __LINE__, ret));
    

    uip_free_gvariant_list(input_list);
    file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_expr(NULL == file_data, HTTP_INTERNAL_SERVER_ERROR, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    split_array = g_strsplit(file_data, SP_FILE_SPLITE_FLAG, 0);
    if (NULL != split_array) {
        length = g_strv_length(split_array);

        for (index_id = 0; index_id < length; index_id++) {
            continue_if_expr(strlen(split_array[index_id]) <= strlen(".json"));
            tmp_json = json_object_new_object();

            json_object_object_add(tmp_json, SP_FILE_TYPE, json_object_new_string(filetype));
            json_object_object_add(tmp_json, SP_FILE_NAME, json_object_new_string(split_array[index_id]));

            json_object_array_add(json_obj, tmp_json);
        }

        g_strfreev(split_array);
    }

    uip_free_gvariant_list(output_list);

    return VOS_OK;
}


gint32 get_sp_file_uri_members(PROVIDER_PARAS_S *i_paras, gchar *format_uri, guchar pathid, json_object *json_obj)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *file_data = NULL;
    gchar **split_array = NULL;
    gint32 length = 0;
    gint32 index_id = 0;
    gint32 count = 0;
    json_object *tmp_json = NULL;
    json_object *array_json = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar *valid_url = NULL;

    if (NULL == i_paras || NULL == format_uri || NULL == json_obj) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    // NetDev,RAID 获取集合信息
    
    if (pathid == UMS_TRANS_FILE_ID_SP_NETDEV || pathid == UMS_TRANS_FILE_ID_SP_RAID ||
        pathid == UMS_TRANS_FILE_ID_SP_RAID_EXPORT) {
        

        ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
        return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

        input_list = g_slist_append(input_list, g_variant_new_byte(pathid));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            SP_UMS_CLASS_NAME, UMS_METHOD_GET_FILE_LIST, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
            &output_list);

        
        return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR, uip_free_gvariant_list(input_list);
            uip_free_gvariant_list(output_list);
            debug_log(DLOG_DEBUG, "%s, %d:  get file list fail, ret is %d.", __FUNCTION__, __LINE__, ret));
        
        uip_free_gvariant_list(input_list);

        file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
        return_val_do_info_if_expr(NULL == file_data, VOS_ERR, uip_free_gvariant_list(output_list);
            debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

        split_array = g_strsplit(file_data, SP_FILE_SPLITE_FLAG, 0);
        if (NULL != split_array) {
            length = g_strv_length(split_array);

            array_json = json_object_new_array();
            if (NULL != array_json) {
                json_object_object_add(json_obj, RFPROP_MEMBERS, array_json);

                for (index_id = 0; index_id < length; index_id++) {
                    
                    debug_log(DLOG_DEBUG, "%s:split_array is %s len %zu", __FUNCTION__, split_array[index_id],
                        strlen(split_array[index_id]));
                    
                    continue_if_expr(strlen(split_array[index_id]) <= strlen(".json"));

                    // 拼接URI
                    (void)memset_s(uri, sizeof(uri), 0, sizeof(uri));
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
                    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, format_uri, slot);
#pragma GCC diagnostic pop
                    do_val_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s:snprintf_s fail", __FUNCTION__));
                    safe_fun_ret = strncat_s(uri, sizeof(uri), "/", strlen("/"));
                    do_if_expr(safe_fun_ret != EOK,
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                    safe_fun_ret = strncat_s(uri, sizeof(uri), split_array[index_id],
                        strlen(split_array[index_id]) - strlen(".json"));
                    do_if_expr(safe_fun_ret != EOK,
                        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

                    
                    if (UMS_TRANS_FILE_ID_SP_RAID_EXPORT == pathid) {
                        
                        valid_url = strstr(uri, "_");
                        do_if_expr(valid_url, (*valid_url = '\0'));
                    }
                    

                    // 增加{"@odata.id":"URI"}对象
                    tmp_json = json_object_new_object();
                    continue_do_info_if_expr(NULL == tmp_json,
                        debug_log(DLOG_ERROR, "%s:tmp_json is null", __FUNCTION__));
                    json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
                    (void)json_object_array_add(array_json, tmp_json);
                    count++;
                    tmp_json = NULL;
                }

                json_object_object_add(json_obj, RFPROP_MEMBERS_COUNT, json_object_new_int(count));
            }
            
            g_strfreev(split_array);
            
        }

        uip_free_gvariant_list(output_list);
    }
    // Result,update,spcfg,osinstall 获取集合信息
    else {
        // 添加"Members@odata.count"
        json_object_object_add(json_obj, RFPROP_MEMBERS_COUNT, json_object_new_int(1));

        // 添加"Members"
        array_json = json_object_new_array();
        return_val_do_info_if_expr(NULL == array_json, VOS_ERR,
            debug_log(DLOG_ERROR, "%s:array_json is null", __FUNCTION__));
        json_object_object_add(json_obj, RFPROP_MEMBERS, array_json);

        tmp_json = json_object_new_object();
        return_val_do_info_if_expr(NULL == array_json, VOS_ERR,
            debug_log(DLOG_ERROR, "%s:tmp_json is null", __FUNCTION__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, format_uri, slot);
#pragma GCC diagnostic pop
        do_val_if_expr(0 >= ret, debug_log(DLOG_ERROR, "%s:snprintf_s fail", __FUNCTION__));
        safe_fun_ret = strncat_s(uri, sizeof(uri), "/1", strlen("/1"));
        do_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

        json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
        (void)json_object_array_add(array_json, tmp_json);
    }

    return VOS_OK;
}


gint32 get_sp_file_data(PROVIDER_PARAS_S *i_paras, guchar pathid, gchar *filename, json_object *json_obj)
{
    int iRet;
    gint32 ret;
    gsize element_num = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *file_data = NULL;
    json_object *file_oject = NULL;
    gchar jsonfilename[MAX_CHARACTER_NUM] = {0};
    gchar *filename_lower = NULL;
    SP_SENSITIVE_PROP_S sensitive_list[] = {
        {UMS_TRANS_FILE_ID_SP_OSINSTALL,   SP_PROP_NAME_CDKEY },
        {UMS_TRANS_FILE_ID_SP_OSINSTALL,   SP_PROP_NAME_ROOT_PWD }
    };
    guint32 list_len = G_N_ELEMENTS(sensitive_list);
    guint32 i;
    gboolean is_sensitive_prop;
    gchar *file_char = NULL;

    if (NULL == i_paras || NULL == filename || NULL == json_obj) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    filename_lower = g_ascii_strdown(filename, strlen(filename));
    return_val_do_info_if_expr(NULL == filename_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d:  filename_lower  fail", __FUNCTION__, __LINE__));

    
    iRet = snprintf_s(jsonfilename, sizeof(jsonfilename), sizeof(jsonfilename) - 1, "%s.json", filename_lower);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    g_free(filename_lower);

    input_list = g_slist_append(input_list, g_variant_new_byte(pathid));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)jsonfilename));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_READ_FILE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);

    return_val_do_info_if_expr(VOS_OK != ret, ret, uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        debug_log(DLOG_DEBUG, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    uip_free_gvariant_list(input_list);

    file_data = (const gchar *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0), &element_num,
        sizeof(gchar));
    return_val_do_info_if_expr(NULL == file_data, VOS_ERR, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    
    file_char = (gchar *)g_malloc0(element_num + 1);
    if (NULL == file_char) {
        debug_log(DLOG_ERROR, "%s, %d: g_malloc0 file_char fail", __FUNCTION__, __LINE__);
        uip_free_gvariant_list(output_list);
        return VOS_ERR;
    }
    if (EOK != memmove_s(file_char, element_num + 1, file_data, element_num)) {
        debug_log(DLOG_ERROR, "%s, %d: memmove_s fail", __FUNCTION__, __LINE__);
        uip_free_gvariant_list(output_list);
        g_free(file_char);
        return VOS_ERR;
    }
    
    file_oject = json_tokener_parse((const char *)file_char);
    g_free(file_char);
    uip_free_gvariant_list(output_list);
    
    return_val_do_info_if_expr(NULL == file_oject, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d:  file_oject is null", __FUNCTION__, __LINE__));
    

    // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
    json_object_object_foreach0(file_oject, name, val)
    {
        continue_if_expr(0 == g_strcmp0(name, RFOBJ_ID));

        
        is_sensitive_prop = FALSE;
        for (i = 0; i < list_len; i++) {
            break_do_info_if_expr(sensitive_list[i].file_id == pathid &&
                0 == g_strcmp0(sensitive_list[i].prop_name, name),
                (is_sensitive_prop = TRUE));
        }

        if (TRUE == is_sensitive_prop) {
            json_object_clear_string(file_oject, name);
            json_object_object_add(json_obj, name, NULL);
        } else {
            json_object_object_add(json_obj, name, json_object_get(val));
        }
        
    }
#pragma GCC diagnostic pop // require GCC 4.6

    json_object_put(file_oject);

    return VOS_OK;
}


LOCAL gint32 _get_config_list(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  *o_result_jso is null", __FUNCTION__, __LINE__));

    (void)_get_sp_filename_by_path(i_paras, UMS_TRANS_FILE_ID_SP_OSINSTALL, *o_result_jso);
    (void)_get_sp_filename_by_path(i_paras, UMS_TRANS_FILE_ID_SP_RAID, *o_result_jso);
    (void)_get_sp_filename_by_path(i_paras, UMS_TRANS_FILE_ID_SP_FWUPDATE, *o_result_jso);
    (void)_get_sp_filename_by_path(i_paras, UMS_TRANS_FILE_ID_SP_CFG, *o_result_jso);

    return HTTP_OK;
}


LOCAL gint32 _get_start_flag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar b_value = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_spservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(obj_handle, g_manager_spservice_provider[i_paras->index].pme_prop_name, &b_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_boolean(b_value);

    return HTTP_OK;
}


LOCAL gint32 _get_time_delay(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint32 u_value = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_spservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret =
        dal_get_property_value_uint32(obj_handle, g_manager_spservice_provider[i_paras->index].pme_prop_name, &u_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_int(u_value);

    return HTTP_OK;
}


LOCAL gint32 __get_sp_version_info(SP_VERSION_INFO *ver_info)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_string(obj_handle, UMS_PROPERTY_SP_VERSION_APP, ver_info->ps_v, MAX_CHARACTER_NUM);
    
    (void)dal_get_property_value_string(obj_handle, UMS_PROPERTY_SP_VERSION_DRV, ver_info->dr_v, MAX_CHARACTER_NUM);
    (void)dal_get_property_value_string(obj_handle, UMS_PROPERTY_SP_REVISON_OS, ver_info->os_v, MAX_CHARACTER_NUM);
    (void)dal_get_property_value_string(obj_handle, UMS_PROPERTY_SP_VERSION_OS, ver_info->euler_v, MAX_CHARACTER_NUM);
    (void)dal_get_property_value_string(obj_handle, UMS_PROPERTY_SP_MAJORVERSION, ver_info->major_v, MAX_CHARACTER_NUM);
    
    (void)dal_get_property_value_string(obj_handle, UMS_PROPERTY_SP_VERSION_COMPONENT,\
        ver_info->component_messages, MAX_CHARACTER_NUM);

    return RET_OK;
}


LOCAL void __fill_component_messages(gchar *component_messages, json_object** o_result_jso)
{
    gchar component_name[MAX_CHARACTER_NUM] = {0};
    gchar component_v[MAX_CHARACTER_NUM] = {0};
    gchar *pTmpH = NULL;
    gchar *pTmpE = NULL;
    gchar *pTmpMesgs = component_messages;

    pTmpMesgs[MAX_CHARACTER_NUM - 1] = '\0'; // 将最后一位置为0，防止越界
    while (strlen(pTmpMesgs)) {
        // 解析出组件名字
        pTmpH = strchr(pTmpMesgs, '\"');
        if (pTmpH == NULL) {
            break;
        }
        pTmpH++;
        pTmpMesgs = pTmpH;
        pTmpE = strchr(pTmpMesgs, '\"');
        if (!pTmpE) {
            break;
        }
        pTmpMesgs = &pTmpE[1];
        *pTmpE = '\0';
        strncpy_s(component_name, MAX_CHARACTER_NUM, pTmpH, MAX_CHARACTER_NUM - 1);
        // 解析出组件版本号
        pTmpH = strchr(pTmpMesgs, '\"');
        if (pTmpH == NULL) {
            break;
        }
        pTmpH++;
        pTmpMesgs = pTmpH;
        pTmpE = strchr(pTmpMesgs, '\"');
        if (!pTmpE) {
            break;
        }
        pTmpMesgs = pTmpE + 1;
        *pTmpE = '\0';
        strncpy_s(component_v, MAX_CHARACTER_NUM, pTmpH, MAX_CHARACTER_NUM - 1);
        
        json_object_object_add(*o_result_jso, (const gchar*)component_name, 
            json_object_new_string((const char *)component_v));
    }

    return;
}


LOCAL gint32 _get_spservice_version(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    SP_VERSION_INFO ver_info = {0};
    
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = __get_sp_version_info(&ver_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get sp version info fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: file_data is null", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    json_object_object_add(*o_result_jso, RFPROP_SPSERVICE_UP_VERSION_APP,
        (check_string_val_effective(ver_info.ps_v) == RET_OK) ? json_object_new_string(ver_info.ps_v) : NULL);
    if (!strlen(ver_info.euler_v)) {
        json_object_object_add(*o_result_jso, RFPROP_SPSERVICE_UP_VERSION_OS,
            (check_string_val_effective(ver_info.os_v) == RET_OK) ? json_object_new_string(ver_info.os_v) : NULL);
    } else {
        json_object_object_add(*o_result_jso, RFPROP_SPSERVICE_UP_REVISON_OS,
            (check_string_val_effective(ver_info.os_v) == RET_OK) ? json_object_new_string(ver_info.os_v) : NULL);
    }
    json_object_object_add(*o_result_jso, RFPROP_SPSERVICE_UP_VERSION_DATA,
        (check_string_val_effective(ver_info.dr_v) == RET_OK) ? json_object_new_string(ver_info.dr_v) : NULL);
    if (strlen(ver_info.euler_v)) {
        json_object_object_add(*o_result_jso, RFPROP_SPSERVICE_UP_VERSION_OS,
            (check_string_val_effective(ver_info.euler_v) == RET_OK) ? json_object_new_string(ver_info.euler_v) : NULL);
    }
    if (strlen(ver_info.major_v)) {
        json_object_object_add(*o_result_jso, RFPROP_SPSERVICE_UP_MAJORVERSION,
            (check_string_val_effective(ver_info.major_v) == RET_OK) ? json_object_new_string(ver_info.major_v) : NULL);
    }

    

    __fill_component_messages(ver_info.component_messages, o_result_jso);
    return HTTP_OK;
}


LOCAL gint32 _get_spservice_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};
    json_object *tmp_json = NULL;
    json_object *jso = NULL;
    gchar   spraid_uri[MAX_URI_LENGTH] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    // 获取target的信息
    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SP_ACTION_TARGETURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
    }

    tmp_json = json_object_new_string((const gchar *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, RFPROP_TARGET, tmp_json);
    tmp_json = NULL;

    // 获取actioninfo的信息
    (void)memset_s(uri, sizeof(uri), 0x00, sizeof(uri));

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SP_ACTION_INFOURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
    }

    tmp_json = json_object_new_string((const gchar *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, RFPROP_MANAGER_SP_ACTION_INFONAME, tmp_json);

    // 生成action信息
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, RFPROP_MANAGER_SP_ACTION_NAME, jso);

    ret = snprintf_s(spraid_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_SPSERVICE_URI, slot);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));
    
    rf_add_action_prop(*o_result_jso, (const gchar *)spraid_uri, RFPROP_MANAGER_SPRAID_EXPORT_ACTION);

    
    
    rf_add_action_prop(*o_result_jso, (const gchar *)spraid_uri, RFPROP_MANAGER_SP_COPY_SCHEMA_ACTION);
    

    return HTTP_OK;
}


LOCAL gint32 _get_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_int(g_sp_timeout);

    return HTTP_OK;
}


LOCAL gint32 _get_finished(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_boolean(g_sp_finished);

    return HTTP_OK;
}


LOCAL gint32 _get_device_info_collect_enabled(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE sp_ums_obj = 0;
    guchar is_enabled = FALSE;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == FALSE) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &sp_ums_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(sp_ums_obj, UMS_PROPERTY_SP_DEVICE_INFO_COLLECT_ENABLE, &is_enabled);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_boolean(is_enabled);
    
    return HTTP_OK;
}


LOCAL gint32 _set_start_flag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_bool start_flag;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_spservice_provider[i_paras->index].property_name, o_message_jso,
        g_manager_spservice_provider[i_paras->index].property_name));

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    
    start_flag = json_object_get_boolean(i_paras->val_jso);
    return_val_do_info_if_expr(start_flag != TRUE && start_flag != FALSE, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    input_list = g_slist_append(input_list, g_variant_new_byte(start_flag));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_SET_SP_CFG_READY, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            if (start_flag == TRUE) {
                (void)create_sp_result_monitor_task();
            }
            return HTTP_OK;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s, %d:  ret :%d fail", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 _set_restart_delay(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_spservice_provider[i_paras->index].property_name, o_message_jso,
        g_manager_spservice_provider[i_paras->index].property_name));
    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    return_val_do_info_if_expr(json_type_int != json_object_get_type(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_SPSERVICE_TIMEDELY, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_SPSERVICE_TIMEDELY));

    
    return_val_do_info_if_expr(0 > json_object_get_int(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, RFPROP_SPSERVICE_TIMEDELY, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso)));
    

    input_list = g_slist_append(input_list, g_variant_new_uint32(json_object_get_int(i_paras->val_jso)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_SET_SYS_RESTART_DELAY, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s, %d:  ret :%d fail", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 _set_timeout(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    GSList *caller_info = NULL;
    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_spservice_provider[i_paras->index].property_name, o_message_jso,
        g_manager_spservice_provider[i_paras->index].property_name));

    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    g_sp_timeout = (guint32)json_object_get_int(i_paras->val_jso);

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
        "Set SP service task timeout to (%d) seconds successfully", g_sp_timeout);
    if (caller_info != NULL) {
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }
    return HTTP_OK;
}


LOCAL gint32 _set_finished(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    GSList *caller_info = NULL;
    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_spservice_provider[i_paras->index].property_name, o_message_jso,
        g_manager_spservice_provider[i_paras->index].property_name));

    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    g_sp_finished = (guint8)json_object_get_boolean(i_paras->val_jso);
    if (g_sp_finished == TRUE) {
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set SP service task status to finished");
    } else {
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set SP service task status to running");
    }

    if (caller_info != NULL) {
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }

    return HTTP_OK;
}


LOCAL gint32 _set_device_info_collect_enabled(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE sp_ums_obj = 0;
    guchar is_enabled;
    GSList* input_list = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == FALSE) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    is_enabled = json_object_get_boolean(i_paras->val_jso);

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &sp_ums_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle fail, ret=%d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(is_enabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, sp_ums_obj,
        SP_UMS_CLASS_NAME, UMS_METHOD_SET_SP_DEVICE_INFO_COLLECT_ENABLE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _act_spservice_delfile(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *str = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 nume_type;
    json_bool bool_ret;
    json_object *obj_json = NULL;

    GSList *input_list = NULL;

    
    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SP_FILE_TYPE);
        (void)create_message_info(MSGID_PROP_MISSING, SP_FILE_TYPE, o_message_jso, SP_FILE_TYPE);

        return HTTP_BAD_REQUEST;
    }

    
    bool_ret = json_object_object_get_ex(i_paras->val_jso, SP_FILE_TYPE, &obj_json);
    if (FALSE == bool_ret) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SP_FILE_TYPE);
        (void)create_message_info(MSGID_PROP_MISSING, SP_FILE_TYPE, o_message_jso, SP_FILE_TYPE);
        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(obj_json);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    
    nume_type = rf_sp_type_to_pathid(str);
    if (nume_type < 0) {
        debug_log(DLOG_ERROR, "%s: rf_sp_type_to_pathid %s", __FUNCTION__, str);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)nume_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_DEL_FILES, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return get_delete_usm_file_result(ret, o_message_jso);
}


LOCAL gint32 get_delete_usm_file_result(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case RET_OK:
        case RFERR_SUCCESS:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;

        case UMS_WR_ERR_BUSY:
            (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_IN_UPGRADING:
            (void)create_message_info(MSGID_SP_IS_BEING_UPGRADED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_CAN_NOT_GET_LOCK:
            (void)create_message_info(MSGID_RSC_IN_USE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default: 
            debug_log(DLOG_ERROR, "%s, %d:  ret :%d fail", __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 manager_spservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_spservice_provider;
        *count = sizeof(g_manager_spservice_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 _get_sp_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SP_ACTION_INFOURI, slot);

    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S  g_manager_actioninfo_sp_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_NULL, _get_sp_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_sp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_actioninfo_sp_provider;
        *count = sizeof(g_manager_actioninfo_sp_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}



gint32 rf_support_sp_service()
{
#define SP_SERVICE_ENABLE 1
    gint32 ret;
    OBJ_HANDLE pme_obj_handle = 0;
    OBJ_HANDLE ums_obj_handle = 0;
    guchar pme_support_sp_flg = 0;
    guchar bios_support_sp_flg = 0;
    guint32 product_version = 0;
    
    // 获取bios支持标志
    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &ums_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: get ums_obj_handle fail.\n", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_byte(ums_obj_handle, UMS_PROPERTY_BIOS_SUPPORT, &bios_support_sp_flg);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: get bios_support_sp_flg value fail.\n", __FUNCTION__, __LINE__));

    // 获取pme支持标志
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &pme_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: get pme_obj_handle fail.\n", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_byte(pme_obj_handle, PROTERY_PME_SERVICECONFIG_UMS, &pme_support_sp_flg);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: get pme_support_sp_flg fail.\n", __FUNCTION__, __LINE__));

    // 获取产品版本
    ret = dal_get_product_version_num(&product_version);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "%s, %d: get product_version fail.\n", __FUNCTION__, __LINE__));

    if (PRODUCT_VERSION_V3 == product_version) {
        ret = FALSE;
    } else if (PRODUCT_VERSION_V5 <= product_version) {
        ret = ((SP_SERVICE_ENABLE == pme_support_sp_flg) && (SP_SERVICE_ENABLE == bios_support_sp_flg)) ? TRUE : FALSE;
    }

    
    return ret;
}


LOCAL gint32 act_spraid_exportconfiguration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *data = "export_cfg";

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TRANS_FILE_ID_SP_RAID_EXPORT));
    input_list = g_slist_append(input_list, g_variant_new_string(SPRAID_TRIGGER_EXPORT_CFG_FILE));
    input_list =
        g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, data, strlen(data), sizeof(guchar)));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_WRITE_FILE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return get_write_ums_file_result(ret, o_message_jso);
}


LOCAL gint32 get_write_ums_file_result(gint32 ret, json_object **o_message_jso)
{
    if (VOS_OK != ret) {
        switch (ret) {
            case RFERR_INSUFFICIENT_PRIVILEGE:
                create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
                return HTTP_FORBIDDEN;
                
            case UMS_TRANS_ERR_FILE_TOO_LARGE:
            case UMS_TRANS_ERR_NO_SPACE:
                (void)create_message_info(MSGID_SP_CFG_OVERSIZED, NULL, o_message_jso);
                break;

            case UMS_TRANS_ERR_FILE_CNT_LIMITED:
                (void)create_message_info(MSGID_SP_CFG_LIMIT, NULL, o_message_jso);
                break;

            case UMS_WR_ERR_BUSY:
                (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, o_message_jso);
                break;

            case UMS_WR_ERR_IN_UPGRADING:
                (void)create_message_info(MSGID_SP_IS_BEING_UPGRADED, NULL, o_message_jso);
                break;

            case UMS_WR_ERR_CAN_NOT_GET_LOCK:
                (void)create_message_info(MSGID_RSC_IN_USE, NULL, o_message_jso);
                break;

            default:
                debug_log(DLOG_ERROR, "%s, %d:  ret :%d fail", __FUNCTION__, __LINE__, ret);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }

        debug_log(DLOG_ERROR, "%s %d: create_sp_item_template fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
        return HTTP_BAD_REQUEST;
    }
    // 成功
    (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
    return HTTP_OK;
}


gint32 redfish_sp_schema_get_bmc_file(const gchar *file_name, gchar *bmc_file_name, gint32 file_name_length)
{
    gint32 ret;
    const gchar *temp_filename = NULL;
    errno_t safe_fun_ret = EOK;

    if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_SPCFG)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_SPCFG;
    } else if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_OSINSTALL)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_OSINSTALL;
    } else if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_RAID)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_RAID;
    } else if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_RESULT)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_RESULT;
    }
    
    else if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_DEVICEINFO)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_DEVICEINFO;
    }
    
    else if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_DIAGNOSE)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_DIAGNOSE;
    } else if (0 == g_ascii_strcasecmp(file_name, SP_SCHEMA_FILE_NAME_DRIVE_ERASE)) {
        ret = VOS_OK;
        temp_filename = SP_SCHEMA_FILE_NAME_DRIVE_ERASE;
    } else {
        ret = VOS_ERR;
    }

    if (ret == VOS_OK && temp_filename != NULL) {
        safe_fun_ret = strcpy_s(bmc_file_name, file_name_length, temp_filename);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, iRet = %d\n", __FUNCTION__, safe_fun_ret));
    }

    return ret;
}


LOCAL gint32 rf_copy_sp_schema_to_bmc(const gchar *desFilePath, const gchar *srcFilePath)
{
    FILE *fp = NULL;
    gint32 ret = VOS_ERR;
    const gchar *fileData = NULL;
    size_t file_data_len = 0;
    gint32 fileLenth = 0;
    json_object *bmcSchemaObj = NULL;
    json_object *spSchemaObj = NULL;
    json_object *definitionsObj = NULL;

    return_val_do_info_if_fail(NULL != desFilePath && NULL != srcFilePath, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    bmcSchemaObj = json_object_from_file(desFilePath);
    goto_label_do_info_if_expr(NULL == bmcSchemaObj, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s: json_object_from_file failed.", __FUNCTION__));

    spSchemaObj = json_object_from_file(srcFilePath);
    goto_label_do_info_if_expr(VOS_OK == spSchemaObj, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s: json_object_from_file failed.", __FUNCTION__));

    if (json_object_get_string_len(spSchemaObj) > UMS_MAX_CFG_FILE_LEN) {
        debug_log(DLOG_ERROR, "%s: file over 2M", __FUNCTION__);
        goto ERR_EXIT;
    }

    ret = json_object_object_get_ex(spSchemaObj, SP_SCHEMA_FILE_DEEFINITIONS, &definitionsObj);
    goto_label_do_info_if_expr(TRUE != ret, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s: The key '%s' is not found in the file.", __FUNCTION__, SP_SCHEMA_FILE_DEEFINITIONS));

    (void)json_object_object_add(bmcSchemaObj, SP_SCHEMA_FILE_DEEFINITIONS, json_object_get(definitionsObj));

    fileData = dal_json_object_get_str(bmcSchemaObj);
    goto_label_do_info_if_expr(NULL == fileData, ERR_EXIT, debug_log(DLOG_ERROR, "%s: fileData is null", __FUNCTION__));

    fp = dal_fopen_check_realpath(desFilePath, "wb+", CONFG_FILE_SCHEMASTORE_EN);
    goto_label_do_info_if_expr(NULL == fp, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s: g_fopen failed.", __FUNCTION__));

    file_data_len = strlen(fileData);

    fileLenth = fwrite(fileData, file_data_len, 1, fp);
    goto_label_do_info_if_fail(fileLenth == 1, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s: fwrite failed, file_data_len=%zu", __FUNCTION__, file_data_len));
    (void)fclose(fp);
    fp = NULL;
    json_object_put(spSchemaObj);
    json_object_put(bmcSchemaObj);
    return VOS_OK;

ERR_EXIT:
    do_val_if_expr(NULL != fp, (void)fclose(fp); fp = NULL);
    json_object_put(spSchemaObj);
    json_object_put(bmcSchemaObj);
    return VOS_ERR;
}


LOCAL gint32 get_file_sha256_from_app_ini(FILE *app_ini_fp, const gchar *schema_file_name, gchar *sha256_checksum,
    guint checksum_len)
{
    errno_t safe_fun_ret = EOK;
    gchar line_buffer[MAX_LINE_SIZE] = {0};
    gchar sha256_tmp[MAX_LINE_SIZE] = {0};
    gchar *sha256_flag = NULL;

#define APP_INI_NAME_FLAG "Name:"
#define APP_INI_SHA256_FLAG "SHA256-Digest:"

    return_val_if_expr(NULL == app_ini_fp || NULL == schema_file_name || NULL == sha256_checksum || checksum_len < 1,
        VOS_ERR);

    // 文件指针指向开始
    if (VOS_OK != fseek(app_ini_fp, 0L, SEEK_SET)) {
        debug_log(DLOG_ERROR, "%s: fseek failed", __FUNCTION__);
        return VOS_ERR;
    }

    while (NULL != vos_fgets(line_buffer, sizeof(line_buffer), app_ini_fp)) {
        // 找到Name ,Name下面第二行是 sha256的值
        if (strstr(line_buffer, APP_INI_NAME_FLAG) && strstr(line_buffer, schema_file_name)) {
            (void)memset_s(line_buffer, sizeof(line_buffer), 0, sizeof(line_buffer));
            if (NULL != vos_fgets(line_buffer, sizeof(line_buffer), app_ini_fp)) {
                (void)memset_s(line_buffer, sizeof(line_buffer), 0, sizeof(line_buffer));
                if (NULL != vos_fgets(line_buffer, sizeof(line_buffer), app_ini_fp)) { // 读取sha256的值
                    if (NULL != (sha256_flag = strstr(line_buffer, APP_INI_SHA256_FLAG))) {
                        safe_fun_ret =
                            strncpy_s(sha256_tmp, sizeof(sha256_tmp), sha256_flag + strlen(APP_INI_SHA256_FLAG),
                            strlen(sha256_flag) - strlen(APP_INI_SHA256_FLAG));
                        do_val_if_expr(safe_fun_ret != EOK,
                            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                        dal_trim_string(sha256_tmp, MAX_LINE_SIZE);
                        safe_fun_ret = strncpy_s(sha256_checksum, checksum_len, sha256_tmp, checksum_len - 1);
                        do_val_if_expr(safe_fun_ret != EOK,
                            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                        return VOS_OK;
                    }
                }
            }
        }
        (void)memset_s(line_buffer, sizeof(line_buffer), 0, sizeof(line_buffer));
    }

    return VOS_ERR;
}

#define MAX_SP_ZIP_FILE_LENGTH (2 * 1024 * 1024)
#define DEV_SCHEMA_DIR "/dev/shm/schema"
#define DEV_SP_SCHEMA_DIR DEV_SCHEMA_DIR "/spschema"
#define DEV_SP_UPDATE_INFO_DIR DEV_SCHEMA_DIR "/updateinfo"
#define DEV_SP_UPDATE_APP_INI DEV_SCHEMA_DIR "/updateinfo/APP.ini"
#define SP_SCHEMA_MAX_PATH_NUM  255


LOCAL gint32 unzip_sp_schema_files(char *file_path, gint32 path_len)
{
#define SP_SCHEMA_PARA_NUM 10

    char* argv[SP_SCHEMA_PARA_NUM] = {NULL};
    gint32 ret;
    sig_t old_handler;
    gint32 i = 0;
    gchar cmd[MAX_CMD_LENGTH] = {0};
    FILE *fp = NULL;
    gint32 count = 0;

    return_val_do_info_if_expr(path_len > MAX_SCHAME_FILEPATH_LENGTH, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:schame file path is too long:%d.", __FUNCTION__, path_len));
    if (dal_check_shell_special_character(file_path) != RET_OK) {
        debug_log(DLOG_ERROR, "%s:command dal_check_shell_special_character failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = snprintf_s(cmd, MAX_CMD_LENGTH, MAX_CMD_LENGTH - 1,
                     "unzip -l %s 2> /dev/null | grep -E '[0-9-]+ [0-9:]+' | sed -r 's/[0-9 ]+[0-9-]+ [0-9:]+   (.*)/\\1/g' | awk '{if(length($0) > %d) {print $0}}' | wc -l",
                     file_path, (int)(SP_SCHEMA_MAX_PATH_NUM - (strlen(DEV_SCHEMA_DIR) + 1)));
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    fp = popen(cmd, "r");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: popen failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = fscanf_s(fp, "%d", &count);
    pclose(fp);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: fscanf_s failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    if (count > 0) {
        debug_log(DLOG_ERROR, "%s: has more than %d characters filepath, ret=%d", __FUNCTION__, 
                  (int)(SP_SCHEMA_MAX_PATH_NUM - (strlen(DEV_SCHEMA_DIR) + 1)), ret);
        return RET_ERR;
    }

    argv[i++] = "/usr/bin/unzip";
    argv[i++] = "-o";
    argv[i++] = "-d";
    argv[i++] = DEV_SCHEMA_DIR;
    argv[i++] = file_path;

    old_handler = signal(SIGCHLD, SIG_DFL);
    ret = vos_system_s("/usr/bin/unzip", argv);
    (void)signal(SIGCHLD, old_handler);
    return_val_do_info_if_expr(ret != 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:unzip_sp_schema_files failed.", __FUNCTION__));
    return VOS_OK;
}


LOCAL void *update_schema_files(void *data)
{
    int iRet;
    gint32 ret;
    gint result_code = VOS_ERR;
    UPDATE_SP_SCHEMA_NEED_INFO *update_schema_info_status = NULL;
    DIR *p_dir = NULL;
    struct dirent *p_entry = NULL;
    gchar dest_filepath[MAX_FILEPATH_LENGTH + 1] = {0};
    gchar src_filepath[MAX_FILEPATH_LENGTH + 1] = {0};
    gchar bmc_file_name[MAX_FILEPATH_LENGTH] = {0};
    gchar dev_sp_schema_path[MAX_FILEPATH_LENGTH + 10] = {0}; // /dev/shm/xxx.zip
    gchar tmp_sp_schema_path[MAX_FILEPATH_LENGTH + 10] = {0}; // /tmp/web/xxx.zip
    struct stat zip_stat;
    FILE *sha256_file_fp = 0;
    gchar sha256_checksum[PROP_VAL_LENGTH + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    guint8 is_from_webui;
    const gchar *user_name = NULL;
    const gchar *client = NULL;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfUpdateSchema");
    return_val_do_info_if_fail(NULL != data, NULL, debug_log(DLOG_ERROR, "%s:data is NULL.", __FUNCTION__));

    update_schema_info_status = (UPDATE_SP_SCHEMA_NEED_INFO *)data;

    is_from_webui = update_schema_info_status->is_from_webui;
    user_name = update_schema_info_status->user_name;
    client = update_schema_info_status->client;

    iRet = snprintf_s(tmp_sp_schema_path, sizeof(tmp_sp_schema_path), sizeof(tmp_sp_schema_path) - 1, "%s/%s",
        REDFISH_UPLOAD_FILE_PATH, update_schema_info_status->file_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    iRet = snprintf_s(dev_sp_schema_path, sizeof(dev_sp_schema_path), sizeof(dev_sp_schema_path) - 1, "%s/%s",
        DEV_TMP_DIR, update_schema_info_status->file_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    // 校验路径
    ret = dal_check_real_path2((const gchar *)tmp_sp_schema_path, TMP_PATH_WITH_SLASH);
    goto_label_do_info_if_fail(VOS_OK == ret, END,
        debug_log(DLOG_ERROR, "%s: dal_check_real_path2 failed", __FUNCTION__));

    (void)memset_s(&zip_stat, sizeof(zip_stat), 0, sizeof(zip_stat));
    ret = stat(tmp_sp_schema_path, &zip_stat);
    goto_label_do_info_if_fail(VOS_OK == ret, END,
        debug_log(DLOG_ERROR, "%s: stat failed", __FUNCTION__));

    // 只支持普通文件
    if (!S_ISREG(zip_stat.st_mode)) {
        debug_log(DLOG_ERROR, "%s: file is not regular", __FUNCTION__);
        goto END;
    }

    // 判断zip长度是否符合要求
    if (zip_stat.st_size > MAX_SP_ZIP_FILE_LENGTH) {
        debug_log(DLOG_ERROR, "%s: file length over %u", __FUNCTION__, MAX_SP_ZIP_FILE_LENGTH);
        goto END;
    }

    // 复制文件到  /dev/shm(/tmp/web目录下的文件属组apache:apache,修改属组后使得redfish拷贝成功)
    ret = proxy_modify_file_mode(is_from_webui, user_name, client, tmp_sp_schema_path, REDFISH_USER_UID, APPS_USER_GID);
    goto_label_do_info_if_fail(VOS_OK == ret, END,
        debug_log(DLOG_ERROR, "%s: modify file mode failed", __FUNCTION__));

    (void)dal_set_file_mode((const gchar *)tmp_sp_schema_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    ret = vos_file_copy((const gchar *)dev_sp_schema_path, (const gchar *)tmp_sp_schema_path);
    goto_label_do_info_if_fail(VOS_OK == ret, END,
        debug_log(DLOG_ERROR, "%s: copy failed", __FUNCTION__));

    // 删除tmp下的/tmp/web/xxx.zip文件
    (void)vos_rm_filepath((const gchar *)tmp_sp_schema_path);

    // 校验zip的sha256值
    ret = verify_file_checksum(dev_sp_schema_path, update_schema_info_status->checksum_val);
    goto_label_do_info_if_fail(VOS_OK == ret, END,
        debug_log(DLOG_ERROR, "%s: verify_schema_checksum failed", __FUNCTION__));

    // 解压缩
    if (vos_check_dir(DEV_SCHEMA_DIR) == FALSE) {
        ret = VOS_ERR;
        debug_log(DLOG_ERROR, "%s: vos_check_dir %s failed", __FUNCTION__, DEV_SCHEMA_DIR);
        goto END;
    }
    ret = unzip_sp_schema_files(dev_sp_schema_path, MAX_SCHAME_FILEPATH_LENGTH);
    goto_label_do_info_if_fail(VOS_OK == ret, END,
        debug_log(DLOG_ERROR, "%s: unzip failed", __FUNCTION__));

    
    ret = uip_call_class_method_redfish(is_from_webui, user_name, client, update_schema_info_status->obj_handle,
        PFN_CLASS_NAME, METHOD_UPDATE_SP_SCHEMA_PREPARE, AUTH_ENABLE, update_schema_info_status->user_role_privilege,
        NULL, NULL);

    goto_label_do_info_if_fail(ret == VOS_OK, END,
        debug_log(DLOG_ERROR, "%s:uip_call_class_method_redfish %s failed.", __FUNCTION__,
        METHOD_UPDATE_SP_SCHEMA_PREPARE));

    
    p_dir = opendir(DEV_SP_SCHEMA_DIR);
    goto_label_do_info_if_fail(NULL != p_dir, END,
        debug_log(DLOG_ERROR, "%s: the path is non-exist!", __FUNCTION__));

    sha256_file_fp = fopen(DEV_SP_UPDATE_APP_INI, "r");
    goto_label_do_info_if_expr(NULL == sha256_file_fp, END,
        debug_log(DLOG_ERROR, "%s: fopen [%s] failed", __FUNCTION__, DEV_SP_UPDATE_APP_INI));

    // rtos7上面删除 readdir_r了，使用 readdir函数
    while ((p_entry = readdir(p_dir)) != NULL) {
        if (p_entry->d_type == DT_REG) {
            if (strlen(p_entry->d_name) > strlen(".json") &&
                TRUE != g_str_has_suffix(p_entry->d_name, "Collection.json")) {
                ret = redfish_sp_schema_get_bmc_file(p_entry->d_name, bmc_file_name, sizeof(bmc_file_name));
                continue_if_expr(VOS_OK != ret);

                (void)memset_s(sha256_checksum, sizeof(sha256_checksum), 0, sizeof(sha256_checksum));
                ret = get_file_sha256_from_app_ini(sha256_file_fp, p_entry->d_name, sha256_checksum,
                    sizeof(sha256_checksum));
                continue_do_info_if_fail(VOS_OK == ret,
                    debug_log(DLOG_ERROR, "%s: get %s checksum failed", __FUNCTION__, p_entry->d_name));

                iRet = snprintf_s(src_filepath, sizeof(src_filepath), sizeof(src_filepath) - 1, "%s/%s",
                    DEV_SP_SCHEMA_DIR, p_entry->d_name);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

                dal_trim_string(sha256_checksum, PROP_VAL_LENGTH + 1);
                ret = verify_file_checksum(src_filepath, sha256_checksum);
                continue_do_info_if_fail(VOS_OK == ret,
                    debug_log(DLOG_ERROR, "%s: verify_file_checksum failed", __FUNCTION__));

                iRet = snprintf_s(dest_filepath, sizeof(dest_filepath), sizeof(dest_filepath) - 1, "%s/%s",
                    CONFG_FILE_SCHEMASTORE_EN, bmc_file_name);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

                ret = rf_copy_sp_schema_to_bmc(dest_filepath, src_filepath);
                continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR,
                    "%s: rf_copy_sp_schema_to_bmc failed", __FUNCTION__));
            }
        }
    }

    update_schema_info_status->update_process = VOS_OK;
    result_code = VOS_OK;
END:
    do_if_expr(sha256_file_fp != NULL, (void)fclose(sha256_file_fp));
    do_if_expr(NULL != p_dir, (void)closedir(p_dir));

    
    (void)proxy_delete_file(is_from_webui, user_name, client, tmp_sp_schema_path);

    // 删除 /dev/shm 下的 zip和 解压出来的 目录(代理接口删除文件)
    (void)proxy_delete_file(is_from_webui, user_name, client, dev_sp_schema_path);
    (void)proxy_delete_file(is_from_webui, user_name, client, DEV_SCHEMA_DIR);

    // 清除flash写入量
    (void)dal_get_object_handle_nth(CLASS_NAND_FLASH, 0, &obj_handle);
    (void)dfl_call_class_method(obj_handle, METHOD_CLEAR_FLASH_STATISTICS, NULL, NULL, NULL);

    do_if_fail(VOS_OK == result_code, (update_schema_info_status->update_process = VOS_ERR));
    return NULL;
}


LOCAL gint32 update_schema_status_monitor(const char *obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    UPDATE_SP_SCHEMA_NEED_INFO *update_schema_info_status = (UPDATE_SP_SCHEMA_NEED_INFO *)monitor_fn_data->user_data;
    return_val_do_info_if_fail((obj_path != NULL && message_obj != NULL && (&(monitor_fn_data->user_data)) != NULL),
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));

#define COPY_SP_SCHEMA_TIMEOUT 30

    user_name = update_schema_info_status->user_name;
    client = update_schema_info_status->client;

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    update_schema_info_status->update_timeout = update_schema_info_status->update_timeout + 1;
    return_val_do_info_if_expr((update_schema_info_status->update_timeout) * REDFISH_TASK_INTERVAL_IN_SEC >=
        COPY_SP_SCHEMA_TIMEOUT,
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MESSAGE_OPERATIONTIMEOUT, NULL, message_obj));

    switch (update_schema_info_status->update_process) {
        case VOS_OK:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
            (void)record_update_schema_method_operation_log(update_schema_info_status->is_from_webui, user_name, client,
                TRUE);
            break;
        case VOS_ERR:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_TASK_UPDATE_SCHEMA_FAILED, NULL, message_obj);
            (void)record_update_schema_method_operation_log(update_schema_info_status->is_from_webui, user_name, client,
                FALSE);
            break;
        default:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            monitor_fn_data->task_progress = 0;
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_obj);
            break;
    }

    return VOS_OK;
}


LOCAL gint32 verify_file_checksum(const gchar *file_path, const gchar *input_checksum)
{
    gint retval;
    GError *err = NULL;
    gsize contents_len = 0;
    gchar *contents_buf = NULL;
    gchar *cksum_str = NULL;

    return_val_if_expr(NULL == input_checksum, VOS_ERR);

    retval = g_file_get_contents(file_path, &contents_buf, &contents_len, &err);
    return_val_do_info_if_fail(TRUE == retval, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:g_file_get_contents failed: %s.", __FUNCTION__, err->message));

    cksum_str = g_compute_checksum_for_data(G_CHECKSUM_SHA256, (const guchar *)contents_buf, contents_len);
    g_free(contents_buf);
    return_val_do_info_if_expr(!cksum_str, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:g_compute_checksum_for_data(sha256) failed", __FUNCTION__));

    if (0 != g_strcmp0(input_checksum, cksum_str)) {
        g_free(cksum_str);
        return VOS_ERR;
    }
    g_free(cksum_str);

    return VOS_OK;
}


LOCAL gint32 get_update_schema_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                                    UPDATE_SP_SCHEMA_NEED_INFO *schema_info)
{
    const gchar *checksum_val = NULL;
    const gchar *file_name = NULL;

    (void)get_element_str(i_paras->val_jso, RF_PROP_SP_SCHEMA_FILE_NAME, &file_name);
    if (file_name == NULL) {
        debug_log(DLOG_ERROR, "%s: %s null.", __FUNCTION__, RF_PROP_SP_SCHEMA_FILE_NAME);
        goto ERR_EXIT;
    }

    (void)get_element_str(i_paras->val_jso, RF_PROP_CHECKSUM_VALUE, &checksum_val);
    if (checksum_val == NULL) {
        debug_log(DLOG_ERROR, "%s: %s null.", __FUNCTION__, RF_PROP_CHECKSUM_VALUE);
        goto ERR_EXIT;
    }

    if (strncpy_s(schema_info->user_name, sizeof(schema_info->user_name), i_paras->user_name,
        strlen(i_paras->user_name)) != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s user_name failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    if (strncpy_s(schema_info->client, sizeof(schema_info->client), i_paras->client, 
        strlen(i_paras->client)) != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s client failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    if (strncpy_s(schema_info->checksum_val, sizeof(schema_info->checksum_val), checksum_val, 
        strlen(checksum_val)) != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s checksum_val failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    if (strncpy_s(schema_info->file_name, sizeof(schema_info->file_name), file_name, strlen(file_name)) != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s file_name failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    schema_info->obj_handle = i_paras->obj_handle;
    schema_info->is_from_webui = i_paras->is_from_webui;
    schema_info->update_process = UPDATE_SCHEMA_IN_PROCESS;
    schema_info->user_role_privilege = i_paras->user_role_privilege;

    return RET_OK;

ERR_EXIT:

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 act_spservice_update_schema(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    TASK_MONITOR_INFO_S *update_monitor_data = NULL;
    UPDATE_SP_SCHEMA_NEED_INFO *update_schema_info = NULL;
    gulong task_id = 0;
    guint8 ums_server_flag = 0;
    const gchar *file_name = NULL;
    gchar file_path[MAX_FILEPATH_LENGTH + 10] = {0}; // 文件路径/tmp/web/xxx.zip

    return_val_do_info_if_fail((o_message_jso != NULL) && (i_paras != NULL), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    (void)get_element_str(i_paras->val_jso, RF_PROP_SP_SCHEMA_FILE_NAME, &file_name);
    goto_label_do_info_if_expr(file_name == NULL, ERR_EXIT,
        debug_log(DLOG_ERROR, "%s: %s null.", __FUNCTION__, RF_PROP_SP_SCHEMA_FILE_NAME));

    iRet =
        snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "%s/%s", REDFISH_UPLOAD_FILE_PATH, file_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    if (!((i_paras->user_role_privilege & USERROLE_BASICSETTING) &&
        (rf_inner_session_id_check(i_paras->session_id) == VOS_OK) &&
        (get_ums_server_flag(&ums_server_flag) == VOS_OK) && 
        (ums_server_flag == UMS_X86_MONOPOLY))) {
        debug_log(DLOG_ERROR, "%s: check privilege fail.", __FUNCTION__);
        (void)record_update_schema_method_operation_log(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            FALSE);
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 该内存，随redfish task删除时释放
    update_schema_info = (UPDATE_SP_SCHEMA_NEED_INFO *)g_malloc0(sizeof(UPDATE_SP_SCHEMA_NEED_INFO));
    if (update_schema_info == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 update schema info failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    // 获取update schema info信息
    ret = get_update_schema_info(i_paras, o_message_jso, update_schema_info);
    if (ret != RET_OK) {
        g_free(update_schema_info);
        update_schema_info = NULL;
        debug_log(DLOG_ERROR, "%s: get update schema info fail, ret = %d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    // 检查绝对路径是否在/tmp/web目录下
    ret = dal_check_real_path2((const gchar *)file_path, REDFISH_UPLOAD_FILE_PATH);
    if (ret != RET_OK) {
        g_free(update_schema_info);
        update_schema_info = NULL;
        debug_log(DLOG_ERROR, "%s: check real path fail, ret = %d", __FUNCTION__, ret);
        (void)record_update_schema_method_operation_log(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            FALSE);
        (void)create_message_info(MSGID_ILLEGAL_URI_PATH, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    // 线程创建成功后，线程函数负责删除临时文件
    ret = vos_task_create(&task_id, "UdateSPSchemaFilesThread", (TASK_ENTRY)update_schema_files, update_schema_info,
        DEFAULT_PRIORITY);
    goto_label_do_info_if_fail(ret == VOS_OK, ERR_EXIT, g_free(update_schema_info); update_schema_info = NULL;
        debug_log(DLOG_ERROR, "%s: vos_task_create failed.", __FUNCTION__));

    update_monitor_data = task_monitor_info_new(g_free);
    return_val_do_info_if_fail(update_monitor_data != NULL, HTTP_INTERNAL_SERVER_ERROR, g_free(update_schema_info);
        update_schema_info = NULL; debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        (void)record_update_schema_method_operation_log(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        FALSE););

    
    update_monitor_data->task_progress = TASK_NO_PROGRESS;
    update_monitor_data->user_data = update_schema_info;
    update_monitor_data->rsc_privilege = USERROLE_BASICSETTING;

    
    ret = create_new_task(RF_UPDATE_SPSERVICE_SCHEMA_FILES, update_schema_status_monitor, update_monitor_data, NULL,
        i_paras->uri, o_result_jso);
    return_val_do_info_if_fail(ret == RF_OK, HTTP_INTERNAL_SERVER_ERROR, task_monitor_info_free(update_monitor_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: create new task failed.", __FUNCTION__);
        (void)record_update_schema_method_operation_log(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        FALSE));
    return HTTP_ACCEPTED;

ERR_EXIT:

    (void)record_update_schema_method_operation_log(i_paras->is_from_webui, i_paras->user_name, i_paras->client, FALSE);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _get_spraid_export_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SPRAID_EXPORT_ACTION_INFOURI, slot);

    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S  g_manager_spraid_export_config_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_NULL, _get_spraid_export_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_spraid_export_config_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_spraid_export_config_actioninfo_provider;
        *count = sizeof(g_manager_spraid_export_config_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 _get_update_sp_schema_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint retval;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    retval = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_UPDATE_SCHEMA_ACTION_INFOURI, slot);
    return_val_do_info_if_fail(retval > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S  g_manager_update_sp_schema_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_update_sp_schema_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_update_sp_schema_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s: not support", __FUNCTION__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_update_sp_schema_actioninfo_provider;
        *count = sizeof(g_manager_update_sp_schema_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}

void write_sp_task_desc(PROVIDER_PARAS_S* i_paras, const gchar* file_content)
{
    gint32 ret;
    GSList* input_list = NULL;

    if (i_paras == NULL || file_content == NULL) {
        debug_log(DLOG_ERROR, "%s:Input param error", __FUNCTION__);
        return;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TRANS_FILE_ID_SP_RESULT));
    input_list = g_slist_append(input_list, g_variant_new_string(SP_TASK_DESC_FILE_NAME));
    input_list = g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, file_content,
        strlen(file_content), sizeof(guchar)));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        0, SP_UMS_CLASS_NAME, UMS_METHOD_WRITE_FILE, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%s failed, ret = %d", __FUNCTION__, UMS_METHOD_WRITE_FILE, ret);
    }

    return;
}