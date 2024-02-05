/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: KMC TA internal tool function
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMCTEE_HUK_H
#define KMCTEE_HUK_H

#include "kmctee_status.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum huk_mode {
    NORMAL_MODE = 0, // HUK pbkdf iter 1000
    FTPM_MODE        // HUK pbkdf iter 2000
};

typedef struct {
    enum huk_mode type;
    WsecUint32 alg_id;           // pbkdfAlgId
    WsecUint32 iteration_count;  // iter count limited[1001,10001]
    const WsecUint8 *pssd;       // password
    WsecUint32 pssd_len;         // password len limited[1,512]
    const WsecUint8 *salt;       // salt
    WsecUint32 salt_len;         // salt len limited[1,60]
    WsecUint8 *derived;          // output key
    WsecUint32 derived_len;      // output key len limited[1,512]
} pbdkf2_huk_data;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // KMCTEE_HUK_H
