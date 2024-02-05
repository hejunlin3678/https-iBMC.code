
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_drive_erase_collection_provider[] = {};
LOCAL PROPERTY_PROVIDER_S  g_manager_sp_drive_erase_provider[] = {};


gint32 get_sp_drive_erase_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso,
    json_object *o_err_array_jso, gchar **o_rsp_body_str)
{
    gint32 ret_val;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_collection_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_DRIVE_ERASES_METADATA,
        MANAGER_SPSERVICE_DRIVE_ERASES_URI);

    ret_val = get_sp_file_uri_members(i_paras, MANAGER_SPSERVICE_DRIVE_ERASES_URI, UMS_TRANS_FILE_ID_SP_DRIVE_ERASE,
        o_rsc_jso);
    if (VOS_OK != ret_val) {
        debug_log(DLOG_DEBUG, "%s: get drive_erase collection resource failed, ret_val = %d", __FUNCTION__, ret_val);
        return RF_FAILED;
    }

    return RF_OK;
}


gint32 get_sp_drive_erase_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret_val;
    json_object *message_info_jso = NULL;
    const gchar *msg_id = NULL;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_DRIVE_ERASE_METADATA,
        MANAGER_SPSERVICE_DRIVE_ERASE_URI, i_paras->member_id);

    ret_val =
        get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_DRIVE_ERASE, RFPROP_MANAGER_SP_DRIVE_ERASE_NAME, o_rsc_jso);
    if (UMS_WR_ERR_BUSY == ret_val) {
        msg_id = MSGID_UMS_IS_EXCLUSIVELY_USED;
    } else if (UMS_WR_ERR_IN_UPGRADING == ret_val) {
        msg_id = MSGID_SP_IS_BEING_UPGRADED;
    }
    if (msg_id != NULL) {
        (void)create_message_info(msg_id, NULL, &message_info_jso);
        (void)json_object_array_add(o_err_array_jso, message_info_jso);
        return RF_RSC_IN_USE;
    }

    do_if_fail(VOS_OK == ret_val,
        debug_log(DLOG_DEBUG, "%s: get drive_erase resource failed, ret_val = %d", __FUNCTION__, ret_val));

    return RF_OK;
}


gint32 manager_sp_drive_erase_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_if_expr(NULL == i_paras || NULL == prop_provider || NULL == count, HTTP_NOT_FOUND);

    if (TRUE != redfish_check_manager_uri_valid(i_paras->uri) || TRUE != rf_support_sp_service()) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_sp_drive_erase_collection_provider;
    *count = 0;

    return VOS_OK;
}


gint32 manager_sp_drive_erase_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_if_expr(NULL == i_paras || NULL == prop_provider || NULL == count, HTTP_NOT_FOUND);

    if (TRUE != redfish_check_manager_uri_valid(i_paras->uri) || TRUE != rf_support_sp_service()) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_sp_drive_erase_provider;
    *count = 0;

    return VOS_OK;
}


gint32 create_sp_drive_erase_conf(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso)
{
    gint32 ret = 0;
    json_object *message_jso = 0;

    return_val_do_info_if_expr((NULL == o_id) || (NULL == o_message_array_jso) || (NULL == i_param) ||
        (NULL == body_jso_checked),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__));

    if (TRUE != redfish_check_manager_uri_valid(i_param->uri) || TRUE != rf_support_sp_service()) {
        ret = HTTP_NOT_FOUND;
        goto EXIT;
    }

    if (0 == (i_param->user_role_privilege & USERROLE_BASICSETTING)) {
        ret = HTTP_FORBIDDEN;
        goto EXIT;
    }

    ret = create_sp_item_template(i_param, body_jso_checked, o_id, id_len, RFPROP_MANAGER_SP_DRIVE_ERASE_NAME,
        UMS_TRANS_FILE_ID_SP_DRIVE_ERASE);
EXIT:

    if (VOS_OK != ret) {
        make_sp_error_message(&ret, i_param->uri, &message_jso);
        (void)json_object_array_add(o_message_array_jso, message_jso);
        do_if_fail(HTTP_NOT_FOUND == ret || HTTP_FORBIDDEN == ret || HTTP_INTERNAL_SERVER_ERROR == ret,
            (ret = HTTP_BAD_REQUEST));
    }

    return ret;
}
