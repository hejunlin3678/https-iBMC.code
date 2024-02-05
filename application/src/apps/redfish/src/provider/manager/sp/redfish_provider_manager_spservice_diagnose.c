
#include "redfish_provider.h"

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_diagnose_collection_provider[] = {};
LOCAL PROPERTY_PROVIDER_S  g_manager_sp_diagnose_provider[] = {};


void make_sp_error_message(gint32 *ret_val, gchar *url, json_object **message_jso)
{
    if (ret_val == NULL || url == NULL || message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return;
    }

    switch (*ret_val) {
        case HTTP_NOT_FOUND:
            (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, message_jso, url);
            break;

        case HTTP_FORBIDDEN:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, message_jso);
            break;

        case UMS_TRANS_ERR_FILE_TOO_LARGE:
        case UMS_TRANS_ERR_NO_SPACE:
            (void)create_message_info(MSGID_SP_CFG_OVERSIZED, NULL, message_jso);
            break;

        case UMS_TRANS_ERR_FILE_CNT_LIMITED:
            (void)create_message_info(MSGID_SP_CFG_LIMIT, NULL, message_jso);
            break;

        case UMS_WR_ERR_BUSY:
            (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, message_jso);
            break;

        case UMS_WR_ERR_IN_UPGRADING:
            (void)create_message_info(MSGID_SP_IS_BEING_UPGRADED, NULL, message_jso);
            break;

        case UMS_WR_ERR_CAN_NOT_GET_LOCK:
            *ret_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_RSC_IN_USE, NULL, message_jso);
            break;

        default:
            *ret_val = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            break;
    }

    return;
}


gint32 get_sp_diagnose_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret_val;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_collection_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_DIAGNOSES_METADATA,
        MANAGER_SPSERVICE_DIAGNOSES_URI);

    ret_val =
        get_sp_file_uri_members(i_paras, MANAGER_SPSERVICE_DIAGNOSES_URI, UMS_TRANS_FILE_ID_SP_DIAGNOSE, o_rsc_jso);
    if (VOS_OK != ret_val) {
        debug_log(DLOG_DEBUG, "%s: get diagnose collection resource failed, ret_val = %d", __FUNCTION__, ret_val);
        return RF_FAILED;
    }

    return RF_OK;
}


gint32 get_sp_diagnose_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret_val;
    json_object *message_info_jso = NULL;
    const gchar *msg_id = NULL;

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_DIAGNOSE_METADATA,
        MANAGER_SPSERVICE_DIAGNOSE_URI, i_paras->member_id);

    ret_val = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_DIAGNOSE, RFPROP_MANAGER_SP_DIAGNOSE_NAME, o_rsc_jso);
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
        debug_log(DLOG_DEBUG, "%s: get diagnose resource failed, ret_val = %d", __FUNCTION__, ret_val));

    return RF_OK;
}


gint32 manager_sp_diagnose_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    if (TRUE != redfish_check_manager_uri_valid(i_paras->uri) || TRUE != rf_support_sp_service()) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_sp_diagnose_collection_provider;
    *count = 0;

    return VOS_OK;
}


gint32 manager_sp_diagnose_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    if (TRUE != redfish_check_manager_uri_valid(i_paras->uri) || TRUE != rf_support_sp_service()) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_sp_diagnose_provider;
    *count = 0;

    return VOS_OK;
}


gint32 create_sp_diagnose_conf(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso)
{
    gint32 ret_val = 0;
    json_object *message_jso = 0;

    return_val_do_info_if_expr((NULL == o_id) || (NULL == o_message_array_jso) || (NULL == i_param) ||
        (NULL == body_jso_checked),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__));

    if (TRUE != redfish_check_manager_uri_valid(i_param->uri) || TRUE != rf_support_sp_service()) {
        ret_val = HTTP_NOT_FOUND;
        goto EXIT;
    }

    if (0 == (i_param->user_role_privilege & USERROLE_BASICSETTING)) {
        ret_val = HTTP_FORBIDDEN;
        goto EXIT;
    }

    ret_val = create_sp_item_template(i_param, body_jso_checked, o_id, id_len, RFPROP_MANAGER_SP_DIAGNOSE_NAME,
        UMS_TRANS_FILE_ID_SP_DIAGNOSE);
EXIT:

    if (VOS_OK != ret_val) {
        make_sp_error_message(&ret_val, i_param->uri, &message_jso);
        (void)json_object_array_add(o_message_array_jso, message_jso);
        do_if_fail(HTTP_NOT_FOUND == ret_val || HTTP_FORBIDDEN == ret_val || HTTP_INTERNAL_SERVER_ERROR == ret_val,
            (ret_val = HTTP_BAD_REQUEST));
    }

    return ret_val;
}
