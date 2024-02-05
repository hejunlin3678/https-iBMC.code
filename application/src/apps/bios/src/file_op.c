

#include <malloc.h>
#include "file_op.h"
#include "smbios.h"
#include "boot_options.h"
#include "bios_cert.h"
#include "bios.h"


#define BIOS_MAXDATA_LEN (248 - 12)

#define USERBUFSIZE 100 // 定义用户提供的buf大小不小于100
#define SMBIOS_TYPE_MEM 0x11
#define SMBIOS_TYPE_CPU 0x04

#define UPLOAD_FW_HPM_NAME "/tmp/image.hpm"
#define BIOS_SYNC_DATA 0x01
#define BIOS_SET_SUCCESS_EVT_CODE "21700BE0"

LOCAL TASKID ras_hotplug_tid = 0;

LOCAL gint32 write_file_finish(const void *request_msg, guint8 *resp_data, guint32 datelen);
LOCAL gint32 write_file_data(const void *request_msg, guint8 *resp_data, guint32 resp_data_size);
LOCAL gint32 write_file_prepare(const void *request_msg, guint8 *resp_data, guint32 resp_data_size);
LOCAL void smbios_mergeProcess(void);
LOCAL gint32 smbios_mergeSmBiosFile(void);
LOCAL gint32 smbios_addNewSmBiosBlock(MERGE_SMBIOS_T *mergeHandle, RAS_SMBIOS_MES_T *RasSmbiosHandle);
LOCAL gint32 smbios_writeMergeToFile(MERGE_SMBIOS_T *mergeHandle);
LOCAL void destoryMergeStruct(MERGE_SMBIOS_T *mergeHandle);
LOCAL gint32 smbios_getRasHandle(RAS_SMBIOS_MES_T **ras_smbios_handle);
LOCAL gint32 process_clp_config_file_from_bt(const void *ptr, size_t size);
LOCAL gint32 bios_result_postprocess(void);
LOCAL guint8 judge_write_file_selector_valid(guint8 file_index);
LOCAL gint32 write_firmware_data_to_bmc(const void *req_msg, const guint8 *src_data, guint32 data_len);
LOCAL gint32 smbios_openRasSmBios(const gchar *pathName, RAS_SMBIOS_MES_T **pHandle);
LOCAL void bios_write_setting_log(json_object *result_jso);
LOCAL gint32 respond_simple_bios_msg(const void *req_msg, guint8 resp_code);
LOCAL void smbios_closeRasSmBios(RAS_SMBIOS_MES_T *RasSmbiosHandle);
LOCAL gint32 initRasBiosToHash(pRAS_HASH_T hashTable, void *rasBiosBuf, guint32 fileLength);
LOCAL gint32 createRasHash(pRAS_HASH_T *hashTable);
LOCAL gint32 freadFile(const gchar *pathName, const gchar *filename, gchar **buf);
LOCAL void destoryRasHash(RAS_HASH_T *hashTable);
LOCAL gint32 insertRasHashNode(pRAS_HASH_T hashTable, void *pData, guint32 fileLength, guint32 *insertLength);
LOCAL gint32 getBiosSilk(void *pData, gchar **pDes, guint32 cur_len, guint32 max_len);
LOCAL guint32 getBiosBlockLength(void *pData, guint32 fileLength);
LOCAL gint32 read_file_prepare(const void *request_msg, guint8 *resp_data, guint32 resp_data_size);
LOCAL gint32 read_file_finish(const void *request_msg, guint8 *resp_data, guint32 resp_data_len);
LOCAL gint32 read_file_data(const void *request_msg, guint8 *resp_data, guint32 resp_data_len);
LOCAL guint8 judge_read_file_selector_valid(guint8 file_index);
LOCAL gint32 clear_bios_setup_sync_data(void);
LOCAL void set_resp_data(guint8 *resp_data, BIOS_DATA_OPERATE_S *data_operate);

LOCAL GMutex g_bios_rd_data_cfg_mutex;
LOCAL GMutex g_file_op_comm_mutex;
 
void init_bios_rd_data_cfg_mutex(void)
{
    g_mutex_init(&g_bios_rd_data_cfg_mutex);
}

void init_file_op_comm_mutex(void)
{
    g_mutex_init(&g_file_op_comm_mutex);
}

LOCAL BIOS_DATA_OPERATE_S* get_buf_from_selector(guint8 file_selector)
{
    if (file_selector == BIOS_FILE_FIRMWARE) {
        BIOS_DATA_OPERATE_S *data_operate = &g_upg_fw_data_cfg;
        return data_operate;
    } else if (file_selector == BIOS_FILE_CONFIGVALUE_NUM ||
        file_selector == BIOS_FILE_POLICYCONFIGREGISTRY_NUM) {
        BIOS_DATA_OPERATE_S *data_operate = &g_bios_imu_wr_data_cfg;
        return data_operate;
    } else {
        BIOS_DATA_OPERATE_S *data_operate = &g_bios_wr_data_cfg;
        return data_operate;
    }
}


gint32 bios_write_file_to_bmc(const void *req_msg, gpointer user_data)
{
    guint8 resp_data[10];

    
    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    

    
    gint32 ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    
    guint8 bios_id = src_data[BIOS_ID_OFFSET];

    if (bios_id > BIOS_ID) {
        debug_log(DLOG_ERROR, "BiosId:%d(MaxId:%d) is invalid!", bios_id, BIOS_ID);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    guint8 sub_cmd = src_data[BIOS_SUB_CMD_OFFSET];

    g_mutex_lock(&g_file_op_comm_mutex);
    switch (sub_cmd) {
        case BIOS_OPERATE_PREPARE:

            // 1)write prepare
            ret_val = write_file_prepare(req_msg, resp_data, sizeof(resp_data));
            break;

        case BIOS_OPERATE_DATA:

            // 2)write data
            ret_val = write_file_data(req_msg, resp_data, sizeof(resp_data));
            break;

        case BIOS_OPERATE_FINISH:

            // 3)write finish
            ret_val = write_file_finish(req_msg, resp_data, sizeof(resp_data));
            break;

        default:
            resp_data[0] = BIOS_ERR_INVALID_STATUS;
            ret_val = construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }
    g_mutex_unlock(&g_file_op_comm_mutex);
    return ret_val;
}

LOCAL guint8 fm_write_file_finish(const gchar *file_property_name, BIOS_DATA_OPERATE_S *data_operate)
{
    _cleanup_fm_fclose_ FM_FILE_S *file_handle = fm_fopen(file_property_name, "w+b");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "open %s file failed", file_property_name);
        return SMBIOS_ERR_FM_FAIL;
    }

    gsize data_size = fm_fwrite(data_operate->data_buf, BIOS_FILE_STREAM_SIZE, data_operate->data_offset, file_handle);
    if (data_operate->data_offset != data_size) {
        debug_log(DLOG_ERROR, "write file(fileName:%s,InLen:%d,OutLen:%zu) failed",
            file_property_name, data_operate->data_offset, data_size);
        return SMBIOS_ERR_FM_FAIL;
    }

    return BIOS_ERR_NORMALLY;
}


LOCAL gint32 write_file_finish(const void *request_msg, guint8 *resp_data, guint32 datelen)
{
    BIOS_DATA_OPERATE_S *data_operate = { 0 };
    guint8 file_selector = 0;

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        goto EXIT;
    }

    file_selector = src_data[BIOS_FILE_SELECT_OFFSET];
    data_operate = get_buf_from_selector(file_selector);
    // 固件升级特殊处理
    if (file_selector == BIOS_FILE_FIRMWARE) {
        // 0x30 0x92 data3=0x09的ipmi命令最短长度是8 能走到这里说明框架已经校验了长度是ok的
        return write_firmware_data_to_bmc(request_msg, src_data, 8);
    }

    
    guint16 check_sum = vos_verify_check_sum(data_operate->data_buf, data_operate->data_offset);
    if ((guint8)check_sum != src_data[BIOS_CHECK_NUM_OFFSET]) {
        debug_log(DLOG_ERROR, "CheckSum(CalcSum:%d,Calclen:%d,RxSum:%d) is error!", check_sum,
            data_operate->data_offset, src_data[BIOS_CHECK_NUM_OFFSET]);
        resp_data[0] = BIOS_ERR_INVALID_CHKSUM;
        goto EXIT;
    }

    
    // 2.1.获取通过对象属性获取文件名，但文件名是固定的. 或者
    // 2.2。指定目录，动态创建文件名，动态注册文件管理
    if (judge_write_file_selector_valid(file_selector) == FALSE) {
        debug_log(DLOG_ERROR, "judge_write_file_selector_valid file_selector:%d fail.", file_selector);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        goto EXIT;
    }

    
    if (file_selector == BIOS_FILE_SETUP_NUM) {
        
        resp_data[0] = BIOS_ERR_NORMALLY;
        construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);

        
        OBJ_HANDLE obj_handle = 0;
        if (dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle) != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: get class: %s obj failed.", __FUNCTION__, BIOS_CLASS_NAME);
            return RET_ERR;
        }

        
        if (dal_set_property_value_array_byte(obj_handle, PROPERTY_BIOS_MENU_DATA, data_operate->data_buf,
            data_operate->data_offset, DF_NONE) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set property: %s value failed.", __FUNCTION__, PROPERTY_BIOS_MENU_DATA);
            return RET_ERR;
        }

        guint8 change_flag = 0;
        if (dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_MENU_CHANGE_FLAG, &change_flag) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get property: %s value failed.", __FUNCTION__, PROPERTY_BIOS_MENU_CHANGE_FLAG);
            clear_data_operate_res(data_operate);
            return RET_ERR;
        }

        if (dal_set_property_value_byte(obj_handle, PROPERTY_BIOS_MENU_CHANGE_FLAG, !change_flag, DF_SAVE) != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set property: %s value failed.", __FUNCTION__, PROPERTY_BIOS_MENU_CHANGE_FLAG);
            clear_data_operate_res(data_operate);
            return RET_ERR;
        }

        clear_data_operate_res(data_operate);
        return RET_OK;
    } else if (file_selector == BIOS_FILE_CLP) {
        if (process_clp_config_file_from_bt(data_operate->data_buf, data_operate->data_offset) != RET_OK) {
            resp_data[0] = SMBIOS_ERR_FM_FAIL;
            goto EXIT;
        }
    } else if (file_selector == BIOS_FILE_CURRENT_SECUREBOOT_NUM) {
        fm_clear_file(SECUREBOOT_CURRENT_SHORT_NAME); // 每次启动Bios都会将完整的信息写入该文件，因此写入前先清空
        resp_data[0] = fm_write_file_finish(SECUREBOOT_CURRENT_SHORT_NAME, data_operate);
        fm_clear_file(SECUREBOOT_NEW_SHORT_NAME); // 直接清空旧的待生效配置
        goto EXIT;
    } else {
        
        if (file_selector >= (sizeof(g_file_property_name) / sizeof(g_file_property_name[0]))) {
            debug_log(DLOG_ERROR, "%s: file_selector(%u) is out of range", __FUNCTION__, file_selector);
            resp_data[0] = SMBIOS_ERR_FM_FAIL;
            goto EXIT;
        }
        resp_data[0] = fm_write_file_finish(g_file_property_name[file_selector], data_operate);
        if (resp_data[0] != BIOS_ERR_NORMALLY) {
            goto EXIT;
        }

        // 设置文件状态  UNREPORTED:未上报给HMM板  REPORTED:已上报给HMM板
        (void)set_status_by_resource_id(file_selector, UNREPORTED);
        debug_log(DLOG_DEBUG, "BIOS set_status_by_resource_id : resource_id = 0x%x , status = UNREPORTED",
            file_selector);
        method_operation_log(ipmimsg_to_callerinfo(request_msg), NULL,
            "Bios write file[resource_id:%x] finish successfully", file_selector);
    }

    
    resp_data[0] = BIOS_ERR_NORMALLY;

    if (file_selector == BIOS_FILE_SMBIOS_RAS) {
        (void)vos_task_create(&ras_hotplug_tid, "ras_hotplug_monitor", (TASK_ENTRY)smbios_mergeProcess, (void *)NULL,
            DEFAULT_PRIORITY);
    }

    
    if (file_selector == BIOS_FILE_RESULT_NUM) {
        (void)bios_result_postprocess();
    }

    
    if (file_selector == BIOS_FILE_REGISTRY_NUM) {
        (void)bios_registry_version_update(FALSE);
    }

    
    if (file_selector == BIOS_FILE_CURRENT_VALUE_NUM &&
        g_bios_file_change[0].file_change_flag == BIOS_SETTING_FILE_UNCHANGED) {
        g_bios_setting_file_state = BIOS_SETTING_FILE_EFFECTIVE;
    }

EXIT:
    
    clear_data_operate_res(data_operate);

    gint32 ret_val = construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);

    
    if (file_selector == BIOS_FILE_CURRENT_VALUE_NUM) {
        (void)bios_dynamic_config_sync();
    }

    debug_log(DLOG_MASS, "%s: call malloc_trim result :%d", __FUNCTION__, malloc_trim(0));
    return ret_val;
}


LOCAL void set_resp_data(guint8 *resp_data, BIOS_DATA_OPERATE_S *data_operate)
{
    resp_data[0] = BIOS_ERR_WRONG_OFFSET;
    resp_data[4] = LONGB0(data_operate->data_offset);
    resp_data[5] = LONGB1(data_operate->data_offset);
    resp_data[6] = LONGB2(data_operate->data_offset);
    resp_data[7] = LONGB3(data_operate->data_offset);
}


LOCAL gint32 write_file_data(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    if (request_msg == NULL || resp_data == NULL || resp_data_size < BIOS_MSG_HEAD_MIN_LEN) {
        debug_log(DLOG_ERROR, "input param invalid!");
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "write_file_data src_data is NULL!");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    BIOS_DATA_OPERATE_S *data_operate = get_buf_from_selector(src_data[BIOS_FILE_SELECT_OFFSET]);
    
    if (data_operate->data_flag != BIOS_FLAG_DOING) {
        debug_log(DLOG_ERROR, "write_file_data: DataFlag:%u(ShouldBe:%u) is invalid!", data_operate->data_flag,
            BIOS_FLAG_DOING);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    guint32 data_real_len = get_ipmi_src_data_len(request_msg) - BIOS_DATA_OFFSET_POS;
    
    if ((data_real_len > BIOS_MAXDATA_LEN) || (data_real_len == 0) ||
        ((data_operate->data_offset + data_real_len) > data_operate->data_len)) {
        debug_log(DLOG_ERROR, "DataRealLen:%u(Max:%u,DataOffset:%u,DataLen:%u) is invalid!", data_real_len,
            BIOS_MAXDATA_LEN, data_operate->data_offset, data_operate->data_len);

        resp_data[0] = SMBIOS_ERR_IV_LEN;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    guint32 bios_offset = MAKE_DWORD(src_data[BIOS_FILE_OFFSET_START + 3], src_data[BIOS_FILE_OFFSET_START + 2],
        src_data[BIOS_FILE_OFFSET_START + 1], src_data[BIOS_FILE_OFFSET_START]);
    if (data_operate->data_offset != bios_offset) {
        debug_log(DLOG_ERROR, "DataOffset:%u(ShouldBe:%u) is invalid!", bios_offset, data_operate->data_offset);

        set_resp_data(resp_data, data_operate);
        return construt_and_respond_msg(request_msg, resp_data, BIOS_ERR_OFFSET_MSG_LEN);
    }

    

    
    guint16 check_sum = vos_verify_check_sum((const guint8 *)&src_data[BIOS_DATA_OFFSET_POS], data_real_len);
    if ((guint8)check_sum != src_data[BIOS_CHECK_NUM_OFFSET]) {
        debug_log(DLOG_ERROR, "CheckSum(CalcSum:%u,RxSum:%u) is error!", check_sum, src_data[BIOS_CHECK_NUM_OFFSET]);

        resp_data[0] = BIOS_ERR_INVALID_CHKSUM;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    debug_log(DLOG_DEBUG, "Offset:%u,DataRealLen:%u!", data_operate->data_offset, data_real_len);

    if ((data_operate->data_offset + data_real_len) > data_operate->data_len) {
        debug_log(DLOG_ERROR, "write_file_data fail, DataLen:%u greater than BufLen:%u!",
            data_operate->data_offset + data_real_len, data_operate->data_len);
        resp_data[0] = SMBIOS_ERR_IV_LEN;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    if (src_data[BIOS_FILE_SELECT_OFFSET] == BIOS_FILE_FIRMWARE) {
        // 固件升级场景 调用写升级包函数执行 ipmi返回由该函数进行
        return write_firmware_data_to_bmc(request_msg, src_data, (data_real_len + BIOS_DATA_OFFSET_POS));
    }

    
    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, "Databuffer is NullPointer!");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }
    for (gint32 i = 0; i < data_real_len; i++) {
        *(data_operate->data_buf + data_operate->data_offset + i) = src_data[BIOS_DATA_OFFSET_POS + i];
    }

    data_operate->data_offset += data_real_len;

    resp_data[0] = BIOS_ERR_NORMALLY;
    return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}


LOCAL gint32 write_file_prepare(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    guint32 bios_file_len = 0;

    if (request_msg == NULL || resp_data == NULL || resp_data_size < BIOS_MSG_HEAD_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s: input false parameters", __FUNCTION__);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "write_file_prepare src_data is NULL!");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    guint8 is_bios_firmware = (src_data[BIOS_FILE_SELECT_OFFSET] == BIOS_FILE_FIRMWARE);

    BIOS_DATA_OPERATE_S *data_operate = get_buf_from_selector(src_data[BIOS_FILE_SELECT_OFFSET]);
    gint32 file_size_limit = (is_bios_firmware) ? (MAX_FILE_SIZE_90M) : (BIOS_FILE_MAX_SIZE);

    
    data_operate->data_flag = BIOS_FLAG_DOING;
    data_operate->data_offset = 0;

    
    guint8 src_len = get_ipmi_src_data_len(request_msg);
    if (src_len >= BIOS_DATA_OFFSET_POS) {
        bios_file_len = MAKE_DWORD(src_data[BIOS_FILE_OFFSET_START + DWORD_OFFSET],
            src_data[BIOS_FILE_OFFSET_START + DWORD_OFFSET - 1], src_data[BIOS_FILE_OFFSET_START + 1],
            src_data[BIOS_FILE_OFFSET_START]);
    }

    if (bios_file_len > file_size_limit) {
        debug_log(DLOG_ERROR, "FileLen:%u(Max:%u) is invalid!", bios_file_len, file_size_limit);
        data_operate->data_flag = BIOS_FLAG_IDLE;
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    data_operate->data_len = bios_file_len;

    debug_log(DLOG_ERROR, "FileLen:%u(Max:%u)!", bios_file_len, BIOS_FILE_MAX_SIZE);

    
    if (data_operate->data_buf != NULL) {
        g_free(data_operate->data_buf);
        data_operate->data_buf = NULL;
    }

    // 固件升级时数据直接写入文件，prepare过程无需分配内存
    if (src_data[BIOS_FILE_SELECT_OFFSET] == BIOS_FILE_FIRMWARE) {
        return write_firmware_data_to_bmc(request_msg, src_data, 0);
    }

    resp_data[0] = BIOS_ERR_NORMALLY;

    data_operate->data_buf = (guint8 *)g_malloc0(bios_file_len + 1);

    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:%u fail!", bios_file_len + 1);

        data_operate->data_flag = BIOS_FLAG_IDLE;
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
    }

    method_operation_log(ipmimsg_to_callerinfo(request_msg), NULL, "Bios write file prepare successfully");
    return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
}

LOCAL void smbios_mergeProcess(void)
{
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;

    vos_task_delay(5000);

    debug_log(DLOG_DEBUG, "[%s]start merge hot plug smbios !\n", __FUNCTION__);

    gint32 ret = smbios_mergeSmBiosFile();

    debug_log(DLOG_DEBUG, "[%s]finish merge hot plug smbios, merge result is %d!\n", __FUNCTION__, ret);

    ret = dfl_get_binded_object(SMBIOS_CLASS_NAME, BIOS_OBJECT_SMBIOS_NAME, &obj_handle);
    if (ret != DFL_OK) {
        
        debug_log(DLOG_ERROR, "[%s] get obj %s failed!", __FUNCTION__, SMBIOS_CLASS_NAME);
        return;
    }

    property_value = g_variant_new_byte(0);
    ret = dfl_set_property_value(obj_handle, SMBIOS_CHANGE_FLAG, property_value, DF_NONE);
    if (ret != DFL_OK) {
        
        debug_log(DLOG_ERROR, "[%s] setprop %s [0] failed!", __FUNCTION__, SMBIOS_CHANGE_FLAG);
        g_variant_unref(property_value);
        return;
    }

    g_variant_unref(property_value);

    property_value = g_variant_new_byte(1);
    ret = dfl_set_property_value(obj_handle, SMBIOS_CHANGE_FLAG, property_value, DF_NONE);
    if (ret != DFL_OK) {
        
        debug_log(DLOG_ERROR, "[%s] setprop %s [1] failed!", __FUNCTION__, SMBIOS_CHANGE_FLAG);
        g_variant_unref(property_value);
        return;
    }

    g_variant_unref(property_value);

    debug_log(DLOG_DEBUG, "[%s]refresh smbios success!\n", __FUNCTION__);

    return;
}

LOCAL gint32 smbios_mergeSmBiosFile(void)
{
    // BIOS配置文件上限 10M  10*1024*1024
    const guint32 SMBIOS_FILE_MAX_SIZE = 10485760;
    guint32 len = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    const gchar *path_name = NULL;
    MERGE_SMBIOS_T *mergeHandle = NULL;
    RAS_SMBIOS_MES_T *RasSmbiosHandle = NULL;

    
    mergeHandle = (MERGE_SMBIOS_T *)g_malloc0(sizeof(MERGE_SMBIOS_T));
    if (mergeHandle == NULL) {
        debug_log(DLOG_ERROR, " get mergeHandle failed ! ");
        return RET_ERR;
    }

    
    gint32 ret = dfl_get_object_handle(SMBIOS_OBJECT_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, " get %s handle failed !", SMBIOS_OBJECT_NAME);
        destoryMergeStruct(mergeHandle);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, " get %s handle!", SMBIOS_OBJECT_NAME);

    
    ret = dfl_get_property_value(obj_handle, SMBIOS_FILE_NAME, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, " get %s property failed !", SMBIOS_FILE_NAME);
        destoryMergeStruct(mergeHandle);
        return RET_ERR;
    }

    path_name = g_variant_get_string(property_value, NULL);

    debug_log(DLOG_DEBUG, "[%s] SMBIOS path!", __FUNCTION__);

    
    len = vos_get_file_length(path_name);
    if (len == 0) {
        debug_log(DLOG_ERROR, "[%s]smbios file length is %d ! ", __FUNCTION__, len);
        g_variant_unref(property_value);
        destoryMergeStruct(mergeHandle);
        return RET_ERR;
    }

    mergeHandle->oldBufLen = len;
    

    ret = freadFile(path_name, SMBIOS_FILE_NAME, &(mergeHandle->oldBuf));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]read file failed!", __FUNCTION__);
        g_variant_unref(property_value);
        destoryMergeStruct(mergeHandle);
        return RET_ERR;
    }

    g_variant_unref(property_value);

    ret = smbios_getRasHandle(&RasSmbiosHandle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]get smbios handle failed!", __FUNCTION__);
        destoryMergeStruct(mergeHandle);
        return RET_ERR;
    }

    mergeHandle->newBufLen = mergeHandle->oldBufLen + RasSmbiosHandle->fileLength;

    debug_log(DLOG_DEBUG, "[%s]SMBIOS LEN:%d RAS LEN:%d MERGE LEN:%d\n", __FUNCTION__, mergeHandle->oldBufLen,
        RasSmbiosHandle->fileLength, mergeHandle->newBufLen);

    // ICSL送检，对malloc值做校验
    if (mergeHandle->newBufLen > SMBIOS_FILE_MAX_SIZE) {
        debug_log(DLOG_ERROR, "[%s]mergeHandle->newBuf exceed the limit of max_file_size! Current size is %d",
            __FUNCTION__, mergeHandle->newBufLen);
        destoryMergeStruct(mergeHandle);
        smbios_closeRasSmBios(RasSmbiosHandle);
        return RET_ERR;
    }
    
    mergeHandle->newBuf = (gchar *)g_malloc0(mergeHandle->newBufLen);

    if (mergeHandle->newBuf == NULL) {
        debug_log(DLOG_ERROR, "[%s]malloc mergeHandle->newBuf failed!", __FUNCTION__);
        destoryMergeStruct(mergeHandle);
        smbios_closeRasSmBios(RasSmbiosHandle);
        return RET_ERR;
    }

    mergeHandle->newFileLen = mergeHandle->newBufLen;

    ret = smbios_addNewSmBiosBlock(mergeHandle, RasSmbiosHandle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]merge SMBIOS file failed!", __FUNCTION__);
        ret = RET_ERR;
    } else {
        debug_log(DLOG_DEBUG, "[%s]merge SMBIOS file success, write to file!", __FUNCTION__);
        ret = smbios_writeMergeToFile(mergeHandle);

        debug_log(DLOG_DEBUG, "[%s]result of write to file action is %d!", __FUNCTION__, ret);
    }

    destoryMergeStruct(mergeHandle);
    smbios_closeRasSmBios(RasSmbiosHandle);

    return ret;
}

LOCAL gint32 smbios_addNewSmBiosBlock(MERGE_SMBIOS_T *mergeHandle, RAS_SMBIOS_MES_T *RasSmbiosHandle)
{
    guint32 curOldPos = 0;
    guint32 curNewPos = 0;
    guint32 oldFruLen = 0;
    RAS_NODE_T *pTemp = NULL;
    RAS_NODE_T *pParent = NULL;
    guchar type = 0;
    guint32 len = 0;
    guint8 current_insert = 0;
    gchar *curSilk = NULL;
    gint32 ret = RET_OK;
    errno_t safe_fun_ret = EOK;

    if ((mergeHandle == NULL) || (RasSmbiosHandle == NULL)) {
        debug_log(DLOG_ERROR, "[%s]mergeHandle or RasSmbiosHandle is NULL", __FUNCTION__);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "[%s]start to merge the SMBIOS file, totally %d frus\n", __FUNCTION__,
        RasSmbiosHandle->pHash->nodeCount);

    debug_log(DLOG_DEBUG, "[%s]oldBufLen:%d newBufLen:%d newFileLen:%d\n", __FUNCTION__, mergeHandle->oldBufLen,
        mergeHandle->newBufLen, mergeHandle->newFileLen);

    
    
    while ((RasSmbiosHandle->pHash->nodeCount > 0) && (curOldPos < mergeHandle->oldBufLen)) {
        type = *((gchar *)mergeHandle->oldBuf + curOldPos);
        if (type == END_OF_TABLE_TYPE) {
            debug_log(DLOG_INFO, "[%s]scan to type %d, curOldPos:%d, break\n", __FUNCTION__, type, curOldPos);
            break;
        }

        debug_log(DLOG_DEBUG, "[%s]scan to type %d, curOldPos:%d\n", __FUNCTION__, type, curOldPos);

        current_insert = 0;

        if (type == SMBIOS_TYPE_MEM || type == SMBIOS_TYPE_CPU) {
            pTemp = RasSmbiosHandle->pHash->ppRasContainer[type];
            pParent = RasSmbiosHandle->pHash->ppRasContainer[type];
            

            ret = getBiosSilk((mergeHandle->oldBuf + curOldPos), &curSilk, curOldPos, mergeHandle->oldBufLen);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "[%s]get fru silk failed, curOldPos is %d", __FUNCTION__, curOldPos);
                return RET_ERR;
            }

            debug_log(DLOG_DEBUG, "[%s]scan fru [%s]\n", __FUNCTION__, curSilk);

            while (pTemp) {
                
                if (!strcmp(curSilk, pTemp->description)) {
                    debug_log(DLOG_INFO, "[%s] find fru %s to refresh!", __FUNCTION__, curSilk);

                    if ((curNewPos + pTemp->length) > mergeHandle->newBufLen) {
                        debug_log(DLOG_ERROR, "[%s]curNewPos:%d  block_len:%d  newBufLen:%d , invalid", __FUNCTION__,
                            curNewPos, pTemp->length, mergeHandle->newBufLen);
                        g_free(curSilk);
                        return RET_ERR;
                    }

                    safe_fun_ret =
                        memcpy_s((mergeHandle->newBuf + curNewPos), (pTemp->length), (pTemp->pData), (pTemp->length));
                    if (safe_fun_ret != EOK) {
                        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                    }
                    
                    curNewPos += pTemp->length;
                    oldFruLen =
                        getBiosBlockLength((mergeHandle->oldBuf + curOldPos), (mergeHandle->oldBufLen - curOldPos));
                    if (oldFruLen == 0) {
                        debug_log(DLOG_ERROR, "[%s]old fru [%s] len is %d, invalid", __FUNCTION__, curSilk, oldFruLen);
                        g_free(curSilk);
                        return RET_ERR;
                    }

                    curOldPos += oldFruLen;

                    debug_log(DLOG_DEBUG, "[%s] oldFruLen:%d  newFruLen:%d !", __FUNCTION__, oldFruLen, pTemp->length);

                    if (curNewPos + 2 > mergeHandle->newBufLen) {
                        debug_log(DLOG_ERROR, "[%s]curNewPos:%d plus 2 more than newBufLen:%d", __FUNCTION__, curNewPos,
                            mergeHandle->newBufLen);
                        g_free(curSilk);
                        return RET_ERR;
                    }

                    
                    *((guchar *)(mergeHandle->newBuf + curNewPos)) = 0;
                    *((guchar *)(mergeHandle->newBuf + curNewPos + 1)) = 0;
                    curNewPos += 2;
                    curOldPos += 2;

                    
                    
                    if (pTemp == RasSmbiosHandle->pHash->ppRasContainer[type]) {
                        RasSmbiosHandle->pHash->ppRasContainer[type] = pTemp->pNext;
                    } else {
                        pParent->pNext = pTemp->pNext;
                    }

                    g_free(pTemp->description);
                    g_free(pTemp);
                    pTemp = NULL;
                    (RasSmbiosHandle->pHash->nodeCount)--;
                    current_insert = 1;

                    debug_log(DLOG_DEBUG, "[%s] new fru count:%d !", __FUNCTION__, RasSmbiosHandle->pHash->nodeCount);
                    break;
                } else {
                    pParent = pTemp;
                    pTemp = pTemp->pNext;
                }
            }

            g_free(curSilk);
        }

        if (!current_insert) {
            len = getBiosBlockLength((mergeHandle->oldBuf + curOldPos), (mergeHandle->oldBufLen - curOldPos));
            if (curNewPos + len > mergeHandle->newBufLen) {
                debug_log(DLOG_ERROR, "[%s]curNewPos:%d  block_len:%d  newBufLen:%d , invalid", __FUNCTION__, curNewPos,
                    len, mergeHandle->newBufLen);
                return RET_ERR;
            }

            safe_fun_ret = memcpy_s((mergeHandle->newBuf + curNewPos), len, (mergeHandle->oldBuf + curOldPos), len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            }
            
            curNewPos += len;
            curOldPos += len;
            

            if (curNewPos + 2 > mergeHandle->newBufLen) {
                debug_log(DLOG_ERROR, "[%s]curNewPos:%d plus 2 more than newBufLen:%d", __FUNCTION__, curNewPos,
                    mergeHandle->newBufLen);
                return RET_ERR;
            }

            *((guchar *)(mergeHandle->newBuf + curNewPos)) = 0;
            *((guchar *)(mergeHandle->newBuf + curNewPos + 1)) = 0;
            curNewPos += 2;
            curOldPos += 2;
        }
    }

    debug_log(DLOG_INFO, "[%s] out of loop, curNewPos:%d!", __FUNCTION__, curNewPos);

    

    if (curOldPos < mergeHandle->oldBufLen) {
        debug_log(DLOG_DEBUG, "[%s] oldBufLen:%d curOldPos:%d copy the rest %d bytes to buffer !", __FUNCTION__,
            mergeHandle->oldBufLen, curOldPos, (mergeHandle->oldBufLen - curOldPos));
        debug_log(DLOG_DEBUG, "[%s] newBufLen:%d curNewPos:%d the rest of buf is %d bytes !", __FUNCTION__,
            mergeHandle->newBufLen, curNewPos, (mergeHandle->newBufLen - curNewPos));

        safe_fun_ret = memcpy_s((mergeHandle->newBuf + curNewPos), (mergeHandle->oldBufLen - curOldPos),
            (mergeHandle->oldBuf + curOldPos), (mergeHandle->oldBufLen - curOldPos));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
        curNewPos += (mergeHandle->oldBufLen - curOldPos);

        debug_log(DLOG_DEBUG, "[%s] after copy, curNewPos is %d !", __FUNCTION__, curNewPos);
    }

    mergeHandle->newFileLen = curNewPos;

    debug_log(DLOG_INFO, "[%s] new file len is %d !", __FUNCTION__, mergeHandle->newFileLen);

    return RET_OK;
}

LOCAL gint32 smbios_writeMergeToFile(MERGE_SMBIOS_T *mergeHandle)
{
    guint32 data_size = 0;
    FM_FILE_S *file_handle = NULL;

    file_handle = fm_fopen(SMBIOS_FILE_NAME, "w+b");
    if (file_handle == NULL) {
        
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", SMBIOS_FILE_NAME);
        return RET_ERR;
    }

    
    data_size = fm_fwrite(mergeHandle->newBuf, BIOS_FILE_STREAM_SIZE, mergeHandle->newFileLen, file_handle);
    if (data_size != mergeHandle->newFileLen) {
        
        debug_log(DLOG_ERROR, "write file(InLen:%d,OutLen:%d) fail!\n", mergeHandle->newFileLen, data_size);

        fm_fclose(file_handle);

        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "write file(InLen:%d,OutLen:%d) success!\n", mergeHandle->newFileLen, data_size);

    fm_fclose(file_handle);
    return RET_OK;
}

LOCAL void destoryMergeStruct(MERGE_SMBIOS_T *mergeHandle)
{
    debug_log(DLOG_DEBUG, "free mergeHandle");

    if (mergeHandle == NULL) {
        return;
    }

    if (mergeHandle->oldBuf != NULL) {
        g_free(mergeHandle->oldBuf);
    }

    if (mergeHandle->newBuf != NULL) {
        g_free(mergeHandle->newBuf);
    }

    g_free(mergeHandle);

    return;
}

LOCAL gint32 smbios_getRasHandle(RAS_SMBIOS_MES_T **ras_smbios_handle)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    const gchar *file_name = NULL;

    
    ret = dfl_get_object_handle(SMBIOS_CLASS_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]get smbios handle failed!", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = dfl_get_property_value(obj_handle, SMBIOS_DIFF_FILE_NAME, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]get prop %s failed!", __FUNCTION__, SMBIOS_DIFF_FILE_NAME);
        return RET_ERR;
    }

    file_name = g_variant_get_string(property_value, NULL);

    
    ret = smbios_openRasSmBios(file_name, ras_smbios_handle);
    g_variant_unref(property_value);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "open Ras smbios fail");
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 process_clp_config_file_from_bt(const void *ptr, size_t size)
{
    gint32 ret_val = RET_ERR;
    size_t rt = 0;
    FILE *fp = NULL;

    if (ptr == NULL) {
        return RET_ERR;
    }

    // 打开临时文件，不存在则创建，存在则清空
    fp = g_fopen(TMP_CLP_CONFIG_FILE_FROM_BT, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open %s file fail!\r\n", TMP_CLP_CONFIG_FILE_FROM_BT);
        return RET_ERR;
    }

    (void)fchmod(fileno(fp), 0640);

    
    rt = fwrite(ptr, size, BIOS_FILE_STREAM_SIZE, fp);
    if (rt != BIOS_FILE_STREAM_SIZE) {
        debug_log(DLOG_ERROR, "write %s with size(%zu) fail!\n", TMP_CLP_CONFIG_FILE_FROM_BT, size);
        (void)fclose(fp);
        return RET_ERR;
    }

    
    (void)fclose(fp);

    ret_val = compare_and_copy_clp_config_file_with_lock(TMP_CLP_CONFIG_FILE_FROM_BT);
    if (ret_val == 0) {
        strategy_log(SLOG_INFO, "Save CLP config file from BT Channel successfully.\n");
    } else {
        strategy_log(SLOG_INFO, "Save CLP config file from BT Channel fail.\n");
    }

    return ret_val;
}


LOCAL gint32 bios_result_postprocess(void)
{
    json_object *result_jso = NULL;
    gchar        datetime_buf[MAX_DATETIME_LEN] = {0};

    FM_FILE_S *file_handle = NULL;
    gchar *data_buf = NULL;
    const gchar *data_str = NULL;
    gint32 fm_fseek_back = 1; // 返回成功是0，不成功非0
    gint32 file_size = 0;
    guint32 file_actual_size = 0;

    
    file_handle = fm_fopen(BIOS_FILE_RESULT_NAME, "r+b");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s: open file %s failed", __FUNCTION__, BIOS_FILE_RESULT_NAME);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_END);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, BIOS_FILE_RESULT_NAME);
        goto err_out;
    }

    file_size = fm_ftell(file_handle);
    if (file_size < 0) {
        debug_log(DLOG_ERROR, "%s: file tell %s failed", __FUNCTION__, BIOS_FILE_RESULT_NAME);
        goto err_out;
    }

    
    data_buf = (gchar *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!\n", file_size + 1);
        goto err_out;
    }

    
    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, BIOS_FILE_RESULT_NAME);
        goto err_out;
    }

    file_actual_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, (guint32)file_size, file_handle);
    if (file_size != file_actual_size) {
        debug_log(DLOG_ERROR, "read file(InLen:%d,OutLen:%d) failed", file_size, file_actual_size);
        goto err_out;
    }

    debug_log(DLOG_INFO, "%s: file read OK, %s ", __FUNCTION__, data_buf);

    
    result_jso = json_tokener_parse(data_buf);
    if (result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        goto err_out;
    }

    
    (void)dal_get_redfish_datetime_stamp(0, datetime_buf, MAX_DATETIME_LEN);

    json_object_object_add(result_jso, "Time", json_object_new_string(datetime_buf));

    
    data_str = dal_json_object_to_json_string(result_jso);
    if (data_str == NULL) {
        debug_log(DLOG_ERROR, "%s: dal_json_object_to_json_string failed", __FUNCTION__);
        goto err_out;
    }

    debug_log(DLOG_INFO, "%s: dal_json_object_to_json_string OK, [%s]", __FUNCTION__, data_str);

    fm_fseek_back = fm_fseek(file_handle, 0, SEEK_SET);
    if (fm_fseek_back != 0) {
        debug_log(DLOG_ERROR, "%s: file seek %s failed", __FUNCTION__, BIOS_FILE_RESULT_NAME);
        goto err_out;
    }

    file_actual_size = fm_fwrite(data_str, BIOS_FILE_STREAM_SIZE, strlen(data_str), file_handle);
    if (strlen(data_str) != file_actual_size) {
        debug_log(DLOG_ERROR, "%s: write file(InLen:%zu,OutLen:%d) failed", __FUNCTION__, strlen(data_str),
            file_actual_size);
        goto err_out;
    }

    
    
    bios_write_setting_log(result_jso);
    

    
    fm_fclose(file_handle);
    g_free(data_buf);
    json_object_put(result_jso);

    return RET_OK;

err_out:
    if (file_handle != NULL) {
        fm_fclose(file_handle);
    }
    if (data_buf != NULL) {
        g_free(data_buf);
    }
    if (result_jso != NULL) {
        json_object_put(result_jso);
    }

    return RET_ERR;
}


LOCAL guint8 judge_write_file_selector_valid(guint8 file_index)
{
    switch (file_index) {
        case BIOS_FILE_DISPLAY_NUM:
        case BIOS_FILE_OPTION_NUM:
        case BIOS_FILE_CHANGE_NUM:
        case BIOS_FILE_CLP_RESP:
        case BIOS_FILE_SETUP_NUM:
        case BIOS_FILE_SMBIOS_RAS:

            
        case BIOS_FILE_REGISTRY_NUM:
        case BIOS_FILE_CURRENT_VALUE_NUM:
        case BIOS_FILE_SETTING_NUM:
        case BIOS_FILE_RESULT_NUM:
        case BIOS_FILE_POLICYCONFIGREGISTRY_NUM:
        case BIOS_FILE_CONFIGVALUE_NUM:

            
            
        case BIOS_FILE_CMES_NUM:
            
            
        case BIOS_FILE_CLP:
            
        case BIOS_FILE_CURRENT_SECUREBOOT_NUM:
            return TRUE;

        case BIOS_FILE_DEBUG_INFO:
        case BIOS_FILE_FIRMWARE:
        case BIOS_FILE_NETCONFIG:
        case BIOS_FILE_SOL:
        case BIOS_FILE_BLACKBOX:
            return FALSE;

        default:

            debug_log(DLOG_ERROR, "write file_index:%d is invalid!\n", file_index);
            return FALSE;
    }
}


LOCAL gint32 write_firmware_data_to_bmc(const void *req_msg, const guint8 *src_data, guint32 data_len)
{
    BIOS_DATA_OPERATE_S *data_operate = &g_upg_fw_data_cfg;
    static FILE *fp = NULL;

    if (src_data[BIOS_SUB_CMD_OFFSET] == BIOS_OPERATE_FINISH) { // finish操作，关闭文件
        clear_data_operate_res(data_operate);
        goto finish_or_prepare_exit;
    } else if (src_data[BIOS_SUB_CMD_OFFSET] == BIOS_OPERATE_PREPARE) { // prepare操作，清除旧文件
        (void)vos_rm_filepath(UPLOAD_FW_HPM_NAME);
        goto finish_or_prepare_exit;
    }

    if (data_len <= BIOS_DATA_OFFSET_POS) {
        debug_log(DLOG_ERROR, "write_firmware_data_to_bmc data_len %u", data_len);
        return respond_simple_bios_msg(req_msg, BIOS_ERR_IV_LEN);
    }

    if (fp == NULL) {
        if (data_operate->data_offset == 0) {
            // 第一帧创建文件，其他使用追加
            fp = dal_fopen_check_realpath(UPLOAD_FW_HPM_NAME, "w+", UPLOAD_FW_HPM_NAME);
            if (fp == NULL) {
                debug_log(DLOG_ERROR, "create %s fail", UPLOAD_FW_HPM_NAME);
                return respond_simple_bios_msg(req_msg, BIOS_ERR_FM_FAIL);
            }
            (void)fchmod(fileno(fp), 0600);
        } else {
            fp = dal_fopen_check_realpath(UPLOAD_FW_HPM_NAME, "a+", UPLOAD_FW_HPM_NAME);
            if (fp == NULL) {
                debug_log(DLOG_ERROR, "open %s fail, offset: %d", UPLOAD_FW_HPM_NAME, data_operate->data_offset);
                return respond_simple_bios_msg(req_msg, BIOS_ERR_FM_FAIL);
            }
        }
    }

    if (fwrite(&src_data[BIOS_DATA_OFFSET_POS], data_len - BIOS_DATA_OFFSET_POS, BIOS_FILE_STREAM_SIZE, fp) !=
        BIOS_FILE_STREAM_SIZE) {
        debug_log(DLOG_ERROR, "write data to hpm file failed");
        (void)fclose(fp);
        fp = NULL;
        return respond_simple_bios_msg(req_msg, BIOS_ERR_FM_FAIL);
    }
    data_operate->data_offset += data_len - BIOS_DATA_OFFSET_POS;

    return respond_simple_bios_msg(req_msg, BIOS_ERR_NORMALLY);

finish_or_prepare_exit:
    if (fp != NULL) {
        (void)fclose(fp);
        fp = NULL;
    }
    return respond_simple_bios_msg(req_msg, BIOS_ERR_NORMALLY);
}


LOCAL gint32 smbios_openRasSmBios(const gchar *pathName, RAS_SMBIOS_MES_T **pHandle)
{
    gint32 ret = 0;
    RAS_SMBIOS_MES_T *RasSmbiosHandle = NULL;

    if (pathName == NULL) {
        debug_log(DLOG_ERROR, "[%s]pathName is NULL!", __FUNCTION__);
        return RET_ERR;
    }

    RasSmbiosHandle = (RAS_SMBIOS_MES_T *)g_malloc0(sizeof(RAS_SMBIOS_MES_T));
    if (!RasSmbiosHandle) {
        debug_log(DLOG_ERROR, "[%s]malloc memory failed!", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = freadFile(pathName, SMBIOS_DIFF_FILE_NAME, &(RasSmbiosHandle->pBuf));
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "[%s]read file to buffer failed!", __FUNCTION__);
        g_free(RasSmbiosHandle);
        return RET_ERR;
    }

    
    RasSmbiosHandle->fileLength = vos_get_file_length(pathName);

    
    ret = createRasHash(&(RasSmbiosHandle->pHash));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]createRasHash failed!", __FUNCTION__);
        g_free(RasSmbiosHandle->pBuf);
        g_free(RasSmbiosHandle);
        return RET_ERR;
    }

    
    ret = initRasBiosToHash(RasSmbiosHandle->pHash, RasSmbiosHandle->pBuf, RasSmbiosHandle->fileLength);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]initRasBiosToHash failed ,close RasSmBios!", __FUNCTION__);
        smbios_closeRasSmBios(RasSmbiosHandle);
        return RET_ERR;
    }

    *pHandle = RasSmbiosHandle;
    return RET_OK;
}



LOCAL void bios_write_setting_log(json_object *result_jso)
{
    json_bool ret_bool = FALSE;
    json_object *setting_jso = NULL;
    json_object *messages_jso = NULL;
    json_object *messages_item = NULL;
    json_object *related_jso = NULL;
    json_object *related_item = NULL;

    json_object *cur_jso = NULL;
    json_object *cur_jso_item = NULL;
    json_bool bool_cur_jso = FALSE;

    GSList *caller_info = NULL;

    gint32 message_cnt = 0;
    gint32 message_index = 0;
    gint32 related_cnt = 0;
    gint32 related_index = 0;

    const gchar *related_str = NULL;
    gchar *related_ptr = NULL;

    if (result_jso == NULL) {
        return;
    }

    setting_jso = json_object_from_file(BIOS_FILE_SETTING_PATH);
    if (setting_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        return;
    }

    
    cur_jso = json_object_from_file(BIOS_FILE_CURRENTVALUE_PATH);
    if (cur_jso == NULL) {
        (void)json_object_put(setting_jso);
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail", __FUNCTION__);
        return;
    }
    

    caller_info = g_slist_append(caller_info, g_variant_new_string(CALLER_INFO_IPMI));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(NA_STR));
    
    caller_info = g_slist_append(caller_info, g_variant_new_string(CALLER_INFO_HOST));

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    // 遍历setting.json文件name值
    json_object_object_foreach0(setting_jso, name, val)
    {
        ret_bool = json_object_object_get_ex(result_jso, PROP_MESSAGES, &messages_jso);
        if (ret_bool != TRUE || messages_jso == NULL) {
            (void)json_object_put(setting_jso);
            (void)json_object_put(cur_jso);
            g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "json_object_object_get_ex Messages failed");
            return;
        }

        // 遍历result.json Messages类型错误
        message_cnt = json_object_array_length(messages_jso);

        for (message_index = 0; message_index < message_cnt; message_index++) {
            messages_item = json_object_array_get_idx(messages_jso, message_index);
            if (messages_item == NULL) {
                (void)json_object_put(setting_jso);
                (void)json_object_put(cur_jso);
                g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
                debug_log(DLOG_ERROR, "json_object_array_get_idx message_index failed");
                return;
            }

            // 遍历Messages类型错误中的RelatedProperties
            ret_bool = json_object_object_get_ex(messages_item, PROP_RELATED_PROPS, &related_jso);
            if (ret_bool != TRUE) {
                (void)json_object_put(setting_jso);
                (void)json_object_put(cur_jso);
                g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
                debug_log(DLOG_ERROR, "json_object_object_get_ex RelatedProperties failed");
                return;
            }

            related_cnt = json_object_array_length(related_jso);

            for (related_index = 0; related_index < related_cnt; related_index++) {
                related_item = json_object_array_get_idx(related_jso, related_index);
                if (related_item == NULL) {
                    (void)json_object_put(setting_jso);
                    (void)json_object_put(cur_jso);
                    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
                    debug_log(DLOG_ERROR, "json_object_array_get_idx related_index failed");
                    return;
                }

                related_str = dal_json_object_get_str(related_item);
                if (related_str == NULL) {
                    (void)json_object_put(setting_jso);
                    (void)json_object_put(cur_jso);
                    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
                    debug_log(DLOG_ERROR, "dal_json_object_get_str failed");
                    return;
                }

                // 字符串匹配 #/Attribute/PropName
                related_ptr = g_strrstr(related_str, "/");
                if (related_ptr != NULL && g_strcmp0(name, related_ptr + 1) == 0) {
                    messages_item = NULL;
                    related_item = NULL;
                    goto next_name;
                }

                related_item = NULL;
            }

            messages_item = NULL;
        }

        
        bool_cur_jso = json_object_object_get_ex(cur_jso, name, &cur_jso_item);
        if (bool_cur_jso != TRUE) {
            (void)json_object_put(setting_jso);
            (void)json_object_put(cur_jso);
            g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "json_object_object_get_ex %s failed", name);
            return;
        }

        method_operation_log(caller_info, NULL, "Set %s from [%s] to [%s] success,EvtCode:%s", name,
            dal_json_object_get_str(cur_jso_item), dal_json_object_get_str(val), BIOS_SET_SUCCESS_EVT_CODE);
        

    next_name:;
        // 取下一个name
    }
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif

    g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    (void)json_object_put(setting_jso);

    (void)json_object_put(cur_jso);
}



LOCAL gint32 respond_simple_bios_msg(const void *req_msg, guint8 resp_code)
{
    guint8 resp_data[4] = {0};

    resp_data[0] = resp_code;
    return construt_and_respond_msg(req_msg, resp_data, sizeof(resp_data));
}


gint32 respmsg_with_customermanuid(const void *req_msg, guint8 *pdata, gint32 size, gint32 data_len)
{
    guint32 manu_id = g_rsp_manufactureid_bios;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "[%s]req_msg is NULL!", __FUNCTION__);
        return MODULE_ERR;
    }
    if (pdata == NULL) {
        debug_log(DLOG_ERROR, "[%s]pdata is NULL!", __FUNCTION__);
        return MODULE_ERR;
    }
    if (size < BIOS_MSG_HEAD_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s: parameter invalid", __FUNCTION__);
        return MODULE_ERR;
    }

    
    pdata[1] = LONGB0(manu_id);
    pdata[2] = LONGB1(manu_id);
    pdata[3] = LONGB2(manu_id);

    if (data_len > IPMB_MSG_MAX_PAYLOAD_LEN) {
        data_len = IPMB_MSG_MAX_PAYLOAD_LEN;
    }

    return ipmi_send_response(req_msg, data_len, pdata);
}

LOCAL void smbios_closeRasSmBios(RAS_SMBIOS_MES_T *RasSmbiosHandle)
{
    debug_log(DLOG_DEBUG, "free RasSmbiosHandle");

    if (RasSmbiosHandle == NULL) {
        return;
    }

    if (RasSmbiosHandle->pBuf != NULL) {
        g_free(RasSmbiosHandle->pBuf);
    }

    destoryRasHash(RasSmbiosHandle->pHash);
    g_free(RasSmbiosHandle);

    return;
}

LOCAL gint32 initRasBiosToHash(pRAS_HASH_T hashTable, void *rasBiosBuf, guint32 fileLength)
{
    gchar *pbuf = NULL;
    gint32 ret = RET_OK;
    guint32 lastInsertLength = 0;
    guchar type = 0;

    if (rasBiosBuf == NULL) {
        debug_log(DLOG_ERROR, "[%s]rasBiosBuf is NULL!", __FUNCTION__);
        return RET_ERR;
    }

    pbuf = (gchar *)rasBiosBuf;
    type = *pbuf;

    while ((type != END_OF_TABLE_TYPE)) {
        debug_log(DLOG_DEBUG, "[%s]type is %d!", __FUNCTION__, type);
        ret = insertRasHashNode(hashTable, pbuf, fileLength, &lastInsertLength);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s]insert Node [type %d] failed!", __FUNCTION__, type);
            return RET_ERR;
        }

        pbuf = pbuf + lastInsertLength + 2;

        type = *pbuf;
    }

    return RET_OK;
}


LOCAL gint32 createRasHash(pRAS_HASH_T *hashTable)
{
    guint32 i = 0;

    if (!hashTable) {
        debug_log(DLOG_ERROR, "[%s]hashTable is NULL!", __FUNCTION__);
        return RET_ERR;
    }

    *hashTable = (RAS_HASH_T *)g_malloc0(sizeof(RAS_HASH_T));

    if (!(*hashTable)) {
        debug_log(DLOG_ERROR, "[%s]malloc RAS_HASH_T memory failed!", __FUNCTION__);
        return RET_ERR;
    }

    for (i = 0; i < RAS_HASH_SIZE; i++) {
        (*hashTable)->ppRasContainer[i] = NULL;
    }

    (*hashTable)->nodeCount = 0;

    debug_log(DLOG_DEBUG, "[%s]create RAS hast struct successfully!", __FUNCTION__);

    return RET_OK;
}

LOCAL gint32 freadFile(const gchar *pathName, const gchar *filename, gchar **buf)
{
    glong length;
    glong getRead = 0;
    FM_FILE_S *file_handle = NULL;

    debug_log(DLOG_DEBUG, "[%s]start to read file to buf!", __FUNCTION__);

    length = vos_get_file_length(pathName);
    if (length == 0) {
        debug_log(DLOG_ERROR, "[%s]len is %ld!", __FUNCTION__, length);
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "[%s]len is %ld!", __FUNCTION__, length);

    file_handle = fm_fopen(filename, "r+b");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "open file failed!");
        return RET_ERR;
    }

    *buf = (gchar *)g_malloc0(length + 10);

    if (!(*buf)) {
        debug_log(DLOG_ERROR, "[%s]malloc mem failed!", __FUNCTION__);
        fm_fclose(file_handle);
        return RET_ERR;
    }

    getRead = fm_fread((*buf), sizeof(gchar), length, file_handle);
    if (length != getRead) {
        debug_log(DLOG_ERROR, "[%s]read buffer failed! len:%ld getRead:%ld", __FUNCTION__, length,
            getRead);
        g_free(*buf);
        *buf = NULL;
        fm_fclose(file_handle);
        return RET_ERR;
    } else {
        debug_log(DLOG_DEBUG, "[%s]read buffer success! len:%ld getRead:%ld", __FUNCTION__, length,
            getRead);
        fm_fclose(file_handle);

        return RET_OK;
    }
}

LOCAL void destoryRasHash(RAS_HASH_T *hashTable)
{
    RAS_NODE_T *pTemp = NULL;

    if (!hashTable) {
        debug_log(DLOG_ERROR, "[%s]hashTable is NULL!", __FUNCTION__);
        return;
    }

    debug_log(DLOG_DEBUG, "[%s]free RAS_NODE_T, count is %d!", __FUNCTION__, hashTable->nodeCount);

    for (gint32 i = 0; i < RAS_HASH_SIZE; i++) {
        while (hashTable->ppRasContainer[i] != NULL) {
            pTemp = hashTable->ppRasContainer[i];
            hashTable->ppRasContainer[i] = pTemp->pNext;

            if (pTemp->description != NULL) {
                g_free(pTemp->description);
            }

            g_free(pTemp);
        }
    }

    if (hashTable != NULL) {
        g_free(hashTable);
    }

    debug_log(DLOG_DEBUG, "[%s]finish free hashTable!", __FUNCTION__);

    return;
}

LOCAL gint32 insertRasHashNode(pRAS_HASH_T hashTable, void *pData, guint32 fileLength, guint32 *insertLength)
{
    RAS_NODE_T *pNode = NULL;
    RAS_NODE_T *pTemp = NULL;

    if ((hashTable == NULL) || (pData == NULL)) {
        debug_log(DLOG_ERROR, "[%s]hashTable is NULL or pData is NULL!", __FUNCTION__);
        return RET_ERR;
    }

    gint32 hash_index = *(gchar *)pData; //  当前block丝印的类型，同时也是hash表里面的键值

    if ((hash_index < 0) || (hash_index > (RAS_HASH_SIZE - 1))) {
        debug_log(DLOG_ERROR, "[%s]type is %d, more than max value %d!", __FUNCTION__, hash_index, RAS_HASH_SIZE);
        return RET_ERR;
    }

    
    pNode = (RAS_NODE_T *)g_malloc0(sizeof(RAS_NODE_T));
    if (pNode == NULL) {
        debug_log(DLOG_ERROR, "[%s]malloc mem failed!", __FUNCTION__);
        return RET_ERR;
    }

    pNode->pNext = NULL;
    pNode->pData = pData;
    pNode->length = getBiosBlockLength(pData, fileLength);

    
    if (pNode->length >= fileLength || pNode->length == 0) {
        debug_log(DLOG_ERROR, "length of type 0x%02x is %d, more than %d", hash_index, pNode->length, fileLength);
        g_free(pNode);
        return RET_ERR;
    }

    
    gint32 ret = getBiosSilk(pData, &(pNode->description), 0, pNode->length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]getBiosSilk failed!", __FUNCTION__);
        g_free(pNode);
        return RET_ERR;
    }

    
    pTemp = hashTable->ppRasContainer[hash_index];

    if (!pTemp) {
        hashTable->ppRasContainer[hash_index] = pNode;
    } else {
        pNode->pNext = pTemp;
        hashTable->ppRasContainer[hash_index] = pNode;
    }

    hashTable->nodeCount++;
    *insertLength = pNode->length;

    return RET_OK;
}

LOCAL gint32 getBiosSilk(void *pData, gchar **pDes, guint32 cur_len, guint32 max_len)
{
    gchar *pbuf = NULL;
    guint32 header_len = 0, silk_len = 0;
    gchar *silk_info = NULL;

    if (pData == NULL || pDes == NULL) {
        debug_log(DLOG_ERROR, "[%s]pData or pDes is NULL!", __FUNCTION__);
        return RET_ERR;
    }

    pbuf = (gchar *)pData;

    if (max_len - cur_len <= 2) {
        debug_log(DLOG_ERROR, "[%s]max_len:%d  cur_len:%d!", __FUNCTION__, max_len, cur_len);
        return RET_ERR;
    }

    header_len = *(pbuf + 1);
    if ((header_len + cur_len) > max_len) {
        debug_log(DLOG_ERROR, "[%s]header_len:%d max_len:%d  cur_len:%d!", __FUNCTION__, header_len, max_len, cur_len);
        return RET_ERR;
    }

    
    pbuf += header_len;
    silk_len = strlen(pbuf);
    if (((header_len + silk_len + cur_len) > max_len) || (silk_len == 0)) {
        debug_log(DLOG_ERROR, "[%s]header_len:%d silk_len:%d max_len:%d  cur_len:%d!", __FUNCTION__, header_len,
            silk_len, max_len, cur_len);
        return RET_ERR;
    }

    
    silk_info = (gchar *)g_malloc0(sizeof(gchar) * (silk_len + 2));
    if (silk_info == NULL) {
        debug_log(DLOG_ERROR, "[%s]malloc mem failed!", __FUNCTION__);
        return RET_ERR;
    }

    
    gint32 safe_fun_ret = strncpy_s(silk_info, (silk_len + 2), pbuf, (silk_len));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    

    debug_log(DLOG_ERROR, "[%s]silk is %s!", __FUNCTION__, silk_info);

    *pDes = silk_info;

    return RET_OK;
}

LOCAL guint32 getBiosBlockLength(void *pData, guint32 fileLength)
{
    gchar *pbuf = NULL;
    guint32 len = 0, header_len = 0;

    if (pData == NULL) {
        debug_log(DLOG_ERROR, "[%s]pData is NULL!", __FUNCTION__);
        return 0;
    }

    pbuf = (gchar *)pData;
    header_len = *(pbuf + 1);
    if (header_len > fileLength) {
        debug_log(DLOG_ERROR, "[%s]header_len [%d] is more than fileLength [%d]!", __FUNCTION__, header_len,
            fileLength);
        return 0;
    }

    pbuf += header_len;
    len = header_len;

    while ((*pbuf != 0) || (*(pbuf + 1) != 0)) {
        pbuf++;
        len++;

        if (len > fileLength) {
            debug_log(DLOG_ERROR, "[%s]len [%d] is more than fileLength [%d]!", __FUNCTION__, len, fileLength);
            return 0;
        }
    }
    return len;
}


gint32 bios_read_file_from_bmc(const void *req_msg, gpointer user_data)
{
    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    const guint8 *src_data = get_ipmi_src_data(req_msg);
    
    
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    
    

    
    gint32 ret_val = judge_manu_id_valid_bios(src_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, ret_val);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_CMD);
    }

    guint8 *resp_data = (guint8 *)g_malloc0(MAX_POSSIBLE_IPMI_FRAME_LEN);
    if (resp_data == NULL) {
        
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!\n", MAX_POSSIBLE_IPMI_FRAME_LEN);
        
        return ipmi_send_simple_response(req_msg, COMP_CODE_MEMOUT);
        
    }

    
    guint8 bios_id = src_data[BIOS_ID_OFFSET];

    if (bios_id > BIOS_ID) {
        debug_log(DLOG_ERROR, "BiosId:%d(MaxId:%d) is invalid!\n", bios_id, BIOS_ID);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        ret_val = construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);

        g_free(resp_data);

        return ret_val;
    }

    
    guint8 sub_cmd = src_data[BIOS_SUB_CMD_OFFSET];

    g_mutex_lock(&g_bios_rd_data_cfg_mutex);
    switch (sub_cmd) {
        case BIOS_OPERATE_PREPARE:

            // 1)write prepare
            ret_val = read_file_prepare(req_msg, resp_data, MAX_POSSIBLE_IPMI_FRAME_LEN);
            break;

        case BIOS_OPERATE_DATA:

            // 2)write data
            ret_val = read_file_data(req_msg, resp_data, MAX_POSSIBLE_IPMI_FRAME_LEN);
            break;

        case BIOS_OPERATE_FINISH:

            // 3)write finish
            ret_val = read_file_finish(req_msg, resp_data, MAX_POSSIBLE_IPMI_FRAME_LEN);
            break;

        default:
            resp_data[0] = SMBIOS_ERR_INVALID_STATUS;
            ret_val = construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
            break;
    }
    g_mutex_unlock(&g_bios_rd_data_cfg_mutex);

    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "SubCmd:%d fail(Ret:0x%x)!\n", sub_cmd, ret_val);
    }

    g_free(resp_data);

    return ret_val;
}

LOCAL void __free_rd_data_buff(void)
{
    if (g_bios_rd_data_cfg.data_buf != NULL) {
        g_free(g_bios_rd_data_cfg.data_buf);
        g_bios_rd_data_cfg.data_buf = NULL;
    }
}

#define BIOS_SILK_JSON_FILE_HEADER          "Properties"
#define BIOS_SILK_JSON_HEADER_TYPE          "Type"
#define BIOS_SILK_JSON_HEADER_TYPE_STR      "BIOS SILK CFG"
#define BIOS_SILK_JSON_HEADER_VERSION       "Version"
#define BIOS_SILK_JSON_HEADER_VER_VALUE     1
#define BIOS_SILK_JSON_MEM_TITLE            "MemSilk"
#define BIOS_SILK_JSON_PCIE_TITLE           "PCIeSilk"
#define BIOS_SILK_JSON_NIC_TITLE            "NICSilk"
#define BIOS_SILK_JSON_DISK_TITLE           "DiskSilk"

#define BIOS_SILK_JSON_SEGMENT              "Segment"
#define BIOS_SILK_JSON_SOCKET_ID            "SocketId"
#define BIOS_SILK_JSON_PHY_CHANNEL_ID       "PhysicalChannelId"
#define BIOS_SIKL_JSON_PHY_ID               "PhyId"
#define BIOS_SIKL_JSON_CONTROL_ID           "ControlId"
#define BIOS_SIKL_JSON_SLOT_ID              "SlotId"
#define BIOS_SILK_JSON_ROOT_BDF             "RootBDF"
#define BIOS_SILK_JSON_LOGICAL_CHANNEL_ID   "LogicalChannelId"
#define BIOS_SILK_JSON_DIMM_ID              "DimmId"
#define BIOS_SILK_JSON_SILK                 "Silk"
#define BIOS_SILK_JSON_ROOT_DEVICE_ID       "RootPortDeviceId"
#define BIOS_SILK_JSON_SLOT_ID              "SlotId"
#define BIOS_SILK_JSON_DEVICE_TYPE          "DeviceType"
#define SILK_JSON_PCIE_TYPE                 1
#define SILK_JSON_NIC_TYPE                  2
#define COMPONENT_NVME_STR                  "NVMe"
#define COMPONENT_PCIE_STR                  "PCIe"
#define COMPONENT_OCP_STR                   "OCP"
#define COMPONENT_NIC_STR                   "NIC"


LOCAL json_object *get_silk_config_header(void)
{
    json_object *file_header_obj = json_object_new_object();
    if (file_header_obj == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_object failed");
        return NULL;
    }
    (void)json_object_object_add(file_header_obj, BIOS_SILK_JSON_HEADER_TYPE,
        json_object_new_string(BIOS_SILK_JSON_HEADER_TYPE_STR));
    (void)json_object_object_add(file_header_obj, BIOS_SILK_JSON_HEADER_VERSION,
        json_object_new_int(BIOS_SILK_JSON_HEADER_VER_VALUE));
    return file_header_obj;
}


LOCAL json_object *get_memory_silk_config(void)
{
    json_object *mem_silk_obj = NULL;
    json_object *mem_node_obj = NULL;
    GSList *mem_list = NULL;
    GSList *mem_node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 socket_id = 0;
    guint8 phy_channel_id = 0xff;
    guint8 logi_channel_id = 0xff;
    guint8 dimm_id = 0;
    gchar dimm_name[NAME_LEN] = {0};

    mem_silk_obj = json_object_new_array();
    if (mem_silk_obj == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_array failed");
        return NULL;
    }
    gint32 ret = dfl_get_object_list(CLASS_MEMORY, &mem_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call class Memory dfl_get_object_list failed, ret=%d", ret);
        return mem_silk_obj;
    }
    for (mem_node = mem_list; mem_node; mem_node = mem_node->next) {
        obj_handle = (OBJ_HANDLE)mem_node->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_CPU_ID, &socket_id);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_CHANNEL_ID, &phy_channel_id);
        (void)dal_get_memory_channel_logical_id_by_physical_id(phy_channel_id, &logi_channel_id);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEM_DIMM_ID, &dimm_id);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_MEM_NAME, dimm_name, sizeof(dimm_name));
        mem_node_obj = json_object_new_object();
        if (mem_node_obj == NULL) {
            debug_log(DLOG_ERROR, "json_object_new_object failed");
            continue;
        }
        (void)json_object_object_add(mem_node_obj, BIOS_SILK_JSON_SOCKET_ID, json_object_new_int(socket_id));
        (void)json_object_object_add(mem_node_obj, BIOS_SILK_JSON_PHY_CHANNEL_ID, json_object_new_int(phy_channel_id));
        (void)json_object_object_add(mem_node_obj, BIOS_SILK_JSON_LOGICAL_CHANNEL_ID,
            json_object_new_int(logi_channel_id));
        (void)json_object_object_add(mem_node_obj, BIOS_SILK_JSON_DIMM_ID, json_object_new_int(dimm_id));
        (void)json_object_object_add(mem_node_obj, BIOS_SILK_JSON_SILK,
            json_object_new_string((const gchar *)dimm_name));
        (void)json_object_array_add(mem_silk_obj, mem_node_obj);
    }
    g_slist_free(mem_list);
    return mem_silk_obj;
}


LOCAL const gchar *convert_device_type_to_string(guint8 device_type)
{
    switch (device_type) {
        case COMPONENT_TYPE_HARDDISK:
            return COMPONENT_NVME_STR;
        case COMPONENT_TYPE_PCIE_CARD:
            return COMPONENT_PCIE_STR;
        case COMPONENT_TYPE_OCP:
            return COMPONENT_OCP_STR;
        default:
            return NULL;
    }
}

LOCAL void get_pcie_json_node_pcie(OBJ_HANDLE pcie_addr_handle, json_object *json_obj, const gchar *type_str)
{
    guint8 socket_id;
    guint8 rootport_device_id;
    guint8 slot_id;
    guint8 segment;

    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_SEGMENT, &segment);
    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_SOCKET_ID, &socket_id);
    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_PCIE_DEVICE, &rootport_device_id);
    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_PCIE_SLOT_ID, &slot_id);
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SEGMENT, json_object_new_int(segment));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SOCKET_ID, json_object_new_int(socket_id));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_ROOT_DEVICE_ID,
        json_object_new_int(rootport_device_id));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SLOT_ID, json_object_new_int(slot_id));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_DEVICE_TYPE, json_object_new_string(type_str));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SILK, json_object_new_string(""));
    return;
}
 
LOCAL void get_pcie_json_node_nic(OBJ_HANDLE pcie_addr_handle, json_object *json_obj)
{
    guint8 socket_id;
    guint8 slot_id;

    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_SOCKET_ID, &socket_id);
    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_PCIE_SLOT_ID, &slot_id);
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SOCKET_ID, json_object_new_int(socket_id));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SLOT_ID, json_object_new_int(slot_id));
    (void)json_object_object_add(json_obj, BIOS_SILK_JSON_SILK, json_object_new_string(""));
    return;
}
 

LOCAL json_object *get_pcie_json_node(OBJ_HANDLE pcie_addr_handle, guint8 type)
{
    guint8 comp_type;
    json_object *json_obj = NULL;
    const gchar *type_str;

    (void)dal_get_property_value_byte(pcie_addr_handle, PROPERTY_COMPONENT_TYPE, &comp_type);
    switch (type) {
        case SILK_JSON_PCIE_TYPE:
            type_str = convert_device_type_to_string(comp_type);
            if (type_str == NULL) {
                break;
            }
            json_obj = json_object_new_object();
            if (json_obj == NULL) {
                debug_log(DLOG_ERROR, "json_object_new_object failed");
                break;
            }
            get_pcie_json_node_pcie(pcie_addr_handle, json_obj, type_str);
            break;
        case SILK_JSON_NIC_TYPE:
            if (comp_type != COMPONENT_TYPE_NIC_CARD) {
                break;
            }
            json_obj = json_object_new_object();
            if (json_obj == NULL) {
                debug_log(DLOG_ERROR, "json_object_new_object failed");
                break;
            }
            get_pcie_json_node_nic(pcie_addr_handle, json_obj);
        default:
            break;
    }
    return json_obj;
}


LOCAL json_object *get_pcie_silk_config(guint8 type)
{
    GSList *pcie_addr_list = NULL;
    GSList *pcie_addr_node = NULL;
    OBJ_HANDLE pcie_addr_handle = 0;
    json_object *node_json_obj = NULL;
    json_object *pcie_silk_array_obj = json_object_new_array();
    if (pcie_silk_array_obj == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_array failed");
        return NULL;
    }
    gint32 ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &pcie_addr_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call PcieAddrInfo dfl_get_object_list failed, ret=%d", ret);
        return pcie_silk_array_obj;
    }
    for (pcie_addr_node = pcie_addr_list; pcie_addr_node; pcie_addr_node = pcie_addr_node->next) {
        pcie_addr_handle = (OBJ_HANDLE)pcie_addr_node->data;
        node_json_obj = get_pcie_json_node(pcie_addr_handle, type);
        if (node_json_obj == NULL) {
            continue;
        }
        debug_log(DLOG_DEBUG, "found Object %s for silk config", dfl_get_object_name(pcie_addr_handle));
        (void)json_object_array_add(pcie_silk_array_obj, node_json_obj);
    }
    g_slist_free(pcie_addr_list);
    return pcie_silk_array_obj;
}


LOCAL json_object *get_disk_silk_config(void)
{
    json_object *disk_silk_obj = NULL;
    json_object *disk_node_obj = NULL;
    GSList *disk_list = NULL;
    GSList *disk_node = NULL;
    OBJ_HANDLE obj_handle;
    guint8 socket_id = 0;
    guint8 control_id = 0;
    guint8 phy_id = 0;
    guint8 slot_id = 0;
    gchar rootbdf[NAME_LEN] = {0};

    disk_silk_obj = json_object_new_array();
    if (disk_silk_obj == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_array failed");
        return NULL;
    }
    gint32 ret = dfl_get_object_list(CLASS_DISK_ADDR_INFO, &disk_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "call class DiskAddrInfo dfl_get_object_list failed, ret=%d", ret);
        return disk_silk_obj;
    }
    for (disk_node = disk_list; disk_node; disk_node = disk_node->next) {
        obj_handle = (OBJ_HANDLE)disk_node->data;
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DISK_ADDR_INFO_SOCKET_ID, &socket_id);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DISK_ADDR_INFO_CONTROL_ID, &control_id);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DISK_ADDR_INFO_PHY_ID, &phy_id);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DISK_ADDR_INFO_SLOT_ID, &slot_id);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DISK_ADDR_INFO_ROOT_BDF, rootbdf, sizeof(rootbdf));
        disk_node_obj = json_object_new_object();
        if (disk_node_obj == NULL) {
            debug_log(DLOG_ERROR, "json_object_new_object failed");
            continue;
        }
        (void)json_object_object_add(disk_node_obj, BIOS_SILK_JSON_SOCKET_ID, json_object_new_int(socket_id));
        (void)json_object_object_add(disk_node_obj, BIOS_SIKL_JSON_CONTROL_ID, json_object_new_int(control_id));
        (void)json_object_object_add(disk_node_obj, BIOS_SIKL_JSON_PHY_ID, json_object_new_int(phy_id));
        (void)json_object_object_add(disk_node_obj, BIOS_SIKL_JSON_SLOT_ID, json_object_new_int(slot_id));
        (void)json_object_object_add(disk_node_obj, BIOS_SILK_JSON_ROOT_BDF,
            json_object_new_string((const gchar *)rootbdf));
        (void)json_object_array_add(disk_silk_obj, disk_node_obj);
    }
    g_slist_free(disk_list);
    return disk_silk_obj;
}


LOCAL gint32 prepare_silk_config(void)
{
    if (vos_get_file_accessible(BIOS_SILK_JSON_FILE_NAME) == TRUE) {
        vos_rm_filepath(BIOS_SILK_JSON_FILE_NAME);
    }
    json_object *file_obj = json_object_new_object();
    if (file_obj == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_object failed");
        return RET_ERR;
    }
    json_object *file_header_obj = get_silk_config_header();
    if (file_header_obj == NULL) {
        debug_log(DLOG_ERROR, "get_silk_config_header failed");
        goto ERROR_EXIT;
    }
    json_object_object_add(file_obj, BIOS_SILK_JSON_FILE_HEADER, file_header_obj);
    json_object *mem_silk_obj = get_memory_silk_config();
    if (mem_silk_obj == NULL) {
        debug_log(DLOG_ERROR, "get_memory_silk_config failed");
        goto ERROR_EXIT;
    }
    json_object_object_add(file_obj, BIOS_SILK_JSON_MEM_TITLE, mem_silk_obj);
    json_object *pcie_silk_obj = get_pcie_silk_config(SILK_JSON_PCIE_TYPE);
    if (pcie_silk_obj == NULL) {
        debug_log(DLOG_ERROR, "get_pcie_silk_config failed");
        goto ERROR_EXIT;
    }
    json_object_object_add(file_obj, BIOS_SILK_JSON_PCIE_TITLE, pcie_silk_obj);
    json_object *nic_silk_obj = get_pcie_silk_config(SILK_JSON_NIC_TYPE);
    if (nic_silk_obj == NULL) {
        debug_log(DLOG_ERROR, "get_nic_silk_config failed");
        goto ERROR_EXIT;
    }
    json_object_object_add(file_obj, BIOS_SILK_JSON_NIC_TITLE, nic_silk_obj);
    json_object *disk_silk_obj = get_disk_silk_config();
    if (disk_silk_obj == NULL) {
        debug_log(DLOG_ERROR, "get_disk_silk_config failed");
        goto ERROR_EXIT;
    }
    json_object_object_add(file_obj, BIOS_SILK_JSON_DISK_TITLE, disk_silk_obj);
    gint32 ret = json_object_to_file(BIOS_SILK_JSON_FILE_NAME, file_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "json_object_to_file failed, ret=%d", ret);
        goto ERROR_EXIT;
    }
    (void)chmod(BIOS_SILK_JSON_FILE_NAME, S_IRUSR);
    json_object_put(file_obj);
    return RET_OK;
ERROR_EXIT:
    json_object_put(file_obj);
    return RET_ERR;
}

LOCAL void load_configsync_ini_print(BIOS_DATA_OPERATE_S *data_operate)
{
    guint16 i = 0;
    for (i = 0; i < (data_operate->data_len) / 5; i++) {
        debug_log(DLOG_ERROR, "Read SetupSync Data %d %d %d %d %d %d", i, data_operate->data_buf[i * 5],
            data_operate->data_buf[i * 5 + 1], data_operate->data_buf[i * 5 + 2], data_operate->data_buf[i * 5 + 3],
            data_operate->data_buf[i * 5 + 4]);
    }
    for (i = 0; i < (data_operate->data_len) % 5; i++) {
        debug_log(DLOG_ERROR, "Read SetupSync Data %d", data_operate->data_buf[(data_operate->data_len) / 5 * 5 + i]);
    }
}


LOCAL gint32 load_configsync_ini(BIOS_DATA_OPERATE_S *data_operate)
{
    OBJ_HANDLE obj_handle;
    GVariant *property_data = NULL;
    gsize parm_temp;

    gint32 ret = dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Class Bios obj fail ret_val = %d", ret);
        return RET_ERR;
    }
    ret = dfl_get_property_value(obj_handle, PROPERTY_BIOS_MENU_DATA, &property_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Class Bios property_data fail ret_val = %d", ret);
        ret = RET_ERR;
        goto EXIT;
    }

    const guint8 *tmp_out = (const guint8 *)g_variant_get_fixed_array(property_data, &parm_temp, sizeof(guint8));
    guint32 file_size = (guint32)parm_temp;
    if (tmp_out == NULL || file_size < BIOS_SETUP_SYNC_MIN_LEN) {
        debug_log(DLOG_ERROR, "property_data is null!");
        ret = RET_ERR;
        goto EXIT;
    }
    
    (void)__free_rd_data_buff();

    
    data_operate->data_buf = (guint8 *)g_malloc0(file_size + 1);
    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!", file_size + 1);
        ret = RET_ERR;
        goto EXIT;
    }
    errno_t securec_rv = memcpy_s(data_operate->data_buf, file_size + 1, tmp_out, file_size);
    if (securec_rv != EOK) {
        debug_log(DLOG_ERROR, "memcpy_s fail, securec_rv = %d", securec_rv);
    }
    data_operate->data_offset = 0;
    data_operate->data_len = file_size;
    debug_log(DLOG_ERROR, "read_file_prepare file_size =%d", file_size);

    load_configsync_ini_print(data_operate);
    ret = RET_OK;
EXIT:
    if (property_data != NULL) {
        g_variant_unref(property_data);
    }
    return ret;
}


gint32 load_file_to_buffer(FILE *file_handle, guint8 **data_buf, guint32 *data_len)
{
    gint32 ret = fseek(file_handle, 0, SEEK_END);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "seek silk file end fail");
        return RET_ERR;
    }
    gint32 file_size = ftell(file_handle);
    if (file_size <= 0) {
        debug_log(DLOG_ERROR, "ftell silk file failed");
        return RET_ERR;
    }
    *data_len = (guint32)file_size;
    if (*data_buf != NULL) {
        g_free(*data_buf);
        *data_buf = NULL;
    }
    *data_buf = (guint8 *)g_malloc0(file_size + 1);
    if (*data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 length %d fail", file_size + 1);
        return RET_ERR;
    }
    ret = fseek(file_handle, 0, SEEK_SET);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "fseek fail start fail");
        g_free(*data_buf);
        return RET_ERR;
    }
    ret = fread(*data_buf, BIOS_FILE_STREAM_SIZE, file_size, file_handle);
    if (ret != file_size) {
        debug_log(DLOG_ERROR, "read silk file fail, read=%d, size=%d", ret, file_size);
        g_free(*data_buf);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 load_silk_config(BIOS_DATA_OPERATE_S *data_operate)
{
    FILE *file_handle = NULL;

    file_handle = fopen(BIOS_SILK_JSON_FILE_NAME, "r");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "open silk file fail");
        return RET_ERR;
    }
    gint32 ret = load_file_to_buffer(file_handle, &(data_operate->data_buf), &data_operate->data_len);
    (void)fclose(file_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "load silk file fail");
        return RET_ERR;
    }
    data_operate->data_offset = 0;
    return RET_OK;
}

LOCAL guint8 fm_read_file_prepare(guint8 file_selector, const gchar *file_name, BIOS_DATA_OPERATE_S *data_operate)
{
    _cleanup_fm_fclose_ FM_FILE_S *file_handle = fm_fopen(file_name, "rb");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "open %s file failed", file_name);
        return SMBIOS_ERR_FM_FAIL;
    }

    
    gint32 ret = fm_fseek(file_handle, 0, SEEK_END);
    if (ret != 0) { // 返回成功是0，不成功非0
        debug_log(DLOG_ERROR, "fm_fseek failed");
        return SMBIOS_ERR_FM_FAIL;
    }

    gint32 file_size = fm_ftell(file_handle);
    if (file_size < 0) {
        debug_log(DLOG_ERROR, "fm_ftell file(file_size: %d) failed", file_size);
        return SMBIOS_ERR_FM_FAIL;
    }

    data_operate->data_len = file_size;

    
    __free_rd_data_buff();
    
    _cleanup_gfree_ guint8 *data_buf = (guint8 *)g_malloc0(file_size + 1);
    if (data_buf == NULL) {
        debug_log(DLOG_ERROR, "g_malloc0 file_size: %d failed", file_size + 1);
        return BIOS_ERR_INVALID_STATUS;
    }

    debug_log(DLOG_INFO, "fm_fread file begin!");

    
    ret = fm_fseek(file_handle, 0, SEEK_SET);
    if (ret != 0) {
        debug_log(DLOG_ERROR, "fm_fseek failed");
        return SMBIOS_ERR_FM_FAIL;
    }

    guint32 file_read_size = fm_fread(data_buf, BIOS_FILE_STREAM_SIZE, file_size, file_handle);
    if (file_size != file_read_size) {
        debug_log(DLOG_ERROR, "fm_fread (file_size:%d, file_read_size:%d) failed", file_size, file_read_size);
        return SMBIOS_ERR_FM_FAIL;
    }

    data_operate->data_buf = TAKE_PTR(data_buf);
    data_operate->data_offset = 0;
    return BIOS_ERR_NORMALLY;
}


LOCAL gint32 read_file_prepare(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    gint32 resp_len = BIOS_MSG_HEAD_MIN_LEN;
    if (request_msg == NULL || resp_data == NULL || resp_data_size < BIOS_ERR_NORMAL_MSG_LEN) {
        return RET_ERR;
    }

    BIOS_DATA_OPERATE_S *data_operate = &g_bios_rd_data_cfg;

    data_operate->data_flag = BIOS_FLAG_DOING;

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is invalid!\n");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, resp_len);
    }

    
    guint8 file_selector = src_data[BIOS_FILE_SELECT_OFFSET];

    if (judge_read_file_selector_valid(file_selector) == FALSE) {
        debug_log(DLOG_ERROR, "judge_read_file_selector_valid file_selector:%d fail.\n", file_selector);
        data_operate->data_flag = BIOS_FLAG_IDLE;
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, resp_len);
    }

    
    if (file_selector == BIOS_FILE_SETUP_NUM) {
        if (load_configsync_ini(data_operate) != RET_OK) {
            data_operate->data_flag = BIOS_FLAG_IDLE;
            resp_data[0] = BIOS_ERR_INVALID_STATUS;
            return construt_and_respond_msg(request_msg, resp_data, resp_len);
        }
        goto EXIT;
    }
    
    if (file_selector == BIOS_FILE_SILK_CONFIG_NUM) {
        if (prepare_silk_config() != RET_OK || load_silk_config(data_operate) != RET_OK) {
            data_operate->data_flag = BIOS_FLAG_IDLE;
            resp_data[0] = BIOS_ERR_INVALID_STATUS;
            return construt_and_respond_msg(request_msg, resp_data, resp_len);
        }
        goto EXIT;
    }

    if (file_selector == BIOS_FILE_NEW_SECUREBOOT_NUM) {
        resp_data[0] = fm_read_file_prepare(file_selector, SECUREBOOT_NEW_SHORT_NAME, data_operate);
        if (resp_data[0] == BIOS_ERR_NORMALLY) {
            goto EXIT;
        }
        data_operate->data_flag = BIOS_FLAG_IDLE;
        return construt_and_respond_msg(request_msg, resp_data, resp_len);
    }

    resp_data[0] = fm_read_file_prepare(file_selector, g_file_property_name[file_selector], data_operate);
    if (resp_data[0] != BIOS_ERR_NORMALLY) {
        data_operate->data_flag = BIOS_FLAG_IDLE;
        return construt_and_respond_msg(request_msg, resp_data, resp_len);
    }

    
    if (file_selector == BIOS_FILE_CLP) {
        set_clp_file_change_flag(FALSE);
    }

    debug_log(DLOG_INFO, "fm_fread file end and clear warning!");

    data_operate->data_offset = 0;

EXIT:
    resp_data[0] = BIOS_ERR_NORMALLY;
    
    resp_data[4] = BIOS_FILE_MORE_DATA;
    resp_data[5] = LONGB0(data_operate->data_len);
    resp_data[6] = LONGB1(data_operate->data_len);
    resp_data[7] = LONGB2(data_operate->data_len);
    resp_data[8] = LONGB3(data_operate->data_len);
    resp_len = BIOS_ERR_NORMAL_MSG_LEN;

    return construt_and_respond_msg(request_msg, resp_data, resp_len);
}

LOCAL void backup_silk_config_file(void)
{
    gint32 ret = vos_file_copy(BIOS_SILK_JSON_BAK_FILE_NAME, BIOS_SILK_JSON_FILE_NAME);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "failed to backup silk file, ret=%d", ret);
        return;
    }
    (void)chmod(BIOS_SILK_JSON_BAK_FILE_NAME, S_IRUSR);
    return;
}


LOCAL gint32 read_file_finish(const void *request_msg, guint8 *resp_data, guint32 resp_data_len)
{
    guint16 check_sum = 0;

    if (request_msg == NULL || resp_data == NULL || resp_data_len < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    BIOS_DATA_OPERATE_S *data_operate = &g_bios_rd_data_cfg;
    if (data_operate->data_buf == NULL) {
        check_sum = 0;
    } else {
        
        check_sum = vos_verify_check_sum(data_operate->data_buf, data_operate->data_len);

        clear_data_operate_res(data_operate);
    }

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is invalid!\n");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    guint8 file_selector = src_data[BIOS_FILE_SELECT_OFFSET];

    if (file_selector == BIOS_FILE_SETUP_NUM) {
        clear_bios_setup_sync_data();
    }
    if (file_selector == BIOS_FILE_SILK_CONFIG_NUM) {
        backup_silk_config_file();
    }

    resp_data[4] = BIOS_FILE_NO_MORE_DATA;
    resp_data[5] = (guint8)check_sum;
    resp_data[0] = BIOS_ERR_NORMALLY;

    return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN + 2);
}


LOCAL gint32 read_file_data(const void *request_msg, guint8 *resp_data, guint32 resp_data_len)
{
    if (request_msg == NULL || resp_data == NULL) {
        return RET_ERR;
    }

    BIOS_DATA_OPERATE_S *data_operate = &g_bios_rd_data_cfg;

    
    if (BIOS_FLAG_DOING != data_operate->data_flag) {
        
        debug_log(DLOG_ERROR, "read_file_data: DataFlag:%d(ShouldBe:%d) is invalid!\n", data_operate->data_flag,
            BIOS_FLAG_DOING);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    const guint8 *src_data = get_ipmi_src_data(request_msg);
    if (src_data == NULL) {
        debug_log(DLOG_ERROR, "src_data is NULL!\n");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    guint8 data_len = get_ipmi_src_data_len(request_msg);
    if (data_len <= BIOS_DATA_LEN_OFFSET) {
        debug_log(DLOG_ERROR, "%s:Invalid cmd length(%u)", __FUNCTION__, data_len);
        resp_data[0] = BIOS_ERR_IV_LEN;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }
    guint32 bios_file_offset = MAKE_DWORD(src_data[BIOS_FILE_OFFSET_START + DWORD_OFFSET - 1],
        src_data[BIOS_FILE_OFFSET_START + 1], src_data[BIOS_FILE_OFFSET_START], src_data[BIOS_FILE_OFFSET_START - 1]);
    guint8 read_len = src_data[BIOS_DATA_LEN_OFFSET];

    if ((bios_file_offset > data_operate->data_len) || ((read_len + 6) > IPMB_MSG_MAX_PAYLOAD_LEN)) {
        debug_log(DLOG_ERROR, "FileOffset:%d or ReadLen:%d is invalid!\n", bios_file_offset, (read_len + 6));
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    if ((bios_file_offset + read_len) >= data_operate->data_len) {
        read_len = data_operate->data_len - bios_file_offset;
        resp_data[4] = BIOS_FILE_NO_MORE_DATA;
    } else {
        resp_data[4] = BIOS_FILE_MORE_DATA;
    }

    
    if (resp_data_len < 5 + read_len) {
        debug_log(DLOG_ERROR, "Response data buffer length is insufficient : %d %d!\n", resp_data_len, read_len);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    if (data_operate->data_buf == NULL) {
        debug_log(DLOG_ERROR, "Databuffer is NullPointer!");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return construt_and_respond_msg(request_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN);
    }
    errno_t safe_fun_ret = memmove_s(&resp_data[5], resp_data_len - 5,
        data_operate->data_buf + bios_file_offset, read_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    guint16 check_sum = vos_verify_check_sum(data_operate->data_buf + bios_file_offset, read_len);
    resp_data[read_len + 5] = (guint8)check_sum;

    debug_log(DLOG_DEBUG, "FileOffset:%d ReadLen:%d CheckSum:%d!\n", bios_file_offset, read_len, check_sum);

    resp_data[0] = BIOS_ERR_NORMALLY;

    
    return construt_and_respond_msg(request_msg, resp_data, read_len + 5 + 1);
}


LOCAL guint8 judge_read_file_selector_valid(guint8 file_index)
{
    switch (file_index) {
        case BIOS_FILE_DISPLAY_NUM:
        case BIOS_FILE_OPTION_NUM:
        case BIOS_FILE_CHANGE_NUM:
        case BIOS_FILE_CLP:
        case BIOS_FILE_SETUP_NUM:

            
        case BIOS_FILE_REGISTRY_NUM:
        case BIOS_FILE_CURRENT_VALUE_NUM:
        case BIOS_FILE_SETTING_NUM:
        case BIOS_FILE_RESULT_NUM:
        case BIOS_FILE_POLICYCONFIGREGISTRY_NUM:
        case BIOS_FILE_CONFIGVALUE_NUM:
            
        case BIOS_FILE_SILK_CONFIG_NUM:
        case BIOS_FILE_NEW_SECUREBOOT_NUM:
            return TRUE;

        case BIOS_FILE_DEBUG_INFO:
        case BIOS_FILE_FIRMWARE:
        case BIOS_FILE_NETCONFIG:
        case BIOS_FILE_SOL:
        case BIOS_FILE_BLACKBOX:
        case BIOS_FILE_CLP_RESP:

            
        case BIOS_FILE_CMES_NUM:
            
            return FALSE;

        default:

            debug_log(DLOG_ERROR, "read file_index:%d is invalid!\n", file_index);
            return FALSE;
    }
}


LOCAL gint32 clear_bios_setup_sync_data(void)
{
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 mode = BIOS_SYNC_DATA;

    gint32 ret_val = dfl_get_object_handle(CLASS_HPC_MANAGEMENT, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "Can't find object %s. Ret is %d\n", "HPCMgmt", ret_val);
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(BIOS_SYNC_DATA));
    input_list = g_slist_append(input_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, &mode, 1, sizeof(guint8)));

    ret_val = dfl_call_class_method(obj_handle, METHOD_HPC_MANAGEMENT_SETCONFIGINFO, NULL, input_list, NULL);
    debug_log(DLOG_DEBUG, "clear_bios_setup_sync_data ret_val = %d\n", ret_val);

    if (ret_val != DFL_OK) {
        if (input_list != NULL) {
            g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        }

        debug_log(DLOG_ERROR, "Call method METHOD_HPC_MANAGEMENT_SETCONFIGINFO error!\n");
        return RET_ERR;
    }

    if (input_list != NULL) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    }

    return RET_OK;
}


void set_clp_file_change_flag(guint8 file_flag)
{
    OBJ_HANDLE object_handle;
    GVariant *property_data = NULL;
    guint8 temp_data = 0;

    gint32 result = dfl_get_object_handle(BIOS_OBJECT_NAME, &object_handle);
    if (result == DFL_OK) {
        result = dfl_get_property_value(object_handle, PROPERTY_BIOS_CLP_CHANGE_FLAG, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_value %s fail!(result=%d)\r\n", PROPERTY_BIOS_CLP_CHANGE_FLAG,
                result);
            return;
        }

        temp_data = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        property_data = NULL;

        debug_log(DLOG_DEBUG, "clp config file_flag:%u changed(Old:%u)\r\n", file_flag, temp_data);

        if (file_flag != temp_data) {
            property_data = g_variant_new_byte(file_flag);
            result = dfl_set_property_value(object_handle, PROPERTY_BIOS_CLP_CHANGE_FLAG, property_data, DF_NONE);
            g_variant_unref(property_data);

            if (result != DFL_OK) {
                debug_log(DLOG_ERROR, "dfl_set_property_value %s fail!(result=%d)\r\n", PROPERTY_BIOS_CLP_CHANGE_FLAG,
                    result);
                return;
            }
        }
    } else {
        debug_log(DLOG_ERROR, "dfl_get_object_handle %s fail!(result=%d)\r\n", BIOS_OBJECT_NAME, result);
    }

    return;
}