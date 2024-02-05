


#include "pme_app/pme_app.h"
#include "pme_app/common/common_macro.h"

#include "pcie_card.h"
#include "pcie_card_comm.h"
#include "pcie_card_parse_bdf_thread.h"






LOCAL gint32 g_inited = FALSE;
LOCAL PCIE_CARD_GROUP_INFO_S *g_pcie_disk_group_local;
LOCAL PCIE_CARD_GROUP_SUB_INFO_S *g_pcie_disk_group_sub_info_local;
LOCAL PCIE_CARD_GROUP_BDF_INFO_S *g_pcie_disk_bdf_info;


LOCAL PCIE_CARD_GROUP_INFO_S *g_pcie_plus_group;
LOCAL PCIE_CARD_GROUP_SUB_INFO_S *g_pcie_plus_group_sub_info;
LOCAL PCIE_CARD_GROUP_BDF_INFO_S *g_pcie_plus_bdf_info;

LOCAL PCIE_CARD_GROUP_INFO_S *g_ocp_group_local;
LOCAL PCIE_CARD_GROUP_SUB_INFO_S *g_ocp_group_sub_info_local;
LOCAL PCIE_CARD_GROUP_BDF_INFO_S *g_ocp_bdf_info_local;

LOCAL gint32 pcie_bdf_parse(GVariant *property_value, PCIE_PARSE_INFO *parse_info);
LOCAL void pcie_type_init(gchar *property_name, PCIE_PARSE_INFO *parse_info);

LOCAL gint32 pcie_bus_valid(guint32 pcie_index, PCIE_PARSE_INFO *parse_info);
LOCAL void pcie_card_info_invalid(guint8 pcie_index, PCIE_PARSE_INFO *parse_info);
LOCAL void pcie_card_info_reset(guint8 pcie_index, PCIE_PARSE_INFO *parse_info);
LOCAL gint8 pcie_card_info_get(guint8 bdf_index, guint8 pcie_index, guint8 slot_id, PCIE_PARSE_INFO *parse_info);
LOCAL gint32 pcie_card_xml_unload(guint8 pcie_index, guint8 slot_id, PCIE_PARSE_INFO *parse_info);
LOCAL void pcie_card_xml_load(PCIE_PARSE_INFO *parse_info);
LOCAL gint8 pcie_slot_reload(guint8 slot_id, gint8 last_flag, PCIE_PARSE_INFO *parse_info);
LOCAL gint32 pcie_slot_scan(gint8 *read_me_flags, guint32 size, PCIE_PARSE_INFO *parse_info);
LOCAL void pcie_max_scan_count(PCIE_PARSE_INFO *parse_info);

LOCAL gint32 pcie_scan(PCIE_PARSE_INFO *parse_info);

#define SINGLE_BDF_GROUP_LEN     5


LOCAL gint32 pcie_bdf_parse(GVariant *property_value, PCIE_PARSE_INFO *parse_info)
{
    gsize bdf_data_len = 0;
    const guint8 *bdf_data = NULL;
    gint32 ret = 0;
    OBJ_HANDLE physical_partition_obj_handle;
    GVariant *property_data = NULL;
    guint8 segment = 0;
    guint32 i;
    guint8 pcie_index = 0;

    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;
    PCIE_CARD_GROUP_INFO_S *pcie_card_info = parse_info->pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info = parse_info->pcie_card_sub_info;

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

    
    
    if (bdf_data_len < 5) {
        debug_log(DLOG_ERROR, "bdf_data_len: %" G_GSIZE_FORMAT " is invalid!", bdf_data_len);
        return RET_ERR;
    }

    if (bdf_data[0] != 0) {
        ret = dfl_get_object_handle(OBJECT_PHYSICAL_PARTITION, &physical_partition_obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_object_handle fail!result = %d", ret);
        }

        
        ret =
            dfl_get_extern_value(physical_partition_obj_handle, PROPERTY_COMPUTER_PARTITION_ID, &property_data, DF_HW);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "dfl_get_property_data %s fail!", PROPERTY_COMPUTER_PARTITION_ID);
        }
        
        if (property_data != NULL) {
            segment = g_variant_get_byte(property_data);
            g_variant_unref(property_data);
        }

        
        // property_data = NULL;
    } else {
        segment = 0;
    }

    
    debug_log(DLOG_INFO, "pcie_card_parse_bdf_info segment = %d, bdf_data_len=%" G_GSIZE_FORMAT, segment, bdf_data_len);

    for (i = 0; i < bdf_data_len / 5; i++) {
        if (bdf_data[5 * i] == segment) {
            pcie_card_bdf_info->bdf_info[pcie_index].segment = bdf_data[5 * i];
            pcie_card_bdf_info->bdf_info[pcie_index].cpuid = bdf_data[5 * i + 1];
            pcie_card_bdf_info->bdf_info[pcie_index].bus_num = bdf_data[5 * i + 2];
            pcie_card_bdf_info->bdf_info[pcie_index].device_num = bdf_data[5 * i + 3];
            pcie_card_bdf_info->bdf_info[pcie_index].function_num = bdf_data[5 * i + 4];

            if (pcie_card_bdf_info->bdf_info[pcie_index].cpuid != G_MAXUINT8 ||
                pcie_card_bdf_info->bdf_info[pcie_index].bus_num != G_MAXUINT8 ||
                pcie_card_bdf_info->bdf_info[pcie_index].device_num != G_MAXUINT8 ||
                pcie_card_bdf_info->bdf_info[pcie_index].function_num != G_MAXUINT8) {
                debug_log(DLOG_ERROR, "PcieCard%d, cpuid:0x%x, bus:0x%x  dev:0x%x  func:0x%x", pcie_index + 1,
                    pcie_card_bdf_info->bdf_info[pcie_index].cpuid, pcie_card_bdf_info->bdf_info[pcie_index].bus_num,
                    pcie_card_bdf_info->bdf_info[pcie_index].device_num,
                    pcie_card_bdf_info->bdf_info[pcie_index].function_num);
            }

            if (pcie_index == MAX_PCIE_CARD) {
                break;
            }

            pcie_index++;
            pcie_card_bdf_info->count = pcie_index;
            pcie_card_info->count = pcie_index;
            pcie_card_sub_info->count = pcie_index;
        }
    }

    return RET_OK;
}


LOCAL void pcie_type_init(gchar *property_name, PCIE_PARSE_INFO *parse_info)
{
    guint8 pcie_type;
    PCIE_CARD_GROUP_INFO_S *pcie_group_info = NULL;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info = NULL;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_group_bdf_info = NULL;

    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;

    
    if (strcmp(property_name, PROPERTY_BIOS_PCIEDISK_BDF) == 0) {
        pcie_type = DISK_CONNECTOR;
        pcie_group_info = g_pcie_disk_group_local;
        pcie_group_sub_info = g_pcie_disk_group_sub_info_local;
        pcie_group_bdf_info = g_pcie_disk_bdf_info;
        (void)memcpy_s(pcie_group_bdf_info, sizeof(PCIE_CARD_GROUP_BDF_INFO_S), pcie_card_bdf_info,
            sizeof(PCIE_CARD_GROUP_BDF_INFO_S));
    } else if (strcmp(property_name, PROPERTY_BIOS_OCPCARD_BDF) == 0) {
        pcie_type = OCPCARD_CONNECTOR;
        pcie_group_info = g_ocp_group_local;
        pcie_group_sub_info = g_ocp_group_sub_info_local;
        pcie_group_bdf_info = g_ocp_bdf_info_local;
        (void)memcpy_s(pcie_group_bdf_info, sizeof(PCIE_CARD_GROUP_BDF_INFO_S), pcie_card_bdf_info,
            sizeof(PCIE_CARD_GROUP_BDF_INFO_S));
    } else {
        pcie_type = PCIE_AND_PCIEPLUS_CONNECTOR;
        pcie_group_info = g_pcie_plus_group;
        pcie_group_sub_info = g_pcie_plus_group_sub_info;
        pcie_group_bdf_info = g_pcie_plus_bdf_info;
        (void)memcpy_s(pcie_group_bdf_info, sizeof(PCIE_CARD_GROUP_BDF_INFO_S), pcie_card_bdf_info,
            sizeof(PCIE_CARD_GROUP_BDF_INFO_S));
    }

    
    parse_info->pcie_group_info = pcie_group_info;
    parse_info->pcie_group_sub_info = pcie_group_sub_info;
    parse_info->pcie_group_bdf_info = pcie_group_bdf_info;
    parse_info->pcie_type = pcie_type;

    return;
}


gint32 pcie_slot_get(guint8 pcie_type, guint8 original_index, guint8 *slot_id, guint8 *pcie_index)
{
    if (pcie_get_slot_by_index(pcie_type, original_index, slot_id) != RET_OK) {
        return RET_ERR;
    };
    
    
    if (pcie_type == DISK_CONNECTOR) {
        *pcie_index = *slot_id;
    } else {
        
        if (*slot_id < 1) {
            return RET_ERR;
        }

        *pcie_index = *slot_id - 1;
    }

    return RET_OK;
}


LOCAL gint32 pcie_bus_valid(guint32 pcie_index, PCIE_PARSE_INFO *parse_info)
{
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;

    if ((pcie_card_bdf_info->bdf_info[pcie_index].bus_num == 0xFF &&
        pcie_card_bdf_info->bdf_info[pcie_index].device_num == 0xFF &&
        pcie_card_bdf_info->bdf_info[pcie_index].function_num == 0xFF) ||
        pcie_card_bdf_info->bdf_info[pcie_index].bus_num == 0) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL void pcie_card_info_invalid(guint8 pcie_index, PCIE_PARSE_INFO *parse_info)
{
    PCIE_CARD_GROUP_INFO_S *pcie_card_info = parse_info->pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info = parse_info->pcie_card_sub_info;
    if (pcie_index >= MAX_PCIE_CARD) {
        debug_log(DLOG_ERROR, "[%s] Array subscript(%u) out of range.", __FUNCTION__, pcie_index);
        return;
    }
    pcie_card_info->info[pcie_index].vender_id = PCIE_INVALID_VID;
    pcie_card_info->info[pcie_index].device_id = PCIE_INVALID_DID;
    pcie_card_sub_info->sub_info[pcie_index].sub_vender_id = PCIE_INVALID_SUBVID;
    pcie_card_sub_info->sub_info[pcie_index].sub_device_id = PCIE_INVALID_SUBDID;

    return;
}


LOCAL void pcie_card_info_reset(guint8 pcie_index, PCIE_PARSE_INFO *parse_info)
{
    PCIE_CARD_GROUP_INFO_S *pcie_group_info = parse_info->pcie_group_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info = parse_info->pcie_group_sub_info;
    PCIE_CARD_GROUP_INFO_S *pcie_card_info = parse_info->pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info = parse_info->pcie_card_sub_info;

    if (pcie_index >= MAX_PCIE_CARD) {
        debug_log(DLOG_ERROR, "[%s] Array subscript out of range.", __FUNCTION__);
        return;
    }
 
    pcie_card_info->info[pcie_index].device_id = pcie_group_info->info[pcie_index].device_id;
    pcie_card_info->info[pcie_index].vender_id = pcie_group_info->info[pcie_index].vender_id;

    pcie_card_sub_info->sub_info[pcie_index].sub_vender_id = pcie_group_sub_info->sub_info[pcie_index].sub_vender_id;
    pcie_card_sub_info->sub_info[pcie_index].sub_device_id = pcie_group_sub_info->sub_info[pcie_index].sub_device_id;

    return;
}


LOCAL gint8 pcie_card_info_get(guint8 bdf_index, guint8 pcie_index, guint8 slot_id, PCIE_PARSE_INFO *parse_info)
{
    gint32 ret;
    gint8 success;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;
    PCIE_CARD_GROUP_INFO_S *pcie_card_info = parse_info->pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info = parse_info->pcie_card_sub_info;

    success = RET_OK;

    ret = pcie_get_did_vid_from_me(bdf_index, pcie_index, pcie_card_info, pcie_card_bdf_info, pcie_card_sub_info);
    
    if ((ret != RET_OK) ||
        (pcie_card_info->info[pcie_index].vender_id == 0 && pcie_card_info->info[pcie_index].device_id == 0)) {
        success = RET_ERR;
    }

    
    if ((TRUE == match_v5_partition_product_peci_enable()) && (PCIE_AND_PCIEPLUS_CONNECTOR == parse_info->pcie_type) &&
        (pcie_card_sub_info->sub_info[pcie_index].sub_vender_id == 0 &&
        pcie_card_sub_info->sub_info[pcie_index].sub_device_id == 0)) {
        success = RET_ERR;
    }

    if (success != RET_OK) {
        debug_log(DLOG_ERROR, "Get pcie card%d subvender_id/subdevice_id fail!", slot_id);
        pcie_card_info_reset(pcie_index, parse_info);
    }

    return success;
}


LOCAL gint32 pcie_card_xml_unload(guint8 pcie_index, guint8 slot_id, PCIE_PARSE_INFO *parse_info)
{
    gint32 ret;
    volatile guint32 old_boardid = 0;
    volatile guint32 new_boardid = 0;
    volatile guint32 old_auxid = 0;
    volatile guint32 new_auxid = 0;
    OBJ_HANDLE obj_handle = 0;
    guint32 pos;

    guint8 pcie_type = parse_info->pcie_type;
    PCIE_CARD_GROUP_INFO_S *pcie_group_info = parse_info->pcie_group_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info = parse_info->pcie_group_sub_info;
    PCIE_CARD_GROUP_INFO_S *pcie_card_info = parse_info->pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info = parse_info->pcie_card_sub_info;

    old_boardid = (pcie_group_info->info[pcie_index].vender_id << VENDER_OFFSET) +
        pcie_group_info->info[pcie_index].device_id;
    new_boardid = (pcie_card_info->info[pcie_index].vender_id << VENDER_OFFSET) +
        pcie_card_info->info[pcie_index].device_id;
    
    old_auxid = (pcie_group_sub_info->sub_info[pcie_index].sub_vender_id << VENDER_OFFSET) +
        pcie_group_sub_info->sub_info[pcie_index].sub_device_id;
    new_auxid = (pcie_card_sub_info->sub_info[pcie_index].sub_vender_id << VENDER_OFFSET) +
        pcie_card_sub_info->sub_info[pcie_index].sub_device_id;
    
    debug_log(DLOG_ERROR, "pcie_card_parse_bdf_info  0x%x  0x%x  0x%x  0x%x", old_boardid, new_boardid, old_auxid,
        new_auxid);

    
    if ((old_boardid != new_boardid) || (old_auxid != new_auxid)) {
        
        if (old_boardid != PCIE_NO_CARD) {
            
            ret = pcie_card_get_connector_handle(slot_id, &obj_handle, pcie_type);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail! Type = %d .", pcie_index, pcie_type);
                return RET_ERR;
            }

            (void)pcie_card_load_xml(obj_handle, old_boardid, old_auxid, PCIE_CARD_XML_UNLOAD, pcie_type);
            pcie_card_log_operation_log(pcie_type, slot_id, pcie_group_info->info[pcie_index].vender_id,
                pcie_group_info->info[pcie_index].device_id, PCIE_CARD_XML_UNLOAD);

            
            pos = 0;
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_CONNECTOR_POSITION, &pos);
            _add_item_to_deleting_pos_list(pos);
            

            pcie_group_info->info[pcie_index].vender_id = PCIE_INVALID_VID;
            pcie_group_info->info[pcie_index].device_id = PCIE_INVALID_DID;
            
            pcie_group_sub_info->sub_info[pcie_index].sub_vender_id = PCIE_INVALID_SUBVID;
            pcie_group_sub_info->sub_info[pcie_index].sub_device_id = PCIE_INVALID_SUBDID;
            
        }
    }

    return RET_OK;
}


LOCAL void pcie_card_xml_load(PCIE_PARSE_INFO *parse_info)
{
    guint32 i;
    guint8 slot_id = 0;
    guint8 pcie_index = 0;
    gint32 ret = 0;

    volatile guint32 old_boardid = 0;
    volatile guint32 new_boardid = 0;
    volatile guint32 old_auxid = 0;
    volatile guint32 new_auxid = 0;
    OBJ_HANDLE obj_handle = 0;

    guint8 pcie_type = parse_info->pcie_type;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;
    PCIE_CARD_GROUP_INFO_S *pcie_card_info = parse_info->pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info = parse_info->pcie_card_sub_info;
    PCIE_CARD_GROUP_INFO_S *pcie_group_info = parse_info->pcie_group_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info = parse_info->pcie_group_sub_info;

    for (i = 0; i < pcie_card_bdf_info->count; i++) {
        
        ret = pcie_slot_get(pcie_type, i, &slot_id, &pcie_index);
        if (ret != RET_OK || pcie_index >= MAX_PCIE_CARD) { // 判断下边数组越界情况
            continue;
        }

        
        if (parse_info->max_slotid < slot_id) {
            parse_info->max_slotid = slot_id;
        }

        old_boardid =
            pcie_group_info->info[pcie_index].vender_id * 0x10000 + pcie_group_info->info[pcie_index].device_id;
        new_boardid = pcie_card_info->info[pcie_index].vender_id * 0x10000 + pcie_card_info->info[pcie_index].device_id;
        
        old_auxid = pcie_group_sub_info->sub_info[pcie_index].sub_vender_id * 0x10000 +
            pcie_group_sub_info->sub_info[pcie_index].sub_device_id;
        new_auxid = pcie_card_sub_info->sub_info[pcie_index].sub_vender_id * 0x10000 +
            pcie_card_sub_info->sub_info[pcie_index].sub_device_id;
        
        if (old_boardid != G_MAXUINT32 || new_boardid != G_MAXUINT32 || old_auxid != G_MAXUINT32 ||
            new_auxid != G_MAXUINT32) {
            debug_log(DLOG_ERROR, "pcie_card_parse_bdf_info  0x%x  0x%x  0x%x  0x%x", old_boardid, new_boardid,
                old_auxid, new_auxid);
        }

        if ((old_boardid != new_boardid) || (old_auxid != new_auxid)) {
            
            if (new_boardid != PCIE_NO_CARD) {
                
                ret = pcie_card_get_connector_handle(slot_id, &obj_handle, pcie_type);
                if (ret != RET_OK) {
                    debug_log(DLOG_ERROR, "Get pcie card%d connector handle fail! Type = %d .", pcie_index,
                        pcie_type);
                    continue;
                }

                (void)pcie_card_load_xml(obj_handle, new_boardid, new_auxid, PCIE_CARD_XML_LOAD, pcie_type);
                pcie_card_log_operation_log(pcie_type, slot_id, pcie_card_info->info[pcie_index].vender_id,
                    pcie_card_info->info[pcie_index].device_id, PCIE_CARD_XML_LOAD);

                pcie_group_info->info[pcie_index].vender_id = pcie_card_info->info[pcie_index].vender_id;
                pcie_group_info->info[pcie_index].device_id = pcie_card_info->info[pcie_index].device_id;
                
                pcie_group_sub_info->sub_info[pcie_index].sub_vender_id =
                    pcie_card_sub_info->sub_info[pcie_index].sub_vender_id;
                pcie_group_sub_info->sub_info[pcie_index].sub_device_id =
                    pcie_card_sub_info->sub_info[pcie_index].sub_device_id;
            }
        } else if (PCIE_INVALID_VID_DID != new_boardid) {
            
            ret = pcie_card_update_bdf(pcie_card_bdf_info, new_boardid, new_auxid, slot_id, pcie_type);
            if (ret == RET_OK) {
                debug_log(DLOG_DEBUG, "Update the BDF success!boardid = 0x%x, auxid = 0x%x, slot_id = %u",
                    new_boardid, new_auxid, slot_id);
            } else {
                debug_log(DLOG_ERROR, "Update the BDF failed!boardid = 0x%x, auxid = 0x%x, slot_id = %u", new_boardid,
                    new_auxid, slot_id);
            }
        }
    }

    return;
}


LOCAL gint8 pcie_slot_reload(guint8 slot_id, gint8 last_flag, PCIE_PARSE_INFO *parse_info)
{
    guint8 pciecard_reload = 0;
    gint8 ret = RET_OK;

    
    if (last_flag == RET_ERR) {
        return RET_ERR;
    }

    if (((parse_info->pcie_type == PCIE_AND_PCIEPLUS_CONNECTOR) || (parse_info->pcie_type == OCPCARD_CONNECTOR)) &&
        (pcie_card_get_reload_enable(slot_id, &pciecard_reload) == RET_OK)) {
        debug_log(DLOG_INFO, "slot%d pciecard_reload  %d   ", slot_id, pciecard_reload);
        if (pciecard_reload == 1) {
            ret = RET_ERR;
        }
    }

    return ret;
}


LOCAL gint32 pcie_slot_scan(gint8 *read_me_flags, guint32 size, PCIE_PARSE_INFO *parse_info)
{
    guint32 i;
    guint32 count;
    guint8 slot_id = 0;
    guint8 pcie_index = 0;
    gint32 ret = 0;
    OBJ_HANDLE cardaction_obj_handle = 0;
    PCIE_ACTION_PRIV_DATA_S *priv_data = NULL;
    guint8 pcie_type = parse_info->pcie_type;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;

    count = pcie_card_bdf_info->count;
    if (count > size) {
        debug_log(DLOG_ERROR, "bdf count over flow, %d", count);
        count = size;
    }

    for (i = 0; i < count; i++) {
        
        
        ret = pcie_slot_get(pcie_type, i, &slot_id, &pcie_index);
        if (ret != RET_OK) {
            
            read_me_flags[i] = RET_OK;
            continue;
        }

        

        pcie_card_info_reset(pcie_index, parse_info);

        do {
            
            if (pcie_bus_valid(i, parse_info) != RET_OK) {
                
                pcie_card_info_invalid(pcie_index, parse_info);
                read_me_flags[i] = RET_OK;
                break;
            }

            
            ret = pcie_card_get_cardaction_handle(slot_id, pcie_type, &cardaction_obj_handle);
            if (ret == RET_OK) {
                
                ret = dfl_get_binded_data(cardaction_obj_handle, (gpointer *)&priv_data);
                
                if (ret != DFL_OK) {
                    debug_log(DLOG_ERROR, "get private property fail, result is %d", ret);
                    return RET_ERR;
                }
                

                debug_log(DLOG_DEBUG, "slot%d PcieCard  Cpld Present status %d ", slot_id, priv_data->present);

                
                if (priv_data->present == PCIE_PLUG_OUT) {
                    
                    pcie_card_info_invalid(pcie_index, parse_info);
                    read_me_flags[i] = RET_OK;
                    break;
                }
            }

            
            
            read_me_flags[i] = pcie_slot_reload(slot_id, read_me_flags[i], parse_info);

            
            if (read_me_flags[i] != RET_OK) {
                debug_log(DLOG_INFO, "slot%d PcieCard  pcie_get_did_vid_from_me", slot_id);

                
                read_me_flags[i] = pcie_card_info_get(i, pcie_index, slot_id, parse_info);
            }
        } while (0);

        
        (void)pcie_card_xml_unload(pcie_index, slot_id, parse_info);
    }

    return RET_OK;
}


LOCAL void pcie_max_scan_count(PCIE_PARSE_INFO *parse_info)
{
    
    if (parse_info->pcie_type == DISK_CONNECTOR) {
        parse_info->pcie_group_info->count = parse_info->max_slotid + 1;
        parse_info->pcie_group_sub_info->count = parse_info->max_slotid + 1;
    } else {
        parse_info->pcie_group_info->count = parse_info->max_slotid;
        parse_info->pcie_group_sub_info->count = parse_info->max_slotid;
    }

    if (parse_info->pcie_group_info->count < parse_info->pcie_card_bdf_info->count) {
        parse_info->pcie_group_info->count = parse_info->pcie_card_bdf_info->count;
        parse_info->pcie_group_sub_info->count = parse_info->pcie_card_bdf_info->count;
    }

    return;
}


LOCAL gint32 pcie_scan(PCIE_PARSE_INFO *parse_info)
{
    guint32 i;
    guint32 wait_retry_times;
#define RETRY_TIMES_MAX 20
    guint32 retry_times = RETRY_TIMES_MAX;
    gint8 read_me_flags[MAX_PCIE_CARD];
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;

    
    for (i = 0; i < MAX_PCIE_CARD; i++) {
        read_me_flags[i] = RET_ERR;
    }
    parse_info->max_slotid = 0;

    do {
        
        
        _deleting_pos_list_remove_all();

        
        if (pcie_slot_scan((gint8 *)read_me_flags, MAX_PCIE_CARD, parse_info) != RET_OK) {
            return RET_ERR;
        }

        
        vos_task_delay(3000);

        
        
        wait_retry_times = 30;

        while (_get_deleting_pos_list_length() > 0 && wait_retry_times > 0) {
            wait_retry_times--;
            debug_log(DLOG_INFO, "current length is %d", _get_deleting_pos_list_length());
            vos_task_delay(1000);
        }

        

        
        
        pcie_card_xml_load(parse_info);

        
        
        // 高性能 PCIE 设备较多，不循环调用。
        if (pcie_card_load_xml_retry_enable() == RET_OK) {
            retry_times--;
        } else {
            
            retry_times = retry_times - 4;
        }

        
        
        for (i = 0; i < pcie_card_bdf_info->count; i++) {
            if (read_me_flags[i] != RET_OK) {
                break;
            }
        }

        if (i == pcie_card_bdf_info->count) {
            retry_times = 0;
            debug_log(DLOG_INFO, "Load xml all success");
        }
    } while (retry_times > 0);

    pcie_max_scan_count(parse_info);

    return RET_OK;
}


gint32 pcie_card_parse_bdf_thread_init(PCIE_CARD_PARSE_PERSAVE_DATA_S *pcie_disk_persave_data_ptr,
    PCIE_CARD_PARSE_PERSAVE_DATA_S *pcie_card_persave_data_ptr,
    PCIE_CARD_PARSE_PERSAVE_DATA_S *ocp_card_persave_data_ptr)
{
    guint32 ret;
    g_inited = RET_ERR;

    if ((pcie_disk_persave_data_ptr == NULL) || (pcie_card_persave_data_ptr == NULL) ||
        (ocp_card_persave_data_ptr == NULL)) {
        debug_log(DLOG_ERROR, "%s failed:input param is null", __FUNCTION__);
        return RET_ERR;
    }

    ret = pcie_disk_persave_data_ptr->group_info_s_ptr && pcie_disk_persave_data_ptr->sub_group_info_s_ptr &&
        pcie_disk_persave_data_ptr->bdf_group_info_s_ptr && pcie_card_persave_data_ptr->group_info_s_ptr &&
        pcie_card_persave_data_ptr->sub_group_info_s_ptr && pcie_card_persave_data_ptr->bdf_group_info_s_ptr &&
        ocp_card_persave_data_ptr->group_info_s_ptr && ocp_card_persave_data_ptr->sub_group_info_s_ptr &&
        ocp_card_persave_data_ptr->bdf_group_info_s_ptr;

    if (ret == 0) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    g_pcie_disk_group_local = pcie_disk_persave_data_ptr->group_info_s_ptr;
    g_pcie_disk_group_sub_info_local = pcie_disk_persave_data_ptr->sub_group_info_s_ptr;
    g_pcie_disk_bdf_info = pcie_disk_persave_data_ptr->bdf_group_info_s_ptr;
    g_pcie_plus_group = pcie_card_persave_data_ptr->group_info_s_ptr;
    g_pcie_plus_group_sub_info = pcie_card_persave_data_ptr->sub_group_info_s_ptr;
    g_pcie_plus_bdf_info = pcie_card_persave_data_ptr->bdf_group_info_s_ptr;
    g_ocp_group_local = ocp_card_persave_data_ptr->group_info_s_ptr;
    g_ocp_group_sub_info_local = ocp_card_persave_data_ptr->sub_group_info_s_ptr;
    g_ocp_bdf_info_local = ocp_card_persave_data_ptr->bdf_group_info_s_ptr;

    g_inited = TRUE;

    return RET_OK;
}

LOCAL void pcie_card_update_resid(guint8 slot_id, guint8 type, guint8 resid, gint32 retry)
{
    gint32 i;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
 
    if (slot_id <= 0 || MAX_PCIE_CARD < slot_id) {
        debug_log(DLOG_ERROR, "%s:PcieCard slot id error:%u", __FUNCTION__, slot_id);
        return;
    }

    for (i = 0; i < retry; i++) {
        ret = pcie_card_get_object_handle_by_idinfo(0, 0, slot_id, type, &obj_handle);
        if (ret != RET_OK) {
            if (retry > 1) {
                vos_task_delay(1000); // viddid上报晚于bdf上报，且需要等待xml加载后生成对象再更新，延时1000ms
                debug_log(DLOG_INFO, "Get card object failed! retry times:%d", i);
            } else {
                debug_log(DLOG_ERROR, "%s:%d Get pcie card object failed!", __FUNCTION__, __LINE__);
                return;
            }
        } else {
            break;
        }
    }
    if (obj_handle == 0) {
        debug_log(DLOG_ERROR, "Get pcie card%u object failed! type %u", slot_id, type);
        return;
    }
 
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_PCIECARD_CPU_ID, resid, DF_NONE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Update pcie card resid failed,ret:%d,slot:%u", __FUNCTION__, ret, slot_id);
        return;
    }
    debug_log(DLOG_DEBUG, "Update resid success, slot_id=%u,resid=%u", slot_id, resid);
    return;
}


void pcie_card_update_ft_resid(PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info,
    PCIE_CARD_GROUP_BDF_INFO_S *ocp_card_bdf_info)
{
#define MAX_RETRY_TIMES 12
    gint32 ret;
    guint32 i;
    guint8 tmp;
    guint8 slot_id;

    if (pcie_card_bdf_info == NULL || ocp_card_bdf_info == NULL) {
        debug_log(DLOG_ERROR, "%s:invalid parameter input", __FUNCTION__);
        return;
    }
    for (i = 0; i < pcie_card_bdf_info->count; i++) {
        if (pcie_card_bdf_info->bdf_info[i].cpuid != G_MAXUINT8) {
            ret = pcie_slot_get(PCIE_AND_PCIEPLUS_CONNECTOR, i, &slot_id, &tmp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:get pcie slot failed, index:%u,slot_id:%u", __FUNCTION__, i, slot_id);
                continue;
            }
            pcie_card_update_resid(slot_id, PCIE_AND_PCIEPLUS_CONNECTOR,
                pcie_card_bdf_info->bdf_info[i].cpuid + 1, MAX_RETRY_TIMES);
        }
    }
    for (i = 0; i < ocp_card_bdf_info->count; i++) {
        if (ocp_card_bdf_info->bdf_info[i].cpuid != G_MAXUINT8) {
            ret = pcie_slot_get(OCPCARD_CONNECTOR, i, &slot_id, &tmp);
            if (ret != RET_OK) {
                debug_log(DLOG_ERROR, "%s:get ocp slot failed, index:%u,slot_id:%u", __FUNCTION__, i, slot_id);
                continue;
            }
            pcie_card_update_resid(slot_id, OCPCARD_CONNECTOR,
                ocp_card_bdf_info->bdf_info[i].cpuid + 1, MAX_RETRY_TIMES);
        }
    }
    return;
}


LOCAL gint32 get_bdf_data_and_len(const guint8 **bdf_data, gsize *bdf_data_len, GVariant *property_value)
{
    *bdf_data = (const guint8 *)g_variant_get_fixed_array(property_value, bdf_data_len, sizeof(guint8));
    if (*bdf_data == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL bdf data", __FUNCTION__);
        return RET_ERR;
    }
    if (*bdf_data_len == 1 && *bdf_data[0] == 0) {
        debug_log(DLOG_ERROR, "%s: bdf data reset for os reset", __FUNCTION__);
        return RET_ERR;
    }
    if (*bdf_data_len < SINGLE_BDF_GROUP_LEN) {
        debug_log(DLOG_ERROR, "bdf_data: bdf_data_len: %" G_GSIZE_FORMAT " failed!", *bdf_data_len);
        return RET_ERR;
    }
 
    debug_log(DLOG_INFO, "%s  bdf_data_len=%" G_GSIZE_FORMAT, __FUNCTION__, *bdf_data_len);
    return RET_OK;
}

LOCAL gint32 updat_pcie_card_bdf(PCIE_PARSE_INFO *parse_info, GVariant *property_value)
{
    gsize bdf_data_len;
    const guint8 *bdf_data = NULL;
    gint32 i, ret;
    guint8 index = 0;
    guint8 slot_id, tmp;
 
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info = parse_info->pcie_card_bdf_info;
    if (get_bdf_data_and_len(&bdf_data, &bdf_data_len, property_value) != RET_OK) {
        return RET_ERR;
    }
    for (i = 0; i < bdf_data_len / SINGLE_BDF_GROUP_LEN; i++) {
        if (index == MAX_PCIE_CARD) {
            break;
        }
        pcie_card_bdf_info->bdf_info[index].segment = bdf_data[SINGLE_BDF_GROUP_LEN * i];
        pcie_card_bdf_info->bdf_info[index].cpuid = bdf_data[SINGLE_BDF_GROUP_LEN * i + 1];
        pcie_card_bdf_info->bdf_info[index].bus_num = bdf_data[SINGLE_BDF_GROUP_LEN * i + 2]; // bus偏移2
        pcie_card_bdf_info->bdf_info[index].device_num = bdf_data[SINGLE_BDF_GROUP_LEN * i + 3]; // device 偏移3
        pcie_card_bdf_info->bdf_info[index].function_num = bdf_data[SINGLE_BDF_GROUP_LEN * i + 4]; // func 偏移4
 
        if (pcie_card_bdf_info->bdf_info[index].cpuid != G_MAXUINT8 ||
            pcie_card_bdf_info->bdf_info[index].bus_num != G_MAXUINT8 ||
            pcie_card_bdf_info->bdf_info[index].device_num != G_MAXUINT8 ||
            pcie_card_bdf_info->bdf_info[index].function_num != G_MAXUINT8) {
            debug_log(DLOG_ERROR, "PcieCard%d, cpuid:%d, bus:%d, dev:%d, func:%d, segment:%d", index + 1,
                pcie_card_bdf_info->bdf_info[index].cpuid, pcie_card_bdf_info->bdf_info[index].bus_num,
                pcie_card_bdf_info->bdf_info[index].device_num, pcie_card_bdf_info->bdf_info[index].function_num,
                pcie_card_bdf_info->bdf_info[index].segment);
        }
 
        if (pcie_card_bdf_info->bdf_info[index].segment != G_MAXUINT8) {
            if (pcie_slot_get(parse_info->pcie_type, index, &slot_id, &tmp) != RET_OK) {
                return RET_ERR;
            }
            if (parse_info->max_slotid < slot_id) {
                parse_info->max_slotid = slot_id;
            }
            ret = pcie_card_update_bdf(pcie_card_bdf_info, 0, 0, slot_id, parse_info->pcie_type);
            if (ret == RET_OK) {
                debug_log(DLOG_DEBUG, "%s:Update BDF success, slot_id:%u", __FUNCTION__, slot_id);
            } else {
                debug_log(DLOG_ERROR, "%s:Update BDF failed, slot_id:%u,type:%u,ret:%d", __FUNCTION__, slot_id,
                    parse_info->pcie_type, ret);
            }
            // update cpu resid
            pcie_card_update_resid(slot_id, parse_info->pcie_type, pcie_card_bdf_info->bdf_info[index].cpuid + 1, 1);
        }
        index++;
        pcie_card_bdf_info->count = index;
    }
 
    return RET_OK;
}

LOCAL gint32 pcie_condition_scan(PCIE_PARSE_INFO *parse_info, GVariant *property_value, gchar *property_name)
{
    gint32 ret;

    if (dal_check_board_special_me_type() == RET_OK) {
        parse_info->max_slotid = 0;
        ret = updat_pcie_card_bdf(parse_info, property_value);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:updat_pcie_card_bdf failed!", __FUNCTION__);
            return RET_ERR;
        }
        pcie_type_init(property_name, parse_info);
    } else {
        ret = pcie_scan(parse_info);
        if (ret != RET_OK) {
            return RET_ERR;
        }
        per_save((guint8 *)parse_info->pcie_group_info);
        per_save((guint8 *)parse_info->pcie_group_sub_info);
    }
    per_save((guint8 *)parse_info->pcie_group_bdf_info);
    return RET_OK;
}

gint32 pcie_card_parse_bdf_thread_run(void *bdf_info)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    gchar *property_name = NULL;
    PCIE_CARD_GROUP_INFO_S pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S pcie_card_sub_info;
    PCIE_CARD_GROUP_BDF_INFO_S pcie_card_bdf_info;
    
    PCIE_BDF_PARSE_INFO *bdf_parse_info = (PCIE_BDF_PARSE_INFO *)bdf_info;
    PCIE_PARSE_INFO parse_info;

    if (g_inited == FALSE) {
        debug_log(DLOG_ERROR, "%s failed:sub system not init ", __FUNCTION__);
        return RET_ERR;
    }
    if (bdf_parse_info == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return RET_ERR;
    }

    
    obj_handle = bdf_parse_info->obj_handle;
    property_name = bdf_parse_info->prop_name;

    
    ret = dfl_get_property_value(obj_handle, property_name, &property_value);
    if (ret != DFL_OK || property_value == NULL) {
        debug_log(DLOG_ERROR, "%s:cann't get pcie card bdf value, ret is %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    (void)memset_s(&pcie_card_bdf_info, sizeof(PCIE_CARD_GROUP_BDF_INFO_S), 0, sizeof(PCIE_CARD_GROUP_BDF_INFO_S));
    (void)memset_s(&pcie_card_info, sizeof(PCIE_CARD_GROUP_INFO_S), 0, sizeof(PCIE_CARD_GROUP_INFO_S));
    (void)memset_s(&pcie_card_sub_info, sizeof(PCIE_CARD_GROUP_SUB_INFO_S), 0, sizeof(PCIE_CARD_GROUP_SUB_INFO_S));

    
    parse_info.pcie_card_info = &pcie_card_info;
    parse_info.pcie_card_sub_info = &pcie_card_sub_info;
    parse_info.pcie_card_bdf_info = &pcie_card_bdf_info;

    
    ret = pcie_bdf_parse(property_value, &parse_info);
    if (ret != RET_OK) {
        g_variant_unref(property_value);
        return ret;
    }

    
    pcie_type_init(property_name, &parse_info);

    
    ret = pcie_condition_scan(&parse_info, property_value, property_name);
    if (ret != RET_OK) {
        g_variant_unref(property_value);
        return RET_ERR;
    }

    
    per_save((guint8 *)parse_info.pcie_group_info);
    per_save((guint8 *)parse_info.pcie_group_bdf_info);
    per_save((guint8 *)parse_info.pcie_group_sub_info);

    g_variant_unref(property_value);

    return RET_OK;
}