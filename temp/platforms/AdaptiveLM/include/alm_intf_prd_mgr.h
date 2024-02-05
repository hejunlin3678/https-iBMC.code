/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2015-2018. All rights reserved.
 * Description: 产品管理，处理添加删除以及查询产品状态等。
 * Author: AdaptiveLM team
 * Create: 2015-07-08
 */
#ifndef ALM_INTF_PRD_MGR_H
#define ALM_INTF_PRD_MGR_H

#include "alm_intf_def.h"
#include "alm_intf_lic_mgr.h"

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

/*
 * 描述: 添加产品，同时配置BBOM。持久化保密文件中会记录
 * 产品编号及版本。若添加产品的编号存在于持久文件中，
 * 且版本不相同时会做版本升级动作。启动后调用。
 * 为减少持久化文件的写入次数，建议一次性添加多个产品。
 * 添加的产品总数目不允许超出持久化配置的最大产品数目。
 * 参数:
 * pstPrds - 输入待添加产品，输出产品添加结果。
 * uiPrdNum - 所需添加产品数。
 * puiFailNum - 多少个产品未能成功添加到内存中。
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrAdd(INOUT ALM_PRD_STRU *pstPrds, IN ALM_UINT32 uiPrdNum, OUT ALM_UINT32 *puiFailNum);

/*
 * 描述: 激活License时，当Licene版本和产品版本匹配，并且产品有双秘钥，是否允许激活弱算法签名的License
 * 默认是允许；可以调用该函数来禁止；
 * 当前的弱算法是：RSA1024_SHA1、RSA2048_SHA1、ECC163_SHA1、ECC233_SHA1。
 * 当前的强算法是：RSA3072_SHA256、ECC283_SHA256。
 * 注意：弱算法会不断的增加，过一段时间后，当前的强算法也会变成弱算法；弱算法参考产品安全红线3.0要求。
 * 调用该函数时，需要先和产品SE确认：(1)是否有必要；(2)产品License的签名算法是什么。
 * 参数:
 * iPrdId - 配置的产品id。
 * bAllow - 是否允许，只能传ALM_TRUE、ALM_FALSE这两个值。
 * ALM_TRUE：允许，默认也是允许的；
 * ALM_FALSE：不允许，需要慎重。
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrAllowVerMatWeakAlgLic(IN ALM_INT32 iPrdId, IN ALM_BOOL bAllow);

/* 移除已添加产品。启动后调用。 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrDel(IN ALM_INT32 iPrdId);

/* 移除持久化内存产品信息,没有执行删除产品之前,  不能执行删除产品的保密信息 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrPsDel(IN ALM_INT32 iPrdId);

/*
 * 描述: 恢复运行已加载的License、及紧急、Stick等。
 * 若没有执行恢复，启动后调用启动紧急、Stick或激活License文件将为恢复过程。
 * 即:不消耗启动次数，且延续之前状态运行(会参考当前时间及环境进行刷新)。
 * 建议在完成启动过程、添加产品且完成紧急、Stick等相关配置后调用。
 * 对已恢复的产品再次调用不受影响。
 * 参数:
 * piPrdId - 指定需要恢复的产品；若为NULL恢复全部产品。
 * pstLic - 指定需要默认加载的License文件；若为NULL不加载。
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrRecover(IN CONST ALM_INT32 *piPrdId, INOUT ALM_LIC_ACTIVE_STRU *pstLics);

/* 使产品以默认值运行 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrDisable(IN ALM_INT32 iPrdId);

/* 查询指定ID的产品名称和版本信息、扩展信息 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrQuery(IN CONST ALM_INT32 iPrdId, OUT ALM_PRD_BASE_STRU *pstPrdInfo,
                                     INOUT ALM_PRD_EXTEND_INFO_STRU *pstPrdExtend);

/**
 * @ingroup AdaptiveLM
 * @brief 设置是否开启当添加产品传入的密钥信息校验失败时，使用第二KeyFile来校验。
 *
 * @attention 该功能默认不开启，若要开启该功能，可通过该API传ALM_TRUE。该API需要在ALM_Start之后调用。
 *
 * @li Memory operation: alloc、free、size:
 * -# 无内存分配</br>
 * @li Thread safe:
 * -# 非线程安全</br>
 * @li OS/CPU difference:
 * -# 无OS差异</br>
 * @li Time consuming:
 * -# 非耗时接口</br>
 *
 * @param bTurnOn [IN] 参数类型#ALM_BOOL，设置开关量。
 *
 * @retval #ALM_OK 0 成功。
 * @retval #其他返回值 失败 - 对应错误码详见API手册。
 */
LIC_EXPORT ALM_INT32 ALM_PrdMgrLoadSecondKeySwitch(IN ALM_BOOL bTurnOn);

#if (defined(ALM_PACK_4) || defined(ALM_PACK_8))
#pragma pack()
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* if __cplusplus */
#endif /* ifdef __cplusplus */

#endif /* ALM_INTF_PRD_MGR_H */
