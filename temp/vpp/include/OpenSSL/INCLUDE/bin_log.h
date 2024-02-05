/**
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * Filename      : bin_log.h
 * Author        : h00230179
 * Description   : This header file contains the function declarations
 *                 for the binary log module.
 * Create        : 2018/11/12
 * Version       : 1.0
*/

#ifndef BIN_LOG_H
#define BIN_LOG_H

#ifndef __SSL_NO_BIN_LOG__

#ifdef __cplusplus
extern "C" {
#endif

#define SSL_BIN_LOG_ENABLE 1

/**
 * @ingroup SSL_BINLOG_bin_log
 * @par description:
 *                  This constant indicates warning log type, configuration log type and
 *                  run time log type is disabled.
*/
#define SSL_BIN_LOG_DISABLE 0

/**
 * @ingroup SSL_BINLOG_bin_log
 * @par description:
 *                  This constant indicates that log callback function is called for the
 *                  warning log type.
*/
#define SSL_BIN_LOG_TYPE_WARNING 0x0001

/**
 * @ingroup SSL_BINLOG_bin_log
 * @par description:
 *                  This constant indicates that log callback function is called for the
 *                  configuration log type.
*/
#define SSL_BIN_LOG_TYPE_CFG 0x0010

/**
 * @ingroup SSL_BINLOG_bin_log
 * @par description:
 *                  This constant indicates that log callback function is called for the
 *                  run time log.
*/
#define SSL_BIN_LOG_TYPE_RUN 0x0100

/**
 * @ingroup SSL_BINLOG_bin_log
 * @par description:
 *                  You can set any one of the log level.
 *                  Based on the log level set by you, log callback functions are called.
 *                  Higher priority log level will automatically be enabled.
*/
typedef enum SSL_BIN_LOG_LEVEL {
    SSL_BIN_LOG_LEVEL_CRITICAL = 0, /* CRITICAL LOG LEVEL */
    SSL_BIN_LOG_LEVEL_ERROR,        /* ERROR LOG LEVEL */
    SSL_BIN_LOG_LEVEL_WARNING,      /* WARNING LOG LEVEL */
    SSL_BIN_LOG_LEVEL_INFO,         /* INFO LOG LEVEL */
    SSL_BIN_LOG_LEVEL_DEBUG,        /* DBG LOG LEVEL */
    SSL_BIN_LOG_LEVEL_END
} SSL_BIN_LOG_LEVEL_E;

/**
 * @ingroup SSL_BINLOG_bin_log
 * Function name: SSL_BINLOG_fix_len_fn
 * @par description:
 *                This is a callback function pointer for fixed length log.From the input
 *                arguments, user can identify the unique log code, log level, and log type.
 *                Using this function, user can get maximum of 4 different unsigned values.
 * Parameters   : @param[in] log_code Indicates unique log code [NA/NA]
 *                @param[in] log_level Indicates unique log level [SSL_BIN_LOG_LEVEL_E]
 *                @param[in] log_type Indicates unique log type.
 *                    [SSL_BIN_LOG_TYPE_WARNING/SSL_BIN_LOG_TYPE_CFG/SSL_BIN_LOG_TYPE_RUN]
 *                @param[in] format Indicates log message. Contains "%d", if any variable
 *                    argument is passed [NA/NA]
 *                @param[in] para1  First unsigned parameter [NA/NA]
 *                @param[in] para2  Second unsigned parameter [NA/NA]
 *                @param[in] para3  Third unsigned parameter [NA/NA]
 *                @param[in] para4  Fourth unsigned parameter [NA/NA]
 * Return Value : None
 * Note         : - If this callback is used in multi-threaded scenario, application need to
 *                   take care of synchronization of the logs.
 *                - ulPara1, ulPara2, ulPara3, and ulPara4 are unsigned parameters and while using
 *                   these parameters application need to typecast to appropriate data type.
 */
typedef void (*SSL_BINLOG_fix_len_fn)(unsigned int log_code, unsigned int log_level, unsigned int log_type,
                                      void *format, void *para1, void *para2, void *para3, void *para4);

/**
 * @ingroup SSL_BINLOG_bin_log
 * Function name: SSL_BINLOG_var_len_fn
 * @par description:
 *                This is a callback function pointer for variable length log.From the input
 *                arguments, user can identify the unique log code, log level, and log type.
 *                Using this function, user can get the string that is passed.
 * Parameters   : @param[in] log_code Indicates unique log code. [NA/NA]
 *                @param[in] log_level Indicates unique log level. [SSL_BIN_LOG_LEVEL_E]
 *                @param[in] log_type Indicates unique log type.
 *                    [SSL_BIN_LOG_TYPE_WARNING/SSL_BIN_LOG_TYPE_CFG/SSL_BIN_LOG_TYPE_RUN]
 *                @param[in] format Log Message. Contains "%s" in the message. [NA/NA]
 *                @param[in] data NULL Terminated string. [NA/NA]
 * Return Value : None
 * Note         : If this callback is used in multi-threaded scenario, application need to
 *                take care of synchronization of the logs.
 */
typedef void (*SSL_BINLOG_var_len_fn)(unsigned int log_code, unsigned int log_level, unsigned int log_type,
                                      void *format, void *data);

/**
 * @ingroup SSL_BINLOG_bin_log
 * @par Description:
 * This structure is used to set the log callback function in SSL Log.
 * binlog_fix_len_fn Specifies Fixed length log function pointer
 * binlog_var_len_fn Specifies Variable length log function pointer
 */
typedef struct SSL_BINLOG_func_st {
    /* Fixed length log function pointer */
    SSL_BINLOG_fix_len_fn binlog_fix_len_fn;

    /* Variable length log function pointer */
    SSL_BINLOG_var_len_fn binlog_var_len_fn;
} SSL_BINLOG_func_st;

/**
 * @defgroup SSL_BINLOG_reg_callback
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_BINLOG_reg_callback(const SSL_BINLOG_func_st *bin_log_func)
 * @endcode
 *
 * @par Purpose
 * This api is used to register the callback functions for the binary log.
 *
 * @par Description
 * This api is used to register the callback functions for the binary log. These
 * callback functions will be called for logging fixed/variable length log messages.
 *
 * @param[in] bin_log_func Pointer to struct of callback functions for binary log.[N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * bin_log.h
 * @par Note
 * This api should be called only in main thread at the beginning of the application.
 * @par Related Topics
 * SSL_BINLOG_set_log_level\n
 * SSL_BINLOG_get_log_level\n
 * SSL_BINLOG_set_log_type\n
 * SSL_BINLOG_get_log_type\n
 */
int SSL_BINLOG_reg_callback(const SSL_BINLOG_func_st *bin_log_func);

/**
 * @defgroup SSL_BINLOG_set_log_level
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_BINLOG_set_log_level(SSL_BIN_LOG_LEVEL_E eLevel)
 * @endcode
 *
 * @par Purpose
 * This api is used to set the current log level.
 *
 * @par Description
 * This api sets the current log level for binary log. Only messages with equal
 * or higher level than the current log level will be logged. If log level is set to
 * SSL_BIN_LOG_LEVEL_END, no message is logged.
 *
 * @param[in] eLevel The log level to be set, see enum SSL_BIN_LOG_LEVEL_E.[N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * bin_log.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_BINLOG_reg_callback\n
 * SSL_BINLOG_get_log_level\n
 * SSL_BINLOG_set_log_type\n
 * SSL_BINLOG_get_log_type\n
 */
int SSL_BINLOG_set_log_level(SSL_BIN_LOG_LEVEL_E eLevel);

/**
 * @defgroup SSL_BINLOG_get_log_level
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_BINLOG_get_log_level(SSL_BIN_LOG_LEVEL_E eLevel)
 * @endcode
 *
 * @par Purpose
 * This api is used to get the current log level.
 *
 * @par Description
 * This api obtains the current log level for binary log.
 *
 * @par Parameters
 * N/A
 *
 * @retval SSL_BIN_LOG_LEVEL_E The current log level, which could be one of these:
 * SSL_BIN_LOG_LEVEL_CRITICAL, SSL_BIN_LOG_LEVEL_ERROR,
 * SSL_BIN_LOG_LEVEL_WARNING, SSL_BIN_LOG_LEVEL_INFO,
 * SSL_BIN_LOG_LEVEL_DEBUG, and SSL_BIN_LOG_LEVEL_END.[SSL_BIN_LOG_LEVEL_E|N/A].
 *
 * @par Required Header File
 * bin_log.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_BINLOG_reg_callback\n
 * SSL_BINLOG_set_log_level\n
 * SSL_BINLOG_set_log_type\n
 * SSL_BINLOG_get_log_type\n
 */
SSL_BIN_LOG_LEVEL_E SSL_BINLOG_get_log_level(void);

/**
 * @defgroup SSL_BINLOG_set_log_type
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_BINLOG_set_log_type(unsigned int warn_enable,
 *                             unsigned int cfg_enable,
 *                             unsigned int run_enable)
 * @endcode
 *
 * @par Purpose
 * This api is used to set the warning, configuration, and runtime log type status.
 *
 * @par Description
 * This api sets the warning, configuration, and runtime log type status. By default,
 * all the log type status are set to SSL_BIN_LOG_ENABLE.
 *
 * @param[in] warn_enable Indicates whether to enable or disable warning log.
 *            SSL_BIN_LOG_ENABLE - Enable, SSL_BIN_LOG_DISABLE - Disable. [N/A]
 * @param[in] cfg_enable Indicates whether to enable or disable the configuration log.
 *            SSL_BIN_LOG_ENABLE - Enable, SSL_BIN_LOG_DISABLE - Disable. [N/A]
 * @param[in] run_enable Indicates whether to enable or disable the runtime log.
 *            SSL_BIN_LOG_ENABLE - Enable, SSL_BIN_LOG_DISABLE - Disable. [N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * bin_log.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_BINLOG_reg_callback\n
 * SSL_BINLOG_set_log_level\n
 * SSL_BINLOG_get_log_level\n
 * SSL_BINLOG_get_log_type\n
 */
int SSL_BINLOG_set_log_type(unsigned int warn_enable, unsigned int cfg_enable, unsigned int run_enable);

/**
 * @defgroup SSL_BINLOG_get_log_type
 * @ingroup sslFunctions
 * @par Prototype
 * @code
 * int SSL_BINLOG_get_log_type(unsigned int *pwarn_enable,
 *                             unsigned int *pcfg_enable,
 *                             unsigned int *prun_enable)
 * @endcode
 *
 * @par Purpose
 * This api is used to get the warning, configuration, and runtime log type status.
 *
 * @par Description
 * This api obtains the warning, configuration, and runtime log type status. By default,
 * all the log type status are set to SSL_BIN_LOG_ENABLE.
 *
 * @param[in] pwarn_enable Indicates the warning log type status.
 *            SSL_BIN_LOG_ENABLE - Enable, SSL_BIN_LOG_DISABLE - Disable. [N/A]
 * @param[in] pcfg_enable Indicates the configuration log type status
 *            SSL_BIN_LOG_ENABLE - Enable, SSL_BIN_LOG_DISABLE - Disable. [N/A]
 * @param[in] prun_enable Indicates the runtime log level status.
 *            SSL_BIN_LOG_ENABLE - Enable, SSL_BIN_LOG_DISABLE - Disable. [N/A]
 *
 * @retval int On success.[1|N/A].
 * @retval int On failure.[0|N/A].
 *
 * @par Required Header File
 * bin_log.h
 * @par Note
 * N/A
 * @par Related Topics
 * SSL_BINLOG_reg_callback\n
 * SSL_BINLOG_set_log_level\n
 * SSL_BINLOG_get_log_level\n
 * SSL_BINLOG_set_log_type\n
 */
int SSL_BINLOG_get_log_type(unsigned int *pwarn_enable, unsigned int *pcfg_enable, unsigned int *prun_enable);

#ifdef __cplusplus
}
#endif /* end of __cplusplus */

#endif /* end of __SSL_NO_BIN_LOG__ */
#endif /* end of BIN_LOG_H */
