

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

#define MAX_RETRY_TIMES   12
#define RETURN_CONTINUE  0x10
#define BDF_STR_LEN 12

LOCAL void update_ocp_netcard_rootbdf(PORT_GROUP_BDF_INFO_S *ocp_port_bdf_info);
LOCAL void update_pcie_netcard_rootbdf(PORT_GROUP_BDF_INFO_S *pcie_port_bdf_info);
LOCAL void pcie_port_update_bdf(guint8 type, PORT_GROUP_BDF_INFO_S *port_bdf_info);
LOCAL void update_pcie_raid_rootbdf(PORT_GROUP_BDF_INFO_S *pcie_raid_bdf_info);

typedef struct {
    gchar info[PCIE_CARD_DUMPINFO_MAX_LEN];
    gchar path_builder[PCIE_CARD_XMLFILE_MAX_LEN];
} XML_PATH;

LOCAL XML_PATH g_xml_path_tbl[] = {
    {"static xml path", "/opt/pme/extern/profile"},
    {"upgrade xml path", "/data/opt/pme/conf/profile"},
    {"self discovery xml path", "/data/opt/pme/conf/dynamic"},
};
LOCAL XML_PATH g_pme_xml_path_tbl[] = {
    {"pme xml path", "/opt/pme/extern/profile"},
    {"pme upgrade xml path", "/data/opt/pme/conf/profile"},
};
LOCAL gint32 xml_path_build(gchar *xml_name, guint32 len,
    const gchar *bom_value, guint32 board_id, guint32 aux_id)
{
    guint32 i;
    gint32 ret;
    
    for (i = 0; i < sizeof(g_xml_path_tbl) / sizeof(XML_PATH); i++) {
        ret = snprintf_s(xml_name, len + 1, len, "%s/%s_%08x_%08x.xml",
            g_xml_path_tbl[i].path_builder, bom_value, board_id, aux_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: %s snprintf_s fail, ret = %d", __FUNCTION__, g_xml_path_tbl[i].info, ret);
            continue;
        }
        if (vos_get_file_accessible(xml_name) == TRUE) {
            return RET_OK;
        }
    }
    for (i = 0; i < sizeof(g_pme_xml_path_tbl) / sizeof(XML_PATH); i++) {
        ret = snprintf_s(xml_name, len + 1, len, "%s/%s_%x_%x.xml",
            g_pme_xml_path_tbl[i].path_builder, bom_value, board_id, aux_id);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: %s snprintf_s fail, ret = %d", __FUNCTION__, g_pme_xml_path_tbl[i].info, ret);
            continue;
        }
        if (vos_get_file_accessible(xml_name) == TRUE) {
            return RET_OK;
        }
    }
    return RET_ERR;
}


gint32 card_connector_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type)
{
    gint32 ret;
    gchar bom_value[CONNECTOR_BOM_ID_MAX_LEN] = {0};
    gchar xml_name[PCIE_CARD_XMLFILE_MAX_LEN + 1] = {0};
    const gchar *card_type_string = pcie_card_get_card_type_string(type);

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_CONNECTOR_BOM, bom_value, sizeof(bom_value));
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = xml_path_build(xml_name, PCIE_CARD_XMLFILE_MAX_LEN, bom_value, board_id, aux_id);
    if (ret != RET_OK) {
        ret = xml_path_build(xml_name, PCIE_CARD_XMLFILE_MAX_LEN, bom_value, board_id, PCIE_INVALID_VID_DID);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s %s %s_%08x_%08x.xml", (state ? "Load" : "Unload"), card_type_string, bom_value,
                0xfffffffe, PCIE_INVALID_VID_DID);
            
            board_id = 0xfffffffe;
            aux_id = PCIE_INVALID_VID_DID;
            ret = dfl_notify_presence_v2(obj_handle, board_id, aux_id, state);
        } else {
            debug_log(DLOG_ERROR, "%s %s %s_%08x_%08x.xml", (state ? "Load" : "Unload"), card_type_string, bom_value,
                board_id, PCIE_INVALID_VID_DID);
            aux_id = PCIE_INVALID_VID_DID;
            ret = dfl_notify_presence_v2(obj_handle, board_id, aux_id, state);
        }
    } else {
        debug_log(DLOG_ERROR, "%s %s %s_%08x_%08x.xml", (state ? "Load" : "Unload"), card_type_string, bom_value,
            board_id, aux_id);
        ret = dfl_notify_presence_v2(obj_handle, board_id, aux_id, state);
    }
    debug_log(DLOG_INFO, "%s %s xml(%s) successfully!", (PCIE_CARD_XML_LOAD == state) ? "Load" : "Unload",
        card_type_string, xml_name);
    return ret;
}


gint32 disk_connector_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type)
{
    gint32 ret;
    gchar bom_value[CONNECTOR_BOM_ID_MAX_LEN] = {0};
    gchar xml_name[PCIE_CARD_XMLFILE_MAX_LEN + 1] = {0};
    gchar xml_name_upgrade[PCIE_CARD_XMLFILE_MAX_LEN + 1] = {0};
    const gchar *card_type_string = pcie_card_get_card_type_string(type);

    
    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_CONNECTOR_BOM, bom_value, sizeof(bom_value));
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = snprintf_s(xml_name, PCIE_CARD_XMLFILE_MAX_LEN + 1, PCIE_CARD_XMLFILE_MAX_LEN,
        "/opt/pme/extern/profile/%s_%08x.xml", bom_value, board_id);
    ret += snprintf_s(xml_name_upgrade, PCIE_CARD_XMLFILE_MAX_LEN + 1, PCIE_CARD_XMLFILE_MAX_LEN,
        "/data/opt/pme/conf/profile/%s_%08x.xml", bom_value, board_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    
    
    if ((vos_get_file_accessible(xml_name) == FALSE) || (vos_get_file_accessible(xml_name_upgrade) == FALSE)) {
        debug_log(DLOG_ERROR, "No %s_%08x.xml file!", bom_value, board_id);
        ret = dfl_notify_presence(obj_handle, PCIE_INVALID_VID_DID, state);
        return RET_ERR;
    } else {
        debug_log(DLOG_ERROR, "%s %s %s_%08x.xml", (state ? "Load" : "Unload"), card_type_string, bom_value, board_id);
        ret = dfl_notify_presence(obj_handle, board_id, state);
    }
    debug_log(DLOG_INFO, "%s %s xml(%s) successfully!", (PCIE_CARD_XML_LOAD == state) ? "Load" : "Unload",
        card_type_string, xml_name);
    return ret;
}


gint32 nvme_vpd_connector_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type)
{
    gint32 ret;
    gchar bom_value[CONNECTOR_BOM_ID_MAX_LEN] = {0};
    gchar xml_name[PCIE_CARD_XMLFILE_MAX_LEN + 1] = {0};
    gchar xml_name_upgrade[PCIE_CARD_XMLFILE_MAX_LEN + 1] = {0};
    const gchar *card_type_string = pcie_card_get_card_type_string(type);

    
    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_CONNECTOR_BOM, bom_value, sizeof(bom_value));
    if (ret != RET_OK) {
        return RET_ERR;
    }
    ret = snprintf_s(xml_name, PCIE_CARD_XMLFILE_MAX_LEN + 1, PCIE_CARD_XMLFILE_MAX_LEN,
        "/opt/pme/extern/profile/%s_%02x.xml", bom_value, board_id);
    ret += snprintf_s(xml_name_upgrade, PCIE_CARD_XMLFILE_MAX_LEN + 1, PCIE_CARD_XMLFILE_MAX_LEN,
        "/data/opt/pme/conf/profile/%s_%02x.xml", bom_value, board_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }
    
    
    if ((vos_get_file_accessible(xml_name) == FALSE) && (vos_get_file_accessible(xml_name_upgrade) == FALSE)) {
        debug_log(DLOG_ERROR, "No %s_%02x.xml file!", bom_value, board_id);
        return RET_ERR;
    } else {
        debug_log(DLOG_ERROR, "%s %s %s_%02x.xml", (state ? "Load" : "Unload"), card_type_string, bom_value, board_id);
        ret = dfl_notify_presence(obj_handle, board_id, state);
    }
    debug_log(DLOG_INFO, "%s %s xml(%s) successfully!", (PCIE_CARD_XML_LOAD == state) ? "Load" : "Unload",
        card_type_string, xml_name);
    return ret;
}

gint32 pcie_card_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type)
{
    gint32 ret;

    
    if ((type == PCIE_AND_PCIEPLUS_CONNECTOR) || (type == OCPCARD_CONNECTOR) || (type == PANGEA_CARD_CONNECTOR)) {
        ret = card_connector_load_xml(obj_handle, board_id, aux_id, state, type);
    } else if (type == DISK_CONNECTOR) {
        ret = disk_connector_load_xml(obj_handle, board_id, aux_id, state, type);
    } else if (type == NVME_VPD_CONNECTOR) {
        ret = nvme_vpd_connector_load_xml(obj_handle, board_id, aux_id, state, type);
    } else {
        debug_log(DLOG_ERROR, "Can't load wrong type : %d PCIE xml.", type);
        return RET_ERR;
    }
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Can't load wrong type : %d PCIE xml.", type);
    }
    return RET_OK;
}


LOCAL void pcie_cards_load_xml_by_per_data(PCIE_CARD_GROUP_INFO_S *pcie_group_info,
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info, guint8 connector_type)
{
    gint32 ret = 0;
    guint8 pcie_index = 0;
    guint8 pcie_slot_id = 0;
    volatile guint32 board_id = 0;
    guint32 aux_id = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 present_status = 0x00;
    const gchar *card_type_string = pcie_card_get_card_type_string(connector_type);

    for (pcie_index = 0; pcie_index < pcie_group_info->count; pcie_index++) {
        board_id = pcie_group_info->info[pcie_index].vender_id * 0x10000 + pcie_group_info->info[pcie_index].device_id;
        if (board_id == PCIE_NO_CARD) {
            continue;
        }

        aux_id = pcie_group_sub_info->sub_info[pcie_index].sub_vender_id * 0x10000 +
            pcie_group_sub_info->sub_info[pcie_index].sub_device_id;
        
        pcie_slot_id = (connector_type == DISK_CONNECTOR) ? pcie_index : pcie_index + 1;
        
        ret = pcie_card_get_connector_handle(pcie_slot_id, &obj_handle, connector_type);
        if (ret != RET_OK) {
            set_pcie_card_infos_default_value(pcie_group_info, pcie_group_sub_info, pcie_index);
            debug_log(DLOG_ERROR, "Get %s%d(0x%08x) connector handle fail and erase VIDDID to 0x%04x%04x!",
                card_type_string, pcie_slot_id, board_id, pcie_group_info->info[pcie_index].vender_id,
                pcie_group_info->info[pcie_index].device_id);
            maintenance_log_v2(MLOG_WARN, FC_CARD_MNG_PCIE_XML_ERR,
                "[Init] Load %s%d[0x%08x] failed because can not find Connector.\n", card_type_string, pcie_slot_id,
                board_id);
            
            continue;
        }

        ret = get_pcie_device_present_status(pcie_slot_id, connector_type, &present_status);
        if (ret == RET_OK) {
            if (present_status == 0) {
                debug_log(DLOG_ERROR, "[%s][%d] %s in slot%d is not present", __FUNCTION__, __LINE__,
                    card_type_string, pcie_slot_id);
                continue;
            }
        }
        
        ret = pcie_card_load_xml(obj_handle, board_id, aux_id, PCIE_CARD_XML_LOAD, connector_type);
        if (ret != RET_OK) {
            set_pcie_card_infos_default_value(pcie_group_info, pcie_group_sub_info, pcie_index);
            debug_log(DLOG_ERROR, "Load %s%d(0x%08x) xml fail and erase VIDDID to 0x%04x%04x!", card_type_string,
                pcie_slot_id, board_id, pcie_group_info->info[pcie_index].vender_id,
                pcie_group_info->info[pcie_index].device_id);
            maintenance_log_v2(MLOG_WARN, FC_CARD_MNG_PCIE_XML_ERR, "[Init] Load %s%d[0x%08x] failed.\n",
                card_type_string, pcie_slot_id, board_id);
            continue;
        }

        debug_log(DLOG_INFO, "Load %s%d xml success!", card_type_string, pcie_index);

        
        vos_task_delay(1000); // 等待1000 ms
    }
}

gint32 pcie_card_ipmi_set_vender_device_id(const void *msg_data, gpointer user_data)
{
#define ONE_PCIE_CARD_INFO_SIZE 4
    gint32 ret = 0;
    guint8 slot_id = 0;
    guint32 connector_id = 0;
    guint8 i = 0;
    guint8 pcieplus_slot_id = 0xFF;
    PCIE_CARD_GROUP_INFO_S pcie_card_info;
    PCIE_CARD_STATUS_GROUP_S pcie_card_status_info;
    volatile guint32 old_bid = 0;   // 设置前的board_id
    volatile guint32 new_bid = 0;   // 新拼接的board_id
    volatile guint32 old_auxid = 0;
    volatile guint32 new_auxid = 0;
    const guint8 *req_data_buf = NULL;
    guint32 product_num = 0;
    OBJ_HANDLE obj_handle = 0;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }
    if (filter_msg_by_sys_channel(msg_data) == TRUE) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_STATUS_INVALID);
    }

    req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    (void)memset_s(&pcie_card_info, sizeof(PCIE_CARD_GROUP_INFO_S), 0, sizeof(PCIE_CARD_GROUP_INFO_S));

    pcie_card_info.count = *(req_data_buf + 1);
    debug_log(DLOG_INFO, "Pcie card number: %d", pcie_card_info.count);

    
    if (pcie_card_info.count > MAX_PCIE_CARD) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_PARA_NOT_SUPPORT);
    }
    
    if (pcie_card_info.count * ONE_PCIE_CARD_INFO_SIZE + 1 >= get_ipmi_src_data_len(msg_data)) {
        debug_log(DLOG_ERROR, "%s: pcie_card_info count %d size > datalen %d", __FUNCTION__, pcie_card_info.count,
            get_ipmi_src_data_len(msg_data));
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    

    
    ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);

    ret = pcie_disk_get_connector_slot_id(&pcieplus_slot_id, &connector_id);

    (void)dal_get_product_version_num(&product_num);

    for (slot_id = 0, i = 0; slot_id < pcie_card_info.count; slot_id++, i++) {
        
        
        
        if ((ret == RET_OK) && (pcieplus_slot_id == slot_id) && ((connector_id == 0x09) || (0x04 == connector_id)) &&
            (product_num <= PRODUCT_VERSION_V3)) {
            
            
            pcie_card_info.info[i].vender_id = 0xffff;
            pcie_card_info.info[i].device_id = 0xffff;
            pcie_card_status_info.status[i] = PCIE_NO_FATALERROR;
            debug_log(DLOG_INFO, "Pcie card%d, Vender id: %d, Device id: %d", i + 1, pcie_card_info.info[i].vender_id,
                pcie_card_info.info[i].device_id);
            i++;
        }

        pcie_card_info.info[i].vender_id =
            MAKE_WORD(*(req_data_buf + 3 + slot_id * 4), *(req_data_buf + 2 + slot_id * 4));
        pcie_card_info.info[i].device_id =
            MAKE_WORD(*(req_data_buf + 5 + slot_id * 4), *(req_data_buf + 4 + slot_id * 4));
        pcie_card_status_info.status[i] = PCIE_NO_FATALERROR;
        debug_log(DLOG_INFO, "Pcie card%d, Vender id: %d, Device id: %d", i + 1, pcie_card_info.info[i].vender_id,
            pcie_card_info.info[i].device_id);
    }

    

    
    for (slot_id = 0; slot_id < i; slot_id++) {
        old_bid = (g_pcie_group.info[slot_id].vender_id << VENDER_OFFSET) + g_pcie_group.info[slot_id].device_id;
        new_bid = (pcie_card_info.info[slot_id].vender_id << VENDER_OFFSET) + pcie_card_info.info[slot_id].device_id;

        
        old_auxid = (g_pcie_group_sub_info.sub_info[slot_id].sub_vender_id << VENDER_OFFSET) +
            g_pcie_group_sub_info.sub_info[slot_id].sub_device_id;
        

        
        if (old_bid != new_bid) {
            if (old_bid != PCIE_NO_CARD) {
                debug_log(DLOG_INFO, "PCI-E PullOut Slot %d, VID=%04x DID=%04x", (slot_id + 1),
                    g_pcie_group.info[slot_id].vender_id, g_pcie_group.info[slot_id].device_id);

                
                ret = pcie_card_get_connector_handle(slot_id + 1, &obj_handle, PCIE_AND_PCIEPLUS_CONNECTOR);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail!", slot_id);
                    continue;
                }

                (void)pcie_card_load_xml(obj_handle, old_bid, old_auxid, PCIE_CARD_XML_UNLOAD,
                    PCIE_AND_PCIEPLUS_CONNECTOR);
                pcie_card_log_operation_log(PCIE_AND_PCIEPLUS_CONNECTOR, slot_id + 1,
                    g_pcie_group.info[slot_id].vender_id, g_pcie_group.info[slot_id].device_id, PCIE_CARD_XML_UNLOAD);
            }
        }
    }

    vos_task_delay(3000);

    
    if (product_num >= PRODUCT_VERSION_V5) {
        (void)vos_thread_sem4_p(g_set_pcie_card_info_lock, 120 * 1000);
    } else {
        
        
        (void)vos_thread_sem4_p(g_set_pcie_card_info_lock, 60 * 1000);
        
    }
    

    for (slot_id = 0; slot_id < i; slot_id++) {
        old_bid = (g_pcie_group.info[slot_id].vender_id << VENDER_OFFSET) + g_pcie_group.info[slot_id].device_id;
        new_bid = (pcie_card_info.info[slot_id].vender_id << VENDER_OFFSET) + pcie_card_info.info[slot_id].device_id;
        new_auxid = PCIE_INVALID_VID_DID;

        
        if (old_bid != new_bid) {
            if (new_bid != PCIE_NO_CARD) {
                debug_log(DLOG_INFO, "PCI-E PlugIn Slot %d, VID=%04x DID=%04x", (slot_id + 1),
                    pcie_card_info.info[slot_id].vender_id, pcie_card_info.info[slot_id].device_id);

                
                ret = pcie_card_get_connector_handle(slot_id + 1, &obj_handle, PCIE_AND_PCIEPLUS_CONNECTOR);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail!", slot_id);
                    continue;
                }

                
                g_pcie_group.info[slot_id].vender_id = pcie_card_info.info[slot_id].vender_id;
                g_pcie_group.info[slot_id].device_id = pcie_card_info.info[slot_id].device_id;
                
                (void)pcie_card_load_xml(obj_handle, new_bid, new_auxid, PCIE_CARD_XML_LOAD,
                    PCIE_AND_PCIEPLUS_CONNECTOR);
                pcie_card_log_operation_log(PCIE_AND_PCIEPLUS_CONNECTOR, slot_id + 1,
                    pcie_card_info.info[slot_id].vender_id, pcie_card_info.info[slot_id].device_id, PCIE_CARD_XML_LOAD);
            }
        }
    }

    (void)memcpy_s(&g_pcie_group, sizeof(g_pcie_group), &pcie_card_info, sizeof(PCIE_CARD_GROUP_INFO_S));
    per_save((guint8 *)&g_pcie_group);

    
    (void)memcpy_s(&g_pcie_status_group, sizeof(g_pcie_status_group), &pcie_card_status_info,
        sizeof(PCIE_CARD_STATUS_GROUP_S));
    per_save((guint8 *)&g_pcie_status_group);
    

    
    notice_pcie_info(&pcie_card_info, PCIE_SWITCH_CLASEE_NAME, PCIE_SWITCH_METHOD_WRITE_PCIESW_CFG);
    notice_pcie_info(&pcie_card_info, CLASS_NAME_BIOS, METHOD_BIOS_NOTIFY_PCIE_INFO);
    

    return RET_OK;
}

LOCAL gint32 pcie_card_load_per_card(void)
{
    // PCIe卡根据持久化信息加载XML
    pcie_cards_load_xml_by_per_data(&g_pcie_group, &g_pcie_group_sub_info, PCIE_AND_PCIEPLUS_CONNECTOR);

    // OCP卡根据持久化信息加载XML
    pcie_cards_load_xml_by_per_data(&g_ocp_group, &g_ocp_group_sub_info, OCPCARD_CONNECTOR);

    
    if (pcie_card_get_vmd_state() == ENABLE) {
        return RET_OK;
    }

    // PCIe硬盘根据持久化信息加载XML
    pcie_cards_load_xml_by_per_data(&g_pcie_disk_group, &g_pcie_disk_group_sub_info, DISK_CONNECTOR);

    return RET_OK;
}

gint32 pcie_disk_ipmi_set_vender_device_id(const void *msg_data, gpointer user_data)
{
#define ONE_PCIE_DISK_INFO_SIZE 4
    gint32 ret = 0;
    guint8 pcie_index = 0;
    PCIE_CARD_GROUP_INFO_S pcie_disk_info;
    guint32 old_boardid = 0;
    guint32 new_boardid = 0;
    guint32 old_auxid = 0;
    guint32 new_auxid = 0;
    OBJ_HANDLE obj_handle = 0;
    const guint8 *req_data_buf = NULL;
    guint32 product_num = 0;

    
    if (msg_data == NULL) {
        return RET_ERR;
    }

    if (filter_msg_by_sys_channel(msg_data)) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_STATUS_INVALID);
    }

    req_data_buf = get_ipmi_src_data(msg_data);
    if (req_data_buf == NULL) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }
    (void)memset_s(&pcie_disk_info, sizeof(PCIE_CARD_GROUP_INFO_S), 0, sizeof(PCIE_CARD_GROUP_INFO_S));

    pcie_disk_info.count = *(req_data_buf + 1);
    debug_log(DLOG_INFO, "Pcie card number: %d", pcie_disk_info.count);

    
    if (pcie_disk_info.count > MAX_PCIE_CARD) {
        return ipmi_send_simple_response(msg_data, COMP_CODE_PARA_NOT_SUPPORT);
    }
    
    if (pcie_disk_info.count * ONE_PCIE_DISK_INFO_SIZE + 1 >= get_ipmi_src_data_len(msg_data)) {
        debug_log(DLOG_ERROR, "%s: pcie_disk_info count %d size > datalen %d", __FUNCTION__, pcie_disk_info.count,
            get_ipmi_src_data_len(msg_data));
        return ipmi_send_simple_response(msg_data, COMP_CODE_INVALID_FIELD);
    }

    for (pcie_index = 0; pcie_index < pcie_disk_info.count; pcie_index++) {
        pcie_disk_info.info[pcie_index].vender_id =
            MAKE_WORD(*(req_data_buf + 3 + pcie_index * 4), *(req_data_buf + 2 + pcie_index * 4));
        pcie_disk_info.info[pcie_index].device_id =
            MAKE_WORD(*(req_data_buf + 5 + pcie_index * 4), *(req_data_buf + 4 + pcie_index * 4));
        debug_log(DLOG_INFO, "Pcie card%d, Vender id: %d, Device id: %d", pcie_index,
            pcie_disk_info.info[pcie_index].vender_id, pcie_disk_info.info[pcie_index].device_id);
    }

    
    ipmi_send_simple_response(msg_data, COMP_CODE_SUCCESS);

    
    for (pcie_index = 0; pcie_index < pcie_disk_info.count; pcie_index++) {
        old_boardid =
            g_pcie_disk_group.info[pcie_index].vender_id * 0x10000 + g_pcie_disk_group.info[pcie_index].device_id;
        new_boardid = pcie_disk_info.info[pcie_index].vender_id * 0x10000 + pcie_disk_info.info[pcie_index].device_id;
        
        old_auxid = g_pcie_disk_group_sub_info.sub_info[pcie_index].sub_vender_id * 0x10000 +
            g_pcie_disk_group_sub_info.sub_info[pcie_index].sub_device_id;

        
        
        if (old_boardid != new_boardid) {
            if (old_boardid != PCIE_NO_CARD) {
                debug_log(DLOG_INFO, "PCI-E PullOut Slot %d, VID=%04x DID=%04x", pcie_index,
                    g_pcie_disk_group.info[pcie_index].vender_id, g_pcie_disk_group.info[pcie_index].device_id);
                
                ret = pcie_card_get_connector_handle(pcie_index, &obj_handle, DISK_CONNECTOR);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail!", pcie_index);
                    continue;
                }

                (void)pcie_card_load_xml(obj_handle, old_boardid, old_auxid, PCIE_CARD_XML_UNLOAD, DISK_CONNECTOR);
                pcie_card_log_operation_log(DISK_CONNECTOR, pcie_index, g_pcie_disk_group.info[pcie_index].vender_id,
                    g_pcie_disk_group.info[pcie_index].device_id, PCIE_CARD_XML_UNLOAD);
            }
        }
    }

    vos_task_delay(3000);

    
    (void)dal_get_product_version_num(&product_num);
    if (product_num >= PRODUCT_VERSION_V5) {
        (void)vos_thread_sem4_p(g_set_pcie_disk_info_lock, 120 * 1000);
    } else {
        
        
        (void)vos_thread_sem4_p(g_set_pcie_disk_info_lock, 60 * 1000);
        
    }
    

    for (pcie_index = 0; pcie_index < pcie_disk_info.count; pcie_index++) {
        old_boardid =
            g_pcie_disk_group.info[pcie_index].vender_id * 0x10000 + g_pcie_disk_group.info[pcie_index].device_id;
        new_boardid = pcie_disk_info.info[pcie_index].vender_id * 0x10000 + pcie_disk_info.info[pcie_index].device_id;
        new_auxid = PCIE_INVALID_VID_DID;

        
        if (old_boardid != new_boardid) {
            if (new_boardid != PCIE_NO_CARD) {
                debug_log(DLOG_INFO, "PCI-E PlugIn Slot %d, VID=%04x DID=%04x", pcie_index,
                    pcie_disk_info.info[pcie_index].vender_id, pcie_disk_info.info[pcie_index].device_id);
                
                ret = pcie_card_get_connector_handle(pcie_index, &obj_handle, DISK_CONNECTOR);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail!", pcie_index);
                    continue;
                }

                
                g_pcie_disk_group.info[pcie_index].vender_id = pcie_disk_info.info[pcie_index].vender_id;
                g_pcie_disk_group.info[pcie_index].device_id = pcie_disk_info.info[pcie_index].device_id;
                
                (void)pcie_card_load_xml(obj_handle, new_boardid, new_auxid, PCIE_CARD_XML_LOAD, DISK_CONNECTOR);
                pcie_card_log_operation_log(DISK_CONNECTOR, pcie_index, pcie_disk_info.info[pcie_index].vender_id,
                    pcie_disk_info.info[pcie_index].device_id, PCIE_CARD_XML_LOAD);
            }
        }
    }

    (void)memcpy_s(&g_pcie_disk_group, sizeof(g_pcie_disk_group), &pcie_disk_info, sizeof(PCIE_CARD_GROUP_INFO_S));
    per_save((guint8 *)&g_pcie_disk_group);

    
    notice_pcie_info(&pcie_disk_info, CLASS_NAME_BIOS, METHOD_BIOS_NOTIFY_PCIE_INFO);

    return RET_OK;
}

LOCAL void _node_pcie_card_reload(OBJ_HANDLE node_pcie_summary_handle, NODE_PCIE_SUMMARY_INFO_S *info_base,
    NODE_PCIE_SUMMARY_INFO_S *info_compare, guint8 action)
{
    guint32 pcie_info_cnt;
    guint32 board_id_base = 0;
    guint32 board_id_compare = 0;
    guint32 aux_id_base = 0;
    guint32 aux_id_compare = 0;
    OBJ_HANDLE connector_handle = 0;
    guint32 i;
    NODE_PCIE_INFO_S *info;
    const NODE_PCIE_INFO_S *info_out = NULL;
    gint32 ret;

    pcie_info_cnt = info_base->pcie_info_cnt * 2 / sizeof(NODE_PCIE_INFO_S);
    info = (NODE_PCIE_INFO_S *)(void *)info_base->pcie_info_array;

    for (i = 0; i < pcie_info_cnt; i++) {
        ret = _find_node_pcie_card_info((const void *)info_compare->pcie_info_array,
            info_compare->pcie_info_cnt * sizeof(guint16), info[i].container_type, info[i].container_slot,
            info[i].pcie_slot, &info_out);

        board_id_base = info[i].vendor_id * 0x10000 + info[i].device_id;
        aux_id_base = info[i].sub_vendor_id * 0x10000 + info[i].sub_device_id;

        if ((ret == RET_OK) && (info_out != NULL)) {
            board_id_compare = info_out->vendor_id * 0x10000 + info_out->device_id;
            aux_id_compare = info_out->sub_vendor_id * 0x10000 + info_out->sub_device_id;
        }

        
        if ((ret == RET_ERR) || (board_id_base != board_id_compare) || (aux_id_base != aux_id_compare)) {
            ret = _find_node_pcie_card_connector(node_pcie_summary_handle, &info[i], &connector_handle);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR,
                    "cann't get connector handle, container type (%d), container slot (%d), pcie slot (%d)",
                    info[i].container_type, info[i].container_slot, info[i].pcie_slot);
                continue;
            }

            debug_log(DLOG_INFO, "%s xml :%#x_%#x", (PCIE_CARD_XML_UNLOAD == action) ? "Unload" : "Load", board_id_base,
                aux_id_base);
            ret = pcie_card_load_xml(connector_handle, board_id_base, aux_id_base, action, PCIE_AND_PCIEPLUS_CONNECTOR);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR,
                    "unload xml failed, container type:%d, container slot:%d, pcie slot:%d, board id:%#x, aux id:%#x",
                    info[i].container_type, info[i].container_slot, info[i].pcie_slot, board_id_base, aux_id_base);
            }

            if (ret == RET_OK) {
                pcie_card_log_operation_log(PCIE_AND_PCIEPLUS_CONNECTOR, info[i].pcie_slot, info[i].vendor_id,
                    info[i].device_id, action);
            }

            
            if (action == PCIE_CARD_XML_LOAD) {
                _update_pcie_connector_binded_data(connector_handle, &info[i]);
                
                vos_task_delay(SMM_PCIE_CARD_LOAD_INTERVAL_MS);
                
            }
        }
    }

    return;
}

gint32 parse_node_pcie_card_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    const guint16 *pcie_info_array = 0;
    gsize array_len;
    NODE_PCIE_SUMMARY_INFO_S *pcie_summary_info = NULL;
    errno_t ptr_ret = EOK;
    guint8 node_id = 0;

    
    gchar *print_str = NULL;

    pcie_info_array = (const guint16 *)g_variant_get_fixed_array(property_value, &array_len, sizeof(guint16));
    if (pcie_info_array == NULL) {
        debug_log(DLOG_ERROR, "%s:g_variant_get_fixed_array return NULL error.", __FUNCTION__);
        return RET_ERR;
    }
    pcie_summary_info = (NODE_PCIE_SUMMARY_INFO_S *)g_malloc0(sizeof(NODE_PCIE_SUMMARY_INFO_S));
    if (pcie_summary_info == NULL) {
        debug_log(DLOG_ERROR, "alloc new pcie summary info failed");
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(object_handle, PROPERTY_NODE_PCIEINFO_SUMMARY_ID, &node_id);

    if (array_len != 0) {
        ptr_ret = memmove_s(pcie_summary_info->pcie_info_array, MAX_PCIE_INFO_ARRAY_LEN, pcie_info_array,
            array_len * sizeof(guint16));
        if (ptr_ret != EOK) {
            debug_log(DLOG_ERROR, "copy pcie info array failed");
            g_free(pcie_summary_info);
            return RET_ERR;
        }
    }

    print_str = g_variant_print(property_value, TRUE);
    debug_log(DLOG_ERROR, "[node%d]size for data from property event is %" G_GSIZE_FORMAT ", data is %s", node_id,
        array_len, print_str);
    g_free(print_str);
    

    pcie_summary_info->pcie_info_cnt = array_len;

    (void)vos_thread_sem4_p(g_set_node_pcie_summary_lock, SEM_WAIT_FOREVER);

#ifdef _DEBUG
    if (node_id >= MAX_NODE_NUM) {
        debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, node_id);
        (void)vos_thread_sem4_v(g_set_node_pcie_summary_lock);
        g_free(pcie_summary_info);
        return RET_ERR;
    }
    size_t old_card_num =
        g_pcie_summary_info.pcie_summary_info[node_id].pcie_info_cnt * sizeof(guint16) / sizeof(NODE_PCIE_INFO_S);
    size_t new_card_num = pcie_summary_info->pcie_info_cnt * sizeof(guint16) / sizeof(NODE_PCIE_INFO_S);
    debug_log(DLOG_DEBUG, "old card num:%zu, new card num:%zu", old_card_num, new_card_num);
#endif

    
    _node_pcie_card_reload(object_handle, &g_pcie_summary_info.pcie_summary_info[node_id], pcie_summary_info,
        PCIE_CARD_XML_UNLOAD);

    
    vos_task_delay(3000);

    
    _node_pcie_card_reload(object_handle, pcie_summary_info, &g_pcie_summary_info.pcie_summary_info[node_id],
        PCIE_CARD_XML_LOAD);

    ptr_ret = memmove_s((void *)&g_pcie_summary_info.pcie_summary_info[node_id], sizeof(NODE_PCIE_SUMMARY_INFO_S),
        (const void *)pcie_summary_info, sizeof(NODE_PCIE_SUMMARY_INFO_S));

    g_free(pcie_summary_info);

    if (ptr_ret != EOK) {
        debug_log(DLOG_ERROR, "copy new node pcie summary info failed");
    }

    if (node_id > g_pcie_summary_info.node_count) {
        g_pcie_summary_info.node_count = node_id;
    }

    per_save((guint8 *)&g_pcie_summary_info);

    (void)vos_thread_sem4_v(g_set_node_pcie_summary_lock);

    return RET_OK;
}

gint32 pcie_card_load_xml_retry_enable(void)
{
    OBJ_HANDLE pro_handle = 0;
    static guint32 product_id = 0xFFFFFFFF;

    if (product_id == 0xFFFFFFFF) {
        (void)dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &pro_handle);
        (void)dal_get_property_value_uint32(pro_handle, BMC_PRODUCT_ID_NAME, &product_id);
    }

    if (PRODUCT_ID_HPC_SERVER == product_id) {
        
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL void pcie_card_load_common_vpd_xml(guint8 pcie_slot, guint8 state, guint8 current_state)
{
    OBJ_HANDLE conn_obj = 0;
    gint32 ret = 0;

    ret = pcie_card_get_vpd_common_connector(pcie_slot, &conn_obj);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get disk %d NVMe VPD Connector handle fail!", pcie_slot);
        return;
    }

    ret = pcie_card_load_xml(conn_obj, 0xff, 0xff, state, NVME_VPD_CONNECTOR);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Load common vpd xml failed!");
        return;
    }

    
    if (VPD_FIRST_TIME_LOAD != current_state) {
        pcie_card_vpd_operation_log(pcie_slot, state);
    }
}

gint32 pcie_card_present_status_switch(OBJ_HANDLE object_handle, guint8 pcie_present,
    PCIE_ACTION_PRIV_DATA_S *priv_data)
{
    gint32 result = 0;
    OBJ_HANDLE connector_obj_handle = 0;
    guint32 boardid = 0;
    guint32 auxid = 0;
    guint8 pcie_slot = 0;
    guint8 pcie_index = 0;
    guint8 bdf_index = 0;
    guint8 pcie_type = 0;
    guint8 cardact_type = 0;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_bdf_info = NULL;
    PCIE_CARD_GROUP_INFO_S *pcie_group_info = NULL;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info = NULL;

    
    result = pcie_get_cardtype_by_cardaction(object_handle, &pcie_type);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "Get pcie_type fail, result is %d", result);
        return RET_ERR;
    }

    result = dal_get_property_value_byte(object_handle, PROPERTY_CARD_ACTION_TYPE, &cardact_type);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "get %s.Type fail!!!", dfl_get_object_name(object_handle));
        return RET_ERR;
    }

    if (cardact_type == NVME_RAID_CONNECTOR) {
        return RET_OK;
    }

    
    result = dal_get_property_value_byte(object_handle, PROPERTY_CARD_ACTION_DATA, &pcie_slot);
    if (result != RET_OK) {
        debug_log(DLOG_ERROR, "GET AgentConnector Slot failed, result is %d", result);
        return result;
    }

    result = pcie_get_index_by_slot(pcie_type, pcie_slot, &bdf_index);
    if (result != RET_OK) {
        debug_log(DLOG_DEBUG, "Get pcie_index fail, result is %d", result);
        return RET_ERR;
    }

    
    if (pcie_type == DISK_CONNECTOR) {
        pcie_group_info = &g_pcie_disk_group;
        pcie_bdf_info = &g_pcie_disks_bdf_info;
        
        pcie_group_sub_info = &g_pcie_disk_group_sub_info;
        
        pcie_index = pcie_slot;
    } else if (pcie_type == OCPCARD_CONNECTOR) {
        pcie_group_info = &g_ocp_group;
        pcie_bdf_info = &g_ocp_bdf_info;
        pcie_group_sub_info = &g_ocp_group_sub_info;
        pcie_index = pcie_slot - 1;

        if (pcie_present == PCIE_PLUG_IN) {
            guint8 smbios_status = 0;
            guint8 ocp_bus = 0;
            guint8 ocp_dev = 0;
            guint8 ocp_fun = 0;
            result = get_smbios_status_value(&smbios_status);
            if (result != RET_OK) {
                debug_log(DLOG_ERROR, "Get smbios status failed, ret = %d", result);
                return result;
            }
            if (smbios_status != SMBIOS_WRITE_FINISH) {
                debug_log(DLOG_ERROR, "Do not need update because smbios status is %d", smbios_status);
                return RET_OK;
            }
            result = ocp_card_update_root_port_bdf(pcie_type, pcie_slot, &ocp_bus, &ocp_dev, &ocp_fun);
            if (result != RET_OK) {
                debug_log(DLOG_DEBUG, "Ocp update root port bdf faild, result = %d", result);
                return result;
            }
            pcie_bdf_info->bdf_info[bdf_index].bus_num = ocp_bus;
            pcie_bdf_info->bdf_info[bdf_index].device_num = ocp_dev;
            pcie_bdf_info->bdf_info[bdf_index].function_num = ocp_fun;
        }
    } else {
        pcie_group_info = &g_pcie_group;
        pcie_bdf_info = &g_pcie_cards_bdf_info;
        
        pcie_group_sub_info = &g_pcie_group_sub_info;
        

        
        if (pcie_slot < 1) {
            debug_log(DLOG_ERROR, "PCIe card slot info error. slot : %d;", pcie_slot);
            return RET_ERR;
        }
        pcie_index = pcie_slot - 1;
    }
    if (pcie_index >= MAX_PCIE_CARD) {
        debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, pcie_index);
        return RET_ERR;
    }

    
    if (pcie_present == PCIE_PLUG_OUT) {
        if ((pcie_card_get_vmd_state() == ENABLE) && (pcie_type == DISK_CONNECTOR)) {
            pcie_card_load_common_vpd_xml(pcie_slot, PCIE_CARD_XML_UNLOAD, priv_data->present);
            priv_data->present = pcie_present;
            priv_data->vpd_load = VPD_UNLOAD;
        } else {
            
            result = pcie_card_get_connector_handle(pcie_slot, &connector_obj_handle, pcie_type);
            if (result != RET_OK) {
                debug_log(DLOG_DEBUG, "[PlugOut]Get pcie card%d connector handle fail!", pcie_slot);
                return result;
            }

            boardid =
                pcie_group_info->info[pcie_index].vender_id * 0x10000 + pcie_group_info->info[pcie_index].device_id;
            
            auxid = pcie_group_sub_info->sub_info[pcie_index].sub_vender_id * 0x10000 +
                pcie_group_sub_info->sub_info[pcie_index].sub_device_id;
            

            
            result = pcie_card_load_xml(connector_obj_handle, boardid, auxid, PCIE_CARD_XML_UNLOAD, pcie_type);
            if (result != RET_OK) {
                debug_log(DLOG_DEBUG, "Unload pcie card%d(%x%x) failed!", pcie_slot, boardid, auxid);
                return result;
            }
            pcie_card_log_operation_log(pcie_type, pcie_slot, pcie_group_info->info[pcie_index].vender_id,
                pcie_group_info->info[pcie_index].device_id, PCIE_PLUG_OUT);
            
            pcie_group_info->info[pcie_index].vender_id = PCIE_INVALID_VID;
            pcie_group_info->info[pcie_index].device_id = PCIE_INVALID_DID;
            
            pcie_group_sub_info->sub_info[pcie_index].sub_vender_id = PCIE_INVALID_SUBVID;
            pcie_group_sub_info->sub_info[pcie_index].sub_device_id = PCIE_INVALID_SUBDID;
            
            per_save((guint8 *)pcie_group_info);
            per_save((guint8 *)pcie_group_sub_info);
            priv_data->present = pcie_present;
        }
    } else {
        // 支持VMD热插拔
        if ((pcie_card_get_vmd_state() == ENABLE) && (pcie_type == DISK_CONNECTOR)) {
            pcie_card_load_common_vpd_xml(pcie_slot, PCIE_CARD_XML_LOAD, priv_data->present);
            priv_data->present = pcie_present;
        } else {
            if (bdf_index >= MAX_PCIE_CARD) {
                debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, bdf_index);
                return RET_ERR;
            }
            
            debug_log(DLOG_DEBUG, "BDF bus_num:%d device_num:%d function_num:%d",
                pcie_bdf_info->bdf_info[bdf_index].bus_num, pcie_bdf_info->bdf_info[bdf_index].device_num,
                pcie_bdf_info->bdf_info[bdf_index].function_num);

            result =
                pcie_get_did_vid_from_me(bdf_index, pcie_index, pcie_group_info, pcie_bdf_info, pcie_group_sub_info);
            if (result != RET_OK) {
                debug_log(DLOG_DEBUG, "Get pcie card%d vid/did from ME failed.", pcie_slot);
                return RET_ERR;
            }

            
            result = pcie_card_get_connector_handle(pcie_slot, &connector_obj_handle, pcie_type);
            if (result != RET_OK) {
                debug_log(DLOG_DEBUG, "[PlugIn]Get pcie card%d connector handle fail!", pcie_slot);
                return result;
            }

            boardid =
                pcie_group_info->info[pcie_index].vender_id * 0x10000 + pcie_group_info->info[pcie_index].device_id;
            
            auxid = pcie_group_sub_info->sub_info[pcie_index].sub_vender_id * 0x10000 +
                pcie_group_sub_info->sub_info[pcie_index].sub_device_id;
            

            if (boardid != PCIE_NO_CARD && boardid != 0) {
                per_save((guint8 *)pcie_group_info);
                per_save((guint8 *)pcie_group_sub_info);

                
                result = pcie_card_load_xml(connector_obj_handle, boardid, auxid, PCIE_CARD_XML_LOAD, pcie_type);
                if (result != RET_OK) {
                    debug_log(DLOG_DEBUG, "Load pcie card%d(%x%x) failed!", pcie_slot, boardid, auxid);
                    return result;
                }
                pcie_card_log_operation_log(pcie_type, pcie_slot, pcie_group_info->info[pcie_index].vender_id,
                    pcie_group_info->info[pcie_index].device_id, PCIE_PLUG_IN);
                
                priv_data->present = pcie_present;
            }
        }
    }

    
    return RET_OK;
}

gint32 pcie_card_load_vpd_xml_via_protocol(OBJ_HANDLE handle, guint8 *load_status)
{
    gint32 ret = 0;
    guint8 pcie_slot = 0;
    OBJ_HANDLE conn_handle = 0;
    guint8 vpd_type = 0xff;

    if ((handle == 0) || (load_status == NULL)) {
        debug_log(DLOG_ERROR, "input param error");
        return RET_ERR;
    }

    
    ret = dal_get_property_value_byte(handle, PROPERTY_CARD_ACTION_DATA, &pcie_slot);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "GET AgentConnector Slot failed, result is %d", ret);
        return ret;
    }

    
    ret = pcie_card_get_vpd_protocol_connector(pcie_slot, &conn_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Get VPD protocol connector failed, %d.", ret);
        return ret;
    }

    
    ret = get_nvme_protocol(conn_handle, &vpd_type);
    if (ret != RET_OK || vpd_type == NVME_VPD_PROTOCOL_UNKNOWN) {
        debug_log(DLOG_DEBUG, "VPD type verify failed, %d.", vpd_type);
        return RET_ERR;
    }
    

    debug_log(DLOG_DEBUG, "ssd slot%d detect the type:%d", pcie_slot, vpd_type);
    ret = pcie_card_load_xml(conn_handle, (guint32)vpd_type, 0xff, PCIE_CARD_XML_LOAD, NVME_VPD_CONNECTOR);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Load VPD%d(%x) failed!", pcie_slot, vpd_type);
        return ret;
    }

    *load_status = VPD_LOAD;

    return RET_OK;
}




gint32 pcie_device_mock(GSList *input_list)
{
    OBJ_HANDLE connector_obj = 0;
    const gchar *connector_name = NULL;
    guint32 pcie_type = DISK_CONNECTOR;
    volatile guint32 vid_did = 0xFFFFFFFF;
    volatile guint32 aux_id = 0xFFFFFFFF;
    volatile guint32 old_vid_did = 0xFFFFFFFF;
    volatile guint32 old_aux_id = 0xFFFFFFFF;
    guint32 input_len = 0;
    guint8 slot_id = 0;
    guint8 pcie_index = 0;
    PCIE_CARD_GROUP_INFO_S *pcie_group_info = NULL;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info = NULL;

    if (input_list == NULL) {
        debug_printf("param is null!");
        return RET_ERR;
    }

    input_len = g_slist_length(input_list);
    if (input_len != 4) {
        debug_printf("usage: pcie_device_mock [connector_name] [pcie_type(1 card; 2 ssd)] [vid_did] [aux_id]\r\n");
        return RET_ERR;
    }

    connector_name = g_variant_get_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    pcie_type = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 1));
    vid_did = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 2));
    aux_id = g_variant_get_uint32((GVariant *)g_slist_nth_data(input_list, 3));

    debug_printf("connector_name=%s, pcie_type=0x%08X, vid_did=0x%08X, aux_id=0x%08X", connector_name, pcie_type,
        vid_did, aux_id);

    // 根据connector 获取 slot id
    if (DFL_OK != dfl_get_object_handle(connector_name, &connector_obj)) {
        debug_printf("invalid connector_name(%s)", connector_name);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(connector_obj, PROPERTY_CONNECTOR_SLOT, &slot_id);

    if (DISK_CONNECTOR == pcie_type) {
        pcie_group_info = &g_pcie_disk_group;
        pcie_group_sub_info = &g_pcie_disk_group_sub_info;
        pcie_index = slot_id;
    } else {
        pcie_group_info = &g_pcie_group;
        pcie_group_sub_info = &g_pcie_group_sub_info;

        
        if (slot_id < 1) {
            debug_printf("invalid connector_name(%s), slot_id=%d", connector_name, slot_id);
            return RET_ERR;
        }

        pcie_index = slot_id - 1;
    }
    if (pcie_index >= MAX_PCIE_CARD) {
        debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, pcie_index);
        return RET_ERR;
    }
    old_vid_did = pcie_group_info->info[pcie_index].vender_id * 0x10000 + pcie_group_info->info[pcie_index].device_id;
    old_aux_id = pcie_group_sub_info->sub_info[pcie_index].sub_vender_id * 0x10000 +
        pcie_group_sub_info->sub_info[pcie_index].sub_device_id;

    debug_printf("old_vid_did=0x%08X, old_aux_id=0x%08X", old_vid_did, old_aux_id);

    // 如果用户输入的是0xFFFFFFFF, 表示卸载卡
    if (PCIE_NO_CARD == vid_did) {
        (void)pcie_card_load_xml(connector_obj, old_vid_did, old_aux_id, PCIE_CARD_XML_UNLOAD, pcie_type);
        debug_printf("unload pcie device successfully");
        return RET_OK;
    }

    // 先卸载卡，再加载卡
    if ((old_vid_did != vid_did) || (old_aux_id != aux_id)) {
        if (PCIE_NO_CARD != old_vid_did) {
            (void)pcie_card_load_xml(connector_obj, old_vid_did, old_aux_id, PCIE_CARD_XML_UNLOAD, pcie_type);
            debug_printf("unload pcie device(0x%08X) successfully", old_vid_did);
            vos_task_delay(3000);
        }

        pcie_group_info->info[pcie_index].vender_id = (guint16)((vid_did >> 16) & 0xFFFF);
        pcie_group_info->info[pcie_index].device_id = (guint16)((vid_did)&0xFFFF);
        pcie_group_sub_info->sub_info[pcie_index].sub_vender_id = (guint16)((aux_id >> 16) & 0xFFFF);
        pcie_group_sub_info->sub_info[pcie_index].sub_device_id = (guint16)((aux_id)&0xFFFF);

        (void)pcie_card_load_xml(connector_obj, vid_did, aux_id, PCIE_CARD_XML_LOAD, pcie_type);
        debug_printf("load pcie device(0x%08X) successfully", vid_did);
    }

    return RET_OK;
}


gint32 pcie_card_get_reload_enable(guint8 slot_id, guint8 *pciecard_reload)
{
    OBJ_HANDLE pciecard_obj_handle = 0;
    gint32 ret = 0;

    if (pciecard_reload == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_PCIECARD_NAME, PROPETRY_PCIECARD_SLOT, slot_id, &pciecard_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s  PcieCard dal_get_specific_object_byte slot %d error", __FUNCTION__, slot_id);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(pciecard_obj_handle, PROPERTY_PCIECARD_RELOAD_XML, pciecard_reload);
    debug_log(DLOG_DEBUG, "get slot%d PcieCard  %s reload state %d ", slot_id, dfl_get_object_name(pciecard_obj_handle),
        *pciecard_reload);
    return ret;
}

gint32 pcie_card_load(void)

{
    
    gint32 ret = 0;
    ret = pcie_card_per_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Persistance init pcie cards info fail! ret=%d", ret);
        return RET_ERR;
    }

    
    ret = pcie_card_load_per_card();
    debug_log(DLOG_ERROR, "g_ocp_port_rootbdf_info count=%u,g_pcie_port_rootbdf_info count=%u",
        g_ocp_port_rootbdf_info.count, g_pcie_port_rootbdf_info.count);
    update_ocp_netcard_rootbdf(&g_ocp_port_rootbdf_info);
    update_pcie_netcard_rootbdf(&g_pcie_port_rootbdf_info);
    pcie_port_update_bdf(COMPONENT_TYPE_PCIE_CARD, &g_pcie_port_rootbdf_info);
    pcie_port_update_bdf(COMPONENT_TYPE_OCP, &g_ocp_port_rootbdf_info);
    update_pcie_raid_rootbdf(&g_pcie_raid_rootbdf_info);
    pcie_card_update_ft_resid(&g_pcie_cards_bdf_info, &g_ocp_bdf_info);
    return ret;
}

LOCAL gint32 get_card_name_from_buffer(gchar *card_info_buf, gchar card_name_buf[], guint32 size)
{
    errno_t safe_fun_ret = EOK;
    gint32 len = 0;

    if (card_info_buf == NULL || card_name_buf == NULL) {
        debug_log(DLOG_ERROR, "Input Buffer is NULL");
        return RET_ERR;
    }

    
    PCIE_SKIP_XDIGITS(card_info_buf);
    PCIE_SKIP_BLANKS(card_info_buf);

    
    safe_fun_ret = strncpy_s(card_name_buf, size, card_info_buf, size - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    card_name_buf[size - 1] = '\0';

    len = strlen(card_name_buf);
    if (card_name_buf[len - 1] == '\n') {
        card_name_buf[len - 1] = '\0';
    }

    debug_log(DLOG_DEBUG, "Get buffer is [%s].", card_name_buf);

    return RET_OK;
}

LOCAL gint32 pcie_card_lookup_vender_name(FILE *fp, guint16 vender_id, gchar buf[], gint32 size)
{
    gint32 ret = 0;
    guint32 vender_id_read = 0;
    gchar line[MAX_LINE_SIZE] = {0};
    gchar *pchar = NULL;

    if ((fp == NULL) || (NULL == buf)) {
        debug_log(DLOG_ERROR, "Input buffer is NULL!");
        return RET_ERR;
    }

    while (pcie_card_get_one_valid_line(fp, line, sizeof(line)) == 0) {
        pchar = line;

        

        if (strncmp("C ", pchar, (guint32)2) == 0) {
            
            debug_log(DLOG_ERROR, "Can't find device name!");
            break;
        }

        if (*(pchar) == '\t') {
            
            continue;
        }

        ret = sscanf_s(pchar, "%x", &vender_id_read);
        if (ret != 1) {
            continue;
        }

        if (vender_id_read != vender_id) {
            continue;
        }

        debug_log(DLOG_DEBUG, "Find vender [0x%04x].", vender_id_read);

        
        ret = get_card_name_from_buffer(pchar, buf, size);
        return ret;
    }

    return RET_ERR;
}

LOCAL gint32 pcie_card_lookup_device_name(FILE *fp, guint16 device_id, gchar buf[], gint32 size)
{
    gint32 ret = 0;
    guint32 device_id_read = 0;
    gchar line[MAX_LINE_SIZE] = {0};
    gchar *pchar = NULL;

    
    if (fp == NULL || buf == NULL) {
        debug_log(DLOG_ERROR, "Input buffer is NULL!. ");
        return RET_ERR;
    }
    

    while (pcie_card_get_one_valid_line(fp, line, sizeof(line)) == 0) {
        pchar = line;

        

        if (strncmp("C ", pchar, (guint32)2) == 0) {
            
            debug_log(DLOG_ERROR, "Can't find device name!");
            break;
        }

        if (*(pchar) != '\t') {
            
            debug_log(DLOG_DEBUG, "Can't find device [0x%04x] !", device_id);
            break;
        }

        pchar++;

        if (*(pchar) == '\t') {
            
            continue;
        }

        ret = sscanf_s(pchar, "%x", &device_id_read);
        if (ret != 1) {
            continue;
        }

        if (device_id_read != device_id) {
            continue;
        }

        debug_log(DLOG_DEBUG, "Find device [0x%04x].", device_id_read);

        
        ret = get_card_name_from_buffer(pchar, buf, size);
        return ret;
    }

    return RET_ERR;
}


LOCAL gint32 pcie_card_lookup_sub_device_name(FILE *fp, guint16 sub_vender_id, guint16 sub_device_id, gchar buf[],
    gint32 size)
{
    gint32 ret = 0;
    guint32 sub_vid_read = 0;
    guint32 sub_did_read = 0;

    gchar line[MAX_LINE_SIZE] = {0};
    gchar *pchar = NULL;

    if (fp == NULL || buf == NULL) {
        debug_log(DLOG_ERROR, "Input buffer is NULL!");
        return RET_ERR;
    }

    while (pcie_card_get_one_valid_line(fp, line, sizeof(line)) == 0) {
        pchar = line;

        

        if (strncmp("C ", pchar, (guint32)2) == 0) {
            
            debug_log(DLOG_ERROR, "Can't find device name!");
            break;
        }

        
        if (*(pchar) != '\t') {
            
            debug_log(DLOG_DEBUG, "Can't find subdevice [0x%04x%04x] !", sub_vender_id, sub_device_id);
            break;
        }
        pchar++;

        if (*(pchar) != '\t') {
            
            debug_log(DLOG_DEBUG, "Can't find subdevice [0x%04x%04x] !", sub_vender_id, sub_device_id);
            break;
        }
        pchar++;
        ret = sscanf_s(pchar, "%x", &sub_vid_read);
        if ((ret != 1) || (sub_vid_read != sub_vender_id)) {
            continue;
        }

        
        PCIE_SKIP_XDIGITS(pchar);
        PCIE_SKIP_BLANKS(pchar);
        ret = sscanf_s(pchar, "%x", &sub_did_read);
        if ((ret != 1) || (sub_did_read != sub_device_id)) {
            continue;
        }

        debug_log(DLOG_DEBUG, "Find sub_vid_did_read [0x%04x%04x].", sub_vid_read, sub_did_read);

        
        ret = get_card_name_from_buffer(pchar, buf, size);
        return ret;
    }

    return RET_ERR;
}
gint32 pcie_card_lookup_name(guint16 vender_id, guint16 device_id, guint16 sub_vender_id, guint16 sub_device_id,
    gchar vender[], gint32 len_vender, gchar device[], gint32 len_device)
{
    gint32 ret = 0;

    FILE *fp = NULL;

    if ((vender == NULL) || (NULL == device)) {
        debug_log(DLOG_ERROR, "Input buffer is NULL!");
        return RET_ERR;
    }

    fp = g_fopen(PCIE_IDS_FILE, "rb");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "Open [%s] failed!", PCIE_IDS_FILE);
        return RET_ERR;
    }

    ret = pcie_card_lookup_vender_name(fp, vender_id, vender, len_vender);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Lookup vender name for [0x%04x] failed!", vender_id);
        (void)fclose(fp);
        return RET_ERR;
    }

    ret = pcie_card_lookup_device_name(fp, device_id, device, len_device);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Lookup device name for [0x%04x] failed!", device_id);
        (void)fclose(fp);
        return RET_ERR;
    }

    
    ret = pcie_card_lookup_sub_device_name(fp, sub_vender_id, sub_device_id, device, len_device);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "Lookup subdevice name for [0x%04x%04x_%04x%04x] failed!", vender_id, device_id,
            sub_vender_id, sub_device_id);
    } else {
        debug_log(DLOG_DEBUG, "Lookup subdevice name for [0x%04x%04x_%04x%04x] is %s success!", vender_id, device_id,
            sub_vender_id, sub_device_id, device);
    }

    (void)fclose(fp);

    return RET_OK;
}


LOCAL gint32 get_port_handle_by_card_info(OBJ_HANDLE card_handle, guint8 type, guint8 port_idx, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    OBJ_HANDLE port_handle;
    OBJ_HANDLE comp_handle = 0;
    GSList *port_list = NULL;
    GSList *port_node = NULL;
    guint8 port_num, comp_type;
    ret = dfl_get_referenced_object(card_handle, PROPERTY_NETCARD_REF_COMPONENT, &comp_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Failed to dfl get PROPERTY_NETCARD_REF_COMPONENT object ！", __FUNCTION__);
        return RET_ERR;
    }
    ret = dal_get_property_value_byte(comp_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_type);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Failed to dfl get PROPERTY_COMPONENT_DEVICE_TYPE object ！", __FUNCTION__);
        return RET_ERR;
    }
    debug_log(DLOG_INFO, "%s:card %s type:%u,input type:%u", __FUNCTION__,
        dfl_get_object_name(card_handle), comp_type, type);
    if (comp_type == type) {
        ret = dal_get_object_list_position(card_handle, ETH_CLASS_NAME_BUSY_ETH, &port_list);
        if (ret != RET_OK || port_list == NULL) {
            debug_log(DLOG_ERROR, "%s:get %s with position %u failed", __FUNCTION__, ETH_CLASS_NAME_BUSY_ETH,
                dfl_get_position(card_handle));
            return RET_ERR;
        }
        for (port_node = port_list; port_node; port_node = g_slist_next(port_node)) {
            port_handle = (OBJ_HANDLE)port_node->data;
            (void)dal_get_property_value_byte(port_handle, BUSY_ETH_ATTRIBUTE_PORT_NUM, &port_num);
            if (port_idx == port_num) {
                *obj_handle = port_handle;
                g_slist_free(port_list);
                return RET_OK;
            }
        }
        g_slist_free(port_list);
    }
    return RETURN_CONTINUE;
}
typedef struct port_handle_by_idx {
    guint8 slot_id;
    guint8 port_idx;
    guint8 type;
    OBJ_HANDLE *obj_handle;
} PORT_HANDLE_BY_IDX;

LOCAL gint32 get_port_handle_by_idx(guint8 slot_id, guint8 port_idx, guint8 type, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    GSList *netcard_list = NULL;
    GSList *netcard_node = NULL;
    OBJ_HANDLE card_handle;
    guint8 card_slot;
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &netcard_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Failed to get obj list of CLASS_NETCARD_NAME", __FUNCTION__);
        return RET_ERR;
    }
    for (netcard_node = netcard_list; netcard_node; netcard_node = netcard_node->next) {
        card_handle = (OBJ_HANDLE)netcard_node->data;
        ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_SLOT_ID, &card_slot);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get card %s %s failed", __FUNCTION__,
                dfl_get_object_name(card_handle), PROPERTY_NETCARD_SLOT_ID);
            g_slist_free(netcard_list);
            return RET_ERR;
        }
        debug_log(DLOG_INFO, "%s:card %s slot:%u,input slot:%u,idx:%u", __FUNCTION__,
            dfl_get_object_name(card_handle), card_slot, slot_id, port_idx);
        if (slot_id == (card_slot - 1)) {
            ret = get_port_handle_by_card_info(card_handle, type, port_idx, obj_handle);
            if (ret == RETURN_CONTINUE) {
                continue;
            }
            g_slist_free(netcard_list);
            return ret;
        }
    }
    debug_log(DLOG_ERROR, "%s:get port handle failed,slot:%u,idx:%u,type:%u", __FUNCTION__, slot_id, port_idx, type);
    g_slist_free(netcard_list);
    return RET_ERR;
}
 

LOCAL gint32 get_port_handle_by_idinfo(guint8 slot_id, guint8 port_idx, guint8 type, OBJ_HANDLE *obj_handle,
    PORT_GROUP_BDF_INFO_S *port_bdf_info)
{
    guint8 i;
    guint8 port_idx_cur = 0;
    for (i = 0; i < port_bdf_info->count; i++) {
        if (slot_id != port_bdf_info->bdf_info[i].slot_id) {
            continue;
        }
        if (i == port_idx) {
            return get_port_handle_by_idx(slot_id, port_idx_cur, type, obj_handle);
        }
        port_idx_cur++;
    }
    return RET_ERR;
}
 

LOCAL void pcie_port_update_bdf(guint8 type, PORT_GROUP_BDF_INFO_S *port_bdf_info)
{
    gint32 ret;
    guint8 slot_id, i;
    OBJ_HANDLE obj_handle = 0;
    gchar bdf_str[BDF_STR_LEN + 1] = {0}; // bdf str 长度固定为12
    GSList *input_list = NULL;
 
    for (i = 0; i < port_bdf_info->count; i++) {
        slot_id = port_bdf_info->bdf_info[i].slot_id;
        if (slot_id < 0 || MAX_PCIE_CARD - 1 < slot_id) {
            debug_log(DLOG_ERROR, "%s incorrect PcieCard slot id:%d", __FUNCTION__, slot_id);
            return;
        }
        debug_log(DLOG_INFO, "%s get pciecard%d port handle count %d", __FUNCTION__, slot_id, i);
        ret = get_port_handle_by_idinfo(slot_id, i, type, &obj_handle, port_bdf_info);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s Get pcie port object failed", __FUNCTION__);
            return;
        }
        ret = sprintf_s(bdf_str, sizeof(bdf_str), "%04x:%02x:%02x.%x", port_bdf_info->bdf_info[i].segment,
            port_bdf_info->bdf_info[i].bus_num, port_bdf_info->bdf_info[i].device_num,
            port_bdf_info->bdf_info[i].function_num);
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret = %d", __FUNCTION__, ret);
            return;
        }
        input_list = g_slist_append(input_list, g_variant_new_string(bdf_str));
        ret = dfl_call_class_method(obj_handle, BUSY_ETH_METHOD_SET_ROOTBDF, NULL, input_list, NULL);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "Set %s ROOTBDF failed, ret = %d.", dfl_get_object_name(obj_handle), ret);
            continue;
        }
        debug_log(DLOG_INFO, "%s Update netcard%d port rootbdf %s success,type:%u", __FUNCTION__,
            slot_id + 1, bdf_str, type);
    }
    if (type == COMPONENT_TYPE_PCIE_CARD) {
        update_rootbdf_info(TYPE_PCIE_CARD, *port_bdf_info);
    } else if (type == COMPONENT_TYPE_OCP) {
        update_rootbdf_info(TYPE_OCP, *port_bdf_info);
    }
    return;
}
 

LOCAL void rootport_bdf_parse(MIX_PORT_GROUP_BDF_INFO_S *mix_port_bdf_info,
    PORT_GROUP_BDF_INFO_S *pcie_port_bdf, PORT_GROUP_BDF_INFO_S *ocp_port_bdf, PORT_GROUP_BDF_INFO_S *pcie_raid_bdf)
{
#define PCIE_TYPE_RAID   2
#define OCP_TYPE_PORT    1
#define PCIE_TYPE_PORT   0
    guint32 i;
    guint32 ocp_cnt = 0;
    guint32 pcie_cnt = 0;
    guint32 raid_cnt = 0;
    if (mix_port_bdf_info == NULL || pcie_port_bdf == NULL || ocp_port_bdf == NULL || pcie_raid_bdf == NULL) {
        return;
    }
    for (i = 0; i < mix_port_bdf_info->count; i++) {
        if (mix_port_bdf_info->bdf_info[i].type == PCIE_TYPE_PORT) {
            pcie_port_bdf->count++;
            pcie_port_bdf->bdf_info[pcie_cnt].segment = mix_port_bdf_info->bdf_info[i].segment;
            pcie_port_bdf->bdf_info[pcie_cnt].slot_id = mix_port_bdf_info->bdf_info[i].slot_id;
            pcie_port_bdf->bdf_info[pcie_cnt].bus_num = mix_port_bdf_info->bdf_info[i].bus_num;
            pcie_port_bdf->bdf_info[pcie_cnt].device_num = mix_port_bdf_info->bdf_info[i].device_num;
            pcie_port_bdf->bdf_info[pcie_cnt].function_num = mix_port_bdf_info->bdf_info[i].function_num;
            pcie_cnt++;
        }
        if (mix_port_bdf_info->bdf_info[i].type == OCP_TYPE_PORT) {
            ocp_port_bdf->count++;
            ocp_port_bdf->bdf_info[ocp_cnt].segment = mix_port_bdf_info->bdf_info[i].segment;
            ocp_port_bdf->bdf_info[ocp_cnt].slot_id = mix_port_bdf_info->bdf_info[i].slot_id;
            ocp_port_bdf->bdf_info[ocp_cnt].bus_num = mix_port_bdf_info->bdf_info[i].bus_num;
            ocp_port_bdf->bdf_info[ocp_cnt].device_num = mix_port_bdf_info->bdf_info[i].device_num;
            ocp_port_bdf->bdf_info[ocp_cnt].function_num = mix_port_bdf_info->bdf_info[i].function_num;
            ocp_cnt++;
        }
        if (mix_port_bdf_info->bdf_info[i].type == PCIE_TYPE_RAID) {
            pcie_raid_bdf->count++;
            pcie_raid_bdf->bdf_info[raid_cnt].segment = mix_port_bdf_info->bdf_info[i].segment;
            pcie_raid_bdf->bdf_info[raid_cnt].slot_id = mix_port_bdf_info->bdf_info[i].slot_id;
            pcie_raid_bdf->bdf_info[raid_cnt].bus_num = mix_port_bdf_info->bdf_info[i].bus_num;
            pcie_raid_bdf->bdf_info[raid_cnt].device_num = mix_port_bdf_info->bdf_info[i].device_num;
            pcie_raid_bdf->bdf_info[raid_cnt].function_num = mix_port_bdf_info->bdf_info[i].function_num;
            raid_cnt++;
        }
    }
    return;
}
 

LOCAL gint32 pcie_port_bdf_parse(GVariant *property_value, MIX_PORT_GROUP_BDF_INFO_S *mix_port_bdf_info,
    PORT_GROUP_BDF_INFO_S *pcie_port_bdf, PORT_GROUP_BDF_INFO_S *ocp_port_bdf, PORT_GROUP_BDF_INFO_S *pcie_raid_bdf)
{
#define SINGLE_ROOTBDF_GROUP_LEN   6
    gsize bdf_data_len;
    const guint8 *bdf_data = NULL;
    guint32 i;
 
    bdf_data = (const guint8 *)g_variant_get_fixed_array(property_value, &bdf_data_len, sizeof(guint8));
    if (bdf_data == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL bdf data", __FUNCTION__);
        return RET_ERR;
    }
 
    // 由于OS启动BMC清空BDF场景，不执行信号量的V操作，使用特定返回码说明
    if (bdf_data_len == 1 && bdf_data[0] == 0) {
        debug_log(DLOG_DEBUG, "%s: bdf data reset for os reset", __FUNCTION__);
        return VOS_ERR_NOTSUPPORT;
    }
 
    
    if (bdf_data_len < SINGLE_ROOTBDF_GROUP_LEN) {
        debug_log(DLOG_ERROR, "bdf_data or bdf_data_len: %" G_GSIZE_FORMAT " failed!", bdf_data_len);
        return RET_ERR;
    }
    
    debug_log(DLOG_INFO, "pcie_card_parse_bdf_info, bdf_data_len=%" G_GSIZE_FORMAT, bdf_data_len);
    if (bdf_data_len / SINGLE_ROOTBDF_GROUP_LEN > MAX_PCIE_CARD) {
        mix_port_bdf_info->count = MAX_PCIE_CARD;
    } else {
        mix_port_bdf_info->count = bdf_data_len / SINGLE_ROOTBDF_GROUP_LEN;
    }
    for (i = 0; i < mix_port_bdf_info->count; i++) {
        mix_port_bdf_info->bdf_info[i].type = bdf_data[SINGLE_ROOTBDF_GROUP_LEN * i];
        mix_port_bdf_info->bdf_info[i].segment = bdf_data[SINGLE_ROOTBDF_GROUP_LEN * i + 1];
        mix_port_bdf_info->bdf_info[i].slot_id = bdf_data[SINGLE_ROOTBDF_GROUP_LEN * i + 2]; // slotid偏移2
        mix_port_bdf_info->bdf_info[i].bus_num = bdf_data[SINGLE_ROOTBDF_GROUP_LEN * i + 3]; // bus偏移3
        mix_port_bdf_info->bdf_info[i].device_num = bdf_data[SINGLE_ROOTBDF_GROUP_LEN * i + 4]; // device偏移4
        mix_port_bdf_info->bdf_info[i].function_num = bdf_data[SINGLE_ROOTBDF_GROUP_LEN * i + 5]; // func偏移5
        debug_log(DLOG_ERROR, "NetCard%d, type:0x%x bus:0x%x dev:0x%x func:0x%x",
            mix_port_bdf_info->bdf_info[i].slot_id, mix_port_bdf_info->bdf_info[i].type,
            mix_port_bdf_info->bdf_info[i].bus_num, mix_port_bdf_info->bdf_info[i].device_num,
            mix_port_bdf_info->bdf_info[i].function_num);
    }
    rootport_bdf_parse(mix_port_bdf_info, pcie_port_bdf, ocp_port_bdf, pcie_raid_bdf);
    return RET_OK;
}

LOCAL gint32 get_netcard_handle_by_portinfo(guint8 slot_id, guint8 type, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    GSList *netcard_node = NULL;
    OBJ_HANDLE card_handle;
    OBJ_HANDLE comp_handle = 0;
    GSList *netcard_list = NULL;
    guint8 card_slot;
    guint8 comp_type;
 
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &netcard_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Failed to get obj list of CLASS_NETCARD_NAME", __FUNCTION__);
        return RET_ERR;
    }
    for (netcard_node = netcard_list; netcard_node; netcard_node = netcard_node->next) {
        card_handle = (OBJ_HANDLE)netcard_node->data;
        ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_SLOT_ID, &card_slot);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get card %s %s failed", __FUNCTION__,
                dfl_get_object_name(card_handle), PROPERTY_NETCARD_SLOT_ID);
            g_slist_free(netcard_list);
            return RET_ERR;
        }
        debug_log(DLOG_DEBUG, "%s:slot:%u,input type:%u,slot:%u", __FUNCTION__, card_slot, type, slot_id);
        if (slot_id == card_slot) {
            ret = dfl_get_referenced_object(card_handle, PROPERTY_NETCARD_REF_COMPONENT, &comp_handle);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "%s:%s get refer obj failed", __FUNCTION__, dfl_get_object_name(card_handle));
                g_slist_free(netcard_list);
                return RET_ERR;
            }
            ret = dal_get_property_value_byte(comp_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_type);
            if (ret != DFL_OK) {
                debug_log(DLOG_ERROR, "%s: get compent device type failed", __FUNCTION__);
                g_slist_free(netcard_list);
                return RET_ERR;
            }
            debug_log(DLOG_DEBUG, "%s:comp type:%u,input type:%u,slot:%u", __FUNCTION__, comp_type, type, slot_id);
            if (comp_type == type) {
                *obj_handle = card_handle;
                g_slist_free(netcard_list);
                return RET_OK;
            }
        }
    }
    debug_log(DLOG_ERROR, "%s:get netcard%u handle failed,type:%u", __FUNCTION__, slot_id, type);
    g_slist_free(netcard_list);
    return RET_ERR;
}
 

LOCAL gint32 set_netcard_id_update_flag(guint8 slot_id, guint8 *set_list, guint8 list_size)
{
    if (slot_id >= list_size) {
        return RET_ERR;
    }
    set_list[slot_id] = slot_id;
    return RET_OK;
}
 

LOCAL void update_single_netcard_rootbdf(PORT_GROUP_BDF_INFO_S *pcie_port_bdf_info,
    PORT_INFO_S *port_info, guint8 *set_list, guint8 list_size)
{
    gint32 ret;
    gint32 i;
    OBJ_HANDLE obj_handle = 0;
    gchar bdf_str[BDF_STR_LEN + 1] = {0}; // bdf str 长度固定为12
    GSList *input_list = NULL;
    for (i = 0; i < MAX_RETRY_TIMES; i++) {
        ret = get_netcard_handle_by_portinfo(port_info->slot_id, port_info->type, &obj_handle);
        if (ret != RET_OK) {
            vos_task_delay(1000); // 每次重试间隔1000ms
            debug_log(DLOG_INFO, "%s Get netcard%u object failed,type:%u", __FUNCTION__,
                port_info->slot_id, port_info->type);
        } else {
            break;
        }
    }
    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s Get netcard object failed", __FUNCTION__);
        return;
    }
    ret = sprintf_s(bdf_str, sizeof(bdf_str), "%04x:%02x:%02x.%x",
        pcie_port_bdf_info->bdf_info[port_info->port_idx].segment,
        pcie_port_bdf_info->bdf_info[port_info->port_idx].bus_num,
        pcie_port_bdf_info->bdf_info[port_info->port_idx].device_num,
        pcie_port_bdf_info->bdf_info[port_info->port_idx].function_num);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s fail, ret = %d", __FUNCTION__, ret);
        return;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(bdf_str));
    ret = dfl_call_class_method(obj_handle, METHOD_NETCARD_SET_ROOTBDF, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Set %s ROOTBDF failed, ret = %d.", dfl_get_object_name(obj_handle), ret);
        return;
    }
    debug_log(DLOG_ERROR, "%s Update netcard%d rootbdf %s success", __FUNCTION__, port_info->slot_id, bdf_str);
    ret = set_netcard_id_update_flag(port_info->slot_id, set_list, list_size);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s netcard%d rootbdf set upgrade flag failed", __FUNCTION__, port_info->slot_id);
    }
    return;
}
 

LOCAL gint32 is_netcard_id_set(guint8 slot_id, guint8 *set_list, guint8 list_size)
{
    if (slot_id >= list_size) {
        return RET_OK;
    }
    if (slot_id == set_list[slot_id]) {
        return RET_OK;
    }
    return RET_ERR;
}
 

LOCAL void update_ocp_netcard_rootbdf(PORT_GROUP_BDF_INFO_S *ocp_port_bdf_info)
{
    guint8 slot_id;
    guint8 i;
    PORT_INFO_S port_info;
 
    guint8 set_list[MAX_PCIE_CARD] = {0};
 
    for (i = 0; i < ocp_port_bdf_info->count; i++) {
        slot_id = ocp_port_bdf_info->bdf_info[i].slot_id;
        if (slot_id < 0 || MAX_PCIE_CARD < slot_id) {
            debug_log(DLOG_ERROR, "%s incorrect OCPCard slot id:%d", __FUNCTION__, slot_id);
            return;
        }
        if (is_netcard_id_set(slot_id + 1, set_list, (guint8)sizeof(set_list)) == RET_ERR) {
            port_info.slot_id = slot_id + 1;
            port_info.port_idx = i;
            port_info.type = COMPONENT_TYPE_OCP;
            update_single_netcard_rootbdf(ocp_port_bdf_info, &port_info, set_list, (guint8)sizeof(set_list));
        } else {
            debug_log(DLOG_INFO, "%s OCP NetCard%d rootbdf already setted", __FUNCTION__, slot_id + 1);
        }
    }
    return;
}
 

LOCAL void update_pcie_netcard_rootbdf(PORT_GROUP_BDF_INFO_S *pcie_port_bdf_info)
{
    guint8 slot_id, i;
    guint8 set_list[MAX_PCIE_CARD] = {0};
    PORT_INFO_S port_info;
 
    for (i = 0; i < pcie_port_bdf_info->count; i++) {
        slot_id = pcie_port_bdf_info->bdf_info[i].slot_id;
        if (slot_id < 0 || MAX_PCIE_CARD - 1 < slot_id) {
            debug_log(DLOG_ERROR, "%s incorrect PcieCard slot id:%d", __FUNCTION__, slot_id);
            return;
        }
        if (is_netcard_id_set(slot_id + 1, set_list, (guint8)sizeof(set_list)) == RET_ERR) {
            port_info.slot_id = slot_id + 1;
            port_info.port_idx = i;
            port_info.type = COMPONENT_TYPE_PCIE_CARD;
            update_single_netcard_rootbdf(pcie_port_bdf_info, &port_info, set_list, (guint8)sizeof(set_list));
        } else {
            debug_log(DLOG_INFO, "%s PCIE NetCard%d rootbdf already setted", __FUNCTION__, slot_id + 1);
        }
    }
    return;
}
 

LOCAL gint32 get_raid_ctrl_handle_by_slot(guint8 slot_id, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    GSList *card_node = NULL;
    OBJ_HANDLE card_handle;
    OBJ_HANDLE ctrl_handle = 0;
    GSList *pciecard_list = NULL;
    guint8 card_slot;
 
    ret = dfl_get_object_list(CLASS_PCIE_CARD, &pciecard_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Failed to get obj list of CLASS_PCIE_CARD", __FUNCTION__);
        return RET_ERR;
    }
    for (card_node = pciecard_list; card_node; card_node = card_node->next) {
        card_handle = (OBJ_HANDLE)card_node->data;
        ret = dal_get_property_value_byte(card_handle, PROPERTY_PCIE_CARD_SLOT_ID, &card_slot);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get card %s %s failed", __FUNCTION__,
                dfl_get_object_name(card_handle), PROPERTY_NETCARD_SLOT_ID);
            g_slist_free(pciecard_list);
            return RET_ERR;
        }
        if (slot_id == card_slot) {
            ret = dal_get_specific_object_position(card_handle, CLASS_RAID_CONTROLLER_NAME, &ctrl_handle);
            if (ret == RET_OK) {
                *obj_handle = ctrl_handle;
                g_slist_free(pciecard_list);
                return RET_OK;
            }
        }
    }
    debug_log(DLOG_INFO, "%s:get raidcard%u handle failed", __FUNCTION__, slot_id);
    g_slist_free(pciecard_list);
    return RET_ERR;
}
 

LOCAL void update_single_raid_rootbdf(PORT_GROUP_BDF_INFO_S *pcie_raid_bdf_info, guint8 slot_id, guint8 index)
{
    gint32 ret;
    gint32 i;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    for (i = 0; i < MAX_RETRY_TIMES; i++) {
        ret = get_raid_ctrl_handle_by_slot(slot_id, &obj_handle);
        if (ret != RET_OK) {
            vos_task_delay(1000); // 每次重试间隔1000ms
            debug_log(DLOG_INFO, "%s Get raidcard%u object failed", __FUNCTION__, slot_id);
        } else {
            break;
        }
    }
    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "%s Get raidcard%u object failed", __FUNCTION__, slot_id);
        return;
    }
 
    input_list = g_slist_append(input_list, g_variant_new_byte(pcie_raid_bdf_info->bdf_info[index].bus_num));
    input_list = g_slist_append(input_list, g_variant_new_byte(pcie_raid_bdf_info->bdf_info[index].device_num));
    input_list = g_slist_append(input_list, g_variant_new_byte(pcie_raid_bdf_info->bdf_info[index].function_num));
    input_list = g_slist_append(input_list, g_variant_new_byte(pcie_raid_bdf_info->bdf_info[index].segment));
    ret = dfl_call_class_method(obj_handle, METHOD_PCIERAID_SET_ROOTBDF, NULL, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Set %s ROOTBDF failed,ret:%d,slot:%u", dfl_get_object_name(obj_handle), ret, slot_id);
        return;
    }
    update_rootbdf_info(TYPE_RAID, *pcie_raid_bdf_info);
    debug_log(DLOG_INFO, "%s Update pcieraid%d rootbdf 0x%u:0x%u:0x%u.0x%u success", __FUNCTION__, slot_id,
        pcie_raid_bdf_info->bdf_info[index].segment, pcie_raid_bdf_info->bdf_info[index].bus_num,
        pcie_raid_bdf_info->bdf_info[index].device_num, pcie_raid_bdf_info->bdf_info[index].function_num);
    return;
}
 

LOCAL void update_pcie_raid_rootbdf(PORT_GROUP_BDF_INFO_S *pcie_raid_bdf_info)
{
    guint8 slot_id;
    guint8 i;
 
    for (i = 0; i < pcie_raid_bdf_info->count; i++) {
        slot_id = pcie_raid_bdf_info->bdf_info[i].slot_id;
        if (slot_id < 0 || slot_id > MAX_PCIE_CARD - 1) {
            debug_log(DLOG_ERROR, "%s incorrect PcieCard slot:%d", __FUNCTION__, slot_id);
            return;
        }
        update_single_raid_rootbdf(pcie_raid_bdf_info, slot_id + 1, i);
    }
    return;
}

LOCAL void pcie_port_parse_bdf_thread_run(PCIE_BDF_PARSE_INFO *bdf_parse_info)
{
    gint32 ret;
    GVariant *property_value = NULL;
    MIX_PORT_GROUP_BDF_INFO_S mix_port_bdf_info;
    PORT_GROUP_BDF_INFO_S pcie_port_bdf_info;
    PORT_GROUP_BDF_INFO_S ocp_port_bdf_info;
    PORT_GROUP_BDF_INFO_S pcie_raid_bdf_info;
    OBJ_HANDLE obj_handle = bdf_parse_info->obj_handle;
    gchar *property_name = bdf_parse_info->prop_name;
 
    
    ret = dfl_get_property_value(obj_handle, property_name, &property_value);
    if (ret != DFL_OK || property_value == NULL) {
        debug_log(DLOG_ERROR, "%s:cann't get pcie port bdf value, ret:%d", __FUNCTION__, ret);
        return;
    }
 
    (void)memset_s(&mix_port_bdf_info, sizeof(MIX_PORT_GROUP_BDF_INFO_S), 0, sizeof(MIX_PORT_GROUP_BDF_INFO_S));
    (void)memset_s(&pcie_port_bdf_info, sizeof(PORT_GROUP_BDF_INFO_S), 0, sizeof(PORT_GROUP_BDF_INFO_S));
    (void)memset_s(&ocp_port_bdf_info, sizeof(PORT_GROUP_BDF_INFO_S), 0, sizeof(PORT_GROUP_BDF_INFO_S));
    (void)memset_s(&pcie_raid_bdf_info, sizeof(PORT_GROUP_BDF_INFO_S), 0, sizeof(PORT_GROUP_BDF_INFO_S));
    ret = pcie_port_bdf_parse(property_value, &mix_port_bdf_info, &pcie_port_bdf_info, &ocp_port_bdf_info,
        &pcie_raid_bdf_info);
    if (ret != RET_OK) {
        g_variant_unref(property_value);
        return;
    }
    vos_task_delay(3000); // bios 上报完BDF早于板卡加载，等待3000ms以确保板卡已经加载
    if (strcmp(property_name, PROPERTY_BIOS_PORT_ROOTBDF) == 0) {
        update_pcie_netcard_rootbdf(&pcie_port_bdf_info);
        update_ocp_netcard_rootbdf(&ocp_port_bdf_info);
        pcie_port_update_bdf(COMPONENT_TYPE_PCIE_CARD, &pcie_port_bdf_info);
        pcie_port_update_bdf(COMPONENT_TYPE_OCP, &ocp_port_bdf_info);
        update_pcie_raid_rootbdf(&pcie_raid_bdf_info);
    }
    
    g_variant_unref(property_value);
    return;
}

LOCAL gint32 pcie_bdf_condition_task(gchar *property_name, guint8 *pcie_type, PCIE_BDF_PARSE_INFO *bdf_parse_info)
{
    if (pcie_type == NULL) {
        debug_log(DLOG_ERROR, "%s:pcie_type should not be null", __FUNCTION__);
        return RET_ERR;
    }
    *pcie_type = PCIE_AND_PCIEPLUS_CONNECTOR;
    if (property_name == NULL) {
        return RET_OK; // 按原有逻辑，property_name为空时按pcie card流程走，返回成功继续执行
    }
    if (strcmp(property_name, PROPERTY_BIOS_PORT_ROOTBDF) == 0 ||
        strcmp(property_name, PROPERTY_BIOS_PCIE_PORT_ROOTBDF) == 0) {
        if (dal_check_board_special_me_type() == RET_OK) {
            pcie_port_parse_bdf_thread_run(bdf_parse_info);
            return RET_ERR; // 执行完port bdf更新流程后说明触发的是port的更新，不需要其它bdf的更新
        }
    }
    if (strcmp(property_name, PROPERTY_BIOS_PCIEDISK_BDF) == 0) {
        *pcie_type = DISK_CONNECTOR;
    } else if (strcmp(property_name, PROPERTY_BIOS_PCIECARD_BDF) == 0) {
        *pcie_type = PCIE_AND_PCIEPLUS_CONNECTOR;
    } else if (strcmp(property_name, PROPERTY_BIOS_OCPCARD_BDF) == 0) {
        *pcie_type = OCPCARD_CONNECTOR;
    }
 
    return RET_OK;
}


void _pcie_card_parse_bdf_thread(void *bdf_info)
{
    gint32 ret = 0;
    PCIE_BDF_PARSE_INFO *bdf_parse_info = (PCIE_BDF_PARSE_INFO *)bdf_info;
    gchar *property_name = NULL;
    guint8 pcie_type;
    PCIE_CARD_PARSE_PERSAVE_DATA_S pcie_card_persave_data_s;
    PCIE_CARD_PARSE_PERSAVE_DATA_S pcie_disk_persave_data_s;
    PCIE_CARD_PARSE_PERSAVE_DATA_S ocp_card_persave_data_s;

    (void)prctl(PR_SET_NAME, (gulong) "PcieParseBDF");

    if (bdf_parse_info == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return;
    }
    property_name = bdf_parse_info->prop_name;

    
    ret = pcie_bdf_condition_task(property_name, &pcie_type, bdf_parse_info);
    if (ret != RET_OK) {
        return;
    }

    if (pcie_type == PCIE_AND_PCIEPLUS_CONNECTOR) {
        g_bdf_info_parsed_status = BDF_UNPARSED_STATE;
    }

    pcie_card_persave_data_s.group_info_s_ptr = &g_pcie_group;
    pcie_card_persave_data_s.sub_group_info_s_ptr = &g_pcie_group_sub_info;
    pcie_card_persave_data_s.bdf_group_info_s_ptr = &g_pcie_cards_bdf_info;

    pcie_disk_persave_data_s.group_info_s_ptr = &g_pcie_disk_group;
    pcie_disk_persave_data_s.sub_group_info_s_ptr = &g_pcie_disk_group_sub_info;
    pcie_disk_persave_data_s.bdf_group_info_s_ptr = &g_pcie_disks_bdf_info;

    ocp_card_persave_data_s.group_info_s_ptr = &g_ocp_group;
    ocp_card_persave_data_s.sub_group_info_s_ptr = &g_ocp_group_sub_info;
    ocp_card_persave_data_s.bdf_group_info_s_ptr = &g_ocp_bdf_info;

    
    ret =
        pcie_card_parse_bdf_thread_init(&pcie_disk_persave_data_s, &pcie_card_persave_data_s, &ocp_card_persave_data_s);
    if (ret != RET_OK) {
        g_free(bdf_info);
        release_thread_sem(pcie_type);
        return;
    }

    
    ret = pcie_card_parse_bdf_thread_run(bdf_info);
    g_free(bdf_info);
    // 对于由于OS重启清空BDF的场景，此处不释放信号量(会触发VIDDID加载PCIE)
    if (ret != VOS_ERR_NOTSUPPORT) {
        release_thread_sem(pcie_type);
    }

    
    g_pcie_card_load_completed = TRUE;
    
    
    if ((pcie_type == PCIE_AND_PCIEPLUS_CONNECTOR) && (ret == RET_OK)) {
        g_bdf_info_parsed_status = BDF_PARSED_STATE;
    }
    
    return;
}


gint32 pcie_card_parse_bdf_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    PCIE_BDF_PARSE_INFO *bdf_parse_info = NULL;
    TASKID task_id;
    gint32 ret;
    const gchar *task_name = "BDFParseTask";

    if (property_name == NULL || property_value == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    
    if (strcmp(property_name, PROPERTY_BIOS_PCIEDISK_BDF) == 0 && pcie_card_get_vmd_state() == ENABLE) {
        return RET_OK;
    }
    

    bdf_parse_info = (PCIE_BDF_PARSE_INFO *)g_malloc0(sizeof(PCIE_BDF_PARSE_INFO));
    if (bdf_parse_info == NULL) {
        debug_log(DLOG_ERROR, "%s:alloc new bdf parse info failed", __FUNCTION__);
        return RET_OK;
    }

    bdf_parse_info->obj_handle = object_handle;
    (void)strncpy_s(bdf_parse_info->prop_name, MAX_NAME_SIZE, property_name, MAX_NAME_SIZE - 1);

    ret = vos_task_create(&task_id, task_name, (TASK_ENTRY)_pcie_card_parse_bdf_thread, (void *)bdf_parse_info,
        DEFAULT_PRIORITY);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "create %s failed, ret is %d", task_name, ret);
        g_free(bdf_parse_info);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 pcie_get_did_vid_from_me(const guint8 bdf_index, const guint8 pcie_index, PCIE_CARD_GROUP_INFO_S *pcie_card_info,
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info, PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info)
{
    gint32 result = RET_OK;
    INFO_NEED_GET_PCIE_INFO info;
    guint32 responce = 0;
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;
    PCIE_CARD_INFO_S pcie_card_info_s = { 0 };
    PCIE_CARD_SUB_INFO_S pcie_card_sub_info_s = { 0 };

    
    me_access_type = pcie_card_get_me_access_type();
    if (me_access_type == ME_ACCESS_TYPE_SMBUS_PECI) {
        return RET_OK;
    }
    

    
    if (pcie_card_info == NULL || pcie_card_bdf_info == NULL || pcie_card_sub_info == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    (void)memset_s(&info, sizeof(INFO_NEED_GET_PCIE_INFO), 0, sizeof(INFO_NEED_GET_PCIE_INFO));
    (void)memset_s(&pcie_card_info_s, sizeof(PCIE_CARD_INFO_S), 0, sizeof(PCIE_CARD_INFO_S));
    (void)memset_s(&pcie_card_sub_info_s, sizeof(PCIE_CARD_SUB_INFO_S), 0, sizeof(PCIE_CARD_SUB_INFO_S));
    
    info.cpu_num = pcie_card_bdf_info->bdf_info[bdf_index].cpuid;
    info.bus = pcie_card_bdf_info->bdf_info[bdf_index].bus_num;
    info.dev = pcie_card_bdf_info->bdf_info[bdf_index].device_num;
    info.func = pcie_card_bdf_info->bdf_info[bdf_index].function_num;
    info.is_local = 0;
    info.address = 0x00;
    info.length = 4;

    
    if (TRUE == match_v5_partition_product_peci_enable()) {
        
        result = pcie_get_by_peci_from_cpu(&info, &pcie_card_info_s);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "get pcie vid/did by by peci failed !");
            return RET_ERR;
        }
        pcie_card_info->info[pcie_index].vender_id = pcie_card_info_s.vender_id;
        pcie_card_info->info[pcie_index].device_id = pcie_card_info_s.device_id;

        
        info.address = PECI_SUBVENDOR_ID_OFFSET;
        result = pcie_get_by_peci_from_cpu(&info, &pcie_card_sub_info_s);
        if (result != RET_OK) {
            debug_log(DLOG_ERROR, "get pcie svid/sdid info by by peci failed !");
            return RET_ERR;
        }
        pcie_card_sub_info->sub_info[pcie_index].sub_vender_id = pcie_card_sub_info_s.sub_vender_id;
        pcie_card_sub_info->sub_info[pcie_index].sub_device_id = pcie_card_sub_info_s.sub_device_id;

        debug_log(DLOG_DEBUG,
            "pcie_get_did_vid_from_me Card vender_id [0x%04x], device_id [0x%04x], sub_vender_id [0x%04x], "
            "sub_device_id [0x%04x]",
            pcie_card_info->info[pcie_index].vender_id, pcie_card_info->info[pcie_index].device_id,
            pcie_card_sub_info->sub_info[pcie_index].sub_vender_id,
            pcie_card_sub_info->sub_info[pcie_index].sub_device_id);
        
    } else {
        
        result = pcie_get_info_from_me(&info, (guint8 *)&responce);
        if (result != RET_OK) {
            debug_log(DLOG_DEBUG,
                "get pcie didvid from me failed with solt:%02x,cpu_num:%02x,bus:%02x,dev:%02x,func:%02x",
                pcie_index, info.cpu_num, info.bus, info.dev, info.func);
            return RET_ERR;
        }

        pcie_card_info->info[pcie_index].vender_id = (guint16)(responce & 0x0000ffff);
        pcie_card_info->info[pcie_index].device_id = (guint16)((responce & 0xffff0000) >> 16);

        info.address = 0x2c;
        info.length = 4;

        
        result = pcie_get_info_from_me(&info, (guint8 *)&responce);
        if (result != RET_OK) {
            debug_log(DLOG_DEBUG,
                "get pcie subdidvid from me failed with solt:%02x,cpu_num:%02x,bus:%02x,dev:%02x,func:%02x",
                pcie_index, info.cpu_num, info.bus, info.dev, info.func);
            return RET_ERR;
        }

        pcie_card_sub_info->sub_info[pcie_index].sub_vender_id = (guint16)(responce & 0x0000ffff);
        pcie_card_sub_info->sub_info[pcie_index].sub_device_id = (guint16)((responce & 0xffff0000) >> 16);
    }

    return RET_OK;
}

gint32 debug_read_viddid_from_me(GSList *input_list)
{
    gint32 result = RET_OK;
    INFO_NEED_GET_PCIE_INFO info;
    guint32 responce = 0;
    guchar me_access_type = ME_ACCESS_TYPE_SMLINK;
    
    me_access_type = pcie_card_get_me_access_type();
    if (me_access_type == ME_ACCESS_TYPE_SMBUS_PECI) {
        return RET_OK;
    }

    
    if (input_list == NULL || 4 != g_slist_length(input_list)) {
        debug_printf("parm is not match\n");
        return RET_ERR;
    }

    (void)memset_s(&info, sizeof(INFO_NEED_GET_PCIE_INFO), 0, sizeof(INFO_NEED_GET_PCIE_INFO));

    info.cpu_num = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));
    info.bus = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    info.dev = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 2));
    info.func = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 3));
    info.is_local = 0;
    info.address = 0x00;
    info.length = 4;

    
    result = pcie_get_info_from_me(&info, (guint8 *)&responce);
    if (result != RET_OK) {
        debug_printf("Read VID/DID Failed(%d)", result);
    } else {
        debug_printf("VID:0X%04X  DID:0X%04X", responce & 0x0000ffff, (responce & 0xffff0000) >> 16);
    }

    info.address = 0x2c;
    info.length = 4;

    
    result = pcie_get_info_from_me(&info, (guint8 *)&responce);
    if (result != RET_OK) {
        debug_printf("Read SVID/SDID Failed(%d)", result);
    } else {
        debug_printf("SVID:0X%04X SDID:0X%04X", responce & 0x0000ffff, (responce & 0xffff0000) >> 16);
    }

    return RET_OK;
}
