
#include "resource_channel.h"
#include "bios.h"
#include "im_export_config.h"
#include "smbios.h"


#define SMBIOS_VERSION_MAX_LEN 48
#define SMBIOS_SKUNUMBER_MAX_LEN 48
#define SMBIOS_FAMILY_MAX_LEN 48



#define DEFAULT_REGISTRY_VERSION "V0.00"
#define HAD_CLEAR_CLP_CONFIG_FILE 1
#define NO_CLEAR_CLP_CONFIG_FILE 0

#define BIOS_GLOBAL_VAR_IV_FORMAT ("The data_addr of GlobalVarible(%s) is null!\n")
#define BIOS_SMBIOS_WR_VAR_NAME ("g_smbios_wr_data_cfg")


#define SMBIOS_MAXDATA_LEN (248 - 11)

LOCAL void clear_clp_config_file_from_bt(void);
LOCAL gint32 smbios_write_finish(const void *request_msg, guint8 *resp_data, guint32 resp_data_len);
LOCAL gint32 smbios_write_data(const void *request_msg, guint8 *resp_data, guint32 resp_data_size);
LOCAL gint32 smbios_write_prepare(const void *request_msg, guint8 *resp_data, gint32 resp_data_len);
void update_smbios_status(guint8 value);
#ifdef ARM64_HI1711_ENABLED
LOCAL void save_backup_bios_version(void);
#endif


LOCAL gulong g_smbios_status_async = 0;
LOCAL GMutex g_smbios_wr_data_cfg_mutex;

void init_smbios_wr_data_cfg_mutex(void)
{
    g_mutex_init(&g_smbios_wr_data_cfg_mutex);
}

LOCAL void __attribute__((constructor)) main_init(void)
{
    (void)vos_thread_binary_sem4_create(&g_smbios_status_async, "biorst", 1UL);
}


gint32 smbios_set_version(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t safe_fun_ret = EOK;
    gint32 retv = 0;
    GVariant *list_value = NULL;
    const gchar *pversion_str = NULL;
    guint32 version_len = 0;
    gchar version_str[SMBIOS_VERSION_MAX_LEN + 1] = {0};
    gsize parm_temp = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios version failed\r\n");
        return RET_ERR;
    }

    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios version failed\r\n");
        return RET_ERR;
    }

    pversion_str = g_variant_get_string(list_value, &parm_temp);
    version_len = (guint32)parm_temp;
    if (version_len > SMBIOS_VERSION_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error: len is %d", __FUNCTION__, version_len);
        method_operation_log(caller_info, NULL, "Set smbios version failed\r\n");
        return RET_ERR;
    }

    
    if (pversion_str == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios version failed\r\n");
        return RET_ERR;
    }
    

    retv = vos_check_incorrect_char(pversion_str, "", strlen(pversion_str));
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "vos_check_incorrect_char(%s) failed", pversion_str);
        method_operation_log(caller_info, NULL, "Set smbios version failed\r\n");
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(version_str, sizeof(version_str), pversion_str, sizeof(version_str) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    retv = dal_set_property_value_string(obj_handle, PROPERTY_SMBIOS_VERSION, version_str, DF_SAVE);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_set_property_value_string smbios version error, ret=%d", retv);
        method_operation_log(caller_info, NULL, "Set smbios version(%s) failed\r\n", version_str);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set smbios version(%s) successfully\r\n", version_str);

    return retv;
}


gint32 smbios_set_skunumber(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t safe_fun_ret = EOK;
    gint32 retv = 0;
    GVariant *list_value = NULL;
    const gchar *pskunumber_str = NULL;
    guint32 skunumber_len = 0;
    gchar skunumber_str[SMBIOS_SKUNUMBER_MAX_LEN + 1] = {0};
    gsize parm_temp = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios skunumber failed\r\n");
        return RET_ERR;
    }

    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios skunumber failed\r\n");
        return RET_ERR;
    }

    pskunumber_str = g_variant_get_string(list_value, &parm_temp);
    skunumber_len = (guint32)parm_temp;
    
    if (pskunumber_str == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios skunumber failed\r\n");
        return RET_ERR;
    }
    

    if (skunumber_len > SMBIOS_SKUNUMBER_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error: len is %d", __FUNCTION__, skunumber_len);
        method_operation_log(caller_info, NULL, "Set smbios skunumber failed\r\n");
        return RET_ERR;
    }

    retv = vos_check_incorrect_char(pskunumber_str, "", strlen(pskunumber_str));
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "vos_check_incorrect_char(%s) failed", pskunumber_str);
        method_operation_log(caller_info, NULL, "Set smbios skunumber failed\r\n");
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(skunumber_str, sizeof(skunumber_str), pskunumber_str, skunumber_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    retv = dal_set_property_value_string(obj_handle, PROPERTY_SMBIOS_SKUNUMBER, skunumber_str, DF_SAVE);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_set_property_value_string smbios skunumber error, ret=%d", retv);
        method_operation_log(caller_info, NULL, "Set smbios skunumber(%s) failed\r\n", skunumber_str);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set smbios skunumber(%s) successfully\r\n", skunumber_str);

    return retv;
}


gint32 smbios_set_family(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t safe_fun_ret = EOK;
    gint32 retv = 0;
    GVariant *list_value = NULL;
    const gchar *pfamily_str = NULL;
    guint32 family_len = 0;
    gchar family_str[SMBIOS_FAMILY_MAX_LEN + 1] = {0};
    gsize parm_temp = 0;

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input_list is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios family failed\r\n");
        return RET_ERR;
    }

    list_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (list_value == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios family failed\r\n");
        return RET_ERR;
    }

    pfamily_str = g_variant_get_string(list_value, &parm_temp);
    family_len = (guint32)parm_temp;
    
    if (pfamily_str == NULL) {
        debug_log(DLOG_ERROR, "%s:list_value is NULL", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set smbios family failed\r\n");
        return RET_ERR;
    }
    

    if (family_len > SMBIOS_FAMILY_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:input parameter range error: len is %d", __FUNCTION__, family_len);
        method_operation_log(caller_info, NULL, "Set smbios family failed\r\n");
        return RET_ERR;
    }

    retv = vos_check_incorrect_char(pfamily_str, "", strlen(pfamily_str));
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "vos_check_incorrect_char(%s) failed", pfamily_str);
        method_operation_log(caller_info, NULL, "Set smbios family failed\r\n");
        return RET_ERR;
    }

    safe_fun_ret = strncpy_s(family_str, sizeof(family_str), pfamily_str, family_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    retv = dal_set_property_value_string(obj_handle, PROPERTY_SMBIOS_FAMILY, family_str, DF_SAVE);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_set_property_value_string smbios family error, ret=%d", retv);
        method_operation_log(caller_info, NULL, "Set smbios family(%s) failed\r\n", family_str);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set smbios family(%s) successfully\r\n", family_str);

    return retv;
}


gint32 bios_clear_smbios(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = FM_OK;

    ret = fm_clear_file(SMBIOS_FILE_NAME);
    if (ret != FM_OK) {
        debug_log(DLOG_ERROR, "%s %d:fm_clear_file , ret = %d\n", __FUNCTION__, __LINE__, ret);
        method_operation_log(caller_info, NULL, "Clear CMOS failed");
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Clear CMOS successfully");

    return RET_OK;
}


gint32 smbios_set_smbios_status_default_value(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 iflag = 0;
    guint8 value = 1;
    
    guint8 temp_data = 0;
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    

    static guint8 power_state_first_callback = TRUE;

    value = g_variant_get_byte(property_value);
    if (!strcmp(property_name, PROPERTY_PAYLOAD_CHASSPWR_STATE)) {
        
        if (value == 0) {
            clear_clp_config_file_from_bt();
        }
        
        
        if (power_state_first_callback == TRUE) {
            power_state_first_callback = FALSE;
            return RET_OK;
        }

        
        // cold reset flag, 0 is power off
        iflag = (value == 0) ? 1 : 0;
    } else if (!strcmp(property_name, PROPERTY_PAYLOAD_RESET_FLAG)) {
        // warm reset flag, 1 is warm reset
        iflag = (value == 1) ? 1 : 0;
    } else if (!strcmp(property_name, PROPERTY_PAYLOAD_SYS_RST_FLAG)) {
        // os reboot flag, 1 is reboot
        iflag = (value == 1) ? 1 : 0;
#ifdef ARM64_HI1711_ENABLED
        if (value == 0x1) { 
            update_bios_bootguard_status();
        }
#endif
    } else {
        return RET_OK;
    }

    if (iflag == 1) {
        // 系统下电或复位时会检查更新Registry.json版本号
        (void)bios_registry_version_update(TRUE);

        if (vos_thread_async_sem4_p(g_smbios_status_async) == 0) {
            
            update_bios_startup_state(BIOS_STARTUP_STATE_OFF);
            
            update_smbios_status(SMBIOS_WRITE_NOT_START);

            
            
            ret = dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "Get Class Bios obj fail ret_val = %d", ret);
            }

            property_data = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &temp_data, 1, sizeof(guint8));

            
            ret = dfl_set_property_value(obj_handle, PROPERTY_BIOS_PCIECARD_BDF, property_data, DF_NONE);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "clear bios  PcieCardBDF fail!result = %d\n", ret);
            }

            ret = dfl_set_property_value(obj_handle, PROPERTY_BIOS_PCIEDISK_BDF, property_data, DF_NONE);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "clear bios  PcieDiskBDF fail!result = %d\n", ret);
            }
            

            g_variant_unref(property_data);
            

            
            (void)bios_set_boot_stage(BIOS_BOOT_STAGE_OFF);
            debug_log(DLOG_INFO, "%s: restore bios boot stage to default\n", __FUNCTION__);
            
            (void)vos_thread_sem4_v(g_smbios_status_async);
        }
    }

    return RET_OK;
}


LOCAL void clear_clp_config_file_from_bt(void)
{
    OBJ_HANDLE bios_handle = 0;
    guchar power_state = 0;
    gint32 ret = RET_OK;

    if (is_clp_config_file_from_bt() > 0) {
        ret = dal_get_func_ability(CLASS_CHASSISPAYLOAD, PROPERTY_PAYLOAD_CHASSPWR_STATE, &power_state);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "get Payload.ChassisPowerState failed\n");
            return;
        }

        if (power_state != 0) {
            debug_log(DLOG_INFO, "Payload.ChassisPowerState is not OFF, will not clear CLP config file.\n");
            return;
        }

        ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "get bios handle failed\n");
            return;
        }

        clear_clp_config_file(bios_handle, TRUE);
        set_clp_config_bt_flag(CLP_FILE_FROM_HMM, TRUE);
    }
}


gint32 bios_registry_version_update(guchar is_check_data_file)
{
    gint32 ret = 0;
    gchar registry_file_name[MAX_FILEPATH_LENGTH] = {0};
    gchar registry_version[BIOS_CONFIG_VALUE_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get object handle for Bios fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_string(obj_handle, BIOS_FILE_REGISTRY_NAME, registry_file_name,
        sizeof(registry_file_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get RegistryVersion property value fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    if (strlen(registry_file_name) == 0) {
        debug_log(DLOG_DEBUG, "%s : RegistryFileName is null", __FUNCTION__);
        return RET_OK;
    }

    
    if (is_check_data_file == TRUE) {
        
        ret = vos_get_file_accessible(registry_file_name);
        if (ret != TRUE) {
            debug_log(DLOG_DEBUG, "%s : registry.json not existed in flash", __FUNCTION__);

            ret = dal_set_property_value_string(obj_handle, BIOS_REGISTRY_VERSION, DEFAULT_REGISTRY_VERSION,
                DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s : set RegistryVersion default value fail, ret = %d", __FUNCTION__, ret);
            }
            return ret;
        }
    }

    
    ret = bios_read_version_from_registry(BIOS_FILE_REGISTRY_NAME, registry_version, BIOS_CONFIG_VALUE_LEN);
    
    if (ret != RET_OK || strlen(registry_version) == 0) {
        debug_log(DLOG_ERROR, "%s : Failed to read version from registry.json, ret = %d", __FUNCTION__, ret);

        ret = dal_set_property_value_string(obj_handle, BIOS_REGISTRY_VERSION, DEFAULT_REGISTRY_VERSION,
            DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : set RegistryVersion default value fail, ret = %d", __FUNCTION__, ret);
        }
        return ret;
    }

    
    ret = dal_set_property_value_string(obj_handle, BIOS_REGISTRY_VERSION, registry_version, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : dal_set_property_value_string fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "%s : Bios RegistryVersion update ok", __FUNCTION__);

    return RET_OK;
}


void update_smbios_status(guint8 value)
{
    gint32 result = RET_OK;
    OBJ_HANDLE handle = 0;
    GSList *handle_list = NULL;
    guchar had_update_smbios_flag = NO_ONCE_UPDATE_SMBIOS;

    result = dfl_get_object_list(SMBIOS_CLASS_NAME, &handle_list);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SMBios object handle failed.");
        return;
    }

    handle = (OBJ_HANDLE)(handle_list->data);
    g_slist_free(handle_list);

    
    (void)dal_set_property_value_byte(handle, PROPERTY_SMBIOS_STATUS_VALUE, value, DF_SAVE_TEMPORARY);

    
    if (SMBIOS_WRITE_FINISH == value) {
        strategy_log(SLOG_INFO, "Update SMBios Finished");
        (void)dal_get_property_value_byte(handle, PROPERTY_SMBIOS_UPDATED_FLAG, &had_update_smbios_flag);

        if (had_update_smbios_flag == NO_ONCE_UPDATE_SMBIOS) {
            (void)dal_set_property_value_byte(handle, PROPERTY_SMBIOS_UPDATED_FLAG, UPDATED_SMBIOS_AT_LEASE_ONCE,
                DF_SAVE_TEMPORARY);
        }

#ifdef ARM64_HI1711_ENABLED
        
        save_backup_bios_version();
#endif
    }

    return;
}



gint32 bios_set_boot_stage(guint8 flag)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] Get Class Bios obj failed ret_val = %d", __FUNCTION__, ret);
        return ret;
    }

    debug_log(DLOG_INFO, "[%s] BiosBootStage change to [%d].\n", __FUNCTION__, flag);
    // BMC重启时需要保留原先的设定值
    gchar accessor_name[MAX_NAME_SIZE] = {0};
    ret = dfl_get_property_accessor(obj_handle, BIOS_START_OPTION_NAME, accessor_name, sizeof(accessor_name));
    if (ret == DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: StartOption is accessor", __FUNCTION__);
        return RET_OK;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_BOOT_STAGE, flag, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] set BiosBootStage fail, result = %d\n", __FUNCTION__, ret);
    }

    return ret;
}



void clear_clp_config_file(OBJ_HANDLE obj_handle, guint8 is_force)
{
    OBJ_HANDLE product_handle = 0;
    OBJ_HANDLE bmc_handle = 0;
    gint32 ret = 0;
    guint32 product_id = 0;
    gchar tmp_buf[10] = {0};
    FM_FILE_S *file_handle = NULL;
    gint32 reset_cause = 0;
    guchar clear_clp_flag = HAD_CLEAR_CLP_CONFIG_FILE;

    if (obj_handle == 0) {
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product_handle);
    if (ret != RET_OK) {
        return;
    }

    (void)dal_get_property_value_uint32(product_handle, PROPERTY_PRODUCT_ID, &product_id);

    // 只有OSCA才需要删除无状态计算相关的配置文件
    if (PRODUCT_TYPE_BLADE != product_id) {
        return;
    }

    if (is_force == FALSE) {
        ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &bmc_handle);
        if (ret != RET_OK) {
            return;
        }

        
        (void)dal_get_property_value_int32(bmc_handle, BMC_RESET_CAUSE_NAME, &reset_cause);

        
        if (RESET_TYPE_POWER_UP != reset_cause) {
            return;
        }
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_CLEAR_CLP_FLAG, &clear_clp_flag);

    
    
    if ((clear_clp_flag == NO_CLEAR_CLP_CONFIG_FILE) || (TRUE == is_force)) {
        file_handle = fm_fopen(PROPERTY_BIOS_CLP_NAME, "w+b");
        if (file_handle == NULL) {
            
            debug_log(DLOG_ERROR, "open %s file fail!\r\n", PROPERTY_BIOS_CLP_NAME);
            return;
        }

        if (fm_fwrite(tmp_buf, BIOS_FILE_STREAM_SIZE, 1, file_handle) != 1) {
            
            debug_log(DLOG_ERROR, "fm_fwrite file fail!\n");
            fm_fclose(file_handle);
            return;
        }

        if (is_force == TRUE) {
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Clear the old ClpConfig file due to PowerState is OFF\n");
        } else {
            maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Clear the old ClpConfig file due to AC restore\n");
        }
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_CLEAR_CLP_FLAG, HAD_CLEAR_CLP_CONFIG_FILE,
            DF_SAVE_TEMPORARY);
        fm_fclose(file_handle);
    }
}


void set_clp_config_bt_flag(guint8 val, guint8 lock)
{
    if (lock == TRUE) {
        g_mutex_lock(&g_clpconfig_mutex);
    }
    g_is_clpconfig_from_bt = val;
    per_save(&g_is_clpconfig_from_bt);
    if (lock == TRUE) {
        g_mutex_unlock(&g_clpconfig_mutex);
    }
}


gint32 bios_write_smbios_data(const void *req_msg, gpointer user_data)
{
    gint32 ret_val;
    guint8 bios_id = 0;
    guint8 sub_cmd = 0;
    guint8 resp_data[10];
    const guint8 *src_data = NULL;
    guint32 smbios_offset = 0xFFFFFFFF;
    guint8 data_len;

    
    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    src_data = get_ipmi_src_data(req_msg);
    data_len = get_ipmi_src_data_len(req_msg);
    
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    

    
    ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    
    bios_id = src_data[BIOS_ID_OFFSET];

    if (bios_id > BIOS_ID) {
        debug_log(DLOG_ERROR, "BiosId:%d(MaxId:%d) is invalid!\n", bios_id, BIOS_ID);

        resp_data[0] = SMBIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    sub_cmd = src_data[SMBIOS_SUB_CMD_OFFSET];

    
    switch (sub_cmd) {
        case SMBIOS_WRITE_PREPARE:

            // 1)write prepare
            update_smbios_status(SMBIOS_WRITE_PREPARE);
            
            update_bios_startup_state(BIOS_STARTUP_SMBIOS_WRITE_PREPARE);
            
            return smbios_write_prepare(req_msg, resp_data, sizeof(resp_data));

        case SMBIOS_WRITE_DATA:

            // 2)write data
            
            if (data_len < SMBIOS_DATA_OFFSET_POS) {
                debug_log(DLOG_ERROR, "%s:Invalid cmd len:%u", __FUNCTION__, data_len);
                resp_data[0] = BIOS_ERR_IV_LEN;
                return construt_and_respond_msg(req_msg, resp_data, sizeof(resp_data));
            }
            smbios_offset = MAKE_DWORD(src_data[SMBIOS_FILE_OFFSET_START + 3], src_data[SMBIOS_FILE_OFFSET_START + 2],
                src_data[SMBIOS_FILE_OFFSET_START + 1], src_data[SMBIOS_FILE_OFFSET_START]);
            if (smbios_offset == 0) { // 只有在第一帧时更新SMBIOS数据状态
                update_smbios_status(SMBIOS_WRITE_DATA);
                
                update_bios_startup_state(BIOS_STARTUP_SMBIOS_WRITE_DATA);
                
            }

            

            return smbios_write_data(req_msg, resp_data, sizeof(resp_data));

        case SMBIOS_WRITE_FINISH:

            // 3)write finish
            update_smbios_status(SMBIOS_WRITE_FINISH);
            
            update_bios_startup_state(BIOS_STARTUP_SMBIOS_WRITE_FINISH);
            
            return smbios_write_finish(req_msg, resp_data, sizeof(resp_data));
            

        default:
            resp_data[0] = SMBIOS_ERR_INVALID_STATUS;
            return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }
}


LOCAL gint32 smbios_write_finish(const void *request_msg, guint8 *resp_data, guint32 resp_data_len)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_data = NULL;
    BIOS_DATA_OPERATE_S *data_operate = &g_smbios_wr_data_cfg;

    if (request_msg == NULL || resp_data == NULL || resp_data_len < BIOS_MSG_HEAD_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s: input false parameters.", __FUNCTION__);
        return RET_ERR;
    }

    g_mutex_lock(&g_smbios_wr_data_cfg_mutex);
    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, BIOS_GLOBAL_VAR_IV_FORMAT, BIOS_SMBIOS_WR_VAR_NAME);
        
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return ipmi_send_simple_response(request_msg, COMP_CODE_INVALID_FIELD);
        
    }

    
    guint16 check_sum = vos_verify_check_sum(data_operate->data_buf, data_operate->data_offset);

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");

        resp_data[0] = SMBIOS_ERR_INVALID_CHKSUM;
        goto bios_quit0;
    }
    guint8 src_len = get_ipmi_src_data_len(request_msg);
    if (src_len < SMBIOS_CHECK_NUM_OFFSET + 1) {
        debug_log(DLOG_ERROR, "%s:Invalid cmd length(%u)", __FUNCTION__, src_len);
        resp_data[0] = BIOS_ERR_IV_LEN;
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    if ((guint8)check_sum != src_data[SMBIOS_CHECK_NUM_OFFSET]) {
        debug_log(DLOG_ERROR, "CheckSum(CalcSum:%d,RxSum:%d) is error!\n", check_sum,
            src_data[SMBIOS_CHECK_NUM_OFFSET]);

        resp_data[0] = SMBIOS_ERR_INVALID_CHKSUM;
        goto bios_quit0;
    }

    
    // 2.1.获取通过对象属性获取文件名，但文件名是固定的. 或者
    // 2.2。指定目录，动态创建文件名，动态注册文件管理
    FM_FILE_S *file_handle = fm_fopen(SMBIOS_FILE_NAME, "w+b");
    if (file_handle == NULL) {
        
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", SMBIOS_FILE_NAME);
        resp_data[0] = SMBIOS_ERR_FM_FAIL;
        goto bios_quit0;
    }

    guint32 data_size =
        fm_fwrite(data_operate->data_buf, BIOS_FILE_STREAM_SIZE, data_operate->data_offset, file_handle);
    if (data_operate->data_offset != data_size) {
        
        debug_log(DLOG_ERROR, "write file(InLen:%d,OutLen:%d) fail!\n", data_operate->data_offset, data_size);

        fm_fclose(file_handle);

        resp_data[0] = SMBIOS_ERR_FM_FAIL;
        goto bios_quit0;
    }

    
    fm_fflush(file_handle);

    
    fm_fclose(file_handle);

    
    gint32 ret_val = dfl_get_binded_object(SMBIOS_CLASS_NAME, BIOS_OBJECT_SMBIOS_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);

        resp_data[0] = SMBIOS_ERR_INVALID_STATUS;
        goto bios_quit0;
    }

    ret_val = dfl_get_property_value(obj_handle, SMBIOS_CHANGE_FLAG, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        resp_data[0] = SMBIOS_ERR_INVALID_STATUS;
        goto bios_quit0;
    }

    guint8 cur_falg = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    property_data = g_variant_new_byte(SMBIOS_FILE_ALREADY_CHANGE == cur_falg ? 0 : SMBIOS_FILE_ALREADY_CHANGE);
    ret_val = dfl_set_property_value(obj_handle, SMBIOS_CHANGE_FLAG, property_data, DF_NONE);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        goto bios_quit1;
    }

    
    resp_data[0] = SMBIOS_ERR_NORMALLY;

    debug_log(DLOG_DEBUG, "recv smbios data from bios starting ok\n");

bios_quit1:
    g_variant_unref(property_data);

bios_quit0:

    
    clear_data_operate_res(data_operate);
    g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);

    return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}


LOCAL gint32 smbios_write_data(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    if (request_msg == NULL || resp_data == NULL || resp_data_size < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    g_mutex_lock(&g_smbios_wr_data_cfg_mutex);
    BIOS_DATA_OPERATE_S *data_operate = &g_smbios_wr_data_cfg;
    if (BIOS_FLAG_DOING != data_operate->data_flag) {
        
        debug_log(DLOG_ERROR, "DataFlag:%d(ShouldBe:%d) is invalid!\n", data_operate->data_flag, BIOS_FLAG_DOING);

        resp_data[0] = SMBIOS_ERR_INVALID_STATUS;

        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);

        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = SMBIOS_ERR_INVALID_CHKSUM;
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    guint32 smbios_offset = MAKE_DWORD(src_data[SMBIOS_FILE_OFFSET_START + 3], src_data[SMBIOS_FILE_OFFSET_START + 2],
        src_data[SMBIOS_FILE_OFFSET_START + 1], src_data[SMBIOS_FILE_OFFSET_START]);
    if (data_operate->data_offset != smbios_offset) {
        if (BIOS_ERR_OFFSET_MSG_LEN > resp_data_size) {
            g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
            return RET_ERR;
        }
        debug_log(DLOG_ERROR, "DataOffset:%d(ShouldBe:%d) is invalid!\n", smbios_offset, data_operate->data_offset);

        resp_data[0] = SMBIOS_ERR_WRONG_OFFSET;
        resp_data[4] = LONGB0(data_operate->data_offset);
        resp_data[5] = LONGB1(data_operate->data_offset);
        resp_data[6] = LONGB2(data_operate->data_offset);
        resp_data[7] = LONGB3(data_operate->data_offset);
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return construt_and_respond_msg(request_msg, resp_data, BIOS_ERR_OFFSET_MSG_LEN);
    }

    

    
    guint32 data_real_len = get_ipmi_src_data_len(request_msg) - SMBIOS_DATA_OFFSET_POS;
    
    if ((data_real_len == 0) || (data_real_len > SMBIOS_MAXDATA_LEN) ||
        ((data_operate->data_offset + data_real_len) > SMBIOS_DATA_SIZE)) {
        debug_log(DLOG_ERROR, "RealLen:%d(Max:%d,DataOffset:%d) is invalid!\n", data_real_len, SMBIOS_MAXDATA_LEN,
            data_operate->data_offset);

        resp_data[0] = SMBIOS_ERR_IV_LEN;
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    guint16 check_sum = vos_verify_check_sum((const guint8 *)&src_data[SMBIOS_DATA_OFFSET_POS], data_real_len);
    if ((guint8)check_sum != src_data[SMBIOS_CHECK_NUM_OFFSET]) {
        debug_log(DLOG_ERROR, "CheckSum(CalcSum:%d,RxSum:%d) is error!\n", check_sum,
            src_data[SMBIOS_CHECK_NUM_OFFSET]);

        resp_data[0] = SMBIOS_ERR_INVALID_CHKSUM;
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    debug_log(DLOG_DEBUG, "Offset:%d,DataLen:%d!\n", data_operate->data_offset, data_real_len);

    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, BIOS_GLOBAL_VAR_IV_FORMAT, BIOS_SMBIOS_WR_VAR_NAME);
        
        g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
        return ipmi_send_simple_response(request_msg, COMP_CODE_INVALID_FIELD);
        
    }

    
    for (guint32 i = 0; i < data_real_len; i++) {
        *(data_operate->data_buf + data_operate->data_offset + i) = src_data[SMBIOS_DATA_OFFSET_POS + i];
    }

    data_operate->data_offset += data_real_len;
    g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);

    resp_data[0] = SMBIOS_ERR_NORMALLY;
    return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}


LOCAL gint32 smbios_write_prepare(const void *request_msg, guint8 *resp_data, gint32 resp_data_len)
{
    BIOS_DATA_OPERATE_S *data_operate = &g_smbios_wr_data_cfg;

    if (request_msg == NULL) {
        return RET_ERR;
    }
    if (resp_data == NULL) {
        return RET_ERR;
    }
    if (resp_data_len < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    g_mutex_lock(&g_smbios_wr_data_cfg_mutex);
    data_operate->data_flag = BIOS_FLAG_DOING;
    data_operate->data_offset = 0;

    
    if (data_operate->data_buf != NULL) {
        g_free(data_operate->data_buf);
        data_operate->data_buf = NULL;
    }

    resp_data[0] = SMBIOS_ERR_NORMALLY;

    data_operate->data_buf = (guint8 *)g_malloc0(SMBIOS_DATA_SIZE);

    if (data_operate->data_buf == NULL) {
        data_operate->data_flag = BIOS_FLAG_IDLE;
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!\n", SMBIOS_DATA_SIZE);
        resp_data[0] = SMBIOS_ERR_INVALID_STATUS;
    }

    g_mutex_unlock(&g_smbios_wr_data_cfg_mutex);
    return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}


gint32 judge_manu_id_valid_bios(const guint8 *manufacture_id)
{
    guint32 manu_id;
    guint32 property_manu_id = g_manufacture_id_bios;

    if (manufacture_id == NULL) {
        debug_log(DLOG_ERROR, "%s: manufacture_id is NULL", __FUNCTION__);
        return RET_ERR;
    }

    manu_id = MAKE_DWORD(0, manufacture_id[2], manufacture_id[1], manufacture_id[0]);
    if (manu_id != property_manu_id) {
        debug_log(DLOG_ERROR, "ManufactureId:%d(ShouldBe:%d) is invalid!\n", manu_id, property_manu_id);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 construt_and_respond_msg(const void *req_msg, guint8 *pdata, gint32 data_len)
{
    guint32 manu_id = g_manufacture_id_bios;

    if (req_msg == NULL) {
        return RET_ERR;
    }
    if (pdata == NULL) {
        return RET_ERR;
    }

    
    pdata[1] = LONGB0(manu_id);
    pdata[2] = LONGB1(manu_id);
    pdata[3] = LONGB2(manu_id);

    if (data_len > IPMB_MSG_MAX_PAYLOAD_LEN) {
        data_len = IPMB_MSG_MAX_PAYLOAD_LEN;
    }

    return ipmi_send_response(req_msg, data_len, pdata);
}


void clear_data_operate_res(BIOS_DATA_OPERATE_S *data_operate_addr)
{
    if (data_operate_addr == NULL) {
        debug_log(DLOG_ERROR, "%s: data_operate_addr is NULL", __FUNCTION__);
        return;
    }

    data_operate_addr->data_flag = BIOS_FLAG_IDLE;
    data_operate_addr->data_len = 0;
    data_operate_addr->data_offset = 0;

    

    if (data_operate_addr->data_buf != NULL) {
        g_free(data_operate_addr->data_buf);

        data_operate_addr->data_buf = NULL;
    }

    return;
}

#ifdef ARM64_HI1711_ENABLED

LOCAL void save_backup_bios_version(void)
{
    OBJ_HANDLE obj_handle = 0;
    gchar version[BIOS_VERSION_LEN] = {0};
    gchar backup_version[BIOS_VERSION_LEN] = {0};
    gint32 ret_val = 0;
    GVariant *property_data = NULL;

    ret_val = dfl_get_binded_object(BIOS_CLASS_NAME, BIOS_OBJECT_HANDLE_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return;
    }

    ret_val = dal_get_property_value_string(obj_handle, BMC_BIOS_VER_NAME, version, BIOS_VERSION_LEN);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "get version err %d", ret_val);
        return;
    }

    ret_val = dal_get_property_value_string(obj_handle, BIOS_BACKUP_VER_NAME, backup_version, BIOS_VERSION_LEN);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "get backup version err %d", ret_val);
        return;
    }

    if (strcmp(version, backup_version) == 0) {
        // 一样不需要备份
        return;
    }

    if (strcmp(version, "000") == 0) {
        return;
    }

    property_data = g_variant_new_string(version);
    if (property_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return;
    }

    ret_val = dfl_set_property_value(obj_handle, BIOS_BACKUP_VER_NAME, property_data, DF_SAVE);
    if (ret_val != DFL_OK) {
        g_variant_unref(property_data);
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return;
    }
    g_variant_unref(property_data);

    debug_log(DLOG_ERROR, "Set Backup BIOS version to (%s)", version);
    return;
}
#endif