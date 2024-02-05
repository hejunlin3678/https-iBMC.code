


#include "redfish_provider.h"
#include "redfish_provider_system.h"

#define RFPATCH_PARAM_BIOS_SETTING "#Settings.v1_0_2.Settings"
#define RESP_HEADER_ETAG "ETag"
#define ETAG_TMP_LEN 9
#define CHECK_SUM_FIRST_EIGHT 8

LOCAL gint32 get_bios_policycfg_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_policycfg_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_policycfg_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_bios_policycfg_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_policycfg_reg_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_policycfg_reg_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_policycfg_reg_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_policycfg_reg_settings(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_sytem_bios_policyconfig_provider[] = {
    {
        RFPROP_ODATA_CONTEXT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_bios_policycfg_odatacontext, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_bios_policycfg_odataid, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ATTRIBUTES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_bios_policycfg_attributes, set_bios_policycfg_attributes, NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_sytem_bios_policyconfig_reg_provider[] = {
    {
        RFPROP_ODATA_CONTEXT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_bios_policycfg_reg_odatacontext, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_bios_policycfg_reg_odataid, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ATTRIBUTES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_bios_policycfg_reg_attributes, NULL, NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_SETTINGS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_bios_policycfg_reg_settings, NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};


LOCAL gint32 get_bios_policycfg_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[2 * MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 获取 @odata.context
    gint32 ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_BIOS_POLICYCONFIG_ODATA_CONTEXT, slot_str);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail,ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(string_value);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: o_result_jso is NULL", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_bios_policycfg_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 获取 @odata.id
    gint32 ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_BIOS_POLICYCONFIG_ODATA_ID, slot_str);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail,ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(string_value);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: o_result_jso is NULL", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_bios_policycfg_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *str = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail, ret = %d",
            __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    GSList *input_list = g_slist_append(NULL, (gpointer)g_variant_new_string(BIOS_JSON_FILE_CONFIGVALUE_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail, ret = %d", __FUNCTION__, ret);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (str == NULL || strlen(str) == 0) {
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_tokener_parse(str);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL gint32 get_bios_policycfg_reg_odatacontext(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[2 * MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_BIOS_POLICYCFG_REG_ODATA_CONTEXT, slot_str);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail,ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(string_value);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: o_result_jso is NULL", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_bios_policycfg_reg_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_BIOS_POLICYCFG_REG_ODATA_ID, slot_str);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail,ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(string_value);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: o_result_jso is NULL", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL void add_settings_etag(json_object *jso_settings)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    gchar etag_tmp[ETAG_TMP_LEN] = {0};
    gchar *check_sum = NULL;

    
    gint32 ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail, ret = %d", __FUNCTION__, ret);
        goto err_out;
    }

    GSList *input_list = g_slist_append(NULL, (gpointer)g_variant_new_string(BIOS_JSON_FILE_POLICYONFIGREGISTRY_NAME));
    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);
    
    uip_free_gvariant_list(input_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail, ret = %d", __FUNCTION__, ret);
        goto err_out;
    }

    const gchar *str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if ((str == NULL) || (strlen(str) == 0)) {
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__);
        goto err_out;
    }

    
    check_sum = g_compute_checksum_for_string(G_CHECKSUM_SHA256, str, strlen(str));
    if (check_sum == NULL) {
        debug_log(DLOG_ERROR, "%s: g_compute_checksum_for_string fail", __FUNCTION__);
        goto err_out;
    }

    
    ret = strncpy_s(etag_tmp, ETAG_TMP_LEN, check_sum, CHECK_SUM_FIRST_EIGHT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, ret);
    }

    json_object_object_add(jso_settings, RESP_HEADER_ETAG, json_object_new_string((const gchar*)etag_tmp));

    g_free(check_sum);
    check_sum = NULL;
    uip_free_gvariant_list(output_list);
    output_list = NULL;

    return;

err_out:
    if (output_list != NULL) {
        uip_free_gvariant_list(output_list);
    }
    if (check_sum != NULL) {
        g_free(check_sum);
    }
    json_object_object_add(jso_settings, RESP_HEADER_ETAG, NULL);
}


LOCAL void add_settings_settingsobject(json_object *jso_settings)
{
    gchar string_value[MAX_RSC_ID_LEN] = {0};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    json_object *jso = json_object_new_object();
    json_object_object_add(jso_settings, RFPROP_SETTINGSOBJECT, jso);
    if (jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return;
    }

    gint32 ret = redfish_get_board_slot(slot_str, sizeof(slot_str) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed, ret = %d", __FUNCTION__, ret);
        json_object_object_add(jso, RFPROP_ODATA_ID, NULL);
        return;
    }

    ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1,
        RF_BIOS_POLICYCONFIG_ODATA_ID, slot_str);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        json_object_object_add(jso, RFPROP_ODATA_ID, NULL);
        return;
    }

    json_object_object_add(jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)string_value));
}


LOCAL gint32 get_bios_policycfg_reg_settings(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!(i_paras->is_satisfy_privi)) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    json_object_object_add(*o_result_jso, RFPROP_ODATA_TYPE,
        json_object_new_string(RFPATCH_PARAM_BIOS_SETTING));

    
    add_settings_etag(*o_result_jso);

    
    add_settings_settingsobject(*o_result_jso);

    return HTTP_OK;
}


LOCAL gint32 get_bios_policycfg_reg_attributes(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (!(i_paras->is_satisfy_privi)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail, ret = %d",
            __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    GSList *input_list = g_slist_append(NULL, (gpointer)g_variant_new_string(BIOS_JSON_FILE_POLICYONFIGREGISTRY_NAME));

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_JSON_FILE, NULL, input_list, &output_list);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: METHOD_BIOS_GET_JSON_FILE fail, ret = %d", __FUNCTION__, ret);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    const gchar *str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (str == NULL || strlen(str) == 0) {
        debug_log(DLOG_ERROR, "%s: g_variant_get_string fail", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_tokener_parse(str);
    uip_free_gvariant_list(output_list);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 check_error_type(gint32 ret, json_object **o_message_jso, PROVIDER_PARAS_S *i_paras)
{
    switch (ret) {
        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_bios_policycfg_attributes(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;
    void *user_data = NULL;

    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: The input parameter is NULL.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        debug_log(DLOG_ERROR, "%s: Check privilege fail.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ATTRIBUTES, o_message_jso,
            RFPROP_ATTRIBUTES);
        return HTTP_FORBIDDEN;
    }

    
    if (json_object_get_user_data(i_paras->val_jso, (void **)&user_data) && user_data != NULL) {
        return HTTP_BAD_REQUEST;
    }

    gint32 ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle for OBJ_NAME_BIOS fail, ret = %d",
            __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    const gchar *json_str = dal_json_object_to_json_string(i_paras->val_jso);
    if (json_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_to_json_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    GSList *input_list = g_slist_append(NULL, (gpointer)g_variant_new_string(BIOS_JSON_FILE_CONFIGVALUE_NAME));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(json_str));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_SET_JSON_FILE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    if (ret != RET_OK) {
        ret = check_error_type(ret, o_message_jso, i_paras);
    } else {
        ret = HTTP_OK;
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return ret;
}


gint32 system_bios_policyconfig_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    guint32 product_ver = 0;
    guchar arm_enable = 0;
    gboolean ret_bool = redfish_check_system_uri_valid(i_paras->uri);
    guchar board_type = 0;
    if (!ret_bool) {
        return HTTP_NOT_FOUND;
    }

    gint32 ret = dal_get_product_version_num(&product_ver);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail,ret = %d",
            __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_get_x86_enable_type(&board_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get board_type fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (board_type == DISABLE) {
        return HTTP_NOT_FOUND;
    }

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (arm_enable == DISABLE && product_ver < PRODUCT_VERSION_V5) {
        debug_log(DLOG_ERROR, "%s: check ability fail", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_sytem_bios_policyconfig_provider;
    *count = sizeof(g_sytem_bios_policyconfig_provider) / sizeof(PROPERTY_PROVIDER_S);
    return RET_OK;
}


gint32 system_bios_policyconfig_reg_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    guint32 product_ver = 0;
    guchar arm_enable = 0;
    gboolean ret_bool = redfish_check_system_uri_valid(i_paras->uri);
    if (!ret_bool) {
        return HTTP_NOT_FOUND;
    }

    guchar board_type = 0;
    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get board_type fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (board_type == DISABLE) {
        return HTTP_NOT_FOUND;
    }

    ret = dal_get_product_version_num(&product_ver);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num fail,ret = %d",
            __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (arm_enable == DISABLE && product_ver < PRODUCT_VERSION_V5) {
        debug_log(DLOG_ERROR, "%s: check ability fail", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_sytem_bios_policyconfig_reg_provider;
    *count = sizeof(g_sytem_bios_policyconfig_reg_provider) / sizeof(PROPERTY_PROVIDER_S);
    return RET_OK;
}