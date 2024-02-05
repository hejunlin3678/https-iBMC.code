/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_ERR_DEF_H
#define H_CMSCBB_ERR_DEF_H

#define CMSCBB_ERR_BASE                               0x88000000

#define CMSCBB_ERR_UNDEFINED                          (CVB_UINT32)(-1)
#define CMSCBB_ERR_SYS_BASE                           0x88000000
#define CMSCBB_ERR_SYS_UNKNOWN                        0x88000000
#define CMSCBB_ERR_SYS_MEM_ALLOC                      0x88000001
#define CMSCBB_ERR_SYS_MEM_COPY                       0x88000002
#define CMSCBB_ERR_SYS_MEM_SET                        0x88000003
#define CMSCBB_ERR_SYS_MEM                            0x880000FF
#define CMSCBB_ERR_SYS_FILE_OPEN                      0x88000101
#define CMSCBB_ERR_SYS_FILE_READ                      0x88000102
#define CMSCBB_ERR_SYS_FILE_WRITE                     0x88000103
#define CMSCBB_ERR_SYS_FILE_GET_SIZE                  0x88000104
#define CMSCBB_ERR_SYS_FILE_CLOSE                     0x88000105
#define CMSCBB_ERR_SYS_FILE_TOO_LARGE                 0x88000106
#define CMSCBB_ERR_SYS_BUF_EOF                        0x88000201
#define CMSCBB_ERR_SYS_BUF_GET_OFFSET                 0x88000202
#define CMSCBB_ERR_SYS_BUF_SET_OFFSET                 0x88000203
#define CMSCBB_ERR_SYS_BUF_APPEND                     0x88000204
#define CMSCBB_ERR_SYS_BUF_TOO_LARGE                  0x88000205
#define CMSCBB_ERR_SYS_UTIL_CONVERT                   0x88000301
#define CMSCBB_ERR_SYS_UTIL_B64_DEC                   0x88000302
#define CMSCBB_ERR_SYS_STR                            0x880004FF
#define CMSCBB_ERR_SYS_LIST_OVERFLOW                  0x88000501

#define CMSCBB_ERR_CONTEXT_BASE                       0x88100000
#define CMSCBB_ERR_CONTEXT_UNKNOWN                    0x88100000
#define CMSCBB_ERR_CONTEXT_INVALID_PARAM              0x88100001
#define CMSCBB_ERR_CONTEXT_INVALID_STRUCT             0x88100002
#define CMSCBB_ERR_CONTEXT_INVALID_CALLBACK           0x88100003
#define CMSCBB_ERR_CONTEXT_NO_PKI                     0x88100004
#define CMSCBB_ERR_CONTEXT_RESET_FAILED               0x88100005
#define CMSCBB_ERR_CONTEXT_INVALID_ALGOSPECIFIED      0x88100006
#define CMSCBB_ERR_CONTEXT_INVALID_SUBALGO            0x88100007

#define CMSCBB_ERR_PKI_BASE                           0x88200000
#define CMSCBB_ERR_PKI_UNKNOWN                        0x88200000
#define CMSCBB_ERR_PKI_CERT_ALREADY_EXIST             0x88200001
#define CMSCBB_ERR_PKI_CERT_REVOKED                   0x88200002
#define CMSCBB_ERR_PKI_CERT_INVALID_CONTENT           0x88200003
#define CMSCBB_ERR_PKI_CERT_ISSUER_NOT_FOUND          0x88200004
#define CMSCBB_ERR_PKI_CERT_INVALID_ISSUER            0x88200005
#define CMSCBB_ERR_PKI_CERT_OUT_COMING_ROOT_CERT      0x88200006
#define CMSCBB_ERR_PKI_CERT_UNMATCHED_PURPOSE         0x88200007
#define CMSCBB_ERR_PKI_CERT_DECODE                    0x88200008
#define CMSCBB_ERR_PKI_CERT_VERIFY_FAILED             0x88200009
#define CMSCBB_ERR_PKI_CERT_OLD_VERSION               0x88200010
#define CMSCBB_ERR_PKI_CERT_ADD_TO_STORE              0x88200011
#define CMSCBB_ERR_PKI_CERT_NO_CRL                    0x88200012
#define CMSCBB_ERR_PKI_CERT_DATETIME_NO_VALID_YET     0x88200013
#define CMSCBB_ERR_PKI_CERT_DATETIME_EXPIRED          0x88200014
#define CMSCBB_ERR_PKI_CERT_HASH_ALGO                 0x88200015
#define CMSCBB_ERR_PKI_CERT_SIG_ALGO                  0x88200016
#define CMSCBB_ERR_PKI_CERT_KEYUSAGE                  0x88200017
#define CMSCBB_ERR_PKI_CERT_DEPTH                     0x88200018
#define CMSCBB_ERR_PKI_CERT_NOT_FOUND                 0x88200019
#define CMSCBB_ERR_PKI_CERT_KEYLEN                    0x88200020
#define CMSCBB_ERR_PKI_CRL_POOL_FROZEN                0x88200101
#define CMSCBB_ERR_PKI_CRL_DECODE                     0x88200102
#define CMSCBB_ERR_PKI_CRL_HAS_EXPIRED                0x88200103
#define CMSCBB_ERR_PKI_CRL_INVALID_ISSUER             0x88200104
#define CMSCBB_ERR_PKI_CRL_EMPTY                      0x88200105
#define CMSCBB_ERR_PKI_CRL_INVALID                    0x88200106
#define CMSCBB_ERR_PKI_CRL_ALREADY_EXIST              0x88200107
#define CMSCBB_ERR_PKI_CRL_TOO_MUCH                   0x88200108
#define CMSCBB_ERR_PKI_CRL_DUPLICATE_ISSUER           0x88200109
#define CMSCBB_ERR_PKI_CRL_FAILED_MAPPING             0x88200110
#define CMSCBB_ERR_PKI_CRL_SIG_ALGO                   0x88200111
#define CMSCBB_ERR_PKI_CRYPTO_DIGEST_INIT             0x88200201
#define CMSCBB_ERR_PKI_CRYPTO_DIGEST_UPDATE           0x88200202
#define CMSCBB_ERR_PKI_CRYPTO_DIGEST_FINAL            0x88200203
#define CMSCBB_ERR_PKI_CRYPTO_DIGEST_ALGO_NOT_SUPPORT 0x88200204
#define CMSCBB_ERR_PKI_CMS_ISSUER_NOT_FOUND           0x88200301
#define CMSCBB_ERR_PKI_CMS_MISSING_KEY_ATTR           0x88200302
#define CMSCBB_ERR_PKI_CMS_DIGEST_VALUE_CONFLICT      0x88200303
#define CMSCBB_ERR_PKI_CMS_CONTENT_NOT_SUPPORT        0x88200304
#define CMSCBB_ERR_PKI_CMS_INVALID_PEM                0x88200305
#define CMSCBB_ERR_PKI_CMS_DIGEST_ALGO_NOT_SUPPORT    0x88200306
#define CMSCBB_ERR_PKI_CMS_UPDATE_FAILED              0x88200307
#define CMSCBB_ERR_PKI_CMS_NO_SIGNER_INFO             0x88200308
#define CMSCBB_ERR_PKI_CMS_VERIFY_FAILED              0x88200309
#define CMSCBB_ERR_PKI_CMS_HASH_ALGO                  0x88200310
#define CMSCBB_ERR_PKI_CMS_SIG_ALGO                   0x88200311
#define CMSCBB_ERR_PKI_CMS_DECODE                     0x88200312
#define CMSCBB_ERR_PKI_CMS_HASH_SIGNED_ATTRIBUTE      0x88200313
#define CMSCBB_ERR_PKI_CMS_PSS_PARAMETER_NOT_EQUAL    0x88200314
#define CMSCBB_ERR_PKI_NO_SIGNEDATTR                  0x88200315
#define CMSCBB_ERR_PKI_NO_ECONTENT                    0x88200316
#define CMSCBB_ERR_PKI_WRONG_ECONTYPE                 0x88200317
#define CMSCBB_ERR_PKI_CMS_CURID                      0x88200318
#define CMSCBB_ERR_PKI_TST_ISSUER_NOT_FOUND           0x88200401
#define CMSCBB_ERR_PKI_TST_INFO_VERIFY                0x88200402
#define CMSCBB_ERR_PKI_TST_CONTENT_VERIFY             0x88200403
#define CMSCBB_ERR_PKI_TST_DECODE                     0x88200404
#define CMSCBB_ERR_PKI_ENCAP_CONTENT_VERIFY           0x88200405
#define CMSCBB_ERR_PKI_TST_WRONG_ENCAP_TYPE           0x88200406
#define CMSCBB_ERR_PKI_DER_DECODE                     0x88200501
#define CMSCBB_ERR_PKI_X509_EXT_DEC                   0x88200601

#define CMSCBB_ERR_ASN1_BASE                          0x88300000
#define CMSCBB_ERR_ASN1_UNKNOWN                       0x88300000
#define CMSCBB_ERR_ASN1_TAG_DEC                       0x88300001
#define CMSCBB_ERR_ASN1_TAG_PEEK                      0x88300002
#define CMSCBB_ERR_ASN1_TAG_CHECK                     0x88300003
#define CMSCBB_ERR_ASN1_LEN_DEC                       0x88300101
#define CMSCBB_ERR_ASN1_OCT_DEC                       0x88300201
#define CMSCBB_ERR_ASN1_OCT_PEEK                      0x88300202
#define CMSCBB_ERR_ASN1_OCT_LINK                      0x88300203
#define CMSCBB_ERR_ASN1_ANY_DEC                       0x88300301
#define CMSCBB_ERR_ASN1_ANY_OPTION_ITEM               0x88300302
#define CMSCBB_ERR_ASN1_CHOICE_DEC                    0x88300303
#define CMSCBB_ERR_ASN1_SEQ_DEC                       0x88300401
#define CMSCBB_ERR_ASN1_BITS_DEC                      0x88300501
#define CMSCBB_ERR_ASN1_INT_DEC                       0x88300601
#define CMSCBB_ERR_ASN1_SETOF_DEC                     0x88300701
#define CMSCBB_ERR_ASN1_BOOL_DEC                      0x88300801
#endif /* H_CMSCBB_ERR_DEF_H */
