/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
* Description: 存储设备管理相关的定义。
* Author: h00282621
* Create: 2020-3-10
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __STORAGE_DEFINE_H__
#define __STORAGE_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PANGEA_BOARD_ID_XPU 0xa3
#define PANGEA_BOARD_ID_CST0221V6 0xa9
#define PANGEA_BOARD_ID_WAFER 0xf5

#define RAID_MGNT_SUPPORT       1
#define RAID_MGNT_UNSUPPORT     0

#define SM_SOFT_RAID_CONTROLLER_TYPE_ID  0xC0     /* 软RAID虚拟控制器的TypeId */


/************存储设备设置相关返回码******************/
#define SM_CODE_SUCCESS                         0x0
#define SM_CODE_BASE                            0x2000
#define SM_CODE_INVALID_RAID_LEVEL              (SM_CODE_BASE + 0x80)
#define SM_CODE_INVALID_SPAN_DEPTH              (SM_CODE_BASE + 0x81)
#define SM_CODE_INVALID_PD_COUNT                (SM_CODE_BASE + 0x82)
#define SM_CODE_INVALID_ARRAY_ID                (SM_CODE_BASE + 0x83)
#define SM_CODE_INVALID_LD_ID                   (SM_CODE_BASE + 0x84)
#define SM_CODE_INVALID_PD_ID                   (SM_CODE_BASE + 0x85)
#define SM_CODE_CAPACITY_OUT_OF_RANGE           (SM_CODE_BASE + 0x86)
#define SM_CODE_STRIP_SIZE_OUT_OF_RANGE         (SM_CODE_BASE + 0x87)
#define SM_CODE_READ_POLCIY_OUT_OF_RANGE        (SM_CODE_BASE + 0x88)
#define SM_CODE_WRITE_POLICY_OUT_OF_RANGE       (SM_CODE_BASE + 0x89)
#define SM_CODE_IO_POLICY_OUT_OF_RANGE          (SM_CODE_BASE + 0x8A)
#define SM_CODE_ACCESS_POLICY_OUT_OF_RANGE      (SM_CODE_BASE + 0x8B)
#define SM_CODE_DISK_CAHCE_POLICY_OUT_OF_RANGE  (SM_CODE_BASE + 0x8C)
#define SM_CODE_INIT_TYPE_OUT_OF_RANGE          (SM_CODE_BASE + 0x8D)
#define SM_CODE_LD_NAME_INVALID_ASCII           (SM_CODE_BASE + 0x8E)
#define SM_CODE_LD_NAME_EXCEED_MAX_LEN          (SM_CODE_BASE + 0x8F)
#define SM_CODE_UPGRADE_IN_PROGRESS             (SM_CODE_BASE + 0x90)
#define SM_CODE_NO_SPACE                        (SM_CODE_BASE + 0x91)
#define SM_CODE_OPERATION_IN_PROGRESS           (SM_CODE_BASE + 0x92)
#define SM_CODE_GET_PD_LIST_FAILED              (SM_CODE_BASE + 0xA0)
#define SM_CODE_PD_NOT_IDENTIFIED               (SM_CODE_BASE + 0xA1)
#define SM_CODE_BUSY                            (SM_CODE_BASE + 0xC0)
#define SM_CODE_PARA_DATA_ILLEGAL               (SM_CODE_BASE + 0xCC)
#define SM_CODE_INVALID_OPERTATION              (SM_CODE_BASE + 0xC1)
#define SM_CODE_NOT_SUPPORT_IN_PRESENT_STATE    (SM_CODE_BASE + 0xD5)
#define SM_CODE_NOT_SUPPORT_RAID_MGNT           (SM_CODE_BASE + 0xD6)


#define GET_SILK_BY_LIGHT_UP     0
#define GET_SILK_BY_BDF          2
#define GET_SILK_BY_BDF_PORT     3
#define GET_SILK_BY_LIGHT_UP_NEW 4

#define IPMI_GET_SILK_LOC_NAME_CMD_LEN   7

#define SD_CONTROLLOR            ("SD Controller")
#define FINISH_SD_UPGRADE        1
#define NO_FINISH_SD_UPGRADE     0

#define DEVICE_NAME_PREFIX_M2      "M.2"
#define DEVICE_NAME_PREFIX_SATADOM "SATA DOM"
#define RF_PROPERTY_PERCENTAGE_USED  "PercentageUsed"

typedef enum {
    STORAGE_MGNT_NOT_READY = 0,    // 0-初始状态，上电未完成
    STORAGE_MGNT_READY,            // 1-上电已完成，且已经获取过一遍控制器信息、硬盘信息、逻辑盘信息
    STORAGE_MGNT_OOB_NOT_SUPPORT,  // 2-上电已完成，且已经判断当前不支持带外管理
    STORAGE_MGNT_CTRL_ABNOMAL,     // 3-上电已完成，且在获取控制器信息过程中出现异常（该异常不包括I2C通信失败）
    STORAGE_MGNT_NO_CTRLS,         // 4-上电已完成，且已经判断当前不存在RAID卡
    STORAGE_MGNT_BUTT
}STORAGE_MGNT_STATE;

typedef enum tag_hdd_storage_logic {
    HDD_STORAGE_LOGIC_SERIAL_NUMBER,
    HDD_STORAGE_LOGIC_MODEL_NUMBER,
    HDD_STORAGE_LOGIC_FIRMWARE,
    HDD_STORAGE_LOGIC_LINK_SPEED,
    HDD_STORAGE_LOGIC_ALARM_CODE,
    HDD_STORAGE_LOGIC_EVENT_CODE
} HDD_STORAGE_LOGIC_E;

#define BIT_MEDIA_ERR                   BIT(0)
#define BIT_PREDICTIVE_FAIL             BIT(1)
#define BIT_OTHER_ERR                   BIT(2)
#define BIT_IO_DETERIORATION            BIT(3)
#define BIT_DRIVE_NOT_READY             BIT(4)
#define BIT_DRIVE_FUNCTIONAL            BIT(5)
#define BIT_RESET_NOT_REQUIRED          BIT(6)
#define BIT_CODE_PORT0_PCIE_LINK_ACTIVE BIT(7)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __STORAGE_DEFINE_H__ */

