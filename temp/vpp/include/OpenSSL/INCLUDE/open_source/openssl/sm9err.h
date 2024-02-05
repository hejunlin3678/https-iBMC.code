/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: sm9 err code.
 * Create on 2023-03-08
 */

 
#ifndef HEADER_SM9ERR_H
# define HEADER_SM9ERR_H
 
# ifndef HEADER_SYMHACKS_H
#  include <openssl/symhacks.h>
# endif
 
# include <openssl/opensslconf.h>
 
# ifndef OPENSSL_NO_SM9
 
#  ifdef  __cplusplus
extern "C"
#  endif

/*
 * SM9 function codes.
 */
/* SM9 - asn1 & encode & decode */
#  define SM9_F_I2D_SM9_SIG                      101
#  define SM9_F_D2I_SM9_SIG                      102
#  define SM9_F_ENCODE_BASE64_SIGN               103
#  define SM9_F_ENCODE_BASE64_ENC                104
#  define SM9_F_ENCODE_BASE64_ENCAP              105
#  define SM9_F_ENCODE_BASE64_EXCHANGE           106
#  define SM9_F_ENCODE_BASE64_KEYGEN             107
#  define SM9_F_DECODE_BASE64_SIGN               108
#  define SM9_F_DECODE_BASE64_ENC                109
#  define SM9_F_DECODE_BASE64_ENCAP              110
#  define SM9_F_DECODE_BASE64_EXCHANGE           111
#  define SM9_F_DECODE_BASE64_KEYGEN             112
/* SM9 - curve */
#  define SM9_F_SM9_G1_POINT_POINT2OCTS          121
#  define SM9_F_SM9_G1_POINT_OCTS2POINT          122
#  define SM9_F_SM9_G2_POINT_POINT2OCTS          123
#  define SM9_F_SM9_G2_POINT_OCTS2POINT          124
#  define SM9_F_FP12_NEW                         125
#  define SM9_F_SM9_G2_POINT_NEW                 126
#  define SM9_F_SM9_G1_POINT_NEW                 127
#  define SM9_F_SM9_GT_ELEMENT_NEW               128
#  define SM9_F_SM9_GT_ELEMENT_CALC              129
#  define SM9_F_SM9_G2_POINT_FROM_OCTETS         130
/* SM9 - lib */
#  define SM9_F_SM9_HASH                         141
#  define SM9_F_SM9_GEN_HA_N                     142
/* SM9 - key */
#  define SM9_F_SM9_KEY_NEW                      161
#  define SM9_F_SM9_KEY_FREE                     162
#  define SM9_F_SM9_KEY_NEW_METHOD               163
#  define SM9_F_SM9_KEY_SET_USAGE                164
#  define SM9_F_SM9_KEY_SET_MASTERPUBKEY         165
#  define SM9_F_SM9_KEY_SET_USERPRIKEY           166
#  define SM9_F_SM9_KEY_SET_PRECALCSIGNG         167
#  define SM9_F_SM9_KEY_GET_G1GENERATOR          168
#  define SM9_F_SM9_KEY_GET_ORDER                169
#  define SM9_F_SM9_SIG_NEW                      170
#  define SM9_F_SM9_SIGN_FREE                    171
#  define SM9_F_SM9_GET_G                        172
#  define SM9_F_SM9_SIG_GEN                      173
#  define SM9_F_SM9_SIG_VERIFY                   174
#  define SM9_F_SM9_KEY_SET_HID                  175
/* SM9 - sign */
#  define SM9_F_SM9_DO_SIGN                      181
#  define SM9_F_SM9_DO_VERIFY                    182
#  define SM9_F_SM9_SIG2OCTS                     183
/* SM9 - enc */
#  define SM9_F_ENC_NEW                          201
#  define SM9_F_ENC_DO_ENC                       202
#  define SM9_F_ENC_DO_DEC                       203
#  define SM9_F_ENC_FREE                         204
#  define SM9_F_ENC_DO_ENC_EX                    205
#  define SM9_F_ENC_DO_DEC_EX                    206
/* SM9 - wrap */
#  define SM9_F_ENCAP_NEW                        221
#  define SM9_F_ENCAP_DO_ENCAP                   222
#  define SM9_F_ENCAP_DO_DECAP                   223
#  define SM9_F_ENCAP_FREE                       224
#  define SM9_F_ENCAP_DO_ENCAP_EX                225
#  define SM9_F_ENCAP_DO_DECAP_EX                226
 
/*
 * SM9 reason codes.
 */
#  define SM9_R_ASN1_ERROR                                  101
#  define SM9_R_BAD_SIGNATURE                               102
#  define SM9_R_BIGNUM_OUT_OF_RANGE                         103
#  define SM9_R_BUFFER_TOO_SMALL                            104
#  define SM9_R_CANNOT_INVERT                               105
#  define SM9_R_COORDINATES_OUT_OF_RANGE                    106
#  define SM9_R_CURVE_DOES_NOT_SUPPORT_ECDH                 107
#  define SM9_R_CURVE_DOES_NOT_SUPPORT_SIGNING              108
#  define SM9_R_D2I_ECPKPARAMETERS_FAILURE                  109
#  define SM9_R_DECODE_ERROR                                110
#  define SM9_R_DISCRIMINANT_IS_ZERO                        111
#  define SM9_R_EC_GROUP_NEW_BY_NAME_FAILURE                112
#  define SM9_R_FIELD_TOO_LARGE                             113
#  define SM9_R_GF2M_NOT_SUPPORTED                          114
#  define SM9_R_GROUP2PKPARAMETERS_FAILURE                  115
#  define SM9_R_I2D_ECPKPARAMETERS_FAILURE                  116
#  define SM9_R_INCOMPATIBLE_OBJECTS                        117
#  define SM9_R_INVALID_ARGUMENT                            118
#  define SM9_R_INVALID_COMPRESSED_POINT                    119
#  define SM9_R_INVALID_COMPRESSION_BIT                     120
#  define SM9_R_INVALID_CURVE                               121
#  define SM9_R_INVALID_DIGEST                              122
#  define SM9_R_INVALID_DIGEST_TYPE                         123
#  define SM9_R_INVALID_ENCODING                            124
#  define SM9_R_INVALID_FIELD                               125
#  define SM9_R_INVALID_FORM                                126
#  define SM9_R_INVALID_GROUP_ORDER                         127
#  define SM9_R_INVALID_KEY                                 128
#  define SM9_R_INVALID_OUTPUT_LENGTH                       129
#  define SM9_R_INVALID_PEER_KEY                            130
#  define SM9_R_INVALID_PENTANOMIAL_BASIS                   131
#  define SM9_R_INVALID_PRIVATE_KEY                         132
#  define SM9_R_INVALID_TRINOMIAL_BASIS                     133
#  define SM9_R_KDF_PARAMETER_ERROR                         134
#  define SM9_R_KEYS_NOT_SET                                135
#  define SM9_R_LADDER_POST_FAILURE                         136
#  define SM9_R_LADDER_PRE_FAILURE                          137
#  define SM9_R_LADDER_STEP_FAILURE                         138
#  define SM9_R_MISSING_OID                                 139
#  define SM9_R_MISSING_PARAMETERS                          140
#  define SM9_R_MISSING_PRIVATE_KEY                         141
#  define SM9_R_NEED_NEW_SETUP_VALUES                       142
#  define SM9_R_NOT_A_NIST_PRIME                            143
#  define SM9_R_NOT_IMPLEMENTED                             144
#  define SM9_R_NOT_INITIALIZED                             145
#  define SM9_R_NO_PARAMETERS_SET                           146
#  define SM9_R_NO_PRIVATE_VALUE                            149
#  define SM9_R_OPERATION_NOT_SUPPORTED                     148
#  define SM9_R_PASSED_NULL_PARAMETER                       149
#  define SM9_R_PEER_KEY_ERROR                              150
#  define SM9_R_PKPARAMETERS2GROUP_FAILURE                  151
#  define SM9_R_POINT_ARITHMETIC_FAILURE                    152
#  define SM9_R_POINT_AT_INFINITY                           153
#  define SM9_R_POINT_COORDINATES_BLIND_FAILURE             154
#  define SM9_R_POINT_IS_NOT_ON_CURVE                       155
#  define SM9_R_RANDOM_NUMBER_GENERATION_FAILED             156
#  define SM9_R_SHARED_INFO_ERROR                           157
#  define SM9_R_SLOT_FULL                                   158
#  define SM9_R_UNDEFINED_GENERATOR                         159
#  define SM9_R_UNDEFINED_ORDER                             160
#  define SM9_R_UNKNOWN_COFACTOR                            161
#  define SM9_R_UNKNOWN_GROUP                               162
#  define SM9_R_UNKNOWN_ORDER                               163
#  define SM9_R_UNSUPPORTED_FIELD                           164
#  define SM9_R_WRONG_CURVE_PARAMETERS                      165
#  define SM9_R_WRONG_ORDER                                 166
#  define SM9_R_UNSUPPORTED_MODE                            167
#  define SM9_R_INVALID_POINT                               168
#  define SM9_R_PAIRING_ERROR                               169
#  define SM9_R_INVALID_SIG                                 170
#  define SM9_R_SIG_VERIFY_FAILED                           171
#  define SM9_R_INVALID_LENGTH                              172
#  define SM9_R_G_CALCULATE_ERROR                           173
#  define SM9_R_INVALID_HASH                                174
#  define SM9_R_INVALID_HID                                 175
#  define SM9_R_INVALID_USAGE                               176
#  define SM9_R_POINT_NOT_AFFINE                            177
 
# endif
#endif
