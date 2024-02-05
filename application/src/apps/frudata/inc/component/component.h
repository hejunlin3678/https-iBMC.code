


#ifndef __FRU_COMPONENT_H__
#define __FRU_COMPONENT_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"

gboolean get_fru_present_by_position(guint32 position);

gint32 get_position_info_by_fruid(DFT_COMPONENT_POSN_INFO_S *position_info);

gint32 get_position_info_by_position_id(DFT_COMPONENT_POSN_INFO_S *position_info);

gint32 get_position_info_by_silk(DFT_COMPONENT_POSN_INFO_S *position_info);

gint32 get_device_manu_info(guint8 device_type, guint8 device_num, gchar *puc_manu_info, gint32 infosize,
    gint32 read_offset, gint32 read_len, guint8 *end_flag);

void get_device_num_by_index(guint8 *device_num, guint8 device_index, guint8 dev_type);

gint32 get_device_max_num(guint8 device_type, guint8 *max_mum);

gint32 get_device_presence(guint8 device_type, guint8 device_num, guint8 *device_presence, guint8 *end_flag);

gint32 get_device_health_status(guint8 device_type, guint8 device_num, gint32 *status, guint8 *end_flag);

gint32 get_device_boardid(guint8 device_type, guint8 device_num, guint8 *id);

gint32 get_device_physical_num(guint8 device_type, guint8 device_num, guint8 *id);

gint32 get_device_property(guint8 property_type, guint8 device_type, guint8 device_num, gchar *ret_buff,
    gint32 ret_buff_size, gint32 read_offset, gint32 read_len, guint8 *end_flag);

gint32 get_device_groupid(guint8 device_type, guint8 device_num, gchar *groupid, guint8 *end_flag);

gint32 get_device_fruid(guint8 device_type_parameter, guint8 device_num, guint8 *fru_id);

gint32 correct_device_attri(guint8 *device_type, guint8 *device_ori_type, guint8 *device_num);

gint32 get_product_name(gint is_customize, gchar *product_name, guint32 name_size);

gint32 save_product_uid(guint8 device_type);

gint32 save_product_name(guint8 device_type);

gint32 save_product_newname(guint8 device_type);

gint32 save_product_name_recover(guint8 device_type);

gint32 save_product_newname_recover(guint8 device_type);

gint32 get_device_type_eeprom_block(guint32 offset, guint32 size, guint8 *eep_block);

gint32 ipmi_cmd_set_device_type_str(gconstpointer req_msg, const SET_DEVICE_TYPE_REQ_S *msg_req, guint8 len_limit);

#endif 
