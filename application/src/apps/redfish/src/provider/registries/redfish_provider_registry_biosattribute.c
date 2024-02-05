
#include "redfish_provider.h"


LOCAL gint32 get_bios_registry_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_bios_registry_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_registry_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_bios_registry_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL void get_bios_attribute_registry_id(gchar *buffer, guint32 buffer_len);

LOCAL PROPERTY_PROVIDER_S g_registry_bios_provider[] = {
};


LOCAL void get_bios_attribute_registry_id(gchar *buffer, guint32 buffer_len)
{
    gint32 ret;
    gchar registry_version[MAX_RSC_ID_LEN] = {0};

    ret = rf_bios_get_registryversion(registry_version, sizeof(registry_version), RF_REGISTRY_VERSION_DOT);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail");
        return;
    }

    add_val_str_to_rsp_body(RFPROP_COMMON_ID, (const gchar *)registry_version, FALSE, buffer, buffer_len);
}


LOCAL void get_regrist_bios_supportedsystems(gchar *buffer, guint32 buffer_len)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar prop_val[MAX_CHARACTER_NUM] = {0};
    gchar tmp_buf[MAX_CHARACTER_NUM * 4] = {0};
    gchar obj_prop_buf[MAX_CHARACTER_NUM * 4] = {0};

    
    
    ret = dal_get_object_handle_nth(CLASS_ELABEL, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle fail, ret:%d", __FUNCTION__, ret);
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_NAME, prop_val, sizeof(prop_val));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret);
    }
    add_val_str_to_rsp_body(RFPROP_REGRIST_BIOS_PRODUCTNAME, (const gchar *)prop_val, FALSE, tmp_buf, sizeof(tmp_buf));

    
    (void)memset_s(prop_val, sizeof(prop_val), 0, sizeof(prop_val));
    ret = dal_get_property_value_string(obj_handle, PROPERTY_ELABEL_PRODUCT_PN, prop_val, sizeof(prop_val));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret);
    }
    add_val_str_to_rsp_body(RFPROP_REGRIST_BIOS_SYSTEMID, (const gchar *)prop_val, FALSE, tmp_buf, sizeof(tmp_buf));

    
    (void)memset_s(prop_val, sizeof(prop_val), 0, sizeof(prop_val));
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_handle of %s fail, ret:%d", __FUNCTION__, BIOS_CLASS_NAME, ret);
    }

    ret = dal_get_property_value_string(obj_handle, BMC_BIOS_VER_NAME, prop_val, sizeof(prop_val));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property_data fail, ret:%d", __FUNCTION__, ret);
    }

    add_val_str_to_rsp_body(RFPROP_REGRIST_BIOS_FIRMWAREVERSION, (const gchar *)prop_val, TRUE, tmp_buf,
        sizeof(tmp_buf));

    ret = snprintf_s(obj_prop_buf, sizeof(obj_prop_buf), sizeof(obj_prop_buf) - 1, "\"%s\":{%s},",
        RFPROP_REGRIST_SUPPORTEDSYSTEMS, tmp_buf);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        return;
    }

    ret = strcat_s(buffer, buffer_len, obj_prop_buf);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcat_s failed, ret:%d", __FUNCTION__, ret);
    }
}

LOCAL void get_regrist_bios_registryentries(PROVIDER_PARAS_S *i_paras, gchar **register_file)
{
    gint32 ret;
    const gchar *register_file_data = NULL;
    GSList *output_list = NULL;
    GSList *input_list = NULL;
    const gchar *str_start = NULL;
    gchar *str_end = NULL;

    *register_file = NULL;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_REGISTRY_NAME));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, BIOS_CLASS_NAME,
        METHOD_BIOS_GET_JSON_FILE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret:%d", __FUNCTION__, METHOD_BIOS_GET_JSON_FILE, ret);
        *register_file = NULL;
        return;
    }
    

    register_file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (register_file_data == NULL || strlen(register_file_data) == 0) {
        goto ERR_EXIT;
    }

    str_start = strstr(register_file_data, "{");
    str_end = g_strrstr(register_file_data, "}");
    if (str_start == NULL || str_end == NULL || str_end <= str_start) {
        goto ERR_EXIT;
    }

    *register_file = (gchar *)g_malloc0(str_end - str_start);
    if (*register_file == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    ret = strncpy_s(*register_file, str_end - str_start, str_start + 1, str_end - str_start - 1);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s:strncpy_s failed, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    uip_free_gvariant_list(output_list);
    return;

ERR_EXIT:
    uip_free_gvariant_list(output_list);
    if (*register_file != NULL) {
        g_free(*register_file);
    }
    *register_file = NULL;
    return;
}

gint32 registrystore_bios_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    guint32 bmc_version = 0;
    gboolean ret;
    guchar board_type = 0;
    gint32 ret_value;
    gchar   string_value[MAX_RSC_ID_LEN] = {0};
    gchar   value[MAX_RSC_ID_LEN] = {0};

    if (NULL == prop_provider || NULL == count) {
        debug_log(DLOG_ERROR, "%s, %d:   NULL.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:redfish_get_x86_enable_type fail. ", __FUNCTION__, __LINE__));
    return_val_if_expr(DISABLE == board_type, HTTP_NOT_FOUND);

    (void)dal_get_product_version_num(&bmc_version);
    guint8 conf_supp = NOT_SUPPORT;
    (void)dal_get_func_ability(BIOS_CLASS_NAME, BIOS_CONFIG_EXPORT_SUPPORT, &conf_supp);
    if (PRODUCT_VERSION_V5 > bmc_version || conf_supp != SUPPORT) {
        return HTTP_NOT_FOUND;
    }

    
    // uri合法性校验
    ret_value = rf_bios_get_registryversion(string_value, sizeof(string_value), RF_REGISTRY_VERSION_V);
    return_val_do_info_if_expr(VOS_OK != ret_value, VOS_ERR, debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail"));

    ret_value = snprintf_s(value, sizeof(value), sizeof(value) - 1, "%s.json", string_value);
    return_val_if_expr(ret_value < 0, VOS_ERR);

    debug_log(DLOG_DEBUG, "member_id : %s, registryversion: %s", i_paras->member_id, value);
    if (0 != g_ascii_strcasecmp(value, i_paras->member_id)) {
        debug_log(DLOG_DEBUG, "uri is invalid %s", i_paras->uri);

        return HTTP_NOT_FOUND;
    }
    

    *prop_provider = g_registry_bios_provider;
    
    *count = 0;
    ;

    return HTTP_OK;
}


gint32 get_bios_registry_file_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    
    gchar buffer[MAX_OEM_PROP_VAL_LEN * 2] = {0};
    gchar *register_file = NULL;
    gint32 ret;
    guint32 rsp_len;
    const guint32 append_str_len = 3;

    
    if (provider_paras_check(i_paras) != VOS_OK || o_err_array_jso == NULL || o_rsp_body_str == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_READONLY) == 0) {
        return HTTP_FORBIDDEN;
    }

    get_bios_attribute_registry_id(buffer, sizeof(buffer));

    add_val_str_to_rsp_body(RFPROP_COMMON_NAME, RFPROP_VAL_REGRIST_BIOS_NAME, FALSE, buffer, sizeof(buffer));

    add_val_str_to_rsp_body(RFPROP_COMMON_DESCRIPTION, RFPROP_VAL_REGRIST_BIOS_DESC, FALSE, buffer, sizeof(buffer));

    add_val_str_to_rsp_body(RFPROP_REGRIST_BIOS_OWNING_ENTITY, RFPROP_COMMON_HUAWEI, FALSE, buffer, sizeof(buffer));

    
    get_regrist_bios_supportedsystems(buffer, sizeof(buffer));

    
    get_regrist_bios_registryentries(i_paras, &register_file);
    if (register_file == NULL) {
        debug_log(DLOG_DEBUG, "%s: bios attribute register file is NULL", __FUNCTION__);
        add_val_str_to_rsp_body(RFPROP_REGRIST_ENTRIES, "", TRUE, buffer, sizeof(buffer));
        ret = set_rsp_body_content((const gchar *)buffer, o_rsp_body_str);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s:set_rsp_body_content, ret:%d", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        return RF_OK;
    }

    rsp_len = strlen(buffer) + strlen(register_file) + append_str_len;
    *o_rsp_body_str = (gchar *)g_malloc0(rsp_len);
    if (*o_rsp_body_str == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        g_free(register_file);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(*o_rsp_body_str, rsp_len, rsp_len - 1, "{%s%s}", buffer, register_file);
    g_free(register_file);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret:%d", __FUNCTION__, ret);
        g_free(*o_rsp_body_str);
        *o_rsp_body_str = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return RF_OK;
}

LOCAL PROPERTY_PROVIDER_S g_registry_bios_location_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_bios_registry_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_bios_registry_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_REGISTRY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_bios_registry_version, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LOCATION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_bios_registry_location, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_bios_registry_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar registry_versioon[MAX_RSC_ID_LEN] = {0};
    gchar uri_value[MAX_PROP_VAL_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_bios_get_registryversion(registry_versioon, sizeof(registry_versioon), RF_REGISTRY_VERSION_V);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri_value, sizeof(uri_value), sizeof(uri_value) - 1, RFPROP_REGRIST_ODTAID_URI, registry_versioon);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const char *)uri_value);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_bios_registry_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar registry_versioon[MAX_RSC_ID_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_bios_get_registryversion(registry_versioon, sizeof(registry_versioon), RF_REGISTRY_VERSION_V);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *)registry_versioon);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_bios_registry_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar registry_versioon[MAX_RSC_ID_LEN] = {0};
    gchar *registry_position = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_bios_get_registryversion(registry_versioon, sizeof(registry_versioon), RF_REGISTRY_VERSION_DOT);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // BiosAttributeRegistry.1.0.0 -->  BiosAttributeRegistry.1.0
    registry_position = g_strrstr(registry_versioon, ".");
    if (NULL == registry_position) {
        debug_log(DLOG_DEBUG, "registry_position fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *registry_position = '\0';

    *o_result_jso = json_object_new_string((const char *)registry_versioon);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_bios_registry_location(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar registry_versioon[MAX_RSC_ID_LEN] = {0};
    gchar uri_value[MAX_PROP_VAL_LEN] = {0};
    json_object *json_obj = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = rf_bios_get_registryversion(registry_versioon, sizeof(registry_versioon), RF_REGISTRY_VERSION_V);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret =
        snprintf_s(uri_value, sizeof(uri_value), sizeof(uri_value) - 1, RFPROP_REGRIST_REGISTRY_URI, registry_versioon);
    return_val_if_expr(ret < 0, HTTP_INTERNAL_SERVER_ERROR);

    json_obj = json_object_new_object();
    if (NULL == json_obj) {
        debug_log(DLOG_ERROR, "json_object_new_object fail");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(json_obj, RFRPOP_LANGUAGE, json_object_new_string(EVT_REGISTRY_LANG_EN));
    json_object_object_add(json_obj, RFPROP_LOCATION_URI, json_object_new_string((const gchar *)uri_value));

    *o_result_jso = json_object_new_array();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "json_object_new_array fail");
        (void)json_object_put(json_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = json_object_array_add(*o_result_jso, json_obj);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "json_object_array_add fail");
        (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        (void)json_object_put(json_obj);

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 registries_bios_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    guint32 bmc_version = 0;
    gboolean ret;
    guchar board_type = 0;
    gint32 ret_value;
    gchar   string_value[MAX_RSC_ID_LEN] = {0};
    guchar arm_enable = 0;

    if (NULL == prop_provider || NULL == count) {
        debug_log(DLOG_ERROR, "%s, %d:   NULL.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:redfish_get_x86_enable_type fail. ", __FUNCTION__, __LINE__));
    return_val_if_expr(DISABLE == board_type, HTTP_NOT_FOUND);

    (void)dal_get_product_version_num(&bmc_version);

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    guint8 conf_supp = NOT_SUPPORT;
    (void)dal_get_func_ability(BIOS_CLASS_NAME, BIOS_CONFIG_EXPORT_SUPPORT, &conf_supp);
    // arm产品支持通过redfish配置bios setup菜单
    if ((arm_enable == DISABLE && bmc_version < PRODUCT_VERSION_V5) || (conf_supp != SUPPORT)) {
        return HTTP_NOT_FOUND;
    }

    
    // uri合法性校验
    ret_value = rf_bios_get_registryversion(string_value, sizeof(string_value), RF_REGISTRY_VERSION_V);
    return_val_do_info_if_expr(VOS_OK != ret_value, VOS_ERR, debug_log(DLOG_ERROR, "rf_bios_get_registryversion fail"));

    debug_log(DLOG_DEBUG, "member_id : %s, registryversion: %s", i_paras->member_id, string_value);
    if (0 != g_ascii_strcasecmp(string_value, i_paras->member_id)) {
        debug_log(DLOG_ERROR, "uri is invalid %s", i_paras->uri);

        return HTTP_NOT_FOUND;
    }
    

    *prop_provider = g_registry_bios_location_provider;
    *count = sizeof(g_registry_bios_location_provider) / sizeof(PROPERTY_PROVIDER_S);

    return HTTP_OK;
}
