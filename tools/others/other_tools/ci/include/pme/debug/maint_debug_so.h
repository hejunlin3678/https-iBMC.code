/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : maint_debug_so.h
  版 本 号   : 初稿
  部    门   :
  作    者   : licheng
  生成日期   : 2013年8月15日
  最近修改   :
  功能描述   : 提供给应用模块的维护调测动态库
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月15日
    作    者   : licheng
    修改内容   : 创建文件

******************************************************************************/


#ifndef __MD_SO_H__
#define __MD_SO_H__

#ifdef __cplusplus
 #if __cplusplus
    extern "C" {
 #endif
#endif /* __cplusplus */

#define LOG_MAX_SZ  512

#define CONNECT_TO_CLI  ("connect_to_cli")
#define DECONNECT_TO_CLI    ("deconnect_to_cli")
#define DLOG_OUTYPE     ("dlog_outype")
#define DLOG_OUTLVL     ("dlog_outlvl")
#define SAVE_DLOG_CFG   ("save_dlog_cfg")
#define SET_TRACE_CFG   ("set_trace_cfg")

#define debug_log(level, format, arg...) /*lint -save -e506 */ \
do  \
{   \
    debug_log_func((level),  __FILE__, __LINE__, format, ##arg);  \
} while (0) /*lint -restore */

#ifdef PME_SUPPORT_FORMAT_WARNING
#define FMT_CHECK(m, n) __attribute__((format(printf, m, n)))
#else
#define FMT_CHECK(m, n)
#endif

typedef enum
{
    DLOG_ERROR,
    DLOG_INFO,
    DLOG_DEBUG,
    DLOG_MASS,
    DLOG_BUTT
} DLOG_LEVEL_E;

typedef enum
{
    SLOG_ERROR,
    SLOG_WARN,
    SLOG_INFO,
    SLOG_BUTT
} SLOG_LEVEL_E;

typedef enum
{
    MLOG_ERROR,
    MLOG_WARN,
    MLOG_INFO,
    MLOG_BUTT
} MLOG_LEVEL_E;

/* 日志类型 */
typedef enum
{
    LOG_TYPE_DEBUG,
    LOG_TYPE_OPERATE,
    LOG_TYPE_STRATEGY,
    LOG_TYPE_MASS_OPERATE,
    LOG_TYPE_MAINT,
    LOG_TYPE_BUTT
} LOG_TYPE_E;

/* 日志元素 */
#define LOG_TIME 0x01
#define LOG_MOD  0x02

extern gint32 init_md_so(void);

/* 不要使用dbg_log，使用debug_log宏 */
extern void dbg_log(DLOG_LEVEL_E level, const gchar *fmt, ...) FMT_CHECK(2, 3);

extern void debug_log_func(DLOG_LEVEL_E level, const gchar *file_name, guint32 line_num, 
    const gchar *fmt, ...) FMT_CHECK(4, 5);

extern void strategy_log(SLOG_LEVEL_E level, const gchar *fmt, ...) FMT_CHECK(2, 3);

extern void operation_log(const gchar * interface, const gchar *username, const gchar *client,
    const gchar *target, const gchar *fmt, ...) FMT_CHECK(5, 6);

extern void mass_operation_log(const gchar * interface, const gchar *username, const gchar *client,
    const gchar *target, const gchar *fmt, ...) FMT_CHECK(5, 6);

extern void maintenance_log(MLOG_LEVEL_E level, const gchar *fmt, ...) FMT_CHECK(2, 3);

/* App自定义日志 */
extern gint custom_log_init(gchar *log_name, guint32 package_size, guint64 privilege);
extern void custom_log(const gchar *log_name, guint8 component,
    const gchar *fmt, ...) FMT_CHECK(3, 4);

/*method_log 转供APP 记录方法调用，记录操作日志*/
extern void method_operation_log(GSList* caller_info, const gchar *target,
    const gchar *fmt, ...) FMT_CHECK(3, 4);
extern void proxy_method_operation_log(GSList* caller_info, const gchar *target,
    const gchar *fmt, ...) FMT_CHECK(3, 4);

extern void security_log(const gchar *fmt, ...) FMT_CHECK(1, 2);

extern void console_printf(const gchar *fmt, ...) FMT_CHECK(1, 2);
extern void debug_printf(const gchar *fmt, ...) FMT_CHECK(1, 2);

extern gint32 module_trace(const gchar* fmt, ...) FMT_CHECK(1, 2);
extern gint is_module_trace_enable(void);

/* 其他 */
extern gint32 get_method_operation_log_info(GSList* caller_info, const gchar **interface_nm,
                                     const gchar **usr_nm, const gchar **client);
extern gint32 clear_log(LOG_TYPE_E log_type);

/* 获取日志文件的压缩容量大小 */
extern gint32 get_log_file_package_size(gchar* log_name,  guint32* package_size);

/*获取dbg log日志级别*/
extern gint get_dbg_log_outlvl(void);

/* 将日志文件刷新到存储介质 */
extern void file_log_flush(void);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif



