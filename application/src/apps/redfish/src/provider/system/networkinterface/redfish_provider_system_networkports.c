
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_networkports_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_networkports_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_networkports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_networkports_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_networkports_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_networkports_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_networkports_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_networkports_members, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_networkports_odata_context, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_networkports_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret =
        snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SYSTEMS_NETWORK_INTERFACES_PORTS_METADATA, slot_id, string_value);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_networkports_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, SYSTEMS_NETWORK_INTERFACES_NETWORKPORTS, slot_id, string_value);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_networkports_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 0;
    gint32 ret_val;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret_val = get_networkports_members_count(i_paras->obj_handle, NULL, &count);
    return_val_if_expr(ret_val != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_int(count);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_networkports_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret_val;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n.", __FUNCTION__, __LINE__));

    ret_val = get_networkports_members_count(i_paras->obj_handle, o_result_jso, NULL);
    return_val_if_expr(ret_val != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


gint32 system_networkports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    gboolean bool_ret;
    gchar               url_tmp[MAX_URL_LEN] = {0};
    gchar *netwokrk_flag = NULL;
    guint32 str_length;

    
    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    do_if_expr(!bool_ret, return HTTP_NOT_FOUND);
    

    // URL 示例 : /redfish/v1/Systems/1/NetworkInterfaces/mainboardNIC1(TM210)/NetworkPorts
    // netwokrk_flag 指向 mainboardNIC1(TM210)
    ret = strcpy_s(url_tmp, sizeof(url_tmp), i_paras->uri);
    return_val_if_expr(ret != EOK, HTTP_NOT_FOUND);
    str_length = strlen(url_tmp);
    if (url_tmp[str_length - 1] == '/') {
        url_tmp[str_length - 1] = 0;
    }
    netwokrk_flag = g_strrstr(url_tmp, SLASH_FLAG_STR);
    return_val_if_expr(netwokrk_flag == NULL, HTTP_NOT_FOUND);
    *netwokrk_flag = 0;
    netwokrk_flag = g_strrstr(url_tmp, SLASH_FLAG_STR);
    return_val_if_expr(netwokrk_flag == NULL, HTTP_NOT_FOUND);
    ret = check_network_adapter_url(netwokrk_flag + 1, &(i_paras->obj_handle)); // NetCard 对象句柄
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_system_networkports_provider;
    *count = sizeof(g_system_networkports_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
