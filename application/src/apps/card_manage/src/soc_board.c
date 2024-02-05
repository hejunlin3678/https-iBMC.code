


#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "pme_app/dalib/dal_obj_priv_data.h"

#include "get_version.h"
#include "pcie_card.h"
#include "pcie_card_comm.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "soc_board.h"

#define U32_BITS 32
// BMC重启后3小时内，除手动外不收集日志
gboolean g_is_soc_board_log_allowed = FALSE;
const guint8 INIT_MAX_RETRY_TIMES = 30;
// SoC主板在位信息。用法见soc_board_init
guint32 g_soc_board_present = 0;
#define BOARD_ID_ATLAS800D_G1 0x01

LOCAL const PM_CHIP_INFO g_default_chip_info = {
    .pf_info_list = NULL,
    .pf_update_delay_cnt = 0,
    .log_delay_time_for_rw = LOG_DELAY_TIME_FOR_RW,
    .common_delay_time_for_rw = COMMON_DELAY_TIME_FOR_RW,
    .collect_log_task_id = 0,
    .last_fault_code_len = 0,
    .log_dump_trigger = LOG_DUMP_TRIGGER_RESET_BMC,
    .ref_cnt = 1,
    .reset_mcu_state = RESET_MCU_IDLE,
    .temp_read_fail_timestamp = 0,
    .ddr_temp_read_fail_timestamp = 0,
    .power_read_fail_timestamp = 0,
    .optical_temp_read_fail_timestamp = 0,
    .optical_current_read_fail_timestamp = 0,
    .log_dump_trigger = LOG_DUMP_TRIGGER_UNRESET,
    .chip_reset_time = 10
};
LOCAL gint32 update_firmware_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info, guint8 *updated)
{
    struct McuVersion {
        guint8 major;
        guint8 minor;
        guint8 revision;
    };

    *updated = FALSE;

    struct McuVersion ver = { 0 };
    gint32 ret = pm_get_firmware(info, (guint8 *)&ver, sizeof(ver));
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        return ret;
    } else if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s pm_get_firm_version fail, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return ret;
    }
    if ((ver.major | ver.minor | ver.revision) == 0) {
        debug_log(DLOG_DEBUG, "%s: %s mcu version empty", __FUNCTION__, dfl_get_object_name(obj_handle));
        (void)dal_set_property_value_string(obj_handle, PROPERTY_SOC_BOARD_MCU_VER, NA_STR, DF_SAVE_TEMPORARY);
        return RET_ERR;
    }

    gchar ver_str[STR_FIRMWARE_LEN] = { 0 };
    if (ver.revision != 0xff) {
        (void)snprintf_s(ver_str, STR_FIRMWARE_LEN, STR_FIRMWARE_LEN - 1,
            "%u.%u.%u", ver.major, ver.minor, ver.revision);
    } else {
        (void)snprintf_s(ver_str, STR_FIRMWARE_LEN, STR_FIRMWARE_LEN - 1, "%u.%u", ver.major, ver.minor);
    }

    gchar ver_old[STR_FIRMWARE_LEN] = { 0 };
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SOC_BOARD_MCU_VER, ver_old, STR_FIRMWARE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s.McuVersion fail, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    if (strcmp(ver_str, ver_old) != 0) {
        ret = dal_set_property_value_string(obj_handle, PROPERTY_SOC_BOARD_MCU_VER, ver_str, DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: set %s.McuVersion fail, ret=%d", __FUNCTION__,
                dfl_get_object_name(obj_handle), ret);
            return ret;
        }
        *updated = TRUE;
    }
    return RET_OK;
}

LOCAL guint32 calc_read_fail_duration(guint32 *timestamp)
{
    guint32 duration = 0;
    guint32 cur_timestamp = (guint32)vos_tick_get();

    if (*timestamp == 0) {
        *timestamp = cur_timestamp;
    } else if (*timestamp <= cur_timestamp) {
        duration = cur_timestamp - *timestamp;
    } else {
        duration = cur_timestamp + (G_MAXUINT32 - *timestamp);
    }

    return duration;
}

LOCAL gint32 update_power(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info)
{
    guint16 power_old = 0;
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_POWER, &power_old);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s.Power fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    guint16 power = 0;
    ret = pm_get_power(info, (guint8 *)&power, sizeof(guint16));
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        return ret;
    } else if (ret != RET_OK) {
        guint32 duration = calc_read_fail_duration(&info->power_read_fail_timestamp);
        if (duration <= ((guint32)info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
            return ret;
        }
        if ((power_old & SENSOR_INVALID_READING) == SENSOR_INVALID_READING) {
            return ret;
        }
        power = power_old | SENSOR_INVALID_READING;
        debug_log(DLOG_DEBUG, "%s: %s get power fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    } else {
        info->power_read_fail_timestamp = 0;
        if (power == PCIE_SENSOR_INVALID_READING) {
            power = power_old | SENSOR_INVALID_READING;
        } else if (power == PCIE_SENSOR_NA_READING) {
            power = (power_old & PCIE_SENSOR_INVALID_READING) | SENSOR_NA_READING;
        }
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_POWER, power, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: set %s.Power fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 update_temp(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info)
{
    guint16 temp_old = 0;
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_TEMP, &temp_old);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s.ChipTemp fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    guint16 temp = 0;
    ret = pm_get_temp(info, (guint8 *)&temp, sizeof(guint16));
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        return ret;
    } else if (ret != RET_OK) {
        guint32 duration = calc_read_fail_duration(&info->temp_read_fail_timestamp);
        if (duration <= ((guint32)info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
            return ret;
        }
        if ((temp_old & SENSOR_INVALID_READING) == SENSOR_INVALID_READING) {
            return ret;
        }
        temp = temp_old | SENSOR_INVALID_READING;
        debug_log(DLOG_DEBUG, "%s: %s get chip temp fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    } else {
        info->temp_read_fail_timestamp = 0;
        if (temp == PCIE_SENSOR_INVALID_READING) {
            temp = temp_old | SENSOR_INVALID_READING;
        } else if (temp == PCIE_SENSOR_NA_READING) {
            temp = (temp_old & PCIE_SENSOR_INVALID_READING) | SENSOR_NA_READING;
        }
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_TEMP, temp, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: set %s.ChipTemp fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 update_device_temp(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info)
{
    GSList *temp_list = NULL;
    gint32 ret = pm_get_dev_temp(info, &temp_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s pm_get_dev_temp fail, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    for (GSList *node = temp_list; node != NULL; node = node->next) {
        OobDeviceTemp *dev_temp = (OobDeviceTemp *)node->data;
        OBJ_HANDLE sensor_handle = 0;
        ret = dal_get_specific_position_object_string(obj_handle, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR,
            PROPERTY_P_DEVICE_SENSOR_NAME, dev_temp->sensor_name, &sensor_handle);
        if (ret == RET_OK) {
            (void)dal_set_property_value_uint16(sensor_handle, PROPERTY_P_DEVICE_SENSOR_VALUE, dev_temp->temp, DF_NONE);
        }
    }
    g_slist_free_full(temp_list, g_free);
    return RET_OK;
}


LOCAL gboolean is_fault_code_changed(PM_CHIP_INFO *pm_chip_info, guint8 *fault_code_buf, guint32 fault_code_buf_len)
{
    guint32 last_buf_len = sizeof(pm_chip_info->last_fault_code);
    guint32 cmp_len = MIN(fault_code_buf_len, last_buf_len);
    if (pm_chip_info->last_fault_code_len == fault_code_buf_len &&
        memcmp(fault_code_buf, pm_chip_info->last_fault_code, cmp_len) == 0) {
        return FALSE;
    }
    (void)memset_s(pm_chip_info->last_fault_code, last_buf_len, 0, last_buf_len);
    (void)memcpy_s(pm_chip_info->last_fault_code, last_buf_len, fault_code_buf, cmp_len);

    pm_chip_info->last_fault_code_len = fault_code_buf_len;
    return TRUE;
}

LOCAL gint32 update_fault_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info)
{
    guint8 health_state = 0;
    gint32 ret = pm_get_health_state(info,  &health_state, sizeof(guint8));
    if (health_state == 0) {
        (void)dal_set_property_value_string(obj_handle, PROPERTY_SOC_BOARD_FAULT_CODE, OOB_ERROR_CODE_NA, DF_NONE);
        (void)dal_set_property_value_uint32(obj_handle, PROPERTY_SOC_BOARD_FAULT_STATE, 0, DF_NONE);
        return RET_OK;
    }

    guint8 *buf = NULL;
    guint32 buf_len = 0;
    ret = pm_get_fault_code(info, &buf, &buf_len);
    if (ret != RET_OK) {
        // 防止刷屏
        debug_log(DLOG_DEBUG, "%s: get fault code fail, ret=%d", __FUNCTION__, ret);
        return ret;
    }
    if (buf == NULL) {
        debug_log(DLOG_DEBUG, "%s: %s fault code is NULL", __FUNCTION__, dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    ret = update_fault_code(obj_handle, info, buf, buf_len, health_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update fault code fail, ret=%d", __FUNCTION__, ret);
        g_free(buf);
        return ret;
    }
    if (is_fault_code_changed(info, buf, buf_len)) {
        soc_board_start_log(obj_handle, NULL);
    }
    g_free(buf);
    return RET_OK;
}

LOCAL gint32 update_mcu_rtc_time(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info)
{
    pcie_card_update_mcu_rtc_time(obj_handle, info);
    return RET_OK;
}


LOCAL void soc_board_set_default_value(OBJ_HANDLE obj_handle, guint16 mask)
{
    guint16 get_val = 0;
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_TEMP, &get_val);
    if (ret == RET_OK) {
        (void)dal_set_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_TEMP,
            (get_val & PCIE_SENSOR_INVALID_READING) | mask, DF_NONE);
    }

    get_val = 0;
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_POWER, &get_val);
    if (ret == RET_OK) {
        (void)dal_set_property_value_uint16(obj_handle, PROPERTY_SOC_BOARD_POWER,
            (get_val & PCIE_SENSOR_INVALID_READING) | mask, DF_NONE);
    }

    GSList *sensor_list = NULL;
    ret = dal_get_object_list_position(obj_handle, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR, &sensor_list);
    for (GSList *node = sensor_list; node != NULL; node = node->next) {
        OBJ_HANDLE sensor_handle = (OBJ_HANDLE)node->data;
        get_val = 0;
        ret = dal_get_property_value_uint16(sensor_handle, PROPERTY_P_DEVICE_SENSOR_VALUE, &get_val);
        if (ret != RET_OK) {
            continue;
        }
        (void)dal_set_property_value_uint16(sensor_handle, PROPERTY_P_DEVICE_SENSOR_VALUE,
            (get_val & PCIE_SENSOR_INVALID_READING) | mask, DF_NONE);
    }
    g_slist_free(sensor_list);

    if (mask == SENSOR_INVALID_READING) {
        pcie_card_update_info_fail(obj_handle);
    } else {
        (void)dal_set_property_value_string(obj_handle, PROPERTY_SOC_BOARD_FAULT_CODE, OOB_ERROR_CODE_NA, DF_NONE);
        (void)dal_set_property_value_uint32(obj_handle, PROPERTY_SOC_BOARD_FAULT_STATE, 0, DF_NONE);
    }
    return;
}

LOCAL const SocBrdMntrItem g_soc_board_mntr_table[] = {
    {SOCBRD_MNTR_ITEM_PERIOD, update_power},
    {SOCBRD_MNTR_ITEM_PERIOD, update_temp},
    {SOCBRD_MNTR_ITEM_PERIOD, update_device_temp},
    {SOCBRD_MNTR_ITEM_PERIOD, update_fault_info},
    {SOCBRD_MNTR_ITEM_PERIOD, update_mcu_rtc_time}
};

LOCAL void soc_board_mntr_cb_free(gpointer data)
{
    if (data == NULL) {
        return;
    }
    SocBrdMntrCB *cb = (SocBrdMntrCB *)data;
    if (cb->pm_chip_info != NULL) {
        pm_clear(cb->pm_chip_info);
        cb->pm_chip_info = NULL;
    }
    g_mutex_clear(&cb->log_mutex);
    free(cb);
}


LOCAL gint32 soc_board_mntr_cb_new(OBJ_HANDLE obj_handle)
{
    SocBrdMntrCB *cb = (SocBrdMntrCB *)g_malloc0(sizeof(SocBrdMntrCB));
    if (cb == NULL) {
        debug_log(DLOG_ERROR, "%s: %s malloc fail", __FUNCTION__, dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    cb->handle = obj_handle;
    cb->exit_flag = 0;
    cb->state = SOCBRD_MNTR_INIT;
    cb->item_table = g_soc_board_mntr_table;
    cb->item_table_len = G_N_ELEMENTS(g_soc_board_mntr_table) > UINT8_MAX ?
        UINT8_MAX : G_N_ELEMENTS(g_soc_board_mntr_table);
    cb->retry_cnt = 0;
    cb->set_default_proc = soc_board_set_default_value;
    cb->update_firm_ver_proc = update_firmware_version;
    cb->log_task_id = 0;
    g_mutex_init(&cb->log_mutex);

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_ID, &cb->id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.Id fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_PWR_STATE, &cb->power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.PowerState fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }

    OBJ_HANDLE oob_handle = 0;
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_SOC_BOARD_OOB_CHAN, &oob_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.OobChannel fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }
    cb->pm_chip_info = oob_init_chip_info(oob_handle);
    if (cb->pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: init %s oob pm_chip_info fail", __FUNCTION__, dfl_get_object_name(obj_handle));
        goto EXIT;
    }

    ret = new_obj_priv_data(obj_handle, (gpointer)cb, soc_board_mntr_cb_free);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: new %s obj priv data fail, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }
    return RET_OK;
EXIT:
    soc_board_mntr_cb_free(cb);
    return RET_ERR;
}


LOCAL void soc_board_monitor_update(SocBrdMntrCB *cb)
{
    union ItemIndex {
        guint8 by;
        struct {
#ifdef BD_LITTLE_ENDIAN
            guint8 word : 3;
            guint8 bit : 5;
#else
            guint8 bit : 5;
            guint8 word : 3;
#endif
        } bi;
    };

    if (cb->power_state == 0) {
        cb->retry_cnt = 0;
        cb->state = SOCBRD_MNTR_INIT;
        return;
    }

    if (cb->pm_chip_info == NULL) {
        return;
    }
    for (guint8 i = 0; i < cb->item_table_len; i++) {
        const SocBrdMntrItem *item = &cb->item_table[i];
        if (item->func == NULL) {
            continue;
        }

        guint8 word = ((union ItemIndex)i).bi.word;
        guint32 bit = ((guint32)0x1 << ((union ItemIndex)i).bi.bit);
        guint8 is_updated = ((cb->item_updated[word] & bit) != 0);
        if (item->is_static && is_updated) {
            continue;
        }

        gint32 ret = item->func(cb->handle, cb->pm_chip_info);
        if (ret == RET_OK) {
            cb->item_updated[word] |= bit;
        }
        vos_task_delay(COMMON_DELAY_TIME_FOR_RW);
    }

    if (cb->update_firm_ver_proc == NULL) {
        return;
    }
    guint8 firm_updated = FALSE;
    gint32 ret = cb->update_firm_ver_proc(cb->handle, cb->pm_chip_info, &firm_updated);
    if (ret == RET_OK && firm_updated) {
        cb->retry_cnt = 0;
        cb->state = SOCBRD_MNTR_INIT;
    }
}


LOCAL void init_soc_board_elabel_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    OBJ_HANDLE fru_handle = 0;

    
    ret = dal_get_specific_object_position(obj_handle, CLASS_FRU, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get fru handle failed, soc name is %s", __FUNCTION__,
            dfl_get_object_name(obj_handle));
    } else {
        ret = dfl_call_class_method(fru_handle, METHOD_FRU_INIT_FRU_INFO_FROM_MCU, NULL, NULL, NULL);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: call InitFruInfoFromMcu failed, Fru name is %s", __FUNCTION__,
                dfl_get_object_name(fru_handle));
        }
    }
}

LOCAL gint32 update_pcb_ver(SocBrdMntrCB *cb)
{
#define PCB_MIN_VERSION         1
#define PCB_MAX_VERSION         26  // 版本阈暂定: A-Z
#define PCB_VER_TO_STR          64

    guint8 pcb_version = 0;
    gint32 ret = pm_get_pcb_version(cb->pm_chip_info, &pcb_version, sizeof(guint8));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get pcb version failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }
    debug_log(DLOG_DEBUG, "[%s] pcb_version = %u", __FUNCTION__, pcb_version);

    if (pcb_version < PCB_MIN_VERSION || pcb_version > PCB_MAX_VERSION) {
        debug_log(DLOG_ERROR, "%s: PCB ID %u is invalid.", __FUNCTION__, pcb_version);
        return RET_ERR;
    }

    /** pcbVer转换:
    * 1 -> .A
    * 2 -> .B
    * 3 -> .C
    * 以此类推
    * */
    gchar pcbVer[STR_PCB_VER_LEN] = {0};
    ret = snprintf_s(pcbVer, STR_PCB_VER_LEN, STR_PCB_VER_LEN - 1, ".%c", (gchar)(pcb_version + PCB_VER_TO_STR));
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    ret = dal_set_property_value_string(cb->handle, PROPERTY_SOC_BOARD_PCBVER, pcbVer, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : update soc_board PCB ID failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }

    debug_log(DLOG_DEBUG, "[%s] update pcb ver to socboard succeed.", __FUNCTION__);
    return ret;
}


LOCAL void soc_board_monitor_init(SocBrdMntrCB *cb)
{
    const guint8 RETRY_DIVISOR = 3;

    // 初始化失败，以6s为周期重试(轮询周期2s，每3次执行一次pm_init)；采用这种方式保证初始化失败不阻塞其它SoC主板的数据更新
    // 重试周期保持和PcieCard一致
    if ((cb->retry_cnt % RETRY_DIVISOR) != 0) {
        cb->retry_cnt++;
        return;
    }

    gint32 ret = pm_init(cb->pm_chip_info);
    if (ret != RET_OK) {
        cb->retry_cnt++;
        if (cb->retry_cnt < INIT_MAX_RETRY_TIMES * RETRY_DIVISOR) {
            debug_log(DLOG_INFO, "%s: %s pm_init fail, retry=%u", __FUNCTION__,
                dfl_get_object_name(cb->handle), cb->retry_cnt);
            return;
        }
        if (cb->set_default_proc != NULL) {
            cb->set_default_proc(cb->handle, SENSOR_INVALID_READING);
        }

        debug_log(DLOG_ERROR, "%s: %s pm_init fail, ret=%d", __FUNCTION__, dfl_get_object_name(cb->handle), ret);
        cb->state = SOCBRD_MNTR_FAIL;
        return;
    }

    vos_task_delay(COMMON_DELAY_TIME_FOR_RW);
    const guint16 DEFAULT_MCU_RESET_TIME = 10;
    cb->pm_chip_info->chip_reset_time = 0;
    ret = pm_get_mcu_reset_time(cb->pm_chip_info, (guint8 *)&cb->pm_chip_info->chip_reset_time, sizeof(guint16));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get mcu reset time fail, ret=%d", __FUNCTION__, ret);
        cb->pm_chip_info->chip_reset_time = DEFAULT_MCU_RESET_TIME;
    }

    ret = update_pcb_ver(cb);
    if (ret != RET_OK) {
        // 出错仅打印日志，不需要返回
        debug_log(DLOG_ERROR, "[%s] get pcb version failed, ret = %d", __FUNCTION__, ret);
    }

    if (cb->power_state == 1) {
        cb->state = SOCBRD_MNTR_UPDATE;
    } else {
        cb->state = SOCBRD_MNTR_WAIT;
        if (cb->set_default_proc != NULL) {
            cb->set_default_proc(cb->handle, SENSOR_NA_READING);
        }
    }
    init_soc_board_elabel_info(cb->handle);
}

LOCAL void soc_board_monitor_wait(SocBrdMntrCB *cb)
{
    if (cb->power_state != 0) {
        cb->retry_cnt = 0;
        cb->state = SOCBRD_MNTR_INIT;
        return;
    }
    if (cb->update_firm_ver_proc == NULL) {
        return;
    }
    guint8 firm_updated = FALSE;
    gint32 ret = cb->update_firm_ver_proc(cb->handle, cb->pm_chip_info, &firm_updated);
    if (ret == RET_OK && firm_updated) {
        cb->retry_cnt = 0;
        cb->state = SOCBRD_MNTR_INIT;
    }
}

LOCAL void soc_board_monitor_fail(SocBrdMntrCB *cb)
{
    if (cb->last_power_state == 1 && cb->power_state == 0) {
        cb->set_default_proc(cb->handle, SENSOR_NA_READING);
    }
    if (cb->last_power_state == 0 && cb->power_state == 1) {
        cb->retry_cnt = 0;
        cb->state = SOCBRD_MNTR_INIT;
    }
}


LOCAL gint32 run_soc_board_state_machine(OBJ_HANDLE obj_handle, gpointer user_data)
{
    struct SoCStateInfo {
        enum SocBrdMntrState state;
        void (*func)(SocBrdMntrCB *cb);
    };

    const struct SoCStateInfo soc_board_state_info[] = {
        {SOCBRD_MNTR_INIT, soc_board_monitor_init},
        {SOCBRD_MNTR_WAIT, soc_board_monitor_wait},
        {SOCBRD_MNTR_UPDATE, soc_board_monitor_update},
        {SOCBRD_MNTR_FAIL, soc_board_monitor_fail}
    };

    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported == 0) {
        return RET_OK;
    }

    SocBrdMntrCB *cb = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&cb);
    if (ret != RET_OK || cb == NULL) {
        debug_log(DLOG_DEBUG, "%s: %s get_obj_priv_data fail", __FUNCTION__, dfl_get_object_name(obj_handle));
        return RET_OK;
    }

    debug_log(DLOG_MASS, "%s: %s state=%d", __FUNCTION__, dfl_get_object_name(obj_handle), cb->state);

    if (cb->state >= SOCBRD_MNTR_EXIT) {
        debug_log(DLOG_ERROR, "%s: %s invalid state=%d", dfl_get_object_name(obj_handle), __FUNCTION__, cb->state);
    } else {
        soc_board_state_info[cb->state].func(cb);
    }

    guint8 power_state = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_PWR_STATE, &power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s get PowerState fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    } else {
        cb->last_power_state = cb->power_state;
        cb->power_state = power_state;
    }

    put_obj_priv_data(obj_handle, cb);
    return RET_OK;
}


LOCAL void collect_log_once_after_bmc_restart(void)
{
    static gulong start_time = 0;
    if (start_time == 0) {
        start_time = vos_tick_get();
        return;
    }
    if (!g_is_soc_board_log_allowed) {
        gulong cur_time = vos_tick_get();
        if (cur_time - start_time > (gulong)3 * 3600 * 1000) { // 延时3小时， 3h * 3600s/h * 1000ms/s
            g_is_soc_board_log_allowed = TRUE;
            (void)dfl_foreach_object(CLASS_SOC_BOARD, soc_board_start_log, NULL, NULL);
        }
    }
}


LOCAL void soc_board_monitor(gpointer data)
{
    (void)prctl(PR_SET_NAME, (gulong)"SoCBoardMonitor");

    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_SOC_BOARD, &obj_list);
    if (ret != DFL_OK && ret != ERRCODE_OBJECT_NOT_EXIST) {
        return;
    }

    // 初始化失败需要考虑
    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;
        guint8 mcu_supported = 0;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
        if (mcu_supported == 0) {
            continue;
        }
        ret = soc_board_mntr_cb_new(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: %s new obj priv data fail, ret=%d", __FUNCTION__,
                dfl_get_object_name(obj_handle), ret);
            continue;
        }
    }
    g_slist_free(obj_list);

    const guint32 PERIOD = 2000;
    while (TRUE) {
        (void)dfl_foreach_object(CLASS_SOC_BOARD, run_soc_board_state_machine, NULL, NULL);
        collect_log_once_after_bmc_restart();
        vos_task_delay(PERIOD);
    }
}


void soc_board_init(void)
{
    GSList *list = NULL;
    (void)dfl_get_object_list(CLASS_CONNECTOR_NAME, &list);
    for (GSList *node = list; node != NULL; node = node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;
        gchar conn_type[MAX_NAME_SIZE] = { 0 };
        gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_CONNECTOR_TYPE, conn_type, MAX_NAME_SIZE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get %s.Type fail, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
            continue;
        }
        if (g_strcmp0(conn_type, "SoCBoardConnector") != 0) {
            continue;
        }
        guint8 slotid = 0;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_CONNECTOR_SLOT, &slotid);
        if (slotid <= 0 || slotid >= U32_BITS) {
            debug_log(DLOG_ERROR, "%s: get %s.Slot=%u out of range", __FUNCTION__,
                dfl_get_object_name(obj_handle), slotid);
            continue;
        }
        guint8 present = 0;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_CONNECTOR_PRESENT, &present);
        if (present) {
            g_atomic_int_or(&g_soc_board_present, (1 << slotid));
        }
    }
}

void soc_board_start(void)
{
    gulong task_id = 0;
    gint32 ret = vos_task_create(&task_id, "SoCBoardMonitor", (TASK_ENTRY)soc_board_monitor, NULL, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create SoCBoardMonitor task fail, ret=%d", __FUNCTION__, ret);
        return;
    }
}

gint32 soc_board_add_object_callback(OBJ_HANDLE obj_handle)
{
    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported != 0) {
        gint32 ret = soc_board_mntr_cb_new(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: %s new obj priv data fail, ret=%d", __FUNCTION__,
                dfl_get_object_name(obj_handle), ret);
        }
    }
 
    guint8 slotid = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_ID, &slotid);
    if (slotid <= 0 || slotid >= U32_BITS) {
        debug_log(DLOG_ERROR, "%s: get %s.Id(%u) out of range", __FUNCTION__, dfl_get_object_name(obj_handle), slotid);
        return RET_OK;
    }
 
    if (g_soc_board_present & (1 << slotid)) {
        return RET_OK;
    }
    // 存在竞争条件，需要使用原子操作
    g_atomic_int_or(&g_soc_board_present, (1 << slotid));
    card_manage_log_operation_log("SoC board",
        obj_handle, PROPERTY_SOC_BOARD_ID, PROPERTY_SOC_BOARD_BOARDNAME, CARD_PLUG_IN);
    return RET_OK;
}

gint32 soc_board_del_object_callback(OBJ_HANDLE obj_handle)
{
    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported != 0) {
        delete_obj_priv_data(obj_handle);
    }
 
    guint8 slotid = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_ID, &slotid);
    if (slotid <= 0 || slotid >= U32_BITS) {
        debug_log(DLOG_ERROR, "%s: get %s.Id(%u) out of range", __FUNCTION__, dfl_get_object_name(obj_handle), slotid);
        return RET_OK;
    }
    // 存在竞争条件，需要使用原子操作
    g_atomic_int_and(&g_soc_board_present, (~(1 << slotid)));
    card_manage_log_operation_log("SoC board",
        obj_handle, PROPERTY_SOC_BOARD_ID, PROPERTY_SOC_BOARD_BOARDNAME, CARD_PLUG_OUT);
    return RET_OK;
}

LOCAL gint32 get_head_para_from_list(GSList *input_list, PROTOCOL_HEAD_PARA *head)
{
#define VALID_LEN 3
#define VALID_LEN_WITH_DATA 4
    guint8 list_length = g_slist_length(input_list);
    if (list_length != VALID_LEN && list_length != VALID_LEN_WITH_DATA) {
        debug_log(DLOG_ERROR, "%s: input parameter cnt error, list len=%d", __FUNCTION__, list_length);
        return RET_ERR;
    }

    guint8 index = 0;
    head->opcode = g_variant_get_uint16((GVariant *)(g_slist_nth_data(input_list, index++)));
    head->data_object_index = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, index++)));
    head->cmd_arg = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, index++)));

    debug_log(DLOG_DEBUG, "%s: opcode=%u, cmd_arg=0x%x, data_object_index=%u", __FUNCTION__, head->opcode,
        head->cmd_arg, head->data_object_index);

    // 请求数据不为空时，获取第4个参数
    const guint8 *input_data = NULL;
    gsize input_data_size = 0;
    if (list_length == VALID_LEN_WITH_DATA) {
        input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, index++)),
            &input_data_size, sizeof(guint8));
    }
    if (input_data != NULL && input_data_size > 0) {
        gint32 ret = memcpy_s(head->input_data, HEAD_PARA_MAX_LEN, input_data, input_data_size);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret=%d", __FUNCTION__, ret);
            return RET_ERR;
        }
        head->input_data_len = input_data_size;
    }
    return RET_OK;
}

gint32 soc_board_mcu_get_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (object_handle == 0 || input_list == NULL || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    PROTOCOL_HEAD_PARA head_para = { 0 };
    gint32 ret = get_head_para_from_list(input_list, &head_para);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: init head para fail, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported == 0) {
        debug_log(DLOG_DEBUG, "%s: %s does not support mcu", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    SocBrdMntrCB *cb = NULL;
    ret = get_obj_priv_data(object_handle, (gpointer *)&cb);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s binded data failed, ret=%d", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return ERRCODE_OBJECT_NOT_EXIST;
    }
    if (cb == NULL) {
        debug_log(DLOG_DEBUG, "%s: %s binded data==NULL", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    if ((cb->state != SOCBRD_MNTR_WAIT) && (cb->state != SOCBRD_MNTR_UPDATE)) {
        debug_log(DLOG_DEBUG, "%s: state is finish or update", __FUNCTION__);
        put_obj_priv_data(object_handle, cb);
        return ERRCODE_OBJECT_NOT_EXIST;
    }

    guint8 *buf = NULL;
    guint32 buf_len = 0;
    ret = pm_get_random_len_data(cb->pm_chip_info, &head_para, &buf, &buf_len);
    put_obj_priv_data(object_handle, cb);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get data failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    // 读取的数据封装
    *output_list =
        g_slist_append(*output_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, buf, buf_len, sizeof(guint8)));

    g_free(buf);
    return RET_OK;
}

gint32 soc_board_mcu_set_data(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (object_handle == 0 || input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    guint8 list_length = g_slist_length(input_list);
    if (list_length != 4) { // 4个入参， opcode, lun, arg, data
        debug_log(DLOG_ERROR, "%s: invalid list len=%u", __FUNCTION__, list_length);
        return RET_ERR;
    }

    guint8 index = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    head_para.opcode = g_variant_get_uint16((GVariant *)(g_slist_nth_data(input_list, index++)));
    head_para.data_object_index = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, index++)));
    head_para.cmd_arg = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, index++)));
    gsize buf_size = 0;
    const guint8 *buf = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, index++)),
        &buf_size, sizeof(guint8));

    if (buf == NULL || buf_size == 0) {
        debug_log(DLOG_INFO, "%s: data written to %s is null", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_OK;
    }

    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported == 0) {
        debug_log(DLOG_DEBUG, "%s: %s does not support mcu", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    SocBrdMntrCB *cb = NULL;
    gint32 ret = get_obj_priv_data(object_handle, (gpointer *)&cb);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s binded data failed, ret=%d", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        return ERRCODE_OBJECT_NOT_EXIST;
    }
    if (cb == NULL) {
        debug_log(DLOG_DEBUG, "%s: %s binded data==NULL", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    if ((cb->state != SOCBRD_MNTR_WAIT) && (cb->state != SOCBRD_MNTR_UPDATE)) {
        debug_log(DLOG_DEBUG, "%s: state is finish or update", __FUNCTION__);
        put_obj_priv_data(object_handle, cb);
        return ERRCODE_OBJECT_NOT_EXIST;
    }
    
    if (pm_send_data(cb->pm_chip_info, &head_para, buf, buf_size) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set data to mcu failed, ret=%d", __FUNCTION__, ret);
    }

    put_obj_priv_data(object_handle, cb);
    return ret;
}

gint32 soc_board_set_host_fault_info(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list)
{
    const guint32 INPUT_LEN = 2;
    if (object_handle == 0 || input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    guint32 list_length = g_slist_length(input_list);
    if (list_length != INPUT_LEN) {
        debug_log(DLOG_ERROR, "%s: input parameter cnt error, list len=%d", __FUNCTION__, list_length);
        return RET_ERR;
    }

    guint8 index = 0;
    guint32 fault_state = g_variant_get_uint32((GVariant *)(g_slist_nth_data(input_list, index++)));
    const gchar *fault_str = g_variant_get_string((GVariant *)(g_slist_nth_data(input_list, index++)), 0);
    (void)dal_set_property_value_uint32(object_handle, PROPERTY_SOC_BOARD_HOST_FAULT_STATE, fault_state, DF_NONE);
    (void)dal_set_property_value_string(object_handle, PROPERTY_SOC_BOARD_HOST_FAULT_CODE, fault_str, DF_NONE);
    return RET_OK;
}

LOCAL gint32 create_log_dir(const gchar *dir)
{
#define MCU_LOG_FILE_MOD 0750
    if (vos_check_dir(dir) == FALSE) {
        // 文件以及路径都不存在就创建路径
        gint32 ret = vos_mkdir_recursive(dir);
        if (ret < 0) {
            debug_log(DLOG_ERROR, "vos_mkdir_recursive failed, ret=%d.", ret);
            return ret;
        }
    }
    (void)chmod(dir, MCU_LOG_FILE_MOD);
    return RET_OK;
}


LOCAL gint32 get_soc_board_log_path(OBJ_HANDLE obj_handle, const gchar *base_dir, gchar *path, guint32 path_len)
{
    OBJ_HANDLE comp_handle = 0;
    gint32 ret = dal_get_specific_position_object_byte(obj_handle, CLASS_COMPONENT, PROPERTY_COMPONENT_DEVICE_TYPE,
        COMPONENT_TYPE_SOC_BOARD, &comp_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get component fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    gchar device_name[CLASSE_NAME_LEN] = { 0 };
    gchar soc_board_name[CLASSE_NAME_LEN] = { 0 };
    (void)dal_get_property_value_string(comp_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, sizeof(device_name));
    (void)dal_get_property_value_string(obj_handle, PROPERTY_SOC_BOARD_NAME, soc_board_name, sizeof(soc_board_name));
    (void)dal_clear_string_blank(device_name, sizeof(device_name));
    (void)dal_clear_string_blank(soc_board_name, sizeof(soc_board_name));

    ret = sprintf_s(path, path_len, "%s%s_%s", base_dir, device_name, soc_board_name);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void soc_board_collect_log_task(gpointer user_data)
{
    // 预备
    OBJ_HANDLE obj_handle = (OBJ_HANDLE)user_data;
    SocBrdMntrCB *cb = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&cb);
    if (ret != RET_OK || cb == NULL) {
        debug_log(DLOG_ERROR, "%s: get binded data failed, ret=%d", __FUNCTION__, ret);
        return;
    }

    // 线程名称设置
    gchar task_name[PROP_VAL_LENGTH] = { 0 };
    (void)sprintf_s(task_name, sizeof(task_name), "CollectSoCLog%u", cb->id);
    (void)prctl(PR_SET_NAME, (gulong)task_name);

    // 避免收集日志时刚好处于初始化状态
    guint32 WAIT_INIT_PERIOD = 6000; // 初始化重试周期 6s
    guint8 wait_cnt = 0;
    while (cb->state == SOCBRD_MNTR_INIT && wait_cnt < INIT_MAX_RETRY_TIMES) {
        wait_cnt++;
        vos_task_delay(WAIT_INIT_PERIOD);
    }
    // 初始化失败直接退出
    if (cb->state != SOCBRD_MNTR_WAIT && cb->state != SOCBRD_MNTR_UPDATE) {
        debug_log(DLOG_ERROR, "%s: task %s exit due to state=%d", __FUNCTION__, task_name, cb->state);
        goto EXIT;
    }

    // 生成日志存放路径
    const gchar *SOCBRD_LOG_BASE_DIR = "/data/var/log/socboard/";
    gchar log_dir_path[MAX_FILEPATH_LENGTH] = { 0 };
    ret = get_soc_board_log_path(obj_handle, SOCBRD_LOG_BASE_DIR, log_dir_path, sizeof(log_dir_path));
    if (ret != RET_OK) {
        goto EXIT;
    }
    if (create_log_dir(SOCBRD_LOG_BASE_DIR) != RET_OK || create_log_dir(log_dir_path) != RET_OK) {
        goto EXIT;
    }

    PM_CHIP_INFO *pm_chip_info = cb->pm_chip_info;
    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: pm_chip_info=NULL", __FUNCTION__);
        goto EXIT;
    }

    // 逐个收集日志
    debug_log(DLOG_ERROR, "%s: collect log start", dfl_get_object_name(obj_handle));
    const guint16 log_list[] = {REGISTER_KEY_ERROR_LOG, REGISTER_KEY_OPERATE_LOG, REGISTER_KEY_MAINTAINCE_LOG};
    const gchar *log_name_prefix[] = {"error_log", "operate_log", "maintaince_log"};
    for (guint32 i = 0; i < G_N_ELEMENTS(log_list) && log_list[i] != 0; i++) {
        pcie_card_update_log(pm_chip_info, log_dir_path, log_list[i], sizeof(log_dir_path), log_name_prefix[i]);
    }
EXIT:
    g_mutex_lock(&cb->log_mutex);
    cb->log_task_id = 0;
    g_mutex_unlock(&cb->log_mutex);
    debug_log(DLOG_ERROR, "%s: collect log finish", dfl_get_object_name(obj_handle));
    put_obj_priv_data(obj_handle, cb);
}


LOCAL void soc_board_do_start_log(OBJ_HANDLE obj_handle)
{
    SocBrdMntrCB *cb = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&cb);
    if (ret != RET_OK || cb == NULL) {
        debug_log(DLOG_ERROR, "%s: get %s binded data failed, ret=%d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return;
    }
    g_mutex_lock(&cb->log_mutex);
    if (cb->log_task_id == 0) {
        ret = vos_task_create(&cb->log_task_id, "collect_log",
            (TASK_ENTRY)soc_board_collect_log_task, (gpointer)obj_handle, 20); // 20: 低优先级任务
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: create soc log task fail, ret=%d", __FUNCTION__, ret);
        }
    }
    g_mutex_unlock(&cb->log_mutex);
    put_obj_priv_data(obj_handle, cb);
}


gint32 soc_board_start_log(OBJ_HANDLE obj_handle, gpointer user_data)
{
    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported == 0) {
        debug_log(DLOG_INFO, "%s does not support mcu log\n", dfl_get_object_name(obj_handle));
        return RET_OK;
    }
    if (g_is_soc_board_log_allowed) {
        soc_board_do_start_log(obj_handle);
    }
    return RET_OK;
}


gint32 collect_soc_board_log_manual(guint32 position, guint8 slot)
{
    OBJ_HANDLE obj_handle = 0;
    GSList *soc_board_list = NULL;
    (void)dfl_get_object_list(CLASS_SOC_BOARD, &soc_board_list);
    for (GSList *node = soc_board_list; node != NULL; node = node->next) {
        OBJ_HANDLE soc_board_handle = (OBJ_HANDLE)node->data;
        if (dfl_get_position(soc_board_handle) != position) {
            continue;
        }
        guint8 soc_board_slot = 0;
        (void)dal_get_property_value_byte(soc_board_handle, PROPERTY_SOC_BOARD_ID, &soc_board_slot);
        if (soc_board_slot == slot) {
            obj_handle = soc_board_handle;
            break;
        }
    }
    g_slist_free(soc_board_list);

    if (obj_handle == 0) {
        return RET_ERR;
    }

    guint8 mcu_supported = 0;
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_MCU_SUPPORT, &mcu_supported);
    if (mcu_supported == 0) {
        debug_printf("%s does not support mcu log\n", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    debug_log(DLOG_ERROR, "%s: Start collect %s mcu log manually.", __FUNCTION__, dfl_get_object_name(obj_handle));
    // 手动触发，无视BMC重启3小时的限制
    soc_board_do_start_log(obj_handle);
    return RET_OK;
}


PM_CHIP_INFO *oob_init_chip_info(OBJ_HANDLE oob_handle)
{
    PM_CHIP_INFO *info = (PM_CHIP_INFO *)g_malloc0(sizeof(PM_CHIP_INFO));
    if (info == NULL) {
        debug_log(DLOG_ERROR, "%s: malloc fail", __FUNCTION__);
        return NULL;
    }
    (void)memcpy_s(info, sizeof(PM_CHIP_INFO), &g_default_chip_info, sizeof(PM_CHIP_INFO));
 
    g_mutex_init(&info->rw_mutex);
    g_mutex_init(&info->key_mutex);
 
    gint32 ret = dal_get_property_value_byte(oob_handle, PROPERTY_OOB_CHAN_PROTOCOL, &info->inf_protocol);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel.Protocol fail, ret=%d", __FUNCTION__, ret);
        goto EXIT;
    }
 
    OBJ_HANDLE chip_handle = 0;
    ret = dfl_get_referenced_object(oob_handle, PROPERTY_OOB_CHAN_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel.RefChip fail, ret=%d", __FUNCTION__, ret);
        goto EXIT;
    }
    const gchar *chip_name = dfl_get_object_name(chip_handle);
    if (chip_name == NULL) {
        debug_log(DLOG_ERROR, "%s: get RefChip name fail", __FUNCTION__);
        goto EXIT;
    }
    (void)strncpy_s(info->chip_name, sizeof(info->chip_name), chip_name, sizeof(info->chip_name) - 1);
 
    ret = dal_get_property_value_uint32(oob_handle, PROPERTY_OOB_CHAN_MAXFRAMELEN, &info->framelen);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel.MaxFrameLen fail, ret=%d", __FUNCTION__, ret);
        goto EXIT;
    }
    return info;
EXIT:
    pm_clear(info);
    return NULL;
}
 

LOCAL void oob_discover_task(gpointer user_data)
{
    const guint8 UNKNOWN_BOARDID = 0xff;
 
    (void)prctl(PR_SET_NAME, (gulong)"OobDiscover");
    OBJ_HANDLE obj_handle = (OBJ_HANDLE)user_data;
    PM_CHIP_INFO *pm_chip_info = oob_init_chip_info(obj_handle);
    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: oob_init_chip_info fail", __FUNCTION__);
        return;
    }
 
    for (guint8 retry = 0; retry < 5; retry++) { // 重试5次
        gint32 ret = pm_init(pm_chip_info);
        if (ret == RET_OK) {
            break;
        }
        vos_task_delay(3000); // 重试间隔 3000ms = 3s
    }
    guint16 boardid = UNKNOWN_BOARDID;
    for (guint8 retry = 0; retry < 5; retry++) { // 重试5次
        gint32 ret = pm_get_board_id(pm_chip_info, (guint8 *)&boardid, sizeof(guint16));
        if (ret == RET_OK) {
            break;
        }
        boardid = UNKNOWN_BOARDID;
        vos_task_delay(3000); // 重试间隔 3000ms = 3s
    }
    pm_clear(pm_chip_info);
 
    OBJ_HANDLE conn_handle = 0;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_OOB_CHAN_REFCONN, &conn_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get ref connector fail, ret=%d", __FUNCTION__, ret);
        return;
    }
    gchar bom[CONNECTOR_BOM_ID_MAX_LEN] = { 0 };
    (void)dal_get_property_value_string(conn_handle, PROPERTY_CONNECTOR_BOM, bom, sizeof(bom));
 
    (void)dfl_notify_presence_v2(conn_handle, boardid, 0x00, 1);
    debug_log(DLOG_ERROR, "%s: Load %s_%x.xml", __FUNCTION__, bom, boardid);
}
 

LOCAL gint32 create_discover_task(OBJ_HANDLE obj_handle, gpointer user_data)
{
    OBJ_HANDLE conn_handle = 0;
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_OOB_CHAN_REFCONN, &conn_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: %s do not need discover, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return RET_OK;
    }
 
    gulong task_id = 0;
    ret = vos_task_create(&task_id, "OobDiscover",
        (TASK_ENTRY)oob_discover_task, (gpointer)obj_handle, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create OobDiscover task fail, ret=%d", __FUNCTION__, ret);
    }
    return RET_OK;
}
 
void oob_chan_start(void)
{
    dfl_foreach_object(CLASS_OOB_CHAN, create_discover_task, NULL, NULL);
}
 
gint32 oob_chan_add_object_callback(OBJ_HANDLE obj_handle)
{
    return create_discover_task(obj_handle, NULL);
}
 
gint32 oob_chan_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}

LOCAL gint32 boot_certfile_sendcmd_typeandlen(OBJ_HANDLE chip_handle, STD_SMBUS_REQ *write_req_cmd,
    guint32 cert_size, guint8 ssl_type)
{
    guint8 *data = NULL;
    gint32 data_size;
    gint32 ret;
    write_req_cmd->cmd_code = SMBUS_REQ_COMMAND_CODE;
    write_req_cmd->arg = 0x03; // 更新证书
    write_req_cmd->opcode = REGISTER_KEY_SEND_BOOT_CERT;
    write_req_cmd->lun &= ~SMBUS_LAST_FRAME_MASK;
    write_req_cmd->data[0] = ssl_type;
    write_req_cmd->data_length = SSL_TYPE_LEN;
    write_req_cmd->data[1] = (cert_size & 0x000000ff);
    write_req_cmd->data[2] = (cert_size & 0x0000ff00) >> 8;  
    write_req_cmd->data[3] = (cert_size & 0x00ff0000) >> 16; 
    write_req_cmd->data[4] = (cert_size & 0xff000000) >> 24; 
    data = (guint8 *)&(write_req_cmd->lun);
    data_size = SSL_TYPE_LEN + MCU_SMBUS_HEADER_SIZE;
 
    
    for (gint8 i = 0; i < 3; i++) {
        ret = dal_pmbus_chip_blkwrite(chip_handle, SMBUS_REQ_COMMAND_CODE, data, data_size);
        if (ret != RET_OK) {
            vos_task_delay(10); 
            continue;
        } else {
            break;
        }
    }
    return ret;
}


LOCAL gint32 soc_board_sendcmd_secureboot(OBJ_HANDLE chip_handle, STD_SMBUS_REQ *write_req_cmd,
    guint8 secureboot)
{
    guint8 *data = NULL;
    gint32 data_size;
    gint32 ret;
    write_req_cmd->cmd_code = SMBUS_REQ_COMMAND_CODE;
    write_req_cmd->arg = 0x04; // 更新安全启动开关状态
    write_req_cmd->opcode = REGISTER_KEY_SEND_BOOT_CERT;
    write_req_cmd->lun &= ~SMBUS_LAST_FRAME_MASK;
    write_req_cmd->data[0] = secureboot;
    write_req_cmd->data_length = SECUREBOOT_LEN;
    data = (guint8 *)&(write_req_cmd->lun);
    data_size = SECUREBOOT_LEN + MCU_SMBUS_HEADER_SIZE;
 
    for (guint8 i = 0; i < MCU_RETRY_TIMES; i++) {
        ret = dal_pmbus_chip_blkwrite(chip_handle, SMBUS_REQ_COMMAND_CODE, data, data_size);
        if (ret != RET_OK) {
            vos_task_delay(10); 
            continue;
        } else {
            break;
        }
    }
    return ret;
}
 

LOCAL gint32 boot_certfile_sendcmd_content(const gchar *cert_string, STD_SMBUS_REQ *write_req_cmd, guint32 cert_size,
    OBJ_HANDLE chip_handle)
{
    guint8 *data = NULL;
    guint32 offset = 0;
    guint32 bytes_read;
    guint32 data_size;
    gsize i;
    gint32 ret;
    errno_t safe_fun_ret;
    while (offset < cert_size) {
        write_req_cmd->offset = offset + SSL_TYPE_LEN;    
        if (offset + MCU_SMBUS_PAYLOAD_MAXSIZE <= cert_size) {
            bytes_read = MCU_SMBUS_PAYLOAD_MAXSIZE;       
        } else {
            bytes_read = cert_size - offset;
        }
        debug_log(DLOG_DEBUG, "dal_pmbus_chip_blkwrite with cert_size:%d offset:%d bytes_read:%d",
            cert_size, offset, bytes_read);

        safe_fun_ret = memcpy_s(write_req_cmd->data, DATA_MAX_NUM, (cert_string + offset), bytes_read);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
        write_req_cmd->data_length = bytes_read;
        offset += bytes_read;
        
        
        if (offset < cert_size) {
            write_req_cmd->lun &= ~SMBUS_LAST_FRAME_MASK;
        } else {
            write_req_cmd->lun |= SMBUS_LAST_FRAME_MASK;  
        }

        data = (guint8 *)&(write_req_cmd->lun);
        data_size = (gint32)(bytes_read + MCU_SMBUS_HEADER_SIZE);
 
        
        for (i = 0; i < MCU_RETRY_TIMES; i++) {
            ret = dal_pmbus_chip_blkwrite(chip_handle, SMBUS_REQ_COMMAND_CODE, data, data_size);
            if (ret != RET_OK) {
                vos_task_delay(10); 
                continue;
            } else {
                break;
            }
        }
        if (i >= MCU_RETRY_TIMES) {
            debug_log(DLOG_ERROR, "%s: blkwrite fail ret = %d!", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}


gint32 upload_boot_cert_string_to_mcu(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list)
{
    STD_SMBUS_REQ *write_req_cmd = NULL;
    guint32 cert_size;
    gint32 ret;
 
    if (input_list == NULL || g_slist_length(input_list) != 2) { // 方法参数个数为2
        debug_log(DLOG_ERROR, "%s param is invalid!", __FUNCTION__);
        return RET_ERR;
    }

    const gchar *cert_string = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    guint8 ssl_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));

    cert_size = (guint32)strlen(cert_string);
    write_req_cmd = (STD_SMBUS_REQ *)g_malloc0(sizeof(STD_SMBUS_REQ));

    OBJ_HANDLE oob_handle = 0;
    OBJ_HANDLE chip_handle = 0;
    ret = dfl_get_referenced_object(object_handle, PROPERTY_SOC_BOARD_OOB_CHAN, &oob_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel fail, ret=%d", __FUNCTION__, ret);
        goto EXIT;
    }
    ret = dfl_get_referenced_object(oob_handle, PROPERTY_OOB_CHAN_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel.RefChip fail, ret=%d", __FUNCTION__, ret);
        goto EXIT;
    }

    ret = boot_certfile_sendcmd_typeandlen(chip_handle, write_req_cmd, cert_size, ssl_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "fail to dal_pmbus_chip_blkwrite:ret(%d)", ret);
        goto EXIT;
    }
    ret = boot_certfile_sendcmd_content(cert_string, write_req_cmd, cert_size, chip_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "fail to dal_pmbus_chip_blkwrite:ret(%d)", ret);
    }

EXIT:
    (void)g_free(write_req_cmd);
    write_req_cmd = NULL;
    return ret;
}
 
gint32 get_boot_cert_status_from_mcu(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list)
{
    gint32 ret = RET_OK;
    guint8 ssl_state = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    SocBrdMntrCB *cb = NULL;
 
    ret = get_obj_priv_data(object_handle, (gpointer *)&cb);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get object %s priv data failed. The error code is %d", __FUNCTION__,
            dfl_get_object_name(object_handle), ret);
        goto EXIT;
    }
    head_para.opcode = REGISTER_KEY_GET_SSL_STATUS;
    head_para.data_object_index = 0;
    head_para.cmd_arg = 0x07;  // 查询证书更新结果
 
    ret = pm_get_fixed_len_data_by_std_smbus(cb->pm_chip_info, (guint8 *)&head_para, (guint8 *)&ssl_state,
        KEY_SSL_STATUS_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get ssl state failed! ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }
    debug_log(DLOG_DEBUG, "%s: get ssl state success state = %d", __FUNCTION__, ssl_state);
    *output_list = g_slist_append(*output_list, g_variant_new_byte(ssl_state));

EXIT:
    put_obj_priv_data(object_handle, cb);
    return ret;
}

gint32 soc_board_set_secureboot_to_mcu(OBJ_HANDLE object_handle, GSList *caller_info,
    GSList *input_list, GSList **output_list)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE chip_handle = 0;
    STD_SMBUS_REQ *write_req_cmd = NULL;

    if (input_list == NULL || g_slist_length(input_list) < 1) { // 方法参数个数为1
        debug_log(DLOG_ERROR, "%s param is invalid!", __FUNCTION__);
        return RET_ERR;
    }
    ret = dfl_get_referenced_object(object_handle, PROPERTY_SOC_BOARD_OOB_CHAN, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_OOB_CHAN_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get OobChannel.RefChip fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    guint8 secureboot = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    debug_log(DLOG_DEBUG, "%s: secureboot = %u.", __FUNCTION__, secureboot);

    write_req_cmd = (STD_SMBUS_REQ *)g_malloc0(sizeof(STD_SMBUS_REQ));
    if (write_req_cmd == NULL) {
        debug_log(DLOG_ERROR, "%s g_malloc0 failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = soc_board_sendcmd_secureboot(chip_handle, write_req_cmd, secureboot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set secureboot failed! ret = %d", __FUNCTION__, ret);
    }

    (void)g_free(write_req_cmd);
    write_req_cmd = NULL;
    return ret;
}

gint32 soc_board_mcu_get_secureboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    SocBrdMntrCB *cb = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&cb);
    if (ret != RET_OK || cb == NULL) {
        debug_log(DLOG_DEBUG, "%s: get_obj_priv_data fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    
    guint8 *buf = NULL;
    guint32 buf_len = 0;
    for (int tryTimes = 0; tryTimes < MCU_RETRY_TIMES; tryTimes++) {
        ret = pm_get_bios_secure_boot(cb->pm_chip_info, &buf, &buf_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get secureboot rspdata failed! ret = %d", __FUNCTION__, ret);
            // 当前更新节点非常慢,需延时100毫秒才能读到有效数据,
            vos_task_delay(100);
            continue;
        }
        break;
    }
    put_obj_priv_data(obj_handle, (gpointer *)&cb);

    // 第二个字节为secureboot的实际状态
    guint8 secureBoot = buf[RSP_INDEX_ERROR_CODE + 1];

    debug_log(DLOG_DEBUG, "%s: get secureboot success, secureboot = %u", __FUNCTION__, secureBoot);
    if (secureBoot == 0xff) { // 如果获取到的数值为0xff,证明MCU之前从来都没有发送信息给BIOS，默认为关闭
        secureBoot = 0;
    }
    *output_list = g_slist_append(*output_list, g_variant_new_byte(secureBoot));
    return RET_OK;
}

/**
* Description: 写双口ram控制寄存器
* History:     2023-02-14 s30038273 新生成函数
*/
LOCAL gint32 write_ram_dual_port(const guint8 *data_buf, guint8 buf_len, guint8 offset)
{
#define MAX_RETRY_TIME 10
#define WAIT_SOCBOARD_LOAD 1000
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 i = 0;
    guint8 retry = 1;

    // bmc重启，socborad对象加载延时，有可能获取不到对象句柄，需要等待
    while (retry <= MAX_RETRY_TIME) {
        ret = dal_get_object_handle_nth(CLASS_SOC_BOARD, 0, &obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: get SoCBoard handle failed, retry = %u.", __FUNCTION__, retry++);
            vos_task_delay(WAIT_SOCBOARD_LOAD);
            continue;
        }
        break;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get SoCBoard handle failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (i = 0; i < buf_len; i++) {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_SHARE_RAM_ADDR, offset++, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write SharedRamAddr failed, ret = %d.", __FUNCTION__, ret);
            return RET_ERR;
        }
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_SOC_BOARD_SHARE_RAM_DATA, data_buf[i], DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: write SharedRamData failed, ret = %d.", __FUNCTION__, ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}

/**
* Description: 监听BMC属性DeviceGuid，写双口ram寄存器
* History:     2023-02-14 s30038273 新生成函数
*/
gint32 write_ram_uuid_atlas800d_g1(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 ret = 0;
    guint8 write_data[BMC_GUID_LEN + 5] = {0x55, 0xAA, 0x10};
    guint8 write_len = 3;
    guint16 calc_crc = 0;
    guint8 i = 0;
    guint8 offset = 0;
    gsize guid_len = 0;
    const guint8 *uuid = NULL;
    
    debug_log(DLOG_DEBUG, "%s: begin write uuid", __FUNCTION__);
    if ((dal_match_board_id(PRODUCT_ID_ATLAS, BOARD_ID_ATLAS800D_G1)) != TRUE) {
        debug_log(DLOG_DEBUG, "%s: not supported write uuid to ram.", __FUNCTION__);
        return RET_OK;
    }
    uuid = (const guint8 *)g_variant_get_fixed_array(property_value, &guid_len, sizeof(guint8));
    if (uuid == NULL || guid_len != BMC_GUID_LEN) {
        debug_log(DLOG_ERROR, "%s: get guid_len:%" G_GSIZE_FORMAT, __FUNCTION__, guid_len);
        return RET_ERR;
    }

    // uuid写入顺序与要反转
    for (i = 0; i < guid_len; i++) {
        write_data[write_len + i] = uuid[guid_len - i - 1];
    }

    // 计算uuid的crc16值
    ret = dal_cal_crc16(0x00, &write_data[write_len], BMC_GUID_LEN, &calc_crc);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cal uuid crc16 failed.", __FUNCTION__);
        return RET_ERR;
    }
    write_len += BMC_GUID_LEN;
    write_data[write_len++] = calc_crc & 0xff;
    write_data[write_len++] = (calc_crc >> 8) & 0xff;  

    // 将uuid写入到寄存器
    ret = write_ram_dual_port(write_data, write_len, offset);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write ram uuid dual port failed.", __FUNCTION__);
    }

    return ret;
}

/**
* Description: 监听Fru0Elabel对象SysSerialNum属性，写双口ram寄存器
* Notes:       NA
* History:     2023-02-14 s30038273 新生成函数
*/
gint32 write_ram_sn_atlas800d_g1(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
#define MAX_SN_LENGTH 48
    gint32 ret = 0;
    guint8 write_data[MAX_SN_LENGTH + 5] = {0x55, 0xAA};
    guint8 write_len = 2;
    // sn写入的偏移地址
    guint8 offset = 32;
    guint16 calc_crc = 0;
    guint8 i = 0;
    const gchar *sn = NULL;
    gsize sn_len = 0;

    debug_log(DLOG_DEBUG, "%s: begin write sn", __FUNCTION__);
    if ((dal_match_board_id(PRODUCT_ID_ATLAS, BOARD_ID_ATLAS800D_G1)) != TRUE) {
        debug_log(DLOG_DEBUG, "not supported write sn to ram.");
        return RET_OK;
    }
    if (strcmp(dfl_get_object_name(object_handle), OBJ_FRU0_ELABEL) != 0) {
        debug_log(DLOG_DEBUG, "Not fru0, No need to write.");
        return RET_OK;
    }
    sn = (const gchar*)g_variant_get_string(property_value, &sn_len);
    if (sn == NULL || sn_len == 0 || sn_len > MAX_SN_LENGTH) {
        debug_log(DLOG_ERROR, "%s: sn Invalid, sn_len%" G_GSIZE_FORMAT, __FUNCTION__, sn_len);
        return RET_ERR;
    }
     // sn转ascii值
    write_data[write_len++] = (guint8)sn_len;
    for (i = 0; i < (guint8)sn_len; i++) {
        write_data[write_len + i] = sn[i];
    }

    // 计算sn的crc16值
    if (dal_cal_crc16(0x00, &write_data[write_len], sn_len, &calc_crc) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: cal sn crc16 failed.", __FUNCTION__);
        return RET_ERR;
    }
    write_len += (guint8)sn_len;
    write_data[write_len++] = calc_crc & 0xff;
    write_data[write_len++] = (calc_crc >> 8) & 0xff;  

    // 将sn写入到寄存器
    ret = write_ram_dual_port(write_data, write_len, offset);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: write ram sn dual port failed.", __FUNCTION__);
    }

    return ret;
}