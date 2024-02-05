
#ifndef BOOT_OPTIONS_H
#define BOOT_OPTIONS_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define BIOS_ID 0x00

#define BIOS_OBJECT_HANDLE_NAME 0x01010201
#define BIOS_OBJECT_SMBIOS_NAME 0x01010202

#define CONFIG_VER_LEN 128
#define BIOS_VERSION_LEN 128
#define BIOS_ATTRIBUTE_NAME_LEN 128
#define CHECK_BIOS_CONFIG_VER 3.00 // 在哪个版本之后需要对BIOS配置项进行校验

#define MAX_BIOS_FILE_LINE_LEN 1024
#define BIOS_CONFIG_HEAD_LINE "//[question]    [offset]    [width]    [value]    [attribute]\r\n"

#define BIOS_BOOT_ACK_INFO_NUM 2
#define BIOS_BOOT_VALID_NUM 5
#define BIOS_BOOT_INIT_INFO_NUM 9
#define BIOS_BOOT_INIT_MAILBOX_NUM 80
#define BIOS_BOOT_VALID_SEG_NUM 16
#define BIOS_MAILBOX_SEG_MAX_NUM (BIOS_BOOT_INIT_MAILBOX_NUM / BIOS_BOOT_VALID_SEG_NUM)

#define BIOS_OP_LOG_DEFAULT_CFG "Set BIOS status failed\r\n"
#define BIOS_OP_LOG_FILE_CHANGE "Set file changed flag failed\r\n"
#define BIOS_OP_LOG_SYS_BOOT "Set system boot options failed\r\n"
#define BIOS_OP_LOG_BOOT_DEVICE "Set boot device to (%s) failed\r\n"
#define BIOS_OP_LOG_MEZZ_MODE "Set osca mezz%d mode to (%s) failed\r\n"
#define BIOS_OP_LOG_MEZZ_MODE_1 "Set osca mezz mode failed\r\n"

#define BIOS_OPTIONS0_INI_FILE_NAME "/data/opt/pme/conf/bios/options0.ini"
#define TMP_BIOS_CHANGE0_INI_FILE_NAME "/tmp/changed0.ini"
#define BIOS_DISPLAY0_INI_FILE_NAME "/data/opt/pme/conf/bios/display0.ini"
#define BIOS_CHANGE_INI_FILE_NAME "/data/opt/pme/conf/bios/changed0.ini"
#define SHORT_BIOS_CHANGE_INI_FILE_NAME "changed0.ini"


#define TMP_CLP_CONFIG_FILE_FROM_BT "/dev/shm/clpconfigfrombt.ini"
#define TMP_BIOS_CLPCONFIG_INI_FILE_NAME "/dev/shm/clpconfig0.ini"


#define BIOS_FILE_SELECT_OFFSET 5
#define BIOS_FILE_PROPERTY_SELECT_OFFSET 6
#define BIOS_CHECK_NUM_OFFSET 7
#define BIOS_FILE_OFFSET_START 8
#define BIOS_ID_OFFSET 4
#define BIOS_VALID_FLAG_INTREVAL 8
#define BIOS_SUB_CMD_OFFSET 6
#define BIOS_DATA_LEN_OFFSET 11
#define BIOS_SUB_CMD_VALID_BIT 0x7f
#define BIOS_BOOT_OPTION_BIT2 2
#define BIOS_BOOT_OPTION_BIT 7
#define SMBIOS_FILE_ALREADY_CHANGE 1
#define SMBIOS_DATA_OFFSET_POS 11
#define SMBIOS_CHECK_NUM_OFFSET 6
#define SMBIOS_FILE_OFFSET_START 7
#define SMBIOS_SUB_CMD_OFFSET 5
#define BIOS_DATA_OFFSET_POS 12
#define BIOS_START_FLAG_PERMANENT 2

#define EXPORT_FILE_TRUE TRUE   // 标识导出文件操作
#define EXPORT_FILE_FALSE FALSE // 标识非导出文件操作

#define BIOS_SETTING_FILE_UNCHANGED 0
#define BIOS_SETTING_FILE_CHANGED 1

#define BIOS_FILE_CMES_NUM 0x1e
#define BIOS_FILE_MAX_SELECTOR 0x32


#define BIOS_SETTING_FILE_INEFFECTIVE 0
#define BIOS_SETTING_FILE_EFFECTIVE 1

#define BOOT_MODE_SUPPORT_FLAG_ENABLE 1

#define BIOS_FILE_SETTING_PATH "/data/opt/pme/conf/bios/setting.json"
#define BIOS_FILE_RESULT_PATH "/data/opt/pme/conf/bios/result.json"
#define BIOS_FILE_CURRENTVALUE_PATH "/data/opt/pme/conf/bios/currentvalue.json"
#define MAX_DATETIME_LEN 128


// file upload/download
#define RESOURCE_NUM 50
#define RESOURCE_BIOS_PROFILE_DISPLAY 0x00
#define RESOURCE_BIOS_PROFILE_OPTION 0x01
#define RESOURCE_BIOS_PROFILE_CHANGED 0x02
#define RESOURCE_BMC_DEBUG_INFO 0x03
#define RESOURCE_IMANA_FIRMWARE 0x04
#define RESOURCE_BMC_LOG_INFO 0x05
#define RESOURCE_SOL_LOG_INFO 0x06
#define RESOURCE_BLACKBOX_INFO 0x07
#define RESOURCE_BIOS_CLP_CONFIG 0x08
#define RESOURCE_BIOS_CLP_RESPONSE 0x09
#define RESOURCE_POWER_VIEW_ID 0x0B
#define RESOURCE_BIOS_PROFILE_REGISTRY 0x19
#define RESOURCE_BIOS_PROFILE_CURRENTVALUE 0x1a
#define RESOURCE_BIOS_PROFILE_SETTING 0x1b
#define RESOURCE_BIOS_PROFILE_RESULT 0x1c
#define RESOURCE_CMES 0x1e

#define BIOS_JSON_REGISTRYVERSION "RegistryVersion"
#define BIOS_CONFIG_VALUE_LEN 64
#define PROP_JSON_TYPE_INTEGER 1
#define PROP_JSON_TYPE_STRING 2
#define BIOS_ERR_OFFSET_MSG_LEN 8
#define BIOS_ERR_NORMAL_MSG_LEN 9

#define CLP_FILE_FROM_HMM 0
#define CLP_FILE_FROM_BT 1


#define SMBIOS_ERR_NORMALLY 0x00
#define SMBIOS_ERR_INVALID_STATUS 0xD5
#define SMBIOS_ERR_INVALID_CHKSUM 0x80
#define SMBIOS_ERR_WRONG_OFFSET 0x81
#define SMBIOS_ERR_FM_FAIL 0x82
#define SMBIOS_ERR_IV_LEN 0x83

#define BIOS_IPMI_CMD_MAX_LEN 248
#define BIOS_IPMI_CMD_MIN_LEN 4
#define BIOS_IPMI_CMD_NETNUM 0x30
#define BIOS_IPMI_CMD_CMDNUM 0x92
#define BIOS_IPMI_CMD_SHMC_CMDNUM 0x94
#define BIOS_IPMI_CMD_BIOS_CMDNUM 0x98
#define BIOS_VERSION_MAX_LEN 32
#define BIOS_VERSION_IPMI_CMD_LEN 7
#define TEEOS_INFO_MAX_LEN 128
#define BIOS_MSG_HEAD_MIN_LEN 4
#define TEEOS_MSG_HEAD_MIN_LEN 6
#define BIOS_SETUP_SYNC_MIN_LEN 160
#define TEEOS_VER_INFO 0


#define BIOS_ERR_NORMALLY 0x00
#define BIOS_ERR_INVALID_STATUS 0xD5
#define BIOS_ERR_IN_PROGRESS 0x80
#define BIOS_ERR_INVALID_CHKSUM 0x80
#define BIOS_ERR_WRONG_OFFSET 0x81
#define BIOS_ERR_FM_FAIL 0x82
#define BIOS_ERR_IV_LEN 0x83

#define SMBIOS_DATA_SIZE (130 * 1024) // 9032 smbios

#define BIOS_FLAG_IDLE 0x00
#define BIOS_FLAG_DOING 0x01

#define U32_BITS 32
#define LEFT_SHIFT7(x) ((x) << 7)
#define IPMB_MSG_MAX_PAYLOAD_LEN 248


#define BIOS_OPERATE_PREPARE 0x00
#define BIOS_OPERATE_DATA 0x01
#define BIOS_OPERATE_FINISH 0x03


#define BIOS_FILE_MAX_SIZE (10 * 1024 * 1024)
#define BIOS_FILE_OPTION "options"
#define BIOS_FILE_DISPLAY "display"
#define BIOS_FILE_CHANGED "changed"
#define BIOS_FILE_DISPLAY_NUM 0x00
#define BIOS_FILE_OPTION_NUM 0x01
#define BIOS_FILE_CHANGE_NUM 0x02
#define BIOS_FILE_DEBUG_INFO 0x03
#define BIOS_FILE_FIRMWARE 0x04
#define BIOS_FILE_NETCONFIG 0x05
#define BIOS_FILE_SOL 0x06
#define BIOS_FILE_BLACKBOX 0x07
#define BIOS_FILE_CLP 0x08
#define BIOS_FILE_CLP_RESP 0x09
#define BIOS_FILE_SETUP_NUM 0x0c
#define BIOS_FILE_SMBIOS_RAS 0x0d


#define START_OPTION_EXT_TFTP     0x00
#define START_OPTION_EXT_HTTPS    0x01
#define START_OPTION_EXT_NVMEM2   0x04
#define START_OPTION_EXT_EMMC     0x05
#define START_OPTION_EXT_SATA     0x06
#define START_OPTION_EXT_USB      0x08
#define START_OPTION_EXT_NONE     0xFF


#define REG_VAL_MAP_TFTP          0x01
#define REG_VAL_MAP_HTTPS         0x02
#define REG_VAL_MAP_NVMEM2        0x01
#define REG_VAL_MAP_EMMC          0x02
#define REG_VAL_MAP_SATA          0x03
#define REG_VAL_MAP_USB           0x01
#define REG_VAL_MAP_DEFAULT       0x01


#define BIOS_FILE_REGISTRY_NUM 0x19
#define BIOS_FILE_CURRENT_VALUE_NUM 0x1a
#define BIOS_FILE_SETTING_NUM 0x1b
#define BIOS_FILE_RESULT_NUM 0x1c

#define BIOS_FILE_SILK_CONFIG_NUM 0x2a
#define BIOS_FILE_NEW_SECUREBOOT_NUM 0x2b
#define BIOS_FILE_CURRENT_SECUREBOOT_NUM 0x2c
#define BIOS_FILE_CONFIGVALUE_NUM 0x2d
#define BIOS_FILE_POLICYCONFIGREGISTRY_NUM 0x2e
#define CONCATENATION_BASE 3
#define DWORD_OFFSET 3

#define RAS_HASH_SIZE 130 //        根据intel规范，SMBIOS中最多128中类型

#define REGISTER_MAX_READ_LENGTH 32
#define REGISTER_MIN_READ_LENGTH 1
#define GPIO_MAX_NUM 148
#define BIOS_GET_REGISTER_INFO_MIN_LEN 11

#define BIOS_BOOT_MAIBOX_SEG_LEN 1
#define BIOS_IPMI_LOG_FORMAT_LEN 3
#define BIOS_ADD_LOG_MIN_LEN 5

#define COMP_CODE_PARAM_NOT_SUPPORTED 0x80
#define COMP_CODE_SET_IN_PROGRESS 0x81
#define COMP_CODE_READ_ONLY 0x82
// set in progress
#define BOOTOPTION_SET_COMPLETE 0
#define BOOTOPTION_SET_INPROGRESS 1
#define BOOTOPTION_COMMIT_WRITE 2

enum {
    BIOS_BOOT_PROGRESS_CMD = 0,
    BIOS_BOOT_PARTITION_SEL_CMD,
    BIOS_BOOT_PARTITION_SCAN_CMD,
    BIOS_BOOT_FLAG_VALID_CMD,
    BIOS_BOOT_ACKNOWLEDGE_CMD,
    BIOS_BOOT_FLAGS_CMD,
    BIOS_BOOT_INITIATOR_INFO_CMD,
    BIOS_BOOT_INITIATOR_MAILBOX_CMD,

    BIOS_BOOT_OEM_WRITE_PROT_CMD = 0x63,
    BIOS_BOOT_NUM = 256
};


enum HARDWARE_REGISTER_TYPE {
    REGISTER_TYPE_CPLD = 1,
    REGISTER_TYPE_GPIO = 2,
    REGISTER_TYPE_IIC = 3,
    REGISTER_TYPE_EDMA = 4,
    REGISTER_TYPE_CPLD_I2C = 5,
    REGISTER_TYPE_OTHER
};

enum {
    BIOS_BOOT_OPTION_INVALID = 0,
    BIOS_BOOT_OPTION_VALID,
};

#define CONCATENATION_MAX_LEVEL 8
typedef struct tag_chip_info_s {
    guint8 bus_id; 
    guint16 addr;  
} CHIP_INFO_S;

typedef struct tag_concatenation_info_s {
    guint8 taoal_level_num; 
    CHIP_INFO_S chip_info[CONCATENATION_MAX_LEVEL];
} CONCATENATION_INFO_S;


typedef union {
    guint8 byte;
    struct {
#ifdef BD_BIG_ENDIAN
        guint8 devid : 4;
        guint8 protect : 4;
#else
        guint8 protect : 4;
        guint8 devid : 4;
#endif
    } bit;
} WRITE_PROTECT_PARA;

typedef struct tg_BOOT_OPTIONS {
    guint8 valid_flag[BIOS_BOOT_VALID_SEG_NUM]; // bit1 代表 selector 1 有效(0=有效), ... bit127 代表 127 有效
    guint8 set_in_progress;
    guint8 service_partition_selector;
    guint8 service_partition_scan;
    guint8 boot_flag_valid_bit_clearing;
    guint8 boot_info_acknowledge[BIOS_BOOT_ACK_INFO_NUM];
    guint8 boot_flags[BIOS_BOOT_VALID_NUM];
    guint8 boot_initiator_info[BIOS_BOOT_INIT_INFO_NUM];
    guint8 boot_initiator_mailbox[BIOS_BOOT_INIT_MAILBOX_NUM];
    WRITE_PROTECT_PARA write_protect;
} BOOT_OPTIONS_S, *PBOOT_OPTIONS_S;

typedef struct bios_change_ini_config {
    gchar boot_item[CONFIG_VER_LEN];
    guint8 offset;
    guint8 width;
    guint8 value;
    guint8 attribute;
} STRUCT_BIOS_CHANGE_INI_CONFIG;

typedef struct tg_BIOS_FILE_CHANGE {
    guint8 file_num;
    guint8 file_change_flag;
    guint8 file_channel;
} BIOS_FILE_CHANGE_S;

typedef struct tg_BIOS_DATA_OPERATE {
    guint32 data_flag;
    guint32 data_offset;
    guint32 data_len;
    guint8 *data_buf;
} BIOS_DATA_OPERATE_S;

typedef struct MERGE_SMBIOS_MES {
    gchar *oldBuf;
    gchar *newBuf;
    guint32 oldBufLen;
    guint32 newBufLen;
    guint32 newFileLen;
} MERGE_SMBIOS_T;

typedef struct RAS_NODE {
    void *pData;
    guint32 length;
    char *description;
    struct RAS_NODE *pNext;
} RAS_NODE_T, *pRAS_NODE_T;

typedef struct RAS_HASH {
    RAS_NODE_T *ppRasContainer[RAS_HASH_SIZE];
    gint32 nodeCount;
} RAS_HASH_T, *pRAS_HASH_T;

typedef struct RAS_SMBIOS_MES {
    pRAS_HASH_T pHash;
    gchar *pBuf;
    guint32 fileLength;
} RAS_SMBIOS_MES_T;

#pragma pack(1)


typedef struct {
#ifdef BD_BIG_ENDIAN
    guint8 flags_valid : 1;
    guint8 effect_opt : 1; 
    guint8 boot_type : 1;  
    guint8 reserved1 : 5;
#else
    guint8 reserved1 : 5;
    guint8 boot_type : 1;
    guint8 effect_opt : 1;
    guint8 flags_valid : 1;
#endif

#ifdef BD_BIG_ENDIAN
    guint8 clear_cmos : 1;
    guint8 lock_kbd : 1; 
    guint8 boot_dev : 4; 
    guint8 screen_blank : 1;
    guint8 lockout_rst_btn : 1; 
#else
    guint8 lockout_rst_btn : 1;
    guint8 screen_blank : 1;
    guint8 boot_dev : 4;
    guint8 lock_kbd : 1;
    guint8 clear_cmos : 1;
#endif

#ifdef BD_BIG_ENDIAN
    guint8 lockout_via_power_btn : 1; 
    guint8 bios_fw_verbose : 2;       
    guint8 force_pet : 1;             
    guint8 upw_bypass : 1;            
    guint8 lockout_sleep_btn : 1;
    guint8 cons_redirection_ctrl : 2; 
#else
    guint8 cons_redirection_ctrl : 2;
    guint8 lockout_sleep_btn : 1;
    guint8 upw_bypass : 1;
    guint8 force_pet : 1;
    guint8 bios_fw_verbose : 2;
    guint8 lockout_via_power_btn : 1;
#endif

#ifdef BD_BIG_ENDIAN
    guint8 reserved2 : 4;
    guint8 shared_override : 1;   
    guint8 mux_ctrl_override : 3; 
#else
    guint8 mux_ctrl_override : 3;
    guint8 shared_override : 1;
    guint8 reserved2 : 4;
#endif

#ifdef BD_BIG_ENDIAN
    guint8 reserved3 : 3;
    guint8 dev_selector : 5; 
#else
    guint8 dev_selector : 5;
    guint8 reserved3 : 3;
#endif
} BOOT_FLAGS_PARAM_S;

typedef struct tag_oem_get_hardware_register_info_req_s {
    guint32 manu_id : 24; 
    guint32 subcmd : 8;   
    guint8 register_type; 
    guint8 chip_id;       
    guint16 addr;         
    guint16 offset;       
    guint8 read_len;      
} OEM_GET_HARDWARE_REGISTER_INFO_REQ_S;

#pragma pack()

gint32 bios_set_start_option(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_start_option_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_start_option_flag(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_start_option_flag_ext(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_boot_order(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_boot_mode(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_boot_mode_sw(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_boot_mode_sw_enable(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_get_boot_order(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 bios_set_system_startup_state(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

gint32 bios_set_system_startup_state_default_value(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);

gint32 bios_set_system_boot_options(const void *req_msg, gpointer user_data);

void set_boot_option_valid(guint8 selector, guint8 valid);
gint32 modify_boot_options_boot_flag(guint8 effective_times);
void set_bios_setting_file_change_flag(guint8 flag, guint8 channel);
gint32 write_boot_mode_to_setting_json(guint8 boot_mode);
void update_bios_startup_state(guint8 value);
guint8 is_clp_config_file_from_bt(void);
extern gint32 write_src_content_to_bios_config(const gchar *src_file_name, const gchar *src_file_mode,
    const glong src_file_len, const gchar *dst_file_name, const gchar *dst_file_mode);
void set_clp_config_bt_flag(guint8 val, guint8 lock);

gint32 compare_and_copy_clp_config_file_with_lock(gchar *full_file_name);
void set_clp_file_change_flag(guint8 file_flag);
gint32 bios_clear_cmos(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
void bios_dynamic_config_sync(void);

gint32 create_ipmi_save_info_queue(void);
void get_bios_boot_flags(const void *req_msg, gpointer user_data, guint8 *resp_data,
                         guint8 resp_data_len, guint8 *resp_len);
void bios_boot_stage_monitor(gpointer data);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
