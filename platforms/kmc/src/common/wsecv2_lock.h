/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: KMC internal interfaces are not open to external systems.
 * Author: x00102361
 * Create: 2014-06-16
 */

#ifndef KMC_SRC_COMMON_WSECV2_LOCK_H
#define KMC_SRC_COMMON_WSECV2_LOCK_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WSEC_LOCK_UNGEN = 0, /* The lock is not created. */
    WSEC_LOCK_GEN        /* A lock has been created. */
} WsecLockGen;

/* When a subfunction is called,
 * it is required that the subfunction lock critical resources.
 */
typedef enum {
    KMC_NOT_LOCK, /* No */
    KMC_NEED_LOCK /* Locked */
} KmcLockOrNot;


/* Initialize the lock (process lock + thread lock). */
unsigned long WsecInitializeLock(WsecHandle kmcCtx);

/* Lock destruction */
WsecVoid WsecFinalizeLock(WsecHandle kmcCtx);

/* Adds a thread lock to a resource by ID. */
WsecVoid WsecThreadLockById(WsecHandle kmcCtx, WsecUint32 lockId);

/* Unlocks a resource by ID. */
WsecVoid WsecThreadUnlockById(WsecHandle kmcCtx, WsecUint32 lockId);

/* Adds a process lock to a resource by ID. */
WsecVoid WsecProcLockById(WsecHandle kmcCtx, WsecUint32 lockId);

/* Releases a process by ID. */
WsecVoid WsecProcUnlockById(WsecHandle kmcCtx, WsecUint32 lockId);

/* Obtains the lock status. */
WsecUint32 GetLockStatus(WsecHandle kmcCtx);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* KMC_SRC_COMMON_WSECV2_LOCK_H */
