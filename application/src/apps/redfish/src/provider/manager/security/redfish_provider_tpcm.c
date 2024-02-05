
#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_util.h"
#include "redfish_provider_tpcm.h"


LOCAL gint32 get_tpcmservice_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_tsb_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_tsb_ip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_actual_digest(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_standard_digest(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 set_standard_digest(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_measure_enabled(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 set_measure_enabled(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 get_control_policy(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
LOCAL gint32 set_control_policy(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_tpcm_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_tpcmservice_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        TRUSTED_CENTERSERVER_ADDR,
        TPCM_CLASS_CONFIG, TRUSTED_CENTERSERVER_ADDR,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_tsb_ip,
        set_tsb_ip,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        MEASURE_ENABLED,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_measure_enabled,
        set_measure_enabled,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        CONTROL_POLICY,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_control_policy,
        set_control_policy,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        ACTUAL_DIGEST,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_actual_digest,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        STANDARD_DIGEST,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_standard_digest,
        set_standard_digest,
        NULL, ETAG_FLAG_ENABLE
    }
};

LOCAL const char *g_measureobj_list[] = {"Bios", "Shim", "Grub", "GrubCfg", "Kernel", "Initrd"};
LOCAL const char *g_digestobj_list[] = {"TPCMBIOS", "TPCMSHIM", "TPCMGRUB", "TPCMOSCfg", "TPCMOSKernel",
    "TPCMOSInitrd"};


LOCAL void record_tpcm_operation_log(PROVIDER_PARAS_S* i_paras, const gchar *fmt, ...)
{
    GSList *caller_info = NULL;
    gchar buf[LOG_MAX_SZ] = {};
    gint32 str_len;
    va_list ap;
    va_start(ap, fmt);
    str_len = vsnprintf_s(buf, sizeof(buf), sizeof(buf), fmt, ap);
    if (str_len < RET_OK) {
        debug_log(DLOG_ERROR, "vsnprintf_s failed.");
    }
    va_end(ap);

    if (0 == strlen(buf)) {
        return;
    }
    caller_info = g_slist_append(caller_info,
        g_variant_new_string(dal_rf_get_op_log_module_name((guchar)i_paras->is_from_webui)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s", buf);
    uip_free_gvariant_list(caller_info);
}


LOCAL gint32 get_tpcmservice_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, MANAGER_TPCMSERVICE_SVC_URI);
}


LOCAL gint32 get_tsb_ip(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gchar addr[PROPERTY_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dfl_get_object_handle(TPCM_CLASS_CONFIG, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle failed (ret:%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_string(obj_handle, TRUSTED_CENTERSERVER_ADDR, addr, sizeof(addr));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get addr failed (ret:%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (strlen(addr) == 0) {
        debug_log(DLOG_DEBUG, "the function pointer is null. (property: %s)", TRUSTED_CENTERSERVER_ADDR);
        return HTTP_OK;
    }
    *o_result_jso = json_object_new_string((const gchar *)addr);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "create the json object failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_measure_enabled(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    guint8 measure = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dfl_get_object_handle(TPCM_CLASS_CONFIG, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle failed (ret:%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MEASURE_ENABLE, &measure);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get addr failed (ret:%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_boolean(measure == 1 ? TRUE : FALSE);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "create the json object failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_control_policy(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    guint32 control = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    const char *policy[] = {"no action", "intercept boot"};
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dfl_get_object_handle(TPCM_CLASS_CONFIG, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle failed (ret:%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_CONTROL_ACTION, &control);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get addr failed (ret:%d)", ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(policy[control]);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "create the json object failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 set_tsb_ip(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    _cleanup_gvariant_slist_full_ GSList* input_list = NULL;

    
    if (provider_paras_check(i_paras) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    const char *addr = json_object_get_string(i_paras->val_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(addr));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        TPCM_CLASS_CONFIG, METHOD_TPCM_SET_TSB_IP, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, TRUSTED_CENTERSERVER_ADDR, o_message_jso);
            debug_log(DLOG_ERROR, "privilege check failed");
            break;
        case RET_OK:
            ret = HTTP_OK;
            break;
        case ADDR_INVALID:
            (void)create_message_info(MSGID_SERVER_ADDRESS_ERR, TRUSTED_CENTERSERVER_ADDR, o_message_jso,
                addr, TRUSTED_CENTERSERVER_ADDR);
            ret = HTTP_BAD_REQUEST;
            break;
        case TSB_DISABLE:
            (void)create_message_info(TSB_DISABLE_MSG, TRUSTED_CENTERSERVER_ADDR, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, TRUSTED_CENTERSERVER_ADDR, o_message_jso);
            debug_log(DLOG_ERROR, "Set tsb ip fail, ret:%d", ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    return ret;
}


LOCAL gint32 set_measure_enabled(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    _cleanup_gvariant_slist_full_ GSList* input_list = NULL;

    
    if (provider_paras_check(i_paras) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    json_bool enable = json_object_get_boolean(i_paras->val_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte(enable ? 1 : 0));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        TPCM_CLASS_CONFIG, METHOD_SET_TPCM_TSB_SWITCH, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, MEASURE_ENABLED, o_message_jso);
            debug_log(DLOG_ERROR, "privilege check failed");
            break;
        case RET_OK:
            ret = HTTP_OK;
            break;
        case TSB_ERROR:
            (void)create_message_info(TSB_SERVER_ERROR, MEASURE_ENABLED, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        case TSB_DISABLE:
            (void)create_message_info(TSB_DISABLE_MSG, MEASURE_ENABLED, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, MEASURE_ENABLED, o_message_jso);
            debug_log(DLOG_ERROR, "Set enable fail, ret:%d", ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    record_tpcm_operation_log(i_paras, "Set tpcm measure enable to (%s) %s", enable ? "open" : "close",
        ret == HTTP_OK ? "successfully" : "failed");
    return ret;
}


LOCAL gint32 set_control_policy(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    _cleanup_gvariant_slist_full_ GSList* input_list = NULL;
    const char *policy[] = {"no action", "intercept boot"};
    
    if (provider_paras_check(i_paras) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    const gchar *control = json_object_get_string(i_paras->val_jso);
    gint32 index = 0;
    gint32 map_len = sizeof(policy) / sizeof(policy[0]);
    for (index = 0; index < map_len; index++) {
        if (strcmp(policy[index], control) == 0) {
            break;
        }
    }
    if (index == map_len) {
        (void)create_message_info(MSGID_UNRECOGNIZED_REQ_BODY, CONTROL_POLICY, o_message_jso);
        debug_log(DLOG_ERROR, "invalid policy");
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(index));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        TPCM_CLASS_CONFIG, METHOD_SET_TPCM_TSB_ACTION, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, CONTROL_POLICY, o_message_jso);
            debug_log(DLOG_ERROR, "privilege check failed");
            break;
        case RET_OK:
            ret = HTTP_OK;
            break;
        case TSB_ERROR:
            (void)create_message_info(TSB_SERVER_ERROR, CONTROL_POLICY, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        case TSB_DISABLE:
            (void)create_message_info(TSB_DISABLE_MSG, CONTROL_POLICY, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, CONTROL_POLICY, o_message_jso);
            debug_log(DLOG_ERROR, "Set tpcm control action fail, ret:%d", ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    record_tpcm_operation_log(i_paras, "Set tpcm control action to (%s) %s", index == 1 ? "open" : "close",
        ret == HTTP_OK ? "successfully" : "failed");
    return ret;
}


LOCAL gint32 get_actual_digest(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    json_object *jso = NULL;
    gchar actualdigest[DIGEST_SIZE] = {0};
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "No object found");
        return HTTP_NOT_FOUND;
    }

    for (gint32 i = 0; i < sizeof(g_digestobj_list)/sizeof(g_digestobj_list[0]); i++) {
        ret = dfl_get_object_handle(g_digestobj_list[i], &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get obj handle fiald, obj = %s", g_digestobj_list[i]);
            return HTTP_NOT_FOUND;
        }
        (void)dal_get_property_value_string(obj_handle, PROPERTY_TPCM_ACTUAL_DIGEST, actualdigest, DIGEST_SIZE);
        jso = json_object_new_string(actualdigest);
        json_object_object_add(*o_result_jso, g_measureobj_list[i], jso);
    }

    return HTTP_OK;
}


LOCAL gint32 get_standard_digest(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    json_object *jso = NULL;
    gchar standarddigest[DIGEST_SIZE] = {0};
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "No object found");
        return HTTP_NOT_FOUND;
    }

    for (gint32 i = 0; i < sizeof(g_digestobj_list)/sizeof(g_digestobj_list[0]); i++) {
        ret = dfl_get_object_handle(g_digestobj_list[i], &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get obj handle fiald, obj = %s", g_digestobj_list[i]);
            return HTTP_NOT_FOUND;
        }
        (void)dal_get_property_value_string(obj_handle, PROPERTY_TPCM_STANDARD_DIGEST, standarddigest, DIGEST_SIZE);
        jso = json_object_new_string(standarddigest);
        json_object_object_add(*o_result_jso, g_measureobj_list[i], jso);
    }

    return HTTP_OK;
}

LOCAL gint32 ret_set_digest(gint32 ret, const gchar *obj, json_object *digest_obj, json_object** o_message_jso)
{
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, obj, &digest_obj);
            json_object_array_add(*o_message_jso, digest_obj);
            debug_log(DLOG_ERROR, "privilege check failed");
            break;
        case RET_OK:
            ret = HTTP_OK;
            break;
        case TSB_ERROR:
            (void)create_message_info(TSB_SERVER_ERROR, obj, &digest_obj);
            json_object_array_add(*o_message_jso, digest_obj);
            ret = HTTP_BAD_REQUEST;
            break;
        case TSB_DISABLE:
            (void)create_message_info(TSB_DISABLE_MSG, obj, &digest_obj);
            json_object_array_add(*o_message_jso, digest_obj);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, obj, &digest_obj);
            json_object_array_add(*o_message_jso, digest_obj);
            debug_log(DLOG_ERROR, "Set standard digest fail, ret:%d", ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    return ret;
}


LOCAL gint32 set_standard_digest(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    GSList* input_list = NULL;
    json_object *digest_obj = NULL;
    json_bool jso_ret_digest;
    const gchar *digest = NULL;
    gchar actualdigest[DIGEST_SIZE] = {0};

    
    if (provider_paras_check(i_paras) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_message_jso = json_object_new_array();
    
    for (int i = 0; i < sizeof(g_digestobj_list) / sizeof(g_digestobj_list[0]); i++) {
        jso_ret_digest = json_object_object_get_ex(i_paras->val_jso, g_measureobj_list[i], &digest_obj);
        if (jso_ret_digest != TRUE) {
            continue;
        }
        ret = dfl_get_object_handle(g_digestobj_list[i], &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "get obj handle fiald, obj = %s", g_digestobj_list[i]);
            return HTTP_NOT_FOUND;
        }
        
        digest = dal_json_object_get_str(digest_obj);
        if (digest == NULL) {
            (void)dal_get_property_value_string(obj_handle, PROPERTY_TPCM_ACTUAL_DIGEST, actualdigest,
                DIGEST_SIZE);
            digest = actualdigest;
        }
        input_list = g_slist_append(input_list, g_variant_new_byte(i + 1));
        input_list = g_slist_append(input_list, g_variant_new_string(digest));

        
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            TPCM_CLASS_MEASURED, METHOD_SET_STANDARD_DIGEST, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
            NULL);
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        
        ret = ret_set_digest(ret, g_digestobj_list[i], digest_obj, o_message_jso);
        record_tpcm_operation_log(i_paras, "Set tpcm %s Standard Digest %s", g_measureobj_list[i],
            ret == HTTP_OK ? "successfully" : "failed");
    }

    return ret;
}


gint32 tpcm_config_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
#ifndef ARM64_HI1711_ENABLED
    return HTTP_NOT_FOUND;
#endif
    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return RET_ERR;
    }
    gboolean b_ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (b_ret != TRUE) {
        return HTTP_NOT_FOUND;
    }

    
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList* output_list = NULL;
    gint32 ret = dfl_get_object_handle(TPCM_CLASS_BASEINFO, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s handle failed ret = %d", TPCM_CLASS_BASEINFO, ret);
        return HTTP_NOT_FOUND;
    }
    ret = dfl_call_class_method(obj_handle, METHOD_TPCM_GETTPCMSPPORTSTATE, NULL, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call method failed ret = %d", ret);
        return HTTP_NOT_FOUND;
    }
    guint8 tcm_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    if (tcm_state != 1) {
        debug_log(DLOG_ERROR, "tpcm check faild, tcm_state = %u", tcm_state);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_tpcm_provider;
    *count = G_N_ELEMENTS(g_tpcm_provider);
    return RET_OK;
}