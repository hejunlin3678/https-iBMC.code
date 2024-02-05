

#include "redfish_provider.h"
#include "redfish_provider_resource.h"

LOCAL gint32 get_krb_controller_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_krb_controller_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S kerberosservice_collection_provider[] = {
    {
        RFPROP_MEMBERS_COUNT,
        LDAP_CLASS_NAME, "\0",
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_NULL,
        get_krb_controller_collection_count,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MEMBERS,
        LDAP_CLASS_NAME, "\0",
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_NULL,
        get_krb_controller_collection_members,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    }
};

LOCAL gint32 get_krb_controller_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    OBJ_HANDLE iter_handle = 0;
    GSList *controller_list = NULL;
    GSList *controller_node = NULL;
    guint8 ldap_id = 0;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    ret = dfl_get_object_list(LDAP_CLASS_NAME, &controller_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: Get controller list failed.", __FUNCTION__, __LINE__));

    
    for (controller_node = controller_list; controller_node; controller_node = controller_node->next) {
        iter_handle = (OBJ_HANDLE)controller_node->data;

        
        ret = dal_get_property_value_byte(iter_handle, LDAP_ATTRIBUTE_ID, &ldap_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, get LDAP service type failed, ret=%d.\n", __FUNCTION__, ret));

        
        if (ldap_id >= LOG_TYPE_LDAP_SERVER6) {
            count++;
        }
    }

    g_slist_free(controller_list);

    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}

LOCAL gint32 get_krb_controller_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 ldap_id = 0;
    OBJ_HANDLE iter_handle = 0;
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
        debug_log(DLOG_ERROR, "%s failed:new json object array failed.", __FUNCTION__));

    
    for (controller_node = controller_list; controller_node; controller_node = controller_node->next) {
        iter_handle = (OBJ_HANDLE)controller_node->data;
        
        ret = dal_get_property_value_byte(iter_handle, LDAP_ATTRIBUTE_ID, &ldap_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, get LDAP service type failed, ret=%d.\n", __FUNCTION__, ret));

        if (ldap_id >= LOG_TYPE_LDAP_SERVER6) {
            
            controller_id = ldap_id - (LOG_TYPE_KRB_SERVER - 1);

            (void)memset_s(controller_uri, sizeof(controller_uri), 0, sizeof(controller_uri));
            ret = snprintf_s(controller_uri, sizeof(controller_uri), sizeof(controller_uri) - 1, "%s%u",
                PROPERTY_KRB_SERVICE_COLLECTION_MEMBER, controller_id);
            do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

            member_json = json_object_new_object();
            continue_if_fail(NULL != member_json);

            json_object_object_add(member_json, RFPROP_ODATA_ID, json_object_new_string(controller_uri));
            json_object_array_add(*o_result_jso, member_json);
        }
    }

    g_slist_free(controller_list);

    return HTTP_OK;
}

gint32 kerberosservice_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = kerberosservice_collection_provider;
    *count = sizeof(kerberosservice_collection_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
