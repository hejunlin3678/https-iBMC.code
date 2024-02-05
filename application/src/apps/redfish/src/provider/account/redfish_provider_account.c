
#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_util.h"
#include "../apps/bmc_global/inc/bmc.h"
#include "../apps/upgrade/inc/upgrade_pub.h"


typedef struct {
    const gchar *prop_name;
    gint32 (*set_prop)(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
        json_object *oem_data_jso);
} ACCOUNT_SET_OEM_METHOD;

#define ACCOUNT_PROP_NAME_LOGININTERFACE "Oem/Huawei/LoginInterface"
#define ACCOUNT_PROP_NAME_SNMP_PRIV_PRO "Oem/Huawei/SnmpV3PrivProtocol"
#define ACCOUNT_PROP_NAME_SNMP_AUTH_PRO "Oem/Huawei/SnmpV3AuthProtocol"
#define ACCOUNT_PROP_NAME_SNMP_PRIV_PWD "Oem/Huawei/SnmpV3PrivPasswd"

LOCAL gint32 get_account_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_username(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_roleid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_locked(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_account_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_account_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 *user_id);
LOCAL gint32 get_account_mutualauthentication_clicert(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso,
    guint8 *user_id);
LOCAL gint32 get_account_ssh_publickey_hash(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 *user_id);
LOCAL gint32 get_account_login_interface(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 *user_id);
LOCAL gint32 get_mutual_auth_client_certificate_actions(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso,
    guint8 *user_id);

LOCAL gint32 get_action_info_odata_id_account(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 set_account_username(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_account_password(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_account_roleid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_account_locked(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_account_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_account_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_account_oem_data(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *o_result_jso);
LOCAL gint32 set_account_insecure_prompt(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *oem_data_jso);
LOCAL gint32 act_account_import_client_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_account_import_ssh_publickey_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_account_delete_client_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_account_delete_ssh_publickey(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_account_import_file_uri(PROVIDER_PARAS_S *i_paras, const gchar *opt_user, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
LOCAL gint32 task_account_import_client_cert(json_object *user_data, json_object *body_jso, guint8 user_id,
    gchar *filepath, json_object **o_message_jso);
LOCAL gint32 task_account_import_ssh_publickey(json_object *user_data, json_object *body_jso, guint8 user_id,
    gchar *filepath, json_object **o_message_jso);
LOCAL TASK_MONITOR_INFO_S *import_monitor_info_new(PROVIDER_PARAS_S *i_paras);
LOCAL void import_monitor_info_free_func(void *monitor_data);
LOCAL void parse_ssh_error_code(gint32 ret, json_object **o_message_jso);
LOCAL void parse_cert_error_code(gint32 ret, json_object **o_message_jso);
LOCAL gint32 parse_logininterface_value(json_object *oem_data_jso, guint32 *logininterface_val);
LOCAL gint32 get_logininterface_value(json_object **o_message_jso, json_object *oem_data_jso,
    guint32 *logininterface_val);
LOCAL gint32 set_snmp_auth_algorithm(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *o_result_jso);
LOCAL gint32 set_account_logininterface(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *o_result_jso);
LOCAL gint32 get_auth_protocol_input_list(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso,
    json_object **o_message_jso, GSList **input_list);

LOCAL PROPERTY_PROVIDER_S g_account_provider[] = {
    {RFPROP_ACCOUNT_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_account_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_account_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_USERNAME, CLASS_USER, PROPERTY_USER_NAME, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, get_account_username, set_account_username, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_PASSWORD, CLASS_USER, PROPERTY_USER_PASSWD, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, NULL, set_account_password, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_ACCOUNT_ROLE_ID, CLASS_USER, PROPERTY_USER_ROLEID, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, get_account_roleid, set_account_roleid, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_LOCKED, CLASS_USER, METHOD_USER_ISUSERLOCKED, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, get_account_locked, set_account_locked, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_ENABLED, CLASS_USER, PROPERTY_USER_ISENABLE, USERROLE_USERMGNT, SYS_LOCKDOWN_FORBID, get_account_enabled, set_account_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_LINKS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_account_links, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACCOUNT_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT,  SYS_LOCKDOWN_FORBID, get_account_oem,       set_account_oem, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT,  SYS_LOCKDOWN_FORBID, NULL,              NULL,              act_account_import_client_cert_entry, ETAG_FLAG_ENABLE},
    {RFACTION_MUTAU_CERT_DELETE_CLI_CERT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT,  SYS_LOCKDOWN_FORBID, NULL,              NULL,              act_account_delete_client_cert, ETAG_FLAG_ENABLE},
    {RFACTION_IMPORT_SSH_PUBLICKEY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, NULL, NULL,              act_account_import_ssh_publickey_entry, ETAG_FLAG_ENABLE},
    {RFACTION_DELETE_SSH_PUBLICKEY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_FORBID, NULL, NULL,              act_account_delete_ssh_publickey, ETAG_FLAG_ENABLE}
};


LOCAL PROPERTY_PROVIDER_S g_account_action_info_provider[] = {
    {RFPROP_ACCOUNT_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_action_info_odata_id_account, NULL, NULL, ETAG_FLAG_ENABLE},
};



LOCAL user_privilege_info_s g_role_info[] = {
    {USER_ROLE_ADMIN,        UUSER_ROLE_ADMIN,     UUSER_ROLE_ADMIN},
    {USER_ROLE_OPERATOR,     UUSER_ROLE_OPER,      UUSER_ROLE_OPER},
    {USER_ROLE_COMMONUSER,   UUSER_ROLE_USER,      UUSER_ROLE_USER},
    {USER_ROLE_CUSTOMROLE1,  UUSER_ROLE_OPER,      UUSER_ROLE_CUSt1},
    {USER_ROLE_CUSTOMROLE2,  UUSER_ROLE_OPER,      UUSER_ROLE_CUSt2},
    {USER_ROLE_CUSTOMROLE3,  UUSER_ROLE_OPER,      UUSER_ROLE_CUSt3},
    {USER_ROLE_CUSTOMROLE4,  UUSER_ROLE_OPER,      UUSER_ROLE_CUSt4},
    {USER_ROLE_NOACCESS,     UUSER_ROLE_NOACCESS,  UUSER_ROLE_NOACCESS}
};


LOCAL gboolean account_first_login_denied(guint8 is_from_webui, const gchar* user_name)
{
    OBJ_HANDLE user_handle = 0;
    gint32 ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, user_name, &user_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s can't found username %s, err %d", __FUNCTION__, user_name, ret);
        return FALSE;
    }

    guchar userid = 0;
    (void)dal_get_property_value_byte(user_handle, PROPERTY_USER_ID, &userid);

    if (is_from_webui) {
        is_h2m_user_need_reset_password(userid);
    }

    return is_m2m_user_login_denied(userid, FALSE);
}


LOCAL gint32 redfish_check_account_uri_effective(const gchar *i_member_id, const gchar *i_uri, gchar *o_username,
    guint32 username_size, OBJ_HANDLE *o_obj_handle, json_object **o_message_jso)
{
    int iRet;
    guint8 uri_id = 0;
    gchar user_name[USER_USERNAME_MAX_LEN + 1] = {0};
    gchar member_id[MAX_MEM_ID_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    const gint str_base_system = 10;

    
    if (NULL == i_member_id || NULL == i_uri || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(member_id, sizeof(member_id), sizeof(member_id) - 1, "%s", i_member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    return_val_do_info_if_fail(USERID_MAX_LEN >= strlen(member_id), HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_uri));
    gint32 ret = vos_str2int(member_id, str_base_system, &uri_id, NUM_TPYE_UCHAR);

    return_val_do_info_if_fail((VOS_OK == ret && MIN_USER_ID <= uri_id && MAX_USER_ID >= uri_id), HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_uri));

    
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, uri_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, user_name, USER_USERNAME_MAX_LEN + 1);
    return_val_do_info_if_fail((0 != strlen(user_name)), HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_uri));

    
    if (NULL != o_obj_handle) {
        *o_obj_handle = obj_handle;
    }

    
    if ((o_username != NULL) && (username_size != 0)) {
        ret = snprintf_s(o_username, username_size, username_size - 1, "%s", user_name);
        do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    }

    return VOS_OK;
}


LOCAL gint32 get_account_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), NULL,
        o_message_jso);

    return_val_if_fail((VOS_OK == ret), ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_ODATA_ID, o_message_jso));
    }

    
    
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, "%s/%s", ACCOUNTSERVICE_ACCOUNTS, i_paras->member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);
    
    return HTTP_OK;
}


LOCAL gint32 get_account_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 result;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL Pointer. ", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    result = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), NULL,
        o_message_jso);
    return_val_if_fail(VOS_OK == result, result);

    if (0 == i_paras->is_satisfy_privi) {
        
        result = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == result, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_ID, o_message_jso));
    }

    
    *o_result_jso = json_object_new_string(i_paras->member_id);
    return HTTP_OK;
}


LOCAL gint32 get_account_username(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), NULL,
        o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_USERNAME, o_message_jso));
    }

    
    *o_result_jso = json_object_new_string((const gchar *)username);
    return HTTP_OK;
}


LOCAL gint32 get_account_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 user_enabled = 0;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 result = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username),
        &obj_handle, o_message_jso);
    return_val_if_fail(VOS_OK == result, result);

    
    if (0 == i_paras->is_satisfy_privi) {
        result = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == result, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_ENABLED, o_message_jso));
    }

    
    (void)dal_get_property_value_byte(obj_handle, g_account_provider[i_paras->index].pme_prop_name, &user_enabled);
    *o_result_jso = json_object_new_boolean(user_enabled);

    return HTTP_OK;
}


LOCAL gint32 get_account_roleid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 role_id = 0;
    gchar role_str[MAX_ROLEID_LENGTH + 1] = {0};
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL Pointer.  ", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), &obj_handle,
        o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_ROLE_ID, o_message_jso));
    }

    
    (void)dal_get_property_value_byte(obj_handle, g_account_provider[i_paras->index].pme_prop_name, &role_id);
    (void)redfish_roleid_int2str(role_id, role_str, sizeof(role_str));
    *o_result_jso = json_object_new_string((const gchar *)role_str);

    return HTTP_OK;
}


LOCAL gint32 get_account_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    guint8 role_id = 0;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    gchar role_str[MAX_ROLEID_LENGTH + 1] = {0};
    gchar role_uri[RF_LINKS_MAXURI_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), &obj_handle,
        o_message_jso);

    return_val_if_fail(VOS_OK == ret, ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_LINKS, o_message_jso));
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ROLEID, &role_id);
    (void)redfish_roleid_int2str(role_id, role_str, sizeof(role_str));
    iRet = snprintf_s(role_uri, sizeof(role_uri), sizeof(role_uri) - 1, "%s/%s", ACCOUNTSERVICE_ROLES, role_str);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    obj_jso = json_object_new_object();
    
    return_val_do_info_if_expr(NULL == obj_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object error", __FUNCTION__));
    
    json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)role_uri));

    *o_result_jso = json_object_new_object();
    
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(obj_jso);
        debug_log(DLOG_ERROR, "%s:json_object_new_object error", __FUNCTION__));
    
    json_object_object_add(*o_result_jso, RF_ACCOUNT_LINKS_ROLE, obj_jso);

    return HTTP_OK;
}


LOCAL gint32 get_account_locked(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    gint32 user_locked;
    OBJ_HANDLE obj_handle = 0;
    GSList *output_list = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), &obj_handle,
        o_message_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    if (0 == i_paras->is_satisfy_privi) {
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_LOCKED, o_message_jso));
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_ISUSERLOCKED, 0, 0, NULL, &output_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get the output_list of user_locked fail.\n", __FUNCTION__, __LINE__));
    user_locked = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);
    *o_result_jso = json_object_new_boolean(user_locked);

    return HTTP_OK;
}

LOCAL void get_message_info_by_namesync(gint32 massage, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    switch (massage) {
        case USER_INVALID_DATA_FIELD:
            (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_ACCOUNT_USERNAME, o_message_jso,
                json_object_get_string(i_paras->val_jso), RFPROP_ACCOUNT_USERNAME, USER_NAME_MAX_LEN_STR);
            break;
        case USER_NAME_EXIST:
            (void)create_message_info(MSGID_RSC_ALREADY_EXIST, RFPROP_ACCOUNT_USERNAME, o_message_jso);
            break;
        case USER_NAME_RESTRICTED:
            (void)create_message_info(MSGID_USERNAME_IS_RESTRICTED, RFPROP_ACCOUNT_USERNAME, o_message_jso);
            break;
        case UUSER_USERNAME_INVALID:
            (void)create_message_info(MSGID_INVALID_USERNAME, RFPROP_ACCOUNT_USERNAME, o_message_jso);
            break;
        case UUSER_LINUX_DEFAULT_USER:
            (void)create_message_info(MSGID_CONFLICT_WITH_LINUX_DEFAULT_USER, RFPROP_ACCOUNT_USERNAME, o_message_jso);
            break;
        default:
            (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, RFPROP_ACCOUNT_USERNAME, o_message_jso);
    }
    return;
}


LOCAL gint32 set_account_username(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar userid = 0;
    OBJ_HANDLE obj_handle = 0;
    const gchar *json_str = NULL;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;
    guint32 count_number = 0;
    const guint32 max_try_count = 10;
    const guint32 query_interval = 200; // 每200毫秒查询一次用户名的修改是否同步到系统

    
    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer, or input paras error.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ACCOUNT_USERNAME, o_message_jso,
        RFPROP_ACCOUNT_USERNAME));

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, NULL, 0, &obj_handle, o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_USERNAME, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFPROP_ACCOUNT_USERNAME, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    json_str = dal_json_object_get_str(i_paras->val_jso);
    
    return_val_do_info_if_expr(NULL == json_str || 0 == strlen(json_str), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_USERNAME, RFPROP_ACCOUNT_USERNAME, o_message_jso));

    // 用户接口中用户名长度限制改为32位，各模块单独限制16位
    if (strlen(json_str) > 16) {
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_ACCOUNT_USERNAME, o_message_jso,
            json_object_get_string(i_paras->val_jso), RFPROP_ACCOUNT_USERNAME, USER_NAME_MAX_LEN_STR);
        return HTTP_BAD_REQUEST;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &userid);
    input_list = g_slist_append(input_list, g_variant_new_byte(userid));
    input_list = g_slist_append(input_list, g_variant_new_string(json_str));
    

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETUSERNAME, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);
    if (VOS_OK == ret) {
        
        // 5. 类方法调用成功，output_list返回值决定消息体，并进行指针释放
        ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        if (ret == VOS_OK) {
            for (count_number = 0; count_number < max_try_count; ++count_number) {
                
                vos_task_delay(query_interval);
                _cleanup_gvariant_slist_full_ GSList *sync_input_list = NULL;
                sync_input_list = g_slist_append(sync_input_list, g_variant_new_string(json_str));
                ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    obj_handle, CLASS_USER, METHOD_USER_CHECK_NAMESYNC_STATE, AUTH_DISABLE,
                    i_paras->user_role_privilege, sync_input_list, NULL);
                break_do_info_if_expr(ret == VOS_OK,
                    debug_log(DLOG_DEBUG, "[%s] User name sync ok in loop %u", __func__, count_number));
            }

            do_if_expr(count_number == max_try_count,
                debug_log(DLOG_ERROR, "[%s] User name sync is not done in 2 sec", __func__));
            
            return HTTP_OK;
        }

        get_message_info_by_namesync(ret, i_paras, o_message_jso);
        

        debug_log(DLOG_ERROR, "%s: set username fail ret:%d.", __FUNCTION__, ret);
        return HTTP_BAD_REQUEST;
    } else {
        debug_log(DLOG_ERROR, "%s: set username ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_USERNAME, o_message_jso);
        uip_free_gvariant_list(output_list);
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 _rf_check_user_configure_priv(OBJ_HANDLE handle, guint8 is_from_webui, guchar priv, const gchar *user_name,
    const gchar *target_user)
{
    // 判断是否是配置自身
    if (g_strcmp0(user_name, target_user) != 0) {
        goto user_mngt_judge;
    }

    guchar configure_self_priv = DISABLED;
    (void)dal_get_user_role_priv_enable(user_name, PROPERTY_USERROLE_CONFIGURE_SELF, &configure_self_priv);
    // 配置自身权限
    if (configure_self_priv == ENABLED) {
        return RET_OK;
    }

user_mngt_judge:
    // 判断是否是具有用户配置权限
    if (priv & USERROLE_USERMGNT) {
        if (account_first_login_denied(is_from_webui, user_name)) {
            return USER_NEED_RESET_PASSWORD;
        }
        return RET_OK;
    }
    return RET_ERR;
}


LOCAL gint32 check_username_password_same(json_object *request_body)
{
    json_object *username_jso = NULL;
    const gchar *username_str = NULL;
    const gchar *value_str = NULL;
    guint32 username_len;
    guint32 passwd_len;
    guint32 i;
    guint32 j;
    guint32 same_count = 0;

    return_val_if_expr(NULL == request_body, VOS_OK);

    // 请求体不存在用户名，返回VOS_OK
    return_val_if_fail(TRUE == json_object_object_get_ex(request_body, RFPROP_ACCOUNT_USERNAME, &username_jso), VOS_OK);

    username_str = dal_json_object_get_str(username_jso);
    return_val_if_expr(NULL == username_str, VOS_OK);

    (void)get_element_str(request_body, RFPROP_ACCOUNT_PASSWORD, &value_str);
    if (value_str == NULL) {
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFPROP_ACCOUNT_PASSWORD);
        return VOS_OK;
    }

    username_len = strlen(username_str);
    passwd_len = strlen(value_str);
    // 长度不相同，肯定不相同，返回VOS_OK
    return_val_if_fail(username_len == passwd_len, VOS_OK);

    // 相同返回VOS_ERR
    return_val_if_expr(0 == g_strcmp0(username_str, value_str), VOS_ERR);

    for (i = 0, j = username_len - 1; i < username_len; i++, j--) {
        do_if_expr(*(value_str + i) == *(username_str + j), (same_count++));
    }
    // 密码和用户名倒写相同，返回VOS_ERR
    return_val_if_expr(same_count == username_len, VOS_ERR);

    return VOS_OK;
}


LOCAL gint32 parse_set_password_return_value(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint32 ret_val)
{
    switch (ret_val) {
        case VOS_OK:
            return HTTP_OK;
        case USER_TIME_LIMIT_UNREASONABLE:
            (void)create_message_info(MSGID_DURING_MIN_PSWD_AGE, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
            break;
        case UUSER_USERPASS_TOO_LONG:
            (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_ACCOUNT_PASSWORD, o_message_jso,
                dal_json_object_get_str(i_paras->val_jso), RFPROP_ACCOUNT_PASSWORD, USER_PWD_MAX_LEN_STR);
            break;
        case USER_CANNT_SET_SAME_PASSWORD:
            (void)create_message_info(MSGID_INVALID_PSWD_SAME_HISTORY, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
            break;
        case UUSER_USERPASS_EMPTY:
        case USER_SET_PASSWORD_EMPTY:
        case COMP_CODE_INVALID_FIELD:
            (void)create_message_info(MSGID_INVALID_PAWD, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
            break;
        case USER_PASS_COMPLEXITY_FAIL:
            (void)create_message_info(MSGID_PSWD_CMPLX_CHK_FAIL, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
            break;
        case USER_SET_PASSWORD_TOO_WEAK:
            (void)create_message_info(MSGID_PSWD_IN_WEAK_PWDDICT, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
            break;
        default:
            (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
    }
    debug_log(DLOG_ERROR, "%s: set password fail ret:%d.", __FUNCTION__, ret_val);

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 exec_set_account_password(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    gint32 ret;
    guchar userid = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE pass_setting_obj = 0;
    guint8 check_password = 0;
    json_object *request_body = NULL;
    

    
    
    return_val_do_info_if_fail(NULL != dal_json_object_get_str(i_paras->val_jso), HTTP_BAD_REQUEST,
        json_string_clear(i_paras->val_jso);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_ACCOUNT_PASSWORD, o_message_jso, RF_VALUE_NULL,
        RFPROP_ACCOUNT_PASSWORD));
    
    

    

    
    (void)dal_get_object_handle_nth(CLASS_PASSWD_SETTING, 0, &pass_setting_obj);
    (void)dal_get_property_value_byte(pass_setting_obj, PROPERTY_USER_PASSWD_SETTING, &check_password);
    if (RF_ENABLE == check_password) {
        request_body = (json_object *)i_paras->user_data;
        ret = check_username_password_same(request_body);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST, json_string_clear(i_paras->val_jso);
            (void)create_message_info(MSGID_PSWD_NOT_STSFY_CMPLX, RFPROP_ACCOUNT_PASSWORD, o_message_jso,
            RFPROP_ACCOUNT_PASSWORD));
    }
    

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &userid);
    input_list = g_slist_append(input_list, g_variant_new_byte(userid));
    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(i_paras->val_jso)));
    input_list = g_slist_append(input_list, g_variant_new_byte(PWSWORDMAXLENGH));
    input_list = g_slist_append(input_list, g_variant_new_byte(OPERATION_SET_PASSWD));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETPASSWORD, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, json_string_clear(i_paras->val_jso);
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_USER_SETPASSWORD, ret));
    

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);

    ret = parse_set_password_return_value(i_paras, o_message_jso, ret);

    
    json_string_clear(i_paras->val_jso);
    

    return ret;

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_PASSWORD, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;

    
}


LOCAL json_bool is_add_ipmi_permission(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle)
{
    json_object *oem_jso = NULL;
    json_object *oem_data_jso = NULL;
    guint32 new_logininterface = 0;
    guint32 old_logininterface = 0;

    json_bool ret_bool = json_object_object_get_ex(i_paras->user_data, RFPROP_ACCOUNT_OEM, &oem_jso);
    if (ret_bool == FALSE || oem_jso == NULL) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, RFPROP_ACCOUNT_OEM);
        return FALSE;
    }

    ret_bool = json_object_object_get_ex(oem_jso, RFPROP_ACCOUNT_HUAWEI, &oem_data_jso);
    if (ret_bool == FALSE || oem_data_jso == NULL) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, RFPROP_ACCOUNT_HUAWEI);
        return FALSE;
    }

    
    gint32 ret = parse_logininterface_value(oem_data_jso, &new_logininterface);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s get logininterface_value fail", __FUNCTION__);
        return FALSE;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &old_logininterface);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, PROPERTY_USER_LOGIN_INTERFACE);
        return FALSE;
    }
    old_logininterface = old_logininterface & USER_LOGIN_INTERFACE_VALUE_MASK;

    
    if ((old_logininterface & RFPROP_LOGIN_INTERFACE_IPMI_VALUE) == 0 &&
        (new_logininterface & RFPROP_LOGIN_INTERFACE_IPMI_VALUE) != 0) {
        debug_log(DLOG_INFO, "%s: add ipmi permission", __FUNCTION__);
        
        return TRUE;
    }

    return FALSE;
}


LOCAL json_bool is_modified_snmp_auth_protocol(PROVIDER_PARAS_S *i_paras)
{
    json_object *oem_jso = NULL;
    json_object *oem_data_jso = NULL;
    struct json_object *temp_jso = NULL;

    json_bool ret_bool = json_object_object_get_ex(i_paras->user_data, RFPROP_ACCOUNT_OEM, &oem_jso);
    if (ret_bool == FALSE || oem_jso == NULL) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, RFPROP_ACCOUNT_OEM);
        return FALSE;
    }

    ret_bool = json_object_object_get_ex(oem_jso, RFPROP_ACCOUNT_HUAWEI, &oem_data_jso);
    if (ret_bool == FALSE || oem_data_jso == NULL) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, RFPROP_ACCOUNT_HUAWEI);
        return FALSE;
    }

    ret_bool = json_object_object_get_ex(oem_data_jso, SNMP_V3AUTH_STR, &temp_jso);
    if (ret_bool == TRUE && temp_jso != NULL) {
        
        return TRUE;
    }

    debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, SNMP_V3AUTH_STR);
    return FALSE;
}


LOCAL gint32 set_password_check_user_privilege(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_bool auth_protocol, json_bool is_add_ipmi, gchar *username, json_object **o_message_jso)
{
    const gchar *prop_name_auth_protocol = "Oem/Huawei/SnmpV3AuthProtocol";
    const gchar *prop_name_logininterface = "Oem/Huawei/LoginInterface";

    gint32 ret = _rf_check_user_configure_priv(obj_handle, i_paras->is_from_webui, i_paras->user_role_privilege,
        i_paras->user_name, username);
    if (ret == VOS_OK) { 
        return HTTP_OK;
    }

    if (ret == USER_NEED_RESET_PASSWORD) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, NULL, o_message_jso);
    } else if (auth_protocol != TRUE && is_add_ipmi != TRUE) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ACCOUNT_PASSWORD, o_message_jso,
            RFPROP_ACCOUNT_PASSWORD);
    } else if (auth_protocol == TRUE && is_add_ipmi == TRUE) {
        json_object *temp_jso = NULL;

        *o_message_jso = json_object_new_array();
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name_auth_protocol, &temp_jso,
            prop_name_auth_protocol);
        (void)json_object_array_add(*o_message_jso, temp_jso);
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name_logininterface, &temp_jso,
            prop_name_auth_protocol);
        (void)json_object_array_add(*o_message_jso, temp_jso);
        temp_jso = NULL;
    } else if (auth_protocol == TRUE) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name_auth_protocol, o_message_jso,
            prop_name_auth_protocol);
    } else if (is_add_ipmi == TRUE) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name_logininterface, o_message_jso,
            prop_name_logininterface);
    }

    return HTTP_FORBIDDEN;
}


LOCAL gint32 parse_set_auth_protocol_return_value(gint32 ret_val, const char *related_property,
    json_object **o_message_jso)
{
    switch (ret_val) {
        case VOS_OK:
            return HTTP_OK;
        
        case USER_DONT_EXIST:
            (void)create_message_info(MSGID_INVALID_USERNAME, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_TIME_LIMIT_UNREASONABLE:
            (void)create_message_info(MSGID_DURING_MIN_PSWD_AGE, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_CANNT_SET_SAME_PASSWORD:
            (void)create_message_info(MSGID_INVALID_PSWD_SAME_HISTORY, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case COMP_CODE_INVALID_FIELD:
            (void)create_message_info(MSGID_INVALID_PAWD, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_DATA_LENGTH_INVALID:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, related_property, o_message_jso,
                USER_PASSWORD_MIN_LEN_STR, USER_PWD_MAX_LEN_STR);
            return HTTP_BAD_REQUEST;

        case USER_SET_PASSWORD_TOO_WEAK:
            (void)create_message_info(MSGID_PSWD_IN_WEAK_PWDDICT, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_SET_PASSWORD_EMPTY:
            (void)create_message_info(MSGID_INVALID_PAWD, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_PASS_COMPLEXITY_FAIL:
            (void)create_message_info(MSGID_PSWD_CMPLX_CHK_FAIL, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_NODE_BUSY:
            (void)create_message_info(MSGID_OPERATION_FAILED, related_property, o_message_jso);
            return HTTP_BAD_REQUEST;
        case UUSER_USERPASS_TOO_LONG:
            (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, related_property, o_message_jso, RF_SENSITIVE_INFO,
                related_property, USER_PWD_MAX_LEN_STR);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, related_property, o_message_jso);
            debug_log(DLOG_ERROR, "%s: unknown method return value %d.", __FUNCTION__, ret_val);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL json_bool add_ipmi_before_auth_pro(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso,
    json_object **o_message_jso)
{
    GSList *input_list = NULL;
    const gchar *prop_name_logininterface = "Oem/Huawei/LoginInterface";
    guint32 new_logininterface = 0;

    
    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ACCOUNT_PROP_NAME_LOGININTERFACE, o_message_jso,
            ACCOUNT_PROP_NAME_LOGININTERFACE);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = get_logininterface_value(o_message_jso, o_result_jso, &new_logininterface);
    if (ret != HTTP_OK) {
        return FALSE;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(new_logininterface));
    // 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SET_USER_LOGIN_INTERFACE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret is %d", __FUNCTION__,
            METHOD_USER_SET_USER_LOGIN_INTERFACE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, prop_name_logininterface, o_message_jso);
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 rollback_ipmi(PROVIDER_PARAS_S *i_paras, guint32 old_logininterface, OBJ_HANDLE obj_handle,
    json_object **o_message_jso)
{
    GSList *input_list = NULL;
    gint32 ret;

    input_list = g_slist_append(input_list, g_variant_new_uint32(old_logininterface));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SET_USER_LOGIN_INTERFACE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    input_list = NULL;
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:call method %s failed, ret is %d", __FUNCTION__, METHOD_USER_SET_USER_LOGIN_INTERFACE,
            ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL void add_err_msg(json_object *o_message_jso, json_object *msg_err_jso)
{
    if (msg_err_jso == NULL || o_message_jso == NULL) {
        return;
    }

    if (json_object_get_type(o_message_jso) != json_type_array) {
        return;
    }

    if (json_object_get_type(msg_err_jso) == json_type_array) {
        guint32 array_len = json_object_array_length(msg_err_jso);
        guint32 msg_index;

        for (msg_index = 0; msg_index < array_len; msg_index++) {
            (void)json_object_array_add(o_message_jso,
                json_object_get(json_object_array_get_idx(msg_err_jso, msg_index)));
        }

        json_object_put(msg_err_jso);
    } else {
        (void)json_object_array_add(o_message_jso, msg_err_jso);
    }
    return;
}


LOCAL gint32 effective_ipmi_permission(PROVIDER_PARAS_S *i_paras, guint32 old_logininterface, OBJ_HANDLE obj_handle,
    json_bool add_ipmi_success, json_object *o_message_jso)
{
    json_object *msg_err_jso = NULL;
    gint32 ret;

    
    if (add_ipmi_success == TRUE) {
        ret = exec_set_account_password(i_paras, obj_handle, &msg_err_jso);
        
        add_err_msg(o_message_jso, msg_err_jso);
        msg_err_jso = NULL;
        // 如果密码设置失败需要回退IPMI权限
        if (ret != HTTP_OK) { // 设置密码失败应该将logininterface重置回原状态
            (void)rollback_ipmi(i_paras, old_logininterface, obj_handle, &msg_err_jso);
            
            add_err_msg(o_message_jso, msg_err_jso);
            msg_err_jso = NULL;
        } else {
            return HTTP_OK;
        }
    }

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 parse_auth_protocol_setting_result(PROVIDER_PARAS_S *i_paras, GSList *output_list,
    guint32 old_logininterface, OBJ_HANDLE obj_handle, json_bool add_ipmi_success, json_object *o_message_jso)
{
    const gint32 auth_protocol_index = 0;
    const gint32 auth_password_index = 1;
    const gint32 priv_password_index = 2;
    json_object *msg_err_jso = NULL;
    gint32 ret_back = HTTP_OK;
    gint32 auth_password_ret_val;
    gint32 priv_password_ret_val;
    gint32 auth_protocol_ret_val;

    auth_protocol_ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, auth_protocol_index));
    auth_password_ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, auth_password_index));
    priv_password_ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, priv_password_index));

    
    gint32 ret = parse_set_auth_protocol_return_value(auth_password_ret_val, RFPROP_ACCOUNT_PASSWORD, &msg_err_jso);
    if (ret != HTTP_OK) {
        ret_back = HTTP_BAD_REQUEST;

        add_err_msg(o_message_jso, msg_err_jso);
        msg_err_jso = NULL;

        
        if (add_ipmi_success == TRUE) {
            (void)rollback_ipmi(i_paras, old_logininterface, obj_handle, &msg_err_jso);
            add_err_msg(o_message_jso, msg_err_jso);
            msg_err_jso = NULL;
        }
    }

    
    ret = parse_set_auth_protocol_return_value(priv_password_ret_val, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, &msg_err_jso);
    if (ret != HTTP_OK) {
        ret_back = HTTP_BAD_REQUEST;
    }
    add_err_msg(o_message_jso, msg_err_jso);
    msg_err_jso = NULL;

    if (auth_protocol_ret_val != VOS_OK) {
        
        if (auth_password_ret_val == VOS_OK && priv_password_ret_val == VOS_OK) {
            (void)create_message_info(MSGID_INTERNAL_ERR, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, &msg_err_jso);

            add_err_msg(o_message_jso, msg_err_jso);
            msg_err_jso = NULL;
        }

        
        if (auth_password_ret_val == VOS_OK) {
            
            ret = effective_ipmi_permission(i_paras, old_logininterface, obj_handle, add_ipmi_success, o_message_jso);
            if (ret == HTTP_OK) {
                ret_back = HTTP_OK;
            }
        }
    }

    return ret_back;
}


LOCAL gint32 add_ipmi_and_modified_snmp_auth_protocol(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso, json_object *o_result_jso)
{
    json_object *msg_err_jso = NULL;
    json_bool add_ipmi_success;
    guint32 old_logininterface = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: null point.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &old_logininterface);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, ACCOUNT_PROP_NAME_LOGININTERFACE, &msg_err_jso);
        add_err_msg(*o_message_jso, msg_err_jso);
        msg_err_jso = NULL;
        debug_log(DLOG_ERROR, "%s: get property value fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    old_logininterface = old_logininterface & USER_LOGIN_INTERFACE_VALUE_MASK;

    
    add_ipmi_success = add_ipmi_before_auth_pro(i_paras, obj_handle, o_result_jso, &msg_err_jso);
    
    add_err_msg(*o_message_jso, msg_err_jso);
    msg_err_jso = NULL;

    
    ret = get_auth_protocol_input_list(i_paras, obj_handle, o_result_jso, &msg_err_jso, &input_list);
    if (ret != HTTP_OK) { // 如果没有返回HTTP_OK，则表示还没有调用远程方法设置鉴权算法
        
        add_err_msg(*o_message_jso, msg_err_jso);
        msg_err_jso = NULL;
        
        return effective_ipmi_permission(i_paras, old_logininterface, obj_handle, add_ipmi_success, *o_message_jso);
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETSNMPAUTHPROTOCOL, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    if (ret == RFERR_NO_RESOURCE) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, &msg_err_jso, i_paras->uri);
        add_err_msg(*o_message_jso, msg_err_jso);
        return HTTP_NOT_FOUND;
    } else if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: uip_call_class_method_redfish fail %d", __FUNCTION__, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_err_jso);
        add_err_msg(*o_message_jso, msg_err_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = parse_auth_protocol_setting_result(i_paras, output_list, old_logininterface, obj_handle, add_ipmi_success,
        *o_message_jso);

    uip_free_gvariant_list(output_list);
    output_list = NULL;

    return ret;
}


LOCAL void clear_jso_and_delete_object(json_object *jso, const gchar *prop_name)
{
    json_object_clear_string(jso, prop_name);
    json_object_object_del(jso, prop_name);
}


LOCAL void set_password_delete_object(PROVIDER_PARAS_S *i_paras, json_bool modify_snmp_auth,
    json_bool add_ipmi_permission)
{
    json_object *oem_jso = NULL;
    json_object *oem_data_jso = NULL;
    json_bool ret_bool;

    ret_bool = json_object_object_get_ex(i_paras->user_data, RFPROP_ACCOUNT_OEM, &oem_jso);
    if (ret_bool == FALSE || oem_jso == NULL) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, RFPROP_ACCOUNT_OEM);
        return;
    }

    ret_bool = json_object_object_get_ex(oem_jso, RFPROP_ACCOUNT_HUAWEI, &oem_data_jso);
    if (ret_bool == FALSE || oem_data_jso == NULL) {
        debug_log(DLOG_ERROR, "%s get property %s fail", __FUNCTION__, RFPROP_ACCOUNT_HUAWEI);
        return;
    }

    
    if (modify_snmp_auth == TRUE) {
        clear_jso_and_delete_object(oem_data_jso, SNMP_V3AUTH_STR);
        clear_jso_and_delete_object(oem_data_jso, SNMP_V3PRIVPASSWD);
    }

    
    if (add_ipmi_permission == TRUE) {
        clear_jso_and_delete_object(oem_data_jso, PROPERTY_USER_LOGIN_INTERFACE);
    }

    gint32 json_len = json_object_object_length(oem_data_jso);
    if (json_len == 0) {
        clear_jso_and_delete_object(oem_jso, RFPROP_ACCOUNT_HUAWEI);
    }

    return;
}


LOCAL gint32 set_account_password(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_bool modify_snmp_auth;
    json_bool add_ipmi_permission;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    json_object *oem_jso = NULL;
    json_object *oem_data_jso = NULL;
    json_bool ret_bool;

    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        
        json_string_clear(i_paras->val_jso);
        
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), &obj_handle,
        o_message_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "check account uri for %s failed, ret is %d", i_paras->uri, ret);
        json_string_clear(i_paras->val_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    add_ipmi_permission = is_add_ipmi_permission(i_paras, obj_handle);
    modify_snmp_auth = is_modified_snmp_auth_protocol(i_paras);

    
    ret = set_password_check_user_privilege(i_paras, obj_handle, modify_snmp_auth, add_ipmi_permission,
        (gchar *)username, o_message_jso);
    if (ret != HTTP_OK) {
        set_password_delete_object(i_paras, modify_snmp_auth, add_ipmi_permission);
        debug_log(DLOG_ERROR, "%s failed:check user privilege failed", __FUNCTION__);
        json_string_clear(i_paras->val_jso);
        return ret;
    }

    ret_bool = json_object_object_get_ex(i_paras->user_data, RFPROP_ACCOUNT_OEM, &oem_jso);
    if (ret_bool == FALSE || oem_jso == NULL) {
        return exec_set_account_password(i_paras, obj_handle, o_message_jso);
    }

    ret_bool = json_object_object_get_ex(oem_jso, RFPROP_ACCOUNT_HUAWEI, &oem_data_jso);
    if (ret_bool == FALSE || oem_data_jso == NULL) {
        return exec_set_account_password(i_paras, obj_handle, o_message_jso);
    }

    
    if (add_ipmi_permission == TRUE && modify_snmp_auth == TRUE) {
        
        ret = add_ipmi_and_modified_snmp_auth_protocol(i_paras, obj_handle, o_message_jso, oem_data_jso);
        
        clear_jso_and_delete_object(oem_data_jso, SNMP_V3AUTH_STR);
        clear_jso_and_delete_object(oem_data_jso, PROPERTY_USER_LOGIN_INTERFACE);
        clear_jso_and_delete_object(oem_data_jso, SNMP_V3PRIVPASSWD);
        gint32 json_len = json_object_object_length(oem_data_jso);
        if (json_len == 0) {
            clear_jso_and_delete_object(oem_jso, RFPROP_ACCOUNT_HUAWEI);
        }
    } else if (add_ipmi_permission != TRUE && modify_snmp_auth == TRUE) { // 单独修改鉴权算法
        ret = set_snmp_auth_algorithm(i_paras, obj_handle, o_message_jso, oem_data_jso);
        
        clear_jso_and_delete_object(oem_data_jso, SNMP_V3AUTH_STR);
        clear_jso_and_delete_object(oem_data_jso, SNMP_V3PRIVPASSWD);
        gint32 json_len = json_object_object_length(oem_data_jso);
        if (json_len == 0) {
            clear_jso_and_delete_object(oem_jso, RFPROP_ACCOUNT_HUAWEI);
        }
    } else if (add_ipmi_permission == TRUE && modify_snmp_auth != TRUE) { // 增加IPMI权限
        ret = set_account_logininterface(i_paras, obj_handle, o_message_jso, oem_data_jso);
        
        clear_jso_and_delete_object(oem_data_jso, PROPERTY_USER_LOGIN_INTERFACE);
        gint32 json_len = json_object_object_length(oem_data_jso);
        if (json_len == 0) {
            clear_jso_and_delete_object(oem_jso, RFPROP_ACCOUNT_HUAWEI);
        }
    } else { // 修改密码
        return exec_set_account_password(i_paras, obj_handle, o_message_jso);
    }

    return ret;
}


LOCAL gint32 set_account_roleid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 i;
    gint32 ret;
    guchar userid = 0;
    guchar priv = 0;
    guchar roleid = 0;
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *priv_input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *role_input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *priv_output_list = NULL;

    

    return_val_do_info_if_expr(((NULL == o_message_jso) || (VOS_OK != provider_paras_check(i_paras))),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ACCOUNT_ROLE_ID, o_message_jso,
        RFPROP_ACCOUNT_ROLE_ID));

    
    for (i = 0, ret = VOS_ERR; i < (int)G_N_ELEMENTS(g_role_info); i++) {
        if (!g_strcmp0(dal_json_object_get_str(i_paras->val_jso), g_role_info[i].role)) {
            roleid = g_role_info[i].role_id;
            priv = g_role_info[i].privilege;
            ret = VOS_OK;
            break;
        }
    }

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_ACCOUNT_ROLE_ID, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_ACCOUNT_ROLE_ID));

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, NULL, 0, &obj_handle, o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_ROLE_ID, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFPROP_ACCOUNT_USERNAME, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &userid);
    priv_input_list = g_slist_append(priv_input_list, g_variant_new_byte(userid));
    priv_input_list = g_slist_append(priv_input_list, g_variant_new_byte(priv));
    priv_input_list = g_slist_append(priv_input_list, g_variant_new_byte(roleid));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETPRIVILEGE, AUTH_DISABLE, i_paras->user_role_privilege, priv_input_list,
        &priv_output_list);
    if (VOS_OK == ret) {
        
        ret = g_variant_get_int32((GVariant *)g_slist_nth_data(priv_output_list, 0));
        switch (ret) {
            case VOS_OK:
                break;

            case USER_PRIV_RESTRICTED:
                (void)create_message_info(MSGID_ROLEID_IS_RESTRICTED, RFPROP_ACCOUNT_ROLE_ID, o_message_jso);
                break;

            case EXCLUDE_USER_WRONG:
                (void)create_message_info(MSGID_EMRGNCY_LOGIN_USER, RFPROP_ACCOUNT_ROLE_ID, o_message_jso,
                    RFPROP_ACCOUNT_ROLE_ID);
                break;

            default:
                (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, RFPROP_ACCOUNT_ROLE_ID, o_message_jso);
        }

        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
            debug_log(DLOG_ERROR, "%s, %d: set roleid fail ret:%d.", __FUNCTION__, __LINE__, ret));
    } else {
        debug_log(DLOG_ERROR, "%s: ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_ROLE_ID, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    role_input_list = g_slist_append(role_input_list, g_variant_new_byte(roleid));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETUSERROLEID, AUTH_DISABLE, i_paras->user_role_privilege, role_input_list, NULL);
    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        case HTTP_INTERNAL_SERVER_ERROR:
            debug_log(DLOG_ERROR,
                "%s, %d: set user roleid ret:%d, internal error occur to uip_call_class_method_redfish.\n",
                __FUNCTION__, __LINE__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_ROLE_ID, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;

        case EXCLUDE_USER_WRONG:
            (void)create_message_info(MSGID_EMRGNCY_LOGIN_USER, RFPROP_ACCOUNT_ROLE_ID, o_message_jso,
                RFPROP_ACCOUNT_ROLE_ID);
            debug_log(DLOG_ERROR, "%s, %d: set user roleid fail ret:%d.", __FUNCTION__, __LINE__, ret);
            return HTTP_BAD_REQUEST;

        default:
            (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, RFPROP_ACCOUNT_ROLE_ID, o_message_jso);
            debug_log(DLOG_ERROR, "%s, %d: set user roleid fail ret:%d.", __FUNCTION__, __LINE__, ret);
            return HTTP_BAD_REQUEST;
    }
}


LOCAL gint32 set_account_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar userid = 0;
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL Pointer. ", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ACCOUNT_ENABLED, o_message_jso,
        RFPROP_ACCOUNT_ENABLED));

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, NULL, 0, &obj_handle, o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_ENABLED, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFPROP_ACCOUNT_ENABLED, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &userid);
    input_list = g_slist_append(input_list, g_variant_new_byte(userid));
    input_list = g_slist_append(input_list, g_variant_new_string(""));
    input_list = g_slist_append(input_list, g_variant_new_byte(PWSWORDMAXLENGH));
    input_list = g_slist_append(input_list, g_variant_new_byte(json_object_get_boolean(i_paras->val_jso)));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETPASSWORD, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);
    if (VOS_OK == ret) {
        // 5. 类方法调用成功，output_list返回值决定消息体，并进行指针释放
        ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

        switch (ret) {
            case VOS_OK:
                return HTTP_OK;

            case USER_PASS_COMPLEXITY_FAIL:
                (void)create_message_info(MSGID_PSWD_NOT_STSFY_CMPLX, RFPROP_ACCOUNT_ENABLED, o_message_jso,
                    RFPROP_ACCOUNT_ENABLED);
                break;

            case USER_UNSUPPORT:
                (void)create_message_info(MSGID_EMRGNCY_LOGIN_USER, RFPROP_ACCOUNT_ENABLED, o_message_jso,
                    RFPROP_ACCOUNT_ENABLED);
                break;
            case USER_LAST_ADMIN_ERR:
                (void)create_message_info(MSGID_DISABLE_LAST_ADMIN, RFPROP_ACCOUNT_ENABLED, o_message_jso);
                break;
            default:
                (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, RFPROP_ACCOUNT_ENABLED, o_message_jso);
        }

        debug_log(DLOG_ERROR, "%s, %d: set user enabled output_list ret:%d.", __FUNCTION__, __LINE__, ret);
        return HTTP_BAD_REQUEST;
    } else {
        debug_log(DLOG_ERROR, "%s: set user enabled ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_ENABLED, o_message_jso);
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_account_locked(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_ACCOUNT_LOCKED, o_message_jso,
        RFPROP_ACCOUNT_LOCKED));

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), &obj_handle,
        o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_LOCKED, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFPROP_ACCOUNT_LOCKED, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    return_val_do_info_if_fail(FALSE == json_object_get_boolean(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_ACCOUNT_LOCKED, o_message_jso, "True",
        RFPROP_ACCOUNT_LOCKED));

    
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)username));
    input_list = g_slist_append(input_list, g_variant_new_string(""));
    input_list = g_slist_append(input_list, g_variant_new_byte(0x80 | LOG_TYPE_LOCAL));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)EXCLUDE_LOGIN_IP));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_AUTHUSER, AUTH_DISABLE, i_paras->user_role_privilege, input_list, &output_list);
    if (VOS_OK == ret) {
        // 5. 类方法调用成功，output_list返回值决定消息体，并进行指针释放
        ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        switch (ret) {
            case VOS_OK:
                return HTTP_OK;

            default:
                (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, RFPROP_ACCOUNT_LOCKED, o_message_jso);
                debug_log(DLOG_ERROR, "%s, %d: set user unlock output_list ret:%d.", __FUNCTION__, __LINE__, ret);
                return HTTP_BAD_REQUEST;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: set user unlock ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_LOCKED, o_message_jso);
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}


gint32 redfish_roleid_int2str(guint8 i_role_id, gchar *o_userrole_str, guint32 str_size)
{
    int iRet;

    return_val_if_fail(NULL != o_userrole_str, HTTP_INTERNAL_SERVER_ERROR);
    return_val_do_info_if_expr(str_size == 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed:input param error, str_size is 0.", __FUNCTION__));
    switch (i_role_id) {
        case UUSER_ROLE_USER:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_COMMONUSER);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_OPER:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_OPERATOR);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_ADMIN:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_ADMIN);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_CUSt1:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_CUSTOMROLE1);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_CUSt2:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_CUSTOMROLE2);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_CUSt3:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_CUSTOMROLE3);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_CUSt4:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_CUSTOMROLE4);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        case UUSER_ROLE_NOACCESS:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", USER_ROLE_NOACCESS);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;

        default:
            iRet = snprintf_s(o_userrole_str, str_size, str_size - 1, "%s", "unknown");
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            break;
    }

    return VOS_OK;
}



LOCAL gint32 get_account_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    // 变量定义                                   // 返回值
    gchar        username[USER_USERNAME_MAX_LEN + 1] = {0}; // 用户名
    guint8 user_id = 0;                                // 用户的ID
    OBJ_HANDLE obj_user_handle = 0;                    // 用户对象的句柄
    json_object *huawei_result_jso = NULL;             // 属性"Huawei"的值

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username),
        &obj_user_handle, o_message_jso);
    return_val_if_fail((VOS_OK == ret), ret);

    
    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_OEM, o_message_jso));
    }

    
    ret = dal_get_property_value_byte(obj_user_handle, PROPERTY_USER_ID, &user_id);
    return_val_do_info_if_fail(VOS_ERR != ret && (MIN_USER_ID <= user_id && MAX_USER_ID >= user_id),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: get user_id fail.\n", __FUNCTION__, __LINE__));

    
    // 获取"Huawei" 的值
    ret = get_account_huawei(i_paras, &huawei_result_jso, &user_id);
    return_val_do_info_if_fail(VOS_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    
    // 为属性"Oem"的值分配空间，用(*o_result_jso)来指向这一块内存的空间
    (*o_result_jso) = json_object_new_object();

    // 对分配空间的结果进行判断
    return_val_do_info_if_fail((NULL != *o_result_jso), HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_result_jso);
        debug_log(DLOG_ERROR, "%s, %d 'json_object_new_object fail' ", __FUNCTION__, __LINE__));

    // 将获得的结果添加入o_result_jso
    json_object_object_add((*o_result_jso), RFPROP_ACCOUNT_HUAWEI, huawei_result_jso);

    return HTTP_OK;
}


LOCAL gint32 get_account_login_rule(PROVIDER_PARAS_S *i_paras, json_object **longin_rule_jso, guint8 user_id)
{
    int iRet = -1;

    OBJ_HANDLE obj_handle = 0;
    guchar prop_value = 0;
    json_object *tmp_json = NULL;
    gchar slot[MAX_CHARACTER_NUM] = {0};
    gchar uri[MAX_CHARACTER_NUM] = {0};

    return_val_if_expr(NULL == longin_rule_jso, VOS_ERR);

    gint ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    *longin_rule_jso = json_object_new_array();
    return_val_if_expr(NULL == *longin_rule_jso, VOS_ERR);

    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_PERMIT_RULE_IDS, &prop_value);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    if (prop_value & 0x01) {
        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LOGRULE, slot, LOGRULE_1);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        tmp_json = json_object_new_object();
        json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
        json_object_array_add(*longin_rule_jso, tmp_json);
    }
    if (prop_value & 0x02) {
        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LOGRULE, slot, LOGRULE_2);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        tmp_json = json_object_new_object();
        json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
        json_object_array_add(*longin_rule_jso, tmp_json);
    }
    if (prop_value & 0x04) {
        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_LOGRULE, slot, LOGRULE_3);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        tmp_json = json_object_new_object();
        json_object_object_add(tmp_json, RFPROP_ODATA_ID, json_object_new_string((const gchar *)uri));
        json_object_array_add(*longin_rule_jso, tmp_json);
    }

    return VOS_OK;
}


LOCAL gint32 get_pwd_valid_days(guint32 *valid_days, guint8 user_id)
{
    OBJ_HANDLE obj_handle = 0;

    return_val_if_expr(NULL == valid_days, VOS_ERR);

    gint ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_PWDVALIDDAYS, valid_days);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    return VOS_OK;
}

LOCAL gboolean is_user_login(const gchar *user_name)
{
    struct utmp *us = NULL;

    setutent();

    while ((us = getutent()) != NULL) {
        if (us->ut_type == USER_PROCESS) {
            if (!strcmp(us->ut_user, user_name)) {
                endutent();
                return FALSE;
            }
        }
    }

    endutent();
    return TRUE;
}


LOCAL gboolean check_unique_admin(OBJ_HANDLE obj_handle)
{
    GSList* obj_list = NULL;
    GSList* obj_node =  NULL;
    guint8 role_id = 0;
    gint32 enabled_admin_num = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_ROLEID, &role_id);
    if (role_id != UUSER_ROLE_ADMIN) { // 当前用户不是管理员用户
        return FALSE;
    }

    gint32 ret = dfl_get_object_list(CLASS_USER, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_list fail", __FUNCTION__);
        return FALSE;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        guint8 is_user_enabled = 0xff;

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_USER_ISENABLE, &is_user_enabled);
        if (is_user_enabled == 0) {
            continue;
        }

        role_id = 0;
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_USER_ROLEID, &role_id);
        if (role_id == UUSER_ROLE_ADMIN) {
            enabled_admin_num++;
        }
    }

    g_slist_free(obj_list);
    if (enabled_admin_num == 1) {
        return TRUE;
    }

    return FALSE;
}


LOCAL gint32 get_user_deleteable(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, json_object **o_result_jso)
{
    gchar user_name[USER_NAME_MAX_LEN + 1] = {0};
    OBJ_HANDLE handle = 0;
    guint8 Version = 0;
    guint8 Exclude = 0;
    guint8 Userid = 0;
    guint8 user_id = 0;

    if (o_result_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    // 判断是否为已登录用户
    (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, user_name, sizeof(user_name));
    if (is_user_login(user_name) == FALSE) {
        json_object_object_add((*o_result_jso), RFPROP_USER_DELETEABLE, json_object_new_boolean(FALSE));
        json_object_object_add(*o_result_jso, RFPROP_USER_DELDISABLEREASON,
            json_object_new_string(RFPROP_USER_SSHLOGIN));
        return RET_OK;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &user_id);
    if (ret != RET_OK) {
        json_object_object_add((*o_result_jso), RFPROP_USER_DELETEABLE, NULL);
        return RET_ERR;
    }

    // 判断是否为安全增强用户
    (void)dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &handle);
    ret = dal_get_property_value_byte(handle, PROPERTY_EXCLUDE_USER, &Exclude);
    if (ret == RET_OK && Exclude == user_id) {
        json_object_object_add((*o_result_jso), RFPROP_USER_DELETEABLE, json_object_new_boolean(FALSE));
        json_object_object_add(*o_result_jso, RFPROP_USER_DELDISABLEREASON,
            json_object_new_string(RFPROP_USER_EMERGENCYUSER));
        return RET_OK;
    }
    // 判断是否为v3使能用户
    (void)dal_get_object_handle_nth(CLASS_TRAP_CONFIG, 0, &handle);
    (void)dal_get_property_value_byte(handle, PROPERTY_TRAP_VERSION, &Version);
    (void)dal_get_property_value_byte(handle, PROPERTY_TRAPV3_USERID, &Userid);
    if ((Version == 3) && (Userid == user_id)) {
        json_object_object_add((*o_result_jso), RFPROP_USER_DELETEABLE, json_object_new_boolean(FALSE));
        json_object_object_add(*o_result_jso, RFPROP_USER_DELDISABLEREASON,
            json_object_new_string(RFPROP_USER_TRAPV3USER));
        return RET_OK;
    }

    if (check_unique_admin(obj_handle) == TRUE) {
        json_object_object_add((*o_result_jso), RFPROP_USER_DELETEABLE, json_object_new_boolean(FALSE));
        json_object_object_add(*o_result_jso, RFPROP_USER_DELDISABLEREASON, 
            json_object_new_string(REPROP_USER_UNIQUEADMINUSER));
        return RET_OK;
    }

    json_object_object_add((*o_result_jso), RFPROP_USER_DELETEABLE, json_object_new_boolean(TRUE));
    json_object_object_add(*o_result_jso, RFPROP_USER_DELDISABLEREASON, NULL);
    return RET_OK;
}


LOCAL gint32 get_account_snmp_pwd_state(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
#define PASSWORD_NOT_INITIALIZED 1

    guint8 snmp_encrypt_pwd_init_state = PASSWORD_NOT_INITIALIZED;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_SNMPPRIVACYPWDINITIALSTATE,
        &snmp_encrypt_pwd_init_state);

    *o_result_jso = json_object_new_boolean(snmp_encrypt_pwd_init_state);

    return_val_do_info_if_expr((NULL == (*o_result_jso)), VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_boolean fail.", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL void get_account_snmp_auth_protocol(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 user_id)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint32 login_interface = 0;

    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:  get object handle for %s fail", __FUNCTION__, CLASS_SNMP);
        *o_result_jso = NULL;
        return;
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &login_interface);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s value fail.", __FUNCTION__, PROPERTY_USER_LOGIN_INTERFACE);
        *o_result_jso = NULL;
        return;
    }

    if ((login_interface & RFPROP_LOGIN_INTERFACE_SNMP_VALUE) != 0) {
        guchar authprotocol;

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_SNMPAUTHPROTOCOL, &authprotocol);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: get property %s value fail.", __FUNCTION__, PROPERTY_USER_SNMPAUTHPROTOCOL);
            *o_result_jso = NULL;
            return;
        }

        if (authprotocol == USE_MD5) {
            *o_result_jso = json_object_new_string(SNMP_AUTHPRO_MD5);
        } else if (authprotocol == USE_SHA) {
            *o_result_jso = json_object_new_string(SNMP_AUTHPRO_SHA);
        } else if (authprotocol == USE_SHA1) {
            *o_result_jso = json_object_new_string(SNMP_AUTHPRO_SHA1);
        } else if (authprotocol == USE_SHA256) {
            *o_result_jso = json_object_new_string(SNMP_AUTHPRO_SHA256);
        } else if (authprotocol == USE_SHA384) {
            *o_result_jso = json_object_new_string(SNMP_AUTHPRO_SHA384);
        } else if (authprotocol == USE_SHA512) {
            *o_result_jso = json_object_new_string(SNMP_AUTHPRO_SHA512);
        } else {
            debug_log(DLOG_ERROR, "%s: the snmp authentication algorithm does not support %d .", __FUNCTION__,
                authprotocol);
            *o_result_jso = NULL;
            return;
        }
    } else {
        *o_result_jso = NULL;
    }

    return;
}


LOCAL void get_snmpv3_priv_protocol(OBJ_HANDLE user_handle, json_object *o_result_jso)
{
    gint32 ret;
    guint8 priv_protocol = INVALID_DATA_BYTE;

    ret = dal_get_property_value_byte(user_handle, PROPERTY_USER_SNMPPRIVACYPROTOCOL, &priv_protocol);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s of %s failed, ret:%d", __FUNCTION__,
            PROPERTY_USER_SNMPPRIVACYPROTOCOL, dfl_get_object_name(user_handle), ret);
        json_object_object_add(o_result_jso, SNMP_V3PRIV_STR, NULL);
        return;
    }

    switch (priv_protocol) {
        case SNMP_PRIVPRO_VAL_DES:
            json_object_object_add(o_result_jso, SNMP_V3PRIV_STR, json_object_new_string(SNMP_PRIVPRO_DES));
            break;
        case SNMP_PRIVPRO_VAL_AES:
            json_object_object_add(o_result_jso, SNMP_V3PRIV_STR, json_object_new_string(SNMP_PRIVPRO_AES));
            break;
        case SNMP_PRIVPRO_VAL_AES256:
            json_object_object_add(o_result_jso, SNMP_V3PRIV_STR, json_object_new_string(SNMP_PRIVPRO_AES256));
            break;

        default:
            json_object_object_add(o_result_jso, SNMP_V3PRIV_STR, NULL);
    }

    return;
}


LOCAL void __get_account_oem_huawei_first_login(PROVIDER_PARAS_S *i_paras, guint8 user_id, json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    gint32 ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get User obj by UserId=%d failed, ret=%d", __FUNCTION__, user_id, ret);
        return;
    }

    guint8 account_init;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_USER_INITIALSTATE, &account_init);
    json_object *insecure_account_jso = json_object_new_boolean(account_init);
    json_object_object_add(o_result_jso, PFPROP_ACCOUNT_INSECURE_PROMPT_ENABLED, insecure_account_jso);

    guint8 first_login_plcy = 0;
    json_object *first_login_plcy_jso = NULL;
    if (i_paras->is_from_webui) {
        (void)dal_get_property_value_byte(obj_handle, PROP_USER_FIRST_LOGIN_POLICY, &first_login_plcy);
    } else {
        (void)dal_get_property_value_byte(obj_handle, PROP_USER_FIRST_M2MLOGIN_POLICY, &first_login_plcy);
    }
    switch (first_login_plcy) {
        case FIRST_LOGIN_PLCY_PROMPT:
            first_login_plcy_jso = json_object_new_string(FIRST_LOGIN_PLCY_PROMPT_STR);
            break;
        case FIRST_LOGIN_PLCY_FORCE:
            first_login_plcy_jso = json_object_new_string(FIRST_LOGIN_PLCY_FORCE_STR);
            break;
        default:
            first_login_plcy_jso = NULL;
            break;
    }

    json_object_object_add(o_result_jso, RF_ACCOUNT_FIRST_LOGIN_PLCY, first_login_plcy_jso);
}


LOCAL gint32 get_account_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 *user_id)
{
    
    guint8 local_user_id = (*user_id);
    json_object *macc_result_jso = NULL;             // 双因素客户端 值的 json形式
    json_object *sshpkh_result_jso = NULL;           // ssh公钥哈希 值的 json形式
    json_object *longin_interface_result_jso = NULL; // Logininterface 值的json形式
    json_object *snmp_auth_proto_result_jso = NULL;  // SNMPV3AuthProtocol 值的json形式
    json_object *snmp_encrypt_pwd_init_jso = NULL;   // snmp的加密密码是否被初始化
    json_object *longin_rule_jso = NULL;             // Logininterface 值的json形式
    json_object *actions_result_jso = NULL;          // actions 值的json形式
    guint32 pwd_valid_days = 0;
    gint32 ret;

    OBJ_HANDLE obj_handle = 0;
    
    // 检查不过返回VOS_ERR，内部错误，不填充消息体*/
    if (o_result_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
        return RET_ERR;
    }

    
    // 获取属性"MutualAuthenticationCliCert "的值，调用方法get_account_MutualAuthenticationCliCer(i_paras,
    // MACliCert_result_jso) 用户需要有权限USERROLE_USERMGNT，才能查看双因素证书

    
    macc_result_jso = NULL;

    if ((USERROLE_USERMGNT & i_paras->user_role_privilege)) {
        (void)get_account_mutualauthentication_clicert(i_paras, &macc_result_jso, &local_user_id);
    }
    

    __get_account_oem_huawei_first_login(i_paras, local_user_id, *o_result_jso);
    

    
    
    (void)dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, local_user_id, &obj_handle);
    (void)get_account_snmp_pwd_state(obj_handle, &snmp_encrypt_pwd_init_jso);
    

    
    // 获取属性"PublickeyHash "的值，调用方法get_account_MutualAuthenticationCliCer(i_paras, MACliCert_result_jso)
    (void)get_account_ssh_publickey_hash(i_paras, &sshpkh_result_jso, &local_user_id);

    
    // 获取属性"Logininterface "的值，调用方法get_account_MutualAuthenticationCliCer(i_paras, MACliCert_result_jso)
    (void)get_account_login_interface(i_paras, &longin_interface_result_jso, &local_user_id);

    get_account_snmp_auth_protocol(i_paras, &snmp_auth_proto_result_jso, local_user_id);

    
    (void)get_account_login_rule(i_paras, &longin_rule_jso, local_user_id);
    
    
    // 获取属性"Actions "的值，调用方法get_mutual_auth_client_certificate_actions(i_paras, MACliCert_result_jso)
    (void)get_mutual_auth_client_certificate_actions(i_paras, &actions_result_jso, &local_user_id);

    
    // 将属性"MutualAuthenticationCliCert"的值填入到o_result_jso 中的相应的属性"MutualAuthenticationCliCert"
    // 用户只有拥有管理权限才能显示该属性
    json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT, (macc_result_jso));
    // 将获得的结果添加入o_result_jso 中的属性"MutualAuthenticationCliCert" 对应的值
    json_object_object_add((*o_result_jso), RFPROP_SSHPK_HASH, (sshpkh_result_jso));
    // 将获得的结果添加入o_result_jso 中的属性"Logininterface" 对应的值
    json_object_object_add((*o_result_jso), RFPROP_LOGIN_INTERFACE, (longin_interface_result_jso));
    // 将获得的结果添加入o_result_jso 中的属性"SnmpV3AuthProtocol" 对应的值
    json_object_object_add((*o_result_jso), SNMP_V3AUTH_STR, snmp_auth_proto_result_jso);
    json_object_object_add((*o_result_jso), SNMP_V3PRIVPASSWD, NULL);
    get_snmpv3_priv_protocol(obj_handle, *o_result_jso);
    // 将获得的结果添加入o_result_jso 中的属性"SNMPEncryptPwdInit" 对应的值
    json_object_object_add((*o_result_jso), RFPROP_SNMP_ENCRYPT_PWD_INIT, (snmp_encrypt_pwd_init_jso));
    
    (void)get_user_deleteable(obj_handle, i_paras, o_result_jso);
    
    json_object_object_add((*o_result_jso), RFPROP_LOGIN_RULE, (longin_rule_jso));
    
    ret = get_pwd_valid_days(&pwd_valid_days, local_user_id);
    
    debug_log(DLOG_DEBUG, "pwd_valid_days = %d", pwd_valid_days);
    if (ret != RET_OK || pwd_valid_days == 0xffff) {
        json_object_object_add((*o_result_jso), RFPROP_PWD_VALID_DAYS, NULL);
    } else {
        json_object_object_add((*o_result_jso), RFPROP_PWD_VALID_DAYS, json_object_new_int(pwd_valid_days));
    }
    
    // 将获得的结果添加入o_result_jso 中的属性"Actions" 对应的值
    json_object_object_add((*o_result_jso), RFPROP_ACTIONS, (actions_result_jso));

    // 返回结果
    return RET_OK;
}


LOCAL gint32 rfaccount_string_check(const gchar *data)
{
    if (NULL == data || 0 == strlen(data) || 0 == g_strcmp0("N/A", data) || 0 == g_strcmp0("Unknown", data)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 get_certinfo_from_mutualauth_class(guint8 hash_id, CERT_INFO_T *cert_info, guint8 *issuer_id,
    gchar *finger, guint32 finger_Len)
{
    gint32 ret;
    guint32 hash_val = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    ret = dal_get_specific_object_byte(MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_HASHID, hash_id,
        &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get object handle fail, ret = %d", __FUNCTION__, ret));

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_ISSUEBY, cert_info->issuer_info,
        sizeof(cert_info->issuer_info));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_ISSUETO, cert_info->subject_info,
        sizeof(cert_info->subject_info));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_VALIDFROM, cert_info->not_before,
        sizeof(cert_info->not_before));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_VALIDTO, cert_info->not_after,
        sizeof(cert_info->not_after));
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_SERIALNUMBER, cert_info->serial,
        sizeof(cert_info->serial));
    (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_CLIENT_CERT_ISSUERID, issuer_id);
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_FINGERPRINT, finger, finger_Len);
    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_SIGALGO, cert_info->sig_algo,
        sizeof(cert_info->sig_algo));

    if (rfaccount_string_check(cert_info->issuer_info) != VOS_OK ||
        rfaccount_string_check(cert_info->subject_info) != VOS_OK ||
        rfaccount_string_check(cert_info->not_before) != VOS_OK ||
        rfaccount_string_check(cert_info->not_after) != VOS_OK || rfaccount_string_check(cert_info->serial) != VOS_OK ||
        rfaccount_string_check(finger) != VOS_OK) {
        return VOS_ERR;
    }

    if (!g_strcmp0(cert_info->sig_algo, "")) {
        // 证书没有初始化，需要重新初始化
        (void)dal_get_property_value_uint32(obj_handle, MUTUAL_AUTH_CLIENT_CERT_HASHVALUE, &hash_val);
        input_list = g_slist_append(input_list, g_variant_new_byte(hash_id));
        input_list = g_slist_append(input_list, g_variant_new_uint32(hash_val));
        ret = dfl_call_class_method(obj_handle, MUTUAL_AUTH_CLIENT_CERT_METHOD_REINIT, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_SIGALGO, cert_info->sig_algo,
            sizeof(cert_info->sig_algo));
    }

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_KEYUSAGE, cert_info->key_usage,
        sizeof(cert_info->key_usage));
    (void)dal_get_property_value_uint32(obj_handle, MUTUAL_AUTH_CLIENT_CERT_KEYLENGTH, &cert_info->key_len);

    return VOS_OK;
}


LOCAL void get_ma_client_cert_revoke_info(guchar hash_id, json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar revoked_state = 0;
    gchar revoked_date[CERT_ITEM_LEN_128] = {0};

    return_do_info_if_expr(o_result_jso == NULL, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    ret = dal_get_specific_object_byte(MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_HASHID, hash_id,
        &obj_handle);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get object handle fail, ret = %d", __FUNCTION__, ret));

    (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_CLIENT_CERT_REVOKED_STATE, &revoked_state);
    if (revoked_state == ENABLED) {
        json_object_object_add(o_result_jso, MUTUAL_AUTH_CLIENT_CERT_REVOKED_STATE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(o_result_jso, MUTUAL_AUTH_CLIENT_CERT_REVOKED_STATE, json_object_new_boolean(FALSE));
    }

    (void)dal_get_property_value_string(obj_handle, MUTUAL_AUTH_CLIENT_CERT_REVOKED_DATE, revoked_date,
        sizeof(revoked_date));
    if (rfaccount_string_check(revoked_date) == VOS_OK) {
        json_object_object_add(o_result_jso, MUTUAL_AUTH_CLIENT_CERT_REVOKED_DATE,
            json_object_new_string((const gchar *)revoked_date));
    } else {
        json_object_object_add(o_result_jso, MUTUAL_AUTH_CLIENT_CERT_REVOKED_DATE, NULL);
    }

    return;
}


LOCAL gint32 get_account_mutualauthentication_clicert(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso,
    guint8 *user_id)
{
    gint32 ret;                        // 返回值
    guint8 local_user_id = (*user_id); // 用户的ID
    OBJ_HANDLE obj_product_handle = 0; // 类MutualAuthenticationCliCert 的对象的句柄
    guint8 ma_enable = 0;
    CERT_INFO_T cert_info;
    gchar finger[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN]; // finger_print 值
    guint8 issuerid = 0;

    
    return_val_do_info_if_expr((o_result_jso == NULL || provider_paras_check(i_paras) != VOS_OK), VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    *o_result_jso = NULL;

    
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    // 获取类"MutualAuthenticationEnable"的值
    ret = dal_get_property_value_byte(obj_product_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &(ma_enable));
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    if (ma_enable == 1) {
        // 单板支持双因素认证
        ret = get_certinfo_from_mutualauth_class(local_user_id, &cert_info, &issuerid, finger, sizeof(finger));
        return_val_if_expr(ret != VOS_OK, VOS_ERR);
        *o_result_jso = json_object_new_object();
        return_val_do_info_if_expr((*o_result_jso == NULL), VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d 'json_object_new_object fail' ", __FUNCTION__, __LINE__));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_ISSUEBY,
            json_object_new_string(cert_info.issuer_info));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_ISSUTO,
            json_object_new_string(cert_info.subject_info));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_VALIDFROM,
            json_object_new_string(cert_info.not_before));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_VALIDTO, json_object_new_string(cert_info.not_after));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_SN, json_object_new_string(cert_info.serial));

        if (issuerid > 0) {
            json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_ROOT_STATE, json_object_new_boolean(TRUE));
        } else {
            json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_ROOT_STATE, json_object_new_boolean(FALSE));
        }

        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_FINGER_PRINT,
            json_object_new_string((const gchar *)finger));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_SIGALGO, json_object_new_string(cert_info.sig_algo));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_KEYUSAGE,
            json_object_new_string(cert_info.key_usage));
        json_object_object_add((*o_result_jso), RFPROP_MUTAU_CERT_KEYLENGTH, json_object_new_int(cert_info.key_len));

        
        get_ma_client_cert_revoke_info(local_user_id, *o_result_jso);
        return VOS_OK;
    }

    return VOS_ERR;
}


LOCAL gint32 get_account_ssh_publickey_hash(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 *user_id)
{
    
    gint32 ret;                                  // 返回值
    guint8 local_user_id = (*user_id);                     // 用户的ID
    OBJ_HANDLE obj_handle = 0;                             // 类User 的对象的句柄
    gchar pkh_string[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN]; // PublickeyHash 值

    
    // 1. 检查不过返回VOS_ERR，内部错误，不填充消息体*/
    if (NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    (void)memset_s(pkh_string, sizeof(pkh_string), 0, sizeof(pkh_string)); // 字符数组初始化

    
    // 获取类"User" 对象的句柄
    // MUTUAL_AUTH_CLIENT_CLASS_NAME

    
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, local_user_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, (*o_result_jso) = NULL;
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));
    

    // 获取类对象中属性"PublickeyHash"的值
    // "PublickeyHash"的的宏定义为:  PROPERTY_USER_PUBLICKEY_HASH
    ret = dal_get_property_value_string(obj_handle, PROPERTY_USER_PUBLICKEY_HASH, pkh_string, sizeof(pkh_string));
    if (VOS_OK != ret) {
        *o_result_jso = NULL;
        
        return VOS_ERR;
    } else {
        // 属性值检查或转换
        if (rfaccount_string_check(pkh_string) != VOS_OK) {
            *o_result_jso = NULL;
            return VOS_ERR;
        } else {
            // 为属性"PublickeyHash"的值分配内存空间
            (*o_result_jso) = json_object_new_string((const gchar *)pkh_string);

            // 对分配空间的结果进行判断
            return_val_do_info_if_fail((NULL != (*o_result_jso)), VOS_ERR,
                debug_log(DLOG_ERROR, "%s, %d 'json_object_new_object fail' ", __FUNCTION__, __LINE__));
            
            return VOS_OK;
        }
    }
}


LOCAL gint32 get_account_login_interface(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso, guint8 *user_id)
{
    
    gint32 ret;                                                    // 返回值
    guint8 local_user_id = (*user_id);                                       // 用户的ID
    OBJ_HANDLE obj_handle = 0;                                               // 类User 的对象的句柄
    guint32 login_interface;                                                 // Logininterface 值
    guint32     login_interface_array[RFPROP_LOGIN_INTERFACE_ARRAY_LEN] = {0}; // Logininterface的10进值对应的2进制
    guint32 i;                                                               // login_interface_array数组的下标
    guint32 base;                                                            // 底数，用来计算base的任意次幂

    
    // 1. 检查不过返回VOS_ERR，内部错误，不填充消息体*/
    if (NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    login_interface = 0;

    base = 1; // 初值为1

    
    // 获取类"User" 对象的句柄
    // MUTUAL_AUTH_CLIENT_CLASS_NAME
    
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, local_user_id, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, (*o_result_jso) = NULL;
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));
    

    // 获取类对象中属性"Logininterface"的值
    // "Logininterface"的的宏定义为:	PROPERTY_USER_PUBLICKEY_HASH
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &login_interface);
    if (VOS_OK != ret) {
        *o_result_jso = NULL;
        return VOS_ERR;
    } else {
        // 为属性"Logininterface"的值分配内存空间
        (*o_result_jso) = json_object_new_array();
        return_val_do_info_if_fail((NULL != (*o_result_jso)), VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d 'json_object_new_object fail' ", __FUNCTION__, __LINE__));

        // 解析"Logininterface"的值为相应的二进制,  并将二进制的每一位转换成为相应字符串,并赋给*o_result_jso 指向的空间
        // Web :  1//SNMP : 2  //IPMI :  4 //SSH:   8  //SFTP: 16 //Local: 64 //Redfish : 128
        login_interface = login_interface & USER_LOGIN_INTERFACE_VALUE_MASK;

        for (i = 0; i < RFPROP_LOGIN_INTERFACE_ARRAY_LEN; i++) {
            login_interface_array[i] = login_interface & base;
            base *= 2; // 底数自乘以2

            if (0 != login_interface_array[i] && i != 5) {
                switch (i) {
                    case 0:
                        ret = json_object_array_add(*o_result_jso, json_object_new_string(RFPROP_LOGIN_INTERFACE_WEB));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;

                    case 1:
                        ret = json_object_array_add(*o_result_jso, json_object_new_string(RFPROP_LOGIN_INTERFACE_SNMP));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;

                    case 2:
                        ret = json_object_array_add(*o_result_jso, json_object_new_string(RFPROP_LOGIN_INTERFACE_IPMI));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;

                    case 3:
                        ret = json_object_array_add(*o_result_jso, json_object_new_string(RFPROP_LOGIN_INTERFACE_SSH));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;

                    case 4:
                        ret = json_object_array_add(*o_result_jso, json_object_new_string(RFPROP_LOGIN_INTERFACE_SFTP));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;

                    case 6:
                        ret =
                            json_object_array_add(*o_result_jso, json_object_new_string(RFPROP_LOGIN_INTERFACE_LOCAL));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;

                    case 7:
                        ret = json_object_array_add(*o_result_jso,
                            json_object_new_string(RFPROP_LOGIN_INTERFACE_REDFISH));
                        return_val_do_info_if_fail(0 == ret, VOS_ERR, json_object_put(*o_result_jso);
                            (*o_result_jso) = NULL;
                            debug_log(DLOG_ERROR, "%s, %d: 'put array element fail.'\n", __FUNCTION__, __LINE__));
                        break;
                }
            }
        }

        
        return VOS_OK;
    }
}


LOCAL void add_action_prop(json_object *action_prop_jso, const gchar *action_name, const gchar *target,
    const gchar *target_info)
{
    json_object *single_action_jso = NULL;
    gchar action_name_pointer[MAX_URI_LENGTH] = {0};

    single_action_jso = json_object_new_object();
    if (single_action_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new json object failed");
        return;
    }

    if (snprintf_s(action_name_pointer, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "#%s", action_name) <= 0) {
        (void)json_object_put(single_action_jso);
        return;
    }

    // 形成"target"的值single_action_jso
    json_object_object_add(single_action_jso, RFPROP_TARGET, json_object_new_string(target));

    // 形成"info"的值single_action_info_jso
    json_object_object_add(single_action_jso, RFPROP_ACTION_INFO, json_object_new_string(target_info));

    // 将"#属性名" 的值single_action_jso加到"#属性名" 中
    json_object_object_add(action_prop_jso, (const gchar *)action_name_pointer, single_action_jso);

    return;
}


LOCAL gint32 get_mutual_auth_client_certificate_actions(PROVIDER_PARAS_S *i_paras, json_object **o_result_jso,
    guint8 *user_id)
{
    gchar        change_uri[MAX_URI_LENGTH] = {0};
    gchar        change_uri_info[MAX_URI_LENGTH] = {0};
    gchar        local_user_id[MAX_RSC_ID_LEN] = {0};
    _cleanup_json_object_ json_object *action_jso = NULL;

    
    if (o_result_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    // 分配空间
    action_jso = json_object_new_object();
    if (action_jso == NULL) {
        debug_log(DLOG_ERROR, "alloc new object failed");
        return VOS_ERR;
    }

    // 形成字符形式的user_id
    if (snprintf_s(local_user_id, sizeof(local_user_id), sizeof(local_user_id) - 1, "%d", (*user_id)) <= 0) {
        return VOS_ERR;
    }

    // 添加具体的属性
    // "ImportMutualAuthClientCert"
    // 用户只有拥有管理权限才能显示该属性
    (void)memset_s(change_uri, sizeof(change_uri), 0, sizeof(change_uri));

    if (snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, ACTION_IMPORT_MUTAU_CERT, 
        i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    (void)memset_s(change_uri_info, sizeof(change_uri_info), 0, sizeof(change_uri_info));

    if (snprintf_s(change_uri_info, sizeof(change_uri_info), sizeof(change_uri_info) - 1, 
        ACTION_INFO_IMPORT_MUTAU_CERT, i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    add_action_prop(action_jso, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, change_uri, change_uri_info);

    // "DeleteMutualAuthClientCert"
    (void)memset_s(change_uri, sizeof(change_uri), 0, sizeof(change_uri));

    if (snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, ACTION_DELETE_MUTAU_CERT, 
        i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    (void)memset_s(change_uri_info, sizeof(change_uri_info), 0, sizeof(change_uri_info));

    if (snprintf_s(change_uri_info, sizeof(change_uri_info), sizeof(change_uri_info) - 1, 
        ACTION_INFO_DELETE_MUTAU_CERT, i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    add_action_prop(action_jso, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, change_uri, change_uri_info);
    // "info"

    // "ImportSSHPublicKey"
    (void)memset_s(change_uri, sizeof(change_uri), 0, sizeof(change_uri));

    if (snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, ACTION_IMPORT_SSH_PUBLIC_KEY,
        i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    (void)memset_s(change_uri_info, sizeof(change_uri_info), 0, sizeof(change_uri_info));

    if (snprintf_s(change_uri_info, sizeof(change_uri_info), sizeof(change_uri_info) - 1,
        ACTION_INFO_IMPORT_SSH_PUBLIC_KEY, i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    add_action_prop(action_jso, RFACTION_IMPORT_SSH_PUBLICKEY, change_uri, change_uri_info);
    // "info"

    // "DeleteSSHPublicKey"
    (void)memset_s(change_uri, sizeof(change_uri), 0, sizeof(change_uri));

    if (snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, ACTION_DELETE_SSH_PUBLIC_KEY,
        i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    (void)memset_s(change_uri_info, sizeof(change_uri_info), 0, sizeof(change_uri_info));

    if (snprintf_s(change_uri_info, sizeof(change_uri_info), sizeof(change_uri_info) - 1,
        ACTION_INFO_DELETE_SSH_PUBLIC_KEY, i_paras->member_id) <= 0) {
        return VOS_ERR;
    }

    add_action_prop(action_jso, RFACTION_DELETE_SSH_PUBLICKEY, change_uri, change_uri_info);

    *o_result_jso = TAKE_PTR(action_jso);
    return VOS_OK;
}


LOCAL gint32 redfish_form_memberid_action_info(PROVIDER_PARAS_S *i_paras)
{
    gchar *uri_lower = g_ascii_strdown(i_paras->uri, strlen(i_paras->uri));

    if (uri_lower == NULL) {
        debug_log(DLOG_ERROR, "%s:  change uri to lower fail", __FUNCTION__);
        return RET_ERR;
    }

    if (strncpy_s(i_paras->uri, MAX_URI_LENGTH, uri_lower, strlen(uri_lower)) != 0) {
        g_free(uri_lower);
        return RET_ERR;
    }

    const gchar *pattern_str = "/redfish/v1/accountservice/accounts/%d/%s";
    const gint32 URI_FORMAT_CNT = 2;  // 固定格式"/redfish/v1/accountservice/accounts/%d/%s"的格式化输出数量 2
    gchar action_name[MAX_ACTION_NAME_LEN] = {0};
    gint32 count_id = 0;

    gint32 securec_rv = sscanf_s(uri_lower, pattern_str, &count_id, action_name, sizeof(action_name) - 1);
    g_free(uri_lower);

    if (securec_rv != URI_FORMAT_CNT) {
        debug_log(DLOG_ERROR, "%s:sscanf_s failed, ret is %d.", __FUNCTION__, securec_rv);
        return RET_ERR;
    }

    snprintf_s(i_paras->member_id, sizeof(i_paras->member_id), sizeof(i_paras->member_id) - 1, "%d", count_id);
    return RET_OK;
}


LOCAL gint32 redfish_form_memberid_effective(PROVIDER_PARAS_S *i_paras)
{
    return redfish_form_memberid_action_info(i_paras);
}



LOCAL gint32 get_action_info_odata_id_account(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 ret_mi_check;
    gchar  username[USER_USERNAME_MAX_LEN + 1] = {0};
    gchar  name_action_info[MAX_CHARACTER_NUM + 1] = {0};
    gchar *flag = NULL;
    gchar  uri[MAX_URI_LENGTH] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    // 形成合法的member_id
    ret_mi_check = redfish_form_memberid_action_info(i_paras);
    return_val_do_info_if_fail(VOS_ERR != ret_mi_check, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: update member_id fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), NULL,
        o_message_jso);
    return_val_if_fail((VOS_OK == ret), ret);

    if (0 == i_paras->is_satisfy_privi) {
        
        ret = g_strcmp0(i_paras->user_name, username);
        return_val_do_info_if_fail(RF_MATCH_OK == ret, HTTP_FORBIDDEN,
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFPROP_ACCOUNT_ODATA_ID, o_message_jso));
    }

    
    flag = g_strrstr(i_paras->uri, "/");
    return_val_do_info_if_fail(NULL != flag, VOS_ERR, debug_log(DLOG_ERROR, "%s failed: g_strrstr", __FUNCTION__));
    flag++;

    if (0 >= snprintf_s(name_action_info, sizeof(name_action_info), sizeof(name_action_info) - 1, "%s", flag)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    if (0 == g_strcmp0(name_action_info, ACTION_INFO_NAME_LOWER_IMPORT_MUTUAL_CERT)) {
        if (0 >= snprintf_s(name_action_info, sizeof(name_action_info), sizeof(name_action_info) - 1, "%s",
            ACTION_INFO_NAME_UPPER_IMPORT_MUTUAL_CERT)) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    if (0 == g_strcmp0(name_action_info, ACTION_INFO_NAME_LOWER_DELETE_MUTUAL_CERT)) {
        if (0 >= snprintf_s(name_action_info, sizeof(name_action_info), sizeof(name_action_info) - 1, "%s",
            ACTION_INFO_NAME_UPPER_DELETE_MUTUAL_CERT)) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    if (0 == g_strcmp0(name_action_info, ACTION_INFO_NAME_LOWER_IMPORT_SSH_PUBLIC)) {
        if (0 >= snprintf_s(name_action_info, sizeof(name_action_info), sizeof(name_action_info) - 1, "%s",
            ACTION_INFO_NAME_UPPER_IMPORT_SSH_PUBLIC)) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    if (0 == g_strcmp0(name_action_info, ACTION_INFO_NAME_LOWER_DELETE_SSH_PUBLIC)) {
        if (0 >= snprintf_s(name_action_info, sizeof(name_action_info), sizeof(name_action_info) - 1, "%s",
            ACTION_INFO_NAME_UPPER_DELETE_SSH_PUBLIC)) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    

    if (0 >= snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, "%s/%s/%s", ACCOUNTSERVICE_ACCOUNTS, i_paras->member_id,
        name_action_info)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)uri);
    return HTTP_OK;
}


LOCAL gint32 act_account_import_client_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    
    const gchar *client_cert_type_str = NULL;
    const gchar *client_cert_value_str = NULL;
    json_object *cert_type_jso = NULL;
    json_object *cert_value_jso = NULL;
    json_bool ret_bool;
    gboolean ret_file_bool = FALSE;
    gint32 ret_uri = 0; // URI 函数调用的返回值
    guint32 client_cert_value_len = 0;
    OBJ_HANDLE obj_user_handle = 0; // 用户类的对象的句柄
    OBJ_HANDLE obj_product_handle = 0;
    guint8 ma_enable = 0;       // 单板是否支持双因素认证
    OBJ_HANDLE obj_macc_handle; // 双因素客户端类的对象的句柄
    guint8 user_id;             // 用户的的ID
    gchar cert_temp_path[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN]; // 证书文件所在的路径
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;
    
    gboolean b_ret;
    
    
    // 形成合法的member_id
    gint32 ret_mi_check = redfish_form_memberid_effective(i_paras);
    return_val_do_info_if_fail(VOS_ERR != ret_mi_check, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: update member_id fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));

    guint32 ret =
        redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, NULL, 0, &obj_user_handle, o_message_jso);
    do_info_if_true(ret == HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));
    return_val_if_fail(ret == VOS_OK, ret);

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 单板是否支持双因素认证
    // 获取类"PRODUCT"的句柄
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product_handle);
    return_val_do_info_if_fail(ret != VOS_ERR, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));
    // 获取类"MutualAuthenticationEnable"的值
    ret = dal_get_property_value_byte(obj_product_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &(ma_enable));
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    if (ma_enable == 0) {
        (void)create_message_info(MSGID_IMPORT_CLICERT_NOT_SUPPORTED_ERR, NULL, o_message_jso);
        
        return HTTP_NOT_IMPLEMENTED;
        
    }

    
    b_ret = check_redfish_running_task_exist(import_status_monitor, i_paras->val_jso, i_paras->uri, NULL);
    return_val_do_info_if_fail(FALSE == b_ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);
        (void)create_message_info(MSGID_CERT_IMPORTING, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));
    

    
    // 获取属性"CertificateType"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_TYPE, &cert_type_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != cert_type_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_PARAM_TYPE, o_message_jso));

    client_cert_type_str = dal_json_object_get_str(cert_type_jso);
    
    return_val_do_info_if_fail(NULL != client_cert_type_str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));
    

    // 获取属性"Certificate"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &cert_value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != cert_value_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_PARAM_CONTENT, o_message_jso));

    client_cert_value_str = dal_json_object_get_str(cert_value_jso);
    
    return_val_do_info_if_fail(NULL != client_cert_value_str, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));
    

    
    if (g_strcmp0(client_cert_type_str, RFACTION_FORMAT_TEXT) == 0) { // 证书以文本的形式存在
        // 将文本保存为文件
        client_cert_value_len = strlen(client_cert_value_str);
        if (client_cert_value_len > IMPORT_CERTIFICATE_MAX_LEN) {
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            return HTTP_BAD_REQUEST;
        }

        ret_file_bool =
            g_file_set_contents(TMP_CLIENT_PEM_FILE, (const gchar *)client_cert_value_str, client_cert_value_len, NULL);
        return_val_do_info_if_fail(ret_file_bool, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "write request cert to %s failed", TMP_CLIENT_PEM_FILE);
            (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));
    } else if (g_strcmp0(client_cert_type_str, RFACTION_FORMAT_FILE) == 0) { // 证书以文件的形式存在
        
        b_ret = g_regex_match_simple(SERVER_CERT_IMPORT_REGEX, client_cert_value_str, G_REGEX_OPTIMIZE,
            (GRegexMatchFlags)0);
        return_val_do_info_if_fail(b_ret == TRUE, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_PARAM_CONTENT, o_message_jso, RF_SENSITIVE_INFO,
            RFACTION_PARAM_CONTENT));
        

        // 判断文件的存在于本地还是远程文件服务器
        // 增加远程文件下载的方法或者获取证书文件路径
        // 证书文件导入
        // 判断是本地导入还是远程文件导入
        if (client_cert_value_str[0] == '/') {
            // 本地文件导入，拷贝Content中的内容到指定的临时文件夹TMP_CLIENT_PEM_FILE  "/tmp/web/client_tmp.cer"
            
            ret = dal_check_real_path(client_cert_value_str);
            if (ret != VOS_OK) {
                
                debug_log(DLOG_ERROR, "%s :Cert file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
                (void)create_message_info(MSGID_CERT_IMPORT_FAILED, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
                
                return HTTP_BAD_REQUEST;
            }

            if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, client_cert_value_str,
                (const gchar *)i_paras->user_roleid)) {
                (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
                return HTTP_BAD_REQUEST;
            }

            

            
            if (g_strcmp0(TMP_CLIENT_PEM_FILE, client_cert_value_str) != 0) {
                // redfish模块降权限后，调用代理方法拷贝文件
                ret =
                    proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, client_cert_value_str,
                    TMP_CLIENT_PEM_FILE, REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
                // 拷贝失败，需要删除临时文件
                if (ret != VOS_OK) {
                    debug_log(DLOG_ERROR, "%s: copy request cert to %s failed", __FUNCTION__, TMP_CLIENT_PEM_FILE);
                    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                        client_cert_value_str);
                    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                        TMP_CLIENT_PEM_FILE);
                    (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
                    return HTTP_INTERNAL_SERVER_ERROR;
                }
            }

            
        } else {
            // 远程文件导入
            ret_uri = act_account_import_file_uri(i_paras, NULL, o_message_jso, o_result_jso);
            return ret_uri;
        }
    } // else

    
    (void)dal_set_file_owner(TMP_CLIENT_PEM_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_CLIENT_PEM_FILE, (S_IRUSR | S_IWUSR));

    
    // 入 参1: 获取用户的ID
    ret = dal_get_property_value_byte(obj_user_handle, PROPERTY_USER_ID, &user_id);
    return_val_do_info_if_fail(VOS_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));

    // 入参2 : 获取用户的ID对应的双因素客户端证书对象的句柄
    
    ret = dal_get_specific_object_byte(MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_HASHID, user_id,
        &obj_macc_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj handle fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso));
    

    // 入参3 : 证书临时文件所在的路径
    safe_fun_ret = strcpy_s(cert_temp_path, sizeof(cert_temp_path), TMP_CLIENT_PEM_FILE);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    // 将入参user_id 和临时客户端证书文件的路径填入到inputlist表中
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)cert_temp_path));

    // 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_macc_handle,
        MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_METHOD_IMPORT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);

    
    // 对UIP的返回值进行判断
    do_val_if_fail(ret == VOS_OK,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", MUTUAL_AUTH_CLIENT_CERT_METHOD_IMPORT, ret));

    // 对类方法ImportClientCertificate 返回值进行判断
    // 返回值的低两位为返回码
    switch (ret) {
            
        case VOS_OK:
        case VOS_ERR_MA_IDEL:
            ret = HTTP_OK;
            (void)create_message_info(MSGID_CLICERT_IMPORT_OK, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            break;

            
            
        case VOS_ERR_MA_FORMAT_ERR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CERTIFICATE_FORMAT_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            break;
            

            
        case VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR:
            
        case VOS_ERR_MA_BASIC_CONSTRAINTS_ERR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CLICERT_CONSTRAINTS_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT,
                o_message_jso);
            break;

            
        case VOS_ERR_MA_CERT_EXSIT:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CLICERT_CERT_EXSIT, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;

            
        case VOS_ERR_MA_NO_ISSUER:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CLICERT_NO_ISSUER, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            break;

            
        case VOS_ERR_MA_REACH_MAX:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CLICERT_REACH_MAX, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            break;

            
        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
            debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
            break;
    }

    return ret;
}


LOCAL gint32 act_account_import_client_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    
    if (i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param is invalid", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param is invalid", __FUNCTION__);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto EXIT;
    }

    
    if ((USERROLE_USERMGNT & i_paras->user_role_privilege) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_MUTAU_CERT_IMPORT_CLI_CERT, o_message_jso);
        ret = HTTP_FORBIDDEN;
        goto EXIT;
    }

    ret = act_account_import_client_cert(i_paras, o_message_jso, o_result_jso);

EXIT:

    
    if (ret != HTTP_ACCEPTED) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }

    return ret;
}


LOCAL void clear_ssh_publickey_file(PROVIDER_PARAS_S *i_paras)
{
    json_bool ret_bool;
    const gchar *public_key_value_str = NULL;
    const gchar *public_key_type_str = NULL;
    json_object *public_key_type_jso = NULL;
    json_object *public_key_value_jso = NULL;
    gint32 ret;

    
    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, TMP_SSH_PUBLIC_KEY_PUB_FILE);

    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_TYPE, &public_key_type_jso);
    if (ret_bool != TRUE || public_key_type_jso == NULL) {
        debug_log(DLOG_ERROR, "%s : get property type fail", __FUNCTION__);
        return;
    }

    public_key_type_str = dal_json_object_get_str(public_key_type_jso);
    if (public_key_type_str == NULL) {
        debug_log(DLOG_ERROR, "%s : get type value fail", __FUNCTION__);
        return;
    }

    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &public_key_value_jso);
    if (ret_bool != TRUE || public_key_value_jso == NULL) {
        debug_log(DLOG_ERROR, "%s : get property content fail", __FUNCTION__);
        return;
    }

    public_key_value_str = dal_json_object_get_str(public_key_value_jso);
    if (public_key_value_str == NULL) {
        debug_log(DLOG_ERROR, "%s : get content value fail", __FUNCTION__);
        return;
    }

    if (g_strcmp0(public_key_type_str, RFACTION_FORMAT_FILE) == 0) {
        
        if (public_key_value_str[0] == '/') {
            
            ret = dal_check_real_path(public_key_value_str);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s: invalid file path", __FUNCTION__);
                return;
            }

            
            if (dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, public_key_value_str,
                (const gchar *)i_paras->user_roleid) != TRUE) {
                debug_log(DLOG_ERROR, "%s: not file owner", __FUNCTION__);
                return;
            }

            
            if (strcmp(public_key_value_str, TMP_SSH_PUBLIC_KEY_PUB_FILE) != 0) {
                (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    public_key_value_str);
            }
        }
    }
    return;
}



LOCAL gint32 act_account_import_ssh_publickey(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    errno_t safe_fun_ret;
    
    const gchar *public_key_type_str = NULL;
    const gchar *public_key_value_str = NULL;
    json_object *public_key_type_jso = NULL;
    json_object *public_key_value_jso = NULL;
    json_bool ret_bool;
    gboolean ret_file_bool = FALSE;
    guint32 public_key_value_len = 0;
    OBJ_HANDLE obj_user_handle = 0;             // 用户类的对象的句柄
    guint8 user_id;                             // 用户的的ID
    gchar user_name[USER_USERNAME_MAX_LEN + 1]; // 用户id对应的用户名
    gchar public_key_temp_path[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN];
    guint8 Keytype; // ssh公钥存在的方式: 文本，文件，待删除
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;
    
    // 形成合法的member_id
    gint32 ret_mi_check = redfish_form_memberid_effective(i_paras);
    return_val_do_info_if_fail(ret_mi_check == VOS_OK, HTTP_INTERNAL_SERVER_ERROR, clear_ssh_publickey_file(i_paras);
        debug_log(DLOG_ERROR, "%s, %d: update member_id fail.\n", __FUNCTION__, __LINE__));

    gint32 ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, user_name, sizeof(user_name),
        &obj_user_handle, o_message_jso);
    do_info_if_true(ret == HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso));
    if (ret != VOS_OK) {
        clear_ssh_publickey_file(i_paras);
        return ret;
    }

    
    // 当前登录的用户只能给本身添加SSH公钥，不能给其他用户添加
    
    ret = _rf_check_user_configure_priv(obj_user_handle, i_paras->is_from_webui, i_paras->user_role_privilege,
        i_paras->user_name, user_name);
    if (ret != RET_OK) {
        clear_ssh_publickey_file(i_paras);
        if (ret == USER_NEED_RESET_PASSWORD) {
            (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
        } else {
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
        }
        return HTTP_FORBIDDEN;
    }
    

    

    
    // 获取属性"PublickeyType"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_TYPE, &public_key_type_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != public_key_type_jso), HTTP_INTERNAL_SERVER_ERROR,
        clear_ssh_publickey_file(i_paras);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));

    public_key_type_str = dal_json_object_get_str(public_key_type_jso);
    
    return_val_do_info_if_fail(NULL != public_key_type_str, HTTP_INTERNAL_SERVER_ERROR,
        clear_ssh_publickey_file(i_paras);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));
    

    // 获取属性"PublicKey"的值
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_CONTENT, &public_key_value_jso);
    return_val_do_info_if_fail(ret_bool && (NULL != public_key_value_jso), HTTP_INTERNAL_SERVER_ERROR,
        clear_ssh_publickey_file(i_paras);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_PARAM_CONTENT, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));

    public_key_value_str = dal_json_object_get_str(public_key_value_jso);
    
    return_val_do_info_if_fail(NULL != public_key_value_str, HTTP_INTERNAL_SERVER_ERROR,
        clear_ssh_publickey_file(i_paras);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__));
    

    
    if (g_strcmp0(public_key_type_str, RFACTION_FORMAT_TEXT) == 0) { // 证书以文本的形式存在
        

        // 公钥的内容不能超过2048
        return_val_do_info_if_expr(strlen(public_key_value_str) > SSH_PUBLIC_KEY_MAX_LEN, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_TASK_PUBLIC_KEY_IMPORT_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso));

        

        // 将文本保存为文件
        public_key_value_len = strlen(public_key_value_str);
        ret_file_bool = g_file_set_contents(TMP_SSH_PUBLIC_KEY_PUB_FILE, (const gchar *)public_key_value_str,
            public_key_value_len, NULL);
        return_val_do_info_if_fail(ret_file_bool, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "write request cert to %s failed", TMP_SSH_PUBLIC_KEY_PUB_FILE);
            (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso));
    } else if (g_strcmp0(public_key_type_str, RFACTION_FORMAT_FILE) == 0) { // 证书以文件的形式存在
        
        return_val_do_info_if_expr(!g_str_has_suffix(public_key_value_str, ".pub"), HTTP_BAD_REQUEST,
            clear_ssh_publickey_file(i_paras);
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso,
            RF_SENSITIVE_INFO, RFACTION_PARAM_CONTENT));

        // 判断文件的存在于本地还是远程文件服务器
        // 增加远程文件下载的方法或者获取证书文件路径
        // 证书文件导入
        // 判断是本地导入还是远程文件导入
        if (public_key_value_str[0] == '/') {
            // 本地文件导入，拷贝Content中的内容到指定的临时文件夹TMP_CLIENT_PEM_FILE  "/tmp/web/client_tmp.cer"
            // 检查是否在/tmp/目录下
            // 判断是否为"/tmp/"开头的路径
            if (!strstr(public_key_value_str, TMP_PATH_WITH_SLASH)) {
                (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso,
                    RF_SENSITIVE_INFO, RFACTION_PARAM_CONTENT);
                return HTTP_BAD_REQUEST;
            }

            
            ret = dal_check_real_path(public_key_value_str);
            if (ret != VOS_OK) {
                debug_log(DLOG_ERROR, "%s :SSH file real_path is illegal! return %d\r\n", __FUNCTION__, ret);

                
                (void)create_message_info(MSGID_TASK_PUBLIC_KEY_IMPORT_ERR, RFACTION_IMPORT_SSH_PUBLICKEY,
                    o_message_jso);
                

                return HTTP_BAD_REQUEST;
            }

            if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, public_key_value_str,
                (const gchar *)i_paras->user_roleid)) {
                (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
                return HTTP_BAD_REQUEST;
            }

            
            if (vos_get_file_length(public_key_value_str) > SSH_PUBLIC_KEY_MAX_LEN) {
                (void)create_message_info(MSGID_TASK_PUBLIC_KEY_IMPORT_ERR, RFACTION_IMPORT_SSH_PUBLICKEY,
                    o_message_jso);
                
                // 删除源文件
                (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    public_key_value_str);
                
                return HTTP_BAD_REQUEST;
            }

            

            
            if (g_strcmp0(TMP_SSH_PUBLIC_KEY_PUB_FILE, public_key_value_str) != 0) {
                // redfish模块降权限后，调用代理方法拷贝文件
                ret = proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, public_key_value_str,
                    TMP_SSH_PUBLIC_KEY_PUB_FILE, REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
                if (ret != VOS_OK) {
                    debug_log(DLOG_ERROR, "%s: copy request ssh public file failed", __FUNCTION__);
                    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                        public_key_value_str);
                    (void)proxy_delete_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                        TMP_SSH_PUBLIC_KEY_PUB_FILE);
                    (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
                    return HTTP_INTERNAL_SERVER_ERROR;
                }
            }
            
        } else {
            // 远程文件导入
            return act_account_import_file_uri(i_paras, (const gchar *)user_name, o_message_jso, o_result_jso);
        }
    } // else

    
    (void)dal_set_file_owner(TMP_SSH_PUBLIC_KEY_PUB_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_SSH_PUBLIC_KEY_PUB_FILE, (S_IRUSR | S_IWUSR));

    
    // 入 参1: 获取用户的ID
    ret = dal_get_property_value_byte(obj_user_handle, PROPERTY_USER_ID, &user_id);
    return_val_do_info_if_fail(DFL_ERR != ret, HTTP_INTERNAL_SERVER_ERROR, clear_ssh_publickey_file(i_paras);
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso));

    // 入参2 : 获取ssh公钥存在的方式
    Keytype = USER_PUBLIC_KEY_TYPE_FILE;

    // 入参3 : 公钥临时文件所在的路径
    safe_fun_ret = strcpy_s(public_key_temp_path, sizeof(public_key_temp_path), TMP_SSH_PUBLIC_KEY_PUB_FILE);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    // 将入参user_id 和临时公钥文件的路径和公钥文件的存在方式填入到inputlist表中
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(Keytype));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)public_key_temp_path));

    // 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_user_handle,
        CLASS_USER, METHOD_USER_SETPUBLICKEY, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    
    // 对UIP的返回值进行判断
    do_val_if_fail(ret == VOS_OK, clear_ssh_publickey_file(i_paras);
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_USER_SETPUBLICKEY, ret));

    // 对类方法ImportClientCertificate 返回值进行判断
    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    switch (ret) {
            
        case VOS_OK:
            (void)create_message_info(MSGID_IMPORT_PUBLIC_KEY_OK, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
            return HTTP_OK;
        case USER_PUBLIC_KEY_FORMAT_ERROR:
            (void)create_message_info(MSGID_IMPORT_PUBLIC_KEY_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
            return HTTP_BAD_REQUEST;
            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
            debug_log(DLOG_ERROR, "unknown SSH public key import error:%d", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_account_import_ssh_publickey_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param is invalid", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param is invalid", __FUNCTION__);
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((USERROLE_READONLY & i_paras->user_role_privilege) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_IMPORT_SSH_PUBLICKEY, o_message_jso);
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = act_account_import_ssh_publickey(i_paras, o_message_jso, o_result_jso);
    
    if (ret != HTTP_ACCEPTED) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }

    return ret;
}

LOCAL gint32 act_account_delete_client_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    OBJ_HANDLE obj_user_handle = 0; // 用户类的对象的句柄
    OBJ_HANDLE obj_macc_handle;     // 双因素客户端类的对象的句柄
    OBJ_HANDLE obj_product_handle = 0;
    guint8 ma_enable = 0; // 单板是否支持双因素认证
    guint8 user_id;       // 用户的的ID
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;
    gchar serialnumber_string[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN] = {0}; // serialnumber 值

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    if (0 == (USERROLE_USERMGNT & i_paras->user_role_privilege)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    // 形成合法的member_id
    gint32 ret_mi_check = redfish_form_memberid_effective(i_paras);
    return_val_do_info_if_fail(VOS_OK == ret_mi_check, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: update member_id fail.\n", __FUNCTION__, __LINE__));

    gint32 ret =
        redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, NULL, 0, &obj_user_handle, o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 单板是否支持双因素认证
    // 获取类"PRODUCT"的句柄
    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_product_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    // 获取类"MutualAuthenticationEnable"的值
    ret = dal_get_property_value_byte(obj_product_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &(ma_enable));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    if (ma_enable == 0) {
        (void)create_message_info(MSGID_DELETE_CLICERT_NOT_SUPPORTED_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT,
            o_message_jso);
        
        return HTTP_NOT_IMPLEMENTED;
        
    }

    
    // 入 参1: 获取用户的ID
    ret = dal_get_property_value_byte(obj_user_handle, PROPERTY_USER_ID, &user_id);
    return_val_do_info_if_fail(VOS_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso));

    // 入参2 : 获取用户的ID对应的双因素客户端证书对象的句柄
    
    ret = dal_get_specific_object_byte(MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_HASHID, user_id,
        &obj_macc_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get obj handle fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso));
    

    
    ret = dal_get_property_value_string(obj_macc_handle, RFPROP_MUTAU_CERT_SN, serialnumber_string,
        sizeof(serialnumber_string));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        if (0 == g_strcmp0(serialnumber_string, "")) {
            (void)create_message_info(MSGID_CLICERT_DELETE_MISSING_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT,
                o_message_jso);
            return HTTP_BAD_REQUEST;
        }
    }

    // 将入参user_id 和临时客户端证书文件的路径填入到inputlist表中
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    // 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_macc_handle,
        MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_METHOD_DELETE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", MUTUAL_AUTH_CLIENT_CERT_METHOD_DELETE, ret));

    switch (ret) {
            
        case VOS_OK:
            (void)create_message_info(MSGID_CLICERT_DELETE_OK, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso,
                i_paras->user_name);
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_MUTAU_CERT_DELETE_CLI_CERT, o_message_jso);
            debug_log(DLOG_ERROR, "unknown cert delete error:%d", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 act_account_delete_ssh_publickey(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    gint32 ret;                                       // 函数的返回值
    OBJ_HANDLE obj_user_handle = 0;                       // 用户类的对象的句柄
    guint8 user_id = 0;                                   // 用户的的ID
    gchar user_name[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN]; // 用户id对应的用户名
    gint32 ret_mi_check;
    guint8 Keytype; // ssh公钥存在的方式: 文本，文件，待删除
    gchar pkh_string[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN];           // 公钥哈希
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;

    (void)memset_s(pkh_string, sizeof(pkh_string), 0, sizeof(pkh_string)); // 字符数组初始化
    (void)memset_s(user_name, sizeof(user_name), 0, sizeof(user_name));    // 字符数组初始化

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    if (0 == (USERROLE_READONLY & i_paras->user_role_privilege)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    // 形成合法的member_id
    ret_mi_check = redfish_form_memberid_effective(i_paras);
    return_val_do_info_if_fail(VOS_OK == ret_mi_check, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: update member_id fail.\n", __FUNCTION__, __LINE__));
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, user_name, sizeof(user_name),
        &obj_user_handle, o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = _rf_check_user_configure_priv(obj_user_handle, i_paras->is_from_webui, i_paras->user_role_privilege,
        i_paras->user_name, user_name);
    if (ret != RET_OK) {
        if (ret == USER_NEED_RESET_PASSWORD) {
            (void)create_message_info(MSGID_PASSWORD_NEED_RESET, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso);
        } else {
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso);
        }

        return HTTP_FORBIDDEN;
    }
    

    
    // 入 参1: 获取用户的ID
    ret = dal_get_property_value_byte(obj_user_handle, PROPERTY_USER_ID, &user_id);
    return_val_do_info_if_fail(DFL_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property value fail.\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso));
    // 入参2 : 获取ssh公钥存在的方式
    Keytype = USER_PUBLIC_KEY_TYPE_DELETE;

    // 不能删除不存在的公钥
    // "PublickeyHash"的的宏定义为:  PROPERTY_USER_PUBLICKEY_HASH
    ret = dal_get_property_value_string(obj_user_handle, PROPERTY_USER_PUBLICKEY_HASH, pkh_string, sizeof(pkh_string));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        if (0 == g_strcmp0(pkh_string, "")) {
            (void)create_message_info(MSGID_DETETE_PUBLIC_KEY_MISSING_ERR, RFACTION_DELETE_SSH_PUBLICKEY,
                o_message_jso);
            return HTTP_BAD_REQUEST;
        }
    }

    // 将入参user_id 和临时公钥文件的路径和公钥文件的存在方式填入到inputlist表中
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(Keytype));
    input_list = g_slist_append(input_list, g_variant_new_string(""));

    // 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_user_handle,
        CLASS_USER, METHOD_USER_SETPUBLICKEY, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    switch (ret) {
            
        case VOS_OK:
            (void)create_message_info(MSGID_DELETE_PUBLIC_KEY_OK, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso);
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_DELETE_SSH_PUBLICKEY, o_message_jso);
            debug_log(DLOG_ERROR, "unknown ssh public key delete error:%d", ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_account_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    gint32 ret;
    json_bool ret_bool;
    OBJ_HANDLE obj_handle = 0;
    json_object *oem_data_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer. ", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    
    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, NULL, 0, &obj_handle, o_message_jso);
    do_info_if_true(HTTP_INTERNAL_SERVER_ERROR == ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, RFPROP_ACCOUNT_OEM, o_message_jso));
    return_val_if_fail(VOS_OK == ret, ret);

    
    gint32 json_len = json_object_object_length(i_paras->val_jso);
    if (json_len == 0) {
        debug_log(DLOG_INFO, "%s, json value is null ", __FUNCTION__);
        return RF_IGNORE_CUR_OPERATION_RESULT;
    }

    // 首先, 获取属性"Huawei"的值
    
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_ACCOUNT_HUAWEI, &oem_data_jso);
    return_val_if_fail(ret_bool && (NULL != oem_data_jso), HTTP_BAD_REQUEST);
    

    
    ret = set_account_oem_data(i_paras, obj_handle, o_message_jso, oem_data_jso);
    if (VOS_OK != ret) {
        return HTTP_BAD_REQUEST;
    }

    
    ret = HTTP_OK;
    return ret;
}


LOCAL gint32 convert_logininterface_form_string_to_val(const gchar *temp_gchar)
{
    guint32 logininterface = 0;

    if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_WEB) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_WEB_VALUE;
    } else if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_SNMP) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_SNMP_VALUE;
    } else if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_IPMI) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_IPMI_VALUE;
    } else if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_SSH) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_SSH_VALUE;
    } else if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_SFTP) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_SFTP_VALUE;
    } else if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_LOCAL) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_LOCAL_VALUE;
    } else if (g_strcmp0(temp_gchar, RFPROP_LOGIN_INTERFACE_REDFISH) == 0) {
        logininterface += RFPROP_LOGIN_INTERFACE_REDFISH_VALUE;
    }

    return logininterface;
}

#define ACCOUNT_PROP_NOT_EXIST 1
#define ACCOUNT_ARRAY_LEN 2
#define ACCOUNT_DUPLICATE 3


LOCAL gint32 parse_logininterface_value(json_object *oem_data_jso, guint32 *logininterface_val)
{
    json_bool ret_bool;
    json_object *logininterface_jso = NULL;
    guint32 len_array_logininterface;
    guint32 i;
    gint32 dup_index = 0;
    json_object *temp_jso = NULL;
    const gchar *temp_gchar = NULL;
    guint32 new_logininterface = 0;
    const guint32 max_loginiterface_num = 7;

    ret_bool = json_object_object_get_ex(oem_data_jso, PROPERTY_USER_LOGIN_INTERFACE, &logininterface_jso);
    if (ret_bool == FALSE || logininterface_jso == NULL) {
        debug_log(DLOG_INFO, "%s, get property %s fail.", __FUNCTION__, PROPERTY_USER_LOGIN_INTERFACE);
        return ACCOUNT_PROP_NOT_EXIST;
    }

    len_array_logininterface = json_object_array_length(logininterface_jso);
    if (len_array_logininterface > max_loginiterface_num) {
        debug_log(DLOG_ERROR, "%s, get json object array length fail", __FUNCTION__);
        return ACCOUNT_ARRAY_LEN;
    }

    // 求解new_logininterface的值
    
    if (json_array_duplicate_check(logininterface_jso, &dup_index) == RF_OK) {
        debug_log(DLOG_ERROR, "%s, json_array_duplicate_check fail", __FUNCTION__);
        return ACCOUNT_DUPLICATE;
    }

    for (i = 0; i < len_array_logininterface; i++) {
        temp_jso = json_object_array_get_idx(logininterface_jso, i);
        if (temp_jso == NULL) {
            continue;
        }

        temp_gchar = dal_json_object_get_str(temp_jso);
        if (temp_gchar == NULL) {
            continue;
        }

        new_logininterface += convert_logininterface_form_string_to_val(temp_gchar);
        temp_jso = NULL;
    }

    *logininterface_val = new_logininterface & USER_LOGIN_INTERFACE_VALUE_MASK;

    return HTTP_OK;
}


LOCAL gint32 get_logininterface_value(json_object **o_message_jso, json_object *oem_data_jso,
    guint32 *logininterface_val)
{
    gint32 ret = parse_logininterface_value(oem_data_jso, logininterface_val);

    switch (ret) {
        case HTTP_OK:
            return HTTP_OK;
        case ACCOUNT_PROP_NOT_EXIST:
            (void)create_message_info(MSGID_PROP_ITEM_MISSING, ACCOUNT_PROP_NAME_LOGININTERFACE, o_message_jso,
                ACCOUNT_PROP_NAME_LOGININTERFACE);
            return HTTP_BAD_REQUEST;
        case ACCOUNT_ARRAY_LEN:
            (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, ACCOUNT_PROP_NAME_LOGININTERFACE, o_message_jso,
                ACCOUNT_PROP_NAME_LOGININTERFACE);
            return HTTP_BAD_REQUEST;
        case ACCOUNT_DUPLICATE:
            (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, ACCOUNT_PROP_NAME_LOGININTERFACE, o_message_jso,
                ACCOUNT_PROP_NAME_LOGININTERFACE);
            return HTTP_BAD_REQUEST;
        default:
            debug_log(DLOG_ERROR, "%s, get logininterface value fail:%d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, PROPERTY_USER_LOGIN_INTERFACE, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_account_logininterface(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *oem_data_jso)
{
    
    guint32 old_logininterface = 0;
    guint32 new_logininterface = 0;
    json_object *password_jso = NULL;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;

    
    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ACCOUNT_PROP_NAME_LOGININTERFACE, o_message_jso,
            ACCOUNT_PROP_NAME_LOGININTERFACE);
        return HTTP_FORBIDDEN;
    }

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, ACCOUNT_PROP_NAME_LOGININTERFACE, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = get_logininterface_value(o_message_jso, oem_data_jso, &new_logininterface);
    if (ret != HTTP_OK) {
        return ret;
    }

    
    // "Logininterface"的的宏定义为:  PROPERTY_USER_LOGIN_INTERFACE
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_USER_LOGIN_INTERFACE, &old_logininterface);
    return_val_do_info_if_fail(DFL_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%d: 'get property value fail.'\n", __LINE__));

    
    if ((old_logininterface & RFPROP_LOGIN_INTERFACE_IPMI_VALUE) == 0 &&
        (new_logininterface & RFPROP_LOGIN_INTERFACE_IPMI_VALUE) != 0) { // ipmi的权限增加
        json_bool ret_bool = json_object_object_get_ex(i_paras->user_data, RFPROP_ACCOUNT_PASSWORD, &password_jso);
        if (ret_bool == FALSE || password_jso == NULL) {
            debug_log(DLOG_ERROR, "%s, Add ipmi permission must reset password.", __FUNCTION__);
            (void)create_message_info(MSGID_ACCOUNT_MUST_RESET_PASSWORD, RFPROP_ACCOUNT_PASSWORD, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
    }

    
    // 填写入参
    input_list = g_slist_append(input_list, g_variant_new_uint32(new_logininterface));
    // 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SET_USER_LOGIN_INTERFACE, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_USER_SET_USER_LOGIN_INTERFACE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((old_logininterface & RFPROP_LOGIN_INTERFACE_IPMI_VALUE) == 0 &&
        (new_logininterface & RFPROP_LOGIN_INTERFACE_IPMI_VALUE) != 0) { // ipmi的权限增加
        i_paras->val_jso = password_jso;
        ret = exec_set_account_password(i_paras, obj_handle, o_message_jso);
        if (ret != HTTP_OK) { // 设置密码失败应该将logininterface重置回原状态
            
            old_logininterface = old_logininterface & USER_LOGIN_INTERFACE_VALUE_MASK;
            (void)rollback_ipmi(i_paras, old_logininterface, obj_handle, o_message_jso);
            ret = HTTP_BAD_REQUEST;
        }
    }

    
    return ret;
}


LOCAL gint32 parse_login_rule(json_object *json_loginrule, json_object **o_message_jso, guchar *rule_val)
{
#define MAX_ROLE_NUM 3
    guint32 array_count;
    gint32 dup_index = 0;
    gchar name_parameter[MAX_CHARACTER_NUM] = {0};
    guint32 loop_tag;
    json_object *login_idx = NULL;
    const gchar *rule = NULL;
    guchar rule_set = 0;
    const gchar* permit_role_map[MAX_ROLE_NUM] = {"Rule1", "Rule2", "Rule3"};

    array_count = json_object_array_length(json_loginrule);
    if (array_count > MAX_ROLE_NUM) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, PROPERTY_LOGIN_RULE_PATH, o_message_jso,
            PROPERTY_LOGIN_RULE_PATH);
        debug_log(DLOG_ERROR, "%s :get json object array length fail", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    if (json_array_duplicate_check(json_loginrule, &dup_index) == RF_OK) {
        if (snprintf_s(name_parameter, sizeof(name_parameter), sizeof(name_parameter) - 1, "Oem/Huawei/LoginRule/%d",
            dup_index) <= 0) {
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, NULL, o_message_jso, (const gchar *)name_parameter);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag = 0; loop_tag < array_count; loop_tag++) {
        login_idx = json_object_array_get_idx(json_loginrule, loop_tag);
        if (login_idx == NULL) {
            continue;
        }

        rule = dal_json_object_get_str(login_idx);
        if (rule == NULL) {
            continue;
        }

        if (g_strcmp0(permit_role_map[0], rule) == 0) {
            rule_set |= 1;
        } else if (g_strcmp0(permit_role_map[1], rule) == 0) {
            rule_set |= (1 << 1);
        } else if (g_strcmp0(permit_role_map[2], rule) == 0) {
            rule_set |= (1 << 2);
        }
    }

    *rule_val = rule_set;
    return HTTP_OK;
}


LOCAL gint32 set_account_loginrule(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *oem_data_jso)
{
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    gint32 ret_code;
    json_object *login_rule = NULL;
    guchar rule_set = 0;

    if ((i_paras->user_role_privilege & USERROLE_USERMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_LOGIN_RULE_PATH, o_message_jso,
            PROPERTY_LOGIN_RULE_PATH);
        return HTTP_FORBIDDEN;
    }

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, PROPERTY_LOGIN_RULE_PATH, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = json_object_object_get_ex(oem_data_jso, RFPROP_LOGIN_RULE, &login_rule);
    return_val_do_info_if_expr(TRUE != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%d: 'get property value fail.'\n", __LINE__));

    ret = parse_login_rule(login_rule, o_message_jso, &rule_set);
    if (ret != HTTP_OK) {
        return ret;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(rule_set));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SET_PERMIT_RULE_ID, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);

    uip_free_gvariant_list(input_list);

    
    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret_code = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    uip_free_gvariant_list(output_list);
    if (ret_code != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        ret = HTTP_INTERNAL_SERVER_ERROR;
    }

    return ret;
}


LOCAL gint32 get_snmp_auth_protocol(json_object *oem_data_jso, json_object **o_message_jso, guint8 *snmp_auth_pro_val)
{
    json_bool ret_bool;
    struct json_object *snmp_auth_protocol_iso = NULL;
    const char *snmp_auth_protocol_str = NULL;

    ret_bool = json_object_object_get_ex(oem_data_jso, SNMP_V3AUTH_STR, &snmp_auth_protocol_iso);
    if (ret_bool != TRUE) {
        (void)create_message_info(MSGID_PROP_ITEM_MISSING, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, o_message_jso,
            ACCOUNT_PROP_NAME_SNMP_AUTH_PRO);
        debug_log(DLOG_ERROR, "%s : get property value fail.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    snmp_auth_protocol_str = dal_json_object_get_str(snmp_auth_protocol_iso);
    if (snmp_auth_protocol_str == NULL) {
        (void)create_message_info(MSGID_PROP_INVALID_VALUE, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO, o_message_jso, "",
            ACCOUNT_PROP_NAME_SNMP_PRIV_PRO);
        debug_log(DLOG_ERROR, "%s, %d: the snmp authentication algorithm is null.", __FUNCTION__, __LINE__);
        return HTTP_BAD_REQUEST;
    }

    if (g_strcmp0(SNMP_AUTHPRO_MD5, snmp_auth_protocol_str) == 0) {
        *snmp_auth_pro_val = USE_MD5;
    } else if (g_strcmp0(SNMP_AUTHPRO_SHA, snmp_auth_protocol_str) == 0) {
        *snmp_auth_pro_val = USE_SHA;
    } else if (g_strcmp0(SNMP_AUTHPRO_SHA1, snmp_auth_protocol_str) == 0) {
        *snmp_auth_pro_val = USE_SHA1;
    } else if (g_strcmp0(SNMP_AUTHPRO_SHA256, snmp_auth_protocol_str) == 0) {
        *snmp_auth_pro_val = USE_SHA256;
    } else if (g_strcmp0(SNMP_AUTHPRO_SHA384, snmp_auth_protocol_str) == 0) {
        *snmp_auth_pro_val = USE_SHA384;
    } else if (g_strcmp0(SNMP_AUTHPRO_SHA512, snmp_auth_protocol_str) == 0) {
        *snmp_auth_pro_val = USE_SHA512;
    } else {
        (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, o_message_jso,
            snmp_auth_protocol_str, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO);
        debug_log(DLOG_ERROR, "%s, %d: the snmp authentication algorithm does not support %s.", __FUNCTION__, __LINE__,
            snmp_auth_protocol_str);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}


LOCAL gint32 convert_priv_protocol_to_val(const gchar *json_privprotocol, guint8 *privprotocol,
    json_object **o_message_jso)
{
    guint8 input_privprotocol = 0;

    if (g_strcmp0(json_privprotocol, SNMP_PRIVPRO_DES) == 0) {
        input_privprotocol = SNMP_PRIVPRO_VAL_DES;
    } else if (g_strcmp0(json_privprotocol, SNMP_PRIVPRO_AES) == 0) {
        input_privprotocol = SNMP_PRIVPRO_VAL_AES;
    } else if (g_strcmp0(json_privprotocol, SNMP_PRIVPRO_AES256) == 0) { // 过滤,AES256只能搭配SHA256/SHA384/SHA512使用
        (void)create_message_info(MSGID_MODIFY_SNMP_PROTOCOL_NOT_MATCH, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO, o_message_jso);
        debug_log(DLOG_ERROR, "%s:privacy protocol is AES256, auth protocol should be SHA256/SHA384/SHA512",
            __FUNCTION__);
        return HTTP_BAD_REQUEST;
    } else {
        (void)create_message_info(MSGID_PROP_ITEM_NOT_IN_LIST, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO, o_message_jso,
            json_privprotocol, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO);
        debug_log(DLOG_ERROR, "%s: invalid property value :%s", __FUNCTION__, json_privprotocol);
        return HTTP_BAD_REQUEST;
    }
    *privprotocol = input_privprotocol;

    return HTTP_OK;
}


LOCAL gint32 check_priv_protocol_match_auth(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso, OBJ_HANDLE obj_handle,
    guchar user_id, json_object **o_message_jso)
{
    // 取传进来的加密算法类型
    struct json_object *snmp_priv_protocol = NULL;
    const gchar *json_privprotocol = NULL;
    guint8 input_privprotocol = 0;
    GSList *input_list = NULL;
    gint32 ret;
    json_bool ret_bool;
    guint8 ori_privprotocol = 0;

    ret_bool = json_object_object_get_ex(o_result_jso, SNMP_V3PRIV_STR, &snmp_priv_protocol);
    if (ret_bool ==
        TRUE) { // 取加密算法成功，说明是同时设置加密算法和鉴权算法。同时设置在此处完成，后面不再进行AES256设置动作
        json_privprotocol = dal_json_object_get_str(snmp_priv_protocol);
        if (json_privprotocol == NULL) {
            clear_jso_and_delete_object(o_result_jso,
                SNMP_V3PRIV_STR); // 删除snmp加密算法避免在set_account_huawei中再次调用set方法设置加密算法
            debug_log(DLOG_ERROR, "%s: dal_json_object_get_str failed", __FUNCTION__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        
        ret = convert_priv_protocol_to_val(json_privprotocol, &input_privprotocol, o_message_jso);
        if (ret != HTTP_OK) {
            clear_jso_and_delete_object(o_result_jso,
                SNMP_V3PRIV_STR); // 删除snmp加密算法避免在set_account_huawei中再次调用set方法设置加密算法
            return ret;
        }

        input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
        input_list = g_slist_append(input_list, g_variant_new_byte(input_privprotocol));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_USER, METHOD_USER_SETSNMPPRIVACYPROTOCOL, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
            NULL);

        uip_free_gvariant_list(input_list);
        clear_jso_and_delete_object(o_result_jso,
            SNMP_V3PRIV_STR); // 删除snmp加密算法避免在set_account_huawei中再次调用set方法设置加密算法
        if (ret != VOS_OK) {
            (void)create_message_info(MSGID_INTERNAL_ERR, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    } else { // 单独设置鉴权算法，取环境加密算法判断
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_SNMPPRIVACYPROTOCOL, &ori_privprotocol);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: get property %s value fail.", __FUNCTION__, PROPERTY_USER_SNMPAUTHPROTOCOL);
            return HTTP_INTERNAL_SERVER_ERROR;
        }

        if (ori_privprotocol >= USE_AES256) {
            debug_log(DLOG_ERROR, "%s:privacy protocol is AES256, auth protocol should be SHA256/SHA384/SHA512",
                __FUNCTION__);
            (void)create_message_info(MSGID_MODIFY_SNMP_PROTOCOL_NOT_MATCH, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO,
                o_message_jso);
            return HTTP_BAD_REQUEST;
        }
    }
    return HTTP_OK;
}


LOCAL gint32 auth_protocol_get_prop_str(json_object *val_jso, gchar *prop_name, const char **prop_val_str)
{
    json_bool ret_bool;
    struct json_object *temp_jso = NULL;
    const char *jso_val_str = NULL;

    ret_bool = json_object_object_get_ex(val_jso, prop_name, &temp_jso);
    if (ret_bool == FALSE || temp_jso == NULL) {
        return HTTP_BAD_REQUEST;
    }

    jso_val_str = dal_json_object_get_str(temp_jso);
    if (jso_val_str == NULL) {
        return HTTP_BAD_REQUEST;
    }

    *prop_val_str = jso_val_str;
    return HTTP_OK;
}


LOCAL gint32 get_auth_protocol_input_list(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso,
    json_object **o_message_jso, GSList **input_list)
{
    gint32 ret;
    guchar user_id = 0;
    guint8 snmp_auth_pro_val = 0;
    const char *snmp_login_password_str = NULL;
    const char *snmp_priv_password_str = NULL;
    GSList *auth_pwd_input_list = NULL;

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &user_id);
    if (ret != VOS_OK || user_id < MIN_USER_ID || user_id > MAX_USER_ID) {
        debug_log(DLOG_ERROR, "%s: get user_id fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_snmp_auth_protocol(o_result_jso, o_message_jso, &snmp_auth_pro_val);
    if (ret != HTTP_OK) {
        return ret;
    }

    
    if (snmp_auth_pro_val < USE_SHA256) { // 加密算法AES256只能搭配鉴权算法SHA256/SHA384/SHA512使用
        ret = check_priv_protocol_match_auth(i_paras, o_result_jso, obj_handle, user_id, o_message_jso);
        if (ret != HTTP_OK) {
            return ret;
        }
    }

    
    ret = auth_protocol_get_prop_str(i_paras->user_data, RFPROP_ACCOUNT_PASSWORD, &snmp_login_password_str);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s, get property %s fail, result %d.", __FUNCTION__, RFPROP_ACCOUNT_PASSWORD, ret);
        (void)create_message_info(MSGID_MODIFY_SNMP_LACK_PROPERTY, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    
    ret = auth_protocol_get_prop_str(o_result_jso, SNMP_V3PRIVPASSWD, &snmp_priv_password_str);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s, get property %s fail, result %d.", __FUNCTION__, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD,
            ret);
        (void)create_message_info(MSGID_MODIFY_SNMP_LACK_PROPERTY, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    
    auth_pwd_input_list = g_slist_append(auth_pwd_input_list, g_variant_new_byte(user_id));
    auth_pwd_input_list = g_slist_append(auth_pwd_input_list, g_variant_new_byte(snmp_auth_pro_val));
    auth_pwd_input_list = g_slist_append(auth_pwd_input_list, g_variant_new_string(snmp_login_password_str));
    auth_pwd_input_list = g_slist_append(auth_pwd_input_list, g_variant_new_string(snmp_priv_password_str));

    *input_list = auth_pwd_input_list;
    return HTTP_OK;
}


LOCAL gint32 parse_auth_protocol_return_value(gint32 auth_protocol, gint32 auth_password, gint32 priv_password,
    json_object **o_message_jso)
{
    gint32 ret_back = HTTP_OK;
    gint32 ret;
    json_object *msg_err_jso = NULL;

    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: null point.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = parse_set_auth_protocol_return_value(auth_password, RFPROP_ACCOUNT_PASSWORD, &msg_err_jso);
    if (ret != HTTP_OK) {
        ret_back = HTTP_BAD_REQUEST;
    }
    add_err_msg(*o_message_jso, msg_err_jso);
    msg_err_jso = NULL;

    
    ret = parse_set_auth_protocol_return_value(priv_password, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, &msg_err_jso);
    if (ret != HTTP_OK) {
        ret_back = HTTP_BAD_REQUEST;
    }
    add_err_msg(*o_message_jso, msg_err_jso);
    msg_err_jso = NULL;

    if (auth_protocol != VOS_OK) {
        if (auth_password == VOS_OK && priv_password == VOS_OK) {
            (void)create_message_info(MSGID_INTERNAL_ERR, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, &msg_err_jso);
            add_err_msg(*o_message_jso, msg_err_jso);
            msg_err_jso = NULL;
        }
    }

    return ret_back;
}


LOCAL gint32 set_snmp_auth_algorithm(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *o_result_jso)
{
    const gint32 auth_protocol_index = 0;
    const gint32 auth_password_index = 1;
    const gint32 priv_password_index = 2;
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 auth_password_ret_val;
    gint32 priv_password_ret_val;
    gint32 auth_protocol_ret_val;

    
    ret = get_auth_protocol_input_list(i_paras, obj_handle, o_result_jso, o_message_jso, &input_list);
    if (ret != HTTP_OK) {
        return ret;
    }

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETSNMPAUTHPROTOCOL, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);

    uip_free_gvariant_list(input_list);
    input_list = NULL;

    
    switch (ret) {
        case VOS_OK:
            break;
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, o_message_jso,
                i_paras->uri);
            return HTTP_NOT_FOUND;
        default:
            debug_log(DLOG_ERROR, "%s: uip_call_class_method_redfish fail %d", __FUNCTION__, ret);
            create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    auth_protocol_ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, auth_protocol_index));
    auth_password_ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, auth_password_index));
    priv_password_ret_val = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, priv_password_index));

    uip_free_gvariant_list(output_list);
    output_list = NULL;

    return parse_auth_protocol_return_value(auth_protocol_ret_val, auth_password_ret_val, priv_password_ret_val,
        o_message_jso);
}


LOCAL gint32 check_user_privilege(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, const gchar *related_prop,
    json_object **o_message_jso)
{
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};

    (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, username, sizeof(username));
    if (strlen(username) == 0) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    gint32 ret = _rf_check_user_configure_priv(obj_handle, i_paras->is_from_webui, i_paras->user_role_privilege,
        i_paras->user_name, username);
    if (ret != RET_OK) {
        if (ret == USER_NEED_RESET_PASSWORD) {
            (void)create_message_info(MSGID_PASSWORD_NEED_RESET, related_prop, o_message_jso);
        } else {
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, related_prop, o_message_jso);
        }

        return HTTP_FORBIDDEN;
    }

    return HTTP_OK;
}


LOCAL gint32 set_account_snmp_auth_algorithm(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso, json_object *o_result_jso)
{
    gint32 ret = check_user_privilege(i_paras, obj_handle, ACCOUNT_PROP_NAME_SNMP_AUTH_PRO, o_message_jso);
    if (ret != HTTP_OK) {
        return ret;
    }

    
    ret = set_snmp_auth_algorithm(i_paras, obj_handle, o_message_jso, o_result_jso);
    
    clear_jso_and_delete_object(o_result_jso, SNMP_V3PRIVPASSWD);

    if (ret == HTTP_OK) {
        ret = VOS_OK;
    }

    return ret;
}


LOCAL gint32 check_snmp_priv_password_permission(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso)
{
    guchar snmp_status = 0;
    guint32 product_ver = 0;

    if ((i_paras->user_role_privilege & USERROLE_READONLY) == 0) {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = check_user_privilege(i_paras, obj_handle, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
    if (ret != HTTP_OK) {
        return ret;
    }

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &snmp_status);
    ret = dal_get_product_version_num(&product_ver);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get product version fail %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (snmp_status == DISABLE || product_ver < PRODUCT_VERSION_V5) {
        debug_log(DLOG_DEBUG, "%s: snmp not supported", __FUNCTION__);
        create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso,
            ACCOUNT_PROP_NAME_SNMP_PRIV_PWD);
        return HTTP_BAD_REQUEST;
    }

    return VOS_OK;
}


LOCAL gint32 call_set_snmp_priv_pass_method(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guchar user_id,
    const gchar *snmp_priv_password_str)
{
    GSList *input_list = NULL;
    OBJ_HANDLE class_user_handle;

    gint32 ret = dal_get_object_handle_nth(CLASS_USER, 0, &class_user_handle);
    if (ret != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(snmp_priv_password_str));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, class_user_handle,
        CLASS_USER, METHOD_USER_SETSNMPPRIVACYPASSWORD, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return VOS_OK;
        case USER_DONT_EXIST:
            (void)create_message_info(MSGID_INVALID_USERNAME, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_DATA_LENGTH_INVALID:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso,
                USER_PASSWORD_MIN_LEN_STR, USER_PWD_MAX_LEN_STR);
            return HTTP_BAD_REQUEST;
        case USER_SET_PASSWORD_TOO_WEAK:
            (void)create_message_info(MSGID_PSWD_IN_WEAK_PWDDICT, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_INVALID_DATA_FIELD:
        case USER_SET_PASSWORD_EMPTY:
            (void)create_message_info(MSGID_INVALID_PAWD, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_PASS_COMPLEXITY_FAIL:
            (void)create_message_info(MSGID_PSWD_CMPLX_CHK_FAIL, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
            return HTTP_BAD_REQUEST;
        case USER_NODE_BUSY:
            (void)create_message_info(MSGID_OPERATION_FAILED, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
            return HTTP_BAD_REQUEST;
        default: 
            debug_log(DLOG_ERROR, "%s, set snmp encryption password fail :%d\n", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, ACCOUNT_PROP_NAME_SNMP_PRIV_PWD, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_account_snmp_priv_password(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso, json_object *o_result_jso)
{
    json_bool ret_bool;
    json_object *snmp_priv_pwd_jso = NULL;
    guchar user_id = 0;
    const gchar *snmp_priv_password_str = NULL;

    gint32 ret = check_snmp_priv_password_permission(i_paras, obj_handle, o_message_jso);
    if (ret != VOS_OK) {
        json_object_clear_string(o_result_jso, SNMP_V3PRIVPASSWD);
        debug_log(DLOG_ERROR, "%s, check_snmp_priv_password_permission fail: %d", __FUNCTION__, ret);
        return ret;
    }

    ret_bool = json_object_object_get_ex(o_result_jso, SNMP_V3PRIVPASSWD, &snmp_priv_pwd_jso);
    if (ret_bool == FALSE || snmp_priv_pwd_jso == NULL) {
        json_object_clear_string(o_result_jso, SNMP_V3PRIVPASSWD);
        debug_log(DLOG_ERROR, "%s, get property %s fail, result %d.", __FUNCTION__, SNMP_V3PRIVPASSWD, ret_bool);
        (void)create_message_info(MSGID_INVALID_PAWD, SNMP_V3PRIVPASSWD, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    snmp_priv_password_str = dal_json_object_get_str(snmp_priv_pwd_jso);
    if (snmp_priv_password_str == NULL) {
        debug_log(DLOG_ERROR, "%s: the snmp encrypt password is null", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_ID, &user_id);
    if (ret != VOS_OK || user_id < MIN_USER_ID || user_id > MAX_USER_ID) {
        json_object_clear_string(o_result_jso, SNMP_V3PRIVPASSWD);
        debug_log(DLOG_ERROR, "%s, %d: get user_id fail", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = call_set_snmp_priv_pass_method(i_paras, o_message_jso, user_id, snmp_priv_password_str);

    json_object_clear_string(o_result_jso, SNMP_V3PRIVPASSWD);

    return ret;
}

LOCAL gint32 priv_protocol_str_to_id(const gchar *val_str, guint8 *priv_protocol, guint8 user_id,
    json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *related_prop = "Oem/Huawei/SnmpV3PrivProtocol";

    if (g_strcmp0(val_str, SNMP_PRIVPRO_DES) == 0) {
        *priv_protocol = SNMP_PRIVPRO_VAL_DES;
    } else if (g_strcmp0(val_str, SNMP_PRIVPRO_AES) == 0) {
        *priv_protocol = SNMP_PRIVPRO_VAL_AES;
    } else if (g_strcmp0(val_str, SNMP_PRIVPRO_AES256) == 0) { // 过滤,AES256只能搭配SHA256/SHA384/SHA512使用
        guint8 authprotocol = 0;
        // 获取当前的鉴权算法
        ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s:  get object handle for %s fail", __FUNCTION__, CLASS_SNMP);
            goto ERR_EXIT;
        }

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_SNMPAUTHPROTOCOL, &authprotocol);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: get property %s value fail.", __FUNCTION__, PROPERTY_USER_SNMPAUTHPROTOCOL);
            goto ERR_EXIT;
        }

        if (authprotocol > USE_SHA512) {
            debug_log(DLOG_ERROR, "%s: auth protocol out of range.", __FUNCTION__);
            goto ERR_EXIT;
        }

        if (authprotocol < USE_SHA256) { // AES256必须搭配SHA256/SHA384/SHA512
            debug_log(DLOG_ERROR, "%s: set priv protocol failed, auth protocol is %d", __FUNCTION__, authprotocol);
            (void)create_message_info(MSGID_MODIFY_SNMP_PROTOCOL_NOT_MATCH, related_prop, o_message_jso);
            return HTTP_BAD_REQUEST;
        }
        *priv_protocol = SNMP_PRIVPRO_VAL_AES256;
    } else {
        debug_log(DLOG_ERROR, "%s: invalid property value :%s", __FUNCTION__, val_str);
        goto ERR_EXIT;
    }

    return VOS_OK;
ERR_EXIT:
    (void)create_message_info(MSGID_INTERNAL_ERR, related_prop, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_account_snmp_priv_protocol(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE user_handle,
    json_object **o_message_jso, json_object *oem_data_jso)
{
    const gchar *val_str = NULL;
    guint8 protocol = 0;
    guint8 user_id = 0;
    GSList *input_list = NULL;
    json_object *val_jso = NULL;

    gint32 ret = check_user_privilege(i_paras, user_handle, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO, o_message_jso);
    if (ret != HTTP_OK) {
        return ret;
    }

    (void)json_object_object_get_ex(oem_data_jso, SNMP_V3PRIV_STR, &val_jso);

    val_str = dal_json_object_get_str(val_jso);
    if (val_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_get_str failed", __FUNCTION__);
        goto ERR_EXIT;
    }

    ret = dal_get_property_value_byte(user_handle, PROPERTY_USER_ID, &user_id);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get user_id failed, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    ret = priv_protocol_str_to_id(val_str, &protocol, user_id, o_message_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: priv protocol translate string to ID failed", __FUNCTION__);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(protocol));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, user_handle,
        CLASS_USER, METHOD_USER_SETSNMPPRIVACYPROTOCOL, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: set snmpv3 user priv protocol failed, ret:%d", __FUNCTION__, ret);
        goto ERR_EXIT;
    }

    return VOS_OK;
ERR_EXIT:
    (void)create_message_info(MSGID_INTERNAL_ERR, ACCOUNT_PROP_NAME_SNMP_PRIV_PRO, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 set_account_insecure_prompt(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *oem_data_jso)
{
    GSList *input_list = NULL;
    gint32 ret;
    json_object *account_insecure = NULL;
    guint8 state;
    const gchar *prop_str = "Oem/Huawei/AccountInsecurePromptEnabled";

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, PROPERTY_LOGIN_RULE_PATH, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = json_object_object_get_ex(oem_data_jso, PFPROP_ACCOUNT_INSECURE_PROMPT_ENABLED, &account_insecure);
    return_val_do_info_if_expr(TRUE != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    state = (TRUE == json_object_get_boolean(account_insecure)) ? 1 : 0;
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SETINITIALSTATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    do_val_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_USER_SETINITIALSTATE, ret));

    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case USER_INVALID_DATA_FIELD:
            (void)create_message_info(MSGID_ACCOUNT_MODIFICATION_RESTRICTED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_str, o_message_jso, prop_str);
            return RFERR_INSUFFICIENT_PRIVILEGE;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_account_first_login_policy(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso, json_object *oem_data_jso)
{
    gint32 ret;
    json_object *first_login_plcy_jso = NULL;
    const gchar *first_login_plcy_str = NULL;
    guint8 first_login_plcy_val = FIRST_LOGIN_PLCY_NULL;
    GSList *input_list = NULL;
    const gchar *prop_str = "Oem/Huawei/FirstLoginPolicy";

    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invalid input parameter", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->user_role_privilege & USERROLE_USERMGNT)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_str, o_message_jso, prop_str);
        return HTTP_FORBIDDEN;
    }

    if (account_first_login_denied(i_paras->is_from_webui, i_paras->user_name)) {
        (void)create_message_info(MSGID_PASSWORD_NEED_RESET, prop_str, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    if (json_object_object_get_ex(oem_data_jso, RF_ACCOUNT_FIRST_LOGIN_PLCY, &first_login_plcy_jso) != TRUE) {
        debug_log(DLOG_ERROR, "%s: get property value fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 类方法调用
    first_login_plcy_str = dal_json_object_get_str(first_login_plcy_jso);
    if (g_strcmp0(first_login_plcy_str, FIRST_LOGIN_PLCY_FORCE_STR) == 0) {
        first_login_plcy_val = FIRST_LOGIN_PLCY_FORCE;
    } else if (g_strcmp0(first_login_plcy_str, FIRST_LOGIN_PLCY_PROMPT_STR) == 0) {
        first_login_plcy_val = FIRST_LOGIN_PLCY_PROMPT;
    } else {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prop_str, o_message_jso, first_login_plcy_str, prop_str);
        debug_log(DLOG_ERROR, "%s: first login policy value invalid", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(first_login_plcy_val));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_USER, METHOD_USER_SET_FIRST_LOGIN_PLCY, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return RET_OK;
}


LOCAL gint32 set_account_oem_data_prop(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_message_jso,
    json_object *oem_data_jso, ACCOUNT_SET_OEM_METHOD method)
{
    json_object *temp_jso = NULL;

    if (method.set_prop == NULL) {
        return HTTP_BAD_REQUEST;
    }

    gint32 ret = method.set_prop(i_paras, obj_handle, &temp_jso, oem_data_jso);
    if (temp_jso != NULL) {
        
        if (json_object_get_type(temp_jso) == json_type_array) {
            gint32 array_len = json_object_array_length(temp_jso);
            gint32 msg_index = 0;

            for (; msg_index < array_len; msg_index++) {
                (void)json_object_array_add(o_message_jso,
                    json_object_get(json_object_array_get_idx(temp_jso, msg_index)));
            }

            json_object_put(temp_jso);
        } else {
            (void)json_object_array_add(o_message_jso, temp_jso);
        }
        temp_jso = NULL;
    }

    return ret;
}


LOCAL gint32 set_account_oem_data(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso,
    json_object *oem_data_jso)
{
    guint32 prop_index;
    gint32 ret;
    gint32 ret_back = HTTP_BAD_REQUEST;
    json_object *prop_jso = NULL;

    ACCOUNT_SET_OEM_METHOD method_list[] = {
        {RFPROP_LOGIN_INTERFACE, set_account_logininterface}, // 设置用户登陆接口
        {RFPROP_LOGIN_RULE, set_account_loginrule},           // 设置用户登陆规则
        // 设置SNMP鉴权算法,为满足AES256仅只能和SHA256/SHA384/SHA512搭配使用的过滤要求,设置鉴权算法必须在设置加密算法之前
        {SNMP_V3AUTH_STR, set_account_snmp_auth_algorithm},
        {SNMP_V3PRIVPASSWD, set_account_snmp_priv_password},                   // 设置SNMP加密密码
        {PFPROP_ACCOUNT_INSECURE_PROMPT_ENABLED, set_account_insecure_prompt}, // 设置账户不安全提示状态
        {SNMP_V3PRIV_STR, set_account_snmp_priv_protocol},                     // 设置SNMP加密算法
        {RF_ACCOUNT_FIRST_LOGIN_PLCY, set_account_first_login_policy},         // 设置用户首次登陆口令修改策略
    };

    if (i_paras == NULL || o_message_jso == NULL || oem_data_jso == NULL) {
        return VOS_ERR;
    }

    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (prop_index = 0; prop_index < G_N_ELEMENTS(method_list); prop_index++) {
        
        if (json_object_object_get_ex(oem_data_jso, method_list[prop_index].prop_name, &prop_jso) != TRUE) {
            continue;
        }

        ret = set_account_oem_data_prop(i_paras, obj_handle, *o_message_jso, oem_data_jso, method_list[prop_index]);
        if (ret == VOS_OK) {
            ret_back = VOS_OK;
        }
    }

    return ret_back;
}


LOCAL void parse_cert_error_code(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
            
        case VOS_OK:
            (void)create_message_info(MSGID_CLICERT_IMPORT_OK, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_FORMAT_ERR:
            
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            
            break;

            
        case VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR:
            
        case VOS_ERR_MA_BASIC_CONSTRAINTS_ERR:
            (void)create_message_info(MSGID_CLICERT_CONSTRAINTS_ERR, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_CERT_EXSIT:
            (void)create_message_info(MSGID_CLICERT_CERT_EXSIT, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_NO_ISSUER:
            (void)create_message_info(MSGID_CLICERT_NO_ISSUER, NULL, o_message_jso);
            break;

            
        case VOS_ERR_MA_REACH_MAX:
            (void)create_message_info(MSGID_CLICERT_REACH_MAX, NULL, o_message_jso);
            break;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "unknown cert import error:%d", ret);
            break;
    }

    return;
}


LOCAL void parse_ssh_error_code(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
            
        case VOS_OK:
            (void)create_message_info(MSGID_IMPORT_PUBLIC_KEY_OK, NULL, o_message_jso);
            break;

        case USER_PUBLIC_KEY_FORMAT_ERROR:
            (void)create_message_info(MSGID_IMPORT_PUBLIC_KEY_ERR, NULL, o_message_jso);
            break;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }

    return;
}


LOCAL gint32 import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    
    gint32 ret = 0;
    gint32 file_down_progress = 0;
    json_object *importing_flag_jso = NULL;
    json_object *file_type_jso = NULL;
    json_object *member_id_jso = NULL;
    json_object *content_jso = NULL;
    json_object *result_jso = NULL;
    gint32 result_ret = 0;
    guint8 member_id = 0;
    gint32 file_type = 0;
    gint32 import_ret = 0;
    json_bool importing_flag;
    gchar         file_path[MAX_URI_LENGTH] = {0};
    char *pstr = NULL;

    
    return_val_do_info_if_fail((origin_obj_path != NULL) && (monitor_fn_data != NULL) && (message_obj != NULL),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORTING_FLAG,
        &importing_flag_jso);
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_FILE_TYPE, &file_type_jso);
    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_USER_ID, &member_id_jso);
    (void)json_object_object_get_ex(body_jso, RFACTION_PARAM_CONTENT, &content_jso);

    
    importing_flag = json_object_get_boolean(importing_flag_jso);

    
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    
    if (importing_flag == FALSE) {
        ret = get_file_transfer_progress(&file_down_progress);
        goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
            debug_log(DLOG_ERROR, "get file packege download progress failed"));

        
        ret = parse_file_transfer_err_code(file_down_progress, message_obj);
        return_val_do_info_if_fail(RF_OK == ret, RF_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
            monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
            debug_log(DLOG_ERROR, "file transfer error occured, error code:%d", file_down_progress));

        
        if (file_down_progress != RF_FINISH_PERCENTAGE) {
            
            monitor_fn_data->task_progress = (file_down_progress * 10) / 100; // 文件传输显示为总体进度的10%
            return RF_OK;
        } else {
            
            // 检测何种类型的文件的导入
            member_id = json_object_get_int(member_id_jso);
            file_type = json_object_get_int(file_type_jso);
            pstr = g_strrstr(dal_json_object_get_str(content_jso), "/");
            if (snprintf_s(file_path, sizeof(file_path), sizeof(file_path) - 1, "/tmp%s", pstr) <= 0) {
                return HTTP_INTERNAL_SERVER_ERROR;
            }

            if (file_type == RF_IMPORT_FILE_TYPE_CERT) {
                // 调用导入证书的方法
                import_ret = task_account_import_client_cert((json_object *)(monitor_fn_data->user_data), body_jso,
                    member_id, file_path, message_obj);
            } else if (file_type == RF_IMPORT_FILE_TYPE_SSH) {
                // 调用导入ssh的方法
                import_ret = task_account_import_ssh_publickey((json_object *)(monitor_fn_data->user_data), body_jso,
                    member_id, file_path, message_obj);
            }

            // 文件导入完成，将导入标识位置成true;同时记录错误返回码方便importing_flag == TRUE 时使用
            importing_flag = TRUE;
            json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_IMPORTING_FLAG,
                json_object_new_boolean(importing_flag));
            json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_IMPORT_RESULT,
                json_object_new_int(import_ret));

            // 更新总体ACTION的进度(PERCENTAGE)
            file_down_progress = 10;

            // POSTMAN显示action的进度
            monitor_fn_data->task_progress = file_down_progress;
        }
    } else {
        // 查询文件导入的状态
        (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_IMPORT_RESULT, &result_jso);
        file_type = json_object_get_int(file_type_jso); // 文件类型
        result_ret = json_object_get_int(result_jso);   // 返回值类型

        debug_log(DLOG_ERROR, "result_ret %d", result_ret);

        // 根据相应的错误码显示相应的自定义消息
        if (file_type == RF_IMPORT_FILE_TYPE_CERT) {
            parse_cert_error_code(result_ret, message_obj);
        } else if (file_type == RF_IMPORT_FILE_TYPE_SSH) {
            parse_ssh_error_code(result_ret, message_obj);
        }

        // 更新TASK状态(正常结束或异常)
        file_down_progress = 0;

        if (result_ret == VOS_OK) {
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            file_down_progress = RF_FINISH_PERCENTAGE;
        } else {
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        }

        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);

        // POSTMAN显示action的进度
        monitor_fn_data->task_progress = 10 + (file_down_progress * 90) / 100;
    }

    // 文件返回
    return RF_OK;

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);

    return RF_OK; 
}


LOCAL void import_monitor_info_free_func(void *monitor_data)
{
    json_object *jso = (json_object *)monitor_data;

    (void)json_object_put(jso);

    return;
}


LOCAL TASK_MONITOR_INFO_S *import_monitor_info_new(PROVIDER_PARAS_S *i_paras)
{
    TASK_MONITOR_INFO_S *monitor_info = NULL;
    json_object *user_data_jso = NULL;
    gint32 file_type = 0;

    // 待导入的文件的类型
    if (g_strrstr(i_paras->uri, "importmutualauthclientcert")) {
        file_type = 1;
    } else if (g_strrstr(i_paras->uri, "importsshpublickey")) {
        file_type = 2;
    } else {
        debug_log(DLOG_ERROR, "%s: get a file type failed", __FUNCTION__);
        return NULL;
    }

    monitor_info = task_monitor_info_new((GDestroyNotify)import_monitor_info_free_func);
    
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
    json_object_object_add(user_data_jso, RF_UPGRADING_FLAG, json_object_new_boolean(FALSE));
    json_object_object_add(user_data_jso, RF_IMPORT_USER_ID, json_object_new_string(i_paras->member_id)); // 用户的ID
    json_object_object_add(user_data_jso, RF_IMPORT_FILE_TYPE,
        json_object_new_int(file_type)); // 标示文件的类型，ssh或者客户端证书
    json_object_object_add(user_data_jso, RF_IMPORT_USER_NAME, json_object_new_string(i_paras->user_name)); // 用户名
    json_object_object_add(user_data_jso, RF_IMPORT_USER_CLIENT, json_object_new_string(i_paras->client));  // 用户IP
    json_object_object_add(user_data_jso, RF_IMPORT_USER_ROLE,
        json_object_new_int(i_paras->user_role_privilege)); // 用户权限
    
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int(i_paras->is_from_webui)); // 是否来自WEB调用的操作
    

    // 给user_data 分配新值
    monitor_info->user_data = user_data_jso;

    return monitor_info;
}


LOCAL gint32 act_account_import_file_uri(PROVIDER_PARAS_S *i_paras, const gchar *opt_user, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    const gchar *transfer_protocol = NULL;
    json_object *body_jso = NULL;
    
    gchar     file_transfer_uri[MAX_URI_PATH_LENGTH] = {0}; 
    
    gint32 ret;
    guint8 file_id = 0; // 文件的id
    GSList *input_list = NULL;
    OBJ_HANDLE file_transfer_handle;
    TASK_MONITOR_INFO_S *import_monitor_data = NULL;
    json_object *running_task = NULL;
    errno_t secure_ret;

    
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
        
        return HTTP_BAD_REQUEST;
    }

    

    

    
    running_task = get_exist_running_task(import_status_monitor, i_paras->val_jso, i_paras->uri);
    if (NULL != running_task) {
        if (g_strrstr(i_paras->uri, RF_IMPORT_ACTION_CERT_NAME)) {
            
            (void)create_message_info(MSGID_CERT_IMPORTING, RF_IMPORT_ACTION_CERT_NAME, o_message_jso);
            
        } else if (g_strrstr(i_paras->uri, RF_IMPORT_ACTION_SSH_NAME)) {
            (void)create_message_info(MSGID_TASK_SSH_IMPORT_ERR, RF_IMPORT_ACTION_SSH_NAME, o_message_jso);
        }

        (void)json_object_put(running_task);

        return HTTP_BAD_REQUEST;
    }

    (void)json_object_put(running_task);

    

    
    if (g_strrstr(i_paras->uri, RF_IMPORT_ACTION_CERT_NAME)) {
        file_id = FILE_ID_CLIENT_CER;
    } else if (g_strrstr(i_paras->uri, RF_IMPORT_ACTION_SSH_NAME)) {
        file_id = FILE_ID_SSH;
    }

    
    if (0 >= snprintf_s(file_transfer_uri, sizeof(file_transfer_uri), sizeof(file_transfer_uri) - 1, "download;%u;%s",
        file_id, transfer_protocol)) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    
    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &file_transfer_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_TRANSFERFILE);
        (void)memset_s(file_transfer_uri, MAX_URI_PATH_LENGTH, 0, MAX_URI_PATH_LENGTH);
        goto err_exit;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)file_transfer_uri));
    (void)memset_s(file_transfer_uri, MAX_URI_PATH_LENGTH, 0, MAX_URI_PATH_LENGTH);
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id));

    
    
    ret = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        (i_paras->auth_type == LOG_TYPE_LOCAL), i_paras->user_roleid, file_transfer_handle, CLASS_NAME_TRANSFERFILE,
        METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    

    uip_free_gvariant_list(input_list);

    if (VOS_OK != ret) {
        (void)parse_file_transfer_err_code(ret, o_message_jso);
        return HTTP_BAD_REQUEST;
    } else {
        import_monitor_data = import_monitor_info_new(i_paras);
        goto_label_do_info_if_fail(NULL != import_monitor_data, err_exit,
            debug_log(DLOG_ERROR, "alloc a new import monitor info failed"));

        import_monitor_data->task_progress = TASK_NO_PROGRESS; 
        import_monitor_data->rsc_privilege = USERROLE_USERMGNT;
        if (opt_user != NULL) {
            secure_ret = strcpy_s(import_monitor_data->task_owner, sizeof(import_monitor_data->task_owner), opt_user);
            if (secure_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret:%d", __FUNCTION__, secure_ret);
            }
        }
        
        // body_jso 中的内容就是 i_paras->val_jso;
        ret = create_new_task(RF_IMPORT_CLIENT_CERT_TASK_DESC, import_status_monitor, import_monitor_data, body_jso,
            i_paras->uri, o_result_jso);
        goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(import_monitor_data);
            debug_log(DLOG_ERROR, "create new task failed"));
    }

    
    return HTTP_ACCEPTED;

err_exit:
    ret = HTTP_INTERNAL_SERVER_ERROR;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return ret;
}


LOCAL gint32 task_account_import_client_cert(json_object *user_data, json_object *body_jso, guint8 user_id,
    gchar *filepath, json_object **o_message_jso)
{
    errno_t safe_fun_ret;
    
    guint32 ret;                                           // 函数的返回值
    OBJ_HANDLE obj_macc_handle;                                // 双因素客户端类的对象的句柄
    gchar cert_temp_path[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN]; // 证书文件所在的路径
    json_object *user_name_jso = NULL;
    json_object *client_jso = NULL;
    json_object *user_role_privilege_jso = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    guint8 user_role_priv;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    
    json_object *from_webui_jso = NULL;
    guchar from_webui_flag;
    

    // 获取函数调用的参数
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_NAME, &user_name_jso);
    user_name = dal_json_object_get_str(user_name_jso);

    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_CLIENT, &client_jso);
    client = dal_json_object_get_str(client_jso);

    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_ROLE, &user_role_privilege_jso);
    user_role_priv = json_object_get_int(user_role_privilege_jso);

    (void)json_object_object_get_ex(user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_jso);
    from_webui_flag = json_object_get_int(from_webui_jso);

    
    
    
    // 复制证书为方法默认的路径
    
    if (0 != g_strcmp0(TMP_CLIENT_PEM_FILE, filepath)) {
        // redfish模块降权限后，调用代理方法拷贝文件
        ret = proxy_copy_file(from_webui_flag, user_name, client, filepath, TMP_CLIENT_PEM_FILE, REDFISH_USER_UID,
            REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
        if (VOS_OK != ret) {
            (void)proxy_delete_file(from_webui_flag, user_name, client, filepath);
            (void)proxy_delete_file(from_webui_flag, user_name, client, TMP_CLIENT_PEM_FILE);

            debug_log(DLOG_ERROR, "%s: copy request cert to %s failed", __FUNCTION__, TMP_CLIENT_PEM_FILE);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    
    (void)dal_set_file_owner(TMP_CLIENT_PEM_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_CLIENT_PEM_FILE, (S_IRUSR | S_IWUSR));

    
    // 入 参1: 获取用户的ID

    // 入参2 : 获取用户的ID对应的双因素客户端证书对象的句柄
    ret = dal_get_specific_object_byte(MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_HASHID, user_id,
        &obj_macc_handle);
    return_val_do_info_if_fail(VOS_ERR != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj handle fail.\n", __FUNCTION__, __LINE__);
        json_object_clear_string(body_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 入参3 : 证书临时文件所在的路径
    safe_fun_ret = strcpy_s(cert_temp_path, sizeof(cert_temp_path), TMP_CLIENT_PEM_FILE);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    // 将入参user_id 和临时客户端证书文件的路径填入到inputlist表中
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)cert_temp_path));

    

    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client, obj_macc_handle,
        MUTUAL_AUTH_CLIENT_CLASS_NAME, MUTUAL_AUTH_CLIENT_CERT_METHOD_IMPORT, AUTH_DISABLE, user_role_priv, input_list,
        &output_list);
    

    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", MUTUAL_AUTH_CLIENT_CERT_METHOD_IMPORT, ret));

    
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    json_object_clear_string(body_jso, RFACTION_PARAM_TYPE);
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);

    return ret;
}



LOCAL gint32 task_account_import_ssh_publickey(json_object *user_data, json_object *body_jso, guint8 user_id,
    gchar *filepath, json_object **o_message_jso)
{
    errno_t safe_fun_ret;
    

    gint32 ret;             // 函数的返回值
    OBJ_HANDLE obj_user_handle; // 用户类的对象的句柄
    gchar public_key_temp_path[RF_ACCOUNT_MACLICERT_ELEMENT_MAXLEN];
    guint8 Keytype; // ssh公钥存在的方式: 文本，文件，待删除
    json_object *user_name_jso = NULL;
    json_object *client_jso = NULL;
    json_object *user_role_privilege_jso = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    guint8 user_role_priv;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    
    json_object *from_webui_jso = NULL;
    guchar from_webui_flag;
    

    // 调用UIP函数
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_NAME, &user_name_jso);
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_CLIENT, &client_jso);
    (void)json_object_object_get_ex(user_data, RF_IMPORT_USER_ROLE, &user_role_privilege_jso);
    (void)json_object_object_get_ex(user_data, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_jso);

    user_name = dal_json_object_get_str(user_name_jso);
    client = dal_json_object_get_str(client_jso);
    user_role_priv = json_object_get_int(user_role_privilege_jso);
    from_webui_flag = json_object_get_int(from_webui_jso);

    
    // 复制证书为方法默认的路径
    
    if (0 != g_strcmp0(TMP_SSH_PUBLIC_KEY_PUB_FILE, filepath)) {
        // redfish模块降权限后，调用代理方法拷贝文件
        ret = proxy_copy_file(from_webui_flag, user_name, client, filepath, TMP_SSH_PUBLIC_KEY_PUB_FILE,
            REDFISH_USER_UID, REDFISH_USER_GID, COPY_FILE_DELETE_FLAG);
        if (VOS_OK != ret) {
            (void)proxy_delete_file(from_webui_flag, user_name, client, filepath);
            (void)proxy_delete_file(from_webui_flag, user_name, client, TMP_SSH_PUBLIC_KEY_PUB_FILE);

            debug_log(DLOG_ERROR, "%s: copy request ssh publickey to %s failed", __FUNCTION__,
                TMP_SSH_PUBLIC_KEY_PUB_FILE);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    

    
    (void)dal_set_file_owner(TMP_SSH_PUBLIC_KEY_PUB_FILE, REDFISH_USER_UID, APPS_USER_GID);
    (void)dal_set_file_mode(TMP_SSH_PUBLIC_KEY_PUB_FILE, (S_IRUSR | S_IWUSR));

    
    // 入 参1: 获取用户的句柄
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &obj_user_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__);
        json_object_clear_string(body_jso, RFACTION_PARAM_TYPE);
        json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 入参2 : 获取ssh公钥存在的方式
    Keytype = USER_PUBLIC_KEY_TYPE_FILE;

    // 入参3 : 公钥临时文件所在的路径
    safe_fun_ret = strcpy_s(public_key_temp_path, sizeof(public_key_temp_path), TMP_SSH_PUBLIC_KEY_PUB_FILE);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    // 将入参user_id 和临时公钥文件的路径和公钥文件的存在方式填入到inputlist表中
    input_list = g_slist_append(input_list, g_variant_new_byte(user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(Keytype));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)public_key_temp_path));

    
    ret = uip_call_class_method_redfish(from_webui_flag, user_name, client, obj_user_handle, CLASS_USER,
        METHOD_USER_SETPUBLICKEY, AUTH_DISABLE, user_role_priv, input_list, &output_list);
    

    
    // 对UIP的返回值进行判断
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", METHOD_USER_SETPUBLICKEY, ret));

    // 获得类方法ImportClientCertificate 返回值
    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

    
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    json_object_clear_string(body_jso, RFACTION_PARAM_TYPE);
    json_object_clear_string(body_jso, RFACTION_PARAM_CONTENT);

    return ret;
}


gint32 account_provider_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    *prop_provider = g_account_action_info_provider;
    *count = sizeof(g_account_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 account_provider_oem_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    *prop_provider = g_account_provider;
    *count = sizeof(g_account_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 account_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    gchar               username[USER_USERNAME_MAX_LEN + 1] = {0};
    json_object *o_message_jso = NULL;

    

    ret = redfish_check_account_uri_effective(i_paras->member_id, i_paras->uri, username, sizeof(username), NULL,
        &o_message_jso);

    do_val_if_expr(NULL != o_message_jso, (void)json_object_put(o_message_jso));

    

    return_val_if_fail((VOS_OK == ret), ret);

    *prop_provider = g_account_provider;
    *count = sizeof(g_account_provider) / sizeof(PROPERTY_PROVIDER_S);

    

    return VOS_OK;
}
