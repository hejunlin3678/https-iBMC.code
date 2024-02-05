/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_X509_DEF_H
#define H_CMSCBB_X509_DEF_H
#include "../asn1/cmscbb_asn1_def.h"
#include "cmscbb_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* X509 Extensions */
typedef struct CmscbbX509BasicConstraintsSt {
    CmscbbAsnBool isCa;
} CmscbbX509BasicConstraints;

typedef CmscbbAsnOcts CMSCBB_X509_SUBJECT_KID;

typedef struct CmscbbX509AttrEntrySt {
    CmscbbAsnOid id;
    CmscbbAsnOcts value;
} CmscbbX509AttrEntry;
DECLARE_SET_OF(CmscbbX509AttrEntry, CMSCBB_X509_ATTR_ENTRY_COUNT);

typedef struct CmscbbX509AttrBundleSt {
    SET_OF(CmscbbX509AttrEntry) attrs;
} CmscbbX509AttrBundle;
DECLARE_SET_OF(CmscbbX509AttrBundle, CMSCBB_X509_ATTR_COUNT);

typedef struct CmscbbX509NameSt {
    SET_OF(CmscbbX509AttrBundle) names;
} CmscbbX509Name;

typedef struct CmscbbX509AlgoIdentifierSt {
    CmscbbAsnOid algorithm;
    CmscbbAsnOcts parameters;
} CmscbbX509AlgoIdentifier;
DECLARE_SET_OF(CmscbbX509AlgoIdentifier, 1);

typedef struct CmscbbX509MaskGenAlgoSt {
    CmscbbAsnOid maskOid;
    CmscbbX509AlgoIdentifier maskAlgo;
}CmscbbX509MaskGenAlgo;

typedef struct CmscbbX509PssParamSt {
    CmscbbX509AlgoIdentifier hashAlgorithm;
    CmscbbX509MaskGenAlgo maskGenAlgorithm;
    CmscbbAsnInt saltLength;
    CmscbbAsnInt trailerField;
} CmscbbX509PssParam;

typedef struct CmscbbX509AlgoPssIdentifierSt {
    CmscbbAsnOid algorithm;
    CmscbbAsnOcts rawSigned;
    CmscbbX509PssParam pssPara;
    CmscbbAsnOcts para;
} CmscbbX509AlgoPssIdentifier;

typedef struct CmscbbRsaPubkeySt {
    CmscbbAsnBigint modules;  /* value n */
    CmscbbAsnBigint publicExponent;  /* value e */
} CmscbbRsaPublicKey;

typedef struct CmscbbX509PubkeySt {
    CmscbbRsaPublicKey rsaPubKey;
} CmscbbX509PublicKey;

typedef struct CmscbbX509SubjectPubkeyInfoSt {
    CmscbbX509AlgoPssIdentifier algorithm;
    CmscbbX509PublicKey subjectPublicKey;
    CmscbbAsnBits eccPubKey;
} CmscbbX509PubkeyInfo;

typedef struct CmscbbX509ExtensionEntrySt {
    CmscbbAsnOid extnId;
    CmscbbAsnBool critical;
    CmscbbAsnOcts extnValue;
} CmscbbX509ExtensionEntry;
DECLARE_SET_OF(CmscbbX509ExtensionEntry, 10); /* 10 is capacity of DECLARE_SET_OF */
DECLARE_LIST_OF(CmscbbX509ExtensionEntry);

typedef struct CmscbbX509ValiditySt {
    CmscbbAsnOcts notBefore;
    CmscbbAsnOcts notAfter;
} CmscbbX509Validity;

typedef struct CmscbbX509CertExtsSt {
    CmscbbX509BasicConstraints caInfo;
    CmscbbAsnInt ku;
    CmscbbAsnInt exku;
} CmscbbX509CertExts;

/* X509 CERT */
typedef struct CmscbbX509CertInfoSt {
    CmscbbAsnBigint serialNumber;
    CmscbbX509Name issuer;
#ifdef CVB_DEBUG
    CVB_CHAR* issuerName;
#endif /* CVB_DEBUG */
    CmscbbX509Validity validity;
    CmscbbX509Name subject;
#ifdef CVB_DEBUG
    CVB_CHAR* subjectName;
#endif /* CVB_DEBUG */
    CmscbbX509PubkeyInfo subjectPubKey;

    CmscbbX509CertExts extensions;
} CmscbbX509CertInfo;

/* ECC signature  */
typedef struct CmscbbX509EccSignatureInfoSt {
    CmscbbAsnBigint xValue;
    CmscbbAsnBigint yValue;
} CmscbbEccSign;

typedef struct CmscbbX509CertSt {
    CmscbbAsnOcts rawSigned;
    CmscbbX509CertInfo toBeSigned;
    CmscbbX509AlgoPssIdentifier algorithm;
    CmscbbAsnBits signature;
    /* it should point to its issuer in verify session after the certificate verified */
    const struct CmscbbX509CertSt* issuer;
    const CVB_BYTE *oriCertAddr;
    CVB_UINT32 certLength;
    CVB_INT32 iRef;
} CmscbbX509Cert;
DECLARE_LIST_OF(CmscbbX509Cert);

typedef struct CmscbbX509CertBundleSt {
    LIST_OF(CmscbbX509Cert) certs;
} CmscbbX509CertBundle;

typedef struct CmscbbX509CertRawBundleSt {
    SET_OF(CmscbbAsnOcts) bundle;
} CmscbbX509CertRawBundle;

/* X509 CRL */
typedef struct CmscbbX509RevokeAttrsSt {
    CmscbbAsnInt reason;
} CmscbbX509RevokeAttrs;

typedef struct CmscbX509RevokedEntrySt {
    CmscbbAsnBigint userCert;
    CmscbbAsnOcts revocationDate;
    CmscbbX509RevokeAttrs attrs;
} CmscbbX509RevokeEntry;
DECLARE_SET_OF(CmscbbX509RevokeEntry, 20); /* 20 is capacity of DECLARE_SET_OF */

typedef struct CmscbbX509RevokedSt {
    SET_OF(CmscbbX509RevokeEntry) revokedList;
} CmscbbX509Revoked;

#if CMSCBB_SUPPORT_INDIRECT_CRL
typedef struct CmscbbX509IssuingDistPointSt {
    CmscbbAsnOcts distpoint;
    CmscbbAsnBool onlyuser;
    CmscbbAsnBool onlyCA;
    CmscbbAsnBits onlysomereasons;
    CmscbbAsnBool indirectCRL;
    CmscbbAsnBool onlyattr;
} CmscbbX509IssuingDistPoint;

typedef struct CmscbbX509CrlAttrsSt {
    CmscbbX509IssuingDistPoint idp;
    CmscbbAsnInt crlNumber;
} CmscbbX509CrlAttrs;
#endif

typedef struct CmscbbX509CrlinfoSt {
    CmscbbAsnInt version;
    CmscbbX509Name issuer;
#ifdef CVB_DEBUG
    CVB_CHAR* issuerName;
#endif /* CVB_DEBUG */
    CmscbbAsnOcts thisUpdateTime;
    CmscbbAsnOcts nextUpdateTime;
    CmscbbX509Revoked revokedCerts;
#if CMSCBB_SUPPORT_INDIRECT_CRL
    CmscbbX509CrlAttrs extensions;
#endif
} CmscbbX509CrlInfo;

typedef struct CmscbbX509CrlSt {
    CmscbbAsnOcts rawSigned;
    CmscbbX509CrlInfo tbsCertList;
    CmscbbX509AlgoPssIdentifier algorithm;
    CmscbbAsnBits signature;
    const CVB_BYTE *oriCrlAddr;
    CVB_UINT32 crlLength;
    CVB_INT iRef;
} CmscbbX509Crl;
DECLARE_SET_OF(CmscbbX509Crl, 3); /* 3 is capacity of DECLARE_SET_OF */
DECLARE_LIST_OF(CmscbbX509Crl);

typedef struct CmscbbX509CrlRawBundleSt {
    SET_OF(CmscbbAsnOcts) bundle;
} CmscbbX509CrlRawBundle;

typedef struct CmscbbX509CrlBundleSt {
    LIST_OF(CmscbbX509Crl) crls;
} CmscbbX509CrlBundle;

typedef enum CmscbbX509RevokeReasonEm {
    RVRS_NONE = -1,
    RVRS_UNUSED = 0,
    RVRS_KEYCOMPROMISE = 1,
    RVRS_CACOMPROMISE = 2,
    RVRS_AFFILIATIONCHANGED = 3,
    RVRS_SUPERSEDED = 4,
    RVRS_CESSATIONOFOPERATION = 5,
    RVRS_CERTIFICATEHOLD = 6,
    RVRS_PRIVILEGEWITHDRAWN = 7,
    RVRS_AACOMPROMISE = 8,
} CmscbbX509RevokeReason;

typedef enum CmscbbUpdateListElementEm {
    ELEMENT_CRL = 1,
    ELEMENT_CERT = 2,
} CmscbbUpdateListElement;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !H_CMSCBB_X509_DEF_H */

