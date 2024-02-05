/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_VERIFY_INFO_H
#define H_CMSCBB_VERIFY_INFO_H
#include "cmscbb_types.h"
#include "cmscbb_cms_def.h"
#include "../asn1/cmscbb_asn1_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Prototype : CmscbbCreateVerifyInfo
 * Params
 *   [IN] info: CmscbbKeyAndAlgInfo
 *   [IN] result:result if success  return CVB_SUCCESS
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbCreateVerifyInfo(CmscbbKeyAndAlgInfo **info);

/*
 * Prototype : CmscbbVerifyInfoInit
 * Params
 *   [IN] pubKey: public Key info
 *   [IN] encAlgo:  signature algorithm
 *   [IN] alg: asn object id
 *   [OUT] verifyInfo : Verify related data.
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbVerifyInfoInit(const CmscbbX509PubkeyInfo* pubKey, CmscbbAoids encAlgo,
    const CmscbbX509AlgoPssIdentifier* alg, CmscbbKeyAndAlgInfo* verifyInfo);

/*
 * Prototype    : CmscbbGetCertVerifyInfo
 * Params
 *   [IN] pubKey: public Key info
 *   [IN] alg:  algorithm
 *   [IN] sigInfo: signature info
 *   [IN] verifyInfo: verifyInfo
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbGetCertVerifyInfo(const CmscbbX509PubkeyInfo* pubKey,
    const CmscbbX509AlgoPssIdentifier* alg, const CmscbbAsnOcts* sigInfo, CmscbbKeyAndAlgInfo* verifyInfo);

/*
 * Prototype    : CmscbbGetCmsVerifyInfo
 * Params
 *   [IN] si: Pkcs7SignedInfo
 *   [IN] cert:  cert
 *   [IN] verifyInfo: verifyInfo
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbGetCmsVerifyInfo(const CmscbbPkcs7SignedInfo* si, const CmscbbX509Cert* cert,
    CmscbbKeyAndAlgInfo* verifyInfo);

/*
 * Prototype    : CmscbbDestoryVerifyInfo
 * Params
 *   [IN] verifyInfo: verifyInfo
 */
CVB_VOID CmscbbDestoryVerifyInfo(CmscbbKeyAndAlgInfo* verifyInfo);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !H_CMSCBB_PKI_COMMON_H */
