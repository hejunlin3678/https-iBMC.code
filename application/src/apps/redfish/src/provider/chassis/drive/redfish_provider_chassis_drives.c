

#include "redfish_provider.h"


LOCAL gint32 get_drive_collection_odata_id(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                         json_object** o_result_jso)
{
    gint32 ret;

    ret = get_odata_id(i_paras, o_message_jso, o_result_jso, URI_FORMAT_CHASSIS_DRIVE_COLLECTION);
    
    return ret;
}


LOCAL gint32 get_drive_collection_members_count(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                                 json_object** o_result_jso)
{
    size_t count;
    json_object* jso_drives = NULL;

    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    jso_drives = json_object_new_array();
    return_val_do_info_if_expr(jso_drives == NULL, HTTP_INTERNAL_SERVER_ERROR, 
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    get_chassis_related_drives_member(jso_drives);
    count = json_object_array_length(jso_drives);
    (void)json_object_put(jso_drives);

    *o_result_jso = json_object_new_int(count);
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_drive_collection_members(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
                                                        json_object** o_result_jso)
{
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, 
        debug_log(DLOG_ERROR, "%s: alloc new json array failed", __FUNCTION__));

    get_chassis_related_drives_member(*o_result_jso);
    
    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_chassis_drive_collection_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_drive_collection_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_drive_collection_members_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_drive_collection_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 chassis_drives_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count)
{
    gint32 ret;
    guchar board_type = 0;
    gboolean ret_result;

    return_val_do_info_if_expr(i_paras == NULL || prop_provider == NULL || count == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret_result = redfish_check_chassis_uri_valid(i_paras->uri, &i_paras->obj_handle);
    if (ret_result == FALSE) {
        return HTTP_NOT_FOUND;
    }

    ret = check_enclosure_component_type(i_paras->obj_handle, FALSE);
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_expr(ret != VOS_OK, HTTP_NOT_FOUND);
    return_val_if_expr(board_type == DISABLE, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_drive_collection_provider;
    *count = G_N_ELEMENTS(g_chassis_drive_collection_provider);
    return VOS_OK;
}

