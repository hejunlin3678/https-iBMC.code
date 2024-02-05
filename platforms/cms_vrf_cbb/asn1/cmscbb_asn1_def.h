/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_ASN1_DEF_H
#define H_CMSCBB_ASN1_DEF_H
#include "cmscbb_config.h"
#include "../asn1/cmscbb_asn1_tag.h"
#include "../cmscbb_common/cmscbb_list_def.h"
#include "cmscbb_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct CmscbbAsnOctsSt {
    CVB_UINT32 len;
    CmscbbBerTag tag;
    const CVB_BYTE* octs;
} CmscbbAsnOcts;
DECLARE_SET_OF(CmscbbAsnOcts, 5); /* 5 is capacity of DECLARE_SET_OF */

typedef struct CmscbbAsnOctsBundleSt {
    SET_OF(CmscbbAsnOcts) bundle;
} CmscbbAsnOctsBundle;

typedef CmscbbAsnOcts CmscbbAsnBits;
typedef CVB_UINT32 CMSCBB_ASN_LEN;
typedef CVB_INT32  CmscbbAsnInt;
typedef CmscbbAsnInt   CmscbbAsnBool;
typedef CVB_INT32  CmscbbAsnEnum;
typedef CVB_CHAR   CMSCBB_ASN_NULL;
typedef CmscbbAsnOcts CmscbbAsnOid;
typedef CmscbbAsnOcts CMSCBB_ASN_RELATIVE_OID;
typedef CmscbbAsnOcts CmscbbAsnBigint;

typedef CmscbbAsnOcts CMSCBB_VISIBLE_STR;
typedef CmscbbAsnOcts CMSCBB_TELETEX_STR;
typedef CmscbbAsnOcts CMSCBB_UTF8_STR;
typedef CmscbbAsnOcts CMSCBB_UNIVERSAL_STR;
typedef CmscbbAsnOcts CMSCBB_PRINTABLE_STR;
typedef CmscbbAsnOcts CMSCBB_NUMERIC_STR;
typedef CmscbbAsnOcts CMSCBB_IA5_STR;
typedef CmscbbAsnOcts CMSCBB_BMP_STR;
typedef CmscbbAsnOcts CMSCBB_UTCTIME;
typedef CmscbbAsnOcts CMSCBB_GENERALIZEDTIME;

DECLARE_SET_OF(CmscbbAsnOid, 10); /* 10 is capacity of DECLARE_SET_OF */

#define CVB_ASN_INDEFINITE_LEN ((CVB_UINT32)~0L)

/* common ASN object id */
typedef enum CmscbbAoidsEm {
    AOID_UNKNOWN = 0,
    AOID_SHA256 = 43,
#if CMSCBB_SUPPORT_SHA384
    AOID_SHA384 = 44,
#endif
#if CMSCBB_SUPPORT_SHA512
    AOID_SHA512 = 45,
#endif
    AOID_SM3 = 46,
    AOID_RSA = 53,
    AOID_RSASSAPSS = 62,
    AOID_SHA256WITHRSAENCRYPTION = 63,
#if CMSCBB_SUPPORT_SHA384
    AOID_SHA384WITHRSAENCRYPTION = 64,
#endif
#if CMSCBB_SUPPORT_SHA512
    AOID_SHA512WITHRSAENCRYPTION = 65,
#endif
    AOID_AT_COMMONNAME = 161,
    AOID_AT_LOCALITYNAME = 162,
    AOID_AT_STATEORPROVINCENAME = 163,
    AOID_AT_ORGANIZATIONNAME = 164,
    AOID_AT_ORGANIZATIONALUNITNAME = 165,
    AOID_AT_COUNTRYNAME = 168,
    AOID_PKCS9_AT_EMAILADDRESS = 172,
    AOID_CE_KEYUSAGE = 176,
    AOID_CE_BASICCONSTRAINTS = 184,
    AOID_CE_EXTKEYUSAGE = 188,
    AOID_KP_CODESIGNING = 192,
    AOID_KP_TIMESTAMPING = 193,
#if CMSCBB_SUPPORT_INDIRECT_CRL
    AOID_CE_ISSUINGDISTRIBUTIONPOINT = 202,
    AOID_CE_CRLNUMBER = 203,
#endif
    AOID_CE_CRLREASONS = 204,
    AOID_SM2_SIGNEDDATA = 263,
    AOID_PKCS7_SIGNEDDATA = 264,
    AOID_PKCS7_TSTINFO = 265,
    AOID_CT_ANIMAJSONVOUCHER = 266,
    AOID_PKCS9_AT_MESSAGEDIGEST = 271,
    AODI_PKCS9_AT_CONTENTTYPE = 272,
    AOID_PKCS9_AT_TIMESTAMP_TOKEN = 307,
    AOID_HW_KP_VOUCHERSIGNING = 372,
    AOID_HW_KP_RELEASESIGNING = 373,
    AOID_ECDSA = 400,
    AOID_NIST_CURVE_P256 = 407,
    AOID_BRAINPOOL_CURVE_P256 = 408,
    AOID_SHA256WITHECDSA = 412,
#if CMSCBB_SUPPORT_SHA384
    AOID_SHA384WITHECDSA = 413,
#endif
#if CMSCBB_SUPPORT_SHA512
    AOID_SHA512WITHECDSA = 414,
#endif
#if CMSCBB_SUPPORT_SHA384
    AOID_NIST_CURVE_P384 = 420,
    AOID_BRAINPOOL_CURVE_P384 = 422,
#endif
#if CMSCBB_SUPPORT_SHA512
    AOID_NIST_CURVE_P512 = 421,
    AOID_BRAINPOOL_CURVE_P512 = 423,
#endif
    AOID_SM2 = 500,
    AOID_CURVE_SM2 = 503,
    AOID_SM2WITHSM3 = 505,
} CmscbbAoids;

#define CMSCBB_ASN_XKU_CODE_SIGN      0x08   /* same as CMSCBB_PKI_XKU_CODE_SIGN */
#define CMSCBB_ASN_XKU_TIMESTAMP      0x40   /* same as CMSCBB_PKI_XKU_TIMESTAMP */
#define CMSCBB_ASN_XKU_VOUCHERSIGNING 0x200  /* same as CMSCBB_ASN_XKU_VOUCHERSIGNING */
#define CMSCBB_ASN_XKU_RELEASESIGNING 0x400  /* same as CMSCBB_ASN_XKU_RELEASESIGNING */

#define CMSCBB_OID_CODE_SIGN      "1.3.6.1.5.5.7.3.3"
#define CMSCBB_OID_TIMESTAMP      "1.3.6.1.5.5.7.3.8"
#define CMSCBB_OID_VOUCHERSIGNING "1.3.6.1.4.1.2011.201.1.3.2"
#define CMSCBB_OID_RELEASESIGNING "1.3.6.1.4.1.2011.201.1.3.1"

typedef struct CmscbbOidInfoSt {
    CmscbbAoids aoid;
    CmscbbAsnOcts asnOid;
    const CVB_CHAR* name;
} CmscbbOidInfo;

typedef struct CmscbbAsnOidBundleSt {
    SET_OF(CmscbbAsnOid) oidBundle;
} CmscbbAsnOidBundle;

CmscbbOidInfo* GetCvbOIDTable(void);

CVB_UINT32 GetCvbOidCount(void);

#if CMSCBB_NEED_RELOCATE
CVB_BOOL GetCvbOidReloced(void);
CVB_VOID SetCvbOidReloced(CVB_BOOL isReloced);
#endif

#define CMSCBB_BUF_INVALID_POS CMSCBB_MAX_INT

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* H_CMSCBB_ASN1_DEF_H */
