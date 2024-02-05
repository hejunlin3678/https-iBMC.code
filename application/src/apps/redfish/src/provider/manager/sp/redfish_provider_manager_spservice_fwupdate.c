
#include "redfish_provider.h"

LOCAL gint32 _get_spservice_up_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_spservice_up_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 _get_sp_up_ts_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_sp_up_ts_filename(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_sp_up_ts_percent(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_sp_up_upgrade_progress(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_sp_up_filelist(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _get_sp_up_ts_messages(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 _get_spservice_up_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _act_spservice_up_update(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_update_firmware_sp_result(gint32 ret, json_object **o_message_jso);

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_update_provider[] = {
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_up_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_up_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_SPSERVICE_UP_TSSTATE, SP_UMS_CLASS_NAME, UMS_PROPERTY_TRANSFER_PROGRESS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_sp_up_ts_state, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_SPSERVICE_UP_TSFILENAME, SP_UMS_CLASS_NAME, UMS_PROPERTY_TRANSFER_FILE_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_sp_up_ts_filename, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_SPSERVICE_UP_TSPROGRESSPERCENT, SP_UMS_CLASS_NAME, UMS_PROPERTY_TRANSFER_PROGRESS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_sp_up_ts_percent, NULL, NULL, ETAG_FLAG_DISABLE},
    {RFPROP_SPSERVICE_UP_UPGRADEPROGRESS, SP_UMS_CLASS_NAME, UMS_PROPERTY_SP_UPGRADE_PROGRESS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_sp_up_upgrade_progress, NULL, NULL, ETAG_FLAG_DISABLE},
    
    {RFPROP_SPSERVICE_UP_MESSAGES, SP_UMS_CLASS_NAME, UMS_PROPERTY_TRANSFER_PROGRESS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_sp_up_ts_messages, NULL, NULL, ETAG_FLAG_DISABLE},
    
    {RFPROP_SPSERVICE_UP_TSFILELIST, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_sp_up_filelist, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, _get_spservice_up_action, NULL, NULL, ETAG_FLAG_ENABLE},

    {RFPROP_MANAGER_SP_UP_POST_ACTION_NAME, CLASS_SMTP_CONFIG, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, _act_spservice_up_update, ETAG_FLAG_ENABLE}
};

typedef struct spservice_up_update_info {
    const gchar *image_uri;
    const gchar *signature_uri;
    const gchar *image_type;
    const gchar *param;
    const gchar *upgrade_mode;
    const gchar *active_method;
} SPSERVICE_UP_ARGS;

LOCAL gint32 _get_spservice_up_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGERS_SPSERVICE_UPDATE_METADATA, slot);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_up_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, MANAGER_SPSERVICE_UPDATE_URI, slot, i_paras->member_id);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_sp_up_ts_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 value = 0;
    
    return_val_do_info_if_expr(NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_sp_update_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, g_manager_sp_update_provider[i_paras->index].pme_prop_name, &value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    if (value == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else if (value == 100) {
        *o_result_jso = json_object_new_string(RFPROP_SPSERVICE_UP_TSSTATE_SUCCESS);
    } else if (value < 100 && value > 0) {
        *o_result_jso = json_object_new_string(RFPROP_SPSERVICE_UP_TSSTATE_PROCESSING);
    } else {
        *o_result_jso = json_object_new_string(RFPROP_SPSERVICE_UP_TSSTATE_FAILURE);
    }

    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  is NULL\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 _get_sp_up_ts_filename(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar str_value[MAX_URL_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_sp_update_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_string(obj_handle, g_manager_sp_update_provider[i_paras->index].pme_prop_name,
        str_value, MAX_URL_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    return_val_if_expr(VOS_OK != check_string_val_effective((const gchar *)str_value), HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)str_value);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  is NULL", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 _get_sp_up_ts_percent(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 value = 0;
    gint32 ret;
    
    return_val_do_info_if_expr(NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_sp_update_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, g_manager_sp_update_provider[i_paras->index].pme_prop_name, &value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    return_val_if_expr(
        (value > 100 || value == UPGRADE_SP_ERROR || value == FIND_CLASS_ERROR || value == MOUNT_UMS_UNAVAILABLE),
        HTTP_INTERNAL_SERVER_ERROR); // -116，-117，-118为内部错误码，对外返回500，0-100是文件传输进度
    *o_result_jso = json_object_new_int(value);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  is NULL", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 _get_sp_up_ts_messages(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 value = 0;
    json_object *res_message = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_sp_update_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, g_manager_sp_update_provider[i_paras->index].pme_prop_name, &value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    ret = parse_file_transfer_err_code(value, &res_message);
    if (RF_OK != ret && res_message != NULL) {
        json_object_array_add(*o_result_jso, res_message);
        // 如果传输错误则不需要继续显示升级的信息
        return HTTP_OK;
    } else if (100 == value) {
        (void)create_message_info(MSGID_SP_TSFILE_OK, NULL, &res_message);
        (void)json_object_array_add(*o_result_jso, res_message);
    } else if (0 < value && value < 100) {
        (void)create_message_info(MSGID_SP_TSFILE_RUN, NULL, &res_message);
        (void)json_object_array_add(*o_result_jso, res_message);
    } else if (0 > value) {
        (void)create_message_info(MSGID_FILE_TRANSFER_ERR_DESC, NULL, &res_message, UNKNOWN_ERROR_STR);
        (void)json_object_array_add(*o_result_jso, res_message);
    }

    res_message = NULL;
    ret = dal_get_property_value_int32(obj_handle, UMS_PROPERTY_SP_UPGRADE_PROGRESS, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get upgrade progress failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (value == 100) { // 进度100表示升级已完成
        (void)create_message_info(MSGID_SP_UPGRADE_OK, NULL, &res_message);
        (void)json_object_array_add(*o_result_jso, res_message);
    } else if (value > 0 && value < 100) { // value介于0到100之间，表示升级中。若value值为0，不返回任何消息
        (void)create_message_info(MSGID_SP_UPGRADE_RUN, NULL, &res_message);
        (void)json_object_array_add(*o_result_jso, res_message);
    }

    return HTTP_OK;
}


LOCAL gint32 _get_sp_up_upgrade_progress(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 value = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_expr(VOS_OK != provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: paras check error.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_object_handle_nth(g_manager_sp_update_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle fail", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, g_manager_sp_update_provider[i_paras->index].pme_prop_name, &value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_int(value);
    return_val_do_info_if_fail(*o_result_jso != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get prop fail\n", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 _get_sp_up_filelist(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *file_data = NULL;
    gchar **split_array = NULL;

    gint32 length = 0;
    gint32 index_id = 0;

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d: new array is null", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TRANS_FILE_ID_SP_FWUPDATE));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_GET_FILE_LIST, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);

    
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_OK, uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        debug_log(DLOG_DEBUG, "%s, %d:  get file list fail, ret is %d.", __FUNCTION__, __LINE__, ret));
    
    uip_free_gvariant_list(input_list);

    file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_expr(NULL == file_data, HTTP_OK, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d:  file_data is null", __FUNCTION__, __LINE__));

    split_array = g_strsplit(file_data, SP_FILE_SPLITE_FLAG, 0);
    if (NULL != split_array) {
        length = g_strv_length(split_array);

        for (index_id = 0; index_id < length; index_id++) {
            continue_if_expr(0 == strlen(split_array[index_id]));
            json_object_array_add(*o_result_jso, json_object_new_string(split_array[index_id]));
        }

        g_strfreev(split_array);
    }

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}


LOCAL gint32 _get_spservice_up_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[SNMP_STRLEN_32] = {0};
    gchar uri[SNMP_STRLEN_256] = {0};
    json_object *tmp_json = NULL;
    json_object *jso = NULL;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    // 获取target的信息
    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SP_UP_ACTION_TARGETURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
    }

    tmp_json = json_object_new_string((const gchar *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, RFPROP_TARGET, tmp_json);
    tmp_json = NULL;

    // 获取actioninfo的信息
    (void)memset_s(uri, sizeof(uri), 0x00, sizeof(uri));

    if (snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SP_UP_ACTION_INFOURI, slot) <= 0) {
        debug_log(DLOG_ERROR, "%s, %d: snprintf_s fail\n", __FUNCTION__, __LINE__);
    }

    tmp_json = json_object_new_string((const gchar *)uri);
    return_val_do_info_if_fail(NULL != tmp_json, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail\n", __FUNCTION__, __LINE__));
    json_object_object_add(jso, RFPROP_MANAGER_SP_UP_ACTION_INFONAME, tmp_json);

    // 生成action信息
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(jso);
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail\n", __FUNCTION__, __LINE__));

    json_object_object_add(*o_result_jso, RFPROP_MANAGER_SP_UP_ACTION_NAME, jso);

    return HTTP_OK;
}


LOCAL gint32 __check_spservice_update_uri(SPSERVICE_UP_ARGS *up_args, json_object **o_message_jso)
{
    gint32 ret;
    const gchar* str_im_uri_file = NULL;

    str_im_uri_file = g_strrstr(up_args->image_uri, "/");
    if (str_im_uri_file == NULL || strlen(str_im_uri_file + 1) > 200) { // 文件名不能超过200字符
        debug_log(DLOG_ERROR, "%s: %s filename is over 200 characters.", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_IM_URI);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SP_ACTION_IM_URI, o_message_jso,
                                  RF_SENSITIVE_INFO, RFPROP_MANAGER_SP_ACTION_IM_URI);
        return HTTP_BAD_REQUEST;
    }

    
    if (g_strcmp0(up_args->image_type, RFPROP_MANAGER_SP_IM_TYPE_FIRMWARE) == 0) {
        str_im_uri_file = g_strrstr(up_args->signature_uri, "/");
        if (str_im_uri_file == NULL || strlen(str_im_uri_file + 1) > 200) { // 文件名不能超过200字符
            debug_log(DLOG_ERROR, "%s: %s filename is over 200 characters.", __FUNCTION__,
                      RFPROP_MANAGER_SP_ACTION_SI_URI);
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SP_ACTION_SI_URI, o_message_jso,
                                      RF_SENSITIVE_INFO, RFPROP_MANAGER_SP_ACTION_SI_URI);
            return HTTP_BAD_REQUEST;
        }
        ret = g_regex_match_simple(SP_ACTION_IM_URI_REGEX, up_args->image_uri, G_REGEX_OPTIMIZE,
                                   (GRegexMatchFlags)0);
        if (ret != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SP_ACTION_IM_URI, o_message_jso,
                RF_SENSITIVE_INFO, RFPROP_MANAGER_SP_ACTION_IM_URI);
            return HTTP_BAD_REQUEST;
        }
        ret = g_regex_match_simple(SP_ACTION_SI_URI_REGEX, up_args->signature_uri, G_REGEX_OPTIMIZE, 
                                   (GRegexMatchFlags)0);
        if (ret != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SP_ACTION_SI_URI, o_message_jso,
                RF_SENSITIVE_INFO, RFPROP_MANAGER_SP_ACTION_SI_URI);
            return HTTP_BAD_REQUEST;
        }
    } else {
        ret = g_regex_match_simple("^(nfs|cifs)://.{1,1000}", up_args->image_uri, G_REGEX_OPTIMIZE,
                                   (GRegexMatchFlags)0);
        if (ret != TRUE) {
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_SP_ACTION_IM_URI, o_message_jso,
                RF_SENSITIVE_INFO, RFPROP_MANAGER_SP_ACTION_IM_URI);
            return HTTP_BAD_REQUEST;
        }
    }
    
    return RET_OK;
}


LOCAL gint32 __get_spservice_up_resource(json_object *i_val_jso, SPSERVICE_UP_ARGS *up_args,
    json_object **o_message_jso)
{
    if (i_val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: wrong value type,  %s", __FUNCTION__, SNMP_TRAPITEM_ID);
        return HTTP_BAD_REQUEST;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SP_ACTION_IM_URI, &(up_args->image_uri)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_IM_URI);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SP_ACTION_IM_URI, o_message_jso,
            RFPROP_MANAGER_SP_ACTION_IM_URI);
        return HTTP_BAD_REQUEST;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SP_ACTION_SI_URI, &(up_args->signature_uri)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_SI_URI);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SP_ACTION_SI_URI, o_message_jso,
            RFPROP_MANAGER_SP_ACTION_SI_URI);
        return HTTP_BAD_REQUEST;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SP_ACTION_IM_TYPE, &(up_args->image_type)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type, %s", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_IM_TYPE);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SP_ACTION_IM_TYPE, o_message_jso,
            RFPROP_MANAGER_SP_ACTION_IM_TYPE);
        return HTTP_BAD_REQUEST;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SP_ACTION_PARA, &(up_args->param)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type,  %s", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_PARA);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SP_ACTION_PARA, o_message_jso,
            RFPROP_MANAGER_SP_ACTION_PARA);
        return HTTP_BAD_REQUEST;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SP_ACTION_UP_MODE, &(up_args->upgrade_mode)) == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type,  %s", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_UP_MODE);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SP_ACTION_UP_MODE, o_message_jso,
            RFPROP_MANAGER_SP_ACTION_UP_MODE);
        return HTTP_BAD_REQUEST;
    }
    if (get_element_str(i_val_jso, RFPROP_MANAGER_SP_ACTION_ACTIVE_MODE, &(up_args->active_method))
        == FALSE) {
        debug_log(DLOG_ERROR, "%s: wrong value type,  %s", __FUNCTION__, RFPROP_MANAGER_SP_ACTION_UP_MODE);
        (void)create_message_info(MSGID_PROP_MISSING, RFPROP_MANAGER_SP_ACTION_ACTIVE_MODE, o_message_jso,
            RFPROP_MANAGER_SP_ACTION_ACTIVE_MODE);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}


LOCAL void __clear_sensitive_uri_information(PROVIDER_PARAS_S *i_paras)
{
    (void)json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_SP_ACTION_IM_URI);
    (void)json_object_clear_string(i_paras->val_jso, RFPROP_MANAGER_SP_ACTION_SI_URI);
}

LOCAL gint32 check_spservice_update_input(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    if (provider_paras_check(i_paras) != VOS_OK || o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    return RET_OK;
}


LOCAL gint32 _act_spservice_up_update(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    GSList *input_list = NULL;
    SPSERVICE_UP_ARGS up_args;

    ret = check_spservice_update_input(i_paras, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    (void)memset_s(&up_args, sizeof(up_args), 0, sizeof(up_args));
    
    if (__get_spservice_up_resource(i_paras->val_jso, &up_args, o_message_jso) != RET_OK) {
        (void)__clear_sensitive_uri_information(i_paras);
        return HTTP_BAD_REQUEST;
    }

    if (__check_spservice_update_uri(&up_args, o_message_jso) != RET_OK) {
        (void)__clear_sensitive_uri_information(i_paras);
        return HTTP_BAD_REQUEST;
    }
    
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.image_uri));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.signature_uri));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.image_type));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.param));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.upgrade_mode));
    input_list = g_slist_append(input_list, g_variant_new_string(up_args.active_method));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_UPDATE_FIRMWARE_SP, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    (void)memset_s(&up_args, sizeof(up_args), 0, sizeof(up_args));
    (void)__clear_sensitive_uri_information(i_paras);

    return get_update_firmware_sp_result(ret, o_message_jso);
}


LOCAL gint32 get_update_firmware_sp_result(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case RFERR_SUCCESS:
        case VOS_OK:
            
            return HTTP_OK;

        case UMS_WR_ERR_IN_UPGRADING:
            (void)create_message_info(MSGID_SP_IS_BEING_UPGRADED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_BUSY:
            (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_BMC_NOT_SUPPORT:
            (void)create_message_info(MSGID_BMC_NOT_SUPPORT_SP, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_WR_ERR_CAN_NOT_GET_LOCK:
            (void)create_message_info(MSGID_RSC_IN_USE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case UMS_ERR_EXIT_UPGRADE_TASK:
            (void)create_message_info(MSGID_TASK_DUMP_EXPORT_UPGRADING_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


gint32 manager_sp_update_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_update_provider;
        *count = sizeof(g_manager_sp_update_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 _get_sp_up_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_SP_UP_ACTION_INFOURI, slot);

    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S  g_manager_actioninfo_sp_up_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_NULL, _get_sp_up_actioninfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_sp_up_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    return_val_do_info_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_actioninfo_sp_up_provider;
        *count = sizeof(g_manager_actioninfo_sp_up_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}
