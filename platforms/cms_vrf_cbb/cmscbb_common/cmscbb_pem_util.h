/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: zhanghui
 * Create: 2021
 * History: 2021/01/01 reconstruction by zhanghui
 */

/*
 * File Name          : cmscbb_pem_util.h
 * Author             : tangzonglei
 * Creation Date      : 2015/07/22 15:52:05
 * Detail Description : The list is not thread safe, so the pVrf should in the same thread.
 * History
 *      Date time           Author        Description
 *      2015/07/22 15:52    tangzonglei     new
 */
#ifndef H_CMSCBB_PEM_UTIL_H
#define H_CMSCBB_PEM_UTIL_H

#include "cmscbb_cms_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if CMSCBB_SUPPORT_PEM
/*
 * Prototype    : CmscbbPemDecodeCms
 * Description  : decode PEM format CMS
 * Params
 *   [IN] rawSet: raw bytes set of certificate and crl
 *   [IN] pszPem: pem header
 *   [IN] nSigLength: signed of  length
 *   [OUT] p7signed: pkcs7 singed content
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 12:01  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbPemDecodeCms(CmscbbListDummy *rawSet, const CVB_CHAR* pszPem, CVB_UINT32 nSigLength,
    CmscbbPkcs7Content** p7signed);
#endif

/*
 * Prototype    : CmscbbDecodeStreamCertCrl
 * Description  : decode crl-bundle, which contains both crl list and cert list
 * Params
 *   [IN] rawSet: raw bytes set of certificate and crl
 *   [IN] content: The data for the signature content
 *   [IN] contentLength: the length of signature content
 *   [IN,OUT] certList: a list of X509 certificate
 *   [IN,OUT] crlList: a list of X509 crl
 * Return Value : CVB_STATIC CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 15:06  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeStreamCertCrl(CmscbbListDummy* rawSet, const CVB_BYTE* content,
    CVB_UINT32 contentLength, LIST_OF(CmscbbX509Cert)* certList, LIST_OF(CmscbbX509Crl)* crlList);

/*
 * Prototype    : CmscbbDecodeCertCrl
 * Description  : decode crl-bundle, which contains both crl list and cert list
 * Params
 *   [IN] rawSet: raw bytes set of certificate and crl
 *   [IN] content: The data for the signature content
 *   [IN] contentLength: length of the signature content
 *   [IN,OUT] certList: list of X509 certificate
 *   [IN,OUT] crlList: list of X509 crl
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 14:59  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeCertCrl(CmscbbListDummy* rawSet, const CVB_BYTE* content,
    CVB_UINT32 contentLength, LIST_OF(CmscbbX509Cert)* certList, LIST_OF(CmscbbX509Crl)* crlList);

#if CMSCBB_SUPPORT_CRL_COMPARE
/*
 * Prototype    : CmscbbDecodeAndCheckCertCrl
 * Description  : decode crl and then check the crl num limit;
 * Params
 *   [IN] rawSet: raw bytes set of certificate and crl
 *   [IN] content: to be decode crl;
 *   [IN] contentLength: the length of crl
 *   [IN,OUT] certList: a list of X509 certificate
 *   [IN,OUT] crlList: a list of X509 crl
 * Return Value : CVB_STATIC CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2021/02/21 15:06  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeAndCheckCrl(CmscbbListDummy* rawSet, const CVB_BYTE* content,
    CVB_UINT32 contentLength, LIST_OF(CmscbbX509Cert)* certList, LIST_OF(CmscbbX509Crl)* crlList);
#endif /* CMSCBB_SUPPORT_CRL_COMPARE */

/*
 * Prototype    : CmscbbDecodeSignature
 * Description  : Parsing the CMS signature.
 * Params
 *   [IN] rawSet: raw bytes set of certificate and crl
 *   [IN] signature: to be decode signature;
 *   [IN] sigLen: the length of signature
 *   [OUT] p7signed: CmscbbPkcs7Content
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbDecodeSignature(CmscbbListDummy *rawSet, const CVB_BYTE* signature, CVB_UINT32 sigLen,
    CmscbbPkcs7Content** p7signed);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* H_CMSCBB_PEM_UTIL_H */
