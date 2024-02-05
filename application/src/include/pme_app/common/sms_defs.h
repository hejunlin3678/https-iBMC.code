/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 系统软件管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __SMS_DEFINE_H__
#define __SMS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


#define SMS_TAG "Sms"

#define SMS_HEALTH_STATE_OK 0
#define SMS_HEALTH_STATE_NOTOK 1
#define SMS_REGISTER_NOTOK 0
#define SMS_REGISTER_OK 1


#define SMS_RESTFUL_STATE_OK 0
#define SMS_BMA_GLOBAL_STATE_OK 0
#define SMS_BMA_GLOBAL_STATE_NOTOK 1

typedef enum {
    IBMA_SUPPORT_STATE_ENABLE = 0,       /* NAND Flash中含有iBMA分区且分区中存在iBMA安装包 */
    IBMA_SUPPORT_STATE_NO_PARTITION = 1, /* NAND Flash中无iBMA分区 */
    IBMA_SUPPORT_STATE_NO_PACKET = 2,    /* 分区中无iBMA安装包 */
    IBMA_SUPPORT_STATE_INVALID = 0xFF    /* 默认无效值 */
} IBMA_SUPPORT_STATE_E;

typedef enum {
    UMS_CONNECT_STATE_IDLE = 0,            /* 未连接 */
    UMS_CONNECT_STATE_CONNECTED = 1,       /* 已连接 */
    UMS_CONNECT_STATE_CONNECT_FAIL = 2,    /* 连接失败 */
    UMS_CONNECT_STATE_DISCONNECT_FAIL = 3, /* 断连失败 */
} UMS_CONNECT_STATE_E;

typedef enum {
    UMS_TYPE_SP = 0,   /* SP */
    UMS_TYPE_IBMA = 1, /* IBMA */
    UMS_TYPE_MAX,
} UMS_TYPE_E;

#define REGISTER_TYPE_BMA 0
#define REGISTER_TYPE_SP 1

#define HTTPS_REQUEST_FORWARD_BMA_MIN_RETRY 1
#define HTTPS_REQUEST_FORWARD_BMA_MAX_RETRY 10

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SMS_DEFINE_H__ */
