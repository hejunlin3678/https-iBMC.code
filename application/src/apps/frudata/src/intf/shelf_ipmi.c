

#include "intf/ipmi.h"
#include "prepare_fru.h"

LOCAL gint32 read_shelf_serial_number(guint8 offset, guint8 read_len, guint8 *resp_data, guint32 data_len,
    guint32 *resp_len, guint8 *end_flag);
LOCAL gint32 read_shelf_location(guint8 read_len, guint8 *resp_data, guint32 data_len, guint32 *resp_len);
LOCAL gint32 write_shelf_location(const guint8 *ipmi_data);


LOCAL gint32 check_software_type_support_shelf_info(void)
{
    guint8 software_type = 0;
    guint8 as_status = 0;
    gint32 ret;

    (void)dal_get_software_type(&software_type);
    if ((software_type != MGMT_SOFTWARE_TYPE_EM) && (!dal_check_if_vsmm_supported())) {
        debug_log(DLOG_ERROR, "Only vsmm and smm support get_shelf_info.");
        return RET_ERR;
    }
    if (software_type == MGMT_SOFTWARE_TYPE_EM) {
        ret = get_board_active_state(&as_status);
        if (ret != RET_OK || as_status != ACTIVE_STATE) {
            debug_log(DLOG_ERROR, "get_board_active_state failed,ret=%d", ret);
            return RET_ERR;
        }
    }
    return RET_OK;
}


gint32 ipmi_get_shelf_info(const void *msg_data, gpointer user_data)
{
    const guint8 *ipmi_data = NULL;
    guint8 resp_data[MAX_READ_FRU_DATA_LEN + 2] = { 0 };
    guint32 resp_len = 0;
    guint8 end_flag = 0;
    guint32 manu_id = HUAWEI_MFG_ID;
    gint32 ret;

    ret = check_software_type_support_shelf_info();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "check_software_type_support_shelf_info failed,ret=%d", ret);
        ipmi_operation_log(msg_data, "Get shelf info failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }
    ipmi_data = get_ipmi_src_data(msg_data);
    // 检查HUAWEI标记
    if (ipmi_data[0x0] != LONGB0(manu_id) || ipmi_data[0x1] != LONGB1(manu_id) || ipmi_data[0x2] != LONGB2(manu_id)) {
        debug_log(DLOG_ERROR, "ManufactureId is invalid!");
        ipmi_operation_log(msg_data, "Get shelf info failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    // 获取查询的机框信息字段,目前支持返回机框SN、机框location
    guint8 shelf_info = ipmi_data[0x5];
    switch (shelf_info) {
        case SHELF_SERIAL_NUMBER:
            ret = read_shelf_serial_number(ipmi_data[0x6], ipmi_data[0x7], &resp_data[0x5], sizeof(resp_data) - 0x5,
                &resp_len, &end_flag);
            break;
        case SHELF_LOCATION:
            ret = read_shelf_location(ipmi_data[0x7], &resp_data[0x5], sizeof(resp_data) - 0x5, &resp_len);
            break;
        default: // 返回不支持
            ret = COMP_CODE_OUTOF_RANGE;
            break;
    }
    if (ret == COMP_CODE_SUCCESS) {
        resp_data[0x0] = COMP_CODE_SUCCESS;
        resp_data[0x1] = LONGB0(manu_id);
        resp_data[0x2] = LONGB1(manu_id);
        resp_data[0x3] = LONGB2(manu_id);
        resp_data[0x4] = end_flag;
        ipmi_operation_log(msg_data, "Get shelf info successful.");
        return ipmi_send_response(msg_data, 0x5 + resp_len, resp_data);
    } else {
        ipmi_operation_log(msg_data, "Get shelf info failed.");
        return ipmi_send_simple_response(msg_data, (guint8)ret);
    }
}


gint32 ipmi_set_shelf_info(const void *msg_data, gpointer user_data)
{
    const guint8 *ipmi_data = NULL;
    guint8 shelf_info;
    guint8 resp_data[4] = { 0 };
    gint32 ret;
    guint32 manu_id = HUAWEI_MFG_ID;

    if (msg_data == NULL) {
        debug_log(DLOG_ERROR, DB_STR_PARAM_POINTER_NULL);
        ipmi_operation_log(msg_data, "Set shelf info failed.");
        return RET_ERR;
    }

    if (!dal_check_if_vsmm_supported()) {
        debug_log(DLOG_ERROR, "Only vsmm support set_shelf info.");
        ipmi_operation_log(msg_data, "Set shelf info failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_PARA_NOT_SUPPORT);
    }

    ipmi_data = get_ipmi_src_data(msg_data);
    if (ipmi_data == NULL) {
        debug_log(DLOG_ERROR, "ipmi_data is NULL.");
        ipmi_operation_log(msg_data, "Set shelf info failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_DATA_NOT_AVAILABLE);
    }

    // 检查HUAWEI标记
    if (ipmi_data[0x0] != LONGB0(manu_id) || ipmi_data[0x1] != LONGB1(manu_id) || ipmi_data[0x2] != LONGB2(manu_id)) {
        debug_log(DLOG_ERROR, "ManufactureId is invalid!");
        ipmi_operation_log(msg_data, "Set shelf info failed.");
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_CMD);
    }

    // 获取设置的机框信息字段,目前只支持机框location
    shelf_info = ipmi_data[0x5];
    switch (shelf_info) {
        case SHELF_LOCATION:
            ret = write_shelf_location(ipmi_data);
            break;
        default:
            ret = COMP_CODE_OUTOF_RANGE;
            break;
    }
    if (ret == COMP_CODE_SUCCESS) {
        resp_data[0x0] = COMP_CODE_SUCCESS;
        resp_data[0x1] = LONGB0(manu_id);
        resp_data[0x2] = LONGB1(manu_id);
        resp_data[0x3] = LONGB2(manu_id);
        ipmi_operation_log(msg_data, "Set shelf info successful!");
        return ipmi_send_response(msg_data, sizeof(resp_data), resp_data);
    } else {
        debug_log(DLOG_ERROR, "Set shelf info failed, ret=%d", ret);
        ipmi_operation_log(msg_data, "Set shelf info failed.");
        return ipmi_send_simple_response(msg_data, ret);
    }
}

LOCAL gint32 get_shelf_elabel_handle(OBJ_HANDLE *elabel_handle)
{
    gint32 ret;
    OBJ_HANDLE fru_handle = 0;

    // 获取电子标签句柄
    ret = dal_get_specific_object_uint32(CLASS_FRU, PROPERTY_POSITION, SMM_BACK_PLANE_POSITION, &fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object of class(%s) by property(%s) value(%u) failed, err code %d", __func__,
            CLASS_FRU, PROPERTY_POSITION, SMM_BACK_PLANE_POSITION, ret);
        return RET_ERR;
    }
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s] Get referenced object(%s) of object(%s) failed, err code %d", __func__,
            PROPERTY_FRU_ELABEL_RO, dfl_get_object_name(fru_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 read_shelf_serial_number(guint8 offset, guint8 read_len, guint8 *resp_data, guint32 data_len,
    guint32 *resp_len, guint8 *end_flag)
{
    errno_t safe_fun_ret;
    GVariant *sn_variant = NULL;
    const gchar *sn_str = NULL;
    gsize sn_length = 0;
    gint32 ret;
    OBJ_HANDLE elabel_handle = 0;

    ret = get_shelf_elabel_handle(&elabel_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get_shelf_elabel_handle failed, ret = %d", __func__, ret);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }
    // 读取电子标签数据
    ret = dfl_get_property_value(elabel_handle, PROPERTY_ELABEL_CHASSIS_SN, &sn_variant);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get property(ChassisSerialNumber) of BackPlaneElabel failed, ret = %d", ret);
        return RET_ERR;
    }

    sn_str = g_variant_get_string(sn_variant, &sn_length);
    if (sn_str == NULL) {
        debug_log(DLOG_ERROR, "[%s] Get string from gvariant failed, return is null", __func__);
        g_variant_unref(sn_variant);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }
    if (sn_length == 0) { // 电子标签长度为零
        *end_flag = 0x80;
        *resp_len = 0;
        g_variant_unref(sn_variant);
        return COMP_CODE_SUCCESS;
    }
    if ((offset + read_len) < sn_length) { // 未读取完
        *end_flag = 0x00;
    } else { // 读取完毕
        *end_flag = 0x80;
    }
    if (offset > sn_length) { // 偏移量超过序列号长度
        *resp_len = 0;
    } else {
        // 希望读取长度超过实际长度，返回可以读取的长度
        // 否则，返回希望读取的长度
        *resp_len = ((read_len + offset) > sn_length) ? (sn_length - offset) : read_len;
        if (*resp_len != 0) {
            safe_fun_ret = memcpy_s(resp_data, data_len, sn_str + offset, *resp_len);
            if (safe_fun_ret != EOK) {
                debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __func__, safe_fun_ret);
                g_variant_unref(sn_variant);
                return COMP_CODE_UNKNOWN;
            }
        }
    }
    g_variant_unref(sn_variant);
    return COMP_CODE_SUCCESS;
}


LOCAL gint32 read_shelf_location(guint8 read_len, guint8 *resp_data, guint32 data_len, guint32 *resp_len)
{
    errno_t safe_fun_ret;
    GVariant *variant = NULL;
    gchar location[CHASSIS_LOCATION_LEN + 1] = { 0 };
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_DFL_HANDLE_NULL);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_CHASSIS_LOCATION, location, sizeof(location));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret);
        return RET_ERR;
    }

    *resp_len = read_len;
    if (*resp_len != 0) {
        safe_fun_ret = memcpy_s(resp_data, data_len, location, *resp_len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __func__, safe_fun_ret);
            g_variant_unref(variant);
            return RET_ERR;
        }
    }

    g_variant_unref(variant);
    return RET_OK;
}

LOCAL gint32 write_shelf_location(const guint8 *ipmi_data)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 location[CHASSIS_LOCATION_LEN + 1] = { 0 };
    gint32 ret_val;
    GSList *input_list = NULL;
    errno_t safe_fun_ret;
    guint8 write_eeprom_flag = 0;
    guint8 broadcast_flag = 0;

    safe_fun_ret = memcpy_s(location, sizeof(location), &ipmi_data[0x9], ipmi_data[0x8]);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __func__, safe_fun_ret);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret_val = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, DB_STR_FORMAT_ERR, ret_val);
        return COMP_CODE_UNKNOWN;
    }
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)location));
    input_list = g_slist_append(input_list, g_variant_new_byte(write_eeprom_flag));
    input_list = g_slist_append(input_list, g_variant_new_byte(broadcast_flag));
    ret_val = dfl_call_class_method(obj_handle, METHOD_CHASSIS_SET_CHASSIS_LOCATION, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, "Call SetChassisLocation method failed, ret:%d", ret_val);
        return COMP_CODE_CANNOT_RESPONSE;
    }
    return COMP_CODE_SUCCESS;
}
