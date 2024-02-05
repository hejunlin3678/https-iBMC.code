



#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/dalib/dal_obj_priv_data.h"
#include "get_version.h"
#include "pcie_card.h"
#include "pcie_card_comm.h"
#include "soc_board.h"
#include "io_board.h"

const guint8 MAX_RETRY_TIMES = 30;
#define MAX_DEVICE_NAME_LEN 32

LOCAL gdouble ioboard_optical_module_temp_handle(guint16 temp)
{
    gint32 ret = RET_ERR;
    guint16 temperature = 0;

    switch (temp) {
        case OPTICAL_MODULE_TEMP_ABSENT:   // 0x7ffe 光模块不在位
        case OPTICAL_MODULE_TEMP_PAD:   // 0x7ffd 温度为无效数据
            temperature = 0;
            break;
        case OPTICAL_MODULE_TEMP_READ_FAIL:  // 0x7fff 温度读取失败
        case SENSOR_INVALID_READING:   // 0x8000 光模块温度获取报文失败
            temperature = SENSOR_INVALID_READING;
            break;
        default:
            ret = RET_OK;
            temperature = temp & 0xff;
    }
    
    return (ret == RET_OK) ? (gdouble)(gint8)temperature : (gdouble)temperature;
}

LOCAL void set_optical_temp(OBJ_HANDLE obj_handle, guint8 index, guint16 temp)
{
    OBJ_HANDLE optical_module_obj_handle = 0;
    gint32 ret = pcie_card_get_optical_module_handle_by_channel(obj_handle, index + 1, &optical_module_obj_handle);
    // 光模块不是必定存在的，该代码块会频繁调用，优化该场景日志打印
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        return;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "get optical module handle by channel fail.");
        return;
    }

    gdouble set_val = ioboard_optical_module_temp_handle(temp);
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_double(set_val));
    ret = dfl_call_class_method(optical_module_obj_handle, METHOD_OPT_MDL_SET_OPTICAL_MODULE_TEMP, NULL,
        input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;

    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: %s set channel(%u) optical module temp fail, ret %d.", __FUNCTION__,
                  dfl_get_object_name(optical_module_obj_handle), index, ret);
    }
}


#define OPTICAL_MODULE_TEMP_LENGTH 2
#define OPTICAL_MODULE_MAX_NUMBER 16
LOCAL gint32 update_optical_module_temp(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint16 *optical_module_temp_raw = NULL;
    guint32 optical_module_temp_total_len = 0;

    gint32 ret = pm_get_optical_module_temp(pm_chip_info, (guint8 **)&optical_module_temp_raw,
                                            &optical_module_temp_total_len);
    // 如果网卡固件版本不支持获取光模块温度，不进行告警
    if (ret == PM_ERROR_CODE_NIC_OP_TEMP_UNSUPPORTED) {
        debug_log(DLOG_INFO, "Get optical module temp unsupported, ret = 0x%4x", ret);
        return ret;
    }

    gboolean success = (ret != RET_OK || optical_module_temp_raw == NULL) ? FALSE : TRUE;
    guint8 max_index = MIN(optical_module_temp_total_len / OPTICAL_MODULE_TEMP_LENGTH, OPTICAL_MODULE_MAX_NUMBER);
    for (guint8 index = 0; index < max_index; index++) {
        set_optical_temp(obj_handle, index, success ? optical_module_temp_raw[index] : SENSOR_INVALID_READING);
    }

    g_free(optical_module_temp_raw);
    return RET_OK;
}


LOCAL gint32 update_netcard_link_status(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 *netcard_link_status = NULL;
    guint32 netcard_link_status_len = 0;
    gint32 ret = pm_get_netcard_link_status(pm_chip_info, (guint8 **)&netcard_link_status, &netcard_link_status_len);
    if ((ret != RET_OK) || (netcard_link_status == NULL)) {
        debug_log(DLOG_INFO, "%s: %s get netcard link status failed, ret = %d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        g_free(netcard_link_status);
        return RET_ERR;
    }

    // 获取link状态成功, 校验 netcard_link_status_len 的合法性
    guint8 port_num = 0;
    get_pcie_netcard_port_num(obj_handle, &port_num);
    netcard_link_status_len = (netcard_link_status_len <= port_num) ? netcard_link_status_len : port_num;

    for (guint32 index = 0; index < netcard_link_status_len; index++) {
        OBJ_HANDLE business_port_handle = 0;
        ret = pcie_card_get_business_port_handle_by_channel(obj_handle, index, &business_port_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: get link status handle by channel fail, ret = %d.", __FUNCTION__, ret);
            continue;
        }

        guint8 old_link_status = 0;
        ret = dal_get_property_value_byte(business_port_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &old_link_status);
        if ((ret == RET_OK) && (old_link_status == netcard_link_status[index])) {
            continue;
        }

        debug_log(DLOG_INFO, "%s: %s.LinkStatus changed from %u into %u.",  __FUNCTION__,
                  dfl_get_object_name(business_port_handle), old_link_status, netcard_link_status[index]);

        GSList *input_list = NULL;
        input_list = g_slist_append(input_list, g_variant_new_byte(netcard_link_status[index]));
        ret = dfl_call_class_method(business_port_handle, BUSY_ETH_METHOD_SET_LINK_STATUS, NULL, input_list, NULL);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: %s set channel(%u) link status fail, ret %d.", __FUNCTION__,
                      dfl_get_object_name(business_port_handle), index, ret);
        }

        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
    }

    g_free(netcard_link_status);
    return RET_OK;
}



LOCAL void print_mac(gchar *mac_str, guint8 mac_str_len, guint8 *mac_data)
{
    if (mac_str_len < NETCARD_MAC_ADDRESS_LENGTH) {
        return;
    }
    const guint8 MAC_SEGMENT_LEN = 3;  // 第三个开始是MAC地址
    // 第三个开始是MAC地址，小字节序；有6组数据，加5个:字符，加结束符18个；  00:00:00:00:00:00
    for (gint32 mac_i = 0; mac_i < MAC_ADDRESS_LENGTH; ++mac_i) {
        gint32 ret = snprintf_truncated_s((mac_str + MAC_SEGMENT_LEN * mac_i), (mac_str_len - MAC_SEGMENT_LEN * mac_i),
                                          "%02x:", mac_data[MAC_SEGMENT_LEN + mac_i]);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_truncated_s fail, ret = %d.", __FUNCTION__, ret);
        }
    }

    mac_str[NETCARD_MAC_ADDRESS_LENGTH - 1] = '\0';
    debug_log(DLOG_INFO, "%s, mac_addr(%s)", __FUNCTION__, mac_str);
}

LOCAL void set_netcard_mac_address(OBJ_HANDLE obj_handle, guint8 *netcard_mac_adress, guint8 index)
{
    guint8 physical_port = netcard_mac_adress[NETCARD_MAC_ADDRESS_RESPONSE_LENGTH * index];
    gchar mac_addr[NETCARD_MAC_ADDRESS_LENGTH] = {0};
    print_mac(mac_addr, sizeof(mac_addr), &netcard_mac_adress[NETCARD_MAC_ADDRESS_RESPONSE_LENGTH * index]);
    if (g_ascii_strcasecmp(mac_addr, "ff:ff:ff:ff:ff:ff") == 0) {
        return;
    }
    OBJ_HANDLE business_port_handle = 0;
    gint32 ret = pcie_card_get_business_port_handle_by_channel(obj_handle, physical_port, &business_port_handle);
    if ((ret != RET_OK) || (business_port_handle == 0)) {
        debug_log(DLOG_DEBUG, "%s: %s get mac address handle by channel fail, ret = %d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        return;
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(mac_addr));
    ret = dfl_call_class_method(business_port_handle, BUSY_ETH_METHOD_SET_MAC, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: %s set port(%u) MacAddress fail, ret %d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), physical_port, ret);
    }
}


LOCAL gint32 update_netcard_mac_address(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    // 调用I2C接口去固件获取pf，MAC地址信息；
    guint8 *netcard_mac_adress = NULL;
    guint32 netcard_mac_address_total_len = 0;
    gint32 ret = pm_get_netcard_mac_address(pm_chip_info, &netcard_mac_adress, &netcard_mac_address_total_len);
    if ((ret != RET_OK) || (netcard_mac_adress == NULL)) {
        debug_log(DLOG_DEBUG, "%s: get %s.MacAddress failed, ret = %d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        g_free(netcard_mac_adress);
        return RET_ERR;
    }

    // 校验通过SMBus获取的MAC地址长度参数，避免出现异常情况下因无符号数翻转导致死循环
    guint32 mac_num = netcard_mac_address_total_len / NETCARD_MAC_ADDRESS_RESPONSE_LENGTH;
    guint8 port_num = 0;
    get_pcie_netcard_port_num(obj_handle, &port_num);
    if (mac_num < port_num) {
        debug_log(DLOG_DEBUG, "%s: %s invalid mac num %d, expect port num %d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), mac_num, port_num);
        g_free(netcard_mac_adress);
        return RET_ERR;
    }

    // 获取MAC地址成功
    for (guint32 index = 0; index < mac_num; index++) {
        set_netcard_mac_address(obj_handle, netcard_mac_adress, index);
    }

    g_free(netcard_mac_adress);
    return RET_OK;
}

LOCAL gint32 update_ioboard_firmware_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info, guint8 *updated)
{
    struct Version {
        guint8 major;
        guint8 minor;
        guint8 revision;
    };

    *updated = FALSE;

    struct Version ver = { 0 };
    gint32 ret = pm_get_firmware(info, (guint8 *)&ver, sizeof(ver));
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        return ret;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s get firmwareVersion fail, ret=%d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    if ((ver.major | ver.minor | ver.revision) == 0) {
        debug_log(DLOG_DEBUG, "%s: %s firmwareVersion empty.", __FUNCTION__, dfl_get_object_name(obj_handle));
        (void)dal_set_property_value_string(obj_handle, PROPERTY_IOBOARD_FIRMWARE_VERSION, NA_STR, DF_SAVE_TEMPORARY);
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
    ret = dal_get_property_value_string(obj_handle, PROPERTY_IOBOARD_FIRMWARE_VERSION, ver_old, STR_FIRMWARE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s.firmwareVersion fail, ret=%d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    if (strcmp(ver_str, ver_old) != 0) {
        ret = dal_set_property_value_string(obj_handle, PROPERTY_IOBOARD_FIRMWARE_VERSION, ver_str, DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: set %s.firmwareVersion fail, ret=%d.", __FUNCTION__,
                      dfl_get_object_name(obj_handle), ret);
            return ret;
        }
        *updated = TRUE;
    }
    return RET_OK;
}

LOCAL gint32 update_ioboard_temp(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info)
{
    guint16 temp_old = 0;
    OBJ_HANDLE netcard_handle = 0;

    gint32 ret = dal_get_specific_object_position(obj_handle, CLASS_NETCARD_NAME, &netcard_handle);
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        return ret;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s get netcard handle failed, ret %d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_uint16(netcard_handle, PROPERTY_NETCARD_CHIP_TEMP, &temp_old);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s.ChipTemp fail, ret=%d.", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    guint16 temp = 0;
    ret = pm_get_temp(info, (guint8 *)&temp, sizeof(guint16));
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        return ret;
    }

    if (ret != RET_OK) {
        if ((temp_old & SENSOR_INVALID_READING) == SENSOR_INVALID_READING) {
            return ret;
        }
        temp = temp_old | SENSOR_INVALID_READING;
        debug_log(DLOG_DEBUG, "%s: %s get chip temp fail, ret=%d", __FUNCTION__,
                  dfl_get_object_name(obj_handle), ret);
    } else {
        if (temp == PCIE_SENSOR_INVALID_READING) {
            temp = temp_old | SENSOR_INVALID_READING;
        } else if (temp == PCIE_SENSOR_NA_READING) {
            temp = (temp_old & PCIE_SENSOR_INVALID_READING) | SENSOR_NA_READING;
        }
    }

    ret = dal_set_property_value_uint16(netcard_handle, PROPERTY_NETCARD_CHIP_TEMP, temp, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: set %s.ChipTemp fail, ret=%d.", __FUNCTION__,
                  dfl_get_object_name(netcard_handle), ret);
    }
    return ret;
}

LOCAL void io_board_mntr_cb_free(gpointer data)
{
    if (data == NULL) {
        return;
    }
    IOBRDMNTRCB *cb = (IOBRDMNTRCB *)data;
    if (cb->pm_chip_info != NULL) {
        pm_clear(cb->pm_chip_info);
        cb->pm_chip_info = NULL;
    }
    g_free(cb);
}


LOCAL void io_board_set_default_value(OBJ_HANDLE obj_handle, guint16 mask)
{
    guint16 get_val = 0;
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_NETCARD_CHIP_TEMP, &get_val);
    if (ret == RET_OK) {
        (void)dal_set_property_value_uint16(obj_handle, PROPERTY_NETCARD_CHIP_TEMP,
            (get_val & PCIE_SENSOR_INVALID_READING) | mask, DF_NONE);
    }
    return;
}


LOCAL void io_board_monitor_init(IOBRDMNTRCB *cb)
{
    const guint8 RETRY_DIVISOR = 3;
    // 初始化失败，以6s为周期重试(轮询周期2s，每3次执行一次pm_init)；采用这种方式保证初始化失败不阻塞其它主板的数据更新
    // 重试周期保持和PcieCard一致
    if ((cb->retry_cnt % RETRY_DIVISOR) != 0) {
        cb->retry_cnt++;
        return;
    }

    gint32 ret = pm_init(cb->pm_chip_info);
    if (ret != RET_OK) {
        cb->retry_cnt++;
        if (cb->retry_cnt < MAX_RETRY_TIMES * RETRY_DIVISOR) {
            debug_log(DLOG_INFO, "%s: %s pm_init fail, retry = %u.", __FUNCTION__,
                dfl_get_object_name(cb->handle), cb->retry_cnt);
            return;
        }
        if (cb->set_default_proc != NULL) {
            cb->set_default_proc(cb->handle, SENSOR_INVALID_READING);
        }

        debug_log(DLOG_ERROR, "%s: %s pm_init fail, ret = %d.", __FUNCTION__, dfl_get_object_name(cb->handle), ret);
        cb->state = IOBRD_MNTR_FAIL;
        return;
    }

    vos_task_delay(COMMON_DELAY_TIME_FOR_RW);
    if (cb->power_state == 1) {
        cb->state = IOBRD_MNTR_UPDATE;
    } else {
        cb->state = IOBRD_MNTR_WAIT;
        if (cb->set_default_proc != NULL) {
            cb->set_default_proc(cb->handle, SENSOR_NA_READING);
        }
    }
}

LOCAL void io_board_monitor_wait(IOBRDMNTRCB *cb)
{
    if (cb->power_state != 0) {
        cb->retry_cnt = 0;
        cb->state = IOBRD_MNTR_INIT;
        return;
    }
    if (cb->update_firm_ver_proc == NULL) {
        return;
    }

    guint8 firm_updated = FALSE;
    gint32 ret = cb->update_firm_ver_proc(cb->handle, cb->pm_chip_info, &firm_updated);
    if (ret == RET_OK && firm_updated == TRUE) {
        cb->retry_cnt = 0;
        cb->state = IOBRD_MNTR_INIT;
    }
}

LOCAL void io_board_monitor_fail(IOBRDMNTRCB *cb)
{
    if (cb->last_power_state == 0 && cb->power_state == 1) {
        cb->retry_cnt = 0;
        cb->state = IOBRD_MNTR_INIT;
    }
}

LOCAL void io_board_monitor_update(IOBRDMNTRCB *cb)
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
        cb->state = IOBRD_MNTR_WAIT;
        if (cb->set_default_proc != NULL) {
            cb->set_default_proc(cb->handle, SENSOR_NA_READING);
        }
        return;
    }

    if (cb->pm_chip_info == NULL) {
        return;
    }

    for (guint8 i = 0; i < cb->item_table_len; i++) {
        const IOBRDMNTRITEM *item = &cb->item_table[i];
        if ((item == NULL) || (item->func == NULL)) {
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
    if (ret == RET_OK && firm_updated == TRUE) {
        cb->retry_cnt = 0;
        cb->state = IOBRD_MNTR_INIT;
    }
}


LOCAL gint32 run_io_board_state_machine(OBJ_HANDLE obj_handle, gpointer user_data)
{
    OBJ_HANDLE oob_handle = 0;
    // 如果OobChannel属性没有关联对象，不执行状态机（非Atlas 1951机型）
    if (dfl_get_referenced_object(obj_handle, PROPERTY_IOBOARD_OOB_CHAN, &oob_handle) != DFL_OK) {
        return RET_OK;
    }
    struct IoStateInfo {
        enum IoBrdMntrState state;
        void (*func)(IOBRDMNTRCB *cb);
    };

    const struct IoStateInfo io_board_state_info[] = {
        {IOBRD_MNTR_INIT, io_board_monitor_init},
        {IOBRD_MNTR_WAIT, io_board_monitor_wait},
        {IOBRD_MNTR_UPDATE, io_board_monitor_update},
        {IOBRD_MNTR_FAIL, io_board_monitor_fail}
    };

    IOBRDMNTRCB *cb = NULL;
    gint32 ret = get_obj_priv_data(obj_handle, (gpointer *)&cb);
    if (ret != RET_OK || cb == NULL) {
        debug_log(DLOG_DEBUG, "%s: %s get_obj_priv_data fail.", __FUNCTION__, dfl_get_object_name(obj_handle));
        return RET_OK;
    }

    debug_log(DLOG_MASS, "%s: %s state = %d.", __FUNCTION__, dfl_get_object_name(obj_handle), cb->state);

    if (cb->state >= IOBRD_MNTR_EXIT) {
        debug_log(DLOG_ERROR, "%s: %s invalid state = %d.", dfl_get_object_name(obj_handle), __FUNCTION__, cb->state);
    } else {
        io_board_state_info[cb->state].func(cb);
    }

    guint8 power_state = 0;
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_IOBOARD_PWR_STATE, &power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: %s get PowerState fail,ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    } else {
        cb->last_power_state = cb->power_state;
        cb->power_state = power_state;
    }

    put_obj_priv_data(obj_handle, cb);
    return RET_OK;
}

LOCAL const IOBRDMNTRITEM g_io_board_mntr_table[] = {
    {IOBRD_MNTR_ITEM_PERIOD, update_optical_module_temp},
    {IOBRD_MNTR_ITEM_PERIOD, update_netcard_link_status},
    {IOBRD_MNTR_ITEM_PERIOD, update_netcard_mac_address},
    {IOBRD_MNTR_ITEM_PERIOD, update_ioboard_temp}
};


LOCAL gint32 io_board_mntr_cb_new(OBJ_HANDLE obj_handle)
{
    IOBRDMNTRCB *cb = (IOBRDMNTRCB *)g_malloc0(sizeof(IOBRDMNTRCB));
    if (cb == NULL) {
        debug_log(DLOG_ERROR, "%s: %s malloc fail.", __FUNCTION__, dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    cb->handle = obj_handle;
    cb->state = IOBRD_MNTR_INIT;
    cb->item_table = g_io_board_mntr_table;
    cb->item_table_len = G_N_ELEMENTS(g_io_board_mntr_table) > UINT8_MAX ?
        UINT8_MAX : G_N_ELEMENTS(g_io_board_mntr_table);
    cb->retry_cnt = 0;
    cb->set_default_proc = io_board_set_default_value;
    cb->update_firm_ver_proc = update_ioboard_firmware_version;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_IOBOARD_SLOT, &cb->id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.Id fail, ret = %d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_IOBOARD_PWR_STATE, &cb->power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.PowerState fail,ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }

    OBJ_HANDLE oob_handle = 0;
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_IOBOARD_OOB_CHAN, &oob_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.OobChannel fail,ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }

    cb->pm_chip_info = oob_init_chip_info(oob_handle);
    if (cb->pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: init %s oob pm_chip_info fail", __FUNCTION__, dfl_get_object_name(obj_handle));
        goto EXIT;
    }

    ret = new_obj_priv_data(obj_handle, (gpointer)cb, io_board_mntr_cb_free);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: new %s obj priv data fail, ret = %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        goto EXIT;
    }
    return RET_OK;
EXIT:
    io_board_mntr_cb_free(cb);
    return RET_ERR;
}


LOCAL void io_board_monitor(gpointer data)
{
    (void)prctl(PR_SET_NAME, (gulong)"IoBoardMonitor");

    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_IOBOARD_NAME, &obj_list);
    if (ret != DFL_OK && ret != ERRCODE_OBJECT_NOT_EXIST) {
        return;
    }

    // 初始化失败需要考虑
    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)node->data;
        OBJ_HANDLE oob_handle = 0;
        if (dfl_get_referenced_object(obj_handle, PROPERTY_IOBOARD_OOB_CHAN, &oob_handle) != DFL_OK) {
            continue;
        }
        ret = io_board_mntr_cb_new(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: %s new obj priv data fail, ret = %d.", __FUNCTION__,
                dfl_get_object_name(obj_handle), ret);
            continue;
        }
    }

    g_slist_free(obj_list);

    const guint32 PERIOD = 2000;
    while (TRUE) {
        (void)dfl_foreach_object(CLASS_IOBOARD_NAME, run_io_board_state_machine, NULL, NULL);
        vos_task_delay(PERIOD);
    }
}


gint32 io_board_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *logic_ver = NULL;
    const gchar *manufacturer = NULL;
    gchar file_name[IO_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar io_board_info[IO_BOARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;
    const gchar *fw_ver = NULL;
    const gchar *partnum = NULL;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_IOBOARD_NAME, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }
    
    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, IO_BOARD_DUMPINFO_MAX_LEN + 1, IO_BOARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    (void)snprintf_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, IO_BOARD_DUMPINFO_MAX_LEN, "%s", "IO board Info\n");

    fwrite_back = fwrite(io_board_info, strlen(io_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, 0, IO_BOARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, IO_BOARD_DUMPINFO_MAX_LEN,
        "%-10s | %-10s | %-32s | %-10s | %-10s | %-15s | %-10s\n", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Logic Ver", "Firmware Ver", "PartNum");
    fwrite_back = fwrite(io_board_info, strlen(io_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_LOGICVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_FIRMWARE_VERSION);

        property_name_list = g_slist_append(property_name_list, PROPERTY_IOBOARD_PART_NUM);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get io board information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 0));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        logic_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        fw_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);
        partnum = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), 0);

        (void)memset_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, 0, IO_BOARD_DUMPINFO_MAX_LEN + 1);
        if (strlen(fw_ver) == 0) {
            iRet = snprintf_s(io_board_info, sizeof(io_board_info), sizeof(io_board_info) - 1,
                "0x%-4x     | %-10s | %-32s | %-10s | %-10s | %-15s | %-10s\n", board_id, name, manufacturer, pcb_ver,
                logic_ver, "N/A", partnum);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
        } else {
            iRet = snprintf_s(io_board_info, sizeof(io_board_info), sizeof(io_board_info) - 1,
                "0x%-4x     | %-10s | %-32s | %-10s | %-10s | %-15s | %-10s\n", board_id, name, manufacturer, pcb_ver,
                logic_ver, fw_ver, partnum);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
        }
        fwrite_back = fwrite(io_board_info, strlen(io_board_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, 0, IO_BOARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, IO_BOARD_DUMPINFO_MAX_LEN, "%s", "\n\n");
    fwrite_back = fwrite(io_board_info, strlen(io_board_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(io_board_info, IO_BOARD_DUMPINFO_MAX_LEN + 1, 0, IO_BOARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);
    
    return RET_OK;
}


LOCAL gint32 process_each_io_board(OBJ_HANDLE handle, gpointer data)
{
    gint32 ret = 0;

    
    ret = get_pcb_version(handle, PROPERTY_IOBOARD_PCBID, PROPERTY_IOBOARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = get_logic_version(handle, PROPERTY_IOBOARD_LOGICVERID, PROPERTY_IOBOARD_LOGICVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


gint32 io_board_add_object_callback(OBJ_HANDLE object_handle)
{
    
    card_manage_log_operation_log("IO board", object_handle, PROPERTY_IOBOARD_SLOT, PROPERTY_IOBOARD_NAME,
        CARD_PLUG_IN);
    OBJ_HANDLE oob_handle = 0;
    if (dfl_get_referenced_object(object_handle, PROPERTY_IOBOARD_OOB_CHAN, &oob_handle) == DFL_OK) {
        gint32 ret = io_board_mntr_cb_new(object_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: %s new obj priv data fail, ret=%d.", __FUNCTION__,
                dfl_get_object_name(object_handle), ret);
        }
    }

    return process_each_io_board(object_handle, NULL);
}


gint32 io_board_del_object_callback(OBJ_HANDLE object_handle)
{
    
    card_manage_log_operation_log("IO board", object_handle, PROPERTY_IOBOARD_SLOT, PROPERTY_IOBOARD_NAME,
        CARD_PLUG_OUT);
    
    OBJ_HANDLE oob_handle = 0;
    if (dfl_get_referenced_object(object_handle, PROPERTY_IOBOARD_OOB_CHAN, &oob_handle) == DFL_OK) {
        delete_obj_priv_data(object_handle);
    }
    return RET_OK;
}


gint32 io_board_set_firmware_version(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    const gchar *fw = NULL;
    gint32 ret = RET_ERR;
    gchar device_name[MAX_DEVICE_NAME_LEN + 1] = {0};
    OBJ_HANDLE comp_handle = 0;

    if (input_list == NULL) {
        return RET_ERR;
    }

    fw = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), 0);
    if (fw == NULL) {
        return ret;
    }

    
    ret = dal_set_property_value_string(object_handle, PROPERTY_IOBOARD_FIRMWARE_VERSION, fw, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set firmVersion failed.");
        return ret;
    }

    ret = dfl_get_referenced_object(object_handle, PROPERTY_IOBOARD_REFCOMPONENT, &comp_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get component handle failed.");
        return ret;
    }

    ret = dal_get_property_value_string(comp_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name, MAX_DEVICE_NAME_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get device name failed, ret = %d.", ret);
        return ret;
    }

    method_operation_log(caller_info, NULL, "Set %s firmware version to %s successfully", device_name, fw);
    return RET_OK;
}


gint32 io_board_init(void)
{
    (void)dfl_foreach_object(CLASS_IOBOARD_NAME, process_each_io_board, NULL, NULL);

    return RET_OK;
}


gint32 io_board_start(void)
{
    gulong task_id = 0;
    gint32 ret = vos_task_create(&task_id, "IoBoardMonitor", (TASK_ENTRY)io_board_monitor, NULL, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create IoBoardMonitor task fail, ret=%d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}
