

#include "pme_app/pme_app.h"
#include "ui_manage_proxy_method.h"
#include "snmp_manage_app.h"
#include "pme_app/dalib/dal_m3_msg_intf.h"
#include "pme_app/kmc/kmc.h"


LOCAL SEMID g_kvm_vmm_encrypt_state_sem;


G_LOCK_DEFINE(g_revert_owner_list_sync);

const gchar* file_path_white_list[] = {
    TMP_PATH_WITH_SLASH,
    DEV_SHM_PATH_WITH_SLASH,
    SCREEN_FILE_PATH,
    FM_FACTORY_RECOVER_POINT_PATH,
    INSTALLABLE_BMA_PATH,
    VIDEO_PATH_CATERROR,
    ORIGIN_KEYSTORE_UPGRADE,
    SP_DEVICE_INFO_BACKUP_FILE_PATH,
    SP_OSINSTALL_PARA_PATH,
    RECORD_FILE,
    DATA_LOG_FILE_PATH
};


LOCAL gboolean kvm_vmm_port_check(guint16 port, guchar dex)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *p_input_list = NULL;
    gint32 retv = 0;
    guchar port_index = dex;

    retv = dfl_get_object_handle(SSH_SERVER_PORT_OBJ_NAME, &obj_handle); 
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "Can't find object %s. Ret is %d\n", "SSH", retv);
        return RET_ERR;
    }

    p_input_list = g_slist_append(p_input_list, g_variant_new_uint16(port));
    p_input_list = g_slist_append(p_input_list, g_variant_new_byte(port_index));
    retv = dfl_call_class_method(obj_handle, SERVER_PORT_METHOD_PORT_CHECK, NULL, p_input_list, NULL);
    g_slist_free_full(p_input_list, (GDestroyNotify)g_variant_unref);

    if (retv != DFL_OK) {
        return RET_ERR;
    }

    return RET_OK;
}

#if defined(COMPATIBILITY_ENABLED) || defined(SECURITY_ENHANCED_COMPATIBLE_BOARD_V1)
LOCAL gboolean kvm_vmm_connect_check(void)
{
#define KVM_IS_IN_USE 0xff
    gint32 ret = 0;
    gint32 connect = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;

    ret = dfl_get_object_handle(KVM_OBJ_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get kmm object failed.\n");
        return RET_ERR;
    }

    ret = dfl_get_property_value(obj_handle, KVM_PROPERTY_NUM, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get kmm connect failed.\n");
        return RET_ERR;
    }

    connect = g_variant_get_int32(property_value);
    g_variant_unref(property_value);

    
    if (connect > 0) {
        
        
        return KVM_IS_IN_USE;
        
    }

    return RET_OK;
}
#endif

gint32 proxy_method_set_kvm_enable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guchar state = 0;
    GVariant *property_value = NULL;
    guchar KVMEnable = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set KVM service state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &KVMEnable);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_KVMEnable_status fail.\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, "KVM", "Set KVM service state failed");
        return ret;
    }
    if (KVMEnable == SERVICE_STATE_DISABLE) {
        method_operation_log(caller_info, "KVM", "Set KVM service state failed");
        return COMP_CODE_INVALID_CMD;
    }
    
    

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    state = g_variant_get_byte(property_value);
    
    if ((state != SERVICE_OFF) && (state != SERVICE_ON)) {
        method_operation_log(caller_info, "KVM", "Set KVM service state failed");
        debug_log(DLOG_ERROR, "Parameter is out of range.\n");
        return RET_ERR;
    }

    
    ret = dfl_set_property_value(object_handle, KVM_PROPERTY_STATE, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set kvm service state failed: %d\n", ret);
        ret = RET_ERR;
        goto out;
    }

out:
    method_operation_log(caller_info, "KVM", "%s KVM service %s", (state == SERVICE_OFF) ? "Disable" : "Enable",
        (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}

gint32 proxy_method_set_kvm_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gint32 port = 0;
    gint32 old_port = 0;
    GVariant *property_value = NULL;
    GVariant *old_property_value = 0;
    guchar KVMEnable = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set KVM service port failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &KVMEnable);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_KVMEnable_status fail.\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, "KVM", "Set KVM service port failed");
        return ret;
    }
    if (KVMEnable == SERVICE_STATE_DISABLE) {
        method_operation_log(caller_info, "KVM", "Set KVM service port failed");
        return COMP_CODE_INVALID_CMD;
    }
    
    

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    port = g_variant_get_int32(property_value);
    
    if ((port > PORT_MAX_VALUE) || (port < PORT_MIN_VALUE)) {
        method_operation_log(caller_info, "KVM", "Set KVM service port failed");
        debug_log(DLOG_ERROR, "KVM port value is out of range.\n");
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(object_handle, KVM_PROPERTY_PORT, &old_property_value);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, "KVM", "Set KVM service port failed");
        debug_log(DLOG_ERROR, "Get KVM port value failed.\n");
        return RET_ERR;
    }

    old_port = g_variant_get_int32(old_property_value);
    g_variant_unref(old_property_value);
    old_property_value = NULL;

    
    if (port == old_port) {
        ret = RET_OK;
        goto out;
    }

    
    ret = kvm_vmm_port_check((guint16)port, KVM_SERVICE_PORT_INDEX);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "kvm_port_check failed\n");
        ret = RET_ERR;
        goto out;
    }

    ret = dfl_set_property_value(object_handle, KVM_PROPERTY_PORT, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set kvm port failed: %d.\n", ret);
    }

out:
    method_operation_log(caller_info, "KVM",
        "Set KVM service port to (%d) "
        "%s",
        port, (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}

gint32 proxy_method_set_screen_switch(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gchar v_switch = 0;
    guint8 save_flag = TRUE;
    GVariant *property_value = NULL;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set screen switch state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    v_switch = g_variant_get_byte(property_value);
    
    if ((v_switch != SERVICE_OFF) && (v_switch != SERVICE_ON)) {
        method_operation_log(caller_info, "KVM", "Set screen switch state failed");
        debug_log(DLOG_ERROR, "Set screen switch state error.\n");
        return RET_ERR;
    }

    if (g_slist_length(input_list) == 2) {
        property_value = (GVariant *)g_slist_nth_data(input_list, 1);
        save_flag = g_variant_get_byte(property_value);
        if (save_flag != TRUE && save_flag != FALSE) {
            method_operation_log(caller_info, "KVM", "Set screen switch state failed");
            debug_log(DLOG_ERROR, "Set screen switch state error.\n");
            return RET_ERR;
        }
    }

    
    ret = dfl_set_property_value(object_handle, SCREEN_PROPERTY_SWITCH, property_value,
        (save_flag == TRUE) ? DF_SAVE : DF_NONE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set screen switch state failed!\r\n");
        goto out;
    }

out:
    method_operation_log(caller_info, "KVM", "%s screen switch %s", (v_switch == SERVICE_OFF) ? "Disable" : "Enable",
        (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}

#ifdef COMPATIBILITY_ENABLED
gint32 proxy_method_set_kvm_encrypt_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 compress = 0;
    gint32 ret = 0;
    GVariant *property_value = NULL;
    gint32 vmm_compress_state = 0;
    OBJ_HANDLE vmm_handle = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set KVM encryption state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    compress = g_variant_get_int32(property_value);
    
    if ((compress != ENCRYPT_STATE_OFF) && (compress != ENCRYPT_STATE_ON)) {
        method_operation_log(caller_info, "KVM", "Set KVM encryption state failed");
        return RET_ERR;
    }

    
    ret = kvm_vmm_connect_check();
    if (ret != RET_OK) {
        method_operation_log(caller_info, "KVM", "%s KVM encryption failed", compress ? "Enable" : "Disable");
        return ret;
    }

    
    ret = dfl_get_object_handle(PROXY_VMM_OBJ_NAME, &vmm_handle);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, "KVM", "Set KVM encryption state failed");
        debug_log(DLOG_ERROR, "Get vmm object failed.\n");
        return RET_ERR;
    }

    (void)vos_thread_sem4_p(g_kvm_vmm_encrypt_state_sem, SEM_WAIT_FOREVER);

    ret = dal_get_property_value_int32(vmm_handle, VMM_PROPERTY_COMPRESS_STATE, &vmm_compress_state);
    if (ret != RET_OK) {
        (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
        method_operation_log(caller_info, "KVM", "Set KVM encryption state failed");
        debug_log(DLOG_ERROR, "Get vmm encryption state failed.\n");
        return RET_ERR;
    }

    
    
    if ((vmm_compress_state == ENCRYPT_STATE_ON) && (compress == ENCRYPT_STATE_OFF)) {
        (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
        method_operation_log(caller_info, "KVM", "Disable KVM encrypt failed");
        return RELY_ERR;
    }
    

    
    ret = dfl_set_property_value(object_handle, KVM_PROPERTY_COMPRESS_STATE, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
        method_operation_log(caller_info, "KVM", "%s KVM encryption failed", compress ? "Enable" : "Disable");
        return ret;
    }

    (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
    method_operation_log(caller_info, "KVM", "%s KVM encryption successfully", compress ? "Enable" : "Disable");
    return ret;
}
#endif

gint32 proxy_method_set_kvm_timeout(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 timeout = 0;
    gint32 ret = 0;
    GVariant *property_value = NULL;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set KVM timeout failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    timeout = g_variant_get_int32(property_value);
    
    if ((timeout < KVM_TIMEOUT_MIN_VALUE) || (timeout > KVM_TIMEOUT_MAX_VALUE)) {
        method_operation_log(caller_info, "KVM", "Set KVM timeout to (%d) minutes failed", timeout);
        return RET_ERR;
    }

    
    ret = dfl_set_property_value(object_handle, KVM_PROPERTY_TIMEOUT, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, "KVM", "Set KVM timeout to (%d) minutes failed", timeout);
        return RET_ERR;
    }

    method_operation_log(caller_info, "KVM", "Set KVM timeout to (%d) minutes successfully", timeout);
    return RET_OK;
}

gint32 proxy_method_set_local_kvm_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GVariant *property_value = NULL;
    guchar state = 0;
    gint32 ret = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set local kvm state  failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    state = g_variant_get_byte(property_value);
    
    if ((state != 0) && (state != 1)) {
        method_operation_log(caller_info, "KVM", "Set local kvm state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    ret = set_vga_display_status(state);
    if (ret != RET_OK) {
        method_operation_log(caller_info, "KVM", "Set local kvm %s failed", state ? "Enabled" : "Disabled");
        debug_log(DLOG_ERROR, "set_vga_display_status fail.\n");
        return RET_ERR;
    }

    ret = dal_set_property_value_byte(object_handle, KVM_PROPERTY_LOCAL_KVM_STATE, state, DF_SAVE);
    if (ret != RET_OK) {
        method_operation_log(caller_info, "KVM", "Set local kvm %s failed", state ? "Enabled" : "Disabled");
        debug_log(DLOG_ERROR, "dal_set_property_value_byte fail.\n");
        return RET_ERR;
    }
    
    method_operation_log(caller_info, "KVM", "Set local kvm %s successfully", state ? "Enabled" : "Disabled");
    
    return RET_OK;
}

gint32 proxy_method_set_vmm_enable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guchar state = 0;
    GVariant *property_value = NULL;
    guchar KVMEnable = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set VMM service state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &KVMEnable);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_KVMEnable_status fail.\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, "KVM", "Set VMM service state failed");
        return ret;
    }
    if (KVMEnable == SERVICE_STATE_DISABLE) {
        method_operation_log(caller_info, "KVM", "Set VMM service state failed");
        return COMP_CODE_INVALID_CMD;
    }
    
    

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    state = g_variant_get_byte(property_value);
    
    if ((state != SERVICE_OFF) && (state != SERVICE_ON)) {
        method_operation_log(caller_info, "KVM", "Set VMM service state failed");
        debug_log(DLOG_ERROR, "Parameter is out of range.\n");
        return RET_ERR;
    }

    
    ret = dfl_set_property_value(object_handle, VMM_PROPERTY_STATE, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set vmm service state failed: %d.\n", ret);
        ret = RET_ERR;
        goto out;
    }

out:
    method_operation_log(caller_info, "KVM", "%s VMM service %s", (state == SERVICE_OFF) ? "Disable" : "Enable",
        (ret == RET_OK) ? "successfully" : "failed");

    return RET_OK;
}

typedef enum _tagPROXY_VMM_KO_TYPE_E {
    PROXY_VMM_KO_TYPE_INS_STO = 0x00,
    PROXY_VMM_KO_TYPE_INS_FP = 0x01,
    PROXY_VMM_KO_TYPE_RM_STO = 0x10,
    PROXY_VMM_KO_TYPE_RM_FP = 0x11,
    PROXY_VMM_KO_TYPE_UNKNOW
} PROXY_VMM_KO_TYPE_E;


LOCAL gint32 driver_operation_get_ko_type(GSList *input_list, guint8 *ko_type)
{
    gint32 ret = RET_OK;
    GVariant *property_value = NULL;
    if (input_list == NULL) {
        debug_log(DLOG_DEBUG, "[%s] failed with input_list is NULL\n", __FUNCTION__);
        return RET_ERR;
    }
    
    if (g_slist_length(input_list) != 1) {
        debug_log(DLOG_DEBUG, "[%s][%d] failed with input_list length is not 1\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }
    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    if (property_value == NULL) {
        debug_log(DLOG_DEBUG, "[%s] failed with propertye_value is NULL\n", __FUNCTION__);
        return RET_ERR;
    }
    *ko_type = g_variant_get_byte(property_value);
    return ret;
}

gint32 proxy_method_driver_operation(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 timeout_cnt = 0;
    guint8 ko_type = 0;
    ret = driver_operation_get_ko_type(input_list, &ko_type);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s] failed with get ko type\n", __FUNCTION__);
        return RET_ERR;
    }
    switch (ko_type) {
        case PROXY_VMM_KO_TYPE_INS_STO: // 加载u盘驱动
            if (vos_get_file_accessible("/dev/sto_drv") == FALSE) {
                if (vos_get_file_accessible("/dev/fp_drv") == TRUE) {
                    // 已存在floppy驱动，需要先卸载之
                    gchar *cmd_vir_fp[3] = {"/sbin/rmmod", "virtual_fp", NULL};
                    (void)vos_system_s("/sbin/rmmod", cmd_vir_fp);
                    gchar *cmd_otg_fp[3] = {"/sbin/rmmod", "dwc_otg_fp", NULL};
                    (void)vos_system_s("/sbin/rmmod", cmd_otg_fp);
                }
                gchar *cmd_otg_ko[3] = {"/sbin/insmod", "/opt/pme/lib/modules/ko/dwc_otg_sto.ko", NULL};
                (void)vos_system_s("/sbin/insmod", cmd_otg_ko);
                gchar *cmd_sto_ko[3] = {"/sbin/insmod", "/opt/pme/lib/modules/ko/virtual_sto.ko", NULL};
                (void)vos_system_s("/sbin/insmod", cmd_sto_ko);
                vos_task_delay(100);
            }
            // 检查 改变驱动、块设备所有者为本模块所有
            while (chown("/dev/sto_drv", KVM_USER_UID, KVM_USER_GID)) {
                vos_task_delay(20);
                timeout_cnt++;
                if (timeout_cnt > 50) {
                    debug_log(DLOG_ERROR, "Set ums drv owner failed.\r\n");
                    return RET_ERR;
                }
            }
            return RET_OK;

        case PROXY_VMM_KO_TYPE_INS_FP: // 加载软驱驱动
            if (vos_get_file_accessible("/dev/fp_drv") == FALSE) {
                if (vos_get_file_accessible("/dev/sto_drv") == TRUE) {
                    // 已存在floppy驱动，需要先卸载之
                    gchar *cmd_vir_sto[3] = {"/sbin/rmmod", "virtual_sto", NULL};
                    (void)vos_system_s("/sbin/rmmod", cmd_vir_sto);
                    gchar *cmd_otg_sto[3] = {"/sbin/rmmod", "dwc_otg_sto", NULL};
                    (void)vos_system_s("/sbin/rmmod", cmd_otg_sto);
                }
                gchar *cmd_fp_ko[3] = {"/sbin/insmod", "/opt/pme/lib/modules/ko/dwc_otg_fp.ko", NULL};
                (void)vos_system_s("/sbin/insmod", cmd_fp_ko);
                gchar *cmd_vir_ko[3] = {"/sbin/insmod", "/opt/pme/lib/modules/ko/virtual_fp.ko", NULL};
                (void)vos_system_s("/sbin/insmod", cmd_vir_ko);
                vos_task_delay(100);
            }
            // 检查 改变驱动、块设备所有者为本模块所有
            while (chown("/dev/fp_drv", KVM_USER_UID, KVM_USER_GID)) {
                vos_task_delay(20);
                timeout_cnt++;
                if (timeout_cnt > 50) {
                    debug_log(DLOG_ERROR, "Set floppy drv owner failed.\r\n");
                    return RET_ERR;
                }
            }
            return RET_OK;

        case PROXY_VMM_KO_TYPE_RM_STO: // 卸载u盘驱动
            if (vos_get_file_accessible("/dev/sto_drv") == TRUE) {
                gchar *cmd_vir_sto[3] = {"/sbin/rmmod", "virtual_sto", NULL};
                (void)vos_system_s("/sbin/rmmod", cmd_vir_sto);
                gchar *cmd_otg_sto[3] = {"/sbin/rmmod", "dwc_otg_sto", NULL};
                (void)vos_system_s("/sbin/rmmod", cmd_otg_sto);
            }
            return RET_OK;

        case PROXY_VMM_KO_TYPE_RM_FP: // 卸载软驱驱动
            if (vos_get_file_accessible("/dev/fp_drv") == TRUE) {
                gchar *cmd_vir_fp[3] = {"/sbin/rmmod", "virtual_fp", NULL};
                (void)vos_system_s("/sbin/rmmod", cmd_vir_fp);
                gchar *cmd_otg_fp[3] = {"/sbin/rmmod", "dwc_otg_fp", NULL};
                (void)vos_system_s("/sbin/rmmod", cmd_otg_fp);
            }
            return RET_OK;

        default:
            debug_log(DLOG_DEBUG, "ko_type error:  %d unknown\n", ko_type);
            return RET_ERR;
    }
}

gint32 proxy_method_format_device(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    const gchar *format_type = NULL;
    const gchar *dst_device = NULL;
    gchar           dst_dev_buf[MAX_FILEPATH_LENGTH] = { 0 };
    gsize tmp_len = 0;

    gchar*           argv[8] = { NULL };
    sig_t old_handler;

    if (input_list == NULL) {
        debug_log(DLOG_DEBUG, "[%s] failed with input_list is NULL\n", __FUNCTION__);
        return RET_ERR;
    }
    
    if (g_slist_length(input_list) != 2) {
        debug_log(DLOG_DEBUG, "[%s]failed with input_list length is not 1\n", __FUNCTION__);
        return RET_ERR;
    }
    format_type = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), &tmp_len);
    dst_device = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), &tmp_len);
    if (strncpy_s(dst_dev_buf, sizeof(dst_dev_buf), dst_device, strlen(dst_device)) != EOK) {
        debug_log(DLOG_DEBUG, "[%s] strncpy_s failed\n", __FUNCTION__);
        return RET_ERR;
    }
    if (strcmp(format_type, "vfat") == 0) {
        argv[0] = "/sbin/mkfs.vfat";
        argv[1] = "-I";
        argv[2] = "-F";
        argv[3] = "32";
        argv[4] = "-n";
        argv[5] = "HUAWEISP";
        argv[6] = dst_dev_buf;
        argv[7] = NULL;
        old_handler = signal(SIGCHLD, SIG_DFL);
        ret = vos_system_s("/sbin/mkfs.vfat", argv);
        (void)signal(SIGCHLD, old_handler);
    } else if (strcmp(format_type, "ext4") == 0) {
        argv[0] = "/sbin/mkfs.ext4";
        // RTOS207.5需要添加-F参数以避免交互式使用mkfs
        argv[1] = "-F";
        
        argv[2] = "-O";
        argv[3] = "^metadata_csum,^64bit";
        argv[4] = dst_dev_buf;
        argv[5] = NULL;
        old_handler = signal(SIGCHLD, SIG_DFL);
        ret = vos_system_s("/sbin/mkfs.ext4", argv);
        (void)signal(SIGCHLD, old_handler);
    } else {
        ret = RET_ERR;
        debug_log(DLOG_DEBUG, "[%s]failed to mkfs", __FUNCTION__);
    }

    return ret ? RET_ERR : RET_OK;
}

gint32 proxy_method_set_vmm_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gint32 port = 0;
    gint32 old_port = 0;
    GVariant *old_property_value = 0;
    GVariant *property_value = NULL;
    guchar KVMEnable = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set VMM service port failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &KVMEnable);
    
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_KVMEnable_status fail.\n", __FUNCTION__, __LINE__);
        method_operation_log(caller_info, "KVM", "Set VMM service port failed");
        return ret;
    }
    if (KVMEnable == SERVICE_STATE_DISABLE) {
        method_operation_log(caller_info, "KVM", "Set VMM service port failed");
        return COMP_CODE_INVALID_CMD;
    }
    
    

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    port = g_variant_get_int32(property_value);
    
    if ((port > PORT_MAX_VALUE) || (port < PORT_MIN_VALUE)) {
        
        method_operation_log(caller_info, "KVM", "Set VMM service port failed");
        debug_log(DLOG_ERROR, "VMM port value is out of range.\n");
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(object_handle, VMM_PROPERTY_PORT, &old_property_value);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, "KVM", "Set VMM service port failed");
        debug_log(DLOG_ERROR, "Get video port value failed.\n");
        return RET_ERR;
    }

    old_port = g_variant_get_int32(old_property_value);
    g_variant_unref(old_property_value);
    old_property_value = NULL;

    
    if (port == old_port) {
        ret = RET_OK;
        goto out;
    }

    
    ret = kvm_vmm_port_check((guint16)port, VMM_SERVICE_PORT_INDEX);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "kvm_vmm_port_check failed\n");
        ret = RET_ERR;
        goto out;
    }

    ret = dfl_set_property_value(object_handle, VMM_PROPERTY_PORT, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set vmm port failed: %d.\n", ret);
    }

out:
    method_operation_log(caller_info, "KVM",
        "Set VMM service port to (%d) "
        "%s",
        port, (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
gint32 proxy_method_set_vmm_encrypt_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 compress = 0;
    gint32 ret = 0;
    GVariant *property_value = NULL;
    gint32 kvm_compress_state = 0;
    OBJ_HANDLE kvm_obj_handle = 0;

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set VMM encryption state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    compress = g_variant_get_int32(property_value);
    
    if ((compress != ENCRYPT_STATE_OFF) && (compress != ENCRYPT_STATE_ON)) {
        method_operation_log(caller_info, "KVM", "Set VMM encryption state failed");
        return RET_ERR;
    }

    
    ret = kvm_vmm_connect_check();
    if (ret != RET_OK) {
        method_operation_log(caller_info, "KVM", "%s VMM encryption failed", compress ? "Enable" : "Disable");
        return ret;
    }

    
    ret = dfl_get_object_handle(PROXY_KVM_OBJ_NAME, &kvm_obj_handle);
    if (ret != DFL_OK) {
        proxy_method_operation_log(caller_info, "KVM", "Set VMM encryption state failed");
        debug_log(DLOG_ERROR, "Get kvm object failed.\n");
        return RET_ERR;
    }

    (void)vos_thread_sem4_p(g_kvm_vmm_encrypt_state_sem, SEM_WAIT_FOREVER);

    ret = dal_get_property_value_int32(kvm_obj_handle, KVM_PROPERTY_COMPRESS_STATE, &kvm_compress_state);
    if (ret != RET_OK) {
        (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
        proxy_method_operation_log(caller_info, "KVM", "Set VMM encryption state failed");
        debug_log(DLOG_ERROR, "Get kvm encryption state failed.\n");
        return RET_ERR;
    }

    
    
    if ((kvm_compress_state == ENCRYPT_STATE_OFF) && (compress == ENCRYPT_STATE_ON)) {
        (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
        method_operation_log(caller_info, "KVM", "Enable VMM encrypt failed");
        return RELY_ERR;
    }
    

    ret = dfl_set_property_value(object_handle, VMM_PROPERTY_COMPRESS_STATE, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
        method_operation_log(caller_info, "KVM", "%s VMM encryption failed", compress ? "Enable" : "Disable");
        return ret;
    }

    (void)vos_thread_sem4_v(g_kvm_vmm_encrypt_state_sem);
    method_operation_log(caller_info, "KVM", "%s VMM encryption successfully", compress ? "Enable" : "Disable");
    return ret;
}
#endif

gint32 proxy_method_set_video_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gint32 port = 0;
    gint32 old_port = 0;
    guint8 javavideo_enable = 0;
    GVariant *old_property_value = 0;
    GVariant *property_value = NULL;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_JAVA_VIDEO_ENABLE,
        &javavideo_enable);
    if (javavideo_enable != TRUE) {
        debug_log(DLOG_ERROR, "Video is hidden.\n");
        return RET_ERR;
    }

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set Video service port failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    port = g_variant_get_int32(property_value);
    
    if ((port > PORT_MAX_VALUE) || (port < PORT_MIN_VALUE)) {
        method_operation_log(caller_info, "KVM", "Set Video service port failed");
        debug_log(DLOG_ERROR, "Video port value is out of range.\n");
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(object_handle, VIDEO_PROPERTY_PORT, &old_property_value);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, "KVM", "Set video service port failed");
        debug_log(DLOG_ERROR, "Get video port value failed.\n");
        return RET_ERR;
    }

    old_port = g_variant_get_int32(old_property_value);
    g_variant_unref(old_property_value);
    old_property_value = NULL;

    
    if (port == old_port) {
        ret = RET_OK;
        goto out;
    }

    ret = kvm_vmm_port_check((guint16)port, VIDEO_SERVICE_PORT_INDEX);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "kvm_port_check failed\n");
        ret = RET_ERR;
        goto out;
    }

    ret = dfl_set_property_value(object_handle, VIDEO_PROPERTY_PORT, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set video port failed: %d.\n", ret);
    }

out:
    method_operation_log(caller_info, "KVM",
        "Set video service port to (%d) "
        "%s",
        port, (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}

gint32 proxy_method_set_video_enable(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    guchar state = 0;
    GVariant *property_value = NULL;
    guint8 javavideo_enable = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_JAVA_VIDEO_ENABLE,
        &javavideo_enable);
    if (javavideo_enable != TRUE) {
        debug_log(DLOG_ERROR, "Video is hidden.\n");
        return RET_ERR;
    }

    
    if (input_list == NULL) {
        method_operation_log(caller_info, "KVM", "Set video service state failed");
        debug_log(DLOG_ERROR, "Parameter invalid.\n");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    state = g_variant_get_byte(property_value);
    
    if ((state != SERVICE_OFF) && (state != SERVICE_ON)) {
        method_operation_log(caller_info, "KVM", "Set video service state failed");
        debug_log(DLOG_ERROR, "Parameter is out of range.\n");
        return RET_ERR;
    }

    ret = dfl_set_property_value(object_handle, VIDEO_PROPERTY_STATE, property_value, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set video service state failed.\n");
        ret = RET_ERR;
        goto out;
    }

out:
    method_operation_log(caller_info, "KVM", "%s video service %s", (state == SERVICE_OFF) ? "Disable" : "Enable",
        (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}

gint32 proxy_ipmi_set_kvm_encry_info(gconstpointer msg, gpointer user_data)
{
    if (msg == NULL) {
        return RET_ERR;
    }

    guint8 kvm_enabled = DISABLE;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (kvm_enabled == DISABLE) {
        return ipmi_send_simple_response(msg, COMP_CODE_STATUS_INVALID);
    }

#ifdef COMPATIBILITY_ENABLED
    GVariant *property_value = 0;
    OBJ_HANDLE vmm_handle = 0;
    OBJ_HANDLE proxy_kvm_handle = 0;
    GSList *caller_info = ipmimsg_to_callerinfo(msg);

    const guint8 *user_msg = get_ipmi_src_data(msg);
    guint8 complete_code = COMP_CODE_BUSY;

    
    do {
        if (user_msg == NULL) {
            method_operation_log(caller_info, "KVM", "Set KVM encryption failed");
            complete_code = COMP_CODE_OUTOF_RANGE;
            break;
        }
        guint8 encry_en = user_msg[8] & 0x01;
        if (dfl_get_object_handle(VMM_OBJ_NAME, &vmm_handle) != DFL_OK) {
            method_operation_log(caller_info, "KVM", "Set KVM encryption failed");
            break;
        }
        if (dfl_get_property_value(vmm_handle, VMM_PROPERTY_COMPRESS_STATE, &property_value) != DFL_OK) {
            debug_log(DLOG_ERROR, "Get vmm encryption state failed.\n");
            method_operation_log(caller_info, "KVM", "%s KVM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
            break;
        }

        gint32 vmm_compress_state = g_variant_get_int32(property_value);
        g_variant_unref(property_value);
        property_value = NULL;

        
        if ((vmm_compress_state == ENCRYPT_STATE_ON) && (encry_en == ENCRYPT_STATE_OFF)) {
            method_operation_log(caller_info, "KVM", "Disable KVM encryption failed");
            break;
        }

        
        if (kvm_vmm_connect_check() != RET_OK) {
            method_operation_log(caller_info, "KVM", "%s KVM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
            break;
        }

        if (dfl_get_object_handle(PROXY_KVM_OBJ_NAME, &proxy_kvm_handle) != DFL_OK) {
            debug_log(DLOG_ERROR, "Get proxy kvm object handle failed.\n");
            method_operation_log(caller_info, "KVM", "%s KVM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
            break;
        }

        property_value = g_variant_new_int32((gint32)encry_en);
        gint32 ret = dfl_set_property_value(proxy_kvm_handle, KVM_PROPERTY_COMPRESS_STATE, property_value, DF_SAVE);
        g_variant_unref(property_value);
        if (ret != DFL_OK) {
            method_operation_log(caller_info, "KVM", "%s KVM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
        } else {
            complete_code = COMP_CODE_SUCCESS;
            method_operation_log(caller_info, "KVM", "%s KVM encryption successfully",
                (encry_en == 0u) ? "Disable" : "Enable");
        }
    } while (0);

    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    return ipmi_send_simple_response(msg, complete_code);
#else
    return ipmi_send_simple_response(msg, COMP_CODE_STATUS_INVALID);
#endif
}

gint32 proxy_ipmi_set_vmm_encry_info(gconstpointer msg, gpointer user_data)
{
    if (msg == NULL) {
        return RET_ERR;
    }

    guint8 kvm_enabled = DISABLE;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (kvm_enabled == DISABLE) {
        return ipmi_send_simple_response(msg, COMP_CODE_STATUS_INVALID);
    }

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    GVariant *property_value = NULL;
    GSList *caller_info = ipmimsg_to_callerinfo(msg);
    OBJ_HANDLE kvm_obj_handle = 0;
    OBJ_HANDLE proxy_vmm_handle = 0;
    const guint8 *user_msg = get_ipmi_src_data(msg);
    guint8 complete_code = COMP_CODE_BUSY;

    do {
        if (user_msg == NULL) {
            method_operation_log(caller_info, "KVM", "Set VMM encryption failed");
            complete_code = COMP_CODE_OUTOF_RANGE;
            break;
        }
        guint8 encry_en = user_msg[8] & 0x01;

        if (dfl_get_object_handle(KVM_OBJ_NAME, &kvm_obj_handle) != DFL_OK) {
            method_operation_log(caller_info, "KVM", "Set VMM encryption failed");
            break;
        }

        if (dfl_get_property_value(kvm_obj_handle, KVM_PROPERTY_COMPRESS_STATE, &property_value) != DFL_OK) {
            debug_log(DLOG_ERROR, "Get kvm encryption state failed.\n");
            method_operation_log(caller_info, "KVM", "%s VMM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
            break;
        }

        gint32 kvm_compress_state = g_variant_get_int32(property_value);
        g_variant_unref(property_value);
        property_value = NULL;

        if ((kvm_compress_state == ENCRYPT_STATE_OFF) && (encry_en == ENCRYPT_STATE_ON)) {
            method_operation_log(caller_info, "KVM", "Enable VMM encryption failed");
            break;
        }

        
        if (kvm_vmm_connect_check() != RET_OK) {
            method_operation_log(caller_info, "KVM", "%s VMM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
            break;
        }

        if (dfl_get_object_handle(PROXY_VMM_OBJ_NAME, &proxy_vmm_handle) != DFL_OK) {
            debug_log(DLOG_ERROR, "Get proxy vmm object handle failed.\n");
            method_operation_log(caller_info, "KVM", "%s VMM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
            break;
        }

        property_value = g_variant_new_int32((gint32)encry_en);
        gint32 ret = dfl_set_property_value(proxy_vmm_handle, VMM_PROPERTY_COMPRESS_STATE, property_value, DF_SAVE);
        g_variant_unref(property_value);

        if (ret != DFL_OK) {
            method_operation_log(caller_info, "KVM", "%s VMM encryption failed",
                (encry_en == 0u) ? "Disable" : "Enable");
        } else {
            method_operation_log(caller_info, "KVM", "%s VMM encryption successfully",
                (encry_en == 0u) ? "Disable" : "Enable");
            complete_code = COMP_CODE_SUCCESS;
        }
    } while (0);
    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    return ipmi_send_simple_response(msg, complete_code);
#else
    return ipmi_send_simple_response(msg, RMCP_INVALID_COMMAND);
#endif
}

LOCAL gint32 update_proxy_property_value(OBJ_HANDLE obj_handle, OBJ_HANDLE proxy_obj_handle, gchar *property_name)
{
    gint32 ret = RET_OK;
    GVariant *property = NULL;

    ret = dfl_get_property_value(obj_handle, property_name, &property);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Compare property error: get property failed, %d.\n", ret);
        return ret;
    }

    
    ret = dfl_set_property_value(proxy_obj_handle, property_name, property, DF_SAVE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Compare property error: set property failed, %d.\n", ret);
    }

    g_variant_unref(property);

    return ret;
}

LOCAL gint32 proxy_kvm_property_init(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE kvm_obj = 0;
    OBJ_HANDLE proxy_kvm_obj = 0;
    gint32 port = 0;
    gint32 state = 0;
    guint8 screen_switch = 0;
#ifdef COMPATIBILITY_ENABLED
    gint32 encrypt_state = 0;
#endif
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    guchar local_kvm_state = 0;

    
    ret = dfl_get_object_handle(PROXY_KVM_OBJ_NAME, &proxy_kvm_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get proxy kvm object failed.\n");
        return ret;
    }

    
    property_name_list = g_slist_append(property_name_list, KVM_PROPERTY_PORT);
    property_name_list = g_slist_append(property_name_list, KVM_PROPERTY_STATE);
    property_name_list = g_slist_append(property_name_list, SCREEN_PROPERTY_SWITCH);
    property_name_list = g_slist_append(property_name_list, KVM_PROPERTY_LOCAL_KVM_STATE);
#ifdef COMPATIBILITY_ENABLED
    property_name_list = g_slist_append(property_name_list, KVM_PROPERTY_COMPRESS_STATE);
#endif

    ret = dfl_multiget_property_value(proxy_kvm_obj, property_name_list, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get proxy kvm property failed.\n");
        g_slist_free(property_name_list);
        return ret;
    }

    
    port = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 0));
    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
    screen_switch = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2));
    local_kvm_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 3));
#ifdef COMPATIBILITY_ENABLED
    encrypt_state = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 4));
#endif

    
    g_slist_free(property_name_list);
    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);

    ret = dfl_get_object_handle(KVM_OBJ_NAME, &kvm_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get kvm object failed.\n");
        return ret;
    }
    

    
    if (port == INVALID_SERVICE_PORT) {
        ret = update_proxy_property_value(kvm_obj, proxy_kvm_obj, KVM_PROPERTY_PORT);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy kvm port failed.\n");
        }
    }

    
    if (state == INVALID_SERVICE_STATE) {
        ret = update_proxy_property_value(kvm_obj, proxy_kvm_obj, KVM_PROPERTY_STATE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy kvm state failed.\n");
        }
    }

    
    if (screen_switch == INVALID_SERVICE_SCREEN_SWITCH) {
        ret = update_proxy_property_value(kvm_obj, proxy_kvm_obj, SCREEN_PROPERTY_SWITCH);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy kvm screen switch state failed.\n");
        }
    }

    
#ifdef COMPATIBILITY_ENABLED
    if (encrypt_state == INVALID_SERVICE_ENCRY_STATE) {
        ret = update_proxy_property_value(kvm_obj, proxy_kvm_obj, KVM_PROPERTY_COMPRESS_STATE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy kvm compress state failed.\n");
        }
    }
#else
    
    ret = dal_set_property_value_int32(proxy_kvm_obj, KVM_PROPERTY_COMPRESS_STATE, ENCRYPT_STATE_ON, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Compare property error: set property failed, %d.\n", ret);
    }
#endif

    
    
    if (local_kvm_state == INVALID_LOCAL_KVM_STATE) {
        ret = update_proxy_property_value(kvm_obj, proxy_kvm_obj, KVM_PROPERTY_LOCAL_KVM_STATE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy kvm state value failed.\n");
        }

        
        (void)dal_get_property_value_byte(proxy_kvm_obj, KVM_PROPERTY_LOCAL_KVM_STATE, &local_kvm_state);
    }

    
    ret = set_vga_display_status(local_kvm_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set_vga_display_status fail.\n");
        return RET_ERR;
    }

    

    return ret;
}

LOCAL gint32 proxy_vmm_property_init(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE vmm_obj = 0;
    OBJ_HANDLE proxy_vmm_obj = 0;
    gint32 port = 0;
    guint8 state = 0;
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    gint32 encrypt_state = 0;
#endif
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    ret = dfl_get_object_handle(PROXY_VMM_OBJ_NAME, &proxy_vmm_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get proxy vmm object failed.\n");
        return ret;
    }
    
    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_PORT);
    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_STATE);
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    property_name_list = g_slist_append(property_name_list, VMM_PROPERTY_COMPRESS_STATE);
#endif

    ret = dfl_multiget_property_value(proxy_vmm_obj, property_name_list, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get proxy vmm property failed.\n");
        g_slist_free(property_name_list);
        return ret;
    }

    
    port = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 0));
    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    encrypt_state = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 2));
#endif

    
    g_slist_free(property_name_list);
    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);

    ret = dfl_get_object_handle(VMM_OBJ_NAME, &vmm_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get vmm object failed.\n");
        return ret;
    }

    
    if (port == INVALID_SERVICE_PORT) {
        ret = update_proxy_property_value(vmm_obj, proxy_vmm_obj, VMM_PROPERTY_PORT);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy vmm port failed.\n");
        }
    }

    
    if (state == INVALID_SERVICE_STATE) {
        ret = update_proxy_property_value(vmm_obj, proxy_vmm_obj, VMM_PROPERTY_STATE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy vmm state failed.\n");
        }
    }

#ifdef SECURITY_ENHANCED_COMPATIBLE_BOARD_V1
    
    if (encrypt_state == INVALID_SERVICE_ENCRY_STATE) {
        ret = update_proxy_property_value(vmm_obj, proxy_vmm_obj, VMM_PROPERTY_COMPRESS_STATE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy vmm compress state failed.\n");
        }
    }
#else
    ret = dal_set_property_value_int32(proxy_vmm_obj, VMM_PROPERTY_COMPRESS_STATE, ENCRYPT_STATE_ON, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set vmm property compress state failed, ret value = %d.\n", ret);
    }
#endif

    
    return ret;
}

LOCAL gint32 proxy_video_property_init(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE video_obj = 0;
    OBJ_HANDLE proxy_video_obj = 0;
    gint32 port = 0;
    guint8 state = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    
    ret = dfl_get_object_handle(PROXY_VIDEO_OBJ_NAME, &proxy_video_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get proxy video object failed.\n");
        return ret;
    }

    
    property_name_list = g_slist_append(property_name_list, VIDEO_PROPERTY_PORT);
    property_name_list = g_slist_append(property_name_list, VIDEO_PROPERTY_STATE);
    ret = dfl_multiget_property_value(proxy_video_obj, property_name_list, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get proxy video property failed.\n");
        g_slist_free(property_name_list);
        return ret;
    }

    
    port = g_variant_get_int32((GVariant *)g_slist_nth_data(property_value, 0));
    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));

    
    g_slist_free(property_name_list);
    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);

    ret = dfl_get_object_handle(VIDEO_OBJ_NAME, &video_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Init error: get video object failed.\n");
        return ret;
    }

    
    if (port == INVALID_SERVICE_PORT) {
        ret = update_proxy_property_value(video_obj, proxy_video_obj, VIDEO_PROPERTY_PORT);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy video port failed.\n");
        }
    }

    
    if (state == INVALID_SERVICE_STATE) {
        ret = update_proxy_property_value(video_obj, proxy_video_obj, VIDEO_PROPERTY_STATE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Init error: update proxy video state failed.\n");
        }
    }

    

    return ret;
}

void ui_manage_proxy_property_init(void)
{
    gint32 ret = DFL_OK;

    ret = proxy_kvm_property_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "proxy_kvm_property_init failed.\n");
    }

    ret = proxy_vmm_property_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "proxy_vmm_property_init failed.\n");
    }

    ret = proxy_video_property_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "proxy_video_property_init failed.\n");
    }

    
    (void)vos_thread_mutex_sem4_create(&g_kvm_vmm_encrypt_state_sem, (gchar *)"kvm_vmm");
    

    return;
}


LOCAL gint32 check_file_path_valid(const gchar *file_path)
{
    gchar *real_path = NULL;

    real_path = realpath(file_path, NULL);
    if (real_path == NULL) {
        debug_log(DLOG_ERROR, "%s : check file real path : file_path is invalid", __FUNCTION__);
        return RET_ERR;
    }

    for (gint32 i = 0; i < sizeof(file_path_white_list) / sizeof(file_path_white_list[0]); i++) {
        if (strncmp(real_path, file_path_white_list[i], strlen(file_path_white_list[i])) == 0) {
            g_free(real_path);
            return RET_OK;
        }
    }

    g_free(real_path);
    return RET_ERR;
}


LOCAL gint32 CopyMoveFile(const char *dest_file, const char *src_file, guint8 *delete_flag)
{
    gint32 ret;
    gboolean tmp_path_flag;

    if (dest_file == NULL || src_file == NULL || delete_flag == NULL) {
        return RET_ERR;
    }
    debug_log(DLOG_DEBUG, "copy or move from %s to %s, flag = %d", src_file, dest_file, *delete_flag);

    tmp_path_flag = dal_check_real_path2(dest_file, TMP_PATH_WITH_SLASH) == VOS_OK &&
        dal_check_real_path2(src_file, TMP_PATH_WITH_SLASH) == VOS_OK;
    if (*delete_flag != 0 && tmp_path_flag == TRUE) {
        debug_log(DLOG_DEBUG, "move file from %s to %s, flag = %d", src_file, dest_file, *delete_flag);
        ret = dal_move_file(dest_file, src_file);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dal_move_file from %s to %s, fail %d", src_file, dest_file, ret);
            return RET_ERR;
        }
        *delete_flag = 0;
    } else {
        ret = vos_file_copy(dest_file, src_file);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "vos_file_copy from %s to %s fail %d", src_file, dest_file, ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}
gint32 CopyFile(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    gint32 reulst = RET_OK;
    gchar dest_path[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL!\n");
        return RET_ERR;
    }
    

    const gchar *src_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    const gchar *dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    uid_t own_uid = (uid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));
    gid_t own_gid = (gid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 3));
    guint8 delete_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 4));

    if ((src_file == NULL) || (dest_file == NULL)) {
        debug_log(DLOG_ERROR, "Parameter error!");
        return RET_ERR;
    }

    
    ret = vos_get_file_accessible(src_file);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "Source file is not exist, ret = %d!", ret);
        return RET_ERR;
    }

    ret = check_file_path_valid(src_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dest file path is not in white list", __FUNCTION__);
        return RET_ERR;
    }

    if ((dal_check_real_path2(dest_file, TMP_PATH_WITH_SLASH) != RET_OK &&
        dal_check_real_path2(dest_file, DEV_SHM_PATH_WITH_SLASH) != RET_OK) ||
        g_file_test(dest_file, G_FILE_TEST_IS_DIR)) {
        debug_log(DLOG_ERROR, "Destination file path is invalid or complicts with other dirctory");
        return RET_ERR;
    }

    (void)vos_rm_filepath(dest_file);

    
    ret = vos_get_file_dir(dest_file, dest_path, SNMP_CONF_MAXLINE_LEN + 1);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "Get dest file path failed, ret = %d!\n", ret);
        reulst = RET_ERR;
        goto __ERROR__;
    }

    ret = vos_check_dir((const gchar *)dest_path);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "Make dest file path failed, ret = %d!\n", ret);
        reulst = RET_ERR;
        goto __ERROR__;
    }

    
    ret = CopyMoveFile(dest_file, src_file, &delete_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Copy file failed, ret = %d!\n", ret);
        reulst = RET_ERR;
        goto __ERROR__;
    }

    (void)chmod(dest_file, (S_IRUSR));

    
    ret = chown(dest_file, own_uid, own_gid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Change file owner failed, ret = %d!\n", ret);
        reulst = RET_ERR;
    }

__ERROR__:

    if (delete_flag != 0) {
        (void)vos_rm_filepath(src_file);
    }

    return reulst;
}


gint32 CheckDir(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    const gchar *dest_path = NULL;

    dest_path = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (dest_path == NULL) {
        debug_log(DLOG_ERROR, "Parameter error!\n");
        return RET_ERR;
    }

    ret = vos_check_dir(dest_path);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "Make upload tmp dir failed, ret = %d!\n", ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 ModifyFileOwn(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    const gchar *dest_file = NULL;
    uid_t own_uid = 0;
    gid_t own_gid = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list can not be NULL", __FUNCTION__);
        return RET_ERR;
    }

    dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    own_uid = (uid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    own_gid = (gid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));
    if (dest_file == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter error", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = vos_get_file_accessible(dest_file);
    if (ret != TRUE) {
        debug_log(DLOG_ERROR, "%s: file %s is not exist, ret = %d", __FUNCTION__, dest_file, ret);
        return RET_ERR;
    }

    ret = check_file_path_valid(dest_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dest file path is not in white list", __FUNCTION__);
        return RET_ERR;
    }
    if (dal_check_realpath_before_open(dest_file) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: realpath check failed.", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = chown(dest_file, own_uid, own_gid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: change file %s owner failed, ret = %d", __FUNCTION__, dest_file, ret);
    }
    return ret;
}


gint32 RevertFileOwn(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 input_list_index = 0;
    uid_t own_uid;
    gid_t own_gid;
    file_owner_info *owner_info = NULL;
    gint32 ret;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list can not be NULL", __FUNCTION__);
        return RET_ERR;
    }

    const gchar *dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, input_list_index), NULL);
    own_uid = (uid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, ++input_list_index));
    own_gid = (gid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, ++input_list_index));
    if (dest_file == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter error", __FUNCTION__);
        return RET_ERR;
    }

    ret = check_file_path_valid(dest_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dest file path is not in white list", __FUNCTION__);
        return RET_ERR;
    }

    owner_info = (file_owner_info *)g_malloc0(sizeof(file_owner_info));
    if (owner_info == NULL) {
        debug_log(DLOG_ERROR, "%s: malloc failed", __FUNCTION__);
        return RET_ERR;
    }

    owner_info->own_gid = own_gid;
    owner_info->own_uid = own_uid;

    ret = snprintf_s(owner_info->file_name, sizeof(owner_info->file_name), sizeof(owner_info->file_name) - 1, "%s",
        dest_file);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        g_free(owner_info);
        return RET_ERR;
    }

    G_LOCK(g_revert_owner_list_sync);
    g_revert_owner_list = g_slist_append(g_revert_owner_list, (gpointer)owner_info);
    G_UNLOCK(g_revert_owner_list_sync);
    return RET_OK;
}


gint32 ModifyFileMode(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret = 0;
    const gchar *dest_file = NULL;
    mode_t file_mode = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list can not be NULL", __FUNCTION__);
        return RET_ERR;
    }

    dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    file_mode = (mode_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    if (dest_file == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter error", __FUNCTION__);
        return RET_ERR;
    }

    
    if (vos_get_file_accessible(dest_file) != TRUE) {
        debug_log(DLOG_ERROR, "%s: file %s is not exist", __FUNCTION__, dest_file);
        return RET_ERR;
    }

    ret = check_file_path_valid(dest_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dest file path is not in white list", __FUNCTION__);
        return RET_ERR;
    }
    if (dal_check_realpath_before_open(dest_file) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: realpath check failed.", __FUNCTION__);
        return RET_ERR;
    }
    
    ret = chmod(dest_file, file_mode);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: change file %s mode failed, ret = %d", __FUNCTION__, dest_file, ret);
    }

    return ret;
}


gint32 DeleteFile(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    const gchar *dest_file = NULL;
    gint32 ret;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list can not be NULL", __FUNCTION__);
        return RET_ERR;
    }

    dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (dest_file == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter error", __FUNCTION__);
        return RET_ERR;
    }

    if (vos_get_file_accessible(dest_file) == TRUE) {
        ret = check_file_path_valid(dest_file);
        if (ret == RET_OK) {
            (void)vos_rm_filepath(dest_file);
        }
    }
    if (dal_check_realpath_before_open(dest_file) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: realpath check failed.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 check_valid_file(const gchar *create_src_file)
{
    guint8 i;
    const gchar* valid_create_file[] = {
        SP_OSINSTALL_PARA_PATH,
        RECORD_FILE
    };

    for (i = 0; i < sizeof(valid_create_file) / sizeof(valid_create_file[0]); i++) {
        if (g_strcmp0(create_src_file, valid_create_file[i]) == 0) {
            return RET_OK;
        }
    }

    return RET_ERR;
}


gint32 CreateFile(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    const gchar *dest_file = NULL;
    const gchar *create_flag = NULL;
    mode_t file_mode = 0;
    uid_t own_uid = 0;
    gid_t own_gid = 0;
    FILE *fp = 0;
    gint32 fd = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input_list can not be NULL", __FUNCTION__);
        return RET_ERR;
    }

    dest_file = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    create_flag = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    file_mode = (mode_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));
    own_uid = (uid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 3));
    own_gid = (gid_t)g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 4));
    if (dest_file == NULL || create_flag == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter error", __FUNCTION__);
        return RET_ERR;
    }

    // 判断文件是否存在, 不存在则创建
    ret = vos_get_file_accessible(dest_file);
    if (ret == TRUE) {
        debug_log(DLOG_DEBUG, "%s: file %s exist", __FUNCTION__, dest_file);
        return RET_OK;
    }

    // 判断待创建的文件是否合理(白名单校验,不支持所有文件创建)
    ret = check_valid_file(dest_file);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: file %s not support create", __FUNCTION__, dest_file);
        return RET_ERR;
    }

    fp = g_fopen(dest_file, create_flag);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: open %s file error", __FUNCTION__, dest_file);
        return RET_ERR;
    }

    // 修改文件权限
    fd = fileno(fp);
    (void)fchmod(fd, file_mode);

    // 修改属主信息
    ret = fchown(fd, own_uid, own_gid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: change file %s owner failed, ret = %d", __FUNCTION__, dest_file, ret);
        ret = RET_ERR;
    }

    (void)fclose(fp);

    return ret;
}

#ifdef ARM64_HI1711_ENABLED

gint32 send_recv_msg_with_m3(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    const guint8 *input_data = NULL;
    gsize input_data_len = 0;
    guint8 *output_data = NULL;
    guint16 output_data_len = 0;
    gint32 ret;

    if (input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input param null.", __FUNCTION__);
        return RET_ERR;
    }
    output_data_len = g_variant_get_uint16((GVariant *)(g_slist_nth_data(input_list, 1)));
    output_data = (guint8 *)g_malloc0(output_data_len);
    if (output_data == NULL) {
        debug_log(DLOG_ERROR, "%s:g_malloc0 fail.", __FUNCTION__);
        return RET_ERR;
    }

    input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 0)),
        &input_data_len, sizeof(guint8));
    ret = dal_send_recv_msg_with_m3(input_data, (guint16)input_data_len, output_data, &output_data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: command sendrecv failed(ret = 0x%x).", __FUNCTION__, ret);
        g_free(output_data);
        return ret;
    }

    *output_list = g_slist_append(*output_list,
        g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, output_data, (guint32)output_data_len, sizeof(guint8)));
    g_free(output_data);

    return RET_OK;
}
#endif
