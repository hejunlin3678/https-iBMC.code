/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: License文件管理，包括商用、调测License管理，紧急及STICK管理.
 * Author: AdaptiveLM team
 * Create: 2015-07-14
 */
#ifndef __ALM_INTF_LIC_MGR_H__
#define __ALM_INTF_LIC_MGR_H__

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

/* 保存多个失效码 */
typedef struct {
    ALM_LIC_RVK_TICKET_STRU *pstTickets;
    ALM_UINT32 uiNum;
    /* 该结构体是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_RVK_TICKETS_STRU;

/* 清理ALM_LIC_RVK_TICKETS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StInitLicRvkTickets(INOUT ALM_LIC_RVK_TICKETS_STRU *pstTickets);

/* 清理ALM_LIC_RVK_TICKETS_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsLicRvkTickets(INOUT ALM_LIC_RVK_TICKETS_STRU *pstTickets);

/* 保存License状态比较结果 */
typedef struct {
    ALM_STATE_LIC_VERIFY_STRU stCnt;
    ALM_STATE_LIC_VERIFY_STRU stNew;
} ALM_LIC_CMP_STATE_STRU;

/* 保存BBOM比较结果，只保存状态、或授权值、或剩余时间不相同的BBOM */
typedef struct {
    ALM_STATE_BBOM_STRU stCnt;
    ALM_STATE_BBOM_STRU stNew;
} ALM_LIC_CMP_BBOM_STRU;

/* 保存SBOM比较结果，两个License文件中的SBOM */
typedef struct {
    ALM_CHAR acName[ALM_CONST_LIC_SBOM_NAME_MAX + 1];
    ALM_CHAR acChsUnit[ALM_CONST_LIC_SBOM_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acEngUnit[ALM_CONST_LIC_SBOM_DESC_ENG_UNIT_MAX + 1];

    ALM_BOOL bCntInc;
    ALM_BOOL bNewInc;

    /* 同名SBOM值累加 */
    ALM_UINT64 ulCntVal;
    ALM_UINT64 ulNewVal;

    /* 同名SBOM截止日期取最小 */
    ALM_CHAR acCntDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acNewDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_CMP_SBOM_STRU;

/* 保存PackageItem比较结果，两个licensen文件中的PackageItem */
typedef struct {
    ALM_CHAR acName[ALM_CONST_LIC_PACK_NAME_MAX + 1];
    ALM_CHAR acChsUnit[ALM_CONST_LIC_PACK_DESC_CHS_UNIT_MAX + 1];
    ALM_CHAR acEngUnit[ALM_CONST_LIC_PACK_DESC_ENG_UNIT_MAX + 1];

    ALM_BOOL bCntInc;
    ALM_BOOL bNewInc;

    /* 同名PackageItem值累加 */
    ALM_UINT64 ulCntVal;
    ALM_UINT64 ulNewVal;

    /* 同名PackageItem截止日期取最小 */
    ALM_CHAR acCntDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_CHAR acNewDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_CMP_PACK_STRU;

/* 存储多个BBOM比较结果 */
typedef struct {
    ALM_LIC_CMP_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_CMP_BBOMS_STRU;

/* 存储多个SBOM比较结果 */
typedef struct {
    ALM_LIC_CMP_SBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_CMP_SBOMS_STRU;

/* 存储多个Pack比较结果 */
typedef struct {
    ALM_LIC_CMP_PACK_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_CMP_PACKS_STRU;

/*
 * 描述:
 * 保存License比较结果。比较结果为只考虑已激活License及指定License激活到指定产品后的
 * 状态信息的比较。包含默认值，不受紧急、Stick等影响。
 */
typedef struct {
    /* License状态比较结果 */
    ALM_LIC_CMP_STATE_STRU stLicCmp;

    /* BBOM比较结果 */
    ALM_LIC_CMP_BBOMS_STRU stBboms;

    /* SBBOM比较结果 */
    ALM_LIC_CMP_SBOMS_STRU stSboms;

    /* Pack比较结果 */
    ALM_LIC_CMP_PACKS_STRU stPacks;
    /* 该结构体是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_CMP_STRU;

/* 初始化ALM_LIC_CMP_STRU对象，一般用于初始化license compare结果 */
LIC_EXPORT ALM_VOID ALM_StInitLicCmp(INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/* 清理ALM_LIC_CMP_STRU对象，一般用于清理license compare结果 */
LIC_EXPORT ALM_VOID ALM_StClsLicCmp(INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/************************************************************************
 * 处理正式License（商用及调测License)
 ************************************************************************/
/* 定义License激活结果 */
typedef struct {
    ALM_INT32 iRet;                         /* 得到激活该文件时的返回值，ALM_OK表示成功 */
    ALM_BOOL bVerifyIsValid;                /* 仅在该项为TRUE时，License验证结果才有参照价值 */
    ALM_STATE_PRD_LIC_VERIFY_STRU stVerify; /* License的验证结果 */
} ALM_LIC_ACTIVE_RSLT_STRU;

/* 定义待激活License */
typedef struct {
    ALM_INT32 iPrdId;                /* 激活到指定产品的编号 */
    ALM_CHAR *pcLicStr;              /* License文件内容 */
    ALM_UINT32 uiLicLen;             /* License文件内容的长度 */
    ALM_LIC_ACTIVE_RSLT_STRU stRslt; /* 输出该License的激活结果 */
} ALM_LIC_ACTIVE_STRU;

/*
 * 描述: 激活License到指定产品，一次性激活多个License只会发送一次变更通知。
 * 参数:
 * pstLics - 指定待激活License，并得到激活结果。
 * uiLicNum - 待激活License的数目。
 * puiFailNum - 激活失败的数目。
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralActive(INOUT ALM_LIC_ACTIVE_STRU *pstLics, IN ALM_UINT32 uiLicNum,
                                          OUT ALM_UINT32 *puiFailNum);

/* 查询当前已激活或被反激活的License文件内容，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralQuery(IN ALM_INT32 iPrdId, INOUT ALM_LIC_STORE_STRU *pstLic,
                                         OUT ALM_LIC_ATTRI_STRU *pstLicAttri);

/* 解析License文件 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralParser(IN ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                          INOUT ALM_LIC_STORE_STRU *pstLic,
                                          INOUT ALM_LIC_FEATS_INFO_STRU *pstLicFeatsInfo);

/* 设置是否限制License大小, 
 * ALMAdaptiveLM组件默认限制License小于2M, 可以通过向该API传ALM_FALSE将此限制取消.
 * 该api需要在ALM_Start前被调用 */
LIC_EXPORT ALM_INT32 ALM_LicSetMaxLengthSwitch(IN ALM_BOOL bTurnOn);

/* 解析License文件，适用于不需要产品管理的产品，本接口不上锁，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_LicParser(CONST ALM_CHAR *pcFileBuf, ALM_UINT32 uiFileLen, CONST ALM_CHAR *pcKey,
                                   ALM_UINT32 uiKeyLen, OUT ALM_LIC_STORE_STRU *pstLicStore);

/* Bpart规划量/使用量转换为Spart规划量/使用量 */
LIC_EXPORT ALM_INT32 ALM_BpartToSpart(ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcBpartName, IN CONST ALM_CHAR *pcSpartName,
                                      ALM_UINT64 ulBpartPlanVal, ALM_UINT64 ulBpartUsageVal,
                                      OUT ALM_S2B_SPART_RSLT_STRU *pstSpartRslt);

/* 用于清理ALM_S2B_SPART_RSLT_STRU对象 */
LIC_EXPORT ALM_VOID ALM_FreeS2bSpartRslt(IN ALM_S2B_SPART_RSLT_STRU *pstSpartRslt);

/*
 * 描述: 比较指定License文件与当前已激活License文件。
 * 本函数，只考虑对应的产品仅激活这两个license后的授权值及状态的比较。
 * （忽略当前的紧急、Stick等状态）。
 * 参数:
 * iPrdId - 产品编号。
 * pcLicStr - 新的License文件。
 * uiLicLen - License文件长度。
 * pstCmpRslt - 得到比较结果，注意清理输出。
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralCmp(IN ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                       INOUT ALM_LIC_CMP_STRU *pstCmpRslt);

/* 预激活License并得到License状态 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralVerify(IN ALM_INT32 iPrdId, IN CONST ALM_CHAR *pcLicStr, IN ALM_UINT32 uiLicLen,
                                          OUT ALM_LIC_VERIFY_STRU *pstRslt);

/*
 * 描述: 对License文件执行失效，若当前License文件已经被失效。仅得到失效码不刷新状态。
 * 参数:
 * 1. iPrdId - 需要失效当前已激活License的产品。
 * 2. pstRvkcTiket - 得到的失效码。
 * 3. pbHaveRvked - 执行失效前若已被失效得到的值为真。
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralRevoke(IN ALM_INT32 iPrdId, OUT ALM_LIC_RVK_TICKET_STRU *pstRvkcTiket,
                                          OUT ALM_BOOL *pbHaveRvked);

/* 查询该产品下全部失效码，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralQueryAllRevoke(IN ALM_INT32 iPrdId, INOUT ALM_LIC_RVK_TICKETS_STRU *pstRvkcTiket);

/*
 * 描述: 计算指定产品的指定lsn的失效码
 * 参数:
 * 1. iPrdId - 需要失效码的产品。
 * 2. pstLsnRvkTicket - 输入需要失效码的license的lsn 和输出失效码。
 */
LIC_EXPORT ALM_INT32 ALM_LicGeneralGetRvkTkt(ALM_INT32 iPrdId, INOUT ALM_LIC_LSN_RVK_TICKET_STRU *pstLsnRvkTicket);

/*
 * 描述: 计算指定keyfile的指定lsn的失效码,不用上锁
 * 参数:
 * 1. pcKey - keyfile。
 * 2. pstLsnRvkTicket - 输入需要失效码的license的lsn 和输出失效码。
 */
LIC_EXPORT ALM_INT32 ALM_LicGetRvkTkt(CONST ALM_CHAR *pcKey, INOUT ALM_LIC_LSN_RVK_TICKET_STRU *pstLsnRvkTicket);

/************************************************************************
 * 处理紧急
 ************************************************************************/
/* 紧急相关配置 */
typedef struct {
    /* 指定支持紧急的BBOM及紧急下的授权值 */
    /* 若为NULL，仅用于配置启动次数，必须已经配置过紧急 */
    ALM_SLIC_BBOM_STRU *pstBboms;

    /* 指定支持紧急的BBOM的数目 */
    /* 若为0，仅用于配置启动次数，必须已经配置过紧急 */
    ALM_UINT32 uiNum;

    /* 紧急购买次数次数，每次启动运行7天，最大7次 */
    /* 消耗完购买次数后将会使用免费次数 */
    ALM_UINT32 uiBuyCount;
} ALM_LIC_EM_CFG_STRU;

/* 配置紧急 */
LIC_EXPORT ALM_INT32 ALM_LicEmCfg(IN ALM_INT32 iPrdId, IN CONST ALM_LIC_EM_CFG_STRU *pstCfg);

/*
 * 启动紧急。
 * 若之前已经启动，且没有调用恢复函数恢复运行，将恢复紧急，不消耗启动次数。
 * 恢复运行时，若发现已经超期，会再次启动并进入新的周期。
 */
LIC_EXPORT ALM_INT32 ALM_LicEmStart(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pbIsRecover);

/* 停止紧急 */
LIC_EXPORT ALM_INT32 ALM_LicEmStop(IN ALM_INT32 iPrdId);

/* 查询紧急配置及运行信息，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_LicEmQuery(IN ALM_INT32 iPrdId, INOUT ALM_SLIC_INFO_STRU *pstRslt);

/************************************************************************
 * 处理Stick
 ************************************************************************/
/* Stick配置 */
typedef struct {
    /* 指定支持Stick的BBOM及Stick下的授权值，不能为NULL */
    ALM_SLIC_BBOM_STRU *pstBboms;

    /* 指定支持Stick的BBOM的数目，必须大于0 */
    ALM_UINT32 uiNum;

    /* 每次启动允许运行的天数，默认30天，最大180天，若为0采用默认 */
    ALM_UINT32 uiAllowDays;

    /* 允许启动的次数，默认2次，最大2次，若为0采用默认 */
    ALM_UINT32 uiAllowCount;
} ALM_LIC_STICK_CFG_STRU;

/* 配置Stick */
LIC_EXPORT ALM_INT32 ALM_LicStickCfg(IN ALM_INT32 iPrdId, IN CONST ALM_LIC_STICK_CFG_STRU *pstCfg);

/*
 * 启动Stick。
 * 若之前已经启动，且没有调用恢复函数恢复运行，将恢复Stick，不消耗启动次数。
 * 恢复运行时，若发现已经超期，会再次启动并进入新的周期。
 */
LIC_EXPORT ALM_INT32 ALM_LicStickStart(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pbIsRecover);

/* 停止Stick */
LIC_EXPORT ALM_INT32 ALM_LicStickStop(IN ALM_INT32 iPrdId);

/* 查询Stick配置及运行信息，注意清理输出 */
LIC_EXPORT ALM_INT32 ALM_LicStickQuery(IN ALM_INT32 iPrdId, INOUT ALM_SLIC_INFO_STRU *pstRslt);

/* 查询ESN=ANY/DEMO/>3的信息 */
LIC_EXPORT ALM_INT32 ALM_LicEsnAnyQuery(IN ALM_INT32 iPrdId, OUT ALM_LIC_ESN_ANY_INFO_STRU *pstEsnAnyInfo);

/*
 * 设置临时项是否有效。
 */
LIC_EXPORT ALM_INT32 ALM_LicSetFixTimeSwitch(IN ALM_INT32 iPrdId, IN ALM_BOOL bFixTime);

LIC_EXPORT ALM_INT32 ALM_LicSetFixTimeGroupSwitch(IN ALM_INT32 iPrdId, ALM_CHAR *pcGroup, ALM_BOOL bFixTimeState);

/*
 * 获取临时项是否有效。
 */
LIC_EXPORT ALM_INT32 ALM_LicIsEnableFixTimeItem(IN ALM_INT32 iPrdId, OUT ALM_BOOL *pbOn);
LIC_EXPORT ALM_INT32 ALM_LicIsEnableFixTimeGroupItem(IN ALM_INT32 iPrdId, ALM_GROUP_FIX_STATE_RSLT_STRU *pstFixGroupState);

typedef struct {
    ALM_LIC_TYPE_ENUM eType;
    ALM_UINT32 uiFeatureId;
    ALM_CHAR acFeatureName[ALM_CONST_LIC_GROUP_NAME_MAX + 1];
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiGraceDays;
} ALM_LIC_FEATURE_STRU;

typedef struct {
    ALM_LIC_FEATURE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    /* 该字段表示是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_FEATURES_STRU;

LIC_EXPORT ALM_INT32 ALM_LicGetAllFeature(ALM_INT32 iPrdId, OUT ALM_LIC_FEATURES_STRU *pstFeatures);
LIC_EXPORT ALM_VOID ALM_StInitFeatures(INOUT ALM_LIC_FEATURES_STRU *pstFeatures);
LIC_EXPORT ALM_VOID ALM_StClsFeatures(INOUT ALM_LIC_FEATURES_STRU *pstFeatures);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */
#endif /* __ALM_INTF_LIC_MGR_H__ */
