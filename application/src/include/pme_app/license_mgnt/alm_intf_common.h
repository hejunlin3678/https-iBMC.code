/*
* Copyright (c) Huawei Technologies Co., Ltd. 2015-2023. All rights reserved.
* Description: ����һЩ��������.
* Author: AdaptiveLM team
* Create: 2015-07-14
*/
#ifndef ALM_INTF_COMMON_H
#define ALM_INTF_COMMON_H

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

/************************************************************************
 * ����: ����ָ���Ļ������亯����
 ************************************************************************/
/* ���ڴ洢ʱ��Ľṹ�� */
typedef struct {
    ALM_UINT16 usYear;
    ALM_UINT8 ucMonth;
    ALM_UINT8 ucDay;
    ALM_UINT8 ucHour;
    ALM_UINT8 ucMinute;
    ALM_UINT8 ucSecond;
    ALM_UINT8 ucReserved;
} ALM_OS_TIME_STRU;

/* �����Ʒ�ص��õ���ESN */
typedef struct {
    ALM_CHAR acEsn[ALM_CONST_ENV_ESN_MAX_LEN + 1];
} ALM_ENV_ESN_STRU;

/* �����ϣ��Ľ�� */
typedef struct {
    ALM_CHAR szStream[ALM_CONST_HASH_LEN + 1];
} ALM_HASH_STRU;

/* ��־����ָ����־�������ӡ��ָ����־������ֵ�����ȫ����־ */
typedef enum {
    /* ���ӡ������ */
    ALM_LOG_LEVEL_TRACE = 0,

    /* ��λ���⡢���Ե�ʹ�õļ��� */
    ALM_LOG_LEVEL_DEBUG,

    /* ����ʹ�õ���־����Ĭ��Ϊ�ü��� */
    ALM_LOG_LEVEL_INFO,

    /* notice������־ */
    ALM_LOG_LEVEL_NOTICE,

    /* warning������־ */
    ALM_LOG_LEVEL_WARNING,

    /* ����δ֪�����ش��� */
    ALM_LOG_LEVEL_ERROR,

    /* critiacl������־ */
    ALM_LOG_LEVEL_CRITICAL,

    /* fatal������־ */
    ALM_LOG_LEVEL_FATAL
} ALM_LOG_LEVEL_ENUM;

/* �����ڴ� */
typedef ALM_VOID *(*ALM_ADAPTER_BASE_MEM_MALLOC_FUNC)(ALM_UINT32 uiSize);

/* �ͷ��ڴ� */
typedef ALM_VOID (*ALM_ADAPTER_BASE_MEM_FREE_FUNC)(INOUT ALM_VOID *pMem);

/* �������������ɹ�Ӧ����ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_CREATE_FUNC)(CONST ALM_CHAR *pcSmName, OUT ALM_UINT32 *puiSmID);

/* ���󻥳������ɹ�Ӧ����ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_ACQUIRE_FUNC)(ALM_UINT32 uiSmID, ALM_UINT32 uiTimeOutInMillSec);

/* �ͷŻ��������ɹ�Ӧ����ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_RELEASE_FUNC)(ALM_UINT32 uiSmID);

/* ɾ�����������ɹ�Ӧ����ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_DELETE_FUNC)(ALM_UINT32 uiSmID);

/* ��ȡϵͳʱ�䣬�ɹ�Ӧ����ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_GET_TIME_FUNC)(OUT ALM_OS_TIME_STRU *pstSysTime);

/* ��ʱ�����ڴ������� */
typedef ALM_VOID (*ALM_ADAPTER_BASE_TIMER_RUN_FUNC)(ALM_VOID *pPara);

/* ������ʱ�� ,uiTimeIdСʱ��ʱ��ID��uiCycleMSecs�������ڣ�pfnTmCallBackִ�к���
 * pDataִ�к�������Σ�puiMaxLockMSecs��ʱʱ��
 */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_TIMER_START_FUNC)(ALM_UINT32 uiTimeId, ALM_UINT32 uiCycleMSecs,
    ALM_ADAPTER_BASE_TIMER_RUN_FUNC pfnTmCallBack, ALM_VOID *pData, OUT ALM_UINT32 *puiMaxLockMSecs);

/* ֹͣ��ʱ */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_TIMER_STOP_FUNC)(ALM_UINT32 uiTimeId);

/* ��ȡESN */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_GET_ESN_FUNC)(OUT ALM_ENV_ESN_STRU *pstEsn);

/* д�����ݵ��־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_PS_WRITE_FUNC)(CONST ALM_VOID *pBuf, ALM_UINT32 uiLen, ALM_UINT32 uiOffSet);

/* �ӳ־û��ļ��ж�ȡ���� */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_PS_READ_FUNC)(OUT ALM_VOID *pBuf, INOUT ALM_UINT32 *puiLen, ALM_UINT32 uiOffSet);

/* ����ָ���Ļ������亯��������: OS/�־û�/��ȡESN�� */
typedef struct {
    ALM_ADAPTER_BASE_MEM_MALLOC_FUNC fnMemMalloc;
    ALM_ADAPTER_BASE_MEM_FREE_FUNC fnMemFree;

    ALM_ADAPTER_BASE_MUTEX_CREATE_FUNC fnMutexCreate;
    ALM_ADAPTER_BASE_MUTEX_ACQUIRE_FUNC fnMutexAcquire;
    ALM_ADAPTER_BASE_MUTEX_RELEASE_FUNC fnMutexRelease;
    ALM_ADAPTER_BASE_MUTEX_DELETE_FUNC fnMutexDel;

    ALM_ADAPTER_BASE_TIMER_START_FUNC fnTimerStart;
    ALM_ADAPTER_BASE_TIMER_STOP_FUNC fnTimerStop;
    ALM_ADAPTER_BASE_GET_TIME_FUNC fnGetTime;

    ALM_ADAPTER_BASE_GET_ESN_FUNC fnGetEsn;

    ALM_ADAPTER_BASE_PS_WRITE_FUNC fnPsWrite;
    ALM_ADAPTER_BASE_PS_READ_FUNC fnPsRead;
} ALM_ADAPTER_BASE_FUNCS_STRU;

/*
 * ����: ����ر��Ļ�������������ǰ���á���ε��ûḲ��֮ǰ�����á�
 * pstBaseFuncs - ָ�������亯��������ָ��Ϊ�յ���Щ��Ա�����Ḳ��֮ǰ���á�
 */
LIC_EXPORT ALM_INT32 ALM_AdapterBaseFuncs(IN CONST ALM_ADAPTER_BASE_FUNCS_STRU *pstBaseFuncs);

/************************************************************************
 * ����: ��ѡ���亯����
 ************************************************************************/
/* C01�汾�־û����Ͷ����������� */
typedef enum {
    ALM_R5C01_PERSISTENT_FRIST = 0,
    ALM_R5C01_PERSISTENT_SECOND = 2
} ALM_R5C01_PERSISTENT_TYPE_ENUM;

/* C00�汾�־û����Ͷ����������� */
typedef enum {
    ALM_R5C00_PERSISTENT_FRIST = 0,
    ALM_R5C00_PERSISTENT_SECOND = 2,
    ALM_R5C00_PERSISTENT_FRIST_BAK = 3,
    ALM_R5C00_PERSISTENT_SECOND_BAK = 4
} ALM_R5C00_PERSISTENT_TYPE_ENUM;

/**
 * ����: �õ�C01�汾�־û��ļ���ز�Ʒ��Ŀ����ز�Ʒ��š�
 * piPrdIds - �洢�õ���C01�־û��洢��صĲ�Ʒ��ŵ����顣��Ϊ�գ�ֻ�����Ʒ��Ŀ��
 * puiPrdNum - piPrdIds��Ϊ��ʱ������piPrdIds�Ľ����Ŀ���������������Ʒ��Ŀ��
 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_C01_Get_PRDS_FUNC)(OUT ALM_INT32 *piPrdIds, INOUT ALM_UINT32 *puiPrdNum);

/* ��R5C01�汾�ĳ־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_PS_C01_READ_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
    OUT ALM_VOID *pBuf, INOUT ALM_UINT32 *puiInNeedOutGetSize, ALM_UINT32 uiOffSet);

/* дR5C01�汾�ĳ־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_PS_C01_WRITE_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
    IN ALM_VOID *pBuf, ALM_UINT32 uiBufSize, ALM_UINT32 uiOffSet);

/**
 * ����: �õ�R2C05�汾�־û��ļ���ز�Ʒ��Ŀ����ز�Ʒ��š�
 * piPrdIds - �洢�õ���R2C05�־û��洢��صĲ�Ʒ��ŵ����顣��Ϊ�գ�ֻ�����Ʒ��Ŀ��
 * puiPrdNum - piPrdIds��Ϊ��ʱ������piPrdIds�Ľ����Ŀ���������������Ʒ��Ŀ��
 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R2C05_Get_PRDS_FUNC)(OUT ALM_INT32 *piPrdIds, INOUT ALM_UINT32 *puiPrdNum);

/* ��R2C05�汾�ĳ־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R2C05_READ_FUNC)(ALM_INT32 iPrdId, OUT ALM_VOID *pBuf,
    INOUT ALM_UINT32 *puiInNeedOutGetSize, ALM_UINT32 uiOffSet);

/* дR2C05�汾�ĳ־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R2C05_WRITE_FUNC)(ALM_INT32 iPrdId, IN ALM_VOID *pBuf, ALM_UINT32 uiBufSize,
    ALM_UINT32 uiOffSet);

/*
 * ����:�õ�R5C00�汾�־û��ļ���ز�Ʒ��Ŀ����ز�Ʒ��š�
 * piPrdIds - �洢�õ���R5C00�־û��洢��صĲ�Ʒ��ŵ����顣��Ϊ�գ�ֻ�����Ʒ��Ŀ
 * puiPrdNum - piPrdIds��Ϊ��ʱ�����piPrdIds�Ľ����Ŀ���������������Ʒ��Ŀ��
 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R5C00_Get_PRDS_FUNC)(OUT ALM_INT32 *piPrdId, INOUT ALM_UINT32 *puiPrdNum);

/* ��R5C00�汾�ĳ־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R5C00_READ_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C00_PERSISTENT_TYPE_ENUM) eType,
    OUT ALM_VOID *pBuf, INOUT ALM_UINT32 *puiInNeedOutGetSize, ALM_UINT32 uiOffset);

/* дR5C00�汾�ĳ־û��ļ� */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R5C00_WRITE_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C00_PERSISTENT_TYPE_ENUM) eType,
    IN ALM_VOID *pBuf, ALM_UINT32 uiBufSize, IN ALM_UINT32 uiOffset);

/*
    ������������������
    pcKey�Ǵ�СΪALM_CONST_KEY_MAX_LEN+1���ڴ棬������Ҫ�����������󳤶�ΪALM_CONST_KEY_MAX_LEN��keyfile���ַ�����
*/
typedef ALM_INT32 (*ALM_ADAPTER_KEYFILE_READ_FUNC)(ALM_INT32 iPrdId, OUT ALM_CHAR *pcKey);

/* ��Ʒע���״̬���֪ͨ�ĺ���ԭ�� */
typedef ALM_INT32 (*ALM_ADAPTER_NOTIFY_CHANGE_FUNC)(CONST ALM_NOTIFY_CHANGES_STRU *pstChange);

/* ��Ʒע��ĸ澯ԭ�� */
typedef ALM_INT32 (*ALM_ADAPTER_ALARM_FUNC)(CONST ALM_ALARM_PRD_CHANGE_STRU *pstArray, ALM_UINT32 uiNum);

/* ��־��ӡ������������־������־���� */
typedef ALM_INT32 (*ALM_ADAPTER_LOG_FUNC)(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel, CONST ALM_CHAR *pcLogStr);

/* ���ݲ�ƷID��ȡESN */
typedef ALM_INT32 (*ALM_ADAPTER_GET_ESN_BY_PRD_ID_FUNC)(ALM_INT32 iPrdId, OUT ALM_ENV_ESN_STRU *pstEsn);

/*
    ��ȡ����ֵ��ʹ��ֵ
    uiNumΪ������ĸ���
    �ⲿ��Ҫ��������ĳ�ԱiBbomId�������ֵ��ʹ��ֵ
*/
typedef ALM_INT32 (*ALM_ADAPTER_GET_CONF_USED_LIST)(ALM_INT32 iPrdId, IN ALM_UINT32 uiNum,
    INOUT ALM_ITEM_CONF_USED_INFO_STRU *pstItemList);
/*
    ���ݱ����ļ�
    �������ļ���ȡʧ�ܻ�У��ʧ�ܣ������ݱ����ļ�
*/
typedef ALM_INT32 (*ALM_ADAPTER_BACKUP_PS_READ)(OUT ALM_VOID* pBuf, INOUT ALM_UINT32* puiLen, ALM_UINT32 uiOffSet);

/*
    �ṩ����dlsym����GetProcAddr������ϵͳapi���ܵĻص�;
    ��ȡ���̵�ַ�ռ��ض����ŵ�ַ�Ļص�����:�����������,���ط��ŵ�ַ;��������ڸ÷���,����NULL;
    ֻ�������㷨���ʱʹ��, �����������ע��
*/
typedef ALM_VOID *(*ALM_ADAPTER_GET_PROC_ADDR)(CONST IN ALM_CHAR *pcName);

typedef ALM_BOOL (*ALM_ADAPTER_GET_KEYFILE_MODE)(ALM_INT32 iPrdId);

/* ��ѡ�����亯�� */
typedef struct {
    ALM_ADAPTER_LOG_FUNC fnLog;

    ALM_ADAPTER_NOTIFY_CHANGE_FUNC fnNotify;
    ALM_ADAPTER_ALARM_FUNC fnAlarm;

    ALM_ADAPTER_PS_C01_READ_FUNC fnPsC01Read;
    ALM_ADAPTER_PS_C01_Get_PRDS_FUNC fnPsC01GetPrds;
    ALM_ADAPTER_PS_C01_WRITE_FUNC fnPsC01Write;
    ALM_ADAPTER_GET_ESN_BY_PRD_ID_FUNC fnGetEsnByPrdId;

    ALM_ADAPTER_PS_R2C05_READ_FUNC fnPsR2C05Read;
    ALM_ADAPTER_PS_R2C05_Get_PRDS_FUNC fnPsR2C05GetPrds;
    ALM_ADAPTER_PS_R2C05_WRITE_FUNC fnPsR2C05Write;

    ALM_ADAPTER_PS_R5C00_READ_FUNC fnPsR5C00Read;
    ALM_ADAPTER_PS_R5C00_Get_PRDS_FUNC fnPsR5C00GetPrds;
    ALM_ADAPTER_PS_R5C00_WRITE_FUNC fnPsR5C00Write;

    ALM_ADAPTER_KEYFILE_READ_FUNC fnKeyfileRead;
    ALM_ADAPTER_KEYFILE_READ_FUNC fnSecondKeyfileRead;

    ALM_ADAPTER_GET_CONF_USED_LIST fnGetConfUsedList;

    ALM_ADAPTER_BACKUP_PS_READ fnBackupPsRead;

    ALM_ADAPTER_GET_PROC_ADDR fnGetProcAddress;

    ALM_ADAPTER_GET_KEYFILE_MODE fnGetKeyFileMode;

} ALM_ADAPTER_OTHER_FUNCS_STRU;

/*
 * ����: �����ѡ�ص�����������ǰ����ɵ��á���ε��ûḲ��֮ǰ�����á�
 * pstFuncs - ָ�������亯�����ظ����ûḲ��֮ǰ���á�
 */
LIC_EXPORT ALM_INT32 ALM_AdapterOtherFuncs(IN CONST ALM_ADAPTER_OTHER_FUNCS_STRU *pstFuncs);

/************************************************************************
 * �־û�����־��
 ************************************************************************/
/* ����persister�����Ϣ */
typedef struct {
    /* ���������Ĳ�Ʒ��������������ɾ�����籣��Ĳ�Ʒ��Ϣ */
    ALM_INT8 cPrdMaxCount;

    /* ÿ����Ʒ����������License��������������ɾ�����籣��ļ�¼ */
    ALM_INT8 cPrdLicMaxCount;

    /* ÿ����Ʒ�����������ʧЧ��License��������������ɾ������ļ�¼ */
    ALM_INT8 cPrdRvkLicMaxCount;
} ALM_PS_CFG_STRU;

/* ����persister�����Ϣ��Startǰ���á�Ĭ�ϼ��������ֵ�� */
/* ����: ALM_CONST_PS_PRD_MAX_NUM, ALM_CONST_PS_PRD_LIC_MAX_NUM */
LIC_EXPORT ALM_INT32 ALM_PsCfgSet(IN CONST ALM_PS_CFG_STRU *pstCfg);

/* ��ѯpersister������� */
LIC_EXPORT ALM_INT32 ALM_PsCfgGet(OUT ALM_PS_CFG_STRU *pstCfg);

/* ������־����ֻ���ӡ��־������ֵ������ָ���������־ */
/* Ĭ����־����: ALM_LOG_LEVEL_INFO */
LIC_EXPORT ALM_INT32 ALM_LogSetLevel(IN ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel);

/* ������ֵת��Ϊ������Ϣ */
LIC_EXPORT CONST ALM_CHAR *ALM_ErrStr(IN ALM_INT32 iRet);

/* �жϴ˴������Ƿ�Ϊ�ر��ص����� */
LIC_EXPORT ALM_BOOL ALM_IsOsCallBackErr(IN ALM_INT32 iRet);

LIC_EXPORT ALM_INT32 ALM_GenerateHash(CONST ALM_UCHAR *pucData, ALM_UINT32 ulDataLen, ALM_HASH_STRU *pstHash);
/* �ⲿ���ûص�����ALM�ڴ��е����ݳ־û� */
LIC_EXPORT ALM_INT32 ALM_PsWrite(ALM_VOID);

/************************************************************************
 * ʱ�䰲ȫ�ԡ�
 ************************************************************************/
/* ����ʱ�䰲ȫ��ز��� */
typedef struct TAG_ALM_SAFETIME_STRU {
    /* ��׼ʱ�� */
    ALM_OS_TIME_STRU stBaseTime;

    /* ��ǰ��׼ʱ����ϵͳ�Ѿ����з����� */
    ALM_UINT32 uiBaseRunMin;

    /* �����Чʱ�� */
    ALM_OS_TIME_STRU stLastValidDay;

    /* �����Чʱ���������з����� */
    ALM_UINT32 uiLastRunMin;

    /* ��Ч������ʱ�� */
    ALM_UINT32 uiValidRunMin;

    /* ��׼ʱ��ĸ���ʱ�� */
    ALM_UINT32 uiChangeMin;

    /* ʱ�䰲ȫ�����ֵ */
    ALM_UINT32 uiTimeCheckThresholdMin;
} ALM_SAFETIME_STRU;

/*
 * ����
 * ���û�׼ʱ�䣬����ǰʱ�����ڻ�׼ʱ��+��׼ʱ��������ʱ�䣬��ǰʱ��Ϊ��Чʱ�䡣
 * ����֧�ְ�ȫʱ��У��Ĳ�Ʒ��ʱ����Чʱ���������µ�License�ļ����Ѽ����
 * License��ʹ�������¼������Чʱ��ȥ��״̬�жϡ�
 * ��׼ʱ���Զ����²���
 * a. ����Licenseʱ������License�Ĵ���ʱ��С�ڵ�ǰ��׼ʱ��+��׼������ʱ�䣬��
 *   ����ǰLicense�Ĵ���ʱ����Ϊ��׼ʱ�䡣
 * b. �ڵ�ǰʱ��������Чʱ�䳬��һ��ʱ�䣬����µ�ǰʱ��Ϊ��׼ʱ�䡣����ʱ��
 *   ���ȼ��Ƿ���øò��Ծ����ɲ������á�
 * ����
 * pcTime - ��׼ʱ�䣬Ĭ��ΪALM��ı���ʱ�䣬����ΪNULL��
 * uiRefreshTm - �ڵ�ǰʱ��������Чʱ�䳬����ֵ������µ�ǰʱ��Ϊ��׼ʱ�䡣��Ϊ0��
 *   ����ȡ�Ĳ��ԡ�
 */
LIC_EXPORT ALM_INT32 ALM_SafeTimeSetBaseTime(IN CONST ALM_CHAR *pcTime, IN ALM_UINT32 uiRefreshTm);

/* ��ѯʱ�䰲ȫ����ز��� */
LIC_EXPORT ALM_INT32 ALM_SafeTimeGetBaseInfo(INOUT ALM_SAFETIME_STRU *pstSafeTime);

/**
 * @ingroup AdaptiveLM
 * @brief ����ʱ�䰲ȫ�����ֵ��
 *
 * @attention ����ֵĬ��1��, ��API��Ҫ��ALM_Start֮����á�
 *
 * @li Memory operation: alloc��free��size:
 * -# ���ڴ����</br>
 * @li Thread safe:
 * -# �̰߳�ȫ</br>
 * @li OS/CPU difference:
 * -# ��OS����</br>
 * @li Time consuming:
 * -# �Ǻ�ʱ�ӿ�</br>
 *
 * @param uiThresholdMin [IN] ��������#ALM_UINT32������ʱ�䰲ȫ�����ֵ��
 *
 * @retval #ALM_OK 0 �ɹ���
 * @retval #��������ֵ ʧ�� - ��Ӧ���������API�ֲᡣ
 */
LIC_EXPORT ALM_INT32 ALM_SafeTimeSetTimeCheckThreshold(ALM_UINT32 uiThresholdMin);

/************************************************************************
 * ������ֹͣALM�����
 ************************************************************************/
/* ������������ */
typedef struct {
    /* ָ��License�汾���ɷ�ֹͷ�ļ���汾��һ�µ����� */
    /* ����ָ��ΪALM_CONST_LIC_CBB_C_VER */
    CONST ALM_CHAR *pcLicLibVer;
    ALM_BOOL bProPrdWrite;
} ALM_START_CFG_STRU;

/* ����汾�ṹ */
typedef struct {
    CONST ALM_CHAR *pcCVer;
    CONST ALM_CHAR *pcBVer;
    CONST ALM_CHAR *pcDVer;
} ALM_BASE_VER_STRU;

/*
 * ����: ��ɱ�Ҫ�������������������á�
 * ֻ������OS/����LOG/�־û���д�ص����ṹ������������������ǰ���ã�
 * ����������ֻ����������ִ�У������ڴ�ĺ������⡣
 * ����
 * pstCfg - �����������ã�����ΪNULL���ֽ׶�����ʵ�������ݡ�
 */
LIC_EXPORT ALM_INT32 ALM_Start(IN CONST ALM_START_CFG_STRU *pstCfg);

/* �ر�ALM��� */
LIC_EXPORT ALM_INT32 ALM_Stop(ALM_VOID);

/* �õ���ǰ�汾��Ϣ */
LIC_EXPORT ALM_VOID ALM_BaseGetVer(INOUT ALM_BASE_VER_STRU *pstVerInfo);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* ALM_INTF_COMMON_H */
