/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file pqchybriderr.h
 * Description: PQC后量子混合模块错误码
 * Author: MOSI VPP
 * Create: 2022-11-14
 */

#ifndef HEADER_PQCHYBRIDERR_H
# define HEADER_PQCHYBRIDERR_H

# ifndef HEADER_SYMHACKS_H
#  include <openssl/symhacks.h>
# endif

# include <openssl/opensslconf.h>

# ifndef OPENSSL_NO_PQCHYBRID

#  ifdef  __cplusplus
extern "C"
#  endif
int ossl_err_load_PQC_HYBRID_strings(void);

/*
 * PQC-HYBRID reason codes.
 */
#  define PQC_HYBRID_R_INVALID_INPUT                               101
#  define PQC_HYBRID_R_NULL_INPUT                                  102
#  define PQC_HYBRID_R_INVALID_LENGTH                              103
#  define PQC_HYBRID_R_UNSUPPORT_ALG                               104                 

# endif
#endif
