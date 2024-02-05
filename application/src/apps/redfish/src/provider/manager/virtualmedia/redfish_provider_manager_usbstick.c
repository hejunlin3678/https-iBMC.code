
#include "redfish_provider.h"


LOCAL gint32 get_manager_usbstick_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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
    ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_MANAGER_USB_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string((const gchar *)slot_uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

gint32 get_manager_usbstick_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_context[MAX_URI_LENGTH] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = strcpy_s(odata_context, sizeof(odata_context), URI_FORMAT_MANAGR_USB_CONTEXT);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));

    return HTTP_OK;
}

LOCAL gint32 redfish_get_usbstick_connectstatus(guint8 *flag)
{
    gint32 ret;
    OBJ_HANDLE usbstick_handle = 0;

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &usbstick_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_object_handle_nth  fail. ", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_byte(usbstick_handle, UMS_PROPERTY_SERV_FLG, flag);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: dal_get_property_value_byte fail. ", __FUNCTION__, __LINE__));

    return VOS_OK;
}

LOCAL gint32 get_usbstick_connectedvia(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 flag = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_usbstick_connectstatus(&flag);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_usbstick_connectstatus fail. ", __FUNCTION__, __LINE__));

    if (USBSTICK_DISCONNECTED != flag) {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_OEM);
    } else {
        *o_result_jso = json_object_new_string(MANAGER_CONNECTVIA_VMM_NOTCONN);
    }

    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:json_object_new_string fail.", __FUNCTION__, __LINE__));
    return HTTP_OK;
}

LOCAL gint32 get_usbstick_inserted(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 flag = 0;
    gint32 ret;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_usbstick_connectstatus(&flag);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: redfish_get_usbstick_connectstatus fail. ", __FUNCTION__, __LINE__));

    (USBSTICK_DISCONNECTED != flag) ? (*o_result_jso = json_object_new_boolean(TRUE)) :
                                      (*o_result_jso = json_object_new_boolean(FALSE));

    return HTTP_OK;
}

gint32 get_usbstick_mediatype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    ret = json_object_array_add(*o_result_jso, json_object_new_string(MANAGER_VMM_MEDIATYPE_USBSTICK));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: json_object_array_add fail. ", __FUNCTION__, __LINE__));

    return HTTP_OK;
}


LOCAL gint32 usbstick_connect_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    USBSTICK_CONNECT_NEED_INFO *connect_usbstick_info_status = (USBSTICK_CONNECT_NEED_INFO *)monitor_fn_data->user_data;
    return_val_do_info_if_fail(
        (NULL != origin_obj_path && NULL != message_obj && NULL != &(monitor_fn_data->user_data)) && (NULL != body_jso),
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    connect_usbstick_info_status->usb_timeout = connect_usbstick_info_status->usb_timeout + 1;
    
    return_val_do_info_if_expr(20 < (connect_usbstick_info_status->usb_timeout) * REDFISH_TASK_INTERVAL_IN_SEC, VOS_OK,
        monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MESSAGE_OPERATIONTIMEOUT, NULL, message_obj));
    

    
    switch (connect_usbstick_info_status->check_usb_info) {
        case USB_CREATE_EXIST:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            
            (void)create_message_info(USBSTICK_MESSAGE_DOUBECONNECT, NULL, message_obj);
            break;

        case VOS_OK:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            
            (void)create_message_info(USB_MESSAGE_MOUNTED, NULL, message_obj);
            break;

        case VOS_ERR:
        case USB_CREATE_ERR:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            
            (void)create_message_info(USB_MESSAGE_CONNECT_FAILED, NULL, message_obj);
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_obj);
            break;
    }

    
    return VOS_OK; 
}


LOCAL void *usbstick_connect(void *data)
{
    gint32 ret;
    GSList *input_list = NULL;

    USBSTICK_CONNECT_NEED_INFO *usbstick_info = (USBSTICK_CONNECT_NEED_INFO *)data;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfUsbStickConn");
    if (usbstick_info == NULL) {
        debug_log(DLOG_ERROR, "%s: data is NULL.", __FUNCTION__);
        return NULL;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_LOCATION_LOCAL));
    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TYPE_IBMA));

    ret = uip_call_class_method_redfish(usbstick_info->from_webui_flag, usbstick_info->user_name, usbstick_info->client,
        usbstick_info->obj_handle, CLASS_IBMA_UMS_NAME, UMS_METHOD_CREAT_UMS, AUTH_DISABLE,
        usbstick_info->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    usbstick_info->check_usb_info = ret;
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: uip_call_class_method_redfish failed, ret=%d.", __FUNCTION__, ret);
        return NULL;
    }

    return NULL;
}



gint32 redfish_usbstickcontrol_connect(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *task_connect_status_info = NULL;
    USBSTICK_CONNECT_NEED_INFO *connect_usbstick_info = NULL;
    json_object *body_jso = NULL;
    body_jso = i_paras->val_jso;
    gulong thread_id = 0;

    connect_usbstick_info = (USBSTICK_CONNECT_NEED_INFO *)g_malloc0(sizeof(USBSTICK_CONNECT_NEED_INFO));
    return_val_do_info_if_expr(NULL == connect_usbstick_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:g_malloc0 failed.", __FUNCTION__, __LINE__));

    errno_t p_ret = memcpy_s(connect_usbstick_info->user_name, SESSION_USER_NAME_MAX_LEN + 1, i_paras->user_name,
        strlen(i_paras->user_name));
    if (p_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed", __FUNCTION__);
        goto ERROR_EXIT;
    }
    p_ret = memcpy_s(connect_usbstick_info->client, SESSION_IP_LEN, i_paras->client, strlen(i_paras->client));
    if (p_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed", __FUNCTION__);
        goto ERROR_EXIT;
    }

    connect_usbstick_info->check_usb_info = USB_OPERATIONINPROCESS;

    
    connect_usbstick_info->from_webui_flag = i_paras->is_from_webui;
    
    connect_usbstick_info->obj_handle = obj_handle;

    
    task_connect_status_info = task_monitor_info_new(g_free);
    if (task_connect_status_info == NULL) {
        debug_log(DLOG_ERROR, "%s: task_monitor_info_new failed", __FUNCTION__);
        goto ERROR_EXIT;
    }
    
    connect_usbstick_info->usb_timeout = 0;
    task_connect_status_info->task_progress = TASK_NO_PROGRESS; 
    task_connect_status_info->user_data = connect_usbstick_info;
    task_connect_status_info->rsc_privilege = USERROLE_VMMMGNT;
    
    ret = create_new_task(MANAGER_USBSTICK_CONNECTING_CHECK_TASK, usbstick_connect_status_monitor,
        task_connect_status_info, body_jso, i_paras->uri, o_result_jso);
    if (ret != VOS_OK) {
        task_monitor_info_free(task_connect_status_info);
        task_connect_status_info = NULL;
        debug_log(DLOG_ERROR, "%s: create_new_task failed", __FUNCTION__);
        goto ERROR_EXIT;
    }

    
    ret = vos_task_create(&thread_id, MANAGER_USBSTICK_CONNECTING_TASK, (TASK_ENTRY)usbstick_connect,
        connect_usbstick_info, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        task_connect_status_info = NULL;
        debug_log(DLOG_ERROR, "%s: vos_task_create failed", __FUNCTION__);
        goto ERROR_EXIT;
    }

    return HTTP_ACCEPTED;

ERROR_EXIT:
    g_free(connect_usbstick_info);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return VOS_ERR;
}

LOCAL gint32 usbstick_disconnect_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    USBSTICK_CONNECT_NEED_INFO *diconnect_usbstick_info_status =
        (USBSTICK_CONNECT_NEED_INFO *)monitor_fn_data->user_data;
    return_val_do_info_if_fail(
        (NULL != origin_obj_path && NULL != message_obj && NULL != &(monitor_fn_data->user_data)) && (NULL != body_jso),
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj));

    monitor_fn_data->task_state = RF_TASK_RUNNING;

    
    diconnect_usbstick_info_status->usb_timeout = diconnect_usbstick_info_status->usb_timeout + 1;
    return_val_do_info_if_expr(30 <= (diconnect_usbstick_info_status->usb_timeout) * REDFISH_TASK_INTERVAL_IN_SEC,
        VOS_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
        (void)create_message_info(MESSAGE_OPERATIONTIMEOUT, NULL, message_obj));
    

    switch (diconnect_usbstick_info_status->check_usb_info) {
        case USB_CLOSE_NONE_EXIST:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            
            (void)create_message_info(USBSTICK_MESSAGE_DOUBEDISCONNECT, NULL, message_obj);
            break;

        case VOS_OK:
            monitor_fn_data->task_state = RF_TASK_COMPLETED;
            monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
            
            (void)create_message_info(USB_MESSAGE_DISMOUNTED, NULL, message_obj);
            break;

        case VOS_ERR:
            monitor_fn_data->task_state = RF_TASK_EXCEPTION;
            
            (void)create_message_info(USB_MESSAGE_DIS_FAILED, NULL, message_obj);
            break;

        default:
            monitor_fn_data->task_state = RF_TASK_RUNNING;
            
            (void)create_message_info(VMM_MESSAGE_OPERATIONINPROCESS, NULL, message_obj);
            break;
    }

    
    return VOS_OK;
}



LOCAL void *usbstick_disconnect(void *data)
{
    gint32 ret;
    USBSTICK_CONNECT_NEED_INFO *usbstick_info = (USBSTICK_CONNECT_NEED_INFO *)data;

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfUsbStickDisconn");
    if (usbstick_info == NULL) {
        debug_log(DLOG_ERROR, "%s: data is NULL.", __FUNCTION__);
        return NULL;
    }

    ret = uip_call_class_method_redfish(usbstick_info->from_webui_flag, usbstick_info->user_name, usbstick_info->client,
        usbstick_info->obj_handle, CLASS_IBMA_UMS_NAME, UMS_METHOD_CLOSE_UMS, AUTH_DISABLE,
        usbstick_info->user_role_privilege, NULL, NULL);

    usbstick_info->check_usb_info = ret;

    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: uip_call_class_method_redfish failed.", __FUNCTION__);
        return NULL;
    }

    return NULL;
}



gint32 redfish_usbstickcontrol_disconnect(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    errno_t p_ret;
    TASK_MONITOR_INFO_S *task_disconnect_status_info = NULL;
    USBSTICK_CONNECT_NEED_INFO *diconnect_usbstick_info = NULL;
    json_object *body_jso = i_paras->val_jso;
    gulong thread_id = 0;

    
    diconnect_usbstick_info = (USBSTICK_CONNECT_NEED_INFO *)g_malloc0(sizeof(USBSTICK_CONNECT_NEED_INFO));
    return_val_do_info_if_expr(NULL == diconnect_usbstick_info, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d:g_malloc0 failed.", __FUNCTION__, __LINE__));

    p_ret = memcpy_s(diconnect_usbstick_info->user_name, SESSION_USER_NAME_MAX_LEN + 1, i_paras->user_name,
        strlen(i_paras->user_name));
    if (p_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed", __FUNCTION__);
        goto ERROR_EXIT;
    }

    p_ret = memcpy_s(diconnect_usbstick_info->client, SESSION_IP_LEN, i_paras->client, strlen(i_paras->client));
    if (p_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s failed", __FUNCTION__);
        goto ERROR_EXIT;
    }

    diconnect_usbstick_info->check_usb_info = USB_OPERATIONINPROCESS;
    
    diconnect_usbstick_info->from_webui_flag = i_paras->is_from_webui;
    
    diconnect_usbstick_info->obj_handle = obj_handle;

    task_disconnect_status_info = task_monitor_info_new(g_free);
    if (task_disconnect_status_info == NULL) {
        debug_log(DLOG_ERROR, "%s %d: task_monitor_info_new failed", __FUNCTION__, __LINE__);
        goto ERROR_EXIT;
    }

    diconnect_usbstick_info->usb_timeout = 0;
    task_disconnect_status_info->task_progress = TASK_NO_PROGRESS; 
    task_disconnect_status_info->user_data = diconnect_usbstick_info;
    task_disconnect_status_info->rsc_privilege = USERROLE_VMMMGNT;
    
    ret = create_new_task(MANAGER_USBSTICK_DISCONNECTING_CHECK_TASK, usbstick_disconnect_status_monitor,
        task_disconnect_status_info, body_jso, i_paras->uri, o_result_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s %d: create_new_task failed", __FUNCTION__, __LINE__);
        task_monitor_info_free(task_disconnect_status_info);
        diconnect_usbstick_info = NULL;
        goto ERROR_EXIT;
    }

        
    ret = vos_task_create(&thread_id, MANAGER_USBSTICK_DISCONNECTING_TASK, (TASK_ENTRY)usbstick_disconnect,
        diconnect_usbstick_info, DEFAULT_PRIORITY);
    if (ret != VOS_OK) {
        diconnect_usbstick_info = NULL;
        debug_log(DLOG_ERROR, "%s %d: vos_task_create failed", __FUNCTION__, __LINE__);
        goto ERROR_EXIT;
    }

    return HTTP_ACCEPTED;

ERROR_EXIT:
    g_free(diconnect_usbstick_info);
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return VOS_ERR;
}


gint32 act_usbstick_control_oem_process(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_bool ret_bool;
    gint32 result;
    json_object *obj_json = NULL;
    const gchar *actions = NULL;
    OBJ_HANDLE usbstick_handle = 0;
    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));
    
    return_val_do_info_if_expr(0 == (USERROLE_VMMMGNT & i_paras->user_role_privilege), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret_bool = json_object_object_get_ex(i_paras->val_jso, MANAGER_PROPERTY_USBSTICKCOLTYPE, &obj_json);
    return_val_do_info_if_expr(FALSE == ret_bool, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, PROPERTY_USBSTICK_CONTROL_ACT,
        MANAGER_PROPERTY_USBSTICKCOLTYPE));
    actions = dal_json_object_get_str(obj_json);

    result = dal_get_object_handle_nth(CLASS_IBMA_UMS_NAME, 0, &usbstick_handle);

    return_val_do_info_if_expr(VOS_OK != result, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    if (VOS_OK == g_strcmp0(MANAGER_PROPERTY_VMMCONNECT, actions)) {
        result = redfish_usbstickcontrol_connect(usbstick_handle, i_paras, o_message_jso, o_result_jso);
    }
    
    else if (VOS_OK == g_strcmp0(MANAGER_PROPERTY_VMMDISCONNECT, actions)) {
        result = redfish_usbstickcontrol_disconnect(usbstick_handle, i_paras, o_message_jso, o_result_jso);
    }

    return_val_if_expr(HTTP_BAD_REQUEST == result, HTTP_BAD_REQUEST);
    return_val_if_expr(HTTP_NOT_IMPLEMENTED == result, HTTP_NOT_IMPLEMENTED);
    return_val_if_expr(HTTP_ACCEPTED == result, HTTP_ACCEPTED);

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S g_usbstick_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_usbstick_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_usbstick_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_INSERTED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_usbstick_inserted, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_CONNECTVIA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_usbstick_connectedvia, NULL, NULL, ETAG_FLAG_ENABLE},
    {MANAGER_PROPERTY_VMM_MEDIATYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_usbstick_mediatype, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 manager_provider_usbstick_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;
    guint32 product_num = 0;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_if_expr(FALSE == rf_support_sp_service(), HTTP_NOT_FOUND);
    

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    
    ret = dal_get_product_version_num(&product_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get product_num fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_expr(PRODUCT_VERSION_V5 > product_num, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:Product is not support. ", __FUNCTION__, __LINE__));

    *prop_provider = g_usbstick_provider;
    *count = sizeof(g_usbstick_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
