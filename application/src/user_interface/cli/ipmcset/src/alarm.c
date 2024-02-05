

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>

#include "ipmcset_user_security.h"
#include "ipmcset_alarm.h"
#include "ipmcset_power.h"


gint32 ipmc_set_precisealarm_mock(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 eventcode = 0;
    guint8 isMocked = 0;
    guint8 isAssert = 0;
    guint16 subject_index = 0;

    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar userip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc < 2) || (argc > 4) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_precisealarm_mock_help();
        return FALSE;
    }
    if (argv[1] == NULL) {
        print_precisealarm_mock_help();
        return FALSE;
    }

    // 取消告警事件模拟，subjectindex不能出现在命令行deassert表示
    if (strcmp(argv[1], "stopall") == 0) {
        if (argc != 2) {
            print_precisealarm_mock_help();
            return FALSE;
        }
        eventcode = 0xFFFFFFFF;
        isMocked = 0;
        isAssert = 0;
    } else {
        // eventcode有效性判断
        ret = get_eventcode_num(argv[1], &eventcode);
        if (ret != RET_OK) {
            g_printf("Invalid event code.\r\n");
            return FALSE;
        }
        if (strcmp(argv[argc - 1], "assert") != 0 && strcmp(argv[argc - 1], "deassert") != 0 &&
            strcmp(argv[argc - 1], "stop") != 0) {
            print_precisealarm_mock_help();
            return FALSE;
        };
    }

    // 事件码特定的事件主体subjectindex有效性判断
    if (argc == 4) {
        ret = str_to_int(argv[2]);
        if (ret == IPMC_ERROR) {
            g_printf("Invalid subject index.\r\n");
            return FALSE;
        }
        // 有效范围为1-65535
        if (ret <= 0 || ret > MAX_SUBJECT_INDEX) {
            g_printf("Invalid subject index.\r\n");
            return FALSE;
        }
        subject_index = (guint16)ret;

        // eventcode为0xFFFFFFFF以及0x**FFFFFF时，subjectindex不能输入，否则命令有误
        if ((eventcode == 0xFFFFFFFF) || ((eventcode & 0x00FFFFFF) == 0x00FFFFFF)) {
            print_precisealarm_mock_help();
            return FALSE;
        }
    }

    // 模拟告警事件产生/恢复获取
    if (strcmp(argv[argc - 1], "stop") == 0) {
        isMocked = 0;
    } else if (strcmp(argv[argc - 1], "assert") == 0) {
        isAssert = 1;
        isMocked = 1;
    } else if (strcmp(argv[argc - 1], "deassert") == 0) {
        isAssert = 0;
        isMocked = 1;
    }

    if (get_username_ip(username, userip, sizeof(userip)) != RET_OK) {
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(userip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    // 封装input_list ，模拟告警接口调用
    input_list = g_slist_append(input_list, g_variant_new_uint32(eventcode));
    input_list = g_slist_append(input_list, g_variant_new_byte(isMocked));
    if (argc == 4) {
        input_list = g_slist_append(input_list, g_variant_new_uint16(subject_index));
    } else {
        input_list = g_slist_append(input_list, g_variant_new_uint16(0));
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(isAssert));

    // 告警信息管理对象获取
    ret = dal_get_object_handle_nth(CLASS_EVENT_INFORMATION, 0, &obj_handle);
    if (obj_handle == 0) {
        uip_free_gvariant_list(input_list);
        g_printf("Wrong value.\r\n");
        return FALSE;
    }

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip,
        CLASS_EVENT_INFORMATION, obj_handle, METHOD_EVENT_MONITOR_PRESICE_ALARM_SIMULATE, input_list, NULL);

    uip_free_gvariant_list(input_list);

    // 命令执行出错，提示检查参数范围，返回FALSE
    if (ret != RET_OK) {
        g_printf(
            "Precise alarm mock failed.\r\nPlease check the range of value, or check operation log for detail.\r\n");
        return FALSE;
    }

    // 成功
    g_printf("Precise alarm mock successfully.\r\n");
    return TRUE;
}


gint32 ipmc_clear_sel(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v")) || (strcmp("clear", argv[1]))) {
        g_printf("Usage: ipmcset %s-d sel -v clear\r\n", help_default_name);
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SEL, CLASS_SEL,
        METHOD_SEL_CLEAR_SEL, NULL, NULL);
    if (ret != RET_OK) {
        g_printf("Clear SEL records failed.");
        return FALSE;
    }

    g_printf("Clear SEL records successfully.\r\n");

    return TRUE;
}



gint32  ipmc_set_sensor_state(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    GSList* input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar userip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 state;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc < SECOND_PARA) || (argc > THIRD_PARA) || ((argv[0]) && strcmp(argv[0], "-v")) ||
        (argv[FIRST_PARA] == NULL) || (argv[SECOND_PARA] == NULL)) {
        print_sensor_state_help();
        return FALSE;
    }
    if (strcmp(argv[SECOND_PARA], "disabled") == 0) {
        state = SENSOR_DISABLE;
    } else if (strcmp(argv[SECOND_PARA], "enabled") == 0) {
        state = SENSOR_ENABLE;
    } else {
        print_sensor_state_help();
        return FALSE;
    }
    if (get_username_ip(username, userip, sizeof(userip)) != VOS_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(userip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    // 门限传感器不支持去使能
    if (get_object_by_sensor_name(CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_SENSOR_NAME, argv[1], 
        &obj_handle) == RET_OK) {
        g_printf("The threshold sensor cannot be %s.\r\n", (state == SENSOR_ENABLE) ? "enabled" : "disabled");
        return FALSE;
    }
    // 查找离散型
    if (get_object_by_sensor_name(CLASS_DISCERETE_SENSOR, PROPERTY_DIS_SENSOR_SENSOR_NAME, argv[1],
        &obj_handle) != RET_OK) {
        g_printf("Wrong sensor name.\r\n");
        return FALSE;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)userip,
                                            CLASS_DISCERETE_SENSOR, obj_handle, METHOD_DIS_SENSOR_STATE, 
                                            input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("%s sensor failed.\r\n", (state == SENSOR_ENABLE) ? "Enable" : "Disable");
        return FALSE;
    }
    g_printf("%s sensor successfully.\r\n", (state == SENSOR_ENABLE) ? "Enable" : "Disable");
    return TRUE;
}




gint32 ipmc_set_sensor_test(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar userip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

#define TYPE_THRESHOLD 1
#define TYPE_DISCRETE 0
    gint32 type = TYPE_THRESHOLD; // 1 数值型; 0 离散型
    guint32 enable = 1;
    gdouble value = 0;
    guint32 discrete_value = 0;
    gchar *end = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if ((argc < 2) || (argc > 3) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_sensor_test_help();
        return FALSE;
    }
    if (argv[1] == NULL) {
        print_sensor_test_help();
        return FALSE;
    }

    // 停止所有测试
    if (strcmp(argv[1], "stopall") == 0) {
        if (argc != 2) {
            g_printf("Too many parameters for stopall.\r\n");
            return FALSE;
        }
        _sensor_stop_all_test();
        return TRUE;
    }

    // 必须要有参数2
    if (argv[2] == NULL) {
        print_sensor_test_help();
        return FALSE;
    }

    // 先查找数值型
    
    type = TYPE_THRESHOLD;
    ret = get_object_by_sensor_name(CLASS_THRESHOLD_SENSOR, PROPERTY_THR_SENSOR_SENSOR_NAME, argv[1], &obj_handle);
    if (ret != RET_OK) {
        // 再查找离散型
        type = TYPE_DISCRETE;
        (void)get_object_by_sensor_name(CLASS_DISCERETE_SENSOR, PROPERTY_DIS_SENSOR_SENSOR_NAME, argv[1], &obj_handle);
    }

    
    if (obj_handle == 0) {
        g_printf("Wrong sensor name.\r\n");
        return FALSE;
    }

    if (strcmp(argv[2], "stop") == 0) {
        enable = 0;
    } else {
        if (type == TYPE_THRESHOLD) {
            // 数值型，转换为浮点数
            value = (gdouble)strtod(argv[2], &end);
            if (end[0] != 0) {
                g_printf("Wrong value.\r\n");
                return FALSE;
            }
        } else {
            // 离散型，限定0~0xFFFF
            discrete_value = (guint32)str_to_int(argv[2]);
            if (discrete_value > 0xFFFF) {
                g_printf("Wrong value.\r\n");
                return FALSE;
            }
        }
    }

    if (get_username_ip(username, userip, sizeof(userip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "$Supervisor");
        snprintf_s(userip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "IPC");
        
    }

    // 调用Test方法
    if (type == TYPE_THRESHOLD) { // 数值型
        input_list = g_slist_append(input_list, g_variant_new_uint32(enable));
        input_list = g_slist_append(input_list, g_variant_new_double(value));
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip,
            CLASS_THRESHOLD_SENSOR, obj_handle, METHOD_THS_SENSOR_TEST, input_list, NULL);
    } else { // 离散型
        input_list = g_slist_append(input_list, g_variant_new_uint32(enable));
        input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)discrete_value));
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)userip,
            CLASS_DISCERETE_SENSOR, obj_handle, METHOD_DIS_SENSOR_TEST, input_list, NULL);
    }

    uip_free_gvariant_list(input_list);

    // 命令执行出错，提示检查value范围，返回FALSE
    if (ret != RET_OK) {
        g_printf("Sensor test failed.\r\nPlease check the range of value, or check operation log for detail.\r\n");
        return FALSE;
    }

    // 成功
    g_printf("Sensor test successfully.\r\n");
    return TRUE;
}



gint32 ipmc_set_syslog_auth(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar str_buff[TRAP_MODE_STR_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    guint8 value = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_syslogauth_usage();
        return FALSE;
    }

    if (argv[1] == NULL) {
        print_syslogauth_usage();
        return FALSE;
    }

    (void)memset_s(str_buff, TRAP_MODE_STR_LEN, 0x00, TRAP_MODE_STR_LEN);

    if (strcmp(argv[1], "1") == 0) {
        // 1     one way authentication
        value = 1;
    } else if (strcmp(argv[1], "2") == 0) {
        // 2     mutual authentication
        value = 2;
    } else {
        print_syslogauth_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(value));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_SET_AUTH_TYPE, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog auth type failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog auth type successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslog_clientcertificate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    errno_t secur_rev;
    gchar path_info[MAX_FILEPATH_LENGTH + 1] = {0};
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    gchar pw[PARAMETER_LEN + 1] = {0};
    gchar *client_pw = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = get_certificate_filepath(argc, argv, path_info, sizeof(path_info));
    if (ret != RET_OK) {
        print_syslog_clientcertificate_usage();
        return FALSE;
    }

    if ((client_pw = getPasswd("Password:")) == NULL) {
        g_printf("Incorrect password.\r\n");
        return RET_ERR;
    }

    secur_rev = strcpy_s(pw, sizeof(pw), client_pw);
    clear_sensitive_info(client_pw, strlen(client_pw));
    if (secur_rev != EOK) {
        debug_log(DLOG_ERROR, "strcpy_s failed, ret = %d.", secur_rev);
        return RET_ERR;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        clear_sensitive_info(pw, sizeof(pw));
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(SYSLOG_CERT_TYPE_CLIENT));
    input_list = g_slist_append(input_list, g_variant_new_string(path_info));
    input_list = g_slist_append(input_list, g_variant_new_string(pw));

    clear_sensitive_info(pw, sizeof(pw));
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SYSLOG_CONFIG,
        OBJECT_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_IMPORT_CERT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set syslog client certificate failed.\r\n");
        return FALSE;
    }

    g_printf("Set syslog client certificate successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_syslog_eventsource(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    gint32 ret;
    guint32 mask = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint8 idx = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc < 3) || (argc > 5) || (strcmp(argv[0], "-v") != 0)) {
        print_eventsource_usage();
        return FALSE;
    }

    ret = get_eventsource_value(argc, argv, &idx, &mask);
    if (ret != RET_OK) {
        print_eventsource_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = dal_get_specific_object_byte(CLASS_SYSLOG_ITEM_CONFIG, PROPERTY_REMOTE_SYSLOG_INDEX, idx, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(mask));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_ITEM_CONFIG, obj_handle, METHOD_REMOTE_SYSLOG_SET_LOG_SRC_MASK, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog log type failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog log type successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslog_identity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar str_buff[TRAP_MODE_STR_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    guint32 value = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_syslogidentity_usage();
        return FALSE;
    }

    if (argv[1] == NULL) {
        print_syslogidentity_usage();
        return FALSE;
    }

    (void)memset_s(str_buff, TRAP_MODE_STR_LEN, 0x00, TRAP_MODE_STR_LEN);
    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        if (strcmp(argv[1], "3") == 0) {
            // 3     Host Name
            value = 3;
        } else if (strcmp(argv[1], "4") == 0) {
            // 4     Chassis Name
            value = 4;
        } else if (strcmp(argv[1], "5") == 0) {
            // 5     Chassis Location
            value = 5;
        } else if (strcmp(argv[1], "6") == 0) {
            // 6     Chassis Serial Number
            value = 6;
        } else {
            print_syslogidentity_usage();
            return FALSE;
        }
    } else {
        if (strcmp(argv[1], "1") == 0) {
            // 1     Board Serial number
            value = 1;
        } else if (strcmp(argv[1], "2") == 0) {
            // 2     Product Asset Tag
            value = 2;
        } else if (strcmp(argv[1], "3") == 0) {
            // 3     Host Name
            value = 3;
        } else {
            print_syslogidentity_usage();
            return FALSE;
        }
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(value));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_SET_MSG_IDENTITY, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog identity failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog identity successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslog_protocol(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar str_buff[TRAP_MODE_STR_LEN] = {0};
    gint32 ret;
    GSList *input_list = NULL;
    guint8 value = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_syslogprotocol_usage();
        return FALSE;
    }

    if (argv[1] == NULL) {
        print_syslogprotocol_usage();
        return FALSE;
    }

    (void)memset_s(str_buff, TRAP_MODE_STR_LEN, 0x00, TRAP_MODE_STR_LEN);

    if (strcmp(argv[1], "1") == 0) {
        // 1     UDP
        value = 1;
    } else if (strcmp(argv[1], "2") == 0) {
        // 2     TCP
        value = 2;
    } else if (strcmp(argv[1], "3") == 0) {
        // 3     TLS
        value = 3;
    } else {
        print_syslogprotocol_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(value));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_SET_NET_PROTOCOL, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog protocol failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog protocol successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslog_rootcertificate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    gchar path_info[MAX_FILEPATH_LENGTH + 1] = {0};
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = get_certificate_filepath(argc, argv, path_info, sizeof(path_info));
    if (ret != RET_OK) {
        print_syslog_rootcertificate_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(SYSLOG_CERT_TYPE_HOST));
    input_list = g_slist_append(input_list, g_variant_new_string(path_info));
    input_list = g_slist_append(input_list, g_variant_new_string("NULL"));
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_SYSLOG_CONFIG,
        OBJECT_SYSLOG_CONFIG, METHOD_SYSLOG_MGNT_IMPORT_CERT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set syslog root certificate failed.\r\n");
        debug_log(DLOG_ERROR, "%s: uip_call_class_method_with_handle failed,ret:0x%x \r\n", __FUNCTION__, ret);
        return FALSE;
    }

    g_printf("Set syslog root certificate successfully.\r\n");
    return TRUE;
}


gint32 ipmc_set_syslog_severity(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    GSList *input_list = NULL;
    gint32 ret;
    guint32 mask = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    ret = argc < 2 || argc > 5 || strcmp(argv[0], "-v");
    if (ret) {
        print_syslogseverity_usage();
        return FALSE;
    }

    ret = get_syslog_severity_value(argc, argv, &mask);
    if (ret != RET_OK) {
        print_syslogseverity_usage();
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

    input_list = g_slist_append(input_list, g_variant_new_uint32(mask));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_CONFIG, obj_handle, METHOD_SYSLOG_MGNT_SET_MSG_SEVE_MASK, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog severity failed.\r\n");
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog severity successfully.\r\n");
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslogdestaddress(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 dest;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar str_type = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 3) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_syslog_dest_address();
        return FALSE;
    }

    dest = str_to_int(argv[1]);
    if ((dest < 1) || (dest > 4) || (strlen(argv[1]) != 1)) {
        print_syslog_dest_address();
        return FALSE;
    }

    
    if (strlen(argv[2]) != 0) {
        ret = dal_get_addr_type((const gchar *)argv[2], &str_type);
        if (ret != RET_OK) {
            g_printf("Input parameter format invalid.\r\n");
            
            print_syslog_dest_address();
            
            return FALSE;
        }

        if (str_type == HOST_ADDR_TYPE_IPV4) {
            ret = vos_ipv4_addr_valid_check((const guchar *)argv[2]);
        } else if (str_type == HOST_ADDR_TYPE_IPV6) {
            ret = vos_ipv6_addr_valid_check((const guchar *)argv[2]);
        } else {
            ret = RET_OK;
        }

        if (ret != RET_OK) {
            g_printf("Input parameter format invalid.\r\n");
            
            print_syslog_dest_address();
            
            return FALSE;
        }
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_specific_object_byte(CLASS_SYSLOG_ITEM_CONFIG, PROPERTY_REMOTE_SYSLOG_INDEX, (guint8)(dest - 1),
        &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(argv[2]));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_ITEM_CONFIG, obj_handle, METHOD_REMOTE_SYSLOG_SET_DEST_ADDR, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog dest%d address failed.\r\n", dest);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog dest%d address successfully.\r\n", dest);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslogenable(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gchar str_state[TRAP_STATE_LEN] = {0};
    gint32 dest = 0;
    guchar l_uiValue = 0;
    guint32 ret;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if (((argc != 2) && (argc != 3)) || ((argv[0]) && strcmp(argv[0], "-v"))) {
        print_syslog_dest_state();
        return FALSE;
    }

    if (argv[1] == NULL) {
        print_syslog_dest_state();
        return FALSE;
    }
    
    if (argc == 2) {
        
        (void)strncpy_s(str_state, TRAP_STATE_LEN, argv[1], TRAP_STATE_LEN - 1);
        
    } else {
        dest = str_to_int(argv[1]);
        if ((dest > 4) || (dest < 1) || (strlen(argv[1]) != 1)) {
            print_syslog_dest_state();
            return FALSE;
        }
        
        (void)strncpy_s(str_state, TRAP_STATE_LEN, argv[2], TRAP_STATE_LEN - 1);
        
    }

    if (strcmp(str_state, "disabled") == 0) {
        l_uiValue = 0;
    } else if (strcmp(str_state, "enabled") == 0) {
        l_uiValue = 1;
    } else {
        print_syslog_dest_state();
        return FALSE;
    }

    if (argc == 2) {
        ret = set_syslog_state(l_uiValue, str_state);
    } else {
        ret = set_syslog_dest_state((guchar)(dest), l_uiValue, str_state);
    }

    return ret;
}


gint32 ipmc_set_syslogport(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *input_list = NULL;
    gint32 dest;
    gint32 port;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 3) || (strcmp(argv[0], "-v"))) {
        print_syslog_dest_port();
        return FALSE;
    }
    if (((argv[1] != NULL) && (index(argv[1], '.') != NULL)) || ((argv[2] != NULL) && (index(argv[2], '.') != NULL))) {
        print_syslog_dest_port();
        return FALSE;
    }
    
    dest = str_to_int(argv[1]);
    if ((dest < 1) || (dest > 4) || (strlen(argv[1]) != 1)) {
        
        print_syslog_dest_port();
        
        return FALSE;
    }

    port = str_to_int(argv[2]);
    if ((port <= 0) || (port > 65535)) {
        
        print_syslog_dest_port();
        
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_specific_object_byte(CLASS_SYSLOG_ITEM_CONFIG, PROPERTY_REMOTE_SYSLOG_INDEX, (guint8)(dest - 1),
        &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)port));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_ITEM_CONFIG, obj_handle, METHOD_REMOTE_SYSLOG_SET_DEST_PORT, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Set syslog dest%d port failed.\r\n", dest);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Set syslog dest%d port successfully.\r\n", dest);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


gint32 ipmc_set_syslogtest(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 dest;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        print_syslog_dest_test();
        return FALSE;
    }
    if ((argv[1] != NULL) && (index(argv[1], '.') != NULL)) {
        print_syslog_dest_test();
        return FALSE;
    }
    
    dest = str_to_int(argv[1]);
    if ((dest < 1) || (dest > 4) || (strlen(argv[1]) != 1)) {
        
        print_syslog_dest_test();
        
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    if (dal_get_specific_object_byte(CLASS_SYSLOG_ITEM_CONFIG, PROPERTY_REMOTE_SYSLOG_INDEX, (guint8)(dest - 1),
        &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(1));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_SYSLOG_ITEM_CONFIG, obj_handle, METHOD_REMOTE_SYSLOG_SET_TEST_TRIGGER, input_list, NULL);
    if (ret != RET_OK) {
        g_printf("Test syslog dest%d failed.\r\n", dest);
        uip_free_gvariant_list(input_list);
        return FALSE;
    }

    g_printf("Test syslog dest%d successfully.\r\n", dest);
    uip_free_gvariant_list(input_list);
    return TRUE;
}


#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V4
gint32 ipmcset_clear_log(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 operate_index;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    // 权限判断
    if (setcommand_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    // 参数判断
    if ((argc != 2) || (strcmp(argv[0], "-v") != 0)) {
        print_clear_log_usage();
        return FALSE;
    }

    operate_index = str_to_int(argv[1]);
    if ((operate_index < CLEAR_OPERATION_LOG) || (operate_index > CLEAR_SECURITY_LOG)) {
        print_clear_log_usage();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    ret = dal_get_object_handle_nth(CLASS_LOG, 0, &obj_handle);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = is_continue_operate();
    if (ret != RET_OK) {
        return FALSE;
    }

    if (operate_index == CLEAR_OPERATION_LOG) {
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOG,
            obj_handle, METHOD_CLEAR_OPERATION_LOG, NULL, NULL);
        if (ret != RET_OK) {
            g_printf("Clear Operation log failed.\r\n");
            return FALSE;
        }

        g_printf("Clear Operation log successfully.\r\n");
    } else if (operate_index == CLEAR_RUN_LOG) {
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOG,
            obj_handle, METHOD_CLEAR_RUN_LOG, NULL, NULL);
        if (ret != RET_OK) {
            g_printf("Clear Run log failed.\r\n");
            return FALSE;
        }

        g_printf("Clear Run log successfully.\r\n");
    } else {
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_LOG,
            obj_handle, METHOD_CLEAR_SECURITY_LOG, NULL, NULL);
        if (ret != RET_OK) {
            g_printf("Clear Security log failed.\r\n");
            return FALSE;
        }

        g_printf("Clear Security log successfully.\r\n");
    }

    return TRUE;
}
#endif