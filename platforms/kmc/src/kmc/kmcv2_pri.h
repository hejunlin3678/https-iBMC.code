/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2014-2020. All rights reserved.
 * Description: KMC internal interface header file, which is not open to external systems.
 * Author: x00102361
 * Create: 2014-06-16
 * History: 2018-10-08 Zhang Jie (employee ID: 00316590) Rectification by UK
 * 2019-04-09 Zhang Jie (employee ID: 00316590): Added some interfaces to adapt to KMC 3.0.
 */

#ifndef KMC_SRC_KMC_KMCV2_PRI_H
#define KMC_SRC_KMC_KMCV2_PRI_H

#include "kmcv2_itf.h"
#include "wsecv2_type.h"
#include "wsecv2_array.h"
#include "wsecv2_share.h"
#include "wsecv2_lock.h"
#include "wsecv2_ctx.h"
#include "kmctee_msg.h"
#include "kmc_ext_ksf.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define KMC_KCF_VER 1 /* KMC configuration file version */

#define KMC_RMK_LEN         32 /* RMK key length */
#define KMC_SHORT_RMK_LEN   16 /* RMK key length for SM4/AES128 */
#define KMC_EK4MKF_LEN      32 /* MKF (MK file) encryption key length */
#define KMC_KEY4HMAC_LEN    32 /* Key length for HMAC */
#define KMC_HMAC_SHA256_LEN 32 /* Length of the SHA256 calculation result */
#define KMC_HASH_SHA256_LEN 32 /* HMACSHA256 calculation result length */
#define WSEC_HASH_LEN_MAX   64 /* Maximum HMAC result length */
#define WSEC_HMAC_LEN_MAX   64 /* Maximum HMAC result length */
#define KMC_SINGLE_DOAMIN_COUNT 1 /* domain count is 1 */

/* valid domainId value range */
#define KMC_DOMAINID_OUT_OF_RANGE(domainId) \
    (((domainId) != KMC_ALL_DOMAIN) && ((domainId) >= KMC_PRI_DOMAIN_ID_MIN))

#define KMC_KEYTYPE_MAX_LIFEDAYSEX (1024 * 1024) /* Maximum validity period of the key, preventing key reversal. */
#define KMC_FLAG_LENGTH 32 /* Length of the KMC file header identifier. The length is 32 bytes. */
#define KMC_MATERIAL_SIZE 32
#define MATERIAL_COUNT 2
#define KMC_DOMAIN_OVERFLOWKEYID  (WsecUint32)(-1)
#define KMC_KSF_CACHE_COUNT 4
#define KMC_MASKED_KEY_LEN 128

#define KMC_KSF_NUM 2
#define KMC_KSF_WITH_THIRD_NUM  (KMC_KSF_NUM + 1)  /* KSF NUM After Enable Third backup */

#define MASTER_KSF_INDEX 0
#define BACKUP_KSF_INDEX (MASTER_KSF_INDEX + 1)
#define THIRD_KSF_INDEX (MASTER_KSF_INDEX + 2)

#define KMC_EXT_KSF_NUM 2
#define MASTER_EXT_KSF_INDEX 0
#define BACKUP_EXT_KSF_INDEX (MASTER_EXT_KSF_INDEX + 1)

#define KMC_ENCRYPT_MK_ALGID  WSEC_ALGID_AES256_CBC  /* MK Encryption Algorithm ID */
#define KMC_HMAC_MK_ALGID     WSEC_ALGID_HMAC_SHA256 /* HMAC protection MK data algorithm ID */

#define DEFAULT_KEY_LIFE_DAYS 180 /* Default MK validity period (days) */
#define DEFAULT_KEY_LEN 32 /* Default key length */
#define DEFAULT_ROOT_KEY_VALIDITY 3650 /* The default validity period of an RK is 10 years. */
#define DEFAULT_ROOT_KEY_RMK_ITERATION 10000 /* Number of iterations for root key derivation */

#define KMC_HAS_KSF_EXT 0x01
#define KMC_KSF_HASH_CTX_NUM 1      /* only calculate ksf hash ctx */
#define KMC_EXTKSF_HASH_CTX_NUM 2   /* calculate ksf hash ctx and extKsf hash ctx */

/* KMC lock protection type */
typedef enum {
    KMC_LOCK_NONE     = 0, /* No lock 0000 */
    KMC_LOCK_CFG      = 1, /* Lock configuration 0001 */
    KMC_LOCK_KEYSTORE_WRITE = 2, /* Locking the Keystore write 0010 */
    KMC_LOCK_KEYSTORE_WRITE_CFG     = 3, /* Lock configuration and Keystore 0011 */
    KMC_LOCK_KEYSTORE_READ = 4, /* Locking the Keystore read 0100 */
    KMC_LOCK_KEYSTORE_READ_CFG = 5, /* Locking the Keystore read and cfg 0101 */
    KMC_LOCK_KEYSTORE = 6, /* Locking the Keystore read and write 0110 */
    KMC_LOCK_KEYSTORE_CFG = 7 /* Locking the Keystore read, write and cfg 0111 */
} KmcLockType;

typedef enum {
    WSEC_WAIT_INIT = 0, /* Uninitialized */
    WSEC_INIT_FAIL,     /* Initialization failed. */
    WSEC_RUNNING,       /* Normal running */
    WSEC_ON_INIT        /* Initializing */
} WsecRunState; /* Status of the CBB */

typedef enum {
    KMC_MIN_KEYID = 1,
    KMC_MAX_KEYID
} KmcKeyIdEndType;

typedef enum {
    KMC_COMPARE_EQ = 0,
    KMC_COMPARE_NOT_EQ,
    KMC_COMPARE_ALL
} KmcCompareResult;

typedef enum TagGetMkDetailType {
    GET_EXPORT_MK_EXTERNAL,
    GET_EXPORT_MK_INTERNAL
} GetMkDetailType;

/* Master Key (MK) data structure */
#pragma pack(1)
typedef struct TagKmcMkRear {
    WsecUint32    plaintextLen; /* MK plaintext length */
    /*
     * Key (When the file is stored, the key is the ciphertext encrypted by the RMK.
     * When the file is stored in the memory, the key is the plaintext decrypted by the RMK.)
     */
    unsigned char key[WSEC_MKTEE_LEN_MAX];
} KmcMkRear; /* MK trailer information */


typedef struct TagKmcMkRearKsf {
    WsecUint32    plaintextLen; /* MK plaintext length */
    /*
     * Key (When the file is stored, the key is the ciphertext encrypted by the RMK.
     * When the file is stored in the memory, the key is the plaintext decrypted by the RMK.)
     */
    unsigned char key[WSEC_MK_LEN_MAX];
} KmcMkRearKsf; /* MK trailer information */
#pragma pack()

#pragma pack(1)
typedef struct TagKmcKsfMk {
    KmcMkInfo     mkInfo;        /* Basic MK information */
    WsecUint32    cipherAlgId;   /* Encryption Method */
    unsigned char reserve[40];   /* 40 bytes are reserved. */
    /* The MK is encrypted using the RMK to store the IV.The length of the IV is 16 bytes. */
    unsigned char iv[16];
    WsecUint32    ciphertextLen; /* Length of the ciphertext of the MK */
    union {
        struct { /* V2 Member */
            KmcMkRearKsf     mkRear;        /* MK tail information */
            unsigned char mkHash[32];    /* HMAC-SHA256, 32 bytes */
        } cipherMkV2;
        struct { /* V3 Member */
            WsecUint32    plaintextLen;    /* MK plaintext length */
            /*
             * In V3, only the first 160 bytes are stored. The remaining bytes are followed by ciphertextLen.
             * In V3, if the hardware directly encrypts the MK,
             * the ciphertext must contain both the ciphertext and MAC address.
             * In V3, if the MK is encrypted using the software-layer root key,
             * AES-GCM is used. The ciphertext contains the tag.
             */
            unsigned char key[160]; /* In V3, 160 bytes of the original 128-byte key and 32-byte HMAC are used */
                                    /* as the first 160 bytes of the key ciphertext. */
        } cipherMkV3;
        struct { /* V4 */
            unsigned char teeTag[16];
            unsigned char teeKey[140];
            unsigned char teeHash[8];
        } cipherMkV4;
    } cipherMk;
} KmcKsfMk; /* MK information stored in the KSF (not stored in the memory) */
#pragma pack()

#pragma pack(1)
typedef struct TagKmcKsfHmac {
    union {
        unsigned char      hashData[WSEC_HASH_LEN_MAX];
        unsigned long long partHash;
    } hashData;
    union {
        unsigned char      hashHmac[WSEC_HMAC_LEN_MAX];
        unsigned long long partHmac;
    } hmacData;
} KmcKsfHmac;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcMemMk {
    KmcMkInfo     mkInfo; /* Basic MK information */
    KmcMkRear     mkRear; /* MK tail information */
    /* Unique identifier of a key, which is the first eight bytes of the SHA256 value. */
    unsigned char hashData[WSEC_MK_HASH_REC_LEN];
} KmcMemMk; /* MK information stored in the memory or written to the MKF after encryption */
#pragma pack()

#pragma pack(1)
typedef struct TagKmcSyncMkHeader {
    WsecUint16 msgType;  /* KmcSyncMsgType */
    WsecUint16 version;
    WsecUint32 msgLen;
    WsecUint32 sharedMkUpdateCounter;
} KmcSyncMkHeader;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcSyncMk {
    KmcMkInfo mkInfo; /* Basic MK information */
    KmcMkRear mkRear; /* MK tail information */
} KmcSyncMk;
#pragma pack()

#define WSEC_MK_CIPHER_LEN 512

#pragma pack(1)
typedef struct TagKmcSyncEncInfoHeader {
    WsecUint32 domainCount;
    WsecUint32 keyCount;
    WsecUint32 rmvDomainCount;
    unsigned char salt[32];
    WsecUint32 saltLen;
    WsecUint32 iter;
    unsigned char reserve[32];       /* 32 bytes are reserved. */
} KmcSyncEncInfoHeader;

#pragma pack()

typedef struct TagKmcTeeRkCreate {
    WsecUint8 *enclaveRk;
    WsecBool realCreate;
} KmcTeeRkCreate;

/* Type of the key to be obtained. */
typedef struct TagKmcKeyTypesInfo {
    int        typeCount; /* The value ranges from 1 to 7. */
    KmcKeyType keyTypes[KMC_KEY_TYPE_MAX - 1]; /* For details about the eight key types, see KmcKeyType. */
} KmcKeyTypesInfo;

/* Active Key's domain and key type info */
typedef struct TagKmcActiveKeyParam {
    WsecUint32 domainId;
    KmcKeyTypesInfo keyTypes;
} KmcActiveKeyParam;

/* KMC configuration data structure (memory) */
typedef struct TagKmcDomainCfg {
    KmcCfgDomainInfo domainInfo;
    WsecArray        keyTypeCfgArray; /* KeyType array, element KmcCfgKeyType type */
} KmcDomainCfg;

/*
 * MK file (MKF for short) data structure (for key export/import)
 * Format:
 * 0. 32-byte format code
 *     1. KmcMkfHeaderWithHmac
 * 2. N MK data ciphertexts (KmcMkFileOneMk)
 * 3. HMAC of all MK ciphertexts
 */
#pragma pack(1)
typedef struct TagKmcMkfHeader {
    WsecUint16    version;           /* MK file version */
    WsecUint16    ksfVersion;        /* Keystore file version */
    WsecUint32    cipherAlgId;       /* Encryption algorithm ID */
    WsecUint32    iterForEncKey;     /* Number of iterations during encryption key generation */
    unsigned char saltForEncKey[16]; /* Salt used for generating the encryption key. The value is fixed to 16 bytes. */
    unsigned char ivForEncMk[16];    /* IV used for encrypting the MK. The value is fixed to 16 bytes. */
    unsigned char reserveA[16];      /* 16 bytes are reserved. */

    WsecUint32    hmacAlgId;          /* HMAC algorithm ID */
    WsecUint32    iterForHmacKey;     /* Number of iterations during HMAC key generation */
    unsigned char saltForHmacKey[16]; /* Salt used for generating the HMAC key. The value is fixed to 16 bytes. */
    WsecUint32    cipherLenPerMk;     /* Length of a single MK ciphertext */
    WsecUint32    mkNum;              /* Number of MKs */
    unsigned char reserveB[16];       /* 16 bytes are reserved. */
} KmcMkfHeader; /* MK file header */
#pragma pack()

#pragma pack(1)
typedef struct TagKmcMkfHeaderWithHmac {
    KmcMkfHeader  mkfHeader;
    unsigned char hmacData[KMC_HMAC_SHA256_LEN]; /* HMAC of the preceding data */
} KmcMkfHeaderWithHmac;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcMkFileOneMk {
    KmcMkInfo     mkInfo;                     /* Basic MK information */
    /* The MK uses the RMK to encrypt the IV and has a fixed length of 16 bytes. */
    unsigned char iv[16];
    WsecUint32    plaintextLen;               /* Plaintext key length */
    unsigned char plainText[WSEC_MK_LEN_MAX]; /* Plaintext key */
} KmcMkFileOneMk; /* MK records in the MK file (encrypted storage) */
#pragma pack()

#pragma pack(1)
typedef struct KmcMakeMemMkParamTag {
    const KmcKsfHardRk *hardRk;
    WsecVoid *mkRead;
    WsecUint32 mkReadLen;
    WsecUint16 ksfVersion;
    WsecBuff rmkBuff;
    WsecUint32 hmacAlgId;
} KmcMakeMemMkParam;
#pragma pack()

#pragma pack(1)
typedef struct TagKmcCnfId {
    WsecUint32 hashAlg;
    WsecUint32 hmacAlg;
    WsecUint32 symmAlgId;
    WsecUint32 workKeyIter;
    WsecUint32 kdfAlg;
    WsecUint32 saltLen;
} KmcCnfId;
#pragma pack()

#define KMC_MASKCODE_LENGTH 128
#define KMC_MASKCODE_KEY_LENGTH (2 * KMC_MASKCODE_LENGTH)

#pragma pack(1)
typedef struct TagKmcSyncKeyInfo {
    unsigned char keyBuff[KMC_DH_TMP_KEY_LEN];
    WsecUint32 keyLen;
} KmcSyncKeyInfo;
#pragma pack()

typedef enum {
    WSEC_KMC_SYMM_ENC_DEC_CLASS,     // KMC_KEY_TYPE_ENCRPT、KMC_KEY_TYPE_ENCRPT_INTEGRITY
    WSEC_KMC_ASYM_ENC_DEC_CLASS,     // KMC_KEY_TYPE_ASYM_ENCRYPT_DECRYPT
    WSEC_KMC_ASYM_SIGN_VERIFY_CLASS, // KMC_KEY_TYPE_ASYM_SIGN_VERIFY
} WsecKeyTypeClass;

/*
 * Obtain the MK based on the domain ID and key ID. If the MK cannot be obtained,
 * obtain it based on the hash. Ensure that the hash values are consistent.
 */
unsigned long KmcGetMkByIDHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId,
    WsecUint32 keyId,
    const unsigned char *hashData, WsecUint32 hashLen,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen);
unsigned long KmcGetMkDetailInternal(KmcCbbCtx *kmcCtx, WsecUint32 domainId, WsecUint32 keyId, KmcMkInfo *mkInfo,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen);

/* Obtains the currently effective key, key ID, and hash value in a specified domain. */
unsigned long KmcGetActiveMkWithHash(KmcCbbCtx *kmcCtx, WsecUint32 domainId,
    unsigned char *keyBuff, WsecUint32 *keyBuffLen,
    WsecUint32 *keyId, unsigned char *keyHash, size_t hashLen, KmcCnfId *ids);

/*
 * This interface is used to obtain a valid key in a specified domain.
 * If multiple keys in a domain are valid, any of them can be obtained.
 * This function also returns the status information of the key.
 * The creation time and expiration time in the information is obtained based on the local time function,
 * The invoker can determine whether the certificate is trusted based on the site requirements.
 * The default expiration time is 180 days. If there are multiple certificates in the keystore,
 * This function is used to obtain a random key from the keys with the same domain but different IDs but in valid state.
 */
unsigned long KmcPriGetActiveMk(KmcCbbCtx *kmcCtx, WsecUint32 domainId, KmcKeyTypesInfo keyTypes, KmcMkInfo *mkInfo,
    unsigned char *keyPlaintextBuff, WsecUint32 *keyBuffLen, KmcCnfId *ids);

/* Check whether the KSF version number is valid. */
WsecBool IsValidKsfVersion(WsecUint16 ksfVersion);

/* KSF V3 or Not */
WsecBool IsKsfV3(WsecUint16 ksfVersion);

/* KSF V1 or Not */
WsecBool IsKsfV1(WsecUint16 ksfVersion);
WsecBool IsKsfV4(WsecUint16 ksfVersion);


/* KSF V2 or Not */
WsecBool IsKsfV2(WsecUint16 ksfVersion);

/* Whether KSF V1 is used can be determined only based on KSF V1 and KSF V2. */
WsecBool IsKsfV1AndNotV2(WsecUint16 ksfVersion);

/* KSF V1 or V2 */
WsecBool IsKsfV1OrV2(WsecUint16 ksfVersion);

/* KSF V1 or V2 or V3 */
WsecBool IsKsfV1OrV2orV3(WsecUint16 ksfVersion);

WsecBool IsKsfV1OrV2OrV4(WsecUint16 ksfVersion);
WsecBool IsKsfV2OrV3OrV4(WsecUint16 ksfVersion);
/* KSF V2 or V3 */
WsecBool IsKsfV2OrV3(WsecUint16 ksfVersion);

/* whether support sync mk by interface */
WsecBool IsSupportSyncMk(WsecUint16 ksfVersion);

/*
 * Calculated in network sequence (RK hash and MK HMAC V1 are calculated in host sequence,
 * and V2/V3 are calculated in network sequence.)
 */
WsecBool CalcByNetWorkOrder(WsecUint16 ksfVersion);

/* Initializes the PRI module.The PRI module manages the KmcSys structure and provides global information for the KMC */
unsigned long PriKmcSysInit(KmcCbbCtx *kmcCtx, WsecUint32 state, const WsecInternalInitParam *initParam);

/* Obtain the KSF name based on the KSF index. */
char *PriKmcSysGetKsf(KmcCbbCtx *kmcCtx, WsecUint32 idx);

/* Get the third ksf backup file name */
char *PriKmcSysGetKsfBackupKsf(KmcCbbCtx *kmcCtx);

/* Enable the third ksf backup file */
WsecVoid PriKmcSysSetEnableThirdBackup(KmcCbbCtx *kmcCtx, WsecBool isEnable);

/* Whether enabled the third ksf backup file */
WsecBool PriKmcSysGetIsEnableThirdBackup(KmcCbbCtx *kmcCtx);

/* delete ksf backup file when init failed */
WsecBool PriKmcSysGetIsDeleteKsfOnInitFailed(KmcCbbCtx *kmcCtx);

/* set delete ksf on startup feature */
WsecVoid PriKmcSysSetDeleteKsfOnInitFailed(KmcCbbCtx *kmcCtx, WsecBool isEnable);

WsecVoid PriKmcSysSetKeyStoreMode(KmcCbbCtx *kmcCtx, unsigned char keyStoreMode);
unsigned char PriKmcSysGetKeyStoreMode(KmcCbbCtx *kmcCtx);

/* Obtains whether hardware protection is enabled (corresponding to the V3 KSF). */
WsecBool PriKmcSysGetIsHardware(KmcCbbCtx *kmcCtx);

/* Sets the current KMC status. */
WsecVoid PriKmcSysSetState(KmcCbbCtx *kmcCtx, WsecUint32 state);

/* Obtaining the Current KMC Status */
WsecUint32 PriKmcSysGetState(KmcCbbCtx *kmcCtx);

/* Set the current KMC role. */
WsecVoid PriKmcSysSetRole(KmcCbbCtx *kmcCtx, WsecUint32 role);

WsecBool PriKmcSysGetIsEnclave(KmcCbbCtx *kmcCtx);
WsecVoid PriKmcSysSetEnclave(KmcCbbCtx *kmcCtx, WsecBool isEnable);

WsecBool PriKmcSysGetHaveHw(KmcCbbCtx *kmcCtx);
WsecVoid PriKmcSysSetHaveHw(KmcCbbCtx *kmcCtx, WsecBool isHaveHw);

WsecVoid PriKmcSysSetTeeInitParam(KmcCbbCtx *kmcCtx, const KmcTeeInitParam *initParam);

WsecVoid PriKmcSysSetUserDefineSrk(KmcCbbCtx *kmcCtx, unsigned char userDefineSrk);

unsigned char PriKmcSysGetUserDefineSrk(KmcCbbCtx *kmcCtx);

/* Obtaining the Current KMC Role */
WsecUint32 PriKmcSysGetRole(KmcCbbCtx *kmcCtx);

/* Whether the root key of the software layer exists, which is determined by the initialization parameter */
WsecBool PriKmcSysGetHasSoftLevelRk(KmcCbbCtx *kmcCtx);

/* Deinitializes the PRI module. */
WsecVoid PriKmcSysUninit(KmcCbbCtx *kmcCtx);

/* init keystore files */
WsecUint32 PriKmcSysInitKsfName(KmcCbbCtx *kmcCtx, const KmcKsfName *ksf);

#if WSEC_COMPILE_ENABLE_ASYM
/* init ext keystore files name */
WsecUint32 PriKmcSysInitExtKsfName(KmcCbbCtx *kmcCtx, const KmcKsfName *extKsf);
#endif // WSEC_COMPILE_ENABLE_ASYM

/* The byte order of the MK information is converted. */
WsecVoid CvtByteOrderForMkInfo(KmcMkInfo *mkInfo, WsecUint32 direction);

/* MK Synchronization Byte Sequence Conversion */
WsecVoid CvtByteOrderForSyncMk(KmcSyncMk *mk, WsecUint32 direction);

/* EncMK Synchronization Byte Sequence Conversion */
WsecVoid CvtByteOrderForSyncEncMk(KmcSyncEncKey *mk, WsecUint32 direction);

/* MK Synchronization Header Byte Sequence Conversion */
WsecVoid WsecCvtByteOrderForSyncMkHeader(KmcSyncMkHeader *header, WsecUint32 direction);
WsecVoid WsecCvtByteOrderForSyncDhPub(KmcSyncKeyInfo *header, WsecUint32 direction);
WsecVoid WsecCvtByteOrderForDhSupport(WsecUint32 *header, WsecUint32 direction);
/* Converts the byte order of the header information in the MKF. */
WsecVoid CvtByteOrderForMkfHdr(KmcMkfHeader *mkfHeader, WsecUint32 direction);

/* The MK information word in the MKF is converted into the stanza. */
WsecVoid CvtByteOrderForMkfMk(KmcMkFileOneMk *mkfMk, WsecUint32 direction);

WsecVoid CvtByteOrderForNovaMkInfo(KmcNovaKeyInfo *mkInfo, WsecUint32 direction);

/* compare the mk domainid with the input domainid */
KmcCompareResult KmcCompareBatchDomain(WsecUint32 compareDomain, const KmcDomainArray *domainArray);

/* Set the key time. */
unsigned long SetLifeTime(KmcCbbCtx *kmcCtx, WsecUint32 lifeDays, WsecSysTime *createUtc, WsecSysTime *expireUtc);

unsigned long TransStrToUint(KmcCbbCtx *kmcCtx, const unsigned char *str, unsigned long strlen, WsecUint32 *num);

unsigned long TransUintToStr(KmcCbbCtx *kmcCtx, WsecUint32 num, unsigned char *str, unsigned long strlen);

WsecBool CheckTeeWithUserDefineSrk(KmcCbbCtx *kmcCtx);

WsecVoid CvtByteOrderForMkParamHeader(KmcSyncEncInfoHeader *mkHeader, WsecUint32 direction);

#if WSEC_COMPILE_ENABLE_ASYM
KmcAsymFuns *GetAsymFuns(void);
void ResetAsymFuns(void);
#endif // WSEC_COMPILE_ENABLE_ASYM

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* KMC_SRC_KMC_KMCV2_PRI_H */
