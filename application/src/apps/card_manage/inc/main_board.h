

#ifndef __MAIN_BOARD_H__
#define __MAIN_BOARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define MAIN_BOARD_DUMPINFO_MAX_LEN 255

extern gint32 main_board_init(void);
extern gint32 main_board_start(void);
extern gint32 main_board_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 main_board_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 main_board_dump_info(const gchar *path);





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
