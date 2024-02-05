/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Signature Verify CBB Library
 * Author: tangzonglei
 * Create: 2015
 * History: 2018/11/23 yebin code rule fixes
 */
#ifndef H_CMSCBB_TEMPL_H
#define H_CMSCBB_TEMPL_H
#include "cmscbb_config.h"
#include "../asn1/cmscbb_asn1_def.h"
#include "../cmscbb_common/cmscbb_buf.h"

#ifdef __cplusplus
extern "C" {
#endif
#define CVB_ASN_NORMAL      0
#define CVB_ASN_OPTIONAL    1
#define CVB_ASN_PEEK        2
#define CVB_ASN_RAW_DER     4
#define CVB_ASN_SINK        8
#define CVB_ASN_DEFAULT     16
#define CVB_ASN_IGNORE      32


#define CVB_ASN_NO_TAGGING 0
#define CVB_ASN_EXPLICIT   1
#define CVB_ASN_IMPLICIT   2

typedef struct CmscbbAsn1TemplateSt {
    struct CmscbbAsn1ItemSt* asn1Item;
#if CMSCBB_RELOC_STRICT_CHECK
    CVB_UINT32 relocted;
#endif
    CVB_UINT32 tagCode;  /* only the code part of the tag */
    CVB_UINT16 tagType;  /* SEC_ASN_NO_TAGGING or SEC_ASN_EXPLICIT or SEC_ASN_IMPLICIT */
    CVB_UINT16 flag;     /* Normal | Optional | Default | ASN ANY | Pointer */
    CVB_UINT32 offsetVal;  /* Pointer to the field that will be encoded / decoded */
#ifdef CVB_DEBUG
    const CVB_CHAR* nameVal;
#endif
    CVB_VOID* defaultVal;  /* To store the default values */
} CmscbbAsn1Template;

typedef CVB_UINT32(*CVB_ASN_DECODE_ITEM)(CmscbbBuf* buf, CVB_VOID* result, CVB_UINT32* decodeLen,
    const struct CmscbbAsn1TemplateSt* templ);

typedef struct CmscbbAsn1AnyMapSt {
    CmscbbAsnInt iAnyId;
    CmscbbAsn1Template *anyTempl;
} CmscbbAsn1AnyMap;

typedef struct CmscbbAsn1FunctionsSt {
    CVB_ASN_DECODE_ITEM decode;
#if CMSCBB_RELOC_STRICT_CHECK
    CVB_UINT32 relocted;
#endif
} CmscbbAsn1Functions;

typedef struct CmscbbAsn1ItemSt {
    struct CmscbbAsn1TemplateSt* asnTempl;
    CmscbbAsn1Functions* asnFunc;
#if CMSCBB_RELOC_STRICT_CHECK
    CVB_UINT32 relocted;
#endif
    CmscbbBerClass berClass;
    CmscbbBerForm berForm;
    CmscbbBerTagCode berCode;
    CVB_UINT32 templCount;
    CVB_UINT32 itemSzie; /* if parent item is "sequence of", then this is for elements' size, not the list structure */
} CmscbbAsn1Item;

#ifdef __64BIT
#define STRUCT_OFFSET(structure, field) ((CVB_UINT32)(CVB_PTR)(&((structure*)(CVB_ULONG)0)->field))
#else
#define STRUCT_OFFSET(structure, field) ((CVB_UINT32)(CVB_PTR)(&((structure*)(CVB_ULONG)1)->field) - 1)
#endif

#define CMSCBB_ASN_TEMPLATE_BEGIN(tname) CVB_STATIC CmscbbAsn1Template g_templ##tname[]
#define CMSCBB_ASN_ANY_MAP_BEGIN(tname) CVB_STATIC CmscbbAsn1AnyMap g_anyMap##tname[]
#define CMSCBB_ASN_TEMPLATE_MAP_BEGIN(tname) CVB_STATIC CmscbbAsn1AnyMap g_map##tname[]

#if CMSCBB_RELOC_STRICT_CHECK
#define CMSCBB_RELOC_INIT_VAL CVB_FALSE,
#else
#define CMSCBB_RELOC_INIT_VAL
#endif

#if CMSCBB_DELAY_ADDRESS_SET
#define CVB_NAME2ITEM(name) CVB_NULL
#else
#define CVB_NAME2ITEM(name) (&(g_item##name))
#endif

#ifdef CVB_DEBUG
#define CMSCBB_ASN_NORMAL_ITEM(tname, subtname, field) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_NORMAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_IMPLICIT_PEEK_ITEM(tname, subtname, field, tagCode) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagCode, CVB_ASN_IMPLICIT, CVB_ASN_PEEK | CVB_ASN_RAW_DER | CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_PEEK_ITEM(tname, field) { CVB_NAME2ITEM(CmscbbAsnOcts), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_PEEK | CVB_ASN_RAW_DER, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_SET_ITEM(tname, subtname, field, count) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_NORMAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, (CVB_VOID*)(count) }
#define CMSCBB_ASN_SET_ITEM_WITH_FLAG(tname, flag, subtname, field, count) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, (CVB_VOID*)(count) }
#define CMSCBB_ASN_ITEM_WITH_FLAG(tname, flag, subtname, field) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_OPTIONAL_ITEM(tname, subtname, field) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_IGNORE_ITEM(tname, subtname, flag) { CVB_NAME2ITEM(subtname), 0, CMSCBB_RELOC_INIT_VAL CVB_ASN_NO_TAGGING, CVB_ASN_IGNORE | (flag), CVB_NULL, "ignored", CVB_NULL }
#define CMSCBB_ASN_IGNORE_NORMAL_ITEM(tname, subtname) { CVB_NAME2ITEM(subtname), 0, CMSCBB_RELOC_INIT_VAL CVB_ASN_NO_TAGGING, CVB_ASN_IGNORE, CVB_NULL, "ignored", CVB_NULL }
#define CMSCBB_ASN_IGNORE_EX_ITEM(tname, subtname, flag, tag, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, tag, CVB_ASN_IGNORE | (flag), CVB_NULL, "ignored", CVB_NULL }
#define CMSCBB_ASN_DEFAULT_ITEM(tname, subtname, field, defaultvalue) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_DEFAULT, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, (CVB_VOID*)(defaultvalue) }
#define CMSCBB_ASN_IMPLICIT_ITEM(tname, subtname, field, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_IMPLICIT, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_IMPLICIT_ITEM_WITH_FLAG(tname, subtname, field, flag, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_IMPLICIT, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_EXPLICIT_ITEM(tname, subtname, field, flag, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_EXPLICIT, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_NORMAL_IMP_ITEM(tname, subtname, field, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_IMPLICIT, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_MAP_ITEM(tname, subtname, field, id) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL id, CVB_ASN_NO_TAGGING, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_MAP_FLAG_ITEM(tname, subtname, field, flag, id) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL id, CVB_ASN_NO_TAGGING, (flag) | CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#define CMSCBB_ASN_MAP_KEY_ITEM() { CVB_NAME2ITEM(CmscbbAsnOid), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_MAP_KEY, CVB_NULL, "map key", CVB_NULL }
#define CMSCBB_ASN_MAP_VALUE_ITEM(tname, field) { CVB_NULL, CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_MAP_VALUE, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, (CVB_VOID*)&(tname##_map) }
#define CMSCBB_ASN_PEEK_OPTIONAL_ITEM(tname, field) { CVB_NAME2ITEM(CmscbbAsnOcts), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_PEEK | CVB_ASN_RAW_DER | CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), #field, CVB_NULL }
#else
#define CMSCBB_ASN_NORMAL_ITEM(tname, subtname, field) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_NORMAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_IMPLICIT_PEEK_ITEM(tname, subtname, field, tagCode) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagCode, CVB_ASN_IMPLICIT, CVB_ASN_PEEK | CVB_ASN_RAW_DER | CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_PEEK_ITEM(tname, field) { CVB_NAME2ITEM(CmscbbAsnOcts), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_PEEK | CVB_ASN_RAW_DER, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_SET_ITEM(tname, subtname, field, count) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_NORMAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), (CVB_VOID*)(count) }
#define CMSCBB_ASN_SET_ITEM_WITH_FLAG(tname, flag, subtname, field, count) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), (CVB_VOID*)(count) }
#define CMSCBB_ASN_ITEM_WITH_FLAG(tname, flag, subtname, field) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_OPTIONAL_ITEM(tname, subtname, field) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_IGNORE_ITEM(tname, subtname, flag) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_IGNORE | (flag), CVB_NULL, CVB_NULL }
#define CMSCBB_ASN_IGNORE_NORMAL_ITEM(tname, subtname) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_IGNORE, CVB_NULL, CVB_NULL }
#define CMSCBB_ASN_IGNORE_EX_ITEM(tname, subtname, flag, tag, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, tag, CVB_ASN_IGNORE | (flag), CVB_NULL, CVB_NULL }
#define CMSCBB_ASN_DEFAULT_ITEM(tname, subtname, field, defaultvalue) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_DEFAULT, (CVB_UINT32)STRUCT_OFFSET(tname, field), (CVB_VOID*)(defaultvalue) }
#define CMSCBB_ASN_IMPLICIT_ITEM(tname, subtname, field, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_IMPLICIT, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_EXPLICIT_ITEM(tname, subtname, field, flag, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_EXPLICIT, flag, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_NORMAL_IMP_ITEM(tname, subtname, field, tagnum) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL tagnum, CVB_ASN_IMPLICIT, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_MAP_ITEM(tname, subtname, field, id) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL id, CVB_ASN_NO_TAGGING, CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_MAP_FLAG_ITEM(tname, subtname, field, flag, id) { CVB_NAME2ITEM(subtname), CMSCBB_RELOC_INIT_VAL id, CVB_ASN_NO_TAGGING, (flag) | CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#define CMSCBB_ASN_MAP_KEY_ITEM() { CVB_NAME2ITEM(CmscbbAsnOid), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_MAP_KEY, CVB_NULL, CVB_NULL }
#define CMSCBB_ASN_MAP_VALUE_ITEM(tname, field) { CVB_NULL, CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_MAP_VALUE, (CVB_UINT32)STRUCT_OFFSET(tname, field), (CVB_VOID*)&(tname##_map) }
#define CMSCBB_ASN_PEEK_OPTIONAL_ITEM(tname, field) { CVB_NAME2ITEM(CmscbbAsnOcts), CMSCBB_RELOC_INIT_VAL 0, CVB_ASN_NO_TAGGING, CVB_ASN_PEEK | CVB_ASN_RAW_DER | CVB_ASN_OPTIONAL, (CVB_UINT32)STRUCT_OFFSET(tname, field), CVB_NULL }
#endif

#define CMSCBB_ASN_TEMPLATE_END(tname, cb, tagcode) CmscbbAsn1Item g_item##tname = { g_templ##tname, &(cb), CMSCBB_RELOC_INIT_VAL CBC_UNIV, CBF_CONS, tagcode, sizeof(g_templ##tname) / sizeof(CmscbbAsn1Template), sizeof(tname) }

#define CMSCBB_ASN_TEMPLATE_END_STATIC(tname, cb, tagcode) CVB_STATIC CmscbbAsn1Item g_item##tname = { g_templ##tname, &(cb), CMSCBB_RELOC_INIT_VAL CBC_UNIV, CBF_CONS, tagcode, sizeof(g_templ##tname) / sizeof(CmscbbAsn1Template), sizeof(tname) }

#if CMSCBB_DELAY_ADDRESS_SET
CVB_VOID CmscbbAsn1TemplInit(CVB_VOID);
CVB_VOID CmscbbCmsTemplInit(CVB_VOID);
CVB_VOID CmscbbX509TemplInit(CVB_VOID);
#endif

CmscbbAsn1Item* CmscbbGetCmscbbX509Cert(void);
CmscbbAsn1Item* CmscbbGetCmscbbX509Crl(void);
CmscbbAsn1Item* CmscbbGetCmscbbPkcs7Content(void);
CmscbbAsn1Item* CmscbbGetCmscbbTimestampInfo(void);
#if CMSCBB_SUPPORT_INDIRECT_CRL
CmscbbAsn1Item* CmscbbGetX509IssuingDistPoint(void);
#endif
CmscbbAsn1Item* CmscbbGetCmscbbX509BasicConstraints(void);
CmscbbAsn1Item* CmscbbGetCmscbbAsnOcts(void);
CmscbbAsn1Item* CmscbbGetCmscbbAsnBits(void);
CmscbbAsn1Item* CmscbbGetCmscbbAsnOidBundle(void);
CmscbbAsn1Item* CmscbbGetCmscbbX509ExtensionEntry(void);
CmscbbAsn1Item* CmscbbGetCmscbbAsnEnum(void);
CmscbbAsn1Item* CmscbbGetCmscbbEccSign(void);
#ifdef __cplusplus
}
#endif

#endif /* H_CMSCBB_TEMPL_H */
