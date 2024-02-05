
#ifndef __SW_CARD_H__
#define __SW_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define SW_CARD_PCB_VER 15
#define SW_CARD_DUMPINFO_MAX_LEN 255


extern gint32 sw_card_init(void);
extern gint32 sw_card_start(void);
extern gint32 sw_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 sw_card_del_object_callback(OBJ_HANDLE object_handle);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
