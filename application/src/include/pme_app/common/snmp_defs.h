/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: SNMP协议相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __SNMP_DEFINE_H__
#define __SNMP_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* **************SNMP自定义错误码，与net-snmp开源软件区分开************** */
#define SNMP_USERID_EXIST -113
#define SNMP_USERID_NOT_EXIST -114
#define SNMP_USERNAME_EXIST -115

#define SNMP_COMMUNITY_COMPLEXITY_FAIL 0x84
#define SNMP_COMMUNITY_LENGTH_FAIL 0x85
#define SNMP_COMMUNITY_SPACE_FAIL 0x86
#define SNMP_COMMUNITY_VERSION3_ERR 0x87
#define SNMP_COMMUNITY_TOO_WEAK 0x88
#define SNMP_COMMUNITY_PREVIOUS 0x89
#define SNMP_COMMUNITY_FIELD 0xFF

#define SNMP_TRANS_AUTHLEN_HMACMD5  128
#define SNMP_TRANS_AUTHLEN_HMACSHA1 160
#define SNMP_TRANS_AUTHLEN_HMAC192SHA256   256
#define SNMP_TRANS_AUTHLEN_HMAC256SHA384   384
#define SNMP_TRANS_AUTHLEN_HMAC384SHA512   512
#define SNMP_TRANS_PRIVLEN_1DES 64
#define SNMP_TRANS_PRIVLEN_AES 128
#define SNMP_TRANS_PRIVLEN_AES256 256
#define SNMP_MAXBUF (1024 * 4)
#define SNMP_MAXBUF_SMALL 512
#define SNMP_MODEL "SNMP"
#define SNMP_SHORT_FILE_NAME "snmpd.conf"

#define USM_LENGTH_EXPANDED_PASSPHRASE (1024 * 1024) /* 1MB */
#define USM_LENGTH_KU_HASHBLOCK 64                   /* In bytes. */
#define USM_LENGTH_OID_TRANSFORM 10
#define USM_LENGTH_P_MIN 8 /* In characters. */

enum SNMPAuthProtocol {
    USE_MD5 = 0,
    USE_SHA = 1,
    USE_SHA1 = 2, /* 鉴权算法统一使用SHA，USE_SHA1等价于USE_SHA，向前兼容 */
    USE_SHA256 = 3,
    USE_SHA384 = 4,
    USE_SHA512 = 5,
    SNMP_AUTH_PROTOCOL_BUT
};

enum SNMPPrivacyProtocol {
    USE_NONE = 0,
    USE_DES = 1,
    USE_AES = 2,
    USE_AES256 = 3,
    SNMP_PRIVACY_PROTOCOL_BUT
};

enum BMAServerAgent {
    TRAP_CHANNEL_0 = 0,        // BoBChannelType为1时,BMC上转发trap 0通道消息的server
    TRAP_CHANNEL_1,            // BoBChannelType为1时,BMC上转发trap 1通道消息的server
    TRAP_CHANNEL_2,            // BoBChannelType为1时,BMC上转发trap 2通道消息的server
    TRAP_CHANNEL_3,            // BoBChannelType为1时,BMC上转发trap 3通道消息的server
    HTTPS_AGENT_SERVER_CHANNEL // BoBChannelType为1时,BMC上转发redfish消息的server
};

enum snmp_trap_identity {
    TRAP_ID_BRDSN = 0,
    TRAP_ID_ASSETTAG,
    TRAP_ID_HOSTNAME,
    TRAP_ID_CHASSIS_NAME,
    TRAP_ID_CHASSIS_LOCATION,
    TRAP_ID_CHASSIS_SERIAL_NUMBER
};

#define TRAP_MODE_EVT_STR "Event Code"
#define TRAP_MODE_OID_STR "OID"
#define TRAP_MODE_ACC_STR "Precise Alarm"
#define TRAP_MODE_STR_LEN 16

enum snmp_trap_mode {
    TRAP_MODE_EVT = 0, // 事件码模式
    TRAP_MODE_OID,     // OID模式
    TRAP_MODE_ACC,     // 精准告警模式(Accurate Alert Mode)
    TRAP_MODE_MAX,
};

#define TRAP_VER_V1_STR "V1"
#define TRAP_VER_V2C_STR "V2C"
#define TRAP_VER_V3_STR "V3"

#define MSG_WRITE_ONLY "write-only"
#define MSG_NO_ACCESS "No Access"

enum snmp_trap_version {
    TRAP_VER_V1 = 0,
    TRAP_VER_V2C,
    TRAP_VER_V3 = 3,
};

/* SNMP OID定制 */
#define CUSTOM_SNMP_OID_BUF_LEN 128
#define MAX_OF_CUSTOM_SNMP_OID 11

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SNMP_DEFINE_H__ */
