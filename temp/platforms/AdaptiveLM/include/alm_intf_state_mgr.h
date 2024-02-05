/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: ����洢״̬��Ϣ�Ľṹ�壬�Լ�״̬У��������á�
 * Author: AdaptiveLM team
 * Create: 2015-07-14
 */
#ifndef ALM_INTF_STATE_MGR_H
#define ALM_INTF_STATE_MGR_H

#include "alm_intf_def.h"

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
 * ����״̬����ز�����
 ************************************************************************/
/**
 * ����: License�ļ�״̬��������״̬��
 * ע��: ���������״̬���޼��йأ���Ч>����>��Ч��
 * ��Ա:
 * 1. ALM_STATE_LIC_INVALID - ��Ч��
 * 2. ALM_STATE_LIC_GRACE_PERIOD - ���ޡ�
 * 3. ALM_STATE_LIC_VALID - ��Ч��
 */
typedef enum {
    ALM_STATE_LIC_INVALID = 0,
    ALM_STATE_LIC_GRACE_PERIOD,
    ALM_STATE_LIC_VALID,
} ALM_STATE_LIC_ENUM;

/**
 * ����: ��Ʒ״̬��
 * ��Ա:
 * ALM_STATE_PRD_INVALID - û���κ���Ȩʱ״̬��������Ĭ����Ȩ������ִ�״̬��
 * ALM_STATE_PRD_DEFAULT - û�м����κ�License�ļ�����Ȩʱ״̬��
 * ALM_STATE_PRD_GRACE_PERIOD - License�ļ�����ֹ������һ��ʱ��Ŀ����ڡ�
 * ALM_STATE_PRD_NORMAL - ��ʽ����License�ļ�����ʹ��ʱ״̬��
 * ALM_STATE_PRD_EMERGENCY - ���ý���ʱ״̬��
 * ALM_STATE_PRD_DEMO - ����License����ʹ��ʱ״̬��
 * ALM_STATE_PRD_STICK - ����Stickʱ״̬��
 */
typedef enum {
    ALM_STATE_PRD_INVALID = 0,
    ALM_STATE_PRD_DEFAULT,
    ALM_STATE_PRD_GRACE_PERIOD,
    ALM_STATE_PRD_NORMAL,
    ALM_STATE_PRD_EMERGENCY,
    ALM_STATE_PRD_DEMO,
    ALM_STATE_PRD_STICK
} ALM_STATE_PRD_ENUM;

/**
 * ����: ��Ҫ����
 *
 */
typedef enum {
    ALM_MINOR_LK_ITEM_DEADLINE_OVER = 1,
    ALM_MINOR_LK_ITEM_TRIAL_OVER = 1 << 1,
    ALM_MINOR_LK_ITEM_DEADLINE_LESSER = 1 << 2,
    ALM_MINOR_LK_ITEM_VALUE_LESSER = 1 << 3,
    ALM_MINOR_LK_ITEM_NOT_PRESENT = 1 << 4,
    ALM_MINOR_LK_ITEM_CONFIG_MORE = 1 << 5,
    ALM_MINOR_LK_ITEM_USED_MORE = 1 << 6,
    ALM_MINOR_LK_ITEM_LESSER_THAN_DEFAULT = 1 << 7,
    ALM_MINOR_LK_ITEM_LESSER_THAN_MINIMUM = 1 << 8,
    ALM_MINOR_LK_ITEM_MORE_THAN_MAXIMUM = 1 << 9,
    ALM_MINOR_STICK_ITEM_NOT_FOUND = 1 << 10,
    /* ���ֹ��� */
    ALM_MINOR_LK_ITEM_PARTIAL_DEADLINE_OVER = 1 << 11,
    /* ���ֹ������� */
    ALM_MINOR_LK_ITEM_PARTIAL_TRIAL_OVER = 1 << 12
} ALM_MINOR_ENUM;

/*
 * ����: �洢license��bbom״̬��Ϣ����������������ʱ����ָStart��Stopǰ��
 * ����ʱ�䣬�Ҳ��ܵ�ǰʱ�������Ӱ��(�ɶ�ʱ�����ڼ�ʱ)��
 * ��Ա:
 * eState - ��ǰ״̬��
 * ulStateRsn - ��ǰ״̬��ԭ��
 * uiTotalMins - ��ǰ״̬������ʱ�俪ʼ���㣬�������е�ʱ�䡣
 *   ����ǰ״̬Ϊ����(bPerm��ԱֵΪTRUE)����ֵΪ0��
 *   ����ǰ״̬�������ã�����ʱ�� + ��ֵ�����Եõ�Ԥ�ڵĽ�ֹʱ�䡣
 *   �����ڼ䵽��Ԥ�ڵĽ�ֹʱ�䲢��һ��ֹͣ����Ҫ����ˢ�¡�
 * uiLeftMins - ��ǰ״̬ʣ��ʱ�䡣�����ڼ�״̬����ʵʱˢ�£���ֵ������ȷ��
 *   ��Ϊ����(bPerm��ԱֵΪTRUE)����ֵΪ0��
 * uiValidPassMins - ��Ч��������ʱ�䡣
 * uiGracePassMins - ������������ʱ�䡣
 * bPerm - ��ǰ״̬�Ƿ�Ϊ���ã���Ϊ����uiTotalMins��uiPassMins��uiLeftMins��Ч��
 * acEnterTime - ��ǰ״̬����ʱ�䡣��ʼ����License����License���֪ͨ��
 *   License֮ǰ״̬�У���ֵΪ���ַ�����
 */
typedef struct {
    ALM_UINT64 ulStateRsn;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eState;

    ALM_UINT32 uiTotalMins;
    ALM_UINT32 uiLeftMins;
    ALM_UINT32 uiValidPassMins;
    ALM_UINT32 uiGracePassMins;
    ALM_BOOL bPerm;
    ALM_UINT32 uiReserved;

    ALM_CHAR acEnterTime[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acRvkTime[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acEnterTrialTime[ALM_CONST_DATE_LEN + 1];
} ALM_STATE_STRU;

/*
 * ����: �洢��Ʒ����ƷBBOM״̬��Ϣ��
 * ��Ա:
 * eState - ��ǰ״̬��
 * acEnterTime - ��ǰ״̬����ʱ�䡣
 * bPerm - ��ǰ״̬�Ƿ�Ϊ���á�
 * uiLeftMins - ��ǰ״̬ʣ��ʱ�䡣�����ڼ�״̬���²���ʵʱ����ֵ������ȷ��
 * acLsn - ��ǰ��Ȩ����License�ļ���
 */
typedef struct {
    ALM_ENUM(ALM_STATE_PRD_ENUM) eState;
    ALM_CHAR acEnterTime[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiLeftMins;
    ALM_BOOL bPerm;
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
} ALM_STATE_PRD_BASE_STRU;

/*
 * ����: ���ڴ洢������״̬��Ϣ��
 * ��Ա:
 * iPrdId - ��Ʒ��š�
 * uiAnyDemoRunMins - ESN ANY/��Ч�ڴ���3��ĵ���License��ʹ�÷�������
 * uiAnyDemoTotalMins - ESN ANY/��Ч�ڴ���3��ĵ���License�ܷ�������
 * stBase - ��ǰ״̬��
 */
typedef struct {
    ALM_INT32 iPrdId;
    ALM_UINT32 uiAnyDemoRunMins;
    ALM_UINT32 uiAnyDemoTotalMins;
    ALM_STATE_PRD_BASE_STRU stBase;
} ALM_STATE_PRD_STRU;

/*
 * ����: �洢��Ʒע���BBOM��״̬��Ϣ������ע����Ϣ��
 * ��Ա:
 * iBbomId - BBOM��š�
 * acBbomName - BBOM����
 * eType - BBOM���͡�
 * ulMinVal - ע��ʱָ������Сֵ��
 * ulMaxVal - ע��ʱָ�������ֵ��
 * ulVal - ��ǰ��Ȩֵ��
 * stBase - BBOM����״̬��Ϣ��
 */
typedef struct {
    ALM_INT32 iBbomId;
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;
    ALM_UINT64 ulMinVal;
    ALM_UINT64 ulMaxVal;
    ALM_UINT64 ulVal;
    ALM_UINT64 ulPermVal;
    ALM_UINT64 ulTempVal;
    ALM_STATE_PRD_BASE_STRU stBase;
    ALM_CHAR acBbomName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];
} ALM_STATE_BBOM_STRU;

/*
 * ����: �洢���BBOM��״̬��Ϣ��
 * ��Ա:
 * pstArray - �洢���BBOM��״̬��Ϣ�����顣
 * uiNum - BBOM������ʵ���Ѵ洢BBOM��Ŀ��
 * uiCap - BBOM���鵱ǰ�����������Ʒ�������ù�ע�˳�Ա��
 */
typedef struct {
    ALM_STATE_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* �ýṹ���Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_STATE_BBOMS_STRU;

/* ���ڳ�ʼ����ѯ�����ALM_STATE_BBOMS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StInitStateBboms(INOUT ALM_STATE_BBOMS_STRU *pstBBoms);

/* ����ALM_STATE_BBOMS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsStateBboms(INOUT ALM_STATE_BBOMS_STRU *pstBBoms);

/*
 * ����: LicenseУ����Ϣ��
 * bEsnMis - License�ļ��Ƿ��뻷��ESN��һ�¡�
 * bPrdNameMis - License�ļ��Ƿ����Ʒ����һ�¡�
 * bPrdVerMis - License�ļ��Ƿ����Ʒ�汾��һ�¡�
 * bRvked - License�ļ��Ƿ��Ѿ���ʧЧ��
 * bValidOver - ��Ч���Ƿ���������ο���ֹ���ڣ���Ч��δ����Ҳ�п��ܽ�����Ч״̬��
 * bGraceOver - �������Ƿ�������ο�����������ʱ�䡢����ʱ�䡢����ʹ�õ��������㡣
 */
typedef struct {
    ALM_BOOL bEsnMis;
    ALM_BOOL bPrdNameMis;
    ALM_BOOL bPrdVerMis;
    ALM_BOOL bRvked;
    ALM_BOOL bValidOver;
    ALM_BOOL bGraceOver;
} ALM_STATE_PRD_LIC_VERIFY_STRU;

/*
 * ����: ����License״̬��Ϣ��
 * stBase - License�ļ��ĵ�ǰ״̬��
 * stVerify - License�ļ��Ƿ��뻷��ESN��һ�¡�
 * bUsing - License�ļ��Ƿ񱻼����δ������ṹ������Ա���ݣ���֮ǰ�������
 *   ����Ϊֹͣǰ��״̬������Ϊ��ֵ(invalid״̬������ʱ��Ϊ���ַ���������ֵΪ0)��
 */
typedef struct {
    ALM_STATE_STRU stBase;
    ALM_STATE_PRD_LIC_VERIFY_STRU stVerify;
    ALM_BOOL bUsing;
} ALM_STATE_LIC_STRU;

typedef struct {
    ALM_INT32 iBbomId;
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;
    ALM_CHAR acBbomName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 iMinor;
    ALM_UINT64 ulLkVal;
    ALM_BOOL bValidOver;
    ALM_BOOL bGraceOver;
    ALM_UINT32 uiReserved;
} ALM_LIC_VERIFY_BBOM_STRU;

typedef struct {
    ALM_LIC_VERIFY_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_VERIFY_BBOMS_STRU;

typedef struct {
    ALM_CHAR acOfferingName[ALM_CONST_LIC_OFFERING_NAME_MAX + 1];
    ALM_CHAR acSbomName[ALM_CONST_LIC_SBOM_NAME_MAX + 1];
    ALM_CHAR acSbomChsUnit[ALM_CONST_LIC_SBOM_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acSbomEngUnit[ALM_CONST_LIC_SBOM_DESC_ENG_UNIT_MAX + 1];
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT64 ulLkVal;
    ALM_BOOL bValidOver;
    ALM_BOOL bGraceOver;
    ALM_UINT32 uiReserved;
} ALM_LIC_VERIFY_SBOM_STRU;

typedef struct {
    ALM_CHAR acOfferingName[ALM_CONST_LIC_OFFERING_NAME_MAX + 1];
    ALM_CHAR acPackItemName[ALM_CONST_LIC_PACK_NAME_MAX + 1];
    ALM_CHAR acPackItemChsUnit[ALM_CONST_LIC_PACK_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acPackItemEngUnit[ALM_CONST_LIC_PACK_DESC_ENG_UNIT_MAX + 1];
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT64 ulLkVal;
    ALM_BOOL bValidOver;
    ALM_BOOL bGraceOver;
    ALM_UINT32 uiReserved;
} ALM_LIC_VERIFY_PACK_STRU;

typedef struct {
    ALM_LIC_VERIFY_PACK_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_VERIFY_PACKS_STRU;

typedef struct {
    ALM_LIC_VERIFY_SBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_VERIFY_SBOMS_STRU;

typedef struct {
    ALM_STATE_STRU stBase;
    ALM_STATE_PRD_LIC_VERIFY_STRU stVerify;
} ALM_STATE_LIC_VERIFY_STRU;

typedef struct {
    ALM_STATE_LIC_VERIFY_STRU stLicState;
    ALM_LIC_VERIFY_BBOMS_STRU stBboms;
    ALM_LIC_VERIFY_SBOMS_STRU stSboms;
    ALM_LIC_VERIFY_PACKS_STRU stPacks;
    ALM_UINT32 uiInitFlag;
} ALM_LIC_VERIFY_STRU;

/* ���ڳ�ʼ����ѯ�����ALM_LIC_VERIFY_STRU���� */
LIC_EXPORT ALM_VOID ALM_StInitLicVerify(INOUT ALM_LIC_VERIFY_STRU *pstVerify);

/* ��������ALM_LIC_VERIFY_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsLicVerify(INOUT ALM_LIC_VERIFY_STRU *pstVerify);

/* �洢�����Ʒ״̬ */
typedef struct {
    ALM_STATE_PRD_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* �ýṹ���Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_STATE_PRDS_STRU;

/* bbom����ֵ��ʹ��ֵ */
typedef struct {
    ALM_INT32 iBbomId;
    ALM_UINT64 ulItemConfVal;
    ALM_UINT64 ulItemUsedVal;
} ALM_ITEM_CONF_USED_INFO_STRU;

/* ���ڳ�ʼ����ѯ�����ALM_STATE_PRDS_STRUT���� */
LIC_EXPORT ALM_VOID ALM_StInitStatePrds(INOUT ALM_STATE_PRDS_STRU *pstPrds);

/* ��������ALM_STATE_PRDS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsStatePrds(INOUT ALM_STATE_PRDS_STRU *pstPrds);

/* ��ѯ��Ʒ״̬ */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryPrd(IN ALM_INT32 iPrdId, OUT ALM_STATE_PRD_STRU *pstState);

/* ��ѯ�õ�ȫ����Ʒ״̬��ע��������� */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryAllPrd(INOUT ALM_STATE_PRDS_STRU *pstState);

/* ��ѯָ����Ʒ��ǰ�Ѽ���License״̬��ԭ�� */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryLic(IN ALM_INT32 iPrdId, OUT ALM_STATE_LIC_VERIFY_STRU *pstLicState);

/* ��ѯָ����Ʒȫ��BBOM״̬����Ȩֵ��ע��������� */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryBboms(IN ALM_INT32 iPrdId, INOUT ALM_STATE_BBOMS_STRU *pstBboms);

/*
 * ����: ��ѯָ����Ʒָ��BBOM״̬����Ȩֵ��
 * ����
 * iPrdId - ��Ʒ��š�
 * iBbomId - ��Ҫ��ѯ��BBOM�ı�ţ�����δָ��BBOM��ʱ��Ч��
 * pcBbomName - ��Ҫ��ѯ��BBOM�����ƣ���ΪNULL���޶����ơ�
 * pstBbom - ��ѯ�õ���BBOM��Ȩ��״̬��Ϣ��
 * pbExist - ָ����BBOM�Ƿ���ڣ��������ڲ�ѯ�����Ч��
 */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryBbom(IN ALM_INT32 iPrdId, IN ALM_INT32 iBbomId, IN CONST ALM_CHAR *pcBbomName,
                                           OUT ALM_STATE_BBOM_STRU *pstBbom, OUT ALM_BOOL *pbExist);

/************************************************************************
 * ���������ڼ��Զ�У�顣
 ************************************************************************/
/* ״̬����ģ��������� */
typedef struct {
    /* �Ƿ�����ˢ�µ�����TRUE-���ƣ�Ĭ��TRUE */
    /* �������Ƴ�ԱucAutoRefreshAtHourֵ��Ч */
    ALM_BOOL bFixRefreshHour;

    /**
     * ����ˢ��״̬�ĵ�������Χ[0, 23]��Ĭ��ALM_CONST_STATE_REFRESH_DFLT_HOUR(2)��
     * ��ָ��Ϊ�̶���ˢ�£�״̬����ʱ�������ڵ�ָ��������������ܵ��¿��ޡ�
     * ������Stick������ʱ�䳬�������������
     */
    ALM_UINT8 ucAutoRefreshAtHour;

    /**
     * ÿ����ô���һ��״̬У�飬��Χ[1, 60]���ӡ�
     * Ĭ��ALM_CONST_STATE_REFRESH_DFLT_INTERVAL(30)��
     * ��ָ��У����������Ƕ�ʱ�����ȣ���ֵ���С��60��ȥ��ʱ��һ
     * Сʱ��ʵ��һСʱ�����ʵ��ƫ�ơ�
     */
    ALM_UINT8 ucAutoRefreshInterval;
} ALM_STATE_MGR_CFG_STRU;

/* ���ö�ʱ�Զ�У�� */
LIC_EXPORT ALM_INT32 ALM_StateMgrCfgAutoRefresh(IN CONST ALM_STATE_MGR_CFG_STRU *pstCfg);

/* �õ���ʱ�Զ�У�������ֵ */
LIC_EXPORT ALM_INT32 ALM_StateMgrGetAutoRefreshCfg(OUT ALM_STATE_MGR_CFG_STRU *pstCfg);

/************************************************************************
 * ����У����
 ************************************************************************/
/* �������õ�У���� */
typedef enum {
    /* Ĭ�ϲ����Դ�Сд����ƥ�䲻������ */
    ALM_STATE_VERIFY_ITEM_ESN = 0,

    /* Ĭ�ϲ����Դ�Сд����ƥ�䲻������ */
    ALM_STATE_VERIFY_ITEM_PRD_NAME,

    /* Ĭ�ϲ����Դ�Сд����ƥ����벻������ */
    ALM_STATE_VERIFY_ITEM_PRD_VER,

    /* ������ֹ���ڣ�Ĭ�ϲ������� */
    ALM_STATE_VERIFY_ITEM_DEADLINE_OVER,

    /* ʧЧ��Ĭ�ϲ������� */
    ALM_STATE_VERIFY_ITEM_RVK
} ALM_STATE_CHECK_ITEM_ENUM;

/*
 * ����: ��������õ�У����򣬲�ͬ��У��������в�ͬ��Ĭ��ֵ����ο�У����塣
 * ����һ���µ�License��������License�ѽ�����Ч״̬���������
 * eItem - ָ��У�������: ALM_STATE_CHECK_ITEM_ENUM��
 * eMisState - �����µ�Licenseʱ���ڸ�У���ƥ��ʱLicenseӦ��Ϊ��״̬��
 *   ��ΪALM_STATE_INVALID����У���ƥ��ʱ���������µ�License��
 * eMisStateByGoOn - ��Ϊ����(��������֮ǰʹ�õ�License��ALM�����Զ�ˢ��)��
 *   �ڸ�У���ƥ��ʱLicenseӦ��Ϊ��״̬��
 * bIgnoreUpperAndLower - ��У�����Ƿ���Դ�Сд���������ַ����Ƚ�ʱ��Ч��
 * bIgnore - �Ƿ���Ը�У��������ԣ���У�����У�����㶨Ϊƥ�䡣
 */
typedef struct {
    ALM_ENUM(ALM_STATE_CHECK_ITEM_ENUM) eItem;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eMisState;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eMisStateByGoOn;
    ALM_BOOL bIgnoreUpAndLower;
    ALM_BOOL bIgnore;
} ALM_STATE_CHECK_CFG_STRU;

/* ������У����������Ϣ */
typedef struct {
    ALM_STATE_CHECK_CFG_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* �ýṹ���Ƿ��ʼ����־ */
    ALM_UINT32 uiInitFlag;
} ALM_STATE_CHECK_CFGS_STRU;

/* ��ʼ��ALM_STATE_CHECK_CFGS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StInitStateCheckCfgs(INOUT ALM_STATE_CHECK_CFGS_STRU *pstCfgs);

/* ����ALM_STATE_CHECK_CFGS_STRU���� */
LIC_EXPORT ALM_VOID ALM_StClsStateCheckCfgs(INOUT ALM_STATE_CHECK_CFGS_STRU *pstCfgs);

/* ����У�����У����򣬲�ͬ��У�����в�ͬ�����÷�Χ���ƣ�����μ�У���� */
/*
 * ��ǰ������ʱ����Ҫ���ô˽ӿ�����У����
 * 1��Ĭ�ϼ��ؽ׶Ρ��������R5C01�Ľӿ�ALM_Enableʱ��У�����
 * 2������license
 * ��Ĭ�ϼ��ؽ׶Σ����˲�Ʒ���Ƶ�eMisState���ó���ALM_STATE_LIC_INVALID��
 * ������У�����eMisState������ΪALM_STATE_LIC_GRACE_PERIOD;
 * ���������ó�����,��һ��Ҫ���ó�����������Ҫ������飬
 * ���ۣ�У�����eMisState����ΪALM_STATE_LIC_INVALIDʱ���Ƿ���Ҫ����Ĭ�ϼ��ص�license
 */
LIC_EXPORT ALM_INT32 ALM_StateMgrSetCheckCfgs(IN ALM_INT32 iPrdId, IN CONST ALM_STATE_CHECK_CFG_STRU *pstItems,
                                              IN ALM_UINT32 uiItemNum);

/* ��ѯУ�����ע��������� */
LIC_EXPORT ALM_INT32 ALM_StateMgrGetCheckCfgs(IN ALM_INT32 iPrdId, INOUT ALM_STATE_CHECK_CFGS_STRU *pstItems);

/************************************************************************
 * ״̬ԭ�򼰴���״̬����Ĳ�����
 ************************************************************************/
/* ��ǰ״̬ԭ��Ϊ��֤���汾������ö��ֵ��Ҫ����䶯�����ֵ��Ҫ����63 */
typedef enum {
    /************************************************************************
     * License��License BBOM״̬ԭ��
     ************************************************************************/
    /* License�ļ���ָ���Ĳ�Ʒ������Ӳ�Ʒʱָ�������Ʋ�һ�� */
    ALM_STATE_RSN_LIC_PRD_NAME_MIS = 0,

    /* License�ļ���ָ���Ĳ�Ʒ�汾����Ӳ�Ʒʱָ���İ汾��һ�� */
    ALM_STATE_RSN_LIC_PRD_VER_MIS = 1,

    /* License�ļ���ָ����ESN�뵱ǰ������ESN��һ�� */
    ALM_STATE_RSN_LIC_ESN_MIS = 2,

    /* ��ǰʱ���Ѿ�������ֹ���� */
    ALM_STATE_RSN_LIC_VALID_OVER = 3,

    /* �����Ѿ����� */
    ALM_STATE_RSN_LIC_GRACE_OVER = 4,

    /* License�Ѿ���ʧЧ */
    ALM_STATE_RSN_LIC_RVK = 5,

    /* License�ļ���ָ���Ĳ�Ʒ�汾����Ӳ�Ʒʱָ���İ汾�ڶ��β�һ�� */
    ALM_STATE_RSN_LIC_PRD_VER_REMIS = 6,

    /* License�ļ���ָ����ESN�뵱ǰ������ESN�ڶ��β�һ�� */
    ALM_STATE_RSN_LIC_ESN_REMIS = 7,

    /* ����R5C01esn/ver�ı�־�������ֵ��µ�LIC_ERR_LK_REPEATED_VER_MISMATCH */
    ALM_STATE_RSN_LIC_ESN_VER_REMIS = 8,

    /* ����LIC_ERR_LK_ESN_ANY_USE_DAY_LESSER */
    ALM_STATE_RSN_LIC_ESN_ANY_USE_DAY_LESSER = 9,

    /* ����LIC_ERR_LK_ESN_ANY_USE_DAY_OVER */
    ALM_STATE_RSN_LIC_ESN_ANY_USE_DAY_OVER = 10,

    /* �����ڼ��� */
    ALM_STATE_RSN_NUM
} ALM_STATE_RSN_ENUM;

/* ALM״̬ԭ����ֵ�п��ܿ���Ϊ���ԭ��Ļ�ϣ���������ԭ����ֵת��Ϊ����ԭ�� */
LIC_EXPORT ALM_INT32 ALM_StateMgrSplitRsn(IN ALM_UINT64 ulRsn,
                                          OUT ALM_ENUM(ALM_STATE_RSN_ENUM) aeRsns[ALM_STATE_RSN_NUM],
                                          OUT ALM_UINT32 *puiRsnNum);

/* ����״̬����Ĳ��� */
typedef enum {
    /* ʹ�ÿյ�license���� */
    ALM_STATE_OPERATE_NO_LK_ACTIVE_LIC,

    /* Ĭ�ϼ���licenseʧ�� */
    ALM_STATE_OPERATE_INIT_FAILED_ACTIVE_LIC,

    /* ����License */
    ALM_STATE_OPERATE_ACTIVE_LIC,

    /* �ָ�Ĭ��License */
    ALM_STATE_OPERATE_RECOVER_LIC,

    /* ʧЧLicense�ļ� */
    ALM_STATE_OPERATE_REVOKE_LIC,

    /* �������� */
    ALM_STATE_OPERATE_START_EMERGENCY,

    /* ֹͣ���� */
    ALM_STATE_OPERATE_STOP_EMERGENCY,

    /* ����Stick */
    ALM_STATE_OPERATE_START_STICK,

    /* ֹͣStick */
    ALM_STATE_OPERATE_STOP_STICK,

    /* ִ�лָ����� */
    ALM_STATE_OPERATE_RECOVER,

    /* ˢ��״̬ */
    ALM_STATE_OPERATE_REFRESH,

    /* ��ͣ */
    ALM_STATE_OPERATE_DISABLE
} ALM_STATE_OPERATE_ENUM;

/************************************************************************
 * ���֪ͨ��
 ************************************************************************/
/* �����ļ�״̬��У����䶯��Ϣ */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eLicType;

    ALM_BOOL bStateChange;
    ALM_BOOL bVerifyChange;
    ALM_BOOL bRecoverUsing;

    ALM_STATE_LIC_STRU stNew;
    ALM_STATE_LIC_STRU stOld;
} ALM_NOTIFY_CHANGE_LIC_STRU;

/* ����BBOM״̬�仯����Ȩֵ�仯 */
typedef struct {
    ALM_INT32 iBbomId;
    ALM_CHAR acName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];

    ALM_BOOL bStateChange;
    ALM_BOOL bValueChange;

    ALM_STATE_BBOM_STRU stOld;
    ALM_STATE_BBOM_STRU stNew;
} ALM_NOTIFY_CHANGE_BBOM_STRU;

/* ����License Bbom״̬�仯��Ϣ�����������License(����/stick��) */
typedef struct {
    ALM_INT32 iBbomId;
    CONST ALM_LIC_STORE_BBOM_STRU *pstBbomInfo;

    ALM_STATE_STRU stOld;
    ALM_STATE_STRU stNew;
    ALM_BOOL bStateChange;
    ALM_UINT32 uiReserved;
} ALM_NOTIFY_CHANGE_LIC_BBOM_STRU;

/* �����Ʒ״̬�仯 */
typedef struct {
    ALM_STATE_PRD_BASE_STRU stOld;
    ALM_STATE_PRD_BASE_STRU stNew;
    ALM_BOOL bStateChange;
} ALM_NOTIFY_CHANGE_PRD_STRU;

/* ������BBOM״̬�仯֪ͨ */
typedef struct {
    ALM_NOTIFY_CHANGE_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGE_BBOMS_STRU;

/* ������License Bbom״̬�仯��Ϣ */
typedef struct {
    ALM_NOTIFY_CHANGE_LIC_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGE_LIC_BBOMS_STRU;

/* ������License���֪ͨ */
typedef struct {
    ALM_NOTIFY_CHANGE_LIC_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGE_LICS_STRU;

/* ���浥����Ʒ״̬��BBOM��License�仯��Ϣ */
typedef struct {
    ALM_INT32 iPrdId;

    /* ���±��α���Ĳ��� */
    ALM_ENUM(ALM_STATE_OPERATE_ENUM) eOperate;

    /* �����Ʒ״̬�仯�����ޱ仯�õ�NULL */
    ALM_NOTIFY_CHANGE_PRD_STRU *pstPrd;

    /* �洢���BBOM�仯��Ϣ */
    ALM_NOTIFY_CHANGE_BBOMS_STRU stBboms;

    /* ������ǰ��ʹ�õ����û����License�ı�� */
    CONST ALM_NOTIFY_CHANGE_LIC_STRU *pstCurLicChange;

    /* �洢���License�仯��Ϣ */
    ALM_NOTIFY_CHANGE_LICS_STRU stLics;

    /* �洢License Bbom�仯��Ϣ */
    ALM_NOTIFY_CHANGE_LIC_BBOMS_STRU stLicBboms;
} ALM_NOTIFY_CHANGE_STRU;

/* ��������Ʒ�ı䶯��Ϣ */
typedef struct {
    ALM_NOTIFY_CHANGE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGES_STRU;

/* �澯���� */
typedef enum {
    ALM_ALARM_LEVEL_CRITICAL = 1,

    ALM_ALARM_LEVEL_MAJOR,

    ALM_ALARM_LEVEL_MINOR,

    ALM_ALARM_LEVEL_SUGGESTION,

    ALM_ALARM_LEVEL_BUTT
} ALM_ALARM_LEVEL_ENUM;

/* �澯���� */
typedef enum {
    ALM_ALARM_NAME_GRACE_PERIOD = 1,

    ALM_ALARM_NAME_DEFAULT,

    ALM_ALARM_NAME_FEATURE_GRACE_PERIOD,

    ALM_ALARM_NAME_FEATURE_DEFAULT,

    ALM_ALARM_NAME_EMERGENCY_START,

    ALM_ALARM_NAME_EMERGENCY_STOP,

    ALM_ALARM_NAME_STICK_START,

    ALM_ALARM_NAME_STICK_STOP,

    ALM_ALARM_NAME_LIC_PREDEADLINE,

    ALM_ALARM_NAME_FEATURE_PREDEADLINE,

    ALM_ALARM_NAME_BUTT
} ALM_ALARM_NAME_ENUM;

/* License �澯ԭ�� */
typedef enum {
    ALM_ALARM_RSN_LIC_PRD_VER_MIS = 0,
    ALM_ALARM_RSN_LIC_ESN_MIS = 1,
    ALM_ALARM_RSN_LIC_DEADLINE_OVER = 2,
    ALM_ALARM_RSN_LIC_GRACE_OVER = 3,
    ALM_ALARM_RSN_LIC_RVK = 4,

    ALM_ALARM_RSN_EMERGENCY_START = 5,
    ALM_ALARM_RSN_EMERGENCY_STOP = 6,
    ALM_ALARM_RSN_STICK_START = 7,
    ALM_ALARM_RSN_STICK_STOP = 8,

    ALM_ALARM_RSN_NO_DEFAULT_LK = 9,
    ALM_ALARM_RSN_INIT_FAILED_LK = 10,

    /* License����Feature�쵽��ֹ����,����PreDeadlineAlarm */
    ALM_ALARM_RSN_NEAR_DEADLINE = 11,

    ALM_ALARM_REASON_NUM
} ALM_ALARM_REASON_ENUM;

/* ALM�澯״̬ԭ����ֵ�п��ܿ���Ϊ���ԭ��Ļ�ϣ���������ԭ����ֵת��Ϊ����ԭ�� */
LIC_EXPORT ALM_INT32 ALM_AlarmSplitRsn(ALM_UINT64 ulRsn,
                                       OUT ALM_ENUM(ALM_ALARM_REASON_ENUM) aeRsns[ALM_ALARM_REASON_NUM],
                                       OUT ALM_UINT32 *puiRsnNum);

/* �澯���� */
typedef enum {
    ALM_ALARM_TYPE_FAULT = 1,
    ALM_ALARM_TYPE_RECOVERY,
    ALM_ALARM_TYPE_INFO,
    ALM_ALARM_TYPE_BUTT
} ALM_ALARM_TYPE_ENUM;

/* �򵥴洢�澯�ṹ�� */
typedef struct {
    ALM_ENUM(ALM_ALARM_NAME_ENUM) eAlarmName;
    ALM_ENUM(ALM_ALARM_LEVEL_ENUM) eAlarmLevel;
    ALM_ENUM(ALM_ALARM_TYPE_ENUM) eAlarmType;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eState;
    ALM_UINT64 ulAlarmReason;
    ALM_UINT32 uiStateLeftMins;
    ALM_CHAR acStateStartTime[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_UINT32 uiReserved;
} ALM_ALARM_BASE_STRU;

/* �ļ��澯 */
typedef struct {
    ALM_ALARM_BASE_STRU stBaseAlarm;
    ALM_CHAR acGraceEnterTime[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acGraceDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_ALARM_LK_STRU;

/* �ļ��澯�����ָ��͹��� */
typedef struct {
    ALM_ALARM_LK_STRU *pstRecoveryLkAlarm;
    ALM_ALARM_LK_STRU *pstFaultLkAlarm;
} ALM_ALARM_TOTAL_LK_STRU;

/* �򵥵Ļ���feature�澯�ṹ�� */
typedef struct {
    ALM_ENUM(ALM_ALARM_NAME_ENUM) eAlarmName;
    ALM_ENUM(ALM_ALARM_LEVEL_ENUM) eAlarmLevel;
    ALM_ENUM(ALM_ALARM_TYPE_ENUM) eAlarmType;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eFeatureState;
    ALM_UINT64 ulAlarmReason;
    ALM_UINT32 uiStateLeftMins;
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
} ALM_ALARM_BASE_FEATURE_STRU;

/* feature �澯�������ϸ澯�ͻָ��澯 */
typedef struct {
    ALM_CHAR acFeatureName[ALM_CONST_NAME_MAX_LEN + 1];
    ALM_CHAR acFeatureDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiFeatureId;
    ALM_ALARM_BASE_FEATURE_STRU *pstRecoveryFeature;
    ALM_ALARM_BASE_FEATURE_STRU *pstFaultFeature;
} ALM_ALARM_FEATURE_STRU;

typedef struct {
    ALM_ALARM_BASE_STRU *pstRecoveryLkAlarm;
    ALM_ALARM_BASE_STRU *pstFaultLkAlarm;

    ALM_UINT32 uiFeatureNum;
    ALM_UINT32 uiFeatureCap;
    ALM_ALARM_FEATURE_STRU *pstFeatureAlarms;
} ALM_ALARM_PREDEADLINE_STRU;

/* �����澯��Ϣ */
typedef struct {
    ALM_ALARM_BASE_STRU *pstStickAlarm;
    ALM_ALARM_TOTAL_LK_STRU stLkAlarm;
    ALM_ALARM_BASE_STRU *pstEmAlarm;
    ALM_UINT32 uiFeatureNum;
    ALM_ALARM_FEATURE_STRU *pstFeatureAlarms;
    ALM_ALARM_PREDEADLINE_STRU stPreDeadlineAlarm;
} ALM_ALARM_STRU;

/* ���浥����Ʒ�澯��Ϣ */
typedef struct {
    ALM_INT32 iPrdId;
    ALM_STATE_PRD_STRU *pstPrdState;
    ALM_ALARM_STRU stAlarm;
} ALM_ALARM_PRD_CHANGE_STRU;

/* ��������Ʒ�ĸ澯��Ϣ */
typedef struct {
    ALM_ALARM_PRD_CHANGE_STRU *pstArray;
    ALM_UINT32 uiNum;
} ALM_ALARM_PRDS_CHANGES_STRU;

/**
 * �ָ���Ʒ�澯;���ڲ�ƷDISABLE��,������������
 * ����: �ָ���Ʒ�����һ�����ϸ澯��
 */
LIC_EXPORT ALM_INT32 ALM_AlarmRecovery(IN ALM_INT32 iPrdId);

/* �����Ƿ�����Feature��������ÿ���ϱ�Feature���޸澯����
 * �ù���Ĭ�ϲ���������Ҫ�����ù��ܣ���ͨ����API��ALM_TRUE
 * ��API��Ҫ��ALM_Start֮ǰ����
 */
LIC_EXPORT ALM_INT32 ALM_AlarmFeatureGracePeriodAlarmSwitch(IN ALM_BOOL bTurnOn);

/* ���ÿ������߹رչ̶�����License������+�̶�����License����Feature��Deadline����ǰN�쿪ʼ�ϱ������Ը澯
 * iPrdId��ƷId;
 * bSwitch: ALM_TRUE, ����PreDeadline�����Ը澯���ܣ�
 * bSwitch: ALM_FALSE, �ر�PreDeadline�����Ը澯���ܣ�
 */
LIC_EXPORT ALM_INT32 ALM_AlarmSetPreDeadlineAlarmSwitch(IN ALM_INT32 iPrdId, IN ALM_BOOL bSwitch);

LIC_EXPORT ALM_INT32 ALM_AlarmGetPreDeadlineAlarmSwitch(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pSwitch);

/* ���ù̶�����License������+�̶�����License����Feature��Deadline����ǰN�쿪ʼ�ϱ������Ը澯
 * iPrdId��ƷId;
 * iPreDay: ����ǰN�쿪ʼ�ϱ���Ĭ��ֵ��0�����ú󣬸�ֵ��һֱ��Ч��ֱ����һ�����á�
 * �������ALM_AlarmSetPreDeadlineAlarmSwitch(iPrdId, ALM_TRUE)�������ù��ܣ�
 * ����û�е���ALM_AlarmSetPreDeadlineAlarmDays��������ǰN��, ��N����Ĭ��ֵ0����ʱ�಻���ϱ�����ǰ�澯
 */
LIC_EXPORT ALM_INT32 ALM_AlarmSetPreDeadlineAlarmDays(IN ALM_INT32 iPrdId, IN ALM_UINT32 iDays);

LIC_EXPORT ALM_INT32 ALM_AlarmGetPreDeadlineAlarmDays(IN ALM_INT32 iPrdId, OUT ALM_UINT32 *pDays);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* ALM_INTF_STATE_MGR_H */
