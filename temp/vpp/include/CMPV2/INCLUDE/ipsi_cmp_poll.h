/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : CMP Poll
 * Author       : Mallesh K M
 * Create       : 28-12-2009
 * FileName     : ipsi_cmp_poll.h
 * Version      : 1
 * Function List: IPSI_CMP_pollGetPendingReqIds
 * IPSI_CMP_pollGetTimeOut
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_POLL_H
#define IPSI_CMP_POLL_H

#include "ipsi_types.h"
#include "sec_list.h"
#include "sec_sys.h"
#include "ipsi_cmp_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup IPSI_CMP_pollGetPendingReqIds
 * @ingroup Poll_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_pollGetPendingReqIds(
 * const IPSI_CMP_POLL_REQ_CTX_S *psPollReqCtx,
 * SEC_INT iReqId[],
 * SEC_UINT uArrSize,
 * SEC_UINT *puNoOfReqIds)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the pending request IDs
 *
 * @par Description
 * IPSI_CMP_pollGetPendingReqIds function gives the pending request IDs from the
 * poll request context structure.
 * This pair of messages is intended to handle scenarios in which the
 * client needs to poll the server in order to determine the status of
 * an outstanding Initialization Request(ir), Certificate Request(cr),
 * or key update request(kur) transaction.
 *
 * @param[in] psPollReqCtx Pointer to IPSI_CMP_POLL_REQ_CTX_S. [NA/NA]
 * @param[in] iReqId Contains the request IDs.[NA/NA]
 * @param[in] uArrSize Input array size.[NA/NA]
 * @param[out] puNoOfReqIds No of response request Ids pending .[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 * @par Note
 * \n
 * User should allocate memory for the array of two integers and pass the
 * same to this API, then this array it will updated the request IDs.
 * puNoOfReqIds contains the no of request ids updated in the array.
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_pollGetPendingReqIds(const IPSI_CMP_POLL_REQ_CTX_S *psPollReqCtx, SEC_INT iReqId[],
    SEC_UINT uArrSize, SEC_UINT *puNoOfReqIds);

/**
 * @defgroup IPSI_CMP_pollGetTimeOut
 * @ingroup Poll_Functions
 * @par Prototype
 * @code
 * SEC_INT IPSI_CMP_pollGetTimeOut(
 * const IPSI_CMP_POLL_REQ_CTX_S *psPollReqCtx,
 * SEC_INT iRequestID,
 * SEC_UINT *uTimeOut)
 * @endcode
 *
 * @par Purpose
 * This function is used to get the poll timeout.
 *
 * @par Description
 * IPSI_CMP_pollGetTimeOut function is used to get the poll time out value
 * of a particular pending request. It waits for at least or as long
 * as the uTimeOut value before sending another poll request.
 *
 * @param[in] psPollReqCtx Pointer to IPSI_CMP_POLL_REQ_CTX_S.[NA/NA]
 * @param[in] iRequestID Request ID of a pending request.[NA/NA]
 * @param[out] uTimeOut Timeout value in seconds.[NA/NA]
 *
 * @retval SEC_INT On successful execution. [IPSI_CMP_SUCCESS|NA]
 * @retval SEC_INT On all failure conditions. [IPSI_CMP_FAILURE|NA]
 *
 */
CMPLINKDLL SEC_INT IPSI_CMP_pollGetTimeOut(const IPSI_CMP_POLL_REQ_CTX_S *psPollReqCtx, SEC_INT iRequestID,
    SEC_UINT *uTimeOut);

#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_RESP_H */
