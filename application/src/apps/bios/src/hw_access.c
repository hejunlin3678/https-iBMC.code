

#include "hw_access.h"
#include "smbios.h"
#include "pme_app/pme_app.h"

LOCAL gint32 check_hardware_info_reading_is_supported(const void *req_msg, guint8 register_type,
    OBJ_HANDLE *ref_chip_handle);
LOCAL guint8 bios_read_edma_shm_info(guint8 area, guint8 *resp_data, guint8 *resp_len);
LOCAL guint8 bios_read_cpld_register_info(const void *req_msg, guint8 *resp_data, gint32 resp_data_sz,
    guint8 *resp_len);
LOCAL guint8 bios_read_i2c_register_info(const void *req_msg, OBJ_HANDLE ref_chip_handle,
    guint8 *resp_data, guint8 *resp_len);
LOCAL guint8 bios_read_gpio_register_info(const void *req_msg, guint8 *resp_data, gint32 resp_data_sz,
    guint8 *resp_len);
LOCAL guint8 i2c_read_no_concatenation(const void *req_msg, guint8 *resp_data, guint8 *resp_len);
LOCAL guint8 i2c_read_from_concatenation(const void *req_msg, guint8 *resp_data, guint8 *resp_len);
LOCAL guint8 bios_read_cpld_i2c_device_info(gconstpointer req_msg, guint8 *resp_data, guint8 *resp_len);
LOCAL gint32 check_whitelist(const void *req_msg, OBJ_HANDLE obj_handle, OBJ_HANDLE *ref_chip_handle);
LOCAL gint32 search_chip(OBJ_HANDLE chip_handle_last, CONCATENATION_INFO_S *concatenation_info);
LOCAL gint32 check_pca9545_data_is_match(const void *req_msg, OBJ_HANDLE obj_handle);
LOCAL gint32 check_guint16_data_is_match(OBJ_HANDLE obj_handle, const gchar *property_name, guint16 cmp_data);
LOCAL gint32 check_byte_data_is_match(OBJ_HANDLE obj_handle, const gchar *property_name, guint8 cmp_data);


LOCAL gboolean is_need_get_hardware_info(const void *req_msg)
{
    gint32 channel_number = 0;
    guint8 software_type = 0;

    // 2、获取通道号,判断是否为BT通道；大西洋直通板例外，使用localhost查询
    channel_number = get_ipmi_chan_num(req_msg);
    (void)dal_get_software_type(&software_type);
    if (channel_number != SYS_CHAN_NUM && software_type != MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        return FALSE;
    }

    return TRUE;
}


gint32 bios_get_hardware_register_info(const void *req_msg, gpointer user_data)
{
    const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *get_register_req = NULL;
    guint8 resp_data[REGISTER_MAX_READ_LENGTH + BIOS_MSG_HEAD_MIN_LEN] = {0};
    guint8 resp_len = 0;
    const guint8 bios_edma_area = 1;
    OBJ_HANDLE ref_chip_handle = 0;

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return RET_ERR;
    }

    // 1、获取命令数据
    get_register_req = (const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *)get_ipmi_src_data(req_msg);
    
    if (get_register_req == NULL) {
        debug_log(DLOG_ERROR, DB_STR_GET_SRC_DATA_FAIL);
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }
    
    if (is_need_get_hardware_info(req_msg) == FALSE) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    // 3、判断厂商信息是否正确
    if (HUAWEI_MFG_ID != get_register_req->manu_id) {
        debug_log(DLOG_ERROR, DB_STR_MUNU_ID_ERR, RET_OK);
        return ipmi_send_simple_response(req_msg, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    
    gint32 ret_val = check_hardware_info_reading_is_supported(req_msg, get_register_req->register_type,
        &ref_chip_handle);
    if (ret_val != RET_OK) {
        debug_log(DLOG_DEBUG, "The ipmicmd is not in white list.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    // 4、获取命令BYTE 5判断寄存器类型
    switch (get_register_req->register_type) {
        case REGISTER_TYPE_CPLD:
            // 根据命令读CPLD寄存器配置
            resp_data[0] = bios_read_cpld_register_info(req_msg, resp_data, sizeof(resp_data), &resp_len);
            break;

        case REGISTER_TYPE_GPIO:
            // 根据命令读GPIO寄存器配置
            resp_data[0] = bios_read_gpio_register_info(req_msg, resp_data, sizeof(resp_data), &resp_len);
            break;

        case REGISTER_TYPE_IIC:
            // 根据命令读I2C寄存器配置
            resp_data[0] = bios_read_i2c_register_info(req_msg, ref_chip_handle, resp_data, &resp_len);
            break;

            
        case REGISTER_TYPE_EDMA:
            // 根据命令读EDMA共享内存偏移地址
            resp_data[0] = bios_read_edma_shm_info(bios_edma_area, resp_data, &resp_len);
            
            break;

        case REGISTER_TYPE_CPLD_I2C:
            resp_data[0] = bios_read_cpld_i2c_device_info(req_msg, resp_data, &resp_len);
            break;

        default:
            resp_data[0] = COMP_CODE_PARA_NOT_SUPPORT;
    }

    return construt_and_respond_msg(req_msg, resp_data, BIOS_MSG_HEAD_MIN_LEN + resp_len);
}


LOCAL gint32 check_hardware_info_reading_is_supported(const void *req_msg, guint8 register_type,
    OBJ_HANDLE *ref_chip_handle)
{
    OBJ_HANDLE object_handle;
    gint32 ret_val = 0;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "%s:req_msg is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    // 如果为读EDMA共享内存的起始地址和大小，则返回支持
    if (register_type == REGISTER_TYPE_EDMA) {
        debug_log(DLOG_DEBUG, "%s:support to read EDMA info.", __FUNCTION__);
        return RET_OK;
    }

    // 检测白名单是否支持
    ret_val = dfl_get_object_handle(BIOS_OBJECT_NAME, &object_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get object(%s) failed (ret=%d).", __FUNCTION__, BIOS_OBJECT_NAME, ret_val);
        return RET_ERR;
    }

    return check_whitelist(req_msg, object_handle, ref_chip_handle);
}


LOCAL guint8 bios_read_edma_shm_info(guint8 area, guint8 *resp_data, guint8 *resp_len)
{
    gchar buff[BIOS_IPMI_CMD_MAX_LEN] = "";
    gint32 i = 0, itmp = 0;
    const gint32 shm_area_num = 2;
    guint32 start = 0;
    guint32 size = 0;
    FILE *pfile = fopen("/sys/module/bmc_edma_drv/parameters/shm_area", "r");

    if (pfile == NULL) {
        debug_log(DLOG_ERROR, "can not open file.");
        return COMP_CODE_UNKNOWN;
    }

    for (i = 0; i < shm_area_num; i++) {
        vos_fgets_s(buff, sizeof(buff), pfile);

        if (sscanf_s(buff, "shm%d addr=0x%X, size=0x%X", &itmp, &start, &size) != 3) {
            debug_log(DLOG_ERROR, "driver info error");
            (void)fclose(pfile);
            return COMP_CODE_UNKNOWN;
        }

        if (area == itmp) {
            (void)fclose(pfile);
            resp_data[4] = LONGB0(start);
            resp_data[5] = LONGB1(start);
            resp_data[6] = LONGB2(start);
            resp_data[7] = LONGB3(start);
            resp_data[8] = LONGB0(size);
            resp_data[9] = LONGB1(size);
            resp_data[10] = LONGB2(size);
            resp_data[11] = LONGB3(size);
            *resp_len = 7;
            return RET_OK;
        }
    }

    (void)fclose(pfile);
    debug_log(DLOG_ERROR, "driver info error");
    return COMP_CODE_UNKNOWN;
}


LOCAL guint8 bios_read_cpld_i2c_device_info(gconstpointer req_msg, guint8 *resp_data, guint8 *resp_len)
{
    const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *get_register_req = NULL;
    guint32 ret;

    get_register_req = (const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *)get_ipmi_src_data(req_msg);
    OBJ_HANDLE card_handle = INVALID_OBJ_HANDLE;
    
    guint8 slot_id = get_register_req->chip_id + 1;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    
    ret = dal_get_specific_object_byte(CLASS_PANGEA_PCIE_CARD, PROPETRY_PCIECARD_SLOT, slot_id, &card_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get pangea card handle failed, ret=%d", ret);
        return COMP_CODE_UNKNOWN;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint16(get_register_req->offset));
    input_list = g_slist_append(input_list, g_variant_new_byte(get_register_req->read_len));
    ret = dfl_call_class_method(card_handle, METHOD_READ_DATA_FROM_MCU, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s read data from mcu failed, ret=%d.", dfl_get_object_name(card_handle), ret);
        return COMP_CODE_UNKNOWN;
    }

    const gchar *info_buf = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    gsize len = 0;
    guchar *card_type = g_base64_decode(info_buf, &len);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    if (card_type == NULL || len == 0) {
        debug_log(DLOG_ERROR, "%s decode base64 str failed.", dfl_get_object_name(card_handle));
        g_free(card_type);
        return COMP_CODE_UNKNOWN;
    }
    *resp_len = (get_register_req->read_len > len) ? len : get_register_req->read_len;
    ret = memcpy_s(resp_data + 4, *resp_len, card_type, *resp_len); 
    g_free(card_type);
    return (ret == RET_OK) ? COMP_CODE_SUCCESS : COMP_CODE_UNKNOWN;
}


LOCAL guint8 bios_read_i2c_register_info(const void *req_msg, OBJ_HANDLE ref_chip_handle,
    guint8 *resp_data, guint8 *resp_len)
{
    guint8 req_msg_len = 0;
    const guint8 *src_data = NULL;
    guint8 expand_len = 0;

    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    req_msg_len = get_ipmi_src_data_len(req_msg);

    src_data = (const guint8 *)get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    if (req_msg_len > BIOS_GET_REGISTER_INFO_MIN_LEN) {
        expand_len = src_data[11];
    }

    
    if (ref_chip_handle != 0) {
        guint8 read_len = src_data[10];
        gint32 read_offset = src_data[8] + (src_data[9] << 8);
        gint32 ret = dfl_chip_blkread(dfl_get_object_name(ref_chip_handle), read_offset,
            read_len, (gchar *)&resp_data[BIOS_MSG_HEAD_MIN_LEN]);
        if (ret == DFL_OK) {
            *resp_len = read_len;
            return BIOS_ERR_NORMALLY;
        }
        return BIOS_ERR_INVALID_STATUS;
    }

    if ((req_msg_len <= BIOS_GET_REGISTER_INFO_MIN_LEN) ||
        (expand_len == 0x00)) { 
        return i2c_read_no_concatenation(req_msg, resp_data, resp_len);
    }

    return i2c_read_from_concatenation(req_msg, resp_data, resp_len);
}


LOCAL guint8 bios_read_gpio_register_info(const void *req_msg, guint8 *resp_data, gint32 resp_data_sz, guint8 *resp_len)
{
    const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *get_register_req = NULL;
    gint32 ret = DFL_OK;
    guint8 value = 0;
    guint16 gpio_num = 0;
    // 5字节是resp_data数组所需最小内存
    if (req_msg == NULL || resp_data_sz < 5) {
        
        debug_log(DLOG_ERROR, "%s: Parameter error", __FUNCTION__);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    get_register_req = (const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *)get_ipmi_src_data(req_msg);
    if (get_register_req == NULL) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    
    if ((get_register_req->addr != 0) ||     
        (get_register_req->chip_id != 1) ||  
        (get_register_req->read_len != 1)) { 
        debug_log(DLOG_ERROR, "paramater error.");
        return COMP_CODE_OUTOF_RANGE;
    }

    
    gpio_num = get_register_req->offset;

    if (gpio_num >= GPIO_MAX_NUM) {
        debug_log(DLOG_ERROR, "gpio number is out of range : %d.", gpio_num);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    ret = gpio_init((guint8)gpio_num, 0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Init GPIO%d failed : ret = %d.", gpio_num, ret);
        return BIOS_ERR_INVALID_STATUS;
    }

    
    ret = gpio_read((guint8)gpio_num, &value, 0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Read GPIO%d level failed : ret = %d.", gpio_num, ret);
        return BIOS_ERR_INVALID_STATUS;
    }

    
    resp_data[4] = value;
    *resp_len = 1;

    return BIOS_ERR_NORMALLY;
}


LOCAL guint8 bios_read_cpld_register_info(const void *req_msg, guint8 *resp_data, gint32 resp_data_sz, guint8 *resp_len)
{
    const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *get_register_req = NULL;
    gint32 ret = DFL_OK;
    guint8 temp_data[REGISTER_MAX_READ_LENGTH] = {0};
    guint8 i = 0;
    // 需保留4字节给厂商ID和返回码
    if (req_msg == NULL || resp_data_sz < REGISTER_MAX_READ_LENGTH + 4) {
        
        debug_log(DLOG_ERROR, "%s: Parameter error", __FUNCTION__);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    get_register_req = (const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *)get_ipmi_src_data(req_msg);
    if (get_register_req == NULL) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    
    if ((get_register_req->addr != 0) ||                           
        (get_register_req->chip_id < 1) ||                         
        (get_register_req->read_len > REGISTER_MAX_READ_LENGTH) || 
        (get_register_req->read_len < REGISTER_MIN_READ_LENGTH)) {
        debug_log(DLOG_ERROR, "paramater error.");
        return COMP_CODE_OUTOF_RANGE;
    }

    
    ret = dal_read_cpld_bytes(get_register_req->chip_id,
        get_register_req->offset, get_register_req->read_len, temp_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_read_cpld_bytes(offset 0x%X) failed: ret %d.", get_register_req->offset, ret);
        return BIOS_ERR_INVALID_STATUS;
    }

    
    for (i = 0; i < get_register_req->read_len; i++) {
        resp_data[4 + i] = temp_data[i];
    }

    *resp_len = get_register_req->read_len;

    return BIOS_ERR_NORMALLY;
}


LOCAL gint32 __check_bus_addr(OBJ_HANDLE obj_node_hanlde, guint8 bus, guint16 addr)
{
    gint32 ret;
    guint8 bus_id_tmp = 0;
    guint32 addr_tmp = 0;
    OBJ_HANDLE bus_hanlde_tmp = 0;

    
    ret = dal_get_property_value_uint32(obj_node_hanlde, PROPERTY_CHIP_ADDR, &addr_tmp);
    if (ret != DFL_OK || addr != addr_tmp) {
        debug_log(DLOG_INFO, "%s addr_tmp[0x%x], ret %d", dfl_get_object_name(obj_node_hanlde), addr_tmp, ret);
        return RET_ERR;
    }

    
    ret = dfl_get_referenced_object(obj_node_hanlde, PROPERTY_CHIP_LBUS, &bus_hanlde_tmp);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "dfl_get_referenced_object %s failed, ret=%d!", PROPERTY_CHIP_LBUS, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(bus_hanlde_tmp, PROPERTY_BUS_ID, &bus_id_tmp);
    if (ret != DFL_OK || bus != bus_id_tmp) {
        debug_log(DLOG_INFO, "%s bus_id_tmp[%d], ret %d", dfl_get_object_name(bus_hanlde_tmp), bus_id_tmp, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 __i2c_read_from_chip(guint8 bus, guint16 addr, guint16 read_offset, gchar *read_data, guint8 read_len)
{
    gint32 ret;
    guint8 i = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_node_hanlde;
    const gchar* class_chip_name_list[] = {"Vrd", NULL}; 
    OBJ_HANDLE chip_hanlde_tmp = 0;

    debug_log(DLOG_INFO, "%s bus[%d] addr[0x%x], read_offset[0x%x]", __FUNCTION__, bus, addr, read_offset);
    while (class_chip_name_list[i] != NULL) {
        ret = dfl_get_object_list(class_chip_name_list[i], &obj_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_INFO, "Get %s object list failed!", class_chip_name_list[i]);
            i++;
            continue;
        }

        
        for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
            obj_node_hanlde = (OBJ_HANDLE)obj_node->data;

            ret = dfl_get_referenced_object(obj_node_hanlde, PROPERTY_CHIP_PRE_CHIP, &chip_hanlde_tmp);
            if (ret == DFL_OK) { 
                continue;
            }

            ret = __check_bus_addr(obj_node_hanlde, bus, addr);
            if (ret != RET_OK) {
                debug_log(DLOG_INFO, "%s check_bus_addr faild, ret %d", __FUNCTION__, ret);
                continue;
            }

            
            ret = dfl_chip_blkread(dfl_get_object_name(obj_node_hanlde), read_offset, read_len, (void *)read_data);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "dfl_chip_blkread(%s) failed, ret=%d!", dfl_get_object_name(obj_node_hanlde),
                    ret);
                continue;
            }
            g_slist_free(obj_list);
            obj_list = NULL;
            return RET_OK;
        }
        i++;
        g_slist_free(obj_list);
        obj_list = NULL;
    }
    debug_log(DLOG_INFO, "i2c_read_from_chip failed(ret = %d), try i2c_read.", ret);

    return RET_ERR;
}
LOCAL guint32 i2c_msg_check(const void *req_msg, const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S **get_register_req)
{
    guint32 ret;
    if (req_msg == NULL) {
        
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    *get_register_req = (const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *)get_ipmi_src_data(req_msg);
    if ((*get_register_req) == NULL) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    
    if (((*get_register_req)->read_len > REGISTER_MAX_READ_LENGTH) ||
        ((*get_register_req)->chip_id < REGISTER_MIN_READ_LENGTH)) {
        debug_log(DLOG_ERROR, "paramater error.");
        return COMP_CODE_OUTOF_RANGE;
    }

    ret = i2c_init((*get_register_req)->chip_id, 100, 0, 0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "I2C init failed : ret = %d.", ret);
        return BIOS_ERR_INVALID_STATUS;
    }

    return RET_OK;
}

LOCAL guint8 i2c_read_no_concatenation(const void *req_msg, guint8 *resp_data, guint8 *resp_len)
{
    const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *get_register_req = NULL;
    I2C_READ_S i2c_read_data = { 0 };
    gint32 ret = DFL_OK;
    guint8 temp_data[REGISTER_MAX_READ_LENGTH] = {0};
    guint8 i = 0;
    guint8 cmd_data[3] = {0};

    // 参数检查与初始化，大函数拆分
    ret = i2c_msg_check(req_msg, &get_register_req);
    if (ret != RET_OK) {
        return ret;
    }

    cmd_data[0] = get_register_req->addr;

    
    debug_log(DLOG_INFO, "Request info : Bus = %d, Addr = %x, Offset = %d.", get_register_req->chip_id,
        get_register_req->addr, get_register_req->offset);

    
    if (get_register_req->offset <= 0xFF) {
        cmd_data[1] = LONGB0(get_register_req->offset);
        i2c_read_data.tx_len = 2;
    } else {
        cmd_data[1] = LONGB0(get_register_req->offset);
        cmd_data[2] = LONGB1(get_register_req->offset);
        i2c_read_data.tx_len = 3;
    }

    

    i2c_read_data.rv_id = get_register_req->chip_id;
    i2c_read_data.tx_buffer = cmd_data;

    i2c_read_data.read_buf = temp_data;
    i2c_read_data.length = get_register_req->read_len;
    i2c_read_data.time_out_cnt = 100;
    i2c_read_data.re_read_cnt = 3;

    ret = __i2c_read_from_chip(get_register_req->chip_id, get_register_req->addr, get_register_req->offset,
        (gchar *)&resp_data[BIOS_MSG_HEAD_MIN_LEN], get_register_req->read_len);
    if (ret == RET_OK) {
        *resp_len = get_register_req->read_len;
        return BIOS_ERR_NORMALLY;
    }

    
    ret = i2c_read(&i2c_read_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "I2C read failed : ret = %d.", ret);
        return BIOS_ERR_INVALID_STATUS;
    }

    
    for (i = 0; i < i2c_read_data.length; i++) {
        resp_data[4 + i] = i2c_read_data.read_buf[i];
    }

    *resp_len = i2c_read_data.length;

    return BIOS_ERR_NORMALLY;
}


LOCAL guint8 i2c_read_from_concatenation(const void *req_msg, guint8 *resp_data, guint8 *resp_len)
{
    gint32 ret = DFL_OK;
    const guint8 *src_data = NULL;
    CONCATENATION_INFO_S concatenation_info;
    guint8 chip_connection_level = 0;
    const gchar* class_chip_name_list[] = {
        "Pca9555", "Chip", "CpldChip", "Eeprom", "FanChip", "Pca9541", "Pca9545", "Vddq",
        "Lm75", "Adt7481", "Lm96080", "Rtc", "Wave", "xeonphi_chip", NULL
    };
    const gchar *class_chip_name_tmp = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_node_hanlde = 0;
    gchar read_data[REGISTER_MAX_READ_LENGTH] = {0, 0};

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    
    guint8 req_msg_len = get_ipmi_src_data_len(req_msg);
    if (req_msg_len <= BIOS_GET_REGISTER_INFO_MIN_LEN) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    (void)memset_s(&concatenation_info, sizeof(concatenation_info), 0, sizeof(concatenation_info));

    src_data = (const guint8 *)get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    guint8 bus_id_last = src_data[5];
    guint16 addr_last = src_data[6] + (src_data[7] << 8);
    guint16 read_offset = src_data[8] + (src_data[9] << 8);
    guint8 read_len = src_data[10];
    guint8 expand_len = src_data[11];

    
    if ((read_len > REGISTER_MAX_READ_LENGTH) || (read_len < REGISTER_MIN_READ_LENGTH)) {
        debug_log(DLOG_ERROR, "paramater error: read length (%u) invalid.", read_len);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    if ((expand_len - 1) % 3 != 0) {
        debug_log(DLOG_ERROR, "paramater error: expand length (%u) invalid.", expand_len);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    chip_connection_level = (expand_len - 1) / 3 + 1; // 中间级联的层次，包括最后一个器件的一层
    if (chip_connection_level > CONCATENATION_MAX_LEVEL) {
        debug_log(DLOG_ERROR, "paramater error: too many concatenation level (%u).", chip_connection_level);
        return COMP_CODE_OUTOF_RANGE;
    }

    
    concatenation_info.taoal_level_num = chip_connection_level;

    for (gsize i = 0; i < chip_connection_level - 1; i++) {
        if ((BIOS_GET_REGISTER_INFO_MIN_LEN + 1 + i * CONCATENATION_BASE + 2) >= req_msg_len) {
            debug_log(DLOG_ERROR, "%s:Invalid cmd length, expand_len:%u, req_msg_len:%u, concatenation level:%u.",
                __FUNCTION__, expand_len, req_msg_len, chip_connection_level);
            break;
        }
        concatenation_info.chip_info[i].bus_id = src_data[BIOS_GET_REGISTER_INFO_MIN_LEN + 1 + i * 3];
        concatenation_info.chip_info[i].addr = (src_data[BIOS_GET_REGISTER_INFO_MIN_LEN + 1 + i * 3 + 1]) +
            (src_data[BIOS_GET_REGISTER_INFO_MIN_LEN + 1 + i * 3 + 2] << 8);
    }

    concatenation_info.chip_info[chip_connection_level - 1].bus_id = bus_id_last;
    concatenation_info.chip_info[chip_connection_level - 1].addr = addr_last;

    
    gsize j = 0;
    class_chip_name_tmp = class_chip_name_list[j++];

    while (class_chip_name_tmp != NULL) {
        ret = dfl_get_object_list(class_chip_name_tmp, &obj_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_INFO, "Get %s object list failed!", class_chip_name_tmp);
            class_chip_name_tmp = class_chip_name_list[j++];
            continue;
        }

        
        for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
            obj_node_hanlde = (OBJ_HANDLE)obj_node->data;

            
            ret = search_chip(obj_node_hanlde, &concatenation_info);
            if (ret != RET_OK) {
                continue;
            }

            
            ret = dfl_chip_blkread(dfl_get_object_name(obj_node_hanlde), read_offset, read_len, (void *)read_data);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "dfl_chip_blkread failed, chip_name = %s, read_offset = %u, "
                    "read_len = %u, ret=%d!", dfl_get_object_name(obj_node_hanlde), read_offset, read_len, ret);
                continue;
            }

            
            for (gsize k = 0; k < read_len; k++) {
                resp_data[4 + k] = read_data[k];
            }

            *resp_len = read_len;

            g_slist_free(obj_list);
            return BIOS_ERR_NORMALLY;
        }

        g_slist_free(obj_list);
        class_chip_name_tmp = class_chip_name_list[j++];
    }

    return COMP_CODE_DATA_NOT_AVAILABLE;
}

LOCAL gint32 check_whitelist_object(const void *req_msg, OBJ_HANDLE handle)
{
    const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *get_register_req = NULL;

    get_register_req = (const OEM_GET_HARDWARE_REGISTER_INFO_REQ_S *)get_ipmi_src_data(req_msg);
    if (get_register_req == NULL) {
        debug_log(DLOG_ERROR, "%s:get_ipmi_src_data failed.", __FUNCTION__);
        return RET_ERR;
    }
 
    gint32 ret;
    
    ret = check_byte_data_is_match(handle, PROPERTY_TYPE, get_register_req->register_type);
    if (ret != RET_OK) {
        return RET_ERR;
    }
 
    
    ret = check_byte_data_is_match(handle, PROPERTY_CHIPID, get_register_req->chip_id);
    if (ret != RET_OK) {
        return RET_ERR;
    }
 
    
    ret = check_guint16_data_is_match(handle, PROPERTY_ADDR, get_register_req->addr);
    if (ret != RET_OK) {
        return RET_ERR;
    }
 
    
    ret = check_guint16_data_is_match(handle, PROPERTY_OFFSET, get_register_req->offset);
    if (ret != RET_OK) {
        return RET_ERR;
    }
 
    
    ret = check_byte_data_is_match(handle, PROPERTY_LENGTH, get_register_req->read_len);
    if (ret != RET_OK) {
        return RET_ERR;
    }
 
    
    return check_pca9545_data_is_match(req_msg, handle);
}


LOCAL gint32 check_whitelist(const void *req_msg, OBJ_HANDLE obj_handle, OBJ_HANDLE *ref_chip_handle)
{
    gint32 ret = 0;
    GVariant *array_obj_name_gvar = NULL;
    GVariant *obj_name_gvar = NULL;
    GVariantIter iter;
    OBJ_HANDLE handle = 0;
    const gchar *string = NULL;
    
    ret = dfl_get_property_value(obj_handle, PROPERTY_BIOS_HDINFO_RD_CMD_WHITELIST, &array_obj_name_gvar);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_property_value(%s.%s) failed: ret=%d.", dfl_get_object_name(obj_handle),
            PROPERTY_BIOS_HDINFO_RD_CMD_WHITELIST, ret);
        return ret;
    }
    
    g_variant_iter_init(&iter, array_obj_name_gvar);
    while ((obj_name_gvar = g_variant_iter_next_value(&iter)) != NULL) {
        handle = 0;
        string = g_variant_get_string(obj_name_gvar, NULL);
        if (string == NULL) {
            g_variant_unref(obj_name_gvar);
            g_variant_unref(array_obj_name_gvar);
            return RET_ERR;
        }
        
        ret = dfl_get_object_handle(string, &handle);
        g_variant_unref(obj_name_gvar);
        obj_name_gvar = NULL;
        if (ret != DFL_OK) {
            g_variant_unref(array_obj_name_gvar);
            debug_log(DLOG_DEBUG, "%s: dfl_get_object_handle(%s) ret=%d", __FUNCTION__, string, ret);
            return ret;
        }

        ret = check_whitelist_object(req_msg, handle);
        if (ret != RET_OK) {
            continue;
        }
        break;
    }
    (void)dfl_get_referenced_object(handle, PROPERTY_BIOS_HDINFO_REF_CHIP_HANDLE, ref_chip_handle);

    g_variant_unref(array_obj_name_gvar);
    return ret;
}


LOCAL gint32 search_chip(OBJ_HANDLE chip_handle_last, CONCATENATION_INFO_S *concatenation_info)
{
    guint8 bus_id_tmp = 0;
    guint32 addr_tmp = 0;
    CONCATENATION_INFO_S concatenation_info_tmp;
    guint8 i = 0;
    OBJ_HANDLE chip_hanlde_tmp = 0;
    OBJ_HANDLE pre_chip_hanlde_tmp = 0;
    OBJ_HANDLE bus_hanlde_tmp = 0;

    (void)memset_s(&concatenation_info_tmp, sizeof(concatenation_info_tmp), 0, sizeof(concatenation_info_tmp));

    if (concatenation_info == NULL) {
        debug_log(DLOG_ERROR, "concatenation_info is NULL, chip name (%s).", dfl_get_object_name(chip_handle_last));
        return RET_ERR;
    }

    guint8 concatenation_level_tmp = concatenation_info->taoal_level_num;
    guint16 addr_last = concatenation_info->chip_info[concatenation_level_tmp - 1].addr;

    (void)dal_get_property_value_uint32(chip_handle_last, PROPERTY_CHIP_ADDR, &addr_tmp);
    
    if (addr_tmp != addr_last) {
        debug_log(DLOG_DEBUG, "search_chip: last chip not matching.");
        return RET_ERR;
    }

    gint32 ret = dfl_get_referenced_object(chip_handle_last, PROPERTY_CHIP_LBUS, &bus_hanlde_tmp);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object %s failed, chip name (%s), ret=%d!",
            PROPERTY_CHIP_LBUS, dfl_get_object_name(chip_handle_last), ret);
        return RET_ERR;
    }
    (void)dal_get_property_value_byte(bus_hanlde_tmp, PROPERTY_BUS_ID, &bus_id_tmp);

    guint8 bus_id_last = concatenation_info->chip_info[concatenation_level_tmp - 1].bus_id;
    
    if (bus_id_tmp != bus_id_last - 1) {
        debug_log(DLOG_DEBUG, "search_chip: last chip not matching.");
        return RET_ERR;
    }

    
    
    concatenation_level_tmp = 0;
    concatenation_info_tmp.chip_info[CONCATENATION_MAX_LEVEL - 1].bus_id = bus_id_tmp;
    concatenation_info_tmp.chip_info[CONCATENATION_MAX_LEVEL - 1].addr = addr_tmp;
    concatenation_level_tmp++;

    chip_hanlde_tmp = chip_handle_last;

    do {
        
        ret = dfl_get_referenced_object(chip_hanlde_tmp, PROPERTY_CHIP_PRE_CHIP, &pre_chip_hanlde_tmp);
        if (ret != DFL_OK) {
            debug_log(DLOG_DEBUG, "dfl_get_referenced_object %s failed, ret=%d!", PROPERTY_CHIP_PRE_CHIP, ret);
            break;
        }
        (void)dal_get_property_value_uint32(pre_chip_hanlde_tmp, PROPERTY_CHIP_ADDR, &addr_tmp);

        ret = dfl_get_referenced_object(pre_chip_hanlde_tmp, PROPERTY_CHIP_LBUS, &bus_hanlde_tmp);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object %s failed, chip name (%s), pre chip name (%s), "
                "bus name(%s), ret=%d!", PROPERTY_CHIP_LBUS, dfl_get_object_name(chip_handle_last),
                dfl_get_object_name(pre_chip_hanlde_tmp), dfl_get_object_name(bus_hanlde_tmp), ret);
            break;
        }
        (void)dal_get_property_value_byte(bus_hanlde_tmp, PROPERTY_BUS_ID, &bus_id_tmp);

        
        concatenation_info_tmp.chip_info[CONCATENATION_MAX_LEVEL - 1 - concatenation_level_tmp].bus_id = bus_id_tmp;
        concatenation_info_tmp.chip_info[CONCATENATION_MAX_LEVEL - 1 - concatenation_level_tmp].addr = addr_tmp;

        concatenation_level_tmp++;
        
        if (concatenation_level_tmp >= CONCATENATION_MAX_LEVEL) {
            debug_log(DLOG_ERROR, "search_chip, more than max concatenation level, "
                "chip name (%s), pre chip name (%s), bus name(%s)", dfl_get_object_name(chip_handle_last),
                dfl_get_object_name(pre_chip_hanlde_tmp), dfl_get_object_name(bus_hanlde_tmp));
            return RET_ERR;
        }

        chip_hanlde_tmp = pre_chip_hanlde_tmp;
    } while (chip_hanlde_tmp != 0);

    
    guint8 gap = CONCATENATION_MAX_LEVEL - concatenation_level_tmp;

    for (i = 0; i < concatenation_level_tmp; i++) {
        concatenation_info_tmp.chip_info[i].bus_id = concatenation_info_tmp.chip_info[gap + i].bus_id;
        concatenation_info_tmp.chip_info[i].addr = concatenation_info_tmp.chip_info[gap + i].addr;
    }

    concatenation_info_tmp.taoal_level_num = concatenation_level_tmp;

    
    
    if (concatenation_info_tmp.taoal_level_num != concatenation_info->taoal_level_num) {
        debug_log(DLOG_ERROR, "search_chip, concatenation_info_tmp taoal_level_num (%u) not equal to "
            "concatenation_info taoal_level_num (%u), chip name(%s)", concatenation_info_tmp.taoal_level_num,
            concatenation_info->taoal_level_num, dfl_get_object_name(chip_handle_last));
        return RET_ERR;
    }

    
    if (concatenation_info_tmp.chip_info[0].bus_id != concatenation_info->chip_info[0].bus_id ||
        concatenation_info_tmp.chip_info[0].addr != concatenation_info->chip_info[0].addr) {
        debug_log(DLOG_DEBUG, "search_chip, first concatenation chip info dismatch, "
            "concatenation_info_tmp bus_id (%u) not equal to concatenation_info bus_id (%u) or "
            "concatenation_info_tmp addr (%u) not equal to concatenation_info addr (%u), chip name(%s)",
            concatenation_info_tmp.chip_info[0].bus_id, concatenation_info->chip_info[0].bus_id,
            concatenation_info_tmp.chip_info[0].addr, concatenation_info->chip_info[0].addr,
            dfl_get_object_name(chip_handle_last));
        return RET_ERR;
    }

    
    for (i = 1; i < concatenation_level_tmp; i++) {
        if (concatenation_info_tmp.chip_info[i].bus_id != concatenation_info->chip_info[i].bus_id - 1 ||
            concatenation_info_tmp.chip_info[i].addr != concatenation_info->chip_info[i].addr) {
            debug_log(DLOG_ERROR, "search_chip, loop %u concatenation chip info dismatch, "
                "concatenation_info_tmp bus_id (%u) not equal to concatenation_info bus_id - 1 (%d) or "
                "concatenation_info_tmp addr (%u) not equal to concatenation_info addr (%u), chip name(%s)", i,
                concatenation_info_tmp.chip_info[i].bus_id, concatenation_info->chip_info[i].bus_id - 1,
                concatenation_info_tmp.chip_info[i].addr, concatenation_info->chip_info[i].addr,
                dfl_get_object_name(chip_handle_last));
            return RET_ERR;
        }
    }

    return RET_OK;
}


LOCAL gint32 check_pca9545_data_is_match(const void *req_msg, OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    guint8 req_msg_len = 0;
    guint8 expand_len = 0;
    const guint8 *src_data = NULL;
    const guint8 *pca9545_busid_array = NULL;
    const guint16 *pca9545_addr_array = NULL;
    guint8 connect_9545_level = 0;
    GVariant *get_busid = NULL;
    GVariant *get_addr = NULL;
    gsize n = 0;
    gsize m = 0;
    gsize i = 0;

#define ONE_PCA9545_INFO_LEN 3 // 每级pca9545_busid + pca9545_addr 共3个字节，判断下一个时需要偏移3字节

    req_msg_len = get_ipmi_src_data_len(req_msg);
    src_data = (const guint8 *)get_ipmi_src_data(req_msg);
    if (src_data == NULL) {
        return RET_ERR;
    }

    expand_len = (req_msg_len > BIOS_GET_REGISTER_INFO_MIN_LEN) ? src_data[11] : 0;

    
    if (src_data[4] != REGISTER_TYPE_IIC) {
        return RET_OK;
    }

    connect_9545_level = (expand_len > 0) ? (expand_len - 1) / ONE_PCA9545_INFO_LEN : 0;

    ret = dfl_get_property_value(obj_handle, PROPERTY_PCA9545_BUSID, &get_busid);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get property(%s) failed!", __FUNCTION__, PROPERTY_PCA9545_BUSID);
        return ret;
    }

    pca9545_busid_array = (const guint8 *)g_variant_get_fixed_array(get_busid, &n, sizeof(guint8));
    if (pca9545_busid_array == NULL) {
        g_variant_unref(get_busid);
        debug_log(DLOG_DEBUG, "%s: get pca9545_busid_array failed!", __FUNCTION__);
        return RET_ERR;
    }

    // 如果ipmi消息没有带9545信息，但白名单配置了，则禁止访问
    if ((n > 1) && ((req_msg_len <= BIOS_GET_REGISTER_INFO_MIN_LEN) || (expand_len == 0x00))) {
        debug_log(DLOG_DEBUG, "%s: no pca9545 info! n=%u", __FUNCTION__, (guint32)n);
        g_variant_unref(get_busid);
        return RET_ERR;
    }

    // 级联级数的和配置不一样，禁止访问
    if ((n > 1) && (n != connect_9545_level)) {
        g_variant_unref(get_busid);
        debug_log(DLOG_DEBUG, "%s: pca9545 level error! n=%u,level=%d", __FUNCTION__, (guint32)n, connect_9545_level);
        return RET_ERR;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_PCA9545_ADDR, &get_addr);
    if (ret != DFL_OK) {
        g_variant_unref(get_busid);
        debug_log(DLOG_ERROR, "%s: get property(%s) failed!", __FUNCTION__, PROPERTY_PCA9545_ADDR);
        return ret;
    }

    pca9545_addr_array = (const guint16 *)g_variant_get_fixed_array(get_addr, &m, sizeof(guint16));
    if (pca9545_addr_array == NULL) {
        g_variant_unref(get_busid);
        g_variant_unref(get_addr);
        debug_log(DLOG_DEBUG, "%s: get pca9545_busid_array failed!", __FUNCTION__);
        return RET_ERR;
    }
    if (n != m) {
        g_variant_unref(get_busid);
        g_variant_unref(get_addr);
        debug_log(DLOG_DEBUG, "%s: pca9545 busid don't match addr! n=%u,m=%u", __FUNCTION__, (guint32)n, (guint32)m);
        return RET_ERR;
    }
    for (i = 0; i < n; i++) {
        if ((pca9545_busid_array[i] != src_data[12 + i * ONE_PCA9545_INFO_LEN]) || (pca9545_addr_array[i] !=
            MAKE_WORD(src_data[14 + i * ONE_PCA9545_INFO_LEN], src_data[13 + i * ONE_PCA9545_INFO_LEN]))) {
            ret = RET_ERR;
            break;
        }
    }
    g_variant_unref(get_busid);
    g_variant_unref(get_addr);
    return ret;
}


LOCAL gint32 check_guint16_data_is_match(OBJ_HANDLE obj_handle, const gchar *property_name, guint16 cmp_data)
{
    gint32 ret = 0;
    guint16 value = 0;

    ret = dal_get_property_value_uint16(obj_handle, property_name, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value(%s) ret=%d", __FUNCTION__, property_name, ret);
        return ret;
    }
    if (value != cmp_data) {
        debug_log(DLOG_DEBUG, "%s:property(%s) value does not match(%d!=%d)", __FUNCTION__, property_name, value,
            cmp_data);
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 check_byte_data_is_match(OBJ_HANDLE obj_handle, const gchar *property_name, guint8 cmp_data)
{
    gint32 ret = 0;
    guint8 value = 0;

    ret = dal_get_property_value_byte(obj_handle, property_name, &value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_property_value(%s) ret=%d", __FUNCTION__, property_name, ret);
        return ret;
    }
    if (value != cmp_data) {
        debug_log(DLOG_DEBUG, "%s:property(%s) value does not match(%d!=%d)", __FUNCTION__, property_name, value,
            cmp_data);
        return RET_ERR;
    }
    return RET_OK;
}
