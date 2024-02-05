

#ifndef __RISER_CARD_H__
#define __RISER_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define RISER_CARD_DUMPINFO_MAX_LEN 255

extern gint32 riser_card_init(void);
extern gint32 riser_card_start(void);
extern gint32 riser_card_del_object_callback(OBJ_HANDLE object_handle);
extern gint32 riser_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 riser_card_dump_info(const gchar *path);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
