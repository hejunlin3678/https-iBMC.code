/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: This header file is public functions of file enc and dec.
 * Author: yangchen
 * Create: 2022-04-16
 * History: None
 */

#ifndef KMC_SRC_SDP_SDP_FILE_H
#define KMC_SRC_SDP_SDP_FILE_H

#include "wsecv2_ctx.h"
#include "wsecv2_type.h"
#include "wsecv2_util.h"
#include "sdpv3_type.h"
#include "sdpv1_itf.h"
#include "sdp_utils.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

unsigned long ReadFileHeaderEnsurePlainLen(KmcCbbCtx *kmcCtx, WsecHandle readStream, SdpCipherFileHeader *fileHeader,
    long *remainLen, WsecBuff *plainBuff);

unsigned long MallocBuffAndReadBodHeader(KmcCbbCtx *kmcCtx, WsecHandle readStream, long *remainLen, WsecBuff *plainBuff,
    WsecBuff *cipherBuff, WsecBuff *bodHeader, WsecUint32 version);

unsigned long ReadBodHeader(KmcCbbCtx *kmcCtx, WsecHandle readStream, long *remainLen, WsecHandle bodHeader,
    WsecUint32 headerLen, WsecTlv *tlv);

/* v1 */
unsigned long MultiSdpFileDecryptV1(KmcCbbCtx *kmcCtx, WsecUint32 domain, const char *cipherFile,
    const char *plainFile, const CallbackSetFileDateTime setFileDateTime);

unsigned long FileEncrypt(KmcCbbCtx *kmcCtx, SdpHeaderId ids, long remainLen, const SdpCipherFileHeader *fileHeader,
    WsecHandle readStream, WsecHandle writeStream, WsecBuff *plainBuff, WsecBuff *cipherBuff);

/* v2 */
unsigned long DoSdpFileEnc(KmcCbbCtx *kmcCtx, WsecUint32 version, WsecUint32 domain, WsecUint32 cipherAlgId,
    WsecUint32 hmacAlgId, const char *plainFile, const char *cipherFile, const CallbackGetFileDateTime getFileDateTime,
    WsecBool *isRemove);

unsigned long FileDecrypt(KmcCbbCtx *kmcCtx, WsecUint32 domain, WsecHandle readStream, WsecHandle writeStream,
    long remainLen, WsecBuff *plainBuff, const WsecBuff *cipherBuff, const WsecHandle sdpbodHeader,
    WsecUint32 bodHeaderLen);

WsecVoid DoSdpFileDecFinal(KmcCbbCtx *kmcCtx, WsecHandle readStream, WsecHandle writeStream, WsecBuff *plainBuff,
    WsecBuff *cipherBuff, SdpCipherFileHeader *fileHeader, const char *plainFile,
    const CallbackSetFileDateTime setFileDateTime, unsigned long *res);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* KMC_SRC_SDP_SDP_FILE_H */
