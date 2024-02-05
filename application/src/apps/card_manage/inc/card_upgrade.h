

#ifndef __CARD_UPGRADE_H__
#define __CARD_UPGRADE_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define BOM_ID_PCIE_CARD "14140130"
#define BOM_ID_PCIE_DISK "14140224"
#define BMC_XML_PATH "/opt/pme/extern/profile"
#define CARD_XML_FILE_PATH "/data/opt/pme/conf/profile"
#define MAX_PROPERTY_LEN 64
#define PCIE_CARD_KEY_LEN 4
#define MAX_CARD_COUNT 200
#define RET_VERIFY_SUCCESS 1
#define RET_VERIFY_FAILED 2
#define RET_SKIP_FILE 3

gint32 upgrade_card_xml(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

typedef struct tag_saved_pcie_info {
    guint16 vid;
    guint16 did;
    guint16 svid;
    guint16 sdid;
    guint8 status_by_bios;
    guint8 status;
} SAVED_PCIE_INFO_S;

typedef struct tag_pcie_xml_up_param {
    guint8 slot_id[MAX_CARD_COUNT];
    guint8 pcie_index[MAX_CARD_COUNT];
    guint8 pcie_count;
    guint32 pcie_type;
} PCIE_XML_UP_PARAM_S;
typedef struct tag_validate_kv_pairs {
    GSList *keys;
    GSList *expected_vals;
    guint32 count;
} VALIDATE_KV_PAIRS_S;

typedef struct tag_upgrade_param {
    const gchar *filename;
    guint8 card_type;
    gboolean need_verify;
} UPGRADE_PARAM_S;

typedef gint32 (*UPGRADE_HANDLER_FUNC)(UPGRADE_PARAM_S *up_param);
typedef gint32 (*VERIFIER_FUNC)(const gchar *filename);
typedef struct tag_upgrade_func {
    UPGRADE_HANDLER_FUNC handler;
    VERIFIER_FUNC verifier;
} UPGRADE_FUNC_S;

typedef struct tag_upgrade_table_element {
    const gchar *bom_id;
    guint8 card_type;
    UPGRADE_HANDLER_FUNC handler;
    VERIFIER_FUNC verifier;
} UPGRADE_TABLE_ELEMENT_S;

typedef enum tag_card_type {
    UP_PCIE_CARD_TYPE = 0x01,
    UP_UNKNOWN_TYPE = 0xff
} CARD_TYPE;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
