

#include "bdf.h"
#include "smbios.h"

#define BDF_MAX_FRAME_NUM 1
#define BDF_CURRENT_FRAME 2
#define BDF_FRAME_PCIE_NUM 3
#define BDF_FIRST_FRAME 1

#define BIOS_PCIEDISK_BDF_ABNORMAL 0x00
#define BIOS_IPMI_CMD_PCIECARD 0xC6
#define BIOS_IPMI_CMD_PCIEDISK 0xC8
#define BIOS_IPMI_CMD_OCPCARD 0xCC
#define BIOS_IPMI_CMD_PORT_ROOT_BDF      0xD1
#define BIOS_IPMI_CMD_PCIE_PORT_ROOT     0xD0
#define BDF_DATA_SIZE 2048


LOCAL BIOS_DATA_OPERATE_S g_bdf_wr_data_cfg = { 0 };

LOCAL gboolean filter_msg_by_sys_channel(const void *req_msg);
LOCAL gint32 bdf_write_first_frame(const void *request_msg, guint8 *resp_data, guint32 resp_data_size);
LOCAL gint32 bdf_write_data(const void *request_msg, guint8 *resp_data, guint size);
LOCAL gint32 bdf_write_finish(const void *request_msg, guint8 *resp_data, guint32 size);
LOCAL gint32 bios_set_bdf_property_value(OBJ_HANDLE object_handle, guint8 type, GVariant *property_data);

LOCAL GMutex g_bdf_comm_mutex;
void init_bdf_comm_mutex(void)
{
    g_mutex_init(&g_bdf_comm_mutex);
}


LOCAL gint32 set_bdf_to_mem(const void *req_msg, guint8 *resp_data, const guint8 *src_data, guint32 data_real_len,
    guint32 resp_data_len)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    GVariant *property_data = NULL;

    
    ret = dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Class Bios obj fail ret_val = %d", ret);
        goto EXIT;
    }
    
    property_data = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, (src_data + 4), data_real_len, sizeof(guint8));
    if (property_data == NULL) {
        debug_log(DLOG_ERROR, "Get property_data as NULL! ret_val = %d", ret);
        goto EXIT;
    }

    ret = bios_set_bdf_property_value(obj_handle, src_data[0], property_data);
    g_variant_unref(property_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set PROPERTY_BIOS_PCIECARD_BDF fail ret_val = %d", ret);
        goto EXIT;
    }
    
    resp_data[0] = BIOS_ERR_NORMALLY;
    return respmsg_with_customermanuid(req_msg, resp_data, resp_data_len, BIOS_MSG_HEAD_MIN_LEN);
EXIT:
    resp_data[0] = BIOS_ERR_INVALID_STATUS;
    return respmsg_with_customermanuid(req_msg, resp_data, resp_data_len, BIOS_MSG_HEAD_MIN_LEN);
}

LOCAL gint32 bios_write_bdf_frame(guint8 frame_num, guint8 max_frame, const void *req_msg, guint8 *resp_data,
    guint32 resp_data_len)
{
    if (frame_num == BDF_FIRST_FRAME) {
        // bdf 写第一帧
        return bdf_write_first_frame(req_msg, resp_data, resp_data_len);
    } else if (frame_num < max_frame) {
        // bdf 写中间帧
        return bdf_write_data(req_msg, resp_data, resp_data_len);
    } else if (frame_num == max_frame) {
        // bdf 写最后一帧
        return bdf_write_finish(req_msg, resp_data, resp_data_len);
    } else {
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(req_msg, resp_data, resp_data_len, BIOS_MSG_HEAD_MIN_LEN);
    }
}

gint32 bios_write_bdf_data_to_bmc(const void *req_msg, gpointer user_data)
{
    const guint8 *src_data = NULL;
    guint8 resp_data[10]; // 默认响应消息长度为10
    guint8 frame_num;
    guint8 max_frame;
    guint32 data_real_len;
    guint8 format_type;
    gint32 ipmi_ret;
    guint8 src_data_len;
    guint8 bdf_len = 5; // 普通bdf长度为5

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    src_data = get_ipmi_src_data(req_msg);
    src_data_len = get_ipmi_src_data_len(req_msg);
    if (filter_msg_by_sys_channel(req_msg)) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_STATUS_INVALID);
    }

    // IPMI 命名注册最短长度为2,此处数组下标直接访问到3,故此处作校验
    if (src_data == NULL || src_data_len < BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        ipmi_ret = ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
        return ipmi_ret;
    }
    if (src_data[0] == BIOS_IPMI_CMD_PORT_ROOT_BDF) {
        bdf_len = 6; // 6,0xd1接口比普通bdf接口多一个字段
    }
    max_frame = (src_data[BDF_MAX_FRAME_NUM]) & 0x7F;
    frame_num = src_data[BDF_CURRENT_FRAME] & 0x7F;
    data_real_len = src_data[BDF_FRAME_PCIE_NUM] * bdf_len;
    
    format_type = src_data[BDF_CURRENT_FRAME] & 0x80;

    
    if ((src_data[0] == BIOS_IPMI_CMD_PCIEDISK) && (format_type == BIOS_PCIEDISK_BDF_ABNORMAL)) {
        debug_log(DLOG_ERROR, "The current version of the BIOS does not support the device.");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(req_msg, resp_data, sizeof(resp_data), BIOS_MSG_HEAD_MIN_LEN);
    }

    

    debug_log(DLOG_ERROR, "sht bios_write_bdf_data_to_bmc max_frame=%d frame_num = %d", max_frame, frame_num);

    
    // 如果上报的是PCIe卡信息并且是最后一帧，设置BiosBootStage属性值为PCIe卡信息上报完成
    // 9032不会BIOS启动过程中不会上报VIDDID，但是PCIe卡的BDF信息还是会上报
    if ((src_data[0] == BIOS_IPMI_CMD_PCIECARD) && (frame_num == max_frame)) {
        (void)bios_set_boot_stage(BIOS_BOOT_STAGE_PCIE_INFO_REPORTED);
        debug_log(DLOG_ERROR, "%s: pcie bdf reported, set bios boot stage completed\n", __FUNCTION__);
    }

    
    if (max_frame == BDF_FIRST_FRAME && frame_num == BDF_FIRST_FRAME) {
        return set_bdf_to_mem(req_msg, resp_data, src_data, data_real_len, sizeof(resp_data));
    } else {
        g_mutex_lock(&g_bdf_comm_mutex);
        gint32 ret_val = bios_write_bdf_frame(frame_num, max_frame, req_msg, resp_data, sizeof(resp_data));
        g_mutex_unlock(&g_bdf_comm_mutex);
        return ret_val;
    }
}


LOCAL gboolean filter_msg_by_sys_channel(const void *req_msg)
{
    if (req_msg == NULL) {
        return TRUE;
    }
    gint32 channel_number;
    channel_number = get_ipmi_chan_num(req_msg);
    if (channel_number == SYS_CHAN_NUM || channel_number == CPLDRAM_CHAN_NUM) {
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 bdf_write_first_frame(const void *request_msg, guint8 *resp_data, guint32 resp_data_size)
{
    BIOS_DATA_OPERATE_S *data_operate = &g_bdf_wr_data_cfg;
    guint32 i;
    guint32 data_real_len;
    const guint8 *src_data = NULL;
    guint8 src_data_len;
    guint8 bdf_len = 5; // 普通bdf长度为5

    if (request_msg == NULL || resp_data == NULL) {
        return RET_ERR;
    }
    if (resp_data_size < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    data_operate->data_flag = BIOS_FLAG_DOING;
    data_operate->data_offset = 0;

    
    if (data_operate->data_buf != NULL) {
        g_free(data_operate->data_buf);
        data_operate->data_buf = NULL;
    }

    data_operate->data_buf = (guint8 *)g_malloc0(BDF_DATA_SIZE);

    if (data_operate->data_buf == NULL) {
        data_operate->data_flag = BIOS_FLAG_IDLE;
        debug_log(DLOG_ERROR, "g_malloc0 len:%d fail!", BDF_DATA_SIZE);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(request_msg, resp_data, resp_data_size, BIOS_MSG_HEAD_MIN_LEN);
    }

    src_data = get_ipmi_src_data(request_msg);
    src_data_len = get_ipmi_src_data_len(request_msg);
    if (src_data == NULL || src_data_len < BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, "src_data is invalid!");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(request_msg, resp_data, resp_data_size, BIOS_MSG_HEAD_MIN_LEN);
    }
    if (src_data[0] == BIOS_IPMI_CMD_PORT_ROOT_BDF) {
        bdf_len = 6; // 6,0xd1接口比普通bdf接口多一个字段
    }
    data_real_len = src_data[3] * bdf_len; // data 3为bdf数量
    if (data_real_len > (guint32)src_data_len - BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s: data real len (%u) is out of range", __FUNCTION__, data_real_len);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(request_msg, resp_data, resp_data_size, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    for (i = 0; i < data_real_len; i++) {
        *(data_operate->data_buf + data_operate->data_offset + i) = src_data[4 + i]; // 从data 4开始为bdf数据
    }

    data_operate->data_offset += data_real_len;
    data_operate->data_len = src_data[2]; // 2 current frame number

    resp_data[0] = BIOS_ERR_NORMALLY;

    return respmsg_with_customermanuid(request_msg, resp_data, resp_data_size, BIOS_MSG_HEAD_MIN_LEN);
}


LOCAL gint32 bdf_write_data(const void *request_msg, guint8 *resp_data, guint size)
{
    BIOS_DATA_OPERATE_S *data_operate = &g_bdf_wr_data_cfg;
    guint32 i;
    guint32 data_real_len;
    guint8 src_data_len;
    guint8 bdf_len = 5; // 普通bdf长度为5

    if (request_msg == NULL || resp_data == NULL) {
        return RET_ERR;
    } else if (size < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    if (BIOS_FLAG_DOING != data_operate->data_flag) {
        debug_log(DLOG_ERROR, "DataFlag:%d(ShouldBe:%d) is invalid!", data_operate->data_flag, BIOS_FLAG_IDLE);
        goto EXIT;
    }
    const guint8 *src_data = NULL;
    src_data = get_ipmi_src_data(request_msg);
    src_data_len = get_ipmi_src_data_len(request_msg);
    if (src_data == NULL || src_data_len < BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, "src_data is invalid!");
        goto EXIT;
    }

    if (data_operate->data_len + 1 != src_data[BDF_CURRENT_FRAME]) {
        resp_data[0] = BIOS_ERR_WRONG_OFFSET;
        return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
    }
    if (src_data[0] == BIOS_IPMI_CMD_PORT_ROOT_BDF) {
        bdf_len = 6; // 6,0xd1接口比普通bdf接口多一个字段
    }
    data_real_len = src_data[BDF_FRAME_PCIE_NUM] * bdf_len;
    if (data_real_len > (guint32)src_data_len - BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s: data real len (%u) is out of range", __FUNCTION__, data_real_len);
        goto EXIT;
    }

    if (data_operate->data_offset + data_real_len > BDF_DATA_SIZE) {
        debug_log(DLOG_ERROR, "RealLen:%d(Max:%d,DataOffset:%d) is invalid!", data_real_len, BDF_DATA_SIZE,
            data_operate->data_offset);

        resp_data[0] = BIOS_ERR_IV_LEN;
        return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
    }

    
    for (i = 0; (i < data_real_len) && (i < ((guint32)src_data_len - 4)); i++) { // 4 + i需小于数据长度
        *(data_operate->data_buf + data_operate->data_offset + i) = src_data[4 + i]; // 从data 4开始为bdf数据
    }

    data_operate->data_offset += data_real_len;
    data_operate->data_len = src_data[BDF_CURRENT_FRAME];

    resp_data[0] = BIOS_ERR_NORMALLY;
    return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);

EXIT:
    resp_data[0] = BIOS_ERR_INVALID_STATUS;
    return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
}


LOCAL gint32 bdf_write_finish(const void *request_msg, guint8 *resp_data, guint32 size)
{
    BIOS_DATA_OPERATE_S *data_operate = &g_bdf_wr_data_cfg;
    const guint8 *src_data = NULL;
    guint32 i;
    guint32 data_real_len;
    GVariant *property_data = NULL;
    OBJ_HANDLE obj_handle;
    gint32 ret_val;
    guint8 src_data_len;
    guint8 bdf_len = 5; // 普通bdf长度为5

    if (request_msg == NULL || resp_data == NULL) {
        return RET_ERR;
    } else if (size < BIOS_MSG_HEAD_MIN_LEN) {
        return RET_ERR;
    }

    
    if (BIOS_FLAG_DOING != data_operate->data_flag) {
        debug_log(DLOG_ERROR, "DataFlag:%d(ShouldBe:%d) is invalid!\n", data_operate->data_flag, BIOS_FLAG_IDLE);

        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
    }

    src_data = get_ipmi_src_data(request_msg);
    src_data_len = get_ipmi_src_data_len(request_msg);
    if (src_data == NULL || src_data_len < BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, "src_data is invalid!\n");
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
    }

    if (data_operate->data_len + 1 != src_data[BDF_CURRENT_FRAME]) {
        resp_data[0] = BIOS_ERR_WRONG_OFFSET;
        goto bdf_quit0;
    }

    if (src_data[0] == BIOS_IPMI_CMD_PORT_ROOT_BDF) {
        bdf_len = 6; // 6,0xd1接口比普通bdf接口多一个字段
    }
    data_real_len = src_data[BDF_FRAME_PCIE_NUM] * bdf_len;
    if (data_real_len > (guint32)src_data_len - BIOS_IPMI_CMD_MIN_LEN) {
        debug_log(DLOG_ERROR, "%s: data_real_len (%u) is out of range", __FUNCTION__, data_real_len);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
    }

    if (data_operate->data_offset + data_real_len > BDF_DATA_SIZE) {
        debug_log(DLOG_ERROR, "RealLen:%d(Max:%d,DataOffset:%d) is invalid!\n", data_real_len, BDF_DATA_SIZE,
            data_operate->data_offset);

        resp_data[0] = BIOS_ERR_IV_LEN;
        return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
    }

    for (i = 0; i < data_real_len; i++) {
        *(data_operate->data_buf + data_operate->data_offset + i) = src_data[4 + i]; // 从data 4开始为bdf数据
    }

    data_operate->data_offset += data_real_len;
    data_operate->data_len = data_operate->data_offset;

    
    ret_val = dfl_get_object_handle(BIOS_CLASS_NAME, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Class Bios obj fail ret_val = %d", ret_val);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        goto bdf_quit0;
    }

    
    property_data = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, data_operate->data_buf, data_operate->data_offset,
        sizeof(guint8));
    if (property_data == NULL) {
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        goto bdf_quit0;
    }

    ret_val = bios_set_bdf_property_value(obj_handle, src_data[0], property_data);
    if (ret_val != RET_OK) {
        debug_log(DLOG_ERROR, "Set PROPERTY_BIOS_PCIECARD_BDF fail ret_val = %d", ret_val);
        resp_data[0] = BIOS_ERR_INVALID_STATUS;
        goto bdf_quit1;
    }

    resp_data[0] = BIOS_ERR_NORMALLY;
    debug_log(DLOG_INFO, "recv bdf data from bios starting ok\n");

bdf_quit1:
    g_variant_unref(property_data);

bdf_quit0:

    
    clear_data_operate_res(data_operate);

    return respmsg_with_customermanuid(request_msg, resp_data, size, BIOS_MSG_HEAD_MIN_LEN);
}
enum CARD_BDF_INFO_NUM {
    PROPERTY_BIOS_PCIECARD_BDF_INFO = 0,
    PROPERTY_BIOS_PCIEDISK_BDF_INFO,
    PROPERTY_BIOS_OCPCARD_BDF_INFO,
    PROPERTY_BIOS_PORT_ROOTBDF_INFO,
    PROPERTY_BIOS_PCIE_PORT_ROOTBDF_INFO,
    PROPERTY_BIOS_MAX_BDF_INFO,
};
typedef struct bdf_resource_info {
    // PCIe卡、PCIe硬盘和OCP卡
    uint8_t CardType;
    // 设置属性
    gchar *property;
} BDF_RESOURCE_INFO;

BDF_RESOURCE_INFO bdf_info_list[] = {
    {BIOS_IPMI_CMD_PCIECARD, PROPERTY_BIOS_PCIECARD_BDF},
    {BIOS_IPMI_CMD_PCIEDISK, PROPERTY_BIOS_PCIEDISK_BDF},
    {BIOS_IPMI_CMD_OCPCARD, PROPERTY_BIOS_OCPCARD_BDF},
    {BIOS_IPMI_CMD_PORT_ROOT_BDF, PROPERTY_BIOS_PORT_ROOTBDF},
    {BIOS_IPMI_CMD_PCIE_PORT_ROOT, PROPERTY_BIOS_PCIE_PORT_ROOTBDF},
};



LOCAL gint32 bios_set_bdf_property_value(OBJ_HANDLE object_handle, guint8 type, GVariant *property_data)
{
    guint32 i = 0;
    for (i = 0; i < PROPERTY_BIOS_MAX_BDF_INFO; i++) {
        if (type == bdf_info_list[i].CardType) {
            return dfl_set_property_value(object_handle, bdf_info_list[i].property, property_data, DF_NONE);
        }
    }
    
    debug_log(DLOG_ERROR, " %s bios_set_bdf_property_value fail !", __FUNCTION__);

    return RET_ERR;
}