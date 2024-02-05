

#include "redfish_provider.h"
LOCAL gint32 get_systems_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S collection_systems_provider[] = {
    {  RFPROP_MEMBERS, MAP_PROPERTY_NULL, RFPROP_ODATA_ID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_collection_members, NULL, NULL, ETAG_FLAG_ENABLE },
};



LOCAL gint32 get_systems_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gchar   pslot[PSLOT_MAX_LEN] = {0};
    gint32 ret;
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    json_object *obj_jso = NULL;

    
    
    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    
    ret = redfish_get_board_slot(pslot, sizeof(pslot));

    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    

    iRet = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_SYSTEM, pslot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    obj_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(obj_jso);
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(slot_uri));
    ret = json_object_array_add(*o_result_jso, obj_jso);

    
    
    do_val_if_expr(VOS_OK != ret, json_object_put(obj_jso); debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__));
    

    return HTTP_OK;
}


gint32 system_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    *prop_provider = collection_systems_provider;
    *count = sizeof(collection_systems_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
