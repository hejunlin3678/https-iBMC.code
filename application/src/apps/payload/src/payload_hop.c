
#include <errno.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <unistd.h>
#include "pme/common/mscm_vos.h"
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "payload_hop.h"
#include "payload_ipmi.h"
#include "payload_pwr.h"
#include "payload_hs.h"
#include "payload_com.h"
#include "payload_storage.h"
#include "pme_app/dalib/dal_m3_msg_intf.h"

static guchar g_reset_event_from_pwr = BUT_EVT_FALSE;
static guchar g_pwrbtn_pwr_on_flag = BUT_EVT_FALSE;
static TASKID g_power_status_task[MAX_FRU_NUM] = {0};
static TASKID g_reset_clr_task = 0;
FRU_EVENT_S g_fru_event;
PPIN_MATED_CB_T gp_pin_mated_cb;
PCRITERIA_CB_T gp_criteria_cb;
guchar g_pwr_off_abort = FALSE;
guint8 g_switch_flag_count = 0;
SEMID g_power_operate_sem4 = 0;


guint8 g_push_pwr_btn_flag = PUSH_PWR_BTN_WAIT;

LOCAL GMutex g_force_pwr_off_mutex = { 0 };

void hop_set_pwr_off_abort(guchar value)
{
    g_pwr_off_abort = value;
    return;
}

guchar hop_get_pwr_off_abort(void)
{
    return g_pwr_off_abort;
}

void hop_set_pwrbtn_on_flag(guchar value)
{
    g_pwrbtn_pwr_on_flag = value;
    return;
}

guchar hop_get_pwrbtn_on_flag(void)
{
    return g_pwrbtn_pwr_on_flag;
}

void hop_set_reset_event_from_pwr(guchar value)
{
    g_reset_event_from_pwr = value;
    return;
}

guchar hop_get_reset_event_from_pwr(void)
{
    return g_reset_event_from_pwr;
}


gint32 hop_chassis_hard_reset(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guint32 product_id = 0;

    (void)dal_get_product_id(&product_id);
    debug_log(DLOG_DEBUG, "call %s\r\n", __FUNCTION__);
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        temp_data = FALSE;
        result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_HARD_RESET_WP, temp_data, DF_HW);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object HardResetWp fail!(result=%d)", result);
        }
        
        if (is_support_storage_payload() == TRUE) {
            result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_HARD_RESET, temp_data, DF_HW);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set ChassisPayload object HardReset fail!(result=%d)\r\n", result);
            }
            vos_task_delay(10); 
        }

        temp_data = TRUE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_HARD_RESET, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object HardReset fail!(result=%d)\r\n", result);
        }

        result = dal_set_property_value_byte(object_handle, PROPERTY_PAYLOAD_HARD_RESET_WP, temp_data, DF_HW);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object HardResetWp fail!(result=%d)", result);
        }
        debug_log(DLOG_ERROR, "%s:hop chassis hard reset.", __FUNCTION__);
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


void hop_get_acpi_status(guchar fru_device_id, guchar *val)
{
    guint32 temp_data;
    gint32 result;
    guchar tmp;
    static guchar old_tmp[MAX_FRU_NUM] = {0};
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (fru_device_id == 0) {
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "get fru%d object fail!(result=%d)\r\n", fru_device_id, result);
            return;
        }
    } else {
        if (result != DFL_OK) {
            debug_log(DLOG_DEBUG, "get fru%d object fail!(result=%d)\r\n", fru_device_id, result);
            return;
        }
    }

    result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_ACPI_STATUS, &property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d property AcpiStatus fail!(result=%d)\r\n", fru_device_id, result);
        return;
    }
    tmp = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    
    *val = tmp;
    if (old_tmp[fru_device_id] != tmp) {
        g_printf("fru%d acpi_status:old_tmp=%02X,tmp=%02X \r\n", fru_device_id, old_tmp[fru_device_id], tmp);
        debug_log(DLOG_ERROR, "fru%d acpi_status:old_tmp=%02X,tmp=%02X", fru_device_id, old_tmp[fru_device_id],
            tmp);
        old_tmp[fru_device_id] = tmp;

        
        temp_data = MAKE_DWORD(0x01, 0xFF, 0xFF, ((ALL_POWER_GOOD_OK == *val) ? 0x00 : 0x06));
        property_data = g_variant_new_uint32(temp_data);
        (void)dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_ACPI_VALUE, property_data, DF_NONE);
        g_variant_unref(property_data);
    }

    return;
}


LOCAL void hop_get_pwrpg_status(guchar fru_device_id, guchar *val)
{
    gint32 result;
    guchar tmp;
    static guchar old_pwrpg_tmp[MAX_FRU_NUM] = {0};
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWR_GD, &property_data);
        if (result != DFL_OK) {
            return;
        }
        tmp = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        *val = tmp;

        if (old_pwrpg_tmp[fru_device_id] != tmp) {
            g_printf("pwrpg_status:old_tmp=%02X,tmp=%02X \r\n", old_pwrpg_tmp[fru_device_id], tmp);
            debug_log(DLOG_ERROR, "pwrpg_status:old_tmp=%02X,tmp=%02X \r\n", old_pwrpg_tmp[fru_device_id], tmp);
            old_pwrpg_tmp[fru_device_id] = tmp;
        }
    } else {
        debug_log(DLOG_DEBUG, "get fru%d Payload object fail!(result=%d)\r\n", fru_device_id, result);
    }

    return;
}


LOCAL void write_short_button_cpld(OBJ_HANDLE handle, guint8 data)
{
    gint32 result;
    GVariant *property_data = NULL;
    property_data = g_variant_new_byte(data);
    result = dfl_set_property_value(handle, PROPERTY_PAYLOAD_PWR_BTN_SHORT, property_data, DF_HW);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonShort fail!(result=%d)\r\n", result);
    }
    return;
}


LOCAL gint32 hop_push_pwr_button_short(guchar fru_device_id, CTRL_BUTTON_HANDLE_E type)
{
    gint32 result;
    OBJ_HANDLE object_handle;

    debug_log(DLOG_DEBUG, "\r\ncall %s\r\n", __FUNCTION__);
    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        if (is_support_storage_payload() == TRUE) {
            // 存储产品
            debug_log(DLOG_DEBUG, "%s:pangea product push button type(%d).", __FUNCTION__, type);
            ctrl_power_button_handle(type);
        } else {
            write_short_button_cpld(object_handle, TRUE);
            // 此处不应该由软件来恢复,如果cpld修改过来了，这部分代码需删除
            vos_task_delay(1000); // 电平持续1000ms后恢复
            write_short_button_cpld(object_handle, FALSE);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d).", result);
    }

    return RET_OK;
}


gint32 hop_set_pwr_button_lock(guint8 button_lock, guint8 persist_mode)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新PwrButtonShort状态
        property_data = g_variant_new_byte(button_lock);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK, property_data, DF_AUTO);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLock fail!(result=%d)", result);
        }
        
        if (persist_mode == PERSIST_LOCK) {
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK_PER, property_data, DF_SAVE);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLockPer fail!(result=%d)", result);
            }
        } else if (persist_mode == SAVE_TEMPORARY_POWER_BUTTON_LOCK) {
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK_PER, property_data,
                DF_SAVE_TEMPORARY);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLockPer %d fail!(result=%d)", button_lock,
                    result);
            }
        }
        g_variant_unref(property_data);
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)", result);
    }
    return RET_OK;
}


gint32 hop_synch_pwr_button_lock(void)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK_PER, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_value fail!(result=%d)\r\n", result);
        }
        if (property_data == NULL) {
            
            debug_log(DLOG_ERROR, DB_STR_GET_PROPERTY_NULL);
            return PAYLOAD_FAIL;
        }
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK, property_data, DF_AUTO);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLock fail!(result=%d)\r\n", result);
        }

        
        g_variant_unref(property_data);

        return RET_OK;
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }
    return RET_OK;
}


LOCAL void hop_push_pwr_button_long(guchar fru_device_id, CTRL_BUTTON_HANDLE_E button_type)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    debug_log(DLOG_DEBUG, "call %s\r\n", __FUNCTION__);

    if (is_support_storage_payload() == TRUE) {
        ctrl_power_button_handle(button_type);
        return;
    }

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        // 更新PwrButtonLong状态
        temp_data = TRUE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LONG, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLong fail!(result=%d)\r\n", result);
        }
        g_push_pwr_btn_flag = PUSH_PWR_BTN_YES;
        // 此处不应该由软件来恢复,如果cpld修改过来了，这部分代码需删除
        vos_task_delay(6000); 
        temp_data = FALSE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LONG, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonLong fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
        g_push_pwr_btn_flag = PUSH_PWR_BTN_NO;
    }

    return;
}


LOCAL gint32 hop_set_nmi_interrupt(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    debug_log(DLOG_DEBUG, "call %s", __FUNCTION__);
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新NMIInterrupt
        temp_data = TRUE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_NMI_INTR, property_data, DF_HW);
        g_variant_unref(property_data);
        property_data = NULL;
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object NMIInterrupt value=%d fail!(result=%d)", result,
                temp_data);
        }

        temp_data = FALSE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_NMI_INTR, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object NMIInterrupt value=%d fail!(result=%d)", result,
                temp_data);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)", result);
    }

    return RET_OK;
}


gint32 hop_get_power_flag(guchar fru_device_id, const gchar *property_name, guchar *val)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        // 先取对象的SysResetFlag
        result = dfl_get_extern_value(object_handle, property_name, &property_data, DF_COPY);
        if (result != DFL_OK) {
            return result;
        }
        *val = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
    } else {
        debug_log(DLOG_DEBUG, "get fru%d %s object fail!(result=%d)\r\n", fru_device_id, CLASS_PAYLOAD, result);
    }

    return RET_OK;
}


void hop_clear_power_flag(guchar fru_device_id, const gchar *property_name)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        // 更新SysResetFlag状态
        temp_data = FALSE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, property_name, property_data, DF_AUTO);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set %s object %s fail!(result=%d)\r\n", CLASS_PAYLOAD, property_name, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)\r\n", CLASS_PAYLOAD, result);
    }

    return;
}


gint32 hop_get_system_reset_flag(guchar *val)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guint32 sys_boot_event;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 先取对象的SysResetFlag
        result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_SYS_RST_FLAG, &property_data, DF_COPY);
        if (result != DFL_OK) {
            return result;
        }
        *val = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        if (*val == 0) { 
            sys_boot_event = MAKE_DWORD(0, 0xff, 0xff, 0x0);
        } else if (*val == 0x1) {
            sys_boot_event = MAKE_DWORD(1, 0xff, 0xff, 0x0);
        } else {
            sys_boot_event = MAKE_DWORD(0, 0xff, 0xff, 0x0);
            debug_log(DLOG_ERROR, "CPLD is error, value is invalid. val = %d", *val);
        }

        result = dal_set_property_value_uint32(object_handle, PROPERTY_PAYLOAD_SYS_BOOT_EVENT, sys_boot_event, DF_NONE);
        if (result != RET_OK) {
            return result;
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


gint32 hop_clear_system_reset_flag(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新SysResetFlag状态
        temp_data = FALSE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_SYS_RST_FLAG, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object SysResetFlag fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


gint32 hop_clear_system_reset_src(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        temp_data = FALSE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_SYS_RST_SRC, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object SysResetSrc fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


gint32 hop_get_pwr_button_lock_stat(guchar *val)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 先取对象的PwrButtonEvt ,涉及对寄存器操作需要使用DF_COPY，否者属性值不同步，设置会失败
        result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK, &property_data, DF_COPY);
        if (result != DFL_OK) {
            return result;
        }
        *val = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 hop_get_pwr_button_event(guchar *val)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 先取对象的PwrButtonEvt ,涉及对寄存器操作需要使用DF_COPY，否者属性值不同步，设置会失败
        result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_EVT, &property_data, DF_COPY);
        if (result != DFL_OK) {
            return result;
        }
        *val = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


gint32 hop_set_pwr_flag(guchar fru_device_id, const gchar *property_name, guint8 value)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, property_name, &property_data);
        if (result != DFL_OK) {
            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, result);
            return RET_ERR;
        }

        temp_data = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        if (temp_data != value) {
            property_data = g_variant_new_byte(value);
            result = dfl_set_property_value(object_handle, property_name, property_data, DF_NONE);
            g_variant_unref(property_data);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "set %s object %s fail!(result=%d)\r\n", CLASS_PAYLOAD, property_name, result);
            }
        }
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)\r\n", CLASS_PAYLOAD, result);
    }

    return RET_OK;
}


void hop_set_listen_sel_flag(guchar fru_device_id, const gchar *property_name, guint8 value)
{
    gint32 result;
    guint32 temp_data;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        temp_data = MAKE_DWORD(value, 0xFF, 0xFF, 0x01);
        property_data = g_variant_new_uint32(temp_data);
        result = dfl_set_property_value(object_handle, property_name, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set %s object %s fail!(result=%d)\r\n", CLASS_PAYLOAD, property_name, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)\r\n", CLASS_PAYLOAD, result);
    }

    return;
}


gint32 hop_ctrl_bios_switch(guchar fru_device_id, guint8 action)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;
    guint8 temp_data;
    guint8 temp_bios;
    GSList *input_list = NULL;
    OBJ_HANDLE partition_obj_handle = 0;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_BIOS_SWTICH, &property_data);
        if (result != DFL_OK) {
            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, result);
            return RET_ERR;
        }

        temp_data = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        // 0xff代表没有双bios，直接返回ok
        if (temp_data == 0xff) {
            debug_log(DLOG_DEBUG, "don't need switch bios.\r\n");
            return RET_OK;
        } else {
            // 做好防抖连续两次要求进行BIOS切换才考虑切换的动作
            if ((g_switch_flag_count != 1) &&
                (RET_OK != dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &partition_obj_handle))) {
                g_switch_flag_count++;
                debug_log(DLOG_DEBUG, "don't need switch bios, need warning again.\r\n");
                return RET_OK;
            }
        }
        // 存在双bios，切换到另一个bios
        temp_bios = (TRUE == temp_data) ? FALSE : TRUE;

        // 双BIOS切换时，记录运行日志
        maintenance_log_v2(MLOG_WARN, FC_BMC_SW_FLASH,
            "BIOS in Flash %u boots failed! Switch to BIOS in Flash %u by condition%d\n", temp_data, temp_bios, action);
        // 拥有硬分区模式的单板通过调用硬分区app进行配置保留
        if ((RET_OK != dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &partition_obj_handle)) &&
            (RET_OK == dal_get_object_handle_nth(CLASS_PARTITION, 0, &partition_obj_handle))) {
            // 由于BIOS配置无法检验,在硬件异常的情况下不进行配置保留
            if (action != SW_BIOS_ACTION_HW) {
                temp_bios |= 0x80;
            }
            property_data = g_variant_new_byte(temp_bios);
            input_list = g_slist_append(input_list, property_data);
            result = dfl_call_class_method(partition_obj_handle, METHOD_SWITCH_BIOS_PARTITION, NULL, input_list, NULL);
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        } else {
            property_data = g_variant_new_byte(temp_bios);
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_BIOS_SWTICH, property_data, DF_HW);
            g_variant_unref(property_data);
        }

        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set %s object %s fail!(result=%d)", CLASS_PAYLOAD, PROPERTY_PAYLOAD_BIOS_SWTICH,
                result);
        } else {
            g_switch_flag_count = 0;
            debug_log(DLOG_INFO, "switch bios:%u ok.", temp_bios);
        }
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)", CLASS_PAYLOAD, result);
    }

    return RET_OK;
}


gint32 hop_is_bios_boot_ok(guchar fru_device_id)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;
    guint8 temp_data;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_BIOS_BOOT, &property_data);
        if (result != DFL_OK) {
            
            debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, result);
            return TRUE;
        }

        temp_data = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        if (temp_data == 0xff) {
            debug_log(DLOG_DEBUG, "don't need check bios boot.\r\n");
            return TRUE;
        }

        return (temp_data == TRUE) ? FALSE : TRUE;
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)\r\n", CLASS_PAYLOAD, result);
        return TRUE;
    }
}


gint32 hop_clear_bios_boot_status(guchar fru_device_id)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &object_handle);
    if (result == DFL_OK) {
        property_data = g_variant_new_byte(FALSE);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_BIOS_BOOT, property_data, DF_AUTO);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set %s object %s fail!(result=%d)\r\n", CLASS_PAYLOAD, PROPERTY_PAYLOAD_BIOS_BOOT,
                result);
        } else {
            debug_log(DLOG_INFO, "clear bios boot fail flag ok\r\n");
        }
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)\r\n", CLASS_PAYLOAD, result);
    }

    return RET_OK;
}


gint32 hop_set_pwr_sel_event(const gchar *obj_name, const gchar *property_name, guint8 value)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    result = dfl_get_object_handle(obj_name, &object_handle);
    if (result == DFL_OK) {
        property_data = g_variant_new_uint32(MAKE_DWORD(value, 0xff, 0xff, 0));
        result = dfl_set_property_value(object_handle, property_name, property_data, DF_NONE);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set %s object %s fail!(result=%d)\r\n", obj_name, property_name, result);
        }
    } else {
        debug_log(DLOG_ERROR, "get %s object fail!(result=%d)\r\n", obj_name, result);
    }

    return RET_OK;
}


gint32 hop_clear_pwr_button_event(void)
{
    gint32 result;
    guint8 temp_data;
    OBJ_HANDLE object_handle;
    GVariant* property_data = NULL;
    guint8 value;

    if (is_support_storage_payload() == TRUE) {
        
        value = 1;
        (void)set_storpayload_property_byte(PROPERTY_PHYSICAL_SHORT_BUTTON_CL, &value, DF_HW);
        vos_task_delay(100); // 延时100ms
        value = 0;
        (void)set_storpayload_property_byte(PROPERTY_PHYSICAL_SHORT_BUTTON_CL, &value, DF_HW);
        debug_log(DLOG_ERROR, "%s: write cpld clear short button.", __FUNCTION__);
        return RET_OK;
    }

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 更新PwrButtonEvt状态
        temp_data = FALSE;
        property_data = g_variant_new_byte(temp_data);
        result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BTN_EVT, property_data, DF_HW);
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set ChassisPayload object PwrButtonEvt fail!(result=%d)\r\n", result);
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


gint32 hop_get_power_before_ac_lost(guchar *val)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
            
            result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, &property_data, DF_AUTO);
        } else {
            result = dfl_get_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, &property_data);
        }
        if (result != DFL_OK) {
            return result;
        }
        *val = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}

gint32 hop_set_power_before_ac_lost(guchar val)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    gchar accesor_name[MAX_NAME_SIZE] = {0};
    debug_log(DLOG_ERROR, "set power before ac lost %d.", val);
    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        property_data = g_variant_new_byte(val);
        if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
            
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, 
                                            property_data, DF_SAVE_TEMPORARY);
            g_variant_unref(property_data);
            return result;
        }

        result = dfl_get_property_accessor(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, accesor_name, MAX_NAME_SIZE);
        if (result == DFL_OK) {
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, property_data, DF_HW);
        } else {
            result = dfl_set_property_value(object_handle, PROPERTY_PAYLOAD_PWR_BEF_AC, property_data, DF_NONE);
        }
        g_variant_unref(property_data);
        if (result != DFL_OK) {
            return result;
        }
    } else {
        debug_log(DLOG_ERROR, "set ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}

gint32 hop_get_forbid_poweron_flag(guint8 *forbid_poweron_flag)
{
    OBJ_HANDLE payload_handle = 0;
    gint32 retv;

    retv = dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &payload_handle);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to call dfl_get_object_handle.\r\n");
        return COMP_CODE_UNKNOWN;
    }

    retv = dal_get_property_value_byte(payload_handle, PROPERTY_PAYLOAD_FORBID_POWERON_FLAG, forbid_poweron_flag);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_byte %s fail! result = %d", PROPERTY_PAYLOAD_FORBID_POWERON_FLAG,
            retv);
        return RET_ERR;
    } else {
        return RET_OK;
    }
}


LOCAL void hop_wait_power_on_permit(void)
{
#define WAIT_POWER_ON_OK 1 
    guchar poweron_permit = 0;
    OBJ_HANDLE payload_handle = 0;
    gint32 retv;
    gint32 i;
    retv = dfl_get_object_handle(OBJECT_CHASSISPAYLOAD, &payload_handle);
    if (retv != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to call dfl_get_object_handle.\r\n");
        return;
    }
    
    for (i = 0; i < 60; i++) {
        (void)dal_get_property_value_byte(payload_handle, PROPERTY_WAIT_POWER_ON_PERMIT, &poweron_permit);
        debug_log(DLOG_ERROR, "%s, i:%d, poweron_permit is %d.\r\n", __FUNCTION__, i, poweron_permit);
        if (WAIT_POWER_ON_OK == poweron_permit) {
            break;
        }
        vos_task_delay(1000); 
    }

    return;
}

#ifdef ARM64_HI1711_ENABLED

LOCAL gint32 __call_bios_verify(guint8 type)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 verify_result;

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: can't find object %s. Ret is %d", __FUNCTION__, PFN_CLASS_NAME, ret);
        return RET_OK;
    }
    input_list = g_slist_append(input_list, g_variant_new_byte(type));
    input_list = g_slist_append(input_list, g_variant_new_byte(TRUE));

    ret = dfl_call_class_method(obj_handle, METHOD_VERIFY_BIOS_FIRMWARE, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call %s.%s failed. ret is %d\n", __FUNCTION__, BIOS_CLASS_NAME,
                  METHOD_VERIFY_BIOS_FIRMWARE, ret);
        return RET_OK;
    }
    verify_result = g_variant_get_int32((GVariant *)g_slist_nth_data(output_list, 0));

    
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return verify_result;
}
#endif


LOCAL gboolean power_on_ok(guint8 fru_device_id)
{
    pp_set_power_ctrl_fault(fru_device_id, POWER_ON, FALSE);
    return TRUE;
}


LOCAL gboolean power_on_fail(guint8 fru_device_id)
{
    
    g_printf("\r\nhop_on:push long power button.");
    debug_log(DLOG_ERROR, "\r\nhop_on:push long power button.");

    set_wol_by_saved_value(WOL_STATE_OFF);

    hop_push_pwr_button_long(fru_device_id, CTRL_BUTTON_BUTT);
    pp_set_power_ctrl_fault(fru_device_id, POWER_ON, TRUE);
    vos_task_delay(5000); 
    g_printf("\r\nhop_on:power on fail.");
    debug_log(DLOG_ERROR, "\r\nhop_on:power on fail.");

    // 现网有执行上电后结果没有上电的问题, 此处增加日志用于快速定界问题
    maintenance_log_v2(MLOG_ERROR, FC_BMC_POWER_ON_FAIL, "Fru%d power on failed.", fru_device_id);
    return FALSE;
}


LOCAL gboolean power_on_by_acpi(guint8 fru_device_id)
{
    gint32 i = 0;
    guint8 acpi = ALL_POWER_GOOD_FAIL;
    // 如果S4/S5信号无效，就短按按钮，否则已经上电就直接返回成功
    hop_get_acpi_status(fru_device_id, &acpi);

    // S0和S3状态下，payload都是上电的
    if (acpi != ALL_POWER_GOOD_OK) {
        g_printf("\r\nhop_on:push short power button.");
        debug_log(DLOG_ERROR, "\r\nhop_on:push short power button.");

        // 短按一下按钮
        (void)hop_push_pwr_button_short(fru_device_id, CTRL_BUTTON_UP);
    } else {
        g_printf("\r\nhop_on:already power on.\r\n");
        debug_log(DLOG_ERROR, "\r\nhop_on:already power on.\r\n");
        return power_on_ok(fru_device_id);
    }

    // 对各检测点进行检测
    hop_get_acpi_status(fru_device_id, &acpi);
    
    while ((acpi != ALL_POWER_GOOD_OK) && (i < 100)) {
        vos_task_delay(100); 
        hop_get_acpi_status(fru_device_id, &acpi);
        i++;
        if (i == 15) { 
            pp_printf_abnormal_pwrstate();
        }
    }
    if (acpi != ALL_POWER_GOOD_OK) {
        // 1秒钟后系统还未上电，则认为控制上电失败
        return power_on_fail(fru_device_id);
    }

    g_printf("\r\nhop_on:power on successfully.\r\n");
    debug_log(DLOG_ERROR, "\r\nhop_on:power on successfully.\r\n");

    
    // 已触发传感器事件，清除寄存器
    hop_clear_power_flag(fru_device_id, PROPERTY_PAYLOAD_PWR_DROP_SRC);
    
    hop_set_listen_sel_flag(fru_device_id, PROPERTY_PAYLOAD_PWRSIG_DROP, FALSE);
    return power_on_ok(fru_device_id);
}


LOCAL void transfer_key_info_from_e2p_to_cpld(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;
    
    if (!dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        return;
    }

    gint32 ret = dfl_get_object_list(CLASS_EEPROM_STORAGE_GROUP_NAME, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]: dfl_get_object_list failed!", __FUNCTION__);
        return;
    }
    GSList* input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(1)); 

    for (guint8 i = 1; i <= 12; i++) { 
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            obj_handle = (OBJ_HANDLE)obj_node->data;
            ret += dfl_call_class_method(obj_handle, METHOD_KEY_PROCUDT_INFO_INIT, NULL, input_list, NULL);
        }

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Read EEP data failed! ret = %d, Retry %d time", ret, i);
            vos_task_delay(10000); 
        } else {
            
            g_slist_free(obj_list);
            g_slist_free(input_list);
            return;
        }
        ret = RET_OK;
    }
    g_slist_free(input_list);
    g_slist_free(obj_list);
}


LOCAL void bmc_write_dvfs_to_cpld(void)
{
#define CPLD_DVFS_REGISTER 0x150
#define CPLD_DVFS_MASK     0x40

    guint8 dvfs_register = 0;
    guint8 dvfs_check = 0;
    gint32 ret;
    OBJ_HANDLE handle = 0;

    
    ret = dal_get_object_handle_nth(CLASS_DVFSFUNCTION, 0, &handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "This model is not supported DVFS.");
        return;
    }
    ret = dal_read_cpld_byte(1, CPLD_DVFS_REGISTER, &dvfs_register);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read register(0x%x) failed, ret(%d)", CPLD_DVFS_REGISTER, ret);
        return;
    }

    
    dvfs_register = dvfs_register | CPLD_DVFS_MASK;

    ret = dal_write_cpld_byte(1, CPLD_DVFS_REGISTER, dvfs_register);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_write_cpld_byte(offset 0x%X) failed: ret %d.", CPLD_DVFS_REGISTER, ret);
        return;
    }
    vos_task_delay(10); 
    ret = dal_read_cpld_byte(1, CPLD_DVFS_REGISTER, &dvfs_check);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read register(0x%x) failed, ret(%d)", CPLD_DVFS_REGISTER, ret);
        return;
    }
    
    if (dvfs_check != dvfs_register) {
        maintenance_log_v2(MLOG_INFO, FC_BMC_POWER_ON_FAIL,
            "DVFS register verification failed. write data:%02X read data:%02X", dvfs_register, dvfs_check);
    }
}


guchar hop_on(guchar fru_device_id)
{
    gint32 ret;
    guint8 forbid_poweron_flag = 0;

    
    bmc_write_dvfs_to_cpld();

    
    transfer_key_info_from_e2p_to_cpld();
    
    ret = hop_get_forbid_poweron_flag(&forbid_poweron_flag);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "hop_get_support_power_on_flag failed");
        return FALSE;
    }

    if (forbid_poweron_flag != UPGRADE_ALLOW_POWER_ON) {
        debug_log(DLOG_ERROR, "Forbid power on.");
        return FALSE;
    }

    if ((is_support_storage_payload() == TRUE) && (is_pw_on_condit_satised() != RET_OK)) {
        debug_log(DLOG_ERROR, "Can not reach power on condition, forbid power on.");
        return FALSE;
    }

#ifdef ARM64_HI1711_ENABLED
    guint8 software_type = 0;
    (void)dal_get_software_type(&software_type);
    if (software_type != MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        
        guint8 TpcmSupportState = 0;
        OBJ_HANDLE obj_handle = 0;
        ret = dal_get_bmc_tpcm_enable(&TpcmSupportState);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get Tpcm Support State failed(ret = %d)", ret);
        }
        if (TpcmSupportState == TCM_ENABLE) {
            (void)dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
            (void)dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_MEASURE_FLAG, BIOS_MEASURE_PROGRESS, DF_NONE);
        }

        
        ret = __call_bios_verify(VERIFY_BY_M3);
        if (TpcmSupportState == TCM_ENABLE) {
            (void)dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_MEASURE_FLAG, BIOS_MEASURE_DONE, DF_NONE);
        }
        if (ret != RET_OK) {
            return FRU_HOP_ON_FAILED_BY_BIOS_ERR;
        }
    }
    
    if (check_bios_measured_result() != RET_OK) {
        debug_log(DLOG_ERROR, "forbid power on, bios measured failed");
        return FRU_HOP_ON_FAILED_BY_BIOS_ERR;
    }
#endif

    hop_wait_power_on_permit();

    
    return power_on_by_acpi(fru_device_id);
}


guchar hop_off(guchar fru_device_id)
{
    gint32 i;
    guint32 timeout;
    guchar acpi = ALL_POWER_GOOD_FAIL;
    guchar ret_val;

    
    if (0 == g_power_operate_sem4) {
        debug_log(DLOG_ERROR, "%s:g_power_operate_sem4 is 0\n", __func__);
        return PP_POWER_OFF_FAIL;
    }
    (void)vos_thread_sem4_p(g_power_operate_sem4, SEM_WAIT_FOREVER);

    {
        hop_set_pwr_off_abort(FALSE);

        // 短按一下按钮，触发ACPI关机
        hop_get_acpi_status(fru_device_id, &acpi);

        if (ALL_POWER_GOOD_OK == acpi) {
            debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "hop_off:push short power button.");

            // 短按一下按钮
            (void)hop_push_pwr_button_short(fru_device_id, CTRL_BUTTON_OFF);
        } else if (ACPI_S3_SLEEPING == acpi) {
            // 目前规格没有声明支持S3状态，也没有明确的方案，但为了状态处理的完整性而增加了这个分支
            // 此处暂借用BH23C的做法，S3状态下先短按power button将系统唤醒，再次短按后触发系统下电
            (void)hop_push_pwr_button_short(fru_device_id, CTRL_BUTTON_OFF);
            
            vos_task_delay(3000);
            // 如果短按动作完成后单板还未被唤醒，就不要再按power button了，以免出现异常
            hop_get_acpi_status(fru_device_id, &acpi);
            if (acpi == ALL_POWER_GOOD_OK) {
                (void)hop_push_pwr_button_short(fru_device_id, CTRL_BUTTON_OFF);
            }
        } else {
            ret_val = PP_SHORT_PUSH_POWRE_OFF_T;
            g_printf("\r\nhop_off:already power off.\r\n");
            debug_log(DLOG_ERROR, "\r\nhop_off:already power off.\r\n");
            goto power_off_ok;
        }

        guint32 timeout_en = pp_get_pwr_off_timeout_en(fru_device_id);
        // 如果禁止了超时强制下电功能，超时了还未完成下电，直接返回下电失败
        if (timeout_en == TIMEOUT_DISABLE) {
            ret_val = PP_SHORT_PUSH_POWRE_OFF_F;
            debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "hop_off:power off timeout is disabled.");
            goto power_off_ok;
        }
        timeout = pp_get_pwr_off_timeout(fru_device_id);
        debug_log(DLOG_ERROR, "Wait short button power-off duration : %u*1000ms", timeout);
        // 循环检测下电指示信号
        for (i = 0; i < (gint32)timeout; i++) {
            
            vos_task_delay(1000); 
            hop_get_acpi_status(fru_device_id, &acpi);
            if (acpi == ALL_POWER_GOOD_FAIL) {
                ret_val = PP_SHORT_PUSH_POWRE_OFF_T;
                g_printf("\r\nhop_off:power off successfully.\r\n");
                debug_log(DLOG_ERROR, "\r\nhop_off:power off successfully.\r\n");
                goto power_off_ok;
            }

            if (TRUE == hop_get_pwr_off_abort()) {
                ret_val = PP_SHORT_PUSH_POWRE_OFF_F;
                g_printf("\r\nhop_off:abort power off.\r\n");
                debug_log(DLOG_ERROR, "\r\nhop_off:abort power off.\r\n");
                goto power_off_ok;
            }
        }

        g_printf("\r\nhop_off:push long power button.");
        debug_log(DLOG_ERROR, "\r\nhop_off:push long power button.");

        set_wol_by_saved_value(WOL_STATE_OFF);

        strategy_log(SLOG_INFO, "Fru%d normal power off failed, then forced power off it.\n", fru_device_id);

        // 超时后长按按钮
        hop_get_acpi_status(fru_device_id, &acpi);

        if (acpi != ALL_POWER_GOOD_FAIL) {
            hop_push_pwr_button_long(fru_device_id, CTRL_BUTTON_FORCE_OFF);
        }

        if (HARD_POWER_OFF == hop_get_pwr_signal(fru_device_id)) {
            ret_val = PP_LONG_PUSH_POWRE_OFF_T;
            g_printf("\r\nhop_off:power off successfully.\r\n");
            debug_log(DLOG_ERROR, "\r\nhop_off:power off successfully.\r\n");
            goto power_off_ok;
        } else {
            ret_val = PP_POWER_OFF_FAIL;
            goto power_off_fail;
        }
    }

power_off_ok:
    pp_set_power_ctrl_fault(fru_device_id, POWER_OFF, FALSE);
    (void)vos_thread_sem4_v(g_power_operate_sem4);
    return ret_val;

power_off_fail:
    g_printf("\r\nhop_off:power off fail.\r\n");
    debug_log(DLOG_ERROR, "\r\nhop_off:power off fail.\r\n");
    pp_set_power_ctrl_fault(fru_device_id, POWER_OFF, TRUE);
    (void)vos_thread_sem4_v(g_power_operate_sem4);
    return ret_val;
}


void set_wol_by_saved_value(guint8 wol_state)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    guint8 saved_wol_state = 0;

    debug_log(DLOG_DEBUG, "wol_state:%d", wol_state);

    ret_val = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    if (ret_val != RET_OK) {
        return;
    }

    
    ret_val = dal_get_saved_value_byte(obj_handle, PROPERTY_WOL_STATE, &saved_wol_state, DF_NONE);
    debug_log(DLOG_DEBUG, "saved_wol_state:0x%x, ret_val:%d", saved_wol_state, ret_val);

    if (ret_val != RET_OK) {
        
        return;
    }

    if (WOL_STATE_OFF == saved_wol_state) {
        return;
    }

    
    ret_val = dal_set_property_value_byte(obj_handle, PROPERTY_WOL_ACCESSOR, wol_state, DF_AUTO);
    if (ret_val == RET_OK) {
        debug_log(DLOG_ERROR, "Set Wake On Lan State(%s) successfully",
            wol_state == WOL_STATE_ON ? "enabled" : "disabled");
    } else {
        debug_log(DLOG_ERROR, "Set Wake On Lan State(%s) failed",
            wol_state == WOL_STATE_ON ? "enabled" : "disabled");
    }
    return;
}


LOCAL void hop_force_off_task(guint32 fru_device_id)
{
    (void)prctl(PR_SET_NAME, (gulong)"HopForceOff");

    
    if (0 == g_power_operate_sem4) {
        debug_log(DLOG_ERROR, "%s:g_power_operate_sem4 is 0\n", __func__);
        g_push_pwr_btn_flag = PUSH_PWR_BTN_NO;
        g_mutex_unlock(&g_force_pwr_off_mutex);
        return;
    }
    (void)vos_thread_sem4_p(g_power_operate_sem4, SEM_WAIT_FOREVER);

    if (HARD_POWER_OFF == hop_get_pwr_signal((guchar)fru_device_id)) {
        debug_log(DLOG_DEBUG, "%s: already in power off state!", __FUNCTION__);
        pp_set_power_ctrl_fault(fru_device_id, POWER_OFF, FALSE); // re: BH8D00582, modified on June 5th, 2009
        // 如果payload已经处于下电状态，就不要再按power button，否则会出现先上电再下电的现象
        (void)vos_thread_sem4_v(g_power_operate_sem4);
        g_push_pwr_btn_flag = PUSH_PWR_BTN_NO;
        g_mutex_unlock(&g_force_pwr_off_mutex);
        return;
    }

    if (fru_device_id == CHASSIS_FRU_ID) {
        set_wol_by_saved_value(WOL_STATE_OFF);
        hop_push_pwr_button_long(fru_device_id, CTRL_BUTTON_FORCE_OFF);
        debug_log(DLOG_ERROR, "%s:push long power button.", __FUNCTION__);
        if (HARD_POWER_OFF == hop_get_pwr_signal((guchar)fru_device_id)) {
            pp_set_power_ctrl_fault(fru_device_id, POWER_OFF, FALSE);

            // 更新ac lost之前的电源状态
            pp_set_pwr_state_before_ac_lost(CHASSIS_FRU_ID, PAYLOAD_POWER_STATE_OFF);
        } else {
            pp_set_power_ctrl_fault(fru_device_id, POWER_OFF, TRUE);

            // 现网有执行强制下电后结果没有下电的问题, 此处增加日志用于快速定界问题
            maintenance_log_v2(MLOG_ERROR, FC_BMC_FORCE_POWER_OFF_FAIL, "Fru%d forced power off failed.",
                fru_device_id);
        }
    } else {
        g_push_pwr_btn_flag = PUSH_PWR_BTN_NO;
    }
    (void)vos_thread_sem4_v(g_power_operate_sem4);
    g_mutex_unlock(&g_force_pwr_off_mutex);
    return;
}


guchar hop_force_off(guchar fru_device_id)
{
    LOCAL TASKID force_pwr_off_taskid = 0;

    if (TRUE == g_mutex_trylock(&g_force_pwr_off_mutex)) {
        if (RET_OK != vos_task_create(&force_pwr_off_taskid, "forcepoweroff", (TASK_ENTRY)hop_force_off_task,
            UINT8_TO_POINTER(fru_device_id), DEFAULT_PRIORITY)) {
            debug_log(DLOG_ERROR, "Failed to create task forcepoweroff");
            g_mutex_unlock(&g_force_pwr_off_mutex);
            return FALSE;
        }
    }
    return TRUE;
}

gboolean is_arm_enable(void)
{
    gint32 ret;
    static guchar arm_enable = 0xff; 

    if (arm_enable == 0xff) { 
        ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
        if (ret != RET_OK) {
            arm_enable = 0xff; 
            return FALSE;
        }
    }

    return arm_enable ? TRUE : FALSE;
}


gint32 hop_get_bbu_present(void)
{
    guint8 bbu_num = 0;
    gint32 ret;
    gint32 bbu_present = FALSE;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_handle failed, result is %d.", ret);
        return bbu_present;
    }
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_BBU_MODULE_NUM, &bbu_num);
    if (bbu_num != 0) {
        ret = dal_get_object_handle_nth(CLASS_BBU_MODULE_NAME, 0, &obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "dfl_get_object_handle failed, result is %d.", ret);
            return bbu_present;
        }
        bbu_present = (ret == RET_OK) ? TRUE : FALSE;
    }
    return bbu_present;
}


gint32 hop_reset(guchar fru_device_id)
{
    gint32 result;

    
    if ((hop_get_bbu_present() == TRUE) && (is_dft_enable_status() == FALSE)) {
        debug_log(DLOG_ERROR, "Can't support force reset action with bbu.");
        return COMP_CODE_CMD_INVALID;
    }
    result = hop_check_fru_ctl_option(FRU_CONTROL_COLD_RESET, fru_device_id);
    if (result == RET_OK) {
        // 执行具体的热重启动作
        (void)hop_fru_hard_reset(fru_device_id);
        debug_log(DLOG_ERROR, "%s:fru hard reset", __FUNCTION__);
        return COMP_CODE_SUCCESS;
    } else {
        debug_log(DLOG_ERROR, "%s: status invalid.", __FUNCTION__);
        return COMP_CODE_STATUS_INVALID;
    }
}


gint32 hop_powercycle(guchar fru_device_id)
{
    gint32 result;

    result = hop_check_fru_ctl_option(FRU_CONTROL_GRACEFUL_REBOOT, fru_device_id);
    if (result == RET_OK) {
        if (FALSE == pp_is_cycle_in_progress(fru_device_id)) {
            pp_set_cycle_interval(fru_device_id, pp_get_power_cycle_interval());
            pp_set_cycle_progress_state(fru_device_id, TRUE);
            pp_save_cycle_param();
            return COMP_CODE_SUCCESS;
        } else {
            debug_log(DLOG_ERROR, "power cycle in progress.");
            return COMP_CODE_BUSY;
        }
    } else {
        debug_log(DLOG_ERROR, "status invalid.");
        return COMP_CODE_STATUS_INVALID;
    }
}


gint32 hop_diag_interrupt(guchar fru_device_id)
{
    gint32 result;

    result = hop_check_fru_ctl_option(FRU_CONTROL_ISSUEDIAGNOSTICINTERRUPT, fru_device_id);
    if (result == RET_OK) {
        return pp_chassis_nmi_interrupt();
    } else {
        return COMP_CODE_STATUS_INVALID;
    }
}


LOCAL gint32 _hop_get_fru_target(gchar fru_device_id, gint8 *target)
{
    gint32 result;
    OBJ_HANDLE fru_handle = 0;
    GVariant *property_value = NULL;
    gint8 temp;

    result = dfl_get_binded_object(CLASS_FRU, fru_device_id, &fru_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "find fru%d obj faile!\r\n", fru_device_id);
        return result;
    }

    result = dfl_get_property_value(fru_handle, PROPERTY_FRU_TARGET, &property_value);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get fru%d prop Target faile!\r\n", fru_device_id);
        return result;
    }
    temp = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    *target = temp;
    return RET_OK;
}


LOCAL gint32 _hop_fru_ctrl_warm_reset(gint8 fru_device_id)
{
    gpointer resp_ipmi = NULL;
    const guint8 *resp_data = NULL;
    IPMI_REQ_MSG_HEAD_S req_header;
    gint8 target = 0;
    gint32 result;

    result = _hop_get_fru_target(fru_device_id, &target);
    if (result != RET_OK) {
        return result;
    }

    req_header.target_type = (TARGET_TYPE_E)target;
    req_header.target_instance = 0x00;
    req_header.lun = 0x00;
    req_header.netfn = NETFN_APP_REQ;
    req_header.cmd = IPMI_COLD_RESET;
    req_header.src_len = 0;

    if (RET_OK != ipmi_send_request(&req_header, NULL, 1, &resp_ipmi, TRUE)) {
        return result;
    }
    debug_log(DLOG_ERROR, "%s:send ipmi command for warm reset.", __FUNCTION__);
    resp_data = (const guint8 *)get_ipmi_src_data((gconstpointer)resp_ipmi);
    if (resp_data == NULL) {
        debug_log(DLOG_ERROR, "get_ipmi_src_data  resp_data is NULL.\n");
        g_free(resp_ipmi);
        return RET_ERR;
    }
    if (resp_data[0] == COMP_CODE_SUCCESS) {
        result = RET_OK;
    }
    g_free(resp_ipmi);

    return result;
}


guchar hop_fru_warm_reset(guchar fru_device_id, guchar channel, guchar restart_cause)
{
    gint32 result;
    OBJ_HANDLE physical_partition_handle = 0;
    
    result = hop_check_fru_ctl_option(FRU_CONTROL_WARM_RESET, fru_device_id);
    if (result != RET_OK) {
        
        if (RET_OK == dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &physical_partition_handle)) {
            hop_chassis_hard_reset();
            pp_set_restart_action(channel, restart_cause);
            pp_set_restart_cause();
            return COMP_CODE_SUCCESS;
        }
        
        
        return COMP_CODE_STATUS_INVALID;
    }
    
    result = _hop_fru_ctrl_warm_reset(fru_device_id);
    return (guchar)result;
}


void hop_fru_hard_reset(guchar fru_device_id)
{
    gint32 result;
    OBJ_HANDLE fru_handle = 0;
    GVariant *property_data = NULL;
    gint8 temp_data;
    guint32 product_id = 0;

    (void)dal_get_product_id(&product_id);

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_device_id, &fru_handle);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "_hop_get_fru_target: find fru%d obj faile!\r\n", fru_device_id);
        return;
    }

    temp_data = FALSE;
    result = dal_set_property_value_byte(fru_handle, PROPERTY_PAYLOAD_HARD_RESET_WP, temp_data, DF_HW);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "set fru%d object HardResetWp fail!(result=%d)", fru_device_id, result);
    }

    
    if (is_support_storage_payload() == TRUE) {
        result = dal_set_property_value_byte(fru_handle, PROPERTY_PAYLOAD_HARD_RESET, temp_data, DF_HW);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "set fru%d object HardReset fail!(result=%d)", fru_device_id, result);
        }
        vos_task_delay(10); 
    }

    
    temp_data = TRUE;
    property_data = g_variant_new_byte(temp_data);
    result = dfl_set_property_value(fru_handle, PROPERTY_PAYLOAD_HARD_RESET, property_data, DF_HW);
    g_variant_unref(property_data);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "set fru%d object HardReset fail!(result=%d)1\r\n", fru_device_id, result);
    }
    result = dal_set_property_value_byte(fru_handle, PROPERTY_PAYLOAD_HARD_RESET_WP, temp_data, DF_HW);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "set fru%d object HardResetWp fail!(result=%d)", fru_device_id, result);
    }

    return;
}


guchar hop_get_pwr_signal(guchar fru_device_id)
{
    guchar acpi = ALL_POWER_GOOD_FAIL;

    hop_get_acpi_status(fru_device_id, &acpi);
    // S3状态也应视作上电
    if (ALL_POWER_GOOD_OK == acpi) {
        // 上电
        return HARD_POWER_ON;
    } else {
        // 下电
        return HARD_POWER_OFF;
    }
}


guchar hop_get_pwrpg_signal(guchar fru_device_id)
{
    guchar pwrpg = ALL_POWER_GOOD_FAIL;

    hop_get_pwrpg_status(fru_device_id, &pwrpg);
    if (ALL_POWER_GOOD_OK == pwrpg) {
        return HARD_POWER_ON;
    } else {
        return HARD_POWER_OFF;
    }
}


gint32 hop_trigger_nmi_interrupt(void)
{
    return hop_set_nmi_interrupt();
}


guint32 hop_get_host_start_timeout(void)
{
    gint32 result;
    // 默认配置时间为3分钟
    guint32 start_timeout = 180000;
    OBJ_HANDLE object_handle = 0;
    GVariant *property_data = NULL;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_HOST_START_TM, &property_data, DF_NONE);
        if (result != DFL_OK) {
            return start_timeout;
        }

        start_timeout = g_variant_get_uint32(property_data);
        g_variant_unref(property_data);
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return start_timeout;
}


LOCAL void hop_start_sys_reset_process(guchar fruid)
{
    guchar acpi = ALL_POWER_GOOD_FAIL;

    // 如果检测到系统复位，需要更新ChassisPayload对象的复位标志属性
    if (RET_OK == pp_check_src_from_reset()) {
        pp_update_reset_flag(1);
    }

    // 如果检测到系统复位，需要启动host检测定时器
    pp_start_host_checker();

    // 系统处于上电状态，又检测到复位信号，证明系统正在复位过程中
    hop_get_acpi_status(fruid, &acpi);
    if (ALL_POWER_GOOD_OK == acpi) {
        pp_set_restart_state(fruid, RESTART_PROCESSING);
    }
}


LOCAL void hop_clear_sys_reset_flag_task(void)
{
    (void)prctl(PR_SET_NAME, (gulong)"ClearSysReset");

    // 延迟30000ms再清理复位标志，保证复位能被其他app监听到
    vos_task_delay(30000);

    // 需要清理掉ChassisPayload对象的复位标志属性
    pp_update_reset_flag(0);

    // 将任务id设置为非法id
    g_reset_clr_task = 0;
}


LOCAL void hop_finish_sys_reset_process(guchar fruid)
{
    if (RET_OK == pp_check_src_from_reset()) {
        if (g_reset_clr_task == 0) {
            (void)vos_task_create(&g_reset_clr_task, "clr_reset", (TASK_ENTRY)hop_clear_sys_reset_flag_task, 0,
                DEFAULT_PRIORITY);
        }
        // 清除复位源
        (void)hop_clear_system_reset_src();
    }
}


LOCAL gint32 hop_set_property_val_byte(guchar fru_id, gchar *property_val, guint8 status)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guint8 temp_data;

    result = dfl_get_binded_object(CLASS_PAYLOAD, fru_id, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, property_val, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_value %s fail(result=%d)\r\n", property_val, result);
        }

        temp_data = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        if (temp_data != status) {
            property_data = g_variant_new_byte(status);
            result = dfl_set_property_value(object_handle, property_val, property_data, DF_NONE);
            g_variant_unref(property_data);
            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "dfl_set_property_value %s fail(result=%d)", property_val, result);
            }
        }
    } else {
        debug_log(DLOG_MASS, "dfl_get_object_handle %s fail(fru_id:%d result=%d)", OBJECT_CHASSISPAYLOAD, fru_id,
            result);
    }

    return RET_OK;
}


LOCAL void hop_check_power_status_task(guint32 fru_id)
{
    guchar fruid = (guchar)fru_id;
    guchar acpi_status;
    guchar pwrpg_status;

    (void)prctl(PR_SET_NAME, (gulong)"CheckPwrState");

    // 增加延时，依赖逻辑检测，acpi晚于pg99ms，故此处增肌110ms延时
    vos_task_delay(110);

    // 获取pwrpg信号
    pwrpg_status = hop_get_pwrpg_signal(fruid);

    // 获取acpi信号
    acpi_status = hop_get_pwr_signal(fruid);

    debug_log(DLOG_MASS, "hop_check_power_status_task fru:%u:pwrpg_status:%u, acpi_status:%u!", fruid, pwrpg_status,
        acpi_status);

    if (((acpi_status == HARD_POWER_ON) && (pwrpg_status == HARD_POWER_OFF)) ||
        ((acpi_status == HARD_POWER_OFF) && (pwrpg_status == HARD_POWER_ON))) {
        
        (void)hop_set_property_val_byte(fru_id, PROPERTY_PAYLOAD_PWR_STATUS, TRUE);
    }

    // 将任务id设置为非法id
    g_power_status_task[fru_id] = 0;
}


gint32 hop_check_power_status(guchar fruid)
{
    guchar acpi_status;
    guchar pwrpg_status;

    // 获取pwrpg信号
    pwrpg_status = hop_get_pwrpg_signal(fruid);

    // 获取acpi信号
    acpi_status = hop_get_pwr_signal(fruid);
    // 比较两个信号是否相符，不相符就更新属性，便于通知传感器
    if (((acpi_status == HARD_POWER_ON) && (pwrpg_status == HARD_POWER_OFF)) ||
        ((acpi_status == HARD_POWER_OFF) && (pwrpg_status == HARD_POWER_ON))) {
        // 不一致时，起个任务延时至少100ms，再次检测两个信号是否一致，
        // 仍不一致时，确定为信号异常，产生传感器事件
        if (g_power_status_task[fruid] == 0) {
            (void)vos_task_create(&g_power_status_task[fruid], "power_status", (TASK_ENTRY)hop_check_power_status_task,
                UINT8_TO_POINTER(fruid), DEFAULT_PRIORITY);
        }
    } else {
        
        (void)hop_set_property_val_byte(fruid, PROPERTY_PAYLOAD_PWR_STATUS, FALSE);
    }

    return PAYLOAD_SUCCESS;
}


LOCAL gboolean hop_is_sys_reset_by_ac_sp_collect(void)
{
#define SP_DEVICE_INFO_COLLECT_STATE_IMPL_BY_AC 2
    gint32 ret;
    guchar restart_cause;
    guchar restart_chan;
    OBJ_HANDLE object_handle;
    OBJ_HANDLE sp_ums_obj;
    guchar collect_state;

    ret = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get Payload object fail, result is %d", ret);
        return FALSE;
    }
    ret = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_RESTART_CAUSE, &restart_cause);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get RestartCause fail, result is %d", ret);
        return FALSE;
    }
    ret = dal_get_property_value_byte(object_handle, PROPERTY_PAYLOAD_RESTART_CHAN, &restart_chan);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get RestartChannel fail, result is %d", ret);
        return FALSE;
    }
    
    if (restart_cause != RESTART_CAUSE_ALWAYSRESTORE || restart_chan != SRC_CHAN_NUM_RPC) {
        return FALSE;
    }

    ret = dal_get_object_handle_nth(SP_UMS_CLASS_NAME, 0, &sp_ums_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get USBMassStorage0 object fail, result is %d", ret);
        return FALSE;
    }
    
    ret = dal_get_property_value_byte(sp_ums_obj, UMS_PROPERTY_SP_DEVICE_INFO_COLLECT_STATE, &collect_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get SPDeviceInfoCollectState fail, result is %d", ret);
        return FALSE;
    }
    
    if (collect_state != SP_DEVICE_INFO_COLLECT_STATE_IMPL_BY_AC) {
        return FALSE;
    }
    
    return TRUE;
}


LOCAL void hop_set_system_reset_cause(void)
{
    if (hop_is_sys_reset_by_ac_sp_collect()) {
        pp_set_restart_action(0, RESTART_CAUSE_RESETBYACSPCOLLECT);
        pp_set_restart_cause();
        pp_update_restart_value();
    } else {
        pp_update_restart_value();
        pp_deal_with_restart_cause_flag();
    }
}


guint32 hop_check_host_reset(guchar fruid)
{
    guchar reset_status = FALSE;
    static guint32 last_time = 0;
    guint32 current_time;
    guint32 diff;
    guchar button_flag = BUT_EVT_FALSE;
    guint32 bmc_ver = 0;
    (void)dal_get_product_version_num(&bmc_ver);
    guint32 scan_cycle = (bmc_ver > PRODUCT_VERSION_V5) ? SENSOR_SCAN_CYCLE_V6 : SENSOR_SCAN_CYCLE;

    // 底板电源状态发生变化，需要检测系统是否已经复位
    if (fruid == CHASSIS_FRU_ID) {
        // 目前只有底板才需要检测复位信号
        if (0 == hop_get_system_reset_flag(&reset_status)) {
            if (TRUE == reset_status) {
                
                current_time = vos_tick_get();
                if (current_time > last_time) {
                    diff = current_time - last_time;
                } else {
                    diff = ((0xFFFFFFFF - last_time) + current_time);
                }
                debug_log(DLOG_INFO, "detect system reset,%d microseconds later than last reset\r\n", diff);

                if (diff < scan_cycle) {
                    last_time = current_time;
                    (void)hop_clear_system_reset_flag();
                    return PAYLOAD_FAIL;
                }

                last_time = current_time;
                (void)hop_get_pwr_button_event(&button_flag);

                if (BUT_EVT_TRUE != button_flag) {
                    hop_set_system_reset_cause();
                } else {
                    
                    // 设置复位标志已经产生，用于记录重启原因
                    if (FALSE == pp_is_pwr_button_test_in_progress()) {
                        debug_log(DLOG_DEBUG, "pp_is_pwr_button_test_in_progress is false.\r\n");
                        hop_set_reset_event_from_pwr(BUT_EVT_TRUE);
                    }
                }
                // 已经检测到复位标志，可以清除了
                (void)hop_clear_system_reset_flag();

                // 启动检测到系统复位后的处理过程
                hop_start_sys_reset_process(fruid);

                // 结束复位检测过程，清除对象中的复位标志
                hop_finish_sys_reset_process(fruid);

                
                (void)hse_unnormal_status_recovery(fruid);
            }
        }
        // 检测BIOS是否启动异常，如果异常且有双BIOS，则切换一个BIOS
        if (FALSE == hop_is_bios_boot_ok(fruid)) {
            // 查询是否双BIOS，如果是，则切换
            (void)hop_ctrl_bios_switch(fruid, SW_BIOS_ACTION_HW);
            (void)hop_clear_bios_boot_status(fruid);
        }
    }

    return PAYLOAD_SUCCESS;
}


guchar hop_is_fru_present(guchar fruid)
{
    return TRUE;
}


guchar hop_get_fru_site_num(guchar fruid)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val;
    GVariant *property_data = NULL;
    guchar xml_slot_id = 1;

    ret_val = dfl_get_object_handle(BMC_BOARD_OBJ_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return xml_slot_id;
    }

    ret_val = dfl_get_property_value(obj_handle, BMC_SLOT_ID_NAME, &property_data);
    if (ret_val != DFL_OK) {
        
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return xml_slot_id;
    }

    xml_slot_id = g_variant_get_byte(property_data);
    g_variant_unref(property_data);

    return (guchar)(xml_slot_id != 0 ? xml_slot_id : 1);
}

guchar hop_get_imana_reset_type(void)
{
    gint32 reset_type = RESET_TYPE_POWER_UP;

    if (dal_get_bmc_reset_type(&reset_type) != RET_OK) {
        debug_log(DLOG_ERROR, "get reset type error!");
        return HARD_RESET;
    }

    // 只有上电复位才是真正的硬复位
    if (reset_type == RESET_TYPE_POWER_UP) {
        return HARD_RESET;
    } else {
        return SOFT_RESET;
    }
}


LOCAL gint32 hop_get_fru_insert_signal(guint8 fru_id, guchar *value)
{
    gint32 result;
    guchar tmp0;
    guchar tmp1;
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;

    // 首先赋值为未插入状态
    *value = FRU_NOT_INSERT;

    result = dfl_get_binded_object(CLASS_PAYLOAD, CHASSIS_FRU_ID, &object_handle);
    if (result == DFL_OK) {
        // 先取对象的InsertStatus0
        result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_INSERT_STATUS0, &property_data, DF_COPY);
        if (result != DFL_OK) {
            return result;
        }
        tmp0 = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        // 先取对象的InsertStatus1
        result = dfl_get_extern_value(object_handle, PROPERTY_PAYLOAD_INSERT_STATUS1, &property_data, DF_COPY);
        if (result != DFL_OK) {
            return result;
        }
        tmp1 = g_variant_get_byte(property_data);
        g_variant_unref(property_data);

        // 同时检测到两个插稳信号，证明单板已插稳，信号低有效
        if ((tmp0 == INSERT_SIGNAL_VALID) && (tmp1 == INSERT_SIGNAL_VALID)) {
            *value = FRU_INSERT;
        }
    } else {
        debug_log(DLOG_ERROR, "get ChassisPayload object fail!(result=%d)\r\n", result);
    }

    return RET_OK;
}


LOCAL gint32 hop_get_board_insert(guint8 fru_id, guchar *value)
{
    gint32 retv = 0;
    guchar board_insert = FRU_NOT_INSERT;
    
    if (TRUE == pp_is_insert_check_en(fru_id)) {
        retv = hop_get_fru_insert_signal(fru_id, &board_insert);
        *value = board_insert;
    } else {
        // 不需要插稳检测，直接返回插稳
        *value = FRU_INSERT;
    }

    return retv;
}


LOCAL gint32 hop_get_handle_switch_status(guint8 fruid, guint8 handle, guchar *val)
{
    
    gint32 retv = 0;

    *val = 0;
    return retv;
}


LOCAL gint32 hop_get_criteria_status(guint8 fru_id, guchar *value)
{
    guchar top_status;
    guchar bottom_status;

    if (hop_get_handle_switch_status(fru_id, 0, &top_status) != 0) {
        return PAYLOAD_FAIL;
    }

    if (hop_get_handle_switch_status(fru_id, 1, &bottom_status) != 0) {
        return PAYLOAD_FAIL;
    }

    // 只要有1个扳手合，就算扳手合上
    if ((0 == top_status) || (0 == bottom_status)) {
        *value = 1;
    } else {
        *value = 0;
    }

    return PAYLOAD_SUCCESS;
}


LOCAL FRU_EVENT_S init_fru_event(void)
{
    FRU_EVENT_S fru_event;
    fru_event.fruid = 0;
    fru_event.event = FRU_PIN_MATED;
    fru_event.value = 0;
    fru_event.casue = FRU_HS_NORMAL_STATE_CHANGE;
    fru_event.count = 0;

    return fru_event;
}

LOCAL void hop_check_pin_state(guint32 *checktime, guint8 fru_id)
{
    guchar value = 0;
    // 连续5次检查插稳状态间隔100ms检查一次
    for (guint32 i = 0; i < DETECT_TIME; i++) {
        gp_pin_mated_cb[fru_id].m_cnt <<= 1;

        // 执行插稳检查句柄，获取对应fru的插稳状态
        if (gp_pin_mated_cb[fru_id].m_fp_chker(0, &value) == RET_OK) {
            if (value) {
                gp_pin_mated_cb[fru_id].m_cnt |= 1;
            }
        }
        vos_task_delay(FRU_DETECT_TIME);
    }
    *checktime += FRU_DETECT_TIME * DETECT_TIME;
    
    if (*checktime >= 1000 * COUNT_SECOND_ONE_DAY) {
        *checktime = 0;
        if (value == 0) {
            maintenance_log_v2(MLOG_WARN, FC_BMC_NOT_INSERT, "Check the InsertStatus fail\r\n");
        }
    }
    return;
}


LOCAL void hop_check_pin_is_reliab(FRU_EVENT_S fru_event, guint8 fru_id, guint8 ser_type)
{
    static guint8 button_lock_mutex = FALSE;
    // 连续监测到5次插入才算可靠的插入检查
    if ((gp_pin_mated_cb[fru_id].m_cnt & 0x1f) == 0x1f) {
        if (gp_pin_mated_cb[fru_id].m_pin_mated == 0) {
            fru_event.value = 1;
            gp_pin_mated_cb[fru_id].m_pin_mated = 1;
            hs_send_evt(fru_id, &fru_event);
        }

        
        if (ser_type == SERVER_TYPE_BLADE) {
            (void)hop_set_property_val_byte(fru_id, PROPERTY_PAYLOAD_STABLE_STATUS, FALSE);
        }

        
        guint8 tmp_val = (ser_type == SERVER_TYPE_BLADE) && (fru_id == CHASSIS_FRU_ID) &&
            (pp_is_insert_check_en(fru_id) == TRUE) &&
            (pp_is_pwr_button_test_in_progress() == FALSE && (pp_is_power_on_criteria_met(fru_id) == TRUE));
        if (tmp_val == TRUE) {
            // 检测到插稳就同步持久化数据
            if (button_lock_mutex == FALSE) {
                (void)hop_synch_pwr_button_lock();
                button_lock_mutex = TRUE;
            }
        }
    } else if (gp_pin_mated_cb[fru_id].m_cnt == 0) {
        if (gp_pin_mated_cb[fru_id].m_pin_mated == 1) {
            fru_event.value = 0;
            gp_pin_mated_cb[fru_id].m_pin_mated = 0;
            hs_send_evt(fru_id, &fru_event);
        }

        
        if (ser_type == SERVER_TYPE_BLADE) {
            (void)hop_set_property_val_byte(fru_id, PROPERTY_PAYLOAD_STABLE_STATUS, TRUE);
        }

        
        guint8 tmp_val =
            (SERVER_TYPE_BLADE == ser_type) && (CHASSIS_FRU_ID == fru_id) && (pp_is_insert_check_en(fru_id) == TRUE);
        if (tmp_val == TRUE) {
            // 检测到未插稳，登不上web，无需考虑web按钮屏蔽动作
            (void)hop_set_pwr_button_lock(TRUE, NO_PER_LOCK);
            // 检测到未插稳，解锁按钮
            button_lock_mutex = FALSE;
        }
    }
    return;
}


void hop_check_fru_pin_task(void)
{
    guint32 checktime = 0;
    guint8 ser_type = 0;

    FRU_EVENT_S fru_event = init_fru_event();
    guint32 fru_num = hs_get_fru_num();

    (void)prctl(PR_SET_NAME, (gulong)"CheckFruPin");

    
    (void)pp_get_server_type(&ser_type);

    for (;;) {
        for (guint8 fru_id = 0; fru_id < fru_num; fru_id++) {
            // 未注册检查句柄，跳过，增加延迟，避免死循环消耗CPU资源
            if (gp_pin_mated_cb[fru_id].m_fp_chker == NULL) {
                vos_task_delay(FRU_DETECT_TIME);
                continue;
            }
            fru_event.fruid = fru_id;

            // 连续5次检查插稳状态间隔100ms检查一次
            hop_check_pin_state(&checktime, fru_id);

            // 连续监测到5次插入才算可靠的插入检查
            hop_check_pin_is_reliab(fru_event, fru_id, ser_type);

            debug_log(DLOG_DEBUG, "gp_pin_mated_cb[%d].m_cnt=0x%0X\r\n", fru_id, gp_pin_mated_cb[fru_id].m_cnt);
        }
        vos_task_delay(FRU_CHECK_INTERVAL_TIME);
        checktime += FRU_CHECK_INTERVAL_TIME;
    }
}


LOCAL void hop_criteria_check_state(FRU_EVENT_S fru_event, guint8 fru_id)
{
    guchar value = 0;
    fru_event.fruid = fru_id;
    gp_criteria_cb[fru_id].m_cnt <<= 1;

    // 执行插入条件检查句柄，获取对应fru的插入条件状态
    if (gp_criteria_cb[fru_id].m_fp_chker(fru_id, &value) == RET_OK) {
        if (value) {
            gp_criteria_cb[fru_id].m_cnt |= 1;
        }
    }
}


LOCAL void hop_criteria_check_is_reliab(FRU_EVENT_S fru_event, guint8 fru_id)
{
    if ((gp_criteria_cb[fru_id].m_cnt & 0x3ff) == 0x3ff) {
        // 满足插入条件有两个: 扳手合上 + 上电恢复策略上电时没有强制为下电状态
        if ((gp_criteria_cb[fru_id].m_criteria_mated != FRU_INSERTION_CRITERIA_MET)) {
            if (FALSE == pp_is_pwr_off_locked(fru_id)) {
                fru_event.event = FRU_INSERTION_CRITERIA_MET;
                fru_event.value = 1;
                gp_criteria_cb[fru_id].m_criteria_mated = FRU_INSERTION_CRITERIA_MET;
                per_save((guint8 *)&gp_criteria_cb[fru_id].m_criteria_mated);

                hs_send_evt(fru_id, &fru_event);
                g_printf("send fru:%u FRU_INSERTION_CRITERIA_MET\r\n", fru_id);
                debug_log(DLOG_ERROR, "send fru:%u FRU_INSERTION_CRITERIA_MET\r\n", fru_id);
            }
        }
    } else if ((gp_criteria_cb[fru_id].m_cnt & 0x3ff) == 0) {
        // 同样的，连续监测到10次拔出才算真正拔出
        if (gp_criteria_cb[fru_id].m_criteria_mated != FRU_EXTRACTION_CRITERIA_MET) {
            fru_event.event = FRU_EXTRACTION_CRITERIA_MET;
            fru_event.value = 1;
            gp_criteria_cb[fru_id].m_criteria_mated = FRU_EXTRACTION_CRITERIA_MET;
            per_save((guint8 *)&gp_criteria_cb[fru_id].m_criteria_mated);

            hs_send_evt(fru_id, &fru_event);
            g_printf("send fru:%u FRU_EXTRACTION_CRITERIA_MET\r\n", fru_id);
            debug_log(DLOG_ERROR, "send fru:%u FRU_EXTRACTION_CRITERIA_MET\r\n", fru_id);
        }
    }
}


void hop_criteria_check_task(void)
{
    FRU_EVENT_S fru_event;
    guint32 fru_num;

    fru_event.fruid = 0;
    fru_event.event = FRU_INSERTION_CRITERIA_MET;
    fru_event.value = 0;
    fru_event.casue = FRU_HS_NORMAL_STATE_CHANGE;
    fru_event.count = 0;

    fru_num = hs_get_fru_num();

    (void)prctl(PR_SET_NAME, (gulong)"CheckCriteria");

    for (;;) {
        for (guint8 fru_id = 0; fru_id < fru_num; fru_id++) {
            // 未注册检查句柄，跳过
            if (gp_criteria_cb[fru_id].m_fp_chker == NULL) {
                continue;
            }

            // 执行插入条件检查句柄，获取对应fru的插入条件状态
            hop_criteria_check_state(fru_event, fru_id);

            // 连续监测到10次插入才算可靠的插入检查
            hop_criteria_check_is_reliab(fru_event, fru_id);
        }
        vos_task_delay(100UL);
    }
}


LOCAL void hop_resister_pin_checker(guchar fru_id, insert_checker pf_checker)
{
    if (pf_checker == NULL) {
        debug_log(DLOG_ERROR, "hop_resister_pin_checker:pf_checker=null\r\n");
        return;
    }

    gp_pin_mated_cb[fru_id].m_cnt = 0;
    gp_pin_mated_cb[fru_id].m_pin_mated = 0;
    gp_pin_mated_cb[fru_id].m_fp_chker = pf_checker;
}


LOCAL void hop_resister_criteria_checker(guchar fru_id, insert_checker pf_checker)
{
    if (pf_checker == NULL) {
        debug_log(DLOG_ERROR, "hop_resister_pin_checker:pf_checker=null\r\n");
        return;
    }

    gp_criteria_cb[fru_id].m_cnt = 0;
    gp_criteria_cb[fru_id].m_criteria_mated = FRU_EXTRACTION_CRITERIA_MET;
    gp_criteria_cb[fru_id].m_fp_chker = pf_checker;
}


guchar hop_pin_checker_init(void)
{
    TASKID tid;
    guint32 fru_num;
    guint8 fru_id;
    guint32 malloc_size;

    // 申请内存用于保存控制信息
    fru_num = hs_get_fru_num();
    malloc_size = fru_num * sizeof(PIN_MATED_CB_T);
    gp_pin_mated_cb = (PPIN_MATED_CB_T)g_malloc0(malloc_size);
    if (gp_pin_mated_cb == NULL) {
        return PAYLOAD_FAIL;
    }
    if (gp_pin_mated_cb == NULL) {
        debug_log(DLOG_ERROR, "hop_pin_checker_init:gp_pin_mated_cb malloc fail\r\n");
        return PAYLOAD_ERESOURCE;
    }

    // 为每一个fru(不管是否可管理FRU)注册一个插入条件检测控制块
    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        if (TRUE == hs_is_fru_managed(fru_id)) {
            hop_resister_pin_checker(fru_id, hop_get_board_insert);
        }
    }

    (void)vos_task_create(&tid, "CheckInsert", (TASK_ENTRY)hop_check_fru_pin_task, 0, DEFAULT_PRIORITY);

    return PAYLOAD_SUCCESS;
}


guchar hop_criteria_checker_init(void)
{
    TASKID tid;
    guint32 fru_num;
    guint8 fru_id;
    guint32 malloc_size;
    PER_S per_data;
    gint32 ret_val;
    gchar key[HOTSWAP_TEMP_BUF_LEN] = {0};

    // 申请内存用于保存控制信息
    fru_num = hs_get_fru_num();
    malloc_size = fru_num * sizeof(CRITERIA_CB_T);
    gp_criteria_cb = (PCRITERIA_CB_T)g_malloc0(malloc_size);
    if (gp_criteria_cb == NULL) {
        debug_log(DLOG_ERROR, "hop_criteria_checker_init:gp_criteria_cb malloc fail\r\n");
        return PAYLOAD_ERESOURCE;
    }

    for (fru_id = 0; fru_id < fru_num; fru_id++) {
        // 为每一个fru(不管是否可管理FRU)注册一个满足条件检测控制块
        if (TRUE == hs_is_fru_managed(fru_id)) {
            hop_resister_criteria_checker(fru_id, hop_get_criteria_status);
        }

        // 恢复持久化
        (void)memset_s(key, sizeof(key), 0, sizeof(key));
        (void)snprintf_s(key, HOTSWAP_TEMP_BUF_LEN, HOTSWAP_TEMP_BUF_LEN - 1, "FRU%uCriteria", fru_id);
        per_data.key = key;
        per_data.mode = PER_MODE_RESET;
        per_data.data = (guint8 *)&gp_criteria_cb[fru_id].m_criteria_mated;
        per_data.len = sizeof(gp_criteria_cb[fru_id].m_criteria_mated);
        ret_val = per_init(&per_data, 1);
        if (ret_val != RET_OK) {
            debug_log(DLOG_ERROR, "FRU%dCriteria persistance fail!\r\n", fru_id);
        }
    }

    (void)vos_task_create(&tid, "CheckHandle", (TASK_ENTRY)hop_criteria_check_task, 0, DEFAULT_PRIORITY);

    return PAYLOAD_SUCCESS;
}


void hop_sem4_init(void)
{
    gint32 ret_val;
    
    ret_val = vos_thread_mutex_sem4_create(&g_power_operate_sem4, "g_power_operate_sem4");
    if (ret_val != 0) {
        debug_log(DLOG_ERROR, "%s:fail to create g_power_operate_sem4.", __FUNCTION__);
    }
    g_mutex_init(&g_force_pwr_off_mutex);
}
