

#include "component/sdi.h"


gint32 get_sdi_set_ip_status(guint8 device_type, guint8 device_num, guint8 *status, guint32 *ipaddr, guint16 *vlan)
{
    
    if (device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "device_type is invalid, device_type = %d", device_type);
        return COMP_CODE_CMD_INVALID;
    }

    GVariant *phy_slot = g_variant_new_byte(device_num);
    OBJ_HANDLE sdi_obj = 0;
    gint32 ret = dfl_get_specific_object(CLASS_PCIE_SDI_CARD, PROPERTY_CARD_SLOT, phy_slot, &sdi_obj);
    g_variant_unref(phy_slot);
    if (ret != DFL_OK) {
        return ret;
    }

    GSList *output_list = NULL;
    ret = dfl_call_class_method(sdi_obj, METHOD_GET_SDI_CARD_SET_IP_STATUS, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s call method %s fail", __FUNCTION__, METHOD_GET_SDI_CARD_SET_IP_STATUS);
        return COMP_CODE_UNKNOWN;
    }

    *status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    *ipaddr = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 1));
    *vlan = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 2));

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return RET_OK;
}


gint32 get_sdi_os_loaded_info(guint8 device_type, guint8 device_num, guint8 *os_loaded)
{
    
    if (device_type != DEVICE_TYPE_SDI_CARD) {
        return COMP_CODE_CMD_INVALID;
    }

    GVariant *phy_slot = g_variant_new_byte(device_num);
    OBJ_HANDLE sdi_obj = 0;
    gint32 ret = dfl_get_specific_object(CLASS_PCIE_SDI_CARD, PROPERTY_CARD_SLOT, phy_slot, &sdi_obj);
    g_variant_unref(phy_slot);

    if (ret != DFL_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_extern_value_byte(sdi_obj, PROPERTY_SDI_CARD_SYSTEM_STATUS, os_loaded, DF_AUTO);
    if (ret != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }

    return RET_OK;
}


gint32 get_sdi_boot_order_info(guint8 device_type, guint8 device_num, guint8 *boot_order, guint8 *vaild_type)
{
    OBJ_HANDLE sdi_obj = 0;
    guint8 mcu_support = 0;

    if (device_type != DEVICE_TYPE_SDI_CARD) {
        return COMP_CODE_CMD_INVALID;
    }

    gint32 ret = dal_get_specific_object_byte(CLASS_PCIE_SDI_CARD, PROPERTY_CARD_SLOT, device_num, &sdi_obj);
    if (ret != DFL_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_byte(sdi_obj, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIeSDICard mcu supported fail", __FUNCTION__);
    }
    ret = dal_get_extern_value_byte(sdi_obj, PROPERTY_SDI_CARD_BOOT_ORDER, boot_order, DF_AUTO);
    if (ret != RET_OK) {
        return COMP_CODE_UNKNOWN;
    }
    if (mcu_support == 1) {
        ret = dal_get_extern_value_byte(sdi_obj, PROPERTY_SDI_CARD_BOOT_VALID_TYPE, vaild_type, DF_AUTO);
        if (ret != RET_OK) {
            return COMP_CODE_UNKNOWN;
        }
    }

    return RET_OK;
}


gint32 get_sdi_storage_ip_info(guint8 device_type, guint8 device_num, guint8 *status, guint32 *ipaddr, guint16 *vlan)
{
    OBJ_HANDLE sdi_obj = 0;
    guint8 mcu_support = 0;
    guint16 vlanid = 0;

    if (device_type != DEVICE_TYPE_SDI_CARD) {
        return COMP_CODE_CMD_INVALID;
    }

    gint32 ret = dal_get_specific_object_byte(CLASS_PCIE_SDI_CARD, PROPERTY_CARD_SLOT, device_num, &sdi_obj);
    if (ret != DFL_OK) {
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_byte(sdi_obj, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIeSDICard mcu supported fail", __FUNCTION__);
    }

    if (mcu_support == 1) {
        ret = dal_get_extern_value_uint32(sdi_obj, PROPERTY_SDI_CARD_STORAGE_IP_ADDR, ipaddr, DF_AUTO);
        if (ret != RET_OK) {
            return COMP_CODE_UNKNOWN;
        }
        ret = dal_get_extern_value_uint16(sdi_obj, PROPERTY_SDI_CARD_STORAGE_VLAN, &vlanid, DF_AUTO);
        if (ret != RET_OK) {
            return COMP_CODE_UNKNOWN;
        }
        *vlan = ((vlanid & 0xff) << 8) + ((vlanid >> 8) & 0xff); 
        *status = 1;
    } else {
        GSList *output_list = NULL;
        ret = dfl_call_class_method(sdi_obj, METHOD_GET_SDI_FSA_IP_STATUS, NULL, NULL, &output_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s call method %s fail", __FUNCTION__, METHOD_GET_SDI_CARD_SET_IP_STATUS);
            return COMP_CODE_UNKNOWN;
        }
        *status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        *ipaddr = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 1));
        *vlan = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 2));
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    return RET_OK;
}


gint32 get_sdi_reset_linkage_info(guint8 device_type, guint8 device_num, guint8 *reset_linkage_mode)
{
    OBJ_HANDLE sdi_obj = 0;
    guint8 mcu_support = 0;
    gint32 ret;

    if (device_type != DEVICE_TYPE_SDI_CARD) {
        debug_log(DLOG_ERROR, "%s device_type = %d", __FUNCTION__, device_type);
        return COMP_CODE_CMD_INVALID;
    }

    ret = dal_get_specific_object_byte(CLASS_PCIE_SDI_CARD, PROPERTY_CARD_SLOT, device_num, &sdi_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get object_handle fail, result is %d", ret);
        return COMP_CODE_DATA_NOT_AVAILABLE;
    }

    ret = dal_get_property_value_byte(sdi_obj, PROPERTY_SDI_CARD_MCUSUPPORTED, &mcu_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIeSDICard mcu supported fail, ret = %d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    if (mcu_support != 1) {
        debug_log(DLOG_ERROR, "%s: PCIeSDICard mcu not supported", __FUNCTION__);
        return COMP_CODE_UNKNOWN;
    }

    ret = dal_get_extern_value_byte(sdi_obj, PROPERTY_SDI_RESET_LINKAGE_POLICY, reset_linkage_mode, DF_AUTO);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PCIeSDICard reset linkage fail, ret = %d", __FUNCTION__, ret);
        return COMP_CODE_UNKNOWN;
    }

    return RET_OK;
}