

#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_util.h"
#include "../apps/bmc_global/inc/bmc.h"
#include "../apps/upgrade/inc/upgrade_pub.h"

#define INVALID_IP_MODE 255

LOCAL PROPERTY_PROVIDER_S *get_self_provider_manager(PROVIDER_PARAS_S *i_paras);
LOCAL gint32 get_manager_action_target(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 act_manager_export_dump(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_manager_export_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_manager_import_config_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_manager_power_on(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 file_uploading(PROVIDER_PARAS_S *i_paras, const gchar *transfer_protocol, gint32 file_id,
    const gchar *temp_file_path, json_object **o_message_jso);
LOCAL gint32 file_downloading(PROVIDER_PARAS_S *i_paras, const gchar *transfer_protocol, gint32 file_id,
    json_object **o_message_jso);
LOCAL gint32 parse_dumping_err_code(gint32 transfer_status, json_object **message_obj);
LOCAL gint32 parse_collecting_err_code(gint32 transfer_status, const gchar *err_str, json_object **message_obj);
LOCAL gint32 dumping(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
LOCAL gint32 collecting(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
LOCAL gint32 importing_configuration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
LOCAL gint32 importing_configuration_in_task(json_object *user_data, json_object *body_jso, const gchar *file_path,
    json_object **o_message_jso);
LOCAL gint32 get_dumping_progress(gint32 *progress);
LOCAL gint32 get_collecting_progress(gint32 *progress, gchar *err_str, guint32 buffer_len);
LOCAL gint32 get_dumping_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj, guint8 local);
LOCAL gint32 get_collecting_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 get_dump_file_transfer_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 get_config_file_transfer_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL void export_monitor_info_free_func(void *monitor_data);
LOCAL void import_manager_monitor_info_free_func(void *monitor_data);
LOCAL TASK_MONITOR_INFO_S *export_monitor_info_new(PROVIDER_PARAS_S *i_paras);
LOCAL TASK_MONITOR_INFO_S *import_manager_monitor_info_local(PROVIDER_PARAS_S *i_paras);
LOCAL TASK_MONITOR_INFO_S *import_manager_monitor_info_uri(PROVIDER_PARAS_S *i_paras);
LOCAL gint32 export_status_monitor_local(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 import_status_monitor_uri(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 import_status_monitor_local(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
static gint32 export_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 act_manager_export_file_local(PROVIDER_PARAS_S *i_paras, guint8 opt_priv, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_manager_import_file_local(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_manager_export_file_uri(PROVIDER_PARAS_S *i_paras, guint8 opt_priv, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_manager_import_file_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 act_manager_delete_language(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_manager_restore_factory(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 act_manager_general_download(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_manager_set_fusion_partition(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *o_result_jso);
LOCAL gint32 set_manager_shared_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_device_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_stateless(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 set_manager_loginrule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_sys_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_search(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_login_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint32 id);
LOCAL gint32 set_manager_login_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_start_jso, json_object **o_result_end_jso, guint32 id);
LOCAL gint32 set_manager_login_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guint32 id);
LOCAL gint32 set_manager_login_mac(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guint32 id);
LOCAL gint32 get_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_provider_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_system_manager_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_system_manager_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_system_manager_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_system_manager_locked_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

void get_manager_oem_language_set(json_object **oem_property);
void get_manager_oem_permit_rule(json_object **oem_property);
LOCAL gint32 set_manager_vga_enabled(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso, json_object *prop_jso);

gint32 get_sp_status(struct json_object *object);


LOCAL PROPERTY_PROVIDER_S g_manager_action_info_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_action_info_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
};




LOCAL gint32 get_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar  username[USER_USERNAME_MAX_LEN + 1] = {0};
    guint8 software_type = 0;
    
    const gchar*        manager_action_info_array[] = {
        RFPROP_MANAGER_RESET,
        RFPROP_MANAGER_DELETE_LANGUAGE,
        RFPROP_MANAGER_RESTORE_FACTORY,
        RFPROP_MANAGER_ROLLBACK,
        RFPROP_MANAGER_EXPORT_DUMP,
        RFPROP_MANAGER_EXPORT_CONFIG,
        RFPROP_MANAGER_IMPORT_CONFIG,
        RFPROP_MANAGER_POWER_ON,
        RFPROP_MANAGER_GENERAL_DOWNLOAD,
        RFPROP_MANAGER_SET_FUSION_PARTITION,
        
        RFPROP_MANAGER_POWER_ON,
        RFPROP_MANAGER_FORCE_FAILOVER,
        RFPROP_MANAGER_DELETE_SWI_PROFILE,
        
        
        RFPROP_MANAGER_RESTORE_SWI_PROFILE,
        RFPROP_MANAGER_PARSE_SWI_PROFILE,
        RFPROP_MANAGER_SHELF_POWER_CONTROL

    };
    gchar               slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar               actioninfo_uri[MAX_URI_LENGTH] = {0};
    gchar               manager_uri[MAX_URI_LENGTH] = {0};
    

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    }
    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) { // HMM板名字与URI有关，HMM1或HMM2
        (void)get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, slot_id, MAX_RSC_NAME_LEN);
    } else {
        (void)redfish_get_board_slot(slot_id, MAX_RSC_NAME_LEN);
    }
    
    ret = snprintf_s(manager_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_MANAGER, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    
    ret = rf_update_rsc_actioninfo_uri(manager_action_info_array, G_N_ELEMENTS(manager_action_info_array), i_paras->uri,
        manager_uri, actioninfo_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "update action info uri (%s) failed", i_paras->uri));

    *o_result_jso = json_object_new_string((const gchar *)actioninfo_uri);

    return HTTP_OK;
    
}


gint32 redfish_get_format_time(PROVIDER_PARAS_S *i_paras, guint32 *p_datetime, gint16 *p_timezone)
{
    gint32 ret;
    gulong time_stamp;
    gint16 time_offset = 0;

    return_val_if_fail(NULL != i_paras, VOS_ERR);

    time_stamp = vos_get_cur_time_stamp();
    ret = dal_get_localtime_offset(&time_offset);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    if (NULL != p_timezone) {
        // 有可能返回RFERR_INSUFFICIENT_PRIVILEGE无权限. 注意这里不能使用i_paras->is_satisfy_privi判断!
        return_val_if_fail((i_paras->user_role_privilege & USERROLE_READONLY), RFERR_INSUFFICIENT_PRIVILEGE);

        // 视需要出参timezone
        *p_timezone = time_offset;
    }

    
    if (NULL != p_datetime) {
        // 视需要出参datetime
        *p_datetime = (guint32)time_stamp;
    }

    return VOS_OK;
}


gint32 redfish_fill_format_time(gchar *str_buf, guint32 str_len, guint32 *p_datetime, gint16 *p_timezone)
{
    gulong time_now = 0;
    gint16 time_zone = 0;
    struct tm ptm = { 0 };
    gint32 len = str_len;
    guint32 n = 0;

    // 若不需要格式化填充strbuf, 直接返回OK.
    return_val_if_fail((NULL != str_buf) && (str_len > 0), VOS_ERR);

    (void)memset_s(str_buf, str_len, 0, str_len);

    // 格式化填充datetime
    if (NULL != p_datetime) {
        time_now = *p_datetime;

        (void)dal_localtime_r((time_t *)&time_now, &ptm);

        snprintf_s(&str_buf[n], len, len - 1, "%.4d-%.2d-%.2dT%.2d:%.2d:%.2d", ptm.tm_year + 1900, ptm.tm_mon + 1,
            ptm.tm_mday, ptm.tm_hour, ptm.tm_min, ptm.tm_sec);
    }

    // 格式化填充timezone
    if (NULL != p_timezone) {
        n = strlen(str_buf);
        len = str_len - n; // 剩余长度
        return_val_if_fail(len > 0, VOS_ERR);

        time_zone = *p_timezone;

        if (time_zone < 0) {
            snprintf_s(&str_buf[n], len, len - 1, "-%.2d:%.2d", -time_zone / 60, -time_zone % 60);
        } else {
            snprintf_s(&str_buf[n], len, len - 1, "+%.2d:%.2d", time_zone / 60, time_zone % 60);
        }
    }

    return VOS_OK;
}


LOCAL gint32 get_manager_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, "/redfish/v1/Managers/%s");
}


LOCAL gint32 get_manager_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string((const gchar *)slot_str);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_manager_bmc_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return redfish_get_property_value_string(get_self_provider_manager(i_paras), i_paras, o_message_jso, o_result_jso);
}


gint32 get_manager_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint16 time_zone = 0; 
    guint32 time_now = 0;
    char strbuf[STRING_LEN_MAX] = {0};
    PROPERTY_PROVIDER_S *self = get_self_provider_manager(i_paras);
    
    return_val_do_info_if_fail((o_message_jso != NULL) && (o_result_jso != NULL) && (self != NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    ret = redfish_get_format_time(i_paras, &time_now, &time_zone);

    
    return_val_do_info_if_expr(ret == RFERR_INSUFFICIENT_PRIVILEGE, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get datetime error", __FUNCTION__));
    
    ret = redfish_fill_format_time(strbuf, sizeof(strbuf), &time_now, &time_zone);

    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: fill datetime error", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)strbuf);
    
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string error", __FUNCTION__));
    
    return HTTP_OK;
}


LOCAL gint32 set_manager_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return_val_do_info_if_fail((NULL != o_message_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_DATATIME, o_message_jso,
        RFPROP_MANAGER_DATATIME);
    return HTTP_NOT_IMPLEMENTED;
}

gint32 get_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    char strbuf[64] = {0};
    PROPERTY_PROVIDER_S *self = get_self_provider_manager(i_paras);

    

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    
    return_val_do_info_if_expr(0 == (i_paras->user_role_privilege & USERROLE_READONLY), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dfl_get_object_handle(OBJECT_BMC, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get obj_handle error", __FUNCTION__));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BMC_TIMEZONE_STR, strbuf, sizeof(strbuf));
    *o_result_jso = json_object_new_string((const gchar *)strbuf);
    
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string error", __FUNCTION__));
    

    return HTTP_OK;
}

gint32 set_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    char strbuf[64] = {'\0', };
    gchar time_zone[64] = {0};
    const char *strtmp = NULL;
    gchar *strnew = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar property_data = 0;

    
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_byte(obj_handle, BMC_TIMESRC_NAME, &property_data);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(2 != property_data, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_TIMEZONE, o_message_jso,
        RFPROP_MANAGER_TIMEZONE));
    
    

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_TIMEZONE, o_message_jso,
        RFPROP_MANAGER_TIMEZONE));

    
    
    return_val_do_info_if_fail((NULL != i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_TIMEZONE, o_message_jso, RF_VALUE_NULL,
        RFPROP_MANAGER_TIMEZONE));
    

    (void)memset_s(strbuf, sizeof(strbuf), 0, sizeof(strbuf));
    strtmp = dal_json_object_get_str(i_paras->val_jso);
    
    (void)strncpy_s(strbuf, sizeof(strbuf), strtmp, sizeof(strbuf) - 1);
    
    strnew = strbuf;

    
    if ('-' == strnew[0] || '+' == strnew[0]) {
        strncat_s(time_zone, sizeof(time_zone), TIME_ZONE_UTC, strlen(TIME_ZONE_UTC));
        strncat_s(time_zone, sizeof(time_zone), strnew, strlen(strnew));
    }
    
    else if ('0' <= strnew[0] && strnew[0] <= '9') {
        
        strncat_s(time_zone, sizeof(time_zone), "UTC+", strlen("UTC+"));
        strncat_s(time_zone, sizeof(time_zone), strnew, strlen(strnew));
    } else {
        strncpy_s(time_zone, sizeof(time_zone), strnew, sizeof(time_zone) - 1);
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)time_zone));
    

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_BMC, METHOD_BMC_TIMEZONE_STR, 0, i_paras->user_role_privilege, input_list, NULL);

    
    uip_free_gvariant_list(input_list);

    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;

        case RFERR_NO_RESOURCE: 
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        case RFERR_WRONG_PARAM:
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_MANAGER_TIMEZONE, o_message_jso,
                (const gchar *)time_zone, RFPROP_MANAGER_TIMEZONE);
            return HTTP_BAD_REQUEST;
            

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_manager_networkprotocol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar NetConfigEnable = 0;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_NETCONFIG, &NetConfigEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_NetConfigEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail((SERVICE_NETCONFIG_ENABLE == NetConfigEnable), HTTP_INTERNAL_SERVER_ERROR);
    
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_NETWORKPROTOCOL, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_manager_ethernetInterfaces(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar NetConfigEnable = 0;

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_NETCONFIG, &NetConfigEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_NetConfigEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail((SERVICE_NETCONFIG_ENABLE == NetConfigEnable), HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_ETHERNETINTERFACES, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


void get_manager_oem_product_id(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint32 out_val = 0;
    gchar product_id[PROPERTY_LEN] = {0};

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail.", __FUNCTION__));

    guint32 product_vendor_id = 0;
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PRODUCT_VENDOR_ID, &product_vendor_id);
    if (ret == RET_OK && product_vendor_id != INVALID_DATA_DWORD) {
        (void)dal_get_property_value_uint32(obj_handle, PROPERTY_PRODUCT_UNIQUE_ID, &out_val);

        (void)snprintf_s(product_id, sizeof(product_id), sizeof(product_id) - 1, "0x%08X%08X",
            product_vendor_id, out_val); // 将数字转换成为16进制的字符串
        *oem_property = json_object_new_string((const gchar *)product_id);
        return;
    }

    
    if (dal_is_customized_machine()) {
        ret = dal_get_property_value_uint32(obj_handle, PROPERTY_CB_PRODUCT_UNIQUE_ID, &out_val);
    } else {
        ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PRODUCT_UNIQUE_ID, &out_val);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_property_value_uint32 fail.", __FUNCTION__);
        return;
    }
    
    (void)snprintf_s(product_id, sizeof(product_id), sizeof(product_id) - 1, "0x%08X",
        out_val); // 将数字转换成为16进制的字符串
    
    *oem_property = json_object_new_string((const gchar *)product_id);
    return_do_info_if_fail(NULL != *oem_property,
        debug_log(DLOG_ERROR, "%s json_object_new_int64 fail.", __FUNCTION__));

    return;
}


LOCAL void get_manager_oem_share_info(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    const guint8 *share_info_value = NULL;
    json_object *property = NULL;
    gsize parm_temp = 0;

    
    gint32 ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail.", __FUNCTION__));

    ret = dfl_get_property_value(obj_handle, RFPROP_PRODUCT_SHARE_INFO, &property_value);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s dfl_get_property_value fail.", __FUNCTION__));

    share_info_value = (const guint8 *)g_variant_get_fixed_array(property_value, &parm_temp, sizeof(guint8));
    guint32 n = (guint32)parm_temp;
    return_do_info_if_fail(NULL != share_info_value, g_variant_unref(property_value); property_value = NULL;
        debug_log(DLOG_ERROR, "%s g_variant_get_fixed_array fail.", __FUNCTION__));

    *oem_property = json_object_new_array();
    return_do_info_if_fail(NULL != *oem_property, g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "%s json_object_new_array fail.", __FUNCTION__));

    for (guint32 i = 0; i < n; i++) {
        property = json_object_new_int(share_info_value[i]);
        if (property == NULL) {
            debug_log(DLOG_ERROR, "%s json_object_new_int fail.", __FUNCTION__);
            json_object_put(*oem_property);
            *oem_property = NULL;
            break;
        }

        ret = json_object_array_add(*oem_property, property);
        if (ret != RET_OK) {
            json_object_put(property);
            json_object_put(*oem_property);
            *oem_property = NULL;
            debug_log(DLOG_ERROR, "%s json_object_array_add fail.", __FUNCTION__);
            break;
        }
    }

    g_variant_unref(property_value);
    return;
}


LOCAL void get_manager_oem_stateless(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    
    gchar manager_id[MAX_RULE_LEN + 1] = {0};
    
    guint8 power_value = 0;
    guint8 enable_value = 0;
    guint8 ctr_power_state = 0;
    gchar manager_ip[IPV4_IP_STR_SIZE + 1] = {0};
    guint32 manager_port = 0;

    
    gint32 ret = dal_get_object_handle_nth(CLASS_STATELESS, 0, &obj_handle);
    
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s dal_get_object_handle_nth fail.", __FUNCTION__));
    
    *oem_property = json_object_new_object();
    return_do_info_if_fail(NULL != *oem_property,
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_STATELESS_SYS_MANAGER_ID, manager_id, sizeof(manager_id));
    if ((VOS_OK == ret) && (strlen(manager_id) > 0)) {
        (void)json_object_object_add(*oem_property, RFPROP_MANAGER_PATCH_STATELESS_SYS_ID,
            json_object_new_string((const gchar *)manager_id));
    } else {
        debug_log(DLOG_MASS, "%s dal_get_property_value_string fail or manager_id is null .", __FUNCTION__);
        (void)json_object_object_add(*oem_property, RFPROP_MANAGER_PATCH_STATELESS_SYS_ID, NULL);
    }

    
    // 查询管理服务器的IP
    (void)dal_get_property_value_string(obj_handle, PROPERTY_STATELESS_SYS_MANAGER_IP, manager_ip, sizeof(manager_ip));

    if (strlen(manager_ip)) {
        json_object_object_add(*oem_property, RFPROP_MANAGER_PATCH_STATELESS_SYS_IP,
            json_object_new_string((const char *)manager_ip));
    } else {
        json_object_object_add(*oem_property, RFPROP_MANAGER_PATCH_STATELESS_SYS_IP, NULL);
    }

    // 查询管理服务器的端口 port
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_STATELESS_SYS_MANAGER_PORT, &manager_port);
    json_object_object_add(*oem_property, RFPROP_MANAGER_PATCH_STATELESS_SYS_PORT,
        json_object_new_int((gint32)manager_port)); // port最大是65535，最小是1
    

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_STATELESS_AUTO_POWER_ON, &power_value);
    if ((VOS_OK == ret) && (1 == power_value || 0 == power_value)) {
        
        // 自主上电的值是0， 受控上电就是1 ，否则受控上电就是0
        do_if_expr(0 == power_value, (ctr_power_state = 1));
        (void)json_object_object_add(*oem_property, RFPROP_STATELESS_POWER_ENABLE,
            json_object_new_boolean(ctr_power_state));
        
    } else {
        
        debug_log(DLOG_DEBUG, "%s dal_get_property_value_byte fail .", __FUNCTION__);
        
        (void)json_object_object_add(*oem_property, RFPROP_STATELESS_POWER_ENABLE, NULL);
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_STATELESS_ENABLE, &enable_value);
    if ((VOS_OK == ret) && (1 == enable_value || 0 == enable_value)) {
        (void)json_object_object_add(*oem_property, RFPROP_STATELESS_ENABLE, json_object_new_boolean(enable_value));
    } else {
        
        debug_log(DLOG_DEBUG, "%s dal_get_property_value_byte fail .", __FUNCTION__);
        
        (void)json_object_object_add(*oem_property, RFPROP_STATELESS_ENABLE, NULL);
    }

    return;
}



void get_manager_oem_system_manager_info(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    gchar manager_id[MAX_PROP_VAL_LEN + 1] = {0};
    gchar manager_name[MAX_PROP_VAL_LEN + 1] = {0};
    gchar manager_ip[MAX_PROP_VAL_LEN + 1] = {0};
    gchar manager_locked_time[MAX_PROP_VAL_LEN + 1] = {0};

    
    gint32 ret = dal_get_object_handle_nth(CLASS_SYSTEM_MANAGER_INFO, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s dal_get_object_handle_nth fail.", __FUNCTION__));
    *oem_property = json_object_new_object();
    return_do_info_if_fail(NULL != *oem_property,
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SYSTEM_MANAGER_INFO_ID, manager_id, sizeof(manager_id));
    if ((VOS_OK == ret) && (strlen(manager_id) > 0)) {
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_ID,
            json_object_new_string((const gchar *)manager_id));
    } else {
        debug_log(DLOG_MASS, "%s dal_get_property_value_string fail or manager_id is null .", __FUNCTION__);
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_ID, NULL);
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SYSTEM_MANAGER_INFO_NAME, manager_name,
        sizeof(manager_name));
    if ((VOS_OK == ret) && (strlen(manager_name) > 0)) {
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_NAME,
            json_object_new_string((const gchar *)manager_name));
    } else {
        debug_log(DLOG_MASS, "%s dal_get_property_value_string fail or manager_name is null .", __FUNCTION__);
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_NAME, NULL);
    }
    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SYSTEM_MANAGER_INFO_IP, manager_ip, sizeof(manager_ip));
    if ((VOS_OK == ret) && (strlen(manager_ip) > 0)) {
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_IP,
            json_object_new_string((const gchar *)manager_ip));
    } else {
        debug_log(DLOG_MASS, "%s dal_get_property_value_string fail or manager_ip is null .", __FUNCTION__);
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_IP, NULL);
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SYSTEM_MANAGER_INFO_LOCKED_TIME, manager_locked_time,
        sizeof(manager_locked_time));
    if ((VOS_OK == ret) && (strlen(manager_locked_time) > 0)) {
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME,
            json_object_new_string((const gchar *)manager_locked_time));
    } else {
        debug_log(DLOG_MASS, "%s dal_get_property_value_string fail or manager_locked_time is null .", __FUNCTION__);
        (void)json_object_object_add(*oem_property, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME, NULL);
    }
    return;
}



LOCAL void get_manager_oem_location(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    
    gchar      location_str[MAX_RULE_LEN + 1] = {0};
    

    gint32 ret = dal_get_object_handle_nth(CLASS_BMC, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail.", __FUNCTION__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_BMC_LOCATION_INFO, location_str, sizeof(location_str));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s dal_get_property_value_string fail.", __FUNCTION__));

    if (strlen(location_str) > 0) {
        *oem_property = json_object_new_string((const gchar *)location_str);
        return_do_info_if_fail(NULL != *oem_property,
            debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));
    } else {
        debug_log(DLOG_MASS, "%s LocationInfo is null.", __FUNCTION__);
    }

    return;
}


LOCAL gint32 get_role_enable(guchar role_id, guchar *role_enable, guchar tmp_value)
{
    if (0 == role_id) {
        *role_enable = 0x1 & tmp_value;
    } else if (1 == role_id) {
        *role_enable = 0x2 & tmp_value;
    } else if (2 == role_id) {
        *role_enable = 0x4 & tmp_value;
    } else {
        
        debug_log(DLOG_DEBUG, "%s role id is invalid.", __FUNCTION__);
        
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL void get_start_end_time(gchar *time_str, gchar *start_time, guint start_len, gchar *end_time, guint end_len)
{
    guint i = 0;

    if (NULL == time_str || 0 == strlen(time_str) || 0 == start_len || 0 == end_len) {
        
        debug_log(DLOG_MASS, "%s, %s input pram is invalid.", __FUNCTION__, start_time);
        
        return;
    }

    while (0 != *time_str && '/' != *time_str) {
        start_time[i] = *time_str;
        time_str++;
        i++;
        if (i >= start_len - 1) {
            return;
        }
    }
    return_if_expr(*time_str == 0);

    time_str++;
    i = 0;

    while ('\0' != *time_str) {
        end_time[i] = *time_str;
        time_str++;
        i++;
        if (i >= end_len - 1) {
            return;
        }
    }

    return;
}


void get_manager_oem_language_set(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    gchar language_set[LANGUAGE_LENGTH + 1] = {0};

    
    return_do_info_if_fail(NULL != oem_property, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    gint32 ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get obj_handle fail, ret is %d.\n", __FUNCTION__, ret));

    
    ret = dal_get_language_set(obj_handle, TRUE, language_set, sizeof(language_set));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get language_set fail, ret is %d.\n", __FUNCTION__, ret));

    
    *oem_property = json_object_new_string((const gchar *)language_set);
    return_do_info_if_fail(NULL != (*oem_property), debug_log(DLOG_ERROR, "%s: new json_obj fail.", __FUNCTION__));
}


void get_manager_oem_permit_rule(json_object **oem_property)
{
    gchar time_str[MAX_PROP_VAL_LEN] = {0};
    gchar start_time[MAX_PROP_VAL_LEN] = {0};
    gchar end_time[MAX_PROP_VAL_LEN] = {0};
    gchar role_str[MAX_RULE_LEN] = {0};
    gchar ip_str[MAX_RULE_LEN] = {0};
    gchar mac_str[MAX_RULE_LEN] = {0};
    guchar role_id = 0;
    guint8 enable_value = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *rule_json = NULL;
    guchar role_enable_value = 0;
    OBJ_HANDLE obj_handle = 0;

    
    gint32 ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth fail.", __FUNCTION__));

    
    ret = dfl_get_object_list(CLASS_PERMIT_RULE, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s dfl_get_object_list fail.", __FUNCTION__));

    *oem_property = json_object_new_array();
    return_do_info_if_fail(*oem_property != NULL, g_slist_free(obj_list); obj_list = NULL;
        debug_log(DLOG_ERROR, "%s json_object_new_array fail.", __FUNCTION__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        rule_json = json_object_new_object();
        return_do_info_if_fail(NULL != rule_json, g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

        
        if (VOS_OK == dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RULE_ID, &role_id)) {
            (void)snprintf_s(role_str, sizeof(role_str), sizeof(role_str) - 1, "Rule%d", role_id + 1);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_ID,
                json_object_new_string((const gchar *)role_str));
        } else {
            debug_log(DLOG_ERROR, "%s dal_get_property_value_byte fail.", __FUNCTION__);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_ID, NULL);
        }

        
        if (VOS_OK == dal_get_property_value_byte(obj_handle, PROPERTY_PERMIT_RULE_IDS, &enable_value)) {
            ret = get_role_enable(role_id, &role_enable_value, enable_value);
            if (VOS_OK == ret) {
                (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_RULE_ENABLED,
                    json_object_new_boolean(role_enable_value));
            } else {
                
                debug_log(DLOG_DEBUG, "%s enable_value is out of range .", __FUNCTION__);
                
                (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_RULE_ENABLED, NULL);
            }
        } else {
            debug_log(DLOG_ERROR, "%s dal_get_property_value_byte fail.", __FUNCTION__);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_RULE_ENABLED, NULL);
        }

        
        if (VOS_OK == dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_TIME_RULE_INFO, time_str,
            sizeof(time_str))) {
            (void)get_start_end_time(time_str, start_time, sizeof(start_time), end_time, sizeof(end_time));
            

            // 判断开始时间的值
            if (0 == g_strcmp0("", start_time)) {
                (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_START_TIME, NULL);
            } else {
                (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_START_TIME,
                    json_object_new_string((const gchar *)start_time));
            }

            // 判断结束时间的值
            if (0 == g_strcmp0("", end_time)) {
                (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_END_TIME, NULL);
            } else {
                (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_END_TIME,
                    json_object_new_string((const gchar *)end_time));
            }

            

            
            (void)memset_s(time_str, sizeof(time_str) - 1, 0, sizeof(time_str) - 1);
            (void)memset_s(start_time, sizeof(start_time) - 1, 0, sizeof(start_time) - 1);
            (void)memset_s(end_time, sizeof(end_time) - 1, 0, sizeof(end_time) - 1);
        } else {
            debug_log(DLOG_ERROR, "%s dal_get_property_value_string fail.", __FUNCTION__);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_START_TIME, NULL);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_END_TIME, NULL);
        }

        
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_IP_RULE_INFO, ip_str, sizeof(ip_str));
        if ((VOS_OK == ret) && strlen(ip_str) > 0) {
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_IP,
                json_object_new_string((const gchar *)ip_str));
            
            (void)memset_s(ip_str, sizeof(ip_str) - 1, 0, sizeof(ip_str) - 1);
        } else {
            debug_log(DLOG_MASS, "%s dal_get_property_value_string fail.", __FUNCTION__);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_IP, NULL);
        }

        
        ret =
            dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MAC_RULE_INFO, mac_str, sizeof(mac_str));
        if ((VOS_OK == ret) && strlen(mac_str) > 0) {
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_MAC,
                json_object_new_string((const gchar *)mac_str));

            
            (void)memset_s(mac_str, sizeof(mac_str) - 1, 0, sizeof(mac_str) - 1);
        } else {
            debug_log(DLOG_MASS, "%s dal_get_property_value_string fail.", __FUNCTION__);
            (void)json_object_object_add(rule_json, RFPROP_MANAGER_PATCH_LOGIN_MAC, NULL);
        }

        ret = json_object_array_add(*oem_property, rule_json);
        do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s json_object_array_add fail.", __FUNCTION__);
            json_object_put(rule_json); rule_json = NULL);
    }

    g_slist_free(obj_list);
    return;
}


LOCAL void get_manager_oem_uptime(json_object **oem_property)
{
    gint32 ret;
    gint32 time_value = 0;
    gint32 time_tmp_day;
    gint32 time_tmp_hour;
    gint32 time_tmp_min;
    gint32 time_tmp_sec;
    gchar *tmp = NULL;
    FILE *fp = NULL;
    gchar *bmc_uptime = NULL;
    gchar *pstr = NULL;
    gchar tempbuf[BMC_UP_TIME_FILE_LENGTH] = {0};
    gchar time_str[BMC_UP_TIME_FILE_LENGTH] = {0};
    gchar split_time_str[BMC_UP_TIME_FILE_LENGTH] = {0};
    errno_t str_ret;

    // 打开配置文件，获取指向文件内容指针
    fp = fopen(BMC_UP_TIME_FILE, "r");
    
    return_do_info_if_fail(NULL != fp, debug_log(DLOG_DEBUG, "%s open file fail", __FUNCTION__));
    

    tmp = vos_fgets(time_str, BMC_UP_TIME_FILE_LENGTH, fp);
    return_do_info_if_fail(NULL != tmp, fclose(fp); debug_log(DLOG_ERROR, "%s get file message fail.", __FUNCTION__));
    (void)fclose(fp);
    tmp = NULL;

    // 拆分字符串，取出整数部分
    tmp = strtok_s(time_str, " ", &pstr);
    
    return_do_info_if_fail(NULL != tmp, debug_log(DLOG_DEBUG, "%s split str fail.", __FUNCTION__));
    pstr = NULL;

    bmc_uptime = strtok_s(tmp, ".", &pstr);
    return_do_info_if_fail(NULL != bmc_uptime, debug_log(DLOG_DEBUG, "%s split str next fail", __FUNCTION__));
    
    tmp = NULL;

    str_ret = strncpy_s(split_time_str, sizeof(split_time_str), bmc_uptime, sizeof(split_time_str) - 1);
    return_do_info_if_fail(EOK == str_ret, debug_log(DLOG_ERROR, "%s strncpy_s fail.", __FUNCTION__));

    // 字符串转换成整数进行时间换算
    ret = vos_str2int(split_time_str, 10, &time_value, NUM_TPYE_INT32);
    
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s:  vos_str2int fail.", __FUNCTION__));
    

    time_tmp_day = (time_value) / (60 * 60 * 24);
    time_tmp_hour = ((time_value) / (60 * 60)) % (24);
    time_tmp_min = ((time_value) / 60) % (60);
    time_tmp_sec = (time_value) % (60);

    ret = snprintf_s(tempbuf, sizeof(tempbuf), sizeof(tempbuf) - 1, "%d days %.2dh:%.2dm:%.2ds", time_tmp_day,
        time_tmp_hour, time_tmp_min, time_tmp_sec);
    return_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "%s snprintf_s fail.", __FUNCTION__));

    *oem_property = json_object_new_string((const gchar *)tempbuf);
    return_do_info_if_fail(NULL != *oem_property,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}

LOCAL void get_manage_oem_string(const gchar *class_name, const gchar *property_name, json_object **oem_property)
{
    gint32 ret;
    gchar prop_value[MAX_OEM_PROP_VAL_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(class_name, 0, &obj_handle);
    
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_DEBUG, "%s: get %s handle failed.\n", __FUNCTION__, class_name));
    

    ret = dal_get_property_value_string(obj_handle, property_name, prop_value, sizeof(prop_value));
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s: get %s's prop_value failed.\n", __FUNCTION__, property_name));

    *oem_property = json_object_new_string((const gchar *)prop_value);
    return_do_info_if_fail(NULL != *oem_property,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}


LOCAL void get_manage_oem_flash_state(const gchar *class_name, const gchar *property_name, json_object **oem_property)
{
    gint32 ret;
    guchar prop_value = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(class_name, 0, &obj_handle);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s: get %s handle failed.\n", __FUNCTION__, class_name));

    ret = dal_get_property_value_byte(obj_handle, property_name, &prop_value);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s: get %s's prop_value failed.\n", __FUNCTION__, property_name));
    if (prop_value) {
        *oem_property = json_object_new_string("ReadOnly");
    } else {
        *oem_property = json_object_new_string("ReadWrite");
    }
    return_do_info_if_fail(*oem_property != NULL,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}


LOCAL void get_manage_oem_platform(json_object *huawei)
{
    gint32 ret;
    guchar prop_value = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *oem_platform = NULL;

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    do_val_if_expr(VOS_OK != ret, goto exit;
        debug_log(DLOG_ERROR, "%s: get %s handle failed.\n", __FUNCTION__, CLASS_NAME_PME_SERVICECONFIG));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_ARM, &prop_value);
    do_val_if_expr(VOS_OK != ret, goto exit;
        debug_log(DLOG_ERROR, "%s: get %s's prop_value failed.\n", __FUNCTION__, PROTERY_PME_SERVICECONFIG_ARM));
    if (prop_value == ARM_ENABLE) {
        oem_platform = json_object_new_string("Arm");
    } else if (prop_value == ARM_DISABLE) {
        oem_platform = json_object_new_string("X86");
    }
    do_val_if_expr(NULL == oem_platform, debug_log(DLOG_ERROR, "%s here json_object_new_string fail.", __FUNCTION__));
exit:
    json_object_object_add(huawei, RFPROP_PLATFORM_NAME, oem_platform);
    return;
}


LOCAL gint32 fusion_partition_supported(void)
{
    gint32 ret_val;
    guint8 node_mode = G_MAXINT8; // 当前是主BMC还是从BMC
    OBJ_HANDLE obj_handle = 0;

    ret_val = dfl_get_object_handle(XMLPARTITION_OBJ_NAME, &obj_handle);
    return_val_if_fail(DFL_OK == ret_val, VOS_ERR); // 获取失败认为不支持硬分区查询

    (void)dal_get_property_value_byte(obj_handle, NODE_MODE, &node_mode);
    return_val_if_fail(NODE_MASTER == node_mode, VOS_ERR); // 不是主BMC认为不支持硬分区查询

    return VOS_OK;
}


LOCAL void get_fusion_partition(json_object *huawei)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    guint8 xml_partition = G_MAXUINT8;
    const gchar *xml_partition_str = NULL;
    json_object *xml_partition_jso = NULL;

    return_if_expr(NULL == huawei);

    ret_val = fusion_partition_supported();
    if (VOS_OK != ret_val) {
        goto exit;
    }

    ret_val = dfl_get_object_handle(XMLPARTITION_OBJ_NAME, &obj_handle);
    if (VOS_OK != ret_val) {
        goto exit;
    }

    (void)dal_get_property_value_byte(obj_handle, XML_PARTITION, &xml_partition);
    if (PARTITION_4P != xml_partition && PARTITION_8P != xml_partition) { // 不是4P或者8P认为不支持查询硬分区
        goto exit;
    }

    if (PARTITION_4P == xml_partition) {
        xml_partition_str = RFPROP_VAL_FP_DUAL_SYSTEM;
    } else {
        xml_partition_str = RFPROP_VAL_FP_SINGLE_SYSTEM;
    }
    xml_partition_jso = json_object_new_string(xml_partition_str);

exit:
    json_object_object_add(huawei, RFPROP_MANAGER_FUSION_PARTITION, xml_partition_jso);
}


LOCAL gint32 bmc_is_4p(void)
{
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    guint8 xml_partition = G_MAXUINT8;

    ret_val = dfl_get_object_handle(XMLPARTITION_OBJ_NAME, &obj_handle);
    return_val_if_fail(DFL_OK == ret_val, VOS_ERR);

    (void)dal_get_property_value_byte(obj_handle, XML_PARTITION, &xml_partition);
    return_val_if_fail(PARTITION_4P == xml_partition, VOS_ERR);

    return VOS_OK;
}


LOCAL void get_remote_bmc_ip(json_object *huawei)
{
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    gchar ip_address[PROP_VAL_LENGTH] = {0};
    json_object *ip_jso = NULL;

    return_if_expr(NULL == huawei);

    ret_val = bmc_is_4p();
    if (VOS_OK != ret_val) {
        goto exit;
    }

    ret_val = dfl_get_object_handle(PARTITION_OBJECT_NAME, &obj_handle);
    if (DFL_OK != ret_val) {
        goto exit;
    }

    (void)dal_get_property_value_string(obj_handle, REMOTE_EXTERN_IP, ip_address, sizeof(ip_address));
    if (VOS_OK != check_string_val_effective((const gchar *)ip_address)) {
        goto exit;
    }

    ip_jso = json_object_new_string((const gchar *)ip_address);

exit:
    json_object_object_add(huawei, RFPROP_MANAGER_REMOTE_IPV4, ip_jso);
}


LOCAL void get_vga_enabled(json_object *huawei)
{
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    guint8 vga_switch = G_MAXUINT8;
    json_object *vga_jso = NULL;

    return_if_expr(NULL == huawei);

    ret_val = bmc_is_4p();
    if (VOS_OK != ret_val) {
        goto exit;
    }

    ret_val = dfl_get_object_handle(PARTITION_OBJECT_NAME, &obj_handle);
    if (DFL_OK != ret_val) {
        goto exit;
    }

    (void)dal_get_property_value_byte(obj_handle, CURRENT_VGA_SWITCH, &vga_switch);
    if (RF_ENABLE != vga_switch && RF_DISABLE != vga_switch) {
        goto exit;
    }

    vga_jso = json_object_new_boolean(vga_switch);

exit:

    json_object_object_add(huawei, RFPROP_MANAGER_VGA_ENABLED, vga_jso);
}


LOCAL void get_nic_link(json_object *huawei)
{
    gint32 ret_val;
    json_object *obj_jso = NULL;

    ret_val = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_NICS, &obj_jso);
    if (ret_val == VOS_OK) {
        json_object_object_add(huawei, RF_RESOURCE_ETHERNETINTERFACES, obj_jso);
    }

    return;
}


LOCAL gint32 get_manager_oem_snmp(json_object *huawei_jso, guchar boardtype, guchar x86_enbale)
{
    gint32 ret;
    guchar snmp_state = 0;
    json_object *tmp_jso = NULL;

    return_val_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    // Snmp资源
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &snmp_state);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: get SnmpdEnable state fail.\n", __FUNCTION__, __LINE__));

    
    if (SERVICE_NETCONFIG_ENABLE == snmp_state &&
        (DISABLE != x86_enbale || BOARD_RM == boardtype || BOARD_SWITCH == boardtype)) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SNMP, &tmp_jso);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_SNMP, tmp_jso);
    } else {
        json_object_object_add(huawei_jso, RFPROP_MANAGER_SNMP, NULL);
    }

    return VOS_OK;
}


LOCAL gint32 get_manager_oem_usbmgmt(json_object *huawei_jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;

    if (huawei_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = is_support_usbmgmt_service();
    if (ret != RET_OK) {
        return RET_OK;
    }

    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_USBMGMT, &tmp_jso);
    if (ret != RET_OK) {
        (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(huawei_jso, RFPROP_MANAGER_USBMGMT, tmp_jso);

    return RET_OK;
}


LOCAL void get_usbmgmt_link(json_object *huawei_jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;

    if (huawei_jso == NULL) {
        return;
    }

    ret = is_support_usbmgmt_service();
    if (ret != RET_OK) {
        return;
    }
    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_USBMGMT, &tmp_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get_odata_id_object fail.\n", __FUNCTION__);
        return;
    }

    json_object_object_add(huawei_jso, RFPROP_MANAGER_USBMGMT, tmp_jso);

    return;
}


LOCAL gint32 get_manager_oem_lldp(json_object *huawei_jso)
{
    gint32 ret;
    guchar lldp_state = 0;
    json_object *tmp_jso = NULL;

    return_val_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_LLDP, &lldp_state);
    return_val_do_info_if_expr((ret == VOS_ERR), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: get LLDPSupport state fail.\n", __FUNCTION__, __LINE__));

    if (lldp_state == SERVICE_NETCONFIG_ENABLE) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_LLDP, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_LLDP, tmp_jso);
    }

    return VOS_OK;
}

LOCAL gint32 get_manager_oem_energy_saving(json_object *huawei_jso)
{
    gint32 ret;
    guchar energy_saving_support = 0;
    json_object *tmp_jso = NULL;

    return_val_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        &energy_saving_support);
    return_val_do_info_if_expr((ret == VOS_ERR), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: get %s state fail.\n", __FUNCTION__, __LINE__,
        PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT));

    if (energy_saving_support == SERVICE_NETCONFIG_ENABLE) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_ENERGY_SAVING, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_ENERGY_SAVING, tmp_jso);
    } else {
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__);
    }

    return VOS_OK;
}

LOCAL gint32 get_manager_oem_kvm(json_object *huawei_jso)
{
    gint32 ret;
    guchar kvm_enabled = 0;
    json_object *tmp_jso = NULL;

    return_val_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    return_val_do_info_if_expr((ret != VOS_OK), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_get_func_ability fail.\n", __FUNCTION__, __LINE__));

    if (kvm_enabled == SERVICE_NETCONFIG_ENABLE) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_KVM, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_KVM, tmp_jso);
    } else {
        json_object_object_add(huawei_jso, RFPROP_MANAGER_KVM, NULL);
    }
    return VOS_OK;
}


LOCAL gint32 get_manager_oem_x86_resource(json_object *huawei_jso, guchar x86_enable)
{
    gint32 ret = 0;
    json_object *tmp_jso = NULL;

    return_val_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    if (DISABLE != x86_enable) {
        // Diagnostic资源
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_DIAGNOSTIC, &tmp_jso);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_DIAGNOSTIC, tmp_jso);
        tmp_jso = NULL;

        
        // SMS资源
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SMS, &tmp_jso);
        return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_SMS, tmp_jso);
        
    } else {
        json_object_object_add(huawei_jso, RFPROP_MANAGER_DIAGNOSTIC, NULL);
        json_object_object_add(huawei_jso, RFPROP_MANAGER_SMS, NULL);
    }

    return VOS_OK;
}


gint32 get_manager_defaultip_group(OBJ_HANDLE *obj_handle)
{
    gint32 ret = 0;

    return_val_do_info_if_fail((obj_handle != NULL), ret, debug_log(DLOG_DEBUG, "%s: NULL Pointer.", __FUNCTION__));

    ret = dal_eth_get_out_type_object(OUT_ETHERNET, obj_handle);
    return_val_do_info_if_expr((ret != VOS_OK), ret,
        debug_log(DLOG_DEBUG, "%s: obj_handle  fail, ret:%d .", __FUNCTION__, ret));
    return VOS_OK;
}


LOCAL void get_manager_oemresource_default_ipaddr_mode(json_object *property_object)
{
    gint32 ret;
    gchar  ipv4_addr[MAX_RSC_ID_LEN] = {0};
    guint8 ip_mode = INVALID_IP_MODE;
    OBJ_HANDLE obj_handle = 0;

    return_do_info_if_fail((NULL != property_object), debug_log(DLOG_DEBUG, "%s:  NULL Pointer.", __FUNCTION__));

    
    
    ret = get_manager_defaultip_group(&obj_handle);
    return_do_info_if_fail((ret == VOS_OK), debug_log(DLOG_DEBUG, "%s: obj_handle  fail .", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_FACTORY_D_IP_MODE, &ip_mode);
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_FACTORY_D_IP_ADDR, ipv4_addr, MAX_RSC_ID_LEN);
    if (ip_mode == INVALID_IP_MODE) {
        
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_D_IP_MODE, &ip_mode);
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_D_IP_ADDR, ipv4_addr, MAX_RSC_ID_LEN);
    }

    if ((ip_mode == RFPROP_MANAGER_MANAGER_IPV4_MODE0) || (ip_mode == RFPROP_MANAGER_MANAGER_IPV4_MODE1)) {
        json_object_object_add(property_object, RFPROP_MANAGER_MANAGER_IPV4_MODE,
            json_object_new_string(RFPROP_MANAGER_MANAGER_MODESTATIC));
        json_object_object_add(property_object, RFPROP_MANAGER_MANAGER_IPV4_ADDRESS,
            json_object_new_string((const gchar *)ipv4_addr));
    } else if (ip_mode == RFPROP_MANAGER_MANAGER_IPV4_MODE2) {
        json_object_object_add(property_object, RFPROP_MANAGER_MANAGER_IPV4_MODE,
            json_object_new_string(RFPROP_MANAGER_MANAGER_MODEDHCP));
        json_object_object_add(property_object, RFPROP_MANAGER_MANAGER_IPV4_ADDRESS, NULL);
    } else {
        json_object_object_add(property_object, RFPROP_MANAGER_MANAGER_IPV4_MODE, NULL);
        json_object_object_add(property_object, RFPROP_MANAGER_MANAGER_IPV4_ADDRESS, NULL);
        debug_log(DLOG_DEBUG, "Mode is %d wrong .", ip_mode);
    }

    return;
}


LOCAL void get_manager_oem_timezone_configurable(json_object **property_object)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar time_zone_flag = 0;

    return_do_info_if_fail(NULL != property_object,
        debug_log(DLOG_ERROR, "%s, %d:  null pointer.\n", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret, *property_object = NULL;
        debug_log(DLOG_ERROR, "%s, %d:  get PRODUCT object fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(obj_handle, BMC_TIMESRC_NAME, &time_zone_flag);
    return_do_info_if_fail(VOS_OK == ret, *property_object = NULL;
        debug_log(DLOG_ERROR, "%s, %d:  get TimeSrc property fail.\n", __FUNCTION__, __LINE__));

    if (TIME_SRC_SMM == time_zone_flag) {
        *property_object = json_object_new_boolean(FALSE);
    } else {
        *property_object = json_object_new_boolean(TRUE);
    }

    return;
}

LOCAL void get_manage_deviceip_string(OBJ_HANDLE obj_handle, const gchar *property_name, json_object **oem_property)
{
    gint32 ret;
    gchar prop_value[MAX_OEM_PROP_VAL_LEN + 1] = {0};

    ret = dal_get_property_value_string(obj_handle, property_name, prop_value, sizeof(prop_value));
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s: get %s's prop_value failed.\n", __FUNCTION__, property_name));

    *oem_property = json_object_new_string((const gchar *)prop_value);
    return_do_info_if_fail(NULL != *oem_property,
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    return;
}


LOCAL void get_manager_deviceip(json_object *huawei_jso)
{
    json_object *oem_property = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = get_manager_defaultip_group(&obj_handle);
    return_do_info_if_fail((ret == VOS_OK), debug_log(DLOG_DEBUG, "%s: obj_handle  fail .", __FUNCTION__));
    
    get_manage_deviceip_string(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, &oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_MANAGER_MANAGER_IP_ADDR, oem_property);
    oem_property = NULL;

    
    get_manage_deviceip_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, &oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_MANAGER_MANAGER_IPV6_ADDR, oem_property);
    oem_property = NULL;

    return;
}


LOCAL void get_dst_enable(json_object **oem_property)
{
    guint8 dst_enable = ENABLE;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    
    ret = dal_get_object_handle_nth(OBJ_NAME_BMC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth failed", __FUNCTION__);
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_BMC_DST_ENABLE, &dst_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed", __FUNCTION__);
        return;
    }

    if ((dst_enable != ENABLE) && (dst_enable != DISABLE)) {
        debug_log(DLOG_ERROR, "%s: invalid DST enabled is %d", __FUNCTION__, dst_enable);
        return;
    }

    *oem_property = json_object_new_boolean((json_bool)dst_enable);

    return;
}

LOCAL void get_manager_oem_time_resource(json_object *huawei_jso)
{
    json_object *oem_property = NULL;
    
    
    get_manager_oem_timezone_configurable(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_MANAGER_OEM_TIMEZONE_CONFIGURABLE, oem_property);
    oem_property = NULL;

    
    (void)get_manager_oem_uptime(&oem_property);
    (void)json_object_object_add(huawei_jso, BMC_UP_TIME_VALUE, oem_property);
    oem_property = NULL;

    
    get_dst_enable(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_MANAGER_DST_ENABLE, oem_property);
    oem_property = NULL;

    return;
}


LOCAL gint32 add_manager_oemresource(json_object *huawei_jso)
{
    json_object *oem_property = NULL;
    json_object *oem_property_object = NULL;
    gint32 ret;
    guchar boardtype = 0;
    json_object *tmp_jso = NULL;

    // 判断是否后插板, 使用获取单板类型接口，而不根据是不是x86来判断
    if (redfish_get_board_type(&boardtype) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_type failed", __FUNCTION__);
        (void)json_object_put(huawei_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    (void)get_manager_oem_language_set(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_MANAGER_LANGUAGE_SET, oem_property);
    oem_property = NULL;

    
    (void)get_manage_oem_string(BMC_CLASEE_NAME, BMC_HOST_NAME, &oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_SERVICEROOT_HOST_NAME, oem_property);
    oem_property = NULL;

    
    oem_property_object = json_object_new_object();
    get_manager_oemresource_default_ipaddr_mode(oem_property_object);
    (void)json_object_object_add(huawei_jso, RFPROP_MANAGER_MANAGER_IPV4_DEFAULT, oem_property_object);

    
    get_manager_deviceip(huawei_jso);

    
    (void)get_manage_oem_string(CLASS_NAME_DNSSETTING, PROPERTY_DNSSETTING_DOMAINNAME, &oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_SERVICEROOT_DOMAIN_NAME, oem_property);
    oem_property = NULL;

    (void)get_manager_oem_time_resource(huawei_jso);

    
    (void)get_manager_oem_product_id(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_PRODUCT_UNIQUE_ID, oem_property);
    oem_property = NULL;

    
    (void)json_object_object_add(huawei_jso, RFPOP_PRODUCT_TYPE, json_object_new_string(IBMC_VERSION));

    // 获取SP状态, 后插板及iRM均不支持SP
    if ((boardtype != BOARD_SWITCH) && (boardtype != BOARD_RM)) {
        (void)get_sp_status(huawei_jso);
    }

    
    (void)get_manager_oem_share_info(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_PRODUCT_SHARE_INFO, oem_property);
    oem_property = NULL;

    
    (void)get_manager_oem_location(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_PRODUCT_LOCATION_INFO, oem_property);
    oem_property = NULL;

    
    (void)get_manager_oem_stateless(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_STATELESS_STATE, oem_property);
    oem_property = NULL;

    
    
    (void)get_manager_oem_system_manager_info(&oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_SYSTEM_MANAGER_INFO, oem_property);
    oem_property = NULL;

    
    (void)get_manager_oem_permit_rule(&oem_property);
    (void)json_object_object_add(huawei_jso, "LoginRule", oem_property);

    
    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SECURITY_SVC, &tmp_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_odata_id_object failed", __FUNCTION__);
        (void)json_object_put(huawei_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(huawei_jso, RFPROP_MANAGER_SECURITY_SVC, tmp_jso);
    tmp_jso = NULL;

    return VOS_OK;
}


LOCAL void get_fdm_link(json_object *huawei_jso)
{
    json_object *tmp_jso = NULL;
    gint32 ret = 0;

    
    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_FDMSERVICE, &tmp_jso);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_INFO, "%s, %d: get_odata_id_object fdm fail.\n", __FUNCTION__, __LINE__));

    json_object_object_add(huawei_jso, RFPROP_MANAGER_FDMSERVICE, tmp_jso);
    

    return;
}
LOCAL gint32 check_fpc_enabled(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 fpc_enable = 0;

    
    ret = dal_get_object_handle_nth(CLASS_FDM_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get FdmConfig handle failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDM_MSP_ENABLE, &fpc_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get fpc enable prop failed.", __FUNCTION__);
        return RET_ERR;
    }
    if (fpc_enable != ENABLE) {
        debug_log(DLOG_INFO, "%s: FPC is not enable.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}
LOCAL void get_manager_oem_fpc(json_object* huawei_jso)
{
    gint32 ret;
    json_object* tmp_jso = NULL;

    if (huawei_jso == NULL) {
        return;
    }
    if (check_fpc_enabled() != RET_OK) {
        debug_log(DLOG_INFO, "%s: fpc is not support.", __FUNCTION__);
        return;
    }
    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_FPCSERVICE, &tmp_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_odata_id_object fail.", __FUNCTION__);
        return;
    }
    json_object_object_add(huawei_jso, RFPROP_MANAGER_FPCSERVICE, tmp_jso);

    return;
}

LOCAL gint32 get_manager_oemresource(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar boardtype = 0;
    json_object *huawei_jso = NULL;
    json_object *tmp_jso = NULL;
    json_object *oem_property = NULL;
    guchar vnc_enabled = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar x86_enable = 0;

    return_val_do_info_if_fail((o_message_jso != NULL) && (o_result_jso != NULL) && (i_paras != NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    huawei_jso = json_object_new_object();
    return_val_do_info_if_expr(huawei_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));
    // 判断是否后插板, 使用获取单板类型接口，而不根据是不是x86来判断
    ret = redfish_get_board_type(&boardtype);
    // Security资源
    
    // 判断是否x86
    ret = redfish_get_x86_enable_type(&x86_enable);
    
    return_val_do_info_if_expr((ret != VOS_OK), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: redfish_get_x86_enable_type fail.\n", __FUNCTION__, __LINE__));

    ret = dal_rf_get_board_type(&boardtype);
    return_val_do_info_if_expr((ret != VOS_OK), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_rf_get_board_type fail.\n", __FUNCTION__, __LINE__));

    ret = add_manager_oemresource(huawei_jso);
    return_val_do_info_if_expr((ret != VOS_OK), ret,
        debug_log(DLOG_ERROR, "%s, %d: get host name or ip or other basic message fail.\n", __FUNCTION__, __LINE__));

    ret = get_manager_oem_snmp(huawei_jso, boardtype, x86_enable);
    return_val_do_info_if_expr((ret != VOS_OK), ret,
        debug_log(DLOG_ERROR, "%s, %d: get snmp resource message fail.\n", __FUNCTION__, __LINE__));

    ret = get_manager_oem_lldp(huawei_jso);
    return_val_do_info_if_expr((ret != VOS_OK), ret,
        debug_log(DLOG_ERROR, "%s, %d: get lldp resource message fail.\n", __FUNCTION__, __LINE__));

    ret = get_manager_oem_usbmgmt(huawei_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get usbmgmt resource message fail.\n", __FUNCTION__, __LINE__);
        return ret;
    }

    ret = get_manager_oem_energy_saving(huawei_jso);
    return_val_do_info_if_expr((ret != VOS_OK), ret,
        debug_log(DLOG_ERROR, "%s, %d: get energySaving resource message fail.\n", __FUNCTION__, __LINE__));

    // x86使能或者是iRM则开启SMTP syslog kvm服务
    if (x86_enable != DISABLE || boardtype == BOARD_RM) {
        // Smtp资源
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SMTP, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_SMTP, tmp_jso);
        tmp_jso = NULL;

        // Syslog资源
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SYSLOG, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_SYSLOG, tmp_jso);
        tmp_jso = NULL;

        
        ret = get_manager_oem_kvm(huawei_jso);
        return_val_do_info_if_expr((ret != VOS_OK), ret,
            debug_log(DLOG_ERROR, "%s, %d: get kvm resource message fail.\n", __FUNCTION__, __LINE__));
    } else {
        json_object_object_add(huawei_jso, RFPROP_MANAGER_SMTP, NULL);
        json_object_object_add(huawei_jso, RFPROP_MANAGER_SYSLOG, NULL);
        json_object_object_add(huawei_jso, RFPROP_MANAGER_KVM, NULL);
    }

    ret = get_manager_oem_x86_resource(huawei_jso, x86_enable);
    return_val_if_expr(ret != VOS_OK, ret);

    // Ntp资源
    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_NTP, &tmp_jso);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

    json_object_object_add(huawei_jso, RFPROP_MANAGER_NTP, tmp_jso);

    tmp_jso = NULL;

    // Vnc资源
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_VNC, &vnc_enabled);
    return_val_do_info_if_expr((ret == VOS_ERR), HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: dal_get_func_ability fail.\n", __FUNCTION__, __LINE__));

    if (vnc_enabled == SERVICE_NETCONFIG_ENABLE) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_VNC, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_VNC, tmp_jso);
    } else {
        json_object_object_add(huawei_jso, RFPROP_MANAGER_VNC, NULL);
    }

    
    // SMS资源, 依赖iBMA(有x86), 后插板及iRM均不支持
    if ((boardtype != BOARD_SWITCH) && (boardtype != BOARD_RM)) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SMS, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_SMS, tmp_jso);
    } else {
        json_object_object_add(huawei_jso, RFPROP_MANAGER_SMS, NULL);
    }
    
    

    
    if (rf_support_sp_service() == TRUE) {
        tmp_jso = NULL;
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_SP, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_SP, tmp_jso);
    }
    

    
    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    if (ret == VOS_OK) {
        tmp_jso = NULL;
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_LICENSE_SERVICE, &tmp_jso);
        return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
            debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));

        json_object_object_add(huawei_jso, RFPROP_MANAGER_LICENSE_SERVICE, tmp_jso);
    }

    // fdmservice 资源
    get_fdm_link(huawei_jso);
    get_manager_oem_fpc(huawei_jso);

    
    get_fusion_partition(huawei_jso);

    get_remote_bmc_ip(huawei_jso);

    get_vga_enabled(huawei_jso);
    
    
    (void)get_manage_oem_flash_state(CLASS_NAND_FLASH, PROPERTY_NAND_FLASH_WP, &oem_property);
    (void)json_object_object_add(huawei_jso, RFPROP_PLASH_PROTECT_NAME, oem_property);
    
    get_manage_oem_platform(huawei_jso);

    get_nic_link(huawei_jso);

    // object add to Huawei
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(huawei_jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, RFPROP_FWINV_HUAWEI, huawei_jso);

    return HTTP_OK;
}

LOCAL const gchar manager_allowable_values[] = "{\"ResetType\": [\"ForceRestart\"]}";


LOCAL gint32 act_manager_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    struct json_object *jso_allowable_values = NULL;
    const gchar *reset_type_str = NULL;

    debug_log(DLOG_DEBUG, "[%s]: act_manager_reset", __FUNCTION__);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    debug_log(DLOG_DEBUG, "[%s]: param check ok", __FUNCTION__);

    jso_allowable_values = json_tokener_parse(manager_allowable_values);

    ret = redfish_action_format_check(RFPROP_MANAGER_RESET, jso_allowable_values, i_paras->val_jso, o_message_jso);

    json_object_put(jso_allowable_values);

    return_val_if_fail((HTTP_OK == ret), ret);

    debug_log(DLOG_DEBUG, "[%s]: %s", __FUNCTION__, ">>>>>>>>>>>>>>>Manager rebootting...");
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_BMC, METHD_BMC_REBOOTPME, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    // 成功
    return_val_do_info_if_expr((VOS_OK == ret), HTTP_OK, (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso));
    // 无权限
    return_val_do_info_if_expr((RFERR_INSUFFICIENT_PRIVILEGE == ret), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    debug_log(DLOG_ERROR, "reset failed, error code:%#x", ret);
    (void)get_element_str(i_paras->val_jso, RFACTION_PARAM_RESET_TYPE, &reset_type_str);
    (void)create_message_info(MSGID_RESET_NOT_ALLOWED, NULL, o_message_jso, reset_type_str);

    return HTTP_BAD_REQUEST;
    
}

LOCAL gint32 get_manager_serialconsole(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 rmcpplus_state = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_json_arr = NULL;
    json_object *obj_json_str_state = NULL;
    json_object *obj_json_str_IPMI = NULL;
    json_object *obj_json_str_int = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object failed.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_handle(RMCP_OBJ_NAME, &obj_handle);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_handle failed.", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LAN_PLUS_STATE, &rmcpplus_state);
    
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get rmcpplus_state failed.", __FUNCTION__, __LINE__);
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_SERVICEENABLED, NULL);
    } else if (RFPROP_MANAGER_STATE_FALSE == rmcpplus_state) {
        obj_json_str_state = json_object_new_boolean(FALSE);
        do_val_if_fail(NULL != obj_json_str_state,
            debug_log(DLOG_ERROR, "%s, %d: rmcpplus_state json_object_new_boolean failed.", __FUNCTION__, __LINE__));
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_SERVICEENABLED, obj_json_str_state);
    } else {
        obj_json_str_state = json_object_new_boolean(TRUE);
        do_val_if_fail(NULL != obj_json_str_state,
            debug_log(DLOG_ERROR, "%s, %d: rmcpplus_state json_object_new_boolean failed.", __FUNCTION__, __LINE__));
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_SERVICEENABLED, obj_json_str_state);
    }

    
    obj_json_arr = json_object_new_array();
    if (NULL == obj_json_arr) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array failed.", __FUNCTION__, __LINE__);
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_CONNECTTYPESSUPPORTED, NULL);
    } else {
        obj_json_str_IPMI = json_object_new_string(RFPROP_MANAGER_IPMI);
        if (NULL == obj_json_str_IPMI) {
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string failed.", __FUNCTION__, __LINE__);
        } else {
            ret = json_object_array_add(obj_json_arr, obj_json_str_IPMI);
            do_val_if_fail(0 == ret, json_object_put(obj_json_str_IPMI);
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add failed.", __FUNCTION__, __LINE__));
        }

        json_object_object_add(*o_result_jso, RFPROP_MANAGER_CONNECTTYPESSUPPORTED, obj_json_arr);
    }

    
    obj_json_str_int = json_object_new_int(RFPROP_MANAGER_IPMI_MAXCONCURRENTSESSIONS);
    do_val_if_fail(NULL != obj_json_str_int,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int failed.", __FUNCTION__, __LINE__));
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_MAXCONCURRENTSESSIONS, obj_json_str_int);

    return HTTP_OK;
}

LOCAL gint32 get_manager_graphicalconsole(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 kvm_state = 0;
    guint8 board_type = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_json_arr = NULL;
    json_object *obj_json_str_state = NULL;
    json_object *obj_json_str_KVMIP = NULL;
    json_object *obj_json_str_int = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    // 判断是否后插板
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    
    return_val_if_expr(DISABLE == board_type, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object failed.", __FUNCTION__, __LINE__));

    
    
    ret = dfl_get_object_handle(PROXY_KVM_OBJ_NAME, &obj_handle);
    
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_handle failed.", __FUNCTION__, __LINE__));
    
    ret = dal_get_property_value_byte(obj_handle, KVM_PROPERTY_STATE, &kvm_state);
    
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: get kvm_state failed.", __FUNCTION__, __LINE__);
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_SERVICEENABLED, NULL);
    } else if (RFPROP_MANAGER_STATE_FALSE == kvm_state) {
        obj_json_str_state = json_object_new_boolean(FALSE);
        do_val_if_fail(NULL != obj_json_str_state,
            debug_log(DLOG_ERROR, "%s, %d: kvm_state  json_object_new_boolean failed.", __FUNCTION__, __LINE__));
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_SERVICEENABLED, obj_json_str_state);
    } else {
        obj_json_str_state = json_object_new_boolean(TRUE);
        do_val_if_fail(NULL != obj_json_str_state,
            debug_log(DLOG_ERROR, "%s, %d:kvm_state json_object_new_boolean failed.", __FUNCTION__, __LINE__));
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_SERVICEENABLED, obj_json_str_state);
    }

    
    obj_json_arr = json_object_new_array();
    if (NULL == obj_json_arr) {
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array failed.", __FUNCTION__, __LINE__);
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_CONNECTTYPESSUPPORTED, NULL);
    } else {
        obj_json_str_KVMIP = json_object_new_string(RFPROP_MANAGER_KVMIP);
        if (NULL == obj_json_str_KVMIP) {
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string failed.", __FUNCTION__, __LINE__);
        } else {
            ret = json_object_array_add(obj_json_arr, obj_json_str_KVMIP);
            do_val_if_fail(0 == ret, json_object_put(obj_json_str_KVMIP);
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add failed.", __FUNCTION__, __LINE__));
        }

        json_object_object_add(*o_result_jso, RFPROP_MANAGER_CONNECTTYPESSUPPORTED, obj_json_arr);
    }

    
    obj_json_str_int = json_object_new_int(RFPROP_MANAGER_KVMIP_MAXCONCURRENTSESSIONS);
    do_val_if_fail(NULL != obj_json_str_int,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int failed.", __FUNCTION__, __LINE__));
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_MAXCONCURRENTSESSIONS, obj_json_str_int);

    return HTTP_OK;
}

LOCAL gint32 get_manager_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return redfish_get_uuid(i_paras, o_message_jso, o_result_jso);
}

LOCAL gint32 get_manager_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 software_type = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_software_type(&software_type);

    if (MGMT_SOFTWARE_TYPE_RM == software_type) {
        *o_result_jso = json_object_new_string(RFPROP_MANAGER_iRM);
    } else {
        *o_result_jso = json_object_new_string(RFPROP_MANAGER_iBMC);
    }

    return HTTP_OK;
}

LOCAL gint32 set_manager_graphical_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    gboolean kvm_state;
    guint8 board_type = 0;
    guint8 state;
    json_object *val_jso = NULL;
    const gchar* prop_name = "GraphicalConsole/ServiceEnabled";

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(DISABLE != board_type, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_GRAPHICALCONSOLE, o_message_jso,
        RFPROP_MANAGER_GRAPHICALCONSOLE));

    
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name));
    

    
    ret = dfl_get_object_handle(PROXY_KVM_OBJ_NAME, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_SERVICEENABLED, &val_jso);
    return_val_do_info_if_fail(ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s %d: json_object_object_get_ex %s fail", __FUNCTION__, __LINE__,
        RFPROP_MANAGER_SERVICEENABLED));
    kvm_state = json_object_get_boolean(val_jso);
    debug_log(DLOG_DEBUG, "%s, %d: kvm_state is: %d.", __FUNCTION__, __LINE__, kvm_state);

    if (kvm_state) {
        state = RFPROP_MANAGER_STATE_TRUE;
    } else {
        state = RFPROP_MANAGER_STATE_FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 act_manager_rollback(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    debug_log(DLOG_DEBUG, "[%s]: act_manager_rollback.", __FUNCTION__);
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    ret = dfl_get_object_handle(OBJ_NAME_UPGRADE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    debug_log(DLOG_DEBUG, "[%s]: Later your system will rollback to the previous version!", __FUNCTION__);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_UPGRADE, METHOD_UPGRADE_ROLLBACK, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

            
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return RFERR_INSUFFICIENT_PRIVILEGE;

            
        case RFERR_NO_RESOURCE:
            debug_log(DLOG_ERROR, "[%s]:%d act_manager_rollback.", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

            
        default:
            debug_log(DLOG_ERROR, "[%s]:%d act_manager_rollback.", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_manager_virtualmedia(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    // 判断是否后插板
    guint8 kvm_enabled = DISABLE;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (kvm_enabled == DISABLE) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, MANAGER_VIRTUALMEDIA, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_manager_logservices(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_LOGSERVICES, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S manager_provider[] = {
    
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_odata_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    
    
    {
        RFPROP_MANAGER_STATUS,
        CLASS_COMPONENT, PROPERTY_COMPONENT_HEALTH,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_provider_status,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_FIRMWARE_VERSION,
        CLASS_BMC, BMC_MANA_VER_NAME,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_bmc_version,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_DATATIME,
        CLASS_BMC, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_datetime,
        set_manager_datetime,
        NULL, ETAG_FLAG_DISABLE
    },
    {
        RFPROP_MANAGER_TIMEZONE,
        CLASS_BMC, PROPERTY_BMC_TIMEZONE_STR,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_timezone,
        set_manager_timezone,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_NETWORKPROTOCOL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_networkprotocol,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_ETHERNETINTERFACES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_ethernetInterfaces,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_RESET,
        CLASS_BMC, METHD_BMC_REBOOTPME,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_reset,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_SERIALCONSOLE,
        RMCP_CLASS_NAME, PROPERTY_LAN_STATE,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_serialconsole,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_GRAPHICALCONSOLE,
        KVM_CLASS_NAME, KVM_PROPERTY_STATE,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_graphicalconsole,
        set_manager_graphical_state,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_UUID,
        CLASS_BMC, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_uuid,
        redfish_set_uuid,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_MODEL,
        CLASS_BMC, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_model,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_ROLLBACK,
        CLASS_NAME_UPGRADE, METHOD_UPGRADE_ROLLBACK,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_rollback,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_LINKS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_link,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_ACTION,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_action_target,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_OEM,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_oemresource,
        set_manager_oem,
        NULL,
        ETAG_FLAG_ENABLE
    },
    

    
    {
        MANAGER_VIRTUALMEDIA,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_virtualmedia,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_LOGSERVICES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_manager_logservices,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_EXPORT_DUMP,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_DIAGNOSEMGNT,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        NULL,
        act_manager_export_dump,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_EXPORT_CONFIG,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        NULL,
        act_manager_export_config,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_IMPORT_CONFIG,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_import_config_entry,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MANAGER_POWER_ON,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_power_on,
        ETAG_FLAG_ENABLE
    },
    

    
    {
        RFPROP_MANAGER_RESTORE_FACTORY,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_restore_factory,
        ETAG_FLAG_ENABLE
    },
    

    
    {
        RFPROP_MANAGER_DELETE_LANGUAGE,
        CLASS_BMC, PROPERTY_BMC_LANGUAGESET,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_delete_language, ETAG_FLAG_ENABLE
    },
    
    
    {
        RFPROP_MANAGER_GENERAL_DOWNLOAD,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_ALLOW,
        NULL,
        NULL,
        act_manager_general_download, ETAG_FLAG_ENABLE
    },
    
    {
        RFPROP_MANAGER_SET_FUSION_PARTITION,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_manager_set_fusion_partition, ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S *get_self_provider_manager(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(manager_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &manager_provider[i_paras->index];
}


LOCAL gint32 get_manager_action_target(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gint32 ret;
    gchar slot_name[32] = {0};
    gchar manager_uri[MAX_URI_LENGTH] = {0};
    const gchar* oem_action_array[] = {
        RFPROP_MANAGER_DELETE_LANGUAGE,
        RFPROP_MANAGER_RESTORE_FACTORY,
        RFPROP_MANAGER_ROLLBACK,
        RFPROP_MANAGER_EXPORT_DUMP,
        RFPROP_MANAGER_EXPORT_CONFIG,
        RFPROP_MANAGER_IMPORT_CONFIG,
        RFPROP_MANAGER_POWER_ON,
        RFPROP_MANAGER_GENERAL_DOWNLOAD,
        RFPROP_MANAGER_SET_FUSION_PARTITION
    };
    guint32 i;
    guint32 len;
    json_object *oem_jso = NULL;
    json_object *hw_jso = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != (*o_result_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    
    ret = redfish_get_board_slot(slot_name, sizeof(slot_name));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        json_object_put(*o_result_jso); *o_result_jso = NULL);

    ret = snprintf_s(manager_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_MANAGER, slot_name);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    
    rf_add_action_prop(*o_result_jso, (const gchar *)manager_uri, RFPROP_MANAGER_RESET);

    oem_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "alloc new json object failed"));

    hw_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != hw_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "alloc new json object failed");
        (void)json_object_put(oem_jso));

    
    len = G_N_ELEMENTS(oem_action_array);

    for (i = 0; i < len; i++) {
        rf_add_action_prop(hw_jso, (const gchar *)manager_uri, oem_action_array[i]);
    }

    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, hw_jso);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_jso);

    

    return HTTP_OK;
}


LOCAL gint32 get_manager_link(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    json_object *o_data_id = NULL;
    json_object *link_object = NULL;
    json_object *link_array = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (VOS_ERR == ret) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYSTEM, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    link_object = json_object_new_object();
    return_val_do_info_if_expr(NULL == link_object, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    o_data_id = json_object_new_string((const char *)uri);
    return_val_do_info_if_expr(NULL == o_data_id, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(link_object);
        (void)json_object_put(*o_result_jso); *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

    json_object_object_add(link_object, RFPROP_ODATA_ID, o_data_id);
    link_array = json_object_new_array();
    return_val_do_info_if_expr(NULL == link_array, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(link_object);
        (void)json_object_put(*o_result_jso); *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__));

    (void)json_object_array_add(link_array, link_object);
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_MANAGERFSERVER, link_array);

    o_data_id = NULL;
    link_object = NULL;
    link_array = NULL;

    
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_CHASSIS, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    link_object = json_object_new_object();
    return_val_do_info_if_expr(NULL == link_object, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__));

    o_data_id = json_object_new_string((const char *)uri);
    return_val_do_info_if_expr(NULL == o_data_id, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(link_object);
        (void)json_object_put(*o_result_jso); *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));

    json_object_object_add(link_object, RFPROP_ODATA_ID, o_data_id);
    link_array = json_object_new_array();
    return_val_do_info_if_expr(NULL == link_array, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(link_object);
        (void)json_object_put(*o_result_jso); *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__));

    (void)json_object_array_add(link_array, link_object);
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_MANAGERFCHASSIS, link_array);
    return HTTP_OK;
}



LOCAL gint32 set_manager_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    gint32 ret;
    json_bool ret_bool;
    json_object *huawei_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL Pointer. ", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    // 首先, 获取属性"Huawei"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_ACCOUNT_HUAWEI, &huawei_jso);
    
    return_val_do_info_if_fail(ret_bool && (NULL != huawei_jso), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));
    

    
    ret = set_manager_huawei(i_paras, o_message_jso, huawei_jso);
    
    if (ret == VOS_OK) {
        ret = HTTP_OK;
    } // if
    else {
        ret = HTTP_BAD_REQUEST;
    } // else

    return ret;
}

LOCAL void __record_language_operation_log(guchar from_webui_flag, const gchar *user_name,
    const gchar *set_language, const gchar *client, gint32 result)
{
    GSList *caller_info = NULL;

    caller_info = g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(from_webui_flag)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(client));
    if (result == RET_OK) {
        (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Successfully set %s.",
                                         set_language);
    } else {
        (void)proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Failed to set %s.",
                                         set_language);
    }
    (void)uip_free_gvariant_list(caller_info);
}

LOCAL gint32 __set_manager_language_set(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
                                             json_object *language_set_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *value_str = NULL;
    const gchar* prop_name = "Oem/Huawei/LanguageSet";
    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &o_message_jso_temp, prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return RET_ERR;
    }
                
    value_str = dal_json_object_get_str(language_set_jso); 
    
    gint32 ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return RET_ERR;
    }    
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));
    
    ret = dfl_call_class_method(obj_handle, METHOD_SET_LANGUAGESET, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    
    (void)__record_language_operation_log(i_paras->is_from_webui, i_paras->user_name, value_str, i_paras->client, ret);
    
    switch (ret) {
        case RET_OK:
            break;
        
        case LANGUAGE_NULL_OR_EMPTY: 
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_PATCH_LANGUAGE, &o_message_jso_temp, 
                                      value_str, RFPROP_MANAGER_PATCH_LANGUAGE);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return RET_ERR;
         
        case LANGUAGE_NO_UNINSTALLED_ZH_OR_EN: 
            (void)create_message_info(MSGID_LANGUAGE_ZH_AND_EN_REQUIRED, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return RET_ERR; 
         
        case LANGUAGE_NOT_SUPPORT: 
            (void)create_message_info(MSGID_LANGUAGE_NOT_SUPPORT, RFPROP_MANAGER_PATCH_LANGUAGE, &o_message_jso_temp,
                                      value_str);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return RET_ERR; 
         
        default:
            debug_log(DLOG_ERROR, "%s : call %s return %d", __FUNCTION__, METHOD_SET_LANGUAGESET, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return RET_ERR;
    }
    return RET_OK;                                         
}

LOCAL void __set_language_set(json_object *huawei_jso, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    gboolean *flag)
{
    gint32 ret;
    json_object *language_set_jso = NULL;
    json_bool ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_LANGUAGE_SET, &language_set_jso);
    if (ret_bool && language_set_jso) { 
        ret = __set_manager_language_set(i_paras, o_message_jso, language_set_jso);
        if (ret == RET_OK) {
            *flag = TRUE;
        }
    }
}


LOCAL gint32 set_dst_enable(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    guint8 dst_enable;
    GSList* input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    if ((o_message_jso == NULL) || (provider_paras_check(i_paras) != RET_OK) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(OBJ_NAME_BMC, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth failed, ret is %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_MANAGER_DST_ENABLE, o_message_jso);
        return RET_ERR;
    }

    dst_enable = (guint8)json_object_get_boolean(*o_result_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte(dst_enable));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_BMC, METHOD_BMC_SET_DST_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case RET_OK:
            return RET_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
                RFPROP_MANAGER_DST_ENABLE, o_message_jso, RFPROP_MANAGER_DST_ENABLE);
            return RET_ERR;
        default:
            debug_log(DLOG_ERROR, "%s: set DST enabled return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_MANAGER_DST_ENABLE, o_message_jso);
            return RET_ERR;
    }
}



LOCAL gint32 set_manager_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *o_result_jso)
{
    
    json_bool ret_bool;
    json_object *huawei_jso = NULL;
    json_object *dst_enable_jso = NULL;
    json_object *remote_info_jso = NULL;
    json_object *device_location_jso = NULL;
    json_object *stateless_jso = NULL;
    json_object *login_rule_jso = NULL;
    json_object *system_manager_info = NULL;
    json_object *prop_jso = NULL;
    gboolean flag = FALSE;

    
    huawei_jso = o_result_jso;                  // o_result_jso  为属性"Huawei"的值
    (*o_message_jso) = json_object_new_array(); // 存储所有出错的消息，数组的形式

    

    // 4.1 夏令时使能
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_DST_ENABLE, &dst_enable_jso);
    if (ret_bool && dst_enable_jso) {
        if (set_dst_enable(i_paras, o_message_jso, &dst_enable_jso) == RET_OK) {
            flag = TRUE;
        }
    }

    // 4.2远程共享信息
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_PATCH_REMOTE, &remote_info_jso);
    if (ret_bool) { // 用户在body中有该属性的设置
        if (set_manager_shared_info(i_paras, o_message_jso, &remote_info_jso) == RET_OK) {
            flag = TRUE;
        }
    }

    // 4.3 设备位置信息
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_PATCH_DEVICE, &device_location_jso);
    if (ret_bool && device_location_jso) { // 用户在body中有该属性的设置
        if (set_manager_device_location(i_paras, o_message_jso, &device_location_jso) == RET_OK) {
            flag = TRUE;
        }
    }

    // 4.4 无状态计算
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_PATCH_STATELESS, &stateless_jso);
    if (ret_bool && stateless_jso) { // 用户在body中有该属性的设置
        if (set_manager_stateless(i_paras, o_message_jso, &stateless_jso) == RET_OK) {
            flag = TRUE;
        }
    }

    // 4.5登录规则
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_PATCH_LOGIN, &login_rule_jso);
    if (ret_bool && login_rule_jso) { // 用户在body中有该属性的设置
        if (set_manager_loginrule(i_paras, o_message_jso, &login_rule_jso) == RET_OK) {
            flag = TRUE;
        }
    }

    
    // 4.6 上层管理软件信息
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_SYSTEM_MANAGER_INFO, &system_manager_info);
    if (ret_bool && system_manager_info) { // 用户在body中有该属性的设置
        if (set_system_manager_info(i_paras, o_message_jso, &system_manager_info) == RET_OK) {
            flag = TRUE;
        }
    }
    

    
    ret_bool = json_object_object_get_ex(huawei_jso, RFPROP_MANAGER_VGA_ENABLED, &prop_jso);
    if (ret_bool && prop_jso) { // 用户在body中有该属性的设置
        if (set_manager_vga_enabled(i_paras, *o_message_jso, prop_jso) == RET_OK) {
            flag = TRUE;
        }
    }
    
    (void)__set_language_set(huawei_jso, i_paras, o_message_jso, &flag);
    
    if (flag == TRUE) {
        return RET_OK; // 成功设置所有的属性
    }
    return RET_ERR; // 成功设置部分的属性
}



LOCAL gint32 set_manager_shared_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *temp_jso = NULL;
    json_object *o_message_jso_temp = NULL;     // 临时消息
    guint8       oem_info[MAX_LEN_OEM_INFO + 1] = {}; // guint8型的数组
    static char  s_oem_info[MAX_LEN_OEM_INFO + 1] = {0};
    gint32 len_array;
    gint32 i;
    gint32 temp_value = 0;
    GSList *input_list = NULL;
    const gchar* prop_name = "Oem/Huawei/RemoteOEMInfo";

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &o_message_jso_temp, prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso); // 第三方共享信息的值
    safe_fun_ret = memcpy_s(oem_info, MAX_LEN_OEM_INFO + 1, s_oem_info, MAX_LEN_OEM_INFO + 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    // 3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    // 3.2 获取json_object数组的长度
    len_array = json_object_array_length(val_jso);
    return_val_do_info_if_fail((len_array <= MAX_LEN_OEM_INFO + 1), VOS_ERR,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_MANAGER_PATCH_REMOTE, &o_message_jso_temp,
        RFPROP_MANAGER_PATCH_REMOTE);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s, %d 'The length of array exceeds the limits' ", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail((len_array > 0), VOS_ERR,
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_MANAGER_PATCH_REMOTE, &o_message_jso_temp,
        RFPROP_MANAGER_PATCH_REMOTE);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s, %d 'The length of array exceeds the limits' ", __FUNCTION__, __LINE__));

    // 3.3 将json_object格式的数组转化成为guint8型的数组
    for (i = 0; i < len_array; i++) {
        // 获取json_object格式的数组中的值
        temp_jso = json_object_array_get_idx(val_jso, i);

        // 判断有效性
        return_val_do_info_if_fail(json_type_int == json_object_get_type(temp_jso), VOS_ERR,
            (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_PATCH_REMOTE, &o_message_jso_temp,
            dal_json_object_get_str(temp_jso), RFPROP_MANAGER_PATCH_REMOTE);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

        // 将值添加到数组oem_info[]中
        temp_value = json_object_get_int(temp_jso);
        if (temp_value < 0 || temp_value > 255) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_PATCH_REMOTE, &o_message_jso_temp,
                dal_json_object_get_str(temp_jso), RFPROP_MANAGER_PATCH_REMOTE);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
        }

        oem_info[i] = temp_value;
    }

    // 4.3 UIP函数调用成功
    // 类方法调用成功，output_list返回值决定消息体，并进行指针释放
    
    input_list =
        g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, oem_info, len_array, sizeof(guint8)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, CLASS_NAME_BMC,
        METHOD_SET_REMOTE_OEM_INFO, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_SET_REMOTE_OEM_INFO, ret));
    

    // 对类方法的返回值进行判断
    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_DEBUG, "failure  :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}



LOCAL gint32 set_manager_device_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL; // 临时消息
    const gchar *value_str = NULL;
    gchar         valid_value_str[MAX_RULE_LEN + 1] = {0};
    GSList *input_list = NULL;
    const gchar* prop_name = "Oem/Huawei/DeviceLocation";

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &o_message_jso_temp, prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                   // 第三方共享信息的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); // (string格式)
    // 有效性检查
    if (DEVICE_LOCATION_MAX_LEN < strlen(value_str)) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_PATCH_DEVICE, &o_message_jso_temp,
            dal_json_object_get_str(val_jso), RFPROP_MANAGER_PATCH_DEVICE);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return VOS_ERR;
    }

    ret = vos_check_incorrect_char(value_str, DEVICE_LOCATION_LIMIT_CHARACTER, strlen(value_str));
    if (VOS_OK != ret) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_PATCH_DEVICE, &o_message_jso_temp,
            dal_json_object_get_str(val_jso), RFPROP_MANAGER_PATCH_DEVICE);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return VOS_ERR;
    }

    

    // 清除数组value_str中的第一个字母之前的所有空格
    // 非空串不允许进行复制操作
    if (strlen(value_str) != 0) {
        if (0 >= snprintf_s(valid_value_str, sizeof(valid_value_str), sizeof(valid_value_str) - 1, "%s", value_str)) {
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
        }

        // 清除首字母之前的空格和尾部的空格和\r, \n, \t的无效字符
        dal_trim_string(valid_value_str, MAX_RULE_LEN + 1);
    }

    

    
    // 3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)valid_value_str));

    

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_CLASEE_NAME, METHOD_BMC_SET_LOCATION_INFO, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_BMC_SET_LOCATION_INFO, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}


LOCAL gint32 set_manager_sys_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *property)
{
    // 内部接口，不做入参检测
    const gchar *ip_string = NULL;
    GSList *input_list = NULL;
    gint32 ret;
    json_object *o_message_jso_temp = NULL;

    ip_string = dal_json_object_get_str(property);

    input_list = g_slist_append(input_list, g_variant_new_string(ip_string));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, CLASS_STATELESS,
        METHOD_STATELESS_SET_SYS_MANAGER_IP, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_MANAGER_STATELESS_SYS_IP, &o_message_jso_temp,
                ERROR_MANAGER_STATELESS_SYS_IP);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;

        case INVALID_IP_ADDRESS:
            (void)create_message_info(MSGID_INVALID_IPV4ADDRESS, ERROR_MANAGER_STATELESS_SYS_IP, &o_message_jso_temp,
                ip_string, ERROR_MANAGER_STATELESS_SYS_IP);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;

        default:
            debug_log(DLOG_ERROR, "%s : call %s return %d", __FUNCTION__, METHOD_STATELESS_SET_SYS_MANAGER_IP, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 set_manager_sys_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object *property)
{
    // 内部接口，不做入参检测
    guint32 port;
    GSList *input_list = NULL;
    gint32 ret;
    json_object *o_message_jso_temp = NULL;

    if (json_type_int != json_object_get_type(property)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_MANAGER_STATELESS_SYS_PORT, &o_message_jso_temp,
            dal_json_object_get_str(property), ERROR_MANAGER_STATELESS_SYS_PORT);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        return VOS_ERR;
    }

    // 在schema中限制了范围  1-65535
    port = (guint32)json_object_get_int(property);

    input_list = g_slist_append(input_list, g_variant_new_uint32(port));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, CLASS_STATELESS,
        METHOD_STATELESS_SET_SYS_MANAGER_PORT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_MANAGER_STATELESS_SYS_PORT,
                &o_message_jso_temp, ERROR_MANAGER_STATELESS_SYS_PORT);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;

        default:
            debug_log(DLOG_ERROR, "%s : call %s return %d", __FUNCTION__, METHOD_STATELESS_SET_SYS_MANAGER_PORT, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 set_manager_stateless(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    json_bool ret_bool;
    json_object *stateless_jso = NULL;
    json_object *sys_id_jso = NULL;
    json_object *power_jso = NULL;
    json_object *search_jso = NULL;
    gint32 ret_set = 0;
    gboolean flag = FALSE;
    json_object *property = NULL;

    
    stateless_jso = (*o_result_jso); // o_result_jso  为属性"Huawei"的值

    
    // 4.1远程共享信息
    ret_bool = json_object_object_get_ex(stateless_jso, RFPROP_MANAGER_PATCH_STATELESS_SYS_ID, &sys_id_jso);
    if (ret_bool && sys_id_jso) { // 用户在body中有该属性的设置
        ret_set = set_manager_sys_id(i_paras, o_message_jso, &sys_id_jso);
        if (ret_set == VOS_OK) {
            flag = TRUE;
        }
    }

    
    // 设置远程管理服务器的IP
    ret_bool = json_object_object_get_ex(stateless_jso, RFPROP_MANAGER_PATCH_STATELESS_SYS_IP, &property);
    if (ret_bool && property) {
        ret_set = set_manager_sys_ip(i_paras, o_message_jso, property);
        do_if_expr(ret_set == VOS_OK, (flag = TRUE));
        property = NULL;
    }

    // 设置远程管理服务器的端口
    ret_bool = json_object_object_get_ex(stateless_jso, RFPROP_MANAGER_PATCH_STATELESS_SYS_PORT, &property);
    if (ret_bool && property) {
        ret_set = set_manager_sys_port(i_paras, o_message_jso, property);
        do_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    

    // 4.2 设置受控上电是否开启
    ret_bool = json_object_object_get_ex(stateless_jso, RFPROP_MANAGER_PATCH_STATELESS_POWER_ENABLED, &power_jso);
    if (ret_bool && power_jso) { // 用户在body中有该属性的设置
        ret_set = set_manager_power(i_paras, o_message_jso, &power_jso);
        if (ret_set == VOS_OK) {
            flag = TRUE;
        }
    }

    // 4.3 无状态计算使能
    ret_bool = json_object_object_get_ex(stateless_jso, RFPROP_MANAGER_PATCH_STATELESS_SEARCH_ENABLED, &search_jso);
    if (ret_bool && search_jso) { // 用户在body中有该属性的设置
        ret_set = set_manager_search(i_paras, o_message_jso, &search_jso);
        if (ret_set == VOS_OK) {
            flag = TRUE;
        }
    }

    
    if (flag == TRUE) {
        return VOS_OK; // 属性设置成功
    } else {
        return VOS_ERR; // 属性设置失败
    }
}



LOCAL gint32 set_manager_sys_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *value_str = NULL;
    GSList *input_list = NULL;
    const gchar* prop_name = "Oem/Huawei/Stateless/SysManagerId";

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &o_message_jso_temp, prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                   // 第三方共享信息的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); // (string格式)

    
    // 3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_STATELESS, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_STATELESS, METHOD_STATELESS_SET_SYS_MANAGER_ID, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_STATELESS_SET_SYS_MANAGER_ID, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_PATCH_STATELESS_SYS_ID,
                &o_message_jso_temp, dal_json_object_get_str(val_jso), RFPROP_MANAGER_PATCH_STATELESS_SYS_ID);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}



LOCAL gint32 set_manager_search(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    gboolean value_bool;
    guint8 value_byte;
    GSList *input_list = NULL;
    const gchar* prop_name = "Oem/Huawei/Stateless/AutoSearchEnabled";

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &o_message_jso_temp, prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                     // 第三方共享信息的值(json_object 格式)
    value_bool = json_object_get_boolean(val_jso); // bool 格式
    if (value_bool == TRUE) {
        value_byte = 1;
    } else {
        value_byte = 0;
    }

    
    // 3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_STATELESS, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_byte(value_byte));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_STATELESS, METHOD_STATELESS_SET_ENABLE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_STATELESS_SET_ENABLE, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}



LOCAL gint32 set_manager_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    gboolean value_bool;
    guint32 value_byte;
    GSList *input_list = NULL;

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_MANAGER_STATELESS_CONTROLLED_POWERON,
        &o_message_jso_temp, ERROR_MANAGER_STATELESS_CONTROLLED_POWERON);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                     // 第三方共享信息的值(json_object 格式)
    value_bool = json_object_get_boolean(val_jso); // (string格式)
    
    // 这个属性是设置受控上电，而调用的方法是设置自主上电的方法
    // 当设置受控上电为true，即要设置自助上电为0，否则相反
    if (value_bool == TRUE) {
        value_byte = 0;
    } else {
        value_byte = 1;
    }

    

    
    // 3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_STATELESS, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_byte(value_byte));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_STATELESS, METHOD_STATELESS_SET_AUTO_POWER_ON, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_STATELESS_SET_AUTO_POWER_ON, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}


LOCAL gint32 rfsmtp_check_array_object(json_object *object)
{
    gint32 i;
    gint32 length;
    json_object *item_jso = NULL;
    json_object *user_date = NULL;

    if (json_type_array != json_object_get_type(object)) {
        return VOS_ERR;
    }

    length = json_object_array_length(object);

    for (i = 0; i < length; i++) {
        item_jso = json_object_array_get_idx(object, i);
        if (TRUE == json_object_get_user_data(item_jso, (void **)&user_date) && NULL != user_date) {
            return VOS_ERR;
        }

        if (VOS_OK != g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(item_jso))) {
            return RF_SPECIAL_VALUE;
        }
    }

    return VOS_OK;
}



gint32 set_manager_loginrule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    json_bool ret_bool = FALSE;
    json_bool ret_start_bool = FALSE;
    json_bool ret_end_bool = FALSE;
    json_object *login_jso = NULL;
    json_object *tmp_jso = NULL;
    json_object *enabled_jso = NULL;
    json_object *start_time_jso = NULL;
    json_object *end_time_jso = NULL;
    json_object *ip_jso = NULL;
    json_object *mac_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    gint32 ret_set = 0;
    gboolean flag = FALSE;
    gboolean flag_deletion = FALSE;
    guint32 i;
    guint32 len;
    const gchar *temp_gchar = NULL;
    guint8 item_num = 0;
    guint8 ret;
    gchar               name_parameter[MAX_CHARACTER_NUM] = {0};

    
    login_jso = (*o_result_jso); // o_result_jso  为属性"Huawei"的值

    
    len = json_object_array_length(login_jso);

    
    ret = rf_check_array_length_valid(login_jso, 3, &o_message_jso_temp, RFPROP_MANAGER_PATCH_LOGIN);
    do_if_true(o_message_jso_temp, (void)json_object_array_add(*o_message_jso, o_message_jso_temp));
    return_val_if_fail(VOS_OK == ret, VOS_ERR);
    

    // 数组中不能出现相同的项在数组中，不会出现重复的设置(以不同的下标来区分不同的规则))
    for (i = 0; i < len; i++) {
        // 获取数组中的一个对象
        tmp_jso = json_object_array_get_idx(login_jso, i);
        temp_gchar = dal_json_object_get_str(tmp_jso); // 转换成为字符串形式
        // 判断是否是"{ }"
        if (VOS_OK == g_strcmp0(JSON_NULL_OBJECT_STR, temp_gchar)) { // 表明是{}
            continue;                                                // { } 不做操作
        }

        // 判断登录规则是否是 null
        if (NULL == temp_gchar) {
            (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_MANAGER_PATCH_LOGIN, &o_message_jso_temp,
                RFPROP_MANAGER_PATCH_LOGIN);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            return VOS_ERR;
        }

        // 统计非空元素的个数
        item_num++;

        // PATCH 操作
        

        // 使能操作
        ret_bool = json_object_object_get_ex(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_RULE_ENABLED, &enabled_jso);
        if (ret_bool && enabled_jso) { // 用户在body中有该属性的设置
            ret_set = set_manager_login_enabled(i_paras, o_message_jso, &enabled_jso, i);
            if (ret_set == VOS_OK) {
                flag = TRUE;
            }
        }

        // 4.3 时间操作
        ret_start_bool = json_object_object_get_ex(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_START_TIME, &start_time_jso);
        ret_end_bool = json_object_object_get_ex(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_END_TIME, &end_time_jso);
        if (ret_start_bool && ret_end_bool) { // 用户开始时间和结束时间必须都要有值
            ret_set = set_manager_login_time(i_paras, o_message_jso, &start_time_jso, &end_time_jso, i);
            if (ret_set == VOS_OK) {
                flag = TRUE;
            }
        } else {
            
            // 判断开始时间和结束时间
            if (ret_start_bool && (!ret_end_bool)) {
                rf_check_property_deleted(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_END_TIME, &flag_deletion);

                if (flag_deletion == FALSE) { // 用户没有输入该属性的值
                    // 构成属性"EndTime"的完整路径
                    if (0 >= snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1,
                        "Oem/Huawei/LoginRule/%u/%s", i, RFPROP_MANAGER_PATCH_LOGIN_END_TIME)) {
                        return VOS_ERR;
                    }

                    (void)create_message_info(MSGID_PROP_MISSING, (const gchar *)name_parameter, &o_message_jso_temp,
                        (const gchar *)name_parameter);
                    (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
                }
            }

            if ((!ret_start_bool) && ret_end_bool) {
                rf_check_property_deleted(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_START_TIME, &flag_deletion);

                if (flag_deletion == FALSE) { // 用户没有输入该属性的值
                    // 构成属性"StartTime"的完整路径
                    if (0 >= snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1,
                        "Oem/Huawei/LoginRule/%u/%s", i, RFPROP_MANAGER_PATCH_LOGIN_START_TIME)) {
                        return VOS_ERR;
                    }

                    (void)create_message_info(MSGID_PROP_MISSING, (const gchar *)name_parameter, &o_message_jso_temp,
                        (const gchar *)name_parameter);
                    (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
                }
            }

            
        }

        // 4.5 ip
        ret_bool = json_object_object_get_ex(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_IP, &ip_jso);
        if (ret_bool && ip_jso) { // 用户在body中有该属性的设置
            ret_set = set_manager_login_ip(i_paras, o_message_jso, &ip_jso, i);
            if (ret_set == VOS_OK) {
                flag = TRUE;
            }
        }

        // 4.5 mac
        ret_bool = json_object_object_get_ex(tmp_jso, RFPROP_MANAGER_PATCH_LOGIN_MAC, &mac_jso);
        if (ret_bool && mac_jso) { // 用户在body中有该属性的设置
            ret_set = set_manager_login_mac(i_paras, o_message_jso, &mac_jso, i);
            if (ret_set == VOS_OK) {
                flag = TRUE;
            }
        }
    }

    

    if (item_num != 0) { // 拥有有效设置数据
        if (flag == TRUE) {
            return VOS_OK; // 所有属性设置成功
        } else {
            return VOS_ERR; // 部分属性设置成功
        }
    } else {
        
        if (TRUE == json_object_object_get_ex(i_paras->user_data, RFPROP_MANAGER_PATCH_LOGIN, &login_jso) &&
            RF_SPECIAL_VALUE == rfsmtp_check_array_object(login_jso)) {
            return VOS_ERR;
        } else {
            (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, RFPROP_MANAGER_PATCH_LOGIN, &o_message_jso_temp,
                RFPROP_MANAGER_PATCH_LOGIN);
            ret = json_object_array_add(*o_message_jso, o_message_jso_temp);
            do_val_if_expr(VOS_OK != ret, json_object_put(o_message_jso_temp));
            return VOS_ERR;
        }
    }
}


LOCAL gint32 set_manager_vga_enabled(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso, json_object *prop_jso)
{
    gint32 ret_val;
    json_object *message_jso = NULL;
    const gchar *json_pointer = "Oem/Huawei/VGAUSBDVDEnabled";
    guint8 vga_enabled;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    ret_val = bmc_is_4p();
    goto_label_do_info_if_fail(VOS_OK == ret_val, exit, ret_val = HTTP_NOT_IMPLEMENTED;
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, json_pointer, &message_jso, json_pointer));

    vga_enabled = (TRUE == json_object_get_boolean(prop_jso)) ? ENABLE : DISABLE;

    ret_val = dfl_get_object_handle(PARTITION_OBJECT_NAME, &obj_handle);
    goto_label_do_info_if_fail(DFL_OK == ret_val, exit, ret_val = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, "%s:dfl_get_object_handle %s fail.", __FUNCTION__, PARTITION_OBJECT_NAME);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(vga_enabled));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PARTITION, METHOD_PARTITION_SWITCHVGA, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return_val_if_expr(VOS_OK == ret_val, VOS_OK);
    goto_label_do_info_if_expr(RFERR_INSUFFICIENT_PRIVILEGE == ret_val, exit,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_jso));

    debug_log(DLOG_ERROR, "%s: set %s failed, ret_val = %d", __FUNCTION__, METHOD_PARTITION_SWITCHVGA, ret_val);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);

exit:
    (void)json_object_array_add(o_message_jso, message_jso);

    return ret_val;
}


LOCAL gint32 set_manager_login_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint32 id)
{
    
    OBJ_HANDLE obj_handle = 0;
    json_object *jso_temp = NULL; // 临时消息
    guint8 permit_id = 0;
    GSList *input_list = NULL;
    gchar prop_name[MAX_LEN_OEM_INFO + 1] = {0};

    gint32 ret = snprintf_s(prop_name, sizeof(prop_name), sizeof(prop_name) - 1, "%s/%u/%s",
        PROPERTY_LOGIN_RULE_PATH, id, "RuleEnabled");
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, VOS_ERR,
        debug_log(DLOG_INFO, "%s: user dont have security privilege.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar*)prop_name, &jso_temp,
            (const gchar*)prop_name);
        (void)json_object_array_add(*o_message_jso, jso_temp));

    
    json_object *val_jso = (*o_result_jso);                     // enabled的值(json_object 格式)
    gboolean value_bool = json_object_get_boolean(val_jso); // 使能的值

    
    // 3.1 获得permitrule的句柄
    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &jso_temp);
        (void)json_object_array_add(*o_message_jso, jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    // 获取类对象中属性"Logininterface"的值
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PERMIT_RULE_IDS, &permit_id);
    if (VOS_OK != ret) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &jso_temp);
        (void)json_object_array_add(*o_message_jso, jso_temp);
        debug_log(DLOG_INFO, "%s: get PermitRuleIds failed ret %d", __FUNCTION__, ret);
        return VOS_ERR;
    } else {
        

        if (value_bool == TRUE) {
            // 更新id的值
            permit_id = permit_id | BIT(id);
        } else if (value_bool == FALSE) {
            // 更新id的值
            permit_id = permit_id & (guint8)(~BIT(id));
        }

        
    }

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_byte(permit_id));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_SECURITY_ENHANCE_SETPERMITRULEIDS, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_SECURITY_ENHANCE_SETPERMITRULEIDS, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: failure :%d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &jso_temp);
            (void)json_object_array_add(*o_message_jso, jso_temp);
            ret = VOS_ERR;
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}

LOCAL gint32 handle_login_rule_retcode(gint32 ret, json_object **o_message_jso, json_object *o_message_jso_temp,
     const gchar* prop_name, const gchar* prop_value)
{
    gint32 result;

    switch (ret) {
        case USER_COMMAND_NORMALLY:
            result = VOS_OK;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: failure :%d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_name, &o_message_jso_temp, prop_value,
                prop_name);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            result = VOS_ERR;            
            break;
    }  
    return result; 
}



LOCAL gint32 set_manager_login_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_start_jso, json_object **o_result_end_jso, guint32 id)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_end_jso = NULL;
    json_object *o_message_jso_temp = NULL; // 临时消息
    const gchar *start_str = NULL;
    const gchar *end_str = NULL;
    gchar time_info[MAX_TIME_LENGTH] = {0};
    GSList *input_list = NULL;
    gchar prop_name[MAX_LEN_OEM_INFO + 1] = {0};

    
    // 只读权限
    ret = snprintf_s(prop_name, sizeof(prop_name), sizeof(prop_name) - 1, "%s/%u/%s", PROPERTY_LOGIN_RULE_PATH, id,
        RFPROP_MANAGER_PATCH_LOGIN_TIME);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, VOS_ERR,
        debug_log(DLOG_INFO, "%s: user dont have security privilege.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar*)prop_name, &o_message_jso_temp,
            (const gchar*)prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    json_object *val_start_jso = (*o_result_start_jso);             // enabled的值(json_object 格式)
    val_end_jso = (*o_result_end_jso);                 // enabled的值(json_object 格式)
    start_str = dal_json_object_get_str(val_start_jso); // (string格式)
    end_str = dal_json_object_get_str(val_end_jso);     // (string格式)
    ret = snprintf_s(time_info, sizeof(time_info), sizeof(time_info) - 1, "%s/%s", start_str, end_str);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));

    // 4. 获得permitrule的句柄
    ret = dal_get_object_handle_nth(CLASS_PERMIT_RULE, id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)time_info));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SECURITY_ENHANCE, METHOD_PERMIT_RULE_SETTIMERULEINFO, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_PERMIT_RULE_SETTIMERULEINFO, ret));

    
    uip_free_gvariant_list(input_list);

    
    return handle_login_rule_retcode(ret, o_message_jso, o_message_jso_temp, RFPROP_MANAGER_PATCH_LOGIN_TIME,
        (const gchar *)time_info);
}



LOCAL gint32 set_manager_login_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guint32 id)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *o_message_jso_temp = NULL; // 临时消息
    const gchar *value_str = NULL;
    GSList *input_list = NULL;
    gchar prop_name[MAX_LEN_OEM_INFO + 1] = {0};

    
    // 只读权限
    ret = snprintf_s(prop_name, sizeof(prop_name), sizeof(prop_name) - 1, "%s/%u/%s", PROPERTY_LOGIN_RULE_PATH,
        id, "IP");
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, VOS_ERR,
        debug_log(DLOG_INFO, "%s: user dont have security privilege.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar*)prop_name, &o_message_jso_temp,
            (const gchar*)prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    json_object* val_jso = (*o_result_jso);                   // ip的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); // (string格式)

    
    // 4.1 获得permitrule的句柄
    ret = dal_get_object_handle_nth(CLASS_PERMIT_RULE, id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PERMIT_RULE, METHOD_PERMIT_RULE_SETIPRULEINFO, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_PERMIT_RULE_SETIPRULEINFO, ret));

    
    uip_free_gvariant_list(input_list);

    
    return handle_login_rule_retcode(ret, o_message_jso, o_message_jso_temp, RFPROP_MANAGER_PATCH_LOGIN_IP, value_str);
}



LOCAL gint32 set_manager_login_mac(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guint32 id)
{
    errno_t safe_fun_ret;

    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *o_message_jso_temp = NULL; // 临时消息
    const gchar *value_str = NULL;
    gchar         mactmp[MAC_RULE_LEN + 1] = {0};
    GSList *input_list = NULL;
    gchar prop_name[MAX_LEN_OEM_INFO + 1] = {0};

    
    // 只读权限
    ret = snprintf_s(prop_name, sizeof(prop_name), sizeof(prop_name) - 1, "%s/%u/%s", PROPERTY_LOGIN_RULE_PATH,
        id, "Mac");
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, VOS_ERR,
        debug_log(DLOG_INFO, "%s: user dont have security privilege.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar*)prop_name, &o_message_jso_temp, 
            (const gchar*)prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    json_object *val_jso = (*o_result_jso);                                                   // ip的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso);                                 // (string格式)
    safe_fun_ret = strncpy_s(mactmp, MAC_RULE_LEN + 1, value_str, MAC_RULE_LEN); //
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    
    // 4.1 获得permitrule的句柄
    ret = dal_get_object_handle_nth(CLASS_PERMIT_RULE, id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PERMIT_RULE, METHOD_PERMIT_RULE_SETMACRULEINFO, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_PERMIT_RULE_SETMACRULEINFO, ret));

    
    uip_free_gvariant_list(input_list);

    
    return handle_login_rule_retcode(ret, o_message_jso, o_message_jso_temp, RFPROP_MANAGER_PATCH_LOGIN_MAC, value_str);
}



gint32 set_system_manager_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    json_bool ret_bool;
    json_object *system_manager_info = NULL;
    json_object *id_jso = NULL;
    json_object *name_jso = NULL;
    json_object *ip_jso = NULL;
    json_object *locked_time_jso = NULL;
    gint32 ret_set = 0;
    gboolean flag = FALSE;

    
    system_manager_info = (*o_result_jso); //  o_result_jso  为属性"SystemManagerInfo"的值

    //  3.1 设置上层管理软件ID信息
    ret_bool = json_object_object_get_ex(system_manager_info, RFPROP_SYSTEM_MANAGER_INFO_ID, &id_jso);
    if (ret_bool && id_jso) { //  用户在body中有该属性的设置
        ret_set = set_system_manager_id(i_paras, o_message_jso, &id_jso);
        do_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    //  3.2 设置上层管理软件NAME信息
    ret_bool = json_object_object_get_ex(system_manager_info, RFPROP_SYSTEM_MANAGER_INFO_NAME, &name_jso);
    if (ret_bool && name_jso) { //  用户在body中有该属性的设置
        ret_set = set_system_manager_name(i_paras, o_message_jso, &name_jso);
        do_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    //  3.3 设置上层管理软件IP信息
    ret_bool = json_object_object_get_ex(system_manager_info, RFPROP_SYSTEM_MANAGER_INFO_IP, &ip_jso);
    if (ret_bool && ip_jso) { //  用户在body中有该属性的设置
        ret_set = set_system_manager_ip(i_paras, o_message_jso, &ip_jso);
        do_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    //  3.4 设置上层管理软件LOCKED_TIME信息
    ret_bool = json_object_object_get_ex(system_manager_info, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME, &locked_time_jso);
    if (ret_bool && locked_time_jso) { //  用户在body中有该属性的设置
        ret_set = set_system_manager_locked_time(i_paras, o_message_jso, &locked_time_jso);
        do_if_expr(ret_set == VOS_OK, (flag = TRUE));
    }

    
    if (flag == TRUE) {
        return VOS_OK; //  属性设置成功
    } else {
        return VOS_ERR; //  属性设置失败
    }
}


LOCAL gint32 set_system_manager_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *value_str = NULL;
    GSList *input_list = NULL;

    
    //  只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSTEM_MANAGER_INFO_ID_PATH,
        &o_message_jso_temp, RFPROP_SYSTEM_MANAGER_INFO_ID_PATH);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                   //  第三方共享信息的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); //  (string格式)

    //  判断值是否长度小于等于128
    return_val_do_info_if_fail(MAX_PROP_VAL_LEN >= strlen(value_str), VOS_ERR,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_SYSTEM_MANAGER_INFO_ID_PATH,
        &o_message_jso_temp, value_str, RFPROP_SYSTEM_MANAGER_INFO_ID_PATH, MAX_PROP_VAL_LEN_STR);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:value_str is too long. The length is %zu", __FUNCTION__, strlen(value_str)));

    
    //  3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_SYSTEM_MANAGER_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSTEM_MANAGER_INFO, METHOD_SYSTEM_MANAGER_INFO_SET_ID, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_SYSTEM_MANAGER_INFO_SET_ID, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_SYSTEM_MANAGER_INFO_ID_PATH,
                &o_message_jso_temp, dal_json_object_get_str(val_jso), RFPROP_SYSTEM_MANAGER_INFO_ID_PATH);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }
    
    uip_free_gvariant_list(input_list);

    
    return ret;
}


LOCAL gint32 set_system_manager_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *value_str = NULL;
    GSList *input_list = NULL;

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH,
        &o_message_jso_temp, RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                   //  第三方共享信息的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); //  (string格式)

    // 判断值是否长度小于等于128
    return_val_do_info_if_fail(MAX_PROP_VAL_LEN >= strlen(value_str), VOS_ERR,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH,
        &o_message_jso_temp, value_str, RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH, MAX_PROP_VAL_LEN_STR);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:value_str is too long. The length is %zu", __FUNCTION__, strlen(value_str)));

    
    //  3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_SYSTEM_MANAGER_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    //  4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    //  4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSTEM_MANAGER_INFO, METHOD_SYSTEM_MANAGER_INFO_SET_NAME, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_SYSTEM_MANAGER_INFO_SET_NAME, ret));

    //  4.3 UIP函数调用成功
    //  对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH,
                &o_message_jso_temp, dal_json_object_get_str(val_jso), RFPROP_SYSTEM_MANAGER_INFO_NAME_PATH);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }
    
    uip_free_gvariant_list(input_list);

    
    return ret;
}


LOCAL gint32 set_system_manager_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *value_str = NULL;
    GSList *input_list = NULL;

    
    //  只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSTEM_MANAGER_INFO_IP_PATH,
        &o_message_jso_temp, RFPROP_SYSTEM_MANAGER_INFO_IP_PATH);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                   //  第三方共享信息的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); //  (string格式)

    //  判断值是否长度小于等于128
    return_val_do_info_if_fail(MAX_PROP_VAL_LEN >= strlen(value_str), VOS_ERR,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_SYSTEM_MANAGER_INFO_IP_PATH,
        &o_message_jso_temp, value_str, RFPROP_SYSTEM_MANAGER_INFO_IP_PATH, MAX_PROP_VAL_LEN_STR);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:value_str is too long. The length is %zu", __FUNCTION__, strlen(value_str)));

    
    //  3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_SYSTEM_MANAGER_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    //  4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    //  4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSTEM_MANAGER_INFO, METHOD_SYSTEM_MANAGER_INFO_SET_IP, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_SYSTEM_MANAGER_INFO_SET_IP, ret));

    //  4.3 UIP函数调用成功

    //  对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_SYSTEM_MANAGER_INFO_IP_PATH,
                &o_message_jso_temp, dal_json_object_get_str(val_jso), RFPROP_SYSTEM_MANAGER_INFO_IP_PATH);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}


LOCAL gint32 set_system_manager_locked_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    const gchar *value_str = NULL;
    GSList *input_list = NULL;

    
    //  只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH,
        &o_message_jso_temp, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                   //  第三方共享信息的值(json_object 格式)
    value_str = dal_json_object_get_str(val_jso); // (string格式)

    //  判断值是否长度小于等于128
    return_val_do_info_if_fail(MAX_PROP_VAL_LEN >= strlen(value_str), VOS_ERR,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH,
        &o_message_jso_temp, value_str, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH, MAX_PROP_VAL_LEN_STR);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:value_str is too long. The length is %zu", __FUNCTION__, strlen(value_str)));

    
    //  3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(CLASS_SYSTEM_MANAGER_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(value_str));

    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SYSTEM_MANAGER_INFO, METHOD_SYSTEM_MANAGER_INFO_SET_LOCKED_TIME, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_SYSTEM_MANAGER_INFO_SET_LOCKED_TIME, ret));

    // 4.3 UIP函数调用成功
    // 对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH,
                &o_message_jso_temp, dal_json_object_get_str(val_jso), RFPROP_SYSTEM_MANAGER_INFO_LOCKEDTIME_PATH);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}




LOCAL gint32 file_uploading(PROVIDER_PARAS_S *i_paras, const gchar *transfer_protocol, gint32 file_id,
    const gchar *temp_file_path, json_object **o_message_jso)
{
    
    GSList *input_list = NULL;
    OBJ_HANDLE file_transfer_handle;
    gint32 ret;
    gchar          file_transfer_uri[MAX_URI_PATH_LENGTH] = {0}; // 数组长度为2048

    
    if (is_valid_file_transfer_protocol(transfer_protocol) == FALSE) {
        (void)create_message_info(MSGID_FILE_TRANSFER_PROTOCOL_MISMATCH, NULL, o_message_jso);
        (void)proxy_delete_file(0, NULL, NULL, temp_file_path);
        return VOS_ERR;
    }

    
    // 构成文件上传的完整路径
    if (0 >= snprintf_s(file_transfer_uri, sizeof(file_transfer_uri), sizeof(file_transfer_uri) - 1, "upload;%d;%s",
        file_id, transfer_protocol)) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        (void)proxy_delete_file(0, NULL, NULL, temp_file_path);
        return VOS_ERR;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &file_transfer_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        (void)memset_s(file_transfer_uri, MAX_URI_PATH_LENGTH, 0, MAX_URI_PATH_LENGTH);
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_TRANSFERFILE);
        (void)proxy_delete_file(0, NULL, NULL, temp_file_path));

    
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_transfer_uri));
    (void)memset_s(file_transfer_uri, MAX_URI_PATH_LENGTH, 0, MAX_URI_PATH_LENGTH);
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));

    
    
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, file_transfer_handle,
        CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);

    uip_free_gvariant_list(input_list);
    

    if (ret != VOS_OK) {
        (void)parse_file_transfer_err_code(ret, o_message_jso);
        debug_log(DLOG_ERROR, "call method %s failed, ret:%d", METHOD_INITRIAL_FILE_TRANSFER, ret);
        (void)proxy_delete_file(0, NULL, NULL, temp_file_path);
        return VOS_ERR;
    }

    return ret;
}


LOCAL gint32 file_downloading(PROVIDER_PARAS_S *i_paras, const gchar *transfer_protocol, gint32 file_id,
    json_object **o_message_jso)
{
    
    GSList *input_list = NULL;
    OBJ_HANDLE file_transfer_handle;
    gint32 ret;
    gchar       file_transfer_uri[MAX_URI_PATH_LENGTH] = {0}; 

    

    
    // 构成文件上传的完整路径
    if (0 >= snprintf_s(file_transfer_uri, sizeof(file_transfer_uri), MAX_URI_PATH_LENGTH - 1, "download;%d;%s",
        file_id, transfer_protocol)) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return VOS_ERR;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &file_transfer_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        (void)memset_s(file_transfer_uri, sizeof(file_transfer_uri), 0, sizeof(file_transfer_uri));
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_TRANSFERFILE));

    
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_transfer_uri));
    (void)memset_s(file_transfer_uri, sizeof(file_transfer_uri), 0, sizeof(file_transfer_uri));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));

    
    
    ret = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        (i_paras->auth_type == LOG_TYPE_LOCAL), (const gchar *)i_paras->user_roleid, file_transfer_handle,
        CLASS_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, (void)parse_file_transfer_err_code(ret, o_message_jso);
        debug_log(DLOG_ERROR, "cann't use methos %s", METHOD_INITRIAL_FILE_TRANSFER));

    return ret;
}


LOCAL gint32 parse_dumping_err_code(gint32 transfer_status, json_object **message_obj)
{
    gint32 ret;
    INT2STR_MAP_S    file_transfer_err_array[] = {
        {UNKNOWN_ERROR,                    UNKNOWN_ERROR_STR},
    };
    const gchar *err_desc = NULL;

    
    if ((transfer_status >= 0) && (transfer_status <= RF_FINISH_PERCENTAGE)) {
        return RF_OK;
    } else {
        transfer_status = -1; // 小于0的返回码统一处理为-1，便于形成错误消息
    }

    
    ret = get_str_val_from_map_info(file_transfer_err_array, G_N_ELEMENTS(file_transfer_err_array), transfer_status,
        &err_desc);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't find error description for %d", transfer_status));

    (void)create_message_info(MSGID_DUMP_COLLECT_ERR_DESC, NULL, message_obj, err_desc);
    return RF_FAILED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_FAILED;
}


LOCAL gint32 parse_collecting_err_code(gint32 transfer_status, const gchar *err_str, json_object **message_obj)
{
    
    if ((transfer_status >= 0) && (transfer_status <= RF_FINISH_PERCENTAGE)) {
        return RF_OK;
    } else {
        

        
        (void)create_message_info(MSGID_COLLECTING_ERR_DESC, NULL, message_obj, err_str);
        

        return RF_FAILED;
    }
}


LOCAL gint32 dumping(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    
    guint32 ret;                // 函数的返回值
    OBJ_HANDLE obj_dump_handle = 0; // 双因素客户端类的对象的句柄
    GSList *output_list = NULL;
    GSList *input_list = NULL;
    guint8 software_type = 0;

    
    // 入参1:  获取用户的ID对应的双因素客户端证书对象的句柄
    ret = dal_get_object_handle_nth(CLASS_DUMP, 0, &obj_dump_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR,
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)i_paras->auth_type));
// 3. 调用UIP函数
    (void)dal_get_software_type(&software_type);

    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        input_list = g_slist_append(input_list, g_variant_new_string(g_ascii_strdown(i_paras->member_id, -1)));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            obj_dump_handle, BMC_CLASEE_NAME, METHOD_DUMP_DUMPINFO_BOTH_SMM_ASYNC, AUTH_DISABLE,
            i_paras->user_role_privilege, input_list, &output_list);
    } else {
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            obj_dump_handle, BMC_CLASEE_NAME, METHOD_DUMP_DUMPINFO_ASYNC, AUTH_DISABLE, i_paras->user_role_privilege,
            input_list, &output_list);
    }
    uip_free_gvariant_list(input_list);
    
    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_DUMP_DUMPINFO_ASYNC, ret));

    
    uip_free_gvariant_list(output_list);
    return ret;
}


LOCAL gint32 collecting(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    
    guint32 ret;                      // 函数的返回值
    OBJ_HANDLE obj_collecting_handle = 0; // 配置文件收集的句柄
    GSList *input_list = NULL;
    
    // 入参1:  获取BMC对象的句柄
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_collecting_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR,
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    

    input_list = g_slist_append(input_list, g_variant_new_string(TMP_CONFIG_PEM_FILE));

    // 3. 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_collecting_handle, BMC_CLASEE_NAME, METHOD_EXPORT_ALL_CONFIG, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);

    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_EXPORT_ALL_CONFIG, ret));

    
    uip_free_gvariant_list(input_list);
    return ret;
}


LOCAL gint32 importing_configuration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    
    guint32 ret;                      // 函数的返回值
    OBJ_HANDLE obj_collecting_handle = 0; // 双因素客户端类的对象的句柄
    GSList *input_list = NULL;

    
    // 入参1:  类BMC对象的句柄obj_collecting_handle
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_collecting_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR,
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    
    // 路径有效性
    ret = dal_check_real_path(TMP_CONFIG_PEM_FILE);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : file real_path:%s is illegal! return %d\r\n", __FUNCTION__, TMP_CONFIG_PEM_FILE,
            ret);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, TMP_CONFIG_PEM_FILE,
            RFACTION_PARAM_CONTENT);
        return VOS_ERR;
    }

    // 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(TMP_CONFIG_PEM_FILE));

    // 3.  调用UIP函数(方法METHOD_IMPORT_ALL_CONFIG中会删除临时文件)
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_collecting_handle, BMC_CLASEE_NAME, METHOD_IMPORT_ALL_CONFIG, AUTH_DISABLE, i_paras->user_role_privilege,
        input_list, NULL);

    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_IMPORT_ALL_CONFIG, ret));

    
    uip_free_gvariant_list(input_list);

    return ret;
}


LOCAL gint32 importing_configuration_in_task(json_object *user_data, json_object *body_jso, const gchar *file_path,
    json_object **o_message_jso)
{
    
    guint32 ret;                      // 函数的返回值
    OBJ_HANDLE obj_collecting_handle = 0; // 双因素客户端类的对象的句柄
    json_object *user_name_jso = NULL;
    json_object *client_jso = NULL;
    json_object *user_role_privilege_jso = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    guint8 user_role_priv;
    GSList *input_list = NULL;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    

    
    // 路径有效性
    ret = dal_check_real_path(file_path);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : file real_path:%s is illegal! return %d\r\n", __FUNCTION__, TMP_CONFIG_PEM_FILE,
            ret);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, TMP_CONFIG_PEM_FILE,
            RFACTION_PARAM_CONTENT);
        return VOS_ERR;
    }

    
    (void)chown(TMP_CONFIG_PEM_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)chmod(TMP_CONFIG_PEM_FILE, (S_IRUSR | S_IWUSR));

    
    // 获取类对象的句柄obj_collecting_handle
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_collecting_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR, json_object_clear_string(body_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    // 填写入参
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));

    // 5.  调用UIP函数(方法METHOD_IMPORT_ALL_CONFIG中会删除临时文件)
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_NAME, &user_name_jso);
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_CLIENT, &client_jso);
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_ROLE, &user_role_privilege_jso);
    user_name = dal_json_object_get_str(user_name_jso);
    client = dal_json_object_get_str(client_jso);
    user_role_priv = json_object_get_int(user_role_privilege_jso);
    
    (void)get_element_int(user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, obj_collecting_handle,
        BMC_CLASEE_NAME, METHOD_IMPORT_ALL_CONFIG, AUTH_DISABLE, user_role_priv, input_list, NULL);
    

    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_IMPORT_ALL_CONFIG, ret));

    
    uip_free_gvariant_list(input_list);

    return VOS_OK;
}


LOCAL gint32 get_dumping_progress(gint32 *progress)
{
    GSList *output_list = NULL;
    gint32 ret;
    gint32 status;
    guint8 software_type = 0;

    if (NULL == progress) {
        return VOS_ERR;
    }

// 获取一键收集的状态
    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM != software_type) {
        ret = uip_call_class_method(NULL, NULL, NULL, CLASS_DUMP, OBJECT_DUMP, METHOD_DUMP_GET_DUMP_PHASE, NULL,
            &output_list);
    } else {
        ret = uip_call_class_method(NULL, NULL, NULL, CLASS_DUMP, OBJECT_DUMP, METHOD_DUMP_GET_BOTH_SMM_DUMP_PHASE,
            NULL, &output_list);
    }


    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Get dumping file download progress failed.");
    }

    status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    *progress = status;
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        
        if (SMM_ACTIVE_DUMP_FAILED == *progress || SMM_STDBY_DUMP_FAILED == *progress) {
            *progress = RF_FINISH_PERCENTAGE;
        }
    }
    
    uip_free_gvariant_list(output_list);
    return VOS_OK;
}


LOCAL gint32 get_collecting_progress(gint32 *progress, gchar *err_str, guint32 buffer_len)
{
    int iRet = -1;
    GSList *output_list = NULL;
    const gchar *err_string = NULL;
    gint32 ret;
    gint32 status;

    if (progress == NULL || buffer_len <= 1) {
        return VOS_ERR;
    }

    // 获取配置文件收集的进度
    ret = uip_call_class_method(NULL, NULL, NULL, BMC_CLASEE_NAME, BMC_OBJECT_NAME, METHOD_GET_IMPORT_EXPORT_STATUS,
        NULL, &output_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Get collecting file download progress failed.");
    }

    // 获取调用方法的结果
    status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    err_string = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);

    // 将该结果填充到出参列表中
    *progress = status;
    if ((NULL != err_string) && strlen(err_string) > 0) {
        iRet = snprintf_s(err_str, buffer_len, buffer_len - 1, "%s", err_string);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    
    uip_free_gvariant_list(output_list);

    
    return VOS_OK;
}



LOCAL gint32 get_dumping_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj, guint8 local)
{
    // 变量定义
    gint32 file_down_progress = 0;
    gint32 ret;

    // 获取文件收集的进度
    ret = get_dumping_progress(&file_down_progress);
    return_val_do_info_if_fail(VOS_OK == ret, RF_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = (local ? file_down_progress :
                                                  ((file_down_progress * 10) / 100)); // 文件传输显示为总体进度的10%
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "get dumping packege progress failed"));

    
    ret = parse_dumping_err_code(file_down_progress, message_obj);
    return_val_do_info_if_fail(RF_OK == ret, RF_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = (local ? file_down_progress :
                                                  ((file_down_progress * 10) / 100)); // 文件传输显示为总体进度的10%
        debug_log(DLOG_ERROR, "dumping error occured, error code:%d", file_down_progress));

    
    if ((monitor_fn_data->task_progress != TASK_NO_PROGRESS) &&
        (file_down_progress < monitor_fn_data->task_progress)) {
        debug_log(DLOG_ERROR, "Dumping process error, current: %d, last: %d",
            file_down_progress, monitor_fn_data->task_progress);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        return RF_OK;
    }

    if (RF_FINISH_PERCENTAGE != file_down_progress) {
        
        monitor_fn_data->task_progress =
            local ? file_down_progress : ((file_down_progress * 10) / 100); // 文件传输显示为总体进度的10%
    } else {
        
        json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_EXPORTING_FLAG,
            json_object_new_boolean(TRUE));

        
        
        monitor_fn_data->task_progress = local ? 99 : 10;
        
    }

    return RF_OK;
}



LOCAL gint32 get_collecting_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    // 变量定义
    gint32 file_down_progress = 0;
    gchar  err_str[MAX_ERR_STRING_LENGTH] = {0};
    gint32 ret;

    // 获取文件收集的进度
    ret = get_collecting_progress(&file_down_progress, err_str, sizeof(err_str));
    return_val_do_info_if_fail(VOS_OK == ret, RF_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "get collecting packege progress failed"));

    
    ret = parse_collecting_err_code(file_down_progress, err_str, message_obj);
    return_val_do_info_if_fail(RF_OK == ret, RF_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
        debug_log(DLOG_ERROR, "collecting error occured, error code:%d", file_down_progress));

    
    if ((monitor_fn_data->task_progress != TASK_NO_PROGRESS) &&
        (file_down_progress < monitor_fn_data->task_progress)) {
        debug_log(DLOG_ERROR, "Collecting process error, current: %d, last: %d",
            file_down_progress, monitor_fn_data->task_progress);
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        return RF_OK;
    }

    if (RF_FINISH_PERCENTAGE != file_down_progress) {
        
        monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
    } else {
        
        json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_EXPORTING_FLAG,
            json_object_new_boolean(TRUE));

        
        monitor_fn_data->task_progress = 10; // 文件传输显示为总体进度的10%
    }

    return RF_OK;
}



LOCAL gint32 get_dump_file_transfer_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    // 变量定义
    gint32 file_down_progress = 0;
    gint32 ret;

    // 获取文件上传的状态
    ret = get_file_transfer_progress(&file_down_progress);
    return_val_do_info_if_fail(ret == VOS_OK, RF_FAILED,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = FILE_DOWNLOAD_BASE_RATIO +
        (file_down_progress * FILE_DOWNLOAD_RATIO) / FILE_DOWNLOAD_RATIO_TOTAL; // 文件传输显示为总体进度的90%
        (void)proxy_delete_file(0, NULL, NULL, TMP_DUMP_ORIGINAL_FILE);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "get file packege download progress failed"));

    
    ret = parse_file_transfer_err_code(file_down_progress, message_obj);
    return_val_do_info_if_fail(ret == VOS_OK, RF_FAILED,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = FILE_DOWNLOAD_BASE_RATIO +
        (file_down_progress * FILE_DOWNLOAD_RATIO) / FILE_DOWNLOAD_RATIO_TOTAL; // 文件传输显示为总体进度的90%
        (void)proxy_delete_file(0, NULL, NULL, TMP_DUMP_ORIGINAL_FILE);
        debug_log(DLOG_ERROR, "file transfer error occured, error code:%d, progress is %d", file_down_progress,
        monitor_fn_data->task_progress));

    
    if (file_down_progress != RF_FINISH_PERCENTAGE) {
        
        monitor_fn_data->task_progress = FILE_DOWNLOAD_BASE_RATIO +
            (file_down_progress * FILE_DOWNLOAD_RATIO) / FILE_DOWNLOAD_RATIO_TOTAL; // 文件传输显示为总体进度的90%
    } else {
        // TASK标志位置为RF_TASK_COMPLETED
        monitor_fn_data->task_state = RF_TASK_COMPLETED;

        // 更新总体ACTION的进度(PERCENTAGE)
        file_down_progress = RF_FINISH_PERCENTAGE;

        // POSTMAN显示action的进度
        monitor_fn_data->task_progress = file_down_progress;

        // 删除dump_info.tar.gz 在/tmp目录下的临时文件
        (void)proxy_delete_file(0, NULL, NULL, TMP_DUMP_ORIGINAL_FILE);

        // 显示成功的消息
        (void)create_message_info(MSGID_DUMPING_OK, NULL, message_obj);
    }

    return RF_OK;
}



LOCAL gint32 get_config_file_transfer_status(TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    // 变量定义
    gint32 file_down_progress = 0;
    gint32 ret;

    // 获取文件上传的状态
    ret = get_file_transfer_progress(&file_down_progress);
    return_val_do_info_if_fail(ret == VOS_OK, RF_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = INVALID_DATA_BYTE; vos_rm_filepath(TMP_CONFIG_ORIGINAL_FILE);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "get file packege download progress failed"));

    
    ret = parse_file_transfer_err_code(file_down_progress, message_obj);
    return_val_do_info_if_fail(ret == RF_OK, RF_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = INVALID_DATA_BYTE; (void)vos_rm_filepath(TMP_CONFIG_ORIGINAL_FILE);
        debug_log(DLOG_ERROR, "file transfer error occured, error code:%d, progress is %d", file_down_progress,
        monitor_fn_data->task_progress));

    
    if (file_down_progress != RF_FINISH_PERCENTAGE) {
        
        monitor_fn_data->task_progress = FILE_DOWNLOAD_BASE_RATIO +
            (file_down_progress * FILE_DOWNLOAD_RATIO) / FILE_DOWNLOAD_RATIO_TOTAL; // 文件传输显示为总体进度的90%
    } else {
        // 更新导出标识
        monitor_fn_data->task_state = RF_TASK_COMPLETED;

        // 更新总体ACTION的进度(PERCENTAGE)
        file_down_progress = RF_FINISH_PERCENTAGE;

        // POSTMAN显示action的进度
        monitor_fn_data->task_progress = file_down_progress;

        // 删除配置文件在/tmp目录下的临时文件
        (void)vos_rm_filepath(TMP_CONFIG_ORIGINAL_FILE); // 删除/dev/shm/config.xml
        (void)vos_rm_filepath(TMP_CONFIG_PEM_FILE);      // 删除/tmp/config.xml

        // 显示导入成功的消息
        (void)create_message_info(MSGID_COLLECTING_CONFIG_OK, NULL, message_obj);
    }

    return RF_OK;
}


LOCAL void export_monitor_info_free_func(void *monitor_data)
{
    json_object *jso = (json_object *)monitor_data;

    (void)json_object_put(jso);

    return;
}


LOCAL TASK_MONITOR_INFO_S *export_monitor_info_new(PROVIDER_PARAS_S *i_paras)
{
    TASK_MONITOR_INFO_S *monitor_info;
    json_object *user_data_jso = NULL;
    gint32 file_type = 0;

    monitor_info = task_monitor_info_new((GDestroyNotify)export_monitor_info_free_func);
    
    if (NULL == monitor_info) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }

    

    user_data_jso = json_object_new_object();
    if (NULL == user_data_jso) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info);
        return NULL;
    }

    // 待导入的文件的类型
    if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) {
        file_type = RF_EXPORT_FILE_DUMP;
    } else if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) {
        file_type = RF_EXPORT_FILE_CONFIG;
    }

    
    // 1、配置用户名,IP信息,用户权限，触发文件的导入方法时需要；
    // 2、将升级中标识初始化置为FALSE；文件传输完毕后升级标识置为TRUE
    json_object_object_add(user_data_jso, RF_EXPORTING_FLAG, json_object_new_boolean(FALSE));
    json_object_object_add(user_data_jso, RF_IMPORT_FILE_TYPE,
        json_object_new_int(file_type)); // 标示文件的类型，dump或者config.xml
    json_object_object_add(user_data_jso, RF_IMPORT_USER_NAME, json_object_new_string(i_paras->user_name)); // 用户名
    json_object_object_add(user_data_jso, RF_IMPORT_USER_CLIENT, json_object_new_string(i_paras->client));  // 用户IP
    json_object_object_add(user_data_jso, RF_SESSION_ID, json_object_new_string(i_paras->session_id)); // session id
    json_object_object_add(user_data_jso, RF_IMPORT_USER_ROLE,
        json_object_new_int(i_paras->user_role_privilege)); // user_role_privilegeid
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG, json_object_new_int(i_paras->is_from_webui));
    json_object_object_add(user_data_jso, RF_AUTH_TYPE, json_object_new_int(i_paras->auth_type));
    // 给user_data 分配新值
    monitor_info->user_data = user_data_jso;

    return monitor_info;
}


LOCAL void import_manager_monitor_info_free_func(void *monitor_data)
{
    json_object *jso = (json_object *)monitor_data;

    (void)json_object_put(jso);

    return;
}


LOCAL TASK_MONITOR_INFO_S *import_manager_monitor_info_local(PROVIDER_PARAS_S *i_paras)
{
    TASK_MONITOR_INFO_S *monitor_info;
    json_object *user_data_jso = NULL;
    monitor_info = task_monitor_info_new((GDestroyNotify)import_manager_monitor_info_free_func);
    
    if (NULL == monitor_info) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }

    

    user_data_jso = json_object_new_object();
    if (NULL == user_data_jso) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info);
        return NULL;
    }

    
    // 1、配置用户名,IP信息,用户权限，触发文件的导入方法时需要；
    // 2、将升级中标识初始化置为FALSE；文件传输完毕后升级标识置为TRUE
    json_object_object_add(user_data_jso, RF_IMPORTING_FLAG, json_object_new_boolean(FALSE));
    json_object_object_add(user_data_jso, RF_IMPORT_USER_NAME, json_object_new_string(i_paras->user_name)); // 用户名
    json_object_object_add(user_data_jso, RF_IMPORT_USER_CLIENT, json_object_new_string(i_paras->client));  // 用户IP
    json_object_object_add(user_data_jso, RF_SESSION_ID, json_object_new_string(i_paras->session_id)); // session id
    json_object_object_add(user_data_jso, RF_IMPORT_USER_ROLE,
        json_object_new_int(i_paras->user_role_privilege)); // user_role_privilegeid

    // 给user_data 分配新值
    monitor_info->user_data = user_data_jso;

    return monitor_info;
}

LOCAL gint32 get_manager_provider_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 provider_health = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret =
        dal_get_specific_object_byte(CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE, COMPONENT_TYPE_BMC, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_HEALTH, &provider_health);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get health status fail.\n", __FUNCTION__, __LINE__));

    
    (void)get_resource_status_property(&provider_health, NULL, STRING_CONSET_VALUE_ENABLE, o_result_jso, FALSE);
    return HTTP_OK;
}


LOCAL TASK_MONITOR_INFO_S *import_manager_monitor_info_uri(PROVIDER_PARAS_S *i_paras)
{
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    json_object *user_data_jso = NULL;
    monitor_info = task_monitor_info_new((GDestroyNotify)import_manager_monitor_info_free_func);

    
    return_val_do_info_if_expr(NULL == monitor_info, NULL, debug_log(DLOG_ERROR, "task_monitor_info_new failed"));
    

    user_data_jso = json_object_new_object();
    if (NULL == user_data_jso) {
        debug_log(DLOG_ERROR, "alloc a new json object failed");
        task_monitor_info_free(monitor_info);
        return NULL;
    }

    
    // 1、配置用户名,IP信息,用户权限，触发文件的导入方法时需要；
    // 2、将升级中标识初始化置为FALSE；文件传输完毕后升级标识置为TRUE
    
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int(i_paras->is_from_webui)); // 请求是否来自WEB
    
    json_object_object_add(user_data_jso, RF_IMPORTING_FLAG, json_object_new_boolean(FALSE));
    json_object_object_add(user_data_jso, RF_IMPORT_USER_NAME, json_object_new_string(i_paras->user_name)); // 用户名
    json_object_object_add(user_data_jso, RF_IMPORT_USER_CLIENT, json_object_new_string(i_paras->client));  // 用户IP
    json_object_object_add(user_data_jso, RF_SESSION_ID, json_object_new_string(i_paras->session_id)); // session id
    json_object_object_add(user_data_jso, RF_IMPORT_USER_ROLE,
        json_object_new_int(i_paras->user_role_privilege)); // user_role_privilegeid

    // 给user_data 分配新值
    monitor_info->user_data = user_data_jso;

    return monitor_info;
}


gint32 copy_tmp_to_des(const gchar *original_file_path, const gchar *dest_file_path, json_object *user_data,
    json_object **o_message_jso)
{
    
    gint32 ret;

    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    return_val_do_info_if_expr((original_file_path == NULL) || (dest_file_path == NULL) || (user_data == NULL) ||
        (o_message_jso == NULL),
        VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    
    // 1.校验/tmp/dump.tar.gz 中的dump.tar.gz是否存在
    ret = dal_check_real_path(original_file_path);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, original_file_path,
            RFACTION_PARAM_CONTENT);
        return VOS_ERR;
    }

    // 2.拷贝/tmp/dump_info.tar.gz 到指定的文件夹dest_file_path
    // redfish模块降权限后调用代理方法拷贝文件
    (void)get_element_str(user_data, RF_IMPORT_USER_NAME, &user_name); // 用户名
    (void)get_element_str(user_data, RF_IMPORT_USER_CLIENT, &client);  // 用户IP
    (void)get_element_int(user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    // redfish模块降权限后，调用代理方法拷贝文件
    ret = proxy_copy_file((guchar)from_webui_flag, user_name, client, original_file_path, dest_file_path,
        REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: copy request file to %s failed", __FUNCTION__, dest_file_path);
        (void)proxy_delete_file((guchar)from_webui_flag, user_name, client, original_file_path);
        (void)proxy_delete_file((guchar)from_webui_flag, user_name, client, dest_file_path);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, dest_file_path, RFACTION_PARAM_CONTENT);
    }

    return ret;
}


LOCAL gint32 export_status_monitor_local(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    int iRet = -1;
    
    gint32 ret = 0;
    json_object *exporting_flag_jso = NULL;
    json_object *content_jso = NULL;
    json_object *file_type_jso = NULL;
    json_bool exporting_flag;
    gint32 file_type;
    const gchar *dest_file_path = NULL;
    gchar            temp_file_path[MAX_FILE_PATH_LENGTH] = {0}; // 数组长度为256
    const gchar *user_name = NULL;
    gint32 auth_type = LOG_TYPE_LOCAL;

    
    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != monitor_fn_data) && (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));
    
    (void)json_object_object_get_ex(body_jso, RFACTION_PARAM_CONTENT, &content_jso);
    dest_file_path = dal_json_object_get_str(content_jso);
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_EXPORTING_FLAG,
        &exporting_flag_jso);
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_FILE_TYPE, &file_type_jso);
    file_type = json_object_get_int(file_type_jso); // 文件类型

    
    exporting_flag = json_object_get_boolean(exporting_flag_jso); // 标志位

    
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    
    // 根据一键收集的状态进行后续的操作

    if (FALSE == exporting_flag) { // 标明dump.tar.gz 未完成，下一步要进行dump.tar.gz的上传
        if (file_type == RF_EXPORT_FILE_DUMP) {
            // 获取一键收集的状态,并根据状态进行文件的(同时更新POSTMAN 的进度)
            (void)get_dumping_status(monitor_fn_data, message_obj, 1);
        }

        if (file_type == RF_EXPORT_FILE_CONFIG) {
            // 获取配置文件收集的状态,并根据状态进行文件的后续操作(更新exporting_flag 同时更新POSTMAN 的进度)
            (void)get_collecting_status(monitor_fn_data, message_obj);
        }
    } else { // 一键收集或者配置文件收集完成
        // 形成临时文件的文件名
        if (file_type == RF_EXPORT_FILE_DUMP) {
            iRet =
                snprintf_s(temp_file_path, sizeof(temp_file_path), sizeof(temp_file_path) - 1, TMP_DUMP_ORIGINAL_FILE);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        }

        // 形成临时文件的文件名
        if (file_type == RF_EXPORT_FILE_CONFIG) {
            iRet = snprintf_s(temp_file_path, sizeof(temp_file_path), sizeof(temp_file_path) - 1, TMP_CONFIG_PEM_FILE);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        }

        // 将临时文件temp_file_path拷贝至指定的BMC环境的路径des_file_path

        
        if (0 != g_strcmp0(temp_file_path, dest_file_path)) {
            ret = copy_tmp_to_des(temp_file_path, dest_file_path, (json_object *)(monitor_fn_data->user_data),
                message_obj);
        } else {
            ret = VOS_OK;
        }

        
        // 更新文件属主与权限
        if (ret == VOS_OK) {
            (void)get_element_str((json_object *)(monitor_fn_data->user_data), RF_IMPORT_USER_NAME, &user_name);
            (void)get_element_int((json_object *)(monitor_fn_data->user_data), RF_AUTH_TYPE, &auth_type);
            ret = dal_change_file_owner(auth_type, dest_file_path, user_name, S_IRUSR | S_IWUSR);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "change file owner failed,ret is %d", ret);
                (void)proxy_delete_file(0, NULL, NULL, dest_file_path);
            }
        }

        if (ret == VOS_ERR) { // VOS_ERR 标明文件传输的初始化出错
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            
            monitor_fn_data->task_progress = 99; // 如果copy文件失败，进度和刚收集完一样
            
        } else {
            // 更新任务状态
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE; // 文件传输显示为总体进度的100%

            // 显示导出成功
            if (file_type == RF_EXPORT_FILE_DUMP) {
                (void)create_message_info(MSGID_DUMPING_OK, NULL, message_obj);
            }

            if (file_type == RF_EXPORT_FILE_CONFIG) {
                (void)create_message_info(MSGID_COLLECTING_CONFIG_OK, NULL, message_obj);
            }
        }
    }

    // 文件返回
    return RF_OK;
}


LOCAL gint32 import_status_monitor_uri(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    
    gint32 ret;
    gint32 file_down_progress = 0;
    json_object *importing_flag_jso = NULL;
    json_object *content_jso = NULL;
    gint32 import_ret = 0;
    json_bool importing_flag;
    gchar         file_path[MAX_URI_LENGTH] = {0};
    gchar         err_str[MAX_ERR_STRING_LENGTH] = {0}; // config.xml 文件导入的返回的错误提示字符串
    gchar *pstr = NULL;

    
    return_val_do_info_if_fail((origin_obj_path != NULL) && (monitor_fn_data != NULL) && (message_obj != NULL),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORTING_FLAG,
        &importing_flag_jso);
    (void)json_object_object_get_ex(body_jso, RFACTION_PARAM_CONTENT, &content_jso);

    
    importing_flag = json_object_get_boolean(importing_flag_jso);

    
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    
    if (importing_flag == FALSE) {
        ret = get_file_transfer_progress(&file_down_progress);
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
            debug_log(DLOG_ERROR, "get file packege download progress failed"));

        
        ret = parse_file_transfer_err_code(file_down_progress, message_obj);
        return_val_do_info_if_fail(ret == RF_OK, RF_OK,
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
            debug_log(DLOG_ERROR, "file transfer error occured, error code:%d", file_down_progress));

        
        if (file_down_progress != RF_FINISH_PERCENTAGE) {
            
            monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
            return RF_OK;
        } else {
            
            // 形成config.xml 下载之后的绝对路径
            pstr = g_strrstr(dal_json_object_get_str(content_jso), "/");
            if (snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "/tmp%s", pstr) <= 0) {
                monitor_fn_data->task_state = RF_TASK_EXCEPTION;
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
                return RF_OK;
            }

            // 调用导入证书的方法
            import_ret = importing_configuration_in_task((json_object *)(monitor_fn_data->user_data), body_jso,
                file_path, message_obj);
            if (import_ret == VOS_ERR) {
                // 错误信息在函数importing_configuration_in_task()中产生
                monitor_fn_data->task_state = RF_TASK_EXCEPTION;
                return RF_OK;
            }

            // 文件导入完成，将导入标识位置成true;同时记录错误返回码方便importing_flag == TRUE 时使用
            importing_flag = TRUE;
            json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_IMPORTING_FLAG,
                json_object_new_boolean(importing_flag));

            // 更新总体ACTION的进度(PERCENTAGE)
            file_down_progress = 10;

            // POSTMAN显示action的进度
            monitor_fn_data->task_progress = file_down_progress;
        }
    } else {
        
        // 形成config.xml 下载之后的绝对路径(用于文件导入失败后的临时文件的删除)
        pstr = g_strrstr(dal_json_object_get_str(content_jso), "/");
        if (snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "/tmp%s", pstr) <= 0) {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            return RF_OK;
        }

        // 获取文件导入的进度
        ret = get_collecting_progress(&file_down_progress, err_str, sizeof(err_str));
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit, (void)proxy_delete_file(0, NULL, NULL, file_path);
            debug_log(DLOG_ERROR, "get collecting packege progress failed"));

        
        ret = parse_collecting_err_code(file_down_progress, err_str, message_obj);
        return_val_do_info_if_fail(ret == RF_OK, RF_OK,
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            monitor_fn_data->task_progress = 10 + (file_down_progress * 90) / 100; // 文件传输显示为总体进度的10%
            (void)proxy_delete_file(0, NULL, NULL, file_path);
            debug_log(DLOG_ERROR, "collecting error occured, error code:%d", file_down_progress));

        
        if (file_down_progress != RF_FINISH_PERCENTAGE) {
            
            monitor_fn_data->task_progress = 10 + (file_down_progress * 90) / 100; // 文件传输显示为总体进度的%
        } else {
            // 更新导出标识
            monitor_fn_data->task_state = RF_TASK_COMPLETED;

            // 更新总体ACTION的进度(PERCENTAGE)
            file_down_progress = RF_FINISH_PERCENTAGE;

            // POSTMAN显示action的进度
            monitor_fn_data->task_progress = file_down_progress;

            // 临时文件的删除由配置文件的导入方法完成

            // 填充导入成功的消息
            (void)create_message_info(MSGID_IMPORT_CONFIG_OK, NULL, message_obj);
        }
    }

    // 文件返回
    return RF_OK;

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_OK; 
}


LOCAL gint32 import_status_monitor_local(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    
    gint32 ret;
    gint32 file_down_progress = 0;
    json_object *content_jso = NULL;
    gchar         err_str[MAX_ERR_STRING_LENGTH] = {0}; // 错误字符串

    
    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != monitor_fn_data) && (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    
    (void)json_object_object_get_ex(body_jso, RFACTION_PARAM_CONTENT, &content_jso);

    
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    

    // 获取config.xml的进度
    ret = get_collecting_progress(&file_down_progress, err_str, sizeof(err_str));
    return_val_do_info_if_fail(VOS_OK == ret, RF_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = file_down_progress; // 文件传输显示为总体进度的10%
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "get importing packege progress failed"));

    
    ret = parse_collecting_err_code(file_down_progress, err_str, message_obj);
    return_val_do_info_if_fail(RF_OK == ret, RF_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        monitor_fn_data->task_progress = file_down_progress; // 文件传输显示为总体进度的10%
        debug_log(DLOG_ERROR, "importing error occured, error code:%d", file_down_progress));

    
    if (RF_FINISH_PERCENTAGE != file_down_progress) {
        
        monitor_fn_data->task_progress = file_down_progress; // 文件传输显示为总体进度的10%
    } else {
        // 更新TASK 的状态
        monitor_fn_data->task_state = RF_TASK_COMPLETED;

        // 更新总体ACTION的进度(PERCENTAGE)
        file_down_progress = RF_FINISH_PERCENTAGE;

        // POSTMAN显示action的进度
        monitor_fn_data->task_progress = file_down_progress;

        // 填充导入成功的消息
        (void)create_message_info(MSGID_IMPORT_CONFIG_OK, NULL, message_obj);
    }

    
    return RF_OK;
}

LOCAL void clear_sensitive_string(json_object *body_jso, REDFISH_TASK_STATE task_state, const gchar *prop_name)
{
    if (body_jso == NULL) {
        return;
    }

    if (task_state == RF_TASK_EXCEPTION || task_state == RF_TASK_COMPLETED || task_state == RF_TASK_KILLED) {
        json_object_clear_string(body_jso, prop_name);
    }
    return;
}


static gint32 export_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    int iRet = -1;
    
    gint32 ret = 0;
    json_object *exporting_flag_jso = NULL;
    json_object *file_type_jso = NULL;
    json_object *user_name_jso = NULL;
    json_object *client_jso = NULL;
    json_object *content_jso = NULL;
    json_object *session_id_jso = NULL;
    gint32 file_type;
    gint32 file_id = 0;
    json_bool exporting_flag;
    gchar            file_path[MAX_URI_LENGTH] = { 0 }; // 数组长度为512
    const gchar *transfer_protocol = NULL;
    PROVIDER_PARAS_S i_paras;           // 用来作为file_uploading的入参
    const gchar *user_name_str = NULL;  // 用户名
    const gchar *client_str = NULL;     // 客户端
    const gchar *session_id_str = NULL; // 当前会话
    gint32 is_from_webui;
    json_object *webui_flag_jso = NULL;

    
    return_val_do_info_if_fail((origin_obj_path != NULL) && (monitor_fn_data != NULL) && (message_obj != NULL),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));
    
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_EXPORTING_FLAG,
        &exporting_flag_jso);
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_FILE_TYPE, &file_type_jso);
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_USER_NAME,
        &user_name_jso); // user_name
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_USER_CLIENT,
        &client_jso); // client
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_SESSION_ID,
        &session_id_jso); // session id
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_USERDATA_FROM_WEBUI_FLAG,
        &webui_flag_jso); // is_from_webui
    (void)json_object_object_get_ex(body_jso, RFACTION_PARAM_CONTENT, &content_jso);

    // 初始化传输协议
    transfer_protocol = dal_json_object_get_str(content_jso);

    // 初始化i_paras
    (void)memset_s(&i_paras, sizeof(PROVIDER_PARAS_S), 0, sizeof(PROVIDER_PARAS_S));
    user_name_str = dal_json_object_get_str(user_name_jso);
    client_str = dal_json_object_get_str(client_jso);
    session_id_str = dal_json_object_get_str(session_id_jso);
    is_from_webui = json_object_get_int(webui_flag_jso);
    (void)strncpy_s(i_paras.user_name, sizeof(i_paras.user_name), user_name_str, sizeof(i_paras.user_name) - 1);
    (void)strncpy_s(i_paras.client, sizeof(i_paras.client), client_str, sizeof(i_paras.client) - 1);
    (void)strncpy_s(i_paras.session_id, sizeof(i_paras.session_id), session_id_str, sizeof(i_paras.session_id) - 1);

    
    exporting_flag = json_object_get_boolean(exporting_flag_jso);

    
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    
    // 获取文件类型
    file_type = json_object_get_int(file_type_jso); // 文件类型

    if (exporting_flag == FALSE) {
        // 进行一键收集
        if (file_type == RF_EXPORT_FILE_DUMP) {
            // 获取一键收集的状态,并根据状态进行文件的(同时更新POSTMAN 的进度)
            (void)get_dumping_status(monitor_fn_data, message_obj, 0);
        }

        // 进行Config.xml
        if (file_type == RF_EXPORT_FILE_CONFIG) {
            // 获取收集config文件的状态(POSTMAN 的文件传输的进度 也在该方法中更新 )
            (void)get_collecting_status(monitor_fn_data, message_obj);
        }

        // 状态判断
        (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_EXPORTING_FLAG,
            &exporting_flag_jso);
        exporting_flag = json_object_get_boolean(exporting_flag_jso);
        // 标明dump.tar.gz 或者config.xml 收集已完成，下一步要进行dump.tar.gz 或者config.xml的上传
        if (exporting_flag == TRUE) {
            
            // 1.进行文件的上传的参数初始化
            file_id = FILE_ID_DUMP;
            iRet = snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, TMP_DUMP_ORIGINAL_FILE);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

            if (file_type == RF_EXPORT_FILE_CONFIG) {
                file_id = FILE_ID_CONFIG;
                iRet = snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, TMP_CONFIG_ORIGINAL_FILE);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
                ret = proxy_modify_file_mode(is_from_webui, user_name_str, client_str, TMP_CONFIG_PEM_FILE,
                    REDFISH_USER_UID, REDFISH_USER_GID);
                return_val_do_info_if_expr(VOS_OK != ret, RF_FAILED,
                    (void)proxy_delete_file(is_from_webui, user_name_str, client_str, TMP_CONFIG_PEM_FILE);
                    debug_log(DLOG_ERROR, "%s: modify file %s mode failed", __FUNCTION__, TMP_CONFIG_PEM_FILE));
                (void)vos_file_copy(TMP_CONFIG_ORIGINAL_FILE, TMP_CONFIG_PEM_FILE); // 复制临时文件到配置文件默认的路径
                (void)proxy_delete_file(is_from_webui, user_name_str, client_str,
                    TMP_CONFIG_PEM_FILE); // 不管是否拷贝成功，都应删除临时文件
            }

            // 2.使用文件上传函数
            ret = file_uploading(&i_paras, transfer_protocol, file_id, file_path,
                message_obj); // 文件上传执行的具体结果由TASK进行解析，并显示
            if (ret == VOS_ERR) {
                monitor_fn_data->task_state = RF_TASK_EXCEPTION;
                json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
                return RF_OK;
            }

            // 3.成功调用文件传输的接口
            // 3.1 更新POSTMAN的进度
            monitor_fn_data->task_progress = FILE_DOWNLOAD_BASE_RATIO; // 文件传输显示为总体进度的90%

            // 3.2 更新文件导出标志位
            exporting_flag = TRUE;
            json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_EXPORTING_FLAG,
                json_object_new_boolean(exporting_flag));
        }
    } else {
        // dump.tar.gz 的上传状态
        if (file_type == FILE_ID_DUMP) {
            // 获取dump文件传输的状态(POSTMAN 的文件传输的进度 也在该方法中更新)
            (void)get_dump_file_transfer_status(monitor_fn_data, message_obj);
        }

        // Config.xml  的上传状态
        if (file_type == RF_EXPORT_FILE_CONFIG) {
            // 获取config文件传输的状态(POSTMAN 的文件传输的进度 也在该方法中更新)
            (void)get_config_file_transfer_status(monitor_fn_data, message_obj);
        }
    }

    
    clear_sensitive_string(body_jso, monitor_fn_data->task_state, RFACTION_PARAM_CONTENT);

    // 文件返回
    return RF_OK;
}


LOCAL gint32 act_manager_export_file_local(PROVIDER_PARAS_S *i_paras, guint8 opt_priv, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    
    const gchar *local_file_path = NULL;
    json_object *body_jso = NULL;
    gint32 ret = 0;
    TASK_MONITOR_INFO_S *export_monitor_data = NULL;
    gchar     task_name[MAX_DES_LENGTH] = {0}; // 任务的描述
    
    gboolean b_ret;
    

    
    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    
    body_jso = i_paras->val_jso;
    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &local_file_path);
    goto_label_do_info_if_fail((NULL != local_file_path), err_exit,
        debug_log(DLOG_ERROR, "cann't get image uri or transfer protocol"));

    
    // 3.1 任务重复性检查
    b_ret = check_redfish_running_task_exist(export_status_monitor_local, i_paras->val_jso, i_paras->uri, NULL);
    if (FALSE != b_ret) {
        // 日志打印
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);

        // 判断任务重复的类型
        if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) { // 一键收集任务重复
            (void)create_message_info(MSGID_TASK_DUMP_EXPORT_ERR, NULL, o_message_jso);
        } else if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) { // 配置文件导出任务重复
            (void)create_message_info(MSGID_TASK_CONFIG_EXPORT_ERR, NULL, o_message_jso);
        }

        // 返回结果
        return HTTP_BAD_REQUEST;
    }

    

    
    if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) { // 一键收集，形成dump文件
        // 进行一键收集
        ret = dumping(i_paras, o_message_jso); // 执行的具体结果由TASK进行解析，并显示

        // 更新任务描述
        iRet = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, RF_EXPORT_EXPORT_DUMP_TASK_NAME);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) { // congfig.xml 拷贝至指定的BMC目录
        // 进行配置文件收集
        ret = collecting(i_paras, o_message_jso); // 执行的具体结果由TASK进行解析，并显示

        // 更新任务描述
        iRet = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, RF_EXPORT_EXPORT_CONFIG_TASK_NAME);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    
    if (ret != VOS_OK) {
        
        // 对一键收集的错误码进行解析
        if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) { // 一键收集，形成dump文件
            if (ret == MD_EPARA) {
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
            } else if (ret == MD_EBUSY) {
                (void)create_message_info(MSGID_TASK_DUMP_EXPORT_ERR, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }
            
            else if (ret == MD_EONUPGRADE) {
                (void)create_message_info(MSGID_TASK_DUMP_EXPORT_UPGRADING_ERR, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            } else if (ret == MD_ENOSPACE) {
                (void)create_message_info(MSGID_TASK_DUMP_EXPORT_NO_MEMORY_ERR, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }

            
        }

        // 对配置文件的错误码进行解析
        if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) { // 配置文件收集
            (void)create_message_info(MSGID_TASK_CONFIG_EXPORT_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        
    }

    
    // 初始化TASK的入参
    export_monitor_data = export_monitor_info_new(i_paras);
    goto_label_do_info_if_fail(NULL != export_monitor_data, err_exit,
        debug_log(DLOG_ERROR, "alloc a new import monitor info failed"));

    // TASK的总体进度
    export_monitor_data->task_progress = TASK_NO_PROGRESS; 
    export_monitor_data->rsc_privilege = opt_priv;

    // 创建TASK
    // body_jso 中的内容就是 i_paras->val_jso;

    ret = create_new_task((const gchar *)task_name, export_status_monitor_local, export_monitor_data, body_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(export_monitor_data);
        debug_log(DLOG_ERROR, "create new task failed"));

    
    // 创建TASK 正常
    return HTTP_ACCEPTED;

err_exit:
    ret = HTTP_INTERNAL_SERVER_ERROR;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return ret;
}


LOCAL gint32 act_manager_import_file_local(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    const gchar *local_file_path = NULL;
    json_object *body_jso = NULL;
    gint32 ret;
    TASK_MONITOR_INFO_S *import_monitor_data = NULL;
    
    gboolean b_ret;
    

    
    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    
    body_jso = i_paras->val_jso;
    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &local_file_path);
    goto_label_do_info_if_fail((NULL != local_file_path), err_exit,
        debug_log(DLOG_ERROR, "cann't get image uri or transfer protocol"));

    
    // 3.1 任务重复性检查
    b_ret = check_redfish_running_task_exist(import_status_monitor_local, i_paras->val_jso, i_paras->uri, NULL);
    if (FALSE != b_ret) {
        // 日志打印
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);

        // 创建消息
        (void)create_message_info(MSGID_TASK_CONFIG_IMPORT_ERR, NULL, o_message_jso);

        // 返回结果
        return HTTP_BAD_REQUEST;
    }

    

    
    // 进行配置文件congfig.xml的导入
    ret = importing_configuration(i_paras, o_message_jso); // 执行的具体结果由TASK进行解析，并显示
    
    if (ret != VOS_OK) {
        
        (void)create_message_info(MSGID_TASK_CONFIG_IMPORT_ERR, NULL, o_message_jso);
        

        return HTTP_BAD_REQUEST;
    }

    
    // 初始化TASK的入参
    import_monitor_data = import_manager_monitor_info_local(i_paras);
    goto_label_do_info_if_fail(NULL != import_monitor_data, err_exit,
        debug_log(DLOG_ERROR, "alloc a new import monitor info failed"));

    // 7. TASK的总体进度
    import_monitor_data->task_progress = TASK_NO_PROGRESS; 
    import_monitor_data->rsc_privilege = USERROLE_USERMGNT;

    // 8. 创建TASK
    // body_jso 中的内容就是 i_paras->val_jso;
    ret = create_new_task(RF_IMPORT_CONFIG_TASK_DESC, import_status_monitor_local, import_monitor_data, body_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(import_monitor_data);
        debug_log(DLOG_ERROR, "create new task failed"));

    
    // 创建TASK 正常
    return HTTP_ACCEPTED;

err_exit:
    ret = HTTP_INTERNAL_SERVER_ERROR;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return ret;
}


LOCAL gint32 act_manager_export_file_uri(PROVIDER_PARAS_S *i_paras, guint8 opt_priv, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    
    const gchar *transfer_protocol = NULL;
    json_object *body_jso = NULL;
    gint32 ret = 0;
    TASK_MONITOR_INFO_S *export_monitor_data = NULL;
    gchar                   task_name[MAX_DES_LENGTH] = { 0 }; // 任务的描述
    
    gboolean b_ret;
    

    
    goto_label_do_info_if_fail((i_paras != NULL) && (o_message_jso != NULL) && (o_result_jso != NULL) &&
        (i_paras->val_jso != NULL),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    
    body_jso = i_paras->val_jso;
    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &transfer_protocol);
    goto_label_do_info_if_fail((transfer_protocol != NULL), err_exit,
        debug_log(DLOG_ERROR, "cann't get image uri or transfer protocol"));

    // 3.1 支持五种文件传输协议
    if (is_valid_file_transfer_protocol(transfer_protocol) == FALSE) {
        

        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT);

        
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }

    
    // 3.2 任务重复性检查
    b_ret = check_redfish_running_task_exist(export_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    if (b_ret != FALSE) {
        // 日志打印
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);

        // 判断任务重复的类型
        if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) { // 一键收集任务重复
            (void)create_message_info(MSGID_TASK_DUMP_EXPORT_ERR, NULL, o_message_jso);
        } else if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) { // 配置文件导出任务重复
            (void)create_message_info(MSGID_TASK_CONFIG_EXPORT_ERR, NULL, o_message_jso);
        }

        // 返回结果
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }

    

    
    if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) { // 一键收集，形成dump文件
        // 进行一键收集
        ret = dumping(i_paras, o_message_jso); // 执行的具体结果由TASK进行解析，并显示

        // 更新任务描述
        iRet = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, RF_EXPORT_EXPORT_DUMP_TASK_NAME);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    } else if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) { // 上传config.xml
        // 将config.xml 从/dev/shm/config.xml  拷贝到临时文件路径

        // 进行配置文件收集
        ret = collecting(i_paras, o_message_jso); // 执行的具体结果由TASK进行解析，并显示

        // 更新任务描述
        iRet = snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, RF_EXPORT_EXPORT_CONFIG_TASK_NAME);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    if (ret != VOS_OK) {
        // 相应的消息在函数()或者collecting()中显示
        
        // 对一键收集的错误码进行解析
        if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_DUMP_NAME)) { // 一键收集，形成dump文件
            if (ret == MD_EPARA) {
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
                json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
                return HTTP_INTERNAL_SERVER_ERROR;
            } else if (ret == MD_EBUSY) {
                (void)create_message_info(MSGID_TASK_DUMP_EXPORT_ERR, NULL, o_message_jso);
                json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
                return HTTP_BAD_REQUEST;
            } else if (ret == MD_EONUPGRADE) {
                
                (void)create_message_info(MSGID_TASK_DUMP_EXPORT_UPGRADING_ERR, NULL, o_message_jso);
                json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
                return HTTP_BAD_REQUEST;
            } else if (ret == MD_ENOSPACE) {
                (void)create_message_info(MSGID_TASK_DUMP_EXPORT_NO_MEMORY_ERR, NULL, o_message_jso);
                json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
                return HTTP_BAD_REQUEST;
            }

            
        }

        // 对配置文件的错误码进行解析
        if (g_strrstr(i_paras->uri, RF_EXPORT_ACTION_EXPORT_CONFIG_NAME)) { // 配置文件收集
            (void)create_message_info(MSGID_TASK_CONFIG_EXPORT_ERR, NULL, o_message_jso);
            json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }
    }

    
    // 初始化TASK的入参
    export_monitor_data = export_monitor_info_new(i_paras);
    goto_label_do_info_if_fail(export_monitor_data != NULL, err_exit,
        debug_log(DLOG_ERROR, "alloc a new import monitor info failed"));

    // 是否正在导入标识，文件下载完后置位
    export_monitor_data->task_progress = TASK_NO_PROGRESS; 
    export_monitor_data->rsc_privilege = opt_priv;

    // 创建TASK
    // body_jso 中的内容就是 i_paras->val_jso;
    ret = create_new_task((const gchar *)task_name, export_status_monitor, export_monitor_data, body_jso, i_paras->uri,
        o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(export_monitor_data);
        debug_log(DLOG_ERROR, "create new task failed"));

    
    return HTTP_ACCEPTED;

err_exit:

    ret = HTTP_INTERNAL_SERVER_ERROR;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    if (i_paras != NULL) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}



LOCAL gint32 act_manager_import_file_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    const gchar *transfer_protocol = NULL;
    json_object *body_jso = NULL;
    gint32 ret;
    guint8 file_id; // 文件的id
    TASK_MONITOR_INFO_S *import_monitor_data = NULL;
    
    gboolean b_ret;
    

    
    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    
    body_jso = i_paras->val_jso;
    (void)get_element_str(body_jso, RFACTION_PARAM_CONTENT, &transfer_protocol);
    goto_label_do_info_if_fail((NULL != transfer_protocol), err_exit,
        debug_log(DLOG_ERROR, "cann't get image uri or transfer protocol"));

    // 3.1 支持五种文件传输协议
    if (is_valid_file_transfer_protocol(transfer_protocol) == FALSE) {
        

        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);

        

        return HTTP_BAD_REQUEST;
    }

    
    // 3.2 任务重复性检查
    b_ret = check_redfish_running_task_exist(import_status_monitor_uri, i_paras->val_jso, i_paras->uri, NULL);
    if (FALSE != b_ret) {
        // 日志打印
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);

        // 创建消息
        (void)create_message_info(MSGID_TASK_CONFIG_IMPORT_ERR, NULL, o_message_jso);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);

        // 返回结果
        return HTTP_BAD_REQUEST;
    }

    

    
    file_id = FILE_ID_CONFIG;
    ret = file_downloading(i_paras, transfer_protocol, file_id, o_message_jso);
    if (ret == VOS_ERR) {
        // file_downloading()中形成文件下载的路径失败
        debug_log(DLOG_ERROR, "%s file_downloading fail", __FUNCTION__);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }

    
    // 初始化TASK的入参
    import_monitor_data = import_manager_monitor_info_uri(i_paras);
    goto_label_do_info_if_fail(NULL != import_monitor_data, err_exit,
        debug_log(DLOG_ERROR, "alloc a new import monitor info failed"));

    // 是否正在导入标识，文件下载完后置位
    import_monitor_data->task_progress = TASK_NO_PROGRESS; 
    import_monitor_data->rsc_privilege = USERROLE_USERMGNT;

    // 创建TASK
    // body_jso 中的内容就是 i_paras->val_jso;
    ret = create_new_task(RF_IMPORT_CONFIG_TASK_DESC, import_status_monitor_uri, import_monitor_data, body_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(import_monitor_data);
        debug_log(DLOG_ERROR, "create new task failed"));

    
    return HTTP_ACCEPTED;

err_exit:

    ret = HTTP_INTERNAL_SERVER_ERROR;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    if (i_paras != NULL) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


gint32 act_manager_export_dump(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    const gchar *value_str = NULL;
    json_object *value_jso = NULL;
    json_bool ret_bool;
    guint32 ret = 0;    // URI  函数的返回值
    gchar *uri_lower = NULL; // URI 转变成为小写

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    

    
    // 必须拥有权限USERROLE_SECURITYMGNT
    if ((0 == (USERROLE_SECURITYMGNT & i_paras->user_role_privilege))) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    

    
    // 获取属性"Content"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != value_jso), HTTP_INTERNAL_SERVER_ERROR,
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    value_str = dal_json_object_get_str(value_jso);

    // 将i_paras中的uri转变成为小写的uri
    uri_lower = g_ascii_strdown(i_paras->uri, strlen(i_paras->uri));
    return_val_do_info_if_fail(NULL != uri_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__));

    // 将postman的路径转换成为小写，并释放uri_lower
    if (0 != strncpy_s(i_paras->uri, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1)) {
        g_free(uri_lower);
        return VOS_ERR;
    }

    // 释放uri_lower
    g_free(uri_lower);

    
    if (value_str[0] == '/') {
        // 本地文件导入，拷贝Content中的内容到指定的临时文件夹TMP_CLIENT_PEM_FILE  "/tmp/web/client_tmp.cer"
        

        if (strlen(value_str) > MAX_FILEPATH_LENGTH - 1) { // 路径长度最多为255(包括文件名)
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO, 
                RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }

        ret = dal_check_real_path2(value_str, TMP_PATH_WITH_SLASH);
        // 导出路径无效
        if (ret != VOS_OK) {
            (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, RF_SENSITIVE_INFO, 
                RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }

        if (g_file_test(value_str, G_FILE_TEST_IS_DIR) == TRUE) {
            (void)create_message_info(MSGID_NOT_ALLOW_OVERWRITE_DIR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        // 执行将一键收集信息进行本地导出
        return act_manager_export_file_local(i_paras, USERROLE_SECURITYMGNT, o_message_jso, o_result_jso);
    } else { // 远程文件导出
        // 执行一键收集信息远程文件导出
        
        return act_manager_export_file_uri(i_paras, USERROLE_SECURITYMGNT, o_message_jso, o_result_jso);
    }
}

LOCAL gint32 act_manager_export_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    const gchar *value_str = NULL;
    json_object *value_jso = NULL;
    json_bool ret_bool;
    gchar *uri_lower = NULL; // URI 转变成为小写

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    // 必须拥有USERROLE_USERMGNT
    if ((USERROLE_USERMGNT & i_paras->user_role_privilege) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    // 获取属性"Content"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != value_jso), HTTP_INTERNAL_SERVER_ERROR,
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    value_str = dal_json_object_get_str(value_jso);

    // 将i_paras中的uri转变成为小写的uri
    uri_lower = g_ascii_strdown(i_paras->uri, strlen(i_paras->uri));
    return_val_do_info_if_fail(NULL != uri_lower, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: change uri to lower fail", __FUNCTION__, __LINE__));

    // 将postman的路径转换成为小写，并释放uri_lower
    if (strncpy_s(i_paras->uri, MAX_URI_LENGTH, uri_lower, MAX_URI_LENGTH - 1) != EOK) {
        g_free(uri_lower);
        return VOS_ERR;
    }

    // 释放uri_lower
    g_free(uri_lower);

    
    if (value_str[0] == '/') {
        if (strlen(value_str) > MAX_FILEPATH_LENGTH - 1) { // 路径长度最多为255(包括文件名)
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO, 
                RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }

        // 导出路径无效
        if (dal_check_real_path2(value_str, TMP_PATH_WITH_SLASH) != VOS_OK) {
            (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, RF_SENSITIVE_INFO, 
                RFACTION_PARAM_CONTENT);
            return HTTP_BAD_REQUEST;
        }

        if (g_file_test(value_str, G_FILE_TEST_IS_DIR) == TRUE) {
            (void)create_message_info(MSGID_NOT_ALLOW_OVERWRITE_DIR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        // 执行将一键收集信息进行本地导出
        return act_manager_export_file_local(i_paras, USERROLE_USERMGNT, o_message_jso, o_result_jso);
    } else {
        // 执行一键收集信息远程文件导出
        return act_manager_export_file_uri(i_paras, USERROLE_USERMGNT, o_message_jso, o_result_jso);
    }
}

LOCAL gint32 act_manager_import_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    const gchar *value_str = NULL;
    json_object *value_jso = NULL;
    guint32 ret = 0;    // URI  函数的返回值

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    // 必须拥有USERROLE_USERMGNT
    if (0 == (USERROLE_USERMGNT & i_paras->user_role_privilege)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    

    // 获取属性"Content"的值
    json_bool ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != value_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    value_str = dal_json_object_get_str(value_jso);
    if (g_regex_match_simple(IMPORT_CONFIGURE_REGEX, value_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }
    
    if (value_str[0] == '/') {
        

        // 本地文件导入，拷贝Content中的内容到指定的临时文件夹TMP_CLIENT_PEM_FILE  "/tmp/web/client_tmp.cer"
        
        ret = dal_check_real_path(value_str);
        if (ret != VOS_OK || g_file_test(value_str, G_FILE_TEST_IS_REGULAR) == FALSE) {
            debug_log(DLOG_ERROR, "%s : file real_path is illegal! return %d\r\n", __FUNCTION__, ret);

            
            (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, value_str, RFACTION_PARAM_CONTENT);
            

            return HTTP_BAD_REQUEST;
        }

        if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, value_str,
            (const gchar *)i_paras->user_roleid)) {
            (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        

        
        // 源文件的路径不能和临时文件的路径相同
        if (0 != g_strcmp0(TMP_CONFIG_PEM_FILE, value_str)) {
            // redfish模块降权限后，调用代理方法拷贝文件
            ret = proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, value_str,
                TMP_CONFIG_PEM_FILE, REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s: copy request file to %s failed", __FUNCTION__, TMP_CONFIG_PEM_FILE);
                (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, value_str);
                (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    TMP_CONFIG_PEM_FILE);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
            }
        }

        
        (void)dal_set_file_owner(TMP_CONFIG_PEM_FILE, REDFISH_USER_UID, APPS_USER_GID);
        (void)dal_set_file_mode(TMP_CONFIG_PEM_FILE, 0660);

        // 执行将配置信息进行本地导入
        
        return act_manager_import_file_local(i_paras, o_message_jso, o_result_jso);
    } else { // 远程文件导入
        // 执行配置信息远程文件导入
        
        return act_manager_import_file_uri(i_paras, o_message_jso, o_result_jso);;
    }
}

LOCAL gint32 act_manager_import_config_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_manager_import_config(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_OK && ret != HTTP_ACCEPTED && ret != RET_OK && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}

LOCAL gint32 act_manager_power_on(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    const gchar *value_str = NULL;
    json_object *value_jso = NULL;
    guint8 value_byte = 0;
    guint32 ret; // URI函数的返回值
    guint32 ret_bool;
    OBJ_HANDLE obj_handle; // /类对象的句柄
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    // 必须拥有USERROLE_BASICSETTING
    // 基本设置权限

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    

    
    // 获取属性"PowerOnPermit"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_MANAGER_ACTION_POWER_ON_PERMIT, &value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != value_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    value_str = dal_json_object_get_str(value_jso);
    if (0 == g_strcmp0(value_str, "On")) {
        value_byte = 1;
    } else if (0 == g_strcmp0(value_str, "Off")) {
        value_byte = 0;
    }

    // 有效性校验

    // 5.1 获得stateless的句柄
    ret = dal_get_object_handle_nth(CLASS_STATELESS, 0, &obj_handle);

    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_byte(value_byte));

    // 4.2 调用UIP函数
    

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, CLASS_STATELESS,
        METHOD_STATELESS_SET_POWER_ON_PERMIT, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_STATELESS_SET_POWER_ON_PERMIT, ret));
    

    

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case MODULE_OK:
            ret = HTTP_OK;
            debug_log(DLOG_DEBUG, " PowerOnPermit success	:%d", ret);
            (void)create_message_info(MSGID_POWER_ON_PERMIT_OK, NULL, o_message_jso);
            break;

        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "PowerOnPermit failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}

LOCAL gint32 __process_uninstall_language_ret(gint32 ret, json_object **o_message_jso, PROVIDER_PARAS_S *i_paras,
    const gchar *delete_language)
{
    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: has no privilege, ret is %d.\n", __FUNCTION__, ret);
            return HTTP_FORBIDDEN;

        
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            debug_log(DLOG_ERROR, "%s: resource not found, ret is %d.\n", __FUNCTION__, ret);
            return HTTP_NOT_FOUND;

        
        case LANGUAGE_NULL_OR_EMPTY:
            (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, delete_language, RFRPOP_LANGUAGE,
                RFPROP_MANAGER_DELETE_LANGUAGE);
            debug_log(DLOG_ERROR, "%s: parameter Language is NULL or empty, ret is %d.\n", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;

        
        case LANGUAGE_ONLY_ONE_LANGUAGE:
            (void)create_message_info(MSGID_ONLY_ONE_LANGUAGE_INSTALLED, NULL, o_message_jso, delete_language);
            debug_log(DLOG_ERROR, "%s: only one language installed, ret is %d.\n", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;

        
        case LANGUAGE_NO_UNINSTALLED_ZH_OR_EN:
            (void)create_message_info(MSGID_NOT_SUPPORT_ZH_AND_EN_UNINSTALLED, NULL, o_message_jso, delete_language);
            debug_log(DLOG_ERROR, "%s: cannot uninstall Chinese or English, ret is %d.\n", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;

        
        case LANGUAGE_NOT_INSTALLED:
            (void)create_message_info(MSGID_LANGUAGE_NOT_INSTALLED, NULL, o_message_jso, delete_language);
            debug_log(DLOG_ERROR, "%s: language %s has not been installed, ret is %d.\n", __FUNCTION__, delete_language,
                ret);
            return HTTP_BAD_REQUEST;
        
        
        case LANGUAGE_NOT_SUPPORT:
            (void)create_message_info(MSGID_LANGUAGE_NOT_SUPPORT, RFPROP_MANAGER_PATCH_LANGUAGE, o_message_jso,
                delete_language);
            debug_log(DLOG_ERROR, "%s: language %s is not supported, ret is %d.\n", __FUNCTION__, delete_language, ret);
            return HTTP_BAD_REQUEST;

        
        default:
            debug_log(DLOG_ERROR, "%s: function uninstall_language_resource error, ret is %d.\n", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 act_manager_delete_language(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *delete_language = NULL;
    json_object *delete_language_jso = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: has no USERROLE_BASICSETTING privilege.\n", __FUNCTION__));

    
    ret = json_object_object_get_ex(i_paras->val_jso, RFRPOP_LANGUAGE, &delete_language_jso);
    return_val_do_info_if_fail(ret && (NULL != delete_language_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get delete_language_jso fail.\n", __FUNCTION__));

    
    delete_language = dal_json_object_get_str(delete_language_jso);
    return_val_do_info_if_fail(NULL != delete_language, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get delete_language fail.\n", __FUNCTION__));

    
    input_list = g_slist_append(input_list, g_variant_new_string(delete_language));
    return_val_do_info_if_fail(NULL != input_list, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: input_list append delete_language fail.\n", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL; debug_log(DLOG_ERROR, "%s: get obj_handle fail, ret is %d.\n", __FUNCTION__, ret));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_CLASEE_NAME, METHOD_UNINSTALL_LANGUAGE_RESOURCE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);

    
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;

    ret = __process_uninstall_language_ret(ret, o_message_jso, i_paras, delete_language);

    return ret;
}


LOCAL gint32 act_manager_restore_factory(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.\n", __FUNCTION__));

    
    return_val_do_info_if_fail(0 != (USERROLE_USERMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        debug_log(DLOG_ERROR, "%s: insufficient privilege.\n", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_handle(OBJECT_LOG, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get log_0 handle fail, ret is %d.\n", __FUNCTION__, ret));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_LOG, METHOD_ADVANCE_FACTORY_RESTORE, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    
    switch (ret) {
        
        case VOS_OK:
        case RFERR_SUCCESS:
            ret = HTTP_OK;
            break;

        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            debug_log(DLOG_ERROR, "%s: insufficient privilege, ret is %d.\n", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            ret = HTTP_FORBIDDEN;
            break;

        
        case RFERR_NO_RESOURCE:
            debug_log(DLOG_ERROR, "%s: resource not found, ret is %d.\n", __FUNCTION__, ret);
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            ret = HTTP_NOT_FOUND;
            break;

        
        
        case MD_EONUPGRADE:
            debug_log(DLOG_ERROR, "%s: restore facotry settings failed, ret is %d.\n", __FUNCTION__, ret);
            (void)create_message_info(MSGID_TASK_DUMP_EXPORT_UPGRADING_ERR, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        

        
        case FM_ERROR_NO_RECOVER_POINT:
            debug_log(DLOG_ERROR, "%s: No restore point is set, ret is %d.", __FUNCTION__, ret);
            (void)create_message_info(MSGID_NO_RESTORE_POINT, NULL, o_message_jso);
            ret = HTTP_NOT_FOUND;
            break;

        
        default:
            debug_log(DLOG_ERROR, "%s: internal error, ret is %d.\n", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


gint32 act_manager_general_download(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *path_file_name = NULL;
    const gchar *path_file_name_str = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.\n", __FUNCTION__));

    ret = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_PATH, &path_file_name);
    if (ret != TRUE || NULL == path_file_name) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_MANAGER_GENERAL_DOWNLOAD,
            RFACTION_PARAM_PATH);
        return HTTP_BAD_REQUEST;
    }

    path_file_name_str = dal_json_object_get_str(path_file_name);

    ret = dal_check_real_path2(path_file_name_str, TMP_PATH_WITH_SLASH);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, path_file_name_str, RFACTION_PARAM_PATH);
        return HTTP_BAD_REQUEST;
    }

    // 文件路径必须是一个已存在的文件,不能是目录
    if (g_file_test(path_file_name_str, G_FILE_TEST_IS_REGULAR) == FALSE) {
        debug_log(DLOG_ERROR, "%s : file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    i_paras->custom_header = json_object_new_object();

    json_object_object_add(i_paras->custom_header, REQ_HEADER_CONTENT_TYPE,
        json_object_new_string(RF_CONTENT_TYPE_OCTET_STREAM));
    json_object_object_add(i_paras->custom_header, REQ_HEADER_CONTENT_DISPOSITION,
        json_object_new_string(path_file_name_str));

    return HTTP_OK;
}


LOCAL gint32 fusion_partition_mode_str2enum(const gchar *fp_mode_str, guint8 *fp_mode_byte)
{
    return_val_if_expr(NULL == fp_mode_str || NULL == fp_mode_byte, VOS_ERR);

    if (0 == g_strcmp0(fp_mode_str, RFPROP_VAL_FP_DUAL_SYSTEM)) {
        *fp_mode_byte = PARTITION_4P;
    } else if (0 == g_strcmp0(fp_mode_str, RFPROP_VAL_FP_SINGLE_SYSTEM)) {
        *fp_mode_byte = PARTITION_8P;
    } else {
        debug_log(DLOG_ERROR, "%s: invalid fusion partition mode is %s", __FUNCTION__, fp_mode_str);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 get_fusion_partition_input_param(json_object *body_jso, json_object **o_message_jso, GSList **input_list)
{
    GSList *inner_input = NULL;
    gint32 ret_val;
    json_object *prop_jso = NULL;
    guint8 mode_byte = G_MAXUINT8;

    return_val_if_expr(NULL == body_jso || NULL == o_message_jso || NULL == input_list, VOS_ERR);

    // 从请求体中获取key值失败产生内部错误是因为，这3个参数都是必选参数，协议层会保证一定有值，否则不会走到接口中
    ret_val = json_object_object_get_ex(body_jso, RFACTION_PARAM_FP, &prop_jso);
    goto_label_do_info_if_fail(TRUE == ret_val, exit, ret_val = VOS_ERR;
        debug_log(DLOG_ERROR, "%s: FusionPartition not exit", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    ret_val = fusion_partition_mode_str2enum(dal_json_object_get_str(prop_jso), &mode_byte);
    goto_label_do_info_if_fail(VOS_OK == ret_val, exit, ret_val = VOS_ERR;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    inner_input = g_slist_append(inner_input, g_variant_new_byte(mode_byte));
    prop_jso = NULL;

    ret_val = json_object_object_get_ex(body_jso, RFACTION_PARAM_FP_REMOTE_NAME, &prop_jso);
    goto_label_do_info_if_fail(TRUE == ret_val, exit, ret_val = VOS_ERR;
        debug_log(DLOG_ERROR, "%s: RemoteNodeUserName not exit", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    inner_input = g_slist_append(inner_input, g_variant_new_string(dal_json_object_get_str(prop_jso)));
    prop_jso = NULL;

    ret_val = json_object_object_get_ex(body_jso, RFACTION_PARAM_FP_REMOTE_PASSWD, &prop_jso);
    goto_label_do_info_if_fail(TRUE == ret_val, exit, ret_val = VOS_ERR;
        debug_log(DLOG_ERROR, "%s: RemoteNodePassword not exit", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    inner_input = g_slist_append(inner_input, g_variant_new_string(dal_json_object_get_str(prop_jso)));
    prop_jso = NULL;

    // 执行到这一步，认为已经成功了
    ret_val = VOS_OK;
exit:

    // 手动清空密码
    do_if_expr(json_object_object_get_ex(body_jso, RFACTION_PARAM_FP_REMOTE_PASSWD, &prop_jso),
        json_object_object_del(body_jso, RFACTION_PARAM_FP_REMOTE_PASSWD));

    if (VOS_OK == ret_val) {
        *input_list = inner_input;
    } else {
        // 该接口会判空
        uip_free_gvariant_list(inner_input);
    }

    return ret_val;
}


LOCAL gint32 act_manager_set_fusion_partition(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define FAILED_WITH_SLAVE_BMC_NOT_PRESENT "the system B is not present"
#define FAILED_WITH_BMC_NOT_POWER_OFF "the OS of system A or B is powered on"
#define FAILED_WITH_CONF_SAME "the FusionPartition's value entered is the same as the current value"
#define FAILED_WITH_BMC_VER_NOT_SAME "the system A and B versions are different"
#define FAILED_WITH_CPLD_VER_NOT_SAME "the CPLD versions of the system A and B are different"
#define FAILED_WITH_SLAVE_AUTH_FAIL "the authentication of the system B failed"
#define FAILED_WITH_SLAVE_NOT_PRI "the user of the system B has insufficient permission"
    gint32 ret_val;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *msg_arg1 = NULL;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret_val = fusion_partition_supported();
    return_val_do_info_if_fail(VOS_OK == ret_val, HTTP_NOT_IMPLEMENTED,
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_FP_REMOTE_PASSWD);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, RFPROP_MANAGER_SET_FUSION_PARTITION));

    ret_val = dfl_get_object_handle(PARTITION_OBJECT_NAME, &obj_handle);
    return_val_do_info_if_fail(DFL_OK == ret_val, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dfl_get_object_handle %s failed", __FUNCTION__, PARTITION_OBJECT_NAME);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_FP_REMOTE_PASSWD);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret_val = get_fusion_partition_input_param(i_paras->val_jso, o_message_jso, &input_list);
    return_val_if_fail(VOS_OK == ret_val, HTTP_BAD_REQUEST);
    input_list = g_slist_append(input_list, g_variant_new_byte(LOG_TYPE_LOCAL));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->client));

    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PARTITION, METHOD_PARTITION_SWITCHPARTITION, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret_val) {
        case VOS_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            ret_val = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            ret_val = HTTP_FORBIDDEN;
            break;

        case PARTITON_CODE_SLAVEBMC_NOT_PRESENT:
            msg_arg1 = FAILED_WITH_SLAVE_BMC_NOT_PRESENT;
            ret_val = HTTP_BAD_REQUEST;
            break;

        case PARTITON_CODE_BMC_NOT_POWEROFF:
            msg_arg1 = FAILED_WITH_BMC_NOT_POWER_OFF;
            ret_val = HTTP_BAD_REQUEST;
            break;

        case PARTITON_CODE_PARTITION_CONF_SAME:
            msg_arg1 = FAILED_WITH_CONF_SAME;
            ret_val = HTTP_BAD_REQUEST;
            break;

        case PARTITON_CODE_BMC_VERSION_NOT_SAME:
            msg_arg1 = FAILED_WITH_BMC_VER_NOT_SAME;
            ret_val = HTTP_BAD_REQUEST;
            break;

        case PARTITON_CODE_CPLD_VERSION_NOT_SAME:
            msg_arg1 = FAILED_WITH_CPLD_VER_NOT_SAME;
            ret_val = HTTP_BAD_REQUEST;
            break;

        case PARTITON_CODE_SLAVE_BMC_AUTH_FAIL:
            msg_arg1 = FAILED_WITH_SLAVE_AUTH_FAIL;
            ret_val = HTTP_BAD_REQUEST;
            break;

        case PARTITON_CODE_SLAVE_BMC_USER_NOT_PRI:
            msg_arg1 = FAILED_WITH_SLAVE_NOT_PRI;
            ret_val = HTTP_BAD_REQUEST;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: uip_call_class_method_redfish %s failed, ret_val = %d", __FUNCTION__,
                METHOD_PARTITION_SWITCHPARTITION, ret_val);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
    }

    do_if_expr(HTTP_BAD_REQUEST == ret_val,
        (void)create_message_info(MSGID_SET_FP_FAILED, NULL, o_message_jso, msg_arg1));

    return ret_val;
}


gint32 manager_provider_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    gboolean b_ret;
    guint8 software_type = 0;

    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);
    (void)dal_get_software_type(&software_type);
    
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        b_ret = redfish_check_smm_manager_uri_valid(i_paras->uri);
    } else {
        b_ret = redfish_check_manager_uri_valid(i_paras->uri);
    }
    

    if (FALSE == b_ret) {
        return HTTP_NOT_FOUND;
    }

    

    *prop_provider = g_manager_action_info_provider;
    *count = sizeof(g_manager_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 manager_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (!ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = manager_provider;
    *count = sizeof(manager_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 manager_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, BMC_UP_TIME_VALUE, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}

gint32 get_sp_status(struct json_object *object)
{
    enum spstatus {
        SPIsWorking = 1,
        SPIsOperable,
        BIOSNeedsUpdate,
        OSIsPoweredOff,
        iBMCNeedsUpdate,
        SPNeedsUpdate,
        SPPartitionTableNotExist,
        Invalid_Data = INVALID_DATA_BYTE
    } status;
    gint32 sp_partition_table;
    gchar sp_app_version[64] = {0};
    OBJ_HANDLE ums_obj_handle = 0;
    OBJ_HANDLE payload_handle = 0;
    guint8 ums_enable = INVALID_DATA_BYTE;
    guint8 bios_support_sp_flag = INVALID_DATA_BYTE;
    guint8 powerstate_byte = INVALID_DATA_BYTE;
    guint32 sp_work_status = INVALID_DATA_BYTE;
    gint32 ret;

    status = Invalid_Data;

    // 判断SP分区表是否存在
    sp_partition_table = vos_get_file_accessible(SP_PARTITION_TABLE_PATH);
    if (INVALID_DATA_BYTE == sp_partition_table) {
        status = Invalid_Data;
        goto out;
    } else if (0 == sp_partition_table) {
        status = SPPartitionTableNotExist;
        goto out;
    }

    // 判断SP是否需要升级
    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &ums_obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "%s: get the %s object fail, ret=%d\n", __FUNCTION__, SP_UMS_CLASS_NAME, ret));
    ret = dal_get_property_value_string(ums_obj_handle, UMS_PROPERTY_SP_VERSION_APP, sp_app_version,
        sizeof(sp_app_version));
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "get the %s object %s property fail, ret=%d\n", SP_UMS_CLASS_NAME,
        UMS_PROPERTY_SP_VERSION_APP, ret));
    if (VOS_ERR == check_string_val_effective((const gchar *)sp_app_version)) {
        status = SPNeedsUpdate;
        goto out;
    }

    // 判断iBMC是否支持SP

    ret = dal_get_property_value_byte(ums_obj_handle, UMS_PROPERTY_ENBLE, (guchar *)(&ums_enable));
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "get the %s object %s property fail, ret=%d\n", SP_UMS_CLASS_NAME, UMS_PROPERTY_ENBLE,
        ret));
    if (INVALID_DATA_BYTE == ums_enable) {
        status = Invalid_Data;
        goto out;
    } else if (0 == ums_enable) {
        status = iBMCNeedsUpdate;
        goto out;
    }

    // 判断OS是否上电
    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload_handle);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "get the %s object fail, ret=%d\n", CLASS_PAYLOAD, ret));
    ret = dal_get_property_value_byte(payload_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &powerstate_byte);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "get the %s object %s property fail, ret=%d\n", CLASS_PAYLOAD,
        PROPERTY_PAYLOAD_CHASSPWR_STATE, ret));
    if (INVALID_DATA_BYTE == powerstate_byte) {
        status = Invalid_Data;
        goto out;
    } else if (0 == powerstate_byte) {
        status = OSIsPoweredOff;
        goto out;
    }

    // 判断bios是否支持SP
    ret = dal_get_property_value_byte(ums_obj_handle, UMS_PROPERTY_BIOS_SUPPORT, (guchar *)(&bios_support_sp_flag));
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "get the %s object %s property fail, ret=%d\n", SP_UMS_CLASS_NAME,
        UMS_PROPERTY_BIOS_SUPPORT, ret));
    if (INVALID_DATA_BYTE == bios_support_sp_flag) {
        status = Invalid_Data;
        goto out;
    } else if (DISABLE == bios_support_sp_flag) {
        status = BIOSNeedsUpdate;
        goto out;
    }

    // 判断SP是否正在工作
    sp_work_status = _sp_get_connect_state();
    if (INVALID_DATA_BYTE == sp_work_status) {
        status = Invalid_Data;
    } else if (0 == sp_work_status) {
        status = SPIsWorking;
    } else if (1 == sp_work_status) {
        status = SPIsOperable;
    }

out:
    switch (status) {
        case SPIsWorking:
            // SP正在工作
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, json_object_new_string("SPIsWorking"));
            break;

        case SPIsOperable:
            // SP功能正常，但是未工作
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, json_object_new_string("SPIsOperable"));
            break;

        case BIOSNeedsUpdate:
            // 需要升级BIOS
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, json_object_new_string("BIOSNeedsUpdate"));
            break;

        case OSIsPoweredOff:
            // 操作系统未上电
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, json_object_new_string("OSIsPoweredOff"));
            break;

        case iBMCNeedsUpdate:
            // 需要升级iBMC
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, json_object_new_string("iBMCNeedsUpdate"));
            break;

        case SPNeedsUpdate:
            // 需要升级SP
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, json_object_new_string("SPNeedsUpdate"));
            break;

        case SPPartitionTableNotExist:
            // SP分区表不存在
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS,
                json_object_new_string("SPPartitionTableNotExist"));
            break;
        case Invalid_Data:
            // 未获取到SP状态
            (void)json_object_object_add(object, RFPOP_PRODUCT_SPSTATUS, NULL);
            break;
    }
    return VOS_OK;
}


LOCAL void get_managers_firmware_version(json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    char strbuf[STRING_LEN_MAX] = {0};
    gint32 ret;

    
    ret = dal_get_object_handle_nth(CLASS_BMC, 0, &obj_handle);
    do_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s:%d get CLASS_BMC object handler fail.", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, BMC_MANA_VER_NAME, strbuf, sizeof(strbuf));
    do_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s:%d get BMC_MANA_VER_NAME object handler fail.", __FUNCTION__, __LINE__));

    
    ret = rf_string_check((const gchar *)strbuf);
    do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s:%d rf_string_check fail.", __FUNCTION__, __LINE__));
    json_object_object_add(o_result_jso, RFPROP_MANAGER_FIRMWARE_VERSION,
        json_object_new_string((const gchar *)strbuf));

    return;
}


LOCAL void get_managers_uuid(json_object *o_result_jso)
{
    gint32 ret;
    gchar uuid[RF_UUID_LEN + 1] = {0};

    
    ret = dal_get_system_guid_string(uuid, sizeof(uuid));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    json_object_object_add(o_result_jso, RFPROP_MANAGER_UUID, json_object_new_string((const gchar *)uuid));

    return;
}


LOCAL void get_system_lockdown(json_object *huawei)
{
    gint32 ret = 0;
    guchar system_lockdown_support = 0;
    guchar system_lockdown_state = 0;

    return_if_expr(NULL == huawei);

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_SYSTEM_LOCKDOWN_SUPPORT,
        &system_lockdown_support);
    if (PME_SERVICE_SUPPORT == system_lockdown_support) {
        ret = dal_get_func_ability(OBJ_SECURITY_ENHANCE, PROPERTY_SECURITY_SYSTEMLOCKDOWNSTATE, &system_lockdown_state);
        if (VOS_OK == ret) {
            if (system_lockdown_state) {
                json_object_object_add(huawei, PROPERTY_ACCOUNT_SERVICE_SYSTEM_LOCKDOWN_ENABLED,
                    json_object_new_boolean(TRUE));
            } else {
                json_object_object_add(huawei, PROPERTY_ACCOUNT_SERVICE_SYSTEM_LOCKDOWN_ENABLED,
                    json_object_new_boolean(FALSE));
            }
        }
    }

    return;
}


LOCAL void get_license_service(json_object **huawei)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_if_expr(NULL == huawei);

    ret = dal_get_object_handle_nth(CLASS_NAME_LICENSE_MANAGE, 0, &obj_handle);
    if (VOS_OK == ret) {
        ret = get_odata_id_object(RF_MANAGERS_URI, RFPROP_MANAGER_LICENSE_SERVICE, huawei);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d: get_odata_id_object fail.\n", __FUNCTION__, __LINE__));
    }

    return;
}


LOCAL void add_prop_manager_datetime(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
    gint32 ret;
    gint16 time_zone = 0;
    guint32 time_now = 0;
    gchar str_buf[STRING_LEN_MAX] = {0};

    ret = redfish_get_format_time(i_paras, &time_now, &time_zone);
    if (ret != VOS_OK) {
        goto ERR_EXIT;
    }

    ret = redfish_fill_format_time(str_buf, sizeof(str_buf), &time_now, &time_zone);
    if (ret != VOS_OK) {
        goto ERR_EXIT;
    }

    json_object_object_add(o_result_jso, RFPROP_MANAGER_DATATIME, json_object_new_string((const gchar *)str_buf));
    return;

ERR_EXIT:
    json_object_object_add(o_result_jso, RFPROP_MANAGER_DATATIME, NULL);
    return;
}


gint32 get_system_overview_managers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    json_object *property_object = NULL;

    
    return_val_do_info_if_expr(NULL == o_message_jso || NULL == o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: paras check fail. ", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   json_object_new_array fail. ", __FUNCTION__, __LINE__));

    
    obj_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   json_object_new_object fail. ", __FUNCTION__, __LINE__));

    
    ret = get_common_id(i_paras, o_message_jso, &obj_jso);
    return_val_do_info_if_expr(HTTP_OK != ret, ret, (void)json_object_put(obj_jso);
        (void)json_object_put(*o_result_jso); *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d: function  return err of get_common_id.", __FUNCTION__, __LINE__));

    
    ret = get_manager_bmc_hostname(i_paras, o_message_jso, &property_object);
    do_if_expr(HTTP_OK != ret,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of get_manager_bmc_hostname.", __FUNCTION__, __LINE__));
    json_object_object_add(obj_jso, RFPROP_MANAGER_BMC_HOSTNAME, property_object);
    property_object = NULL;

    
    get_managers_firmware_version(obj_jso);

    
    get_managers_uuid(obj_jso);

    
    get_vga_enabled(obj_jso);

    
    get_system_lockdown(obj_jso);

    
    get_fusion_partition(obj_jso);

    
    get_remote_bmc_ip(obj_jso);

    
    get_system_overview_mac(&property_object);
    json_object_object_add(obj_jso, RFPROP_MANAGER_ETH_MACADDR, property_object);

    
    property_object = json_object_new_object();
    get_manager_oemresource_default_ipaddr_mode(property_object);
    json_object_object_add(obj_jso, RFPROP_MANAGER_MANAGER_IPV4_DEFAULT, property_object);
    property_object = NULL;

    
    get_manage_oem_string(CLASS_NAME_DNSSETTING, PROPERTY_DNSSETTING_DOMAINNAME, &property_object);
    json_object_object_add(obj_jso, RFPROP_SERVICEROOT_DOMAIN_NAME, property_object);
    property_object = NULL;

    
    get_manage_oem_string(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IP_ADDR, &property_object);
    json_object_object_add(obj_jso, RFPROP_MANAGER_MANAGER_IP_ADDR, property_object);
    property_object = NULL;

    
    get_manage_oem_string(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, &property_object);
    json_object_object_add(obj_jso, RFPROP_MANAGER_MANAGER_IPV6_ADDR, property_object);
    property_object = NULL;

    
    get_license_service(&property_object);
    json_object_object_add(obj_jso, RFPROP_MANAGER_LICENSE_SERVICE, property_object);
    property_object = NULL;

    // fdmservice 资源
    get_fdm_link(obj_jso);

    
    get_usbmgmt_link(obj_jso);

    
    add_prop_manager_datetime(i_paras, obj_jso);

    
    get_manage_oem_platform(obj_jso);

    (void)json_object_array_add(*o_result_jso, obj_jso);
    return HTTP_OK;
}
