

#include "ipmcget_component_mgmt.h"

typedef struct {
    guint16 target_id;
    const gchar *drive_name;
    guint8 drive_status;
    guint8 raid_level;
    guint8 def_read_policy;
    guint8 def_write_policy;
    guint8 def_cache_policy;
    guint8 cur_read_policy;
    guint8 cur_write_policy;
    guint8 cur_cache_policy;
    guint8 access_policy;
    guint8 disk_cache_policy;
    guint8 cc_state;
    guint8 strip_size;
    guint32 size;
    guint8 span_depth;
    guint8 drive_num_per_span;
    guint8 bgi_state;
    guint8 bootable;
    guint8 boot_priority;
    guint8 is_sscd;
    const guint8 *pd_slot;
    const guint16 *ref_array;
    guint8 sscd_caching_enable;
    const guint16 *associated_lds;
    const guint8 *spared_pd_slots;
    guint8 init_state;
    const gchar *os_letter;
    const guint16 *pd_enclosure;
    const guint16 *spared_pd_enclosures;
    guint8 accelerator;
    guint8 cache_line_size;
    guint8 rebuild_state;
    guint8 rebuild_progress;
} CLI_LD_INFO;


LOCAL void print_controller_device_interface(OBJ_HANDLE raid_handle)
{
    gchar device_interface_str[128] = { 0 };

    if (raid_handle == 0) {
        return;
    }

    g_printf("%-45s : ", "Device Interface");
    (void)dal_get_property_value_string(raid_handle, PROPERTY_RAID_CONTROLLER_DEVICE_INTERFACE_STR,
        device_interface_str, sizeof(device_interface_str));
    g_printf("%s\n", device_interface_str);

    return;
}


LOCAL void print_controller_cache_pinned_info(guint8 cache_pinned, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        return;
    }
    g_printf("%-45s : ", "Controller Cache Is Pinned");

    if (cache_pinned == TRUE) {
        g_printf("%s\n", "Yes");
    } else if (cache_pinned == FALSE) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }
    return;
}


LOCAL void print_controller_maint_pd_fail_info(guint8 maint_pd_fail)
{
    g_printf("%-45s : ", "Maintain PD Fail History across Reboot");

    if (maint_pd_fail == TRUE) {
        g_printf("%s\n", "Yes");
    } else if (maint_pd_fail == FALSE) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }
    return;
}


LOCAL void print_controller_spare_activation_mode(guint8 spare_activation_mode, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        g_printf("%-45s : ", "Spare Activation Mode");
        if (spare_activation_mode == 1) {  // SpareActivationMode为Predictive
            g_printf("%s\n", "Predictive");
        } else if (spare_activation_mode == 0) { // SpareActivationMode为failure
            g_printf("%s\n", "Failure");
        } else {
            g_printf("%s\n", "N/A");
        }
    }
    return;
}


LOCAL void print_controller_pcie_link_width(guint8 pcie_link_width, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC) ||
        dal_test_controller_vendor(type_id, VENDER_HUAWEI)) {
        if (pcie_link_width == STORAGE_INFO_INVALID_BYTE) {
            g_printf("%-45s : %s\n", "Host bus link Width", "N/A");
        } else {
            g_printf("%-45s : %d bit(s)/link(s)\n", "Host bus link Width", pcie_link_width);
        }
    }
    return;
}


LOCAL void print_controller_no_battery_write_cache(guint8 no_battery_write_cache, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        g_printf("%-45s : ", "No-Battery Write Cache");
        if (no_battery_write_cache == TRUE) {
            g_printf("%s\n", "Enabled");
        } else if (no_battery_write_cache == FALSE) {
            g_printf("%s\n", "Disabled");
        } else {
            g_printf("%s\n", "N/A");
        }
    }
    return;
}


LOCAL void print_controller_read_cache_percent(guint8 read_cache_percent, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        if (read_cache_percent == STORAGE_INFO_INVALID_BYTE) {
            g_printf("%-45s : %s\n", "Read Cache Percentage", "N/A");
        } else {
            g_printf("%-45s : %d percent\n", "Read Cache Percentage", read_cache_percent);
        }
    }
    return;
}


LOCAL void print_controller_health(guint16 health_code)
{
    g_printf("%-45s : ", "Controller Health");

    if (health_code == 0xFFFF) {
        g_printf("%s\n", "N/A");
        return;
    }

    if (health_code == 0) {
        g_printf("\033[1;32m%s\033[0m\n", "Normal");
        return;
    }

    g_printf("\033[1;31m%s\033[0m\n", "Abnormal");
    g_printf("\t%s\n", "Details:");

    if (health_code & TRUE) {
        g_printf("\t\t%s\n", "Memory correctable errors");
    }

    if (health_code & (TRUE << BIT_OFFSET_MEMORY_CORRECTABLE_ERROR)) {
        g_printf("\t\t%s\n", "Memory uncorrectable errors");
    }

    if (health_code & (TRUE << BIT_OFFSET_MEMORY_UNCORRECTABLE_ERROR)) {
        g_printf("\t\t%s\n", "Memory ECC errors reached limit");
    }

    if (health_code & (TRUE << BIT_OFFSET_ECC_ERROR)) {
        g_printf("\t\t%s\n", "NVRAM uncorrectable errors");
    }

    if (health_code & (TRUE << BIT_OFFSET_COMMUNICATION_LOST)) {
        g_printf("\t\t%s\n", "Raid communication loss");
    }

    g_printf("\n\n");

    return;
}


LOCAL void print_controller_strip_size_options(guint8 min_strip, guint8 max_strip)
{
    g_printf("%-45s : ", "Minimum Strip Size Supported");

    if (min_strip == 0xFF) {
        g_printf("%s\n", "N/A");
    } else if (min_strip == 0) {
        g_printf("%d B\n", 512);
    } else if (min_strip <= 10) {
        g_printf("%d KB\n", 1 << (min_strip - 1));
    } else {
        g_printf("%d MB\n", (1 << (min_strip - 1)) / 1024);
    }

    g_printf("%-45s : ", "Maximum Strip Size Supported");

    if (max_strip == 0xFF) {
        g_printf("%s\n", "N/A");
    } else if (max_strip == 0) { // 1
        g_printf("%d B\n", 512);
    } else if (max_strip <= 10) {
        g_printf("%d KB\n", 1 << (max_strip - 1));
    } else {
        g_printf("%d MB\n", (1 << (max_strip - 1)) / 1024);
    }
}


LOCAL void print_controller_properties(guint8 copyback_enabled, guint8 smarter_copyback_enabled, guint8 jbod_enabled,
    guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        return;
    }

    g_printf("%-45s : ", "Copyback Enabled");

    if (copyback_enabled == TRUE) {
        g_printf("%s\n", "Yes");
    } else if (copyback_enabled == FALSE) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    g_printf("%-45s : ", "Copyback on SMART error Enabled");

    if (smarter_copyback_enabled == TRUE) {
        g_printf("%s\n", "Yes");
    } else if (smarter_copyback_enabled == FALSE) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    g_printf("%-45s : ", "JBOD Enabled");
	
    if (jbod_enabled == TRUE) {
        g_printf("%s\n", "Yes");
    } else if (jbod_enabled == FALSE) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }
    return;
}

LOCAL void print_drive_write_cache_policy(guint32 ctrl_opt, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        SML_CTRL_OPTION3_S ctrl_opt_t = (SML_CTRL_OPTION3_S)ctrl_opt;
        SML_CTRL_OPTION3_S *opt3 = &ctrl_opt_t;
        g_printf("%-45s : ", "Configured Drives Write Cache Policy");
        g_printf("%s\n", sml_ctrl_drive_wcp2str(opt3->opt3_div.configured_drive_wcp));
        g_printf("%-45s : ", "Unonfigured Drives Write Cache Policy");
        g_printf("%s\n", sml_ctrl_drive_wcp2str(opt3->opt3_div.unconfigured_drive_wcp));
        g_printf("%-45s : ", "HBA Drives Write Cache Policy");
        g_printf("%s\n", sml_ctrl_drive_wcp2str(opt3->opt3_div.hba_drive_wcp));
    }
    return;
}


LOCAL void print_supported_raid_level(guint32 raid_level)
{
#define BUFFER_SZ 40
    errno_t safe_fun_ret = EOK;
    guint32 i;
    gchar* level_array[] = {"0", "1", "5", "6", "10", "50", "60", "1(ADM)", "10(ADM)", "1Triple", "10Triple"};
    gchar buf[BUFFER_SZ] = { 0 };
    gchar buffer[BUFFER_SZ] = { 0 };

    g_printf("%-45s : ", "Supported RAID Levels");
    if ((raid_level & 0x7FF00) == 0) {  // 0x7FF00表示从bit8~bit19的掩码
        g_printf("%s", "N/A");
        g_printf("\n");
        return;
    }

    for (i = 0; i < G_N_ELEMENTS(level_array); i++) {
        if (raid_level & (1 << (i + 8))) {  // 从第8位开始
            if (i > 0) {
                safe_fun_ret = strncat_s(buf, sizeof(buf), "/", strlen("/"));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }

            safe_fun_ret = strncat_s(buf, sizeof(buf), level_array[i], strlen(level_array[i]));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
        }
    }
    gint32 iRet = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "%s%s%s", "RAID(", buf, ")");
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
    }
    g_printf("%s", buffer);
    g_printf("\n");
    return;
}


LOCAL void print_bbu_status(OBJ_HANDLE raid_obj)
{
    gint32 iRet = -1;
    OBJ_HANDLE bbu_obj = 0;
    guint8 bbu_present = 0;
    gchar  bbu_type[32] = {0};
    guint8 normal = TRUE;
    guint8 voltage_low = 0;
    guint8 replace_pack = 0;
    guint8 learn_cycle_failed = 0;
    guint8 learn_cycle_timeout = 0;
    guint8 predictive_failure = 0;
    guint8 remaining_capacity_low = 0;
    guint8 no_space = 0;
    gchar  error_details[1024] = {0};

    gint32 ret = dfl_get_referenced_object(raid_obj, PROPERTY_RAID_CONTROLLER_REF_OBJECT_BBUSTATUS, &bbu_obj);
    if (ret != DFL_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_PRESENT, &bbu_present);
    if (ret != RET_OK) {
        return;
    }

    g_printf("\n%-45s : ", "BBU Status");
    if (bbu_present == 0) {
        g_printf("%s\n", "Absent");
        return;
    } else if (bbu_present == 0xff) {
        g_printf("%s\n", "N/A");
        return;
    } else {
        g_printf("%s\n", "Present");
    }

    ret = dal_get_property_value_string(bbu_obj, PROPERTY_BBU_TYPE, bbu_type, sizeof(bbu_type));
    if (ret != RET_OK) {
        return;
    }

    g_printf("%-45s : %s\n", "BBU Type", bbu_type);

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_VOLTAGE_LOW, &voltage_low);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_REPLACE_PACK, &replace_pack);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_LEARN_CYCLE_FAILED, &learn_cycle_failed);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_LEARN_CYCLE_TIMEOUT, &learn_cycle_timeout);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_PREDICTIVE_FAILURE, &predictive_failure);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_REMAINING_CAPACITY_LOW, &remaining_capacity_low);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_property_value_byte(bbu_obj, PROPERTY_BBU_NO_SPACE_FOR_CACHE_OFFLOAD, &no_space);
    if (ret != RET_OK) {
        return;
    }

    g_printf("%-45s : ", "BBU Health");

    
    if (voltage_low == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "Voltage Low");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    if (replace_pack == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "Battery Needs To Be Replaced");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    if (learn_cycle_failed == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "Learn Cycle Failed");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    if (learn_cycle_timeout == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "Learn Cycle Timeout");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    if (predictive_failure == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "Pack is about to fail");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    if (remaining_capacity_low == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "Remaining Capacity Low");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    if (no_space == 1) {
        iRet = snprintf_s(error_details + strlen(error_details), sizeof(error_details) - strlen(error_details),
            sizeof(error_details) - strlen(error_details) - 1, "\t\t%s\n", "No Space for Cache Offload");
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, iRet);
        }
        normal = FALSE;
    }

    
    if (normal == TRUE) {
        g_printf("\033[1;32m%s\033[0m\n", "Normal");
    } else {
        g_printf("\033[1;31m%s\033[0m\n", "Abnormal");
        g_printf("\t%s\n", "Details:");
        g_printf("%s\n", error_details);
    }

    return;
}


LOCAL void print_controller_phy_err_info(OBJ_HANDLE raid_obj)
{
    GVariant *array_obj = NULL;
    GVariant *obj_name_array = NULL;
    GVariantIter iter;
    OBJ_HANDLE ref_object = 0;
    guint8 id = 0;
    guint32 invalid_dword_count;
    guint32 loss_dword_sync_count;
    guint32 phy_reset_problem_count;
    guint32 running_disparity_error_count;
    guint8 available = FALSE;

    gint32 retval = dfl_get_property_value(raid_obj, PROPERTY_RAID_CONTROLLER_REF_OBJECT_SASPHYSTATUS, &array_obj);
    if (retval != DFL_OK) {
        debug_log(DLOG_MASS, "Not found SAS PHY Status object at %s", dfl_get_object_name(raid_obj));
        return;
    }
    g_variant_iter_init(&iter, array_obj);

    g_printf("\n%-45s : ", "PHY Status");

    while ((obj_name_array = g_variant_iter_next_value(&iter)) != NULL) {
        const gchar *obj_name = g_variant_get_string(obj_name_array, NULL);
        if (obj_name == NULL) {
            g_variant_unref(obj_name_array);
            break;
        }

        
        retval = dfl_get_object_handle(obj_name, &ref_object);
        if (retval != DFL_OK) {
            g_variant_unref(obj_name_array);
            debug_log(DLOG_ERROR, "%s: Get object (%s) handle failed, result=%d\n", __FUNCTION__, obj_name, retval);
            break;
        }
        g_variant_unref(obj_name_array);

        retval = dal_get_property_value_byte(ref_object, PROPERTY_SASPHYSTATUS_PHY_ID, &id);
        if (retval != RET_OK) {
            continue;
        }
        retval =
            dal_get_property_value_uint32(ref_object, PROPERTY_SASPHYSTATUS_INVALID_DWORD_COUNT, &invalid_dword_count);
        if (retval != RET_OK) {
            continue;
        }
        retval = dal_get_property_value_uint32(ref_object, PROPERTY_SASPHYSTATUS_LOSS_DWORD_SYNC_COUNT,
            &loss_dword_sync_count);
        if (retval != RET_OK) {
            continue;
        }
        retval = dal_get_property_value_uint32(ref_object, PROPERTY_SASPHYSTATUS_PHY_RESET_PROBLEM_COUNT,
            &phy_reset_problem_count);
        if (retval != RET_OK) {
            continue;
        }
        retval = dal_get_property_value_uint32(ref_object, PROPERTY_SASPHYSTATUS_RUNNING_DISPARITY_ERROR_COUNT,
            &running_disparity_error_count);
        if (retval != RET_OK) {
            continue;
        }

        if (id != 0xFF) {
            g_printf("\n\tPHY #%d :\n", id);
            g_printf("\t\t%-30s: %d\n", "Invalid Dword Count", invalid_dword_count);
            g_printf("\t\t%-30s: %d\n", "Loss Dword Sync Count", loss_dword_sync_count);
            g_printf("\t\t%-30s: %d\n", "PHY Reset Problem Count", phy_reset_problem_count);
            g_printf("\t\t%-30s: %d\n", "Running Disparity Error Count", running_disparity_error_count);
            available = TRUE;
        }
    }

    if (available == FALSE) {
        g_printf("%s\n", "Unavailable");
    }
    g_variant_unref(array_obj);

    return;
}
LOCAL void print_controller_ddr_ecc_count(guint16 ddr_ecc_count, guint8 type_id)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        return;
    }
    if (ddr_ecc_count == 0xFFFF) {
        g_printf("%-45s : %s\n", "DDR ECC Count", "N/A");
    } else {
        g_printf("%-45s : %d\n", "DDR ECC Count", ddr_ecc_count);
    }
}

LOCAL void print_controller_mem_size(guint16 mem_size)
{
    if (mem_size == 0xFFFF) {
        g_printf("%-45s : %s\n", "Memory Size", "N/A");
    } else {
        g_printf("%-45s : %d MB\n", "Memory Size", mem_size);
    }
}

LOCAL gint32 print_controller_sms_state_related_info(guint8 sms_state, guint8 oob_support,
    const char* controller_drive_version, const char* controller_drive_name)
{
    if (sms_state == SMS_BMA_GLOBAL_STATE_OK) {
        g_printf("%-45s : %s\n", "Controller Driver Name", controller_drive_name);
        g_printf("%-45s : %s\n", "Controller Driver Type", controller_drive_version);
    }
    if ((sms_state == SMS_BMA_GLOBAL_STATE_NOTOK) && (oob_support != TRUE)) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void print_ctrl_boot_devices(const gchar **boot_devices, gsize boot_devices_count, guint8 type_id)
{
    if (boot_devices_count >= 1 && boot_devices[0] != NULL) {
        g_printf("%-45s : %s\n", "Primary Boot Device", boot_devices[0]);
    }
    if (dal_test_controller_vendor(type_id, VENDER_HUAWEI)) {
        return;
    }
    if (boot_devices_count >= 2 && boot_devices[1] != NULL) {  // 至少有2个启动盘
        g_printf("%-45s : %s\n", "Secondary Boot Device", boot_devices[1]);
    }

    return;
}

LOCAL gint32 get_ctrl_properties(OBJ_HANDLE raid_obj, GSList **property_value_list)
{
    GSList *property_name_list = NULL;

    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_TYPE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_OOB_SUPPORT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_FW_VERSION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_NVDT_VERSION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_MEM_SIZE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_SAS_ADDR);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_CACHE_PINNED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_MAINT_PD_FAIL);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_COPYBACK_ENABLED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_SMARTER_COPYBACK_ENABLED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_JBOD_ENABLED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_MIN_STRIP_SUPPORT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_MAX_STRIP_SUPPORT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_ECC_COUNT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_HEALTH_STATUS_CODE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_MODE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_DRIVE_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_DRIVE_VERSION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_CTRL_OPTION3);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_HARDWARE_REVISION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_SPARE_ACTIVATION_MODE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_PCIE_LINK_WIDTH);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_NO_BATTERY_WRITE_CACHE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_READ_CACHE_PERCENT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RAID_CONTROLLER_BOOT_DEVICES);

    gint32 ret = uip_multiget_object_property(CLASS_RAID_CONTROLLER_NAME, dfl_get_object_name(raid_obj),
        property_name_list, property_value_list);
    g_slist_free(property_name_list);

    return ret;
}


void print_raid_controller_info(OBJ_HANDLE raid_obj)
{
    GSList *property_value_list = NULL;
    guint8 sms_state = 0xFF;
    guint8 type_id = 0;

    gint32 ret = get_ctrl_properties(raid_obj, &property_value_list);
    if (ret != RET_OK) {
        g_printf("Get RAID controller information failed.\r\n");
        return;
    }
    guint8 property_index = 0;
    const gchar *controller_name =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    const gchar *controller_type =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    guint8 oob_support = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    const gchar *component_name =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    const gchar *fw_version =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    const gchar *nvdata_version =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    guint16 mem_size = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, property_index++));

    const gchar *sas_addr =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    guint8 cache_pinned = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 maint_pd_fail = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    
    guint8 copyback_enabled = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 smarter_copyback_enabled =
        g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 jbod_enabled = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 min_strip = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 max_strip = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    
    guint16 ddr_ecc_count = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint16 health_code = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 raid_mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));

    
    const gchar *controller_drive_name =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    const gchar *controller_drive_version =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    
    guint32 ctrl_opt3 = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    const gchar *hardware_revision =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    guint8 spare_activation_mode =
        g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 pcie_link_width = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 no_battery_write_cache =
        g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint8 read_cache_percent = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    gsize boot_devices_count = 0;
    const gchar **boot_devices =
        g_variant_get_strv((GVariant *)g_slist_nth_data(property_value_list, property_index++), &boot_devices_count);
    g_printf("%-45s : %s\n", "Controller Name", controller_name);
    g_printf("%-45s : %s\n", "Controller Type", controller_type);
    g_printf("%-45s : %s\n", "Component Name", component_name);
    g_printf("%-45s : %s\n", "Support Out-of-Band Management", (oob_support == TRUE) ? "Yes" : "No");
    (void)dal_get_property_value_byte(raid_obj, PROPERTY_RAID_CONTROLLER_TYPE_ID, &type_id);
    print_supported_raid_level(ctrl_opt3);
    
    // 检测BMA的SMS连接状态
    
    (void)dal_get_bma_connect_state(&sms_state);
    if (print_controller_sms_state_related_info(sms_state, oob_support, controller_drive_version,
        controller_drive_name) != RET_OK) {
        goto exit;
    }
    
    
    g_printf("%-45s : ", "Controller Mode");
    g_printf("%s\n", sml_raid_ctrl_mode_num2str(raid_mode));

    print_controller_health(health_code);
    g_printf("%-45s : %s\n", "Firmware Version", fw_version);
    if (dal_test_controller_vendor(type_id, VENDER_LSI)) {
        g_printf("%-45s : %s\n", "NVDATA Version", nvdata_version);
    }
    if (type_id == PMC_3152_8I_SMART_RAID) {
        g_printf("%-45s : %s\n", "Hardware Revision", hardware_revision);
    }
    print_controller_mem_size(mem_size);

    
    print_controller_device_interface(raid_obj);
    
    g_printf("%-45s : %s\n", "SAS Address", sas_addr);

    print_controller_strip_size_options(min_strip, max_strip);

    print_controller_spare_activation_mode(spare_activation_mode, type_id);
    print_controller_pcie_link_width(pcie_link_width, type_id);
    print_controller_no_battery_write_cache(no_battery_write_cache, type_id);
    print_controller_read_cache_percent(read_cache_percent, type_id);
    print_drive_write_cache_policy(ctrl_opt3, type_id);
	
    print_controller_cache_pinned_info(cache_pinned, type_id);
    if (dal_test_controller_vendor(type_id, VENDER_LSI)) {
        print_controller_maint_pd_fail_info(maint_pd_fail);
    }
    print_controller_properties(copyback_enabled, smarter_copyback_enabled, jbod_enabled, type_id);
    print_controller_ddr_ecc_count(ddr_ecc_count, type_id);
    print_ctrl_boot_devices(boot_devices, boot_devices_count, type_id);
    g_free(boot_devices);

    print_bbu_status(raid_obj);
    if (type_id != PMC_3152_8I_SMART_RAID) {
        print_controller_phy_err_info(raid_obj);
    }
exit:
    uip_free_gvariant_list(property_value_list);
    return;
}


LOCAL void print_logical_drive_status(guint8 drive_status)
{
    g_printf("%-40s : ", "State");

    guint8 level = LEVEL_UNKNOWN;
    const gchar* desc = sml_ld_state2str(drive_status, &level);

    if (level == LEVEL_NORMAL) {
        g_printf("\033[1;32m%s\033[0m\n", desc);  // 浅绿
    } else if (level == LEVEL_WARNING) {
        g_printf("\033[1;33m%s\033[0m\n", desc);  // 黄
    } else if (level == LEVEL_CRITICAL) {
        g_printf("\033[1;31m%s\033[0m\n", desc);  // 浅红
    } else {
        g_printf("%s\n", desc);
    }

    return;
}


LOCAL void print_logical_drive_raid_level(guint8 raid_level)
{
    g_printf("%-40s : ", "Type");

    
    if (raid_level == RAID_LEVEL_1E) {
        g_printf("%s\n", "RAID1(1E)");
    } else if (raid_level == RAID_LEVEL_1ADM) {
        g_printf("%s\n", "RAID1(ADM)");
    } else if (raid_level == RAID_LEVEL_10ADM) {
        g_printf("%s\n", "RAID10(ADM)");
    } else if (raid_level == RAID_LEVEL_1TRIPLE) {
        g_printf("%s\n", "RAID1Triple");
    } else if (raid_level == RAID_LEVEL_10TRIPLE) {
        g_printf("%s\n", "RAID10Triple");
    } else if (raid_level == 0xFF) {
        g_printf("%s\n", "N/A");
    } else {
        g_printf("%s%d\n", "RAID", raid_level);
    }

    return;
}


LOCAL void print_logical_drive_read_policy(guint8 read_policy)
{
    
    switch (read_policy) {
        case LD_CACHE_NO_READ_AHEAD:
            g_printf("%s\n", "No Read Ahead");
            break;

        case LD_CACHE_READ_AHEAD:
            g_printf("%s\n", "Read Ahead");
            break;

        case LD_CACHE_READ_AHEAD_ADAPTIVE:
            g_printf("%s\n", "Read Ahead Adaptive");
            break;

        default:
            g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_write_policy(guint8 write_policy)
{
    
    switch (write_policy) {
        case 0:
            g_printf("%s\n", "Write Through");
            break;

        case 1:
            g_printf("%s\n", "Write Back with BBU");
            break;

        case 2:
            g_printf("%s\n", "Write Back");
            break;

        default:
            g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_cache_policy(guint8 cache_policy)
{
    
    switch (cache_policy) {
        case 0:
            g_printf("%s\n", "Cached IO");
            break;

        case 1:
            g_printf("%s\n", "Direct IO");
            break;

        default:
            g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_access_policy(guint8 cache_policy)
{
    
    g_printf("%-40s : ", "Access Policy");

    switch (cache_policy) {
        case 0:
            g_printf("%s\n", "Read Write");
            break;

        case 1:
            g_printf("%s\n", "Read Only");
            break;

        case 2:
            g_printf("%s\n", "Blocked");
            break;

        case 3:
            g_printf("%s\n", "Hidden");
            break;

        default:
            g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_policy(guint8 type_id, CLI_LD_INFO *ld_info)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        if (ld_info->is_sscd) {
            g_printf("%-40s : ", "Default Write Policy");
            print_logical_drive_write_policy(ld_info->def_write_policy);
            g_printf("%-40s : ", "Current Write Policy");
            print_logical_drive_write_policy(ld_info->cur_write_policy);
        }
    } else if (dal_test_controller_vendor(type_id, VENDER_HUAWEI)) {
        g_printf("%-40s : ", "Default Read Policy");
        print_logical_drive_read_policy(ld_info->def_read_policy);
        g_printf("%-40s : ", "Default Write Policy");
        print_logical_drive_write_policy(ld_info->def_write_policy);
        g_printf("%-40s : ", "Current Read Policy");
        print_logical_drive_read_policy(ld_info->cur_read_policy);
        g_printf("%-40s : ", "Current Write Policy");
        print_logical_drive_write_policy(ld_info->cur_write_policy);
    } else {
        g_printf("%-40s : ", "Default Read Policy");
        print_logical_drive_read_policy(ld_info->def_read_policy);
        g_printf("%-40s : ", "Default Write Policy");
        print_logical_drive_write_policy(ld_info->def_write_policy);
        g_printf("%-40s : ", "Default Cache Policy");
        print_logical_drive_cache_policy(ld_info->def_cache_policy);
        g_printf("%-40s : ", "Current Read Policy");
        print_logical_drive_read_policy(ld_info->cur_read_policy);
        g_printf("%-40s : ", "Current Write Policy");
        print_logical_drive_write_policy(ld_info->cur_write_policy);
        g_printf("%-40s : ", "Current Cache Policy");
        print_logical_drive_cache_policy(ld_info->cur_cache_policy);
        print_logical_drive_access_policy(ld_info->access_policy);
    }

    return;
}

LOCAL void print_logical_drive_span_info(guint8 span_depth, guint8 drive_num_per_span)
{
    g_printf("%-40s : ", "Span depth");

    if (span_depth == 0xFF) {
        g_printf("%s\n", "N/A");
    } else {
        g_printf("%d\n", span_depth);
    }

    g_printf("%-40s : ", "Number of drives per span");

    if (drive_num_per_span == 0xFF) {
        g_printf("%s\n", "N/A");
    } else {
        g_printf("%d\n", drive_num_per_span);
    }

    return;
}


LOCAL void print_logical_drive_strip_size(guint8 strip_size)
{
    g_printf("%-40s : ", "Strip Size");

    if (strip_size == STORAGE_INFO_INVALID_BYTE) {
        g_printf("%s\n", "N/A");
    } else if (strip_size == 0) {
        g_printf("%d B\n", 512);
    } else if (strip_size <= 10) {
        g_printf("%d KB\n", 1 << (strip_size - 1));
    } else {
        g_printf("%d MB\n", (1 << (strip_size - 1)) / 1024);
    }

    return;
}



LOCAL void print_capacity(guint32 size)
{
    guint32 capacity_basic_unit = 1024;

    if (size == STORAGE_INFO_INVALID_DWORD) {
        g_printf("%s\n", "N/A");
    } else if (size < capacity_basic_unit) { // 小于1GB
        g_printf("%d MB\n", size);
    } else if (size < (capacity_basic_unit * capacity_basic_unit)) { // 小于1TB
        g_printf("%.3lf GB\n", ((gdouble)size) / (capacity_basic_unit * 1.0));
    } else if (size < (capacity_basic_unit * capacity_basic_unit * capacity_basic_unit)) { // 小于1PB
        g_printf("%.3lf TB\n", ((gdouble)size) / (capacity_basic_unit * capacity_basic_unit * 1.0));
    } else {
        g_printf("%.3lf PB\n",
            ((gdouble)size) / (capacity_basic_unit * capacity_basic_unit * capacity_basic_unit * 1.0));
    }

    return;
}


LOCAL void print_logical_drive_capacity(guint32 size)
{
    g_printf("%-40s : ", "Total Size");
    print_capacity(size);

    return;
}



LOCAL void print_logical_drive_disk_cache_state(guint8 disk_cache_policy)
{
    
    g_printf("%-40s : ", "Disk Cache Policy");

    switch (disk_cache_policy) {
        case 0:
            g_printf("%s\n", "Disk's Default");
            break;

        case 1:
            g_printf("%s\n", "Enabled");
            break;

        case 2:
            g_printf("%s\n", "Disabled");
            break;

        default:
            g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_cc_state(guint8 cc_state)
{
    g_printf("%-40s : ", "Consistency Checking");

    if (cc_state == 1) {
        g_printf("%s\n", "Yes");
    } else if (cc_state == 0) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_bgi_state(guint8 bgi_state)
{
    g_printf("%-40s : ", "BGI Enabled");

    if (bgi_state == 1) {
        g_printf("%s\n", "Yes");
    } else if (bgi_state == 0) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_drive_bootable(guint8 bootable)
{
    g_printf("%-40s : ", "Bootable");

    if (bootable > 0) {
        g_printf("%s\n", "Yes");
    } else if (bootable == 0) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_drive_boot_priority(guint8 boot_priority)
{
    g_printf("%-40s : ", "Boot Priority");

    switch (boot_priority) {
        case BOOT_PRIORITY_NONE:
            g_printf("%s\n", "None");
            break;
        case BOOT_PRIORITY_PRIMARY:
            g_printf("%s\n", "Primary");
            break;
        case BOOT_PRIORITY_SECONDARY:
            g_printf("%s\n", "Secondary");
            break;
        case BOOT_PRIORITY_ALL:
            g_printf("%s\n", "All");
            break;
        default:
            g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_isSSCD(guint8 isSSCD)
{
    g_printf("%-40s : ", "Used for Secondary Cache");

    if (isSSCD == 1) {
        g_printf("%s\n", "Yes");
    } else if (isSSCD == 0) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    return;
}



LOCAL void print_logical_drive_sscd_caching_enable(guint8 sscd_caching_enable, guint8 isSSCD)
{
    if (isSSCD != 0) {
        return;
    }

    g_printf("%-40s : ", "SSCD Caching Enable");

    if (sscd_caching_enable == 1) {
        g_printf("%s\n", "Yes");
    } else if (sscd_caching_enable == 0) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }

    return;
}


LOCAL void print_logical_drive_sscd_associated_lds(const guint16 *ld_list, guint32 ld_count, guint8 isSSCD)
{
    if (ld_list == NULL || isSSCD != 1) {
        return;
    }

    g_printf("%-40s : ", "SSCD Associated LD List(ID#)");

    if (ld_count == 0) {
        g_printf("%s\n", "N/A");
    } else {
        for (guint32 idx = 0; idx < ld_count; idx++) {
            if (ld_list[idx] == 0xffff) {
                g_printf("%s", "N/A");
            } else {
                g_printf("%d", ld_list[idx]);
            }

            if (idx != ld_count - 1) {
                g_printf(",");
            }
        }

        g_printf("\n");
    }

    return;
}



LOCAL void print_logical_drive_ssdcaching_info(guint8 type_id, CLI_LD_INFO *ld_info, guint32 ref_ld_count)
{
    // 自研卡不涉及ssd caching功能
    if (dal_test_controller_vendor(type_id, VENDER_HUAWEI)) {
        return;
    }

    print_logical_drive_isSSCD(ld_info->is_sscd);
    print_logical_drive_sscd_caching_enable(ld_info->sscd_caching_enable, ld_info->is_sscd);
    print_logical_drive_sscd_associated_lds(ld_info->associated_lds, ref_ld_count, ld_info->is_sscd);
    return;
}



LOCAL void print_logical_drive_spared_pds(OBJ_HANDLE ld_obj, const guint8 *pd_list, const guint16 *pd_enclosure,
    guint32 pd_count)
{
    gchar  ld_ref_raid_obj_name[MAX_NAME_SIZE] = {0};
    
    if (pd_list == NULL || pd_enclosure == NULL) {
        return;
    }
    

    (void)dal_get_property_value_string(ld_obj, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, ld_ref_raid_obj_name,
        sizeof(ld_ref_raid_obj_name));

    g_printf("%-40s : ", "Dedicated Hot Spare PD (ID#)");

    if (pd_count == 0) {
        g_printf("%s\n", "N/A");
    } else {
        for (guint32 idx = 0; idx < pd_count; idx++) {
            guint8 pd_id = INVALID_DATA_BYTE;
            
            dal_get_pd_id_by_slot_and_enclosure(ld_ref_raid_obj_name, pd_list[idx], pd_enclosure[idx], &pd_id);

            
            if (pd_id == INVALID_DATA_BYTE) {
                g_printf("%s", "N/A");
            } else {
                g_printf("%d", pd_id);
            }

            if (idx != pd_count - 1) {
                g_printf(",");
            }
        }

        g_printf("\n");
    }

    return;
}




LOCAL void print_logical_drive_init_state(guint8 init_state)
{
    
    g_printf("%-40s : ", "Init State");

    switch (init_state) {
        case 0:
            g_printf("%s\n", "No Init");
            break;

        case 1:
            g_printf("%s\n", "Quick Init");
            break;

        case 2:
            g_printf("%s\n", "Full Init");
            break;

        default:
            g_printf("%s\n", "N/A");
    }

    return;
}



LOCAL void print_logical_drive_state(guint8 type_id, CLI_LD_INFO *ld_info)
{
    if (dal_test_controller_vendor(type_id, VENDER_PMC)) {
        // 不涉及
    } else if (dal_test_controller_vendor(type_id, VENDER_HUAWEI)) {
        print_logical_drive_disk_cache_state(ld_info->disk_cache_policy);
    } else {
        print_logical_drive_disk_cache_state(ld_info->disk_cache_policy);
        print_logical_drive_init_state(ld_info->init_state);
        print_logical_drive_cc_state(ld_info->cc_state);
        print_logical_drive_bgi_state(ld_info->bgi_state);
    }
    return;
}


LOCAL void print_drive_pds(gchar *ld_ref_raid_obj_name, const guint8 *pd_slot, const guint16 *pd_enclosure,
    guint32 pd_count)
{
    if (ld_ref_raid_obj_name == NULL || pd_slot == NULL || pd_enclosure == NULL) {
        g_printf("%s\n", "N/A");
        return;
    }

    for (guint32 idx = 0; idx < pd_count; idx++) {
        guint8 pd_id = 0xFF;

        
        dal_get_pd_id_by_slot_and_enclosure(ld_ref_raid_obj_name, pd_slot[idx], pd_enclosure[idx], &pd_id);
        
        if (pd_id == 0xFF) {
            g_printf("%s", "N/A");
        } else {
            g_printf("%d", pd_id);
        }

        if (idx != pd_count - 1) {
            g_printf(",");
        }
    }
}


LOCAL void print_logical_drive_pds_with_single_span(OBJ_HANDLE ld_obj, const guint8 *pd_slot,
    const guint16 *pd_enclosure, guint32 pd_count)
{
    gchar  ld_ref_raid_obj_name[MAX_NAME_SIZE] = {0};

    (void)dal_get_property_value_string(ld_obj, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, ld_ref_raid_obj_name,
        sizeof(ld_ref_raid_obj_name));

    g_printf("%-40s : ", "PD participating in LD (ID#)");

    if (pd_slot == NULL || (pd_count == 1 && pd_slot[0] == 0xFF)) {
        g_printf("%s\n", "N/A");
    } else {
        print_drive_pds(ld_ref_raid_obj_name, pd_slot, pd_enclosure, pd_count);
        g_printf("\n");
    }

    return;
}


LOCAL void print_logical_drive_pds_with_multi_span(OBJ_HANDLE ld_obj, const guint16 *array_ref, guint32 array_count,
    guint8 span_depth)
{
    gchar  ld_ref_raid_obj_name[MAX_NAME_SIZE] = {0};
    gchar  array_ref_raid_obj_name[MAX_NAME_SIZE] = {0};
    guint16 array_id = 0xFFFF;
    gsize pd_count = 0;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    GVariant *property_value = NULL;
    GVariant *property_enclosure_value = NULL;
    const guint16 *pd_enclosure_ids = NULL;

    g_printf("%-40s : ", "PD participating in LD (ID#)");

    if (array_ref == NULL || array_ref[0] == 0xFFFF || span_depth == 0xFF) {
        g_printf("%s\n", "N/A");
        return;
    }

    (void)dal_get_property_value_string(ld_obj, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER, ld_ref_raid_obj_name,
        sizeof(ld_ref_raid_obj_name));

    gint32 retval = dal_get_array_list_by_ctrl(ld_ref_raid_obj_name, &obj_list);
    if (retval != DFL_OK) {
        g_printf("%s\n", "N/A");
        return;
    }

    guint32 span_index = 0;
    for (guint32 i = 0; i < array_count; i++) {
        if (array_ref[i] == 0xFFFF) {
            continue;
        }

        for (node = obj_list; node; node = g_slist_next(node)) {
            OBJ_HANDLE array_obj = (OBJ_HANDLE)node->data;
            (void)dal_get_property_value_string(array_obj, PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
                array_ref_raid_obj_name, sizeof(array_ref_raid_obj_name));
            (void)dal_get_property_value_uint16(array_obj, PROPERTY_DISK_ARRAY_ID, &array_id);

            if (strcmp(ld_ref_raid_obj_name, array_ref_raid_obj_name) == 0 && array_ref[i] == array_id) {
                
                retval =
                    dfl_get_property_value(array_obj, PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES, &property_enclosure_value);
                if (retval == DFL_OK) {
                    pd_enclosure_ids = (const guint16 *)g_variant_get_fixed_array(property_enclosure_value, &pd_count,
                        sizeof(guint16));
                }

                retval = dfl_get_property_value(array_obj, PROPERTY_DISK_ARRAY_REF_PD_SLOTS, &property_value);
                if (retval != DFL_OK) {
                    g_printf("\n%-20sSpan%d - Unavailable\n", "", span_index);
                    g_variant_unref(property_enclosure_value);
                    property_enclosure_value = NULL;
                    break;
                }

                const guint8 *pd_slots =
                    (const guint8 *)g_variant_get_fixed_array(property_value, &pd_count, sizeof(guint8));
                if (pd_slots == NULL || pd_enclosure_ids == NULL || (pd_count == 1 && pd_slots[0] == 0xFF)) {
                    g_printf("\n%-20sSpan%d - Unavailable", "", span_index);
                } else {
                    g_printf("\n%-20sSpan%d - ", "", span_index);
                    print_drive_pds(ld_ref_raid_obj_name, pd_slots, pd_enclosure_ids, pd_count);
                }

                g_variant_unref(property_value);
                property_value = NULL;
                g_variant_unref(property_enclosure_value);
                property_enclosure_value = NULL;
                
                span_index++;
            }
        }
    }

    g_printf("\n");

    g_slist_free(obj_list);

    return;
}


LOCAL void print_logical_drive_accelerator(guint8 accelerator)
{
    g_printf("%-40s : ", "Acceleration Method");

    if (accelerator == LD_ACCELERATOR_NONE) {
        g_printf("%s\n", "None");
    } else if (accelerator == LD_ACCELERATOR_CACHE) {
        g_printf("%s\n", "Controller Cache");
    } else if (accelerator == LD_ACCELERATOR_IOBYPASS) {
        g_printf("%s\n", "IO Bypass");
    } else if (accelerator == LD_ACCELERATOR_MAXCACHE) {
        g_printf("%s\n", "maxCache");
    } else {
        g_printf("%s\n", "Unknown");
    }

    return;
}

LOCAL void print_logical_drive_cache_line_size(guint8 cache_line_size, guint8 is_sscd)
{
    if (is_sscd == 0) {
        return;
    }

    g_printf("%-40s : ", "Cache Line Size");

    if (cache_line_size == LD_CACHE_LINE_SIZE_64K) {
        g_printf("%s\n", "64 KB");
    } else if (cache_line_size == LD_CACHE_LINE_SIZE_256K) {
        g_printf("%s\n", "256 KB");
    } else {
        g_printf("%s\n", "Unknown");
    }

    return;
}


LOCAL void print_rebuild_state(guint8 rebuild_state, guint8 rebuild_progress, guint8 type_id, gboolean is_pd)
{
    g_printf("%-40s : ", "Rebuild in Progress");

    if (rebuild_state == 0xFF) {
        g_printf("%s\n", "N/A");
    } else if (rebuild_state == FALSE) {
        g_printf("%s\n", "No");
    } else if (rebuild_state == TRUE) {
        if (rebuild_progress == 0xFF) {
            if (is_pd && dal_test_controller_vendor(type_id, VENDER_PMC)) {
                g_printf("%s\n", "Yes (N/A)");
            } else {
                g_printf("%s\n", "Yes (Unknown Progress)");
            }
        } else {
            g_printf("%s (%d%%)\n", "Yes", rebuild_progress);
        }
    } else {
        g_printf("%s\n", "Unknown");
    }

    return;
}


LOCAL GSList *get_ld_multi_properties(OBJ_HANDLE ld_obj)
{
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_TARGET_ID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_STATUS);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_DEF_READ_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_DEF_WRITE_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_DEF_CACHE_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_CUR_READ_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_CUR_WRITE_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_CUR_CACHE_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_ACCESS_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_DISK_CACHE_POLICY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_CC_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_STRIP_SZIE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_SIZE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_SPAN_DEPTH);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_NUMDRIVE_PER_SPAN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_BGI_ENABLED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_BOOTABLE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_BOOT_PRIORITY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_IS_SSCD);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_REF_ARRAY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_CACHECADE_LD);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_ASSOCIATED_LD);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_SLOT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_INIT_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_OS_DRIVE_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_ENCLOSURE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_ACCELERATION_METHOD);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_CACHE_LINE_SIZE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_REBUILD_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LOGICAL_DRIVE_REBUILDPROGRESS);

    (void)uip_multiget_object_property(CLASS_LOGICAL_DRIVE_NAME, dfl_get_object_name(ld_obj), property_name_list,
        &property_value_list);
    g_slist_free(property_name_list);
    return property_value_list;
}


LOCAL void print_logical_drive_info(OBJ_HANDLE ld_obj, guint8 ctrl_type_id)
{
    gsize pd_eid_count = 0;
    gsize parm_temp = 0;

    GSList *property_value_list = get_ld_multi_properties(ld_obj);
    if (property_value_list == NULL) {
        g_printf("Get logical drive information failed.\r\n");
        return;
    }

    guint8 property_index = 0;
    CLI_LD_INFO ld_info;
    ld_info.target_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.drive_name =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    ld_info.drive_status = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.raid_level = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.def_read_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.def_write_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.def_cache_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.cur_read_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.cur_write_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.cur_cache_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.access_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.disk_cache_policy = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.cc_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.strip_size = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.size = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.span_depth = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.drive_num_per_span =
        g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.bgi_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.bootable = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.boot_priority = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.is_sscd = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.pd_slot = (const guint8 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint8));
    guint32 pd_count = (guint32)parm_temp;
    ld_info.ref_array = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint16));
    guint32 array_count = (guint32)parm_temp;
    
    ld_info.sscd_caching_enable =
        g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.associated_lds = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint16));
    guint32 ref_ld_count = (guint32)parm_temp;
    
    
    ld_info.spared_pd_slots = (const guint8 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint8));
    guint32 spared_pd_count = (guint32)parm_temp;
    

    
    ld_info.init_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    

    
    ld_info.os_letter =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, property_index++), NULL);
    
    
    ld_info.pd_enclosure = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &pd_eid_count, sizeof(guint16));
    ld_info.spared_pd_enclosures = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &pd_eid_count, sizeof(guint16));
    
    ld_info.accelerator = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.cache_line_size = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.rebuild_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    ld_info.rebuild_progress = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    g_printf("%s\n", "Logical Drive Information");
    g_printf("----------------------------------------------------------------------\n");
    g_printf("%-40s : %d\n", "Target ID", ld_info.target_id);
    g_printf("%-40s : %s\n", "Name", ld_info.drive_name);
    print_logical_drive_raid_level(ld_info.raid_level);
    print_logical_drive_status(ld_info.drive_status);
    print_logical_drive_policy(ctrl_type_id, &ld_info);
    print_logical_drive_span_info(ld_info.span_depth, ld_info.drive_num_per_span);

    print_logical_drive_strip_size(ld_info.strip_size);

    
    print_logical_drive_capacity(ld_info.size);
    

    print_logical_drive_state(ctrl_type_id, &ld_info);

    print_drive_bootable(ld_info.bootable);
    print_drive_boot_priority(ld_info.boot_priority);

    print_logical_drive_ssdcaching_info(ctrl_type_id, &ld_info, ref_ld_count);

    
    g_printf("%-40s : %s\n", "OS Letter", ld_info.os_letter);
    

    
    if (ld_info.span_depth == 1) {
        print_logical_drive_pds_with_single_span(ld_obj, ld_info.pd_slot, ld_info.pd_enclosure, pd_count);
    } else {
        print_logical_drive_pds_with_multi_span(ld_obj, ld_info.ref_array, array_count, ld_info.span_depth);
    }

    
    print_logical_drive_spared_pds(ld_obj, ld_info.spared_pd_slots, ld_info.spared_pd_enclosures, spared_pd_count);
    
    
    if (dal_test_controller_vendor(ctrl_type_id, VENDER_PMC)) {
        print_logical_drive_accelerator(ld_info.accelerator);
        print_logical_drive_cache_line_size(ld_info.cache_line_size, ld_info.is_sscd);
        print_rebuild_state(ld_info.rebuild_state, ld_info.rebuild_progress, ctrl_type_id, FALSE);
    }
    g_printf("----------------------------------------------------------------------\n\n");
    uip_free_gvariant_list(property_value_list);
    return;
}


LOCAL gboolean is_deleted_ld(OBJ_HANDLE ld_obj)
{
    guint8 ld_raid_level = 0;

    (void)dal_get_property_value_byte(ld_obj, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL, &ld_raid_level);

    if (ld_raid_level == RAID_LEVEL_DELETED) {
        return TRUE;
    }

    return FALSE;
}

LOCAL void find_all_ld(OBJ_HANDLE raid_obj, GSList *obj_list)
{
    guint8 match_flag = FALSE;
    gchar  ref_raid_obj_name[MAX_NAME_SIZE] = {0};
    guint8 ctrl_type_id = 0;

    (void)dal_get_property_value_byte(raid_obj, PROPERTY_RAID_CONTROLLER_TYPE_ID, &ctrl_type_id);

    for (GSList *node = obj_list; node; node = g_slist_next(node)) {
        if (is_deleted_ld((OBJ_HANDLE)(node->data)) == TRUE) {
            continue;
        }

        gint32 ret = dal_get_property_value_string((OBJ_HANDLE)(node->data), PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            ref_raid_obj_name, sizeof(ref_raid_obj_name));
        
        if (ret == DFL_OK && strcmp(dfl_get_object_name(raid_obj), ref_raid_obj_name) == 0) {
            print_logical_drive_info((OBJ_HANDLE)(node->data), ctrl_type_id);
            match_flag = TRUE;
        }
    }

    if (match_flag == FALSE) {
        g_printf("Not Found Logical Drive.\r\n");
    }

    return;
}

LOCAL void find_specific_ld(OBJ_HANDLE raid_obj, GSList *obj_list, gint32 ld)
{
    guint8 match_flag = FALSE;
    gchar  ref_raid_obj_name[MAX_NAME_SIZE] = {0};
    guint8 ctrl_type_id = 0;
    guint16 target_id = 0;

    (void)dal_get_property_value_byte(raid_obj, PROPERTY_RAID_CONTROLLER_TYPE_ID, &ctrl_type_id);

    for (GSList *node = obj_list; node; node = g_slist_next(node)) {
        if (is_deleted_ld((OBJ_HANDLE)(node->data)) == TRUE) {
            continue;
        }

        gint32 ret = dal_get_property_value_string((OBJ_HANDLE)(node->data), PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            ref_raid_obj_name, sizeof(ref_raid_obj_name));
        
        if (ret == DFL_OK && strcmp(dfl_get_object_name(raid_obj), ref_raid_obj_name) == 0) {
            ret = dal_get_property_value_uint16((OBJ_HANDLE)(node->data), PROPERTY_LOGICAL_DRIVE_TARGET_ID,
                &target_id);
            if (ret == DFL_OK && target_id == ld) { // 指定的logical drive
                print_logical_drive_info((OBJ_HANDLE)(node->data), ctrl_type_id);
                match_flag = TRUE;
                break;
            }
        }
    }

    if (match_flag == FALSE) {
        g_printf("Invalid Logical Drive Target Id : %d\r\n", ld);
    }

    return;
}


void find_logical_drive(OBJ_HANDLE raid_obj, gint32 ld)
{
    GSList *obj_list = NULL;

    gint32 ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Not Found Logical Drive.\r\n");
        return;
    }

    if (ld == -1) {
        find_all_ld(raid_obj, obj_list);
    } else {
        find_specific_ld(raid_obj, obj_list, ld);
    }

    g_slist_free(obj_list);

    return;
}



LOCAL void print_physical_drive_health_status(guint8 health_status)
{
    g_printf("%-40s : ", "Health Status");

    switch (health_status) {
        case 0:  // 0: Normal
            g_printf("\033[1;32m%s\033[0m\n", "Normal");
            break;

        case 1:  // 1: Minor
            g_printf("\033[1;33m%s\033[0m\n", "Minor");
            break;

        case 2:  // 2: Major
            g_printf("\033[1;33m%s\033[0m\n", "Major");
            break;

        case 3:  // 3: Critical
            g_printf("\033[1;31m%s\033[0m\n", "Critical");
            break;

        default:  // 其它: Unknown
            g_printf("%s\n", "Unknown");
            break;
    }

    return;
}



LOCAL void print_physical_drive_fw_status(guint8 fw_status)
{
    
    const gchar* fw_status_string[] = {"UNCONFIGURED GOOD",
                                       "UNCONFIGURED BAD",
                                       "HOT SPARE",
                                       "OFFLINE",
                                       "FAILED",
                                       "REBUILD",
                                       "ONLINE",
                                       "COPYBACK",
                                       "JBOD",
                                       "UNCONFIGURED(Shielded)",
                                       "HOT SPARE(Shielded)",
                                       "CONFIGURED(Shielded)",
                                       "FOREIGN",
                                       "ACTIVE",
                                       "STAND-BY",
                                       "SLEEP",
                                       "DST executing in background",
                                       "SMART Off-line Data Collection executing in background",
                                       "SCT command executing in background",
                                       "ONLINE",
                                       "RAW",
                                       "READY",
                                       "NOT SUPPORTED",
                                       "PREDICTIVE FAILURE",
                                       "DIAGNOSING",
                                       "INCOMPATIBLE",
                                       "ERASE IN PROGRESS"};
    
    g_printf("%-40s : ", "Firmware State");
    
    if (fw_status < sizeof(fw_status_string) / sizeof(fw_status_string[0])) {
        g_printf("%s\n", fw_status_string[fw_status]);
    } else if (fw_status == 255) {
        g_printf("%s\n", "N/A");
    } else {
        g_printf("%s(0x%02X)\n", "UNDEFINED", fw_status);
    }

    return;
}


LOCAL void print_physical_drive_power_state(guint8 power_state)
{
    
    g_printf("%-40s : ", "Power State");

    switch (power_state) {
        case 0:
            g_printf("%s\n", "Spun Up");
            break;

        case 1:
            g_printf("%s\n", "Spun Down");
            break;

        case 2:
            g_printf("%s\n", "Transition");
            break;

        default:
            g_printf("%s\n", "N/A");
            break;
    }

    return;
}


LOCAL void print_physical_drive_media_type(guint8 media_type)
{
    
    g_printf("%-40s : ", "Media Type");

    switch (media_type) {
        case 0:
            g_printf("%s\n", "HDD");
            break;

        case 1:
            g_printf("%s\n", "SSD");
            break;

        case 2:
            g_printf("%s\n", "SSM");
            break;

        default:
            g_printf("%s\n", "N/A");
            break;
    }

    return;
}


LOCAL void print_physical_drive_interface_type(guint8 if_type)
{
    g_printf("%-40s : ", "Interface Type");

    switch (if_type) {
        case PD_INTERFACE_TYPE_UNDEFINED:
            g_printf("%s\n", "Unknown");
            break;

        case PD_INTERFACE_TYPE_PARALLEL_SCSI:
            g_printf("%s\n", "Parallel SCSI");
            break;

        case PD_INTERFACE_TYPE_SAS:
            g_printf("%s\n", "SAS");
            break;

        case PD_INTERFACE_TYPE_SATA:
            g_printf("%s\n", "SATA");
            break;

        case PD_INTERFACE_TYPE_FC:
            g_printf("%s\n", "Fiber Channel");
            break;

            
        case PD_INTERFACE_TYPE_SATA_SAS:
            g_printf("%s\n", "SATA/SAS");
            break;

        case PD_INTERFACE_TYPE_PCIE:
            g_printf("%s\n", "PCIe");
            break;

            
        default:
            g_printf("%s\n", "N/A");
            break;
    }

    return;
}


LOCAL void print_physical_drive_speed(guint8 speed)
{
    
    gchar *speed_string[] = {"1.5 Gbps", "3.0 Gbps", "6.0 Gbps", "12.0 Gbps",
                             "2.5 Gbps", "5.0 Gbps", "8.0 Gbps", "10.0 Gbps",
                             "16.0 Gbps", "20.0 Gbps", "30.0 Gbps", "32.0 Gbps",
                             "40.0 Gbps", "64.0 Gbps", "80.0 Gbps", "96.0 Gbps",
                             "128.0 Gbps", "160.0 Gbps", "256.0 Gbps", "22.5 Gbps", "N/A"};

    if ((speed >= 1) && (speed <= 20)) {
        g_printf("%s\n", speed_string[speed - 1]);
    } else {
        g_printf("%s\n", "N/A");
    }
    return;
}



LOCAL void print_physical_drive_capacity(guint32 capacity)
{
    g_printf("%-40s : ", "Capacity");
    print_capacity(capacity);

    return;
}



LOCAL void print_physical_drive_hot_spare_state(guint8 hot_spare)
{
    g_printf("%-40s : ", "Hot Spare");

    switch (hot_spare) {
        case PD_HOT_SPARE_NONE:
            g_printf("%s\n", "None");
            break;

        case PD_HOT_SPARE_GLOBAL:
            g_printf("%s\n", "Global");
            break;

        case PD_HOT_SPARE_DEDICATED:
            g_printf("%s\n", "Dedicated");
            break;

        case PD_HOT_SPARE_AUTO_REPLACE:
            g_printf("%s\n", "Auto Replace");
            break;

        default:
            g_printf("%s\n", "N/A");
            break;
    }

    return;
}



LOCAL void print_physical_locate_state(guint8 fault, guint8 locate_led)
{
    guint8 locate_state;

    if ((fault == 0) && (locate_led == 1)) {
        locate_state = TRUE; // location started
    } else if ((fault == 0xFF) || (locate_led == 0xFF)) {
        locate_state = 0xFF; // location unknnown
    } else {
        locate_state = FALSE; // location stopped
    }

    
    g_printf("%-40s : ", "Location State");
    

    if (locate_state == 0xFF) {
        g_printf("%s\n", "N/A");
    } else if (locate_state == FALSE) {
        g_printf("%s\n", "Off");
    } else {
        g_printf("%s\n", "On");
    }

    return;
}


LOCAL void print_physical_drive_err_count(OBJ_HANDLE obj_handle)
{
    guint32 media_err_count = 0;
    guint32 prefail_err_count = 0;
    guint32 other_err_count = 0;
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_HDD_MEDIA_ERR_COUNT, &media_err_count);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_HDD_PREFAIL_ERR_COUNT, &prefail_err_count);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_HDD_OTHER_ERR_COUNT, &other_err_count);
    g_printf("%-40s : ", "Media Error Count");

    if (media_err_count != 0xFFFFFFFF) {
        g_printf("%d\n", media_err_count);
    } else {
        g_printf("%s\n", "N/A");
    }

    g_printf("%-40s : ", "Prefail Error Count");

    if (prefail_err_count != 0xFFFFFFFF) {
        g_printf("%d\n", prefail_err_count);
    } else {
        g_printf("%s\n", "N/A");
    }

    g_printf("%-40s : ", "Other Error Count");

    if (other_err_count != 0xFFFFFFFF) {
        g_printf("%d\n", other_err_count);
    } else {
        g_printf("%s\n", "N/A");
    }
    return;
}

LOCAL void print_physical_drive_patrol_state(guint8 patrol_state)
{
    g_printf("%-40s : ", "Patrol Read in Progress");

    if (patrol_state == TRUE) {
        g_printf("%s\n", "Yes");
    } else if (patrol_state == FALSE) {
        g_printf("%s\n", "No");
    } else {
        g_printf("%s\n", "N/A");
    }
    return;
}

void get_raid_controller_type_id(OBJ_HANDLE pd_obj, guint8 *type_id)
{
    gchar raid_controller[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE raid_obj;
    (void)dal_get_property_value_string(pd_obj, PROPERTY_HDD_REF_RAID_CONTROLLER, raid_controller,
                                        sizeof(raid_controller));
    gint32 ret = dfl_get_object_handle(raid_controller, &raid_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s:dfl_get_object_handle raidcontroller fail", __FUNCTION__);
        return;
    }
    (void)dal_get_property_value_byte(raid_obj, PROPERTY_RAID_CONTROLLER_TYPE_ID, type_id);
    return;
}

LOCAL void print_physical_drive_remnant_media_weraout(guint8 remnant_media_wearout)
{
    g_printf("%-40s : ", "Remnant Media Wearout");
    if (remnant_media_wearout == 0xFF) {
        g_printf("%s\n", "N/A");
    } else {
        g_printf("%d%%\n", remnant_media_wearout);
    }
    return;
}


LOCAL void print_physical_drive_boot(guint8 boot_priority)
{
    print_drive_bootable(boot_priority);
    print_drive_boot_priority(boot_priority);
    return;
}


LOCAL void print_physical_drive_power_on_hours(gdouble hours)
{
    g_printf("%-40s : ", "Power-On Hours");
    if (hours >= INVALID_UINT16) {
        g_printf("%s\r\n", "N/A");
    } else {
        g_printf("%d\r\n", (guint16)hours);
    }
}


LOCAL void print_physical_drive_info(OBJ_HANDLE pd_obj)
{
    guint8 id = 0;
    guint8 present = 0;
    guint8 health_status = 0;
    guint8 fw_status = 0;
    guint8 power_state = 0;
    guint8 media_type = 0;
    guint8 interface_type = 0;
    guint8 interface_speed = 0;
    guint8 link_speed = 0;
    guint8 temperature = 0;
    guint32 capacity = 0;
    guint8 hot_spare = 0;
    guint8 rebuild_state = 0;
    guint8 rebuild_progress = 0;
    guint8 patrol_state = 0;
    guint8 remnant_media_wearout = 0;
    gchar device_name[PROP_VAL_LENGTH] = {0};
    gchar sas_addr1[PROP_VAL_LENGTH] = {0};
    gchar sas_addr2[PROP_VAL_LENGTH] = {0};
    gchar sn[PROP_VAL_LENGTH] = {0};
    gchar model[PROP_VAL_LENGTH] = {0};
    gchar fw_version[PROP_VAL_LENGTH] = {0};
    gchar manufacturer[PROP_VAL_LENGTH] = {0};
    guint8 fault = 0;
    guint8 locate_led = 0;
    gdouble power_on_hours = 0xFFFF;
    OBJ_HANDLE obj_handle = 0;
    guint8 type_id = 0;
    guint8 boot_priority = 0xFF;
    

    gint32 ret = dfl_get_object_handle(dfl_get_object_name(pd_obj), &obj_handle);
    if (ret != DFL_OK) {
        g_printf("Get physical drive information failed.\r\n ");
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_ID, &id);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_DEVICENAME, device_name, sizeof(device_name));

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_PRESENCE, &present);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_MANUFACTURER, manufacturer, sizeof(manufacturer));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_SERIAL_NUMBER, sn, sizeof(sn));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_MODEL_NUMBER, model, sizeof(model));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_FIRMWARE_VERSION, fw_version, sizeof(fw_version));

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_HEALTH, &health_status);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_FIRMWARE_STATUS, &fw_status);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_POWER_STATE, &power_state);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_MEDIA_TYPE, &media_type);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_INTERFACE_TYPE, &interface_type);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_INTERFACE_SPEED, &interface_speed);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_LINK_SPEED, &link_speed);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_TEMPERATURE, &temperature);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_HDD_CAPACITY_MB, &capacity);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_HOT_SPARE, &hot_spare);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_REBUILD_STATE, &rebuild_state);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_REBUILDPROGRESS, &rebuild_progress);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_PATROL_STATE, &patrol_state);

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_REMNANT_MEDIA_WEAROUT, &remnant_media_wearout);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_SAS_ADDR1, sas_addr1, sizeof(sas_addr1));

    (void)dal_get_property_value_string(obj_handle, PROPERTY_HDD_SAS_ADDR2, sas_addr2, sizeof(sas_addr2));
    

    
    (void)dal_get_property_value_byte(pd_obj, PROPERTY_HDD_FAULT, &fault);
    (void)dal_get_property_value_byte(pd_obj, PROPERTY_HDD_LOCATION, &locate_led);
    
    
    (void)dal_get_property_value_double(obj_handle, PROPERTY_HDD_HOUR_POWER_UP, &power_on_hours);
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_HDD_BOOT_PRIORITY, &boot_priority);
    get_raid_controller_type_id(pd_obj, &type_id);

    g_printf("%s\n", "Physical Drive Information");
    g_printf("----------------------------------------------------------------------\n");
    g_printf("%-40s : %d\n", "ID", id);
    g_printf("%-40s : %s\n", "Device Name", device_name);

    if (present == 0) {
        g_printf("%s\n", "Drive is not present");
        goto exit;
    }

    g_printf("%-40s : %s\n", "Manufacturer", manufacturer);
    g_printf("%-40s : %s\n", "Serial Number", sn);
    g_printf("%-40s : %s\n", "Model", model);
    g_printf("%-40s : %s\n", "Firmware Version", fw_version);

    print_physical_drive_health_status(health_status);
    print_physical_drive_fw_status(fw_status);
    if (type_id != PMC_3152_8I_SMART_RAID) {
        print_physical_drive_power_state(power_state);
    }
    print_physical_drive_media_type(media_type);
    print_physical_drive_interface_type(interface_type);

    
    g_printf("%-40s : ", "Capable Speed");
    print_physical_drive_speed(interface_speed);
    g_printf("%-40s : ", "Negotiated Speed");
    
    print_physical_drive_speed(link_speed);

    if (temperature != 0xFF) {
        g_printf("%-40s : %d(Celsius)\n", "Drive Temperature", temperature);
    } else {
        g_printf("%-40s : %s\n", "Drive Temperature", "N/A");
    }

    
    print_physical_drive_capacity(capacity);
    

    print_physical_drive_hot_spare_state(hot_spare);
    print_rebuild_state(rebuild_state, rebuild_progress, type_id, TRUE);
    if (type_id != PMC_3152_8I_SMART_RAID) {
        print_physical_drive_patrol_state(patrol_state);
    }
    print_physical_drive_remnant_media_weraout(remnant_media_wearout);
    print_physical_drive_power_on_hours(power_on_hours);

    g_printf("%-40s : %s\n", "SAS Address(0)", sas_addr1);
    g_printf("%-40s : %s\n", "SAS Address(1)", sas_addr2);

    
    print_physical_locate_state(fault, locate_led);
    

    print_physical_drive_boot(boot_priority);

    g_printf("\n");
    if (type_id != PMC_3152_8I_SMART_RAID) {
        print_physical_drive_err_count(pd_obj);
    }
exit:
    g_printf("----------------------------------------------------------------------\n\n");
    return;
}


void find_physical_drive(gint32 pd_id)
{
    guint8 pd_id_prop = 0;
    guint8 match_flag = FALSE;
    GSList *node = NULL;
    GSList *obj_list = NULL;
    guint8 presence = 0;

    gint32 ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    if (ret != RET_OK || obj_list == NULL) {
        g_printf("Not Found Physical Drive.\r\n");
        return;
    }
    if (pd_id == -1) {
        for (node = obj_list; node; node = g_slist_next(node)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_HDD_PRESENCE, &presence);
            if (ret == RET_OK && presence == 1) {
                print_physical_drive_info((OBJ_HANDLE)(node->data));
                match_flag = TRUE;
            }
        }

        if (match_flag == FALSE) {
            g_printf("Not Found Physical Drive.\r\n");
        }
    } else {
        for (node = obj_list; node; node = g_slist_next(node)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_HDD_ID, &pd_id_prop);
            if (ret == RET_OK && pd_id_prop == pd_id) { // 指定的physical drive
                match_flag = TRUE;
                ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_HDD_PRESENCE, &presence);
                if (ret == RET_OK && presence == 0) {
                    g_printf("The physical drive is not in position.\r\n");
                }

                print_physical_drive_info((OBJ_HANDLE)(node->data));
                break;
            }
        }

        if (match_flag == FALSE) {
            g_printf("Invalid Physical Drive ID : %d\r\n", pd_id);
        }
    }

    g_slist_free(obj_list);

    return;
}



LOCAL void print_array_space_info(guint32 used_space, guint32 free_space, const guint32 *free_block, gsize n_element)
{
    g_printf("%-40s : ", "Used Space");
    print_capacity(used_space);

    g_printf("%-40s : ", "Free Space");
    print_capacity(free_space);

    g_printf("%-40s : ", "Free Blocks Space");
    if (free_block == NULL) {
        g_printf("N/A\n");
        return;
    }

    for (gsize i = 0; i < n_element; i++) {
        if (i > 0) {
            g_printf("%-40s : ", "");
        }
        g_printf("(%" G_GSIZE_FORMAT ")", i);
        print_capacity(free_block[i]);
    }

    return;
}


LOCAL void print_array_ldinfo(OBJ_HANDLE array_obj, const guint16 *lds, guint32 ld_count)
{
    g_printf("%-40s : ", "Logcial Drive(s) ID");

    if (ld_count == 0) {
        g_printf("%s\n", "None");
    } else if (lds == NULL || ld_count == 0xFF || (ld_count == 1 && lds[0] == 0xFFFF)) {
        g_printf("%s\n", "N/A");
    } else {
        for (guint32 idx = 0; idx < ld_count; idx++) {
            g_printf("%d", lds[idx]);

            if (idx != ld_count - 1) {
                g_printf(",");
            }
        }

        g_printf("\n");
    }
}


LOCAL void print_array_pdinfo(OBJ_HANDLE array_obj, const guint8 *pd_slots, const guint16 *pd_enclosures,
    guint32 pd_count)
{
    gchar  ref_ctrl_obj_name[MAX_NAME_SIZE] = {0};

    (void)dal_get_property_value_string(array_obj, PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER, ref_ctrl_obj_name,
        sizeof(ref_ctrl_obj_name));

    g_printf("%-40s : ", "Physical Drive(s) ID");

    
    if (pd_count == 0) {
        g_printf("%s\n", "None");
    } else if (pd_slots == NULL || pd_enclosures == NULL || pd_count == 0xFF ||
        (pd_count == 1 && pd_slots[0] == 0xFF)) {
        g_printf("%s\n", "N/A");
    } else {
        for (guint32 idx = 0; idx < pd_count; idx++) {
            guint8 pd_id = 0xFF;

            dal_get_pd_id_by_slot_and_enclosure(ref_ctrl_obj_name, pd_slots[idx], pd_enclosures[idx], &pd_id);

            
            if (pd_id == 0xFF) {
                g_printf("%s", "N/A");
            } else {
                g_printf("%d", pd_id);
            }

            if (idx != pd_count - 1) {
                g_printf(",");
            }
        }

        g_printf("\n");
    }
}


LOCAL void print_disk_array_info(OBJ_HANDLE array_obj)
{
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    gsize parm_temp = 0;
    gsize n_element = 0;
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_ID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_USED_SPACE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_TOTAL_FREE_SPACE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_FREE_BLOCKS_SPACE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_LD_COUNT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_REF_LDS);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_PD_COUNT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_REF_PD_SLOTS);
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES);

    gint32 ret = uip_multiget_object_property(CLASS_DISK_ARRAY_NAME, dfl_get_object_name(array_obj), property_name_list,
        &property_value_list);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get disk array information failed.\r\n");
        return;
    }

    guint8 property_index = 0;
    guint16 array_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint32 used_space = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    guint32 free_space = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    const guint32 *free_block = (const guint32 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &n_element, sizeof(guint32));
    guint8 ld_count = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    const guint16 *lds = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint16));
    guint32 ld_element = (guint32)parm_temp;
    guint8 pd_count = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, property_index++));
    const guint8 *pd_slots = (const guint8 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint8));
    const guint16 *pd_enclosure_ids = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(property_value_list, property_index++), &parm_temp, sizeof(guint16));
    guint32 pd_element = (guint32)parm_temp;
    g_printf("%s\n", "Disk Array Information");
    g_printf("----------------------------------------------------------------------\n");
    g_printf("%-40s : %d\n", "Array ID", array_id);

    print_array_space_info(used_space, free_space, free_block, n_element);
    print_array_ldinfo(array_obj, lds, MIN(ld_count, ld_element));
    print_array_pdinfo(array_obj, pd_slots, pd_enclosure_ids, MIN(pd_count, pd_element));
    
    g_printf("----------------------------------------------------------------------\n\n");
    uip_free_gvariant_list(property_value_list);

    return;
}


void find_disk_array(OBJ_HANDLE raid_obj, gint32 array_id)
{
    guint16 array_ref = 0;
    gchar  ref_raid_obj_name[MAX_NAME_SIZE] = {0};
    guint8 match_flag = FALSE;
    GSList *node = NULL;
    GSList *obj_list = NULL;

    gint32 ret = dfl_get_object_list(CLASS_DISK_ARRAY_NAME, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Not Found Disk Array.\r\n");
        return;
    }

    if (array_id == -1) {
        for (node = obj_list; node; node = g_slist_next(node)) {
            ret = dal_get_property_value_string((OBJ_HANDLE)(node->data), PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
                ref_raid_obj_name, sizeof(ref_raid_obj_name));
            
            if (ret == RET_OK && strcmp(dfl_get_object_name(raid_obj), ref_raid_obj_name) == 0) {
                print_disk_array_info((OBJ_HANDLE)(node->data));
                match_flag = TRUE;
            }
        }

        if (match_flag == FALSE) {
            g_printf("Not Found Disk Array.\r\n");
        }
    } else {
        for (node = obj_list; node; node = g_slist_next(node)) {
            ret = dal_get_property_value_string((OBJ_HANDLE)(node->data), PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
                ref_raid_obj_name, sizeof(ref_raid_obj_name));
            
            if (ret == RET_OK && strcmp(dfl_get_object_name(raid_obj), ref_raid_obj_name) == 0) {
                ret = dal_get_property_value_uint16((OBJ_HANDLE)(node->data), PROPERTY_DISK_ARRAY_ID, &array_ref);
                if (ret == RET_OK && array_ref == array_id) { // 指定的DiskArray
                    print_disk_array_info((OBJ_HANDLE)(node->data));
                    match_flag = TRUE;
                    break;
                }
            }
        }

        if (match_flag == FALSE) {
            g_printf("Invalid Disk Array ID : %d\r\n", array_id);
        }
    }

    g_slist_free(obj_list);

    return;
}