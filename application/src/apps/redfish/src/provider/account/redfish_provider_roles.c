

#include "redfish_provider.h"

LOCAL gint32 get_roles_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_roles_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_roles_provider[] = {
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_roles_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, get_roles_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_roles_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guint8 count = 0;
    guchar roleid = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_USERROLE, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_USERROLE_ID, &roleid);
        if ((VOS_OK == ret) && (0 == i_paras->is_satisfy_privi)) {
            ret = redfish_role_comparison(i_paras->user_roleid, roleid);
            do_if_expr(VOS_OK == ret, count++);
        } else if ((VOS_OK == ret) && (0 != i_paras->is_satisfy_privi)) {
            count++;
        }

        roleid = 0;
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 get_roles_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gint32 match_flag = 1;
    guint8 roleid = 0;
    gchar rolename[USER_ROLEID_MAX_LEN + 1] = {0};
    gchar role_uri[ACCOUNTS_URI_LEN + USER_ROLEID_MAX_LEN + 2] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_list(CLASS_USERROLE, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_array();

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        if (0 == i_paras->is_satisfy_privi) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_USERROLE_ID, &roleid);
            do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d: get roleid fail.\n", __FUNCTION__, __LINE__));
            ret = redfish_role_comparison(i_paras->user_roleid, roleid);
            do_if_expr(VOS_OK != ret, continue);
            match_flag = VOS_OK;
        }

        
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_USERROLE_ROLENAME, rolename,
            USER_ROLEID_MAX_LEN + 1);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s, %d: get rolename fail.\n", __FUNCTION__, __LINE__));

        
        iRet = snprintf_s(role_uri, sizeof(role_uri), sizeof(role_uri) - 1, "%s/%s", ACCOUNTSERVICE_ROLES, rolename);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        

        obj_jso = json_object_new_object();
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(role_uri));
        json_object_array_add((*o_result_jso), obj_jso);
        obj_jso = NULL;

        (void)memset_s(rolename, sizeof(rolename), 0, sizeof(rolename));
    }

    g_slist_free(obj_list);
    return_val_do_info_if_fail((0 != i_paras->is_satisfy_privi) || (VOS_OK == match_flag), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: none of roleid matching.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


gint32 roles_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    *prop_provider = g_roles_provider;
    *count = sizeof(g_roles_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
