

#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_logservice_collection_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_collection_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_logservice_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 check_para_effective(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_logservice_collection_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_collection_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_collection_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_collection_count, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_logservice_collection_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 check_para_effective(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return VOS_OK;
}


LOCAL gint32 get_logservice_collection_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: function  return err of redfish_slot_id.", __FUNCTION__));

    
    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
        RFPROP_LOGSERVICE_COLLECTION_CONTEXT, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : function  return err of redfish_slot_id.", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_logservice_collection_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, RFPROP_LOGSERVICE_COLLECTION_ODATA_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    *o_result_jso = json_object_new_string(odata_id);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: function  return err of redfish_slot_id.", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_logservice_collection_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 1;
    gint32 ret;
    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    // 带内获取，返回post地址, 成员数量为 2
    ret = check_logservice_hostlog_service_support(i_paras->session_id);
    if (VOS_OK == ret) {
        count = 2;
    }

    
    *o_result_jso = json_object_new_int(count);

    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_logservice_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_NAME_LEN] = {0};
    json_object *odata_id_json = NULL;
    json_object *mem_id = NULL;

    
    ret = check_para_effective(i_paras, o_message_jso, o_result_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, RFPROP_LOGSERVICE_COLLECTION_MEMBER_ID, slot_id);
    return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail.", __FUNCTION__));

    odata_id_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != odata_id_json, HTTP_OK,
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

    mem_id = json_object_new_string(odata_id);
    return_val_do_info_if_fail(NULL != mem_id, HTTP_OK, json_object_put(odata_id_json);
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

    (void)json_object_object_add(odata_id_json, RFPROP_ODATA_ID, mem_id);

    ret = json_object_array_add(*o_result_jso, odata_id_json);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, json_object_put(odata_id_json);
        debug_log(DLOG_ERROR, "%s json_object_array_add fail.", __FUNCTION__));

    
    // 带内获取，返回post地址
    ret = check_logservice_hostlog_service_support(i_paras->session_id);
    if (VOS_OK == ret) {
        
        ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
            RFPROP_LOGSERVICE_COLLECTION_HOSTLOG_MEMBER_ID, slot_id);
        return_val_do_info_if_fail(ERROR_SNPRINTF < ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s:snprintf_s fail.", __FUNCTION__));

        odata_id_json = json_object_new_object();
        return_val_do_info_if_fail(NULL != odata_id_json, HTTP_OK,
            debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

        mem_id = json_object_new_string(odata_id);
        return_val_do_info_if_fail(NULL != mem_id, HTTP_OK, json_object_put(odata_id_json);
            debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));

        (void)json_object_object_add(odata_id_json, RFPROP_ODATA_ID, mem_id);

        ret = json_object_array_add(*o_result_jso, odata_id_json);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_OK, json_object_put(odata_id_json);
            debug_log(DLOG_ERROR, "%s json_object_array_add fail.", __FUNCTION__));
    }
    

    return HTTP_OK;
}


gint32 system_provider_logservice_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_if_fail(ret, HTTP_NOT_FOUND);

    *prop_provider = g_system_logservice_collection_provider;
    *count = sizeof(g_system_logservice_collection_provider) / sizeof(PROPERTY_PROVIDER_S);

    
    return VOS_OK;
    
}
