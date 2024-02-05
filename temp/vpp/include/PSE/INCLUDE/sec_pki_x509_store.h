/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: the operation of x509 store.
 * Author: mayihui
 * Create: 2021-07-20
 */

#ifndef SEC_PKI_X509_STORE_H
#define SEC_PKI_X509_STORE_H

#include "sec_pki_def.h"

#ifndef IPSI_SEC_NO_PKI

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SEC_PKI_ctxSet1TrustStore
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * void SEC_PKI_ctxSet1TrustStore(SEC_PKI_CTX_S *pstContext, SEC_PKI_X509_STORE_S *trust_store);
 * @endcode
 *
 * @par Purpose
 * set trust store to pki context, and trust store's reference add one.
 *
 * @par Description
 * set trust store to pki context, free the old trust store, and trust store's reference add one.
 *
 * @param[in] pstContext Pointer to SEC_PKI_CTX_S.[NA/NA]
 * @param[in] trust_store Pointer to SEC_PKI_X509_STORE_S.[NA/NA]
 *
 * @retval none
 */
PSELINKDLL void SEC_PKI_ctxSet1TrustStore(SEC_PKI_CTX_S *pstContext, SEC_PKI_X509_STORE_S *trust_store);

/**
 * @defgroup SEC_PKI_ctxGetTrustStore
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * SEC_PKI_X509_STORE_S *SEC_PKI_ctxGetTrustStore(SEC_PKI_CTX_S *pstContext);
 * @endcode
 *
 * @par Purpose
 * get trust store from pki context
 *
 * @par Description
 * get trust store from pki context, only return the address.
 *
 * @param[in] pstContext Pointer to SEC_PKI_CTX_S.[NA/NA]
 *
 * @retval Pointer to SEC_PKI_X509_STORE_S
 */
PSELINKDLL SEC_PKI_X509_STORE_S *SEC_PKI_ctxGetTrustStore(SEC_PKI_CTX_S *pstContext);

/**
 * @defgroup SEC_PKI_ctxSet1VerifyStore
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * void SEC_PKI_ctxSet1VerifyStore(SEC_PKI_CTX_S *pstContext, SEC_PKI_X509_STORE_S *verify_store);
 * @endcode
 *
 * @par Purpose
 * set verify store to pki contxt, and verify store's reference add one.
 *
 * @par Description
 * set verify store to pki contxt, and verify store's reference add one.
 *
 * @param[in] pstContext Pointer to SEC_PKI_CTX_S.[NA/NA]
 * @param[in] verify_store Pointer to SEC_PKI_X509_STORE_S.[NA/NA]
 *
 * @retval None
 */
PSELINKDLL void SEC_PKI_ctxSet1VerifyStore(SEC_PKI_CTX_S *pstContext, SEC_PKI_X509_STORE_S *verify_store);

/**
 * @defgroup SEC_PKI_ctxGetVerifyStore
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * SEC_PKI_X509_STORE_S *SEC_PKI_ctxGetVerifyStore(SEC_PKI_CTX_S *pstContext);
 * @endcode
 *
 * @par Purpose
 * get verify store from pki context, verify store is used for verify cert chain.
 *
 * @par Description
 * get verify store from pki context, verify store is used for verify cert chain..
 *
 * @param[in] pstContext Pointer to SEC_PKI_CTX_S.[NA/NA]
 *
 * @retval Pointer to SEC_PKI_X509_STORE_S
 */
PSELINKDLL SEC_PKI_X509_STORE_S *SEC_PKI_ctxGetVerifyStore(SEC_PKI_CTX_S *pstContext);

/**
 * @defgroup SEC_PKI_objSet1VerifyStore
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * SEC_PKI_X509_STORE_S *SEC_PKI_objSet1VerifyStore(SEC_PKI_OBJ_S *pstObject,
 * SEC_PKI_X509_STORE_S *verify_store);
 * @endcode
 *
 * @par Purpose
 * set verify store to pki obj, verify store is used for verify the cert chain.
 *
 * @par Description
 * set verify store to pki obj, verify store is used for verify the cert chain.
 *
 * @param[in] pstContext Pointer to SEC_PKI_CTX_S.[NA/NA]
 * @param[in] verify_store Pointer to SEC_PKI_X509_STORE_S.[NA/NA]
 *
 * @retval None
 */
PSELINKDLL void SEC_PKI_objSet1VerifyStore(SEC_PKI_OBJ_S *pstObject, SEC_PKI_X509_STORE_S *verify_store);

/**
 * @defgroup SEC_PKI_X509_storeSetVerifyFlags
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * void SEC_PKI_X509_storeSetVerifyFlags(SEC_PKI_X509_STORE_S *store, SEC_UINT flags);
 * @endcode
 *
 * @par Purpose
 * set verify flags to X509 Store. Does not support multithreading.
 *
 * @par Description
 * set verify flags to X509 Store. Does not support multithreading.
 *
 * @param[in] store Pointer to SEC_PKI_X509_STORE_S.[NA/NA]
 * @param[in] flags the verify flags.
 *
 * @retval None
 */
PSELINKDLL void SEC_PKI_X509_storeSetVerifyFlags(SEC_PKI_X509_STORE_S *store, SEC_UINT flags);

/**
 * @defgroup SEC_PKI_X509_storeGetVerifyFlags
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * SEC_UINT SEC_PKI_X509_storeGetVerifyFlags(SEC_PKI_X509_STORE_S *store);
 * @endcode
 *
 * @par Purpose
 * get verify flags from X509 Store. Does not support multithreading.
 *
 * @par Description
 * get verify flags from X509 Store. Does not support multithreading.
 *
 * @param[in] store Pointer to SEC_PKI_X509_STORE_S.[NA/NA]
 *
 * @retval the verify flags
 */
PSELINKDLL SEC_UINT SEC_PKI_X509_storeGetVerifyFlags(SEC_PKI_X509_STORE_S *store);

/**
 * @defgroup SEC_PKI_X509_storeClearVerifyFlags
 * @ingroup PKI_Functions
 * @par Prototype
 * @code
 * void SEC_PKI_X509_storeClearVerifyFlags(SEC_PKI_X509_STORE_S *store, SEC_UINT flags);
 * @endcode
 *
 * @par Purpose
 * clear verify flags from X509 Store. Does not support multithreading.
 *
 * @par Description
 * clear verify flags from X509 Store. Does not support multithreading.
 *
 * @param[in] store Pointer to SEC_PKI_X509_STORE_S.[NA/NA]
 * @param[in] flags the verify flags.
 *
 * @retval None
 */
PSELINKDLL void SEC_PKI_X509_storeClearVerifyFlags(SEC_PKI_X509_STORE_S *store, SEC_UINT flags);
#ifdef __cplusplus
}
#endif

#endif // IPSI_SEC_NO_PKI
#endif // SEC_PKI_X509_STORE_H
