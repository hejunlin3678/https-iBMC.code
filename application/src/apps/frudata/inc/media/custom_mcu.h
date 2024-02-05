

#ifndef __CUSTOM_MCU_H__
#define __CUSTOM_MCU_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

typedef struct FRU_LABEL_FUNC_TBL {
    guint8 dev_type;
    gint32 (*get_fru_info)(guint8 dev_num, FRU_FILE_S *fru_file);
} FRU_LABEL_FUNC_TBL_S;

#define EXTEND_ELABEL_KEY_CLEI_CODE     "CLEICode="
#define EXTEND_ELABEL_KEY_BOM_ID        "BOM="
#define EXTEND_ELABEL_KEY_MODEL         "Model="
#define EXTEND_ELABEL_KEY_DESC          "Description="
#define PRODUCT_ELABEL_KEY_BAR_CODE     "BarCode="
#define PRODUCT_ELABEL_KEY_ITEM         "Item="
#define PRODUCT_ELABEL_KEY_VEN_NAME     "VendorName="
#define PRODUCT_ELABEL_KEY_BOARD_TYPE   "BoardType="
#define PRODUCT_ELABEL_KEY_ISSUE_NUM    "IssueNumber="
#define BOARD_ELABEL_KEY_MANUFACTURED   "Manufactured="

void change_pangea_card_elabel_format(const guint8 *src_data, guint32 src_data_len, OBJ_HANDLE fru_obj,
    FRU_INFO_S *fru_info);

gint32 _write_elabel_to_mcu_custom(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);
gint32 read_elabel_from_custom_mcu(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif 