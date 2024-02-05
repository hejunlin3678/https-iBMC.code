

#ifndef __GPU_BOARD_H__
#define __GPU_BOARD_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define GPU_REG_INFO_DUMP_FILE_NAME "GPUBoardCPLDRegInfo"
#define GPU_REG_INFO_HEADER_FORMAT "GPU Board%u(board id is %#x) Reg Info\r\n"
#define GPU_BOARD_REG_DUMP_PAGE_SIZE 16


#define GPU_BOARD_DUMPINFO_MAX_LEN 255

extern gint32 gpu_board_init(void);
extern gint32 gpu_board_start(void);
extern gint32 gpu_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 gpu_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 gpu_board_dump_info(const gchar *path);
extern gint32 process_reg_action_with_start_condition(OBJ_HANDLE obj_handle, gpointer user_data);
extern gint32 process_reg_action_with_dynamic_condition(OBJ_HANDLE object_handle, gpointer user_data,
    gchar *property_name, GVariant *property_value);
extern gint32 process_reg_action_with_hotswap_condition(OBJ_HANDLE obj_handle, gpointer user_data);
extern gint32 card_manage_start_reg_action(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 card_manage_check_reg_action(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 init_each_repeater_mgnt(OBJ_HANDLE handle, gpointer data);
gint32 component_default_power_ctrl(guint8 device_type, guint16 device_num, guint32 device_pos, guint8 cmd_code,
    guint8 *comp_code);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif
