


#ifndef APPLICATION_SRC_APPS_FRUDATA_INC_INTF_IPMI_H
#define APPLICATION_SRC_APPS_FRUDATA_INC_INTF_IPMI_H


#include "common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define FRUDATA_START_COMMAND 0xAA
#define FRUDATA_GET_STATUS 0x00
#define KEY_VALUE_DEVISION '='
#define UPDATA_ELABEL_OK 0x00
#define MAX_LOG_BUF_SIZE 512
#define INVALID_FRU_ID 0xff

#define PRESENCE_STATUS 2
#define HEALTH_STATUS 3
#define SUB_DEV_MAX_NUM 4
#define AVAILABILITY 5
#define INFO_BOARDID 6
#define INFO_PHYSICALNUMBER 7
#define INFO_LOCATION 8
#define INFO_FUNCTION 9
#define INFO_DEVICENAME 10
#define INFO_GROUPID 11
#define INFO_FRU_DEVICEID 0x10
#define INFO_PCIE_CONNECT 0x11
#define INFO_SDI_STORAGE_IP 0x16
#define INFO_SDI_BOOT_ORDER 0x17
#define INFO_SDI_OS_LOADED 0x18
#define INFO_SDI_SET_IP_STATUS 0x19
#define INFO_LSW_SET_PORT 0x20
#define INFO_RACK_UCOUNT 0x1C
#define INFO_RACK_EMPTY_SN 0x1D
#define INFO_RACK_TYPICAL_CONF_SN 0x1E
#define INFO_RACK_MODEL 0x1F
#define INFO_SDI_EP_INIT_STATE 0x21
#define INFO_MANU 99
#define TYPE_FRU_ID 1
#define TYPE_POSITION_ID 2
#define TYPE_SILK_TEXT 3

#define RESERVED_VALUE 0
#define CUSTOMIZE_RSP_CNT 3
#define TAISHAN_CUSTOMIZE_FLG 2
#define KUNPENG_CUSTOMIZE_FLG 3
#define GET_CUSTOMIZE_FLG_RSP 2
#define RESP_DATA_HEADER_LEN 5
#define END_OF_LIST_INDEX 4
#define DATA_START_INDEX 5

#pragma pack(1)
typedef struct tag_get_fru_area_req_s {
    guint8 fru_id;
} GET_FRU_AREA_REQ_S;

typedef struct tag_get_fru_area_resp_s {
    guint8 completion;
    guint16 size;
    guint8 word_access : 1;
    guint8 reserve : 7;
} GET_FRU_AREA_RESP_S;

typedef struct tag_update_clear_elabel_req_s {
    guint8 subcmd;
    guint8 fru_id;
    guint8 option;
} UPDATE_CLEAR_ELABEL_REQ_S;

typedef struct tag_update_clear_elabel_resp_s {
    guint8 comp_code;
    guint8 progress;
    guint8 result;
} UPDATE_CLEAR_ELABEL_RESP_S;

typedef struct tag_custom_req_s {
    guint8 subcmd;
    guint8 para_select;
    guint8 custom_flag;
} CUSTOM_REQ_S;

#pragma pack()

typedef enum tag_BLADE_INFO_PARA {
    BLADE_INFO_PARA_PRESENT = 1,
    BLADE_INFO_PARA_BOARDID = 2,
    BLADE_INFO_PARA_RESERVE
} BLADE_INFO_PARA;

typedef gint32 (*__get_device_info_func)(const void *msg_data, guint8 resp_data[DEVICE_INFO_LEN + 6],
    guint8 device_type, guint8 device_ori_type, guint8 device_num, guint8 max_num);

typedef struct tag_get_device_info {
    guint8 cond_filed;
    __get_device_info_func exec_func;
} GET_DEVICE_INFO_S;

typedef struct tag_get_custom_req_s {
    guint8 subcmd;
    guint8 para_select;
    guint8 reserved[2];
} GET_CUSTOM_REQ_S;

#define SHELF_SERIAL_NUMBER 1
#define SHELF_LOCATION 2

gint32 ipmi_oem_get_fruid_info(const void *msg_data, gpointer user_data);
gint32 ipmi_oem_get_fruid_info_from_uid(const void *msg_data, gpointer user_data);
gint32 ipmi_get_addr_info(const void *msg_data, gpointer user_data);
gint32 ipmi_get_device_locator_record_id(const void *msg_data, gpointer user_data);
gint32 ipmi_fru_control_capabilities(const void *msg_data, gpointer user_data);
gint32 ipmi_get_device_info(const void *msg_data, gpointer user_data);
gint32 ipmi_read_frudata(const void *pdata, gpointer user_data);
gint32 ipmi_write_frudata(const void *pdata, gpointer user_data);
gint32 ipmi_get_fru_inventory(const void *pdata, gpointer user_data);
gint32 ipmi_dft_clear_elabel(const void *pdata, gpointer user_data);
gint32 ipmi_dft_write_elabel(const void *pdata, gpointer user_data);
gint32 ipmi_dft_read_elabel(const void *pdata, gpointer user_data);
gint32 ipmi_dft_update_elabel(const void *pdata, gpointer user_data);
gint32 ipmi_dft_custom(const void *pdata, gpointer user_data);
gint32 ipmi_get_dft_custom_cmd(const void *pdata, gpointer user_data);
gint32 ipmi_dft_get_component_pos(const void *msg_data, gpointer user_data);
gint32 ipmi_dft_get_comp_present(const void *msg_data, gpointer user_data);
gint32 ipmi_compute_power_properties(const void *msg_data, gpointer user_data);
gint32 ipmi_set_blade_info_parameters(const void *msg_data, gpointer user_data);
gint32 ipmi_get_blade_info_parameters(const void *msg_data, gpointer user_data);
gint32 ipmi_get_shelf_info(const void *msg_data, gpointer user_data);
gint32 ipmi_set_shelf_info(const void *msg_data, gpointer user_data);
gint32 ipmi_cmd_get_ecu_present_state(gconstpointer req_msg, gpointer user_data);
gint32 ipmi_cmd_set_ecu_present_state(gconstpointer req_msg, gpointer user_data);
gint32 get_sdi_ep_init_state_from_mcu(OBJ_HANDLE sdi_obj, guint8 *inited_state);
gint32 __get_init_state_from_related_accessor(OBJ_HANDLE com_obj, const gchar *property_name, guint8 *state);
gint32 ipmi_set_k_device_type(gconstpointer req_msg, gpointer user_data);
gint32 get_device_type_eeprom_block(guint32 offset, guint32 size, guint8 *eep_block);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif 
