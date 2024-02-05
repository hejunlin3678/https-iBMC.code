
#include "redfish_provider.h"


LOCAL gint32 get_snmp_syscontact(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_snmp_syscontact(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_syslocation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_snmp_syslocation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_v1v2c_login_rule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_snmp_longpassword(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_trap_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_snmp_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 set_snmp_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_snmp_longpassword(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_snmp_v1v2c_login_rule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_snmp_community(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_rocommunity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_rwcommunity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 set_trap_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL gint32 act_snmp_sendtest(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_snmp_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);


LOCAL gint32 get_rw_community_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_rw_community_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 act_snmp_config_snmp_privpasswd(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_snmp_authuser(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_snmp_oldengineid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_manager_snmp_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {SNMP_ACTIONSNAME_STR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {SNMP_LINKSNAME_STR, CLASS_SNMP, PROPERTY_SNMP_V1V2C_PERMIT, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_links, NULL, NULL, ETAG_FLAG_ENABLE},

    {SNMP_VERSION_V1_STR, CLASS_SNMP, SNMP_PROPERTY_V1STATE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_version, set_snmp_version, NULL, ETAG_FLAG_ENABLE},
    {SNMP_VERSION_V2C_STR, CLASS_SNMP, SNMP_PROPERTY_V2CSTATE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_version, set_snmp_version, NULL, ETAG_FLAG_ENABLE},
    {SNMP_VERSION_V3_STR, CLASS_SNMP, PROPERTY_SNMP_V3STATUS, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_version, set_snmp_version, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SNMPV1V2C_LOGIN_RULE, CLASS_SNMP, PROPERTY_SNMP_V1V2C_PERMIT, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_v1v2c_login_rule, set_snmp_v1v2c_login_rule, NULL, ETAG_FLAG_ENABLE},
    {SNMP_LONGPASSWD_STR, CLASS_SNMP, PROPERTY_SNMP_LONG_PASSWORD_ENABLE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_longpassword, set_snmp_longpassword, NULL, ETAG_FLAG_ENABLE},

    
    {SNMP_RW_COMMUNITY_ENABLED_STR, CLASS_SNMP, PROPERTY_SNMP_RW_COMMUNITY_STATE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_rw_community_enabled, set_rw_community_enabled, NULL, ETAG_FLAG_ENABLE},
    

    {SNMP_ROCOMMUNITY_STR, CLASS_SNMP, PROPERTY_SNMP_ROCOMMUNITY, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_rocommunity, set_snmp_community, NULL, ETAG_FLAG_ENABLE},
    {SNMP_RWCOMMUNITY_STR, CLASS_SNMP, PROPERTY_SNMP_RWCOMMUNITY, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_rwcommunity, set_snmp_community, NULL, ETAG_FLAG_ENABLE},
    {SNMP_V3AUTH_USER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_authuser, NULL, NULL, ETAG_FLAG_ENABLE},
    {SNMP_V3ENGINE_ID, CLASS_SNMP, PROPERTY_SNMP_OLDENGINEID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_oldengineid, NULL, NULL, ETAG_FLAG_ENABLE},
    {SNMP_SYSTEM_CONTACT_NAME, CLASS_SNMP, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_syscontact, set_snmp_syscontact, NULL, ETAG_FLAG_ENABLE},
    {SNMP_SYSTEM_LOCATION_NAME, CLASS_SNMP, PROPERTY_SNMP_SYS_LOCATION, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_snmp_syslocation, set_snmp_syslocation, NULL, ETAG_FLAG_ENABLE},
    {SNMP_TRAP_INFO, CLASS_TRAP_CONFIG, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_trap_info, set_trap_info, NULL, ETAG_FLAG_ENABLE},

    {SNMP_ACTIONS_STR, CLASS_TRAP_CONFIG, METHOD_TRAP_SEND_TEST, USERROLE_BASICSETTING, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_snmp_sendtest, ETAG_FLAG_ENABLE},
    {SNMP_ACTIONS_SNMPPASSWD_STR, CLASS_USER, METHOD_USER_SETSNMPPRIVACYPASSWORD, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_snmp_config_snmp_privpasswd, ETAG_FLAG_ENABLE}
};

LOCAL PROPERTY_PROVIDER_S g_manager_snmp_actioninfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_snmp_actioninfo, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_snmp_syscontact(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar sys_contact[SNMP_STANDARD_SYS_CONTACT_MAX_LEN + 1] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret =
        dal_get_property_value_string(i_paras->obj_handle, PROPERTY_SNMP_SYS_CONTACT, sys_contact, sizeof(sys_contact));
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get snmp SystemContact fail ret = %d\n", __FUNCTION__, __LINE__, ret));

    
    ret = check_string_val_effective((const gchar *)sys_contact);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid string", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)sys_contact);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  o_result_jso is null\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 set_snmp_syscontact(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const char *sys_contact = NULL;
    GSList *input_list = NULL;
    gchar max_len_str[SNMP_STRLEN_32] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_snmp_provider[i_paras->index].property_name, o_message_jso,
        g_manager_snmp_provider[i_paras->index].property_name));

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].property_name);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_manager_snmp_provider[i_paras->index].property_name,
            o_message_jso, RF_VALUE_NULL, g_manager_snmp_provider[i_paras->index].property_name);
        return HTTP_BAD_REQUEST;
    }

    sys_contact = dal_json_object_get_str(i_paras->val_jso);
    if (sys_contact == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: SystemContact have NULL string.", __FUNCTION__, __LINE__);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(sys_contact));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_SNMP_SET_SYS_CONTACT,
        AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;
        case RFERR_WRONG_PARAM:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, SNMP_SYSTEM_CONTACT_NAME, o_message_jso, sys_contact,
                SNMP_SYSTEM_CONTACT_NAME);
            return HTTP_BAD_REQUEST;
        case ERR_EXCEED_MAX_LEN:
            (void)snprintf_truncated_s(max_len_str, sizeof(max_len_str), "%d", SNMP_STANDARD_SYS_CONTACT_MAX_LEN);
            (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, SNMP_SYSTEM_CONTACT_NAME, o_message_jso, sys_contact,
                SNMP_SYSTEM_CONTACT_NAME, (const gchar *)max_len_str);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_snmp_syslocation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar sys_location[SNMP_STANDARD_SYS_LOCATION_MAX_LEN + 1] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = dal_get_property_value_string(i_paras->obj_handle, PROPERTY_SNMP_SYS_LOCATION, sys_location,
        sizeof(sys_location));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d:  get snmp SystemLocation fail ret = %d\n", __FUNCTION__, __LINE__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = check_string_val_effective((const gchar *)sys_location);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s:invalid string", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)sys_location);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: o_result_jso is null\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 set_snmp_syslocation(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const char *sys_location = NULL;
    GSList *input_list = NULL;
    gchar max_len_str[SNMP_STRLEN_32] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].property_name);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_manager_snmp_provider[i_paras->index].property_name,
            o_message_jso, RF_VALUE_NULL, g_manager_snmp_provider[i_paras->index].property_name);
        return HTTP_BAD_REQUEST;
    }

    
    sys_location = dal_json_object_get_str(i_paras->val_jso);
    if (sys_location == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: SystemLocation have NULL string.", __FUNCTION__, __LINE__);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(sys_location));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_SNMP_SET_SYS_LOCATION,
        AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    switch (ret) {
        case VOS_OK:
            return HTTP_OK;
        case RFERR_WRONG_PARAM:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, SNMP_SYSTEM_LOCATION_NAME, o_message_jso, sys_location,
                SNMP_SYSTEM_LOCATION_NAME);
            return HTTP_BAD_REQUEST;
        case ERR_EXCEED_MAX_LEN:
            (void)snprintf_truncated_s(max_len_str, sizeof(max_len_str), "%d", SNMP_STANDARD_SYS_LOCATION_MAX_LEN);
            (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, SNMP_SYSTEM_LOCATION_NAME, o_message_jso, sys_location,
                SNMP_SYSTEM_LOCATION_NAME, (const gchar *)max_len_str);
            return HTTP_BAD_REQUEST;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, SNMP_SYSTEM_LOCATION_NAME, o_message_jso,
                SNMP_SYSTEM_LOCATION_NAME);
            return HTTP_FORBIDDEN;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s,%d: call methed %s return %d", __FUNCTION__, __LINE__,
                METHOD_SNMP_SET_SYS_LOCATION, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_snmp_actioninfo(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};
    gchar *action_name = NULL;
    gchar *dot_flag = NULL;
    guint32 i;
    const gchar*         snmp_action_info_array[] = {
        SNMP_ACTION_INFOURI,
        SNMP_ACTION_INFO_CONFIG_PRIPASSWD_URI
    };

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    for (i = 0; i < G_N_ELEMENTS(snmp_action_info_array); i++) {
        dot_flag = g_strrstr(snmp_action_info_array[i], SLASH_FLAG_STR);
        return_val_do_info_if_fail(NULL != dot_flag, VOS_ERR,
            debug_log(DLOG_ERROR, "cann't find dot flag from %s", snmp_action_info_array[i]));

        action_name = dot_flag + 1;

        if (g_str_case_rstr(i_paras->uri, action_name)) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
            if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, snmp_action_info_array[i], slot) <= 0) {
#pragma GCC diagnostic pop
                debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail.", __FUNCTION__, __LINE__);
                return HTTP_INTERNAL_SERVER_ERROR;
            } else {
                break;
            }
        }
    }

    *o_result_jso = json_object_new_string((const char *)uri);

    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_snmp_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SNMP_URI_CONTEXT, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *)uri);
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_snmp_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SNMP_URI_ODATAID, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d, json_object_new_string fail.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_snmp_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};
    gchar config_uri[MAX_URI_LENGTH] = {0};
    json_object *tmp_json = NULL;
    json_object *jso = NULL;

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

    // target
    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SNMP_ACTION_TARGETURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        (void)json_object_put(jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    tmp_json = json_object_new_string((const gchar *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_ACTION_TARGET, tmp_json);
    tmp_json = NULL;

    // actioninfo
    (void)memset_s(uri, sizeof(uri), 0x00, sizeof(uri));

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SNMP_ACTION_INFOURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        (void)json_object_put(jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    tmp_json = json_object_new_string((const gchar *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_ACTION_INFONAME, tmp_json);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, SNMP_ACTION_NAME, jso);

    // 添加修改snmp 鉴权密码的action
    jso = NULL;

    if (snprintf_s(config_uri, sizeof(config_uri), sizeof(config_uri) - 1, SNMP_URI_ODATAID, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    rf_add_action_prop(*o_result_jso, (const gchar *)config_uri, SNMP_ACTION_TARGET_CONFIG_PRIPASSWD_URI);
    return HTTP_OK;
}


LOCAL gint32 get_snmp_links_add_loginrule(json_object *array_json, gchar *slot, guint8 memberid)
{
    gint32 ret;
    json_object *tmp_json = NULL;
    gchar uri[SNMP_STRLEN_256] = {0};

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SNMP_LINKS_URI, slot, memberid) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    tmp_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != tmp_json, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const char *)uri));
    ret = json_object_array_add(array_json, tmp_json);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, (void)json_object_put(tmp_json);
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_snmp_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 prop_value = 0;
    json_object *array_json = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    
    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_SNMP));

    ret = dal_get_property_value_byte(obj_handle, g_manager_snmp_provider[i_paras->index].pme_prop_name, &prop_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name, g_manager_snmp_provider[i_paras->index].pme_prop_name));

    array_json = json_object_new_array();
    return_val_do_info_if_fail(NULL != array_json, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n", __FUNCTION__, __LINE__));

    if (prop_value & 0x01) {
        ret = get_snmp_links_add_loginrule(array_json, slot, 0);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(array_json);
            debug_log(DLOG_ERROR, "%s, %d: get_snmp_links_add_loginrule fail\n", __FUNCTION__, __LINE__));
    }

    if (prop_value & 0x02) {
        ret = get_snmp_links_add_loginrule(array_json, slot, 1);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(array_json);
            debug_log(DLOG_ERROR, "%s, %d: get_snmp_links_add_loginrule fail\n", __FUNCTION__, __LINE__));
    }

    if (prop_value & 0x04) {
        ret = get_snmp_links_add_loginrule(array_json, slot, 2);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(array_json);
            debug_log(DLOG_ERROR, "%s, %d: get_snmp_links_add_loginrule fail\n", __FUNCTION__, __LINE__));
    }
    

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(array_json);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, SNMP_LINKS_RULES, array_json);

    return HTTP_OK;
}


LOCAL gint32 get_enable_state(char *class_name, char *prop_name, json_object **o_result_jso)
{
    gint32 ret;
    guint8 prop_value = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_SNMP));

    ret = dal_get_property_value_byte(obj_handle, prop_name, &prop_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, class_name, prop_name));
    if (0 != prop_value && 1 != prop_value) {
        
        debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, class_name, prop_name,
            prop_value);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_boolean(prop_value);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_snmp_v1v2c_login_rule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 snmp_login_rule = 0;
    const gchar* permit_role_map[MAX_RULE_COUNT] = {RFPROP_VAL_RULE1, RFPROP_VAL_RULE2, RFPROP_VAL_RULE3};
    guint32 i;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    (void)dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_SNMP_V1V2C_PERMIT,
        &snmp_login_rule); // 失败了返回空数组，内存不用释放
    return_val_do_info_if_fail(snmp_login_rule <= MAX_RULE_VALUE, HTTP_OK,
        debug_log(DLOG_DEBUG, "%s: invalid snmp v1v2c value is %d", __FUNCTION__, snmp_login_rule));

    for (i = 0; i < MAX_RULE_COUNT; i++) {
        if (snmp_login_rule & BIT(i)) {
            json_object_array_add(*o_result_jso, json_object_new_string(permit_role_map[i]));
        }
    }

    return HTTP_OK;
}


LOCAL gint32 get_snmp_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_enable_state(g_manager_snmp_provider[i_paras->index].pme_class_name,
        g_manager_snmp_provider[i_paras->index].pme_prop_name, o_result_jso);
    return_val_if_expr((ret != VOS_OK), ret);

    return HTTP_OK;
}


LOCAL gint32 get_snmp_longpassword(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_enable_state(g_manager_snmp_provider[i_paras->index].pme_class_name,
        g_manager_snmp_provider[i_paras->index].pme_prop_name, o_result_jso);
    return_val_if_expr((ret != VOS_OK), ret);

    return HTTP_OK;
}


LOCAL gint32 get_rw_community_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_enable_state(g_manager_snmp_provider[i_paras->index].pme_class_name,
        g_manager_snmp_provider[i_paras->index].pme_prop_name, o_result_jso);
    return_val_if_expr((ret != VOS_OK), ret);

    return HTTP_OK;
}


LOCAL gint32 get_snmp_authuser(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 id = 0;
    gchar name[128] = {0};
    json_object *user_json_array = NULL;
    json_object *user_json = NULL;
    guchar snmp_status = 0;
    guchar configure_self_priv = DISABLE;
    guint32 product_ver = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_user_role_priv_enable(i_paras->user_name, PROPERTY_USERROLE_CONFIGURE_SELF, &configure_self_priv);

    return_val_do_info_if_fail(DISABLE != configure_self_priv, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "Insufficient permissions\n"));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &snmp_status);

    ret = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_get_product_version_num fail.\n", __FUNCTION__, __LINE__));

    if ((DISABLE == snmp_status) || (PRODUCT_VERSION_V5 > product_ver)) {
        debug_log(DLOG_DEBUG, "%s, %d: snmp not supported\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    user_json_array = json_object_new_array();

    // 权限判断
    if (i_paras->user_role_privilege & USERROLE_USERMGNT) {
        // 获取所有用户，放入数组中
        for (id = 2; id < IPMC_MAX_USER_NUMBER; id++) {
            continue_do_info_if_fail(VOS_OK ==
                dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, id, &obj_handle),
                debug_log(DLOG_ERROR, "%s, %d: get specific object fail", __FUNCTION__, __LINE__));

            (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, name, sizeof(name));

            if (0 != strlen(name)) {
                user_json = NULL;
                user_json = json_object_new_string((const gchar *)name);
                continue_do_info_if_fail(NULL != user_json,
                    debug_log(DLOG_ERROR, "%s, %d: new json object fail.", __FUNCTION__, __LINE__));
                ret = json_object_array_add(user_json_array, user_json);
                continue_do_info_if_fail(VOS_OK == ret,
                    debug_log(DLOG_ERROR, "%s, %d: json array add fail,ret = %d", __FUNCTION__, __LINE__, ret);
                    (void)json_object_put(user_json));
            }
        }

        *o_result_jso = user_json_array;
    } else {
        // 获取当前用户
        user_json = json_object_new_string(i_paras->user_name);
        return_val_do_info_if_fail(NULL != user_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(user_json_array);
            debug_log(DLOG_ERROR, "%s, %d:  new json fail\n", __FUNCTION__, __LINE__));
        ret = json_object_array_add(user_json_array, user_json);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(user_json_array);
            debug_log(DLOG_ERROR, "%s, %d:  json array  fail\n", __FUNCTION__, __LINE__));

        *o_result_jso = user_json_array;
    }

    return HTTP_OK;
}


LOCAL gint32 get_snmp_oldengineid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar oldengineid[MAX_STRBUF_LEN] = {0};
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_SNMP));

    ret = dal_get_property_value_string(obj_handle, g_manager_snmp_provider[i_paras->index].pme_prop_name, oldengineid,
        sizeof(oldengineid));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name, g_manager_snmp_provider[i_paras->index].pme_prop_name));

    *o_result_jso = json_object_new_string((const gchar *)oldengineid);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  o_result_jso is null\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 get_trap_version(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint8 val = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_TRAP_VERSION, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_VERSION));
    if (0 == val) {
        *json_obj = json_object_new_string(SNMP_TRAPVER_V1);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (1 == val) {
        *json_obj = json_object_new_string(SNMP_TRAPVER_V2C);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (3 == val) {
        *json_obj = json_object_new_string(SNMP_TRAPVER_V3);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else {
        
        debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
            PROPERTY_TRAP_VERSION, val);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 get_trap_username(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint8 val = 0;
    OBJ_HANDLE user_handle = 0;
    gchar user_name[SNMP_STRLEN_32] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_TRAPV3_USERID, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAPV3_USERID));

    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, val, &user_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get user handle fail, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_USER,
        PROPERTY_USER_ID, val));

    ret = dal_get_property_value_string(user_handle, PROPERTY_USER_NAME, user_name, sizeof(user_name));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_USER,
        PROPERTY_USER_NAME));

    *json_obj = json_object_new_string((const gchar *)user_name);
    return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_trap_mode(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint8 val = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_TRAP_MODE, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_MODE));

    if (0 == val) {
        *json_obj = json_object_new_string(SNMP_TRAPMODE_EVENT);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (1 == val) {
        *json_obj = json_object_new_string(SNMP_TRAPMODE_OID);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (2 == val) {
        *json_obj = json_object_new_string(SNMP_TRAPMODE_ALARM);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else {
        
        debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
            PROPERTY_TRAP_MODE, val);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 get_trap_identity(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint8 val = 0;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_TRAP_IDENTITY, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_IDENTITY));

    if (0 == val) {
        *json_obj = json_object_new_string(RF_BRDSN_STR);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (1 == val) {
        *json_obj = json_object_new_string(RF_PRODUCT_ASSET_TAG_STR);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (2 == val) {
        *json_obj = json_object_new_string(RF_HOST_NAME_STR);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (TRAP_ID_CHASSIS_NAME == val) {
        *json_obj = json_object_new_string(RF_CHASSIS_NAME_STR);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (TRAP_ID_CHASSIS_LOCATION == val) {
        *json_obj = json_object_new_string(RF_CHASSIS_LOCATION_STR);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else if (TRAP_ID_CHASSIS_SERIAL_NUMBER == val) {
        *json_obj = json_object_new_string(RF_CHASSIS_SERIAL_NUMBER_STR);
        return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    } else {
        
        debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
            PROPERTY_TRAP_IDENTITY, val);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 get_trap_sendseverity(OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    guint8 val = 0;
    gchar severity_str[SNMP_STRLEN_32] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_TRAP_SEND_SEVERITY, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_SEND_SEVERITY));

    ret = redfish_alarm_severities_change(&val, severity_str, sizeof(severity_str));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: invalid severity", __FUNCTION__, __LINE__));

    ret = redfish_prase_severity_str_to_jso((const gchar *)severity_str, json_obj);
    return ret;
}


LOCAL gint32 get_item_index_number(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    guint8 index_number = 0;
    gchar number_str[SNMP_STRLEN_32] = {0};

    ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_INDEX_NUM, &index_number);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_ITEM_INDEX_NUM));
    (void)memset_s(number_str, sizeof(number_str), 0x00, sizeof(number_str));
    if (snprintf_s(number_str, sizeof(number_str), sizeof(number_str) - 1, "%u", index_number) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    tmp_jso = json_object_new_string((const gchar *)number_str);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_TRAPITEM_ID, tmp_jso);

    return ret;
}


LOCAL gint32 get_bob_enable(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    guint8 bobable_value = 0;

    ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_BOB_ENABLE, &bobable_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_BOB_ENABLE));

    if (BOB_ENABLE != bobable_value && BOB_DISABLE != bobable_value) {
        debug_log(DLOG_ERROR, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_ITEM_CONFIG,
            PROPERTY_TRAP_BOB_ENABLE, bobable_value);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    tmp_jso = json_object_new_boolean(bobable_value);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_boolean fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_BOBTRAPITEM_ENABLE, tmp_jso);
    return ret;
}


LOCAL gint32 get_item_enable(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    guint8 able_value = 0;

    ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_ENABLE, &able_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_ITEM_DEST_IPADDR));
    if (0 != able_value && 1 != able_value) {
        
        debug_log(DLOG_DEBUG, "%s, %d:  prop invalid, %s, %s, %d\n", __FUNCTION__, __LINE__, CLASS_TRAP_ITEM_CONFIG,
            PROPERTY_TRAP_ITEM_ENABLE, able_value);
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    tmp_jso = json_object_new_boolean(able_value);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_boolean fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_TRAPITEM_ENABLE, tmp_jso);
    return ret;
}


LOCAL gint32 get_item_dst_ipaddress(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    gchar ipaddress[HOST_ADDR_MAX_LEN + 1] = {0};

    // 接收者地址
    (void)memset_s(ipaddress, sizeof(ipaddress), 0x00, sizeof(ipaddress));

    ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_DEST_IPADDR, ipaddress,
        sizeof(ipaddress));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_ITEM_DEST_IPADDR));

    tmp_jso = json_object_new_string((const gchar *)ipaddress);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_TRAPITEM_IP, tmp_jso);
    return ret;
}


LOCAL gint32 get_item_dst_port(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    guint32 port = 0;

    // 接收者端口号
    ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_DEST_IPPORT, &port);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail, %s, %s\n", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG,
        PROPERTY_TRAP_ITEM_DEST_IPADDR));
    // 1－65535
    tmp_jso = json_object_new_int((int32_t)port);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_int fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, SNMP_TRAPITEM_PORT, tmp_jso);
    return ret;
}


LOCAL gint32 get_item_separator(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    guint8 delimiter_number = 0;
    gchar delimiter[SNMP_STRLEN_32 + 1] = {0};

    ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_SEPARATOR, &delimiter_number);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop %s fail (%d).", __func__, PROPERTY_TRAP_ITEM_SEPARATOR, ret));

    (void)snprintf_s(delimiter, SNMP_STRLEN_32 + 1, SNMP_STRLEN_32, "%c", delimiter_number);

    tmp_jso = json_object_new_string((const gchar *)delimiter);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string for prop %s fail.", __func__, PROPERTY_TRAP_ITEM_SEPARATOR));
    json_object_object_add(jso, SNMP_TRAP_MESSAGE_DELIMITER, tmp_jso);
    return ret;
}


LOCAL gint32 get_item_message_content(GSList *obj_node, json_object *jso)
{
    gint32 ret = VOS_OK;
    json_object *tmp_jso = NULL;
    guint8 content_enable = 0;
    json_object *message_context_json = NULL;
    const gchar* content_props[] = {PROPERTY_TRAP_ITEM_TIME, PROPERTY_TRAP_ITEM_SENSOR_NAME, PROPERTY_TRAP_ITEM_SEVERITY, PROPERTY_TRAP_ITEM_EVENT_CODE, PROPERTY_TRAP_ITEM_EVENT_DESC};
    const gchar* content_items[] = {SNMP_TRAP_MESSAGE_TIME_SELECTED, SNMP_TRAP_MESSAGE_SENSOR_NAME_SELECTED, SNMP_TRAP_MESSAGE_SEVERITY_SELECTED, SNMP_TRAP_MESSAGE_EVENT_CODE_SELECTED, SNMP_TRAP_MESSAGE_EVENT_DESCRIPTION_SELECTED};

    message_context_json = json_object_new_object();
    json_object_object_add(jso, SNMP_TRAP_MESSAGE_CONTENT, message_context_json);

    for (guint8 index_number = 0; index_number < G_N_ELEMENTS(content_props); index_number++) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, content_props[index_number], &content_enable);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: get prop %s fail (%d).", __func__, content_props[index_number], ret));

        tmp_jso = json_object_new_boolean(content_enable);
        return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: json_object_new_boolean for prop %s fail.", __func__,
            content_props[index_number]));
        json_object_object_add(message_context_json, content_items[index_number], tmp_jso);
        tmp_jso = NULL;
    }
    return ret;
}


LOCAL gint32 get_item_show_keyword(GSList *obj_node, json_object *jso)
{
    gint32 ret;
    json_object *tmp_jso = NULL;
    guint8 show_keyword_enabled_value = 0;

    ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_TRAP_ITEM_SHOW_KEYWORD,
        &show_keyword_enabled_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get prop %s fail (%d).", __func__, PROPERTY_TRAP_ITEM_SHOW_KEYWORD, ret));

    tmp_jso = json_object_new_boolean(show_keyword_enabled_value);
    return_val_do_info_if_fail(NULL != tmp_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string for prop %s fail.", __func__,
        PROPERTY_TRAP_ITEM_SHOW_KEYWORD));

    json_object_object_add(jso, SNMP_TRAP_MESSAGE_DISPLAY_KEYWORD_ENABLED, tmp_jso);
    return ret;
}


LOCAL gint32 get_trap_item(json_object **json_obj)
{
    gint32 ret;
    json_object *jso = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);

    ret = dfl_get_object_list(CLASS_TRAP_ITEM_CONFIG, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dfl_get_object_list fail\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        jso = json_object_new_object();
        return_val_do_info_if_fail(NULL != jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

        // 接收者序号
        ret = get_item_index_number(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }
        
        if (MGMT_SOFTWARE_TYPE_EM == software_type) {
            json_object_object_add(jso, SNMP_BOBTRAPITEM_ENABLE, NULL);
        } else
        
        {
            
            // 是否启用带内通道
            ret = get_bob_enable(obj_node, jso);
            if (ret != VOS_OK) {
                goto err_ret;
            }
            
        }
        // 是否启用该接收者
        ret = get_item_enable(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }

        // 接收者地址
        ret = get_item_dst_ipaddress(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }

        // 接收者端口
        ret = get_item_dst_port(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }

        
        // 报文分隔符，按ASCII码转换
        ret = get_item_separator(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }

        // 报文显示内容，包括时间、传感器名称、级别、事件码、事件描述
        ret = get_item_message_content(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }

        // 报文显示关键字使能
        ret = get_item_show_keyword(obj_node, jso);
        if (ret != VOS_OK) {
            goto err_ret;
        }
        

        ret = json_object_array_add(*json_obj, jso);
        if (ret != VOS_OK) {
            g_slist_free(obj_list);
            obj_list = NULL;
            (void)json_object_put(jso);
            jso = NULL;
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        jso = NULL;
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list); obj_list = NULL);
    return VOS_OK;

err_ret:
    (void)json_object_put(jso);
    g_slist_free(obj_list);
    obj_list = NULL;
    jso = NULL;
    return ret;
}

LOCAL gint32 get_trap_community(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **json_obj)
{
    gint32 ret;
    const gchar *event_des_str = NULL;
    GSList *output_list = NULL;
    GVariant *element = NULL;

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, g_manager_snmp_provider[i_paras->index].pme_class_name,
        METHOD_TRAP_GET_COMMUNITY_NAME, 0, 0, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: uip_call_class_method_redfish failed!", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    element = (GVariant *)g_slist_nth_data(output_list, 0);
    if (element == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: output_list is NULL", __FUNCTION__, __LINE__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    event_des_str = g_variant_get_string(element, NULL);
    *json_obj = json_object_new_string(event_des_str);
    return_val_do_info_if_fail(NULL != *json_obj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail", __FUNCTION__, __LINE__));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return VOS_OK;
}


LOCAL gint32 get_trap_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_new_object fail", __FUNCTION__, __LINE__));

    // TrapConfig信息
    // 获取TRAP功能启用状态
    ret = get_enable_state(g_manager_snmp_provider[i_paras->index].pme_class_name, PROPERTY_TRAP_ENABLE, &jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get_enable_state fail", __FUNCTION__, __LINE__));
    json_object_object_add(*o_result_jso, SNMP_TRAP_ENABLE, jso);
    jso = NULL;

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG));

    // TRAP使用的版本 0--V1 1--V2C 2--V3
    get_trap_version(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SNMP_TRAP_VERSION, jso);
    jso = NULL;

    // TRAPV3用户名
    get_trap_username(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SNMP_TRAP_V3USER, jso);
    jso = NULL;

    // TRAP上报的模式
    get_trap_mode(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SNMP_TRAP_MODE, jso);
    jso = NULL;

    // Trap主机标识
    get_trap_identity(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SNMP_TRAP_SERVICE_ID, jso);
    jso = NULL;

    // trap团体名
    get_trap_community(i_paras, obj_handle, &jso);
    json_object_object_add(*o_result_jso, SNMP_TRAP_COMMUNITY, jso);
    jso = NULL;

    // 发送的严重性级别
    get_trap_sendseverity(obj_handle, &jso);
    json_object_object_add(*o_result_jso, SNMP_TRAP_SEVERITY, jso);
    jso = NULL;

    // TrapItem消息
    jso = json_object_new_array();
    return_val_do_info_if_fail(jso != NULL, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s, %d:  json_object_new_array fail", __FUNCTION__, __LINE__));

    // 获取trapitem信息
    get_trap_item(&jso);
    json_object_object_add(*o_result_jso, SNMP_TRAPITEM_INFO, jso);

    return HTTP_OK;
}

LOCAL gint32 inquire_snmp_version(PROVIDER_PARAS_S *i_paras, guint8 *version)
{
    if (g_strcmp0(SNMP_PROPERTY_V1STATE, g_manager_snmp_provider[i_paras->index].pme_prop_name) == 0) {
        *version = SNMP_VERSION_V1;
    } else if (g_strcmp0(SNMP_PROPERTY_V2CSTATE, g_manager_snmp_provider[i_paras->index].pme_prop_name) == 0) {
        *version = SNMP_VERSION_V2C;
    } else if (g_strcmp0(PROPERTY_SNMP_V3STATUS, g_manager_snmp_provider[i_paras->index].pme_prop_name) == 0) {
        *version = SNMP_VERSION_V3;
    } else {
        return RET_ERR;
    }
    return HTTP_OK;
}


LOCAL gint32 set_snmp_version(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
                              json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_bool state;
    guint8 version = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_snmp_provider[i_paras->index].property_name, o_message_jso,
        g_manager_snmp_provider[i_paras->index].property_name));

    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].property_name);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_manager_snmp_provider[i_paras->index].property_name,
            o_message_jso, RF_VALUE_NULL, g_manager_snmp_provider[i_paras->index].property_name);
        return HTTP_BAD_REQUEST;
    }

    // 状态
    state = json_object_get_boolean(i_paras->val_jso);

    // 版本
    ret = inquire_snmp_version(i_paras, &version);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s, invalid property name %s", __FUNCTION__,
            g_manager_snmp_provider[i_paras->index].pme_prop_name);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));
    input_list = g_slist_append(input_list, g_variant_new_byte(version));
    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_SNMP_SETSNMPVERSIONSTATE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set Snmp version(%d) state(%d) failed!\r\n", version, state);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_snmp_v1v2c_login_rule(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 array_len;
    gint32 i = 0;
    gchar json_pointer[PROP_VAL_LENGTH] = {0};
    guint8 snmp_v1v2c_login = 0;
    const gchar *login_rule = NULL;
    const gchar* permit_role_map[MAX_RULE_COUNT] = {RFPROP_VAL_RULE1, RFPROP_VAL_RULE2, RFPROP_VAL_RULE3};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret_val;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    array_len = json_object_array_length(i_paras->val_jso);
    return_val_do_info_if_expr(array_len > MAX_RULE_COUNT, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_SNMPV1V2C_LOGIN_RULE, o_message_jso,
        RFPROP_SNMPV1V2C_LOGIN_RULE));

    if (json_array_duplicate_check(i_paras->val_jso, &i) == VOS_OK) {
        (void)snprintf_truncated_s(json_pointer, sizeof(json_pointer), "%s/%d", RFPROP_SNMPV1V2C_LOGIN_RULE, i);
        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, (const gchar *)json_pointer, o_message_jso,
            (const gchar *)json_pointer);
        return HTTP_BAD_REQUEST;
    }

    for (i = 0; i < array_len; i++) {
        login_rule = dal_json_object_get_str(json_object_array_get_idx(i_paras->val_jso, i));
        continue_if_expr(NULL == login_rule);

        if (0 == g_strcmp0(login_rule, permit_role_map[0])) {
            snmp_v1v2c_login |= BIT(0);
        } else if (0 == g_strcmp0(login_rule, permit_role_map[1])) {
            snmp_v1v2c_login |= BIT(1);
        } else { // 协议层会确保只有这3个枚举值
            snmp_v1v2c_login |= BIT(2);
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(snmp_v1v2c_login));

    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_SNMP, METHOD_SNMP_SETSNMPV1V2CPERMIT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    uip_free_gvariant_list(input_list);

    switch (ret_val) {
        case VOS_OK:
            if (VOS_OK == g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0))) {
                ret_val = HTTP_OK;
            } else {
                ret_val = HTTP_INTERNAL_SERVER_ERROR;
                debug_log(DLOG_ERROR, "%s: set snmpv1_v2c login rule(%s) failed, ret_val = %d", __FUNCTION__,
                    METHOD_SNMP_SETSNMPV1V2CPERMIT, g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0)));
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            }
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SNMPV1V2C_LOGIN_RULE, o_message_jso,
                RFPROP_SNMPV1V2C_LOGIN_RULE);
            break;

        default:
            debug_log(DLOG_ERROR, "%s: call %s failed, ret_val = %d", __FUNCTION__, METHOD_SNMP_SETSNMPV1V2CPERMIT,
                ret_val);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
    }
    uip_free_gvariant_list(output_list);

    return ret_val;
}


LOCAL gint32 set_snmp_longpassword(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_snmp_provider[i_paras->index].property_name, o_message_jso,
        g_manager_snmp_provider[i_paras->index].property_name));

    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].property_name);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_manager_snmp_provider[i_paras->index].property_name,
            o_message_jso, RF_VALUE_NULL, g_manager_snmp_provider[i_paras->index].property_name);
        return HTTP_BAD_REQUEST;
    }

    // 设置使能状态
    json_bool state = json_object_get_boolean(i_paras->val_jso);

    gint32 ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_SNMP_SETLONGPASSWORDENABLE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Snmp longpassword state failed!\r\n");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return ret;
    }

    return HTTP_OK;
}


LOCAL gint32 set_rw_community_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV,
        g_manager_snmp_provider[i_paras->index].property_name, o_message_jso,
        g_manager_snmp_provider[i_paras->index].property_name));

    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].property_name);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, g_manager_snmp_provider[i_paras->index].property_name,
            o_message_jso, RF_VALUE_NULL, g_manager_snmp_provider[i_paras->index].property_name);
        return HTTP_BAD_REQUEST;
    }

    // 设置使能状态
    json_bool state = json_object_get_boolean(i_paras->val_jso);
    
    return_val_do_info_if_expr(TRUE != state && FALSE != state, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    

    gint32 ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_SNMP_SET_RW_COMMUNITY_STATE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Snmp rw_community state failed!\r\n");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return ret;
    }

    return HTTP_OK;
}


LOCAL gint32 parse_snmp_community_return_value(PROVIDER_PARAS_S *i_paras, gint32 ret, gboolean is_rw_community, 
    json_object **o_message_jso)
{
    const gchar* prop_name = g_manager_snmp_provider[i_paras->index].property_name;
    switch (ret) {
        case RET_OK:
            return HTTP_OK;
        case SNMP_COMMUNITY_TOO_WEAK:
            debug_log(DLOG_ERROR, "%s: Community is in weak passwd dictionary.", __FUNCTION__);
            create_message_info(MSGID_COMMUNITY_IN_WEAK_PWDDICT, prop_name, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SNMP_COMMUNITY_SPACE_FAIL:
            debug_log(DLOG_ERROR, "%s: Community contain space chedck faile.", __FUNCTION__);
            create_message_info(MSGID_COMMUNITY_SPACE_ERR, prop_name, o_message_jso, prop_name);
            return HTTP_BAD_REQUEST;
        case SNMP_COMMUNITY_LENGTH_FAIL:
            debug_log(DLOG_ERROR, "%s: Community length check faile", __FUNCTION__);
            create_message_info(MSGID_COMMUNITY_LENGTH_ERR, prop_name, o_message_jso, prop_name);
            return HTTP_BAD_REQUEST;
        case SNMP_COMMUNITY_COMPLEXITY_FAIL:
            debug_log(DLOG_ERROR, "%s: Community is too simple.", __FUNCTION__);
            create_message_info(MSGID_COMMUNITY_SIMPLE_ERR, prop_name, o_message_jso, prop_name);
            return HTTP_BAD_REQUEST;
        case SNMP_COMMUNITY_FIELD:
            debug_log(DLOG_ERROR, "%s: Community is same with others.", __FUNCTION__);
            create_message_info(is_rw_community == TRUE ? MSGID_RWCOMMUNITY_SAME_ERR : MSGID_ROCOMMUNITY_SAME_ERR, 
                prop_name, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SNMP_COMMUNITY_PREVIOUS:
            debug_log(DLOG_ERROR, "%s: Community is the same as previous value.", __FUNCTION__);
            create_message_info(is_rw_community == TRUE ? MSGID_RWCOMMUNITY_SIMILAR_WITH_HISTORY : 
                MSGID_ROCOMMUNITY_SIMILAR_WITH_HISTORY, prop_name, o_message_jso);
            return HTTP_BAD_REQUEST; 
        default:
            debug_log(DLOG_ERROR, "%s: Set Snmp Community failed %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 get_snmp_rocommunity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *event_des_str = NULL;
    GSList *output_list = NULL;
    GVariant *element = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG));
   
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, g_manager_snmp_provider[i_paras->index].pme_class_name,
        METHOD_SNMP_GETSNMPROCOMMUNITY, 0, 0, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: uip_call_class_method_redfish failed!", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
  
    element = (GVariant *)g_slist_nth_data(output_list, 0);
    if (element == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: output_list is NULL", __FUNCTION__, __LINE__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    event_des_str = g_variant_get_string(element, NULL);
    *o_result_jso = json_object_new_string(event_des_str);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail", __FUNCTION__, __LINE__));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return HTTP_OK;
}


LOCAL gint32 get_snmp_rwcommunity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *event_des_str = NULL;
    GSList *output_list = NULL;
    GVariant *element = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, g_manager_snmp_provider[i_paras->index].pme_class_name,
        METHOD_SNMP_GETSNMPRWCOMMUNITY, 0, 0, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: uip_call_class_method_redfish failed!", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
 
    element = (GVariant *)g_slist_nth_data(output_list, 0);
    if (element == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: output_list is NULL", __FUNCTION__, __LINE__);
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    event_des_str = g_variant_get_string(element, NULL);
    *o_result_jso = json_object_new_string(event_des_str);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail", __FUNCTION__, __LINE__));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    
    return HTTP_OK;
}


LOCAL gint32 set_snmp_community(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gboolean is_rw_community; // 设置读写团体名
    const gchar* prop_name = g_manager_snmp_provider[i_paras->index].property_name;
    // 校验入参
    if (provider_paras_check(i_paras) != RET_OK || o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer!.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 判断权限
    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name);
        json_string_clear(i_paras->val_jso);
        return HTTP_FORBIDDEN;
    }

    if (i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: val_jso null", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name, o_message_jso, RF_VALUE_NULL, prop_name);
        return HTTP_BAD_REQUEST;
    }

    // 判断是设置snmp只读团体名还是读写团体名
    if (strcmp(prop_name, SNMP_RWCOMMUNITY_STR) == 0) {
        is_rw_community = TRUE;
    } else if (strcmp(prop_name, SNMP_ROCOMMUNITY_STR) == 0) {
        is_rw_community = FALSE;
    } else {
        debug_log(DLOG_ERROR, "%s: property_name value error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        json_string_clear(i_paras->val_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取类句柄
    gint32 ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        json_string_clear(i_paras->val_jso);
        debug_log(DLOG_ERROR, "%s: get object handle for %s fail", __FUNCTION__, 
            g_manager_snmp_provider[i_paras->index].pme_class_name);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 调用RPC方法设置团体名
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(i_paras->val_jso)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, 
        is_rw_community == TRUE ? METHOD_SNMP_SETSNMPRWCOMMUNITY : METHOD_SNMP_SETSNMPROCOMMUNITY, 
        AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    // 清空团体名
    json_string_clear(i_paras->val_jso);
    uip_free_gvariant_list(input_list);
    
    // 处理设置结果
    return parse_snmp_community_return_value(i_paras, ret, is_rw_community, o_message_jso);
}


LOCAL gint32 set_trap_enable(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    gchar prop_str[SNMP_STRLEN_128] = {0};
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, SNMP_TRAP_ENABLE) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    json_bool state = json_object_get_boolean(jso);

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_ENABLE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Trap enabled state failed.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_version(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    guint8 state = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    gchar prop_str[SNMP_STRLEN_128] = {0};

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, SNMP_TRAP_VERSION) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, (const gchar *)prop_str);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    const gchar* str = dal_json_object_get_str(jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    if (0 == g_strcmp0(str, SNMP_TRAPVER_V1)) {
        state = 0;
    } else if (0 == g_strcmp0(str, SNMP_TRAPVER_V2C)) {
        state = 1;
    } else if (0 == g_strcmp0(str, SNMP_TRAPVER_V3)) {
        state = 3;
    } else {
        debug_log(DLOG_ERROR, "%s, %d:  prop invalid, %s, %s\n", __FUNCTION__, __LINE__, SNMP_TRAP_VERSION, str);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_VERSION, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Trap TrapVersion failed.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_v3user(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    const gchar *str = NULL;
    gchar prop_str[SNMP_STRLEN_128] = {0};
    guint8 userid = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE user_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, SNMP_TRAP_V3USER) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail.", __FUNCTION__, __LINE__));

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, str, &user_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: user name %s is not exist.", __FUNCTION__, __LINE__, str);
        (void)create_message_info(MSGID_USERNAME_NOT_EXIST, (const gchar *)prop_str, &obj_jso, str,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return VOS_ERR;
    }

    ret = dal_get_property_value_byte(user_handle, PROPERTY_USER_ID, &userid);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_byte fail.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail.", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    input_list = g_slist_append(input_list, g_variant_new_byte(userid));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_TRAPV3_USERID, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case SNMP_PARAMETER_ERR:
            debug_log(DLOG_ERROR, "Set Trap v3user failed! check privilege or overdue fail");
            (void)create_message_info(MSGID_USERNAME_NOT_USED, (const gchar *)prop_str, &obj_jso, str,
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "Set Trap v3user fail.");
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_trap_mode(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    const char *str = NULL;
    guint8 state = 0;
    gchar prop_str[SNMP_STRLEN_128] = {0};
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, SNMP_TRAP_MODE) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, (const gchar *)prop_str);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    if (0 == g_strcmp0(str, SNMP_TRAPMODE_EVENT)) {
        state = 0;
    } else if (0 == g_strcmp0(str, SNMP_TRAPMODE_OID)) {
        state = 1;
    } else if (0 == g_strcmp0(str, SNMP_TRAPMODE_ALARM)) {
        state = 2;
    } else {
        debug_log(DLOG_ERROR, "%s, %d:  prop invalid, %s, %s.", __FUNCTION__, __LINE__, SNMP_TRAP_VERSION, str);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_MODE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Trap TrapMode failed.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_serverid(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    const char *str = NULL;
    guint8 state = 0;
    gchar prop_str[SNMP_STRLEN_128] = {0};
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, SNMP_TRAP_SERVICE_ID) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail.", __FUNCTION__, __LINE__));
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        if (0 == g_strcmp0(str, RF_HOST_NAME_STR)) {
            state = TRAP_ID_HOSTNAME;
        } else if (0 == g_strcmp0(str, RF_CHASSIS_NAME_STR)) {
            state = TRAP_ID_CHASSIS_NAME;
        } else if (0 == g_strcmp0(str, RF_CHASSIS_LOCATION_STR)) {
            state = TRAP_ID_CHASSIS_LOCATION;
        } else if (0 == g_strcmp0(str, RF_CHASSIS_SERIAL_NUMBER_STR)) {
            state = TRAP_ID_CHASSIS_SERIAL_NUMBER;
        } else {
            debug_log(DLOG_ERROR, "%s, %d:  prop invalid, %s, %s.", __FUNCTION__, __LINE__, SNMP_TRAP_VERSION, str);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].pme_class_name));
    } else {
        if (0 == g_strcmp0(str, RF_BRDSN_STR)) {
            state = TRAP_ID_BRDSN;
        } else if (0 == g_strcmp0(str, RF_PRODUCT_ASSET_TAG_STR)) {
            state = TRAP_ID_ASSETTAG;
        } else if (0 == g_strcmp0(str, RF_HOST_NAME_STR)) {
            state = TRAP_ID_HOSTNAME;
        } else {
            debug_log(DLOG_ERROR, "%s, %d:  prop invalid, %s, %s.", __FUNCTION__, __LINE__, SNMP_TRAP_VERSION, str);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
            g_manager_snmp_provider[i_paras->index].pme_class_name));
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_IDENTITY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Trap TrapServerIdentity fail.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_communityname(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    const gchar *str = NULL;
    gchar prop_str[SNMP_STRLEN_128] = {0};
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, SNMP_TRAP_COMMUNITY) <=
        0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail.", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(jso);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail.", __FUNCTION__, __LINE__));

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_string_clear(jso);
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    input_list = g_slist_append(input_list, g_variant_new_string(str));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_COMMUNITY_NAME, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    // 清空团体名
    json_string_clear(jso);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case VOS_OK:
            break;

        case SNMP_COMMUNITY_VERSION3_ERR:
            debug_log(DLOG_ERROR, "%s, %d :Set trap communityname failed, %s, version 3 can not setting community name",
                __FUNCTION__, __LINE__, prop_str);
            create_message_info(MSGID_V3_NOT_SET_COMMUNITY, (const gchar *)prop_str, &obj_jso);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

            return HTTP_BAD_REQUEST;

        case SNMP_COMMUNITY_SPACE_FAIL:
            debug_log(DLOG_ERROR, "%s, %d : %s, Community contain space faile.", __FUNCTION__, __LINE__, prop_str);
            create_message_info(MSGID_COMMUNITY_SPACE_ERR, (const gchar *)prop_str, &obj_jso,
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
            return HTTP_BAD_REQUEST;

        case SNMP_COMMUNITY_LENGTH_FAIL:
            debug_log(DLOG_ERROR, "%s, %d :Set trap communityname failed, Community length chedck faile, %s",
                __FUNCTION__, __LINE__, prop_str);
            create_message_info(MSGID_COMMUNITY_LENGTH_ERR, (const gchar *)prop_str, &obj_jso,
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

            return HTTP_BAD_REQUEST;

        case SNMP_COMMUNITY_COMPLEXITY_FAIL:
            debug_log(DLOG_ERROR, "%s, %d : Set trap communityname failed, community complexity chedck fail.",
                __FUNCTION__, __LINE__);
            create_message_info(MSGID_COMMUNITY_SIMPLE_ERR, (const gchar *)prop_str, &obj_jso,
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

            return HTTP_BAD_REQUEST;

        default:
            debug_log(DLOG_ERROR, "%s, %d : Set trap communityname fail.", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_sendseverity(PROVIDER_PARAS_S *i_paras, json_object *jso, json_object **o_message_jso)
{
    gint32 ret;
    gchar severity_str[PROP_VAL_LENGTH] = {0};
    guint8 state = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    errno_t err_ret;

    err_ret = redfish_get_severity_from_jso(jso, severity_str, sizeof(severity_str));
    return_val_do_info_if_expr(EOK != err_ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    ret = redfish_alarm_severities_change(&state, severity_str, 0);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: prase severity from string to byte fail, ret:%d", __FUNCTION__, __LINE__, ret));

    ret = dal_get_object_handle_nth(g_manager_snmp_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].pme_class_name));

    input_list = g_slist_append(input_list, g_variant_new_byte(state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SET_SEND_SEVERITY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Set Trap TrapVersion fail.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trapitem_ip(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso, gint32 i_index,
    json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    const gchar *ip_addr = NULL;
    gchar prop_str[SNMP_STRLEN_128] = {0};

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/%d/%s", SNMP_TRAP_INFO, SNMP_TRAPITEM_INFO,
        i_index, SNMP_TRAPITEM_IP) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == item_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    ip_addr = dal_json_object_get_str(item_jso);
    return_val_do_info_if_fail(NULL != ip_addr, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_string(ip_addr));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_IP_ADDR, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case VOS_OK:
            break;

        case SNMP_OUT_OF_RANGE_ERR:
        case SNMP_PARAMETER_ERR:
        case SENSOR_MODULE_INVALID_IP_ADDR:
        case SENSOR_MODULE_INVALID_HOST_DOMAIN_NAME:
            debug_log(DLOG_ERROR, "%s, %d, check IP address invalid!, %s", __FUNCTION__, __LINE__, ip_addr);
            (void)create_message_info(MSGID_SERVER_ADDRESS_ERR, (const gchar *)prop_str, &obj_jso, ip_addr,
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

            return VOS_ERR;

        default:
            debug_log(DLOG_ERROR, "%s, %d, Set Trap server IP address failed!", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trapitem_port(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso, gint32 i_index,
    json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    gint32 ip_port;
    gchar prop_str[SNMP_STRLEN_128] = {0};
    gchar val_str[SNMP_STRLEN_128] = {0};

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/%d/%s", SNMP_TRAP_INFO, SNMP_TRAPITEM_INFO,
        i_index, SNMP_TRAPITEM_PORT) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (NULL == item_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }
    if (TRUE == json_object_is_type(item_jso, json_type_double)) {
        if (snprintf_s(val_str, sizeof(val_str), sizeof(val_str) - 1, "%lf", json_object_get_double(item_jso)) <= 0) {
            debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)prop_str, &obj_jso,
            (const gchar *)val_str, (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    ip_port = json_object_get_int(item_jso);

    input_list = g_slist_append(input_list, g_variant_new_uint32(ip_port));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_IP_PORT, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    switch (ret) {
        case VOS_OK:
            break;

        case SNMP_OUT_OF_RANGE_ERR:
            debug_log(DLOG_ERROR, "%s, %d, trap portt is invalid, %d", __FUNCTION__, __LINE__, ip_port);
            if (snprintf_s(val_str, sizeof(val_str), sizeof(val_str) - 1, "%d", ip_port) <= 0) {
                debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
                return HTTP_INTERNAL_SERVER_ERROR;
            }
            (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, (const gchar *)prop_str, &obj_jso,
                (const gchar *)val_str, (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
            return VOS_ERR;

        default:
            debug_log(DLOG_ERROR, "%s, %d, Set Trap server IP address failed!", __FUNCTION__, __LINE__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trapitem_bobenable(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso,
    gint32 i_index, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    json_bool jso_ret;
    guint8 val = 0;
    gchar prop_str[SNMP_STRLEN_128] = {0};

    GSList *input_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/%d/%s", SNMP_TRAP_INFO, SNMP_TRAPITEM_INFO,
        i_index, SNMP_BOBTRAPITEM_ENABLE) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (NULL == item_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    jso_ret = json_object_get_boolean(item_jso);
    if (FALSE == jso_ret) {
        val = 0;
    } else if (TRUE == jso_ret) {
        val = 1;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(val));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_BOB_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d, Set Trap Server BOB enable failed!", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trapitem_enable(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso,
    gint32 i_index, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    json_bool jso_ret;
    guint8 val = 0;
    gchar prop_str[SNMP_STRLEN_128] = {0};

    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s/%d/%s", SNMP_TRAP_INFO, SNMP_TRAPITEM_INFO,
        i_index, SNMP_TRAPITEM_ENABLE) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (NULL == item_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, prop_str);
        (void)create_message_info(MSGID_PROP_ITEM_TYPE_ERR, (const gchar *)prop_str, &obj_jso, RF_VALUE_NULL,
            (const gchar *)prop_str);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_BAD_REQUEST;
    }

    jso_ret = json_object_get_boolean(item_jso);
    if (FALSE == jso_ret) {
        val = 0;
    } else if (TRUE == jso_ret) {
        val = 1;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(val));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d, Set Trap server enable failed!", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        ret = json_object_array_add(*o_message_jso, obj_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_message_delimiter(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso,
    gint32 i_index, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    const gchar *val_str = NULL;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(NULL != i_paras && NULL != item_jso && NULL != o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    val_str = dal_json_object_get_str(item_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte(*val_str));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_SEPARATOR, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Set prop %s to %c failed, index is %d.", __func__, SNMP_TRAP_MESSAGE_DELIMITER,
        *val_str, i_index);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        (void)json_object_array_add(*o_message_jso, obj_jso));

    return VOS_OK;
}


LOCAL gint32 set_trap_message_content(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso,
    gint32 i_index, json_object **o_message_jso)
{
    gint32 ret = 0;
    gint32 ret_code = VOS_ERR;
    json_bool jso_ret = FALSE;
    json_object *obj_jso = NULL;
    json_object *prop_jso = NULL;
    json_bool bool_value = FALSE;
    ;
    guint8 index_number;
    GSList *input_list = NULL;
    const gchar* content_items[] = {
        SNMP_TRAP_MESSAGE_TIME_SELECTED,
        SNMP_TRAP_MESSAGE_SENSOR_NAME_SELECTED,
        SNMP_TRAP_MESSAGE_SEVERITY_SELECTED,
        SNMP_TRAP_MESSAGE_EVENT_CODE_SELECTED,
        SNMP_TRAP_MESSAGE_EVENT_DESCRIPTION_SELECTED
    };
    const gchar* content_methods[] = {
        METHOD_TRAP_ITEM_SET_TIME_SEND,
        METHOD_TRAP_ITEM_SET_SENSOR_NAME_SEND,
        METHOD_TRAP_ITEM_SET_SEVERITY_SEND,
        METHOD_TRAP_ITEM_SET_EVENT_CODE_SEND,
        METHOD_TRAP_ITEM_SET_EVENT_DESC_SEND
    };

    
    return_val_do_info_if_fail(NULL != i_paras && NULL != item_jso && NULL != o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    for (index_number = 0; index_number < G_N_ELEMENTS(content_items); index_number++) {
        
        jso_ret = json_object_object_get_ex(item_jso, content_items[index_number], &prop_jso);
        if (TRUE == jso_ret) {
            
            bool_value = json_object_get_boolean(prop_jso);
            if (TRUE == bool_value) {
                input_list = g_slist_append(input_list, g_variant_new_byte(1));
            } else {
                input_list = g_slist_append(input_list, g_variant_new_byte(0));
            }

            
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                CLASS_TRAP_ITEM_CONFIG, content_methods[index_number], AUTH_ENABLE, i_paras->user_role_privilege,
                input_list, NULL);
            uip_free_gvariant_list(input_list);
            input_list = NULL;

            
            if (ret == VOS_OK) {
                ret_code = VOS_OK;
            } else {
                debug_log(DLOG_ERROR, "%s: Set prop %s to %d failed, index is %d.", __func__,
                    content_items[index_number], bool_value, i_index);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
                (void)json_object_array_add(*o_message_jso, obj_jso);
            }
        }

        prop_jso = NULL;
    }

    return ret_code;
}


LOCAL gint32 set_trap_message_display_keyword(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *item_jso,
    gint32 i_index, json_object **o_message_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    json_bool bool_value;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(NULL != i_paras && NULL != item_jso && NULL != o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    bool_value = json_object_get_boolean(item_jso);
    if (TRUE == bool_value) {
        input_list = g_slist_append(input_list, g_variant_new_byte(1));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(0));
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_TRAP_ITEM_CONFIG, METHOD_TRAP_ITEM_SET_SHOW_KEYWORD, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Set prop %s to %d failed, index is %d.", __func__,
        SNMP_TRAP_MESSAGE_DISPLAY_KEYWORD_ENABLED, bool_value, i_index);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &obj_jso);
        (void)json_object_array_add(*o_message_jso, obj_jso));

    return VOS_OK;
}


LOCAL gint32 set_trap_item(PROVIDER_PARAS_S *i_paras, json_object *array_jso, gint32 *succ_flag,
    json_object **o_message_jso)
{
    gint32 ret = 0;
    json_bool jso_ret = FALSE;
    gint32 array_index;
    gint32 len;
    OBJ_HANDLE obj_handle = 0;
    json_object *item_jso = NULL;
    json_object *tmp_obj = NULL;

    gchar prop_str[SNMP_STRLEN_128] = {0};
    guint8 software_type = 0;
    json_object *obj_jso = NULL;
    (void)dal_get_software_type(&software_type);
    len = json_object_array_length(array_jso);

    // 设置json数组数据
    for (array_index = 0; array_index < len; array_index++) {
        item_jso = NULL;
        item_jso = json_object_array_get_idx(array_jso, array_index);
        continue_do_info_if_fail(NULL != item_jso,
            debug_log(DLOG_ERROR, "%s, %d : item_jso is null, array_index %d", __FUNCTION__, __LINE__, array_index));

        ret = dal_get_specific_object_byte(CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_INDEX_NUM, array_index,
            &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: dal_get_specific_object_byte fail\n", __FUNCTION__, __LINE__));

        // 是否启用该接收者
        jso_ret = json_object_object_get_ex(item_jso, SNMP_TRAPITEM_ENABLE, &tmp_obj);
        if (TRUE == jso_ret) {
            ret = set_trapitem_enable(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
            do_if_expr(VOS_OK == ret, (*succ_flag)++);
        }
        tmp_obj = NULL;

        // 是否启用带内转发
        jso_ret = json_object_object_get_ex(item_jso, SNMP_BOBTRAPITEM_ENABLE, &tmp_obj);
        if (TRUE == jso_ret) {
            if (MGMT_SOFTWARE_TYPE_EM == software_type) {
                (void)memset_s(prop_str, sizeof(prop_str), 0, sizeof(prop_str));
                if (sprintf_s(prop_str, sizeof(prop_str), "%s/%s/%d/%s", SNMP_TRAP_INFO, SNMP_TRAPITEM_INFO,
                    array_index, SNMP_BOBTRAPITEM_ENABLE) > 0) {
                    (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)prop_str, &obj_jso,
                        (const gchar *)prop_str);
                    debug_log(DLOG_DEBUG, "%s, %d: unsupport modify SNMP_BOBTRAPITEM_ENABLE.", __FUNCTION__, __LINE__);
                    ret = json_object_array_add(*o_message_jso, obj_jso);
                    do_if_expr(ret != 0,
                        debug_log(DLOG_ERROR, "add snmp bobtrap enable message_jso to message_arr err,ret(%d).", ret);
                        json_object_put(obj_jso));
                } else {
                    debug_log(DLOG_ERROR, "%s, %d, sprintf_s fail", __FUNCTION__, __LINE__);
                }
            } else {
                ret = set_trapitem_bobenable(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
                do_if_expr(VOS_OK == ret, (*succ_flag)++);
            }
        }
        tmp_obj = NULL;
        

        // 设置接收者IP
        jso_ret = json_object_object_get_ex(item_jso, SNMP_TRAPITEM_IP, &tmp_obj);
        if (TRUE == jso_ret) {
            ret = set_trapitem_ip(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
            do_if_expr(VOS_OK == ret, (*succ_flag)++);
        }
        tmp_obj = NULL;

        // 设置接收者端口号
        jso_ret = json_object_object_get_ex(item_jso, SNMP_TRAPITEM_PORT, &tmp_obj);
        if (TRUE == jso_ret) {
            ret = set_trapitem_port(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
            do_if_expr(VOS_OK == ret, (*succ_flag)++);
        }
        tmp_obj = NULL;

        
        // 设置报文分隔符
        jso_ret = json_object_object_get_ex(item_jso, SNMP_TRAP_MESSAGE_DELIMITER, &tmp_obj);
        if (TRUE == jso_ret) {
            ret = set_trap_message_delimiter(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
            do_if_expr(VOS_OK == ret, (*succ_flag)++);
        }
        tmp_obj = NULL;

        // 设置报文显示内容选择，包括时间、传感器名称、级别、事件码、事件描述
        jso_ret = json_object_object_get_ex(item_jso, SNMP_TRAP_MESSAGE_CONTENT, &tmp_obj);
        if (TRUE == jso_ret) {
            ret = set_trap_message_content(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
            do_if_expr(VOS_OK == ret, (*succ_flag)++);
        }
        tmp_obj = NULL;

        // 设置 报文显示关键字使能
        jso_ret = json_object_object_get_ex(item_jso, SNMP_TRAP_MESSAGE_DISPLAY_KEYWORD_ENABLED, &tmp_obj);
        if (TRUE == jso_ret) {
            ret = set_trap_message_display_keyword(i_paras, obj_handle, tmp_obj, array_index, o_message_jso);
            do_if_expr(VOS_OK == ret, (*succ_flag)++);
        }
        tmp_obj = NULL;
        
    }

    return VOS_OK;
}


LOCAL gint32 set_trap_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret = 0;
    gint32 succ_flag = 0;
    gint32 len = 0;
    json_bool jso_ret;
    json_object *tmp_obj = NULL;
    json_object *obj_jso = NULL;
    gchar prop_str[SNMP_STRLEN_128] = {0};

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, 
        g_manager_snmp_provider[i_paras->index].property_name, o_message_jso, 
        g_manager_snmp_provider[i_paras->index].property_name); dal_proxy_operation_log(MODULE_NAME_REDFISH, 
        i_paras->user_name, i_paras->client, CLASS_TRAP_CONFIG, "Set trap info failed"));

    return_val_do_info_if_fail(NULL != i_paras->val_jso, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__,
        g_manager_snmp_provider[i_paras->index].property_name); (void)create_message_info(MSGID_PROP_TYPE_ERR,
        g_manager_snmp_provider[i_paras->index].property_name,
        o_message_jso, RF_VALUE_NULL, g_manager_snmp_provider[i_paras->index].property_name));

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  json_object_new_array fail\n", __FUNCTION__, __LINE__));

    // TRAP功能使能设置
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_ENABLE, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_enable(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // TRAP版本设置
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_VERSION, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_version(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // TRAPV3用户设置
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_V3USER, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_v3user(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // TRAP上报的模式设置
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_MODE, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_mode(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // Trap主机标识
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_SERVICE_ID, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_serverid(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // 团体名
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_COMMUNITY, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_communityname(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // 发送的严重性级别
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAP_SEVERITY, &tmp_obj);
    if (TRUE == jso_ret) {
        ret = set_trap_sendseverity(i_paras, tmp_obj, o_message_jso);
        do_if_expr(VOS_OK == ret, succ_flag++);
    }
    tmp_obj = NULL;

    // trapitem信息
    jso_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAPITEM_INFO, &tmp_obj);
    if (TRUE == jso_ret) {
        if (snprintf_s(prop_str, sizeof(prop_str), sizeof(prop_str) - 1, "%s/%s", SNMP_TRAP_INFO, "TrapServer") <= 0) {
            debug_log(DLOG_ERROR, "%s, %d, snprintf_s fail", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        len = json_object_array_length(tmp_obj);
        // 数组过多
        if (len > SNMP_ITEMCOUNT_MAX) {
            debug_log(DLOG_ERROR, "%s, %d, the items of %s is exceed max.", __FUNCTION__, __LINE__, prop_str);
            (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, (const gchar *)prop_str, &obj_jso, 
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
            goto Exit;
        }
        // 数组对象为空判断
        else if (0 == len || VOS_OK != json_array_is_empty(tmp_obj)) {
            debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s is null", __FUNCTION__, __LINE__, prop_str);
            (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, (const gchar *)prop_str, &obj_jso,
                (const gchar *)prop_str);
            ret = json_object_array_add(*o_message_jso, obj_jso);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: json_object_array_add fail\n", __FUNCTION__, __LINE__));
            goto Exit;
        }

        (void)set_trap_item(i_paras, tmp_obj, &succ_flag, o_message_jso);
    }

Exit:
    // 全部失败返回400 其他返回200
    return_val_if_expr(succ_flag <= 0, HTTP_BAD_REQUEST);

    return HTTP_OK;
}


LOCAL gint32 act_snmp_sendtest(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 trap_item_no = 0;
    const gchar *str = NULL;
    guint8 trap_state = 0;
    guint8 trapitem_state = 0;
    gchar ip_addr[SNMP_STRLEN_64] = {0};
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE trap_handle = 0;
    json_bool bool_ret;
    json_object *obj_json = NULL;

    GSList *input_list = NULL;
    GSList *output_list = NULL;
    GSList *caller_info = NULL;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_TRAPITEM_ID);
        (void)create_message_info(MSGID_PROP_MISSING, SNMP_TRAPITEM_ID, o_message_jso, SNMP_TRAPITEM_ID);

        return HTTP_BAD_REQUEST;
    }

    bool_ret = json_object_object_get_ex(i_paras->val_jso, SNMP_TRAPITEM_ID, &obj_json);
    if (FALSE == bool_ret) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_TRAPITEM_ID);
        (void)create_message_info(MSGID_PROP_MISSING, SNMP_TRAPITEM_ID, o_message_jso, SNMP_TRAPITEM_ID);

        return HTTP_BAD_REQUEST;
    }

    str = dal_json_object_get_str(obj_json);
    return_val_do_info_if_fail(NULL != str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_json_object_get_str fail\n", __FUNCTION__, __LINE__));

    ret = vos_str2int(str, 10, &trap_item_no, NUM_TPYE_INT32);
    if (VOS_OK != ret) { // 转换失败 输入参数错误
        debug_log(DLOG_ERROR, "%s, %d, vos_str2int fail", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, SNMP_TRAPITEM_ID, o_message_jso, str, SNMP_TRAPITEM_ID);

        return HTTP_BAD_REQUEST;
    }

    if (trap_item_no < SNMP_CHAR_MIN || trap_item_no > SNMP_CHAR_MAX) {
        // 数据范围错误
        debug_log(DLOG_ERROR, "%s, %d, value is out of range,  %s, %d", __FUNCTION__, __LINE__, SNMP_TRAPITEM_ID,
            trap_item_no);

        (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, SNMP_TRAPITEM_ID, o_message_jso, SNMP_TRAPITEM_ID);

        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_specific_object_byte(CLASS_TRAP_ITEM_CONFIG, PROPERTY_TRAP_ITEM_INDEX_NUM, trap_item_no, &obj_handle);
    if (VOS_OK != ret) {
        // 无效的memberid
        debug_log(DLOG_ERROR, "%s, %d, dal_get_specific_object_byte fail, TrapItemCfg, IndexNum, %d", __FUNCTION__,
            __LINE__, trap_item_no);
        (void)create_message_info(MSGID_TRAPITEM_MEMBERID_NOTEXIST, SNMP_TRAPITEM_ID, o_message_jso, str,
            SNMP_TRAPITEM_ID);

        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_TRAP_CONFIG, 0, &trap_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle for %s fail", __FUNCTION__, __LINE__, CLASS_TRAP_CONFIG));
    ret = dal_get_property_value_byte(trap_handle, PROPERTY_TRAP_ENABLE, &trap_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_byte fail\n", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_TRAP_ITEM_ENABLE, &trapitem_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_byte fail\n", __FUNCTION__, __LINE__));
    if (trap_state != 1 || trapitem_state != 1) {
        // 没有使能
        debug_log(DLOG_ERROR, "%s, %d, Trap server enable closed. IndexNum %d", __FUNCTION__, __LINE__, trap_item_no);
        create_message_info(MSGID_TRAPITEM_SENDTEST_ERR, SNMP_TRAPITEM_ID, o_message_jso, str, SNMP_TRAPITEM_ID);

        goto __ERROR;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_TRAP_ITEM_DEST_IPADDR, ip_addr, sizeof(ip_addr));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_string fail\n", __FUNCTION__, __LINE__));
    if (0 == strlen(ip_addr)) {
        // ip错误
        debug_log(DLOG_ERROR, "%s, %d, Trap server IP is null. IndexNum %d", __FUNCTION__, __LINE__, trap_item_no);
        create_message_info(MSGID_TRAPITEM_SENDTEST_ERR, SNMP_TRAPITEM_ID, o_message_jso, str, SNMP_TRAPITEM_ID);

        goto __ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_TRAP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_object_handle_nth fail\n", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(trap_item_no));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_TRAP_SEND_TEST, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d, Send trap test failed!", __FUNCTION__, __LINE__);
        
        create_message_info(MSGID_TRAPITEM_SENDTEST_ERR, SNMP_TRAPITEM_ID, o_message_jso, str, SNMP_TRAPITEM_ID);

        goto __ERROR;
        
    }

    return HTTP_OK;

    
__ERROR:
    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name)),
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Test trap destination %d address failed",
        trap_item_no + 1);
    uip_free_gvariant_list(caller_info);
    return HTTP_BAD_REQUEST;

    
}


LOCAL gint32 act_snmp_config_snmp_privpasswd(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar snmp_status = 0;
    guchar configure_self_priv = DISABLE;
    guint32 product_ver = 0;
    const gchar *snmp_auth_user = NULL;
    const gchar *snmp_priv_pwd = NULL;
    GVariant *property_value = NULL;
    guchar user_id;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if ((VOS_OK != provider_paras_check(i_paras)) || (NULL == o_message_jso)) {
        debug_log(DLOG_ERROR, "%s %d NULL Pointer!.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN,
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_user_role_priv_enable(i_paras->user_name, PROPERTY_USERROLE_CONFIGURE_SELF, &configure_self_priv);

    return_val_do_info_if_fail(DISABLE != configure_self_priv, HTTP_FORBIDDEN,
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &snmp_status);

    ret = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    if ((DISABLE == snmp_status) || (PRODUCT_VERSION_V5 > product_ver)) {
        debug_log(DLOG_DEBUG, "%s, %d: snmp not supported\n", __FUNCTION__, __LINE__);
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        create_message_info(MSGID_ACT_NOT_SUPPORTED, SNMP_ACTIONS_SNMPPASSWD_STR, o_message_jso,
            SNMP_ACTIONS_SNMPPASSWD_STR);
        return HTTP_BAD_REQUEST;
    }

    if (NULL == i_paras->val_jso) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_V3AUTH_USER);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, SNMP_ACTIONS_SNMPPASSWD_STR,
            SNMP_V3AUTH_USER);

        return HTTP_BAD_REQUEST;
    }

    if (FALSE == get_element_str(i_paras->val_jso, SNMP_V3AUTH_USER, &snmp_auth_user)) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_V3AUTH_USER);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, SNMP_ACTIONS_SNMPPASSWD_STR,
            SNMP_V3AUTH_USER);
        return HTTP_BAD_REQUEST;
    }

    if (FALSE == get_element_str(i_paras->val_jso, SNMP_V3PRIVPASSWD, &snmp_priv_pwd)) {
        debug_log(DLOG_ERROR, "%s, %d, wrong value type,  %s", __FUNCTION__, __LINE__, SNMP_V3PRIVPASSWD);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, SNMP_ACTIONS_SNMPPASSWD_STR,
            SNMP_V3PRIVPASSWD);
        return HTTP_BAD_REQUEST;
    }

    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == FALSE) {
        if (0 != g_strcmp0(snmp_auth_user, i_paras->user_name)) {
            debug_log(DLOG_ERROR, "no Privilege to config user_name=%s  \n", snmp_auth_user);
            json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
            json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        }
    }

    property_value = g_variant_new_string(snmp_auth_user);
    ret = dfl_get_specific_object(CLASS_USER, PROPERTY_USER_NAME, property_value, &obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Get user object handle failed!\n");
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso, SNMP_V3AUTH_USER,
            SNMP_ACTIONS_SNMPPASSWD_STR);
        return HTTP_BAD_REQUEST;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_USER_ID, &property_value);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "Get user id failed!\n");
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    user_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    ret = dal_get_object_handle_nth(CLASS_USER, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
        json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_object_handle_nth fail\n", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(snmp_priv_pwd));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        g_manager_snmp_provider[i_paras->index].pme_class_name, METHOD_USER_SETSNMPPRIVACYPASSWORD, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    json_object_clear_string(i_paras->val_jso, SNMP_V3AUTH_USER);
    json_object_clear_string(i_paras->val_jso, SNMP_V3PRIVPASSWD);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case USER_INVALID_DATA_FIELD:
        case USER_DONT_EXIST:
            (void)create_message_info(MSGID_INVALID_USERNAME, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case USER_DATA_LENGTH_INVALID:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, SNMP_V3PRIVPASSWD, o_message_jso,
                USER_PASSWORD_MIN_LEN_STR, USER_PWD_MAX_LEN_STR);
            return HTTP_BAD_REQUEST;

        case USER_SET_PASSWORD_TOO_WEAK:
            (void)create_message_info(MSGID_PSWD_IN_WEAK_PWDDICT, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case USER_SET_PASSWORD_EMPTY:
            (void)create_message_info(MSGID_INVALID_PAWD, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case USER_PASS_COMPLEXITY_FAIL:
            (void)create_message_info(MSGID_PSWD_CMPLX_CHK_FAIL, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case USER_NODE_BUSY:
            (void)create_message_info(MSGID_OPERATION_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 snmp_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean bool_ret;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar val = DISABLE;

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_object_handle_nth fail\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_SNMPD, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_byte fail\n", __FUNCTION__, __LINE__));

    if (val == DISABLE) {
        debug_log(DLOG_INFO, "%s, %d: PME not support Snmp service", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    bool_ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (0 == bool_ret) {
        debug_log(DLOG_ERROR, "%s, %d: redfish_check_manager_uri_valid fail", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    
    (void)dal_get_object_handle_nth(CLASS_SNMP, 0, &i_paras->obj_handle);
    

    *prop_provider = g_manager_snmp_provider;
    *count = sizeof(g_manager_snmp_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 snmp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean bool_ret;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar val = DISABLE;

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_object_handle_nth fail\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_SNMPD, &val);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_byte fail\n", __FUNCTION__, __LINE__));

    if (DISABLE == val) {
        debug_log(DLOG_ERROR, "%s, %d: PME not support Snmp service", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    bool_ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (0 == bool_ret) {
        debug_log(DLOG_ERROR, "%s, %d: redfish_check_manager_uri_valid fail", __FUNCTION__, __LINE__);
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_snmp_actioninfo_provider;
    *count = sizeof(g_manager_snmp_actioninfo_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
