/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : debug_log_macro.h
  版 本 号   : 初稿
  作    者   : zengrenchang (z00296903)
  生成日期   : 2017年6月14日
  最近修改   :
  功能描述   : debug_log宏定义
  函数列表   :
  修改历史   :
  1.日    期   : 2016年6月14日
    作    者   : zengrenchang (z00296903)
    修改内容   : DTS2017110402551 【优化问题】优化diagnose和discovery的调试打印日志 

******************************************************************************/
#ifndef __DEBUG_LOG_MACRO_H__
#define __DEBUG_LOG_MACRO_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 业务要求发布版本只打印DLOG_ERROR级别的日志, 其他级别日志字面量不编入二进制中.
 * 发布版本宏与非发布版本的区别, 在于CMAKE_BUILD_TYPE的参数, 以及_DEBUG / NDEBUG 宏
 */
#define BMC_LOG_MARCO(level, format, arg...) /*lint -save -e506 */             \
    do {                                                                       \
        debug_log_func((level), __FILE__, __LINE__, format, ##arg);            \
    } while (0) /*lint -restore */

// Release版本, 只保留INFO和ERROR日志
#ifdef BMC_RELEASE
#    undef debug_log
#    define debug_log(level, format, arg...) BMC_LOG_##level(format, ##arg)
#define BMC_LOG_DLOG_DEBUG(format, arg...)
#define BMC_LOG_DLOG_MASS(format, arg...)
#define BMC_LOG_DLOG_BUTT(format, arg...)
#    define BMC_LOG_DLOG_INFO(format, arg...)                                  \
        BMC_LOG_MARCO(DLOG_INFO, format, ##arg)
#    define BMC_LOG_DLOG_ERROR(format, arg...)                                 \
        BMC_LOG_MARCO(DLOG_ERROR, format, ##arg)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

