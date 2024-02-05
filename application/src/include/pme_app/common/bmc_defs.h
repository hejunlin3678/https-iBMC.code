/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: BMC配置相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义
 */

#ifndef __BMC_DEFINE_H__
#define __BMC_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 高四位是模块错误号，中间两位是子模块错误号，低两位是错误码 */
#define ERR_ID_BMC_GLOBAL 0x7FFF0000
#define ERR_ID_BMC_GLOBAL_MA 0x0100

enum {
    VOS_ERR_MA_IDEL = ERR_ID_BMC_GLOBAL + ERR_ID_BMC_GLOBAL_MA, // 0x7FFF0100
    VOS_ERR_MA_REACH_MAX,                                       /* 证书达到上限 */
    VOS_ERR_MA_CERT_EXSIT,                                      /* 证书已经存在 */
    VOS_ERR_MA_FORMAT_ERR,                                      /* 证书格式错误 */
    VOS_ERR_MA_NO_ISSUER,                                       /* 未找到签发者 */
    VOS_ERR_MA_BASIC_CONSTRAINTS_ERR,                           /* 证书基本约束不满足 */
    VOS_ERR_MA_CERT_ID_OUT_OF_RANGE,                            /* 证书ID超限 */
    VOS_ERR_MA_CERT_NOT_EXIST,                                  /* 指定证书不存在 */
    VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR,                         /* 证书密钥用法错误 */
    VOS_ERR_MA_CERT_NO_MATCH_DEVICE,                            /* 证书与设备不匹配 */
    VOS_ERR_MA_CERT_EXPIRE,                                     /* 证书过期 */
    VOS_ERR_MA_FILE_PATH_INVALILD,                              /* 证书文件路径无效 */
    VOS_ERR_MA_FILE_LEN_EXCEED,                                 /* 证书文件大小过大 */
    VOS_ERR_MA_PWD_LEN_INVALID,                                 /* 证书密码长度不合法 */
    VOS_ERR_MA_CRL_NOT_EXIST                                    /* 指定证书的CRL不存在 */
};

#define RESET_TYPE_POWER_UP 0
#define RESET_TYPE_EXT_WDT_KEY 1
#define RESET_TYPE_IN_WDT_UP 2

#define DEV_CPLD_NAME "/dev/cpld"
#define CPLD_MAP_ADDR_SIZE 0x1000
#define CPLD_INFO_SIZE 1152

#define BMC_FLASH_AREA ("/proc/mounts")
#define BMC_FLASH_CTRL_AREA ("/dev/mtdblock12")


#define UID_LOCATION_MAINBOARD 0 /* UID灯在主板上 */
#define UID_LOCATION_CHASSIS 1   /* UID灯在机框上 */
#define UID_MAINBOARD_AND_CHASSIS 2   /* 机框和主板上都有UID灯 */

#define HMM_BLADE_MANAGE_MODE 1    // E9000刀片是HMM管理模式
#define ESIGHT_BLADE_MANAGE_MODE 2 // E9000刀片是esight扁平化管理模式

#define LANGUAGE_LENGTH 256

/* 是否为白牌机标志 */
#define IS_CUSTOM_BRAND 1
#define IS_NO_CUSTOM_BRAND 0

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __BMC_DEFINE_H__ */
