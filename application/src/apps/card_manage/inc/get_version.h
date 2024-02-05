

#ifndef __GET_VERSION_H__
#define __GET_VERSION_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 



#define CARD_PCB_VER_LEN 15
#define CARD_LOGIC_VER_LEN 15
#define CARD_PLUG_IN 0
#define CARD_PLUG_OUT 1
#define CARD_BUTTON_PRESS 2

gint32 get_logic_version(OBJ_HANDLE handle, const gchar *logic_id, const gchar *logic_ver);
gint32 get_pcb_version(OBJ_HANDLE handle, const gchar *pcb_id, const gchar *pcb_ver);
void card_manage_log_operation_log(gchar *card_name, OBJ_HANDLE object_handle, const gchar *property_name,
    const gchar *device_name_property, guint8 option);


typedef struct pcbver_info {
    OBJ_HANDLE obj_handle;
    gchar property_pcb_id[MAX_OBJECT_NAME_LEN];
    gchar property_pcb_ver[MAX_OBJECT_NAME_LEN];
} PCBVER_INFO;

typedef struct logicver_info {
    OBJ_HANDLE obj_handle;
    gchar property_logic_id[MAX_OBJECT_NAME_LEN];
    gchar property_logic_ver[MAX_OBJECT_NAME_LEN];
} LOGICVER_INFO;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
