
#ifndef __CIC_CARD_H__
#define __CIC_CARD_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




#define CIC_CARD_PCB_VER (15)
#define CIC_CARD_DUMPINFO_MAX_LEN (255)


extern gint32 cic_card_init(void);
extern gint32 cic_card_start(void);
extern gint32 cic_card_add_object_callback(OBJ_HANDLE object_handle);
extern gint32 cic_card_del_object_callback(OBJ_HANDLE object_handle);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
