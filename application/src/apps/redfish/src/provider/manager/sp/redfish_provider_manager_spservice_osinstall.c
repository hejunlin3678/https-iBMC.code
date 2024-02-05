
#include "redfish_provider.h"

#define PROP_ENCRYPTION "Encryption"

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
LOCAL gint32 create_unencrypted_sp_osinstall(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id,
    guint32 id_len, json_object *o_message_array_jso);
#endif
LOCAL gint32 create_encrypted_sp_osinstall(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id,
    guint32 id_len, json_object *o_message_array_jso);

LOCAL PROPERTY_PROVIDER_S  g_manager_sp_osinstall_provider[] = {
};


gint32 rf_inner_session_id_check(const gchar *session_id)
{
    gint32 ret;
    OBJ_HANDLE session_handle = 0;
    GSList *input_list = NULL;

    return_val_do_info_if_expr(session_id == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    
    return_val_if_expr(strlen(session_id) == 0, VOS_ERR);

    ret = dal_get_object_handle_nth(CLASS_SESSION, 0, &session_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get object session_handle failed", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_string(session_id));

    // 调用方法判断session_id是不是框内redfish会话
    ret = dfl_call_class_method(session_handle, METHOD_SESSION_ISREDFISHINNERSESSION, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s:call %s fail", __FUNCTION__, METHOD_SESSION_ISREDFISHINNERSESSION));

    return VOS_OK;
}


LOCAL gint32 sp_osinstall_info_exist_check(void)
{
    return_val_if_expr(vos_get_file_accessible(SP_OSINSTALL_PARA_PATH) != TRUE, SP_DATA_MISSING);

    return SP_DATA_EXIST;
}


gint32 sp_info_cache(const char *sp_para_name, const char *sp_para_path, json_object *body_jso_checked)
{
    gint32 ret;
    const gchar *body_str = NULL;
    GVariant *tmp_data = NULL;
    FILE *fp = NULL;
    const gchar **encrypted_data = NULL;
    gsize encrypted_data_len = 0;
    gsize i;
    gint32 fd;

    // 入参检查 && 克隆部署配置信息状态检查
    return_val_do_info_if_expr(NULL == body_jso_checked, SP_DATA_UNKNOWN,
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));

    // json转字符串
    body_str = dal_json_object_get_str(body_jso_checked); // body_str在释放body_jso_checked后释放
    return_val_do_info_if_expr(NULL == body_str, SP_DATA_UNKNOWN,
        debug_log(DLOG_ERROR, "%s: get %s data string failed.", __FUNCTION__, sp_para_name));

    // 数据长度有效性判断
    return_val_do_info_if_expr(SP_CACHEDATA_MAX_LENGTH < strlen(body_str), SP_DATA_TOO_LONG,
        debug_log(DLOG_ERROR, "%s: the length of body_str is more than (%d).", __FUNCTION__, SP_CACHEDATA_MAX_LENGTH));

    // 加密配置信息
    ret = rf_get_property_encrypted_data(body_str, &tmp_data);
    
    return_val_do_info_if_expr(VOS_OK != ret, SP_DATA_ENCRYPT_FAILED,
        debug_log(DLOG_DEBUG, "%s: encrypted error, ret (%d).", __FUNCTION__, ret));
    

    // 文件不存在时先调用代理接口创建文件
    if (vos_get_file_accessible(sp_para_path) == FALSE) {
        ret = proxy_create_file(sp_para_path, "w+", REDFISH_USER_UID, ROOT_USER_GID, S_IRUSR | S_IWUSR);
        return_val_do_info_if_expr(VOS_OK != ret, SP_DATA_UNKNOWN, g_variant_unref(tmp_data); tmp_data = NULL;
            debug_log(DLOG_ERROR, "%s: create sp %s file error", __FUNCTION__, sp_para_name));
    }

    fp = g_fopen(sp_para_path, "w+");
    return_val_do_info_if_expr(NULL == fp, SP_DATA_UNKNOWN, g_variant_unref(tmp_data); tmp_data = NULL;
        debug_log(DLOG_ERROR, "%s: open sp %s file error", __FUNCTION__, sp_para_name));

    fd = fileno(fp);
    (void)flock(fd, LOCK_EX);

    encrypted_data = g_variant_get_strv(tmp_data, &encrypted_data_len);
    return_val_do_info_if_expr(NULL == encrypted_data, SP_DATA_UNKNOWN, g_variant_unref(tmp_data); tmp_data = NULL;
        debug_log(DLOG_ERROR, "%s: open sp %s get encrypted_data error", __FUNCTION__, sp_para_name);
        (void)flock(fd, LOCK_UN); (void)fclose(fp); fp = NULL);

    for (i = 0; i < encrypted_data_len; i++) {
        ret = fwrite(encrypted_data[i], strlen(encrypted_data[i]), 1, fp);
        break_do_info_if_expr(ret != 1, ret = SP_DATA_UNKNOWN;
            debug_log(DLOG_ERROR, "write sp %s file failed\n", sp_para_name));

        ret = fwrite("\n", 1, 1, fp);
        break_do_info_if_expr(ret != 1, ret = SP_DATA_UNKNOWN;
            debug_log(DLOG_ERROR, "write sp %s file failed\n", sp_para_name));
    }
    (void)flock(fd, LOCK_UN);
    (void)fclose(fp);
    fp = NULL;
    g_variant_unref(tmp_data);
    tmp_data = NULL;
    g_free(encrypted_data);
    encrypted_data = NULL;

    do_if_expr(ret != 1 && FALSE == vos_rm_filepath(sp_para_path),
        debug_log(DLOG_ERROR, "%s sp %s remove failed\n", __FUNCTION__, sp_para_name));
    return (ret == 1) ? VOS_OK : VOS_ERR;
}


LOCAL gint32 sp_osinstall_info_save(json_object *body_jso_checked)
{
    return sp_info_cache(SP_OSINSTALL_PARA_NAME, SP_OSINSTALL_PARA_PATH, body_jso_checked);
}


LOCAL gint32 sp_info_exist_check(const char *path)
{
    return_val_if_expr(access(path, F_OK) != 0, SP_DATA_MISSING);

    return SP_DATA_EXIST;
}


LOCAL gint32 sp_info_get_poperty_data(const char *sp_para_path, GVariant **o_property_data)
{
    FILE *fp = NULL;
    gint32 fd;
    gchar tmp_buf[MAX_ENCRYPTO_DATA_LEN + 1] = {0};
    gchar *strv[SP_CACHEDATA_MAX_LENGTH / (ENCRYPTED_DATA_MAX_LEN + 1)];
    int iRet;
    guint count = 0;
    guint i;
    gchar *resolved_path = realpath(sp_para_path, NULL);

    if (resolved_path == NULL) {
        return SP_DATA_UNKNOWN;
    }

    fp = g_fopen(resolved_path, "r");
    g_free(resolved_path);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: open sp file error", __FUNCTION__);
        return SP_DATA_UNKNOWN;
    }
    fd = fileno(fp);
    (void)flock(fd, LOCK_EX);

    *o_property_data = NULL;
    // 从文件读取克隆配置
    while (vos_fgets_s(tmp_buf, MAX_ENCRYPTO_DATA_LEN, fp) != NULL &&
        count < (SP_CACHEDATA_MAX_LENGTH / (ENCRYPTED_DATA_MAX_LEN + 1))) {
        strv[count] = NULL;
        strv[count] = (gchar *)g_malloc0(MAX_ENCRYPTO_DATA_LEN + 1);

        if (strv[count] == NULL) {
            goto exception;
        }

        iRet = snprintf_s(strv[count], MAX_ENCRYPTO_DATA_LEN + 1, MAX_ENCRYPTO_DATA_LEN, "%s", tmp_buf);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }

        count++;
    }

    // 使用还原的配置创建GVariant
    *o_property_data = g_variant_new_strv((const gchar * const *)strv, count);

exception:
    (void)flock(fd, LOCK_UN);
    fd = 0;
    (void)fclose(fp);
    fp = NULL;

    for (i = 0; i < count; i++) {
        clear_sensitive_info(strv[i], MAX_ENCRYPTO_DATA_LEN + 1);
        g_free(strv[i]);
    }

    if (*o_property_data == NULL) {
        debug_log(DLOG_ERROR, "%s: property_data error", __FUNCTION__);
        return SP_DATA_UNKNOWN;
    }

    return VOS_OK;
}


LOCAL gint32 sp_info_get(const char *sp_para_name, const char *sp_para_path, json_object *o_result_jso)
{
    GVariant *property_data = NULL;
    gint32 ret;
    gchar *data_str = NULL;
    json_object *data_jso = NULL;

    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        return SP_DATA_UNKNOWN;
    }

    if (sp_info_exist_check(sp_para_path) == SP_DATA_MISSING) {
        debug_log(DLOG_INFO, "%s sp %s has not been saved.", __FUNCTION__, sp_para_name);
        return SP_DATA_MISSING;
    }

    ret = sp_info_get_poperty_data(sp_para_path, &property_data);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: property_data error", __FUNCTION__);
        return ret;
    }

    // 内部会话解密返回
    ret = rf_get_property_plain_data(property_data, &data_str);
    if (ret == VOS_OK && data_str != NULL) {
        data_jso = json_tokener_parse(data_str);
        if (data_jso == NULL) {
            clear_sensitive_info(data_str, strlen(data_str));
            g_free(data_str);
            g_variant_unref(property_data);
            debug_log(DLOG_ERROR, "%s:  data is null", __FUNCTION__);
            return SP_DATA_UNKNOWN;
        }

        // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
        json_object_object_foreach0(data_jso, name, val)
        {
            json_object_object_add(o_result_jso, name, json_object_get(val));
        }
#pragma GCC diagnostic pop         // require GCC 4.6
        json_object_put(data_jso); // 封装后立即释放jso资源
    }

    if (data_str != NULL) {
        clear_sensitive_info(data_str, strlen(data_str));
        g_free(data_str);
    }
    g_variant_unref(property_data);

    return VOS_OK;
}


LOCAL gint32 sp_osinstall_info_get(json_object *o_result_jso, gboolean return_om_flag)
{
    gint32 ret;
    const gchar* sensitive_array[] = {SP_PROP_NAME_CDKEY, SP_PROP_NAME_ROOT_PWD};
    guint32 list_len = G_N_ELEMENTS(sensitive_array);
    gboolean is_sensitive_prop;
    json_object *sposinstallpara_jso = NULL;
    guint32 i;

    // 获取加密信息
    sposinstallpara_jso = json_object_new_object();
    ret = sp_info_get(SP_OSINSTALL_PARA_NAME, SP_OSINSTALL_PARA_PATH, sposinstallpara_jso);
    return_val_do_info_if_fail(VOS_OK == ret, ret, (void)json_object_put(sposinstallpara_jso);
        debug_log(DLOG_ERROR, "%s: get encrypt file failed.\n", __FUNCTION__));

    // 此处用于消除foreach编译告警"assignment discards 'const' qualifier from pointer target type"
#pragma GCC diagnostic push // require GCC 4.6
#pragma GCC diagnostic ignored "-Wcast-qual"
    json_object_object_foreach0(sposinstallpara_jso, name, val)
    {
        is_sensitive_prop = FALSE;

        for (i = 0; i < list_len; i++) {
            break_do_info_if_expr(0 == g_strcmp0(sensitive_array[i], name), (is_sensitive_prop = TRUE));
        }

        if (TRUE == is_sensitive_prop && TRUE == return_om_flag) {
            // 如果字段是敏感字段且是返回给OM调用的需要隐藏该字段
            json_object_clear_string(sposinstallpara_jso, name);
            json_object_object_add(o_result_jso, name, NULL);
        } else {
            json_object_object_add(o_result_jso, name, json_object_get(val));
        }
    }
#pragma GCC diagnostic pop // require GCC 4.6

    (void)json_object_put(sposinstallpara_jso);

    return VOS_OK;
}


LOCAL gint32 sp_info_del(const char *path)
{
    gint32 ret;
    ret = proxy_delete_file(0, NULL, NULL, path);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "sp para remove failed! ret = %d.", ret);
        return VOS_ERR;
    }
    return VOS_OK;
}


LOCAL gint32 sp_osinstall_info_del(void)
{
    return sp_info_del(SP_OSINSTALL_PARA_PATH);
}


LOCAL gint32 handle_sp_osinstall_data(guint32 handle_op, json_object *body_jso_checked, json_object *o_result_jso)
{
    gint32 ret;
    // 对入参无约束

    switch (handle_op) {
        case SP_HANDLE_POST:
            ret = sp_osinstall_info_save(body_jso_checked);
            break;

        case SP_HANDLE_OM_GET:
            ret = sp_osinstall_info_get(o_result_jso, TRUE); // OM查询，隐藏敏感信息
            break;

        case SP_HANDLE_SP_GET:
            ret = sp_osinstall_info_get(o_result_jso, FALSE);
            break;

        case SP_HANDLE_DEL:
            ret = sp_osinstall_info_del();
            break;

        case SP_HANDLE_EXIST_CHECK:
            ret = sp_osinstall_info_exist_check();
            break;

        default:
            debug_log(DLOG_INFO, "%s:handle_op unknown.", __FUNCTION__);
            ret = SP_DATA_UNKNOWN;
    }

    return ret;
}

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1


LOCAL gint32 sp_osinstall_encrypt_suppport_check(PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    json_bool ret_bool;

    json_object *o_rsc_jso = NULL;
    json_object *sposinstallpara_jso = NULL;
    json_object *encrypt_item = NULL;

    // 获取support.json
    o_rsc_jso = json_object_new_object();
    ret = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_CFG, RFPROP_MANAGER_SP_SUPPORT_NAME, o_rsc_jso);
    return_val_do_info_if_fail((VOS_OK == ret), ret, (void)json_object_put(o_rsc_jso);
        debug_log(DLOG_ERROR, "%s: get support file failed.\n", __FUNCTION__));

    // 获取support.json中SPOSInstallPara模块的信息
    ret_bool = json_object_object_get_ex(o_rsc_jso, RFPROP_SPSERVICE_OS, &sposinstallpara_jso);
    return_val_do_info_if_expr(TRUE != ret_bool || NULL == sposinstallpara_jso, RF_RSC_NOT_FOUND,
        (void)json_object_put(o_rsc_jso);
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex SPOSInstallPara failed.\n", __FUNCTION__));

    // 获取加密状态
    ret_bool = json_object_object_get_ex(sposinstallpara_jso, PROP_ENCRYPTION, &encrypt_item);
    return_val_do_info_if_expr(TRUE != ret_bool || NULL == encrypt_item, RF_RSC_NOT_FOUND,
        (void)json_object_put(o_rsc_jso);
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex Encryption failed.\n", __FUNCTION__));

    return_val_do_info_if_fail(json_object_get_boolean(encrypt_item), RF_FAILED, (void)json_object_put(o_rsc_jso);
        debug_log(DLOG_ERROR, "%s: SP does not support encryption.\n", __FUNCTION__));

    (void)json_object_put(o_rsc_jso);

    return ret;
}
#endif

LOCAL gint32 sp_osinstall_ums_del(PROVIDER_PARAS_S *i_param)
{
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    (void)dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);

    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TRANS_FILE_ID_SP_OSINSTALL));
    input_list = g_slist_append(input_list, g_variant_new_string(RFPROP_MANAGER_SP_OS_NAME));

    ret_val = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_DEL_FILE, AUTH_DISABLE, i_param->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);
    return ret_val;
}

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1

LOCAL gint32 get_unencrypted_sp_osinstall_status(PROVIDER_PARAS_S *i_paras)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *file_data = NULL;
    gchar **split_array = NULL;
    guint length;
    guint index_id;
    gchar filePathName[MAX_FILEPATH_LENGTH] = {0};

    // 生成完整路径
    (void)snprintf_truncated_s(filePathName, sizeof(filePathName), "%s.json", RFPROP_MANAGER_SP_OS_NAME);

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, SP_DATA_MISSING,
        debug_log(DLOG_ERROR, "%s:  get object handle fail", __FUNCTION__));

    input_list = g_slist_append(input_list, g_variant_new_byte(UMS_TRANS_FILE_ID_SP_OSINSTALL));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        SP_UMS_CLASS_NAME, UMS_METHOD_GET_FILE_LIST, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    uip_free_gvariant_list(input_list);
    return_val_do_info_if_expr(ret != VOS_OK, SP_DATA_MISSING,
        debug_log(DLOG_DEBUG, "%s:  get file list fail, ret is %d.", __FUNCTION__, ret));

    file_data = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    return_val_do_info_if_expr(file_data == NULL || strlen(file_data) == 0, SP_DATA_MISSING,
        uip_free_gvariant_list(output_list);
        debug_log(DLOG_DEBUG, "%s: file_data is null", __FUNCTION__));

    split_array = g_strsplit(file_data, SP_FILE_SPLITE_FLAG, 0);
    if (split_array == NULL) {
        uip_free_gvariant_list(output_list);
        return SP_DATA_MISSING;
    }

    length = g_strv_length(split_array);
    for (index_id = 0; index_id < length; index_id++) {
        if (split_array[index_id] == NULL) {
            break;
        }

        if (strlen(split_array[index_id]) <= strlen(".json")) {
            continue;
        }

        if (g_strcmp0(filePathName, split_array[index_id]) == 0) {
            g_strfreev(split_array);
            uip_free_gvariant_list(output_list);
            return SP_DATA_EXIST;
        }
    }

    g_strfreev(split_array);
    uip_free_gvariant_list(output_list);

    return SP_DATA_MISSING;
}
#endif


gint32 get_sp_osinstall_status(PROVIDER_PARAS_S *i_paras)
{
    // 先检查加密任务是否存在, 如果不存在在去判断ums中的任务文件是否存在
    if (handle_sp_osinstall_data(SP_HANDLE_EXIST_CHECK, NULL, NULL) == SP_DATA_EXIST) {
        return SP_DATA_EXIST;
    }

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    return get_unencrypted_sp_osinstall_status(i_paras);
#endif

    return SP_DATA_MISSING;
}


gint32 manager_sp_osinstall_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guint8 spservice_os_enable = TRUE;

    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SP_SERVICE_OS_ENABLE,
        &spservice_os_enable);
    if (VOS_OK != ret) {
        debug_log(DLOG_DEBUG, "%s, %d: Get SP service for OS enable state failed.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_expr(FALSE == rf_support_sp_service() || FALSE == spservice_os_enable, HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: not support", __FUNCTION__, __LINE__));

    return_val_do_info_if_expr(0 != g_strcmp0(i_paras->member_id, "1"), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: url error", __FUNCTION__, __LINE__));

    // 数据存在性校验，不存在数据则返回404
    return_val_do_info_if_expr(SP_DATA_MISSING == get_sp_osinstall_status(i_paras), HTTP_NOT_FOUND,
        debug_log(DLOG_MASS, "%s, %d: the configuration information does not exist", __FUNCTION__, __LINE__));

    if (redfish_check_manager_uri_valid(i_paras->uri)) {
        *prop_provider = g_manager_sp_osinstall_provider;
        
        *count = 0;
        
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}


gint32 get_sp_osinstall_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    gint32 ret;
    guint8 ums_server_flag = 0;
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    json_object *message_info_jso = NULL;
    const gchar *msg_id = NULL;
#endif

    if (o_rsc_jso == NULL || o_err_array_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 更新固定属性@odata.context 和@odata.id
    (void)get_sp_memberid_odata_context_id(o_rsc_jso, MANAGERS_SPSERVICE_OSINSTALL_METADATA,
        MANAGER_SPSERVICE_OSINSTALL_URI, i_paras->member_id);

    // 内部会话并且SP是在线的才返回包含敏感信息的信息，返回后删除配置文件，否则返回隐藏敏感信息后的信息
    if (rf_inner_session_id_check(i_paras->session_id) == VOS_OK && get_ums_server_flag(&ums_server_flag) == VOS_OK &&
        ums_server_flag == UMS_X86_MONOPOLY) {
        ret = handle_sp_osinstall_data(SP_HANDLE_SP_GET, NULL, o_rsc_jso);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_INFO, "%s:(SP)Failed to get sp osinstall data.", __FUNCTION__));

        // SP查过了，要执行信息删除
        (void)handle_sp_osinstall_data(SP_HANDLE_DEL, NULL, NULL);
    } else {
        ret = handle_sp_osinstall_data(SP_HANDLE_OM_GET, NULL, o_rsc_jso);
        do_if_expr(VOS_OK != ret, debug_log(DLOG_INFO, "%s:(OM)Failed to get sp osinstall data.", __FUNCTION__));
    }

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    // 如果没有获取到加密任务则去获取UMS上的任务
    if (VOS_OK != ret) {
        
        ret = get_sp_file_data(i_paras, UMS_TRANS_FILE_ID_SP_OSINSTALL, RFPROP_MANAGER_SP_OS_NAME, o_rsc_jso);
        
        
        if (UMS_WR_ERR_BUSY == ret) {
            msg_id = MSGID_UMS_IS_EXCLUSIVELY_USED;
        } else if (UMS_WR_ERR_IN_UPGRADING == ret) {
            msg_id = MSGID_SP_IS_BEING_UPGRADED;
        }
        if (msg_id != NULL) {
            (void)create_message_info(msg_id, NULL, &message_info_jso);
            (void)json_object_array_add(o_err_array_jso, message_info_jso);
            return RF_RSC_IN_USE;
        }
        

        do_val_if_expr(VOS_OK != ret, debug_log(DLOG_DEBUG, "%s, %d: no file.\n", __FUNCTION__, __LINE__));
    }
#endif

    return RF_OK;
}

LOCAL gint32 check_spservice_osinstall_input(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id,
    json_object *o_message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;
    guint8 spservice_os_enable = TRUE;
    
    if (o_id == NULL || o_message_array_jso == NULL || i_param == NULL || body_jso_checked == NULL) {
        debug_log(DLOG_ERROR, "%s input param error", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
        json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto ERR_EXIT;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SP_SERVICE_OS_ENABLE,
        &spservice_os_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get SP service for OS enable state failed", __FUNCTION__);
        ret = HTTP_INTERNAL_SERVER_ERROR;
        goto ERR_EXIT;
    }

    
    if (rf_support_sp_service() == FALSE || spservice_os_enable == FALSE) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_NOT_FOUND;
        goto ERR_EXIT;
    }

    
    ret = redfish_check_manager_uri_valid(i_param->uri);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: user input uri is bad", __FUNCTION__);
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_info_jso, i_param->uri);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_NOT_FOUND;
        goto ERR_EXIT;
    }

    
    // 判断用户是否具有基础设置权限,没有直接返回禁止
    if ((i_param->user_role_privilege & USERROLE_BASICSETTING) == 0) {
        debug_log(DLOG_ERROR, "%s: user has no privilege, user is %s", __FUNCTION__, i_param->user_name);
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_FORBIDDEN;
        goto ERR_EXIT;
    }

    return RET_OK;

ERR_EXIT:
    json_object_clear_string(body_jso_checked, SP_PROP_NAME_CDKEY);
    json_object_clear_string(body_jso_checked, SP_PROP_NAME_ROOT_PWD);
    return ret;
}


gint32 create_sp_osinstall_id(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso)
{
    gint32 ret;
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    json_object *message_info_jso = NULL;
    const gchar *msg_id = NULL;
#endif
    GSList *caller_info = NULL;
    gchar *logs = NULL;
    gchar succ_log[LOG_MAX_LENGTH] = "OS installation parameters configured successfully";
    gchar fail_log[LOG_MAX_LENGTH] = "Failed to configure OS installation parameters";

    ret = check_spservice_osinstall_input(i_param, body_jso_checked, o_id, o_message_array_jso);
    if (ret != RET_OK) {
        return ret;
    }

    caller_info =
        g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_param->is_from_webui)));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->user_name));
    caller_info = g_slist_append(caller_info, g_variant_new_string(i_param->client));

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    ret = sp_osinstall_encrypt_suppport_check(i_param);
    do_if_expr(UMS_WR_ERR_BUSY == ret, (msg_id = MSGID_UMS_IS_EXCLUSIVELY_USED));
    do_if_expr(UMS_WR_ERR_IN_UPGRADING == ret, (msg_id = MSGID_SP_IS_BEING_UPGRADED));
    if (msg_id != NULL) {
        (void)create_message_info(msg_id, NULL, &message_info_jso);
        (void)json_object_array_add(o_message_array_jso, message_info_jso);
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s", fail_log);
        do_info_if_true(caller_info != NULL, g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref));
        json_object_clear_string(body_jso_checked, SP_PROP_NAME_CDKEY);
        json_object_clear_string(body_jso_checked, SP_PROP_NAME_ROOT_PWD);
        return HTTP_BAD_REQUEST;
    }
    if (ret != VOS_OK) {
        // 不加密的方式是通过ums创建，在ums中记录操作日志
        ret = create_unencrypted_sp_osinstall(i_param, body_jso_checked, o_id, id_len, o_message_array_jso);
    } else {
#endif
        ret = create_encrypted_sp_osinstall(i_param, body_jso_checked, o_id, id_len, o_message_array_jso);
        if (ret == RET_OK) {
            write_sp_task_desc(i_param, HAVE_SP_TASK); // 写任务描述文件
            logs = succ_log;
        } else {
            logs = fail_log;
        }
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "%s", logs);
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    }
#endif
    do_info_if_true(caller_info != NULL, g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref));
    json_object_clear_string(body_jso_checked, SP_PROP_NAME_CDKEY);
    json_object_clear_string(body_jso_checked, SP_PROP_NAME_ROOT_PWD);
    return ret;
}

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1

LOCAL gint32 create_unencrypted_sp_osinstall(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id,
    guint32 id_len, json_object *o_message_array_jso)
{
    gint32 ret;
    json_object *message_info_jso = NULL;
    // 参数有效性由上层函数保证，不重复校验参数

    ret = create_sp_item_template(i_param, body_jso_checked, o_id, id_len, RFPROP_MANAGER_SP_OS_NAME,
        UMS_TRANS_FILE_ID_SP_OSINSTALL);
    if (VOS_OK != ret) {
        switch (ret) {
            case UMS_WR_ERR_IN_UPGRADING:
                (void)create_message_info(MSGID_SP_IS_BEING_UPGRADED, NULL, &message_info_jso);
                break;

            case UMS_TRANS_ERR_NO_SPACE:
            case UMS_TRANS_ERR_FILE_TOO_LARGE:
                (void)create_message_info(MSGID_SP_CFG_OVERSIZED, NULL, &message_info_jso);
                break;

            case UMS_WR_ERR_BUSY:
                (void)create_message_info(MSGID_UMS_IS_EXCLUSIVELY_USED, NULL, &message_info_jso);
                break;

            default:
                debug_log(DLOG_ERROR, "%s, %d: failed with ret %d", __FUNCTION__, __LINE__, ret);
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
                json_object_array_add(o_message_array_jso, message_info_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }

        debug_log(DLOG_ERROR, "%s %d: create_sp_item_template fail, ret is 0x%x", __FUNCTION__, __LINE__, ret);
        json_object_array_add(o_message_array_jso, message_info_jso);
        ret = HTTP_BAD_REQUEST;
    }
    // 删除加密的任务文件
    (void)handle_sp_osinstall_data(SP_HANDLE_DEL, NULL, NULL);
    debug_log(DLOG_INFO, "%s: Create sp osinstall id to ums.", __FUNCTION__);
    return ret;
}
#endif

LOCAL gint32 create_encrypted_sp_osinstall(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id,
    guint32 id_len, json_object *o_message_array_jso)
{
    gchar buff[16] = {0};
    gint32 ret;
    json_object *message_info_jso = NULL;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};
    // 参数有效性由上层函数保证，不重复校验参数

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    // 持久化保存加密任务文件
    ret = handle_sp_osinstall_data(SP_HANDLE_POST, body_jso_checked, NULL);
    do_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s: Failed to save sp osinstall data, ret(%d).", __FUNCTION__, ret));

    switch (ret) {
        case VOS_OK:
            break;

        case SP_DATA_TOO_LONG:
            (void)snprintf_s(buff, sizeof(buff), sizeof(buff) - 1, "%d", SP_CACHEDATA_MAX_LENGTH);
            (void)create_message_info(MSGID_BODY_EXC_MAXLEN, NULL, &message_info_jso, (const char *)buff);
            json_object_array_add(o_message_array_jso, message_info_jso);
            ret = HTTP_BAD_REQUEST;
            break;

        case SP_DATA_EXIST:
            (void)create_message_info(MSGID_RSC_ALREADY_EXIST, NULL, &message_info_jso);
            json_object_array_add(o_message_array_jso, message_info_jso);
            ret = VOS_OK;
            break;

        case SP_DATA_ENCRYPT_FAILED:
        case SP_DATA_UNKNOWN:
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_info_jso);
            json_object_array_add(o_message_array_jso, message_info_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    return_val_do_info_if_expr(id_len == 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed:input param error, id_len is 0.", __FUNCTION__));
    // 填充memberid，供创建成功结果查询
    (void)snprintf_s(o_id, id_len, id_len - 1, "%s", "1");

    // 删除ums上的任务文件
    (void)sp_osinstall_ums_del(i_param);
    debug_log(DLOG_INFO, "%s: Create sp osinstall id to %s.", __FUNCTION__, custom_name);
    return ret;
}
