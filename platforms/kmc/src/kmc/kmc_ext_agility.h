/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: KMC extKsf agility interface
 * Author: zhanghao
 * Create: 2022-10-19
 * Notes: New Create
 */

#ifndef KMC_SRC_KMC_EXT_AGILITY_H
#define KMC_SRC_KMC_EXT_AGILITY_H

#include "wsecv2_type.h"
#include "wsecv2_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int CompareKmcSubTlv(const WsecVoid *p1, const WsecVoid *p2);

WsecVoid FreeKmcSubTlv(WsecHandle kmcCtx, WsecVoid *element, WsecUint32 elementSize);

unsigned long CloneExtSubArr(KmcCbbCtx *kmcCtx, WsecBool withCache, WsecArray srcArr, WsecArray *desArr);

unsigned long WriteExtSubs(KmcCbbCtx *kmcCtx, WsecHandle ksf, WsecHandle *hashCtx, WsecUint32 ctxNum,
    unsigned char extTag, WsecArray subs);

unsigned long ReadExtSubs(KmcCbbCtx *kmcCtx, WsecHandle ksf, WsecHandle *hashCtx, WsecUint32 ctxNum,
    unsigned char hasExt, unsigned char expectExtTag, WsecArray *subs);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // KMC_SRC_KMC_EXT_AGILITY_H
