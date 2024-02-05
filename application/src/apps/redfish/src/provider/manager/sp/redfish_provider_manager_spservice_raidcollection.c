
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_raids_provider[] = {
};


gint32 manager_sp_raids_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_raids_provider;
        
        *count = 0;
        ;
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 get_sp_raids_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_collection_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_RAIDS_METADATA, MANAGER_SPSERVICE_RAIDS_URI);

    (void)get_sp_file_uri_members(i_paras, MANAGER_SPSERVICE_RAIDS_URI, UMS_TRANS_FILE_ID_SP_RAID, o_rsc_jso);

    return RF_OK;
}


gint32 get_sp_collection_odata_context_id(json_object *o_rsc_jso, gchar *metadata, gchar *odataid)
{
    gint32 ret;
    json_object *o_jso = NULL;
    gchar uri_id[MAX_URI_LENGTH] = {0};
    gchar uri_context[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(uri_context, sizeof(uri_context), sizeof(uri_context) - 1, metadata, slot);
#pragma GCC diagnostic pop
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    o_jso = json_object_new_string((const gchar *)uri_context);
    return_val_do_info_if_expr(NULL == o_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_CONTEXT, o_jso);
    o_jso = NULL;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret = snprintf_s(uri_id, sizeof(uri_id), sizeof(uri_id) - 1, odataid, slot);
#pragma GCC diagnostic pop
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    o_jso = json_object_new_string((const gchar *)uri_id);
    return_val_do_info_if_expr(NULL == o_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, o_jso);

    return VOS_OK;
}
