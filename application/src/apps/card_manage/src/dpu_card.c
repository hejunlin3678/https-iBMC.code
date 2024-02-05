

#include "pme_app/pme_app.h"
#include "get_version.h"

#include "dpu_card.h"


guint8 g_ip_data[DPU_CARD_IPADDR_READ_LEN] = {0};
guint8 g_ip_updated = DPU_CARD_IP_NOT_UPDATED;


LOCAL gint32 print_dpu_info(FILE *fp, GSList *obj_node)
{
    gint32 ret;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gchar card_info[DPU_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    size_t fwrite_back;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_DPU_CARD_SLOT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DPU_CARD_M2_PRESENCE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DPU_CARD_SLAVE_CARD_PRESENCE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DPU_CARD_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_DPU_CARD_MANUFACTURER);

    ret = dfl_multiget_property_value((OBJ_HANDLE)obj_node->data, property_name_list, &property_value);
    g_slist_free(property_name_list);
    property_name_list = NULL;
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get dpu card information failed.");
        return RET_ERR;
    }

    guint8 slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0)); // 0：第一个数据为槽位
    guint8 m2_pst = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1)); // 1：第二个数据为M.2盘在位
    guint8 slave_card_pst =
        g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2)); // 2：第三个数据为扣板在位
    const gchar *name =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0); // 3：第四个数据为扣板名称
    const gchar *manufacturer =
        g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 4), 0); // 4：第五个数据为制造商

    (void)snprintf_s(card_info, sizeof(card_info), sizeof(card_info) - 1,
        "%-4s | %-20s | %-20s | %-20s | %-30s \n"
        "%-4u | %-20s | %-20s | %-20s | %-30s \n",
        "Slot", "Slave Card Name", "Manufacturer", "M.2 Presence", "Slave Card Presence", slot_id, name, manufacturer,
        (m2_pst == 1) ? "Presence" : "Absence", (slave_card_pst == 1) ? "Presence" : "Absence");

    fwrite_back = fwrite(card_info, strlen(card_info), 1, fp);
    g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
    if (fwrite_back != 1) { // 1: 是否写成功
        debug_log(DLOG_ERROR, "fwrite failed.");
        return RET_ERR;
    }

    return RET_OK;
}


gint32 dpu_card_dump_info(const gchar *path)
{
    gint32 ret;
    gchar file_name[DPU_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar dpu_card_info[DPU_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    size_t fwrite_back;

    ret = dfl_get_object_list(CLASS_STORAGE_DPU_CARD, &obj_list);
    if (ret != DFL_OK) {
        return (ret == ERRCODE_OBJECT_NOT_EXIST) ? RET_OK : RET_ERR;
    }

    
    ret = snprintf_s(file_name, DPU_CARD_DUMPINFO_MAX_LEN + 1, DPU_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail, ret = %d.", ret);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "Open card_info failed.");
        ret = RET_ERR;
        goto log_out1;
    }

    (void)fchmod(fileno(fp), 0640); // 0640: 打开文件的模式
    (void)snprintf_s(dpu_card_info, sizeof(dpu_card_info), sizeof(dpu_card_info) - 1, "%s", "DPU Card Info\n");
    fwrite_back = fwrite(dpu_card_info, strlen(dpu_card_info), 1, fp);
    if (fwrite_back != 1) { // 1: 是否写成功
        debug_log(DLOG_ERROR, "fwrite failed.");
        goto log_out2;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        ret = print_dpu_info(fp, obj_node);
        if (ret != RET_OK) {
            goto log_out2;
        }
    }

    (void)snprintf_s(dpu_card_info, sizeof(dpu_card_info), sizeof(dpu_card_info) - 1, "%s", "\n\n");

    fwrite_back = fwrite(dpu_card_info, strlen(dpu_card_info), 1, fp);
    if (fwrite_back != 1) { // 1: 是否写成功
        debug_log(DLOG_ERROR, "fwrite failed.");
        ret = RET_ERR;
    }

log_out2:
    (void)fclose(fp);
log_out1:
    g_slist_free(obj_list);
    return ret;
}


gint32 dpu_card_add_object_callback(OBJ_HANDLE object_handle)
{
    gint32 ret;

    
    ret = get_pcb_version(object_handle, PROPERTY_DPU_CARD_PCB_ID, PROPERTY_DPU_CARD_PCB_VER);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 dpu_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}


LOCAL gint32 get_dpucard_ipaddr(guint8 dpu_slot, guint8 ip_type, gchar *res_data)
{
    gint32 ret;

    
    if (ip_type == DPU_IPMI_GET_IPV4_ADDR) {
        ret = memcpy_s(res_data, DPU_IPMI_IPV4_ADDR_LEN, g_ip_data, DPU_IPMI_IPV4_ADDR_LEN);
    } else if (ip_type == DPU_IPMI_GET_IPV6_ADDR) {
        ret = memcpy_s(res_data, DPU_IPMI_IPV6_ADDR_LEN, g_ip_data + 5, DPU_IPMI_IPV6_ADDR_LEN); // 5， ipv6在数组中开始位置
    } else {
        debug_log(DLOG_ERROR, "Current ip type is not allowed, ip_type = %hhu", ip_type);
        return RET_ERR;
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "memcpy_s: copy ip info failed, ret =  %d", ret);
    }

    return ret;
}


gint32 ipmi_get_dpucard_ipaddr(gconstpointer req_msg, gpointer user_data)
{
    gint32 ret;
    const IPMIMSG_GET_DPUCARD_IPADDR_REQ_S *req_data = NULL;
    IPMIMSG_GET_DPUCARD_IPADDR_RESP_S resp_data = { 0 };
    guint8 resp_data_len;

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "invalid input parameter.");
        return RET_ERR;
    }

    req_data = (const IPMIMSG_GET_DPUCARD_IPADDR_REQ_S *)get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "req_data is null.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (dal_check_ipmi_req_huawei_iana(req_data->manu_id, MANUFAC_ID_LEN) == FALSE) {
        debug_log(DLOG_ERROR, "manu_id is invalid.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    if (get_ipmi_src_data_len(req_msg) != DPU_IPMI_IP_ADDR_RES_DATA_LEN) {
        debug_log(DLOG_ERROR, "param_len is invalid. param_len = %u", req_data->param_len);
        return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
    }

    
    if (req_data->ip_type > DPU_IPMI_GET_IPV6_ADDR) {
        debug_log(DLOG_ERROR, "ip_type is invalid. ip_type = %d", req_data->ip_type);
        return ipmi_send_simple_response(req_msg, COMP_CODE_OUTOF_RANGE);
    }

    if (dal_set_ipmi_resp_huawei_iana(resp_data.manu_id, sizeof(resp_data.manu_id)) != TRUE) {
        debug_log(DLOG_ERROR, "dal_set_ipmi_resp_huawei_iana fail.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    ret = get_dpucard_ipaddr(req_data->slot, req_data->ip_type, resp_data.data);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    resp_data.slot = req_data->slot;
    resp_data.comp_code = COMP_CODE_SUCCESS;
    resp_data_len = (req_data->ip_type == 0) ? DPU_IPMI_IPV4_ADDR_LEN : DPU_IPMI_IPV6_ADDR_LEN;
    return ipmi_send_response(req_msg, DPU_IPMI_RESP_DATA_BASE_LEN + resp_data_len, (guint8 *)&resp_data);
}


LOCAL gint32 get_dpucard_port_status(guint8 dpu_slot, guint8 *link_status)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 status = 0;

    
    ret = dal_get_specific_object_byte(CLASS_STORAGE_DPU_CARD, PROPERTY_DPU_CARD_SLOT, dpu_slot, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get obj_handle fail, ret = %d", ret);
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_DPU_CARD_ETH_LINK_STATUS, &status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get DpuEthLinkStatus property fail, ret = %d", ret);
        return RET_ERR;
    }

    *link_status = status;
    return ret;
}


gint32 ipmi_get_dpucard_port_status(gconstpointer req_msg, gpointer user_data)
{
    gint32 ret;
    const IPMIMSG_GET_DPUCARD_PORT_STATUS_REQ_S *req_data = NULL;
    IPMIMSG_GET_DPUCARDD_PORT_STATUS_RESP_S resp_data = { 0 };

    if (req_msg == NULL) {
        debug_log(DLOG_ERROR, "invalid input parameter.");
        return RET_ERR;
    }

    req_data = (const IPMIMSG_GET_DPUCARD_PORT_STATUS_REQ_S *)get_ipmi_src_data(req_msg);
    if (req_data == NULL) {
        debug_log(DLOG_ERROR, "req_data is null.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    if (dal_check_ipmi_req_huawei_iana(req_data->manu_id, MANUFAC_ID_LEN) == FALSE) {
        debug_log(DLOG_ERROR, "manu_id is invalid.");
        return ipmi_send_simple_response(req_msg, COMP_CODE_INVALID_FIELD);
    }

    
    if (get_ipmi_src_data_len(req_msg) != DPU_IPMI_PORT_STATE_RES_DATA_LEN) {
        debug_log(DLOG_ERROR, "param_len is invalid. param_len = %u", req_data->param_len);
        return ipmi_send_simple_response(req_msg, COMP_CODE_LEN_INVALID);
    }

    if (dal_set_ipmi_resp_huawei_iana(resp_data.manu_id, sizeof(resp_data.manu_id)) != TRUE) {
        debug_log(DLOG_ERROR, "dal_set_ipmi_resp_huawei_iana fail");
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    ret = get_dpucard_port_status(req_data->slot, &resp_data.link_status);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(req_msg, COMP_CODE_UNKNOWN);
    }

    resp_data.slot = req_data->slot;
    resp_data.comp_code = COMP_CODE_SUCCESS;
    return ipmi_send_response(req_msg, sizeof(resp_data), (guint8 *)&resp_data);
}


LOCAL gint32 get_dpu_card_referenced_chip_value(const OBJ_HANDLE object_handle, const guint8 chip_offset,
    const gsize chip_data_size, guint8 *out_data)
{
    gint32 ret;
    OBJ_HANDLE chip_handle;

    if (out_data == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_referenced_object(object_handle, PROPERTY_DPU_CARD_CPLD_CHIP, &chip_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object fail, obj_name = %s, ret = %d",
            dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    ret = dfl_chip_blkread(dfl_get_object_name(chip_handle), chip_offset, chip_data_size, (void *)out_data);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "read chip failed, obj_name = %s, ret = %d", dfl_get_object_name(chip_handle), ret);
    }
    return ret;
}


LOCAL void read_dpu_cpld_reg_info(OBJ_HANDLE obj_handle, gchar *cpld_info, guint32 cpld_info_len)
{
    gint32 ret;
    guint8 row;
    guint8 col;
    guint8 offset;
    guint8 read_value[DPU_CPLD_LOG_COL_NUM];
    guint8 retry_times;
    gchar tmp[DPU_EACH_DATA_LEN];
    errno_t safe_fun_ret;

    for (row = 0; row < DPU_CPLD_LOG_ROW_NUM; row++) {
        offset = row * DPU_CPLD_LOG_COL_NUM;
        (void)memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
        (void)snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%04X:  ", offset);
        
        safe_fun_ret = strncat_s(cpld_info, cpld_info_len, tmp, strlen(tmp));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "strncat_s fail, ret = %d", safe_fun_ret);
        }

        
        for (retry_times = 0; retry_times < DPU_GET_CPLD_LOG_RETRY_TIMES; retry_times++) {
            ret = get_dpu_card_referenced_chip_value(obj_handle, offset, DPU_CPLD_LOG_COL_NUM, read_value);
            if (ret == RET_OK) {
                break;
            }
        }
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get dpu cpld reg value failed! offset = %d, ret = %d", offset, ret);
            return;
        }

        
        for (col = 0; col < DPU_CPLD_LOG_COL_NUM; col++) {
            (void)memset_s(tmp, sizeof(tmp), 0, sizeof(tmp));
            (void)snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%02X ", read_value[col]);

            safe_fun_ret = strncat_s(cpld_info, cpld_info_len, tmp, strlen(tmp));
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "strncat_s fail, ret = %d", safe_fun_ret);
            }
        }
        safe_fun_ret = strncat_s(cpld_info, cpld_info_len, "\n", strlen("\n"));
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "strncat_s fail, ret = %d", safe_fun_ret);
        }
    }
}


LOCAL gint32 dump_cpld_reg_of_one_dpu(OBJ_HANDLE obj_handle, FILE *fp)
{
    gint32 ret;
    guint8 slot;
    errno_t fun_ret;
    size_t fwrite_back;
    gchar cpld_info[DPU_CPLD_DUMPINFO_MAX_LEN] = {0};
    gchar cpld_info_detial[DPU_CPLD_INFO_MAX_LEN] = {0};

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_DPU_CARD_SLOT, &slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get dpu slot failed, ret = %d", ret);
        return RET_ERR;
    }

    
    (void)snprintf_s(cpld_info, sizeof(cpld_info), sizeof(cpld_info) - 1, "PCIe Slot %hhu Info\n", slot);
    fun_ret = strncat_s(cpld_info, sizeof(cpld_info), "offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n",
        strlen("offset 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n"));
    if (fun_ret != EOK) {
        debug_log(DLOG_ERROR, "strncat_s fail, ret = %d", fun_ret);
    }

    fun_ret = strncat_s(cpld_info, sizeof(cpld_info), "------------------------------------------------------\n",
        strlen("------------------------------------------------------\n"));
    if (fun_ret != EOK) {
        debug_log(DLOG_ERROR, "strncat_s fail, ret = %d", fun_ret);
    }

    
    fwrite_back = fwrite(cpld_info, strlen(cpld_info), 1, fp);
    if (fwrite_back != 1) { 
        debug_log(DLOG_ERROR, "fwrite title failed, fwrite_back = %zu", fwrite_back);
        return RET_ERR;
    }

    
    read_dpu_cpld_reg_info(obj_handle, cpld_info_detial, sizeof(cpld_info_detial));

    
    fwrite_back = fwrite(cpld_info_detial, strlen(cpld_info_detial), 1, fp);
    if (fwrite_back != 1) { 
        debug_log(DLOG_ERROR, "fwrite detial failed, fwrite_back = %zu", fwrite_back);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 dpu_card_dump_cpld_reg(const gchar *path)
{
    gint32 ret;
    FILE *fp = NULL;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar file_name[DPU_CPLD_DUMPINFO_MAX_LEN];

    if (path == NULL) {
        return RET_ERR;
    }
    ret = dfl_get_object_list(CLASS_STORAGE_DPU_CARD, &obj_list);
    if (ret != DFL_OK) {
        return (ret == ERRCODE_OBJECT_NOT_EXIST) ? RET_OK : RET_ERR;
    }

    
    ret = snprintf_truncated_s(file_name, sizeof(file_name), "%s/dpu_card_cpld_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_truncated_s fail, ret = %d", ret);
        return RET_ERR;
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);

    
    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dump_cpld_reg_of_one_dpu((OBJ_HANDLE)obj_note->data, fp);
        if (ret != RET_OK) {
            g_slist_free(obj_list);
            (void)fclose(fp);
            debug_log(DLOG_ERROR, "write dpu cpld info to file failed, ret = %d", ret);
            return ret;
        }
    }

    g_slist_free(obj_list);
    (void)fclose(fp);

    return RET_OK;
}

gint32 method_get_dpucard_ipaddr(OBJ_HANDLE card_obj, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    GVariant *property_value = NULL;

    if (g_ip_updated) {
        property_value = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, g_ip_data,
            DPU_CARD_DATA_ONLY_IP_LEN, sizeof(guint8));
        *output_list = g_slist_append(*output_list, (gpointer)property_value);
        return RET_OK;
    }

    debug_log(DLOG_DEBUG, "%s ip is not updated.", dfl_get_object_name(card_obj));
    return RET_ERR;
}


LOCAL gint32 calc_ip_crc(guint8 data[], guint8 len)
{
    guint8 tmp;
    guint8 index;
    guint8 i;
    guint8 end_index;
    guint8 calc_crc8;
    guint8 real_crc8;

    
    end_index = DPU_IPMI_IPV4_ADDR_LEN - 1;
    for (i = 0; i < DPU_IPMI_IPV4_ADDR_LEN / 2; i++) { // 2, 逆序整理
        index = end_index - i;
        tmp = data[i];
        data[i] = data[index];
        data[index] = tmp;
    }
    end_index = DPU_IPMI_IPV6_ADDR_LEN + DPU_IPMI_IPV4_ADDR_LEN;
    for (i = 0; i < DPU_IPMI_IPV6_ADDR_LEN / 2; i++) { // 2, 逆序整理
        index = end_index - i;
        tmp = data[5 + i]; // 5, ipv6起始
        data[5 + i] = data[index]; // 5, ipv6起始
        data[index] = tmp;
    }

    
    calc_crc8 = dal_cal_crc8(0, data + 5, DPU_IPMI_IPV6_ADDR_LEN + 1); // 5, ipv6起始
    real_crc8 = data[len - 1];
    if (calc_crc8 != real_crc8) {
        debug_log(DLOG_ERROR, "CRC is not correct(calc = 0x%x, real = 0x%x)", calc_crc8, real_crc8);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void update_dpucard_ip_data(OBJ_HANDLE card_obj)
{
    gint32 ret;
    guint8 load_status = 0;
    guint8 out_data[DPU_CARD_IPADDR_READ_LEN] = {0};

    
    ret = dal_get_extern_value_byte(card_obj, PROPERTY_DPU_CARD_SYSTEM_LOADED_STATUS, &load_status, DF_AUTO);
    if (ret != RET_OK || load_status == 0) {
        debug_log(DLOG_DEBUG, "BSP is not ready, obj_name = %s", dfl_get_object_name(card_obj));
        return;
    }

    ret = get_dpu_card_referenced_chip_value(card_obj, DPU_CARD_IPADDR_START_REG, DPU_CARD_IPADDR_READ_LEN,
        (void *)out_data);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s read chip failed, ret = %d", dfl_get_object_name(card_obj), ret);
        return;
    }

    
    if (out_data[28] != 170) { // 28, 卡OS写IP的校验数据， 170(0xaa)， IP写OK
        debug_log(DLOG_DEBUG, "%s IP data is not ready, reg(0x5c) = %d", dfl_get_object_name(card_obj), out_data[28]);
        return;
    }

    
    ret = calc_ip_crc(out_data, DPU_CARD_IPADDR_READ_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s check crc failed", dfl_get_object_name(card_obj));
        return;
    }

    ret = memcpy_s(g_ip_data, sizeof(g_ip_data), out_data, sizeof(out_data));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "memcpy_s:copy ipinfo failed, ret = %d", ret);
        return;
    }
    
    g_ip_updated = DPU_CARD_IP_UPDATED;
}


LOCAL gint32 dpu_card_update_hw_info(OBJ_HANDLE object_handle, gpointer data)
{
    
    update_dpucard_ip_data(object_handle);

    
    return RET_OK;
}


LOCAL void dpu_card_update_info_task(void)
{
    (void)prctl(PR_SET_NAME, (gulong) "updatedpuinfo");

    for (;;) {
        (void)dfl_foreach_object(CLASS_STORAGE_DPU_CARD, dpu_card_update_hw_info, NULL, NULL);
        vos_task_delay(5000); // 5000, 延时5秒
    }
}


gint32 dpu_card_start(void)
{
    gint32 ret;
    gulong task_update_hw_info_id = 0;

    ret = vos_task_create(&task_update_hw_info_id, "update_hw_info", (TASK_ENTRY)dpu_card_update_info_task, 0,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Creat task_update_hw_info_id task failed!(ret = 0x%x)", ret);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 method_dpu_card_set_eid(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    GVariant *eid = NULL;
    GVariant *phy_addr = NULL;
    guint8 is_support = 0;

    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input param null.");
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_DPU_CARD_MCTP_SUPPORT, &is_support);
    if (is_support == 0) {
        debug_log(DLOG_DEBUG, "%s doesn't support mctp function", dfl_get_object_name(object_handle));
        return RET_OK;
    }

    eid = (GVariant *)g_slist_nth_data(input_list, 0);
    ret = dfl_set_property_value(object_handle, PROPERTY_DPU_CARD_EID, eid, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s set eid failed, ret = %d", dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }

    phy_addr = (GVariant *)g_slist_nth_data(input_list, 1);
    ret = dfl_set_property_value(object_handle, PROPERTY_DPU_CARD_PHY_ADDR, phy_addr, DF_NONE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s set phy addr failed, ret = %d", dfl_get_object_name(object_handle), ret);
        return RET_ERR;
    }
    return RET_OK;
}