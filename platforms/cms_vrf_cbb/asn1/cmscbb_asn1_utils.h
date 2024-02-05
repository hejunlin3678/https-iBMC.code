/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_ASN1_UTILS_H
#define H_CMSCBB_ASN1_UTILS_H
#include "../asn1/cmscbb_asn1_def.h"

/*
 * Prototype    : CmscbbCompareAsnOcts
 * Description  : compare two CmscbbAsnOcts
 * Return Value :
 *   if pAsnOctsExpect > pAsnOctsActual return >0
 *   else if pAsnOctsExpect == pAsnOctsActual return ==0
 *   else return <0
 *   Date              Author     Modification
 *   2015/08/03 9:35  tangzonglei  Create
 */
CVB_INT32 CmscbbCompareAsnOcts(const CmscbbAsnOcts* asnOctsExpect, const CmscbbAsnOcts* asnOctsActual);

#define CMSCBB_COMPARE_ASN_BITS CmscbbCompareAsnOcts
#define CMSCBB_COMPARE_ASN_OID CmscbbCompareAsnOcts

/*
 * Prototype    : CmscbbFindOid
 * Description  : Find object id by id index.
 * Params
 *   [IN] aoid: id index
 * Return Value : matched object id, or CVB_NULL if not found.
 *   Date              Author     Modification
 *   2015/07/17 11:39  tangzonglei  Create
 */
const CmscbbAsnOid* CmscbbFindOid(CmscbbAoids aoid);

/*
 * Prototype    : CmscbbFindAoid
 * Description  : find id index by object id
 * Params
 *   [IN] oid: object id
 * Return Value : matched id index, or AOID_UNKNOWN if not found.
 *   Date              Author     Modification
 *   2015/07/29 17:20  tangzonglei  Create
 */
CmscbbAoids CmscbbFindAoid(const CmscbbAsnOid* oid);

/* create tag from class, form and code */
#define CMSCBB_MAKE_TAG(tag, _cls, _form, _code) \
    do {                                         \
        (tag).cls = (CmscbbBerClass)(_cls);      \
        (tag).form = (CmscbbBerForm)(_form);     \
        (tag).code = (CmscbbBerTagCode)(_code);  \
    } while (0)

/*
 * Prototype    : CmscbbConvertFromTime
 * Description  : Convert time object into time structure.
 * Params
 *   [IN] asnTime: a string of time in  pAsnTime->octs
 *   [OUT] signId: timestamp
 *   Return Value : CMSCBB_ERROR_CODE
 *   Date              Author     Modification
 *   2015/08/17 14:30  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbConvertFromTime(const CmscbbAsnOcts* asnTime, CmscbbDatetime* signId);

/*
 * Prototype    : CmscbbConvertDatetimeToTime
 * Description  : Convert time object to time_t value
 * Params
 *   [IN] asnTime: a string of time in  pAsnTime->octs
 *   [OUT] time: a timestamp
 * Return Value : CMSCBB_ERROR_CODE
 *
 *   Date              Author     Modification
 *   2015/08/17 14:41  tangzonglei  Create
 */
CMSCBB_ERROR_CODE CmscbbConvertDatetimeToTime(const CmscbbAsnOcts* asnTime, CVB_TIME_T* time);

/*
 * Prototype    : CmscbbGetHashAoidFromSign
 * Description  : return associated hash algorithm from signature algorithm
 * Params
 *   [IN] signId: object id
 * Return Value : CmscbbAoids
 * Remarks      : Associated hash algorithm, or AOID_UNKNOWN if not found.
 *   Date              Author     Modification
 *   2015/11/11 9:47  tangzonglei  Create
 */
CmscbbAoids CmscbbGetHashAoidFromSign(const CmscbbAsnOid* signId);

/*
 * Prototype    : CmscbbGetEncAoidFromSign
 * Description  : return associated Encrypted algorithm from signature algorithm
 * Params
 *   [IN] signId: object id
 * Return Value : CMSCBB_AOIDS
 * Remarks      : Associated Encrypted algorithm, or AOIDS_UNKNOW if not found.
 */
CmscbbAoids CmscbbGetEncAoidFromSign(const CmscbbAsnOid* signId);

/*
 * Prototype    : CmscbbGetEncAoidFromSigAlgoAoid
 * Description  : return associated Encrypted algorithm from signature algorithm Aoid
 * Params
 *   [IN]encAlgo: encAlgo Aoid
 * Return Value : CMSCBB_AOIDS
 * Remarks      : Associated Encrypted algorithm, or AOIDS_UNKNOW if not found.
 */
CmscbbAoids CmscbbGetEncAoidFromSigAlgoAoid(const CmscbbAoids encAlgo);
#endif /* H_CMSCBB_ASN1_UTILS_H */

