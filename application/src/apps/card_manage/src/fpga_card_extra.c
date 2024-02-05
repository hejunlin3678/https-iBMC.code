

#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_comm.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "fpga_card.h"

LOCAL gint32 pcie_card_get_optical_temp_volt_from_mcu(PM_CHIP_INFO *pm_chip_info, guint32 *retry_time, guint8 cmd_arg,
    OP_TEMP_VOLT_DOUBLE *output_data);
LOCAL gint32 pcie_card_get_optical_power_current_from_mcu(PM_CHIP_INFO *pm_chip_info, guint32 *retry_time,
    guint8 cmd_arg, OP_POWER_CURRENT *output_data, guint32 data_len);


gint32 get_op_temp_volt_and_power_current_from_mcu(PM_CHIP_INFO *pm_chip_info, guint8 port_num,
    OP_TEMP_VOLT_DOUBLE *optical_temp_volt, OP_POWER_CURRENT *optical_power_current)
{
    gint32 ret;

    if (optical_temp_volt == NULL || optical_power_current == NULL || pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    ret = pcie_card_get_optical_temp_volt_from_mcu(pm_chip_info,
        &(pm_chip_info->optical_temp_read_fail_timestamp), port_num, optical_temp_volt);
    // 不支持操作码，退出
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        debug_log(DLOG_DEBUG,
            "%s: not support get pcie_card_get_optical_temp_volt_from_mcu, port_silknum is %d, ret %d",
            __FUNCTION__, port_num, ret);
        return ret;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG,
            "%s: get pcie_card_get_optical_temp_volt_from_mcu failed, port_silknum is %d, ret %d", __FUNCTION__,
            port_num, ret);
    }

    ret = pcie_card_get_optical_power_current_from_mcu(pm_chip_info,
        &(pm_chip_info->optical_current_read_fail_timestamp), port_num, optical_power_current,
        sizeof(optical_power_current));
    // 不支持操作码，退出
    if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
        debug_log(DLOG_DEBUG,
            "%s: not support get pcie_card_get_optical_power_current_from_mcu, port_silknum is %d, ret %d",
            __FUNCTION__, port_num, ret);
        return ret;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG,
            "%s: get pcie_card_get_optical_power_current_from_mcu failed, port_silknum is %d, ret %d",
            __FUNCTION__, port_num, ret);
    }

    return RET_OK;
}


LOCAL gint32 pcie_card_get_optical_temp_volt_from_mcu(PM_CHIP_INFO *pm_chip_info, guint32 *retry_time, guint8 cmd_arg,
    OP_TEMP_VOLT_DOUBLE *output_data)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint32 duration = 0;
    OP_TEMP_VOLT optical_temp_volt = { 0 };

    if (output_data == NULL) {
        debug_log(DLOG_DEBUG, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_MFPGA_OPTICAL_TEMP_INFO;
    head_para.cmd_arg = cmd_arg;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&optical_temp_volt,
        sizeof(optical_temp_volt));
    if (ret != RET_OK) {
        return ret;
    } else if (ret == RET_OK) {
        *retry_time = 0;

        // 如果MCU返回的值为0x7fff,表示MCU获取温度电压失败,将值置为0x8000告警值,如果为0x7ffd,表示光模块不在位
        if (optical_temp_volt.temp == OP_TEMP_VOLT_CURRENT_POWER_INVALID) {
            output_data->temp = SENSOR_INVALID_READING;
        } else if (optical_temp_volt.temp == OP_TEMP_VOLT_CURRENT_POWER_NA) {
            output_data->temp = 0xFFFFFFFFFFFFFFFFULL;
        } else {
            output_data->temp = (gint32)(REVERT_UINT16(optical_temp_volt.temp) / 256); // 处理温度，256代表将数据处理，右移8位
        }

        if (optical_temp_volt.volt == OP_TEMP_VOLT_CURRENT_POWER_INVALID) {
            output_data->volt = SENSOR_INVALID_READING;
        } else if (optical_temp_volt.volt == OP_TEMP_VOLT_CURRENT_POWER_NA) {
            output_data->volt = 0xFFFFFFFFFFFFFFFFULL;
        } else {
            // 处理电压值，（电压值 / 1000）* 0.1转换
            output_data->volt = (gint32)(REVERT_UINT16(optical_temp_volt.volt) / 1000) * 0.1;
        }
    } else {
        guint32 cur_timestamp = (guint32)vos_tick_get();
        if (*retry_time == 0) {
            *retry_time = cur_timestamp;
        } else if (cur_timestamp >= (*retry_time)) {
            duration = cur_timestamp - *retry_time;
        } else {
            duration = cur_timestamp + (G_MAXUINT32 - *retry_time);
        }

        // 如果超过防抖时间仍然读取不到，将值置为0x8000告警值
        if (duration > ((guint32)pm_chip_info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
            output_data->temp = SENSOR_INVALID_READING;
            output_data->volt = SENSOR_INVALID_READING;
            debug_log(DLOG_DEBUG, "%s : get optical module temperature/voltage failed, return %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL gint32 pcie_card_get_optical_power_current_from_mcu(PM_CHIP_INFO *pm_chip_info, guint32 *retry_time,
    guint8 cmd_arg, OP_POWER_CURRENT *output_data, guint32 data_len)
{
    guint8 *buf = NULL;
    guint32 buf_len = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint32 interval = 0;
    guint8 channel_index = 0;
    OP_POWER_CURRENT_CONVERT *optical_power_current_convert = NULL;

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_MFPGA_OPTICAL_POWER_INFO;
    head_para.cmd_arg = cmd_arg;
    gint32 ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, &buf, &buf_len);
    if (ret != RET_OK) {
        return ret;
    } else if (ret == RET_OK && buf != NULL) {
        *retry_time = 0;
        guint8 channel_num = buf[0];
        optical_power_current_convert = (OP_POWER_CURRENT_CONVERT *)(buf + 1);

        for (channel_index = 0; channel_index < channel_num; channel_index++) {
            // 防止数组越界
            if (channel_index >= MAX_CHANNEL_NUM) {
                break;
            }
            // 把各通道的接收功率放到一个数组变量里面
            output_data->RxPower[channel_index] = REVERT_UINT16(optical_power_current_convert[channel_index].RxPower);
            // 把各通道的发送功率放到一个数组变量里面
            output_data->TxPower[channel_index] = REVERT_UINT16(optical_power_current_convert[channel_index].TxPower);
            // 把各通道的偏置电流放到一个数组变量里面
            output_data->TxBiasCurrent[channel_index] =
                REVERT_UINT16(optical_power_current_convert[channel_index].TxBiasCurrent);
        }
    } else {
        guint32 cur_timestamp = (guint32)vos_tick_get();
        if (*retry_time == 0) {
            *retry_time = cur_timestamp;
        } else if (cur_timestamp >= (*retry_time)) {
            interval = cur_timestamp - *retry_time;
        } else {
            interval = cur_timestamp + (G_MAXUINT32 - *retry_time);
        }

        // 如果超过防抖时间仍然读取不到，将值置为0xffff
        if (interval > ((guint32)pm_chip_info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
            SET_ARRAY_VALUE(output_data->TxBiasCurrent, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
            SET_ARRAY_VALUE(output_data->TxPower, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
            SET_ARRAY_VALUE(output_data->RxPower, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
            debug_log(DLOG_DEBUG, "%s : get optical module power/current failed, return %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    }

    if (buf != NULL) {
        g_free(buf);
    }
    return RET_OK;
}
