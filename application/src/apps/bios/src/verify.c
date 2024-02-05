

#include "verify.h"

#ifdef ARM64_HI1711_ENABLED

#define SWITCH_TO_PCH 0
#define SWITCH_TO_BMC 1

#define BACKUP_BIOS_REGION 1
#define IPMI_BUFFER_LEN 32
#define IPMI_ME_RETYR_COUNT 5
#define IPMI_IMU_RETRY_COUNT 11
#define BOOTGUARD_CAP_OFFSET 4
#define BOOTGUARD_SUPPORT_AND_ENABLED_MASK 0XC0
#define BOOTGUARD_HEALTH_DONE_OFFSET 4
#define BOOTGUARD_HEALTH_DONE_MASK 0X40
#define BOOTGUARD_HEALTH_STATUS_OFFSET 8
#define BOOTGUARD_HEALTH_MASK 0X1F
#define ME_RETYR_INTERAL 400

#define IPMI_ME_NETFN 0x2e
#define IPMI_ME_BOOTGUARD_CAP_CMD 0x81
#define IPMI_ME_BOOTGUARD_HEALTH_CMD 0x82


#define LINK_UP_BIOS_UPG_ACTION     "LinkUpBiosUpgAction"


static const gchar g_intel_r_string[] = { 0x49, 0x6e, 0x74, 0x65, 0x6c, 0xc2, 0xae, 0x00 };
static gulong g_boot_guard_task_id = 0;

LOCAL gint32 __get_bootguard_health_status(gchar *resp, guint32 resp_len);
LOCAL gint32 __wait_and_update_bootguard_health_status(void);
LOCAL gint32 g_bios_reported = RET_ERR;

void set_bios_reported_flag(gint32 flag)
{
    debug_log(DLOG_DEBUG, "%s: set bios reported flag to %d", __FUNCTION__, flag);
    g_bios_reported = flag;
}

gint32 get_bios_reported_flag(void)
{
    debug_log(DLOG_DEBUG, "%s: get bios reported flag %d", __FUNCTION__, g_bios_reported);
    return g_bios_reported;
}


LOCAL gboolean tianchi_support_bootguard(void)
{
    gint32 ret;
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;

    if (!dal_match_product_id(PRODUCT_ID_TIANCHI)) {
        return FALSE;
    }

    ret = dal_get_func_ability(CLASS_ME_INFO, PROPERTY_ME_ACCESS_TYPE, &me_access_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get MeInfo.MeAccessType failed!");
        return FALSE;
    }
    if (me_access_type == ME_ACCESS_TYPE_QZ) {
        return FALSE;
    }

    return TRUE;
}


LOCAL gchar __me_ipmi_send_request(guint8 netfn, guint8 cmd, gchar *senddata, guint32 datalenth, gchar *respdata,
    guint32 respdata_len)
{
    errno_t safe_fun_ret = EOK;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    const guint8 *resp_src_data = NULL;
    guint8 *ipmi_resp = NULL;
    guint8 resp_src_len;

    req_msg_head.target_type = IPMI_ME;
    req_msg_head.target_instance = 0;
    req_msg_head.netfn = netfn;
    req_msg_head.lun = 0;
    req_msg_head.cmd = cmd;
    req_msg_head.src_len = datalenth;

    if (ipmi_send_request(&req_msg_head, senddata, MAX_POSSIBLE_IPMI_FRAME_LEN, (gpointer *)&ipmi_resp, TRUE) ==
        RET_OK) {
        resp_src_data = get_ipmi_src_data(ipmi_resp);
        if (resp_src_data == NULL) {
            
            g_free(ipmi_resp);
            debug_log(DLOG_ERROR, "%s: Failed to get cmd:%02x %02x from me src_data", __FUNCTION__, netfn, cmd);
            return RET_ERR;
        }
        resp_src_len = get_ipmi_src_data_len(ipmi_resp);
        if (respdata != NULL) {
            safe_fun_ret = memmove_s(respdata, respdata_len, &resp_src_data[0], resp_src_len);
        }
        
        g_free(ipmi_resp);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: Recv cmd:%02x %02x response src_data_len:%d(Max:%d) invalid, safe_fun_ret = %d",
                __FUNCTION__, netfn, cmd, resp_src_len, respdata_len, safe_fun_ret);
            return RET_ERR;
        } else {
            return RET_OK;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: Failed to get cmd:%02x %02x from me src_data", __FUNCTION__, netfn, cmd);
        return RET_ERR;
    }
}

LOCAL gint32 __set_verify_result(OBJ_HANDLE obj_handle, guint8 type, guint8 is_arm, guint result_in)
{
    guint8 verify_result = VERIFY_RESULT_SUCCESS;

    if (result_in == VERIFY_RESULT_SUCCESS) {
        if (is_arm == 1) {
            return dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_FW_VERIFY_RESULT, result_in, DF_SAVE);
        } else if (type == VERIFY_BY_BOOTGUARD) {
            
            return dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_FW_VERIFY_RESULT, result_in, DF_SAVE);
        }
    } else {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_FW_VERIFY_RESULT, &verify_result);

        if (verify_result == VERIFY_RESULT_SUCCESS) {
            return dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_FW_VERIFY_RESULT, result_in, DF_SAVE);
        } else {
            return dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_BAKCUP_FW_VERIFY_RESULT, result_in, DF_SAVE);
        }
    }
    return RET_OK;
}


LOCAL void __recover_bios_from_gold(OBJ_HANDLE obj_handle)
{
    guint8 verify_result = 0;
    gint32 ret;
    OBJ_HANDLE up_handle = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_BAKCUP_FW_VERIFY_RESULT, &verify_result);

    if (verify_result == VERIFY_RESULT_FAILED) {
        
        debug_log(DLOG_ERROR, "%s: backup verify result is %d, no need recover", __FUNCTION__, verify_result);
        return;
    }

    
    if (vos_get_file_accessible(PFR_BIOS_GOLD_FILE) == FALSE) {
        
        __set_verify_result(obj_handle, INVALID_DATA_BYTE, INVALID_DATA_BYTE, VERIFY_RESULT_FAILED);
        debug_log(DLOG_ERROR, "%s: %s is not exist, no need recover", __FUNCTION__, PFR_BIOS_GOLD_FILE);
        return;
    }

    
    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &up_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: can't find object %s. ret is %d", __FUNCTION__, PFN_CLASS_NAME, ret);
        return;
    }

    ret = dfl_call_class_method(up_handle, METHOD_UPGRADE_RECOVER_BIOS, NULL, NULL, NULL);
    if (ret != DFL_OK) {
        
        debug_log(DLOG_ERROR, "%s: dfl_call_class_method %s.%s. ret is %d", __FUNCTION__, PFN_CLASS_NAME,
            METHOD_UPGRADE_RECOVER_BIOS, ret);
    }
    return;
}


LOCAL gint32 get_bios_resume_file(gchar* filename, gint8 filename_len)
{
    gint32 ret;

    if (g_access(PFR_BIOS_GOLD_FILE, F_OK) == RET_OK) {
        if (vos_check_dir(CACHE_UPGRADE_DIR) == FALSE) {
            (void)vos_mkdir_recursive(CACHE_UPGRADE_DIR);
            chmod(CACHE_UPGRADE_DIR, S_IRWXU);
        }
        ret = vos_file_copy(CACHE_BIOS_HPM_FILE, PFR_BIOS_GOLD_FILE);
        debug_log(DLOG_ERROR, "%s copy cpld resume file(%s) ret(%d).\n", __func__, PFR_BIOS_GOLD_FILE, ret);
        if (ret != RET_OK) {
            return ret;
        }
        ret = strcpy_s(filename, filename_len, CACHE_BIOS_HPM_FILE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s strcpy failed, ret=%d", __func__, ret);
        }
    } else {
        ret = vos_file_copy(PFR_TMP_BIOS_RESUME_FILE, PFR_BIOS_RESUME_FILE);
        debug_log(DLOG_ERROR, "%s copy bios resume file(%s) ret(%d).\n", __func__, PFR_BIOS_RESUME_FILE, ret);
        if (ret != RET_OK) {
            return ret;
        }
        ret = strcpy_s(filename, filename_len, PFR_TMP_BIOS_RESUME_FILE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s strcpy failed, ret=%d", __func__, ret);
        }
    }
    return ret;
}


LOCAL gint32 check_bios_upgrade_status(void)
{
    OBJ_HANDLE pfn_handle = 0;
    guint8 upgrade_flag = 0;
    guint8 component_id = 0;
    gint32 ret;

    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &pfn_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_object_handle_nth error(%d).", ret);
        return ret;
    }
    ret = dal_get_property_value_byte(pfn_handle, PFN_UPGRADE_INFLAG_NAME, &upgrade_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_byte error(%d).", ret);
        return ret;
    }

    ret = dal_get_property_value_byte(pfn_handle, PFN_COMP_ID_NAME, &component_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_byte error(%d).", ret);
        return ret;
    }
    
    if ((component_id == COMP_BIOS) && (upgrade_flag == TRUE)) {
        return RET_OK;
    }

    return RET_ERR;
}


LOCAL gint32 call_upgrade(gchar* filename, gint8 filename_len)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 forced_upgrade_flag;

    
    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Can't find object %s. Ret is %d", PFN_CLASS_NAME, ret);
        return RET_ERR;
    }

    forced_upgrade_flag = 0;
    input_list = g_slist_append(input_list, g_variant_new_string(filename));
    input_list = g_slist_append(input_list, g_variant_new_byte(forced_upgrade_flag));

    ret = dfl_call_class_method(obj_handle, METHOD_PFN_INITIATE_UP, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_call_class_method %s.%s. Ret is %d", PFN_CLASS_NAME, METHOD_PFN_INITIATE_UP, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void repair_bios(void)
{
    gint32 ret;
    gchar repair_bios_filename[RESUME_FILENAME_MAX_LEN] = { 0 };
    debug_log(DLOG_ERROR, "verify bios failed, repair bios now.");

    ret = get_bios_resume_file(repair_bios_filename, RESUME_FILENAME_MAX_LEN);
    if (ret != RET_OK) {
        return;
    }
    ret = call_upgrade(repair_bios_filename, RESUME_FILENAME_MAX_LEN);
    if (ret != RET_OK) {
        return;
    }

    strategy_log(SLOG_INFO, "The BIOS verification fails and the backup version is restored.");

    vos_task_delay(5000); 

    
    while (check_bios_upgrade_status() == MODULE_OK) {
        debug_log(DLOG_DEBUG, "bios is repairing now, please wait.");
        vos_task_delay(5000); 
    }

    debug_log(DLOG_ERROR, "bios repair is finished.");
}


LOCAL gint32 __get_bios_verify_result_from_m3(guint8 is_arm, guint32 retry_cnt)
{
    const guint32 VERIFY_RETRY_INTERVAL = 20; // 重试间隔
    gint32 ret;
    guint16 verify_result;
    guint32 i;

    for (i = 0; i < retry_cnt; i++) {
        verify_result = VERIFY_RESULT_SUCCESS;
        
        ret = dal_get_bios_verify_result(&verify_result);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: dal_get_bios_verify_result fail, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
        if (verify_result == VERIFY_RESULT_SUCCESS) {
            
            debug_log(DLOG_ERROR, "%s: The BIOS firmware is verified successfully by Secure Core", __FUNCTION__);
            syslog(LOG_AUTH | LOG_INFO, "The BIOS firmware is verified successfully by Secure Core.");
            break;
        } else if (verify_result == VERIFY_RESULT_FAILED) {
            
            repair_bios();
            strategy_log(SLOG_INFO, "Failed to verify the BIOS firmware and the backup version will be restored.");
            syslog(LOG_AUTH | LOG_INFO, "Secure Core Failed to verify the BIOS firmware.");
            debug_log(DLOG_ERROR, "%s: Secure Core Failed to verify the BIOS firmware, result = %d", __FUNCTION__,
                verify_result);
            return RET_ERR;
        } else {
            
            debug_log(DLOG_ERROR, "%s: get verify result err, result = %d, retry_cnt = %d", __FUNCTION__, verify_result,
                i);
        }
        vos_task_delay(VERIFY_RETRY_INTERVAL);
    }
    return RET_OK;
}

LOCAL void __set_bios_owner(guint8 value)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(PFN_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: can't find object %s. ret is %d", __FUNCTION__, PFN_CLASS_NAME, ret);
        return;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(value));
    ret = dfl_call_class_method(obj_handle, METHOD_PFN_CHANGE_BIOS_MUX, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_call_class_method %s.%s. ret is %d", __FUNCTION__, PFN_CLASS_NAME,
            METHOD_PFN_CHANGE_BIOS_MUX, ret);
    }
    return;
}


LOCAL gint32 deal_with_update_general_action(OBJ_HANDLE obj_handle)
{
    guint8 property_value = 0;
    GSList *input_list = NULL;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_UPGRADE_ACTION_DATA, &property_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get action data fail, ret=%d", ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(property_value));
    ret = dfl_call_class_method(obj_handle, METHOD_SET_ACTION_DATA, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    debug_log(DLOG_ERROR, "write action data %s, ret=%d", (ret == RET_OK) ? "successfully" : "failed", ret);

    return ret;
}
 

LOCAL gint32 extra_before_load_bios_CST0220V6(void)
{
    gint32 ret;
    OBJ_HANDLE action_obj = 0;
 
    ret = dfl_get_object_handle(LINK_UP_BIOS_UPG_ACTION, &action_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get %s Action failed! ret=%d", LINK_UP_BIOS_UPG_ACTION, ret);
        return RET_ERR;
    }
    ret = deal_with_update_general_action(action_obj);
    if (ret == RET_OK) {
        debug_log(DLOG_ERROR, " %s Action success!", LINK_UP_BIOS_UPG_ACTION);
    }
    
    return ret;
}


LOCAL void verify_bios_restore_flash_switch(void)
{
    gint32 ret;
    OBJ_HANDLE up_handle = 0;
    GSList *input_list = NULL;
    guint8 default_val = 0;
 
    ret = dal_get_object_handle_nth(CLASS_BIOS_FLASH_SWITCH, 0, &up_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get object handle failed", __FUNCTION__);
        return; // 没有该对象的环境说明不需要切换flash
    }
 
    input_list = g_slist_append(input_list, g_variant_new_byte(default_val));
    ret = dfl_call_class_method(up_handle, METHOD_SET_BIOS_FLASH_SWITCH, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    debug_log(DLOG_INFO, "%s: restore switch execute, result is %d.", __FUNCTION__, ret);
 
    vos_task_delay(1000); // 根据硬件反馈切换biosflash后需要等待几毫秒生效,延时1000ms可成功
    return;
}

LOCAL gint32 verify_bios_by_m3(OBJ_HANDLE obj_handle, guint8 is_arm)
{
    const guint32 VERIFY_RETRY_CNT_X86 = 3;  // X86重试次数
    const guint32 VERIFY_RETRY_CNT_ARM = 50; // ARM重试次数
    guint8 flash_num = get_bios_flash_num();
    guint8 bios_pfr_support = SUPPORT; // 此处默认视为支持，少数需要依赖CPLD配套不支持

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_BIOS_PFR_SUPPORT,
        &bios_pfr_support);
    if (bios_pfr_support != SUPPORT) {
        return RET_OK;
    }

    if (is_arm == 0) {
        
        gint32 result = __get_bios_verify_result_from_m3(is_arm, VERIFY_RETRY_CNT_X86);
        
        return (result != RET_OK ? RET_ERR : RET_OK);
    }

    // bios的gpio通道复用，只有计算型存储半宽计算涉及，不通用
    if (dal_match_board_id(PRODUCT_ID_PANGEA_V6, CST0220V6_BOARD_ID) == TRUE) {
        extra_before_load_bios_CST0220V6();
    }

    
    __set_bios_owner(SWITCH_TO_BMC);

    for (gint32 i = 0; i < flash_num; i++) {
        if (bios_flash_switch(i, 1) != RET_OK) {
            (void)unlock_bios_flash_switch(1);
            verify_bios_restore_flash_switch();
            __set_bios_owner(SWITCH_TO_PCH);
            return RET_ERR;
        }
        
        gint32 ret = dal_set_bios_verify_ctrl(is_arm);
        if (ret == BIOS_VERIFY_ERROR_CODE) {
            repair_bios();
        }

        if (ret != RET_OK) {
            (void)unlock_bios_flash_switch(1);
            verify_bios_restore_flash_switch();
            __set_bios_owner(SWITCH_TO_PCH);
            return RET_ERR;
        }
        
        ret = __get_bios_verify_result_from_m3(is_arm, VERIFY_RETRY_CNT_ARM);
        (void)unlock_bios_flash_switch(1);  // 此处无需关心返回值，如果失败后面会报错
        if (ret == RET_OK) {
            debug_log(DLOG_INFO, "%s::bios %d verify success", __FUNCTION__, i);
            continue;
        } else {
            debug_log(DLOG_ERROR, "%s:bios %d verify failed,ret:%d", __FUNCTION__, i, ret);
            verify_bios_restore_flash_switch();
            return RET_ERR; 
        }
    }
    verify_bios_restore_flash_switch();
    
    __set_bios_owner(SWITCH_TO_PCH);
    return RET_OK;
}


LOCAL gint32 __get_bios_bootguard_capabilities(guint8 *cap)
{
    guint8 retry = IPMI_ME_RETYR_COUNT;
    gchar req[] = { 0x57, 0x01, 0x00 };
    gchar resp[IPMI_BUFFER_LEN] = {0};
    gint32 ret;

    if (tianchi_support_bootguard()) {
        req[0] = 0xDB;    // 0: manufactureid[0]
        req[1] = 0x07;    // 1: manufactureid[1]
        req[2] = 0x00;    // 2: manufactureid[2]
    }
    *cap = 0;
    while (retry--) {
        ret = __me_ipmi_send_request(IPMI_ME_NETFN, IPMI_ME_BOOTGUARD_CAP_CMD, req, sizeof(req), resp, sizeof(resp));
        if (ret == RET_OK && resp[0] == COMP_CODE_SUCCESS) {
            debug_log(DLOG_ERROR, "%s: get boot guard capabilities success, cap = 0x%02x", __FUNCTION__,
                resp[BOOTGUARD_CAP_OFFSET]);
            *cap = resp[BOOTGUARD_CAP_OFFSET];
            return RET_OK;
        }
        vos_task_delay(ME_RETYR_INTERAL);
    }
    
    debug_log(DLOG_ERROR, "%s: get boot guard capabilities failed, ret = %d", __FUNCTION__, ret);

    return RET_ERR;
}


LOCAL gint32 __get_bios_bootguard_capabilities_with_debounce(guint8 *cap)
{
    gint32 ret;
    guint8 retry = IPMI_ME_RETYR_COUNT;
    guint8 zero_cout = 0;
    *cap = 0;

    if (tianchi_support_bootguard()) {
        retry = IPMI_IMU_RETRY_COUNT;
    }
    while (--retry) {
        
        ret = __get_bios_bootguard_capabilities(cap);
        if (ret == RET_OK) {
            if (*cap == 0) {
                zero_cout++;
            } else {
                break;
            }
        }
        vos_task_delay(DELAY_HALF_SECOND);
    }

    if (retry == 0) {
        if (zero_cout > 0) {
            
            *cap = 0;
            return RET_OK;
        }
        debug_log(DLOG_ERROR, "%s:get boot guard capabilities failed", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __update_boot_guard_status_task(void)
{
    OBJ_HANDLE bios_handle = 0;
    gint32 ret = RET_OK;
    guint8 cap = INVALID_DATA_BYTE;
    (void)dfl_get_object_handle(BIOS_CLASS_NAME, &bios_handle);
    ret = __get_bios_bootguard_capabilities_with_debounce(&cap);
    if (ret == RET_OK) {
        debug_log(DLOG_DEBUG, "%s: boot guard capabilities %02x", __FUNCTION__, cap);

        
        switch (cap & BOOTGUARD_SUPPORT_AND_ENABLED_MASK) {
            case 0XC0:
                syslog(LOG_AUTH | LOG_INFO, "%s Boot Guard is supported and enabled.", g_intel_r_string);
                ret = __wait_and_update_bootguard_health_status();
                break;
            case 0:
                syslog(LOG_AUTH | LOG_INFO, "%s Boot Guard is not supported.", g_intel_r_string);
                break;
            case 0X40:
                syslog(LOG_AUTH | LOG_INFO, "%s Boot Guard is supported but not enabled.", g_intel_r_string);
                break;
            default:
                debug_log(DLOG_ERROR, "%s: boot guard capabilities is unknown (0x%02x)", __FUNCTION__, cap);
                break;
        }
    }
    if (ret == RET_OK) {
        __set_verify_result(bios_handle, VERIFY_BY_BOOTGUARD, 0, VERIFY_RESULT_SUCCESS);
    } else {
        __set_verify_result(bios_handle, INVALID_DATA_BYTE, INVALID_DATA_BYTE, VERIFY_RESULT_FAILED);
        __recover_bios_from_gold(bios_handle);
    }
    g_boot_guard_task_id = 0;
    return RET_OK;
}

void update_bios_bootguard_status(void)
{
    const guint32 INTEVAL_TIME = 8000; // 参考hop_check_host_reset,间隔8秒内的复位不算
    static gulong last_time = 0;
    gulong current_time;
    guint32 diff;
    guint8 arm_enable = TRUE;

    set_bios_reported_flag(RET_ERR);

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (arm_enable && !tianchi_support_bootguard()) {
        // ARM服务器除部分天池机型，其他都不支持boot guard
        return;
    }
    
    current_time = vos_tick_get();
    if (current_time > last_time) {
        diff = current_time - last_time;
    } else {
        diff = ((0xFFFFFFFF - last_time) + current_time);
    }
    debug_log(DLOG_INFO, "detect system reset,%d microseconds later than last reset", diff);
    if (diff < INTEVAL_TIME) {
        return;
    }
    last_time = current_time;

    if (g_boot_guard_task_id == 0) {
        if (vos_task_create(&g_boot_guard_task_id, "bg_status", (TASK_ENTRY)__update_boot_guard_status_task, NULL,
            DEFAULT_PRIORITY) != RET_OK) {
            debug_log(DLOG_ERROR, "Failed to create task __update_boot_guard_status_task");
            return;
        }
    }

    return;
}

LOCAL gint32 __get_bootguard_health_status(gchar *resp, guint32 resp_len)
{
    gint32 ret;
    guint8 me_read_retry = IPMI_ME_RETYR_COUNT;
    gchar req[] = { 0x57, 0x01, 0x00 };

    if (tianchi_support_bootguard()) {
        req[0] = 0xDB;    // 0: manufactureid[0]
        req[1] = 0x07;    // 1: manufactureid[1]
        req[2] = 0x00;    // 2: manufactureid[2]
    }

    while (--me_read_retry) {
        ret = __me_ipmi_send_request(IPMI_ME_NETFN, IPMI_ME_BOOTGUARD_HEALTH_CMD, req, sizeof(req), resp, resp_len);
        if (ret == RET_OK && resp[0] == COMP_CODE_SUCCESS) {
            break;
        }
        vos_task_delay(ME_RETYR_INTERAL);
    }
    if (me_read_retry == 0) {
        
        debug_log(DLOG_ERROR, "%s: get boot guard health failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    debug_log(DLOG_DEBUG, "%s Boot Guard health status:%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x", g_intel_r_string,
        resp[4], resp[5], resp[6], resp[7], resp[8], resp[9], resp[10], resp[11], resp[12]);
    return RET_OK;
}

LOCAL gint32 __wait_acm_boot_done(void)
{
    guint32 i;
    gint32 ret;
    const guint32 ACM_WAIT_COUNT = 20; // 等待20s
    gchar resp[IPMI_BUFFER_LEN] = {0};

    for (i = 0; i < ACM_WAIT_COUNT; i++) {
        ret = __get_bootguard_health_status(resp, sizeof(resp));
        
        if (ret == RET_OK &&
            (((guchar)resp[BOOTGUARD_HEALTH_DONE_OFFSET] & BOOTGUARD_HEALTH_DONE_MASK) == BOOTGUARD_HEALTH_DONE_MASK)) {
            return RET_OK;
        }
        vos_task_delay(DELAY_ONE_SECOND);
    }
    return RET_ERR;
}

LOCAL gint32 __wait_bios_report(void)
{
    const gint32 MAX_WAIT_COUNT = 60; 
    gint32 i;
    OBJ_HANDLE payload_handle = 0;
    guint8 power_gd = 0;
    OBJ_HANDLE sec_moudle_handle = 0;
    gchar sec_moudle_type[MAX_PROPERTY_VALUE_LEN] = {};
    gint32 ret;

    
    ret = dal_get_specific_object_byte(CLASS_SECURITY_MODULE, PROPERTY_SECURITY_MODULE_PRESENCE, 1, &sec_moudle_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: SecurityModule is not present", __FUNCTION__);
        return RET_ERR;
    }
    (void)dal_get_property_value_string(sec_moudle_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE, sec_moudle_type,
        sizeof(sec_moudle_type));
    if (strcmp(sec_moudle_type, "TPM") == 0) {
        
        debug_log(DLOG_DEBUG, "%s: SecurityModule is %s, do not wait", __FUNCTION__, sec_moudle_type);
        return RET_ERR;
    }

    (void)dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &payload_handle);

    
    for (i = 0; i < MAX_WAIT_COUNT; i++) {
        if (get_bios_reported_flag() == RET_OK) {
            return RET_OK;
        }
        dal_get_property_value_byte(payload_handle, PROPERTY_PAYLOAD_PWR_GD, &power_gd);
        if (power_gd == 0) {
            debug_log(DLOG_ERROR, "%s: waiting is aborted due to power off", __FUNCTION__);
            return RET_ERR;
        }
        vos_task_delay(DELAY_HALF_SECOND);
    }
    return RET_ERR;
}

LOCAL gint32 __wait_and_update_bootguard_health_status(void)
{
    gint32 ret;
    gchar resp[IPMI_BUFFER_LEN] = {0};
    guint8 status;

    ret = __wait_acm_boot_done();
    if (ret != RET_OK) {
        
        debug_log(DLOG_ERROR, "%s: ACM is not boot done, 0x%02d", __FUNCTION__, resp[BOOTGUARD_HEALTH_DONE_OFFSET]);
    }
    ret = __get_bootguard_health_status(resp, sizeof(resp));
    if (ret != RET_OK) {
        
        debug_log(DLOG_ERROR, "%s: ACM is not boot done, 0x%02d", __FUNCTION__, resp[BOOTGUARD_HEALTH_DONE_OFFSET]);
        return RET_OK;
    }
    
    status = (guchar)resp[BOOTGUARD_HEALTH_STATUS_OFFSET] & BOOTGUARD_HEALTH_MASK;

    
    if (status == 0 || status == 0x0c) {
        syslog(LOG_AUTH | LOG_INFO, "The BIOS firmware is verified successfully by %s Boot Guard.", g_intel_r_string);
        debug_log(DLOG_ERROR, "%s: The BIOS firmware is verified successfully by %s Boot Guard, ESC(0x%02X).",
            __FUNCTION__, g_intel_r_string, status);
        return RET_OK;
    } else {
        debug_log(DLOG_ERROR, "%s: Boot Guard health status: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x",
            __FUNCTION__, resp[4], resp[5], resp[6], resp[7], resp[8], resp[9], resp[10], resp[11], resp[12]);

        
        ret = __wait_bios_report();
        if (ret == RET_OK) {
            debug_log(DLOG_ERROR, "%s: Boot Guard ESC (0x%02X), but bios is report ok.", __FUNCTION__, status);
            syslog(LOG_AUTH | LOG_INFO, "The BIOS firmware is verified successfully by %s Boot Guard.",
                g_intel_r_string);
            return RET_OK;
        }

        strategy_log(SLOG_INFO, "Failed to verify the BIOS firmware and the backup version will be restored.");
        syslog(LOG_AUTH | LOG_INFO, "%s Boot Guard failed to verify the BIOS firmware.", g_intel_r_string);
        debug_log(DLOG_ERROR, "%s: %s Boot Guard failed to verify the BIOS firmware, ESC(0x%02X).", __FUNCTION__,
            g_intel_r_string, status);

        return RET_ERR;
    }
}

LOCAL gint32 __verify_bios_by_bootguard(OBJ_HANDLE obj_handle, guint8 is_arm)
{
    guint8 cap = INVALID_DATA_BYTE;

    
    if (is_arm && !tianchi_support_bootguard()) {
        return RET_OK;
    }
    if (__get_bios_bootguard_capabilities_with_debounce(&cap) != RET_OK) {
        return RET_OK;
    }

    if ((cap & BOOTGUARD_SUPPORT_AND_ENABLED_MASK) != BOOTGUARD_SUPPORT_AND_ENABLED_MASK) {
        
        return RET_OK;
    }

    
    if (__wait_and_update_bootguard_health_status() == RET_OK) {
        return RET_OK;
    }

    return RET_ERR;
}

gint32 method_set_bios_fw_verify_result(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 verify_result;
    guint8 backup_verify_result;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        return RET_ERR;
    }
    verify_result = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    backup_verify_result = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (backup_verify_result != INVALID_DATA_BYTE) {
        dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_BAKCUP_FW_VERIFY_RESULT, backup_verify_result, DF_SAVE);
    }
    if (verify_result != INVALID_DATA_BYTE) {
        dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_FW_VERIFY_RESULT, verify_result, DF_SAVE);
    }
    return RET_OK;
}

gint32 method_verify_bios_firmware(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 type;
    guchar arm_enable = 0;
    gint32 result = RET_OK;
    guint8 auto_recover;

    
    if (input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list or output_list is NULL", __FUNCTION__);
        return RET_OK;
    }

    type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    auto_recover = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (TRUE == vos_get_file_accessible((const gchar *)CACHE_BIOS_HPM_FILE)) {
        
        debug_log(DLOG_ERROR, "%s:bios chache file exsits, no need to verify", __FUNCTION__);
        goto EXIT;
    }

    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s.%s failed, ret = %d", __FUNCTION__, CLASS_NAME_PME_SERVICECONFIG,
            PROTERY_PME_SERVICECONFIG_ARM, ret);
        goto EXIT;
    }

    debug_log(DLOG_MASS, "%s:arm_enable is %d, type = %d, auto_recover = %d", __FUNCTION__, arm_enable, type,
        auto_recover);
    switch (type) {
        case VERIFY_BY_M3:
            result = verify_bios_by_m3(obj_handle, arm_enable);
            break;
        case VERIFY_BY_BOOTGUARD:
            result = __verify_bios_by_bootguard(obj_handle, arm_enable);
            break;
        default:
            break;
    }
EXIT:

    if (result == RET_OK) {
        __set_verify_result(obj_handle, type, arm_enable, VERIFY_RESULT_SUCCESS);
    } else {
        __set_verify_result(obj_handle, INVALID_DATA_BYTE, INVALID_DATA_BYTE, VERIFY_RESULT_FAILED);
        if (auto_recover) {
            __recover_bios_from_gold(obj_handle);
        }
    }
    *output_list = g_slist_append(*output_list, g_variant_new_int32(result));

    return RET_OK;
}

gint32 ipmi_get_bios_gold_valid(const void *msg_data, gpointer user_data)
{
    OBJ_HANDLE bios_handle = 0;
    guint8 resp[] = { 0, 0 };
    guint8 backup_verify_result = VERIFY_RESULT_SUCCESS;
    if (msg_data == NULL) {
        return RET_ERR;
    }

    (void)dfl_get_object_handle(BIOS_CLASS_NAME, &bios_handle);
    (void)dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_BAKCUP_FW_VERIFY_RESULT, &backup_verify_result);

    resp[0] = COMP_CODE_SUCCESS;

    if (vos_get_file_accessible(PFR_BIOS_GOLD_FILE) == TRUE && backup_verify_result == VERIFY_RESULT_SUCCESS) {
        
        resp[1] = TRUE;
    } else {
        resp[1] = FALSE;
    }

    return ipmi_send_response(msg_data, sizeof(resp), resp);
}

#endif