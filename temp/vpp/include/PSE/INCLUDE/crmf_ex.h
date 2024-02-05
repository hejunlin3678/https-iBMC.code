/**

 * Copyright (c) Huawei Technologies Co., Ltd. 2010-2019. All rights reserved.
 * Description: This header file declares all the structures and methods that
                are required to create CRMF standard defined Request Messages
                            ALL RIGHTS RESERVED

------------------------------------------------------------------------------

                              crmf.h

  Project Code: IPSI
   Module Name: CRMF

 * Create: 2010-1-19
        Author: Mallesh
   @brief This header file declares all the structures and methods that
                are required to create CRMF standard defined Request Messages

*/
#ifndef CRMF_EX_H
#define CRMF_EX_H

#include "ipsi_pse_build_conf.h"
#ifndef PSE_SSL

/* SeCert Headers */
#include "sec_sys.h"
#include "sec_error.h"
#include "crmf.h"
#include "x509_basictypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup crmf
 * This section contains the crmf Enums, Structures and Functions */
/**
 * @defgroup crmfEnums
 * @ingroup crmf
 * This section contains the crmf Enums
 */
/**
 * @defgroup crmfStructures
 * @ingroup crmf
 * This section contains the crmf Structures
 */
/**
 * @defgroup crmfFunctions
 * @ingroup crmf
 * This section contains the crmf Functions
 */
/**
* @defgroup CRMF_encodeCertReqMsgs
* @ingroup crmfFunctions
* @par Prototype
* @code
* SEC_UCHAR* CRMF_encodeCertReqMsgs (
* CRMF_CertReqMsgList* pstCertReqMsgs,
SEC_UINT32* pulLength);
* @endcode
*
* @par Description
*
*
* @param[in] pstCertReqMsgs NA [NA]
* @param[in] pulLength NA [NA]
* @param[out] NA NA [NA]
*
* @retval SEC_UCHAR* NA [NA|NA]
*
* @par Dependency
* None
*
* @par Note
* \n
*
* None
*
* @par Related Topics
* None
*/
PSELINKDLL SEC_UCHAR *CRMF_encodeCertReqMsgs(CRMF_CertReqMsgList *pstCertReqMsgs, SEC_UINT32 *pulLength);

/**
 * @defgroup CRMF_decodeCertReqMsgs
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * CRMF_CertReqMsgList* CRMF_decodeCertReqMsgs (
 * SEC_UCHAR* pucEncodedCertReqMsgs,
 * SEC_UINT32 ulEncodedLength,
 * SEC_UINT32* pulBytesDecoded);
 * @endcode
 *
 * @par Description
 *
 *
 * @param[in] pucEncodedCertReqMsgs NA [NA]
 * @param[in] ulEncodedLength NA [NA]
 * @param[in] pulBytesDecoded NA [NA]
 * @param[out] NA NA [NA]
 *
 * @retval CRMF_CertReqMsgList* NA [NA|NA]
 *
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * None
 *
 * @par Related Topics
 * None
 */
PSELINKDLL CRMF_CertReqMsgList *CRMF_decodeCertReqMsgs(SEC_UCHAR *pucEncodedCertReqMsgs, SEC_UINT32 ulEncodedLength,
    SEC_UINT32 *pulBytesDecoded);

/**
 * @defgroup CRMF_createCertTemplate_ex
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * CRMF_CERT_TEMPLATE_S* CRMF_createCertTemplate_ex (
 * CRMF_REQMSG_TYPE_E enReqMsgType);
 * @endcode
 *
 * @par Description
 *
 *
 * @param[in] enReqMsgType The type of the crmf format cert req to create the cert req template [NA]
 * @param[out] CRMF_CERT_TEMPLATE_S*  The crmf cert req template be created[NA]
 *
 * @retval CRMF_CERT_TEMPLATE_S* The crmf cert req template be created [CRMF_CERT_TEMPLATE_S*|NA]
 * @retval CRMF_CERT_TEMPLATE_S* If malloc failed [SEC_NULL|NA]
 *
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * - If any related error occur,should call CRMF_freeCertTemplate() free the return value
 *
 * @par Related Topics
 * None
 */
PSELINKDLL CRMF_CERT_TEMPLATE_S *CRMF_createCertTemplate_ex(CRMF_REQMSG_TYPE_E enReqMsgType);

/**
 * @defgroup CRMF_addVersion
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * SEC_INT CRMF_addVersion (
 * CRMF_CERT_TEMPLATE_S *pstCertTemplate,
 * SEC_UINT32 ulVersion);
 * @endcode
 *
 * @par Description
 *
 *
 * @param[in] pstCertTemplate Pointer to the CRMF Format cert template [NA]
 * @param[in] ulVersion Pointer to the SEC_UINT32 version to add to the cmrf cert template,can only be 2 [2]
 * @param[out] NA NA [NA]
 *
 * @retval SEC_INT on failure [SEC_ERROR|NA]
 * @retval SEC_INT on success [SEC_SUCCESS|NA]
 *
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * None
 *
 * @par Related Topics
 * None
 */
PSELINKDLL SEC_INT CRMF_addVersion(CRMF_CERT_TEMPLATE_S *pstCertTemplate, SEC_UINT32 ulVersion);

/**
 * @defgroup CRMF_addValidity
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * SEC_INT CRMF_addValidity (
 * CRMF_CERT_TEMPLATE_S *pstCertTemplate,
 * CRMF_OPT_VALIDITY_S* pstValidity);
 * @endcode
 *
 * @par Description
 *
 * @param[in] pstCertTemplate Pointer to the CRMF Format cert template [NA]
 * @param[in] pstValidity Pointer to the CRMF_OPT_VALIDITY_S struct to add to the cmrf cert template [NA]
 * @param[out] NA NA [NA]
 *
 * @retval SEC_INT on failure [SEC_ERROR|NA]
 * @retval SEC_INT on success [SEC_SUCCESS|NA]
 *
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * None
 *
 * @par Related Topics
 * None
 */
PSELINKDLL SEC_INT CRMF_addValidity(CRMF_CERT_TEMPLATE_S *pstCertTemplate, CRMF_OPT_VALIDITY_S *pstValidity);

/**
 * @defgroup CRMF_addSubjectName
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * SEC_INT CRMF_addSubjectName (
 * CRMF_CERT_TEMPLATE_S *pstCertTemplate,
 * SEC_NAME_S* pstSubject);
 * @endcode
 *
 * @par Description
 *
 * @param[in] pstCertTemplate Pointer to the CRMF Format cert template [NA]
 * @param[in] pstSubject Pointer to the SEC_NAME_S struct to add to the cmrf cert template [NA]
 * @param[out] NA NA [NA]
 *
 * @retval SEC_INT on failure [SEC_ERROR|NA]
 * @retval SEC_INT on success [SEC_SUCCESS|NA]
 *
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * None
 *
 * @par Related Topics
 * None
 */
PSELINKDLL SEC_INT CRMF_addSubjectName(CRMF_CERT_TEMPLATE_S *pstCertTemplate, SEC_NAME_S *pstSubject);

/**
 * @defgroup CRMF_addPublickeyInfo
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * SEC_INT CRMF_addPublickeyInfo (
 * CRMF_CERT_TEMPLATE_S *pstCertTemplate,
 * SEC_PUBKEY_INFO_S *pstPublicKeyInfo);
 * @endcode
 *
 * @par Description
 *
 * @param[in] pstCertTemplate Pointer to the CRMF Format cert template [NA]
 * @param[in] pstPublicKeyInfo Pointer to the publickeyinfo struct to add to the cmrf cert template [NA]
 * @param[out] NA NA [NA]
 *
 * @retval SEC_INT on  failure [SEC_ERROR|NA]
 * @retval SEC_INT on success [SEC_SUCCESS|NA]
 *
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * None
 *
 * @par Related Topics
 * None
 */
PSELINKDLL SEC_INT CRMF_addPublickeyInfo(CRMF_CERT_TEMPLATE_S *pstCertTemplate, SEC_PUBKEY_INFO_S *pstPublicKeyInfo);

/**
 * @defgroup CRMF_addExtensions
 * @ingroup crmfFunctions
 * @par Prototype
 * @code
 * SEC_INT CRMF_addExtensions (
 * CRMF_CERT_TEMPLATE_S *pstCertTemplate,
 * Extensions *pstExtensions);
 * @endcode
 *
 * @par Description
 *
 *
 * @param[in] pstCertTemplate Pointer to the CRMF Format cert template [NA]
 * @param[in] pstExtensions Pointer to the extensions list to add to the cert template[NA]
 * @param[out] NA NA [NA]
 *
 * @retval SEC_INT on  failure [SEC_ERROR|NA]
 * @retval SEC_INT on success [SEC_SUCCESS|NA]
 * @par Dependency
 * None
 *
 * @par Note
 * \n
 *
 * None
 *
 * @par Related Topics
 * None
 */
PSELINKDLL SEC_INT CRMF_addExtensions(CRMF_CERT_TEMPLATE_S *pstCertTemplate, Extensions *pstExtensions);

/**
 * @defgroup crmf
 * This section contains the crmf Enums, Structures and Functions */
/**
 * @defgroup crmfEnums
 * @ingroup crmf
 * This section contains the crmf Enums
 */
/**
 * @defgroup crmfStructures
 * @ingroup crmf
 * This section contains the crmf Structures
 */
/**
 * @defgroup crmfFunctions
 * @ingroup crmf
 * This section contains the crmf Functions
 */
/**
* @defgroup CRMF_encodeCertReq
* @ingroup crmfFunctions
* @par Prototype
* @code
* SEC_UCHAR* CRMF_encodeCertReq (
* CRMF_CERT_REQ_S* pstCertRequest,
SEC_UINT32* pulLength);
* @endcode
*
* @par Description
* This API is used o encode the Certificate request message.
*
* @param[in] pstCertRequest Ceritificate request [NA]
* @param[in] pulLength length of the request [NA]
*
* @retval SEC_UCHAR* Encoded Certificate message [NA|NA]
*
* @par Dependency
* None
*
* @par Note
* \n
*
* None
*
* @par Related Topics
* None
*/
PSELINKDLL SEC_UCHAR *CRMF_encodeCertReq(CRMF_CERT_REQ_S *pstCertRequest, SEC_UINT32 *pulLength);

#ifdef __cplusplus
}
#endif

#endif

#endif
