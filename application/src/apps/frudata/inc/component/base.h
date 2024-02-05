


#ifndef __FRU_COMPONENT_BASE_H__
#define __FRU_COMPONENT_BASE_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"

gint32 init_component(OBJ_HANDLE obj_handle, gpointer user_data);

void init_component_base(void);

void lock_connector(void);

void unlock_connector(void);

gint32 position_compare(gconstpointer a, gconstpointer b);

GSList *get_connectors(void);

GSList **get_connectors_addr(void);

void free_connectors(void);

void snprintf_with_unknown(gchar *puc_manu_info, gint32 info_len, gint32 value, const char *str);

void snprintf_with_guint32(gchar *puc_manu_info, gint32 info_len, guint32 value, const char *str);

gboolean is_device_type_valid(guint8 device_type);

void frudata_convert_mac_hex2string(guchar *input, gchar *output, gint32 out_size, guint8 data_len);

void frudata_mac_str2int(gchar *input, guchar *output, guint32 out_size, guint8 *output_len);

gint32 component_device_num_compare_fun(gconstpointer a, gconstpointer b);

gint32 get_sdi_card_handle_by_logic_num(guint8 logicalNum, OBJ_HANDLE *obj_handle);

guint8 get_blade_location(FRU_PRIV_PROPERTY_S *fru_priv);

gint32 fill_device_physical_num(guint8 mezz_location, guint8 device_num, guint8 slot_id, guint8 physical_num,
    guint8 *id);

gint32 check_device_type(guint8 comp_device_type);

gboolean check_device_num_range(guint8 device_type, guint8 device_num);

gboolean is_manu_id_valid(const guint8 *manufacture_id);

gint32 is_component_type_support_fru(guint8 component_device_type);

gint32 create_component_obj_add_task(void);

gint32 send_component_obj_add_msg_to_queue(OBJ_HANDLE obj_handle, guint8 option);

void update_ps_ref_component_device_name(OBJ_HANDLE obj_handle);

#endif 
