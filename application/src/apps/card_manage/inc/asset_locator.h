


#ifndef __ASSET_LOCATOR_H__
#define __ASSET_LOCATOR_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 asset_brd_add_object_callback(OBJ_HANDLE obj_hnd);
gint32 asset_brd_del_object_callback(OBJ_HANDLE obj_hnd);
gint32 asset_locator_init(OBJ_HANDLE obj_handle);
gint32 asset_locator_start(OBJ_HANDLE obj_handle);
gint32 asset_locator_dump_info(const gchar *path);
gint32 upgrade_al_board(GSList *input_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif
