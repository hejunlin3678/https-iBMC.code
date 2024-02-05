

#ifndef __CPU_BOARD_H__
#define __CPU_BOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define CPU_BOARD_DUMPINFO_MAX_LEN 255

typedef enum tag_cpubrd_hot_remove_stat {
    CPUBRD_HOT_REMOVE_TYPE0 = 0, 
    CPUBRD_HOT_REMOVE_TYPE1 = 1, 
    CPUBRD_HOT_REMOVE_TYPE2 = 2, 
    CPUBRD_HOT_REMOVE_TYPE3 = 3, 
    CPUBRD_HOT_REMOVE_TYPE4 = 4, 
    CPUBRD_HOT_REMOVE_RESERVE
} CPUBRD_HOT_REMOVE;

typedef enum tag_cpubrd_hot_plug_stat {
    CPUBRD_HOT_PLUG_TYPE0 = 0, 
    CPUBRD_HOT_PLUG_TYPE1 = 1, 
    CPUBRD_HOT_PLUG_TYPE2 = 2, 
    CPUBRD_HOT_PLUG_TYPE3 = 3, 
    CPUBRD_HOT_PLUG_TYPE4 = 4, 
    CPUBRD_HOT_PLUG_RESERVE
} CPUBRD_HOT_PLUG;

typedef enum tag_cpubrd_ras_event_stat {
    CPUBRD_HOT_REMOVE_START = 0,
    CPUBRD_HOT_REMOVE_CPU_OFFLINE = 1,
    CPUBRD_HOT_REMOVE_CPUBOARD_POWEROFF = 2,
    CPUBRD_HOT_REMOVE_FINISH = 3,
    CPUBRD_HOT_PLUG_START = 4,
    CPUBRD_HOT_PLUG_POWERON = 5,
    CPUBRD_HOT_PLUG_CPU_ONLINE = 6,
    CPUBRD_HOT_PLUG_CPU_RESET = 7,
    CPUBRD_HOT_PLUG_FINISH = 8,
    CPUBRD_HOT_RAS_RESERVE
} CPUBRD_RAS_EVENT;

typedef enum tag_cpubrd_hw_stat {
    CPUBRD_HW_STAT_TYPE0 = 0,
    CPUBRD_HW_STAT_TYPE1 = 1,
    CPUBRD_HW_STAT_TYPE2 = 2,
    CPUBRD_HW_STAT_TYPE3 = 3,
    CPUBRD_HW_STAT_RESERVE
} CPUBRD_HW_STAT;

typedef struct tag_cpu_ras_event_s {
    guint8 cpubrd_slot_id;
    guint8 event;
} CPU_RAS_EVENT_S;

typedef struct rsp_get_ras_mission_status {
    guint8 compcode;
    guint8 manufacturer_id_h;
    guint8 manufacturer_id_m;
    guint8 manufacturer_id_l;
    guint8 component_info;
    guint8 action;
    guint8 id_number;
    guint8 status;
} RSP_GET_MISSION_STATUS_S;

typedef enum tag_ras_event_stat {
    RAS_EVENT_BUSY = 0,
    RAS_EVENT_CPU_HOT_PLUG = 1,
    RAS_EVENT_MEM_HOT_PLUG = 4,
    RAS_EVENT_INVALID_STAT
} RAS_EVENT_STAT;



extern gint32 cpu_board_init(void);
extern gint32 cpu_board_start(void);
extern gint32 cpu_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 cpu_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 cpu_board_dump_info(const gchar *path);
extern gint32 cpu_board_ipmi_set_device_status(const void *msg_data, gpointer user_data);
extern gint32 ipmi_get_ras_mission_status(const void *msg_data, gpointer user_data);
extern gint32 cpubrd_hot_operate_sync_event(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 cpubrd_ras_init_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 cpu_board_set_bios_fw_ver(const void *req_msg, gpointer user_data);
gint32 cpu_board_get_bios_fw_ver(const void *req_msg, gpointer user_data);
extern guint32 ras_mission_status;





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
