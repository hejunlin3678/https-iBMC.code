/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2013. All rights reserved.
 * Description:
 * Author:
 * Create:
 * History: 创建文件
 */

#ifndef __PCIE_MGMT__BY_I2C_H__
#define __PCIE_MGMT__BY_I2C_H__

#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_NUM_DATA_REGION 20
#define VERSION_REGION_LEN 6
#define HEADER_REGION_LEN 4
#define DATA_REGION_LEN 2
#define DATA_REGION_CONTENT 2
#define TEN 10


// 故障码列表
typedef enum Fault_Code_List {
    FAULT_CODE_4096 = 4096,
    FAULT_CODE_4097,
    FAULT_CODE_4098,
    FAULT_CODE_4099,
    FAULT_CODE_4100,
    FAULT_CODE_4101,
    FAULT_CODE_4102,
    FAULT_CODE_4103,
    FAULT_CODE_4104,
    FAULT_CODE_4105,
    FAULT_CODE_4106,
    FAULT_CODE_4107,
    FAULT_CODE_4108,
    FAULT_CODE_4109,
    FAULT_CODE_4110,
    FAULT_CODE_4111,
    FAULT_CODE_4112,
    INVALID_FAULT_CODE // 无效故障码
} FAULT_CODE_LIST;

// 错误码列表，与故障码对应
typedef enum Error_Code_List {
    ERROR_CODE_4096 = 0,
    ERROR_CODE_4097,
    ERROR_CODE_4098,
    ERROR_CODE_4099,
    ERROR_CODE_4100,
    ERROR_CODE_4101,
    ERROR_CODE_4102,
    ERROR_CODE_4103,
    ERROR_CODE_4104,
    ERROR_CODE_4105,
    ERROR_CODE_4106,
    ERROR_CODE_4107,
    ERROR_CODE_4108,
    ERROR_CODE_4109,
    ERROR_CODE_4110,
    ERROR_CODE_4111,
    ERROR_CODE_4112,
    INVALID_ERROR_CODE // 无效错误码
} ERROR_CODE_LIST;

// 故障事件描述
#define FAULT_EVENT_DESC_4096 "The clock is out of lock"
#define FAULT_EVENT_DESC_4097 "DDR 0 calibration failed"
#define FAULT_EVENT_DESC_4098 "DDR 1 calibration failed"
#define FAULT_EVENT_DESC_4099 "Abnormal work on DDR 0"
#define FAULT_EVENT_DESC_4100 "Abnormal work on DDR 1"
#define FAULT_EVENT_DESC_4101 "PCIE Link Failed"
#define FAULT_EVENT_DESC_4102 "Abnormal interruption on tm Model"
#define FAULT_EVENT_DESC_4103 "Abnormal interruption on prm"
#define FAULT_EVENT_DESC_4104 "Abnormal interruption on lz"
#define FAULT_EVENT_DESC_4105 "Abnormal interruption on hfm0"
#define FAULT_EVENT_DESC_4106 "Abnormal interruption on ift"
#define FAULT_EVENT_DESC_4107 "Abnormal interruption on ddr"
#define FAULT_EVENT_DESC_4108 "Abnormal interruption on rcm"
#define FAULT_EVENT_DESC_4109 "Abnormal interruption on pciea"
#define FAULT_EVENT_DESC_4110 "Abnormal interruption on hfm1"
#define FAULT_EVENT_DESC_4111 "Abnormal interruption on seu uncorrectable"
#define FAULT_EVENT_DESC_4112 "Abnormal interruption on seu correctable"


// Header Region
#pragma pack(1)
typedef struct Header_Region {
    guint16 key;
    guint16 data_offset;
    // guint16 offset_num;
} HEADER_REGION;
#pragma pack()

// Version Region
#pragma pack(1)
typedef struct Version_Region {
    guint8 type;
    guint8 header_num;
    guint16 header_offset;
    // guint16 offset_num;
} VERSION_REGION;
#pragma pack()

extern gint32 pm_init_data_by_i2c(PM_CHIP_INFO *input);
extern gint32 pm_get_temp_by_i2c(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_sn_by_i2c(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_firmware_by_i2c(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_fault_code_by_i2c(PM_CHIP_INFO *input, guint8 **buf, guint32 *buf_len);
extern gint32 pm_get_board_id_by_i2c(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_pcb_id_by_i2c(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_health_state_by_i2c(PM_CHIP_INFO *input, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_chip_reset_time_by_i2c(PM_CHIP_INFO *chip_info, guint8 *buf, guint32 buf_len);
extern gint32 pm_get_ddr_temp_by_i2c(PM_CHIP_INFO *chip_info, guint8 **buf, guint32 *buf_len);
extern gint32 pm_get_shell_id_by_i2c(PM_CHIP_INFO *chip_info, guint8 *buf, guint32 buf_len);
gint32 pm_get_fixed_len_data_by_key(PM_CHIP_INFO *chip_info, const guint16 key_value, guint8 *buf,
    guint32 buf_len);
gint32 pm_get_data_by_key(PM_CHIP_INFO *chip_info, const guint16 key_value, guint8 **buf, guint32 *buf_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PCIE_MGNT_H__ */
