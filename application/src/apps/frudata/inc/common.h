


#ifndef __FRU_COMMON_H__
#define __FRU_COMMON_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#define FRU_DATA_APP_NAME "FruData"
#define CHAR_TYPE 0x03
#define MAX_CONNECTOR_SILK_TEXT_LEN 32
#define SYS_AREA_MAX_NUM 32
#define MAX_BMC_VER_LEN 20 
#define DEFAULT_MFG_HUAWEI ("Huawei")
#define FRU_ITEM_LENGTH (32 - 1)
#define CHASSIS_LOCATION_OFFSET 0x44
#define CHASSIS_LOCATION_LEN 20
#define CHASSIS_LOCATION_BLOCK_SIZE 24
#define CHASSIS_ID_OFFSET 0x5c
#define CHASSIS_ID_LEN 4
#define CHASSIS_ID_BLOCK_SIZE 8
#define CHASSIS_NAME_OFFSET 0x6c
#define CHASSIS_NAME_LEN 20
#define CHASSIS_NAME_BLOCK_SIZE 24
#define TASK_DELAY_ONE_SECOND 1000
#define TASK_DELAY_200_MS 200
#define MEZZ_MAC_BLOCK_OFFSET (0xc00 + 0xa68)
#define CANBUS_DEV_EEPROM_INFO_BASIC_OFFSET 0x17a8 // EEPROM保存电源信息起始位置
#define CANBUS_DEV_EEPROM_INFO_MAX_SIZE 6232
#define ELABEL_INFO_BLOCK_OFFSET 0x0C00
#define ELABEL_EXTEND_INFO_BLOCK_OFFSET 0x3000
#define WRITE_EEPROM_RETRY_TIMES 10
#define ELABEL_MULTI_AREA_OFFSET (0x3780) // V2 multi record电子标签的EEP的起始偏移地址
#define EEP_READ_INTERVAL 200
#define EEP_READ_RETRY 10
#define SYS_AREA_ELABEL_NUM 4
#define ELABEL_FILE_MAX_BUFF_SIZE 512 
#define SESA_COMPONENT_ID 9
#define MAX_NUM 1
#define ELABEL_SUFFIX_MIN_LEN 11
#define ELABEL_MAX_WRITE_REQ_LEN 0xff
#define MMC_MAX_ELABEL_LEN 0xff
#define COMPONENT_VIRTUAL 0
#define COMPONENT_PHYSICS 1
#define DEFAULT_AREA_VER 0x01
#define OFFSET_8_BIT 8


#define SYSTEM_MANUFACTURE_NAME 0
#define SYSTEM_NAME 1
#define SYSTEM_VERSION 2
#define SYSTEM_SERIAL_NUMBER 3
#define FRU_HEALTH_OK 0
#define FRU_HEALTH_FAIL 1

#define DEVICE_INFO_LEN 511
// 常量6表示返回除INFO部分还需要6个字节(响应码, 厂商, 结束标志, 以及预留一个字节空间)形成IPMI响应报文
#define DEVICE_INFO_RSP_LEN (DEVICE_INFO_LEN + 6)

#define MAX_BLADE_SLOT_NUM 32

#define FRU_SOURCE_FROM_NONE 0
#define FRU_SOURCE_FROM_E2P 1
#define FRU_SOURCE_FROM_PSU 2
#define E2P_SYSTEM_AREA_OFF 0x800


#define SUPER_BLOCK_SIZE 32
#define CHASSIS_PART_NUMBER_LEN 22
#define CHASSIS_SERIAL_NUMBER_LEN 32

#define FAN_FRU_HEAD_LEN 4

#define DEVICE_PROPERTY_TYPE_LOCATION 1
#define DEVICE_PROPERTY_TYPE_FUNCTION 2
#define DEVICE_PROPERTY_TYPE_NAME 3

#define COMPONENT_NOT_IN_PRESENT 0x00
#define COMPONENT_FORBIDDEN_SCAN 0x02

#define MGMT_SOFTWARE_TYPE_UNKNOW 0xff

// max_num value of the automatically allocated fruid
#define FRU_ID_MAX_AUTO_ALLOC 63
#define MAX_FRU_ID 0xff
#define FRU_ID_MAINBOARD 0
#define MAX_EXTERN_ELABEL_NUM 10
#define FRU_EXTERN_LABEL_PREFIX_LEN 6


#define CM_DEVICE_TYPE_PS 3
#define CM_DEVICE_TYPE_HDD_BACKPLANE 5
#define CM_DEVICE_TYPE_RAID_CARD 6
#define CM_DEVICE_TYPE_PCIE_CARD 8
#define CM_DEVICE_TYPE_AMC 9
#define CM_DEVICE_TYPE_MEZZ_CARD 11
#define CM_DEVICE_TYPE_NIC_CARD 13
#define CM_DEVICE_TYPE_MEMORY_BOARD 14
#define CM_DEVICE_TYPE_PCIE_RISER 15
#define CM_DEVICE_TYPE_MAINBOARD 16
#define CM_DEVICE_TYPE_CHASSIS_BACKPLANE 22
#define CM_DEVICE_TYPE_FANBOARD 24
#define CM_DEVICE_TYPE_BMC 26
#define CM_DEVICE_TYPE_MMC 27
#define CM_DEVICE_TYPE_BASEBOARD 28
#define CM_DEVICE_TYPE_FABRIC 30
#define CM_DEVICE_TYPE_SWITCH_MEZZ 31
#define CM_DEVICE_TYPE_IOBOARD 35
#define CM_DEVICE_TYPE_CPUBOARD 36
#define CM_DEVICE_TYPE_PCIE_ADAPTER 38
#define CM_DEVICE_TYPE_EXPAND_BOARD 45
#define CM_DEVICE_TYPE_TIANCHI_EXPBOARD 93
#define CM_DEVICE_TYPE_GPU_BOARD 64
#define CM_DEVICE_TYPE_CONVERGE_BOARD 91
#define CM_DEVICE_TYPE_SOC_BOARD 92
#define CM_DEVICE_TYPE_ALL 0xff
#define CM_DEVICE_NUM_ALL 0xff
#define CM_GROUP_ID_ALL 0xff

#define do_value_ifn_null(expr, value) \
    do {                               \
        if ((expr) != NULL) {          \
            *(expr) = value;           \
        }                              \
    } while (0)

typedef gint32 (*FRUDEV_READ_FRU)(OBJ_HANDLE object_handle, FRU_FILE_S *fru_file);

typedef gint32 (*GET_DEVICE_MAX_NUM_FUN)(const gchar *property_str, guint32 *max_num);

#pragma pack(1)

typedef union tag_system_info_u {
    SYSTEM_AREA_S system_area_st;
    TC_SYSTEM_DESC_S tc_system_desc_st;
    DATA_LENGTH_S system_area_array[SYS_AREA_ELABEL_NUM];
} SYSTEM_INFO_U;

typedef struct tag_elabel_internal_area_s {
    guint16 crc;
    guint16 len;
    guint8 data[FRU_INTERNAL_AREA_LEN - 4];
} ELABEL_INTERNAL_AREA_S;

typedef struct tag_elabel_field_s {
    guint16 crc;
    guint16 len;
    guint8 data[ELABEL_FILE_MAX_BUFF_SIZE];
} ELABEL_FIELD_S;

typedef struct tag_elabel_field_8_bytes_s {
    guint16 crc;
    guint16 len;
    guint8 data[4];
} ELABEL_FIELD_8_BYTES_S;

typedef struct tag_elabel_field_16_bytes_s {
    guint16 crc;
    guint16 len;
    guint8 data[12];
} ELABEL_FIELD_16_BYTES_S;

typedef struct tag_elabel_field_72_bytes_s {
    guint16 crc;
    guint16 len;
    guint8 data[68];
} ELABEL_FIELD_72_BYTES_S;

typedef struct tag_elabel_field_128_bytes_s {
    guint16 crc;
    guint16 len;
    guint8 data[124];
} ELABEL_FIELD_128_BYTES_S;

typedef struct tag_elabel_field_256_bytes_s {
    guint16 crc;
    guint16 len;
    guint8 data[252];
} ELABEL_FIELD_256_BYTES_S;

typedef struct tag_elabel_chassis_area_s {
    ELABEL_FIELD_8_BYTES_S type;
    ELABEL_FIELD_72_BYTES_S part_num;
    ELABEL_FIELD_72_BYTES_S serial_num;
    ELABEL_FIELD_256_BYTES_S extra;
} ELABEL_CHASSIS_AREA_S;

typedef struct tag_elabel_board_area_s {
    ELABEL_FIELD_8_BYTES_S mfg_time;
    ELABEL_FIELD_72_BYTES_S manufacture;
    ELABEL_FIELD_72_BYTES_S product_name;
    ELABEL_FIELD_72_BYTES_S serial_num;
    ELABEL_FIELD_72_BYTES_S part_num;
    ELABEL_FIELD_72_BYTES_S file_id;
} ELABEL_BOARD_AREA_S;

typedef struct tag_elabel_product_area_s {
    ELABEL_FIELD_72_BYTES_S manufacture;
    ELABEL_FIELD_72_BYTES_S name;
    ELABEL_FIELD_72_BYTES_S part_num;
    ELABEL_FIELD_72_BYTES_S version;
    ELABEL_FIELD_72_BYTES_S serial_num;
    ELABEL_FIELD_72_BYTES_S asset_tag;
    ELABEL_FIELD_72_BYTES_S file_id;
    ELABEL_FIELD_128_BYTES_S reserval;
} ELABEL_PRODUCT_AREA_S;

typedef struct tag_elabel_extend_area_s {
    guint16 crc;
    guint16 len;
    guint8 num;
    guint8 valid_flag;
    guint8 reserved2;
    guint8 reserved3;
    guint8 data[FRU_EXTERN_LABEL_AREA_LEN - 8];
} ELABEL_EXTEND_AREA_S;

typedef struct tag_elabel_system_area_s {
    ELABEL_FIELD_72_BYTES_S manufacture;
    ELABEL_FIELD_72_BYTES_S name;
    ELABEL_FIELD_72_BYTES_S version;
    ELABEL_FIELD_72_BYTES_S serial_num;
    ELABEL_FIELD_72_BYTES_S reserval;
} ELABEL_SYSTEM_AREA_S;

typedef struct tag_elabel_info_s {
    ELABEL_INTERNAL_AREA_S internal;
    ELABEL_CHASSIS_AREA_S chassis;
    ELABEL_BOARD_AREA_S board;
    ELABEL_PRODUCT_AREA_S product;
    ELABEL_EXTEND_AREA_S extend;
    ELABEL_SYSTEM_AREA_S system;
    ELABEL_EXTEND_AREA_S internal_extend; // The extended domains of the internal, chassis, and product domains
    ELABEL_EXTEND_AREA_S chassis_extend;
    ELABEL_EXTEND_AREA_S product_extend;
    guint8 multi_record[MAX_MULTI_LEN];
} ELABEL_INFO_S;

#define ELABEL_INFO_AREA_SIZE (sizeof(ELABEL_INFO_S) - (3 * sizeof(ELABEL_EXTEND_AREA_S)) - MAX_MULTI_LEN)
#define ELABEL_INFO_EXTEND_SIZE (sizeof(ELABEL_EXTEND_AREA_S))
#define ELABEL_EXTEND_INFO_SIZE (3 * sizeof(ELABEL_EXTEND_AREA_S))

typedef struct tag_ps_property_type_info_s {
    guint16 overallcapacity;
    guint16 input_type;
    guint16 peakVA;
    guint8 inrush_current;
    guint8 inrush_interval;
    guint16 low_input_vol1;
    guint16 high_input_vol1;
    guint16 low_input_vol2;
    guint16 high_input_vol2;
    guint8 low_input_freq;
    guint8 high_input_freq;
    guint8 ac_dropout_tol;
    guint8 binary_flags;

    union {
        guint16 pc_ht_us;
        struct {
            guint16 peakcapacity : 12;
            guint16 holduptime : 4;
        } pc_ht_st;
    } pc_ht_un;

    union {
        guint8 combined_vol_and_wat_id_uc;
        struct {
            guint8 voltage2 : 4;
            guint8 voltage1 : 4;
        } combined_vol_and_wat_id_st;
    } combined_vol_and_wat_id_un;

    guint16 combined_wattage;
    guint8 predictive;
} PS_PROPERTY_TYPE_INFO_S;

typedef struct tag_fru_property_s {
    OBJ_HANDLE fru_handle;
    guint8 fru_id;
    guint8 fru_type;
    guint16 fru_offset;
    guint8 pcb_id;
    guint8 is_cont_sys_area;
    OBJ_HANDLE elabel_handle;
    guint8 is_sup_dft;
    guint8 is_support_config_area;
    guint8 parser_protocol;
    OBJ_HANDLE frudev_handle;
    guint16 frudev_inv_size;
    guint8 frudev_type;
    gchar product_name[SYS_AREA_MAX_NUM + 1];
    guint8 site_type;
    guint8 ipmb_addr;
    guint8 slot_id;
    gchar fru_ver[MAX_BMC_VER_LEN + 1];
    guint16 locator_record_id; 
    FRUDEV_READ_FRU frudev_read_fru_pf;
    gboolean frudev_crc_state;
    OBJ_HANDLE rimm_obj_handle;

    
    const gchar *eeprom_accessor_prop;

    
    guint8 dft_operate_type;
} FRU_PROPERTY_S;

typedef struct C51_FRU_LABEL_STRU {
    guint8 len;
    gint8 content[FRU_ITEM_LENGTH];
} C51_FRU_LABEL_S;

typedef struct C51_FRU_INFO_STRU {
    C51_FRU_LABEL_S manufacturer;
    C51_FRU_LABEL_S model;
    C51_FRU_LABEL_S version;
    C51_FRU_LABEL_S data;
    C51_FRU_LABEL_S sn;
    guint32 input_type;
    C51_FRU_LABEL_S part_num;
    guint16 ps_rate;
} C51_FRU_INFO_S;


typedef union component_temp {
    guint32 component_alias_id;
    guint8 device_info[4];
} component_union;

typedef struct tag_ps_product_serial_num_s {
    guint8 commodity_code;
    guint8 assembly_code[4];
    guint8 revision_level[2];
    guint8 supplier_site[2];
    guint8 week_year[2];
    guint8 unique_sequence_id[2];
} PS_PRODUCT_SERIAL_NUM_S;

typedef struct tag_component_position_info {
    guint8 fru_id;
    guint32 position_id;
    gchar silk_text[MAX_CONNECTOR_SILK_TEXT_LEN];
} DFT_COMPONENT_POSN_INFO_S;


typedef struct tag_macaddr_area_s {
    guint16 crc;
    guint16 len;
    guint8 mac_id;
    guint8 interface_type;
    guint8 data[10];
    guint8 resv_data[16];
} MACADDR_AREA_S;

#define MACADDR_AREA_SIZE (sizeof(MACADDR_AREA_S))

typedef struct tag_dft_write_elabel_req_s {
    guint8 subcmd;
    guint8 fru_id;
    guint8 area;
    guint8 field;
    guint8 flag_offset;
    guint8 len;
    guint8 data[1];
} DFT_WRITE_ELABEL_REQ_S;

struct common_header {
    guint16 magic;   
    guint8 version;  
    guint8 checksum; 
};

struct super_block {
    struct common_header header;
    guint8 metadata[SUPER_BLOCK_SIZE - sizeof(struct common_header)];
};

typedef struct tag_super_block_v1 {
    struct common_header header;
    guint8 journal_offset; 
    guint8 journal_size;   
    guint8 journal_num;    
    guint8 iblock_offset;  
    guint8 iblock_size;    
    guint8 iblock_num;     
    guint8 dblock_offset;  
    guint8 dblock_size;    
    guint8 dblock_num;     
} SUPER_BLOCK_V1_S;

typedef struct tag_journal_v1 {
    guint8 begin;
    guint8 count;
    struct {
        guint8 inode;
        guint8 action;
    } transactions[2];
    guint8 checksum;
    guint8 end;
} JOURNAL_V1_S;

typedef struct tag_inode_block_v1 {
    guint8 flag;
    guint8 len;
    guint8 key[8];
    guint8 size;
    guint8 data[4];
    guint8 checksum;
} INODE_BLOCK_V1_S;

typedef struct data_block_v1 {
    guint8 data[16];
} DATA_BLOCK_V1_S;

#pragma pack()

typedef struct tag_eeprom_cafe {
    guint32 offset;
    OBJ_HANDLE frudev_handle;

    SUPER_BLOCK_V1_S *super;

    JOURNAL_V1_S *journal;
    guint8 journal_index;

    INODE_BLOCK_V1_S *iblock;
    GList *inode_free;
    GList *inode_used;

    struct data_block_v1 *dblock;
    GList *data_free;
    GList *data_used;

    GHashTable *records;

    guchar format_flag; 
} EEPROM_CAFE_S;

typedef struct tag_eeprom_cafe *EEPROM_CAFE_SP;

#pragma pack(1)

typedef struct tag_fru_priv_property_s {
    guint8 update_status; 
    gint32 update_fault;  
    gulong update_task_id;
    gulong elabel_task_id;
    gboolean elabel_task_state;
    gulong elabel_syn_semid;
    gboolean update_task_state;
    gulong update_syn_semid;
    gulong update_sys_task_id; 
    gboolean update_sys_task_state;
    gulong update_sys_syn_semid;
    gulong eep_scan_task_id;
    gboolean eep_scan_task_state;
    gboolean upgrate_state;
    gboolean initialized; 
    guint8 eeprom_format;
    TC_FILE_HEADER_S *tc_header;
    AREA_OFFSET_S *area_offsets;
    FRU_PROPERTY_S fru_property;
    FRU_FILE_S *fru_file;
    FRU_INFO_S *fru_info;
    SYSTEM_INFO_U *system_info;
    ELABEL_INFO_S *elabel_info;

    
    EEPROM_CAFE_SP cafe_info;
    guint8 fru_first_init_flag;
} FRU_PRIV_PROPERTY_S;

#pragma pack()

enum {
    STORAGE_TYPE_FILE,        
    STORAGE_TYPE_EEPROM,      
    STORAGE_TYPE_FLASH,       
    INTELLIGENT_NON_REPOSITY, 
    INTELLIGENT_REPOSITY,     
    STORAGE_TYPE_HI51,        
    STORAGE_TYPE_MCU,         
    STORAGE_TYPE_E2P_RAW, 
    STORAGE_TYPE_E2P_CUSTOM,          
    STORAGE_TYPE_MCU_FAN,             
    STORAGE_TYPE_PSU,                 
    STORAGE_TYPE_STD_E2P,             
    STORAGE_TYPE_E2P_DOUBLE,          
    STORAGE_TYPE_MCU_CUSTOM,          
    STORAGE_TYPE_MCU_INTELLIGENT_FAN, 
    STORAGE_TYPE_E2P_V4 = 15          
};

enum {
    STANDAND_PARSER, 
    HUAWEI_PARSER    
};


enum {
    MOTHER_BOARD,   
    MEZZ_CARD,      
    AMC_CONTROLLER, 
    MMC_CONTROLLER, 
    PS_MODULE       
};


enum {
    SYN_UPDATING,
    SYN_UPDAT_COMPLETED,
    SYN_NEED_UPDATE
};


enum {
    SYN_FRU_OK = 0,
    SYN_WRITE_FILE_FAIL = 0xE0,
    SYN_OPEN_FILE_FAIL,
    SYN_WRITE_E2P_FAIL,
    SYN_DFL_FAIL
};

#endif 
