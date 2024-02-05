

#ifndef __FRU_ELABEL_H__
#define __FRU_ELABEL_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"

#define MSM_PRIMARY_STATE          1
#define MSM_SECONDARY_STATE        0

typedef struct tag_ipmi_get_shelf_data_resp_s {
    guint8 completion;
    guint8 manu_id[3]; 
    guint8 end_flag;
    guint8 content[1];
} IPMIMSG_GET_SHELF_DATA_RESP_S;

gint32 send_write_elabel(guint8 fru_id, guint8 area_id, guint8 field_id, const gchar *mmc_elabel);

gint32 get_fru_priv_data_by_id(guint8 fruid, FRU_PRIV_PROPERTY_S **fru_priv_data, OBJ_HANDLE *fru_handle);

gint32 read_elabel_info(guint8 *resp_data, guint32 data_len, guint32 *resp_len, guint8 *end_flag,
    FRU_PRIV_PROPERTY_S *fru_priv, const DFT_READ_ELABEL_REQ_S *dft_read_elabel_req);

gint32 send_peripheral_elabel_cmd(OBJ_HANDLE obj_handle, const guint8 *input, guint32 inputlen, guint8 *output,
    guint32 *outputlen);

gint32 get_sync_elabel_state(guint8 *state);

gint32 get_node_index_suffix(gchar *suffix_buf, gint8 buf_len);

void upgrade_elabel(guint8 fru_id);

gint32 handle_extra_elabel_info(FRU_PRIV_PROPERTY_S *fru_priv);

gint32 set_elabel_property(FRU_PROPERTY_S *fru_prop, const FRU_INFO_S *fru_info);

gint32 get_elabel(FRU_PRIV_PROPERTY_S *fru_priv, ELABEL_INFO_S *elabel);

void check_elabel_from_eeprom(FRU_PRIV_PROPERTY_S *fru_priv_property, gpointer elabel_tmp);

gint32 fru_elabel_product_combine(OBJ_HANDLE object_handle, gpointer user_data);

gint32 update_elabel_info(FRU_PRIV_PROPERTY_S *fru_priv, const DFT_WRITE_ELABEL_REQ_S *dft_write_elabel_req,
    guint8 req_len);

gint32 update_ps_elabel(OBJ_HANDLE object_handle, const FRU_INFO_S *fru_info);

gint32 update_system_area(FRU_PRIV_PROPERTY_S *fru_priv);

void elabel_internal_area_paser(FRU_INTERNAL_AREA_S *internal_area, const guint8 *databuf,
    const FRU_INFO_HEADER_S *header, gsize buf_size);

void elabel_chassis_area_paser(FRU_CHASSIS_AREA_S *chassis_area, const guint8 *databuf, gsize buf_size);

void elabel_board_area_paser(FRU_BOARD_AREA_S *board_area, const guint8 *databuf, gsize buf_size);

void elabel_product_area_paser(FRU_PRODUCT_AREA_S *product_area, const guint8 *databuf, gsize buf_size);

gint32 update_elabel_product_assettag(GSList *caller_info, guint8 fruid, const gchar *assettag);

void update_file_id(FRU_PRIV_PROPERTY_S *fru_priv);

void split_extend_with_null(ELABEL_EXTEND_AREA_S *elabel_extend);

void write_extend_elabel(OBJ_HANDLE object_handle, guint32 offset, ELABEL_EXTEND_AREA_S *elabel_extend, guint32 length);

void init_update_elabel_lock(void);
void task_get_chassis_sn(void);
gint32 update_extend_elabel(ELABEL_EXTEND_AREA_S *extend_buf, guint8 *input, guint16 length);
void init_board_fru_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info);
void init_extend_elabel_from_data(char *src_data, guint16 data_size, FRU_INFO_S *fru_info);
void get_correspond_elabel_data(gchar *all_elabel_data, const gchar *key_buf, FRU_ELABEL_FORMAT_S *elabel_buf);
void get_manufacture_time(const gchar *src_data, FRU_INFO_S *fru_info);
void get_extend_elabel_data_by_key(gchar *elabel_data, const gchar *key_buf, ELABEL_EXTEND_AREA_S *extend_buf);
#endif
