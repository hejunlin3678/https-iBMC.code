
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "fpga_card.h"
#include "get_version.h"

typedef struct tag_ipmi_mcu_reset_param {
    OBJ_HANDLE obj_handle;
    guint8 device_num;
} IPMI_MCU_RESET_PARAM;


LOCAL gint32 pcie_card_get_random_data_from_mcu(PM_CHIP_INFO *pm_chip_info, guint16 opcode, guint8 cmd_arg,
    guint8 **output_data, guint32 *data_len);
LOCAL gint32 __update_npucard_bdf_info(OBJ_HANDLE fpga_handle, gpointer user_data);

LOCAL pthread_mutex_t g_restore_mutex = PTHREAD_MUTEX_INITIALIZER;


FPGA_FW_RESTORE_STATUS g_fpga_fw_restore_status[MAX_SLOT_COUNT];



LOCAL gint32 process_each_fpga_card(OBJ_HANDLE handle, gpointer data)
{
    
    gint32 ret = get_pcb_version(handle, PROPERTY_FPGA_CARD_PCBID, PROPERTY_FPGA_CARD_PCBVER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    
    ret = get_logic_version(handle, PROPERTY_FPGA_CARD_LOGIC_VERID, PROPERTY_FPGA_CARD_LOGIC_VER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 fpga_card_init(void)
{
    (void)dfl_foreach_object(CLASS_PCIE_FPGA_CARD, process_each_fpga_card, NULL, NULL);

    return RET_OK;
}

LOCAL void __update_npu_bdf_per_pciecard_task(OBJ_HANDLE npu_handle)
{
    if (__update_npucard_bdf_info(npu_handle, NULL) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update %s bdf failed.", __FUNCTION__, dfl_get_object_name(npu_handle));
    }
    return;
}


gint32 fpga_card_start(void)
{
    return RET_OK;
}


gint32 fpga_card_add_object_callback(OBJ_HANDLE object_handle)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 functionclass = 0;

    
    (void)dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &obj_handle);

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);

    if (functionclass == PCIECARD_FUNCTION_NPU) {
        OBJ_HANDLE smbios_handle = 0;
        guint8 smbios_state = SMBIOS_WRITE_NOT_START;
        gint32 ret = dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &smbios_handle);
        if (ret == RET_OK) {
            (void)dal_get_property_value_byte(smbios_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_state);
        }
        if (smbios_state == SMBIOS_WRITE_FINISH) {
            gulong task_id;
            (void)vos_task_create(&task_id, "update_npu_bdf_per_pciecard",
                (TASK_ENTRY)__update_npu_bdf_per_pciecard_task, (void *)(long)object_handle, DEFAULT_PRIORITY);
        }
        card_manage_log_operation_log("NPU Card", object_handle, PROPERTY_FPGA_CARD_SLOT, NULL, CARD_PLUG_IN);
    } else {
        card_manage_log_operation_log("FPGA Card", object_handle, PROPERTY_FPGA_CARD_SLOT, NULL, CARD_PLUG_IN);
    }

    return process_each_fpga_card(object_handle, NULL);
}


gint32 fpga_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}


gint32 fpga_card_dump_info(const gchar *path)
{
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *pcb_ver = NULL;
    const gchar *logic_ver = NULL;
    const gchar *manufacturer = NULL;
    gchar file_name[FPGA_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar fpga_card_info[FPGA_CARD_DUMPINFO_MAX_LEN + 1] = {0};

    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    if (path == NULL) {
        return RET_ERR;
    }

    gint32 ret = dfl_get_object_list(CLASS_PCIE_FPGA_CARD, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    gint32 iRet = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    FILE *fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    (void)snprintf_s(fpga_card_info, sizeof(fpga_card_info), sizeof(fpga_card_info) - 1, "%s", "FPGA Card Info\n");
    size_t fwrite_back = fwrite(fpga_card_info, strlen(fpga_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(fpga_card_info, FPGA_CARD_DUMPINFO_MAX_LEN + 1, 0, FPGA_CARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(fpga_card_info, sizeof(fpga_card_info), sizeof(fpga_card_info) - 1,
        "%-4s | %-10s | %-10s | %-32s | %-10s | %s\n", "Slot", "BoardId", "Name", "Manufacturer", "PCB Ver",
        "Logic Ver");
    fwrite_back = fwrite(fpga_card_info, strlen(fpga_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_FPGA_CARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FPGA_CARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FPGA_CARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FPGA_CARD_MANU);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FPGA_CARD_PCBVER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_FPGA_CARD_LOGIC_VER);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get FPGA Card information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }
        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        pcb_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        logic_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);

        (void)memset_s(fpga_card_info, FPGA_CARD_DUMPINFO_MAX_LEN + 1, 0, FPGA_CARD_DUMPINFO_MAX_LEN + 1);
        iRet = snprintf_s(fpga_card_info, sizeof(fpga_card_info), sizeof(fpga_card_info) - 1,
            "%-4u | 0x%-4x     | %-10s | %-32s | %-10s | %s\n", slot_id, board_id, name, manufacturer, pcb_ver,
            logic_ver);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        fwrite_back = fwrite(fpga_card_info, strlen(fpga_card_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(fpga_card_info, FPGA_CARD_DUMPINFO_MAX_LEN + 1, 0, FPGA_CARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(fpga_card_info, sizeof(fpga_card_info), sizeof(fpga_card_info) - 1, "%s", "\n\n");
    fwrite_back = fwrite(fpga_card_info, strlen(fpga_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(fpga_card_info, FPGA_CARD_DUMPINFO_MAX_LEN + 1, 0, FPGA_CARD_DUMPINFO_MAX_LEN + 1);
    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}

gint32 card_manage_operate_virturl_fpga_card(guint8 slot_in, guint8 flg_in)
{
#define FPGA_BOM_ID "14140130"
#define FPGA_ID 0xFFFF

    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 slot = 0;
    guint8 present = 0;
    gchar bom_id[MAX_NAME_SIZE+1] = {0};

    flg_in = flg_in ? PCIE_CARD_XML_LOAD : PCIE_CARD_XML_UNLOAD;

    gint32 ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &obj_list);
    if (ret != DFL_OK) {
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        bom_id[0] = 0;
        slot = 0;
        // checklist bomid -> slot -> present
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_CONNECTOR_BOM, bom_id, sizeof(bom_id));
        if (strcmp(bom_id, FPGA_BOM_ID) != 0) {
            continue;
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CONNECTOR_SLOT, &slot);
        if (slot_in != slot) {
            continue;
        }

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CONNECTOR_PRESENT, &present);
        if (ret == RET_OK) {
            if (present == flg_in) {
                continue;
            }
        } else {
            continue;
        }

        dfl_notify_presence_v2((OBJ_HANDLE)obj_node->data, FPGA_ID, 0, flg_in); // flag_in： 1 插入 0拔出

        break;
    }

    g_slist_free(obj_list);
    return RET_OK;
}

gint32 card_manage_waite_virturl_fpga_card(guint8 slot_in, guint32 position, guint8 flg_in, OBJ_HANDLE *obj_handle)
{
    card_manage_operate_virturl_fpga_card(slot_in, flg_in);

    // 等待加载完成
    for (gint32 i = 0; i < LOAD_XML_WAIT_OBJ_TIME; i++) {
        (void)vos_task_delay(CHECK_OBJ_PERIOD);
        (void)get_card_obj_handle_by_slot_position(obj_handle, slot_in, position);
        if (*obj_handle != 0) {
            break;
        }
    }

    if (*obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s: get virtual FPGA card obj failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 pcie_card_get_dimm_presence_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    guint8 buf[RESP_DATA_LENGTH_DIMM_PRESENCE] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };
    GSList *input_list = NULL;
    guint8 single_dimm_present_state = 0;
    GSList *dimm_list = NULL;
    GSList *dimm_node = NULL;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_DIMM_PRESENT;
    // 从MCU获取内存在位信息
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info,
        (guint8 *)&head_para, buf, RESP_DATA_LENGTH_DIMM_PRESENCE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get DIMM presence failed, ret = %d", __FUNCTION__, ret);
        return ret;
    }

    DIMM_PRESENT_INFO *dimm_present_info = (DIMM_PRESENT_INFO *)buf;

    ret = dal_get_object_list_position(obj_handle, CLASS_FPGA_MEMORY, &dimm_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get fpga_memory_obj_handle failed, object_handle is %s, ret %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return ret;
    }
    guint32 all_dimm_present_state = dimm_present_info->dimm_present;

    // 遍历所有内存句柄，设置内存在位信息
    for (dimm_node = dimm_list; dimm_node; dimm_node = g_slist_next(dimm_node)) {
        single_dimm_present_state = all_dimm_present_state & 1;
        input_list = g_slist_append(input_list, g_variant_new_string(PROPERTY_MEM_PRESENCE));
        input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
            &single_dimm_present_state, sizeof(single_dimm_present_state), sizeof(guint8)));

        ret = dfl_call_class_method((OBJ_HANDLE)dimm_node->data, METHOD_MEM_SET_DIMM_INFO_FROM_MCU, NULL, input_list,
            NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        all_dimm_present_state = all_dimm_present_state >> 1;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:call METHOD_MEM_SET_DIMM_INFO_FROM_MCU fail, ret %d", __FUNCTION__, ret);
            continue;
        }
    }

    g_slist_free(dimm_list);
    return RET_OK;
}

LOCAL void pcie_card_init_dimm_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    GSList *dimm_list = NULL;
    GSList *dimm_node = NULL;
    guint8 dimm_present_state = 0;
    guint8 *buf = NULL;
    guint32 buf_len = 0;
    GSList *input_list = NULL;
    guint8 dimm_num = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__);
        return;
    }
    // 先更新所有内存条在位信息
    gint32 ret = pcie_card_get_dimm_presence_from_mcu(obj_handle, pm_chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: update fpga dimm presence state failed, ret %d", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_object_list_position(obj_handle, CLASS_FPGA_MEMORY, &dimm_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get fpga_memory_obj_handle failed, ret %d", __FUNCTION__, ret);
        return;
    }

    // 遍历所有内存句柄，获取各内存属性
    for (dimm_node = dimm_list; dimm_node; dimm_node = g_slist_next(dimm_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)dimm_node->data, PROPERTY_MEM_PRESENCE, &dimm_present_state);
        // 如果当前内存条不在位，继续遍历下一个
        if (DIMM_PREENCE != dimm_present_state) {
            continue;
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)dimm_node->data, PROPERTY_MEM_DIMM_NUM, &dimm_num);
        ret = pcie_card_get_random_data_from_mcu(pm_chip_info, REGISTER_KEY_DIMM_INFO, dimm_num - 1, &buf, &buf_len);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: pcie card get dimm info from mcu failed, ret %d", __FUNCTION__, ret);
            continue;
        }

        input_list = g_slist_append(input_list, g_variant_new_string(DIMM_FIXED_VALUE_PROPERTY));
        input_list = g_slist_append(input_list,
            (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, buf, buf_len, sizeof(guint8)));
        ret = dfl_call_class_method((OBJ_HANDLE)dimm_node->data, METHOD_MEM_SET_DIMM_INFO_FROM_MCU, NULL, input_list,
            NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        g_free(buf);
        buf = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:call METHOD_MEM_SET_DIMM_INFO_FROM_MCU fail, ret %d", __FUNCTION__, ret);
        }
    }

    g_slist_free(dimm_list);
}

gint32 check_mcu_version_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gchar new_mcu_version[STR_FIRMWARE_LEN] = {0};
    gchar old_mcu_version[STR_FIRMWARE_LEN] = {0};
    guint8 buf[KEY_FIRM_VER_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_FIRMWARE;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_FIRM_VER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get mcu version failed", __FUNCTION__);
        return FIRMWARE_UNKNOW;
    }

    (void)snprintf_s((gchar *)new_mcu_version, STR_FIRMWARE_LEN, STR_FIRMWARE_LEN - 1, "%u.%u.%u", buf[0], buf[1],
        buf[2]);

    ret = dal_get_property_value_string(obj_handle, PROPERTY_FPGA_CARD_MCUVERSION, old_mcu_version,
        sizeof(old_mcu_version) - 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get mcu version failed", __FUNCTION__);
        return FIRMWARE_UNKNOW;
    }

    if (strcmp(new_mcu_version, old_mcu_version) != 0) {
        ret = dal_set_property_value_string(obj_handle, PROPERTY_FPGA_CARD_MCUVERSION, new_mcu_version,
            DF_SAVE_TEMPORARY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set mcu version failed", __FUNCTION__);
            return FIRMWARE_UNKNOW;
        }
        debug_log(DLOG_ERROR, "%s: firmware change, old version: %s, new version: %s", __FUNCTION__, old_mcu_version,
            new_mcu_version);
        return RET_ERR;
    }

    return RET_OK;
}

guint32 calc_ddr_temp_read_fail_duration(PM_CHIP_INFO *pm_chip_info)
{
    guint32 duration = 0;
    guint32 cur_timestamp = (guint32)vos_tick_get();

    if (pm_chip_info->ddr_temp_read_fail_timestamp == 0) {
        pm_chip_info->ddr_temp_read_fail_timestamp = cur_timestamp;
    } else if (pm_chip_info->ddr_temp_read_fail_timestamp <= cur_timestamp) {
        duration = cur_timestamp - pm_chip_info->ddr_temp_read_fail_timestamp;
    } else {
        duration = cur_timestamp + (G_MAXUINT32 - pm_chip_info->ddr_temp_read_fail_timestamp);
    }

    return duration;
}


void pcie_card_update_dimm_temp_by_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    GSList *dimm_node = NULL;
    gint32 ret = 0;
    GSList *dimm_list = NULL;
    guint8 dimm_present_state = 0;
    guint8 dimm_num = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guint16 dimm_temp_value = 0;
    GSList *input_list = NULL;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: input parameter pm_chip_info is NULL.", __FUNCTION__);
        return;
    }

    
    if (pcie_card_mgnt_ready() == TRUE) {
        ret = dal_get_object_list_position(obj_handle, CLASS_FPGA_MEMORY, &dimm_list);
        if (ret != RET_OK) {
            debug_log(DLOG_INFO, "%s: get fpga_memory_obj_handle failed, ret %d", __FUNCTION__, ret);
            return;
        }

        // 遍历所有内存句柄，获取各内存温度
        for (dimm_node = dimm_list; dimm_node != NULL; dimm_node = g_slist_next(dimm_node)) {
            (void)dal_get_property_value_byte((OBJ_HANDLE)dimm_node->data, PROPERTY_MEM_PRESENCE, &dimm_present_state);
            if (DIMM_PREENCE != dimm_present_state) {
                continue;
            }

            (void)dal_get_property_value_byte((OBJ_HANDLE)dimm_node->data, PROPERTY_MEM_DIMM_NUM, &dimm_num);

            (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
            head_para.opcode = REGISTER_KEY_DIMM_TEMP;
            head_para.cmd_arg = dimm_num - 1;
            ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (guint8 *)&dimm_temp_value,
                sizeof(dimm_temp_value));
            if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
                continue;
            } else if (ret == RET_OK) {
                debug_log(DLOG_INFO, "%s: get DIMM temperature value %u", __FUNCTION__, dimm_temp_value);
                pm_chip_info->power_read_fail_timestamp = 0;

                // 处理从MCU读取温度异常值:0x7fff,0x7ffd
                if (dimm_temp_value == DIMM_TEMP_READ_INVALID) {
                    dimm_temp_value = SENSOR_NA_READING;
                } else if (dimm_temp_value == DIMM_TEMP_READ_FAIL) {
                    dimm_temp_value = SENSOR_INVALID_READING;
                }
            } else { // 用芯片复位时间作防抖
                guint32 duration = calc_ddr_temp_read_fail_duration(pm_chip_info);
                if (duration > ((guint32)pm_chip_info->chip_reset_time * TIME_UNIT_SECOND * TIME_MULTIPLE)) {
                    dimm_temp_value = SENSOR_INVALID_READING;
                    debug_log(DLOG_DEBUG, "%s : get DIMM temperature over SMBus failed, return %d", __FUNCTION__, ret);
                    continue; // 如果没超过防抖时间，不更新温度值
                }
            }

            input_list = g_slist_append(input_list, g_variant_new_string(PROPERTY_MEM_TEMP));
            input_list = g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
                (guint8 *)&dimm_temp_value, sizeof(dimm_temp_value), sizeof(guint8)));
            ret = dfl_call_class_method((OBJ_HANDLE)dimm_node->data, METHOD_MEM_SET_DIMM_INFO_FROM_MCU, NULL,
                input_list, NULL);
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
            input_list = NULL;
            if (ret != DFL_OK) {
                debug_log(DLOG_DEBUG, "%s:call METHOD_MEM_SET_DIMM_INFO_FROM_MCU fail, ret %d", __FUNCTION__, ret);
            }
        }

        g_slist_free(dimm_list);
    }

    return;
}

LOCAL gint32 get_memory_capacity(PM_CHIP_INFO *pm_chip_info, const guint8 npu_id, guint8 *buf, guint32 buf_len)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    head_para.opcode = REGISTER_KEY_PCIE_MEMORY;
    head_para.data_object_index = npu_id;
    head_para.cmd_arg = REGISTER_KEY_PCIE_MEMORY_CMD;
    head_para.input_data_len = 1;
    head_para.input_data[0] = REGISTER_KEY_PCIE_MEMORY_INPUT;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, buf_len);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get Memory Capacity failed, npu_id %u, return %d", __FUNCTION__, npu_id, ret);
        return RET_ERR;
    }
    return RET_OK;
}

void pcie_card_update_memory_capacity(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    guint8 npu_present_state = 0;
    guint32 npu_memory_support = 0;
    guint8 npu_id = 0;
    guint32 get_value = 0;
    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: input parameter pm_chip_info is NULL.", __FUNCTION__);
        return;
    }

    
    if (!pcie_card_mgnt_ready()) {
        return;
    }
    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get npu_obj_handle failed, ret %d", __FUNCTION__, ret);
        return;
    }

    // 遍历所有内存句柄
    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)npu_node->data, PROPERTY_NPU_PRESENCE, &npu_present_state);
        (void)dal_get_property_value_uint32((OBJ_HANDLE)npu_node->data, PROPERTY_NPU_CAPABILITY_SUPPORT_MASK,
                                            &npu_memory_support);
        if ((npu_present_state != NPU_PRESENCE) || (npu_memory_support != NPU_MEMORY_SUPPORT)) {
            continue;
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)npu_node->data, PROPERTY_NPU_ID, &npu_id);
        if (npu_id == 0) {
            continue;
        }

        ret = get_memory_capacity(pm_chip_info, npu_id, (guint8 *)&get_value, sizeof(get_value));
        if (ret != RET_OK) {
            continue;
        }
    
        GSList *input_list = g_slist_append(input_list, g_variant_new_uint32(get_value));
        ret = dfl_call_class_method((OBJ_HANDLE)npu_node->data, METHOD_SET_MEMORY_CAPACITY, NULL, input_list, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : update pcie card Memory Capacity failed, npu_id %u, return %d.",
                __FUNCTION__, npu_id, ret);
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
    }
    g_slist_free(npu_list);
}

gint32 pcie_card_update_npu_info(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, NPU_PROP_INFO *info,
    PROTOCOL_HEAD_PARA head_para)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    guint8 npu_present_state = 0;
    GSList *input_list = NULL;
    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: Input parameter pm_chip_info is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)npu_node->data, PROPERTY_NPU_PRESENCE, &npu_present_state);
        if (npu_present_state != NPU_PRESENCE) {
            continue;
        }
        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para,
            info->buf, info->buf_size);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : Get prop %s data failed, return %d.", __FUNCTION__, info->prop, ret);
            continue;
        }
        input_list = g_slist_append(input_list, g_variant_new_string(info->prop));
        input_list = g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
            info->buf, info->buf_size, sizeof(guint8)));
        ret = dfl_call_class_method((OBJ_HANDLE)npu_node->data, METHOD_SET_NPU_INFO_FROM_MCU, NULL, input_list, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : Update npu prop %s failed, return %d.", __FUNCTION__, info->prop, ret);
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
    }
    g_slist_free(npu_list);
    return RET_OK;
}

LOCAL void pcie_card_update_npu_video_memory_band_width_memory(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    NPU_PROP_INFO info = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    info.buf_size = sizeof(guint32);
    info.prop = PROPERTY_NPU_MEMORY_BAND_WIDTH;
    info.buf = (guint8*)g_malloc0(info.buf_size);
    if (info.buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : Malloc buf size failed", __FUNCTION__);
        return;
    }
    head_para.opcode = REGISTER_KEY_PCIE_MEMORY;
    head_para.input_data_len = 1;
    head_para.input_data[0] = REGISTER_KEY_PCIE_RATED_MEMORY_INPUT;

    gint32 ret = pcie_card_update_npu_info(obj_handle, pm_chip_info, &info, head_para);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Update npu video memory band width failed, return %d.", __FUNCTION__, ret);
    }
    g_free(info.buf);
}

void pcie_card_update_npu_sn(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    NPU_PROP_INFO info = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    info.buf_size = NPU_SN_LEN;
    info.prop = PROPERTY_NPU_SERIAL_NUMBER;
    info.buf = (guint8*)g_malloc0(info.buf_size);
    if (info.buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : Malloc buf size failed", __FUNCTION__);
        return;
    }
    head_para.opcode = REGISTER_KEY_SN;
    gint32 ret = pcie_card_update_npu_info(obj_handle, pm_chip_info, &info, head_para);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Update npu sn failed, return %d.", __FUNCTION__, ret);
    }
    g_free(info.buf);
}

void pcie_card_update_npu_fireware_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    NPU_PROP_INFO info = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    info.buf_size = NPU_FIRE_LEN;
    info.prop = PROPERTY_NPU_FIRMWARE_VERSION;
    info.buf = (guint8*)g_malloc0(info.buf_size);
    if (info.buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : Malloc buf size failed", __FUNCTION__);
        return;
    }
    head_para.opcode = REGISTER_KEY_PCIE_NPU_FIRMWARE;
    gint32 ret = pcie_card_update_npu_info(obj_handle, pm_chip_info, &info, head_para);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Update npu fireware version failed, return %d.", __FUNCTION__, ret);
    }
    g_free(info.buf);
}

void pcie_card_update_npu_reading_celsius(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    GSList *npu_node = NULL;
    GSList *npu_list = NULL;
    guint8 npu_present_state = 0;
    PROTOCOL_HEAD_PARA head_para = {0};
    guint16 get_value = 0;
    GSList *input_list = NULL;
    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: Input parameter pm_chip_info is NULL.", __FUNCTION__);
        return;
    }
    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_NPU, &npu_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: Get npu_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }

    for (npu_node = npu_list; npu_node != NULL; npu_node = g_slist_next(npu_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)npu_node->data, PROPERTY_NPU_PRESENCE, &npu_present_state);
        if (npu_present_state != NPU_PRESENCE) {
            continue;
        }

        head_para.opcode = REGISTER_KEY_CHIP_TEMP;
        ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para,
            (guint8 *)&get_value, sizeof(get_value));
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : Get temp failed, return %d.", __FUNCTION__, ret);
            continue;
        }
        if (get_value == NPU_TEMP_READ_INVALID) {
            get_value = SENSOR_NA_READING;
        } else if (get_value == NPU_TEMP_READ_FAIL) {
            get_value = SENSOR_INVALID_READING;
        }
        input_list = g_slist_append(input_list, g_variant_new_string(PROPERTY_NPU_READING_CELSIUS));
        input_list = g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
            (guint8*)&get_value, sizeof(guint16), sizeof(guint8)));
        ret = dfl_call_class_method((OBJ_HANDLE)npu_node->data, METHOD_SET_NPU_INFO_FROM_MCU, NULL, input_list, NULL);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s : Update pcie card temp failed, return %d.", __FUNCTION__, ret);
        }
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
    }
    g_slist_free(npu_list);
}

LOCAL void pcie_card_update_npu_power_capacity_watts(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    NPU_PROP_INFO info = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    info.buf_size = sizeof(guint32);
    info.prop = PROPERTY_NPU_POWER_CAPACITY;
    info.buf = (guint8*)g_malloc0(info.buf_size);
    if (info.buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : Malloc buf size failed", __FUNCTION__);
        return;
    }
    head_para.opcode = REGISTER_KEY_PCIE_RATED_POWER;
    head_para.input_data[0] = REGISTER_KEY_PCIE_RATED_POWER_INPUT;
    head_para.input_data_len = 1;
    gint32 ret = pcie_card_update_npu_info(obj_handle, pm_chip_info, &info, head_para);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Update npu power capacity watts failed, return %d.", __FUNCTION__, ret);
    }
    g_free(info.buf);
}

void pcie_card_update_npu_power_consunmed_watts(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    NPU_PROP_INFO info = {0};
    PROTOCOL_HEAD_PARA head_para = {0};
    info.buf_size = sizeof(gint16);
    info.prop = PROPERTY_NPU_POWER_CONSUME;
    info.buf = (guint8*)g_malloc0(info.buf_size);
    if (info.buf == NULL) {
        debug_log(DLOG_DEBUG, "%s : Malloc buf size failed", __FUNCTION__);
        return;
    }
    head_para.opcode = REGISTER_KEY_POWER;
    gint32 ret = pcie_card_update_npu_info(obj_handle, pm_chip_info, &info, head_para);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : Update npu power consunmed failed, return %d.", __FUNCTION__, ret);
    }
    g_free(info.buf);
}


LOCAL gint32 pcie_card_get_fixed_data_from_mcu(PM_CHIP_INFO *pm_chip_info, guint8 cmd_arg, guint16 opcode,
    guint8 *output_data, guint32 data_len)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL || output_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.cmd_arg = cmd_arg;
    head_para.opcode = opcode;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, output_data, data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card fixed data from mcu failed! ret = %d", __FUNCTION__, ret);
        return ret;
    }

    return ret;
}

LOCAL void update_optical_info_from_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 silknum,
    guint8 optical_present_state)
{
    gchar optical_vendorname[RESP_DATA_LENGTH_OPTICAL_VENDORNAME] = {0};
    gchar optical_partnumber[RESP_DATA_LENGTH_OPTICAL_PARTNUMBER] = {0};
    gchar optical_sn[RESP_DATA_LENGTH_OPTICAL_SN] = {0};
    gchar optical_manufacturedate[RESP_DATA_LENGTH_OPTICAL_MANUFACTUREDATE] = {0};
    guint8 optical_type_info[RESP_DATA_LENGTH_OPTICAL_TYPE_INFO] = {0};
    json_object *json_obj = NULL;
    OP_CURRENT_POWER_THER *current_power_ther_data = NULL;
    GSList *input_list = NULL;
    gint32 ret = 0;
    const gchar *rsc_str = NULL;
    gchar optical_type[MAX_OPTICAL_TYPE_LEN] = {0};
    gchar optical_wavelength_str[MAX_OPTICAL_WAVELENGTH_LEN] = {0};
    gchar optical_mediummode_str[MAX_OPTICAL_MEDIUMMODE_LEN] = {0};
    gint32 tmp_power_alarm = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: input param error", __FUNCTION__);
        return;
    }

    // 获取光模块厂家名称
    pcie_card_get_fixed_data_from_mcu(pm_chip_info, silknum, REGISTER_KEY_MFPGA_OPTICAL_VENDORNAME,
        (guint8 *)optical_vendorname, RESP_DATA_LENGTH_OPTICAL_VENDORNAME);
    // 获取光模块厂家部件号
    pcie_card_get_fixed_data_from_mcu(pm_chip_info, silknum, REGISTER_KEY_MFPGA_OPTICAL_PARTNUMBER,
        (guint8 *)optical_partnumber, RESP_DATA_LENGTH_OPTICAL_PARTNUMBER);
    // 获取光模块序列号
    pcie_card_get_fixed_data_from_mcu(pm_chip_info, silknum, REGISTER_KEY_MFPGA_OPTICAL_SN, (guint8 *)optical_sn,
        RESP_DATA_LENGTH_OPTICAL_SN);
    // 获取光模块生产日期
    pcie_card_get_fixed_data_from_mcu(pm_chip_info, silknum, REGISTER_KEY_MFPGA_OPTICAL_MANUFACTUREDATE,
        (guint8 *)optical_manufacturedate, RESP_DATA_LENGTH_OPTICAL_MANUFACTUREDATE);
    // 获取光模块类型等信息
    pcie_card_get_fixed_data_from_mcu(pm_chip_info, silknum, REGISTER_KEY_MFPGA_OPTICAL_TYPE_INFO, optical_type_info,
        RESP_DATA_LENGTH_OPTICAL_TYPE_INFO);

    json_obj = json_object_new_object();
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_PRESENT, json_object_new_int((int32_t)optical_present_state));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_VENDOR_NAME,
        json_object_new_string((const gchar *)optical_vendorname));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_PART_NUMBER,
        json_object_new_string((const gchar *)optical_partnumber));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_SERIAL_NUMBER, json_object_new_string((const gchar *)optical_sn));
    
    dal_remove_space_in_string(optical_manufacturedate, sizeof(optical_manufacturedate));
    
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_MANUFACTURE_DATE,
        json_object_new_string((const gchar *)optical_manufacturedate));

    current_power_ther_data = (OP_CURRENT_POWER_THER *)optical_type_info;

    // 转换光模块类型数据
    if (OPTICAL_TYPE_QSFP == current_power_ther_data->type) {
        (void)strncpy_s(optical_type, sizeof(optical_type), "QSFP", strlen("QSFP"));
    } else {
        (void)strncpy_s(optical_type, sizeof(optical_type), "N/A", strlen("N/A"));
        debug_log(DLOG_DEBUG, "%s: optical module type is invaild", __FUNCTION__);
    }

    // 转换光模块波长数据
    current_power_ther_data->Wavelength = current_power_ther_data->Wavelength * 5 / 100;
    (void)snprintf_s(optical_wavelength_str, sizeof(optical_wavelength_str), sizeof(optical_wavelength_str) - 1, "%unm",
        current_power_ther_data->Wavelength);

    // 转换光模块传输模式数据
    if (OPTICAL_MEDIUM_MODE_FOR_MM == current_power_ther_data->MediumMode) {
        (void)strncpy_s(optical_mediummode_str, sizeof(optical_mediummode_str), "MM",
            sizeof(optical_mediummode_str) - 1);
    } else if (OPTICAL_MEDIUM_MODE_FOR_SM == current_power_ther_data->MediumMode) {
        (void)strncpy_s(optical_mediummode_str, sizeof(optical_mediummode_str), "SM",
            sizeof(optical_mediummode_str) - 1);
    } else {
        (void)strncpy_s(optical_mediummode_str, sizeof(optical_mediummode_str), "N/A",
            sizeof(optical_mediummode_str) - 1);
        debug_log(DLOG_DEBUG, "%s: optical module mediummode is invaild", __FUNCTION__);
    }

    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TYPE, json_object_new_string((const gchar *)optical_type));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_WAVE_LENGTH,
        json_object_new_string((const gchar *)optical_wavelength_str));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_MEDIUM_MODE,
        json_object_new_string((const gchar *)optical_mediummode_str));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_CHANNEL_NUM,
        json_object_new_int(current_power_ther_data->ChannelNum));

    // 小数位只保留2位
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->RxPowerHighAlarm)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_HIGH_ALARM,
        json_object_new_double(tmp_power_alarm * 0.01));
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->RxPowerHighWarn)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_HIGH_WARN,
        json_object_new_double(tmp_power_alarm * 0.01));
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->RxPowerLowAlarm)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_LOW_ALARM,
        json_object_new_double(tmp_power_alarm * 0.01));
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->RxPowerLowWarn)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_LOW_WARN,
        json_object_new_double(tmp_power_alarm * 0.01));

    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_HIGH_ALARM,
        json_object_new_double((gint32)(REVERT_UINT16(current_power_ther_data->TxBiasHighAlarm) / 5) * 0.01));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_HIGH_WARN,
        json_object_new_double((gint32)(REVERT_UINT16(current_power_ther_data->TxBiasHighWarn) / 5) * 0.01));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_LOW_ALARM,
        json_object_new_double((gint32)(REVERT_UINT16(current_power_ther_data->TxBiasLowAlarm) / 5) * 0.01));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_LOW_WARN,
        json_object_new_double((gint32)(REVERT_UINT16(current_power_ther_data->TxBiasLowWarn) / 5) * 0.01));

    // 小数位只保留2位
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->TxPowerHighAlarm)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_HIGH_ALARM,
        json_object_new_double(tmp_power_alarm * 0.01));
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->TxPowerHighWarn)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_HIGH_WARN,
        json_object_new_double(tmp_power_alarm * 0.01));
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->TxPowerLowAlarm)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_LOW_ALARM,
        json_object_new_double(tmp_power_alarm * 0.01));
    tmp_power_alarm = (gint32)(log10(REVERT_UINT16(current_power_ther_data->TxPowerLowWarn)) * 100);
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_LOW_WARN,
        json_object_new_double(tmp_power_alarm * 0.01));

    rsc_str = dal_json_object_get_str(json_obj);
    if (rsc_str == NULL) {
        (void)json_object_put(json_obj);
        debug_log(DLOG_DEBUG, "%s: rsc_str is null.", __FUNCTION__);
        return;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(rsc_str));
    (void)json_object_put(json_obj);
    ret = dfl_call_class_method(obj_handle, METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: call METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO failed, result is %d.", __FUNCTION__,
            ret);
        return;
    }
}


LOCAL void update_optical_info_default(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info, guint8 optical_present_state)
{
    GSList *input_list = NULL;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: input param error", __FUNCTION__);
        return;
    }

    json_object *json_obj = json_object_new_object();
    if (json_obj == NULL) {
        debug_log(DLOG_DEBUG, "%s: json_object_new_object failed", __FUNCTION__);
        return;
    }

    json_object_object_add(json_obj, PROPERTY_OPT_MDL_PRESENT, json_object_new_int((int32_t)optical_present_state));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_VENDOR_NAME, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_PART_NUMBER, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_SERIAL_NUMBER, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_MANUFACTURE_DATE, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TYPE, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_WAVE_LENGTH, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_MEDIUM_MODE, json_object_new_string(INVALID_DATA_STRING));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_CHANNEL_NUM, json_object_new_int((int32_t)INVALID_DATA_BYTE));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_HIGH_ALARM, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_LOW_ALARM, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_HIGH_WARN, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_LOW_WARN, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_HIGH_ALARM, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_LOW_ALARM, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_HIGH_WARN, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_LOW_WARN, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_HIGH_ALARM, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_LOW_ALARM, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_HIGH_WARN, json_object_new_double(INVALID_DATA_WORD));
    json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_LOW_WARN, json_object_new_double(INVALID_DATA_WORD));

    const gchar *rsc_str = dal_json_object_get_str(json_obj);
    if (rsc_str == NULL) {
        (void)json_object_put(json_obj);
        debug_log(DLOG_DEBUG, "%s: rsc_str is null", __FUNCTION__);
        return;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(rsc_str));
    (void)json_object_put(json_obj);
    gint32 ret = dfl_call_class_method(obj_handle, METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: call METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO failed, result is %d", __FUNCTION__,
            ret);
        return;
    }
}

void pcie_card_check_optical_present_changed(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define DATA_LEN 5
    PROTOCOL_HEAD_PARA head_para = { 0 };
    GSList *optical_module_list = NULL;
    GSList *opt_node = NULL;
    guint8 old_optical_present = 0;
    OBJ_HANDLE port_handle = 0;
    guint8 port_num = 0;
    guint8 new_optical_present = 0;
    guint8 data_buf[DATA_LEN] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s: input param error", __FUNCTION__);
        return;
    }

    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_VIRTUAL_OPTICAL_MODULE, &optical_module_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get optical_module_obj_handle failed, ret %d", __FUNCTION__, ret);
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_MFPGA_OPTICAL_PRESENT;
    ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, data_buf, 5);
    if (ret != RET_OK) {
        g_slist_free(optical_module_list);
        debug_log(DLOG_DEBUG, "%s: pm_get_opcode_result failed, result is %d, obj name is %s", __FUNCTION__, ret,
            dfl_get_object_name(obj_handle));
        return;
    }

    guint8 list_len = g_slist_length(optical_module_list);
    if (list_len != data_buf[0]) {
        g_slist_free(optical_module_list);
        debug_log(DLOG_DEBUG, "%s: obj(%s) get optical num(%d) error, optical list is %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), data_buf[0], list_len);
        return;
    }

    // 获取所有光模块的在位信息
    guint32 optical_present_status =
        data_buf[1] | ((guint32)data_buf[2] << 8) | ((guint32)data_buf[3] << 16) | ((guint32)data_buf[4] << 24);

    // 遍历所有光模块，如果有在位状态发生变化，重新初始化
    for (opt_node = optical_module_list; opt_node; opt_node = g_slist_next(opt_node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_PRESENT, &old_optical_present);
        ret = dfl_get_referenced_object((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_REF_PORT_OBJ, &port_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: get bussiness_port_obj_class_name failed, ret is %d", __FUNCTION__, ret);
            continue;
        }
        (void)dal_get_property_value_byte(port_handle, BUSY_ETH_ATTRIBUTE_PORT_NUM, &port_num);

        new_optical_present = 1 & (optical_present_status >> port_num);

        // 光模块在位状态发生变化
        if (old_optical_present != new_optical_present) {
            if (new_optical_present == 1) {
                update_optical_info_from_mcu((OBJ_HANDLE)opt_node->data, pm_chip_info, port_num, new_optical_present);
            } else {
                update_optical_info_default((OBJ_HANDLE)opt_node->data, pm_chip_info, new_optical_present);
            }
        }
    }

    g_slist_free(optical_module_list);
}


LOCAL json_object *create_json_double_array(gdouble *double_array, gint32 count)
{
    json_object *jo = json_object_new_array();

    for (gint32 i = 0; i < count && i < MAX_CHANNEL_NUM; i++) {
        json_object_array_add(jo, json_object_new_double(double_array[i]));
    }

    return jo;
}

void pcie_card_update_optical_temp_power(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    GSList *optical_module_list = NULL;
    GSList *opt_node = NULL;
    OBJ_HANDLE port_handle = 0;
    guint8 port_num = 0;
    guint8 optical_present = 0;
    OP_TEMP_VOLT_DOUBLE optical_temp_volt = { 0 };
    OP_POWER_CURRENT optical_power_current = { { 0 }, { 0 }, { 0 } };
    guint8 channel_num = 0;
    GSList *input_list = NULL;
    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }

    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_VIRTUAL_OPTICAL_MODULE, &optical_module_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get optical_module_obj_handle failed, ret %d", __FUNCTION__, ret);
        return;
    }

    // 遍历在位的光模块，更新光模块信息
    for (opt_node = optical_module_list; opt_node; opt_node = g_slist_next(opt_node)) {
        // 找到光模块的丝印信息
        ret = dfl_get_referenced_object((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_REF_PORT_OBJ, &port_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: get bussiness_port_obj_class_name failed, ret is %d", __FUNCTION__, ret);
            continue;
        }
        (void)dal_get_property_value_byte(port_handle, BUSY_ETH_ATTRIBUTE_PORT_NUM, &port_num);

        (void)dal_get_property_value_byte((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_PRESENT, &optical_present);

        if (optical_present == 1) {
            ret = get_op_temp_volt_and_power_current_from_mcu(pm_chip_info, port_num,
                &optical_temp_volt, &optical_power_current);
            // 如果是不支持操作码，不能继续设置为默认值，应进入下次循环
            if (ret == PM_ERROR_CODE_CAP_UNSUPPORTED) {
                continue;
            }
        } else { // 如果光模块不在位,将值置为NA
            optical_temp_volt.temp = 0xFFFFFFFFFFFFFFFFULL;
            optical_temp_volt.volt = 0xFFFFFFFFFFFFFFFFULL;
            SET_ARRAY_VALUE(optical_power_current.TxBiasCurrent, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
            SET_ARRAY_VALUE(optical_power_current.TxPower, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
            SET_ARRAY_VALUE(optical_power_current.RxPower, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
        }

        json_object *json_obj = json_object_new_object();
        if (json_obj == NULL) {
            debug_log(DLOG_DEBUG, "%s: json_object_new_object failed", __FUNCTION__);
            continue;
        }
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_TEMP_CURRENT, json_object_new_double(optical_temp_volt.temp));
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_VCC_CURRENT, json_object_new_double(optical_temp_volt.volt));

        (void)dal_get_property_value_byte((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_CHANNEL_NUM, &channel_num);
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_CURRENT,
            create_json_double_array(optical_power_current.TxBiasCurrent, channel_num));
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_CURRENT,
            create_json_double_array(optical_power_current.TxPower, channel_num));
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_CURRENT,
            create_json_double_array(optical_power_current.RxPower, channel_num));

        const gchar *rsc_str = dal_json_object_get_str(json_obj);
        if (rsc_str == NULL) {
            (void)json_object_put(json_obj);
            debug_log(DLOG_DEBUG, "%s: rsc_str is null", __FUNCTION__);
            continue;
        }

        input_list = g_slist_append(input_list, g_variant_new_string(rsc_str));
        ret = dfl_call_class_method((OBJ_HANDLE)opt_node->data, METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO, NULL,
            input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        (void)json_object_put(json_obj);
        json_obj = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: call METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO failed, result is %d", __FUNCTION__,
                ret);
            continue;
        }
    }

    g_slist_free(optical_module_list);
}

void update_optical_temp_power_default(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    GSList *optical_module_list = NULL;
    GSList *opt_node = NULL;
    OP_TEMP_VOLT_DOUBLE optical_temp_volt = { 0 };
    OP_POWER_CURRENT optical_power_current = { { 0 }, { 0 }, { 0 } };
    guint8 channel_num = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_DEBUG, "%s (%d): Input parameter pm_chip_info is NULL.", __FUNCTION__, __LINE__);
        return;
    }

    gint32 ret = dal_get_object_list_position(obj_handle, CLASS_VIRTUAL_OPTICAL_MODULE, &optical_module_list);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: get optical_module_obj_handle failed, ret %d.", __FUNCTION__, ret);
        return;
    }

    // 遍历在位的光模块，更新光模块信息
    for (opt_node = optical_module_list; opt_node; opt_node = g_slist_next(opt_node)) {
        optical_temp_volt.temp = 0xFFFFFFFFFFFFFFFFULL;
        optical_temp_volt.volt = 0xFFFFFFFFFFFFFFFFULL;
        SET_ARRAY_VALUE(optical_power_current.TxBiasCurrent, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
        SET_ARRAY_VALUE(optical_power_current.TxPower, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);
        SET_ARRAY_VALUE(optical_power_current.RxPower, MAX_CHANNEL_NUM, 0xFFFFFFFFFFFFFFFFULL);

        json_object *json_obj = json_object_new_object();
        if (json_obj == NULL) {
            debug_log(DLOG_DEBUG, "%s: json_object_new_object failed", __FUNCTION__);
            continue;
        }
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_TEMP_CURRENT, json_object_new_double(optical_temp_volt.temp));
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_VCC_CURRENT, json_object_new_double(optical_temp_volt.volt));

        (void)dal_get_property_value_byte((OBJ_HANDLE)opt_node->data, PROPERTY_OPT_MDL_CHANNEL_NUM, &channel_num);
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_BIAS_CURRENT,
            create_json_double_array(optical_power_current.TxBiasCurrent, channel_num));
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_TX_POWER_CURRENT,
            create_json_double_array(optical_power_current.TxPower, channel_num));
        json_object_object_add(json_obj, PROPERTY_OPT_MDL_RX_POWER_CURRENT,
            create_json_double_array(optical_power_current.RxPower, channel_num));
        const gchar *rsc_str = dal_json_object_get_str(json_obj);
        if (rsc_str == NULL) {
            (void)json_object_put(json_obj);
            debug_log(DLOG_DEBUG, "%s: rsc_str is null", __FUNCTION__);
            continue;
        }

        GSList *input_list = g_slist_append(input_list, g_variant_new_string(rsc_str));
        ret = dfl_call_class_method((OBJ_HANDLE)opt_node->data, METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO, NULL,
            input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        (void)json_object_put(json_obj);
        json_obj = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s: call METHOD_OPT_MDL_UPDATE_OPTICAL_PROP_INFO failed, result is %d.",
                __FUNCTION__, ret);
            continue;
        }
    }

    g_slist_free(optical_module_list);
}

LOCAL void set_fpga_restore_status_fail(FPGA_FW_RESTORE_STATUS *restore_status)
{
    restore_status->status = FW_SELFHEALING_FAILED;
    restore_status->progress = 0;
}

LOCAL gint32 _fpga_golden_fw_restore_thread(void *req_data_buf)
{
#define LOAD_OBJ 1
#define REMOVE_OBJ 0

    OBJ_HANDLE obj_handle = 0;
    guint8 result[2] = {0};
    PCIE_RESTORE_INFO *data_buf = (PCIE_RESTORE_INFO *)req_data_buf;
    gchar card_desc[MAX_PCIE_CARD_DESC_LEN] = {0};
    gchar obj_name_bak[MAX_NAME_SIZE+1] = {0};
    gint32 resend_cnt = 10;

    guint8 slot_id = data_buf->slot_id;
    guint32 position = data_buf->position;

    (void)get_card_obj_handle_by_slot_position(&obj_handle, slot_id, position);

    if (obj_handle == 0) {
        card_manage_waite_virturl_fpga_card(slot_id, position, LOAD_OBJ, &obj_handle);
        if (obj_handle == 0) {
            set_fpga_restore_status_fail(&g_fpga_fw_restore_status[slot_id]);
            debug_log(DLOG_ERROR, "%s: get virtual FPGA card obj failed", __FUNCTION__);
            goto __EXIT_RESTORE;
        }

        pcie_mgnt_init_virtual_card_info(obj_handle);
        // prepare ok! go on
    }

    gint32 ret = strcpy_s(obj_name_bak, sizeof(obj_name_bak), dfl_get_object_name(obj_handle));
    if (ret != RET_OK) {
        set_fpga_restore_status_fail(&g_fpga_fw_restore_status[slot_id]);
        debug_log(DLOG_ERROR, "%s: backup obj name failed. ret = %d", __FUNCTION__, ret);
        goto __EXIT_RESTORE;
    }

    ret = ipmi_mcu_set_fpga_golden_fw_restore(obj_handle);
    if (ret != RET_OK) {
        set_fpga_restore_status_fail(&g_fpga_fw_restore_status[slot_id]);
        debug_log(DLOG_ERROR, "%s: mcu set fpga golden firmware restore failed. ret = %d", __FUNCTION__, ret);
        goto __EXIT_RESTORE;
    }

    do {
        
        (void)vos_task_delay(5 * 1000);

        ret = ipmi_mcu_get_fpga_golden_fw_restore_result(obj_handle, result, sizeof(result));
        if ((ret != RET_OK) && (resend_cnt > 0)) {
            resend_cnt--;
            if (dfl_get_object_handle(obj_name_bak, &obj_handle) != DFL_OK) {
                card_manage_waite_virturl_fpga_card(slot_id, position, LOAD_OBJ, &obj_handle);
                if (obj_handle == 0) {
                    set_fpga_restore_status_fail(&g_fpga_fw_restore_status[slot_id]);
                    debug_log(DLOG_ERROR, "%s: get virtual FPGA card obj failed. ret = %d", __FUNCTION__, ret);
                    goto __EXIT_RESTORE;
                }
                // initial obj info
                pcie_mgnt_init_virtual_card_info(obj_handle);
            }

            continue;
        }

        if (ret != RET_OK) {
            set_fpga_restore_status_fail(&g_fpga_fw_restore_status[slot_id]);
            debug_log(DLOG_ERROR, "%s: mcu get fpga golden firmware restore result failed. ret = %d", __FUNCTION__,
                ret);
            goto __EXIT_RESTORE;
        }

        resend_cnt = 10;

        g_fpga_fw_restore_status[slot_id].status = result[0];
        g_fpga_fw_restore_status[slot_id].progress = result[1];
    } while (FW_SELFHEALING_IN_PROGERSS == result[0] && FW_SELFHEALING_COMPLETED != result[1]);

__EXIT_RESTORE:
    if (obj_handle) {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_DESC, card_desc, sizeof(card_desc));

        if (strcmp(card_desc, VIRTUAL_FPGA_CARD_DES) == 0) {
            (void)card_manage_operate_virturl_fpga_card(slot_id, REMOVE_OBJ);
        }
    }
    if (slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s:slot id: %d is out of range.", __FUNCTION__, slot_id);
        g_free(data_buf);
        return RET_ERR;
    }
    if (RESTORE_FROM_IPMI == data_buf->op_type) {
        if (FW_SELFHEALING_COMPLETED == g_fpga_fw_restore_status[slot_id].progress) {
            ipmi_operation_log(&data_buf->msg, "The restore of the Golden firmware of the FPGA card %d is successful",
                slot_id);
        } else {
            ipmi_operation_log(&data_buf->msg, "Failed to restore the Golden firmware of the FPGA card %d", slot_id);
        }
    }

    g_free(data_buf);

    (void)pthread_mutex_lock(&g_restore_mutex);
    g_fpga_fw_restore_status[slot_id].doing = 0;
    (void)pthread_mutex_unlock(&g_restore_mutex);

    return RET_OK;
}

LOCAL gint32 prepare_to_restore_fpga_card(FPGA_FW_RESTORE_STATUS *status, void *input, gint32 *todoflg)
{
    gint32 ret = RET_OK;
    TASKID task_id;
    const gchar *task_name = "FpgaGoldenFwRestoreTask";

#define RESET_DOING_FLG                         \
    (void)pthread_mutex_lock(&g_restore_mutex); \
    status->doing = 0;                          \
    (void)pthread_mutex_unlock(&g_restore_mutex)

    (void)pthread_mutex_lock(&g_restore_mutex);
    if (status->doing == 0) {
        status->doing = 1;
        status->status = 0;
        *todoflg = 1;
    }
    (void)pthread_mutex_unlock(&g_restore_mutex);

    if (*todoflg == 1) {
        ret = vos_task_create(&task_id, task_name, (TASK_ENTRY)_fpga_golden_fw_restore_thread, input, DEFAULT_PRIORITY);
        if (ret == RET_ERR) {
            RESET_DOING_FLG;
            g_free(input);
            debug_log(DLOG_ERROR, "create %s failed, ret is %d", task_name, ret);
            return RET_ERR;
        }

        debug_log(DLOG_ERROR, "%s:_fpga_golden_fw_restore_thread created successfully.", __FUNCTION__);
    } else {
        g_free(input);
    }

    return ret;
}

gint32 card_manage_restore_fpga_card(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 todo_flg = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    PCIE_RESTORE_INFO *input = (PCIE_RESTORE_INFO *)g_malloc(sizeof(PCIE_RESTORE_INFO));
    if (input == NULL) {
        debug_log(DLOG_ERROR, "%s:malloc memory fail.", __FUNCTION__);
        return RET_ERR;
    }

    input->slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (input->slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s:slot id: %d is out of range.", __FUNCTION__, input->slot_id);
        g_free(input);
        return RET_ERR;
    }

    input->position = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    input->op_type = RESTORE_FROM_CLI;

    gint32 ret = prepare_to_restore_fpga_card(&g_fpga_fw_restore_status[input->slot_id], input, &todo_flg);

    *output_list = g_slist_append(*output_list, g_variant_new_int32(todo_flg));

    return ret;
}

gint32 card_manage_restore_fpga_card_percent(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }
    guint8 slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s:slot_id is out of range.", __FUNCTION__);
        return RET_ERR;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_byte(g_fpga_fw_restore_status[slot_id].status));
    *output_list = g_slist_append(*output_list, g_variant_new_byte(g_fpga_fw_restore_status[slot_id].progress));

    return RET_OK;
}

gint32 card_manage_ipmi_restore_fpga_card(const void *msg_data, const void *req_data_buf, gint32 *todo_flg)
{
    if ((req_data_buf == NULL || msg_data == NULL)) {
        debug_log(DLOG_ERROR, "%s:input param null.", __FUNCTION__);
        return RET_ERR;
    }

    PCIE_RESTORE_INFO *input = (PCIE_RESTORE_INFO *)g_malloc(sizeof(PCIE_RESTORE_INFO));
    if (input == NULL) {
        debug_log(DLOG_ERROR, "%s:malloc memory fail.", __FUNCTION__);
        return RET_ERR;
    }

    *input = *(const PCIE_RESTORE_INFO *)(req_data_buf);
    if (input->slot_id >= MAX_SLOT_COUNT) {
        debug_log(DLOG_ERROR, "%s:slot id: %d is out of range.", __FUNCTION__, input->slot_id);
        g_free(input);
        return RET_ERR;
    }
    errno_t safe_fun_ret = memcpy_s(&input->msg, sizeof(IPMI_MSG_S), msg_data, sizeof(IPMI_MSG_S));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
    input->op_type = RESTORE_FROM_IPMI;

    gint32 ret = prepare_to_restore_fpga_card(&g_fpga_fw_restore_status[input->slot_id], input, todo_flg);

    return ret;
}

LOCAL void pcie_card_update_mcu_version(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    gchar mcu_version[STR_FIRMWARE_LEN] = {0};
    guint8 buf[KEY_FIRM_VER_LEN] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_FIRMWARE;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_FIRM_VER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s : get mcu version failed, ret = %d", __FUNCTION__, ret);
        return;
    }

    if (buf[0] + buf[1] + buf[2] == 0) {
        debug_log(DLOG_DEBUG, "%s: mcu version is not valid, firmware is %u.%u.%u", __FUNCTION__, buf[0], buf[1],
            buf[2]);
        return;
    }
    (void)snprintf_s(mcu_version, STR_FIRMWARE_LEN, STR_FIRMWARE_LEN - 1, "%u.%u.%u", buf[0], buf[1], buf[2]);

    ret = dal_set_property_value_string(obj_handle, PROPERTY_FPGA_CARD_MCUVERSION, mcu_version, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: update pcie card mcu version failed", __FUNCTION__);
        return;
    }
}

LOCAL void pcie_card_update_mcu_reset_time(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
#define DEFAULT_MCU_RESET_TIME 10

    guint8 buf[KEY_CHIP_RESET_TIME] = {0};
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__);
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_CHIP_RESET_TIME;
    gint32 ret = pm_get_fixed_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, buf, KEY_CHIP_RESET_TIME);
    if (ret == RET_OK) {
        pm_chip_info->chip_reset_time = ((buf[1] << 8) | buf[0]);
    } else {
        pm_chip_info->chip_reset_time = DEFAULT_MCU_RESET_TIME;
        debug_log(DLOG_DEBUG, "%s : get mcu reset time failed, use default time(%ds), ret = %d.", __FUNCTION__,
            DEFAULT_MCU_RESET_TIME, ret);
    }
}

LOCAL void pcie_card_update_rtc_time_to_mcu(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    struct tm t_tm;
    PROTOCOL_HEAD_PARA head_para = {0};
    guint8 set_data[7] = {0};

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__);
        return;
    }

    if (pm_chip_info->card_type != SDI_CARD) {
        debug_log(DLOG_DEBUG, "%s : not support", dfl_get_object_name(obj_handle));
        return;
    }

    (void)memset_s(&t_tm, sizeof(struct tm), 0, sizeof(struct tm));
    gint32 ret = dal_localtime_r(NULL, &t_tm);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get time fail!", __FUNCTION__);
        return;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_SET_RTC_TIME;

    
    set_data[0] = t_tm.tm_sec;
    set_data[1] = t_tm.tm_min;
    set_data[2] = t_tm.tm_hour;
    set_data[3] = t_tm.tm_mday;
    set_data[4] = t_tm.tm_mon + 1;
    set_data[5] = t_tm.tm_year - 100;   // BMC向MCU同步的实际年份从2000起始，故此处减去100
    set_data[6] = t_tm.tm_wday;

    ret = pm_send_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (const guint8 *)set_data, sizeof(set_data));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: sdi card obj(%s) set rtc time to mcu failed, ret is %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), ret);
        return;
    }

    return;
}


void pcie_card_update_mcu_rtc_time(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    gint16 time_offset = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__);
        return;
    }

    if (pm_chip_info->card_type == SDI_CARD) {
        return;
    }

    // 获取当前系统时间戳
    guint32 time_stamp = vos_get_cur_time_stamp();

    // 获取相对于GMT的时区偏移
    gint32 ret = dal_get_localtime_offset(&time_offset);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dal_get_localtime_offset fail, ret = %d", ret);
        return;
    }

    debug_log(DLOG_INFO, "%s: BMC system time is %u, zone offset is %d", __FUNCTION__, time_offset, time_stamp);

    // 计算带时区的时间戳值
    time_stamp = time_stamp + time_offset * 60;

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = REGISTER_KEY_SET_SYSTEM_TIME;

    debug_log(DLOG_INFO, "%s: BMC set mcu rtc time is %u", __FUNCTION__, time_stamp);

    ret =
        pm_send_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, (const guint8 *)&time_stamp, sizeof(time_stamp));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: card obj(%s) set data to mcu failed, mcu rtc time is %u, ret is %d", __FUNCTION__,
            dfl_get_object_name(obj_handle), time_stamp, ret);
        return;
    }

    return;
}


LOCAL void pcie_card_update_sdi_slot(OBJ_HANDLE slave_obj_handle, PM_CHIP_INFO* pm_chip_info)
{
    guint8 pcie_slot = 0;
    PROTOCOL_HEAD_PARA head_para = {0};

    if (pm_chip_info->card_type != SDI_CARD) {
        debug_log(DLOG_DEBUG, "%s : not support", dfl_get_object_name(slave_obj_handle));
        return;
    }
    // 查询pcie槽位号
    gint32 retv = dal_get_property_value_byte(slave_obj_handle, PROPERTY_SDI_CARD_SLOT, &pcie_slot);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get sdi card slot failed, retv:%d", __FUNCTION__, retv);
        return;
    }

    // 向mcu设置pcie槽位
    head_para.opcode = REGISTER_KEY_SET_CARD_SLOT;
    head_para.input_data_len = 0;
    guint16 data_lenth = 1;
    retv = pm_send_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, &pcie_slot, data_lenth);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set pcie slot failed, retv=%d", __FUNCTION__, retv);
        return;
    }
    
    debug_log(DLOG_ERROR, "%s: set pcie slot %d to mcu successfully", __FUNCTION__, pcie_slot);
    return;
}


LOCAL gint32 pcie_card_get_random_data_from_mcu(PM_CHIP_INFO *pm_chip_info, guint16 opcode, guint8 cmd_arg,
    guint8 **output_data, guint32 *data_len)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };

    if (pm_chip_info == NULL || output_data == NULL || data_len == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(&head_para, sizeof(PROTOCOL_HEAD_PARA), 0, sizeof(PROTOCOL_HEAD_PARA));
    head_para.opcode = opcode;
    head_para.cmd_arg = cmd_arg;
    gint32 ret = pm_get_random_len_data_by_std_smbus(pm_chip_info, (guint8 *)&head_para, output_data, data_len);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card dimm info failed! ret = %d", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}
 
LOCAL void pcie_card_update_static_value(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    // 初始化PCIE卡信息以及MCU复位时间
    pcie_card_update_mcu_rtc_time(obj_handle, pm_chip_info);
    // SDI5x,只在初始化设置一次
    pcie_card_update_rtc_time_to_mcu(obj_handle, pm_chip_info);
    pcie_card_update_board_id(obj_handle, pm_chip_info);
    pcie_card_update_bom_id(obj_handle, pm_chip_info);
    pcie_card_update_pcb_ver(obj_handle, pm_chip_info);
    pcie_card_update_mcu_reset_time(obj_handle, pm_chip_info);
    pcie_card_update_npu_video_memory_band_width_memory(obj_handle, pm_chip_info);
    pcie_card_update_npu_power_capacity_watts(obj_handle, pm_chip_info);
}

LOCAL void pcie_card_update_funs(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    // 初始化PCIE卡信息静态信息
    pcie_card_update_static_value(obj_handle, pm_chip_info);
    // 初始化PCIE卡信息以及MCU复位时间
    pcie_card_update_mcu_rtc_time(obj_handle, pm_chip_info);
    // SDI5.x,只在初始化设置一次
    pcie_card_update_rtc_time_to_mcu(obj_handle, pm_chip_info);
    pcie_card_update_board_id(obj_handle, pm_chip_info);
    pcie_card_update_bom_id(obj_handle, pm_chip_info);
    pcie_card_update_pcb_ver(obj_handle, pm_chip_info);
    pcie_card_update_mcu_reset_time(obj_handle, pm_chip_info);
}


gint32 pcie_mgnt_init_info_by_std_smbus(OBJ_HANDLE obj_handle, PM_CHIP_INFO *pm_chip_info)
{
    OBJ_HANDLE slave_obj_handle = 0;
    gchar slave_card_class_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE ref_handle = 0;

    if (pm_chip_info == NULL) {
        debug_log(DLOG_ERROR, "%s: pm_chip_info is null", __FUNCTION__);
        return RET_ERR;
    }

    // 获取refChip对象即mcu地址信息
    guint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REFCHIP, &ref_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get chip handle failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 获取MCU对象
    const gchar *ptr_obj_name = dfl_get_object_name(ref_handle);
    if (ptr_obj_name == NULL) {
        debug_log(DLOG_ERROR, "%s: failed to get object name", __FUNCTION__);
        return RET_ERR;
    }
    (void)strncpy_s(pm_chip_info->chip_name, sizeof(pm_chip_info->chip_name), ptr_obj_name,
        sizeof(pm_chip_info->chip_name) - 1);

    // 获取单帧长度
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PCIE_CARD_MAXFRAMELEN, &pm_chip_info->framelen);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card frame len failed, max frame len is %d", __FUNCTION__,
            pm_chip_info->framelen);
        return ret;
    }

    // 获取MCU的能力码
    if (pm_init(pm_chip_info) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card opcode failed, object name is %s", __FUNCTION__,
            dfl_get_object_name(obj_handle));
        return RET_ERR;
    }

    pcie_card_update_funs(obj_handle, pm_chip_info);

    // 获取PCIE卡芯片对象
    if ((ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name))) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get slave_card_class_name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (strlen(slave_card_class_name) != 0) {
        // 获取PCIE卡对应的中端PFGA卡对象句柄
        ret = dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get chip handle failed, ret=%d", __FUNCTION__, ret);
            return RET_ERR;
        }

        // 设置sdi的槽位号到mcu
        pcie_card_update_sdi_slot(slave_obj_handle, pm_chip_info);
        // 初始化卡的SN信息、芯片的FW版本以及MCU版本信息
        pcie_card_update_sn(slave_obj_handle, pm_chip_info);
        pcie_card_update_mcu_version(slave_obj_handle, pm_chip_info);
        pcie_card_update_firmware_version(slave_obj_handle, pm_chip_info);
        
        pcie_card_init_dimm_info(obj_handle, pm_chip_info);
        
    }

    return RET_OK;
}


LOCAL void mcu_action_operation_log(const void *msg_data, gint32 operate_result, guint8 slot_num, guint8 device_num,
    guint8 oprate_type)
{
    
    if (msg_data == NULL) {
        return;
    }
    if (oprate_type == MCU_ACTION_RESET_CHIP || oprate_type == ACTION_RESET_CHIP) {
        if (device_num == 0) {
            ipmi_operation_log(msg_data, "Reset all chips of PCIe card %u %s", slot_num,
                operate_result == RET_OK ? "successfully" : "failed");
        } else {
            ipmi_operation_log(msg_data, "Reset chip%u of PCIe card %u %s", device_num, slot_num,
                operate_result == RET_OK ? "successfully" : "failed");
        }
    } else if (oprate_type == MCU_ACTION_RESET_MCU) {
        ipmi_operation_log(msg_data, "Reset MCU of PCIe card %u %s", slot_num,
            operate_result == RET_OK ? "successfully" : "failed");
    }

    return;
}

void update_reset_mcu_state(PM_CHIP_INFO *chip_info)
{
    guint16 current_tick = 0;
    PROTOCOL_HEAD_PARA head_para = { 0 };
    guchar mcu_version[KEY_FIRM_VER_LEN] = {0};

    if (chip_info == NULL) {
        return;
    }
    // 超时时间设置为芯片复位时间
    guint16 reset_timeout_val = chip_info->chip_reset_time;

    chip_info->reset_mcu_state = RESETING_MCU;
    head_para.opcode = REGISTER_KEY_FIRMWARE;

    while (1) {
        // 延时 1s，查询固件版本，成功查询到了认为复位成功，超时认为失败
        if (current_tick >= reset_timeout_val) {
            chip_info->reset_mcu_state = RESET_MCU_FAILED;
            debug_log(DLOG_ERROR, "%s : get mcu version timeout, current_tick:%d, reset_timeout_val:%d", __FUNCTION__,
                current_tick, reset_timeout_val);
            break;
        }
        vos_task_delay(1000);
        current_tick++;

        gint32 retval = pm_get_fixed_len_data_by_std_smbus(chip_info,
            (guint8 *)&head_para, mcu_version, KEY_FIRM_VER_LEN);
        if (retval != RET_OK || mcu_version[0] + mcu_version[1] + mcu_version[2] == 0) {
            debug_log(DLOG_INFO, "%s : get mcu version failed, ret = %d", __FUNCTION__, retval);
        } else {
            chip_info->reset_mcu_state = RESET_MCU_SUCCESSFUL;
            debug_log(DLOG_ERROR, "%s: mcu fw: %d, %d, %d", __FUNCTION__, mcu_version[0], mcu_version[1],
                mcu_version[2]);
            break;
        }
    }
    chip_info->reset_mcu_task_id = 0;
    pm_unref_binded_data((gpointer)chip_info);
}

LOCAL gint32 __ipmi_mcu_set_device_action(guint8 oprate_type, guint8 *resp_buf, gsize resp_buf_len,
    guint32 manufactureid, OBJ_HANDLE obj_handle, guint8 device_num)
{
    // 若resp_buf_len小于4，数组会访问越界
    if (resp_buf_len < 4) {
        debug_log(DLOG_ERROR, "%s: wrong parameter", __FUNCTION__);
        return RET_ERR;
    }

    if (oprate_type == MCU_ACTION_RESET_CHIP || oprate_type == MCU_ACTION_RESET_MCU_GPIO) {
        resp_buf[0] = COMP_CODE_SUCCESS;
        resp_buf[1] = LONGB0(manufactureid);
        resp_buf[2] = LONGB1(manufactureid);
        resp_buf[3] = LONGB2(manufactureid);

        return ipmi_mcu_reset_chip(obj_handle, device_num, oprate_type);
    } else if (oprate_type == MCU_ACTION_RESET_MCU) {
        return ipmi_mcu_reset_mcu(obj_handle, device_num);
    }

    return RET_OK;
}



gint32 card_manage_ipmi_mcu_set_device_action(const void *msg_data, gpointer user_data)
{
    OBJ_HANDLE obj_handle = 0;
    guint8  resp_buf[RMCP_IPMI_BUF_SIZE] = {0};
    PCIE_RESTORE_INFO data_buf;
    gint32 todo_flg = 0;

    (void)memset_s(&data_buf, sizeof(PCIE_RESTORE_INFO), 0, sizeof(PCIE_RESTORE_INFO));
    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    TARGET_TYPE_E target_type = get_ipmi_target_type(msg_data);
    if ((target_type != IPMI_HOST) && (target_type != 0x9)) { 
        debug_log(DLOG_ERROR, "action target_type:%d(Should be %d) invalid!", target_type, IPMI_HOST);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    const guint8 *req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint32 manufactureid = MAKE_DWORD(0, req_data_buf[2], req_data_buf[1], req_data_buf[0]);
    if (manufactureid != HUAWEI_MFG_ID) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    guint32 psosition = MAKE_DWORD(req_data_buf[4], req_data_buf[5], req_data_buf[6], req_data_buf[7]);
    guint8 slot_id = req_data_buf[8];
    guint8 device_num = req_data_buf[9];
    guint8 oprate_type = req_data_buf[10];

    
    data_buf.slot_id = slot_id;
    data_buf.position = psosition;

    (void)get_card_obj_handle_by_slot_position(&obj_handle, slot_id, psosition);

    if ((oprate_type == MCU_ACTION_RESET_CHIP || oprate_type == MCU_ACTION_RESET_MCU) && obj_handle == 0) {
        mcu_action_operation_log(msg_data, RET_ERR, slot_id, device_num, oprate_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (oprate_type == MCU_ACTION_RESET_CHIP || oprate_type == MCU_ACTION_RESET_MCU ||
        oprate_type == MCU_ACTION_RESET_MCU_GPIO) {
        gint32 ret = __ipmi_mcu_set_device_action(oprate_type, resp_buf, RMCP_IPMI_BUF_SIZE, manufactureid,
            obj_handle, device_num);
        if (ret == RET_OK) {
            mcu_action_operation_log(msg_data, ret, slot_id, device_num, oprate_type);
            return ipmi_send_response(msg_data, 4, resp_buf);
        }
        mcu_action_operation_log(msg_data, ret, slot_id, device_num, oprate_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    } else if (oprate_type == MCU_ACTION_FW_SELFHEALING) {
        
        if (device_num != 0) {
            ipmi_operation_log(msg_data, "The restore of the Golden firmware of the FPGA card %d is failed", slot_id);
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
        }

        (void)card_manage_ipmi_restore_fpga_card(msg_data, (void *)&data_buf, &todo_flg);
        if (todo_flg == 1) {
            ipmi_operation_log(msg_data, "The restore of the Golden firmware of the FPGA card %d is starting", slot_id);
            return ipmi_send_response(msg_data, 4, resp_buf);
        }
        ipmi_operation_log(msg_data, "Failed to restore the Golden firmware of the FPGA card %d", slot_id);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    } else if (oprate_type == ACTION_RESET_CHIP) {
        gint32 ret = ipmi_set_device_action(resp_buf, RMCP_IPMI_BUF_SIZE, manufactureid, obj_handle, device_num);
        if (ret == RET_OK) {
            mcu_action_operation_log(msg_data, ret, slot_id, device_num, oprate_type);
            return ipmi_send_response(msg_data, 4, resp_buf);  // 4表示响应长度
        }
        mcu_action_operation_log(msg_data, ret, slot_id, device_num, oprate_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_DES_UNAVAILABLE);
    }

    return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
}


LOCAL gint32 ipmi_mcu_get_reset_result(OBJ_HANDLE obj_handle, guint8 device_num, guint8 *result_data, guint32 *length,
    guint32 manufactureid)
{
    guint8 chip_number = 0;
    const guint32 resp_header_size = 4;
    errno_t securec_rv = EOK;

    if (result_data == NULL || length == NULL) {
        return RET_ERR;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_CHIP_NUMBER, &chip_number);
    if (ret != DFL_OK || chip_number == 0 || chip_number == INVALID_DATA_BYTE) {
        debug_log(DLOG_ERROR, "%s : get property failed, ret = %d, chip_number = %d", __FUNCTION__, ret, chip_number);
        return RET_ERR;
    }

    if (*length < chip_number + resp_header_size) {
        debug_log(DLOG_ERROR, "%s : length error, length = %d, chip_number = %d", __FUNCTION__, *length, chip_number);
        return RET_ERR;
    }

    guint8 *result = (guint8 *)g_malloc0(chip_number * sizeof(guint8));
    if (result == NULL) {
        debug_log(DLOG_ERROR, "%s : g_malloc0 failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = pcie_card_mcu_get_reset_result(obj_handle, device_num, chip_number, result);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : get reset result failed, ret = %d", __FUNCTION__, ret);
        g_free(result);
        return ret;
    }

    result_data[0] = COMP_CODE_SUCCESS;
    result_data[1] = LONGB0(manufactureid);
    result_data[2] = LONGB1(manufactureid);
    result_data[3] = LONGB2(manufactureid);

    if (device_num != 0) {
        result_data[4] = result[0];
        *length = resp_header_size + 1;
    } else {
        securec_rv = memcpy_s(&result_data[4], *length - resp_header_size, result, chip_number);
        if (securec_rv != EOK) {
            debug_log(DLOG_ERROR, "%s : memcpy_s failed, ret = %d", __FUNCTION__, securec_rv);
            g_free(result);
            return RET_ERR;
        }
        *length = resp_header_size + chip_number;
    }

    g_free(result);
    return RET_OK;
}


gint32 card_manage_ipmi_mcu_get_device_action_result(const void *msg_data, gpointer user_data)
{
    OBJ_HANDLE obj_handle = 0;
    guint8  resp_buf[RMCP_IPMI_BUF_SIZE] = {0};
    guint32 resp_buf_len = 0;
    gint32 ret = 0;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    TARGET_TYPE_E target_type = get_ipmi_target_type(msg_data);
    if (target_type != IPMI_HOST) {
        debug_log(DLOG_ERROR, "get action result target_type:%d(Should be %d) invalid!", target_type, IPMI_HOST);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    const guint8 *req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    guint32 manufactureid = MAKE_DWORD(0, req_data_buf[2], req_data_buf[1], req_data_buf[0]);
    if (manufactureid != HUAWEI_MFG_ID) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    guint32 psosition = MAKE_DWORD(req_data_buf[4], req_data_buf[5], req_data_buf[6], req_data_buf[7]);
    guint8 slot_id = req_data_buf[8];
    guint8 device_num = req_data_buf[9];
    guint8 oprate_type = req_data_buf[10];

    if (oprate_type == MCU_ACTION_RESET_CHIP || oprate_type == MCU_ACTION_RESET_MCU) {
        (void)get_card_obj_handle_by_slot_position(&obj_handle, slot_id, psosition);
        if (obj_handle == 0) {
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
        }

        resp_buf_len = sizeof(resp_buf);
        if (oprate_type == MCU_ACTION_RESET_CHIP) {
            ret = ipmi_mcu_get_reset_result(obj_handle, device_num, resp_buf, &resp_buf_len, manufactureid);
        } else if (oprate_type == MCU_ACTION_RESET_MCU) {
            ret = ipmi_mcu_get_reset_mcu_result(obj_handle, device_num, resp_buf, &resp_buf_len, manufactureid);
        }
        if (ret == RET_OK) {
            return ipmi_send_response(msg_data, resp_buf_len, resp_buf);
        } else {
            return ipmi_send_simple_response(msg_data, COMP_CODE_CANNOT_RESPONSE);
        }
    } else if (oprate_type == MCU_ACTION_FW_SELFHEALING) {
        
        if (device_num != 0) {
            ipmi_operation_log(msg_data, "The restore of the Golden firmware of the FPGA card %d is failed", slot_id);
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
        }

        if (slot_id >= MAX_SLOT_COUNT) {
            debug_log(DLOG_ERROR, "%s: Invalid parameter", __FUNCTION__);
            return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
        }

        resp_buf[0] = COMP_CODE_SUCCESS;
        resp_buf[1] = LONGB0(manufactureid);
        resp_buf[2] = LONGB1(manufactureid);
        resp_buf[3] = LONGB2(manufactureid);
        resp_buf[4] = g_fpga_fw_restore_status[slot_id].status;
        resp_buf[5] = g_fpga_fw_restore_status[slot_id].progress;

        return ipmi_send_response(msg_data, 6, resp_buf);
        
    }

    return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
}

LOCAL guint8 __insert_npu_bdf_list(const BDF_VERIFICATION_INFO *npu_bdf_check_info, guint16 segment, guint8 socket,
    guint8 *found_npu_num, GSList **npu_bdf_str_list)
{
    gchar bdf_str[MAX_BDF_STR_LEN] = {0};
    PCIE_VIDDID_SVIDSDID pcie_viddid = { 0 };
    gint32 ret = pciecard_test_bdf_is_valid(socket, npu_bdf_check_info->bus_index, npu_bdf_check_info->dev_index,
        npu_bdf_check_info->fun_index, &pcie_viddid);
    if (ret == RET_ERR) {
        
        if (npu_bdf_check_info->fun_index == 0) {
            return DEVICE_BREAK_FLAG;
        }
        return FUNCTION_BREAK_FLAG;
    } else if ((pcie_viddid.vid == npu_bdf_check_info->vid) && (pcie_viddid.did == npu_bdf_check_info->did)) {
        (*found_npu_num)++;
        
        (void)memset_s(bdf_str, MAX_BDF_STR_LEN, 0, MAX_BDF_STR_LEN);
        (void)snprintf_s(bdf_str, MAX_BDF_STR_LEN, MAX_BDF_STR_LEN - 1, "%04x:%02x:%02x.%01x", segment,
            npu_bdf_check_info->bus_index, npu_bdf_check_info->dev_index, npu_bdf_check_info->fun_index);
        *npu_bdf_str_list = g_slist_append(*npu_bdf_str_list, g_variant_new_string(bdf_str));
        if (*found_npu_num == npu_bdf_check_info->expected_num) {
            debug_log(DLOG_DEBUG, "find all expected bdf number(%d).", *found_npu_num);
            return DEVICE_BREAK_FLAG;
        }
    }
    return NO_BREAK_FLAG;
}

LOCAL void __find_npus_bdf_per_pciecard(GSList **npu_bdf_str_list, BDF_VERIFICATION_INFO *npu_bdf_check_info,
    guint8 *found_npu_num, guint16 segment, guint8 socket)
{
    guint8 fun_index;
    guint8 dev_index = 0;
    guint8 ret;
    for (; dev_index <= 31; dev_index++) { // Device:[0,31]
        npu_bdf_check_info->dev_index = dev_index;
        for (fun_index = 0; fun_index <= 7; fun_index++) { // Function:[0,7]
            npu_bdf_check_info->fun_index = fun_index;
            debug_log(DLOG_INFO, "bus_index:%02x,dev_index:%02x,fun_index:%02x", npu_bdf_check_info->bus_index,
                dev_index, fun_index);
            ret = __insert_npu_bdf_list(npu_bdf_check_info, segment, socket, found_npu_num, npu_bdf_str_list);
            if (ret == DEVICE_BREAK_FLAG) {
                return;
            } else if (ret == FUNCTION_BREAK_FLAG) {
                break;
            }
        }
    }
    return;
}

LOCAL gint32 __get_npu_bdf_list(OBJ_HANDLE pcieaddr_handle, OBJ_HANDLE pciecard_handle, guint8 *found_npu_num,
    GSList **npu_bdf_str_list)
{
    
    guint16 segment = 0;
    guint8 socket = 0;
    guint8 secbus = 0;
    guint8 subbus = 0;
    guint8 bus_index;
    BDF_VERIFICATION_INFO npu_bdf_check_info = { 0 };
    
    (void)dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SOCKET_ID, &socket);
    (void)dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SEC_BUS, &secbus);
    (void)dal_get_property_value_byte(pcieaddr_handle, PROPERTY_SUB_BUS, &subbus);

    (void)dal_get_property_value_uint16(pciecard_handle, PROPERTY_PCIE_CARD_VID, &(npu_bdf_check_info.vid));
    (void)dal_get_property_value_uint16(pciecard_handle, PROPERTY_PCIE_CARD_DID, &(npu_bdf_check_info.did));
    (void)dal_get_property_value_byte(pciecard_handle, PROPERTY_PCIE_CARD_CHIP_NUMBER,
        &(npu_bdf_check_info.expected_num));

    
    for (bus_index = secbus; bus_index <= subbus; bus_index++) {
        npu_bdf_check_info.bus_index = bus_index;
        __find_npus_bdf_per_pciecard(npu_bdf_str_list, &npu_bdf_check_info, found_npu_num, segment, socket);
        if (*found_npu_num == npu_bdf_check_info.expected_num) {
            return RET_OK;
        }
    }
    debug_log(DLOG_ERROR, "%s: Find %s BDF failed with found_npu_num: %d and expected num %d.", __FUNCTION__,
        dfl_get_object_name(pciecard_handle), *found_npu_num, npu_bdf_check_info.expected_num);
    return RET_ERR;
}

LOCAL gint32 __set_pciecard_npu_bdf(GSList *npu_chip_list, GSList *npu_bdf_str_list, guint8 bdf_list_len)
{
    OBJ_HANDLE npu_handle = 0;
    GSList *npu_obj_node = NULL;
    guint8 npu_id = 0;
    guint8 presence = 0;
    GSList *input_list = NULL;
    const gchar *bdf_str_ptr = NULL;
    for (npu_obj_node = npu_chip_list; npu_obj_node; npu_obj_node = npu_obj_node->next) {
        npu_handle = (OBJ_HANDLE)npu_obj_node->data;
        (void)dal_get_property_value_byte(npu_handle, PROPERTY_NPU_PRESENCE, &presence);
        if (presence == 1) {
            (void)dal_get_property_value_byte(npu_handle, PROPERTY_NPU_ID, &npu_id);
            if (npu_id < 1 || npu_id > bdf_list_len) {
                debug_log(DLOG_ERROR, "%s: Invalid NPU id %d.", __FUNCTION__, npu_id);
                break;
            }
            bdf_str_ptr = g_variant_get_string((GVariant *)g_slist_nth_data(npu_bdf_str_list, npu_id - 1), NULL);
            if (bdf_str_ptr == NULL) {
                debug_log(DLOG_ERROR, "%s: Find npu(%s) bdf failed in list", __FUNCTION__,
                    dfl_get_object_name(npu_handle));
                continue;
            }
            input_list = g_slist_append(input_list, g_variant_new_string(bdf_str_ptr));
            gint32 ret = dfl_call_class_method(npu_handle, METHOD_SET_NPU_BDF, NULL, input_list, NULL);
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
            input_list = NULL;
            bdf_str_ptr = NULL;
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Set %s BDF failed, ret = %d.", __FUNCTION__, dfl_get_object_name(npu_handle),
                    ret);
            }
        }
    }
    return RET_OK;
}

LOCAL gint32 __pciecard_npu_bdf_info_update(GSList *npu_chip_list, OBJ_HANDLE pcieaddr_handle,
    OBJ_HANDLE pciecard_handle)
{
    GSList *npu_bdf_str_list = NULL;
    guint8 found_npu_num = 0; // 一张NPU卡可以最终通过和ME或者IMU交换可以找到BDF的NPU芯片数量
    
    gint32 ret = __get_npu_bdf_list(pcieaddr_handle, pciecard_handle, &found_npu_num, &npu_bdf_str_list);
    if (ret == RET_OK) {
        ret = __set_pciecard_npu_bdf(npu_chip_list, npu_bdf_str_list, found_npu_num);
    } else {
        debug_log(DLOG_ERROR, "%s: update %s NPU BDF failed", __FUNCTION__, dfl_get_object_name(pciecard_handle));
    }
    g_slist_free_full(npu_bdf_str_list, (GDestroyNotify)g_variant_unref);
    return ret;
}


LOCAL gint32 __update_npucard_bdf_info(OBJ_HANDLE fpga_handle, gpointer user_data)
{
    OBJ_HANDLE pcieaddr_handle = 0;
    GSList *npu_chip_list = NULL;
    OBJ_HANDLE pciecard_handle = 0;

    gint32 ret = dal_get_object_list_position(fpga_handle, CLASS_NPU, &npu_chip_list);
    if (npu_chip_list == NULL) {
        debug_log(DLOG_DEBUG, "%s: find %s npu_chip_list failed.", __FUNCTION__, dfl_get_object_name(fpga_handle));
        return RET_OK;
    }
    
    ret = dal_get_specific_object_position(fpga_handle, CLASS_PCIE_CARD, &pciecard_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: find %s related pciecard handle failed.", __FUNCTION__,
            dfl_get_object_name(fpga_handle));
        g_slist_free(npu_chip_list);
        return RET_OK;
    }
    ret = pcie_find_netcard_related_pcieaddrinfo_handle(pciecard_handle, 0, &pcieaddr_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: find %s related pcieaddrinfo handle failed.", __FUNCTION__,
            dfl_get_object_name(pciecard_handle));
        g_slist_free(npu_chip_list);
        return RET_OK;
    }

    ret = __pciecard_npu_bdf_info_update(npu_chip_list, pcieaddr_handle, pciecard_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update %s Related NPU Chip BDF failed.", __FUNCTION__,
            dfl_get_object_name(fpga_handle));
    }
    g_slist_free(npu_chip_list);
    return RET_OK;
}

void update_all_npus_bdf_task(void)
{
    (void)dfl_foreach_object(CLASS_PCIE_FPGA_CARD, __update_npucard_bdf_info, NULL, NULL);
}