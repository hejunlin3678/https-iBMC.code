/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_CMS_DEF_H
#define H_CMSCBB_CMS_DEF_H
#include "cmscbb_x509_def.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TS INFO */
typedef struct CmscbbTsMsgImprintSt {
    CmscbbX509AlgoIdentifier hashAlgo;
    CmscbbAsnOcts hashValue;
} CmscbbTsMsgImprint;


typedef struct CmscbbTstInfoSt {
    CmscbbTsMsgImprint msgImprint;
    CmscbbAsnOcts time;
} CmscbbTimestampInfo;

typedef struct CmscbbPkcs7ContentInfoSt {
    CmscbbAsnOid type;
    CmscbbAsnOcts content;
} CmscbbPkcs7ContentInfo;

typedef struct CmscbbPkcs7IssuerAndSerialSt {
    CmscbbX509Name issuer;
    CmscbbAsnBigint sn;
} CmscbbPkcs7IssuerAndSerial;

#if CMSCBB_SUPPORT_SIGNED_ATTRIBUTE
typedef struct CmscbbX509SignedAttrsSt {
    CmscbbAsnOcts contentType;
    CmscbbAsnOcts hashValue;
} CmscbbX509SignedAttrs;
#endif

typedef struct CmscbbX509UnsignedAttrsSt {
    CmscbbAsnOcts timestamp;
} CmscbbX509UnsignedAttrs;

typedef struct CmscbbPkcs7SignerInfoSt {
    CmscbbAsnInt version;
    CmscbbPkcs7IssuerAndSerial issuerSn;
#ifdef CVB_DEBUG
    CVB_CHAR* issuerName;
#endif /* CVB_DEBUG */
    CmscbbX509AlgoIdentifier digestAlgo;
#if CMSCBB_SUPPORT_SIGNED_ATTRIBUTE
    CmscbbAsnOcts rawSigned;
    CmscbbX509SignedAttrs authAttrs;
#endif
    CmscbbX509AlgoPssIdentifier signatureAlgo;
    CmscbbAsnOcts encryptedDigest;
    CmscbbX509UnsignedAttrs unauthAttrs;
} CmscbbPkcs7SignedInfo;
DECLARE_SET_OF(CmscbbPkcs7SignedInfo, 1);

typedef struct CmscbbPkcs7SignerInfoBundleSt {
    SET_OF(CmscbbPkcs7SignedInfo) infos;
} CmscbbPkcs7SignedInfoBundle;

typedef struct CmscbbPkcs7DigestAlgoBundleSt {
    SET_OF(CmscbbX509AlgoIdentifier) digests;
} CmscbbPkcs7DigestAlgoBundle;

typedef struct CmscbbPkcs7SignedDataSt {
    CmscbbAsnInt version;
    CmscbbPkcs7DigestAlgoBundle digestAlgos;
    CmscbbPkcs7ContentInfo content;
    CmscbbX509CertRawBundle rawCerts;
    CmscbbX509CertBundle certificates;
    CmscbbX509CrlRawBundle rawCrls;
    CmscbbX509CrlBundle crls;
    CmscbbPkcs7SignedInfoBundle signerInfos;
} CmscbbPkcs7SignedData;

typedef struct CmscbbContentinfoSt {
    CmscbbAsnOid contentType;
    CmscbbPkcs7SignedData signedData;
} CmscbbPkcs7Content;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* H_CMSCBB_CMS_DEF_H */
