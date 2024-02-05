
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_memory_metrics_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memory_metrics_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_memory_metrics_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_memory_metrics_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_system_memory_metrics_oem, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 redfish_check_memory_metrics_uri_effective(const gchar *i_uri, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar device_name[MEMORY_DEVICE_LEN + 1] = {0};
    gchar memory_location[MEMORY_LOCATION_LEN + 1] = {0};
    guchar memory_onset = 0;
    gchar str_uri[MAX_URI_LENGTH] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get boad solt fail.", __FUNCTION__);
        return RET_ERR;
    }
    ret = dfl_get_object_list(CLASS_MEMORY, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get memory list fail.", __FUNCTION__);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)memset_s(device_name, sizeof(device_name), 0, sizeof(device_name));
        (void)memset_s(memory_location, sizeof(memory_location), 0, sizeof(memory_location));
        (void)memset_s(str_uri, sizeof(str_uri), 0, sizeof(str_uri));

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_DEVICENAME, device_name,
            sizeof(device_name));
        (void)dal_clear_string_blank(device_name, sizeof(device_name));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_LOCATION, memory_location,
            sizeof(memory_location));
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PRESENCE, &memory_onset);
        (void)dal_clear_string_blank(memory_location, sizeof(memory_location));

        if (strlen(device_name) == 0 || strlen(memory_location) == 0 || !g_strcmp0(device_name, MEMORY_NA)
            || memory_onset != 1) {
            continue;
        }

        ret = snprintf_s(str_uri, sizeof(str_uri), sizeof(str_uri) - 1, URI_FORMAT_SYSTEM_REMEMORY_METRICS,
            slot_id, memory_location, device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__);
            g_slist_free(obj_list);
            return RET_ERR;
        }

        if (g_ascii_strcasecmp(str_uri, i_uri) == RF_MATCH_OK) {
            *o_obj_handle = (OBJ_HANDLE)obj_node->data;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}


gint32 system_memory_metrics_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        return HTTP_NOT_FOUND;
    }
    // 判断是否支持业务系统，一般配置1，cx916、RM210这些机型没有跑业务才会配置0
    ret = redfish_get_x86_enable_type(&board_type);
    if (ret != RET_OK || board_type == DISABLE) {
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_memory_metrics_uri_effective(i_paras->uri, &i_paras->obj_handle);
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_memory_metrics_provider;
    *count = sizeof(g_system_memory_metrics_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL gint32 get_memory_metrics_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri_metrics[MAX_URI_LENGTH] = {0};
    gchar memory_location[MEMORY_LOCATION_LEN + 1] = {0};
    gchar device_name[MEMORY_NAME_LEN + 1] = {0};
    gchar uri_system_metrics[MAX_URI_LENGTH] = {0};

    // 入参检查,检查不过返回500，内部错误，不填充消息体
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK ||
        i_paras->obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 用户权限判断: 需满足只读权限，否则返回403，无权限访问
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = get_odata_id(i_paras, o_message_jso, o_result_jso, URI_FORMAT_SYSTEM);
    if (ret != HTTP_OK || *o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: get_odata_id fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    const gchar *uri_system = json_object_get_string(*o_result_jso);
    (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_MEM_DEVICENAME, device_name,
        sizeof(device_name));
    (void)dal_clear_string_blank(device_name, sizeof(device_name));
    (void)dal_get_property_value_string(i_paras->obj_handle, PROPERTY_MEM_LOCATION, memory_location,
        sizeof(memory_location));
    ret = snprintf_s(uri_metrics, sizeof(uri_metrics), sizeof(uri_metrics) - 1, URI_FORMAT_REMEMORY_METRICS,
        memory_location, device_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = snprintf_s(uri_system_metrics, sizeof(uri_system_metrics), sizeof(uri_system_metrics) - 1,
        "%s%s", uri_system, uri_metrics);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const char *)uri_system_metrics);
    return HTTP_OK;
}


gint32 get_system_memory_metrics_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = RET_OK;
    guint16 storm_cnt = 0;
    json_object *huawei_obj = NULL;

    // 入参检查,检查不过返回500，内部错误，不填充消息体
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK ||
        i_paras->obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 用户权限判断: 需满足只读权限，否则返回403，无权限访问
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_uint16(i_paras->obj_handle, PROPERTY_MEM_STORM_CNT, &storm_cnt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property StormCnt fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    huawei_obj = json_object_new_object();
    json_object_object_add(huawei_obj, PROPERTY_MEM_STORM_CNT, json_object_new_int(storm_cnt));
    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei_obj);
    return HTTP_OK;
}