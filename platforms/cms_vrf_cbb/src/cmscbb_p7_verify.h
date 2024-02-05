/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: wangqinyu
 * Create: 2022
 * History: 2022/3/2 wangqinyu create
 */
#ifndef H_CMSCBB_P7_VERIFY_H
#define H_CMSCBB_P7_VERIFY_H

#include "cmscbb_types.h"
#include "cmscbb_cms_vrf.h"
#include "../asn1/cmscbb_asn1_def.h"
#include "../cmscbb_common/cmscbb_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

CVB_VOID CmscbbFreeMdInfo(CmscbbVerifyDigestInfo* digestInfo);

CMSCBB_ERROR_CODE CmscbbVerifyCms(CmscbbVrf* vrf, const CmscbbPkcs7Content* p7signed);

CMSCBB_ERROR_CODE CmscbbVerifyVoucherSignature(CmscbbVrf* vrf, const CmscbbPkcs7Content* p7signed,
    CmscbbVoucherContent** voucherContent);

CMSCBB_ERROR_CODE CmsCbbGetSubAlgoWithEncryptedDigest(const CmscbbPkcs7Content* p7signed, CmscbbAoids algoAoid,
    CVB_UINT32 *subAlgo);

CMSCBB_ERROR_CODE CmsCbbCheckAlgoSpecifiedAndGetHashBit(CVB_UINT32 algoSpecified, CVB_UINT32 *hashAlgoBit);

CMSCBB_ERROR_CODE CmsCbbCheckSigAlgoAndGetBit(CVB_UINT32 sigAlgo, CVB_UINT32 *sigAlgoBit);

CMSCBB_ERROR_CODE CmsCbbCheckSubAlgoAndGetBit(CVB_UINT32 sigAlgo, CVB_UINT32 subAlgo, CVB_UINT32 *subAlgoBit);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* H_CMSCBB_P7_VERIFY_H */
