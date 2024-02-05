/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: TA return value and macro definition
 * Author: liwei
 * Create: 2021-05-03
 */

#ifndef KMC_SRC_TEE_KMCTEE_STATUS_H
#define KMC_SRC_TEE_KMCTEE_STATUS_H

#include "wsecv2_type.h"
#include "wsecv2_mem.h"
#include "kmctee_pri.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PARAMNUM 4

#define DEFAULT_AEIV_LEN 12 /* ae iv */

#define BYTE_SIZE_OF_BIT 8

#define DEFAULT_BLOCK_LEN 16
#define DEFAULT_TAG_LEN 16
#define DEFAULT_MAXHMAC_LEN 64
#define DEFAULT_MAXHASH_LEN 64

#define DEFAULT_ROOTKEY_SALT 32
#define DEFAULT_ROOTKEY_LEN 128 /* 32 symm key, 64 hmac key + 32 reverse */
#define DEFAULT_ROOTKEYENC_LEN 32
#define DEFAULT_ROOTKEYMAC_LEN 64

#define ENCHMAC_WORKKEY_LEN 16
#define ENCMK_ROOTKEY_SALT 32
#define MKCIPHER_VERSION_1 1
#define MKHMAC_BERSION_1 1

typedef enum TagEnclaveMode {
    ENCLAVE_MODE_ENC,
    ENCLAVE_MODE_DEC
} EnclaveMode;

typedef enum TagEnclaveAsymMode {
    ENCLAVE_ASYM_MODE_SIGN,
    ENCLAVE_ASYM_MODE_DEC,
} EnclaveModeAsym;

typedef struct TagEnclaveMkInfo {
    WsecUint32 mode;
    WsecUint32 symmAlg;
    WsecUint32 hmacAlg;
} EnclaveMkInfo;

#ifndef WSEC_TEE_STATUS_BASE
#define WSEC_TEE_STATUS_BASE 1000
#endif

#define WSEC_TEE_ERROR_CODE(seq) (WSEC_TEE_STATUS_BASE + (seq))

typedef enum TagKmcTaResult {
    WSEC_TEE_SUCCESS = 0,
    WSEC_TEE_FAIL                       =             WSEC_TEE_ERROR_CODE(1),
    /* param error */
    WSEC_TEE_INVALID_ARG                =             WSEC_TEE_ERROR_CODE(2),
    WSEC_TEE_OUTPUT_BUFF_NOT_ENOUGH     =             WSEC_TEE_ERROR_CODE(3),
    WSEC_TEE_INVALID_KEYLEN             =             WSEC_TEE_ERROR_CODE(4),
    WSEC_TEE_MK_CIPHER_LEN_FAILED       =             WSEC_TEE_ERROR_CODE(5),
    WSEC_TEE_INVALID_ALG_ARG            =             WSEC_TEE_ERROR_CODE(6),
    WSEC_TEE_INVALID_TYPE_ARG           =             WSEC_TEE_ERROR_CODE(7),
    WSEC_TEE_INVALID_RKINFO_ARG         =             WSEC_TEE_ERROR_CODE(8),
    WSEC_TEE_INVALID_SALT_ARG           =             WSEC_TEE_ERROR_CODE(9),
    WSEC_TEE_INVALID_IV_ARG             =             WSEC_TEE_ERROR_CODE(10),
    WSEC_TEE_INVALID_INBUFF_ARG         =             WSEC_TEE_ERROR_CODE(11),
    WSEC_TEE_INVALID_OUTBUFF_ARG        =             WSEC_TEE_ERROR_CODE(12),
    WSEC_TEE_INVALID_INMK_ARG           =             WSEC_TEE_ERROR_CODE(13),
    WSEC_TEE_INVALID_OUTMK_ARG          =             WSEC_TEE_ERROR_CODE(14),
    WSEC_TEE_INVALID_OUTHASH_ARG        =             WSEC_TEE_ERROR_CODE(15),
    WSEC_TEE_INVALID_SDPINFO_ARG        =             WSEC_TEE_ERROR_CODE(16),
    WSEC_TEE_INVALID_INDATA_ARG         =             WSEC_TEE_ERROR_CODE(17),
    WSEC_TEE_INVALID_CTX_ARG            =             WSEC_TEE_ERROR_CODE(18),
    WSEC_TEE_INVALID_TAG_ARG            =             WSEC_TEE_ERROR_CODE(19),
    WSEC_TEE_INVALID_MKINFO_ARG         =             WSEC_TEE_ERROR_CODE(20),
    WSEC_TEE_INVALID_STREAMINFO_ARG     =             WSEC_TEE_ERROR_CODE(21),
    WSEC_TEE_INVALID_HASH_ARG           =             WSEC_TEE_ERROR_CODE(22),
    WSEC_TEE_SHORT_CIPHERBUFFER         =             WSEC_TEE_ERROR_CODE(23),
    WSEC_TEE_HMAC_LEN_FAILED            =             WSEC_TEE_ERROR_CODE(24),
    WSEC_TEE_INVALID_COMMAND_ID         =             WSEC_TEE_ERROR_CODE(25),
    WSEC_TEE_INVALID_ITER_NUM           =             WSEC_TEE_ERROR_CODE(26),
    WSEC_TEE_INVALID_KEY_PARAM          =             WSEC_TEE_ERROR_CODE(27),
    WSEC_TEE_KEY_LEN_OVERFLOW           =             WSEC_TEE_ERROR_CODE(28),
    /* tlv */
    WSEC_TEE_NOT_MATCH_TLV_LEN          =             WSEC_TEE_ERROR_CODE(40),
    WSEC_TEE_TLV_CORRUPT                =             WSEC_TEE_ERROR_CODE(41),
    WSEC_TEE_FIND_NUM_ERR               =             WSEC_TEE_ERROR_CODE(42),
    /* resources */
    WSEC_TEE_MEMORY_OUT                 =             WSEC_TEE_ERROR_CODE(50),
    WSEC_TEE_NOT_SUPPORT                =             WSEC_TEE_ERROR_CODE(51),
    WSEC_TEE_MANAGE_NOTFIND             =             WSEC_TEE_ERROR_CODE(52),
    WSEC_TEE_MANAGE_NOT_SUPPORT         =             WSEC_TEE_ERROR_CODE(53),
    WSEC_TEE_RESOURCES_NULL             =             WSEC_TEE_ERROR_CODE(54),
    WSEC_TEE_MEMCPY_FAIL                =             WSEC_TEE_ERROR_CODE(55),
    WSEC_TEE_MEMMOVE_FAIL               =             WSEC_TEE_ERROR_CODE(56),
    /* dec enc hmac */
    WSEC_TEE_HMAC_AUTH_FAIL             =             WSEC_TEE_ERROR_CODE(100),
    WSEC_TEE_CIPHERUPDATE_FAIL          =             WSEC_TEE_ERROR_CODE(101),
    WSEC_TEE_CIPHEFINAL_FAIL            =             WSEC_TEE_ERROR_CODE(102),
    WSEC_TEE_NOT_SUPPORT_SYMM           =             WSEC_TEE_ERROR_CODE(103),
    WSEC_TEE_NOT_SUPPORT_HMAC           =             WSEC_TEE_ERROR_CODE(104),
    WSEC_TEE_NOT_SUPPORT_PBKDF          =             WSEC_TEE_ERROR_CODE(105),
    /* gp */
    WSEC_TEE_ALLOCATION_FAIL            =             WSEC_TEE_ERROR_CODE(151),
    WSEC_TEE_TRANSOB_FAIL               =             WSEC_TEE_ERROR_CODE(152),
    WSEC_TEE_POPULATE_FAIL              =             WSEC_TEE_ERROR_CODE(153),
    WSEC_TEE_SETOPERATIONKEY_FAIL       =             WSEC_TEE_ERROR_CODE(154),
    WSEC_TEE_AEINIT_FAIL                =             WSEC_TEE_ERROR_CODE(155),
    WSEC_TEE_AEUPDATE_FAIL              =             WSEC_TEE_ERROR_CODE(156),
    WSEC_TEE_AEENCFINAL_FAIL            =             WSEC_TEE_ERROR_CODE(157),
    WSEC_TEE_AEDECFINAL_FAIL            =             WSEC_TEE_ERROR_CODE(158),
    WSEC_TEE_TEEMAC_FAIL                =             WSEC_TEE_ERROR_CODE(159),
    WSEC_TEE_TEETYPE_CORRUPTED          =             WSEC_TEE_ERROR_CODE(160),
    WSEC_TEE_DIGEST_FIANL_FAIL          =             WSEC_TEE_ERROR_CODE(161),
    WSEC_TEE_DERIVE_WK_FAIL             =             WSEC_TEE_ERROR_CODE(162),
    WSEC_TEE_DERIVE_RK_FAIL             =             WSEC_TEE_ERROR_CODE(163),
    WSEC_TEE_INVALID_TAG_LEN            =             WSEC_TEE_ERROR_CODE(164),
    WSEC_TEE_INVALID_PARAM_TYPE         =             WSEC_TEE_ERROR_CODE(165),
    WSEC_TEE_SET_SOFT_SEC_FAIL          =             WSEC_TEE_ERROR_CODE(166),
    WSEC_TEE_GETOBJATTR_FAIL            =             WSEC_TEE_ERROR_CODE(167),
    WSEC_TEE_VERIFY_DIGEST_FAIL         =             WSEC_TEE_ERROR_CODE(168),
    /* sync */
    WSEC_TEE_IN_AND_OUTPUT_BUFF_NOT_MATCH      =      WSEC_TEE_ERROR_CODE(300),
    WSEC_TEE_CIPHER_MK_LEN_ERROR        =             WSEC_TEE_ERROR_CODE(301),
    WSEC_TEE_GENERATEKEY_FAILED         =             WSEC_TEE_ERROR_CODE(302),
    WSEC_TEE_GETDHSK_FAILED             =             WSEC_TEE_ERROR_CODE(303),
    WSEC_TEE_GETDHPUB_FAILED            =             WSEC_TEE_ERROR_CODE(304),
    WSEC_TEE_INVALID_SPEC               =             WSEC_TEE_ERROR_CODE(305),
    WSEC_TEE_INVALID_OBJECT_MAGIC       =             WSEC_TEE_ERROR_CODE(306),
    /* asymmetric */
    WSEC_TEE_GET_ATTR_FAILED            =             WSEC_TEE_ERROR_CODE(350),
    WSEC_TEE_SIGN_DIGEST_FAILED         =             WSEC_TEE_ERROR_CODE(351),
    WSEC_TEE_UNSUPPORTED_KEY_SPEC       =             WSEC_TEE_ERROR_CODE(352),
    WSEC_TEE_UNSUPPORTED_ALGO           =             WSEC_TEE_ERROR_CODE(353),
    WSEC_TEE_INVALID_INPUB_ARG          =             WSEC_TEE_ERROR_CODE(354),
    WSEC_TEE_INVALID_OUTPUB_ARG         =             WSEC_TEE_ERROR_CODE(355),
    WSEC_TEE_INVALID_INPRI_ARG          =             WSEC_TEE_ERROR_CODE(356),
    WSEC_TEE_INVALID_OUTPRI_ARG         =             WSEC_TEE_ERROR_CODE(357)
} KmcTaResult;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // KMCTEE_STATUS_H
