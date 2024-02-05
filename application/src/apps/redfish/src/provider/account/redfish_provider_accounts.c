

#include "redfish_provider.h"

LOCAL gint32 get_accounts_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_accounts_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_accounts_provider[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_accounts_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_accounts_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_accounts_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_USER, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(username, sizeof(username), 0, sizeof(username));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_USER_NAME, username, sizeof(username));
        do_if_expr(0 == strlen(username), continue);

        if (0 == i_paras->is_satisfy_privi) {
            ret = g_strcmp0(username, i_paras->user_name);
            do_if_expr(VOS_OK != ret, continue);
        }

        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 get_accounts_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    guint8 userid = 0;
    gchar username[USER_USERNAME_MAX_LEN + 1] = {0};
    gchar user_uri[ACCOUNTS_URI_LEN + USERID_MAX_LEN + 2] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_USER, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_array();
    
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array error", __FUNCTION__);
        g_slist_free(obj_list));
    

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(username, sizeof(username), 0, sizeof(username));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_USER_NAME, username, sizeof(username));

        do_if_expr(0 == strlen(username), continue);

        if (0 == i_paras->is_satisfy_privi) {
            ret = g_strcmp0(username, i_paras->user_name);
            do_if_expr(VOS_OK != ret, continue);
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_USER_ID, &userid);

        
        iRet = snprintf_s(user_uri, sizeof(user_uri), sizeof(user_uri) - 1, "%s/%u", ACCOUNTSERVICE_ACCOUNTS, userid);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        

        
        (void)rf_validate_rsc_exist(user_uri, &obj_jso);
        do_if_expr(NULL == obj_jso, continue);
        

        json_object_array_add(*o_result_jso, obj_jso);
        obj_jso = NULL;
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}


gint32 accounts_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    *prop_provider = g_accounts_provider;
    *count = sizeof(g_accounts_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
