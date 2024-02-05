
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"

#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_provider_resource.h"
#include "redfish_provider.h"


#define REDFISH_CREATE_ACCOUNT  1
#define REDFISH_MODIFY_ACCOUNT  0

typedef struct {
    OBJ_HANDLE user_handle;
    guint8 user_id;
    guint8 privilege;
    const gchar *user_name;
    const gchar *request_ip;
    const gchar *login_name;
    const gchar *password;
    guchar from_webui_flag;
    guint8 role_id;
} ACCOUNT_USER_INFO_S;


LOCAL user_privilege_info_s g_privilege_info[] = {
    {USER_ROLE_ADMIN,        UIP_USER_ADMIN,       UIP_USER_ADMIN},
    {USER_ROLE_OPERATOR,     UIP_USER_OPERATOR,    UIP_USER_OPERATOR},
    {USER_ROLE_COMMONUSER,   UIP_USER_COMMON,      UIP_USER_COMMON},
    {USER_ROLE_CUSTOMROLE1,  UIP_USER_OPERATOR,    UIP_USER_CUSTOMROLE1},
    {USER_ROLE_CUSTOMROLE2,  UIP_USER_OPERATOR,    UIP_USER_CUSTOMROLE2},
    {USER_ROLE_CUSTOMROLE3,  UIP_USER_OPERATOR,    UIP_USER_CUSTOMROLE3},
    {USER_ROLE_CUSTOMROLE4,  UIP_USER_OPERATOR,    UIP_USER_CUSTOMROLE4},
    {USER_ROLE_NOACCESS,     UUSER_ROLE_NOACCESS,  UUSER_ROLE_NOACCESS}
};

typedef struct _user_error_msg_id {
    gint32 err_code;
    const gchar *msg_id;
    const gchar *related_property;
    const gchar *err_des_var1;
    const gchar *err_des_var2;
} USER_ERR_MSG_S;
LOCAL USER_ERR_MSG_S g_error_message_id_tbl[] = {
    {USER_NAME_EXIST, MSGID_RSC_ALREADY_EXIST, NULL, NULL, NULL},
    {USER_NAME_RESTRICTED, MSGID_USERNAME_IS_RESTRICTED, NULL, NULL, NULL},
    {USER_PRIVILEGE_WRONG, MSGID_ROLEID_IS_RESTRICTED, NULL, NULL, NULL},
    {UUSER_USERNAME_INVALID, MSGID_INVALID_USERNAME, NULL, NULL, NULL},
    {UUSER_USERPASS_TOO_LONG, MSGID_INVALID_PSWD_LEN, PASSWORD, USER_PASSWORD_MIN_LEN_STR, USER_PWD_MAX_LEN_STR},
    {USER_PASS_COMPLEXITY_FAIL, MSGID_PSWD_CMPLX_CHK_FAIL, NULL, NULL, NULL},
    {UUSER_USERPASS_EMPTY, MSGID_INVALID_PAWD, NULL, NULL, NULL},
    {USER_SET_PASSWORD_EMPTY, MSGID_INVALID_PAWD, NULL, NULL, NULL},
    {USER_SET_PASSWORD_TOO_WEAK, MSGID_PSWD_IN_WEAK_PWDDICT, NULL, NULL, NULL},
    {UUSER_LINUX_DEFAULT_USER, MSGID_CONFLICT_WITH_LINUX_DEFAULT_USER, NULL, NULL, NULL},
    {USER_CREATE_CONFILIC_ERR, MSGID_CONFILICT_WITH_CREATE_ACCOUNTS, NULL, NULL, NULL}
};


LOCAL gint32 get_privilege_roleid_from_role(const gchar *role, guint8 *privilege, guint8 *role_id)
{
    gint32 i;

    
    gint32 privilege_info_count = (int)G_N_ELEMENTS(g_privilege_info);

    for (i = 0; i < privilege_info_count; i++) {
        
        if (!g_strcmp0(role, g_privilege_info[i].role)) {
            *privilege = g_privilege_info[i].privilege;
            *role_id = g_privilege_info[i].role_id;
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


LOCAL gint32 redfish_set_user_name(ACCOUNT_USER_INFO_S *account_info, guint8 create_account_flag) 
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_fail(
        ((NULL != account_info->user_name) && (NULL != account_info->request_ip) && (NULL != account_info->login_name)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s %d :set user %d name fail", __FUNCTION__, __LINE__, account_info->user_id));

    // 用户接口中用户名长度限制改为32位，各模块单独限制16位
    if (strlen(account_info->user_name) > 16) {
        debug_log(DLOG_ERROR, "set user %u name(%s) more than 16 character", account_info->user_id,
                  account_info->user_name);
        return USER_INVALID_DATA_FIELD;
    }

    // 填充入参信息
    input_list = g_slist_append(input_list, g_variant_new_byte(account_info->user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(account_info->user_name));
    if (create_account_flag == REDFISH_CREATE_ACCOUNT)  {
        input_list = g_slist_append(input_list, g_variant_new_string(""));
    }

    // 第一步调用方法设置用户名
    
    ret = uip_call_class_method_redfish(account_info->from_webui_flag, account_info->login_name,
        account_info->request_ip, account_info->user_handle, CLASS_USER, METHOD_USER_SETUSERNAME, AUTH_DISABLE,
        AUTH_PRIV_NONE, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :call method %s fail", __FUNCTION__, __LINE__, METHOD_USER_SETUSERNAME));

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s %d :call method %s return value 0x%x fail", __FUNCTION__, __LINE__,
            METHOD_USER_SETUSERNAME, ret);
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;

    return ret;
}


LOCAL gint32 redfish_set_user_password(ACCOUNT_USER_INFO_S *account_info)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_fail(((NULL != account_info->user_name) && (NULL != account_info->request_ip) &&
        (NULL != account_info->password) && (NULL != account_info->login_name)),
        VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d :set user %d password fail", __FUNCTION__, __LINE__, account_info->user_id));

    // 第二步调用方法设置密码
    input_list = g_slist_append(input_list, g_variant_new_byte(account_info->user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(account_info->password));
    input_list = g_slist_append(input_list, g_variant_new_byte(PWSWORDMAXLENGH));
    input_list = g_slist_append(input_list, g_variant_new_byte(OPERATION_SET_PASSWD));

    
    ret = uip_call_class_method_redfish(account_info->from_webui_flag, account_info->login_name,
        account_info->request_ip, account_info->user_handle, CLASS_USER, METHOD_USER_SETPASSWORD, AUTH_DISABLE,
        AUTH_PRIV_NONE, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :call method %s fail", __FUNCTION__, __LINE__, METHOD_USER_SETPASSWORD));

    

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s %d :call method %s return value %d fail", __FUNCTION__, __LINE__,
            METHOD_USER_SETPASSWORD, ret);
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    
    
    ret = (COMP_CODE_INVALID_FIELD == ret) ? UUSER_USERPASS_EMPTY : ret;
    

    return ret;
}


LOCAL gint32 redfish_set_user_enable(ACCOUNT_USER_INFO_S *account_info)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_fail(((NULL != account_info->user_name) && (NULL != account_info->request_ip)), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d :set user %d enable state fail", __FUNCTION__, __LINE__, account_info->user_id));

    input_list = g_slist_append(input_list, g_variant_new_byte(account_info->user_id));
    input_list = g_slist_append(input_list, g_variant_new_string(account_info->password));
    input_list = g_slist_append(input_list, g_variant_new_byte(PWSWORDMAXLENGH));
    input_list = g_slist_append(input_list, g_variant_new_byte(OPERATION_ENABLE_USER));

    
    ret = uip_call_class_method_redfish(account_info->from_webui_flag, account_info->login_name,
        account_info->request_ip, account_info->user_handle, CLASS_USER, METHOD_USER_SETPASSWORD, AUTH_DISABLE,
        AUTH_PRIV_NONE, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :call method %s set enable state fail", __FUNCTION__, __LINE__,
        METHOD_USER_SETPASSWORD));

    

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s %d :call method %s set enable state return value %d fail", __FUNCTION__, __LINE__,
            METHOD_USER_SETPASSWORD, ret);
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return ret;
}


LOCAL gint32 redfish_set_user_privilege(ACCOUNT_USER_INFO_S *account_info)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_fail(((NULL != account_info->user_name) && (NULL != account_info->request_ip)), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d :set user %d privilege fail", __FUNCTION__, __LINE__, account_info->user_id));

    input_list = g_slist_append(input_list, g_variant_new_byte(account_info->user_id));
    input_list = g_slist_append(input_list, g_variant_new_byte(account_info->privilege));
    input_list = g_slist_append(input_list, g_variant_new_byte(account_info->role_id));

    
    ret = uip_call_class_method_redfish(account_info->from_webui_flag, account_info->login_name,
        account_info->request_ip, account_info->user_handle, CLASS_USER, METHOD_USER_SETPRIVILEGE, AUTH_DISABLE,
        AUTH_PRIV_NONE, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :call method %s set privilege fail", __FUNCTION__, __LINE__,
        METHOD_USER_SETPRIVILEGE));
    

    ret = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s %d :call method %s return value %d fail", __FUNCTION__, __LINE__,
            METHOD_USER_SETPRIVILEGE, ret);
    }

    return ret;
}


LOCAL gint32 redfish_set_user_roleid(guchar from_webui_flag, OBJ_HANDLE user_handle, const gchar *user_name,
    guint8 role_id, const gchar *request_ip, const gchar *login_name)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    return_val_do_info_if_fail(((NULL != user_name) && (NULL != request_ip)), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d :set user %s roleid fail", __FUNCTION__, __LINE__, user_name));

    input_list = g_slist_append(input_list, g_variant_new_byte(role_id));

    
    ret = uip_call_class_method_redfish(from_webui_flag, login_name, request_ip, user_handle, CLASS_USER,
        METHOD_USER_SETUSERROLEID, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, &output_list);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :call method %s set roleid fail", __FUNCTION__, __LINE__,
        METHOD_USER_SETUSERROLEID));
    

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return ret;
}


LOCAL gint32 add_account_to_sys(guchar from_webui_flag, guint8 *i_user_id, const gchar *user_name,
    const gchar *password, const gchar *role, const gchar *request_ip, const gchar *login_name)
{
    guint8 privilege = 0;
    OBJ_HANDLE user_handle;
    guint8 role_id = 0;

    ACCOUNT_USER_INFO_S account_info;

    return_val_do_info_if_fail(((NULL != user_name) && (NULL != password) && (NULL != role) && (NULL != request_ip)),
        VOS_ERR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    guint8 user_id = *i_user_id;

    // 获取privilege借鉴web将role_id转换为用户权限
    gint32 ret = get_privilege_roleid_from_role(role, &privilege, &role_id);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s %d :get_privilege_roleid_from_role fail, ret is %d", __FUNCTION__, __LINE__,
        privilege));

    // 根据id获取user用户的句柄
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &user_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s %d :get user %d fail", __FUNCTION__, __LINE__, user_id));

    (void)memset_s(&account_info, sizeof(account_info), 0, sizeof(account_info));
    account_info.user_handle = user_handle;
    account_info.user_id = user_id;
    account_info.user_name = user_name;
    account_info.request_ip = request_ip;
    account_info.login_name = login_name;
    account_info.password = password;
    account_info.privilege = privilege;
    account_info.from_webui_flag = from_webui_flag;
    account_info.role_id = role_id;

    // 第一步调用方法设置用户名, 设置失败User模块内自己会善后
    ret = redfish_set_user_name(&account_info, REDFISH_CREATE_ACCOUNT);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s fail", __FUNCTION__, METHOD_USER_SETUSERNAME);
        return ret;
    }

    // 第二步调用方法设置密码
    ret = redfish_set_user_password(&account_info);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d :call method %s fail", __FUNCTION__, __LINE__, METHOD_USER_SETPASSWORD));

    // 第三步调用方法使能用户
    ret = redfish_set_user_enable(&account_info);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d :call method %s set enable state fail", __FUNCTION__, __LINE__,
        METHOD_USER_SETPASSWORD));

    // 第四步调用方法设置用户权限
    ret = redfish_set_user_privilege(&account_info);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d :call method %s set user privilege fail", __FUNCTION__, __LINE__,
        METHOD_USER_SETPRIVILEGE));

    ret = redfish_set_user_roleid(from_webui_flag, user_handle, user_name, role_id, request_ip, login_name);
    goto_label_do_info_if_fail((VOS_OK == ret), error_exit,
        debug_log(DLOG_ERROR, "%s %d :call method %s set user roleid fail", __FUNCTION__, __LINE__,
        METHOD_USER_SETUSERROLEID));

    *i_user_id = user_id;

    return ret;
error_exit:

    // 中间过程设置失败则清空用户名(即删除用户),设置用户名阶段失败无需删除,User内部有善后处理
    account_info.user_name = "";
    (void)redfish_set_user_name(&account_info, REDFISH_MODIFY_ACCOUNT);
    return ret;
}


LOCAL gint32 _rf_get_avaliable_user_id(json_object *body_jso_checked, guchar *user_id, json_object **o_message_jso)
{
    gint32 ret;
    guint32 input_user_id = 0;
    const gchar *input_user_id_str = NULL;
    guchar i;
    OBJ_HANDLE obj_handle = 0;
    gchar name[RF_MAX_USER_NAME_LEN] = {0};

    return_val_do_info_if_fail((NULL != body_jso_checked) && (NULL != user_id) && (NULL != o_message_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__));

    
    for (i = USER_MIN_USER_NUMBER; i < USER_MAX_USER_NUMBER; i++) {
        (void)dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, i, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, name, RF_MAX_USER_NAME_LEN);

        if (0 == name[0]) {
            break;
        }
    }

    // 判断是否超出最大账户限制，若超出采用create族接口生成错误信息后返回
    return_val_do_info_if_fail(i < USER_MAX_USER_NUMBER, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s %d: user num limit, user id is %d", __FUNCTION__, __LINE__, i);
        (void)create_message_info(MSGID_CREATE_LIMIT_RSC, NULL, o_message_jso));

    (void)get_element_str(body_jso_checked, RFPROP_ID, &input_user_id_str);

    
    if (NULL == input_user_id_str) {
        *user_id = i;

        return VOS_OK;
    }

    
    ret = vos_str2int(input_user_id_str, 10, (void *)(guint32 *)&input_user_id, NUM_TPYE_UINT32);
    return_val_do_info_if_fail((VOS_OK == ret) && (input_user_id >= USER_MIN_USER_NUMBER) &&
        (input_user_id < USER_MAX_USER_NUMBER),
        VOS_ERR, debug_log(DLOG_ERROR, "%s %d: invalid input user id [%s]", __FUNCTION__, __LINE__, input_user_id_str);
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_ID, o_message_jso, RFPROP_ID));

    (void)dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, (guchar)input_user_id, &obj_handle);
    ret = dal_get_property_value_string(obj_handle, PROPERTY_USER_NAME, name, RF_MAX_USER_NAME_LEN);
    return_val_do_info_if_fail((VOS_OK == ret) && ('\0' == name[0]), VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: invalid input user id [%s]", __FUNCTION__, __LINE__, input_user_id_str);
        (void)create_message_info(MSGID_PROPERTY_VALUE_ERR, RFPROP_ID, o_message_jso, RFPROP_ID));

    *user_id = input_user_id;

    return VOS_OK;
}


LOCAL gint32 parse_create_account_info(json_object *body_jso_checked, json_object *o_message_array_jso,
    const gchar **user_name, const gchar **word, const gchar **role_id)
{
    json_bool is_ret;
    json_object *message_info_jso = NULL;
    json_object *val_jso = NULL;

    // 从请求体中解析用户名、密码、角色
    is_ret = json_object_object_get_ex(body_jso_checked, USERNAME, &val_jso);
    if (!is_ret) {
        debug_log(DLOG_ERROR, "%s %d: json_object_object_get_ex %s fail", __FUNCTION__, __LINE__, USERNAME);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *user_name = dal_json_object_get_str(val_jso);
    // 不支持创建用户名为空
    if (*user_name == NULL || strlen(*user_name) == 0) {
        debug_log(DLOG_ERROR, "%s %d :input user name null not support", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INVALID_USERNAME, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_BAD_REQUEST;
    }

    val_jso = NULL;
    is_ret = json_object_object_get_ex(body_jso_checked, PASSWORD, &val_jso);
    if (!is_ret) {
        debug_log(DLOG_ERROR, "%s %d: json_object_object_get_ex %s fail", __FUNCTION__, __LINE__, PASSWORD);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *word = dal_json_object_get_str(val_jso);
    val_jso = NULL;
    if (*word == NULL) {
        debug_log(DLOG_ERROR, "%s %d: dal_json_object_get_str %s null", __FUNCTION__, __LINE__, PASSWORD);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    is_ret = json_object_object_get_ex(body_jso_checked, ROLEID, &val_jso);
    if (!is_ret) {
        debug_log(DLOG_ERROR, "%s %d: json_object_object_get_ex %s fail", __FUNCTION__, __LINE__, ROLEID);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *role_id = dal_json_object_get_str(val_jso);
    if (*role_id == NULL) {
        debug_log(DLOG_ERROR, "%s %d: dal_json_object_get_str %s null", __FUNCTION__, __LINE__, ROLEID);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return VOS_OK;
}

LOCAL void __parase_create_account_error(const gint32 error_code, const gchar *user_name, 
    const gchar *role_id, json_object **o_message_jso)
{
    guint32 i;
    if (error_code == RET_OK) {
        return;
    }

    for (i = 0; i < G_N_ELEMENTS(g_error_message_id_tbl); i++) {
        if (error_code == g_error_message_id_tbl[i].err_code) {
            if (g_error_message_id_tbl[i].related_property == NULL) {
                (void)create_message_info(g_error_message_id_tbl[i].msg_id, NULL, o_message_jso);
            } else {
                (void)create_message_info(g_error_message_id_tbl[i].msg_id, g_error_message_id_tbl[i].related_property, 
                    o_message_jso, g_error_message_id_tbl[i].err_des_var1, g_error_message_id_tbl[i].err_des_var2);
            }
            return;  
        }
    }
    if (error_code == USER_INVALID_DATA_FIELD) {
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, USERNAME, o_message_jso, user_name,
            USERNAME, USER_NAME_MAX_LEN_STR);
        return;
    }
    
    if (error_code == HTTP_BAD_REQUEST) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, ROLEID, o_message_jso, role_id, ROLEID);
        return;
    }
    
    (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, NULL, o_message_jso);
}


gint32 create_account(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso)
{
    gint32 ret;
    guint8 user_id;
    const gchar *user_name = NULL;
    const gchar *word = NULL; // 密码
    const gchar *role_id = NULL;
    json_object *message_info_jso = NULL;

    return_val_do_info_if_fail(
        ((NULL != o_id) && (NULL != o_message_array_jso) && (NULL != i_param) && (NULL != body_jso_checked)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        json_object_clear_string(body_jso_checked, PASSWORD));
    // 判断用户是否具有用户管理权限,没有直接返回禁止
    return_val_do_info_if_fail(0 != (i_param->user_role_privilege & USERROLE_USERMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_ERROR, "%s %d: user has no privilege, user is %s", __FUNCTION__, __LINE__, i_param->user_name);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        json_object_clear_string(body_jso_checked, PASSWORD));

    
    ret = _rf_get_avaliable_user_id(body_jso_checked, &user_id, &message_info_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s %d: get avaliable user id failed, ret is %d", __FUNCTION__, __LINE__, ret);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        json_object_clear_string(body_jso_checked, PASSWORD));

    ret = parse_create_account_info(body_jso_checked, o_message_array_jso, &user_name, &word, &role_id);
    return_val_do_info_if_expr(ret != VOS_OK, ret, json_object_clear_string(body_jso_checked, PASSWORD));

    
    ret = add_account_to_sys(i_param->is_from_webui, &user_id, user_name, word, role_id, i_param->client,
        i_param->user_name);
    json_object_clear_string(body_jso_checked, PASSWORD); // 密码信息使用完成立即删除
    

    if (VOS_OK != ret) {
        __parase_create_account_error(ret, user_name, role_id, &message_info_jso);
        debug_log(DLOG_ERROR, "%s %d: add_account_to_sys fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
        json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_BAD_REQUEST;
        goto error_exit;
    }
    return_val_do_info_if_expr(id_len == 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed:input param error, id_len is 0.", __FUNCTION__));
    (void)snprintf_s(o_id, id_len, id_len - 1, "%u", user_id);

error_exit:

    return ret;
}


gint32 delete_account(PROVIDER_PARAS_S *i_param, json_object *message_array_jso)
{
    gint32 ret;
    guint8 user_id;
    OBJ_HANDLE user_handle;
    json_object *message_info_jso = NULL;
    gchar user_name[RF_MAX_USER_NAME_LEN + 1] = {0};

    ACCOUNT_USER_INFO_S account_info;

    // 判断用户是否具有用户管理权限,没有直接返回禁止
    return_val_do_info_if_fail(0 != (i_param->user_role_privilege & USERROLE_USERMGNT), HTTP_FORBIDDEN,
        debug_log(DLOG_ERROR, "%s %d: user has no privilege, user is %s", __FUNCTION__, __LINE__, i_param->user_name);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    return_val_do_info_if_fail(USERID_MAX_LEN >= strlen(i_param->member_id), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s %d :member id %s is illegal", __FUNCTION__, __LINE__, i_param->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    ret = vos_str2int(i_param->member_id, 10, &user_id, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail((VOS_OK == ret && MIN_USER_ID <= user_id && MAX_USER_ID >= user_id), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s %d :member id %s is illegal, change to int fail", __FUNCTION__, __LINE__,
        i_param->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    // 根据id获取user用户的句柄
    ret = dal_get_specific_object_byte(CLASS_USER, PROPERTY_USER_ID, user_id, &user_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s %d :get user %d fail", __FUNCTION__, __LINE__, user_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    (void)dal_get_property_value_string(user_handle, PROPERTY_USER_NAME, user_name, RF_MAX_USER_NAME_LEN + 1);
    return_val_do_info_if_fail(0 != strlen(user_name), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s %d :user %d name is null", __FUNCTION__, __LINE__, user_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    (void)memset_s(&account_info, sizeof(account_info), 0, sizeof(account_info));
    account_info.user_handle = user_handle;
    account_info.user_id = user_id;
    account_info.user_name = "";
    account_info.request_ip = i_param->client;
    account_info.login_name = i_param->user_name;
    
    account_info.from_webui_flag = i_param->is_from_webui;
    

    ret = redfish_set_user_name(&account_info, REDFISH_MODIFY_ACCOUNT);

    return_val_do_info_if_expr(ret == VOS_OK, VOS_OK,
        debug_log(DLOG_DEBUG, "%s %d :delete user %d successfully.\n", __FUNCTION__, __LINE__, user_id);
        (void)create_message_info(MSGID_ACCOUNT_REMOVED, NULL, &message_info_jso);
        (void)json_object_array_add(message_array_jso, message_info_jso));

    // 生成AccountRemoved消息
    switch (ret) {
        
        case USER_DELETE_TRAPV3_USER_UNSUPPORT:
        case USER_DELETE_EXCLUDE_USER_UNSURPPORT:
            
            (void)create_message_info(MSGID_ACCOUNT_FORBID_REMOVED, NULL, &message_info_jso);
            break;
        case USER_LAST_ADMIN_ERR:
            (void)create_message_info(MSGID_DELETE_LAST_ADMIN, NULL, &message_info_jso);
            break;
        default:
            (void)create_message_info(MSGID_ACCOUNT_NOT_MODIFIED, NULL, &message_info_jso);
    }

    debug_log(DLOG_ERROR, "%s %d :delete user %d failed.\n", __FUNCTION__, __LINE__, user_id);
    json_object_array_add(message_array_jso, message_info_jso);
    return HTTP_BAD_REQUEST;
}


gint32 generate_account_response_headers(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso)
{
    return generate_rsc_create_response_headers(i_param, id, header_array_jso, message_array_jso);
}


gint32 generate_del_account_response_headers(PROVIDER_PARAS_S *i_param, json_object **header_array_jso,
    const gchar *uri, json_object *message_array_jso)
{
    return generate_del_common_response_headers(i_param, header_array_jso, uri, message_array_jso);
}
