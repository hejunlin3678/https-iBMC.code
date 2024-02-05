/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: FRU/电子标签数据管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __FRU_DEFINE_H__
#define __FRU_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_READ_FRU_DATA_LEN 246

#define MAX_FRU_NUM 128

#define INVALID_FRUID 0xFF
#define MAIN_FRUID 0 //  主板FRU

#define CHASSIS_PAYLOAD_FRUID 0
#define CHASSIS_FRU_ID 0
#define BASE_FRU_ID 1
#define FABRIC_FRU_ID 2
#define FC_FRU_ID 3


#define BOARD_LEN_OFFSET 1
#define PRODUCT_LEN_OFFSET 1
#define CHASSIS_LEN_OFFSET 1

#define NO_MORE_INFO 0xC1 // FRU数据的特殊标记

#define FRU_MAX_ELABEL_LEN 48
#define MAX_SYSNAME_LEN 32
#define ELABEL_MAX_LEN 640

#define PANGEA_ELABEL_MAX_LEN 1024
#define PANGEA_ELABEL_GET_LEN 228

#define SYS_VER_MAX_LEN 1
#define SYS_MFG_NAME_MAX_LEN 48
#define SYS_PRODUCT_NAME_MAX_LEN 48
#define SYS_SN_MAX_LEN 48
#define SYS_DESC_LENGTH 256 // 产品定制化信息共占1K，前256字节存储系统域信息，后768字节存储mezz mac、canbus dev信息

#define SYS_VER_OFFSET 0
#define SYS_MFG_NAME_OFFSET (SYS_VER_OFFSET + SYS_VER_MAX_LEN)
#define SYS_PRODUCT_NAME_OFFSET (SYS_MFG_NAME_OFFSET + SYS_MFG_NAME_MAX_LEN)
#define SYS_SN_OFFSET (SYS_PRODUCT_NAME_OFFSET + SYS_PRODUCT_NAME_MAX_LEN)

#define FRU_INTERNAL_AREA_LEN 256
#define FRU_MAX_MULITI_RECORD_LEN 256
#define FRU_MAX_LEN 2048
#define FRU_AREA_MAX_LEN 2040 /* 分配给FRU区域的是2048个字节,减去头部的8个字节 */
#define FRU_FILE_MAX_LEN 2048
#define FRU_DATA_STRING_LEN 128

#define TC_STANDARD_CODE_OFFSET 8
#define TC_HEADER_FRU_OFFSET 13
#define FRU_HEADER_TAG 0x00555246
#define TC_STANDARD_CODE 0xA55AA55A
#define OFFSET_WORD_LENGTH 8

#define FRU_NOT_INSERT 0
#define FRU_INSERT 1


/* 无效FRU区域索引定义 */
#define FRU_FILED_INVALID (-1)
#define FRU_AREA_INVALID (-1)

#define FRU_EXTEND_1 1
#define FRU_EXTEND_2 2
#define FRU_EXTEND_3 3
#define FRU_EXTEND_4 4

/* internal field */
#define FRU_INTERNAL_DEFAULT 0
#define FRU_INTERNAL_EXTRA 1

/* chassis  field */
#define FRU_CHASSIS_TYPE 0
#define FRU_CHASSIS_PART_NUMBER 1
#define FRU_CHASSIS_SERIAL_NUMBER 2
#define FRU_CHASSIS_EXTRA 3

/* board  field */
#define FRU_BOARD_MFGDATE 0
#define FRU_BOARD_MANUFACTURER 1
#define FRU_BOARD_PRODUCTNAME 2
#define FRU_BOARD_SERIALNUMBER 3
#define FRU_BOARD_PARTNUMBER 4
#define FRU_BOARD_FRUFILEID 5
#define FRU_BOARD_EXTRA 6

#define FRU_BOARD_EXTEND_DESP           6
#define FRU_BOARD_EXTEND_ISSUENUMBER    7
#define FRU_BOARD_EXTEND_CLEICODE       8
#define FRU_BOARD_EXTEND_BOM            9
#define FRU_BOARD_EXTEND_MODEL          10

/* product  field */
#define FRU_PRODUCT_MANUFACTURER 0
#define FRU_PRODUCT_NAME 1
#define FRU_PRODUCT_PARTNUMBER 2
#define FRU_PRODUCT_VERSION 3
#define FRU_PRODUCT_SERIALNUMBER 4
#define FRU_PRODUCT_ASSETTAG 5
#define FRU_PRODUCT_FRUFILEID 6
#define FRU_PRODUCT_EXTRA 7

#define FRU_EXTERN_LABEL_AREA_LEN 640
#define FRU_EXTERN_LABEL_AREA_HEAD_LEN 8
#define FRU_FILE_VERSION 1
#define FRU_EXTEND_DATA_MAX_LEN     (FRU_EXTERN_LABEL_AREA_LEN - FRU_EXTERN_LABEL_AREA_HEAD_LEN)

/* system  field */
#define FRU_SYSTEM_MANUFACTURE 0
#define FRU_SYSTEM_PRODUCTNAME 1
#define FRU_SYSTEM_VERSION 2
#define FRU_SYSTEM_SERIALNUMBER 3

#define MULITPLE_PARA 8
#define MIN_OFFSET 2 // 电子标签结束标志和预留校验和的2个位置

#define FRU_TYPE_POWER 4
#define FRU_TYPE_LOM_NIC 12
#define FRU_TYPE_PCIE_NIC 13
#define FRU_TYPE_BACKPLANE 22
#define FRU_TYPE_FANTRAY 24
#define FRU_TYPE_EXP 50
#define FRU_TYPE_EPM 88
#define FRU_TYPE_FAN_BOARD 89
#define FRU_TYPE_OCP3     83
#define SINGLE_MULTI_HEAD_LEN   5
#define MAX_MULTI_LEN    1024
#define MASK_BIT7     128
#define MAX_SINGLE_MULTI_LEN   256

#define COMPONENT_DEVICE_NAME_LEN 64
#define COMPONENT_LOCATION_LEN 64

#define DEFAULT_PRODUCT_NAME 0
#define CUSTOM_PRODUCT_NAME 1
#define NEW_PRODUCT_NAME 2

/* 存放鲲鹏机型k与非k标志 E2P的起始位置为3B80写入一个字节 */
#define DEVICE_INFO_AREA_EEP_OFFSET 0x3B80
#define DEVICE_INFO_AREA_LENGTH_OFFSET 2
#define DEVICE_INFO_AREA_CRC_LEN 2
#define DEVICE_EEP_TYPE_OFFSET (DEVICE_INFO_AREA_EEP_OFFSET)
#define DEVICE_EEP_TYPE_EEP_SIZE 16
#define DEVICE_EEP_TYPE_EEP_DATA_LEN 12

#define DEVICE_INFO_STR_SIZE 12     // 16字节减去CRC(2字节)、减去长度(2字节)
#define DEVICE_TYPE_LEN_LIMIT 8
#define MAX_PROPERTY_VALUE_LEN 129  /* 属性值的最大长度 */
#define IPMI_DEVICE_TYPE_LAST_DATA 0x00                   /* 最后一帧数据标志 */

#define MAINBOARD_COMPONENT_DEVICE_TYPE (COMPONENT_TYPE_MAINBOARD)

enum {
    FRU_AREA_INTERNALUSE,
    FRU_AREA_CHASSISINFO,
    FRU_AREA_BOARDINFO,
    FRU_AREA_PRODUCTINFO,
    FRU_AREA_RESERVED,
    FRU_AREA_EXTENDELABLE,
    FRU_AREA_SYSTEMINFO,
    FRU_INTERNAL_EXTEND_AREA,
    FRU_CHASSIS_EXTEND_AREA,
    FRU_PRODUCT_EXTEND_AREA,
    FRU_AREA_MULTIRECORD,
    MAX_AREA_NUM
};

enum {
    EEPROM_FORMAT_V2,
    EEPROM_FORMAT_TIANCHI
};

#pragma pack(1)

typedef struct tag_data_length_s {
    guint8 len;
    guint8 data[FRU_MAX_ELABEL_LEN];
} DATA_LENGTH_S;

typedef struct tag_system_area_s {
    DATA_LENGTH_S sys_mfg_name;
    DATA_LENGTH_S sys_product_name;
    DATA_LENGTH_S sys_ver;
    DATA_LENGTH_S sys_sn;
    gint is_customize; /* 是否定制化, 需要持久化 */
    guint16 crc;
} SYSTEM_AREA_S;

typedef struct tag_tc_system_desc_s {
    guint8 sys_ver;
    gchar sys_mfg_name[48];
    gchar sys_product_name[48];
    gchar sys_sn[48];
    gint is_customize; /* 是否定制化, 需要持久化 */
    guint32 crc;
} TC_SYSTEM_DESC_S;

typedef struct tag_dft_read_elabel_req_s {
    guint8 subcmd;
    guint8 fru_id;
    guint8 area;
    guint8 field;
    guint8 flag_offset;
    guint8 len;
} DFT_READ_ELABEL_REQ_S;

typedef struct tag_read_fru_data_req_s {
    guint8 fru_id;
    guint16 offset;
    guint8 cnt;
} READ_FRU_DATA_REQ_S;

typedef struct tag_read_fru_data_resp_s {
    guint8 completion;
    guint8 cnt;
    guint8 content[1];
} READ_FRU_DATA_RESP_S;

typedef struct tag_fru_elabel_format_s {
    guint8 len : 6;
    guint8 type : 2;
    guint8 content[FRU_MAX_ELABEL_LEN];
} FRU_ELABEL_FORMAT_S;

typedef struct tag_fru_chassis_area_s {
    guint8 area_ver;
    guint16 area_len;
    guint8 chassis_type;
    FRU_ELABEL_FORMAT_S chassis_part_num;
    FRU_ELABEL_FORMAT_S chassis_serial_num;
    GList *custom_info;
} FRU_CHASSIS_AREA_S;

typedef struct tag_fru_board_area_s {
    guint8 area_ver;
    guint16 area_len;
    guint8 lang;
    guint8 mfg_date_time[4]; /* 从1996年1月1号开始计算的秒数 */
    FRU_ELABEL_FORMAT_S board_manufacturer;
    FRU_ELABEL_FORMAT_S board_product_name;
    FRU_ELABEL_FORMAT_S board_serial_num;
    FRU_ELABEL_FORMAT_S board_part_num;
    FRU_ELABEL_FORMAT_S file_id;
    GList *extension_label;
} FRU_BOARD_AREA_S;

typedef struct tag_tc_file_header_s {
    guint8 header_padding[8];
    guint32 standard_code;
    guint8 version;
    guint16 fru_offset;
    guint16 system_desc_offset;
} TC_FILE_HEADER_S;

typedef struct tag_area_offset_s {
    guint16 fru_offset;
    guint16 system_desc_offset;
    guint16 mezz_mac_offset;
    guint16 canbus_dev_offset;
} AREA_OFFSET_S;

typedef struct tag_fru_file_header_s {
    gchar flagc1;
    gchar flagc2;
    gchar flagc3;
    gchar flagc4;
    guint16 len;
    gushort crc;
} FRU_FILE_HEADER_S;

typedef struct tag_fru_file_s {
    union {
        FRU_FILE_HEADER_S fru_head;
        guint8 frudata[8];
    } header;
    guint8 frudata[FRU_AREA_MAX_LEN];
} FRU_FILE_S;

typedef struct tag_fru_info_header_s {
    guint8 version;
    struct {
        guint8 internal;
        guint8 chassis;
        guint8 board;
        guint8 product;
        guint8 multi;
    } offset;

    guint8 pad;
    guint8 checksum;
} FRU_INFO_HEADER_S;

typedef struct tag_fru_internal_area_s {
    guint16 len;
    guint8 *data;
} FRU_INTERNAL_AREA_S;

typedef struct tag_fru_product_area_s {
    guint8 area_ver;
    guint16 area_len;
    guint8 lang;
    FRU_ELABEL_FORMAT_S product_manufacturer;
    FRU_ELABEL_FORMAT_S product_name;
    FRU_ELABEL_FORMAT_S product_part_num;
    FRU_ELABEL_FORMAT_S product_version;
    FRU_ELABEL_FORMAT_S product_serial_num;
    FRU_ELABEL_FORMAT_S product_asset_tag;
    FRU_ELABEL_FORMAT_S file_id;
    GList *custom_info;
} FRU_PRODUCT_AREA_S;

typedef struct tag_fru_info_s {
    gboolean has_internal;
    gboolean has_chassis;
    gboolean has_board;
    gboolean has_product;
    gboolean has_multi;
    FRU_INFO_HEADER_S header;
    FRU_INTERNAL_AREA_S internal;
    FRU_CHASSIS_AREA_S chassis;
    FRU_BOARD_AREA_S board;
    FRU_PRODUCT_AREA_S product;
    GList *multi_record;
} FRU_INFO_S;

typedef struct tag_fru_multi_header_s {
    guint8 type;
    guint8 format;
    guint8 len;
    guint8 record_checksum;
    guint8 header_checksum;
} FRU_MULTI_HEADER_S;

typedef struct tag_fru_multi_area_s {
    FRU_MULTI_HEADER_S header;
    guint8 data[FRU_MAX_MULITI_RECORD_LEN];
} FRU_MULTI_AREA_S;

typedef struct tag_FRU_OBJECT_S {
    OBJ_HANDLE obj_handle;
    guint8 fru_id;
} FRU_OBJECT_S;

typedef struct {
    guint16 crc16;
    guint16 length;
    gchar device_type_str[DEVICE_INFO_STR_SIZE];
} DEVICE_TYPE_STR_AREA;

typedef struct tag_set_device_type_req_s {
    guint32 manufacturer : 24; // 产商信息
    guint8 subcmd;
    guint8 device_type;
    guint8 device_info;
    guint8 reserved;
    guint16 device_num;
    guint8 lastdata;
    guint16 offset;
    guint8 length;
    guint8 data[MAX_POSSIBLE_IPMI_FRAME_LEN];
} SET_DEVICE_TYPE_REQ_S;

#define SET_DEVICE_TYPE_REQ_FIXED_LEN (sizeof(SET_DEVICE_TYPE_REQ_S) - MAX_POSSIBLE_IPMI_FRAME_LEN)

typedef struct tag_set_device_type_resp_s {
    guint8 comp_code;
    guint32 manufacturer : 24; // 产商信息
} SET_DEVICE_TYPE_RESP_S;

#pragma pack()


#define FILL_BUF_FROM_LABEL(buf, out_size, label, offset)                             \
    do {                                                                              \
        if ((out_size) > (offset) + 1 + label.len) {                                  \
            memmove_s(&buf[offset], (out_size) - (offset), &(label), 1);              \
            offset++;                                                                 \
            memmove_s(&buf[offset], (out_size) - (offset), label.content, label.len); \
            offset += label.len;                                                      \
        }                                                                             \
    } while (0)


#define FILL_BUF_FROM_LABEL_POINTER(buf, out_size, plabel_pointor, offset)                                \
    do {                                                                                                  \
        if ((out_size) > (offset) + 1 + plabel_pointor->len) {                                            \
            memmove_s(&buf[offset], (out_size) - (offset), plabel_pointor, 1);                            \
            offset++;                                                                                     \
            memmove_s(&buf[offset], (out_size) - (offset), plabel_pointor->content, plabel_pointor->len); \
            offset += plabel_pointor->len;                                                                \
        }                                                                                                 \
    } while (0)


#define FILL_PAD(buf, out_size, offset, out_put)                                       \
    do {                                                                               \
        /* 填充电子标签结束标志和预留校验和的位置,如果加上后不足8的整数倍时,则填充0 */   \
        if ((out_size) >= (offset + MIN_OFFSET)) {                                     \
            out_put[offset++] = NO_MORE_INFO;                                          \
            out_put[offset++] = 0;                                                     \
            guint16 tmp = offset % MULITPLE_PARA;                                      \
            if (0 != tmp) {                                                            \
                if ((out_size) > offset) {                                             \
                    size_t destmax = ((out_size) - offset);                            \
                    size_t srclen = (MULITPLE_PARA - tmp);                             \
                    size_t srcmax = ((destmax >= srclen) ? srclen : destmax);          \
                    memset_s(&out_put[offset], destmax, 0, srcmax);                    \
                    offset += srcmax;                                                  \
                }                                                                      \
            }                                                                          \
        }                                                                              \
    } while (0)

gint32 dal_get_fru_elabel_data(guint8 com_type, const guint8 dev_num, const gint32 offset, guint8 *elabel,
    guint32 elabel_max_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FRU_DEFINE_H__ */
