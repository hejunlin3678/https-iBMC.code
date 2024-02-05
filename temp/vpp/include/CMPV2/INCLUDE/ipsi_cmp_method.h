/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2009-2019. All rights reserved.
 * Description  : CMP Method Interface
 * Author       : VPP
 * Create       : 14-10-2009
 * FileName     : ipsi_cmp_method.h
 * Version      : 1
 * Function List: None
 * <author>   <date>          <desc>
 */
#ifndef IPSI_CMP_METHOD_H
#define IPSI_CMP_METHOD_H

#include "sec_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

// Macro to set the type for http
#define IPSI_CMP_METHOD_HTTP 1

/**
 * @defgroup IPSI_CMP_TRANS_METH_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef struct stCMPMethod IPSI_CMP_TRANS_METH_S;
 * @endcode
 * @par Description
 * This structure holds the Transport method
 * repository for client to server message transaction.
 *
 */
typedef struct stCMPMethod IPSI_CMP_TRANS_METH_S;

/**
 * @defgroup IPSI_CMP_METH_RDWRFUNC_S
 * @ingroup Structures
 * @par Prototype
 * @code
 * typedef  struct {
 * SEC_INT (*write) (SEC_VOID *pMethData, const SEC_UCHAR * pcBuf,
 * SEC_UINT uiLen);
 * SEC_INT (*read) (SEC_VOID *pMethData, SEC_UCHAR **pcBuf,
 * SEC_UINT *uiLen);
 * }IPSI_CMP_METH_RDWRFUNC_S;
 * @endcode
 * @par Description
 * This structure holds the read and write function
 * repository for reading and writing the messages.
 * @datastruct write To write the messages.
 * @datastruct read To read the messages.
 *
 * @param[in] pMethData Pointer that holds the data
 * Method related information.[NA/NA]
 * @param[in] pcBuf Buffer pointer.[NA/NA]
 * @param[in] uiLen Buffer Length.[NA/NA]
 *
 */
typedef struct {
    // write function
    SEC_INT (*write)(SEC_VOID *pMethData, const SEC_UCHAR *pcBuf, SEC_UINT uiLen);

    // read function
    SEC_INT (*read)(SEC_VOID *pMethData, SEC_UCHAR **pcBuf, SEC_UINT *uiLen);
} IPSI_CMP_METH_RDWRFUNC_S;

/**
 * @defgroup stCMPMethod
 * @ingroup Structures
 * @par Prototype
 * @code
 * struct stCMPMethod
 * {
 * SEC_INT iMethType
 * SEC_VOID *pstMethData
 * IPSI_CMP_METH_RDWRFUNC_S stCmpRdWrFuncs
 * }
 * @endcode
 * @par Description
 * This structure holds the method information
 * repository for reading and writing the messages.
 * @datastruct iMethType Method type.
 * @datastruct pstMethData Pointer that holds the data
 * Method related information.
 * @datastruct stCmpRdWrFuncs Structure that holds the read and write functions.
 *
 */
struct stCMPMethod {
    SEC_INT iMethType;
    SEC_VOID *pstMethData;
    SEC_VOID *pUserData;
    IPSI_CMP_METH_RDWRFUNC_S stCmpRdWrFuncs;
};

#ifdef __cplusplus
}
#endif

#endif /* IPSI_CMP_METHOD_H */
