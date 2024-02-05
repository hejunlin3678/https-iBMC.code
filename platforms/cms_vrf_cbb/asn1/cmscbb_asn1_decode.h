/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_ASN1_DECODE_H
#define H_CMSCBB_ASN1_DECODE_H
#include "../cms/cmscbb_templ.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Prototype    : RelocateTemplate
 * Description  : Address of static pointer in redirect template
 * Params
 *   [IN] asn1Item: asn1 object
 *   [IN] reloc_off: reset loaction offset
 * Return Value : None
 *   Date              Author     Modification
 *   2016/08/03 13:36  tangzonglei  Create
 */
CVB_VOID RelocateTemplate(CmscbbAsn1Item* asn1Item, CVB_ULONG relocOff);

/*
 * Prototype    : CmscbbAsnDecode
 * Description  : decode a DER encoded asn item
 * Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/11/11 9:55  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbAsnDecode(const CVB_BYTE* buf, CVB_UINT32 bufLen, struct CmscbbAsn1ItemSt* asnItem,
    CVB_BYTE flag, CVB_VOID** result, CVB_UINT32* decodedLen);

CMSCBB_ERROR_CODE CmscbbDecodeSeq(CmscbbBuf* b, CVB_VOID* v, CVB_UINT32* bytesDecoded, const CmscbbAsn1Template* templ);

CMSCBB_ERROR_CODE CmscbbDecodeOctet(CmscbbBuf *b, CVB_VOID *v, CVB_UINT32 *bytesDecoded,
    const CmscbbAsn1Template *templ);

CMSCBB_ERROR_CODE CmscbbDecodeMap(CmscbbBuf* b, CVB_VOID* v, CVB_UINT32* bytesDecoded, const CmscbbAsn1Template* templ);

CMSCBB_ERROR_CODE CmscbbDecodeSetof(CmscbbBuf* b, CVB_VOID* v, CVB_UINT32* bytesDecoded,
    const CmscbbAsn1Template* templ);

CMSCBB_ERROR_CODE CmscbbOidToExku(CmscbbBuf *b, CVB_VOID *v, CVB_UINT32 *bytesDecoded, const CmscbbAsn1Template* templ);

CMSCBB_ERROR_CODE CmscbbBitsToInt(CmscbbBuf *b, CVB_VOID *v, CVB_UINT32 *bytesDecoded, const CmscbbAsn1Template* templ);

CMSCBB_ERROR_CODE CmscbbDecodeInt(CmscbbBuf* b, CVB_VOID* v, CVB_UINT32* bytesDecoded, const CmscbbAsn1Template* templ);

CMSCBB_ERROR_CODE CmscbbDecodeBits(CmscbbBuf *b, CVB_VOID *v, CVB_UINT32 *bytesDecoded,
    const CmscbbAsn1Template* templ);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* H_CMSCBB_ASN1_DECODE_H */
