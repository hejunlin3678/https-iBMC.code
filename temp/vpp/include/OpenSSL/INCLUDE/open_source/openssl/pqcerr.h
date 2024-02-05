/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * @file pqcerr.h
 * Description: PQC后量子模块错误码
 * Author: MOSI VPP
 * Create: 2022-11-14
 */

#ifndef HEADER_PQCERR_H
# define HEADER_PQCERR_H

# ifndef HEADER_SYMHACKS_H
#  include <openssl/symhacks.h>
# endif

# include <openssl/opensslconf.h>

# ifndef OPENSSL_NO_PQC

#  ifdef  __cplusplus
extern "C"
#  endif

/*
 * PQC reason codes.
 */
#  define PQC_R_NULL_INPUT                                  101
#  define PQC_R_INVALID_LENGTH                              102
#  define PQC_R_INVALID_ALG                                 103                           

# endif
#endif
