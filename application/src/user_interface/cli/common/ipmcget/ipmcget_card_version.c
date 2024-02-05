
#include "ipmcget_card_version.h"


void print_pangea_card_mcu_ver(void)
{
    gchar mcu_ver[MAX_MCU_FW_VER_LEN] = { 0 };
    gint32 ret;
    gint32 slot_ret;
    guint8 slot_id = INVALID_VAL;
    OBJ_HANDLE card_obj;
    GSList* node = NULL;
    GSList* card_list = NULL;

    ret = dfl_get_object_list(CLASS_PANGEA_PCIE_CARD, &card_list);
    if (ret != DFL_OK) {
        return;
    }

    g_printf("-------------- PCIe Card INFO --------------\r\n");
    for (node = card_list; node; node = node->next) {
        card_obj = (OBJ_HANDLE)node->data;
        ret = dal_get_property_value_string(card_obj, PROPERTY_PANGEA_PCIE_MCUVER, mcu_ver, sizeof(mcu_ver));
        slot_ret = dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_LOGIC_SLOT, &slot_id);
        if (slot_ret != RET_OK || slot_id == 0xff) {
            ret += dal_get_property_value_byte(card_obj, PROPERTY_PCIE_CARD_SLOT_ID, &slot_id);
        }
        if (ret != RET_OK) {
            continue;
        }
        g_printf("PCIeCard%-7d MCUVer:           %s\r\n", slot_id, mcu_ver);
    }
    g_slist_free(card_list);
    return;
}


void print_vrd_version(void)
{
    GSList* handle_list = NULL;
    GSList* list_item = NULL;
    OBJ_HANDLE obj_handle;
    gchar device_name[NAME_LEN] = {0};
    gchar perform_name[NAME_LEN] = {0};
    gchar vrd_version[VRD_VERSION_LENGTH] = {0};
    gboolean bprint_title = TRUE;
    gint32 ret;

    ret = dfl_get_object_list(CLASS_VRD_UPGRADE_MGNT, &handle_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "dfl_get_object_list failed. ret = %d.", ret);
        return;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        ret = dfl_get_referenced_object((OBJ_HANDLE)list_item->data, PROPERTY_COMPONENT_OBJECT, &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_referenced_object failed! obj name: %s, ret = %d.",
                dfl_get_object_name((OBJ_HANDLE)list_item->data), ret);
            continue;
        }
        (void)dal_get_property_value_string(obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        ret = sprintf_s(perform_name, sizeof(perform_name), "%sVRD", device_name);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "sprintf_s fail, ret = %d.", ret);
            continue;
        }
        (void)dal_get_property_value_string((OBJ_HANDLE)list_item->data, PROPERTY_VRDVERSION, vrd_version,
            sizeof(vrd_version));

        if (bprint_title == TRUE) {
            bprint_title = FALSE;
            g_printf("------------------- POWER INFO -------------------\r\n");
        }
        g_printf("%-14s Version:           %s \r\n", perform_name, vrd_version);
    }
    g_slist_free(handle_list);
}