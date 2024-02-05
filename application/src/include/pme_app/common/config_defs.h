/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 配置管理（导入/导出）相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __CONFIG_DEFINE_H__
#define __CONFIG_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 配置导入、导出特性相关定义 */
#define COMPONENT_NAME_BIOS "BIOS"

#define IMPORT_CONFIG_USER_NAME "(import)"

#define PREFIX_IMPORT_CFG_CALL "PREFIX_IMPORT_"
#define MOTHER_BOARD_FRU_ID 0
#define TYPE_OUT_CALL 0
#define TYPE_COMMON_INNER_CALL 1
#define TYPE_IMPORT_CFG_INNER_CALL 2
#define TYPE_UNKNOW_CALL 0xff


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CONFIG_DEFINE_H__ */
