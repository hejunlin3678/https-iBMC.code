/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_DEF_H
#define H_CMSCBB_DEF_H
#include "cmscbb_cms_vrf.h"
#include "../asn1/cmscbb_asn1_def.h"

#define CVB_STATIC static

typedef struct CmscbbCertNameInfoSt {
    CVB_CHAR issuerName[MAX_ISSUER_NAME_LENGTH];
    CVB_CHAR subjectName[MAX_ISSUER_NAME_LENGTH];
} CmscbbCertNameInfo;
DECLARE_LIST_OF(CmscbbCertNameInfo);

/*
 * data for verify process
 * NOTE: digestCtx & vrfCtx should not both valid.
 */
typedef struct CmscbbVerifyDigestInfoSt {
#if CMSCBB_SUPPORT_SIGNED_ATTRIBUTE
    CMSCBB_CRYPTO_MD_CTX digestCtx;          /* Hash handler, for verifying cms with signed attribute */
    CmscbbAoids digestAlgID;                 /* Hash algorithm id */
    CVB_BYTE digest[CMSCBB_MAX_DIGEST_SIZE]; /* hash value in cms */
    CVB_UINT32 digestSize;                   /* hash length */
#endif
#if CMSCBB_SUPPORT_NO_SIGNED_ATTR
    CMSCBB_CRYPTO_VRF_CTX vrfCtx;              /* rsa crypt handler, for verifying cms with signed attribute */
    CVB_BYTE signature[CMSCBB_MAX_CRYPT_SIZE]; /* rsa signature value */
    CVB_UINT32 signatureLen;                   /* rsa signature length */
#endif
} CmscbbVerifyDigestInfo;
DECLARE_LIST_OF(CmscbbVerifyDigestInfo);

typedef struct CmscbbVerifyProcessSt {
    LIST_OF(CmscbbVerifyDigestInfo) mdInfoList;
} CmscbbVerifyProcess;

/* data for verify context */
#define RESERV_SIZE_FOR_UPDATE 54
typedef struct CmscbbVrfCtxSt {
    CVB_SIZE_T size;

#if (CMSCBB_SUPPORT_PEM || CMSCBB_CACHE_ASN_DATA)
    CmscbbListDummy rawSet;
#endif
    CVB_BOOL crlFrozen;          /* can add new crl */

    CVB_VOID* pkiCtx;            /* PKI context */
    CmscbbVerifyProcess vrfProc; /* data for verify process */
    CVB_BOOL isHasCrl;
    CVB_TIME_T baseTime;         /* base time, mostly it should be the timestamp from CMS */
    CMSCBB_ERROR_CODE lastErr;   /* the last error */
    CVB_INT currDepth;           /* current depth of certificate tree */
#if CMSCBB_ALLOW_NO_CHECK_TSA_CRL
    CmscbbSerialNum tsa_cert_sn;
#endif
    CVB_INT usageType;
    CVB_UINT8 hashAlgo;
    CVB_UINT8 signAlgo;
    CVB_UINT32 subAlgo; // keyLen or curveId
    CVB_BYTE resv[RESERV_SIZE_FOR_UPDATE]; /* reserve data for structure upgrade */
} CmscbbVrf;

#endif /* H_CMSCBB_DEF_H */

