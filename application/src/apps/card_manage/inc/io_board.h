

#ifndef __IO_BOARD_H__
#define __IO_BOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

enum IoBrdMntrState {
    IOBRD_MNTR_INIT = 0,
    IOBRD_MNTR_WAIT,
    IOBRD_MNTR_UPDATE,
    IOBRD_MNTR_FAIL,
    IOBRD_MNTR_EXIT // 占位符，状态机代码暂不使用
};

#define IOBRD_MNTR_ITEM_PERIOD 0
#define IOBRD_MNTR_ITEM_STATIC 1

typedef gint32 (*io_board_mntr_proc)(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info);
typedef struct tag_IoBrdMntrItem {
    guint8 is_static;
    io_board_mntr_proc func;
} IOBRDMNTRITEM;

typedef struct tag_IoBrdMntrCB {
    PM_CHIP_INFO *pm_chip_info;
    OBJ_HANDLE handle;
    enum IoBrdMntrState state;
    guint8 id;
    // 支持下电停止轮询
    guint8 last_power_state;
    guint8 power_state;
    // 初始化失败重试
    guint8 retry_cnt;
    // 单次获取报文更新标志，支持最大256个回调
    guint8 item_table_len;
    const IOBRDMNTRITEM *item_table;
    guint32 item_updated[8];
    // 下电恢复默认值回调，当前恢复代码量较少，只使用一个回调
    void (*set_default_proc)(OBJ_HANDLE obj_handle, guint16 mask);
    gint32 (*update_firm_ver_proc)(OBJ_HANDLE obj_handle, PM_CHIP_INFO *info, guint8 *updated);
} IOBRDMNTRCB;


#define IO_BOARD_DUMPINFO_MAX_LEN 255

extern gint32 io_board_init(void);
extern gint32 io_board_start(void);
extern gint32 io_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 io_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 io_board_dump_info(const gchar *path);
 
gint32 io_board_set_firmware_version(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
