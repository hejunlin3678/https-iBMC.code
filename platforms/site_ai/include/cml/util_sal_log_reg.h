/***********************************************************************
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 *
 * -----------------------------------------------------------------------
 * util_sal_log_reg.h
 *
 * Project Code:  NA
 * Module Name :  ppl
 * Create Date :  2020-03-04
 * Version     :  Initial Draft
 * Author      :  zhangziyang
 * Description :  定义日志注册对外接口
 * -----------------------------------------------------------------------
 * Modification History
 * DATE        NAME             DESCRIPTION
 * ----------------------------------------------------------------------
 * 2020-03-04  zhangziyang      creat
 ************************************************************************/

#ifndef DOPAI_UTIL_SAL_LOG_REG_H
#define DOPAI_UTIL_SAL_LOG_REG_H
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup logReg DOPAI日志回调接口 */
#define DOPAI_SAL_LOG_REG_API_C

/**
 * @ingroup  logReg
 * @brief DOPAI_SAL_LogReg用于向DOPAI注册记录日志的回调函数
 *
 * @par 描述：
 * 当用户需要使用自有日志模块接管SiteAI日志模块时，需要构造如下结构体，并调用日志注册接口DOPAI_SAL_LogRegister完成日志模块的替换
 *
 * @param mGetModId [IN] 获取模块ID的函数指针，SiteAI将会调用此接口获取SiteAI在用户侧的日志模块id，
 * 此id用于后续打印日志时进行模块区分
 *      SiteAI将通过入参modString传入"DOPAI", 来获取SiteAI在用户侧的日志模块id，用以后续的日志打印
 *      SiteAI将通过出参modeId获取"DOPAI"的日志id
 *      SiteAI认为此函数返回0时正常，其余值异常
 *
 * @param mLogStr [IN] 写日志的函数指针，SiteAI将会调用此接口进行日志输出
 *      SiteAI会通过入参level传入当前待打印日志的等级，传入的值从0-4依次表示此条日志等级为紧急、错误、警告、信息、调试，若用户的日志框架
 * level与SiteAI侧存在不一致，可通过相应手段进行转换；
 *      SiteAI会通过入参modId传入从mGetModId接口获取的模块Id；
 *      SiteAI会通过入参str传入待打印的日志字符串
 *      SiteAI认为此函数返回0时正常，其余值异常
 *
 * @param mGetLogBufSize [IN] 获取日志缓冲区大小的函数指针，SiteAI将通过此接口获取用户侧日志模块所支持的单条日志的最大字符长度
 *      SiteAI认为此函数返回值小于等于0时异常，其余值正常，同时若返回值超过SiteAI内部规定的最大日志长度，SiteAI将会以内部最大长度为限
 * @since SiteAI V100R021
 */
typedef struct tagDOPAI_SAL_LogReg {
    int32_t (*mGetModId) (const char *modString, uint32_t *modId);          /**< 获取日志模块ID回调函数 */
    int32_t (*mLogStr) (uint32_t level, uint32_t modId, const char *str);   /**< 写文本日志回调函数 */
    uint32_t (*mGetLogBufSize) (void);                                      /**< 获取日志buf大小回调函数 */
} DOPAI_SAL_LogReg;

/**
 * @ingroup  logReg
 * @brief 记录日志的注册日志接口
 *
 * @par 描述:
 * 记录日志的注册日志接口，调用成功后可启用注册的接口记录日志
 *
 * @attention 需要用户自定义注册日志[必备]
 * 需要在记录日志前调用(如pipeline启动前), 可重复注册，会使用最后一次注册回调函数记录日志
 *
 * @param  logInfo [IN]  记录日志的回调函数结构体
 *
 * @retval #UTIL_NO_ERROR 成功
 * @retval #UTIL_LOGREG_INFO_PTR_NULL 失败, 注册信息指针为空
 * @retval #UTIL_LOGREG_INFO_FUNC_PTR_NULL 失败, 注册信息的函数指针为空
 * @retval #UTIL_LOGREG_REG_FAIL 失败, 注册回调函数失败
 * @retval #UTIL_LOG_REG_GET_LOG_BUF_FAIL 失败, 获取日志bufer大小错误
 * @par 依赖
 * <ul><li>util_sal_log_reg.h：该接口声明所在的头文件。</li></ul>
 * @since XX
 */
int32_t DOPAI_SAL_LogRegister(DOPAI_SAL_LogReg *logInfo);

#ifdef __cplusplus
}
#endif

#endif /* DOPAI_SAL_LOG_REG_H */