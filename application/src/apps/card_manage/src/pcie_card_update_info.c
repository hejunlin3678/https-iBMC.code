

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include <linux/pci_regs.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "pcie_card_update_class_info.h"




LOCAL gint32 m_update_pcie_card = 0;


void pcie_card_update_version_info(OBJ_HANDLE obj_handle)
{
    gchar accesor_name[ACCESSOR_NAME_LEN] = {0};

    gint32 ret = dfl_get_property_accessor(obj_handle, PROPERTY_PCIE_CARD_PCB_ID_REG, accesor_name, ACCESSOR_NAME_LEN);
    
    if (ret == DFL_OK) {
        (void)get_pcb_version(obj_handle, PROPERTY_PCIE_CARD_PCB_ID_REG, PROPERTY_PCIE_CARD_PCB_VER);
    }
}


void pcie_card_update_resId_info(OBJ_HANDLE obj_handle, guint8 slot_id)
{
    guchar cpu_id = 0;
    
    gint32 ret = get_card_cpu_id(obj_handle, &cpu_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get %s ResID failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
        return;
    }
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_CPU_ID, cpu_id, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Update %s ResID(%d) failed! ret:%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
            cpu_id, ret);
        return;
    }
    
}


void pcie_card_update_container_info(OBJ_HANDLE obj_handle)
{
    OBJ_HANDLE anchor_handle = 0;
    OBJ_HANDLE connector_handle = 0;
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE smm_pcie_info_handle = 0;
    guchar component_type = 0;
    guchar component_slot = 0;

    gint32 ret = dal_get_specific_object_position(obj_handle, CLASS_NAME_SMM_PCIEINFO, &smm_pcie_info_handle);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_specific_object_position(obj_handle, CLASS_ANCHOR, &anchor_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get anchor for object handle (%s) failed, ret is %d", dfl_get_object_name(obj_handle),
            ret);
        return;
    }

    ret = dal_get_anchor_related_connector(anchor_handle, &connector_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get connector for anchor (%s) failed, ret is %d", dfl_get_object_name(anchor_handle),
            ret);
        return;
    }

    ret = dal_get_specific_object_position(connector_handle, CLASS_COMPONENT, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get component handle with object %s failed", dfl_get_object_name(connector_handle));
        return;
    }

    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &component_slot);

    ret = dal_set_property_value_byte(smm_pcie_info_handle, PROPERTY_SMM_PCIEINFO_CONTAINER_TYPE, component_type,
        DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set property value %s for %s failed, ret is %d", PROPERTY_SMM_PCIEINFO_CONTAINER_TYPE,
            dfl_get_object_name(smm_pcie_info_handle), ret);
    }

    ret = dal_set_property_value_byte(smm_pcie_info_handle, PROPERTY_SMM_PCIEINFO_CONTAINER_SLOT, component_slot,
        DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set property value %s for %s failed, ret is %d", PROPERTY_SMM_PCIEINFO_CONTAINER_SLOT,
            dfl_get_object_name(smm_pcie_info_handle), ret);
    }

    return;
}


void pcie_card_update_board_id(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = RET_OK;
    guint16 board_id = 0;
    guint8 buf[KEY_BOARD_ID_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    
    if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS) {
        (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
        head_para.opcode = REGISTER_KEY_BOARD_ID;
        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_BOARD_ID_LEN);
    } else {
        ret = pm_get_board_id(pm_chip_info, buf, KEY_BOARD_ID_LEN);
    }

    
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get pcie card BoardID failed, ret = %d.", __FUNCTION__, ret);
        return;
    }

    // 小字节序
    board_id = ((buf[1] << 8) | buf[0]);
    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_BOARD_ID, board_id, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : update pcie card BoardID failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
}


void pcie_card_update_bom_id(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret;
    guint8 bom_id;
    guint8 buf[KEY_BOM_ID_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS) {
        head_para.opcode = REGISTER_KEY_BOM_ID;

        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_BOM_ID_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : Get pcie card BomID failed, ret = %d", __FUNCTION__, ret);
            return;
        }
        bom_id = buf[0];
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_BOM_ID, bom_id, DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : Update pcie card BomID failed, ret = %d", __FUNCTION__, ret);
            return;
        }
    }
}


LOCAL void pcie_card_update_chip_reset_time(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 buf[KEY_CHIP_RESET_TIME] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__);
        return;
    }

    gint32 ret = pm_get_chip_reset_time(pm_chip_info, buf, KEY_CHIP_RESET_TIME);
    if (ret == RET_OK) {
        pm_chip_info->chip_reset_time = ((buf[1] << 8) | buf[0]);
    } else {
        pm_chip_info->chip_reset_time = DEFAULT_CHIP_RESET_TIME;
        debug_log(DLOG_DEBUG, "%s : get chip reset time failed, use default time(%ds), ret = %d.", __FUNCTION__,
            DEFAULT_CHIP_RESET_TIME, ret);
    }
}


void pcie_card_update_pcb_ver(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define PCB_MIN_VALUE 1
#define PCB_MAX_VALUE 42
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint8 buf = 0;
    gint32 ret = RET_OK;
    gchar pcbVer[STR_PCB_VER_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    
    if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS) {
        (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
        head_para.opcode = REGISTER_KEY_PCB_ID;
        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, &buf, KEY_PCB_ID_LEN);
    } else {
        ret = pm_get_pcb_id(pm_chip_info, &buf, KEY_PCB_ID_LEN);
    }

    
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get pcie card PCB ID failed, ret = %d.", __FUNCTION__, ret);
        return;
    }

    
    // buf的有效性判断
    if (buf < PCB_MIN_VALUE || buf > PCB_MAX_VALUE) {
        debug_log(DLOG_ERROR, "%s: pcie card PCB ID %u is not valid", __FUNCTION__, buf);
        return;
    }
    

    (void)snprintf_s(pcbVer, STR_PCB_VER_LEN, STR_PCB_VER_LEN - 1, ".%c", (gchar)(buf + 64));

    (void)dal_set_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_PCB_ID_REG, buf, DF_SAVE_TEMPORARY);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_PCIE_CARD_PCB_VER, pcbVer, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : update pcie card PCB ID failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
}


LOCAL void pcie_card_update_firm_ver(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gchar firmware[STR_FIRMWARE_LEN + 1] = {0};
    guint8 buf[KEY_FIRM_VER_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    gint32 ret = pm_get_firmware(pm_chip_info, buf, KEY_FIRM_VER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Get pcie card firmware failed, ret = %d.", __FUNCTION__, ret);
        return;
    }

    (void)snprintf_s(firmware, STR_FIRMWARE_LEN + 1, STR_FIRMWARE_LEN, "%x.%x.%x", buf[0], buf[1], buf[2]);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_ACCELERATE_FIRMWARE_VERSION, firmware, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Update pcie card firmware version failed, ret = %d.", __FUNCTION__, ret);
        return;
    }
}


LOCAL void pcie_card_update_shell_id(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gchar firmware[STR_FIRMWARE_LEN + 1] = {0};
    guint8 buf[KEY_SHELL_ID_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    gint32 ret = pm_get_shell_id(pm_chip_info, buf, KEY_SHELL_ID_LEN);
    // shell id不是所有板卡都有，修改为debug级别，防止反复刷日志
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Get pcie card shell id failed, ret = %d.", __FUNCTION__, ret);
        return;
    }

    (void)snprintf_s(firmware, STR_FIRMWARE_LEN + 1, STR_FIRMWARE_LEN, "%02x%02x%02x%02x", buf[0], buf[1], buf[2],
        buf[3]);

    // 设置属性
    ret = dal_set_property_value_string(obj_handle, PROPERTY_FPGA_CARD_SHELL_ID, firmware, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Update pcie card shell id failed, ret = %d.", __FUNCTION__, ret);
        return;
    }

    return;
}


LOCAL void pcie_card_update_SN(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 buf[KEY_SN_LEN] = {0};
    guint32 buf_len;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "input param error");
        return;
    }

    gint32 ret = pm_get_sn(pm_chip_info, buf, KEY_SN_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Get pcie card SN failed, ret = %d. ", __FUNCTION__, ret);
        return;
    }
    
    if (buf[KEY_SN_LEN - 1] == '\0') {
        buf_len = strlen((const gchar *)(buf));
    } else {
        buf_len = sizeof(buf);
    }
    if (g_utf8_validate((gchar *)(buf), buf_len, NULL) != TRUE) {
        debug_log(DLOG_ERROR, "%s : SN is not legal, ret = %d.", __FUNCTION__, ret);
        return;
    }
    
    ret = dal_set_property_value_string(obj_handle, PROPERTY_ACCELERATE_CARD_SN, (const gchar *)buf, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Update pcie card SN failed, ret = %d. ", __FUNCTION__, ret);
        return;
    }
}


void pcie_card_update_ddr_temp(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint32 i = 0;
    guint8 *buf = NULL;
    guint32 buf_len = 0;
    guint16 temp = 0;
    guint8 dimm_num = 0;
    guint16 max_temp = 0;
    guint16 temp_value = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): input param error", __FUNCTION__, __LINE__);
        return;
    }

    // 只有FPGA卡才支持
    if (pm_chip_info->card_type != FPGA_ACCELERATE_CARD) {
        return;
    }

    // 从属性中获取当前值
    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_DIMM_TEMP, &temp_value);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get ddr temp failed, ret: %d", __FUNCTION__, ret);
        return;
    }

    
    if (pcie_card_mgnt_ready() != TRUE) {
        temp_value = (temp_value & PCIE_SENSOR_INVALID_READING) | SENSOR_NA_READING; // 先清除最高位再置次高位为1
        ret = dal_set_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_DIMM_TEMP, temp_value, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s (%d): Update pcie card ddr temperature failed!", __FUNCTION__, __LINE__);
            return;
        }
        return;
    }

    ret = pm_get_ddr_temp(pm_chip_info, &buf, &buf_len);
    if (ret != RET_OK) {
        // 获取失败
        guint32 duration = calc_ddr_temp_read_fail_duration(pm_chip_info);
        if (duration > ((guint32)pm_chip_info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
            temp_value = temp_value | SENSOR_INVALID_READING;
            debug_log(DLOG_DEBUG, "%s (%d): pm_get_temp failed, ret=0x%4x", __FUNCTION__, __LINE__, ret);
        }
    } else {
        pm_chip_info->ddr_temp_read_fail_timestamp = 0;

        for (i = 0; i < buf_len - 1; i += 2) {
            temp = buf[i] | (buf[i + 1] << 8);
            dimm_num++;
            debug_log(DLOG_DEBUG, "%s (%d): dimm_num: %d; ddr temp: %d!", __FUNCTION__, __LINE__, dimm_num, temp);

            if (temp == PCIE_SENSOR_INVALID_READING) {
                max_temp = PCIE_SENSOR_INVALID_READING;
                break;
            }

            // 如果当前温度有效，则更新最大温度和最大温度号
            if ((temp != PCIE_SENSOR_NA_READING) && (max_temp < temp)) {
                max_temp = temp;
            }
        }

        temp_value = max_temp;

        if (temp_value == PCIE_SENSOR_INVALID_READING) {
            temp_value = temp_value | SENSOR_INVALID_READING;
        }

        g_free(buf);
    }

    // 更新到属性
    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_DIMM_TEMP, temp_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: update pcie card ddr temperature failed", __FUNCTION__);
        return;
    }

    return;
}


void pcie_card_update_firmware_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gchar firm_version[STR_FIRMWARE_LEN] = {0};
    guint8 buf[KEY_FIRM_VER_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }

    if (pm_chip_info->card_type == SDI_CARD) {
        debug_log(DLOG_DEBUG, "%s : not support", dfl_get_object_name(obj_handle));
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_FIRMWARE;
    head_para.data_object_index = 1;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_FIRM_VER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get chip firmware version failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    if (buf[0] + buf[1] + buf[2] == 0) {
        debug_log(DLOG_DEBUG, "%s: firmware version is not valid, firmware is %x.%x.%x", __FUNCTION__, buf[0], buf[1],
            buf[2]);
        return;
    }
    (void)snprintf_s(firm_version, STR_FIRMWARE_LEN, STR_FIRMWARE_LEN - 1, "%x.%x.%x", buf[0], buf[1], buf[2]);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_ACCELERATE_FIRMWARE_VERSION, firm_version,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: update pcie card chip firmware version failed", __FUNCTION__);
        return;
    }
}

void update_sensors_reading_state(OBJ_HANDLE obj_handle, guint16 state)
{
    GSList *temp_sensor_list = NULL;
    GSList *node = NULL;
    guint16 temp_reading = 0;

    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR, &temp_sensor_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s : get PCIe card temperature sensors handle failed, return %d", __FUNCTION__, ret);
        return;
    }

    for (node = temp_sensor_list; node != NULL; node = g_slist_next(node)) {
        (void)dal_get_property_value_uint16((OBJ_HANDLE)node->data, PROPERTY_P_DEVICE_SENSOR_VALUE, &temp_reading);
        temp_reading = (temp_reading & PCIE_SENSOR_INVALID_READING) | state;
        (void)dal_set_property_value_uint16((OBJ_HANDLE)node->data, PROPERTY_P_DEVICE_SENSOR_VALUE, temp_reading,
            DF_NONE);
        temp_reading = 0;
    }

    g_slist_free(temp_sensor_list);

    return;
}

void pcie_card_update_temperature_sensors(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define TEMP_SENSOR_NAME_LEN 8
#define TEMP_SENSOR_READING_LEN 2
#define TEMP_SENSOR_DATA_TOTAL_LEN 160 // 16个传感器数据
    errno_t securec_rv = EOK;
    gint32 ret = RET_OK;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint8 buf[TEMP_SENSOR_DATA_TOTAL_LEN + 1] = {0};
    gchar sensor_name[TEMP_SENSOR_NAME_LEN + 1] = {0};
    OBJ_HANDLE temp_sensor_handle = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter is NULL", __FUNCTION__);
        return;
    }
    
    head_para.opcode = REGISTER_KEY_DEVICE_TEMP;
    head_para.data_object_index = 1;
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, TEMP_SENSOR_DATA_TOTAL_LEN);
    if (ret != RET_OK && ret != PM_ERROR_CODE_REQUEST_LEN_ERROR) {
        debug_log(DLOG_DEBUG, "%s : get temperature sensors failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    guint8 sensor_num = buf[0]; // 温度个数
    gint32 offset = 1;
    guint16 sensor_reading;
    for (guint32 i = 0; i < sensor_num; i++) {
        if ((securec_rv =
                    strncpy_s(sensor_name, sizeof(sensor_name), (gchar *)&buf[offset], TEMP_SENSOR_NAME_LEN)) != EOK) {
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, return %d", __FUNCTION__, securec_rv);
            return;
        }
        sensor_reading =
            buf[offset + TEMP_SENSOR_NAME_LEN] | (buf[offset + TEMP_SENSOR_NAME_LEN + 1] << 8); // 温度值

        ret = dal_get_specific_position_object_string(obj_handle, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR,
            PROPERTY_P_DEVICE_SENSOR_NAME, sensor_name, &temp_sensor_handle);
        if (ret == RET_OK) {
            (void)dal_set_property_value_uint16(temp_sensor_handle, PROPERTY_P_DEVICE_SENSOR_VALUE, sensor_reading,
                DF_NONE);
        }
        
        offset = offset + TEMP_SENSOR_NAME_LEN + TEMP_SENSOR_READING_LEN;
        
        if (offset + TEMP_SENSOR_NAME_LEN + TEMP_SENSOR_READING_LEN > TEMP_SENSOR_DATA_TOTAL_LEN) {
            debug_log(DLOG_INFO, "%s : offset %d is too long", __FUNCTION__, offset);
            break;
        }
    }

    return;
}


void pcie_card_update_sn(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    errno_t safe_fun_ret = EOK;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint32 buf_len = 0;
    guint8 buf[KEY_SN_LEN] = {0};
    guint8 *sn_buf = NULL;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__);
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_SN;
    gint32 ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, &sn_buf, &buf_len);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get pcie card sn failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    if (sn_buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : get pcie card SN failed, SN is NULL", __FUNCTION__);
        return;
    }
    safe_fun_ret = memcpy_s(buf, KEY_SN_LEN, sn_buf, buf_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    g_free(sn_buf);
    
    if (buf[KEY_SN_LEN - 1] == '\0') {
        buf_len = strlen((const gchar *)(buf));
    } else {
        buf_len = sizeof(buf);
    }
    if (g_utf8_validate((gchar *)(buf), buf_len, NULL) != TRUE) {
        debug_log(DLOG_ERROR, "%s : SN is not legal, ret = %d.", __FUNCTION__, ret);
        return;
    }
    
    ret =
        dal_set_property_value_string(obj_handle, PROPERTY_FPGA_CARD_SERIALNAME, (const gchar *)buf, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : update pcie card sn failed, ret = %d", __FUNCTION__, ret);
        return;
    }
}

gint32 pcie_mgnt_init_info_by_i2c(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    OBJ_HANDLE ref_handle = 0;
    OBJ_HANDLE slave_obj_handle = 0;
    const gchar *ptr_obj_name = NULL;
    gchar slave_card_class_name[MAX_NAME_SIZE] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): pm_chip_info is null!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): Get slave_card name failed!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (strlen(slave_card_class_name) != 0) {
        // 获取PCIE卡对应的压缩卡对象句柄
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret == RET_OK) {
            ret = dfl_get_referenced_object(slave_obj_handle, PROPERTY_ACCELERATE_CARD_REF_CHIP, &ref_handle);
            if (ret == DFL_OK) {
                ptr_obj_name = dfl_get_object_name(ref_handle);
                if (ptr_obj_name == NULL) {
                    debug_log(DLOG_ERROR, "%s (%d): Get object name failed, handle %" OBJ_HANDLE_FORMAT, __FUNCTION__,
                        __LINE__, ref_handle);
                    return RET_ERR;
                }

                (void)strncpy_s(pm_chip_info->chip_name, sizeof(pm_chip_info->chip_name), ptr_obj_name,
                    sizeof(pm_chip_info->chip_name) - 1);

                // 初始化芯片信息及偏移映射信息
                ret = pm_init(pm_chip_info);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "%s (%d): Init Pcie card management failed!", __FUNCTION__, __LINE__);
                    return RET_ERR;
                }

                // 更新加速卡相关信息
                pcie_card_update_board_id(obj_handle, pm_chip_info);
                pcie_card_update_bom_id(obj_handle, pm_chip_info);
                pcie_card_update_pcb_ver(obj_handle, pm_chip_info);
                pcie_card_update_chip_reset_time(obj_handle, pm_chip_info);
                pcie_card_update_firm_ver(slave_obj_handle, pm_chip_info);
                
                pcie_card_update_shell_id(slave_obj_handle, pm_chip_info);
                
                pcie_card_update_SN(slave_obj_handle, pm_chip_info);
            }
        }
    }

    return RET_OK;
}

gint32 pcie_ssd_update_status(OBJ_HANDLE obj_handle, gpointer user_data)
{
    GVariant *property_value = NULL;
    const gchar **accessor_name;
    gsize length = 0;
    guint32 count = 0;
    gchar read_buf[PCIE_SSD_MAX_BUFF_SIZE] = {0};

    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "Error : Input parameter error. ");
        return RET_ERR;
    }

    gint32 ret = dfl_get_property_value(obj_handle, PROPERTY_PCIESSDCARD_STORAGE_LOGIC, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get PCIeSSDCard StorageLoc faild!");
        return RET_OK;
    }

    
    accessor_name = g_variant_get_strv(property_value, &length);
    if (accessor_name == NULL) {
        debug_log(DLOG_DEBUG, "Get accessor name is NULL!");
        return RET_OK;
    }

    for (count = 0; count < length; count++) {
        if (strlen(accessor_name[count]) == 0) {
            continue;
        }
        (void)memset_s(read_buf, sizeof(read_buf), 0x00, sizeof(read_buf));
        
        ret = dal_block_read(accessor_name[count], read_buf, sizeof(read_buf));
        if (ret == RET_OK) {
            
            if (PCIE_SSD_SN == count) {
                ret = dal_set_property_value_string(obj_handle, PROPERTY_PCIESSDCARD_SN, read_buf, DF_NONE);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Set PCIeSSDCard property serial number failed : ret = %d ", ret);
                }
            }
            
            else if (PCIE_SSD_MN == count) {
                ret = dal_set_property_value_string(obj_handle, PROPERTY_PCIESSDCARD_MN, read_buf, DF_NONE);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Set PCIeSSDCard property model number failed! : ret = %d ", ret);
                }
            }
            
            else if (PCIE_SSD_FW_VER == count) {
                ret = dal_set_property_value_string(obj_handle, PROPERTY_PCIESSDCARD_FWVERSION, read_buf, DF_NONE);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Set PCIeSSDCard property  Firmware Version failed! : ret = %d ", ret);
                }
            }
        } else {
            debug_log(DLOG_DEBUG, "Read info%d failed : ret = %d ", count, ret);
        }
    }

    g_free(accessor_name);
    g_variant_unref(property_value);

    return RET_OK;
}

gint32 check_update_pfinfo_condition(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 sync_pfinfo = 0;
    gchar class_name [ MAX_NAME_SIZE ] = {0};
    // 为避免循环太频繁访问I2C，每进来5次才进行一次I2C的读取
    pm_chip_info->pf_update_delay_cnt++;
    if (pm_chip_info->pf_update_delay_cnt % 5 != 0) {
        return FALSE;
    }
    pm_chip_info->pf_update_delay_cnt = 0;

    // 如果不是ExtendPcieCard 就退出，不执行下面的操作；
    gint32 ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get class_name failed. ret(%d)", __FUNCTION__, ret);
        return FALSE;
    }

    if (g_strcmp0(class_name, CLASS_EXTEND_PCIECARD_NAME) != 0) {
        debug_log(DLOG_INFO, "class_name is not ExtendPcieCard!");
        return FALSE;
    }

    
    if (pcie_card_mgnt_ready() != TRUE && !is_ocp3_card(obj_handle)) {
        return FALSE;
    }
    // 如果获取sync_pfinfo失败或者不是1，则是不支持pf信息显示的网卡，直接退出；
    ret = dal_get_property_value_byte(obj_handle, PROPETRY_EXTEND_PCIECARD_SYNC_PFINFO, &sync_pfinfo);
    if ((ret != RET_OK) || (sync_pfinfo == 0)) {
        debug_log(DLOG_INFO, "%s: not support pfinfo, %d, obj_handle %s, ret %d", __FUNCTION__, sync_pfinfo,
            dfl_get_object_name(obj_handle), ret);
        return FALSE;
    } else {
        // 所有异常情况都不是，则返回成功，表示需要通过I2C去获取pf信息
        return TRUE;
    }
}

LOCAL guint32 __calc_temperature_read_fail_duration(PM_CHIP_INFO *pm_chip_info)
{
    guint32 duration = 0;
    guint32 cur_timestamp = (guint32)vos_tick_get();

    if (pm_chip_info->temp_read_fail_timestamp == 0) {
        pm_chip_info->temp_read_fail_timestamp = cur_timestamp;
    } else if (pm_chip_info->temp_read_fail_timestamp <= cur_timestamp) {
        duration = cur_timestamp - pm_chip_info->temp_read_fail_timestamp;
    } else {
        duration = cur_timestamp + (G_MAXUINT32 - pm_chip_info->temp_read_fail_timestamp);
    }

    return duration;
}


void pcie_card_update_temperature(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = RET_OK;
    guint16 temp_value = 0;
    guint16 get_value = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : parameter is NULL", __FUNCTION__);
        return;
    }

    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, &temp_value);

    
    
    if ((pcie_card_mgnt_ready() == TRUE) || (check_is_atlas_smm() == RET_OK)) {
        

        
        if (pm_chip_info->inf_protocol == PCIE_MGNT_PROTOCOL_STD_SMBUS) {
            (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
            head_para.opcode = REGISTER_KEY_CHIP_TEMP;
            head_para.data_object_index = 1;
            ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&get_value,
                sizeof(guint16));
        } else {
            ret = pm_get_temp(pm_chip_info, (guint8 *)&get_value, sizeof(guint16));
        }

        
        if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
            return;
        } else if (ret == RET_OK) {
            pm_chip_info->temp_read_fail_timestamp = 0;
            temp_value = get_value;
        } else {
            guint32 duration = __calc_temperature_read_fail_duration(pm_chip_info);
            if (duration > ((guint32)pm_chip_info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
                if ((temp_value & SENSOR_INVALID_READING) == SENSOR_INVALID_READING) {
                    return;
                }
                temp_value = temp_value | SENSOR_INVALID_READING;
                debug_log(DLOG_DEBUG, "%s : get chip temperature over I2C/SMBus failed, return %d", __FUNCTION__, ret);
            }
        }
    } else {
        temp_value = (temp_value & PCIE_SENSOR_INVALID_READING) | SENSOR_NA_READING; // 先清除最高位再置次高位为1
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, temp_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : update pcie card chip temperature failed, return %d", __FUNCTION__, ret);
        return;
    }
}

void pcie_card_update_ecc_count(OBJ_HANDLE obj_handle, OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint32 *ecc_count_buf = NULL;
    guint32 get_value = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint8 chip_index = 0;
    guint8 chip_number = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : Input parameter pm_chip_info is NULL.", __FUNCTION__);
        return;
    }

    // 只有DMINI卡才支持
    if (pm_chip_info->card_type != DMINI_CARD) {
        debug_log(DLOG_MASS, "card type is %d.", pm_chip_info->card_type);
        return;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_CHIP_NUMBER, &chip_number);
    if (ret != DFL_OK || chip_number == 0 || chip_number == INVALID_DATA_BYTE) {
        debug_log(DLOG_ERROR, "%s : get property failed, ret = %d, chip_number = %d", __FUNCTION__, ret, chip_number);
        return;
    }

    ecc_count_buf = (guint32 *)g_malloc0(sizeof(guint32) * chip_number);
    if (ecc_count_buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : malloc memory failed", __FUNCTION__);
        return;
    }
    
    if (pcie_card_mgnt_ready() == TRUE) {
        for (chip_index = 1; chip_index <= chip_number; chip_index++) {
            (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
            head_para.opcode = REGISTER_KEY_ECC_STATISTICS;
            head_para.data_object_index = chip_index;
            ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&get_value,
                sizeof(guint32));
            if (ret != RET_OK) {
                ecc_count_buf[chip_index - 1] = INVALID_DATA_DWORD;
                debug_log(DLOG_DEBUG, "%s : get ECC statitics failed, chip_index %d, return %d", __FUNCTION__,
                    chip_index, ret);
                continue;
            }
            ecc_count_buf[chip_index - 1] = get_value;
        }
    }

    ret = dal_set_property_value_array_uint32(slave_obj_handle, PROPERTY_PCIE_CARD_CHIPECC, ecc_count_buf, chip_number,
        DF_NONE);
    g_free(ecc_count_buf);

    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : update pcie card ECC failed, return %d.", __FUNCTION__, ret);
        return;
    }
}

void update_temp_read_state(OBJ_HANDLE obj_handle, guint16 set_value)
{
    guint16 temp_value = 0;

    if ((obj_handle == 0) || ((SENSOR_INVALID_READING != set_value) && (SENSOR_NA_READING != set_value))) {
        return;
    }

    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, &temp_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Get pcie card temperature failed, ret = %d. ", __FUNCTION__, ret);
        return;
    }

    if (set_value == (temp_value & set_value)) {
        return;
    }

    if (set_value == SENSOR_INVALID_READING) {
        temp_value = temp_value | set_value;
    } else {
        temp_value = (temp_value & PCIE_SENSOR_INVALID_READING) | set_value;
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, temp_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : Update pcie card temperature failed, ret = %d. ", __FUNCTION__, ret);
        return;
    }

    return;
}


void update_ddr_temp_read_state(OBJ_HANDLE obj_handle, guint16 set_value)
{
    gint32 ret = RET_OK;
    gchar slave_card_class_name[CLASSE_NAME_LEN] = {0};
    OBJ_HANDLE slave_obj_handle = 0;
    guint16 ddr_temp = 0;

    if ((obj_handle == 0) || ((set_value != SENSOR_INVALID_READING) && (set_value != SENSOR_NA_READING))) {
        return;
    }

    // 如果有ddrtemp，也设置
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strncmp(slave_card_class_name, CLASS_PCIE_FPGA_CARD, strlen(CLASS_PCIE_FPGA_CARD)) == 0) {
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : Get slave card object handle failed, ret = %d. ", __FUNCTION__, ret);
            return;
        }

        ret = dal_get_property_value_uint16(slave_obj_handle, PROPERTY_FPGA_CARD_DIMM_TEMP, &ddr_temp);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : Get slave card ddr temperature failed, ret = %d. ", __FUNCTION__, ret);
            return;
        }

        if ((ddr_temp & SENSOR_INVALID_READING) == SENSOR_INVALID_READING) {
            return;
        }

        if (set_value == SENSOR_INVALID_READING) {
            ddr_temp = ddr_temp | set_value;
        } else {
            ddr_temp = (ddr_temp & PCIE_SENSOR_INVALID_READING) | set_value;
        }

        ret = dal_set_property_value_uint16(slave_obj_handle, PROPERTY_FPGA_CARD_DIMM_TEMP, ddr_temp, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s : Update slave card ddr card temperature failed, ret = %d. ", __FUNCTION__,
                ret);
            return;
        }
    }

    return;
}

void update_power_read_state(OBJ_HANDLE obj_handle, guint16 set_value)
{
    guint16 power = 0;

    if ((obj_handle == 0) || ((set_value != SENSOR_INVALID_READING) && (set_value != SENSOR_NA_READING))) {
        return;
    }

    gint32 ret = dal_get_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_POWER, &power);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get pcie card power failed, ret: %d", __FUNCTION__, ret);
        return;
    }

    if (set_value == (power & set_value)) {
        return;
    }

    if (set_value == SENSOR_INVALID_READING) {
        power = power | set_value;
    } else {
        power = (power & PCIE_SENSOR_INVALID_READING) | set_value;
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_POWER, power, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : update pcie card power failed, ret: %d", __FUNCTION__, ret);
        return;
    }

    return;
}


void info_pcie_card_change(void)
{
    g_atomic_int_inc(&m_update_pcie_card);
}

void notice_pcie_info(PCIE_CARD_GROUP_INFO_S *pcie_card_info, const gchar *class_name, const gchar *method_name)
{
    OBJ_HANDLE obj_handle;
    GSList *input_list = NULL;
    gint32 ret = 0;
    GVariant *vid_did_data = NULL;

    if ((pcie_card_info == NULL) || (NULL == class_name) || (NULL == method_name)) {
        return;
    }

    ret = dal_get_object_handle_nth(class_name, 0, &obj_handle);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "[%s %d]no %s object found.", __FUNCTION__, __LINE__, class_name);
        return;
    }

    vid_did_data = create_gvar_array(pcie_card_info->info, pcie_card_info->count);
    input_list = g_slist_append(input_list, (gpointer)vid_did_data);
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "[%s %d]Memory is not enough", __FUNCTION__, __LINE__);
        g_variant_unref(vid_did_data);
        return;
    }

    ret = dfl_call_class_method(obj_handle, method_name, NULL, input_list, NULL);
    debug_log(DLOG_INFO, "[%s %d]call method %s of class %s, ret is %d", __FUNCTION__, __LINE__, method_name,
        class_name, ret);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return;
}

LOCAL gint32 pcie_send_ipmi_get_info(const INFO_NEED_GET_PCIE_INFO *info, IPMI_REQ_MSG_HEAD_S *header,
    gconstpointer req_data, guint8 *pcie_info)
{
#define READ_REG_DELAY 10000 

    guint8 ret = RET_OK;
    gint32 channel_type = CHANNEL_ME;
    gulong taskdelaytime = READ_REG_DELAY / 1000;
    const guint8 *resp_data_buf = NULL;
    gint8 retry_time = RETRY_TIME;
    gint32 part_index;
    gpointer response = NULL;

    
    guint8 resp_len = 4 + info->length;
    do {
        retry_time--;
        
        ret = card_manage_send_ipmi_request(header, req_data, resp_len, &response, TRUE, channel_type);
        if (ret != RET_OK || response == NULL) {
            debug_log(DLOG_ERROR, "Ipmi send fail(%d)!(chn=%x, retry_time=%x)", ret, channel_type, retry_time);
            vos_task_delay(taskdelaytime);
            continue;
        }
        
        resp_data_buf = get_ipmi_src_data((gconstpointer)response);
        if (resp_data_buf == NULL) {
            g_free(response);
            debug_log(DLOG_ERROR, "[%s]: get_ipmi_src_data failed!", __FUNCTION__);
            return RET_ERR;
        }

        if (resp_data_buf[0] == RET_OK) {
            for (part_index = 0; part_index < info->length; part_index++) {
                pcie_info[part_index] = resp_data_buf[4 + part_index];
            }
            g_free(response);
            response = NULL;
            debug_log(DLOG_DEBUG, "[%s] rsp OK!(chn=%x,retry_time=%x)", __FUNCTION__, channel_type, retry_time);
            return RET_OK;
        } else {
            debug_log(DLOG_ERROR, "Ipmi rsp=%x!(chn=%x, retry=%x)", resp_data_buf[0], channel_type, retry_time);
            response_error_process(resp_data_buf[0], &channel_type, &taskdelaytime, &retry_time);
            g_free(response);
            response = NULL;
        }
        vos_task_delay(taskdelaytime);
    } while (retry_time >= 0);
    return RET_ERR;
}


gint32 pcie_get_info_from_me(const INFO_NEED_GET_PCIE_INFO *info, guint8 *pcie_info)
{
    IPMI_REQ_MSG_HEAD_S req_msg_header;
    PCIE_IPMI_RESQ_DATA req_msg_data;
    guchar arm_enable = 0;
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;
    guint32 product_version = 0;
    guint32 manufactureId;

    
    
    gint32 ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get property failed", __func__);
        return ret;
    }
    

    
    me_access_type = pcie_card_get_me_access_type();
    if (ME_ACCESS_TYPE_SMBUS_PECI == me_access_type) {
        return RET_OK;
    }
    

    
    req_msg_header.target_type = IPMI_ME;
    req_msg_header.target_instance = 0;
    req_msg_header.lun = 0;
    req_msg_header.netfn = NETFN_INTEL_2E;
    req_msg_header.cmd = IPMI_INTEL_GET_CPU_IA_PCIREG;
    req_msg_header.src_len = (guint8)sizeof(PCIE_IPMI_RESQ_DATA);
    
    req_msg_data.intel_id[0] = 0x57;
    req_msg_data.intel_id[1] = 0x01;
    req_msg_data.intel_id[2] = 0x00;

    
    
    
    if (arm_enable == 1) {
        ret = dal_get_product_version_num(&product_version);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get product version num failed", __func__);
            return ret;
        }

        if (product_version <= PRODUCT_VERSION_V5) {
            req_msg_header.target_instance = info->cpu_num & 0x07;
            debug_log(DLOG_ERROR, "%s cpu_id = %d", __func__, req_msg_header.target_instance);
        } else {
            // v6版本的IMU修改为正确的厂商id
            //  注入Huawei OEM 信息
            manufactureId = dal_get_manu_id();
            req_msg_data.intel_id[0] = LONGB0(manufactureId);
            req_msg_data.intel_id[1] = LONGB0(manufactureId);
            req_msg_data.intel_id[2] = LONGB0(manufactureId);
        }
    }

    
    req_msg_data.cpu_num = (info->is_local) ? (0x40 | (info->cpu_num & 0x07)) : (info->cpu_num & 0x07);

    
    req_msg_data.pci_addr[0] = info->address & 0xff;
    req_msg_data.pci_addr[1] = ((info->func & 0x07) << 4) | ((info->dev & 0x01) << 7);
    req_msg_data.pci_addr[2] = ((info->dev >> 1) & 0x0f) | ((info->bus & 0x0f) << 4);
    req_msg_data.pci_addr[3] = info->bus >> 4;

    
    req_msg_data.read_length = (4 == info->length) ? 3 : info->length;

    
    return pcie_send_ipmi_get_info(info, &req_msg_header, &req_msg_data, pcie_info);
}


gint32 pcie_get_by_peci_from_cpu(INFO_NEED_GET_PCIE_INFO *pcie_info, gpointer pcie_id)
{
#define CPU_PCICFG_LEN_4BYTE 3
    guint8 cpu_id = 0;
    guint8 bus_num = 0;
    guint8 device_num = 0;
    guint8 function_num = 0;
    guint8 regoffset = 0;
    guint8 is_local = 0;
    GSList *list = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 read_len = CPU_PCICFG_LEN_4BYTE;
    PCIE_CARD_INFO_S *peci_info_s = (PCIE_CARD_INFO_S *)pcie_id;
    guint8 count = 0;

    if ((pcie_info == NULL) || (NULL == pcie_id)) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    cpu_id = pcie_info->cpu_num;
    bus_num = pcie_info->bus;
    device_num = pcie_info->dev;
    function_num = pcie_info->func;
    regoffset = pcie_info->address;

    gint32 ret = dfl_get_object_list(CLASS_CPU, &list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get Cpu object handle failed!%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    obj_handle = (OBJ_HANDLE)g_slist_nth_data(list, 0);
    g_slist_free(list);

    
    for (count = 0; count < PECI_GET_ID_MAXTRY; count++) {
        output_list = NULL;
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(cpu_id));       // 0 cpuid
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(bus_num));      // 1 //1 bus_num
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(device_num));   // 2 //2 dev_num
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(function_num)); // 3  //3 fuc_num
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(regoffset));    // 4 regoffset
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(is_local));     // 4 //5 is_local
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(read_len));     // 6 read_len
        ret = dfl_call_class_method(obj_handle, METHOD_CPU_GET_PCIE_INFO, NULL, input_list, &output_list);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

        input_list = NULL;

        if (ret != DFL_OK || output_list == NULL) {
            debug_log(DLOG_ERROR, "[%s] failed with result;%d", __FUNCTION__, ret);
            return RET_ERR;
        }

        peci_info_s->vender_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
        peci_info_s->device_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));

        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

        if ((peci_info_s->vender_id != 0) && (0 != peci_info_s->device_id)) {
            break;
        }
    }
    debug_log(DLOG_DEBUG, "[%s] get vid/did use count[%d]", __FUNCTION__, count);
    
    return RET_OK;
}

gint32 _find_node_pcie_card_info(const void *pcie_array_info, guint32 array_info_len, guint16 container_type,
    guint16 container_slot, guint16 pcie_slot, const NODE_PCIE_INFO_S **info_out)
{
    const NODE_PCIE_INFO_S *info_in = NULL;
    guint32 info_cnt;
    guint32 i;

    if (pcie_array_info == NULL || info_out == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    info_cnt = array_info_len / sizeof(NODE_PCIE_INFO_S);
    info_in = (const NODE_PCIE_INFO_S *)pcie_array_info;

    for (i = 0; i < info_cnt; i++) {
        if ((container_type == info_in->container_type) && (container_slot == info_in->container_slot) &&
            (pcie_slot == info_in->pcie_slot)) {
            *info_out = info_in;

            return RET_OK;
        }

        info_in++;
    }

    return RET_ERR;
}

gint32 pcie_card_update_bdf(PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info, guint32 boardid, guint32 auxid,
    guint8 slot_id, guint8 type)
{
    gint32 ret = 0;
    gint8 RetryNum;
    OBJ_HANDLE obj_handle = 0;

    if (slot_id <= 0 || MAX_PCIE_CARD < slot_id) {
        debug_log(DLOG_ERROR, "PcieCard slot id is error!");
        return RET_ERR;
    }

    for (RetryNum = 0; RetryNum < 12; RetryNum++) { // 最多重试12次
        ret = pcie_card_get_object_handle_by_idinfo(boardid, auxid, slot_id, type, &obj_handle);
        if (ret != RET_OK) {
            vos_task_delay(1000); // viddid上报晚于bdf上报，且需要等待xml加载后生成对象再更新，延时1000ms
            debug_log(DLOG_INFO, "Get pcie card object failed! retry times:%d", RetryNum);
        } else {
            break;
        }
    }
    if (RetryNum >= 12) { // 最多重试12次,失败记录日志
        debug_log(DLOG_ERROR, "%s:%d Get pcie card object failed!", __FUNCTION__, __LINE__);
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_BUS_NUM,
        pcie_card_bdf_info->bdf_info[slot_id - 1].bus_num, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card bus num failed!");
        return ret;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_DEV_NUM,
        pcie_card_bdf_info->bdf_info[slot_id - 1].device_num, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card device num failed!");
        return ret;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUN_NUM,
        pcie_card_bdf_info->bdf_info[slot_id - 1].function_num, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card function num failed!");
        return ret;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_SEGMENT,
        pcie_card_bdf_info->bdf_info[slot_id - 1].segment, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card segment num failed!");
        return ret;
    }

    return RET_OK;
}

gint32 set_pcie_addr_info(OBJ_HANDLE pcie_addr_handle, guint8 bus, guint8 me_secbus, guint8 me_subbus)
{
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(bus));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(me_secbus));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(me_subbus));

    gint32 ret = dfl_call_class_method(pcie_addr_handle, METHOD_SET_ROOT_PORT_BUS_INFO, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "set root port bus info failed, ret = %d", ret);
    }

    return ret;
}

gint32 check_firmware_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gchar new_firmware[STR_FIRMWARE_LEN] = {0};
    gchar old_firmware[STR_FIRMWARE_LEN] = {0};
    guint8 buf[KEY_FIRM_VER_LEN] = {0};

    gint32 ret = pm_get_firmware(pm_chip_info, buf, KEY_FIRM_VER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s (%d): Get pcie card firmware failed!", __FUNCTION__, __LINE__);
        return FIRMWARE_UNKNOW;
    }

    // 如果MM板重启，会刷成0.0.0，这里返回错误，重新初始化
    if (buf[0] + buf[1] + buf[2] == 0) {
        return RET_ERR;
    }

    (void)snprintf_s((gchar *)new_firmware, STR_FIRMWARE_LEN, STR_FIRMWARE_LEN - 1, "%x.%x.%x", buf[0], buf[1], buf[2]);

    ret = dal_get_property_value_string(obj_handle, PROPERTY_ACCELERATE_FIRMWARE_VERSION, old_firmware,
        sizeof(old_firmware) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): Get pcie card firmware failed!", __FUNCTION__, __LINE__);
        return FIRMWARE_UNKNOW;
    }

    if (strcmp(new_firmware, old_firmware) != 0) {
        // shell id是和固件版本一致的
        pcie_card_update_shell_id(obj_handle, pm_chip_info);

        ret = dal_set_property_value_string(obj_handle, PROPERTY_ACCELERATE_FIRMWARE_VERSION, new_firmware,
            DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): Set pcie card firmware failed!", __FUNCTION__, __LINE__);
            return FIRMWARE_UNKNOW;
        }
        debug_log(DLOG_ERROR, "%s (%d): firmware change, old version: %s, new version: %s", __FUNCTION__, __LINE__,
            old_firmware, new_firmware);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 pcie_card_get_class_info(const PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info,
    PCIE_CARD_GROUP_CLASS_INFO_S *pcie_cards_class_info)
{
    gint32 bdf_index = 0;
    guint32 pcie_index = 0;
    guint8 slot_id = 0;
    gint32 ret = 0;
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;
    INFO_NEED_GET_PCIE_INFO info;
    PCIE_CARD_CLASS_INFO_S pcie_card_class_info_s = { 0 };
    pcie_cards_class_info->count = 0;

    me_access_type = pcie_card_get_me_access_type();
    if (me_access_type == ME_ACCESS_TYPE_SMBUS_PECI) {
        return RET_ERR;
    }
    (void)memset_s(&info, sizeof(INFO_NEED_GET_PCIE_INFO), 0, sizeof(INFO_NEED_GET_PCIE_INFO));

    for (bdf_index = 0; bdf_index < pcie_card_bdf_info->count; bdf_index++) {
        
        ret = pcie_get_slot_by_index(PCIE_AND_PCIEPLUS_CONNECTOR, bdf_index, &slot_id);
        if (ret != RET_OK) {
            continue;
        }

        
        
        if (slot_id < 1) {
            continue;
        }

        pcie_index = slot_id - 1;

        
        info.cpu_num = pcie_card_bdf_info->bdf_info[bdf_index].cpuid;
        info.bus = pcie_card_bdf_info->bdf_info[bdf_index].bus_num;
        info.dev = pcie_card_bdf_info->bdf_info[bdf_index].device_num;
        info.func = pcie_card_bdf_info->bdf_info[bdf_index].function_num;
        info.is_local = 0;
        info.address = 0x08;
        info.length = 4;

        if ((pcie_card_bdf_info->bdf_info[pcie_index].bus_num == 0xFF &&
            pcie_card_bdf_info->bdf_info[pcie_index].device_num == 0xFF &&
            pcie_card_bdf_info->bdf_info[pcie_index].function_num == 0xFF) ||
            pcie_card_bdf_info->bdf_info[pcie_index].bus_num == 0) {
            pcie_cards_class_info->class_info[pcie_index].class_code = 0xFF;
            pcie_cards_class_info->class_info[pcie_index].sub_class_code = 0xFF;
            pcie_cards_class_info->class_info[pcie_index].prog = 0xFF;
            pcie_cards_class_info->class_info[pcie_index].revision = 0xFF;
            continue;
        }

        ret = pcie_card_update_class_info(pcie_cards_class_info, pcie_index, &info, &pcie_card_class_info_s);
        if (ret == RET_ERR) {
            continue;
        }
        debug_log(DLOG_ERROR, "get class info :pcie_index=%d class=0x%02X subClass=0x%02X", pcie_index,
            pcie_card_class_info_s.class_code, pcie_card_class_info_s.sub_class_code);

        pcie_cards_class_info->class_info[pcie_index].class_code = pcie_card_class_info_s.class_code;
        pcie_cards_class_info->class_info[pcie_index].sub_class_code = pcie_card_class_info_s.sub_class_code;
        pcie_cards_class_info->class_info[pcie_index].prog = pcie_card_class_info_s.prog;
        pcie_cards_class_info->class_info[pcie_index].revision = pcie_card_class_info_s.revision;
        
        if (pcie_card_class_info_s.class_code == 0xFF && pcie_card_class_info_s.sub_class_code == 0xFF &&
            pcie_card_class_info_s.prog == 0xFF && pcie_card_class_info_s.revision == 0xFF) {
            debug_log(DLOG_ERROR, "get class info :pcie_index=%d class=0x%02X subClass=0x%02X", pcie_index,
                pcie_card_class_info_s.class_code, pcie_card_class_info_s.sub_class_code);
            pcie_cards_class_info->class_info[pcie_index].class_code = 0xFE;
            pcie_cards_class_info->class_info[pcie_index].sub_class_code = 0xFE;
            pcie_cards_class_info->class_info[pcie_index].prog = 0xFE;
            pcie_cards_class_info->class_info[pcie_index].revision = 0xFE;
        }
    }
    pcie_cards_class_info->count = pcie_card_bdf_info->count;
    return RET_OK;
}

gint32 card_manage_set_pcie_desc(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    GVariant *card_desc = NULL;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Set pcie card desc failed, incorrect input parameter");
        return RET_ERR;
    }

    card_desc = (GVariant *)g_slist_nth_data(input_list, 0);
    if (card_desc == NULL) {
        return RET_ERR;
    }

    (void)dfl_set_property_value(object_handle, PROPERTY_PCIE_CARD_DESC, card_desc, DF_NONE);

    return RET_OK;
}


gint32 get_pcieaddr_info(OBJ_HANDLE pcieaddr_handle, PcieAddrInfo *info)
{
    if (info == NULL) {
        debug_log(DLOG_ERROR, "PcieAddrInfo is NULL");
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_BDF_NO, &info->bdfno);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s BDFNo failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }
    
    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SOCKET_ID, &info->socket_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s SocketId failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }
    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SEC_BUS, &info->secbus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s SecBus failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }
    ret = dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SUB_BUS, &info->subbus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s SubBus failed, ret = %d.", dfl_get_object_name(pcieaddr_handle), ret);
        return ret;
    }
    return ret;
}

guint8 pcie_card_get_power_state(OBJ_HANDLE obj_handle)
{
    guint8 power_state = PCIE_POWER_ON;
    OBJ_HANDLE referencd_handle = 0;

    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &referencd_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: Get %s refcomponent failed. The error code is %d.", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return PCIE_POWER_ON;
    }

    (void)dal_get_property_value_byte(referencd_handle, PROPERTY_COMPONENT_POWERSTATE, &power_state);

    return power_state;
}

gint32 get_pcie_card_extra_info(OBJ_HANDLE obj_handle, PCIE_CARD_EXTRA_INFO *p_extra_info)
{
    gint32 iRet = -1;
    OBJ_HANDLE refer_handle = 0;
    gchar slave_card_class_name[ACCESSOR_NAME_LEN] = {0};
    guint8 classification = 0;
    gchar firmmare_ver[FIRMWARE_VERSION_STRING_LEN] = {0};
    gchar location_info[FIRMWARE_VERSION_STRING_LEN] = {0};
    guint8 slot_id = 0;

    if (p_extra_info == NULL) {
        return RET_ERR;
    }
    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_BOARD_ID, &p_extra_info->board_id);
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_PCB_VER, p_extra_info->pcb_ver,
        PCIE_CARD_PCB_VERSION_LEN);
    if (ret != RET_OK || strlen(p_extra_info->pcb_ver) == 0 || g_strcmp0("NA", p_extra_info->pcb_ver) == 0) {
        (void)strncpy_s(p_extra_info->pcb_ver, sizeof(p_extra_info->pcb_ver), NA_STR, strlen(NA_STR));
    }
    
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &refer_handle);
    if (ret == DFL_OK) {
        
        (void)dal_get_property_value_byte(refer_handle, PROPERTY_SOFTWARE_CLASSIFICATIONS, &classification);

        
        if ((FW_CLASSIFICATIONS_BACKPLANE_CPLD == classification) || (FW_CLASSIFICATIONS_CPLD == classification)) {
            
            (void)dal_get_property_value_string(refer_handle, PROPERTY_SOFTWARE_LOCATION, location_info,
                FIRMWARE_VERSION_STRING_LEN);
            (void)dal_get_property_value_string(refer_handle, PROPERTY_SOFTWARE_VERSIONSTRING, firmmare_ver,
                FIRMWARE_VERSION_STRING_LEN);

            iRet = snprintf_s(p_extra_info->cpld_ver, FIRMWARE_VERSION_STRING_LEN, FIRMWARE_VERSION_STRING_LEN - 1,
                "(%s)%s", location_info, firmmare_ver);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
        }
    }

    
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strlen(slave_card_class_name) != 0) {
        ret = dal_get_specific_object_byte(slave_card_class_name, PROPERTY_CARD_SLOT, slot_id, &refer_handle);
        if (ret == RET_OK) {
            (void)dal_get_property_value_byte(refer_handle, PROPERTY_ACCELERATE_CARD_BOMID,
                &p_extra_info->sub_card_bom_id);
        }
    }

    return RET_OK;
}

gint32 pcie_card_get_business_port_handle_by_channel(OBJ_HANDLE pcie_handle, guint8 opt_id, OBJ_HANDLE *port_handle)
{
    GSList *business_port_list = NULL;
    GSList *port_node = NULL;
    guint8 chan_num = 0;

    if (port_handle == NULL) {
        debug_log(DLOG_INFO, "%s: NULL pointer", __FUNCTION__);
        return RET_ERR;
    }

    gint32 ret = dal_get_object_list_position(pcie_handle, ETH_CLASS_NAME_BUSY_ETH, &business_port_list);
    // 支持SMBus访问的PCIe网卡不一定配置有BusinessPort对象（如，SDI的扩展网卡），优化该场景日志打印
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        return ret;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get business_port_obj_handle failed, ret %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = RET_ERR;
    for (port_node = business_port_list; port_node; port_node = g_slist_next(port_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)port_node->data, BUSY_ETH_ATTRIBUTE_PORT_NUM, &chan_num);

        if (chan_num == opt_id) {
            *port_handle = (OBJ_HANDLE)port_node->data;
            ret = RET_OK;
            break;
        }
    }

    g_slist_free(business_port_list);

    return ret;
}


gint32 pcie_card_get_optical_module_handle_by_channel(OBJ_HANDLE pcie_handle, guint8 opt_id, OBJ_HANDLE *opt_handle)
{
    GSList *optical_module_list = NULL;
    GSList *opt_node = NULL;
    guint8 chan_num = 0;

    gint32 ret = dal_get_object_list_position(pcie_handle, CLASS_VIRTUAL_OPTICAL_MODULE, &optical_module_list);
    // 光模块不是必定存在的，该代码块会频繁调用，优化该场景日志打印
    if (ret == ERRCODE_OBJECT_NOT_EXIST) {
        return ret;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get optical_module_obj_handle failed, ret %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = RET_ERR;
    for (opt_node = optical_module_list; opt_node; opt_node = g_slist_next(opt_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_CHANNEL_NUM, &chan_num);

        if (chan_num == opt_id) {
            *opt_handle = (OBJ_HANDLE)opt_node->data;
            ret = RET_OK;
            break;
        }
    }

    g_slist_free(optical_module_list);

    return ret;
}

gint32 pcie_card_block_read(const gchar *accessor_name, guint8 *info, guint32 info_size)
{
    OBJ_HANDLE access_handle = 0;
    OBJ_HANDLE chip_handle = 0;
    guint32 offset = 0;
    guint8 size = 0;

    if ((accessor_name == NULL) || (info == NULL)) {
        debug_log(DLOG_ERROR, "Error : Input paramater is NULL .");
        return RET_ERR;
    }

    gint32 ret = dfl_get_object_handle(accessor_name, &access_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get accessor object handle failed : ret = %d .", ret);
        return RET_ERR;
    }

    
    ret = dfl_get_referenced_object(access_handle, "Chip", &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get reference chip object failed : ret = %d .", ret);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_uint32(access_handle, "Offset", &offset);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get accessor offset failed : ret = %d .", ret);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_byte(access_handle, "Size", &size);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get accessor size failed : ret = %d .", ret);
        return RET_ERR;
    }

    if (info_size < size) {
        debug_log(DLOG_ERROR, "Get info_size failed: read_len :%u,chip_len:%u", info_size, size);
        return RET_ERR;
    }

    
    ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), offset, size, (void *)info);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "dfl_chip_blkread, read info failed : name = %s, ret = %d ",
            dfl_get_object_name(chip_handle), ret);
        return RET_ERR;
    }

    return ret;
}

gint32 get_smbios_status_value(guint8 *smbios_status)
{
    OBJ_HANDLE smbios_obj_handle = 0;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &smbios_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s object handle failed, ret = %d", CLASS_NAME_SMBIOS, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(smbios_obj_handle, PROPERTY_SMBIOS_STATUS_VALUE, smbios_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get smbios object %s property failed, ret = %d", PROPERTY_SMBIOS_STATUS_VALUE, ret);
    }

    return ret;
}

void pcie_get_backplane_slot_array(const gchar *class_name, GByteArray **slot_array)
{
    OBJ_HANDLE back_plane_handle = 0;
    GSList *obj_list = NULL;
    GSList *list_item = NULL;
    GVariant *property_value = NULL;
    const guint8 *bdf_data = NULL;
    guint32 bdf_data_len = 0;
    gsize parm_temp = 0;

    if (class_name == NULL || NULL == slot_array) {
        debug_log(DLOG_ERROR, "Failed : parameter is null.");
        return;
    }

    
    gint32 retv = dfl_get_object_list(class_name, &obj_list);
    if (retv == DFL_OK) {
        for (list_item = obj_list; list_item; list_item = g_slist_next(list_item)) {
            back_plane_handle = (OBJ_HANDLE)list_item->data;
            property_value = NULL;
            
            retv = dfl_get_property_value(back_plane_handle, PROPERTY_HDDBACKPLANE_PCIESLOT_SILKARRAY, &property_value);
            if (retv != DFL_OK) {
                debug_log(DLOG_ERROR, "Get HDDBackPlane property PcieSlotSilkArray failed. Ret : %d.", retv);
                continue;
            }

            bdf_data = (const guint8 *)g_variant_get_fixed_array(property_value, &parm_temp, sizeof(guint8));
            bdf_data_len = (guint32)parm_temp;

            
            
            if (bdf_data == NULL || ((1 == bdf_data_len) && (bdf_data[0] == 0xff))) {
                g_variant_unref(property_value);
                continue;
            }

            

            g_byte_array_append(*slot_array, bdf_data, bdf_data_len);
            g_variant_unref(property_value);
        }

        g_slist_free(obj_list);
    } else {
        debug_log(DLOG_DEBUG, "Get HDDBackPlane object handle failed. Ret = %d.", retv);
    }
}


LOCAL void bios_boot_stage_monitor(void)
{
    OBJ_HANDLE bios_obj = 0;
    static guint8 last_boot_stage = 0;
    guint8 boot_stage = 0;

    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get Bios object failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    ret = dal_get_property_value_byte(bios_obj, PROPERTY_BIOS_BOOT_STAGE, &boot_stage);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Get BiosBootStage property failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    if ((boot_stage != last_boot_stage) && (boot_stage == BIOS_BOOT_STAGE_PCIE_INFO_REPORTED)) {
        info_pcie_card_change();
        debug_log(DLOG_DEBUG, "%s: info pcie card changed due to bios boot stage changed to 1", __FUNCTION__);
    }

    last_boot_stage = boot_stage;
}


void pcie_card_scan_update_list_task(void *p)
{
    gint32 state = 0;
    gint32 n = 0;
#define UPDATE_STATE_IDLE 0 // 空闲
#define UPDATE_STATE_WAIT 1 // 等待
#define UPDATE_STATE_WORK 2 // 工作

#define UPDATE_WAIT_TIME 20 // 20s

    (void)prctl(PR_SET_NAME, (gulong) "PcieScanTask");

    while (1) {
        // 检测是否有板卡发生了更换
        card_replace_record_detect();
        // 检测系统启动触发卡带外管理重新初始化
        bios_boot_stage_monitor();
        vos_task_delay(1000);

        switch (state) {
            case UPDATE_STATE_IDLE:
                if (m_update_pcie_card != 0) {
                    state = UPDATE_STATE_WAIT;
                    n = 0;
                    g_atomic_int_set(&m_update_pcie_card, 0); // 标志位归 0
                    debug_log(DLOG_MASS, "%s %d: IDLE -> WAIT", __FUNCTION__, __LINE__);
                }

                break;

            case UPDATE_STATE_WAIT:
                // 如果此时又有对象添加或删除，回到空闲状态，重来
                if (m_update_pcie_card != 0) {
                    state = UPDATE_STATE_IDLE;
                    debug_log(DLOG_MASS, "%s %d: WAIT -> IDLE", __FUNCTION__, __LINE__);
                    break;
                }

                // 稳定期没有新对象加入，开始更新列表
                if ((n++) >= UPDATE_WAIT_TIME) {
                    state = UPDATE_STATE_WORK;
                    debug_log(DLOG_MASS, "%s %d: WAIT -> WORK", __FUNCTION__, __LINE__);
                }

                break;

            case UPDATE_STATE_WORK:
                // 干活
                debug_log(DLOG_MASS, "=========== %s %d: WORK start=========", __FUNCTION__, __LINE__);

                if (pcie_mgnt_init() == RET_OK) {
                    state = UPDATE_STATE_IDLE; // 回归 空闲状态
                }

                debug_log(DLOG_MASS, "=========== %s %d: WORK end  =========", __FUNCTION__, __LINE__);
                break;

            default:
                break;
        }
    }
}

gint32 get_pcie_device_present_status(guint8 pcie_slot, guint8 type, guint8 *present)
{
    OBJ_HANDLE cardaction_obj_handle = 0;
    guint8 present_status = 0x00;

    if (type != DISK_CONNECTOR && type != PCIE_AND_PCIEPLUS_CONNECTOR && type != OCPCARD_CONNECTOR) {
        return RET_ERR;
    }
    if (present == NULL) {
        return RET_ERR;
    }

    *present = 0;
    gint32 ret = pcie_card_get_cardaction_handle(pcie_slot, type, &cardaction_obj_handle);
    if (ret == RET_OK) {
        
        
        ret = dal_get_extern_value_byte(cardaction_obj_handle, PROPERTY_CARD_ACTION_DESTINATION, &present_status,
            DF_AUTO);
        *present = (ret == RET_OK) ? present_status : 0;
    }

    return ret;
}


#pragma pack(1)
typedef struct {
    guint8 seg_num;
    guint8 cpu_num;
    guint8 root_bus;
    guint8 root_device;
    guint8 root_function;
    guint8 bus;
    guint8 device;
    guint8 function;
} PCIE_DEVICE_ENUM_INFO;
#pragma pack()


LOCAL gint32 get_ipmi_msg_req_data(const void *msg_data, const guint8 **req_data_buf, guint8 *req_data_len)
{
    *req_data_buf = get_ipmi_src_data(msg_data);
    if (*req_data_buf == NULL) {
        return RET_ERR;
    }

    *req_data_len = get_ipmi_src_data_len(msg_data);

    return RET_OK;
}


LOCAL gint32 parse_pcie_specific_info_header(const guint8 *req_data_buf, guint8 req_data_len,
    PCIE_DEVICE_ENUM_INFO *pcie_device)
{
#define IPMI_SET_SPECIFIC_INFO_HEADER_SZIE (sizeof(PCIE_DEVICE_ENUM_INFO) + 1) // +1表示一个字节的Parameter Selector
    if (!(req_data_buf != NULL && req_data_len >= IPMI_SET_SPECIFIC_INFO_HEADER_SZIE && pcie_device != NULL)) {
        return RET_ERR;
    }

    if (EOK != memcpy_s(pcie_device, sizeof(PCIE_DEVICE_ENUM_INFO), &req_data_buf[1], sizeof(PCIE_DEVICE_ENUM_INFO))) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 find_pcie_card_with_enum_info(PCIE_DEVICE_ENUM_INFO *pcie_device, OBJ_HANDLE *object_handle)
{
    GSList *obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE tmp_handle;
    guint8 tmp_cpunum;
    guint8 tmp_busnum;
    guint8 tmp_devnum;
    guint8 tmp_funcnum;

    if (object_handle == NULL || pcie_device == NULL) {
        return RET_ERR;
    }

    // 如果请求中的ROOT BDF数据有效则根据ROOT BDF找到PCIeAddrInfo对象, 预留
    if (pcie_device->root_bus != 0xFF && pcie_device->root_device != 0xFF && pcie_device->root_function != 0xFF) {
        ;
    }

    gint32 retval = dfl_get_object_list(CLASS_PCIE_CARD, &obj_list);
    if (retval != DFL_OK || obj_list == NULL) {
        return RET_ERR;
    }

    for (node = obj_list; node != NULL; node = g_slist_next(node)) {
        tmp_handle = (OBJ_HANDLE)node->data;

        tmp_busnum = 0xFF;
        tmp_devnum = 0xFF;
        tmp_funcnum = 0xFF;
        tmp_cpunum = 0xFF;
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIECARD_BUS_NUM, &tmp_busnum);
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIECARD_DEV_NUM, &tmp_devnum);
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIECARD_FUN_NUM, &tmp_funcnum);
        (void)dal_get_property_value_byte(tmp_handle, PROPERTY_PCIECARD_CPU_ID, &tmp_cpunum);

        if (tmp_busnum == pcie_device->bus && pcie_device->bus != 0xFF && tmp_devnum == pcie_device->device &&
            pcie_device->device != 0xFF && tmp_funcnum == pcie_device->function && pcie_device->function != 0xFF) {
            // 如果请求中的CPU号是有效值，则进行匹配
            if (tmp_cpunum != (pcie_device->cpu_num + 1) && // 属性中的ID是从1开始的，因此+1
                pcie_device->cpu_num != 0xFF &&             // 不是PCIe switch
                pcie_device->cpu_num != 0xFE &&             // 不是PCH
                pcie_device->cpu_num != 0xFD) {             // 不是CPU1 AND CPU2
                continue;
            }

            *object_handle = tmp_handle;
            break;
        }
    }

    g_slist_free(obj_list);

    if (node == NULL) { // 循环遍历完，没有匹配
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 find_pcie_card_object_with_param(const guint8 *req_data_buf, guint8 req_data_len,
    OBJ_HANDLE *object_handle)
{
    PCIE_DEVICE_ENUM_INFO pcie_device = { 0 };

    if (req_data_buf == NULL || object_handle == NULL) {
        return RET_ERR;
    }

    // 提取请求的头部
    gint32 retval = parse_pcie_specific_info_header(req_data_buf, req_data_len, &pcie_device);
    if (retval != RET_OK) {
        return RET_ERR;
    }

    // 根据设备BDF找到PCIeCard对象
    return find_pcie_card_with_enum_info(&pcie_device, object_handle);
}


LOCAL gint32 verify_chip_data_length(OBJ_HANDLE object_handle, guint8 val_count, guint8 one_value_size,
    guint32 actual_len)
{
    guint8 chip_number = 0;

    gint32 retval = dal_get_property_value_byte(object_handle, PROPERTY_PCIE_CARD_CHIP_NUMBER, &chip_number);
    if (retval != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s property %s value failed", __FUNCTION__, dfl_get_object_name(object_handle),
            PROPERTY_PCIE_CARD_CHIP_NUMBER);
        return RET_ERR;
    }

    // VALUE个数大于芯片个数不符
    if (val_count > chip_number) {
        debug_log(DLOG_DEBUG, "%s: value count %u exceeds chip number %u", __FUNCTION__, val_count, chip_number);
        return RET_ERR;
    }

    // 与实际VALUE个数不符
    if (val_count * one_value_size != actual_len) {
        debug_log(DLOG_DEBUG, "%s: data length %u invalid", __FUNCTION__, actual_len);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL guint16 __get_chip_temperature(const guint8 *temperature_data, guint8 temperature_data_count)
{
    guint16 max_chip_temp = 0;
    guint16 *chip_temp = NULL;
    guint32 temperature_data_len = sizeof(guint16) * temperature_data_count;

    chip_temp = (guint16 *)g_malloc0(temperature_data_len);
    if (chip_temp == NULL) {
        debug_log(DLOG_DEBUG, "%s: g_malloc0 failed", __FUNCTION__);
        return 0;
    }

    (void)memcpy_s(chip_temp, temperature_data_len, temperature_data, temperature_data_len);

    // 当前处理为取出最高温度值填到属性中，后续如果要独立处理，需要通过其它属性或类承载
    for (guint32 i = 0; i < temperature_data_count; i++) {
        max_chip_temp = (chip_temp[i] > max_chip_temp) ? chip_temp[i] : max_chip_temp;
    }

    g_free(chip_temp);

    return max_chip_temp;
}

LOCAL gboolean is_record_operation_log(guint16 max_chip_temp, guint16 origin_max_chip_temp)
{
#define PCIE_DEVICE_CHIP_TEMP_DIFF 5

    // 上报的是异常值，原来是正常值，则记录日志
    if ((max_chip_temp == PCIE_SENSOR_INVALID_READING || max_chip_temp == PCIE_SENSOR_NA_READING) &&
        ((origin_max_chip_temp & 0xC000) == 0)) {
        return TRUE;
    }

    // 上报的是正常值，原来是异常值，则记录日志
    if ((max_chip_temp != PCIE_SENSOR_INVALID_READING && max_chip_temp != PCIE_SENSOR_NA_READING) &&
        ((origin_max_chip_temp & 0xC000) != 0)) {
        return TRUE;
    }

    // 上报的是正常值，原来也是正常值，两者差值大于5，则记录日志
    guint16 chip_temp_diff;
    if ((max_chip_temp != PCIE_SENSOR_INVALID_READING && max_chip_temp != PCIE_SENSOR_NA_READING) &&
        ((origin_max_chip_temp & 0xC000) == 0)) {
        if (max_chip_temp > origin_max_chip_temp) {
            chip_temp_diff = max_chip_temp - origin_max_chip_temp;
        } else {
            chip_temp_diff = origin_max_chip_temp - max_chip_temp;
        }
        if (chip_temp_diff > PCIE_DEVICE_CHIP_TEMP_DIFF) {
            return TRUE;
        }
    }

    return FALSE;
}

#define PCIE_DEVICE_CHIP_COUNT_OFFSET 10
#define PCIE_DEVICE_CHIP_TEMP_OFFSET 11

gint32 pcie_card_ipmi_set_chip_temp(const void *msg_data, gpointer user_data)
{
    OBJ_HANDLE obj_handle = 0;
    const guint8 *req_data_buf = NULL;
    guint8 req_data_len = 0;
    guint16 origin_max_chip_temp = 0;

    if (msg_data == NULL) {
        return RET_ERR;
    }

    if (get_ipmi_target_type(msg_data) != IPMI_HOST) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    if (get_ipmi_msg_req_data(msg_data, &req_data_buf, &req_data_len) != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (find_pcie_card_object_with_param(req_data_buf, req_data_len, &obj_handle) != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 temp_val_count = req_data_buf[PCIE_DEVICE_CHIP_COUNT_OFFSET];

    gint32 retval = verify_chip_data_length(obj_handle, temp_val_count, sizeof(guint16),
        req_data_len - PCIE_DEVICE_CHIP_TEMP_OFFSET);
    if (retval != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    // 当前处理为取出最高温度值填到属性中，后续如果要独立处理，需要通过其它属性或类承载
    guint16 max_chip_temp =
        __get_chip_temperature((const guint8 *)&req_data_buf[PCIE_DEVICE_CHIP_TEMP_OFFSET], temp_val_count);

    debug_log(DLOG_DEBUG, "%s: PCIe card (%s) max chip temp %u", __FUNCTION__, dfl_get_object_name(obj_handle),
        max_chip_temp);

    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, &origin_max_chip_temp);

    // 对记录操作日志进行抑制处理
    gboolean record_flag = is_record_operation_log(max_chip_temp, origin_max_chip_temp);

    // 简化处理，如果上报的温度有读取失败/不可读，则最高温度为读取失败/不可读
    if (max_chip_temp == PCIE_SENSOR_INVALID_READING) {
        max_chip_temp = origin_max_chip_temp | SENSOR_INVALID_READING;
    } else if (max_chip_temp == PCIE_SENSOR_NA_READING) {
        max_chip_temp = (origin_max_chip_temp & PCIE_SENSOR_INVALID_READING) | SENSOR_NA_READING;
    }

    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, max_chip_temp, DF_NONE);
    guint8 card_slot = 0xFF;
    (void)dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &card_slot);

    if (record_flag == TRUE) {
        if ((max_chip_temp & 0xC000) == 0) {
            ipmi_operation_log(msg_data, "Set the chip temperature of PCIe Card %u to %u degrees Celsius", card_slot,
                max_chip_temp);
        } else {
            ipmi_operation_log(msg_data, "Set the chip temperature of PCIe Card %u to unavailable", card_slot);
        }
    }

    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}

#define PCIE_DEVICE_CHIP_NORMAL 0
#define PCIE_DEVICE_CHIP_FAULT 1

LOCAL guint8 __get_chip_health_state(const guint8 *health_data, guint8 health_data_count)
{
    guint8 chip_fault_state = PCIE_DEVICE_CHIP_NORMAL;
    // 当前处理为只要有芯片出现故障就产生告警，后续如果要独立告警，需要通过其它属性或类承载
    for (guint32 i = 0; i < health_data_count; i++) {
        chip_fault_state = (health_data[i] > chip_fault_state) ? health_data[i] : chip_fault_state;
    }

    return chip_fault_state;
}

LOCAL gint32 __set_chip_health_status(OBJ_HANDLE obj_handle, guint8 chip_fault_state, gboolean *state_changed)
{
    PM_CHIP_INFO *pm_chip_info = NULL;

    // 简化处理，如果上报的多个芯片中有故障，则总体状态为故障并保存在私有数据中，由带外扫描任务结合带外获取的数据后更新到属性
    gint32 retval = pm_get_binded_data(obj_handle, (gpointer *)&pm_chip_info);
    if (retval != DFL_OK || pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s:  invalid arguments: pm_chip_info is NULL or Get binded data failed, ret=%d",
            __FUNCTION__, retval);
        return COMP_CODE_UNKNOWN;
    }

    if (chip_fault_state == PCIE_DEVICE_CHIP_FAULT) {
        // 上报的状态是故障，原来的状态是正常，状态发生反转，记录操作日志
        if ((pm_chip_info->fault_state_from_sms & 0x400000) == 0) {
            *state_changed = TRUE;
        }
        pm_chip_info->fault_state_from_sms |= 0x400000; // BIT22 表示芯片故障
    } else if (chip_fault_state == PCIE_DEVICE_CHIP_NORMAL) {
        // 上报的状态是正常，原来的状态是故障，状态发生反转，记录操作日志
        if ((pm_chip_info->fault_state_from_sms & 0x400000) != 0) {
            *state_changed = TRUE;
        }
        pm_chip_info->fault_state_from_sms &= ~0x400000;
    } else { // 故障状态有非法值
        pm_unref_binded_data(pm_chip_info);
        return COMP_CODE_OUTOF_RANGE;
    }

    pm_unref_binded_data(pm_chip_info);
    return COMP_CODE_SUCCESS;
}

#define PCIE_DEVICE_CHIP_HEALTH_OFFSET 11

gint32 pcie_card_ipmi_set_chip_health_status(const void *msg_data, gpointer user_data)
{
    OBJ_HANDLE obj_handle = 0;
    const guint8 *req_data_buf = NULL;
    guint8 req_data_len = 0;

    if (msg_data == NULL) {
        return RET_ERR;
    }

    TARGET_TYPE_E target_type = get_ipmi_target_type(msg_data);
    if (target_type != IPMI_HOST) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    gint32 retval = get_ipmi_msg_req_data(msg_data, &req_data_buf, &req_data_len);
    if (retval != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    retval = find_pcie_card_object_with_param(req_data_buf, req_data_len, &obj_handle);
    if (retval != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint8 health_val_count = req_data_buf[PCIE_DEVICE_CHIP_COUNT_OFFSET];

    retval = verify_chip_data_length(obj_handle, health_val_count, sizeof(guint8),
        req_data_len - PCIE_DEVICE_CHIP_HEALTH_OFFSET);
    if (retval != RET_OK) {
        ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    // 当前处理为只要有芯片出现故障就产生告警，后续如果要独立告警，需要通过其它属性或类承载
    guint8 chip_fault_state =
        __get_chip_health_state((const guint8 *)&req_data_buf[PCIE_DEVICE_CHIP_HEALTH_OFFSET], health_val_count);

    debug_log(DLOG_DEBUG, "%s: PCIe card (%s) chip health state %u", __FUNCTION__, dfl_get_object_name(obj_handle),
        chip_fault_state);

    gboolean state_changed = FALSE;
    retval = __set_chip_health_status(obj_handle, chip_fault_state, &state_changed);
    if (retval == COMP_CODE_SUCCESS) {
        guint8 card_slot = 0xFF;
        (void)dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &card_slot);
        if (state_changed == TRUE && chip_fault_state == PCIE_DEVICE_CHIP_NORMAL) {
            ipmi_operation_log(msg_data, "Set the chip status of PCIe Card %u to normal", card_slot);
        }
        if (state_changed == TRUE && chip_fault_state == PCIE_DEVICE_CHIP_FAULT) {
            ipmi_operation_log(msg_data, "Set the chip status of PCIe Card %u to faulty", card_slot);
        }
    }
    return ipmi_send_simple_response(msg_data, retval);
}

LOCAL gint32 get_pcie_config_register(INFO_NEED_GET_PCIE_INFO *info, guint32 *value)
{
    gint32 ret;
    guchar me_access_type;

    me_access_type = pcie_card_get_me_access_type();
    if (me_access_type == ME_ACCESS_TYPE_SMBUS_PECI) {
        return RET_OK;
    }

    if (match_v5_partition_product_peci_enable() == TRUE) {
        ret = pcie_get_by_peci_from_cpu(info, value);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get pcie vid/did by by peci failed");
            return RET_ERR;
        }
    } else {
        ret = pcie_get_info_from_me(info, (guint8 *)value);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "get pcie info from me failed with cpu_num:%02x,bus:%02x,dev:%02x,func:%02x",
                info->cpu_num, info->bus, info->dev, info->func);
            return RET_ERR;
        }
    }

    return RET_OK;
}

LOCAL inline guint32 pcie_get_link_width(guint32 val)
{
    return ((val) >> PCIE_LINK_WIDTH_OFFSET) & PCIE_LINK_WIDTH_MASK;
}

LOCAL inline guint32 pcie_get_link_status(guint32 val)
{
    return ((val) >> PCIE_LINK_STATUS_OFFSET) & PCIE_LINK_STATUS_MASK;
}

LOCAL inline guint32 pcie_cap_get_next_point(guint32 val)
{
    return ((val) >> PCIE_CAP_NEXT_POINT_OFFSET) & PCIE_CAP_NEXT_POINT_MASK;
}

LOCAL gint32 pcie_get_capid_addr(INFO_NEED_GET_PCIE_INFO *info, guint8 cap_id, guint8 *addr)
{
#define MAX_REG_NUM 20
    guint32 i;
    guint32 reg_value = 0;
    guint8 next_cap_point;

    
    info->address = PCIE_CAP_ID_OFFSET;
    gint32 ret = get_pcie_config_register(info, &reg_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get first cap id", __FUNCTION__);
        return RET_ERR;
    }

    info->address = reg_value & PCIE_CAP_ID_MASK;
    for (i = 0; i < MAX_REG_NUM; i++) {
        ret = get_pcie_config_register(info, &reg_value);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: failed to get reg address", __FUNCTION__);
            return ret;
        }

        
        if ((reg_value & PCIE_CAP_ID_MASK) != cap_id) {
            next_cap_point = pcie_cap_get_next_point(reg_value);
            if (next_cap_point == 0) {
                debug_log(DLOG_ERROR, "%s: not find cap id(%d)", __FUNCTION__, cap_id);
                return RET_ERR;
            }
            info->address = pcie_cap_get_next_point(reg_value);
            continue;
        }

        *addr = info->address;
        return RET_OK;
    }

    return RET_ERR;
}

LOCAL gint32 pcie_get_lane_info(guint8 slot, guint8 *max_link_width, guint8 *nego_link_width)
{
    INFO_NEED_GET_PCIE_INFO info;
    guint8 addr;
    guint32 value = 0;
    guint16 link_status;

    if (slot < 1 || slot >= MAX_PCIE_CARD) {
        debug_log(DLOG_ERROR, "Invalid slot id %u", slot);
        return RET_ERR;
    }

    info.cpu_num = g_pcie_cards_bdf_info.bdf_info[slot - 1].cpuid;
    info.bus = g_pcie_cards_bdf_info.bdf_info[slot - 1].bus_num;
    info.dev = g_pcie_cards_bdf_info.bdf_info[slot - 1].device_num;
    info.func = g_pcie_cards_bdf_info.bdf_info[slot - 1].function_num;
    info.is_local = 0;
    info.address = 0x0;
    info.length = (guint8)sizeof(value);

    // PCI_CAP_ID_EXP, Link Capabilities Register, 见pci_reg.h, Capability lists
    gint32 ret = pcie_get_capid_addr(&info, PCI_CAP_ID_EXP, &addr);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get cap id addr failed", __FUNCTION__);
        return ret;
    }

    
    info.address = addr + PCIE_LINK_CAP_REG_OFFSET;
    ret = get_pcie_config_register(&info, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get link cap reg value failed", __FUNCTION__);
        return ret;
    }
    *max_link_width = pcie_get_link_width(value);

    info.address = addr + PCIE_LINK_CONTROL_OFFSET;
    ret = get_pcie_config_register(&info, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get link status reg value failed", __FUNCTION__);
        return ret;
    }
    link_status = pcie_get_link_status(value);
    *nego_link_width = pcie_get_link_width(link_status);

    return RET_OK;
}

LOCAL gint32 trans_lans_info(guint8 link_width, gchar *str_link_width, guint32 str_size)
{
    typedef struct {
        guint8 num;
        const gchar *str;
    } LinkWidth;
    gint32 ret;
    guint32 i;
    LinkWidth links[] = {
        {LINK_WIDTH_X1, "X1"},
        {LINK_WIDTH_X2, "X2"},
        {LINK_WIDTH_X4, "X4"},
        {LINK_WIDTH_X8, "X8"},
        {LINK_WIDTH_X12, "X12"},
        {LINK_WIDTH_X16, "X16"}
    };

    for (i = 0; i < sizeof(links) / sizeof(LinkWidth); i++) {
        if (links[i].num != link_width) {
            continue;
        }
        ret = strcpy_s(str_link_width, str_size, links[i].str);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret(%d)", __FUNCTION__, ret);
            return RET_ERR;
        }
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "%s: failed to trans lane info(%d)", __FUNCTION__, link_width);
    return RET_ERR;
}

void update_pcie_lane_info(OBJ_HANDLE obj_handle, guint8 slot_id)
{
#define LINK_WIDTH_SIZE 10
    gchar max_link_str[LINK_WIDTH_SIZE] = {0};
    gchar nego_link_str[LINK_WIDTH_SIZE] = {0};
    guint8 max_link_num;
    guint8 nego_link_num;

    if (obj_handle == 0) {
        return;
    }

    gint32 ret = pcie_get_lane_info(slot_id, &max_link_num, &nego_link_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get lane info failed", __FUNCTION__);
        return;
    }

    ret = trans_lans_info(max_link_num, max_link_str, LINK_WIDTH_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: trans pcie card max_link_width failed", __FUNCTION__);
        return;
    }

    ret = dal_set_property_value_string(obj_handle, PROPETRY_PCIECARD_LINK_WIDTH_ABILITY, max_link_str, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Update pcie card max_link_width failed", __FUNCTION__);
        return;
    }

    ret = trans_lans_info(nego_link_num, nego_link_str, LINK_WIDTH_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: trans pcie card nego_link_width failed", __FUNCTION__);
        return;
    }

    ret = dal_set_property_value_string(obj_handle, PROPETRY_PCIECARD_LINK_WIDTH, nego_link_str, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Update pcie card nego_link_width failed", __FUNCTION__);
        return;
    }

    return;
}
