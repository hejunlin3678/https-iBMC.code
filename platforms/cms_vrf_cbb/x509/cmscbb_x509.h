/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_X509_H
#define H_CMSCBB_X509_H
#include "cmscbb_x509_def.h"
#include "../asn1/cmscbb_asn1_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CMSCBB_X509_KU_ENCIPHER_ONLY 0x0001
#define CMSCBB_X509_KU_CRL_SIGN      0x0002
#define CMSCBB_X509_KU_KEY_CERT_SIGN 0x0004
#define CMSCBB_X509_KU_KEY_AGREEMENT 0x0008
#define CMSCBB_X509_KU_DATA_ENCIPHERMENT 0x0010
#define CMSCBB_X509_KU_KEY_ENCIPHERMENT  0x0020
#define CMSCBB_X509_KU_NON_REPUDIATION   0x0040
#define CMSCBB_X509_KU_DIGITAL_SIGNATURE 0x0080
#define CMSCBB_X509_KU_DECIPHER_ONLY 0x0100

#define CMSCBB_PKI_XKU_SSL_SERVER 0x01
#define CMSCBB_PKI_XKU_SSL_CLIENT 0x02
#define CMSCBB_PKI_XKU_SMIME      0x04
#define CMSCBB_PKI_XKU_CODE_SIGN  0x08
#define CMSCBB_PKI_XKU_SGC       0x10
#define CMSCBB_PKI_XKU_OCSP_SIGN 0x20
#define CMSCBB_PKI_XKU_TIMESTAMP 0x40
#define CMSCBB_PKI_XKU_IPSECIKE  0x80
#define CMSCBB_PKI_XKU_ANYEXTENDEDKEYUSAGE 0x100
#define CMSCBB_PKI_XKU_VOUCHERSIGNING 0x200
#define CMSCBB_PKI_XKU_RELEASE_SIGN 0x400

#define CMSCBB_X509_EXT_KU_CODE_SIGNING (CMSCBB_X509_KU_DIGITAL_SIGNATURE)
#define CMSCBB_X509_EXT_KU_TIMESTAMP_OCSPSIGNING  (CMSCBB_X509_KU_DIGITAL_SIGNATURE | CMSCBB_X509_KU_NON_REPUDIATION)

/*
 * Prototype    : CmscbbX509IsSelfSigned
 * Description  : Check if the certificate is self signed.
 * Params
 *   [IN] pCert: X509 certificate
 *   [OUT] isSelfSigned: selfsigned or not
 * Return Value : CMSCBB_ERROR_CODE
 * History
 *   Date              Author     Modification
 *   2015/11/10 17:14  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbX509IsSelfSigned(CmscbbX509Cert* cert, CVB_BOOL* isSelfSigned);

/*
 * Prototype    : CmscbbX509DecodeCert
 * Description  : decode DER encoded certificate
 * Params
 *   [IN] pbEncodedCert: encode certificate
 *   [IN] nEncodedLength: the length of encode certificate
 *   [OUT] ppCert: X509 certificate
 *   [OUT] bytesDecoded: bytes decode from pbEncodedCert
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 16:02  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbX509DecodeCert(const CVB_BYTE* encodedCert, CVB_UINT32 encodedLength,
    CmscbbX509Cert** cert, CVB_UINT32* bytesDecoded);

/*
 * Prototype    : CmscbbX509FreeCert
 * Description  : free certificate structure
 * Params
 *   [IN] pCert: X509 certificate
 * Return Value : CVB_VOID
 *   Date              Author     Modification
 *   2015/11/10 16:08  tangzonglei  Create
 */
CVB_VOID CmscbbX509FreeCert(CmscbbX509Cert* cert);

/*
 * Prototype    : CmscbbX509DecodeCrl
 * Description  : decode DER encoded CRL
 * Params
 *   [IN] pbEncodedCrl: encode crl
 *   [IN] nEncodedLen: length of encode crl
 *   [OUT] ppCrl: X509 crl
 *   [OUT] bytesDecoded: bytes decode from pbEncodedCrl
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/10 16:09  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbX509DecodeCrl(const CVB_BYTE* encodedCrl, CVB_UINT32 encodedLen, CmscbbX509Crl** crl,
    CVB_UINT32* bytesDecoded);

/*
 * Prototype    : CmscbbX509FreeCrl
 * Description  : free crl structure
 * Params
 *   [IN] pCrl: ppCrl: X509 crl
 * Return Value : CVB_VOID
 *   Date              Author     Modification
 *   2015/11/10 17:04  tangzonglei  Create
 */
CVB_VOID CmscbbX509FreeCrl(CmscbbX509Crl* crl);

/*
 * Prototype    : CmscbbCompareX509Name
 * Description  : Compare two x509 name to get if they are identical.
 * Params
 *   [IN] pNameExpect: Expect X509 name
 *   [IN] pNameActual: Actual X509 name
 * Return Value : CVB_SUCCESS if identical, otherwise not.
 *   Date              Author     Modification
 *   2015/11/10 15:17  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbCompareX509Name(const CmscbbX509Name* nameExpect, const CmscbbX509Name* nameActual);

/*
 * Prototype    : CmscbbX509PubKeyVerify
 * Params
 *   [IN] srcInfo: src content
 *   [IN] sigInfo:  signed content
 *   [IN] nSig: length of  signed content
 *   [IN] pubKey: public key
 *   [IN] alg: asn object id
 *   [IN] result:result if success  return CVB_SUCCESS
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/12/21 9:57  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbX509PubKeyVerify(const CmscbbAsnOcts* srcInfo, const CmscbbAsnOcts* sigInfo,
    const CmscbbX509PubkeyInfo* pubKey, const CmscbbX509AlgoPssIdentifier* alg, CVB_BOOL* result);

/*
 * Prototype : CmscbbCryptoVerifyQuick
 * Params
 *   [IN] srcInfo : src data
 *   [IN] sigInfo: signature data
 *   [IN] verifyInfo : Verify related data.
 *   [OUT] result:result if success  return CVB_SUCCESS
 * Return Value : CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbCryptoVerifyQuick(const CmscbbAsnOcts* srcInfo, const CmscbbAsnOcts* sigInfo,
    CmscbbKeyAndAlgInfo* verifyInfo, CVB_INT *result);

/*
 * Prototype    : CmscbbConvertFromX509Name
 * Params
 *   [IN] pName: X509 Name
 *   [OUT] pszReadableName: convert from pName
 *   [OUT] pNameLen: length of pszReadableName
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/08/17 9:55  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbConvertFromX509Name(const CmscbbX509Name* name, CVB_CHAR** readableName,
    CVB_UINT32* nameLen);

/**
 * Prototype    : CmscbbCheckAlgoPara
 * Description  : check if the PSS parameter is valid.
 * Params
 *   [IN] subjCert: X509 subject certificate
 *   [IN] sigAlgo: signature algo
 * Return Value : CVB_STATIC CMSCBB_ERROR_CODE
 */
CMSCBB_ERROR_CODE CmscbbCheckAlgoPara(const CmscbbX509AlgoPssIdentifier* keyInfo,
    const CmscbbX509AlgoPssIdentifier* sigAlgo);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* H_CMSCBB_X509_H */
