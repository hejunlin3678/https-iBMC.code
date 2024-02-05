

#ifndef __PASSTHROUGH_CARD_H__
#define __PASSTHROUGH_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 






extern gint32 passthroughcard_init(void);
extern gint32 passthroughcard_start(void);
extern gint passthroughcard_add_object_callback(OBJ_HANDLE obj_handle);
extern gint passthroughcard_del_object_callback(OBJ_HANDLE obj_handle);
extern gint32 passthrough_card_dump_info(const gchar *path);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
