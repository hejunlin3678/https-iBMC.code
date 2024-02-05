

#include "intf/ipmi.h"
#include "protocol.h"
#include "component/component.h"
#include "component/base.h"
#include "prepare_fru.h"
#include "pme_app/uip/ipmc_public.h"

#define TYPE_PCIE_EP_STATUS 2

LOCAL gint32 ipmi_get_fruid_by_uid(const gchar *uid_ipmi, guint8 *resp_data, guint8 *resp_len)
{
#define IPMI_RETURN_FIXED_LEN 5
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar uid_device[MAX_UID_LENGTH] = {0};
    gint32 ret;
    guint8 fruid = 0;
    guint8 fru_cnt = 0;

    ret = dfl_get_object_list(CLASS_COMPONENT, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "there is no %s object, ret = %d", CLASS_COMPONENT, ret);
        return RET_ERR;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_UNIQUE_ID, uid_device, sizeof(uid_device));
        if (g_strcmp0(uid_device, uid_ipmi) == 0) {
            (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_FRUID, &fruid);
            if (fruid == 0xff) {
                debug_log(DLOG_DEBUG, "%s: fruid(%u) is invalid", __FUNCTION__, fruid);
                continue;
            }

            debug_log(DLOG_DEBUG, "uid(%s) fruid(%u)", uid_device, fruid);
            resp_data[IPMI_RETURN_FIXED_LEN + fru_cnt] = fruid;
            fru_cnt++;
            
            if (fru_cnt == 0xff - IPMI_RETURN_FIXED_LEN) {
                break;
            }
        }
    }

    resp_data[4] = fru_cnt;
    *resp_len = fru_cnt + IPMI_RETURN_FIXED_LEN;
    g_slist_free(obj_list);
    return RET_OK;
}


gint32 ipmi_oem_get_fruid_info_from_uid(const void *msg_data, gpointer user_data)
{
    guint8 resp_data[MAX_POSSIBLE_IPMI_FRAME_LEN] = {0};
    gchar uid[MAX_UID_LENGTH] = {0};
    guint8 uid_len = 0;
    errno_t safe_fun_ret = EOK;
    guint8 resp_len = 0;
    gint32 ret;

    const guint8 *src_data = get_ipmi_src_data(msg_data);

    
    resp_data[0] = COMP_CODE_SUCCESS;
    resp_data[1] = 0xdb;
    resp_data[2] = 0x07;
    resp_data[3] = 0x00;

    uid_len = src_data[5];
    if (uid_len >= MAX_UID_LENGTH) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    safe_fun_ret = memmove_s(uid, sizeof(uid) - 1, (const gchar *)&(src_data[6]), uid_len);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    ret = ipmi_get_fruid_by_uid(uid, resp_data, &resp_len);
    if (ret != RET_OK) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_UNKNOWN);
    }

    return ipmi_send_response(msg_data, resp_len, resp_data);
}

gint32 get_sdi_ep_init_state_from_mcu(OBJ_HANDLE sdi_obj, guint8 *inited_state)
{
    OBJ_HANDLE pcie_obj = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    PROTOCOL_HEAD_PARA head_para = { 0 };

    head_para.opcode = REGISTER_KEY_GET_DEVICE_STATUS;
    head_para.input_data[0] = TYPE_PCIE_EP_STATUS;    // Request data--- data[0]: Host侧PCIE EP状态
    head_para.input_data_len = 1;
    
    // 入参链表数据封装
    input_list = g_slist_append(input_list, g_variant_new_uint16(head_para.opcode));
    input_list = g_slist_append(input_list, g_variant_new_byte(head_para.data_object_index));
    input_list = g_slist_append(input_list, g_variant_new_byte(head_para.cmd_arg));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE,
        head_para.input_data, head_para.input_data_len, sizeof(guint8)));

    (void) dal_get_specific_object_position(sdi_obj, CLASS_PCIE_CARD, &pcie_obj);
    gint32 ret = dfl_call_class_method(pcie_obj, METHOD_PCIE_CARD_GET_DATA_FROM_MCU, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if ((ret != RET_OK) || (output_list == NULL)) {
        debug_log(DLOG_ERROR, "%s: fail to get data from mcu:ret(%d)", __FUNCTION__, ret);
        return RET_ERR;
    }
    gsize parm_temp = 0;
    const guint8 *output_data = (const guint8 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(output_list, 0), &parm_temp, sizeof(guint8));
    guint32 output_data_len = (guint32)parm_temp;

    // Response data---BYTE[2n-2]：运行状态n的Type，BYTE[2n-1]：运行状态n的Value
    if ((output_data == NULL) || (output_data_len < 2)) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        debug_log(DLOG_ERROR, "%s: PCIE EP status length is not valid, output_data is NULL or output_data_len is %d.",
            __FUNCTION__, output_data_len);
        return RET_ERR;
    }
    // Host侧PCIE EP状态
    if (output_data[0] == TYPE_PCIE_EP_STATUS) {
        *inited_state = output_data[1];
    }
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}


gint32 __get_init_state_from_related_accessor(OBJ_HANDLE com_obj, const gchar *property_name, guint8 *state)
{
#define SDI_INIT_STATE_REG 0X4C

    gchar accessor_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE acc_obj = 0;
    OBJ_HANDLE chip_obj = 0;
    guint8 read_byte = 0xff;
    const gchar *chip_name = NULL;
    gint32 ret;

    
    (void)dfl_get_property_accessor(com_obj, property_name, accessor_name, MAX_NAME_SIZE);
    (void)dfl_get_object_handle(accessor_name, &acc_obj);
    (void)dfl_get_referenced_object(acc_obj, PROPERTY_ACCESSOR_CHIP, &chip_obj);

    
    chip_name = dfl_get_object_name(chip_obj);

    ret = dfl_chip_blkread(chip_name, SDI_INIT_STATE_REG, 1, &read_byte);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:ret = %d read_byte = %d", __FUNCTION__, ret, read_byte);
        return ret;
    }
    
    *state = (((read_byte >> 4) & 1) == 0);

    return RET_OK;
}