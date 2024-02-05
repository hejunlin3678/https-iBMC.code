/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 服务管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __SERVICES_DEFINE_H__
#define __SERVICES_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SERVICE_OFF 0
#define SERVICE_ON 1
#define SERVICE_RESTART 2

#define SERVICE_STOP 0
#define SERVICE_START 1

#define STATE_ENABLE 1
#define STATE_DISABLE 0

#define ENCRYPT_STATE_OFF 0
#define ENCRYPT_STATE_ON 1


#define SERVICE_DEFAULT_PORT_KVM 2198
#define SERVICE_DEFAULT_PORT_VMM 8208

#define SERVICE_CONFIGURATION_FTP 0x01
#define SERVICE_CONFIGURATION_SSH 0x02
#define SERVICE_CONFIGURATION_KVM 0x04
#define SERVICE_CONFIGURATION_VMM 0x05
#define SERVICE_CONFIGURATION_CIM 0x06
#define SERVICE_CONFIGURATION_WSMAN 0x07
#define SERVICE_CONFIGURATION_SNMP 0x08
#define SERVICE_CONFIGURATION_WEB_HTTP 0x09
#define SERVICE_CONFIGURATION_WEB_HTTPS 0x0B
#define SERVICE_CONFIGURATION_IPMILAN 0x0A
#define SERVICE_CONFIGURATION_HMMSSHNAT 0x0C
#define SERVICE_CONFIGURATION_SSDP 0x0D // 扩展,对外不展示
#define SERVICE_CONFIGURATION_HMMSSHNAT2 0x0E

/* 服务使能状态 */
#define SERVICE_STATE_ENABLE 0x01
#define SERVICE_STATE_DISABLE 0x00

/* 服务端口最小值和最大值 */
#define PORT_MIN_VALUE 1
#define PORT_MAX_VALUE 65535

/* 服务名称 */
#define SERVICE_NAME_SSDP "ssdpd"
#define SSDP_RECEIVE_BIT 1
#define SSDP_SEND_BIT 0

#define INVALID_SERVICE_PORT 0x00
#define INVALID_SERVICE_STATE 0xff
#define INVALID_SERVICE_SCREEN_SWITCH 0xff
#define INVALID_SERVICE_ENCRY_STATE 0xff
#define INVALID_SERVICE_TIMEOUT 0xffff
#define INVALID_SERVICE_VALIDITY_PERIOD 0xffff
#define INVALID_LOCAL_KVM_STATE 0xff

enum {
    SSH_SERVICE_PORT_INDEX = 0,   /* 0 */
    WEB_HTTP_SERVICE_PORT_INDEX,  /* 1, WEB_HTTP_SERVICE_PORT_INDEX 和WEB_HTTPS_SERVICE_PORT_INDEX必须连续排列 */
    WEB_HTTPS_SERVICE_PORT_INDEX, /* 2 */
    HMM_SSH_NAT_PORT_INDEX,       /* 3 */
    SNMP_SERVICE_PORT_INDEX,      /* 4 */
    KVM_SERVICE_PORT_INDEX,       /* 5 */
    VNC_SERVICE_PORT_INDEX,       /* 6 */
    VMM_SERVICE_PORT_INDEX,       /* 7 */
    VIDEO_SERVICE_PORT_INDEX,     /* 8 */
    IPMI_SERVICE_PORT1_INDEX,     /* 9 */
    IPMI_SERVICE_PORT2_INDEX,     /* 10 */
    SSDP_SERVICE_PORT_INDEX,      /* 11 */
    HMM_SSH_NAT2_PORT_INDEX,      /* 12 */
    PORT_IDEX_MAX                 /* keep as the last item, do not insert item after this one */
};

/* 状态检测index */
enum {
    SSH_SERVICE_STATE_INDEX = 0,     /* 0 */
    WEB_HTTPS_SERVICE_STATE_INDEX,   /* 1 */
    RMCP_LAN_SERVICE_STATE_INDEX,    /* 2 */
    RMCP_LANPLUS_SERVICE_STATE_INDEX /* 3 */
};

#define MAX_PORT_PER_SERVIVCE 2

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SERVICES_DEFINE_H__ */
