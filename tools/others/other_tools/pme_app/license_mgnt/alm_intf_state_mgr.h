/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: 定义存储状态信息的结构体，以及状态校验相关配置。
 * Author: AdaptiveLM team
 * Create: 2015-07-14
 */
#ifndef __ALM_INTF_STATE_MGR_H__
#define __ALM_INTF_STATE_MGR_H__

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
 * 定义状态及相关操作。
 ************************************************************************/
/**
 * 描述: License文件状态、控制项状态。
 * 注意: 本处定义和状态有限级有关，有效>宽限>无效。
 * 成员:
 * 1. ALM_STATE_LIC_INVALID - 无效。
 * 2. ALM_STATE_LIC_GRACE_PERIOD - 宽限。
 * 3. ALM_STATE_LIC_VALID - 生效。
 */
typedef enum {
    ALM_STATE_LIC_INVALID = 0,
    ALM_STATE_LIC_GRACE_PERIOD,
    ALM_STATE_LIC_VALID,
} ALM_STATE_LIC_ENUM;

/**
 * 描述: 产品状态。
 * 成员:
 * ALM_STATE_PRD_INVALID - 没有任何授权时状态。至少有默认授权不会出现此状态。
 * ALM_STATE_PRD_DEFAULT - 没有加载任何License文件或授权时状态。
 * ALM_STATE_PRD_GRACE_PERIOD - License文件过截止日期有一段时间的宽限期。
 * ALM_STATE_PRD_NORMAL - 正式商用License文件正常使用时状态。
 * ALM_STATE_PRD_EMERGENCY - 启用紧急时状态。
 * ALM_STATE_PRD_DEMO - 调测License正常使用时状态。
 * ALM_STATE_PRD_STICK - 启用Stick时状态。
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
 * 描述: 次要错误
 *
 */
typedef enum {
    ALM_MINOR_LK_ITEM_DEADLINE_OVER = 1 << 0,
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
    /* 部分过期 */
    ALM_MINOR_LK_ITEM_PARTIAL_DEADLINE_OVER = 1 << 11,
    /* 部分过宽限期 */
    ALM_MINOR_LK_ITEM_PARTIAL_TRIAL_OVER = 1 << 12
} ALM_MINOR_ENUM;

/*
 * 描述: 存储license及bbom状态信息。下文中在线运行时间是指Start后Stop前的
 * 运行时间，且不受当前时间跳变的影响(由定时器定期计时)。
 * 成员:
 * eState - 当前状态。
 * ulStateRsn - 当前状态的原因。
 * uiTotalMins - 当前状态自启动时间开始计算，允许运行的时间。
 *   若当前状态为永久(bPerm成员值为TRUE)，此值为0。
 *   若当前状态并非永久，进入时间 + 此值，可以得到预期的截止时间。
 *   运行期间到了预期的截止时间并不一定停止，需要触发刷新。
 * uiLeftMins - 当前状态剩余时间。运行期间状态并非实时刷新，此值并不精确。
 *   若为永久(bPerm成员值为TRUE)，此值为0。
 * uiValidPassMins - 有效期已运行时间。
 * uiGracePassMins - 宽限期已运行时间。
 * bPerm - 当前状态是否为永久，若为永久uiTotalMins、uiPassMins及uiLeftMins无效。
 * acEnterTime - 当前状态进入时间。初始激活License，在License变更通知中
 *   License之前状态中，此值为空字符串。
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
 * 描述: 存储产品及产品BBOM状态信息。
 * 成员:
 * eState - 当前状态。
 * acEnterTime - 当前状态进入时间。
 * bPerm - 当前状态是否为永久。
 * uiLeftMins - 当前状态剩余时间。运行期间状态更新并非实时，此值并不精确。
 * acLsn - 当前授权关联License文件。
 */
typedef struct {
    ALM_ENUM(ALM_STATE_PRD_ENUM) eState;
    ALM_CHAR acEnterTime[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiLeftMins;
    ALM_BOOL bPerm;
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
} ALM_STATE_PRD_BASE_STRU;

/*
 * 描述: 用于存储完整的状态信息。
 * 成员:
 * iPrdId - 产品编号。
 * uiAnyDemoRunMins - ESN ANY/有效期大于3天的调测License已使用分钟数。
 * uiAnyDemoTotalMins - ESN ANY/有效期大于3天的调测License总分钟数。
 * stBase - 当前状态。
 */
typedef struct {
    ALM_INT32 iPrdId;
    ALM_UINT32 uiAnyDemoRunMins;
    ALM_UINT32 uiAnyDemoTotalMins;
    ALM_STATE_PRD_BASE_STRU stBase;
} ALM_STATE_PRD_STRU;

/*
 * 描述: 存储产品注册的BBOM的状态信息及部分注册信息。
 * 成员:
 * iBbomId - BBOM编号。
 * acBbomName - BBOM名。
 * eType - BBOM类型。
 * ulMinVal - 注册时指定的最小值。
 * ulMaxVal - 注册时指定的最大值。
 * ulVal - 当前授权值。
 * stBase - BBOM基础状态信息。
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
    // ALM_UINT32 uiReserved;
} ALM_STATE_BBOM_STRU;

/*
 * 描述: 存储多个BBOM的状态信息。
 * 成员:
 * pstArray - 存储多个BBOM的状态信息的数组。
 * uiNum - BBOM数组中实际已存储BBOM数目。
 * uiCap - BBOM数组当前最大容量，产品开发不用关注此成员。
 */
typedef struct {
    ALM_STATE_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* 该结构体是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_STATE_BBOMS_STRU;

/* 用于初始化查询输出的ALM_STATE_BBOMS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StInitStateBboms(INOUT ALM_STATE_BBOMS_STRU *pstBBoms);

/* 清理ALM_STATE_BBOMS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsStateBboms(INOUT ALM_STATE_BBOMS_STRU *pstBboms);

/*
 * 描述: License校验信息。
 * bEsnMis - License文件是否与环境ESN不一致。
 * bPrdNameMis - License文件是否与产品名不一致。
 * bPrdVerMis - License文件是否与产品版本不一致。
 * bRvked - License文件是否已经被失效。
 * bValidOver - 有效期是否结束。仅参考截止日期，有效期未结束也有可能进入无效状态。
 * bGraceOver - 宽限期是否结束。参考宽限已运行时间、进入时间、允许使用的天数计算。
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
 * 描述: 保存License状态信息。
 * stBase - License文件的当前状态。
 * stVerify - License文件是否与环境ESN不一致。
 * bUsing - License文件是否被激活。若未激活，本结构其他成员内容，若之前激活过，
 *   内容为停止前的状态；否则为初值(invalid状态，进入时间为空字符串，其他值为0)。
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

/* 用于初始化查询输出的ALM_LIC_VERIFY_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StInitLicVerify(INOUT ALM_LIC_VERIFY_STRU *pstVerify);

/* 用于清理ALM_LIC_VERIFY_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsLicVerify(INOUT ALM_LIC_VERIFY_STRU *pstVerify);

/* 存储多个产品状态 */
typedef struct {
    ALM_STATE_PRD_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* 该结构体是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_STATE_PRDS_STRU;

/* bbom配置值，使用值 */
typedef struct {
    ALM_INT32 iBbomId;
    ALM_UINT64 ulItemConfVal;
    ALM_UINT64 ulItemUsedVal;
} ALM_ITEM_CONF_USED_INFO_STRU;

/* 用于初始化查询输出的ALM_STATE_PRDS_STRUT对象 */
LIC_EXPORT ALM_VOID ALM_StInitStatePrds(INOUT ALM_STATE_PRDS_STRU *pstPrds);

/* 用于清理ALM_STATE_PRDS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsStatePrds(INOUT ALM_STATE_PRDS_STRU *pstPrds);

/* 查询产品状态 */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryPrd(IN ALM_INT32 iPrdId, OUT ALM_STATE_PRD_STRU *pstState);

/* 查询得到全部产品状态，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryAllPrd(INOUT ALM_STATE_PRDS_STRU *pstState);

/* 查询指定产品当前已激活License状态及原因 */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryLic(IN ALM_INT32 iPrdId, OUT ALM_STATE_LIC_VERIFY_STRU *pstLicState);

/* 查询指定产品全部BBOM状态及授权值，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryBboms(IN ALM_INT32 iPrdId, INOUT ALM_STATE_BBOMS_STRU *pstBboms);

/*
 * 描述: 查询指定产品指定BBOM状态及授权值。
 * 参数:
 * iPrdId - 产品编号。
 * iBbomId - 需要查询的BBOM的编号，仅在未指定BBOM名时有效。
 * pcBbomName - 需要查询的BBOM的名称，若为NULL不限定名称。
 * pstBbom - 查询得到的BBOM授权及状态信息。
 * pbExist - 指定的BBOM是否存在，若不存在查询结果无效。
 */
LIC_EXPORT ALM_INT32 ALM_StateMgrQueryBbom(IN ALM_INT32 iPrdId, IN ALM_INT32 iBbomId, IN CONST ALM_CHAR *pcBbomName,
                                           OUT ALM_STATE_BBOM_STRU *pstBbom, OUT ALM_BOOL *pbExist);

/************************************************************************
 * 配置运行期间自动校验。
 ************************************************************************/
/* 状态管理模块相关配置 */
typedef struct {
    /* 是否限制刷新点数，TRUE-限制，默认TRUE */
    /* 若不限制成员ucAutoRefreshAtHour值无效 */
    ALM_BOOL bFixRefreshHour;

    /**
     * 允许刷新状态的点数，范围[0, 23]，默认ALM_CONST_STATE_REFRESH_DFLT_HOUR(2)。
     * 若指定为固定点刷新，状态过期时间点会延期到指定点数。因而可能导致宽限、
     * 紧急、Stick的运行时间超出允许的天数。
     */
    ALM_UINT8 ucAutoRefreshAtHour;

    /**
     * 每隔多久触发一次状态校验，范围[1, 60]分钟。
     * 默认ALM_CONST_STATE_REFRESH_DFLT_INTERVAL(30)。
     * 若指定校验点数，考虑定时器精度，此值最好小于60减去定时器一
     * 小时比实际一小时多出的实际偏移。
     */
    ALM_UINT8 ucAutoRefreshInterval;

} ALM_STATE_MGR_CFG_STRU;

/* 配置定时自动校验 */
LIC_EXPORT ALM_INT32 ALM_StateMgrCfgAutoRefresh(IN CONST ALM_STATE_MGR_CFG_STRU *pstCfg);

/* 得到定时自动校验的配置值 */
LIC_EXPORT ALM_INT32 ALM_StateMgrGetAutoRefreshCfg(OUT ALM_STATE_MGR_CFG_STRU *pstCfg);

/************************************************************************
 * 配置校验项
 ************************************************************************/
/* 可以配置的校验项 */
typedef enum {
    /* 默认不忽略大小写。不匹配不允许激活 */
    ALM_STATE_VERIFY_ITEM_ESN = 0,

    /* 默认不忽略大小写。不匹配不允许激活 */
    ALM_STATE_VERIFY_ITEM_PRD_NAME,

    /* 默认不忽略大小写。不匹配进入不允许激活 */
    ALM_STATE_VERIFY_ITEM_PRD_VER,

    /* 超过截止日期，默认不允许激活 */
    ALM_STATE_VERIFY_ITEM_DEADLINE_OVER,

    /* 失效，默认不允许激活 */
    ALM_STATE_VERIFY_ITEM_RVK

} ALM_STATE_CHECK_ITEM_ENUM;

/*
 * 描述: 定义可配置的校验规则，不同的校验项可能有不同的默认值，请参考校验项定义。
 * 激活一个新的License，若发现License已进入无效状态，不允许激活。
 * eItem - 指定校验项。参照: ALM_STATE_CHECK_ITEM_ENUM。
 * eMisState - 激活新的License时，在该校验项不匹配时License应该为的状态。
 *   若为ALM_STATE_INVALID，该校验项不匹配时不允许激活新的License。
 * eMisStateByGoOn - 若为延续(包含激活之前使用的License、ALM定期自动刷新)，
 *   在该校验项不匹配时License应该为的状态。
 * bIgnoreUpperAndLower - 该校验项是否忽略大小写。仅存在字符串比较时生效。
 * bIgnore - 是否忽略该校验项。若忽略，该校验项的校验结果恒定为匹配。
 */
typedef struct {
    ALM_ENUM(ALM_STATE_CHECK_ITEM_ENUM) eItem;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eMisState;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eMisStateByGoOn;
    ALM_BOOL bIgnoreUpAndLower;
    ALM_BOOL bIgnore;
} ALM_STATE_CHECK_CFG_STRU;

/* 保存多个校验项配置信息 */
typedef struct {
    ALM_STATE_CHECK_CFG_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* 该结构体是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_STATE_CHECK_CFGS_STRU;

/* 初始化ALM_STATE_CHECK_CFGS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StInitStateCheckCfgs(INOUT ALM_STATE_CHECK_CFGS_STRU *);

/* 清理ALM_STATE_CHECK_CFGS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsStateCheckCfgs(INOUT ALM_STATE_CHECK_CFGS_STRU *);

/* 配置校验项的校验规则，不同的校验项有不同的配置范围限制，详情参见校验项 */
/*
 *当前有两个时机需要调用此接口配置校验项:
 *1、默认加载阶段。适配的是R5C01的接口ALM_Enable时的校验规则
 *2、激活license
 *在默认加载阶段，除了产品名称的eMisState设置成了ALM_STATE_LIC_INVALID，
 *其他的校验项的eMisState均设置为ALM_STATE_LIC_GRACE_PERIOD;
 *不允许设置成其他,若一定要设置成其他规则，需要拉齐会议，
 *讨论：校验项的eMisState设置为ALM_STATE_LIC_INVALID时，是否需要管理默认加载的license
 */
LIC_EXPORT ALM_INT32 ALM_StateMgrSetCheckCfgs(IN ALM_INT32 iPrdId, IN CONST ALM_STATE_CHECK_CFG_STRU *pstItems,
                                              IN ALM_UINT32 uiItemNum);

/* 查询校验规则，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_StateMgrGetCheckCfgs(IN ALM_INT32 iPrdId, INOUT ALM_STATE_CHECK_CFGS_STRU *pstItems);

/************************************************************************
 * 状态原因及触发状态变更的操作。
 ************************************************************************/
/* 当前状态原因，为保证各版本，兼容枚举值不要随意变动，最大值不要超出63 */
typedef enum {
    /************************************************************************
     * License或License BBOM状态原因。
     ************************************************************************/
    /* License文件中指定的产品名与添加产品时指定的名称不一致 */
    ALM_STATE_RSN_LIC_PRD_NAME_MIS = 0,

    /* License文件中指定的产品版本与添加产品时指定的版本不一致 */
    ALM_STATE_RSN_LIC_PRD_VER_MIS = 1,

    /* License文件中指定的ESN与当前环境的ESN不一致 */
    ALM_STATE_RSN_LIC_ESN_MIS = 2,

    /* 当前时间已经超过截止日期 */
    ALM_STATE_RSN_LIC_VALID_OVER = 3,

    /* 宽限已经结束 */
    ALM_STATE_RSN_LIC_GRACE_OVER = 4,

    /* License已经被失效 */
    ALM_STATE_RSN_LIC_RVK = 5,

    /* License文件中指定的产品版本与添加产品时指定的版本第二次不一致 */
    ALM_STATE_RSN_LIC_PRD_VER_REMIS = 6,

    /* License文件中指定的ESN与当前环境的ESN第二次不一致 */
    ALM_STATE_RSN_LIC_ESN_REMIS = 7,

    /* 兼容R5C01esn/ver的标志不能区分导致的LIC_ERR_LK_REPEATED_VER_MISMATCH */
    ALM_STATE_RSN_LIC_ESN_VER_REMIS = 8,

    /* 兼容LIC_ERR_LK_ESN_ANY_USE_DAY_LESSER */
    ALM_STATE_RSN_LIC_ESN_ANY_USE_DAY_LESSER = 9,

    /* 兼容LIC_ERR_LK_ESN_ANY_USE_DAY_OVER */
    ALM_STATE_RSN_LIC_ESN_ANY_USE_DAY_OVER = 10,

    /* 仅用于计数 */
    ALM_STATE_RSN_NUM
} ALM_STATE_RSN_ENUM;

/* ALM状态原因数值中可能可能为多个原因的汇合，本函数将原因数值转换为具体原因 */
LIC_EXPORT ALM_INT32 ALM_StateMgrSplitRsn(IN ALM_UINT64 ulRsn,
                                          OUT ALM_ENUM(ALM_STATE_RSN_ENUM) aeRsns[ALM_STATE_RSN_NUM],
                                          OUT ALM_UINT32 *puiRsnNum);

/* 触发状态变更的操作 */
typedef enum {
    /* 使用空的license激活 */
    ALM_STATE_OPERATE_NO_LK_ACTIVE_LIC,

    /* 默认激活license失败 */
    ALM_STATE_OPERATE_INIT_FAILED_ACTIVE_LIC,

    /* 激活License */
    ALM_STATE_OPERATE_ACTIVE_LIC,

    /* 恢复默认License */
    ALM_STATE_OPERATE_RECOVER_LIC,

    /* 失效License文件 */
    ALM_STATE_OPERATE_REVOKE_LIC,

    /* 启动紧急 */
    ALM_STATE_OPERATE_START_EMERGENCY,

    /* 停止紧急 */
    ALM_STATE_OPERATE_STOP_EMERGENCY,

    /* 启动Stick */
    ALM_STATE_OPERATE_START_STICK,

    /* 停止Stick */
    ALM_STATE_OPERATE_STOP_STICK,

    /* 执行恢复动作 */
    ALM_STATE_OPERATE_RECOVER,

    /* 刷新状态 */
    ALM_STATE_OPERATE_REFRESH,

    /* 暂停 */
    ALM_STATE_OPERATE_DISABLE

} ALM_STATE_OPERATE_ENUM;

/************************************************************************
 * 变更通知。
 ************************************************************************/
/* 保存文件状态或校验项变动信息 */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eLicType;

    ALM_BOOL bStateChange;
    ALM_BOOL bVerifyChange;
    ALM_BOOL bRecoverUsing;

    ALM_STATE_LIC_STRU stNew;
    ALM_STATE_LIC_STRU stOld;
} ALM_NOTIFY_CHANGE_LIC_STRU;

/* 保存BBOM状态变化及授权值变化 */
typedef struct {
    ALM_INT32 iBbomId;
    ALM_CHAR acName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];

    ALM_BOOL bStateChange;
    ALM_BOOL bValueChange;

    ALM_STATE_BBOM_STRU stOld;
    ALM_STATE_BBOM_STRU stNew;
} ALM_NOTIFY_CHANGE_BBOM_STRU;

/* 保存License Bbom状态变化信息，不输出虚拟License(紧急/stick等) */
typedef struct {
    ALM_INT32 iBbomId;
    CONST ALM_LIC_STORE_BBOM_STRU *pstBbomInfo;

    ALM_STATE_STRU stOld;
    ALM_STATE_STRU stNew;
    ALM_BOOL bStateChange;
    ALM_UINT32 uiReserved;
} ALM_NOTIFY_CHANGE_LIC_BBOM_STRU;

/* 保存产品状态变化 */
typedef struct {
    ALM_STATE_PRD_BASE_STRU stOld;
    ALM_STATE_PRD_BASE_STRU stNew;
    ALM_BOOL bStateChange;
} ALM_NOTIFY_CHANGE_PRD_STRU;

/* 保存多个BBOM状态变化通知 */
typedef struct {
    ALM_NOTIFY_CHANGE_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGE_BBOMS_STRU;

/* 保存多个License Bbom状态变化信息 */
typedef struct {
    ALM_NOTIFY_CHANGE_LIC_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGE_LIC_BBOMS_STRU;

/* 保存多个License变更通知 */
typedef struct {
    ALM_NOTIFY_CHANGE_LIC_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGE_LICS_STRU;

/* 保存单个产品状态、BBOM、License变化信息 */
typedef struct {
    ALM_INT32 iPrdId;

    /* 导致本次变更的操作 */
    ALM_ENUM(ALM_STATE_OPERATE_ENUM) eOperate;

    /* 保存产品状态变化，若无变化得到NULL */
    ALM_NOTIFY_CHANGE_PRD_STRU *pstPrd;

    /* 存储多个BBOM变化信息 */
    ALM_NOTIFY_CHANGE_BBOMS_STRU stBboms;

    /* 给出当前正使用的商用或调测License的变更 */
    CONST ALM_NOTIFY_CHANGE_LIC_STRU *pstCurLicChange;

    /* 存储多个License变化信息 */
    ALM_NOTIFY_CHANGE_LICS_STRU stLics;

    /* 存储License Bbom变化信息 */
    ALM_NOTIFY_CHANGE_LIC_BBOMS_STRU stLicBboms;

} ALM_NOTIFY_CHANGE_STRU;

/* 保存多个产品的变动信息 */
typedef struct {
    ALM_NOTIFY_CHANGE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_NOTIFY_CHANGES_STRU;

/* 告警级别 */
typedef enum {
    ALM_ALARM_LEVEL_CRITICAL = 1,

    ALM_ALARM_LEVEL_MAJOR,

    ALM_ALARM_LEVEL_MINOR,

    ALM_ALARM_LEVEL_SUGGESTION,

    ALM_ALARM_LEVEL_BUTT
} ALM_ALARM_LEVEL_ENUM;

/* 告警名称 */
typedef enum {
    ALM_ALARM_NAME_GRACE_PERIOD = 1,

    ALM_ALARM_NAME_DEFAULT,

    ALM_ALARM_NAME_FEATURE_GRACE_PERIOD,

    ALM_ALARM_NAME_FEATURE_DEFAULT,

    ALM_ALARM_NAME_EMERGENCY_START,

    ALM_ALARM_NAME_EMERGENCY_STOP,

    ALM_ALARM_NAME_STICK_START,

    ALM_ALARM_NAME_STICK_STOP,

    ALM_ALARM_NAME_BUTT
} ALM_ALARM_NAME_ENUM;

/* License 告警原因 */
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

    ALM_ALARM_REASON_NUM
} ALM_ALARM_REASON_ENUM;

/* ALM告警状态原因数值中可能可能为多个原因的汇合，本函数将原因数值转换为具体原因 */
LIC_EXPORT ALM_INT32 ALM_AlarmSplitRsn(ALM_UINT64 ulRsn,
                                       OUT ALM_ENUM(ALM_ALARM_REASON_ENUM) aeRsns[ALM_ALARM_REASON_NUM],
                                       OUT ALM_UINT32 *puiRsnNum);

/* 告警类型 */
typedef enum {
    ALM_ALARM_TYPE_FAULT = 1,
    ALM_ALARM_TYPE_RECOVERY,
    ALM_ALARM_TYPE_INFO,
    ALM_ALARM_TYPE_BUTT
} ALM_ALARM_TYPE_ENUM;

/* 简单存储告警结构体 */
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

/* 文件告警 */
typedef struct {
    ALM_ALARM_BASE_STRU stBaseAlarm;
    ALM_CHAR acGraceEnterTime[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acGraceDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_ALARM_LK_STRU;

/* 文件告警包括恢复和故障 */
typedef struct {
    ALM_ALARM_LK_STRU *pstRecoveryLkAlarm;
    ALM_ALARM_LK_STRU *pstFaultLkAlarm;
} ALM_ALARM_TOTAL_LK_STRU;

/* 简单的基本feature告警结构体 */
typedef struct {
    ALM_ENUM(ALM_ALARM_NAME_ENUM) eAlarmName;
    ALM_ENUM(ALM_ALARM_LEVEL_ENUM) eAlarmLevel;
    ALM_ENUM(ALM_ALARM_TYPE_ENUM) eAlarmType;
    ALM_ENUM(ALM_STATE_LIC_ENUM) eFeatureState;
    ALM_UINT64 ulAlarmReason;
    ALM_UINT32 uiStateLeftMins;
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
} ALM_ALARM_BASE_FEATURE_STRU;

/* feature 告警包括故障告警和恢复告警 */
typedef struct {
    ALM_CHAR acFeatureName[ALM_CONST_NAME_MAX_LEN + 1];
    ALM_CHAR acFeatureDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiFeatureId;
    ALM_ALARM_BASE_FEATURE_STRU *pstRecoveryFeature;
    ALM_ALARM_BASE_FEATURE_STRU *pstFaultFeature;
} ALM_ALARM_FEATURE_STRU;

/* 单个告警信息 */
typedef struct {
    ALM_ALARM_BASE_STRU *pstStickAlarm;
    ALM_ALARM_TOTAL_LK_STRU stLkAlarm;
    ALM_ALARM_BASE_STRU *pstEmAlarm;
    ALM_UINT32 uiFeatureNum;
    ALM_ALARM_FEATURE_STRU *pstFeatureAlarms;
} ALM_ALARM_STRU;

/* 保存单个产品告警信息 */
typedef struct {
    ALM_INT32 iPrdId;
    ALM_STATE_PRD_STRU *pstPrdState;
    ALM_ALARM_STRU stAlarm;
} ALM_ALARM_PRD_CHANGE_STRU;

/* 保存多个产品的告警信息 */
typedef struct {
    ALM_ALARM_PRD_CHANGE_STRU *pstArray;
    ALM_UINT32 uiNum;
} ALM_ALARM_PRDS_CHANGES_STRU;

/**
 *恢复产品告警;仅在产品DISABLE后,才能正常调用
 * 描述: 恢复产品的最后一条故障告警。
 */
LIC_EXPORT ALM_INT32 ALM_AlarmRecovery(IN ALM_INT32 iPrdId);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* __ALM_INTF_STATE_MGR_H__ */
