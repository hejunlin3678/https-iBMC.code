

#ifndef __FRU_ELABEL_BASE_H__
#define __FRU_ELABEL_BASE_H__
#include "common.h"

void split_extend_with_semicolon(ELABEL_EXTEND_AREA_S *elabel_extend);

void sync_elabel_from_fru(ELABEL_FIELD_72_BYTES_S *elabel_field, const FRU_ELABEL_FORMAT_S fruinfo_label);

void combine_extension_label(GList *input, gchar *output, gint32 out_size, guint8 *num);

void send_ver_change_evt(guint8 evt_code);

void init_elabel_base(void);

void set_one_elabel_property(OBJ_HANDLE handle, const gchar *property_name, const gchar *desc);

gint32 lookup_operator_field_addr_size(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field, gpointer *addr,
    guint32 *field_size, guint32 *field_data_size, guint32 *max_length);

gint32 check_elabel_length(guint32 field_data_size, guint8 offset, guint32 cur_len, guint32 last_len, guint8 req_len,
    guint32 max_length);

void check_and_fill_default_manufacturer(ELABEL_INFO_S *elabel_info);

gint32 get_label_buf_and_len(FRU_PRIV_PROPERTY_S *fru_priv, guint8 area, guint8 field, guint8 **label, guint32 *len);

void set_label_text(FRU_ELABEL_FORMAT_S *label, const guint8 *buf, guint16 *offset, gsize buf_size);

void set_elabel_board_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info);

void set_elabel_product_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info);

void set_elabel_chassis_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info);

gint32 add_one_extend_label(ELABEL_EXTEND_AREA_S *extend_label, guint8 *buf, guint16 length);

gint32 search_equal_symbol_1st_postion(gchar **p, gchar **p1, guint8 *input);

gint32 elabel_area_parse(const guint8 *databuf, guint16 *offset, GList **extension_label, gsize buf_size);

gboolean check_support_elabel(guint8 fru_type);

gint32 fru_elabel_manufacture_syn_hook(OBJ_HANDLE object_handle, gpointer user_data);

void sync_elabel_to_fru(FRU_INFO_S *fru_info, ELABEL_INFO_S *elabel_info);

guint32 copy_extend_label(GList **extend_label, ELABEL_EXTEND_AREA_S *elabel_extend_info);

gboolean is_open_fru_read_back(void);

void open_fru_read_back(void);

gboolean is_support_extern_lable(guint8 fru_id);

#endif
