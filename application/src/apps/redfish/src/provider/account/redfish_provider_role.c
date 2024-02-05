

#include "redfish_provider.h"

LOCAL gint32 get_role_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_role_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_role_ispredefined(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_role_assignedprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_role_oemprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 set_role_assignedprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_role_oemprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_role_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_role_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_role_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ROLE_ISPREDEFINED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_role_ispredefined, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ROLE_ASSIGNEDPRIVILEGES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, get_role_assignedprivileges, set_role_assignedprivileges, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ROLE_OEMPRIVILEGES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, get_role_oemprivileges, set_role_oemprivileges, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 redfish_check_role_uri_effective(const gchar *i_member_id, const gchar *i_uri, guchar *o_roleid,
    OBJ_HANDLE *o_obj_handle, json_object **o_message_jso)
{
    gint32 ret;
    gint32 match_flag = 1;
    guchar roleid = 0;
    gchar role_str[USER_ROLEID_MAX_LEN + 1] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (NULL == i_member_id || NULL == i_uri || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_USERROLE, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(role_str, sizeof(role_str), 0, sizeof(role_str));

        
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_USERROLE_ROLENAME, role_str,
            USER_ROLEID_MAX_LEN + 1);
        match_flag = g_ascii_strncasecmp(i_member_id, role_str, USER_ROLEID_MAX_LEN + 1);
        break_if_fail(VOS_OK != match_flag);
    }

    
    return_val_do_info_if_fail(VOS_OK == match_flag, HTTP_NOT_FOUND, g_slist_free(obj_list);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_uri));
    do_info_if_true((NULL != o_obj_handle && NULL != obj_node), (*o_obj_handle = (OBJ_HANDLE)obj_node->data));

    do_info_if_true((NULL != o_roleid && NULL != obj_node),
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_USERROLE_ID, &roleid);
        *o_roleid = roleid);

    g_slist_free(obj_list);
    return VOS_OK;
}


gint32 redfish_role_comparison(const gchar *i_user_roleid, const guchar uri_roleid)
{
    gint32 i = 0;
    gint32 j = 0;
    gchar  user_roleid[USER_ROLEID_MAX_LEN + 1] = {0};
    gchar  uri_ascii_roleid[USER_ROLEID_MAX_LEN + 1] = {0};

    // 1. 入参检查,检查不过返回500，内部错误，不填充消息体
    if (NULL == i_user_roleid) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 2. 根据uri的roleid，匹配用户的roleid，失败则禁止访问
    (void)snprintf_s(uri_ascii_roleid, sizeof(uri_ascii_roleid), sizeof(uri_ascii_roleid) - 1, "%u", uri_roleid);

    while (i_user_roleid[i] != '\0' && j < USER_ROLEID_MAX_LEN) {
        if (i_user_roleid[i] == ',') {
            i++;
        }
        user_roleid[j++] = i_user_roleid[i++];

        if (i_user_roleid[i] == ',' || i_user_roleid[i] == '\0') {
            user_roleid[j] = '\0';
            return_val_if_expr(VOS_OK == g_strcmp0(uri_ascii_roleid, user_roleid), VOS_OK);
            (void)memset_s(user_roleid, sizeof(user_roleid), 0, sizeof(user_roleid));
            j = 0;
        }
    }

    return VOS_ERR;
}


LOCAL gint32 get_role_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    guchar roleid = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar rolename[USER_ROLEID_MAX_LEN + 1] = {0};
    gchar  uri[MAX_URI_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer, or input paras error.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, &obj_handle, o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = redfish_role_comparison(i_paras->user_roleid, roleid);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_USERROLE_ROLENAME, rolename, USER_ROLEID_MAX_LEN + 1);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, "%s/%s", ACCOUNTSERVICE_ROLES, rolename);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    *o_result_jso = json_object_new_string(uri);
    

    return HTTP_OK;
}


LOCAL gint32 get_role_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar roleid = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar rolename[USER_ROLEID_MAX_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer, or input paras incorrect.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, &obj_handle, o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    ret = dal_get_property_value_string(obj_handle, PROPERTY_USERROLE_ROLENAME, rolename, USER_ROLEID_MAX_LEN + 1);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);
    

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = redfish_role_comparison(i_paras->user_roleid, roleid);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    }

    
    *o_result_jso = json_object_new_string(rolename);

    return HTTP_OK;
}


LOCAL gint32 get_role_ispredefined(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar roleid = 0;
    json_bool flag = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, NULL, o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = redfish_role_comparison(i_paras->user_roleid, roleid);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    }

    
    do_info_if_true(((UUSER_ROLE_USER <= roleid && UUSER_ROLE_ADMIN >= roleid) || UUSER_ROLE_NOACCESS == roleid),
        (flag = 1));
    do_info_if_true(UUSER_ROLE_CUSt1 <= roleid && UUSER_ROLE_CUSt4 >= roleid, (flag = 0));
    *o_result_jso = json_object_new_boolean(flag);
    

    return HTTP_OK;
}


LOCAL gint32 get_role_assignedprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar roleid = 0;
    guchar privilege = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    
    str_map_s           privilege_map_info[] = {
        {PROPERTY_USERROLE_READONLY,                 RF_VALUE_LOGIN},
        {PROPERTY_USERROLE_USERMGNT,                 RF_VALUE_CONFIGURE_USERS},
        {PROPERTY_USERROLE_CONFIGURE_SELF,           RF_VALUE_CONFIGURE_SELF},
        {PROPERTY_USERROLE_BASICSETTING,             RF_VALUE_CONFIGURE_COMPONENTS}
    };
    guint32 i;
    guint32 map_info_len;
    const gchar *prop_name = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, &obj_handle, o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = redfish_role_comparison(i_paras->user_roleid, roleid);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    }

    
    *o_result_jso = json_object_new_array();

    map_info_len = (guint32)G_N_ELEMENTS(privilege_map_info);
    for (i = 0; i < map_info_len; i++) {
        prop_name = privilege_map_info[i].str1;
        ret = dal_get_property_value_byte(obj_handle, privilege_map_info[i].str1, &privilege);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
            debug_log(DLOG_ERROR, "get property %s from handle %s failed, ret is %d", prop_name,
            dfl_get_object_name(obj_handle), ret));

        continue_if_expr(DISABLED == privilege);

        obj_jso = json_object_new_string(privilege_map_info[i].str2);

        ret = json_object_array_add(*o_result_jso, obj_jso);
        return_val_do_info_if_fail(0 == ret, HTTP_OK,
            debug_log(DLOG_ERROR, "add item to json array failed, ret is %d", ret);
            (void)json_object_put(obj_jso));
    }

    

    return HTTP_OK;
}


LOCAL gint32 get_role_oemprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar roleid = 0;
    guchar privilege = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;
    guchar board_type = 0;
    
    str_map_s           privilege_map_info[] = {
        {PROPERTY_USERROLE_KVMMGNT,                  RF_VALUE_KVM},
        {PROPERTY_USERROLE_VMMMGNT,                  RF_VALUE_VMM},
        {PROPERTY_USERROLE_SECURITYMGNT,             RF_VALUE_SECURITY_SETTINGS},
        {PROPERTY_USERROLE_POWERMGNT,                RF_VALUE_POWER_CONTROL},
        {PROPERTY_USERROLE_DIAGNOSEMGNT,             RF_VALUE_DIAGNOSIS}
    };
    guint32 i;
    guint32 len;
    const gchar *prop_name = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras incorrect.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, &obj_handle, o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = redfish_role_comparison(i_paras->user_roleid, roleid);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    }

    
    *o_result_jso = json_object_new_array();

    
    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(BOARD_SWITCH != board_type, HTTP_OK);
    

    len = (guint32)G_N_ELEMENTS(privilege_map_info);
    for (i = 0; i < len; i++) {
        prop_name = privilege_map_info[i].str1;
        ret = dal_get_property_value_byte(obj_handle, prop_name, &privilege);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK,
            debug_log(DLOG_ERROR, "get property %s from handle %s failed, ret is %d", prop_name,
            dfl_get_object_name(obj_handle), ret));

        continue_if_expr(DISABLED == privilege);

        obj_jso = json_object_new_string(privilege_map_info[i].str2);
        ret = json_object_array_add(*o_result_jso, obj_jso);
        return_val_do_info_if_fail(0 == ret, HTTP_OK,
            debug_log(DLOG_ERROR, "add item to json array failed, ret is %d", ret));
    }

    

    
    return HTTP_OK;
}


LOCAL gint32 _role_priv_config_precheck(gboolean check_oem_priv, gint32 role_id, json_object *priv_array_jso,
    json_object **o_message_jso)
{
    gint32 duplicate_index;
    gint32 ret;
    gchar               prop_path[MAX_CHARACTER_NUM] = {0};
    const gchar *priv_prop_name = NULL;
    gboolean b_ret;
    json_object *tmp_jso = NULL;

    
    
    priv_prop_name = (TRUE == check_oem_priv) ? RFPROP_ROLE_OEMPRIVILEGES : RFPROP_ROLE_ASSIGNEDPRIVILEGES;

    return_val_do_info_if_fail(UUSER_ROLE_CUSt1 <= role_id && UUSER_ROLE_CUSt4 >= role_id, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_WRITABLE, NULL, o_message_jso, priv_prop_name));

    
    
    ret = json_array_duplicate_check(priv_array_jso, &duplicate_index);
    if (RF_OK == ret) {
        ret = snprintf_s(prop_path, MAX_CHARACTER_NUM, MAX_CHARACTER_NUM - 1, "%s/%d", priv_prop_name, duplicate_index);
        return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "format property path failed, ret is %d", ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, priv_prop_name, o_message_jso, prop_path);

        return HTTP_BAD_REQUEST;
    }

    if (FALSE == check_oem_priv) {
        
        tmp_jso = json_object_new_string(RF_VALUE_CONFIGURE_USERS);
        b_ret = is_json_object_array_element_exist(priv_array_jso, tmp_jso);
        (void)json_object_put(tmp_jso);

        return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, priv_prop_name, o_message_jso,
            RF_VALUE_CONFIGURE_USERS, priv_prop_name));

        tmp_jso = json_object_new_string(RF_VALUE_LOGIN);
        b_ret = is_json_object_array_element_exist(priv_array_jso, tmp_jso);
        (void)json_object_put(tmp_jso);

        return_val_do_info_if_fail(TRUE == b_ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_ITEM_MISSING, priv_prop_name, o_message_jso, RF_VALUE_LOGIN));
        
        tmp_jso = json_object_new_string(RF_VALUE_CONFIGURE_MANAGER);
        b_ret = is_json_object_array_element_exist(priv_array_jso, tmp_jso);
        (void)json_object_put(tmp_jso);
        return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, priv_prop_name, o_message_jso,
            RF_VALUE_CONFIGURE_MANAGER, priv_prop_name));
        
    }

    return VOS_OK;
    
}


LOCAL gint32 _check_priv_changed_and_perform_setting(guchar from_webui_flag, const gchar *user_name,
    const gchar *client_ip, OBJ_HANDLE role_handle, json_object *priv_array_jso, const gchar *priv_prop_name,
    const gchar *rf_priv_str)
{
    gchar               method_name[METHOD_NAME_MAX_LEN + 1] = {0};
    gint32 ret;
    guchar priv_enabled;
    const gchar *object_name;
    json_object *priv_jso = NULL;
    gboolean priv_set_flag = FALSE;
    GSList *input_list = NULL;
    gboolean b_ret;

    object_name = dfl_get_object_name(role_handle);
    ret = dal_get_property_value_byte(role_handle, priv_prop_name, &priv_enabled);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get %s from object handle %s failed, ret is %d", priv_prop_name, object_name, ret));

    priv_jso = json_object_new_string(rf_priv_str);
    b_ret = is_json_object_array_element_exist(priv_array_jso, priv_jso);
    (void)json_object_put(priv_jso);
    if (ENABLED == priv_enabled) {
        
        if (FALSE == b_ret) {
            priv_set_flag = TRUE;
            priv_enabled = FALSE;
        }
    } else {
        
        if (TRUE == b_ret) {
            priv_set_flag = TRUE;
            priv_enabled = TRUE;
        }
    }

    
    return_val_if_expr(FALSE == priv_set_flag, VOS_OK);

    ret = snprintf_s(method_name, METHOD_NAME_MAX_LEN + 1, METHOD_NAME_MAX_LEN, "Set%s", priv_prop_name);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "format privilege set method failed, ret is %d", ret));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(priv_enabled));
    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client_ip, role_handle, CLASS_USERROLE, method_name,
        AUTH_DISABLE, 0, input_list, NULL);
    uip_free_gvariant_list(input_list);
    

    return ret;
}


LOCAL gint32 set_role_assignedprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar roleid = 0;
    OBJ_HANDLE obj_handle = 0;
    
    
    str_map_s           assigned_priv_check_array[] = {
        {PROPERTY_USERROLE_CONFIGURE_SELF,                 RF_VALUE_CONFIGURE_SELF},
        {PROPERTY_USERROLE_BASICSETTING,                   RF_VALUE_CONFIGURE_COMPONENTS}
    };
    guint32 len;
    guint32 i;
    

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer or input paras error.", __FUNCTION__, __LINE__);

        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ROLE_ASSIGNEDPRIVILEGES, o_message_jso,
        RFPROP_ROLE_ASSIGNEDPRIVILEGES));

    
    
    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, &obj_handle, o_message_jso);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "check role uri for %s failed, ret is %d", i_paras->uri, ret));

    ret = _role_priv_config_precheck(FALSE, roleid, i_paras->val_jso, o_message_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "precheck for setting assigned privilege failed, ret is %d", ret));

    len = (guint32)G_N_ELEMENTS(assigned_priv_check_array);
    for (i = 0; i < len; i++) {
        
        ret = _check_priv_changed_and_perform_setting(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            obj_handle, i_paras->val_jso, assigned_priv_check_array[i].str1, assigned_priv_check_array[i].str2);
        if (VOS_OK != ret) {
            
            do_val_if_expr(NULL == *o_message_jso, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

            debug_log(DLOG_ERROR, "check property setting for %s failed, ret is %d", assigned_priv_check_array[i].str1,
                ret);
            continue;
        }
        
    }

    
    return (NULL == *o_message_jso) ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;

    
}


LOCAL gint32 set_role_oemprivileges(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 result;
    guchar roleid = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guchar board_type = 0;
    
    guint32 len;
    str_map_s           oem_priv_map_info[] = {
        {PROPERTY_USERROLE_KVMMGNT,                    RF_VALUE_KVM},
        {PROPERTY_USERROLE_VMMMGNT,                    RF_VALUE_VMM},
        {PROPERTY_USERROLE_POWERMGNT,                  RF_VALUE_POWER_CONTROL},
        {PROPERTY_USERROLE_SECURITYMGNT,               RF_VALUE_SECURITY_SETTINGS},
        {PROPERTY_USERROLE_DIAGNOSEMGNT,               RF_VALUE_DIAGNOSIS}
    };
    guint32 i;

    
    return_val_do_info_if_expr(NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    ret = redfish_get_board_type(&board_type);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(BOARD_SWITCH != board_type, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_ROLE_OEMPRIVILEGES, o_message_jso,
        RFPROP_ROLE_OEMPRIVILEGES));
    

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ROLE_OEMPRIVILEGES, o_message_jso,
        RFPROP_ROLE_OEMPRIVILEGES));

    
    result = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, &obj_handle, o_message_jso);
    goto_label_do_info_if_fail(VOS_OK == result, err_exit,
        debug_log(DLOG_ERROR, "check role uri for %s failed, ret is %d", i_paras->uri, result));

    ret = _role_priv_config_precheck(TRUE, roleid, i_paras->val_jso, o_message_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret,
        debug_log(DLOG_ERROR, "oem privilege precheck failed, ret is %d", ret));

    len = (guint32)G_N_ELEMENTS(oem_priv_map_info);
    for (i = 0; i < len; i++) {
        
        ret = _check_priv_changed_and_perform_setting(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            obj_handle, i_paras->val_jso, oem_priv_map_info[i].str1, oem_priv_map_info[i].str2);
        if (VOS_OK != ret) {
            do_val_if_expr(NULL == *o_message_jso, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

            debug_log(DLOG_ERROR, "check property setting for %s failed, ret is %d", oem_priv_map_info[i].str1, ret);

            continue;
        }
        
    }

    return (NULL == *o_message_jso) ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;

    

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    debug_log(DLOG_ERROR, "%s, %d: internal error occur to uip_call_class_method_redfish.\n", __FUNCTION__, __LINE__);

    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 role_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    gint32 ret;
    guchar roleid;
    json_object *o_message_jso = NULL;

    ret = redfish_check_role_uri_effective(i_paras->member_id, i_paras->uri, &roleid, NULL, &o_message_jso);

    do_val_if_expr(NULL != o_message_jso, (void)json_object_put(o_message_jso));

    
    return_val_if_fail(VOS_OK == ret, ret);

    

    *prop_provider = g_role_provider;
    *count = sizeof(g_role_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
