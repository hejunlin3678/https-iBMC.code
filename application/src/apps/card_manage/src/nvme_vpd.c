

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"




const gchar* g_gModelNumberNameArray[] =
{
    "SAMSUNG MZWLL1T6HAJQ",
    "SAMSUNG MZWLL3T2HAJQ",
    "SAMSUNG MZWLL6T4HMLA",
    "SAMSUNG MZWLL12THMLA",
    "SAMSUNG MZQLB960HAJR",
    "SAMSUNG MZQLB1T9HAJR",
    "SAMSUNG MZQLB3T8HALS",
    "SAMSUNG MZQLB7T6HMLA"
};


void pcie_card_vpd_operation_log(guint8 slot_id, guint8 option)
{
    GSList *caller_info_list = NULL;

    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("127.0.0.1"));

    method_operation_log(caller_info_list, NULL, "NVMe Disk%d %s successfully\n", slot_id,
        (PCIE_CARD_XML_LOAD == option) ? "plug in" : "plug out");

    g_slist_free_full(caller_info_list, (GDestroyNotify)g_variant_unref);
    return;
}


gint32 pcie_card_get_vpd_common_connector(guint8 pcie_slot, OBJ_HANDLE *conn_obj)
{
    GSList *conn_list = NULL;
    GSList *conn_node = NULL;
    gint32 ret = RET_ERR;
    guint8 slot = 0;
    gchar type[CONNECTOR_TYPE_MAX_LEN] = {0};
    OBJ_HANDLE connector_obj_handle;

    if (conn_obj == NULL) {
        return RET_ERR;
    }

    // 遍历Connector，根据槽位和Type==NVMeVPDConnector，获取对应连接器句柄
    ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &conn_list);
    if (ret != DFL_OK || g_slist_length(conn_list) == 0) {
        return RET_ERR;
    }

    ret = RET_ERR;

    for (conn_node = conn_list; conn_node; conn_node = conn_node->next) {
        connector_obj_handle = (OBJ_HANDLE)conn_node->data;
        (void)dal_get_property_value_string(connector_obj_handle, PROPERTY_CONNECTOR_TYPE, type, sizeof(type));
        (void)dal_get_property_value_byte(connector_obj_handle, PROPERTY_CONNECTOR_SLOT, &slot);

        if ((strcmp(type, CONNECTOR_TYPE_NVMEVPDCONNECTOR) == 0) && slot == pcie_slot) {
            *conn_obj = connector_obj_handle;
            ret = RET_OK;
            break;
        }
    }

    g_slist_free(conn_list);

    return ret;
}

LOCAL gint32 pcie_card_get_vpd_model_number_value(OBJ_HANDLE conn_handle, guint8 *modelnumber, gsize size)
{
    gint32 ret = RET_ERR;
    gsize length = 0;
    gchar **accessor_name = NULL;
    guint8 mdlnumber[VPD_COMMON_MODEL_NUMBER_LEN] = {0};
    if (modelnumber == NULL) {
        debug_log(DLOG_ERROR, " %s: vpd_ModelNumber is NULL.", __FUNCTION__);
        return RET_ERR;
    }
    ret = dal_get_property_value_strv(conn_handle, CONNECTOR_LEGACY, &accessor_name, &length);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Get Connector Legacy failed, ret=%d.", __FUNCTION__, ret);
        return ret;
    }
    // accessor_name[1]为寄存器VirtualVPDTmpAccessor，用于获取VPD结构中的modelnumber
    if (length <= 1) {
        g_strfreev(accessor_name);
        debug_log(DLOG_ERROR, "%s: Get Accessor illegal.", __FUNCTION__);
        return RET_ERR;
    }
    if (strlen(accessor_name[ACCESS_ARRAY_FOR_GET_MODELNUMBER_VALUE]) == 0) {
        g_strfreev(accessor_name);
        debug_log(DLOG_ERROR, "Accessor name is NULL !");
        return RET_ERR;
    }
    ret = pcie_card_block_read(accessor_name[ACCESS_ARRAY_FOR_GET_MODELNUMBER_VALUE], mdlnumber, size);
    if (ret == RET_OK) {
        (void)memcpy_s(modelnumber, GET_MODEL_NUMBER_LEN, mdlnumber + OFFSET_MODEL_NUMBER_LEN, GET_MODEL_NUMBER_LEN);
        debug_log(DLOG_INFO, "%s: get model number value is %s.", __FUNCTION__, modelnumber);
    } else {
        debug_log(DLOG_ERROR, "%s: get model number value failed.", __FUNCTION__);
    }
    g_strfreev(accessor_name);
    return ret;
}

LOCAL void get_samsung_vpd_type(OBJ_HANDLE conn_handle, guint8 *flag)
{
    guint8 modelnumber[VPD_COMMON_MODEL_NUMBER_LEN] = {0};
    gint32 ret = RET_ERR;

    ret = pcie_card_get_vpd_model_number_value(conn_handle, modelnumber, VPD_COMMON_MODEL_NUMBER_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get model number failed, ret is %d.", __FUNCTION__, ret);
        return;
    }
    size_t count = sizeof(g_gModelNumberNameArray) / sizeof(g_gModelNumberNameArray[0]);
    for (size_t i = 0; i < count; i++) {
        if (strncmp((const gchar *)modelnumber, g_gModelNumberNameArray[i], strlen(g_gModelNumberNameArray[i])) == 0) {
            *flag = SAMSUNG_NVME_VPD_PROTOCOL_SSD_FORM_FACTOR;
            return;
        }
    }
    *flag = NVME_VPD_PROTOCOL_SSD_FORM_FACTOR;
    return;
}

LOCAL void verify_vpd_protocol(guint8 *common_header, gsize header_size, guint8 *vpd_type, OBJ_HANDLE conn_handle)
{
#define SSD_FORM_CLASS_CODE_OFFSET0 0
#define SSD_FORM_CLASS_CODE_OFFSET1 1
#define SSD_FORM_CLASS_CODE_OFFSET2 2
#define MAX_UINT8_BIT_LENGTH 8
#define MAX_UINT16_BIT_LENGTH 16
    guint16 header_sum = 0;
    guint8 check_sum = 0;
    guint8 i = 0;
    guint16 vendor_id = 0;
    guint8 flag = NVME_VPD_PROTOCOL_SSD_FORM_FACTOR;
    
    guint32 ssd_form_class_code = 0x010802;

    if ((common_header == NULL) || (vpd_type == NULL)) {
        return;
    }

    for (i = 0; i < header_size - 1; i++) {
        header_sum = header_sum + common_header[i];
    }

    check_sum = (guint8)(~(guint8)(header_sum % 256)) + 1;

    debug_log(DLOG_DEBUG, "Bit0 ~ Bit%" G_GSIZE_FORMAT " checksum is %x compare with %x", (header_size - 2),
        check_sum, common_header[header_size - 1]);

    
    if (check_sum == common_header[header_size - 1] && (common_header[SSD_FORM_CLASS_CODE_OFFSET0] |
        (common_header[SSD_FORM_CLASS_CODE_OFFSET1] << MAX_UINT8_BIT_LENGTH) |
        (common_header[SSD_FORM_CLASS_CODE_OFFSET2] << MAX_UINT16_BIT_LENGTH)) != ssd_form_class_code) {
        *vpd_type = NVME_VPD_PROTOCOL_NVME_MI;
    } else {
        vendor_id = common_header[NVME_VPD_PROTOCOL_SSD_FORM_VENDOR_ID_OFFSET_L] |
            (common_header[NVME_VPD_PROTOCOL_SSD_FORM_VENDOR_ID_OFFSET_H] << 8);

        if (vendor_id == NVME_VPD_VENDOR_ID_FOR_SAMSUNG) {
            get_samsung_vpd_type(conn_handle, &flag);
            *vpd_type = flag;
        } else {
            *vpd_type = NVME_VPD_PROTOCOL_SSD_FORM_FACTOR;
        }
    }
}

gint32 pcie_card_get_vpd_protocol_connector(guint8 pcie_slot, OBJ_HANDLE *conn_handle)
{
    GSList *conn_list = NULL;
    GSList *conn_node = NULL;
    gint32 ret = RET_ERR;
    guint8 slot = 0;
    gchar type[CONNECTOR_TYPE_MAX_LEN] = {0};
    gchar bom_id[CONNECTOR_BOM_ID_MAX_LEN] = {0};
    OBJ_HANDLE connector_obj_handle;

    if (conn_handle == NULL) {
        return RET_ERR;
    }

    // 遍历Connector，根据槽位和Type==NVMeVPDConnector，获取对应连接器句柄
    ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &conn_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Connector failed.");
        return ret;
    }

    ret = RET_ERR;

    // 遍历Connector，根据槽位和bom==14140224_VPD，获取对应连接器句柄
    for (conn_node = conn_list; conn_node; conn_node = conn_node->next) {
        connector_obj_handle = (OBJ_HANDLE)conn_node->data;
        (void)dal_get_property_value_string(connector_obj_handle, PROPERTY_CONNECTOR_TYPE, type, sizeof(type));
        (void)dal_get_property_value_byte(connector_obj_handle, PROPERTY_CONNECTOR_SLOT, &slot);
        (void)dal_get_property_value_string(connector_obj_handle, PROPERTY_CONNECTOR_BOM, bom_id, sizeof(bom_id));

        if ((strcmp(bom_id, VPD_CONNECTOR_BOM_PROTOCOL) != 0) || (0 != strcmp(type, VIRTUAL_CONNECTOR)) ||
            (slot != pcie_slot)) {
            continue;
        }

        *conn_handle = connector_obj_handle;
        ret = RET_OK;
        break;
    }

    g_slist_free(conn_list);

    return ret;
}

LOCAL gint32 pcie_card_get_vpd_common_header(OBJ_HANDLE conn_handle, guint8 *vpd_header, gsize header_size)
{
    gint32 ret = RET_ERR;
    gsize length = 0;
    GVariant *property_value = NULL;
    const gchar **accessor_name = NULL;

    if (vpd_header == NULL) {
        debug_log(DLOG_ERROR, "vpd_header is NULL.");
        return RET_ERR;
    }

    ret = dfl_get_property_value(conn_handle, CONNECTOR_LEGACY, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get Connector Legacy failed, ret=%d.", ret);
        return ret;
    }

    accessor_name = g_variant_get_strv(property_value, &length);
    if (accessor_name == NULL) {
        g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "Accessor name is NULL.");
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "Connector Legacy length : %" G_GSIZE_FORMAT ", %s", length, accessor_name[0]);

    
    if (strlen(accessor_name[0]) == 0) {
        g_free(accessor_name);
        g_variant_unref(property_value);
        debug_log(DLOG_ERROR, "Accessor name is NULL !");
        return RET_ERR;
    }

    // 读取头文件
    ret = pcie_card_block_read(accessor_name[0], vpd_header, header_size);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Read chip %s fail, ret=%d", accessor_name[0], ret);
        ret = RET_ERR;
    } else {
        debug_log(DLOG_DEBUG, "Read VPD chip common-hearder successfully.");
        ret = RET_OK;
    }

    g_free(accessor_name);
    g_variant_unref(property_value);
    return ret;
}


gint32 get_nvme_protocol(OBJ_HANDLE conn_handle, guint8 *protocol_type)
{
#define READ_VPD_RETRY_TIMES 3
    gint32 ret = RET_OK;
    guint8 common_header[VPD_COMMON_HEADER_LEN] = {0};
    guint8 i = 0;
    guint8 slot_id = 0;

    if (protocol_type == NULL) {
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(conn_handle, CONNECTOR_SLOT, &slot_id);
    for (i = 0; i < READ_VPD_RETRY_TIMES; ++i) {
        if (i != 0) {
            vos_task_delay(READ_VPD_DELAY_TIME);
        }
        ret = pcie_card_get_vpd_common_header(conn_handle, common_header, VPD_COMMON_HEADER_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "Get NVMe[%u] Protocol Header failed, ret = %d", slot_id, ret);
            vos_task_delay(READ_VPD_DELAY_TIME);
            continue;
        }

        debug_log(DLOG_DEBUG, "NVMe slot id:%u, NVMe Protocol Header:%02x%02x%02x%02x%02x%02x%02x%02x", slot_id,
            common_header[0], common_header[1], common_header[2], common_header[3], common_header[4], common_header[5],
            common_header[6], common_header[7]);

        
        verify_vpd_protocol(common_header, VPD_COMMON_HEADER_LEN, protocol_type, conn_handle);

        if (*protocol_type != NVME_VPD_PROTOCOL_NVME_MI) {
            
            (void)memset_s(common_header, sizeof(common_header), 0, sizeof(common_header));
        } else {
            
            break;
        }
    }
    return ret;
}


gint32 pcie_card_get_vmd_state(void)
{
    OBJ_HANDLE prod_handle = 0;
    guint8 state = DISABLE;

    (void)dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &prod_handle);

    (void)dal_get_property_value_byte(prod_handle, PROPERTY_NVME_VMD_ENABLE, &state);

    if (ENABLE != state) {
        return DISABLE;
    }

    return state;
}
