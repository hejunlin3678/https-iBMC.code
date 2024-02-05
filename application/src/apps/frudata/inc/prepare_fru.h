

#ifndef __FRU_PREPARE_FRU_H__
#define __FRU_PREPARE_FRU_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"

#define EXTEND_ELABEL_KEY_CLEI_CODE     "CLEICode="
#define EXTEND_ELABEL_KEY_BOM_ID        "BOM="
#define EXTEND_ELABEL_KEY_MODEL         "Model="
#define EXTEND_ELABEL_KEY_DESC          "Description="
#define EXTEND_ELABEL_KEY_ISSUE_NUM     "IssueNumber="
#define PRODUCT_ELABEL_KEY_BAR_CODE     "BarCode="
#define PRODUCT_ELABEL_KEY_ITEM         "Item="
#define PRODUCT_ELABEL_KEY_VEN_NAME     "VendorName="
#define PRODUCT_ELABEL_KEY_BOARD_TYPE   "BoardType="
#define BOARD_ELABEL_KEY_MANUFACTURED   "Manufactured="

#define MAX_ELABEL_FRAME_NUM 5
#define GET_ELABEL_DATA_SIZE_PER_FRAME 228
#define MAX_ELABLE_LEN 1024

#pragma pack(1)

typedef struct tag_pangea_card_ipmi_msg_head {
    guint8 slot_id;
    guint8 fru_type0;
    guint8 fru_type1;
    guint8 fru_id;
    guint8 sdr_type;
    guint16 total_num;
    guint16 cur_num;
} IPMIMSG_ELABELHEAD;

#pragma pack()
#define SCAN_STARTING 0x10
#define SCAN_STARTED  0x11
#define SCAN_STOPPING 0x20
#define SCAN_STOPPED  0x21

void init_prepare_fru(void);

gint32 init_fru(OBJ_HANDLE object_handle, gpointer user_data);

void enable_fru_init(void);

void disable_fru_init(void);

void update_fru_in_standby_smm(gpointer data);

gint32 frudata_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);

gint32 frudata_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);

gint32 frudata_add_object_complete_callback(guint32 position);

gint32 frudata_del_object_complete_callback(guint32 position);

void check_eeprom_write_status(void);

gint32 create_fru_data(FRU_INFO_S *fru_info, const guint8 *fru_buf);

gint32 on_fru_obj_add_event(OBJ_HANDLE object_handle);

gboolean is_fru_initialized(OBJ_HANDLE object_handle);

gboolean is_support_fru_id(guint8 fruid);
gboolean check_fru_id_is_valid(guint8 fru_id);

gint32 frudev_read_arcard_fru_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

gboolean get_fru_callback_flag(void);
void set_fru_callback_flag(gboolean flag);
#endif 
