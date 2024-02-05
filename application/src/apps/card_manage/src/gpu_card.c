

#include "pme_app/pme_app.h"
#include "get_version.h"
#include "pme_app/common/public_redfish.h"


#include "gpu_card.h"





#define GPU_ERR_LOG_INTERVAL (10 * 60 * 1000)
#define INVALID_NVLINKNUM 1
#define VASTAI_VID  0x1ec6


#define MAX_NVLINKNUM 256

#define REG_BIT_WIDTH 32
#define BITS_PER_READ (2 * REG_BIT_WIDTH)
#define NV_GPU_STATUS_MASK 0x1F

LOCAL void gpu_action_error_log(OBJ_HANDLE gpu_handle, gchar *error_log, guchar refresh_time_flag);

LOCAL gint32 __update_gpu_resource(json_object *resource_jso);
LOCAL gint32 __update_gpucard_summary(json_object *resource_jso);
LOCAL pthread_mutex_t smbpbi_mutex = PTHREAD_MUTEX_INITIALIZER; // 确保写CMD、读DATA寄存器为一次原子操作
LOCAL GMutex g_gpu_card_mutex;

typedef gint32 (*parse_redfish_resource)(json_object *resource_jso);
typedef struct tag_bma_resource_info {
    // 需要提取的BMA资源的odata.type值(后缀)
    gchar odata_type[ODATA_TYPE_MAX_LEN];
    // 此处注册函数,解析redfish资源,提取需要的属性设置到BMC对应的对象属性中
    parse_redfish_resource parse_resource;
} BMA_RESOURCE_INFO;

LOCAL BMA_RESOURCE_INFO g_gpu_bma_resource[] = {
    {RF_TYPE_GPUS, __update_gpu_resource},
    {RF_TYPE_GPU_SUMMARY, __update_gpucard_summary}
};

void gpucard_agentless_init(void)
{
    g_mutex_init(&g_gpu_card_mutex);
}


LOCAL void __update_gpucard_summary_prop(json_object *gpu_uti_jso, OBJ_HANDLE gpu_handle)
{
    guint16 temp_gpu_value = 0;
    guint16 temp_mem_value = 0;

    dal_cache_property_uint16(gpu_uti_jso, RF_TYPE_GPU_UTLIZATION_GPU, &temp_gpu_value);
    dal_cache_property_uint16(gpu_uti_jso, RF_TYPE_GPU_UTLIZATION_MEM, &temp_mem_value);

    (void)dal_set_property_value_uint16(gpu_handle, PROPERTY_GPU_CARD_GPUUTILIZATION, temp_gpu_value, DF_NONE);
    (void)dal_set_property_value_uint16(gpu_handle, PROPERTY_GPU_CARD_MEMORYUTILIZATION, temp_mem_value, DF_NONE);
}


LOCAL gint32 __update_gpucard_summary_default(void)
{
    gint32 ret;
    GSList *gpucard_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE gpu_handle = 0;

    ret = dfl_get_object_list(CLASS_GPU_CARD, &gpucard_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get gpuard_list fail, ret with %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (node = gpucard_list; node; node = node->next) {
        gpu_handle = (OBJ_HANDLE)node->data;
        __update_gpucard_summary_prop(NULL, gpu_handle);
    }

    g_slist_free(gpucard_list);

    return RET_OK;
}


LOCAL gint32 __update_gpucard_summary_updated(json_object *resource_jso)
{
    gint32 ret;
    OBJ_HANDLE gpu_handle = 0;
    json_object *gpu_uti_jso = NULL;
    json_bool ret_bool = TRUE;
    gchar bma_id[PROP_VAL_MAX_LENGTH] = {0};

    if (resource_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error.", __FUNCTION__);
        return RET_ERR;
    }

    // 获取对象句柄
    ret = dal_get_bma_id(resource_jso, bma_id, sizeof(bma_id));
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = dal_get_specific_object_string(CLASS_GPU_CARD, PROPERTY_GPU_CARD_BMAID, bma_id, &gpu_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get handle fail from BMAID failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret_bool = json_object_object_get_ex(resource_jso, RF_TYPE_UTLIZATION_ALL, &gpu_uti_jso);
    if (ret_bool != TRUE || gpu_uti_jso == NULL) {
        return RET_ERR;
    }
    // 更新BMC中各个属性值
    __update_gpucard_summary_prop(gpu_uti_jso, gpu_handle);
    return RET_OK;
}


LOCAL gint32 __update_gpucard_summary(json_object *resource_jso)
{
    gint32 ret;

    // 入参为NULL,则将数据清除
    if (resource_jso == NULL) {
        return __update_gpucard_summary_default();
    }

    ret = __update_gpucard_summary_updated(resource_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update gpu summary failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void __update_gpu_resource_prop(json_object *resource_jso, OBJ_HANDLE pme_handle)
{
    dal_update_property_string_try_agent(resource_jso, RF_PROPERTY_ODATA_ID, pme_handle, PROPERTY_GPU_CARD_BMAID);
    return;
}


LOCAL gint32 __update_gpu_resource_default(void)
{
    gint32 ret;
    GSList *gpucard_list = NULL;
    GSList *gpucard_node = NULL;
    OBJ_HANDLE obj_handle = 0;

    ret = dfl_get_object_list(CLASS_GPU_CARD, &gpucard_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_list failed.", __FUNCTION__);
        return RET_ERR;
    }

    for (gpucard_node = gpucard_list; gpucard_node; gpucard_node = gpucard_node->next) {
        obj_handle = (OBJ_HANDLE)gpucard_node->data;
        __update_gpu_resource_prop(NULL, obj_handle);
    }

    g_slist_free(gpucard_list);
    return RET_OK;
}


LOCAL gint32 __get_gpucard_handle_by_bdf(const guint8 *bdf, gsize bdf_len, OBJ_HANDLE *pme_handle)
{
    gint32 ret;
    GSList *pcie_card_list = NULL;
    GSList *pcie_card_node = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 bus_num = 0;
    guint8 dev_num = 0;
    guint8 fun_num = 0;

    if (bdf == NULL || pme_handle == NULL || bdf_len < BDF_INFO_NUM) {
        debug_log(DLOG_ERROR, "%s: input is null with bdf_len %zu.", __FUNCTION__, bdf_len);
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_PCIE_CARD, &pcie_card_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_object_list CLASS_PCIE_CARD fail with %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (pcie_card_node = pcie_card_list; pcie_card_node; pcie_card_node = pcie_card_node->next) {
        obj_handle = (OBJ_HANDLE)pcie_card_node->data;
        // 直接获取BDF信息,如果匹配则说明直接找到
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_BUS_NUM, &bus_num);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_DEV_NUM, &dev_num);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUN_NUM, &fun_num);

        if (bus_num == bdf[1] && dev_num == bdf[2] && fun_num == bdf[3]) { // bdf的1、2、3字段对应bus、devcie、function
            dal_get_specific_object_position(obj_handle, CLASS_GPU_CARD, pme_handle);
            g_slist_free(pcie_card_list);
            return RET_OK;
        }
    }

    g_slist_free(pcie_card_list);
    debug_log(DLOG_ERROR, "%s:failed to get gpucard_handle", __FUNCTION__);
    return RET_ERR;
}


LOCAL gint32 __get_gpucard_handle(json_object *resource_jso, OBJ_HANDLE *pme_handle)
{
    gint32 ret;
    json_object *odataid_jso = NULL;
    guint8 bdf[BDF_INFO_NUM] = {0};
    json_bool ret_bool = TRUE;
    const gchar *id_str = NULL;
    const gchar *bdf_res = NULL;

    // 根据ID找不到则根据BDF去进行匹配
    ret_bool = json_object_object_get_ex(resource_jso, RF_PROPERTY_ODATA_ID, &odataid_jso);
    if (ret_bool != TRUE || odataid_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:get odata id fail.", __FUNCTION__);
        return RET_ERR;
    }

    id_str = dal_json_object_get_str(odataid_jso);
    if (id_str == NULL) {
        debug_log(DLOG_ERROR, "%s:get string id_str fail.", __FUNCTION__);
        return RET_ERR;
    }

    // 根据BMA ID索引信息查找对象,找到则直接返回句柄即可
    ret = dal_get_specific_object_string(CLASS_GPU_CARD, PROPERTY_GPU_CARD_BMAID, id_str, pme_handle);
    if (ret == RET_OK) {
        return RET_OK;
    }

    bdf_res = dal_get_bdf_info_by_bma(resource_jso, RF_PROPERTY_BDF);
    if (bdf_res == NULL) {
        return RET_ERR;
    }

    ret = dal_convert_raid_bdf_info(bdf_res, strlen(bdf_res), bdf, sizeof(bdf));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:convert_raid_bdf_info fail.", __FUNCTION__);
        return RET_ERR;
    }

    ret = __get_gpucard_handle_by_bdf(bdf, sizeof(bdf), pme_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 __update_gpu_resource_updated(json_object *resource_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    if (resource_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:input param error.", __FUNCTION__);
        return RET_ERR;
    }

    g_mutex_lock(&g_gpu_card_mutex);

    // 获取GPU对象句柄
    ret = __get_gpucard_handle(resource_jso, &obj_handle);
    // 如果为空则说明未找到,返回错误码RSC_NOT_EXIST
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: find gpu object handle failed.", __FUNCTION__);
        g_mutex_unlock(&g_gpu_card_mutex);
        return RSC_NOT_EXIST;
    }
    __update_gpu_resource_prop(resource_jso, obj_handle);

    g_mutex_unlock(&g_gpu_card_mutex);
    return RET_OK;
}


LOCAL gint32 __update_gpu_resource(json_object *resource_jso)
{
    gint32 ret;

    // 入参为空则刷新属性为默认值
    if (resource_jso == NULL) {
        return __update_gpu_resource_default();
    }

    // 如果是删除事件,入参对象中为字符串对象,携带索引信息
    if (json_object_get_type(resource_jso) == json_type_string) {
        // 当前GPU还不支持热插拔，不用删除对象
        debug_log(DLOG_ERROR, "%s:invalid removed event", __FUNCTION__);
        ret = RET_OK;
    } else {
        // 对于更新类事件,找到对应对象句柄，解析对应属性并设置
        ret = __update_gpu_resource_updated(resource_jso);
    }
    return ret;
}


gint32 set_gpucard_info_from_bma(OBJ_HANDLE group_obj, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    json_object *resource_jso = NULL;
    const gchar *type_str = NULL;
    const gchar *rsc_str = NULL;
    gint32 ret = RET_ERR;
    guint32 array_index;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "%s: Input_list can not be NULL.", __FUNCTION__);
        return RET_ERR;
    }

    type_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (type_str == NULL) {
        debug_log(DLOG_ERROR, "%s: get type string fail", __FUNCTION__);
        return RET_ERR;
    }

    rsc_str = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    if (rsc_str == NULL) {
        debug_log(DLOG_ERROR, "%s: get resource string fail", __FUNCTION__);
        return RET_ERR;
    }

    if (strlen(rsc_str) != 0) {
        resource_jso = json_tokener_parse(rsc_str);
        if (resource_jso == NULL) {
            resource_jso = json_object_new_string(rsc_str);
        }
        if (resource_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: get resource_jso fail", __FUNCTION__);
            return RET_ERR;
        }
    }

    for (array_index = 0; array_index < ARRAY_SIZE(g_gpu_bma_resource); array_index++) {
        if (g_gpu_bma_resource[array_index].parse_resource == NULL) {
            continue;
        }
        // type_str中存储的是完整的odata.type，bmc_resource中为odata.type的后缀不带版本号等信息
        if (g_str_has_suffix(type_str, g_gpu_bma_resource[array_index].odata_type)) {
            debug_log(DLOG_INFO, "%s : matched type is %s", __FUNCTION__, g_gpu_bma_resource[array_index].odata_type);
            ret = g_gpu_bma_resource[array_index].parse_resource(resource_jso);
            break;
        }
    }
    if (resource_jso != NULL) {
        json_object_put(resource_jso);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update fail", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gchar *transfer_status_to_string(const guint32 status)
{
    switch (status) {
        case CMD_STATUS_NULL:
            return "No response";

        case CMD_STATUS_ERR_REQUEST:
            return "ERR_REQUEST";

        case CMD_STATUS_ERR_OPCODE:
            return "ERR_OPCODE";

        case CMD_STATUS_ERR_ARG1:
            return "ERR_ARG1";

        case CMD_STATUS_ERR_ARG2:
            return "ERR_ARG2";

        case CMD_STATUS_ERR_DATA:
            return "ERR_DATA";

        case CMD_STATUS_ERR_MISC:
            return "ERR_MISC";

        case CMD_STATUS_ERR_I2C_ACCESS:
            return "ERR_I2C_ACCESS";

        case CMD_STATUS_ERR_NOT_SUPPORTED:
            return "ERR_NOT_SUPPORTED";

        case CMD_STATUS_INACTIVE:
            return "INACTIVE";

        case CMD_STATUS_READY:
            return "READY";

        case CMD_STATUS_SUCCESS:
            return "SUCCESS";

        default:
            return "Unknown error";
    }
}


LOCAL guint32 get_data_reg_addr(OBJ_HANDLE obj_handle)
{
    guint32 gpu_type = 0;

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_GPU_CARD_GPUTYPE, &gpu_type);

    if (gpu_type == NV_GPU_TYPE_FERMI) {
        debug_log(DLOG_DEBUG, "NV_DATA_REG = 0x%02x", NV_DATA_REG_FERMI);
        return NV_DATA_REG_FERMI;
    }

    return NV_DATA_REG;
}


LOCAL guint32 assemble_smbpbi_cmd(guint8 OpCode, guint8 Arg1, guint8 Arg2)
{
    
    reg_t smbpbi_cmd;

    smbpbi_cmd.byte[0] = OpCode;
    smbpbi_cmd.byte[1] = Arg1;
    smbpbi_cmd.byte[2] = Arg2;
    smbpbi_cmd.byte[3] = 0x80;

    return smbpbi_cmd.word;
}


LOCAL gint32 read_nv_gpu_reg(OBJ_HANDLE gpu_handle, guint8 reg_offset, reg_t *data)
{
    gint32 ret = 0;
    guint8          read_buf[sizeof(smbpbi_data_t) + 1] = {0};
    guint8 length = 0;
    const gchar *obj_name = NULL;

    obj_name = dfl_get_object_name(gpu_handle);

    ret = dfl_block_read(gpu_handle, PROPERTY_GPU_CARD_DEV_ACCESSOR, reg_offset, sizeof(reg_t) + 1, read_buf);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: read data from 0x%02x failed. ret=%d", obj_name, reg_offset, ret);
        return ret;
    }

    length = read_buf[0];
    data->word = MAKE_DWORD(read_buf[4], read_buf[3], read_buf[2], read_buf[1]);

    debug_log(DLOG_MASS, "%s: read  0x%02x [0x%02x 0x%02x 0x%02x 0x%02x] from Reg 0x%02x succeed.", obj_name, length,
        data->byte[0], data->byte[1], data->byte[2], data->byte[3], reg_offset);

    if (length != NV_REG_SIZE) {
        debug_log(DLOG_MASS, "%s: read length=0x%02x(expect=0x04) from Reg 0x%02x is invalid", obj_name, length,
            reg_offset);
    }

    return ret;
}


LOCAL gint32 write_nv_gpu_reg(OBJ_HANDLE gpu_handle, guint8 reg_offset, reg_t *data)
{
    gint32 ret = 0;
    guint8          write_buf[sizeof(smbpbi_data_t) + 1] = {0};
#ifndef BMC_RELEASE
    const gchar *obj_name = dfl_get_object_name(gpu_handle);
#endif
    write_buf[0] = NV_REG_SIZE;
    write_buf[1] = data->byte[0];
    write_buf[2] = data->byte[1];
    write_buf[3] = data->byte[2];
    write_buf[4] = data->byte[3];

    ret = dfl_block_write(gpu_handle, PROPERTY_GPU_CARD_DEV_ACCESSOR, reg_offset, sizeof(reg_t) + 1, write_buf);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: write data 0x%08x to 0x%02x failed. ret=%d", obj_name, data->word, reg_offset,
            ret);
        return ret;
    }

    debug_log(DLOG_MASS, "%s: write 0x%02x [0x%02x 0x%02x 0x%02x 0x%02x] to   Reg 0x%02x succeed.", obj_name,
        write_buf[0], write_buf[1], write_buf[2], write_buf[3], write_buf[4], reg_offset);

    return ret;
}


LOCAL gint32 smbus_postbox_intf(OBJ_HANDLE handle, reg_t cmd, reg_t *data_in, reg_t *data_out, reg_t *ex_data_out)
{
    gint32 ret = RET_OK;
    reg_t status_reg;
    guint8 status = CMD_STATUS_NULL;
    guint32 count = 0;
    guint32 data_reg_addr = NV_DATA_REG;
#ifndef BMC_RELEASE
    const gchar *obj_name = dfl_get_object_name(handle);
#endif

    // 根据xml配置的GPU类型确定DATA寄存器的地址
    data_reg_addr = get_data_reg_addr(handle);

    // 写入CMD寄存器之前加锁，本次邮箱存取操作结束后再解锁
    (void)pthread_mutex_lock(&smbpbi_mutex);

    // 若输入数据不为空，则将其写入到DATA寄存器
    if (data_in != NULL) {
        ret = write_nv_gpu_reg(handle, data_reg_addr, data_in);
        if (ret != RET_OK) {
            status = CMD_STATUS_ERR_I2C_ACCESS;
            goto EXIT;
        }
    }

    // 将命令数据写入到CMD寄存器
    ret = write_nv_gpu_reg(handle, NV_CMD_REG, &cmd);
    if (ret != RET_OK) {
        status = CMD_STATUS_ERR_I2C_ACCESS;
        goto EXIT;
    }

    vos_task_delay(100);

    // 轮询状态(命令)寄存器，直到获取到有效状态，最多尝试MAX_STATUS_POLLS次
    count = 0;

    do {
        ret = read_nv_gpu_reg(handle, NV_CMD_REG, &status_reg);
        if (ret != RET_OK) {
            status = CMD_STATUS_ERR_I2C_ACCESS;
            goto EXIT;
        }

        // 返回的数值，例如获取温度[0x04] [0x02 0x00 0x00 0x1f]，前三数值与输入命令一致，最后1字节为状态字
        status = status_reg.byte[3] & NV_GPU_STATUS_MASK;

        if (status != CMD_STATUS_NULL) {
            break;
        }

        count++;
        vos_task_delay(1000);
    } while ((status == CMD_STATUS_NULL) && (count < MAX_STATUS_POLLS)); // 状态字节为有效值时退出轮询

    debug_log(DLOG_DEBUG, "%s: GPU SMB(OpCode=0x%02x, Arg1=0x%02x, Arg2=0x%02x) status: %s(0x%02x) count: %d", obj_name,
        cmd.byte[0], cmd.byte[1], cmd.byte[2], transfer_status_to_string(status), status, count);

    // 若期望有输出，则读取数据寄存器，例如获取温度的返回数据: [0x04] [0x00 0x29 0x00 0x00]
    if ((data_out != NULL) && (status == CMD_STATUS_SUCCESS)) {
        ret = read_nv_gpu_reg(handle, data_reg_addr, data_out);
        if (ret != RET_OK) {
            status = CMD_STATUS_ERR_I2C_ACCESS;
            goto EXIT;
        }
    }

    if ((ex_data_out != NULL) && (status == CMD_STATUS_SUCCESS)) {
        data_reg_addr = data_reg_addr + 1;
        ret = read_nv_gpu_reg(handle, data_reg_addr, ex_data_out);
        if (ret != RET_OK) {
            status = CMD_STATUS_ERR_I2C_ACCESS;
            goto EXIT;
        }
    }

EXIT:
    // 邮箱存取操作结束，解锁
    (void)pthread_mutex_unlock(&smbpbi_mutex);

    return status;
}


LOCAL gint32 test_postbox_protocal(OBJ_HANDLE obj_handle)
{
    reg_t output;
    reg_t empty_cmd;

    empty_cmd.word = assemble_smbpbi_cmd(OPCODE_NULL_CMD, 0, 0);

    return smbus_postbox_intf(obj_handle, empty_cmd, NULL, &output, NULL);
}


LOCAL guint32 get_specific_cap_set(OBJ_HANDLE obj_handle, guint8 num, OBJ_HANDLE action_obj_handle)
{
    gint32 iRet = -1;
    gint32 ret = RET_OK;
    reg_t get_cap_cmd;
    reg_t cap_output;
    gchar    error_log_buf[LOG_MAX_SZ] = {0};

    if (num >= NV_CAPABILITY_NUM) {
        debug_log(DLOG_ERROR, "Wrong capability set num=%d", num);
        return 0;
    }

    cap_output.word = 0;

    get_cap_cmd.word = assemble_smbpbi_cmd(OPCODE_GET_CAP, num, 0);

    ret = smbus_postbox_intf(obj_handle, get_cap_cmd, NULL, &cap_output, NULL);
    if (ret == CMD_STATUS_SUCCESS) {
        return cap_output.word;
    }

    
    iRet = snprintf_s(error_log_buf, sizeof(error_log_buf), sizeof(error_log_buf) - 1,
        "%s: Get Capability%u failed:%s(0x%02x)\n", dfl_get_object_name(obj_handle), num,
        transfer_status_to_string(ret), ret);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }

    if (action_obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s", error_log_buf);
    } else {
        (void)gpu_action_error_log(action_obj_handle, error_log_buf, FALSE);
    }

    

    return 0;
}


LOCAL gint32 clear_gpu_action_data(OBJ_HANDLE action_handle)
{
    gint32 ret = RET_OK;
    guint8 data_type = 0;
    guint32 info_array[INVALID_NVLINKNUM] = {0};

    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_DATA_TYPE, &data_type);

    if (data_type == KEEP_ASCII || data_type == ARRAY_TO_ASCII_BIGENDIAN || data_type == ARRAY_TO_ASCII_LITTLEENDIAN) {
        ret = dal_set_property_value_string(action_handle, PROPERTY_NVGPU_DATA, INVALID_DATA_STRING, DF_NONE);
    } else if (data_type == KEEP_DIGITAL) {
        ret = dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA, HW_ACCESS_IN_UPDATE_PROGRESS, DF_NONE);
    } else if (data_type == KEEP_ARRAY) {
        ret = dal_set_property_value_array_uint32(action_handle, PROPERTY_NVGPU_DATA, info_array, INVALID_NVLINKNUM,
            DF_NONE);
    }

    return ret;
}


LOCAL gint32 access_fail_gpu_action_data(OBJ_HANDLE action_handle)
{
    gint32 ret = RET_OK;
    guint8 data_type = 0;
    guint32 info_array[INVALID_NVLINKNUM] = {0};

    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_DATA_TYPE, &data_type);

    
    if (data_type == KEEP_ASCII || data_type == ARRAY_TO_ASCII_BIGENDIAN || data_type == ARRAY_TO_ASCII_LITTLEENDIAN) {
        ret = dal_set_property_value_string(action_handle, PROPERTY_NVGPU_DATA, INVALID_DATA_STRING, DF_NONE);
    } else if (data_type == KEEP_DIGITAL) {
        ret = dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA, HW_ACCESS_FAIL_MASK, DF_NONE);
    } else if (data_type == KEEP_ARRAY) {
        ret = dal_set_property_value_array_uint32(action_handle, PROPERTY_NVGPU_DATA, info_array, INVALID_NVLINKNUM,
            DF_NONE);
    }

    return ret;
}


LOCAL guint32 get_digital_actual_value(guint32 raw_data, guint32 mask)
{
    guint8 offset = 0;
    guint32 mask_temp = mask;

    
    while (mask_temp) {
        if ((mask_temp & 1) == 1) {
            break;
        }
        mask_temp >>= 1;
        offset++;
    }

    return (raw_data & mask) >> offset;
}


LOCAL gint32 gpu_action_digital_process(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle)
{
    gint32 ret;
    gint32 nret;
    reg_t action_cmd;
    guint8 opode = 0;
    guint8 arg1 = 0;
    guint8 arg2 = 0;
    reg_t data_out;
    gchar  error_log_buf[LOG_MAX_SZ] = {0};
    gchar  action_name[MAX_NV_ACTION_NAME_LEN + 1] = {0};
    guint32 data_mask = 0xffffffff;
    guint32 action_data = 0;

    (void)dal_get_property_value_string(action_handle, PROPERTY_NVGPU_NAME, action_name, sizeof(action_name));
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_OPCODE, &opode);
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_ARG1, &arg1);
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_ARG2, &arg2);
    (void)dal_get_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA_MASK, &data_mask);

    action_cmd.word = assemble_smbpbi_cmd(opode, arg1, arg2);

    ret = smbus_postbox_intf(gpu_handle, action_cmd, NULL, &data_out, NULL);
    if (ret != CMD_STATUS_SUCCESS) {
        nret = snprintf_s(error_log_buf, sizeof(error_log_buf), sizeof(error_log_buf) - 1,
            "%s: Get %s failed. ret=0x%x", dfl_get_object_name(gpu_handle), action_name, ret);
        if (nret > 0) {
            (void)gpu_action_error_log(action_handle, error_log_buf, FALSE);
        }
        (void)dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA, HW_ACCESS_FAIL_MASK, DF_NONE);
        return VOS_ERR;
    }

    
    debug_log(DLOG_DEBUG, "%s: %s raw_data=0x%08x, data_mask=0x%08x", dfl_get_object_name(gpu_handle), action_name,
        data_out.word, data_mask);
    action_data = get_digital_actual_value(data_out.word, data_mask);
    debug_log(DLOG_DEBUG, "%s: %s=0x%08x=%d", dfl_get_object_name(gpu_handle), action_name, action_data, action_data);

    ret = dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA, action_data, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set %s for %s failed. ret=%d", dfl_get_object_name(gpu_handle), PROPERTY_NVGPU_DATA,
            action_name, ret);
    }

    return ret;
}


LOCAL guint8 ascii_data_fetch_time(OBJ_HANDLE action_handle)
{
    guint8 fetch_time;
    guint8 data_lenth = 0;

    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_DATA_LENGTH, &data_lenth);
    
    data_lenth = (data_lenth > NV_GPU_PROPERTY_LEN) ? NV_GPU_PROPERTY_LEN : data_lenth;

    
    if ((data_lenth % sizeof(reg_t)) == 0) {
        fetch_time = (guint8)(data_lenth / sizeof(reg_t)); // 整除时的次数
    } else {
        fetch_time = (guint8)(1 + data_lenth / sizeof(reg_t)); // 不整除时的次数
    }

    return fetch_time;
}


LOCAL void convert_ascii_to_string(guint8 data_type, reg_t data_out, gchar *action_data_str, guint8 action_data_size)
{
    gchar       tmp_buf[16] = {0}; // 用于拼接寄存器输出数据数组长度：16
    gint32 nret;
    errno_t safe_fun_ret;

    if (data_type == KEEP_ASCII) {
        tmp_buf[0] = data_out.byte[0];
        tmp_buf[1] = data_out.byte[1];
        tmp_buf[2] = data_out.byte[2];
        tmp_buf[3] = data_out.byte[3];
    } else if (data_type == ARRAY_TO_ASCII_BIGENDIAN) {
        nret = snprintf_s(tmp_buf, sizeof(tmp_buf), sizeof(tmp_buf) - 1, "%02x%02x%02x%02x", data_out.byte[0],
            data_out.byte[1], data_out.byte[2], data_out.byte[3]);
        if (nret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s falied.", __FUNCTION__);
            return;
        }
    } else if (data_type == ARRAY_TO_ASCII_LITTLEENDIAN) {
        nret = snprintf_s(tmp_buf, sizeof(tmp_buf), sizeof(tmp_buf) - 1, "%02x%02x%02x%02x", data_out.byte[3],
            data_out.byte[2], data_out.byte[1], data_out.byte[0]);
        if (nret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s falied.", __FUNCTION__);
            return;
        }
    }

    safe_fun_ret = strncat_s(action_data_str, action_data_size, tmp_buf, strlen(tmp_buf));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s failed.", __FUNCTION__);
    }

    return;
}


LOCAL gint32 gpu_action_ascii_process(guint8 data_type, OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle)
{
    guint8 fetch_time;
    gint32 nret;
    reg_t action_cmd;
    guint8 opode = 0;
    guint8 arg1 = 0;
    reg_t data_out;
    gint32 i;
    gint32 ret = VOS_OK;
    gchar       error_log_buf[LOG_MAX_SZ] = {0};
    gchar       action_name[MAX_NV_ACTION_NAME_LEN + 1] = {0};
    gchar       action_data_str[NV_GPU_PROPERTY_LEN + 1] = {0};

    (void)dal_get_property_value_string(action_handle, PROPERTY_NVGPU_NAME, action_name, sizeof(action_name));
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_OPCODE, &opode);
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_ARG1, &arg1);

    fetch_time = ascii_data_fetch_time(action_handle);

    for (i = 0; i < fetch_time; i++) {
        action_cmd.word = assemble_smbpbi_cmd(opode, arg1, i);

        ret = smbus_postbox_intf(gpu_handle, action_cmd, NULL, &data_out, NULL);
        if (ret != CMD_STATUS_SUCCESS) {
            nret = snprintf_s(error_log_buf, sizeof(error_log_buf), sizeof(error_log_buf) - 1,
                "%s: Get %s(Part%d) failed. ret=0x%x", dfl_get_object_name(gpu_handle), action_name, i, ret);
            if (nret > 0) {
                (void)gpu_action_error_log(action_handle, error_log_buf, FALSE);
            }
            return RET_ERR;
        }

        convert_ascii_to_string(data_type, data_out, action_data_str, sizeof(action_data_str));
    }

    debug_log(DLOG_DEBUG, "%s: %s=%s", dfl_get_object_name(gpu_handle), action_name, action_data_str);
    if (g_utf8_validate (action_data_str, -1, NULL) != TRUE) {
        debug_log(DLOG_ERROR, "validate utf8 failed");
        return RET_ERR;
    }

    if (strlen(action_data_str) > 0) {
        ret = dal_set_property_value_string(action_handle, PROPERTY_NVGPU_DATA, action_data_str, DF_NONE);
    } else {
        ret = dal_set_property_value_string(action_handle, PROPERTY_NVGPU_DATA, INVALID_DATA_STRING, DF_NONE);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Set %s for %s failed. ret=%d", dfl_get_object_name(gpu_handle), PROPERTY_NVGPU_DATA,
            action_name, ret);
    }

    return ret;
}


LOCAL guint8 nvlink_info_fetch_time(guint32 nvlink_num, guint32 item_bits)
{
    guint32 read_bits;
    guint8 fetch_time;

    read_bits = nvlink_num * item_bits;
    if ((read_bits % BITS_PER_READ) == 0) {
        fetch_time = read_bits / BITS_PER_READ; // 整除时的次数
    } else {
        fetch_time = 1 + read_bits / BITS_PER_READ; // 不整除时的次数
    }

    return fetch_time;
}


LOCAL void convert_nvlink_data_to_array(reg_t data_out, guint32 *info_array_offset, guint32 item_bits,
    guint32 *info_array, guint32 nvlink_num)
{
    guint8 reg_offset = 0;
    guint32 data_mask;

    data_mask = (0xffffffff) >> (REG_BIT_WIDTH - item_bits);

    while (reg_offset < REG_BIT_WIDTH) {
        if (*info_array_offset < nvlink_num) {
            info_array[(*info_array_offset)++] = (data_out.word >> reg_offset) & data_mask;
            reg_offset += item_bits;
        } else {
            break;
        }
    }

    return;
}


LOCAL gint32 gpu_action_array_process(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle)
{
    guint32 nvlink_num = 0;
    guint32 item_bits = 0;
    guint32 *info_array = NULL;
    gchar       action_name[MAX_NV_ACTION_NAME_LEN + 1] = {0};
    reg_t action_cmd;
    guint8 opode = 0;
    guint8 arg1 = 0;
    gint32 ret, nret;
    reg_t data_out, ex_data_out;
    gchar       error_log_buf[LOG_MAX_SZ] = {0};
    guint32 info_array_offset = 0;

    (void)dal_get_property_value_string(action_handle, PROPERTY_NVGPU_NAME, action_name, sizeof(action_name));
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_OPCODE, &opode);
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_ARG1, &arg1);
    (void)dal_get_property_value_uint32(gpu_handle, PROPERTY_GPU_CARD_NVLINK_NUM, &nvlink_num);
    (void)dal_get_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA_LENGTH, &item_bits);

    if ((nvlink_num == 0) || (nvlink_num == HW_ACCESS_FAIL_MASK) || (nvlink_num == HW_ACCESS_IN_UPDATE_PROGRESS) ||
        (nvlink_num > MAX_NVLINKNUM)) {
        debug_log(DLOG_DEBUG, "%s: nvlink_num is invalid: %u", __FUNCTION__, nvlink_num);
        return VOS_OK;
    }

    info_array = (guint32 *)g_malloc0(nvlink_num * sizeof(guint32));
    if (info_array == NULL) {
        return VOS_ERR;
    }

    guint8 fetch_time = nvlink_info_fetch_time(nvlink_num, item_bits);
    for (guint8 i = 0; i < fetch_time; i++) {
        action_cmd.word = assemble_smbpbi_cmd(opode, arg1, i);

        ret = smbus_postbox_intf(gpu_handle, action_cmd, NULL, &data_out, &ex_data_out);
        if (ret != CMD_STATUS_SUCCESS) {
            nret = snprintf_s(error_log_buf, sizeof(error_log_buf), sizeof(error_log_buf) - 1,
                "%s: Get %s(Part%u) failed. ret=0x%x", dfl_get_object_name(gpu_handle), action_name, i, ret);
            if (nret > 0) {
                (void)gpu_action_error_log(action_handle, error_log_buf, FALSE);
            }
            (void)dal_set_property_value_array_uint32(action_handle, PROPERTY_NVGPU_DATA, info_array, INVALID_NVLINKNUM,
                DF_NONE);

            g_free(info_array);
            return VOS_ERR;
        }

        debug_log(DLOG_DEBUG, "%s: %s item_bits=%u, info_array_offset=%u, raw_data=0x%08x, ex_raw_data=0x%08x",
            dfl_get_object_name(gpu_handle), action_name, item_bits, info_array_offset, data_out.word,
            ex_data_out.word);

        convert_nvlink_data_to_array(data_out, &info_array_offset, item_bits, info_array, nvlink_num);
        convert_nvlink_data_to_array(ex_data_out, &info_array_offset, item_bits, info_array, nvlink_num);
    }

    (void)dal_set_property_value_array_uint32(action_handle, PROPERTY_NVGPU_DATA, info_array, nvlink_num, DF_NONE);

    g_free(info_array);

    return VOS_OK;
}


LOCAL gint32 get_gpu_action_data(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle)
{
    gint32 ret = RET_OK;
    gchar       action_name[MAX_NV_ACTION_NAME_LEN + 1] = {0};
    guint8 cap_num = 0;
    guint32 capability = 0;
    guint32 cap_mask = 0;
    guint8 data_type = 0;

    (void)dal_get_property_value_string(action_handle, PROPERTY_NVGPU_NAME, action_name, sizeof(action_name));
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_CAP_DWORD_NUM, &cap_num);
    (void)dal_get_property_value_uint32(action_handle, PROPERTY_NVGPU_CAP_MASK, &cap_mask);
    (void)dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_DATA_TYPE, &data_type);

    
    capability = get_specific_cap_set(gpu_handle, cap_num, action_handle);
    if (cap_mask != (capability & cap_mask)) {
        debug_log(DLOG_DEBUG, "%s: %s not supported. Capability%d=0x%08x", dfl_get_object_name(gpu_handle), action_name,
            cap_num, capability);
        ret = clear_gpu_action_data(action_handle);
        return ret;
    }

    switch (data_type) {
        case KEEP_DIGITAL:
            ret = gpu_action_digital_process(gpu_handle, action_handle);
            break;

        case KEEP_ASCII:
        case ARRAY_TO_ASCII_BIGENDIAN:
        case ARRAY_TO_ASCII_LITTLEENDIAN:
            ret = gpu_action_ascii_process(data_type, gpu_handle, action_handle);
            break;

        case KEEP_ARRAY:
            ret = gpu_action_array_process(gpu_handle, action_handle);
            break;

        default:
            break;
    }

    return ret;
}




LOCAL void gpu_action_error_log(OBJ_HANDLE gpu_action_handle, gchar *error_log, guchar refresh_time_flag)
{
    guint32 log_count = 0;
    guint32 current_time = 0;
    guint32 last_log_time = 0;

    if (error_log == NULL) {
        return;
    }
    current_time = vos_tick_get() & 0xffffffff;

    debug_log(DLOG_MASS, "current_time:%u %s", current_time, error_log);

    (void)dal_get_property_value_uint32(gpu_action_handle, PROPERTY_NVGPU_LOG_COUNT, &log_count);

    if (log_count == 0) {
        debug_log(DLOG_ERROR, "%s", error_log);
        if (refresh_time_flag == TRUE) {
            (void)dal_set_property_value_uint32(gpu_action_handle, PROPERTY_NVGPU_LAST_LOG_TIME, current_time, DF_NONE);
        }
        return;
    }

    (void)dal_get_property_value_uint32(gpu_action_handle, PROPERTY_NVGPU_LAST_LOG_TIME, &last_log_time);

    if ((current_time > last_log_time && (current_time - last_log_time) > GPU_ERR_LOG_INTERVAL) ||
        (last_log_time > current_time && (last_log_time - current_time) > GPU_ERR_LOG_INTERVAL)) {
        debug_log(DLOG_ERROR, "%s", error_log);
        if (refresh_time_flag == TRUE) {
            (void)dal_set_property_value_uint32(gpu_action_handle, PROPERTY_NVGPU_LAST_LOG_TIME, current_time, DF_NONE);
        }
        return;
    }

    return;
}

static gint32 get_smbios_status(guint8 *status)
{
    OBJ_HANDLE smbios_handle = 0;
    gint32 ret;
    guint8 smbios_status = 0;

    ret = dfl_get_object_handle(CLASS_NAME_SMBIOS, &smbios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get CLASS_NAME_SMBIOS failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(smbios_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PROPERTY_SMBIOS_STATUS_VALUE failed", __FUNCTION__);
        return RET_ERR;
    }

    *status = smbios_status;

    return RET_OK;
}
LOCAL void get_nv_gpu_action_data(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle, const gchar *action_name)
{
    gint32 ret;
    guint32 log_count = 0;
    gchar error_log_buf[LOG_MAX_SZ] = {0};

    
    ret = test_postbox_protocal(gpu_handle);
    if ((ret != CMD_STATUS_SUCCESS) && (ret != CMD_STATUS_READY)) {
        (void)access_fail_gpu_action_data(action_handle);
        return;
    }

    
    ret = get_gpu_action_data(gpu_handle, action_handle);
    if (ret == RET_OK) {
        (void)dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_LOG_COUNT, 0, DF_NONE);
        return;
    }

    (void)dal_get_property_value_uint32(action_handle, PROPERTY_NVGPU_LOG_COUNT, &log_count);
    (void)memset_s(error_log_buf, sizeof(error_log_buf), 0, sizeof(error_log_buf));
    ret = snprintf_s(error_log_buf, sizeof(error_log_buf), sizeof(error_log_buf) - 1,
        "%s: Get %s failed. ret=%d  error count=%d \n", dfl_get_object_name(gpu_handle), action_name, ret,
        log_count + 1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    (void)gpu_action_error_log(action_handle, error_log_buf, TRUE);

    log_count = log_count + 1;
    (void)dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_LOG_COUNT, log_count, DF_NONE);
    return;
}


LOCAL gint32 check_is_vastai(OBJ_HANDLE gpu_handle)
{
    guint16 vender_id = 0;
    OBJ_HANDLE pcie_handle = 0;

    
    gint32 ret = dal_get_specific_object_position(gpu_handle, CLASS_PCIE_CARD, &pcie_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PcieCard object failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_uint16(pcie_handle, PROPERTY_PCIE_CARD_VID, &vender_id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PcieCard vender_id failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (vender_id != VASTAI_VID) {
        debug_log(DLOG_INFO, "%s: not vastai gpu card.", __FUNCTION__);
        return RET_ERR;
    }
    
    return RET_OK;
}


LOCAL gint32 vastai_verify_data(guint8 *data_in, guint32 data_len)
{
    guint32 i;
    guint8 check_sum = 0;
    
    
    for (i = 0; i < data_len; i++) {
        debug_log(DLOG_MASS, "%s: data_in[%u] = %u.", __FUNCTION__, i, data_in[i]);
        check_sum += data_in[i];
    }
    
    if (((guint8)(check_sum + 1)) != 0) {
        debug_log(DLOG_INFO, "%s: verify data check sum failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_vastai_gpu_data(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle, guint8 *length, guint8 **read_buf)
{
    gint32 ret;
    guint8 offset = 0;

    
    ret = dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_ARG1, &offset);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s offset failed. ret=%d",
            __FUNCTION__, dfl_get_object_name(action_handle), ret);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_DATA_LENGTH, length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s length failed. ret=%d",
            __FUNCTION__, dfl_get_object_name(action_handle), ret);
        return RET_ERR;
    }

    if (*length == 0) {
        debug_log(DLOG_ERROR, "%s: length error.", __FUNCTION__);
        return RET_ERR;
    }

    *read_buf = g_malloc0(*length);
    if ((*read_buf) == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = dfl_block_read(gpu_handle, PROPERTY_GPU_CARD_DEV_ACCESSOR, offset, *length, (*read_buf));
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: %s read data from 0x%02x failed. ret=%d",
            __FUNCTION__, dfl_get_object_name(gpu_handle), offset, ret);
        return RET_ERR;
    }

    
    ret = vastai_verify_data((*read_buf), *length);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "%s: verify_and_trans_data failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void get_vastai_gpu_digital_data(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle, const gchar *action_name)
{
#define OFFSET_8 8
    gint32 ret;
    guint8 length = 0;
    guint8 *read_buf = NULL;
    guint32 action_data = HW_ACCESS_FAIL_MASK;

    ret = get_vastai_gpu_data(gpu_handle, action_handle, &length, &read_buf);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s data failed.", __FUNCTION__, dfl_get_object_name(action_handle));
        goto err;
    }

    
    action_data = 0;
    for (int i = 1; i < length; i++) {
        action_data =
            (guint32)(((guint32)(action_data)) | (((guint32)((guint8)(read_buf[i]))) << (OFFSET_8 * (i - 1))));
    }
    debug_log(DLOG_DEBUG, "%s: %s=0x%08x=%d", __FUNCTION__, action_name, action_data, action_data);

err:
    ret = dal_set_property_value_uint32(action_handle, PROPERTY_NVGPU_DATA, action_data, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set %s property failed. ret=%d",
            __FUNCTION__, dfl_get_object_name(action_handle), ret);
    }
    
    if (read_buf != NULL) {
        g_free(read_buf);
    }

    return;
}


LOCAL void get_vastai_gpu_ascii_data(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle, const gchar *action_name)
{
    gint32 ret;
    gint32 ret_val = RET_OK;
    guint8 length = 0;
    guint8 *read_buf = NULL;
    gchar action_data[NV_GPU_PROPERTY_LEN] = {0};

    ret = get_vastai_gpu_data(gpu_handle, action_handle, &length, &read_buf);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s data failed.", __FUNCTION__, dfl_get_object_name(action_handle));
        ret_val = RET_ERR;
        goto err;
    }
    
    ret = snprintf_s(action_data, sizeof(action_data), sizeof(action_data) - 1, "%s", read_buf + 1);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s falied. ret = %d", __FUNCTION__, ret);
        ret_val = RET_ERR;
        goto err;
    }
    debug_log(DLOG_DEBUG, "%s: %s data(%s)", __FUNCTION__, action_name, action_data);

err:
    if (ret_val == RET_OK) {
        ret = dal_set_property_value_string(action_handle, PROPERTY_NVGPU_DATA, action_data, DF_NONE);
    } else {
        ret = dal_set_property_value_string(action_handle, PROPERTY_NVGPU_DATA, INVALID_DATA_STRING, DF_NONE);
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set %s data failed. ret=%d",
            __FUNCTION__, dfl_get_object_name(action_handle), ret);
    }

    if (read_buf != NULL) {
        g_free(read_buf);
    }

    return;
}


LOCAL void get_vastai_gpu_action_data(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle, const gchar *action_name)
{
    gint32 ret;
    guint8 data_type = 0;

    ret = dal_get_property_value_byte(action_handle, PROPERTY_NVGPU_DATA_TYPE, &data_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s DataType failed. ret=%d",
            __FUNCTION__, dfl_get_object_name(action_handle), ret);
        return;
    }
    
    switch (data_type) {
        case KEEP_DIGITAL:
            get_vastai_gpu_digital_data(gpu_handle, action_handle, action_name);
            break;

        case KEEP_ASCII:
            get_vastai_gpu_ascii_data(gpu_handle, action_handle, action_name);
            break;

        default:
            debug_log(DLOG_ERROR, "%s: %s DataType(%u) out of range.", __FUNCTION__, action_name, data_type);
            break;
    }
    
    return;
}


LOCAL void get_data_by_gpu_action(OBJ_HANDLE gpu_handle, OBJ_HANDLE action_handle, const gchar *action_name)
{
    // 如果是瀚博GPU，执行瀚博的流程
    if (check_is_vastai(gpu_handle) == RET_OK) {
        get_vastai_gpu_action_data(gpu_handle, action_handle, action_name);
        return;
    }

    get_nv_gpu_action_data(gpu_handle, action_handle, action_name);
    return;
}


LOCAL void nvidia_action_task(gpointer data)
{
    gint32 ret = 0;
    guint8 power_state = 0;
    GSList *list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE action_handle;
    OBJ_HANDLE gpu_handle;
    gchar           action_name[MAX_NV_ACTION_NAME_LEN + 1] = {0};
    guint8 smbios_status = SMBIOS_WRITE_NOT_START;
    (void)prctl(PR_SET_NAME, (gulong) "NvidiaTask");

    
    debug_log(DLOG_DEBUG, "%s:nvidia_action_task started.", __FUNCTION__);
    

    while (1) {
        
        ret = dfl_get_object_list(CLASS_NVGPU_ATCION, &list);
        if ((ret != DFL_OK) || (list == NULL)) {
            debug_log(DLOG_DEBUG, "%s: No %s object found", __FUNCTION__, CLASS_NVGPU_ATCION);
            vos_task_delay(30000);
            continue;
        }

        
        for (obj_node = list; obj_node; obj_node = obj_node->next) {
            action_handle = (OBJ_HANDLE)obj_node->data;

            
            ret = dal_get_property_value_string(action_handle, PROPERTY_NVGPU_NAME, action_name, sizeof(action_name));
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get name for %s failed. ret=%d", dfl_get_object_name(action_handle), ret);
                continue;
            }

            
            if (dfl_get_referenced_object(action_handle, PROPERTY_NVGPU_REFDEV, &gpu_handle) != DFL_OK) {
                ret = dal_get_specific_object_position(action_handle, CLASS_GPU_CARD, &gpu_handle);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Get GPUCard object for %s failed. ret=%d", action_name, ret);
                    continue;
                }
            }

            
            ret = dal_get_property_value_byte(gpu_handle, PROPERTY_GPU_CARD_POWER_STATE, &power_state);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Get power state failed. ret=%d.", dfl_get_object_name(gpu_handle), ret);
                continue;
            }

            ret = get_smbios_status(&smbios_status);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s: Get smbios_status failed. ret=%d.", dfl_get_object_name(gpu_handle), ret);
                continue;
            }

            
            if ((power_state == 0) || (smbios_status != SMBIOS_WRITE_FINISH)) {
                (void)clear_gpu_action_data(action_handle);
                continue;
            }
            get_data_by_gpu_action(gpu_handle, action_handle, action_name);
            
        }

        g_slist_free(list);
        list = NULL;
        vos_task_delay(DELAY_ONE_SECOND);
    }
}


gint32 gpu_card_add_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("GPU Card", object_handle, PROPERTY_GPU_CARD_SLOT, NULL, CARD_PLUG_IN);
    return RET_OK;
}


gint32 gpu_card_del_object_callback(OBJ_HANDLE object_handle)
{
    card_manage_log_operation_log("GPU Card", object_handle, PROPERTY_GPU_CARD_SLOT, NULL, CARD_PLUG_OUT);
    return RET_OK;
}


gint32 gpu_card_dump_info(const gchar *path)
{
    gint32 iRet = -1;
    gint32 ret = 0;
    guint16 board_id = 0;
    const gchar *name = NULL;
    const gchar *ser_num = NULL;
    const gchar *firm_ver = NULL;
    const gchar *manufacturer = NULL;
    guint8 gpu_type = 0;
    guint32 nv_gpu_cap[NV_CAPABILITY_NUM] = {0};
    guint8 cap_index = 0;
    ecc_t retired_page;
    gchar file_name[GPU_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar gpu_card_info[GPU_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;

    guint8 slot_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    size_t fwrite_back = 0;
    retired_page.sbe = 0;
    retired_page.dbe = 0;

    if (path == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_GPU_CARD, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }

        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    iRet = snprintf_s(file_name, GPU_CARD_DUMPINFO_MAX_LEN + 1, GPU_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    (void)snprintf_s(gpu_card_info, sizeof(gpu_card_info), sizeof(gpu_card_info) - 1, "%s", "GPU Card Info\n");
    fwrite_back = fwrite(gpu_card_info, strlen(gpu_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    (void)memset_s(gpu_card_info, GPU_CARD_DUMPINFO_MAX_LEN + 1, 0, GPU_CARD_DUMPINFO_MAX_LEN + 1);
    (void)snprintf_s(gpu_card_info, GPU_CARD_DUMPINFO_MAX_LEN + 1, GPU_CARD_DUMPINFO_MAX_LEN,
        "%-4s | %-6s |%-2s | %-20s | %-20s | %-16s | %-16s | %-10s | %-10s | %-10s | %-10s | %-4s | %s\n", "Slot",
        "BoardId", "GPUType", "Name", "Manufacturer", "SerialNum", "FirmVer", "Capab0", "Capab1", "Capab2", "Capab3",
        "SBE", "DBE");
    fwrite_back = fwrite(gpu_card_info, strlen(gpu_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_BOARDID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_NAME);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_MANU);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_SER_NUM);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_FIRM_VER);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_GPUTYPE);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_SBE_COUNT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_GPU_CARD_DBE_COUNT);

        ret = dfl_multiget_property_value((OBJ_HANDLE)obj_note->data, property_name_list, &property_value);
        g_slist_free(property_name_list);
        property_name_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get GPU Card information failed!");
            g_slist_free(obj_list);
            (void)fclose(fp);
            return RET_ERR;
        }

        slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        board_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 2), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        ser_num = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0);
        firm_ver = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 5), 0);
        gpu_type = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 6));
        retired_page.sbe = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 7));
        retired_page.dbe = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 8));

        (void)memset_s(gpu_card_info, GPU_CARD_DUMPINFO_MAX_LEN + 1, 0, GPU_CARD_DUMPINFO_MAX_LEN + 1);

        // Nvidia GPU还需收集Capability属性
        if ((gpu_type == NV_GPU_TYPE_FERMI) || (gpu_type == NV_GPU_TYPE_KEPLER) || (gpu_type == NV_GPU_TYPE_PASCAL) ||
            (gpu_type == NV_GPU_TYPE_VOLTA)) {
            for (cap_index = 0; cap_index < NV_CAPABILITY_NUM; cap_index++) {
                nv_gpu_cap[cap_index] = get_specific_cap_set((OBJ_HANDLE)obj_note->data, cap_index, 0);
            }

            (void)snprintf_s(gpu_card_info, sizeof(gpu_card_info), sizeof(gpu_card_info) - 1,
                "%u | 0x%-5x |%u | %-20s | %-20s | %-16s | %-16s | 0x%-8x | 0x%-8x | 0x%-8x | 0x%-8x | %u | %u\n",
                slot_id, board_id, gpu_type, name, manufacturer, ser_num, firm_ver, nv_gpu_cap[0], nv_gpu_cap[1],
                nv_gpu_cap[2], nv_gpu_cap[3], retired_page.sbe, retired_page.dbe);
        } else {
            iRet = snprintf_s(gpu_card_info, sizeof(gpu_card_info), sizeof(gpu_card_info) - 1,
                "%-4u | 0x%-5x |%-7u | %-20s | %-20s | %-16s | %s\n", slot_id, board_id, gpu_type, name, manufacturer,
                ser_num, firm_ver);
            if (iRet <= 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
            }
        }

        fwrite_back = fwrite(gpu_card_info, strlen(gpu_card_info), 1, fp);
        if (fwrite_back != 1) {
            (void)fclose(fp);
            g_slist_free(obj_list);
            g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_ERROR, "fwrite failed!");
            return RET_ERR;
        }

        (void)memset_s(gpu_card_info, GPU_CARD_DUMPINFO_MAX_LEN + 1, 0, GPU_CARD_DUMPINFO_MAX_LEN + 1);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_value = NULL;
    }

    g_slist_free(obj_list);
    (void)snprintf_s(gpu_card_info, sizeof(gpu_card_info), sizeof(gpu_card_info) - 1, "%s", "\n\n");
    fwrite_back = fwrite(gpu_card_info, strlen(gpu_card_info), 1, fp);
    if (fwrite_back != 1) {
        (void)fclose(fp);
        debug_log(DLOG_ERROR, "fwrite failed!");
        return RET_ERR;
    }
    (void)memset_s(gpu_card_info, GPU_CARD_DUMPINFO_MAX_LEN + 1, 0, GPU_CARD_DUMPINFO_MAX_LEN + 1);

    // 关闭文件
    (void)fclose(fp);

    return RET_OK;
}


gint32 gpu_card_start(void)
{
    gint32 ret = RET_OK;
    TASKID nv_gpu_action_task_id = 0;

    
    OBJ_HANDLE computer_handle = 0;
    if (dal_get_object_handle_nth(CLASS_COMPUTER_PARTITION, 0, &computer_handle) == RET_OK) {
        return RET_OK;
    }
    

    
    ret = vos_task_create(&nv_gpu_action_task_id, "Nvidia_Action_Task", (TASK_ENTRY)nvidia_action_task, NULL,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Nvidia_Action_Task create failed, ret=%d", __FUNCTION__, ret);
    }

    debug_log(DLOG_DEBUG, "%s:Nvidia_Action_Task created.", __FUNCTION__);

    return RET_OK;
}
