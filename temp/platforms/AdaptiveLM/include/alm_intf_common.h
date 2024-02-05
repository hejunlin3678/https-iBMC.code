/*
* Copyright (c) Huawei Technologies Co., Ltd. 2015-2023. All rights reserved.
* Description: 定义一些公共内容.
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
 * 适配: 必须指定的基础适配函数。
 ************************************************************************/
/* 用于存储时间的结构体 */
typedef struct {
    ALM_UINT16 usYear;
    ALM_UINT8 ucMonth;
    ALM_UINT8 ucDay;
    ALM_UINT8 ucHour;
    ALM_UINT8 ucMinute;
    ALM_UINT8 ucSecond;
    ALM_UINT8 ucReserved;
} ALM_OS_TIME_STRU;

/* 保存产品回调得到的ESN */
typedef struct {
    ALM_CHAR acEsn[ALM_CONST_ENV_ESN_MAX_LEN + 1];
} ALM_ENV_ESN_STRU;

/* 保存哈希后的结果 */
typedef struct {
    ALM_CHAR szStream[ALM_CONST_HASH_LEN + 1];
} ALM_HASH_STRU;

/* 日志级别，指定日志级别后会打印比指定日志级别数值更大的全部日志 */
typedef enum {
    /* 会打印出错函数 */
    ALM_LOG_LEVEL_TRACE = 0,

    /* 定位问题、调试等使用的级别 */
    ALM_LOG_LEVEL_DEBUG,

    /* 建议使用的日志级别，默认为该级别 */
    ALM_LOG_LEVEL_INFO,

    /* notice级别日志 */
    ALM_LOG_LEVEL_NOTICE,

    /* warning级别日志 */
    ALM_LOG_LEVEL_WARNING,

    /* 出现未知或严重错误 */
    ALM_LOG_LEVEL_ERROR,

    /* critiacl级别日志 */
    ALM_LOG_LEVEL_CRITICAL,

    /* fatal级别日志 */
    ALM_LOG_LEVEL_FATAL
} ALM_LOG_LEVEL_ENUM;

/* 分配内存 */
typedef ALM_VOID *(*ALM_ADAPTER_BASE_MEM_MALLOC_FUNC)(ALM_UINT32 uiSize);

/* 释放内存 */
typedef ALM_VOID (*ALM_ADAPTER_BASE_MEM_FREE_FUNC)(INOUT ALM_VOID *pMem);

/* 创建互斥量，成功应返回ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_CREATE_FUNC)(CONST ALM_CHAR *pcSmName, OUT ALM_UINT32 *puiSmID);

/* 请求互斥量，成功应返回ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_ACQUIRE_FUNC)(ALM_UINT32 uiSmID, ALM_UINT32 uiTimeOutInMillSec);

/* 释放互斥量，成功应返回ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_RELEASE_FUNC)(ALM_UINT32 uiSmID);

/* 删除互斥量，成功应返回ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_MUTEX_DELETE_FUNC)(ALM_UINT32 uiSmID);

/* 获取系统时间，成功应返回ALM_OK */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_GET_TIME_FUNC)(OUT ALM_OS_TIME_STRU *pstSysTime);

/* 定时器周期触发函数 */
typedef ALM_VOID (*ALM_ADAPTER_BASE_TIMER_RUN_FUNC)(ALM_VOID *pPara);

/* 启动定时器 ,uiTimeId小时定时器ID，uiCycleMSecs触发周期，pfnTmCallBack执行函数
 * pData执行函数的入参，puiMaxLockMSecs超时时间
 */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_TIMER_START_FUNC)(ALM_UINT32 uiTimeId, ALM_UINT32 uiCycleMSecs,
    ALM_ADAPTER_BASE_TIMER_RUN_FUNC pfnTmCallBack, ALM_VOID *pData, OUT ALM_UINT32 *puiMaxLockMSecs);

/* 停止定时 */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_TIMER_STOP_FUNC)(ALM_UINT32 uiTimeId);

/* 获取ESN */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_GET_ESN_FUNC)(OUT ALM_ENV_ESN_STRU *pstEsn);

/* 写入数据到持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_PS_WRITE_FUNC)(CONST ALM_VOID *pBuf, ALM_UINT32 uiLen, ALM_UINT32 uiOffSet);

/* 从持久化文件中读取数据 */
typedef ALM_INT32 (*ALM_ADAPTER_BASE_PS_READ_FUNC)(OUT ALM_VOID *pBuf, INOUT ALM_UINT32 *puiLen, ALM_UINT32 uiOffSet);

/* 必须指定的基础适配函数，包含: OS/持久化/获取ESN等 */
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
 * 描述: 适配必备的基础函数，启动前调用。多次调用会覆盖之前的设置。
 * pstBaseFuncs - 指定的适配函数，对已指定为空的这些成员，不会覆盖之前设置。
 */
LIC_EXPORT ALM_INT32 ALM_AdapterBaseFuncs(IN CONST ALM_ADAPTER_BASE_FUNCS_STRU *pstBaseFuncs);

/************************************************************************
 * 适配: 可选适配函数。
 ************************************************************************/
/* C01版本持久化类型读的数据类型 */
typedef enum {
    ALM_R5C01_PERSISTENT_FRIST = 0,
    ALM_R5C01_PERSISTENT_SECOND = 2
} ALM_R5C01_PERSISTENT_TYPE_ENUM;

/* C00版本持久化类型读的数据类型 */
typedef enum {
    ALM_R5C00_PERSISTENT_FRIST = 0,
    ALM_R5C00_PERSISTENT_SECOND = 2,
    ALM_R5C00_PERSISTENT_FRIST_BAK = 3,
    ALM_R5C00_PERSISTENT_SECOND_BAK = 4
} ALM_R5C00_PERSISTENT_TYPE_ENUM;

/**
 * 描述: 得到C01版本持久化文件相关产品数目或相关产品编号。
 * piPrdIds - 存储得到的C01持久化存储相关的产品编号的数组。若为空，只输出产品数目。
 * puiPrdNum - piPrdIds不为空时，输入piPrdIds的结点数目；否则用于输出产品数目。
 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_C01_Get_PRDS_FUNC)(OUT ALM_INT32 *piPrdIds, INOUT ALM_UINT32 *puiPrdNum);

/* 读R5C01版本的持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_C01_READ_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
    OUT ALM_VOID *pBuf, INOUT ALM_UINT32 *puiInNeedOutGetSize, ALM_UINT32 uiOffSet);

/* 写R5C01版本的持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_C01_WRITE_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C01_PERSISTENT_TYPE_ENUM) eType,
    IN ALM_VOID *pBuf, ALM_UINT32 uiBufSize, ALM_UINT32 uiOffSet);

/**
 * 描述: 得到R2C05版本持久化文件相关产品数目或相关产品编号。
 * piPrdIds - 存储得到的R2C05持久化存储相关的产品编号的数组。若为空，只输出产品数目。
 * puiPrdNum - piPrdIds不为空时，输入piPrdIds的结点数目；否则用于输出产品数目。
 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R2C05_Get_PRDS_FUNC)(OUT ALM_INT32 *piPrdIds, INOUT ALM_UINT32 *puiPrdNum);

/* 读R2C05版本的持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R2C05_READ_FUNC)(ALM_INT32 iPrdId, OUT ALM_VOID *pBuf,
    INOUT ALM_UINT32 *puiInNeedOutGetSize, ALM_UINT32 uiOffSet);

/* 写R2C05版本的持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R2C05_WRITE_FUNC)(ALM_INT32 iPrdId, IN ALM_VOID *pBuf, ALM_UINT32 uiBufSize,
    ALM_UINT32 uiOffSet);

/*
 * 描述:得到R5C00版本持久化文件相关产品数目或相关产品编号。
 * piPrdIds - 存储得到的R5C00持久化存储相关的产品编号的数组。若为空，只输出产品数目
 * puiPrdNum - piPrdIds不为空时，输出piPrdIds的结点数目；否则用于输出产品数目。
 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R5C00_Get_PRDS_FUNC)(OUT ALM_INT32 *piPrdId, INOUT ALM_UINT32 *puiPrdNum);

/* 读R5C00版本的持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R5C00_READ_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C00_PERSISTENT_TYPE_ENUM) eType,
    OUT ALM_VOID *pBuf, INOUT ALM_UINT32 *puiInNeedOutGetSize, ALM_UINT32 uiOffset);

/* 写R5C00版本的持久化文件 */
typedef ALM_INT32 (*ALM_ADAPTER_PS_R5C00_WRITE_FUNC)(ALM_INT32 iPrdId, ALM_ENUM(ALM_R5C00_PERSISTENT_TYPE_ENUM) eType,
    IN ALM_VOID *pBuf, ALM_UINT32 uiBufSize, IN ALM_UINT32 uiOffset);

/*
    西班牙问题自愈方案
    pcKey是大小为ALM_CONST_KEY_MAX_LEN+1的内存，函数需要往里面填充最大长度为ALM_CONST_KEY_MAX_LEN的keyfile的字符串。
*/
typedef ALM_INT32 (*ALM_ADAPTER_KEYFILE_READ_FUNC)(ALM_INT32 iPrdId, OUT ALM_CHAR *pcKey);

/* 产品注册的状态变更通知的函数原型 */
typedef ALM_INT32 (*ALM_ADAPTER_NOTIFY_CHANGE_FUNC)(CONST ALM_NOTIFY_CHANGES_STRU *pstChange);

/* 产品注册的告警原型 */
typedef ALM_INT32 (*ALM_ADAPTER_ALARM_FUNC)(CONST ALM_ALARM_PRD_CHANGE_STRU *pstArray, ALM_UINT32 uiNum);

/* 日志打印函数，传入日志级别及日志内容 */
typedef ALM_INT32 (*ALM_ADAPTER_LOG_FUNC)(ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel, CONST ALM_CHAR *pcLogStr);

/* 根据产品ID获取ESN */
typedef ALM_INT32 (*ALM_ADAPTER_GET_ESN_BY_PRD_ID_FUNC)(ALM_INT32 iPrdId, OUT ALM_ENV_ESN_STRU *pstEsn);

/*
    获取配置值、使用值
    uiNum为控制项的个数
    外部需要按照里面的成员iBbomId填充配置值和使用值
*/
typedef ALM_INT32 (*ALM_ADAPTER_GET_CONF_USED_LIST)(ALM_INT32 iPrdId, IN ALM_UINT32 uiNum,
    INOUT ALM_ITEM_CONF_USED_INFO_STRU *pstItemList);
/*
    备份保密文件
    主保密文件读取失败或校验失败，读备份保密文件
*/
typedef ALM_INT32 (*ALM_ADAPTER_BACKUP_PS_READ)(OUT ALM_VOID* pBuf, INOUT ALM_UINT32* puiLen, ALM_UINT32 uiOffSet);

/*
    提供类似dlsym或者GetProcAddr这样的系统api功能的回调;
    获取进程地址空间特定符号地址的回调函数:输入符号名字,返回符号地址;如果不存在该符号,返回NULL;
    只有在弱算法拆分时使用, 其他情况不用注册
*/
typedef ALM_VOID *(*ALM_ADAPTER_GET_PROC_ADDR)(CONST IN ALM_CHAR *pcName);

typedef ALM_BOOL (*ALM_ADAPTER_GET_KEYFILE_MODE)(ALM_INT32 iPrdId);

/* 可选的适配函数 */
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
 * 描述: 适配可选回调函数，启动前后均可调用。多次调用会覆盖之前的设置。
 * pstFuncs - 指定的适配函数，重复设置会覆盖之前设置。
 */
LIC_EXPORT ALM_INT32 ALM_AdapterOtherFuncs(IN CONST ALM_ADAPTER_OTHER_FUNCS_STRU *pstFuncs);

/************************************************************************
 * 持久化及日志。
 ************************************************************************/
/* 定义persister规格信息 */
typedef struct {
    /* 最多允许保存的产品数量；若超出会删除更早保存的产品信息 */
    ALM_INT8 cPrdMaxCount;

    /* 每个产品最多允许保存的License数量；若超出会删除更早保存的记录 */
    ALM_INT8 cPrdLicMaxCount;

    /* 每个产品最多允许保存已失效的License数量；若超出会删除更早的记录 */
    ALM_INT8 cPrdRvkLicMaxCount;
} ALM_PS_CFG_STRU;

/* 配置persister规格信息，Start前调用。默认及最大配置值， */
/* 参照: ALM_CONST_PS_PRD_MAX_NUM, ALM_CONST_PS_PRD_LIC_MAX_NUM */
LIC_EXPORT ALM_INT32 ALM_PsCfgSet(IN CONST ALM_PS_CFG_STRU *pstCfg);

/* 查询persister规格配置 */
LIC_EXPORT ALM_INT32 ALM_PsCfgGet(OUT ALM_PS_CFG_STRU *pstCfg);

/* 设置日志级别，只会打印日志级别数值不低于指定级别的日志 */
/* 默认日志级别: ALM_LOG_LEVEL_INFO */
LIC_EXPORT ALM_INT32 ALM_LogSetLevel(IN ALM_ENUM(ALM_LOG_LEVEL_ENUM) eLevel);

/* 将返回值转换为错误信息 */
LIC_EXPORT CONST ALM_CHAR *ALM_ErrStr(IN ALM_INT32 iRet);

/* 判断此错误码是否为必备回调错误 */
LIC_EXPORT ALM_BOOL ALM_IsOsCallBackErr(IN ALM_INT32 iRet);

LIC_EXPORT ALM_INT32 ALM_GenerateHash(CONST ALM_UCHAR *pucData, ALM_UINT32 ulDataLen, ALM_HASH_STRU *pstHash);
/* 外部调用回调，将ALM内存中的数据持久化 */
LIC_EXPORT ALM_INT32 ALM_PsWrite(ALM_VOID);

/************************************************************************
 * 时间安全性。
 ************************************************************************/
/* 保存时间安全相关参数 */
typedef struct TAG_ALM_SAFETIME_STRU {
    /* 基准时间 */
    ALM_OS_TIME_STRU stBaseTime;

    /* 当前基准时间下系统已经运行分钟数 */
    ALM_UINT32 uiBaseRunMin;

    /* 最近有效时间 */
    ALM_OS_TIME_STRU stLastValidDay;

    /* 最近有效时间下已运行分钟数 */
    ALM_UINT32 uiLastRunMin;

    /* 有效运行总时间 */
    ALM_UINT32 uiValidRunMin;

    /* 基准时间的更新时间 */
    ALM_UINT32 uiChangeMin;

    /* 时间安全检查阈值 */
    ALM_UINT32 uiTimeCheckThresholdMin;
} ALM_SAFETIME_STRU;

/*
 * 描述
 * 设置基准时间，若当前时间早于基准时间+基准时间已运行时间，则当前时间为无效时间。
 * 对已支持安全时间校验的产品，时间无效时不允许激活新的License文件，已激活的
 * License会使用最近记录到的有效时间去做状态判断。
 * 基准时间自动更新策略
 * a. 激活License时，发现License的创建时间小于当前基准时间+基准已运行时间，会
 *   将当前License的创建时间作为基准时间。
 * b. 在当前时间连续有效时间超过一定时间，会更新当前时间为基准时间。具体时间
 *   长度及是否采用该策略均可由参数设置。
 * 参数
 * pcTime - 基准时间，默认为ALM库的编译时间，不能为NULL。
 * uiRefreshTm - 在当前时间连续有效时间超过此值，会更新当前时间为基准时间。若为0，
 *   不采取改策略。
 */
LIC_EXPORT ALM_INT32 ALM_SafeTimeSetBaseTime(IN CONST ALM_CHAR *pcTime, IN ALM_UINT32 uiRefreshTm);

/* 查询时间安全性相关参数 */
LIC_EXPORT ALM_INT32 ALM_SafeTimeGetBaseInfo(INOUT ALM_SAFETIME_STRU *pstSafeTime);

/**
 * @ingroup AdaptiveLM
 * @brief 设置时间安全检查阈值。
 *
 * @attention 该阈值默认1天, 该API需要在ALM_Start之后调用。
 *
 * @li Memory operation: alloc、free、size:
 * -# 无内存分配</br>
 * @li Thread safe:
 * -# 线程安全</br>
 * @li OS/CPU difference:
 * -# 无OS差异</br>
 * @li Time consuming:
 * -# 非耗时接口</br>
 *
 * @param uiThresholdMin [IN] 参数类型#ALM_UINT32，设置时间安全检查阈值。
 *
 * @retval #ALM_OK 0 成功。
 * @retval #其他返回值 失败 - 对应错误码详见API手册。
 */
LIC_EXPORT ALM_INT32 ALM_SafeTimeSetTimeCheckThreshold(ALM_UINT32 uiThresholdMin);

/************************************************************************
 * 启动及停止ALM组件。
 ************************************************************************/
/* 定义启动配置 */
typedef struct {
    /* 指定License版本，可防止头文件与版本不一致的问题 */
    /* 建议指定为ALM_CONST_LIC_CBB_C_VER */
    CONST ALM_CHAR *pcLicLibVer;
    ALM_BOOL bProPrdWrite;
} ALM_START_CFG_STRU;

/* 定义版本结构 */
typedef struct {
    CONST ALM_CHAR *pcCVer;
    CONST ALM_CHAR *pcBVer;
    CONST ALM_CHAR *pcDVer;
} ALM_BASE_VER_STRU;

/*
 * 描述: 完成必要组件的启动及规格性配置。
 * 只有配置OS/配置LOG/持久化读写回调及结构体清理函数可以在启动前调用，
 * 其他函数均只能在启动后执行，清理内存的函数除外。
 * 参数
 * pstCfg - 定义启动配置，不能为NULL。现阶段暂无实质性内容。
 */
LIC_EXPORT ALM_INT32 ALM_Start(IN CONST ALM_START_CFG_STRU *pstCfg);

/* 关闭ALM组件 */
LIC_EXPORT ALM_INT32 ALM_Stop(ALM_VOID);

/* 得到当前版本信息 */
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
