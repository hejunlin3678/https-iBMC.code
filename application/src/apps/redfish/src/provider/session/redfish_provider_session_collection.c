
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S *get_self_provider_session_collection(PROVIDER_PARAS_S *i_paras);


LOCAL gint32 get_session_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 idx, count, val_count = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    struct json_object *jso_org = NULL; 
    struct json_object *jso_dict = NULL;
    struct json_object *jso = NULL;
    const gchar *str = NULL;
    const gchar *session_checksum = NULL;
    PROPERTY_PROVIDER_S *self = get_self_provider_session_collection(i_paras);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id)); 
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SESSION, METHOD_SESSION_GETSESSIONLIST, 0, i_paras->is_satisfy_privi, input_list, &output_list);
    
    uip_free_gvariant_list(input_list);
    
    
    return_val_do_info_if_expr(RFERR_NO_RESOURCE == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri));
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    

    
    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    
    jso_org = json_tokener_parse(str);
    
    uip_free_gvariant_list(output_list);
    
    count = json_object_array_length(jso_org);

    for (idx = 0; idx < count; idx++) {
        
        jso_dict = json_object_array_get_idx(jso_org, idx);
        
        if (json_object_object_get_ex(jso_dict, RF_SESSION_CHECKSUM, &jso)) {
            session_checksum = dal_json_object_get_str(jso);

            
            continue_if_expr_true((!i_paras->is_satisfy_privi) &&
                (0 != g_strcmp0(i_paras->session_id, session_checksum)));

            
            val_count += 1;
        }
    }

    
    json_object_put(jso_org);
    
    
    
    
    *o_result_jso = json_object_new_int(val_count);
    return HTTP_OK;
}

LOCAL gint32 get_session_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar strbuf[REDFISH_SESSION_STR_LEN] = {0};
    guint32 idx, count;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    struct json_object *jso_org = NULL; 
    struct json_object *jso_dict = NULL;
    struct json_object *jso = NULL;
    struct json_object *jso_new = NULL;
    const gchar *session_checksum = NULL;
    const gchar *str = NULL;
    PROPERTY_PROVIDER_S *self = get_self_provider_session_collection(i_paras);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != self),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->session_id)); 
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SESSION, METHOD_SESSION_GETSESSIONLIST, 0, i_paras->user_role_privilege, input_list, &output_list);
    
    uip_free_gvariant_list(input_list);

    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            break;

        case RFERR_NO_RESOURCE: 
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
            return HTTP_NOT_FOUND;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR; // return ret?
    }

    
    str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    
    jso_org = json_tokener_parse(str);
    
    uip_free_gvariant_list(output_list);
    
    count = json_object_array_length(jso_org);

    
    *o_result_jso = json_object_new_array();

    for (idx = 0; idx < count; idx++) {
        
        jso_dict = json_object_array_get_idx(jso_org, idx);
        
        if (json_object_object_get_ex(jso_dict, RF_SESSION_CHECKSUM, &jso)) {
            session_checksum = dal_json_object_get_str(jso);

            
            continue_if_expr_true((!i_paras->is_satisfy_privi) &&
                (0 != g_strcmp0(i_paras->session_id, session_checksum)));
            
            
            jso_new = json_object_new_object();

            
            
            

            iRet = snprintf_s(strbuf, sizeof(strbuf), sizeof(strbuf) - 1, "%s/%s", SESSIONSERVICE_SESSIONS,
                session_checksum);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

            
            
            json_object_object_add(jso_new, RFPROP_ODATA_ID, json_object_new_string(strbuf));

            
            json_object_array_add(*o_result_jso, jso_new);
        }
    }

    
    json_object_put(jso_org);
    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S session_collection_provider[] = {
    {
        RFPROP_MEMBERS_COUNT,
        CLASS_SESSION, "\0",
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_NULL,
        get_session_collection_count,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_MEMBERS,
        CLASS_SESSION, "\0",
        USERROLE_USERMGNT,
        SYS_LOCKDOWN_NULL,
        get_session_collection_members,
        NULL,
        NULL, ETAG_FLAG_ENABLE
    },
};

LOCAL PROPERTY_PROVIDER_S *get_self_provider_session_collection(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(session_collection_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &session_collection_provider[i_paras->index];
}


gint32 session_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    return_val_if_fail(NULL != i_paras && NULL != prop_provider && NULL != count, HTTP_INTERNAL_SERVER_ERROR);

    *prop_provider = session_collection_provider;
    *count = sizeof(session_collection_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
