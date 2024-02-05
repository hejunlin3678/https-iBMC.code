

#ifndef __PCIE_CARD_PARSE_BDF_THREAD_H__
#define __PCIE_CARD_PARSE_BDF_THREAD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define VENDER_OFFSET   16

typedef struct pcie_parse_info {
    
    PCIE_CARD_GROUP_INFO_S *pcie_card_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info;

    
    PCIE_CARD_GROUP_INFO_S *pcie_group_info;
    PCIE_CARD_GROUP_SUB_INFO_S *pcie_group_sub_info;
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_group_bdf_info;

    guint8 pcie_type;
    guint8 max_slotid;
} PCIE_PARSE_INFO;



void _deleting_pos_list_remove_all(void);
void _add_item_to_deleting_pos_list(guint32 pos);
gint32 pcie_card_load_xml_retry_enable(void);
gint32 pcie_get_slot_by_index(guint8 pcie_type, guint8 pcie_index, guint8 *slot_idx);
gint32 pcie_card_get_cardaction_handle(guint8 slot, guint8 type, OBJ_HANDLE *obj_handle);
gint32 pcie_card_get_reload_enable(guint8 slot_id, guint8 *pciecard_reload);
gint32 pcie_card_get_connector_handle(guint8 slot_id, OBJ_HANDLE *obj_handle, guint8 connector_type);
gint32 disk_connector_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type);
gint32 nvme_vpd_connector_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type);
gint32 card_connector_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type);
gint32 pcie_card_load_xml(OBJ_HANDLE obj_handle, guint32 board_id, guint32 aux_id, guint8 state, guint8 type);
void pcie_card_log_operation_log(guint8 type, guint8 slot_id, guint16 vender_id, guint16 device_id, guint8 option);
guint32 _get_deleting_pos_list_length(void);
gint32 pcie_card_update_bdf(PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info, guint32 boardid, guint32 auxid,
    guint8 slot_id, guint8 type);
gint32 match_v5_partition_product_peci_enable(void);
gint32 pcie_get_did_vid_from_me(const guint8 bdf_index, const guint8 pcie_index, PCIE_CARD_GROUP_INFO_S *pcie_card_info,
    PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info, PCIE_CARD_GROUP_SUB_INFO_S *pcie_card_sub_info);


gint32 pcie_card_parse_bdf_thread_init(PCIE_CARD_PARSE_PERSAVE_DATA_S *pcie_disk_persave_data_ptr,
    PCIE_CARD_PARSE_PERSAVE_DATA_S *pcie_card_persave_data_ptr,
    PCIE_CARD_PARSE_PERSAVE_DATA_S *ocp_card_persave_data_ptr);

gint32 pcie_card_parse_bdf_thread_run(void *bdf_info);
gint32 pcie_slot_get(guint8 pcie_type, guint8 original_index, guint8 *slot_id, guint8 *pcie_index);
void pcie_card_update_ft_resid(PCIE_CARD_GROUP_BDF_INFO_S *pcie_card_bdf_info,
    PCIE_CARD_GROUP_BDF_INFO_S *ocp_card_bdf_info);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
