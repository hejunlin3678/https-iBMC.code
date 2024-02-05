
#include "redfish_provider.h"

LOCAL gint32 get_manager_vmm_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_board_slot fail.", __FUNCTION__, __LINE__));
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, MANAGER_VMM_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string((const gchar *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_manager_vmm_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, MANAGR_VMM_CONTEXT, slot_id);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_vmmconnecturi(gchar *vmuri, gint32 len)
{
    gint32 ret;
    OBJ_HANDLE vmm_handle = 0;
    ret = dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &vmm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_string(vmm_handle, VMM_PROPERTY_CONNECT_URL, vmuri, len);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_string fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_connectnum(gint32 *num)
{
    gint32 ret;
    OBJ_HANDLE vmm_handle = 0;
    ret = dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &vmm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_int32(vmm_handle, VMM_PROPERTY_NUM, num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_string fail. ", __FUNCTION__, __LINE__));
    return VOS_OK;
}

LOCAL gint32 get_vmm_image(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar vmuri[VMM_MANAGER_IMAGE_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = get_vmmconnecturi(vmuri, sizeof(vmuri));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vmmconnecturi fail. ", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(0 != strlen(vmuri), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: image is null. ", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)vmuri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));
    return HTTP_OK;
}

LOCAL gint32 get_vmm_imagename(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar vmuri[VMM_MANAGER_IMAGE_LEN] = {0};
    gchar *flag_name = NULL;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = get_vmmconnecturi(vmuri, sizeof(vmuri));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vmmconnecturi fail. ", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(0 != strlen(vmuri), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s,%d: imagename is null. ", __FUNCTION__, __LINE__));
    flag_name = g_strrstr(vmuri, VMM_MANAGER_SEPARATOR);
    return_val_do_info_if_fail(NULL != flag_name, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: g_strrstr fail. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string(flag_name + 1);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 get_vmm_connectedvia(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 num = 0;
    gchar vmuri[VMM_MANAGER_IMAGE_LEN] = {0};
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = get_vmmconnecturi(vmuri, sizeof(vmuri));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vmmconnecturi fail. ", __FUNCTION__, __LINE__));

    ret = get_connectnum(&num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_connectnum fail. ", __FUNCTION__, __LINE__));

    if (1 == num && 0 != strlen(vmuri)) {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_URI);
    } else if (1 == num && 0 == strlen(vmuri)) {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_APPLET);
    } else {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_NOTCONN);
    }

    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));
    return HTTP_OK;
}

LOCAL gint32 get_vmm_inserted(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 num = 0;
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = get_connectnum(&num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_connectnum fail. ", __FUNCTION__, __LINE__));

    (num != 0) ? (*o_result_jso = json_object_new_boolean(TRUE)) : (*o_result_jso = json_object_new_boolean(FALSE));

    return HTTP_OK;
}

LOCAL gint32 get_vmm_mediatype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar vmuri[VMM_MANAGER_IMAGE_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_array();
    ret = get_vmmconnecturi(vmuri, sizeof(vmuri));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: get_vmmconnecturi fail. ", __FUNCTION__, __LINE__));
    return_val_if_fail(0 != strlen(vmuri), HTTP_OK);

    if (NULL != dal_str_has_casesuffix((const gchar *)vmuri, VMM_MANAGER_ISO)) {
        ret = json_object_array_add(*o_result_jso, json_object_new_string(VMM_MANAGER_MEDIATYPE));
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s,%d: json_object_array_add fail. ", __FUNCTION__, __LINE__));
    }

    return HTTP_OK;
}


LOCAL gint32 get_vmm_state(json_object* huawei_json, json_object* actions_json)
{
    gint32 ret;
    OBJ_HANDLE vmm_handle = 0;
    guint8 vmm_floppy = 0;
    gint32 vmm_encryption = 0;

    ret = dal_get_object_handle_nth(PROXY_VMM_CLASS_NAME, 0, &vmm_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth  fail. ", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_int32(vmm_handle, VMM_PROPERTY_COMPRESS_STATE, &vmm_encryption);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte  fail. ", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (vmm_encryption == VMM_ENCRYPTION_TRUE) ?
        (json_object_object_add(huawei_json, VMM_MANAGER_ENCRYPTION, json_object_new_boolean(TRUE))) :
        (json_object_object_add(huawei_json, VMM_MANAGER_ENCRYPTION, json_object_new_boolean(FALSE)));
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    json_object_object_add(huawei_json, VMM_MANAGER_ENCRYPTIONCONFIGURABLE, json_object_new_boolean(TRUE));
#else
    json_object_object_add(huawei_json, VMM_MANAGER_ENCRYPTIONCONFIGURABLE, json_object_new_boolean(FALSE));
#endif

    ret = dal_get_func_ability(VMM_CLASS_NAME, VMM_PROPERTY_VMM_FLOPPY_DRIVE, &vmm_floppy);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_func_ability failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (vmm_floppy == VMM_FLOPPY_ENABLE_TRUE) {
        json_object_object_add(huawei_json, PROPERTY_MANAGER_FLOPPYDRIVE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(huawei_json, PROPERTY_MANAGER_FLOPPYDRIVE, json_object_new_boolean(FALSE));
    }
    return ret;
}

/*****************************************************************************
 函 数 名  : get_manager_vmm_oem
 功能描述  : 更新oem
 输入参数  : PROVIDER_PARAS_S* i_paras
             json_object** o_message_jso
             json_object** o_result_jso
 输出参数  : 无
 返 回 值  : LOCAL

 修改历史      :
  1.日    期   : 2017年3月8日
    作    者   : gwx401761
    修改内容   : 新生成函数
  2.日    期   : 2017年4月15日
    作    者   : gwx401761
     修改内容   :      AR-0000276582-002-029
**************************************************************************** */
LOCAL gint32 get_manager_vmm_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei_json = NULL;
    json_object *actions_json = NULL;
    json_object *oem_vmm_control = NULL;

    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar vmmcontroluri[PSLOT_URI_LEN] = {0};
    gchar vmmactioninfo[PSLOT_URI_LEN] = {0};
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(vmmcontroluri, sizeof(vmmcontroluri), sizeof(vmmcontroluri) - 1, MANAGER_VMMCONTROL_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    ret = snprintf_s(vmmactioninfo, sizeof(vmmactioninfo), sizeof(vmmactioninfo) - 1, MANAGER_ACTIONINFO_URI, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    oem_vmm_control = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_vmm_control, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_object fail.", __FUNCTION__, __LINE__));
    json_object_object_add(oem_vmm_control, MANAGER_PROPERTY_VMMTARGET,
        json_object_new_string((const gchar *)vmmcontroluri));
    json_object_object_add(oem_vmm_control, MANAGER_PROPERTY_ACTIONINFO,
        json_object_new_string((const gchar *)vmmactioninfo));

    actions_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != actions_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(oem_vmm_control);
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_object fail.", __FUNCTION__, __LINE__));

    json_object_object_add(actions_json, MANAGER_PROPERTY_VMMCONTROL, oem_vmm_control);

    huawei_json = json_object_new_object();
    return_val_do_info_if_fail(NULL != huawei_json, HTTP_INTERNAL_SERVER_ERROR, json_object_put(actions_json);
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_object fail.", __FUNCTION__, __LINE__));

    // 获取VMM加密和软驱使能状态
    ret = get_vmm_state(huawei_json, actions_json);
    if (ret != RET_OK) {
        json_object_put(huawei_json);
        json_object_put(actions_json);
        huawei_json = NULL;
        actions_json = NULL;
        debug_log(DLOG_ERROR, "%s: function get_vmm_state fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(huawei_json, MANAGER_PROPERTY_ACTIONS, actions_json);
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != (*o_result_jso), HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_json);
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_object fail.", __FUNCTION__, __LINE__));
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_HUAWEI, huawei_json);

    return HTTP_OK;
}


LOCAL void check_vmm_connect_status(guchar status, json_object **message_obj, TASK_MONITOR_INFO_S *monitor_fn_data)
{
    switch (status) {
            
        case VMM_CLIENT_ON_CONNECTION:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_obj);
            debug_log(DLOG_ERROR, "%s, %d:The operation is executing.", __FUNCTION__, __LINE__);
            break;

            
        case VMM_CLIENT_CONNECT_SUCCESSFULLY:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            (void)create_message_info(VMM_MESSAGE_VMM_MOUNTED, NULL, message_obj);
            break;

            
        case VMM_CLIENT_ALREADY_CONNECTED:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(VMM_MESSAGE_CONNECTOCCUPIED, NULL, message_obj);
            debug_log(DLOG_ERROR, "%s, %d:The connection is occupied.", __FUNCTION__, __LINE__);
            break;

            
        case VMM_CLIENT_FILE_TYPE_WRONG:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(VMM_MESSAGE_WRONGFILE_FORMAT, NULL, message_obj);
            debug_log(DLOG_ERROR, "%s, %d:The file type is wrong.", __FUNCTION__, __LINE__);
            break;

        case VMM_CLIENT_PROTOCOL_MISMATCH:
            
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MSGID_FILE_TRANSFER_PROTOCOL_MISMATCH, NULL, message_obj);
            debug_log(DLOG_ERROR, "%s, %d:The protocol does not match.", __FUNCTION__, __LINE__);
            break;

            
        case VMM_CLIENT_PARAMERTER_LENGTH:
            
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(MAGID_VMM_IMAGE_LEN_ILLEGAL, NULL, message_obj);
            debug_log(DLOG_ERROR, "%s, %d:Parameter Image length is not legal.", __FUNCTION__, __LINE__);
            break;
            

        case VMM_CLIENT_HTTPS_SSL_VERIFY_FAILED:
            
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)parse_file_transfer_err_code(CURL_PEER_SSL_VERIFY_FAILED, message_obj);
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            (void)create_message_info(VMM_MESSAGE_CONNECT_FAILED, NULL, message_obj);
            debug_log(DLOG_ERROR, "%s, %d:connect failed.", __FUNCTION__, __LINE__);
            break;
    }
}

LOCAL gint32 vmm_connect_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    OBJ_HANDLE obj_handle;
    gint32 ret;
    guchar state = 0;
    gint32 *timeout = NULL;
    return_val_do_info_if_fail(
        (NULL != origin_obj_path && NULL != message_obj && NULL != &(monitor_fn_data->user_data)) && (NULL != body_jso),
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));
    timeout = (gint32 *)monitor_fn_data->user_data;

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    ret = dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));

    
    ret = dal_get_property_value_byte(obj_handle, VMM_PROPERTY_VMM_CONNECT_STATUS, &state);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));

    *timeout = *timeout + 1;
    
    return_val_do_info_if_expr(600 < (*timeout) * REDFISH_TASK_INTERVAL_IN_SEC, VOS_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MESSAGE_OPERATIONTIMEOUT, NULL, message_obj));
    
    check_vmm_connect_status(state, message_obj, monitor_fn_data);
    return VOS_OK; 
}

LOCAL gint32 redfish_vmmcontrol_connect(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *image = NULL;
    json_object *obj_json = NULL;
    json_bool ret_bool;
    gint32 ret;
    GSList *input_list = NULL;
    gint32 *timeout = NULL;
    json_object *body_jso;
    TASK_MONITOR_INFO_S *task_connect_monitor_info = NULL;
    body_jso = i_paras->val_jso;
    ret_bool = json_object_object_get_ex(i_paras->val_jso, MANAGER_PROPERTY_VMM_IMAGE, &obj_json);
    return_val_do_info_if_expr(FALSE == ret_bool, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, MANAGER_PROPERTY_VMVMMCONTROL,
        MANAGER_PROPERTY_VMM_IMAGE);
        debug_log(DLOG_ERROR, "%s, %d:json_object_object_get_ex failed.", __FUNCTION__, __LINE__));
    
    image = dal_json_object_get_str(obj_json);
    input_list = g_slist_append(input_list, g_variant_new_string(image));
    
    json_object_clear_string(i_paras->val_jso, MANAGER_PROPERTY_VMM_IMAGE);
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        VMM_CLASS_NAME, VMM_METHOD_SET_CONNECT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        (void)create_message_info(VMM_MESSAGE_CONNECT_FAILED, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d:uip_call_class_method_redfish failed.", __FUNCTION__, __LINE__));

    
    task_connect_monitor_info = task_monitor_info_new(g_free);
    return_val_do_info_if_fail(NULL != task_connect_monitor_info, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: task_monitor_info_new failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    timeout = (gint32 *)g_malloc0(sizeof(gint32));

    
    return_val_do_info_if_fail(NULL != timeout, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: g_malloc0 failed", __FUNCTION__, __LINE__);
        task_monitor_info_free(task_connect_monitor_info);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    

    *timeout = 0;
    task_connect_monitor_info->task_progress = TASK_NO_PROGRESS; 
    task_connect_monitor_info->user_data = timeout;
    task_connect_monitor_info->rsc_privilege = USERROLE_VMMMGNT;
    
    ret = create_new_task(VMM_MANAGER_CONNECT_TASK, vmm_connect_status_monitor, task_connect_monitor_info, body_jso,
        i_paras->uri, o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, task_monitor_info_free(task_connect_monitor_info);
        debug_log(DLOG_ERROR, "%s %d: create_new_task failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    return HTTP_ACCEPTED;
}

LOCAL gint32 vmm_disconnect_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 num = 0;
    return_val_do_info_if_fail(
        (NULL != origin_obj_path && NULL != message_obj && NULL != &(monitor_fn_data->user_data)) && (NULL != body_jso),
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));
    gint32 *diconnect_vmm_info_status = (gint32 *)monitor_fn_data->user_data;
    
    *diconnect_vmm_info_status = *diconnect_vmm_info_status + 1;
    
    return_val_do_info_if_expr(65 <= (*diconnect_vmm_info_status) * REDFISH_TASK_INTERVAL_IN_SEC, VOS_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MESSAGE_OPERATIONTIMEOUT, NULL, message_obj));

    ret = get_connectnum(&num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));
    return_val_do_info_if_expr(0 != num, VOS_OK, monitor_fn_data->task_state = RF_TASK_RUNNING;
        (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_obj));

    monitor_fn_data->task_state = RF_TASK_COMPLETED;
    monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
    (void)create_message_info(VMM_MESSAGE_VMM_DISMOUNTED, NULL, message_obj);
    return VOS_OK;
}

LOCAL gint32 vmm_disconnect_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;
    gint32 num = 0;
    return_val_do_info_if_fail(
        (NULL != origin_obj_path && NULL != message_obj && NULL != &(monitor_fn_data->user_data)) && (NULL != body_jso),
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));
    DISCONNECT_NEED_INFO *diconnect_vmm_info_status = (DISCONNECT_NEED_INFO *)monitor_fn_data->user_data;
    monitor_fn_data->task_state = RF_TASK_RUNNING;
    ret = get_connectnum(&num);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));
    return_val_do_info_if_expr(0 == num, VOS_OK, monitor_fn_data->task_state = RF_TASK_COMPLETED;
        monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
        (void)create_message_info(VMM_MESSAGE_VMM_DISMOUNTED, NULL, message_obj));
    
    ret =
        uip_call_class_method_redfish(diconnect_vmm_info_status->from_webui_flag, diconnect_vmm_info_status->user_name,
        diconnect_vmm_info_status->client, diconnect_vmm_info_status->obj_handle, VMM_CLASS_NAME,
        VMM_METHOD_DELECT_LINK, AUTH_ENABLE, diconnect_vmm_info_status->user_role_privilege, NULL, NULL);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        debug_log(DLOG_ERROR, "%s, %d:uip_call_class_method_redfish failed.", __FUNCTION__, __LINE__));
    monitor_fn_data->task_state = RF_TASK_COMPLETED;
    monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
    

    return VOS_OK;
}

LOCAL gint32 redfish_vmmcontrol_disconnect(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_bool ret_bool;
    TASK_MONITOR_INFO_S *task_disconnect_monitor_info = NULL;
    TASK_MONITOR_INFO_S *task_disconnect_status_info = NULL;
    DISCONNECT_NEED_INFO *diconnect_vmm_info = NULL;
    json_object *body_jso;
    json_object *obj_json = NULL;
    gint32 *dis_timeout = NULL;
    errno_t p_ret;
    body_jso = i_paras->val_jso;
    ret_bool = json_object_object_get_ex(i_paras->val_jso, MANAGER_PROPERTY_VMM_IMAGE, &obj_json);
    return_val_do_info_if_expr(TRUE == ret_bool, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_UNKNOWN, NULL, o_message_jso, MANAGER_PROPERTY_VMM_IMAGE);
        debug_log(DLOG_ERROR, "%s, %d:json_object_object_get_ex failed.", __FUNCTION__, __LINE__));

    task_disconnect_monitor_info = task_monitor_info_new(g_free);
    return_val_do_info_if_fail(NULL != task_disconnect_monitor_info, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: task_monitor_info_new failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    diconnect_vmm_info = (DISCONNECT_NEED_INFO *)g_malloc0(sizeof(DISCONNECT_NEED_INFO));
    return_val_do_info_if_expr(diconnect_vmm_info == NULL, VOS_ERR,
        task_monitor_info_free(task_disconnect_monitor_info);
        task_disconnect_monitor_info = NULL; debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__));
    (void)memset_s(diconnect_vmm_info->user_name, SESSION_USER_NAME_MAX_LEN + 1, 0, SESSION_USER_NAME_MAX_LEN + 1);
    (void)memset_s(diconnect_vmm_info->client, SESSION_IP_LEN, 0, SESSION_IP_LEN);
    p_ret = memcpy_s(diconnect_vmm_info->user_name, SESSION_USER_NAME_MAX_LEN + 1, i_paras->user_name,
        strlen(i_paras->user_name));
    return_val_do_info_if_fail(EOK == p_ret, VOS_ERR, g_free(diconnect_vmm_info); diconnect_vmm_info = NULL;
        task_monitor_info_free(task_disconnect_monitor_info); task_disconnect_monitor_info = NULL;
        debug_log(DLOG_ERROR, "%s %d: memcpy_s failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    p_ret = memcpy_s(diconnect_vmm_info->client, SESSION_IP_LEN, i_paras->client, strlen(i_paras->client));
    return_val_do_info_if_fail(EOK == p_ret, VOS_ERR, g_free(diconnect_vmm_info); diconnect_vmm_info = NULL;
        task_monitor_info_free(task_disconnect_monitor_info); task_disconnect_monitor_info = NULL;
        debug_log(DLOG_ERROR, "%s %d: memcpy_s failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    diconnect_vmm_info->obj_handle = obj_handle;
    diconnect_vmm_info->user_role_privilege = i_paras->user_role_privilege;
    task_disconnect_monitor_info->task_progress = TASK_NO_PROGRESS; 
    
    diconnect_vmm_info->from_webui_flag = i_paras->is_from_webui;
    
    task_disconnect_monitor_info->user_data = diconnect_vmm_info;
    task_disconnect_monitor_info->rsc_privilege = USERROLE_VMMMGNT;

    
    ret = create_new_task(VMM_MANAGER_DISCONNECT_TASK, vmm_disconnect_monitor, task_disconnect_monitor_info, body_jso,
        i_paras->uri, o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, task_monitor_info_free(task_disconnect_monitor_info);
        task_disconnect_monitor_info = NULL;
        debug_log(DLOG_ERROR, "%s %d: create_new_task failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    task_disconnect_status_info = task_monitor_info_new(g_free);
    return_val_do_info_if_fail(NULL != task_disconnect_status_info, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: task_monitor_info_new failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    

    dis_timeout = (gint32 *)g_malloc0(sizeof(gint32));
    
    return_val_do_info_if_fail(NULL != dis_timeout, VOS_ERR,
        debug_log(DLOG_ERROR, "%s %d: g_malloc0 failed", __FUNCTION__, __LINE__);
        task_monitor_info_free(task_disconnect_status_info);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    

    *dis_timeout = 0;
    task_disconnect_status_info->task_progress = TASK_NO_PROGRESS; 
    task_disconnect_status_info->user_data = dis_timeout;
    task_disconnect_status_info->rsc_privilege = USERROLE_VMMMGNT;
    
    ret = create_new_task(VMM_MANAGER_DISCONNECT_STAUS_TASK, vmm_disconnect_status_monitor, task_disconnect_status_info,
        body_jso, i_paras->uri, o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, task_monitor_info_free(task_disconnect_status_info);
        debug_log(DLOG_ERROR, "%s %d: create_new_task failed", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    return HTTP_ACCEPTED;
}

LOCAL gint32 act_vmm_control_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_bool ret_bool;
    gint32 result;
    json_object *obj_json = NULL;
    const gchar *actions = NULL;
    OBJ_HANDLE vmm_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_expr(0 == (USERROLE_VMMMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret_bool = json_object_object_get_ex(i_paras->val_jso, MANAGER_PROPERTY_VMMCOLTYPE, &obj_json);
    return_val_do_info_if_expr(FALSE == ret_bool, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, MANAGER_PROPERTY_VMVMMCONTROL,
        MANAGER_PROPERTY_VMMCOLTYPE));
    actions = dal_json_object_get_str(obj_json);
    result = dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &vmm_handle);
    return_val_do_info_if_expr(VOS_OK != result, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    if (VOS_OK == g_strcmp0(MANAGER_PROPERTY_VMMCONNECT, actions)) {
        result = redfish_vmmcontrol_connect(vmm_handle, i_paras, o_message_jso, o_result_jso);
    }
    
    else if (VOS_OK == g_strcmp0(MANAGER_PROPERTY_VMMDISCONNECT, actions)) {
        result = redfish_vmmcontrol_disconnect(vmm_handle, i_paras, o_message_jso, o_result_jso);
    }

    return_val_if_expr(HTTP_BAD_REQUEST == result, HTTP_BAD_REQUEST);
    return_val_if_expr(HTTP_ACCEPTED == result, HTTP_ACCEPTED);
    return_val_if_expr(VOS_OK == result, HTTP_OK);

    return HTTP_INTERNAL_SERVER_ERROR;
}

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
LOCAL gint32 get_kvm_encryption(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    gint32 kvm_encryption = 0;
    OBJ_HANDLE kvm_handle = 0;
    
    ret = dal_get_object_handle_nth(PROXY_KVM_CLASS_NAME, 0, &kvm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));
    ret = dal_get_property_value_int32(kvm_handle, KVM_PROPERTY_COMPRESS_STATE, &kvm_encryption);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_byte fail. ", __FUNCTION__, __LINE__));
    
    if (KVM_ENCRYPTION_TRUE != kvm_encryption) {
        (void)create_message_info(MSG_VMM_SET_ENCRYPTION_FAIL, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    return VOS_OK;
}
#endif


LOCAL gint32 set_vmmencryption(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object* o_result_jso)
{
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    gint32 ret;
    OBJ_HANDLE vmm_handle = 0;
    gint32 vmm_encryption;
    GSList* input_list = NULL;
    json_bool ret_bool;
#endif
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1

    ret_bool = json_object_get_boolean(o_result_jso);
    (VMM_ENCRYPTION_TRUE == ret_bool) ? (vmm_encryption = VMM_ENCRYPTION_TRUE) :
                                        (vmm_encryption = VMM_ENCRYPTION_FALSE);
    if (VMM_ENCRYPTION_TRUE == vmm_encryption) {
        ret = get_kvm_encryption(i_paras, o_message_jso);
        return_val_if_expr(VOS_OK != ret, ret);
    }

    ret = dal_get_object_handle_nth(PROXY_VMM_CLASS_NAME, 0, &vmm_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_int32(vmm_encryption));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vmm_handle,
        PROXY_VMM_CLASS_NAME, VMM_METHOD_SET_CRYPT_ENABLE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case MANAGER_KVM_IS_IN_USE:
            
            (void)create_message_info(MANAGER_KVM_MESSAGE_KVMUSE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;

        case VOS_OK:
            return HTTP_OK;

            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
#else
    (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, VMM_MANAGER_ENCRYPTION, o_message_jso,
        VMM_MANAGER_ENCRYPTION);
    return HTTP_BAD_REQUEST;
#endif
}


LOCAL gint32 set_vmm_floppy(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object* o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE vmm_handle = 0;
    guint8 vmm_floppy;
    GSList* input_list = NULL;

    if ((o_result_jso == NULL) || (provider_paras_check(i_paras) != RET_OK) || (o_message_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_object_handle_nth(VMM_CLASS_NAME, 0, &vmm_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth fail.",  __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    vmm_floppy = json_object_get_boolean(o_result_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte(vmm_floppy));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, vmm_handle,
        VMM_CLASS_NAME, VMM_METHOD_SET_FLOPPYDRIVE_STATE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case MANAGER_VMM_ENABLE_IN_USE:
            
            (void)create_message_info(MANAGER_VMM_ENABLE_INUSE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case MANAGER_VMM_DISABLE_IN_USE:
            
            (void)create_message_info(MANAGER_VMM_DISABLE_INUSE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case VOS_OK:
            return HTTP_OK;
            
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_manager_vmm_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret = HTTP_BAD_REQUEST;
    gboolean     flag_vmmencryption = FALSE;
    gboolean     flag_vmmfloppy = FALSE;
    json_object* huawei_json = NULL;
    json_object* encryption_json = NULL;
    json_object* floppy_json = NULL;
    json_bool ret_bool;
    const gchar* prop_name = "Oem/Huawei/EncryptionEnabled";

    if ((o_result_jso == NULL) || (provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!(i_paras->user_role_privilege & USERROLE_VMMMGNT)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name);
        return HTTP_FORBIDDEN;
    }

    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_HUAWEI, &huawei_json);
    if (!ret_bool) {
        return HTTP_BAD_REQUEST;
    }

    
    ret_bool = json_object_object_get_ex(huawei_json, PROPERTY_MANAGER_VMMENCRYPTION, &encryption_json);
    if (ret_bool && encryption_json != NULL) {
        ret = set_vmmencryption(i_paras, o_message_jso, encryption_json);
        if (ret == HTTP_OK) {
            flag_vmmencryption = TRUE;
        }
    }

    
    ret_bool = json_object_object_get_ex(huawei_json, PROPERTY_MANAGER_FLOPPYDRIVE, &floppy_json);
    if (ret_bool && floppy_json != NULL) { 
        ret = set_vmm_floppy(i_paras, o_message_jso, floppy_json);
        if (ret == HTTP_OK) {
            flag_vmmfloppy = TRUE;
        }
    }

    if (flag_vmmencryption == TRUE || flag_vmmfloppy == TRUE) {
        return HTTP_OK;
    } else {
        return ret;
    }
}

/*****************************************************************************
 功能描述  : BMC公共资源provider资源映射表
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无

 修改历史      :
  1.日    期   : 2017年3月8日
    作    者   : gwx401761
    修改内容   : 新生成函数
**************************************************************************** */
LOCAL PROPERTY_PROVIDER_S g_vmm_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmm_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_vmm_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_INSERTED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_vmm_inserted, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_CONNECTVIA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_vmm_connectedvia, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_IMAGE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_vmm_image, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_MEDIATYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_vmm_mediatype, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID, get_manager_vmm_oem, set_manager_vmm_oem, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_IMAGENAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_vmm_imagename, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMVMMCONTROL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_VMMMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_vmm_control_oem, ETAG_FLAG_ENABLE},
};

gint32 manager_provider_vmm_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    guint8 kvm_enabled = DISABLE;
    gint32 ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_func_ability fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (kvm_enabled == DISABLE) {
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    *prop_provider = g_vmm_provider;
    *count = sizeof(g_vmm_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
