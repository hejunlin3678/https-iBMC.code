/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: KMC - Key Management Component - Sync MK
 * Author: yangdingfu
 * Create: 2020-11-03
 * Notes: This File split from kmcv2_ksm.c since the original file is near 2000 lines.
 */

#ifndef KMC_SRC_KMC_KMC_SYNC_H
#define KMC_SRC_KMC_KMC_SYNC_H

#include "wsecv2_util.h"
#include "kmcv2_itf.h"
#include "kmcv2_pri.h"
#include "cacv2_pri.h"
#include "kmc_sync_util.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * master send to agent configure msg counter
 * NOTE: NEED UPDATE COUNTER WHEN ADD CONFIGURE COUNTER
 * 1 - hash alg configure
 */
#define KMC_SYNC_CNF_COUNT     1
#define KMC_SYNC_CNF_COUNT_MAX 10
#define KMC_SYNC_MAX_MSG_LEN 512

#define KM_DH_CAP_LEN 8
#define KMC_SYNC_BATCH_KEY_TEE_COUNT 30  // 30 batch mk


#define KMC_SYNC_CAPABILITY_DH_POS 7

#define KMC_SYNC_CAPABILITY_BITMAP(value, pos) (((value) >> (pos)) & 0x1)
#define KMC_SYNC_CAPABILITY_SETBIT(value, pos) ((value) = ((value) | (0x1 << (pos))))
#define KMC_SYNC_CAPABILITY_CLRBIT(value, pos) ((value) = ((value) & ~(0x1 << (pos))))

#define  KMC_SYNC_CAPABILITY_DH(cap1, cap2) ((KMC_SYNC_CAPABILITY_BITMAP(cap1, KMC_SYNC_CAPABILITY_DH_POS)) & (KMC_SYNC_CAPABILITY_BITMAP(cap2, KMC_SYNC_CAPABILITY_DH_POS)))

/* MASTER: indicates that the AGENT sends all shared master keys. */
unsigned long MemMasterSendMkByDomain(WsecUint32 domainId, WsecVoid *param, CallbackSendSyncData sendSyncData);

unsigned long MemMasterSendBatchMk(KmcCbbCtx *kmcCtx, const KmcDomainArray *domainArray,
                                   WsecVoid *param, CallbackSendSyncData sendSyncData, WsecBool isEncrypt,
                                   KmcSynEncInfo *encInfo, WsecBool isSendAsym);

/* The AGENT receives all shared master keys from the master. */
unsigned long MemAgentRecvMkByDomain(KmcCbbCtx *kmcCtx, WsecUint32 recvMode, KmcSyncCtx *syncCtx,
                                     KmcRecvSyncInfo **syncInfo, WsecVoid *param, CallbackRecvSyncData recvSyncData,
                                     WsecBool isEncrypt, KmcSynEncInfo *encInfo);

/* Send cnf count, called by master */
unsigned long SyncSendCnfCount(WsecVoid *param, const CallbackSendSyncData sendSyncData, WsecUint16 sendCnt);

/* Recv agent response, called by master */
unsigned long SyncRecvAgentResponse(KmcCbbCtx *kmcCtx,  WsecVoid *param, const CallbackRecvSyncData recvSyncData);

unsigned long KmcSyncMasterSendCnf(KmcCbbCtx *kmcCtx, WsecVoid *param,
    const CallbackSendSyncData sendSyncData, const CallbackRecvSyncData recvSyncData);

unsigned long KmcSyncAgentRecvCnf(KmcCbbCtx *kmcCtx, WsecHandle *ctx,
    WsecVoid *param, const CallbackSendSyncData sendSyncData, const CallbackRecvSyncData recvSyncData);

void KmcSyncAgentRecvCnfFinal(KmcCbbCtx *kmcCtx, const WsecHandle *ctx);

/* sync mk with nego */
unsigned long KmcSyncCreateCtx(KmcCbbCtx *kmcCtx, WsecHandle *syncCtx, WsecVoid *sendParam, CallbackSendSyncData syncSend, WsecVoid *recvParam, CallbackRecvSyncData syncRecv);

unsigned long KmcSyncSetNegoCapability(WsecHandle syncCtx, WsecBool isNego);

unsigned long KmcSyncSetCtrlParam(KmcCbbCtx *kmcCtx, WsecHandle syncCtx, const KmcSyncCtrlParam *ctrlParam);

unsigned long KmcMasterSyncSendKey(KmcCbbCtx *kmcCtx, WsecHandle syncCtx, KmcSyncParam *syncParam);

unsigned long KmcAgentSyncRecvKey(KmcCbbCtx *kmcCtx, WsecHandle syncCtx);

unsigned long KmcAgentGetRecvResult(KmcCbbCtx *kmcCtx, WsecHandle syncCtx, KmcRecvSyncInfo *syncInfo);

void KmcSyncFinalCtx(KmcCbbCtx *kmcCtx, WsecHandle syncCtx);
/* end */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMC_SYNC_H */
