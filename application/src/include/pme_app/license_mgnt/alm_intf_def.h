/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: Ϊ���ļ���ϵ�������˶��ģ���õ��Ľṹ���塣
 * Author: AdaptiveLM team
 * Create: 2015-07-14
 */
#ifndef ALM_INTF_DEF_H
#define ALM_INTF_DEF_H

#include "alm_intf_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#if defined(ALM_PACK_4)
#pragma pack(4)
#elif defined(ALM_PACK_8)
#pragma pack(8)
#endif

/************************************************************************
 * ����洢License�ļ���ؽṹ��
 ************************************************************************/
/*
 * ����: ָ����Ч�ڡ���Ч�ڲ��ý�ֹ���ڼ�������������ʾ��
 *   ����ͬʱָ����Ҳ���Խ�ָ������һ�����û��ָ����������Ч�ڡ�
 *   ���ü�����License����ָ���˽�ֹ���ڡ�
 *   ���ڹ���Stick/����������License�����ָ�������������
 * ��Ա:
 * acDeadLine - ��ֹ���ڣ��ڴ����ڵ���һ��License������Ч�ڡ����ַ�����ʾ
 *   ��ָ����ֹ���ڡ�
 *   ��Ϊ���ô�ֵΪ����ALM_CONST_LIC_DEADLINE_PERM��Ӧ���ַ�����
 * uiRelDays - �������е���������Ϊ0����ָ����ֵ��
 */
typedef struct {
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiRelDays;
} ALM_LIC_TIME_VALID_INFO_STRU;

/*
 * ����: ʱ�����ơ�
 * ��Ա:
 * 1. stValid - ��Ч�ڡ�
 * 2. uiGraceDays - ����������������Ϊ0���޿��ޡ�
 */
typedef struct {
    ALM_LIC_TIME_VALID_INFO_STRU stValid;
    ALM_UINT32 uiGraceDays;
} ALM_LIC_TIME_INFO_STRU;

/* ����BBOM���� */
typedef enum {
    /* ��Դ������ */
    ALM_LIC_BBOM_RESOURCE,

    /* ���������ͣ�ȡֵ0����0 */
    ALM_LIC_BBOM_FUNCTION
} ALM_LIC_BBOM_TYPE_ENUM;

/* License�ļ����� */
typedef enum {
    ALM_LIC_TYPE_DEMO,
    ALM_LIC_TYPE_COMM,
    ALM_LIC_TYPE_STICK,
    ALM_LIC_TYPE_EMERGENCY,
    ALM_LIC_TYPE_DFLT,
} ALM_LIC_TYPE_ENUM;

/**
 *  ALM_LIC_LK_LOCKMODEL_ENUM
 *
 *  ALM_LIC_LK_LOCKMODEL_NODELOCK Specifies that the Esn node locked Commercial
 *  ALM_LIC_LK_LOCKMODEL_DUALLOCK Specifies that the Esn dual node locked Demo(Commissioning)
 *  ALM_LIC_LK_LOCKMODEL_BUTT Specifies invalid lock model
 */
/* Classification of LK lock model. */
typedef enum {
    /* node lock model */
    ALM_LIC_LK_LOCKMODEL_NODELOCK = 1,
    /* dual lock model */
    ALM_LIC_LK_LOCKMODEL_DUALLOCK,
    /* Unused */
    ALM_LIC_LK_LOCKMODEL_BUTT
} ALM_LIC_LK_LOCKMODEL_ENUM;

typedef enum {
    /* Normal */
    ALM_NORMAL = 1,
    /* package */
    ALM_PACK,
    /* UnPack */
    ALM_UNPACK
} ALM_SALEITEM_TYPE_ENUM;

/* ���浥��ESN */
typedef struct {
    ALM_CHAR acEsn[ALM_CONST_LIC_SINGLE_ESN_LEN_MAX + 1];
} ALM_LIC_ESN_STRU;

/* ������ESN */
typedef struct {
    ALM_LIC_ESN_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_ESNS_STRU;

/* ����ָ��lsn��ʧЧ�� */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_CHAR acRvkTicket[ALM_CONST_RVK_TIKET_LEN + 1];
} ALM_LIC_LSN_RVK_TICKET_STRU;

/* ����ʧЧ�� */
typedef struct {
    ALM_CHAR acRvkTicket[ALM_CONST_RVK_TIKET_LEN + 1];
    ALM_CHAR acRvkTime[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_RVK_TICKET_STRU;

/* ���ڱ���License�ļ�ͨ����Ϣ */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eLicType;
    ALM_LIC_ESNS_STRU stEsns;
    ALM_CHAR acFilePrdName[ALM_CONST_PRD_NAME_LEN_MAX + 1];
    ALM_CHAR acFilePrdVer[ALM_CONST_PRD_VER_MAX + 1];
    ALM_UINT32 uiAllowCount;
    ALM_UINT32 uiGraceDays;
    ALM_CHAR acSoftId[ALM_CONST_LIC_SOFT_ID_MAX + 1];

    /*
     * ��ΪESN ANY/DEMO/��Ч�ڴ���3���ҷ����õ��ض�License����ֵΪ��Ч��������
     * ����Ϊ0��
     * ��Ч��Ϊ��ֹ���� - ����ʱ�䣬����ʱ�䶼ֻ��ȷ����(����ʱ����)��
     * �ڲ�Ʒ�ĵ����汾�����У�ȫ����һ��License������Ч����ʱ��ᱻ���Ӽ�¼��
     * �汾�������Ϊ0��
     * ����ֵ��Ϊ0����ֵֻ�в�С�ڻ��۵�����Ч����ʱ����������
     * �����޷�������㻹δ����ֹ���ڡ����߻����ڿ��ޣ����������
     */
    ALM_UINT32 uiAnyDemoDays;

    ALM_BOOL bHaveRvked;
    ALM_LIC_RVK_TICKET_STRU stRvkcTiket;
} ALM_LIC_STORE_GENERAL_STRU;

/* ����: �洢license��bbom��Ϣ�� */
typedef struct {
    ALM_CHAR *pcBBomName;
    ALM_CHAR *pcGroupName;
    ALM_CHAR *pcFeatureName;
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;
    ALM_UINT64 ulValue;
    ALM_UINT32 uiSameNameId;
    ALM_UINT32 uiFeatureId;
    ALM_LIC_TIME_INFO_STRU stTimeInfo;
    ALM_UINT32 uiReserved;
} ALM_LIC_STORE_BBOM_STRU;

/* ��װ���bbom info���󣬷������ */
typedef struct {
    ALM_LIC_STORE_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_BBOMS_STRU;

/* ����SBOM��BBOM */
typedef struct {
    ALM_LIC_STORE_BBOM_STRU stBbom;
} ALM_LIC_STORE_S2BBOM_STRU;

/* ����SBOM��BBOM */
typedef struct {
    ALM_LIC_STORE_S2BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_S2BBOMS_STRU;

/* ����Sbom��Values */
typedef struct {
    ALM_UINT64 ulVal;
    ALM_CHAR acDeadline[ALM_CONST_DATE_LEN + 1];
    ALM_BOOL bIsnoControl;
} ALM_LIC_STORE_SBOM_VALUE_STRU;

/* ����Sbom��Values */
typedef struct {
    ALM_LIC_STORE_SBOM_VALUE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_SBOM_VALUES_STRU;

/* ��ҵģʽ:ָ���ĺ�ֵ�͹淶�и���ֵ����һ�� */
typedef enum {
    ALM_BUSINESS_MODE_NONE = 0,
    ALM_PERPETUAL_LICENSE = 1,
    ALM_SNS = 2,
    ALM_ASRF = 3,
    ALM_TBL = 4,
    ALM_SUS = 5,
    ALM_TBL_PERPETUAL = 7
} ALM_BUSINESS_MODE_ENUM;

/* ����License�ļ�����SBOM��Ϣ */
typedef struct {
    ALM_CHAR *pcName;
    /* ��ѽ�ֹ���� */
    ALM_CHAR *pcAnnualFeeValidDate;
    /* ��ҵģʽ, ���license�ļ���û���ṩBusinessModel����, ��ΪNONE */
    ALM_ENUM(ALM_BUSINESS_MODE_ENUM) eBusinessModel;
    ALM_CHAR *pcClassificationCode;
    ALM_UINT64 ulAnnualFeeQty;
    ALM_CHAR *pcChsUnit;
    ALM_CHAR *pcEngUnit;
    ALM_CHAR *pcChsDesc;
    ALM_CHAR *pcEngDesc;
    ALM_LIC_STORE_SBOM_VALUES_STRU stValues;
    ALM_LIC_STORE_S2BBOMS_STRU stS2Bboms;
    ALM_ENUM(ALM_SALEITEM_TYPE_ENUM) eSaleItemType;
    ALM_UINT32 uiPackageItemIdx;
    /*
     * ������stS2Bboms������S2B�У�����quoteUnitEN���ԱpcEngUnit��ͬ��S2B,
     * ����stS2Bboms�е�һ��S2B(������ǰ����ͬ�����) �����ֺͶ�Ӧ��ϵ�� */
    ALM_CHAR *pcS2BName;
    float amount;
} ALM_LIC_STORE_SBOM_STRU;

/* ����License�ļ�SBOM��Ϣ */
typedef struct {
    ALM_LIC_STORE_SBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_SBOMS_STRU;

/* ����PackageItem��Value */
typedef struct {
    ALM_UINT64 ulVal;
    ALM_CHAR acDeadline[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiReserved;
} ALM_LIC_STORE_PACKAGEITEM_VALUE_STRU;

/* ����PackageItem��Values */
typedef struct {
    ALM_LIC_STORE_PACKAGEITEM_VALUE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_PACKAGEITEM_VALUES_STRU;

typedef struct {
    ALM_LIC_STORE_SBOM_STRU *pstSbom;
} ALM_LIC_STORE_P2SBOM_STRU;

typedef struct {
    ALM_LIC_STORE_P2SBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_P2SBOMS_STRU;

/* ����PackageItem */
typedef struct {
    ALM_CHAR acName[ALM_CONST_LIC_PACK_NAME_MAX + 1];
    ALM_CHAR acAnnualFeeValidDate[ALM_CONST_DATE_LEN + 1];
    ALM_ENUM(ALM_BUSINESS_MODE_ENUM) eBusinessModel;
    ALM_CHAR acClassificationCode[ALM_CONST_LIC_CLASSIFICATIONCODE_MAX + 1];
    ALM_UINT64 ulAnnualFeeQty;
    ALM_CHAR acChsUnit[ALM_CONST_LIC_PACK_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acEngUnit[ALM_CONST_LIC_PACK_DESC_ENG_UNIT_MAX + 1];
    ALM_CHAR acChsDesc[ALM_CONST_LIC_PACK_DESC_CHS_MAX + 1];
    ALM_CHAR acEngDesc[ALM_CONST_LIC_PACK_DESC_ENG_MAX + 1];
    ALM_LIC_STORE_PACKAGEITEM_VALUES_STRU stValues;
    ALM_LIC_STORE_P2SBOMS_STRU stP2Sboms;
} ALM_LIC_STORE_PACKAGEITEM_STRU;

/* ����SBOM�������� */
typedef struct {
    ALM_LIC_STORE_PACKAGEITEM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_PACKAGEITEMS_STRU;

typedef struct {
    ALM_CHAR acOfferName[ALM_CONST_LIC_OFFERING_NAME_MAX + 1];
    ALM_CHAR acOfferVer[ALM_CONST_LIC_OFFERING_VER_MAX + 1];
    /* GAʱ�� */
    ALM_CHAR acGADate[ALM_CONST_DATE_LEN + 1];
    /* EOFSʱ�� */
    ALM_CHAR acEOFSDate[ALM_CONST_DATE_LEN + 1];
    /* PoolDevSeries */
    ALM_CHAR acPoolDevSeries[ALM_CONST_LIC_OFFERING_POOLDEVSERIES_MAX + 1];
    ALM_LIC_STORE_SBOMS_STRU stSboms;
    ALM_LIC_STORE_PACKAGEITEMS_STRU stPackageItems;
} ALM_LIC_STORE_OFFER_STRU;

typedef struct {
    ALM_LIC_STORE_OFFER_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_OFFERS_STRU;

/*
 * ����License�ļ�����ʹ�õ����㷨: �궨���ֵ���ܱ�
 */
typedef enum {
    ALM_ALG_LIC_PARSER_RSA_2048 = 0,
    ALM_ALG_LIC_PARSER_RSA_1024 = 1,
    ALM_ALG_LIC_PARSER_EC2N_163 = 2,
    ALM_ALG_LIC_PARSER_EC2N_233 = 3,
    ALM_ALG_LIC_PARSER_EC2N_283 = 4,
    ALM_ALG_LIC_PARSER_RSA_3072 = 5
} ALM_ALG_LIC_PARSER_ALG_ENUM;

/* ����License�ļ�������Ϣ */
typedef struct {
    ALM_CHAR acFileCopyRight[ALM_CONST_LIC_COPY_RIGHT_MAX + 1];
    ALM_CHAR acFileFormatVersion[ALM_CONST_LIC_FILE_VER_LEN_MAX + 1];
    ALM_CHAR acLicFileType[ALM_CONST_LIC_FILE_TYPE_LEN_MAX + 1];
    ALM_CHAR acFileCreator[ALM_CONST_LIC_CREATER_MAX + 1];
    ALM_CHAR acFileIssuer[ALM_CONST_LIC_ISSUER_MAX + 1];
    ALM_CHAR acFileCreateTime[ALM_CONST_DATE_LEN + 1];
    /* ��������������(�����ѽ�ֹ����) */
    ALM_CHAR acSwUpgradeDueDate[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acFileDataVersion[ALM_CONST_LIC_DATAVERSION_MAX + 1];

    ALM_CHAR acFileCountry[ALM_CONST_LIC_COUNTRY_MAX + 1];
    ALM_CHAR acFileOffice[ALM_CONST_LIC_OFFICE_MAX + 1];
    ALM_CHAR acFileCustom[ALM_CONST_LIC_CUSTOMER_MAX + 1];
    ALM_CHAR acFileDomain[ALM_CONST_LIC_DOMAIN_MAX + 1];

    ALM_UINT32 uiLkNodeSequense;
    ALM_ENUM(ALM_LIC_LK_LOCKMODEL_ENUM) eLkLockModel;
    ALM_CHAR acFileNodeName[ALM_CONST_LIC_NODEINFO_NODE_MAX + 1];
    ALM_CHAR acFileNodeDes[ALM_CONST_LIC_NODE_DES_MAX + 1];

    ALM_CHAR *pcFileComment;

    ALM_ENUM(ALM_ALG_LIC_PARSER_ALG_ENUM) eComAlg;
    ALM_ENUM(ALM_ALG_LIC_PARSER_ALG_ENUM) ePrdAlg;
    /* ���ά����ֹ���� */
    ALM_CHAR acSwMaintainDueDate[ALM_CONST_DATE_LEN + 1];
    /* Ӳ��ά����ֹ���� */
    ALM_CHAR acHwMaintainDueDate[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_STORE_ADDITION_STRU;

/* ���ڱ���������License�ļ��Ľṹ�� */
typedef struct {
    ALM_LIC_STORE_GENERAL_STRU stGeneralInfo;
    ALM_LIC_STORE_BBOMS_STRU stBbomInfo;
    ALM_LIC_STORE_OFFERS_STRU stOfferInfo;
    ALM_LIC_STORE_ADDITION_STRU stAddtion;
    /* ���ֶα�ʾ�Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_STORE_STRU;

/* ���ڱ���License�ļ������Խṹ�� */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eLicType;
    ALM_CHAR acFileFormatVersion[ALM_CONST_LIC_FILE_VER_LEN_MAX + 1];
} ALM_LIC_ATTRI_STRU;

/**
 * ����������λ��
 * 1. uiStreamSize - ���Ĵ�С��
 * 2. pcStream  - ���ĳ�ʼλ��
 */
typedef struct TAG_ALM_LIC_STREAM_VAR_STRU {
    ALM_UINT32 uiStreamSize;
    ALM_CHAR *pcStream;
} ALM_LIC_STREAM_VAR_STRU;

typedef ALM_LIC_STREAM_VAR_STRU ALM_LIC_RES_LIST;
typedef ALM_LIC_STREAM_VAR_STRU ALM_LIC_FUN_LIST;

/**
 * ����:�洢Feature�������Ϣ��
 * acAtrrib--Ŀǰ��license�ļ����ͣ���ֹ���ڣ�������
 */
typedef struct {
    ALM_CHAR acFilePrdName[ALM_CONST_PRD_NAME_LEN_MAX + 1];
    ALM_CHAR acFilePrdVer[ALM_CONST_PRD_VER_MAX + 1];
    ALM_CHAR acFeatName[ALM_CONST_LIC_GROUP_NAME_MAX + 1];
    ALM_CHAR acEsn[ALM_CONST_LIC_ESN_FEAT_FORMAT_LEN_MAX + 1];
    ALM_CHAR acAtrrib[ALM_CONST_LIC_ATRRIBULT_MAX + 1];
} ALM_LIC_FEAT_ATRRIBULT_STRU;

/**
 * ����: �洢����Feature��Ϣ��
 */
typedef struct {
    ALM_LIC_FEAT_ATRRIBULT_STRU stFeatAtrrib;
    ALM_LIC_RES_LIST stResList;
    ALM_LIC_FUN_LIST stFunList;
} ALM_LIC_FEAT_INFO_STRU;

/* ���ڴ洢������Feature��Ϣ */
typedef struct {
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    ALM_LIC_FEAT_INFO_STRU *pstArray;
    /* ���ֶα�ʾ�Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_FEATS_INFO_STRU;

/* ����spart��Ϣ */
typedef struct {
    ALM_CHAR *pcSpartName;      /* spart���� */
    ALM_UINT64 ulSpartPlanVal;  /* spart�滮�� */
    ALM_UINT64 ulSpartUsageVal; /* spartʹ���� */
} ALM_S2B_ONE_SPART_RSLT_STRU;

/* spart��Ϣ */
typedef struct {
    ALM_S2B_ONE_SPART_RSLT_STRU *pstArray; /* spart���� */
    ALM_UINT32 uiNum;                      /* �����С */
} ALM_S2B_SPART_RSLT_STRU;

/* ���ڳ�ʼ����ѯ�����ALM_LIC_STORE_STRU���� */
LIC_EXPORT ALM_VOID ALM_StInitLicStore(INOUT ALM_LIC_STORE_STRU *pstLic);

/* ����ALM_LIC_STORE_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsLicStore(INOUT ALM_LIC_STORE_STRU *pstLic);

/* ��ʼ��ALM_LIC_FEATS_INFO_STRU�ṹ�� */
LIC_EXPORT ALM_VOID ALM_StInitLicFeatsInfo(INOUT ALM_LIC_FEATS_INFO_STRU *pstLicFeatsInfo);

/* ����ALM_LIC_FEATS_INFO_STRU�ṹ�� */
LIC_EXPORT ALM_VOID ALM_StClsLicFeatsInfo(INOUT ALM_LIC_FEATS_INFO_STRU *pstLicFeatsInfo);

/* ����License����BBOMֵ���� */
typedef struct {
    /* ��ΪALM_LIC_BBOM_FUNCTION���ͣ�ֻ��Ϊ0��1 */
    ALM_UINT64 ulValue;

    /* Bbom�� */
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;

    /* ���ֶε�����ֻӰ������ӿڵ����� */
    /* ���ϣ�����õ�Bbomֻ����Ĭ��״̬����������(RM201606076800)������Ҫ���ó�ALM_FALSE����������¶����ó�ALM_TRUE */
    ALM_BOOL bNormalEm;

    /* Bbom����ע�Ᵽ֤��'\0'��β */
    ALM_CHAR acBBomName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];
} ALM_SLIC_BBOM_STRU;

/* ���ڱ��������STICK���ü�����״̬��Ϣ */
typedef struct {
    /* ���������� */
    ALM_BOOL bIsRun;

    /* ���������Ĵ������������ */
    ALM_UINT32 uiStartTotal;

    /* ������������������ǽ�������Ҫ��ע���ֶ� */
    ALM_UINT32 uiFreeStartCount;

    /* ���������� �������ĵ��ܴ������������ */
    ALM_UINT32 uiAlreadyStartCount;

    /* ʣ��ʱ�䣬��λΪ���� */
    /* ����״̬�Զ�ˢ�´���ʱ�������˴����ݲ�����ȷ */
    /* ��δ���ù̶�����ˢ�£������ݿ��Ծ�ȷ��Сʱ�����鳬��һСʱ������һСʱ���� */
    /* ��������ÿ��̶�����ˢ�£������ݽ��ܾ�ȷ���죬���鳬��һ�첿����һ����� */
    ALM_UINT32 uiRemainMins;
    ALM_UINT32 uiTotalMins;
    ALM_UINT32 uiValidPassMins;
    ALM_CHAR acEnterTime[ALM_CONST_DATE_LEN + 1];

    /* ���õ�BBOM�б� */
    ALM_SLIC_BBOM_STRU *pstBboms;

    /* ���õ�BBOM��Ŀ */
    ALM_UINT32 uiBbomNum;

    /* �ýṹ���Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_SLIC_INFO_STRU;

/* ���ڳ�ʼ����ѯ�����ALM_SLIC_INFO_STRU���� */
LIC_EXPORT ALM_VOID ALM_StInitSlicInfo(INOUT ALM_SLIC_INFO_STRU *pstInfo);

/* ���������ѯ�õ���ALM_SLIC_INFO_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsSlicInfo(INOUT ALM_SLIC_INFO_STRU *pstInfo);

/* ���ڱ���ESN=ANY/DEMO/>3����Ϣ */
typedef struct {
    /* ��ǰ�����LIC �ļ��Ƿ�Ϊ���������Esn + Any��DEMO �ļ� */
    ALM_BOOL bIsUsing;

    /* �Ƿ񼤻��ESN=ANY��License */
    ALM_BOOL bHadActived;

    /* ʣ������ */
    ALM_UINT32 uiRemainDays;

    /* ���������� */
    ALM_UINT32 uiRunningDays;

    /* ESN=ANY+DEMO+����3���license����Ч�����Ľ�ֹ���� */
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_ESN_ANY_INFO_STRU;

/************************************************************************
 * ����洢��Ʒ��Ϣ��ؽṹ��
 ************************************************************************/
/* ��Ʒ������Ϣ */
typedef struct {
    ALM_INT32 iPrdId;
    ALM_CHAR acPrdName[ALM_CONST_PRD_NAME_LEN_MAX + 1];

    /*
     * ��Ʒ�汾������У��License��ָ���汾�Ƿ�һ�£�����ʹ�ü�¼��
     * У��Licenseʱ��ֻ��Ҫ��Ʒ�汾��License�а汾��ͷ���ɡ�
     * ��ֵ�䶯�������ý�����Stick����ʹ�ô���Ϊ0��Ҳ�ỹԭESN ANY��
     * ��Ч�ڳ�3���ҷ����õĵ���License����ʹ������Ϊ0��
     */
    ALM_CHAR acPrdVer[ALM_CONST_PRD_VER_MAX + 1];
} ALM_PRD_BASE_STRU;

/* ��Ʒ��չ��Ϣ */
typedef struct {
    /* �Ƿ���ʱ�䰲ȫ�Լ���ʾ */
    ALM_BOOL bSafeTimeEnable;

    /* ����bbom����ʱ�Ƿ�ʹ����Сֵ��־ */
    ALM_BOOL bUseMinValInPartOver;

    /* �Ƿ����ù̶����� */
    ALM_BOOL bFixTimeSupport;

    /* �Ƿ�����ESN��汾��ƥ��ֻ����һ�� */
    ALM_BOOL bEsnVerMisOnce;

    /* �Ƿ����R5C01�ȽϽӿڵ�У����� */
    ALM_BOOL bForceCmpSupport;

    /* �Ƿ����R5C01У���Ҫ���� */
    ALM_BOOL bVerifyMinor;
} ALM_PRD_EXTEND_INFO_STRU;

/* ��ƷBBOM������Ϣ */
typedef struct {
    /* BBOM��ţ��������ظ� */
    ALM_INT32 iBbomId;

    /* ָ��BBOM���� */
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;

    /*
     * Ĭ��ֵ���ڵ�ǰδ�����κ�License��Ҳδ����������Sickʱ�����߸�BBOM������
     * ���Ѽ����License�С�Ҳû�����õ�������Stick��ʱ��BBOM����Ȩֵ����Ĭ��ֵ��
     * ��Ϊ������(ALM_LIC_BBOM_FUNCTION)��ֻ��ȡֵ0��1��
     */
    ALM_UINT64 ulDfltVal;

    /* �ڵ�ǰ��ȨֵС�ڸ�ֵʱ����Ȩֵ��Ϊ��ֵ��������warning�������־��ʾ�� */
    /* ��Ϊ0���ԡ���Ϊ������(ALM_LIC_BBOM_FUNCTION)��ֻ��ȡֵ0��1�� */
    ALM_UINT64 ulMinVal;

    /* �ڵ�ǰ��Ȩֵ���ڸ�ֵʱ����Ȩֵ��Ϊ��ֵ��������warning�������־��ʾ�� */
    /* ��Ϊ0���ԡ���Ϊ������(ALM_LIC_BBOM_FUNCTION)��ֻ��ȡֵ0��1�� */
    ALM_UINT64 ulMaxVal;

    /* BBOM���ƣ��������ظ� */
    ALM_CHAR acName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];
} ALM_PRD_BBOM_STRU;

/* �洢��Ʒ��BBOM���� */
typedef struct {
    /* ��Ʒ��š����Ƽ��汾 */
    ALM_PRD_BASE_STRU stBase;

    ALM_PRD_EXTEND_INFO_STRU stExInfo;

    /* ���ò�ƷBBOM��ָ��Ĭ��ֵ��������Ϊ�� */
    ALM_PRD_BBOM_STRU *pstBboms;

    /* ��ƷBBOM��Ŀ���������0 */
    ALM_UINT32 uiBbomNum;

    /* License��֤��Կ��C����ַ�������'\0'��ʾ��β����ΪNULL���ݲ�������Կ */
    ALM_CHAR *pcPrdKey;

    /* ��Ӹò�Ʒʱ�ķ���ֵ��ALM_OK��ʾ��ӳɹ� */
    ALM_INT32 iRet;
} ALM_PRD_STRU;

typedef struct {
    ALM_CHAR acGroup[ALM_CONST_LIC_BBOMGROUP_NAME_MAX + 1]; /* group���� */
    ALM_BOOL bOn; /* ON/OFF */
} ALM_GROUP_FIX_STATE_STRU;

typedef struct {
    ALM_UINT32 uiNum;                                              /* �����С */
    ALM_GROUP_FIX_STATE_STRU stArrays[ALM_CONST_FIXTIME_GROUPNUM]; /* ���� */
} ALM_GROUP_FIX_STATE_RSLT_STRU;

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
#endif /* ALM_INTF_DEF_H */
