
#include "redfish_provider.h"
#include "../apps/bmc_global/inc/bmc.h"
#include <ldap.h>

#define LOGIN_RULE_MAX_ID 8
#define LOGIN_RULE_MIN_ID 2
#define NOACCESS_LOGIN_OFFSET 7
#define LOGIN_INTERFACE_MAX_COUNT 3


typedef struct priv_type {
    guchar priv_num;
    gchar priv_str[ARRAY_LENTH];
} PRIV_TYPE;
LOCAL PRIV_TYPE priv_arr_krb[] = {{2, "Common User"}, {3, "Operator"}, {4, "Administrator"}, {5, "Custom Role 1"},
                                  {6, "Custom Role 2"}, {7, "Custom Role 3"}, {8, "Custom Role 4"}, {15, "No Access"}
                                 };

LOCAL gint32 krb_get_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_groups_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_set_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_set_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_set_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_set_controller_groups(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_import_krb_keytab_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_acion_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 krb_get_controller_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_data_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 krb_get_controller_object_handle(guchar uriMemberId, OBJ_HANDLE *objHandle);

LOCAL PROPERTY_PROVIDER_S kerberos_controller_provider[] = {
    {
        RFPROP_ODATA_CONTEXT,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        krb_get_controller_odata_context,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },

    {
        RFPROP_ODATA_ID,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        krb_get_controller_odata_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_ID,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        krb_get_controller_data_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_KRB_SERVICE_ADDRESS,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        krb_get_controller_addr,
        krb_set_controller_addr,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_KRB_SERVICE_PORT,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        krb_get_controller_port,
        krb_set_controller_port,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_KRB_SERVICE_REALM,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        krb_get_controller_domain,
        krb_set_controller_domain,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_KRB_SERVICE_KRBGROUPS,
        LDAP_GROUP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        krb_get_controller_groups_info,
        krb_set_controller_groups,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACTIONS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        krb_get_controller_actions,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_KRB_SERVICE_IMPORT_METHOD,
        LDAP_CLASS_NAME, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_import_krb_keytab_entry, ETAG_FLAG_ENABLE
    }
};

LOCAL gint32 check_controller_id(const gchar *uri)
{
    gint32 ret;
    gchar *ctrlIdStr = NULL;
    guchar objectId = 0;
    GSList *controllerList = NULL;
    GSList *controllerNode = NULL;
    gchar tmpStr[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(NULL != uri, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ctrlIdStr = g_strrstr(uri, "/");
    ret = dfl_get_object_list(LDAP_CLASS_NAME, &controllerList);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Get controller list failed.", __FUNCTION__));

    for (controllerNode = controllerList; controllerNode; controllerNode = controllerNode->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)controllerNode->data, LDAP_ATTRIBUTE_ID, &objectId);
        if (VOS_OK != ret) {
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret = %d.", __FUNCTION__, ret);
            continue;
        }
        if (objectId >= LOG_TYPE_LDAP_SERVER6) {
            (void)snprintf_s(tmpStr, sizeof(tmpStr), sizeof(tmpStr) - 1, "/%d", (objectId - LOG_TYPE_KRB_SERVER + 1));

            if (VOS_OK == g_strcmp0(ctrlIdStr, tmpStr)) {
                g_slist_free(controllerList);
                return VOS_OK;
            }
        }
    }

    g_slist_free(controllerList);

    debug_log(DLOG_ERROR, "%s: not found controller.", __FUNCTION__);
    return VOS_ERR;
}

LOCAL gint32 krb_get_controller_object_handle(guchar uriMemberId, OBJ_HANDLE *objHandle)
{
    gint32 ret;
    guchar krb_server_id = 0;
    GSList *controller_list = NULL;
    GSList *controller_node = NULL;

    return_val_do_info_if_fail(NULL != objHandle && uriMemberId > 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = dfl_get_object_list(LDAP_CLASS_NAME, &controller_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Get controller list failed.", __FUNCTION__));

    for (controller_node = controller_list; controller_node; controller_node = controller_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)controller_node->data, LDAP_ATTRIBUTE_ID, &krb_server_id);
        continue_do_info_if_expr(VOS_OK != ret,
            debug_log(DLOG_DEBUG, "%s : dal_get_property_value_byte failed, ret = %d.\r\n", __FUNCTION__, ret));

        continue_if_expr(krb_server_id < LOG_TYPE_LDAP_SERVER6);

        break_do_info_if_expr((uriMemberId - 1) == (krb_server_id - LOG_TYPE_KRB_SERVER),
            (*objHandle = (OBJ_HANDLE)controller_node->data));
    }
    g_slist_free(controller_list);
    return VOS_OK;
}

LOCAL gint32 krb_get_controller_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar resultStr[ARRAY_LENTH] = {0};
    gint32 ret;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = snprintf_s(resultStr, ARRAY_LENTH, ARRAY_LENTH - 1, RFPROP_KRB_ODATA_CONTEXT_STR, i_paras->member_id);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const char *)resultStr);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 krb_get_controller_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar resultStr[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = snprintf_s(resultStr, ARRAY_LENTH, ARRAY_LENTH - 1, RFPROP_KRB_COLLECTION_MEMBER, i_paras->member_id);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    *o_result_jso = json_object_new_string((const char *)resultStr);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 krb_get_controller_data_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar ctrlId = 0;
    gchar strFormatOfId[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = dal_get_property_value_byte(i_paras->obj_handle, LDAP_ATTRIBUTE_ID, &ctrlId);
    if (VOS_OK != ret) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ctrlId = ctrlId - (LOG_TYPE_KRB_SERVER - 1);
    (void)snprintf_s(strFormatOfId, ARRAY_LENTH, ARRAY_LENTH - 1, "%u", ctrlId);
    *o_result_jso = json_object_new_string((const char *)strFormatOfId);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 krb_get_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar hostAddr[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = dal_get_property_value_string(i_paras->obj_handle, LDAP_ATTRIBUTE_HOST_ADDR, hostAddr, sizeof(hostAddr));
    if (VOS_OK != ret) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_expr(VOS_OK == g_strcmp0(PARAM_DEFAULT_VALUE_EMPTY_STR, hostAddr), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: HostAddr is not configured.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)hostAddr);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 krb_set_controller_addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *addrStr = NULL;
    gchar tmpStr[ARRAY_LENTH] = {0};
    GSList *inputList = NULL;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    addrStr = dal_json_object_get_str(i_paras->val_jso);
    if (PROPERTY_LDAP_SERVICE_DTR_LENTH < strlen(addrStr)) {
        (void)snprintf_s(tmpStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, addrStr, RFPROP_KRB_SERVICE_ADDRESS,
            (const char *)tmpStr);
        return HTTP_BAD_REQUEST;
    }

    
    inputList = g_slist_append(inputList, g_variant_new_string(addrStr));

    
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_HOST_ADDR, AUTH_ENABLE, i_paras->user_role_privilege, inputList, NULL);
    uip_free_gvariant_list(inputList);

    
    return get_set_function_normal_ret(i_paras, ret, o_message_jso, addrStr, RFPROP_KRB_SERVICE_ADDRESS);
}

LOCAL gint32 krb_get_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 portNum = 0;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    ret = dal_get_property_value_int32(i_paras->obj_handle, LDAP_ATTRIBUTE_PORT, &portNum);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get port failed.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_int(portNum);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 krb_set_controller_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 portNum;
    GSList *inputList = NULL;
    gchar portStr[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(json_type_int == json_object_get_type(i_paras->val_jso), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_KRB_SERVICE_PORT, o_message_jso,
        dal_json_object_get_str(i_paras->val_jso), RFPROP_KRB_SERVICE_PORT));

    portNum = json_object_get_int(i_paras->val_jso);

    inputList = g_slist_append(inputList, g_variant_new_int32(portNum));

    
    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_PORT, AUTH_ENABLE, i_paras->user_role_privilege, inputList, NULL);
    uip_free_gvariant_list(inputList);

    (void)snprintf_s(portStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%d", portNum);
    return get_set_function_normal_ret(i_paras, ret, o_message_jso, portStr, RFPROP_KRB_SERVICE_PORT);
}

LOCAL gint32 krb_get_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar userDomain[TMP_STR_MAX] = {0};

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret =
        dal_get_property_value_string(i_paras->obj_handle, LDAP_ATTRIBUTE_USER_DOMAIN, userDomain, sizeof(userDomain));
    if (VOS_OK != ret) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_expr(VOS_OK == g_strcmp0(PARAM_DEFAULT_VALUE_EMPTY_STR, userDomain),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_DEBUG, "%s: Realm is not configured.", __FUNCTION__));

    *o_result_jso = json_object_new_string((const char *)userDomain);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 krb_set_controller_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    const gchar *userDomain = NULL;
    gchar tmpStr[ARRAY_LENTH] = {0};
    GSList *inputList = NULL;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    userDomain = dal_json_object_get_str(i_paras->val_jso);
    if (PROPERTY_LDAP_SERVICE_DTR_LENTH < strlen(userDomain)) {
        (void)snprintf_s(tmpStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%d", PROPERTY_LDAP_SERVICE_DTR_LENTH);
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, NULL, o_message_jso, userDomain, RFPROP_KRB_SERVICE_REALM,
            (const char *)tmpStr);
        return HTTP_BAD_REQUEST;
    }

    inputList = g_slist_append(inputList, g_variant_new_string(userDomain));

    ret =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        LDAP_CLASS_NAME, LDAP_METHOD_SET_USER_DOMAIN, AUTH_ENABLE, i_paras->user_role_privilege, inputList, NULL);

    uip_free_gvariant_list(inputList);

    return get_set_function_normal_ret(i_paras, ret, o_message_jso, userDomain, RFPROP_KRB_SERVICE_REALM);
}

LOCAL gint32 krb_get_group_login_rule(json_object *groupInfo, OBJ_HANDLE objHandle)
{
    gchar slotStr[MAX_RSC_ID_LEN] = {0};
    gchar roleStr[TMP_STR_MAX] = {0};
    guchar groupPermitRole = 0;
    guint32 i_index;
    json_object *permitRoleArray = NULL;
    json_object *permitRoleJsonObj = NULL;

    return_val_do_info_if_fail(NULL != groupInfo, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    gint32 ret = redfish_get_board_slot(slotStr, sizeof(slotStr));
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    
    (void)dal_get_property_value_byte(objHandle, LDAP_GROUP_ATTRIBUTE_GROUP_PERMIT_ID, &groupPermitRole);

    permitRoleArray = json_object_new_array();

    for (i_index = 0; i_index < LOGRULE_3; i_index++) {
        if ((groupPermitRole >> i_index) & 1) {
            permitRoleJsonObj = json_object_new_object();
            if (NULL == permitRoleJsonObj) {
                continue;
            }

            ret = snprintf_s(roleStr, TMP_STR_MAX, TMP_STR_MAX - 1, URI_FORMAT_LDAP_SERVICE_PCIEDEVICES_URI, slotStr,
                (i_index + 1));
            if (VOS_OK == ret) {
                json_object_put(permitRoleJsonObj);
                continue;
            }

            json_object_object_add(permitRoleJsonObj, PROPERTY_LDAP_SERVICE_ODATA_ID, 
                json_object_new_string((const char *)roleStr));
            json_object_array_add(permitRoleArray, permitRoleJsonObj);
        }
    }

    json_object_object_add(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE, permitRoleArray);

    return VOS_OK;
}

LOCAL gint32 krb_get_group_login_interface(json_object *groupInfo, OBJ_HANDLE objHandle)
{
    guint32 loginInterface = 0;
    json_object *loginInterfaceArray = NULL;
    guint32 roleNum[] = {RFPROP_LOGIN_INTERFACE_WEB_NUM, USER_LOGIN_INTERFACE_REDFISH_OFFSET};
    const gchar* interfaceArray[] = {RFPROP_LOGIN_INTERFACE_WEB, RFPROP_LOGIN_INTERFACE_REDFISH};
    guint32 i_index;

    return_val_do_info_if_fail(NULL != groupInfo, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_uint32(objHandle, LDAP_GROUP_ATTRIBUTE_LOGIN_INTERFACE, &loginInterface);
    loginInterfaceArray = json_object_new_array();
    return_val_if_expr(NULL == loginInterfaceArray, VOS_ERR);

    for (i_index = 0; i_index < sizeof(roleNum) / sizeof(guint32); i_index++) {
        if ((loginInterface >> roleNum[i_index]) & 1) {
            do_if_expr(NULL != interfaceArray[i_index],
                json_object_array_add(loginInterfaceArray, json_object_new_string(interfaceArray[i_index])));
        }
    }

    json_object_object_add(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE, loginInterfaceArray);

    return VOS_OK;
}

LOCAL void krb_group_add_prop(json_object* groupInfo, const gchar* prop_name, const gchar* prop_val)
{
    if (g_strcmp0(PARAM_DEFAULT_VALUE_EMPTY_STR, prop_val) != RET_OK) {
        json_object_object_add(groupInfo, prop_name, json_object_new_string(prop_val));
    } else {
        json_object_object_add(groupInfo, prop_name, NULL);
    }
}


LOCAL gint32 krb_get_controller_groups_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *groupList = NULL;
    GSList *groupNode = NULL;
    guchar serverId = 0;
    guchar groupId = 0;
    guchar groupRuleId = 0;
    gchar groupName[TMP_STR_MAX] = {0};
    gchar groupDomain[TMP_STR_MAX] = {0};
    gchar groupSID[TMP_STR_MAX] = {0};
    json_object *groupInfo = NULL;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    gint32 ret = dfl_get_object_list(LDAP_GROUP_CLASS_NAME, &groupList);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: Get group list failed.", __FUNCTION__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, VOS_ERR, g_slist_free(groupList);
        groupList = NULL;
        debug_log(DLOG_ERROR, "%s: Get new json array failed.", __FUNCTION__));

    for (groupNode = groupList; NULL != groupNode; groupNode = groupNode->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_LDAPSERVER_ID, &serverId);
        continue_if_expr(VOS_OK != ret || LOG_TYPE_KRB_SERVER > (serverId & 0x0f));

        ret = dal_get_property_value_byte((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_GROUP_ID, &groupId);
        continue_if_expr(VOS_OK != ret);

        ret = dal_get_property_value_string((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_GROUP_NAME, groupName,
            sizeof(groupName));
        continue_if_expr(VOS_OK != ret);

        ret =
            dal_get_property_value_string((OBJ_HANDLE)groupNode->data, LDAP_GROUP_SID_INFO, groupSID, sizeof(groupSID));
        continue_if_expr(VOS_OK != ret);

        
        ret = dal_get_property_value_string((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_GROUP_FOLDER, groupDomain,
            sizeof(groupDomain));
        continue_if_expr(VOS_OK != ret);
        

        groupInfo = json_object_new_object();
        continue_if_expr(NULL == groupInfo);

        json_object_object_add(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_ID, json_object_new_int((gint32)groupId));

        krb_group_add_prop(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_NAME, groupName);
        krb_group_add_prop(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, groupDomain);
        krb_group_add_prop(groupInfo, RFPROP_KRB_SERVICE_GROUP_SID, groupSID);

        ret = dal_get_property_value_byte((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_GROUP_USERROLEID,
            &groupRuleId);
        if (LOGIN_RULE_MAX_ID >= groupRuleId && LOGIN_RULE_MIN_ID <= groupRuleId && VOS_OK == ret) {
            json_object_object_add(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE,
                json_object_new_string(priv_arr_krb[groupRuleId - LOGIN_RULE_MIN_ID].priv_str));
        } else if (VOS_OK == ret) {
            json_object_object_add(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE,
                json_object_new_string(priv_arr_krb[NOACCESS_LOGIN_OFFSET].priv_str));
        } else {
            json_object_object_add(groupInfo, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE, NULL);
        }

        ret = krb_get_group_login_rule(groupInfo, (OBJ_HANDLE)groupNode->data);
        continue_do_info_if_expr(VOS_OK != ret, json_object_put(groupInfo); (groupInfo = NULL));

        ret = krb_get_group_login_interface(groupInfo, (OBJ_HANDLE)groupNode->data);
        continue_do_info_if_expr(VOS_OK != ret, json_object_put(groupInfo); (groupInfo = NULL));

        json_object_array_add(*o_result_jso, groupInfo);
    }

    g_slist_free(groupList);

    return HTTP_OK;
}

LOCAL gint32 krb_set_group_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE objHandle,
    json_object *groupObject, gint32 groupId)
{
    gint32 safe_fun_ret;
    gint32 ret;
    json_object *groupNameObject = NULL;
    const gchar *groupNameStr = NULL;
    GSList *inputList = NULL;
    gchar messageStr[ARRAY_LENTH] = {0};
    json_object *tmpMessageObj = NULL;

    return_val_do_info_if_fail(NULL != groupObject, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    gboolean isFind = json_object_object_get_ex(groupObject, PROPERTY_LDAP_SERVICE_GROUP_NAME, &groupNameObject);
    return_val_do_info_if_fail(FALSE != isFind, VOS_OK,
        debug_log(DLOG_DEBUG, "%s: Not found group name.", __FUNCTION__));

    
    groupNameStr = dal_json_object_get_str(groupNameObject);
    if (strlen(groupNameStr) <= 0) {
        safe_fun_ret = snprintf_s(messageStr, ARRAY_LENTH, ARRAY_LENTH - 1, "KerberosGroups/%d/%s", groupId,
            PROPERTY_LDAP_SERVICE_GROUP_NAME);
        do_val_if_expr(safe_fun_ret <= 0,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, (const gchar*)messageStr, &tmpMessageObj, 
            groupNameStr, (const gchar*)messageStr);
        (void)json_object_array_add(*o_message_jso, tmpMessageObj);
        debug_log(DLOG_DEBUG, "%s: group name is empty.", __FUNCTION__);
        return VOS_ERR;
    }

    

    
    inputList = g_slist_append(inputList, g_variant_new_string(groupNameStr));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_NAME, AUTH_ENABLE, i_paras->user_role_privilege, inputList,
        NULL);
    uip_free_gvariant_list(inputList);
    safe_fun_ret = snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1, "KerberosGroups/%d/%s", groupId,
        PROPERTY_LDAP_SERVICE_GROUP_NAME);
    do_val_if_expr(safe_fun_ret <= 0,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, groupNameStr);
}

LOCAL gint32 krb_set_group_domain(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE objHandle,
    json_object *groupObject, gint32 groupId)
{
    gint32 ret;
    const gchar *groupDomain = NULL;
    GSList *inputList = NULL;
    gchar messageStr[ARRAY_LENTH] = {0};
    json_object *groupDomainObject = NULL;
    gboolean isFind;

    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != o_message_jso && NULL != groupObject,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1, "KerberosGroups/%d/%s", groupId,
        PROPERTY_LDAP_SERVICE_GROUP_DOMAIN);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    isFind = json_object_object_get_ex(groupObject, PROPERTY_LDAP_SERVICE_GROUP_DOMAIN, &groupDomainObject);
    return_val_do_info_if_fail(FALSE != isFind, VOS_OK,
        debug_log(DLOG_DEBUG, "%s: Not found group domain.", __FUNCTION__));
    groupDomain = dal_json_object_get_str(groupDomainObject);
    if (NULL == groupDomain) {
        debug_log(DLOG_DEBUG, "%s: group domain is NULL.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    inputList = g_slist_append(inputList, g_variant_new_string(groupDomain));

    
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_FOLDER, AUTH_ENABLE, i_paras->user_role_privilege, inputList,
        NULL);
    
    uip_free_gvariant_list(inputList);

    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, groupDomain);
}


LOCAL gint32 krb_set_group_role(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE objHandle,
    json_object *groupObject, gint32 groupId)
{
    gint32 ret;
    gint32 safe_fun_ret;
    gint32 loop_tag;
    guchar roleId = 0;
    const gchar *roleIdStr = NULL;
    GSList *input_list_p = NULL;
    GSList *input_list_u = NULL;
    gchar messageStr[ARRAY_LENTH] = {0};
    json_object *roleIdObject = NULL;

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != o_message_jso && NULL != groupObject,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (TRUE == json_object_object_get_ex(groupObject, PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE, &roleIdObject)) {
        roleIdStr = dal_json_object_get_str(roleIdObject);
    } else {
        return VOS_OK;
    }

    
    for (loop_tag = 0; loop_tag < LOGIN_RULE_MAX_ID; loop_tag++) {
        if (VOS_OK == g_strcmp0(priv_arr_krb[loop_tag].priv_str, roleIdStr)) {
            roleId = priv_arr_krb[loop_tag].priv_num;
            break;
        }
    }

    // 先设置privilage 将roleid传入并在ldap设置方法内转换为privilege
    input_list_p = g_slist_append(input_list_p, g_variant_new_byte(roleId));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_PRIVILEGE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list_p, NULL);
    debug_log(DLOG_INFO, "call LDAP_GROUP_METHOD_SET_GROUP_PRIVILEGE_NEW ret = %d", ret);
    uip_free_gvariant_list(input_list_p);

     // 设置完privilage才能设置用户权限位
    input_list_u = g_slist_append(input_list_u, g_variant_new_byte(roleId));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_USERROLEID, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list_u, NULL);
    uip_free_gvariant_list(input_list_u);

    safe_fun_ret = snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1, "KerberosGroups/%d/%s", groupId,
        PROPERTY_LDAP_SERVICE_GROUP_USER_ROLE);
    do_val_if_expr(safe_fun_ret <= 0,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, roleIdStr);
}

LOCAL gint32 krb_del_group_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guchar groupId,
    OBJ_HANDLE groupHandle)
{
    gint32 ret;
    json_object *tmpMessage = NULL;
    gchar messageStr[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &tmpMessage);
        (void)json_object_array_add(*o_message_jso, tmpMessage));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, groupHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_DEL_GROUP, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);
    return_val_if_expr(VOS_OK == ret, VOS_OK);

    (void)snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1, "KerberosGroups/%u", groupId);
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, NULL);
}

LOCAL gint32 krb_set_group_sid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE objHandle,
    json_object *groupObject, gint32 groupId)
{
    gint32 ret;
    int safe_fun_ret;
    json_object *SIDObject = NULL;
    const gchar *SIDStr = NULL;
    gchar messageStr[ARRAY_LENTH] = {0};
    GSList *inputList = NULL;
    json_object *tmpMessageObj = NULL;

    return_val_do_info_if_fail(VOS_OK == provider_paras_check(i_paras) && NULL != o_message_jso && NULL != groupObject,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (TRUE == json_object_object_get_ex(groupObject, RFPROP_KRB_SERVICE_GROUP_SID, &SIDObject)) {
        SIDStr = dal_json_object_get_str(SIDObject);
    } else {
        return VOS_ERR;
    }

    if (NULL == SIDStr) {
        ret = krb_del_group_info(i_paras, o_message_jso, groupId, objHandle);
        return ret;
    }

    
    return_val_do_info_if_fail(
        strlen(SIDStr) > 0, VOS_ERR,
        safe_fun_ret = snprintf_s(messageStr, ARRAY_LENTH, ARRAY_LENTH - 1,
                          "KerberosGroups/%d/%s", groupId,
                          RFPROP_KRB_SERVICE_GROUP_SID);
        do_val_if_expr(safe_fun_ret <= 0,
                       debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d", safe_fun_ret));
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, messageStr,
                                  &tmpMessageObj, SIDStr, messageStr);
        (void)json_object_array_add(*o_message_jso, tmpMessageObj);
        debug_log(DLOG_DEBUG, "%s: group SID is empty.", __FUNCTION__));
    

    
    inputList = g_slist_append(inputList, g_variant_new_string(SIDStr));
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, KRB_GROUP_METHOD_SET_GROUP_SID, AUTH_ENABLE, i_paras->user_role_privilege, inputList,
        NULL);
    uip_free_gvariant_list(inputList);

    safe_fun_ret = snprintf_s(messageStr, ARRAY_LENTH, ARRAY_LENTH - 1, "KerberosGroups/%d/%s",
        groupId, RFPROP_KRB_SERVICE_GROUP_SID);
    do_val_if_expr(safe_fun_ret <= 0, debug_log(DLOG_ERROR, "snprintf_s messageStr fail, ret = %d", safe_fun_ret));
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, SIDStr);
}

LOCAL gint32 krb_set_group_permit_role(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE objHandle,
    json_object *groupObject, gint32 groupId)
{
    gint32 ret;
    gint32 safe_fun_ret;
    gint32 loop_tag_i;
    guint32 loop_tag_j = 0;
    guchar permitRoleId = 0;
    GSList *input_list = NULL;
    json_object *roleObject = NULL;
    json_object *tmpMessage = NULL;
    json_object *roleNode = NULL;
    const gchar *roleNodeStr = NULL;
    const gchar* permit_role_map[] = {RFPROP_VAL_RULE1, RFPROP_VAL_RULE2, RFPROP_VAL_RULE3};
    gint32 int_arr[LOGRULE_3] = {0};
    gchar messageStr[ARRAY_LENTH] = {0};

    return_val_do_info_if_fail((provider_paras_check(i_paras) == VOS_OK) && (o_message_jso != NULL) &&
        (groupObject != NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (json_object_object_get_ex(groupObject, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE, &roleObject) != TRUE) {
        return VOS_OK;
    }

    gint32 roleCount = json_object_array_length(roleObject);
    if (roleCount > LOGRULE_3) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE, &tmpMessage,
            PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE);
        (void)json_object_array_add(*o_message_jso, tmpMessage);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag_i = 0; loop_tag_i < roleCount; loop_tag_i++) {
        roleNode = json_object_array_get_idx(roleObject, loop_tag_i);
        roleNodeStr = dal_json_object_get_str(roleNode);

        for (loop_tag_j = 0; loop_tag_j < LOGRULE_3; loop_tag_j++) {
            if (g_strcmp0(permit_role_map[loop_tag_j], roleNodeStr) == VOS_OK) {
                if (int_arr[loop_tag_j]) {
                    (void)snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1,
                        "KerberosGroups/%d/GroupLoginRule/%d", groupId, loop_tag_i);
                    
                    (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, (const gchar*)messageStr, 
                        &tmpMessage, (const gchar*)messageStr);
                    
                    (void)json_object_array_add(*o_message_jso, tmpMessage);
                    return HTTP_BAD_REQUEST;
                }

                permitRoleId += (1 << loop_tag_j);
                int_arr[loop_tag_j]++;
            }
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(permitRoleId));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_PERMIT_ID, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    safe_fun_ret = snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1, "KerberosGroups/%d/%s", groupId,
        PROPERTY_LDAP_SERVICE_GROUP_PERMIT_ROLE);
    do_val_if_expr(safe_fun_ret <= 0,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, NULL);
}

LOCAL gint32 krb_set_group_login_interface(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, OBJ_HANDLE objHandle,
    json_object *groupObject, gint32 groupId)
{
    gint32 ret;
    gint32 safe_fun_ret;
    gint32 loop_tag_i;
    guint32 loop_tag_j = 0;
    guint32 loginInterface = 0;
    GSList *input_list = NULL;
    json_object *interfaceObject = NULL;
    json_object *tmpMessage = NULL;
    json_object *interfaceNode = NULL;
    const gchar *interfaceNodeStr = NULL;
    const gchar* interfaceMap[] = {RFPROP_LOGIN_INTERFACE_WEB, RFPROP_LOGIN_INTERFACE_REDFISH};
    gint32 tmpArray[LOGRULE_3] = {0};
    const guint32 interfaceMapNum[] = {RFPROP_LOGIN_INTERFACE_WEB_NUM, USER_LOGIN_INTERFACE_REDFISH_OFFSET};
    gchar messageStr[ARRAY_LENTH] = {0};

    
    return_val_do_info_if_fail(provider_paras_check(i_paras) == VOS_OK && o_message_jso != NULL && groupObject != NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (json_object_object_get_ex(groupObject, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE, &interfaceObject) != TRUE) {
        return VOS_OK;
    }

    gint32 interfaceCount = json_object_array_length(interfaceObject);
    
    if (LOGIN_INTERFACE_MAX_COUNT < interfaceCount) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE,
            &tmpMessage, PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE);
        (void)json_object_array_add(*o_message_jso, tmpMessage);
        return HTTP_BAD_REQUEST;
    }

    for (loop_tag_i = 0; loop_tag_i < interfaceCount; loop_tag_i++) {
        interfaceNode = json_object_array_get_idx(interfaceObject, loop_tag_i);
        interfaceNodeStr = dal_json_object_get_str(interfaceNode);

        for (loop_tag_j = 0; loop_tag_j < sizeof(interfaceMapNum) / sizeof(guint32) && interfaceMap[loop_tag_j] != NULL;
            loop_tag_j++) {
            if (g_strcmp0(interfaceMap[loop_tag_j], interfaceNodeStr) == VOS_OK) {
                if (tmpArray[loop_tag_j]) {
                    (void)snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1,
                        "KerberosGroups/%d/GroupLoginInterface/%d", groupId, loop_tag_i);
                    
                    (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, (const gchar*)messageStr, 
                        &tmpMessage, (const gchar*)messageStr);
                    
                    (void)json_object_array_add(*o_message_jso, tmpMessage);
                    return HTTP_BAD_REQUEST;
                }

                loginInterface += (1 << interfaceMapNum[loop_tag_j]);
                tmpArray[loop_tag_j]++;
            }
        }
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(loginInterface));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_GROUP_CLASS_NAME, LDAP_GROUP_METHOD_SET_GROUP_LOGIN_INTERFACE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    safe_fun_ret = snprintf_s(messageStr, sizeof(messageStr), sizeof(messageStr) - 1, "KerberosGroups/%d/%s", groupId,
        PROPERTY_LDAP_SERVICE_GROUP_LOGIN_INTERFACE);
    do_val_if_expr(safe_fun_ret <= 0,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    return get_set_function_complex_ret(i_paras, ret, o_message_jso, messageStr, NULL);
}


LOCAL gint32 krb_set_controller_groups(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *tmpMessage = NULL;
    json_object *groupObject = NULL;
    GSList *groupList = NULL;
    GSList *groupNode = NULL;
    gint32 i;
    guchar serverId = 0;
    guchar groupId = 0;
    gboolean is_find = FALSE;
    OBJ_HANDLE groupNodeHandle = 0;
    gchar tmpStr[ARRAY_LENTH] = {0};
    gchar indexGroupStr[ARRAY_LENTH] = {0};
    guchar uriMemberId = 0;
    gboolean successFlag = FALSE;
    gint32 retCode = HTTP_BAD_REQUEST;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = vos_str2int(i_paras->member_id, 10, &uriMemberId, NUM_TPYE_UCHAR);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: vos_str2int error.", __FUNCTION__));

    
    *o_message_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: new array failed.", __FUNCTION__));

    gint32 groupCount = json_object_array_length(i_paras->val_jso);
    if (MAX_GROUP_COUNT < groupCount) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_KRB_SERVICE_KRBGROUPS, &tmpMessage,
            RFPROP_KRB_SERVICE_KRBGROUPS);
        (void)json_object_array_add(*o_message_jso, tmpMessage);
        return HTTP_BAD_REQUEST;
    }

    ret = dfl_get_object_list(LDAP_GROUP_CLASS_NAME, &groupList);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &tmpMessage);
        (void)json_object_array_add(*o_message_jso, tmpMessage);
        debug_log(DLOG_ERROR, "%s: Get group list failed.", __FUNCTION__));

    for (i = 0; i < groupCount; i++) {
        groupObject = json_object_array_get_idx(i_paras->val_jso, i);
        continue_if_expr(NULL == groupObject);

        for (groupNode = groupList; NULL != groupNode; groupNode = groupNode->next) {
            is_find = FALSE;
            ret =
                dal_get_property_value_byte((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_LDAPSERVER_ID, &serverId);
            continue_if_expr(VOS_OK != ret || LOG_TYPE_KRB_SERVER > (serverId & 0x0f));

            ret = dal_get_property_value_byte((OBJ_HANDLE)groupNode->data, LDAP_GROUP_ATTRIBUTE_GROUP_ID, &groupId);
            break_do_info_if_expr(VOS_OK == ret && i == groupId, (is_find = TRUE);
                (groupNodeHandle = (OBJ_HANDLE)groupNode->data));
        }

        if (!is_find) {
            (void)snprintf_s(tmpStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%u", uriMemberId);
            (void)snprintf_s(indexGroupStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%d", i);
            (void)create_message_info(MSGID_LDAP_MISMATCH_GROUP_AND_CONTOLLER, NULL, &tmpMessage, 
                (const gchar*)indexGroupStr, (const gchar*)tmpStr);
            (void)json_object_array_add(*o_message_jso, tmpMessage);
            g_slist_free(groupList);
            return HTTP_BAD_REQUEST;
        }
        
        ret = krb_set_group_role(i_paras, o_message_jso, groupNodeHandle, groupObject, i);
        get_operation_result(ret, &successFlag, &retCode);

        
        ret = krb_set_group_sid(i_paras, o_message_jso, groupNodeHandle, groupObject, i);
        continue_do_info_if_expr(VOS_OK == ret, (retCode = HTTP_OK)); // 仅删除成功时才会返回VOS_OK
        get_operation_result(ret, &successFlag, &retCode);

        
        ret = krb_set_group_permit_role(i_paras, o_message_jso, groupNodeHandle, groupObject, i);
        get_operation_result(ret, &successFlag, &retCode);

        
        ret = krb_set_group_login_interface(i_paras, o_message_jso, groupNodeHandle, groupObject, i);
        get_operation_result(ret, &successFlag, &retCode);

        
        ret = krb_set_group_name(i_paras, o_message_jso, groupNodeHandle, groupObject, i);
        get_operation_result(ret, &successFlag, &retCode);

        
        ret = krb_set_group_domain(i_paras, o_message_jso, groupNodeHandle, groupObject, i);
        get_operation_result(ret, &successFlag, &retCode);
    }

    g_slist_free(groupList);
    return retCode;
}


LOCAL gint32 krb_get_controller_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar *ctrlIdStr = NULL;
    gchar targerStr[ARRAY_LENTH] = {0};
    gchar actionInfoStr[ARRAY_LENTH] = {0};
    json_object *actionsJsonObj = NULL;

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ctrlIdStr = g_strrstr(i_paras->uri, "/");

    ret = snprintf_s(targerStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%s%s%s", RFPROP_KRB_ACTION_INFO_URI_B, ctrlIdStr + 1,
        RFPROP_KRB_ACTION_URI_A);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    ret = snprintf_s(actionInfoStr, ARRAY_LENTH, ARRAY_LENTH - 1, "%s%s%s", RFPROP_KRB_ACTION_INFO_URI_B, ctrlIdStr + 1,
        RFPROP_KRB_ACTION_INFO_URI_A);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

    actionsJsonObj = json_object_new_object();
    return_val_do_info_if_fail(NULL != actionsJsonObj, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: New object failed.", __FUNCTION__));

    json_object_object_add(actionsJsonObj, RFPROP_TARGET, json_object_new_string((const gchar*)targerStr));
    json_object_object_add(actionsJsonObj, RFPROP_ACTION_INFO, json_object_new_string((const gchar*)actionInfoStr));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(actionsJsonObj);
        debug_log(DLOG_ERROR, "%s: New result object failed.", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_KRB_IMPORT_METHOD, actionsJsonObj);
    return HTTP_OK;
}

LOCAL gint32 act_import_krb_keytab(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    const gchar *srcPathStr = NULL;
    OBJ_HANDLE objHandle = 0;
    guchar uriMemberId;
    GSList *inputList = NULL;
    gchar *urlTail = NULL;
    gchar urlHead[MAX_URL_LEN] = {0};

    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_USERMGNT, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    urlTail = strstr(i_paras->uri, "/Actions/");
    if (NULL == urlTail) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, (const gchar*)i_paras->uri);
        return HTTP_BAD_REQUEST;
    }

    ret = snprintf_s(urlHead, sizeof(urlHead), sizeof(urlHead) - 1, "%s", i_paras->uri);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));
    urlHead[strlen(i_paras->uri) - strlen(urlTail)] = '\0';
    ret = check_controller_id((const gchar *)urlHead);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    uriMemberId = (guchar)(urlHead[strlen(urlHead) - 1] - '0');

    ret = krb_get_controller_object_handle(uriMemberId, &objHandle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: cann't get object handle.", __FUNCTION__));

    (void)get_element_str(i_paras->val_jso, PROPERTY_ACCOUNT_SERVICE_ROOT_CONTENT, &srcPathStr);
    return_val_do_info_if_fail(NULL != srcPathStr, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: cann't get key table from request.", __FUNCTION__));

    
    if (strlen(srcPathStr) > MAX_FILEPATH_LENGTH - 1) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, NULL, o_message_jso, srcPathStr, RFACTION_PARAM_CONTENT);
        return HTTP_BAD_REQUEST;
    }

    
    ret = dal_check_real_path(srcPathStr);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: import path is invalid, return %d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_KRB_KEYTAB_FILE_UPLOAD_FAILED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    if (!dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, srcPathStr,
        (const gchar *)i_paras->user_roleid)) {
        (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    (void)chmod(srcPathStr, (S_IRUSR | S_IWUSR));

    inputList = g_slist_append(inputList, g_variant_new_string(srcPathStr));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, objHandle,
        LDAP_CLASS_NAME, KRB_METHOD_UPLOAD_KEYTABLE, AUTH_ENABLE, i_paras->user_role_privilege, inputList, NULL);
    uip_free_gvariant_list(inputList);

    
    switch (ret) {
        case VOS_OK:
            ret = HTTP_OK;
            (void)create_message_info(MSGID_KRB_KEYTAB_FILE_UPLOAD_SUCCESS, NULL, o_message_jso);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            ret = RFERR_INSUFFICIENT_PRIVILEGE;
            break;
        
        case RFPROP_KRB_KEYTABLE_UPLOAD_FAILED:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_KRB_KEYTAB_FILE_UPLOAD_FAILED, NULL, o_message_jso);
            break;

        case RFPROP_KRB_KEYTABLE_FORMAT_ERROR:
            ret = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_KRB_KEYTAB_FILE_UPLOAD_FAILED, NULL, o_message_jso);
            break;
        
        default:
            ret = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "unknown upload keytab file error:%d", ret);
            break;
    }

    return ret;
}

LOCAL gint32 act_import_krb_keytab_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, 
    json_object **o_result_jso)
{
    gint32 ret = act_import_krb_keytab(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


gint32 kerberos_controller_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret = 0;
    gchar *url_back = NULL;
    guchar uriMemberId = 0;

    if (!is_kerberos_support()) {
        return HTTP_NOT_FOUND;
    }
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    url_back = strstr(i_paras->uri, "/Actions/");
    
    if (NULL == url_back) {
        ret = check_controller_id(i_paras->uri);
        return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
        ret = vos_str2int(i_paras->member_id, 10, &uriMemberId, NUM_TPYE_UCHAR);
        return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);
        ret = krb_get_controller_object_handle(uriMemberId, &i_paras->obj_handle);
        return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);
    }

    *prop_provider = kerberos_controller_provider;
    *count = sizeof(kerberos_controller_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S kerberos_controller_provider_import[] = {
    {
        PROPERTY_LDAP_SERVICE_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        krb_get_acion_odata_id,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    }
};

LOCAL gint32 krb_get_acion_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return_val_do_info_if_fail(NULL != i_paras && NULL != o_message_jso && NULL != o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    *o_result_jso = json_object_new_string(i_paras->uri);

    if (NULL != *o_result_jso) {
        return HTTP_OK;
    }

    return HTTP_INTERNAL_SERVER_ERROR;
}

gint32 kerberos_controller_provider_import_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    gchar uriFullStr[ARRAY_LENTH] = {0};

    if (!is_kerberos_support()) {
        return HTTP_NOT_FOUND;
    }
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(uriFullStr, sizeof(uriFullStr), sizeof(uriFullStr) - 1, "%s", i_paras->uri);
    if (0 >= ret) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    uriFullStr[strlen(i_paras->uri) - strlen(RFPROP_KRB_ACTION_INFO_URI_A)] = '\0';

    ret = check_controller_id((const gchar *)uriFullStr);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = kerberos_controller_provider_import;
    *count = sizeof(kerberos_controller_provider_import) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
