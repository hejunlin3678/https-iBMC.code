/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: wangqinyu
 * Create: 2022
 * History: 2022/3/2 wangqinyu create
 */
#ifndef H_CMSCBB_VERIFY_COMMON_H
#define H_CMSCBB_VERIFY_COMMON_H
#include "cmscbb_types.h"
#include "../cmscbb_common/cmscbb_def.h"
#include "cmscbb_x509_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CMSCBB_ERROR_CODE CmscbbCheckVerifyOjbect(const CmscbbVrf* vrf);

CMSCBB_ERROR_CODE CmscbbCheckBufLen(CVB_UINT32 bufLength);

CMSCBB_ERROR_CODE CmscbbAddCertToStore(const CmscbbVrf* vrf, const LIST_OF(CmscbbX509Cert) *certList,
    CVB_BOOL allowRoot);

CMSCBB_ERROR_CODE CmscbbAddCrlToStore(const CmscbbVrf* vrf, const LIST_OF(CmscbbX509Crl) *crlList);

#if CMSCBB_SUPPORT_FILE
CMSCBB_ERROR_CODE CmscbbReadFile(const CVB_CHAR* filePath, CVB_BYTE** data, CVB_INT32* size);

CVB_VOID CmscbbCloseFile(CVB_FILE_HANDLE fpSrc);
#endif /* CMSCBB_SUPPORT_FILE */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* H_CMSCBB_VERIFY_COMMON_H */
