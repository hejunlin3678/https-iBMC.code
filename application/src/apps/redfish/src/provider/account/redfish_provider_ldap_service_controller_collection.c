

#include "redfish_provider.h"

LOCAL gint32 get_controller_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_controller_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S ldapservice_collection_provider[] = {
    {
        RFPROP_MEMBERS_COUNT,
        LDAP_CLASS_NAME, "\0",
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_NULL,
        get_controller_collection_count,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MEMBERS,
        LDAP_CLASS_NAME, "\0",
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_NULL,
        get_controller_collection_members,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
};

LOCAL gint32 get_controller_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    GSList *controller_list = NULL;
    GSList *controller_node = NULL;
    guint8 ldap_id = 0;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    ret = dfl_get_object_list(LDAP_CLASS_NAME, &controller_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: Get controller list failed.", __FUNCTION__, __LINE__));
    
    
    for (controller_node = controller_list; controller_node; controller_node = controller_node->next) {
        
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)controller_node->data, LDAP_ATTRIBUTE_ID, &ldap_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, get LDAP.ID failed, ret=%d.\n", __FUNCTION__, ret));

        if (ldap_id < LOG_TYPE_LDAP_SERVER6) {
            count++;
        }
        
    }

    g_slist_free(controller_list);

    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}

LOCAL gint32 get_controller_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar controller_id = 0;
    json_object *member_json = NULL;
    gchar controller_uri[ARRAY_LENTH] = {0};
    GSList *controller_list = NULL;
    GSList *controller_node = NULL;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    ret = dfl_get_object_list(LDAP_CLASS_NAME, &controller_list);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(controller_list);
        debug_log(DLOG_DEBUG, "%s, %d: Get controller list failed.", __FUNCTION__, __LINE__));
    
    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(controller_list);
        controller_list = NULL; debug_log(DLOG_ERROR, "%s failed:new json object array failed.", __FUNCTION__));

    
    for (controller_node = controller_list; controller_node; controller_node = controller_node->next) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)controller_node->data, LDAP_ATTRIBUTE_ID, &controller_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, get LDAP.ID failed, ret=%d.\n", __FUNCTION__, ret));

        if (controller_id < LOG_TYPE_LDAP_SERVER6) {
            (void)memset_s(controller_uri, sizeof(controller_uri), 0, sizeof(controller_uri));
            // sizeof(controller_uri)大小为128，大于格式化后字符串长度，无需判断返回值
            (void)snprintf_s(controller_uri, sizeof(controller_uri), sizeof(controller_uri) - 1, "%s%u",
                PROPERTY_LDAP_SERVICE_COLLECTION_MEMBER, controller_id);
            member_json = json_object_new_object();
            if (NULL == member_json) {
                continue;
            }

            json_object_object_add(member_json, RFPROP_ODATA_ID, json_object_new_string(controller_uri));
            json_object_array_add(*o_result_jso, member_json);
        }
        
    }

    g_slist_free(controller_list);

    return HTTP_OK;
}

gint32 ldapservice_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = ldapservice_collection_provider;
    *count = sizeof(ldapservice_collection_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
