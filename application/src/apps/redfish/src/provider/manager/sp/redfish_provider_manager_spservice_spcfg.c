
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_cfg_provider[] = {
};


gint32 manager_sp_cfg_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    return_val_do_info_if_expr(0 != g_strcmp0(i_paras->member_id, "1"), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: url error", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_cfg_provider;
        
        *count = 0;
        ;
        ;

        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 get_sp_cfg_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    json_object *message_info_jso = NULL;
    const gchar *msg_id = NULL;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_CFG_METADATA, MANAGER_SPSERVICE_CFG_URI,
        i_paras->member_id);

    ret = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_CFG, RFPROP_MANAGER_SP_CFG_NAME, o_rsc_jso);
    
    if (ret == UMS_WR_ERR_BUSY) {
        msg_id = MSGID_UMS_IS_EXCLUSIVELY_USED;
    } else if (ret == UMS_WR_ERR_IN_UPGRADING) {
        msg_id = MSGID_SP_IS_BEING_UPGRADED;
    }

    if (msg_id != NULL) {
        (void)create_message_info(msg_id, NULL, &message_info_jso);
        (void)json_object_array_add(o_err_array_jso, message_info_jso);
        return RF_RSC_IN_USE;
    }
    

    do_val_if_expr(VOS_OK != ret, debug_log(DLOG_DEBUG, "%s, %d: no file.\n", __FUNCTION__, __LINE__));

    return RF_OK;
}
