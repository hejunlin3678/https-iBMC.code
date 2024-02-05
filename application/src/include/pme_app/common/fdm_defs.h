/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 故障诊断相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __FAULT_DIAGNOSE_DEFINE_H__
#define __FAULT_DIAGNOSE_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SYSTEMCOM_TAR_FILE_NAME "/tmp/systemcom.tar"
#define BLACKBOX_TAR_FILE_NAME "/tmp/blackbox.tar"
#define HEALTH_REPORT_JSON_PATH "/opt/pme/pram/diagnose_fdm_health_report.json"

#define SDI_BLACKBOX_TAR_FILE_NAME  "/tmp/sdi_blackbox.tar"

/* 如果snprintf要写入的字符串长度大于buffer的最大长度，则返回 */
#define IF_LE_MAXLEN_RETURN(strlen, maxbuffer) {  \
        if (strlen >= maxbuffer) {                \
            strlen = maxbuffer - 1;               \
            return strlen;                        \
        }                                         \
    }

/* 用4个空格分隔错误解析是，同一行内不同的内容 */
#define ITEM_DELIMITER_STRING "    "

#define BLACKBOX_DATA_SIZE 0x400000
#define SDI_BLACKBOX_DATA_SIZE 0x1000000

#define QUARTER_M_SIZE  (1024*256)
#define SOL_SYSTEMCOM   1

#ifndef ARM64_HI1711_ENABLED
#define SOL_TX_BUFFER_SIZE (2 * 1024 * 1024)
#define SOL_RX_BUFFER_SIZE (2 * 1024 * 1024)
#else
#define SOL_TX_BUFFER_SIZE (4 * 1024 * 1024)
#define SOL_RX_BUFFER_SIZE (1 * 1024 * 1024)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FAULT_DIAGNOSE_DEFINE_H__ */
