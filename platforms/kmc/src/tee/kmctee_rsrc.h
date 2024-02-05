/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: KMC TEE internal resource management module
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_RSRC_H
#define KMC_SRC_TEE_KMCTEE_RSRC_H
#include "kmctee_status.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
struct TagKmcRsrcElement {
    void *resource;
    WsecUint32 type;
    struct TagKmcRsrcElement *next;
};

typedef struct TagKmcRsrcElement KmcRsrcElement;

typedef struct TagKmcRsrcManage {
    KmcRsrcElement *list;
    WsecUint32 size;
} KmcRsrcManage;

KmcTaResult KmcRsrcRegister(KmcRsrcManage *manage, void *resource, WsecUint32 type);

KmcTaResult KmcRsrcRelease(KmcRsrcManage *manage, const void *resource);

void KmcRsrcAllRelease(KmcRsrcManage *manage);

typedef enum TagRsrcType {
    MANAGERS_MALLOC,
    MANAGERS_OPERATION,
    MANAGERS_OBJECT,
    MANAGERS_CTX,
} ManageRsrcType;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // KMCTEE_RSRC_H
