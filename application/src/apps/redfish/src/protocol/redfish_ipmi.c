
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "redfish_main.h"
#include "redfish_http.h"
#include "redfish_message.h"
#include "redfish_class.h"
#include "redfish_task.h"
#include "redfish_main.h"
#include "redfish_event.h"
#include "redfish_provider_manager.h"
#include "redfish_ipmi.h"

extern guint32 g_sp_timeout;
extern guint8 g_sp_finished;

LOCAL gint32 __find_object_by_json_path(const gchar *json_path, guint8 path_len, json_object *src_obj,
    json_object **dst_obj, gchar *key_name, gint32 length)
{
    gint32 i = 0;
    gint32 ret = 0;
    gint32 idx = 0;
    gchar *pstr = NULL;
    gchar *result = NULL;
    json_object *p_obj = NULL;
    json_object *t_obj = NULL;
    gchar tmp_json[TMP_STR_MAX] = {0};
    gchar tmp_name[TMP_STR_MAX] = {0};

    if ((src_obj == NULL) || (dst_obj == NULL) || (key_name == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d:  Invalid parameter.\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    if ((json_path[0] != '$') || (json_path[1] != '.') || (json_path[path_len - 1] == '.') ||
        (strstr(json_path, "..") != NULL)) {
        debug_log(DLOG_ERROR, "%s, %d:  Invalid parameter.\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    errno_t safe_fun_ret = memcpy_s(tmp_json, sizeof(tmp_json), json_path + 2, path_len - 2);
    return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    t_obj = src_obj;
    pstr = tmp_json;
    while (NULL != (result = strtok_s(pstr, ".", &pstr))) {
        (void)memset_s((void *)tmp_name, sizeof(tmp_name), 0, sizeof(tmp_name));
        safe_fun_ret = strncpy_s(tmp_name, sizeof(tmp_name), result, sizeof(tmp_name) - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        p_obj = t_obj;

        i = strlen(tmp_name) - 1;
        if (i < 0 || i + 1 >= sizeof(tmp_name)) {
            debug_log(DLOG_ERROR, "%s: json_name is empty.", __FUNCTION__);
            return RET_ERR;
        }
        
        
        idx = -1;
        if (tmp_name[i] == ']') { // 数组索引右括号
            tmp_name[i] = 0;
            while (i > 0) {
                i--;
                if (tmp_name[i] == '[') { // 数组索引左括号
                    tmp_name[i] = 0;
                    return_val_do_info_if_expr(strlen(&tmp_name[i + 1]) == 0, VOS_ERR,
                        debug_log(DLOG_ERROR, "%s, %d: index string is empty.\n", __FUNCTION__, __LINE__));
                    ret = vos_str2int(&tmp_name[i + 1], 10, &idx, NUM_TPYE_UINT32);
                    return_val_do_info_if_expr(ret == VOS_ERR, VOS_ERR,
                        debug_log(DLOG_ERROR, "%s, %d: index string(%s) invalid.\n", __FUNCTION__, __LINE__,
                        &tmp_name[i + 1]));
                    break;
                }
            }

            // 未找到数组左括号或属性名为空，无效jsonpath
            return_val_if_expr((i == 0) || (strlen(tmp_name) == 0), VOS_ERR);
        }

        if (!json_object_object_get_ex(p_obj, tmp_name, &t_obj)) {
            debug_log(DLOG_ERROR, "%s, %d:  Can't find object %s.\n", __FUNCTION__, __LINE__, tmp_name);
            return VOS_ERR;
        }

        if (idx >= 0) {
            p_obj = json_object_array_get_idx(t_obj, idx);
            return_val_do_info_if_expr(p_obj == NULL, VOS_ERR,
                debug_log(DLOG_ERROR, "%s, %d:  Can't find object(%d) from array(%s).\n", __FUNCTION__, __LINE__, idx,
                tmp_name));
            t_obj = p_obj;
        }
    }

    if ((p_obj == NULL) || (strlen(tmp_name) == 0)) {
        debug_log(DLOG_ERROR, "%s, %d:  Can't find valid info.\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }
    *dst_obj = p_obj;
    safe_fun_ret = strncpy_s(key_name, length, tmp_name, length - 1);
    return_val_do_info_if_expr(safe_fun_ret != EOK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    return VOS_OK;
}

LOCAL gint32 rf_get_sp_json_mem(json_object** u_obj)
{
    *u_obj = json_object_new_object();
    if (*u_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: u_obj is null.", __FUNCTION__);
        return VOS_ERR;
    }

    get_sp_result_in_mem(*u_obj);    // 获取全局变量中保存的result信息
    if (*u_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: copy json object failed.", __FUNCTION__);
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gint32 __check_input_param(const gchar *req_data, guint32 length, guint8 *val_len, guint8 *key_len)
{
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 128

    if ((req_data == NULL) || (length == 0)) {
        debug_log(DLOG_ERROR, "%s: req_data invalid!", __FUNCTION__);
        return RET_ERR;
    }

    
    *key_len = req_data[0];
    // [key_len + 2 > length - 1] --> [key_len > length - 3]; key的内容至少有$.
    if (*key_len > MAX_KEY_LEN || *key_len + 3 > length || *key_len < 3) {
        debug_log(DLOG_ERROR, "%s: req_data invalid, key_len: %d, length: %d", __FUNCTION__, *key_len, length);
        return RET_ERR;
    }

    *val_len = req_data[*key_len + 2];
    if ((*val_len > MAX_VALUE_LEN) || (length != (*key_len + *val_len + 3))) {
        debug_log(DLOG_ERROR, "%s: req_data invalid, key_len: %d, val_len: %d, length: %d", __FUNCTION__, *key_len,
            *val_len, length);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL guint8 __set_sp_result(const gchar *req_data, guint32 length)
{
    errno_t safe_fun_ret;
    gint32 ret;
    guint8 val_len;
    guint8 key_len;
    gchar val_str[TMP_STR_MAX] = {0};
    gchar key_str[TMP_STR_MAX] = {0};
    gchar file_name[TMP_STR_MAX] = {0};
    json_object *u_obj = NULL;
    json_object *p_obj = NULL;
    gchar key_name[TMP_STR_MAX] = {0};

    debug_log(DLOG_DEBUG, "%s: Begin set sp result.", __FUNCTION__);

    if (__check_input_param(req_data, length, &val_len, &key_len) != RET_OK) {
        return COMP_CODE_INVALID_FIELD;
    }

    
    safe_fun_ret = memcpy_s(key_str, sizeof(key_str), req_data + 1, key_len);
    return_val_do_info_if_expr(safe_fun_ret != EOK, COMP_CODE_UNKNOWN,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = memcpy_s(val_str, sizeof(val_str), req_data + 3 + key_len, val_len);
    return_val_do_info_if_expr(safe_fun_ret != EOK, COMP_CODE_UNKNOWN,
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret));

    
    (void)snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s.json", RFPROP_MANAGER_SP_RESULT_NAME);
    (void)rf_get_sp_json_file(UMS_TRANS_FILE_ID_SP_RESULT, file_name, &u_obj);
    if (u_obj == NULL) {
        debug_log(DLOG_ERROR, "%s: get json object from result.json failed.", __FUNCTION__);
        if (rf_get_sp_json_mem(&u_obj) != VOS_OK) {
            return COMP_CODE_UNKNOWN;
        }
    }

    
    ret = __find_object_by_json_path((const gchar *)key_str, key_len, u_obj, &p_obj, key_name, sizeof(key_name));
    return_val_do_info_if_expr(ret == VOS_ERR, COMP_CODE_UNKNOWN, json_object_put(u_obj);
        debug_log(DLOG_ERROR, "%s: find json object failed", __FUNCTION__));

    
    json_object_object_add(p_obj, key_name, json_object_new_string(val_str));
    ret = rf_set_sp_json_file(UMS_TRANS_FILE_ID_SP_RESULT, file_name, u_obj);
    json_object_put(u_obj);
    return_val_do_info_if_expr(ret != RET_OK, COMP_CODE_UNKNOWN,
        debug_log(DLOG_ERROR, "%s: write json object failed.", __FUNCTION__));
    if (g_ascii_strcasecmp(key_name, RFPROP_PROGRESS) != 0) {
        debug_log(DLOG_ERROR, "%s: Set SP Service task result [%s: %s] successfully", __FUNCTION__, key_name, val_str);
    }
    create_sp_result_monitor_task();
    set_sp_service_result_status(SP_RET_DEPLOYING);
    return COMP_CODE_SUCCESS;
}


LOCAL guint8 __set_sp_timeout(const void *req_ptr, const gchar *req_data, guint32 length)
{
    guint32 timeout;

    debug_log(DLOG_DEBUG, "[%s]:line(%d): Begain set sp timeout.", __FUNCTION__, __LINE__);

    if ((req_data == NULL) || (length != 4)) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): req_data invalid!\n", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }

    timeout = MAKE_DWORD(req_data[3], req_data[2], req_data[1], req_data[0]);
    if ((timeout < SP_MIN_TIMEOUT) || (timeout > SP_MAX_TIMEOUT)) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): timeout value (%d) invalid!\n", __FUNCTION__, __LINE__, timeout);
        return COMP_CODE_OUTOF_RANGE;
    }

    g_sp_timeout = timeout;
    proxy_ipmi_operation_log(req_ptr, "Set SP service task timeout to (%d) seconds successfully", g_sp_timeout);
    return COMP_CODE_SUCCESS;
}


LOCAL guint8 __set_sp_finished(const void *req_ptr, const gchar *req_data, guint32 length)
{
    guint8 state;

    debug_log(DLOG_DEBUG, "[%s]:line(%d): Begain set sp finished state.", __FUNCTION__, __LINE__);

    if ((req_data == NULL) || (length != 1)) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): req_data invalid!\n", __FUNCTION__, __LINE__);
        return COMP_CODE_INVALID_FIELD;
    }

    state = req_data[0];
    if ((state != FALSE) && (state != TRUE)) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): finished flag (%d) invalid!\n", __FUNCTION__, __LINE__, state);
        return COMP_CODE_OUTOF_RANGE;
    }

    g_sp_finished = state;
    if (g_sp_finished == TRUE) {
        set_sp_service_result_status(SP_RET_FINISHED);
        proxy_ipmi_operation_log(req_ptr, "Set SP service task status to finished");
    } else {
        proxy_ipmi_operation_log(req_ptr, "Set SP service task status to running");
    }
    return COMP_CODE_SUCCESS;
}


gint32 IpmiSetSPInfo(const void *req_ptr, gpointer user_data)
{
    guint8 ret_code;
    guint32 id;
    guint32 manufactureid = 0;
    gint32 ipmi_type;
    guint8 req_data_len;
    const guint8 *req_data = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (NULL == req_ptr) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): request parameter is NULL!\n", __FUNCTION__, __LINE__);
        return ipmi_send_simple_response(req_ptr, COMP_CODE_INVALID_CMD);
    }

    
    ipmi_type = get_ipmi_target_type(req_ptr);
    req_data = get_ipmi_src_data(req_ptr);
    req_data_len = get_ipmi_src_data_len(req_ptr);
    if ((NULL == req_data) || (IPMI_HOST != ipmi_type)) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): req_data is NULL or channel:%d unsupport!\n", __FUNCTION__, __LINE__,
            ipmi_type);
        return ipmi_send_simple_response(req_ptr, COMP_CODE_INVALID_CMD);
    }

    
    (void)dal_get_object_handle_nth(BMC_OBJECT_NAME, 0, &obj_handle);
    if (VOS_OK != dal_get_property_value_uint32(obj_handle, BMC_MANU_ID_NAME, &manufactureid)) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): get manufacture id failed.\n", __FUNCTION__, __LINE__);
        return ipmi_send_simple_response(req_ptr, COMP_CODE_UNKNOWN);
    }

    
    id = MAKE_DWORD(0, req_data[2], req_data[1], req_data[0]);
    if (manufactureid != id) {
        debug_log(DLOG_ERROR, "[%s]:line(%d): manufacture id(%x) mismatch.\n", __FUNCTION__, __LINE__, id);
        return ipmi_send_simple_response(req_ptr, COMP_CODE_INVALID_CMD);
    }

    switch (req_data[4]) {
        case SP_RESULT:
            ret_code = __set_sp_result((const gchar *)(req_data + 5), req_data_len - 5);
            break;
        case SP_TIMEOUT:
            ret_code = __set_sp_timeout(req_ptr, (const gchar *)(req_data + 5), req_data_len - 5);
            break;
        case SP_FINISHED:
            ret_code = __set_sp_finished(req_ptr, (const gchar *)(req_data + 5), req_data_len - 5);
            break;
        default:
            ret_code = COMP_CODE_INVALID_FIELD;
            debug_log(DLOG_ERROR, "[%s]:line(%d): unsupported sp cmd[%d]!\n", __FUNCTION__, __LINE__, req_data[4]);
    }

    return ipmi_send_simple_response(req_ptr, ret_code);
}
