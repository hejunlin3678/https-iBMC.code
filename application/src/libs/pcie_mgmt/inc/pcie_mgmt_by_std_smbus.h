/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2013. All rights reserved.
 * Description:
 * Author: h00422363
 * Create:
 * History: h00422363 创建文件
 */
#ifndef __PCIE_MGMT_BY_STD_SMBUS_H__
#define __PCIE_MGMT_BY_STD_SMBUS_H__

#include <glib.h>
#include "pme/common/mscm_macro.h"
#include "pme/pme.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// 错误码
#define ERROR_CODE_FOR_SUCCESS 0            // Success
#define ERROR_CODE_FOR_OPCODE_NOT_SUPPORT 1 // Opcode Not Support
#define ERROR_CODE_FOR_PARAMETER_ERROR 2    // Parameter Error
#define ERROR_CODE_FOR_INTERNAL_ERROR 3     // Internal Error
#define ERROR_CODE_FOR_CRC_ERROE 4          // CRC Error

#define DATA_MAX_NUM 32     // 数据区最大长度
#define HEADER_LEN 12       // 数据头长度
#define CRC32_LEN 4         // 校验码长度
#define REQUEST_DATA_LEN 16 // 请求数据长度

#define BYTE_COUNT 1              // 读取的数据的byte count长度
#define PEC_LEN 1                 // pec校验码长度
#define SMBUS_REQ_COMMAND_CODE 0x20 // smbus读/写请求命令码
#define SMBUS_RSP_COMMAND_CODE 0x21 // smbus读/写响应命令码
#define SMBUS_LAST_FRAME_MASK (1 << 7)
#define MCU_SMBUS_HEADER_SIZE 12
#define MCU_SMBUS_FRAME_MAXSIZE 32
#define MCU_SMBUS_PAYLOAD_MAXSIZE (MCU_SMBUS_FRAME_MAXSIZE - MCU_SMBUS_HEADER_SIZE)
#define RESULT_STR_MAX_LEN 512
#define SSL_TYPE_LEN 5  // ssltype和证书长度5字节
#define SECUREBOOT_LEN 1  // secureboot长度1字节

#define MAX_FRAME_LEN 0xffffffff // 变长数据存放的数据长度
#define DFT_STATUS_UNSTART 0     // 装备测试未启动
#define DFT_STATUS_TESTING 1     // 装备测试中

/* BEGIN: Modified by h00422363, 2018/7/28 14:17:11   问题单号:AR.SR.SFEA02130917.004.001 支持一键导出MCU的日志 */
#define MCU_ERROR_LOG_MAX_LEN (200 * 1024)      // MCU故障日志大小限制200K
#define MCU_OPERATE_LOG_MAX_LEN (100 * 1024)    // MCU操作日志大小限制100K
#define MCU_MAINTAINCE_LOG_MAX_LEN (100 * 1024) // MCU维护日志大小限制100K
/* END:   Modified by h00422363, 2018/7/28 14:17:37 */

#pragma pack(1)


// 获取能力数据
typedef struct std_smbus_data_capability {
    guint16 id;         // 识别码
    guint8 type;        // PCIE类型
    guint16 opcode_num; // opcode数量
    guint16 opcodes;
} STD_SMBUS_DATA_CAPABILITY;


// 标准SMBUS协议发送读/写请求
typedef struct std_smbus_req {
    guint8 cmd_code;
    guint8 lun;                // 具体含义参见PCIE带外管理规范
    guint8 arg;                // opcode的补充参数
    guint16 opcode;            // 操作命令字
    guint32 offset;            // 请求数据偏移
    guint32 data_length;       // 请求数据长度
    guint8 data[DATA_MAX_NUM]; // 请求数据
} STD_SMBUS_REQ;

// 标准SMBUS协议读响应数据带头部
typedef struct std_smbus_rsp {
    guint16 error_code;   // 错误码
    guint16 opcode;       // 操作码
    guint32 total_length; // 总长度
    guint32 data_length;  // 数据长度
    guint8 data;          // 数据部分
} STD_SMBUS_RSP;

enum {
    STD_SMBUS_LAST_FRAME = 0x80,  // 最后一帧请求数据
    STD_SMBUS_MIDDLE_FRAME = 0x00 // 数据未写完
};


#pragma pack()

extern gint32 pm_get_chip_capability_by_std_smbus(PM_CHIP_INFO *chip_info);
extern gint32 pm_get_fixed_len_data_by_std_smbus(PM_CHIP_INFO *chip_info, guint8 *head_para, guint8 *buf,
    guint32 buf_len);
extern gint32 pm_get_random_len_data_by_std_smbus(PM_CHIP_INFO *chip_info, guint8 *head_para, guint8 **buf,
    guint32 *buf_len);
extern gint32 pm_send_data_by_std_smbus(PM_CHIP_INFO *chip_info, guint8 *head_para, const guint8 *buf, guint32 buf_len);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PCIE_MGMT_BY_STD_SMBUS_H__ */
