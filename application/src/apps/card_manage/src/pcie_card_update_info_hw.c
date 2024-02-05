

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"





// 获取在位信息（Opcode：0x1020）应答数据的ID编号:
typedef enum {
    DEV_PRESENCE_SLIMLINE_ID = 1, // 1 - Slimline
    DEV_PRESENCE_NCSI_ID, // 2 - NCSI
    DEV_PRESENCE_SSD1_ID, // 3 - SSD1
    DEV_PRESENCE_SSD2_ID, // 4 - SSD2
    DEV_PRESENCE_SFP1_ID, // 5 - SFP1
    DEV_PRESENCE_SFP2_ID, // 6 - SFP2
    DEV_PRESENCE_SFP3_ID, // 7 - SFP3
    DEV_PRESENCE_SFP4_ID, // 8 - SFP4
} DEV_PRESENCE_ID_E;

// M2Presence M.2在位状态的的bit位
typedef enum {
    M2_PRESENCE_SSD1_BIT = 0,
    M2_PRESENCE_SSD2_BIT
} DEV_M2_PRESENCE_BIT_E;

#define SET_M2_PRESENCE(data, m2_presence, bit) \
    do {    \
        if ((data) == 1) {    \
            (m2_presence) |= (1 << (bit));   \
        } else {    \
            (m2_presence) &= (~(1 << (bit)));    \
        }   \
    } while (0)

#define IP_ADDR_MAX_LEN 10

// 通过stdsmbus更新sdi卡信息函数
typedef void (*update_pcie_card_info_by_std_smbus_func)(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO *pm_chip_info);

LOCAL void pcie_card_update_sdi_mcu_ipv4(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info);
LOCAL void pcie_card_get_ipv4addr_from_mcu(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info);
LOCAL void pcie_card_get_bootorder_from_mcu(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info);
LOCAL void pcie_card_update_m2_presence_from_mcu(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info);
LOCAL void pcie_card_get_osstatus_from_mcu(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info);
LOCAL void pcie_card_get_reset_linkage_from_mcu(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info);

update_pcie_card_info_by_std_smbus_func g_update_sdi_info_func[] = {
    pcie_card_update_sdi_mcu_ipv4,          // SDI5X 设置 bmc ip到mcu
    pcie_card_get_ipv4addr_from_mcu,        // SDI5X支持,更新sdi卡的ipv4addr和vlan
    pcie_card_get_bootorder_from_mcu,       // 更新sdi卡的启动顺序
    pcie_card_update_m2_presence_from_mcu,  // 更新M.2在位状态
    pcie_card_get_osstatus_from_mcu,        // 更新SDI5X os的运行状态
    pcie_card_get_reset_linkage_from_mcu    // 更新SDI卡复位联动策略
};


LOCAL gint32 pcie_card_get_firmware_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *chip_info)
{
    guint32 ret = 0;
    guint8 major_num = 0;
    guint8 minor_num = 0;
    guint8 buf[RESP_DATA_LENGTH_VERSION] = {0};
    OBJ_HANDLE firmware_obj_handle = 0;
    gchar version_str[STR_FIRMWARE_LEN + 1] = {0};
    GSList *input_list = NULL;

    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_FIRMWARE_OBJ, &firmware_obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s (%d): Get firmware_obj_class_name failed!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = pm_get_firmware(chip_info, buf, RESP_DATA_LENGTH_VERSION);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): pm_get_firmware failed! ret=0x%4x", __FUNCTION__, __LINE__, ret);
        return ret;
    }
    
    major_num = buf[1] / 10;
    minor_num = buf[1] % 10;

    ret = snprintf_s(version_str, STR_FIRMWARE_LEN + 1, STR_FIRMWARE_LEN, "%u.%u.%u.%u", buf[0], major_num, minor_num,
        buf[2]);
    
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s (%d): snprintf_s failed!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(version_str));
    ret = dfl_call_class_method(firmware_obj_handle, METHOD_SOFTWARE_SET_FRMWARE_VERSION, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:call METHOD_SOFTWARE_SET_FRMWARE_VERSION fail, ret %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 pcie_mgnt_init_info_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = 0;
    const gchar *ptr_obj_name = NULL;
    OBJ_HANDLE ref_handle;
    gchar class_name [ MAX_NAME_SIZE ] = {0};

    // 获取句柄
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REFCHIP, &ref_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get chip name failed, ret=%d!", ret);
        return RET_ERR;
    }
    // 获取chip name
    ptr_obj_name = dfl_get_object_name(ref_handle);
    if (ptr_obj_name == NULL) {
        debug_log(DLOG_ERROR, "Failed to get object name.");
        return RET_ERR;
    }
    (void)strncpy_s(pm_chip_info->chip_name, sizeof(pm_chip_info->chip_name), ptr_obj_name,
        sizeof(pm_chip_info->chip_name) - 1);

    // 获取单帧长度
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_MAXFRAMELEN, &pm_chip_info->framelen);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get pcie card frame len failed!");
        return ret;
    }

    ret = pm_init(pm_chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): pm_init failed! ret = %d", __FUNCTION__, __LINE__, ret);
        return RET_ERR;
    }

    if (dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE) != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to call dfl_get_class_name by obj [%s]", dfl_get_object_name(obj_handle));
        return RET_ERR;
    }
    if (g_strcmp0(CLASS_MEZZCARD_NAME, class_name) != 0) {
        // 获取固件版本
        ret = pcie_card_get_firmware_by_smbus(obj_handle, pm_chip_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s (%d): pcie_card_get_firmware_by_smbus failed! ret = %d", __FUNCTION__, __LINE__,
                ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}

LOCAL guint8 check_optical_module_temp(guint16 *temp)
{
    guint8 flag = 0;
    guint16 optical_module_temp = *temp;

    // 光模块不在位，温度值设为0
    if ((optical_module_temp == OPTICAL_MODULE_TEMP_ABSENT) || (optical_module_temp == OPTICAL_MODULE_TEMP_PAD)) {
        optical_module_temp = 0;
    } else if (optical_module_temp == OPTICAL_MODULE_TEMP_READ_FAIL) { // 光模块温度读取失败，最高位置1
        optical_module_temp = SENSOR_INVALID_READING;
    } else {
        
        flag = NORMAL_OPT_TEMP;
        optical_module_temp = optical_module_temp & 0xff;
    }

    *temp = optical_module_temp;
    return flag;
}

void pcie_card_update_optical_module_temp_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define OPTICAL_MODULE_TEMP_LENGTH 2

#define OPTICAL_MODULE_MAX_NUMBER 16
    
    gint32 ret = RET_OK;
    guint16 *optical_module_temp_raw = NULL;
    guint32 optical_module_temp_total_len = 0;
    guint16 optical_module_temp = 0;
    OBJ_HANDLE optical_module_obj_handle = 0;
    guint8 channal_index = 0;
    GSList *input_list = NULL;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }

    
    if ((pcie_card_mgnt_ready() != TRUE) && !(is_ocp3_card(obj_handle))) {
        return;
    }
    ret = pm_get_optical_module_temp(pm_chip_info, (guint8 **)&optical_module_temp_raw, &optical_module_temp_total_len);
    // 获取温度失败
    if ((ret != RET_OK) || (optical_module_temp_raw == NULL)) {
        // 如果网卡固件版本不支持获取光模块温度，不进行告警
        if (ret == PM_ERROR_CODE_NIC_OP_TEMP_UNSUPPORTED) {
            debug_log(DLOG_INFO, "Get optical module temp unsupported, ret = 0x%4x", ret);
            return;
        }

        // 如果获取失败，将所有温度值最高位置1
        debug_log(DLOG_DEBUG, "pm_get_optical_module_temp failed, ret = %d", ret);

        for (channal_index = 0; channal_index < optical_module_temp_total_len / OPTICAL_MODULE_TEMP_LENGTH;
            channal_index++) {
            
            // 防止channal_index越界导致死循环
            if (channal_index >= OPTICAL_MODULE_MAX_NUMBER) {
                break;
            }
            
            optical_module_temp = SENSOR_INVALID_READING;
            ret = pcie_card_get_optical_module_handle_by_channel(obj_handle, channal_index + 1,
                &optical_module_obj_handle);
            // 光模块不是必定存在的，该代码块会频繁调用，优化该场景日志打印
            if (ret == ERRCODE_OBJECT_NOT_EXIST) {
                continue;
            }

            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "get optical module handle by channel%d fail.", channal_index + 1);
                continue;
            }
            input_list = g_slist_append(input_list, g_variant_new_double((gdouble)optical_module_temp));
            ret = dfl_call_class_method(optical_module_obj_handle, METHOD_OPT_MDL_SET_OPTICAL_MODULE_TEMP, NULL,
                input_list, NULL);
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
            input_list = NULL;

            if (ret != DFL_OK) {
                debug_log(DLOG_DEBUG, "%s:Set channel%d optical module temp fail, ret %d.", __FUNCTION__, channal_index,
                    ret);
                continue;
            }
        }

        if (optical_module_temp_raw != NULL) {
            g_free(optical_module_temp_raw);
        }

        return;
    }

    // 获取温度成功
    for (channal_index = 0; channal_index < optical_module_temp_total_len / OPTICAL_MODULE_TEMP_LENGTH;
        channal_index++) {
        
        // 防止channal_index越界导致死循环
        if (channal_index >= OPTICAL_MODULE_MAX_NUMBER) {
            break;
        }
        
        optical_module_temp = optical_module_temp_raw[channal_index];

        ret = pcie_card_get_optical_module_handle_by_channel(obj_handle, channal_index + 1, &optical_module_obj_handle);
        // 光模块不是必定存在的，该代码块会频繁调用，优化该场景日志打印
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            continue;
        }
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "get optical module handle by channel%d fail.", channal_index + 1);
            continue;
        }

        debug_log(DLOG_DEBUG, "optical_module_temp[%d] is %d", channal_index, optical_module_temp);
        if (check_optical_module_temp(&optical_module_temp) == NORMAL_OPT_TEMP) {
            
            input_list = g_slist_append(input_list, g_variant_new_double((gdouble)(gint8)optical_module_temp));
        } else {
            input_list = g_slist_append(input_list, g_variant_new_double((gdouble)optical_module_temp));
        }
        ret = dfl_call_class_method(optical_module_obj_handle, METHOD_OPT_MDL_SET_OPTICAL_MODULE_TEMP, NULL, input_list,
            NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;

        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s:Set channel%d optical module temp fail, ret %d.", __FUNCTION__, channal_index,
                ret);
            continue;
        }
    }

    g_free(optical_module_temp_raw);
}

void pcie_card_update_temperature_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = RET_OK;
    guint16 temp_value = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }

    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, &temp_value);

    
    ret = check_is_atlas_smm();
    
    if ((pcie_card_mgnt_ready() == TRUE) || (ret == RET_OK) || is_ocp3_card(obj_handle)) {
        
        ret = pm_get_temp(pm_chip_info, (guint8 *)&temp_value, sizeof(guint16));
        if (ret != RET_OK) {
            temp_value = temp_value | 0x8000;
            debug_log(DLOG_DEBUG, "%s failed, temp_value=0x%04x, ret=0x%4x", __FUNCTION__, temp_value, ret);
        }
    } else {
        temp_value = (temp_value & PCIE_SENSOR_INVALID_READING) | 0x4000; // 先清除最高位再置次高位为1
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, temp_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set prop ChipTemp failed, ret=%d", __FUNCTION__, ret);
    }
}

LOCAL guint32 __calc_power_read_fail_duration(PM_CHIP_INFO *pm_chip_info)
{
    guint32 duration = 0;
    guint32 cur_timestamp = (guint32)vos_tick_get();

    if (pm_chip_info->power_read_fail_timestamp == 0) {
        pm_chip_info->power_read_fail_timestamp = cur_timestamp;
    } else if (pm_chip_info->power_read_fail_timestamp <= cur_timestamp) {
        duration = cur_timestamp - pm_chip_info->power_read_fail_timestamp;
    } else {
        duration = cur_timestamp + (G_MAXUINT32 - pm_chip_info->power_read_fail_timestamp);
    }

    return duration;
}


LOCAL void pcie_card_update_power_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = RET_OK;
    guint16 power_value = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input parameter pm_chip_info is NULL", __FUNCTION__);
        return;
    }

    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_POWER, &power_value);

    
    if (pcie_card_mgnt_ready() == TRUE) {
        (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
        head_para.opcode = REGISTER_KEY_POWER;
        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&power_value,
            sizeof(guint16));
        debug_log(DLOG_DEBUG, "%s: update pcie card power, power_value is %u", __FUNCTION__, power_value);

        if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
            return;
        } else if (ret == RET_OK) {
            pm_chip_info->power_read_fail_timestamp = 0;
            // 处理从MCU读取功耗异常值:0x7fff、0x7ffd，具体见PCIE带外管理规范
            if (power_value == PCIE_SENSOR_NA_READING || power_value == PCIE_SENSOR_INVALID_READING) {
                power_value = SENSOR_NA_READING;
            }
        } else {
            guint32 duration = __calc_power_read_fail_duration(pm_chip_info);
            if (duration > ((guint32)pm_chip_info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
                if ((power_value & SENSOR_INVALID_READING) == SENSOR_INVALID_READING) {
                    return;
                }
                power_value = SENSOR_INVALID_READING;
                debug_log(DLOG_DEBUG, "%s : get power over SMBus failed, return %d", __FUNCTION__, ret);
            }
        }
    } else {
        power_value = SENSOR_NA_READING; // 表示数据无法读取
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_FPGA_CARD_POWER, power_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: update pcie card power failed, ret=%d", __FUNCTION__, ret);
        return;
    }
}

LOCAL gint32 analysis_mac_info(const guint8 *buf, guint8 *port_num, guint16 *pf, gchar *mac_addr, guint32 mac_len)
{
    *port_num = buf[0];
    *pf = ((buf[1]) << 8) | buf[2];
    if ((*pf & 0x8000) != 0) {
        debug_log(DLOG_DEBUG, "%s : pf = 0x%x", __FUNCTION__, *pf);
        return RET_ERR;
    }
    gint32 ret = snprintf_s(mac_addr, mac_len, mac_len - 1, "%02X:%02X:%02X:%02X:%02X:%02X",
        buf[3], buf[4], buf[5], buf[6], buf[7], buf[8]);
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "%s: snprintf_s fail, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }
    if (g_strcmp0(MAC_ADDR_DEFAULT, mac_addr) == 0) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void pcie_card_update_netcard_mac(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    OBJ_HANDLE extend_handle = 0;
    PROTOCOL_HEAD_PARA head_para = {0};
    gchar mac_addr[MAX_MAC_STR_LEN + 1] = {0};
    guint8 buf[NETCARD_MAC_ADDRESS_RESPONSE_LENGTH * BUSSINESS_PORT_PFINFO_MAX_CNT + 1] = {0};
    PM_CHIP_INFO *netcard_chip_info = NULL;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter is NULL", __FUNCTION__);
        return;
    }
    gint32 ret = dal_get_specific_object_string(CLASS_EXTEND_PCIECARD_NAME, PROPETRY_EXTEND_PCIECARD_BASECARD,
        dfl_get_object_name(obj_handle), &extend_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "dal_get_specific_object_string %s error", dfl_get_object_name(obj_handle));
        return;
    }
    if (pcie_card_mgnt_ready() != TRUE) {
        return;
    }
    ret = dfl_get_binded_data(extend_handle, (gpointer*)&netcard_chip_info);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "get %s binded data fail, ret: %d", dfl_get_object_name(extend_handle), ret);
        return;
    }
    head_para.opcode = REGISTER_KEY_SDI_GET_MAC;
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, sizeof(buf));
    
    if (ret != RET_OK && ret != PM_ERROR_CODE_REQUEST_LEN_ERROR) {
        debug_log(DLOG_DEBUG, "%s get %d failed, ret = %d", dfl_get_object_name(extend_handle), head_para.opcode, ret);
        return;
    }

    for (gint32 offset = 0; offset <= sizeof(buf) - NETCARD_MAC_ADDRESS_RESPONSE_LENGTH;
        offset = offset + NETCARD_MAC_ADDRESS_RESPONSE_LENGTH) {
        guint8 port_num;
        guint16 pf;
        ret = analysis_mac_info(buf + offset, &port_num, &pf, mac_addr, sizeof(mac_addr));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "analysis_mac_info error ret:%d", ret);
            continue;
        }
        ret = pcie_card_set_pfinfo(extend_handle, netcard_chip_info, pf, port_num, (gchar *)&mac_addr);
        debug_log(DLOG_MASS, "port_num %d pf:vf %d mac:%s", port_num, pf, mac_addr);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "ppcie_card_set_pfinfo error ret:%d", ret);
        }
    }
    return;
}


LOCAL gboolean is_bmc_ip_changed(PM_CHIP_INFO* pm_chip_info, const guint32 ip_digit)
{
    guint8 ipv4_addr_digit[IP_ADDR_MAX_LEN + 1] = {0};
    PROTOCOL_HEAD_PARA head_para = {0};

    head_para.opcode = REGISTER_KEY_GET_IP;
    head_para.input_data_len = 1;
    head_para.input_data[0] = 0;  // 0 --BMC, 1 --卡上os
    gint32 retv = pm_get_fixed_len_data_by_std_smbus(pm_chip_info,
        (guint8 *)&head_para, ipv4_addr_digit, IP_ADDR_MAX_LEN);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR,
            "%s: get mcu bmc ippm_get_fixed_len_data_by_std_smbus failed, retv=%d", __FUNCTION__, retv);
        return TRUE;
    }

    guint32 ip_digit_temp = (guint32)MAKE_DWORD(ipv4_addr_digit[3],
        ipv4_addr_digit[2], ipv4_addr_digit[1], ipv4_addr_digit[0]);
    
    if (ip_digit_temp == ip_digit) {
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 set_bmc_ipv4_to_mcu(PM_CHIP_INFO* pm_chip_info, const guint32 ip_digit,
    const guint32 mask_digit, const guint16 cur_vlan_id)
{
    guint8 ipv4_addr_digit[IP_ADDR_MAX_LEN + 1] = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    guint32 req_data_len = 11; // 请求数据长度 0x0b

    ipv4_addr_digit[0] = 0; // 设置BMC
    ipv4_addr_digit[1] = LONGB0(ip_digit);
    ipv4_addr_digit[2] = LONGB1(ip_digit);
    ipv4_addr_digit[3] = LONGB2(ip_digit);
    ipv4_addr_digit[4] = LONGB3(ip_digit);

    ipv4_addr_digit[5] = LONGB0(mask_digit);
    ipv4_addr_digit[6] = LONGB1(mask_digit);
    ipv4_addr_digit[7] = LONGB2(mask_digit);
    ipv4_addr_digit[8] = LONGB3(mask_digit);

    ipv4_addr_digit[9] = LONGB0(cur_vlan_id);
    ipv4_addr_digit[10] = LONGB1(cur_vlan_id);
    
    head_para.opcode = REGISTER_KEY_SET_IP;
    gint32 retv = pm_send_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, ipv4_addr_digit, req_data_len);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set data to mcu failed, retv is %d", __FUNCTION__, retv);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void pcie_card_update_sdi_mcu_ipv4(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info)
{
#define IPV4_ADDR_LEN 16

    guint32 ip_digit = 0;
    guint32 mask_digit = 0;
    guint16 cur_vlan_id = 0;
    gchar ipv4_addr[IPV4_ADDR_LEN] = {0};
    gchar ipv4_mask[IPV4_IP_STR_SIZE + 1] = {0};
    OBJ_HANDLE group_obj = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: Input parameter pm_chip_info is NULL.", __FUNCTION__);
        return;
    }

    
    gint32 retv = dfl_get_object_handle(OUT_ETHGROUP_OBJ_NAME, &group_obj);
    if (retv != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_get_object_handle(OUT_ETHGROUP_OBJ_NAME) failed, ret=%d", __FUNCTION__, retv);
        return;
    }

    
    retv = dal_get_property_value_uint16(group_obj, ETH_GROUP_ATTRIBUTE_VLAN_ID, &cur_vlan_id);
    if (retv != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get bmc VlanID failed, retv=%d", __FUNCTION__, retv);
        return;
    }
    retv = dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_SUB_MASK, ipv4_mask, sizeof(ipv4_mask));
    if (retv != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get bmc SubnetMask failed, retv=%d", __FUNCTION__, retv);
        return;
    }
    retv = dal_get_property_value_string(group_obj, ETH_GROUP_ATTRIBUTE_IP_ADDR, ipv4_addr, IPV4_ADDR_LEN);
    if (retv != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get bmc ipv4 failed, retv=%d", __FUNCTION__, retv);
        return;
    }

    (void)inet_pton(AF_INET, ipv4_addr, &ip_digit);
    (void)inet_pton(AF_INET, ipv4_mask, &mask_digit);
   
    if (is_bmc_ip_changed(pm_chip_info, ip_digit) != TRUE) {
        debug_log(DLOG_DEBUG, "%s: ip has not changed, no need to update", __FUNCTION__);
        return;
    }
    
    retv = set_bmc_ipv4_to_mcu(pm_chip_info, ip_digit, mask_digit, cur_vlan_id);
    if (retv != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: set data to mcu failed, retv is %d", __FUNCTION__, retv);
        return;
    }

    return;
}

LOCAL void pcie_card_get_ipv4addr_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 buf[IP_ADDR_MAX_LEN + 1] = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    guint32 ipaddr = 0;
    guint16 vlan = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }
    gint32 ret = dal_get_extern_value_uint32(obj_handle, PROPERTY_SDI_CARD_STORAGE_IP_ADDR, &ipaddr, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    ret = dal_get_extern_value_uint16(obj_handle, PROPERTY_SDI_CARD_STORAGE_VLAN, &vlan, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    head_para.opcode = REGISTER_KEY_GET_IP;
    head_para.input_data_len = 1;
    head_para.input_data[0] = 1;  // Request data  0 --BMC, 1 --卡上os
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, IP_ADDR_MAX_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get sdi card ipv4addr and vlan failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    guint32 ipaddr_temp = (guint32)MAKE_DWORD(buf[3], buf[2], buf[1], buf[0]);
    guint16 vlan_temp = (guint16)MAKE_WORD(buf[9], buf[8]);
    if ((ipaddr == ipaddr_temp) && (vlan == vlan_temp)) {
        return;
    }
    
    ret = dal_set_property_value_uint32(obj_handle, PROPERTY_SDI_CARD_STORAGE_IP_ADDR, ipaddr_temp, DF_SAVE);
    ret += dal_set_property_value_uint16(obj_handle, PROPERTY_SDI_CARD_STORAGE_VLAN, vlan_temp, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: update sdi card ipv4addr and vlan failed", __FUNCTION__);
        return;
    }
    return;
}

LOCAL guint8 transform_boot_order_value(guint8 boot_order)
{
    guint8 new_boot_order = 0;
    switch (boot_order) {
        case 0:
            new_boot_order = 255;
            break;
        case 1:
            new_boot_order = 1;
            break;
        case 2:
            new_boot_order = 0;
            break;
    }
    return new_boot_order;
}

LOCAL void pcie_card_get_bootorder_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 buf[KEY_BOOTORDER_LEN + 1] = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    guint8 bootorder = 0;
    guint8 valid_type = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }
    gint32 ret = dal_get_extern_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_ORDER, &bootorder, DF_AUTO);
    ret += dal_get_extern_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, &valid_type, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    head_para.opcode = REGISTER_KEY_GET_BOOT_ORDER;
    head_para.input_data_len = 2;
    head_para.input_data[0] = 1;  // Request data--- data[0]:配置固化选项
    head_para.input_data[1] = 2;  // Request data--- data[1]:BIOS引导OS启动方式
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_BOOTORDER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get sdi card boot order failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    guint8 bootorder_temp =  transform_boot_order_value(buf[3]);
    if (bootorder_temp == bootorder && buf[1] == valid_type) {
        return;
    }
    // 配置固化选项
    if (buf[0] == 1) {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, buf[1], DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: update sdi card boot order failed", __FUNCTION__);
            return;
        }
    }
    // BIOS引导OS启动方式
    if (buf[2] == 2) {
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_SDI_CARD_BOOT_ORDER, bootorder_temp, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: update sdi card boot order failed", __FUNCTION__);
            return;
        }
    }
    return;
}

LOCAL gint32 pcie_card_get_presence_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info,
    guint8 **buf, guint32 *buf_len)
{
    PROTOCOL_HEAD_PARA head_para = {0};
    head_para.opcode = REGISTER_KEY_GET_PRESENCE;
    gint32 ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, buf_len);
    if (ret != RET_OK || buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : getting pcie card presence from mcu is failed, ret = %d.", __FUNCTION__, ret);
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void pcie_card_update_m2_presence_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint32 buf_len = 0;
    guint8 *buf = NULL;
    guint8 prev_m2_presence = 0;

    gint32 ret = dal_get_extern_value_byte(obj_handle, PROPERTY_SDI_CARD_M2, &prev_m2_presence, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : getting %s.%s failed, ret = %d.",
            __FUNCTION__, dfl_get_object_name(obj_handle), PROPERTY_SDI_CARD_M2, ret);
        return;
    }
    
    ret = pcie_card_get_presence_from_mcu(obj_handle, pm_chip_info, &buf, &buf_len);
    if (ret != RET_OK) {
        return;
    }

    guint8 m2_presence = prev_m2_presence;
    
    for (gint8 i = 1; i < buf_len; i += 2) {
        if (buf[i - 1] == DEV_PRESENCE_SSD1_ID) {
            SET_M2_PRESENCE(buf[i], m2_presence, M2_PRESENCE_SSD1_BIT);
        } else if (buf[i - 1] == DEV_PRESENCE_SSD2_ID) {
            SET_M2_PRESENCE(buf[i], m2_presence, M2_PRESENCE_SSD2_BIT);
        }
    }
    g_free(buf);
    if (m2_presence != prev_m2_presence) { // 与原来相同,不更新
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_SDI_CARD_M2, m2_presence, DF_SAVE);
    }
    return;
}

LOCAL void pcie_card_get_osstatus_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define KEY_OSSTATUS_TYPE   6

    guint8 buf[KEY_OSSTATUS_LEN + 1] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint8 status = 0;

    gint32 ret = dal_get_extern_value_byte(obj_handle, PROPERTY_SDI_CARD_SYSTEM_STATUS, &status, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    head_para.opcode = REGISTER_KEY_GET_DEVICE_STATUS;
    head_para.input_data_len = 1;
    head_para.input_data[0] = KEY_OSSTATUS_TYPE;  // Request data Type为6表示获取os的运行状态
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_OSSTATUS_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get SDI card os status failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    debug_log(DLOG_DEBUG, "%s : get SDI card os status successful, buf[0]=%d, buf[1]=%d",
        __FUNCTION__, buf[0], buf[1]);

    // type不为os运行状态或者运行状态与之前状态相同则直接return
    if (buf[0] != KEY_OSSTATUS_TYPE || buf[1] == status) {
        return;
    }
    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_SDI_CARD_SYSTEM_STATUS, buf[1], DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: update SDI card os status failed", __FUNCTION__);
        return;
    }
    return;
}


LOCAL void pcie_card_get_reset_linkage_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 buf[KEY_SDI_RESET_LINKAGE_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint8 reset_linkage = 0;

    gint32 ret = dal_get_extern_value_byte(obj_handle, PROPERTY_SDI_RESET_LINKAGE_POLICY, &reset_linkage, DF_AUTO);
    if (ret != RET_OK) {
        return;
    }

    head_para.opcode = REGISTER_KEY_GET_RESET_LINKAGE_POLICY;
    head_para.input_data_len = 1;
    head_para.input_data[0] = 0;  // 获取复位联动策略不使用Request parameter
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_SDI_RESET_LINKAGE_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get SDI card reset linkage failed, ret = %d", __FUNCTION__, ret);
        return;
    }
    debug_log(DLOG_DEBUG, "%s : get SDI card reset linkage successful, buf[0]=%d", __FUNCTION__, buf[0]);

    // 复位联动策略与之前相同则直接return
    if (buf[0] == reset_linkage) {
        return;
    }
    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_SDI_RESET_LINKAGE_POLICY, buf[0], DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: update SDI card reset linkage failed", __FUNCTION__);
        return;
    }
    return;
}


LOCAL void __do_pcie_card_info_update_by_std_smbus(OBJ_HANDLE obj_handle, OBJ_HANDLE slave_obj_handle,
    PM_CHIP_INFO *pm_chip_info)
{
    guint8 mcu_support = 0;
    guint32 i;
    // MCU RTC时间更新
    pcie_card_update_mcu_rtc_time(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // PCIE卡的FW版本更新
    pcie_card_update_firmware_version(slave_obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // PCIE卡的MAC的更新
    pcie_card_update_netcard_mac(obj_handle, pm_chip_info);  // SDI5X支持
    PCIE_UPDATE_RELEX_CPU();
    // 更新PCIE卡的功耗
    pcie_card_update_power_by_smbus(slave_obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // 更新PCIE卡的温度
    pcie_card_update_temperature(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    
    pcie_card_update_ecc_count(obj_handle, slave_obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    
    // 更新PCIe卡上的温度传感器数据
    pcie_card_update_temperature_sensors(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    
    // 更新PCIE卡的内存条温度
    pcie_card_update_dimm_temp_by_smbus(obj_handle, pm_chip_info);
    
    PCIE_UPDATE_RELEX_CPU();
    pcie_card_update_memory_capacity(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // PCIE卡故障码更新
    pcie_card_update_fault_code_by_std_smbus(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    
    // 监测光模块在位状态并更新光模块信息
    pcie_card_check_optical_present_changed(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // 更新光模块的温度、电压、功率以及偏置电流
    pcie_card_update_optical_temp_power(obj_handle, pm_chip_info);
    
    // 更新npu卡温度
    PCIE_UPDATE_RELEX_CPU();
    pcie_card_update_npu_reading_celsius(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // 更新npu卡功耗
    pcie_card_update_npu_power_consunmed_watts(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // 更新npu卡版本号
    pcie_card_update_npu_fireware_version(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // 更新npu卡SN码
    pcie_card_update_npu_sn(obj_handle, pm_chip_info);
    PCIE_UPDATE_RELEX_CPU();
    // PCIE卡日志收集
    (void)dal_get_property_value_byte(slave_obj_handle, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 1 && pm_chip_info->card_type == SDI_CARD) {
        for (i = 0; i < G_N_ELEMENTS(g_update_sdi_info_func); i++) {
            g_update_sdi_info_func[i](slave_obj_handle, pm_chip_info);
            PCIE_UPDATE_RELEX_CPU();
        }
    }
    pcie_card_collect_log(obj_handle, pm_chip_info);
}


void update_pcie_card_info_by_std_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gint32 ret = 0;
    gchar slave_card_class_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE slave_obj_handle = 0;
    gchar card_desc[MAX_PCIE_CARD_DESC_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: parameter is null", __FUNCTION__);
        return;
    }

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_DESC, card_desc, sizeof(card_desc));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s (%d): get card description failed!", __FUNCTION__, __LINE__);
        return;
    }

    if (strcmp(card_desc, VIRTUAL_FPGA_CARD_DES) == 0) {
        return;
    }
    

    // 获取slave card对象
    (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));

    if (strlen(slave_card_class_name) != 0) {
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get pcie card slave card object handle failed", __FUNCTION__);
            return;
        }
    }

    // 对单卡上下电进行判断，如果下电则清理数据，并返回OK
    if (pcie_card_get_power_state(obj_handle) != PCIE_POWER_ON) {
        // 设成未初始化，下面重新初始化
        pm_chip_info->init_state = INIT_STATE_INCOMPLETE;
        update_temp_read_state(obj_handle, SENSOR_NA_READING);
        update_sensors_reading_state(obj_handle, SENSOR_NA_READING);
        update_power_read_state(slave_obj_handle, SENSOR_NA_READING);
        update_optical_temp_power_default(obj_handle, pm_chip_info);
        // PCIE卡的故障码清空
        clear_pcie_card_fault(obj_handle, pm_chip_info);
        // 下电时清空过温过流状态位, 以及系统软件上报的故障状态
        pm_chip_info->pcieslot_fault_state = 0;
        pm_chip_info->fault_state_from_sms = 0;
        pm_chip_info->reset_mcu_state = RESET_MCU_IDLE;
        pm_chip_info->reset_mcu_task_id = 0;
        return;
    }
    
    // 初始化失败，温度读取失败告警，功耗读取失败告警
    if (INIT_STATE_FAIL == pm_chip_info->init_state && (PCIE_POWER_ON == pcie_card_get_power_state(obj_handle))) {
        
        update_temp_read_state(obj_handle, SENSOR_INVALID_READING);
        update_sensors_reading_state(obj_handle, SENSOR_INVALID_READING);
        update_power_read_state(slave_obj_handle, SENSOR_INVALID_READING);
        pcie_card_update_info_fail(obj_handle);
        return;
    }

    // MCU升级或初始化未完成，重新初始化PCIE卡信息
    gint32 mcu_version_state = check_mcu_version_info(slave_obj_handle, pm_chip_info);
    if ((mcu_version_state == RET_ERR) || (pm_chip_info->init_state == INIT_STATE_INCOMPLETE)) {
        gint32 init_flag = PCIE_INIT_WITHOUT_LOG_DUMP;
        pcie_mgnt_init_info(obj_handle, (gpointer)&init_flag);
        return;
    }

    __do_pcie_card_info_update_by_std_smbus(obj_handle, slave_obj_handle, pm_chip_info);
}
 
 

LOCAL void pcie_card_update_temperature_by_smbus_reg(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define MAX_RETRY_TIME 3
    gint32 ret;
    guint32 temp_value32 = 0;
    guint32 *p_temp_value32 = &temp_value32;
    guint16 temp_value = 0;
    OBJ_HANDLE ref_handle = 0;
 
    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, &temp_value);
    ret += dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REFCHIP, &ref_handle);
    
    if (pcie_card_mgnt_ready() == TRUE && ret == RET_OK) {
        for (gint32 retry_time = 0; retry_time < MAX_RETRY_TIME; ++retry_time) {
            ret = dfl_chip_blkread(dfl_get_object_name(ref_handle), 0x03, sizeof(temp_value32), &temp_value32);
            if (ret == DFL_OK) {
                break;
            }
            
            
            vos_task_delay(500);
        }
        debug_log(DLOG_DEBUG, "%s: %s temp_value32=%x", __FUNCTION__, dfl_get_object_name(ref_handle),
            temp_value32);
        temp_value = ceil(*((float *)((void *)p_temp_value32))); 
        if (ret != RET_OK) {
            temp_value = temp_value | 0x8000;
            debug_log(DLOG_DEBUG, "%s failed, temp_value=0x%04x, ret=0x%4x", __FUNCTION__, temp_value, ret);
        }
    } else {
        temp_value = (temp_value & PCIE_SENSOR_INVALID_READING) | 0x4000; // 先清除最高位再置次高位为1
    }
 
    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_PCIE_CARD_CHIPTEMP, temp_value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set %s ChipTemp failed, ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle), ret);
    }
}
 

void update_pcie_card_info_by_smbus_reg(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    pcie_card_update_temperature_by_smbus_reg(obj_handle, pm_chip_info);
}