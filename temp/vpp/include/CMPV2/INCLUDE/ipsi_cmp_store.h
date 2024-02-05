/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : CMP Store APIs
 * Author       : VPP
 * Create       : 14-10-2009
 * FileName     : ipsi_cmp_store.h
 * Version      : 1
 * Function List: IPSI_CMP_storeGetCertReqID
 * IPSI_CMP_storeGetStatus
 * IPSI_CMP_storeSetStatusString
 * IPSI_CMP_storeReferCert
 * IPSI_CMP_storeGetCert
 * IPSI_CMP_storeGetConfWaitTime
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_STORE_H
#define IPSI_CMP_STORE_H

#include "ipsi_types.h"
#include "ipsi_cmp_def.h"
#include "x509.h"
#include "cmp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup  IPSI_CMP_storeGetCertReqID
 * @ingroup Store_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeGetCertReqID(
 * const IPSI_CMP_STORE_S *pstCmpStore,
 * SEC_INT *piOutReqId)
 * @endcode
 * @par Purpose
 * This function is used to get the certificate request ID.
 *
 * @par Description
 * IPSI_CMP_storeGetCertReqID function gives the
 * certificate request ID for which this response is received.
 *
 * @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
 * @param[out] piOutReqId Contains the certificate request ID .[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT When library is not initialised. [IPSI_CMP_FAILURE|NA]
 * @retval SEC_INT On invalid arguments are passed. [IPSI_CMP_ERR_INVALID_ARGS|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeGetCertReqID(const IPSI_CMP_STORE_S *pstCmpStore, SEC_INT *piOutReqId);

/**
 * @defgroup  IPSI_CMP_storeGetStatus
 * @ingroup Store_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeGetStatus(
 * const IPSI_CMP_STORE_S *pstCmpStore,
 * CERT_RESP_STATUS_E *enCmpStatus)
 * @endcode
 * @par Purpose
 * This function is used to get the status of store.
 *
 * @par Description
 * IPSI_CMP_storeGetStatus function gives the certificate
 * status value from CMP store.
 *
 * @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
 * @param[out] enCmpStatus Pointer that stores the status type.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT When library is not initalised. [IPSI_CMP_FAILURE|NA]
 * @retval SEC_INT On invalid arguments being passed. [IPSI_CMP_ERR_INVALID_ARGS|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeGetStatus(const IPSI_CMP_STORE_S *pstCmpStore, CERT_RESP_STATUS_E *penCmpStatus);

/**
 * @defgroup  IPSI_CMP_storeSetStatusString
 * @ingroup Store_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeSetStatusString(
 * const IPSI_CMP_STORE_S *pstCmpStore,
 * SEC_List_S *pstStatusStrList)
 * @endcode
 * @par Purpose
 * This function is used to set the status string.
 *
 * @par Description
 * IPSI_CMP_storeSetStatusString function sets the status
 * strings in the CMP store
 *
 * @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
 * @param[in] pstStatusStrList List of status strings.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On arguments validation failure. [IPSI_CMP_ERR_INVALID_ARGS|NA]
 * @retval SEC_INT On all other failure conditions. [IPSI_CMP_FAILURE|NA]
 * @par Note
 * \n
 * Each element within pstStatusStrList is of type SEC_AsnOcts_S.
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeSetStatusString(const IPSI_CMP_STORE_S *pstCmpStore, SEC_List_S *pstStatusStrList);

/**
 * @defgroup IPSI_CMP_storeReferCert
 * @ingroup Store_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeReferCert(
 * const IPSI_CMP_STORE_S *pstCmpStore,
 * X509_CERT_S **pstCert)
 * @endcode
 * @par Purpose
 * This function is used to get the certificate
 *
 * @par Description
 * IPSI_CMP_storeReferCert function retrieves the certificate
 * from the CMP store which is received from the CA server
 *
 * @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
 * @param[out] pstCert Response Certificate received from CA server.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On invalid arguments being passed. [IPSI_CMP_ERR_INVALID_ARGS|NA]
 * @retval SEC_INT On all other failure conditions. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeReferCert(const IPSI_CMP_STORE_S *pstCmpStore, X509_CERT_S **pstRefCert);

/**
 * @defgroup IPSI_CMP_storeGetCert
 * @ingroup Store_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeGetCert(
 * const IPSI_CMP_STORE_S *pstCmpStore,
 * X509_CERT_S **pstCert)
 * @endcode
 * @par Purpose
 * This function is used to get the certificate form store.
 *
 * @par Description
 * IPSI_CMP_storeGetCert function retrieves the certificate
 * from the CMP store which is previously received from the CA server.
 * Same memory which is present in the store will be returned to the user
 * application. Once this certificate is taken from the store then
 * certificate present in the store will be removed.
 *
 * @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
 * @param[out] pstCert Response Certificate received from CA server.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On invalid arguments being passed . [IPSI_CMP_ERR_INVALID_ARGS|NA]
 * @retval SEC_INT Onfailure to allocate memory. [IPSI_CMP_ERR_MALLOC_FAILED|NA]
 * @retval SEC_INT On all other failure conditions. [IPSI_CMP_FAILURE|NA]
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeGetCert(const IPSI_CMP_STORE_S *pstCmpStore, X509_CERT_S **pstOutCert);

/**
* @defgroup IPSI_CMP_storeGetConfWaitTime
* @ingroup Store_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_storeGetConfWaitTime(
*           const IPSI_CMP_STORE_S *pstCmpStore,
*           ipsi_time_t *ulSecs)
* @endcode
* @par Purpose
* This function is used to get the confirm timeout in seconds.
*
* @par Description
* IPSI_CMP_storeGetConfWaitTime function retrieves the confirm
* wait Time in seconds. If user doesnot confirms within this timeout
* ca will revoke the certificate and abort the transaction.
*
* @param[in] pstCmpStore Pointer to IPSI_CMP_STORE_S.[NA/NA]
* @param[out] ulSecs Confirm wait time in seconds.[NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT When Library is not initialised. [IPSI_CMP_FAILURE|NA]
* @retval SEC_INT When Invalid Arguments are Passed. [IPSI_CMP_ERR_INVALID_ARGS|NA]
* @retval SEC_INT When server has not set confirm wait time [IPSI_CMP_ERR_CONF_WAIT_NOT_AVAILABLE|NA]
*
* @par Note
* \n
*	- It will update the time out value irrespective of whether it is
* valid or not.
*	- If timeout given is invalid or negitive, one of the reason for this is
* current time is less than server time.

*/
CMPLINKDLL SEC_INT IPSI_CMP_storeGetConfWaitTime(const IPSI_CMP_STORE_S *pstCmpStore, ipsi_time_t *plSecs);

/**
 * @defgroup IPSI_CMP_storeGetEncryptedPriKey
 * @ingroup Store_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_storeGetEncryptedPriKey(const IPSI_CMP_STORE_S *pstCmpStore,
 *     CRMF_ENCRYPTED_KEY_S **pstEncPrivateKey);
 * @endcode
 *
 * @par Purpose
 * @par Description
 * 从cmpstore 获取数字信封
 *
 * @param[in] pstCmpStore        IPSI_CMP_STORE_S指针.[NA/NA]
 * @param[out] pstEncPrivateKey   获取的数字信封 .[NA/NA]
 *
 * @retval 成功 [IPSI_CMP_SUCCESS|NA]
 * @retval 失败，无效参数 [IPSI_CMP_ERR_INVALID_ARGS|NA]
 * @retval 失败，内存申请失败 [IPSI_CMP_ERR_MALLOC_FAILED|NA]
 * @retval 失败，获取信封失败 [IPSI_CMP_ERR_GET_ENCRYPTED_PRIVKEY_FAILED|NA]
 * @retval 失败，CMP库未初始化 [IPSI_CMP_FAILURE|NA]
 * @par Dependency
 *
 * @par Note
 * @li Memory operation: alloc、free、size
 *    获取出来的 pstEncPrivateKey 通过 CRMF_freeEncryptedKey 接口释放
 * @li Thread safe:
 *     线程安全函数
 * @li OS difference:
 *     无OS差异
 * @li Time consuming:
 *     不耗时
 * @par Related Topics
 * N/A
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_storeGetEncryptedPriKey(const IPSI_CMP_STORE_S *pstCmpStore,
    CRMF_ENCRYPTED_KEY_S **pstEncPrivateKey);

#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_SORE_H */
