/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: License管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __LICENSE_DEFINE_H__
#define __LICENSE_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* License 服务错误码 */
typedef enum _tag_LM_error {
    LM_OK = 0,
    LM_ERR,
    LM_ERR_PARAMETER,
    LM_ERR_NOT_INSTALLED,
    LM_ERR_BUSY,
    LM_ERR_VERIFY,
    LM_ERR_ACTIVATE,
    LM_ERR_MAX
} LM_ERROR_E;

#define LM_BMC_PRD_NUM 0x01
#define LM_BMC_PRD_ID 0x01
#define LM_BMC_LICENSE_NUM 0x01
#define LM_LONG_STREAM_LEN 2056
#define LM_MINI_STREAM_LEN 72
#define LM_SHORT_STREAM_LEN 136

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __LICENSE_DEFINE_H__ */
