
#include "redfish_provider.h"
#include "redfish_forward.h"
#include "redfish_util.h"
LOCAL gint32 count_managers_collection_members(OBJ_HANDLE handle, gpointer user_data);

LOCAL void test_blade_managers_task(gpointer data)
{
    int iRet = -1;
    errno_t safe_fun_ret = EOK;
    struct tag_task_param *task_param = (struct tag_task_param *)data;
    struct tag_managers_collection_param *param = task_param->param;
    guint8 ipmb_addr = 0;
    json_object *header = NULL;
    json_object *response_info = NULL;
    json_object *message_array_obj = NULL;
    json_object *odata_id = NULL;
    json_object *blade = NULL;
    gint32 ret;
    gchar odata_str[MAX_URI_LEN + 1] = { 0 };
    const gchar *name_ptr = NULL;
    gchar name_f[MAX_NAME_SIZE] = { 0 };
    (void)dal_get_property_value_byte(task_param->blade_handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &ipmb_addr);
    message_array_obj = json_object_new_array();
    ret = send_redfish_request_to_target(ipmb_addr, NULL, param->req_info, &header, &response_info, message_array_obj);
    do_info_if_true(header, json_object_put(header); header = NULL);
    json_object_put(message_array_obj);
    do_if_expr(response_info, json_object_put(response_info); response_info = NULL);
    if (ret / HTTP_OK == 1) { // 单板支持透传管理
        blade = json_object_new_object();
        if (blade == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            goto exit;
        }
        name_ptr = dfl_get_object_name(task_param->blade_handle);
        safe_fun_ret = strncpy_s(name_f, sizeof(name_f), name_ptr, sizeof(name_f) - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        format_string_as_upper_head(name_f, strlen(name_f));
        iRet = snprintf_s(odata_str, sizeof(odata_str), sizeof(odata_str) - 1, "/redfish/v1/Managers/%s", name_f);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        odata_id = json_object_new_string((const gchar *)odata_str);
        if (odata_id == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            json_object_put(blade);
            goto exit;
        }
        json_object_object_add(blade, RFPROP_ODATA_ID, odata_id);
        (void)pthread_mutex_lock(&(param->write_lock)); // 增加记录
        param->members_count++;
        (void)json_object_array_add(param->members_array, blade);
        (void)pthread_mutex_unlock(&(param->write_lock));
    }
exit:
    // 通知任务结束
    (void)pthread_mutex_lock(&(param->m));
    param->task_running--;
    (void)pthread_cond_broadcast(&(param->c)); // 通知收集线程，任务数发生变化
    (void)pthread_mutex_unlock(&(param->m));
    g_free(task_param);
}


LOCAL gint32 count_managers_collection_members(OBJ_HANDLE handle, gpointer user_data)
{
    struct tag_managers_collection_param *param = (struct tag_managers_collection_param *)user_data;
    struct tag_task_param *task_param = NULL;
    gint32 ret;
    guint8 blade_type = 0;
    guint8 presence = 0;
    gulong task_id = 0;
    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0x03) { // 管理板，不访问
        return VOS_OK;
    }
    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    if (presence != BLADE_PRESENCE) {
        return VOS_OK;
    }
    
    task_param = (struct tag_task_param *)g_malloc(sizeof(struct tag_task_param));
    if (task_param == NULL) {
        debug_log(DLOG_ERROR, "[%s] Malloc failed", __func__);
        return VOS_OK; // 继续访问其他单板
    }
    task_param->param = param;
    task_param->blade_handle = handle;
    
    (void)pthread_mutex_lock(&(param->m));
    param->task_running++; // 增加任务数
    ret = vos_task_create(&task_id, "TestBladeManagersTask", (TASK_ENTRY)test_blade_managers_task, task_param,
        DEFAULT_PRIORITY);
    if (ret != VOS_OK) { // 创建失败
        g_free(task_param);
        debug_log(DLOG_ERROR, "[%s] Create task failed, err code %d", __func__, ret);
        param->task_running--;
    }
    (void)pthread_mutex_unlock(&(param->m));
    return VOS_OK;
}


LOCAL gint32 set_pthread_member_arry(struct tag_managers_collection_param *managers_param,
                                     json_object *o_rsc_jso)
{
    gchar slot_uri[PSLOT_URI_LEN] = {0};
    gchar   pslot[PSLOT_MAX_LEN] = {0};
    json_object* obj = NULL;
    gint32 ret;

    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) == TRUE) {
        ret = redfish_get_board_slot(pslot, sizeof(pslot));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        ret = snprintf_s(slot_uri, sizeof(slot_uri), sizeof(slot_uri) - 1, URI_FORMAT_MANAGER,  pslot);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
        managers_param->members_count = 1; 
        obj = json_object_new_object();
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(slot_uri));
        json_object_array_add(managers_param->members_array, obj);
    } else {
        managers_param->members_count = 2; 
        obj = json_object_new_object();
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(RF_MANAGERS_HMM1));
        json_object_array_add(managers_param->members_array, obj);
        obj = json_object_new_object();
        json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string(RF_MANAGERS_HMM2));
        json_object_array_add(managers_param->members_array, obj);
        
        (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, count_managers_collection_members, managers_param, NULL);

        
        (void)pthread_mutex_lock(&managers_param->m);
        
        while (managers_param->task_running != 0) {
            (void)pthread_cond_wait(&managers_param->c, &managers_param->m);
        }
        (void)pthread_mutex_unlock(&managers_param->m);

        json_object_array_sort(managers_param->members_array, shelf_managers_systems_sort_by_odataid);
    }

    
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(managers_param->members_count));
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS, managers_param->members_array);

    return RET_OK;
}


gint32 smm_get_managers_collection(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    struct tag_managers_collection_param managers_param = {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0
    };

    json_object *msg_obj = NULL;

    
    if (!o_rsc_jso || !o_err_array_jso || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj));

    REQUEST_INFO_S *req_info = (REQUEST_INFO_S *)g_malloc0(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_expr(req_info == NULL, VOS_ERR, debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__));

    
    (void)strcpy_s(req_info->request_method, sizeof(req_info->request_method), HTTP_GET);
    (void)strcpy_s(req_info->uri_lower, sizeof(req_info->uri_lower), RF_MANAGERS_URI);

    
    managers_param.req_info = req_info;
    managers_param.members_array = json_object_new_array();
    if (managers_param.members_array == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json array failed", __func__);
        free_request_info(req_info);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gint32 ret = set_pthread_member_arry(&managers_param, o_rsc_jso);
    g_free(req_info);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return RET_OK;
}
