
#ifndef APPLICATION_SRC_APPS_FRUDATA_INC_MEDIA_PORT_RATE_H
#define APPLICATION_SRC_APPS_FRUDATA_INC_MEDIA_PORT_RATE_H

#include <ctype.h>
#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define CPLD_BOARD_PORT_SPEED_ADDR         0x156
#define CPLD_BOARD_CPU1_PORT_SPEED_ADDR    0x155

#define RESP_LENS 2

#define RETRY_3TIMES 3

#define INVALID_VALUE8 0xFF


#define FANCHIP_OFFSET_ELABEL 0x34

#define CHASSIS_NUMBER_MAX 0xffe 


#define DFL_FANCHIP_ELABEL_UNINITIALIZED (-5019)

typedef struct tagBOARD_GET_PORT_SPEED_S {
    guint8 comp_code;
    guint8 port_speed;
} BOARD_GET_PORT_SPEED_S;

typedef struct tagGET_MULTIPLE_CPUS_PORT_SPEED_S {
    guint8 fru_id;
    guint8 fru_type_0;
    guint8 fru_type_1;
    guint8 device_id;
    guint8 sdr_type;
    guint8 cpuid;
} GET_MULTIPLE_CPUS_PORT_SPEED_S;

typedef struct tagBOARD_SET_PORT_SPEED_REQ_S {
    guint8 fru_id;
    guint8 fru_type_0;
    guint8 fru_type_1;
    guint8 device_id;
    guint8 sdr_type;
    guint8 port_speed;
    guint8 cpuid;
} BOARD_SET_PORT_SPEED_REQ_S;


void init_storage_persis_prop(void);
guint8 get_board_port_speed(void);
gint32 set_board_port_speed(guint8 port_speed);
guint32 get_cpu_num(void);
guint8 get_lom_port_speed_by_cpuid(guint8 cpuid);
gint32 set_cpu_lom_speed(guint8 cpuid, guint speed);
gint32 set_lom_speed_to_cpld(guint8 cpuid, guint speed);
extern guint8 g_board_port_speed;
void pange_storage_node_persistent_value_init(void);
void pyload_set_key_info(void);
gint32 payload_set_eeprom_key_info(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 method_set_eep_stop_routine_flag(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_eeprom_status(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_cpld_status_to_fail(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif 