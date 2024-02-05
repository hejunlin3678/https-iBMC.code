

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "retimer_manage.h"
#include "card_manage_module.h"
#include "retimer_driver.h"
#include "retimer_dft.h"

#define RETIMER_LOG_CYCLE  0
#define RETIMER_LOG_DUMP   1
#define RETIMER_MAX_NUM 128
#define PCIE_LINK_UP_OFFSET 6
#define PCIE_LINK_SPEED_OFFSET 7
#define PCIE_LINK_WIDTH_OFFSET 8

LOCAL gint32 create_retimer_bind_data(OBJ_HANDLE retimer, gpointer user_data);
LOCAL gint32 __retimer_get_logs_initial(OBJ_HANDLE retimer, gpointer user_data);
LOCAL gint32 retimer_heartbeat_check(OBJ_HANDLE obj_handle);
LOCAL gint32 retimer_refresh_version(OBJ_HANDLE obj_handle);

#define INVALID_SLOT_ID 0xff
#define RESP_DATA_STATUS 0
#define RESP_DATA_ERRNUM 1
#define RESP_DATA_INDEX2 2
#define BOARD_TYPE_MAINBOARD 0
#define BOARD_TYPE_EXPANDBOARD 1
#define COMPONENT_LOCATION_MAIN_BOARD "mainboard"

typedef struct tagSlotGetRetimerReq {
    guint8 opcode;
    guint8 subcmd;
    guint8 chip_type;
    guint8 board_type;
} SlotGetRetimerReq;

// 升级完毕后，是否有业务上下电的标记，如果有，新版本生效，刷新版本号。
guchar g_retimer_valid_poweron_flg = 0;
LOCAL guint8 g_retimer_log_power_off_flg = 0;
LOCAL guint8 g_retimer_log_sys_reset_flg = 0;

LOCAL gint32 g_retimer_scan_flg = 0xff;

OPT_CDR_ADAPT g_gOptCdrAdapt[] = {
    {"0d", "0c", 0xffff, OPT_40G,  CHANNEL_SPEED_10G, {0, -2, 51, -10, 0}},
    {"11", "0c", 0xffff, OPT_100G, CHANNEL_SPEED_25G, {0, -2, 51, -10, 0}},
    {"11", "23", 0xffff, AOC_100G, CHANNEL_SPEED_25G, {0, -2, 51, -10, 0}},
    {"11", "23", 0x1,    DAC_100G, CHANNEL_SPEED_25G, {0, -6, 51, -6,  0}},
    {"11", "23", 0x3,    DAC_100G, CHANNEL_SPEED_25G, {0, -6, 57,  0,  0}},
    {"11", "23", 0x5,    DAC_100G, CHANNEL_SPEED_25G, {0, -6, 57,  0,  0}},
    {"  ", "  ", 0xffff, OPT_BUTT, CHANNEL_SPEED_25G, {0, -2, 51, -10, 0}},
};

OPT_CDR_ADAPT g_OptCdrDs280[] = {
    {"0d", "0c", 0xffff, OPT_40G,  0x00, {0, -2, 16, 0, 0}},
    {"11", "0c", 0xffff, OPT_100G, CHANNEL_SPEED_DS280, {0, -2, 16, 0, 0}},
    {"11", "23", 0x1,    DAC_100G, CHANNEL_SPEED_DS280, {0, -4, 22, 0, 0}},
    {"11", "23", 0x3,    DAC_100G, CHANNEL_SPEED_DS280, {0, -4, 22, 0, 0}},
    {"11", "23", 0x5,    DAC_100G, CHANNEL_SPEED_DS280, {0, -4, 22, 0, 0}},
    {"  ", "  ", 0xffff, OPT_BUTT, CHANNEL_SPEED_DS280, {0, -2, 16, 0, 0}},
};


void notify_retimer_power_state(guint8 power_state)
{
    debug_log(DLOG_DEBUG, "%s: receive power state(%d) change event!", __FUNCTION__, power_state);
    if (power_state == 0) {
        g_retimer_log_power_off_flg = 1;
    }
}


void notify_retimer_sys_reset_state(guint8 sys_reset_state)
{
    debug_log(DLOG_DEBUG, "%s: receive sys reset state(%d) change event!", __FUNCTION__, sys_reset_state);
    if (sys_reset_state == 1) {
        g_retimer_log_sys_reset_flg = 1;
    }
}


gint32 retimer_channel_switch_by_handle(OBJ_HANDLE object_handle, guchar channel_switch)
{
    gchar accessor_name[MAX_OBJECT_NAME_LEN] = {0};

    gint32 ret =
        dfl_get_property_accessor(object_handle, PROPERTY_RETIMER_ACCESS_SWITCH, accessor_name, sizeof(accessor_name));
    
    if (ret == DFL_OK) {
        if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
            
            channel_switch = (channel_switch != RETIMER_CHN_OPEN) ? 0 : 1;
        }

        ret = dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_ACCESS_SWITCH, channel_switch, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Set channel switch failed(ret: %d).", __FUNCTION__, ret);
            return ret;
        }
    }
    return RET_OK;
}


gint32 retimer_set_upg_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_value = NULL;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "input list is NULL.");
        return RET_ERR;
    }

    property_value = (GVariant *)g_slist_nth_data(input_list, 0);
    guint8 upg_status = g_variant_get_byte(property_value);

    gint32 ret = dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_UPG_STATE, upg_status, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:retimer_set_upg_status failed(ret: %d).", __FUNCTION__, ret);
    }

    g_retimer_valid_poweron_flg = 0;
    return ret;
}


LOCAL gint32 get_netcard_slotid(OBJ_HANDLE obj_handle, guint8 board_type, guint8 *slot_id)
{
    guint8 upg_result = 0;
    guint8 chip_type = 0;

    gchar buffer[OBJ_NAME_LENGTH + 1] = {0};
    OBJ_HANDLE netcard_handle = 0;
    OBJ_HANDLE obj_com_handle = 0;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_UPG_RESULT, &upg_result);
    gint32 retv = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_CHIP_TYPE, &chip_type);
    if (ret != RET_OK || retv != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get property failed.", __FUNCTION__);
        return RET_ERR;
    }

    if ((upg_result == RETIMER_UPG_RESULT_ERR) && (chip_type == RETIMER_TYPE_5902L)) {
        
        ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REF_COMPONET, &obj_com_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get PROPERTY_RETIMER_REF_COMPONET property failed(ret: %d)", __FUNCTION__, ret);
            return RET_OK;
        }
        ret =
            dal_get_property_value_string(obj_com_handle, PROPERTY_COMPONENT_LOCATION, buffer, sizeof(buffer));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Get PROPERTY_COMPONENT_LOCATION property failed(ret: %d)", __FUNCTION__, ret);
            return RET_ERR;
        }
        if (((board_type == BOARD_TYPE_MAINBOARD) && (g_strcmp0(buffer, COMPONENT_LOCATION_MAIN_BOARD) == 0)) ||
            ((board_type == BOARD_TYPE_EXPANDBOARD) && (g_strcmp0(buffer, COMPONENT_LOCATION_MAIN_BOARD) != 0))) {
            ret = dal_get_specific_object_position(obj_handle, CLASS_NETCARD_NAME, &netcard_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:Get CLASS_NETCARD_NAME failed(ret: %d)", __FUNCTION__, ret);
                return RET_ERR;
            }
            ret = dal_get_property_value_byte(netcard_handle, PROPERTY_NETCARD_SLOT_ID, slot_id);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:Get PROPERTY_NETCARD_SLOT_ID failed(ret: %d)", __FUNCTION__, ret);
                return RET_ERR;
            }
        }
    }
    return RET_OK;
}


gint32 ipmi_retimer_get_upgrade_err_slot(const void *msg_data, gpointer user_data)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;

    const  SlotGetRetimerReq *p_retimer_slot = NULL;
    guint8 resp_data[RESP_MSG_MAX_DATA_LEN] = {0};
    guint8 errnum = 0;

    
    p_retimer_slot = (const SlotGetRetimerReq *)get_ipmi_src_data(msg_data);
    if (p_retimer_slot == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    
    if ((p_retimer_slot->chip_type != 0) ||
        (p_retimer_slot->board_type != BOARD_TYPE_MAINBOARD && p_retimer_slot->board_type != BOARD_TYPE_EXPANDBOARD)) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    gint32 ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s:dfl_get_object_list failed(ret: %d)", __FUNCTION__, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_STATUS_INVALID);
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        guint8 slot_id = INVALID_SLOT_ID;
        ret = get_netcard_slotid(obj_handle, p_retimer_slot->board_type, &slot_id);
        if (ret == RET_ERR) {
            g_slist_free(obj_list);
            return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
        }
        if ((slot_id !=INVALID_SLOT_ID) && ((RESP_DATA_INDEX2 + errnum) <= RESP_MSG_MAX_DATA_LEN)) {
            resp_data[RESP_DATA_INDEX2 + errnum] = slot_id;
            errnum++;
        }
    }

    g_slist_free(obj_list);
    resp_data[RESP_DATA_STATUS] = COMP_CODE_SUCCESS;
    resp_data[RESP_DATA_ERRNUM] = errnum;
    return ipmi_send_response(msg_data, RESP_DATA_INDEX2 + errnum, resp_data);
}


guint8 retimer_get_all_status_ready(void)
{
    GSList *handle_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_RETIMER_NAME, &handle_list);
    if (ret != DFL_OK) {
        return RETIMER_NOT_READY;
    }
    guint8 tot_status = RETIMER_HAS_READY;
    for (GSList *list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        OBJ_HANDLE obj = (OBJ_HANDLE)list_item->data;
        if (get_retimer_status_ready(obj) == RETIMER_NOT_READY) {
            tot_status = RETIMER_NOT_READY;
            debug_log(DLOG_DEBUG, "Retimer(%s) is not ready", dfl_get_object_name(obj));
            break;
        }
    }
    g_slist_free(handle_list);
    return tot_status;
}


LOCAL gint32 retimer_get_temperature_by_handle(OBJ_HANDLE object_handle)
{
#define I8_MAX 127
#define I8_MIN (-128)
#define INVALID_CHIP_TYPE 0xFF

    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint16 temp_monsenvalue = 0;
    gint16 temp_data = SENSOR_INVALID_READING;
    guchar chip_type = INVALID_CHIP_TYPE;

    (void)dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &obj_handle);
    if (get_retimer_status_ready(object_handle) != RETIMER_HAS_READY) {
        (void)dal_set_property_value_uint16(object_handle, PROPERTY_RETIMER_TEMPERATURE, SENSOR_NA_READING, DF_NONE);
        return RET_OK;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_RETIMER_CHIP_TYPE, &chip_type);

    if (chip_type == RETIMER_TYPE_5902L) {
        ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
        if (ret != RET_OK) {
            return RET_ERR;
        }
        ret = dal_get_property_value_uint16(obj_handle, PROPERTY_PRODUCT_CDRMAXTEMP, &temp_monsenvalue);
        if (ret != RET_OK) {
            return RET_ERR;
        }

        // imu 获取到cdr温度。（包括正常值和错误值）
        if (temp_monsenvalue != SENSOR_NA_READING) {
            if (temp_monsenvalue == SENSOR_INVALID_READING_STATE) {
                temp_monsenvalue = SENSOR_NA_READING;
            }
            (void)dal_set_property_value_uint16(object_handle, PROPERTY_RETIMER_TEMPERATURE, temp_monsenvalue, DF_NONE);
            return RET_OK;
        }
    }

    // retimer走此流程 CDR如果当前imu不支持温度读取，也是走此流程
    ret = retimer_channel_switch_by_handle(object_handle, RETIMER_CHN_OPEN);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: retimer_channel_switch_by_handle open error, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = retimer_get_temperature(object_handle, &temp_data);
    if (ret != RET_OK) {
        if (get_retimer_status_ready(object_handle) == RETIMER_HAS_READY) {
            (void)dal_set_property_value_uint16(object_handle, PROPERTY_RETIMER_TEMPERATURE, SENSOR_INVALID_READING,
                DF_NONE);
        }
        (void)retimer_channel_switch_by_handle(object_handle, RETIMER_CHN_CLOSE);
        return RET_ERR;
    }

    if (temp_data > I8_MAX) {
        temp_data = I8_MAX;
    } else if (temp_data < I8_MIN) {
        temp_data = I8_MIN;
    }
    temp_monsenvalue = (guint16)temp_data & 0xFF;

    (void)dal_set_property_value_uint16(object_handle, PROPERTY_RETIMER_TEMPERATURE, temp_monsenvalue, DF_NONE);
    ret = retimer_channel_switch_by_handle(object_handle, RETIMER_CHN_CLOSE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: retimer_channel_switch_by_handle close error, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 dft_5902_set_dieid_info_to_property(OBJ_HANDLE object_handle)
{
    gchar dieid_string[MAX_DIEID_LEN + 1] = {0};
    SMBUS_REQ req = {0};
    RETIMER_RESP_DATA_S resp_data = { 0 };
    guchar i = 0;
    gchar tmp[DIEID_SINGLE_LENGTH];
    guint32 invalid_count = 0;

    // 组查询帧
    req.lun = CDR5902_REQ_LUN;
    req.arg = SMBUS_I2C_ARG_VALUE;
    req.opcode = RETIMER_CMD_REG_READ;
    req.offset = DIEID_START_ADDR;
    req.length = DIEID_TOTAL_LENGTH;

    (void)memset_s(resp_data.data, sizeof(resp_data.data), 0, sizeof(resp_data.data));

    if (sizeof(resp_data) < req.length + SMBUS_RESP_HEAD) {
        return RET_ERR;
    }
    gint32 ret = retimer_get_info(object_handle, &req, 0, (gint8 *)&resp_data, BUFFER_MAX_LEN); 
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: retimer_get_info (DIEID) failed. ret: %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (i = 0; i < DIEID_TOTAL_NUM; i++) {
        if (resp_data.data[i] == INVALID_DATA_BYTE) {
            invalid_count++;
        }
        (void)memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
        if (i != (DIEID_TOTAL_NUM - 1)) {
            (void)snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%08x ", resp_data.data[i]);
        } else {
            (void)snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%08x", resp_data.data[i]);
        }
        ret = strcat_s(dieid_string, sizeof(dieid_string), tmp);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: strncat_s fail, ret = %d.", __FUNCTION__, ret);
            return RET_ERR;
        }
    }
    if (invalid_count == DIEID_TOTAL_NUM) {
        debug_log(DLOG_DEBUG, "%s: invalid dieid, %s", __FUNCTION__, dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    ret = dal_set_property_value_string(object_handle, PROPERTY_RETIMER_DIEID, dieid_string, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: dal_set_property_value_string failed. ret: %d", __FUNCTION__, ret);
    }

    return ret;
}


LOCAL void retimer_property_init(OBJ_HANDLE obj_handle)
{
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_ADAPT_FLAG, RETIMER_ADAPT_FAILED, DF_SAVE_TEMPORARY);
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_CHANNEL_SPEED, CHANNEL_SPEED_BUTT,
        DF_SAVE_TEMPORARY);
    
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_UPG_STATE, RETIMER_5902_UPG_IDLE, DF_NONE);
    (void)dal_set_property_value_uint16(obj_handle, PROPERTY_RETIMER_TEMPERATURE, 0x4000, DF_NONE); 
}


LOCAL gint32 retimer_get_version_by_handle(OBJ_HANDLE obj_handle)
{
    gint32 ret = retimer_refresh_version(obj_handle);
    if (ret == RETIMER_ACCESS_FAIL) {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_COMMUNICATION_STATE, 1, DF_AUTO);
        debug_log(DLOG_DEBUG, "[%s] Failed to access retimer %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "[%s] retimer_refresh_version error %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_COMMUNICATION_STATE, 0, DF_AUTO);

    return ret;
}


LOCAL gint32 retimer_refresh_version(OBJ_HANDLE obj_handle)
{
    gint8 resp_data_buf[RSP_MAX_BUF_LEN] = {0};
    gchar version[RSP_MAX_BUF_LEN] = {0};

    gint32 ret = retimer_get_version(obj_handle, resp_data_buf, RSP_MAX_BUF_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] retimer_get_version failed, ret: %d", dfl_get_object_name(obj_handle), ret);
        return ret;
    }

    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        ret = snprintf_s(version, sizeof(version), sizeof(version) - 1, "%02d.%02d.%02d.%02d", resp_data_buf[0],
            resp_data_buf[1], resp_data_buf[2], resp_data_buf[3]); 
    } else {
        ret = snprintf_s(version, sizeof(version), sizeof(version) - 1, "%d.%d.%d.%d", resp_data_buf[0],
            resp_data_buf[1], resp_data_buf[2], resp_data_buf[3]); 
    }
    if (ret < 0) {
        debug_log(DLOG_ERROR, "Retimer[%s] version snprintf_s failed. ret = %d", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    ret = dal_set_property_value_string(obj_handle, PROPERTY_RETIMER_SOFT_VER, version, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_set_property_value_string failed. ret:%d", dfl_get_object_name(obj_handle), ret);
    }

    debug_log(DLOG_INFO, "update Retimer[%s] version: %s", dfl_get_object_name(obj_handle), version);
    return ret;
}


LOCAL gint32 retimer_reset_info_after_valid(OBJ_HANDLE object_handle)
{
    guchar upg_state = RETIMER_5902_UPG_IDLE;
    if (get_retimer_status_ready(object_handle) != RETIMER_HAS_READY) {
        g_retimer_valid_poweron_flg = 1;
        debug_log(DLOG_MASS, "Chip %s is not ready", dfl_get_object_name(object_handle));
        (void)dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_COMMUNICATION_STATE, 0, DF_AUTO);
        return RET_ERR;
    }

    if (g_retimer_valid_poweron_flg) {
        (void)dal_get_property_value_byte(object_handle, PROPERTY_RETIMER_UPG_STATE, &upg_state);
        if (upg_state == RETIMER_5902_UPG_DONE) { // 生效中
            (void)dal_set_property_value_byte(object_handle, PROPERTY_RETIMER_UPG_STATE, RETIMER_5902_UPG_IDLE,
                DF_SAVE_TEMPORARY);
            (void)dal_set_property_value_string(object_handle, PROPERTY_RETIMER_SOFT_VER, "\0", DF_SAVE_TEMPORARY);
            (void)dal_set_property_value_string(object_handle, PROPERTY_RETIMER_DIEID, "\0", DF_SAVE_TEMPORARY);
        }
    }

    return RET_OK;
}

LOCAL gint32 retimer_get_info_by_handle(OBJ_HANDLE object_handle)
{
    gchar dieid[MAX_DIEID_LEN + 1] = {0};

    gint32 ret = retimer_reset_info_after_valid(object_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = retimer_channel_switch_by_handle(object_handle, RETIMER_CHN_OPEN);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = retimer_get_version_by_handle(object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] retimer_get_version_by_handle failed, ret: %d", __FUNCTION__, ret);
        goto EXIT;
    }

    (void)dal_get_property_value_string(object_handle, PROPERTY_RETIMER_DIEID, dieid, sizeof(dieid));
    if (strlen(dieid) != 0) {
        debug_log(DLOG_DEBUG, "chip %s dieid ok: %s", dfl_get_object_name(object_handle), dieid);
        ret = RET_OK;
        goto EXIT;
    }

    ret = dft_5902_set_dieid_info_to_property(object_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] dft_5902_set_dieid_info_to_property failed, ret: %d", __FUNCTION__, ret);
        goto EXIT;
    }

EXIT:
    if (retimer_channel_switch_by_handle(object_handle, RETIMER_CHN_CLOSE) != RET_OK) {
        return RET_ERR;
    }
    return ret;
}


LOCAL gint32 retimer_get_pcie_status(OBJ_HANDLE retimer, guint8 core_id, guint8 port_id, LinkStatus *link_status)
{
    gint8 resp_buff[REQ_MAX_BUF_LEN] = {0};
    SMBUS_REQ req = {0};
    RetimerRespDataHead *resp = NULL;

    resp = (RetimerRespDataHead *)&resp_buff[0];

    req.lun = CDR5902_REQ_LUN;
    req.opcode = RETIMER_CMD_GET_PCIE_STATUS;
    req.data[0] = core_id;
    req.data[1] = (port_id == 1) ? 4 : port_id; 
    req.data[2] = 0; 

    gint32 ret = retimer_get_info(retimer, &req, 3, resp_buff, BUFFER_MAX_LEN);  
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: retimer smbus opcode(%u) failed, ret = %d", __FUNCTION__,
            RETIMER_CMD_GET_PCIE_STATUS, ret);
        return ret;
    }

    debug_log(DLOG_DEBUG, "%s: %s: core: %d, port: %d, link_status: %u, link_speed: %u, link_width: %u,",
        __FUNCTION__, dfl_get_object_name(retimer), core_id, port_id, resp->data[PCIE_LINK_UP_OFFSET],
        resp->data[PCIE_LINK_SPEED_OFFSET], resp->data[PCIE_LINK_WIDTH_OFFSET]);

    if (resp->data[PCIE_LINK_UP_OFFSET] == 0xff || resp->data[PCIE_LINK_SPEED_OFFSET] == 0xff ||
        resp->data[PCIE_LINK_WIDTH_OFFSET] == 0xff) {
        debug_log(DLOG_DEBUG, "%s: %s: core: %d, port: %d, link_status: 0xff, link_speed: 0xff, link_width: 0xff",
            __FUNCTION__, dfl_get_object_name(retimer), core_id, port_id);
        return RET_ERR;
    }

    link_status->link_status = resp->data[PCIE_LINK_UP_OFFSET];
    link_status->link_speed = resp->data[PCIE_LINK_SPEED_OFFSET];
    link_status->link_width = resp->data[PCIE_LINK_WIDTH_OFFSET];

    debug_log(DLOG_DEBUG, "%s: %s: core: %d, port: %d, link_status: %u, link_speed: %u, link_width: %u,",
        __FUNCTION__, dfl_get_object_name(retimer), core_id, port_id, link_status->link_status,
        link_status->link_speed, link_status->link_width);

    return RET_OK;
}


LOCAL gint32 rt_get_pcie_status_ok(OBJ_HANDLE retimer, guint8 port_id)
{
    LinkStatus pcie_status = {0};

    gint32 ret = retimer_get_pcie_status(retimer, 0, port_id, &pcie_status);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: retimer get port %d pcie status failed, ret = %d", __FUNCTION__, port_id, ret);
        return ret;
    }
    
    if (pcie_status.link_status == 0x01 && pcie_status.link_speed == 0x04 && pcie_status.link_width == 0x02) {
        return RET_OK;
    }

    return RET_ERR;
}


LOCAL gint32 __retimer_get_data_foreach(OBJ_HANDLE obj_handle, gpointer user_data)
{
    guchar is_upgrade = 0;
    guint8 ipmb_index = 0;

    if (!retimer_is_need_scan_task(obj_handle)) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "%s status is not need scan.", dfl_get_object_name(obj_handle));
        return RET_OK;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_IPMB_INDEX, &ipmb_index);
    if (ipmb_index != CDR_IPMB_INDEX_INVALID) {
        return RET_OK;
    }

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, &is_upgrade);
    if ((ret != RET_OK || is_upgrade == RETIMER_UPGRADING)) {
        dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_COMMUNICATION_STATE, 0, DF_AUTO);
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "[%s] is_upgrade %d %d ", dfl_get_object_name(obj_handle),
            is_upgrade, ret);
        return RET_OK;
    }

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_UPGRADING, DF_AUTO);
    if (ret != RET_OK) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "%s:Set channel notify %d failed(ret: %d).", __FUNCTION__,
            RETIMER_UPGRADING, ret);
        return RET_OK;
    }

    
    vos_task_delay(NOTIFY_DELAY_TIME);

    gint32 info_ret = retimer_get_info_by_handle(obj_handle);
    gint32 temp_ret = retimer_get_temperature_by_handle(obj_handle);
    if (!(info_ret == RET_OK && temp_ret == RET_OK)) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "%s: get info of %s failed, info_ret:%d, temp_ret:%d",
            __FUNCTION__, dfl_get_object_name(obj_handle), info_ret, temp_ret);
    }

    if ((dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL) == FALSE) ||
        
        ((rt_get_pcie_status_ok(obj_handle, 0) == RET_OK) && (rt_get_pcie_status_ok(obj_handle, 4) == RET_OK))) {
        retimer_disable_lane(obj_handle);
    }

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_IDLE, DF_AUTO);
    if (ret != RET_OK) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "%s:Set channel notify %d failed(ret: %d).", __FUNCTION__,
            RETIMER_IDLE, ret);
    }
    return RET_OK;
}

void retimer_release_i2c(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;

    (void)prctl(PR_SET_NAME, (unsigned long)"Retimer5902Task");
    gint32 ret = dfl_get_object_list(CLASS_RETIMER_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "[%s] dfl_get_object_list failed(ret: %d)", __FUNCTION__, ret);
        return;
    }
    // bmc重启复位i2c总线占用,
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        debug_log(DLOG_ERROR, "retimer(%s) i2c change to os begin.", dfl_get_object_name((OBJ_HANDLE)obj_node->data));
        ret = retimer_channel_switch_by_handle((OBJ_HANDLE)obj_node->data, RETIMER_CHN_CLOSE);
        ret += dal_set_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RETIMER_REQ_ACC, RETIMER_IDLE, DF_AUTO);
        debug_log(DLOG_ERROR, "retimer(%s) i2c change to os finished, ret=%d.",
            dfl_get_object_name((OBJ_HANDLE)obj_node->data), ret);
    }
    g_slist_free(obj_list);
}


LOCAL void retimer_get_data_task(void)
{
    (void)prctl(PR_SET_NAME, (unsigned long)"retimer_get_data");
    retimer_release_i2c();
    while (1) {
        vos_task_delay(RETIMER_DELAY);

        if (read_retimer_routine_open_status() != RETIMER_OPEN_ROUTINE) { 
            write_retimer_routine_open_status(RETIMER_ALREADY_CLOSE_ROUTINE);
            continue;
        }

        (void)dfl_foreach_object(CLASS_RETIMER_NAME, __retimer_get_data_foreach, NULL, NULL);
    }
}


LOCAL gint32 retimer_get_powerstate(guint8 *power_state)
{
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get payload handle failed ret=%d.", ret);
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, power_state);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get chassis property PowerState failed ret=%d.", ret);
    }

    return ret;
}


LOCAL gint32 is_loop_test_optical(OBJ_HANDLE obj_handle)
{
    OBJ_HANDLE optical_handle;
    gchar part_number[PART_NUMBER_LEN] = {0};

    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFOPTICAL, &optical_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Retimer %s get reference optical obj failed, ret(%d).",
            dfl_get_object_name(optical_handle), ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(optical_handle, PROPERTY_OPT_MDL_PART_NUMBER, part_number, sizeof(part_number));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Retimer %s get part number failed, ret(%d).", dfl_get_object_name(optical_handle), ret);
        return RET_ERR;
    }

    
    if ((strncmp(part_number, PART_NUMBER_HUAWEI, strlen(PART_NUMBER_HUAWEI)) != 0) &&
        (strncmp(part_number, PART_NUMBER_LIXIN, strlen(PART_NUMBER_LIXIN)) != 0)) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 retimer_real_adapt(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint32 tmp_addr[PRBS_ON_OFF_REG_NUM] = {0x22004304, 0x23004304}; 
    guint8 i;

    gint32 flag_loop_test = is_loop_test_optical(obj_handle);
    if (flag_loop_test != RET_OK) {
        
        for (i = 0; i < PRBS_ON_OFF_REG_NUM; i++) {
            ret = retimer_set_reg_info(obj_handle, tmp_addr[i], 0x4000);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "[%s] close prbs failed.", dfl_get_object_name(obj_handle));
                return ret;
            }
        }
    }

    
    ret = retimer_serdes_adapt(obj_handle, 0, RETIMER_LANE_NUM); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer %s serdes adapt failed! ret = %d", dfl_get_object_name(obj_handle), ret);
    }

    if (flag_loop_test != RET_OK) {
        
        for (i = 0; i < PRBS_ON_OFF_REG_NUM; i++) {
            ret += retimer_set_reg_info(obj_handle, tmp_addr[i], 0x0);
        }
    }

    return ret;
}


LOCAL void retimer_adapt_monitor(OBJ_HANDLE obj_handle)
{
    guint8 adpt_flag = RETIMER_ADAPT_FAILED;
    guint8 link_status = LINK_DOWN;

    
    gint32 ret = retimer_check_lane_loss(obj_handle);
    if (ret != RET_OK) {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_ADAPT_FLAG, RETIMER_ADAPT_FAILED,
            DF_SAVE_TEMPORARY);
        debug_log(DLOG_INFO, "Retimer %s lane is los", dfl_get_object_name(obj_handle));
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ADAPT_FLAG, &adpt_flag);
    (void)dal_get_extern_value_byte(obj_handle, PROPERTY_RETIMER_LINK_STATUS, &link_status, DF_AUTO);
    
    if (adpt_flag == RETIMER_ADAPT_SUCCEED && link_status == LINK_UP) {
        return;
    }

    ret = retimer_real_adapt(obj_handle);
    if (ret != RET_OK) {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_ADAPT_FLAG, RETIMER_ADAPT_FAILED,
            DF_SAVE_TEMPORARY);
        debug_log(DLOG_DEBUG, "[%s] real adapt failed.", dfl_get_object_name(obj_handle));
        return;
    }

    debug_log(DLOG_ERROR, "Retimer %s serdes adapt succeed, adpt_flag = %d, link_status = %d.",
        dfl_get_object_name(obj_handle), adpt_flag, link_status);

    
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_ADAPT_FLAG, RETIMER_ADAPT_SUCCEED,
        DF_SAVE_TEMPORARY);

    return;
}


LOCAL gint32 retimer_switch_speed(OBJ_HANDLE obj_handle, guint8 cdr_speed)
{
    gint32 ret;
    guint8 channel_num;
    gint32 ret_value = RET_OK;

    for (channel_num = 0; channel_num < RETIMER_LANE_NUM; channel_num++) {
        ret = retimer_set_speed(obj_handle, channel_num, SPEED_FAST_PI_MODE, cdr_speed);
        if (ret != RET_OK) {
            ret_value = RET_ERR;
            debug_log(DLOG_ERROR, "Retimer %s switch channel %d speed failed, ret(%d)", dfl_get_object_name(obj_handle),
                channel_num, ret);
        }
    }

    return ret_value;
}


LOCAL void retimer_speed_match_handle(OBJ_HANDLE obj_handle, guint8 opt_speed)
{
    guint8 cur_speed = CHANNEL_SPEED_BUTT;

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_CHANNEL_SPEED, &cur_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get current speed failed,ret(%d).", dfl_get_object_name(obj_handle), ret);
        return;
    }

    if (cur_speed == opt_speed) {
        return;
    }

    ret = retimer_switch_speed(obj_handle, opt_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Switch %s ChannelSpeed(%d) failed,ret(%d).", dfl_get_object_name(obj_handle), opt_speed,
            ret);
        return;
    }

    debug_log(DLOG_ERROR, "Retimer %s Switch ChannelSpeed(%d) succeed.", dfl_get_object_name(obj_handle), opt_speed);
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_CHANNEL_SPEED, opt_speed, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer %s update channel speed property failed,ret(%d).",
            dfl_get_object_name(obj_handle), ret);
    }

    return;
}


LOCAL gint32 retimer_txffe_check(OBJ_HANDLE obj_handle, CDR_TX_FFE *tx_ffe, guint8 start_channel, guint8 channel_cnt)
{
    guint8 i;
    gint32 ret;
    guint32 tmp_len = 0x10; 
    gint8 data_info[BUFFER_MAX_LEN] = {};
    CDR_TX_FFE *read_tx_ffe = NULL;

    if (tx_ffe == NULL) {
        debug_log(DLOG_ERROR, "Retimer %s tx_ffe is null.", dfl_get_object_name(obj_handle));
        return RET_OK;
    }

    for (i = 0; i < channel_cnt; i++) {
        ret = retimer_get_tx_ffe(obj_handle, start_channel, data_info, BUFFER_MAX_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Retimer %s get channel %d txffe failed ret=%d.", dfl_get_object_name(obj_handle),
                start_channel, ret);
            return ret;
        }

        read_tx_ffe = (CDR_TX_FFE *)(void *)&data_info[tmp_len];

        if (memcmp(tx_ffe, read_tx_ffe, sizeof(CDR_TX_FFE))) {
            debug_log(DLOG_ERROR,
                "Retimer %s channel %d expect txffe param, pre2(%d), pre1(%d), main0(%d), post1(%d) post2(%d)",
                dfl_get_object_name(obj_handle), start_channel, tx_ffe->pre2, tx_ffe->pre1, tx_ffe->main0,
                tx_ffe->post1, tx_ffe->post2);
            debug_log(DLOG_ERROR,
                "Retimer %s channel %d actual txffe param, pre2(%d), pre1(%d), main0(%d), post1(%d) post2(%d)",
                dfl_get_object_name(obj_handle), start_channel, read_tx_ffe->pre2, read_tx_ffe->pre1,
                read_tx_ffe->main0, read_tx_ffe->post1, read_tx_ffe->post2);
            return RET_ERR;
        }

        start_channel++;
    }

    return RET_OK;
}


LOCAL gint32 retimer_txffe_set(OBJ_HANDLE obj_handle, CDR_TX_FFE *tx_ffe, guint8 start_channel, guint8 channel_cnt)
{
    guint8 i;
    gint32 ret;
    gint8 data_info[BUFFER_MAX_LEN] = {0};
    gint32 ret_value = RET_OK;
    SET_TX_FFE_DEBUG set_tx_ffe = { 0 };

    set_tx_ffe.pre2 = tx_ffe->pre2;
    set_tx_ffe.pre1 = tx_ffe->pre1;
    set_tx_ffe.main0 = tx_ffe->main0;
    set_tx_ffe.post1 = tx_ffe->post1;
    set_tx_ffe.post2 = tx_ffe->post2;

    for (i = 0x0; i < channel_cnt; i++) {
        set_tx_ffe.channel_id = start_channel;
        ret = retimer_set_tx_ffe(obj_handle, &set_tx_ffe, data_info, BUFFER_MAX_LEN);
        if (ret != RET_OK) {
            ret_value = RET_ERR;
            debug_log(DLOG_ERROR, "Retimer %s set channel %d txffe failed, ret(%d)", dfl_get_object_name(obj_handle),
                start_channel, ret);
            continue;
        }
        start_channel++;
    }

    return ret_value;
}


LOCAL void retimer_txffe_match_handle(OBJ_HANDLE obj_handle, CDR_TX_FFE *tx_ffe, guint8 start_channel,
    guint8 channel_cnt)
{
    gint32 ret = retimer_txffe_check(obj_handle, tx_ffe, start_channel, channel_cnt);
    
    if (ret == RET_OK) {
        return;
    }

    ret = retimer_txffe_set(obj_handle, tx_ffe, start_channel, channel_cnt);
    if (ret != RET_OK) {
        return;
    }

    debug_log(DLOG_ERROR, "Retimer %s set channel %d txffe succeed.", dfl_get_object_name(obj_handle), start_channel);

    return;
}


LOCAL gint32 retimer_get_para_by_optical(OBJ_HANDLE obj_handle, guint8 *speed, CDR_TX_FFE *ptx_ffe,
    OPT_CDR_ADAPT *opt_cdr_adapt, guint8 len)
{
    guint8 i;
    guint8 present = 0xff;
    OBJ_HANDLE optical_handle;
    gchar identify[0x4] = {0x0};
    gchar conntype[0x4] = {0x0};
    guint16 tans_length = 0xff;

    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_RETIMER_REFOPTICAL, &optical_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Retimer %s get reference optical obj failed, ret(%d).", dfl_get_object_name(obj_handle),
            ret);
        return ret;
    }

    ret = dal_get_property_value_byte(optical_handle, PROPERTY_OPT_MDL_PRESENT, &present);
    if ((ret != RET_OK) || (present != 0x1)) {
        return RET_ERR;
    }

    ret = dal_get_property_value_string(optical_handle, PROPERTY_OPT_MDL_INDENTIFIER, identify, sizeof(identify));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get optical %s property failed, ret(%d).", dfl_get_object_name(optical_handle), ret);
        return ret;
    }
    ret = dal_get_property_value_string(optical_handle, PROPERTY_OPT_MDL_CONNECTOR_TYPE, conntype, sizeof(conntype));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get optical %s property failed, ret(%d).", dfl_get_object_name(optical_handle), ret);
        return ret;
    }
    ret = dal_get_property_value_uint16(optical_handle, PROPERTY_OPT_MDL_TRANSFER_DISTANCE, &tans_length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get optical %s property failed,ret(%d).", dfl_get_object_name(optical_handle), ret);
        return ret;
    }

    for (i = 0; i < len; i++) {
        
        if ((strcmp(identify, opt_cdr_adapt[i].opt_idenntify) == 0) &&
            ((strcmp(conntype, "0c") == 0) || (tans_length == opt_cdr_adapt[i].opt_trans_length))) {
            *speed = opt_cdr_adapt[i].cdr_speed;
            *ptx_ffe = opt_cdr_adapt[i].tx_ffe;
            return RET_OK;
        }
    }

    
    *speed = opt_cdr_adapt[len - 1].cdr_speed;
    *ptx_ffe = opt_cdr_adapt[len - 1].tx_ffe;

    debug_log(DLOG_INFO, "Optical%s identify(%s) conntype(%s) tanslength(%d) match table failed, use default param.",
        dfl_get_object_name(optical_handle), identify, conntype, tans_length);

    return RET_OK;
}


LOCAL void retimer_serdes_para_monitor(OBJ_HANDLE obj_handle)
{
    guint8 channel_cnt = 0x4; 
    guint8 opt_speed = CHANNEL_SPEED_BUTT;
    CDR_TX_FFE cdr_tx_ffe_optical = { 0 };
    CDR_TX_FFE cdr_tx_ffe_backboard = { 0, -6, 57, 0, 0 }; 

    
    gint32 ret = retimer_get_para_by_optical(obj_handle, &opt_speed, &cdr_tx_ffe_optical, g_gOptCdrAdapt,
        ARRAY_SIZE(g_gOptCdrAdapt));
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s connect optical not ready", dfl_get_object_name(obj_handle));
        return;
    }
    
    retimer_speed_match_handle(obj_handle, opt_speed);
    
    retimer_txffe_match_handle(obj_handle, &cdr_tx_ffe_backboard, 0x4, channel_cnt); 
    
    retimer_txffe_match_handle(obj_handle, &cdr_tx_ffe_optical, 0x0, channel_cnt); 

    return;
}


LOCAL void retimer_communication_eventmonitor(OBJ_HANDLE obj_handle, guint8 state)
{
    guint8 retimer_id;
    static guint8 temp_arr[RETIMER_DEVICE_NUM] = {0}; 

    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get retimer id failed, object = %s.", dfl_get_object_name(obj_handle));
        return;
    }
    if (retimer_id >= RETIMER_DEVICE_NUM) {
        debug_log(DLOG_ERROR, "Get wrong retimer id, object = %s, retimerId = %d.", dfl_get_object_name(obj_handle),
            retimer_id);
        return;
    }

    if (state != 0) {
        temp_arr[retimer_id] += 1; 
    } else {
        temp_arr[retimer_id] = 0;
    }

    if (temp_arr[retimer_id] > 5) { 
        temp_arr[retimer_id] = 5;   
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_COMMUNICATION_STATE, 1, DF_AUTO); 
    } else {
        (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_COMMUNICATION_STATE, 0, DF_AUTO);
    }

    return;
}


LOCAL gint32 retimer_config_monitor(OBJ_HANDLE obj_handle)
{
    gint32 ret = retimer_get_heartbeat(obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer %s heartbeat check not ok,start init", dfl_get_object_name(obj_handle));
        retimer_property_init(obj_handle);
        retimer_communication_eventmonitor(obj_handle, 1); 

        ret = retimer_cdr_init(obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Retimer %s init failed, ret(%d)", dfl_get_object_name(obj_handle), ret);
            return ret;
        }
        debug_log(DLOG_ERROR, "Retimer %s init succeed", dfl_get_object_name(obj_handle));
    }

    
    (void)dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_UPG_STATE, RETIMER_5902_UPG_VALID, DF_NONE);
    retimer_communication_eventmonitor(obj_handle, 0);

    return ret;
}


LOCAL void retimer_get_smm_temperature_by_handle(OBJ_HANDLE obj_handle)
{
    gint16 temp_data = 0x4000; 

    gint32 ret = retimer_get_temperature(obj_handle, &temp_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get temperature failed, object = %s.", dfl_get_object_name(obj_handle));
    }

    ret = dal_set_property_value_uint16(obj_handle, PROPERTY_RETIMER_TEMPERATURE, temp_data, DF_NONE);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s set temperature failed. ret = %d.", dfl_get_object_name(obj_handle), ret);
    }
    return;
}


LOCAL void get_smm_retimer_logs_foreach(OBJ_HANDLE obj_handle)
{
    static guint8 get_diag_flag[RETIMER_DEVICE_NUM] = {0};
    guint8 retimer_id = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
    if (retimer_id >= RETIMER_DEVICE_NUM) {
        debug_log(DLOG_DEBUG, "Retimer id is too large. id = %d", retimer_id);
        return;
    }
    get_diag_flag[retimer_id] += 1;
    if (get_diag_flag[retimer_id] < 120) { 
        return;
    }
    get_diag_flag[retimer_id] = 0;

    gchar file_path[RETIMER_LOG_PATH_MAX_LEN] = {0};
    if (sprintf_s(file_path, sizeof(file_path), "%s%s", RETIMER_LOG_PATH_BASE, dfl_get_object_name(obj_handle)) <= 0) {
        return;
    }
    
    if (access(file_path, F_OK) != 0) {
        __retimer_get_logs_initial(obj_handle, NULL);
        debug_log(DLOG_ERROR, "%s: The BMC is upgraded to a version that supports log collection for the first time.",
            __FUNCTION__);
    }

    check_get_smm_retimer_logs(obj_handle);
}


LOCAL gint32 retimer_power_state_handle(OBJ_HANDLE obj_handle)
{
    guint8 power_state = 0xff;
    static guint8 power_flag = 0; 

    
    gint32 ret = retimer_get_powerstate(&power_state);
    if (ret != RET_OK) {
        return ret;
    }

    if (power_state != PAYLOAD_POWER_STATE_ON) {
        (void)dal_set_property_value_string(obj_handle, PROPERTY_RETIMER_SOFT_VER, "\0", DF_NONE);
        retimer_property_init(obj_handle); 
        power_flag = 0;
        return RET_ERR;
    }
    power_flag++;
    if (power_flag > 2) { 
        power_flag = 2;
    }

    
    if (power_flag == 1) {
        debug_log(DLOG_ERROR, "The power-on is complete and the initialization starts.");
        __retimer_get_logs_initial(obj_handle, NULL);
    }

    return RET_OK;
}


LOCAL gint32 retimer_upgrade_state_handle(GSList *obj_list)
{
    gint32 ret;
    guint8 is_upgrade;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, &is_upgrade);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] dal_get_property_value_byte failed. ret = %d.", dfl_get_object_name(obj_handle),
                ret);
            return RET_ERR;
        }

        if (is_upgrade == RETIMER_UPGRADING) {
            debug_log(DLOG_DEBUG, "[%s] is_upgrade %d.", dfl_get_object_name(obj_handle), is_upgrade);
            return RET_ERR;
        }
    }
    return RET_OK;
}


LOCAL gint32 retimer_change_list_order(GSList **obj_list_addr)
{
    GSList *tmp_obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 i;
    guint8 load_sequence = 0;
    guint8 len;

    gint32 ret = dfl_get_object_list(CLASS_RETIMER_NAME, &tmp_obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "dfl_get_object_list failed(ret: %d)", ret);
        return ret;
    }

    len = g_slist_length(tmp_obj_list);
    for (i = 0x0; i < len; i++) {
        for (obj_node = tmp_obj_list; obj_node; obj_node = obj_node->next) {
            
            obj_handle = (OBJ_HANDLE)obj_node->data;
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_LOAD_SEQUENCE, &load_sequence);

            
            if (load_sequence == i) {
                *obj_list_addr = g_slist_append(*obj_list_addr, (gpointer)obj_handle);
                break;
            }
        }
    }

    g_slist_free(tmp_obj_list);
    tmp_obj_list = NULL;

    if (*obj_list_addr == NULL) {
        debug_log(DLOG_ERROR, "After change list order, list is null.");
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL void retimer_health_eventmonitor(OBJ_HANDLE obj_handle)
{
    guint8 retimer_id = 0;
    static guint32 error_num[RETIMER_DEVICE_NUM] = {0}; 
    static time_t start_tick[RETIMER_DEVICE_NUM] = {0};
    time_t cur_tick[RETIMER_DEVICE_NUM] = {0};
    static gint8 log_flag = 0; 

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &retimer_id);
    if (retimer_id >= RETIMER_DEVICE_NUM) {
        debug_log(DLOG_DEBUG, "Retimer id is too large. id = %d", retimer_id);
        return;
    }

    gint32 ret = retimer_get_and_clear_chip_health(obj_handle); 
    if (ret != RET_OK) {
        if (error_num[retimer_id] == 0) { 
            start_tick[retimer_id] = dal_tick_get_seconds();
            debug_log(DLOG_ERROR, "%s firstly occur health error, start_tick = %ld.", dfl_get_object_name(obj_handle),
                start_tick[retimer_id]);
        }
        error_num[retimer_id]++;
    }

    if (error_num[retimer_id] >= 0x2) { 
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_CHIP_STATE, 0x1, DF_SAVE_TEMPORARY); 
        if (log_flag == 0x0) {
            log_flag = 0x1;
            debug_log(DLOG_ERROR, "%s secondly occur health error, ret = %d.", dfl_get_object_name(obj_handle), ret);
        }
    }

    cur_tick[retimer_id] = dal_tick_get_seconds(); 
    if ((cur_tick[retimer_id] - start_tick[retimer_id]) >= (48 * 3600)) { 
        debug_log(DLOG_ERROR, "Get %s health state, error_num = %d.", dfl_get_object_name(obj_handle),
            error_num[retimer_id]);
        error_num[retimer_id] = 0;
        start_tick[retimer_id] = dal_tick_get_seconds(); 
        log_flag = 0;
    }

    return;
}


LOCAL void retimer_get_dieid(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gchar dieid[MAX_DIEID_LEN + 1] = {0};

    (void)dal_get_property_value_string(obj_handle, PROPERTY_RETIMER_DIEID, dieid, sizeof(dieid));
    if (strlen(dieid) != 0) {
        debug_log(DLOG_DEBUG, "chip %s dieid ok: %s", dfl_get_object_name(obj_handle), dieid);
        return;
    }

    ret = dft_5902_set_dieid_info_to_property(obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] dft_5902_set_dieid_info_to_property failed, ret: %d",
            dfl_get_object_name(obj_handle), ret);
    }
}


LOCAL void retimer_scan_task(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;

    (void)prctl(PR_SET_NAME, (gulong) "Retimer5902");

    gint32 ret = retimer_change_list_order(&obj_list);
    if (ret != RET_OK) {
        return;
    }

    
    (void)dfl_foreach_object(CLASS_RETIMER_NAME, create_retimer_bind_data, NULL, NULL);

    while (1) {
        vos_task_delay(5000); 

        if (read_retimer_routine_open_status() != RETIMER_OPEN_ROUTINE) { 
            write_retimer_routine_open_status(RETIMER_ALREADY_CLOSE_ROUTINE);
            continue;
        }

        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            obj_handle = (OBJ_HANDLE)obj_node->data;
            if (retimer_power_state_handle(obj_handle) != RET_OK) {
                
                continue;
            }

            if (retimer_upgrade_state_handle(obj_list) != RET_OK) {
                
                break;
            }

            if (retimer_config_monitor(obj_handle) != RET_OK) {
                
                continue;
            }

            
            retimer_serdes_para_monitor(obj_handle);

            
            retimer_adapt_monitor(obj_handle);

            
            ret = retimer_refresh_version(obj_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "[%s] retimer_refresh_version failed.", dfl_get_object_name(obj_handle));
            }

            
            retimer_get_smm_temperature_by_handle(obj_handle);

            
            retimer_health_eventmonitor(obj_handle);

            
            retimer_get_dieid(obj_handle);

            
            get_smm_retimer_logs_foreach(obj_handle);
        }
    }
}


LOCAL gint32 retimer_heartbeat_check(OBJ_HANDLE obj_handle)
{
    gint32 ret = retimer_get_heartbeat(obj_handle);
    guint8 slot;
    if (ret != RET_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_ID, &slot);
        retimer_err_log_limit(slot, ret, "Retimer %s heartbeat check failed, ret(%d).", dfl_get_object_name(obj_handle),
            ret);
        retimer_property_init(obj_handle);
        retimer_communication_eventmonitor(obj_handle, 1); 
        return ret;
    }
    retimer_communication_eventmonitor(obj_handle, 0);
    return ret;
}


LOCAL void retimer_ds280_config_by_lane(OBJ_HANDLE obj_handle, guint8 speed, CDR_TX_FFE *ptx_optical,
    CDR_TX_FFE *ptx_backboard)
{
    gint32 ret;
    guint8 i;
    CDR_DS280_CFG cdr_cfg;

    for (i = 0; i < RETIMER_LANE_NUM; i++) {
        cdr_cfg.cdr_chan = i;
        cdr_cfg.lane_speed = speed;

        if (i < 4) { 
            cdr_cfg.ffe_main = ptx_optical->main0;
            cdr_cfg.ffe_post1 = ptx_optical->post1;
            cdr_cfg.ffe_pre1 = ptx_optical->pre1;
        } else {
            cdr_cfg.ffe_main = ptx_backboard->main0;
            cdr_cfg.ffe_post1 = ptx_backboard->post1;
            cdr_cfg.ffe_pre1 = ptx_backboard->pre1;
        }

        ret = retimer_ds280_config(obj_handle, &cdr_cfg);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s config failed, ret(%d).", dfl_get_object_name(obj_handle), ret);
            return;
        }
    }

    debug_log(DLOG_DEBUG, "%s config succeed.", dfl_get_object_name(obj_handle));
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_CHANNEL_SPEED, speed, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Retimer %s update channel speed property failed,ret(%d).",
            dfl_get_object_name(obj_handle), ret);
    }
}


LOCAL void retimer_ds280_para_monitor(OBJ_HANDLE obj_handle)
{
    guint8 opt_speed = CHANNEL_SPEED_BUTT;
    guint8 cur_speed = CHANNEL_SPEED_BUTT;
    CDR_TX_FFE cdr_tx_ffe_optical = { 0 };
    CDR_TX_FFE cdr_tx_ffe_backboard = { 0, -4, 22, 0, 0 }; 
    guint8 link_status = LINK_DOWN;

    
    gint32 ret = retimer_get_para_by_optical(obj_handle, &opt_speed, &cdr_tx_ffe_optical, g_OptCdrDs280,
        ARRAY_SIZE(g_OptCdrDs280));
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s connect optical not ready", dfl_get_object_name(obj_handle));
        return;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_RETIMER_CHANNEL_SPEED, &cur_speed);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get current speed failed,ret(%d).", dfl_get_object_name(obj_handle), ret);
        return;
    }

    
    
    (void)dal_get_extern_value_byte(obj_handle, PROPERTY_RETIMER_LINK_STATUS, &link_status, DF_AUTO);
    ret = retimer_txffe_check(obj_handle, &cdr_tx_ffe_optical, 0x0, 0x4);    
    ret += retimer_txffe_check(obj_handle, &cdr_tx_ffe_backboard, 0x4, 0x4); 
    if ((ret == RET_OK) && (cur_speed == opt_speed) && (link_status == LINK_UP)) {
        return;
    }

    debug_log(DLOG_DEBUG,
        "DS280[%s] need to config. ret = %d, cur_speed = 0x%x, opt_speed = 0x%x, link_status = %d",
        dfl_get_object_name(obj_handle), ret, cur_speed, opt_speed, link_status);

    
    retimer_ds280_config_by_lane(obj_handle, opt_speed, &cdr_tx_ffe_optical, &cdr_tx_ffe_backboard);
}


LOCAL void retimer_scan_task_ds280(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle;

    (void)prctl(PR_SET_NAME, (gulong) "Retimer DS280");

    gint32 ret = retimer_change_list_order(&obj_list);
    if (ret != RET_OK) {
        return;
    }

    while (1) {
        vos_task_delay(5000); 

        if (read_retimer_routine_open_status() != RETIMER_OPEN_ROUTINE) { 
            write_retimer_routine_open_status(RETIMER_ALREADY_CLOSE_ROUTINE);
            continue;
        }

        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            obj_handle = (OBJ_HANDLE)obj_node->data;
            if (retimer_power_state_handle(obj_handle) != RET_OK) {
                
                continue;
            }

            if (retimer_heartbeat_check(obj_handle) != RET_OK) {
                
                continue;
            }

            
            retimer_ds280_para_monitor(obj_handle);
        }
    }
}

 
LOCAL void get_imu_cdr_temp_request_msg(IPMI_REQ_MSG_HEAD_S *req_msg_head, guint8 *req_data, guint8 data_len)
{
    guint8 frame_ptr = 0;
    req_data[frame_ptr++] = LONGB0(MANUFACTURE_HUAWEI);
    req_data[frame_ptr++] = LONGB1(MANUFACTURE_HUAWEI);
    req_data[frame_ptr++] = LONGB2(MANUFACTURE_HUAWEI); 
    req_data[frame_ptr++] = 0x21;                       
    req_data[frame_ptr++] = 0x00;                       
    req_msg_head->target_type = IPMI_ME;
    req_msg_head->target_instance = 0;
    req_msg_head->netfn = NETFN_OEM_REQ;
    req_msg_head->lun = 0;
    req_msg_head->cmd = 0x98;
    req_msg_head->src_len = frame_ptr;
    return;
}

#define FRAME_HEADER_SIZE 10
#define CDR_TEMP_DATA_LEN 16
#define REQ_DATA_LEN 5
LOCAL void get_cdr_temp_from_imu(OBJ_HANDLE cdr_handle)
{
    guint8 req_data[REQ_DATA_LEN] = {0};
    gpointer resp_ipmi = NULL;
    IPMI_REQ_MSG_HEAD_S req_msg_head;
    const guint8 *resp_data_buf = NULL;
    guint16 cur_cdr_temp = SENSOR_NA_READING;
    guint8 retimer_id = 0;
    guint8 resp_data_len;

    gint32 result = dal_get_property_value_byte(cdr_handle, PROPERTY_RETIMER_ID, &retimer_id);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get obj_handle=%s RetimerId fail! result(%d).", dfl_get_object_name(cdr_handle), result);
        return;
    }
    debug_log(DLOG_DEBUG, "%s: obj=%s get_cdr_temp_from_imu", __FUNCTION__, dfl_get_object_name(cdr_handle));
    get_imu_cdr_temp_request_msg(&req_msg_head, req_data, REQ_DATA_LEN);
    result = ipmi_send_request(&req_msg_head, req_data, RESQ_IPMI_SIZE, &resp_ipmi, TRUE);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "Get OS time send ipmi fail, result(%d)", result);
        return;
    }

    do {
        resp_data_buf = get_ipmi_src_data((gconstpointer)resp_ipmi);
        if (resp_data_buf == NULL || resp_data_buf[0] != COMP_CODE_SUCCESS) {
            debug_log(DLOG_DEBUG, "%s: Resp data is not valid.", __FUNCTION__);
            break;
        }
        resp_data_len = get_ipmi_src_data_len((gconstpointer)resp_ipmi);
        if (resp_data_len <= (FRAME_HEADER_SIZE + retimer_id * CDR_TEMP_DATA_LEN)) {
            break;
        }
        
        cur_cdr_temp = resp_data_buf[FRAME_HEADER_SIZE + retimer_id * CDR_TEMP_DATA_LEN];
        if (cur_cdr_temp == 0x7e) {
            cur_cdr_temp = SENSOR_NA_READING;
            break;
        }
        
        if (cur_cdr_temp == 0x7f) {
            cur_cdr_temp = SENSOR_INVALID_READING;
            break;
        }
        cur_cdr_temp = resp_data_buf[FRAME_HEADER_SIZE + retimer_id * CDR_TEMP_DATA_LEN - 1];
    } while (0);

    result = dal_set_property_value_uint16(cdr_handle, PROPERTY_RETIMER_TEMPERATURE, cur_cdr_temp, DF_NONE);
    if (result != DFL_OK) {
        debug_log(DLOG_DEBUG, "set obj_handle=%s temp fail! result = %d.", dfl_get_object_name(cdr_handle), result);
    }
    g_free(resp_ipmi);
    return;
}


LOCAL void set_dmp_req(DMP_REQ *dmp_req, guint8 op_cmd, guint8 op_fun, guint32 data_length, guint32 offset)
{
    dmp_req->hw_head[DMP_REQ_HEAD_INDEX0] = 0xdb;
    dmp_req->hw_head[DMP_REQ_HEAD_INDEX1] = 0x07;
    dmp_req->hw_head[DMP_REQ_HEAD_INDEX2] = 0x00;

    dmp_req->cmd = GET_ARM_CDR_CMD;
    dmp_req->op_cmd = op_cmd;
    dmp_req->op_fun = op_fun;
    dmp_req->lun = DMP_LAST_REQ;
    dmp_req->data_length = data_length; // 响应帧的数据段长度
    dmp_req->offset = offset;
}


LOCAL void set_req_msg_header(IPMI_REQ_MSG_HEAD_S *req_msg_header, guint8 ipmb_id, guint8 src_len)
{
    req_msg_header->target_type = IPMI_ME;     // 后续ME可能改为IMU
    req_msg_header->target_instance = ipmb_id; // IPMB通道号
    req_msg_header->netfn = NETFN_OEM_REQ;     // 功能号
    req_msg_header->cmd = DMP_CMD_NPU;
    req_msg_header->src_len = src_len;
}


LOCAL gint32 get_cdr_fw_version_temprature_send_ipmi(guint8 ipmb_id, gpointer *resp_ipmi)
{
    gint32 result;
    IPMI_REQ_MSG_HEAD_S req_msg_header = {};
    DMP_REQ dmp_req = { 0 };
    set_dmp_req(&dmp_req, DMP_GET_CDR_FW_VERSION_TEMPRATURE, DMP_OP_CODE, CDR_CMD_LEN, 0);
    set_req_msg_header(&req_msg_header, ipmb_id, sizeof(DMP_REQ));

    result = ipmi_send_request(&req_msg_header, &dmp_req.hw_head[0], RESQ_IPMI_SIZE, resp_ipmi, TRUE);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "%s:ipmi_send_request fail !", __FUNCTION__);
        return result;
    }

    return RET_OK;
}

LOCAL gint32 get_cdr_fw_version_temprature(OBJ_HANDLE cdr_handle, guint8 ipmb_id)
{
    const guint8 *resp_data_buf = NULL;
    gpointer resp_ipmi = NULL;
    gint16 temperature;
    gchar fw_version[FW_VER_MAX_LEN] = {0};
    guint32 str_len = 0;

    gint32 result = get_cdr_fw_version_temprature_send_ipmi(ipmb_id, &resp_ipmi);
    if (result != RET_OK) {
        return result;
    }

    resp_data_buf = get_ipmi_src_data((gconstpointer)resp_ipmi);
    if (resp_data_buf == NULL) {
        g_free(resp_ipmi);
        resp_ipmi = NULL;
        debug_log(DLOG_DEBUG, "%s:get_ipmi_src_data fail !", __FUNCTION__);
        return RET_ERR;
    }

    guint8 data_len = get_ipmi_src_data_len(resp_ipmi);
    if (data_len < RESPONSE_DATA_INDEX0 + 7) {
        g_free(resp_ipmi);
        resp_ipmi = NULL;
        debug_log(DLOG_DEBUG, "%s:ipmi data len overflow !", __FUNCTION__);
        return RET_ERR;
    }
    if ((resp_data_buf[0] == COMP_CODE_SUCCESS)) {
        debug_log(DLOG_DEBUG, "%s:GET fw SUCCESS !", __FUNCTION__);
        temperature = resp_data_buf[RESPONSE_DATA_INDEX0 + 2];
        temperature += resp_data_buf[RESPONSE_DATA_INDEX0 + 3] << HIGH_8BIT_OFFSET;
        for (guint8 i = RESPONSE_DATA_INDEX0 + 4; i <= RESPONSE_DATA_INDEX0 + 7; i++) {
            str_len += snprintf_truncated_s(fw_version + str_len, sizeof(fw_version) - str_len, "%u", resp_data_buf[i]);
            if (i != RESPONSE_DATA_INDEX0 + 7 && str_len < FW_VER_MAX_LEN) {
                fw_version[str_len++] = '.';
            }
        }
    } else {
        debug_log(DLOG_DEBUG, "%s:GET fw N/A FAIL !", __FUNCTION__);
        temperature = 0;
        strcpy_s(fw_version, sizeof(fw_version), "N/A");
    }

    result = dal_set_property_value_string(cdr_handle, PROPERTY_RETIMER_SOFT_VER, fw_version, DF_SAVE_TEMPORARY);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set fw_version fail ret = %d !", __FUNCTION__, result);
    }

    result = dal_set_property_value_uint16(cdr_handle, PROPERTY_RETIMER_TEMPERATURE, temperature, DF_NONE);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "%s:set temprature fail ret = %d !", __FUNCTION__, result);
    }

    g_free(resp_ipmi);
    return RET_OK;
}

LOCAL void get_cdr_temp_or_ver_from_imu(OBJ_HANDLE cdr_handle, guint8 ipmb_id)
{
    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        get_cdr_temp_from_imu(cdr_handle);
    } else {
        (void)get_cdr_fw_version_temprature(cdr_handle, ipmb_id);
    }
}


LOCAL void update_cdr_i2c_by_handle(OBJ_HANDLE cdr_handle)
{
    guint8 cdr_imu_num = 0;
    gint32 info_ret;

    (void)dal_get_property_value_byte(cdr_handle, PROPERTY_RETIMER_IPMB_INDEX, &cdr_imu_num);
    if (cdr_imu_num == CDR_IPMB_INDEX_INVALID) {
        return;
    }
    info_ret = dal_set_property_value_byte(cdr_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_BUSY, DF_AUTO);
    debug_log(DLOG_DEBUG, "detect retimer(%s) status change to busy, recsan by i2c, ret=%d.",
        dfl_get_object_name(cdr_handle), info_ret);
    vos_task_delay(NOTIFY_DELAY_TIME); 
    info_ret = retimer_get_info_by_handle(cdr_handle);
    info_ret += dal_set_property_value_byte(cdr_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_IDLE, DF_AUTO);
    debug_log(DLOG_DEBUG, "detect retimer(%s) status change to idle, recsan by os, ret=%d.",
        dfl_get_object_name(cdr_handle), info_ret);
    return;
}


LOCAL gint32 check_cdr_direct_by_handle(OBJ_HANDLE cdr_handle)
{
    gint32 ret;
    guint8 status;
    guint8 upg_status;
    guint8 cdr_imu_num = 0;
    gchar version[RSP_MAX_BUF_LEN] = {0};

    (void)dal_get_property_value_byte(cdr_handle, PROPERTY_RETIMER_IPMB_INDEX, &cdr_imu_num);
    if (cdr_imu_num == CDR_IPMB_INDEX_INVALID) {
        return FALSE;
    }
    ret = dal_get_property_value_string(cdr_handle, PROPERTY_RETIMER_SOFT_VER, version, sizeof(version));
    if (ret != RET_OK || strlen(version) == 0) {
        debug_log(DLOG_DEBUG, "%s soft verion is null, ret=%d", dfl_get_object_name(cdr_handle), ret);
        return TRUE;
    }
    ret = dal_get_property_value_byte(cdr_handle, PROPERTY_RETIMER_REQ_ACC, &status);
    ret += dal_get_property_value_byte(cdr_handle, PROPERTY_RETIMER_UPG_STATE, &upg_status);
    if ((ret != RET_OK || status != RETIMER_IDLE) && (upg_status == RETIMER_5902_UPG_IDLE)) {
        debug_log(DLOG_DEBUG, "%s:get %s channel notify status=%d upg_status=%d, ret=%d.", __FUNCTION__,
            dfl_get_object_name(cdr_handle), status, upg_status, ret);
        return TRUE;
    }
    return FALSE;
}


LOCAL void cdr_task_monitor_scan(void)
{
    (void)prctl(PR_SET_NAME, (unsigned long)"get_cdr_info_task");
    // 初始化前置状态为未就绪，跳转为准备好才进行扫描
    guint8 pre_ready_status[RETIMER_MAX_NUM];
    (void)memset_s(pre_ready_status, sizeof(pre_ready_status), RETIMER_NOT_READY, sizeof(pre_ready_status));

    while (1) {
        vos_task_delay(RETIMER_DELAY);
        GSList *retimer_list = NULL;
        (void)dfl_get_object_list(CLASS_RETIMER_NAME, &retimer_list);
        for (GSList *retimer_node = retimer_list; retimer_node; retimer_node = retimer_node->next) {
            OBJ_HANDLE retimer_obj = (OBJ_HANDLE)retimer_node->data;
            guint8 cur_id = RETIMER_MAX_NUM;

            
            (void)dal_get_property_value_byte(retimer_obj, PROPERTY_RETIMER_ID, &cur_id);
            if (cur_id >= RETIMER_MAX_NUM) {
                debug_log(DLOG_ERROR, "cur retimer id(%d), is out of max num", cur_id);
                continue;
            }
            guint8 cdr_imu_num = 0;
            
            (void)dal_get_property_value_byte(retimer_obj, PROPERTY_RETIMER_IPMB_INDEX, &cdr_imu_num);
            if (cdr_imu_num == CDR_IPMB_INDEX_INVALID) {
                continue;
            }
            
            
            get_cdr_temp_or_ver_from_imu(retimer_obj, cdr_imu_num);

            
            guint8 cur_status = get_retimer_status_ready(retimer_obj);
            if (pre_ready_status[cur_id] == RETIMER_NOT_READY && cur_status == RETIMER_HAS_READY) {
                update_cdr_i2c_by_handle(retimer_obj);
            }
            pre_ready_status[cur_id] = cur_status;

            
            if (check_cdr_direct_by_handle(retimer_obj) == TRUE) {
                update_cdr_i2c_by_handle(retimer_obj);
            }
        }
        g_slist_free(retimer_list);
    }
}


LOCAL gboolean event_is_caused_by_sys_reset(void)
{
    const guint16 CHECK_CYCLE = 1000;
    const guint8 CHECK_TIMES = 5;
    guint32 i;

    for (i = 0; i < CHECK_TIMES; i++) {
        if (g_retimer_log_sys_reset_flg == 1) {
            return TRUE;
        }
        vos_task_delay(CHECK_CYCLE);
    }

    return FALSE;
}

LOCAL guint8 check_pcie_link_down(OBJ_HANDLE retimer, guint8 port_id, RetimerPCIeStatus *retimer_pcie_status,
    RetimerPCIeStatus pre_retimer_pcie_status)
{
    gint32 ret;
    guint32 cur_time = (guint32)vos_get_cur_time_stamp();
    guint8 port_status = RETIMER_PORT_OK;

    // up -> down
    if (pre_retimer_pcie_status.link_status[0][port_id].link_status == RETIMER_PCIE_LINK_UP &&
        retimer_pcie_status->link_status[0][port_id].link_status == RETIMER_PCIE_LINK_DOWN) {
        if (cur_time - pre_retimer_pcie_status.link_status_tick[0][port_id].link_status_tick >
            RETIMER_LOG_WRITE_INTERVAL_MAX) {
            if (event_is_caused_by_sys_reset()) {
                retimer_pcie_status->link_status[0][port_id].link_status =
                    pre_retimer_pcie_status.link_status[0][port_id].link_status;
                return port_status;
            }
            retimer_pcie_status->link_status_tick[0][port_id].link_status_tick = cur_time;

            ret = dal_set_property_value_uint32(retimer, PROPERTY_RETIMER_ERR1_TIME, cur_time, DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: %u", __FUNCTION__, cur_time);
            }
            port_status = RETIMER_PORT_ERR;
            debug_log(DLOG_ERROR, "%s: %s port(%u) PCIe link status changed from up to down, need to store log",
                __FUNCTION__, dfl_get_object_name(retimer), port_id);
        }
    }

    debug_log(DLOG_DEBUG, "%s: %u %u", __FUNCTION__, cur_time,
        pre_retimer_pcie_status.link_status_tick[0][port_id].link_status_tick);
    return port_status;
}

LOCAL guint8 check_pcie_link_speed(OBJ_HANDLE retimer, guint8 port_id, RetimerPCIeStatus *retimer_pcie_status,
    RetimerPCIeStatus pre_retimer_pcie_status)
{
    gint32 ret;
    guint32 cur_time = (guint32)vos_get_cur_time_stamp();
    guint8 port_status = RETIMER_PORT_OK;

    // 降速
    if (pre_retimer_pcie_status.link_status[0][port_id].link_speed >
        retimer_pcie_status->link_status[0][port_id].link_speed) {
        if (cur_time - pre_retimer_pcie_status.link_status_tick[0][port_id].link_speed_tick >
            RETIMER_LOG_WRITE_INTERVAL_MAX) {
            if (event_is_caused_by_sys_reset()) {
                retimer_pcie_status->link_status[0][port_id].link_speed =
                    pre_retimer_pcie_status.link_status[0][port_id].link_speed;
                return port_status;
            }
            retimer_pcie_status->link_status_tick[0][port_id].link_speed_tick = cur_time;

            ret = dal_set_property_value_uint32(retimer, PROPERTY_RETIMER_ERR2_TIME, cur_time, DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: %u", __FUNCTION__, cur_time);
            }
            port_status = RETIMER_PORT_ERR;
            debug_log(DLOG_ERROR, "%s: %s port(%u) PCIe link speed changed from %u to %u, need to store log",
                __FUNCTION__, dfl_get_object_name(retimer), port_id,
                pre_retimer_pcie_status.link_status[0][port_id].link_speed,
                retimer_pcie_status->link_status[0][port_id].link_speed);
        }
    }

    debug_log(DLOG_DEBUG, "%s: %u %u", __FUNCTION__, cur_time,
        pre_retimer_pcie_status.link_status_tick[0][port_id].link_speed_tick);
    return port_status;
}

LOCAL guint8 check_pcie_link_width(OBJ_HANDLE retimer, guint8 port_id, RetimerPCIeStatus *retimer_pcie_status,
    RetimerPCIeStatus pre_retimer_pcie_status)
{
    gint32 ret;
    guint32 cur_time = (guint32)vos_get_cur_time_stamp();
    guint8 port_status = RETIMER_PORT_OK;

    // 带宽变窄
    if (pre_retimer_pcie_status.link_status[0][port_id].link_width >
        retimer_pcie_status->link_status[0][port_id].link_width) {
        if (cur_time - pre_retimer_pcie_status.link_status_tick[0][port_id].link_width_tick >
            RETIMER_LOG_WRITE_INTERVAL_MAX) {
            if (event_is_caused_by_sys_reset()) {
                retimer_pcie_status->link_status[0][port_id].link_width =
                    pre_retimer_pcie_status.link_status[0][port_id].link_width;
                return port_status;
            }
            retimer_pcie_status->link_status_tick[0][port_id].link_width_tick = cur_time;

            ret = dal_set_property_value_uint32(retimer, PROPERTY_RETIMER_ERR3_TIME, cur_time, DF_SAVE_TEMPORARY);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: %u", __FUNCTION__, cur_time);
            }
            port_status = RETIMER_PORT_ERR;
            debug_log(DLOG_ERROR, "%s: %s port(%u) PCIe link width changed from %u to %u, need to store log",
                __FUNCTION__, dfl_get_object_name(retimer), port_id,
                pre_retimer_pcie_status.link_status[0][port_id].link_width,
                retimer_pcie_status->link_status[0][port_id].link_width);
        }
    }

    debug_log(DLOG_DEBUG, "%s: %u %u", __FUNCTION__, cur_time,
        pre_retimer_pcie_status.link_status_tick[0][port_id].link_width_tick);
    return port_status;
}


LOCAL void check_pcie_link_status(OBJ_HANDLE retimer, guint8 *port_status, guint8 port_num,
    RetimerPCIeStatus *retimer_pcie_status)
{
    gint32 ret;
    guint8 core_id = 0; // core_id固定0
    guint8 port_id;
    RetimerPCIeStatus pre_retimer_pcie_status;

    pre_retimer_pcie_status = *retimer_pcie_status;

    for (port_id = 0; (port_id < port_num) && (g_retimer_log_sys_reset_flg != 1); port_id++) {
        ret = retimer_get_pcie_status(retimer, core_id, port_id, &(retimer_pcie_status->link_status[core_id][port_id]));
        if (ret != RET_OK) {
            continue;
        }

        if (retimer_pcie_status->initial_flg == 0) {
            retimer_pcie_status->initial_flg = 1;
            continue; // 成功读取到初值
        }

        // 未读取到初始值
        if (retimer_pcie_status->initial_flg == 0) {
            continue; // 成功读取到初值
        }

        if (retimer_pcie_status->initial_flg == 1) {
            if (!(check_pcie_link_down(retimer, port_id, retimer_pcie_status, pre_retimer_pcie_status) == 0 &&
                check_pcie_link_speed(retimer, port_id, retimer_pcie_status, pre_retimer_pcie_status) == 0 &&
                check_pcie_link_width(retimer, port_id, retimer_pcie_status, pre_retimer_pcie_status) == 0)) {
                port_status[port_id] = RETIMER_PORT_ERR;
            }
        }
    }
}


LOCAL void retimer_initial_err_time(OBJ_HANDLE retimer)
{
    gint32 ret;

    ret = dal_set_property_value_uint32(retimer, PROPERTY_RETIMER_ERR1_TIME, 0, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: power on set time to 0", __FUNCTION__);
    }
    ret = dal_set_property_value_uint32(retimer, PROPERTY_RETIMER_ERR2_TIME, 0, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: power on set time to 0", __FUNCTION__);
    }
    ret = dal_set_property_value_uint32(retimer, PROPERTY_RETIMER_ERR3_TIME, 0, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: power on set time to 0", __FUNCTION__);
    }
}


LOCAL gint32 retimer_initial_rebuild_dir(gchar *file_path, gint32 buff_len)
{
    gint32 path_len;
    (void)vos_mkdir_recursive(file_path);

    
    (void)chown(file_path, ADMINISTRATOR_USER_UID, ADMINISTRATOR_GID);
    (void)chmod(file_path, S_IRWXU | S_IRGRP | S_IXGRP);

    path_len = strlen(file_path);
    // 建立目录
    if (path_len < 1)
        return RET_ERR;
    file_path[path_len] = '\0';
    (void)strcat_s(file_path, buff_len, RETIMER_LOG_PATH_RAM);
    (void)vos_mkdir_recursive(file_path);
    
    (void)chown(file_path, ADMINISTRATOR_USER_UID, ADMINISTRATOR_GID);
    (void)chmod(file_path, S_IRWXU | S_IRGRP | S_IXGRP);

    file_path[path_len] = '\0';
    (void)strcat_s(file_path, buff_len, RETIMER_LOG_PATH_PCIE);
    (void)vos_mkdir_recursive(file_path);

    
    (void)chown(file_path, ADMINISTRATOR_USER_UID, ADMINISTRATOR_GID);
    (void)chmod(file_path, S_IRWXU | S_IRGRP | S_IXGRP);

    file_path[path_len] = '\0';
    (void)strcat_s(file_path, buff_len, RETIMER_LOG_PATH_SERDES);
    (void)vos_mkdir_recursive(file_path);

    
    (void)chown(file_path, ADMINISTRATOR_USER_UID, ADMINISTRATOR_GID);
    (void)chmod(file_path, S_IRWXU | S_IRGRP | S_IXGRP);

    debug_log(DLOG_ERROR, "%s: vos mkdir recursive sucessfully.", __FUNCTION__);
    return RET_OK;
}


LOCAL gint32 retimer_initial_poweron_para(OBJ_HANDLE retimer)
{
    gchar file_path[RETIMER_LOG_PATH_MAX_LEN] = {0};

    // /data/var/log/Retimer/Cdr5902H_Obj_1-10
    gint32 ret = strcat_s(file_path, RETIMER_LOG_PATH_MAX_LEN, RETIMER_LOG_PATH_BASE);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcat failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }
    // 目录不存在，建立目录。
    if (access(file_path, F_OK) != 0) {
        (void)vos_mkdir_recursive(file_path);
    }
    
    (void)chown(file_path, ADMINISTRATOR_USER_UID, ADMINISTRATOR_GID);
    (void)chmod(file_path, S_IRWXU | S_IRGRP | S_IXGRP);

    ret = strcat_s(file_path, RETIMER_LOG_PATH_MAX_LEN, dfl_get_object_name(retimer));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcat failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }
    // 文件夹存在，删除所有的日志，重新开始事件记录
    if (access(file_path, F_OK) == 0) {
        debug_log(DLOG_ERROR, "%s: clear file after power-on", __FUNCTION__);
        vos_rm_filepath(file_path);
    }

    ret = retimer_initial_rebuild_dir(file_path, sizeof(file_path));
    retimer_initial_err_time(retimer);

    return ret;
}


LOCAL gint32 __retimer_get_logs_initial(OBJ_HANDLE retimer, gpointer user_data)
{
    guint8 retimer_type = RETIMER_5902_TYPE_L;

    // 仅收集retimer，cdr由1620收集
    (void)dal_get_property_value_byte(retimer, PROPERTY_RETIMER_CHIP_TYPE, &retimer_type);
    if (retimer_type == RETIMER_5902_TYPE_L) {
        if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) { 
            return RET_OK;
        }
    }

    RetimerPCIeStatus *retimer_pcie_status = NULL;

    gint32 ret = dfl_get_binded_data(retimer, (gpointer *)&retimer_pcie_status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get bind data of %s failed ret = %d", __FUNCTION__, dfl_get_object_name(retimer),
            ret);
        return RET_OK;
    }

    // 写时间至retimer，此时间在后面日志里面会体现
    retimer_set_host_time(retimer);
    // os上电初始化
    ret = retimer_initial_poweron_para(retimer);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: mkdir and set time to %s failed.", __FUNCTION__, dfl_get_object_name(retimer));
        return RET_OK;
    }

    gint32 core_id;
    gint32 port_id;
    guint32 last_time = 0; // 上电错误日志记录时间清零

    for (core_id = 0; core_id < RETIMER_CORE_NUM; core_id++) {
        for (port_id = 0; port_id < RETIMER_CORE_NUM; port_id++) {
            retimer_pcie_status->link_status_tick[core_id][port_id].link_status_tick = last_time;
            retimer_pcie_status->link_status_tick[core_id][port_id].link_speed_tick = last_time;
            retimer_pcie_status->link_status_tick[core_id][port_id].link_width_tick = last_time;
            retimer_pcie_status->link_status[core_id][port_id].link_status = 0;
            retimer_pcie_status->link_status[core_id][port_id].link_speed = 0;
            retimer_pcie_status->link_status[core_id][port_id].link_width = 0;
        }
    }
    debug_log(DLOG_DEBUG, "%s: power on, set err log time to 1970-01-01", __FUNCTION__);

    return RET_OK;
}


LOCAL void retimer_request_pcie_log(OBJ_HANDLE retimer, guint8 port_id, guint16 opcode)
{
    SMBUS_REQ req = { 0 };
    gint32 ret;
    gint8 resp_buff[BUFFER_MAX_LEN] = {0};

    // b01
    req.lun = CDR5902_REQ_LUN;
    req.opcode = opcode;

    gint32 core_id;
    for (core_id = 0; core_id < RETIMER_CORE_NUM; core_id++) {
        req.data[0] = core_id;                      // core id
        req.data[1] = (port_id == 1) ? 4 : port_id; // 第二个port 编号为4

        gint32 k;
        for (k = 0; k < RETIMER_LANE_NUM; k++) {
            req.data[2] = k;
            // 不需要读取，打印到ram 统一读取
            ret = retimer_get_info(retimer, &req, 3, resp_buff, BUFFER_MAX_LEN); 
            debug_log(DLOG_ERROR, "%s: retimer request opcode(%u) finished, ret = %d.", __FUNCTION__, opcode, ret);
        }
    }
}


LOCAL gint32 retimer_get_pcie_log(OBJ_HANDLE retimer, guint8 *port_status, gint32 port_num,
    RetimerPCIeStatus *retimer_pcie_status)
{
    char filename[RETIMER_LOG_PATH_MAX_LEN] = {0};
    gint32 port_id;
    gint32 ret;

    for (port_id = 0; port_id < port_num; port_id++) {
        // port 正常
        if (port_status[port_id] == RETIMER_PORT_OK) {
            continue;
        }

        // B01
        retimer_request_pcie_log(retimer, port_id, RETIMER_CMD_GET_PCIE_STATUS);
        // B04
        retimer_request_pcie_log(retimer, port_id, RETIMER_CMD_GET_PCIE_LTSSM);
        // B06
        retimer_request_pcie_log(retimer, port_id, RETIMER_CMD_GET_PCIE_TRACE);
    }

    ret = retimer_get_log_name(retimer_pcie_status->pcie_log_name, RETIMER_LOG_NUM + 1, filename, sizeof(filename));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get log name , ret = %d.", __FUNCTION__, ret);
        return ret;
    }
    // 0x0c 开始生成文件
    return retimer_get_and_store_log(retimer, filename);
}

LOCAL gint32 retimer_get_serdes_log(OBJ_HANDLE retimer, RetimerPCIeStatus *retimer_pcie_status)
{
    char file_name[RETIMER_LOG_PATH_MAX_LEN] = {0};

    // 0xA0A
    gint32 ret = retimer_request_serdes_main_log(retimer);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: request serdes logs failed, ret = %d.", __FUNCTION__, ret);
        return ret;
    }
    debug_log(DLOG_ERROR, "%s: request serdes logs successed.", __FUNCTION__);

    ret = retimer_get_log_name(retimer_pcie_status->serdes_log_name, RETIMER_LOG_NUM + 1, file_name, sizeof(file_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get log name , ret = %d.", __FUNCTION__, ret);
        return ret;
    }

    // 0x0c 开始生成文件
    return retimer_get_and_store_log(retimer, file_name);
}

LOCAL void check_get_retimer_logs(OBJ_HANDLE retimer, guint8 dump_flag)
{
    guint8 port_status[RETIMER_PORT_NUM] = {RETIMER_PORT_OK};

    RetimerPCIeStatus *retimer_pcie_status = NULL;
    gint32 ret = dfl_get_binded_data(retimer, (gpointer *)&retimer_pcie_status);
    if (ret != DFL_OK || retimer_pcie_status == NULL) {
        debug_log(DLOG_ERROR, "%s: get bind data of %s failed, ret = %d.", __FUNCTION__, dfl_get_object_name(retimer),
            ret);
        return;
    }

    // step 1 状态巡检
    check_pcie_link_status(retimer, port_status, RETIMER_PORT_NUM, retimer_pcie_status);
    if (port_status[1] == RETIMER_PORT_OK && port_status[0] == RETIMER_PORT_OK && dump_flag != RETIMER_LOG_DUMP) {
        // 1 故障间隔太短，保护flash，不处理。
        // 2 未识别到故障
        return;
    }
    debug_log(DLOG_ERROR, "%s: retimer %s: port0(%d), port1(%d).", __FUNCTION__, dfl_get_object_name(retimer),
        port_status[0], port_status[1]);

    if (dump_flag == RETIMER_LOG_CYCLE) {
        debug_log(DLOG_ERROR, "%s: start to get ram log.", __FUNCTION__);
        // step 2.1：记录RAMLog
        ret = retimer_get_ram_log(retimer, retimer_pcie_status);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: failed to get ram log ret = %d.", __FUNCTION__, ret);
        }

        debug_log(DLOG_ERROR, "%s: start to get pcie log.", __FUNCTION__);
        // step 2.2: 记录PCIelog
        ret = retimer_get_pcie_log(retimer, port_status, RETIMER_PORT_NUM, retimer_pcie_status);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: failed to get pcie log ret = %d.", __FUNCTION__, ret);
        }
    } else { // RETIMER_LOG_DUMP
        guint32 last_err1_time = 0;
        guint32 last_err2_time = 0;
        guint32 last_err3_time = 0;

        (void)dal_get_property_value_uint32(retimer, PROPERTY_RETIMER_ERR1_TIME, &last_err1_time);
        (void)dal_get_property_value_uint32(retimer, PROPERTY_RETIMER_ERR2_TIME, &last_err2_time);
        (void)dal_get_property_value_uint32(retimer, PROPERTY_RETIMER_ERR3_TIME, &last_err3_time);
        if (last_err1_time != 0 || last_err2_time != 0 || last_err3_time != 0) { // 故障过
            debug_log(DLOG_ERROR, "%s: start to get serdes log.", __FUNCTION__);
            // step 2.3: 记录SerDeslog
            ret = retimer_get_serdes_log(retimer, retimer_pcie_status);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: failed to get serdes log ret = %d.", __FUNCTION__, ret);
            }
        }
    }
}


LOCAL gint32 get_retimer_logs_foreach(OBJ_HANDLE retimer, gpointer user_data)
{
    guint8 retimer_type = RETIMER_5902_TYPE_L;
    guint8 is_upgrade = RETIMER_5902_UPG_ON_GOING;
    guint8 is_busy = 1;

    guint8 dump_flag = *(guint8 *)user_data;

    // 仅收集retimer，cdr由1620收集
    (void)dal_get_property_value_byte(retimer, PROPERTY_RETIMER_CHIP_TYPE, &retimer_type);
    if (retimer_type == RETIMER_5902_TYPE_L) {
        return RET_OK;
    }

    if (!retimer_is_need_scan_task(retimer)) {
        debug_log_limit_when_fail(DLOG_LIMIT_60, TRUE, "%s status is not need scan.", dfl_get_object_name(retimer));
        return RET_OK;
    }

    // 如果总线被占用等到总线释放
    for (gint32 i = 0; i < RETRY_LOG_RETRY_TIMES; i++) {
        (void)dal_get_property_value_byte(retimer, PROPERTY_RETIMER_REQ_ACC, &is_busy);
        if (is_busy == RETIMER_BUSY) {
            // 升级中，升级需要长时间占用，不等待直接退出。
            (void)dal_get_property_value_byte(retimer, PROPERTY_RETIMER_UPG_STATE, &is_upgrade);
            if (is_upgrade == RETIMER_5902_UPG_ON_GOING) {
                debug_log(DLOG_DEBUG, "%s: %s wait until the upgrade is complete.", __FUNCTION__,
                    dfl_get_object_name(retimer));
                return RET_OK;
            }
            // 被其他占用（比如温度查询），此种类型秒级占用，等待超时退出。
            vos_task_delay(NOTIFY_DELAY_TIME);
            continue;
        } else {
            break;
        }
    }

    // 超时未等到空闲，异常退出
    if (is_busy == RETIMER_BUSY) {
        debug_log(DLOG_ERROR, "%s: wait for i2c bus time out.", __FUNCTION__);
        return RET_OK;
    }

    // 占用芯片
    (void)dal_set_property_value_byte(retimer, PROPERTY_RETIMER_REQ_ACC, RETIMER_BUSY, DF_AUTO);

    gchar file_path[RETIMER_LOG_PATH_MAX_LEN] = {0};
    if (sprintf_s(file_path, sizeof(file_path), "%s%s", RETIMER_LOG_PATH_BASE, dfl_get_object_name(retimer)) <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed.", __FUNCTION__);
        return RET_OK;
    }

    // 存在一种情况是带电情况下从不支持本功能的版本，升级到支持本功能的版本，需要按照上电来初始化
    if (access(file_path, F_OK) != 0) {
        __retimer_get_logs_initial(retimer, NULL);
        debug_log(DLOG_ERROR, "%s: The BMC is upgraded to a version that supports log collection for the first time.",
            __FUNCTION__);
    }

    check_get_retimer_logs(retimer, dump_flag);
    // 释放芯片
    (void)dal_set_property_value_byte(retimer, PROPERTY_RETIMER_REQ_ACC, RETIMER_IDLE, DF_AUTO);

    return RET_OK;
}


LOCAL gint32 create_logs_name(OBJ_HANDLE retimer, gchar **name_buff, gint32 len, const gchar *path)
{
    gint32 ret = RET_OK;
    gint32 i;
    gchar *name = NULL;

    for (i = 1; i < len; i++) {
        name = (gchar *)g_malloc0(RETIMER_LOG_PATH_MAX_LEN + 1);
        ret = sprintf_s(name, RETIMER_LOG_PATH_MAX_LEN + 1, "%s%s%s%d%s", RETIMER_LOG_PATH_BASE,
            dfl_get_object_name(retimer), path, i, ".log");
        if (ret <= 0) {
            g_free(name);
            debug_log(DLOG_ERROR, "%s: sprintf failed, ret = %d.", __FUNCTION__, ret);
            break;
        }
        name_buff[i] = name;
    }

    return ret;
}

LOCAL void destroy_retimer_bind_data(gpointer data)
{
    RetimerPCIeStatus *retimer_bind_data = (RetimerPCIeStatus *)data;
    gint32 len = RETIMER_LOG_NUM + 1;

    if (retimer_bind_data == NULL) {
        return;
    }

    for (gint32 i = 1; i < len; i++) {
        if (retimer_bind_data->ram_log_name[i] != NULL) {
            g_free(retimer_bind_data->ram_log_name[i]);
            retimer_bind_data->ram_log_name[i] = NULL;
        }
 
        if (retimer_bind_data->pcie_log_name[i] != NULL) {
            g_free(retimer_bind_data->pcie_log_name[i]);
            retimer_bind_data->pcie_log_name[i] = NULL;
        }
 
        if (retimer_bind_data->serdes_log_name[i] != NULL) {
            g_free(retimer_bind_data->serdes_log_name[i]);
            retimer_bind_data->serdes_log_name[i] = NULL;
        }
    }
 
    g_free(retimer_bind_data);
}


LOCAL void retimer_bind_data_by_handle(OBJ_HANDLE obj_handle, RetimerPCIeStatus *retimer_bind_data)
{
    gint32 core_id;
    gint32 port_id;
    guint32 last_time;

    for (core_id = 0; core_id < RETIMER_CORE_NUM; core_id++) {
        for (port_id = 0; port_id < RETIMER_CORE_NUM; port_id++) {
            last_time = 0;
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_RETIMER_ERR1_TIME, &last_time);
            retimer_bind_data->link_status_tick[core_id][port_id].link_status_tick = last_time;
            last_time = 0;
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_RETIMER_ERR2_TIME, &last_time);
            retimer_bind_data->link_status_tick[core_id][port_id].link_speed_tick = last_time;
            last_time = 0;
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_RETIMER_ERR3_TIME, &last_time);
            retimer_bind_data->link_status_tick[core_id][port_id].link_width_tick = last_time;
        }
    }
}


LOCAL gint32 create_retimer_bind_data(OBJ_HANDLE retimer, gpointer user_data)
{
    gint32 ret;
    guint8 retimer_type = RETIMER_5902_TYPE_L;
    RetimerPCIeStatus *retimer_bind_data = NULL;

    (void)dal_get_property_value_byte(retimer, PROPERTY_RETIMER_CHIP_TYPE, &retimer_type);
    if (retimer_type == RETIMER_5902_TYPE_L) {
        if (!dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) { 
            return RET_OK;
        }
    }

    retimer_bind_data = (RetimerPCIeStatus *)g_malloc0(sizeof(RetimerPCIeStatus));
    if (retimer_bind_data == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid arguments: retimer_bind_data is NULL", __FUNCTION__);
        return RET_ERR;
    }
    ret = dfl_bind_object_data(retimer, (gpointer)retimer_bind_data, destroy_retimer_bind_data);
    if (ret != DFL_OK) {
        g_free(retimer_bind_data);
        debug_log(DLOG_ERROR, "bind object data to %s failed, ret %d.", dfl_get_object_name(retimer), ret);
        return RET_OK;
    }

    // 重启初始化
    // 文件名提前准备好，省去临时拼接的时间
    if (create_logs_name(retimer, retimer_bind_data->ram_log_name, RETIMER_LOG_NUM + 1, RETIMER_LOG_PATH_RAM) <= 0) {
        debug_log(DLOG_ERROR, "%s: create ram names failed, ret = %d.", __FUNCTION__, ret);
        return RET_OK;
    }
    if (create_logs_name(retimer, retimer_bind_data->pcie_log_name, RETIMER_LOG_NUM + 1, RETIMER_LOG_PATH_PCIE) <= 0) {
        debug_log(DLOG_ERROR, "%s: create pcie names failed, ret = %d.", __FUNCTION__, ret);
        return RET_OK;
    }
    if (create_logs_name(retimer, retimer_bind_data->serdes_log_name, RETIMER_LOG_NUM + 1, RETIMER_LOG_PATH_SERDES) <=
        0) {
        debug_log(DLOG_ERROR, "%s: create serdes names failed, ret = %d.", __FUNCTION__, ret);
        return RET_OK;
    }

    retimer_bind_data_by_handle(retimer, retimer_bind_data);

    return RET_OK;
}


gint32 retimer_add_object_callback(OBJ_HANDLE obj_handle)
{
    gint32 ret = dal_set_property_value_byte(obj_handle, PROPERTY_RETIMER_REQ_ACC, RETIMER_IDLE, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Set channel notify %d failed(ret: %d).", __FUNCTION__, RETIMER_IDLE, ret);
    }
    
    if (g_retimer_scan_flg == 0) {
        g_retimer_scan_flg = 1;
        (void)retimer_manage_start();
    }
    return create_retimer_bind_data(obj_handle, NULL);
}


gint32 retimer_del_object_callback(OBJ_HANDLE obj_handle)
{
    return RET_OK;
}


LOCAL void retimer_get_logs_task(void)
{
    guint8 power_state = 0;
    guint8 pre_power_state = 0;
    gint32 reset_type = RESET_TYPE_POWER_UP;
    guint8 dump_flag = RETIMER_LOG_CYCLE;

    (void)prctl(PR_SET_NAME, (unsigned long)"retimer_get_logs");

    // 初始化一些重启会丢失的数据
    (void)dfl_foreach_object(CLASS_RETIMER_NAME, create_retimer_bind_data, NULL, NULL);

    gint32 ret = dal_get_bmc_reset_type(&reset_type);
    if (ret != RET_OK) {
        // 失败了按照默认的AC处理
        debug_log(DLOG_ERROR, "%s get BMC reset type failed, ret = %d", __FUNCTION__, ret);
    }

    OBJ_HANDLE payload_handle = 0;
    OBJ_HANDLE smbios_handle = 0;
    (void)dal_get_object_handle_nth(CLASS_CHASSISPAYLOAD, 0, &payload_handle);
    (void)dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &smbios_handle);

    if (reset_type != RESET_TYPE_POWER_UP) {
        (void)dal_get_property_value_byte(payload_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &pre_power_state);
    }

    while (1) {
        (void)dal_get_property_value_byte(payload_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &power_state);

        // 等待启动完成
        if (retimer_get_all_status_ready() != RETIMER_HAS_READY || power_state != PAYLOAD_POWER_STATE_ON) {
            (void)vos_task_delay(RETIMER_DELAY_WAIT_PG);
            debug_log(DLOG_DEBUG, "%s: Wait for BIOS initialization to complete.", __FUNCTION__);
            continue;
        }

        g_retimer_log_sys_reset_flg = 0;
        // 检测到上电
        if ((power_state == PAYLOAD_POWER_STATE_ON && power_state != pre_power_state) ||
            (g_retimer_log_power_off_flg == 1)) {
            g_retimer_log_power_off_flg = 0;
            debug_log(DLOG_ERROR, "%s: The power-on is complete and the initialization starts.", __FUNCTION__);
            // 初始化一些os掉电需要重新初始化的数据
            (void)dfl_foreach_object(CLASS_RETIMER_NAME, __retimer_get_logs_initial, NULL, NULL);
        }
        pre_power_state = power_state;

        if (read_retimer_routine_open_status() != RETIMER_OPEN_ROUTINE) { 
            write_retimer_routine_open_status(RETIMER_ALREADY_CLOSE_ROUTINE);
            continue;
        }

        // 开始遍历Retimer
        (void)dfl_foreach_object(CLASS_RETIMER_NAME, get_retimer_logs_foreach, &dump_flag, NULL);
        vos_task_delay(RETIMER_DELAY_BETWEEN_COLLECT);
    }
}


gint32 retimer_get_logs_dump(void)
{
    guint8 power_state = 0;
    OBJ_HANDLE payload_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_CHASSISPAYLOAD, 0, &payload_handle);
    (void)dal_get_property_value_byte(payload_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &power_state);

    // chip is not ready seders 收集失败
    if (retimer_get_all_status_ready() != RETIMER_HAS_READY || power_state != PAYLOAD_POWER_STATE_ON) {
        debug_log(DLOG_ERROR, "%s: Wait for BIOS initialization to complete.", __FUNCTION__);
        return RET_ERR;
    }

    guint8 dump_flag = RETIMER_LOG_DUMP;
    (void)dfl_foreach_object(CLASS_RETIMER_NAME, get_retimer_logs_foreach, &dump_flag, NULL);
    return RET_OK;
}


gint32 retimer_manage_start(void)
{
    gulong taskid;
    gulong task_info;
    gulong task_log;
    gint32 ret;
    gint32 chip_type;

    chip_type = retimer_get_type();
    if (chip_type == 0xff) { 
        g_retimer_scan_flg = 0; 
        return RET_OK;
    }
    retimer_driver_init(chip_type);

    if (dal_match_software_type(MGMT_SOFTWARE_TYPE_PANGEA_MGNT)) {
        if (chip_type == CDR_CHIP_TYPE_DS280) {
            ret = vos_task_create(&taskid, "retimer_scan", (TASK_ENTRY)retimer_scan_task_ds280, NULL, DEFAULT_PRIORITY);
        } else {
            retimer_dft_init(); 
            ret = vos_task_create(&taskid, "retimer_scan", (TASK_ENTRY)retimer_scan_task, NULL, DEFAULT_PRIORITY);
        }

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Creat retimer_scan failed! ret = %d", ret);
        }
    } else {
        ret =
            vos_task_create(&task_info, "retimer_get_data", (TASK_ENTRY)retimer_get_data_task, NULL, DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Creat retimer_get_data_task failed! ret = %d", ret);
        }
        ret = vos_task_create(&taskid, "retimer_get_data_from_ipmb", (TASK_ENTRY)cdr_task_monitor_scan, NULL,
            DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Creat retimer_get_data_from_ipmb failed! ret = %d", ret);
        }
        ret = vos_task_create(&task_log, "retimer_get_logs", (TASK_ENTRY)retimer_get_logs_task, NULL, DEFAULT_PRIORITY);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Creat retimer_get_logs_task failed! ret = %d", ret);
        }
    }

    return ret;
}
