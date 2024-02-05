/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_PKI_COMMON_H
#define H_CMSCBB_PKI_COMMON_H
#include "cmscbb_cms_vrf.h"
#include "cmscbb_x509_def.h"
#include "../cmscbb_common/cmscbb_def.h"
#include "../asn1/cmscbb_asn1_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Prototype    : CmscbbPkiInit
 * Description  : Initialize PKI context.
 * Params
 *   [IN] vrf: PKI context
 * Return Value : CMSCBB_ERROR_CODE
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:20  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbPkiInit(CmscbbVrf* vrf);

/*
 * Prototype    : CmscbbPkiStoreAddCert
 * Description  : Add certificate into store
 * Params
 *   [IN] vrf: PKI context
 *   [IN] cert: Add certificate pointer
 * Return Value : CMSCBB_ERROR_CODE
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:21  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbPkiStoreAddCert(const CmscbbVrf* vrf, CmscbbX509Cert* cert);

/*
 * Prototype    : CmscbbPkiStoreAddCrl
 * Description  : Add CRL into store.
 * Params
 *   [IN] vrf: PKI context
 *   [IN] crl: Add CRL pointer
 * Return Value : CMSCBB_ERROR_CODE
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:22  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbPkiStoreAddCrl(const CmscbbVrf* vrf, CmscbbX509Crl* crl);

/*
 * Prototype    : CmscbbPkiVerifyCert
 * Description  : Verify a certificate
 * Params
 *   [IN] vrf: PKI context
 *   [IN] cert: Verify certificate pointer
 *   [OUT] result: Verify result
 * Return Value : CMSCBB_ERROR_CODE
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:23  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbPkiVerifyCert(CmscbbVrf* vrf, CmscbbX509Cert* cert, CVB_BOOL resetDepth,
    CVB_BOOL isTsRelative, CVB_BOOL* result);

/*
 * Prototype    : CmscbbPkiFindCertByIssuerSn
 * Description  : Find the certificate from store by name and serial number.
 * Params
 *   [IN] vrf: PKI context
 *   [IN] issuer: Certificate name pointer
 *   [IN] sn:  Certificate serial number pointer
 * Return Value : matched certificate, or CVB_NULL if not found.
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:24  tangzonglei  Create
 */
CmscbbX509Cert* CmscbbPkiFindCertByIssuerSn(const CmscbbVrf* vrf, const CmscbbX509Name* issuer,
    const CmscbbAsnBigint* sn);

CmscbbX509Cert* CmscbbFindCertByIssuerSnInCerts(const LIST_OF(CmscbbX509Cert)* certs,
    const CmscbbX509Name* issuer, const CmscbbAsnBigint* sn);

/*
 * Prototype    : CmscbbPkiUninit
 * Description  : Uninitialize the PKI context.
 * Params
 *   [IN] vrf: PKI context
 * Return Value : CVB_VOID
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:27  tangzonglei  Create
 */
CVB_VOID CmscbbPkiUninit(CmscbbVrf* vrf);

/*
 * Prototype    : InternalVerifyCert
 * Description  : Verify the integrity of the added certificate.
 * Params
 *   [IN] vrf: PKI context
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbPkiVerifyCertList(CmscbbVrf* vrf);

#if CMSCBB_SUPPORT_CRL_COMPARE
/*
 * Prototype    : CmscbbGetCrlIsAuthored
 * Description  : Get the crl which is authorized
 * Params
 *   [IN] vrf: data for verify context
 *   [IN] crlInfo: crl information
 *   [OUT] crl: X509 crl
 * Return Value : CVB_STATIC CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbGetCrlIsAuthored(CmscbbVrf* vrf, CmscbbCrlInfo* crlInfo, const CmscbbX509Crl* crl);
#endif /* CMSCBB_SUPPORT_CRL_COMPARE */

/*
 * Prototype    : CmscbbGetCertList
 * Description  : get certList.
 * Params
 *   [IN] pVrf: PKI context
 *   [OUT] certList: cert List
 *   [OUT] crlList: crl List
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbGetCertAndCrlList(CmscbbVrf* ctx, LIST_OF(CmscbbX509Cert)** certList,
    LIST_OF(CmscbbX509Crl)** crlList);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !H_CMSCBB_PKI_COMMON_H */
