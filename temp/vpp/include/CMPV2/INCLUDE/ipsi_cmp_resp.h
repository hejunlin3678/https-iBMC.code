/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : CMP Response
 * Author       : Mallesh K M
 * Create       : 14-10-2009
 * FileName     : ipsi_cmp_resp.h
 * Version      : 1
 * Function List: IPSI_CMP_ctxGetResp
 * IPSI_CMP_ctxClearResp
 * IPSI_CMP_respGetReqIds
 * IPSI_CMP_respGetCAList
 * IPSI_CMP_respReferCAList
 * IPSI_CMP_respGetExtraCertList
 * IPSI_CMP_respReferExtraCertList
 * IPSI_CMP_respGetCert
 * IPSI_CMP_respReferCert
 * IPSI_CMP_respGetStatus
 * IPSI_CMP_respGetFailureInfo
 * IPSI_CMP_respGetStatusString
 * IPSI_CMP_respFree
 * IPSI_CMP_respGetStatus_ex
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_RESP_H
#define IPSI_CMP_RESP_H

#include "ipsi_types.h"
#include "sec_list.h"
#include "sec_sys.h"
#include "ipsi_cmp_def.h"
#include "cmp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup IPSI_CMP_ctxGetResp
* @ingroup Response_Functions
* @par Prototype
* @code
* SEC_INT IPSI_CMP_ctxGetResp(
              IPSI_CMP_CTX_S *pstContext,
              IPSI_CMP_RESP_S ** dpstCMPResp)
* @endcode
*
* @par Purpose
* This function is used to get the CMP Response.
*
* @par Description
* IPSI_CMP_ctxGetResp function gives the CMP Response that is
* stored in the CMP Context.
*
* @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
* @param[out] dpstCMPResp Double pointer to IPSI_CMP_RESP_S.[NA/NA]
*
* @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
* @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
*
* @par Note
* \n
* User should free the memory allocated for the CMP Response by
* calling IPSI_CMP_respFree API.
*
*/
CMPLINKDLL SEC_INT IPSI_CMP_ctxGetResp(IPSI_CMP_CTX_S *pstContext, IPSI_CMP_RESP_S **dpstCMPResp);

/**
* @defgroup IPSI_CMP_ctxClearResp
* @ingroup Response_Functions
* @par Prototype
* @code
* SEC_VOID IPSI_CMP_ctxClearResp(
              IPSI_CMP_CTX_S *pstContext)
* @endcode
*
* @par Purpose
* This function is used to Clear the CMP Response.
*
* @par Description
* This function is used to Clear the CMP Response that is
* stored in the CMP Context.
*
* @param[in] pstContext Pointer to IPSI_CMP_CTX_S.[NA/NA]
*
* @retval SEC_VOID This function does not return any value. [NA|NA]
*
* @par Note
* \n
* User should call this API before calling the CMP message Request API's,
* in case if the already obtained response is not processed by calling
* IPSI_CMP_ctxGetResp API.
*
*/
CMPLINKDLL SEC_VOID IPSI_CMP_ctxClearResp(IPSI_CMP_CTX_S *pstContext);

/**
 * @defgroup IPSI_CMP_respGetReqIds
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respGetReqIds(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iReqId[],
 * SEC_UINT uiArrSize,
 * SEC_UINT *puiNoOfReqIds)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the request ids
 *
 * @par Description
 * IPSI_CMP_respGetReqIds function gives the request IDs from the
 * response structure.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iReqId Contains all the request ids.[NA/NA]
 * @param[in] uiArrSize Input array size.[NA/NA]
 * @param[out] puiNoOfReqIds Pointer that holds
 * number of response request IDs .[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * User should pass the array size to the no of certificates that
 * they have requested. In this array it will update the request Ids.
 * puiNoOfReqIds contains the number of request ids updated in the array.
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetReqIds(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iReqId[], SEC_UINT uiArrSize,
    SEC_UINT *puiNoOfReqIds);

/**
 * @defgroup IPSI_CMP_respGetCAList
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respGetCAList(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_List_S **dpstCAList)
 * @endcode
 *
 * @par Purpose
 * This function is used to extract the CA List from the response.
 *
 * @par Description
 * IPSI_CMP_respGetCAList function is used to extract the CA list from
 * response sent by CA server.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[out] dpstCAList Pointer that holds the List of CA Certificates [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * API will allocate the memory pstCAList to free this memory user
 * application has to call the following API SEC_LIST_FREE by passing the
 * X509_freeCert as free function
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetCAList(const IPSI_CMP_RESP_S *pstCMPResp, SEC_List_S **dpstCAList);

/**
 * @defgroup IPSI_CMP_respReferCAList
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respReferCAList(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_List_S **dpstCAList)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the response count.
 *
 * @par Description
 * IPSI_CMP_respReferCAList function is used to extract the CA list from
 * from CA server.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[out] dpstCAList List of CA Certificates [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * Memory will not be allocated only a reference count of the cert is
 * increased
 * To dereference the memory user application has to call the following API
 * SEC_LIST_FREE by passing the X509_freeCert as free function
 */
CMPLINKDLL SEC_INT IPSI_CMP_respReferCAList(const IPSI_CMP_RESP_S *pstCMPResp, SEC_List_S **dpstCAList);

/**
 * @defgroup IPSI_CMP_respGetExtraCertList
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT SEC_INT IPSI_CMP_respGetExtraCertList(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_List_S **dpstExtraCertList)
 * @endcode
 *
 * @par Purpose
 * This function is used to extract the extra cert list.
 *
 * @par Description
 * IPSI_CMP_respGetExtraCertList function is used to extract
 * extra cert list received from the CA server
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[out] dpstExtraCertList Poniter that holds the
 * List of extra certificates. [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * API will allocate the memory pstExtraCertList to free this memory user
 * application has to call the following API SEC_LIST_FREE by passing the
 * X509_freeCert as free function.
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetExtraCertList(const IPSI_CMP_RESP_S *pstCMPResp, SEC_List_S **dpstExtraCertList);

/**
 * @defgroup IPSI_CMP_respReferExtraCertList
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respReferExtraCertList(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_List_S **dpstExtraCertList)
 * @endcode
 *
 * @par Purpose
 * This function is used to read the extra cert list
 *
 * @par Description
 * IPSI_CMP_respReferExtraCertList function is used to read certificate
 * list recieved from the CA server
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[out] dpstExtraCertList Poniter that holds the
 * List of extra certificates. [NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * Memory will not be allocated only a reference count of the cert is
 * increased.
 * To dereference the memory user application has to call the following API
 * SEC_LIST_FREE by passing the X509_freeCert as free function
 */
CMPLINKDLL SEC_INT IPSI_CMP_respReferExtraCertList(const IPSI_CMP_RESP_S *pstCMPResp, SEC_List_S **dpstExtraCertList);

/**
 * @defgroup IPSI_CMP_respGetCert
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respGetCert(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iRequestID,
 * X509_CERT_S **dpstOutCert)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the certificate.
 *
 * @par Description
 * IPSI_CMP_respGetCert function is used to get the certificate from the
 * response structure based on the request ID.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iRequestID Request ID .[NA/NA]
 * @param[out] dpstOutCert Pointer that holds the Output certificate.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * API will allocate the memory for the pstOutCert, application has to call
 * the X509_freeCert API to free this memory.
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetCert(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    X509_CERT_S **dpstOutCert);

/**
 * @defgroup IPSI_CMP_respReferCert
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respReferCert(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iRequestID,
 * X509_CERT_S **dpstOutCert)
 * @endcode
 *
 * @par Purpose
 * This function is used to read the certificate.
 *
 * @par Description
 * IPSI_CMP_respReferCert function is used to read the certificate from the
 * response structure based on the request ID.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iRequestID Request ID.[NA/NA]
 * @param[out] dpstOutCert Pointer that holds the Output certificate .[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * Memory will not be allocated only a reference count of the cert is
 * increased.
 * To dereference the memory user application has to call the following API
 * SEC_LIST_FREE by passing the X509_freeCert as free function.
 */
CMPLINKDLL SEC_INT IPSI_CMP_respReferCert(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    X509_CERT_S **dpstOutCert);

/**
 * @defgroup IPSI_CMP_respGetStatus
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respGetStatus(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iRequestID,
 * CERT_RESP_STATUS_E *penOutStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the status of the particular request.
 *
 * @par Description
 * IPSI_CMP_respGetStatus function is used to get the status of the
 * certificate from the response structure based on the request ID.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iRequestID Request ID .[NA/NA]
 * @param[in] penOutStatus Pointer that holds the Output certificate .[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetStatus(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    CERT_RESP_STATUS_E *penOutStatus);

/**
 * @defgroup  IPSI_CMP_respGetFailureInfo
 * @ingroup Response_Functions
 * @par Prototype
 * @code SEC_INT IPSI_CMP_respGetFailureInfo (
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iRequestID,
 * SEC_AsnBits_S ** dpstCmpFailInfo));
 * @endcode
 * @par Purpose
 * This function is used to get the failure information
 *
 * @par Description
 * IPSI_CMP_respGetFailureInfo function gives the failure
 * information or the string to corresponding request ID.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iRequestID Request ID .[NA/NA]
 * @param[out] dpstCmpFailInfo Pointer that holds
 * the failure information type.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * User has to free the dpstCmpFailInfo using  SEC_freeAsnBits.
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetFailureInfo(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    SEC_AsnBits_S **dpstCmpFailInfo);

/**
 * @defgroup  IPSI_CMP_respGetStatusString
 * @ingroup Response_Functions
 * @par Prototype
 * @code SEC_INT IPSI_CMP_respGetStatusString(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iRequestID,
 * SEC_List_S **dpstStatusStrList)
 * @endcode
 * @par Purpose
 * This function is used to get the status string.
 *
 * @par Description
 * IPSI_CMP_respGetStatusString  function gives the status
 * strings.
 *
 * @param[in] pstCMPResp   Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iRequestID   Request ID [NA/NA]
 * @param[out] dpstStatusStrList  list of status strings.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * API will allocate the memory pstExtraCertList to free this memory
 * user application has to call the following API SEC_LIST_FREE by
 * passing the SEC_freeAsnOcts as free function.
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetStatusString(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    SEC_List_S **dpstStatusStrList);

/**
 * @defgroup IPSI_CMP_respFree
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_VOID IPSI_CMP_respFree(IPSI_CMP_RESP_S *pstCMPResp)
 * @endcode
 *
 * @par Purpose
 * This function is used to free the CMP Response structure.
 *
 * @par Description
 * IPSI_CMP_respFree function is used to free the CMP Response structure.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[out] NA NA [NA/NA]
 *
 * @retval SEC_VOID This function does not return any value. [NA|NA]
 */
CMPLINKDLL SEC_VOID IPSI_CMP_respFree(IPSI_CMP_RESP_S *pstCMPResp);

/**
 * @defgroup IPSI_CMP_respGetStatus_ex
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respGetStatus_ex(
 * const IPSI_CMP_RESP_S *pstCMPResp,
 * SEC_INT iRequestID,
 * CERT_RESP_STATUS_E *penOutStatus)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the status of the particular request.
 *
 * @par Description
 * IPSI_CMP_respGetStatus_ex function is used to get the status of the
 * certificate from the response structure based on the request ID. This API will not clear the errors
 * (from error stack) occured during the API called before IPSI_CMP_respGetStatus_ex.
 *
 * @param[in] pstCMPResp Pointer to IPSI_CMP_RESP_S.[NA/NA]
 * @param[in] iRequestID Request ID .[NA/NA]
 * @param[in] penOutStatus Pointer that holds the Output certificate .[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_respGetStatus_ex(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    CERT_RESP_STATUS_E *penOutStatus);

/**
 * @defgroup IPSI_CMP_respGetEncryptedPriKey
 * @ingroup Response_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_respGetEncryptedPriKey(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
 *     CRMF_ENCRYPTED_KEY_S **pstEncPrivateKey);
 * @endcode
 *
 * @par Purpose
 * @par Description
 * 根据请求id, 从cmpresp 中获取数字信封
 *
 * @param[in] pstCMPResp          IPSI_CMP_RESP_S 指针.[NA/NA]
 * @param[in] iRequestID          请求ID.[NA/NA]
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
CMPLINKDLL SEC_INT IPSI_CMP_respGetEncryptedPriKey(const IPSI_CMP_RESP_S *pstCMPResp, SEC_INT iRequestID,
    CRMF_ENCRYPTED_KEY_S **pstEncPrivateKey);

#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_RESP_H */
