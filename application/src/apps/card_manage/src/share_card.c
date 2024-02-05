

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include <unistd.h>
#include <dirent.h>

#include "pme/pme.h"
#include "pme_app/pme_app.h"


#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "pme_app/common/debug_log_macro.h"
#include "pme/haslib/haslib.h"
#include "pcie_card_comm.h"
#include "get_version.h"
#include "share_card.h"

LOCAL GSList *g_pangea_card_action_list = NULL;
LOCAL pthread_mutex_t g_card_action_list_mutex = PTHREAD_MUTEX_INITIALIZER;
LOCAL FM_REGISTER_S g_pangea_card_log_file[] = {
    {CARD_MCU_LOG, CARD_MCU_LOG_DIR"/"CARD_MCU_LOG, FM_PROTECT_POWER_OFF, (100 * 1024), NULL,
        CARD_MCU_LOG_DIR"/"CARD_MCU_LOG".tar.gz", (FM_PRIVILEGE_RGRP), NULL, NULL},
};


LOCAL gint32 share_card_write_read_from_mcu(guint8 slot, OBJ_HANDLE chip_handle, PANGEA_CARD_WRITE_READ_S input)
{
    gint32 ret;
    gint32 read_len;
    guint8 *frame_buf = NULL;
    guint32 buf_len;
    gint32 offset;

    
    buf_len = IOCARD_IIC_READ_HEADLEN + input.write_len + input.read_len + IOCARD_IIC_PEC_LEN;
    frame_buf = (guint8 *)g_malloc0(buf_len);
    if (frame_buf == NULL) {
        debug_log(DLOG_ERROR, "malloc mem failed");
        return RET_ERR;
    }

    frame_buf[0] = input.chip_addr; 
    frame_buf[1] = input.cmd;       
    frame_buf[IOCARD_IIC_WRITE_HEADLEN + input.write_len] = input.chip_addr | IOCARD_IIC_READ_BIT;
    ret = memcpy_s(&frame_buf[IOCARD_IIC_WRITE_HEADLEN], input.write_len, input.write_data, input.write_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "memcpy_s data to data buffer failed, ret=%d", ret);
        g_free(frame_buf);
        return ret;
    }

    read_len = IOCARD_IIC_READ_HEADLEN + input.write_len;
    offset = MAKE_WORD(input.cmd, frame_buf[IOCARD_IIC_WRITE_HEADLEN]);
    ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), offset, input.read_len + IOCARD_IIC_PEC_LEN,
        frame_buf + read_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s read data from mcu failed, ret=%d.", dfl_get_object_name(chip_handle), ret);
        g_free(frame_buf);
        return ret;
    }

    ret = share_card_check_read_data(frame_buf, buf_len);
    if (ret != RET_OK) {
        card_err_log_limit(slot, ret, "%s check crc failed.", dfl_get_object_name(chip_handle));
        g_free(frame_buf);
        return ret;
    }

    ret = memcpy_s(input.read_data, input.read_len, &frame_buf[IOCARD_IIC_READ_HEADLEN + input.write_len],
        input.read_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "memcpy_s data to data buffer failed, ret=%d", ret);
    }

    g_free(frame_buf);
    return ret;
}


LOCAL void init_log_info(void)
{
    gint32 ret;

    
    if (vos_check_dir(CARD_MCU_LOG_DIR) == FALSE) {
        debug_log(DLOG_ERROR, "Make diretory(%s) failed.", CARD_MCU_LOG_DIR);
        return;
    }

    (void)chmod(CARD_MCU_LOG_DIR, 0750); 
    ret = fm_init(g_pangea_card_log_file, 1);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Init pangea card mcu log file info failed, ret=%d.", ret);
        return;
    }
}


LOCAL gint32 pangea_card_init_state_info(PANGEA_CARD_STATE_INFO *card_state, OBJ_HANDLE action_obj)
{
    gint32 ret;
    if (card_state == NULL || action_obj == INVALID_OBJ_HANDLE) {
        return VOS_ERR;
    }

    card_state->cur_state = PANGEA_CARD_ABSENT;
    card_state->pre_state = PANGEA_CARD_ABSENT;
    card_state->is_power_on = FALSE;
    card_state->load_state = CARD_STATE_PLUG_OUT;
    card_state->card_action_obj = action_obj;
    ret = dal_get_property_value_byte(action_obj, PROPERTY_CARD_ACTION_DATA, &(card_state->slot_id));
    if (ret != RET_OK) {
        card_state->slot_id = INVALID_SLOT_ID;
    }

    ret = pcie_card_get_connector_handle(card_state->slot_id, &(card_state->mcu_conn_obj), CHIP_CONNECTOR);
    if (ret != RET_OK) {
        card_state->mcu_conn_obj = INVALID_OBJ_HANDLE;
    }

    ret = pcie_card_get_connector_handle(card_state->slot_id, &(card_state->card_conn_obj), PANGEA_CARD_CONNECTOR);
    if (ret != RET_OK) {
        card_state->card_conn_obj = INVALID_OBJ_HANDLE;
    }

    ret = dal_get_refobject_handle_nth(card_state->card_conn_obj, PROPERTY_CONNECTOR_LEGACY, 0, &(card_state->mcu_obj));
    if (ret != RET_OK) {
        card_state->mcu_obj = INVALID_OBJ_HANDLE;
    }

    return RET_OK;
}


gint32 init_pangea_card_action(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    guint8 type;
    PANGEA_CARD_STATE_INFO *priv_data = NULL;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CARD_ACTION_TYPE, &type);
    if (ret != RET_OK) {
        return ret;
    }
    if (type != PANGEA_CARD_CONNECTOR) { 
        return RET_OK;
    }

    priv_data = (PANGEA_CARD_STATE_INFO *)g_malloc0(sizeof(PANGEA_CARD_STATE_INFO));
    if (priv_data == NULL) {
        debug_log(DLOG_ERROR, "malloc priv_data mem failed.");
        return VOS_ERR;
    }
    ret = pangea_card_init_state_info(priv_data, obj_handle);
    if (ret != RET_OK) {
        g_free(priv_data);
        return VOS_ERR;
    }

    ret = dfl_bind_object_data(obj_handle, priv_data, g_free);
    if (ret != DFL_OK) {
        g_free(priv_data);
        debug_log(DLOG_ERROR, "dfl_bind_object_data fail!result = %d", ret);
        return ret;
    }
    (void)pthread_mutex_lock(&g_card_action_list_mutex);
    g_pangea_card_action_list = g_slist_append(g_pangea_card_action_list, GUINT_TO_POINTER(obj_handle));
    (void)pthread_mutex_unlock(&g_card_action_list_mutex);
    return RET_OK;
}


gint32 pangea_card_action_add_object_callback(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    ret = init_pangea_card_action(obj_handle, NULL);
    if (ret != RET_OK) {
        return VOS_ERR;
    }
    return RET_OK;
}


gint32 pangea_card_action_del_object_callback(OBJ_HANDLE obj_handle)
{
    (void)pthread_mutex_lock(&g_card_action_list_mutex);
    g_pangea_card_action_list = g_slist_remove(g_pangea_card_action_list, GUINT_TO_POINTER(obj_handle));
    (void)pthread_mutex_unlock(&g_card_action_list_mutex);

    return RET_OK;
}


gint32 pangea_card_add_object_callback(OBJ_HANDLE obj_handle)
{
    card_manage_log_operation_log("Pcie Card", obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, NULL, CARD_PLUG_IN);
    return RET_OK;
}

gint32 pangea_card_del_object_callback(OBJ_HANDLE obj_handle)
{
    card_manage_log_operation_log("Pcie Card", obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, NULL, CARD_PLUG_OUT);
    return RET_OK;
}


gint32 pangea_card_get_mcu_ver(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 retry_times;
    guint8 mcu_ver[IOCARD_MCU_VER_LEN + 1] = { 0 };

    if (obj_handle == INVALID_OBJ_HANDLE) {
        return VOS_ERR;
    }

    for (retry_times = 0; retry_times < 3; retry_times++) { 
        ret = dfl_block_read(obj_handle, PROPERTY_PANGEA_PCIE_MCUVER, MCU_IIC_VER_OUT, IOCARD_MCU_VER_LEN,
            (gpointer)mcu_ver);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "%s get card mcu version failed, ret=%d.", dfl_get_object_name(obj_handle), ret);
            continue;
        } else {
            ret = dal_set_property_value_string(obj_handle, PROPERTY_PANGEA_PCIE_MCUVER,
                (const gchar *)(mcu_ver + MCU_VER_HEAD_LENGTH), DF_AUTO);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s set share card mcu ver to xml failed, ret=%d.",
                    dfl_get_object_name(obj_handle), ret);
            }
        }

        if (ret == RET_OK) {
            break;
        }
    }

    debug_log(DLOG_INFO, "%s mcu ver:%s", dfl_get_object_name(obj_handle), (mcu_ver + MCU_VER_HEAD_LENGTH));
    return RET_OK;
}


LOCAL gint32 pangea_card_update_static_info(PANGEA_CARD_STATE_INFO *card_state)
{
    gint32 ret;
    OBJ_HANDLE card_obj;

    if (card_state->slot_id == INVALID_SLOT_ID) {
        return VOS_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPETRY_PCIECARD_SLOT, card_state->slot_id, &card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %d card obj failed, ret=%d", card_state->slot_id, ret);
        return ret;
    }

    ret = pangea_card_get_mcu_ver(card_obj); 
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get mcu ver failed, ret=%d", ret);
    }

    init_elabel_info(card_obj);
    init_log_info();
    return ret;
}


LOCAL void pangea_card_syn_mcu_time(OBJ_HANDLE card_obj)
{
    OBJ_HANDLE chip_obj = INVALID_OBJ_HANDLE;
    struct tm cur_time = { 0 };
    SYS_TIME time_value = { 0 }; 
    time_t time_temp;
    gint32 ret;

    time_temp = time((time_t *)NULL);
    if (time_temp < 0) {
        debug_log(DLOG_DEBUG, "Get sys time failed.");
        return;
    }

    localtime_r(&time_temp, &cur_time);
    time_value.year = cur_time.tm_year + 1900; 
    time_value.month = cur_time.tm_mon + 1;    
    time_value.day = cur_time.tm_mday;
    time_value.hours = cur_time.tm_hour;
    time_value.minutes = cur_time.tm_min;
    time_value.second = cur_time.tm_sec;

    ret = dfl_get_referenced_object(card_obj, PROPERTY_MCU_CHIP, &chip_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s get ref obj handle failed, ret=%d.", dfl_get_object_name(card_obj), ret);
        return;
    }

    ret = share_card_send_data_to_mcu(chip_obj, MCU_IIC_SYS_SYNC_TIME, (guint8 *)&time_value, sizeof(time_value));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s sync system time failed, ret=%d.", dfl_get_object_name(card_obj), ret);
    }
}


LOCAL gint32 pangea_card_get_mcu_log(OBJ_HANDLE card_obj, guint8 *log_buf, gint32 buf_size)
{
    OBJ_HANDLE chip_handle = INVALID_OBJ_HANDLE;
    guint8 slot = INVALID_SLOT_ID;
    gint32 ret;
    guint32 chip_addr;

    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get slot failed, ret=%d.", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    ret = dfl_get_referenced_object(card_obj, PROPERTY_PCIE_CARD_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s get ref obj fail, ret=%d.", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    ret = dal_get_property_value_uint32(chip_handle, PROPERTY_MCU_CHIP_ADDR, &chip_addr);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get chip addr failed,ret=%d.", dfl_get_object_name(chip_handle), ret);
        return ret;
    }

    PANGEA_CARD_WRITE_READ_S input = { 0 };
    input.chip_addr = chip_addr;
    input.cmd = (gint32)MCU_IIC_LOG_READ;
    input.read_data = log_buf;
    input.read_len = buf_size;
    input.write_data = &slot;
    input.write_len = sizeof(guint8);
    ret = share_card_write_read_from_mcu(slot, chip_handle, input);
    if (ret == RET_OK && log_buf[0] != 0) { 
        return RET_OK;
    }
    return RET_ERR;
}


LOCAL void pangea_card_save_mcu_log(const gchar *short_name, gchar *log_buf, gint32 len)
{
    FM_FILE_S *file_handle = NULL;

    file_handle = fm_fopen(short_name, "a+");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "Open %s file fail.", short_name);
        return;
    }

    fm_fwrite(log_buf, len, sizeof(gchar), file_handle);
    fm_fclose(file_handle);
    return;
}


LOCAL void pangea_card_collect_mcu_log(OBJ_HANDLE card_obj)
{
    guint8 mcu_log[64 + 1] = {0}; 
    gint32 ret;
    guint8 slot;
    gchar log_buf[MAX_LEN_PER_LOG] = {0}; 
    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        return;
    }

    
    ret = pangea_card_get_mcu_log(card_obj, mcu_log, sizeof(mcu_log) - 1); 
    if (ret != RET_OK) {
        return;
    }

    
    ret = sprintf_s(log_buf, MAX_LEN_PER_LOG - 1, "%s:%s\r\n", dfl_get_object_name(card_obj), mcu_log);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s mcu log content sprintf_s failed.", dfl_get_object_name(card_obj));
        return;
    }
    
    pangea_card_save_mcu_log((const gchar *)g_pangea_card_log_file[0].short_name, (gchar *)log_buf,
        strlen((const gchar *)log_buf));
}


LOCAL gint32 pangea_card_update_dynamic_info(PANGEA_CARD_STATE_INFO *card_state)
{
    gint32 ret;
    OBJ_HANDLE card_obj;

    if (card_state->slot_id == INVALID_SLOT_ID) {
        return VOS_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPETRY_PCIECARD_SLOT, card_state->slot_id, &card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %d card obj failed, ret=%d", card_state->slot_id, ret);
        return ret;
    }

    ret = pangea_card_update_temp(card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get mcu temp failed, ret=%d", ret);
    }

    pangea_card_syn_mcu_time(card_obj);
    pangea_card_collect_mcu_log(card_obj);
    return ret;
}


LOCAL gint32 check_pangea_card_hotplug(PANGEA_CARD_STATE_INFO *card_info, guint8 *pst_state)
{
    gint32 ret;
    guint8 pst_change_state = 1; 
    OBJ_HANDLE action_obj = card_info->card_action_obj;

    
    if (action_obj == INVALID_OBJ_HANDLE) {
        return RET_OK;
    }

    ret = dal_get_extern_value_byte(action_obj, PROPERTY_CARD_ACTION_DESTINATION, &pst_change_state, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get card action %s failed, ret=%d.", PROPERTY_CARD_ACTION_DESTINATION, ret);
        return VOS_ERR;
    }

    if (pst_change_state == 0) {
        ret = dal_set_property_value_byte(action_obj, PROPERTY_CARD_ACTION_DESTINATION, 1, DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set card action %s failed, ret=%d.", PROPERTY_CARD_ACTION_DESTINATION, ret);
            return VOS_ERR;
        }
    }

    *pst_state = pst_change_state;
    return RET_OK;
}


LOCAL void set_pangea_card_load_state(PANGEA_CARD_STATE_INFO *load_state)
{
    gint32 ret;
    guint8 pst_change = PANGEA_CARD_NOT_HOT_PLUG;
    CARD_STATE_E cur_load_state = load_state->load_state;

    
    ret = check_pangea_card_hotplug(load_state, &pst_change);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get card present change state failed.");
        return;
    }

    
    if (pst_change == PANGEA_CARD_HOT_PLUG && load_state->cur_state == PANGEA_CARD_PRESENT) {
        if (load_state->pre_state == PANGEA_CARD_ABSENT) {
            cur_load_state = CARD_STATE_ASK_ACTIVE;
        } else if (load_state->pre_state == PANGEA_CARD_PRESENT) {
            load_state->is_power_on = FALSE;
            cur_load_state = CARD_STATE_INACTIVING;
        }
    }

    
    if (pst_change == PANGEA_CARD_NOT_HOT_PLUG && load_state->cur_state == PANGEA_CARD_ABSENT) {
        if (load_state->pre_state == PANGEA_CARD_PRESENT) {
            cur_load_state = CARD_STATE_INACTIVING;
        } else {
            cur_load_state = CARD_STATE_PLUG_OUT;
        }
    }

    
    if (pst_change == PANGEA_CARD_NOT_HOT_PLUG && load_state->pre_state == PANGEA_CARD_PRESENT) {
        
        if ((load_state->load_state == CARD_STATE_PLUG_OUT) || (load_state->load_state == CARD_STATE_INACTIVING)) {
            cur_load_state = CARD_STATE_ASK_ACTIVE;
        }
        if (load_state->load_state == CARD_STATE_ASK_ACTIVE && load_state->is_power_on) {
            cur_load_state = CARD_STATE_ACTIVED;
        }
    }
    load_state->pre_state = load_state->cur_state;
    load_state->load_state = cur_load_state;
    debug_log(DLOG_INFO, "card%d pre_state=%d, load_state=%d.", load_state->slot_id, load_state->cur_state,
        load_state->load_state);
    return;
}


LOCAL void pangea_card_load_state_machine(PANGEA_CARD_STATE_INFO *load_state)
{
    gint32 ret;
    OBJ_HANDLE mcu_conn = load_state->mcu_conn_obj;

    if (mcu_conn == INVALID_OBJ_HANDLE) {
        return;
    }

    ret = dal_get_property_value_byte(mcu_conn, PROPERTY_CONNECTOR_PRESENT, &(load_state->cur_state));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get connector present state failed, ret=%d.", dfl_get_object_name(mcu_conn), ret);
        return;
    }

    set_pangea_card_load_state(load_state);
    return;
}


LOCAL gint32 pangea_card_update_card_type(OBJ_HANDLE chip_obj, guint32 type)
{
    gint32 ret;
    OBJ_HANDLE card_type_obj;

    ret = dal_get_specific_position_object_string(chip_obj, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR,
        PROPERTY_P_DEVICE_SENSOR_NAME, CARD_TYPE_SENOR_NAME, &card_type_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get %s %s obj failed.", CARD_TYPE_SENOR_NAME, CLASS_NAME_PERIPHERAL_DEVICE_SENSOR);
        return ret;
    }

    ret = dal_set_property_value_uint16(card_type_obj, PROPERTY_P_DEVICE_SENSOR_VALUE, (guint16)type, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Set %s %s value failed, ret=%d.", CARD_TYPE_SENOR_NAME,
            CLASS_NAME_PERIPHERAL_DEVICE_SENSOR, ret);
    }

    return ret;
}


LOCAL gboolean check_card_xml_exist(OBJ_HANDLE conn_obj, guint32 card_type)
{
    gint32 ret;
    gchar xml_name[PCIE_CARD_XMLFILE_MAX_LEN + 1] = {0};
    gchar bom_val[CONNECTOR_BOM_ID_MAX_LEN] = {0};

    ret = dal_get_property_value_string(conn_obj, PROPERTY_CONNECTOR_BOM, bom_val, sizeof(bom_val));
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "Get %s bom value failed, ret=%d.", dfl_get_object_name(conn_obj), ret);
        return FALSE;
    }

    ret = snprintf_s(xml_name, PCIE_CARD_XMLFILE_MAX_LEN + 1, PCIE_CARD_XMLFILE_MAX_LEN,
                     "/opt/pme/extern/profile/%s_%08x_%08x.xml", bom_val, card_type, 0);
    
    if (ret <= 0) {
        debug_log(DLOG_DEBUG, "Format xml file name failed, ret=%d.", ret);
        return FALSE;
    }
    
    if (access(xml_name, F_OK) == 0) {
        return TRUE;
    }
    return FALSE;
}


LOCAL gint32 pangea_check_card_type(PANGEA_CARD_STATE_INFO* card_state, guint8* card_type, guint32* type)
{
    gint32 ret;
    guint8 hw_desc[CARD_HW_DESC_LEN + 1] = {0}; 
    gboolean is_exist;

    *type = MAKE_WORD(card_type[0], card_type[1]);
    ret = pangea_card_update_card_type(card_state->mcu_obj, *type);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "Get %s card type failed, ret=%d.", dfl_get_object_name(card_state->mcu_obj), ret);
        return ret;
    }

    is_exist = check_card_xml_exist(card_state->card_conn_obj, *type);
    if (is_exist == TRUE) {
        return RET_OK;
    }

    ret = share_card_read_data_from_mcu(card_state->slot_id, card_state->mcu_obj, MCU_IIC_HW_SELFDES,
                                        hw_desc, sizeof(hw_desc));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get hw desc failed, ret=%d.", dfl_get_object_name(card_state->mcu_obj), ret);
        return ret;
    }

    if (hw_desc[0] == CARD_DESC_FLAG_VALID) {
        *type = MAKE_WORD(hw_desc[2], hw_desc[1]); 
    }

    is_exist = check_card_xml_exist(card_state->card_conn_obj, *type);
    if (is_exist == TRUE) {
        debug_log(DLOG_INFO, "%s get hw desc(0x%x) successfully.", dfl_get_object_name(card_state->mcu_obj), *type);
        return RET_OK;
    }

    debug_log(DLOG_INFO, "Get pangea card%d type:0x%x.", card_state->slot_id, *type);
    return RET_ERR;
}


LOCAL gint32 pangea_card_load_xml(PANGEA_CARD_STATE_INFO *card_state, OBJ_HANDLE conn_obj, guint8 state)
{
    gint32 ret;
    guint8 card_type[4] = { 0 }; 
    guint32 type = INVALIDE_CARD_TYPE;

    if (card_state->mcu_obj == INVALID_OBJ_HANDLE || conn_obj == INVALID_OBJ_HANDLE) {
        return VOS_ERR;
    }

    ret = share_card_read_data_from_mcu(card_state->slot_id, card_state->mcu_obj,
        MCU_IIC_SYS_SYNC_CARDTYPE, card_type, sizeof(card_type));
    
    if (ret == RET_OK && MAKE_DWORD(card_type[0], card_type[1], card_type[2], card_type[3]) == PCIE_NO_CARD) {
        ret = share_card_read_data_from_mcu(card_state->slot_id, card_state->mcu_obj,
            MCU_IIC_READ_CARDTYPE, card_type, sizeof(card_type));
    }

    if (ret == CARD_TYPE_CRC_VALIDATE_FAILED) {
        debug_log(DLOG_DEBUG, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_state->mcu_obj), ret);
        (void)pangea_card_update_card_type(card_state->mcu_obj, CARD_TYPE_CRC_INVALIDE);
        return CARD_TYPE_CRC_VALIDATE_FAILED;
    } else if (ret == GET_CARD_TYPE_FAILED) {
        debug_log(DLOG_DEBUG, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_state->mcu_obj), ret);
        (void)pangea_card_update_card_type(card_state->mcu_obj, type);
        return GET_CARD_TYPE_FAILED;
    }

    ret = pangea_check_card_type(card_state, card_type, &type);
    if (ret != RET_OK) {
        return ret;
    }

    ret = pcie_card_load_xml(conn_obj, type, 0, state, PANGEA_CARD_CONNECTOR);
    if (ret != RET_OK) {
        return VOS_ERR;
    }

    return RET_OK;
}


LOCAL void pangea_card_deal_diff_state(PANGEA_CARD_STATE_INFO *card_state)
{
    pangea_card_load_state_machine(card_state);
    switch (card_state->load_state) {
        case CARD_STATE_PLUG_OUT: 
            break;

        case CARD_STATE_ASK_ACTIVE:                                                           
            pangea_card_load_xml(card_state, card_state->card_conn_obj, PANGEA_CARD_PRESENT); 
            pangea_card_power_on(card_state);
            pangea_card_update_static_info(card_state);
            break;

        case CARD_STATE_ACTIVED: 
            pangea_card_update_dynamic_info(card_state);
            break;

        case CARD_STATE_INACTIVING:                                                         
            pangea_card_load_xml(card_state, card_state->mcu_conn_obj, PANGEA_CARD_ABSENT); 
            break;

        default:
            break;
    }
    return;
}


LOCAL void pangea_card_monitor_task(gpointer *param)
{
    gint32 ret;
    OBJ_HANDLE action_obj;
    GSList *node = NULL;
    PANGEA_CARD_STATE_INFO *card_info = NULL;

    
    ret = pangea_card_get_connector_list(CONNECTOR_TYPE_CHIP, NULL);
    if (ret != TRUE) {
        return;
    }

    for (;;) {
        vos_task_delay(1000); 
        (void)pthread_mutex_lock(&g_card_action_list_mutex);
        for (node = g_pangea_card_action_list; node; node = node->next) {
            action_obj = (OBJ_HANDLE)node->data;
            ret = dfl_get_binded_data(action_obj, (gpointer *)&card_info);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "%s get binded data failed, ret=%d.", dfl_get_object_name(action_obj), ret);
                continue;
            }

            if (card_info != NULL) {
                pangea_card_deal_diff_state(card_info);
            }
        }
        (void)pthread_mutex_unlock(&g_card_action_list_mutex);
    }
}


gint32 pangea_card_get_connector_list(gchar *conn_type, GSList **conn_list)
{
    gint32 ret;
    gint32 is_find = FALSE;
    OBJ_HANDLE object_handle;
    gchar connector_type_string[CONNECTOR_TYPE_LEN] = { 0 };
    GSList *connector_list = NULL;
    GSList *node = NULL;

    ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &connector_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get connector obj list failed, ret=%d.", ret);
        return FALSE;
    }
    for (node = connector_list; node; node = node->next) {
        object_handle = (OBJ_HANDLE)node->data;
        (void)memset_s(connector_type_string, sizeof(connector_type_string), 0, sizeof(connector_type_string));
        ret = dal_get_property_value_string(object_handle, PROPERTY_CONNECTOR_TYPE, &connector_type_string[0],
            CONNECTOR_TYPE_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get connector type:%s failed.", connector_type_string);
            continue;
        }

        if (!strncmp(conn_type, connector_type_string, strlen(conn_type))) {
            is_find = TRUE;
            if (conn_list != NULL) {
                debug_log(DLOG_INFO, "Get %s connector.", connector_type_string);
                *conn_list = g_slist_append(*conn_list, (gpointer)object_handle);
            }
        }
    }
    g_slist_free(connector_list);
    return is_find;
}


LOCAL gint32 pangea_card_set_insert_ok(OBJ_HANDLE obj_handle, guint8 status)
{
    gint32 ret;
    guint8 insert_ok;

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_IOCARD_INSERTOK_WP, OPEN_INSERT_OK_WP, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s open insert_ok wp failed, ret=%d", dfl_get_object_name(obj_handle), ret);
    }

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_IOCARD_INSERTOK, status, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s set insert_ok failed, ret=%d", dfl_get_object_name(obj_handle), ret);
    }

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_IOCARD_INSERTOK_WP, CLOSE_INSERT_OK_WP, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s close insert_ok wp failed, ret=%d", dfl_get_object_name(obj_handle), ret);
    }

    
    ret = dal_get_extern_value_byte(obj_handle, PROPERTY_IOCARD_INSERTOK, &insert_ok, DF_COPY);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s get insert_ok value failed, ret=%d", dfl_get_object_name(obj_handle), ret);
        return ret;
    } else if (insert_ok != status) {
        debug_log(DLOG_MASS, "%s set actual insert_ok=%d, read insert_ok value=%d from reg.",
            dfl_get_object_name(obj_handle), status, insert_ok);
        return VOS_ERR;
    }

    debug_log(DLOG_DEBUG, "%s set insert ok value(0x%x) successful.", dfl_get_object_name(obj_handle), insert_ok);
    return RET_OK;
}


LOCAL gint32 pangea_card_get_insert_ok(OBJ_HANDLE obj_handle, guint8 *status)
{
    gint32 ret;
    guint8 power_status;

    ret = dal_get_extern_value_byte(obj_handle, PROPERTY_IOCARD_INSERTOK, &power_status, DF_COPY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get insert_ok value failed, ret=%d", dfl_get_object_name(obj_handle), ret);
        return VOS_ERR;
    }

    *status = power_status;
    return RET_OK;
}


gint32 pangea_card_power_on(PANGEA_CARD_STATE_INFO *card_state)
{
    guint8 power_state;
    OBJ_HANDLE card_obj;
    gint32 ret;

    if (card_state == NULL || card_state->slot_id == INVALID_SLOT_ID) {
        return VOS_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPETRY_PCIECARD_SLOT, card_state->slot_id, &card_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get slot%d card obj failed, ret=%d", card_state->slot_id, ret);
        return ret;
    }

    ret = pangea_card_get_insert_ok(card_obj, &power_state);
    if (ret != RET_OK) {
        return VOS_ERR;
    }

    
    if (power_state == PANGEA_CARD_STATE) {
        card_state->is_power_on = TRUE;
        return RET_OK;
    }
    
    ret = pangea_card_set_insert_ok(card_obj, PANGEA_CARD_STATE);
    if (ret == RET_OK) {
        card_state->is_power_on = TRUE;
        return ret;
    }

    return VOS_ERR;
}


LOCAL gint32 pangea_card_get_obj_by_slot(OBJ_HANDLE *card_handle, guint8 slot_id)
{
    OBJ_HANDLE obj_handle;
    guint8 node_slot_id;
    gint32 ret;
    GSList *card_list = NULL;
    GSList *node = NULL;

    ret = dfl_get_object_list(CLASS_PANGEA_PCIE_CARD, &card_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get share card list failed, ret=%d", ret);
        return ret;
    }

    for (node = card_list; node; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;
        ret = dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &node_slot_id);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s get card slot id failed, ret=%d", dfl_get_object_name(obj_handle), ret);
            continue;
        }

        if (node_slot_id != slot_id) {
            continue;
        }

        *card_handle = obj_handle;
        g_slist_free(card_list);
        return RET_OK;
    }
    g_slist_free(card_list);
    return VOS_ERR;
}


gint32 pangea_card_reset_mcu(guint8 slot_id, guint8 status)
{
    gint32 ret;
    OBJ_HANDLE card_handle;
    OBJ_HANDLE chip_handle;

    
    ret = pangea_card_get_obj_by_slot(&card_handle, slot_id);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_printf("%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    
    ret = share_card_send_data_to_mcu(chip_handle, MCU_IIC_SYS_SET_CHIP_RESET, (gpointer)&status, sizeof(status));
    if (ret != RET_OK) {
        debug_printf("%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }
    return RET_OK;
}


gint32 pangea_card_ipmi_reset_mcu(const void *msg_data, gpointer user_data)
{
    gint32 ret;
    guint8 slot_id;
    guint8 status = 1; 
    const guint8 *req_data_buf = NULL;

    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, "msg data is null");
        return VOS_ERR;
    }

    req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    slot_id = req_data_buf[0];
    ret = pangea_card_reset_mcu(slot_id, status);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


LOCAL gint32 pangea_card_get_chip_status(OBJ_HANDLE card_handle, guint8 *status)
{
    gint32 ret;
    OBJ_HANDLE chip_handle;

    ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    guint8 slot_id;
    ret = dal_get_property_value_byte(card_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    ret = share_card_read_data_from_mcu(slot_id, chip_handle, MCU_IIC_CORE_CHIP_READY, status, sizeof(guint8));
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get chip status failed, ret=%d", dfl_get_object_name(card_handle), ret);
    }
    return ret;
}


LOCAL gint32 pangea_card_get_coretemp_by_smbus(OBJ_HANDLE card_obj, guint16 *value)
{
    gint32 ret;
    PANGEA_CARD_1822_TEMP_RSP temp_data = { 0 };
    guint8 status = PANGEA_CARD_CHIP_NOT_READY;

    ret = pangea_card_get_chip_status(card_obj, &status);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get chip status failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    if (status != PANGEA_CARD_CHIP_READY) {
        debug_log(DLOG_DEBUG, "%s core chip status(%d).", dfl_get_object_name(card_obj), status);
        return RET_OK;
    }

    ret =
        dfl_block_read(card_obj, PROPERTY_PANGEA_CARD_CHIPTEMP, CORE_CHIP_CHIP_TEMP_CMD, sizeof(temp_data), &temp_data);
    if (ret == VOS_OK) {
        *value = (guint16)temp_data.data;
    }
    return ret;
}

LOCAL gint32 pangea_card_get_pg(OBJ_HANDLE card_handle, guint8 *status)
{
    gint32 ret;
    OBJ_HANDLE chip_handle;

    if (card_handle == INVALID_OBJ_HANDLE) {
        return VOS_ERR;
    }
    
    ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    guint8 slot_id;
    ret = dal_get_property_value_byte(card_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    
    ret = share_card_read_data_from_mcu(slot_id, chip_handle, MCU_IIC_SYS_READ_PG, status, sizeof(guint8));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_handle), ret);
    }

    return ret;
}


LOCAL gint32 pangea_card_get_cx6chip_temp(OBJ_HANDLE card_obj, guint16 *value)
{
    gint32 ret;
    guint8 pg_state;

    ret = pangea_card_get_pg(card_obj, &pg_state);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get card pg state failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    ret = dal_set_property_value_byte(card_obj, PROPERTY_PANGEA_CARD_PG_STATUS, pg_state, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s set card pg state failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    if (pg_state != PANGEA_CARD_PG_OK) {
        return RET_OK;
    }

    ret = dal_get_extern_value_uint16(card_obj, PROPERTY_PANGEA_CARD_CHIPTEMP, value, DF_HW);
    return ret;
}

LOCAL gint32 pangea_card_get_1882core_temp(OBJ_HANDLE card_obj)
{
    gint32 ret;
    guint16 value = 0x4000; 
    guint8 protocol;

    if (card_obj == INVALID_OBJ_HANDLE) {
        return VOS_ERR;
    }

    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_PROTOCOL, &protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get protocol failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    if (protocol == PCIE_MGNT_PROTOCOL_SMBUS) {
        ret = pangea_card_get_coretemp_by_smbus(card_obj, &value);
    } else if (protocol == 0xff) { 
        ret = pangea_card_get_cx6chip_temp(card_obj, &value);
    }

    if (ret != DFL_OK) {
        value = HW_ACCESS_FAILED;
        debug_log(DLOG_DEBUG, "get share card core temp failed, ret=%d.", ret);
    }

    debug_log(DLOG_DEBUG, "%s 1822 core temp = %d.", dfl_get_object_name(card_obj), value);
    ret = dal_set_property_value_uint16(card_obj, PROPERTY_PANGEA_CARD_CHIPTEMP, value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s set chip temp failed.", dfl_get_object_name(card_obj));
    }

    return ret;
}


LOCAL void pangea_card_update_sfp_temp(OBJ_HANDLE card_obj, gint16 *sfp_temp_data, guint16 buf_size, guint8 sfp_num)
{
    gint8 idx;
    gint32 ret;
    gint32 sfp_temp;
    OBJ_HANDLE optical_module_obj_handle;
    GSList *input_list = NULL;

    if (sfp_temp_data == NULL || sfp_num > buf_size) {
        debug_log(DLOG_ERROR, "Input param is invalid.");
        return;
    }

    for (idx = 0; idx < sfp_num; idx++) {
        ret = pcie_card_get_optical_module_handle_by_channel(card_obj, idx + 1, &optical_module_obj_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Get optical module obj failed, ret=%d.", ret);
            continue;
        }

        debug_log(DLOG_DEBUG, "sfp_temp_data[%d]=%d", idx, sfp_temp_data[idx]);
        
        if (!(sfp_temp_data[idx] == 0x7ffd || sfp_temp_data[idx] == 0x7ffe || sfp_temp_data[idx] == 0x7fff)) {
            sfp_temp = sfp_temp_data[idx];
        } else if (sfp_temp_data[idx] == 0x7ffd ||
            sfp_temp_data[idx] == 0x7ffe) { 
            sfp_temp = 0;
        } else {
            sfp_temp = HW_ACCESS_FAILED;
        }

        input_list = g_slist_append(input_list, g_variant_new_double((gdouble)sfp_temp));
        ret = dfl_call_class_method(optical_module_obj_handle, METHOD_OPT_MDL_SET_OPTICAL_MODULE_TEMP, NULL, input_list,
            NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "Set optical module %d temp failed, ret=%d.", sfp_temp, ret);
        }
    }
}


LOCAL gint32 pangea_card_get_sfp_temp(OBJ_HANDLE card_obj)
{
    gint32 ret;
    guint8 protocol;
    guint8 status;
    guint8 slot_id;
    PANGEA_CARD_SFP_TEMP_RSP temp_data = { 0 };
    OBJ_HANDLE chip_handle;

    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_PROTOCOL, &protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get protocol failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    if (protocol != PCIE_MGNT_PROTOCOL_SMBUS) {
        return RET_OK;
    }

    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    ret = pangea_card_get_chip_status(card_obj, &status);
    if (ret != RET_OK) {
        card_err_log_limit(slot_id, ret, "%s get chip status failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    if (status != PANGEA_CARD_CHIP_READY) {
        debug_log(DLOG_DEBUG, "%s get core chip status(%d), status not ready.", dfl_get_object_name(card_obj), status);
        return RET_OK;
    }

    ret = dfl_get_referenced_object(card_obj, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    ret = dfl_block_read(card_obj, PROPERTY_PANGEA_CARD_SFPTEMP, CORE_CHIP_SFP_TEMP_CMD, sizeof(temp_data), &temp_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "get share card sfp temp failed, ret=%d.", ret);
    } else {
        pangea_card_update_sfp_temp(card_obj, temp_data.data, SFP_DATA_LEN, 4); 
    }
    return ret;
}


LOCAL gint32 pangea_card_get_mcu_temp(OBJ_HANDLE card_obj)
{
    gint32 ret;
    guint16 value;
    guint8 temp;

    if (card_obj == INVALID_OBJ_HANDLE) {
        return VOS_ERR;
    }

    ret = dfl_block_read(card_obj, PROPERTY_PANGEA_CARD_MCUTEMP, MCU_IIC_READ_TEMPERATURE, sizeof(temp), &temp);
    if (ret != DFL_OK) {
        value = HW_ACCESS_FAILED;
        debug_log(DLOG_DEBUG, "get share card mcu temp failed, ret=%d.", ret);
    } else {
        value = (guint16)temp;
        debug_log(DLOG_DEBUG, "%s MCU temp = %d.", dfl_get_object_name(card_obj), value);
    }

    ret = dal_set_property_value_uint16(card_obj, PROPERTY_PANGEA_CARD_MCUTEMP, value, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s set mcu temp failed.", dfl_get_object_name(card_obj));
    }
    return ret;
}


LOCAL gint32 pangea_card_get_mcu_chip(guint8 slot_id, const gchar *prop_name, OBJ_HANDLE *chip_obj)
{
    gint32 ret;
    OBJ_HANDLE card_handle = INVALID_OBJ_HANDLE;
    
    ret = pangea_card_get_obj_by_slot(&card_handle, slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get pangea card handle failed, ret=%d", ret);
        return ret;
    }

    
    ret = dfl_get_referenced_object(card_handle, prop_name, chip_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
    }

    return ret;
}


gint32 pangea_card_get_alarm_led(OBJ_HANDLE card_obj, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 cmd = 0x1; 
    guint32 chip_addr;
    OBJ_HANDLE chip_obj = 0;
    guint8 slot;
    guint8 led_state[8] = { 0 }; 

    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get card slot id failed, ret=%d.", dfl_get_object_name(card_obj), ret);
        return ret;
    }
    ret = pangea_card_get_mcu_chip(slot, PROPERTY_MCU_CHIP, &chip_obj);
    if (ret != RET_OK) {
        return ret;
    }

    ret = dal_get_property_value_uint32(chip_obj, PROPERTY_CHIP_ADDR, &chip_addr);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get chip addr failed, ret=%d.", dfl_get_object_name(chip_obj), ret);
        return ret;
    }

    ret = share_card_read_data_from_mcu(slot, chip_obj, (MCU_IIC_CMD_E)cmd, led_state, sizeof(led_state));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get led state failed, ret=%d.", dfl_get_object_name(chip_obj), ret);
        return ret;
    }
    *output_list = g_slist_append(*output_list, g_variant_new_byte(led_state[0]));
    return ret;
}


gint32 pangea_card_set_alarm_led(OBJ_HANDLE card_obj, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    const guint8 *led_state = NULL;
    gsize len;
    guint8 cmd = 0x1; 
    guint8 slot;
    guint32 chip_addr;
    OBJ_HANDLE chip_obj = 0;
    SET_CARD_LED_REQ_S led_req = { 0 };

    ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set pcie card alarm led failed.");
        debug_log(DLOG_ERROR, "%s get card slot id failed, ret=%d.", dfl_get_object_name(card_obj), ret);
        return ret;
    }

    ret = pangea_card_get_mcu_chip(slot, PROPERTY_MCU_CHIP, &chip_obj);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set pcie card alarm led failed.");
        return ret;
    }

    ret = dal_get_property_value_uint32(chip_obj, PROPERTY_CHIP_ADDR, &chip_addr);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set pcie card alarm led failed.");
        debug_log(DLOG_ERROR, "%s get chip addr failed, ret=%d.", dfl_get_object_name(chip_obj), ret);
        return ret;
    }

    led_state =
        (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(input_list, 0), &len, sizeof(guint8));
    if (led_state == NULL || len == 0) {
        method_operation_log(caller_info, NULL, "Set pcie card alarm led failed.");
        debug_log(DLOG_ERROR, "Set pcie card alarm led failed, get error led_state, len %u.", (guint32)len);
        return ret;
    }

    ret = memcpy_s(led_req.data, len, led_state, len);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set pcie card alarm led failed.");
        debug_log(DLOG_ERROR, "Memcpy led state to dest buf failed, ret=%d.", ret);
        return ret;
    }

    led_req.chip_addr = chip_addr;
    led_req.cmd = cmd;
    
    led_req.data[3] = dal_cal_crc8(0, (guint8 *)&led_req, sizeof(SET_CARD_LED_REQ_S) - 1);
    ret = dfl_chip_blkwrite(dfl_get_object_name(chip_obj), (gint32)(gint8)cmd, sizeof(led_req.data), led_req.data);
    if (ret != DFL_OK) {
        method_operation_log(caller_info, NULL, "Set pcie card alarm led failed.");
        debug_log(DLOG_ERROR, "%s set alarm led light failed, ret=%d.", dfl_get_object_name(chip_obj), ret);
    }
    method_operation_log(caller_info, NULL, "Set pcie card(%d) alarm led state(%d) successfully.", slot, led_state[1]);
    return ret;
}


gint32 set_card_time_out_val(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 time_out_val;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "input_list is NULL");
        return DFL_ERR;
    }

    time_out_val = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_VIRTUALI2CIP2_TIMEOUTSET, time_out_val, DF_HW);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set %s timeout failed, ret %d", dfl_get_object_name(obj_handle), ret);
    }

    return ret;
}


gint32 reset_cpld_core(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 reset_val;
    guint8 idx;
    guint8 list_len;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "input_list is NULL");
        return RET_ERR;
    }

    list_len = g_slist_length(input_list);
    if (list_len < 2) { 
        return RET_ERR;
    }

    for (idx = 0; idx < list_len; ++idx) {
        reset_val = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, idx));
        ret = dal_set_property_value_byte(obj_handle, PROPERTY_CPLD_CORE_REPAIR, reset_val, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set %s reset cpld core val(%d) failed, ret %d", dfl_get_object_name(obj_handle),
                reset_val, ret);
            return ret;
        }
    }

    return RET_OK;
}


gint32 pangea_card_update_temp(OBJ_HANDLE obj_handle)
{
    gint32 ret;

    ret = pangea_card_get_mcu_temp(obj_handle);
    ret += pangea_card_get_sfp_temp(obj_handle);
    ret += pangea_card_get_1882core_temp(obj_handle);

    return ret;
}


gint32 pangea_card_update_version(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 mcu_ver[IOCARD_MCU_VER_LEN + 1] = { 0 };

    ret = dfl_block_read(object_handle, PROPERTY_PANGEA_PCIE_MCUVER, MCU_IIC_VER_OUT, IOCARD_MCU_VER_LEN,
        (gpointer)mcu_ver);
    if (ret != DFL_OK) {
        debug_log(DLOG_MASS, "%s read mcu ver failed, ret=%d.", dfl_get_object_name(object_handle), ret);
        return ret;
    } else {
        ret = dal_set_property_value_string(object_handle, PROPERTY_PANGEA_PCIE_MCUVER,
            (const gchar *)(mcu_ver + MCU_VER_HEAD_LENGTH), DF_AUTO);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s set share card mcu ver to xml failed, ret=%d.",
                dfl_get_object_name(object_handle), ret);
            return ret;
        }
    }
    return RET_OK;
}


LOCAL gint32 get_virtuali2c_time_out(OBJ_HANDLE chip_obj, guint8 *time_out)
{
    gint32 ret;
    OBJ_HANDLE lbus_handle = INVALID_OBJ_HANDLE;

    ret = dfl_get_referenced_object(chip_obj, PROPERTY_CHIP_LBUS, &lbus_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get %s LBus failed, ret %d", dfl_get_object_name(chip_obj), ret);
        return ret;
    }

    ret = dal_get_property_value_byte(lbus_handle, PROPERTY_VIRTUALI2CIP2_TIMEOUTSET, time_out);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s timeout failed, ret %d", dfl_get_object_name(lbus_handle), ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 set_virtuali2c_time_out(OBJ_HANDLE chip_obj, guint8 time_out)
{
    gint32 ret;
    OBJ_HANDLE lbus_handle = INVALID_OBJ_HANDLE;
    GSList *input_list = NULL;

    ret = dfl_get_referenced_object(chip_obj, PROPERTY_CHIP_LBUS, &lbus_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "Get %s LBus failed, ret %d", dfl_get_object_name(chip_obj), ret);
        return ret;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(time_out));
    ret = dfl_call_class_method(lbus_handle, METHOD_SET_TIME_OUT_VALUE, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set %s timeout failed, ret %d", dfl_get_object_name(lbus_handle), ret);
        return ret;
    }

    return RET_OK;
}

LOCAL gint32 get_elabel_data_from_mcu(guint8 slot, OBJ_HANDLE chip_obj, guint8* elabel_data, guint32 len)
{
    guint8 idx;
    guint8 frame_len = PANGEA_CARD_ELABLE_FRAME_LEN;
    guint16 offset;
    PANGEA_CARD_WRITE_READ_S input = { 0 };
    guint32 chip_addr;
    gint32 ret;

    ret = dal_get_property_value_uint32(chip_obj, PROPERTY_MCU_CHIP_ADDR, &chip_addr);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get chip addr failed,ret=%d.", dfl_get_object_name(chip_obj), ret);
        return RET_ERR;
    }
    for (idx = 0; idx < PANGEA_CARD_ELABLE_FRAME; idx++) {
        offset = idx * frame_len;
        input.chip_addr = chip_addr;
        input.cmd = (gint32)MCU_IIC_EKEY_ACCESS_SHORT;
        input.read_data = elabel_data + offset;
        input.read_len = frame_len; 
        input.write_data = &idx;
        input.write_len = sizeof(guint8);
        ret = share_card_write_read_from_mcu(slot, chip_obj, input);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set pangea card elable num(%d) failed.", idx);
            break;
        }
    }
    return ret;
}

gint32 pangea_card_get_elable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint32 chip_addr;
    guint8 data[2048]; 
    OBJ_HANDLE chip_handle;
    guint8 time_out_val = 0;
    guint8 slot;
    ret = dfl_get_referenced_object(object_handle, PROPERTY_PCIE_CARD_REFCHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get ref obj fail, ret=%d.", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_byte(object_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get slot failed, ret=%d.", dfl_get_object_name(object_handle), ret);
        return ret;
    }

    ret = dal_get_property_value_uint32(chip_handle, PROPERTY_MCU_CHIP_ADDR, &chip_addr);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get chip addr failed,ret=%d.", dfl_get_object_name(chip_handle), ret);
        return RET_ERR;
    }

    ret = get_virtuali2c_time_out(chip_handle, &time_out_val);
    if (ret != RET_OK) {
        return ret;
    }

    ret = set_virtuali2c_time_out(chip_handle, MAX_CPLD_I2C_TIME_OUT_VAL);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_elabel_data_from_mcu(slot, chip_handle, data, 2048); 
    if (ret != RET_OK) {
        return ret;
    }

    ret = set_virtuali2c_time_out(chip_handle, time_out_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set chip(%s) virtual i2c time out val failed.", dfl_get_object_name(chip_handle));
        return ret;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, data,
                                                                          FRU_FILE_MAX_LEN, sizeof(guint8)));
    return RET_OK;
}


gint32 method_read_data_from_mcu(OBJ_HANDLE card_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    OBJ_HANDLE chip_obj;
    guint8* buf = NULL;

    
    gint32 ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    guint8 slot_id;
    ret = dal_get_property_value_byte(card_handle, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    guint16 opcode = g_variant_get_uint16((GVariant*)g_slist_nth_data(input_list, 0));
    guint8 read_len = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (read_len < 2 || read_len > 4) { 
        debug_log(DLOG_ERROR, "%s input read len(%d) invalid.", dfl_get_object_name(card_handle), read_len);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "opcode:%d, read_len:%d.", opcode, read_len);
    do {
        buf = g_malloc0(read_len * sizeof(guint8));
        if (buf == NULL) {
            debug_log(DLOG_ERROR, "%s malloc data buf failed.", dfl_get_object_name(chip_obj));
            ret = RET_ERR;
            break;
        }

        ret = share_card_read_data_from_mcu(slot_id, chip_obj, opcode, buf, read_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s read data from mcu failed, ret=%d, opcode=%d.", dfl_get_object_name(card_handle),
                ret, opcode);
            break;
        }

        gchar *resp_str = g_base64_encode(buf, read_len);
        if (resp_str == NULL) {
            ret = RET_ERR;
            debug_log(DLOG_ERROR, "%s encode base64 string failed.", dfl_get_object_name(card_handle));
            break;
        }
        *output_list = g_slist_append(*output_list, g_variant_new_string(resp_str));
        g_free(resp_str);
    } while (0);

    if (buf != NULL) {
        g_free(buf);
    }

    return ret;
}


gint32 reset_card_chip(GSList *input_list)
{
    gint32 result;
    guint8 slot_id;
    guint8 status = 1; 

    if (input_list == NULL || g_slist_length(input_list) != 1) { 
        debug_printf("parm is not match\n");
        return MODULE_ERR;
    }

    slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    result = pangea_card_reset_mcu(slot_id, status);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "Reset card chip failed, ret=%d", result);
        return MODULE_ERR;
    }

    return MODULE_OK;
}


gint32 get_card_powergood(GSList *input_list)
{
    gint32 result;
    guint8 slot_id;
    guint8 status = 0;
    OBJ_HANDLE card_handle;

    if (input_list == NULL || g_slist_length(input_list) != 1) { 
        debug_printf("parm is not match\n");
        return MODULE_ERR;
    }

    slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    
    result = pangea_card_get_obj_by_slot(&card_handle, slot_id);
    if (result != RET_OK) {
        return result;
    }

    result = pangea_card_get_pg(card_handle, &status);
    if (result != RET_OK) {
        debug_printf("Get card powergood state failed, ret=%d", result);
    }

    debug_printf("slot(%d) power good:%d", slot_id, status);
    return MODULE_OK;
}


LOCAL gint32 pangea_card_set_card_type(guint8 slot_id, guint16 type)
{
    gint32 ret;
    OBJ_HANDLE card_conn_obj;
    OBJ_HANDLE chip_handle = 0;
    guint16 card_type = type;

    
    ret = pcie_card_get_connector_handle(slot_id, &card_conn_obj, PANGEA_CARD_CONNECTOR);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %d card's mcu connector handle failed, ret=%d", slot_id, ret);
        return ret;
    }

    ret = dal_get_refobject_handle_nth(card_conn_obj, PROPERTY_CONNECTOR_LEGACY, 0, &chip_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %d card's mcu chip handle failed, ret=%d", slot_id, ret);
        return ret;
    }

    
    ret = share_card_send_data_to_mcu(chip_handle, MCU_IIC_SYS_SYNC_CARDTYPE, (gpointer)&card_type, sizeof(card_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(chip_handle), ret);
        return ret;
    }
    return RET_OK;
}


LOCAL gint32 pangea_card_get_cardtype(guint8 slot_id, guint16 *type)
{
    gint32 ret;
    OBJ_HANDLE card_handle;
    OBJ_HANDLE chip_handle;
    guint8 card_type[4]; 
    
    ret = pangea_card_get_obj_by_slot(&card_handle, slot_id);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    
    ret = share_card_read_data_from_mcu(slot_id, chip_handle, MCU_IIC_SYS_SYNC_CARDTYPE, card_type, sizeof(card_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }
    *type = MAKE_WORD(card_type[0], card_type[1]);
    return RET_OK;
}

LOCAL gint32 pangea_card_set_hilinktype(guint8 slot_id, guint16 type)
{
    gint32 ret;
    OBJ_HANDLE card_handle;
    OBJ_HANDLE chip_handle;
    guint8 temp;
    guint16 hilink_type;
    
    ret = pangea_card_get_obj_by_slot(&card_handle, slot_id);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    temp = HIGH(type);
    hilink_type = MAKE_WORD(temp, type);
    
    ret = share_card_send_data_to_mcu(chip_handle, MCU_IIC_SYS_SYNC_LINKTYPE, (gpointer)&hilink_type,
        sizeof(hilink_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }
    return RET_OK;
}

LOCAL gint32 pangea_card_set_workmode(guint8 slot_id, QOS_INFO_S work_mode)
{
    gint32 ret;
    OBJ_HANDLE card_handle;
    OBJ_HANDLE chip_handle;

    
    ret = pangea_card_get_obj_by_slot(&card_handle, slot_id);
    if (ret != RET_OK) {
        return ret;
    }

    
    ret = dfl_get_referenced_object(card_handle, PROPERTY_MCU_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get mcu chip handle failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    
    ret = share_card_send_data_to_mcu(chip_handle, MCU_IIC_SYS_SYNC_QOSINFO, (gpointer)&work_mode, sizeof(QOS_INFO_S));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s send cmd to mcu failed, ret=%d", dfl_get_object_name(card_handle), ret);
        return ret;
    }

    return RET_OK;
}


gint32 set_card_hilink(GSList *input_list)
{
    gint32 result;
    guint8 slot_id;
    guint16 card_type = 0;
    guint16 hilink_type;
    if (input_list == NULL || g_slist_length(input_list) != 2) { 
        debug_printf("parm is not match\n");
        return MODULE_ERR;
    }
    slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    hilink_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    result = pangea_card_get_cardtype(slot_id, &card_type);
    if (result != RET_OK) {
        debug_printf("Get card type failed, ret=%d", result);
        return MODULE_ERR;
    }
    result = pangea_card_set_hilinktype(slot_id, hilink_type);
    if (result != RET_OK) {
        debug_printf("Set hilink type failed, ret=%d", result);
        return MODULE_ERR;
    }
    debug_printf("slot(%d) card type:%d, hilink_type:%d", slot_id, card_type, hilink_type);
    return MODULE_OK;
}


gint32 set_card_type(GSList *input_list)
{
    if (input_list == NULL || g_slist_length(input_list) != 3) { 
        debug_printf("parm is not match\n");
        return MODULE_ERR;
    }

    guint8 slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    guint8 type_high = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    guint8 type_low = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2)); 

    guint16 card_type = MAKE_WORD(type_low, type_high);
    gint32 result = pangea_card_set_card_type(slot_id, card_type);
    if (result != RET_OK) {
        debug_printf("Set slot(%d) card type:%d failed", slot_id, card_type);
        return MODULE_ERR;
    }
    debug_printf("Set slot(%d) card type:%d successfully", slot_id, card_type);
    return MODULE_OK;
}


gint32 set_card_workmode(GSList *input_list)
{
    gint32 result;
    guint8 slot_id;
    QOS_INFO_S work_mode;

    if (input_list == NULL || g_slist_length(input_list) != 3) { 
        debug_printf("parm is not match\n");
        return MODULE_ERR;
    }

    (void)memset_s(&work_mode, sizeof(QOS_INFO_S), 0, sizeof(QOS_INFO_S));
    slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    work_mode.mode = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1)); 
    work_mode.speed = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2)); 
    result = pangea_card_set_workmode(slot_id, work_mode);
    if (result != RET_OK) {
        debug_printf("Set hilink type failed, ret=%d", result);
        return MODULE_ERR;
    }

    return MODULE_OK;
}


LOCAL gint32 set_keyvalue_format_elabel(OBJ_HANDLE chip_obj, guint16 frame_num, const guint8 *elabel_data,
    guint32 data_len)
{
    gint32 ret;
    guint32 offset;
    guint32 len;
    LOCAL guint8 buf[PANGEA_CARD_MAX_ELABLE_LEN];
    guint8 time_out_val = 0;

    if (frame_num <= 0 || frame_num > MAX_ELABEL_FRAME_NUM || elabel_data == NULL) { 
        return RET_ERR;
    }

    
    len = (frame_num == MAX_ELABEL_FRAME_NUM) ? (PANGEA_CARD_MAX_ELABLE_LEN - SET_ELABEL_DATA_SIZE_PER_FRAME * 4) :
                                                data_len;
    offset = SET_ELABEL_DATA_SIZE_PER_FRAME * (frame_num - 1);
    ret = memcpy_s(buf + offset, PANGEA_CARD_MAX_ELABLE_LEN - offset, elabel_data, len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Memcpy elabel data failed, ret=%d", ret);
        return RET_ERR;
    }

    if (frame_num == MAX_ELABEL_FRAME_NUM) {
        ret = get_virtuali2c_time_out(chip_obj, &time_out_val);
        if (ret != RET_OK) {
            return ret;
        }

        ret = set_virtuali2c_time_out(chip_obj, MAX_CPLD_I2C_TIME_OUT_VAL);
        if (ret != RET_OK) {
            return ret;
        }

        ret = share_card_send_data_to_mcu(chip_obj, MCU_IIC_EKEY_ACCESS, buf, PANGEA_CARD_MAX_ELABLE_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set pangea card elable faild. ret = %d", ret);
            (void)set_virtuali2c_time_out(chip_obj, time_out_val);
            return ret;
        }

        ret = set_virtuali2c_time_out(chip_obj, time_out_val);
        if (ret != RET_OK) {
            return ret;
        }
    }
    return RET_OK;
}


LOCAL gint32 get_all_elabel_data(guint8 slot, OBJ_HANDLE chip_obj, guint8* elabel_data, guint32 len)
{
    gint32 ret;
    guint8 time_out_val = 0;

    ret = get_virtuali2c_time_out(chip_obj, &time_out_val);
    if (ret != RET_OK) {
        return ret;
    }

    ret = set_virtuali2c_time_out(chip_obj, MAX_CPLD_I2C_TIME_OUT_VAL);
    if (ret != RET_OK) {
        return ret;
    }

    ret = get_elabel_data_from_mcu(slot, chip_obj, elabel_data, len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get elabel data failed, ret=%d.", dfl_get_object_name(chip_obj), ret);
        return ret;
    }

    ret = set_virtuali2c_time_out(chip_obj, time_out_val);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set chip(%s) virtual i2c time out val failed.", dfl_get_object_name(chip_obj));
    }
    return ret;
}

LOCAL gint32 get_keyvalue_format_elabel(guint8 slot, OBJ_HANDLE chip_obj, guint16 frame_num,
                                        guint8* elabel_data, guint32 len)
{
    gint32 ret;
    guint16 offset;
    LOCAL guint8 data_buf[PANGEA_CARD_MAX_ELABLE_LEN];
    guint32 read_len;

    if (frame_num == 1) { 
        (void)memset_s(data_buf, PANGEA_CARD_MAX_ELABLE_LEN, 0, PANGEA_CARD_MAX_ELABLE_LEN);
        ret = get_all_elabel_data(slot, chip_obj, data_buf, PANGEA_CARD_MAX_ELABLE_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s get all elabel data failed, ret=%d", dfl_get_object_name(chip_obj), ret);
            return ret;
        }
    }

    
    read_len = (frame_num == 5) ? (PANGEA_CARD_MAX_ELABLE_LEN - GET_ELABEL_DATA_SIZE_PER_FRAME * 4) : len;
    offset = GET_ELABEL_DATA_SIZE_PER_FRAME * (frame_num - 1);
    ret = memcpy_s(elabel_data, read_len, data_buf + offset, read_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Memcpy elabel data failed, ret=%d", ret);
    }
    return ret;
}


gint32 ipmi_cmd_set_pangea_card_elabel(const void *msg_data, gpointer user_data)
{
    guint8 slot_id;
    guint16 frame_num;
    gint32 ret;
    OBJ_HANDLE chip_obj = INVALID_OBJ_HANDLE;
    const guint8 *src_data = NULL;
    const PANGEA_CARD_IPMIMSG_ELABELHEAD *msg_head = NULL;
    guint8 ipmi_msg_head_size;

    
    if (msg_data == NULL) { 
        return VOS_ERR;
    }
    src_data = get_ipmi_src_data(msg_data);
    if (src_data == NULL) { 
        ipmi_operation_log(msg_data, "Set pangea card elabel failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    msg_head = (const PANGEA_CARD_IPMIMSG_ELABELHEAD *)src_data;
    slot_id = msg_head->slot_id + 1; 
    frame_num = msg_head->cur_num;

    if (frame_num <= 0 || frame_num > MAX_ELABEL_FRAME_NUM) {
        ipmi_operation_log(msg_data, "Set pangea card(%d) elabel failed.", slot_id);
        return ipmi_send_simple_response(msg_data, USER_PARAMETER_OUT_OF_RANGE);
    }

    ret = pangea_card_get_mcu_chip(slot_id, PROPERTY_MCU_CHIP, &chip_obj);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set pangea card(%d) elabel failed.", slot_id);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ipmi_msg_head_size = (guint8)sizeof(PANGEA_CARD_IPMIMSG_ELABELHEAD);
    ret =
        set_keyvalue_format_elabel(chip_obj, frame_num, &src_data[ipmi_msg_head_size], SET_ELABEL_DATA_SIZE_PER_FRAME);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set pangea card(%d) frame(%d) elabel failed.", slot_id, frame_num);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    ipmi_operation_log(msg_data, "Set pangea card(%d) frame(%d) elabel successfully.", slot_id, frame_num);
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


gint32 ipmi_cmd_get_pangea_card_elabel(const void *msg_data, gpointer user_data)
{
    gint32 ret;
    guint8 slot_id;
    guint16 frame_num;
    OBJ_HANDLE chip_obj = 0;
    const guint8 *src_data = NULL;
    const PANGEA_CARD_IPMIMSG_ELABELHEAD* msg_head = NULL;
    guint8 one_frame_data[GET_ELABEL_DATA_SIZE_PER_FRAME + 1] = { 0 }; 

    
    if (msg_data == NULL) { 
        return RET_ERR;
    }
    src_data = get_ipmi_src_data(msg_data);
    if (src_data == NULL) { 
        ipmi_operation_log(msg_data, "Get pangea card elabel failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    msg_head = (const PANGEA_CARD_IPMIMSG_ELABELHEAD *)src_data;
    slot_id = msg_head->slot_id + 1; 
    frame_num = msg_head->cur_num;

    if (frame_num <= 0 || frame_num > MAX_ELABEL_FRAME_NUM) {
        ipmi_operation_log(msg_data, "Get pangea card(%d) elabel failed.", slot_id);
        return ipmi_send_simple_response(msg_data, USER_PARAMETER_OUT_OF_RANGE);
    }
    ret = pangea_card_get_mcu_chip(slot_id, PROPERTY_PCIE_CARD_REFCHIP, &chip_obj);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Get pangea card(%d) elabel failed.", slot_id);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    
    ret = get_keyvalue_format_elabel(slot_id, chip_obj, frame_num, &one_frame_data[1], GET_ELABEL_DATA_SIZE_PER_FRAME);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Get pangea card(%d) frame(%d) elabel failed.", slot_id, frame_num);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    one_frame_data[0] = COMP_CODE_SUCCESS;
    ipmi_operation_log(msg_data, "Get pangea card(%d) frame(%d) elabel successfully.", slot_id, frame_num);
    return ipmi_send_response(msg_data, sizeof(one_frame_data), one_frame_data);
}


gint32 ipmi_cmd_get_pangea_card_type(gconstpointer msg_data, gpointer user_data)
{
    
    if (msg_data == NULL) { 
        return RET_ERR;
    }

    const guint8 *src_data = NULL;
    const CARD_TYPE_IPMIMSG_INFO* msg_head = NULL;

    src_data = get_ipmi_src_data(msg_data);
    msg_head = (const CARD_TYPE_IPMIMSG_INFO*)src_data;
    guint8 slot_id = msg_head->slot_id + 1; 
    OBJ_HANDLE chip_obj;

    gint32 ret = pangea_card_get_mcu_chip(slot_id, PROPERTY_MCU_CHIP, &chip_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get slot%d pangea card mcu obj failed, ret=%d.", slot_id, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    CARD_TYPE_IPMIMSG_RSP rsp = {0}; 
    ret = share_card_read_data_from_mcu(slot_id, chip_obj, MCU_IIC_SYS_SYNC_CARDTYPE,
        rsp.card_type, sizeof(rsp.card_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get slot%d pangea card type failed, ret=%d.", slot_id, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    rsp.completion_code = COMP_CODE_SUCCESS;
    return ipmi_send_response(msg_data, sizeof(CARD_TYPE_IPMIMSG_RSP) - 2, (guint8*)&rsp); 
}


LOCAL gint32 change_card_load_state(guint8 slot_id)
{
    OBJ_HANDLE action_obj = INVALID_OBJ_HANDLE;
    PANGEA_CARD_STATE_INFO* card_info = NULL;

    for (GSList* node = g_pangea_card_action_list; node; node = node->next) {
        gint32 ret = dfl_get_binded_data((OBJ_HANDLE)node->data, (gpointer*)&card_info);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s get binded data failed, ret=%d.", dfl_get_object_name(action_obj), ret);
            continue;
        }

        if (card_info->slot_id == slot_id) {
            action_obj = (OBJ_HANDLE)node->data;
            break;
        }
    }

    if (action_obj == INVALID_OBJ_HANDLE) {
        debug_log(DLOG_ERROR, "Get slot%d card action obj failed.", slot_id);
        return RET_ERR;
    }

    gint32 board_id;
    gint32 ret = dal_get_property_value_int32(card_info->card_conn_obj, PROPERTY_CONNECTOR_ID, &board_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get board id failed, ret=%d.", dfl_get_object_name(card_info->card_conn_obj), ret);
        return ret;
    }
    
    (void)dfl_notify_presence_v2(card_info->card_conn_obj, board_id, 0, PANGEA_CARD_ABSENT);
    ret = dal_set_property_value_byte(action_obj, PROPERTY_CARD_ACTION_DESTINATION, 0, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set card action %s failed, ret=%d.", PROPERTY_CARD_ACTION_DESTINATION, ret);
    }
    return ret;
}


gint32 ipmi_cmd_set_pangea_card_type(gconstpointer msg_data, gpointer user_data)
{
    
    if (msg_data == NULL) { 
        return RET_ERR;
    }

    const guint8 *src_data = NULL;
    src_data = get_ipmi_src_data(msg_data);
    const SET_CARD_TYPE_IPMIMSG_S* msg_head = (const SET_CARD_TYPE_IPMIMSG_S*)src_data;
    guint8 slot_id = msg_head->slot_id + 1; 
    guint16 card_type = MAKE_WORD(msg_head->fru_type_id, msg_head->board_id);

    gint32 ret = pangea_card_set_card_type(slot_id, card_type);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set slot%d card type failed.", slot_id);
        debug_log(DLOG_ERROR, "Set slot%d pangea card type failed, ret=%d.", slot_id, ret);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ret = change_card_load_state(slot_id);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set slot%d card type failed.", slot_id);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ipmi_operation_log(msg_data, "Set slot%d card typesuccessfully.", slot_id);
    return ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
}


gint32 pangea_card_init()
{
    return RET_OK;
}


gint32 pangea_card_start()
{
    gint32 ret;
    ret = vos_task_create(NULL, "load xml", (TASK_ENTRY)pangea_card_monitor_task, (gpointer)NULL, DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Create load pangea card xml task failed, ret=%d.", ret);
    }
    return ret;
}