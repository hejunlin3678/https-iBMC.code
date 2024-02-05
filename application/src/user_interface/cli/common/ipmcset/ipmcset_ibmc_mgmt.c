

#include "ipmcset_ibmc_mgmt.h"
#include "ipmcset_sys_mgmt.h"
#include "ipmcset_alarm.h"

#define MAX_SLOT_COUNT 32

#define NOT_SUPPORT_SELFHEALIN 0xff
#define IDLE_STATE 0
#define FPGA_IS_SELFHEALING 1
#define STATE_NOT_SUPPORT_SELFHEALIN 2
#define SELFHEALIN_FAILED 3
#define FPGA_SELFHEALING_COMPLETED 100
#define MAX_SLOT_COUNT 32

LOCAL gint32 get_fpga_golden_fw_restore_data_from_mcu(guint8 *retult_status, guint8 *retult_percent, guint8 slot_id);


gint32 set_fpga_golden_fw_restore(guint8 slot_id, guint32 position, gint32 *todo_flg)
{
    GSList *output_list = NULL;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret;

    if (slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter\n", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_MAINBOARD_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get objlist for CardAction fail!!!");
        return ret;
    }

    // 入参链表数据封装
    input_list = g_slist_append(input_list, g_variant_new_byte(slot_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(position));
    ret = dfl_call_class_method(obj_handle, METHOD_MAINBOARD_RESTORE_FPGA_CARD, NULL, input_list, &output_list);
    if (ret == DFL_OK) {
        *todo_flg = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
    }

    if (input_list) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }
    if (output_list) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    return ret;
}


gint32 get_fpga_golden_fw_restore_result(guint8 slot_id)
{
    gint32 ret;
    gint32 i;
    guint8 result_status = 0;
    guint8 result_percent = 0;

    
    for (i = 0; i < 2 * LOAD_XML_WAIT_OBJ_TIME; i++) {
        (void)vos_task_delay(CHECK_OBJ_PERIOD);
        (void)get_fpga_golden_fw_restore_data_from_mcu(&result_status, &result_percent, slot_id);
        if (result_status != IDLE_STATE) {
            break;
        }
    }
    

    if (result_status == NOT_SUPPORT_SELFHEALIN) {
        g_printf("The device does not support Golden firmware restore.\r\n");
        ret = RET_ERR;
    } else if (result_status == FPGA_IS_SELFHEALING) {
        g_printf("The restore of the Golden firmware of the FPGA card is starting.\r\n");
        (void)fflush(stdout);

        while (1) {
            ret = get_fpga_golden_fw_restore_data_from_mcu(&result_status, &result_percent, slot_id);
            if (ret != RET_OK) {
                g_printf("Failed to restore the Golden firmware of the FPGA card.\r\n");
                return RET_ERR;
            }

            if (!(result_status == FPGA_IS_SELFHEALING && result_percent != FPGA_SELFHEALING_COMPLETED)) {
                break;
            }

            g_printf("\r%d%%", result_percent);
            (void)fflush(stdout);

            
            (void)vos_task_delay(5 * 1000);
        }

        if (result_percent == FPGA_SELFHEALING_COMPLETED) {
            g_printf("\r%d%%", result_percent);
            (void)fflush(stdout);
            g_printf("\nThe restore of the Golden firmware of the FPGA card is successful.\r\n");
            ret = RET_OK;
        } else {
            g_printf("\nThe restore of the Golden firmware of the FPGA card is failed.\r\n");
            ret = RET_ERR;
        }
    } else if (result_status == STATE_NOT_SUPPORT_SELFHEALIN) {
        g_printf("The current state of the FPGA card does not support Golden firmware restore.\r\n");
        ret = RET_ERR;
    } else if (result_status == SELFHEALIN_FAILED) {
        g_printf("Failed to restore the Golden firmware of the FPGA card.\r\n");
        ret = RET_ERR;
    } else {
        g_printf("Unknown error.\r\n");
        ret = RET_ERR;
    }
    return ret;
}


LOCAL gint32 get_fpga_golden_fw_restore_data_from_mcu(guint8 *retult_status, guint8 *retult_percent, guint8 slot_id)
{
    GSList *output_list = NULL;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret;

    if (slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s: Invalid parameter\n", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_MAINBOARD_NAME, 0, &obj_handle);
    if ((ret != RET_OK)) {
        debug_log(DLOG_ERROR, "get objlist for CardAction failed");
        return ret;
    }

    // 入参链表数据封装
    input_list = g_slist_append(input_list, g_variant_new_byte(slot_id));
    ret = dfl_call_class_method(obj_handle, METHOD_MAINBOARD_GET_FPGA_RESTORE_STATUS, NULL, input_list, &output_list);
    if (ret == DFL_OK) {
        *retult_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        *retult_percent = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
    }

    if (input_list) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }
    if (output_list) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    return ret;
}

gint32 maintenance_upgrade_cpld(gchar *path_info, guint32 len)
{
    GVariant *value = NULL;
    gint32 ret;
    guint8 upgrade_flag;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (len == 0) {
        g_printf("File path length is 0.\r\n");
        return FALSE;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    
    ret = uip_get_object_property(PFN_CLASS_NAME, OBJECT_PFN, PFN_UPGRADE_INFLAG_NAME, &value);
    if (ret != RET_OK) {
        g_printf("Get upgrade flag failed.\r\n");
        ipmc_log_operation_log("Maintenance", "Upgrade CPLD failed");
        return FALSE;
    }

    upgrade_flag = g_variant_get_byte(value);
    g_variant_unref(value);
    if (upgrade_flag != 0) {
        g_printf("An upgrade operation is in progress, please wait.\r\n");
        ipmc_log_operation_log("Maintenance", "Upgrade CPLD failed");
        return FALSE;
    }

    g_printf("Please make sure the %s is working while upgrading.\r\n", custom_name);
    ret = send_update_command(path_info, FORCE_UPGRADE_CPLD, NO_RESET_FLAG);
    if (ret != RET_OK) {
        g_printf("Upgrade failed.\r\n");
        (void)fflush(stdout);
        return FALSE;
    }

    (void)fflush(stdout);
    return TRUE;
}



gint32 check_is_digint(gchar *ptr, guint8 len)
{
    guint8 iindex;

    if ((len == 0) || (ptr == NULL)) {
        return 1;
    }

    for (iindex = 0; iindex < len; iindex++) {
        if (!(isdigit(ptr[iindex]))) {
            return 1;
        }
    }

    return 0;
}


gint32 check_time_data(guint16 Year, guint8 Month, guint8 Date, guint8 Hour, guint8 Minute, guint Second)
{
    // 判断时间范围是否正确
    if ((Hour > MAX_HOUR_DAY) || (Minute > MAX_MINUTE) || (Second > MAX_SECOND) ||
        (!((Date >= MONTH_DATE_MIN) && (Date <= MONTH_DATE_MAX))) ||
        (!((Month >= MONTH_JAN) && (Month <= MONTH_DEC)))) {
        g_printf("The input time is incorrect.\r\n");
        return TIME_FORMAT_ERR;
    }

    // 判断4,6,9,11月的天数是否正确
    if ((Month == MONTH_APIR) || (Month == MONTH_JUNE) || (Month == MONTH_SEP) || (Month == MONTH_NOV)) {
        if (Date > MONTH_DATE_30) {
            g_printf("The input time is incorrect.\r\n");
            return TIME_FORMAT_ERR;
        }
        return RET_OK;
    }
    // 判断2月份天数是否正确
    else if (Month == MONTH_FEB) {
        if (year_is_leap(Year) == TRUE) {
            if (Date > LEAP_MONTH_DAY) {
                g_printf("The input time is incorrect.\r\n");
                return TIME_FORMAT_ERR;
            }
            return RET_OK;
        } else {
            if (Date > MONTH_DATE_28) {
                g_printf("The input time is incorrect.\r\n");
                return TIME_FORMAT_ERR;
            }
            return RET_OK;
        }
    }

    return RET_OK;
}


gint32 year_is_leap(guint16 Year)
{
    return ((((Year) % YEAR_LEAP_4) == 0) && ((((Year) % YEAR_LEAP_100) != 0) || (((Year) % YEAR_LEAP_400) == 0)));
}


gint32 set_on_board_rtc_time(guint32 timestamp)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *handle_list = NULL;
    OBJ_HANDLE handle;

    ret = dfl_get_object_list(CLASS_ON_BOARD_RTC, &handle_list);
    if (ret != DFL_OK) {
        return ret;
    }

    handle = (OBJ_HANDLE)(handle_list->data);
    g_slist_free(handle_list);

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(timestamp));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_ON_BOARD_RTC,
        dfl_get_object_name(handle), METHOD_SET_ON_BOARD_RTC_TIME, input_list, NULL);

    uip_free_gvariant_list(input_list);

    return ret;
}


gint32 ipmc_set_timezone(const gchar *time_zone)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(time_zone));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_NAME_BMC,
        OBJ_NAME_BMC, METHOD_BMC_TIMEZONE_STR, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        if (ret == 0x0f) {
            g_printf("Illegal time zone, please set a legal time zone.\r\n");
        } else {
            g_printf("Set time zone failed.\r\n");
        }

        return FALSE;
    }

    return TRUE;
}


void constr_area_city_to_timezone(const gchar *area, const gchar *city, gchar *tz, guint32 tz_sz)
{
    errno_t safe_fun_ret;
    if (area == NULL || city == NULL || tz == NULL || tz_sz == 0) {
        return;
    }

    memset_s(tz, tz_sz, 0, tz_sz);
    safe_fun_ret = strncat_s(tz, tz_sz - 1, area, strlen(area));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = strncat_s(tz, tz_sz - 1, "/", 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    safe_fun_ret = strncat_s(tz, tz_sz - 1, city, strlen(city));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
}


guchar check_usb_mgmt_support_state(const gchar *print_str)
{
    OBJ_HANDLE usb_mgmt_obj = 0;
    guchar is_present = 0;
    gint32 ret;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    ret = dfl_get_object_handle(OBJ_USB_MGMT, &usb_mgmt_obj);
    if (ret != DFL_OK) {
        g_printf("%s", print_str);
        return FALSE;
    }

    ret = dal_get_property_value_byte(usb_mgmt_obj, PROPERTY_USB_MGMT_PRESENCE, &is_present);
    if (ret != DFL_OK) {
        g_printf("%s", print_str);
        return FALSE;
    }

    if (is_present == 0) {
        g_printf("The mounting ear with %s directly connected management interface is not present.\r\n", custom_name);
        return FALSE;
    }

    return TRUE;
}


gint32 get_update_filepath(guchar argc, gchar **argv, gchar *update_file, guint32 len, guint8 *reset_flag)
{
    errno_t safe_fun_ret;
    guint8 num;
    gchar *real_path = NULL;

    if (update_file == NULL || len == 0) {
        return RET_ERR;
    }

    
    
    if (argc < 2) { 
        
        return RET_ERR;
    }

    
    if (argv == NULL) {
        return RET_ERR;
    }
    for (num = 0; num < 2; ++num) {
        if (argv[num] == NULL) {
            return RET_ERR;
        }
    }

    if (strncmp(argv[0], "-v", strlen("-v") + 1)) {
        return RET_ERR;
    }

    if (strlen(argv[1]) > MAX_FILEPATH_LENGTH - 1 || strncmp(argv[1], "/", strlen("/"))) {
        return RET_ERR;
    }

    
    real_path = realpath(argv[1], NULL);
    if (real_path == NULL) {
        
        return errno;
        
    }

    if (strncmp(real_path, "/tmp", strlen("/tmp"))) {
        g_free(real_path);
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(update_file, len, real_path, len - 1);
    g_free(real_path);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    
    *reset_flag = get_upgrade_bmc_reset_flag(argc, argv);
    if ((*reset_flag != REBOOT_TO_VALID_BMC) && (*reset_flag != WAIT_FOR_MANUAL_REBOOT)) {
        return RET_ERR;
    }
    return RET_OK;
}


guint8 get_upgrade_bmc_reset_flag(guchar argc, gchar **argv)
{
#define CLI_WITH_NO_RESET_PARA 2
#define CLI_WITH_RESET_PARA 3
#define CLI_RESET_INDEX 2
#define CLI_VALID_PARA 0

    guint8 reset_value = REBOOT_TO_VALID_BMC;  
    gint32 reset_para = REBOOT_TO_VALID_BMC;

    if (dal_check_if_mm_board() == TRUE) {
        return WAIT_FOR_MANUAL_REBOOT;
    }

    if (argc == CLI_WITH_RESET_PARA) {
        
        reset_para = str_to_int(argv[CLI_RESET_INDEX]);
        if (reset_para < CLI_VALID_PARA) {
            return RET_ERR;
        }

        reset_value = (guint8)reset_para;
        // 参数不对的话，无法升级
        if (reset_value != REBOOT_TO_VALID_BMC && reset_value != WAIT_FOR_MANUAL_REBOOT) {
            return RET_ERR;
        }
    }

    return reset_value;
}
