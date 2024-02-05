
#include "redfish_provider.h"
#include <libgen.h>

#define VIDEO_JNLP_FORMAT \
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
	<jnlp spec=\"1.0+\" codebase=\"https://%s\">\
		<information>\
			<title>Video Player   IP : %s   SN : %s</title>\
			<vendor>iBMC</vendor>\
		</information>\
		<resources>\
			<j2se version=\"1.7+\" />\
			<jar href=\"/bmc/pages/jar/%s?authParam=%s\" main=\"true\"/>\
		</resources>\
		<applet-desc name=\"Video Player   IP : %s   SN : %s\" main-class=\"com.huawei.kinescope.ui.KinAppletPlay\" width=\"950\" height=\"700\" >\
			<param name=\"local\" value=\"%s\"/>\
			<param name=\"videoType\" value=\"%u\"/>\
			<param name=\"press\" value=\"%u\"/>\
			<param name=\"salt\" value=\"%s\"/>\
			<param name=\"videokey\" value=\"%s\"/>\
			<param name=\"port\" value=\"%d\"/>\
			<param name=\"bladesize\" value=\"1\"/>\
			<param name=\"ip\" value=\"%s\"/>\
		</applet-desc>\
		<security>\
			<all-permissions/>\
		</security>\
	</jnlp>"

#define NPU_LOG_FILE_NAME "/tmp/npu_log.tar.gz"
#define MIN_NPU_ID 1
#define MAX_NPU_ID 8

LOCAL const gchar *g_diagnostic_action_array[] = {
    RFACTION_STOP_VIDEO_PLAYBACK,
    RFACTION_EXPORT_VIDEO,
    RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE,
    RFACTION_CAPTURE_SCREEN_SHOT,
    RFACTION_DELETE_SCREEN_SHOT,
    RFACTION_EXPORT_BLACK_BOX,
    RFACTION_EXPORT_SERIAL_PORT_DATA,
    RFACTION_EXPORT_NPU_LOG
};


LOCAL gint32 get_manager_diagnostic_service_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 i;
    gint32 ret;
    guint32 len;
    json_object *action_jso = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar rsc_uri[MAX_URI_LENGTH] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(rsc_uri, sizeof(rsc_uri), sizeof(rsc_uri) - 1, MANAGER_DIAGNOSTIC_ODATAID, slot);
    return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));
    action_jso = json_object_new_object();
    if (NULL == action_jso) {
        debug_log(DLOG_ERROR, "alloc new object failed");
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    len = G_N_ELEMENTS(g_diagnostic_action_array);

    for (i = 0; i < len; i++) {
        rf_add_action_prop(action_jso, rsc_uri, g_diagnostic_action_array[i]);
    }

    *o_result_jso = action_jso;

    return HTTP_OK;
}


LOCAL gint32 get_diagnostic_enabled_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, gchar *class_name, gchar *property_name)
{
    gint32 ret;
    guint8 enabled = 0;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_object_handle_nth(class_name, 0, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, property_name, &enabled);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: Get enabled state failed. ", __FUNCTION__, __LINE__));
    (1 == enabled) ? (*o_result_jso = json_object_new_boolean(TRUE)) : (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}


LOCAL gint32 set_diagnostic_enabled_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, gchar *enabled_str, gchar *class_name, gchar *method_name)
{
    gint32 ret;
    guint8 enabled;
    json_bool enabled_bool;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, enabled_str, o_message_jso, enabled_str));

    enabled_bool = json_object_get_boolean(i_paras->val_jso);
    (TRUE == enabled_bool) ? (enabled = 1) : (enabled = 0);

    ret = dal_get_object_handle_nth(class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s, %d: get %s handle failed.", __FUNCTION__, __LINE__, class_name);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(enabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        class_name, method_name, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_serialportdata_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 enabled = 0;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);
    ret = dal_get_property_value_int32(obj_handle, PROPERTY_SOL_DATA_STATE, &enabled);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: Get enabled state failed. ", __FUNCTION__, __LINE__));
    (1 == enabled) ? (*o_result_jso = json_object_new_boolean(TRUE)) : (*o_result_jso = json_object_new_boolean(FALSE));
    return HTTP_OK;
}

LOCAL gint32 set_serialportdata_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 enabled;
    json_bool enabled_bool;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_SERIALPORTDATA_ENABLED, o_message_jso,
        PROPERTY_SERIALPORTDATA_ENABLED));

    enabled_bool = json_object_get_boolean(i_paras->val_jso);
    (TRUE == enabled_bool) ? (enabled = 1) : (enabled = 0);

    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_int32(enabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_SET_SOL_STATE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 get_dfp_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 enabled = 0;
    OBJ_HANDLE obj_handle = 0;
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_object_handle_nth(CLASS_DFP, 0, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_DFP_ENABLE, &enabled);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get enabled state failed. ", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = (enabled == TRUE) ? json_object_new_boolean(TRUE) : json_object_new_boolean(FALSE);
    return HTTP_OK;
}

LOCAL gint32 set_dfp_service_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
#define DFP_DISABLE 0
#define DFP_ENABLE  1
    gint32 ret;
    guint8 enabled;
    json_bool enabled_bool;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, PROPERTY_DFP_ENABLED, o_message_jso,
            PROPERTY_DFP_ENABLED);
        debug_log(DLOG_ERROR, "%s, %d", __FUNCTION__, HTTP_FORBIDDEN);
        return HTTP_FORBIDDEN;
    }

    enabled_bool = json_object_get_boolean(i_paras->val_jso);
    if (enabled_bool == TRUE) {
        enabled = DFP_ENABLE;
    } else if (enabled_bool == FALSE) {
        enabled = DFP_DISABLE;
    } else {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    (void)dal_get_object_handle_nth(CLASS_DFP, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_byte(enabled));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_DFP, METHOD_DFP_SET_DFP_ENABLE, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case DFL_OK:
            return HTTP_OK;
        case ERRCODE_ROUTE_NOT_FOUND:
            (void)create_message_info(MSGID_OPERATION_NOT_SUPPORTED, PROPERTY_DFP_ENABLED, o_message_jso);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 copy_screenshot_img_to_tmp_path(PROVIDER_PARAS_S *i_paras)
{
    gint32 ret = 0;
    gint32 i;
    gchar img_src_path[MAX_FILEPATH_LENGTH] = {0};
    gchar img_dst_path[MAX_FILEPATH_LENGTH] = {0};

    
    if (FALSE == vos_check_dir(SCREEN_FILE_WEB_PATH)) {
        (void)vos_mkdir_recursive(SCREEN_FILE_WEB_PATH);
    }
    (void)chmod(SCREEN_FILE_WEB_PATH, 0770);
    (void)chown(SCREEN_FILE_WEB_PATH, APACHE_UID, APPS_USER_GID);

    
    if (TRUE == vos_get_file_accessible(IMG_PATH)) {
        // redfish模块降权限后，调用代理方法拷贝文件
        (void)proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, IMG_PATH, IMG_WEB_PATH,
            APACHE_UID, APACHE_GID, COPY_FILE_RESERVE_FLAG);

        (void)proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
            IMG_PATH, IMG_TMP_WEB_PATH, APACHE_UID, APACHE_GID, COPY_FILE_RESERVE_FLAG);
    }

    
    for (i = 0; i < 3; i++) {
        (void)memset_s(img_src_path, sizeof(img_src_path), 0, sizeof(img_src_path));
        (void)memset_s(img_dst_path, sizeof(img_dst_path), 0, sizeof(img_dst_path));

        ret = snprintf_s(img_src_path, sizeof(img_src_path), sizeof(img_src_path) - 1, "%s%s%d%s", SCREEN_FILE_PATH,
            AUTO_SCREEN_FILE_PREFIX, (i + 1), AUTO_SCREEN_FILE_SUFFIX);

        do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

        if (TRUE == vos_get_file_accessible(img_src_path)) {
            ret = snprintf_s(img_dst_path, sizeof(img_dst_path), sizeof(img_dst_path) - 1, "%s%s%d%s",
                SCREEN_FILE_WEB_PATH, AUTO_SCREEN_FILE_PREFIX, (i + 1), AUTO_SCREEN_FILE_SUFFIX);

            do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret));

            // redfish模块降权限后，调用代理方法拷贝文件
            (void)proxy_copy_file(i_paras->is_from_webui, i_paras->user_name, i_paras->client, img_src_path,
                img_dst_path, APACHE_UID, APACHE_GID, 0);
        }
    }
    return VOS_OK;
}


LOCAL gint32 get_pcieinterface_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, CLASS_DIAGNOSE_NAME,
        PROPERTY_PCIEINTERFACE_SWITCH);
}


LOCAL gint32 get_blackbox_dump_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, CLASS_DIAGNOSE_NAME,
        PROPERTY_BLACKBOX_STATE);
}


LOCAL gint32 get_blackbox_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:Input para is pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    OBJ_HANDLE obj_handle = 0;
    guint8 pcie_interface_enabled = 0;
    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIEINTERFACE_SWITCH, &pcie_interface_enabled);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get PCIe interface state failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    guint8 black_box_dump_enabled = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_BLACKBOX_STATE, &black_box_dump_enabled);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get black box state failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (pcie_interface_enabled == 1 && black_box_dump_enabled == 1) {
        *o_result_jso = json_object_new_boolean(TRUE);
    } else {
        *o_result_jso = json_object_new_boolean(FALSE);
    }

    return HTTP_OK;
}


LOCAL gint32 set_pcieinterface_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_PCIEINTERFACE_ENABLED,
        CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_SET_PCIEINTERFACE_SWITCH);
}


LOCAL gint32 set_blackbox_dump_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_BLACKBOXDUMP_ENABLED,
        CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_SET_BLACKBOX_DUMP_STATE);
}


LOCAL gint32 set_blackbox_state(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    if (provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s:Input para is pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (json_object_get_boolean(i_paras->val_jso) == TRUE) {
        ret = set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_BLACKBOX_ENABLED,
            CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_SET_PCIEINTERFACE_SWITCH);
        if (ret != HTTP_OK) {
            return ret;
        }

        ret = set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_BLACKBOX_ENABLED,
            CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_SET_BLACKBOX_DUMP_STATE);
        if (ret != HTTP_OK) {
            return ret;
        }
    } else {
        ret = set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_BLACKBOX_ENABLED,
            CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_SET_PCIEINTERFACE_SWITCH);
        if (ret != HTTP_OK) {
            return ret;
        }
    }

    return HTTP_OK;
}

LOCAL gint32 get_screenshot_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    (void)copy_screenshot_img_to_tmp_path(i_paras);
    
    return get_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, KVM_CLASS_NAME, SCREEN_PROPERTY_SWITCH);
}

LOCAL gint32 set_screenshot_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_SCREENSHOT_ENABLED,
        PROXY_KVM_CLASS_NAME, KVM_METHOD_SCREEN_SWITCH);
}

LOCAL gint32 get_video_recording_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, VIDEO_CLASS_NAME, VIDEO_PROPERTY_SWITCH);
}

LOCAL gint32 set_video_recording_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return set_diagnostic_enabled_info(i_paras, o_message_jso, o_result_jso, PROPERTY_VIDEO_RECORDING_ENABLED,
        VIDEO_CLASS_NAME, VIDEO_METHOD_SET_SWITCH);
}

LOCAL gint32 get_video_playback_conn_num(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 connect_num = 0;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_object_handle_nth(VIDEO_CLASS_NAME, 0, &obj_handle);
    ret = dal_get_property_value_int32(obj_handle, VIDEO_PROPERTY_NUM, &connect_num);

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: Get video playback conn num failed. ", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_int(connect_num);

    return HTTP_OK;
}

LOCAL gint32 get_video_create_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *tmp_obj = NULL;
    guint32 i;
    gint32 ret = 0;
    struct tm local_time = { 0 };
    struct stat file_stat;
    gchar buffer[MAX_LINE_SIZE] = {0};
    glong file_len = 0;

    gchar video_files[][MAX_LINE_SIZE] = {VIDEO_PATH_CATERROR, VIDEO_PATH_POWEROFF, VIDEO_PATH_OSRESET};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: New json object failed. ", __FUNCTION__, __LINE__));

    for (i = 0; i < sizeof(video_files) / sizeof(video_files[0]); i++) {
        ret = stat(video_files[i], &file_stat);
        if (ret != 0) {
            json_object_array_add(*o_result_jso, NULL);
        } else {
            file_len = vos_get_file_length(video_files[i]);

            (void)localtime_r(&file_stat.st_ctime, &local_time);
            (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%04d-%02d-%02d %02d:%02d:%02d",
                local_time.tm_year + YEAR_1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour,
                local_time.tm_min, local_time.tm_sec);

            tmp_obj = json_object_new_object();
            json_object_array_add(*o_result_jso, tmp_obj);

            json_object_object_add(tmp_obj, PROPERTY_VIDEO_SIZE_BYTE, json_object_new_int(file_len));
            json_object_object_add(tmp_obj, PROPERTY_CREATE_TIME, json_object_new_string((const gchar *)buffer));
            tmp_obj = NULL;
        }
    }

    return HTTP_OK;
}

LOCAL gint32 get_screenshot_create_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 i;
    gint32 ret = 0;
    struct tm local_time = { 0 };
    struct stat file_stat;
    gchar buffer[MAX_LINE_SIZE] = {0};
    gchar img_files[][MAX_LINE_SIZE] = {"/data/share/img/img1.jpeg", "/data/share/img/img2.jpeg",
                                        "/data/share/img/img3.jpeg", "/data/share/img/manualscreen.jpeg"};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: New json object failed. ", __FUNCTION__, __LINE__));

    for (i = 0; i < sizeof(img_files) / sizeof(img_files[0]); i++) {
        ret = stat(img_files[i], &file_stat);
        if (ret != 0) {
            json_object_array_add(*o_result_jso, NULL);
        } else {
            (void)localtime_r(&file_stat.st_ctime, &local_time);
            (void)snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%04d-%02d-%02d %02d:%02d:%02d",
                local_time.tm_year + YEAR_1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour,
                local_time.tm_min, local_time.tm_sec);
            json_object_array_add(*o_result_jso, json_object_new_string((const gchar *)buffer));
        }
    }

    return HTTP_OK;
}

LOCAL gint32 get_manager_diagnostic_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso, MANAGER_DIAGNOSTIC_ODATAID);
}

LOCAL gint32 get_manager_diagnostic_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_universal_provider_odata_info(i_paras, o_message_jso, o_result_jso, MANAGER_DIAGNOSTIC_CONTEXT);
}


LOCAL gint32 get_ref_sol_uart_obj(GSList **ref_comobj_list)
{
#define SOL_FILEANME_MAXLENGTH   25
    gint32 ret;
    GVariant *array_obj_name_gvar = NULL;
    GVariantIter iter;
    const gchar *obj_name = NULL;
    GVariant *obj_name_gvar = NULL;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE cur_refobj_handle = 0;

    ret = dfl_get_object_handle(OBJ_DIAGNOSE_NAME, &obj_handle);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_SOLUARTOBJ, &array_obj_name_gvar);
    if (ret != DFL_OK) {
        return RET_ERR;
    }

    g_variant_iter_init(&iter, array_obj_name_gvar);
    while ((obj_name_gvar = g_variant_iter_next_value(&iter)) != NULL) {
        cur_refobj_handle = 0;
        obj_name = g_variant_get_string(obj_name_gvar, NULL);
        ret = dfl_get_object_handle(obj_name, &cur_refobj_handle);
        g_variant_unref(obj_name_gvar);
        obj_name_gvar = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s failed, ret = %d", __FUNCTION__, ret);
            g_variant_unref(array_obj_name_gvar);
            return RET_ERR;
        }
        *ref_comobj_list = g_slist_append(*ref_comobj_list, (gpointer)cur_refobj_handle);
    }
    g_variant_unref(array_obj_name_gvar);
    return RET_OK;
}


LOCAL gint32 get_cur_com_log_size(OBJ_HANDLE obj_handle, gint32 *len)
{
    gint32 ret;
    guint8 sol_enable = 0;
    guint8 sol_data_direction = 0;
    guint8 txsize = 0;
    guint8 rxsize = 0;
    gint32 config_size_index;
    guint32 sol_ddr_size[] = {1, 2, 4, 8, 16, 32, 64, 128};

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOL_ENABLED, &sol_enable);
    if (sol_enable != 1) {
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_DATADIRECTION, &sol_data_direction);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj %s fail, ret = %d", __FUNCTION__, PROPERTY_SOL_DATADIRECTION, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_TXSIZE, &txsize);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj %s fail, ret = %d", __FUNCTION__, PROPERTY_SOL_TXSIZE, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_RXSIZE, &rxsize);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj %s fail, ret = %d", __FUNCTION__, PROPERTY_SOL_RXSIZE, ret);
        return RET_ERR;
    }

    
    if (sol_data_direction == 0) {
        config_size_index = txsize;
    } else {
        config_size_index = rxsize;
    }

    if (config_size_index < (sizeof(sol_ddr_size) / sizeof(guint32))) {
        *len = sol_ddr_size[config_size_index] * QUARTER_M_SIZE;
    } else {
        debug_log(DLOG_ERROR, "%s: config len val is %d,out of index", __FUNCTION__, config_size_index);
        return RET_ERR;
    }

    return RET_OK;
}

 
LOCAL gint32 get_manager_system_com_size(gint32* syscom_size)
{
#ifndef ARM64_HI1711_ENABLED
    *syscom_size = SOL_TX_BUFFER_SIZE;
    return RET_OK;
#endif

    gint32 ret;
    gint32 total_len = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;

    ret = get_ref_sol_uart_obj(&obj_list);
    if (ret != RET_OK || obj_list == NULL) {
        debug_log(DLOG_ERROR, "%s: get_ref_sol_uart_obj failed ,ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        gint32 single_len = 0;
        ret = get_cur_com_log_size(obj_handle, &single_len);
        if (ret != RET_OK) {
            continue;
        }

        total_len += single_len;
    }
    g_slist_free(obj_list);

    if (total_len == 0) {
        return RET_ERR;
    }

    *syscom_size = total_len;
    return RET_OK;
}


LOCAL gint32 get_manager_diagnostic_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    json_object* oem_date_result_jso = NULL;
    gint32 ret;
    gint32 syscom_size = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_manager_system_com_size(&syscom_size);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get system uart data size fail, ret = %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    oem_date_result_jso = json_object_new_object();
    if (oem_date_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add(oem_date_result_jso, PROPERTY_BLACKBOX_SIZE, json_object_new_int(BLACKBOX_DATA_SIZE));
    json_object_object_add(oem_date_result_jso, PROPERTY_SERIALPOARTDATA_SIZE, json_object_new_int(syscom_size));

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_object fail", __FUNCTION__);
        json_object_put(oem_date_result_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object_object_add((*o_result_jso), RFPROP_OEM_HUAWEI, oem_date_result_jso);
    return HTTP_OK;
}

LOCAL gint32 get_manager_diagnostic_workrecord(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *workrecord_jso = NULL;

    ret = get_universal_provider_odata_info(i_paras, o_message_jso, &workrecord_jso, DIAGNOSTIC_WORKRECORD_ODATAID);
    return_val_if_expr(ret != HTTP_OK, ret);

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR, json_object_put(workrecord_jso));

    (void)json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, workrecord_jso);

    return HTTP_OK;
}

LOCAL gint32 act_stop_video_playback(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_object_handle_nth(VIDEO_CLASS_NAME, 0, &obj_handle);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        VIDEO_CLASS_NAME, VIDEO_METHOD_DELECT_LINK, AUTH_DISABLE, i_paras->user_role_privilege, NULL, NULL);

    return_val_do_info_if_expr(VIDEO_UNCONNECTED == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_VIDEOLINK_NOT_EXIST, NULL, o_message_jso));

    return_val_do_info_if_expr(RFERR_NO_RESOURCE == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        debug_log(DLOG_ERROR, "%s: resource not found, ret is %d.", __FUNCTION__, ret));

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Stop video playback failed", __FUNCTION__));

    return HTTP_OK;
}

LOCAL gint32 get_export_video_info(const gchar *video_type, EXPORT_TRANSACTION_INFO_S* export_info)
{
    
    if (strcmp(video_type, PROPERTY_POWEROFF) == 0) {
        export_info->file_id = FILE_ID_POWEROFF;
        strcpy_s(export_info->src, sizeof(export_info->src), VIDEO_PATH_POWEROFF);
        strcpy_s(export_info->file_category, sizeof(export_info->file_category), FILE_CATEGORY_POWEROFF_VIDEO);
    } else if (strcmp(video_type, PROPERTY_OSRESET) == 0) {
        export_info->file_id = FILE_ID_OSRESET;
        strcpy_s(export_info->src, sizeof(export_info->src), VIDEO_PATH_OSRESET);
        strcpy_s(export_info->file_category, sizeof(export_info->file_category), FILE_CATEGORY_OSRESET_VIDEO);
    } else if (strcmp(video_type, PROPERTY_CATERROR) == 0) {
        export_info->file_id = FILE_ID_CATERROR;
        strcpy_s(export_info->src, sizeof(export_info->src), VIDEO_PATH_CATERROR);
        strcpy_s(export_info->file_category, sizeof(export_info->file_category), FILE_CATEGORY_CATERROR_VIDEO);
    } else {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 act_export_video(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    const gchar *video_type = NULL;
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));

    
    json_bool is_ok = get_element_str(i_paras->val_jso, PROPERTY_VIDEO_TYPE, &video_type);
    return_val_do_info_if_expr(!is_ok, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RFACTION_PARAM_TYPE, o_message_jso, RFACTION_EXPORT_VIDEO,
        RFACTION_PARAM_TYPE));

    gint32 ret = get_export_video_info(video_type, &export_info);
    if (ret != RET_OK) {
        create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, o_message_jso, PROPERTY_VIDEO_TYPE, video_type);
        return HTTP_BAD_REQUEST;
    }

    
    if (vos_get_file_accessible(export_info.src) != TRUE) {
        create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    export_info.permission = USERROLE_DIAGNOSEMGNT;
    export_info.need_del = 0;
    export_info.get_status = NULL;
    export_info.generate_file = NULL;

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}


LOCAL gint32 act_export_video_playback_startup_file(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    errno_t safe_fun_ret;
    gint32 ret;
    const gchar *video_type = NULL;
    const gchar *hostname = NULL;
    const gchar *servername = NULL;
    const gchar *language = NULL;
    gint32 port = 0;
    guint8 type = 0;
    guint8 compress = 0;
    gchar *file_path = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *tmp_str = NULL;
    gchar sn[TMP_STR_LEN] = {0};
    gchar format_sn[MAX_LINE_SIZE] = {0};
    gchar key[TMP_STR_LEN] = {0};
    gchar salt[TMP_STR_LEN] = {0};
    gchar jar_name[MAX_FILE_NAME_LEN] = {0};
    gchar buffer[MAX_BUFF_SIZE] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    json_object *buffer_jso = NULL;
    json_object *file_jso = NULL;

    goto_label_do_info_if_fail((i_paras != NULL) && (o_message_jso != NULL) && (o_result_jso != NULL) &&
        (i_paras->val_jso != NULL),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    
    
    if ((i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT) == 0) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE,
            o_message_jso);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
            VIDEO_JNLP);
        return HTTP_FORBIDDEN;
    }

    
    return_val_do_info_if_expr(get_element_str(i_paras->val_jso, PROPERTY_VIDEO_TYPE, &video_type) == FALSE,
        HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, RFACTION_PARAM_TYPE, o_message_jso,
        RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE, RFACTION_PARAM_TYPE);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
        VIDEO_JNLP));

    return_val_do_info_if_expr(get_element_str(i_paras->val_jso, PROPERTY_HOSTNAME, &hostname) == FALSE,
        HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_HOSTNAME, o_message_jso,
        RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE, PROPERTY_HOSTNAME);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
        VIDEO_JNLP));

    return_val_do_info_if_expr(get_element_str(i_paras->val_jso, PROPERTY_SERVERNAME, &servername) == FALSE,
        HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_SERVERNAME, o_message_jso,
        RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE, PROPERTY_SERVERNAME);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
        VIDEO_JNLP));

    return_val_do_info_if_expr(get_element_str(i_paras->val_jso, PROPERTY_LANGUAGE, &language) == FALSE,
        HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, PROPERTY_LANGUAGE, o_message_jso,
        RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE, PROPERTY_LANGUAGE);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
        VIDEO_JNLP));

    
    if (strcmp(video_type, PROPERTY_POWEROFF) == 0) {
        type = 1;
        compress = 1;
        file_path = VIDEO_PATH_POWEROFF;
    } else if (strcmp(video_type, PROPERTY_OSRESET) == 0) {
        type = 3;
        compress = 1;
        file_path = VIDEO_PATH_OSRESET;
    } else if (strcmp(video_type, PROPERTY_CATERROR) == 0) {
        type = 2;
        compress = 1;
        file_path = VIDEO_PATH_CATERROR;
    } else if (strcmp(video_type, PROPERTY_LOCAL) == 0) {
        type = 0;
        compress = 0;
    } else {
        create_message_info(MSGID_ACT_PARA_UNKNOWN, NULL, o_message_jso, PROPERTY_VIDEO_TYPE, video_type);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
            VIDEO_JNLP);
        return HTTP_BAD_REQUEST;
    }

    
    return_val_do_info_if_expr(file_path != NULL && vos_get_file_accessible(file_path) != TRUE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_FILE_NOT_EXIST, NULL, o_message_jso);
        (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name, i_paras->client,
        VIDEO_JNLP));

    
    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    ret = dal_get_property_value_string(obj_handle, PROPERTY_BMC_DEV_SERIAL, sn, sizeof(sn));
    goto_label_do_info_if_expr(ret != VOS_OK, err_exit, debug_log(DLOG_ERROR, "Get serial number failed."));

    
    (void)dal_get_object_handle_nth(VIDEO_CLASS_NAME, 0, &obj_handle);
    ret = dal_get_property_value_int32(obj_handle, VIDEO_PROPERTY_PORT, &port);
    goto_label_do_info_if_expr(ret != VOS_OK, err_exit, debug_log(DLOG_ERROR, "Get video port failed."));

    
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->user_name));
    input_list = g_slist_append(input_list, g_variant_new_string(i_paras->client));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        VIDEO_CLASS_NAME, VIDEO_METHOD_SET_VIDEO_NAME, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    goto_label_do_info_if_expr(ret != VOS_OK, err_exit, debug_log(DLOG_ERROR, "Set video info failed."));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        VIDEO_CLASS_NAME, VIDEO_METHOD_GET_VIDEO_KEY, AUTH_DISABLE, i_paras->user_role_privilege, NULL, &output_list);
    goto_label_do_info_if_expr(ret != VOS_OK, err_exit, debug_log(DLOG_ERROR, "Get video key failed."));
    tmp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    safe_fun_ret = strncpy_s(key, sizeof(key), tmp_str, sizeof(key) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    tmp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    safe_fun_ret = strncpy_s(salt, sizeof(salt), tmp_str, sizeof(salt) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    uip_free_gvariant_list(output_list);

    
    ret = find_fullfilename_from_directory("/opt/pme/web/htdocs/bmc/pages/jar/", "videoplayer", jar_name,
        sizeof(jar_name));
    goto_label_do_info_if_expr(ret != VOS_OK, err_exit, debug_log(DLOG_ERROR, "Get file name failed."));

    dal_format_device_sn(sn, format_sn, sizeof(format_sn));
    iRet = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, VIDEO_JNLP_FORMAT, hostname, servername, format_sn,
        jar_name, key, servername, format_sn, language, type, compress, salt, key, port, servername);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    buffer_jso = json_object_new_string((const gchar *)buffer);
    if (buffer_jso == NULL) {
        debug_log(DLOG_ERROR, "new json object string buffer failed");
        goto err_exit;
    }
    file_jso = json_object_new_string("video.jnlp");
    if (file_jso == NULL) {
        debug_log(DLOG_ERROR, "new json object string jnlp_path failed");
        goto err_exit;
    }
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "new json object string o_result_json failed");
        goto err_exit;
    }
    json_object_object_add(*o_result_jso, RFPROP_JNLP_FILE_NAME, file_jso);
    json_object_object_add(*o_result_jso, RFPROP_JNLP_FILE_CONTENT, buffer_jso);

    (void)record_method_operation_log(i_paras->is_from_webui, TRUE, i_paras->user_name, i_paras->client, VIDEO_JNLP);
    
    i_paras->custom_header = json_object_new_object();
    json_object_object_add(i_paras->custom_header, REQ_HEADER_CONTENT_TYPE,
        json_object_new_string(RF_CONTENT_TYPE_OCTET_STREAM));
    return HTTP_OK;

err_exit:
    if (buffer_jso != NULL) {
        json_object_put(buffer_jso);
    }
    if (file_jso != NULL) {
        json_object_put(file_jso);
    }
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    do_if_expr(i_paras != NULL, (void)record_method_operation_log(i_paras->is_from_webui, FALSE, i_paras->user_name,
        i_paras->client, VIDEO_JNLP));
    

    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 capture_screen_shot_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    int iRet;
    gchar dst_file_path[MAX_FILEPATH_LENGTH] = {0};

    return_val_do_info_if_fail((NULL != monitor_fn_data) && (NULL != message_obj), RF_FAILED,
        debug_log(DLOG_ERROR, "input param error"));
    
    iRet = snprintf_s(dst_file_path, sizeof(dst_file_path), sizeof(dst_file_path) - 1, "%s%s", SCREEN_FILE_WEB_PATH,
        basename(IMG_PATH));
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    
    if (TRUE != vos_get_file_accessible(IMG_PATH)) {
        monitor_fn_data->task_state = RF_TASK_RUNNING;
        return RF_OK;
    }

    (void)vos_file_copy((const gchar *)dst_file_path, IMG_PATH);
    (void)chmod((const gchar *)dst_file_path, S_IRUSR);
    (void)chown((const gchar *)dst_file_path, APACHE_UID, APACHE_GID);

    (void)vos_file_copy(IMG_TMP_WEB_PATH, IMG_PATH);
    (void)chmod(IMG_TMP_WEB_PATH, S_IRUSR);
    (void)chown(IMG_TMP_WEB_PATH, APACHE_UID, APACHE_GID);

    monitor_fn_data->task_state = RF_TASK_COMPLETED;
    monitor_fn_data->task_progress = RF_FINISH_PERCENTAGE;
    (void)create_message_info(MSGID_SUCCESS, NULL, message_obj);
    return RF_OK;
}


LOCAL gint32 act_capture_screen_shot(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    TASK_MONITOR_INFO_S *monitor_info = NULL;

    goto_label_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso) &&
        (NULL != i_paras->val_jso),
        err_exit, debug_log(DLOG_ERROR, "input param error."));

    
    if (0 == (i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT)) {
        debug_log(DLOG_ERROR, "%s failed:insufficient privilege", __FUNCTION__);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_CAPTURE_SCREEN_SHOT, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &obj_handle);
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        KVM_CLASS_NAME, KVM_METHOD_SAVE_SCREEN, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return_val_do_info_if_expr(0xFF == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_DISABLE_SCREENSHOT_IN_CURRENT_STATE, NULL, o_message_jso));

    return_val_do_info_if_expr(RFERR_NO_RESOURCE == ret, HTTP_NOT_FOUND,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, o_message_jso, i_paras->uri);
        debug_log(DLOG_ERROR, "%s: resource not found, ret is %d.", __FUNCTION__, ret));

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Start manul screen shot failed, ret=%d.", __FUNCTION__, ret));

    
    monitor_info = task_monitor_info_new(NULL);
    goto_label_do_info_if_fail((NULL != monitor_info), err_exit,
        debug_log(DLOG_ERROR, "task_monitor_info_new failed "));

    monitor_info->task_progress = TASK_NO_PROGRESS;
    monitor_info->rsc_privilege = USERROLE_DIAGNOSEMGNT;
    ret = create_new_task(NULL, capture_screen_shot_status_monitor, monitor_info, i_paras->val_jso, i_paras->uri,
        o_result_jso);
    goto_label_do_info_if_fail(RF_OK == ret, err_exit, task_monitor_info_free(monitor_info);
        debug_log(DLOG_ERROR, "create new task failed"));

    return HTTP_ACCEPTED;

err_exit:
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}


LOCAL gint32 act_delete_screen_shot(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_DIAGNOSEMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    if (vos_get_file_accessible(IMG_WEB_PATH) != TRUE && vos_get_file_accessible(IMG_PATH) != TRUE &&
        vos_get_file_accessible(IMG_TMP_PATH) != TRUE) {
        return HTTP_OK;
    }

    ret = dal_get_object_handle_nth(KVM_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, RFACTION_DELETE_SCREEN_SHOT);
        debug_log(DLOG_ERROR, "%s: get kvm obj failed, ret %d", __FUNCTION__, ret));

    input_list = g_slist_append(input_list, g_variant_new_byte(1));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        KVM_CLASS_NAME, KVM_METHOD_RM_SCREEN, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Delete screen shot failed, ret=%d.", __FUNCTION__, ret));
    return HTTP_OK;
}

LOCAL gint32 get_blackbox_status(json_object **message_obj, guint8 *progress)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_BLACKBOX_PROCESSBAR, progress);
    if (VOS_OK != ret) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 generate_blackbox(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    gint32 ret;
    gint32 state = 0;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);
    ret = dal_get_property_value_int32(obj_handle, PROPERTY_BLACKBOX_STATE, &state);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Get sol state failed, ret=%d.", __FUNCTION__, ret));

    
    if (state == 0) {
        create_message_info(MSGID_FEATURE_BE_DISABLED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = uip_call_class_method_redfish(action_args->is_from_webui, action_args->user_name, action_args->client,
        obj_handle, CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_GETBLACKBOX, AUTH_DISABLE, 0, NULL, &output_list);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Export blackbox failed, ret=%d.", __FUNCTION__, ret));
    uip_free_gvariant_list(output_list);
    return VOS_OK;
}


LOCAL gint32 act_export_blackbox(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));
    export_info.file_id = FILE_ID_BLACKBOX;
    export_info.permission = USERROLE_DIAGNOSEMGNT;
    export_info.need_del = 1;
    export_info.get_status = get_blackbox_status;
    export_info.generate_file = generate_blackbox;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), BLACKBOX_TAR_FILE_NAME, strlen(BLACKBOX_TAR_FILE_NAME));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_BLACKBOX_DATA,
        strlen(FILE_CATEGORY_BLACKBOX_DATA));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}

LOCAL gint32 get_serial_status(json_object **message_obj, guint8 *progress)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SYSTEMCOM_PROCESSBAR, progress);
    if (VOS_OK != ret) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 generate_serial_data(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    gint32 ret;
    gint32 state = 0;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_DIAGNOSE_NAME, 0, &obj_handle);

    ret = dal_get_property_value_int32(obj_handle, PROPERTY_SOL_DATA_STATE, &state);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Get sol state failed, ret=%d.", __FUNCTION__, ret));

    
    if (state == 0) {
        create_message_info(MSGID_FEATURE_BE_DISABLED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = uip_call_class_method_redfish(action_args->is_from_webui, action_args->user_name, action_args->client,
        obj_handle, CLASS_DIAGNOSE_NAME, METHOD_DIAGNOS_GETSYSTEMCOM, AUTH_DISABLE, 0, NULL, &output_list);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: Export serial failed, ret=%d.", __FUNCTION__, ret));
    uip_free_gvariant_list(output_list);
    return VOS_OK;
}


LOCAL gint32 act_export_serial_port_data(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = { 0 };

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: input param error.", __FUNCTION__, __LINE__));
    export_info.file_id = FILE_ID_SOL;
    export_info.permission = USERROLE_DIAGNOSEMGNT;
    export_info.need_del = 1;
    export_info.get_status = get_serial_status;
    export_info.generate_file = generate_serial_data;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), SYSTEMCOM_TAR_FILE_NAME, strlen(SYSTEMCOM_TAR_FILE_NAME));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_SERIAL_PORT_DATA,
        strlen(FILE_CATEGORY_SERIAL_PORT_DATA));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}


LOCAL gint32 __get_npu_log_status(json_object** message_obj, guint8 *progress)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = INVALID_OBJ_HANDLE;

    // export_transaction获取进度时不会传入参数，因此使用第一个NPU的进度
    (void)dal_get_object_handle_nth(CLASS_NPU, 0, &obj_handle);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_NPU_LOG_PROCESSBAR, progress);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Failed to get progress, ret = %d", ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
        return RET_ERR;
    }

    if (*progress > RF_FINISH_PERCENTAGE) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __generate_npu_log(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso)
{
    gint32 ret;
    json_bool ret_bool;
    json_object *value_jso = NULL;
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 npu_id;
    const gchar* action_name = "DiagnosticService.ExportNPULog";

    ret_bool = json_object_object_get_ex(action_args->param_jso, RFPROP_SYSTEM_PROCESSOR_NPU_ID, &value_jso);
    if (ret_bool != TRUE) {
        debug_log(DLOG_ERROR, "%s, %d: get npu id value failed.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL,  o_message_jso, NULL, RFPROP_SYSTEM_PROCESSOR_NPU_ID);
        goto ERROR_BAD_REQUEST;
    }

    npu_id = json_object_get_int(value_jso);
    if (npu_id > MAX_NPU_ID || npu_id < MIN_NPU_ID) {
        debug_log(DLOG_ERROR, "%s, %d: invalid npu value.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL,  o_message_jso, NULL,
            RFPROP_SYSTEM_PROCESSOR_NPU_ID);
        goto ERROR_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NPU, npu_id - 1, &obj_handle);
    // 获取NPU失败或者是PCIe卡的NPU，返回不支持
    if (ret != VOS_OK || dal_is_pcie_npu() == TRUE) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, action_name);
        debug_log(DLOG_ERROR, "%s: no NPU found", __FUNCTION__);
        goto ERROR_BAD_REQUEST;
    }
    ret = uip_call_class_method_redfish(action_args->is_from_webui, action_args->user_name, action_args->client,
        obj_handle, CLASS_NPU, METHOD_COLLECT_NPU_LOG, AUTH_DISABLE, 0, NULL, &output_list);
    // 调用失败说明已有收集任务在运行
    if (ret == RET_ERR) {
        (void)create_message_info(MSGID_RSC_IN_USE, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: NPU log collection task already existed", __FUNCTION__);
        uip_free_gvariant_list(output_list);
        goto ERROR_BAD_REQUEST;
    } else if (ret != RET_OK) {
        (void)create_message_info(MSGID_GENERAL_ERROR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: call method METHOD_COLLECT_NPU_LOG failed, ret = %d", __FUNCTION__, ret);
        uip_free_gvariant_list(output_list);
        goto ERROR_BAD_REQUEST;
    }
    uip_free_gvariant_list(output_list);
    return RET_OK;

ERROR_BAD_REQUEST:
    return HTTP_BAD_REQUEST;
}


LOCAL gint32 __act_export_npu_log(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    EXPORT_TRANSACTION_INFO_S export_info = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    export_info.file_id = FILE_ID_NPU_LOG;
    export_info.permission = USERROLE_DIAGNOSEMGNT;
    export_info.need_del = 1;
    export_info.get_status = __get_npu_log_status;
    export_info.generate_file = __generate_npu_log;
    (void)strncpy_s(export_info.src, sizeof(export_info.src), NPU_LOG_FILE_NAME, strlen(NPU_LOG_FILE_NAME));
    (void)strncpy_s(export_info.file_category, sizeof(export_info.file_category), FILE_CATEGORY_NPU_LOG,
        strlen(FILE_CATEGORY_NPU_LOG));

    return create_export_transaction(i_paras, o_message_jso, o_result_jso, &export_info);
}


LOCAL PROPERTY_PROVIDER_S g_diagnostic_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_diagnostic_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_diagnostic_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_VIDEO_PLAYBACK_CONN_NUM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_video_playback_conn_num, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_VIDEO_RECORDING_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_video_recording_enabled, set_video_recording_enabled, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_SCREENSHOT_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_screenshot_enabled, set_screenshot_enabled, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_PCIEINTERFACE_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_pcieinterface_state, set_pcieinterface_state, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_BLACKBOXDUMP_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_blackbox_dump_state, set_blackbox_dump_state, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_BLACKBOX_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_blackbox_state, set_blackbox_state, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_SERIALPORTDATA_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_serialportdata_enabled, set_serialportdata_enabled, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_DFP_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID,
        get_dfp_service_enabled, set_dfp_service_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY,  SYS_LOCKDOWN_NULL,
        get_manager_diagnostic_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_VIDEO_RECORD_INFO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_video_create_time, NULL, NULL, ETAG_FLAG_ENABLE},
    {PROPERTY_SCREENSHOT_CREATE_TIME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_screenshot_create_time, NULL, NULL, ETAG_FLAG_ENABLE},
    {DIAGNOSTIC_WORKRECORD, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_diagnostic_workrecord, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ACTION, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_diagnostic_service_actions, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_STOP_VIDEO_PLAYBACK, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_stop_video_playback, ETAG_FLAG_ENABLE},
    {RFACTION_EXPORT_VIDEO, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_video, ETAG_FLAG_ENABLE},
    {RFACTION_EXPORT_VIDEO_PLAYBACK_STARTUP_FILE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_video_playback_startup_file, ETAG_FLAG_ENABLE},
    {RFACTION_CAPTURE_SCREEN_SHOT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_capture_screen_shot, ETAG_FLAG_ENABLE},
    {RFACTION_DELETE_SCREEN_SHOT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL, act_delete_screen_shot, ETAG_FLAG_ENABLE},
    {RFACTION_EXPORT_BLACK_BOX, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_blackbox, ETAG_FLAG_ENABLE},
    {RFACTION_EXPORT_SERIAL_PORT_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_export_serial_port_data, ETAG_FLAG_ENABLE},
    {RFACTION_EXPORT_NPU_LOG, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_DIAGNOSEMGNT, SYS_LOCKDOWN_ALLOW, NULL,
        NULL, __act_export_npu_log, ETAG_FLAG_ENABLE}
};


gint32 managers_provider_diagnosticservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    guchar board_type = 0;
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_expr(VOS_OK != ret || DISABLE == board_type, HTTP_NOT_FOUND);

    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(TRUE != ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s,%d:URI check failed. ", __FUNCTION__, __LINE__));

    *prop_provider = g_diagnostic_provider;
    *count = sizeof(g_diagnostic_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}
