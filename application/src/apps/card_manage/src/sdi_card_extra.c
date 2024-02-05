

#include <arpa/inet.h>
#include "pme_app/pme_app.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"


#include "sdi_card.h"
#include "pcie_card.h"

LOCAL gint32 check_ipmi_reset_linkage_msg_data(const SDI_RESET_LINKAGE_REQ_S *reset_linkage_req);
LOCAL gint32 set_sdicard_reset_linkage(OBJ_HANDLE object_handle, guint8 reset_linkage_mode);

LOCAL void record_operation_log_by_boardid(const void *msg_data, OBJ_HANDLE obj_hnd, guint32 boardid,
    const SDI_SET_BOOT_ORDER_REQ_S *boot_order_req)
{
    gchar orderStr[SDI_MAX_BOOT_ORDER_NUM + 1][SDI_MAX_BOOT_ORDER_STR] = {
        "HDD", "Base Board PXE", "", "Buckle Card1 PXE", "", "Buckle Card2 PXE", "", "Buckle Card3 PXE"
    };
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};
    guint8 slot = 0;

    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    (void)dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    gchar* desc = sdi_desc[0] == 0 ? "SD100" : sdi_desc;

    if (boardid == SDI_CARD_BOARDID_5E || boardid == SDI_CARD_36_BOARDID_AB || boardid == SDI_CARD_BOARDID_AC) {
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard%d(%s) to %s successfully", slot,
            desc, orderStr[boot_order_req->boot_order]);
    } else if (is_valid_sdi_or_dpu_boardid(obj_hnd) == RET_OK) {
        if (boot_order_req->boot_order == NONE_ORDER) {
            ipmi_operation_log(msg_data, "Set the boot option of PCIeCard%d(%s) to %s %s successfully", slot,
                desc, "None", boot_order_req->vaild_type == 0 ? "Once":"Permanent");
        } else {
            ipmi_operation_log(msg_data, "Set the boot option of PCIeCard%d(%s) to %s %s successfully", slot,
                desc, orderStr[boot_order_req->boot_order], boot_order_req->vaild_type == 0 ? "Once":"Permanent");
        }
    } else {
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard%d(%s) to %s successfully", slot,
            desc, boot_order_req->boot_order ? "PXE" : "HDD");
    }
}

gint32 sdi_card_set_boot_order(const void *msg_data, OBJ_HANDLE obj_hnd, guint32 boardid,
    const SDI_SET_BOOT_ORDER_REQ_S *boot_order_req)
{
    gint32 ret;
    SDI_IPMI_RESP_S boot_order_resp;

    guint8 req_len = get_ipmi_src_data_len(msg_data);
    if (req_len != SDI_MAX_BOOT_ORDER_LEN - 1) {
        debug_log(DLOG_ERROR, "%s: req_len(%d) is invalid", __FUNCTION__, req_len);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    ret = dal_set_property_value_byte(obj_hnd, PROPERTY_SDI_CARD_BOOT_ORDER, boot_order_req->boot_order, DF_HW);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set boot_order failed, ret=%d", __FUNCTION__, ret);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    boot_order_resp.comp_code = COMP_CODE_SUCCESS;
    boot_order_resp.manufacturer = boot_order_req->manufacturer;

    record_operation_log_by_boardid(msg_data, obj_hnd, boardid, boot_order_req);

    return ipmi_send_response(msg_data, sizeof(boot_order_resp), (guint8 *)&boot_order_resp);
}

LOCAL guint8 transfer_boot_order_to_mcu(guint8 boot_order)
{
    guint8 new_boot_order = 0;
    
    
    switch (boot_order) {
        case 0:
            new_boot_order = 2;
            break;
        case 1:
            new_boot_order = 1;
            break;
        case 255:
            new_boot_order = 0;
            break;
    }
    return new_boot_order;
}

LOCAL gboolean is_valid_boot_order_valid_type(guint8 valid_type)
{
    return (valid_type == BOOT_ORDER_ONCE || valid_type == BOOT_ORDER_PERMANENT);
}

gint32 set_sdi_info_to_mcu(OBJ_HANDLE obj_handle, guint16 opcode, guint8 *in_data, guint8 in_size)
{
    PROTOCOL_HEAD_PARA head_para = { 0 };
    PM_CHIP_INFO *chip_info = NULL;
    // 复位SDI，设置长度可以为0
    if (in_size < 0 || in_size >255) {
        debug_log(DLOG_ERROR, " size len is error, in_size = %d", in_size);
        return RET_ERR;
    }

    gint32 ret = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj bund data failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }
    
    head_para.opcode = opcode;
    ret = pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, in_data, in_size);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set data to mcu failed, ret is %d", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)chip_info);
        return ret;
    }

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}


gint32 set_sdi_parameters_to_mcu(OBJ_HANDLE obj_handle, guint8 req_param, guint16 opcode, guint8 *in_data,
    guint8 in_size)
{
    PROTOCOL_HEAD_PARA head_para = {0};
    PM_CHIP_INFO *chip_info = NULL;
    // in_data长度区间为[0, 255]
    if (in_size < 0 || in_size > 255) {
        debug_log(DLOG_ERROR, " size len is error, in_size = %u", in_size);
        return RET_ERR;
    }

    gint32 ret = pm_get_binded_data(obj_handle, (gpointer *)&chip_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj binded data failed, ret is %d", __FUNCTION__, ret);
        return ret;
    }
    
    head_para.cmd_arg = req_param;
    head_para.opcode = opcode;
    ret = pm_send_data_by_std_smbus(chip_info, (guint8 *)&head_para, in_data, in_size);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set data to mcu failed, ret is %d", __FUNCTION__, ret);
        pm_unref_binded_data((gpointer)chip_info);
        return ret;
    }

    pm_unref_binded_data((gpointer)chip_info);
    return RET_OK;
}

gint32 sdi_card_set_boot_order_mcu(const void *msg_data, OBJ_HANDLE obj_hnd, guint32 boardid,
    const SDI_SET_BOOT_ORDER_REQ_S *boot_order_req)
{
    gint32 ret;
    SDI_IPMI_RESP_S boot_order_resp;
    OBJ_HANDLE pcie_obj = 0;
    guint8 in_data[4] = {0};    // Request data len is 4

    guint8 req_len = get_ipmi_src_data_len(msg_data);
    if (req_len != SDI_MAX_BOOT_ORDER_LEN || is_valid_boot_order_valid_type(boot_order_req->vaild_type) != TRUE) {
        debug_log(DLOG_ERROR, "%s : SDI card  boot order valid type(%d) or req_len(%d) is invalid",
            __FUNCTION__, boot_order_req->vaild_type, req_len);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    (void)dal_get_specific_object_position(obj_hnd, CLASS_PCIE_CARD, &pcie_obj);
    in_data[0] = 1; // Request data--- data[0]:配置固化选项
    in_data[1] = boot_order_req->vaild_type;
    in_data[2] = 2; // Request data--- data[1]:BIOS引导OS启动方式
    in_data[3] = transfer_boot_order_to_mcu(boot_order_req->boot_order);

    ret = set_sdi_info_to_mcu(pcie_obj, REGISTER_KEY_SET_BOOT_ORDER, in_data, sizeof(in_data));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set boot order failed, ret=%d", __FUNCTION__, ret);
        ipmi_operation_log(msg_data, "Set the boot option of PCIeCard failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    boot_order_resp.comp_code = COMP_CODE_SUCCESS;
    boot_order_resp.manufacturer = boot_order_req->manufacturer;

    record_operation_log_by_boardid(msg_data, obj_hnd, boardid, boot_order_req);

    return ipmi_send_response(msg_data, sizeof(boot_order_resp), (guint8 *)&boot_order_resp);
}

LOCAL void record_operation_log_method(GSList *caller_info, OBJ_HANDLE obj_hnd)
{
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};
    guint8 slot = 0;
    guint8 boot_order = 0;
    guint8 valid_type = 0;
    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    (void)dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    (void)dal_get_property_value_byte(obj_hnd, PROPERTY_SDI_CARD_BOOT_ORDER, &boot_order);
    (void)dal_get_property_value_byte(obj_hnd, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, &valid_type);
    gchar* desc = sdi_desc[0] == 0 ? "SD100" : sdi_desc;
    if (boot_order == NONE_ORDER) {
        method_operation_log(caller_info, NULL, "Set the boot option of PCIeCard%u(%s) to %s %s successfully", slot,
            desc, "None", valid_type == BOOT_ORDER_ONCE ? "Once" : "Permanent");
    } else {
        method_operation_log(caller_info, NULL, "Set the boot option of PCIeCard%u(%s) to %s %s successfully", slot,
            desc, boot_order ? "Base Board PXE" : "HDD", valid_type == BOOT_ORDER_ONCE ? "Once" : "Permanent");
    }
}

LOCAL gboolean is_valid_boot_order(guint8 boot_order)
{
    return (boot_order == HDD_ORDER || boot_order == PXE_ORDER || boot_order == NONE_ORDER);
}

LOCAL gboolean is_valid_type(guint8 valid_type)
{
    return (valid_type == BOOT_ORDER_ONCE || valid_type == BOOT_ORDER_PERMANENT);
}

LOCAL gint32 set_sdicard_prop(OBJ_HANDLE object_handle, guint8 boot_order, guint8 valid_type)
{
    guint32 ret = dal_set_property_value_byte(object_handle, PROPERTY_SDI_CARD_BOOT_ORDER, boot_order, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set BootOrder failed, ret=%d", ret);
        return RET_ERR;
    }
    ret = dal_set_property_value_byte(object_handle, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, valid_type, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "set BootOrderValidType failed, ret=%d", ret);
        return RET_ERR;
    }
    return RET_OK;
}

gint32 method_sdi_card_set_boot_order(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint8 mcu_support = 0;
    gint32 ret;

    if ((input_list == NULL) || (g_slist_length(input_list) != 2)) {
        debug_log(DLOG_ERROR, "%s: input parameter error.", __FUNCTION__);
        return RET_ERR;
    }

    guint8 bootorder = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    if (!is_valid_boot_order(bootorder)) {
        debug_log(DLOG_ERROR, "sdi card boot_order is not valid, boot_order: %u", bootorder);
        return RET_ERR;
    }

    guint8 valid_type = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    if (!is_valid_type(valid_type)) {
        debug_log(DLOG_ERROR, "sdi card valid_type is not valid, valid_type: %u", valid_type);
        return RET_ERR;
    }
    OBJ_HANDLE pcie_obj = 0;
    guint8 in_data[4] = {0};

    (void)dal_get_property_value_byte(object_handle, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (mcu_support == 1) {
        (void)dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj);
        in_data[0] = 1; // Request data--- data[0]:配置固化选项
        in_data[1] = valid_type;
        in_data[2] = 2; // Request data--- data[1]:BIOS引导OS启动方式
        in_data[3] = transfer_boot_order_to_mcu(bootorder);

        ret = set_sdi_info_to_mcu(pcie_obj, REGISTER_KEY_SET_BOOT_ORDER, in_data, sizeof(in_data));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: set boot order failed, ret=%d", __FUNCTION__, ret);
            method_operation_log(caller_info, NULL, "Set the boot option of PCIeCard failed");
            return RET_ERR;
        }
    }
    ret = set_sdicard_prop(object_handle, bootorder, valid_type);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set the boot option of PCIeCard failed");
        return RET_ERR;
    }
    record_operation_log_method(caller_info, object_handle);
    return RET_OK;
}

gboolean is_sdi_support_mcu(OBJ_HANDLE obj_handle)
{
    OBJ_HANDLE slave_obj_handle = 0;
    gchar slave_card_class_name[MAX_NAME_SIZE] = {0};
    gchar class_name[MAX_NAME_SIZE] = {0};

    if (obj_handle == 0) {
        return FALSE;
    }
    // 获取slave card对象
    gint32 ret = dal_get_property_value_string(obj_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_card_class_name,
        sizeof(slave_card_class_name));
    if (ret != RET_OK || strlen(slave_card_class_name) == 0) {
        debug_log(DLOG_DEBUG, "%s: get pcie card slave card class name failed", __FUNCTION__);
        return FALSE;
    }

    if (dal_get_specific_object_position(obj_handle, slave_card_class_name, &slave_obj_handle) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get pcie card slave card object handle failed", __FUNCTION__);
        return FALSE;
    }

    guint8 mcu_support = 0;
    (void)dal_get_property_value_byte(slave_obj_handle, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    (void)dfl_get_class_name(slave_obj_handle, class_name, MAX_NAME_SIZE);
    if (mcu_support == 1 && g_strcmp0(class_name, CLASS_PCIE_SDI_CARD) == 0) {
        debug_log(DLOG_INFO, "%s: SDI CARD %s  support mcu", __FUNCTION__, dfl_get_object_name(obj_handle));
        return TRUE;
    }
    return FALSE;
}


gboolean is_sdi_support_mcu_by_pciecard(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;

    gint32 ret = dfl_get_object_list(CLASS_PCIE_CARD, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return FALSE;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        if (is_sdi_support_mcu(obj_handle) == TRUE) {
            g_slist_free(obj_list);
            return TRUE;
        }
    }
    g_slist_free(obj_list);
    return FALSE;
}


gint32 is_valid_sdi_or_dpu_boardid(OBJ_HANDLE object_handle)
{
    guint32 boardid = 0;
    guint32 i;
    gint32 ret;
    guint32 valid_boardid[] = {
        SDI_CARD_50_BOARDID_A5,
        SDI_CARD_51_BOARDID_A9,
        DPU_CARD_12_BOARDID_ED,
    };
    ret = dal_get_property_value_uint32(object_handle, PROPERTY_SDI_CARD_BOARDID, &boardid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret = %d", __FUNCTION__, dfl_get_object_name(object_handle),
            PROPERTY_SDI_CARD_BOARDID, ret);
        return RET_ERR;
    }

    for (i = 0; i < sizeof(valid_boardid) / sizeof(valid_boardid[0]); i++) {
        if (boardid == valid_boardid[i]) {
            return RET_OK;
        }
    }
    
    return RET_ERR;
}

LOCAL gint32 set_sdicard_nmi(OBJ_HANDLE object_handle)
{
    OBJ_HANDLE pcie_obj = 0;
    guint8 in_data = 0;

    (void)dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj);
    // 设置NMI中断根据硬件提供文档请求不需要添加数据，数据长度为0
    gint32 ret = set_sdi_parameters_to_mcu(pcie_obj, SDI_CARD_NMI_PARAM, REGISTER_KEY_SET_NMI, &in_data, 0);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set sdi card NMI failed, ret=%d", __FUNCTION__, ret);
    }

    return ret;
}


gint32 method_set_sdicard_nmi(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
#define SDI_CARD_NOT_SUPPORT_NMI (-2)
    gint32 ret;
    OBJ_HANDLE obj_hnd = 0;
    gchar card_desc[MAX_PCIE_CARD_DESC_LEN] = {0};

    if ((input_list == NULL) || (g_slist_length(input_list) != 1)) {
        debug_log(DLOG_ERROR, "%s: input parameter error.", __FUNCTION__);
        return RET_ERR;
    }

    guint8 slotid = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    (void)dal_get_specific_object_byte(CLASS_PCIE_SDI_CARD, PROPERTY_SDI_CARD_SLOT, slotid, &obj_hnd);
    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, card_desc, sizeof(card_desc));

    ret = is_valid_sdi_or_dpu_boardid(obj_hnd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: the card not support nmi", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set sdi card NMI failed");
        return SDI_CARD_NOT_SUPPORT_NMI;
    }

    ret = set_sdicard_nmi(obj_hnd);
    if (ret != RET_OK) {
        method_operation_log(caller_info, NULL, "Set PCIeCard%u(%s) NMI failed", slotid, card_desc);
        return RET_ERR;
    }

    method_operation_log(caller_info, NULL, "Set PCIeCard%u(%s) NMI successfully", slotid, card_desc);
    return RET_OK;
}


gint32 ipmi_cmd_set_sdicard_nmi(const void *msg_data, gpointer user_data)
{
    guint8 slot = 0;
    OBJ_HANDLE obj_hnd = 0;
    SDI_IPMI_RESP_S reset_resp = {0};
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};

    const SDI_NMI_REQ_S *nmi_req = (const SDI_NMI_REQ_S *)get_ipmi_src_data(msg_data);
    if (nmi_req == NULL) {
        debug_log(DLOG_ERROR, "%s: nmi_req is NULL", __FUNCTION__);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (nmi_req->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: nmi_req->manufacturer(%d) is invalid", __FUNCTION__, nmi_req->manufacturer);
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    if (nmi_req->device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s: nmi_req->device_type(%d) is invalid", __FUNCTION__, nmi_req->device_type);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
    }

    if (nmi_req->length != 0) {
        debug_log(DLOG_ERROR, "%s: nmi_req->length(%d) is invalid", __FUNCTION__, nmi_req->length);
        return ipmi_send_simple_response(msg_data, COMP_CODE_CMD_INVALID);
    }

    gint32 ret = get_sdi_card_handle_by_logic_num(nmi_req->logical_num, &obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi card NMI failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = is_valid_sdi_or_dpu_boardid(obj_hnd);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: the card not support nmi", __FUNCTION__);
        ipmi_operation_log(msg_data, "Set sdi card NMI failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    (void)dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    ret = set_sdicard_nmi(obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set PCIeCard%u(%s) NMI failed", slot, sdi_desc);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    reset_resp.comp_code = COMP_CODE_SUCCESS;
    reset_resp.manufacturer = nmi_req->manufacturer;

    ipmi_operation_log(msg_data, "Set PCIeCard%u(%s) NMI successfully", slot, sdi_desc);
    return ipmi_send_response(msg_data, sizeof(reset_resp), (guint8 *)&reset_resp);
}


gint32 ipmi_cmd_set_sdicard_reset_linkage(const void *msg_data, gpointer user_data)
{
    guint8 slot = 0;
    OBJ_HANDLE obj_hnd = 0;
    SDI_IPMI_RESP_S reset_resp = {0};
    gchar sdi_desc[MAX_STRING_LENGTH] = {0};
    gint32 ret;

    const SDI_RESET_LINKAGE_REQ_S *reset_linkage_req = (const SDI_RESET_LINKAGE_REQ_S *)get_ipmi_src_data(msg_data);

    ret = check_ipmi_reset_linkage_msg_data(reset_linkage_req);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, ret);
    }

    ret = get_sdi_card_handle_by_logic_num(reset_linkage_req->logical_num, &obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi card RESET LINKAGE failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    ret = is_valid_sdi_or_dpu_boardid(obj_hnd);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set sdi card RESET LINKAGE failed");
        return ipmi_send_simple_response(msg_data, COMP_CODE_OUTOF_RANGE);
    }

    (void)dal_get_extern_value_byte(obj_hnd, PROPERTY_CARD_SLOT, &slot, DF_AUTO);
    (void)dal_get_property_value_string(obj_hnd, PROPERTY_SDI_CARD_DESC, sdi_desc, MAX_STRING_LENGTH);
    ret = set_sdicard_reset_linkage(obj_hnd, reset_linkage_req->data);
    if (ret != RET_OK) {
        ipmi_operation_log(msg_data, "Set PCIeCard%u(%s) RESET LINKAGE failed", slot, sdi_desc);
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    reset_resp.comp_code = COMP_CODE_SUCCESS;
    reset_resp.manufacturer = reset_linkage_req->manufacturer;

    ipmi_operation_log(msg_data, "Set PCIeCard%u(%s) RESET LINKAGE successfully", slot, sdi_desc);
    return ipmi_send_response(msg_data, sizeof(reset_resp), (guint8 *)&reset_resp);
}


LOCAL gint32 check_ipmi_reset_linkage_msg_data(const SDI_RESET_LINKAGE_REQ_S *reset_linkage_req)
{
#define IPMI_END_FLAG 0
    if (reset_linkage_req == NULL) {
        debug_log(DLOG_ERROR, "%s: reset_linkage_req is NULL", __FUNCTION__);
        return COMP_CODE_INVALID_FIELD;
    }

    if (reset_linkage_req->manufacturer != HUAWEI_MFG_ID) {
        debug_log(DLOG_ERROR, "%s: reset_linkage_req->manufacturer(%d) is invalid",
            __FUNCTION__, reset_linkage_req->manufacturer);
        return COMP_CODE_INVALID_FIELD;
    }

    if (reset_linkage_req->device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s: reset_linkage_req->device_type(%d) is invalid",
            __FUNCTION__, reset_linkage_req->device_type);
        return COMP_CODE_CMD_INVALID;
    }

    if (reset_linkage_req->end_flag != IPMI_END_FLAG) {
        debug_log(DLOG_ERROR, "%s: reset_linkage_req->end_flag(%d) is invalid",
            __FUNCTION__, reset_linkage_req->end_flag);
        return COMP_CODE_CMD_INVALID;
    }

    if (reset_linkage_req->length != IPMI_RESET_LINKAGE_DATA_LENGTH) {
        debug_log(DLOG_ERROR, "%s: reset_linkage_req->length(%d) is invalid", __FUNCTION__, reset_linkage_req->length);
        return COMP_CODE_CMD_INVALID;
    }

    if (reset_linkage_req->data != 0 && reset_linkage_req->data != 1) {
        debug_log(DLOG_ERROR, "%s: reset_linkage_req->data(%d) is invalid", __FUNCTION__, reset_linkage_req->data);
        return COMP_CODE_OUTOF_RANGE;
    }

    return RET_OK;
}


LOCAL gint32 set_sdicard_reset_linkage(OBJ_HANDLE object_handle, guint8 reset_linkage_mode)
{
    OBJ_HANDLE pcie_obj = 0;
    gint32 ret;

    (void)dal_get_specific_object_position(object_handle, CLASS_PCIE_CARD, &pcie_obj);
    // 设置NMI中断根据硬件提供文档请求不需要添加数据，数据长度为0
    ret = set_sdi_parameters_to_mcu(pcie_obj, SDI_CARD_PARAM_NONE, REGISTER_KEY_SET_RESET_LINKAGE_POLICY,
        &reset_linkage_mode, IPMI_RESET_LINKAGE_DATA_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set sdi card reset linkage failed, ret=%d", __FUNCTION__, ret);
    }

    return ret;
}