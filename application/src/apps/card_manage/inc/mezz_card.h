

#ifndef __MEZZ_CARD_H__
#define __MEZZ_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define MEZZ_CARD_PCB_VER 15
#define MEZZ_CARD_DUMPINFO_MAX_LEN 255


extern gint32 mezz_card_init(void);
extern gint32 mezz_card_start(void);
extern gint32 mezz_card_ipmi_get_pcie_width_info(const void *msg_data, gpointer user_data);
extern gint32 mezz_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 mezz_card_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 mezz_card_dump_info(const gchar *path);
extern gint32 mezz_set_mezz_mac_addr(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 card_manage_set_fdm_fault_state(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern void mezz_card_set_device_status_default_value(void);
extern gint32 mezz_card_ipmi_set_health_status(const void *msg_data, gpointer user_data);


extern gint32 pcie_mgnt_init_info(OBJ_HANDLE obj_handle, gpointer user_data);
extern gint32 pcie_card_update_info(OBJ_HANDLE obj_handle, gpointer user_data);





typedef enum tag_mezzcard_hw_stat {
    MEZZCARD_HW_HEALTH_STATUS = 0,
    MEZZCARD_HW_STAT_RESERVE
} MEZZCARD_HW_STAT;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
