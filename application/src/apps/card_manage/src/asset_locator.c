
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "get_version.h"


#include "modbus_device.h"
#include "asset_locator.h"


#define TASK_NAME_AL_BRD_DETECT "al_brd_detect"         
#define TASK_NAME_AL_BRD_STAT_SYNC "al_brd_status_sync" 


#define MCU_PWR_ON 0                  
#define MCU_PWR_OFF 1                 
#define MB_QUERY_RETRY_TIMES 10       
#define MCU_BOOT_MODE_INVALID 0       
#define MCU_BOOT_MODE_UPGRAD 1        
#define MCU_BOOT_MODE_APP 2           
#define MCU_BOOT_UP_TIME 3000         
#define ASSET_BRD_PRES_CHK_TIME 3000  
#define ASSET_BRD_STAT_SYNC_TIME 3000 
#define PING_DEFAULT_SLAVE_TIMES 5    
#define SLAVE_ADDR_REGISTERS 0x0000   
#define MB_ADDR_ALLOC_NULL 0          
#define WAIT_FOR_XML_LOAD_UNLOAD 1000 
#define MAX_WAIT_FOR_XML_COUNT 36     
#define MB_BRD_ID_RETRY_TIMES 10      
#define MAX_QUERY_BOOT_MODE_COUNT 20  


#define FC_FILE_EXCHANGE 0x41        
#define FC_FILE_UPLOAD_START 0x01    
#define FC_FILE_UPLOAD 0x02          
#define FC_FILE_UPLOAD_COMPLETE 0x03 

#define FILE_EXC_FRAME_SIZE 224 
#define WAIT_FRAME_DIVSOR 50    

#define FULL_FILE_NAME_LEN 128 
#define FIRMWARE_PATH "/opt/pme/conf/asset_locator"
#define FIRMWARE_FILE_MAX_SIZE (200 * 1024) 
#define UPGRADE_RETRY_TIMES 3               
#define UPGRADE_RECIVE_OK 1                 
#define UPGRADE_RECIVE_FAIL 0               


#define FC_MCU_CTRL 0x42              
#define FC_MCU_CTRL_NEXT_POWER 0x01   
#define FC_MCU_CTRL_BOOT_MODE 0x02    
#define FC_MCU_CTRL_RESTORE_ADDR 0x03 
#define FC_MCU_CTRL_RESET_MCU 0x04    

#define FILE_CONTENT_BUF_LEN 4096
#define FILE_NAME_LEN 256


#define MB_CHANNEL_NOT_AVAILABLE 0                  
#define MB_CHANNEL_COMMON_READ_ALLOWED (1 << 0)     
#define MB_CHANNEL_NOTIFY_TAG_POLL_ALLOWED (1 << 1) 
#define MB_CHANNEL_UNIT_OPER_ALLOWED (1 << 2)       
#define MB_CHANNEL_EXTERNAL_MB_CMD_ALLOWED (1 << 3) 

#define MB_POLL_TAG_TIME 20 


#define INPUT_REG_HW_ID 0x0000       
#define INPUT_REG_BOOT_MODE 0x002D   
#define HOLD_REG_BOOT_VER_POS 0x0001 
#define HOLD_REG_BOOT_VER_SIZE 2 
#define HOLD_REG_FIRM_VER_POS 0x0003 
#define HOLD_REG_FIRM_VER_SIZE 2 
#define INPUT_REG_MCU_STATUS_START 0x000B 
#define MCU_STATUS_REG_NUM 2              
#define MCU_STATUS_REG_START_POS 0
#define MCU_STATUS_REG_END_POS 1


LOCAL gint32 g_mb_addr_tbl[MAX_SLAVE_NUM + 1] = {0}; 
LOCAL guint8 g_need_upgrade[MAX_SLAVE_NUM + 1] = {0}; 
LOCAL guint8 g_need_active[MAX_SLAVE_NUM + 1] = {0};  


#define UPGRADE_AL_BRD_ALL 0xff
LOCAL guint8 g_debug_upgrade_al_brd_slot = UPGRADE_AL_BRD_ALL;
LOCAL gchar g_debug_upgrade_file_path[FULL_FILE_NAME_LEN] = {0};

#define MCU_START_STATUS_MASK 0x0001
#define MCU_START_OK 0
#define MCU_START_FAIL 1


typedef enum {
    ALLOC_ADDR_OK = 0x00,   
    ALLOC_ADDR_SYS_ERROR,   
    ALLOC_ADDR_PROTO_ERROR, 
    ALLOC_ADDR_DEV_ERROR,   
} addr_alloc_stat_t;

typedef enum {
    FILE_TYPE = 0x01, 
    LOG_TYPE,         
} file_t;


LOCAL gint32 mb_restore_slave(OBJ_HANDLE obj_mb_addr_alloc_mgmt);
LOCAL void __restore_addr_of_mb_slave(OBJ_HANDLE obj_mb_addr_alloc_mgmt);


LOCAL gint32 __modbus_rtu_conn_ctx_init_by_ref_handle(OBJ_HANDLE ref_handle, modbus_t *ctx)
{
    gint32 ret;
    OBJ_HANDLE obj_slave = OBJ_HANDLE_COMMON;
    OBJ_HANDLE obj_modbus = OBJ_HANDLE_COMMON;
    guint32 slave_addr = 0;
    guchar uart_ch = 0;

    if ((ref_handle == OBJ_HANDLE_COMMON) || (ctx == NULL)) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return RET_ERR;
    }

    

    (void)dal_get_specific_object_backward_position(ref_handle, CLASS_MODBUS_SLAVE, &obj_slave);

    
    (void)dal_get_property_value_uint32(obj_slave, PROPERTY_MB_SLAVE_ADDR, &slave_addr);

    
    ret = dfl_get_referenced_object(obj_slave, PROPERTY_MB_SLAVE_LBUS, &obj_modbus);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:Get slave(addr=0x%02X) LBus object failed.", __FUNCTION__, slave_addr);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart_ch);

    ret = modbus_rtu_conn_ctx_init(uart_ch, slave_addr, ctx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:init connection context for [%s] failed.", __FUNCTION__,
            dfl_get_object_name(ref_handle));
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 modbus_get_mcu_start_status(modbus_t *ctx, OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    guint16     mcu_status_temp[MCU_STATUS_REG_NUM] = {0};
    guint32 mcu_status = 0;
    guint8 asset_slot_id = 0;

    if ((ctx == NULL) || (obj_asset_brd == 0)) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return RET_ERR;
    }

    ret = modbus_read_input_registers(ctx, INPUT_REG_MCU_STATUS_START, MCU_STATUS_REG_NUM, mcu_status_temp);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_DEBUG, "Get mcu start status modbus reg failed");
        return MODBUS_ERR;
    }

    mcu_status = ((mcu_status_temp[1] << 16u) | mcu_status_temp[0]);
    (void)dal_get_property_value_byte(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_SLOT, &asset_slot_id);

    if ((mcu_status & MCU_START_STATUS_MASK) == MCU_START_FAIL) {
        maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_MCU_BOOT_FAIL, "The MCU of asset locator board %d failed to start.",
            asset_slot_id);
    } else {
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "The MCU of asset locator board %d start success.", asset_slot_id);
    }

    return RET_OK;
}


LOCAL gint32 get_firmware_full_name(const gchar *file_name, gchar *full_file_name, guint32 full_file_name_buf_len)
{
    errno_t nRet = EOK;

    if ((file_name == NULL) || (full_file_name == NULL) || (full_file_name_buf_len == 0)) {
        debug_log(DLOG_ERROR, "%s: Invalid input", __FUNCTION__);
        return RET_ERR;
    }

    
    if (g_debug_upgrade_al_brd_slot != UPGRADE_AL_BRD_ALL) {
        nRet = memmove_s(full_file_name, full_file_name_buf_len, g_debug_upgrade_file_path,
            strlen(g_debug_upgrade_file_path));
        if (nRet != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s failed.ret=%d", __FUNCTION__, nRet);
            return RET_ERR;
        }

        return RET_OK;
    }

    

    
    nRet = snprintf_s(full_file_name, full_file_name_buf_len, full_file_name_buf_len - 1, "%s/%s", FIRMWARE_PATH,
        file_name);
    if (nRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed.ret=%d", __FUNCTION__, nRet);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 load_file_to_buffer(gchar *file_name, guint8 *buffer, guint32 buffer_size)
{
    gint32 ret = RET_OK;
    guint32 file_size = 0;
    FILE *fd = NULL;

    if ((file_name == NULL) || (buffer == NULL) || (buffer_size == 0)) {
        debug_log(DLOG_ERROR, "%s: Input invalid.", __FUNCTION__);
        return RET_ERR;
    }

    // 校验文件长度
    file_size = vos_get_file_length(file_name);
    if (file_size > buffer_size) {
        debug_log(DLOG_ERROR, "%s: file %s size=%d is larger chan buffer size=%d.", __FUNCTION__, file_name,
            file_size, buffer_size);
        return RET_ERR;
    }
    fd = dal_fopen_check_realpath(file_name, "rb", file_name);
    if (fd == NULL) {
        debug_log(DLOG_ERROR, "%s: Open file %s failed.", __FUNCTION__, file_name);
        return RET_ERR;
    }

    ret = fseek(fd, 0, SEEK_SET);
    if (ret != 0) {
        (void)fclose(fd);
        debug_log(DLOG_ERROR, "%s: fseek file %s failed.", __FUNCTION__, file_name);
        return RET_ERR;
    }

    ret = fread(buffer, sizeof(guint8), file_size, fd);
    if (ret != file_size) {
        (void)fclose(fd);
        debug_log(DLOG_ERROR, "%s: read file %s to buffer failed. ret=%d", __FUNCTION__, file_name, ret);
        return RET_ERR;
    }

    (void)fclose(fd);

    return RET_OK;
}


LOCAL gint32 modbus_set_new_addr(modbus_t *ctx, guint8 addr_new)
{
    gint32 rc = 0;
    if (ctx == NULL || addr_new > MB_ADDR_END) {
        debug_log(DLOG_ERROR, "%s: Illegal modbus slave addr 0x%02X", __FUNCTION__, addr_new);
        return MODBUS_ERR;
    }

    rc = modbus_write_register(ctx, SLAVE_ADDR_REGISTERS, addr_new, MB_RECV_MAX_TMOUT);
    if (rc == MODBUS_ERR) {
        debug_log(DLOG_DEBUG, "%s: Failed to set slave addr: 0x%02X", __FUNCTION__, addr_new);
        return rc;
    }

    return rc;
}


LOCAL gint32 modbus_next_power_on(modbus_t *ctx)
{
#define MB_NEXT_PWR_CTRL_CMD_DATA_LEN 1
#define MB_NEXT_PWR_CTRL_CMD_LEN 4
    gint32 rc = 0;
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    mb_req.ctx = ctx;
    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_MCU_CTRL;
    mb_req.data[2] = MB_NEXT_PWR_CTRL_CMD_DATA_LEN; 
    mb_req.data[3] = FC_MCU_CTRL_NEXT_POWER;
    mb_req.req_len = MB_NEXT_PWR_CTRL_CMD_LEN;

    mb_req.timeout_ms = MB_RECV_MAX_TMOUT;

    rc = modbus_send_command(&mb_req, &mb_resp);

    return rc;
}


LOCAL gint32 modbus_set_boot_mode(modbus_t *ctx, guint8 boot_mode)
{
#define MB_SET_BOOT_MODE_CMD_DATA_LEN 2
#define MB_SET_BOOT_MODE_CMD_LEN 5
    gint32 rc = 0;
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    mb_req.ctx = ctx;
    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_MCU_CTRL;
    mb_req.data[2] = MB_SET_BOOT_MODE_CMD_DATA_LEN; 
    mb_req.data[3] = FC_MCU_CTRL_BOOT_MODE;
    mb_req.data[4] = boot_mode;
    mb_req.req_len = MB_SET_BOOT_MODE_CMD_LEN;

    mb_req.timeout_ms = MB_RECV_MAX_TMOUT;

    rc = modbus_send_command(&mb_req, &mb_resp);

    return rc;
}


LOCAL gint32 modbus_get_boot_mode(modbus_t *ctx, guint8 *boot_mode)
{
    gint32 ret = RET_OK;
    guint16 reg_val = 0;

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    // 从只读的输入寄存器指定偏移读取单板启动模式
    
    ret = modbus_read_input_registers(ctx, INPUT_REG_BOOT_MODE, 1, &reg_val);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_DEBUG, "Get boot mode from slave(0x%02X) failed", ctx->slave);
        return MODBUS_ERR;
    }
    

    *boot_mode = reg_val & 0x00FF;
    if ((*boot_mode != MCU_BOOT_MODE_UPGRAD) && (*boot_mode != MCU_BOOT_MODE_APP)) {
        debug_log(DLOG_ERROR, "Boot mode value=%d from slave(0x%02X) is invalid.", *boot_mode, ctx->slave);
    }

    return RET_OK;
}


LOCAL gint32 modbus_get_board_id(modbus_t *ctx, guint32 *board_id)
{
    gint32 ret = RET_OK;
    guint16 reg_val = 0;

    // 从只读的输入寄存器 INPUT_REG_HW_ID 中读取单板的硬件Id
    ret = modbus_read_input_registers(ctx, INPUT_REG_HW_ID, 1, &reg_val);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_ERROR, "Get hardware ID from slave(0x%02X) failed", ctx->slave);
        return MODBUS_ERR;
    }

    *board_id = reg_val & 0x00FF;

    return RET_OK;
}


LOCAL gint32 modbus_update_pcb_id(modbus_t *ctx, OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    guint16 reg_val = 0;
    guint8 pcb_id = 0;

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    // 从只读的输入寄存器 INPUT_REG_HW_ID 中读取单板的硬件Id
    ret = modbus_read_input_registers(ctx, INPUT_REG_HW_ID, 1, &reg_val);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_DEBUG, "Get hardware ID from slave(0x%02X) failed", ctx->slave);
        return MODBUS_ERR;
    }

    // 解析出单板的PcbId: 高字节的低4位
    pcb_id = (reg_val >> 8) & 0x0F;
    ret = dal_set_property_value_byte(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_PCB_ID, pcb_id, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Save pcb id 0x%02X to %s failed. ret=%d", pcb_id, dfl_get_object_name(obj_asset_brd),
            ret);
        return MODBUS_ERR;
    }

    // 根据单板ID解析PCB版本
    ret = get_pcb_version(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_PCB_ID, PROPERTY_ASSET_LOCATE_BRD_PCB_VER);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Save pcb ver for %s failed. ret=%d", dfl_get_object_name(obj_asset_brd), ret);
        return MODBUS_ERR;
    }

    return RET_OK;
}


LOCAL gint32 modbus_update_boot_version(modbus_t *ctx, OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    guint16 ver_reg[HOLD_REG_BOOT_VER_SIZE] = {0};
    
    gchar   version[MAX_MCU_FW_VER_LEN] = {0};
    
    guint8 ver_major = 0;
    guint8 ver_minor = 0;
    guint8 ver_build = 0;

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    // 从保持寄存器获取uboot固件版本
    ret = modbus_read_registers(ctx, HOLD_REG_BOOT_VER_POS, HOLD_REG_BOOT_VER_SIZE, ver_reg);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_ERROR, "Get bootloader ver from slave(0x%02X) failed", ctx->slave);
        return ret;
    }

    // bootloader版本存放在两个16 bit寄存器中
    ver_major = ver_reg[0];
    ver_minor = (ver_reg[1] >> 8) & 0xff;
    ver_build = ver_reg[1] & 0xff;
    (void)snprintf_s(version, sizeof(version), sizeof(version) - 1, "%u.%u.%u", ver_major, ver_minor, ver_build);

    if (strcmp(version, "") != 0) {
        ret = dal_set_property_value_string(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_BOOT_VER, version, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set bootloader version for slave(0x%02X) failed. ret=%d", ctx->slave, ret);
            return MODBUS_ERR;
        }
        debug_log(DLOG_INFO, "Slave(0x%02X) Bootloader Version: %s", ctx->slave, version);
    }

    return RET_OK;
}


LOCAL gint32 modbus_update_mcu_version(modbus_t *ctx, OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    guint16 ver_reg[HOLD_REG_BOOT_VER_SIZE] = {0};
    
    gchar   version[MAX_MCU_FW_VER_LEN] = {0};
    guint8 ver_major = 0;
    guint8 ver_minor = 0;
    guint8 ver_build = 0;
    gchar               mcu_expect_ver[MAX_MCU_FW_VER_LEN] = {0};

    if ((ctx == NULL) || (obj_asset_brd == 0)) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_string(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_EXPECT_VER, mcu_expect_ver,
        sizeof(mcu_expect_ver));
    (void)dal_get_property_value_string(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, version, sizeof(version));

    
    if (g_strcmp0(mcu_expect_ver, version) == 0) {
        return RET_OK;
    }
    

    // 从保持寄存器获取MCU固件版本
    ret = modbus_read_registers(ctx, HOLD_REG_FIRM_VER_POS, HOLD_REG_FIRM_VER_SIZE, ver_reg);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_ERROR, "Read MCU ver from slave(0x%02X) failed.", ctx->slave);
        return MODBUS_ERR;
    }

    // MCU固件版本存放在两个16 bit寄存器中
    ver_major = ver_reg[0];
    ver_minor = (ver_reg[1] >> 8) & 0xff;
    ver_build = ver_reg[1] & 0xff;
    (void)snprintf_s(version, sizeof(version), sizeof(version) - 1, "%u.%u.%u", ver_major, ver_minor, ver_build);

    if (strcmp(version, "") != 0) {
        ret = dal_set_property_value_string(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, version, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Set MCU version for slave(0x%02X) failed. ret=%d", ctx->slave, ret);
            return MODBUS_ERR;
        }
        debug_log(DLOG_INFO, "Slave(0x%02X) MCU Version: %s", ctx->slave, version);
    }

    return RET_OK;
}


LOCAL void modbus_restore_addr(modbus_t *ctx)
{
#define MB_RESTORE_ADDR_CMD_DATA_LEN 1
#define MB_RESTORE_ADDR_CMD_LEN 4
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };
    guchar i;

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return;
    }

    mb_req.ctx = ctx;
    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_MCU_CTRL;
    mb_req.data[2] = MB_RESTORE_ADDR_CMD_DATA_LEN; 
    mb_req.data[3] = FC_MCU_CTRL_RESTORE_ADDR;
    mb_req.req_len = MB_RESTORE_ADDR_CMD_LEN;

    mb_req.ignore_rsp_flag = ENABLED; 

    
    for (i = 0; i < MB_RETRY_TIMES; i++) {
        (void)modbus_send_command(&mb_req, &mb_resp);

        vos_task_delay(MB_RECV_MAX_TMOUT);
    }

    return;
}


LOCAL gint32 modbus_reset_mcu(modbus_t *ctx)
{
#define MB_RESET_MCU_CMD_DATA_LEN 1
#define MB_RESET_MCU_CMD_LEN 4
    gint32 rc = 0;
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    mb_req.ctx = ctx;
    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_MCU_CTRL;
    mb_req.data[2] = MB_RESET_MCU_CMD_DATA_LEN; 
    mb_req.data[3] = FC_MCU_CTRL_RESET_MCU;
    mb_req.req_len = MB_RESET_MCU_CMD_LEN;

    mb_req.ignore_rsp_flag = ENABLED;

    rc = modbus_send_command(&mb_req, &mb_resp);

    return rc;
}


LOCAL gint32 modbus_upload_start(modbus_t *ctx, guint8 type, guint32 length, guint8 frame_size)
{
#define MB_UPLOAD_START_CMD_DATA_LEN 7
#define MB_UPLOAD_START_CMD_LEN 10
    gint32 rc = 0;
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    mb_req.ctx = ctx;
    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_FILE_EXCHANGE;
    mb_req.data[2] = MB_UPLOAD_START_CMD_DATA_LEN; 
    mb_req.data[3] = FC_FILE_UPLOAD_START;
    mb_req.data[4] = type;
    mb_req.data[5] = (length >> 24) & 0xFF;
    mb_req.data[6] = (length >> 16) & 0xFF;
    mb_req.data[7] = (length >> 8) & 0xFF;
    mb_req.data[8] = length & 0xFF;
    mb_req.data[9] = frame_size;
    mb_req.req_len = MB_UPLOAD_START_CMD_LEN;

    rc = modbus_send_command(&mb_req, &mb_resp);

    return rc;
}


LOCAL gint32 modbus_broadcast_upload(modbus_t *ctx, guint8 type, guint16 frame_idx, guint8 length, guint8 *data)
{
#define MB_UPLOAD_CMD_META_LEN 5
#define MB_UPLOAD_CMD_LEN 8
    gint32 rc = 0;
    guint8 i = 0;
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    mb_req.ctx = ctx;
    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_FILE_EXCHANGE;
    mb_req.data[2] = MB_UPLOAD_CMD_META_LEN + length; 
    mb_req.data[3] = FC_FILE_UPLOAD;
    mb_req.data[4] = type;
    mb_req.data[5] = (frame_idx >> 8) & 0xFF;
    mb_req.data[6] = frame_idx & 0xFF;
    mb_req.data[7] = length;
    mb_req.req_len = MB_UPLOAD_CMD_LEN;

    
    for (i = 0; i < length; i++) {
        mb_req.data[mb_req.req_len++] = data[i];
    }

    mb_req.ignore_rsp_flag = ENABLED;

    rc = modbus_send_command(&mb_req, &mb_resp);

    return rc;
}


LOCAL gint32 modbus_upload_complete(modbus_t *ctx, guint8 type, guint16 expect_crc, guint8 *status)
{
#define MB_UPLOAD_END_CMD_DATA_LEN 4
#define MB_UPLOAD_END_CMD_LEN 7
    gint32 rc = 0;
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_resp = { { 0 } };

    if ((ctx == NULL) || (status == NULL)) {
        debug_log(DLOG_ERROR, "%s: input para is invalid!", __FUNCTION__);
        return MODBUS_ERR;
    }

    mb_req.ctx = ctx;

    mb_req.data[0] = (guint8)(ctx->slave);
    mb_req.data[1] = FC_FILE_EXCHANGE;
    mb_req.data[2] = MB_UPLOAD_END_CMD_DATA_LEN; 
    mb_req.data[3] = FC_FILE_UPLOAD_COMPLETE;
    mb_req.data[4] = type;
    mb_req.data[5] = expect_crc & 0xFF;        
    mb_req.data[6] = (expect_crc >> 8) & 0xFF; 
    mb_req.req_len = MB_UPLOAD_END_CMD_LEN;

    rc = modbus_send_command(&mb_req, &mb_resp);
    if (rc == MODBUS_ERR) {
        *status = UPGRADE_RECIVE_FAIL;
        debug_log(DLOG_ERROR, "%s: send modbus command failed, ret is %d", __FUNCTION__, rc);
        return rc;
    }
    *status = UPGRADE_RECIVE_OK;

    return rc;
}


LOCAL gint32 update_al_brd_info(OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    modbus_t ctx = { 0 };
    const gchar *al_brd_name = dfl_get_object_name(obj_asset_brd);

    ret = __modbus_rtu_conn_ctx_init_by_ref_handle(obj_asset_brd, &ctx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s failed: can't init modbus connection context for [%s]", __FUNCTION__,
            dfl_get_object_name(obj_asset_brd));
        return ret;
    }

    ret = modbus_update_pcb_id(&ctx, obj_asset_brd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get pcb id from %s failed.", __FUNCTION__, al_brd_name);
    }

    ret += modbus_update_boot_version(&ctx, obj_asset_brd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get boot version from %s failed.", __FUNCTION__, al_brd_name);
    }

    ret += modbus_update_mcu_version(&ctx, obj_asset_brd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get mcu version from %s failed.", __FUNCTION__, al_brd_name);
    }

    // 更新mcu启动状态
    ret += modbus_get_mcu_start_status(&ctx, obj_asset_brd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get mcu start status from %s failed.", __FUNCTION__, al_brd_name);
    }

    return ret;
}


LOCAL gint32 modbus_refresh_mcu_status(modbus_t *ctx, OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    guint16     mcu_status_temp[MCU_STATUS_REG_NUM] = {0};
    guint32 mcu_status = 0;

    if (ctx == NULL) {
        debug_log(DLOG_ERROR, "%s: ctx is NULL!", __FUNCTION__);
        return MODBUS_ERR;
    }

    ret = modbus_read_input_registers(ctx, INPUT_REG_MCU_STATUS_START, MCU_STATUS_REG_NUM, mcu_status_temp);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_DEBUG, "Read CLRC663 Version from slave(0x%02X) failed.", ctx->slave);
        return MODBUS_ERR;
    }

    mcu_status = ((mcu_status_temp[1] << 16u) | mcu_status_temp[0]);

    
    ret = dal_set_property_value_uint32(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_WARNING_STATUS, mcu_status,
        DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Set MCU Status to %s failed. ret=%d", dfl_get_object_name(obj_asset_brd), ret);
        return MODBUS_ERR;
    }

    return RET_OK;
}


LOCAL gint32 sync_asset_brd_slave_addr(OBJ_HANDLE obj_asset_brd)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_mb_slave = OBJ_HANDLE_COMMON;
    guint8 slave_id = 0;
    gint32 slave_addr = 0;

    
    ret = dal_get_specific_object_backward_position(obj_asset_brd, CLASS_MODBUS_SLAVE, &obj_mb_slave);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Get slave for %s failed. ret=%d", __FUNCTION__, dfl_get_object_name(obj_asset_brd),
            ret);
        return RET_OK;
    }

    // SlaveId与单板槽位号挂钩
    (void)dal_get_property_value_byte(obj_mb_slave, PROPERTY_MB_SLAVE_ID, &slave_id);
    (void)dal_get_property_value_int32(obj_mb_slave, PROPERTY_MB_SLAVE_ADDR, &slave_addr);

    
    if (slave_id <= MAX_SLAVE_NUM && (g_mb_addr_tbl[slave_id] != 0) && (g_mb_addr_tbl[slave_id] != slave_addr)) {
        slave_addr = g_mb_addr_tbl[slave_id];

        
        ret = dal_set_property_value_uint32(obj_mb_slave, PROPERTY_MB_SLAVE_ADDR, (guint32)slave_addr, DF_NONE);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Set allocated addr(0x%02X) to slave%d failed. ret=%d", __FUNCTION__, slave_addr,
                slave_id, ret);
            return RET_ERR;
        }

        debug_log(DLOG_DEBUG, "%s: Set allocated addr=0x%02X to slave%d", __FUNCTION__, slave_addr, slave_id);
    }

    return RET_OK;
}


LOCAL gint32 update_asset_locator_info(OBJ_HANDLE obj_modbus)
{
    gint32 ret = RET_OK;
    GSList *asset_brd_list = NULL;
    GSList *asset_brd_node = NULL;
    OBJ_HANDLE obj_asset_brd = OBJ_HANDLE_COMMON;
    OBJ_HANDLE obj_asset_mgmt = OBJ_HANDLE_COMMON;
    guint8 dev_num = 0;
    guint8 u_count = 0;
    guint8 u_count_total = 0;
    GSList *input_list = NULL;

    ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &asset_brd_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get AssetLocateBoard failed. ret=%d", __FUNCTION__, ret);
        goto update_u_count;
    }

    
    for (asset_brd_node = asset_brd_list; asset_brd_node != NULL; asset_brd_node = asset_brd_node->next) {
        obj_asset_brd = (OBJ_HANDLE)asset_brd_node->data;

        
        (void)dal_get_property_value_byte(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_SLOT, &dev_num);
        (void)dal_get_property_value_byte(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_U_COUNT, &u_count);

        u_count_total += u_count;
        debug_log(DLOG_INFO, "%s: AssetLocateBoard%d is a %dU device", __FUNCTION__, dev_num, u_count);

        
        ret = sync_asset_brd_slave_addr(obj_asset_brd);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:sync addr for asset locate %d board failed!", __FUNCTION__, dev_num);
        }

        // 更新单板ID、版本等信息到属性
        ret = update_al_brd_info(obj_asset_brd);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Update brd info from %s failed.", __FUNCTION__,
                dfl_get_object_name(obj_asset_brd));
            continue;
        }
    }

    g_slist_free(asset_brd_list);

update_u_count:
    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_asset_mgmt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get RackAssetMgmt handle failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(0));
    ret = dfl_call_class_method(obj_asset_mgmt, METHOD_SET_UNIT_COUNT_OCCUPIED, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set used u count failed. ret=%d", ret);
    }

    debug_log(DLOG_INFO, "%s: Occupied unit count init to 0", __FUNCTION__);

    
    input_list = g_slist_append(input_list, g_variant_new_byte(u_count_total));
    ret = dfl_call_class_method(obj_asset_mgmt, METHOD_SET_UNIT_COUNT_TOTAL, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set total u count failed. ret=%d", ret);
    }

    debug_log(DLOG_INFO, "%s: Total unit count is %d", __FUNCTION__, u_count_total);

    return RET_OK;
}


LOCAL void al_brd_status_sync_task(gpointer user_data)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_modbus = (OBJ_HANDLE)user_data;
    OBJ_HANDLE obj_asset_brd = OBJ_HANDLE_COMMON;
    OBJ_HANDLE obj_mb_addr_alloc_mgmt = OBJ_HANDLE_COMMON;
    OBJ_HANDLE obj_rack_asset_mgmt = OBJ_HANDLE_COMMON;
    GSList *asset_brd_list = NULL;
    GSList *asset_brd_node = NULL;
    guint8 uart = 0;
    guint8 status = ASSET_MGMT_SVC_OFF;
    modbus_t ctx = { 0 };

    prctl(PR_SET_NAME, (gulong)TASK_NAME_AL_BRD_STAT_SYNC);

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);

    ret = dal_get_specific_object_byte(CLASS_HW_MB_ADDR_ALLOC_MGMT, PROP_MB_ADDR_ALLOC_UART_CHAN, uart,
        &obj_mb_addr_alloc_mgmt);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: no %s obj found for UART%d", __FUNCTION__, CLASS_HW_MB_ADDR_ALLOC_MGMT, uart);
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset_mgmt);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: RackAssetMgmt obj not found", __FUNCTION__);
        return;
    }

    for (;;) {
        
        vos_task_delay(ASSET_BRD_STAT_SYNC_TIME);

        
        (void)dal_get_property_value_byte(obj_rack_asset_mgmt, PROPERTY_RACK_ASSET_MGMT_SVC_STATUS, &status);
        if (status != ASSET_MGMT_SVC_ON) {
            vos_task_delay(ASSET_BRD_STAT_SYNC_TIME);
            debug_log(DLOG_MASS, "%s: rack asset mgmt service not ready", __FUNCTION__);
            continue;
        }

        
        ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &asset_brd_list);
        if (ret != DFL_OK) {
            vos_task_delay(ASSET_BRD_STAT_SYNC_TIME);
            continue;
        }

        for (asset_brd_node = asset_brd_list; asset_brd_node != NULL; asset_brd_node = asset_brd_node->next) {
            obj_asset_brd = (OBJ_HANDLE)asset_brd_node->data;

            (void)memset_s((void *)&ctx, sizeof(modbus_t), 0, sizeof(modbus_t));
            ret = __modbus_rtu_conn_ctx_init_by_ref_handle(obj_asset_brd, &ctx);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:Connect to slave failed with ref handle (%s). ret=%d", __FUNCTION__,
                    dfl_get_object_name(obj_asset_brd), ret);
                continue;
            }

            
            ret = modbus_update_mcu_version(&ctx, obj_asset_brd);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s failed: can't update mcu firmware version, ret is %d", __FUNCTION__, ret);
            }
            

            
            ret = modbus_refresh_mcu_status(&ctx, obj_asset_brd);
            if (ret == MODBUS_ERR) {
                debug_log(DLOG_DEBUG, "Refresh MCU status for slave(0x%02X) failed", ctx.slave);
            }
        }

        g_slist_free(asset_brd_list);
        asset_brd_list = NULL;
    }
}


LOCAL gint32 notify_brd_presence(OBJ_HANDLE obj_modbus, guint8 brd1_pres)
{
    gint32 ret = RET_OK;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar           connector_type[MAX_PROPERTY_VALUE_LEN] = {0};
    guint8 dev_num = 0;
    OBJ_HANDLE obj_connector;
    guint32 board_id = 0xff;
    guint32 aux_id = 0xff;
    guint8 count = 0; 
    guchar uart_ch = 0;
    modbus_t ctx = { 0 };
    gint32 slave_addr = 0;

    ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Connector object list failed. ret=%d", ret);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart_ch);

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_connector = (OBJ_HANDLE)obj_node->data;
        (void)dal_get_property_value_string(obj_connector, PROPERTY_CONNECTOR_TYPE, connector_type,
            sizeof(connector_type));
        (void)dal_get_property_value_byte(obj_connector, PROPERTY_CONNECTOR_SLOT, &dev_num);
        (void)dal_get_property_value_uint32(obj_connector, PROPERTY_CONNECTOR_ID, &board_id);
        (void)dal_get_property_value_uint32(obj_connector, PROPERTY_CONNECTOR_AUXID, &aux_id);
        if (dev_num > MAX_SLAVE_NUM) {
            debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, dev_num);
            g_slist_free(obj_list);
            return RET_ERR;
        }
        slave_addr = g_mb_addr_tbl[dev_num];

        
        if (strcmp(connector_type, CONNECTOR_MODBUS_DYNAMIC) == 0) {
            
            if (brd1_pres == 1) {
                if ((slave_addr < MB_ADDR_START) || (slave_addr > MB_ADDR_END)) {
                    (void)dfl_notify_presence_v2(obj_connector, board_id, aux_id, 0);
                    debug_log(DLOG_DEBUG, "MCU%d has no legal modbus addr, set to absent", dev_num);
                    continue;
                }

                (void)memset_s((void *)&ctx, sizeof(modbus_t), 0, sizeof(modbus_t));
                ret = modbus_rtu_conn_ctx_init(uart_ch, slave_addr, &ctx);
                if (ret != RET_OK) {
                    debug_log(DLOG_DEBUG, "%s: can't init modbus connection for channel [%d]", __FUNCTION__, uart_ch);
                    continue;
                }

                // 从只读的输入寄存器 INPUT_REG_HW_ID 中读取单板的硬件Id，改成带重试的函数封装
                ret = modbus_get_board_id(&ctx, &board_id);
                if (ret == MODBUS_ERR) {
                    debug_log(DLOG_ERROR, "Read hardware ID from slave(0x%02X) failed", slave_addr);
                    continue;
                }
            }

            ret = dfl_notify_presence_v2(obj_connector, board_id, aux_id, brd1_pres);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "Notify Connector%d presence failed. ret=%d", dev_num, ret);
                continue;
            }

            count++;
            debug_log(DLOG_INFO, "Set Connector%d to %s successfully", dev_num,
                (brd1_pres == 1) ? "present" : "absent");
        }
    }

    g_slist_free(obj_list);

    
    return count;
}


LOCAL gint32 modbus_slave_addr_ping(modbus_t *ctx, gint32 slave_addr, guint8 times)
{
    gint32 ret = RET_OK;
    guint8 count = 0;
    guint8         tab_bytes[MAX_MESSAGE_LENGTH] = {0};

    if ((slave_addr < MB_ADDR_START) || (slave_addr > MB_ADDR_END)) {
        debug_log(DLOG_ERROR, "%s: Illegal modbus slave addr 0x%02X", __FUNCTION__, slave_addr);
        return RET_ERR;
    }

    modbus_set_slave(ctx, slave_addr);

    while (count < times) {
        ret = modbus_report_slave_id(ctx, tab_bytes, sizeof(tab_bytes));
        if (ret == MODBUS_ERR) {
            count++;
            vos_task_delay(200);
            debug_log(DLOG_DEBUG, "%s: Ping slave(0x%02X) failed. times:%d", __FUNCTION__, slave_addr, count);
            continue;
        }

        return RET_OK;
    }

    debug_log(DLOG_ERROR, "%s: Ping slave(0x%02X) failed, out of times(%d)", __FUNCTION__, slave_addr, times);
    return RET_ERR;
}


LOCAL void update_addr_alloc_status(OBJ_HANDLE obj_mb_addr_alloc_mgmt, guint8 slave_id, guint8 alloc_status)
{
    gint32 ret = RET_OK;
    guint8 status_val = 0;

    
    status_val = (alloc_status << 4) + (slave_id & 0x0F);

    
    ret = dal_set_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_STATUS, status_val, DF_SAVE_TEMPORARY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Failed to set Modbus Addr Allocate Status. ret=%d", __FUNCTION__, ret);
        return;
    }

    debug_log(DLOG_INFO, "%s: Set addr alloc status to 0x%02X", __FUNCTION__, status_val);

    return;
}


LOCAL void __pwr_on_first_board(OBJ_HANDLE obj_mb_addr_alloc_mgmt)
{
    guint8 powerall = MCU_PWR_OFF;
    guint8 power1 = MCU_PWR_OFF;
    gint32 ret = RET_OK;

    
    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR_ALL, &powerall);

    if (powerall != MCU_PWR_OFF) {
        ret =
            dal_set_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR_ALL, MCU_PWR_OFF, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Set all asset board power off failed. ret=%d", __FUNCTION__, ret);
            return;
        }
    }

    
    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR1, &power1);

    if (power1 != MCU_PWR_ON) {
        ret = dal_set_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR1, MCU_PWR_ON, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Set asset board 1 power on failed. ret=%d", __FUNCTION__, ret);
            return;
        }

        debug_log(DLOG_DEBUG, "%s: Set asset board 1 power on", __FUNCTION__);

        
        vos_task_delay(MCU_BOOT_UP_TIME);
    }

    return;
}


LOCAL gint32 modbus_slave_addr_alloc(OBJ_HANDLE obj_mb_addr_alloc_mgmt)
{
    gint32 ret = RET_OK;
    guint8 slave_idx = 0;
    gint32 def_addr = 0;
    gint32 slave_addr_start = 0;
    guint8 max_slave_num = MAX_SLAVE_NUM;
    gint32 slave_addr_new = 0;
    guint8 status = 0;
    guint8 history_slave_id = 0;
    modbus_t ctx = { 0 };
    guchar uart_ch = 0;

    
    ret = mb_restore_slave(obj_mb_addr_alloc_mgmt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Restore default addr failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    __pwr_on_first_board(obj_mb_addr_alloc_mgmt);

    (void)dal_get_property_value_int32(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_DEFAULT_ADDR, &def_addr);
    (void)dal_get_property_value_int32(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_START_ADDR, &slave_addr_start);
    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_UART_CHAN, &uart_ch);
    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_MAX_SLAVE_NUM, &max_slave_num);

    if ((slave_addr_start + max_slave_num) > MB_ADDR_END) {
        debug_log(DLOG_ERROR, "%s: Illegal slave_address_start", __FUNCTION__);
        update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_SYS_ERROR);
        return RET_ERR;
    }

    ret = modbus_rtu_conn_ctx_init(uart_ch, def_addr, &ctx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: init connection context for UART%d failed", __FUNCTION__, uart_ch);
        update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_SYS_ERROR);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: modbus_slave_addr_alloc begin", __FUNCTION__);

    slave_addr_new = slave_addr_start;

    while (modbus_slave_addr_ping(&ctx, def_addr, PING_DEFAULT_SLAVE_TIMES) == RET_OK) {
        debug_log(DLOG_DEBUG, "%s: Ping slave%d with default addr 0x%02X OK", __FUNCTION__, slave_idx + 1, def_addr);

        ret = modbus_set_new_addr(&ctx, slave_addr_new);
        if (ret == MODBUS_ERR) {
            debug_log(DLOG_ERROR, "%s: Failed to write slave address : 0x%02X", __FUNCTION__, slave_addr_new);
            update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_PROTO_ERROR);
            return RET_ERR;
        }

        debug_log(DLOG_DEBUG, "%s: Set new addr=0x%02X to slave(0x%02X)", __FUNCTION__, slave_addr_new, def_addr);

        ret = modbus_slave_addr_ping(&ctx, slave_addr_new, PING_DEFAULT_SLAVE_TIMES);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Failed to change new slave address : 0x%02X", __FUNCTION__, slave_addr_new);
            update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_PROTO_ERROR);
            return RET_ERR;
        }

        debug_log(DLOG_INFO, "%s: Ping slave%d with new addr 0x%02X OK", __FUNCTION__, slave_idx + 1, slave_addr_new);

        ret = modbus_next_power_on(&ctx);
        if (ret == MODBUS_ERR) {
            debug_log(DLOG_ERROR, "%s: Failed to send init next power cmd", __FUNCTION__);
            update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_PROTO_ERROR);
            return RET_ERR;
        }

        g_mb_addr_tbl[slave_idx + 1] = slave_addr_new;
        debug_log(DLOG_DEBUG, "%s: Send next_pwr_on cmd to slave%d(0x%02X)", __FUNCTION__, slave_idx + 1,
            slave_addr_new);

        slave_idx++;
        slave_addr_new++;
    }

    debug_log(DLOG_INFO, "%s: No default addr slave found. Current SlaveId=%d", __FUNCTION__, slave_idx + 1);

    
    ret = dal_set_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR_ALL, MCU_PWR_ON, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Failed to set powerall. ret=%d", __FUNCTION__, ret);
        update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_SYS_ERROR);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: Power on all slave", __FUNCTION__);

    
    ret = modbus_slave_addr_ping(&ctx, def_addr, PING_DEFAULT_SLAVE_TIMES);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Found slave with default addr, error occured", __FUNCTION__);
        update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx + 1, ALLOC_ADDR_DEV_ERROR);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: No default addr slave found after power on all slave", __FUNCTION__);

    // 只修改状态位，索引ID位保持不变，以便恢复告警时能正确显示
    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_STATUS, &status);
    history_slave_id = status & 0x0F;

    // 若持久化的ID为无效值0，则修改为新遍历到的值
    if (history_slave_id == 0) {
        history_slave_id = slave_idx + 1;
    }

    update_addr_alloc_status(obj_mb_addr_alloc_mgmt, history_slave_id, ALLOC_ADDR_OK);

    return RET_OK;
}


LOCAL gint32 asset_locator_pwr_switch(OBJ_HANDLE obj_mb_addr_alloc_mgmt, guint8 option)
{
    gint32 ret = RET_OK;
    guint8 power1 = 0;
    guint8 powerall = 0;

    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR_ALL, &powerall);

    if (option != powerall) {
        ret = dal_set_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR_ALL, option, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Set all asset board power %s failed. ret=%d", __FUNCTION__,
                (option == MCU_PWR_ON) ? "on" : "off", ret);
            return RET_ERR;
        }
    }

    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR1, &power1);

    if (option != power1) {
        ret = dal_set_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PWR1, option, DF_HW);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Set asset board 1 power %s failed. ret=%d", __FUNCTION__,
                (option == MCU_PWR_ON) ? "on" : "off", ret);
            return RET_ERR;
        }
    }

    vos_task_delay(MCU_BOOT_UP_TIME);

    return RET_OK;
}


LOCAL gint32 mb_restore_slave(OBJ_HANDLE obj_mb_addr_alloc_mgmt)
{
    gint32 ret = RET_OK;

    // 先确保上电信号去除使能
    ret = asset_locator_pwr_switch(obj_mb_addr_alloc_mgmt, MCU_PWR_OFF);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set asset board power off failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 使全部MCU上电后，执行地址恢复流程
    ret = asset_locator_pwr_switch(obj_mb_addr_alloc_mgmt, MCU_PWR_ON);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set asset board power on failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    __restore_addr_of_mb_slave(obj_mb_addr_alloc_mgmt);

    debug_log(DLOG_INFO, "%s: All slave power on, try restore default modbus addr ...", __FUNCTION__);

    // 确保上电信号去除使能，避免多个单板同时处于上电状态，ping默认地址时有多个响应导致分配地址失败
    ret = asset_locator_pwr_switch(obj_mb_addr_alloc_mgmt, MCU_PWR_OFF);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set asset board power off failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: Addr restored, try realloc new modbus addr ...", __FUNCTION__);

    return RET_OK;
}


LOCAL gint32 mb_check_and_prepare(OBJ_HANDLE obj_al_board, OBJ_HANDLE obj_modbus, guint8 *need_upgrade_count)
{
    gint32 ret = RET_OK;
    guint8 uart = 0;
    GSList *asset_brd_list = NULL;
    GSList *asset_brd_node = NULL;
    OBJ_HANDLE obj_asset_brd;
    OBJ_HANDLE obj_mb_slave;
    OBJ_HANDLE obj_ref_bus;
    guint8 slave_id = 0;
    gint32 slave_addr = 0;
    gchar           full_file_name[FULL_FILE_NAME_LEN] = {0};
    
    gchar           expect_ver[MAX_MCU_FW_VER_LEN] = {0};
    gchar           firm_ver[MAX_MCU_FW_VER_LEN] = {0};
    
    guint8 boot_mode = 0;
    guint32 file_size = 0;
    guint8 al_brd_slot = 0;
    modbus_t ctx = { 0 };
    gchar           file_name[FILE_NAME_LEN] = {0};

    if (need_upgrade_count == NULL) {
        debug_log(DLOG_ERROR, "%s: input para is invalid", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);

    // 获取预期的固件名
    (void)dal_get_property_value_string(obj_al_board, PROPERTY_ASSET_LOCATE_BRD_FIRM_NAME, file_name,
        sizeof(file_name));
    debug_log(DLOG_DEBUG, "%s: Expect mcu firmware file name: %s", __FUNCTION__, file_name);

    // 获取预期的固件版本
    (void)dal_get_property_value_string(obj_al_board, PROPERTY_ASSET_LOCATE_BRD_EXPECT_VER, expect_ver,
        sizeof(expect_ver));
    debug_log(DLOG_INFO, "%s: Expect mcu firmware version: %s", __FUNCTION__, expect_ver);

    // 获取升级固件全名
    ret = get_firmware_full_name(file_name, full_file_name, sizeof(full_file_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get full firmware file name for mcu failed", __FUNCTION__);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "%s: Full mcu firmware file name: %s", __FUNCTION__, full_file_name);

    // 获取文件内容长度
    file_size = vos_get_file_length(full_file_name);
    if (file_size > FIRMWARE_FILE_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: File size=%d is out of limit=%d", __FUNCTION__, file_size, FIRMWARE_FILE_MAX_SIZE);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: MCU firmware file size: %d", __FUNCTION__, file_size);

    // 遍历各资产定位单板
    ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &asset_brd_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:Get asset board obj list failed. ret=%d", __FUNCTION__, ret);
        return RET_OK;
    }

    for (asset_brd_node = asset_brd_list; asset_brd_node != NULL; asset_brd_node = asset_brd_node->next) {
        obj_asset_brd = (OBJ_HANDLE)asset_brd_node->data;

        
        (void)dal_get_property_value_byte(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_SLOT, &al_brd_slot);

        if ((g_debug_upgrade_al_brd_slot != UPGRADE_AL_BRD_ALL) && (g_debug_upgrade_al_brd_slot != al_brd_slot)) {
            continue;
        }

        

        // 获取当前单板的固件版本信息
        (void)dal_get_property_value_string(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, firm_ver,
            sizeof(firm_ver));
        debug_log(DLOG_DEBUG, "%s: %s mcu firmware version: %s", __FUNCTION__, dfl_get_object_name(obj_asset_brd),
            firm_ver);

        // 升级策略调整，对于版本号无效的情况，也进行升级
        if ((strlen(firm_ver) == 0) || (strcmp(firm_ver, "N/A") == 0)) {
            debug_log(DLOG_ERROR, "%s:Invalid mcu firmware version, will upgrade", __FUNCTION__);
        }

        // 对于版本与预期不一致的单板, 置升级标志，切升级模式，发送升级开始命令(附带升级文件大小及分片大小信息)
        
        
        if (strcmp(expect_ver, firm_ver) != 0 || (g_debug_upgrade_al_brd_slot != UPGRADE_AL_BRD_ALL)) {
            
            // 需要升级的MCU计数
            (*need_upgrade_count)++;

            
            ret = dal_get_specific_object_backward_position(obj_asset_brd, CLASS_MODBUS_SLAVE, &obj_mb_slave);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:Get slave for %s failed. ret=%d", __FUNCTION__,
                    dfl_get_object_name(obj_asset_brd), ret);
                continue;
            }

            
            ret = dfl_get_referenced_object(obj_mb_slave, PROPERTY_MB_SLAVE_LBUS, &obj_ref_bus);
            if ((ret != DFL_OK) || (obj_ref_bus != obj_modbus)) {
                continue;
            }

            // SlaveId与单板槽位号挂钩
            (void)dal_get_property_value_byte(obj_mb_slave, PROPERTY_MB_SLAVE_ID, &slave_id);
            (void)dal_get_property_value_int32(obj_mb_slave, PROPERTY_MB_SLAVE_ADDR, &slave_addr);
            if (slave_id > MAX_SLAVE_NUM) {
                debug_log(DLOG_ERROR, "[%s] Array subscript out of range.", __FUNCTION__);
                g_slist_free(asset_brd_list);
                return RET_ERR;
            }

            // 若为待生效模式，则说明进入了重试流程，但当前Slave收包是成功并校验通过的，所以跳过
            if (g_need_active[slave_id] == 1) {
                debug_log(DLOG_INFO, "%s: Slave%d(0x%02X) has firmware to be activated, skip", __FUNCTION__, slave_id,
                    slave_addr);
                continue;
            }

            // 若待生效标志未置位，但升级标志已经置位，则说明进入了重试流程，且当前Slave收包或校验不成功
            if (g_need_upgrade[slave_id] == 1) {
                debug_log(DLOG_INFO, "%s: Slave%d(0x%02X) retry upgrade...", __FUNCTION__, slave_id, slave_addr);
            }

            // 若需要升级的标志未置位，则说明是首次进入该流程，记录维护日志
            if (g_need_upgrade[slave_id] == 0) {
                maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Slave%d(0x%02X) need firmware upgrade", slave_id,
                    slave_addr);
            }

            // 置升级标志
            g_need_upgrade[slave_id] = 1;
            debug_log(DLOG_DEBUG, "%s: Mark slave%d(0x%02X) need to upgrade", __FUNCTION__, slave_id, slave_addr);

            
            (void)memset_s((void *)&ctx, sizeof(modbus_t), 0, sizeof(modbus_t));
            ret = modbus_rtu_conn_ctx_init(uart, slave_addr, &ctx);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:Connect to slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr, ret);
                continue;
            }

            // 获取当前工作模式; 若获取失败，则默认其还在APP模式，需要做切换到UPGRADE模式的操作
            boot_mode = MCU_BOOT_MODE_INVALID;
            ret = modbus_get_boot_mode(&ctx, &boot_mode);
            if (ret == MODBUS_ERR) {
                boot_mode = MCU_BOOT_MODE_APP;
                debug_log(DLOG_ERROR, "%s: Get boot mode from slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr,
                    ret);
            }

            debug_log(DLOG_DEBUG, "%s: Slave(0x%02X) mode: %s", __FUNCTION__, slave_addr,
                (boot_mode == MCU_BOOT_MODE_UPGRAD) ? "UPGRADE" : "APP");

            // 若不是升级模式，则切换到升级模式(重试时预期无需切换)
            if (boot_mode != MCU_BOOT_MODE_UPGRAD) {
                // 设置下次的启动工作模式为升级模式
                boot_mode = MCU_BOOT_MODE_UPGRAD;
                ret = modbus_set_boot_mode(&ctx, boot_mode);
                if (ret == MODBUS_ERR) {
                    debug_log(DLOG_ERROR, "%s: Set boot mode(%d) for slave(0x%02X) failed. ret=%d", __FUNCTION__,
                        boot_mode, slave_addr, ret);
                    continue;
                }

                debug_log(DLOG_DEBUG, "%s: Set slave(0x%02X) to mode: %s", __FUNCTION__, slave_addr,
                    (boot_mode == MCU_BOOT_MODE_UPGRAD) ? "UPGRADE" : "APP");

                // 复位MCU，使其进入升级模式
                ret = modbus_reset_mcu(&ctx);
                if (ret == MODBUS_ERR) {
                    debug_log(DLOG_ERROR, "%s: Reset slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr, ret);
                    continue;
                }

                // 复位MCU需要等待启动时间
                vos_task_delay(MCU_BOOT_UP_TIME);

                // 等待切换升级模式成功或超时
                boot_mode = MCU_BOOT_MODE_INVALID;
                (void)modbus_get_boot_mode(&ctx, &boot_mode);

                // 检测失败则关闭与当前MCU的连接并记录错误
                if (boot_mode != MCU_BOOT_MODE_UPGRAD) {
                    debug_log(DLOG_ERROR, "%s: Slave(0x%02X) switch to upgrade mode failed. boot_mode=%d", __FUNCTION__,
                        slave_addr, boot_mode);
                    continue;
                }
            }

            // 发送升级开始命令，附带文件包大小及分帧信息
            ret = modbus_upload_start(&ctx, FILE_TYPE, file_size, FILE_EXC_FRAME_SIZE);
            if (ret == MODBUS_ERR) {
                debug_log(DLOG_ERROR, "%s: Send upload start cmd to slave(0x%02X) failed. ret=%d", __FUNCTION__,
                    slave_addr, ret);
                continue;
            }

            debug_log(DLOG_DEBUG, "%s: Send start upload cmd to slave(0x%02X) with file_size=%d", __FUNCTION__,
                slave_addr, file_size);
        }
    }

    g_slist_free(asset_brd_list);

    return ret;
}


LOCAL gint32 mb_broadcast_send_file(OBJ_HANDLE obj_al_board, OBJ_HANDLE obj_modbus, guint16 *file_crc)
{
    gint32 ret = RET_OK;
    guint8 uart = 0;
    gchar       file_name[FILE_NAME_LEN] = {0};
    gchar       full_file_name[FULL_FILE_NAME_LEN] = {0};
    guchar *file_buffer = NULL;
    guint32 file_size = 0;
    guint32 remain = 0;
    guint16 frame_idx = 0;
    guint32 offset = 0;
    guint8 data_len = 0;
    modbus_t ctx = { 0 };

    if (file_crc == NULL) {
        debug_log(DLOG_ERROR, "%s: input para is NULL", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);

    // 获取预期的固件名
    (void)dal_get_property_value_string(obj_al_board, PROPERTY_ASSET_LOCATE_BRD_FIRM_NAME, file_name,
        sizeof(file_name));

    // 获取升级固件全名
    ret = get_firmware_full_name(file_name, full_file_name, sizeof(full_file_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get full firmware file name for mcu failed", __FUNCTION__);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "%s: Full mcu firmware file name: %s", __FUNCTION__, full_file_name);

    // 获取文件内容长度并校验

    file_size = vos_get_file_length(full_file_name);
    if (file_size > FIRMWARE_FILE_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: File size=%d is out of limit=%d", __FUNCTION__, file_size, FIRMWARE_FILE_MAX_SIZE);
        return RET_ERR;
    }

    // 申请读取文件内容的空间并初始化，注意释放
    file_buffer = (guchar *)g_malloc(file_size);
    if (file_buffer == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc memory for file %s failed", __FUNCTION__, file_name);
        return RET_ERR;
    }

    (void)memset_s(file_buffer, file_size, 0, file_size);

    // 加载文件内容到buf中
    ret = load_file_to_buffer(full_file_name, file_buffer, file_size);
    if (ret != RET_OK) {
        g_free(file_buffer);
        debug_log(DLOG_ERROR, "%s: Load file to buffer failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    // 计算CRC16
    *file_crc = modbus_calcu_crc16(file_buffer, file_size);
    debug_log(DLOG_INFO, "%s: MCU firmware crc: 0x%04X", __FUNCTION__, *file_crc);

    
    ret = modbus_rtu_conn_ctx_init(uart, MB_ADDR_BROADCAST, &ctx);
    if (ret != RET_OK) {
        g_free(file_buffer);
        debug_log(DLOG_ERROR, "%s:Connect to slave(0x%02X) failed. ret=%d", __FUNCTION__, MB_ADDR_BROADCAST, ret);
        return RET_ERR;
    }

    // 多帧传包，帧序号从1开始
    frame_idx = 1;
    offset = 0;

    do {
        remain = file_size - offset;
        data_len = (remain < FILE_EXC_FRAME_SIZE) ? remain : FILE_EXC_FRAME_SIZE;

        ret = modbus_broadcast_upload(&ctx, FILE_TYPE, frame_idx, data_len, file_buffer + offset);
        if (ret == MODBUS_ERR) {
            g_free(file_buffer);
            debug_log(DLOG_ERROR, "%s:Send file frame%d (offset=%d) failed. ret=%d", __FUNCTION__, frame_idx, offset,
                ret);
            return RET_ERR;
        }
        if ((frame_idx % WAIT_FRAME_DIVSOR) == 0) {
            debug_log(DLOG_DEBUG, "%s: frame_idx=%d, data_len=%d, offset=%d", __FUNCTION__, frame_idx, data_len,
                offset);
        }

        frame_idx++;
        offset += FILE_EXC_FRAME_SIZE;
        vos_task_delay(MB_STANDARD_CMD_INTERVAL);
    } while (remain > FILE_EXC_FRAME_SIZE);

    debug_log(DLOG_DEBUG, "%s: last frame_idx=%d", __FUNCTION__, frame_idx);

    g_free(file_buffer);

    return RET_OK;
}


LOCAL gint32 mb_recv_file_check(OBJ_HANDLE obj_modbus, guint16 file_crc, guint8 *need_retry_count)
{
    gint32 ret = RET_OK;
    guint8 uart = 0;
    GSList *asset_brd_list = NULL;
    GSList *asset_brd_node = NULL;
    OBJ_HANDLE obj_asset_brd;
    OBJ_HANDLE obj_mb_slave;
    OBJ_HANDLE obj_ref_bus;
    guint8 slave_id = 0;
    gint32 slave_addr = 0;
    guint8 boot_mode = 0;
    guint8 check_status = UPGRADE_RECIVE_FAIL;
    modbus_t ctx = { 0 };

    if (need_retry_count == NULL) {
        debug_log(DLOG_ERROR, "%s: input para is invalid", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);

    ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &asset_brd_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get asset board obj list failed. ret=%d", __FUNCTION__, ret);
        return RET_OK;
    }

    for (asset_brd_node = asset_brd_list; asset_brd_node != NULL; asset_brd_node = asset_brd_node->next) {
        obj_asset_brd = (OBJ_HANDLE)asset_brd_node->data;

        
        check_status = UPGRADE_RECIVE_FAIL;

        
        ret = dal_get_specific_object_backward_position(obj_asset_brd, CLASS_MODBUS_SLAVE, &obj_mb_slave);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: Get slave for %s failed. ret=%d", __FUNCTION__,
                dfl_get_object_name(obj_asset_brd), ret);
            continue;
        }

        
        ret = dfl_get_referenced_object(obj_mb_slave, PROPERTY_MB_SLAVE_LBUS, &obj_ref_bus);
        if ((ret != DFL_OK) || (obj_ref_bus != obj_modbus)) {
            continue;
        }

        // SlaveId与单板槽位号挂钩
        (void)dal_get_property_value_byte(obj_mb_slave, PROPERTY_MB_SLAVE_ID, &slave_id);
        (void)dal_get_property_value_int32(obj_mb_slave, PROPERTY_MB_SLAVE_ADDR, &slave_addr);

        // 若为待生效模式，则说明进入了重试流程，但当前Slave收包是成功并校验通过的，所以跳过
        if (slave_id <= MAX_SLAVE_NUM && (g_need_active[slave_id]) == 1) {
            debug_log(DLOG_INFO, "%s: Slave%d(0x%02X) has firmware to be activated, skip", __FUNCTION__, slave_id,
                slave_addr);
            continue;
        }

        // 若待生效标志未置位，但升级标志已经置位，则说明进入了重试流程，且当前Slave收包或校验不成功
        if (slave_id <= MAX_SLAVE_NUM && g_need_upgrade[slave_id] == 1) {
            debug_log(DLOG_INFO, "%s: Slave%d(0x%02X) retry check...", __FUNCTION__, slave_id, slave_addr);

            // 先默认将标志++，校验一致后再--
            (*need_retry_count)++;

            
            (void)memset_s((void *)&ctx, sizeof(modbus_t), 0, sizeof(modbus_t));
            ret = modbus_rtu_conn_ctx_init(uart, slave_addr, &ctx);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Connect to slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr, ret);
                continue;
            }

            // 获取当前工作模式，预期仍在升级模式
            boot_mode = MCU_BOOT_MODE_INVALID;
            ret = modbus_get_boot_mode(&ctx, &boot_mode);
            if (ret == MODBUS_ERR) {
                debug_log(DLOG_ERROR, "%s: Get boot mode from slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr,
                    ret);
                continue;
            }

            debug_log(DLOG_DEBUG, "%s: Slave(0x%02X) is in %s mode.", __FUNCTION__, slave_addr,
                (boot_mode == MCU_BOOT_MODE_UPGRAD) ? "UPGRADE" : "APP");

            if (boot_mode != MCU_BOOT_MODE_UPGRAD) {
                debug_log(DLOG_ERROR, "%s: Slave(0x%02X) not in upgrade mode. boot_mode=%d", __FUNCTION__, slave_addr,
                    boot_mode);
                continue;
            }

            // 发送升级结束命令，附带文件总校验和
            ret = modbus_upload_complete(&ctx, FILE_TYPE, file_crc, &check_status);
            if (ret == MODBUS_ERR) {
                debug_log(DLOG_ERROR, "%s: Send upload complete cmd to slave(0x%02X) failed. ret=%d", __FUNCTION__,
                    slave_addr, ret);
                continue;
            }

            // 若校验的结果为1，则标记待生效，清除之前自增的重试计数
            if (check_status == UPGRADE_RECIVE_OK) {
                g_need_active[slave_id] = 1;
                (*need_retry_count)--;
                maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK,
                    "Slave%d(0x%02X) recive firmware successfully, check sum is 0x%04X", slave_id, slave_addr,
                    file_crc);
                debug_log(DLOG_INFO, "%s: Slave%d(0x%02X) firmware check successfully, mark to activate", __FUNCTION__,
                    slave_id, slave_addr);
            } else {
                debug_log(DLOG_ERROR, "%s: Slave%d(0x%02X) firmware check failed, need retry", __FUNCTION__, slave_id,
                    slave_addr);
            }
        }
    }

    g_slist_free(asset_brd_list);

    return ret;
}


LOCAL gint32 mb_load_activate(OBJ_HANDLE obj_al_board, OBJ_HANDLE obj_modbus, guint8 *act_fail_count)
{
    guint8 uart = 0;
    gint32 ret = RET_OK;
    GSList *asset_brd_list = NULL;
    GSList *asset_brd_node = NULL;
    OBJ_HANDLE obj_asset_brd;
    OBJ_HANDLE obj_mb_slave;
    OBJ_HANDLE obj_ref_bus;
    guint8 slave_id = 0;
    gint32 slave_addr = 0;
    guint8 boot_mode = 0;
    
    gchar           firm_ver[MAX_MCU_FW_VER_LEN] = {0};
    gchar           expect_ver[MAX_MCU_FW_VER_LEN] = {0};
    
    modbus_t ctx = { 0 };

    if (act_fail_count == NULL) {
        debug_log(DLOG_ERROR, "%s: input para is invalid", __FUNCTION__);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);
    (void)dal_get_property_value_string(obj_al_board, PROPERTY_ASSET_LOCATE_BRD_EXPECT_VER, expect_ver,
        sizeof(expect_ver));

    ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &asset_brd_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Get asset board obj list failed. ret=%d", __FUNCTION__, ret);
        return RET_OK;
    }

    for (asset_brd_node = asset_brd_list; asset_brd_node != NULL; asset_brd_node = asset_brd_node->next) {
        obj_asset_brd = (OBJ_HANDLE)asset_brd_node->data;

        
        ret = dal_get_specific_object_backward_position(obj_asset_brd, CLASS_MODBUS_SLAVE, &obj_mb_slave);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Get slave for %s failed. ret=%d", __FUNCTION__,
                dfl_get_object_name(obj_asset_brd), ret);
            continue;
        }

        
        ret = dfl_get_referenced_object(obj_mb_slave, PROPERTY_MB_SLAVE_LBUS, &obj_ref_bus);
        if ((ret != DFL_OK) || (obj_ref_bus != obj_modbus)) {
            continue;
        }

        // SlaveId与单板槽位号挂钩
        (void)dal_get_property_value_byte(obj_mb_slave, PROPERTY_MB_SLAVE_ID, &slave_id);
        (void)dal_get_property_value_int32(obj_mb_slave, PROPERTY_MB_SLAVE_ADDR, &slave_addr);

        
        if (slave_id <= MAX_SLAVE_NUM && g_need_upgrade[slave_id] == 0) {
            debug_log(DLOG_DEBUG, "%s: Slave%d(0x%02X) no need to upgrade, skip active schedule", __FUNCTION__,
                slave_id, slave_addr);
            continue;
        }

        
        if (slave_id <= MAX_SLAVE_NUM && g_need_active[slave_id] != 1) {
            debug_log(DLOG_ERROR, "%s: Slave%d(0x%02X) need upgrade but has no firmware to activate", __FUNCTION__,
                slave_id, slave_addr);
        }

        // 先默认将标志++，生效成功后再--
        (*act_fail_count)++;

        
        memset_s((void *)&ctx, sizeof(ctx), 0, sizeof(ctx));
        ret = modbus_rtu_conn_ctx_init(uart, slave_addr, &ctx);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:Connect to slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr, ret);
            continue;
        }

        // 获取当前工作模式，预期仍在升级模式; 若获取失败，则默认其还在UPGRADE模式，需要做切换回APP模式的操作
        boot_mode = MCU_BOOT_MODE_INVALID;
        ret = modbus_get_boot_mode(&ctx, &boot_mode);
        if (ret == MODBUS_ERR) {
            boot_mode = MCU_BOOT_MODE_UPGRAD;
            debug_log(DLOG_ERROR, "Get boot mode from slave(0x%02X) failed. ret=%d", slave_addr, ret);
        }

        debug_log(DLOG_DEBUG, "%s: Slave(0x%02X) mode: %s", __FUNCTION__, slave_addr,
            (boot_mode == MCU_BOOT_MODE_UPGRAD) ? "UPGRADE" : "APP");

        // 若已经为APP模式，则打印异常并跳过
        if (boot_mode == MCU_BOOT_MODE_APP) {
            debug_log(DLOG_ERROR, "Slave(0x%02X) need activate but already in APP mode", slave_addr);
            continue;
        }

        // 设置下次的启动工作模式为APP模式
        boot_mode = MCU_BOOT_MODE_APP;
        ret = modbus_set_boot_mode(&ctx, boot_mode);
        if (ret == MODBUS_ERR) {
            debug_log(DLOG_ERROR, "Set boot mode(%d) to slave(0x%02X) failed. ret=%d", boot_mode, slave_addr, ret);
            continue;
        }

        debug_log(DLOG_DEBUG, "%s: Set slave(0x%02X) to mode: %s", __FUNCTION__, slave_addr,
            (boot_mode == MCU_BOOT_MODE_UPGRAD) ? "UPGRADE" : "APP");

        // 复位MCU，使其进入APP模式
        ret = modbus_reset_mcu(&ctx);
        if (ret == MODBUS_ERR) {
            debug_log(DLOG_ERROR, "Send reset cmd to slave(0x%02X) failed. ret=%d", slave_addr, ret);
            continue;
        }

        // 复位MCU需要等待启动时间
        vos_task_delay(MCU_BOOT_UP_TIME);

        // 等待切换升级模式成功或超时
        boot_mode = MCU_BOOT_MODE_INVALID;

        (void)modbus_get_boot_mode(&ctx, &boot_mode);

        // 检测失败则关闭与当前MCU的连接并记录错误
        if (boot_mode != MCU_BOOT_MODE_APP) {
            debug_log(DLOG_ERROR, "Slave(0x%02X) switch to APP mode failed. boot_mode=%d", slave_addr, boot_mode);
            continue;
        }

        // 生效成功后清标志
        debug_log(DLOG_DEBUG, "%s: Slave(0x%02X) now switched to mode: %s", __FUNCTION__, slave_addr,
            (boot_mode == MCU_BOOT_MODE_UPGRAD) ? "UPGRADE" : "APP");
        (void)dal_get_property_value_string(obj_asset_brd, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER, firm_ver,
            sizeof(firm_ver));
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Slave%d(0x%02X) activate firmware from %s to %s successfully",
            slave_id, slave_addr, firm_ver, expect_ver);
        (*act_fail_count)--;
    }

    g_slist_free(asset_brd_list);

    return ret;
}


LOCAL gint32 mcu_check_and_upgrade(OBJ_HANDLE obj_modbus)
{
    gint32 ret = RET_OK;
    guint32 count = 0;
    OBJ_HANDLE obj_al_board = 0;
    guint8 need_upgrade_count = 0;
    guint8 need_retry_count = 0;
    guint8 act_fail_count = 0;
    guint16 file_crc = 0;

    // 资产定位板的MCU固件包名及版本维护在资产定位板本身的类和对象中
    ret = dal_get_object_handle_nth(CLASS_ASSET_LOCATE_BOARD, 0, &obj_al_board);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: AssetLocateBoard obj not found", __FUNCTION__);
        return RET_OK;
    }

    // 固件升级检查、传包、校验、生效全流程，最多执行次数限制
    while (count < UPGRADE_RETRY_TIMES) {
        count++;

        // 计数归0
        need_upgrade_count = 0;
        need_retry_count = 0;

        debug_log(DLOG_INFO, "-------------------------- MCU upgrade check %d --------------------------", count);

        // 升级检查和准备(切换到升级模式)
        (void)mb_check_and_prepare(obj_al_board, obj_modbus, &need_upgrade_count);

        // 若需要升级的MCU计数为0，即所有MCU版本都符合预期，则直接return
        if (need_upgrade_count == 0) {
            debug_log(DLOG_INFO, "%s: check %d: All firmware version match, no need to upgrade", __FUNCTION__, count);
            return RET_OK;
        }

        debug_log(DLOG_DEBUG, "%s: check %d: need_upgrade_count=%d", __FUNCTION__, count, need_upgrade_count);
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "%d MCU need firmware upgrade.(try times %d)", need_upgrade_count,
            count);

        // 广播传包
        ret = mb_broadcast_send_file(obj_al_board, obj_modbus, &file_crc);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "%s: check %d: Broadcast send file failed. ret=%d", __FUNCTION__, count, ret);
            continue;
        }

        // 校验升级包
        (void)mb_recv_file_check(obj_modbus, file_crc, &need_retry_count);

        // 若需要重传的MCU计数为0，即所有MCU收包正确，则直接return
        if (need_retry_count == 0) {
            debug_log(DLOG_INFO, "%s: check %d: All firmware recive ok, no need to retry", __FUNCTION__, count);
            break;
        }

        debug_log(DLOG_DEBUG, "%s: check %d: need_retry_count=%d", __FUNCTION__, count, need_retry_count);
    }

    if (count >= UPGRADE_RETRY_TIMES) {
        debug_log(DLOG_ERROR, "%s: Upgrade failed, out of times(%d)", __FUNCTION__, count);
    }

    // 升级生效(切换回APP模式)
    ret = mb_load_activate(obj_al_board, obj_modbus, &act_fail_count);
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: Active new firmware file failed. ret=%d", __FUNCTION__, ret);
        maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_MCU_UPGRADE_FAIL, "Upgrade MCU firmware failed");
        return RET_ERR;
    }

    
    (void)memset_s(g_need_upgrade, sizeof(g_need_upgrade), 0, sizeof(g_need_upgrade));
    (void)memset_s(g_need_active, sizeof(g_need_active), 0, sizeof(g_need_active));
    

    // 若生效失败计数不为0，则标记升级失败
    if (act_fail_count != 0) {
        debug_log(DLOG_ERROR, "%s: Activate new firmware faild, count=%d", __FUNCTION__, act_fail_count);
        maintenance_log_v2(MLOG_WARN, FC_CARD_MNG_MCU_UPGRADE_FAIL, "Upgrade MCU firmware failed");
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "%s: Active new firmware OK", __FUNCTION__);
    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Upgrade MCU firmware OK");

    return RET_OK;
}


LOCAL gint32 set_rack_asset_mgmt_svc_status(OBJ_HANDLE obj_rack_asset_mgmt, ASSET_MGMT_STATUS status)
{
    gint32 ret = RET_OK;
    GSList *input_list = NULL;

    input_list = g_slist_append(input_list, g_variant_new_byte(status));

    ret = dfl_call_class_method(obj_rack_asset_mgmt, METHOD_SET_ASSET_MGMT_STAT, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:Set status to %d failed. ret=%d", __FUNCTION__, status, ret);
        return ret;
    }

    debug_log(DLOG_INFO, "%s:Set status to %s", __FUNCTION__,
        (status == ASSET_MGMT_SVC_OFF) ? "unavaliable" : "available");
    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Set rack asset management service status to %s",
        (status == ASSET_MGMT_SVC_OFF) ? "unavaliable" : "available");

    return RET_OK;
}


LOCAL void wait_until_obj_load_unload_finish(const gchar *obj_class_name, gint32 expect_obj_cnt, guint8 tmout_cnt)
{
    gint32 ret = RET_OK;
    GSList *obj_list = NULL;
    guint8 wait_count = 0;
    guint8 obj_count = 0;
    gboolean wait_condition = FALSE;

    if (expect_obj_cnt < 0) {
        return;
    }

    do {
        wait_count++;
        ret = dfl_get_object_list(obj_class_name, &obj_list);
        obj_count = g_slist_length(obj_list);

        
        wait_condition = (expect_obj_cnt != obj_count);
        if ((ret != DFL_OK) || (wait_condition == TRUE)) {
            if (obj_list != NULL) {
                g_slist_free(obj_list);
            }
            obj_list = NULL;
            debug_log(DLOG_DEBUG, "%s: wait for %d ms until all %s obj %s, times=%d", __FUNCTION__,
                WAIT_FOR_XML_LOAD_UNLOAD, obj_class_name, (expect_obj_cnt == 0) ? "unload" : "load", wait_count);
            vos_task_delay(WAIT_FOR_XML_LOAD_UNLOAD);
        } else {
            if (obj_list != NULL) {
                g_slist_free(obj_list);
            }
            obj_list = NULL;
            break;
        }
    } while (wait_count < tmout_cnt);

    return;
}


LOCAL void __restore_addr_of_mb_slave(OBJ_HANDLE obj_mb_addr_alloc_mgmt)
{
    gint32 ret;
    guchar uart_ch = 0;
    modbus_t ctx = { 0 };

    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_UART_CHAN, &uart_ch);

    ret = modbus_rtu_conn_ctx_init(uart_ch, MB_ADDR_BROADCAST, &ctx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Failed to connect slave(0x%02X).", __FUNCTION__, MB_ADDR_BROADCAST);
        return;
    }

    
    modbus_restore_addr(&ctx);
    

    return;
}


LOCAL void al_brd_detect_task(gpointer user_data)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_modbus = OBJ_HANDLE_COMMON;
    OBJ_HANDLE obj_asset_mgmt = OBJ_HANDLE_COMMON;
    OBJ_HANDLE obj_mb_addr_alloc_mgmt = (OBJ_HANDLE)user_data;
    guint8 uart = UART4;
    guint8 detected_last = 0;
    guint8 detected = 0;
    guint8 alloc_status = 0;
    guint8 slave_idx = 0;
    gint32 brd_count = 0;

    prctl(PR_SET_NAME, (gulong)TASK_NAME_AL_BRD_DETECT);

    (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_UART_CHAN, &uart);

    ret = dal_get_specific_object_byte(CLASS_BUS_MODBUS, PROPERTY_MODBUS_UART, uart, &obj_modbus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "No Modbus object found with chan %d", uart);
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_asset_mgmt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: RackAssetMgmt obj not found", __FUNCTION__);
        return;
    }

    
    (void)set_rack_asset_mgmt_svc_status(obj_asset_mgmt, ASSET_MGMT_SVC_OFF);

    for (;;) {
        
        (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_HW_MB_ADDR_ALLOC_MGMT_PRST, &detected);

        
        if (detected_last == detected) {
            vos_task_delay(ASSET_BRD_PRES_CHK_TIME);
            continue;
        } else if ((detected_last == 1) && (detected == 0)) { 
            debug_log(DLOG_INFO, "%s: Asset board removed", __FUNCTION__);

            detected_last = detected;

            
            (void)memset_s(g_mb_addr_tbl, sizeof(g_mb_addr_tbl), 0, sizeof(g_mb_addr_tbl));
            (void)memset_s(g_need_upgrade, sizeof(g_need_upgrade), 0, sizeof(g_need_upgrade));
            (void)memset_s(g_need_active, sizeof(g_need_active), 0, sizeof(g_need_active));

            
            (void)dal_get_property_value_byte(obj_mb_addr_alloc_mgmt, PROP_MB_ADDR_ALLOC_STATUS, &alloc_status);
            slave_idx = alloc_status & 0x0F;
            update_addr_alloc_status(obj_mb_addr_alloc_mgmt, slave_idx, ALLOC_ADDR_OK);

            
            (void)set_rack_asset_mgmt_svc_status(obj_asset_mgmt, ASSET_MGMT_SVC_OFF);

            
            (void)notify_brd_presence(obj_modbus, 0);

            // 去使能"全部上电"信号，防止热拔出后再次插入时全部同时上电
            ret = asset_locator_pwr_switch(obj_mb_addr_alloc_mgmt, MCU_PWR_OFF);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Set asset board power off failed. ret=%d", __FUNCTION__, ret);
            }

            
            wait_until_obj_load_unload_finish(CLASS_ASSET_LOCATE_BOARD, 0, MAX_WAIT_FOR_XML_COUNT);

            // 刷新资产定位板ID、版本等信息，并重新计算U位总数
            ret = update_asset_locator_info(obj_modbus);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Update rack info failed", __FUNCTION__);
            }

            continue;
        } else { 
            debug_log(DLOG_INFO, "%s: Asset board installed or uninitialized", __FUNCTION__);

            detected_last = detected;

            
            ret = modbus_slave_addr_alloc(obj_mb_addr_alloc_mgmt);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Modbus slave addresss allocate failed", __FUNCTION__);
                continue;
            }

            debug_log(DLOG_INFO, "%s:Modbus slave addr allocate successfully", __FUNCTION__);

            
            brd_count = notify_brd_presence(obj_modbus, 1);
            wait_until_obj_load_unload_finish(CLASS_ASSET_LOCATE_BOARD, brd_count, MAX_WAIT_FOR_XML_COUNT);

            // 升级前先获取资产定位板ID、版本等信息
            ret = update_asset_locator_info(obj_modbus);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Update rack info before upgrade failed", __FUNCTION__);
                continue;
            }

            // 执行MCU固件版本检查和升级，失败也仍然启动
            ret = mcu_check_and_upgrade(obj_modbus);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Check and upgrade mcu failed", __FUNCTION__);
            }

            // 升级成功后重新获取板卡信息
            ret = update_asset_locator_info(obj_modbus);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Update rack info after upgrade failed", __FUNCTION__);
                continue;
            }

            
            (void)set_rack_asset_mgmt_svc_status(obj_asset_mgmt, ASSET_MGMT_SVC_ON);

            continue;
        }

        
    }
}


gint32 asset_brd_add_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("AssetLocateBoard", object_handle, PROPERTY_ASSET_LOCATE_BRD_SLOT, NULL,
        CARD_PLUG_IN);
    return RET_OK;
}


gint32 asset_brd_del_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("AssetLocateBoard", object_handle, PROPERTY_ASSET_LOCATE_BRD_SLOT, NULL,
        CARD_PLUG_OUT);
    return RET_OK;
}

LOCAL gboolean has_physical_asset_locator_connector(void)
{
    OBJ_HANDLE obj_rack_asset_mgmt = OBJ_HANDLE_COMMON;
    guint8 rack_asset_mgmt_type = RACK_ASSET_MGMT_TYPE_MODBUS;

    (void)dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset_mgmt);
    (void)dal_get_property_value_byte(obj_rack_asset_mgmt, PROPERTY_RACK_ASSET_MGMT_TYPE, &rack_asset_mgmt_type);

    if (rack_asset_mgmt_type == RACK_ASSET_MGMT_TYPE_MODBUS) {
        return TRUE;
    } else {
        return FALSE;
    }
}


gint32 asset_locator_init(OBJ_HANDLE obj_modbus)
{
    gulong al_brd_detect_task_id = 0;
    const gchar *modbus_name = NULL;
    guint8 uart = 0;
    OBJ_HANDLE obj_mb_addr_alloc_mgmt = OBJ_HANDLE_COMMON;
    gboolean has_phy_al = TRUE;

    // 如果不是使用物理资产定位条的方式进行资产管理，则提前返回
    has_phy_al = has_physical_asset_locator_connector();
    if (has_phy_al == FALSE) {
        return RET_OK;
    }

    modbus_name = dfl_get_object_name(obj_modbus);

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);

    
    if (dal_get_specific_object_byte(CLASS_HW_MB_ADDR_ALLOC_MGMT, PROP_MB_ADDR_ALLOC_UART_CHAN, uart,
        &obj_mb_addr_alloc_mgmt) == RET_OK) {
        
        if (vos_task_create(&al_brd_detect_task_id, TASK_NAME_AL_BRD_DETECT, (TASK_ENTRY)al_brd_detect_task,
            (void *)obj_mb_addr_alloc_mgmt, DEFAULT_PRIORITY) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: create %s task for %s failed", __FUNCTION__, TASK_NAME_AL_BRD_DETECT,
                modbus_name);
            return RET_ERR;
        }

        debug_log(DLOG_INFO, "%s: create %s for task %s successfully", __FUNCTION__, TASK_NAME_AL_BRD_DETECT,
            modbus_name);
    }

    return RET_OK;
}


gint32 asset_locator_start(OBJ_HANDLE obj_modbus)
{
    gint32 ret = RET_OK;
    
    gulong al_brd_status_sync_task_id = 0;
    
    const gchar *modbus_name = NULL;
    guint8 uart_chan = 0;
    gboolean has_phy_al = TRUE;

    // 如果不是使用物理资产定位条的方式进行资产管理，则提前返回
    has_phy_al = has_physical_asset_locator_connector();
    if (has_phy_al == FALSE) {
        return RET_OK;
    }

    modbus_name = dfl_get_object_name(obj_modbus);
    ret = dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_ID, &uart_chan);
    if ((ret == RET_OK) && (uart_chan == UART4)) {
        
        if (vos_task_create(&al_brd_status_sync_task_id, TASK_NAME_AL_BRD_STAT_SYNC,
            (TASK_ENTRY)al_brd_status_sync_task, (void *)obj_modbus, DEFAULT_PRIORITY) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: create %s task for %s failed", __FUNCTION__, TASK_NAME_AL_BRD_STAT_SYNC,
                modbus_name);
            return RET_ERR;
        }
        

        debug_log(DLOG_INFO, "%s: create %s task for %s successfully", __FUNCTION__, TASK_NAME_AL_BRD_STAT_SYNC,
            modbus_name);
    }

    return RET_OK;
}


gint32 asset_locator_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *boot_ver = NULL;
    const gchar *firm_ver = NULL;
    const gchar *brd_name = NULL;
    const gchar *manu = NULL;
    guint8 u_count = 0;
    gchar           file_name[FILE_NAME_LEN] = {0};
    gchar           content[FILE_CONTENT_BUF_LEN] = {0};
    FILE *fp = NULL;
    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_ASSET_LOCATE_BOARD, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, sizeof(file_name), sizeof(file_name) - 1, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    (void)snprintf_s(content, sizeof(content), sizeof(content) - 1, "%s", "Asset Locator Info\n");
    fwrite_back = fwrite(content, strlen(content), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(content, sizeof(content), 0, sizeof(content));
    (void)snprintf_s(content, sizeof(content), sizeof(content) - 1,
        "%-4s | %-6s | %-5s | %-12s | %-9s | %-13s | %-7s | %s\n", "Slot", "BoardId", "Name", "Manufacturer",
        "BoardName", "BootloaderVer", "FirmVer", "UCount");
    fwrite_back = fwrite(content, strlen(content), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_ID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_PRODUCT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_MANU);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_BOOT_VER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_FIRM_VER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_ASSET_LOCATE_BRD_U_COUNT);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get asset locator information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }

        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manu = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        brd_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        boot_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);
        firm_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 6), 0);
        u_count = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 7));

        (void)memset_s(content, sizeof(content), 0, sizeof(content));

        (void)snprintf_s(content, sizeof(content), sizeof(content) - 1,
            "%-4u | 0x%-5x | %-5s | %-12s | %-9s | %-13s | %-7s | %u\n", slot_id, board_id, name, manu, brd_name,
            boot_ver, firm_ver, u_count);

        fwrite_back = fwrite(content, strlen(content), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(content, sizeof(content), 0, sizeof(content));
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(content, sizeof(content), sizeof(content) - 1, "%s", "\n\n");
    fwrite_back = fwrite(content, strlen(content), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(content, sizeof(content), 0, sizeof(content));

    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


gint32 upgrade_al_board(GSList *input_list)
{
    guint8 brd_number;
    gint32 ret;
    OBJ_HANDLE modbus_handle = 0;
    errno_t p_ret = EOK;
    const gchar *path_str = NULL;

    if (input_list == NULL) {
        debug_printf("%s failed: input param error", __FUNCTION__);
        return RET_ERR;
    }

    brd_number = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    ret = dal_get_specific_object_byte(CLASS_BUS_MODBUS, PROPERTY_MODBUS_UART, UART4, &modbus_handle);
    if (ret != RET_OK) {
        debug_printf("Command is not supported in current state.\n");
        return RET_ERR;
    }

    if (brd_number > MAX_SLAVE_NUM) {
        debug_printf("Invalid asset locator board slot: %d\r\n", brd_number);

        return RET_ERR;
    }

    
    g_debug_upgrade_al_brd_slot = brd_number;

    path_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);

    p_ret = memmove_s(g_debug_upgrade_file_path, sizeof(g_debug_upgrade_file_path), path_str, strlen(path_str));
    if (p_ret != EOK) {
        debug_printf("Copy upgrade file path failed.");

        return RET_ERR;
    }

    
    debug_printf("Upgrading asset locator board %d...", brd_number);
    ret = mcu_check_and_upgrade(modbus_handle);
    
    g_debug_upgrade_al_brd_slot = UPGRADE_AL_BRD_ALL;
    (void)memset_s(g_debug_upgrade_file_path, sizeof(g_debug_upgrade_file_path), 0, sizeof(g_debug_upgrade_file_path));

    if (ret != RET_OK) {
        debug_printf("Upgrade asset locator board %d in debug mode failed.\n", brd_number);
        return RET_ERR;
    }

    (void)update_asset_locator_info(modbus_handle);

    debug_printf("Upgrade asset locator board %d in debug mode successfully.\n", brd_number);
    

    return RET_OK;
}
