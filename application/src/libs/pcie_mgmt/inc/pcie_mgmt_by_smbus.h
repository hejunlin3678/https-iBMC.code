/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2013. All rights reserved.
 * Description:
 * Author:
 * Create:
 * History: 创建文件
 */
#ifndef __PCIE_MGMT_BY_SMBUS_H__
#define __PCIE_MGMT_BY_SMBUS_H__

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


#pragma pack(1)


// SMBUS发送数据字段
typedef struct smbus_data {
    guint16 reserved;    // 保留字段
    guint16 command;     // 填充命令字
    guint32 offset;      // 填充偏移
    guint32 read_length; // 填充读取长度
} SMBUS_DATA;

// SMBUS发送数据带CRC字段
typedef struct smbus_data_req {
    SMBUS_DATA data; // 发送数据字段
    guint32 crc;     // 填充校验和
} SMBUS_SEND_REQ;

// SMBUS接受数据带头部
typedef struct smbus_data_rsp {
    guint16 error_code;   // 错误码
    guint16 opcode;       // 操作码
    guint32 total_length; // 总长度
    guint32 data_length;  // 数据长度
    guint8 data;          // 数据部分
} SMBUS_DATA_RSP;

// 获取能力数据
typedef struct smbus_data_capability {
    guint16 id;         // 识别码
    guint8 type;        // PCIE类型
    guint8 reserved;    // 保留字段
    guint16 opcode_num; // opcode数量
    guint16 opcodes;
} SMBUS_DATA_CAPABILITY;

#pragma pack()


#define SMB_CRC32_WIDTH (8 * sizeof(guint32)) /* in bits  */
#define SMB_CRC32_INIT_REMAIDER 0xFFFFFFFF
#define SMB_CRC32_FINAL_XOR_VALUE 0xFFFFFFFF

extern gint32 pm_get_chip_capability_by_smbus(PM_CHIP_INFO *input);
extern gint32 pm_get_chip_temp_by_smbus(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_firmawre_version_by_smbus(PM_CHIP_INFO *chip_info, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_optical_module_temp_by_smbus(PM_CHIP_INFO *chip_info, guint8 **buf, guint32 *buf_len);
extern gint32 pm_get_health_state_by_smbus(PM_CHIP_INFO *chip_info, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_fault_code_by_smbus(PM_CHIP_INFO *chip_info, guint8 **buf, guint32 *buf_len);
extern gint32 pm_get_log_by_smbus(PM_CHIP_INFO *chip_info, guint16 key, guint8 **buf, guint32 *buf_len);
/* * BEGIN: Modified by qinjiaxiang, 2018/5/14 问题单号:SR.SFEA02130923.001.001 修改原因: */
extern gint32 pm_get_netcard_link_status_by_smbus(PM_CHIP_INFO *chip_info, guint8 **buf, guint32 *buf_len);
extern gint32 pm_get_netcard_mac_address_by_smbus(PM_CHIP_INFO *chip_info, guint8 **buf, guint32 *buf_len);
/* * END:   Added by qinjiaxiang, 2018/5/14 */
gint32 pm_get_fixed_len_data_by_smbus(PM_CHIP_INFO *chip_info, guint16 opcode, gulong delay_time, guint8 *buf,
    guint32 buf_len);
gint32 pm_get_random_len_data_by_smbus(PM_CHIP_INFO *chip_info, guint16 opcode, gulong delay_time, guint8 **buf,
    guint32 *buf_len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PCIE_MGMT_BY_SMBUS_H__ */
