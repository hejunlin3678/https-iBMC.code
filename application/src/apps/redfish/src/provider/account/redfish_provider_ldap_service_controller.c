

#include "redfish_provider.h"
#include "../apps/bmc_global/inc/bmc.h"
#include <ldap.h>

typedef struct priv_type {
    guchar priv_num;
    gchar priv_str[ARRAY_LENTH];
} PRIV_TYPE;
LOCAL PRIV_TYPE priv_arr_ldap[] = {{2, "Common User"}, {3, "Operator"}, {4, "Administrator"}, {5, "Custom Role 1"},
                                   {6, "Custom Role 2"}, {7, "Custom Role 3"}, {8, "Custom Role 4"}, {15, "No Access"}};

LOCAL gint32 get_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_controller_cert_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_controller_cert_level(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_controller_cert_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_controller_cert_chain_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_controller_groups_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_ldap_controller_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_controller_certificate_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_controller_certificate_level(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_controller_groups(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_controller_group_user_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id);
LOCAL gint32 set_controller_group_user_folder(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id);
LOCAL gint32 set_controller_group_user_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id);
LOCAL gint32 set_controller_group_user_role(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id);
LOCAL gint32 set_controller_group_user_permit_role(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id);
LOCAL gint32 set_controller_group_user_login_interface(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id);
LOCAL gint32 act_import_ldap_certificate_to_server(json_object *body_jso, json_object *user_data,
    json_object **message_obj, OBJ_HANDLE ldap_obj);
LOCAL gint32 act_import_ldap_certificat_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_import_ldap_certificate_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar *uri, OBJ_HANDLE obj_handle, guchar controller_id);
LOCAL TASK_MONITOR_INFO_S *ldap_cert_import_info_new(guchar from_webui_flag, const gchar *user_name,
    const gchar *client, gint32 user_priv, OBJ_HANDLE obj_handle, guchar controller_id);
LOCAL gint32 ldap_cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 get_controller_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_controller_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_controller_data_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_controller_bind_dn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_controller_bind_dn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_controller_bind_pwd(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 rf_check_domain_name(json_object *one_group);
LOCAL gint32 rf_check_domain_folder(json_object *one_group);


LOCAL PROPERTY_PROVIDER_S ldapservice_controller_provider[] = {
    {
        PROPERTY_LDAP_SERVICE_ODATA_CONTEXT,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_odata_context,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },

    {
        PROPERTY_LDAP_SERVICE_ODATA_ID,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_odata_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_ID,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_data_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_ADDRESS,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_addr,
        set_controller_addr,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_PORT,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_port,
        set_controller_port,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_USER_DOMAIN,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_domain,
        set_controller_domain,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_BIND_DN,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_bind_dn,
        set_controller_bind_dn,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_BIND_PWD,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        NULL,
        set_controller_bind_pwd,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CERT_ENABLED,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_cert_enable,
        set_controller_certificate_enable,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CRL_ENABLED,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_crl_enabled,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CRL_VALID_FROM,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_crl_start_time,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CRL_VALID_TO,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_crl_expire_time,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        PROPERTY_LDAP_SERVICE_CERT_LEVEL,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_cert_level,
        set_controller_certificate_level,
        NULL, ETAG_FLAG_ENABLE
    },
    
    {
        PROPERTY_LDAP_SERVICE_CERT_INFO,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_cert_info,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CERT_CHAIN_INFO,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_controller_cert_chain_info,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_LDAPGROUPS,
        LDAP_GROUP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_controller_groups_info,
        set_controller_groups,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_ACTION,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_ldap_controller_actions,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CERT_IMPORT,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_import_ldap_certificat_entry, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CRL_IMPORT,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_import_ldap_crl, ETAG_FLAG_ENABLE
    },
    {
        PROPERTY_LDAP_SERVICE_CRL_DELETE,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_delete_ldap_crl, ETAG_FLAG_ENABLE
    }
};

LOCAL gint32 get_controller_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
        PROPERTY_LDAP_SERVICE_ODATA_CONTEXT_STR, i_paras->member_id);
    if (VOS_OK < ret) {
        *o_result_jso = json_object_new_string((const gchar*)odata_context);

        if (NULL != *o_result_jso) {
            return HTTP_OK;
        }
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_controller_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, "%s%s", PROPERTY_LDAP_SERVICE_COLLECTION_MEMBER,
        i_paras->member_id);
    if (VOS_OK < ret) {
        *o_result_jso = json_object_new_string((const gchar*)odata_id);

        if (NULL != *o_result_jso) {
            return HTTP_OK;
        }
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 get_controller_data_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(i_paras->member_id);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL void parse_single_ldap_value(json_object *obj_json, gchar *value_str, const gchar *str_value)
{
    json_object *val_json = NULL;

    if (NULL == str_value || VOS_OK == g_strcmp0("", str_value)) {
        json_object_object_add(obj_json, value_str, NULL);
    } else {
        val_json = json_object_new_string(str_value);
        if (NULL != val_json) {
            json_object_object_add(obj_json, value_str, val_json);
        } else {
            json_object_object_add(obj_json, value_str, NULL);
        }
    }

    return;
}


LOCAL gint32 get_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    gchar controller_addr[ARRAY_LENTH * 2] = {0};
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &member_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_string(obj_handle, LDAP_ATTRIBUTE_HOST_ADDR, controller_addr, sizeof(controller_addr));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get controller_addr failed.", __FUNCTION__, __LINE__));

    if (VOS_OK != g_strcmp0("", controller_addr)) {
        *o_result_jso = json_object_new_string((const gchar*)controller_addr);

        if (NULL != *o_result_jso) {
            return HTTP_OK;
        }
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 get_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    gint32 controller_port = 0;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &member_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_int32(obj_handle, LDAP_ATTRIBUTE_PORT, &controller_port);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get port failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_int(controller_port);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 get_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    const gchar *user_domain = NULL;
    GSList *output_list = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &member_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_COMPOSE_USER_DOMAIN, AUTH_DISABLE, AUTH_PRIV_NONE, NULL, &output_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get controller_domain failed.", __FUNCTION__, __LINE__));
    user_domain = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), 0);
    if (VOS_OK != g_strcmp0("", user_domain)) {
        *o_result_jso = json_object_new_string(user_domain);

        if (NULL != *o_result_jso) {
            uip_free_gvariant_list(output_list);
            return HTTP_OK;
        }
    }

    uip_free_gvariant_list(output_list);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 get_controller_bind_dn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar bind_dn[SMALL_BUFFER_SIZE] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(obj_handle, LDAP_ATTRIBUTE_BIND_DN, bind_dn, sizeof(bind_dn));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get bind dn failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar*)bind_dn);
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_controller_cert_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    guchar cert_enable = 0;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &member_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    ret = dal_get_property_value_byte(obj_handle, LDAP_ATTRIBUTE_CERTSTATUS, &cert_enable);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get cert_enable failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_boolean((0 == cert_enable) ? FALSE : TRUE);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL INT2STR_MAP_S cert_level_num_string_map[] = {{LDAP_OPT_X_TLS_DEMAND, "Demand"}, {LDAP_OPT_X_TLS_ALLOW, "Allow"}};


LOCAL gint32 get_controller_cert_level(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    guchar cert_level = 0;
    OBJ_HANDLE obj_handle = 0;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_do_info_if_expr(ret != 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_INFO, "%s, %d: NULL pointer or frobidden.", __FUNCTION__, __LINE__));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &member_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, LDAP_ATTRIBUTE_CERT_VERIFI_LEVEL, &cert_level);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get cert_enable failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((cert_level_num_string_map[1].int_val == cert_level) ?
        cert_level_num_string_map[1].str_val :
        cert_level_num_string_map[0].str_val);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_controller_certificate_level(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    const gchar *cert_level_string = NULL;
    GSList *input_list = NULL;
    guchar level;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_BAD_REQUEST;
    }

    
    cert_level_string = dal_json_object_get_str(i_paras->val_jso);
    if (0 == g_strcmp0(cert_level_num_string_map[1].str_val, cert_level_string)) {
        level = (guchar)cert_level_num_string_map[1].int_val;
    } else {
        level = (guchar)cert_level_num_string_map[0].int_val;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(level));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_CERT_VERIFI_LEVEL, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);

    uip_free_gvariant_list(input_list);

    return get_set_function_normal_ret(i_paras, ret, o_message_jso, cert_level_string, "CertificateVerificationLevel");
}




LOCAL gint32 packet_query_ldap_cert_param(GSList **prop_name_list)
{
    return_val_do_info_if_expr(prop_name_list == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__));

    
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_SUBJECT_INFO);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_ISSUER_INFO);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_START_TIME);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_EXPIRATION);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_SERIALNUMBER);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_KEYUSAGE);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_KEYLENGTH);
    *prop_name_list = g_slist_append(*prop_name_list, LDAP_CERT_PROPERTY_SIGALGO);

    return VOS_OK;
}


LOCAL gint32 x509_ldap_cert_info_pack(OBJ_HANDLE object_handle, json_object **cert_info_jso)
{
    _cleanup_gslist_ GSList *prop_name_list = NULL;
    _cleanup_uip_gvariant_list_ GSList *prop_val_list = NULL;
    gint32 ret;
    const gchar *subject_str = NULL;
    const gchar *issuer_str = NULL;
    const gchar *start_time_str = NULL;
    const gchar *expiration_str = NULL;
    const gchar *serial_number_str = NULL;
    const gchar *sig_algo_str = NULL;
    const gchar *key_usage_str = NULL;
    guint32 key_len;

    ret = packet_query_ldap_cert_param(&prop_name_list);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s packet_query_ldap_cert_param failed", __FUNCTION__));

    ret = dfl_multiget_property_value(object_handle, prop_name_list, &prop_val_list);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:cann't get mutiple properties", __FUNCTION__));

    json_object* _cleanup_json_object_ info_jso = json_object_new_object();
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:alloc a new jso failed", __FUNCTION__));

    subject_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 0), NULL);
    issuer_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 1), NULL);
    start_time_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 2), NULL);
    expiration_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 3), NULL);
    serial_number_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 4), NULL);
    key_usage_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 5), NULL);
    key_len = g_variant_get_uint32((GVariant *)g_slist_nth_data(prop_val_list, 6));
    sig_algo_str = g_variant_get_string((GVariant *)g_slist_nth_data(prop_val_list, 7), NULL);

    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_ISSUEBY, json_object_new_string(issuer_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_ISSUETO, json_object_new_string(subject_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_VALIDFROM, json_object_new_string(start_time_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_VALIDTO, json_object_new_string(expiration_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_SERIALNUMBER,
        json_object_new_string(serial_number_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_SIGALGO, json_object_new_string(sig_algo_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_KEYUSAGE, json_object_new_string(key_usage_str));
    json_object_object_add(info_jso, PROPERTY_LDAP_SERVICE_CERT_KEYLENGTH, json_object_new_int(key_len));

    *cert_info_jso = TAKE_PTR(info_jso);
    return VOS_OK;
}


LOCAL gint32 get_controller_cert_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE ldap_cert_handle = 0;
    guint8 cert_id;
    guint8 cert_type = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &controller_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    cert_id = (controller_id - 1) * CERT_CHAIN_LDAP_MAX + 1;
    debug_log(DLOG_DEBUG, "[%s]operating ldap cert id = %d, serverid = %d\n", __FUNCTION__, cert_id, controller_id);
    ret = dal_get_specific_object_byte(LDAP_CERT_CLASS_NAME, LDAP_CERT_PROPERTY_CERTID, cert_id, &ldap_cert_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s]cann't get LDAPCert obj, certid = %d", __FUNCTION__, cert_id));

    
    ret = dal_get_property_value_byte(ldap_cert_handle, LDAP_CERT_PROPERTY_CERTTYPE, &cert_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "[%s]get property %s failed.", __FUNCTION__, LDAP_CERT_PROPERTY_CERTTYPE));
    
    return_val_do_info_if_expr(cert_type == CERT_TYPE_RESERVED, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "[%s]LDAPCert is invalid, cert_type = %d", __FUNCTION__, cert_type));

    ret = x509_ldap_cert_info_pack(ldap_cert_handle, o_result_jso);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "[%s]pack ldap cert info failed.", __FUNCTION__));
    
    return HTTP_OK;
}


LOCAL gint32 get_controller_cert_chain_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *cert_info_jso = NULL;
    json_object *x509_cert_jso = NULL;
    json_object *intermediate_cert_array_jso = NULL;
    GSList *ldap_cert_list = NULL;
    GSList *ldap_cert_node = NULL;
    OBJ_HANDLE ldap_cert_handle = 0;
    guint8 cert_id = 0;
    guint8 cert_type = 0;
    guint8 count = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &controller_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(LDAP_CERT_CLASS_NAME, &ldap_cert_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Get LDAPCert list failed.", __FUNCTION__, __LINE__));

    for (ldap_cert_node = ldap_cert_list; ldap_cert_node; ldap_cert_node = ldap_cert_node->next) {
        ldap_cert_handle = (OBJ_HANDLE)ldap_cert_node->data;
        ret = dal_get_property_value_byte((OBJ_HANDLE)ldap_cert_node->data, LDAP_CERT_PROPERTY_CERTID, &cert_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "[%s]get property %s failed.", __FUNCTION__, LDAP_CERT_PROPERTY_CERTID));

        
        if (((cert_id - 1) / CERT_CHAIN_LDAP_MAX + 1) != controller_id) {
            debug_log(DLOG_DEBUG, "[%s]LDAP cert_id = %d doesn't match controller_id = %d\n", __FUNCTION__, cert_id,
                controller_id);
            continue;
        }

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)ldap_cert_node->data, LDAP_CERT_PROPERTY_CERTTYPE, &cert_type);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "[%s]get property %s failed.", __FUNCTION__, LDAP_CERT_PROPERTY_CERTTYPE));
        
        continue_do_info_if_expr(cert_type == CERT_TYPE_RESERVED,
            debug_log(DLOG_DEBUG, "[%s]LDAPCert is invalid", __FUNCTION__));

        ret = x509_ldap_cert_info_pack(ldap_cert_handle, &x509_cert_jso);
        
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "[%s]pack ldap cert info failed.", __FUNCTION__));
        
        
        count++;
        if (count == 1) {
            cert_info_jso = json_object_new_object();
        }

        if (CERT_TYPE_SERVER == cert_type) {
            json_object_object_add(cert_info_jso, RFPROP_LDAP_CERT_SEVER_CERT, x509_cert_jso);
        } else if (CERT_TYPE_INTERMEDIATE == cert_type) {
            (void)json_object_object_get_ex(cert_info_jso, RFPROP_LDAP_CERT_INTER_CERT, &intermediate_cert_array_jso);
            do_val_if_fail(NULL != intermediate_cert_array_jso, intermediate_cert_array_jso = json_object_new_array();
                json_object_object_add(cert_info_jso, RFPROP_LDAP_CERT_INTER_CERT, intermediate_cert_array_jso));

            ret = json_object_array_add(intermediate_cert_array_jso, x509_cert_jso);
            continue_do_info_if_fail(0 == ret, (void)json_object_put(x509_cert_jso));
        } else if (CERT_TYPE_ROOT == cert_type) {
            json_object_object_add(cert_info_jso, RFPROP_LDAP_CERT_ROOT_CERT, x509_cert_jso);
        } else {
            (void)json_object_put(x509_cert_jso);
            debug_log(DLOG_ERROR, "[%s]unknown cert type :%d", __FUNCTION__, cert_type);
        }
    }

    g_slist_free(ldap_cert_list);
    if (count == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = cert_info_jso;

    return HTTP_OK;
}


LOCAL gint32 get_controller_groups_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 loop_tag = 0;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *group_list = NULL;
    GSList *group_node = NULL;
    json_object *user_group_info = NULL;
    guchar group_id = 0;
    guchar group_server_id = 0;
    gchar group_name[ARRAY_LENTH * 2] = {0};
    gchar group_folder[ARRAY_LENTH * 2] = {0};
    gchar permit_role_link[ARRAY_LENTH * 2] = {0};
    guchar group_user_role = 0;
    guchar group_permit_role = 0;
    guint32 group_login_interface = 0;
    json_object *permit_role_arr = NULL;
    json_object *one_permit_role = NULL;
    json_object *login_interface_arr = NULL;
    gint32 right_num[] = {0, 3, 7};
    const gchar* login_interface[] = {"Web", "SSH", "Redfish"};
    gchar slot_str[MAX_RSC_ID_LEN] = {0};
    const gchar *user_domain = NULL;
    GSList *output_list = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (VOS_ERR == get_controller_id(i_paras->uri, &controller_id, &obj_handle)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dfl_get_object_list(LDAP_GROUP_CLASS_NAME, &group_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Get group list failed.", __FUNCTION__, __LINE__));
    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, VOS_ERR, g_slist_free(group_list);
        group_list = NULL;
        debug_log(DLOG_ERROR, "%s, %d: Get new json array failed.", __FUNCTION__, __LINE__));

    
    for (group_node = group_list; group_node; group_node = group_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_GROUP_ID, &group_id);
        continue_if_fail(VOS_OK == ret);

        ret = dal_get_property_value_byte((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_LDAPSERVER_ID,
            &group_server_id);
        
        if ((VOS_OK != ret) || ((group_server_id & 0x0f) >= LOG_TYPE_KRB_SERVER) ||
            ((group_server_id - controller_id) % 16 != 0)) {
            continue;
        }
        
        
        user_group_info = json_object_new_object();
        if (NULL == user_group_info) {
            continue;
        }

        
        
        json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_ID, json_object_new_int((gint32)group_id));
        
        ret = dal_get_property_value_string((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_GROUP_NAME, group_name,
            sizeof(group_name));
        parse_single_ldap_value(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_NAME, group_name);

        
        ret = dal_get_property_value_string((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_GROUP_FOLDER,
            group_folder, sizeof(group_folder));
        parse_single_ldap_value(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_FOLDER, group_folder);

        
        
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            (OBJ_HANDLE)group_node->data, LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_COMPOSE_GROUP_DOMAIN, AUTH_DISABLE,
            AUTH_PRIV_NONE, NULL, &output_list);
        
        return_val_do_info_if_fail(VOS_OK == ret, ret, g_slist_free(group_list); json_object_put(user_group_info);
            debug_log(DLOG_ERROR, "%s, %d: Set LDAP_GROUP_METHOD_COMPOSE_GROUP_DOMAIN return failed!", __FUNCTION__,
            __LINE__));
        user_domain = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), 0);
        if (NULL == user_domain) {
            json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, NULL);
        } else {
            parse_single_ldap_value(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, user_domain);
        }

        uip_free_gvariant_list(output_list);

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_GROUP_USERROLEID,
            &group_user_role);
        if (8 >= group_user_role && 2 <= group_user_role && VOS_OK == ret) {
            json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE,
                json_object_new_string(priv_arr_ldap[group_user_role - 2].priv_str));
        } else if (VOS_OK == ret) {
            json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE,
                json_object_new_string(priv_arr_ldap[7].priv_str));
        } else {
            json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE, NULL);
        }

        
        ret = redfish_get_board_slot(slot_str, sizeof(slot_str));
        if (VOS_OK != ret) {
            json_object_put(user_group_info);
            continue;
        }

        
        (void)dal_get_property_value_byte((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_GROUP_PERMIT_ID,
            &group_permit_role);

        permit_role_arr = json_object_new_array();

        for (loop_tag = 0; loop_tag < 3; loop_tag++) {
            if ((group_permit_role >> loop_tag) & 1) {
                one_permit_role = json_object_new_object();
                if (NULL == one_permit_role) {
                    continue;
                }

                // "@odata.id": "/redfish/v1/Manager/<id>#/Oem/Huawei/LoginRole/<RoleId>"
                
                ret = snprintf_s(permit_role_link, sizeof(permit_role_link), sizeof(permit_role_link) - 1,
                    URI_FORMAT_LDAP_SERVICE_PCIEDEVICES_URI, slot_str, (loop_tag + 1));
                
                if (VOS_OK >= ret) {
                    json_object_put(one_permit_role);
                    continue;
                }

                json_object_object_add(one_permit_role, PROPERTY_LDAP_SERVICE_ODATA_ID,
                    json_object_new_string((const gchar*)permit_role_link));
                json_object_array_add(permit_role_arr, one_permit_role);
            }
        }

        json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE, permit_role_arr);
        
        (void)dal_get_property_value_uint32((OBJ_HANDLE)group_node->data, LDAP_GROUP_ATTRIBUTE_LOGIN_INTERFACE,
            &group_login_interface);
        login_interface_arr = json_object_new_array();

        for (loop_tag = 0; loop_tag < 3; loop_tag++) {
            if ((group_login_interface >> right_num[loop_tag]) & 1) {
                json_object_array_add(login_interface_arr, json_object_new_string(login_interface[loop_tag]));
            }
        }

        json_object_object_add(user_group_info, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE, login_interface_arr);

        
        json_object_array_add(*o_result_jso, user_group_info);
    }

    g_slist_free(group_list);

    return HTTP_OK;
}

LOCAL gint32 get_ldap_controller_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar rsc_uri[MAX_RSC_URI_LEN] = {0};

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (get_controller_id(i_paras->uri, &controller_id, &obj_handle) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // sizeof(rsc_uri)大小为256，远大于格式化后的字符串长度，无需判断返回值
    (void)snprintf_truncated_s(rsc_uri, sizeof(rsc_uri), "%s%u", PROPERTY_LDAP_SERVICE_COLLECTION_MEMBER,
        controller_id);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    rf_add_action_prop(*o_result_jso, (const gchar *)rsc_uri, PROPERTY_LDAP_SERVICE_IMPORT_CERT_METHOD);
    rf_add_action_prop(*o_result_jso, (const gchar *)rsc_uri, PROPERTY_LDAP_SERVICE_IMPORT_CRL_METHOD);
    rf_add_action_prop(*o_result_jso, (const gchar *)rsc_uri, PROPERTY_LDAP_SERVICE_DELETE_CRL_METHOD);

    return HTTP_OK;
}


void get_operation_result(gint32 ret, gboolean *is_success, gint32 *ret_code)
{
    if (HTTP_OK == ret) {
        *is_success = TRUE;
        *ret_code = HTTP_OK;
    } else if (TRUE == *is_success) {
        *ret_code = HTTP_OK;
    } else {
        *ret_code = ret;
    }
}

gint32 get_controller_id(const char *uri, guchar *controller_id, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    guchar one_id = 0;
    gboolean is_find = FALSE;
    gchar *controller_str = NULL;
    gchar one_id_str[ARRAY_LENTH] = {0};
    GSList *controller_list = NULL;
    GSList *controller_node = NULL;

    
    controller_str = g_strrstr(uri, "/");

    
    ret = dfl_get_object_list(LDAP_CLASS_NAME, &controller_list);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: Get controller list failed.", __FUNCTION__, __LINE__));

    
    for (controller_node = controller_list; controller_node; controller_node = controller_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)controller_node->data, LDAP_ATTRIBUTE_ID, &one_id);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s : dal_get_property_value_byte failed, ret = %d.\r\n", __FUNCTION__, ret);
            continue;
        }
        
        if (one_id < LOG_TYPE_KRB_SERVER) {
            (void)snprintf_s(one_id_str, sizeof(one_id_str), sizeof(one_id_str) - 1, "/%u", one_id);

            if (g_strcmp0(controller_str, one_id_str) == VOS_OK) {
                *controller_id = one_id;
                *obj_handle = (OBJ_HANDLE)controller_node->data;
                is_find = TRUE;
                break;
            }
        }
        
    }

    g_slist_free(controller_list);

    
    if (!is_find) {
        debug_log(DLOG_ERROR, "%s, %d: controller id(%d) error.", __FUNCTION__, __LINE__, *controller_id);
        return VOS_ERR;
    }

    return VOS_OK;
}


gint32 get_set_function_normal_ret(PROVIDER_PARAS_S *i_paras, gint32 ret, json_object **o_message_jso,
    const gchar *key_str, const gchar *value_str)
{
    gint32 ret_val;

    
    switch (ret) {
        case VOS_OK:
            ret_val = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE: // 无权限
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, value_str, o_message_jso, value_str);
            ret_val = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

        case RFERR_WRONG_PARAM:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            ret_val = RFERR_WRONG_PARAM;
            break;

        case PROPERTY_LDAP_RET_ERROR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, key_str, value_str);
            ret_val = RFERR_WRONG_PARAM;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret_val;
}


LOCAL gint32 set_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    const gchar *addr = NULL;
    GSList *input_list = NULL;
    gchar num[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    
    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_BAD_REQUEST;
    }

    
    addr = dal_json_object_get_str(i_paras->val_jso);
    
    if (PROPERTY_LDAP_SERVICE_DTR_LENTH < strlen(addr)) {
        (void)snprintf_s(num, sizeof(num), sizeof(num) - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, addr, PROPERTY_LDAP_SERVICE_ADDRESS,
            (const gchar*)num);
        return HTTP_BAD_REQUEST;
    }

    

    
    input_list = g_slist_append(input_list, g_variant_new_string(addr));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_HOST_ADDR, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    return get_set_function_normal_ret(i_paras, ret, o_message_jso, addr, "LdapServerAddress");
}

LOCAL gint32 set_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 controller_port;
    GSList *input_list = NULL;
    gchar port_str[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_BAD_REQUEST;
    }

    
    return_val_do_info_if_fail(json_type_int == json_object_get_type(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, "LdapPort", o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), "LdapPort"));
    
    controller_port = json_object_get_int(i_paras->val_jso);

    
    input_list = g_slist_append(input_list, g_variant_new_int32(controller_port));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_PORT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    (void)snprintf_s(port_str, sizeof(port_str), sizeof(port_str) - 1, "%d", controller_port);
    return get_set_function_normal_ret(i_paras, ret, o_message_jso, port_str, "LdapPort");
}

LOCAL gint32 set_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    const gchar *domain = NULL;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, "UserDomain", o_message_jso, "UserDomain"));
    

    
    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_BAD_REQUEST;
    }

    domain = dal_json_object_get_str(i_paras->val_jso);

    
    input_list = g_slist_append(input_list, g_variant_new_string(domain));

    
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SPLIT_USER_DOMAIN, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    
    uip_free_gvariant_list(input_list);

    return get_set_function_normal_ret(i_paras, ret, o_message_jso, domain, "UserDomain");
}


LOCAL gint32 set_controller_bind_dn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    const char *bind_dn = NULL;
    GSList *input_list = NULL;
    gchar num[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_BAD_REQUEST;
    }

    bind_dn = dal_json_object_get_str(i_paras->val_jso);
    if (strlen(bind_dn) > PROPERTY_LDAP_SERVICE_DTR_LENTH) {
        (void)snprintf_s(num, sizeof(num), sizeof(num) - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, "", PROPERTY_LDAP_SERVICE_BIND_DN,
            (const gchar*)num);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(bind_dn));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_BIND_DN, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return get_set_function_normal_ret(i_paras, ret, o_message_jso, bind_dn, PROPERTY_LDAP_SERVICE_BIND_DN);
}


LOCAL gint32 set_controller_bind_pwd(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    const char *bind_pwd = NULL;
    GSList *input_list = NULL;
    gchar num[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        json_string_clear(i_paras->val_jso);
        return HTTP_BAD_REQUEST;
    }

    bind_pwd = dal_json_object_get_str(i_paras->val_jso);
    if (strlen(bind_pwd) > USER_PASSWORD_MAX_LEN) {
        (void)snprintf_s(num, sizeof(num), sizeof(num) - 1, "%d", USER_PASSWORD_MAX_LEN);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, "", PROPERTY_LDAP_SERVICE_BIND_PWD,
            (const gchar*)num);
        json_string_clear(i_paras->val_jso);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(bind_pwd));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_BIND_DN_PSW, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    json_string_clear(i_paras->val_jso);
    return get_set_function_normal_ret(i_paras, ret, o_message_jso, "", PROPERTY_LDAP_SERVICE_BIND_PWD);
}


LOCAL gint32 set_controller_certificate_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar member_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gboolean cert_enable;
    GSList *input_list = NULL;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    if (get_controller_id(i_paras->uri, &member_id, &obj_handle) != VOS_OK) {
        (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso, (const gchar*)(g_strrstr(i_paras->uri, "/")));
        return HTTP_BAD_REQUEST;
    }

    
    cert_enable = json_object_get_boolean(i_paras->val_jso);

    
    input_list = g_slist_append(input_list, g_variant_new_byte((TRUE == cert_enable) ? 1 : 0));

    
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_CERTSTATUS, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    
    uip_free_gvariant_list(input_list);

    return get_set_function_normal_ret(i_paras, ret, o_message_jso, ((cert_enable) ? "true" : "false"),
        "CertificateVerificationEnabled");
}


LOCAL gint32 del_group_info(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE group_handle, json_object **o_message_jso,
    guchar group_id)
{
    gint32 ret;
    json_object *message_temp_jso = NULL;
    gchar message_str[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, group_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_DEL_GROUP, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);

    (void)snprintf_s(message_str, sizeof(message_str), sizeof(message_str) - 1, "LdapGroups/%u", group_id);
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, NULL);
}

LOCAL void set_controller_groupname_domain(json_object *one_group, guchar group_id, OBJ_HANDLE obj_group_handle,
    gboolean *del_group_flag, gint32 *ret_code, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *val_json_in = NULL;
    gboolean is_success = FALSE;
    gchar message_str[ARRAY_LENTH] = {0};
    gint32 checkfolder_ret;
    gint32 ret = 0;
    json_object *message_temp_jso = NULL;

    return_do_info_if_fail(NULL != one_group && NULL != del_group_flag && NULL != ret_code,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    
    gint32 check_ret = rf_check_domain_name(one_group);
    if (VOS_OK != check_ret) {
        // sizeof(message_str)大小为128，远大于格式化字符串长度，无需判断返回值
        (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id,
            PROPERTY_LDAP_SERVICE_GROUP_NAME);
        *ret_code = get_set_function_complex_ret(i_paras, check_ret, o_message_jso, message_str, "CN");
    }

    
    checkfolder_ret = rf_check_domain_folder(one_group);
    if (VOS_OK != checkfolder_ret) {
        (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id,
            PROPERTY_LDAP_SERVICE_GROUP_FOLDER);
        *ret_code = get_set_function_complex_ret(i_paras, checkfolder_ret, o_message_jso, message_str, "OU");
    }

    (void)memset_s(message_str, ARRAY_LENTH, 0, ARRAY_LENTH);
    check_ret = check_ret + checkfolder_ret;
    
    if (TRUE == json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_NAME, &val_json_in) &&
        VOS_OK == check_ret) {
        
        if (NULL != val_json_in) {
            ret = set_controller_group_user_name(i_paras, o_message_jso, o_result_jso, val_json_in, obj_group_handle,
                group_id);
            get_operation_result(ret, &is_success, ret_code);
        } else {
            ret = del_group_info(i_paras, obj_group_handle, o_message_jso, group_id);
            get_operation_result(ret, &is_success, ret_code);
            *del_group_flag = TRUE;
            return;
        }
    }
    
    
    if (TRUE == json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_FOLDER, &val_json_in) &&
        VOS_OK == check_ret) {
        if (val_json_in != NULL) {
            ret = set_controller_group_user_folder(i_paras, o_message_jso, o_result_jso, val_json_in, obj_group_handle,
                group_id);
            get_operation_result(ret, &is_success, ret_code);
        } else {
            // sizeof(message_str)大小为128，大于格式化后字符串长度，无需判断返回值
            (void)snprintf_s(message_str, sizeof(message_str), sizeof(message_str) - 1, "LdapGroups/%u/GroupFolder",
                group_id);
            (void)create_message_info(MSGID_PROP_TYPE_ERR, (const gchar*)message_str, &message_temp_jso, 
                RF_VALUE_NULL, (const gchar*)message_str);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            *ret_code = HTTP_BAD_REQUEST;
        }
    }
    
    if (TRUE == json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, &val_json_in) &&
        VOS_OK == check_ret) {
        ret = set_controller_group_user_domain(i_paras, o_message_jso, o_result_jso, val_json_in, obj_group_handle,
            group_id);
        get_operation_result(ret, &is_success, ret_code);
    }

    return;
}


LOCAL gint32 set_controller_groups(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_json_in = NULL;
    json_object *one_group = NULL;
    gint32 ret_code = HTTP_BAD_REQUEST;
    gint32 group_count;
    gboolean is_success = FALSE;
    gint32 group_id;
    guchar group_server_id = 0;
    OBJ_HANDLE obj_group_handle = 0;
    GSList *group_list = NULL;
    GSList *group_node = NULL;
    guchar get_group_id = 0;
    gboolean is_find = 0;
    json_object *message_temp_jso = NULL;
    gchar controller_str[ARRAY_LENTH] = {0};
    gchar group_str[ARRAY_LENTH] = {0};
    gboolean del_group_flag = FALSE;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_expr(get_controller_id(i_paras->uri, &controller_id, &obj_handle) != VOS_OK,
        HTTP_BAD_REQUEST, (void)create_message_info(MSGID_INVALID_IDX, NULL, o_message_jso,
        (const gchar*)(g_strrstr(i_paras->uri, "/"))));

    
    gint32 ret = dfl_get_object_list(LDAP_GROUP_CLASS_NAME, &group_list);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        g_slist_free(group_list); debug_log(DLOG_ERROR, "%s, %d: new array failed.", __FUNCTION__, __LINE__));

    
    
    group_count = json_object_array_length(i_paras->val_jso);
    if (MAX_GROUP_COUNT < group_count) { // 当前是5个用户组，如果设置超过5个，则报错
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, PROPERTY_LDAP_SERVICE_LDAPGROUPS, &message_temp_jso,
            PROPERTY_LDAP_SERVICE_LDAPGROUPS);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        g_slist_free(group_list);
        return HTTP_BAD_REQUEST;
    }

    for (group_id = 0; group_id < group_count; group_id++) {
        one_group = json_object_array_get_idx(i_paras->val_jso, group_id);
        
        continue_do_info_if_expr(NULL == one_group,
            debug_log(DLOG_DEBUG, "%s, %d: Group NULL.", __FUNCTION__, __LINE__));

        
        for (group_node = group_list; group_node; group_node = group_node->next) {
            is_find = FALSE;
            obj_group_handle = (OBJ_HANDLE)group_node->data;
            
            ret = dal_get_property_value_byte(obj_group_handle, LDAP_GROUP_ATTRIBUTE_LDAPSERVER_ID, &group_server_id);

            continue_if_expr(VOS_OK != ret);
            
            
            continue_if_expr(((group_server_id & 0x0f) >= LOG_TYPE_KRB_SERVER) ||
                (0 != (group_server_id - controller_id) % 16));
            
            ret = dal_get_property_value_byte(obj_group_handle, LDAP_GROUP_ATTRIBUTE_GROUP_ID, &get_group_id);
            break_do_info_if_expr(VOS_OK == ret && group_id == get_group_id, (is_find = TRUE));
            
        }

        if (!is_find) {
            ret = snprintf_s(controller_str, sizeof(controller_str), sizeof(controller_str) - 1, "%u", controller_id);
            if (VOS_OK < ret) {
                ret = snprintf_s(group_str, sizeof(group_str), sizeof(group_str) - 1, "%d", group_id);
                if (VOS_OK < ret) {
                    (void)create_message_info(MSGID_LDAP_MISMATCH_GROUP_AND_CONTOLLER, NULL, &message_temp_jso,
                        (const gchar*)group_str, (const gchar*)controller_str);
                    (void)json_object_array_add(*o_message_jso, message_temp_jso);
                    g_slist_free(group_list);
                    return HTTP_BAD_REQUEST;
                }
            }

            g_slist_free(group_list);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        
        del_group_flag = FALSE;
        set_controller_groupname_domain(one_group, group_id, obj_group_handle, &del_group_flag, &ret_code, i_paras,
            o_message_jso, o_result_jso);
        continue_if_expr(TRUE == del_group_flag);
        

        
        if (TRUE == json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE, &val_json_in)) {
            ret = set_controller_group_user_role(i_paras, o_message_jso, o_result_jso, val_json_in, obj_group_handle,
                group_id);
            get_operation_result(ret, &is_success, &ret_code);
        }

        
        if (TRUE == json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE, &val_json_in)) {
            ret = set_controller_group_user_permit_role(i_paras, o_message_jso, o_result_jso, val_json_in,
                obj_group_handle, group_id);
            get_operation_result(ret, &is_success, &ret_code);
        }

        
        if (TRUE == json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE, &val_json_in)) {
            ret = set_controller_group_user_login_interface(i_paras, o_message_jso, o_result_jso, val_json_in,
                obj_group_handle, group_id);
            get_operation_result(ret, &is_success, &ret_code);
        }
    }

    g_slist_free(group_list);

    
    if (TRUE == check_array_object(i_paras->val_jso)) {
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, PROPERTY_LDAP_SERVICE_LDAPGROUPS, &message_temp_jso,
            PROPERTY_LDAP_SERVICE_LDAPGROUPS);
        ret = json_object_array_add(*o_message_jso, message_temp_jso);
        do_val_if_expr(VOS_OK != ret, json_object_put(message_temp_jso));
        return HTTP_BAD_REQUEST;
    }

    return ret_code;
}

gint32 get_set_function_complex_ret(PROVIDER_PARAS_S *i_paras, gint32 ret, json_object **o_message_jso,
    const gchar *key_str, const gchar *value_str)
{
    gint32 ret_val;
    json_object *message_temp_jso = NULL;
    gchar num[ARRAY_LENTH] = {0};

    
    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: // 无权限
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, key_str, &message_temp_jso, key_str);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = RFERR_INSUFFICIENT_PRIVILEGE;
            break;

        case RFERR_WRONG_PARAM:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = RFERR_WRONG_PARAM;
            break;

        
        case PROPERTY_LDAP_RET_ERROR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, key_str, &message_temp_jso, value_str, key_str);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = RFERR_WRONG_PARAM;
            break;
        
        case PROPERTY_LDAP_LENGTH_ERROR:
            (void)snprintf_s(num, sizeof(num), sizeof(num) - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
            (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, key_str, &message_temp_jso, 
                value_str, key_str, (const gchar*)num);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = HTTP_BAD_REQUEST;
            break;
        case PROPERTY_LDAP_NO_MATCH_ERROR:
            (void)create_message_info(MSGID_LDAP_NO_MATCH, key_str, &message_temp_jso, value_str, key_str);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = HTTP_BAD_REQUEST;
            break;
        
        
        case PROPERTY_LDAP_FOLDER_MISMATCH:
            (void)create_message_info(MSGID_PROP_VALUE_MISMATCH, key_str, &message_temp_jso, value_str,
                PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, key_str);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret_val;
}


LOCAL gint32 rf_check_domain_name(json_object *one_group)
{
    errno_t safe_fun_ret;
    gchar **str_arr = NULL;
    gchar group_domain[PROPERTY_LDAP_SERVICE_DTR_LENTH+1] = {0};
    const gchar *name_val = NULL;
    json_object *name_json = NULL;
    json_object *domain_json = NULL;
    const gchar *domain_val = NULL;
    gboolean name_flag;
    gboolean domain_flag;

    
    return_val_do_info_if_fail(NULL != one_group, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:  Input Parameters NULL.\n", __FUNCTION__));
    
    name_flag = json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_NAME, &name_json);
    domain_flag = json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, &domain_json);
    
    
    return_val_do_info_if_expr(FALSE == domain_flag || FALSE == name_flag || NULL == name_json, VOS_OK,
        debug_log(DLOG_DEBUG, "%s:  domain_json is null,name_flag=%d.domain_flag= %d\n", __FUNCTION__, name_flag,
        domain_flag));
    
    domain_val = dal_json_object_get_str(domain_json);
    name_val = dal_json_object_get_str(name_json);

    
    safe_fun_ret = strncpy_s(group_domain, sizeof(group_domain), domain_val, sizeof(group_domain) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    str_arr = g_strsplit(group_domain, DOMAIN_STR_SPLIT_FLAG, 2);
    return_val_if_fail(NULL != str_arr, VOS_ERR);
    
    if ((NULL == str_arr[0]) || (strlen(str_arr[0]) < DOMAIN_STR_CN_OFFSET)) {
        debug_log(DLOG_ERROR, "%s:group_domain_cn is invalid", __FUNCTION__);
        g_strfreev(str_arr);
        return PROPERTY_LDAP_NO_MATCH_ERROR;
    }
    
    return_val_do_info_if_fail(!g_ascii_strcasecmp(str_arr[0] + DOMAIN_STR_CN_OFFSET, name_val),
        PROPERTY_LDAP_NO_MATCH_ERROR, debug_log(DLOG_ERROR, "%s:group_domain_cn=%s", __FUNCTION__, str_arr[0]);
        g_strfreev(str_arr));

    g_strfreev(str_arr);
    return VOS_OK;
}


LOCAL gint32 rf_check_domain_folder(json_object *one_group)
{
    errno_t safe_fun_ret;
    gchar **str_arr = NULL;
    gchar group_domain[PROPERTY_LDAP_SERVICE_DTR_LENTH+1] = {0};
    const gchar *folder_val = NULL;
    json_object *folder_json = NULL;
    json_object *domain_json = NULL;
    const gchar *domain_val = NULL;
    gboolean folder_flag;
    gboolean domain_flag;
    gchar *pBegin = NULL;

    
    return_val_do_info_if_fail(NULL != one_group, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:  Input Parameters NULL.\n", __FUNCTION__));

    folder_flag = json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_FOLDER, &folder_json);
    domain_flag = json_object_object_get_ex(one_group, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, &domain_json);
    
    return_val_do_info_if_expr(
        FALSE == domain_flag || FALSE == folder_flag || NULL == domain_json || NULL == folder_json, VOS_OK,
        debug_log(DLOG_DEBUG, "%s:  domain_json is null,folder_flag=%d.domain_flag= %d\n", __FUNCTION__, folder_flag,
        domain_flag));

    domain_val = dal_json_object_get_str(domain_json);
    folder_val = dal_json_object_get_str(folder_json);

    
    pBegin = g_strrstr(domain_val, "OU=");
    if (pBegin == NULL) {
        debug_log(DLOG_ERROR, "%s:No OU= in domain val", __FUNCTION__);
        return PROPERTY_LDAP_FOLDER_MISMATCH;
    }
    safe_fun_ret = strncpy_s(group_domain, sizeof(group_domain), pBegin, sizeof(group_domain) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    str_arr = g_strsplit(group_domain, DOMAIN_STR_SPLIT_FLAG, 2);
    return_val_if_fail(NULL != str_arr, VOS_ERR);
    
    if ((NULL == str_arr[0]) || (strlen(str_arr[0]) < DOMAIN_STR_CN_OFFSET)) {
        debug_log(DLOG_ERROR, "%s:group_domain_ou is invalid", __FUNCTION__);
        g_strfreev(str_arr);
        return PROPERTY_LDAP_FOLDER_MISMATCH;
    }
    
    return_val_do_info_if_fail(!g_ascii_strcasecmp(str_arr[0] + DOMAIN_STR_CN_OFFSET, folder_val),
        PROPERTY_LDAP_FOLDER_MISMATCH, debug_log(DLOG_ERROR, "%s:group_domain_ou=%s", __FUNCTION__, str_arr[0]);
        g_strfreev(str_arr));

    g_strfreev(str_arr);
    return VOS_OK;
}


LOCAL gint32 set_controller_group_user_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar message_str[ARRAY_LENTH] = {0};
    gchar num[ARRAY_LENTH] = {0};
    gchar message[ARRAY_LENTH] = {0};
    json_object *message_temp_jso = NULL;
    
    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != *o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    const gchar* group_name = dal_json_object_get_str(val_json_in);
    
    if (PROPERTY_LDAP_SERVICE_DTR_LENTH < strlen(group_name)) {
        (void)snprintf_s(num, sizeof(num), sizeof(num) - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
        (void)snprintf_truncated_s(message, sizeof(message), "LdapGroups/%u/%s", group_id,
            PROPERTY_LDAP_SERVICE_GROUP_NAME);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, &message_temp_jso, group_name, 
            (const gchar*)message, (const gchar*)num);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }
    
    

    
    input_list = g_slist_append(input_list, g_variant_new_string(group_name));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_NAME, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    // sizeof(message_str)大小为128，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id, "GroupName");
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, group_name);
}


LOCAL gint32 set_controller_group_user_folder(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar message_str[ARRAY_LENTH] = {0};
    gchar num[ARRAY_LENTH] = {0};
    json_object *message_temp_jso = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != *o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    // sizeof(message_str)大小为128，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id,
        PROPERTY_LDAP_SERVICE_GROUP_FOLDER);

    
    const gchar *group_folder = dal_json_object_get_str(val_json_in);
    if (PROPERTY_LDAP_SERVICE_DTR_LENTH < strlen(group_folder)) {
        (void)snprintf_s(num, sizeof(num), sizeof(num) - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, &message_temp_jso, group_folder, 
            (const gchar*)message_str, (const gchar*)num);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(group_folder));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_FOLDER, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, group_folder);
}


LOCAL gint32 set_controller_group_user_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar message_str[ARRAY_LENTH] = {0};
    json_object *message_temp_jso = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != *o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    
    // sizeof(message_str)大小为128，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id, "GroupDomain");
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar*)message_str, &message_temp_jso, 
            (const gchar*)message_str);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    
    
    
    const gchar *group_domain = dal_json_object_get_str(val_json_in);
    
    

    
    

    
    input_list = g_slist_append(input_list, g_variant_new_string(group_domain));

    
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_KRB_GROUP_METHOD_SET_GROUP_INFO, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    

    uip_free_gvariant_list(input_list);

    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, group_domain);
}

LOCAL gint32 set_controller_group_user_role(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id)
{
    gint32 ret;
    gint32 loop_tag;
    guchar user_role = 0;
    const gchar *user_role_str = NULL;
    GSList *input_list_p = NULL;
    GSList *input_list_u = NULL;
    guchar group_p_id = 0;
    gchar message_str[ARRAY_LENTH] = {0};
    json_object *message_temp_jso = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != *o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    user_role_str = dal_json_object_get_str(val_json_in);

    
    for (loop_tag = 0; loop_tag < 8; loop_tag++) {
        if (VOS_OK == g_strcmp0(priv_arr_ldap[loop_tag].priv_str, user_role_str)) {
            user_role = priv_arr_ldap[loop_tag].priv_num;
            break;
        }
    }

    ret = dal_get_property_value_byte(obj_handle, LDAP_GROUP_ATTRIBUTE_GROUP_PRIVILEGE, &group_p_id);

    // 先设置privilage 将roleid传入并在ldap设置方法内转换为privilege
    input_list_p = g_slist_append(input_list_p, g_variant_new_byte(user_role));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_PRIVILEGE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list_p, NULL);
    debug_log(DLOG_INFO, "call LDAP_GROUP_METHOD_SET_GROUP_PRIVILEGE_NEW ret = %d", ret);
    uip_free_gvariant_list(input_list_p);

     // 设置完privilage才能设置用户权限位
    input_list_u = g_slist_append(input_list_u, g_variant_new_byte(user_role));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_USERROLEID, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list_u, NULL);
    uip_free_gvariant_list(input_list_u);
    // sizeof(message_str)大小为128，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id, "GroupRole");
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, user_role_str);
}

LOCAL gint32 set_controller_group_user_permit_role(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id)
{
    gint32 ret;
    gint32 loop_tag_i;
    guint32 loop_tag_j = 0;
    guchar permit_role = 0;
    GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    json_object *one_role = NULL;
    const gchar *one_role_str = NULL;
    const gchar* permit_role_map[] = {"Rule1", "Rule2", "Rule3"}; // 去重
    gint32 int_arr[3] = {0};
    gchar name_parameter[ARRAY_LENTH] = {0};
    gchar message_str[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != *o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));

    
    gint32 array_count = json_object_array_length(val_json_in);
    
    if (3 < array_count) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE,
            &message_temp_jso, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag_i = 0; loop_tag_i < array_count; loop_tag_i++) {
        one_role = json_object_array_get_idx(val_json_in, loop_tag_i);
        one_role_str = dal_json_object_get_str(one_role);

        for (loop_tag_j = 0; loop_tag_j < 3; loop_tag_j++) {
            if (VOS_OK == g_strcmp0(permit_role_map[loop_tag_j], one_role_str)) {
                
                if (int_arr[loop_tag_j]) {
                    // break;  
                    
                    (void)snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1,
                        "LdapGroups/%u/GroupLoginRule/%d", group_id, loop_tag_i);
                    (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, NULL, &message_temp_jso, 
                        (const gchar*)name_parameter);
                    (void)json_object_array_add(*o_message_jso, message_temp_jso);
                    return HTTP_BAD_REQUEST;
                }

                permit_role += (1 << loop_tag_j);
                int_arr[loop_tag_j]++;
            }
        }
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(permit_role));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_PERMIT_ID, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    // sizeof(message_str)大小为128，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_truncated_s(message_str, sizeof(message_str), "LdapGroups/%u/%s", group_id, "GroupLoginRule");
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, NULL);
    
}

LOCAL gint32 set_controller_group_user_login_interface(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, json_object *val_json_in, OBJ_HANDLE obj_handle, guchar group_id)
{
    int iRet;
#define ARRAY_COUNT_MIN_VALUE 3
    gint32 ret;
    gint32 loop_tag_i;
    gint32 loop_tag_j = 0;
    guint32 login_interface = 0;
    GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    json_object *one_interface = NULL;
    const gchar *one_interface_str = NULL;
    const gchar* login_interface_map[] = {"Web", "SSH", "Redfish"};
    gint32 int_arr[3] = {0};
    const guint32 login_interface_map_num[] = {0, 3, 7};
    gchar name_parameter[ARRAY_LENTH] = {0};
    gchar message_str[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != *o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso));
    gint32 array_count = json_object_array_length(val_json_in);
    
    if (ARRAY_COUNT_MIN_VALUE < array_count) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE,
            &message_temp_jso, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag_i = 0; loop_tag_i < array_count; loop_tag_i++) {
        one_interface = json_object_array_get_idx(val_json_in, loop_tag_i);
        one_interface_str = dal_json_object_get_str(one_interface);

        for (loop_tag_j = 0; loop_tag_j < 3; loop_tag_j++) {
            if (VOS_OK == g_strcmp0(login_interface_map[loop_tag_j], one_interface_str)) {
                
                if (int_arr[loop_tag_j]) {
                    // break;  
                    
                    (void)snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1,
                        "LdapGroups/%u/GroupLoginInterface/%d", group_id, loop_tag_i);
                    (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, NULL, &message_temp_jso, 
                        (const gchar*)name_parameter);
                    (void)json_object_array_add(*o_message_jso, message_temp_jso);
                    return HTTP_BAD_REQUEST;
                }

                login_interface += (1 << login_interface_map_num[loop_tag_j]);
                int_arr[loop_tag_j]++;
            }
        }
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(login_interface));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_LOGIN_INTERFACE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    iRet = snprintf_s(message_str, sizeof(message_str), sizeof(message_str) - 1, "LdapGroups/%u/%s", group_id,
        "GroupLoginInterface");
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, message_str, NULL);
    
}

LOCAL gint32 act_import_ldap_certificate_to_server(json_object *body_jso, json_object *user_data,
    json_object **message_obj, OBJ_HANDLE ldap_obj)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = ldap_obj;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    const gchar *import_type = NULL;
    const gchar *import_uri = NULL;
    gchar import_path[ARRAY_LENTH * 2] = {0};
    gchar *file_name = NULL;
    gint32 controller_id = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 out_ret;
    glong file_lenth;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    
    errno_t ret_val = EOK;

    
    (void)get_element_str(user_data, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str(user_data, RF_LOG_USER_IP, &client);
    (void)get_element_int(user_data, RF_USER_PRIV, &user_priv);
    (void)get_element_int(user_data, "ControllerId", &controller_id);
    
    (void)get_element_int(user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);
    

    (void)get_element_str(body_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CERTIFICATE_TYPE, &import_type);
    
    return_val_do_info_if_fail(NULL != import_type, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MISSING, PROPERTY_ACCOUNT_SERVICE_ROOT_CERTIFICATE_TYPE, message_obj, 
            PROPERTY_ACCOUNT_SERVICE_ROOT_CERTIFICATE_TYPE);
        debug_log(DLOG_ERROR, "get json content failed"));
    

    
    
    if (VOS_OK == g_strcmp0(PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI, import_type)) {
        
        (void)get_element_str(body_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT, &import_uri);
        
        return_val_do_info_if_fail(NULL != import_uri, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_MISSING, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT, message_obj, 
                PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT));
        

        
        if ('/' == import_uri[0]) {
            
            ret = dal_check_real_path(import_uri);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s :Cert file real_path is illegal! return %d", __FUNCTION__, ret);
                (void)create_message_info(MSGID_INVALID_PATH, NULL, message_obj, 
                    RF_SENSITIVE_INFO, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT);
                return HTTP_BAD_REQUEST;
            }

            

            ret_val = strncpy_s(import_path, sizeof(import_path), import_uri, strlen(import_uri));
            do_if_expr(EOK != ret_val, debug_log(DLOG_ERROR, "%s: strncpy_s %s failed", __FUNCTION__, import_uri));
        }
        
        else {
            file_name = g_strrstr(import_uri, "/");
            ret = snprintf_s(import_path, sizeof(import_path), sizeof(import_path) - 1, "/tmp%s", file_name);
            do_val_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "snprintf_s failed"));
        }

        
        // 源文件和临时文件的路径和文件名不能相同
        if (0 != g_strcmp0(LDAP_RS_CACERT_TMP_PATH, import_path)) {
            // redfish模块降权限后，调用代理方法拷贝文件
            ret = proxy_copy_file(from_webui_flag, user_name, client, import_path, LDAP_RS_CACERT_TMP_PATH,
                REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
            if (VOS_OK != ret) {
                debug_log(DLOG_ERROR, "%s: copy request file failed", __FUNCTION__);
                (void)proxy_delete_file(from_webui_flag, user_name, client, import_path);
                (void)proxy_delete_file(from_webui_flag, user_name, client, LDAP_RS_CACERT_TMP_PATH);
                (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_LDAP_SERVICE_CERT_IMPORT, message_obj);
                return HTTP_BAD_REQUEST;
            }
        }
    }

    
    
    ret = dal_check_real_path(LDAP_RS_CACERT_TMP_PATH);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "import path is invalid, return %d", ret);
        (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, message_obj));

    

    
    
    (void)dal_set_file_owner(LDAP_RS_CACERT_TMP_PATH, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(LDAP_RS_CACERT_TMP_PATH, (S_IRUSR | S_IWUSR));
    

    
    
    file_lenth = vos_get_file_length(LDAP_RS_CACERT_TMP_PATH);
    input_list = g_slist_append(input_list, g_variant_new_int32((gint32)file_lenth));
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)controller_id));
    input_list = g_slist_append(input_list, g_variant_new_string(LDAP_RS_CACERT_TMP_PATH));
    
    
    ret = uip_call_class_method_redfish((gchar)from_webui_flag, user_name, client, obj_handle, LDAP_CLASS_NAME,
        LDAP_METHOD_CERTUPLOAD, AUTH_ENABLE, user_priv, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    

    
    json_object_clear_string(body_jso, RFACTION_PARAM_CERTICATE);

    out_ret = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);
    if (VOS_OK == ret) {
        switch (out_ret) {
            case PROPERTY_LDAP_CACERT_UPLOAD_SUCCESS:
                ret = HTTP_OK;
                (void)create_message_info(MSGID_LDAP_CERT_IMPORT_SUCCESS, NULL, message_obj);
                break;

                
            case PROPERTY_LDAP_CACERT_UPLOAD_FAILED:
                ret = HTTP_BAD_REQUEST;
                
                (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, message_obj);
                
                break;

                
            case RFERR_INSUFFICIENT_PRIVILEGE:
                (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, message_obj);
                ret = RFERR_INSUFFICIENT_PRIVILEGE;
                break;

            
            case LDAP_CACERT_OVERDUED:
                (void)create_message_info(MSGID_CERT_EXPIRED, NULL, message_obj);
                ret = HTTP_BAD_REQUEST;
                break;
                

            
            case PROPERTY_LDAP_CACERT_TOO_BIG:
            case PROPERTY_LDAP_CACERT_FORMAT_FAILED:
                ret = HTTP_BAD_REQUEST;
                (void)create_message_info(MSGID_CERTIFICATE_FORMAT_ERR, NULL, message_obj);
                break;

                
            default:
                ret = HTTP_INTERNAL_SERVER_ERROR;
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
                debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
                break;
        }
    } else {
        if (ret == RFERR_INSUFFICIENT_PRIVILEGE) {
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, message_obj);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
        } else {
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        }
    }

    return ret;
}

LOCAL TASK_MONITOR_INFO_S *ldap_cert_import_info_new(guchar from_webui_flag, const gchar *user_name,
    const gchar *client, gint32 user_priv, OBJ_HANDLE obj_handle, guchar controller_id)
{
    TASK_MONITOR_INFO_S *monitor_info;
    json_object *user_data_jso = NULL;

    monitor_info = task_monitor_info_new((GDestroyNotify)cert_import_info_free_func);
    
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

    
    json_object_object_add(user_data_jso, RF_LOG_USER_NAME, json_object_new_string(user_name));
    json_object_object_add(user_data_jso, RF_LOG_USER_IP, json_object_new_string(client));
    json_object_object_add(user_data_jso, RF_USER_PRIV, json_object_new_int(user_priv));
    json_object_object_add(user_data_jso, "ControllerId", json_object_new_int((gint32)controller_id));
    
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG, json_object_new_int((gint32)from_webui_flag));
    

    monitor_info->user_data = user_data_jso;
    monitor_info->ldap_obj_handle = obj_handle;

    return monitor_info;
}

LOCAL gint32 ldap_cert_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 file_down_progress = 0;

    return_val_do_info_if_fail((NULL != origin_obj_path) && (NULL != monitor_fn_data) && (NULL != message_obj),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    ret = get_file_transfer_progress(&file_down_progress);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "get upgrade packege download progress failed"));

    
    ret = parse_file_transfer_err_code(file_down_progress, message_obj);
    return_val_do_info_if_fail(RF_OK == ret, RF_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        
        monitor_fn_data->task_progress = 0;
        
        debug_log(DLOG_ERROR, "file transfer error occured, error code:%d", file_down_progress));

    debug_log(DLOG_DEBUG, "----------file down progress :%d---------", file_down_progress);

    if (RF_FINISH_PERCENTAGE != file_down_progress) {
        monitor_fn_data->task_progress = (guchar)(file_down_progress * 0.1);
    } else {
        ret = act_import_ldap_certificate_to_server(body_jso, (json_object *)monitor_fn_data->user_data, message_obj,
            monitor_fn_data->ldap_obj_handle);
        if (HTTP_OK == ret) {
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = 100;
        } else {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            monitor_fn_data->task_progress = 10;
            debug_log(DLOG_ERROR, "Import cert from /tmp dictionary to server failed. ret = %d", ret);
        }
    }

    return RF_OK;

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_OK; 
}


LOCAL gint32 act_import_ldap_certificate_uri(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar *uri, OBJ_HANDLE obj_controller_handle, guchar controller_id)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *cert_import_data = NULL;
    GSList *input_list = NULL;
    json_object *body_jso = NULL;
    OBJ_HANDLE obj_handle;
    gchar uri_out[ARRAY_LENTH * 8] = {0};

    
    ret = dfl_get_object_handle(OBJ_NAME_TRANSFERFILE, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        (void)parse_file_transfer_err_code(UNKNOWN_ERROR, o_message_jso));

    
    ret = snprintf_s(uri_out, sizeof(uri_out), sizeof(uri_out) - 1, "download;%d;%s", 0x12, uri);
    do_val_if_fail(VOS_OK < ret, debug_log(DLOG_ERROR, "%s, %d: snprintf_s failed.", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)uri_out));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));
    ret = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        (i_paras->auth_type == LOG_TYPE_LOCAL), i_paras->user_roleid, obj_handle, CLASS_NAME_TRANSFERFILE,
        METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s failed:initial file transfer error, ret is %d", __FUNCTION__, ret);
        clear_sensitive_info(uri_out, sizeof(uri_out));
        (void)parse_file_transfer_err_code(ret, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    

    
    cert_import_data = ldap_cert_import_info_new(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->user_role_privilege, obj_controller_handle, controller_id);
    goto_label_do_info_if_fail(NULL != cert_import_data, err_exit,
        debug_log(DLOG_ERROR, "alloc a new monitor info failed"));
    

    cert_import_data->task_progress = TASK_NO_PROGRESS; 
    cert_import_data->rsc_privilege = USERROLE_USERMGNT;

    body_jso = i_paras->val_jso;
    ret = create_new_task(PROPERTY_LDAP_SERVICE_CERT_URI_IMPORT, ldap_cert_import_status_monitor, cert_import_data,
        body_jso, i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(cert_import_data);
        debug_log(DLOG_ERROR, "create new task failed"));
    clear_sensitive_info(uri_out, sizeof(uri_out));
    return HTTP_ACCEPTED;

err_exit:
    ret = HTTP_INTERNAL_SERVER_ERROR;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    clear_sensitive_info(uri_out, sizeof(uri_out));
    return ret;
}


gint32 import_ldap_cert_check_param(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guchar *controller_id, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    gchar *url_back = NULL;
    gchar url_front[MAX_URI_LENGTH] = {0};

    
    return_val_do_info_if_expr((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    url_back = strstr(i_paras->uri, "/Actions/");
    if (url_back == NULL) {
        goto ERR_EXIT;
    }

    ret = snprintf_s(url_front, sizeof(url_front), sizeof(url_front) - 1, "%s", i_paras->uri);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    url_front[strlen(i_paras->uri) - strlen(url_back)] = '\0';

    if (get_controller_id(url_front, controller_id, obj_handle) != VOS_OK) {
        goto ERR_EXIT;
    }

    return VOS_OK;

ERR_EXIT:
    (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
    return HTTP_NOT_FOUND;
}


LOCAL gint32 act_import_ldap_certificate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar controller_id = 0;
    const gchar *import_type = NULL;
    const gchar *cert_str = NULL;
    gint32 cert_file_len = 0;
    gboolean b_ret;
    OBJ_HANDLE obj_handle;
    json_object *user_data = NULL;
    gint32 ret;

    
    ret = import_ldap_cert_check_param(i_paras, o_message_jso, o_result_jso, &controller_id, &obj_handle);
    return_val_if_expr(ret != VOS_OK, ret);

    
    b_ret = check_redfish_running_task_exist(ldap_cert_import_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(b_ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_CERT_IMPORTING, NULL, o_message_jso));
    

    
    (void)get_element_str(i_paras->val_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT, &cert_str);
    goto_label_do_info_if_fail(cert_str != NULL, err_exit,
        debug_log(DLOG_ERROR, "cann't get %s from request.", RFACTION_PARAM_CERTICATE));

    
    (void)get_element_str(i_paras->val_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CERTIFICATE_TYPE, &import_type);

    
    
    if (g_strcmp0(import_type, PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_URI) == VOS_OK) {
        b_ret = g_regex_match_simple(LDAP_CERT_IMPORT_REGEX, cert_str, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
        return_val_do_info_if_fail(b_ret == TRUE, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT));

        if (cert_str[0] != '/') {
            return act_import_ldap_certificate_uri(i_paras, o_message_jso, o_result_jso, cert_str, obj_handle,
                controller_id);
        } else {
            
            ret = dal_check_real_path(cert_str);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s :Cert file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
                (void)create_message_info(MSGID_INVALID_PATH, NULL, o_message_jso, 
                    RF_SENSITIVE_INFO, RFACTION_PARAM_CONTENT);
                return HTTP_BAD_REQUEST;
            }

            if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, cert_str,
                (const gchar *)i_paras->user_roleid)) {
                (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
                return HTTP_BAD_REQUEST;
            }

            
        }
    } else if (g_strcmp0(import_type, PROPERTY_ACCOUNT_SERVICE_CERTIFICATE_TEXT) == VOS_OK) {
        
        cert_file_len = strlen(cert_str);
        goto_label_do_info_if_fail(cert_file_len <= MAX_SERVER_CERT_FILE_LEN, err_exit,
            debug_log(DLOG_ERROR, "cert file size exceed, size is %d.", cert_file_len));

        
        
        b_ret = g_file_set_contents(LDAP_RS_CACERT_TMP_PATH, cert_str, cert_file_len, NULL);
        goto_label_do_info_if_fail(b_ret, err_exit,
            debug_log(DLOG_ERROR, "save certificate info failed."));
    }

    

    
    
    (void)dal_set_file_owner(LDAP_RS_CACERT_TMP_PATH, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(LDAP_RS_CACERT_TMP_PATH, (S_IRUSR | S_IWUSR));
    
    user_data = json_object_new_object();
    goto_label_do_info_if_fail(NULL != user_data, err_exit, debug_log(DLOG_ERROR, "new user_data failed."));
    json_object_object_add(user_data, RF_LOG_USER_NAME, json_object_new_string(i_paras->user_name));
    json_object_object_add(user_data, RF_LOG_USER_IP, json_object_new_string(i_paras->client));
    json_object_object_add(user_data, RF_USER_PRIV, json_object_new_int(i_paras->user_role_privilege));
    json_object_object_add(user_data, "ControllerId", json_object_new_int((gint32)controller_id));
    
    json_object_object_add(user_data, RF_USERDATA_FROM_WEBUI_FLAG, json_object_new_int((gint32)i_paras->is_from_webui));
    

    
    ret = act_import_ldap_certificate_to_server(i_paras->val_jso, user_data, o_message_jso, obj_handle);
    json_object_put(user_data);
    return ret;
    
err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 act_import_ldap_certificat_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_import_ldap_certificate(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL && i_paras->val_jso != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT);
        json_object_clear_string(i_paras->val_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT);
    }
    return ret;
}


gint32 ldapservice_controller_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = 0;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar *url_back = NULL;
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    url_back = strstr(i_paras->uri, "/Actions/"); // action操作在方法中检测url是否合法
    if (NULL == url_back) {
        
        ret = get_controller_id(i_paras->uri, &controller_id, &obj_handle);
        return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    }

    *prop_provider = ldapservice_controller_provider;
    *count = sizeof(ldapservice_controller_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


LOCAL gint32 get_import_cert_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_ldap_controller_action_info_odata_id(i_paras, o_message_jso, o_result_jso,
        PROPERTY_LDAP_SERVICE_COLLECTION_MEMBER, PROPERTY_LDAP_SERVICE_IMPORT_CERT_ACTIONINFO);
}


LOCAL PROPERTY_PROVIDER_S g_ldapservice_controller_provider_import[] = {
    {
        PROPERTY_LDAP_SERVICE_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_import_cert_actioninfo_odata_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    }
};

gint32 get_ldap_controller_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar *actioninfo_uri_prifix, const gchar *actioninfo_uri_postfix)
{
    gint32 ret;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar uri_full_str[MAX_RSC_URI_LEN] = {0};
    gchar odata_str[MAX_RSC_URI_LEN] = {0};

    
    return_val_do_info_if_expr(
        (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = snprintf_s(uri_full_str, sizeof(uri_full_str), sizeof(uri_full_str) - 1, "%s", i_paras->uri);
    if (ret <= 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    uri_full_str[strlen(i_paras->uri) - strlen(actioninfo_uri_postfix)] = '\0';

    ret = get_controller_id(uri_full_str, &controller_id, &obj_handle);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(odata_str, sizeof(odata_str), sizeof(odata_str) - 1, "%s%u%s", actioninfo_uri_prifix,
        controller_id, actioninfo_uri_postfix);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail, ret:%d", __FUNCTION__, __LINE__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)odata_str);
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


gint32 ldapservice_controller_provider_import_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar controller_id = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar uri_full_str[ARRAY_LENTH] = {0};

    
    return_val_if_fail(i_paras != NULL && prop_provider != NULL && count != NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(uri_full_str, sizeof(uri_full_str), sizeof(uri_full_str) - 1, "%s", i_paras->uri);
    if (ret <= VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    uri_full_str[strlen(i_paras->uri) - strlen(PROPERTY_LDAP_SERVICE_IMPORT_CERT_ACTIONINFO)] = '\0';

    
    ret = get_controller_id(uri_full_str, &controller_id, &obj_handle);
    return_val_if_fail(ret == VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_ldapservice_controller_provider_import;
    *count = sizeof(g_ldapservice_controller_provider_import) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
