/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2021 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef OPENSSL_CTERR_H
# define OPENSSL_CTERR_H
#ifdef OPENSSL_PRAGMA_ONCE
# pragma once
#endif

# include <openssl/opensslconf.h>
# include <openssl/symhacks.h>
# include <openssl/cryptoerr_legacy.h>


# ifndef OPENSSL_NO_CT


/*
 * CT reason codes.
 */
#  define CT_R_BASE64_DECODE_ERROR                         108
#  define CT_R_INVALID_LOG_ID_LENGTH                       100
#  define CT_R_LOG_CONF_INVALID                            109
#  define CT_R_LOG_CONF_INVALID_KEY                        110
#  define CT_R_LOG_CONF_MISSING_DESCRIPTION                111
#  define CT_R_LOG_CONF_MISSING_KEY                        112
#  define CT_R_LOG_KEY_INVALID                             113
#  define CT_R_SCT_FUTURE_TIMESTAMP                        116
#  define CT_R_SCT_INVALID                                 104
#  define CT_R_SCT_INVALID_SIGNATURE                       107
#  define CT_R_SCT_LIST_INVALID                            105
#  define CT_R_SCT_LOG_ID_MISMATCH                         114
#  define CT_R_SCT_NOT_SET                                 106
#  define CT_R_SCT_UNSUPPORTED_VERSION                     115
#  define CT_R_UNRECOGNIZED_SIGNATURE_NID                  101
#  define CT_R_UNSUPPORTED_ENTRY_TYPE                      102
#  define CT_R_UNSUPPORTED_VERSION                         103

# endif
#endif
