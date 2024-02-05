
#include "redfish_provider.h"
#include "redfish_http.h"
#include <arpa/inet.h>

typedef gint32 (*SHIELD_CHECK_FUNC)(REQUEST_INFO_S *request_info, json_object *message_array_obj,
    gint32 *response_code);

LOCAL gint32 shiled_check_func_as_status_except_act_hmm(REQUEST_INFO_S *request_info, json_object *message_array_obj,
    gint32 *response_code);
LOCAL gint32 shiled_check_func_as_status_except_act_hmm_for_event_service(REQUEST_INFO_S *request_info,
    json_object *message_array_obj, gint32 *response_code);

LOCAL gint32 shiled_check_func_as_status(REQUEST_INFO_S *request_info, json_object *message_array_obj,
    gint32 *response_code);

typedef struct tag_shield_provider_s {
    const gchar *uri;
    SHIELD_CHECK_FUNC func; // 函数返回TRUE表示屏蔽，FALSE表示不屏蔽
} SHIELD_PROVIDER_S;

LOCAL SHIELD_PROVIDER_S shield_table[] = {
    // 从上向下匹配
    { "^/redfish/v1/managers/hmm", shiled_check_func_as_status_except_act_hmm }, // 管理板下的路径，主用板可以访问，用于透传查询备用板信息
    { "^/redfish/v1/chassis/hmm[12]$", shiled_check_func_as_status_except_act_hmm }, // 管理板下的路径，主用板可以访问，用于透传查询备用板信息
    { "^/redfish/v1/chassis/hmm[12]/(threshold|discrete)sensors$", shiled_check_func_as_status_except_act_hmm }, // 管理板下的路径，主用板可以访问，用于透传查询备用板信息
    { "^/redfish/v1/UpdateService/FirmwareInventory", shiled_check_func_as_status_except_act_hmm }, // 固件版本下的路径，主用板可以访问，用于透传查询备用板信息
    { "^/redfish/v1/EventService", shiled_check_func_as_status_except_act_hmm_for_event_service }, // 事件重传的路径，备板接收主板的透传上报备板告警
    { "^/redfish/v1.+", shiled_check_func_as_status }, 
    { NULL, NULL},
};


LOCAL gint32 shiled_check_func_as_status_except_act_hmm_for_event_service(REQUEST_INFO_S *request_info,
    json_object *message_array_obj, gint32 *response_code)
{
    guint8 slot_id = 0;
    guint32 ip_segment = 0;
    struct in_addr addr = { 0 };
    _cleanup_gfree_ gchar *smm_ip = NULL;
    *response_code = HTTP_FORBIDDEN;
    json_object *msg_obj = NULL;
    guint32 offset;
    guint8 software_type = 0;
    gint32 ret;
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM != software_type) { // 只用于MM920
        return FALSE;
    }

    
    if (g_ascii_strcasecmp(request_info->action_name, REARM_REQUEST) != 0) {
        return shiled_check_func_as_status(request_info, message_array_obj, response_code);
    }

    // 判断是否来自管理板的访问
    // 获取对板槽位号
    ret = get_other_smm_slotID(&slot_id);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get other smm slot id failed, ret %d", __func__, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(message_array_obj, msg_obj);
        *response_code = HTTP_INTERNAL_SERVER_ERROR;
        return TRUE;
    }
    // 获取内网IP网段
    ret = get_internal_ip_segment(&ip_segment);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(message_array_obj, msg_obj);
        *response_code = HTTP_INTERNAL_SERVER_ERROR;
        return TRUE;
    }

    // 获取对板ip
    offset = (guint32)(128 + slot_id);
    addr.s_addr = ip_segment + g_htonl(offset);
    smm_ip = dal_inet_ntop_ipv4(addr);
    // 判断是否管理板ip,是管理板则可以访问URI
    if (g_strcmp0(smm_ip, request_info->request_ip) == 0) {
        return FALSE;
    }
    // 否则需要判断是否是备用板，备用板不可访问
    return shiled_check_func_as_status(request_info, message_array_obj, response_code);
}


LOCAL gint32 shiled_check_func_as_status_except_act_hmm(REQUEST_INFO_S *request_info, json_object *message_array_obj,
    gint32 *response_code)
{
    guint8 slot_id = 0;
    gint32 ret;
    guint32 ip_segment = 0;
    struct in_addr addr = { 0 };
    _cleanup_gfree_ gchar *smm_ip = NULL;
    *response_code = HTTP_FORBIDDEN;
    json_object *msg_obj = NULL;
    guint32 offset;
    guint8 software_type = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 ipmb_addr = 0;

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM != software_type) { // 只用于MM920
        return FALSE;
    }
    // 判断是否来自管理板的访问
    // 获取对板槽位号
    ret = get_other_smm_slotID(&slot_id);
    goto_label_do_info_if_expr(ret != VOS_OK, EXIT,
        debug_log(DLOG_ERROR, "[%s] Get other smm slot id failed, ret %d", __func__, ret));

    // 获取内网IP网段,双平面根据与对板ipmi通道选取网段
    if (!dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        get_internal_ip_segment(&ip_segment);
    } else {
        ret = dfl_get_object_handle(IPMI_CHANNEL_AS_OBJECT, &obj_handle);
        goto_label_do_info_if_expr(ret != VOS_OK, EXIT,
            debug_log(DLOG_ERROR, "[%s] Get other smm ipmi channel obj failed, ret %d", __func__, ret));

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_IPMITRAGET_DEST_ADDR, &ipmb_addr);
        goto_label_do_info_if_expr(ret != VOS_OK, EXIT,
            debug_log(DLOG_ERROR, "[%s] Get other smm ipmi address failed, ret %d", __func__, ret));

        ret = dal_eth_get_inner_network(ipmb_addr, &ip_segment);
        if (ret != RET_OK) {
            goto EXIT;
        }
    }

    // 获取对板ip
    offset = (guint32)(slot_id + 128);
    addr.s_addr = ip_segment + g_htonl(offset);
    smm_ip = dal_inet_ntop_ipv4(addr);
    // 判断是否管理板ip,是管理板则可以访问URI
    if (g_strcmp0(smm_ip, request_info->request_ip) == 0) {
        return FALSE;
    }
    // 否则需要判断是否是备用板，备用板不可访问
    return shiled_check_func_as_status(request_info, message_array_obj, response_code);

EXIT:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
    json_object_array_add(message_array_obj, msg_obj);
    *response_code = HTTP_INTERNAL_SERVER_ERROR;
    return TRUE;
}


LOCAL gint32 shiled_check_func_as_status(REQUEST_INFO_S *request_info, json_object *message_array_obj,
    gint32 *response_code)
{
    guint8 as_status = 0;
    gint32 ret;
    OBJ_HANDLE handle = 0;
    json_object *message_info_obj = NULL;
    *response_code = HTTP_FORBIDDEN;
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM != software_type) { // 只用于MM920
        return FALSE;
    }
    ret = dfl_get_object_handle(ASM_OBJECT_NAME, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get objecct(%s) handle failed, err code %d", __func__, ASM_OBJECT_NAME, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_obj);
        json_object_array_add(message_array_obj, message_info_obj);
        *response_code = HTTP_INTERNAL_SERVER_ERROR;
        return TRUE;
    }
    ret = dal_get_property_value_byte(handle, PROPERTY_ASM_AS_STATUS, &as_status);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get property(%s) value failed, err code %d", __func__, PROPERTY_ASM_AS_STATUS, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_obj);
        json_object_array_add(message_array_obj, message_info_obj);
        *response_code = HTTP_INTERNAL_SERVER_ERROR;
        return TRUE;
    }
    if (as_status == ASM_ACTIVE) { // 主用板不屏蔽
        return FALSE;
    }
    // 生成提示消息
    (void)create_message_info(MSGID_NOT_ALLOWED_ON_STANDBY_MM, NULL, &message_info_obj);
    json_object_array_add(message_array_obj, message_info_obj);
    return TRUE;
}

LOCAL SHIELD_PROVIDER_S *find_shield_provider(gchar *uri)
{
    gint32 i;
    gchar *pattern_lower = NULL;
    GRegex *regex = NULL;
    GMatchInfo *match_info = NULL;
    gboolean is_match = 0;
    // 查找表项
    for (i = 0; shield_table[i].uri != NULL; i++) {
        pattern_lower = g_ascii_strdown(shield_table[i].uri, strlen(shield_table[i].uri));
        if (pattern_lower == NULL) {
            debug_log(DLOG_ERROR, "[%s] Call g_ascii_strdown failed, uri pattern is %s", __func__, shield_table[i].uri);
            break;
        }
        regex = g_regex_new((const gchar *)pattern_lower, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
        is_match = g_regex_match(regex, uri, (GRegexMatchFlags)0x0, &match_info);
        g_free(pattern_lower);
        if (match_info != NULL) {
            g_match_info_free(match_info);
            match_info = NULL;
        }
        if (regex != NULL) {
            g_regex_unref(regex);
            regex = NULL;
        }
        if (TRUE == is_match) { // 找到表项
            return &shield_table[i];
        }
    }
    return NULL;
}


gint32 check_uri_shield_condition(REQUEST_INFO_S *request_info, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret;
    SHIELD_PROVIDER_S *provider = NULL;
    json_object *message_array_obj = NULL;
    json_object *header_obj = NULL;
    json_object *body_obj = NULL;
    json_object *status_obj = NULL;
    json_object *response_obj = NULL;
    gint32 response_code = 0;

    provider = find_shield_provider(request_info->uri_lower);
    if (provider == NULL) { // 未找到表项，不屏蔽
        return FALSE;
    }
    message_array_obj = json_object_new_array();
    if (message_array_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json array failed", __func__);
        return TRUE;
    }
    ret = provider->func(request_info, message_array_obj, &response_code); // 检查是否屏蔽
    if (ret == FALSE) {
        (void)json_object_put(message_array_obj);
        return FALSE;
    }
    // 给出屏蔽提示
    header_obj = json_object_new_array();
    if (header_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json array failed", __func__);
        (void)json_object_put(message_array_obj);
        return TRUE;
    }
    response_obj = json_object_new_object();
    if (response_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
        (void)json_object_put(message_array_obj);
        (void)json_object_put(header_obj);
        return TRUE;
    }
    // 生成响应头
    ret = generate_common_response_headers(&header_obj, request_info->uri_lower, message_array_obj);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Generate common response headers failed, err code %d", __func__, ret);
        (void)json_object_put(message_array_obj);
        (void)json_object_put(header_obj);
        (void)json_object_put(response_obj);
        return TRUE;
    }
    json_object_object_add(response_obj, RESPONSE_HEADERS, header_obj);
    // 生成响应状态
    ret = generate_response_status(response_code, &status_obj, message_array_obj);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Generate response status failed, err code %d", __func__, ret);
        (void)json_object_put(message_array_obj);
        (void)json_object_put(response_obj);
        return TRUE;
    }
    json_object_object_add(response_obj, RESPONSE_STATUS_CODE, status_obj);
    // 生成响应体
    (void)generate_error_message_info(message_array_obj, &body_obj);
    json_object_object_add(response_obj, RESPONSE_BODY, body_obj);
    
    generate_redfish_resp_data_from_jso(response_obj, response);

    // 释放资源
    (void)json_object_put(message_array_obj);
    (void)json_object_put(response_obj);
    return TRUE;
}

