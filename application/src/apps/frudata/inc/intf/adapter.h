


#ifndef __FRU_ADAPTER_H__
#define __FRU_ADAPTER_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"

gint32 method_set_ver_change(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_component_set_health(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_fru_set_mezzcard_macaddr(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_elabel_set_product_assettag(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_fru_elabel_syn_elable(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_update_comp_dev_name(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_fru_init_from_mcu(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_ocp_fru_set_pcb_ver(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_read_cafe_record(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

gint32 method_write_cafe_record(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_elabel_property_sync(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_fru_set_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_fru_get_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_set_canbus_dev_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_get_canbus_dev_eeprom_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_update_mainboard_fru_slot_id(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 method_fru_set_eeprom_raw_data(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_elabel_sync_property(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

#endif 
