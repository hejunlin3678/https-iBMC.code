/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: IPMI规范相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __IPMI_DEFINE_H__
#define __IPMI_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* IPMI规范定义的当前参数版本 */
#define RMCP_PARAMETER_REVERSION 0x11

#define RMCP_IPMI_BUF_SIZE 512
#define IPMI_KG_BUFFER_SIZE 21 /* key plus null byte */

/* session support */
#define CHAN_SESSION_LESS 0
#define CHAN_SINGLE_SESSION 1
#define CHAN_MULTI_SESSION 2
#define CHAN_SESSION_BASED 3

#define SESSION_LESS_ID 0xff /* 无需鉴权的session ID */

#define IPMI_TIMEOUT_EM 700 /* EM的IPMI消息超时时间，单位ms */


/* IPMI规范中的权限级别定义 */
#define PRIVILEGE_LEVEL_RESERVE 0       /* 保留 */
#define PRIVILEGE_LEVEL_CALLBACK 1      /* 回调级别 */
#define PRIVILEGE_LEVEL_USER 2          /* 普通用户级别 */
#define PRIVILEGE_LEVEL_OPERATOR 3      /* 操作员级别 */
#define PRIVILEGE_LEVEL_ADMINISTRATOR 4 /* 管理员级别 */
#define PRIVILEGE_LEVEL_OEM 5           /* 厂商自定义级别 */
#define PRIVILEGE_LEVEL_NOACCESS 15     /* 无权限级别 */

/* IPMI规范中的BootOption */
#define NO_OVERRIDE 0x00
/* Force PXE */
#define FORCE_PEX 0x01
/* Force boot from default Hard-drive */
#define FORCE_HARD_DRIVE 0x02
/* Force boot from default Hard-drive, request Safe Mode */
#define FORCE_SAFE_MODE 0x03
/* Force boot from default Diagnostic Partition */
#define FORCE_DIAGNOSTIC_PARTITION 0x04
/* Force boot from default CD/DVD */
#define FORCE_CD_DVD 0x05
/* Force boot into BIOS Setup */
#define FORCE_BIOS_SETUP 0x06
/* Force boot from Floppy/primary removable media */
#define FORCE_FLOPPY_REMOVABLE_MEDIA 0x0F

/* RMCP通道用户级别 */
#define RMCP_RESERVED 0
#define RMCP_CALLBACK 1
#define RMCP_USER 2
#define RMCP_OPERATOR 3
#define RMCP_ADMIN 4
#define RMCP_OEM 5
#define RMCP_NO_ACCESS 15

/* 完成码 */
#define RMCP_COMMAND_NORMALLY 0x00
#define RMCP_NODE_BUSY 0xC0 /* 命令临时无法执行 */
#define RMCP_INVALID_COMMAND 0xC1
#define RMCP_COMMAND_INVALID 0xC2 /* Command invalid for given LUN. */
#define RMCP_TIMEOUT 0xC3
#define RMCP_OUT_OF_SPACE 0xC4 /* 内存不够 */
#define RMCP_INVALID_RESERVATION_ID 0xC5
#define RMCP_REQUEST_DATA_TRUNCATED 0xC7
#define RMCP_DATA_LENGTH_INVALID 0xC7
#define RMCP_REQ_LENGTH_INVALID 0XC7
#define RMCP_FIELD_EXCEEDED 0xC8 /* Request data field length limit exceeded. */
#define RMCP_PARAMETER_OUT_OF_RANGE 0xC9
#define RMCP_RETURN_BYRES_ERROR 0xCA /* Cannot return number of requested data bytes. */
#define RMCP_REQUESTED_ABSENT 0xCB   /* Requested Sensor, data, or record not present. */
#define RMCP_INVALID_DATA_FIELD 0xCC
#define RMCP_COMMAND_ILLEGAL 0xCD    /* Command illegal for specified sensor or record type. */
#define RMCP_RESPONSE_ERROR 0xCE     /* Command response could not be provided. */
#define RMCP_DUPLICATED_REQUEST 0xCF /* This completion code is for devices which cannot return the response */

/* that was returned for the original instance of therequest. */
#define RMCP_RESPONCE_UNPROVIDED1 0xD0 /* Command response could not be provided. SDR Repository in update mode. */
#define RMCP_RESPONCE_UNPROVIDED2 0xD1 /* Command response could not be provided. Device in firmware update mode. */
#define RMCP_RESPONCE_UNPROVIDED3 0xD2 /* BMC initialization or initialization agent in progress. */
#define RMCP_DES_UNAVAILABLE 0xD3      /* unavailable. Cannot deliver request to selected destination. */
#define RMCP_LEVEL_NOT_ENOUGH 0xD4
#define RMCP_UNSUPPORT 0xD5         /* Command, or request parameter(s), not supported in present state. */
#define RMCP_PARAMETER_ILLEGAL 0xD6 /* Parameter is illegal because command sub-function has been disabled or is \
                                       unavailable */

#define COMP_CODE_PARA_NOT_SUPPORT_REAL_INFO 0x84
#define COMP_CODE_UNSUPPORT 0xD7

#define IPMI_INTEL_GET_CPU_IA_PCIREG 0x44 /* Intel Node Manager IPMI Spec */
#define IPMI_OEM_SHMC_COMMAND 0x94

#define SOL_MAX_CLIENT 2 /* 同一类型的SOL会话的TCP连接用户数 */
#define SOL_SESSION_SHARED 0
#define SOL_SESSION_DEDICATED 1
#define ERR_SOL_SESSION_DISABLED (-8001)
#define ERR_SOL_SESSION_ID_OUT_OF_RANGE (-8002)
#define ERR_SOL_SESSION_REACHED_MAX (-8003)
#define ERR_SOL_SESSION_OPEN_BY_RMCP (-8004)
#define ERR_SOL_SESSION_IN_DEDICATED_MODE (-8005)
#define ERR_SOL_SESSION_IN_SHARED_MODE (-8006)
#define ERR_SOL_SESSION_ALEADY_CLOSED (-8007)

typedef enum _tag_SOL_Session_Type {
    SOL_SESSION_UNKONWN = -1,
    SOL_SESSION_HMM = 0,
    SOL_SESSION_CLI,
    SOL_SESSION_IPMI,
} SOL_SESSION_TYPE_E;

/* **********第三方OEM命令网络功能码********** */
#define NETFN_THRID_OEM 0x3E


#define IPMI_LAN_RMCP 1
#define IPMI_LAN_RMCPPLUS 2
#define RMCP_PORT_SAVE 2

#define RMCP_SESSION_MAX_NUM 5
#define RMCP_INCLUDE_OPEN_SESSION_MAX_NUM (2 * RMCP_SESSION_MAX_NUM)
#define INVALID_OPEN_SESSION_SESSION_INDEX 0xff

#define IPMI_DATA_HEAD_LEN 6
#define IPMI_MSG_MAX_PAYLOAD_LEN 248
#define IPMI_MSG_HEAD_LEN (sizeof(IPMI_MSG_S) - IPMI_MSG_MAX_PAYLOAD_LEN)
#define IPMI_RESTRICTED_CMD_LEN 10

#define IPMI_SYS_LOCKDOWN_MASK 0x80   // 系统锁定使用最高位
#define IPMI_SYS_PRIVILEGE_MASK 0x7F  // 用户权限掩码
#define IPMI_SYS_LOCKDOWN_FORBID 0x80 // 开启系统锁定后禁止SET操作
#define IPMI_SYS_LOCKDOWN_ALLOW 0x0   // 开启系统锁定后允许SET操作


/* IPMI Channel ID的定义 */
typedef enum {
    IPMB_CHAN_NUM = 0,      /* IPMIChannel->ChanID:0，E6000 SMM板通道号 */
    LAN1_CHAN_NUM = 1,      /* IPMIChannel->ChanID:1，网口通道1 */
    LAN2_CHAN_NUM = 2,      /* IPMIChannel->ChanID:2，网口通道2 */
    IPMB_CARD_CHAN_NUM = 5,  /* IPMIChannel->ChanID:5，BMC 1711和板卡上1711通信通道 */
    IPMB_SM_CHAN_NUM = 6,   /* IPMIChannel->ChanID:6，ME通道号 */
    IPMB_L_CHAN_NUM = 7,    /* IPMIChannel->ChanID:7，MMC通道号 */
    IPMB_ETH_CHAN_NUM = 8,  /* IPMIChannel->ChanID:8，E9000 SMM板通道号 */
    EDMA_CHAN_NUM = 9,      /* IPMIChannel->ChanID:9，EDMA通道号 */
    IPMB_MM_CHAN_NUM = 10,  /* IPMIChannel->ChanID:10，主备SMM通信的通道号 */
    LOCAL_MM_CHAN_NUM = 11, /* IPMIChannel->ChanID:11，LOCALHOST SMM的通道号 */
    CPLDRAM_CHAN_NUM = 13,  /* IPMIChannel->ChanID:13，cpldram通道 */
    PRSENT_CHAN_NUM = 14,   /* IPMIChannel->ChanID:14，指当前通道 */
    SYS_CHAN_NUM = 15,      /* IPMIChannel->ChanID:15，系统接口通道 */
    MAINT_CHAN_NUM = 16,    /* IPMIChannel->ChanID:16，近端运维网口通道 */
    RACK_MAIN_CHAN_NUM = 17, /* IPMIChannel->ChanID:17，PoD柜内通信主网口通道 */
    RACK_BAK_CHAN_NUM = 18,   /* IPMIChannel->ChanID:18，PoD柜内通信备网口通道 */
    REMOTE_BUS_NUM = 19,     /* IPMIChannel->ChanID:19， 基于主从转发实现的IPMI通道，如存储北冰洋基于此通道实现IPMI over can */
    VETH_LAN_CHAN_NUM = 20,     /* IPMIChannel->ChanID:20， 计算型存储基于veth实现的lan通道 */
    MAX_IPMI_CHAN_NUM,
    INVALID_CHAN_NUM = 255   /* 无效通道 */
} CHANNEL_ID_E;


#define VETH_CHANNEL 9      /* veth设备的IPMI通道号，等同于上面的EDMA_CHAN_NUM */
#define OTHET_SMM_CHANNEL 1 /* 与对端SMM通信的IPMI通道号 */

/* IPMB地址和软件ID定义 */
#define SHMC_ADDR 0x20
#define IPMC_ADDR 0x20
#define KCS_SWID 0x61
#define LAN_SWID 0x63
#define EDMA_SWID 0x65
#define SP_SWID 0x66
#define CPLDRAM_SWID 0x67 /* 支持CPLD RAM 功能，存储pangea产品应用场景 */
#define REMOTECALL_SWID 0x68 /* 支持REMOTE BUS 功能，实现IPMI over can转发 */
#define VETH_LAN_SWID 0x69 /* 支持带内通过veth lan通道接收ipmi功能 */

#define BASE_IPMB_ADDR 0x71   /* base平面IPMB地址 */
#define FABRIC_IPMB_ADDR 0x73 /* fabric平面IPMB地址 */
#define FCOE_IPMB_ADDR 0x75   /* fcoe平面IPMB地址 */

/* IPMI命令通过类ID和属性ID读/写属性 */
#define IPMI_OEM_GET_ALL_OBJECTINFO 0x38
#define IPMI_SET_OBJ_PROP_CMD 0x35
#define IPMI_QUERY_OBJ_PROP_CMD 0x36
#define TLV_DATA_S_FORMAT 1
#define TLV_PROPERTY_DATA_S_FORMAT 2
#define SPE_LENGTH_TLV_DATA_FORMAT 4
#define SPE_LENGTH_TLV_PROPERTY_DATA_FORMAT 5

#define CCODE_SIZE 1 /* IPMI响应中完成码的长度 */

#define MAX_PICMG_RESPONCE_DATA_LEN 24

#define IPMI_MAX_MSG_LENGTH 272 /* multiple of 16 */


#pragma pack(1)

typedef struct tag_PICMG_RESPONCE {
    guint8 completion;
    guint8 picmgId;
    guint8 data[MAX_PICMG_RESPONCE_DATA_LEN];
} PICMG_RESPONCE_S;

typedef struct tg_ipmb_data {
    guint8 dest_addr;      // byte1
    guint8 dest_lun : 2;   //
    guint8 dest_netfn : 6; // byte2     必填
    guint8 chksum1;        // byte3
    guint8 src_addr;       // byte4

    guint8 src_lun : 2; //
    guint8 src_seq : 6; // byte5

    guint8 src_cmd;                            // byte6       必填
    guint8 src_data[IPMI_MSG_MAX_PAYLOAD_LEN]; // byte7~byteN-1  必填
    // guint8 src_chksum;       // last byte, cannot direct used
} IPMI_DATA_S;

/* IPMI消息结构体 */
typedef struct tg_ipmi_msg {
    /* 以下部分主要IPMI模块使用 */
    guint8 src_chan_num;
    guint8 dest_chan_num;
    guint8 chan_protocol; // channel protocol
    guint8 req_bus;       // 仅IPMB通道有意义，从哪个IPMB通道接收到的请求
    guint32 req_timeinms; // BMC 接收到该请求的时间(单位: ms)

    /* 以下部分主要APP模块使用 */
    guint32 manuidflag;        // IPMI厂商定制化需求新增字段
    TARGET_TYPE_E target_type; // APP主动发IPMI消息时需要关心    必填
    guint8 target_instance;    // 默认为0，如MMC、ME有多个需要填写
    guint8 data_len;           // data length, not including ipmi msg header & tail (cmd,netfn,seq,..) 必填

    IPMI_SESSION_S session;

    union {
        guint8 data[IPMI_MSG_MAX_PAYLOAD_LEN + IPMI_DATA_HEAD_LEN];
        IPMI_DATA_S ipmi_data;
    } payload;

    guint8 reservd[4]; // 保留字节
} IPMI_MSG_S;

typedef struct tg_ipmi_msg_filter_cond {
    guint8 dest_lun : 2;
    guint8 dest_netfn : 6;
    guint8 src_lun : 2;
    guint8 src_seq : 6;
    guint8 cmd;
} IPMI_MSG_FILTER_COND_T;

typedef struct tg_ipmi_msg_filter {
    guint8 valid; /* 该条件是否有效 */
    IPMI_MSG_FILTER_COND_T ipmi_fileter_condition;
} IPMI_MSG_FILTER_T;

typedef struct tg_ipmi_filtered_msg {
    guint8 valid;    /* 用以标记表中的该条目是否有效; 1表示该条目有效不能再被占用，0表示该条目无效可被占用 */
    guint8 data_len; /* ipmi消息data长度 */
    IPMI_DATA_S ipmi_data;
} IPMI_FILTERED_MSG_T;


/* 定义从ME获取PCIE卡BDF码信息的请求体 */
typedef struct pcie_ipmi_resq_data_struct {
    guint8 intel_id[3]; /* Byte 1:3 = Intel Manufacturer ID ?C 000157h, LS byte first. */
    guint8 cpu_num;     /* Byte 4  CPU Number */
    guint8 pci_addr[4]; /* Byte 5:8 PCI Address */
    guint8 read_length; /* Byte 6 ?C Read Length */
} PCIE_IPMI_RESQ_DATA;


/* 定义pkg寄存器读取命令的请求体  */
typedef struct pkgconfig_reg_ipmi_data_struct {
    guint8 intel_id[3]; /* Byte 1:3 = Intel Manufacturer ID ?C 000157h, LS byte first. */
    guint8 socket_id;   /* Byte 4 socket_id */
    guint8 PCS_index;   /* Byte 5 PCS Index */
    guint8 para_num[2]; /*  Byte 6:7 Parameter Number (WORD)
                            Byte 6 Parameter [7..0]
                            Byte 7 Parameter [15..8] */
    guint8 read_length; /*  Byte 8 Read Length */
} PKGCONFIG_REG_IPMI_DATA;


/* 获取属性的IPMI命令请求/响应 */
// AR-0000976084, Modify to 512
// DTS2020081908H920P0H00 Modify from 512 to 1024
#define IPMI_PROPERTY_MAX_LEN 1024

typedef struct tag_all_object_info_req {
    guint32 huawei_iana : 24;
    guint32 sub_cmd : 8;
    guint16 function_id;
    guint8 info_count;
    guint8 req_data_len;
    guint16 read_offset;
    guint8 data[1];
} ALL_OBJECT_INFO_REQ_S;

typedef struct tag_all_object_info_resp {
    guint32 complete_code : 8;
    guint32 huawei_iana : 24;
    guint8 data_end_state;
    guint8 data[1];
} ALL_OBJECT_INFO_RESP_S;

/* IPMI过来的TLV数据格式 */
typedef struct tag_tlv_data_s {
    guint16 id;     /* TLV的ID字段 */
    guint8 type;    /* TLV的类型字段 */
    guint8 length;  /* TLV的长度字段 */
    guint8 data[1]; /* TLV的数据字段 */
} TLV_DATA_S;

typedef struct tag_tlv_data_ss {
    guint16 property_id;
    guint8 type;
    guint16 length;
    guint8 data[1];
} TLV_PROPERTY_DATA_S;

typedef struct tag_set_prop_info_req {
    guint32 huawei_iana : 24;
    guint32 sub_cmd : 8;
    guint16 function_id;
    guint8 info_count;
    guint8 end_flag;
    guint16 offset;
    guint8 data[1];
} SET_PROP_INFO_REQ_S;

typedef struct tag_set_prop_info_resp {
    guint32 complete_code : 8;
    guint32 huawei_iana : 24;
} SET_PROP_INFO_RESP_S;

typedef struct tag_IPMIMSG_PROP_QUERY_INFO_RESP_S {
    guint8 comp_code;      // 返回码；
    guint32 oem_info : 24; // OEM信息；
    guint32 end_list : 8;  // 保留位；
    guint8 data;           // 数据位首地址；
} IPMIMSG_PROP_QUERY_INFO_RESP_S;

typedef struct tag_ipmi_prop_info_resp_s {
    guint16 property_id;    // 属性对应ID值；
    guint8 property_type;   // 属性值类型；
    guint8 property_length; // 属性值长度；
    guint8 property_value;  // 属性值数据首地址；
} IPMI_PROP_INFO_RESP_S;

typedef struct tag_one_obj_info_query_request_info_s {
    guint32 manufacturer : 24; // 产商信息
    guint32 sub_command : 8;   // 子命令
    guint16 class_id;
    guint8 object_count;
    guint8 req_len;
    guint16 offset;
    guint16 key_id;
    guint8 key_type;
    guint8 key_length;
    guint8 key_value[1];
    guint16 object_id[1];
} ONE_OBJ_INFO_REQ_S;

#define MANUF_ID_LEN 3
typedef struct tag_IPMI_GET_DEVICE_ID_RESP {
    guint8 CCode;
    guint8 DeviceId;
    guint8 DeviceRev_SdlPresent;
    guint8 FwMajorRev_InUPDATE; // binary encoded
    guint8 FwMinorRev;          // BCD encoded
    guint8 IpmiVersion;         // BCD encoded, reverced digit order
    guint8 FeatureMask;         // bitmask of supported features
    guint8 ManufId[MANUF_ID_LEN];
    guint16 ProdId;
    guint8 FwIfaceVer; // BCD encoded two digits
    guint16 BuildNo;   // BCD encoded build
    guint8 ImageNo;    // 0-Recovery, 1-Operational1, 2-Operational2
} IPMI_GET_DEVICE_ID_RESP_S;

typedef struct {
    guint8 comp_code;
    guint8 manu_id[MANUFAC_ID_LEN];
    guint8 config_len;
    gchar config_val[MAX_PROPERTY_VALUE_LEN];
} IPMIMSG_OEM_GET_CONFIG_RESP;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN];
    guint8 subcmd;
    guint16 param_selector; /* Little-Endian */
    guint16 config_len; /* Little-Endian */
} IPMIMSG_OEM_SET_CONFIG_REQ;

typedef struct {
    guint8 comp_code;
    guint8 manu_id[MANUFAC_ID_LEN];
} IPMIMSG_OEM_SET_CONFIG_RESP;


#pragma pack()


#ifndef ENCTYPTDATA_CALLBACK
#define ENCTYPTDATA_CALLBACK
typedef void (*EncryptData_CALLBACK)(guchar *ikey, /* 加密密钥 */
    guchar iv[16],                                 /* 初始化向量 */
    guchar *input,                                 /*  */
    guchar *output,                                /*  */
    gint32 len, guchar padding);                   /*  */
#endif

#ifndef DECTYPTDATA_CALLBACK
#define DECTYPTDATA_CALLBACK
typedef void (*DecryptData_CALLBACK)(guchar *ikey, /* 加密密钥 */
    guchar iv[16],                                 /* 初始化向量 */
    guchar *input,                                 /*  */
    guchar *output,                                /*  */
    gint32 len, guchar padding);                   /*  */
#endif


#define proxy_ipmi_operation_log(reg_msg, format, arg...)                                               \
    do {                                                                                                \
        TARGET_TYPE_E macro_target_type = get_ipmi_target_type(reg_msg);                                \
        const IPMI_SESSION_S *macro_ipmi_session_ptr = NULL;                                            \
        const gchar *macro_module_name = dfl_get_module_name();                                         \
        macro_ipmi_session_ptr = get_ipmi_rmcp_info(reg_msg);                                           \
        if (NULL == macro_ipmi_session_ptr) {                                                           \
            debug_log(DLOG_ERROR, "[%s]:line(%d): get_ipmi_rmcp_info failed!", __FUNCTION__, __LINE__); \
            break;                                                                                      \
        }                                                                                               \
        const gchar *macro_user_name = macro_ipmi_session_ptr->user_name;                               \
        gchar macro_client_buf[65];                                                                     \
        gchar *macro_client = macro_client_buf;                                                         \
        const gchar *macro_target_type_name[] = {                                                       \
                                                    "HMM",                                              \
                                                    "ME",                                               \
                                                    "MMC",                                              \
                                                    "HOST",                                             \
                                                    "LAN"                                               \
                                                };                                                      \
        gchar *macro_default_name = "N/A";                                                                         \
        guint32 macro_index = (guint32)macro_target_type;                                                          \
        macro_index -= (guint32)IPMI_SMM;                                                                          \
        memset_s(macro_client_buf, sizeof(macro_client_buf), 0, sizeof(macro_client_buf));                         \
        if (macro_ipmi_session_ptr->user_name[0] != 0) {                                                           \
            gint32 ret = snprintf_s(macro_client_buf, sizeof(macro_client_buf), sizeof(macro_client_buf) - 1, "%s", \
                macro_ipmi_session_ptr->user_ip);                                                                  \
            if (ret <= 0) {                                                                                        \
                debug_log(DLOG_ERROR, "snprintf_s failed, ret=%d", ret);                                           \
            }                                                                                                      \
        } else {                                                                                                   \
            macro_user_name = macro_default_name;                                                                  \
            if (macro_index >= (sizeof(macro_target_type_name) / sizeof(macro_target_type_name[0]))) {             \
                macro_client = macro_default_name;                                                                 \
            } else {                                                                                               \
                (void)snprintf_s(macro_client_buf, sizeof(macro_client_buf), sizeof(macro_client_buf) - 1, "%s",   \
                    macro_target_type_name[macro_index]);                                                          \
            }                                                                                                      \
        }                                                                                                          \
        dal_proxy_operation_log("IPMI", macro_user_name, macro_client, macro_module_name, format, ##arg);          \
    } while (0)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IPMI_DEFINE_H__ */
