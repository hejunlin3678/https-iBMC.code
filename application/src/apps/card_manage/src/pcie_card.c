

#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <math.h>
#include "pme_app/pme_app.h"
#include "pcie_card.h"
#include "pcie_card_parse_bdf_thread.h"
#include "card_ipmi_msg.h"
#include "fpga_card.h"
#include "get_version.h"
#include "card_manage_module.h"
#include "share_card.h"
#include "pcie_card_comm.h"
#include "pme_app/common/debug_log_macro.h"
#include "../../../libs/pcie_mgmt/inc/pcie_mgmt_by_std_smbus.h"
#include "soc_board.h"



PCIE_CARD_GROUP_INFO_S g_pcie_group;
PCIE_CARD_GROUP_INFO_S g_pcie_disk_group;
PCIE_CARD_STATUS_GROUP_S g_pcie_status_group;
PCIE_CARD_GROUP_SUB_INFO_S g_pcie_group_sub_info;

PCIE_CARD_GROUP_SUB_INFO_S g_pcie_disk_group_sub_info;

PCIE_CARD_GROUP_BDF_INFO_S g_pcie_cards_bdf_info;
PCIE_CARD_GROUP_BDF_INFO_S g_pcie_disks_bdf_info;
PCIE_CARD_STATUS_GROUP_S g_pcie_status_by_bios_group;

PCIE_CARD_GROUP_INFO_S g_ocp_group;
PCIE_CARD_GROUP_SUB_INFO_S g_ocp_group_sub_info;
PCIE_CARD_GROUP_BDF_INFO_S g_ocp_bdf_info;
PORT_GROUP_BDF_INFO_S g_pcie_port_rootbdf_info;
PORT_GROUP_BDF_INFO_S g_ocp_port_rootbdf_info;
PORT_GROUP_BDF_INFO_S g_pcie_raid_rootbdf_info;



NODE_PCIE_GROUP_SUMMARY_INFO g_pcie_summary_info;


LOCAL TASKID g_status_hotplug_task = 0x00;
LOCAL TASKID g_status_pcie_card_pdate_task = 0x00;
LOCAL gboolean g_status_hotplug_task_exit = FALSE;


guint8 g_bdf_info_parsed_status = BDF_UNPARSED_STATE;




gulong g_set_pcie_card_info_lock = 0;
gulong g_set_pcie_disk_info_lock = 0;



gulong g_set_node_pcie_summary_lock = 0;



GAsyncQueue *g_pcie_dec_bandwidth_msg_queue = NULL;
guint32 g_pcie_alarm_state = 0;



LOCAL GSList *g_deleting_pos_list = NULL;
LOCAL pthread_mutex_t g_pos_list_mutex = PTHREAD_MUTEX_INITIALIZER;
LOCAL void _delete_item_from_deleting_pos_list(guint32 pos);


typedef struct pcie_card_ipmi_info {
    guint8 slot_id;
    PCIE_CARD_GROUP_INFO_S pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S pcie_card_sub_info;
    PCIE_CARD_STATUS_GROUP_S pcie_card_status_info;
} PCIE_CARD_IPMI_INFO_S;

LOCAL void pcie_card_info_per_save(PCIE_CARD_IPMI_INFO_S *pcie_card_data);
LOCAL void pcie_card_xml_load_for_bios(PCIE_CARD_IPMI_INFO_S *pcie_card_data);
LOCAL void pcie_card_xml_unload_for_bios(PCIE_CARD_IPMI_INFO_S *pcie_card_data);
LOCAL void pcie_card_info_parse(PCIE_CARD_IPMI_INFO_S *pcie_card_data, const guint8 *req_data_buf);
LOCAL void pcie_card_operation_log(guint8 type, guint8 slot_id, guint8 option);

LOCAL PER_S persistance_tbl[] = {
    {"PCIE_CARDS_INFO", PER_MODE_RESET, (guint8 *)&g_pcie_group, sizeof(g_pcie_group)},
    {"PCIE_DISK_CARDS_INFO", PER_MODE_RESET, (guint8 *)&g_pcie_disk_group, sizeof(g_pcie_disk_group)},
    {"PCIE_CARDS_STATUS_INFO", PER_MODE_RESET, (guint8 *)&g_pcie_status_group, sizeof(g_pcie_status_group)},
    {"PCIE_CARDS_SUB_INFO", PER_MODE_RESET, (guint8 *)&g_pcie_group_sub_info, sizeof(g_pcie_group_sub_info)},
    {"PCIE_CARDS_BDF_INFO", PER_MODE_RESET, (guint8 *)&g_pcie_cards_bdf_info, sizeof(g_pcie_cards_bdf_info)},
    {"PCIE_DISKS_BDF_INFO", PER_MODE_RESET, (guint8 *)&g_pcie_disks_bdf_info, sizeof(g_pcie_disks_bdf_info)},
    {"PCIE_CARDS_STATUS_BY_BIOS_INFO",
        PER_MODE_RESET,
        (guint8 *)&g_pcie_status_by_bios_group,
        sizeof(g_pcie_status_by_bios_group)},
    
    {"PCIE_DISK_CARDS_SUB_INFO",
        PER_MODE_RESET,
        (guint8 *)&g_pcie_disk_group_sub_info,
        sizeof(g_pcie_disk_group_sub_info)},
    
    
    {"PCIE_ALARM_STATUS", PER_MODE_RESET, (guint8 *)&g_pcie_alarm_state, (sizeof(g_pcie_alarm_state))},
    
    {"OCP_CARDS_INFO", PER_MODE_RESET, (guint8 *)&g_ocp_group, sizeof(g_ocp_group)},
    {"OCP_CARDS_SUB_INFO", PER_MODE_RESET, (guint8 *)&g_ocp_group_sub_info, sizeof(g_ocp_group_sub_info)},
    {"OCP_CARDS_BDF_INFO", PER_MODE_RESET, (guint8 *)&g_ocp_bdf_info, sizeof(g_ocp_bdf_info)},
    {"PCIE_PORT_ROOTBDF_INFO", PER_MODE_RESET, (guint8*)&g_pcie_port_rootbdf_info, sizeof(g_pcie_port_rootbdf_info)},
    {"OCP_PORT_ROOTBDF_INFO", PER_MODE_RESET, (guint8*)&g_ocp_port_rootbdf_info, sizeof(g_ocp_port_rootbdf_info)},
    {"PCIE_RAID_ROOTBDF_INFO", PER_MODE_RESET, (guint8*)&g_pcie_raid_rootbdf_info, sizeof(g_pcie_raid_rootbdf_info)}
};
LOCAL CARD_TYPE_TO_STRING_S g_card_type_string_table[] =
{
    {PCIE_AND_PCIEPLUS_CONNECTOR, CONNECTOR_TYPE_PCIE},
    {DISK_CONNECTOR, CONNECTOR_TYPE_DISKCONNECTOR},
    {NVME_VPD_CONNECTOR, CONNECTOR_TYPE_NVMEVPDCONNECTOR},
    {OCPCARD_CONNECTOR, CONNECTOR_TYPE_OCPCARDCONNECTOR},
    {PANGEA_CARD_CONNECTOR, CONNECTOR_TYPE_PANGEA_CARD},
    {CHIP_CONNECTOR, CONNECTOR_TYPE_CHIP}
};

LOCAL const gchar* g_card_type_string[] =
{
    "pcie card",  // PCIE_AND_PCIEPLUS_CONNECTOR
    "nvme disk",  // DISK_CONNECTOR
    "nvme disk",  // NVME_VPD_CONNECTOR
    "ocp card",   // OCPCARD_CONNECTOR
    "pangea card" // PANGEA_CARD
};


LOCAL const gchar *pcie_card_get_connector_type_string(guint8 type_code)
{
    guint32 i;
    for (i = 0; i < sizeof(g_card_type_string_table) / sizeof(CARD_TYPE_TO_STRING_S); i++) {
        if (g_card_type_string_table[i].type == type_code) {
            return g_card_type_string_table[i].type_string;
        }
    }

    return NULL;
}

const gchar *pcie_card_get_card_type_string(guint8 type_code)
{
    if (type_code > (sizeof(g_card_type_string) / sizeof(gchar *))) {
        return g_card_type_string[0];
    }
    // type_code是从1开始，因此要减1
    return g_card_type_string[type_code - 1];
}

LOCAL gboolean local_pcie_mgnt_enabled(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    guint8 local_pcie_support = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PME_LOCAL_PCIE_MGNT_SUPPORT, &local_pcie_support);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s failed. ret=%d", __FUNCTION__, PROPERTY_PME_LOCAL_PCIE_MGNT_SUPPORT,
            ret);
        return FALSE;
    }
    return (local_pcie_support == 0) ? FALSE : TRUE;
}

gboolean pcie_card_mgnt_ready(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    guint8 smbios_status = SMBIOS_WRITE_NOT_START;

    
    if (local_pcie_mgnt_enabled() == TRUE) {
        return TRUE;
    }
    ret = dal_get_object_handle_nth(CLASS_NAME_SMBIOS, 0, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SMBIOS_STATUS_VALUE, &smbios_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get prop %s failed. ret=%d", __FUNCTION__, PROPERTY_SMBIOS_STATUS_VALUE, ret);
        return FALSE;
    }

    if (smbios_status == SMBIOS_WRITE_FINISH) {
        return TRUE;
    }

    debug_log(DLOG_INFO, "%s: %s is %d, expect %d", __FUNCTION__, PROPERTY_SMBIOS_STATUS_VALUE, smbios_status,
        SMBIOS_WRITE_FINISH);
    return FALSE;
}






guchar pcie_card_get_me_access_type(void)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;

    ret = dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);
    if ((ret == RET_OK) && (obj_handle != 0)) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_ME_ACCESS_TYPE, &me_access_type);
    }

    return me_access_type;
}
void pcie_card_log_operation_log(guint8 type, guint8 slot_id, guint16 vender_id, guint16 device_id, guint8 option)
{
    GSList *caller_info_list = NULL;

    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("127.0.0.1"));

    
    method_operation_log(caller_info_list, NULL, "%s card%d(vendor id: 0x%04x, device id: 0x%04x) %s successfully\n",
        (type == OCPCARD_CONNECTOR) ? "OCP" : "PCIe", slot_id, vender_id, device_id,
        (PCIE_CARD_XML_LOAD == option) ? "plug in" : "plug out");
    g_slist_free_full(caller_info_list, (GDestroyNotify)g_variant_unref);
    
    return;
}

gint32 pcie_disk_get_connector_slot_id(guint8 *slot_id, guint32 *connector_id)
{
    gint32 ret = 0;
    guchar slot = 0;
    guint32 connectorid = 0;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar connector_type[64] = {0};

    if (slot_id == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROPERTY_CONNECTOR_TYPE, connector_type,
            sizeof(connector_type));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get connector type failed!");
            continue;
            
        }

        if (strcmp(connector_type, CONNECTOR_TYPE_PCIEPLUS) != 0) {
            continue;
        }
        
        if (dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_CONNECTOR_SLOT, &slot) == RET_OK &&
            dal_get_property_value_uint32((OBJ_HANDLE)obj_note->data, PROPERTY_CONNECTOR_ID, &connectorid) == RET_OK) {
            *slot_id = slot - 1;
            *connector_id = connectorid;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }

    g_slist_free(obj_list);

    return RET_ERR;
}


gint32 pcie_card_set_obj_property(OBJ_HANDLE handle, const gchar *name, const void *indata, PCIE_CARD_P_TYPE_E type,
    guint8 option)
{
    gint32 result = 0x00;
    gint32 ret_value = RET_OK;
    GVariant *property_data = NULL;
    guint8 slot_id;

    switch (type) {
        case PCIE_CARD_P_TYPE_BYTE:
            property_data = g_variant_new_byte(*((const guint8 *)indata));
            break;

        case PCIE_CARD_P_TYPE_UINT16:
            property_data = g_variant_new_uint16(*((const guint16 *)indata));
            break;

        case PCIE_CARD_P_TYPE_UINT32:
            property_data = g_variant_new_uint32(*((const guint32 *)indata));
            break;

        case PCIE_CARD_P_TYPE_DOUBLE:
            property_data = g_variant_new_double(*((const gdouble *)indata));
            break;

        case PCIE_CARD_P_TYPE_STRING:
            property_data = g_variant_new_string((const gchar *)indata);
            break;

        default:
            return RET_ERR;
    }

    result = dfl_set_property_value(handle, name, property_data, option);
    g_variant_unref(property_data);

    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: Set %s property fail", dfl_get_object_name(handle), name);
        ret_value = RET_ERR;
    }

    
    if (strcmp(name, PROPERTY_PCIECARD_FATALERROR) == 0) {
        result = dfl_get_property_value(handle, PROPETRY_PCIECARD_SLOT, &property_data);
        if (result != DFL_OK) {
            debug_log(DLOG_ERROR, "result is fail");
            return RET_ERR;
        }
        slot_id = g_variant_get_byte(property_data);
        g_variant_unref(property_data);
        
        g_pcie_status_group.status[slot_id - 1] = *(const guint8 *)indata;
        per_save((guint8 *)&g_pcie_status_group);
        
    }

    

    if (strcmp(name, PROPERTY_PCIECARD_FALT_BY_BIOS) == 0) {
        (void)dal_get_property_value_byte(handle, PROPETRY_PCIECARD_SLOT, &slot_id);
        if (slot_id > MAX_PCIE_CARD) {
            debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, slot_id);
            return RET_ERR;
        }
        g_pcie_status_by_bios_group.status[slot_id - 1] = *(const guint8 *)indata;
        per_save((guint8 *)&g_pcie_status_by_bios_group);
    }

    return ret_value;
}

void pcie_set_device_status_to_log(const guint8 *dev_status, guint32 dev_status_len)
{
    guint8 event_type = 0;
    guint8 bus_id = 0;
    guint8 device_id = 0;
    guint8 function_id = 0;
    static PCIE_EVENT_TYPE pcie_event[] =
    {
        {PCIE_DEV_STAT_TYPE0, "PCIe Device Fatal Error"},
        {PCIE_DEV_STAT_TYPE1, "Completion timeout on NP transactions outstanding on PCI Express/DMI"},
        {PCIE_DEV_STAT_TYPE2, "Received PCIe Poisoned TLP"},
        {PCIE_DEV_STAT_TYPE3, "Received PCIe unexpected Completion"},
        {PCIE_DEV_STAT_TYPE4, "PCIe Flow Control Protocol Error"},
        {PCIE_DEV_STAT_TYPE5, "Received a Request from a downstream component that is unsupported"},
        {PCIE_DEV_STAT_TYPE6, "Received a Request from a downstream component that is to be completer aborted"},
        {PCIE_DEV_STAT_TYPE7, "ACS Violation"},
        {PCIE_DEV_STAT_TYPE8, "PCIe Malformed TLP"},
        {PCIE_DEV_STAT_TYPE9, "PCIe Data Link Protocol Error"},
        {PCIE_DEV_STAT_TYPE10, "PCIe Receiver Overflow"},
        {PCIE_DEV_STAT_TYPE11, "Surprise Down"},
        {PCIE_DEV_STAT_TYPE12, "Master Data Parity Error"},
        {PCIE_DEV_STAT_TYPE13, "Receive Target Abort"},
        {PCIE_DEV_STAT_TYPE14, "Signal System Error"},
        {PCIE_DEV_STAT_TYPE15, "Detected Parity Error"},
        {PCIE_DEV_STAT_TYPE16, "PCIe Bandwidth Error"},
        {PCIE_DEV_STAT_TYPE17, "MMIO resoure not enough"},
        {PCIE_DEV_STAT_TYPE18, "legacy IO resoure not enough"},
        {PCIE_DEV_STAT_TYPE19, "legacy oprom resoure not enough"},
        {PCIE_DEV_STAT_TYPE20, "Reserve Memory Resource Realocate"},
    };

    if (dev_status == NULL || dev_status_len < 4) {
        return;
    }

    event_type = dev_status[0];
    bus_id = dev_status[1];
    device_id = dev_status[2];
    function_id = dev_status[3];
    size_t pcie_event_num = sizeof(pcie_event) / sizeof(pcie_event[0]);

    debug_log(DLOG_DEBUG,
        "pcie_set_device_status_to_log: event_num(%zu) event_type(0x%x) bus(0x%x) device(0x%x) function(0x%x)!",
        pcie_event_num, event_type, bus_id, device_id, function_id);

    for (size_t i = 0; i < pcie_event_num; i++) {
        if (event_type == pcie_event[i].event_type) {
            maintenance_log_v2(MLOG_ERROR, FC_CARD_MNG_PCIE_ERR,
                "BIOS, PCIe, %s, Bus(0x%x) Device(0x%x) Function(0x%x).\r\n", pcie_event[i].event_info, bus_id,
                device_id, function_id);
            return;
        }
    }

    debug_log(DLOG_ERROR,
        "pcie_set_device_status_to_log: pcie_event_type(0x%x) is not support, bus(0x%x) device(0x%x) function(0x%x)!",
        event_type, bus_id, device_id, function_id);
    return;
}

gint32 pcie_card_get_one_valid_line(FILE *fp, gchar line[], gint32 len)
{
    while (vos_fgets_s(line, len, fp) != NULL) {
        if (*line == '#') {
            continue;
        }

        
        if (strlen(line) <= 4) {
            continue;
        }

        return RET_OK;
    }

    return RET_ERR;
}


LOCAL gint32 pcie_cmpbyte(gconstpointer a, gconstpointer b)
{
    const guint8 *i1 = (const guint8 *)a;
    const guint8 *i2 = (const guint8 *)b;

    return (gint32)(*i1) - (gint32)(*i2);
}


gint32 pcie_get_slot_by_index(guint8 pcie_type, guint8 pcie_index, guint8 *slot_idx)
{
    gint32 retv = RET_OK;
    GByteArray *slot_array = NULL;

    if (slot_idx == NULL) {
        debug_log(DLOG_ERROR, "Failed : input parameter is NULL !");
        return RET_ERR;
    }

    
    if (DISK_CONNECTOR != pcie_type) {
        *slot_idx = pcie_index + 1;
        return RET_OK;
    }

    
    slot_array = g_byte_array_new();
    if (slot_array == NULL) {
        debug_log(DLOG_DEBUG, "g_byte_array_new failed.");
        return RET_ERR;
    }

    pcie_get_backplane_slot_array(CLASS_HDDBACKPLANE_NAME, &slot_array);
    pcie_get_backplane_slot_array(CLASS_VIRTUAL_HDDBACKPLANE_NAME, &slot_array);

    
    if ((slot_array->len == 0) || (pcie_index >= slot_array->len)) {
        g_byte_array_free(slot_array, TRUE);
        return RET_ERR;
    }

    
    g_byte_array_sort(slot_array, pcie_cmpbyte);

    *slot_idx = slot_array->data[pcie_index];

    g_byte_array_free(slot_array, TRUE);
    return retv;
}


gint32 pcie_get_index_by_slot(guint8 pcie_type, guint8 slot_idx, guint8 *pcie_index)
{
    gint32 retv = RET_OK;
    guint32 idx = 0;
    GByteArray *slot_array = NULL;

    if (pcie_index == NULL) {
        debug_log(DLOG_ERROR, "Failed : input parameter is NULL !");
        return RET_ERR;
    }

    
    if (pcie_type == PCIE_AND_PCIEPLUS_CONNECTOR || pcie_type == OCPCARD_CONNECTOR) {
        *pcie_index = slot_idx - 1;
        return RET_OK;
    }

    
    slot_array = g_byte_array_new();
    if (slot_array == NULL) {
        debug_log(DLOG_DEBUG, "g_byte_array_new failed.");
        return RET_ERR;
    }

    pcie_get_backplane_slot_array(CLASS_HDDBACKPLANE_NAME, &slot_array);
    pcie_get_backplane_slot_array(CLASS_VIRTUAL_HDDBACKPLANE_NAME, &slot_array);

    
    if (slot_array->len == 0) {
        g_byte_array_free(slot_array, TRUE);
        debug_log(DLOG_DEBUG, "Get slot index array failed .");
        return RET_ERR;
    }

    
    g_byte_array_sort(slot_array, pcie_cmpbyte);

    
    retv = RET_ERR;

    for (idx = 0; idx < slot_array->len; idx++) {
        if (slot_array->data[idx] == slot_idx) {
            *pcie_index = (guint8)idx;
            retv = RET_OK;
            break;
        }
    }

    g_byte_array_free(slot_array, TRUE);

    return retv;
}


gint32 pcie_get_cardtype_by_cardaction(OBJ_HANDLE action_handle, guint8 *pcie_type)
{
    gchar obj_name[MAX_NAME_SIZE] = {0};
    gchar property_name[MAX_NAME_SIZE] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar connect_type[MAX_NAME_SIZE] = {0};

    
    gint32 ret = dfl_get_referenced_property(action_handle, PROPERTY_UPGRADE_ACTION_DATA, obj_name, property_name,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get cardaction referenced object failed. ");
        return RET_ERR;
    }

    
    ret = dfl_get_object_handle(obj_name, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get connector handle failed. obj_name : %s .", obj_name);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_CONNECTOR_TYPE, connect_type, sizeof(connect_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get connector type failed. obj_name : %s .", obj_name);
        return RET_ERR;
    }

    if (strncmp(connect_type, CONNECTOR_TYPE_DISKCONNECTOR, strlen(CONNECTOR_TYPE_DISKCONNECTOR)) == 0 ||
        strncmp(connect_type, CONNECTOR_TYPE_NVMEVPDCONNECTOR, strlen(CONNECTOR_TYPE_NVMEVPDCONNECTOR)) == 0) {
        *pcie_type = DISK_CONNECTOR;
    } else if (strncmp(connect_type, CONNECTOR_TYPE_OCPCARDCONNECTOR, strlen(CONNECTOR_TYPE_OCPCARDCONNECTOR)) == 0) {
        *pcie_type = OCPCARD_CONNECTOR;
    } else {
        *pcie_type = PCIE_AND_PCIEPLUS_CONNECTOR;
    }

    return RET_OK;
}

void init_elabel_info(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;

    OBJ_HANDLE ref_component_handle;
    guint8 fru_id = 0;
    OBJ_HANDLE fru_handle;

    // 初始化PCIE卡的电子标签信息,由于电子标签初始化需要os上电
    ret = dfl_get_referenced_object(obj_handle, PROPERTY_PCIE_CARD_REF_COMPONENT, &ref_component_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get pcie card component handle failed, ret=%d", __FUNCTION__, ret);
        return;
    }
    (void)dal_get_property_value_byte(ref_component_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &fru_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get fru handle failed, pcie card name is %s", __FUNCTION__,
            dfl_get_object_name(obj_handle));
    } else {
        ret = dfl_call_class_method(fru_handle, METHOD_FRU_INIT_FRU_INFO_FROM_MCU, NULL, NULL, NULL);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: call InitFruInfoFromMcu failed, Fru name is %s", __FUNCTION__,
                dfl_get_object_name(fru_handle));
        }
    }

    return;
}


gint32 extend_pcie_card_add_object_callback(OBJ_HANDLE obj_handle)
{
    info_pcie_card_change();
    return RET_OK;
}


gint32 extend_pcie_card_del_object_callback(OBJ_HANDLE object_handle)
{
    return RET_OK;
}

LOCAL gint32 need_update_vendor_info(guint16 vendor_id, guint16 device_id, guint16 sub_vender_id, guint16 sub_device_id)
{
    if (((PCIE_3004RAID_VID == vendor_id) && (PCIE_3004RAID_DID == device_id) &&
        (PCIE_3004RAID_SUBVID == sub_vender_id || sub_vender_id == PCIE_HUAWEI_SUBVID) &&
        (PCIE_3004RAID_SUBDID == sub_device_id)) ||
        ((PCIE_3152RAID_VID == vendor_id) && (PCIE_3152RAID_DID == device_id) &&
        (PCIE_3152RAID_SUBVID == sub_vender_id) && (PCIE_3152RAID_SUBDID == sub_device_id))) {
        return RET_ERR;
    }
    return RET_OK;
}


LOCAL gint32 update_pcie_card_vid_did(OBJ_HANDLE obj_handle, guint8 slot_id)
{
    gchar referencd_object[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};
    GVariant *property_value = NULL;
    gint32 ret = 0;

    
    if (dfl_get_referenced_property(obj_handle, PROPERTY_PCIECARD_DID, referencd_object,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
        property_value = g_variant_new_uint16(g_pcie_group.info[slot_id - 1].device_id);
        ret = dfl_set_property_value(obj_handle, PROPERTY_PCIECARD_DID, property_value, DF_NONE);
        g_variant_unref(property_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: Update pcie card device id failed,ret=%d", __FUNCTION__, ret);
            return ret;
        }
    }

    if (dfl_get_referenced_property(obj_handle, PROPERTY_PCIECARD_VID, referencd_object,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
        property_value = g_variant_new_uint16(g_pcie_group.info[slot_id - 1].vender_id);
        ret = dfl_set_property_value(obj_handle, PROPERTY_PCIECARD_VID, property_value, DF_NONE);
        g_variant_unref(property_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: Update pcie card vender id failed,ret=%d", __FUNCTION__, ret);
            return ret;
        }
    }
    
    
    if (dfl_get_referenced_property(obj_handle, PROPERTY_PCIECARD_SUBDID, referencd_object,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
        property_value = g_variant_new_uint16(g_pcie_group_sub_info.sub_info[slot_id - 1].sub_device_id);
        ret = dfl_set_property_value(obj_handle, PROPERTY_PCIECARD_SUBDID, property_value, DF_NONE);
        g_variant_unref(property_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: Update pcie card sub device id failed,ret=%d", __FUNCTION__, ret);
            return ret;
        }
    }

    if (dfl_get_referenced_property(obj_handle, PROPERTY_PCIECARD_SUBVID, referencd_object,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
        property_value = g_variant_new_uint16(g_pcie_group_sub_info.sub_info[slot_id - 1].sub_vender_id);
        ret = dfl_set_property_value(obj_handle, PROPERTY_PCIECARD_SUBVID, property_value, DF_NONE);
        g_variant_unref(property_value);
        property_value = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s:Update pcie card sub vender id failed,ret=%d", __FUNCTION__, ret);
            return ret;
        }
    }
    
    return RET_OK;
}


gint32 pcie_card_add_object_callback(OBJ_HANDLE obj_handle)
{
    gint32 ret = 0;
    guint8 slot_id = 0;
    GVariant *property_value = NULL;
    gchar card_desc[MAX_PCIE_CARD_DESC_LEN] = {0};
    gchar manufacturer[256] = {0};
    guint16 vendor_id = 0;
    guint16 device_id = 0;
    guint16 sub_vender_id = 0;
    guint16 sub_device_id = 0;
    guint8 pciids_support = 1;
    gchar referencd_object[MAX_NAME_SIZE] = {0};
    gchar referencd_property[MAX_NAME_SIZE] = {0};

    ret = dfl_get_property_value(obj_handle, PROPETRY_PCIECARD_SLOT, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get pcie card slot id failed!");
        return RET_ERR;
    }

    slot_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);
    property_value = NULL;

    if (slot_id <= 0 || MAX_PCIE_CARD < slot_id) {
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "Add pcie card%d object", slot_id);

    
    ret = update_pcie_card_vid_did(obj_handle, slot_id);
    if (ret != RET_OK) {
        return ret;
    }

    update_pcie_lane_info(obj_handle, slot_id);

    
    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_BUS_NUM,
        g_pcie_cards_bdf_info.bdf_info[slot_id - 1].bus_num, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card bus num failed!");
        return ret;
    }

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_DEV_NUM,
        g_pcie_cards_bdf_info.bdf_info[slot_id - 1].device_num, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card device num failed!");
        return ret;
    }
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUN_NUM,
        g_pcie_cards_bdf_info.bdf_info[slot_id - 1].function_num, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card function num failed!");
        return ret;
    }
    

    
    ret = dfl_get_saved_value_v2(obj_handle, PROPERTY_FDM_FAULT_STATE, &property_value, DF_SAVE_TEMPORARY);
    if (ret == DFL_OK) {
        (void)dfl_set_property_value(obj_handle, PROPERTY_FDM_FAULT_STATE, property_value, DF_SAVE_TEMPORARY);
        g_variant_unref(property_value);
        property_value = NULL;
    }
    

    
    ret = dfl_get_saved_value_v2(obj_handle, PROPERTY_PFA_EVENT, &property_value, DF_SAVE_TEMPORARY);
    if (ret == DFL_OK) {
        (void)dfl_set_property_value(obj_handle, PROPERTY_PFA_EVENT, property_value, DF_SAVE_TEMPORARY);
        g_variant_unref(property_value);
        property_value = NULL;
    }
    
    ret = dfl_get_saved_value_v2(obj_handle, PROPERTY_PCIECARD_DECREASE_BANDWIDTH, &property_value, DF_SAVE_TEMPORARY);
    if (ret == DFL_OK) {
        (void)dfl_set_property_value(obj_handle, PROPERTY_PCIECARD_DECREASE_BANDWIDTH, property_value,
            DF_SAVE_TEMPORARY);
        g_variant_unref(property_value);
        property_value = NULL;
    }

    

    
    property_value = g_variant_new_byte(g_pcie_status_group.status[slot_id - 1]);
    ret = dfl_set_property_value(obj_handle, PROPERTY_PCIECARD_FATALERROR, property_value, DF_NONE);
    g_variant_unref(property_value);
    property_value = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Update pcie card device id failed!");
        return ret;
    }
    

    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_FALT_BY_BIOS,
        g_pcie_status_by_bios_group.status[slot_id - 1], DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Update pcie card status by bios failed!");
        return ret;
    }

    
    pcie_card_update_version_info(obj_handle);
    

    
    if (dfl_get_referenced_property(obj_handle, PROPERTY_PCIECARD_CPU_ID, referencd_object,
        referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
        pcie_card_update_resId_info(obj_handle, slot_id);
    }
    

    
    pcie_card_update_container_info(obj_handle);
    

    
    
    
    ret = update_smm_pcie_info(obj_handle, &vendor_id, &device_id, &sub_vender_id, &sub_device_id);
    
    vendor_id = (ret == RET_OK) ? vendor_id : g_pcie_group.info[slot_id - 1].vender_id;
    device_id = (ret == RET_OK) ? device_id : g_pcie_group.info[slot_id - 1].device_id;

    
    
    sub_vender_id = (ret == RET_OK) ? sub_vender_id : g_pcie_group_sub_info.sub_info[slot_id - 1].sub_vender_id;
    sub_device_id = (ret == RET_OK) ? sub_device_id : g_pcie_group_sub_info.sub_info[slot_id - 1].sub_device_id;
    

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_SUPPORT_PCIIDS, &pciids_support);
    if ((pciids_support == 0) ||
        need_update_vendor_info(vendor_id, device_id, sub_vender_id, sub_device_id) != RET_OK) {
        
        debug_log(DLOG_DEBUG,
            "This Pcie Card vender_id [0x%04x], device_id [0x%04x], sub_vender_id [0x%04x], sub_device_id [0x%04x]",
            vendor_id, device_id, sub_vender_id, sub_device_id);
    } else {
        ret = pcie_card_lookup_name(vendor_id, device_id, sub_vender_id, sub_device_id, manufacturer,
            sizeof(manufacturer), card_desc, sizeof(card_desc));
        
        if (ret == RET_OK) {
            
            if (PCIE_HUAWEI_SUBVID == sub_vender_id) {
                
                if (0 == ((PCIE_INTEL_VID == vendor_id) && (PCIE_INTEL_DH895XCC_DID == device_id))) {
                    (void)strncpy_s(manufacturer, sizeof(manufacturer), PCIE_HUAWEI_MANU, sizeof(PCIE_HUAWEI_MANU));
                }
            }

            if (dfl_get_referenced_property(obj_handle, PROPETRY_PCIECARD_MANUFACTURER, referencd_object,
                referencd_property, MAX_NAME_SIZE, MAX_NAME_SIZE) != DFL_OK) {
                (void)dal_set_property_value_string(obj_handle, PROPETRY_PCIECARD_MANUFACTURER, manufacturer, DF_NONE);
            }

            (void)dal_set_property_value_string(obj_handle, PROPERTY_PCIECARD_DESC, card_desc, DF_NONE);
        }

        
    }

    
    
    

    
    (void)pcie_card_change_notify_mctp();
    

    

    
    info_pcie_card_change();
    

    
    return RET_OK;
}


gint32 pcie_card_delete_complete_func(guint32 position)
{
    _delete_item_from_deleting_pos_list(position);

    return RET_OK;
}

gint32 pcie_card_del_object_callback(OBJ_HANDLE object_handle)
{
    gint32 ret = 0;
    guint8 slot_id = 0;
    GVariant *property_value = NULL;
    guint8 functionclass = 0;

    ret = dfl_get_property_value(object_handle, PROPETRY_PCIECARD_SLOT, &property_value);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "Get pcie card slot id failed!");
        return RET_ERR;
    }

    
    (void)dal_get_property_value_byte(object_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);

    if (functionclass == PCIECARD_FUNCTION_NPU) {
        card_manage_log_operation_log("NPU Card", object_handle, PROPETRY_PCIECARD_SLOT, NULL, CARD_PLUG_OUT);
    } else if (functionclass == PCIECARD_FUNCTION_FPGA) {
        card_manage_log_operation_log("FPGA Card", object_handle, PROPETRY_PCIECARD_SLOT, NULL, CARD_PLUG_OUT);
    }

    slot_id = g_variant_get_byte(property_value);
    g_variant_unref(property_value);

    if (slot_id <= 0 || slot_id > MAX_PCIE_CARD) {
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "Delete pcie card%d object", slot_id);
    g_pcie_group.info[slot_id - 1].device_id = PCIE_INVALID_DID;
    g_pcie_group.info[slot_id - 1].vender_id = PCIE_INVALID_VID;
    
    g_pcie_group_sub_info.sub_info[slot_id - 1].sub_device_id = PCIE_INVALID_SUBDID;
    g_pcie_group_sub_info.sub_info[slot_id - 1].sub_vender_id = PCIE_INVALID_SUBVID;
    
    
    g_pcie_status_group.status[slot_id - 1] = PCIE_NO_FATALERROR;
    g_pcie_status_by_bios_group.status[slot_id - 1] = 0x00;

    per_save((guint8 *)&g_pcie_group);
    
    per_save((guint8 *)&g_pcie_group_sub_info);
    
    per_save((guint8 *)&g_pcie_status_group);
    per_save((guint8 *)&g_pcie_status_by_bios_group);
    

    
    // PCIE卡被拔掉后,清除对应的复位持久化数据
    (void)pcie_card_recover_device_status(object_handle, NULL);
    

    
    (void)pcie_card_change_notify_mctp();
    

    return RET_OK;
}

LOCAL gint32 pcie_card_fprint_head(FILE *fp, const gchar *title)
{
    gint32 ret = fprintf(fp, "%s\n", title);
    if (ret < 0) {
        return ret;
    }

    ret = fprintf(fp,
        "%-4s | %-10s | %-10s | %-15s | %-15s | %-12s | %-14s | %-16s | %-40s | %-10s | %-14s | %-14s | %-16s |"
        " %-8s | %-40s | %-40s | %-40s | %-40s\n", "Slot", "Vender Id", "Device Id", "Sub Vender Id", "Sub Device Id",
        "Bus Number", "Device Number", "Function Number", "Card Desc", "Board Id", "PCB Version", "CPLD Version",
        "Sub Card Bom Id", "PartNum", "Manufacturer", "Position", "AssociatedResource", "ProductName");
    if (ret < 0) {
        return ret;
    }
    return RET_OK;
}

LOCAL void pcie_card_get_associated_resource(OBJ_HANDLE pcie_card_obj, gchar *associated_resource, gint32 length)
{
    guchar x86_enabled = DISABLE;
    OBJ_HANDLE obj_handle = 0;
    
    (void)strcpy_s(associated_resource, length, "N/A");
    (void)dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s: get object PMEServiceConfig faild.", __FUNCTION__);
        return;
    }
    (void)dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_X86, &x86_enabled);
    // 只有x86架构的机型才能获取到Associated Resource
    if (x86_enabled != ENABLE) {
        return;
    }
    guint8 Resource_Id = 0;
    gint32 ret = dal_get_property_value_byte(pcie_card_obj, PROPERTY_PCIECARD_CPU_ID, &Resource_Id);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property Resource ID failed!", __FUNCTION__);
        return;
    }
    if (Resource_Id == RESOURCE_ID_PCIE_SWTICH) {
        (void)strncpy_s(associated_resource, length, "PCIeSwitch", strlen("PCIeSwitch"));
    } else if (Resource_Id == RESOURCE_ID_PCH) {
        (void)strncpy_s(associated_resource, length, "PCH", strlen("PCH"));
    } else if (Resource_Id == RESOURCE_ID_CPU1_AND_CPU2) {
        (void)strncpy_s(associated_resource, length, "CPU1,CPU2", strlen("CPU1,CPU2"));
    } else if (Resource_Id == RESOURCE_ID_CPU1_TO_CPU4) {
        (void)strncpy_s(associated_resource, length, "CPU1,CPU2,CPU3,CPU4", strlen("CPU1,CPU2,CPU3,CPU4"));
    } else if (Resource_Id >= RESOURCE_ID_PCIE_DEVICE_BASE && Resource_Id <= RESOURCE_ID_PCIE_DEVICE_MAX) {
        GSList *component_obj_list = NULL;
        ret = dfl_get_object_list(CLASS_COMPONENT, &component_obj_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: dfl_get_object_list failed! ret = %d.", __FUNCTION__, ret);
            return;
        }

        guint8 fruid;
        for (GSList *component_obj_note = component_obj_list; component_obj_note != NULL;
            component_obj_note = component_obj_note->next) {
            (void)dal_get_property_value_byte((OBJ_HANDLE)component_obj_note->data, PROPERTY_COMPONENT_FRUID, &fruid);
            if (fruid == Resource_Id - RESOURCE_ID_PCIE_DEVICE_BASE) {
                (void)dal_get_property_value_string((OBJ_HANDLE)component_obj_note->data,
                    PROPERTY_COMPONENT_DEVICE_NAME, associated_resource, length);
                break;
            }
        }
        g_slist_free(component_obj_list);
    } else if (Resource_Id != 0) {
        (void)snprintf_s(associated_resource, length, length - 1, "CPU%u", Resource_Id);
    }
    return;
}


gint32 pcie_card_dump_info(const gchar *class_name, const gchar *title, const gchar *path)
{
    gchar file_name[PCIE_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    FILE *fp = NULL;
    const gchar *partnum = NULL;
    const gchar *card_desc = NULL;
    const gchar *manufacturer = NULL;
    const gchar *product_name = NULL;
    GSList *obj_list = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    OBJ_HANDLE obj_handle = 0;
    PCIE_CARD_EXTRA_INFO extra_info;
    gchar extra_info_board_id_buf[PCIE_CARD_DUMPINFO_MAX_LEN + 1] = {0};
    gchar extra_info_sub_card_id_buf[PCIE_CARD_DUMPINFO_MAX_LEN + 1] = {0};

    if (path == NULL || class_name == NULL || title == NULL) {
        return RET_ERR;
    }

    gint32 ret = dfl_get_object_list(class_name, &obj_list);
    if (ret != DFL_OK) {
        if (ret == ERRCODE_OBJECT_NOT_EXIST) {
            return RET_OK;
        }
        return RET_ERR;
    }

    // 在dump_dir目录下创建文件
    ret = snprintf_s(file_name, PCIE_CARD_DUMPINFO_MAX_LEN + 1, PCIE_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        g_slist_free(obj_list);
        return RET_ERR;
    }

    
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    

    ret = pcie_card_fprint_head(fp, title);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: fprintf fail, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    for (GSList *obj_note = obj_list; obj_note != NULL; obj_note = obj_note->next) {
        property_name_list = g_slist_append(property_name_list, PROPETRY_PCIECARD_SLOT);
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_VID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_DID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_DESC);
        
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_SUBVID);
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_SUBDID);
        
        
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_BUS_NUM);
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_DEV_NUM);
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_FUN_NUM);
        // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
        property_name_list = g_slist_append(property_name_list, PROPERTY_PCIECARD_PART_NUMBER);
        
        property_name_list = g_slist_append(property_name_list, PROPETRY_PCIECARD_MANUFACTURER);
        property_name_list = g_slist_append(property_name_list, NAME);

        obj_handle = (OBJ_HANDLE)obj_note->data;
        ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get pcie card information failed!");
            g_slist_free(property_name_list);
            goto EXIT;
        }

        guint8 slot_id = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        guint16 vender_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 1));
        guint16 device_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 2));
        card_desc = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
        
        guint16 sub_vender_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 4));
        guint16 sub_device_id = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 5));
        
        
        guint8 bus_num = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 6));
        guint8 device_num = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 7));
        guint8 function_num = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 8));
        

        // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
        partnum = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 9), 0);
        manufacturer = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 10), 0);
        product_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 11), 0);

        
        (void)memset_s(&extra_info, sizeof(PCIE_CARD_EXTRA_INFO), 0, sizeof(PCIE_CARD_EXTRA_INFO));
        extra_info.board_id = 0xffff;
        (void)strcpy_s(extra_info.pcb_ver, sizeof(extra_info.pcb_ver), "N/A");
        (void)strcpy_s(extra_info.cpld_ver, sizeof(extra_info.cpld_ver), "N/A");
        extra_info.sub_card_bom_id = 0xff;
        (void)get_pcie_card_extra_info(obj_handle, &extra_info);

        // 将附加信息输出到字符串缓冲区，无效数据转换成N/A字符串
        if (extra_info.board_id != 0xffff && extra_info.board_id != 0) {
            snprintf_s(extra_info_board_id_buf, sizeof(extra_info_board_id_buf), sizeof(extra_info_board_id_buf) - 1,
                "0x%04x", extra_info.board_id);
        } else {
            strcpy_s(extra_info_board_id_buf, sizeof(extra_info_board_id_buf), "N/A");
        }

        if (extra_info.sub_card_bom_id != 0xff) {
            (void)snprintf_s(extra_info_sub_card_id_buf, sizeof(extra_info_sub_card_id_buf),
                sizeof(extra_info_sub_card_id_buf) - 1, "0x%02x", extra_info.sub_card_bom_id);
        } else {
            strcpy_s(extra_info_sub_card_id_buf, sizeof(extra_info_sub_card_id_buf), "N/A");
        }

        OBJ_HANDLE ref_comp_handle = 0;
        gchar position [GET_MODEL_NUMBER_LEN + 1] = {0};
        ret = dfl_get_referenced_object(obj_handle, PROPERTY_COMPOSITION_ENDPOINT_REF_COMP, &ref_comp_handle);
        if (ret == DFL_OK) {
            ret = dal_get_property_value_string(ref_comp_handle, PROPERTY_COMPONENT_LOCATION, position,
                GET_MODEL_NUMBER_LEN);
        }
        if (ret != DFL_OK) {
            (void)strcpy_s(position, sizeof(position), "N/A");
        }

        // 获取PCIE卡的Associated Resource值
        gchar associated_resource[GET_MODEL_NUMBER_LEN] = {0};
        pcie_card_get_associated_resource(obj_handle, associated_resource, sizeof(associated_resource));

        // 201705108246 需求支持PARNUM查询, 20190723 RAOJIANZHONG 00267466
        ret = fprintf(fp,
            "%-4u | 0x%04x     | 0x%04x     | 0x%04x          | 0x%04x          | 0x%02x         | 0x%02x           "
            "| 0x%02x             | %-40s | %-10s | %-14s | %-14s | %-16s | %-8s | %-40s | %-40s | %-40s | %-40s\n",
            slot_id, vender_id, device_id, sub_vender_id, sub_device_id, bus_num, device_num, function_num, card_desc,
            extra_info_board_id_buf, extra_info.pcb_ver, extra_info.cpld_ver, extra_info_sub_card_id_buf, partnum,
            manufacturer, position, associated_resource, product_name);
        
        g_slist_free(property_name_list);
        g_slist_free_full(property_value, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
        property_value = NULL;
        if (ret < 0) {
            debug_log(DLOG_ERROR, "fprintf failed!");
            goto EXIT;
        }
    }

    ret = fprintf(fp, "\n\n");
    if (ret < 0) {
        debug_log(DLOG_ERROR, "fprintf failed!");
        goto EXIT;
    }

    g_slist_free(obj_list);
    // 关闭文件
    (void)fclose(fp);
    return MODULE_OK;

EXIT:
    (void)fclose(fp);
    g_slist_free(obj_list);
    return RET_ERR;
}

void set_pcie_card_infos_default_value(PCIE_CARD_GROUP_INFO_S *pciecard_group_info,
    PCIE_CARD_GROUP_SUB_INFO_S *pciecard_sub_group_info, guint8 slot_id)
{
    if ((slot_id > MAX_PCIE_CARD - 1) || (pciecard_group_info == NULL) || (pciecard_sub_group_info == NULL)) {
        return;
    }
    pciecard_group_info->info[slot_id].device_id = PCIE_INVALID_DID;
    pciecard_group_info->info[slot_id].vender_id = PCIE_INVALID_VID;
    pciecard_sub_group_info->sub_info[slot_id].sub_device_id = PCIE_INVALID_DID;
    pciecard_sub_group_info->sub_info[slot_id].sub_vender_id = PCIE_INVALID_VID;
}
LOCAL void set_pcie_card_bdf_info_default_value(PCIE_CARD_GROUP_BDF_INFO_S *pciecard_bdf_group_info, guint8 slot_id)
{
    if ((slot_id > MAX_PCIE_CARD - 1) || (pciecard_bdf_group_info == NULL)) {
        return;
    }
    pciecard_bdf_group_info->bdf_info[slot_id].bus_num = 0xff;
    pciecard_bdf_group_info->bdf_info[slot_id].device_num = 0xff;
    pciecard_bdf_group_info->bdf_info[slot_id].function_num = 0xff;
}


LOCAL void set_port_rootbdf_default_value(PORT_GROUP_BDF_INFO_S *pcie_rootbdf_group_info, guint8 slot_id)
{
    if ((slot_id > MAX_PCIE_CARD - 1) || (pcie_rootbdf_group_info == NULL)) {
        debug_log(DLOG_ERROR, "%s :check set port rootbdf default value para file!\n", __FUNCTION__);
        return;
    }
    pcie_rootbdf_group_info->bdf_info[slot_id].bus_num = INVALID_DATA_BYTE;
    pcie_rootbdf_group_info->bdf_info[slot_id].device_num = INVALID_DATA_BYTE;
    pcie_rootbdf_group_info->bdf_info[slot_id].function_num = INVALID_DATA_BYTE;
}


LOCAL void set_gpcie_default_values(guint8 slot_id)
{
    if (slot_id > MAX_PCIE_CARD - 1) {
        debug_log(DLOG_ERROR, "%s :check set gpcie default values para file!\n", __FUNCTION__);
        return;
    }
    set_pcie_card_infos_default_value(&g_pcie_group, &g_pcie_group_sub_info, slot_id);
    set_pcie_card_bdf_info_default_value(&g_pcie_cards_bdf_info, slot_id);
    set_pcie_card_infos_default_value(&g_pcie_disk_group, &g_pcie_disk_group_sub_info, slot_id);
    set_pcie_card_bdf_info_default_value(&g_pcie_disks_bdf_info, slot_id);
    set_pcie_card_infos_default_value(&g_ocp_group, &g_ocp_group_sub_info, slot_id);
    set_pcie_card_bdf_info_default_value(&g_ocp_bdf_info, slot_id);
    set_port_rootbdf_default_value(&g_pcie_port_rootbdf_info, slot_id);
    set_port_rootbdf_default_value(&g_ocp_port_rootbdf_info, slot_id);
    set_port_rootbdf_default_value(&g_pcie_raid_rootbdf_info, slot_id);
    return;
}


gint32 pcie_card_per_init(void)
{
    guint8 slot_id = 0;

    
    
    gint32 ret = vos_thread_counter_sem4_create(&g_set_pcie_card_info_lock, "set_pcie_card_info", 0UL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : create sem fail :!", __FUNCTION__);
        return RET_ERR;
    }

    ret = vos_thread_counter_sem4_create(&g_set_pcie_disk_info_lock, "set_pcie_disk_info", 0UL);
    if (RET_OK != ret) {
        debug_log(DLOG_ERROR, "%s : create sem fail :!", __FUNCTION__);
        return RET_ERR;
    }
    

    
    ret = vos_thread_mutex_sem4_create(&g_set_node_pcie_summary_lock, "pcie summary");
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : create pcie summary sem failed, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    

    
    (void)memset_s(&g_pcie_group, sizeof(g_pcie_group), 0, sizeof(g_pcie_group));
    (void)memset_s(&g_pcie_disk_group, sizeof(g_pcie_disk_group), 0, sizeof(g_pcie_disk_group));
    (void)memset_s(&g_pcie_group_sub_info, sizeof(g_pcie_group_sub_info), 0, sizeof(g_pcie_group_sub_info));
    
    (void)memset_s(&g_pcie_disk_group_sub_info, sizeof(g_pcie_disk_group_sub_info), 0,
        sizeof(g_pcie_disk_group_sub_info));
    
    (void)memset_s(&g_pcie_cards_bdf_info, sizeof(g_pcie_cards_bdf_info), 0, sizeof(g_pcie_cards_bdf_info));
    (void)memset_s(&g_pcie_disks_bdf_info, sizeof(g_pcie_disks_bdf_info), 0, sizeof(g_pcie_disks_bdf_info));

    
    (void)memset_s(&g_pcie_summary_info, sizeof(g_pcie_summary_info), 0, sizeof(g_pcie_summary_info));
    
    (void)memset_s(&g_ocp_group, sizeof(g_ocp_group), 0, sizeof(g_ocp_group));
    (void)memset_s(&g_ocp_group_sub_info, sizeof(g_ocp_group_sub_info), 0, sizeof(g_ocp_group_sub_info));
    (void)memset_s(&g_ocp_bdf_info, sizeof(g_ocp_bdf_info), 0, sizeof(g_ocp_bdf_info));
    (void)memset_s(&g_pcie_port_rootbdf_info, sizeof(g_pcie_port_rootbdf_info), 0, sizeof(g_pcie_port_rootbdf_info));
    (void)memset_s(&g_ocp_port_rootbdf_info, sizeof(g_ocp_port_rootbdf_info), 0, sizeof(g_ocp_port_rootbdf_info));
    (void)memset_s(&g_pcie_raid_rootbdf_info, sizeof(g_pcie_raid_rootbdf_info), 0, sizeof(g_pcie_raid_rootbdf_info));

    for (slot_id = 0; slot_id < MAX_PCIE_CARD; slot_id++) {
        set_gpcie_default_values(slot_id);
    }
    (void)memset_s(&g_pcie_status_group, sizeof(g_pcie_status_group), 0, sizeof(g_pcie_status_group));
    (void)memset_s(&g_pcie_status_by_bios_group, sizeof(g_pcie_status_by_bios_group), 0,
        sizeof(g_pcie_status_by_bios_group));

    ret = per_init(persistance_tbl, (guint32)(sizeof(persistance_tbl) / sizeof(persistance_tbl[0])));
    
    if ((gint32)ret != PER_OK) {
        
        debug_log(DLOG_ERROR, "Persistance init pcie cards info fail, Error code : %d!", ret);
        return RET_ERR;
    }

    debug_log(DLOG_INFO, "Persistance init pcie cards info success!");

    
    g_bdf_info_parsed_status = BDF_PARSED_STATE;
    
    return RET_OK;
}

gint32 pcie_card_get_connector_handle(guint8 slot_id, OBJ_HANDLE *obj_handle, guint8 connector_type)
{
    gint32 ret = 0;
    guint8 slot = 0;
    gchar connector_type_string[MAX_CONNECTOR_TYPE_STRING_LEN] = {0};
    const gchar *type_string = NULL;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    OBJ_HANDLE object_handle = 0;

    if (obj_handle == NULL) {
        return RET_ERR;
    }
    ret = dfl_get_object_list(CLASS_CONNECTOR_NAME, &obj_list);
    if (ret != DFL_OK || g_slist_length(obj_list) == 0) {
        return RET_ERR;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        object_handle = (OBJ_HANDLE)obj_note->data;
        ret = dal_get_property_value_byte(object_handle, PROPERTY_CONNECTOR_SLOT, &slot);
        if (ret != RET_OK) {
            continue;
        }

        // 如果槽位信息不一致就退出本轮循环
        if (slot != slot_id) {
            continue;
        }

        (void)memset_s(connector_type_string, sizeof(connector_type_string), 0, sizeof(connector_type_string));
        ret = dal_get_property_value_string(object_handle, PROPERTY_CONNECTOR_TYPE, &connector_type_string[0],
            MAX_CONNECTOR_TYPE_STRING_LEN);
        if (ret != RET_OK) {
            continue;
        }

        type_string = pcie_card_get_connector_type_string(connector_type);
        if (type_string == NULL) {
            continue;
        }

        // 如果类型匹配成功，则直接返回句柄
        if (!strcmp(type_string, connector_type_string)) {
            g_slist_free(obj_list);
            *obj_handle = object_handle;
            return RET_OK;
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}


GVariant *create_gvar_array(PCIE_CARD_INFO_S *info, guint32 element_num)
{
    guint32 i = 0;
    GVariant **element_gvar = NULL;
    GVariant *result = NULL;

    element_gvar = g_new0(GVariant *, (element_num * 2));
    if (element_gvar == NULL) {
        debug_log(DLOG_ERROR, "[%s %d]no memory malloced", __FUNCTION__, __LINE__);
        return NULL;
    }

    for (i = 0; i < element_num; i++) {
        element_gvar[2 * i] = g_variant_new_uint16(info[i].vender_id);

        if (element_gvar[2 * i] == NULL) {
            break;
        }

        element_gvar[2 * i + 1] = g_variant_new_uint16(info[i].device_id);

        if (element_gvar[2 * i + 1] == NULL) {
            break;
        }
    }

    
    if (i < element_num) {
        guint32 k = 0;

        for (k = 0; k < i; k++) {
            g_variant_unref(element_gvar[2 * i]);
            g_variant_unref(element_gvar[2 * i + 1]);
        }

        g_free(element_gvar);
        debug_log(DLOG_ERROR, "[%s %d]Memory is not enough", __FUNCTION__, __LINE__);
        return NULL;
    }

    debug_log(DLOG_INFO, "[%s %d] element_num is %d", __FUNCTION__, __LINE__, element_num);

    
    result = g_variant_new_array(G_VARIANT_TYPE_UINT16, element_gvar, (element_num * 2));
    g_free(element_gvar);
    return result;
}


gboolean filter_msg_by_sys_channel(const void *msg_data)
{
    if (msg_data == NULL) {
        return TRUE;
    }
    gint32 channel_number;
    channel_number = get_ipmi_chan_num(msg_data);
    if (channel_number == SYS_CHAN_NUM || channel_number == CPLDRAM_CHAN_NUM) {
        return FALSE;
    }
    return TRUE;
}


void response_error_process(const guint8 errcode, gint32 *channel_type, gulong *taskdelaytime, gint8 *retry_time)
{
    guint32 product_num = 0;

    (void)dal_get_x86_platform_version_num(&product_num);
    
    if (product_num <= PRODUCT_VERSION_V3) {
        if (errcode == 0xa2) {
            *retry_time = -1; 
        }
        return;
    }
    
    
    if ((CHANNEL_ME == *channel_type) && ((errcode == 0xa2) || (0xA8 == errcode) || (0 == *retry_time))) {
        *channel_type = CHANNEL_PECI;
        *retry_time = 7;      
        *taskdelaytime = 100; 
    }
    
    else if ((CHANNEL_PECI == *channel_type) && !(errcode & 0x80)) {
        *retry_time = -1; 
    }
}


gint32 match_v5_partition_product_peci_enable(void)
{
    gint32 ret = RET_OK;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE product_handle = 0;
    guint8 peci_enable = 0;

    ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &product_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "dfl_get_object_handle fail! ret = %d", ret);
        return FALSE;
    }

    (void)dal_get_property_value_byte(product_handle, PROPERTY_PECI_ENABLE, &peci_enable);
    if (peci_enable != 1) {
        return FALSE;
    }

    if (dal_get_object_handle_nth(CLASS_PARTITION, 0, &obj_handle) == RET_OK) {
        return TRUE;
    }
    return FALSE;
}


LOCAL gint32 _pcie_card_connnector_compare(gconstpointer data1, gconstpointer data2)
{
    gint32 ret;
    const NODE_PCIE_INFO_S *info = NULL;
    OBJ_HANDLE component_handle;
    guchar component_type;
    guchar component_slot;

    if (data1 == NULL || data2 == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    component_handle = *(const OBJ_HANDLE *)data1;
    info = (const NODE_PCIE_INFO_S *)data2;

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &component_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get component type failed, ret is %d", ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, &component_slot);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get component slot failed, ret is %d", ret);
        return RET_ERR;
    }

    if ((component_type == info->container_type) && (component_slot == info->container_slot)) {
        return RET_OK;
    }

    return RET_ERR;
}


gint32 _find_node_pcie_card_connector(OBJ_HANDLE node_pcie_summary_handle, NODE_PCIE_INFO_S *info,
    OBJ_HANDLE *obj_handle)
{
    gint32 ret = RET_OK;
    GVariant *pcie_container_entry_var = NULL;
    GVariantIter iter;
    GVariant *entry_var = NULL;
    const gchar *entry_handle_name = NULL;
    OBJ_HANDLE entry_handle = 0;
    OBJ_HANDLE anchor_handle = 0;
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE connector_handle = 0;
    gchar               class_name[MAX_NAME_SIZE] = {0};
    GSList *component_handle_list = NULL;
    GSList *connector_handle_list = NULL;
    GSList *obj_list_iter = NULL;
    gchar               connector_type_str[MAX_CONNECTOR_TYPE_LEN] = {0};
    
    guchar connector_slot = 0;
    

    
    ret = dfl_get_property_value(node_pcie_summary_handle, PROEPRTY_NODE_PCIEINFO_SUMMARY_CONNECTORS,
        &pcie_container_entry_var);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "get pcie container entries failed");
        return RET_ERR;
    }

    g_variant_iter_init(&iter, pcie_container_entry_var);

    while ((entry_var = g_variant_iter_next_value(&iter)) != NULL) {
        entry_handle_name = g_variant_get_string(entry_var, NULL);

        ret = dfl_get_object_handle(entry_handle_name, &entry_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "cann't get object handle with name (%s)", entry_handle_name);
            goto exit;
        }

        
        ret = dfl_get_class_name(entry_handle, class_name, MAX_NAME_SIZE);
        if (ret != DFL_OK || g_strcmp0(class_name, CLASS_CONNECTOR_NAME) != 0) {
            debug_log(DLOG_ERROR, "error use object (%s) config for pcie container connectors", entry_handle_name);
            goto exit;
        }
        

        ret = dal_get_connector_related_anchor(entry_handle, &anchor_handle);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get anchor for connector (%s) failed", entry_handle_name);
            g_variant_unref(entry_var);
            continue;
        }

        
        component_handle_list = NULL;
        ret = dal_get_container_specific_handle_list(anchor_handle, CLASS_COMPONENT, _pcie_card_connnector_compare,
            info, &component_handle_list);
        debug_log(DLOG_INFO, "get object handle list with handle (%s), object list count is %d",
            dfl_get_object_name(anchor_handle), g_slist_length(component_handle_list));
        if (ret != RET_OK || g_slist_length(component_handle_list) != 1) {
            g_variant_unref(entry_var);
            g_slist_free(component_handle_list);
            component_handle_list = NULL;
            continue;
        }

        component_handle = (OBJ_HANDLE)component_handle_list->data;
        (void)dal_get_object_list_position(component_handle, CLASS_CONNECTOR_NAME, &connector_handle_list);
        debug_log(DLOG_INFO, "item count for %s on (%s) is %d", CLASS_CONNECTOR_NAME,
            dfl_get_object_name(component_handle), g_slist_length(connector_handle_list));

        for (obj_list_iter = connector_handle_list; obj_list_iter != NULL; obj_list_iter = obj_list_iter->next) {
            connector_handle = (OBJ_HANDLE)obj_list_iter->data;
            (void)dal_get_property_value_string(connector_handle, PROPERTY_CONNECTOR_TYPE, connector_type_str,
                MAX_CONNECTOR_TYPE_LEN);
            (void)dal_get_property_value_byte(connector_handle, PROPERTY_CONNECTOR_SLOT, &connector_slot);

            if (g_strcmp0(CONNECTOR_TYPE_PCIE, connector_type_str) == 0 && (connector_slot == info->pcie_slot)) {
                debug_log(DLOG_INFO, "get connector handle successfully, current slot:%d", connector_slot);
                *obj_handle = connector_handle;
                ret = RET_OK;

                goto exit;
            }
        }

        g_variant_unref(entry_var);

        g_slist_free(component_handle_list);
        component_handle_list = NULL;

        g_slist_free(connector_handle_list);
        connector_handle_list = NULL;
    }

    ret = RET_ERR;

exit:

    g_slist_free(component_handle_list);
    g_slist_free(connector_handle_list);
    if (entry_var != NULL) {
        g_variant_unref(entry_var);
    }
    if (pcie_container_entry_var != NULL) {
        g_variant_unref(pcie_container_entry_var);
    }

    return ret;
}


gint32 pcie_card_get_cardaction_handle(guint8 slot, guint8 type, OBJ_HANDLE *obj_handle)
{
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gint32 result;
    guint8 des_data = 0;
    guint8 des_type = 0;
    guint8 i = 0;
    guint8 retry_time = 5;
    result = dfl_get_object_list(CLASS_CARD_ACTION, &obj_list);
    if (result != DFL_OK) {
        debug_log(DLOG_ERROR, "get objlist for CardAction fail!!!");
        return result;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        
        for (i = 0; i < retry_time; i++) {
            result = dal_get_property_value_byte((OBJ_HANDLE)(obj_note->data), PROPERTY_CARD_ACTION_DATA, &des_data);
            if (result != RET_OK) {
                debug_log(DLOG_ERROR, "Get the %s cardaction's Data fail times %d result= %d!",
                    dfl_get_object_name((OBJ_HANDLE)(obj_note->data)), i, result);
                vos_task_delay(100);
                continue;
            } else {
                break;
            }
            
        }
        if (des_data == slot) {
            result = pcie_get_cardtype_by_cardaction((OBJ_HANDLE)(obj_note->data), &des_type);
            if (result != RET_OK) {
                debug_log(DLOG_ERROR, "get cardtype for %s fail!!!", dfl_get_object_name((OBJ_HANDLE)(obj_note->data)));
                g_slist_free(obj_list);
                return result;
            }

            if (des_type == type) {
                *obj_handle = (OBJ_HANDLE)(obj_note->data);
                g_slist_free(obj_list);
                return RET_OK;
            }
        }
    }

    g_slist_free(obj_list);
    return RET_ERR;
}


void _deleting_pos_list_remove_all(void)
{
    (void)pthread_mutex_lock(&g_pos_list_mutex);

    g_slist_free(g_deleting_pos_list);
    g_deleting_pos_list = NULL;

    (void)pthread_mutex_unlock(&g_pos_list_mutex);

    return;
}


void _add_item_to_deleting_pos_list(guint32 pos)
{
    (void)pthread_mutex_lock(&g_pos_list_mutex);

    g_deleting_pos_list = g_slist_append(g_deleting_pos_list, GUINT_TO_POINTER(pos));

    (void)pthread_mutex_unlock(&g_pos_list_mutex);

    return;
}


LOCAL void _delete_item_from_deleting_pos_list(guint32 pos)
{
    (void)pthread_mutex_lock(&g_pos_list_mutex);

    g_deleting_pos_list = g_slist_remove(g_deleting_pos_list, GUINT_TO_POINTER(pos));

    debug_log(DLOG_ERROR, "delete item from deleting list, position is %#x, current length is %d", pos,
        g_slist_length(g_deleting_pos_list));

    (void)pthread_mutex_unlock(&g_pos_list_mutex);

    return;
}


guint32 _get_deleting_pos_list_length(void)
{
    guint32 len;

    (void)pthread_mutex_lock(&g_pos_list_mutex);

    len = g_slist_length(g_deleting_pos_list);

    (void)pthread_mutex_unlock(&g_pos_list_mutex);

    return len;
}


LOCAL gint32 type2classname(guint8 type, gchar **class_name)
{
    if (class_name == NULL) {
        return RET_ERR;
    }
    if (type == PCIE_AND_PCIEPLUS_CONNECTOR) {
        *class_name = CLASS_PCIE_CARD;
    } else if (type == OCPCARD_CONNECTOR) {
        *class_name = CLASS_OCP_CARD;
    } else {
        return RET_ERR;
    }
    return RET_OK;
}

gint32 pcie_card_get_object_handle_by_idinfo(guint32 boardid, guint32 auxid, guint8 slot_id, guint8 type,
    OBJ_HANDLE *object_handle)
{
    gint32 ret = RET_ERR;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    guint16 device_id, vender_id, sub_device_id, sub_vender_id;
    guint32 pcie_boardid, pcie_auxid;
    guint8 pcie_slot_id = 0;
    gchar *class_name = NULL;
    OBJ_HANDLE obj_handle = 0;

    if (type2classname(type, &class_name) != RET_OK) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(class_name, &handle_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get PcieCard object list failed!");
        return ret;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        obj_handle = (OBJ_HANDLE)list_item->data;
        (void)dal_get_property_value_byte(obj_handle, PROPETRY_PCIECARD_SLOT, &pcie_slot_id);

        if (pcie_slot_id != slot_id) {
            continue;
        } else if (dal_check_board_special_me_type() == RET_OK) {
            *object_handle = obj_handle;
            g_slist_free(handle_list);
            return RET_OK;
        }

        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIECARD_DID, &device_id);
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIECARD_VID, &vender_id);
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIECARD_SUBDID, &sub_device_id);
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIECARD_SUBVID, &sub_vender_id);
        pcie_boardid = vender_id * 0x10000 + device_id;
        pcie_auxid = sub_vender_id * 0x10000 + sub_device_id;

        if ((pcie_boardid == boardid) && (pcie_auxid == auxid)) {
            *object_handle = obj_handle;
            g_slist_free(handle_list);
            return RET_OK;
        }
    }

    g_slist_free(handle_list);
    return RET_ERR;
}


void release_thread_sem(guint8 pcie_type)
{
    if (pcie_type == DISK_CONNECTOR) {
        if (g_set_pcie_disk_info_lock != 0) {
            (void)vos_thread_sem4_v(g_set_pcie_disk_info_lock);
        }
    } else if (pcie_type == PCIE_AND_PCIEPLUS_CONNECTOR) {
        if (g_set_pcie_card_info_lock != 0) {
            (void)vos_thread_sem4_v(g_set_pcie_card_info_lock);
        }
    }

    return;
}

gint32 pcie_card_get_type_by_property_name(gchar *property_name, guint8 *pcie_type)
{
    if ((property_name == NULL) || (pcie_type == NULL)) {
        return RET_ERR;
    }

    if (strcmp(property_name, PROPERTY_BIOS_PCIEDISK_BDF) == 0) {
        *pcie_type = DISK_CONNECTOR;
    } else if (strcmp(property_name, PROPERTY_BIOS_PCIECARD_BDF) == 0) {
        *pcie_type = PCIE_AND_PCIEPLUS_CONNECTOR;
    } else if (strcmp(property_name, PROPERTY_BIOS_OCPCARD_BDF) == 0) {
        *pcie_type = OCPCARD_CONNECTOR;
    } else {
        return RET_ERR;
    }

    return RET_OK;
}


gint32 get_pcieaddr_handle_by_type_slot(guint8 Component_type, guint8 slot, OBJ_HANDLE *pcie_addr_handle)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 pcie_addr_type = 0;
    guint8 pcie_addr_slot = 0;
    OBJ_HANDLE obj_handle;

    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get %s object list handle failed, ret = %d", CLASS_PCIE_ADDR_INFO, ret);
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        obj_handle = (OBJ_HANDLE)obj_node->data;
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPONENT_TYPE, &pcie_addr_type);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get %s property component type failed, ret = %d", dfl_get_object_name(obj_handle),
                ret);
            continue;
        }
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_SLOT_ID, &pcie_addr_slot);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Get %s property slot id failed, ret = %d", dfl_get_object_name(obj_handle), ret);
            continue;
        }

        if (pcie_addr_type == Component_type && pcie_addr_slot == slot) {
            *pcie_addr_handle = obj_handle;
            g_slist_free(obj_list);
            return RET_OK;
        }
    }
    g_slist_free(obj_list);
    return RET_ERR;
}

gint32 get_bdf_property_value(OBJ_HANDLE obj_handle, INFO_NEED_GET_PCIE_INFO *info)
{
    gint32 ret;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SOCKET_ID, &(info->cpu_num));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s property socket id failed, ret = %d", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_BUS, &(info->bus));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s property bus failed, ret = %d", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_DEVICE, &(info->dev));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s property device failed, ret = %d", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_FUNCTION, &(info->func));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get %s property function failed, ret = %d", dfl_get_object_name(obj_handle), ret);
        return RET_ERR;
    }

    return RET_OK;
}



gint32 card_action_del_object_callback(OBJ_HANDLE object_handle)
{
    
    gint32 ret = 0;
    guint8 card_type = 0;
    guint8 ssd_slot = 0;
    guint8 ssd_index = 0;

    (void)pangea_card_action_del_object_callback(object_handle);
    ret = pcie_get_cardtype_by_cardaction(object_handle, &card_type);
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get cardtype for %s fail!!!", dfl_get_object_name(object_handle));
        ret = dal_get_property_value_byte(object_handle, PROPERTY_CARD_ACTION_TYPE, &card_type);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "get %s.Type fail!!!", dfl_get_object_name(object_handle));
            return ret;
        }
    }

    
    if (card_type == DISK_CONNECTOR) {
        ret = dal_get_property_value_byte(object_handle, PROPERTY_CARD_ACTION_DATA, &ssd_slot);
        if (ret != RET_OK || ssd_slot >= MAX_PCIE_CARD) {
            debug_log(DLOG_ERROR, "ssd_slot(=%d) is out of buff or get %s.Data fail!", ssd_slot,
                dfl_get_object_name(object_handle));
            return ret;
        }

        ssd_index = ssd_slot;

        debug_log(DLOG_INFO, "Delete SSDCard %u 0x%04x%04xobject", ssd_index,
            g_pcie_disk_group.info[ssd_index].vender_id, g_pcie_disk_group.info[ssd_index].device_id);

        g_pcie_disk_group.info[ssd_index].vender_id = PCIE_INVALID_VID;
        g_pcie_disk_group.info[ssd_index].device_id = PCIE_INVALID_DID;

        per_save((guint8 *)&g_pcie_disk_group);
    }

    
    return RET_OK;
}



void get_slotid_by_handle(OBJ_HANDLE obj_handle, guchar *slot_id)
{
    gchar class_name [ MAX_NAME_SIZE ] = {0};

    if (slot_id == NULL) {
        debug_log(DLOG_ERROR, "%s: Get slotid failed, slot_id is NULL.", __FUNCTION__);
        return;
    }
    if (dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE) != DFL_OK) {
        debug_log(DLOG_ERROR, "Failed to call dfl_get_class_name by obj [%s]", dfl_get_object_name(obj_handle));
        return;
    }
    if (g_strcmp0(CLASS_PCIE_CARD, class_name) == 0) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIE_CARD_SLOT_ID, slot_id);
    } else if (g_strcmp0(CLASS_MEZZ, class_name) == 0) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_MEZZ_SLOT, slot_id);
    } else {
        debug_log(DLOG_ERROR, "obj [%s] class[%s] not support!", dfl_get_object_name(obj_handle), class_name);
    }
    return;
}


gint32 get_pcie_mgnt_init_state(PM_CHIP_INFO *pm_chip_info)
{
    if (pm_chip_info == NULL) {
        return RET_ERR;
    }

    return pm_chip_info->init_state;
}


LOCAL void *pcie_monitor_task(void *p) //
{
    guint8 pcie_card_state = 0;

    (void)prctl(PR_SET_NAME, (gulong) "PcieMonitor");

    
    (void)dfl_foreach_object(CLASS_CARD_ACTION, pcie_init_action_foreach, NULL, NULL);

    while (FALSE == g_status_hotplug_task_exit) {
        
        vos_task_delay(PCIE_MONITOR_TASK_SLEEP_TIME);
        

        pcie_card_state = 0;

        
        (void)dfl_foreach_object(CLASS_CARD_ACTION, pcie_hotplug_status_foreach, NULL, NULL);
        
        
        (void)dfl_foreach_object(CLASS_PCIESSDCARD_NAME, pcie_ssd_update_status, NULL, NULL);
        
        
        (void)dfl_foreach_object(CLASS_PCIE_CARD, pcie_card_update_info, &pcie_card_state, NULL);
        

        debug_log(DLOG_DEBUG, "%s fault state is %d", __FUNCTION__, pcie_card_state);
        pcie_card_set_pcieslot_fault_state(pcie_card_state);

        
        (void)dfl_foreach_object(CLASS_EXTEND_PCIECARD_NAME, pcie_card_update_info, NULL, NULL);
        

        
        (void)dfl_foreach_object(CLASS_LOCAL_PCIECARD_NAME, pcie_card_update_info, NULL, NULL);
        (void)dfl_foreach_object(CLASS_OCP_CARD, pcie_card_update_info, NULL, NULL);
    }

    return NULL;
}


gint32 card_manage_smbios_state_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    guint8 smbios_state = SMBIOS_WRITE_NOT_START;

    if (property_value == NULL) {
        return RET_ERR;
    }

    smbios_state = g_variant_get_byte(property_value);
    if (smbios_state == SMBIOS_WRITE_FINISH) {
        (void)dfl_foreach_object(CLASS_NETCARD_NAME, update_netcard_and_port_bdf_info, NULL, NULL);
        gulong task_id;
        (void)vos_task_create(&task_id, "update_all_npus_bdf_task", (TASK_ENTRY)update_all_npus_bdf_task, (void *)NULL,
            DEFAULT_PRIORITY);
        (void)dfl_foreach_object(CLASS_SOC_BOARD, soc_board_start_log, NULL, NULL);
    }
    return RET_OK;
}

LOCAL gboolean is_need_check_bios_status(void)
{
    // 检查特定板卡
    return ((check_is_atlas_smm() != RET_OK) && (local_pcie_mgnt_enabled() != TRUE) &&
        (is_sdi_support_mcu_by_pciecard() != TRUE));
}


gint32 pcie_mgnt_init(void)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 boot_stage = 0;
    gint32 ret = RET_ERR;

    
    
    if ((is_need_check_bios_status() == TRUE)) {
        
        if (dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &obj_handle) != RET_OK) {
            return RET_OK;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_BOOT_STAGE, &boot_stage);

        if (BIOS_BOOT_STAGE_PCIE_INFO_REPORTED != boot_stage) {
            return RET_ERR;
        }
    }

    
    
    if (dfl_get_object_list(CLASS_LOCAL_PCIECARD_NAME, &obj_list) == DFL_OK) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            obj_handle = (OBJ_HANDLE)obj_node->data;
            ret = pcie_mgnt_init_info(obj_handle, NULL);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "%s: init for %s failed. ret=%d", __FUNCTION__, dfl_get_object_name(obj_handle),
                    ret);
                continue;
            }
        }
        g_slist_free(obj_list);
        obj_list = NULL;
    }
    if (dfl_get_object_list(CLASS_OCP_CARD, &obj_list) == DFL_OK) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            (void)pcie_mgnt_init_info((OBJ_HANDLE)obj_node->data, NULL);
        }
        g_slist_free(obj_list);
        obj_list = NULL;
    }
    
    if (dfl_get_object_list(CLASS_PCIECARD_NAME, &obj_list) == DFL_OK) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            (void)pcie_mgnt_init_info((OBJ_HANDLE)obj_node->data, NULL);
        }
        g_slist_free(obj_list);
        obj_list = NULL;
    }
 
    
    if (dfl_get_object_list(CLASS_EXTEND_PCIECARD_NAME, &obj_list) == DFL_OK) {
        for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
            (void)pcie_mgnt_init_info((OBJ_HANDLE)obj_node->data, NULL);
        }
        g_slist_free(obj_list);
    }

    return RET_OK;
}


gint32 check_is_atlas_smm(void)
{
    guint32 icount = 0;
    guchar MgmtSoftWareType = 0;

    (void)dal_get_software_type(&MgmtSoftWareType);
    (void)dfl_get_object_count(CLASS_NAME_NODE_PCIEINFO_SUMMARY, &icount);

    if ((icount != 0) && (MGMT_SOFTWARE_TYPE_FRAME_PERIPHERAL_MGMT == MgmtSoftWareType)) {
        return RET_OK;
    }

    return RET_ERR;
}




gint32 pcie_card_start(void)
{
    gint32 ret;

    pm_ref_mutex_init();

    (void)vos_task_create(&g_status_hotplug_task, "pcie_hotplug", (TASK_ENTRY)pcie_monitor_task, (void *)NULL,
        DEFAULT_PRIORITY);

    (void)vos_task_create(&g_status_pcie_card_pdate_task, "pcie_card_update_monitor",
        (TASK_ENTRY)pcie_card_scan_update_list_task, (void *)NULL, DEFAULT_PRIORITY);

    
    if (local_pcie_mgnt_enabled() == TRUE || is_sdi_support_mcu_by_pciecard() == TRUE) {
        info_pcie_card_change();
        debug_log(DLOG_INFO, "%s: local pcie card mgnt or sdi5x supported", __FUNCTION__);
    }

    
    ret = vos_queue_create(&g_pcie_dec_bandwidth_msg_queue);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Create pcie decrease lanewidth message queue failed!", __FUNCTION__);
    }
    
    return RET_OK;
}


gint32 pcie_card_change_notify_mctp(void)
{
    gint32 status = 0;
    guchar board_type = 0;
    GSList *local_output_list = NULL;
    OBJ_HANDLE handle = 0;
    gint32 ret = RET_ERR;
    
    ret = dal_rf_get_board_type(&board_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s] get board type failed.", __FUNCTION__);
        return RET_ERR;
    }

    if (board_type != BOARD_BLADE && board_type != BOARD_RACK) {
        return RET_OK;
    }
    ret = dal_get_object_handle_nth(MCTP_CLASS_NAME, 0, &handle);
    
    if (ret == RET_OK) {
        ret = dfl_call_class_method(handle, METHOD_MCTP_ROUTE_TABLE_TO_CHANGE, NULL, NULL, &local_output_list);
        if (ret == DFL_OK) {
            // 0为处于recover状态
            status = g_variant_get_int32((GVariant *)g_slist_nth_data(local_output_list, 0));
            debug_log(DLOG_ERROR, "[%s] change mctp route table success status(%d)", __FUNCTION__, status);
        } else {
            debug_log(DLOG_ERROR, "[%s] get method %s fail, ret = %d", __FUNCTION__, METHOD_MCTP_ROUTE_TABLE_TO_CHANGE,
                ret);
        }
    } else {
        debug_log(DLOG_ERROR, "[%s] get the %s object fail, ret = %d", __FUNCTION__, MCTP_CLASS_NAME, ret);
    }

    g_slist_free_full(local_output_list, (GDestroyNotify)g_variant_unref);
    local_output_list = NULL;
    return ret;
}


gint32 get_card_obj_handle_by_slot_position(OBJ_HANDLE *obj_handle, guint8 slot_id, guint32 psosition)
{
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 node_slot_id = 0;
    guint32 node_psosition = 0;
    gint32 ret = 0;

    if (obj_handle == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_list(CLASS_PCIE_CARD, &obj_list);
    if (ret != DFL_OK) {
        return ret;
    }

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPETRY_PCIECARD_SLOT, &node_slot_id);
        if (node_slot_id != slot_id) {
            continue;
        }

        if (psosition != 0) {
            node_psosition = dfl_get_position((OBJ_HANDLE)obj_node->data);
            if (node_psosition != psosition) {
                continue;
            }
        }

        *obj_handle = (OBJ_HANDLE)obj_node->data;

        break;
    }

    g_slist_free(obj_list);

    return RET_OK;
}


gint32 check_third_pcie_raid_card(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gint32 ret;
    guint8 i = 0;
    PCIE_CARD_GROUP_CLASS_INFO_S pcie_cards_class_info;

    (void)memset_s(&pcie_cards_class_info, sizeof(PCIE_CARD_GROUP_CLASS_INFO_S), 0,
        sizeof(PCIE_CARD_GROUP_CLASS_INFO_S));
    if (g_bdf_info_parsed_status == BDF_UNPARSED_STATE) {
        debug_log(DLOG_DEBUG, "%s:bdf unparsed.", __FUNCTION__);
        return RET_ERR;
    }

    ret = pcie_card_get_class_info(&g_pcie_cards_bdf_info, &pcie_cards_class_info);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    for (i = 0; i < g_pcie_group_sub_info.count && i < pcie_cards_class_info.count; i++) {
        debug_log(DLOG_DEBUG, "class_code=0x%02X sub_class_code=0x%02X sub_vender_id=0x%04X sub_device_id=0x%04X",
            pcie_cards_class_info.class_info[i].class_code, pcie_cards_class_info.class_info[i].sub_class_code,
            g_pcie_group_sub_info.sub_info[i].sub_vender_id, g_pcie_group_sub_info.sub_info[i].sub_device_id);
        // 当class_code为0x01(Mass storage controller), 且sub_class_code为0x04时，表明PCIe设备是Raid控制器；
        // 若同时sub_vender_id 不是 0x19e5, 表明是第三方Raid标卡
        if (pcie_cards_class_info.class_info[i].class_code == MASS_STORAGE_CONTROLLER_CODE &&
            (pcie_cards_class_info.class_info[i].sub_class_code == RAID_CONTROLLER_CODE ||
            pcie_cards_class_info.class_info[i].sub_class_code == SCSI_CONTROLLER_CODE) &&
            HUAWEI_VENDER_ID != g_pcie_group_sub_info.sub_info[i].sub_vender_id) {
            *output_list = g_slist_append(*output_list, g_variant_new_byte((guint8)HAVE_THIRD_PCIE_RAID_CARD));
            debug_log(DLOG_ERROR, "%s:Find a third PCIe Raid card: vid=0x%04X did=0x%04X svid=0x%04X sdid=0x%04X",
                __FUNCTION__, g_pcie_group.info[i].vender_id, g_pcie_group.info[i].device_id,
                g_pcie_group_sub_info.sub_info[i].sub_vender_id, g_pcie_group_sub_info.sub_info[i].sub_device_id);
            return RET_OK;
        }

        // class_code、sub_class_code、prog、revision四个字段全为0xFE表示该卡的class code信息未知，
        // 可能是Raid卡，为避免误告警，将其当做第三方Raid标卡处理
        if (pcie_cards_class_info.class_info[i].class_code == 0xFE &&
            pcie_cards_class_info.class_info[i].sub_class_code == 0xFE &&
            pcie_cards_class_info.class_info[i].prog == 0xFE && 0xFE == pcie_cards_class_info.class_info[i].revision) {
            *output_list = g_slist_append(*output_list, g_variant_new_byte((guint8)HAVE_THIRD_PCIE_RAID_CARD));
            debug_log(DLOG_ERROR,
                "%s:Find a PCIe Device get class info failed: vid=0x%04X did=0x%04X svid=0x%04X sdid=0x%04X",
                __FUNCTION__, g_pcie_group.info[i].vender_id, g_pcie_group.info[i].device_id,
                g_pcie_group_sub_info.sub_info[i].sub_vender_id, g_pcie_group_sub_info.sub_info[i].sub_device_id);
            return RET_OK;
        }
    }
    *output_list = g_slist_append(*output_list, g_variant_new_byte((guint8)NO_THIRD_PCIE_RAID_CARD));
    return RET_OK;
}


void get_group_info_by_type(guint8 card_type, PCIE_CARD_GROUP_INFO_S **card_info,
    PCIE_CARD_GROUP_SUB_INFO_S **card_sub_info)
{
    if (card_type == PCIE_AND_PCIEPLUS_CONNECTOR) {
        if (card_info != NULL) {
            (*card_info) = &g_pcie_group;
        }
        if (card_sub_info != NULL) {
            (*card_sub_info) = &g_pcie_group_sub_info;
        }
    } else {
        debug_log(DLOG_ERROR, "%s: Unknow card type. card_type = %d.", __FUNCTION__, card_type);
    }

    return;
}

void get_pcie_status_info(PCIE_CARD_STATUS_GROUP_S **status_group, PCIE_CARD_STATUS_GROUP_S **status_by_bios_group)
{
    if (status_group != NULL) {
        (*status_group) = &g_pcie_status_group;
    }
    if (status_by_bios_group != NULL) {
        (*status_by_bios_group) = &g_pcie_status_by_bios_group;
    }
    return;
}


gint32 pciecard_test_bdf_is_valid(guint8 socket_id, guint8 bus, guint8 device, guint8 function,
    PCIE_VIDDID_SVIDSDID *pcie_viddid_ptr)
{
    gint32 ret = RET_OK;
    INFO_NEED_GET_PCIE_INFO info;
    guint32 response = 0;
    guchar me_access_type;
    
    me_access_type = pcie_card_get_me_access_type();
    if (me_access_type == ME_ACCESS_TYPE_SMBUS_PECI) {
        return RET_OK;
    }
    

    if (pcie_viddid_ptr == NULL) {
        debug_log(DLOG_ERROR, "%s: Invalid input!", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(&info, sizeof(INFO_NEED_GET_PCIE_INFO), 0, sizeof(INFO_NEED_GET_PCIE_INFO));
    info.cpu_num = socket_id;
    info.bus = bus;
    info.dev = device;
    info.func = function;
    info.is_local = 0;
    info.address = 0x00;
    info.length = 4; // 获取的数据长度为4字节（VID + DID）

    
    ret = pcie_get_info_from_me(&info, (guint8 *)&response);
    if (ret != RET_OK) {
        debug_log(DLOG_INFO, "Read BDF(socket:%d,bus:%d,device:%d,funtion:%d) related card VID/DID Failed,ret=%d.",
            socket_id, bus, device, function, ret);
        return ret;
    }

    pcie_viddid_ptr->vid = response & 0x0000ffff;
    pcie_viddid_ptr->did = (response & 0xffff0000) >> 16; // 返回数据最后16位为DID
    debug_log(DLOG_INFO, "Read BDF(socket:%d,bus:%d,device:%d,funtion:%d) related card VID:0X%04X,DID:0X%04X.",
        socket_id, bus, device, function, response & 0x0000ffff,
        (response & 0xffff0000) >> 16); // 返回数据最后16位为DID
    return ret;
}


LOCAL void pcie_card_operation_log(guint8 type, guint8 slot_id, guint8 option)
{
    GSList* caller_info_list = NULL;
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("N/A"));
    caller_info_list = g_slist_append(caller_info_list, g_variant_new_string("127.0.0.1"));

    method_operation_log(caller_info_list, NULL,
        "%s card%d(vid: 0x%04x, did: 0x%04x, sub_vid: 0x%04x, sub_did: 0x%04x) %s successfully",
        (OCPCARD_CONNECTOR == type) ? "OCP" : "PCIe",
        slot_id + 1,
        g_pcie_group.info[slot_id].vender_id,
        g_pcie_group.info[slot_id].device_id,
        g_pcie_group_sub_info.sub_info[slot_id].sub_vender_id,
        g_pcie_group_sub_info.sub_info[slot_id].sub_device_id,
        (PCIE_CARD_XML_LOAD == option) ? "plug in" : "plug out");
    g_slist_free_full(caller_info_list, (GDestroyNotify)g_variant_unref);
    return;
}

LOCAL void pcie_card_info_parse(PCIE_CARD_IPMI_INFO_S *pcie_card_data, const guint8 *req_data_buf)
{
    guint8 slot_id;

    for (slot_id = 0; slot_id < pcie_card_data->pcie_card_info.count; slot_id++) {
        
        pcie_card_data->pcie_card_info.info[slot_id].vender_id = MAKE_WORD(*(req_data_buf + 3 + slot_id * 8),
            *(req_data_buf + 2 + slot_id * 8)); // 小端在前 vid data2-3 Subdid data8-9
        
        pcie_card_data->pcie_card_info.info[slot_id].device_id = MAKE_WORD(*(req_data_buf + 5 + slot_id * 8),
            *(req_data_buf + 4 + slot_id * 8)); // 小端在前 did data4-5 Subdid data8-9
        
        pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_vender_id = MAKE_WORD(*(req_data_buf
            + 7 + slot_id * 8), *(req_data_buf + 6 + slot_id * 8)); // 小端在前 Subvid data6-7 Subdid data8-9
        
        pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_device_id = MAKE_WORD(*(req_data_buf
            + 9 + slot_id * 8), *(req_data_buf + 8 + slot_id * 8)); // 小端在前 Subdid data8-9
        pcie_card_data->pcie_card_status_info.status[slot_id] = PCIE_NO_FATALERROR;
        debug_log(DLOG_ERROR, "Pcie card%d, Vender id: %4x, Device id: %4x, Sub Vender id: %4x, Sub Device id: %4x",
            slot_id + 1,
            pcie_card_data->pcie_card_info.info[slot_id].vender_id,
            pcie_card_data->pcie_card_info.info[slot_id].device_id,
            pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_vender_id,
            pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_device_id);
    }
    pcie_card_data->slot_id = slot_id;
}


LOCAL void pcie_card_xml_unload_for_bios(PCIE_CARD_IPMI_INFO_S *pcie_card_data)
{
    guint8 slot_id;
    gint32 ret;
    guint8 i = pcie_card_data->slot_id;
    volatile guint32 old_boardid;
    volatile guint32 new_boardid;
    volatile guint32 old_auxid;
    volatile guint32 new_auxid;
    OBJ_HANDLE obj_handle = 0;
    
    for (slot_id = 0; slot_id < i; slot_id++) {
        old_boardid = g_pcie_group.info[slot_id].vender_id * 0x10000 + g_pcie_group.info[slot_id].device_id;
        old_auxid = g_pcie_group_sub_info.sub_info[slot_id].sub_vender_id * 0x10000 +
            g_pcie_group_sub_info.sub_info[slot_id].sub_device_id;
        new_boardid = pcie_card_data->pcie_card_info.info[slot_id].vender_id * 0x10000 +
            pcie_card_data->pcie_card_info.info[slot_id].device_id;
        new_auxid = pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_vender_id * 0x10000 +
            pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_device_id;
        if (old_boardid == new_boardid && old_auxid == new_auxid) {
            continue;
        }
        if (old_boardid != PCIE_NO_CARD && old_auxid != PCIE_NO_CARD) {
            debug_log(DLOG_ERROR, "PCI-E PullOut Slot %d, VID=%04x DID=%04x SubVID=%04x SubDID=%04x",
                (slot_id + 1),
                g_pcie_group.info[slot_id].vender_id, g_pcie_group.info[slot_id].device_id,
                g_pcie_group_sub_info.sub_info[slot_id].sub_vender_id,
                g_pcie_group_sub_info.sub_info[slot_id].sub_device_id);
            
            ret = pcie_card_get_connector_handle(slot_id + 1, &obj_handle, PCIE_AND_PCIEPLUS_CONNECTOR);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail!", slot_id + 1);
                continue;
            }
            
            (void)pcie_card_load_xml(obj_handle, old_boardid, old_auxid, PCIE_CARD_XML_UNLOAD,
                PCIE_AND_PCIEPLUS_CONNECTOR);
            pcie_card_operation_log(PCIE_AND_PCIEPLUS_CONNECTOR, slot_id, PCIE_CARD_XML_UNLOAD);
        }
    }
}


LOCAL void pcie_card_xml_load_for_bios(PCIE_CARD_IPMI_INFO_S *pcie_card_data)
{
    guint8 slot_id;
    gint32 ret;
    guint8 i = pcie_card_data->slot_id;
    volatile guint32 old_boardid;
    volatile guint32 old_auxid;
    volatile guint32 new_boardid;
    volatile guint32 new_auxid;
    OBJ_HANDLE obj_handle = 0;
    for (slot_id = 0; slot_id < i; slot_id++) {
        old_boardid = g_pcie_group.info[slot_id].vender_id * 0x10000 + g_pcie_group.info[slot_id].device_id;
        old_auxid = g_pcie_group_sub_info.sub_info[slot_id].sub_vender_id * 0x10000 +
            g_pcie_group_sub_info.sub_info[slot_id].sub_device_id;
        new_boardid = pcie_card_data->pcie_card_info.info[slot_id].vender_id * 0x10000 +
            pcie_card_data->pcie_card_info.info[slot_id].device_id;
        new_auxid = pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_vender_id * 0x10000 +
            pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_device_id;
        if (old_boardid == new_boardid && old_auxid == new_auxid) {
            continue;
        }
        if (new_boardid != PCIE_NO_CARD && new_auxid != PCIE_NO_CARD) {
            debug_log(DLOG_ERROR, "PCI-E PullIn Slot %d, VID=%04x DID=%04x SubVID=%04x SubDID=%04x",
                (slot_id + 1),
                pcie_card_data->pcie_card_info.info[slot_id].vender_id,
                pcie_card_data->pcie_card_info.info[slot_id].device_id,
                pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_vender_id,
                pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_device_id);
            
            ret = pcie_card_get_connector_handle(slot_id + 1, &obj_handle, PCIE_AND_PCIEPLUS_CONNECTOR);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail!", slot_id);
                continue;
            }
            g_pcie_group.info[slot_id].vender_id = pcie_card_data->pcie_card_info.info[slot_id].vender_id;
            g_pcie_group.info[slot_id].device_id = pcie_card_data->pcie_card_info.info[slot_id].device_id;
            guint16 tmp_sub_vender_id = pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_vender_id;
            guint16 tmp_sub_device_id = pcie_card_data->pcie_card_sub_info.sub_info[slot_id].sub_device_id;
            g_pcie_group_sub_info.sub_info[slot_id].sub_vender_id = tmp_sub_vender_id;
            g_pcie_group_sub_info.sub_info[slot_id].sub_device_id = tmp_sub_device_id;
            
            (void)pcie_card_load_xml(obj_handle, new_boardid, new_auxid, PCIE_CARD_XML_LOAD,
                PCIE_AND_PCIEPLUS_CONNECTOR);
            pcie_card_operation_log(PCIE_AND_PCIEPLUS_CONNECTOR, slot_id, PCIE_CARD_XML_LOAD);
        }
    }
}


LOCAL void pcie_card_info_per_save(PCIE_CARD_IPMI_INFO_S *pcie_card_data)
{
    memcpy_s(&g_pcie_group, sizeof(g_pcie_group), &pcie_card_data->pcie_card_info, sizeof(PCIE_CARD_GROUP_INFO_S));
    per_save((guint8*)&g_pcie_group);

    memcpy_s(&g_pcie_status_group, sizeof(g_pcie_status_group), &pcie_card_data->pcie_card_status_info,
        sizeof(PCIE_CARD_STATUS_GROUP_S));
    per_save((guint8*)&g_pcie_status_group);

    memcpy_s(&g_pcie_group_sub_info, sizeof(g_pcie_group_sub_info), &pcie_card_data->pcie_card_sub_info,
        sizeof(PCIE_CARD_GROUP_SUB_INFO_S));
    per_save((guint8*)&g_pcie_group_sub_info);

    notice_pcie_info(&pcie_card_data->pcie_card_info, PCIE_SWITCH_CLASEE_NAME, PCIE_SWITCH_METHOD_WRITE_PCIESW_CFG);
    notice_pcie_info(&pcie_card_data->pcie_card_info, CLASS_NAME_BIOS, METHOD_BIOS_NOTIFY_PCIE_INFO);
}


gint32 pciecard_parse_vid_did_subvid_subdid(gconstpointer msg_data, gpointer user_data)
{
#define ONE_PCIE_CARD_INFO_SIZE 8
    PCIE_CARD_IPMI_INFO_S pcie_card_data;
    const guint8 *req_data_buf = NULL;
    
    if (msg_data == NULL) {
        return VOS_ERR;
    }
    if (filter_msg_by_sys_channel(msg_data)) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_STATUS_INVALID);
    }

    req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    (void)memset_s(&pcie_card_data, sizeof(PCIE_CARD_IPMI_INFO_S), 0, sizeof(PCIE_CARD_IPMI_INFO_S));

    pcie_card_data.pcie_card_info.count = *(req_data_buf + 1);
    debug_log(DLOG_ERROR, "Pcie card number: %d", pcie_card_data.pcie_card_info.count);

    
    if (pcie_card_data.pcie_card_info.count > MAX_PCIE_CARD) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_PARA_NOT_SUPPORT);
    }
    
    if (pcie_card_data.pcie_card_info.count * ONE_PCIE_CARD_INFO_SIZE + 1 >= get_ipmi_src_data_len(msg_data)) {
        debug_log(DLOG_ERROR, "%s: pcie_card_info count %d size > datalen %d", __FUNCTION__,
            pcie_card_data.pcie_card_info.count, get_ipmi_src_data_len(msg_data));
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    
    ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);
    
    pcie_card_info_parse(&pcie_card_data, req_data_buf);
    
    pcie_card_xml_unload_for_bios(&pcie_card_data);
    
    vos_task_delay(3000);
    
    pcie_card_xml_load_for_bios(&pcie_card_data);
    pcie_card_info_per_save(&pcie_card_data);

    return RET_OK;
}

void update_rootbdf_info(guint8 type, PORT_GROUP_BDF_INFO_S bdf_info)
{
    switch (type) {
        case TYPE_PCIE_CARD:
            g_pcie_port_rootbdf_info = bdf_info;
            per_save((guint8 *)&g_pcie_port_rootbdf_info);
            break;
        case TYPE_OCP:
            g_ocp_port_rootbdf_info = bdf_info;
            per_save((guint8 *)&g_ocp_port_rootbdf_info);
            break;
        case TYPE_RAID:
            g_pcie_raid_rootbdf_info = bdf_info;
            per_save((guint8 *)&g_pcie_raid_rootbdf_info);
            break;
        default:
            debug_log(DLOG_ERROR, "type is invalid, type = %d", type);
            break;
    }
}