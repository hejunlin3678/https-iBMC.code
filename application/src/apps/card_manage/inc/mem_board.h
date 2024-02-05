

#ifndef __MEM_BOARD_H__
#define __MEM_BOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define MEM_BOARD_DUMPINFO_MAX_LEN 255
#define MEM_BRD_MIGRATION_OVERTIME 5
#define MEM_BRD_MAX_NODE_NUM 16
#define MEM_BRD_MAX_STRING_LENGTH 128
#define MEM_BRD_SPARE_ENABLE 1
#define MEM_BRD_SPARE_DISABLE 0
#define MEM_BRD_SPARE_NOT_EXIST 0xff
#define MEM_BRD_DEVICE_STATUS_LEN 6

extern gint32 mem_board_init(void);
extern gint32 mem_board_start(void);
extern gint32 mem_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 mem_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 mem_board_sync_set_dev_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern void mem_board_set_device_status_default_value(void);
extern gint32 mem_board_dump_info(const gchar *path);
extern gint32 mem_board_set_migration_enable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 mem_board_ipmi_set_device_status(const void *msg_data, gpointer user_data);
extern gint32 mem_board_get_migration_status(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 mem_board_set_migration_refresh(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 mem_board_migration_slotid_valid_check(guint8 src_node_id, guint8 det_node_id);



typedef enum tag_membrd_hot_remove_stat {
    MEMBRD_HOT_REMOVE_TYPE0 = 0, 
    MEMBRD_HOT_REMOVE_TYPE1 = 1, 
    MEMBRD_HOT_REMOVE_TYPE2 = 2, 
    MEMBRD_HOT_REMOVE_TYPE3 = 3, 
    MEMBRD_HOT_REMOVE_TYPE4 = 4, 
    MEMBRD_HOT_REMOVE_RESERVE
} MEMBRD_HOT_REMOVE;

typedef enum tag_membrd_hot_plug_stat {
    MEMBRD_HOT_PLUG_TYPE0 = 0, 
    MEMBRD_HOT_PLUG_TYPE1 = 1, 
    MEMBRD_HOT_PLUG_TYPE2 = 2, 
    MEMBRD_HOT_PLUG_TYPE3 = 3, 
    MEMBRD_HOT_PLUG_TYPE4 = 4, 
    MEMBRD_HOT_PLUG_RESERVE
} MEMBRD_HOT_PLUG;

typedef enum tag_membrd_p_type_e {
    MEMBRD_P_TYPE_BYTE = 0,
    MEMBRD_P_TYPE_UINT16,
    MEMBRD_P_TYPE_UINT32,
    MEMBRD_P_TYPE_DOUBLE,
    MEMBRD_P_TYPE_STRING,
    MEMBRD_P_TYPE_INT16
} MEMBRD_P_TYPE_E;

typedef enum tag_membrd_migration_e {
    MEMBRD_MIGRATION_STAT0 = 0,
    MEMBRD_MIGRATION_STAT1,
    MEMBRD_MIGRATION_STAT2,
    MEMBRD_MIGRATION_STAT3,
    MEMBRD_MIGRATION_STAT4,
    MEMBRD_MIGRATION_STAT5,
    MEMBRD_MIGRATION_STAT6,
    MEMBRD_MIGRATION_STAT7,
    MEMBRD_MIGRATION_STAT8,
    MEMBRD_MIGRATION_STAT9,
    MEMBRD_MIGRATION_STAT10,
    MEMBRD_MIGRATION_STAT11,
    MEMBRD_MIGRATION_STAT12,
    MEMBRD_MIGRATION_RESERVE
} MEMBRD_MIGRATION_E;

typedef enum tag_membrd_dev_stat {
    MEMBRD_DEV_STAT_TYPE0 = 4,
    MEMBRD_DEV_STAT_TYPE1 = 7,
    MEMBRD_DEV_STAT_TYPE2 = 11,
    MEMBRD_DEV_STAT_TYPE3 = 12,
    MEMBRD_DEV_STAT_TYPE4 = 14,
    MEMBRD_DEV_STAT_TYPE5 = 17,
    MEMBRD_DEV_STAT_TYPE6 = 6,
    
    MEMBRD_DEV_STAT_TYPE254 = 0xfe, // 设置内存板PROPERTY_MEMBOARD_CONFIG_ERROR_CODE,上报config error、vmse error
    
    MEMBRD_DEV_STAT_RESERVE
} MEMBRD_DEV_STAT;

typedef enum tag_membrd_hw_stat {
    MEMBRD_HW_STAT_TYPE0 = 0,
    MEMBRD_HW_STAT_TYPE1 = 1,
    MEMBRD_HW_STAT_TYPE2 = 2,
    MEMBRD_HW_STAT_TYPE3 = 3,
    MEMBRD_HW_STAT_RESERVE
} MEMBRD_HW_STAT;

typedef enum tag_membrd_migration_t {
    MEMBRD_MIGRATION_TMOUT_COUNT = 0,
    MEMBRD_MIGRATION_PGS_STATUS,
    MEMBRD_MIGRATION_PERCENTAGE,
    MEMBRD_MIGRATION_SRC_NODE,
    MEMBRD_MIGRATION_DES_NODE,
    MEMBRD_MIGRATION_T_RESERVE
} MEMBRD_MIGRATION_T;

typedef enum tag_membrd_migration_s {
    MIGRATION_END = 0,
    MIGRATION_START,
    MIGRATION_BEGIN_ONLINE,
    MIGRATION_ONLINE_SUCCESS,
    MIGRATION_ONLINE_FAIL,
    MIGRATION_INPROGRESS,
    MIGRATION_SUCCESS,
    MIGRATION_FAIL,
    MIGRATION_PATROLSCRUB_INPROGRESS,
} MEMBRD_MIGRATION_S;

typedef struct tag_membrd_mig_progress {
    guint8 tmout_count;
    guint8 migration_state;
    guint8 percentage;
    guint8 cur_node_id;
    guint8 cur_spare_id;
} MEMBRD_MIG_PROGRESS;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
