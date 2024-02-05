
#include "redfish_provider.h"
#include "redfish_forward.h"
#include "redfish_util.h"
LOCAL gint32 count_systems_collection_members(OBJ_HANDLE handle, gpointer user_data);

LOCAL void test_blade_systems_task(gpointer data)
{
    int iRet = -1;
    errno_t safe_fun_ret = EOK;
    struct tag_task_param *task_param = (struct tag_task_param *)data;
    struct tag_managers_collection_param *managerParam = task_param->param;
    guint8 ipmb_addr = 0;
    json_object *header = NULL;
    json_object *response_info = NULL;
    json_object *message_array_obj = NULL;
    json_object *odata_id = NULL;
    json_object *blade = NULL;
    const gchar *name_ptr = NULL;
    gchar odata_str[MAX_URI_LEN + 1] = { 0 };
    gchar name_f[MAX_NAME_SIZE] = { 0 };
    gint32 ret;
    (void)dal_get_property_value_byte(task_param->blade_handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &ipmb_addr);
    message_array_obj = json_object_new_array();
    ret = send_redfish_request_to_target(ipmb_addr, NULL, managerParam->req_info, &header, &response_info,
        message_array_obj);
    do_info_if_true(header, json_object_put(header); header = NULL);
    json_object_put(message_array_obj);
    do_if_expr(response_info, json_object_put(response_info); response_info = NULL);
    if (ret / HTTP_OK == 1) { // 单板支持透传管理
        blade = json_object_new_object();
        if (NULL == blade) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            goto exit;
        }
        name_ptr = dfl_get_object_name(task_param->blade_handle);
        safe_fun_ret = strncpy_s(name_f, sizeof(name_f), name_ptr, sizeof(name_f) - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        format_string_as_upper_head(name_f, strlen(name_f));
        iRet = snprintf_s(odata_str, sizeof(odata_str), sizeof(odata_str) - 1, "/redfish/v1/Systems/%s", name_f);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        odata_id = json_object_new_string((const gchar *)odata_str);
        if (odata_id == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            json_object_put(blade);
            goto exit;
        }
        json_object_object_add(blade, RFPROP_ODATA_ID, odata_id);
        (void)pthread_mutex_lock(&(managerParam->write_lock)); // 增加记录
        managerParam->members_count++;
        (void)json_object_array_add(managerParam->members_array, blade);
        (void)pthread_mutex_unlock(&(managerParam->write_lock));
    }
exit:
    // 通知任务结束
    (void)pthread_mutex_lock(&(managerParam->m));
    managerParam->task_running--;
    (void)pthread_cond_broadcast(&(managerParam->c)); // 通知收集线程，任务数发生变化
    (void)pthread_mutex_unlock(&(managerParam->m));
    g_free(task_param);
}


LOCAL gint32 count_systems_collection_members(OBJ_HANDLE handle, gpointer user_data)
{
    struct tag_managers_collection_param *param = (struct tag_managers_collection_param *)user_data;
    struct tag_task_param *task_param = NULL;
    guint8 blade_type = 0;
    guint8 presence = 0;
    gulong task_id = 0;
    gint32 ret;

    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0x03) { // 0x03表示HMM管理板，如果是管理板则直接返回
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
    ret = vos_task_create(&task_id, "TestBladeSystemsTask", (TASK_ENTRY)test_blade_systems_task, task_param,
        DEFAULT_PRIORITY);
    if (ret != VOS_OK) { // 创建失败
        g_free(task_param);
        param->task_running--;
        debug_log(DLOG_ERROR, "[%s] Create task failed, err code %d", __func__, ret);
    }
    (void)pthread_mutex_unlock(&(param->m));

    return VOS_OK;
}


gint32 smm_get_systems_collection(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    struct tag_managers_collection_param systems_param = {
        0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0
    };
    json_object *msg_obj = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar systems_uri[MAX_URI_LEN + 1] = {0};
    guint8 as_status = 0;
    return_val_do_info_if_expr(!o_err_array_jso || !o_rsc_jso || (provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj));

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj));

    REQUEST_INFO_S *req_info = (REQUEST_INFO_S *)g_malloc0(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_expr(req_info == NULL, VOS_ERR, debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__));

    
    (void)strncpy_s(req_info->request_method, sizeof(req_info->request_method), HTTP_GET, strlen(HTTP_GET));

    gchar *uri = "/redfish/v1/Systems";
    (void)strcpy_s(req_info->uri_lower, sizeof(req_info->uri_lower), uri);
    
    
    systems_param.req_info = req_info;
    systems_param.members_array = json_object_new_array();
    if (systems_param.members_array == NULL) {
        free_request_info(req_info);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret == VOS_ERR, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    json_object *obj = json_object_new_object();
    ret = sprintf_s(systems_uri, sizeof(systems_uri), URI_FORMAT_SYSTEM, slot);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    json_object_object_add(obj, RFPROP_ODATA_ID, json_object_new_string((const gchar *)systems_uri));
    json_object_array_add(systems_param.members_array, obj);
    systems_param.members_count = 1;

    (void)get_board_active_state(&as_status);
    if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT) && as_status == ACTIVE_STATE) {
        
        (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, count_systems_collection_members, &systems_param, NULL);

        
        (void)pthread_mutex_lock(&systems_param.m);
        
        while (systems_param.task_running != 0) {
            (void)pthread_cond_wait(&systems_param.c, &systems_param.m);
        }
        (void)pthread_mutex_unlock(&systems_param.m);

        json_object_array_sort(systems_param.members_array, shelf_managers_systems_sort_by_odataid);
    }
    
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(systems_param.members_count));
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS, systems_param.members_array);
    
    free_request_info(req_info);
    return VOS_OK;
}
