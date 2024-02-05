
#include "redfish_provider.h"
















LOCAL PROPERTY_PROVIDER_S  g_manager_sp_raid_currents_provider[] = {
};


gint32 manager_sp_raid_currents_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_raid_currents_provider;
        *count = 0;
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 get_sp_raid_currents_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_collection_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_RAID_CURRENTS_METADATA,
        MANAGER_SPSERVICE_RAID_CURRENTS_URI);

    (void)get_sp_file_uri_members(i_paras, MANAGER_SPSERVICE_RAID_CURRENTS_URI, UMS_TRANS_FILE_ID_SP_RAID_EXPORT,
        o_rsc_jso);

    return RF_OK;
}
