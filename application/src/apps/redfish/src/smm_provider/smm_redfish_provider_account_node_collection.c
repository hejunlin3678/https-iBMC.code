
#include "redfish_provider.h"
#include "redfish_forward.h"

LOCAL gint32 foreach_blade_get_account_info(OBJ_HANDLE obj_handle, gpointer user_data);
LOCAL gint32 smm_get_account_node_collection_members_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 smm_get_account_node_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S collection_account_node_provider[] = {
    { RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, smm_get_account_node_collection_members_count, NULL, NULL, ETAG_FLAG_DISABLE },
    { RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_USERMGNT, SYS_LOCKDOWN_NULL, smm_get_account_node_collection_members, NULL, NULL, ETAG_FLAG_DISABLE },
};

static json_object *g_members = NULL;
static GMutex g_members_lock;

struct account_info_param {
    REQUEST_INFO_S *request_info;
    guint32 count;
    json_object *members_array;
    guchar expand; // 是否使用展开的格式
};


LOCAL gint32 foreach_blade_get_account_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    int iRet;
    errno_t safe_fun_ret = EOK;
    guint8 blade_type = 0;
    guint8 ipmb_addr = 0;
    struct account_info_param *param = (struct account_info_param *)user_data;
    json_object *response_info = NULL;
    json_object *message_array_obj = NULL;
    json_object *header = NULL;
    json_object *expanded_account_obj = NULL;
    gint32 ret;
    json_object *members_count_obj = NULL;
    json_object *members_array_obj = NULL;
    json_object *member = NULL;
    const gchar *bmc_user_uri = NULL;
    gchar *bmc_user_id = NULL;
    gchar odata_id[MAX_URI_LEN + 1] = { 0 };
    json_object *odata_id_obj = NULL;
    guint8 slot_id;
    gchar slot_id_str[MAX_SLOT_ID_STR_LEN + 1] = { 0 };
    gchar user_id_str[MAX_USER_ID_STR_LEN + 1] = { 0 };

    message_array_obj = json_object_new_array();
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    // 只访问计算节点 和 交换板
    if (blade_type != 0x00 && blade_type != 0xc0) {
        json_object_put(message_array_obj);
        return VOS_OK;
    }
    // 透传查询用户信息
    (void)dal_get_property_value_byte(obj_handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &ipmb_addr);
    // 计算slot_id
    slot_id = ipmb_addr / 2 - 0x40;

    ret = send_redfish_request_to_target(ipmb_addr, NULL, param->request_info, &header, &response_info,
        message_array_obj);
    do_info_if_true(header, json_object_put(header); header = NULL);
    json_object_put(message_array_obj);
    if (ret / HTTP_OK != 1) { // 单板透传失败
        debug_log(DLOG_DEBUG, "[%s] Send request[%s] to target(slot:%d) failed, http response %d", __func__,
            param->request_info->uri_lower, slot_id, ret);
        do_if_true(response_info, json_object_put(response_info); response_info = NULL);
        return VOS_OK;
    }
    // 解析信息重新封装, 失败继续访问下一个单板
    if (response_info == NULL) {
        debug_log(DLOG_ERROR, "[%s] Blade [ipmb addr : %u], request [%s], response json is null", __func__, ipmb_addr,
            param->request_info->uri_lower);
        return VOS_OK;
    }
    // 用户数目
    ret = json_object_object_get_ex(response_info, "Members@odata.count", &members_count_obj);
    if (!ret) {
        debug_log(DLOG_ERROR, "[%s] Blade [ipmb addr : %u], Get Members@odata.count from json object failed", __func__,
            ipmb_addr);
        json_object_put(response_info);
        return VOS_OK;
    }
    param->count += json_object_get_int(members_count_obj);
    // 用户信息
    ret = json_object_object_get_ex(response_info, "Members", &members_array_obj);
    if (!ret) {
        debug_log(DLOG_ERROR, "[%s] Blade [ipmb addr : %u], Get Members from json object failed", __func__, ipmb_addr);
        json_object_put(response_info);
        return VOS_OK;
    }
    json_object_array_foreach(members_array_obj, member)
    {
        // 获取用户id
        // BMC user URI is /redfish/v1/accountservice/accounts/id
        (void)json_object_object_get_ex(member, RFPROP_ODATA_ID, &odata_id_obj);
        bmc_user_uri = dal_json_object_get_str(odata_id_obj);
        bmc_user_id = g_strrstr(bmc_user_uri, "/");
        if (bmc_user_id == NULL) {
            debug_log(DLOG_ERROR, "[%s] Bmc account odata id is not right, (%s)", __func__, bmc_user_uri);
            continue;
        }
        bmc_user_id++; // 指向ID
        if (param->expand) {
            (void)snprintf_s(slot_id_str, sizeof(slot_id_str), sizeof(slot_id_str) - 1, "%u", slot_id);
            safe_fun_ret = strncpy_s(user_id_str, sizeof(user_id_str), bmc_user_id, sizeof(user_id_str) - 1);
            do_val_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            expanded_account_obj = json_object_get(member);
            format_account_body(expanded_account_obj, user_id_str, slot_id_str);
            json_object_array_add(param->members_array, expanded_account_obj);
            continue;
        }
        // 生成新的URI
        iRet = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1,
            "/redfish/v1/AccountService/Oem/Huawei/Nodes/%s_Slot%u", bmc_user_id, slot_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        odata_id_obj = json_object_new_string(odata_id);
        if (odata_id_obj == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json string failed", __func__);
            continue;
        }
        member = json_object_new_object();
        if (member == NULL) {
            debug_log(DLOG_ERROR, "[%s] Create new json object failed", __func__);
            json_object_put(odata_id_obj);
            continue;
        }
        json_object_object_add(member, RFPROP_ODATA_ID, odata_id_obj);
        json_object_array_add(param->members_array, member);
    }
    json_object_put(response_info);
    return VOS_OK;
}


gint32 smm_get_account_node_collection_members_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    struct account_info_param param = { 0 };
    gchar *uri = NULL;
    gchar *slotid_str = NULL;
    guint64 slotid = 0;
    guint8 tmp_slot_id = 0;
    OBJ_HANDLE object_handle = 0;
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
                               { create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso); });

    REQUEST_INFO_S *request_info = (REQUEST_INFO_S *)g_malloc0(sizeof(REQUEST_INFO_S));
    return_val_do_info_if_expr(request_info == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed", __FUNCTION__));

    // 构造请求头
    (void)strncpy_s(request_info->request_method, sizeof(request_info->request_method), HTTP_GET, strlen(HTTP_GET));
    uri = "/redfish/v1/AccountService/Accounts";
    (void)strcpy_s(request_info->uri_lower, sizeof(request_info->uri_lower), uri);
    // 设置参数
    param.request_info = request_info;
    param.members_array = json_object_new_array();
    if (param.members_array == NULL) {
        debug_log(DLOG_ERROR, "[%s] Create new json array failed", __func__);
        free_request_info(request_info);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (i_paras->uri_params) {
        slotid_str = (gchar *)g_hash_table_lookup(i_paras->uri_params, "slot");
    }
    if (slotid_str != NULL) {
        
        ret = vos_str2int(slotid_str, 10, &tmp_slot_id, NUM_TPYE_UCHAR);
        return_val_do_info_if_fail(ret == VOS_OK, HTTP_FORBIDDEN, free_request_info(request_info);
            (void)json_object_put(param.members_array);
            create_message_info(MSGID_QUERT_NOT_SUPPORTED, NULL, o_message_jso));
        slotid = g_ascii_strtoull(slotid_str, NULL, 10);
    }
    if (slotid) {
        ret = dal_get_specific_object_byte((const gchar *)CLASS_NAME_IPMBETH_BLADE,
            (const gchar *)PROTERY_IPMBETH_BLADE_SLAVEADDR, get_blade_ipmbaddr(slotid), &object_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_BAD_REQUEST, free_request_info(request_info);
            (void)json_object_put(param.members_array);
            create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, o_message_jso, "get account", "slot"));
        param.expand = 1;
        (void)foreach_blade_get_account_info(object_handle, &param);
    } else {
        // 获取信息
        ret = dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, foreach_blade_get_account_info, &param, NULL);
    }
    free_request_info(request_info);
    if (ret != VOS_OK) {
        json_object_put(param.members_array);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 释放之前的值
    g_mutex_lock(&g_members_lock); // 保证值正确传递
    g_members = param.members_array;
    *o_result_jso = json_object_new_int(param.count);

    return HTTP_OK;
}


LOCAL gint32 smm_get_account_node_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *tmp_members = g_members;
    g_members = NULL;
    g_mutex_unlock(&g_members_lock); // 接收到值
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        json_object_put(tmp_members);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN, {
        json_object_put(tmp_members);
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
    });
    *o_result_jso = tmp_members;
    return HTTP_OK;
}


gint32 smm_account_node_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    *prop_provider = collection_account_node_provider;
    *count = sizeof(collection_account_node_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
