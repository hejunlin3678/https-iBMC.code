/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_BASE64_H
#define H_CMSCBB_BASE64_H

#include "cmscbb_config.h"

#if CMSCBB_SUPPORT_PEM
#include "cmscbb_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Prototype    : CmscbbBase64Decode
 * Description  : decode base64 stream
 * Params
 *   [IN] encoded: encode by base64
 *   [IN] encodedLen: encode length
 *   [IN] decoded: decode from  base64
 *   [IN] decodedLen: decode length
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/02 15:22  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbBase64Decode(const CVB_CHAR* encoded, CVB_UINT32 encodedLen,
    CVB_BYTE* decoded, CVB_UINT32* decodedLen);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* H_CMSCBB_BASE64_H */

#endif
