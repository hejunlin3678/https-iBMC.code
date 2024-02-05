
#include <malloc.h>
#include "curl/curl.h"

#include "redfish_provider.h"

#include "redfish_event.h"
#include "redfish_event_custom.h"

#define EVT_TEMPLATE_FILE               "/redfish/v1/redfish_template/event_template.json"
#define EVT_MSG_TEMPLATE_FILE           "/redfish/v1/redfish_template/event_message_template.json"

#define EVT_LANG_FILE_PATH (EVENT_LANGUAGE_FILE_PATH "/" EVENT_LANGUAGE_FILE)

#define MAX_EVT_SUBSCRIPTION_NUM 4


#define RF_RSC_MONITOR_INTERVAL_IN_SEC 20


#define RF_EVT_SCAN_CHECK_INTERVAL_IN_SEC 3


#define RF_EVT_SCAN_RSC_INTERVAL_MS 50

#define RF_EVT_SUBSCRIBER_BMA 0
#define RF_EVT_SUBSCRIBER_OTHER 1
#define MAX_CACHED_RETRY 10

#define MINUTE_TO_MSEC 60
#define HOUR_TO_MSEC (60 * MINUTE_TO_MSEC)
#define DAY_TO_MSEC (24 * HOUR_TO_MSEC)
#define MONTH_TO_MSEC (30 * DAY_TO_MSEC)
#define YEAR_TO_MSEC (365 * 24 * 60 * 60)

LOCAL GAsyncQueue*                     g_evt_msg_queue[MAX_EVT_SUBSCRIPTION_NUM] = {NULL};
LOCAL GAsyncQueue*                     g_forever_queue[MAX_EVT_SUBSCRIPTION_NUM] = {NULL};

LOCAL guint8 g_evt_subscriber_certs[MAX_EVT_SUBSCRIPTION_NUM][RF_EVT_SUBSCRIBER_ROOTCA_COUNT] = {
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
    {1, 2, 3, 4},
};


typedef struct _tag_event_post_queue_info {
    json_object *evt_jso;
    OBJ_HANDLE obj_handle;
    gboolean ignore_ssl_cert_verify;
} EVT_POST_QUEUE_INFO;

typedef struct _tag_rsc_etag_info {
    
    gchar status_prop_path[MAX_STATUS_PROP_PATH_LEN];

    
    gchar status_etag[ETAG_LEN];
} RSC_ETAG_INFO;

typedef struct _tag_rsc_monitor_data {
    
    gchar rsc_node_name[MAX_MEM_ID_LEN];

    
    gulong last_update_timestamp;

    
    gchar rsc_no_status_etag[ETAG_LEN];

    
    gchar rsc_shield_etag[ETAG_LEN];

    
    GSList *rsc_status_etag_info;
} RSC_MONITOR_DATA;

typedef struct subscription_info {
    guint8 id;
    gint32 max_retry_time;
    gint32 delay_sec;
    guint8 subscriber_type;
    json_object *headers_jso;
    gchar ip_addr[MAX_IPV6_LEN];
} SUBSCRIPTION_IFNO;

#define EVENTS_REPORT_CURL_ERR_CODE_NUM 10
typedef struct event_curl_resp_code_info {
    gint32 resp_code;
    gchar time[MAX_DATETIME_LEN];
} EVENT_CURL_RESP_CODE_INFO;

typedef struct event_report_info {
    EVENT_CURL_RESP_CODE_INFO resp_info[EVENTS_REPORT_CURL_ERR_CODE_NUM];
    guint32 len;
    guint32 head_pos;
} EVENT_REPORT_INFO;


enum EVT_HANDLE_POLICY {
    RESUBMIT = 0, // 继续上报
    CACHED_SUSPENDED = 1, // 事件已经被缓存到挂起事件数据库
    ABANDONED = 2, // 事件已经被丢弃
    CACHED_RETRY = 3, // 事件缓存到重试数据库
    FORWAR_RETRY_FOREVER, // 事件转发到永远尝试的地方
    POLICY_ERROR = 0xff // 策略内部处理错误
};


typedef enum {
    EVENT_ORIGIN_TYPE_CURRENT = 0, // 当前产生的事件
    EVENT_ORIGIN_TYPE_RETRY_FOREVER = 1, // 一直重试产生的事件
    EVENT_ORIGIN_TYPE_SUSPEND_RETRIES = 2, // 恢复挂起产生的事件
} EVT_ORIGIN_TYPE;

typedef enum {
    BEFORE_CREATE_THREAD = 1, // 表示未起多线程
    BEFORE_ENQUIRY_SUM = 2, // 表示未查询事件的总数量
    SENDING = 3, // 发送事件中
    SENDING_COMPLETED = 4 // 事件发送完成
} RESUME_ACTION_PHASE;

typedef struct eo_task_entity {
    guint32 end_time;
    guint32 start_time;
    RESUME_ACTION_PHASE processing_phase; // task运行到了那个阶段
    OBJ_HANDLE obj_handle;
    gint32 sub_id;
    gint32 number_of_failed_event; // 事件总数量
    gint32 number_of_successfully_send; // 发送成功的事件数量
} RESUME_TASK_MONITOR_INFO;

guint8 g_evt_retry_forever_flag[RF_EVT_SUBSCRIPTION_ID_MAX] = {0};
typedef struct handle_queue_evt_info {
    guint8 id;
    enum CACHE_EVENT_TYPE type;
} HANDLE_QUEUE_EVT_INFO;
typedef struct clear_temp_info {
    gchar *type_str;
    const gchar **type_strp;
    json_object **type_json;
} CLEAR_TEMP_INFO;

typedef struct {
    GAsyncQueue* queue;
    EVT_ORIGIN_TYPE type;
} POCT_EVENT_THERAD_INFO;

LOCAL EVENT_REPORT_INFO g_evt_report_result[MAX_EVT_SUBSCRIPTION_NUM]  = {0};
LOCAL gchar g_manufacturer[TMP_STR_MAX] = RFPROP_COMMON_HUAWEI;
LOCAL pthread_mutex_t g_event_rsc_id_mutex = PTHREAD_MUTEX_INITIALIZER;
LOCAL guint32 g_event_rsc_id = 0;
LOCAL guint32 g_last_alert_id = 0;

LOCAL pthread_mutex_t g_rsc_node_tree_mutex = PTHREAD_MUTEX_INITIALIZER;
LOCAL GNode *g_rsc_node_tree = NULL;

LOCAL gulong g_cur_update_timestamp;

LOCAL guint8 g_rsc_initail_update = 1;

LOCAL void _post_redfish_evt_thread(void *data);

LOCAL gint32 _update_rsc_tree_node_from_uri(const gchar *rsc_uri);

LOCAL RSC_MONITOR_DATA *_new_empty_rsc_node_data(const gchar *node_name);

LOCAL void _check_redfish_evt_rsc_for_hmm(guchar evt_rsc_type, gchar *board_slot);

LOCAL void hand_event_after_sent(OBJ_HANDLE description);

LOCAL enum EVT_HANDLE_POLICY handle_evt_policy(EVT_POST_QUEUE_INFO *evt_queue_msg,
    gint32 already_retry_count, EVT_ORIGIN_TYPE origin_type);

LOCAL gint32 get_evt_subscription_id(OBJ_HANDLE subscription, guint8 *id);
LOCAL gint32 create_evt_from_template(json_object* evt_msg_jso, guint32 evt_id,
    OBJ_HANDLE description, EVT_POST_QUEUE_INFO **msg_queue_info);
LOCAL void _free_evt_queue_msg_info(EVT_POST_QUEUE_INFO **queue_msg_info);
LOCAL guint32 increase_and_get_event_rsc_id(void);

LOCAL void record_evt_report_resp_info(guint8 channel_id, gint32 resp_code)
{
    guint32 pos;

    if (channel_id < RF_EVT_SUBSCRIPTION_ID_MIN || channel_id > RF_EVT_SUBSCRIPTION_ID_MAX) {
        return;
    }

    guint8 id = channel_id - 1;
    if (g_evt_report_result[id].len == 0) {
        pos = 0;
    } else {
        pos = (g_evt_report_result[id].head_pos != 0) ? (g_evt_report_result[id].head_pos - 1) : 
            (EVENTS_REPORT_CURL_ERR_CODE_NUM - 1);
    }

    // 只记录与上次不一样的事件上报响应信息
    if (g_evt_report_result[id].resp_info[pos].resp_code != resp_code) {
        // 更新响应码和时间
        g_evt_report_result[id].resp_info[g_evt_report_result[id].head_pos].resp_code = resp_code;
        (void)dal_get_redfish_datetime_stamp(0, 
            g_evt_report_result[id].resp_info[g_evt_report_result[id].head_pos].time, MAX_DATETIME_LEN);

        // 递增一次记录
        if (g_evt_report_result[id].len < EVENTS_REPORT_CURL_ERR_CODE_NUM) {
            g_evt_report_result[id].len += 1;
        }
        g_evt_report_result[id].head_pos += 1;
        g_evt_report_result[id].head_pos = g_evt_report_result[id].head_pos % EVENTS_REPORT_CURL_ERR_CODE_NUM;
    }
    
    return;
}


gint32 redfish_print_evt_report_error_info(GSList *input_list)
{
    // 遍历所有事件订阅通道
    for (gint32 i = 0; i < MAX_EVT_SUBSCRIPTION_NUM; i++) {
        debug_printf("channel %d:", i + 1);
        // 遍历订阅通道的事件响应信息，并打印
        for (gint32 j = 0; j < g_evt_report_result[i].len; j++) {
            gint32 pos = g_evt_report_result[i].head_pos - 1 - j;
            if (pos < 0) {
                pos = pos + EVENTS_REPORT_CURL_ERR_CODE_NUM;
            }
            debug_printf("%s : curl response %d", g_evt_report_result[i].resp_info[pos].time, 
                g_evt_report_result[i].resp_info[pos].resp_code);
        }
    }
    return RET_OK;
}


gint32 rf_dump_evt_sender_info(const gchar* path)
{
    gchar file_name[MAX_FILENAME_LEN] = {0};
    errno_t secure_ret;
    secure_ret = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/event_sender_info.log", path);
    if (secure_ret <= 0) {
        debug_log(DLOG_ERROR, "%s: call snprintf_s failed, ret:%d", __FUNCTION__, secure_ret);
        return RET_ERR;
    }

    FILE *fp = dal_fopen_check_realpath((const gchar*)file_name, "w+", (const gchar*)file_name);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: g_fopen failed", __FUNCTION__);
        return RET_ERR;
    }

    
    (void)chmod((const gchar*)file_name, (S_IRUSR | S_IWUSR | S_IRGRP));

    for (gint32 i = 0; i < MAX_EVT_SUBSCRIPTION_NUM; i++) {
        g_fprintf(fp, "channel %d:\n", i + 1);
        for (gint32 j = 0; j < g_evt_report_result[i].len; j++) {
            gint32 pos = g_evt_report_result[i].head_pos - 1 - j;
            if (pos < 0) {
                pos = pos + EVENTS_REPORT_CURL_ERR_CODE_NUM;
            }
            g_fprintf(fp, "%s: curl response %d\n", g_evt_report_result[i].resp_info[pos].time, 
                g_evt_report_result[i].resp_info[pos].resp_code);
        }
    }

    (void)fclose(fp);
    
    (void)chmod((const gchar*)file_name, (S_IRUSR | S_IRGRP));
    return RET_OK;
}


LOCAL GNode *_find_child_rsc_node_by_name(GNode *parent, const gchar *node_name)
{
    if (parent == NULL || node_name == NULL) {
        return NULL;
    }

    GNode *node = parent->children;
    while (node != NULL) {
        GNode *current = node;
        node = current->next;

        RSC_MONITOR_DATA *monitor_data = (RSC_MONITOR_DATA *)current->data;
        if (g_ascii_strcasecmp(node_name, monitor_data->rsc_node_name) == 0) {
            return current;
        }
    }

    return NULL;
}


LOCAL gint32 _find_or_new_rsc_tree_node(const gchar *rsc_uri, GNode **node)
{
    gchar               rsc_uri_dup[MAX_URI_LENGTH] = {0};
    gchar*              rsc_node_segment[MAX_URI_SEGMENT_NUM] = {0};
    RSC_MONITOR_DATA *monitor_data = NULL;

    return_val_do_info_if_fail((NULL != rsc_uri) && (NULL != node), VOS_ERR,
        debug_log(DLOG_DEBUG, "%s failed:input param error", __FUNCTION__));

    errno_t str_ret = strncpy_s(rsc_uri_dup, MAX_URI_LENGTH, rsc_uri, strlen(rsc_uri));
    return_val_do_info_if_fail(EOK == str_ret, VOS_ERR, debug_log(DLOG_ERROR, "dup rsc uri %s failed", rsc_uri));

    redfish_trim_uri(rsc_uri_dup, sizeof(rsc_uri_dup));

    gint32 segment_num = split_path(rsc_uri_dup, sizeof(rsc_uri_dup), rsc_node_segment);
    return_val_do_info_if_fail(segment_num >= 0, VOS_ERR, debug_log(DLOG_MASS, "split rsc uri %s failed", rsc_uri));

    if (NULL == g_rsc_node_tree) {
        monitor_data = _new_empty_rsc_node_data(SERVICE_ROOT_PATH);
        return_val_do_info_if_fail(NULL != monitor_data, VOS_ERR,
            debug_log(DLOG_ERROR, "alloc new resource monitor data failed"));

        g_rsc_node_tree = g_node_new(monitor_data);
        return_val_do_info_if_fail(NULL != g_rsc_node_tree, VOS_ERR, debug_log(DLOG_ERROR, "alloc new GNode failed");
            g_free(monitor_data));

        debug_log(DLOG_DEBUG, "new root node successfully");
    }

    GNode *parent = g_rsc_node_tree;
    GNode *leaf = g_rsc_node_tree;

    
    for (gint32 i = 0; i < segment_num; i++) {
        leaf = _find_child_rsc_node_by_name(parent, rsc_node_segment[i]);
        if (NULL == leaf) {
            monitor_data = _new_empty_rsc_node_data(rsc_node_segment[i]);
            return_val_do_info_if_fail(NULL != monitor_data, VOS_ERR,
                debug_log(DLOG_ERROR, "alloc new resource monitor data failed"));

            leaf = g_node_append_data(parent, monitor_data);
            return_val_do_info_if_fail(NULL != leaf, VOS_ERR, debug_log(DLOG_ERROR, "alloc new GNode failed");
                g_free(monitor_data));
        }

        parent = leaf;
    }

    *node = leaf;

    return VOS_OK;
}


LOCAL gint32 _create_rsc_status_etag_list(json_object *rsc_jso, gchar *prop_path, guint32 prop_path_len,
    GSList **status_etag_list)
{
    RSC_ETAG_INFO *etag_info = NULL;
    gint32 ret;
    json_object *array_iter = NULL;
    gchar               prop_path_dup[MAX_URI_LENGTH] = {0};
    errno_t ret_string;

    return_val_do_info_if_fail((json_type_object == json_object_get_type(rsc_jso)) && (NULL != status_etag_list) &&
        (NULL != prop_path),
        VOS_ERR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret_string = strncpy_s(prop_path_dup, MAX_URI_LENGTH, prop_path, strlen(prop_path));
    return_val_do_info_if_fail(EOK == ret_string, VOS_ERR,
        debug_log(DLOG_ERROR, "dup string for %s failed", prop_path));


#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(rsc_jso, prop_name, val_jso)
    {
        // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
        if (!g_strcmp0(RFPROP_STATUS, prop_name)) {
            etag_info = (RSC_ETAG_INFO *)g_malloc0(sizeof(RSC_ETAG_INFO));
            return_val_do_info_if_fail(NULL != etag_info, VOS_ERR,
                debug_log(DLOG_ERROR, "alloc new rsc etag info failed"));

            
            if (RF_OK !=
                json_custom_get_elem_str(rsc_jso, ODATA_ID, etag_info->status_prop_path, MAX_STATUS_PROP_PATH_LEN)) {
                ret_string = strncpy_s(etag_info->status_prop_path, MAX_STATUS_PROP_PATH_LEN, prop_path_dup,
                    strlen(prop_path_dup));
                return_val_do_info_if_fail(EOK == ret_string, VOS_ERR,
                    debug_log(DLOG_ERROR, "copy status path %s failed", prop_path_dup);
                    g_free(etag_info));

                debug_log(DLOG_MASS, "get @odata.id failed, use default property path:%s", prop_path_dup);
            }

            rf_gen_json_object_etag(val_jso, etag_info->status_etag, ETAG_LEN);

            
            *status_etag_list = g_slist_append(*status_etag_list, etag_info);
            json_object_object_del(rsc_jso, RFPROP_STATUS);
        } else if (json_type_array == json_object_get_type(val_jso)) {
            json_object_array_foreach(val_jso, array_iter)
            {
                if (json_type_object != json_object_get_type(array_iter)) {
                    break;
                }

                ret = snprintf_s(prop_path_dup, sizeof(prop_path_dup), sizeof(prop_path_dup) - 1, "%s/%s/%d", prop_path,
                    prop_name, array_iter_i);
                return_val_do_info_if_fail(ret > 0, VOS_ERR,
                    debug_log(DLOG_ERROR, "join %s and %d failed, ret is %d", prop_path, array_iter_i, ret));

                ret = _create_rsc_status_etag_list(array_iter, prop_path_dup, prop_path_len, status_etag_list);
                return_val_do_info_if_fail(VOS_OK == ret, ret,
                    debug_log(DLOG_ERROR, "%s failed, ret is %d", __FUNCTION__, ret));
            }
        } else if (json_type_object == json_object_get_type(val_jso)) {
            ret = snprintf_s(prop_path_dup, sizeof(prop_path_dup), sizeof(prop_path_dup) - 1, "%s/%s", prop_path,
                prop_name);
            return_val_do_info_if_fail(ret > 0, VOS_ERR,
                debug_log(DLOG_ERROR, "join %s with %s failed, ret is %d", prop_path, prop_name, ret));

            ret = _create_rsc_status_etag_list(val_jso, prop_path_dup, prop_path_len, status_etag_list);
            return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
                debug_log(DLOG_ERROR, "%s failed, ret is %d", __FUNCTION__, ret));
        } else {
            continue;
        }
    }

    return VOS_OK;
}


LOCAL void _free_rsc_node_data(RSC_MONITOR_DATA *data)
{
    return_do_info_if_fail(NULL != data, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    if (data->rsc_status_etag_info) {
        g_slist_free_full(data->rsc_status_etag_info, (GDestroyNotify)g_free);
    }

    g_free(data);

    return;
}


LOCAL RSC_MONITOR_DATA *_new_empty_rsc_node_data(const gchar *node_name)
{
    return_val_do_info_if_fail((NULL != node_name), NULL,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    RSC_MONITOR_DATA *new_data = (RSC_MONITOR_DATA *)g_malloc0(sizeof(RSC_MONITOR_DATA));
    return_val_do_info_if_fail(NULL != new_data, NULL, debug_log(DLOG_ERROR, "alloc new resource monitor data failed"));

    errno_t str_ret = strncpy_s(new_data->rsc_node_name, MAX_MEM_ID_LEN, node_name, strlen(node_name));
    return_val_do_info_if_fail(EOK == str_ret, NULL, debug_log(DLOG_ERROR, "copy string %s failed", node_name);
        g_free(new_data));

    return new_data;
}

LOCAL gint32 fill_resource_provider_input_paras(const gchar *rsc_uri, PROVIDER_PARAS_S *paras)
{
    (void)strncpy_s(paras->user_name, SESSION_USER_NAME_MAX_LEN + 1, NA_STR, strlen(NA_STR));
    (void)strncpy_s(paras->client, SESSION_IP_LEN, INNER_IP_ADDR, strlen(INNER_IP_ADDR));

    gint32 ret = strncpy_s(paras->uri, MAX_URI_LENGTH, rsc_uri, strlen(rsc_uri));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    gchar *member_id = g_strrstr(rsc_uri, SLASH_FLAG_STR);
    if (member_id == NULL) {
        debug_log(DLOG_ERROR, "%s: get member id from %s failed", __FUNCTION__, rsc_uri);
        return RET_ERR;
    }

    ret = strncpy_s(paras->member_id, MAX_MEM_ID_LEN, member_id + 1, strlen(member_id + 1));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void get_rsc_etag_without_status(json_object **rsc_jso, const gchar *rsc_uri, RSC_MONITOR_DATA old_monitor_data,
    RSC_MONITOR_DATA *monitor_data)
{
    RSC_EVT_SHIELD_TYPE shield_type;

    
    rf_gen_json_object_etag(*rsc_jso, monitor_data->rsc_no_status_etag, ETAG_LEN);

    
    gint32 ret = delete_shield_resources(rsc_uri, rsc_jso, &shield_type);
    if (ret != RET_OK) {
        
        (void)memcpy_s(monitor_data->rsc_shield_etag, ETAG_LEN, monitor_data->rsc_no_status_etag, ETAG_LEN);
        return;
    }

    
    if (shield_type == EVT_SHIELD_RESOURCE) {
        
        (void)memcpy_s(monitor_data->rsc_no_status_etag, ETAG_LEN, old_monitor_data.rsc_no_status_etag, ETAG_LEN);
    }

    
    if (shield_type != EVT_SHIELD_ATTRUBITES) {
        (void)memcpy_s(monitor_data->rsc_shield_etag, ETAG_LEN, monitor_data->rsc_no_status_etag, ETAG_LEN);
        return;
    }

    rf_gen_json_object_etag(*rsc_jso, monitor_data->rsc_shield_etag, ETAG_LEN);
    return;
}


LOCAL RSC_MONITOR_DATA *_new_rsc_node_data(RSC_MONITOR_DATA old_monitor_data, json_object **rsc_jso,
    const gchar *rsc_uri)
{
    gchar *uri_lower = NULL;
    RESOURCE_PROVIDER_S *resource_provider = NULL;
    PROVIDER_PARAS_S i_paras;
    guint32 count;
    guint32 array_index;
    PROPERTY_PROVIDER_S *property_provider = NULL;
    gint32 ret;
    gchar prop_path[MAX_URI_LENGTH] = {0};
    GSList *status_etag_list = NULL;
    RSC_MONITOR_DATA *monitor_data = NULL;
    const gchar *rsc_node_name = NULL;

    memset_s(&i_paras, sizeof(PROVIDER_PARAS_S), 0, sizeof(PROVIDER_PARAS_S));
    ret = fill_resource_provider_input_paras(rsc_uri, &i_paras);
    if (ret != RET_OK) {
        return NULL;
    }

    uri_lower = g_ascii_strdown(rsc_uri, strlen(rsc_uri));
    
    if (VOS_OK == get_resource_provider((const gchar *)uri_lower, &resource_provider) &&
        resource_provider->resource_provider_fn &&
        VOS_OK == resource_provider->resource_provider_fn(&i_paras, &property_provider, &count)) {
        do_val_if_expr(NULL != resource_provider->etag_del_prop, (void)resource_provider->etag_del_prop(*rsc_jso));

        // 遍历资源属性，如果不支持Etag计算则从对象中剔除
        for (array_index = 0; array_index < count; array_index++) {
            do_val_if_expr(ETAG_FLAG_ENABLE != property_provider[array_index].etag_flag,
                json_object_object_del(*rsc_jso, property_provider[array_index].property_name));
        }
    }

    g_free(uri_lower);
    
    (void)rf_provider_paras_free(&i_paras);

    ret = snprintf_s(prop_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s#", rsc_uri);
    return_val_do_info_if_fail(ret > 0, NULL,
        debug_log(DLOG_ERROR, "format string for %s failed, ret is %d", rsc_uri, ret));

    
    ret = _create_rsc_status_etag_list(*rsc_jso, prop_path, MAX_URI_LENGTH, &status_etag_list);
    return_val_do_info_if_fail(VOS_OK == ret, NULL,
        debug_log(DLOG_ERROR, "create resource status etag list failed, ret is %d", ret);
        g_slist_free_full(status_etag_list, (GDestroyNotify)g_free));

    rsc_node_name = (0 == g_strcmp0(rsc_uri, SERVICE_ROOT_PATH)) ? SERVICE_ROOT_PATH : i_paras.member_id;
    monitor_data = _new_empty_rsc_node_data(rsc_node_name);
    return_val_do_info_if_fail(NULL != monitor_data, NULL,
        debug_log(DLOG_ERROR, "alloc new resource monitor data failed");
        g_slist_free_full(status_etag_list, (GDestroyNotify)g_free));

    monitor_data->rsc_status_etag_info = status_etag_list;
    monitor_data->last_update_timestamp = g_cur_update_timestamp;

    
    get_rsc_etag_without_status(rsc_jso, rsc_uri, old_monitor_data, monitor_data);

    return monitor_data;
}


LOCAL void _check_rsc_changed(const gchar *rsc_uri, RSC_MONITOR_DATA *old_data, RSC_MONITOR_DATA *new_data)
{
    gint32 ret;
    GSList *old_list_iter = NULL;
    GSList *new_list_iter = NULL;
    guint32 old_list_len;
    guint32 new_list_len;
    RSC_ETAG_INFO *old_etag_info = NULL;
    RSC_ETAG_INFO *new_etag_info = NULL;
    gchar prop_path_dup[MAX_URI_LENGTH] = {0};
    errno_t str_ret = EOK;
    guint32 len;

    
    if ((g_strcmp0(old_data->rsc_no_status_etag, new_data->rsc_no_status_etag) != 0) &&
        (g_strcmp0(old_data->rsc_shield_etag, new_data->rsc_shield_etag) != 0)) {
        debug_log(DLOG_DEBUG, "begin to send resource update event for %s", rsc_uri);
        ret = dal_rf_rsc_evt_log(RF_RSC_UPDATED_EVT, FALSE, "%s", rsc_uri);
        do_val_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "send rsc updated event for %s failed, ret is %d", rsc_uri, ret));
    }

    
    old_list_len = g_slist_length(old_data->rsc_status_etag_info);
    new_list_len = g_slist_length(new_data->rsc_status_etag_info);
    return_do_info_if_fail(old_list_len == new_list_len,
        debug_log(DLOG_ERROR, "etag list len of new old mismatch for %s", rsc_uri));

    new_list_iter = new_data->rsc_status_etag_info;
    for (old_list_iter = old_data->rsc_status_etag_info; NULL != old_list_iter; old_list_iter = old_list_iter->next) {
        old_etag_info = (RSC_ETAG_INFO *)old_list_iter->data;
        new_etag_info = (RSC_ETAG_INFO *)new_list_iter->data;

        return_do_info_if_fail((NULL != old_etag_info) && (NULL != new_etag_info),
            debug_log(DLOG_ERROR, "compare status etag failed, null list data"));

        if (0 != g_strcmp0(old_etag_info->status_prop_path, new_etag_info->status_prop_path)) {
            debug_log(DLOG_ERROR, "compare status etag failed, prop path mismatch: %s and %s",
                old_etag_info->status_prop_path, new_etag_info->status_prop_path);

            return;
        }

        if (0 != g_strcmp0(old_etag_info->status_etag, new_etag_info->status_etag)) {
            str_ret = strncpy_s(prop_path_dup, MAX_URI_LENGTH, old_etag_info->status_prop_path,
                strlen(old_etag_info->status_prop_path));

            return_do_info_if_fail(EOK == str_ret,
                debug_log(DLOG_ERROR, "copy %s failed", old_etag_info->status_prop_path));

            
            len = strlen(prop_path_dup);
            do_val_if_expr('#' == prop_path_dup[len - 1], (prop_path_dup[len - 1] = '\0'));

            debug_log(DLOG_DEBUG, "begin to send resource status change event for %s", prop_path_dup);
            ret = dal_rf_rsc_evt_log(RF_RSC_STATUS_CHANGED_EVT, FALSE, "%s", prop_path_dup);
            do_val_if_fail(VOS_OK == ret,
                debug_log(DLOG_ERROR, "send status change event for %s failed", prop_path_dup));
        }

        new_list_iter = new_list_iter->next;
    }

    return;
}


LOCAL void _rsc_link_node_data_update(json_object *rsc_jso)
{
    json_object *array_iter = NULL;
    gint32 ret;
    
    const gchar *resource_uri = NULL;
    
    json_type t;

    t = json_object_get_type(rsc_jso);
    if (json_type_object == t) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
        json_object_object_foreach0(rsc_jso, name, val)
        {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
            t = json_object_get_type(val);

            
            if (!g_strcmp0(name, ODATA_ID)) {
                resource_uri = dal_json_object_get_str(val);
                ret = _update_rsc_tree_node_from_uri(resource_uri);
                do_val_if_fail(VOS_OK == ret,
                    debug_log(DLOG_MASS, "update resource tree node for %s failed", resource_uri));

                
                vos_task_delay(RF_EVT_SCAN_RSC_INTERVAL_MS);
                
            } else if ((json_type_object == t) || (json_type_array == t)) {
                _rsc_link_node_data_update(val);
            } else {
                continue;
            }
        }
    } else if (json_type_array == t) {
        json_object_array_foreach(rsc_jso, array_iter)
        {
            t = json_object_get_type(array_iter);
            break_if_fail(json_type_object == t);

            _rsc_link_node_data_update(array_iter);
        }
    } else {
        debug_log(DLOG_ERROR, "%s failed:input param error, type is %d", __FUNCTION__, t);
        return;
    }

    return;
}


LOCAL const gchar* g_rsc_scan_black_list[] = {
    RF_EVT_SCAN_BLACK_LIST_LOGENTRY,
    RF_EVT_SCAN_BLACK_LIST_JSON_SCHEMA,
    RF_EVT_SCAN_BLACK_LIST_MANAGER_LOGENTRY,
    RF_EVT_SCAN_BLACK_LIST_DATA_ACQ_REPORT,
    RF_EVT_SCAN_BLACK_LIST_SYSTEM_OVERVIEW,
    RF_EVT_SCAN_BLACK_LIST_REGISTRIES,
    RF_EVT_SCAN_BLACK_LIST_SCHEMASTORE,
    RF_EVT_SCAN_BLACK_LIST_PRIVILEGE_MAP
};


LOCAL gint32 _rf_check_rsc_scan_blacklist(const gchar *rsc_uri)
{
    gboolean b_ret;

    
    return_val_if_expr(NULL == rsc_uri, VOS_ERR);
    

    gint32 count = G_N_ELEMENTS(g_rsc_scan_black_list);

    for (gint32 i = 0; i < count; i++) {
        b_ret = g_regex_match_simple(g_rsc_scan_black_list[i], rsc_uri, (GRegexCompileFlags)0, (GRegexMatchFlags)0);
        return_val_if_expr(TRUE == b_ret, VOS_ERR);
    }

    debug_log(DLOG_MASS, "scan condition check ok for %s", rsc_uri);

    return VOS_OK;
}



LOCAL gint32 _update_rsc_tree_node_from_uri(const gchar *rsc_uri)
{
    GNode *node_found = NULL;
    RSC_MONITOR_DATA *rsc_monitor_data_found = NULL;
    RSC_MONITOR_DATA *rsc_monitor_data_new = NULL;
    gint32 ret;
    json_object *rsc_jso = NULL;
    json_object *tmp_jso = NULL;

    
    
    return_val_if_expr((NULL == rsc_uri) || (NULL != g_strrstr(rsc_uri, URI_JSON_POINTER_FLAG)), VOS_OK);
    

    
    ret = _rf_check_rsc_scan_blacklist(rsc_uri);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_MASS, "rsc scan condition check failed, rsc uri (%s)", rsc_uri));
    

    (void)_find_or_new_rsc_tree_node(rsc_uri, &node_found);
    return_val_do_info_if_fail(NULL != node_found, VOS_ERR,
        debug_log(DLOG_MASS, "find or alloc new node for %s failed", rsc_uri));

    rsc_monitor_data_found = (RSC_MONITOR_DATA *)node_found->data;
    
    if (rsc_monitor_data_found->last_update_timestamp == g_cur_update_timestamp) {
        debug_log(DLOG_MASS, "warning: rsc node data at %s has been updated", rsc_uri);

        return VOS_OK;
    }

    ret = rf_validate_rsc_exist(rsc_uri, &rsc_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "validate rsc at %s failed, ret is %d", rsc_uri, ret));

    rsc_monitor_data_new = _new_rsc_node_data(*rsc_monitor_data_found, &rsc_jso, rsc_uri);
    return_val_do_info_if_fail(NULL != rsc_monitor_data_new, VOS_ERR,
        debug_log(DLOG_ERROR, "alloc new resource node data failed");
        (void)json_object_put(rsc_jso); rsc_jso = NULL);

    
    if ((0 == rsc_monitor_data_found->last_update_timestamp) && (0 == g_rsc_initail_update)) {
        debug_log(DLOG_DEBUG, "begin to send rsc added event for %s", rsc_uri);
        ret = dal_rf_rsc_evt_log(RF_RSC_ADDED_EVT, FALSE, "%s", rsc_uri);
        do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "send resource added event for %s failed", rsc_uri));
    } else {
        
        do_val_if_expr(0 != rsc_monitor_data_found->last_update_timestamp,
            _check_rsc_changed(rsc_uri, rsc_monitor_data_found, rsc_monitor_data_new));
    }

    
    _free_rsc_node_data(rsc_monitor_data_found);
    node_found->data = rsc_monitor_data_new;

    
    json_object_object_get_ex(rsc_jso, ODATA_ID, &tmp_jso);
    if (tmp_jso != NULL) {
        json_object_object_del(rsc_jso, ODATA_ID);
    }

    

    

    (void)vos_task_delay(RF_EVT_SCAN_RSC_INTERVAL_MS);
    
    _rsc_link_node_data_update(rsc_jso);

    (void)json_object_put(rsc_jso);

    return VOS_OK;
}


LOCAL void _rf_maintain_rsc_tree(void)
{
    (void)pthread_mutex_lock(&g_rsc_node_tree_mutex);
    gint32 ret = _update_rsc_tree_node_from_uri(SERVICE_ROOT_PATH);
    
    debug_log(DLOG_MASS, "total redfish rsc num :%u", g_node_n_nodes(g_rsc_node_tree, G_TRAVERSE_ALL));
    
    (void)pthread_mutex_unlock(&g_rsc_node_tree_mutex);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s failed, ret is %d", __FUNCTION__, ret);

        return;
    }

    return;
}


LOCAL gboolean _free_rsc_node_fn(GNode *node, gpointer data)
{
    return_val_if_fail(NULL != node, FALSE);

    _free_rsc_node_data((RSC_MONITOR_DATA *)node->data);
    g_node_destroy(node);

    
    return FALSE;
}


LOCAL void _delete_rsc_node(GNode *node)
{
    check_fail_return(node);

    
    g_node_traverse(node, G_POST_ORDER, G_TRAVERSE_ALL, -1, _free_rsc_node_fn, NULL);

    return;
}


LOCAL void _check_rsc_node_exist_and_post_event(GNode **node, const gpointer data)
{
    const gchar *rsc_uri = (const gchar *)data;

    
    
    
    if (node == NULL || *node == NULL) {
        return;
    }
    
    RSC_MONITOR_DATA *rsc_data = (RSC_MONITOR_DATA*)(*node)->data;
    if (rsc_data == NULL) {
        return;
    }

    if (rsc_data->last_update_timestamp == 0) {
        return;
    }

    
    gint32 ret = rf_validate_rsc_exist(rsc_uri, NULL);
    if (VOS_OK != ret) {
        
        _delete_rsc_node(*node);
        *node = NULL;

        debug_log(DLOG_DEBUG, "begin to send rsc removed event for %s", rsc_uri);
        ret = dal_rf_rsc_evt_log(RF_RSC_REMOVED_EVT, FALSE, "%s", rsc_uri);
        do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "send resource delete event for %s failed", rsc_uri));
    }

    

    return;
}


LOCAL void rsc_tree_node_traverse(GNode **node, gchar *data, guint16 data_len)
{
    gchar               path_dup[MAX_URI_LENGTH] = {0};
    gint32 ret;

    
    return_if_fail((NULL != node) && (NULL != *node) && (NULL != data) && data_len != 0);

    RSC_MONITOR_DATA *rsc_data = (RSC_MONITOR_DATA *)(*node)->data;

    
    if (0 != g_strcmp0(rsc_data->rsc_node_name, SERVICE_ROOT_PATH)) {
        ret = snprintf_s(path_dup, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s/%s", data, rsc_data->rsc_node_name);
        return_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));
    } else {
        errno_t str_ret = strncpy_s(path_dup, MAX_URI_LENGTH, data, strlen(data));
        return_do_info_if_fail(EOK == str_ret, debug_log(DLOG_ERROR, "copy string %s failed", data));
    }

    if ((*node)->children) {
        _check_rsc_node_exist_and_post_event(node, (const gpointer)path_dup);
        return_if_expr(NULL == *node);

        GNode *child = (*node)->children;
        while (child) {
            GNode *current = child;
            child = current->next;

            
            rsc_tree_node_traverse(&current, path_dup, sizeof(path_dup));
        }
    } else {
        _check_rsc_node_exist_and_post_event(node, (const gpointer)path_dup);
    }
    

    return;
}


LOCAL void _rf_listen_rsc_node_remove_event(void)
{
    
    (void)pthread_mutex_lock(&g_rsc_node_tree_mutex);
    rsc_tree_node_traverse(&g_rsc_node_tree, SERVICE_ROOT_URI, sizeof(SERVICE_ROOT_URI));
    (void)pthread_mutex_unlock(&g_rsc_node_tree_mutex);
    

    return;
}


LOCAL gboolean _check_redfish_event_scan_condition(void)
{
    guchar post_enabled;
    gint32 ret;
    OBJ_HANDLE evt_svc_handle;
    GSList *obj_list = NULL;
    GSList *obj_list_iter = NULL;
    gchar               evt_destination[MAX_EVT_DESTINATION_LEN] = {0};

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &evt_svc_handle);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "get object handle for class %s failed", CLASS_RF_EVT_SVC));

    ret = dal_get_property_value_byte(evt_svc_handle, PROPERTY_RF_EVT_SVC_SERVICE_ENABLED, &post_enabled);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "get property value for %s failed", PROPERTY_RF_EVT_SVC_SERVICE_ENABLED));

    if (0 == post_enabled) {
        debug_log(DLOG_MASS, "event service disabled, stop event scan");

        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE, debug_log(DLOG_ERROR, "get subscription list failed"));

    for (obj_list_iter = obj_list; obj_list_iter != NULL; obj_list_iter = obj_list_iter->next) {
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_list_iter->data, PROPERTY_EVT_SUBSCRIPTION_DES,
            evt_destination, MAX_EVT_DESTINATION_LEN);
        continue_if_expr(VOS_OK != ret);

        if ('\0' != evt_destination[0]) {
            g_slist_free(obj_list);

            return TRUE;
        }
    }

    g_slist_free(obj_list);

    debug_log(DLOG_MASS, "no event subscription, stop event scan");

    return FALSE;
}


LOCAL void _rf_rsc_tree_update_thread(void *data)
{
    gulong last_malloc_tirm_time = 0;
    gulong current_time;
    const gulong malloc_trim_interval = 600;

    
    get_event_shielded_resources_conf();

    (void)prctl(PR_SET_NAME, (uintptr_t) "rfRscUpdate");
    while (1) {
        current_time = vos_get_cur_time_stamp();
        
        
        if (current_time >= (last_malloc_tirm_time + malloc_trim_interval) || current_time < last_malloc_tirm_time) {
            (void)malloc_trim(0);
            last_malloc_tirm_time = current_time;
        }

        
        
        if (FALSE == _check_redfish_event_scan_condition()) {
            (void)pthread_mutex_lock(&g_rsc_node_tree_mutex);
            _delete_rsc_node(g_rsc_node_tree);
            g_rsc_node_tree = NULL;
            (void)pthread_mutex_unlock(&g_rsc_node_tree_mutex);

            g_rsc_initail_update = 1;

            (void)vos_task_delay(RF_EVT_SCAN_CHECK_INTERVAL_IN_SEC * 1000);
            continue;
        }
        

        g_cur_update_timestamp = current_time;

        
        _rf_maintain_rsc_tree();

        g_rsc_initail_update = 0;

        vos_task_delay(RF_RSC_MONITOR_INTERVAL_IN_SEC * 1000);
        
        (void)malloc_trim(0);
    }
}


LOCAL void _rf_rsc_tree_structure_listen_thread(void *data)
{
    (void)prctl(PR_SET_NAME, (uintptr_t) "rfRscTreeStListen");
    while (1) {
        
        
        if (FALSE == _check_redfish_event_scan_condition()) {
            (void)pthread_mutex_lock(&g_rsc_node_tree_mutex);
            _delete_rsc_node(g_rsc_node_tree);
            g_rsc_node_tree = NULL;
            (void)pthread_mutex_unlock(&g_rsc_node_tree_mutex);

            g_rsc_initail_update = 1;

            (void)vos_task_delay(RF_EVT_SCAN_CHECK_INTERVAL_IN_SEC * 1000);

            continue;
        }
        

        
        _rf_listen_rsc_node_remove_event();

        vos_task_delay(RF_RSC_MONITOR_INTERVAL_IN_SEC * 1000);
    }
}


LOCAL gint32 __get_post_event_resource_id(guint32 *event_rsc_id)
{
    OBJ_HANDLE evt_svc_handle;

    gint32 ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &evt_svc_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle for class %s failed", CLASS_RF_EVT_SVC);
        return ret;
    }

    ret = dal_get_property_value_uint32(evt_svc_handle, PROPERTY_RF_EVT_SVC_EVENT_RESOURCE_ID, event_rsc_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get property %s of event service failed", PROPERTY_RF_EVT_SVC_EVENT_RESOURCE_ID);
    }

    return ret;
}


LOCAL gint32 __get_post_event_last_alert_id(guint32 *evt_last_alert_id)
{
    OBJ_HANDLE evt_svc_handle;

    gint32 ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &evt_svc_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle for class %s failed", CLASS_RF_EVT_SVC);
        return ret;
    }

    ret = dal_get_property_value_uint32(evt_svc_handle, PROPERTY_RF_EVT_SVC_LAST_ALERT_ID, evt_last_alert_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get property %s of event service failed", PROPERTY_RF_EVT_SVC_LAST_ALERT_ID);
    }

    return ret;
}

LOCAL gint32  post_retry_forever_evt_handle(OBJ_HANDLE obj_handle, guint32 id,
    json_object *item, gboolean ssl_verify_flag)
{
    EVT_POST_QUEUE_INFO *msg_queue_info = NULL;

    gint32 ret = create_evt_from_template(item, id, obj_handle, &msg_queue_info);
    if (ret != RET_OK || msg_queue_info == NULL) {
        debug_log(DLOG_ERROR, "create_evt_from_template fail, ret=%d", ret);
        return ret;
    }
    guint8 subscription_id;
    ret = get_evt_subscription_id(obj_handle, &subscription_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get event subscription id fail, ret=%d", ret);
        return ret;
    }
    msg_queue_info->ignore_ssl_cert_verify = ssl_verify_flag;

    ret = vos_queue_send(g_forever_queue[subscription_id - 1], msg_queue_info);
    if (ret != RET_OK) {
        if (g_forever_queue[subscription_id - 1] == NULL) {
            debug_log(DLOG_ERROR, "queue is null");
        }
        _free_evt_queue_msg_info(&msg_queue_info);
        debug_log(DLOG_ERROR, "%s: vos_queue_send fail, ret=%d, id=%u", __FUNCTION__, ret, id);
    }
    return ret;
}

LOCAL gint32 send_retry_forever_evt_to_subscription(OBJ_HANDLE description, json_object* events)
{
    json_bool ret_bool;
    json_object *sumj = NULL;
    json_object *eventsj = NULL;

    ret_bool = json_object_object_get_ex(events, "sum", &sumj);
    gint32 sum = json_object_get_int(sumj);
    debug_log(DLOG_INFO, "%s: message: %d", __FUNCTION__, sum);
    if (ret_bool == FALSE) {
        debug_log(DLOG_ERROR, "%s: get event size failed", __FUNCTION__);
        return RET_ERR;
    }
    ret_bool = json_object_object_get_ex(events, "data", &eventsj);
    if (ret_bool == FALSE) {
        debug_log(DLOG_ERROR, "%s: get event list failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object *item;
    json_object *ssl_verify_flag_json = NULL;
    gboolean ssl_verify_flag;
    json_object_array_foreach(eventsj, item) {
        ssl_verify_flag = FALSE;
        json_object_object_get_ex(item, RFPROP_EVT_SSL_CERT_VERIFY, &ssl_verify_flag_json);
        if (ret_bool == FALSE) {
            debug_log(DLOG_ERROR, "get event %s failed", RFPROP_EVT_SSL_CERT_VERIFY);
        } else {
            ssl_verify_flag = (gboolean)json_object_get_boolean(ssl_verify_flag_json);
        }
        json_object_object_del(item, RFPROP_EVT_SSL_CERT_VERIFY);
        guint32 evt_id = increase_and_get_event_rsc_id();
        gint32 ret = post_retry_forever_evt_handle(description, evt_id, item, ssl_verify_flag);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:send message failed", __FUNCTION__);
        }
        vos_task_delay(50); // 防止CPU占用率过高，每次发送等待50ms
    }
    return RET_OK;
}


LOCAL void query_send_retry_forever_evt(void)
{
    OBJ_HANDLE description;
    for (gint32 i = 0; i < MAX_EVT_SUBSCRIPTION_NUM; i++) {
        EVT_TIME_INFO time = {0};
        time.start = 0;
        time.end = vos_get_cur_time_stamp();
        gint32 ret = dal_get_specific_object_byte(CLASS_EVT_SUBSCRIPTION,
            PROPERTY_EVT_SUBSCRIPTION_ID, (guchar)(i + 1), &description);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "not find %s(id=%d), ret=%d", CLASS_EVT_SUBSCRIPTION, i + 1, ret);
            continue;
        }
        json_object* events = get_event_list(time, i + 1, RETRY_TYPE);
        if (events == NULL) {
            debug_log(DLOG_ERROR, "not find any event info");
            continue;
        }
        
        ret = send_retry_forever_evt_to_subscription(description, events);
        json_object_put(events);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "sent event message failed!");
            continue;
        }
        delete_event_log(time, i + 1, RETRY_TYPE);
    }
}


LOCAL void init_retry_forever_event(void * param)
{
    (void)prctl(PR_SET_NAME, (uintptr_t) "retry_forever_init");
    
    gchar task_thread_name[MAX_THREAD_NAME_LEN] = {0};
    POCT_EVENT_THERAD_INFO *thread_info = NULL;

    for (gint32 i = 0; i < MAX_EVT_SUBSCRIPTION_NUM; i++) {
        gint32 ret = vos_queue_create(&g_forever_queue[i]);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "create forever event queue failed, ret=%d", ret);
            continue;
        }
        (void)snprintf_s(task_thread_name, MAX_THREAD_NAME_LEN, MAX_THREAD_NAME_LEN - 1, "retry_forever_evt_%d",
            i + 1);
        debug_log(DLOG_DEBUG, "task thread name for %d is %s", i + 1, task_thread_name);
        thread_info = g_malloc(sizeof(POCT_EVENT_THERAD_INFO));
        if (thread_info == NULL) {
            (void)vos_queue_delete(&g_forever_queue[i]);
            debug_log(DLOG_ERROR, "%s: malloc fail", __FUNCTION__);
            continue;
        }
        thread_info->queue = g_forever_queue[i];
        thread_info->type = EVENT_ORIGIN_TYPE_RETRY_FOREVER;
        ret = vos_task_create(NULL, task_thread_name, (TASK_ENTRY)_post_redfish_evt_thread,
            (void *)(thread_info), DEFAULT_PRIORITY);
        if (ret != VOS_OK) {
            (void)vos_queue_delete(&g_forever_queue[i]);
            g_free(thread_info);
            debug_log(DLOG_ERROR, "create %s failed, ret is %d", task_thread_name, ret);
            continue;
        }
    }
    
    vos_task_delay(REDFISH_DELAY_1000_MSEC * 30);
    query_send_retry_forever_evt();
}

gint32 redfish_event_init(void)
{
    gint32 i;
    gulong pulTaskID;
    gchar task_thread_name[MAX_THREAD_NAME_LEN] = {0}; 
    guint32 evt_id = 0;
    guint32 last_alert_id = 0;

    gint32 ret = __get_post_event_resource_id(&evt_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "initailize event resource id failed, ret = %d", ret);
    }
    g_event_rsc_id = evt_id;

    ret = __get_post_event_last_alert_id(&last_alert_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "initailize event last alert id failed, ret = %d", ret);
    }
    g_last_alert_id = last_alert_id;
    POCT_EVENT_THERAD_INFO *thread_info = NULL;
    (void)prctl(PR_SET_NAME, (uintptr_t) "rfEvtThread");
    
    for (i = 0; i < MAX_EVT_SUBSCRIPTION_NUM; i++) {
        ret = vos_queue_create(&g_evt_msg_queue[i]);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "create event message queue failed, ret is %d", ret));

        (void)memset_s(task_thread_name, MAX_THREAD_NAME_LEN, 0, MAX_THREAD_NAME_LEN);
        (void)snprintf_s(task_thread_name, MAX_THREAD_NAME_LEN, MAX_THREAD_NAME_LEN - 1, "rf_evt_queue_thread_%d",
            i + 1);
        debug_log(DLOG_DEBUG, "task thread name for %d is %s", i + 1, task_thread_name);
        thread_info = g_malloc(sizeof(POCT_EVENT_THERAD_INFO));
        if (thread_info == NULL) {
            (void)vos_queue_delete(&g_evt_msg_queue[i]);
            debug_log(DLOG_ERROR, "%s: malloc fail", __FUNCTION__);
            continue;
        }
        thread_info->queue = g_evt_msg_queue[i];
        thread_info->type = EVENT_ORIGIN_TYPE_CURRENT;
        ret = vos_task_create(&pulTaskID, task_thread_name, (TASK_ENTRY)_post_redfish_evt_thread,
            (void *)thread_info, DEFAULT_PRIORITY);
        if (ret != VOS_OK) {
            (void)vos_queue_delete(&g_evt_msg_queue[i]);
            g_free(thread_info);
            debug_log(DLOG_ERROR, "create %s failed, ret is %d", task_thread_name, ret);
            continue;
        }
    }
    ret = vos_task_create(&pulTaskID, "init_retry_forever_event_thread", (TASK_ENTRY)init_retry_forever_event,
        NULL, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "create init_retry_forever_event_thread failed, ret is %d", ret);
    }
    return VOS_OK;
}



gint32 redfish_event_start(void)
{
    gulong pulTaskID;

    
    gint32 ret = vos_task_create(&pulTaskID, "rsc tree update thread", (TASK_ENTRY)_rf_rsc_tree_update_thread, NULL,
        DEFAULT_PRIORITY);
    return_val_do_info_if_fail(VOS_OK == ret, RET_ERR,
        debug_log(DLOG_ERROR, "create redfish resource monitor thread failed"));

    
    ret = vos_task_create(&pulTaskID, "tree structure listen thread", (TASK_ENTRY)_rf_rsc_tree_structure_listen_thread,
        NULL, DEFAULT_PRIORITY);
    return_val_do_info_if_fail(VOS_OK == ret, RET_ERR,
        debug_log(DLOG_ERROR, "create redfish tree structure listen thread failed, ret is %d", ret));

    ret = delete_simulate_alert_event_record();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: delete simulatealert event failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gboolean _is_event_destination_element_match(OBJ_HANDLE obj_handle, json_object *evt_msg_jso,
    const gchar *rf_prop_name, const gchar *obj_prop_name, gboolean case_insensitive)
{
    const gchar **str_value_arr = NULL;
    GVariant *arr_value = NULL;
    gint32 ret;
    const gchar *value_iter = NULL;
    const gchar *prop_value = NULL;
    guint i;
    gboolean b_ret;
    
    gsize array_len = 0;
    
    
    GCompareFunc compare_func;
    gchar uri_buf[MAX_URI_LENGTH] = {0};
    errno_t str_ret = EOK;
    json_object *origin_of_condition_jso = NULL;
    

    return_val_do_info_if_fail(evt_msg_jso != NULL && rf_prop_name != NULL && obj_prop_name != NULL, FALSE,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    (void)get_element_str(evt_msg_jso, rf_prop_name, &prop_value);
    
    if (g_strcmp0(rf_prop_name, RFPROP_ORIGIN_RSC) == 0) {
        (void)json_object_object_get_ex(evt_msg_jso, rf_prop_name, &origin_of_condition_jso);
        (void)get_element_str(origin_of_condition_jso, RFPROP_ODATA_ID, &prop_value);
    }
    

    ret = dfl_get_property_value(obj_handle, obj_prop_name, &arr_value);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_ERROR, "get property value for %s failed", obj_prop_name));

    
    str_value_arr = g_variant_get_strv(arr_value, &array_len);
    
    if (str_value_arr == NULL) {
        debug_log(DLOG_ERROR, "get value array for %s failed", obj_prop_name);

        g_variant_unref(arr_value);

        return FALSE;
    }

    
    
    if (array_len == 1 && g_strcmp0(RF_STR_EMPTY, str_value_arr[0]) == 0) {
        b_ret = TRUE;
    } else {
        
        b_ret = FALSE;
        for (i = 0; (str_value_arr[i] != NULL) && (prop_value != NULL); i++) {
            
            if (g_strcmp0(rf_prop_name, RFPROP_ORIGIN_RSC) == 0) {
                str_ret = strncpy_s(uri_buf, MAX_URI_LENGTH, str_value_arr[i], strlen(str_value_arr[i]));
                return_val_do_info_if_fail(EOK == str_ret, FALSE,
                    debug_log(DLOG_ERROR, "copy string %s failed", str_value_arr[i]);
                    g_free(str_value_arr); g_variant_unref(arr_value));

                rf_trim_https_uri(uri_buf, sizeof(uri_buf));
                value_iter = uri_buf;
            } else {
                value_iter = str_value_arr[i];
            }

            debug_log(DLOG_DEBUG, "compare info :%s and %s", prop_value, value_iter);

            
            compare_func = (GCompareFunc)((TRUE == case_insensitive) ? g_ascii_strcasecmp : g_strcmp0);
            if (compare_func((gconstpointer)prop_value, (gconstpointer)value_iter) == 0) {
                b_ret = TRUE;
                break;
            }
            
        }
        
    }
    

    g_free(str_value_arr);
    g_variant_unref(arr_value);

    return b_ret;
}


LOCAL EVT_POST_QUEUE_INFO *_new_evt_post_queue_info(json_object *evt_jso, OBJ_HANDLE obj_handle)
{
    return_val_do_info_if_fail(NULL != evt_jso, NULL, debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    EVT_POST_QUEUE_INFO *new_info = (EVT_POST_QUEUE_INFO *)g_malloc0(sizeof(EVT_POST_QUEUE_INFO));
    return_val_do_info_if_fail((NULL != new_info), NULL, debug_log(DLOG_ERROR, "malloc thread info failed"));

    new_info->evt_jso = evt_jso;
    new_info->obj_handle = obj_handle;

    return new_info;
}


LOCAL void _free_evt_queue_msg_info(EVT_POST_QUEUE_INFO **queue_msg_info)
{
    return_do_info_if_fail((NULL != queue_msg_info) && (NULL != *queue_msg_info),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    EVT_POST_QUEUE_INFO *info = *queue_msg_info;

    (void)json_object_put(info->evt_jso);

    g_free(info);

    *queue_msg_info = NULL;

    return;
}


LOCAL void _add_server_header_to_subscription_headers(json_object *subscription_headers,
    const gchar *server_header_title, const gchar *server_header_content)
{
    int iRet = -1;
    const gchar *origin_header_content = NULL;
    guint32 size;
    gchar *new_header_content = NULL;

    (void)get_element_str(subscription_headers, server_header_title, &origin_header_content);

    
    if (origin_header_content == NULL) {
        json_object_object_add(subscription_headers, server_header_title,
            json_object_new_string(server_header_content));
        
    } else if (g_ascii_strcasecmp(origin_header_content, server_header_content) == 0) {
        return;
        
    } else {
        
        size = strlen(origin_header_content) + strlen(server_header_content) + 16;
        new_header_content = (gchar *)g_malloc0(size);
        return_do_info_if_fail((new_header_content != NULL), debug_log(DLOG_ERROR, "malloc new header content failed"));

        iRet = snprintf_s(new_header_content, size, size - 1, "%s, %s", server_header_content, origin_header_content);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        json_object_object_add(subscription_headers, server_header_title,
            json_object_new_string((const gchar *)new_header_content));
        g_free(new_header_content);
    }

    return;
}

LOCAL void fit_bma_event_report_addr_ipv4(gchar *req_url, guint32 url_len)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint8 bob_channel_type = 0;
    gchar   tmp_req_uri[MAX_CURL_REQ_URI_LEN] = {0};
    guint16 agent_trap_port = 0; // 本地localhost 上的端口号，代替bma上的veth上的端口号
    gchar *tmp_url_ptr = NULL;
    gchar *tmp_url_body_ptr = NULL;

    if (req_url == NULL) {
        debug_log(DLOG_ERROR, "%s: req_url == null\n", __FUNCTION__);
        return;
    }

    tmp_url_ptr = strstr(req_url, "127.0.0.1");
    
    if (tmp_url_ptr == NULL) {
        return;
    }

    (void)dal_get_object_handle_nth(CLASS_SMS, 0, &object_handle);
    (void)dal_get_property_value_byte(object_handle, PROPERTY_SMS_BOB_CHANNEL_TYPE, &bob_channel_type);
    if (bob_channel_type == PCIE_VETH) {
        return;
    }

    // 如果当前是字符设备模式，需要将目的地址改成localhost，经locathost转发给bma
    ret = dal_get_object_handle_nth(CLASS_LOCAL_HOST_AGENT_PORT, 0, &object_handle);
    return_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR,
        "%s: dal_get_object_handle_nth CLASS_LOCAL_HOST_AGENT_PORT fail,ret:%d\n", __FUNCTION__, ret));
    ret = dal_get_property_value_uint16(object_handle, PROPERTY_LOCAL_HTTPS_AGENT_PORT, &agent_trap_port);
    return_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR,
        "%s: dal_get_object_handle_nth PROPERTY_LOCAL_HTTPS_AGENT_PORT fail,ret:%d\n", __FUNCTION__, ret));
    // 将url格式为https://127.0.0.1:8090/**** ,替换成https://::1:30004/****

    ret = memcpy_s(tmp_req_uri, MAX_CURL_REQ_URI_LEN, req_url, tmp_url_ptr - req_url);
    do_val_if_expr(ret != EOK, debug_log(DLOG_ERROR, "%s: str_ip_addr memcpy_s fail, ret = %d\n", __FUNCTION__, ret));
    tmp_url_body_ptr = strstr(tmp_url_ptr, "/"); // 查找端口号之后的第一个'/'
    if (tmp_url_body_ptr == NULL) {
        return;
    }

    ret = snprintf_s(tmp_req_uri + (tmp_url_ptr - req_url), MAX_CURL_REQ_URI_LEN - (tmp_url_ptr - req_url),
        MAX_CURL_REQ_URI_LEN - 1 - (tmp_url_ptr - req_url), "[::1]:%u%s", agent_trap_port, tmp_url_body_ptr);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: str_ip_addr snprintf_s fail, ret = %d\n", __FUNCTION__, ret));
    (void)memset_s(req_url, url_len, 0, url_len);
    ret = strcpy_s(req_url, url_len, tmp_req_uri);
    do_val_if_expr(ret != EOK, debug_log(DLOG_ERROR, "%s: strcpy_s fail, ret = %d\n", __FUNCTION__, ret));

    debug_log(DLOG_DEBUG, "%s: fit url:%s\n", __FUNCTION__, req_url);
}


LOCAL void fit_report_uri_for_bma(gchar *req_uri, guint32 buffer_len)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar bma_ip[MAX_IPV6_LEN] = {0};
    gchar new_uri[MAX_CURL_REQ_URI_LEN] = {0};
    gchar *ip_start = NULL;
    gchar *veth_flag = NULL;
    guint8 ip_type = 0;

    if (req_uri == NULL) {
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    if (ret != VOS_OK) {
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SMS_IPADDR, bma_ip, sizeof(bma_ip));
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property value failed, ret:%d", __FUNCTION__, ret);
        return;
    }

    
    ret = dal_get_addr_type((const gchar *)bma_ip, &ip_type);
    if (ret != VOS_OK || ip_type != HOST_ADDR_TYPE_IPV6) {
        return fit_bma_event_report_addr_ipv4(req_uri, buffer_len);
    }

    ip_start = strstr(req_uri, (const gchar *)bma_ip);
    
    if (ip_start == NULL || ((ip_start > req_uri) && (*(ip_start - 1) == '['))) {
        return fit_bma_event_report_addr_ipv4(req_uri, buffer_len);
    }

    
    veth_flag = strstr(ip_start, "%veth");
    if (veth_flag == NULL) {
        return;
    }
    veth_flag = veth_flag + strlen("%veth");
    
    if (strncpy_s(new_uri, sizeof(new_uri), req_uri, ip_start - req_uri) != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s failed", __FUNCTION__);
        return;
    }

    if (strncat_s(new_uri, sizeof(new_uri), "[", 1) != EOK ||
        strncat_s(new_uri, sizeof(new_uri), bma_ip, strlen(bma_ip)) != EOK ||
        strncat_s(new_uri, sizeof(new_uri), "%veth", strlen("%veth")) != EOK ||
        strncat_s(new_uri, sizeof(new_uri), "]", 1) != EOK ||
        strncat_s(new_uri, sizeof(new_uri), veth_flag, strlen(veth_flag)) != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s failed", __FUNCTION__);
        return;
    }

    if (strncpy_s(req_uri, buffer_len, new_uri, strlen(new_uri)) != EOK) {
        debug_log(DLOG_ERROR, "%s: vos_strncpy_s failed", __FUNCTION__);
    }
}


LOCAL gint32 check_ethgroup_ip_valid(OBJ_HANDLE obj_handle)
{
    gchar ipv4[MAX_IPV4_LEN] = {0};
    gchar ipv6[MAX_IPV6_LEN] = {0};

    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, ipv4, sizeof(ipv4));
    if (vos_ipv4_addr_valid_check((const guchar *)ipv4) == RET_OK) {
        debug_log(DLOG_INFO, "%s: ipv4=%s is valid", dfl_get_object_name(obj_handle), ipv4);
        return RET_OK;
    }

    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, ipv6, sizeof(ipv6));
    if (vos_ipv6_addr_valid_check((const guchar *)ipv6) == RET_OK) {
        debug_log(DLOG_INFO, "%s: ipv6=%s is valid", dfl_get_object_name(obj_handle), ipv6);
        return RET_OK;
    }

    debug_log(DLOG_INFO, "%s: ipv4=%s and ipv6=%s are invalid", dfl_get_object_name(obj_handle), ipv4, ipv6);
    return RET_ERR;
}


LOCAL gint32 check_inner_om_ip_valid(void)
{
#define INNER_OM_VLAN_ID    4092
    gint32 result = RET_ERR;
    GSList* eth_obj_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint16 vlan_id = 0;

    gint32 ret = dfl_get_object_list(ETH_CLASS_NAME_ETHGROUP, &eth_obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: get %s object list fail(%d)", __FUNCTION__, ETH_CLASS_NAME_ETHGROUP, ret);
        return result;
    }
    
    for (GSList *node = eth_obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)node->data;
        (void)dal_get_property_value_uint16(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_ID, &vlan_id);
        if (vlan_id != INNER_OM_VLAN_ID) {
            continue;
        }

        if (check_ethgroup_ip_valid(obj_handle) == RET_OK) {
            result = RET_OK;
            break;
        }
    }
    g_slist_free(eth_obj_list);

    return result;
}


LOCAL gboolean redfish_check_event_send_from_float_ip(void)
{
    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        if (check_inner_om_ip_valid() == RET_OK) {
            debug_log(DLOG_INFO, "send_from_float_flag is inner OM.");
            return FALSE;
        }
    }

    debug_log(DLOG_INFO, "send_from_float_flag is outer OM.");
    return TRUE;
}


LOCAL gint32 redfish_get_ip_from_uri(gchar *req_uri, guint32 uri_len, gchar *ipaddr_str, guint32 ip_len)
{
    errno_t safe_fun_ret;
    URL_STRUCT_INFO_S url_info;

    if (req_uri == NULL || ipaddr_str == NULL || uri_len > MAX_CURL_REQ_URI_LEN || ip_len > HOST_ADDR_MAX_LEN + 1) {
        debug_log(DLOG_ERROR, "wrong param, req_uri is NULL or uri_len = %d or ipaddr_str is NULL or ip_len = %d",
            uri_len, ip_len);
        return RET_ERR;
    }

    // 从url中解析出ip字段
    (void)memset_s(&url_info, sizeof(url_info), 0, sizeof(url_info));
    gint32 ret = dal_parser_string_from_url(req_uri, uri_len, &url_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "parser ip string from url fail, ret = %d", ret);
        return ret;
    }
    
    // 如果ip字段有[，需要去掉[] 
    if ((url_info.serverip[0] == '[') && (strlen(url_info.serverip) > strlen("[]"))) {
        safe_fun_ret = strncpy_s(ipaddr_str, ip_len, &url_info.serverip[1], strlen(url_info.serverip) - strlen("[]"));
    } else {
        safe_fun_ret = strcpy_s(ipaddr_str, ip_len, &url_info.serverip[0]);
    }

    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "strncpy_s fail, ret = %d", safe_fun_ret);
        return RET_ERR;
    }
    
    debug_log(DLOG_INFO, "the uri ipaddr : %s", ipaddr_str); 
    return RET_OK;
}


LOCAL gint32 redfish_get_ip_addr(gchar *ip_addr, guchar len, guchar addr_type)
{
    OBJ_HANDLE ethgroup_handle = 0;
    gchar float_ip[MAX_IPV6_LEN] = {0};

    if ((ip_addr == NULL) || (len > MAX_IPV6_LEN)) {
        debug_log(DLOG_ERROR, "wrong param, eth_name is NULL or eth_name len = %d", len);
        return RET_ERR;
    }
    
    gint32 ret = dfl_get_object_handle(FLOAT_ETHGROUP_OBJ_NAME, &ethgroup_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get object %s failed", FLOAT_ETHGROUP_OBJ_NAME);
        return RET_ERR;
    }  

    if (addr_type == HOST_ADDR_TYPE_IPV4) {
        (void)dal_get_property_value_string(ethgroup_handle, ETH_GROUP_ATTRIBUTE_IP_ADDR, float_ip, MAX_IPV4_LEN);
    } else {
        (void)dal_get_property_value_string(ethgroup_handle, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, float_ip, MAX_IPV6_LEN);
    }
    // 如果IP无效，或者双平面场景下使用内网上报时，则告警中不指定浮动IP上报，直接返回
    if ((strlen(float_ip) == 0) || (g_strcmp0(float_ip, "0.0.0.0") == 0) ||
        (redfish_check_event_send_from_float_ip() == FALSE)) {
        debug_log(DLOG_INFO, "strlen(float_ip) = %zu ,float_ip = %s not use float ip", strlen(float_ip), float_ip);
        return RET_OK;
    }
    debug_log(DLOG_INFO, "get object addr_type=%u float_ip = %s", addr_type, float_ip);
    ret = strcpy_s(ip_addr, len, float_ip);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, float_ip=%s ret(%d).", __FUNCTION__, float_ip, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_out_port_ip(gchar *req_uri, guint32 uri_len, gchar *ip_addr, guchar len)
{
    gchar ipaddr_str[HOST_ADDR_MAX_LEN + 1] = { 0 };
    guchar addr_type_tmp = HOST_ADDR_TYPE_DOMAIN;
    gint32 ret = redfish_get_ip_from_uri(req_uri, uri_len, ipaddr_str, sizeof(ipaddr_str));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "redfish_get_ip_from_uri err ret = %d", ret);
        return RET_OK;
    }
    
    // 获取当前订阅服务器使用的地址类型
    (void)dal_get_addr_type(ipaddr_str, &addr_type_tmp);
 
    OBJ_HANDLE group_obj = 0;
    gchar cur_ip[TMP_STR_LEN + 1] = {0};
    (void)dfl_get_object_handle(OUT_ETHGROUP_OBJ_NAME, &group_obj);
 
    if (addr_type_tmp == HOST_ADDR_TYPE_IPV4) {
        (void)dal_get_property_value_string(group_obj, PROPERTY_DEVICE_INFO_IP_ADDR, cur_ip, sizeof(cur_ip));
    } else if (addr_type_tmp == HOST_ADDR_TYPE_IPV6) {
        (void)dal_get_property_value_string(group_obj, PROPERTY_DEVICE_INFO_IPV6_ADDR, cur_ip, sizeof(cur_ip));
    } else {
        debug_log(DLOG_ERROR, "%s: unknow address type %d", __FUNCTION__, addr_type_tmp);
        return RET_OK;
    }
 
    // 管理网口IP地址未配置时，不指定IP地址进行上报
    ret = dal_check_net_addr_valid(cur_ip, NULL);
    if (ret != NET_ADDR_OK) {
        return RET_OK;
    }
 
    ret = strcpy_s(ip_addr, len, cur_ip);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s fail, cur_ip=%s ret(%d).", __FUNCTION__, cur_ip, ret);
    }
 
    return RET_OK;
}


LOCAL gint32 redfish_get_event_out_going_ip(gchar *ip_addr, guchar len, gchar *req_uri, guint32 uri_len)
{
    guint8 as_status = 0;
    gchar ipaddr_str[HOST_ADDR_MAX_LEN + 1] = { 0 };
    guchar addr_type_tmp = HOST_ADDR_TYPE_DOMAIN;

    if ((ip_addr == NULL) || (len > MAX_IPV6_LEN) || (req_uri == NULL) || uri_len > MAX_CURL_REQ_URI_LEN) {
        debug_log(DLOG_ERROR, "wrong param, eth_name is NULL or eth_name len = %d or req_uri is NULL or uri_len = %d", 
            len, uri_len);
        return RET_ERR;
    }

    if (dal_check_if_mm_board() == FALSE) {
        return get_out_port_ip(req_uri, uri_len, ip_addr, len);
    }

    (void)get_board_active_state(&as_status);
    if (as_status != ACTIVE_STATE) {
        debug_log(DLOG_DEBUG, "not active state don't send http msg");
        return RET_ERR;
    }

    // 提取uri中的ip地址
    gint32 ret = redfish_get_ip_from_uri(req_uri, uri_len, ipaddr_str, sizeof(ipaddr_str));
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "redfish_get_ip_from_uri err ret = %d", ret); 
        //  获取ip失败，返回OK，还是要继续重试发送
        return RET_OK;
    }

    
    (void)dal_get_addr_type(ipaddr_str, &addr_type_tmp);
    if ((addr_type_tmp == HOST_ADDR_TYPE_IPV4) || (addr_type_tmp == HOST_ADDR_TYPE_IPV6)) {
        return redfish_get_ip_addr(ip_addr, len, addr_type_tmp);
    }

    return RET_OK;
}


LOCAL gint32 get_subscription_info(EVT_POST_QUEUE_INFO *msg_info, SUBSCRIPTION_IFNO *info)
{
    GVariant *evt_http_headers_var = NULL;
    gchar *evt_http_headers_str = NULL;
    gint32 ret;
    OBJ_HANDLE evt_svc_handle = 0;
    gchar req_uri[MAX_CURL_REQ_URI_LEN] = {0};

    (void)dal_get_property_value_byte(msg_info->obj_handle, PROPERTY_EVT_SUBSCRIPTION_ID, &(info->id));
    
    if (info->id < RF_EVT_SUBSCRIPTION_ID_MIN || info->id > RF_EVT_SUBSCRIPTION_ID_MAX) {
        debug_log(DLOG_ERROR, "%s: subscription id(%u) is invalid", __FUNCTION__, info->id);
        return RET_ERR;
    }

    info->subscriber_type = RF_EVT_SUBSCRIBER_OTHER;
    (void)dal_get_property_value_byte(msg_info->obj_handle, PROPERTY_EVT_SUBSCRIPTION_TYPE, &(info->subscriber_type));

    ret = dfl_get_property_value(msg_info->obj_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS,
        &evt_http_headers_var);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get property value for [%s:%s], ret is %d",
            dfl_get_object_name(msg_info->obj_handle), PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, ret);
        return RET_ERR;
    }

    (void)rf_get_property_plain_data(evt_http_headers_var, &evt_http_headers_str);
    g_variant_unref(evt_http_headers_var);

    (void)dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &evt_svc_handle);
    (void)dal_get_property_value_int32(evt_svc_handle, PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES, &(info->max_retry_time));
    (void)dal_get_property_value_int32(evt_svc_handle, PROPERTY_RF_EVT_SVC_EVENT_DELAY_INSEC, &(info->delay_sec));

    
    if (evt_http_headers_str != NULL) {
        info->headers_jso = json_tokener_parse(evt_http_headers_str);
        clear_sensitive_info(evt_http_headers_str, strlen(evt_http_headers_str));
        g_free(evt_http_headers_str);
    }

    
    if (info->headers_jso == NULL) {
        info->headers_jso = json_object_new_object();
    }

    if (info->headers_jso == NULL || json_object_get_type(info->headers_jso) != json_type_object) {
        debug_log(DLOG_ERROR, "%s:cann't get http headers ", __FUNCTION__);
        (void)json_object_put(info->headers_jso);
        return RET_ERR;
    }
    
    (void)dal_get_property_value_string(msg_info->obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, req_uri,
        MAX_CURL_REQ_URI_LEN);
    ret = redfish_get_event_out_going_ip(info->ip_addr, sizeof(info->ip_addr), req_uri, MAX_CURL_REQ_URI_LEN);
    if (ret != RET_OK) {
        json_object_put(info->headers_jso);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 _get_event_type(json_object *jso, gchar *event_type, guint32 evevt_type_len)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    ret = dal_get_object_handle_nth(CLASS_SYSTEM_MANAGER_INFO, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: dal_get_object_handle_nth failde", __FUNCTION__);
        return RET_ERR;
    }

    
    gchar manager_ip[MAX_PROP_VAL_LEN + 1] = {0};
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SYSTEM_MANAGER_INFO_IP, manager_ip, sizeof(manager_ip));
    if (ret != DFL_OK || strlen(manager_ip) == 0) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string failed or manager_ip is null", __FUNCTION__);
        return RET_ERR;
    }

    json_object *events_jso = NULL;
    json_bool b_ret = json_object_object_get_ex(jso, RFPROP_EVENTS, &events_jso);
    if (!b_ret) {
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFPROP_EVENTS);
        return RET_ERR;
    }

    json_object *event_jso = NULL;
    event_jso = json_object_array_get_idx(events_jso, 0); 
    if (event_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Missing event jso", __FUNCTION__);
        return RET_ERR;
    }

    const gchar *event_type_content = NULL;
    (void)get_element_str(event_jso, RFPROP_EVT_TYPE, &event_type_content);
    if (event_type_content == NULL) {
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFPROP_EVT_TYPE);
        return RET_ERR;
    }

    ret = strcpy_s(event_type, evevt_type_len, event_type_content);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret = %d", __FUNCTION__, ret);
    }

    return RET_OK;
}

LOCAL gint32 __get_event_type_from_jso(json_object *event_jso, gchar *event_type, guint32 event_type_len)
{
    gint32 ret;

    const gchar *event_type_content = NULL;
    (void)get_element_str(event_jso, RFPROP_EVT_TYPE, &event_type_content);
    if (event_type_content == NULL) {
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFPROP_EVT_TYPE);
        return RET_ERR;
    }

    ret = strcpy_s(event_type, event_type_len, event_type_content);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret = %d", __FUNCTION__, ret);
    }

    return RET_OK;
}

LOCAL gint32 __update_event_rsc_id_info(OBJ_HANDLE obj_handle, json_object *evt_msg_jso, guint32 *last_alert_id)
{
    gint32 ret;

    gchar event_type[MAX_EVT_TYPE_LEN] = {0};
    ret = __get_event_type_from_jso(evt_msg_jso, event_type, sizeof(event_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get alert event last rsc id failed");
        return RET_ERR;
    }

    
    (void)pthread_mutex_lock(&g_event_rsc_id_mutex);
    g_event_rsc_id++;
    if (0 == g_event_rsc_id) {
        g_event_rsc_id = 1;
    }
    *last_alert_id = g_last_alert_id;
    if (g_strcmp0(event_type, RF_ALERT_EVT_STR) == 0) {
        g_last_alert_id = g_event_rsc_id;
    }
    (void)pthread_mutex_unlock(&g_event_rsc_id_mutex);

    ret = dal_save_property_value_uint32_remote(obj_handle, PROPERTY_RF_EVT_SVC_EVENT_RESOURCE_ID,
        g_event_rsc_id, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "save redfish event rsc id failed, ret = %d", ret);
        return RET_ERR;
    }

    ret = dal_save_property_value_uint32_remote(obj_handle, PROPERTY_RF_EVT_SVC_LAST_ALERT_ID,
        g_last_alert_id, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "save redfish alert event last rsc id failed, ret = %d", ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL CURL_REQ_INFO* generate_curl_req_info(EVT_POST_QUEUE_INFO *queue_msg_info, SUBSCRIPTION_IFNO *info)
{
    CURL_REQ_INFO *req_info = NULL;
    gint32 ret;
    gchar cur_header_content[MAX_EVT_HEADERS_SIZE] = {0};

    req_info = (CURL_REQ_INFO *)g_malloc0(sizeof(CURL_REQ_INFO));
    if (req_info == NULL) {
        debug_log(DLOG_ERROR, "%s: malloc failed for curl request info", __FUNCTION__);
        return NULL;
    }

    req_info->method_name = HTTP_POST;
    (void)dal_get_property_value_string(queue_msg_info->obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, req_info->req_uri,
        MAX_EVT_DESTINATION_LEN);
    fit_report_uri_for_bma(req_info->req_uri, sizeof(req_info->req_uri));

    req_info->req_body = g_strdup(dal_json_object_get_str(queue_msg_info->evt_jso));
    if (req_info->req_body == NULL) {
        debug_log(DLOG_ERROR, "dup string failed");
        g_free(req_info);
        return NULL;
    }

    
    _add_server_header_to_subscription_headers(info->headers_jso, HEADER_CONTENT_TYPE, CONTENT_TYPE_JSON);

    
    gchar event_type[MAX_EVT_TYPE_LEN] = {0};
    if (_get_event_type(queue_msg_info->evt_jso, event_type, sizeof(event_type)) == RET_OK) {
        _add_server_header_to_subscription_headers(info->headers_jso, RFPROP_EVT_TYPE, event_type);
    }
    
    
    // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(info->headers_jso, header_title, header_val_jso)
    {
        // require GCC 4.6
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
        ret = snprintf_s(cur_header_content, MAX_EVT_HEADERS_SIZE, MAX_EVT_HEADERS_SIZE - 1, "%s:%s", header_title,
            dal_json_object_get_str(header_val_jso));
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }

        req_info->request_headers = curl_slist_append(req_info->request_headers, (const gchar *)cur_header_content);
        
        (void)memset_s(cur_header_content, MAX_EVT_HEADERS_SIZE, 0, MAX_EVT_HEADERS_SIZE);
    }

    return req_info;
}

LOCAL json_object* format_event_json_info(const json_object* evt_jso, gboolean ignore_ssl_cert_verify)
{
    json_object *ejs_arr = NULL;
    json_bool ret = json_object_object_get_ex(evt_jso, RFPROP_EVENTS, &ejs_arr);
    if (!ret) {
        debug_log(DLOG_ERROR, "%s, get Events filed failed", __FUNCTION__);
        return NULL;
    }
    json_object *event = json_object_array_get_idx(ejs_arr, 0);
    if (event == NULL) {
        debug_log(DLOG_ERROR, "get event_json from description_json fail");
        return event;
    }
    (void)json_object_object_add(event, RFPROP_EVT_SSL_CERT_VERIFY, json_object_new_boolean(ignore_ssl_cert_verify));
    return event;
}


LOCAL gint32 get_event_create_timestamp(json_object* event, guint32 *datetime)
{
    json_object *time_json = NULL;
    json_bool retb = json_object_object_get_ex(event, RFPROP_EVT_TIMESTAMP, &time_json);
    if (!retb) {
        debug_log(DLOG_ERROR, "%s, get Timestamp filed failed", __FUNCTION__);
        return RET_ERR;
    }
    const gchar *time_str = json_object_get_string(time_json);
    gint32 ret = dal_parse_redfish_datetime_to_stamp(time_str, (gulong *)datetime);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get Timestamp second failed, ret=%d", __FUNCTION__, ret);
    }
    return ret;
}

LOCAL void save_send_error_event_log(json_object* description_json, guint8 evt_subscription_id,
    gint32 type, gboolean ignore_ssl_cert_verify)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    json_object *ejs = format_event_json_info(description_json, ignore_ssl_cert_verify);
    if (ejs == NULL) {
        debug_log(DLOG_ERROR, "get events failed");
        return;
    }
    guint32 datetime;
    gint32 ret = get_event_create_timestamp(ejs, &datetime);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get Timestamp second failed, ret=%d", __FUNCTION__, ret);
        return;
    }
    const gchar *evt_info = json_object_to_json_string(ejs);
    input_list = g_slist_append(input_list, g_variant_new_byte(evt_subscription_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(datetime));
    input_list = g_slist_append(input_list, g_variant_new_string(evt_info));
    input_list = g_slist_append(input_list, g_variant_new_int32(type));

    ret = dal_get_object_handle_nth(CLASS_SEL, 0, &obj_handle);
    if (ret != RET_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s: get Sel obj failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    ret = dfl_call_class_method(obj_handle, METHOD_SAVE_EVENT_INFO, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s fail, ret=%d", __FUNCTION__, METHOD_SAVE_EVENT_INFO, ret);
    }
}


LOCAL gboolean _handle_redfish_evt_msg_queue(EVT_POST_QUEUE_INFO *queue_msg_info, EVT_ORIGIN_TYPE origin_type)
{
    CURL_REQ_INFO *req_info = NULL;
    CURL_RESPONSE_INFO *resp_info = NULL;
    CURL_OPT_INFO option;
    SUBSCRIPTION_IFNO info;
    gint32 ret;

    (void)memset_s(&info, sizeof(info), 0, sizeof(info));
    if (get_subscription_info(queue_msg_info, &info) != RET_OK) {
        return FALSE;
    }

    req_info = generate_curl_req_info(queue_msg_info, &info);
    json_object_put(info.headers_jso);
    if (req_info == NULL) {
        return FALSE;
    }

    option.src_ip = (strlen(info.ip_addr) == 0) ? NULL : (const gchar*)info.ip_addr;
    
    option.verify_peer_ssl =
        (info.subscriber_type != RF_EVT_SUBSCRIBER_BMA && queue_msg_info->ignore_ssl_cert_verify == FALSE);
    option.certs = g_evt_subscriber_certs[info.id - 1];
    option.certs_cnt = RF_EVT_SUBSCRIBER_ROOTCA_COUNT;
    gint32 i = 0;
    gboolean result = FALSE;
    while (handle_evt_policy(queue_msg_info, i, origin_type) == RESUBMIT) {
        resp_info = NULL;
        ret = dal_send_http_msg_with_multi_certs(req_info, &resp_info, &option);
        record_evt_report_resp_info(info.id, (ret == VOS_OK) ? resp_info->response_code : ret);
        
        if ((ret == VOS_OK) && ((resp_info->response_code / HTTP_OK) == 1)) {
            debug_log(DLOG_DEBUG, "send redfish event message successfully, ret=%d, http status=%ld",
                ret, resp_info->response_code);
            dal_free_curl_response_info(&resp_info);
            result = TRUE;
            break;
        }
        debug_log(DLOG_DEBUG, "send redfish event message to channal %d,"
          "address %s failed(ret:%d, respCode:%ld), retry time:%d",
            info.id, req_info->req_uri, ret, (ret == VOS_OK ? resp_info->response_code : ret), i + 1);
        dal_free_curl_response_info(&resp_info);
        vos_task_delay(REDFISH_DELAY_1000_MSEC * info.delay_sec);
        i++;
    }
    dal_free_curl_request_info(&req_info);
    if (origin_type == EVENT_ORIGIN_TYPE_RETRY_FOREVER) {
        hand_event_after_sent(queue_msg_info->obj_handle);
    }
    return result;
}


LOCAL void _check_redfish_evt_rsc_for_hmm(guchar evt_rsc_type, gchar *board_slot)
{
    gchar *hmm_slotid = NULL;

    return_if_expr(NULL == board_slot);

    if (evt_rsc_type == EVENT_RESOURCE_HMM_STANDBY) {
        hmm_slotid = strstr(board_slot, HMM_BORAD);
        if (NULL != hmm_slotid) {
            if (hmm_slotid[3] == '1') {
                hmm_slotid[3] = '2';
            } else if (hmm_slotid[3] == '2') {
                hmm_slotid[3] = '1';
            } else {
                debug_log(DLOG_ERROR, "hmm_slotid is not equal 1 or 2, hmm_slotid is %c", hmm_slotid[3]);
            }
        } else {
            debug_log(DLOG_ERROR, "board slot do not include HMM ,is %s", board_slot);
        }
    }
}

LOCAL gint32 get_evt_subscription_id(OBJ_HANDLE subscription, guint8 *id)
{
    gint32 ret = dal_get_property_value_byte(subscription, PROPERTY_EVT_SUBSCRIPTION_ID, id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d",
            dfl_get_object_name(subscription), PROPERTY_EVT_SUBSCRIPTION_ID, ret);
        return ret;
    }
    
    if (*id < RF_EVT_SUBSCRIPTION_ID_MIN || *id > RF_EVT_SUBSCRIPTION_ID_MAX) {
        debug_log(DLOG_ERROR, "subscription id(%u) is invalid", *id);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gboolean check_event_retry_condition(gint32 already_retry_count)
{
    gint32 max_retry_count = 0;
    OBJ_HANDLE evt_svc_handle;
    gint32 ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, &evt_svc_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle for %s failed, ret is %d", __FUNCTION__, CLASS_RF_EVT_SVC, ret);
        return TRUE;
    }
    ret = dal_get_property_value_int32(evt_svc_handle, PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES, &max_retry_count);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d", dfl_get_object_name(evt_svc_handle),
            PROPERTY_RF_EVT_SVC_EVENT_RETRY_TIMES, ret);
        return TRUE;
    }
    return max_retry_count > already_retry_count;
}


LOCAL enum HTTP_RESPONSE_CODE set_state_value(OBJ_HANDLE description, const gchar *state)
{
    if (check_property_status(state) != TRUE) {
        debug_log(DLOG_ERROR, "%s(%s) is invalids", PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, state);
        return HTTP_BAD_REQUEST;
    }

    gchar old_state[MAX_EVT_STATE_LEN] = {0};
    gint32 ret = dal_get_property_value_string(description,
        PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, old_state, MAX_EVT_STATE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d", dfl_get_object_name(description),
            PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_save_property_value_string_remote(description, PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, state, DF_SAVE);
    debug_log(DLOG_ERROR, "set %s to %s from %s successfully", RFPROP_EVENT_STATE, state, old_state);
    return HTTP_OK;
}

LOCAL enum HTTP_RESPONSE_CODE set_drp_value(OBJ_HANDLE description, const gchar *drp)
{
    if (check_property_delivery_retry_policy(drp) != TRUE) {
        debug_log(DLOG_ERROR, "%s(%s) is invalids", PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, drp);
        return HTTP_BAD_REQUEST;
    }

    gchar old_drp[MAX_EVT_DRP_LEN] = {0};
    gint32 ret = dal_get_property_value_string(description,
        PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, old_drp, MAX_EVT_DRP_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d", dfl_get_object_name(description),
            PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_save_property_value_string_remote(description, PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, drp, DF_SAVE);
    debug_log(DLOG_ERROR, "set %s to %s from %s successfully", RFPROP_DELIVERY_RETRY_POLICY, drp, old_drp);
    return HTTP_OK;
}

enum HTTP_RESPONSE_CODE change_state_value(PROVIDER_PARAS_S* i_paras,
    OBJ_HANDLE description, const gchar *state)
{
    gchar old_state[MAX_EVT_STATE_LEN] = {0};
    (void)dal_get_property_value_string(description, PROPERTY_EVT_SUBSCRIPTION_EVT_STATE,
        old_state, MAX_EVT_STATE_LEN);
    gint32 ret = set_state_value(description, state);
    if (i_paras != NULL) {
        GSList* caller_info = NULL;
        caller_info = g_slist_append(caller_info,
            g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set %s to %s from %s %s",
            PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, state, old_state, ret == HTTP_OK ? "successfully" : "fail");
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }
    return ret;
}


enum HTTP_RESPONSE_CODE change_drp_value(PROVIDER_PARAS_S* i_paras,
    OBJ_HANDLE description, const gchar *drp)
{
    gchar old_drp[MAX_EVT_DRP_LEN] = {0};
    (void)dal_get_property_value_string(description, PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, old_drp, MAX_EVT_DRP_LEN);
    gint32 ret = set_drp_value(description, drp);
    if (i_paras != NULL) {
        GSList* caller_info = NULL;
        caller_info = g_slist_append(caller_info,
            g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set %s to %s from %s %s",
            PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, drp, old_drp, ret == HTTP_OK ? "successfully" : "fail");
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }
    return ret;
}

LOCAL gboolean check_event_match_description(OBJ_HANDLE subscription_handle, json_object* evt_msg_jso)
{
    gchar event_des[MAX_EVT_DESTINATION_LEN] = {0};
    json_object *evts_jso = NULL;
    
    (void)dal_get_property_value_string(subscription_handle, PROPERTY_EVT_SUBSCRIPTION_DES, event_des,
        MAX_EVT_DESTINATION_LEN);
    if (event_des[0] == '\0') {
        debug_log(DLOG_INFO, "the description is deleted, but still need to cache events");
        return TRUE;
    }
    json_object_object_get_ex(evt_msg_jso, RFPROP_EVENTS, &evts_jso);
    if (evts_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, get Events filed failed", __FUNCTION__);
        return FALSE;
    }
    json_object *event = json_object_array_get_idx(evts_jso, 0);
    if (event == NULL) {
        debug_log(DLOG_ERROR, "get event_json from description_json fail");
        return FALSE;
    }
    gboolean b_ret = _is_event_destination_element_match(subscription_handle, event, RFPROP_EVT_TYPE,
        PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, FALSE) &&
        _is_event_destination_element_match(subscription_handle, event, RFPROP_MSG_ID,
        PROPERTY_EVT_SUBSCRIPTION_MSG_ID, FALSE) &&
        _is_event_destination_element_match(subscription_handle, event, RFPROP_ORIGIN_RSC,
        PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, TRUE);
    if (b_ret == FALSE) {
        debug_log(DLOG_DEBUG, "match failed, destination:%s", event_des);
    }
    return b_ret;
}

LOCAL gboolean check_description_enable_match(OBJ_HANDLE subscription, json_object* evt_msg_jso, gchar *status)
{
    if (check_event_match_description(subscription, evt_msg_jso) == FALSE) {
        debug_log(DLOG_DEBUG, "the event is abanboned");
        return FALSE;
    }
    if (g_strcmp0(status, STATE_VALUE_DISABLED) == 0) {
        debug_log(DLOG_DEBUG, "the event is abandoned");
        return FALSE;
    }
    return TRUE;
}

LOCAL void copy_msg_queue_info(EVT_POST_QUEUE_INFO *old_info, EVT_POST_QUEUE_INFO **new_info)
{
    (*new_info)->evt_jso = json_object_get(old_info->evt_jso);
    (*new_info)->obj_handle = old_info->obj_handle;
    (*new_info)->ignore_ssl_cert_verify = old_info->ignore_ssl_cert_verify;
}


LOCAL enum EVT_HANDLE_POLICY handle_retry_forever_resubmit(EVT_POST_QUEUE_INFO *evt_queue_msg,
    gint32 already_retry_count, guint8 id, EVT_ORIGIN_TYPE origin_type)
{
    if (already_retry_count < 1) {
        return RESUBMIT;
    }
    if (origin_type != EVENT_ORIGIN_TYPE_RETRY_FOREVER) {
        EVT_POST_QUEUE_INFO *new_evt_queue_msg = g_malloc(sizeof(EVT_POST_QUEUE_INFO));
        if (new_evt_queue_msg == NULL) {
            debug_log(DLOG_ERROR, "%s: gmalloc failed!", __FUNCTION__);
            return POLICY_ERROR;
        }
        copy_msg_queue_info(evt_queue_msg, &new_evt_queue_msg);
        gint32 ret = vos_queue_send(g_forever_queue[id - 1], new_evt_queue_msg);
        if (ret != VOS_OK) {
            _free_evt_queue_msg_info(&new_evt_queue_msg);
            debug_log(DLOG_ERROR, "send event queue info for event subscription %d failed", id);
            return POLICY_ERROR;
        }
        return FORWAR_RETRY_FOREVER;
    }
    gint queue_len = g_async_queue_length(g_forever_queue[id - 1]);
    if (queue_len > 0) {
        save_send_error_event_log(evt_queue_msg->evt_jso, id, RETRY_TYPE, evt_queue_msg->ignore_ssl_cert_verify);
        g_evt_retry_forever_flag[id - 1] = 1; // 置起重试标记
        debug_log(DLOG_INFO, "the event is cached, because of multiple failures");
        return CACHED_RETRY;
    }
    return RESUBMIT;
}

LOCAL enum EVT_HANDLE_POLICY handle_evt_policy(EVT_POST_QUEUE_INFO *evt_queue_msg,
    gint32 already_retry_count, EVT_ORIGIN_TYPE origin_type)
{
    gchar status[MAX_EVT_STATE_LEN] = {0};
    gchar retry_poclicy[MAX_EVT_DRP_LEN] = {0};
    guint8 id = 0;
    gint32 ret = dal_get_property_value_string(evt_queue_msg->obj_handle,
        PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, status, MAX_EVT_STATE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d", dfl_get_object_name(evt_queue_msg->obj_handle),
            PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, ret);
        return POLICY_ERROR;
    }
    if (check_description_enable_match(evt_queue_msg->obj_handle, evt_queue_msg->evt_jso, status) != TRUE) {
        return ABANDONED;
    }
    ret = get_evt_subscription_id(evt_queue_msg->obj_handle, &id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d",
            dfl_get_object_name(evt_queue_msg->obj_handle), PROPERTY_EVT_SUBSCRIPTION_ID, ret);
        return POLICY_ERROR;
    }
    if (g_strcmp0(status, STATUS_VALUE_STANDBY_OFFLINE) == 0) {
        save_send_error_event_log(evt_queue_msg->evt_jso, id, SUSPENED_TYPE, evt_queue_msg->ignore_ssl_cert_verify);
        debug_log(DLOG_DEBUG, "the event is suspended");
        return CACHED_SUSPENDED;
    }
    (void)dal_get_property_value_string(evt_queue_msg->obj_handle,
        PROPERTY_EVT_SUBSCRIPTION_EVT_DRP, retry_poclicy, MAX_EVT_DRP_LEN);
    
    if (g_strcmp0(retry_poclicy, DELIVER_RETRY_VALUE_TERMINATE_AFTER_RETRY) == 0) {
        if (check_event_retry_condition(already_retry_count)) {
            return RESUBMIT;
        }
        debug_log(DLOG_DEBUG, "the event is abandoned, because of multiple failures");
        return ABANDONED;
    }

    if (g_strcmp0(retry_poclicy, DELIVER_RETRY_VALUE_SUSPEND_RETRIES) == 0) {
        if (check_event_retry_condition(already_retry_count)) {
            return RESUBMIT;
        }
        save_send_error_event_log(evt_queue_msg->evt_jso, id, SUSPENED_TYPE, evt_queue_msg->ignore_ssl_cert_verify);
        (void)set_state_value(evt_queue_msg->obj_handle, STATUS_VALUE_STANDBY_OFFLINE);
        debug_log(DLOG_DEBUG, "the event is cached, because of multiple failures");
        return CACHED_SUSPENDED;
    }
    if (g_strcmp0(retry_poclicy, DELIVER_RETRY_VALUE_RETRY_FOREVER) == 0) {
        return handle_retry_forever_resubmit(evt_queue_msg, already_retry_count, id, origin_type);
    }
    debug_log(DLOG_ERROR, "handle_evt_policy error");
    return POLICY_ERROR;
}


LOCAL void hand_event_after_sent(OBJ_HANDLE description)
{
    guint8 id = 0;
    gint32 ret = get_evt_subscription_id(description, &id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.%s fail, ret=%d",
            dfl_get_object_name(description), PROPERTY_EVT_SUBSCRIPTION_ID, ret);
        return;
    }
    if (g_evt_retry_forever_flag[id - 1] ==  0) {
        return;
    }
    gint queue_len = g_async_queue_length(g_forever_queue[id - 1]);
    if (queue_len >= 1) {
        return;
    }
    EVT_TIME_INFO time = {0};
    time.start = 0;
    time.end = vos_get_cur_time_stamp();
    json_object* events = get_event_list(time, id, RETRY_TYPE);
    if (events == NULL) {
        debug_log(DLOG_ERROR, "not find any event info");
        return;
    }
    g_evt_retry_forever_flag[id - 1] = 0;
    ret = send_retry_forever_evt_to_subscription(description, events);
    json_object_put(events);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: sent event message failed, ret=%d", __FUNCTION__, ret);
        g_evt_retry_forever_flag[id - 1] = 1;
        return;
    }
    delete_event_log(time, id, RETRY_TYPE);
}

LOCAL void _post_redfish_evt_thread(void *data)
{
    POCT_EVENT_THERAD_INFO *info = (POCT_EVENT_THERAD_INFO*)data;
    GAsyncQueue *queue = info->queue;
    EVT_ORIGIN_TYPE type = info->type;
    g_free(data);
    gint32 ret;
    EVT_POST_QUEUE_INFO *evt_queue_msg = NULL;

    while (1) {
        debug_log(DLOG_DEBUG, "_post_redfish_evt_thread: wait for message...");

        ret = vos_queue_receive(queue, (gpointer *)&evt_queue_msg, QUE_WAIT_FOREVER);
        if (ret != VOS_OK) {
            continue;
        }

        (void)_handle_redfish_evt_msg_queue(evt_queue_msg, type);
        _free_evt_queue_msg_info(&evt_queue_msg);
        debug_log(DLOG_DEBUG, "_post_redfish_evt_thread: receive one message");
    }
}
LOCAL gint32 create_evt_from_template(json_object* evt_msg_jso, guint32 evt_id,
    OBJ_HANDLE description, EVT_POST_QUEUE_INFO **msg_queue_info)
{
    gchar evt_rsc_id[MAX_RSC_ID_LEN] = {0};
    gchar evt_rsc_odata_id[MAX_RSC_URI_LEN] = {0};
    gchar evt_context[MAX_EVT_CONTEXT_LEN] = {0};
    gint32 ret =
        dal_get_property_value_string(description, PROPERTY_EVT_SUBSCRIPTION_EVT_CTX, evt_context, MAX_EVT_CONTEXT_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get property %s value, ret=%d", PROPERTY_EVT_SUBSCRIPTION_EVT_CTX, ret);
        return ret;
    }
    json_object *evt_jso = get_resource_cfg_json(EVT_TEMPLATE_FILE);
    if (evt_jso == NULL) {
        debug_log(DLOG_ERROR, "cann't load event template from %s", EVT_TEMPLATE_FILE);
        return RET_ERR;
    }
    json_object *evt_record_arr_jso = json_object_new_array();
    if (evt_record_arr_jso == NULL) {
        debug_log(DLOG_ERROR, "malloc json array failed");
        (void)json_object_put(evt_jso);
        return RET_ERR;
    }
    json_object_array_add(evt_record_arr_jso, json_object_get(evt_msg_jso));

    (void)snprintf_s(evt_rsc_id, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%u", evt_id);
    ret = snprintf_s(evt_rsc_odata_id, MAX_RSC_URI_LEN, MAX_RSC_URI_LEN - 1, EVT_RSC_PATTERN, evt_rsc_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    json_object_object_add(evt_jso, ODATA_ID, json_object_new_string((const gchar *)evt_rsc_odata_id));
    json_object_object_add(evt_jso, RFOBJ_ID, json_object_new_string((const gchar *)evt_rsc_id));
    json_object_object_add(evt_jso, RFPROP_EVT_CONTEXT, json_object_new_string((const gchar *)evt_context));
    json_object_object_add(evt_jso, RFPROP_EVENTS, evt_record_arr_jso);

    *msg_queue_info = _new_evt_post_queue_info(evt_jso, description);
    if (msg_queue_info == NULL) {
        (void)json_object_put(evt_jso);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL gint32 post_evt_msg_for_evt_subcription_handle(OBJ_HANDLE obj_handle, guint32 evt_id, json_object* evt_msg_jso,
    gboolean ignore_ssl_cert_verify)
{
    EVT_POST_QUEUE_INFO *msg_queue_info = NULL;
    guchar evt_subscription_id = 0;
    return_val_do_info_if_fail(NULL != evt_msg_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    gint32 ret = create_evt_from_template(evt_msg_jso, evt_id, obj_handle, &msg_queue_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "create_evt_from_template fail, ret=%d", ret);
        return ret;
    }
    msg_queue_info->ignore_ssl_cert_verify = ignore_ssl_cert_verify;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_EVT_SUBSCRIPTION_ID, &evt_subscription_id);
    if (evt_subscription_id == 0 || evt_subscription_id > MAX_EVT_SUBSCRIPTION_NUM) {
        _free_evt_queue_msg_info(&msg_queue_info);
        debug_log(DLOG_ERROR, "the evt id out of range, evt_subscription_id=%u", evt_subscription_id);
        return VOS_ERR;
    }
    ret = vos_queue_send(g_evt_msg_queue[evt_subscription_id - 1], msg_queue_info);
    if (VOS_OK != ret) {
        _free_evt_queue_msg_info(&msg_queue_info);
        debug_log(DLOG_ERROR, "send event queue info for event subscription %d failed", evt_subscription_id);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL void handle_send_event_msg(GSList *obj_list, guint32 evt_id, guint8 evt_origin_type,
    json_object* evt_msg_jso, gboolean ignore_ssl_cert_verify)
{
    gchar event_des[MAX_EVT_DESTINATION_LEN] = {0};
    OBJ_HANDLE subscription_handle;
    GSList *obj_iter = NULL;
    gboolean b_ret;

    for (obj_iter = obj_list; obj_iter != NULL; obj_iter = obj_iter->next) {
        subscription_handle = (OBJ_HANDLE)obj_iter->data;

        
        (void)dal_get_property_value_string(subscription_handle, PROPERTY_EVT_SUBSCRIPTION_DES, event_des,
            MAX_EVT_DESTINATION_LEN);
        if (event_des[0] == '\0') {
            continue;
        }

        
        
        if (evt_origin_type != EVENT_MONITOR_MODE_TEST) {
            b_ret = _is_event_destination_element_match(subscription_handle, evt_msg_jso, RFPROP_EVT_TYPE,
                PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, FALSE) &&
                _is_event_destination_element_match(subscription_handle, evt_msg_jso, RFPROP_MSG_ID,
                PROPERTY_EVT_SUBSCRIPTION_MSG_ID, FALSE) &&
                _is_event_destination_element_match(subscription_handle, evt_msg_jso, RFPROP_ORIGIN_RSC,
                PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, TRUE);
            if (b_ret == FALSE) {
                debug_log(DLOG_DEBUG, "match failed, destination:%s", event_des);
                continue;
            }
        }
        

        post_evt_msg_for_evt_subcription_handle(subscription_handle, g_event_rsc_id, evt_msg_jso,
            ignore_ssl_cert_verify);
    }
    return;
}


gint32 redfish_post_event_msg(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GSList *obj_list = NULL;
    const gchar *evt_msg_str = NULL;
    json_object *evt_msg_jso = NULL;
    guchar service_enabled;
    
    return_val_do_info_if_expr(NULL == input_list, VOS_ERR, debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n"));
    

    guchar evt_origin_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    gboolean ignore_ssl_cert_verify = (g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2)) != 0);

    gint32 ret = dal_get_property_value_byte(object_handle, PROPERTY_RF_EVT_SVC_SERVICE_ENABLED, &service_enabled);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "get property value for %s failed", PROPERTY_RF_EVT_SVC_SERVICE_ENABLED);
        return VOS_ERR;
    }

    
    
    check_success_do_return_val((0 == service_enabled) && (EVENT_MONITOR_MODE_TEST != evt_origin_type), VOS_OK,
        debug_log(DLOG_ERROR, "%s failed, post status disabled", __FUNCTION__));
    

    evt_msg_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    
    if (NULL == evt_msg_str) {
        debug_log(DLOG_ERROR, "get evt_msg_str failed");
        return VOS_ERR;
    }
    
    evt_msg_jso = json_tokener_parse(evt_msg_str);
    debug_log(DLOG_DEBUG, "%s,get evt_msg_str :%s", __FUNCTION__, evt_msg_str);
    return_val_do_info_if_fail(NULL != evt_msg_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "parse event msg to json object failed"));

    ret = dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get object list for %s", CLASS_EVT_SUBSCRIPTION);
        (void)json_object_put(evt_msg_jso));

    handle_send_event_msg(obj_list, g_event_rsc_id, evt_origin_type, evt_msg_jso, ignore_ssl_cert_verify);
    g_slist_free(obj_list);
    (void)json_object_put(evt_msg_jso);

    return VOS_OK;
}

LOCAL guint32 increase_and_get_event_rsc_id(void)
{
    (void)pthread_mutex_lock(& g_event_rsc_id_mutex);
    g_event_rsc_id++;
    if (g_event_rsc_id == 0) {
        g_event_rsc_id = 1;
    }
    (void)pthread_mutex_unlock(& g_event_rsc_id_mutex);
    return g_event_rsc_id;
}

gint32 get_event_orgin_rsc(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    OBJ_HANDLE evt_monitor_handle = 0;
    OBJ_HANDLE component_handle = 0;
    gchar                          component_uri[MAX_URI_LENGTH] = {0};
    guchar component_type;
    
    if (NULL == input_list || NULL == output_list) {
        debug_log(DLOG_ERROR, "Get event orgin resource failed, input_list and output_list can not be NULL.\r\n");
        return VOS_ERR;
    }
    

    const gchar *evt_monitor_name = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    return_val_do_info_if_fail(NULL != evt_monitor_name, VOS_ERR,
        debug_log(DLOG_ERROR, "get event monitor name from input list failed"));

    debug_log(DLOG_DEBUG, "Event monitor name:%s", evt_monitor_name);

    gint32 ret = dfl_get_object_handle(evt_monitor_name, &evt_monitor_handle);
    if (ret != RET_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            *output_list = g_slist_append(*output_list, g_variant_new_string(""));
            return RET_OK;
        }
        debug_log(DLOG_ERROR, "get object handle for %s faile %d", evt_monitor_name, ret);
        return RET_ERR;
    }

    ret = dfl_get_referenced_object(evt_monitor_handle, PROPERTY_EVENT_MONITOR_SUBJECT_OBJ, &component_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "get subject object for %s failed", evt_monitor_name));

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get device type from object handle [%s]", dfl_get_object_name(component_handle)));

    (void)rf_gen_component_uri(component_handle, component_uri, MAX_URI_LENGTH);
    do_val_if_fail('\0' != component_uri[0], debug_log(DLOG_ERROR,
        "unknown origin rsc uri ,event monitor name :%s, component type :%d", evt_monitor_name, component_type));

    ret = rf_validate_rsc_exist((const gchar *)component_uri, NULL);
    
    if ((VOS_OK != ret) && ('\0' != component_uri[0])) {
        debug_log(DLOG_MASS, "validate origin rsc failed, rsc uri:%s", component_uri);
        (void)memset_s(component_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
    }

    *output_list = g_slist_append(*output_list, g_variant_new_string((const gchar *)component_uri));

    return VOS_OK;
}


gint32 set_redfish_evt_server_identity_source(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gchar               server_identity_source[MAX_SERVER_IDENTITY_LEN] = {0};
    
    return_val_do_info_if_expr(NULL == input_list, VOS_ERR, debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n"));
    

    guchar identity_source_index = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    
    (void)server_identity_source_trans(&identity_source_index, server_identity_source, MAX_SERVER_IDENTITY_LEN,
        IDENTITY_INDEX_TO_STR);

    (void)dal_save_property_value_byte_remote(object_handle, PROPERTY_RF_EVT_SVC_SERVER_IDENTITY_SRC,
        identity_source_index, DF_SAVE);

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
        "Set Redfish event server identity to %s successfully.", server_identity_source);

    return VOS_OK;
}


gint32 set_redfish_evt_service_enabled(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    
    return_val_do_info_if_expr(NULL == input_list, VOS_ERR, debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n"));
    

    gchar post_enabled = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    (void)dal_save_property_value_byte_remote(object_handle, PROPERTY_RF_EVT_SVC_SERVICE_ENABLED, post_enabled,
        DF_SAVE);

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s Redfish event service successfully.",
        (TRUE == post_enabled) ? "Enable" : "Disable");

    return VOS_OK;
}


gint32 set_redfish_evt_subscription_ctx(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    const gchar *ctx_str = NULL;
    gint32 ret;
    
    return_val_do_info_if_expr(NULL == input_list, VOS_ERR, debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n"));
    

    ctx_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);

    return_val_do_info_if_fail((NULL != ctx_str) && (strlen(ctx_str) < MAX_COMMON_PROP_VAL_BUF_LEN), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set Redfish event subscription context failed."));

    ret = dal_save_property_value_string_remote(object_handle, PROPERTY_EVT_SUBSCRIPTION_EVT_CTX, ctx_str, DF_SAVE);

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set Redfish event subscription context %s.",
        (VOS_OK == ret) ? "successfully" : "failed");

    return ret;
}


gint32 set_redfish_evt_subscription_http_headers(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    const gchar *http_headers_str = NULL;
    gint32 ret;
    
    guchar subscription_id;
    
    GVariant *old_http_headers_var = NULL;
    GVariant *new_http_headers_var = NULL;
    gchar *old_header_plain_data = NULL;

    return_val_do_info_if_expr(input_list == NULL, VOS_ERR, debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n"));

    ret = dal_get_property_value_byte(object_handle, PROPERTY_EVT_SUBSCRIPTION_ID, &subscription_id);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "get event subscription id failed, ret is %d", ret));

    http_headers_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);

    goto_label_do_info_if_fail((NULL != http_headers_str) && (strlen(http_headers_str) < MAX_EVT_HEADERS_SIZE), exit,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        ret = VOS_ERR);

    ret =
        dfl_get_property_value(object_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, &old_http_headers_var);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "get event subscription handle for id [%d] failed, ret is %d", subscription_id, ret));

    ret = rf_get_property_plain_data(old_http_headers_var, &old_header_plain_data);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "get event subscription plain data failed, ret is %d", ret));

    ret = rf_get_property_encrypted_data(http_headers_str, &new_http_headers_var);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "get event subscription encrypted data failed, ret is %d", ret));

    ret = dfl_set_property_value(object_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, new_http_headers_var,
        DF_SAVE);
    // 模块降权限与回调接口存在并发调用，因此此处重试
    if (VOS_OK != ret && (0 != getuid())) {
        ret = dal_save_property_value_gvariant_remote(object_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS,
            new_http_headers_var, DF_SAVE);
    }
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "set evt subscription property failed, ret is %d", ret));

    
    if (0 != g_strcmp0(http_headers_str, old_header_plain_data)) {
        (void)dal_rf_rsc_evt_log(RF_RSC_UPDATED_EVT, FALSE, URI_FORMAT_EVT_SUBSCRIPTION, subscription_id);
    }

    ret = VOS_OK;

exit:

    do_val_if_expr(NULL != old_http_headers_var, g_variant_unref(old_http_headers_var));
    do_val_if_expr(NULL != new_http_headers_var, g_variant_unref(new_http_headers_var));

    do_val_if_expr(NULL != old_header_plain_data,
        clear_sensitive_info(old_header_plain_data, strlen(old_header_plain_data)));
    g_free(old_header_plain_data);

    

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Set Redfish event subscription http headers %s.",
        (VOS_OK == ret) ? "successfully" : "failed");

    

    return ret;
}


LOCAL gint32 _find_available_event_handle(const gchar *destination_str, OBJ_HANDLE *obj_handle,
    json_object **message_info_jso)
{
    errno_t safe_fun_ret;
    GSList *obj_list = NULL;
    GSList *obj_iter = NULL;
    OBJ_HANDLE tmp_handle = 0;
    gint32 ret;
    gchar          evt_destination_dup[MAX_EVT_DESTINATION_LEN] = {0};
    gchar          evt_destination[MAX_EVT_DESTINATION_LEN] = {0};
    gboolean obj_handle_found_flag = FALSE;
    guchar evt_subscription_id = MAX_EVT_SUBSCRIPTION_NUM;
    guchar last_id = MAX_EVT_SUBSCRIPTION_NUM;

    safe_fun_ret =
        strncpy_s(evt_destination_dup, sizeof(evt_destination_dup), destination_str, sizeof(evt_destination_dup) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    ret = dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &obj_list);
    return_val_do_info_if_fail((VOS_OK == ret), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso));

    for (obj_iter = obj_list; obj_iter != NULL; obj_iter = obj_iter->next) {
        tmp_handle = (OBJ_HANDLE)(obj_iter->data);

        (void)dal_get_property_value_string(tmp_handle, PROPERTY_EVT_SUBSCRIPTION_DES, evt_destination,
            MAX_EVT_DESTINATION_LEN);

        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_EVT_SUBSCRIPTION_ID, &evt_subscription_id);

        redfish_trim_uri(evt_destination_dup, sizeof(evt_destination_dup));
        if (0 == g_strcmp0(evt_destination, evt_destination_dup)) {
            (void)create_message_info(MSGID_RSC_IN_USE, NULL, message_info_jso);
            g_slist_free(obj_list);

            return HTTP_CONFLICT;
        }

        
        if ('\0' == evt_destination[0]) {
            do_val_if_expr(evt_subscription_id <= last_id, *obj_handle = tmp_handle; last_id = evt_subscription_id);
            obj_handle_found_flag = TRUE;
        }
    }

    g_slist_free(obj_list);

    if (obj_handle_found_flag == FALSE) {
        (void)create_message_info(MSGID_EVENT_LIMIT_EXCEED, NULL, message_info_jso);

        return HTTP_BAD_REQUEST;
    }

    return VOS_OK;
}


LOCAL gint32 _validate_evt_msg_id_and_origin_rsc(json_object *msg_id_jso, json_object *origin_rsc_jso,
    json_object **message_info_jso)
{
    gint32 ret;
    const gchar *value_str = NULL;
    json_object *id_iter = NULL;
    json_object *origin_jso_iter = NULL;

    
    json_object_array_foreach(msg_id_jso, id_iter) {
        value_str = dal_json_object_get_str(id_iter);

        
        ret = check_event_msg_id_validity(value_str);
        
        if (ret != RET_OK) {
            (void)create_message_info(MSGID_EVENT_MSG_ID_INVALID, RFPROP_MSG_IDS, message_info_jso, value_str,
                RFPROP_MSG_IDS);

            return HTTP_BAD_REQUEST;
        }
    }

    
    if (origin_rsc_jso != NULL) {
        
        json_object_array_foreach(origin_rsc_jso, origin_jso_iter) {
            value_str = dal_json_object_get_str(origin_jso_iter);

            
            ret = rf_validate_rsc_exist(value_str, NULL);
            if (ret != RET_OK) {
                (void)create_message_info(MSGID_EVENT_ORIGIN_RSC, RFPROP_ORIGIN_RSCES, message_info_jso, value_str,
                    RFPROP_ORIGIN_RSCES);

                return HTTP_BAD_REQUEST;
            }
            
        }
    }
    return RET_OK;
}


LOCAL gint32 _get_origin_rsc_uri_array_jso(json_object *origin_rsc_jso, json_object **array_jso)
{
    json_object *jso = NULL;
    json_object *iter = NULL;
    const gchar *rsc_uri = NULL;
    gint32 ret;
    json_object *item_jso = NULL;

    return_val_do_info_if_fail(json_type_array == json_object_get_type(origin_rsc_jso), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != jso, VOS_ERR, debug_log(DLOG_ERROR, "alloc new json array failed"));

    json_object_array_foreach(origin_rsc_jso, iter)
    {
        rsc_uri = NULL;
        (void)get_element_str(iter, ODATA_ID, &rsc_uri);

        return_val_do_info_if_fail(NULL != rsc_uri, VOS_ERR, (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "invalid origin resource uri"));

        item_jso = json_object_new_string(rsc_uri);
        ret = json_object_array_add(jso, item_jso);
        return_val_do_info_if_fail(0 == ret, VOS_ERR, (void)json_object_put(item_jso); (void)json_object_put(jso);
            debug_log(DLOG_ERROR, "add item to array jso failed"));
    }

    *array_jso = jso;

    return VOS_OK;
}


LOCAL gint32 _check_subscription_array_param_num(const char *prop_name, json_object *param_jso, gint32 max_item_num,
    json_object **message_info_jso)
{
    gint32 item_num;

    
    return_val_if_expr(json_type_array != json_object_get_type(param_jso), VOS_OK);

    item_num = json_object_array_length(param_jso);
    if (0 == item_num) {
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, prop_name, message_info_jso, prop_name);
        debug_log(DLOG_ERROR, "no item for %s found", prop_name);

        return HTTP_BAD_REQUEST;
    } else if (item_num > max_item_num) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, prop_name, message_info_jso, prop_name);
        debug_log(DLOG_ERROR, "item of count for %s exceeded, count is %d", prop_name, item_num);

        return HTTP_BAD_REQUEST;
    } else {
        debug_log(DLOG_DEBUG, "item for %s is %d", prop_name, item_num);
        return VOS_OK;
    }
}


LOCAL gint32 _check_originresources_array(json_object* param_jso, json_object** message_info_jso)
{
    gint32 item_num;

    
    if (param_jso == NULL) {
        return RET_OK;
    }

    
    if (json_object_get_type(param_jso) != json_type_array) {
        return RET_OK;
    }

    
    item_num = json_object_array_length(param_jso);
    if (item_num > MAX_RF_EVT_ORIGIN_RSC_NUM) {
        (void)create_message_info(MSGID_ARRAY_ITEM_EXCEED, RFPROP_ORIGIN_RSCES, message_info_jso, RFPROP_ORIGIN_RSCES);
        debug_log(DLOG_ERROR, "item of count for %s exceeded, count is %d", RFPROP_ORIGIN_RSCES, item_num);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}

gboolean check_property_delivery_retry_policy(const char *property)
{
    if (property == NULL) {
        return FALSE;
    }
    const char *option_delivery_retry_policy[] = {
        DELIVER_RETRY_VALUE_TERMINATE_AFTER_RETRY,
        DELIVER_RETRY_VALUE_SUSPEND_RETRIES,
        DELIVER_RETRY_VALUE_RETRY_FOREVER,
    };
    guint i = 0;
    for (; i < G_N_ELEMENTS(option_delivery_retry_policy); i++) {
        if (strcmp(option_delivery_retry_policy[i], property) == 0) {
            break;
        }
    }
    return i < G_N_ELEMENTS(option_delivery_retry_policy);
}

gboolean check_property_status(const char *property)
{
    if (property == NULL) {
        return FALSE;
    }
    const char *option_status[] = {
        STATE_VALUE_EANBLED,
        STATE_VALUE_DISABLED,
        STATUS_VALUE_STANDBY_OFFLINE,
    };
    guint i = 0;
    for (; i < G_N_ELEMENTS(option_status); i++) {
        if (strcmp(option_status[i], property) == 0) {
            break;
        }
    }
    return i < G_N_ELEMENTS(option_status);
}


LOCAL gint32 _validate_subscription_create_oper(json_object *body_jso, OBJ_HANDLE *obj_handle,
    json_object **message_info_jso, gboolean isComeFromBMA)
{
    const gchar *destination_str = NULL;
    const gchar *http_headers_str = NULL;
    const gchar *ctx_str = NULL;
    const gchar *drp_str = NULL;
    json_object *msg_ids_jso = NULL;
    json_object *origin_rsc_jso = NULL;
    json_object *uri_array_jso = NULL;
    json_object *evt_type_jso = NULL;
    gint32 ret;
    gint32 len;
    gchar               num_str_buf[MAX_STRBUF_LEN] = {0};

    (void)get_element_str(body_jso, RFPROP_EVT_DESTINATION, &destination_str);
    (void)get_element_str(body_jso, RFPROP_HTTP_HEADERS, &http_headers_str);
    (void)get_element_str(body_jso, RFPROP_CONTEXT, &ctx_str);
    (void)get_element_str(body_jso, RFPROP_DELIVERY_RETRY_POLICY, &drp_str);
    if (ctx_str == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso);
        debug_log(DLOG_ERROR, "%s: Missing %s in jso", __FUNCTION__, RFPROP_CONTEXT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)json_object_object_get_ex(body_jso, RFPROP_EVT_TYPES, &evt_type_jso);
    (void)json_object_object_get_ex(body_jso, RFPROP_MSG_IDS, &msg_ids_jso);
    (void)json_object_object_get_ex(body_jso, RFPROP_ORIGIN_RSCES, &origin_rsc_jso);

    return_val_do_info_if_fail(NULL != destination_str, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_info_jso);
        debug_log(DLOG_ERROR, "null value for %s", RFPROP_EVT_DESTINATION));

    len = strlen(destination_str);
    (void)snprintf_s(num_str_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%d", MAX_EVT_DESTINATION_LEN - 1);
    
    return_val_do_info_if_fail(len < MAX_EVT_DESTINATION_LEN, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_EVT_DESTINATION, message_info_jso,
        RFPROP_EVT_DESTINATION, destination_str, (const gchar *)num_str_buf);
        debug_log(DLOG_ERROR, "length for %s exceed, current length is %d", RFPROP_EVT_DESTINATION, len));
    
    
    
    if (NULL != origin_rsc_jso) {
        ret = _get_origin_rsc_uri_array_jso(origin_rsc_jso, &uri_array_jso);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST,
            (void)create_message_info(MSGID_MALFORMED_JSON, NULL, message_info_jso);
            debug_log(DLOG_ERROR, "get origin resource uri array failed"));
    }
    

    (void)json_object_object_add(body_jso, RFPROP_ORIGIN_RSCES, uri_array_jso);

    
    ret = _check_subscription_array_param_num(RFPROP_EVT_TYPES, evt_type_jso, MAX_RF_EVT_TYPE_NUM, message_info_jso);
    return_val_if_fail(VOS_OK == ret, ret);
    
    if (drp_str != NULL && strlen(drp_str) != 0 && check_property_delivery_retry_policy(drp_str) != TRUE) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_DELIVERY_RETRY_POLICY,
            message_info_jso, drp_str, RFPROP_DELIVERY_RETRY_POLICY);
        debug_log(DLOG_ERROR, "DeliveryRetryPolicy(%s) not valid", drp_str);
        return HTTP_BAD_REQUEST;
    }
    
    ret = _check_subscription_array_param_num(RFPROP_MSG_IDS, msg_ids_jso, MAX_RF_EVT_MSG_ID_NUM, message_info_jso);
    return_val_if_fail(VOS_OK == ret, ret);

    
    ret = _check_originresources_array(uri_array_jso, message_info_jso);
    return_val_if_fail(ret == RET_OK, ret);

    
    len = (http_headers_str) ? strlen(http_headers_str) : 0;
    (void)snprintf_s(num_str_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%d", MAX_EVT_HEADERS_SIZE - 1);
    return_val_do_info_if_fail(len < MAX_EVT_HEADERS_SIZE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_HTTP_HEADERS, message_info_jso, RF_SENSITIVE_INFO,
        RFPROP_HTTP_HEADERS, (const gchar *)num_str_buf);
        debug_log(DLOG_ERROR, "length for %s exceed, length is %d", RFPROP_HTTP_HEADERS, len));

    
    len = strlen(ctx_str);
    (void)snprintf_s(num_str_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%d", MAX_COMMON_PROP_VAL_BUF_LEN - 1);
    return_val_do_info_if_fail(len < MAX_COMMON_PROP_VAL_BUF_LEN, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_VAL_EXC_MAXLEN, RFPROP_CONTEXT, message_info_jso, ctx_str, RFPROP_CONTEXT,
        (const gchar *)num_str_buf);
        debug_log(DLOG_ERROR, "length for %s exceed, length is %d", RFPROP_CONTEXT, len));

    
    
    if (!((TRUE == g_str_has_prefix(destination_str, HTTPS_PREFIX)) ||
        (TRUE == isComeFromBMA && TRUE == g_str_has_prefix(destination_str, HTTP_PREFIX)))) {
        (void)create_message_info(MSGID_INVALID_EVT_DESTINATION, RFPROP_EVT_DESTINATION, message_info_jso,
            destination_str);
        return HTTP_BAD_REQUEST;
    }

    

    
    ret = _validate_evt_msg_id_and_origin_rsc(msg_ids_jso, uri_array_jso, message_info_jso);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "validate event message or origin resource failed");

        return ret;
    }

    
    ret = _find_available_event_handle(destination_str, obj_handle, message_info_jso);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "find available event handle failed, ret is %d", ret);

        return ret;
    }

    return VOS_OK;
}



LOCAL void _clear_evt_subscription_info(OBJ_HANDLE obj_handle)
{
    json_object *empty_arr_jso = NULL;
    gint32 ret;
    

    
    (void)dal_save_property_value_string_remote(obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, RF_STR_EMPTY, DF_SAVE);
    (void)dal_save_property_value_string_remote(obj_handle, PROPERTY_EVT_SUBSCRIPTION_EVT_CTX, RF_STR_EMPTY, DF_SAVE);
    (void)dal_save_property_value_string_remote(obj_handle, PROPERTY_EVT_SUBSCRIPTION_EVT_PROTOCOL, RF_STR_EMPTY,
        DF_SAVE);
    empty_arr_jso = json_object_new_array();

    return_do_info_if_fail(NULL != empty_arr_jso, debug_log(DLOG_ERROR, "alloc new json array failed"));

    ret = dal_set_json_array_to_obj_handle(obj_handle, PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, empty_arr_jso, DF_SAVE);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "set property %s failed, ret is %d", PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, ret));

    ret = dal_set_json_array_to_obj_handle(obj_handle, PROPERTY_EVT_SUBSCRIPTION_MSG_ID, empty_arr_jso, DF_SAVE);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "set property %s failed, ret is %d", PROPERTY_EVT_SUBSCRIPTION_MSG_ID, ret));

    ret = dal_set_json_array_to_obj_handle(obj_handle, PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, empty_arr_jso, DF_SAVE);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "set property %s failed, ret is %d", PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, ret));

    ret = dal_set_json_array_to_obj_handle(obj_handle, PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, empty_arr_jso,
        DF_SAVE);
    do_val_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "set property %s failed, ret is %d",
        PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, ret));

    

    (void)json_object_put(empty_arr_jso);
    return;
}


LOCAL void record_subscriber_type(OBJ_HANDLE obj_handle, const gchar *session_id)
{
    gint32 ret;
    guint8 type;

    // 内部会话说明是来自BMA的订阅
    if (rf_inner_session_id_check(session_id) == RET_OK) {
        type = RF_EVT_SUBSCRIBER_BMA;
    } else {
        type = RF_EVT_SUBSCRIBER_OTHER;
    }

    ret = dal_save_property_value_byte_remote(obj_handle, PROPERTY_EVT_SUBSCRIPTION_TYPE, type, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call dal_save_property_value_byte_remote failed, ret:%d", __FUNCTION__, ret);
    }
}


LOCAL gint32 check_form_ibma(PROVIDER_PARAS_S *i_param, gboolean *result)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    *result = FALSE;
    gint32 ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]: Get %s object handle error, ret=%d", __FUNCTION__, CLASS_SESSION, ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(i_param->session_id));
    ret = dfl_call_class_method(obj_handle, METHOD_SESSION_ISREDFISHINNERSESSION, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret == RET_OK) {
        *result = TRUE;
    }
    return RET_OK;
}


LOCAL gint32 clear_tmp_info(CLEAR_TEMP_INFO *temp, const gchar *property, gint32 ret)
{
    *(temp->type_json) = NULL;
    temp->type_str[0] = '\0';
    *(temp->type_strp) = NULL;
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set property value %s failed, ret is %d", property, ret);
    }
    return ret;
}


LOCAL gint32 remote_save_event_encrypt_property(OBJ_HANDLE scription, json_object *body)
{
    const gchar *type_strp = NULL;
    gsize encrypted_data_len = 0;
    
    (void)get_element_str(body, RFPROP_HTTP_HEADERS, &type_strp);

    if (type_strp != NULL) {
        GVariant *encrypted_data_var = NULL;
        const gchar **encrypted_data = NULL;
        gint32 ret = rf_get_property_encrypted_data(type_strp, &encrypted_data_var);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get encrypted property data failed, ret is %d", ret);
            return ret;
        }
        encrypted_data = g_variant_get_strv(encrypted_data_var, &encrypted_data_len);
        if (encrypted_data == NULL) {
            debug_log(DLOG_ERROR, "get value array for encrypted property data failed");
            g_variant_unref(encrypted_data_var);
            return RET_ERR;
        }
        (void)dal_save_property_value_array_string_remote(scription,
            PROPERTY_EVT_SUBSCRIPTION_ENCRYPTED_HTTP_HEADERS, encrypted_data, encrypted_data_len, DF_SAVE);
        g_free(encrypted_data);
        g_variant_unref(encrypted_data_var);
    }
    return RET_OK;
}

LOCAL gint32 remote_save_cmcc_event_property(OBJ_HANDLE scription, json_object *body)
{
    const gchar *type_strp = NULL;
    gint32 ret;
    (void)get_element_str(body, RFPROP_DELIVERY_RETRY_POLICY, &type_strp);
    if (dal_is_customized_by_cmcc()) {
        ret = dal_save_property_value_string_remote(scription, PROPERTY_EVT_SUBSCRIPTION_EVT_DRP,
            (type_strp == NULL || type_strp[0] == '\0') ?
            DELIVER_RETRY_VALUE_SUSPEND_RETRIES : type_strp, DF_SAVE);
    } else {
        ret = dal_save_property_value_string_remote(scription, PROPERTY_EVT_SUBSCRIPTION_EVT_DRP,
            (type_strp == NULL || type_strp[0] == '\0') ?
            DELIVER_RETRY_VALUE_TERMINATE_AFTER_RETRY : type_strp, DF_SAVE);
    }
    return ret;
}

LOCAL gint32 remote_save_event_property(OBJ_HANDLE scription, json_object *body)
{
    gchar type_str[MAX_EVT_DESTINATION_LEN] = {0};
    const gchar *type_strp = NULL;
    json_object *type_json = NULL;
    CLEAR_TEMP_INFO temp = {type_str, &type_strp, &type_json};
    
    (void)json_custom_get_elem_str(body, RFPROP_EVT_DESTINATION, type_str, MAX_EVT_DESTINATION_LEN);
    (void)redfish_trim_uri(type_str, sizeof(type_str));
    gint32 ret = dal_save_property_value_string_remote(scription,
        PROPERTY_EVT_SUBSCRIPTION_DES, type_str, DF_SAVE);
    if (clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_DES, ret) != RET_OK) {
        return ret;
    }
    ret = remote_save_cmcc_event_property(scription, body);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "save cmcc property fail, ret=%d", ret);
        return ret;
    }
    
    (void)json_object_object_get_ex(body, RFPROP_EVT_TYPES, &type_json);
    ret = dal_set_json_array_to_obj_handle(scription, PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, type_json, DF_SAVE);
    if (clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_EVT_TYPES, ret) != RET_OK) {
        return ret;
    }

    
    (void)json_object_object_get_ex(body, RFPROP_MSG_IDS, &type_json);
    
    (void)dal_set_json_array_to_obj_handle(scription, PROPERTY_EVT_SUBSCRIPTION_MSG_ID, type_json, DF_SAVE);
    clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_MSG_ID, RET_OK);

    
    (void)json_object_object_get_ex(body, RFPROP_ORIGIN_RSCES, &type_json);
    
    (void)dal_set_json_array_to_obj_handle(scription, PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, type_json, DF_SAVE);
    clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_ORIGIN_RSC, RET_OK);

    
    ret = dal_save_property_value_string_remote(scription, PROPERTY_EVT_SUBSCRIPTION_EVT_STATE,
        STATE_VALUE_EANBLED, DF_SAVE);
    if (clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_EVT_STATE, ret) != RET_OK) {
        return ret;
    }

    
    (void)get_element_str(body, RFPROP_CONTEXT, &type_strp);
    ret = dal_save_property_value_string_remote(scription, PROPERTY_EVT_SUBSCRIPTION_EVT_CTX, type_strp, DF_SAVE);
    if (clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_EVT_CTX, ret) != RET_OK) {
        return ret;
    }
    
    (void)get_element_str(body, RFPROP_PROTOCOL, &type_strp);
    ret = dal_save_property_value_string_remote(scription, PROPERTY_EVT_SUBSCRIPTION_EVT_PROTOCOL, type_strp, DF_SAVE);
    if (clear_tmp_info(&temp, PROPERTY_EVT_SUBSCRIPTION_EVT_PROTOCOL, ret) != RET_OK) {
        return ret;
    }
    return RET_OK;
}


LOCAL gint32 create_subscription(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso)
{
    gint32 ret;
    OBJ_HANDLE evt_obj_handle = 0;
    json_object *message_info_jso = NULL;
    gint32 array_add_ret;
    guchar evt_subscription_id;
    GSList *caller_info = NULL;
    gboolean is_comefromBMA = FALSE;

    // 判断是否来自于iBMA
    if (check_form_ibma(i_param, &is_comefromBMA) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = _validate_subscription_create_oper(body_jso_checked, &evt_obj_handle, &message_info_jso, is_comefromBMA);

    
    goto_label_do_info_if_fail(VOS_OK == ret, err_quit,
        debug_log(DLOG_ERROR, "validate subscription create operation failed, ret is %d", ret));

    (void)dal_get_property_value_byte(evt_obj_handle, PROPERTY_EVT_SUBSCRIPTION_ID, &evt_subscription_id);
    (void)snprintf_s(id, id_len, id_len - 1, "%u", evt_subscription_id);

    if (remote_save_event_encrypt_property(evt_obj_handle, body_jso_checked) != RET_OK) {
        _clear_evt_subscription_info(evt_obj_handle);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (remote_save_event_property(evt_obj_handle, body_jso_checked) != RET_OK) {
        _clear_evt_subscription_info(evt_obj_handle);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    record_subscriber_type(evt_obj_handle, (const gchar *)i_param->session_id);

    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_param->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->client));

    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Create event subscription successfully.");
    uip_free_gvariant_list(caller_info);

    return VOS_OK;

err_quit:
    array_add_ret = json_object_array_add(message_array_jso, message_info_jso);
    return_val_do_info_if_fail(0 == array_add_ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(message_info_jso));

    return ret;
}

gint32 create_subscription_entry(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id,
    guint32 id_len, json_object *message_array_jso)
{
    gint32 ret;

    
    json_object *message_info_jso = NULL;
    if (i_param == NULL || body_jso_checked == NULL || id == NULL || message_array_jso == NULL || id_len == 0) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if ((i_param->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        debug_log(DLOG_ERROR, "%s failed:privilege insufficient privilege.", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        ret = json_object_array_add(message_array_jso, message_info_jso);
        if (ret != 0) {
            (void)json_object_put(message_info_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        return HTTP_FORBIDDEN;
    }

    ret = create_subscription(i_param, body_jso_checked, id, id_len, message_array_jso);
    json_object_clear_string(body_jso_checked, RFPROP_HTTP_HEADERS);
    return ret;
}

gint32 delete_subscription(PROVIDER_PARAS_S *i_param, json_object *message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar evt_subscription_id = 0;
    gchar              evt_destination[MAX_EVT_DESTINATION_LEN] = {0};
    GSList *caller_info = NULL;
    gint32 array_add_ret;

    return_val_do_info_if_fail((NULL != i_param) && (NULL != message_array_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = vos_str2int(i_param->member_id, 10, &evt_subscription_id, NUM_TPYE_UCHAR);
    goto_label_do_info_if_fail(VOS_OK == ret, quit,
        debug_log(DLOG_ERROR, "get subscription id from %s failed", i_param->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri); ret = HTTP_NOT_FOUND);

    ret = dal_get_specific_object_byte(CLASS_EVT_SUBSCRIPTION, PROPERTY_EVT_SUBSCRIPTION_ID, evt_subscription_id,
        &obj_handle);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_EVT_SUBSCRIPTION_DES, evt_destination,
        MAX_EVT_DESTINATION_LEN);

    if ((VOS_OK != ret) || ('\0' == evt_destination[0])) {
        debug_log(DLOG_ERROR, "invalid event subscription id:%s", i_param->member_id);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);

        ret = HTTP_NOT_FOUND;
        goto quit;
    }

    goto_label_do_info_if_fail(0 != (i_param->user_role_privilege & USERROLE_BASICSETTING), quit, ret = HTTP_FORBIDDEN;
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso));

    
    _clear_evt_subscription_info(obj_handle);

    
    
    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_param->is_from_webui)));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->client));
    proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Delete event subscription successfully.");
    uip_free_gvariant_list(caller_info);

    ret = VOS_OK;

quit:
    array_add_ret = json_object_array_add(message_array_jso, message_info_jso);
    return_val_do_info_if_fail(0 == array_add_ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(message_info_jso);
        debug_log(DLOG_ERROR, "%s failed:add item to json array failed, ret is %d", __FUNCTION__, ret));

    return ret;
}


LOCAL gint32 _gen_messaeg_str_with_trigger_mode(guchar trigger_mode, const gchar *original_message_str,
    gchar *message_str_buf, guint32 msg_buf_len)
{
    errno_t str_ret;

    return_val_do_info_if_fail((NULL != original_message_str) && (NULL != message_str_buf), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    str_ret = memset_s(message_str_buf, msg_buf_len, 0, msg_buf_len);
    return_val_do_info_if_fail(EOK == str_ret, VOS_ERR, debug_log(DLOG_ERROR, "clean string failed"));

    debug_log(DLOG_DEBUG, "event trigger mode :%d", trigger_mode);

    dal_get_event_trigger_mode_string(trigger_mode, message_str_buf, msg_buf_len);

    str_ret = strncat_s(message_str_buf, msg_buf_len, original_message_str, strlen(original_message_str));
    return_val_do_info_if_fail(EOK == str_ret, VOS_ERR, debug_log(DLOG_ERROR, "cat message string failed"));

    return VOS_OK;
}


LOCAL void _get_server_location(gchar *chassis_location, guint32 max_location_len, guchar evt_rsc_type)
{
    OBJ_HANDLE product_handle = 0;
    gint32 ret;
    guint32 chassis_id = 0;
    gchar               board_slot[MAX_RSC_NAME_LEN] = {0};

    return_do_info_if_fail((NULL != chassis_location),
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));
    return_do_info_if_expr(max_location_len == 0,
        debug_log(DLOG_ERROR, "%s failed:input param error, max_location_len is 0.", __FUNCTION__));
    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "get product handle failed, ret is %d", ret));
    ret = dal_get_property_value_uint32(product_handle, PROPERTY_CHASSIS_NUM, &chassis_id);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "get chassis number failed, ret is %d", ret));

    (void)dal_rf_get_board_slot(board_slot, MAX_RSC_NAME_LEN);

    _check_redfish_evt_rsc_for_hmm(evt_rsc_type, board_slot);
    
    if (0xFFFFFFFF == chassis_id) {
        ret = snprintf_s(chassis_location, max_location_len, max_location_len - 1, OSCA_EVT_LOCATION_ANOTHER_FMT,
            board_slot);
    } else {
        ret = snprintf_s(chassis_location, max_location_len, max_location_len - 1, OSCA_EVT_LOCATION_FMT, chassis_id,
            board_slot);
    }

    
    return_do_info_if_fail(ret > 0, debug_log(DLOG_ERROR, "format osca chassis location failed, ret is %d", ret));

    return;
}


gint32 add_str_to_jso(json_object *obj_jso, const gchar *str, const gchar *prop_name)
{
    json_object *str_jso = NULL;

    if (str == NULL || strlen(str) == 0) {
        str_jso = NULL;
    } else {
        str_jso = json_object_new_string(str);
        return_val_do_info_if_expr(str_jso == NULL, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: new json object string fail", __FUNCTION__));
    }

    json_object_object_add(obj_jso, prop_name, str_jso);
    return VOS_OK;
}


LOCAL gint32 replace_event_detail_info_by_custom_id(guint8 event_detail_custom_id, RF_EVT_MSG_INFO *evt_info,
    json_object *evt_msg_jso, json_object *hw_jso)
{
    gint32 ret;
    
    switch (event_detail_custom_id) {
        case RF_RSC_CUSTOM_ID_CMCC:
            // MM板的事件上报未对severity和monitor_name赋有效值，后续如果支持定制化需要注意加上
            ret = replace_event_severity_customized_by_cmcc(evt_info, evt_msg_jso);
            if (ret != RET_OK) {
                break;
            }
            ret = replace_event_id_customized_by_cmcc_cucc(evt_info, evt_msg_jso);
            if (ret != RET_OK) {
                break;
            }
            ret = get_event_detail_customized_by_cmcc(evt_info, hw_jso);
            break;
        case RF_RSC_CUSTOM_ID_CUCC:
            ret = replace_event_id_customized_by_cmcc_cucc(evt_info, evt_msg_jso);
            if (ret != RET_OK) {
                break;
            }
            ret = get_event_detail_customized_by_cucc(evt_info, hw_jso);
            break;
        default:
            ret = RET_OK;
            break;
    }
    return ret;
}

gint32 update_redfish_oem_manufacturer(OBJ_HANDLE object_handle, gpointer user_data, gchar* property_name,
    GVariant* property_value)
{
    const gchar* custom_manu = NULL;
    guint32 manu_len;

    
    custom_manu = g_variant_get_string(property_value, 0);
    if (custom_manu == NULL) {
        debug_log(DLOG_ERROR, "%s: g_variant_get_string error.", __FUNCTION__);
        return RET_ERR;
    }

    manu_len = strlen(custom_manu);
    if (manu_len == 0 || manu_len >= sizeof(g_manufacturer)) {
        debug_log(DLOG_ERROR, "%s: manufacturer property len(%d) error.", __FUNCTION__, manu_len);
        return RET_ERR;
    }

    if (strncpy_s(g_manufacturer, sizeof(g_manufacturer), custom_manu, manu_len) != EOK) {
        debug_log(DLOG_ERROR, "%s: vos_strncpy_s failed.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL const gchar* get_custom_manufacturer_name(OBJ_HANDLE* obj_handle, guint8* event_detail_custom_id)
{
    gint32 ret;

    ret = dal_get_object_handle_nth(CLASS_RF_EVT_SVC, 0, obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle for %s failed, ret is %d", __FUNCTION__, CLASS_RF_EVT_SVC, ret);
        return NULL;
    }

    ret = dal_get_property_value_byte(*obj_handle, PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID,
        event_detail_custom_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property %s failed, ret is %d",
            __FUNCTION__, PROPERTY_RF_EVT_SVC_EVENT_DETAIL_CUSTOMIZED_ID, ret);
        return NULL;
    }

    return (const gchar*)g_manufacturer;
}


LOCAL gint32 add_event_level(RF_EVT_MSG_INFO *evt_info, json_object *jso)
{
    gint32 ret;
    const gchar *level_str[] = { INFORMATIONAL_STRING, MINOR_STRING, MAJOR_STRING, CRITICAL_STRING };
 
    if (strcmp(evt_info->event_type, RF_ALERT_EVT_STR) != 0) {
        return RET_OK;
    }
 
    if (evt_info->severity < G_N_ELEMENTS(level_str)) {
        ret = add_str_to_jso(jso, level_str[evt_info->severity], RFPROP_EVT_LEVEL);
    } else {
        ret = RET_ERR;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "add_event_level severity: %u failed, ret is %d", evt_info->severity, ret);
    }
    return ret;
}

LOCAL gint32 get_event_identity(OBJ_HANDLE obj_handle, gchar *identity, guint32 len)
{
    guint8 server_identity_source = 0;
    gint32 ret = dal_get_property_value_byte(obj_handle,
        PROPERTY_RF_EVT_SVC_SERVER_IDENTITY_SRC, &server_identity_source);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get server identity source failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dal_get_server_identity(server_identity_source, identity, len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get server identity failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 get_event_detail_info(RF_EVT_MSG_INFO *evt_info, json_object *evt_msg_jso)
{
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;
    gchar event_server_location[MAX_SERVER_LOCATION_LEN] = {0};
    gchar event_server_identity[MAX_SERVER_IDENTITY_LEN] = {0};
    guint8 event_detail_custom_id = 0;
    guint32 last_alert_id = 0;
    gchar evt_last_alert_id[MAX_RSC_ID_LEN] = {0};

    json_object *oem_jso = json_object_new_object();
    return_val_do_info_if_expr(oem_jso == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: alloc new json object for oem failed.", __FUNCTION__));
    json_object_object_add(evt_msg_jso, RFPROP_COMMON_OEM, oem_jso);

    const gchar *oem_name = get_custom_manufacturer_name(&obj_handle, &event_detail_custom_id);
    if (oem_name == NULL) {
        debug_log(DLOG_ERROR, "%s: get custom manufacturer name failed", __FUNCTION__);
        return RET_ERR;
    }

    json_object *hw_jso = json_object_new_object();
    return_val_do_info_if_expr(hw_jso == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: alloc new json object for hw failed.", __FUNCTION__));
    json_object_object_add(oem_jso, oem_name, hw_jso);

    gint32 ret = add_str_to_jso(hw_jso, (const gchar *)(evt_info->evt_subject), RFPROP_EVT_SUBJECT);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    ret = get_event_identity(obj_handle, event_server_identity, MAX_SERVER_IDENTITY_LEN);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_server_location(event_server_location, MAX_SERVER_LOCATION_LEN);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get server location failed, ret is %d", __FUNCTION__, ret));
    
    do_val_if_expr(strlen(event_server_location) == 0,
        _get_server_location(event_server_location, MAX_SERVER_LOCATION_LEN, evt_info->evt_rsc_type));
    
    

    ret = __update_event_rsc_id_info(obj_handle, evt_msg_jso, &last_alert_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "update event rsc id and last alert id failed, ret = %d", ret);
        return RET_ERR;
    }
    (void)snprintf_s(evt_last_alert_id, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%u", last_alert_id);

    ret = add_str_to_jso(hw_jso, (const gchar *)event_server_identity, RFPROP_SERVER_IDENTITY);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);

    ret = add_str_to_jso(hw_jso, (const gchar *)event_server_location, RFPROP_SERVER_LOCATION);
    return_val_if_expr(ret != VOS_OK, VOS_ERR);
    
    if (add_str_to_jso(hw_jso, (const gchar *)evt_last_alert_id, RFPROP_LAST_ALERT_ID) != RET_OK) {
        return RET_ERR;
    }

    ret = replace_event_detail_info_by_custom_id(event_detail_custom_id, evt_info, evt_msg_jso, hw_jso);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return add_event_level(evt_info, hw_jso);
}


LOCAL gint32 get_event_severity_json(RF_EVT_MSG_INFO *evt_info, json_object *msg_cfg_jso, json_object *evt_msg_jso)
{
    json_object *severity_jso = NULL;

    
    (void)json_object_object_get_ex(msg_cfg_jso, SEVERITY_KEY, &severity_jso);
    return_val_do_info_if_expr(severity_jso == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't find property %s for %s", SEVERITY_KEY, evt_info->msg_id));
    json_object_object_add(evt_msg_jso, SEVERITY_KEY, json_object_get(severity_jso));

    guchar software_type = 0;
    const char *severity_json_str = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;
    guint8 severity = 0;
    const char* severity_str[] = {
        OK_STRING,
        WARNING_STRING,
        WARNING_STRING,
        CRITICAL_STRING,
    };
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        debug_log(DLOG_DEBUG, "%s: evt_id=%d, msg_id=%s", __FUNCTION__, evt_info->evt_id, evt_info->msg_id);
        return_val_if_expr(evt_info->evt_id == 0, VOS_OK);

        ret = dal_get_specific_object_string(CLASS_EVENT_INFORMATION, PROPERTY_EVENT_INFORMATION_EVENT_NAME,
            evt_info->msg_id, &obj_handle);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_OK,
            debug_log(DLOG_ERROR, "%s: dal_get_specific_object_string failed. ret = %d", __FUNCTION__, ret));

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_EVENT_INFORMATION_SEVERITY, &severity);
        if (severity >= G_N_ELEMENTS(severity_str)) {
            debug_log(DLOG_ERROR, "%s: severity out of range, severity= %d", __FUNCTION__, severity);
            return VOS_ERR;
        }

        severity_json_str = dal_json_object_get_str(severity_jso);
        return_val_if_expr(strcmp(severity_str[severity], severity_json_str) == 0, VOS_OK);
        severity_jso = json_object_new_string(severity_str[severity]);
        json_object_object_add(evt_msg_jso, SEVERITY_KEY, severity_jso);
    }

    return VOS_OK;
}

gint32 create_evt_message_info(RF_EVT_MSG_INFO *evt_info, json_object **message_info_jso)
{
    errno_t safe_fun_ret;
    guint i, array_len;
    GSList *param_list = NULL;
    gchar               event_id_buf[MAX_STRBUF_LEN] = {0};
    gchar               msg_id_buf[MSGID_MAX_LEN] = {0};
    json_object *msg_cfg_jso = NULL;
    json_object *param_array_jso = NULL;
 
    gchar               message_format_str[MESSAGE_MAX_LENGTH] = {0};
    gchar *msg_formatted = NULL;
    json_object *odata_id_jso = NULL;
    json_object *str_jso = NULL;

    return_val_do_info_if_expr((evt_info == NULL) || (message_info_jso == NULL), VOS_ERR,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    gchar **arg_v = g_strsplit(evt_info->msg_params, MSG_ARG_DELIMITER, 0);
    return_val_do_info_if_expr(arg_v == NULL, VOS_ERR, debug_log(DLOG_ERROR, "split message param failed"));

    array_len = g_strv_length(arg_v);
    for (i = 0; i < array_len; i++) {
        const gchar *desp_end = NULL;
        if (g_utf8_validate((const gchar *)arg_v[i], -1, &desp_end) != TRUE) {
            arg_v[i][desp_end - arg_v[i]] = 0;
        }
        param_list = g_slist_append(param_list, g_variant_new_string(arg_v[i]));
    }

    gint32 ret = find_msg_info(evt_info->msg_id, msg_id_buf, MSGID_MAX_LEN, &msg_cfg_jso);
    return_val_do_info_if_expr((ret != VOS_OK) || (msg_cfg_jso == NULL), VOS_ERR,
        debug_log(DLOG_MASS, "cann't find message config for %s", evt_info->msg_id);
        uip_free_gvariant_list(param_list); g_strfreev(arg_v));

    
    ret = json_custom_get_elem_str(msg_cfg_jso, MESSAGE_KEY, message_format_str, MESSAGE_MAX_LENGTH);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't find property %s for %s", MESSAGE_KEY, evt_info->msg_id);
        uip_free_gvariant_list(param_list); g_strfreev(arg_v));

    
    dal_formated_str(message_format_str, param_list, &msg_formatted, '.');
    uip_free_gvariant_list(param_list);

    return_val_do_info_if_expr(msg_formatted == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "get formatted string for %s of msg id %s failed", message_format_str, evt_info->msg_id);
        g_strfreev(arg_v));

    ret = _gen_messaeg_str_with_trigger_mode(evt_info->evt_origin_type, msg_formatted, message_format_str,
        MESSAGE_MAX_LENGTH);
    g_free(msg_formatted);
    return_val_do_info_if_expr(
        ret != VOS_OK, VOS_ERR, debug_log(DLOG_ERROR, "get message str with trigger mode failed"); g_strfreev(arg_v));

    dal_strcat_bom_sn(evt_info->sn, evt_info->pn, message_format_str, MESSAGE_MAX_LENGTH);

    GVariant *str_array_var = g_variant_new_strv((const gchar * const *)arg_v, -1);
    ret = rf_get_json_array_from_variant(str_array_var, &param_array_jso);
    g_variant_unref(str_array_var);
    g_strfreev(arg_v);

    
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get message args json array for %s", evt_info->msg_id));

    
    json_object *evt_msg_jso = get_resource_cfg_json(EVT_MSG_TEMPLATE_FILE);
    return_val_do_info_if_expr(evt_msg_jso == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "new json object from %s failed", EVT_MSG_TEMPLATE_FILE);
        (void)json_object_put(param_array_jso));

    
    json_object_object_add(evt_msg_jso, MESSAGE_KEY, json_object_new_string((const gchar *)message_format_str));

    
    json_object_object_add(evt_msg_jso, MESSAGE_ARGS_KEY, param_array_jso);

    ret = get_event_severity_json(evt_info, msg_cfg_jso, evt_msg_jso);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "get_event_severity_json for %s failed.", evt_info->msg_id);
        (void)json_object_put(evt_msg_jso));

    
    json_object_object_add(evt_msg_jso, RFPROP_EVT_TYPE, json_object_new_string(evt_info->event_type));

    
    json_object_object_add(evt_msg_jso, RFPROP_EVT_TIMESTAMP, json_object_new_string(evt_info->event_time_stamp));

    
    ret = snprintf_s(event_id_buf, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "0x%08X", evt_info->evt_id);
    return_val_do_info_if_expr(ret <= 0, VOS_ERR, debug_log(DLOG_ERROR, "format event id failed, ret is %d", ret);
        (void)json_object_put(evt_msg_jso));

    str_jso = (0 == evt_info->evt_id) ? NULL : json_object_new_string((const gchar *)event_id_buf);
    json_object_object_add(evt_msg_jso, RFPROP_EVT_ID, str_jso);

    
    safe_fun_ret = strncat_s(msg_id_buf, MSGID_MAX_LEN, DOT_STR, strlen(DOT_STR));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(msg_id_buf, MSGID_MAX_LEN, evt_info->msg_id, strlen(evt_info->msg_id));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    json_object_object_add(evt_msg_jso, MESSAGE_ID_KEY, json_object_new_string((const gchar *)msg_id_buf));

    
    odata_id_jso = json_object_new_object();
    return_val_do_info_if_expr(odata_id_jso == NULL, VOS_ERR, debug_log(DLOG_ERROR, "alloc new json object failed");
        (void)json_object_put(evt_msg_jso));

    str_jso = (strlen(evt_info->origin_rsc) == 0) ? NULL : json_object_new_string(evt_info->origin_rsc);
    json_object_object_add(odata_id_jso, ODATA_ID, str_jso);
    json_object_object_add(evt_msg_jso, RFPROP_ORIGIN_RSC, odata_id_jso);

    
    ret = get_event_detail_info(evt_info, evt_msg_jso);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR, (void)json_object_put(evt_msg_jso);
        debug_log(DLOG_ERROR, "%s: get event detail info failed.", __FUNCTION__));

    *message_info_jso = evt_msg_jso;

    return VOS_OK;
}


LOCAL gint32 get_evt_info(GSList *input_list, RF_EVT_MSG_INFO *evt_msg_info)
{
    const gchar *sn_str = NULL;
    const gchar *pn_str = NULL;
    const gchar *subject_name = NULL;
    const gchar *subject_location = NULL;
    const gchar *message_id_str = NULL;
    const gchar *message_args_str = NULL;
    const gchar *event_type_str = NULL;
    const gchar *timestamp_str = NULL;
    const gchar *origin_rsc_str = NULL;
    const gchar *event_subject_str = NULL;
    errno_t str_ret;
    guint8 severity;
    const gchar *monitor_name = NULL;

    return_val_do_info_if_expr((input_list == NULL || evt_msg_info == NULL), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: failed, input param error", __FUNCTION__));

    message_id_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    message_args_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    event_type_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 2), NULL);
    timestamp_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 3), NULL);
    evt_msg_info->evt_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 4));
    origin_rsc_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 5), NULL);
    
    event_subject_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 6), NULL);
    evt_msg_info->evt_origin_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 7));
    
    evt_msg_info->evt_rsc_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 8));
    sn_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 9), NULL);
    pn_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 10), NULL);
    evt_msg_info->subject_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 11));
    subject_name = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 12), NULL);
    subject_location = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 13), NULL);
    evt_msg_info->evt_state = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 14));
    monitor_name = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 15), NULL);
    severity = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 16));

    
    if ((message_id_str == NULL) || (message_args_str == NULL) || (event_type_str == NULL) || (timestamp_str == NULL) ||
        (origin_rsc_str == NULL) || (event_subject_str == NULL) || (sn_str == NULL) || (pn_str == NULL) ||
        (subject_name == NULL) || (subject_location == NULL) || (monitor_name == NULL)) {
        debug_log(DLOG_ERROR, "%s :g_variant_get_string faiied", __FUNCTION__);
        return VOS_ERR;
    }
    
    evt_msg_info->severity = severity;
    (void)strncpy_s(evt_msg_info->monitor_name, MAX_NAME_SIZE + 1, monitor_name, MAX_NAME_SIZE);
    (void)strncpy_s(evt_msg_info->msg_id, MAX_MSG_ID_LEN, message_id_str, MAX_MSG_ID_LEN - 1);
    (void)strncpy_s(evt_msg_info->msg_params, MAX_MSG_PARAM_LEN, message_args_str, MAX_MSG_PARAM_LEN - 1);
    (void)strncpy_s(evt_msg_info->event_type, MAX_EVT_TYPE_LEN, event_type_str, MAX_EVT_TYPE_LEN - 1);
    (void)strncpy_s(evt_msg_info->event_time_stamp, MAX_RF_TIME_STAMP_LEN, timestamp_str, MAX_RF_TIME_STAMP_LEN - 1);
    (void)strncpy_s(evt_msg_info->origin_rsc, MAX_URI_LENGTH, origin_rsc_str, MAX_URI_LENGTH - 1);
    str_ret = strncpy_s(evt_msg_info->sn, NAME_LEN, sn_str, NAME_LEN - 1);
    do_val_if_expr(str_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s sn fail, str_ret = %d\n", __FUNCTION__, str_ret));
    str_ret = strncpy_s(evt_msg_info->pn, NAME_LEN, pn_str, NAME_LEN - 1);
    do_val_if_expr(str_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s pn fail, str_ret = %d\n", __FUNCTION__, str_ret));

    
    str_ret = strncpy_s(evt_msg_info->evt_subject, MAX_EVT_SUBJECT_DETAIL_LEN, event_subject_str,
        MAX_EVT_SUBJECT_DETAIL_LEN - 1);
    return_val_do_info_if_expr(EOK != str_ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: copy string %s failed", __FUNCTION__, event_subject_str));
    
    str_ret = strncpy_s(evt_msg_info->subject_name, MAX_NAME_SIZE, subject_name, strlen(subject_name));
    return_val_do_info_if_expr(str_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: copy string %s failed", __FUNCTION__, subject_name));
    str_ret =
        strncpy_s(evt_msg_info->subject_location, MAX_EVT_LOCATION_LEN, subject_location, strlen(subject_location));
    return_val_do_info_if_expr(str_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: copy string %s failed", __FUNCTION__, subject_location));

    return VOS_OK;
}


gint32 create_event_message(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    json_object *message_info_jso = NULL;
    RF_EVT_MSG_INFO evt_info;

    
    return_val_do_info_if_expr((NULL == input_list) || (NULL == output_list), VOS_ERR,
        debug_log(DLOG_ERROR, "Create event message failed, input_list and output_list can not be NULL.\r\n"));
    

    (void)memset_s(&evt_info, sizeof(RF_EVT_MSG_INFO), 0, sizeof(RF_EVT_MSG_INFO));

    ret = get_evt_info(input_list, &evt_info);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get event info failed, ret is %d", __FUNCTION__, ret);
        return VOS_ERR;
    }
    ret = create_evt_message_info(&evt_info, &message_info_jso);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_MASS, "create event message info failed, ret is %d", ret));

    *output_list = g_slist_append(*output_list, g_variant_new_string(dal_json_object_get_str(message_info_jso)));
    (void)json_object_put(message_info_jso);

    return VOS_OK;
}
LOCAL gint32 post_resume_evt_for_subcription_handle(OBJ_HANDLE obj_handle, guint32 evt_id, json_object* evt_msg_jso,
    gboolean ignore_ssl_cert_verify)
{
    EVT_POST_QUEUE_INFO *msg_queue_info = NULL;

    gint32 ret = create_evt_from_template(evt_msg_jso, evt_id, obj_handle, &msg_queue_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "create_evt_from_template fail, ret=%d", ret);
        return ret;
    }
    msg_queue_info->ignore_ssl_cert_verify = ignore_ssl_cert_verify;
    gboolean flag = _handle_redfish_evt_msg_queue(msg_queue_info, EVENT_ORIGIN_TYPE_SUSPEND_RETRIES);
    _free_evt_queue_msg_info(&msg_queue_info);
    return flag ? RET_OK : RET_ERR;
}


LOCAL gint32 send_resumed_event_to_subscription(TASK_MONITOR_INFO_S *monitor_data, const json_object* event_data)
{
    RESUME_TASK_MONITOR_INFO *user_data = (RESUME_TASK_MONITOR_INFO*)(monitor_data->user_data);
    json_bool ret_bool;
    json_object *sumj = NULL;
    json_object *eventsj = NULL;

    ret_bool = json_object_object_get_ex(event_data, "sum", &sumj);
    gint32 sum = json_object_get_int(sumj);
    gint32 already_submitted_count = 0;
    debug_log(DLOG_INFO, "%s: message: %d", __FUNCTION__, sum);
    if (ret_bool == FALSE) {
        debug_log(DLOG_ERROR, "%s: get event size failed", __FUNCTION__);
        return RET_ERR;
    }
    ret_bool = json_object_object_get_ex(event_data, "data", &eventsj);
    if (ret_bool == FALSE) {
        debug_log(DLOG_ERROR, "%s: get event list failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object *item;
    json_object *ssl_verify_flag_json = NULL;
    gboolean ssl_verify_flag;
    json_object_array_foreach(eventsj, item) {
        ssl_verify_flag = FALSE;
        json_object_object_get_ex(item, RFPROP_EVT_SSL_CERT_VERIFY, &ssl_verify_flag_json);
        if (ret_bool == FALSE) {
            debug_log(DLOG_ERROR, "get event %s failed", RFPROP_EVT_SSL_CERT_VERIFY);
        } else {
            ssl_verify_flag = (gboolean)json_object_get_boolean(ssl_verify_flag_json);
        }
        json_object_object_del(item, RFPROP_EVT_SSL_CERT_VERIFY);
        guint32 evt_id = increase_and_get_event_rsc_id();
        gint32 ret = post_resume_evt_for_subcription_handle(user_data->obj_handle, evt_id, item, ssl_verify_flag);
        if (ret != RET_OK) {
            user_data->number_of_failed_event++;
            debug_log(DLOG_ERROR, "%s:send message failed", __FUNCTION__);
        } else {
            user_data->number_of_successfully_send++;
        }
        already_submitted_count++;
        monitor_data->task_progress = (already_submitted_count * 90 / sum) + 10; // 这里占90的进度，前面占10的进度
        vos_task_delay(50); // 防止CPU占用率过高，每次发送等待50ms
    }
    return RET_OK;
}

LOCAL void send_resumed_evt_msg_by_date(TASK_MONITOR_INFO_S* monitor_data)
{
    RESUME_TASK_MONITOR_INFO *user_data = (RESUME_TASK_MONITOR_INFO*)(monitor_data->user_data);
    EVT_TIME_INFO time = {0};
    time.end = user_data->end_time;
    time.start = user_data->start_time;
    gint32 sub_id = user_data->sub_id;
    if (time.end < time.start) {
        debug_log(DLOG_ERROR, "end time does not less than minus time, end=%u, start=%u", time.end, time.start);
        monitor_data->task_state = RF_TASK_EXCEPTION;
        return;
    }
    debug_log(DLOG_INFO, "start = %u, end = %u", time.start, time.end);
    monitor_data->task_progress = 5; // 5%进度
    json_object* event_data = get_event_list(time, sub_id, SUSPENED_TYPE);
    if (event_data == NULL) {
        monitor_data->task_state = RF_TASK_EXCEPTION;
        debug_log(DLOG_ERROR, "%s:count not find any event info", __FUNCTION__);
        return;
    }
    monitor_data->task_progress = 10; // 10%进度
    user_data->processing_phase = SENDING;
    gint32 ret = send_resumed_event_to_subscription(monitor_data, event_data);
    json_object_put(event_data);
    if (ret != RET_OK) {
        monitor_data->task_state = RF_TASK_EXCEPTION;
        debug_log(DLOG_ERROR, "%s: sent event message failed!", __FUNCTION__);
        return;
    }
    user_data->processing_phase = SENDING_COMPLETED;
    delete_event_log(time, sub_id, SUSPENED_TYPE);
    monitor_data->task_progress = 100; // 100%进度
    monitor_data->task_state = RF_TASK_COMPLETED;
}


LOCAL TASK_MONITOR_INFO_S *new_resume_event_monitor(guint32 minus_sec, OBJ_HANDLE description, gint32 id)
{
    
    TASK_MONITOR_INFO_S *monitor_info;
    
    RESUME_TASK_MONITOR_INFO *task = g_malloc0(sizeof(RESUME_TASK_MONITOR_INFO));
    if (task == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__);
        return NULL;
    }
    task->end_time = vos_get_cur_time_stamp();
    task->start_time = task->end_time - minus_sec;
    task->obj_handle = description;
    task->sub_id = id;
    task->processing_phase = BEFORE_CREATE_THREAD;
    task->number_of_successfully_send = 0;
    task->number_of_failed_event = 0;
    monitor_info = task_monitor_info_new((GDestroyNotify)g_free);
    if (monitor_info == NULL) {
        g_free(task);
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }
    
    monitor_info->user_data = task;
    
    monitor_info->task_progress = TASK_NO_PROGRESS; 
    monitor_info->rsc_privilege = USERROLE_BASICSETTING; 
    return monitor_info;
}


LOCAL gint32 resume_subscription_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gchar thread_name[32] = {0}; 
    gchar progress_str1[RF_MAX_PROGRESS_LEN] = {0};
    gchar progress_str2[RF_MAX_PROGRESS_LEN] = {0};
    gchar progress_str3[RF_MAX_PROGRESS_LEN] = {0};
    RESUME_TASK_MONITOR_INFO *user_data = (RESUME_TASK_MONITOR_INFO *)(monitor_fn_data->user_data);
    gint32 ret;
    switch (user_data->processing_phase) {
        case BEFORE_CREATE_THREAD:
            user_data->processing_phase = BEFORE_ENQUIRY_SUM;
            (void)sprintf_s(thread_name, sizeof(thread_name), "ResumeSubscription%d", user_data->sub_id);
            ret = vos_task_create(NULL, thread_name, (TASK_ENTRY)send_resumed_evt_msg_by_date,
                (void*)monitor_fn_data, DEFAULT_PRIORITY);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:create send event task(%s) error", __FUNCTION__, thread_name);
                goto err_exit;
            }
        case BEFORE_ENQUIRY_SUM:
        case SENDING:
            (void)snprintf_s(progress_str1, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1,
                "%d", user_data->sub_id);
            (void)snprintf_s(progress_str2, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1,
                "%d", user_data->number_of_successfully_send);
            (void)snprintf_s(progress_str3, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1,
                "%d", user_data->number_of_failed_event);
            (void)create_message_info(MSGID_SUBMITTING_RESUMED_EVENT, NULL, message_obj,
                progress_str1, progress_str2, progress_str3);
            return RET_OK;
        case SENDING_COMPLETED:
            (void)snprintf_s(progress_str1, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1,
                "%d", user_data->number_of_successfully_send);
            (void)snprintf_s(progress_str2, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1,
                "%d", user_data->number_of_failed_event);
            (void)create_message_info(MSGID_SUBMIT_RESUMED_EVENT_COMPLETELY, NULL, message_obj,
                progress_str1, progress_str2);
            return RET_OK;
    }

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    return RET_OK;
}

LOCAL gint32 get_duration_value_pre_t(gchar *duration, guint32 *out)
{
#define IOS8601_DURATION_REGEX1 "^P(?:\\d+Y)?(?:\\d+M)?(?:\\d+D)?$"

    if (g_regex_match_simple(IOS8601_DURATION_REGEX1, duration, (GRegexCompileFlags)0, (GRegexMatchFlags)0) != TRUE) {
        debug_log(DLOG_ERROR, "regex check fail, string=%s", duration);
        return RET_ERR;
    }
    guint32 result = 0;
    struct {
        gchar *time_name;
        guint32 unit;
    } unit_map[] = {
        {"Y", YEAR_TO_MSEC}, {"M", MONTH_TO_MSEC}, {"D", DAY_TO_MSEC}
    };
    for (gint32 i = 0; i < ARRAY_SIZE(unit_map); i++) {
        char *p = strstr(duration, unit_map[i].time_name);
        if (p != NULL) {
            *p = '\0';
            result += strtol(p, NULL, DECIMAL_NUM) * unit_map[i].unit;
            duration = p + 1;
        }
    }
    *out = result;
    return RET_OK;
}

LOCAL gint32 get_duration_value_before_t(gchar *duration, guint32 *out)
{
#define IOS8601_DURATION_REGEX2 "^(?:\\d+H)?(?:\\d+M)?(?:\\d+S)?$"

    if (g_regex_match_simple(IOS8601_DURATION_REGEX2, duration, (GRegexCompileFlags)0, (GRegexMatchFlags)0) != TRUE) {
        debug_log(DLOG_ERROR, "regex check fail, string=%s", duration);
        return RET_ERR;
    }
    guint32 result = 0;
    struct {
        gchar *time_name;
        guint32 unit;
    } unit_map[] = {
        {"D", HOUR_TO_MSEC}, {"M", MINUTE_TO_MSEC}, {"S", 1}
    };
    for (gint32 i = 0; i < ARRAY_SIZE(unit_map); i++) {
        char *p = strstr(duration, unit_map[i].time_name);
        if (p != NULL) {
            *p = '\0';
            result += strtol(p, NULL, DECIMAL_NUM) * unit_map[i].unit;
            duration = p + 1;
        }
    }
    *out = result;
    return RET_OK;
}

LOCAL gint32 duration_time_format(const gchar *duration, guint32 *minus_sec)
{
#define IOS8601_DURATION_REGEX "^P(?:\\d+Y)?(?:\\d+M)?(?:\\d+D)?(?:T)?(?:\\d+H)?(?:\\d+M)?(?:\\d+S)?$"
    guint32 result1 = 0;
    guint32 result2 = 0;
    if (duration == NULL || duration[0] == '\0') {
        *minus_sec = vos_get_cur_time_stamp();
        return RET_OK;
    }
    if (g_regex_match_simple(IOS8601_DURATION_REGEX, duration, (GRegexCompileFlags)0, (GRegexMatchFlags)0) != TRUE) {
        debug_log(DLOG_ERROR, "regex check fail, string=%s", duration);
        return RET_ERR;
    }
    gchar **str_arr = g_strsplit(duration, "T", 2);
    if (str_arr == NULL) {
        debug_log(DLOG_ERROR, "g_strsplit fail");
        return RET_ERR;
    }
    guint arr_len = g_strv_length(str_arr);
    
    if (arr_len != 2 && arr_len != 1) {
        debug_log(DLOG_ERROR, "check fail, string=%s", duration);
        g_strfreev(str_arr);
        return RET_ERR;
    }
    gint32 ret = get_duration_value_pre_t(str_arr[0], &result1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get millsec of duration(%s) fail", duration);
        g_strfreev(str_arr);
        return ret;
    }
    
    if (arr_len == 2) {
        ret = get_duration_value_before_t(str_arr[1], &result2);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get millsec of duration(%s) fail", duration);
            g_strfreev(str_arr);
            return ret;
        }
    }
    *minus_sec = result1 + result2;
    g_strfreev(str_arr);
    return RET_OK;
}

LOCAL void resume_evt_respons_cmcc_json(json_object **o_message_jso)
{
    (void)json_object_put(*o_message_jso);
    *o_message_jso = NULL;
}


enum HTTP_RESPONSE_CODE resume_suspended_event(json_object **o_message_jso, PROVIDER_PARAS_S *i_paras,
    OBJ_HANDLE description, gint32 id)
{
    json_object *duration_json = NULL;
    guint32 minus_sec = vos_get_cur_time_stamp();
    const gchar *duration = NULL;
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_EVT_DELIVER_BUFFERED_EVT_DURATION, &duration_json);
    if (duration_json != NULL) {
        duration = json_object_get_string(duration_json);
        debug_log(DLOG_INFO, "duration: %s", duration);
        if (duration_time_format(duration, &minus_sec) != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get start time failed, duration = %s", __FUNCTION__, duration);
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_EVT_DELIVER_BUFFERED_EVT_DURATION, o_message_jso,
                duration, RFPROP_EVT_DELIVER_BUFFERED_EVT_DURATION);
            return HTTP_BAD_REQUEST;
        }
    }
    if (check_redfish_running_task_exist(resume_subscription_status_monitor, json_object_new_int(0),
        i_paras->uri, NULL)) {
        debug_log(DLOG_ERROR, "%s failed:running task exist", __FUNCTION__);
        (void)create_message_info(MSGID_SUBMITTING_RESUMED_ERR, NULL, o_message_jso);
        // 返回结果
        return HTTP_BAD_REQUEST;
    }
    TASK_MONITOR_INFO_S *resume_monitor_data = new_resume_event_monitor(minus_sec, description, id);
    if (resume_monitor_data == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc a new resume monitor info failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    gchar task_name[MAX_DES_LENGTH] = {0}; // 任务的描述
    if (snprintf_s(task_name, sizeof(task_name), sizeof(task_name) - 1, RFPROP_RESUME_TASK_DESC, id) < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
    }
    gint32 ret = create_new_task(task_name, resume_subscription_status_monitor, resume_monitor_data,
        json_object_new_int(0), i_paras->uri, o_message_jso);
    if (ret != RF_OK) {
        task_monitor_info_free(resume_monitor_data);
        debug_log(DLOG_ERROR, "%s:create new task failed %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (dal_is_customized_by_cmcc()) {
        resume_evt_respons_cmcc_json(o_message_jso);
    }
    return HTTP_OK;
}

gint32 print_description_info(GSList *input_list)
{
    OBJ_HANDLE subscription_handle;
    GSList *obj_list = NULL;
    GSList *obj_iter = NULL;
    gint32 ret = dfl_get_object_list(CLASS_EVT_SUBSCRIPTION, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get object list for %s, ret=%d", CLASS_EVT_SUBSCRIPTION, ret);
        return ret;
    }
    guint8 id;
    for (obj_iter = obj_list; obj_iter != NULL; obj_iter = obj_iter->next) {
        subscription_handle = (OBJ_HANDLE)obj_iter->data;
        ret = get_evt_subscription_id(subscription_handle, &id);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "cann't get subscription id, ret=%d", ret);
            g_slist_free(obj_list);
            return ret;
        }
        gint queue_len_current = g_async_queue_length(g_evt_msg_queue[id - 1]);
        gint queue_len_retry_forever = g_async_queue_length(g_forever_queue[id - 1]);
        debug_printf("scription(id=%u), current_queuelen=%d, retry_forever_queuelen=%d, flag=%u",
            id, queue_len_current, queue_len_retry_forever, g_evt_retry_forever_flag[id - 1]);
    }
    g_slist_free(obj_list);
    return RET_OK;
}