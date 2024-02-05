

#include "ipmcset_sys_mgmt.h"

LOCAL gint32 send_active_mode(guint8 reset_flag);
LOCAL void __print_upgrade_result_string(guchar component_id, guint8 reset_flag);
LOCAL void __print_upgrade_failed_suggest_message(guchar component_id);
LOCAL void __print_psu_upgrade_failed_suggest_message(void);
LOCAL void print_psu_upgrade_failed_suggest_message_for_canbus(void);

typedef struct upgrade_error_dispalyinfo_tag {
    gint32 error_code;
    const gchar *str_val;
    gint32 return_code;
} UPGRADE_ERROR_DISPLAYINFO_S;

typedef struct upgrade_info_tag {
    guchar process;
    guchar mode;
    guchar compentid;
    gint32 err_code;
} UPGRADE_INFO_S;

#define WHITEBRAND_VER_MISMATCH_STR "The version of the update pack does not match the current web interface version."
LOCAL UPGRADE_ERROR_DISPLAYINFO_S g_upgrade_error_info[] = {
    {UP_COMM_FILE_NOT_EXIST, "The file does not exist.", RET_ERR},
    {UP_COMM_FILE_ERROR, "Invalid upgrade file.", RET_ERR},
    {UP_COMM_LITTLE_MEM, "Not enough memory.", RET_ERR},
    {UP_COMM_POWER_TATUS_ERR, "Please power off OS first, and then upgrade BIOS again.", RET_ERR},
    {UP_COMM_FIRMWARE_FILE_MISMATCH, "The upgrade file does not match the device to be upgraded.", RET_ERR},
    {UP_COMM_FIRMWARE_ERR_WRONG_ME_STATUS, "The BIOS upgrade fails because the ME status is not correct.", RET_ERR},
    {UP_COMM_FIRMWARE_UNKNOWN_ERR, "Unknown error.", RET_ERR},
    {UP_COMM_FIRMWARE_ERR_RETRY, "Powering on... Failed to upgrade the firmware.", RET_ERR},
    {UP_COMM_FIRMWARE_ERR_UPGRADE_FAILED, "", RET_ERR},
    {UP_COMM_WHITEBRAND_VER_MISMATCH, WHITEBRAND_VER_MISMATCH_STR, RET_ERR},
    {UP_COMM_FIRMWARE_EARLY_ERR, "The upgrade package version is too early.", RET_ERR}
};

#define UP_LOCAL_SUCCESS_ERR_STR "The local EM upgrade succeeded. The remote EM upgrade failed in the extra operation."
LOCAL UPGRADE_ERROR_DISPLAYINFO_S g_upgrade_error_detail_info[] = {
    {UP_COMM_MAIN_MAX, "Unknown error.", RET_ERR},
    {UP_SHELF_FAN_PARTLY_OK, "", RET_OK},
    {UP_SHELF_FAN_ERR, "EM upgrade frame fan failed.", RET_ERR},
    {UP_EM_EXTRA_CHECK_FAILED, "Additional checks before EM upgrade failed.", RET_ERR},
    {UP_LOCAL_SUCCESS_EXTRA_ERR, UP_LOCAL_SUCCESS_ERR_STR, RET_ERR},
    {UP_COMM_STANDBY_FORBIDDEN_ERR, "Currently is a standby board, no upgrades are allowed.", RET_ERR},
    {UP_COMM_REMOTE_POWER_TATUS_ERR, "Remote power state error.", RET_ERR},
    {UP_COMM_FIRMWARE_VER_MISMATCH, "Firmware version does not match.", RET_ERR}
};

LOCAL UPGRADE_ERROR_DISPLAYINFO_S* __match_upgrade_error_displayinfo(gint32 errorcode,
    UPGRADE_ERROR_DISPLAYINFO_S* displayinfo_array, gint32 len)
{
    gint32 i;
    UPGRADE_ERROR_DISPLAYINFO_S* displayinfo_ptr = NULL;

    for(i = 0; i < len; i++) {
        displayinfo_ptr = (displayinfo_array + i);
        if (errorcode == displayinfo_ptr->error_code) {
            return displayinfo_ptr;
        }
    }

    return NULL;
}

LOCAL gint32 __print_upgrade_error_info(gint32 errorcode, UPGRADE_ERROR_DISPLAYINFO_S* displayinfo_array, gint32 len,
    gint32 *return_code)
{
    UPGRADE_ERROR_DISPLAYINFO_S* displayinfo_ptr = NULL;
    displayinfo_ptr = __match_upgrade_error_displayinfo(errorcode, displayinfo_array, len);
    if (displayinfo_ptr == NULL) {
        return RET_OK;
    }

    if (strlen(displayinfo_ptr->str_val) == 0) {
        g_printf("\r\n");
    } else {
        g_printf("%s\r\n", displayinfo_ptr->str_val);
    }
    *return_code = displayinfo_ptr->return_code;
    return RET_ERR;
}

gint32 set_ascend_mode(OBJ_HANDLE object_handle, const gchar *username, const gchar *ip, guint8 ascend_mode)
{
    guint8 ret;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(ascend_mode));

    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_NPU, object_handle,
        METHOD_SET_WORK_MODE, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call %s fail ret:%d", METHOD_SET_WORK_MODE, ret);
        return RET_ERR;
    }

    return RET_OK;
}


void set_autodiscovery(guchar state, gchar *netsegment, guint32 option_val, guint32 port)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    OBJ_HANDLE obj_handle;
    GSList *obj_list = NULL;

     
    ret = dfl_get_object_list(CLASS_STATELESS, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Get object failed.\n");
        return;
    }

    
    obj_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "$Supervisor");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "IPC");
        
    }

    // 1 state
    input_list = g_slist_append(input_list, g_variant_new_byte(state));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_STATELESS, obj_handle, METHOD_STATELESS_SET_ENABLE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    if (ret != RET_OK) {
        g_printf("Set state to (%s) failed.\r\n", (state == TRUE) ? "enable" : "disable");
        return;
    }

    g_printf("Set state to (%s) successfully.\r\n", (state == TRUE) ? "enable" : "disable");

    // 2 netsegment
    if ((netsegment == NULL) || (netsegment[0] == 0)) {
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(netsegment));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_STATELESS, obj_handle, METHOD_STATELESS_SET_BROADCAST_NET_SEGMENT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    if (ret != RET_OK) {
        g_printf("Set broadcast to (%s) failed.\r\n", netsegment);
        return;
    }

    g_printf("Set broadcast to (%s) successfully.\r\n", netsegment);

    // 3 port
    if (port == 0) {
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32(port));
    ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_STATELESS, obj_handle, METHOD_STATELESS_SET_BROADCAST_NET_PORT, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set netport to (%d) failed.\r\n", port);
        return;
    }

    g_printf("Set netport to (%d) successfully.\r\n", port);
}


gint32 ipmc_get_resetiME_support(void)
{
    gint32 retv;
    OBJ_HANDLE obj_handle = 0;
    gchar  accessor_name[ACCESSOR_NAME_LEN] = { 0 };

    retv = dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);
    if (retv == RET_OK) {
        retv = dfl_get_property_accessor(obj_handle, PROPERTY_MEINFO_RESET_IME, accessor_name, ACCESSOR_NAME_LEN);
    }

    return retv;
}
LOCAL gint32 __send_active_mode_and_call_upgrade_method(gchar *path, guint8 forced_upgrade, guint8 reset_flag)
{
    GSList *input_list = NULL;
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    
    ret = send_active_mode(reset_flag);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    
    input_list = g_slist_append(input_list, g_variant_new_string(path));
    input_list = g_slist_append(input_list, g_variant_new_byte(forced_upgrade));
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, PFN_CLASS_NAME,
        OBJECT_PFN, METHOD_PFN_INITIATE_UP, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Initiate upgrade failed.\r\n");
    }

    return ret;
}


LOCAL void wait_upgrade_finish(void)
{
    OBJ_HANDLE upgrade_handle = 0;
    guint8 upgrade_flag = INVALID_DATA_BYTE;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_UPGRADE, 0, &upgrade_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle(%s) failed.", __FUNCTION__, CLASS_NAME_UPGRADE);
        return;
    }

    // 最多等待20秒（循环40次）
    for (guint32 i = 0; i < 40; i++) {
        (void)dal_get_property_value_byte(upgrade_handle, PFN_UPGRADE_INFLAG_NAME, &upgrade_flag);
        if (upgrade_flag == FALSE) {
            return;
        }
        // 500ms查询一次
        vos_task_delay(500);
    }
    debug_log(DLOG_ERROR, "Wait for upgrade progress finish(20 seconds) time out");
}

LOCAL gint32 __handle_upgrade_process_info(guchar compentid, guchar mode, guint8 reset_flag, guchar process,
    guchar *processflag)
{
    if (mode == UPDATE_MODE_PROCESS) {
        if (*processflag == 0) {
            *processflag = 1;

            
            if (compentid == COMP_POWER) {
                g_printf("During the upgrade of the PSUs, do not remove or install the PSUs or power off the "
                         "server.\r\n");
            }
            

            g_printf("Updating...\r\n");

            if (compentid == BIOSCOMPNENTID) {
                g_printf("System needs two minutes time to prepare.\r\n");
            }

            (void)fflush(stdout);
        } else {
            g_printf("\r%d%%", process);
            (void)fflush(stdout);
        }
    } else if (mode == UPDATE_MODE_ACTIVITY) {
        g_printf("\r100%%\r\n");  // 激活的时候打印进度100%

        wait_upgrade_finish(); // 升级结束等待升级完成（升级标志清除）
        
        __print_upgrade_result_string(compentid, reset_flag);
        

        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __get_upgrade_info(OBJ_HANDLE handle, UPGRADE_INFO_S *upgrade_info, gchar *detail_result, gint32 len)
{
    guint32 ret;
    ret = (guint32)dal_get_property_value_byte(handle, PFN_UPGRADE_STATE_NAME, &upgrade_info->process);
    ret |= (guint32)dal_get_property_value_byte(handle, PFN_UPGRADE_MODE_NAME, &upgrade_info->mode);
    ret |= (guint32)dal_get_property_value_byte(handle, PFN_COMP_ID_NAME, &upgrade_info->compentid);
    ret |= (guint32)dal_get_property_value_int32(handle, PFN_ERR_CODE_NAME, &upgrade_info->err_code);
    ret |= (guint32)dal_get_property_value_string(handle, PFN_UPGRADE_DETAILED_RESULTS, detail_result, len);
    if (ret == RET_OK) {
        return RET_OK;
    }
    return RET_ERR;
}

gint32 send_update_command(gchar *path, guint8 forced_upgrade, guint8 reset_flag)
{
    gint32 ret;
    UPGRADE_INFO_S upgrade_info;
    OBJ_HANDLE handle = 0;
    guchar processflag = 0;
    gchar detail_res[UP_DETAIL_RESULT_MAX_LEN + 1] = {0};

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = __send_active_mode_and_call_upgrade_method(path, forced_upgrade, reset_flag);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    for (;;) {
        memset_s(detail_res, sizeof(detail_res), 0, sizeof(detail_res));
        
        ret = __get_upgrade_info(handle, &upgrade_info, &detail_res[0], sizeof(detail_res));
        if (ret != RET_OK) {
            g_printf("Get upgrade state failed.\r\n");
            return RET_ERR;
        }

        if (upgrade_info.err_code != 0) {
            gint32 ret_code = 0;
            gint32 array_len = sizeof(g_upgrade_error_info) / sizeof(g_upgrade_error_info[0]);
            ret = __print_upgrade_error_info(upgrade_info.err_code, g_upgrade_error_info, array_len, &ret_code);
            if (ret == RET_ERR) {
                return ret_code;
            }

            
            if (strlen(detail_res) != 0) {
                g_printf("\n%s\r\n", detail_res);

                
                __print_upgrade_failed_suggest_message(upgrade_info.compentid);
                
                
                ret = (upgrade_info.err_code == UP_SHELF_FAN_PARTLY_OK) ? RET_OK : RET_ERR;
                
                return ret;
            }

            
            array_len = sizeof(g_upgrade_error_detail_info) / sizeof(g_upgrade_error_detail_info[0]);
            ret = __print_upgrade_error_info(upgrade_info.err_code, g_upgrade_error_detail_info, array_len, &ret_code);
            if (ret == RET_ERR) {
                return ret_code;
            }

            g_printf("Error code : 0x%x\r\n", upgrade_info.err_code);
            return RET_ERR;
        }

        ret = __handle_upgrade_process_info(upgrade_info.compentid, upgrade_info.mode, reset_flag,
            upgrade_info.process, &processflag);
        if (ret != RET_OK) {
            break;
        }

        
        (void)vos_task_delay(DELAY_TIME);
    }

    return RET_OK;
}


LOCAL gint32 send_active_mode(guint8 reset_flag)
{
    gint32 ret;
    GSList *input_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (reset_flag == NO_RESET_FLAG) {
        return RET_OK;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(reset_flag));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, PFN_CLASS_NAME,
        OBJECT_PFN, METHOD_SET_BMC_ACTIVE_MODE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        g_printf("Set bmc active failed.\r\n");
        return RET_ERR;
    }

    return RET_OK;
}

guint8 get_bmc_valid_mode(void)
{
    guint8 bmc_valid_mode = BMC_AUTO_VALID;

    (void)dal_get_func_ability(CLASS_NAME_PME_PRODUCT, PROTERY_PRODUCT_BMC_VALID_MODE, &bmc_valid_mode);

    
#ifdef DFT_ENABLED
#ifdef ARM64_HI1711_ENABLED
    guint8 software_type = 0xff;
    if (dal_get_software_type(&software_type) == RET_OK) {
        if (software_type == MGMT_SOFTWARE_TYPE_RM || software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
            bmc_valid_mode = BMC_MANUAL_VALID;
        }
    }
#endif
#endif

    return bmc_valid_mode;
}


LOCAL void __print_upgrade_result_string(guchar component_id, guint8 reset_flag)
{
    OBJ_HANDLE obj_handle = 0;
    guint32 product_id = 0;
    guchar cache_file_status;
    guint8 ibmc_valid_type;
    GSList *output_list_status = NULL;

    switch (component_id) {
        case COMP_CPLD_BY_I2C: {
            (void)dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &obj_handle);
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_PRODUCT_ID, &product_id);
            if ((product_id == PRODUCT_ID_ATLAS) && (component_id == COMP_CPLD_BY_I2C)) {
                g_printf("The upgrade is successful. Power off the related node for the upgrade to take effect.\r\n");
            } else {
                g_printf("Upgrade successfully.\r\n");
            }
            break;
        }
        case COMP_BIOS: {
            (void)dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
            (void)dfl_call_class_method(obj_handle, METHOD_PFN_GET_CACHE_UPGRADE_FILE_STATUS, NULL, NULL,
                &output_list_status);
            cache_file_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list_status, 0));
            g_slist_free_full(output_list_status, (GDestroyNotify)g_variant_unref);

            if (cache_file_status == 0) {
                g_printf("Upgrade successfully.\r\n");
            } else {
                g_printf(
                    "Upload upgrade file successfully and take effect when OS is power-off or reseted next time.\r\n");
            }
            break;
        }
        case COMP_IPMC_EXT4: {
            ibmc_valid_type = get_bmc_valid_mode();
            if (ibmc_valid_type == BMC_MANUAL_VALID && reset_flag == BMC_MANUAL_VALID) {
                g_printf("Upgrade successfully and need to reboot the %s to active the upgrade\r\n", product_type_name);
            } else {
                g_printf("Upgrade successfully.\r\n");
            }
            break;
        }
        default:
            g_printf("Upgrade successfully.\r\n");
            break;
    }
    (void)fflush(stdout);
    return;
}


LOCAL void __print_upgrade_failed_suggest_message(guchar component_id)
{
    switch (component_id) {
        case COMP_POWER:
            __print_psu_upgrade_failed_suggest_message();
            break;
        default:
            break;
    }

    return;
}


LOCAL void __print_psu_upgrade_failed_suggest_message(void)
{
    guint32 ret;
    guint8 max_ps_num = 0;
    guint8 min_required_ps_num = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 syspower_redundancy_mode = 0;
    OBJ_HANDLE obj_one_power = 0;
    guint8 protocol = PROTOCOL_PMBUS;

    (void)dal_get_object_handle_nth(CLASS_NAME_PS, 0, &obj_one_power);
    (void)dal_get_property_value_byte(obj_one_power, PROTERY_PS_PROTOCAL, &protocol);
    if (protocol == PROTOCOL_CANBUS) {
        print_psu_upgrade_failed_suggest_message_for_canbus();
        (void)fflush(stdout);
        return;
    }

    
    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROTERY_SYSPOWER_REDUNDANCY_MODE, &syspower_redundancy_mode);
        
        if (syspower_redundancy_mode != 0) {
            min_required_ps_num = (syspower_redundancy_mode >> 4) & 0x0F; 
        }
    }
    if (min_required_ps_num == 0) {
        ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &obj_handle);
        if (ret == RET_OK) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &max_ps_num);
            min_required_ps_num = max_ps_num / 2;
        }
    }

    if (dal_check_if_mm_board()) {
        g_printf("Ensure that the following conditions are met: \r\n1. The PSUs support upgrade.\r\n"
            "2. No PSU was removed or installed during the upgrade.\r\n3. All PSUs are present.\r\n"
            "4. At most one PSU is unhealthy.\r\n");
    } else {
        g_printf("Ensure that the following conditions are met or try again after the system is powered off: \r\n"
            "1. The PSUs support upgrade.\r\n2. No PSU was removed or installed during the upgrade.\r\n"
            "3. At least one PSU is working normally when the system is powered off.\r\n");
        
        if ((min_required_ps_num == 0) || (min_required_ps_num == 1)) {
            g_printf("4. At least half PSUs are working normally when system is powered on.\r\n");
        } else {
            g_printf("4. At least %d PSUs are working normally when system is powered on.\r\n", min_required_ps_num);
        }
    }
    (void)fflush(stdout);
    return;
}

LOCAL void print_psu_upgrade_failed_suggest_message_for_canbus(void)
{
    g_printf("Ensure that the following conditions are met or try again later: \r\n\
1. There is no alarm 0x0300000D (The AC/DC input of PSU is lost or out-of-range) or alarm 0x0300003D (\
The ID of PSU conflicts with that of another PSU along the bus).\r\n\
2. Peak power clipping is not triggered in the current rack.\r\n\
3. At least two PSUs are in position.\r\n\
4. The output power of the PSUs in the rack meets the following condition: P0 >= P1 + 1 * P2. \
P0 indicates the sum of the rated output power of all the PSUs. \
P1 indicates the larger value between the total real-time power of the rack and the power cap value set for the rack. \
P2 indicates the rated output power of a PSU.\r\n");

    return;
}


gint32 get_file_path_from_url(const gchar *url, guint8 file_id, gchar *file_path, gint32 len)
{
    gint32 iRet;
    gchar url_path[URL_MAX_LENGTH + 1] = {0};
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gint32 ret;
    gint32 status = 0;
    guint32 i;
    gchar *pstr = NULL;

    
    if (url == NULL || (file_path == NULL) || (len <= 1)) {
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    iRet = snprintf_s(url_path, sizeof(url_path), URL_MAX_LENGTH, "download;%u;%s", file_id, url);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar*)url_path));
    
    input_list = g_slist_append(input_list, g_variant_new_string(" "));
    
    g_printf("Downloading...\r\n");
    
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_TRANSFERFILE, OBJ_NAME_TRANSFERFILE, METHOD_INITRIAL_FILE_TRANSFER, input_list, NULL);

    clear_sensitive_info(url_path, sizeof(url_path));
    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        return FALSE;
    } else {
        (void)vos_task_delay(1000);
    }

    
    for (i = 0; i < 600; i++) {
        (void)vos_task_delay(500);

        ret = uip_call_class_method(NULL, NULL, NULL, CLASS_NAME_TRANSFERFILE, OBJ_NAME_TRANSFERFILE,
            METHOD_GET_TRANSFER_STATUS, NULL, &output_list);
        if (ret != RET_OK) {
            return FALSE;
        }

        status = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));
        if (status < 0) {
            if (status == CURL_PEER_SSL_VERIFY_FAILED) {
                g_printf("\r\n peer's certificate or fingerprint wasn't verified fine.\r\n");
            } else {
                g_printf("\r\n");
            }
            uip_free_gvariant_list(output_list);
            return FALSE;
        }

        uip_free_gvariant_list(output_list);

        g_printf("\r%2d%%", status);
        (void)fflush(stdout);

        
        if (status == 100) {
            g_printf("\r\n");

            
            pstr = strrchr(url, '/');
            iRet = snprintf_s(file_path, len, len - 1, "/tmp%s", pstr);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
            return TRUE;
        }
    }

    g_printf("\r\n");
    return FALSE;
}


gint32 set_collect_configuration(const gchar *file_path)
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

    

    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    ret = ipmc_call_method_user_info(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, BMC_CLASEE_NAME,
        BMC_OBJECT_NAME, METHOD_IMPORT_ALL_CONFIG, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        return FALSE;
    }

    
    g_printf("Setting configuration...\r\n");

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


gint32 set_physical_drive_config_confirm(gint32 is_cryptoerase)
{
    gint32 input;

    if (is_cryptoerase) {
        g_printf("WARNING: If crypto erase is performed, the data in the physical drive cannot be recovered.\r\n");
    }
    g_printf("WARNING: The operation may have many adverse effects.\r\n");
    input = check_the_input_operation(CLI_COMFIRM_STRING);
    return input;
}


void set_ctrl_mode(OBJ_HANDLE obj_handle, guint8 mode, const gchar* username, gchar* ip)
{
    gint32 ret;
    GSList* input_list = NULL;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(mode));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(0xFF)); // profile_id目前不用，固定填0xFF

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar*)username, (const gchar*)ip,
                                CLASS_RAID_CONTROLLER_NAME, dfl_get_object_name(obj_handle),
                                METHOD_RAID_CONTROLLER_SET_MODE, input_list, NULL);
    if (ret == SML_SUCCESS) {
        g_printf("Set RAID controller mode successfully.\r\n");
    } else if (ret == SML_ERR_REBOOT_REQUIRED) {
        g_printf("Set RAID controller mode successfully, please reboot the OS to take effect.\r\n");
    } else if (ret == SML_ERR_OPERATION_NOT_POSSIBLE) {
        g_printf("Set RAID controller mode failed, the operation is not possible at this time.\r\n");
    } else if (ret == SML_ERR_CONFIG_PRESENT_ERROR) {
        g_printf("Set RAID controller mode failed, the operation cannot be performed because there were "
                 "volumes or enhanced physical drives.\r\n");
    } else if (ret == SML_ERR_CTRL_OPERATION_NOT_SUPPORT) {
        g_printf("Set RAID controller mode operation is not supported.\r\n");
    } else {
        g_printf("Set RAID controller mode failed. Error code : 0x%04X.\r\n", ret);
    }

    uip_free_gvariant_list(input_list);

    return;
}


void set_pd_state(OBJ_HANDLE obj_handle, guint8 state, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(state));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_HDD_NAME,
        dfl_get_object_name(obj_handle), METHOD_HDD_SET_FIRMWARE_STATE, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set physical drive state successfully.\r\n");
    } else if (ret == SML_ERR_PD_OPERATION_NOT_SUPPORT) {
        g_printf("Set physical drive state operation is not supported.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set physical drive state. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set physical drive state. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}


void set_pd_hotspare_state(OBJ_HANDLE obj_handle, guint8 state, LD_LIST_S *ld_list, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL || ld_list == NULL || ld_list->count == 0) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(state));
    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_UINT16, ld_list->ids, ld_list->count, sizeof(guint16)));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_HDD_NAME,
        dfl_get_object_name(obj_handle), METHOD_HDD_SET_HOT_SPARE, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set physical drive hot spare state successfully.\r\n");
    } else if (ret == SML_ERR_PD_OPERATION_NOT_SUPPORT) {
        g_printf("Set physical drive hot spare state operation is not supported.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set physical drive hot spare state. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set physical drive hot spare state. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}


void set_pd_locate_state(OBJ_HANDLE obj_handle, guint8 state, guint8 duration, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(state));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(duration));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_HDD_NAME,
        dfl_get_object_name(obj_handle), METHOD_HDD_SET_LOCATE_STATE, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set physical drive location state successfully.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set physical drive location state. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set physical drive location state. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}


void set_pd_crypto_erase(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip)
{
    gint32 ret;

    if (username == NULL || ip == NULL) {
        return;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_HDD_NAME,
        dfl_get_object_name(obj_handle), METHOD_HDD_CRYPTO_ERASE, NULL, NULL);
    if (ret == RET_OK) {
        g_printf("Cryptographically erase physical drive successfully.\r\n");
    } else if (ret == SML_ERR_PD_OPERATION_NOT_SUPPORT || ret == SML_ERR_CTRL_OPERATION_NOT_SUPPORT) {
        g_printf("Cryptographically erase physical drive operation is not supported.\r\n");
    } else {
        g_printf("Cryptographically erase physical drive. Error code : 0x%04X.\r\n", ret);
    }
}


void set_pd_bootable(OBJ_HANDLE obj_handle, guint8 boot_param, gchar *username, gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(boot_param));

    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip, CLASS_HDD_NAME,
        dfl_get_object_name(obj_handle), METHOD_HDD_SET_BOOTABLE, input_list, NULL);
    if (ret == RET_OK) {
        g_printf("Set physical drive as boot device successfully.\r\n");
    } else {
        g_printf("Set physical drive as boot device failed. Error code : 0x%04X.\r\n", ret);
    }

    uip_free_gvariant_list(input_list);

    return;
}


void set_ctrl_copyback(OBJ_HANDLE obj_handle, guint8 copyback, const gchar *username, const gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(copyback));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_RAID_CONTROLLER_NAME,
        dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_SET_COPYBACK, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set RAID controller Copyback state successfully.\r\n");
    } else if (ret == SML_ERR_CTRL_OPERATION_NOT_SUPPORT) {
        g_printf("Set RAID controller Copyback state operation is not supported.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set RAID controller Copyback state failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set RAID controller Copyback state failed. Error code : 0x%04X.\r\n", ret);
        }
    }
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}


LOCAL void print_set_ctrl_jbod_log(gint32 ret_code)
{
    const gchar* message = NULL;
    switch (ret_code) {
        case RET_OK:
            g_printf("Set RAID controller JBOD mode successfully.\r\n");
            return;
        case SML_ERR_CTRL_OPERATION_NOT_SUPPORT:
            message = "Operation not supported";
            break;
        case SM_CODE_PARA_DATA_ILLEGAL:
            message = "Value not in list";
            break;
        case SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE:
            message = "Not supported in present state, try later";
            break;
        default:
            message = "No details";
            break;
    }
    g_printf("Set RAID controller JBOD mode failed. Error code : 0x%04X (%s).\r\n", ret_code, message);
}

void set_ctrl_jbod(OBJ_HANDLE obj_handle, guint8 jbod, const gchar *username, const gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(jbod));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_RAID_CONTROLLER_NAME,
        dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_SET_JBOD, input_list, &output_list);
    print_set_ctrl_jbod_log(ret);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}


void set_ctrl_restore(OBJ_HANDLE obj_handle, const gchar *username, const gchar *ip)
{
    gint32 ret;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_RAID_CONTROLLER_NAME,
        dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_RESTORE_SETTINGS, NULL, &output_list);
    if (ret == RET_OK) {
        g_printf("Restore RAID controller settings successfully.\r\n");
    } else if (ret == SML_ERR_REBOOT_REQUIRED) {
        g_printf("Restore RAID controller settings successfully, please reboot the OS to take effect.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Restore RAID controller settings failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Restore RAID controller settings failed. Error code : 0x%04X.\r\n", ret);
        }
    }

    uip_free_gvariant_list(output_list);
}


void set_ctrl_smarter_copyback(OBJ_HANDLE obj_handle, guint8 smarter_copyback,
    const gchar *username, const gchar *ip)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message = NULL;

    if (username == NULL || ip == NULL) {
        return;
    }

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(smarter_copyback));
    ret = uip_call_class_method(OPERATOR_LOG_CLI, username, ip, CLASS_RAID_CONTROLLER_NAME,
        dfl_get_object_name(obj_handle), METHOD_RAID_CONTROLLER_SET_SMARTER_COPYBACK, input_list, &output_list);
    if (ret == RET_OK) {
        g_printf("Set RAID controller Copyback state on SMART error successfully.\r\n");
    } else if (ret == SML_ERR_CTRL_OPERATION_NOT_SUPPORT) {
        g_printf("Set RAID controller Copyback state on SMART operation is not supported.\r\n");
    } else {
        if (output_list != NULL) {
            error_message = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
            g_printf("Set RAID controller Copyback state on SMART error failed. Error code : 0x%04X (%s).\r\n", ret,
                error_message != NULL ? error_message : "No details");
        } else {
            g_printf("Set RAID controller Copyback state on SMART error failed. Error code : 0x%04X.\r\n", ret);
        }
    }
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
}


void print_set_cli_session_timeout_helpinfo(void)
{
    g_printf("Usage: ipmcset -d notimeout -v <enabled|disabled> [value]\r\n");
    g_printf("Value:\r\n");
    g_printf("    1-480    CLI timeout period(min)\r\n");
    return;
}
