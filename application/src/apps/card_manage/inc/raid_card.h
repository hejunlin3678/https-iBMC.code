

#ifndef __RAID_H__
#define __RAID_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define RAID_FAULT 1
#define RAID_OK 0
#define RAID_CARD_PCB_VER 15
#define RAID_CARD_STATE_UPDATE_TIME 2000
#define RAID_CARD_DUMPINFO_MAX_LEN 255

extern gint32 raid_card_init(void);
extern gint32 raid_card_start(void);
extern gint32 raid_card_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 raid_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 raid_card_dump_info(const gchar *path);
extern void raid_set_device_status_default_value(void);

extern gint32 ipmi_set_raid_fault_status(const void *msg_data, gpointer user_data);
extern gint32 set_raid_fault_status_by_bios(guint8 bus, guint8 device, guint8 function, guint8 device_status);

extern gint32 card_manage_set_pfa_event(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
