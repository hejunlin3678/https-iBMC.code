

#include "redfish_provider.h"
#include "../apps/sensor_alarm/inc/remote_syslog.h"
#include "../apps/bmc_global/inc/bmc.h"
#include "redfish_provider_system.h"

#define FRSO_RESULT_INIT                         0
#define FRSO_RESULT_WAIT                         0xfe
#define FRSO_RESULT_OK                           0x55
#define FRSO_RESULT_FAIL                         0xaa
LOCAL void __add_result_jso(gint32 result, const char* blade_name, json_object** o_result_jso)
{
    switch (result) {
        case FRSO_RESULT_WAIT:
            json_object_object_add(*o_result_jso, blade_name, json_object_new_string("running"));
            break;
        case FRSO_RESULT_OK:
            json_object_object_add(*o_result_jso, blade_name, json_object_new_string("successful"));
            break;
        case FRSO_RESULT_FAIL:
            json_object_object_add(*o_result_jso, blade_name, json_object_new_string("failed"));
            break;
        case FRSO_RESULT_INIT:
            json_object_object_add(*o_result_jso, blade_name, json_object_new_string("initials"));
            break;
        default:
            json_object_object_add(*o_result_jso, blade_name, json_object_new_string("N/A"));
            break;
    }
}

LOCAL gint32 __get_system_frso_state(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (is_force_restart_enabled() != TRUE) {
        debug_log(DLOG_ERROR, "%s: Force restart OS is disabled", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Json obj is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    guint8 max_slot_id = get_board_max_slot_id();
    GSList* output_list = NULL;
    gchar blade_name[8] = { 0 }; // i为guint8类型，不会超过三位，blade_name最长不会超过8个字符
    for (guint8 i = 1; i <= max_slot_id; i++) {
        GSList *input_list = g_slist_append(NULL, g_variant_new_byte(i));
        gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            i_paras->obj_handle, CLASS_NAME_STORAGE_ARB, METHOD_ARB_GET_FORCE_RESTART_OS_RESULT, AUTH_ENABLE,
            i_paras->user_role_privilege, input_list, &output_list);
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        if (ret != RET_OK) {
            continue;
        }
        gint32 result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        uip_free_gvariant_list(output_list);
        output_list = NULL;
        ret = sprintf_s(blade_name, sizeof(blade_name), "%s%d", "Slot", i - 1);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: call sprintf_s failed, i=%d", __FUNCTION__, i);
            continue;
        }
        __add_result_jso(result, blade_name, o_result_jso);
    }

    return HTTP_OK;
}


LOCAL gint32 __act_system_frso_service(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    if (i_paras == NULL || o_message_jso == NULL || i_paras->val_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (is_force_restart_enabled() != TRUE) {
        debug_log(DLOG_ERROR, "%s: Force restart OS is disabled", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    json_object *obj_json = NULL;
    gint32 ret = json_object_object_get_ex(i_paras->val_jso, "SlotId", &obj_json);
    if (ret == FALSE) {
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SYSTEM_RESTARTSERVICE, "SlotId");
        return HTTP_BAD_REQUEST;
    }

    guint8 blade_id = (guint8)json_object_get_int(obj_json) + 1;
    GSList* input_list = g_slist_append(NULL, g_variant_new_byte(blade_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_NAME_STORAGE_ARB, METHOD_ARB_SET_FORCE_RESTART_OS, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    debug_log(DLOG_ERROR, "Force restart os %s, BladeId:%d", (ret == RET_OK) ? "successfully" : "failed", blade_id);
    switch (ret) {
        case RET_OK:
            return HTTP_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SERVICE_ENABLED, o_message_jso,
                RFPROP_SERVICE_ENABLED);
            return HTTP_BAD_REQUEST;
        default:
            debug_log(DLOG_ERROR, "%s: Call method[%s] failed, ret=%d", __FUNCTION__, METHOD_ARB_SET_FORCE_RESTART_OS,
                ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 __get_force_restart_service_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_SYSTEM_FORCE_RESTART_SERVICE);
}

LOCAL gint32 get_system_slot_ation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot[PROP_VAL_LENGTH] = {0};
    gchar system_uri[MAX_URI_LENGTH] = {0};
    json_object *oem_jso = NULL;
    json_object *hw_jso = NULL;

    if (o_message_jso == NULL || o_result_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "Input parameter validation failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get slot fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(system_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1,
        URI_FORMAT_SYSTEM_FORCERESTARTSERVICE, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "new action object fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    oem_jso = json_object_new_object();
    if (oem_jso == NULL) {
        (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "alloc new json object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    hw_jso = json_object_new_object();
    if (hw_jso == NULL) {
        (void)json_object_put(*o_result_jso);
        (void)json_object_put(oem_jso);
        *o_result_jso = NULL;
        oem_jso = NULL;
        debug_log(DLOG_ERROR, "alloc new json object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 生成ForceRestartService.RestartOs的action的对象
    rf_add_action_prop(hw_jso, (const gchar *)system_uri, RFPROP_SYSTEM_RESTARTSERVICE);
    
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, hw_jso);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_jso);
    
    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_system_frso_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        __get_force_restart_service_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        __get_system_frso_state, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_system_slot_ation, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_RESTARTSERVICE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT,
        SYS_LOCKDOWN_FORBID, NULL, NULL, __act_system_frso_service, ETAG_FLAG_ENABLE}
};

gint32 system_restart_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret = redfish_check_system_uri_valid(i_paras->uri);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_STORAGE_ARB, 0, &i_paras->obj_handle);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_frso_provider;
    *count = G_N_ELEMENTS(g_system_frso_provider);

    return RET_OK;
}

LOCAL gint32 __get_restart_os_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso,
        RFPROP_SYSTEM_RESTART_OS_ACTION_INFOURI);
}

LOCAL PROPERTY_PROVIDER_S g_system_actioninfo_restartservice_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        __get_restart_os_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 system_restartos_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = redfish_check_system_uri_valid(i_paras->uri);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: Invalid uri is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_STORAGE_ARB, 0, &i_paras->obj_handle);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_actioninfo_restartservice_provider;
    *count = sizeof(g_system_actioninfo_restartservice_provider) / sizeof(PROPERTY_PROVIDER_S);

    return HTTP_OK;
}