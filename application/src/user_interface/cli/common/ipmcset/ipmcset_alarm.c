

#include "ipmcset_alarm.h"


gint32 get_eventcode_num(gchar *str, guint32 *retval)
{
    gchar *end = NULL;

    if (str == NULL) {
        return RET_ERR;
    }

    if (strlen(str) > 2 && strlen(str) <= 10) {
        if ((strncmp(str, "0x", 2) == 0) || (strncmp(str, "0X", 2) == 0)) {
            *retval = (guint32)strtoul(str, &end, 16);

            if (*end == 0) {
                return RET_OK;
            }
        }
    }

    return RET_ERR;
}



void _sensor_stop_all_test(void)
{
    GSList *obj_list = NULL;
    GSList *node = NULL;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar userip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, userip, sizeof(userip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "$Supervisor");
        snprintf_s(userip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "IPC");
        
    }

    // 数值型
    if (dfl_get_object_list(CLASS_THRESHOLD_SENSOR, &obj_list) != DFL_OK) {
        g_printf("Get threshold sensor list fail.\r\n");
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(0));
    input_list = g_slist_append(input_list, g_variant_new_double(0));

    for (node = obj_list; node; node = node->next) {
        (void)uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip,
            CLASS_THRESHOLD_SENSOR, (OBJ_HANDLE)node->data, METHOD_THS_SENSOR_TEST, input_list, NULL);
    }

    uip_free_gvariant_list(input_list);
    g_slist_free(obj_list);
    input_list = NULL;
    obj_list = NULL;
    // 离散型
    if (dfl_get_object_list(CLASS_DISCERETE_SENSOR, &obj_list) != DFL_OK) {
        g_printf("Get discrete sensor list fail.\r\n");
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(0));
    input_list = g_slist_append(input_list, g_variant_new_uint16(0));

    for (node = obj_list; node; node = node->next) {
        (void)uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip,
            CLASS_DISCERETE_SENSOR, (OBJ_HANDLE)node->data, METHOD_DIS_SENSOR_TEST, input_list, NULL);
    }
    uip_free_gvariant_list(input_list);
    g_slist_free(obj_list);

    g_printf("Stop all sensor test successfully.\r\n");
}



gint32 get_object_by_sensor_name(const gchar *class_name, gchar *property_name, gchar *sensor_name,
    OBJ_HANDLE *obj_handle)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    gchar sensor_name_tmp[SENSOR_NAME_MAX_LENGTH] = {0};

    if (class_name == NULL || property_name == NULL || sensor_name == NULL || obj_handle == NULL) {
        return ERRCODE_WRONG_PARAMETER;
    }
    result = dfl_get_object_list(class_name, &handle_list);
    if (result != DFL_OK) {
        return result;
    }

    result = ERRCODE_OBJECT_NOT_EXIST;

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        gint32 result_get_prop =
            dal_get_property_value_string(object_handle, property_name, sensor_name_tmp, sizeof(sensor_name_tmp));
        if (result_get_prop != RET_OK) {
            g_slist_free(handle_list);
            return result_get_prop;
        }

        if (strcasecmp(sensor_name, sensor_name_tmp) == 0) {
            *obj_handle = object_handle;
            result = RET_OK;
            break;
        }
    }

    g_slist_free(handle_list);
    return result;
}




gint32 get_certificate_filepath(guchar argc, gchar **argv, gchar *file, guint32 len)
{
    errno_t safe_fun_ret;
    guint8 num;
    if (file == NULL) {
        return RET_ERR;
    }

    if (argc < 2) {
        return RET_ERR;
    }

    
    if (argv == NULL) {
        return RET_ERR;
    }

    for (num = 0; num < argc; ++num) {
        if (argv[num] == NULL) {
            return RET_ERR;
        }
    }

    if (strncmp(argv[0], "-v", strlen("-v") + 1)) {
        return RET_ERR;
    }

    
    if ((size_t)strlen(argv[1]) >= (size_t)len || strncmp(argv[1], "/", strlen("/")) ||
        dal_check_real_path(argv[1]) != RET_OK) {
        
        return RET_ERR;
    }
    
    safe_fun_ret = strncpy_s(file, len, argv[1], len - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    

    return RET_OK;
}


gint32 get_eventsource_value(guchar argc, gchar** argv, guint8* idx, guint32* out)
{
    gint32 count;
    guint32 value = 0;
    gint32 tmp;
    guint32 minlen;  

    
    if (argc < 3) {
        return VOS_ERR;
    }

    
    tmp = str_to_int(argv[1]);
    return_val_if_fail(!((tmp < 1) || (tmp > 4) || (strlen(argv[1]) != 1)), VOS_ERR); 
    *idx = (guint8)(tmp - 1); 

    count = 2; 
    if (argc == 3) {    
        minlen = (strlen(argv[count]) < strlen("none")) ? strlen(argv[count]) : strlen("none");
        if (strlen(argv[count]) <= strlen("none") && !strncmp("none", argv[count], minlen)) {
            *out = 0;
            return RET_OK;
        }
        minlen = (strlen(argv[count]) < strlen("all")) ? strlen(argv[count]) : strlen("all");
        if (strlen(argv[count]) <= strlen("all") && !strncmp("all", argv[count], minlen)) {
            *out = 0x7;
            return RET_OK;
        }
    }

    for (; count < argc; count++) {
        minlen = (strlen(argv[count]) < strlen("operationlogs")) ? strlen(argv[count]) : strlen("operationlogs");
        if (strlen(argv[count]) <= strlen("operationlogs") && !strncmp("operationlogs", argv[count], minlen)) {
            value = value | 0x01;
            continue;
        }
        minlen = (strlen(argv[count]) < strlen("securitylogs")) ? strlen(argv[count]) : strlen("securitylogs");
        if (strlen(argv[count]) <= strlen("securitylogs") && !strncmp("securitylogs", argv[count], minlen)) {
            value = value | 0x02;
            continue;
        }
        minlen = (strlen(argv[count]) < strlen("eventlogs")) ? strlen(argv[count]) : strlen("eventlogs");
        if (strlen(argv[count]) <= strlen("eventlogs") && !strncmp("eventlogs", argv[count], minlen)) {
            value = value | 0x04;
            continue;
        }
        return RET_ERR;
    }

    *out = value;
    return RET_OK;
}


gint32 get_syslog_severity_value(guchar argc, gchar **argv, guint32 *out)
{
    gint32 ret = RET_OK;

    if (argc == 2) {
        if (!strcmp(argv[1], "none")) {
            *out = 0;
        } else if (!strcmp(argv[1], "all")) {
            *out = 0x0F;
        } else if (!strcmp(argv[1], "normal")) {
            *out = 0x01 | 0x2 | 0x4 | 0x8;
        } else if (!strcmp(argv[1], "minor")) {
            *out = 0x2 | 0x4 | 0x8;
        } else if (!strcmp(argv[1], "major")) {
            *out = 0x4 | 0x8;
        } else if (!strcmp(argv[1], "critical")) {
            *out = 0x8;
        } else {
            ret = RET_ERR;
        }
    } else {
        ret = RET_ERR;
    }

    return ret;
}


gint32 set_syslog_state(guchar state, gchar *str_buf)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (!str_buf) {
        g_printf("set_syslog_state:str_buf is null ptr.\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_SET_ENABLE_STATE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog %s failed.\r\n", str_buf);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog %s successfully.\r\n", str_buf);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 set_syslog_dest_state(guchar dest, guchar state, gchar *str_buf)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (str_buf == NULL) {
        g_printf("set_syslog_dest_state:str_buf is null ptr.\n");
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = dal_get_specific_object_byte(CLASS_SYSLOG_ITEM_CONFIG, PROPERTY_REMOTE_SYSLOG_INDEX, dest - 1, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_ITEM_CONFIG, obj_handle, METHOD_REMOTE_SYSLOG_SET_ENABLE_STATE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog dest%d %s failed.\r\n", dest, str_buf);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog dest%d %s successfully.\r\n", dest, str_buf);
    uip_free_gvariant_list(input_list);
    return TRUE;
}

void ipmc_log_operation_log(const gchar *target, const gchar *log_context)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;

    if (log_context == NULL || target == NULL) {
        return;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_string(target));
    input_list = g_slist_append(input_list, g_variant_new_string(log_context));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOG, OBJECT_LOG,
        METHOD_OPERATE_LOG_PROXY, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        g_printf("Log operation log failed.\r\n");
    }

    return;
}
