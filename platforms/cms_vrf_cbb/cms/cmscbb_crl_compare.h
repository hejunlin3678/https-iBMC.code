/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: yangdingfu
 * Create: 2021
 * History: 2021/02/24 new
 */
#ifndef H_CMSCBB_CRL_COMPARE_H
#define H_CMSCBB_CRL_COMPARE_H
#include "cmscbb_cms_vrf.h"
#include "cmscbb_x509_def.h"
#include "../cmscbb_common/cmscbb_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#if CMSCBB_SUPPORT_CRL_COMPARE

/*
 * Prototype    : CmscbbAddCertCrlToUpdateList
 * Description  : Add cert or crl address info to targetCrlList
 * Params
 *   [OUT] targeCrlList: The target list to add cert.
 *   [IN] toAddCrlAddr: the cert or crl address info to be add
 *   [IN] crlList: the crl info to be updated.
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/27 18:49  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbAddCertCrlToUpdateList(CmscbbCrlToUpdateList* targeCrlList,
    const CmscbbCrlAddressInfo* toAddCrlAddr, const CmscbbCrlToUpdateList* crlList);

/*
 * Prototype    : CmscbbSetCrlAddrInfo
 * Description  : Set address Info by a crl
 * Params
 *   [OUT] addressInfo: to obtain CRL information.
 *   [IN] crl: the crl info to be got
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/27 18:49  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbSetCrlAddrInfo(CmscbbCrlAddressInfo* addressInfo, const CmscbbX509Crl* crl);

/*
 * Prototype    : CmscbbAddCertsToUpdateList
 * Description  : Add certs to updatelist.
 * Params
 *   [OUT] result: The target list to add cert.
 *   [IN] certList: the cert list to be add
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/27 18:49  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbAddCertsToUpdateList(CmscbbCrlToUpdateList* result,
    const LIST_OF(CmscbbX509Cert)* certList);

/*
 * Prototype    : CmscbbCrlListCompare
 * Description  : Compare two crl update lists and return a newer crls list;
 * Params
 *   [IN] crlToUpdate: The update list to be update
 *   [IN] crlOnDevice: The update list on device
 *   [OUT] resultCrlResult: The result crl update list which is newer in between crlToUpdate and crlOndevice
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/27 18:49  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbCrlListCompare(const CmscbbCrlToUpdateList* crlToUpdate,
    const CmscbbCrlToUpdateList* crlOnDevice, CmscbbCrlToUpdateList* resultCrlResult);

/*
 * Prototype    : CmscbbCheckCrlUpdateList
 * Description  : Check crl update list param
 * Params
 *   [IN] updateList: The update list to be check
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/27 18:49  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbCheckCrlUpdateList(const CmscbbCrlToUpdateList* updateList);

/*
 * Prototype    : CmscbbCheckBundleCompareResult
 * Description  : Compare two CRL file information which update
 * Params
 *   [IN] crlToUpdate: The object to compare
 *   [IN] crlOnDevice: The object to compare
 *   [OUT] curState: Results of comparisons,
 *           SCPS_NEW: S1 all CRLs are newer than S2.
 *           SCPS_OLD: S1 all CRLs are older than S2.
 *           SCPS_MIX: Cannot be compared, S1 has a new CRL than S2, and there are older CRLs than S2
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:49  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbCheckBundleCompareResult(const CmscbbCrlBundleInfo *crlToUpdate,
    const CmscbbCrlBundleInfo *crlOnDevice, CmscbbCrlPeriodStat curState);

/*
 * Prototype    : CmscbbFindBundleDuplicateCrlIssuer
 * Description  : To troubleshoot duplicate publishers in the same CRL package
 * Params
 *   [IN] crlToUpdate: CRLs to be updated
 * Return Value : Cvb_true There is a duplicate publisher; Cvb_false No duplicate publisher
 * Remarks      : Repeat the name of the publisher in the CRL, as suggested by Xiao Hui
 *   Date              Author     Modification
 *   2016/04/28 10:11  tangzonglei  Create
 */
CVB_BOOL CmscbbFindBundleDuplicateCrlIssuer(const CmscbbCrlBundleInfo* crlToUpdate);

/*
 * Prototype    : InternalCompareIntercrossCrl
 * Description  : Compare two crl bundle, which contains inter-crossed crl list.
 * Params
 *   [IN] vrf: data for verify context
 *   [IN] s1: crl bundle info 1
 *   [IN] s2: crl bundle info 2
 *   [OUT] curState: Results of comparisons,
 *           SCPS_NEW: S1 all CRLs are newer than S2.
 *           SCPS_OLD: S1 all CRLs are older than S2.
 *           SCPS_MIX: Cannot be compared, S1 has a new CRL than S2, and there are older CRLs than S2
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 14:56  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbCompareBundleIntercrossCrl(const CmscbbCrlBundleInfo* s1, const CmscbbCrlBundleInfo* s2,
    CmscbbCrlPeriodStat* curState);

/*
 * Prototype    : CmscbbCopyRevokeList
 * Description  : Copy revoke List from crlInfo to updatelist
 * Params
 *   [IN/OUT] result: the target crl update list to store revoke list;
 *   [IN/OUT] revokeIter: position to store revokeList in crl update list;
 *   [IN] crlInfo: crl information
 * Return Value : CVB_STATIC CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2021/05/15 16:04  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbCopyRevokeList(CmscbbCrlToUpdateList *result, CVB_INT *revokeIter,
    const CmscbbCrlInfo *crlInfo);

/*
 * Prototype    : CmscbbCrlInfoFree
 * Description  : Free CmscbbCrlInfo
 * Params
 *   [IN/OUT] crlInfo: The crlinfo to be free;
 * Return Value : CVB_VOID
 *   Date              Author     Modification
 *   2021/05/15 16:04  yangdingfu  Create
 */
CVB_VOID CmscbbCrlInfoFree(CmscbbCrlInfo *crlInfo);

/*
 * Prototype    : CmscbbCrlBaseInfoFree
 * Description  : Free CrlBaseInfo
 * Params
 *   [IN/OUT] crlInfo: The crl base info to be free;
 * Return Value : CVB_VOID
 *   Date              Author     Modification
 *   2021/10/25 11:05  wangqinyu  Create
 */
CVB_VOID CmscbbCrlBaseInfoFree(CmscbbCrlBaseInfo* crlInfo);

/*
 * Prototype    : CmscbbCertBaseInfoFree
 * Description  : Free CertBaseInfo
 * Params
 *   [IN/OUT] certInfo: The cert base info to be free;
 * Return Value : CVB_VOID
 *   Date              Author     Modification
 *   2021/10/25 11:05  wangqinyu  Create
 */
CVB_VOID CmscbbCertBaseInfoFree(CmscbbCertBaseInfo* certInfo);

/*
 * Prototype    : CmscbbAddCertsInfoToList
 * Description  : Add certs info to info list;
 * Params
 *   [IN] result: target crls info list to add cert
 *   [IN] certList: cert list to be add.
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/10/25 11:05  wangqinyu  Create
 */
CMSCBB_ERROR_CODE CmscbbAddCertsInfoToList(CmscbbCrlCertInfoList* result, const LIST_OF(CmscbbX509Cert)* certList);

/*
 * Prototype    : CmscbbAddCrlsInfoToList
 * Description  : Add crl info to info list;
 * Params
 *   [IN] result: target crls info list to add CRL
 *   [IN] crlList: CRL list to be add.
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/10/25 11:05  wangqinyu  Create
 */
CMSCBB_ERROR_CODE CmscbbAddCrlsInfoToList(CmscbbCrlCertInfoList* result, const LIST_OF(CmscbbX509Crl)* crlList);

#if CMSCBB_FOR_WIRELESS
/*
 * Prototype    : CmscbbObtainRelatedCrl
 * Description  : Obtaining CRLs Based on Certificates.
 * Params
 *   [IN] ctx: data for verify context
 *   [IN] crlInfo: All CRLs.
 *   [IN] certList: Certificate Collection.
 *   [OUT] crlResult: Related Crl.
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2021/04/23 16:22  zhanghui   Create
 */
CMSCBB_ERROR_CODE CmscbbObtainRelatedCrl(CMSCBB_VRF_CTX ctx, const CmscbbCrlToUpdateList *crlInfo,
    const LIST_OF(CmscbbCertNameInfo) *certList, CmscbbCrlToUpdateList** crlResult);

/*
 * Prototype    : CmscbbGetCertNameInfo
 * Description  : Obtaining Certificate Issuer and Subject Information.
 * Params
 *   [IN] certs: All Certificates.
 *   [OUT] certList: Certificate Issuer and Subject Information Collection.
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2021/04/23 16:22  zhanghui   Create
 */
CMSCBB_ERROR_CODE CmscbbGetCertNameInfo(const LIST_OF(CmscbbX509Cert)* certs, LIST_OF(CmscbbCertNameInfo)* certList);
#endif /* CMSCBB_FOR_WIRELESS */

/*
 * Prototype    : CmscbbGetCrlInfo
 * Description  : get information from CRL
 * Params
 *   [IN] vrf: data for verify context
 *   [IN] crl: X509 crl
 *   [OUT] crlInfo: crl information
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 14:53  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbGetCrlInfo(CmscbbVrf* vrf, const CmscbbX509Crl* crl, CmscbbCrlInfo** crlInfo);

/*
 * Prototype    : CmscbbUpdateCrlBundle
 * Description  : UpdateCrlBundle
 * Params
 *   [IN] vrf: data for verify context
 *   [OUT] result: X509 crl info
 *   [IN] crlList: crl information list
 * Return Value : CMSCBB_ERROR_CODE
 */
CVB_VOID CmscbbUpdateCrlBundle(CmscbbVrf* vrf, CmscbbCrlBundleInfo* result, const LIST_OF(CmscbbX509Crl)* crlList);

#endif /* CMSCBB_SUPPORT_CRL_COMPARE */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* H_CMSCBB_CRL_COMPARE_H */
