/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */

/*
 * File Name          : cmscbb_cms_vrf.h
 * Author             : tangzonglei
 * Creation Date      : 2015/05/28 11:20
 * Detail Description : cmscbb  Interface Definition header File
 * History            : tangzonglei   2015/05/28 11:20     new
 */
#ifndef H_CMSCBB_CMS_VRF_H
#define H_CMSCBB_CMS_VRF_H
#include "cmscbb_err_def.h"
#include "cmscbb_plt_def.h"
#include "cmscbb_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef CVB_VOID* CMSCBB_VRF_CTX;

#define MAX_ISSUER_NAME_LENGTH 256

#if CMSCBB_SUPPORT_CRL_COMPARE || CMSCBB_ALLOW_NO_CHECK_TSA_CRL
#define CMSCBB_MAX_SN_LEN 32
/* define serials number type */
typedef struct CmscbbSerialNumSt {
    CVB_BYTE sn[CMSCBB_MAX_SN_LEN];
    CVB_UINT32 snLenth;
} CmscbbSerialNum;
#endif
#if CMSCBB_SUPPORT_CRL_COMPARE

/* list of serials number of revoked certs */
typedef struct CmscbbRevokeListSt {
    CmscbbSerialNum* snList;
    CVB_UINT32 revokeCount;
} CmscbbRevokeList;

/* Keep information for CRL parse result */
#define MAX_CRL_SUPPORT 16
#define MAX_CERT_SUPPORT 16
#define MAX_REVOKE_LIST_NUM 5
typedef struct CmscbbCrlInfoSt {
#if CMSCBB_STRICT_CHECK
    CVB_SIZE_T st_size;
#endif
    int valid; /* is valid */
#if CMSCBB_BYTE_ALIGN
    CVB_BYTE resv0[4]; /* if algin, needs reserved text, 4 bytes reserved */
#endif
    CVB_INT64 updateTime;                    /* Date of issue, number of seconds from 1970-1-1 0 */
    CVB_INT64 validUntil;                    /* Next issue date, number of seconds from 1970-1-1 0. */
    CVB_CHAR issuer[MAX_ISSUER_NAME_LENGTH]; /* Publishing person Information */
    CmscbbSerialNum issuerSn;                /* Issue Certificate serial number */
    CmscbbRevokeList revokeList;             /* Root certificate Revocation List */
    CVB_CHAR resv[60];                       /* 60 units reserved text in crl */
} CmscbbCrlInfo;

/* CRL result bundle/set */
typedef struct CmscbbCrlBundleInfoSt {
    CVB_SIZE_T st_size;
    CmscbbCrlInfo* crlInfoList[MAX_CRL_SUPPORT]; /* CRL Information list, maximum can save 16 */
    CVB_INT crlCount;                            /* Number of true information of CRLs */
    CVB_CHAR resv[64];                           /* 64 units reserved text in futures */
} CmscbbCrlBundleInfo;

/* CRL bundle/set issue time compare options */
typedef enum CmscbbCrlPeriodStat {
    SCPS_SAME = 0,
    SCPS_NEW = 1,
    SCPS_OLD = 2,
    SCPS_MIX = 3,
} CmscbbCrlPeriodStat;
typedef struct CmscbbCrlAddressInfoSt {
    CVB_CHAR type;  // crl or cert
    const CVB_BYTE *crlAddr;
    CVB_UINT32 length;
    CVB_INT64 updateTime;
    CVB_INT32 crlNumber;
    CmscbbSerialNum serialNum;
    CVB_CHAR name[MAX_ISSUER_NAME_LENGTH];
} CmscbbCrlAddressInfo;

typedef struct CmscbbCrlToUpdateListSt {
    CVB_INT totalCount; /* crlCount + certCount */
    CVB_INT certCount;
    CmscbbCrlAddressInfo* addressInfoList[MAX_CRL_SUPPORT + MAX_CERT_SUPPORT];
    CmscbbRevokeList revokeList[MAX_REVOKE_LIST_NUM];
} CmscbbCrlToUpdateList;

typedef struct CmscbbCrlBaseInfoSt {
    CVB_INT32 algorithm;
    CVB_CHAR* issuerName;
    CVB_UINT32 issuerLength;
    CVB_INT64 thisUpdateTime;
    CVB_INT64 nextUpdateTime;
    CVB_INT32 crlNumber;
    CVB_CHAR resv[16];
} CmscbbCrlBaseInfo;

typedef struct CmscbbCertBaseInfoSt {
    CmscbbSerialNum serialNum;
    CVB_INT32 algorithm;
    CVB_CHAR* issuerName;
    CVB_UINT32 issuerLength;
    CVB_INT64 notBefore;
    CVB_INT64 notAfter;
    CVB_CHAR* subjectName;
    CVB_UINT32 subjectLength;
    CVB_UINT64 publicKeyLength;
    CVB_CHAR resv[16];
} CmscbbCertBaseInfo;

typedef struct CmscbbCrlCertInfoListSt {
    CVB_INT32 crlCount;
    CVB_INT32 certCount;
    CmscbbCrlBaseInfo* crlInfoList[MAX_CRL_SUPPORT];
    CmscbbCertBaseInfo* certInfoList[MAX_CERT_SUPPORT];
} CmscbbCrlCertInfoList;
#endif /* CMSCBB_SUPPORT_CRL_COMPARE */

typedef struct CmscbbBufferSt {
    CVB_BYTE* buf;
    CVB_UINT32 len;
} CmscbbBuffer;

typedef struct CmscbbVoucherContentSt {
    CmscbbBuffer contentType;
    CmscbbBuffer content;
} CmscbbVoucherContent;

typedef struct CmscbbDecodeCrlParamSt {
    const CVB_BYTE* crl;
    CVB_UINT32 len;
    CVB_BOOL needVerify;
    CVB_BOOL saveCA;
} CmscbbDecodeCrlParam;

typedef struct CmscbbBufferParamSt {
    const CVB_BYTE* buf;
    CVB_UINT32 len;
} CmscbbBufferParam;

typedef enum CmscbbSignatureUsageTypeEm {
    USAGE_BUILD_SIGNING = 1,
    USAGE_RELEASE_SIGNING = 2,
    USAGE_MAX_SIGNING,  /* New type should be insert before USAGE_MAX_SIGNING and keep continious */
} CmscbbSignatureUsageType;

/*
 * Keep information for creating verify context
 * callback_funcs: platform related functions, which implemented by caller
 */
typedef struct CmscbbCtxCreateParamsSt {
    CVB_SIZE_T st_size; /* size of this struct */

#if CMSCBB_NEED_RELOCATE
    CVB_ULONG relocBase; /* system base address to relocate compiled address */
#endif
    CVB_CHAR resv[64]; /* 64 units reserved text in verify context */
} CmscbbCtxCreateParams;

typedef struct CmscbbSignatureInfoSt {
    CVB_UINT16 version;
    CVB_UINT32 hashAlgo;   /* CmscbbAoids */
    CVB_UINT32 signAlgo;
    CVB_UINT32 subAlgo; // keyLen or curveId
    CVB_INT64 signTime;
    CVB_VOID *signatureType;
    CVB_UINT32 signatureTypeLen;
    CVB_VOID *reservePoint[4];
    CVB_CHAR reserveByte[16];
} CmscbbSignatureInfo;

/*
 * Prototype    : CmscbbVerifyCreateCtx
 * Description  : Create verify context
 * Params
 *   [IN] ctx: Verify that the above
 *   [IN] params: Verify that the above build param
 * Return Value : return cvb_success correctly, error return error code
 * Remarks      : Params must be set correctly when initializing st_size = sizeof(CmscbbCtxCreateParams)
 *   Date              Author     Modification
 *   2015/11/09 15:52  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyCreateCtx(CMSCBB_VRF_CTX* ctx,
    const CmscbbCtxCreateParams* params);

#if CMSCBB_SUPPORT_FILE
/*
 * Prototype    : CmscbbVerifyAddCertFile
 * Description  : Add a CA certificate as a file and a signing certificate
 * Params
 *   [IN] ctx: Signature context
 *   [IN] certFile: Certificate file path
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 15:55  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyAddCertFile(CMSCBB_VRF_CTX ctx,
    const CVB_CHAR* certFile);
#endif

/*
 * Prototype    : CmscbbVerifyAddCert
 * Description  : Adding CA certificates and signing certificates as data
 * Params
 *   [IN] ctx: Signature context
 *   [IN] cert: X509Certificate data
 *   [IN] certLength: X509Certificate data length
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 15:57  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyAddCert(CMSCBB_VRF_CTX ctx,
    const CVB_BYTE* cert, CVB_UINT32 certLength);

#if CMSCBB_SUPPORT_FILE
/*
 * Prototype    : CmscbbVerifyAddCrlFile
 * Description  : Add a CRL list as a file
 * Params
 *   [IN] ctx: Signature context
 *   [IN] crlFile: CRL file path
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 15:58  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyAddCrlFile(CMSCBB_VRF_CTX ctx, const CVB_CHAR* crlFile);
#endif

/*
 * Prototype    : CmscbbVerifyAddCrl
 * Description  : Add a list of CRLs in data form
 * Params
 *   [IN] ctx: Signature context
 *   [IN] crl: CRL list
 *   [IN] crlLength: CRL data length
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:27  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyAddCrl(CMSCBB_VRF_CTX ctx,
    const CVB_BYTE* crl, CVB_UINT32 crlLength);

#if CMSCBB_SUPPORT_FILE
/*
 * Prototype    : CmscbbVerifyDetachSignatureQuick
 * Description  : Verifying a separate signature
 * Params
 *   [IN] ctx: Validation context
 *   [IN] srcfile: Verify the source file path of the signature file
 *   [IN] sigfile: CMS-formatted signature file path
 *   [OUT] res: Return validation result, 1 is passed, 0 is not passed
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:28  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyDetachSignatureQuick(CMSCBB_VRF_CTX ctx,
    const CVB_CHAR* srcfile, const CVB_CHAR* sigfile, CVB_INT32* res);
#endif

/*
 * Prototype    : CmscbbVerifyDetachSignatureBegin
 * Description  : Preparing verification signatures
 * Params
 *   [IN] ctx: Signature context
 *   [IN] signature: Signature data in CMS format
 *   [IN] sigLen: The length of the signature data
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:29  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyDetachSignatureBegin(CMSCBB_VRF_CTX ctx,
    const CVB_BYTE* signature, CVB_UINT32 sigLen);

/*
 * Prototype    : CmscbbVerifyDetachSignatureUpdate
 * Description  : Enter the contents of the signature
 * Params
 *   [IN] ctx: Signature context
 *   [IN] content: The data for the signature content, if the signature data is too large,
 *        you can call the update function multiple times, * Must ensure that the segmented data sequence is unchanged
 *   [IN] contentLength: The length of the signed content fragment data
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:31  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyDetachSignatureUpdate(CMSCBB_VRF_CTX ctx,
    const CVB_BYTE* content, CVB_INT32 contentLength);

/*
 * Prototype    : CmscbbVerifyDetachSignatureFinal
 * Description  : Verifying signatures
 * Params
 *   [IN] ctx: Signature context
 *   [IN] res: Validation results, 1 means pass, 0 means no.
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:35  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyDetachSignatureFinal(CMSCBB_VRF_CTX ctx, CVB_INT32* res);

/*
 * Prototype    : CmscbbVrfCtxFree
 * Description  : revoked Validation context
 * Params
 *   [IN] ctx: A CMSCBB_VERIFY_CTX structure that needs to be released
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:41  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVrfCtxFree(CMSCBB_VRF_CTX ctx);

/*
 * Prototype    : CmscbbResetCert
 * Description  : Reset certList
 * Params
 *   [IN] ctx: A CMSCBB_VERIFY_CTX structure that needs to be released
 *   [IN] isResetRoot: flag Whether to reset the root certificate
 * Return Value : return cvb_success correctly, error return error code
 */
CMSCBB_ERROR_CODE CmscbbResetCert(CMSCBB_VRF_CTX ctx, CVB_BOOL isResetRoot);

/*
 * Prototype    : CmscbbResetCrl
 * Description  : Reset crlList
 * Params
 *   [IN] ctx: A CMSCBB_VERIFY_CTX structure that needs to be released
 * Return Value : return cvb_success correctly, error return error code
 */
CMSCBB_ERROR_CODE CmscbbResetCrl(CMSCBB_VRF_CTX ctx);

#if CMSCBB_ALLOW_NO_CHECK_TSA_CRL
/*
 * Prototype    : CmscbbVerifyGetTsaCertSn
 * Description  : return the tsa cert's sn of cms
 * Params
 *   [IN] ctx: verify context
 *   [OUT] sn: serials number of tsa cert
 * Return Value : CMSCBB_ERROR_CODE
 * Remarks      : the sn will return correctly only when the cms verify passed
 *   Date              Author       Modification
 *   2016/07/14 10:37  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbVerifyGetTsaCertSn(CMSCBB_VRF_CTX ctx, CmscbbSerialNum* sn);
#endif

/*
 * Prototype    : CmscbbGetVersion
 * Description  : Get version number
 * Params
 *   [IN] :NA
 * Return Value : Return version Information
 *   Date              Author       Modification
 *   2015/11/09 18:42   tangzonglei  Create
 */
const CVB_CHAR* CmscbbGetVersion(CVB_VOID);

#if CMSCBB_SUPPORT_CRL_COMPARE
#if CMSCBB_SUPPORT_FILE
/*
 * Prototype    : CmscbbDecodeCrlFile
 * Description  : Get CRL File Information
 * Params
 *   [IN] ctx: Validation context
 *   [IN] crlFile: CRL file path
 *   [OUT] result:  Parse the result, including all CRL information in the file
 * Return Value : return cvb_success correctly, error return error code
 * Remarks      :The CRL file may have merged a CRL published by multiple root CAs, so the return result is
 *               a list of CRL information
 *   Date              Author       Modification
 *   2015/11/09 18:42   tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeCrlFile(CMSCBB_VRF_CTX ctx,
    const CVB_CHAR* crlFile, CmscbbCrlBundleInfo** result);
#endif

/*
 * Prototype    : CmscbbDecodeCrl
 * Description  : Resolving CRL Data
 * Params
 *   [IN] ctx: Validation context
 *   [IN] crl: CRL data address
 *   [IN] crlLength: CRL data length
 *   [OUT] pointerResult: CRL resolution results, containing all CRL information
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author       Modification
 *   2015/11/09 18:48   tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeCrl(CMSCBB_VRF_CTX ctx,
    const CVB_BYTE* crl, CVB_UINT32 crlLength, CmscbbCrlBundleInfo** pointerResult);

/*
 * Prototype    : CmscbbCrlFree
 * Params
 *   [IN] ctx: Validation context
 *   [IN] crlBundle: CRL resolution Results
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:48  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbCrlFree(CMSCBB_VRF_CTX ctx, CmscbbCrlBundleInfo* crlBundle);

/*
 * Prototype    : CmscbbCrlCompare
 * Description  : Compare two CRL file information which update
 * Params
 *   [IN] ctx: Validation context
 *   [IN] crlToUpdate: The object to compare
 *   [IN] crlOnDevice: The object to compare
 *   [OUT] stat: Results of comparisons,
 *           SCPS_NEW: S1 all CRLs are newer than S2.
 *           SCPS_OLD: S1 all CRLs are older than S2.
 *           SCPS_MIX: Cannot be compared, S1 has a new CRL than S2, and there are older CRLs than S2
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2015/11/09 18:49  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbCrlCompare(CMSCBB_VRF_CTX ctx, const CmscbbCrlBundleInfo* crlToUpdate,
    const CmscbbCrlBundleInfo* crlOnDevice, CmscbbCrlPeriodStat* stat);

/*
 * Prototype    : CmscbbDecodeCrlToList
 * Description  : Decode crl to crl list
 * Params
 *   [IN] ctx: Validation context
 *   [IN] crlParam: CRL data address and param
 *     1. For toUpdate crl file, the flag needVerify should be set CVB_TRUE;
 *     2. For onDevice crl file, needVerify set depend on product self, for saving memory consider, the flag needVerify
 *        and saveCA couild set CVB_FALSE, that means ondevice CrL not need verify integrity and no need to save the CA.
 *     3. The to update crl flag saveCA should same with on device crl file.
 *     4. The two flag should set be CVB_TRUE if you need verify the on device crl in both toUpdate and onDevice crl.
 *   [OUT] crlInfo: CRL resolution results, containing all CRL information
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author       Modification
 *   2021/02/26 18:48   yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeCrlToList(CMSCBB_VRF_CTX ctx, const CmscbbDecodeCrlParam* crlParam,
    CmscbbCrlToUpdateList** crlUpdateInfo);

/*
 * Prototype    : CmscbbCrlUpdateListCompare
 * Description  : Compare two CRL list and select newest CRLs in both crlToUpdate and crlOnDevice
 * Params
 *   [IN] ctx: Validation context
 *   [IN] crlToUpdate: The crl list to compare
 *   [IN] crlOnDevice: The crl list to compare
 *   [OUT] crlResult: Results of comparisons, the result will be the newest CRLs in both crlToUpdate and crlOnDevice
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/26 18:49  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbCrlUpdateListCompare(CMSCBB_VRF_CTX ctx, const CmscbbCrlToUpdateList* crlToUpdate,
    const CmscbbCrlToUpdateList* crlOnDevice, CmscbbCrlToUpdateList** crlResult);

/*
 * Prototype    : CmscbbCrlUpdateListFree
 * Params
 *   [IN] crlList: CRL Results to be free
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/02/26 18:48  yangdingfu  Create
 */
CMSCBB_ERROR_CODE CmscbbCrlUpdateListFree(CmscbbCrlToUpdateList* crlList);

/*
 * Prototype    : CmscbbDecodeCrlCertInfoToList
 * Description  : Decode crl info to  list
 * Params
 *   [IN] crl: CRL data address
 *   [IN] crlLength: CRL data length
 *   [OUT] infoList: CRL resolution results, containing all CRL information
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author       Modification
 *   2021/10/25 11:05  wangqinyu    Create
 */
CMSCBB_ERROR_CODE CmscbbDecodeCrlCertInfoToList(const CVB_BYTE* crl, CVB_UINT32 crlLength,
    CmscbbCrlCertInfoList** infoList);

/*
 * Prototype    : CmscbbCrlCertInfoListFree
 * Params
 *   [IN] infoList: CRL info list to be free
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/10/25 11:05  wangqinyu    Create
 */
CMSCBB_ERROR_CODE CmscbbCrlCertInfoListFree(CmscbbCrlCertInfoList* infoList);

#if CMSCBB_FOR_WIRELESS
/*
 * Prototype    : CmscbbObtainMinimumCrlSet
 * Params
 *   [IN] ctx: Validation context
 *   [IN] sig: The signature data
 *   [IN] crl: The crl data
 *   [OUT] crlResult: Result of obtaining the minimum CRL set
 * Return Value : return cvb_success correctly, error return error code
 *   Date              Author     Modification
 *   2021/04/26 18:48  zhanghui   Create
 */
CMSCBB_ERROR_CODE CmscbbObtainMinimumCrlSet(CMSCBB_VRF_CTX ctx, const CmscbbBufferParam *sig,
    const CmscbbBufferParam *crl, CmscbbCrlToUpdateList** crlResult);
#endif
#endif /* CMSCBB_SUPPORT_CRL_COMPARE */

/*
 * Prototype    : CmscbbVerifyVoucherSignatureQuick
 * Description  : verify voucher signature
 * Params
 *   [IN] ctx: Validation context
 *   [IN] sig: The voucher signature
 *   [IN] sigLen: The length of voucher signature
 *   [OUT] voucherContent: Results of voucher signature
 * Return Value : return cvb_success correctly, error return error code
 */
CMSCBB_ERROR_CODE CmscbbVerifyVoucherSignatureQuick(CMSCBB_VRF_CTX ctx, const CVB_BYTE* sig, CVB_UINT32 sigLen,
    CmscbbVoucherContent** voucherContent);

#if CMSCBB_SUPPORT_FILE
/*
 * Prototype    : CmscbbVerifyVoucherSignatureQuickFile
 * Description  : verify voucher signature
 * Params
 *   [IN] ctx: Validation context
 *   [IN] sigfile: The voucher signature file
 *   [OUT] voucherContent: Results of voucher signature
 * Return Value : return cvb_success correctly, error return error code
 */
CMSCBB_ERROR_CODE CmscbbVerifyVoucherSignatureQuickFile(CMSCBB_VRF_CTX ctx, const CVB_CHAR* sigfile,
    CmscbbVoucherContent** voucherContent);
#endif

/*
 * Prototype    : CmscbbDestoryVoucherContent
 * Description  : Destory the VoucherContent
 * Params
 *   [IN] voucherContent: Results of voucher signature
 * Return Value : return cvb_success correctly, error return error code
 */
void CmscbbDestoryVoucherContent(CmscbbVoucherContent* voucherContent);

/*
 * Prototype    : CmscbbSetSignatureUsageType
 * Description  : set signature usage type
 * Params
 *   [IN] ctx: Validation context
 *   [IN] usageType: the signature usage type, currently support building signing and release signing.
 * Return Value : return cvb_success correctly, error return error code
 */
CMSCBB_ERROR_CODE CmscbbSetSignatureUsageType(CMSCBB_VRF_CTX ctx, CmscbbSignatureUsageType usageType);

/*
 * @Memory operation:
 *      1、alloc: The interface alloc memory for CmscbbSignatureInfo and return the point. After the memory is used,
                the interface invokes CmscbbSignatureInfoFree to release the memory.
        2、size: 106B - 115B
 * @Thread safe: no
 * @Time consuming: no
 */
CMSCBB_ERROR_CODE CmscbbGetInfoFromSignature(const CVB_BYTE* sig, CVB_UINT32 sigLen,
    CmscbbSignatureInfo** signatureInfo);

/*
 * @Memory operation:
 *      1、free: The interface releases the memory of CmscbbSignatureInfo.
        2、size: 106B - 115B
 * @Thread safe: no
 * @Time consuming: no
 */
CVB_VOID CmscbbSignatureInfoFree(CmscbbSignatureInfo* signatureInfo);

/*
 * @Memory operation: Do not alloc or free memory.
 * @Thread safe: no
 * @Time consuming: no
 */
CMSCBB_ERROR_CODE CmscbbSetSignatureAlgoType(CMSCBB_VRF_CTX ctx, CVB_UINT32 algoSpecified, CVB_UINT32 subAlgo);

/*
 * @Memory operation: Do not alloc or free memory.
 * @Thread safe: no
 * @Time consuming: no
 */
CMSCBB_ERROR_CODE CmscbbResetSignatureAlgoType(CMSCBB_VRF_CTX ctx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* H_CMSCBB_CMS_VRF_H */
