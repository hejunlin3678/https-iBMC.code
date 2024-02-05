/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: 为简化文件关系，收纳了多个模块用到的结构定义。
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
 * 定义存储License文件相关结构。
 ************************************************************************/
/*
 * 描述: 指定有效期。有效期采用截止日期及可运行天数表示。
 *   可以同时指定，也可以仅指定其中一项。若都没有指定，则无有效期。
 *   商用及调测License都仅指定了截止日期。
 *   用于管理Stick/紧急的虚拟License，则仅指定了相对天数。
 * 成员:
 * acDeadLine - 截止日期，在此日期的下一天License超出有效期。空字符串表示
 *   不指定截止日期。
 *   若为永久此值为常量ALM_CONST_LIC_DEADLINE_PERM对应的字符串。
 * uiRelDays - 允许运行的天数。若为0，不指定该值。
 */
typedef struct {
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiRelDays;
} ALM_LIC_TIME_VALID_INFO_STRU;

/*
 * 描述: 时间限制。
 * 成员:
 * 1. stValid - 有效期。
 * 2. uiGraceDays - 宽限允许天数，若为0，无宽限。
 */
typedef struct {
    ALM_LIC_TIME_VALID_INFO_STRU stValid;
    ALM_UINT32 uiGraceDays;
} ALM_LIC_TIME_INFO_STRU;

/* 定义BBOM类型 */
typedef enum {
    /* 资源型类型 */
    ALM_LIC_BBOM_RESOURCE,

    /* 功能型类型，取值0、非0 */
    ALM_LIC_BBOM_FUNCTION
} ALM_LIC_BBOM_TYPE_ENUM;

/* License文件类型 */
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

/* 保存单个ESN */
typedef struct {
    ALM_CHAR acEsn[ALM_CONST_LIC_SINGLE_ESN_LEN_MAX + 1];
} ALM_LIC_ESN_STRU;

/* 保存多个ESN */
typedef struct {
    ALM_LIC_ESN_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_ESNS_STRU;

/* 保存指定lsn的失效码 */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_CHAR acRvkTicket[ALM_CONST_RVK_TIKET_LEN + 1];
} ALM_LIC_LSN_RVK_TICKET_STRU;

/* 保存失效码 */
typedef struct {
    ALM_CHAR acRvkTicket[ALM_CONST_RVK_TIKET_LEN + 1];
    ALM_CHAR acRvkTime[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_RVK_TICKET_STRU;

/* 用于保存License文件通用信息 */
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
     * 若为ESN ANY/DEMO/有效期大于3天且非永久的特定License，此值为有效期天数；
     * 否则为0。
     * 有效期为截止日期 - 创建时间，其中时间都只精确到天(忽略时分秒)。
     * 在产品的单个版本周期中，全部这一类License的已有效运行时间会被叠加记录，
     * 版本变更重置为0。
     * 若此值不为0，此值只有不小于积累的已有效运行时间才允许被激活；
     * 否则无法激活，即便还未过截止日期、或者还存在宽限，都不允许激活。
     */
    ALM_UINT32 uiAnyDemoDays;

    ALM_BOOL bHaveRvked;
    ALM_LIC_RVK_TICKET_STRU stRvkcTiket;
} ALM_LIC_STORE_GENERAL_STRU;

/* 描述: 存储license中bbom信息。 */
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

/* 包装多个bbom info对象，方便操作 */
typedef struct {
    ALM_LIC_STORE_BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_BBOMS_STRU;

/* 保存SBOM下BBOM */
typedef struct {
    ALM_LIC_STORE_BBOM_STRU stBbom;
} ALM_LIC_STORE_S2BBOM_STRU;

/* 保存SBOM下BBOM */
typedef struct {
    ALM_LIC_STORE_S2BBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_S2BBOMS_STRU;

/* 保存Sbom的Values */
typedef struct {
    ALM_UINT64 ulVal;
    ALM_CHAR acDeadline[ALM_CONST_DATE_LEN + 1];
    ALM_BOOL bIsnoControl;
} ALM_LIC_STORE_SBOM_VALUE_STRU;

/* 保存Sbom的Values */
typedef struct {
    ALM_LIC_STORE_SBOM_VALUE_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_SBOM_VALUES_STRU;

/* 商业模式:指定的宏值和规范中给的值保持一致 */
typedef enum {
    ALM_BUSINESS_MODE_NONE = 0,
    ALM_PERPETUAL_LICENSE = 1,
    ALM_SNS = 2,
    ALM_ASRF = 3,
    ALM_TBL = 4,
    ALM_SUS = 5,
    ALM_TBL_PERPETUAL = 7
} ALM_BUSINESS_MODE_ENUM;

/* 保存License文件单个SBOM信息 */
typedef struct {
    ALM_CHAR *pcName;
    /* 年费截止日期 */
    ALM_CHAR *pcAnnualFeeValidDate;
    /* 商业模式, 如果license文件中没有提供BusinessModel属性, 则为NONE */
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
     * 保存在stS2Bboms里所有S2B中，属性quoteUnitEN与成员pcEngUnit相同的S2B,
     * 或者stS2Bboms中第一个S2B(不存在前者相同的情况) 的名字和对应的系数 */
    ALM_CHAR *pcS2BName;
    float amount;
} ALM_LIC_STORE_SBOM_STRU;

/* 保存License文件SBOM信息 */
typedef struct {
    ALM_LIC_STORE_SBOM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_SBOMS_STRU;

/* 保存PackageItem的Value */
typedef struct {
    ALM_UINT64 ulVal;
    ALM_CHAR acDeadline[ALM_CONST_DATE_LEN + 1];
    ALM_UINT32 uiReserved;
} ALM_LIC_STORE_PACKAGEITEM_VALUE_STRU;

/* 保存PackageItem的Values */
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

/* 保存PackageItem */
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

/* 保存SBOM的所属包 */
typedef struct {
    ALM_LIC_STORE_PACKAGEITEM_STRU *pstArray;
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
} ALM_LIC_STORE_PACKAGEITEMS_STRU;

typedef struct {
    ALM_CHAR acOfferName[ALM_CONST_LIC_OFFERING_NAME_MAX + 1];
    ALM_CHAR acOfferVer[ALM_CONST_LIC_OFFERING_VER_MAX + 1];
    /* GA时间 */
    ALM_CHAR acGADate[ALM_CONST_DATE_LEN + 1];
    /* EOFS时间 */
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
 * 解析License文件可能使用到的算法: 宏定义的值不能变
 */
typedef enum {
    ALM_ALG_LIC_PARSER_RSA_2048 = 0,
    ALM_ALG_LIC_PARSER_RSA_1024 = 1,
    ALM_ALG_LIC_PARSER_EC2N_163 = 2,
    ALM_ALG_LIC_PARSER_EC2N_233 = 3,
    ALM_ALG_LIC_PARSER_EC2N_283 = 4,
    ALM_ALG_LIC_PARSER_RSA_3072 = 5
} ALM_ALG_LIC_PARSER_ALG_ENUM;

/* 保存License文件附加信息 */
typedef struct {
    ALM_CHAR acFileCopyRight[ALM_CONST_LIC_COPY_RIGHT_MAX + 1];
    ALM_CHAR acFileFormatVersion[ALM_CONST_LIC_FILE_VER_LEN_MAX + 1];
    ALM_CHAR acLicFileType[ALM_CONST_LIC_FILE_TYPE_LEN_MAX + 1];
    ALM_CHAR acFileCreator[ALM_CONST_LIC_CREATER_MAX + 1];
    ALM_CHAR acFileIssuer[ALM_CONST_LIC_ISSUER_MAX + 1];
    ALM_CHAR acFileCreateTime[ALM_CONST_DATE_LEN + 1];
    /* 软件免费升级日期(软件年费截止日期) */
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
    /* 软件维护截止日期 */
    ALM_CHAR acSwMaintainDueDate[ALM_CONST_DATE_LEN + 1];
    /* 硬件维护截止日期 */
    ALM_CHAR acHwMaintainDueDate[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_STORE_ADDITION_STRU;

/* 用于保存完整的License文件的结构体 */
typedef struct {
    ALM_LIC_STORE_GENERAL_STRU stGeneralInfo;
    ALM_LIC_STORE_BBOMS_STRU stBbomInfo;
    ALM_LIC_STORE_OFFERS_STRU stOfferInfo;
    ALM_LIC_STORE_ADDITION_STRU stAddtion;
    /* 该字段表示是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_STORE_STRU;

/* 用于保存License文件的属性结构体 */
typedef struct {
    ALM_CHAR acLsn[ALM_CONST_LIC_LSN + 1];
    ALM_ENUM(ALM_LIC_TYPE_ENUM) eLicType;
    ALM_CHAR acFileFormatVersion[ALM_CONST_LIC_FILE_VER_LEN_MAX + 1];
} ALM_LIC_ATTRI_STRU;

/**
 * 描述：流单位。
 * 1. uiStreamSize - 流的大小。
 * 2. pcStream  - 流的初始位置
 */
typedef struct TAG_ALM_LIC_STREAM_VAR_STRU {
    ALM_UINT32 uiStreamSize;
    ALM_CHAR *pcStream;
} ALM_LIC_STREAM_VAR_STRU;

typedef ALM_LIC_STREAM_VAR_STRU ALM_LIC_RES_LIST;
typedef ALM_LIC_STREAM_VAR_STRU ALM_LIC_FUN_LIST;

/**
 * 描述:存储Feature段相关信息。
 * acAtrrib--目前存license文件类型，截止日期，宽限期
 */
typedef struct {
    ALM_CHAR acFilePrdName[ALM_CONST_PRD_NAME_LEN_MAX + 1];
    ALM_CHAR acFilePrdVer[ALM_CONST_PRD_VER_MAX + 1];
    ALM_CHAR acFeatName[ALM_CONST_LIC_GROUP_NAME_MAX + 1];
    ALM_CHAR acEsn[ALM_CONST_LIC_ESN_FEAT_FORMAT_LEN_MAX + 1];
    ALM_CHAR acAtrrib[ALM_CONST_LIC_ATRRIBULT_MAX + 1];
} ALM_LIC_FEAT_ATRRIBULT_STRU;

/**
 * 描述: 存储单个Feature信息。
 */
typedef struct {
    ALM_LIC_FEAT_ATRRIBULT_STRU stFeatAtrrib;
    ALM_LIC_RES_LIST stResList;
    ALM_LIC_FUN_LIST stFunList;
} ALM_LIC_FEAT_INFO_STRU;

/* 用于存储完整的Feature信息 */
typedef struct {
    ALM_UINT32 uiNum;
    ALM_UINT32 uiCap;
    ALM_LIC_FEAT_INFO_STRU *pstArray;
    /* 该字段表示是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_LIC_FEATS_INFO_STRU;

/* 单个spart信息 */
typedef struct {
    ALM_CHAR *pcSpartName;      /* spart名称 */
    ALM_UINT64 ulSpartPlanVal;  /* spart规划量 */
    ALM_UINT64 ulSpartUsageVal; /* spart使用量 */
} ALM_S2B_ONE_SPART_RSLT_STRU;

/* spart信息 */
typedef struct {
    ALM_S2B_ONE_SPART_RSLT_STRU *pstArray; /* spart数组 */
    ALM_UINT32 uiNum;                      /* 数组大小 */
} ALM_S2B_SPART_RSLT_STRU;

/* 用于初始化查询输出的ALM_LIC_STORE_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StInitLicStore(INOUT ALM_LIC_STORE_STRU *pstLic);

/* 清理ALM_LIC_STORE_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsLicStore(INOUT ALM_LIC_STORE_STRU *pstLic);

/* 初始化ALM_LIC_FEATS_INFO_STRU结构体 */
LIC_EXPORT ALM_VOID ALM_StInitLicFeatsInfo(INOUT ALM_LIC_FEATS_INFO_STRU *pstLicFeatsInfo);

/* 清理ALM_LIC_FEATS_INFO_STRU结构体 */
LIC_EXPORT ALM_VOID ALM_StClsLicFeatsInfo(INOUT ALM_LIC_FEATS_INFO_STRU *pstLicFeatsInfo);

/* 简易License单个BBOM值配置 */
typedef struct {
    /* 若为ALM_LIC_BBOM_FUNCTION类型，只能为0或1 */
    ALM_UINT64 ulValue;

    /* Bbom类 */
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;

    /* 本字段的配置只影响紧急接口的配置 */
    /* 如果希望配置的Bbom只能在默认状态下启动紧急(RM201606076800)，则需要配置成ALM_FALSE，其他情况下都配置成ALM_TRUE */
    ALM_BOOL bNormalEm;

    /* Bbom名，注意保证以'\0'结尾 */
    ALM_CHAR acBBomName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];
} ALM_SLIC_BBOM_STRU;

/* 用于保存紧急或STICK配置及运行状态信息 */
typedef struct {
    /* 否正在运行 */
    ALM_BOOL bIsRun;

    /* 允许启动的次数，包括免费 */
    ALM_UINT32 uiStartTotal;

    /* 紧急免费启动次数，非紧急不需要关注此字段 */
    ALM_UINT32 uiFreeStartCount;

    /* 已启动次数 ，已消耗的总次数，包括免费 */
    ALM_UINT32 uiAlreadyStartCount;

    /* 剩余时间，单位为分钟 */
    /* 由于状态自动刷新存在时间间隔，此处数据并不精确 */
    /* 若未配置固定点数刷新，此数据可以精确到小时，建议超出一小时部分做一小时计算 */
    /* 若配置了每天固定点数刷新，此数据仅能精确到天，建议超出一天部分做一天计算 */
    ALM_UINT32 uiRemainMins;
    ALM_UINT32 uiTotalMins;
    ALM_UINT32 uiValidPassMins;
    ALM_CHAR acEnterTime[ALM_CONST_DATE_LEN + 1];

    /* 配置的BBOM列表 */
    ALM_SLIC_BBOM_STRU *pstBboms;

    /* 配置的BBOM数目 */
    ALM_UINT32 uiBbomNum;

    /* 该结构体是否初始化标志 */
    ALM_UINT32 uiInitFlag;
} ALM_SLIC_INFO_STRU;

/* 用于初始化查询输出的ALM_SLIC_INFO_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StInitSlicInfo(INOUT ALM_SLIC_INFO_STRU *pstInfo);

/* 用于清理查询得到的ALM_SLIC_INFO_STRU对象 */
LIC_EXPORT ALM_VOID ALM_StClsSlicInfo(INOUT ALM_SLIC_INFO_STRU *pstInfo);

/* 用于保存ESN=ANY/DEMO/>3的信息 */
typedef struct {
    /* 当前激活的LIC 文件是否为超过三天的Esn + Any的DEMO 文件 */
    ALM_BOOL bIsUsing;

    /* 是否激活过ESN=ANY的License */
    ALM_BOOL bHadActived;

    /* 剩余天数 */
    ALM_UINT32 uiRemainDays;

    /* 已运行天数 */
    ALM_UINT32 uiRunningDays;

    /* ESN=ANY+DEMO+大于3天的license的有效天数的截止日期 */
    ALM_CHAR acDeadLine[ALM_CONST_DATE_LEN + 1];
} ALM_LIC_ESN_ANY_INFO_STRU;

/************************************************************************
 * 定义存储产品信息相关结构。
 ************************************************************************/
/* 产品基本信息 */
typedef struct {
    ALM_INT32 iPrdId;
    ALM_CHAR acPrdName[ALM_CONST_PRD_NAME_LEN_MAX + 1];

    /*
     * 产品版本，用于校验License中指定版本是否一致，及绑定使用记录。
     * 校验License时，只需要产品版本以License中版本起头即可。
     * 此值变动，会重置紧急及Stick的已使用次数为0，也会还原ESN ANY、
     * 有效期超3天且非永久的调测License的已使用天数为0。
     */
    ALM_CHAR acPrdVer[ALM_CONST_PRD_VER_MAX + 1];
} ALM_PRD_BASE_STRU;

/* 产品扩展信息 */
typedef struct {
    /* 是否开启时间安全性检测标示 */
    ALM_BOOL bSafeTimeEnable;

    /* 部分bbom过期时是否使用最小值标志 */
    ALM_BOOL bUseMinValInPartOver;

    /* 是否启用固定期限 */
    ALM_BOOL bFixTimeSupport;

    /* 是否限制ESN或版本不匹配只激活一次 */
    ALM_BOOL bEsnVerMisOnce;

    /* 是否兼容R5C01比较接口的校验规则 */
    ALM_BOOL bForceCmpSupport;

    /* 是否兼容R5C01校验次要错误 */
    ALM_BOOL bVerifyMinor;
} ALM_PRD_EXTEND_INFO_STRU;

/* 产品BBOM基本信息 */
typedef struct {
    /* BBOM编号，不允许重复 */
    ALM_INT32 iBbomId;

    /* 指定BBOM类型 */
    ALM_ENUM(ALM_LIC_BBOM_TYPE_ENUM) eType;

    /*
     * 默认值，在当前未激活任何License、也未启动紧急及Sick时，或者该BBOM不存在
     * 于已激活的License中、也没有配置到紧急或Stick中时，BBOM的授权值采用默认值。
     * 若为功能项(ALM_LIC_BBOM_FUNCTION)，只能取值0或1。
     */
    ALM_UINT64 ulDfltVal;

    /* 在当前授权值小于该值时，授权值置为此值，并发送warning级别的日志提示。 */
    /* 若为0忽略。若为功能项(ALM_LIC_BBOM_FUNCTION)，只能取值0或1。 */
    ALM_UINT64 ulMinVal;

    /* 在当前授权值大于该值时，授权值置为此值，并发送warning级别的日志提示。 */
    /* 若为0忽略。若为功能项(ALM_LIC_BBOM_FUNCTION)，只能取值0或1。 */
    ALM_UINT64 ulMaxVal;

    /* BBOM名称，不允许重复 */
    ALM_CHAR acName[ALM_CONST_LIC_BBOM_NAME_MAX + 1];
} ALM_PRD_BBOM_STRU;

/* 存储产品及BBOM配置 */
typedef struct {
    /* 产品编号、名称及版本 */
    ALM_PRD_BASE_STRU stBase;

    ALM_PRD_EXTEND_INFO_STRU stExInfo;

    /* 配置产品BBOM并指定默认值，不允许为空 */
    ALM_PRD_BBOM_STRU *pstBboms;

    /* 产品BBOM数目，必须大于0 */
    ALM_UINT32 uiBbomNum;

    /* License验证密钥，C风格字符串，以'\0'表示结尾。若为NULL，暂不设置密钥 */
    ALM_CHAR *pcPrdKey;

    /* 添加该产品时的返回值，ALM_OK表示添加成功 */
    ALM_INT32 iRet;
} ALM_PRD_STRU;

typedef struct {
    ALM_CHAR acGroup[ALM_CONST_LIC_BBOMGROUP_NAME_MAX + 1]; /* group名称 */
    ALM_BOOL bOn; /* ON/OFF */
} ALM_GROUP_FIX_STATE_STRU;

typedef struct {
    ALM_UINT32 uiNum;                                              /* 数组大小 */
    ALM_GROUP_FIX_STATE_STRU stArrays[ALM_CONST_FIXTIME_GROUPNUM]; /* 数组 */
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
