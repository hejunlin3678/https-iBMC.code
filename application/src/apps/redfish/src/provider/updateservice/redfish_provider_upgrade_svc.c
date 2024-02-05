

#include "redfish_provider.h"
#include "redfish_message_system.h"
#include "redfish_util.h"
#include "../apps/upgrade/inc/upgrade_pub.h"

#define PFN_BMC_SYNC_PROGRESS_BASE        50
#define PFN_BMC_SYNC_TRANS_PROGRESS_BASE 3
#define ACTION_NAME_ACTIVATE_BIOS "activate BIOS"
typedef struct {
    guint8 code;
    const gchar state[PROP_VAL_LENGTH];
} SYNC_STATE_MAP;

enum {
    RF_UPGRADE_STATE_BIOS_IN_UPGRADE,
    RF_UPGRADE_STATE_NO_BIOS_UPGRADE
};

LOCAL gint32 parse_upgrade_err_result(gint32 err_code, const gchar **err_str);
LOCAL guint8 get_active_mode(json_object *val_jso);

LOCAL PROPERTY_PROVIDER_S  g_manager_actioninfo_bmc_sync_up_provider[] = {};
LOCAL PROPERTY_PROVIDER_S  g_manager_actioninfo_activate_bios[] = {};

LOCAL GHashTable *g_active_mode_tbl = NULL;


LOCAL gint32 reset_upgrade_flag(json_object *user_data_jso)
{
    OBJ_HANDLE obj_handle;
    GSList *input_list = NULL;
    gint32 ret;
    const gchar *user_name = NULL;
    json_object *priv_jso = NULL;
    const gchar *client = NULL;
    gint32 user_role_priv;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    

    (void)get_element_str(user_data_jso, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str(user_data_jso, RF_LOG_USER_IP, &client);
    if (user_name == NULL || client == NULL) {
        debug_log(DLOG_ERROR, "cann't get user name or client ip from user data jso");
        return RET_ERR;
    }

    
    (void)get_element_int(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);

    (void)json_object_object_get_ex(user_data_jso, RF_USER_PRIV, &priv_jso);
    user_role_priv = json_object_get_int(priv_jso);

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cann't get object handle for %s", OBJECT_PFN);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(FALSE));

    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, obj_handle, PFN_CLASS_NAME,
        METHOD_PFN_SET_UP_FLAG, AUTH_DISABLE, user_role_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);
    

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set upgrade flag failed");
    }

    return ret;
}

LOCAL gint32 set_bmc_active_mode(const gchar *user_name, const gchar *client, json_object *body_jso, guchar is_from_web)
{
    json_object *reset_flag_jso = NULL;
    guint8 active_mode = BMC_ACTIVE_MODE_IMMEDIATELY;
    const gchar *active_mode_str = NULL;
    GSList *input_list = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    if ((user_name == NULL) || (client == NULL) || (body_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)json_object_object_get_ex(body_jso, RFACTION_BMC_ACTIVE_MODE, &reset_flag_jso);
    if (reset_flag_jso != NULL) {
        active_mode_str = dal_json_object_get_str(reset_flag_jso);
        active_mode = dal_active_mode_str2int(active_mode_str);
        debug_log(DLOG_DEBUG, "%s: Active mode set by user is : %d\n", __FUNCTION__, active_mode);
    }

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle for %s failed", OBJECT_PFN);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)active_mode));
    ret = uip_call_class_method_redfish(is_from_web, user_name, client, obj_handle, PFN_CLASS_NAME,
        METHOD_SET_BMC_ACTIVE_MODE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);

    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method failed(%d).", __FUNCTION__, ret);
        return RET_ERR;
    }
    return ret;
}


LOCAL gboolean product_support_hotupgrade(void)
{
    return dal_match_product_id(PRODUCT_ID_TIANCHI) || dal_match_product_id(PRODUCT_ID_PRO);
}


LOCAL gint32 set_bios_active_mode(const gchar *user_name, const gchar *client, json_object *body_jso,
    guchar is_from_web)
{
    json_object *upgrade_mode_flag_jso = NULL;
    guint8 active_mode = BIOS_ACTIVE_MODE_NONE;
    GSList *input_list = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
 
    if ((user_name == NULL) || (client == NULL) || (body_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }
 
    (void)json_object_object_get_ex(body_jso, RFACTION_ACTIVATED_SCOPE, &upgrade_mode_flag_jso);
    if (upgrade_mode_flag_jso != NULL) {
        if (!product_support_hotupgrade()) {
            return RET_ERR;
        }
        active_mode = get_active_mode(upgrade_mode_flag_jso);
        debug_log(DLOG_DEBUG, "%s: Active mode set by user is : %d", __FUNCTION__, active_mode);
    }
 
    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle for %s failed", OBJECT_PFN);
        return RET_ERR;
    }
 
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)active_mode));
    ret = uip_call_class_method_redfish(is_from_web, user_name, client, obj_handle, PFN_CLASS_NAME,
        METHOD_SET_BIOS_ACTIVE_MODE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
 
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method failed(%d).", __FUNCTION__, ret);
        return RET_ERR;
    }
    return ret;
}

LOCAL gint32 set_upgrade_params(const gchar *user_name, const gchar *client, json_object *body_jso,
    guchar is_from_web)
{
    gint32 ret;
    ret = set_bmc_active_mode(user_name, client, body_jso, is_from_web);
    if (ret != RET_OK) {
        // 设置属性失败，则显示默认生效模式，不影响用户选择的生效模式
        debug_log(DLOG_ERROR, "%s: set current active mode failed.", __FUNCTION__);
        return ret;
    }
    ret = set_bios_active_mode(user_name, client, body_jso, is_from_web);
    if (ret != RET_OK) {
        // 设置属性失败，则显示默认升级模式，不影响用户选择的升级模式
        debug_log(DLOG_ERROR, "%s: set current bios active mode failed.", __FUNCTION__);
        return ret;
    }
    return ret;
}


LOCAL gint32 initial_package_upgrade(json_object *body_jso, json_object *user_data_jso, gboolean need_file_trasfer,
    json_object **message_obj)
{
    errno_t safe_fun_ret = EOK;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    const gchar *session_id = NULL;
    const gchar *image_uri = NULL;
    gchar           upgrade_package_path[MAX_URI_LENGTH] = {0};
    gchar *slash_index = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret;
    GSList *input_list = NULL;
    guchar forced_upgrade = 0;
    const gchar *err_result = NULL;
    
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    gint32 auth_type = 0;
    const gchar *role_id = NULL;

    (void)get_element_str(user_data_jso, RF_LOG_USER_NAME, &user_name);
    (void)get_element_str(user_data_jso, RF_LOG_USER_IP, &client);
    (void)get_element_str(user_data_jso, RF_SESSION_ID, &session_id);
    (void)get_element_int(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG, &from_webui_flag);
    (void)get_element_int(user_data_jso, RF_AUTH_TYPE, &auth_type);
    (void)get_element_str(user_data_jso, RF_ROLE_ID, &role_id);

    if (user_name == NULL || client == NULL || session_id == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return RF_FAILED;
    }

    (void)get_element_str(body_jso, RFACTION_PARAM_IMAGE_URI, &image_uri);
    if (image_uri == NULL) {
        debug_log(DLOG_ERROR, "cann't get image uri from body jso");
        return RF_FAILED;
    }
    
    if (need_file_trasfer == TRUE) {
        slash_index = g_strrstr(image_uri, SLASH_FLAG_STR);
        if (slash_index == NULL) {
            debug_log(DLOG_ERROR, "%s: get upgrade package name failed", __FUNCTION__);
            return RF_FAILED;
        }

        
        ret = snprintf_s(upgrade_package_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "/tmp%s", slash_index);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        }
    } else {
        // 如果不是通过文件传输得到的文件，认为ImageURI就是升级包路径
        safe_fun_ret = strncpy_s(upgrade_package_path, MAX_URI_LENGTH, image_uri, MAX_URI_LENGTH - 1);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    
    if (vos_get_file_accessible(upgrade_package_path) != 1) {
        
        debug_log(DLOG_ERROR, "upgrade package doesn't exist");
        ret = parse_upgrade_err_result(UP_COMM_FILE_NOT_EXIST, &err_result);
        
        if (ret != RF_OK) {
            return RF_FAILED;
        }

        (void)create_message_info(MSGID_FW_UPGRADE_ERROR, NULL, message_obj, err_result);
        return RF_FAILED;
    }

    if (!need_file_trasfer &&
        !dal_check_file_opt_user((guchar)auth_type, user_name, (const gchar *)upgrade_package_path, role_id)) {
        (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, message_obj);
        return RF_FAILED;
    }

    
    
    (void)chmod(upgrade_package_path, (S_IRUSR | S_IWUSR));

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle for %s failed", OBJECT_PFN);
        return RF_FAILED;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)upgrade_package_path));
    input_list = g_slist_append(input_list, g_variant_new_byte(forced_upgrade));

    
    ret = set_upgrade_params(user_name, client, body_jso, (guchar)from_webui_flag);
    if (ret != RET_OK) {
        uip_free_gvariant_list(input_list);
        return RF_FAILED;
    }
    ret = uip_call_class_method_redfish((guchar)from_webui_flag, user_name, client, obj_handle, PFN_CLASS_NAME,
        METHOD_PFN_INITIATE_UP, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    
    uip_free_gvariant_list(input_list);

    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "initial firmware upgrade failed");
        (void)reset_upgrade_flag(user_data_jso);
        (void)unlink(upgrade_package_path);
        return RF_FAILED;
    }

    return RF_OK;
}

void monitor_info_free_func(void *monitor_data)
{
    json_object *jso = (json_object *)monitor_data;

    (void)json_object_put(jso);

    return;
}


LOCAL TASK_MONITOR_INFO_S *upgrade_monitor_info_new(PROVIDER_PARAS_S *i_paras, json_bool need_file_trasfer)
{
    TASK_MONITOR_INFO_S *monitor_info;
    json_object *user_data_jso = NULL;

    monitor_info = task_monitor_info_new((GDestroyNotify)monitor_info_free_func);
    
    if (NULL == monitor_info) {
        debug_log(DLOG_ERROR, "task_monitor_info_new failed");
        return NULL;
    }

    

    user_data_jso = json_object_new_object();
    if (NULL == user_data_jso) {
        debug_log(DLOG_ERROR, "Failed to alloc a new json object.");
        task_monitor_info_free(monitor_info);

        return NULL;
    }

    
    json_object_object_add(user_data_jso, RF_LOG_USER_NAME, json_object_new_string(i_paras->user_name));
    json_object_object_add(user_data_jso, RF_LOG_USER_IP, json_object_new_string(i_paras->client));
    json_object_object_add(user_data_jso, RF_USER_PRIV, json_object_new_int(i_paras->user_role_privilege));
    json_object_object_add(user_data_jso, RF_UPGRADING_FLAG, json_object_new_boolean(FALSE));
    json_object_object_add(user_data_jso, RF_UPGRADING_NEED_FILE_TRASFER, json_object_new_boolean(need_file_trasfer));
    json_object_object_add(user_data_jso, RF_SESSION_ID, json_object_new_string(i_paras->session_id));
    
    json_object_object_add(user_data_jso, RF_USERDATA_FROM_WEBUI_FLAG,
        json_object_new_int((int32_t)i_paras->is_from_webui));
    
    json_object_object_add(user_data_jso, RF_AUTH_TYPE, json_object_new_int((int32_t)i_paras->auth_type));
    json_object_object_add(user_data_jso, RF_ROLE_ID, json_object_new_string(i_paras->user_roleid));

    monitor_info->user_data = user_data_jso;

    return monitor_info;
}

LOCAL gint32 parse_upgrade_err_result(gint32 err_code, const gchar **err_str)
{
    gint32 ret;

    
    INT2STR_MAP_S    err_result_map[] = {
        {UP_COMM_FILE_NOT_EXIST,          UP_COMM_FILE_NOT_EXIST_STR},
        {UP_COMM_FILE_ERROR,              UP_COMM_FILE_ERROR_STR},
        {UP_COMM_LITTLE_MEM,              UP_COMM_LITTLE_MEM_STR},
        {UP_COMM_POWER_TATUS_ERR,         UP_OS_POWER_ERROR_STR},
        {UP_COMM_FIRMWARE_VER_MISMATCH,   UP_COMM_FIRMWARE_MISMATCH_STR},
        {UP_COMM_FIRMWARE_FILE_MISMATCH,   UP_COMM_FILE_MISMATCH_STR},
                                       
        {UP_COMM_FIRMWARE_ERR_WRONG_ME_STATUS,   UP_COMM_ME_ERROR_STR},
        {UP_COMM_FIRMWARE_UNKNOWN_ERR,   UP_COMM_UNKNOWN_ERROR_STR},
        {UP_COMM_FIRMWARE_ERR_UPGRADE_FAILED, UP_COMM_UPGRADE_FAILED},
                                       
        {UP_COMM_MAIN_MAX,                UP_COMM_MAIN_MAX_STR},
        {UP_COMM_WHITEBRAND_VER_MISMATCH,     UP_COMM_WHITEBRAND_FILE_MISMATCH_STR},
        {UP_COMM_FIRMWARE_ERR_RETRY,     UP_COMM_POWERING_ON_ERR_STR},
        {UP_COMM_FIRMWARE_EARLY_ERR,     UP_COMM_VERSION_EARLY_ON_ERR_STR}
    };
    

    *err_str = NULL;
    ret = get_str_val_from_map_info(err_result_map, G_N_ELEMENTS(err_result_map), err_code, err_str);
    if (RF_OK != ret) {
        debug_log(DLOG_ERROR, "cann't get upgrade error description for %d", err_code);

        return RF_FAILED;
    }

    return RF_OK;
}


LOCAL gint32 update_upgrading_monitor_info(json_object *body_jso, TASK_MONITOR_INFO_S *monitor_info,
    json_object **message_info_jso)
{
    OBJ_HANDLE obj_handle;
    gchar         upgrade_result_str[UP_DETAIL_RESULT_MAX_LEN + 1] = {0};
    gint32 upgrade_err_code = 0;
    const gchar *err_result = NULL;
    guchar process = 0;
    guchar in_upgrade_flag = 0;
    gchar         component_str[MAX_COMPONENT_STR_LEN] = {0};
    guint8 software_type = 0;

    gint32 ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(ret == VOS_OK, RF_FAILED,
        debug_log(DLOG_ERROR, "get object handle for %s failed", OBJECT_PFN));

    (void)dal_get_property_value_int32(obj_handle, PFN_ERR_CODE_NAME, &upgrade_err_code);
    (void)dal_get_property_value_byte(obj_handle, PFN_UPGRADE_STATE_NAME, &process);
    (void)dal_get_property_value_byte(obj_handle, PFN_UPGRADE_INFLAG_NAME, &in_upgrade_flag);
    
    (void)dal_get_property_value_string(obj_handle, PFN_UPGRADE_DETAILED_RESULTS, upgrade_result_str,
        sizeof(upgrade_result_str));
    str_delete_char(upgrade_result_str, '\n');
    
    (void)dal_get_property_value_string(obj_handle, PFN_COMP_STR, component_str, MAX_COMPONENT_STR_LEN);
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        if (upgrade_err_code == UP_SHELF_FAN_PARTLY_OK) { 
            upgrade_err_code = 0;
        }
    }
    
    if (upgrade_err_code != 0) {
        
        (void)reset_upgrade_flag((json_object *)(monitor_info->user_data));

        monitor_info->task_state = RF_TASK_EXCEPTION;
        json_object_clear_string(body_jso, RFACTION_PARAM_IMAGE_URI);

        
        if (strlen(upgrade_result_str) != 0) {
            err_result = upgrade_result_str;
        } else {
            ret = parse_upgrade_err_result(upgrade_err_code, &err_result);

            
            return_val_if_fail(ret == RF_OK, RF_FAILED);
        }

        (void)create_message_info(MSGID_FW_UPGRADE_ERROR, NULL, message_info_jso, err_result);
    } else {
        
        
        if (in_upgrade_flag == FALSE) {
            
            (void)create_message_info(MSGID_FW_UPGRADE_COMPLETE, NULL, message_info_jso, (const gchar *)component_str);
            monitor_info->task_state = RF_TASK_COMPLETED;
            json_object_clear_string(body_jso, RFACTION_PARAM_IMAGE_URI);
        } else {
            (void)create_message_info(MSGID_FW_UPGRADE_COMPONENT, NULL, message_info_jso, (const gchar *)component_str);
        }
        monitor_info->task_progress = process;
        
    }

    return RF_OK; 
}


gint32 parse_file_transfer_err_code(gint32 transfer_status, json_object **message_obj)
{
    gint32 ret;
    INT2STR_MAP_S    file_transfer_err_array[] = {
        {UNKNOWN_ERROR,                    UNKNOWN_ERROR_STR},
        {FILENAME_TOO_LONG,                FILENAME_TOO_LONG_STR},
        {FILELEN_OUTOF_MAX,                FILELEN_OUTOF_MAX_STR},
        {SET_UPDATEFLAG_ERROR,             UNKNOWN_ERROR_STR},
        {CURL_RUN_ERROR,                   CURL_RUN_ERROR_STR},
        {TRANSFILE_UNSUPPORTED_PROTOCOL,   TRANSFILE_UNSUPPORTED_PROTOCOL_STR},
                                                
        {TRANSFILE_URL_FORMAT_ERR,         TRANSFILE_URL_FORMAT_STR},
                                                
        {UMS_BUSY_ERROR,                   UMS_BUSY_ERROR_STR},
        {FIND_CLASS_ERROR,                 UNKNOWN_ERROR_STR},
        {MOUNT_FILE_UNAVAILABLE,           MOUNT_FILE_UNAVAILABLE_STR},
        {MOUNT_UMS_UNAVAILABLE,            UNKNOWN_ERROR_STR},
        {CMS_AUTH_ERROR,                   CMS_AUTH_ERROR_STR},
        {UPGRADE_SP_ERROR,                 UNKNOWN_ERROR_STR},
        {IMAGEURI_IS_ILLEGAL,              IMAGEURI_IS_ILLEGAL_STR},
        {UMS_TRANSFER_TIMEOUT,             UMS_TRANSFER_TIMEOUT_STR},
        {SP_IS_BUSY,                       SP_IS_BUSY_STR},
        {SP_OF_X86_NEEDED,                 SP_OF_X86_NEEDED_STR},
        {SP_OF_ARM_NEEDED,                 SP_OF_ARM_NEEDED_STR},
        {IN_UPGRADE_PROGRESS,              IN_UPGRADE_PROGRESS_STR},
        {CURL_PEER_SSL_VERIFY_FAILED,           CURL_PEER_SSL_VERIFY_FAILED_STR},
        {NO_PERMISSION_TO_OPERATE_FILE,    CURRENT_FILE_NO_PERMISSION_STR}
    };
    const gchar *err_desc = NULL;
    gchar param_buf[PROP_VAL_LENGTH] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar component_str[MAX_COMPONENT_STR_LEN] = {0};

    
    if ((transfer_status >= 0) && (transfer_status <= RF_FINISH_PERCENTAGE)) {
        return RF_OK;
    }

    
    ret = get_str_val_from_map_info(file_transfer_err_array, G_N_ELEMENTS(file_transfer_err_array), transfer_status,
        &err_desc);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't find error description for %d", transfer_status));

    if (transfer_status == IN_UPGRADE_PROGRESS) {
        (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PFN_COMP_STR, component_str, MAX_COMPONENT_STR_LEN);
        goto_if_expr(snprintf_s(param_buf, sizeof(param_buf), sizeof(param_buf) - 1, IN_UPGRADE_PROGRESS_STR,
            component_str) <= 0,
            err_exit);
        err_desc = param_buf;
    }

    (void)create_message_info(MSGID_FILE_TRANSFER_ERR_DESC, NULL, message_obj, err_desc);
    return RF_FAILED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);

    return RF_FAILED;
}


LOCAL gint32 upgrade_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret = 0;
    gint32 file_down_progress = 0;
    json_object *upgrading_flag_jso = NULL;
    json_bool upgrading_flag;
    json_bool need_file_trasfer = TRUE;
    gchar         progress_str[RF_MAX_PROGRESS_LEN] = {0};

    return_val_do_info_if_fail((origin_obj_path != NULL) && (monitor_fn_data != NULL) && (message_obj != NULL),
        RF_FAILED, debug_log(DLOG_ERROR, "input param error"));

    (void)json_object_object_get_ex((json_object *)(monitor_fn_data->user_data), RF_UPGRADING_FLAG,
        &upgrading_flag_jso);
    
    (void)get_element_boolean((json_object *)(monitor_fn_data->user_data), RF_UPGRADING_NEED_FILE_TRASFER,
        &need_file_trasfer);
    
    
    upgrading_flag = json_object_get_boolean(upgrading_flag_jso);

    debug_log(DLOG_DEBUG, "--upgrade flag:%d--------", upgrading_flag);
    monitor_fn_data->task_state = RF_TASK_RUNNING;

    if (upgrading_flag == FALSE) {
        
        if (need_file_trasfer == TRUE) {
            ret = get_file_transfer_progress(&file_down_progress);
            goto_label_do_info_if_fail(ret == VOS_OK, err_exit,
                debug_log(DLOG_ERROR, "get upgrade packege download progress failed"));

            
            ret = parse_file_transfer_err_code(file_down_progress, message_obj);
            return_val_do_info_if_fail(RF_OK == ret, RF_OK, monitor_fn_data->task_state = RF_TASK_EXCEPTION;
                json_object_clear_string(body_jso, RFACTION_PARAM_IMAGE_URI);
                debug_log(DLOG_ERROR, "file transfer error occured, error code:%d", file_down_progress));

            debug_log(DLOG_DEBUG, "----------file down progress :%d---------", file_down_progress);
        } else {
            file_down_progress = RF_FINISH_PERCENTAGE;
        }

        

        
        if (file_down_progress != RF_FINISH_PERCENTAGE) {
            
            (void)snprintf_s(progress_str, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1, "%d%%", file_down_progress);
            (void)create_message_info(MSGID_FILE_TRANSFER_PROGRESS, NULL, message_obj,
                (const gchar *)progress_str);

            return RF_OK;
        } else {
            
            upgrading_flag = TRUE;
            json_object_object_add((json_object *)(monitor_fn_data->user_data), RF_UPGRADING_FLAG,
                json_object_new_boolean(upgrading_flag));

            debug_log(DLOG_DEBUG, "----------begin to upgrade---------");
            
            ret = initial_package_upgrade(body_jso, (json_object *)monitor_fn_data->user_data, need_file_trasfer,
                message_obj);
            
            return_val_do_info_if_expr(RF_OK != ret && NULL != message_obj, RF_OK,
                monitor_fn_data->task_state = RF_TASK_EXCEPTION;
                json_object_clear_string(body_jso, RFACTION_PARAM_IMAGE_URI));
            
            goto_label_do_info_if_fail(ret == RF_OK || message_obj != NULL, err_exit,

                debug_log(DLOG_ERROR, "initial package upgrade failed"));
            
        }
    } else { 
        ret = update_upgrading_monitor_info(body_jso, monitor_fn_data, message_obj);
        goto_label_do_info_if_fail(ret == RF_OK, err_exit,
            debug_log(DLOG_ERROR, "update upgrading monitor info failed"));
    }

    return RF_OK;

err_exit:
    monitor_fn_data->task_state = RF_TASK_EXCEPTION;
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
    json_object_clear_string(body_jso, RFACTION_PARAM_IMAGE_URI);

    return RF_OK; 
}


LOCAL gint32 resolve_transfer_uri(const gchar *image_uri, const gchar *transfer_protocol, gchar *uri_out,
    guint32 uri_len, json_object **err_message_jso)
{
    errno_t safe_fun_ret;
    gchar *p_protocol_index = NULL;
    gchar *protocol_lower_str = NULL;
    const gchar *file_path_uri = NULL;
    gint cmp_result;
    gint32 ret;
    gint32 cmp_len;
    gint32 protocol_len;

    return_val_do_info_if_expr(uri_len == 0, RF_FAILED,
        debug_log(DLOG_ERROR, "%s failed:input param error, uri_len is 0.", __FUNCTION__));
    

    (void)snprintf_s(uri_out, uri_len, uri_len - 1, "download;%d;", RF_FILE_TRANSFER_OBJ_FILE_ID);
    p_protocol_index = g_strstr_len(image_uri, -1, RF_TRANSFER_PROTOCOL_FLAG);

    protocol_lower_str = g_ascii_strdown(transfer_protocol, -1);
    if (NULL == protocol_lower_str) {
        debug_log(DLOG_ERROR, "get lower uri failed");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, err_message_jso);

        return RF_FAILED;
    }

    
    if (NULL != p_protocol_index) {
        protocol_len = p_protocol_index - image_uri;
        cmp_len = (strlen(protocol_lower_str) >= protocol_len) ? (strlen(protocol_lower_str)) : protocol_len;
        cmp_result = g_ascii_strncasecmp(protocol_lower_str, image_uri, cmp_len);
        if (0 != cmp_result) {
            ret = RF_FILE_TRANSFER_PROTOCAL_MISMATCH;

            debug_log(DLOG_ERROR, "transfer protocol mismatch");
            (void)create_message_info(MSGID_FILE_TRANSFER_PROTOCOL_MISMATCH, NULL, err_message_jso);

            g_free(protocol_lower_str);

            return ret;
        }
    }

    
    safe_fun_ret = strncat_s(uri_out, uri_len, protocol_lower_str, strlen(protocol_lower_str));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    safe_fun_ret = strncat_s(uri_out, uri_len, RF_TRANSFER_PROTOCOL_FLAG, strlen(RF_TRANSFER_PROTOCOL_FLAG));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    
    file_path_uri = (p_protocol_index) ? (p_protocol_index + strlen(RF_TRANSFER_PROTOCOL_FLAG)) : image_uri;
    safe_fun_ret = strncat_s(uri_out, uri_len, file_path_uri, strlen(file_path_uri));
    do_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    g_free(protocol_lower_str);

    return RF_OK;
}


LOCAL gint32 check_bios_status(PROVIDER_PARAS_S *i_param)
{
    json_object *upgrade_mode_flag_jso = NULL;
    guint8 active_mode = BIOS_ACTIVE_MODE_NONE;
    guint8 bios_status = 0;
    json_object *body_jso = i_param->val_jso;
    if (body_jso == NULL) {
        return VOS_ERR;
    }
    (void)json_object_object_get_ex(body_jso, RFACTION_ACTIVATED_SCOPE, &upgrade_mode_flag_jso);
    if (upgrade_mode_flag_jso != NULL) {
        active_mode = get_active_mode(upgrade_mode_flag_jso);
        debug_log(DLOG_ERROR, "%s: BiosActiveMode is : %d", __FUNCTION__, active_mode);
    }
    
    if (active_mode == BIOS_ACTIVE_MODE_NONE) {
        return VOS_OK;
    }
    (void)dal_get_func_ability(CLASS_NAME_BIOS, PROPERTY_BIOS_STARTUP_STATE, &bios_status);
    
    if (bios_status != BIOS_STARTUP_POST_STAGE_FINISH) {
        debug_log(DLOG_ERROR, "%s Bios status %d is not ok", __FUNCTION__, bios_status);
        GSList *caller_info = NULL;
        caller_info =
            g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_param->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->client));
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "BIOS hot upgrade failed");
        uip_free_gvariant_list(caller_info);
        return VOS_ERR;
    }
    return VOS_OK;
}


LOCAL gint32 upgrade_prep_check(PROVIDER_PARAS_S *i_param, json_object **o_message_jso)
{
    gint32 available_task_id;
    gint32 ret;
    OBJ_HANDLE obj_handle;
    guchar upgrading_flag;
    guchar transfering_flag;
    
    gboolean b_fw_upgrading;
    
    guint8 software_type = 0;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    guint32 presence = 0;

    
    if (0 == (USERROLE_BASICSETTING & (i_param->user_role_privilege))) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);

        return HTTP_FORBIDDEN;
    }

    
    
    b_fw_upgrading = check_redfish_running_task_exist(upgrade_status_monitor, i_param->val_jso, i_param->uri, NULL);
    return_val_do_info_if_expr(b_fw_upgrading == TRUE, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "another upgrade task is running");
        (void)create_message_info(MSGID_FW_UPGRADING, NULL, o_message_jso));

    

    
    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit, debug_log(DLOG_ERROR, "cann't get object %s", OBJECT_PFN));

    ret = dal_get_property_value_byte(obj_handle, PFN_UPGRADE_INFLAG_NAME, &upgrading_flag);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "get property value %s failed", PFN_UPGRADE_INFLAG_NAME));

    return_val_do_info_if_expr(upgrading_flag == TRUE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_FW_UPGRADING, NULL, o_message_jso));

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        ret = get_other_board_present(&presence);
        goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
            debug_log(DLOG_ERROR, "[%s]Get other smm present state failed.", __func__));

        if (BOARD_PRESENT == presence) {
            property_name_list = g_slist_append(property_name_list, g_variant_new_string(PFN_UPGRADE_INFLAG_NAME));
            ret = get_other_smm_property(OBJECT_PFN, property_name_list, &property_value_list);
            g_slist_free_full(property_name_list, (GDestroyNotify)g_variant_unref);
            property_name_list = NULL;

            if (ret != VOS_OK) {
                (void)create_message_info(MSGID_FW_UPGRADE_ERROR, NULL, o_message_jso,
                    "Failed to get the upgrade property of othersmm.");
                debug_log(DLOG_ERROR, "[%s]get other smm upgrade flag failed.\r\n", __func__);
                return HTTP_FORBIDDEN;
            }

            upgrading_flag = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
            g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
            property_value_list = NULL;

            if (0 != upgrading_flag) {
                debug_log(DLOG_ERROR, "[%s]Other smm has an upgrade operation in progress.\r\n", __func__);
                (void)create_message_info(MSGID_FW_UPGRADING, NULL, o_message_jso);

                return HTTP_BAD_REQUEST;
            }
        }
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't get object for %s", CLASS_NAME_TRANSFERFILE));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_IS_TRANSFERING, &transfering_flag);
    goto_label_do_info_if_fail(VOS_OK == ret, err_exit,
        debug_log(DLOG_ERROR, "cann't get object property %s", PROTERY_IS_TRANSFERING));
    
    return_val_do_info_if_fail(FALSE == transfering_flag, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "upgrade failed, another file transfer task is running");
        (void)create_message_info(MSGID_FILE_DOWNLOAD_TASK_OCCUPIED, NULL, o_message_jso));

    
    
    available_task_id = find_available_task_id();
    return_val_do_info_if_fail(RF_INVALID_TASK_ID != available_task_id, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_TASK_LIMIT_EXCEED, NULL, o_message_jso));
    

    
    return_val_do_info_if_fail(check_bios_status(i_param) == VOS_OK, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_BIOS_STATE_NOT_ALLOWED, NULL, o_message_jso));

    return RF_OK;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 perform_update_svc_with_local_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *upgrade_monitor_data = NULL;

    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso), err_exit,
        debug_log(DLOG_ERROR, "input param error."));

    
    
    upgrade_monitor_data = upgrade_monitor_info_new(i_paras, FALSE);
    goto_label_do_info_if_fail(NULL != upgrade_monitor_data, err_exit,
        debug_log(DLOG_ERROR, "alloc a new upgrade monitor info failed"));

    upgrade_monitor_data->task_progress = TASK_NO_PROGRESS; 
    upgrade_monitor_data->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(RF_UPGRADE_TASK_DESC, upgrade_status_monitor, upgrade_monitor_data, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(upgrade_monitor_data);
        debug_log(DLOG_ERROR, "create new task failed"));
    

    return HTTP_ACCEPTED;

err_exit:

    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL void set_default_active_mode(json_object *body_jso)
{
    guchar mgmt_software_type = 0;
    json_object *reset_flag_jso = NULL;

    debug_log(DLOG_DEBUG, "%s: now in set_current_active_mode", __FUNCTION__);

    if (body_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__);
        return;
    }

    (void)dal_get_software_type(&mgmt_software_type);
    (void)json_object_object_get_ex(body_jso, RFACTION_BMC_ACTIVE_MODE, &reset_flag_jso);

    if (reset_flag_jso == NULL) {
        debug_log(DLOG_DEBUG, "%s:reset flag jso is null, set default active mode...\n", __FUNCTION__);
        if (mgmt_software_type == MGMT_SOFTWARE_TYPE_EM) {
            json_object_object_add(body_jso, RFACTION_BMC_ACTIVE_MODE,
                json_object_new_string(RFACTION_BMC_ACTIVE_MODE_RESET));
        } else {
            json_object_object_add(body_jso, RFACTION_BMC_ACTIVE_MODE,
                json_object_new_string(RFACTION_BMC_ACTIVE_MODE_IMMEDIATELY));
        }
    }
}

LOCAL gint32 perform_update_svc_with_remote_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    const gchar *image_uri, const gchar *transfer_protocol, json_object **o_result_jso)
{
    gchar  transfer_file_uri[MAX_URI_LENGTH] = { 0 }; 
    GSList *input_list = NULL;
    OBJ_HANDLE transfer_file_handle = 0;

    set_default_active_mode(i_paras->val_jso);
    
    gint32 ret = resolve_transfer_uri(image_uri, transfer_protocol, transfer_file_uri, sizeof(transfer_file_uri), 
        o_message_jso);
    if (ret != RF_OK) {
        debug_log(DLOG_ERROR, "resolve transfer uri failed");
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_TRANSFERFILE, 0, &transfer_file_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "cann't get objhandle for %s", OBJ_NAME_TRANSFERFILE);
        (void)memset_s(transfer_file_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
        goto err_exit;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)transfer_file_uri));
    (void)memset_s(transfer_file_uri, MAX_URI_LENGTH, 0, MAX_URI_LENGTH);
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)i_paras->session_id));
    
    ret = uip_redfish_call_class_method_with_userinfo(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        (i_paras->auth_type == LOG_TYPE_LOCAL), i_paras->user_roleid, transfer_file_handle, CLASS_NAME_TRANSFERFILE,
        METHOD_INITRIAL_FILE_TRANSFER, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        (void)parse_file_transfer_err_code(ret, o_message_jso);
        return HTTP_BAD_REQUEST;
    } 
    
    TASK_MONITOR_INFO_S *upgrade_monitor_data = upgrade_monitor_info_new(i_paras, TRUE);
    if (upgrade_monitor_data == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc a new upgrade monitor info failed", __FUNCTION__);
        goto err_exit;
    }
    upgrade_monitor_data->task_progress = TASK_NO_PROGRESS; 
    
    upgrade_monitor_data->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(RF_UPGRADE_TASK_DESC, upgrade_status_monitor, upgrade_monitor_data, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    if (ret != RF_OK) {
        task_monitor_info_free(upgrade_monitor_data);
        debug_log(DLOG_ERROR, "%s:create new task failed %d", __FUNCTION__, ret);
        goto err_exit;
    }

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 check_upgrade_uri(const gchar *image_uri)
{
#define UPGRADE_IMAGE_URI_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.hpm$"
#define UPGRADE_IMAGE_NAME_REGEX "[^a-zA-Z0-9\\._\\(\\)\\-\\s]+"

    const guint32 hpm_file_suffix_len = 4;
    gchar file_name[URL_MAX_LENGTH] = {0};

    
    if (g_regex_match_simple(UPGRADE_IMAGE_URI_REGEX, image_uri, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) != TRUE) {
        debug_log(DLOG_ERROR, "%s : Invalid image uri.", __FUNCTION__);
        return RET_ERR;
    }

    gchar *str_tmp = g_strrstr(image_uri, "/");
    if (str_tmp == NULL) {
        debug_log(DLOG_ERROR, "%s:Get file name ptr failed.", __FUNCTION__);
        return RET_ERR;
    }

    str_tmp++;

    gint32 name_len = strlen(str_tmp) - hpm_file_suffix_len;
    if (name_len <= 0) {
        debug_log(DLOG_ERROR, "%s:File name len invalid. len[%d]", __FUNCTION__, name_len);
        return RET_ERR;
    }

    gint32 ret = strncpy_s(file_name, sizeof(file_name), str_tmp, name_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s:Copy fime name failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    if (g_regex_match_simple(UPGRADE_IMAGE_NAME_REGEX, file_name, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) == TRUE) {
        debug_log(DLOG_ERROR, "%s:Invalid file name.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_component_object(PROVIDER_PARAS_S *i_paras, json_object **component_info)
{
    GSList *output_list = NULL;
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(BIOS_JSON_FILE_COMPONENTVERSION_NAME));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        BIOS_CLASS_NAME, METHOD_BIOS_GET_JSON_FILE, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call method %s failed, ret:%d", __FUNCTION__, METHOD_BIOS_GET_JSON_FILE, ret);
        uip_free_gvariant_list(output_list);
        return RET_ERR;
    }

    const gchar *file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (file_data == NULL || strlen(file_data) == 0) {
        debug_log(DLOG_ERROR, "%s: file_data is NULL or empty.", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        return RET_ERR;
    }
    *component_info = json_tokener_parse(file_data);
    ret = dal_set_active_mode_tbl(file_data, NULL, &g_active_mode_tbl);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_set_active_mode_tbl failed.", __FUNCTION__);
    }
    uip_free_gvariant_list(output_list);
    return RET_OK;
}

LOCAL gint32 traverse_component_info(GHashTable *com_hash, json_object *com_info)
{
    if (json_object_get_type(com_info) != json_type_array) {
        debug_log(DLOG_ERROR, "%s: the com_info is not array.", __FUNCTION__);
        return RET_ERR;
    }
    size_t len = json_object_array_length(com_info);
    for (size_t idx = 0; idx < len; idx++) {
        json_object *item = json_object_array_get_idx(com_info, idx);
        json_object *name_obj = NULL;
        if (json_object_object_get_ex(item, RFPROP_FWINV_NAME, &name_obj) != TRUE) {
            debug_log(DLOG_ERROR, "%s: get component name failed.", __FUNCTION__);
            continue;
        }
        const gchar *name = json_object_get_string(name_obj);
        gsize key_len = strlen(name) + 1;
        gpointer key = g_malloc0(key_len);
        if (strcpy_s((gchar *)key, key_len, name) != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s failed.", __FUNCTION__);
            g_free(key);
            continue;
        }
        json_object *attribute_obj = NULL;
        if (json_object_object_get_ex(item, RFPROP_FWINV_ATTRIBUTE, &attribute_obj) != TRUE) {
            debug_log(DLOG_ERROR, "%s: get component attribute failed.", __FUNCTION__);
            g_free(key);
            continue;
        }
        size_t attribute_len = json_object_array_length(attribute_obj);
        gboolean hotfix_flag = FALSE;
        for (size_t attr_idx = 0; attr_idx < attribute_len; attr_idx++) {
            const gchar *attr_str = json_object_get_string(
                json_object_array_get_idx(attribute_obj, attr_idx));
            if (g_strcmp0(attr_str, RFPROP_FWINV_ATTRIBUTE_HOTFIX) == 0) {
                hotfix_flag = TRUE;
                break;
            }
        }
        if (hotfix_flag) {
            g_hash_table_insert(com_hash, key, UINT32_TO_POINTER(1));
        } else {
            g_hash_table_insert(com_hash, key, UINT32_TO_POINTER(0));
        }
    }
    return RET_OK;
}


LOCAL gint32 construct_hotfix_info(PROVIDER_PARAS_S *i_paras, GHashTable **component_table)
{
    json_object *com_obj = NULL;
    gint32 ret = get_component_object(i_paras, &com_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get_component_object failed.", __FUNCTION__);
        GSList *caller_info = NULL;
        caller_info =
            g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "BIOS hot upgrade failed");
        uip_free_gvariant_list(caller_info);
        return RET_ERR;
    }
    json_object *com_info = NULL;
    if (json_object_object_get_ex(com_obj, RFPROP_FWINV_COMPONENT_INFO, &com_info) != TRUE) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed.", __FUNCTION__);
        (void)json_object_put(com_obj);
        return RET_ERR;
    }
    *component_table = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    if (*component_table == NULL) {
        debug_log(DLOG_ERROR, "%s: g_hash_table_new failed.", __FUNCTION__);
        return RET_ERR;
    }
    ret = traverse_component_info(*component_table, com_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: traverse_component_info failed.", __FUNCTION__);
        g_hash_table_unref(*component_table);
        (void)json_object_put(com_obj);
        *component_table = NULL;
        return RET_ERR;
    }
    (void)json_object_put(com_obj);
    return RET_OK;
}

LOCAL gint32 check_local_image_path(const gchar *image_uri, const gchar *action_name,
    PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret = dal_check_real_path(image_uri);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_check_real_path failed.", __FUNCTION__);
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO, 
            RFACTION_PARAM_IMAGE_URI, action_name);
        return HTTP_BAD_REQUEST;
    }

    if (dal_check_file_opt_user(i_paras->auth_type, (const gchar *)i_paras->user_name, image_uri,
        (const gchar *)i_paras->user_roleid) != TRUE) {
        (void)create_message_info(MSGID_NO_PRIV_OPT_FILE, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}

LOCAL gint32 check_hotfix_component(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gboolean has_none)
{
    json_object *activated_scope_obj = NULL;
    
    if (json_object_object_get_ex(i_paras->val_jso,
        RFACTION_ACTIVATED_SCOPE, &activated_scope_obj) != TRUE) {
        return RET_OK;
    }
    GHashTable *comp_table = NULL;
    gint32 ret = construct_hotfix_info(i_paras, &comp_table);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: construct_hotfix_info failed.", __FUNCTION__);
        (void)create_message_info(MSGID_GENERAL_ERROR, NULL, o_message_jso);
        return RET_ERR;
    }
    size_t scope_len = json_object_array_length(activated_scope_obj);
    for (size_t idx = 0; idx < scope_len; idx++) {
        const gchar *item = json_object_get_string(json_object_array_get_idx(activated_scope_obj, idx));
        if (has_none && g_strcmp0(item, NONE_STRING) == 0) {
            continue;
        }
        if (g_hash_table_contains(comp_table, item)) {
            if (POINTER_TO_UINT32(g_hash_table_lookup(comp_table, item)) == 0) {
                (void)create_message_info(MSGID_PROP_INVALID_VALUE, NULL,
                    o_message_jso, item, RFACTION_ACTIVATED_SCOPE);
                g_hash_table_unref(comp_table);
                return RET_ERR;
            }
        } else {
            (void)create_message_info(MSGID_PROP_NOT_IN_LIST, NULL,
                o_message_jso, item, RFACTION_ACTIVATED_SCOPE);
            g_hash_table_unref(comp_table);
            return RET_ERR;
        }
    }
    g_hash_table_unref(comp_table);
    return RET_OK;
}


LOCAL gint32 act_update_svc_simple_update(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *image_uri = NULL;
    json_object *body_jso = NULL;
    gint32 ret;
    
    body_jso = i_paras->val_jso;
    

    
    ret = check_hotfix_component(i_paras, o_message_jso, TRUE);
    if (ret != RET_OK) {
        return HTTP_BAD_REQUEST;
    }

    
    ret = upgrade_prep_check(i_paras, o_message_jso);
    return_val_do_info_if_fail(ret == RF_OK, ret, debug_log(DLOG_ERROR, "upgrade pre check failed"));
    
    (void)get_element_str(body_jso, RFACTION_PARAM_IMAGE_URI, &image_uri);
    if (image_uri == NULL || strlen(image_uri) == 0) {
        (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PARAM_IMAGE_URI, o_message_jso,
            RFACTION_PARAM_IMAGE_URI);
        return HTTP_BAD_REQUEST;
    }

    if (check_upgrade_uri(image_uri) != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO, 
            RFACTION_PARAM_IMAGE_URI, RFACTION_UPDATE_SVC_SIMPLE_UPDATE);
        return HTTP_BAD_REQUEST;
    }

    if (image_uri[0] == '/') {
        ret = check_local_image_path(image_uri, RFACTION_UPDATE_SVC_SIMPLE_UPDATE, i_paras, o_message_jso);
        if (ret != HTTP_OK) {
            return ret;
        }
        // 本地升级
        return perform_update_svc_with_local_file(i_paras, o_message_jso, o_result_jso);
    } 

    const gchar *transfer_protocol = NULL;
    (void)get_element_str(body_jso, RFACTION_PARAM_TRANSFER_PROTOCOL, &transfer_protocol);
    if (transfer_protocol == NULL || strlen(transfer_protocol) == 0) {
        (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PARAM_TRANSFER_PROTOCOL, o_message_jso,
            RFACTION_PARAM_TRANSFER_PROTOCOL);
        return HTTP_BAD_REQUEST;
    }

    
    return perform_update_svc_with_remote_file(i_paras, o_message_jso, image_uri, transfer_protocol, o_result_jso);;
}


LOCAL gint32 act_update_svc_simple_update_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param is invalid.", __FUNCTION__);
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = act_update_svc_simple_update(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
    }
    return ret;
}


LOCAL gint32 set_upgrade_sync_state(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, guint8 sync_state)
{
    gint32 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(sync_state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                                        PFN_CLASS_NAME, METHOD_UPGRADE_SET_SYNC_STATE, AUTH_DISABLE,
                                        AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set property value failed. ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 start_sync_update(const gchar *method_name, PROVIDER_PARAS_S *i_paras,
    GSList *input_list, json_object **o_message_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 sync_state = BMC_SYNC_STATE_NONE;

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get object handle for %s failed", __FUNCTION__, OBJECT_PFN);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, PFN_BMC_SYNC_STATE, &sync_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get BMCSyncState failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (sync_state == BMC_SYNC_STATE_UPGRADING || sync_state == BMC_SYNC_STATE_UPGRADE_COMPLETE ||
        sync_state == BMC_SYNC_STATE_SYNC || sync_state == BMC_SYNC_STATE_SYNC_COMPLETE) {
        debug_log(DLOG_ERROR, "%s:Other sync tasks are in progress.", __FUNCTION__);
        (void)create_message_info(MSGID_FW_UPGRADING, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = set_upgrade_sync_state(i_paras, obj_handle, BMC_SYNC_STATE_UPGRADING);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set sync state to (upgrading) failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        PFN_CLASS_NAME, method_name, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Start firmware upgrade failed. ret=%d", __FUNCTION__, ret);
        ret = set_upgrade_sync_state(i_paras, obj_handle, BMC_SYNC_STATE_UPGRADE_FAILED);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Set sync state to (none) failed", __FUNCTION__);
        }
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 update_svc_sync_local(const gchar *image_uri, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;
    guchar forced_upgrade = 0;

    input_list = g_slist_append(input_list, g_variant_new_string(image_uri));
    input_list = g_slist_append(input_list, g_variant_new_byte(forced_upgrade));

    gint32 ret = start_sync_update((const gchar *)METHOD_PFN_INITIATE_UP, i_paras, input_list, o_message_jso);
    uip_free_gvariant_list(input_list);

    return ret;
}


LOCAL gint32 update_svc_sync_remote(const gchar *image_uri, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar transfer_file_uri[MAX_URI_LENGTH] = {0};
    json_object *body_jso = i_paras->val_jso;
    const gchar *transfer_protocol = NULL;

    (void)get_element_str(body_jso, RFACTION_PARAM_TRANSFER_PROTOCOL, &transfer_protocol);
    if (transfer_protocol == NULL || strlen(transfer_protocol) == 0) {
        (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PARAM_TRANSFER_PROTOCOL, o_message_jso,
            RFACTION_PARAM_TRANSFER_PROTOCOL);
        return HTTP_BAD_REQUEST;
    }

    ret = resolve_transfer_uri(image_uri, transfer_protocol, transfer_file_uri, sizeof(transfer_file_uri), 
        o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Resolve transfer uri failed.ret=%d", __FUNCTION__, ret);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)transfer_file_uri));
    input_list = g_slist_append(input_list, g_variant_new_byte(REMOTE_UPDATE_MODE_SYNC));
    ret = start_sync_update((const gchar *)METHOD_PFN_REMOTE_UP, i_paras, input_list, o_message_jso);
    uip_free_gvariant_list(input_list);

    return ret;
}


LOCAL gint32 act_update_svc_sync_update(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *image_uri = NULL;
    json_object *body_jso = i_paras->val_jso;

    gint32 ret = upgrade_prep_check(i_paras, o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Upgrade pre check failed.ret=%d", __FUNCTION__, ret);
        return ret;
    }
    
    (void)get_element_str(body_jso, RFACTION_PARAM_IMAGE_URI, &image_uri);
    if (image_uri == NULL || strlen(image_uri) == 0) {
        (void)create_message_info(MSGID_PROP_MISSING, RFACTION_PARAM_IMAGE_URI, o_message_jso,
            RFACTION_PARAM_IMAGE_URI);
        return HTTP_BAD_REQUEST;
    }

    if (check_upgrade_uri(image_uri) != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_FORMAT_ERR, NULL, o_message_jso, RF_SENSITIVE_INFO, 
            RFACTION_PARAM_IMAGE_URI, RFACTION_UPDATE_SVC_SYNC_UPDATE);
        return HTTP_BAD_REQUEST;
    }

    if (image_uri[0] != '/') {
        return update_svc_sync_remote(image_uri, i_paras, o_message_jso, o_result_jso);
    } 

    ret = check_local_image_path(image_uri, RFACTION_UPDATE_SVC_SYNC_UPDATE, i_paras, o_message_jso);
    if (ret != HTTP_OK) {
        return ret;
    }
    return update_svc_sync_local(image_uri, i_paras, o_message_jso, o_result_jso);
}


LOCAL gboolean get_bmc_sync_upgrade_enable(void)
{
    guint8 sync_enable = 0;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle(PRODUCT) fail ret=%d.", __FUNCTION__, ret);
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PRODUCT_SYNC_UPGRADE_ENABLE, &sync_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get prop(SyncUpgradeEnable) failed. ret=%d", __FUNCTION__, ret);
        return FALSE;
    }

    if (sync_enable == 0) {
        debug_log(DLOG_DEBUG, "%s: BMC sync upgrade is not support.", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 act_update_svc_sync_update_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (get_bmc_sync_upgrade_enable() != TRUE) {
        return HTTP_NOT_FOUND;
    }

    if (i_paras == NULL || i_paras->val_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Input param is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Ouput param is NULL.", __FUNCTION__);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = act_update_svc_sync_update(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_ACCEPTED) {
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_IMAGE_URI);
    }
    return ret;
}


LOCAL gint32 get_upate_svc_tasks(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet = -1;
    gint32 task_id = 0;
    char  task_rsc_path[MAX_URI_LENGTH] = {0};
    GSList *cur_task_list = NULL;
    GSList *task_iter = NULL;
    json_object *task_obj = NULL;
    const gchar *task_name = NULL;
    const gchar *task_status = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    if ((i_paras->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: o_result_jso new obiect is failed.", __FUNCTION__, __LINE__));

    cur_task_list = get_cur_task_list();
    for (task_iter = cur_task_list; task_iter; task_iter = task_iter->next) {
        task_id = GPOINTER_TO_INT(task_iter->data);
        
        task_obj = get_task_rsc_from_id(task_id, NULL, NULL);

        
        (void)get_element_str(task_obj, RF_TASK_STATE_PROP, &task_status);
        (void)get_element_str(task_obj, RFOBJ_NAME, &task_name);

        if ((0 == g_strcmp0(task_name, RF_UPGRADE_TASK_DESC)) &&
            (0 == g_strcmp0(task_status, RF_MONITOR_TASK_RUNNING))) {
            iRet = snprintf_s(task_rsc_path, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, "%s%d", TASK_COLLECTION_PATH, task_id);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            json_object_object_add(*o_result_jso, ODATA_ID, json_object_new_string((const gchar *)task_rsc_path));
            (void)json_object_put(task_obj);
            g_slist_free(cur_task_list);
            return HTTP_OK;
        }
        (void)json_object_put(task_obj);
    }
    g_slist_free(cur_task_list);
    (void)json_object_put(*o_result_jso);
    *o_result_jso = NULL;

    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 cal_sync_progress(guint8 upgrade_progress, guint8 state, gchar *state_str, gint32 buff_size)
{
    gint32 ret;
    guint8 base;
    guint8 progress;

    
    base = ((state == BMC_SYNC_STATE_SYNC) ? PFN_BMC_SYNC_PROGRESS_BASE : 0);
    progress = (upgrade_progress >> 1) + base;
    progress = ((progress > PFN_BMC_SYNC_TRANS_PROGRESS_BASE) ? progress : PFN_BMC_SYNC_TRANS_PROGRESS_BASE);

    ret = sprintf_s(state_str, buff_size, "%u%%", progress);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 convert_sync_state(gchar *state_str, gint32 buff_size, guint8 state)
{
    gint32 ret;
    guint8 upgrade_progress = 0;
    guint8 upgrade_in_process = 0;
    OBJ_HANDLE obj_handle = 0;

    
    const SYNC_STATE_MAP sync_state_map[] = {
        {BMC_SYNC_STATE_TRANS_TRANSFERRING, "3%"},
        {BMC_SYNC_STATE_UPGRADE_COMPLETE, "50%"},
        {BMC_SYNC_STATE_SYNC_COMPLETE, "100%"},
        {BMC_SYNC_STATE_COMPLETE, "Completed"},
        {BMC_SYNC_STATE_TRANS_FAILED, "FileTransferFailed"},
        {BMC_SYNC_STATE_UPGRADE_FAILED, "UpgradeFailed"},
        {BMC_SYNC_STATE_SYN_FAILED, "SynchronizationFailed"},
        {BMC_SYNC_STATE_NONE, "null"}
    };

    if (state == BMC_SYNC_STATE_UPGRADING || state == BMC_SYNC_STATE_SYNC) {
        ret = dal_get_object_handle_nth(CLASS_NAME_UPGRADE, 0, &obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get object handle(Upgrade) fail ret=%d.", __FUNCTION__, ret);
            return RET_ERR;
        }

        (void)dal_get_property_value_byte(obj_handle, PFN_UPGRADE_INFLAG_NAME, &upgrade_in_process);
        if (upgrade_in_process == 0) {
            upgrade_progress = 0;
        } else {
            ret = dal_get_property_value_byte(obj_handle, PFN_UPGRADE_STATE_NAME, &upgrade_progress);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Get upgrade progress failed (ret:%d)", __FUNCTION__, ret);
                return RET_ERR;
            }
        }

        return cal_sync_progress(upgrade_progress, state, state_str, buff_size);
    }

    for (gint32 i = 0; i < G_N_ELEMENTS(sync_state_map); i++) {
        if (sync_state_map[i].code != state) {
            continue;
        }
        ret = strncpy_s(state_str, buff_size, sync_state_map[i].state, strlen(sync_state_map[i].state));
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret=%d", __FUNCTION__, ret);
            return RET_ERR;
        }
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "%s: State is not found", __FUNCTION__);
    return RET_ERR;
}


LOCAL void get_update_svc_sync_state_prop(OBJ_HANDLE upg_obj_handle, json_object** huawei_json)
{
    gint32 ret;
    guint8 sync_state;
    OBJ_HANDLE obj_handle = 0;
    gchar state_str[PROP_VAL_LENGTH] = {0};

    if (get_bmc_sync_upgrade_enable() != TRUE) {
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_PRODUCT, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle(PRODUCT) fail ret=%d.", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_property_value_byte(upg_obj_handle, PFN_BMC_SYNC_STATE, &sync_state);
    if (ret == RET_OK && convert_sync_state(state_str, sizeof(state_str), sync_state) == RET_OK) {
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_SYNCUPDATESTATE, json_object_new_string(state_str));
        return;
    }

    debug_log(DLOG_ERROR, "%s: Get sync state fail. ret=%d", __FUNCTION__, ret);

    json_object_object_add((*huawei_json), RF_UPDATE_SVC_SYNCUPDATESTATE, json_object_new_string("null"));
}


LOCAL gint32 is_bios_upgrade(OBJ_HANDLE upg_obj_handle)
{
    guint8 compnent_id = 0;

    gint32 ret = dal_get_property_value_byte(upg_obj_handle, PFN_COMP_ID_NAME, &compnent_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get prop(CompnentID) failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (compnent_id != COMP_BIOS) {
        return RF_UPGRADE_STATE_NO_BIOS_UPGRADE;
    }
    return RF_UPGRADE_STATE_BIOS_IN_UPGRADE;
}


LOCAL void get_bios_upgrade_state(OBJ_HANDLE upg_obj_handle, json_object** huawei_json)
{
    const char *upgrede_completed = "Completed";
    const char *upgrade_failed = "UpgradeFailed";
    const char *upgrade_null = "null";
    guint8 upgrade_state = 0;
    guchar in_upgrade_flag = 0;
    gchar task_progress_str[RF_MAX_PROGRESS_LEN] = {0};

    gint32 ret = is_bios_upgrade(upg_obj_handle);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: is_bios_upgrade failed. ret=%d", __FUNCTION__, ret);
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_BIOSUPDATESTATE, json_object_new_string(upgrade_failed));
        return;
    } else if (ret == RF_UPGRADE_STATE_NO_BIOS_UPGRADE) {
        debug_log(DLOG_DEBUG, "%s: bios is not upgrading. ret=%d", __FUNCTION__, ret);
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_BIOSUPDATESTATE, json_object_new_string(upgrade_null));
        return;
    }

    ret = dal_get_property_value_byte(upg_obj_handle, PFN_UPGRADE_STATE_NAME, &upgrade_state);
    if (ret != RET_OK) {
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_BIOSUPDATESTATE, json_object_new_string(upgrade_failed));
        debug_log(DLOG_ERROR, "%s: Get prop(UpgradeState) failed. ret=%d", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_property_value_byte(upg_obj_handle, PFN_UPGRADE_INFLAG_NAME, &in_upgrade_flag);
    if (ret != RET_OK) {
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_BIOSUPDATESTATE, json_object_new_string(upgrade_failed));
        debug_log(DLOG_ERROR, "%s: Get prop(InUpgradeFlag) failed. ret=%d", __FUNCTION__, ret);
        return;
    }

    if (in_upgrade_flag == FALSE) {
        
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_BIOSUPDATESTATE,
            json_object_new_string(upgrede_completed));
    } else {
        (void)snprintf_s(task_progress_str, RF_MAX_PROGRESS_LEN, RF_MAX_PROGRESS_LEN - 1, "%u%%", upgrade_state);
        json_object_object_add((*huawei_json), RF_UPDATE_SVC_BIOSUPDATESTATE,
            json_object_new_string(task_progress_str));
    }
}


LOCAL gint32 get_update_svc_oem_prop(PROVIDER_PARAS_S* i_paras, json_object** huawei_json)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    guchar downgrade_disabled_flag;

    
    *huawei_json = json_object_new_object();
    if (*huawei_json == NULL) {
        debug_log(DLOG_ERROR, "New json object failed.");
        return RET_ERR;
    }
    
    (void)dfl_get_object_handle(CLASS_NAME_UPGRADE, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PFN_DOWNGRADE_DISABLED, &downgrade_disabled_flag);
    parse_single_value(ret == RET_OK, *huawei_json, RF_UPDATE_SVC_DOWNGRADE_DISABLED, downgrade_disabled_flag, NULL);
    
    get_update_svc_sync_state_prop(obj_handle, huawei_json);
    get_bios_upgrade_state(obj_handle, huawei_json);

    return RET_OK;
}


LOCAL gint32 get_update_svc_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;
    json_object* oem_json = NULL;
     
    if (o_result_jso ==  NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Invalid input parameters.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }
    ret = get_update_svc_oem_prop(i_paras, &oem_json);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "UpdateService get oem properties failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (o_result_jso == NULL) {
        json_object_put(oem_json);
        debug_log(DLOG_ERROR, "New json object failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, oem_json);

    return HTTP_OK;
}


LOCAL gint32 get_simple_update_action(json_object** o_result_jso)
{
#define SIMPLE_UPDATE_URI   "/redfish/v1/UpdateService/Actions/UpdateService.SimpleUpdate"
#define SIMPLE_ACTION_INFO   "/redfish/v1/UpdateService/SimpleUpdateActionInfo"
#define SIMPLE_ACTION_OID   "#UpdateService.SimpleUpdate"
    json_object* simple_update_json = NULL;
    simple_update_json = json_object_new_object();
    if (simple_update_json == NULL) {
        debug_log(DLOG_ERROR, "%s:New json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    (void)json_object_object_add(simple_update_json, RFPROP_TARGET, json_object_new_string(SIMPLE_UPDATE_URI));
    (void)json_object_object_add(simple_update_json, ACTION_INFO, json_object_new_string(SIMPLE_ACTION_INFO));
    (void)json_object_object_add(*o_result_jso, SIMPLE_ACTION_OID, simple_update_json);

#undef SIMPLE_UPDATE_URI
#undef SIMPLE_ACTION_INFO
#undef SIMPLE_ACTION_OID

    return RET_OK;
}


LOCAL gint32 get_sync_update_action(json_object** o_result_jso)
{
    if (get_bmc_sync_upgrade_enable() != TRUE) {
        return RET_OK;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:New json object(sync update json) failed.", __FUNCTION__);
        return RET_ERR;
    }

    (void)json_object_object_add(*o_result_jso, RFPROP_TARGET, json_object_new_string(RF_BMC_SYNC_UPDATE_URI));
    (void)json_object_object_add(*o_result_jso, ACTION_INFO, json_object_new_string(RF_BMC_SYNC_UPDATE_ACTIONINFO_URI));

    return RET_OK;
}


LOCAL gint32 get_activate_bios_action(json_object** o_result_jso)
{
    
    if (dal_match_product_id(PRODUCT_ID_TIANCHI) != TRUE) {
        return RET_OK;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:New json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    (void)json_object_object_add(*o_result_jso, RFPROP_TARGET, json_object_new_string(RF_ACTIVATE_BIOS_URI));
    (void)json_object_object_add(*o_result_jso, ACTION_INFO, json_object_new_string(RF_ACTIVATE_BIOS_ACTIONINFO_URI));

    return RET_OK;
}


LOCAL gint32 get_oem_action_info(json_object** o_result_jso)
{
    gint32 ret;
    json_object* oem_json = NULL;
    json_object* hw_json = NULL;
    json_object* sync_update_json = NULL;
    json_object* activate_bios_json = NULL;

    ret = get_sync_update_action(&sync_update_json);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:UpdateService get sync update info failed.", __FUNCTION__);
        goto err_exit;
    }

    ret = get_activate_bios_action(&activate_bios_json);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:UpdateService get activate bios info failed.", __FUNCTION__);
        goto err_exit;
    }
    if (sync_update_json == NULL && activate_bios_json == NULL) {
        return RET_OK;
    }

    oem_json = json_object_new_object();
    if (oem_json == NULL) {
        debug_log(DLOG_ERROR, "%s:New json object(oem action json) failed.", __FUNCTION__);
        goto err_exit;
    }

    hw_json = json_object_new_object();
    if (hw_json == NULL) {
        debug_log(DLOG_ERROR, "%s:New json object(hw json) failed.", __FUNCTION__);
        goto err_exit;
    }

    if (sync_update_json != NULL) {
        (void)json_object_object_add(hw_json, "#UpdateService.StartSyncUpdate", sync_update_json);
    }
    if (activate_bios_json != NULL) {
        (void)json_object_object_add(hw_json, "#UpdateService.ActivateBios", activate_bios_json);
    }
    (void)json_object_object_add(oem_json, RFPROP_OEM_HUAWEI, hw_json);
    (void)json_object_object_add(*o_result_jso, RFPROP_OEM, oem_json);

    return RET_OK;

err_exit:
    (void)json_object_put(sync_update_json);
    (void)json_object_put(activate_bios_json);
    (void)json_object_put(oem_json);
    return RET_ERR;
}


LOCAL gint32 get_update_svc_action(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret;

     
    if (o_result_jso ==  NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Invalid input parameters.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:New json object failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_simple_update_action(o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:UpdateService get simple update info failed.", __FUNCTION__);
        (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_oem_action_info(o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:UpdateService get oem action info failed.", __FUNCTION__);
        (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_downgrade_disabled_flag(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, 
                                         json_object** o_result_jso, json_object *json_obj)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList* input_list = NULL;
    json_object* message_temp_jso = NULL;
    const gchar* prop_str = "Oem/Huawei/DowngradeDisabled";
    guint8 downgrade_disabled;

    ret = dfl_get_object_handle(OBJ_NAME_UPGRADE, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s get Upgrade handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*o_message_jso, message_temp_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    downgrade_disabled = json_object_get_boolean(json_obj) ? 1 : 0;
    input_list = g_slist_append(input_list, g_variant_new_byte(downgrade_disabled));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                                        CLASS_NAME_UPGRADE, METHOD_PFN_SET_DOWNGRADE_DISABLED, AUTH_ENABLE,
                                        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            ret = HTTP_OK;
            break;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, prop_str, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret =  RFERR_INSUFFICIENT_PRIVILEGE;
            break;
        
        case RFERR_NO_RESOURCE:
            (void)create_message_info(MSGID_RSC_MISSING_URI, prop_str, &message_temp_jso, i_paras->uri);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret =  HTTP_NOT_FOUND;
            break;
        
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_str, &message_temp_jso);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            debug_log(DLOG_ERROR, "Unknown error：%d.", ret);
            ret =  HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


LOCAL gint32 set_update_svc_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    json_object *val_json = NULL;
    json_bool ret_bool;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *json_obj = NULL;

    
    if (provider_paras_check(i_paras) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &val_json);
    if (ret_bool != TRUE || val_json == NULL) {
        return HTTP_BAD_REQUEST;
    }
    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "New json array failed.");
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    
    if (json_object_object_get_ex(val_json, RF_UPDATE_SVC_DOWNGRADE_DISABLED, &json_obj) == TRUE) {
        ret_code = set_downgrade_disabled_flag(i_paras, o_message_jso, o_result_jso, json_obj);
    }

    return ret_code;
}


LOCAL gint32 check_bios_upgrade_state(void)
{
    guint8 bios_active_mode = BIOS_ACTIVE_MODE_NONE;
    (void)dal_get_func_ability(PFN_CLASS_NAME, PROTERY_UPGRADE_BIOS_CURRENT_ACTIVE_MODE, &bios_active_mode);
    if (bios_active_mode != BIOS_ACTIVE_MODE_NONE) {
        debug_log(DLOG_ERROR, "bios is online upgrading, active_mode: %d", bios_active_mode);
        return VOS_ERR;
    }
    return VOS_OK;
}


LOCAL gint32 handle_activate_bios_err(gint32 err_code, json_object **o_message_jso)
{
    const gchar *action_name = ACTION_NAME_ACTIVATE_BIOS;
    if (o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    switch (err_code) {
        case COMP_CODE_FW_UPDATING:
            debug_log(DLOG_ERROR, "%s: Cannot activate BIOS because BIOS flash is upgrading.", __FUNCTION__);
            (void)create_message_info(MSGID_ACT_FAILED_BY_BIOS_UPGRADE, NULL, o_message_jso, action_name);
            return HTTP_BAD_REQUEST;
        case COMP_CODE_STATUS_INVALID:
            debug_log(DLOG_ERROR, "%s: Cannot activate BIOS because BIOS status is not ok.", __FUNCTION__);
            (void)create_message_info(MSGID_BIOS_STATE_NOT_ALLOWED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            debug_log(DLOG_ERROR, "%s: Cannot activate BIOS, err_code: %d.", __FUNCTION__, err_code);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL guint8 get_active_mode(json_object *val_jso)
{
    guint8 mode = 0;
    size_t size = json_object_array_length(val_jso);
    if (size == 0) {
        return BIOS_ACTIVE_MODE_NONE;
    }
    for (size_t i = 0; i < size; i++) {
        const gchar *item = json_object_get_string(json_object_array_get_idx(val_jso, i));
        mode |= dal_bios_active_mode_str2int(item, g_active_mode_tbl);
    }
    return mode;
}

LOCAL gint32 check_before_recover(json_object **o_message_jso, const gchar *action_name, PROVIDER_PARAS_S *i_paras)
{
    guint8 bios_status = 0;
    
    if (!product_support_hotupgrade()) {
        return HTTP_METHOD_NOT_ALLOWED;
    }
    
    (void)dal_get_func_ability(CLASS_NAME_BIOS, PROPERTY_BIOS_STARTUP_STATE, &bios_status);
    if (bios_status != BIOS_STARTUP_POST_STAGE_FINISH) {
        (void)create_message_info(MSGID_BIOS_STATE_NOT_ALLOWED, NULL, o_message_jso);
        GSList *caller_info = NULL;
        caller_info =
            g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH,
            "BIOS hot upgrade fails due to abnormal BIOS status");
        uip_free_gvariant_list(caller_info);
        return HTTP_BAD_REQUEST;
    }
    
    if (check_bios_upgrade_state() != VOS_OK) {
        (void)create_message_info(MSGID_ACT_FAILED_BY_BIOS_UPGRADE, NULL, o_message_jso, action_name);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}


LOCAL gint32 act_update_activate_bios_entry(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    guint8 activate_mode = BIOS_ACTIVE_MODE_NONE;
    const gchar *action_name = ACTION_NAME_ACTIVATE_BIOS;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE pfn_obj = 0;
    
    if (provider_paras_check(i_paras) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object *activated_scope = NULL;
    json_bool ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_ACTIVATED_SCOPE, &activated_scope);
    if (ret_bool == FALSE) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }
    if (json_object_get_type(activated_scope) != json_type_array) {
        debug_log(DLOG_ERROR, "%s: BiosActivatedScope is not an array.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }
    if (json_object_array_length(activated_scope) == 0) {
        debug_log(DLOG_ERROR, "%s: BiosActivatedScope is empty.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_VAL_NOT_EMPTY, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    gint32 ret = check_hotfix_component(i_paras, o_message_jso, FALSE);
    if (ret != RET_OK) {
        return HTTP_BAD_REQUEST;
    }
    ret = check_before_recover(o_message_jso, action_name, i_paras);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get upgrade object handle failed.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }
    activate_mode = get_active_mode(activated_scope);
    input_list = g_slist_append(input_list, g_variant_new_byte(activate_mode));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, pfn_obj,
        PFN_CLASS_NAME, METHOD_PFN_BIOS_HOT_UPGRADE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    if (ret != COMP_CODE_SUCCESS) {
        return handle_activate_bios_err(ret, o_message_jso);
    }
    return HTTP_ACCEPTED;
}

LOCAL PROPERTY_PROVIDER_S g_update_svc_provider[] = {
    {
        RFACTION_UPDATE_SVC_SYNC_UPDATE,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_update_svc_sync_update_entry,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_UPDATE_SVC_SIMPLE_UPDATE,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_update_svc_simple_update_entry,
        ETAG_FLAG_ENABLE
    },
    {
        TASK_CLASS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_upate_svc_tasks,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_COMMON_OEM,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_update_svc_oem,
        set_update_svc_oem,
        NULL, 
        ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACIONS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_update_svc_action,
        NULL,
        NULL, 
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_UPDATE_ACTIVATE_BIOS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_BASICSETTING,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_update_activate_bios_entry,
        ETAG_FLAG_ENABLE
    }
};


gint32 update_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), VOS_ERR,
        debug_log(DLOG_ERROR, "input param error"));

    *prop_provider = g_update_svc_provider;
    *count = G_N_ELEMENTS(g_update_svc_provider);

    return VOS_OK;
}

gint32 sync_up_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    if ((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)) {
        debug_log(DLOG_ERROR, "Input param error");
        return RET_ERR;
    }

    if (get_bmc_sync_upgrade_enable() != TRUE) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_actioninfo_bmc_sync_up_provider;
    *count = 0;

    return RET_OK;
}

gint32 activate_biosinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    if ((i_paras == NULL) || (prop_provider == NULL) || (count == NULL)) {
        debug_log(DLOG_ERROR, "Input param error");
        return RET_ERR;
    }

    
    if (dal_match_product_id(PRODUCT_ID_TIANCHI) != TRUE) {
        return RET_ERR;
    }

    *prop_provider = g_manager_actioninfo_activate_bios;
    *count = 0;

    return RET_OK;
}
