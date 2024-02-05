/**

 * Copyright (c) Huawei Technologies Co., Ltd. 2005-2019. All rights reserved.
 * Description: This file contains the sec_common functions and enums

------------------------------------------------------------------------------

                              sec_common.h

  Project Code: SeCert
   Module Name: Common Module

 * Create: 2005-07-04
        Author: Sanjay Vasudevan
   @brief: Contains structures and functions required in two or more
   modules.
------------------------------------------------------------------------------
*/
#ifndef SEC_COMMON_H
#define SEC_COMMON_H

#include "ipsi_pse_build_conf.h"

#include "ipsi_types.h"
#include "sec_sys.h"

#include "sec_log.h"

#include "asn-types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSE_VERSION "1.1" /* This is Version 1 Release 1 of PSE lib */

/**
 * @defgroup sec_common
 * This section contains the sec_common Functions and Enums
 */
/* enumeration to hold Certificate type: used in PKCS12,SCEP */
/**
 * @defgroup sec_commonEnum
 * @ingroup sec_common
 * This section contains the sec_common Enum
 */
/**
 * @defgroup SEC_CertType_E
 * @ingroup sec_commonEnum
 * @par Prototype
 * @code
 * typedef enum enSEC_CertType
 * {
 * 	CERT_UNKNOWN = 0,
 * 	CERT_X509    = 1,
 * 	CERT_SDSI    = 2
 * } SEC_CertType_E;
 *
 * @endcode
 *
 * @datastruct CERT_UNKNOWN=0 Unknown certificate type.
 * @datastruct CERT_X509=1 x509 certificate.
 * @datastruct CERT_SDSI=2 SDSI (Simple Distributed Security Infrastructure) certificate.
 */
typedef enum enSEC_CertType {
    CERT_UNKNOWN = 0, /* Unknown certificate type */
    CERT_X509 = 1,    /* x509 certificate */
    CERT_SDSI = 2     /* SDSI (Simple Distributed Security Infrastructure) certificate */
} SEC_CertType_E;
/* end of certtype */
/**
 * @defgroup sec_commonFunctions
 * @ingroup sec_common
 * This section contains the sec_common Functions
 */
/**
 * @defgroup SEC_getPSEVersion
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_CHAR* SEC_getPSEVersion(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * To get the version of the PSE Library.
 *
 * @par Description
 * This function returns the version of the PSE library.
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_CHAR* The version of PSE library [SEC_CHAR*|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL const SEC_CHAR *SEC_getPSEVersion(SEC_VOID);

/**
 * @defgroup SEC_displayPSEVersion
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_VOID SEC_displayPSEVersion(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * To display(print) the version of the PSE Library.
 *
 * @par Description
 * This function displays the version of the PSE library.
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_VOID This function does not return any value [N/A|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * This function internally uses printf to print the PSE Version to the console. Users are advised NOT to
 * use this function in C++ based adapter codes as it may have a conflict with std::cout.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_VOID SEC_displayPSEVersion(SEC_VOID);

/**
 * @defgroup SEC_getCID
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getCID (const SEC_AsnOid_S* pAsnOid);
 * @endcode
 *
 * @par Purpose
 * To get the common ID of a given OID.
 *
 * @par Description
 * This function returns the commmon ID of an OID. It uses the OID table.
 *
 * @param[in] pAsnOid The input OID [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 On Success [The Common ID (CID)|N/A]
 * @retval SEC_UINT32 In case of\n
 * 1. Error (like invalid Oid)\n
 * 2. NULL pointer input\n
 * 3. Empty OID [0(CID_UNKNOWN)|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
// This function is used by SSL. Hence import define added here
PSELINKDLL SEC_UINT32 SEC_getCID(const SEC_AsnOid_S *pAsnOid);

/**
 * @defgroup SEC_getOID
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_AsnOid_S* SEC_getOID (SEC_UINT32 ulCID);
 * @endcode
 *
 * @par Purpose
 * To get the OID corresponding to a common ID.
 *
 * @par Description
 * This function returns the OID to a commmon id.  It uses the OID table. The
 * returned pointer is a pointer into the existing OID table and should not be freed by
 * caller.
 *
 * @param[in] ulCID The input Common ID [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_AsnOid_S* The equivalent OID [SEC_AsnOid_S*|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * The returned OID pointer should not be freed by the user.
 *
 * @par Related Topics
 * N/A
 */
/* returns the AsnOid for a Common ID */
PSELINKDLL SEC_AsnOid_S *SEC_getOID(SEC_UINT32 ulCID);

/**
 * @defgroup SEC_getCIDFromLongOIDName
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getCIDFromLongOIDName (const SEC_CHAR* pucLongOIDName);
 * @endcode
 *
 * @par Purpose
 * To get the CID corresponding to an OID Long name.
 *
 * @par Description
 * This function returns the commmon id of an OID given its Longname.  It uses
 * the OID table.
 *
 * @param[in] pucLongOIDName The Long name of OID [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 The Common ID (CID) [SEC_UNIT32|N/A]
 * @retval SEC_UINT32 If unsuccessful [0 (CID_UNKNOWN)|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_getCIDFromLongOIDName(const SEC_CHAR *pucLongOIDName);

/**
 * @defgroup SEC_getLongOIDName
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_CHAR* SEC_getLongOIDName (SEC_UINT32 ulCID);
 * @endcode
 *
 * @par Purpose
 * To get the OID Long name corresponding to a common ID.
 *
 * @par Description
 * This function returns the Long OID Name corresponding to a commmon id. It
 * uses the OID table. The  returned pointer is a pointer to the OID
 * table and should not be freed by the caller.
 *
 * @param[in] ulCID The input Common ID [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_CHAR* The equivalent Long name [SEC_CHAR*|N/A]
 * @retval SEC_CHAR* If unsuccessful [SEC_NULL|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_CHAR *SEC_getLongOIDName(SEC_UINT32 ulCID);

/**
 * @defgroup SEC_getCIDFromShortOIDName
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getCIDFromShortOIDName (const SEC_CHAR* pucShortOIDName);
 * @endcode
 *
 * @par Purpose
 * To get the CID corresponding to an OID short name.
 *
 * @par Description
 * This function returns the commmon ID of an OID given its short name. It
 * uses the OID table.
 *
 * @param[in] pucShortOIDName The short name of OID [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 The common ID (CID) [any of the values SEC_CID_E |N/A]
 * @retval SEC_UINT32 If unsuccessful [0 (CID_UNKNOWN)|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_getCIDFromShortOIDName(const SEC_CHAR *pucShortOIDName);

/**
 * @defgroup SEC_getShortOIDName
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_CHAR* SEC_getShortOIDName (SEC_UINT32 ulCID);
 * @endcode
 *
 * @par Purpose
 * To get the OID short name corresponding to a common ID.
 *
 * @par Description
 * This function returns the short OID Name corresponding to a commmon id. It
 * uses the OIDtable. The  returned pointer is a pointer into OID
 * table and should not be freed by caller.
 *
 * @param[in] ulCID The input Common ID [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_CHAR* The equivalent short OID Name [SEC_CHAR*|N/A]
 * @retval SEC_CHAR* If unsuccessful [SEC_NULL|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_CHAR *SEC_getShortOIDName(SEC_UINT32 ulCID);

/**
 * @defgroup SEC_compareDates
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * DATE_COMPARE_E SEC_compareDates(const DATETIME_S* pDateA, const DATETIME_S* pDateB, SEC_UINT32 *pulDiffSeconds)
 * @endcode
 *
 * @par Purpose
 * To compare two DATETIME_S structures.
 *
 * @par Description
 * This function reads in two DATETIME_S structures and compares them.  It
 * specifies whether first date is BEFORE,AFTER or SAME as the
 * second date. It also gives the difference between the two
 * dates.
 *
 * @param[in] pDateA The first date [N/A]
 * @param[in] pDateB The second date [N/A]
 * @param[out] pulDiffSeconds The difference between the two
 * 					 dates in seconds [N/A]
 *
 * @retval DATE_COMPARE_E Error in comparison [ERROR_CMP|N/A]
 * @retval DATE_COMPARE_E The two dates are same [EQUAL_DATE|N/A]
 * @retval DATE_COMPARE_E First date is before second date [BEFORE_DATE|N/A]
 * @retval DATE_COMPARE_E First date is after second date [AFTER_DATE|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * The ucUTCSign,ucUTCHour,ucUTCMinute fields of DATETIME_S are
 * not considered in this function.
 *
 * @par Related Topics
 * N/A
 */
// This function is used by SSL.Hence import define has been added.
PSELINKDLL DATE_COMPARE_E SEC_compareDates(const DATETIME_S *pDateA, const DATETIME_S *pDateB,
    SEC_UINT32 *pulDiffSeconds);

/**
 * @defgroup SEC_compareDates_ex
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * DATE_COMPARE_E SEC_compareDates_ex(const DATETIME_S* pDateA, const DATETIME_S* pDateB, SEC_TIME_T *pDiffSeconds)
 * @endcode
 *
 * @par Purpose
 * To compare two DATETIME_S structures.
 *
 * @par Description
 * This function reads in two DATETIME_S structures and compares them. It
 * specifies whether first date is BEFORE,AFTER or SAME as the
 * second date. It also gives the difference between the two
 * dates.
 *
 * @param[in] pDateA The first date [N/A]
 * @param[in] pDateB The second date [N/A]
 * @param[out] pDiffSeconds The difference between the two
 * 					 dates in seconds as SEC_TIME_T[N/A]
 *
 * @retval DATE_COMPARE_E Error in comparison [ERROR_CMP|N/A]
 * @retval DATE_COMPARE_E The two dates are same [EQUAL_DATE|N/A]
 * @retval DATE_COMPARE_E First date is before second date [BEFORE_DATE|N/A]
 * @retval DATE_COMPARE_E First date is after second date [AFTER_DATE|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * \n
 * The ucUTCSign,ucUTCHour,ucUTCMinute fields of DATETIME_S are
 * not considered in this function.
 *
 * @par Related Topics
 * N/A
 */
// This function is used by SSL.Hence import define has been added.
PSELINKDLL DATE_COMPARE_E SEC_compareDates_ex(const DATETIME_S *pDateA, const DATETIME_S *pDateB,
    SEC_TIME_T *pulDiffSeconds);

/**
 * @defgroup SEC_sysTime
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * DATETIME_S* SEC_sysTime()
 * @endcode
 *
 * @par Purpose
 * To get the present system time.
 *
 * @par Description
 * This function obtains the current system time in DATETIME_S structure. It
 * uses Dopra function/ system call to get the current system time in
 * (DATE_T & TIME_T) or time_t structure and then converts that to DATETIME_S format.
 * In case of Dopra usage, it allocates the memory for the DATE_T & TIME_T structure.
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval DATETIME_S* The current system time [DATETIME_S*|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
// This function is used by SSL.Hence import define has been added.
PSELINKDLL DATETIME_S *SEC_sysTime(SEC_VOID);

/**
 * @defgroup SEC_setMaxPSEBuffSize
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_INT32 SEC_setMaxPSEBuffSize(SEC_UINT32 uiBuffSize)
 * @endcode
 *
 * @par Purpose
 * To set the Max PSE Buff Size.
 *
 * @par Description
 * This function set the max Buff Size.Default value is 10 MB.
 *
 * @param[in] SEC_INT32 Max Buff Size. it should in range of [1MB, 64MB] [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 SEC_SUCCESS on success SEC_ERROR on Failure [SEC_INT32|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * - This is a global function and should not be called in multithread. No PSE operation should be running while calling
 * this API. N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_INT32 SEC_setMaxPSEBuffSize(SEC_UINT32 uiBuffSize);

/**
 * @defgroup SEC_getMaxPSEBuffSize
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getMaxPSEBuffSize(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function returns the MAX PSE Buff Size.
 *
 * @par Description
 * This function returns the MAX PSE Buff Size
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 Returns PSE Buff Size[N/A|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 *
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_getMaxPSEBuffSize(SEC_VOID);

/**
 * @defgroup SEC_setMaxASNBuffSize
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_setMaxASNBuffSize(SEC_UINT32 uiBuffSize)
 * @endcode
 *
 * @par Purpose
 * To set the Max ASN Buff Size.
 *
 * @par Description
 * This function set the max ASN Buff Size.Default value is 500 MB.
 *
 * @param[in] SEC_UINT32 Max ASN Buff Size. it should in range of [64MB, 512MB] [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_INT32 SEC_SUCCESS on success SEC_ERROR on Failure [SEC_INT32|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * - This is a global function and should not be called in multithread.
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_INT32 SEC_setMaxASNBuffSize(SEC_UINT32 uiBuffSize);

/**
 * @defgroup SEC_getMaxASNBuffSize
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getMaxASNBuffSize(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * To get the ASN Buff Size.
 *
 * @par Description
 * This function get the ASN Buff Size which is configured..
 *
 * @param[in]  N/A  N/A [N/A]
 * @param[out] N/A  N/A [N/A]
 *
 * @retval SEC_UINT32 Returns configured ASN Buff Size [SEC_UINT32|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_getMaxASNBuffSize(SEC_VOID);

/**
 * @defgroup SEC_setMaxStackDepth
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_INT32 SEC_setMaxStackDepth(SEC_UINT32 uiMaxDepth)
 * @endcode
 *
 * @par Purpose
 * To set the Max Stack Depth.
 *
 * @par Description
 * This function set the max Stack Depth.Default value is 100.
 *
 * @param[in] SEC_UINT32 Max Stack Depth. it should in range of [10, 4095] [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_INT32 SEC_SUCCESS on success SEC_ERROR on Failure [SEC_INT32|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * - This is a global function and should not be called in multithread. Mainly it will have interaction with ASN
 * decoding and encoding.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_INT32 SEC_setMaxStackDepth(SEC_UINT32 uiMaxDepth);

/**
 * @defgroup SEC_getMaxStackDepth
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_getMaxStackDepth(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * To get the Max Stack depth.
 *
 * @par Description
 * This function is to get the max stack depth , which is configured.
 *
 * @param[in]  N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 Returns Max Depth size, which is configured [SEC_UINT32|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_getMaxStackDepth(SEC_VOID);

/**
 * @defgroup SEC_toggleStrictCheck
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_toggleStrictCheck(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * To enable/disable Asn Strict Check.
 *
 * @par Description
 * This function enables/disables ASN strict Check.By Default it is disabled.
 * TLV(Tag Length Value) : If the tag is present then the length field defines the length of the value. So the length
 * field value will never be zero as to write any value in the length should be minimum 1. So if the length field value
 * is 0 than ASN decoding will throwing an error if the strict check is enabled.
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 Returns the current state of strict check flag [SEC_UINT32|N/A]
 * @retval SEC_TRUE If the Strict Check flag is enabled [SEC_UINT32|N/A]
 * @retval SEC_FALSE If strict check flag is disabled  [SEC_UINT32|N/A]
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * - This is a global function and should not be called in multithread. ASN decoding should not be called in parallel to
 * this API
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_toggleStrictCheck(SEC_VOID);

/**
 * @defgroup SEC_isStrictCheckEnable
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_UINT32 SEC_isStrictCheckEnable(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * To get ASN strict check state.
 *
 * @par Description
 * This function Gets the ASN strict Check state.
 *
 * @param[in] N/A N/A[N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_UINT32 SEC_TRUE on enabled SEC_FALSE on disabled [N/A|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * N/A
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_UINT32 SEC_isStrictCheckEnable(SEC_VOID);

/**
* @defgroup SEC_saveDERCodeToFile
* @ingroup sec_commonFunctions
* @image html Security.jpg <b> -# If the access permission for the file is required
  to be configured, then user should not use this API, instead user should save buffer
* in file with the required product access permission.
-# User must take care the file access controls and must provide canonical path when using this API.
  </b>
* @par Prototype
* @code
* SEC_UINT32 SEC_saveDERCodeToFile (const SEC_CHAR* pszDERCode, SEC_UINT32 ulDERCodelen, const SEC_CHAR* pszFileName);
* @endcode
*
* @par Purpose
* To save DER code into a file.
*
* @par Description
* This function writes the DER code from the given buffer into a file.
*
* @param[in] pszDERCode The buffer containing the DER code which is to be written into a file. [N/A]
* @param[in] ulDERCodelen The length of the der code [N/A]
* @param[in] pszFileName The file name to which the DER code will be written to [N/A]
* @param[out] N/A N/A [N/A]
*
* @retval SEC_UINT32 On successful operation [The number of bytes written into the file|N/A]
* @retval SEC_UNIT32 On failure [0|N/A]
*
* @par Dependency
* sec_common.h
*
* @par Note
* \n
* This API just opens the file specified by pszFileName in write mode and writes the contents specified by
* pszDERCode into it. If the file doesn't exist, it may be created newly, but *No* specific access
* permissions are set to the file by this API. Only default permissions as supported by the system API
* will be set. If user wishes to save the contents of pszDERCode into a file with specific access permissions,
* then they can directly save pszDERCode into a file of their choice with required access permissions.
*
* This API also doesn't do any validation of the pszFileName argument apart from normal NULL check. Any path
* canonicalization, path and file name validations should be done by the user before passing to this API.
*
*
* This API doesn't do any additional activity apart from opening a file and saving the contents of pszDERCode to it.
* It is suggested that users can use filesystem APIs directly than using this API. This API is supported only
* for historical reasons and backward compatibility. Users wanting to save data to a file which should not be
* publicly available should NOT use this API.
* \n
* N/A
*
* @par Related Topics
* N/A
*/
PSELINKDLL SEC_UINT32 SEC_saveDERCodeToFile(const SEC_CHAR *pszDERCode, SEC_UINT32 ulDERCodelen,
    const SEC_CHAR *pszFileName);

/**
* @defgroup SEC_readDERCodeFromFile
* @ingroup sec_commonFunctions
* @image html Security.jpg <b> User must take care the file access controls
  and must provide canonical path when using this API. </b>
* @par Prototype
* @code
* SEC_UINT32 SEC_readDERCodeFromFile (SEC_CHAR** pszDERCode, const SEC_CHAR* pszFileName);
* @endcode
*
* @par Purpose
* To read DER code from a file.
*
* @par Description
* This function reads a DER code from a specified file and stores it into the given buffer.
*
* @param[in] pszFileName The file from which the DER code is to be read [N/A]
* @param[in] pszDERCode The buffer to store the DER code [N/A]
*
* @param[out] N/A N/A [N/A]
*
* @retval SEC_UINT32 On successful operation [The number of bytes read from the file|N/A]
* @retval SEC_UNIT32 On failure [0|N/A]
*
* @par Dependency
* sec_common.h
*
* @par Note
* \n
* - This API will allocate memory for the buffer which is storing the file content.
* - The memory should be freed by using function "ipsi_free".
* - File size should be less than or equal to the value set  by the API SEC_setMaxReadFileSize.
*   The default size supported is 256MB.

*
* @par Related Topics
* N/A
*/
// This function is used by SSL. Hence import define added here
PSELINKDLL SEC_UINT32 SEC_readDERCodeFromFile(SEC_CHAR **pszDERCode, const SEC_CHAR *pszFileName);

/**
 * @defgroup SEC_encodeBase64
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_CHAR* SEC_encodeBase64 (const SEC_CHAR* pucInBuf, SEC_UINT32 ulInBufLen );
 * @endcode
 *
 * @par Purpose
 * To encode the given DER code to base 64 format.
 *
 * @par Description
 * This function converts the given DER code to base 64 format.
 *
 * @param[in] pucInBuf Input buffer [N/A]
 * @param[in] ulInBufLen Length of input buffer [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_CHAR* On successful completion [Output buffer containg the data in base 64 format|N/A]
 * @retval SEC_CHAR* On failure [SEC_NULL|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * \n
 * - The buffer allocated by this API for return value should be freed by using function "ipsi_free".
 * - Output buffer length is calculated as ulOutBufLen = ((ulInBufLen / 48) + 1) * 65. If the value is more than 512MB
 * than API will return fail. SEC_setEnocdeBase64MaxSize can be used to set the value for the maximum output buffer
 * length supported by the stack.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_CHAR *SEC_encodeBase64(const SEC_CHAR *pucInBuf, SEC_UINT32 ulInBufLen);

/**
 * @defgroup SEC_decodeBase64
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_CHAR*  SEC_decodeBase64(const SEC_CHAR*  pucInBuf, SEC_UINT32* pulOutBufLen )
 * @endcode
 *
 * @par Purpose
 * To decode from base64 to DER format.
 *
 * @par Description
 * This function is used to decode from base 64 to DER format.
 * In PEM_decode it is used for decoding the object from PEM to the desired format.
 *
 * @param[in] pucInBuf Input buffer , pucInBuf must be null terminated buffer. [N/A]
 * @param[out] pulOutBufLen buffer length [N/A]
 *
 * @retval SEC_CHAR* On successful completion [Output buffer containg the decoded data|N/A]
 * @retval SEC_CHAR* In case of error [SEC_NULL|N/A]
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * \n
 * - The buffer allocated by this API for return value should be freed by using function "ipsi_free".
 * - For the Bigger input buffer , the API will process for longer time to decode each character from input buffer.
 * since ulOutBufLen is calculated as  ((ulInBufLen / 64 ) + 1) * 48.
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_CHAR *SEC_decodeBase64(const SEC_CHAR *pucInBuf, SEC_UINT32 *pulOutBufLen);

/**
* @defgroup SEC_setMaxReadFileSize
* @ingroup sec_commonFunctions
* @par Prototype
* @code
* SEC_INT SEC_setMaxReadFileSize(SEC_UINT32 uiMaxFileSize)

* @endcode
*
* @par Purpose
* To set maximum file size which is used SEC_readDERCodeFromFile. Default value is 256 MB.
*
* @par Description
* This function set maximum file size which is used SEC_readDERCodeFromFile .Default value is 256 MB
*
* @param[in] uiMaxFileSize Maximum size of File which we are reading[N/A]
* @param[out] N/A N/A [N/A]
*
* @retval SEC_INT If value is  set successfully . [IPSI_SUCCESS|N/A]
* @retval SEC_INT If fail to set uiMaxFileSize. [IPSI_FAILURE|N/A]
*
* @par Dependency
* sec_common.h
*
* @par Note
* \n
* - The maximum value supported for uiMaxFileSize is 536870910 bytes
* - This is a global function and should not be called in multithread.
*
* @par Related Topics
* N/A
*/
PSELINKDLL SEC_INT SEC_setMaxReadFileSize(SEC_UINT32 uiMaxFileSize);

/**
 * @defgroup SEC_setEnocdeBase64MaxSize
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_INT SEC_setEnocdeBase64MaxSize(SEC_UINT32 uiMaxEncBase64Size)
 * @endcode
 *
 * @par Purpose
 * To set maximum size retuired as output of SEC_encodeBase64. Default value is 512 MB.
 *
 * @par Description
 * This function is set maximum size retuired as output of SEC_encodeBase64 . Default value is 512 MB
 *
 * @param[in] uiMaxEncBase64Size Maximum size allwoed as output of SEC_encodeBase64 [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval SEC_INT If value is  set successfully . [IPSI_SUCCESS|N/A]
 * @retval SEC_INT If fail to set uiMaxEncBase64Size. [IPSI_FAILURE|N/A]
 *
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * - The Maximum value which can be set to this API 512MB.
 * - The Minimum value which can be set to this API 65.
 * - This is a global function and should not be called in multithread.
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_INT SEC_setEnocdeBase64MaxSize(SEC_UINT32 uiMaxFileSize);

/**
* @defgroup SEC_setMemoryPoolSize
* @ingroup sec_commonFunctions
* @par Prototype
* @code
* SEC_INT SEC_setMemoryPoolSize (SEC_INT uiPoolSize, SEC_INT uiMaxNoPool, SEC_BOOL bMultithread);
* @endcode
*
* @par Purpose
* This function configure the entries allowed in each memory pool , maximum number of pool and used in multithread
* application or not.
*
* @par Description
* This function configure the entries allowed in each pool , maximum number of pool and used in multithread
* application or not. It also create lock to protect global pool when bMultithread is passed as SEC_TRUE.
*
* @param[in] uiPoolSize  Number of entries in a Pool [N/A]
* @param[in] uiMaxNoPool Maximum number of pool allowed [N/A]
* @param[in] bMultithread  This parameter will decide whether to creat lock for protecting memory pool [N/A]
* @param[out] N/A N/A [N/A]
*
* @retval SEC_INT If value is  set successfully . [IPSI_SUCCESS|N/A]
* @retval SEC_INT If fail to set Pool configuration. [IPSI_FAILURE|N/A]
*
*
* @par Dependency
* sec_common.h
*
* @par Note
* \n
* - This function need to call in start of application before any VPP API is called. Also it should not be called in
multithread.
*    It should not be called twice in one stack untill SEC_freeMemoryPool is done for the previous memory pool created.
* - Memory pool will be created dynamically only if application scenario have decoding of CRL and application has
*    called this API  , otherwise pool will not be created
* - There will be two pool created one for X509_REVOKED_S and other for SEC_ListNode_S
* - While decoding of CRL if all the memory of current pool is exhausted, then new pool is created until it reaches
*   maximum allowed by application.
* -	If all memory pool is exhausted (reached Max) then CRL decoding will failed
* - While returning memory to pool, if no memory from the pool is used, then the pool will freed

*
* @par Related Topics
* N/A
*/
PSELINKDLL SEC_INT SEC_setMemoryPoolSize(SEC_INT uiPoolSize, SEC_INT uiMaxNoPool, SEC_BOOL bMultithread);

/**
 * @defgroup SEC_freeMemoryPool
 * @ingroup sec_commonFunctions
 * @par Prototype
 * @code
 * SEC_VOID SEC_freeMemoryPool(SEC_VOID)
 * @endcode
 *
 * @par Purpose
 * This function will free the memory allocated for all the pool and lock if created.
 *
 * @par Description
 * This function will free the memory allocated for all the pool and lock  if created
 *
 * @param[in] N/A N/A [N/A]
 * @param[out] N/A N/A [N/A]
 *
 * @retval N/A N/A [N/A]
 *
 *
 * @par Dependency
 * sec_common.h
 *
 * @par Note
 * \n
 * - Applicatiion must call SEC_freeMemoryPool API just before exit of application
 *
 * @par Related Topics
 * N/A
 */
PSELINKDLL SEC_VOID SEC_freeMemoryPool(SEC_VOID);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif
