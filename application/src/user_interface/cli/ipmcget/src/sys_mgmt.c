

#include "ipmcget_sys_mgmt.h"


gint32 ipmc_get_port80_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 port80_info[256] = {0};
    GSList *output_list = NULL;
    
    gint32 offset;
    
    gint32 info_len;
    gint32 count;
    guchar arm_enable = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (arm_enable == ENABLE) {
        g_printf("The server type does not support this command.\r\n");
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d port80\r\n");
        return FALSE;
    }

    ret = uip_set_object_property(CLASS_DIAGNOSE_NAME, OBJ_DIAGNOSE_NAME, METHOD_DIAGNOS_GETPORT80, NULL, &output_list);
    if (ret != RET_OK) {
        g_printf("Get port80 status information failed.\r\n");
        return FALSE;
    }

    gvar_array_to_byte((GVariant *)g_slist_nth_data(output_list, 0), port80_info, sizeof(port80_info));
    offset = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 1));
    info_len = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 2));

    g_printf("port80 diagnose code:\r\n");

    
    info_len = MIN(info_len, (gint32)sizeof(port80_info));
    
    for (count = 0; count < info_len;) {
        // 当前偏移位置的80口信息输出格式是[xx]
        if (count == offset) {
            g_printf("[%02X]", port80_info[count]);
        } else {
            g_printf("%02X", port80_info[count]);
        }

        count++;

        if (!(count % 16)) {
            g_printf("\r\n");
        } else if (!(count % 8)) {
            g_printf("--");
        } else {
            g_printf("-");
        }
    }

    uip_free_gvariant_list(output_list);
    return TRUE;
}


gint32 ipmc_get_diagnose_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 process_bar = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    uid_t uid = 0;
    ret = ipmc_get_uid(&uid);
    if (ret != RET_OK) {
        g_printf("Get user info failed.\r\n");
        return FALSE;
    }
    guint8 is_domain_user = (uid >= LDAP_USER_ID_BASE) ? TRUE : FALSE;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(is_domain_user));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_DUMP, OBJECT_DUMP,
        METHOD_DUMP_DUMPINFO_ASYNC, input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    if (ret != RET_OK) {
        if (ret == MD_ENOSPACE) {
            g_printf("Failed to collect data due to insufficient memory.\r\n");
        } else if (ret == MD_EONUPGRADE) {
            g_printf("An upgrade is in progress. Please wait.\r\n");
        } else if (ret == MD_EBUSY) {
            g_printf("Task in progress... Please wait.\r\n");
        } else {
            g_printf("Get diagnose info failed.\r\n");
        }
        return FALSE;
    }

    

    g_printf("Downloading...\r\n");
    for (;;) {
        ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_DUMP,
            OBJECT_DUMP, METHOD_DUMP_GET_DUMP_PHASE, NULL, &output_list);
        if (ret != RET_OK) {
            g_printf("Get diagnose process info failed.\r\n");
            return FALSE;
        }

        process_bar = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        uip_free_gvariant_list(output_list);
        output_list = NULL;

        if (process_bar < 0) {
            g_printf("\r%d%% Download diagnose info failed.\r\n", process_bar);
            (void)fflush(stdout);
            return FALSE;
        } else if (process_bar < 100) {
            g_printf("\r%2d%%", process_bar);
            (void)fflush(stdout);
        } else {
            g_printf("\r100%%\r\n");
            (void)fflush(stdout);
            break;
        }

        (void)vos_task_delay(DELAY_TIME);
    }

    g_printf("Download diagnose info to /tmp/ successfully.\r\n");
    return TRUE;
}


gint32 ipmc_get_systemcom_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    const gchar *filename = NULL;
    GVariant *value = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    gint32 state;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d systemcom\r\n");
        return FALSE;
    }

    
    ret = uip_get_object_property(CLASS_DIAGNOSE_NAME, OBJ_DIAGNOSE_NAME, PROPERTY_SOL_DATA_STATE, &value);
    if (ret != RET_OK) {
        g_printf("Get System Com state failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_int32(value);
    g_variant_unref(value);
    if (state == 0) {
        g_printf("System Com function is disabled.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_DIAGNOSE_NAME,
        OBJ_DIAGNOSE_NAME, METHOD_DIAGNOS_GETSYSTEMCOM, NULL, &output_list);
    if (ret != RET_OK) {
        goto EXIT;
    }
    g_printf("Downloading...\r\n");
    ret = get_file_transfer_state(CLASS_DIAGNOSE_NAME, OBJ_DIAGNOSE_NAME, PROPERTY_SYSTEMCOM_PROCESSBAR);
    if (ret != RET_OK) {
        goto EXIT;
    }
    filename = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), 0);
    ret = ipmc_change_file_owner(filename);
    if (ret != RET_OK) {
        goto EXIT;
    }
    g_printf("Download System Com data to %s successfully.\r\n", filename);
    uip_free_gvariant_list(output_list);
    return TRUE;

EXIT:
    uip_free_gvariant_list(output_list);
    g_printf("Download System Com data failed.\r\n");
    return FALSE;
}


gint32 ipmc_get_blackbox_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    const gchar *filename = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    GVariant *value = NULL;
    guint8 state;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d blackbox\r\n");
        return FALSE;
    }

    
    ret = uip_get_object_property(CLASS_DIAGNOSE_NAME, OBJ_DIAGNOSE_NAME, PROPERTY_BLACKBOX_STATE, &value);
    if (ret != RET_OK) {
        g_printf("Get Black Box state failed.\r\n");
        return FALSE;
    }

    state = g_variant_get_byte(value);
    g_variant_unref(value);
    value = NULL;
    if (state == 0) {
        g_printf("Black Box function is disabled.\r\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_DIAGNOSE_NAME,
        OBJ_DIAGNOSE_NAME, METHOD_DIAGNOS_GETBLACKBOX, NULL, &output_list);
    if (ret != RET_OK) {
        if (ret == 0xff) {
            g_printf("Other user downloading, please wait.\r\n");
        } else {
            g_printf("Download Black Box data failed.\r\n");
        }
        uip_free_gvariant_list(output_list);
        return FALSE;
    }

    g_printf("Downloading...\r\n");

    
    ret = get_file_transfer_state(CLASS_DIAGNOSE_NAME, OBJ_DIAGNOSE_NAME, PROPERTY_BLACKBOX_PROCESSBAR);
    if (ret != RET_OK) {
        goto EXIT;
    }

    filename = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), 0);
    ret = ipmc_change_file_owner(filename);
    if (ret != RET_OK) {
        goto EXIT;
    }

    g_printf("Download Black Box data to %s successfully.\r\n", filename);
    uip_free_gvariant_list(output_list);
    return TRUE;
EXIT:
    uip_free_gvariant_list(output_list);
    g_printf("Download Black Box data failed.\r\n");
    return FALSE;
}


gint32 ipmc_get_health(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 minor_cnt = 0;
    guint32 major_cnt = 0;
    guint32 critical_cnt = 0;
    gint32 ret;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_WARNING_MINOR_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_WARNING_MAJOR_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_WARNING_CRITICAL_NUM);
    ret = uip_multiget_object_property(CLASS_WARNING, OBJECT_WARNING, property_name_list, &property_value);
    if (ret != RET_OK) {
        g_printf("Get system health failed.\r\n");
        g_slist_free(property_name_list);
        return FALSE;
    } else {
        
        minor_cnt = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 0));

        
        major_cnt = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 1));

        
        critical_cnt = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 2));
        if (!(minor_cnt || major_cnt || critical_cnt)) {
            g_printf("System in health state.\r\n");
            g_slist_free(property_name_list);
            uip_free_gvariant_list(property_value);
            return TRUE;
        }

        g_printf("System Events:\r\n");

        if (minor_cnt) {
            g_printf("%u minor events.\r\n", minor_cnt);
        }

        if (major_cnt) {
            g_printf("%u major events.\r\n", major_cnt);
        }

        if (critical_cnt) {
            g_printf("%u critical events.\r\n", critical_cnt);
        }
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return TRUE;
}


gint32 ipmc_get_healthevents(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guint32 event_time = 0x00;
    guint16 record_id = 0x00;
    guint16 event_num = 0;
    guint16 next_record_id = 0x00;
    const gchar *event_sensor_name_ptr = NULL;
    const gchar *event_entity_name_ptr = NULL;
    const gchar *event_desc_ptr = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint8 event_level = 0;
    guint32 event_code = 0;
    gchar event_code_str[256] = {0};
    gint32 user_input = 0;
    gchar event_sensor_name[MAX_NAME_SIZE + MAX_ADINFO_LENGTH] = {0};
    gchar event_entity_name[MAX_NAME_SIZE] = {0};
    gchar event_desc[EVENT_DESC_LENGTH] = {0};
    gchar event_time_str[MAX_SEL_TIME_LEN] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    while (record_id != 0xffff) {
        input_list = g_slist_append(input_list, g_variant_new_uint16(record_id));
        ret =
            uip_set_object_property(CLASS_WARNING, OBJECT_WARNING, METHOD_WARNING_GET_EVENT, input_list, &output_list);
        if (record_id == 0x00 && ret == COMP_CODE_CANNOT_RET_DATA) { // 认为record_id为0 时为第一次循环；
            // 如果第一次调用方法得到特定的返回值，则系统处于健康状态
            g_printf("System in health state.\r\n");
            uip_free_gvariant_list(input_list);
            return TRUE;
        } else if (ret != VOS_OK) { // 其他情况获取失败
            uip_free_gvariant_list(input_list);
            g_printf("Get Event Obj failed.\r\n");
            if (ret == COMP_CODE_OUTOF_RANGE) {
                return TRUE;
            }
            return FALSE;
        } else if (record_id == 0x00) { // 获取成功即系统存在健康事件待打印，第一次时需打印表头
            g_printf("%-10s | %-20s | %-12s | %-12s | %s\r\n",
                "Event Num", "Event Time", "Alarm Level", "Event Code", "Event Description");
        }

        next_record_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
        event_entity_name_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
        event_sensor_name_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
        event_time = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 3));
        event_level = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 4));
        event_code = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 5));
        event_desc_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 6), NULL);

        
        
        if (event_sensor_name_ptr != NULL) {
            safe_fun_ret = strncpy_s(event_sensor_name, sizeof(event_sensor_name), event_sensor_name_ptr,
                sizeof(event_sensor_name) - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            strncpy_s(event_sensor_name, sizeof(event_sensor_name), "Unknown", strlen("Unknown"));
        }

        if (event_entity_name_ptr != NULL) {
            safe_fun_ret = strncpy_s(event_entity_name, sizeof(event_entity_name), event_entity_name_ptr,
                sizeof(event_entity_name) - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            strncpy_s(event_entity_name, sizeof(event_entity_name), "Unknown", strlen("Unknown"));
        }

        if (event_desc_ptr != NULL) {
            safe_fun_ret = strncpy_s(event_desc, sizeof(event_desc), event_desc_ptr, sizeof(event_desc) - 1);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        } else {
            strncpy_s(event_desc, sizeof(event_desc), "Unknown", strlen("Unknown"));
        }

        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);
        input_list = NULL;
        output_list = NULL;

        memset_s(event_time_str, MAX_SEL_TIME_LEN, 0, MAX_SEL_TIME_LEN);
        ret = uip_parse_sel_time(event_time, event_time_str, sizeof(event_time_str));
        if (ret != RET_OK) {
            g_printf("Parse event time failed.\r\n ");
            return FALSE;
        }

        snprintf_s(event_code_str, sizeof(event_code_str), sizeof(event_code_str) - 1, "0x%08X", event_code);

        g_printf("%-10d | %-20s | %-12s | %-12s | %s\r\n", ++event_num, event_time_str,
            (event_level > EVENT_ALARM_LEVEL_NUM - 1) ? "Unknown" : event_alarm_level[event_level], event_code_str,
            event_desc);

        if ((event_num % ONEPAGE_RECORD_NUMBER == 0) && (next_record_id != 0xffff)) {
            g_printf("\r\n");
            
            user_input = ipmc_get_user_input("Input 'q' quit:"); // 仅取第一个字符进行判断
            
            if (user_input == 'q' || user_input == 'Q') {
                return TRUE;
            }
        }

        record_id = next_record_id;
    }

    return TRUE;
}


gint32 ipmc_get_sel(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 selid = 0;

    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc < 2 || (strcmp(argv[0], "-v")) || (argc != 2 && strcmp(argv[1], "suggestion")) ||
        (!strcmp(argv[1], "suggestion") && argc != 3)) {
        printf_sel_helpinfo();
        return FALSE;
    }

    
    if (!strcmp(argv[1], "info")) {
        
        get_sel_info();
    } else if (!strcmp(argv[1], "list")) {
        
        if (get_sel_list() != RET_OK) {
            g_printf("List SEL failed.\r\n");
            return FALSE;
        }
    } else if (!strcmp(argv[1], "suggestion")) {
        if (vos_str2int(argv[2], 10, &selid, NUM_TPYE_INT32) != RET_OK || get_sel_suggestion(selid) != RET_OK) {
            g_printf("Please check ID.\r\n");
            return FALSE;
        }
    } else {
        printf_sel_helpinfo();
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 copy_log_file_to_tmp(const gchar *log_path, const gchar* temp_path)
{
#define MAX_FILE_SIZE (10 * 1024 * 1024) // 最大支持10M的文件拷贝
    gint32 ret;
    guint32 uid = 0;
    guint32 gid = 0;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    struct stat statbuf;
    if (stat(log_path, &statbuf) < 0) {
        debug_log(DLOG_ERROR, "Get operation log file stat fail.");
        return RET_ERR;
    }

    int size = statbuf.st_size;
    if (size > MAX_FILE_SIZE) {
        debug_log(DLOG_ERROR, "Get operation log too big.");
        return RET_ERR;
    }

    ret = ipmc_get_local_uid_gid(&uid, &gid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: ipmc_get_local_uid_gid fail", __FUNCTION__);
        return RET_ERR;
    }

    if (uid >= LDAP_USER_ID_BASE) {
        uid = APACHE_UID;
        gid = APPS_USER_GID;
    }

    if (dal_get_object_handle_nth(CLASS_PRIVILEGEAGENT, 0, &obj_handle) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: object handle fail", __FUNCTION__);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(log_path));    // 拷贝源路径
    input_list = g_slist_append(input_list, g_variant_new_string(temp_path));   // 拷贝目标路径
    input_list = g_slist_append(input_list, g_variant_new_uint32(uid));         // 拷贝后文件uid
    input_list = g_slist_append(input_list, g_variant_new_uint32(gid));         // 拷贝后文件gid
    input_list = g_slist_append(input_list, g_variant_new_uint32(0));           // 文件拷贝后不删除源文件
    ret = dfl_call_class_method(obj_handle, METHOD_PRIVILEGEAGENT_COPYFILE, NULL, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    return ret;
}


LOCAL gint32 ipmc_read_log_to_list(const gchar* log_path, GSList** log_list)
{
    guint32 log_count = 0;
    gchar tmp_buf[MAX_PRINT_STR_LEN + 1] = {0};

    // 防止并行执行, 使用进程ID，作为临时文件
    gchar tmp_file_path[MAX_FILEPATH_LENGTH] = {0};
    (void)snprintf_truncated_s(tmp_file_path, sizeof(tmp_file_path), "/dev/shm/tmp_operation_log_%d", getpid());
    // 将日志文件拷贝为临时文件
    if (copy_log_file_to_tmp(log_path, (const gchar*)tmp_file_path) != RET_OK) {
        g_printf("Failed to copy operation logs to a temporary file.\r\n");
        return RET_ERR;
    }

    // 修改文件权限，赋予读权限
    if (dal_set_file_mode(tmp_file_path, 444) != RET_OK) {
        g_printf("Failed to modify the file permission.\r\n");
        return RET_ERR;
    }

    FILE* pf = fopen((const gchar*)tmp_file_path, "r");
    if (pf == NULL) {
        g_printf("open operation log fail\r\n");
        dal_delete_file((const gchar*)tmp_file_path);
        return RET_ERR;
    }

    // 按行读取操作日志
    while (fgets(tmp_buf, sizeof(tmp_buf) - 1, pf) != NULL) {
        gchar* str_line = tmp_buf;
        while (*str_line == '\0' && str_line - tmp_buf <= sizeof(tmp_buf) - 1) {
            str_line++;
        }
        
        if (str_line - tmp_buf > sizeof(tmp_buf) - 1) {
            continue;
        }

        dal_filter_invisible_char(str_line);
        if (*str_line == '\0') {
            continue;
        }
        *log_list = g_slist_append(*log_list, g_variant_new_string(str_line));
        memset_s(tmp_buf, sizeof(tmp_buf), 0, sizeof(tmp_buf));
        log_count++;
    }

    (void)fclose(pf);
    pf = NULL;
    (void)dal_delete_file((const gchar*)tmp_file_path);

    if (log_count == 0) {
        g_printf("No operation log.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 ipmc_print_log(const gchar *log_path)
{
    GSList* log_list = NULL;
    gint32 ret = 0;

    // 检查文件是否存在
    if (g_file_test(log_path, G_FILE_TEST_EXISTS) != TRUE) {
        g_printf("No operation log.\r\n");
        return RET_OK;
    }

    ret = ipmc_read_log_to_list(log_path, &log_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: read log to file fail", __FUNCTION__);
        return ret;
    }

    gint32 log_count = g_slist_length(log_list);
    guchar num = 0;
    for (gint32 i = log_count; i > 0; i--) {
        if (num == ONEPAGE_RECORD_NUMBER) {
            g_printf("Input 'q' to quit:\r\n");
            gint32 user_input = ipmc_get_user_input(""); // 仅取第一个字符进行判断
            if (user_input == 'q' || user_input == 'Q') {
                break;
            }
            num = 0;
        }

        gsize str_len = 0;
        const gchar *str_line = g_variant_get_string((GVariant *)g_slist_nth_data(log_list, i - 1), &str_len);
        if (str_line == NULL || str_len == 0) {
            g_printf("The operation log contains empty lines.\r\n");
            g_slist_free_full(log_list, (GDestroyNotify)g_variant_unref);
            return RET_ERR;
        }

        if (str_line[str_len - 1] == '\n') {
            g_printf("%s", str_line);
        } else {
            g_printf("%s\n", str_line);
        }

        num++;
    }

    g_slist_free_full(log_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}


gint32 ipmc_get_operate_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 iRet;
    gchar file_path[MAX_FILE_NAME_LEN + 1] = {0};
    gchar file_name[MAX_FILE_NAME_LEN + 1] = {0};
    gchar real_path[MAX_FILE_NAME_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_LOG, 0, &obj_handle) != RET_OK) {
        g_printf("Get Log object failed.\r\n");
        return FALSE;
    }

    // 读取文件路径
    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOG_DIR, file_path, sizeof(file_path));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_LOG_OPERATELOG_FILE, file_name, sizeof(file_name));

    if (strlen(file_path) + strlen(file_name) >= MAX_FILE_NAME_LEN - 2) {
        return FALSE;
    }

    
    iRet = snprintf_s(real_path, sizeof(real_path), sizeof(real_path) - 1, "%s/%s", file_path, file_name);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    

    return ipmc_print_log(real_path);
}


gint32 ipmc_get_serialnumber(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    const gchar *serial = NULL;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_NAME_SMBIOS, OBJ_NAME_SMBIOS, PROTERY_SMBIOS_SERIALNUM, &value);
    if (ret != RET_OK) {
        g_printf("Get system SN failed.\r\n");
        return FALSE;
    } else {
        serial = g_variant_get_string(value, 0);
    }

    g_printf("System SN is:%s\r\n", serial);
    g_variant_unref(value);
    return TRUE;
}

gint32 print_fru_info(OBJ_HANDLE obj_handle)
{
    gint32 retval;
    struct fru_primary_props fru_basic_props = {0};
    const gchar *fru_prop_names[ELABLE_PROPERTIES_NUM] = {
        "Chassis Type          ", "Chassis Part Number   ", "Chassis Serial Number ", "Chassis Extend label  ",
        "Board Mfg. Date       ", "Board Manufacturer    ", "Board Product Name    ", "Board Serial Number   ",
        "Board Part Number     ", "Board FRU File ID     ", "Extend label          ", "Product Manufacturer  ",
        "Product Name          ", "Product Part Number   ", "Product Version       ", "Product Serial Number ",
        "Product Asset Tag     ", "Product FRU File ID   ", "Product Extend label  "
    };

    gint32 ret = __extract_fru_basic_props(obj_handle, &fru_basic_props, &retval);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __extract_fru_basic_props failed", __FUNCTION__);
        __destruct_fru_basic_props(&fru_basic_props);
        return retval;
    }

    if (fru_basic_props.ref_obj_elabel == NULL || strlen(fru_basic_props.ref_obj_elabel) == 0) {
        __destruct_fru_basic_props(&fru_basic_props);
        return RET_OK;
    }

    GSList *fru_prop_value = NULL;
    ret = __extract_fru_props(obj_handle, &fru_basic_props, &fru_prop_value);
    __destruct_fru_basic_props(&fru_basic_props);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __extract_fru_props failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = __print_fru_props(fru_prop_value, fru_prop_names, ELABLE_PROPERTIES_NUM);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __print_fru_props failed", __FUNCTION__);
    }

    uip_free_gvariant_list(fru_prop_value);
    return ret;
}


gint32 ipmc_get_fruinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 fruid = 0;
    guint32 obj_num;
    GVariant *value = NULL;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    FRU_OBJECT_S *fru_object = NULL;
    guint32 count = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_FRU, &obj_list);
    if (ret != DFL_OK) {
        g_printf("Get FRU object list failed.\r\n");
        return FALSE;
    }

    obj_num = g_slist_length(obj_list);
    if (obj_num == 0) {
        g_printf("System has no fru information.\r\n");
        return FALSE;
    }

    fru_object = (FRU_OBJECT_S *)malloc(sizeof(FRU_OBJECT_S) * obj_num);
    if (fru_object == NULL) {
        g_printf("CLI malloc failed.\r\n");
        g_slist_free(obj_list);
        return FALSE;
    }

    memset_s(fru_object, sizeof(FRU_OBJECT_S) * obj_num, 0, sizeof(FRU_OBJECT_S) * obj_num);
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        fru_object[count].obj_handle = (OBJ_HANDLE)obj_note->data;
        ret = dfl_get_property_value((OBJ_HANDLE)obj_note->data, PROPERTY_FRU_ID, &value);
        if (ret != DFL_OK) {
            g_printf("Get FRU id failed.\r\n");
            g_slist_free(obj_list);
            free(fru_object);
            return FALSE;
        }

        fru_object[count].fru_id = g_variant_get_byte(value);
        count++;
        g_variant_unref(value);
        value = NULL;
    }

    array_object_handle_by_fruid(fru_object, obj_num);
    if (target == NULL) {
        for (count = 0; count < obj_num; count++) {
            
            if (fru_object[count].fru_id == INVALID_FRUID) {
                continue;
            }
            

            
            ret = print_fru_info(fru_object[count].obj_handle);
            if (ret != RET_OK) {
                g_printf("Get fru%d information failed.\r\n", fru_object[count].fru_id);
                g_slist_free(obj_list);
                free(fru_object);
                return FALSE;
            }
        }
    } else {
        
        fruid = 0;

        for (count = 0; count < obj_num; count++) {
            
            if (fru_object[count].fru_id == fruid) {
                ret = print_fru_info(fru_object[count].obj_handle);
                if (ret != RET_OK) {
                    g_printf("Get fru%d information failed.\r\n", fru_object[count].fru_id);
                    g_slist_free(obj_list);
                    free(fru_object);
                    return FALSE;
                }
            }
        }
    }

    g_slist_free(obj_list);
    free(fru_object);
    return TRUE;
}


gint32 ipmc_get_systemname(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    const gchar *sys_name = NULL;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_NAME_BMC, OBJ_NAME_BMC, PROPERTY_BMC_SYSTEMNAME, &value);
    if (ret != RET_OK) {
        g_printf("Get system name failed.\r\n");
        return FALSE;
    } else {
        sys_name = g_variant_get_string(value, 0);
    }

    g_printf("System name is:%s\r\n", sys_name);
    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_maintenance_biosprint(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get bios handle failed\n");
        g_printf("Get BIOS print info failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, BIOS_PRINT_FLAG_NAME, &status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get BIOS property failed\n");
        g_printf("Get BIOS print info failed.\r\n");
        return FALSE;
    }

    if (status == BIOS_DEGUG_INFO_ENABLE) {
        g_printf("BIOS debug info enable\r\n");
    } else if (status == BIOS_DEGUG_INFO_BY_SETUP_MENU) {
        
        g_printf("BIOS debug info enabled/disabled by BIOS setup menu\r\n");
        
    } else {
        g_printf("unknown status\r\n");
    }

    return ret;
}


gint32 ipmc_get_maintenance_coolingpowermode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 status = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get Cooling handle failed\n");
        g_printf("Get cooling power mode failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, COOLING_PROPERTY_POWER_MODE, &status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get Cooling.PowerMode property failed\n");
        g_printf("Get cooling power mode failed.\r\n");
        return FALSE;
    }

    if (status == 0) {
        g_printf("Power saving mode\r\n");
    } else {
        g_printf("High reliability mode\r\n");
    }

    return ret;
}


gint32 ipmc_get_maintenance_raidcom(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 com_channel = 0xff;
    gchar controller_name[PROP_VAL_LENGTH] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t maintenance -d raidcom \r\n");
        return FALSE;
    }

    ret = dal_get_specific_object_byte(SOL_CLASS_NAME, PROPERTY_SOL_COM_ID, SERIAL_SELECTOR_RAID, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get RAID com channel information failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOL_COM_CHANNEL, &com_channel);
    if (ret != RET_OK) {
        g_printf("Get RAID com channel information failed.\r\n");
        return FALSE;
    }

    ret = dal_get_specific_object_byte(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_COM_CHANNEL, com_channel,
        &obj_handle);
    if (ret != RET_OK) {
        g_printf("No RAID com connected.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME, controller_name,
        sizeof(controller_name));

    g_printf("Current RAID com connected:\r\n");
    g_printf("%-16s:  %d\r\n", "Com Channel", com_channel);
    g_printf("%-16s:  %s\r\n", "Device Name", controller_name);

    return TRUE;
}


gint32 ipmc_get_maintenance_ps_on_lock_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    guchar psOnState = 0;
    OBJ_HANDLE objHandle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -t maintenance -d poweronlock\r\n");
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_CHASSISPAYLOAD, 0, &objHandle);
    if (ret != RET_OK) {
        g_printf("Get power on lock state failed.\r\n");
        return FALSE;
    }
    ret = dal_get_property_value_byte(objHandle, PROPERTY_PAYLOAD_PWRON_TM_SRC, &psOnState);
    if (ret != RET_OK) {
        g_printf("Get power on lock state failed.\r\n");
        return FALSE;
    }
    g_printf("Power on lock state:   %s\r\n", psOnState ? "Locked" : "Unlocked");
    return ret;
}


gint32 ipmc_get_maintenance_ipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE group_obj = 0;
    gchar maint_ip_addr[IPV4_IP_STR_SIZE + 1] = { 0 };
    gchar maint_subnet_mask[IPV4_IP_STR_SIZE + 1] = { 0 };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, MAINT_DEDICATED_ETHERNET,
        &group_obj);
    if (ret != RET_OK) {
        g_printf("Get the maintenance IP information of dedicated port failed.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_IP_ADDR, maint_ip_addr, sizeof(maint_ip_addr));
    (void)dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_SUB_MASK, maint_subnet_mask,
        sizeof(maint_subnet_mask));

    if (strlen(maint_ip_addr) != 0) {
        g_printf("IP Address  : %s\r\n", maint_ip_addr);
    } else {
        g_printf("IP Address  : \r\n");
    }

    if (strlen(maint_subnet_mask) != 0) {
        g_printf("Subnet Mask : %s\r\n", maint_subnet_mask);
    } else {
        g_printf("Subnet Mask : \r\n");
    }

    return TRUE;
}


static gint32 get_collect_configuration(gchar *file_path_in, guint32 len)
{
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    gint32 status = 0;
    const gchar *err_string = NULL;
    guint32 i;

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    
    if ((file_path_in == NULL) || (strlen(file_path_in) == 0)) {
        input_list = g_slist_append(input_list, g_variant_new_string(""));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_string(file_path_in));
    }

    
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, BMC_CLASEE_NAME,
        BMC_OBJECT_NAME, METHOD_EXPORT_ALL_CONFIG, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Initiate config output failed.");
        return FALSE;
    }

    
    g_printf("Collecting configuration...\r\n");

    for (i = 0; i < 600; i++) {
        (void)vos_task_delay(1000);

        ret = uip_call_class_method(NULL, NULL, NULL, BMC_CLASEE_NAME, BMC_OBJECT_NAME, METHOD_GET_IMPORT_EXPORT_STATUS,
            NULL, &output_list);
        if (ret != RET_OK) {
            return FALSE;
        }

        status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        err_string = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
        if (status < 0) {
            if (i > 0) {
                g_printf("\r\n"); 
            }

            g_printf("%s\r\n", err_string);

            uip_free_gvariant_list(output_list);

            return FALSE;
        }

        uip_free_gvariant_list(output_list);
        err_string = NULL;

        g_printf("\r%2d%%", status);
        (void)fflush(stdout);

        if (status == 100) {
            g_printf("\r\n");
            return TRUE;
        }
    }

    g_printf("\r\n");
    return FALSE;
}


gint32 ipmc_get_config_export(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
#define PREFIX_SIZE 5
    gchar file_path[MAX_FILEPATH_LENGTH + 1] = {0};
    gint32 ret;
    gint32 len = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_export_config_usage();
        return FALSE;
    }

    if (is_support_bios_configuration()) {
        g_printf("NOTE: The exported RAID Controller configurations are valid only if they are exported after the POST "
                 "is complete.\r\n");
    }

    gboolean is_local_file = TRUE;
    const gchar* remote_prefix[PREFIX_SIZE] = {"https://", "sftp://", "nfs://", "cifs://", "scp://"};

    for (gint32 i = 0; i < PREFIX_SIZE; i++) {
        if (strncmp(argv[1], remote_prefix[i], strlen(remote_prefix[i])) == 0) {
            is_local_file = FALSE;
            break;
        }
    }

    
    if (is_local_file == TRUE) {
        if ((strlen(argv[1]) > MAX_FILEPATH_LENGTH) || (strncmp(argv[1], "/", strlen("/")))) {
            print_export_config_usage();
            return FALSE;
        }

        
        ret = dal_check_real_path2(argv[1], TMP_PATH_WITH_SLASH);
        if (ret != RET_OK) {
            g_printf("Export configuration failed.\r\n");
            return FALSE;
        }

        (void)vos_get_file_dir(argv[1], file_path, MAX_FILEPATH_LENGTH + 1);
        
        len = strlen(file_path);
        file_path[len] = '/';
        file_path[len + 1] = '\0';
        

        
        
        ret = vos_get_file_accessible(file_path);
        if (ret != TRUE) {
            g_printf("Export configuration failed.\r\n");
            return FALSE;
        }
        

        
        ret = get_collect_configuration(argv[1], MAX_FILEPATH_LENGTH + 1);
        if (ret != TRUE) {
            g_printf("Export configuration failed.\r\n");
            return FALSE;
        }
    } else {
        
        ret = get_collect_configuration(NULL, MAX_FILEPATH_LENGTH + 1);
        if (ret != TRUE) {
            g_printf("Export configuration failed.\r\n");
            return FALSE;
        }
        ret = upload_file_to_url(argv[1], FILE_ID_CONFIG_EXPORT_IMPORT);
        clear_sensitive_info(argv[1], strlen(argv[1]));
        (void)dal_delete_file(GET_CONFIG_FILE_NAME);
        if (ret != TRUE) {
            g_printf("Export configuration failed.\r\n");
            return FALSE;
        }
    }

    g_printf("Export configuration successfully.\r\n");
    return TRUE;
}


gint32 ipmc_get_notimeoutstate(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    GVariant* value = NULL;
    guint32 cli_session_timeout;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d notimeoutstate\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(BMC_CLASEE_NAME, BMC_OBJECT_NAME, PROPERTY_BMC_CLI_SESSION_TIMEOUT, &value);
    if (ret != RET_OK) {
        g_printf("Usage: ipmcget -d notimeoutstate\r\n");
        return FALSE;
    } else {
        cli_session_timeout = g_variant_get_uint32(value);
        if (cli_session_timeout == 0) {
            g_printf("Current notimeout state: enabled\r\n");
        } else {
            g_printf("Current notimeout state: disabled\r\n");
            g_printf("Timeout period: %d(min)\r\n", cli_session_timeout);
        }
    }

    g_variant_unref(value);
    return TRUE;
}
