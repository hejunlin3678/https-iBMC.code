



#include "pme_app/pme_app.h"

#include "redfish_task.h"

#include "redfish_util.h"
#include "json_patch_util.h"
#include "redfish_provider_resource.h"

#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_TASK_RSC_NUM 32


LOCAL TASK_RSC_S*      g_task_rsc_array[MAX_TASK_RSC_NUM] = {NULL};

LOCAL pthread_mutex_t g_task_rsc_mutex = PTHREAD_MUTEX_INITIALIZER;


LOCAL const char* g_task_state_str[] = { 
    "New",
    "Starting",
    "Running",
    "Suspended",
    "Interrupted",
    "Pending",
    "Stopping",
    "Completed",
    "Killed",
    "Exception",
    "Service"
};

#define TASK_ALIVE_TIME 300

LOCAL void delete_task(TASK_RSC_S **task_info);


LOCAL const char *get_task_state_str(REDFISH_TASK_STATE state)
{
    int n_state = (int)state;

    check_fail_return_val(((n_state >= 0) && (n_state < (int)G_N_ELEMENTS(g_task_state_str))), NULL);

    return g_task_state_str[n_state];
}


LOCAL gboolean task_info_compare_func(task_status_monitor_fn status_monitor_fn, json_object *param_jso,
    const char *origin_rsc_path, TASK_RSC_S *task_rsc)
{
    gboolean b_ret = FALSE;

    check_fail_return_val(task_rsc, FALSE);

    debug_log(DLOG_DEBUG, "rsc path %s , %s", origin_rsc_path, task_rsc->origin_rsc_path);
    
    check_success_do(!g_ascii_strcasecmp(origin_rsc_path, task_rsc->origin_rsc_path) &&
        (status_monitor_fn == task_rsc->status_monitor_fn) &&
        (!g_strcmp0(dal_json_object_get_str(param_jso), dal_json_object_get_str(task_rsc->param_jso))) &&
        RF_TASK_RUNNING == task_rsc->task_state,
        (b_ret = TRUE));
    

    return b_ret;
}


TASK_MONITOR_INFO_S *task_monitor_info_new(GDestroyNotify destroy_func)
{
    TASK_MONITOR_INFO_S *new_monitor_info;

    new_monitor_info = (TASK_MONITOR_INFO_S *)g_malloc0(sizeof(TASK_MONITOR_INFO_S));
    if (NULL == new_monitor_info) {
        return NULL;
    }

    new_monitor_info->user_data_destory_func = destroy_func;

    return new_monitor_info;
}


void task_monitor_info_free(TASK_MONITOR_INFO_S *task_monitor_info)
{
    if (NULL == task_monitor_info) {
        return;
    }

    if (NULL != task_monitor_info->response_header) {
        json_object_put(task_monitor_info->response_header);
    }

    if (NULL != task_monitor_info->extended_info) {
        json_object_put(task_monitor_info->extended_info);
    }

    if ((NULL != task_monitor_info->user_data) && (NULL != task_monitor_info->user_data_destory_func)) {
        task_monitor_info->user_data_destory_func(task_monitor_info->user_data);
    }

    g_free(task_monitor_info);

    return;
}


gint32 find_available_task_id(void)
{
    errno_t safe_fun_ret = EOK;
    int i;
    char            oldest_task_time[MAX_DATETIME_LEN] = {0};
    int new_task_index = -1;
    const char *task_end_time = NULL;

    (void)pthread_mutex_lock(&g_task_rsc_mutex);

    for (i = 0; i < MAX_TASK_RSC_NUM; i++) {
        
        if (!g_task_rsc_array[i]) {
            new_task_index = i;
            break;
        } else if (*(g_task_rsc_array[i]->end_time)) {
            task_end_time = g_task_rsc_array[i]->end_time;

            
            if (('\0' == *oldest_task_time) || (g_strcmp0(task_end_time, oldest_task_time) < 0)) {
                safe_fun_ret = strncpy_s(oldest_task_time, MAX_DATETIME_LEN, task_end_time, MAX_DATETIME_LEN - 1);
                do_val_if_expr(safe_fun_ret != EOK,
                    debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
                new_task_index = i;
            }
        } else {
            continue;
        }
    }

    
    return_val_do_info_if_fail(new_task_index >= 0, RF_INVALID_TASK_ID,
        debug_log(DLOG_ERROR, "task create failed, limit exceed");
        (void)pthread_mutex_unlock(&g_task_rsc_mutex));
    

    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return new_task_index;
}


LOCAL gint32 _add_task_info(TASK_RSC_S *task_rsc)
{
    int new_task_index;
    errno_t securec_rv;

    
    new_task_index = find_available_task_id();
    return_val_do_info_if_fail(RF_INVALID_TASK_ID != new_task_index, RF_TASK_LIMIT_EXCEED,
        debug_log(DLOG_ERROR, "task create failed, limit exceed"));
    

    (void)pthread_mutex_lock(&g_task_rsc_mutex);
    
    
    
    delete_task(&(g_task_rsc_array[new_task_index]));
    
    g_task_rsc_array[new_task_index] = task_rsc;

    securec_rv = snprintf_s(g_task_rsc_array[new_task_index]->task_id, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%d",
        new_task_index + 1);
    if (securec_rv <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv);
    }

    
    check_success_do('\0' == g_task_rsc_array[new_task_index]->task_name[0],
        (void)strncpy_s(g_task_rsc_array[new_task_index]->task_name, MAX_TASK_NAME_LEN,
        g_task_rsc_array[new_task_index]->task_id, MAX_TASK_NAME_LEN - 1));

    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return RF_OK;
}


json_object *pack_task_rsc(TASK_RSC_S *task_info)
{
    errno_t securec_rv;
    json_object *task_jso = NULL;
    char           task_rsc_path[MAX_URI_LENGTH] = {0};
    gint32 ret;
    const char *task_state_str = NULL;
    json_object *oem_jso = NULL;
    json_object *huawei_jso = NULL;
    gchar          task_progress_str[RF_MAX_PROGRESS_LEN] = {0};

    check_fail_do_return_val(task_info, NULL, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    task_jso = json_object_new_object();

    securec_rv = snprintf_s(task_rsc_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%s", TASK_COLLECTION_PATH,
        task_info->task_id);
    if (securec_rv <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv);
    }
    ret = fill_odata_head_info(task_rsc_path, task_jso);
    check_fail_do_return_val(RF_OK == ret, NULL,
        debug_log(DLOG_ERROR, "generate odata head for %s failed", task_rsc_path);
        (void)json_object_put(task_jso));

    task_state_str = get_task_state_str(task_info->task_state);
    json_object_object_add(task_jso, RFOBJ_ID, json_object_new_string(task_info->task_id));
    json_object_object_add(task_jso, RFOBJ_NAME, json_object_new_string(task_info->task_name));
    json_object_object_add(task_jso, RF_TASK_STATE_PROP, json_object_new_string(task_state_str));
    json_object_object_add(task_jso, RF_TASK_START_TIME_PROP, json_object_new_string(task_info->start_time));
    check_success_do('\0' != task_info->end_time[0],
        json_object_object_add(task_jso, RF_TASK_END_TIME_PROP, json_object_new_string(task_info->end_time)));
    check_success_do('\0' != task_info->status[0],
        json_object_object_add(task_jso, RF_TASK_STATUS_PROP, json_object_new_string(task_info->status)));

    
    json_object_object_add(task_jso, RF_TASK_MESSAGES_PROP, json_object_get(task_info->message_array_jso));

    
    oem_jso = json_object_new_object();
    huawei_jso = json_object_new_object();

    json_object_object_add(task_jso, RFPROP_COMMON_OEM, oem_jso);
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, huawei_jso);

    if (task_info->task_progress <= 100) {
        securec_rv = snprintf_s(task_progress_str, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1, "%d%%",
            task_info->task_progress);
        if (securec_rv <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, securec_rv);
        }
        json_object_object_add(huawei_jso, RF_TASK_PERCENTAGE,
            json_object_new_string((const gchar *)task_progress_str));
    } else {
        json_object_object_add(huawei_jso, RF_TASK_PERCENTAGE, NULL);
    }

    return task_jso;
}


json_object *get_task_rsc_from_id(int task_id, guint8 *task_priv, const gchar **task_owner)
{
    json_object *task_jso = NULL;
    
    if (task_id <= 0 || task_id > MAX_TASK_RSC_NUM) {
        debug_log(DLOG_ERROR, "invalid task num:%d", task_id);

        return NULL;
    }
    
    (void)pthread_mutex_lock(&g_task_rsc_mutex);
    if (g_task_rsc_array[task_id - 1] == NULL) {
        debug_log(DLOG_ERROR, "%s: task %d invalid", __FUNCTION__, task_id);
        (void)pthread_mutex_unlock(&g_task_rsc_mutex);
        return NULL;
    }
    task_jso = pack_task_rsc(g_task_rsc_array[task_id - 1]);
    if (task_priv != NULL) {
        *task_priv = (g_task_rsc_array[task_id - 1])->monitor_fn_data->rsc_privilege;
    }
    if (task_owner != NULL) {
        *task_owner = (const gchar *)(g_task_rsc_array[task_id - 1])->monitor_fn_data->task_owner;
    }
    (void)pthread_mutex_unlock(&g_task_rsc_mutex);
    return task_jso;
}

json_object *pack_task_monitor_data(TASK_RSC_S *task_info)
{
    json_object *task_jso = NULL;
    json_object *tmp_jso = NULL;
    const char *task_state_str = NULL;
    TASK_MONITOR_INFO_S *task_moniter_message = NULL;

    
    return_val_do_info_if_fail(NULL != task_info, NULL,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    
    task_state_str = get_task_state_str(task_info->task_state);
    return_val_do_info_if_fail(NULL != task_state_str, NULL,
        debug_log(DLOG_ERROR, "%s get_task_state_str fail.", __FUNCTION__));

    task_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != task_jso, NULL,
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__));

    
    tmp_jso = json_object_new_string(task_state_str);
    return_val_do_info_if_fail(NULL != tmp_jso, NULL, json_object_put(task_jso);
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));
    json_object_object_add(task_jso, RF_TASK_STATE_PROP, tmp_jso);
    tmp_jso = NULL;

    
    tmp_jso = json_object_new_string(task_info->origin_rsc_path);
    return_val_do_info_if_fail(NULL != tmp_jso, NULL, json_object_put(task_jso);
        debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));
    json_object_object_add(task_jso, RF_MONITOR_ORIGIN_RESOURSE, tmp_jso);
    tmp_jso = NULL;

    
    task_moniter_message = task_info->monitor_fn_data;

    if (strlen(task_moniter_message->create_resourse) > 0) {
        tmp_jso = json_object_new_string(task_moniter_message->create_resourse);
        return_val_do_info_if_fail(NULL != tmp_jso, NULL, json_object_put(task_jso);
            debug_log(DLOG_ERROR, "%s json_object_new_string fail.", __FUNCTION__));
        json_object_object_add(task_jso, RF_MONITOR_CREATE_RESOURSE, tmp_jso);
    }

    if (NULL != task_info->monitor_fn_data->extended_info) {
        json_object_object_add(task_jso, RF_MONITOR_EXTENDED_INFO,
            json_object_get(task_info->monitor_fn_data->extended_info));
    }
    return task_jso;
}

json_object *get_task_monitor_need_info(int task_id, guint8 *task_priv, const gchar **task_owner)
{
    json_object *task_jso = NULL;
    
    if (task_id <= 0 || task_id > MAX_TASK_RSC_NUM) {
        debug_log(DLOG_ERROR, "invalid task num:%d", task_id);
        return NULL;
    }
    
    (void)pthread_mutex_lock(&g_task_rsc_mutex);
    task_jso = pack_task_monitor_data(g_task_rsc_array[task_id - 1]);
    if (task_priv != NULL) {
        *task_priv = (g_task_rsc_array[task_id - 1])->monitor_fn_data->rsc_privilege;
    }
    if (task_owner != NULL) {
        *task_owner = (const gchar *)(g_task_rsc_array[task_id - 1])->monitor_fn_data->task_owner;
    }
    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return task_jso;
}


TASK_RSC_S *get_task_rsc_structure(gint32 task_id)
{
    TASK_RSC_S *task_rsc = NULL;

    if (task_id <= 0 || task_id > MAX_TASK_RSC_NUM) {
        debug_log(DLOG_ERROR, "invalid task num:%d", task_id);
        return NULL;
    }

    (void)pthread_mutex_lock(&g_task_rsc_mutex);
    task_rsc = g_task_rsc_array[task_id - 1];
    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return task_rsc;
}


GSList *get_cur_task_list()
{
    int i;
    GSList *task_list = NULL;

    (void)pthread_mutex_lock(&g_task_rsc_mutex);

    for (i = 0; i < MAX_TASK_RSC_NUM; i++) {
        if (g_task_rsc_array[i]) {
            task_list = g_slist_append(task_list, GINT_TO_POINTER(i + 1));
        }
    }

    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return task_list;
}



json_object *get_exist_running_task(task_status_monitor_fn status_monitor_fn, json_object *param_jso,
    const char *origin_rsc_path)
{
    int i;
    TASK_RSC_S *task_rsc = NULL;
    json_object *task_jso = NULL;

    (void)pthread_mutex_lock(&g_task_rsc_mutex);

    for (i = 0; i < MAX_TASK_RSC_NUM; i++) {
        if (task_info_compare_func(status_monitor_fn, param_jso, origin_rsc_path, g_task_rsc_array[i])) {
            task_rsc = g_task_rsc_array[i];
            break;
        }
    }

    check_success_do(task_rsc, (task_jso = pack_task_rsc(task_rsc)));

    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return task_jso;
}



gboolean check_redfish_running_task_exist(task_status_monitor_fn status_monitor_fn, json_object *param_jso,
    const gchar *origin_rsc_path, gint32 *task_id)
{
    int i;
    gboolean b_ret = FALSE;

    return_val_do_info_if_fail((NULL != status_monitor_fn) && (NULL != param_jso) && (NULL != origin_rsc_path), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    (void)pthread_mutex_lock(&g_task_rsc_mutex);

    for (i = 0; i < MAX_TASK_RSC_NUM; i++) {
        if (task_info_compare_func(status_monitor_fn, param_jso, origin_rsc_path, g_task_rsc_array[i])) {
            b_ret = TRUE;
            
            if (task_id) {
                *task_id = i + 1;
            }
            
            break;
        }
    }
    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return b_ret;
}


gboolean check_export_task_exist(task_status_monitor_fn status_monitor_fn, json_object* param_jso, 
    const gchar* origin_rsc_path, gint32* task_id)
{
    int i;
    gboolean b_ret = FALSE;
    TASK_RSC_S* task_rsc = NULL;

    if (status_monitor_fn == NULL || param_jso == NULL || origin_rsc_path == NULL)  {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return b_ret;
    }

    (void)pthread_mutex_lock(&g_task_rsc_mutex);
    for (i = 0; i < MAX_TASK_RSC_NUM; i++) {
        if (g_task_rsc_array[i] == NULL) {
            continue;
        }

        task_rsc = g_task_rsc_array[i];
        if (g_ascii_strcasecmp(origin_rsc_path, task_rsc->origin_rsc_path) == 0 &&
            status_monitor_fn == task_rsc->status_monitor_fn && RF_TASK_RUNNING == task_rsc->task_state) {
            b_ret = TRUE;
            if (task_id != NULL) {
                *task_id = i + 1;
            }
            break;
        }
    }
    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return b_ret;
}


gint32 create_new_task(const char *task_name, task_status_monitor_fn status_monitor_fn,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object *param_jso, const char *origin_obj_path, json_object **task_obj)
{
    /*lint -save -e429*/
    TASK_RSC_S *task_info = NULL;

    check_success_do(task_obj, (*task_obj = NULL));
    
    check_fail_do_return_val(task_obj && status_monitor_fn && monitor_fn_data && origin_obj_path, RF_FAILED,
        debug_log(DLOG_ERROR, "task create failed, param error"));
    

    task_info = (TASK_RSC_S *)g_malloc0(sizeof(TASK_RSC_S));
    
    return_val_if_expr(NULL == task_info, RF_FAILED);
    

    // 填充Name属性
    check_success_do(task_name,
        (void)strncpy_s(task_info->task_name, MAX_TASK_NAME_LEN, task_name, MAX_TASK_NAME_LEN - 1));

    
    (void)dal_get_redfish_datetime_stamp(0, task_info->start_time, MAX_DATETIME_LEN);
    task_info->task_state = RF_TASK_RUNNING;

    task_info->status_monitor_fn = status_monitor_fn;
    task_info->monitor_fn_data = monitor_fn_data;
    (void)strncpy_s(task_info->origin_rsc_path, MAX_URI_LENGTH, origin_obj_path, MAX_URI_LENGTH - 1);
    
    task_info->param_jso = json_object_get(param_jso);
    task_info->stop_flag = FALSE;
    task_info->alive_time = TASK_INITIAL_TMOUT;
    task_info->task_progress = TASK_NO_PROGRESS;

    if (RF_OK != _add_task_info(task_info)) {
        json_object_put(param_jso);
        g_free(task_info);
        return RF_RSC_LIMIT_EXCEEDED;
    }

    (void)pthread_mutex_lock(&g_task_rsc_mutex);
    task_info->message_array_jso = json_object_new_array();
    *task_obj = pack_task_rsc(task_info);
    (void)pthread_mutex_unlock(&g_task_rsc_mutex);

    return RF_OK;
}


LOCAL void delete_task(TASK_RSC_S **task_info)
{
    TASK_RSC_S *info = NULL;

    check_fail_return(task_info && *task_info);

    info = *task_info;

    (void)json_object_put(info->param_jso);
    (void)json_object_put(info->message_array_jso);

    
    task_monitor_info_free(info->monitor_fn_data);

    (void)memset_s(info, sizeof(TASK_RSC_S), 0, sizeof(TASK_RSC_S));

    g_free(info);

    
    *task_info = NULL;

    return;
}


LOCAL void _redfish_change_task_status(TASK_RSC_S *task_rsc, TASK_MONITOR_INFO_S *monitor_info,
    json_object *message_obj)
{
    
    const char *task_status = NULL;
    

    check_fail_return(NULL != task_rsc);

    task_rsc->task_state = monitor_info->task_state;
    task_rsc->task_progress = monitor_info->task_progress;

    if (message_obj) {
        
        (void)json_object_put(task_rsc->message_array_jso);
        task_rsc->message_array_jso = message_obj;
    }

    // 下列状态表明任务已经结束，这时需要更新"EndTime"/"TaskStatus"属性，并且将Task资源添加到倒计时列表
    switch (task_rsc->task_state) {
            /*lint -save -e616*/
        case RF_TASK_KILLED: {
            
            check_success_do(!task_status, (task_status = RF_STATUS_CRITICAL));
            
        }

        case RF_TASK_COMPLETED: {
            
            check_success_do(!task_status, (task_status = RF_STATUS_OK));
            
        }

        case RF_TASK_EXCEPTION: {
            check_success_do(!task_status, (task_status = RF_STATUS_WARNING));
            
            errno_t securec_rv = strcpy_s(task_rsc->status, MAX_TASK_STATUS_LEN, task_status);
            if (securec_rv != EOK) {
                debug_log(DLOG_ERROR, "%s: copy src file path fail", __FUNCTION__);
            }
        
            (void)dal_get_redfish_datetime_stamp(0, task_rsc->end_time, MAX_DATETIME_LEN);

            task_rsc->stop_flag = TRUE;
            break;
        }

        default:
            break;
            /*lint -restore*/
    }

    return;
}


static void task_list_foreach_func(TASK_RSC_S *task_rsc)
{
    json_object *message_obj = NULL;
    gint32 ret;

    check_fail_return(task_rsc);

    
    check_fail_return(!task_rsc->stop_flag);
    

    ret = task_rsc->status_monitor_fn(task_rsc->origin_rsc_path, task_rsc->param_jso, task_rsc->monitor_fn_data,
        &message_obj);
    if (RF_OK != ret) {
        debug_log(DLOG_ERROR, "update task info for %s failed", task_rsc->origin_rsc_path);

        (void)json_object_put(message_obj);
        return;
    }

    _redfish_change_task_status(task_rsc, task_rsc->monitor_fn_data, message_obj);

    return;
}


/*lint -save -e527*/
static void *status_monitor_thread(void *data)
{
    (void)prctl(PR_SET_NAME, (uintptr_t) "rfTaskMonitor");
    for (;;) {
        int i;

        (void)pthread_mutex_lock(&g_task_rsc_mutex);

        for (i = 0; i < MAX_TASK_RSC_NUM; i++) {
            TASK_RSC_S *task_info = g_task_rsc_array[i];

            task_list_foreach_func(task_info);

            if (task_info && task_info->stop_flag) {
                task_info->alive_time -= REDFISH_TASK_INTERVAL_IN_SEC;

                
                check_success_do(task_info->alive_time <= 0, delete_task(&g_task_rsc_array[i]));
            }
        }

        (void)pthread_mutex_unlock(&g_task_rsc_mutex);

        (void)vos_task_delay(1000 * REDFISH_TASK_INTERVAL_IN_SEC);
    }

    return NULL;
}
/*lint -restore*/


int redfish_task_init(void)
{
    gulong task_id;

    (void)vos_task_create(&task_id, "redfish task thread", (TASK_ENTRY)status_monitor_thread, NULL, DEFAULT_PRIORITY);

    return 0;
}
