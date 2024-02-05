/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: License�ļ������������á�����License����������STICK����.
 * Author: AdaptiveLM team
 * Create: 2015-07-14
 */
#ifndef ALM_INTF_LIC_MGR_H
#define ALM_INTF_LIC_MGR_H

#include "alm_intf_def.h"
#include "alm_intf_state_mgr.h"

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

/* ������ʧЧ�� */
typedef struct {
    ALM_LIC_RVK_TICKET_STRU *pstTickets;
    ALM_UINT32 uiNum;
    /* �ýṹ���Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_RVK_TICKETS_STRU;

/* ����ALM_LIC_RVK_TICKETS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StInitLicRvkTickets(INOUT ALM_LIC_RVK_TICKETS_STRU *pstTickets);

/* ����ALM_LIC_RVK_TICKETS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsLicRvkTickets(INOUT ALM_LIC_RVK_TICKETS_STRU *pstTickets);

/* ����License״̬�ȽϽ�� */
typedef struct {
    ALM_STATE_LIC_VERIFY_STRU stCnt;
    ALM_STATE_LIC_VERIFY_STRU stNew;
} ALM_LIC_CMP_STATE_STRU;

/* ����BBOM�ȽϽ����ֻ����״̬������Ȩֵ����ʣ��ʱ�䲻��ͬ��BBOM */
typedef struct {
    ALM_STATE_BBOM_STRU stCnt;
    ALM_STATE_BBOM_STRU stNew;
} ALM_LIC_CMP_BBOM_STRU;

/* ����SBOM�ȽϽ��������License�ļ��е�SBOM */
typedef struct {
    ALM_CHAR acName[ALM_CONST_LIC_SBOM_NAME_MAX + 1];
    ALM_CHAR acChsUnit[ALM_CONST_LIC_SBOM_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acEngUnit[ALM_CONST_LIC_SBOM_DESC_ENG_UNIT_MAX + 1];

    ALM_BOOL bCntInc;
    ALM_BOOL bNewInc;

    /* ͬ��SBOMֵ�ۼ� */
    ALM_UINT64 ulCntVal;
    ALM_UINT64 ulNewVal;

    /* ͬ��SBOM��ֹ����ȡ��С */
    ALM_CHAR acCntDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acNewDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_CMP_SBOM_STRU;

/* ����PackageItem�ȽϽ��������licensen�ļ��е�PackageItem */
typedef struct {
    ALM_CHAR acName[ALM_CONST_LIC_PACK_NAME_MAX + 1];
    ALM_CHAR acChsUnit[ALM_CONST_LIC_PACK_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acEngUnit[ALM_CONST_LIC_PACK_DESC_ENG_UNIT_MAX + 1];

    ALM_BOOL bCntInc;
    ALM_BOOL bNewInc;

    /* ͬ��PackageItemֵ�ۼ� */
    ALM_UINT64 ulCntVal;
    ALM_UINT64 ulNewVal;

    /* ͬ��PackageItem��ֹ����ȡ��С */
    ALM_CHAR acCntDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acNewDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_CMP_PACK_STRU;

/* �洢���BBOM�ȽϽ�� */
typedef struct {
    ALM_LIC_CMP_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_CMP_BBOMS_STRU;

/* �洢���SBOM�ȽϽ�� */
typedef struct {
    ALM_LIC_CMP_SBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_CMP_SBOMS_STRU;

/* �洢���Pack�ȽϽ�� */
typedef struct {
    ALM_LIC_CMP_PACK_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_CMP_PACKS_STRU;

/*
 * ����:
 * ����License�ȽϽ�����ȽϽ��Ϊֻ�����Ѽ���License��ָ��License���ָ����Ʒ���
 * ״̬��Ϣ�ıȽϡ�����Ĭ��ֵ�����ܽ�����Stick��Ӱ�졣
 */
typedef struct {
    /* License״̬�ȽϽ�� */
    ALM_LIC_CMP_STATE_STRU stLicCmp;

    /* BBOM�ȽϽ�� */
    ALM_LIC_CMP_BBOMS_STRU stBboms;

    /* SBBOM�ȽϽ�� */
    ALM_LIC_CMP_SBOMS_STRU stSboms;

    /* Pack�ȽϽ�� */
    ALM_LIC_CMP_PACKS_STRU stPacks;
    /* �ýṹ���Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_CMP_STRU;

/* ��ʼ��ALM_LIC_CMP_STRU����һ�����ڳ�ʼ��license compare��� */
LIC_EXPORT ALM_VOID ALM_StInitLicCmp(INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/* ����ALM_LIC_CMP_STRU����һ����������license compare��� */
LIC_EXPORT ALM_VOID ALM_StClsLicCmp(INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/************************************************************************
 * ������ʽLicense�����ü�����License)
 ************************************************************************/
/* ����License������ */
typedef struct {
    ALM_INT32 iRet;                         /* �õ�������ļ�ʱ�ķ���ֵ��ALM_OK��ʾ�ɹ� */
    ALM_BOOL bVerifyIsValid;                /* ���ڸ���ΪTRUEʱ��License��֤������в��ռ�ֵ */
    ALM_STATE_PRD_LIC_VERIFY_STRU stVerify; /* License����֤��� */
} ALM_LIC_ACTIVE_RSLT_STRU;

/* ���������License */
typedef struct {
    ALM_INT32 iPrdId;                /* ���ָ����Ʒ�ı�� */
    ALM_CHAR *pcLicStr;              /* License�ļ����� */
    ALM_UINT32 uiLicLen;             /* License�ļ����ݵĳ��� */
    ALM_LIC_ACTIVE_RSLT_STRU stRslt; /* �����License�ļ����� */
} ALM_LIC_ACTIVE_STRU;

/*
 * ����: ����License��ָ����Ʒ��һ���Լ�����Licenseֻ�ᷢ��һ�α��֪ͨ��
 * ����:
 * pstLics - ָ��������License�����õ���������
 * uiLicNum - ������License����Ŀ��
 * puiFailNum - ����ʧ�ܵ���Ŀ��
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralActive(INOUT ALM_LIC_ACTIVE_STRU *pstLics, IN ALM_UINT32 uiLicNum,
                                          OUT ALM_UINT32 *puiFailNum);

/* ��ѯ��ǰ�Ѽ���򱻷������License�ļ����ݣ�ע��������� */
LIC_EXPORT ALM_INT32 ALM_LicGeneralQuery(IN ALM_INT32 iPrdId, INOUT ALM_LIC_STORE_STRU *pstLic,
                                         OUT ALM_LIC_ATTRI_STRU *pstLicAttri);

/**
 * @ingroup AdaptiveLM
 * @brief ���ݵ�ǰlicense״̬��ѯ��ǰ�Ѽ����ȥ�����License�ļ����ݡ�
 *
 * @par ����: ��ALM_LicGeneralQuery���ƣ����ص�SBOM��ֵ��SBOM��BBOM��ֵ��BBOM��ֵ��PACKAGE��ֵ����License״̬��ʱ��仯��
 *
 * @attention ����pstLic��ʹ��ǰע���ʼ����ALM_StInitLicStore��ʹ�ú�ע�����������ALM_StClsLicStore��
 *
 * @li Memory operation: alloc��free��size:
 * -# �ýӿڻᶯ̬�������pstLic�ڴ棬ʹ��ǰע���ʼ����ALM_StInitLicStore��ʹ�ú�ע�����������ALM_StClsLicStore��
 * @li Thread safe:
 * -# �̰߳�ȫ����������߶���������</br>
 * @li OS/CPU difference:
 * -# ��OS����</br>
 * @li Time consuming:
 * -# �Ǻ�ʱ�ӿ�</br>
 *
 * @param iPrdId [IN] ��������#ALM_INT32��ָ����Ʒ��š�
 * @param pstLic [INOUT] ��������#ALM_LIC_STORE_STRU *����ѯ�õ���License�ļ������ݡ�
 * @param pstLicAttri [OUT] ��������#ALM_LIC_ATTRI_STRU *����ѯ�õ���License�ļ����������ԡ�
 *
 * @retval #ALM_OK 0 �ɹ���
 * @retval #��������ֵ ʧ�� - ��Ӧ���������API�ֲᡣ
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralQueryByStatus(IN ALM_INT32 iPrdId, INOUT ALM_LIC_STORE_STRU *pstLic,
                                                 OUT ALM_LIC_ATTRI_STRU *pstLicAttri);

/* ����License�ļ� */
LIC_EXPORT ALM_INT32 ALM_LicGeneralParser(IN ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                          INOUT ALM_LIC_STORE_STRU *pstLic,
                                          INOUT ALM_LIC_FEATS_INFO_STRU *pstLicFeatsInfo);

/* �����Ƿ�����License��С,
 * ALMAdaptiveLM���Ĭ������LicenseС��2M, ����ͨ�����API��ALM_FALSE��������ȡ��.
 * ��api��Ҫ��ALM_Startǰ������ */
LIC_EXPORT ALM_INT32 ALM_LicSetMaxLengthSwitch(IN ALM_BOOL bTurnOn);

/* ����License�ļ��������ڲ���Ҫ��Ʒ����Ĳ�Ʒ�����ӿڲ�������ע��������� */
LIC_EXPORT ALM_INT32 ALM_LicParser(CONST ALM_CHAR *pcFileBuf, ALM_UINT32 uiFileLen, CONST ALM_CHAR *pcKey,
                                   ALM_UINT32 uiKeyLen, OUT ALM_LIC_STORE_STRU *pstLicStore);

/* Bpart�滮��/ʹ����ת��ΪSpart�滮��/ʹ���� */
LIC_EXPORT ALM_INT32 ALM_BpartToSpart(ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcBpartName, IN CONST ALM_CHAR *pcSpartName,
                                      ALM_UINT64 ulBpartPlanVal, ALM_UINT64 ulBpartUsageVal,
                                      OUT ALM_S2B_SPART_RSLT_STRU *pstSpartRslt);

/* ��������ALM_S2B_SPART_RSLT_STRU���� */
LIC_EXPORT ALM_VOID ALM_FreeS2bSpartRslt(IN ALM_S2B_SPART_RSLT_STRU *pstSpartRslt);

/* SPart-BPartת������ */
typedef struct {
    ALM_CHAR acName[ALM_CONST_LIC_NAME_MAX + 1];
    ALM_UINT64 ulAmount;
    double dAmount;
} ALM_B2S_ITEM_INFO_STRU;

/* ����Spart��Ϣ: ��Spart�����ֺ���������Spart��ת��ϵ�����Լ�Spart�µ�Bpart�б� */
typedef struct {
    ALM_B2S_ITEM_INFO_STRU stSpart;

    /* ��Spart���ж��ٸ�Bpart */
    ALM_UINT32 uiNum;
    /* Bpart�б� */
    ALM_B2S_ITEM_INFO_STRU *pBpartArray;
} ALM_B2S_SPART_INFO_STRU;

/* ת����� */
typedef struct {
    /* û�м�Ȩת��ϵ�� */
    ALM_B2S_ITEM_INFO_STRU stItem;
    /* ��Ȩת��ϵ�� */
    ALM_B2S_ITEM_INFO_STRU stItemRadio;
} ALM_B2S_CONVERTION_RST_STRU;

typedef struct {
    ALM_B2S_CONVERTION_RST_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* ���ֶα�ʾ�Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_B2S_CONVERTION_RSTS_STRU;

/*
 * ����: ����Bpart-Spartװ����
 * ����: 3�����������1���������
 * pBpart - ָ��Bpart���ݣ�����Bpart�����ֺ������������Ը���������
 * pSbRelation - Spart��Ϣ��
 * uiNum - ������ٸ�Spart��
 * pRsts - ���������Bpart���䵽����ЩSpart�ϣ���
 */
LIC_EXPORT ALM_INT32 ALM_B2sConvertion(IN CONST ALM_B2S_ITEM_INFO_STRU *pBpart,
                                       IN CONST ALM_B2S_SPART_INFO_STRU *pSpartArray,
                                       IN CONST ALM_UINT32 uiNum,
                                       ALM_B2S_CONVERTION_RSTS_STRU *pRsts);

LIC_EXPORT ALM_VOID ALM_StInitB2sConvertionRsts(INOUT ALM_B2S_CONVERTION_RSTS_STRU *pRsts);

LIC_EXPORT ALM_VOID ALM_StClsB2sConvertionRsts(INOUT ALM_B2S_CONVERTION_RSTS_STRU *pRsts);


/*
 * ����: �Ƚ�ָ��License�ļ��뵱ǰ�Ѽ���License�ļ���
 * ��������ֻ���Ƕ�Ӧ�Ĳ�Ʒ������������license�����Ȩֵ��״̬�ıȽϡ�
 * �����Ե�ǰ�Ľ�����Stick��״̬����
 * ����:
 * iPrdId - ��Ʒ��š�
 * pcLicStr - �µ�License�ļ���
 * uiLicLen - License�ļ����ȡ�
 * pstCmpRslt - �õ��ȽϽ����ע�����������
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralCmp(IN ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                       INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/**
 * @ingroup AdaptiveLM
 * @brief ����ESN=ANY��������ʱ�䣬�Ƚ�ָ��License�ļ��뵱ǰ�Ѽ���License�ļ���
 *
 * @par ����: ͬALM_LicGeneralCmp���ܡ����License ESN=ANY�������ESN=ANY��������ʱ�䡣
 *
 * @attention ����pstCmpRslt��ʹ��ǰע���ʼ����ALM_StInitLicCmp��ʹ�ú�ע�����������ALM_StClsLicCmp��
 *
 * @li Memory operation: alloc��free��size:
 * -# �ýӿڻᶯ̬�������pstCmpRslt�ڴ棬ʹ��ǰע���ʼ����ALM_StInitLicCmp��ʹ�ú�ע�����������ALM_StClsLicCmp��
 * @li Thread safe:
 * -# �̰߳�ȫ����������߶���������</br>
 * @li OS/CPU difference:
 * -# ��OS����</br>
 * @li Time consuming:
 * -# �Ǻ�ʱ�ӿ�</br>
 *
 * @param iPrdId [IN] ��������#ALM_INT32��ָ����Ʒ��š�
 * @param pcLicStr [IN] ��������#CONST ALM_CHAR *��License�ļ�����, ��Ҫ����C�����ַ���������'\0����
 * @param uiLicLen [IN] ��������#ALM_UINT32��License�ļ����ȡ�
 * @param pstCmpRslt [INOUT] ��������#ALM_LIC_CMP_STRU *���õ��ıȽϽ����
 *
 * @retval #ALM_OK 0 �ɹ���
 * @retval #��������ֵ ʧ�� - ��Ӧ���������API�ֲᡣ
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralCmpIgnoreEsnAny(IN ALM_INT32 iPrdId,
                                                   IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                                   INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/* Ԥ����License���õ�License״̬ */
LIC_EXPORT ALM_INT32 ALM_LicGeneralVerify(IN ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                          OUT ALM_LIC_VERIFY_STRU *pstRslt);

/*
 * ����: ��License�ļ�ִ��ʧЧ������ǰLicense�ļ��Ѿ���ʧЧ�����õ�ʧЧ�벻ˢ��״̬��
 * ����
 * 1. iPrdId - ��ҪʧЧ��ǰ�Ѽ���License�Ĳ�Ʒ��
 * 2. pstRvkcTiket - �õ���ʧЧ�롣
 * 3. pbHaveRvked - ִ��ʧЧǰ���ѱ�ʧЧ�õ���ֵΪ�档
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralRevoke(IN ALM_INT32 iPrdId, OUT ALM_LIC_RVK_TICKET_STRU *pstRvkcTiket,
                                          OUT ALM_BOOL *pbHaveRvked);

/* ��ѯ�ò�Ʒ��ȫ��ʧЧ�룬ע��������� */
LIC_EXPORT ALM_INT32 ALM_LicGeneralQueryAllRevoke(IN ALM_INT32 iPrdId, INOUT ALM_LIC_RVK_TICKETS_STRU *pstRvkcTiket);

/*
 * ����: ����ָ����Ʒ��ָ��lsn��ʧЧ��
 * ����:
 * 1. iPrdId - ��ҪʧЧ��Ĳ�Ʒ��
 * 2. pstLsnRvkTicket - ������ҪʧЧ���license��lsn �����ʧЧ�롣
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralGetRvkTkt(ALM_INT32 iPrdId, INOUT ALM_LIC_LSN_RVK_TICKET_STRU *pstLsnRvkTicket);

/*
 * ����: ����ָ��keyfile��ָ��lsn��ʧЧ��,��������
 * ����:
 * 1. pcKey - keyfile��
 * 2. pstLsnRvkTicket - ������ҪʧЧ���license��lsn �����ʧЧ�롣
 */
LIC_EXPORT ALM_INT32 ALM_LicGetRvkTkt(CONST ALM_CHAR *pcKey, INOUT ALM_LIC_LSN_RVK_TICKET_STRU *pstLsnRvkTicket);

/************************************************************************
 * �������
 ************************************************************************/
/* ����������� */
typedef struct {
    /* ָ��֧�ֽ�����BBOM�������µ���Ȩֵ */
    /* ��ΪNULL���������������������������Ѿ����ù����� */
    ALM_SLIC_BBOM_STRU *pstBboms;

    /* ָ��֧�ֽ�����BBOM����Ŀ */
    /* ��Ϊ0���������������������������Ѿ����ù����� */
    ALM_UINT32 uiNum;

    /* �����������������ÿ����������7�죬���7�� */
    /* �����깺������󽫻�ʹ����Ѵ��� */
    ALM_UINT32 uiBuyCount;
} ALM_LIC_EM_CFG_STRU;

/* ���ý��� */
LIC_EXPORT ALM_INT32 ALM_LicEmCfg(IN ALM_INT32 iPrdId, IN CONST ALM_LIC_EM_CFG_STRU *pstCfg);

/*
 * ����������
 * ��֮ǰ�Ѿ���������û�е��ûָ������ָ����У����ָ�����������������������
 * �ָ�����ʱ���������Ѿ����ڣ����ٴ������������µ����ڡ�
 */
LIC_EXPORT ALM_INT32 ALM_LicEmStart(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pbIsRecover);

/* ֹͣ���� */
LIC_EXPORT ALM_INT32 ALM_LicEmStop(IN ALM_INT32 iPrdId);

/* ��ѯ�������ü�������Ϣ��ע��������� */
LIC_EXPORT ALM_INT32 ALM_LicEmQuery(IN ALM_INT32 iPrdId, INOUT ALM_SLIC_INFO_STRU *pstRslt);

/************************************************************************
 * ����Stick
 ************************************************************************/
/* Stick���� */
typedef struct {
    /* ָ��֧��Stick��BBOM��Stick�µ���Ȩֵ������ΪNULL */
    ALM_SLIC_BBOM_STRU *pstBboms;

    /* ָ��֧��Stick��BBOM����Ŀ���������0 */
    ALM_UINT32 uiNum;

    /* ÿ�������������е�������Ĭ��30�죬���180�죬��Ϊ0����Ĭ�� */
    ALM_UINT32 uiAllowDays;

    /* ���������Ĵ�����Ĭ��2�Σ����2�Σ���Ϊ0����Ĭ�� */
    ALM_UINT32 uiAllowCount;
} ALM_LIC_STICK_CFG_STRU;

/* ����Stick */
LIC_EXPORT ALM_INT32 ALM_LicStickCfg(IN ALM_INT32 iPrdId, IN CONST ALM_LIC_STICK_CFG_STRU *pstCfg);

/*
 * ����Stick��
 * ��֮ǰ�Ѿ���������û�е��ûָ������ָ����У����ָ�Stick������������������
 * �ָ�����ʱ���������Ѿ����ڣ����ٴ������������µ����ڡ�
 */
LIC_EXPORT ALM_INT32 ALM_LicStickStart(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pbIsRecover);

/* ֹͣStick */
LIC_EXPORT ALM_INT32 ALM_LicStickStop(IN ALM_INT32 iPrdId);

/* ��ѯStick���ü�������Ϣ��ע��������� */
LIC_EXPORT ALM_INT32 ALM_LicStickQuery(IN ALM_INT32 iPrdId, INOUT ALM_SLIC_INFO_STRU *pstRslt);

/* ��ѯESN=ANY/DEMO/>3����Ϣ */
LIC_EXPORT ALM_INT32 ALM_LicEsnAnyQuery(IN ALM_INT32 iPrdId, OUT ALM_LIC_ESN_ANY_INFO_STRU *pstEsnAnyInfo);

/*
 * ������ʱ���Ƿ���Ч��
 */
LIC_EXPORT ALM_INT32 ALM_LicSetFixTimeSwitch(IN ALM_INT32 iPrdId, IN ALM_BOOL bFixTime);

LIC_EXPORT ALM_INT32 ALM_LicSetFixTimeGroupSwitch(IN ALM_INT32 iPrdId, ALM_CHAR *pcGroup, ALM_BOOL bFixTimeState);

/*
 * ��ȡ��ʱ���Ƿ���Ч��
 */
LIC_EXPORT ALM_INT32 ALM_LicIsEnableFixTimeItem(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pbOn);
LIC_EXPORT ALM_INT32 ALM_LicIsEnableFixTimeGroupItem(IN ALM_INT32 iPrdId,
                                                     ALM_GROUP_FIX_STATE_RSLT_STRU *pstFixGroupState);

typedef struct {
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eType;
    ALM_UINT32 uiFeatureId;
    ALM_CHAR acFeatureName[ALM_CONST_LIC_GROUP_NAME_MAX + 1];
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiGraceDays;
} ALM_LIC_FEATURE_STRU;

typedef struct {
    ALM_LIC_FEATURE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* ���ֶα�ʾ�Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_FEATURES_STRU;

LIC_EXPORT ALM_INT32 ALM_LicGetAllFeature(ALM_INT32 iPrdId, OUT ALM_LIC_FEATURES_STRU *pstFeatures);
LIC_EXPORT ALM_VOID ALM_StInitFeatures(INOUT ALM_LIC_FEATURES_STRU *pstFeatures);
LIC_EXPORT ALM_VOID ALM_StClsFeatures(INOUT ALM_LIC_FEATURES_STRU *pstFeatures);

typedef struct {
    ALM_INT32 iPrdId;
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eLicType;
} ALM_LIC_ACTIVEDLIC_INFO_STRU;

typedef struct {
    ALM_LIC_ACTIVEDLIC_INFO_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* ���ֶα�ʾ�Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_ACTIVEDLIC_INFOS_STRU;

LIC_EXPORT ALM_VOID ALM_StInitActivedLicInfos(INOUT ALM_LIC_ACTIVEDLIC_INFOS_STRU *pstInfos);

LIC_EXPORT ALM_VOID ALM_StClsActivedLicInfos(INOUT ALM_LIC_ACTIVEDLIC_INFOS_STRU *pstInfos);

LIC_EXPORT ALM_INT32 ALM_LicGetActivedLicInfos(ALM_INT32 iPrdId, OUT ALM_LIC_ACTIVEDLIC_INFOS_STRU *pstInfos);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
#endif /* ALM_INTF_LIC_MGR_H */
