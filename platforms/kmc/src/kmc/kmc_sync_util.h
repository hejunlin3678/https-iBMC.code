/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC - Key Management Component - Sync MK
 * Author: gaoyu
 * Create: 2022-08-27
 * Notes: This File split from kmcv2_ksm.c since the original file is near 2000 lines.
 */
#ifndef KMC_SRC_KMC_KMC_SYNC_UTIL_H
#define KMC_SRC_KMC_KMC_SYNC_UTIL_H


#include "wsecv2_util.h"
#include "kmcv2_pri.h"
#include "cacv2_pri.h"
#include "wsecv2_order.h"
#include "wsecv2_errorcode.h"
#include "wsecv2_sync_type.h"
#include "kmc_ext_ksm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KMC_SYNC_VERSION_V1  1
#define KMC_DH_SECRETKEY_SALT_LEN 16u
#define KMC_DH_SECRETKEY_ITERATION 1

/* 同步消息类型 */
typedef enum {
    WSEC_SYNC_TYPE_MK = 0, /* reserve 1~9 for mk sync */
    WSEC_SYNC_TYPE_KEY_PARAM = 1,
    WSEC_SYNC_TYPE_NEGO_CAP = 2,
    WSEC_SYNC_TYPE_DH_ALG = 3,
    WSEC_SYNC_TYPE_DH_PUBLIC_KEY = 4,
    WSEC_SYNC_TYPE_SUPPORT_DH = 5,
    WSEC_SYNC_TYPE_ERR = 6,

    WSEC_SYNC_TYPE_CNF_COUNT = 10,
    WSEC_SYNC_TYPE_CNF_HASH_ALG,
    WSEC_SYNC_TYPE_AGENT_RESPONSE = 20,
    WSEC_SYNC_TYPE_UNKNOWN = 30
} KmcSyncMsgType;

/* 同步消息头版本 */
typedef enum {
    WSEC_SYNC_HEADER_VER0,
    WSEC_SYNC_HEADER_VER1,
    WSEC_SYNC_HEADER_VER2
} KmcSyncMsgVersion;

typedef struct KmcDomainInfoTag {
    WsecUint32 domainId;
    WsecUint32 domainType;
} KmcDomainInfo;

typedef struct TagKmcRecvHeaderParam {
    WsecUint32 domainCount;
    WsecUint32 keyCount;
    WsecUint32 rmvDomainCount;
} KmcRecvHeaderParam;

/* 配置信息上下文 ctx 结构 */
typedef struct TagKmcNegoSyncCtx {
    WsecUint32 version;
    WsecBool isPeerNego;
    KmcSyncCtrlParam ctrlParam;
    WsecVoid *sendParam;
    CallbackSendSyncData syncSend;
    WsecVoid *recvParam;
    CallbackRecvSyncData syncRecv;
    KmcRecvSyncInfo *syncInfo;
    KmcDhApis dhApis;
} KmcNegoSyncCtx;

/* 配置消息结构 */
#pragma pack(1)
typedef struct TagKmcSyncCnf {
    WsecUint32 hashAlgId;      /* WSEC_ALGID_SHA256 or WSEC_ALGID_SM3 */
    unsigned char reserve[24];
} KmcSyncCnf;
#pragma pack()

/* 配置信息上下文 ctx 结构 */
typedef struct TagKmcSyncCtx {
    KmcSyncCnf cnf;
    unsigned char reserve[16];
} KmcSyncCtx;

unsigned long MergeAllKsfMem(KmcCbbCtx *kmcCtx, KmcKsfMem *dest, WsecArray baseMkArray, KmcKsfMem *ksfRecvMem,
                             WsecUint32 mergeMode, WsecBool isEncrypt);

/* Send header, called by master or agent */
WsecBool SyncSendHeader(KmcCbbCtx *kmcCtx, KmcSyncMsgType msgType, WsecUint32 msgLen,
                        WsecVoid *param, const CallbackSendSyncData sendSyncData);

unsigned long SyncCheckHeader(KmcCbbCtx *kmcCtx, const KmcSyncMkHeader *header, KmcSyncMsgType expectType,
                              WsecUint16 expectVersion, WsecUint32 expectMsgLen);

unsigned long KmcSendInfoWithHeader(KmcCbbCtx *kmcCtx, KmcSyncMsgType msgType, unsigned char* data, WsecUint32 dataLen,
                                    WsecVoid *sendParam, CallbackSendSyncData syncSend);

unsigned long KmcRecvInfoWithHeader(KmcCbbCtx *kmcCtx, const WsecHandle ctx, KmcSyncMsgType expectType,
                                    unsigned char *data, WsecUint32 dataLen, WsecVoid *recvParam,
                                    CallbackRecvSyncData syncRecv);

unsigned long KmcMasterSyncTwoWayChannel(KmcCbbCtx *kmcCtx, KmcSyncMsgType msgType, unsigned char* sendData,
                                         WsecUint32 sendDataLen, WsecVoid *sendParam, CallbackSendSyncData syncSend,
                                         unsigned char *recvData, WsecUint32 recvDataLen, WsecVoid *recvParam,
                                         CallbackRecvSyncData syncRecv);

unsigned long KmcAgentSyncTwoWayChannel(KmcCbbCtx *kmcCtx, KmcSyncMsgType msgType, unsigned char *recvData,
                                        WsecUint32 recvDataLen, WsecVoid *recvParam,
                                        CallbackRecvSyncData syncRecv, unsigned char *sendData,
                                        WsecUint32 sendDataLen, WsecVoid *sendParam,
                                        CallbackSendSyncData syncSend);

unsigned long KmcCheckDHAlg(KmcCbbCtx *kmcCtx, WsecUint32 *localAlg, WsecUint32 localAlgLen, WsecUint32 *originAlg,
                            WsecUint32 originAlgLen, const unsigned char *algArr, WsecUint32 algLen);

unsigned long KmcAsymGetAlg(KmcCbbCtx *kmcCtx, WsecUint32 *mAlg, WsecUint32 mAlgLen, WsecUint32 *aAlg,
                            WsecUint32 aAlgLen, WsecUint32 *alg, WsecUint32 *encAlg, WsecUint32 *kdfAlg);

unsigned long MasterSendSyncData(KmcCbbCtx *kmcCtx, const WsecHandle ctx, const unsigned char *buff, WsecUint32 len,
                                 WsecVoid *param, CallbackSendSyncData sendSyncData);

unsigned long SyncSendHeaderWithHash(KmcCbbCtx *kmcCtx, const WsecHandle ctx, KmcSyncMsgType msgType, WsecUint32 msgLen,
                                     WsecVoid *param, const CallbackSendSyncData sendSyncData);

unsigned long KmcSyncInitEncParam(KmcCbbCtx *kmcCtx, KmcSynEncInfo *encInfo, WsecBuff *wk);

unsigned long AgentRecvSyncData(KmcCbbCtx *kmcCtx, const WsecHandle ctx, unsigned char *buff, WsecUint32 len,
                                WsecVoid *param, CallbackRecvSyncData recvSyncData);

unsigned long KmcConvertSyncParamToDomainArray(KmcCbbCtx *kmcCtx, KmcSyncParam *syncParam, KmcDomainArray *domainArray);

unsigned long KmcCheckCtrlParam(KmcCbbCtx *kmcCtx, const KmcSyncCtrlParam *ctrlParam, unsigned char *algArr,
                                WsecUint32 *algLen);

unsigned long InitSyncInfo(KmcCbbCtx *kmcCtx, WsecArray opArray, KmcRecvSyncInfo *syncInfo);

void ReleaseRecvSyncInfo(KmcCbbCtx *kmcCtx, KmcRecvSyncInfo *syncInfo);

unsigned long GetDomainArrayFromMem(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, WsecArray opArray, WsecBool isEncrypt);

unsigned long MasterSendGetKeyCount(KmcCbbCtx *kmcCtx, KmcKsfMem *memKeystore, const KmcDomainArray *domainArray,
                                    WsecUint32 *totalKeyCount, WsecBool isSendAsym);

void FillSyncResultInfo(KmcCbbCtx *kmcCtx, const KmcKsfMem *compareKsfMem, const KmcKsfMem *destCompareKsfMem,
                        WsecUint32 recvMode, KmcRecvSyncInfo *syncInfo, WsecBool isEncrypt);

WsecBool CompareMkArrayForUpdateKsf(KmcCbbCtx *kmcCtx, const KmcKsfMem *compareKsfMem,
    const KmcKsfMem *destCompareKsfMem, WsecUint32 recvMode, const KmcRecvSyncInfo *syncInfo, WsecBool isEncrypt);

unsigned long CopyMkInfoSymToAsym(KmcCbbCtx *kmcCtx, KmcMkInfo *srcMkInfo, KmcNovaKeyInfo *destMkInfo);
unsigned long SyncCheckSendDomainArray(KmcCbbCtx *kmcCtx, const KmcDomainArray *domainArray);
unsigned long MasterSyncSendBatchMkHeader(KmcCbbCtx *kmcCtx, const WsecHandle ctx, KmcKsfMem *memKeystore,
    WsecVoid *param, CallbackSendSyncData sendSyncData, WsecBool isEncrypt, WsecUint32 mkCount);
unsigned long MasterSendInitEncPram(KmcCbbCtx *kmcCtx, CacCryptoParam *encParam, KmcSynEncInfo *encInfo);
unsigned long MasterSyncKeyParamHeadInfo(KmcCbbCtx *kmcCtx, const WsecHandle ctx, WsecVoid *param,
                                         CallbackSendSyncData sendSyncData, const KmcDomainArray *domainArray, KmcSynEncInfo *encInfo, WsecUint32 keyCount);
unsigned long CheckAgentData(KmcCbbCtx *kmcCtx, const KmcSyncMkHeader *syncHeader, int *count,
                             ImpExpMkType *mkType, WsecBool isEncrypt, KmcRecvHeaderParam *recvHeaderParam);
unsigned long AgentSyncDecParam(KmcCbbCtx *kmcCtx, const WsecHandle ctx, WsecVoid *param,
    CallbackRecvSyncData recvSyncData, KmcRecvHeaderParam *recvHeaderParam, KmcSynEncInfo *encInfo);
unsigned long CopyMkInfoAsymToSym(KmcCbbCtx *kmcCtx, KmcNovaKeyInfo *srcMkInfo, KmcMkInfo *destMkInfo);
unsigned long AgentRecvAndCheckHash(KmcCbbCtx *kmcCtx, WsecHandle *ctx,
                                    WsecVoid *param, CallbackRecvSyncData recvSyncData,
                                    unsigned char *readHash, WsecUint32 len);
unsigned long SyncCheckRecvDomainArray(KmcCbbCtx *kmcCtx, WsecUint32 recvMode, const KmcRecvSyncInfo *syncInfo);
unsigned long KmcCommonPrepareForDh(KmcCbbCtx *kmcCtx, KmcNegoSyncCtx *negoSyncCtx, WsecUint32 *localSupportDh,
                                    unsigned char *algArr, WsecUint32 *algLen);
unsigned long CreateMemKeyFromPlainInfo(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, ImpExpMkType mkType,
    KmcSyncEncKey *encMk, WsecBool *hasAsymKey, unsigned char *plainText, WsecUint32 plainTextLen, WsecUint8 *keyBuff);
WsecVoid WsecCvtByteOrderForSyncCnf(KmcSyncCnf *cnf, WsecUint32 direction);
unsigned long SyncSendAgentResponse(KmcCbbCtx *kmcCtx, WsecVoid *param, const CallbackSendSyncData sendSyncData,
                                    unsigned long ret);
WsecVoid CvtByteOrderForU32Arr(const WsecUint32 *originArr, WsecUint32 originArrLen, WsecUint32 *outArr,
                               WsecUint32 outArrLen, WsecUint32 direction);

unsigned long SyncCheckEncKey(KmcCbbCtx *kmcCtx, KmcSyncEncKey *encKey);
#if WSEC_COMPILE_ENABLE_TEE
unsigned long CompressOneSendMkToBuff(KmcCbbCtx *kmcCtx, KmcMemMk *memMk, WsecBuff *inMkBuff);
unsigned long CompressOneSendAsymKeyToBuff(KmcCbbCtx *kmcCtx, KmcMemNovaKey *novaKey, WsecBuff *inKeyBuff);
unsigned long CompressOneRecvKeyToBuff(KmcCbbCtx *kmcCtx, KmcSyncEncKey *encKey, WsecBuff *inKeyBuff);
unsigned long CreateMemKeyInCipher(KmcCbbCtx *kmcCtx, KmcKsfMem *ksfMem, KmcTeeAsymKeyInfo *key);
unsigned long CheckKeyLenWithHeaderOverFlow(KmcCbbCtx *kmcCtx, WsecUint32 priKeyLen, WsecUint32 pubKeyLen,
                                            WsecUint32 headerLen, WsecUint32 *buffLen);
#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMC_SYNC_UTIL_H */