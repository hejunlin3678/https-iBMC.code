

#include "component/mezz.h"
#include "component/base.h"
#include "media/e2p_large.h"

#define FRUDATA_OBJECT_NOT_EXSIT 0
#define FRUDATA_OBJECT_EXSIT 1
#define MEZZ_PORT_SIZE 12

typedef struct tag_get_mezz_port_obj_s {
    guint8 data[MEZZ_PORT_SIZE];
} GET_MEZZ_PORT_S;

LOCAL gint32 __read_mezz_mac_from_eep(FRU_PRIV_PROPERTY_S *fru_priv, gchar (*mac_add_str)[MAX_MAC_STR_LEN])
{
    MACADDR_AREA_S mac_data[MAX_MAC_NUM] = {0};
    gint32 ret = frudev_read_from_eeprom(fru_priv->fru_property.frudev_handle, fru_priv->area_offsets->mezz_mac_offset,
        mac_data, MAX_MAC_NUM * MACADDR_AREA_SIZE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "call frudev_read_from_eeprom failed, error = %d", ret);
        return ret;
    }

    const guint8 mac_len_correct_value = 2;
    for (guint8 mac_index = 0; mac_index < MAX_MAC_NUM; mac_index++) {
        guint16 crc = arith_make_crc_checksum(0, (const guchar *)&(mac_data[mac_index].len),
            MIN(sizeof(guint16) + mac_data[mac_index].len, sizeof(MACADDR_AREA_S) - sizeof(guint16)));
        if (crc != mac_data[mac_index].crc) {
            debug_log(DLOG_ERROR, "CRC check failed");
            continue;
        }

        guint8 mac_len = (guint8)(mac_data[mac_index].len & 0xFF) - mac_len_correct_value;
        if (mac_len <= 0 || mac_len > sizeof(mac_data[mac_index].data)) {
            debug_log(DLOG_ERROR, "%s incorrect mac_len, mac_len = %d, data_len = %" G_GSIZE_FORMAT, __FUNCTION__,
                mac_len, sizeof(mac_data[mac_index].data));
            continue;
        }

        // 6 and 8 are the data lengths specified in the protocol.
        if (mac_len != 6 && mac_len != 8) {
            debug_log(DLOG_ERROR, "invaild data length %d", mac_len);
            continue;
        }

        frudata_convert_mac_hex2string(mac_data[mac_index].data, mac_add_str[mac_index], MAX_MAC_STR_LEN, mac_len);
    }

    return RET_OK;
}


LOCAL gint32 __lookup_mezz_port_object(OBJ_HANDLE object_handle, GET_MEZZ_PORT_S *info)
{
    if (info == NULL) {
        return FRUDATA_OBJECT_NOT_EXSIT;
    }

    guint8 card_type = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_CARD_DEV_TYPE, &card_type);

    guint8 card_num = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_CARD_DEV_NUMBER, &card_num);

    guint8 port_num = 0;
    (void)dal_get_property_value_byte(object_handle, PROPERTY_PORT_NUMBER, &port_num);

    if ((info->data[0] != card_type) || (info->data[1] != card_num) || (info->data[2] != port_num)) {
        return FRUDATA_OBJECT_NOT_EXSIT;
    }

    errno_t safe_fun_ret =
        memmove_s(info->data + 3, sizeof(info->data) - 3, (guint8 *)&object_handle, sizeof(OBJ_HANDLE));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return FRUDATA_OBJECT_NOT_EXSIT;
    }

    return FRUDATA_OBJECT_EXSIT;
}


LOCAL gint32 __get_mezz_port_obj(guint8 card_dev_type, guint8 card_dev_num, guint8 port_num, OBJ_HANDLE *handle)
{
    GET_MEZZ_PORT_S info = { 0 };
    info.data[0] = card_dev_type;
    info.data[1] = card_dev_num;
    info.data[2] = port_num;

    gint32 result = dfl_foreach_object(CLASS_PCIE_ROOT_PORT_MAP, (OBJECT_FOREACH_FUNC)__lookup_mezz_port_object,
        (gpointer)&info, NULL);
    if (result != FRUDATA_OBJECT_EXSIT) {
        return ERRCODE_OBJECT_NOT_EXIST;
    }

    errno_t safe_fun_ret = memmove_s((guint8 *)handle, sizeof(OBJ_HANDLE), &info.data[3], sizeof(OBJ_HANDLE));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    return result;
}


LOCAL void __task_write_mezzmac_to_eeprom(gpointer parameter)
{
    (void)prctl(PR_SET_NAME, (gulong) "WriteMezzEeprom");

    OBJ_HANDLE fru_obj = 0;
    guint8 fruid = POINTER_TO_UINT8(parameter);
    (void)dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fruid, &fru_obj);

    gint32 ret = dfl_call_class_method(fru_obj, METHOD_FRU_SET_MEZZ_MACADDR, NULL, NULL, NULL);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "__task_write_mezzmac_to_eeprom failed, ret = %d.", ret);
    }

    return;
}

gint32 renew_mezz_mac(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv == NULL) {
        debug_log(DLOG_ERROR, "The fru_priv is null.");
        return RET_ERR;
    }

    FRU_PROPERTY_S *fru_property = &fru_priv->fru_property;

    
    GVariant *property_value = g_variant_new_byte(fru_property->fru_id);
    OBJ_HANDLE mezz_obj_handle = 0;
    gint32 ret =
        dfl_get_specific_object(CLASS_MEZZCARD_NAME, PROPERTY_MEZZCARD_FRU_ID, property_value, &mezz_obj_handle);
    g_variant_unref(property_value);
    property_value = NULL;

    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_specific_object failed, ret = %d.", ret);
        return ret;
    }

    gchar mac_add_str[MAX_MAC_NUM][MAX_MAC_STR_LEN] = {0};
    ret = __read_mezz_mac_from_eep(fru_priv, mac_add_str);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "__read_mezz_mac_from_eep(%s) failed, ret = %d.",
            dfl_get_object_name(fru_property->frudev_handle), ret);
        return ret;
    }

    GSList *input_list = NULL;
    for (gint32 mac_index = 0; mac_index < MAX_MAC_NUM; mac_index++) {
        if (strlen(mac_add_str[mac_index]) == 0) {
            continue;
        }

        
        property_value = g_variant_new_byte(mac_index + 1);
        input_list = g_slist_append(input_list, property_value);

        property_value = g_variant_new_string((const gchar *)mac_add_str[mac_index]);
        input_list = g_slist_append(input_list, property_value);

        
        ret = dfl_call_class_method(mezz_obj_handle, METHOD_SET_MEZZ_MAC_ADDR, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;

        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_call_class_method (%s)failed, ret = %d.", METHOD_SET_MEZZ_MAC_ADDR, ret);
            return ret;
        }
    }

    return ret;
}


gint32 read_mezz_card_mac_addr(OBJ_HANDLE object_handle, gchar *property_name, guchar (*mac_addr)[MAX_MAC_STR_LEN],
    guint8 *mac_len, guint32 mac_addr_num)
{
    if (mac_addr == NULL || mac_addr_num != MAX_MAC_NUM) {
        return RET_ERR;
    }
    if (property_name == NULL) {
        return RET_ERR;
    }

    
    GVariant *property_value = NULL;
    gint32 ret = dfl_get_property_value(object_handle, property_name, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dfl_get_property_value [%s.%s] fail. ret is %d", __FUNCTION__,
            dfl_get_object_name(object_handle), property_name, ret);
        return ret;
    }

    gsize mac_num = 0;
    gchar **mac_vector = g_variant_dup_strv(property_value, &mac_num);
    g_variant_unref(property_value);
    if (mac_vector == NULL) {
        debug_log(DLOG_ERROR, "%s g_variant_dup_strv fail.", __FUNCTION__);
        return RET_ERR;
    }

    if (mac_num != mac_addr_num) {
        debug_log(DLOG_ERROR, "mac_num is incorrect, mac_num = %" G_GSIZE_FORMAT ".", mac_num);
        g_strfreev(mac_vector);
        return RET_ERR;
    }

    gchar mac_addr_str[MAX_MAC_STR_LEN] = {0};
    for (guint32 i = 0; i < mac_addr_num; i++) {
        guint8 len = (guint8)strlen(mac_vector[i]);
        if (len == 0) {
            continue;
        }

        if (len >= MAX_MAC_STR_LEN) {
            debug_log(DLOG_ERROR, "mac str len error ,len = %d.", len);
            g_strfreev(mac_vector);
            return RET_ERR;
        }

        errno_t safe_fun_ret = memmove_s(mac_addr_str, sizeof(mac_addr_str), mac_vector[i], len);
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        }

        frudata_mac_str2int(mac_addr_str, mac_addr[i], MAX_MAC_STR_LEN, &mac_len[i]);
    }

    g_strfreev(mac_vector);

    return RET_OK;
}


gint32 get_mezz_pcie_connect_info(guchar device_type, guint8 device_num, guchar *resp_data, gint32 data_size,
    gint32 read_offset, guint8 read_len, guint8 *end_flag, guint8 *resp_data_len)
{
    if (resp_data == NULL || data_size == 0 || end_flag == NULL || resp_data_len == NULL) {
        return RET_ERR;
    }

    if (device_type != DEVICE_TYPE_MEZZ) {
        debug_log(DLOG_ERROR, "%s device_type = %d not support", __FUNCTION__, device_type);
        return RET_ERR;
    }

    const gint32 max_total_len = 8;
    if (read_offset > max_total_len) {
        return RET_ERR;
    }

    OBJ_HANDLE handle = 0;
    gint32 ret = __get_mezz_port_obj(device_type, device_num, 1, &handle); // 1: port number
    if (ret != FRUDATA_OBJECT_EXSIT) {
        debug_log(DLOG_ERROR, "%s get_mezz_port_obj(%d %d 1) failed. ret(%d)", __FUNCTION__, device_type, device_num,
            ret);
        return ret;
    }

    guchar mezz_pcie_connect[32] = {0};
    (void)dal_get_property_value_byte(handle, PROPERTY_CONNECT_TYPE, &mezz_pcie_connect[0]);
    (void)dal_get_property_value_byte(handle, PROPERTY_BUS_NUMBER, &mezz_pcie_connect[1]);
    (void)dal_get_property_value_byte(handle, PROPERTY_FUNCTION_NUMBER, &mezz_pcie_connect[3]);

    // The value is defined as the u type in the XML file, but is processed as the guchar type after being obtained.
    guint32 tmp_dev_num = 0;
    (void)dal_get_property_value_uint32(handle, PROPERTY_DEVICE_NUMBER, &tmp_dev_num);
    mezz_pcie_connect[2] = tmp_dev_num & 0xff;

    mezz_pcie_connect[4] = 0xff;
    mezz_pcie_connect[5] = 0xff;
    mezz_pcie_connect[6] = 0xff;
    mezz_pcie_connect[7] = 0xff;

    ret = __get_mezz_port_obj(device_type, device_num, 2, &handle); // 2: port number
    if (ret == FRUDATA_OBJECT_EXSIT) {
        (void)dal_get_property_value_byte(handle, PROPERTY_CONNECT_TYPE, &mezz_pcie_connect[4]);
        (void)dal_get_property_value_byte(handle, PROPERTY_BUS_NUMBER, &mezz_pcie_connect[5]);
        (void)dal_get_property_value_byte(handle, PROPERTY_FUNCTION_NUMBER, &mezz_pcie_connect[7]);

        (void)dal_get_property_value_uint32(handle, PROPERTY_DEVICE_NUMBER, &tmp_dev_num);
        mezz_pcie_connect[6] = tmp_dev_num & 0xff;
    }

    *end_flag = 1;
    *resp_data_len = read_len;
    if (read_offset + read_len >= max_total_len) {
        *end_flag = 0;
        *resp_data_len = max_total_len - read_offset;
    }

    if (*resp_data_len == 0) {
        return RET_OK;
    }
    if (memmove_s(resp_data, data_size, &mezz_pcie_connect[read_offset], *resp_data_len) != EOK) {
        *resp_data_len = 0;
        return RET_ERR;
    }

    return RET_OK;
}


void clear_mezz_mac(FRU_PRIV_PROPERTY_S *fru_priv)
{
    if (fru_priv->fru_property.fru_type != MEZZ_CARD) {
        return;
    }

    OBJ_HANDLE handle = 0;
    (void)dal_get_specific_object_byte(CLASS_MEZZCARD_NAME, PROPERTY_MEZZCARD_FRU_ID, fru_priv->fru_property.fru_id,
        &handle);

    GSList *input_list = NULL;
    for (guint8 mac_index = 0; mac_index < MAX_MAC_NUM; mac_index++) {
        
        guint8 mac_id = mac_index + 1;
        
        GVariant *property_value = g_variant_new_byte(mac_id);
        input_list = g_slist_append(input_list, property_value);

        property_value = g_variant_new_string((const gchar *)""); // 写入空MAC
        input_list = g_slist_append(input_list, property_value);

        
        gint32 ret = dfl_call_class_method(handle, METHOD_SET_MEZZ_MAC_ADDR, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;

        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "dfl_call_class_method (%s)failed!(ret: %d).", METHOD_SET_MEZZ_MAC_ADDR, ret);
        }
    }

    
    TASKID task_id_write_eeprom = 0;
    (void)vos_task_create(&task_id_write_eeprom, "ClearMACtoEEPROM", (TASK_ENTRY)__task_write_mezzmac_to_eeprom,
        UINT8_TO_POINTER(fru_priv->fru_property.fru_id), DEFAULT_PRIORITY);
}