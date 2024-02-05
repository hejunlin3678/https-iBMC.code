
#include <json.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "json_patch_util.h"
#include "redfish_forward.h"
#include "curl/curl.h"
#include "redfish_class.h"


LOCAL gint32 account_nodes_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 chassis_board_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 direct_forward_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 direct_forward_master_blade_provider(REQUEST_INFO_S *request_info,
    PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 managers_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 account_nodes_collection_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object **header, json_object **body, json_object *message_array_obj);
LOCAL gint32 hmm_manager_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 hmm_eth_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 hmm_chassis_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
LOCAL gint32 hmm_firmware_inventory_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object **header, json_object **body, json_object *message_array_obj);
LOCAL gint32 forward_to_hmm_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);

LOCAL gint32 ipmi_cmd_get_redfish_token_of_target(guint8 ipmb_addr, gchar **token);
LOCAL gint32 get_target_token(guint8 ipmb_addr, gchar **token);
LOCAL gint32 delete_target_token(guint8 ipmb_addr);
LOCAL gint32 get_forward_uri(guint8 ipmb_addr, gchar *base_uri, gchar *new_uri, gsize new_uri_lenth);
LOCAL json_object *formate_header(json_object *header_obj);
LOCAL guint8 get_method_name_mask(gchar *method_name);
LOCAL gint32 check_forward_privilege(gchar *method_name, guint8 user_role_privilege, FORWARD_PROVIDER_S *provider);
LOCAL gint32 get_item_string(json_object *jso, const char *item_name, const char **str);
LOCAL gint32 hmm_event_rearm_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj);
FORWARD_PROVIDER_S forward_uri_table[] = {
    { URI_MANAGERS_BOARD,                       ALLOW_ALL,               USERROLE_READONLY, USERROLE_BASICSETTING, managers_provider },
    { URI_MANAGERS_BOARD_ETH_INTERFACES_PREFIX, ALLOW_ALL,               USERROLE_READONLY, USERROLE_BASICSETTING, direct_forward_provider },
    { URI_SYSTEMS_BOARD,                        ALLOW_ALL,               USERROLE_READONLY, USERROLE_BASICSETTING, direct_forward_provider },
    { URI_SYSTEMS_BOARD_RESET_ACTION_INFO,      ALLOW_ALL,               USERROLE_READONLY, USERROLE_READONLY,     direct_forward_provider },
    { URI_SYSTEMS_BOARD_FRU_CTRL_ACTION_INFO,   ALLOW_ALL,               USERROLE_READONLY, USERROLE_READONLY,     direct_forward_provider },
    { URI_CHASSIS_BOARD,                        ALLOW_GET | ALLOW_PATCH, USERROLE_READONLY, USERROLE_BASICSETTING, chassis_board_provider },
    { URI_CHASSIS_BOARD_POWER,                  ALLOW_ALL,               USERROLE_READONLY, USERROLE_POWERMGNT,    direct_forward_provider },
    { URI_CHASSIS_BOARD_BOARDS_MAINBOARD,       ALLOW_ALL,               USERROLE_READONLY, USERROLE_BASICSETTING, direct_forward_provider },
    { URI_ACCOUNT_NODES,                        ALLOW_ALL,               USERROLE_USERMGNT, USERROLE_USERMGNT,     account_nodes_provider },
    { URI_NODE_ACCOUNT_COLLECTION,              ALLOW_POST,              USERROLE_USERMGNT, USERROLE_USERMGNT,     account_nodes_collection_provider },
    { NULL,                                     0,                       0,                 0,                     NULL }
};

FORWARD_PROVIDER_S forward_to_hmm_table[] = {
    { URI_MANAGERS_HMM,                       ALLOW_ALL,  USERROLE_READONLY, USERROLE_BASICSETTING, hmm_manager_provider },
    { URI_MANAGERS_HMM_ETH_INTERFACES_PREFIX, ALLOW_ALL,  USERROLE_READONLY, USERROLE_BASICSETTING, hmm_eth_provider },
    { URI_CHASSIS_HMM,                        ALLOW_ALL,  USERROLE_READONLY, USERROLE_BASICSETTING, hmm_chassis_provider },
    { URI_CHASSIS_HMM_SENSORS,                ALLOW_GET,  USERROLE_READONLY, USERROLE_BASICSETTING, hmm_chassis_provider },
    { URI_FIRMWARE_INVENTORY,                 ALLOW_ALL,  USERROLE_READONLY, USERROLE_BASICSETTING, hmm_firmware_inventory_provider },
    { URI_EVENT_REARM,                        ALLOW_POST, USERROLE_READONLY, USERROLE_BASICSETTING, hmm_event_rearm_provider },
#ifdef ARM64_HI1711_ENABLED
    {URI_MANAGERS_HMM_ETH_PREFIX, ALLOW_ALL, USERROLE_READONLY, USERROLE_BASICSETTING, hmm_eth_provider},
    {URI_MANAGERS_HMM_ETH_STATIC_PREFIX, ALLOW_ALL, USERROLE_READONLY, USERROLE_BASICSETTING, hmm_eth_provider},
    {URI_MANAGERS_HMM_ETH_VLAN_PREFIX, ALLOW_ALL, USERROLE_READONLY, USERROLE_BASICSETTING, hmm_eth_provider},
#endif
    {NULL, 0, 0, 0, NULL}
};

FORWARD_PROVIDER_S g_forward_to_master_table[] = {
    { URI_FORWARD_CHASSIS_ENC,                                     ALLOW_ALL,
        USERROLE_READONLY, USERROLE_BASICSETTING,   direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENC_THERMAL,                             ALLOW_ALL,
        USERROLE_READONLY, USERROLE_BASICSETTING,   direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENC_POWER,                               ALLOW_ALL,
        USERROLE_READONLY, USERROLE_POWERMGNT,      direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENC_LEDGROUP,                            ALLOW_ALL,
        USERROLE_READONLY, USERROLE_BASICSETTING,   direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENCLOSURE,                               ALLOW_ALL,
        USERROLE_READONLY, USERROLE_BASICSETTING,   direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENCLOSURE_THERMAL,                       ALLOW_ALL,
        USERROLE_READONLY, USERROLE_BASICSETTING,   direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENCLOSURE_POWER,                         ALLOW_ALL,
        USERROLE_READONLY, USERROLE_POWERMGNT,      direct_forward_master_blade_provider},
    { URI_FORWARD_CHASSIS_ENCLOSURE_HDD_DRAWER,                    ALLOW_ALL,
        USERROLE_READONLY, USERROLE_POWERMGNT,      direct_forward_master_blade_provider},
    { NULL, 0, 0, 0, NULL }
};

LOCAL GHashTable *g_target_token_table = NULL;
LOCAL GMutex g_target_token_table_lock;

LOCAL gint32 ipmi_cmd_get_redfish_token_of_target(guint8 ipmb_addr, gchar **token)
{
    errno_t safe_fun_ret;
    const guint8 _IPMI_OEM_GET_TARGET_TOKEN = 0x39;
    const guint8 ADMINISTRATOR = 4;
    const guint8 REDFISH_SESSION = 1;
    const guint8 LOCAL_USER = 0;
    const guint8 IPV4 = 0;
    IPMI_REQ_MSG_HEAD_S head;
    guint8 cmd[28] = { 0 };
    gpointer response = NULL;
    guint32 response_data_len;
    const guint8 *response_data = NULL;
    gint32 ret;
    guint32 iana_id = HUAWEI_IANA;

    return_val_if_expr(token == NULL, VOS_ERR);

    (void)memset_s(&head, sizeof(head), 0, sizeof(head));
    ret = fill_ipmi_target_info(ipmb_addr, &head);

    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s] Get ipmi target info failed, ipmb[%u], err code %d", __func__, ipmb_addr, ret));

    head.lun = 0;
    head.netfn = NETFN_OEM_REQ;
    head.cmd = IPMI_OEM_SHMC_COMMAND;
    head.src_len = sizeof(cmd);
    cmd[0] = (guint8)(iana_id & 0xff);
    cmd[1] = (guint8)((iana_id >> 8) & 0xff);
    cmd[2] = (guint8)((iana_id >> 16) & 0xff);
    cmd[3] = _IPMI_OEM_GET_TARGET_TOKEN;
    cmd[4] = ADMINISTRATOR;   // Role字段，固定为4，即管理员
    cmd[5] = REDFISH_SESSION; // 会话类型，0表示Web，1表示Redfish
    cmd[6] = LOCAL_USER;      // 用户类型，0表示本地用户，1表示域用户
    cmd[7] = IPV4;            // IP协议，当前固定为IPv4格式
    cmd[8] = 10;              // 0x0a
    cmd[9] = 10;              // 0x0a
    cmd[10] = 1;              // 0x01
    cmd[27] = 17;             // 0x11
    cmd[11] = 103;            // 0x67

    ret = ipmi_send_request(&head, (gconstpointer)cmd, MAX_POSSIBLE_IPMI_FRAME_LEN, &response, TRUE);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s] Send ipmi failed, err code %d", __func__, ret));

    response_data = get_ipmi_src_data(response);

    return_val_do_info_if_expr(response_data == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s] Ipmi response data is null", __func__);
        g_free(response));

    if (VOS_OK != response_data[0]) {
        ret = response_data[0];
        debug_log(DLOG_INFO, "[%s] Ipmi response complete code is not ok, ret is %u", __func__,
            response_data[0]); // 部分单板不支持时，不刷日志
        g_free(response);
        return ret;
    }

    return_val_do_info_if_expr(dal_check_ipmi_req_huawei_iana(1 + response_data, MANUFAC_ID_LEN) == FALSE, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s] Ipmi response oem code error, ret is %u %u %u", __func__, response_data[1],
        response_data[2], response_data[3]);
        g_free(response));

    response_data_len = get_ipmi_src_data_len(response);

    return_val_do_info_if_expr(response_data_len <= 4, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s] Ipmi response length is not enough, expect > 4, acture is %u", __func__,
        response_data_len);
        g_free(response));

    *token = (gchar *)g_malloc0(response_data_len - 4 + 1);

    return_val_do_info_if_expr(*token == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "[%s] Malloc failed, size %u", __func__, response_data_len - 4 + 1);
        g_free(response));

    safe_fun_ret = memcpy_s(*token, response_data_len - 4 + 1, response_data + 4, response_data_len - 4);
    return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        g_free(response); g_free(*token); *token = NULL);
    g_free(response);
    return VOS_OK;
}

LOCAL gint32 get_target_token(guint8 ipmb_addr, gchar **token)
{
    gchar *token_tmp = NULL;
    gint32 ret = 0;
    
    g_mutex_lock(&g_target_token_table_lock);
    if (g_target_token_table == NULL) {
        g_target_token_table = g_hash_table_new(NULL, NULL);
        if (g_target_token_table == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new hash table failed", __func__);
            g_mutex_unlock(&g_target_token_table_lock);
            return VOS_ERR;
        }
    }
    if (g_hash_table_contains(g_target_token_table, UINT8_TO_POINTER(ipmb_addr))) {
        token_tmp = (gchar *)g_hash_table_lookup(g_target_token_table, UINT8_TO_POINTER(ipmb_addr));
    }
    
    if (token_tmp == NULL) {
        ret = ipmi_cmd_get_redfish_token_of_target(ipmb_addr, &token_tmp);
        if (ret != VOS_OK) {
            debug_log(DLOG_INFO, "[%s] Get target(ipmb addr : %u) token_tmp by ipmi failed", __func__,
                ipmb_addr); // INFO级别防止刷日志
            g_mutex_unlock(&g_target_token_table_lock);
            return ret;
        }
        g_hash_table_insert(g_target_token_table, UINT8_TO_POINTER(ipmb_addr), (gpointer)token_tmp);
    }
    g_mutex_unlock(&g_target_token_table_lock);

    *token = token_tmp;
    return ret;
}

LOCAL gint32 delete_target_token(guint8 ipmb_addr)
{
    gchar *token = NULL;
    
    g_mutex_lock(&g_target_token_table_lock);
    if (g_target_token_table == NULL) {
        g_target_token_table = g_hash_table_new(NULL, NULL);
        if (g_target_token_table == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new hash table failed", __func__);
            g_mutex_unlock(&g_target_token_table_lock);
            return VOS_ERR;
        }
    }
    if (!g_hash_table_contains(g_target_token_table, UINT8_TO_POINTER(ipmb_addr))) {
        g_mutex_unlock(&g_target_token_table_lock);
        return VOS_OK;
    }
    
    token = (gchar *)g_hash_table_lookup(g_target_token_table, UINT8_TO_POINTER(ipmb_addr));
    if (token != NULL) {
        (void)memset_s(token, strlen(token), 0, strlen(token));
        g_free(token);
    }
    (void)g_hash_table_remove(g_target_token_table, UINT8_TO_POINTER(ipmb_addr));
    g_mutex_unlock(&g_target_token_table_lock);
    return VOS_OK;
}

LOCAL gint32 get_forward_uri(guint8 ipmb_addr, gchar *base_uri, gchar *new_uri, gsize new_uri_lenth)
{
    int iRet;
    gchar ip[MAX_IPADDR_LEN];
    gint32 ret;

    ret = get_board_internal_ip(ipmb_addr, ip, MAX_IPADDR_LEN);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get board(ipmb %u) ip failed, err code %d", __func__, ipmb_addr, ret);
        return VOS_ERR;
    }
    if (new_uri_lenth <= 1) {
        debug_log(DLOG_ERROR, "[%s] Input new uri buffer length is %" G_GSIZE_FORMAT ", not long enough", __func__,
            new_uri_lenth);
        return VOS_ERR;
    }
    iRet = snprintf_s(new_uri, new_uri_lenth, new_uri_lenth - 1, "https://%s:40443%s", ip, base_uri);
    return_val_do_info_if_expr(iRet <= 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    return VOS_OK;
}


void format_account_body(json_object *body, const gchar *uid_str, const gchar *slot_id_str)
{
    int iRet;
    gchar odata_id[MAX_URI_LEN + 1] = { 0 };
    json_object *obj = NULL;
    gchar id[STRING_LEN_MAX + 1] = { 0 };

    return_do_info_if_fail(body && uid_str && slot_id_str, debug_log(DLOG_ERROR, "[%s] Input param error", __func__););

    // 重新设置信息
    // @odata.context
    obj = json_object_new_string("/redfish/v1/$metadata#AccountService/Oem/Huawei/Nodes/Members/$entity");
    json_object_object_add(body, RFPROP_ODATA_CONTEXT, obj);
    iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
        "/redfish/v1/AccountService/Oem/Huawei/Nodes/%s_Slot%s", uid_str, slot_id_str);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    // @odata.id
    obj = json_object_new_string((const gchar *)odata_id);
    json_object_object_add(body, RFPROP_ODATA_ID, obj);
    // @odata.type
    obj = json_object_new_string("#Node.v1_0_2.Node");
    json_object_object_add(body, RFPROP_ODATA_TYPE, obj);
    // Id
    iRet = snprintf_s(id, sizeof(id), sizeof(id) - 1, "%s_Slot%s", uid_str, slot_id_str);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    obj = json_object_new_string((const gchar *)id);
    json_object_object_add(body, RFPROP_ID, obj);
    // Name
    obj = json_object_new_string("Node User");
    json_object_object_add(body, "Name", obj);
    // Slot
    obj = json_object_new_string(slot_id_str);
    json_object_object_add(body, "Slot", obj);

    // 删除多余信息
    if (json_object_object_get_ex(body, RFPROP_OEM, &obj) && json_object_object_get_ex(obj, RFPROP_OEM_HUAWEI, &obj)) {
        json_object_object_del(obj, RFPROP_ACTIONS);
    }
    json_object_object_del(body, RFPROP_LINKS); // MM920不显示连接信息
}


LOCAL gint32 account_nodes_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    int iRet;
    errno_t safe_fun_ret;
    guint8 slot_id;
    gchar *info_str_start = NULL;
    gchar *info_str_end = NULL;
    gchar uid_str[MAX_USER_ID_STR_LEN + 1] = { 0 };
    gchar slot_id_str[MAX_SLOT_ID_STR_LEN + 1] = { 0 };
    guint8 ipmb_addr;
    gint32 ret;
    json_object *message_obj = NULL;
    gint32 value_int;
    
    
    info_str_start = g_strrstr(request_info->uri_lower, "/"); // 指向 /N_SlotM
    if (info_str_start == NULL) {
        debug_log(DLOG_ERROR, "Get user id and slot id in uri failed");
        create_message_info(MSGID_INTERNAL_ERR, NULL, &message_obj);
        json_object_array_add(message_array_obj, message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    info_str_start++; // 指向 N_SlotM
    info_str_end = g_strstr_len(info_str_start, -1, "_");
    if (info_str_end == NULL) {
        create_message_info(MSGID_INTERNAL_ERR, NULL, &message_obj);
        json_object_array_add(message_array_obj, message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    safe_fun_ret = strncpy_s(uid_str, sizeof(uid_str), info_str_start, info_str_end - info_str_start); // UID
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    info_str_start = info_str_end + 5; // 指向 M
    value_int = arith_str_to_int(info_str_start);
    if (value_int > 38 || value_int <= 0) {
        create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_obj, request_info->relative_uri);
        json_object_array_add(message_array_obj, message_obj);
        return HTTP_NOT_FOUND;
    }
    safe_fun_ret = strncpy_s(slot_id_str, sizeof(slot_id_str), info_str_start,
        sizeof(slot_id_str) - 1); // Slot id
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    slot_id = value_int;
    
    iRet = snprintf_s(request_info->uri_lower, sizeof(request_info->uri_lower), sizeof(request_info->uri_lower) - 1,
        "/redfish/v1/accountservice/accounts/%s", uid_str);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    ipmb_addr = (slot_id + 0x40) * 2;
    
    ret = send_redfish_request_to_target(ipmb_addr, i_param, request_info, header, body, message_array_obj);
    if (ret / HTTP_OK != 1) {
        if (ret == HTTP_NOT_FOUND) { // 远端资源未找到，响应中包含URI需要修改为SMM上的URI
            json_object_put(*body);
            *body = NULL;
            create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_obj, request_info->relative_uri);
            json_object_array_add(message_array_obj, message_obj);
        }
        return ret;
    }
    
    if (*body != NULL && strcasecmp(request_info->request_method, HTTP_DELETE) != 0) { // 存在返回值，且请求是GET和PATCH
        format_account_body(*body, uid_str, slot_id_str);
    }
    return ret;
}


gint32 add_blade_system_name(guint8 ipmb, json_object *body)
{
    errno_t safe_fun_ret = EOK;
    const gchar *model_str = NULL;
    gchar model_name[MAX_STRBUF_LEN] = { 0 };
    gint32 ret;
    OBJ_HANDLE handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    if (body == NULL) {
        return VOS_ERR;
    }
    // 获取单板句柄
    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, ipmb, &handle);
    if (ret != VOS_OK) {
        json_object_object_add(body, "Model", NULL);
        debug_log(DLOG_ERROR, "[%s] Get class[%s] object[%d] handle failed, err code %d", __func__,
            CLASS_NAME_IPMBETH_BLADE, 0, ret);
        return ret;
    }
    // 获取单板信息
    input_list = g_slist_append(input_list, g_variant_new_byte(ipmb));
    ret = dfl_call_class_method(handle, METHOD_SHELF_GET_BLADE_INFO, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != VOS_OK) {
        json_object_object_add(body, "Model", NULL);
        debug_log(DLOG_INFO, "[%s] Call class[%s] method[%s] failed, err code %d", __func__, CLASS_NAME_IPMBETH_BLADE,
            METHOD_SHELF_GET_BLADE_INFO, ret);
        return ret;
    }
    model_str = g_variant_get_bytestring((GVariant *)g_slist_nth_data(output_list, 0));
    if (model_str != NULL) {
        
        safe_fun_ret = strncpy_s(model_name, sizeof(model_name), model_str, strlen(model_str));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        (void)g_strstrip((gchar *)model_name);
        
    }
    json_object_object_add(body, "Model", json_object_new_string((const gchar *)model_name));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return VOS_OK;
}


gint32 convert_width_height(guint8 size, json_object *json_obj)
{
    json_object *width_jso = NULL;
    json_object *height_jso = NULL;

    if (json_obj == NULL) {
        return VOS_ERR;
    }
    switch (size) { // 刀片大小按其所占用的槽位信息个数来区别
        case 1:
            width_jso = json_object_new_int(1);
            height_jso = json_object_new_int(1);
            break;
        case 2:
            width_jso = json_object_new_int(1);
            height_jso = json_object_new_int(2);
            break;
        case 4:
            width_jso = json_object_new_int(2);
            height_jso = json_object_new_int(2);
            break;
        default:
            break;
    }
    // 添加到结果中
    json_object_object_add(json_obj, RFPROP_OVERVIEW_WIDTH, width_jso);
    json_object_object_add(json_obj, RFPROP_OVERVIEW_HEIGHT, height_jso);
    return VOS_OK;
}


gint32 add_blade_size_to_obj(const gchar *blade_name, json_object *json_obj)
{
    gint32 ret;
    guint8 size = 0;
    gchar *blade_name_lower = NULL;
    guint8 slot_id;
    OBJ_HANDLE blade_handle = 0;

    if (json_obj == NULL || blade_name == NULL) {
        return VOS_ERR;
    }
    // 获取单板尺寸
    // 交换板
    if (g_ascii_strncasecmp(blade_name, "swi", 3) == 0) {
        json_object_object_add(json_obj, RFPROP_OVERVIEW_WIDTH, json_object_new_int(1));
        json_object_object_add(json_obj, RFPROP_OVERVIEW_HEIGHT, json_object_new_int(1));
        return VOS_OK;
    }
    blade_name_lower = g_ascii_strdown(blade_name, strlen(blade_name));
    if (NULL == blade_name_lower) {
        debug_log(DLOG_ERROR, "[%s] Convert bladename(%s) to lower failed", __func__, blade_name);
        return VOS_ERR;
    }

    slot_id = get_blade_num(blade_name_lower);
    g_free(blade_name_lower);
    if (INVALID_DATA_BYTE == slot_id) {
        debug_log(DLOG_ERROR, "[%s] get_blade_num failed, blade_name:%s", __func__, blade_name);
        return VOS_ERR;
    }
    ret = dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, (slot_id + 0x40) * 2,
        &blade_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "[%s]dal_get_specific_object_byte(IpmbEthBlade.Id=%d) failed.ret:%d", __func__, slot_id,
            ret);
        return VOS_ERR;
    }
    (void)dal_get_property_value_byte(blade_handle, PROPERTY_IPMBETH_BOARD_WIDTH_TYPE, &size);
    return convert_width_height(size, json_obj);
}


gint32 add_blade_size(const gchar *blade_name, json_object *body)
{
    json_object *oem = NULL;
    json_object *huawei = NULL;

    if (body == NULL || blade_name == NULL) {
        return VOS_ERR;
    }
    (void)json_object_object_get_ex(body, RFPROP_OEM, &oem);
    (void)json_object_object_get_ex(oem, RFPROP_OEM_HUAWEI, &huawei);
    if (oem == NULL) { // 无OEM
        oem = json_object_new_object();
        json_object_object_add(body, RFPROP_OEM, oem);
    }
    if (huawei == NULL) {
        huawei = json_object_new_object();
        json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
    }

    return add_blade_size_to_obj(blade_name, huawei);
}


LOCAL gint32 chassis_board_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    guint8 ipmb_addr = 0;
    gchar blade_name[MAX_NAME_LEN] = { 0 };
    json_object *tmp_message_obj = NULL;
    guint8 presence = 0;

    ret = get_blade_name_from_uri(request_info->uri_lower, blade_name, sizeof(blade_name),
        message_array_obj); // 获取单板名
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get blade name from uri failed. ret=%d\n", __FUNCTION__, ret));

    ret = get_board_slave_addr(blade_name, &ipmb_addr); // 获取单板ipmb地址
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade(%s) ipmb addr failed, err code %d", __func__, blade_name, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = get_board_presence(ipmb_addr, &presence); // 获取在位信息
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade(%s) presence failed, err code %d", __func__, blade_name, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (presence != BLADE_PRESENCE // 获取不在位单板时返回默认信息
        && g_ascii_strcasecmp(request_info->request_method, HTTP_GET) == 0) {
        return HANDLE_LOCAL;
    }
    ret = direct_forward_provider(request_info, i_param, header, body, message_array_obj);
    if (ret / HTTP_OK != 1) { // 获取失败时，填充默认信息
        if (g_ascii_strcasecmp(request_info->request_method, HTTP_GET) == 0) {
            if (*header != NULL) {
                json_object_put(*header);
            }
            if (*body != NULL) {
                json_object_put(*body);
            }
            return HANDLE_LOCAL;
        }
        return ret;
    }
    ret = add_blade_system_name(ipmb_addr, *body); // 通过ipmi获取单板硬件型号
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: add blade system name failed. ret=%d\n", __FUNCTION__, ret));
    ret = add_blade_size(blade_name, *body); // 通过ipmi获取单板尺寸
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: add blade size failed. ret=%d\n", __FUNCTION__, ret));
    return HTTP_OK;
}


LOCAL gint32 get_item_string(json_object *jso, const char *item_name, const char **str)
{
    json_object *item = NULL;
    gint32 ret;

    ret = json_object_object_get_ex(jso, item_name, &item);
    if (!ret) {
        return VOS_ERR;
    }
    *str = dal_json_object_get_str(item);
    return VOS_OK;
}


LOCAL gint32 account_nodes_collection_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object **header, json_object **body, json_object *message_array_obj)
{
    json_object *user_name_obj = NULL;
    json_object *password_obj = NULL;
    json_object *role_id_obj = NULL;
    json_object *slot_id_obj = NULL;
    json_object *account_locked_obj = NULL;
    json_object *account_enabled_obj = NULL;
    const gchar *uri = NULL;
    const gchar *request_body_str = NULL;
    json_object *message_info_jso = NULL;
    const gchar *member_id_str = NULL;
    json_object *body_jso_checked = NULL;
    gchar odata_id[MAX_URI_LEN] = { 0 };
    gchar *odata_type = "#Node.v1_0_2.Node";
    gchar user_id[STRING_LEN_MAX] = { 0 };
    
    // 校验输入参数
    gint32 ret = redfish_post_format_check("Node.v1_0_2.Node", request_info->request_body, NULL, &body_jso_checked,
        message_array_obj);
    if (!ret) {
        debug_log(DLOG_ERROR, "[%s] Redfish post format check failed", __func__);
        clear_sensitive_info(request_info->request_body, sizeof(request_info->request_body));
        return HTTP_BAD_REQUEST;
    }
    // 获取用户名
    ret = json_object_object_get_ex(body_jso_checked, RFPROP_ACCOUNT_USERNAME, &user_name_obj);
    if (!ret) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_ACCOUNT_USERNAME, &message_info_jso,
            RFPROP_ACCOUNT_USERNAME);
        goto exit;
    }
    // 获取密码
    ret = json_object_object_get_ex(body_jso_checked, RFPROP_ACCOUNT_PASSWORD, &password_obj);
    if (!ret) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_ACCOUNT_PASSWORD, &message_info_jso,
            RFPROP_ACCOUNT_PASSWORD);
        goto exit;
    }
    // 获取角色
    ret = json_object_object_get_ex(body_jso_checked, RFPROP_ACCOUNT_ROLE_ID, &role_id_obj);
    if (!ret) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_ACCOUNT_ROLE_ID, &message_info_jso,
            RFPROP_ACCOUNT_ROLE_ID);
        goto exit;
    }
    // 获取Slot值
    ret = json_object_object_get_ex(body_jso_checked, "Slot", &slot_id_obj);
    if (!ret) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, "Slot", &message_info_jso, "Slot");
        goto exit;
    }
    // 校验Slot值
    gint32 value_int = arith_str_to_int(dal_json_object_get_str(slot_id_obj));
    if (value_int <= 0 || value_int > 38) {
        create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, "Slot", &message_info_jso,
            dal_json_object_get_str(slot_id_obj), "Slot", "1", "38");
        goto exit;
    }
    guint8 slot_id = value_int;
    json_object_get(user_name_obj);
    json_object_get(role_id_obj);
    json_object_get(slot_id_obj);
    json_object_object_del(body_jso_checked, "Slot");
    // 获取ipmb地址
    guint8 ipmb_addr = get_blade_ipmbaddr(slot_id);
    // 构造请求头
    (void)strncpy_s(request_info->request_method, sizeof(request_info->request_method), HTTP_POST, strlen(HTTP_POST));
    uri = "/redfish/v1/AccountService/Accounts";
    (void)strncpy_s(request_info->uri_lower, sizeof(request_info->uri_lower), uri, strlen(uri));
    request_body_str = dal_json_object_get_str(body_jso_checked);
    if (request_body_str == NULL) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, NULL, &message_info_jso, NULL);
        debug_log(DLOG_ERROR, "%s: json body is null", __FUNCTION__);
        goto exit;
    }
    (void)strncpy_s(request_info->request_body, sizeof(request_info->request_body), request_body_str,
        strlen(request_body_str));
    // 清除密码信息
    json_object_clear_string(body_jso_checked, RFPROP_ACCOUNT_PASSWORD);
    json_object_object_del(body_jso_checked, RFPROP_ACCOUNT_PASSWORD);
    json_object_put(body_jso_checked);
    // 透传发送创建用户请求
    ret = send_redfish_request_to_target(ipmb_addr, i_param, request_info, header, body, message_array_obj);
    // 清除密码信息
    clear_sensitive_info(request_info->request_body, sizeof(request_info->request_body));
    if (ret / HTTP_OK != 1) {
        json_object_put(user_name_obj);
        json_object_put(role_id_obj);
        json_object_put(slot_id_obj);
        return ret;
    }

    
    // 获取锁定状态
    gint32 ret_val = json_object_object_get_ex(*body, RFPROP_ACCOUNT_LOCKED, &account_locked_obj);
    if (!ret_val) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_ACCOUNT_LOCKED, &message_info_jso,
            RFPROP_ACCOUNT_LOCKED);
        goto exit1;
    }
    // 获取使能状态
    ret_val = json_object_object_get_ex(*body, RFPROP_ACCOUNT_ENABLED, &account_enabled_obj);
    if (!ret_val) {
        create_message_info(MSGID_CREATE_FAILED_REQ_PROP, RFPROP_ACCOUNT_ENABLED, &message_info_jso,
            RFPROP_ACCOUNT_ENABLED);
        goto exit1;
    }

    json_bool bool_account_locked = json_object_get_boolean(account_locked_obj);
    json_bool bool_account_enabled = json_object_get_boolean(account_enabled_obj);

    json_object_put(*body);
    *body = NULL;

    
    get_item_string(*header, "Location", &member_id_str); // Location: /redfish/v1/xxx/xx/id
    if (member_id_str == NULL) {
        debug_log(DLOG_ERROR, "[%s] Get location in header received from blade(%u) failed", __func__, slot_id);
        json_object_put(user_name_obj);
        json_object_put(role_id_obj);
        json_object_put(slot_id_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    while (*member_id_str != '\0') {
        member_id_str++;
    }
    while (*member_id_str != '/') {
        member_id_str--;
    }
    member_id_str++;
    ret_val = snprintf_s(user_id, sizeof(user_id), sizeof(user_id) - 1, "%s_Slot%u", member_id_str, slot_id);
    do_val_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret_val));
    ret_val = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
        "/redfish/v1/AccountService/Oem/Huawei/Nodes/%s", user_id);
    do_val_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret_val));
    
    *body = json_object_new_object();
    json_object_object_add(*body, RFPROP_ODATA_CONTEXT,
        json_object_new_string("/redfish/v1/$metadata#/AccountService/Oem/Huawei/Nodes/Members/$entuty"));
    json_object_object_add(*body, RFPROP_ODATA_ID, json_object_new_string((const gchar *)odata_id));
    json_object_object_add(*body, RFPROP_ODATA_TYPE, json_object_new_string(odata_type));
    json_object_object_add(*body, RFPROP_ID, json_object_new_string((const gchar *)user_id));
    json_object_object_add(*body, RFPROP_NAME, json_object_new_string("Node User"));
    json_object_object_add(*body, RFPROP_ACCOUNT_USERNAME, user_name_obj);
    json_object_object_add(*body, RFPROP_ACCOUNT_PASSWORD, NULL);
    json_object_object_add(*body, RFPROP_ACCOUNT_ROLE_ID, role_id_obj);
    json_object_object_add(*body, "Slot", slot_id_obj);
    json_object_object_add(*body, RFPROP_ACCOUNT_LOCKED, json_object_new_boolean(bool_account_locked));
    json_object_object_add(*body, RFPROP_ACCOUNT_ENABLED, json_object_new_boolean(bool_account_enabled));

    return ret;
exit1:
    json_object_array_add(message_array_obj, message_info_jso);
    json_object_put(user_name_obj);
    json_object_put(role_id_obj);
    json_object_put(slot_id_obj);
    json_object_put(*body);
    *body = NULL;
    return HTTP_BAD_REQUEST;

exit:
    // 清除密码信息
    json_object_array_add(message_array_obj, message_info_jso);
    json_object_clear_string(body_jso_checked, RFPROP_ACCOUNT_PASSWORD);
    json_object_put(body_jso_checked);
    clear_sensitive_info(request_info->request_body, sizeof(request_info->request_body));
    return HTTP_BAD_REQUEST;
}


LOCAL gint32 hmm_manager_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    json_object *msg_jso = NULL;
    guint32 presence = 0;

    // 不能设置备用板信息
    if (g_ascii_strcasecmp(request_info->request_method, HTTP_PATCH) == 0) {
        (void)create_message_info(MSGID_CURRENT_MM_NOT_ACTIVE, NULL, &msg_jso);
        json_object_array_add(message_array_obj, msg_jso);
        debug_log(DLOG_ERROR, "%s:, the standby board does not support patch operation.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }
    if (g_ascii_strcasecmp(request_info->request_method, HTTP_ACTION) == 0) { // 动作请求,权限检查
        if (g_strcmp0(request_info->action_name + strlen("actions"), "/Oem/Huawei/Manager.Dump") == 0 &&
            (i_param->user_role_privilege & USERROLE_SECURITYMGNT) == 0) { // 无信息收集权限
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &msg_jso);
            json_object_array_add(message_array_obj, msg_jso);
            return HTTP_FORBIDDEN;
        }
    }
    ret = get_other_board_present(&presence);
    if (ret != VOS_OK || presence != BLADE_PRESENCE) { // 对板不在位时，由本板处理其请求
        return HANDLE_LOCAL;
    }
    // 透传到备板
    ret = forward_to_hmm_provider(request_info, i_param, header, body, message_array_obj);
    return ret;
}


LOCAL gint32 hmm_eth_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    guint32 presence = 0;

    // 对板在位检查
    ret = get_other_board_present(&presence);
    if (ret != VOS_OK || presence != BLADE_PRESENCE) { // 对板不在位时，本板进行处理
        return HANDLE_LOCAL;
    }
    // 透传到备板
    ret = forward_to_hmm_provider(request_info, i_param, header, body, message_array_obj);
    return ret;
}


LOCAL gint32 hmm_chassis_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    guint32 presence = 0;

    // 对板在位检查
    ret = get_other_board_present(&presence);
    if (ret != VOS_OK || presence != BLADE_PRESENCE) { // 对板不在位时，本板进行处理
        return HANDLE_LOCAL;
    }
    // 透传到备板
    ret = forward_to_hmm_provider(request_info, i_param, header, body, message_array_obj);
    return ret;
}


LOCAL gint32 hmm_event_rearm_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    guint32 presence = 0;

    // 对板在位检查
    ret = get_other_board_present(&presence);
    if (ret != VOS_OK || presence != BLADE_PRESENCE) { // 对板不在位时，本板进行处理
        debug_log(DLOG_DEBUG, "%s: forward event rearm to HMM standby failed,the standby board is not present",
            __FUNCTION__);
        return HANDLE_LOCAL;
    }

    if (g_ascii_strcasecmp(request_info->action_name, REARM_REQUEST) != 0) {
        return HANDLE_LOCAL;
    }
    // 透传到备板

    ret = forward_to_hmm_provider(request_info, i_param, header, body, message_array_obj);
    if (HTTP_OK != ret) {
        debug_log(DLOG_ERROR, "%s: forward event rearm to HMM standby failed,ret=%d", __FUNCTION__, ret);
    }

    return HANDLE_LOCAL;
}

LOCAL gint32 hmm_firmware_inventory_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object **header, json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    guint32 presence = 0;
    gchar *firmware_id = NULL;
    guint8 smm_index = 0;
    // 对板在位检查
    ret = get_other_board_present(&presence);
    if (ret != VOS_OK || presence != BLADE_PRESENCE) { // 对板不在位时，本板进行处理
        return HANDLE_LOCAL;
    }
    // 查看是否访问对板的固件
    firmware_id = g_strrstr(request_info->uri_lower, "/");
    if (firmware_id == NULL) {
        return HANDLE_LOCAL;
    }
    if (g_ascii_strncasecmp(firmware_id, "/HMM", 4) != 0) {
        return HANDLE_LOCAL;
    }
    get_smm_index(&smm_index);
    if (smm_index == firmware_id[4] - '0') { // 访问本板固件
        return HANDLE_LOCAL;
    }
    // 透传到备板
    ret = forward_to_hmm_provider(request_info, i_param, header, body, message_array_obj);
    return ret;
}


LOCAL gint32 forward_to_hmm_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gint32 ret;
    guint8 ipmb_addr = 0;
    json_object *tmp_message_obj = NULL;

    ret = get_other_smm_slave_addr(&ipmb_addr);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get other smm slave addr failed, ret %d", __func__, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = send_redfish_request_to_target(ipmb_addr, i_param, request_info, header, body, message_array_obj);
    return ret;
}


gint32 get_blade_name_from_uri(gchar *uri, gchar *blade_name, gsize buff_len, json_object *message_array_obj)
{
    errno_t safe_fun_ret;
    
    gchar *pblade_start = NULL;
    gchar *pblade_end = NULL;
    json_object *tmp_message_obj = NULL;
    pblade_start = g_strstr_len(uri + strlen("/redfish/v1/"), -1, "/");
    if (pblade_start == NULL) {
        create_message_info(MSGID_RSC_MISSING_URI, NULL, &tmp_message_obj, (const gchar*)uri);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_NOT_FOUND;
    }
    pblade_start++; // 指向单板名
    // 找到单板名结束位置
    pblade_end = g_strstr_len(pblade_start, -1, "/"); // 单板名后是字符 '/'
    if (pblade_end == NULL) {                         // 单板名后是字符 '\0'
        safe_fun_ret = strncpy_s(blade_name, buff_len, pblade_start, buff_len - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else { // 单板名后是字符 '/'
        
        safe_fun_ret = strncpy_s(blade_name, buff_len, pblade_start, pblade_end - pblade_start);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }
    return VOS_OK;
}


LOCAL gint32 direct_forward_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gchar blade_name[MAX_NAME_SIZE] = { 0 };
    guint8 ipmb_addr = 0;
    gint32 ret;
    json_object *tmp_message_obj = NULL;
    ret = get_blade_name_from_uri(request_info->uri_lower, blade_name, MAX_NAME_SIZE, message_array_obj);
    if (ret != VOS_OK) {
        return ret;
    }
    
    ret = get_board_slave_addr(blade_name, &ipmb_addr);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade(%s) ipmb addr failed, err code %d", __func__, blade_name, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    ret = send_redfish_request_to_target(ipmb_addr, i_param, request_info, header, body, message_array_obj);
    return ret;
}


LOCAL gint32 direct_forward_master_blade_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    json_object **header, json_object **body, json_object *message_array_obj)
{
    gchar blade_name[MAX_NAME_SIZE] = {0};
    guint8 ipmb_addr = 0;
    gint32 ret;
    json_object *tmp_message_obj = NULL;

    ret = get_blade_name_from_uri(request_info->uri_lower, blade_name, MAX_NAME_SIZE, message_array_obj);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get blade name ret %d, uri:%s", ret, request_info->uri_lower);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_board_master_addr(blade_name, &ipmb_addr);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade(%s) master ipmb addr failed, err code %d", __func__, blade_name, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    debug_log(DLOG_DEBUG, "forward method = %s, replaced uri = %s\n",
        request_info->request_method, request_info->uri_lower);
    ret = send_redfish_request_to_target(ipmb_addr, i_param, request_info, header, body, message_array_obj);
    debug_log(DLOG_DEBUG, " send_redfish_request_to_target 0x%x ret %d", ipmb_addr, ret);
    return ret;
}

LOCAL gint32 managers_provider(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, json_object **header,
    json_object **body, json_object *message_array_obj)
{
    gchar blade_name[MAX_NAME_SIZE] = {0};
    gint32 ret;
    json_object *tmp_message_obj = NULL;

    if (g_ascii_strcasecmp(request_info->action_name, "Actions/Oem/Huawei/Manager.Dump") == 0) {  // 信息收集
        ret = get_blade_name_from_uri(request_info->uri_lower, blade_name, MAX_NAME_SIZE, message_array_obj);
        if (ret != VOS_OK) {
            return ret;
        }
        if (g_ascii_strncasecmp(blade_name, "blade", strlen("blade")) == 0) {  // 刀片不支持通过emm信息收集
            debug_log(DLOG_ERROR, "[%s] blade(%s) info collect not support", __func__, blade_name);
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &tmp_message_obj, request_info->action_name);
            json_object_array_add(message_array_obj, tmp_message_obj);
            return HTTP_BAD_REQUEST;
        }
        return HANDLE_LOCAL;
    }
    return direct_forward_provider(request_info, i_param, header, body, message_array_obj);
}

LOCAL json_object *formate_header(json_object *header_obj)
{
    json_object *new_header_obj = NULL;
    json_object *item = NULL;
    gint32 ret = 0;

    const gchar *key = NULL;
    json_object *val = NULL;
    struct lh_table *table = NULL;
    struct lh_entry *entry = NULL;

    new_header_obj = json_object_new_array();
    table = json_object_get_object(header_obj);
    if (table == NULL) {
        debug_log(DLOG_ERROR, "[%s] Get lh_table of header object failed", __func__);
        return new_header_obj;
    }
    for (entry = table->head; entry != NULL; entry = entry->next) {
        key = (const gchar *)entry->k;
        ret = json_object_object_get_ex(header_obj, key, &val);
        if (ret) {
            item = json_object_new_object();
            if (item != NULL) {
                (void)json_object_get(val);
                json_object_object_add(item, key, val);
                json_object_array_add(new_header_obj, item);
            } else {
                json_object_put(new_header_obj);
                return json_object_new_array();
            }
        }
    }
    return new_header_obj;
}


LOCAL void generate_new_request_head(CURL_REQ_INFO *req_info, REQUEST_INFO_S *request_info, gchar *token)
{
    int iRet;
    gchar header_item[MAX_HEADER_CONTENT_LEN + MAX_HEADER_TITLE_LEN + 1] = { 0 };
    guint32 i;

    iRet = snprintf_s(header_item, sizeof(header_item), sizeof(header_item) - 1, "%s:%s", HEADER_X_AUTH_TOKEN, token);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
    req_info->request_headers = curl_slist_append(req_info->request_headers, (const gchar *)header_item);
    for (i = 0; i < MAX_HEADER_NUM; i++) {
        if (0 == strlen(request_info->headers[i].header_title) ||
            0 == g_ascii_strcasecmp(request_info->headers[i].header_title, REQ_HEADER_X_AUTH_TOKEN) ||
            0 == g_ascii_strcasecmp(request_info->headers[i].header_title, REQ_HEADER_BASIC_AUTH) ||
            0 == g_ascii_strcasecmp(request_info->headers[i].header_title, REQ_HEADER_FROM)) {
            continue;
        }
        iRet = snprintf_s(header_item, sizeof(header_item), sizeof(header_item) - 1, "%s:%s",
            request_info->headers[i].header_title, request_info->headers[i].header_content);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet));
        req_info->request_headers = curl_slist_append(req_info->request_headers, (const gchar *)header_item);
        (void)memset_s(header_item, sizeof(header_item), 0, sizeof(header_item));
    }
}


gint32 send_redfish_request_to_target(guint8 ipmb_addr, PROVIDER_PARAS_S *i_param, REQUEST_INFO_S *request_info,
    json_object **header, json_object **body, json_object *message_array_obj)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    CURL_REQ_INFO req_info = { 0 };
    CURL_RESPONSE_INFO *resp_info = NULL;
    gint32 retry_times = 2; // 第一次发送返回码如果是401，则token过期，需要更新token后再次尝试发送
    gchar *token = NULL;
    guint8 presence = 0;
    json_object *tmp_message_obj = NULL;
    gchar uri_lower[MAX_URI_LENGTH] = { 0 };
    const gchar *blade_name = NULL;
    GSList *caller_info = NULL;
    OBJ_HANDLE blade_handle = 0;

    (void)dal_get_specific_object_byte(CLASS_NAME_IPMBETH_BLADE, PROTERY_IPMBETH_BLADE_SLAVEADDR, ipmb_addr,
        &blade_handle);
    blade_name = dfl_get_object_name(blade_handle);

    
    ret = get_board_presence(ipmb_addr, &presence);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get blade %u presence failed", __func__, ipmb_addr);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);

        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto exit;
    }
    if (presence != BOARD_PRESENT) {
        create_message_info(MSGID_BLADE_NOT_PRESENT, NULL, &tmp_message_obj);
        json_object_array_add(message_array_obj, tmp_message_obj);
        ret = HTTP_FORBIDDEN;
        goto exit;
    }
    
    do {
        
        ret = get_target_token(ipmb_addr, &token);
        if (token == NULL) {
            switch (ret) {
                case COMP_CODE_TIMEOUT:
                    create_message_info(MSGID_BLADE_COMMUNICATION_LOST, NULL, &tmp_message_obj);
                    ret = HTTP_SERVICE_UNAVAILABLE;
                    break;
                default:
                    create_message_info(MSGID_REDFISH_NOT_SUPPORTED_BY_BOARD, NULL, &tmp_message_obj, blade_name);
                    ret = HTTP_FORBIDDEN;
                    break;
            }

            json_object_array_add(message_array_obj, tmp_message_obj);
            goto exit;
        }
        
        generate_new_request_head(&req_info, request_info, token);
        
        (void)strncpy_s(uri_lower, sizeof(uri_lower), request_info->uri_lower, sizeof(uri_lower) - 1);
        
        if (g_ascii_strcasecmp(request_info->request_method, HTTP_ACTION) == 0) {
            safe_fun_ret = strncat_s(uri_lower, sizeof(uri_lower), "/", 1);
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            safe_fun_ret =
                strncat_s(uri_lower, sizeof(uri_lower), request_info->action_name, strlen(request_info->action_name));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        }
        
        req_info.method_name = request_info->request_method;
        if (strcmp(req_info.method_name, HTTP_ACTION) == 0 || strcmp(req_info.method_name, HTTP_CREATE) == 0) {
            req_info.method_name = HTTP_POST;
        }
        
        ret = get_forward_uri(ipmb_addr, uri_lower, req_info.req_uri, sizeof(req_info.req_uri));
        if (ret != VOS_OK) {
            create_message_info(MSGID_INTERNAL_ERR, NULL, &tmp_message_obj);
            json_object_array_add(message_array_obj, tmp_message_obj);
            curl_slist_free_all(req_info.request_headers);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            goto exit;
        }
        
        if (strlen(request_info->request_body) != 0) {
            req_info.req_body = request_info->request_body;
        }
        
        ret = dal_send_http_msg_with_curl(&req_info, &resp_info, NULL, FALSE, INVALID_DATA_BYTE);
        curl_slist_free_all(req_info.request_headers);
        req_info.request_headers = NULL;
        if (ret != VOS_OK || resp_info == NULL) {
            debug_log(DLOG_DEBUG, "[%s] Send http request to target[%s] failed, err code %d", __func__,
                req_info.req_uri, ret);
            create_message_info(MSGID_REDFISH_NOT_SUPPORTED_BY_BOARD, NULL, &tmp_message_obj, blade_name);
            json_object_array_add(message_array_obj, tmp_message_obj);
            ret = HTTP_SERVICE_UNAVAILABLE;
            goto exit;
        }
        ret = resp_info->response_code;
        if (resp_info->response_code != HTTP_UNAUTHORIZED) { // 非token过期，不进行尝试
            break;
        }
        
        delete_target_token(ipmb_addr);
        // while体最多执行2次。第一次执行时token过期，重试前释放resp_info内存，避免泄漏。第二次执行还是token过期，则不做处理返回结果。
        do_info_if_true(retry_times > 1,
            do_info_if_true(resp_info->response_header_jso != NULL, json_object_put(resp_info->response_header_jso));
            do_info_if_true(resp_info->response_body != NULL, g_free(resp_info->response_body)); g_free(resp_info);
            resp_info = NULL);
    } while (--retry_times);

    if (resp_info == NULL) {
        debug_log(DLOG_DEBUG, "[%s] resp_info is NULL", __func__);
        goto exit;
    }

    
    *header = resp_info->response_header_jso;
    
    if (*header != NULL) {
        if (json_object_has_key(*header, "Transfer-Encoding")) {
            json_object_object_del(*header, "Transfer-Encoding");
        }
    }
    *body = NULL;
    if (resp_info->response_body != NULL) {
        *body = json_tokener_parse(resp_info->response_body);
        g_free(resp_info->response_body);
    }
    g_free(resp_info);

exit:

    if (strcmp(request_info->request_method, HTTP_GET) && (NULL != i_param)) {
        
        caller_info =
            g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_param->is_from_webui)));
        
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(request_info->request_ip));

        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Request to %s(%s %s) %s", blade_name,
            request_info->request_method, request_info->uri_lower, (1 == ret / HTTP_OK) ? "successfully" : "failed");
        
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
        
    }

    return ret;
}

LOCAL guint8 get_method_name_mask(gchar *method_name)
{
    guint8 method_name_mask = 0;
    if (strcasecmp(method_name, HTTP_GET) == 0) {
        method_name_mask = ALLOW_GET;
    } else if (strcasecmp(method_name, HTTP_PATCH) == 0) {
        method_name_mask = ALLOW_PATCH;
    } else if (strcasecmp(method_name, HTTP_DELETE) == 0) {
        method_name_mask = ALLOW_DELETE;
    } else if (strcasecmp(method_name, HTTP_POST) == 0 || strcasecmp(method_name, HTTP_CREATE) == 0 ||
        strcasecmp(method_name, HTTP_ACTION) == 0) {
        method_name_mask = ALLOW_POST;
    }
    return method_name_mask;
}

LOCAL gint32 check_forward_privilege(gchar *method_name, guint8 user_role_priv, FORWARD_PROVIDER_S *provider)
{
    if (strcasecmp(method_name, HTTP_GET) == 0) {
        return user_role_priv & provider->get_privilege;
    } else {
        return user_role_priv & provider->set_privilege;
    }
}

FORWARD_PROVIDER_S *find_forward_request(gchar *uri, gchar *method_name)
{
    gint32 i;
    GRegex *regex = NULL;
    gchar *pattern_lower = NULL;
    gboolean is_match;
    GMatchInfo *match_info = NULL;
    guint8 method_name_mask;
    gchar *chassis_id = NULL;
    guint8 smm_index = 0;
    guint8 as_status = 0;

    // 主用板可以执行透传操作
    (void)get_board_active_state(&as_status);
    if (as_status != ASM_ACTIVE) {
        return NULL;
    }
    method_name_mask = get_method_name_mask(method_name);
    // 遍历透传到单板的表项
    for (i = 0; forward_uri_table[i].origin_uri != NULL; i++) {
        pattern_lower = g_ascii_strdown(forward_uri_table[i].origin_uri, strlen(forward_uri_table[i].origin_uri));
        return_val_do_info_if_fail(NULL != pattern_lower, NULL,
            debug_log(DLOG_ERROR, "[%s]: g_ascii_strdown fail, uri_pattern is %s", __FUNCTION__,
            forward_uri_table[i].origin_uri));
        regex = g_regex_new((const gchar *)pattern_lower, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
        is_match = g_regex_match(regex, uri, (GRegexMatchFlags)0x0, &match_info);
        g_free(pattern_lower);
        if (NULL != match_info) {
            g_match_info_free(match_info);
            match_info = NULL;
        }

        if (regex != NULL) {
            g_regex_unref(regex);
            regex = NULL;
        }
        if (TRUE == is_match) { // 查找到表项后，需要确定操作是否支持透传
            if ((method_name_mask & (forward_uri_table[i].allowed_operation)) == 0) { // 确定此操作是否需要透传
                return NULL;
            }
            return &(forward_uri_table[i]);
        }
    }
    // 遍历透传到HMM板的表项
    for (i = 0; forward_to_hmm_table[i].origin_uri != NULL; i++) {
        pattern_lower = g_ascii_strdown(forward_to_hmm_table[i].origin_uri, strlen(forward_to_hmm_table[i].origin_uri));
        return_val_do_info_if_fail(NULL != pattern_lower, NULL,
            debug_log(DLOG_ERROR, "[%s]: g_ascii_strdown fail, uri_pattern is %s", __FUNCTION__,
            forward_to_hmm_table[i].origin_uri));
        regex = g_regex_new((const gchar *)pattern_lower, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
        is_match = g_regex_match(regex, uri, (GRegexMatchFlags)0x0, &match_info);
        g_free(pattern_lower);
        if (match_info != NULL) {
            g_match_info_free(match_info);
            match_info = NULL;
        }

        if (NULL != regex) {
            g_regex_unref(regex);
            regex = NULL;
        }

        if (TRUE == is_match) { // 查找到表项后，需要确定操作是否支持透传
            if ((method_name_mask & (forward_to_hmm_table[i].allowed_operation)) == 0) { // 确定此操作是否需要透传
                return NULL;
            }

            // 是否访问本板
            chassis_id = g_strrstr(uri, "hmm");
            // 不带hmm的url，如rearm直接返回
            if (chassis_id == NULL) {
                return &(forward_to_hmm_table[i]);
            }
            get_smm_index(&smm_index);
            if (chassis_id[3] - '0' == smm_index) { // 访问本板
                return NULL;
            }

            return &(forward_to_hmm_table[i]);
        }
    }
    return NULL;
}


gint32 forward_for_em(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response)
{
    FORWARD_PROVIDER_S *forward_provider = NULL;
    gint32 ret = HANDLE_LOCAL;

    return_val_if_expr(NULL == request_info, ret);

    forward_provider = find_forward_request(request_info->uri_lower, request_info->request_method);
    if (forward_provider != NULL) { // 透传
        ret = handle_forward_request(request_info, i_param, forward_provider, response);
    }

    return ret;
}


FORWARD_PROVIDER_S *find_forward_request_from_table(gchar* uri, gchar* method_name,
    FORWARD_PROVIDER_S* forward_tab, gint32 tab_len)
{
    gint32 i;
    GRegex *regex = NULL;
    gchar *pattern_lower = NULL;
    gboolean is_match = 0;
    GMatchInfo *match_info = NULL;
    guint8 method_name_mask = get_method_name_mask(method_name);
    // 遍历透传到单板的表项
    for (i = 0; i < tab_len && forward_tab[i].origin_uri != NULL; i++) {
        pattern_lower = g_ascii_strdown(forward_tab[i].origin_uri, strlen(forward_tab[i].origin_uri));
        return_val_do_info_if_fail(pattern_lower != NULL, NULL,
                                   debug_log(DLOG_ERROR, "[%s]: g_ascii_strdown fail, uri_pattern is %s", __FUNCTION__,
                                             forward_tab[i].origin_uri));
        regex = g_regex_new(pattern_lower, (GRegexCompileFlags)0x0, (GRegexMatchFlags)0x0, NULL);
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

        // 查找到表项后，需要确定操作是否支持透传
        if (is_match == TRUE) {
            // 确定此操作是否需要透传
            if ((method_name_mask & (forward_tab[i].allowed_operation)) == 0) {
                return NULL;
            }
            debug_log(DLOG_DEBUG, "find_forward_request_from_table muri is %s", uri);
            return &(forward_tab[i]);
        }
    }
    return NULL;
}


gint32 forward_for_public_device(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    HTTP_RESPONSE_DATA_S* response)
{
    FORWARD_PROVIDER_S *forward_provider = NULL;
    gint32 ret = HANDLE_LOCAL;
    guint8 as_status = ASM_ACTIVE;
    gchar *pattern_lower = NULL;

    if (request_info == NULL) {
        return ret;
    }

    
    (void)get_board_active_state(&as_status);
    if (as_status == ASM_ACTIVE) {
        return ret;
    }

    
    pattern_lower = g_ascii_strdown(request_info->relative_uri, strlen(request_info->relative_uri));
    if (pattern_lower == NULL) {
        debug_log(DLOG_ERROR, "[%s]: g_ascii_strdown fail, uri_pattern is %s", __FUNCTION__, pattern_lower);
        return ret;
    }
    forward_provider = find_forward_request_from_table(pattern_lower, request_info->request_method,
        g_forward_to_master_table, sizeof(g_forward_to_master_table) / sizeof(FORWARD_PROVIDER_S));
    g_free(pattern_lower);
    pattern_lower = NULL;

    
    if (forward_provider == NULL) {
        return ret;
    }

    
    if (!check_forward_privilege(request_info->request_method, i_param->user_role_privilege, forward_provider)) {
        
        i_param->auth_type = AUTH_TYPE_PERMIT_DINED; 
        debug_log(DLOG_INFO, "forward permission denied, not compute etag value!");
        return ret;
    }

    
    ret = handle_forward_request(request_info, i_param, forward_provider, response);

    return ret;
}


gint32 handle_forward_request(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param,
    FORWARD_PROVIDER_S *forward_provider, HTTP_RESPONSE_DATA_S *response)
{
    gint32 ret = 0;
    json_object *body_obj = NULL;
    json_object *header_obj = NULL;
    json_object *status_obj = NULL;
    json_object *response_obj = NULL;
    json_object *message_array_obj = NULL;
    gint32 http_code;
    json_object *tmp_msg_obj = NULL;

    message_array_obj = json_object_new_array();
    response_obj = json_object_new_object();
    
    if (!check_forward_privilege(request_info->request_method, i_param->user_role_privilege,
        forward_provider)) { // 没有权限执行此URI透传
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &tmp_msg_obj);
        json_object_array_add(message_array_obj, tmp_msg_obj);
        http_code = HTTP_FORBIDDEN;
    } else {
        
        http_code = forward_provider->func(request_info, i_param, &header_obj, &body_obj, message_array_obj);
        if (http_code == HANDLE_LOCAL) { // 某些条件下，需本板进行处理
            json_object_put(message_array_obj);
            json_object_put(response_obj);
            return HANDLE_LOCAL;
        }
    }
    // header
    if (header_obj != NULL) { // 透传有响应头
        json_object_object_add(response_obj, RESPONSE_HEADERS, formate_header(header_obj));
        (void)json_object_put(header_obj);
    } else { // 无响应头则生成通用响应头
        header_obj = json_object_new_array();
        ret = generate_common_response_headers(&header_obj, request_info->relative_uri, message_array_obj);
        if (ret != VOS_OK) {
            if (header_obj != NULL) {
                json_object_put(header_obj);
            }
            goto err_exit;
        }
        json_object_object_add(response_obj, RESPONSE_HEADERS, header_obj);
    }
    // body
    if (body_obj != NULL) {
        json_object_object_add(response_obj, RESPONSE_BODY, body_obj);
    }
    // Status
    ret = generate_response_status(http_code, &status_obj, message_array_obj);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Generate response status failed", __func__);
        goto err_exit;
    }
    json_object_object_add(response_obj, RESPONSE_STATUS_CODE, status_obj);
    // Message
    if (0 != json_object_array_length(message_array_obj)) { // 存在错误信息
        if (body_obj != NULL) {
            json_object_object_add(body_obj, EXTENDED_INFO_KEY, json_object_get(message_array_obj));
        } else {
            (void)generate_error_message_info(message_array_obj, &body_obj);
            json_object_object_add(response_obj, RESPONSE_BODY, body_obj);
        }
    }
    // 生成结果
    generate_redfish_resp_data_from_jso(response_obj, response);
    (void)json_object_put(response_obj);
    (void)json_object_put(message_array_obj);
    return VOS_OK;

err_exit:
    json_object_put(response_obj);
    json_object_put(message_array_obj);
    return ret;
}


void redfish_forward_init(void)
{
    g_target_token_table = g_hash_table_new(NULL, NULL);
    if (g_target_token_table == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create hash table failed", __func__);
    }
    g_mutex_init(&g_target_token_table_lock);
}
