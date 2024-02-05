
#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_class.h"
#include "redfish_message.h"
#include "redfish_provider_resource.h"
#include "redfish_provider.h"

#include <time.h>

LOCAL gint32 fdm_check_fdmservice_uri_valid(gchar *uri_string, const gchar *URI_FORMAT);
LOCAL gint32 check_fdm_report_file_exist(PROVIDER_PARAS_S *i_paras);

LOCAL gint32 get_fdm_report_json(PROVIDER_PARAS_S *provider_param);

LOCAL gint32 free_fdm_report_json(json_object *report_jso);
LOCAL gint32 get_event_report_link(gchar *alias, gchar *odata_id, guint32 odata_size);
LOCAL gint32 get_device_info_link(gint32 alias, json_object *node_jos, gchar *odata_id);
LOCAL gint32 get_silk_name_by_alias(json_object *node_jso, gint32 alias, gchar *silk_name);
LOCAL gint32 get_component_id_by_alias(json_object *node_jso, gint32 alias, guint8 *component_id);
LOCAL gint32 get_odata_id_by_alias(gint32 alias, json_object *node_jos, gchar *odata_id);
LOCAL gint32 get_odata_id_pre(guint8 component_id, gchar *url_pre);
LOCAL gint32 get_odata_id_end(const gchar *silk_name, gchar *url_end, guint32 url_end_size);
LOCAL gint32 fdm_erase_blank(const gchar *src_name, gchar *dst_name, guint32 dst_name_size);
LOCAL gint32 get_alias_from_uri(gchar *uri, gint32 *alias);
LOCAL gint32 check_pfae_time_valid(gint32 val);
LOCAL gint32 check_refresh_interval_valid(gint32 val);
LOCAL gint32 fdm_add_action_prop(json_object **action_prop_jso, const gchar *action_name);
LOCAL gint32 fdm_set_ret_func(gint32 ret, json_object **o_message_jso, const gchar *prop_name);
LOCAL gint32 get_event_reports_by_alias(gint32 alias, json_object *health_report, json_object **reports_jso);
LOCAL gint32 get_maintence_reports_by_alias(gint32 alias, guint8 *depth, json_object *prop_jso,
    json_object **reports_jso);
LOCAL gint32 get_related_devices(json_object *node_jso, json_object **reformed_jso, guint8 *depth);
LOCAL gint32 get_url_format_by_component_id(guint8 component_id, gchar *url_format, guint32 url_format_size);
LOCAL gint32 fdm_get_prop_values(PROVIDER_PARAS_S *provider_param, json_object **o_result_jso,
    json_object *o_message_array_jso, const PROPERTY_PROVIDER_S *provider, const guint32 count);
LOCAL gint32 get_manager_fdmservice_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_report_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_auto_refresh_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_pfae_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_auto_update(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_ierr_fail_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_pfae_notify_os_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_ierr_fail_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_ierr_success_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_ierr_success_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_mem_poor_contact_alarm_enabled(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_mem_poor_contact_alarm_enabled(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_health_report(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_device_tree(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_auto_refresh_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_pfae_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_pfae_notify_os_enable(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 act_regenerate_report(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_remove_alarm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_fdmservice_event_reports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_fdmservice_event_records(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_fdmservice_regenerate_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_fdmservice_removealarm_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_fdmservice_mem_dynamic_remapping_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_manager_fdmservice_mem_dynamic_remapping_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

#define FDM_REPORT_REFRESH_FIRST_INTERVAL 1
#define FDM_REPORT_REFRESH_SECOND_INTERVAL 24
#define FDM_REPORT_REFRESH_THIRD_INTERVAL 168
#define FDM_REPORT_FIRST_DURATION 12
#define FDM_REPORT_SECOND_DURATION 20
#define FDM_REPORT_THIRD_DURATION 30
#define FDM_PROPERTY_INDEX_ONE 0
#define FDM_PROPERTY_INDEX_TWO 1
#define FDM_PROPERTY_INDEX_THREE 2
#define FDM_REVERSE_ERROR (-1)
#define FDM_DIRECTION_IN 0
#define FDM_DIRECTION_OUT 1
#define FDM_HEALTH_REPORT_PARA_ERR 3

LOCAL PROPERTY_PROVIDER_S g_manager_fdmservice_provider[] = {
    {
        RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_context, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_odata_id, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_REPORT_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_report_time, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_AUTO_UPDATE_FLAG, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_auto_update, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_REFRESH_INTERVAL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_auto_refresh_interval, set_manager_fdmservice_auto_refresh_interval, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_DIAGNOSE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_pfae_time, set_manager_fdmservice_pfae_time, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_DIAGNOSE_FAIL_POLICY, CLASS_FDM_NAME, PROPERTY_FDM_IERR_DIAG_FAIL_POLICY, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_ierr_fail_policy, set_manager_fdmservice_ierr_fail_policy, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY, CLASS_FDM_NAME, PROPERTY_FDM_IERR_DIAG_SUCCESS_POLICY, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_ierr_success_policy, set_manager_fdmservice_ierr_success_policy, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_MEM_POOR_CONTACT_ALARM_ENABLED, CLASS_FDM_NAME, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_mem_poor_contact_alarm_enabled,
        set_manager_fdmservice_mem_poor_contact_alarm_enabled, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_PFAE_NOTIFY_OS_ENABLED, CLASS_FDM_WEB_NAME, PROPERTY_FDMWEB_PFAE_NOTIFY_SYSTEM, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_pfae_notify_os_enable, set_manager_fdmservice_pfae_notify_os_enable, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE, CLASS_FDM_NAME, PROPERTY_FDM_DAYNAMIC_REMAPPING_ENABLE, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_manager_fdmservice_mem_dynamic_remapping_enable, set_manager_fdmservice_mem_dynamic_remapping_enable, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_HEALTH_REPORT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_health_report, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_DEVICE_TREE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_device_tree, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_manager_fdmservice_action, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_FDM_REGENERATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW,
        NULL, NULL, act_regenerate_report,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_FDM_REMOVEALARM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_FORBID,
        NULL, NULL, act_remove_alarm,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_fdmservice_event_reports_provider[] = {
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_fdmservice_event_reports_odata_id, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_FDM_EVENT_RECORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_fdmservice_event_records, NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_fdmservice_regenerate_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_fdmservice_regenerate_actioninfo_odata_id, NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_fdmservice_removealarm_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_fdmservice_removealarm_actioninfo_odata_id, NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL gint32 get_relevant_refresh_input(gint32 val, guint8 direction)
{
    switch (val) {
        // index to value
        case FDM_PROPERTY_INDEX_ONE:
            return FDM_REPORT_REFRESH_FIRST_INTERVAL;

        case FDM_PROPERTY_INDEX_TWO:
            if (FDM_DIRECTION_IN != direction) {
                return FDM_REPORT_REFRESH_SECOND_INTERVAL;
            } else {
                return FDM_PROPERTY_INDEX_ONE;
            }

        case FDM_PROPERTY_INDEX_THREE:
            return FDM_REPORT_REFRESH_THIRD_INTERVAL;
        // value to index
        case FDM_REPORT_REFRESH_SECOND_INTERVAL:
            return FDM_PROPERTY_INDEX_TWO;

        case FDM_REPORT_REFRESH_THIRD_INTERVAL:
            return FDM_PROPERTY_INDEX_THREE;
        default:
            return FDM_REVERSE_ERROR;
    }
}


LOCAL gint32 get_reverse_pfae_input(gint32 val)
{
    switch (val) {
        // index to value
        case FDM_PROPERTY_INDEX_ONE:
            return FDM_REPORT_FIRST_DURATION;

        case FDM_PROPERTY_INDEX_TWO:
            return FDM_REPORT_SECOND_DURATION;

        case FDM_PROPERTY_INDEX_THREE:
            return FDM_REPORT_THIRD_DURATION;
        // value to index
        case FDM_REPORT_FIRST_DURATION:
            return FDM_PROPERTY_INDEX_ONE;

        case FDM_REPORT_SECOND_DURATION:
            return FDM_PROPERTY_INDEX_TWO;

        case FDM_REPORT_THIRD_DURATION:
            return FDM_PROPERTY_INDEX_THREE;
        default:
            return FDM_REVERSE_ERROR;
    }
}


LOCAL guint8 get_pfae_enable(void)
{
    guint8 pfae_enable = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_FDM_WEB_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s obj failed(%d)", __FUNCTION__, CLASS_FDM_WEB_NAME, ret);
        return pfae_enable;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PFAE_ENABLE, &pfae_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s property failed(%d)", __FUNCTION__, PROPERTY_PFAE_ENABLE, ret);
        return pfae_enable;
    }
    return pfae_enable;
}


LOCAL gint32 get_pfae_info_display_enable(guint8 *display_enable)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 pfae_enable = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_FDM_WEB_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s obj failed(%d)", __FUNCTION__, CLASS_FDM_WEB_NAME, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PFAE_ENABLE, &pfae_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s property failed(%d)", __FUNCTION__, PROPERTY_PFAE_ENABLE, ret);
        return ret;
    }

    if (pfae_enable == 0) {
        *display_enable = 0;
        return RET_OK;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PFAE_RESULT_DISPLAY, display_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s property failed(%d)", __FUNCTION__, PROPERTY_PFAE_RESULT_DISPLAY, ret);
        return ret;
    }
    return RET_OK;
}


LOCAL gint32 get_manager_fdmservice_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_id[MAX_URL_LEN] = {0};

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    ret = rf_get_rsc_odata_prop(MANAGER_FDMSERVICE_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get context prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);
    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_id[MAX_URL_LEN] = {0};

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    ret = rf_get_rsc_odata_prop(URI_FORMAT_FDMSERVICE, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);
    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_report_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *prop_jso = NULL;
    long long_time;
    char time_buf[DATE_TYPE_STRING_LEN] = {0};
    struct tm *local_time_ret = NULL;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    
    json_bool ret_json = json_object_object_get_ex(i_paras->user_data, PROPERTY_FDM_COMPONENT_MAINTENCE_TINE,
        &prop_jso);
    

    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: get ReportTime prop failed.", __FUNCTION__));

    long_time = json_object_get_int64(prop_jso);
    return_val_do_info_if_fail(0 != long_time, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: invalid ReportTime %ld.", __FUNCTION__, __LINE__, long_time));

    struct tm local_time;
    (void)memset_s(&local_time, sizeof(local_time), 0, sizeof(local_time));
    local_time_ret = localtime_r((time_t *)&long_time, &local_time);
    check_fail_do_return_val(NULL != local_time_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "get local time failed."));

    if (strftime(time_buf, DATE_TYPE_STRING_LEN, "%Y-%m-%d %H:%M:%S", &local_time) == 0) {
        debug_log(DLOG_ERROR, "strftime returned error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *)time_buf);

    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_auto_refresh_interval(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 update_cycle = 0;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    // 从框架获取属性值
    ret = dal_get_object_handle_nth(CLASS_FDM_WEB_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: get FdmWebConfig handle failed.", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDMWEB_AUTO_UP_CYCLE, &update_cycle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:get %s failed", __FUNCTION__, RFPROP_FDM_REFRESH_INTERVAL));

    gint32 update_cycle_hours = get_relevant_refresh_input((gint32)update_cycle, FDM_DIRECTION_OUT);

    *o_result_jso = json_object_new_int(update_cycle_hours);

    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_pfae_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 pfae_time = 0;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));
    // 从框架获取属性值
    ret = dal_get_object_handle_nth(CLASS_FDM_WEB_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: get FdmWebConfig handle failed.", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDMWEB_TIMEQUANT, &pfae_time);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s:get %s failed", __FUNCTION__, RFPROP_FDM_DIAGNOSE_TIME));

    gint32 pfae_time_days = get_reverse_pfae_input((gint32)pfae_time);

    *o_result_jso = json_object_new_int(pfae_time_days);

    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_auto_update(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *prop_jso = NULL;
    json_object *sub_prop_jso = NULL;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    
    json_bool ret_json = json_object_object_get_ex(i_paras->user_data, PROPERTY_FDM_UPDATE_CONFIG, &prop_jso);
    

    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: get AutoUpdate object failed.", __FUNCTION__, __LINE__));

    ret_json = json_object_object_get_ex(prop_jso, PROPERTY_FDM_AUTO_UPDATE_FLAG, &sub_prop_jso);
    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: get AutoUpdate prop failed.", __FUNCTION__, __LINE__));

    gint8 auto_update_flag = json_object_get_int(sub_prop_jso);

    *o_result_jso = json_object_new_int(auto_update_flag);

    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_ierr_fail_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 policy = 0;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    // 从框架获取属性值
    ret = dal_get_object_handle_nth(g_manager_fdmservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get FdmConfig handle failed.", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, g_manager_fdmservice_provider[i_paras->index].pme_prop_name, &policy);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:get %s failed.", __FUNCTION__, RFPROP_FDM_DIAGNOSE_FAIL_POLICY));

    switch (policy) {
        case RF_FDM_DIAG_POLICY_NO_ACTION_VALUE:
            *o_result_jso = json_object_new_string(RF_FDM_DIAG_POLICY_NO_ACTION);
            break;
        case RF_FDM_DIAG_POLICY_WARM_RESET_VALUE:
            *o_result_jso = json_object_new_string(RF_FDM_DIAG_POLICY_WARM_RESET);
            break;
        case RF_FDM_DIAG_POLICY_POWEROFF_VALUE:
            *o_result_jso = json_object_new_string(RF_FDM_DIAG_POLICY_POWEROFF);
            break;
        default:
            debug_log(DLOG_ERROR, "%s:Invalid FDM diagnose IERR policy value %d .", __FUNCTION__, policy);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_ierr_success_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 policy = DISABLE;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer or forbidden.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 从框架获取属性值
    ret = dal_get_object_handle_nth(g_manager_fdmservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get FdmConfig handle failed, ret: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDM_IERR_DIAG_SUCCESS_POLICY, &policy);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s failed, ret: %d.", __FUNCTION__, PROPERTY_FDM_IERR_DIAG_SUCCESS_POLICY, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    switch (policy) {
        case RF_FDM_DIAG_POLICY_NO_ACTION_VALUE:
            *o_result_jso = json_object_new_string(RF_FDM_DIAG_POLICY_NO_ACTION);
            break;
        case RF_FDM_DIAG_POLICY_WARM_RESET_VALUE:
            *o_result_jso = json_object_new_string(RF_FDM_DIAG_POLICY_WARM_RESET);
            break;
        default:
            debug_log(DLOG_ERROR, "%s:Invalid FDM diagnose IERR success policy value %u.", __FUNCTION__, policy);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_mem_poor_contact_alarm_enabled(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 mem_poor_contact_alarm = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: NULL pointer or forbidden. ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_FDM_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get FdmConfig handle failed. ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED,
        &mem_poor_contact_alarm);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s failed. ret = %d", __FUNCTION__,
            PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_bool alarm_enabled = (json_bool)((mem_poor_contact_alarm == 0) ? FALSE : TRUE);
    *o_result_jso = json_object_new_boolean(alarm_enabled);
    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_pfae_notify_os_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 pfae_notify = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden. ret = %d", __FUNCTION__, __LINE__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 从框架获取属性值
    ret = dal_get_object_handle_nth(CLASS_FDM_WEB_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get FdmWebConfig handle failed. ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDMWEB_PFAE_NOTIFY_SYSTEM, &pfae_notify);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s:get %s failed. ret = %d", __FUNCTION__, PROPERTY_FDMWEB_PFAE_NOTIFY_SYSTEM, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_bool notify = (json_bool)((pfae_notify == 0) ? FALSE : TRUE);
    *o_result_jso = json_object_new_boolean(notify);
    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_health_report(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *prop_jso = NULL;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    guint8 display_enable = 0;
    gint8 chk_ret = check_fdm_enabled();
    if (chk_ret == RET_OK) {
        ret = get_pfae_info_display_enable(&display_enable);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s, %d: get pfae info display failed(%d).", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    if (chk_ret != RET_OK || display_enable == 0) {
        *o_result_jso = json_object_new_array();
        debug_log(DLOG_INFO, "%s, %d: fdm/display enable status %u/%u, return empty array.",
            __FUNCTION__, __LINE__, (chk_ret == RET_OK) ? 1 : 0, display_enable);
        return HTTP_OK;
    }

    
    json_bool ret_json = json_object_object_get_ex(i_paras->user_data, PROPERTY_FDM_HEALTH_REPORT, &prop_jso);
    

    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: get HealthReport prop failed.", __FUNCTION__));

    gint32 array_len = json_object_array_length(prop_jso);
    return_val_do_info_if_expr(0 == array_len, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: report length = 0.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_get(prop_jso);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_INFO, "%s, %d: health report is NULL.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_device_tree(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *prop_jso = NULL;
    json_object *sub_prop_jso = NULL;
    guint8 depth = MAX_RECURSION_DEPTH;

    gint32 ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: NULL pointer or frobidden.", __FUNCTION__));

    guint8 display_enable = 0;
    gint8 chk_ret = check_fdm_enabled();
    if (chk_ret == RET_OK) {
        ret = get_pfae_info_display_enable(&display_enable);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s, %d: get pfae info display failed(%d).", __FUNCTION__, __LINE__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
    if (chk_ret != RET_OK || display_enable == 0) {
        *o_result_jso = json_object_new_object();
        debug_log(DLOG_INFO, "%s, %d: fdm/display enable status %u/%u, return empty obj.",
            __FUNCTION__, __LINE__, (chk_ret == RET_OK) ? 1 : 0, display_enable);
        return HTTP_OK;
    }

    
    json_bool ret_json = json_object_object_get_ex(i_paras->user_data, PROPERTY_FDM_DEVICE_TREE, &prop_jso);
    

    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: get DevTree prop failed.", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: new object failed.", __FUNCTION__, __LINE__));

    json_object *child_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == child_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: new object failed.", __FUNCTION__, __LINE__));

    ret_json = json_object_object_get_ex(prop_jso, PROPERTY_FDM_DEVICE_CHILDREN, &sub_prop_jso);
    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(child_jso);
        debug_log(DLOG_INFO, "%s: get RelatedDevices prop failed.", __FUNCTION__));

    if (VOS_OK == get_related_devices(sub_prop_jso, &child_jso, &depth)) {
        debug_log(DLOG_MASS, "%s: set RelatedDevices prop to response body:%s.", __FUNCTION__,
            dal_json_object_get_str(child_jso));
        json_object_object_add(*o_result_jso, RFPROP_FDM_RELEATED_DEVICE, child_jso);
    } else {
        debug_log(DLOG_INFO, "%s: generate RelatedDevices prop failed.", __FUNCTION__);
        (void)json_object_put(child_jso);
    }

    return HTTP_OK;
}


LOCAL gint32 get_manager_fdmservice_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 array_len;
    gint32 ret;
    const gchar* action_array[] = {
        RFACTION_FDM_REGENERATE,
        RFACTION_FDM_REMOVEALARM
    };

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: NULL pointer or frobidden.", __FUNCTION__));

    *o_result_jso = json_object_new_object();
    array_len = G_N_ELEMENTS(action_array);

    for (gint32 i = 0; i < array_len; i++) {
        (void)fdm_add_action_prop(o_result_jso, action_array[i]);
    }

    debug_log(DLOG_MASS, "%s: action_jso=%s.", __FUNCTION__, dal_json_object_get_str(*o_result_jso));

    return HTTP_OK;
}


LOCAL gint32 fdm_set_ret_func(gint32 ret, json_object **o_message_jso, const gchar *prop_name)
{
    gint32 ret_val = 0;

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            ret_val = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            debug_log(DLOG_INFO, "%s: Access forbidden.", __FUNCTION__);
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name);
            ret_val = HTTP_FORBIDDEN;
            break;

        case VOS_ERR:
        case RF_ERROR:
            debug_log(DLOG_INFO, "%s: Operation not support.", __FUNCTION__);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name, o_message_jso, prop_name);
            ret_val = HTTP_NOT_IMPLEMENTED;
            break;

        default: 
            debug_log(DLOG_INFO, "%s: Internal error.", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret_val;
}


LOCAL gint32 set_manager_fdmservice_auto_refresh_interval(PROVIDER_PARAS_S* i_paras,
        json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    GSList* input_list = NULL;
    guint8 auto_refresh_interval;
    gint32 refresh_interval_hours;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail((i_paras->user_role_privilege) & (USERROLE_DIAGNOSEMGNT), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_FDM_REFRESH_INTERVAL,
        o_message_jso, RFPROP_FDM_REFRESH_INTERVAL));

    return_val_do_info_if_expr(NULL == i_paras->val_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: Bad request.", __FUNCTION__));

    
    refresh_interval_hours = json_object_get_int(i_paras->val_jso);
    debug_log(DLOG_MASS, "%s:RefreshIntervalHours=%d",  __FUNCTION__, refresh_interval_hours);
    if (check_refresh_interval_valid(refresh_interval_hours) != VOS_OK) {
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_FDM_REFRESH_INTERVAL, 
            o_message_jso, RFPROP_FDM_REFRESH_INTERVAL);
        debug_log(DLOG_INFO, "%s: invalid PFAETimeDays value %d.", __FUNCTION__, refresh_interval_hours);
        return HTTP_BAD_REQUEST;
    }
    auto_refresh_interval = (guint8)get_relevant_refresh_input(refresh_interval_hours, FDM_DIRECTION_IN);
    
    input_list = g_slist_append(input_list, g_variant_new_byte(auto_refresh_interval));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_WEB_NAME, METHOD_SET_REFRESH_INTERVAL, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_REFRESH_INTERVAL);
}


LOCAL gint32 set_manager_fdmservice_pfae_time(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    guint8 pfae_time;
    gint32 pfae_time_days;
    GSList* input_list = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail((i_paras->user_role_privilege) & (USERROLE_DIAGNOSEMGNT), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_FDM_DIAGNOSE_TIME,
        o_message_jso, RFPROP_FDM_DIAGNOSE_TIME););

    return_val_do_info_if_expr(i_paras->val_jso == NULL, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: Bad request.", __FUNCTION__));
    
    pfae_time_days = json_object_get_int(i_paras->val_jso);
    debug_log(DLOG_MASS, "%s:PFAETimeDays=%d",  __FUNCTION__, pfae_time_days);
    if (check_pfae_time_valid(pfae_time_days) != VOS_OK) {
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_FDM_DIAGNOSE_TIME,
            o_message_jso, RFPROP_FDM_DIAGNOSE_TIME);
        debug_log(DLOG_INFO, "%s: invalid PFAETimeDays value %d.", __FUNCTION__, pfae_time_days);
        return HTTP_BAD_REQUEST;
    }
    pfae_time = (guint8)get_reverse_pfae_input(pfae_time_days);
    
    input_list = g_slist_append(input_list, g_variant_new_byte(pfae_time));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_WEB_NAME, METHOD_SET_PFAE_TIMES, AUTH_ENABLE, 
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_DIAGNOSE_TIME);
}


LOCAL gint32 set_manager_fdmservice_ierr_fail_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const char *policy = NULL;

    GSList *input_list = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail((i_paras->user_role_privilege) & (USERROLE_DIAGNOSEMGNT), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_FDM_DIAGNOSE_TIME, o_message_jso,
        RFPROP_FDM_DIAGNOSE_TIME););

    return_val_do_info_if_expr(NULL == i_paras->val_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: Bad request.", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(g_manager_fdmservice_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s, %d: get FdmConfig handle failed.", __FUNCTION__, __LINE__));

    
    policy = dal_json_object_get_str(i_paras->val_jso);
    debug_log(DLOG_MASS, "%s:policy=%s", __FUNCTION__, policy);

    if (0 == g_strcmp0(RF_FDM_DIAG_POLICY_NO_ACTION, policy)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_FDM_DIAG_POLICY_NO_ACTION_VALUE));
    } else if (0 == g_strcmp0(RF_FDM_DIAG_POLICY_WARM_RESET, policy)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_FDM_DIAG_POLICY_WARM_RESET_VALUE));
    } else if (0 == g_strcmp0(RF_FDM_DIAG_POLICY_POWEROFF, policy)) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_FDM_DIAG_POLICY_POWEROFF_VALUE));
    } else {
        debug_log(DLOG_ERROR, "%s:Invalid Policy Setting: %s", __FUNCTION__, policy);
        return HTTP_BAD_REQUEST;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_NAME, METHOD_FDM_SET_IERR_DIAG_FAIL_POLICY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_DIAGNOSE_FAIL_POLICY);
}


LOCAL gint32 set_manager_fdmservice_ierr_success_policy(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        debug_log(DLOG_ERROR, "%s: Access forbidden.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY,
            o_message_jso, RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY);
        return HTTP_FORBIDDEN;
    }

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Bad request.", __FUNCTION__);
        (void)create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY, o_message_jso,
            RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY);
        return HTTP_BAD_REQUEST;
    }

    const gchar *policy = json_object_get_string(i_paras->val_jso);
    if (g_strcmp0(RF_FDM_DIAG_POLICY_NO_ACTION, policy) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_FDM_DIAG_POLICY_NO_ACTION_VALUE));
    } else if (g_strcmp0(RF_FDM_DIAG_POLICY_WARM_RESET, policy) == 0) {
        input_list = g_slist_append(input_list, g_variant_new_byte(RF_FDM_DIAG_POLICY_WARM_RESET_VALUE));
    } else {
        debug_log(DLOG_ERROR, "%s:Invalid Policy Setting: %s", __FUNCTION__, policy);
        (void)create_message_info(MSGID_PROP_UNKNOWN, RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY, o_message_jso,
            RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY);
        return HTTP_BAD_REQUEST;
    }

    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_NAME, METHOD_FDM_SET_IERR_DIAG_SUCCESS_POLICY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_DIAGNOSE_SUCCESS_POLICY);
}


LOCAL gint32 set_manager_fdmservice_mem_poor_contact_alarm_enabled(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        debug_log(DLOG_ERROR, "%s: Access forbidden.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED,
            o_message_jso, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED);
        return HTTP_FORBIDDEN;
    }

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_INFO, "%s: Bad request.", __FUNCTION__);
        (void)create_message_info(MSGID_CREATE_FAILED_REQ_PROP, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED,
            o_message_jso, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED);
        return HTTP_BAD_REQUEST;
    }

    
    json_bool enable = json_object_get_boolean(i_paras->val_jso);
    guint8 notify = enable ? 1 : 0;

    
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(notify));
    
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_NAME, METHOD_FDM_SET_MEM_POOR_CONTACT_ALARM_ENABLED, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return fdm_set_ret_func(ret, o_message_jso, PROPERTY_FDM_MEM_POOR_CONTACT_ALARM_ENABLED);
}


LOCAL gint32 set_manager_fdmservice_pfae_notify_os_enable(PROVIDER_PARAS_S* i_paras,
        json_object** o_message_jso, json_object** o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_FDM_DIAGNOSE_TIME,
            o_message_jso, RFPROP_FDM_DIAGNOSE_TIME);
        return HTTP_FORBIDDEN;
    }

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_INFO, "%s: Bad request.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    
    json_bool enable = json_object_get_boolean(i_paras->val_jso);
    guint8 notify = enable ? 1 : 0;

    
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(notify));
    
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_WEB_NAME, METHOD_SET_PFAE_NOTIFY_SYSTEM_ENABLE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_PFAE_NOTIFY_OS_ENABLED);
}


LOCAL gint32 act_regenerate_report(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail((i_paras->user_role_privilege) & (USERROLE_DIAGNOSEMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s: 403.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFACTION_FDM_REGENERATE, o_message_jso,
        RFACTION_FDM_REGENERATE));

    return_val_do_info_if_expr(NULL == i_paras->val_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: 400.", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_byte(FDM_SERVICE_ENABLED));

    
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_FDM_WEB_NAME, METHOD_FDM_FRUTREE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return fdm_set_ret_func(ret, o_message_jso, RFACTION_FDM_REGENERATE);
}


LOCAL gint32 act_remove_alarm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    json_bool ret_json;
    json_object *prop_jso = NULL;
    guint32 num_key;
    GSList *output_list = NULL;
    gchar alias_str[MAX_ALIAS_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 fdm_processbar = 0;
    guint16 usProtect = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail((i_paras->user_role_privilege) & (USERROLE_DIAGNOSEMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_INFO, "%s, %d: 403.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFACTION_FDM_REMOVEALARM, o_message_jso,
        RFACTION_FDM_REMOVEALARM););

    return_val_do_info_if_expr(NULL == i_paras->val_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));
    
    
    ret_json = json_object_object_get_ex(i_paras->val_jso, PROPERTY_REMOVE_ALARM_ALIAS, &prop_jso);
    return_val_do_info_if_fail(ret_json, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: get Alias prop failed.", __FUNCTION__));

    num_key = json_object_get_int(prop_jso);
    debug_log(DLOG_MASS, "%s: Alias value=%d.", __FUNCTION__, num_key);
    
    input_list = g_slist_append(input_list, g_variant_new_byte(FDM_REMOVE_ALL_ALARM_ENABLED));
    
    input_list = g_slist_append(input_list, g_variant_new_byte(FDM_REMOVE_ALL_ALARM_ENABLED));
    
    input_list = g_slist_append(input_list, g_variant_new_uint32(num_key));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_WEB_NAME, METHOD_SET_FDM_FRUTREE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    debug_log(DLOG_MASS, "%s: Remove alarm uip ret value=%d.", __FUNCTION__, ret);
    uip_free_gvariant_list(input_list);

    if (NULL != output_list) {
        ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        uip_free_gvariant_list(output_list);
        output_list = NULL;
    }
    debug_log(DLOG_MASS, "%s: Remove alarm ret value=%d.", __FUNCTION__, ret);

    return_val_do_info_if_expr(FDM_HEALTH_REPORT_PARA_ERR == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_INFO, "%s: Input parameter error.", __FUNCTION__);
        (void)snprintf_s(alias_str, sizeof(alias_str), sizeof(alias_str) - 1, "%d", num_key);
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, RFPROP_FDM_ALIAS, o_message_jso, alias_str,
        RFPROP_FDM_ALIAS););

    
    ret = dal_get_object_handle_nth(CLASS_FDM_NAME, 0, &obj_handle);
    goto_label_do_info_if_expr(VOS_OK != ret, err_exit, (ret = HTTP_INTERNAL_SERVER_ERROR);
        debug_log(DLOG_ERROR, "%s: get FdmConfig handle failed.", __FUNCTION__));

    
    vos_task_delay(3000);
    while ((fdm_processbar < 100) && (usProtect < 100)) {
        usProtect++;
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDM_PROCESSBAR, &fdm_processbar);
        goto_label_do_info_if_expr(VOS_OK != ret, err_exit, (ret = HTTP_INTERNAL_SERVER_ERROR);
            debug_log(DLOG_ERROR, "%s: get FdmProcessbar value failed.", __FUNCTION__));
        vos_task_delay(100);
    }
    

err_exit:

    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_ALIAS);
}


LOCAL gint32 get_fdmservice_event_reports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};
    gint32 alias = 0;
    gchar alias_str[MAX_ALIAS_LEN] = {0};

    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    ret = get_alias_from_uri(i_paras->uri, &alias);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get Alias from uri failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)snprintf_s(alias_str, MAX_ALIAS_LEN, MAX_ALIAS_LEN - 1, "%d", alias);
    ret = rf_get_rsc_odata_prop(URI_FORMAT_FDM_EVENT_REPORT, (const gchar *)alias_str, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);
    return HTTP_OK;
}

LOCAL json_object *load_health_report_json(void)
{
    gint32 handle = dal_file_lock(HEALTH_REPORT_JSON_PATH, FALSE);
    if (handle == RET_ERR) {
        return NULL;
    }

    json_object *health_report = json_object_from_file(HEALTH_REPORT_JSON_PATH);
    dal_file_unlock(handle);
    return health_report;
}


LOCAL gint32 get_fdmservice_event_records(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 uri_alias = 0;
    json_object *prop_jso = NULL;
    json_object *child_jso = NULL;
    json_bool ret_json;
    guint8 depth = MAX_RECURSION_DEPTH;
    gint32 event_ret;
    gint32 maintence_ret;

    debug_log(DLOG_MASS, "%s: call get_fdmservice_event_records.", __FUNCTION__);
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: NULL pointer or frobidden.", __FUNCTION__));
    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: new object failed.", __FUNCTION__));

    guint8 display_enable = 0;
    ret = get_pfae_info_display_enable(&display_enable);
    if (ret != RET_OK || display_enable == 0) {
        debug_log(DLOG_INFO, "%s, %d: ret/display_enbale=%d/%u.", __FUNCTION__, __LINE__, ret, display_enable);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_alias_from_uri(i_paras->uri, &uri_alias);

    
    return_val_do_info_if_expr((ret != VOS_OK || uri_alias == 0), HTTP_NOT_FOUND,
        debug_log(DLOG_INFO, "%s: get Alias from uri failed or invaild Alias.", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri););

    json_object *health_report = load_health_report_json();
    if (health_report == NULL) {
        debug_log(DLOG_INFO, "%s: new object from file failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    event_ret = get_event_reports_by_alias(uri_alias, health_report, o_result_jso);
    do_info_if_true((event_ret != VOS_OK), debug_log(DLOG_MASS, "%s: event records is null.", __FUNCTION__));

    ret_json = json_object_object_get_ex(health_report, PROPERTY_FDM_DEVICE_TREE, &prop_jso);
    return_val_do_info_if_fail(ret_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(health_report);
        debug_log(DLOG_INFO, "%s: get DeviceTree prop failed.", __FUNCTION__));

    ret_json = json_object_object_get_ex(prop_jso, PROPERTY_FDM_DEVICE_CHILDREN, &child_jso);
    return_val_do_info_if_fail(ret_json, VOS_ERR, (void)json_object_put(health_report);
        debug_log(DLOG_DEBUG, "%s: get RelatedDevices prop failed.", __FUNCTION__));

    maintence_ret = get_maintence_reports_by_alias(uri_alias, &depth, child_jso, o_result_jso);
    do_info_if_true(maintence_ret != VOS_OK, debug_log(DLOG_MASS, "%s: maintence records is null.", __FUNCTION__));

    
    return_val_do_info_if_expr((event_ret != VOS_OK && maintence_ret != VOS_OK), HTTP_NOT_FOUND,
        (void)json_object_put(health_report);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri););

    return_val_do_info_if_expr(json_object_array_length(*o_result_jso) == 0, HTTP_INTERNAL_SERVER_ERROR,
        (void)json_object_put(health_report);
        debug_log(DLOG_INFO, "%s, %d: event records is empty.", __FUNCTION__, __LINE__));

    (void)json_object_put(health_report);

    return HTTP_OK;
}


LOCAL gint32 get_fdmservice_regenerate_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_id_pre[MAX_URL_LEN] = {0};
    gchar odata_id[MAX_URL_LEN] = {0};
    debug_log(DLOG_INFO, "%s: call get_fdmservice_regenerate_actioninfo_odata_id.", __FUNCTION__);
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: NULL pointer or frobidden.", __FUNCTION__));

    ret = rf_get_rsc_odata_prop(URI_FORMAT_FDMSERVICE, NULL, odata_id_pre, sizeof(odata_id_pre));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet =
        snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "%s/%s", odata_id_pre, URI_FORMAT_REGERNATE_END);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_fdmservice_removealarm_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_id_pre[MAX_URL_LEN] = {0};
    gchar odata_id[MAX_URL_LEN] = {0};
    debug_log(DLOG_INFO, "%s: call get_fdmservice_removealarm_actioninfo_odata_id.", __FUNCTION__);
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s: NULL pointer or frobidden.", __FUNCTION__));

    ret = rf_get_rsc_odata_prop(URI_FORMAT_FDMSERVICE, NULL, odata_id_pre, sizeof(odata_id_pre));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "%s/%s", odata_id_pre,
        URI_FORMAT_REMOVE_ALARM_END);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


LOCAL gint32 fdm_add_action_prop(json_object **action_prop_jso, const gchar *action_name)
{
    int iRet;
    json_object *single_action_jso = NULL;
    gchar action_name_pointer[MAX_URI_LENGTH] = {0};
    gchar target[MAX_URI_LENGTH] = {0};
    gchar actioninfo_uri[MAX_URI_LENGTH] = {0};
    gchar action_uri[MAX_URI_LENGTH] = {0};
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    if (NULL == *action_prop_jso || NULL == action_name) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: redfish_get_board_slot failed.", __FUNCTION__);
        return VOS_ERR;
    }

    single_action_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == single_action_jso, VOS_ERR,
        debug_log(DLOG_INFO, "%s: new object failed.", __FUNCTION__));

    iRet = snprintf_s(action_uri, sizeof(action_uri), sizeof(action_uri) - 1, URI_FORMAT_FDMSERVICE, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    iRet = snprintf_s(action_name_pointer, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "#%s", action_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    iRet = snprintf_s(target, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FOMRAT_ACTION_URI, action_uri, action_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    iRet = snprintf_s(actioninfo_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%sActionInfo", action_uri, action_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    debug_log(DLOG_MASS, "%s: action name=%s, target=%s, actioninfo=%s.", __FUNCTION__, action_name_pointer, target,
        actioninfo_uri);
    json_object_object_add(single_action_jso, RFPROP_TARGET, json_object_new_string((const gchar *)target));
    json_object_object_add(single_action_jso, RFPROP_ACTION_INFO,
        json_object_new_string((const gchar *)actioninfo_uri));

    debug_log(DLOG_MASS, "%s: single_action_jso=%s.", __FUNCTION__, dal_json_object_get_str(single_action_jso));
    json_object_object_add(*action_prop_jso, (const gchar *)action_name_pointer, single_action_jso);
    debug_log(DLOG_MASS, "%s: action_prop_jso=%s.", __FUNCTION__, dal_json_object_get_str(*action_prop_jso));
    return VOS_OK;
}


LOCAL gint32 get_device_info_link(gint32 alias, json_object *node_jos, gchar *odata_id)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar o_id[MAX_URL_LEN] = {0};
    gchar tmp_data[MAX_URL_LEN] = {0};

    if (NULL == odata_id || NULL == node_jos || 0 == alias) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }

    ret = get_odata_id_by_alias(alias, node_jos, o_id);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get odata_id prop failed.", __FUNCTION__);
        return VOS_ERR;
    }

    (void)fdm_erase_blank(o_id, tmp_data, sizeof(tmp_data));
    safe_fun_ret = strncpy_s(odata_id, sizeof(tmp_data), tmp_data, sizeof(tmp_data) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    debug_log(DLOG_MASS, "%s, %d: get device_link odata_id value:%s.", __FUNCTION__, __LINE__, odata_id);
    return VOS_OK;
}


LOCAL gint32 get_event_report_link(gchar *alias, gchar *odata_id, guint32 odata_size)
{
    gint32 ret;
    gchar o_id[MAX_URL_LEN] = {0};

    if (NULL == odata_id || NULL == alias) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }

    ret = rf_get_rsc_odata_prop(MANAGER_FDMSERVICE_EVENTREPORT, (const gchar *)alias, o_id, sizeof(o_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s: get odata_id prop failed.", __FUNCTION__);
        return VOS_ERR;
    }

    return_val_do_info_if_expr(odata_size < MAX_URL_LEN, VOS_ERR,
        debug_log(DLOG_ERROR, "invalid length = %d\n", odata_size));
    if (EOK != strncpy_s(odata_id, odata_size, o_id, sizeof(o_id) - 1)) {
        debug_log(DLOG_ERROR, "strncpy_s failed");
        return VOS_ERR;
    }
    debug_log(DLOG_MASS, "%s, %d: get report_link odata_id value:%s.", __FUNCTION__, __LINE__, odata_id);
    return VOS_OK;
}


LOCAL gint32 get_silk_name_by_alias(json_object *node_jso, gint32 alias, gchar *silk_name)
{
    json_object *sub_prop_jso = NULL;
    json_bool ret_json;
    gint32 array_len;

    return_val_do_info_if_fail(NULL != node_jso || NULL != silk_name, VOS_ERR,
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));

    array_len = json_object_object_length(node_jso);
    return_val_do_info_if_expr(0 == array_len, VOS_ERR,
        debug_log(DLOG_INFO, "%s: input json object length = 0.", __FUNCTION__));

    ret_json = json_object_object_get_ex(node_jso, PROPERTY_FDM_SILK_NAME, &sub_prop_jso);
    return_val_do_info_if_fail(ret_json, VOS_ERR, debug_log(DLOG_INFO, "%s: get SilkName prop failed.", __FUNCTION__));

    (void)strncpy_s(silk_name, MAX_SILK_NAME_LEN, dal_json_object_get_str(sub_prop_jso), MAX_SILK_NAME_LEN - 1);

    debug_log(DLOG_MASS, "%s: get SilkName value:%s.", __FUNCTION__, silk_name);
    return VOS_OK;
}


LOCAL gint32 get_component_id_by_alias(json_object *node_jso, gint32 alias, guint8 *component_id)
{
    gint32 array_len;
    json_bool ret_json;
    json_object *sub_prop_jso = NULL;

    return_val_do_info_if_expr(NULL == node_jso || NULL == component_id, VOS_ERR,
        debug_log(DLOG_INFO, "%s: NULL pointer", __FUNCTION__));

    array_len = json_object_object_length(node_jso);
    return_val_do_info_if_expr(0 == array_len, VOS_ERR,
        debug_log(DLOG_INFO, "%s: input json object length = 0", __FUNCTION__));

    ret_json = json_object_object_get_ex(node_jso, PROPERTY_FDM_COMPONENT_ID, &sub_prop_jso);
    return_val_do_info_if_expr(TRUE != ret_json, VOS_ERR,
        debug_log(DLOG_INFO, "%s: get component id prop failed", __FUNCTION__));

    *component_id = (guint8)json_object_get_int(sub_prop_jso);

    debug_log(DLOG_MASS, "%s: get component value: %d", __FUNCTION__, *component_id);

    return VOS_OK;
}


LOCAL gint32 get_odata_id_by_alias(gint32 alias, json_object *node_jos, gchar *odata_id)
{
    gint32 ret;
    gchar url_pre[MAX_URL_LEN] = {0};
    gchar url_end[MAX_URL_END_LEN] = {0};
    gchar silk_name[MAX_SILK_NAME_LEN] = {0};
    guint8 component_id = 0;

    return_val_do_info_if_expr((NULL == odata_id || NULL == node_jos), VOS_ERR,
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));

    ret = get_silk_name_by_alias(node_jos, alias, silk_name);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_INFO, "%s: get silkname by alias failed.", __FUNCTION__));

    
    ret = get_component_id_by_alias(node_jos, alias, &component_id);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_INFO, "%s: get component id by alias failed.", __FUNCTION__));
    

    if (VOS_OK == get_odata_id_pre(component_id, url_pre) &&
        VOS_OK == get_odata_id_end(silk_name, url_end, sizeof(url_end))) {
        ret = snprintf_s(odata_id, MAX_URL_LEN, MAX_URL_LEN - 1, "%s/%s", url_pre, url_end);
        return_val_do_info_if_fail(ret > 0, VOS_ERR,
            debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
        debug_log(DLOG_MASS, "%s, %d: odata_id=%s.", __FUNCTION__, __LINE__, odata_id);
        return VOS_OK;
    }
    return VOS_ERR;
}


LOCAL gint32 fdm_erase_blank(const gchar *src_name, gchar *dst_name, guint32 dst_name_size)
{
    guint32 pos = 0;

    return_val_do_info_if_expr(NULL == src_name || NULL == dst_name, VOS_ERR,
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));

    for (gint32 i = 0; src_name[i] != '\0'; i++) {
        if (src_name[i] != ' ') {
            return_val_do_info_if_expr(pos >= (dst_name_size - 1), VOS_ERR,
                debug_log(DLOG_INFO, "%s: dst_name buffer OutOfRange.", __FUNCTION__));
            dst_name[pos] = src_name[i];
            pos++;
        }
    }
    dst_name[pos] = '\0';
    return VOS_OK;
}


LOCAL gint32 get_delimiter_pos(const gchar *src)
{
    gint32 tmp = 0;
    if (NULL == src) {
        return VOS_OK;
    }
    while (*src != '\0') {
        if ((*src > '9') || (*src < '0')) {
            tmp++;
            src++;
        } else {
            return (tmp);
        }
    }
    return (tmp);
}


LOCAL gint32 get_continuous_num(const gchar *src, gchar *num_str, gint32 num_str_size)
{
    gchar str[MAX_SILK_NAME_LEN] = {"0"};

    if (NULL == src || NULL == num_str) {
        return VOS_OK;
    }

    (void)memset_s(str, sizeof(str), '\0', sizeof(str));
    gint32 pos1 = get_delimiter_pos(src);
    return_val_if_expr((pos1 + 1 > strlen(src) || VOS_OK == pos1), VOS_OK);
    gint32 pos2 = pos1;
    gint32 tmp = pos1;
    errno_t safe_fun_ret = strncpy_s(str, sizeof(str), src + pos1, strlen(src) - pos1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    for (gint32 i = 0; str[i] != '\0'; i++) {
        if ((str[i] <= '9') && (str[i] >= '0') && tmp == pos2) {
            pos2++;
        }
        tmp++;
    }

    safe_fun_ret = strncpy_s(num_str, num_str_size, src + pos1, pos2 - pos1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    debug_log(DLOG_MASS, "%s: num_str=%s,pos1=%d,pos2=%d.", __FUNCTION__, num_str, pos1, pos2);
    return (pos2 - pos1);
}


LOCAL gint32 get_split_name(const gchar *src_name, gchar *str_pre, gint32 *num, gchar *str_end)
{
    errno_t safe_fun_ret;
    gint32 pos;
    gchar num_str[MAX_SILK_NAME_LEN] = {"0"};
    pos = get_delimiter_pos(src_name);
    if (pos == 0) {
        safe_fun_ret = strncpy_s(str_pre, strlen(src_name) + 1, src_name, strlen(src_name));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else {
        safe_fun_ret = strncpy_s(str_pre, pos + 1, src_name, pos);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    gint32 ret = get_continuous_num(src_name, num_str, sizeof(num_str));
    if (VOS_OK != ret) {
        gint sscanf_cnt = sscanf_s(num_str, "%d", num);
        return_val_do_info_if_expr(1 != sscanf_cnt, VOS_ERR,
            debug_log(DLOG_ERROR, "sscanf_s failed. sscanf_cnt = %d\n", sscanf_cnt));
        safe_fun_ret = strncpy_s(str_end, strlen(src_name) - strlen(str_pre) - strlen(num_str) + 1,
            src_name + strlen(str_pre) + strlen(num_str), strlen(src_name) - strlen(str_pre) - strlen(num_str));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else {
        *num = 0;
    }
    debug_log(DLOG_MASS, "%s: first part=%s,end part=%s,num=%d.", __FUNCTION__, str_pre, str_end, *num);
    return VOS_OK;
}


LOCAL gint32 get_url_format_by_component_id(guint8 component_id, gchar *url_format, guint32 url_format_size)
{
    errno_t safe_fun_ret;

    INT2STR_MAP_S component_url_map[] = {
        {COMPONENT_TYPE_MAINBOARD,        URI_FORMAT_BOARD_PRE},
        {COMPONENT_TYPE_PS,                        URI_FORMAT_PS_PRE},
        {COMPONENT_TYPE_CPU,                     URI_FORMAT_CPU_PRE},
        {COMPONENT_TYPE_MEMORY,             URI_FORMAT_DIMM_PRE},
        {COMPONENT_TYPE_FAN,                     URI_FORMAT_FAN_PRE},
        {COMPONENT_TYPE_HDD_BACKPLANE, URI_FORMAT_BOARD_PRE},
        {COMPONENT_TYPE_NIC_CARD,            URI_FORMAT_NIC_PRE},
        {COMPONENT_TYPE_PCIE_RISER,           URI_FORMAT_BOARD_PRE},
        {COMPONENT_TYPE_RAID_CARD,          URI_FORMAT_BOARD_PRE},
        {COMPONENT_TYPE_HARDDISK,            URI_FORMAT_DISK_PRE},
        {COMPONENT_TYPE_PCIE_CARD,           URI_FORMAT_PCIE_PRE},
        {COMPONENT_TYPE_CHASSIS,               URI_FORMAT_CHASSIS_PRE},
                                          
        {COMPONENT_TYPE_LEAKDET_CARD,           URI_FORMAT_BOARD_PRE}
    };

    return_val_do_info_if_expr(NULL == url_format || 0 == url_format_size, VOS_ERR,
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));

    debug_log(DLOG_MASS, "%s: component_id=%d.", __FUNCTION__, component_id);

    gint32 size = G_N_ELEMENTS(component_url_map);

    for (gint32 i = 0; i < size; i++) {
        if (component_id == component_url_map[i].int_val) {
            debug_log(DLOG_MASS, "%s: component id=%d, uri_head=%s.", __FUNCTION__, component_url_map[i].int_val,
                component_url_map[i].str_val);
            safe_fun_ret = strncpy_s(url_format, url_format_size, component_url_map[i].str_val, url_format_size - 1);
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            debug_log(DLOG_MASS, "%s, %d: uri format=%s.", __FUNCTION__, __LINE__, url_format);
            return VOS_OK;
        }
    }

    safe_fun_ret = strncpy_s(url_format, url_format_size, URI_FORMAT_BOARD_PRE, strlen(URI_FORMAT_BOARD_PRE));
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));
    debug_log(DLOG_MASS, "%s: return default uri format.", __FUNCTION__);
    return VOS_OK;
}


LOCAL gint32 get_odata_id_pre(guint8 component_id, gchar *url_pre)
{
    gchar tmp_name[MAX_URL_LEN] = {0};

    return_val_do_info_if_expr(NULL == url_pre, VOS_ERR, debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));

    debug_log(DLOG_MASS, "%s, %d: get odata_id first part.", __FUNCTION__, __LINE__);
    gint32 ret = get_url_format_by_component_id(component_id, tmp_name, sizeof(tmp_name));
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_INFO, "%s, %d: get header of odata_id failed.", __FUNCTION__, __LINE__));

    ret = rf_get_rsc_odata_prop(tmp_name, NULL, url_pre, sizeof(tmp_name));
    debug_log(DLOG_INFO, "%s, %d: uri_pre_format=%s,uri=%s.", __FUNCTION__, __LINE__, tmp_name, url_pre);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s, %d: get odata_id pre failed.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    debug_log(DLOG_MASS, "%s, %d: get header of odata_id value:%s.", __FUNCTION__, __LINE__, url_pre);
    return VOS_OK;
}


LOCAL gint32 get_odata_id_end(const gchar *silk_name, gchar *url_end, guint32 url_end_size)
{
    int iRet = -1;
    gint32 ret;
    gint32 id = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *obj_lst = NULL;
    GSList *obj_ptr = NULL;
    gchar device_name[MAX_SILK_NAME_LEN] = {0};
    gchar tmp_name[MAX_SILK_NAME_LEN] = {0};
    gchar name_pre[MAX_SILK_NAME_LEN] = {0};
    gchar name_end[MAX_SILK_NAME_LEN] = {0};
    gchar silk_name_no_blank[MAX_SILK_NAME_LEN] = {0};

    return_val_do_info_if_expr(!silk_name || !url_end, VOS_ERR,
        debug_log(DLOG_INFO, "%s: NULL pointer.", __FUNCTION__));
    return_val_do_info_if_expr(url_end_size == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: input param url_end_size error, equal to 0.", __FUNCTION__));

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_lst);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        do_if_expr(obj_lst != NULL, g_slist_free(obj_lst); obj_lst = NULL);
        debug_log(DLOG_INFO, "%s: dfl get object list failed.", __FUNCTION__));

    for (obj_ptr = obj_lst; obj_ptr != NULL; obj_ptr = obj_ptr->next) {
        obj_handle = (OBJ_HANDLE)obj_ptr->data;
        ret = dal_get_property_value_string(obj_handle, PROPERTY_TYPE_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            do_if_expr(obj_lst != NULL, g_slist_free(obj_lst); obj_lst = NULL);
            debug_log(DLOG_INFO, "%s: get component DeviceName failed.", __FUNCTION__));

        if (g_strcmp0(silk_name, device_name) == 0) {
            if (obj_lst != NULL) {
                g_slist_free(obj_lst);
                obj_lst = NULL;
            }
            (void)get_split_name(silk_name, name_pre, &id, name_end);
            return_val_do_info_if_expr(0 == strlen(name_pre), VOS_ERR,
                debug_log(DLOG_INFO, "%s, %d: get end part from device_name failed.", __FUNCTION__, __LINE__));
            
            (void)fdm_erase_blank(device_name, silk_name_no_blank, sizeof(silk_name_no_blank));
            if (NULL != strstr(silk_name_no_blank, DEVICE_NAME_PCIE_STRING)) {
                ret = snprintf_s(tmp_name, sizeof(tmp_name), sizeof(tmp_name) - 1, "%s", name_pre);
                do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));
            } else {
                ret = snprintf_s(tmp_name, sizeof(tmp_name), sizeof(tmp_name) - 1, "%s%s", name_pre, name_end);
                do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));
            }
            
            (void)memset_s(name_end, sizeof(name_end), 0, sizeof(name_end));
            (void)fdm_erase_blank(tmp_name, name_end, sizeof(name_end));
            debug_log(DLOG_MASS, "%s: component device_name=%s.", __FUNCTION__, name_end);

            if (g_strcmp0(DEVICE_NAME_PSU_STRING, name_end) == 0) {
                (void)snprintf_s(url_end, url_end_size, url_end_size - 1, "%d", id - 1);
                debug_log(DLOG_MASS, "%s: uri_end=%s.", __FUNCTION__, url_end);
                return VOS_OK;
            } else if (g_strcmp0(DEVICE_NAME_CPU_STRING, name_end) == 0) {
                (void)snprintf_s(url_end, url_end_size, url_end_size - 1, "%d", id);
                debug_log(DLOG_MASS, "%s: uri_end=%s.", __FUNCTION__, url_end);
                return VOS_OK;
            } else if (g_strcmp0(DEVICE_NAME_FAN_STRING, name_end) == 0) {
                (void)snprintf_s(url_end, url_end_size, url_end_size - 1, "%d", id - 1);
                debug_log(DLOG_MASS, "%s: uri_end=%s.", __FUNCTION__, url_end);
                return VOS_OK;
            } else if (g_strcmp0(DEVICE_NAME_PCIE_STRING, name_end) == 0) {
                iRet = snprintf_s(url_end, url_end_size, url_end_size - 1, "%s%d", DEVICE_NAME_PCIE_STRING, id);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
                debug_log(DLOG_MASS, "%s: uri_end=%s.", __FUNCTION__, url_end);
                return VOS_OK;
            } else {
                ret = dal_get_property_value_string(obj_handle, PROPERTY_TYPE_COMPONENT_LOCATION, name_pre,
                    sizeof(name_pre));
                return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
                    debug_log(DLOG_INFO, "%s: get component DeviceName failed.", __FUNCTION__));

                return_val_do_info_if_expr(fdm_erase_blank(device_name, name_end, sizeof(name_end)) == VOS_ERR, VOS_ERR,
                    debug_log(DLOG_INFO, "%s, %d: get url_end failed.", __FUNCTION__, __LINE__));

                iRet = snprintf_s(url_end, url_end_size, url_end_size - 1, "%s%s", name_pre, name_end);
                do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
                debug_log(DLOG_MASS, "%s: uri_end=%s.", __FUNCTION__, url_end);
                return VOS_OK;
            }
        }
    }

    if (obj_lst != NULL) {
        g_slist_free(obj_lst);
        obj_lst = NULL;
    }
    debug_log(DLOG_INFO, "%s: get prop failed.", __FUNCTION__);
    return VOS_ERR;
}


LOCAL gint32 get_event_reports_by_alias(gint32 alias, json_object *health_report, json_object **reports_jso)
{
    json_object *prop_jso = NULL;
    json_object *sub_child_jso = NULL;
    json_object *object_jso = NULL;

    // 入参检查
    if (NULL == reports_jso || NULL == health_report) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }
    // 从请求消息 的对象中获取HealthReport 的json对象
    json_bool ret_json = json_object_object_get_ex(health_report, PROPERTY_FDM_HEALTH_REPORT, &prop_jso);
    return_val_do_info_if_fail(ret_json, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: get HealthReport prop failed.", __FUNCTION__));

    gint32 array_len = json_object_array_length(prop_jso);
    return_val_do_info_if_expr(0 == array_len, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d: input json object length = 0.", __FUNCTION__, __LINE__));
    // 遍历对象，获取HealthReport对象的每一个子对象
    for (gint32 array_idx = 0; array_idx < array_len; array_idx++) {
        json_object *sub_prop_jso = json_object_array_get_idx(prop_jso, array_idx);
        ret_json = json_object_object_get_ex(sub_prop_jso, PROPERTY_FDM_ALIAS, &object_jso);
        return_val_do_info_if_fail(ret_json, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: get Alias prop failed.", __FUNCTION__));

        debug_log(DLOG_DEBUG, "%s, %d: alias prop=%d.", __FUNCTION__, __LINE__, json_object_get_int(object_jso));
        if (alias == json_object_get_int(object_jso)) {
            ret_json = json_object_object_get_ex(sub_prop_jso, PROPERTY_FDM_EVENT_REPORT, &sub_child_jso);
            return_val_do_info_if_fail(ret_json, VOS_ERR,
                debug_log(DLOG_DEBUG, "%s: get EventRecords prop failed.", __FUNCTION__));

            gint32 child_array_len = json_object_array_length(sub_child_jso);
            return_val_do_info_if_expr(0 == child_array_len, VOS_ERR,
                debug_log(DLOG_DEBUG, "%s, %d: event report json object length = 0.", __FUNCTION__, __LINE__));

            for (gint32 child_array_idx = 0; child_array_idx < child_array_len; child_array_idx++) {
                json_object *leaf_jso = json_object_array_get_idx(sub_child_jso, child_array_idx);
                return_val_do_info_if_expr(NULL == leaf_jso, VOS_ERR,
                    debug_log(DLOG_INFO, "%s, %d: new object failed.", __FUNCTION__, __LINE__));
                (void)json_object_array_add(*reports_jso, json_object_get(leaf_jso));
            }

            return VOS_OK;
        }
    }
    debug_log(DLOG_INFO, "%s: get prop failed.", __FUNCTION__);
    return VOS_ERR;
}


LOCAL gint32 get_maintence_reports_by_alias(gint32 alias, guint8 *depth, json_object *prop_jso,
    json_object **reports_jso)
{
    json_object *maintence_jso = NULL;
    json_object *leaf_jso = NULL;
    json_object *alias_jso = NULL;
    gint32 ret = 0;

    gint32 array_len = json_object_array_length(prop_jso);
    return_val_do_info_if_expr(0 == array_len, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: input json object length = 0.", __FUNCTION__, __LINE__));

    *depth = *depth - 1;

    for (gint32 array_idx = 0; array_idx < array_len; array_idx++) {
        json_object *sub_child_jso = json_object_array_get_idx(prop_jso, array_idx);
        json_bool ret_json = json_object_object_get_ex(sub_child_jso, PROPERTY_FDM_ALIAS, &alias_jso);
        return_val_do_info_if_fail(ret_json, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: get Alias prop failed.", __FUNCTION__));

        if (alias == json_object_get_int(alias_jso)) {
            ret_json = json_object_object_get_ex(sub_child_jso, PROPERTY_FDM_MAINTENCE_REPORT, &maintence_jso);

            gint32 child_array_len = json_object_array_length(maintence_jso);
            return_val_do_info_if_expr(0 == child_array_len, VOS_ERR,
                debug_log(DLOG_DEBUG, "%s, %d: maintence report json object length = 0.", __FUNCTION__, __LINE__));

            for (gint32 child_array_idx = 0; child_array_idx < child_array_len; child_array_idx++) {
                json_object *object_jso = json_object_array_get_idx(maintence_jso, child_array_idx);
                (void)json_object_array_add(*reports_jso, json_object_get(object_jso));
            }
            return VOS_OK;
        }

        ret_json = json_object_object_get_ex(sub_child_jso, PROPERTY_FDM_DEVICE_CHILDREN, &leaf_jso);
        if (ret_json) {
            ret = get_maintence_reports_by_alias(alias, depth, leaf_jso, reports_jso);
            do_if_true(VOS_OK == ret, return VOS_OK);
        }
    }

    debug_log(DLOG_INFO, "%s, %d: get prop failed.", __FUNCTION__, __LINE__);
    return VOS_ERR;
}


LOCAL gint32 get_online_timestamp(json_object *input_jso, json_object *output_jso)
{
    json_object *maint_history_jso = NULL;
    json_object *prop_jso = NULL;

    return_val_do_info_if_expr(NULL == input_jso || NULL == output_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (FALSE == json_object_object_get_ex(input_jso, PROPERTY_FDM_MAINTENCE_REPORT, &maint_history_jso)) {
        debug_log(DLOG_MASS, "%s: %s not found.", __FUNCTION__, PROPERTY_FDM_MAINTENCE_REPORT);
        return VOS_ERR;
    }

    if (0 >= json_object_array_length(maint_history_jso)) {
        debug_log(DLOG_MASS, "%s: %s is empty.", __FUNCTION__, PROPERTY_FDM_MAINTENCE_REPORT);
        return VOS_ERR;
    }

    
    json_object *sub_jso = json_object_array_get_idx(maint_history_jso, 0);
    return_val_do_info_if_expr(NULL == sub_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_array_get_idx failed.", __FUNCTION__));

    if (FALSE == json_object_object_get_ex(sub_jso, PROPERTY_FDM_COMPONENT_MAINTENCE_TINE, &prop_jso)) {
        debug_log(DLOG_MASS, "%s: %s not found.", __FUNCTION__, PROPERTY_FDM_COMPONENT_MAINTENCE_TINE);
        return VOS_ERR;
    }

    gint64 time_stamp = json_object_get_int64(prop_jso);
    return_val_do_info_if_expr(time_stamp < 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_get_int64 failed.", __FUNCTION__));
    json_object_object_add(output_jso, PROPERTY_FDM_ONLINE_TIME_STAMP, json_object_new_int64(time_stamp));

    prop_jso = NULL;
    if (FALSE == json_object_object_get_ex(sub_jso, PROPERTY_FDM_COMPONENT_MAINTENCE_TIME_ZONE, &prop_jso)) {
        debug_log(DLOG_MASS, "%s: %s not found.", __FUNCTION__, PROPERTY_FDM_COMPONENT_MAINTENCE_TIME_ZONE);
        return VOS_ERR;
    }

    gint32 time_zone = json_object_get_int(prop_jso);
    return_val_do_info_if_expr((time_zone > MAX_TIME_ZONE) || (time_zone < MIN_TIME_ZONE), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_get_int failed.", __FUNCTION__));
    json_object_object_add(output_jso, PROPERTY_FDM_ONLINE_TIME_ZONE, json_object_new_int(time_zone));

    return VOS_OK;
}


LOCAL gint32 get_related_devices(json_object *node_jso, json_object **reformed_jso, guint8 *depth)
{
    json_object *prop_jso = NULL;
    json_object *sub_prop_jso = NULL;
    json_bool ret_json = FALSE;
    gint32 alias = 0;
    gchar alias_str[MAX_ALIAS_LEN] = {0};
    gchar odata_id[MAX_URL_LEN] = {0};

    return_val_do_info_if_expr(NULL == node_jso || NULL == *reformed_jso, VOS_ERR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_expr(1 > *depth || MAX_RECURSION_DEPTH < *depth, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: over max recursion depth, current %d.", __FUNCTION__, __LINE__, *depth));

    *depth = *depth - 1;

    gint32 len = json_object_array_length(node_jso);
    return_val_do_info_if_expr(0 == len, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s, %d: input json object length = 0.", __FUNCTION__, __LINE__));

    for (gint32 idx = 0; idx < len; idx++) {
        debug_log(DLOG_MASS, "%s, %d: get child index %d.", __FUNCTION__, __LINE__, idx);
        json_object *message_info_jso = json_object_new_object();
        return_val_do_info_if_expr(NULL == message_info_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_INFO, "%s, %d: new object failed.", __FUNCTION__, __LINE__));

        json_object *leaf_jso = json_object_array_get_idx(node_jso, idx);
        ret_json = json_object_object_get_ex(leaf_jso, PROPERTY_FDM_SILK_NAME, &prop_jso);
        if (FALSE == ret_json) {
            debug_log(DLOG_INFO, "%s, %d: get prop failed.", __FUNCTION__, __LINE__);
            (void)json_object_put(message_info_jso);
            return VOS_ERR;
        } else {
            json_object_object_add(message_info_jso, RFPROP_FDM_SILK_NAME, json_object_get(prop_jso));
        }

        ret_json = json_object_object_get_ex(leaf_jso, PROPERTY_FDM_COMPONENT_TYPE, &prop_jso);
        if (FALSE == ret_json) {
            debug_log(DLOG_INFO, "%s, %d: get prop failed.", __FUNCTION__, __LINE__);
            (void)json_object_put(message_info_jso);
            return VOS_ERR;
        } else {
            json_object_object_add(message_info_jso, RFPROP_FDM_COMPONENT_TYPE, json_object_get(prop_jso));
        }

        ret_json = json_object_object_get_ex(leaf_jso, PROPERTY_FDM_COMPONENT_ID, &prop_jso);
        if (FALSE == ret_json) {
            debug_log(DLOG_INFO, "%s, %d: get prop failed.", __FUNCTION__, __LINE__);
            (void)json_object_put(message_info_jso);
            return VOS_ERR;
        } else {
            json_object_object_add(message_info_jso, RFPROP_FDM_COMPONENT_ID, json_object_get(prop_jso));
        }

        ret_json = json_object_object_get_ex(leaf_jso, PROPERTY_FDM_HEALTH_STATUS, &prop_jso);
        if (FALSE == ret_json) {
            debug_log(DLOG_INFO, "%s, %d: get prop failed.", __FUNCTION__, __LINE__);
            (void)json_object_put(message_info_jso);
            return VOS_ERR;
        } else {
            json_object_object_add(message_info_jso, RFPROP_FDM_HEALTH_STATUS, json_object_get(prop_jso));
        }

        ret_json = json_object_object_get_ex(leaf_jso, PROPERTY_FDM_ALIAS, &prop_jso);
        if (FALSE == ret_json) {
            debug_log(DLOG_INFO, "%s, %d: get prop failed.", __FUNCTION__, __LINE__);
            (void)json_object_put(message_info_jso);
            return VOS_ERR;
        } else {
            json_object_object_add(message_info_jso, RFPROP_FDM_ALIAS, json_object_get(prop_jso));

            alias = json_object_get_int(prop_jso);
            if (0 != alias) {
                (void)snprintf_s(alias_str, sizeof(alias_str), sizeof(alias_str) - 1, "%d", alias);
                debug_log(DLOG_MASS, "%s, %d: find alias %d associated link.", __FUNCTION__, __LINE__, alias);
                (void)memset_s(odata_id, sizeof(odata_id), 0, sizeof(odata_id));
                do_info_if_true(VOS_OK == get_event_report_link(alias_str, odata_id, sizeof(odata_id)),
                    json_object_object_add(message_info_jso, RFPROP_FDM_EVENT_REPORT,
                    json_object_new_string((const gchar *)odata_id)));

                (void)memset_s(odata_id, sizeof(odata_id), 0, sizeof(odata_id));
                do_info_if_true(VOS_OK == get_device_info_link(alias, leaf_jso, odata_id), json_object_object_add(
                    message_info_jso, RFPROP_FDM_DEVICE_INFO, json_object_new_string((const gchar *)odata_id)));
            }
        }

        
        return_val_do_info_if_expr(VOS_OK != get_online_timestamp(leaf_jso, message_info_jso), VOS_ERR,
            (void)json_object_put(message_info_jso);
            debug_log(DLOG_INFO, "%s: get_online_timestamp failed.", __FUNCTION__));
        

        ret_json = json_object_object_get_ex(leaf_jso, PROPERTY_FDM_DEVICE_CHILDREN, &sub_prop_jso);
        if (FALSE != ret_json) {
            json_object *array_child_jso = json_object_new_array();
            return_val_do_info_if_expr(NULL == array_child_jso, HTTP_INTERNAL_SERVER_ERROR,
                (void)json_object_put(message_info_jso);
                debug_log(DLOG_INFO, "%s, %d: new object failed.", __FUNCTION__, __LINE__));
            if (VOS_OK == get_related_devices(sub_prop_jso, &array_child_jso, depth)) {
                json_object_object_add(message_info_jso, RFPROP_FDM_RELEATED_DEVICE, array_child_jso);
                debug_log(DLOG_MASS, "%s: child_jso=%s.", __FUNCTION__, dal_json_object_get_str(array_child_jso));
            } else {
                json_object_put(array_child_jso);
            }
        }

        if (0 != json_object_object_length(message_info_jso)) {
            debug_log(DLOG_MASS, "%s: index %d, devtree child length=%d, str=%s .", __FUNCTION__, idx,
                json_object_object_length(message_info_jso), dal_json_object_get_str(message_info_jso));
            return_val_do_info_if_expr(VOS_OK != json_object_array_add(*reformed_jso, message_info_jso), VOS_ERR,
                (void)json_object_put(message_info_jso);
                debug_log(DLOG_INFO, "%s, %d: add object failed.", __FUNCTION__, __LINE__));
            debug_log(DLOG_MASS, "%s: message_info_jso=%s.", __FUNCTION__, dal_json_object_get_str(message_info_jso));
            debug_log(DLOG_MASS, "%s: reformed_jso=%s.", __FUNCTION__, dal_json_object_get_str(*reformed_jso));
        } else {
            debug_log(DLOG_INFO, "%s: devtree child length=0, index %d, .", __FUNCTION__, idx);
            (void)json_object_put(message_info_jso);
        }
    }
    debug_log(DLOG_MASS, "%s, %d: exit get child method.", __FUNCTION__, __LINE__);

    return VOS_OK;
}

LOCAL gint32 fdm_check_fdmservice_uri_valid(gchar *uri_string, const gchar *URI_FORMAT)
{
    int iRet;
    gchar board_slot[MAX_RSC_NAME_LEN];
    gchar fdmservice_uri_prefix[MAX_URI_LENGTH];
    gint32 match_flag = VOS_ERR;

    (void)memset_s(board_slot, MAX_RSC_NAME_LEN, 0, MAX_RSC_NAME_LEN);
    gint32 ret = redfish_get_board_slot(board_slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_INFO, "%s,%d: get board slot id fail. ", __FUNCTION__, __LINE__));

    (void)memset_s(fdmservice_uri_prefix, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(fdmservice_uri_prefix, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT, board_slot);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    gint32 fdmservice_uri_len = strlen(uri_string);
    gint32 fdmservice_uri_prefix_len = strlen(fdmservice_uri_prefix);
    if (fdmservice_uri_len >= fdmservice_uri_prefix_len) {
        if (0 == g_ascii_strncasecmp(uri_string, fdmservice_uri_prefix, fdmservice_uri_prefix_len)) {
            if (('\0' == *(uri_string + fdmservice_uri_prefix_len)) ||
                ('/' == *(uri_string + fdmservice_uri_prefix_len))) {
                match_flag = VOS_OK;
            }
        }
    }

    return match_flag;
}


LOCAL gint32 get_alias_from_uri(gchar *uri, gint32 *alias)
{
    gchar alias_str[MAX_ALIAS_LEN] = {0};
    char *pos_fdm = NULL;
    char *pos_event = NULL;
    gint sscanf_cnt;

    debug_log(DLOG_DEBUG, "%s, %d: uri=%s.", __FUNCTION__, __LINE__, uri);
    (void)memset_s(alias_str, sizeof(alias_str), '\0', sizeof(alias_str));

    gchar *uri_lower = g_ascii_strdown(uri, strlen(uri));
    check_fail_do_return_val(NULL != uri_lower, VOS_ERR, debug_log(DLOG_INFO, "cann't get uri name."));

    pos_fdm = strstr(uri_lower, URI_FDMSERVICE);
    pos_event = strstr(uri_lower, URI_EVENT_REPORT);
    return_val_do_info_if_expr(NULL == pos_fdm || NULL == pos_event, VOS_ERR,
        do_if_expr(NULL != uri_lower, g_free(uri_lower); uri_lower = NULL);
        debug_log(DLOG_DEBUG, "%s, %d: find separator %s or %s failed.", __FUNCTION__, __LINE__, URI_FDMSERVICE,
        URI_EVENT_REPORT));

    gint32 alias_len = pos_event - pos_fdm - strlen(URI_FDMSERVICE) - 2;
    return_val_do_info_if_expr(alias_len > MAX_ALIAS_LEN || alias_len <= 0, VOS_ERR,
        do_if_expr(NULL != uri_lower, g_free(uri_lower); uri_lower = NULL););
    errno_t safe_fun_ret = strncpy_s(alias_str, sizeof(alias_str),
        pos_fdm + strlen(URI_FDMSERVICE) + 1, alias_len);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    return_val_do_info_if_expr((alias_str[0] == '\0' || alias_str[0] == '0'), VOS_ERR,
        do_if_expr(NULL != uri_lower, g_free(uri_lower); uri_lower = NULL);
        debug_log(DLOG_DEBUG, "%s, %d: get alias from uri failed.", __FUNCTION__, __LINE__));

    sscanf_cnt = sscanf_s(alias_str, "%d", alias);
    do_if_expr(1 != sscanf_cnt, debug_log(DLOG_ERROR, "sscanf_s failed. sscanf_cnt = %d\n", sscanf_cnt));
    debug_log(DLOG_DEBUG, "%s, %d: alias_str=%s, alias=%d.", __FUNCTION__, __LINE__, alias_str, *alias);
    do_if_expr(NULL != uri_lower, g_free(uri_lower); uri_lower = NULL);
    return VOS_OK;
}


LOCAL gint32 check_refresh_interval_valid(gint32 val)
{
    
    LOCAL gint32 refresh_array_int[] = {1, 24, 168};
    for (guint32 i = 0; i < sizeof(refresh_array_int) / sizeof(refresh_array_int[0]); i++) {
        return_val_if_expr(refresh_array_int[i] == val, VOS_OK);
    }
    return VOS_ERR;
}


LOCAL gint32 check_pfae_time_valid(gint32 val)
{
    
    LOCAL gint32 pfae_array_int[] = {12, 20, 30};
    for (guint32 i = 0; i < sizeof(pfae_array_int) / sizeof(pfae_array_int[0]); i++) {
        return_val_if_expr(pfae_array_int[i] == val, VOS_OK);
    }
    return VOS_ERR;
}


LOCAL gint32 check_fdm_report_file_exist(PROVIDER_PARAS_S *i_paras)
{
    GSList *input_list = NULL;
    gint8 generate_mode = 0; // 传递自动或手动生成 自动为0，手动为1

    gboolean file_exsit = vos_get_file_accessible(HEALTH_REPORT_JSON_PATH);
    if (TRUE != file_exsit) {
        input_list = g_slist_append(input_list, g_variant_new_byte(generate_mode));

        
        gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            i_paras->obj_handle, CLASS_FDM_WEB_NAME, METHOD_FDM_FRUTREE, AUTH_ENABLE, i_paras->user_role_privilege,
            input_list, NULL);

        uip_free_gvariant_list(input_list);

        if (RFERR_SUCCESS != ret && VOS_OK != ret) {
            debug_log(DLOG_INFO, "%s:fdm health report file not exsit,manual generate report failed!", __FUNCTION__);
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


LOCAL gint32 get_fdm_report_json(PROVIDER_PARAS_S *provider_param)
{
    gint32 handle;

    if (provider_param == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    // fdm/pfae未使能，返回空对象
    gint8 chk_ret = check_fdm_enabled();
    guint8 pfae_enable = get_pfae_enable();
    if (chk_ret != RET_OK || pfae_enable == 0) {
        provider_param->user_data = json_object_new_object();
        return (provider_param->user_data == NULL) ? RET_ERR : RET_OK;
    }

    gint32 ret = check_fdm_report_file_exist(provider_param);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: check health report file exist fail(%d).", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    for (gint32 i = 0; i < 10; i++) {
        break_if_expr(vos_get_file_accessible(HEALTH_REPORT_JSON_PATH));
        vos_task_delay(1000);
    }

    
    
    handle = dal_file_lock(HEALTH_REPORT_JSON_PATH, FALSE);
    if (handle == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: lock health report file fail.", __FUNCTION__);
        return RET_ERR;
    }

    
    glong file_len = vos_get_file_length(HEALTH_REPORT_JSON_PATH);
    if (file_len <= 0) {
        provider_param->user_data = json_object_new_object();
    } else {
        provider_param->user_data = json_object_from_file(HEALTH_REPORT_JSON_PATH);
    }
    dal_file_unlock(handle);
    

    return (provider_param->user_data == NULL) ? RET_ERR : RET_OK;
}


LOCAL gint32 free_fdm_report_json(json_object *report_json)
{
    if (NULL != report_json) {
        (void)json_object_put(report_json);
    }

    return VOS_OK;
}


LOCAL gint32 fdm_get_prop_values(PROVIDER_PARAS_S *provider_param, json_object **o_result_jso,
    json_object *o_message_array_jso, const PROPERTY_PROVIDER_S *provider, const guint32 count)
{
    json_object *prop_value_jso = NULL;
    json_object *message_info = NULL;

    // 拷贝生成属性值获取接口层入参信息
    for (guint32 array_index = 0; array_index < count; array_index++) {
        if (NULL != provider[array_index].pfn_get) {
            // 得到是否满足权限标识，传递给下层接口使用
            provider_param->is_satisfy_privi =
                provider_param->user_role_privilege & provider[array_index].require_privilege;
            provider_param->index = array_index;
            message_info = NULL;
            prop_value_jso = NULL;

            
            (void)json_object_object_get_ex(*o_result_jso, provider[array_index].property_name, &prop_value_jso);
            debug_log(DLOG_MASS, "%s: pfn_get call %d", __FUNCTION__, array_index);
            gint32 ret = provider[array_index].pfn_get(provider_param, &message_info, &prop_value_jso);
            // 正常返回，且存在属性值对象，则更新
            // 正常返回，不存在属性值对象，则采用配置属性值
            if (HTTP_OK == ret) {
                if (NULL != prop_value_jso) {
                    json_object_object_add(*o_result_jso, provider[array_index].property_name, prop_value_jso);
                    debug_log(DLOG_MASS, "%s: prop key:%s / value:%s", __FUNCTION__,
                        provider[array_index].property_name, dal_json_object_get_str(prop_value_jso));
                }
            }
            // 内部错误，获取属性值失败，则更新配置属性值为null
            else if (HTTP_INTERNAL_SERVER_ERROR == ret) {
                json_bool b_ret = json_object_has_key(*o_result_jso, provider[array_index].property_name);
                do_val_if_expr(TRUE == b_ret,
                    json_object_object_add(*o_result_jso, provider[array_index].property_name, NULL));
            }
            // 如果为其他失败场景，直接返回错误消息
            else {
                // 防止下层接口异常场景仍然返回值信息
                if (NULL != prop_value_jso) {
                    (void)json_object_put(prop_value_jso);
                }

                if (NULL != message_info) {
                    (void)json_object_array_add(o_message_array_jso, message_info);
                } else {
                    debug_log(DLOG_INFO, "%s: message_info is null", __FUNCTION__);
                }

                debug_log(DLOG_MASS, "%s: pfn_get returned %d", __FUNCTION__, ret);
                return ret;
            }

            // 防止下层接口多生成错误消息的场景，此处释放以免内存泄露(理论不应该出现)
            if (NULL != message_info) {
                (void)json_object_put(message_info);
                message_info = NULL;
            }
        }
    }

    return VOS_OK;
}


gint32 manager_fdmservice_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {1, {RFPROP_FDM_REPORT_TIME, NULL, NULL, NULL, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


gint32 check_fdm_enabled(void)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 fdm_enable = 0;

    
    gint32 ret = dal_get_object_handle_nth(CLASS_FDM_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_INFO, "%s: get FdmConfig handle failed.", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDM_ENABLE, &fdm_enable);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_INFO, "%s: get fdm enable prop failed.", __FUNCTION__));

    return_val_do_info_if_expr(0 == fdm_enable, VOS_ERR, debug_log(DLOG_INFO, "%s: get fdm disabled.", __FUNCTION__));
    return VOS_OK;
}


gint32 get_fdmservice_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    PROPERTY_PROVIDER_S *provider = NULL;
    guint32 count = 0;

    
    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (manager_fdmservice_provider_entry(i_paras, &provider, &count) != VOS_OK) {
        debug_log(DLOG_INFO, "%s: get prop failed.", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    
    gint32 ret = get_fdm_report_json(i_paras);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s, %d: open fdm report json failed(%d).", __FUNCTION__, __LINE__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = fdm_get_prop_values(i_paras, &o_rsc_jso, o_err_array_jso, provider, count);
    debug_log(DLOG_MASS, "%s: body_jso=%s.", __FUNCTION__, dal_json_object_get_str(o_rsc_jso));
    free_fdm_report_json(i_paras->user_data);
    debug_log(DLOG_MASS, "%s, %d: fdmservice get return value %d.", __FUNCTION__, __LINE__, ret);
    

    return ret;
}


gint32 manager_fdmservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret = fdm_check_fdmservice_uri_valid(i_paras->uri, URI_FORMAT_FDMSERVICE);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s, %d: uri check failed.", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_fdmservice_provider;
    *count = sizeof(g_manager_fdmservice_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 fdm_event_reports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret = fdm_check_fdmservice_uri_valid(i_paras->uri, URI_FORMAT_FDMSERVICE);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s, %d: uri check failed.", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    
    return_val_do_info_if_fail(VOS_OK == check_fdm_enabled(), RF_RSC_NOT_FOUND,
        debug_log(DLOG_INFO, "%s, %d: fdm not supported.", __FUNCTION__, __LINE__));
    debug_log(DLOG_INFO, "%s, %d: fdm_event_reports_provider_entry in.", __FUNCTION__, __LINE__);
    *prop_provider = g_fdmservice_event_reports_provider;
    *count = sizeof(g_fdmservice_event_reports_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 fdm_regenerate_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = fdm_check_fdmservice_uri_valid(i_paras->uri, URI_FORMAT_FDMSERVICE);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s, %d: uri check failed.", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    
    return_val_do_info_if_fail(VOS_OK == check_fdm_enabled(), HTTP_NOT_FOUND,
        debug_log(DLOG_INFO, "%s, %d: fdm not supported.", __FUNCTION__, __LINE__));
    debug_log(DLOG_INFO, "%s, %d: fdm_regenerate_action_info_provider_entry in.", __FUNCTION__, __LINE__);
    *prop_provider = g_fdmservice_regenerate_actioninfo_provider;
    *count = sizeof(g_fdmservice_regenerate_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 fdm_remove_alarm_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = fdm_check_fdmservice_uri_valid(i_paras->uri, URI_FORMAT_FDMSERVICE);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s, %d: uri check failed.", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    
    return_val_do_info_if_fail(VOS_OK == check_fdm_enabled(), HTTP_NOT_FOUND,
        debug_log(DLOG_INFO, "%s, %d: fdm not supported.", __FUNCTION__, __LINE__));
    debug_log(DLOG_INFO, "%s, %d: fdm_remove_alarm_action_info_provider_entry in.", __FUNCTION__, __LINE__);
    *prop_provider = g_fdmservice_removealarm_actioninfo_provider;
    *count = sizeof(g_fdmservice_removealarm_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_manager_fdmservice_mem_dynamic_remapping_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 dps_enable = DISABLE;
    OBJ_HANDLE obj_handle = 0;
 
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    guint32 ret = dal_get_object_handle_nth(CLASS_FDM_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get FdmConfig handle failed, ret: %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_FDM_DAYNAMIC_REMAPPING_ENABLE, &dps_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s failed, ret: %d.", __FUNCTION__, PROPERTY_FDM_DAYNAMIC_REMAPPING_ENABLE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_boolean(dps_enable == 0 ? FALSE : TRUE);
    return HTTP_OK;
}
 

LOCAL gint32 set_manager_fdmservice_mem_dynamic_remapping_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;
 
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        debug_log(DLOG_ERROR, "%s: Access forbidden.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE,
            o_message_jso, RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE);
        return HTTP_FORBIDDEN;
    }
    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Bad request.", __FUNCTION__);
        (void)create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE, o_message_jso,
            RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE);
        return HTTP_BAD_REQUEST;
    }
 
    json_bool dps_enable = json_object_get_boolean(i_paras->val_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte(dps_enable ? 1 : 0));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_FDM_NAME, METHOD_FDM_SET_DYNAMIC_REMAPPING_ENABLED, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return fdm_set_ret_func(ret, o_message_jso, RFPROP_FDM_DYNAMIC_REMAPPING_ENABLE);
}