/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Notes
 * Author: VPP
 * Create: 2023-03-13
 * Notes: Notes
 */

#ifndef OSSL_PROVEX_TYPE_API_H
#define OSSL_PROVEX_TYPE_API_H

#include <stddef.h>
#include <openssl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t OSSLX_PROV_HANDLE;

#define OSSLX_INVALID_PROV_HANDLE 0

typedef struct osslx_rand_meth_st {
    int (*seed)(const void *buf, int num);
    int (*bytes)(unsigned char *buf, int num);
    void (*cleanup)(void);
    int (*status)(void);
} OSSLX_RAND_METH;

typedef struct osslx_seed_meth_st {
    size_t (*get_entropy)(void *ctx,
                        unsigned char **pout,
                        int entropy,
                        size_t min_len,
                        size_t max_len,
                        int prediction_resistance);
    void (*cleanup_entropy)(void *ctx,
                            unsigned char *out,
                            size_t outlen);
    size_t (*get_nonce)(void *ctx,
                        unsigned char *pout,
                        unsigned int strength,
                        size_t min_len,
                        size_t max_len);
    int (*seed_generate)(void *vseed,
                        unsigned char *out,
                        size_t outlen,
                        unsigned int strength,
                        int prediction_resistance,
                        const unsigned char *adin,
                        size_t adin_len);
} OSSLX_SEED_METH;

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif // OSSL_PROVEX_TYPE_API_H

