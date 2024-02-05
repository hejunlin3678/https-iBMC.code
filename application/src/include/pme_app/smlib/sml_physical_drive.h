/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: physical drive头文件
 * Author: wangliang 00247157
 * Create: 2021-2-18
 * Notes:
 * History:2021-2-18 内容拆分自sml_base.h
 */

#ifndef __SML_PHYSICAL_DRIVE_H__
#define __SML_PHYSICAL_DRIVE_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SML_PD_SN_LENGTH           32
#define SML_PD_MODEL_LENGTH        48
#define SML_PD_FW_VERSION_LENGTH   32
#define SML_PD_MANUFACTURER_LENGTH 32

#define PD_BLOCK_SIZE_512          512
#define PD_BLOCK_SIZE_4K           4096


// SCSI Peripheral Device Type (SPC-2 Table 22)
#define SPC_HARDDISK            0x00
#define SPC_TAPE                0x01
#define SPC_PROCESSOR           0x03
#define SPC_CDROM               0x05
#define SPC_STRG_ARR_CTRL_DEV   0x0C
#define SPC_SES_DEV             0x0D
#define SPC_UNKNOWN             0x1F

typedef enum tag_physical_drive_hot_spare {
    PD_HOT_SPARE_NONE      = 0,
    PD_HOT_SPARE_GLOBAL    = 1,
    PD_HOT_SPARE_DEDICATED = 2,
    PD_HOT_SPARE_AUTO_REPLACE = 3, // 单个Array的局部热备；重构完成后不会恢复热备状态，新插入的硬盘成为新的Auto Replace Spare
    PD_HOT_SPARE_UNKNOWN   = 255
} SML_PHYSICAL_DRIVE_HOT_SPARE_E;

typedef enum tag_physical_drive_intf_type {
    PD_INTERFACE_TYPE_UNDEFINED     = 0,
    PD_INTERFACE_TYPE_PARALLEL_SCSI = 1,
    PD_INTERFACE_TYPE_SAS           = 2,
    PD_INTERFACE_TYPE_SATA          = 3,
    PD_INTERFACE_TYPE_FC            = 4,
    PD_INTERFACE_TYPE_SATA_SAS      = 5, // SATA/SAS硬盘不受带外管理时接口类型的默认值
    PD_INTERFACE_TYPE_PCIE          = 6, // PCIe SSD硬盘接口类型的默认值
    PD_INTERFACE_TYPE_UNKNOWN       = 255
} SML_PHYSICAL_DRIVE_INTERFACE_TYPE_E;

/* BEGIN: Added by Yangshigui YWX386910, 2016/11/14   问题单号:AR-0000264511-001-001 */
typedef enum tag_pd_media_type {
    PD_MEDIA_TYPE_ROTATIONAL = 0, // rotating media, HDD
    PD_MEDIA_TYPE_SSD        = 1, // SSD
    PD_MEDIA_TYPE_SSM_FLASH  = 2, // Solid state flash module
    PD_MEDIA_TYPE_UNKNOWN	 = 255
} SML_PD_MEDIA_TYPE;
/* END:   Added by Yangshigui YWX386910, 2016/11/14 */

typedef enum _tag_pd_state {
    PD_STATE_UNCONFIGURED_GOOD   = 0,  // Unconfigured good drive
    PD_STATE_UNCONFIGURED_BAD    = 1,  // Unconfigured bad drive
    PD_STATE_HOT_SPARE           = 2,  // Hot spare drive
    PD_STATE_OFFLINE             = 3,  // Configured - good drive (data invalid)
    PD_STATE_FAILED              = 4,  // Configured - bad drive (data invalid)
    PD_STATE_REBUILD             = 5,  // Configured - drive is rebuilding
    PD_STATE_ONLINE              = 6,  // Configured - drive is online
    PD_STATE_COPYBACK            = 7,  // drive is getting copied
    PD_STATE_SYSTEM              = 8,  // drive is exposed and controlled by host
    PD_STATE_SHIELD_UNCONFIGURED = 9,  // UnConfigured - shielded
    PD_STATE_SHIELD_HOT_SPARE    = 10, // Hot Spare - shielded
    PD_STATE_SHIELD_CONFIGURED   = 11, // Configured - shielded
    PD_STATE_FOREIGN             = 12, // With foreign configuration
    PD_STATE_ACTIVE              = 13,
    PD_STATE_STANDBY             = 14,
    PD_STATE_SLEEP               = 15,
    PD_STATE_DST                 = 16,
    PD_STATE_SMART               = 17,
    PD_STATE_SCT                 = 18,
    PD_STATE_EPD                 = 19, // enhanced physical drive
    PD_STATE_RAW                 = 20,
    PD_STATE_READY               = 21,
    PD_STATE_NOT_SUPPORTED       = 22,
    PD_STATE_PREDICTIVE_FAILURE  = 23,
    PD_STATE_DIAGNOSING          = 24, // 磁盘临时进入诊断状态，诊断完成后恢复成成员盘或者热备盘
    PD_STATE_INCOMPATIBLE        = 25, // 导入的配置文件不兼容
    PD_STATE_ERASING             = 26,
    PD_STATE_UNKNOWN             = 255 // Unknown state
} SML_PD_STATE_E;

typedef enum _tag_pd_power_state {
    PD_POWER_STATE_ACTIVE        = 0,   // Device is active (spun up)
    PD_POWER_STATE_STOP          = 1,   // Device is stopped (spun down)
    PD_POWER_STATE_TRANSITIONING = 2,   // Device is transitioning between power states
    PD_POWER_STATE_UNKNOWN       = 255, // Unknown state
} SML_PD_POWER_STATE_E;

typedef enum tag_sml_pd_log_type {
    PD_LOG_SATA_SMART_ATTRIBUTE = 0,
    PD_LOG_SATA_SMART_ATTRIBUTE_THRESHOLD,
    PD_LOG_SATA_ERROR,
    PD_LOG_SATA_EXTENT_ERROR,
    PD_LOG_SATA_EXTENDED_SELF_TEST,
    PD_LOG_SATA_PHY_EVENT,
    PD_LOG_SATA_SEAGATE_GLIST,
    PD_LOG_SATA_SEAGATE_PLIST,
    PD_LOG_SATA_SEAGATE_CRITICAL_EVENT,
    PD_LOG_SATA_SEAGATE_FARM,
    PD_LOG_SATA_SEAGATE_FACTORY_FARM,
    PD_LOG_SAS_SELF_TEST = 100,
    PD_LOG_SAS_WRITE_ERROR_CNT,
    PD_LOG_SAS_READ_ERROR_CNT,
    PD_LOG_SAS_VERIFY_ERROR_CNT,
    PD_LOG_SAS_NONMEDIUM_ERROR_CNT,
    PD_LOG_SAS_PHY_ERROR_CNT,
    PD_LOG_SAS_INTERNAL_EXCEPTION,
    PD_LOG_SAS_TEMPERATURE,
    PD_LOG_SAS_BACKGORUN_MEDIA_SCAN,
    PD_LOG_SAS_GLIST,
    PD_LOG_SAS_GENERAL_STATISTICS,
    PD_LOG_SAS_HUAWEI_SPECIFIC32H,
    PD_LOG_SAS_HUAWEI_SPECIFIC34H,
    PD_LOG_SAS_HUAWEI_SPECIFIC35H,
    PD_LOG_SAS_HUAWEI_IO_INFO,
    PD_LOG_SAS_KIOXIA_SSD_SPECIFIC,
    PD_LOG_SAS_KIOXIA_EOPT,
    PD_LOG_SAS_KIOXIA_SMART_DATA
} SML_PD_LOG_TYPE_E;

typedef enum {
    PD_FORM_FACTOR_UNKNOWN,

    // Dimensional Form Factor
    PD_FORM_FACTOR_5_25_INCH,
    PD_FORM_FACTOR_3_5_INCH,
    PD_FORM_FACTOR_2_5_INCH,
    PD_FORM_FACTOR_1_8_INCH,
    PD_FORM_FACTOR_LESS_THAN_1_8_INCH,

    // Connector Form Factor
    PD_FORM_FACTOR_MSATA,
    PD_FORM_FACTOR_M2,
    PD_FORM_FACTOR_MICRO_SSD,
    PD_FORM_FACTOR_CFAST,
    PD_FORM_FACTOR_INVALID = 0xff
} SML_PD_FORM_FACTOR_E;

#define SML_ASYNC_OPERATION_OFFSET      0x80

typedef enum _tag_pd_operation {
    PD_OPERATION_STOP_LOCATE            = 0,
    PD_OPERATION_LOCATE                 = 1,
    PD_OPERATION_SET_GLOBAL_HOTSPARE    = 2, // 设置PD为全局热备盘
    PD_OPERATION_SET_DEDICATED_HOTSPARE = 3, // 设置PD为局部热备盘
    PD_OPERATION_CANCEL_HOTSPARE        = 4, // 取消PD的热备盘设置
    PD_OPERATION_SET_STATE              = 5, // 设置PD的固件状态Online/Offline/JBOD/UG
    PD_OPERATION_CRYPTO_ERASE           = 6, // 安全擦除SED(self-enrypting drive)
    PD_OPERATION_SMART_TEST             = 7, // 触发PD执行测试
    PD_OPERATION_SET_BOOTABLE           = 8, // 设置物理盘为启动盘
    PD_OPERATION_SET_PATROLREAD_STATE   = 9, // 设置物理盘巡检状态
    PD_OPERATION_SET_AUTO_REPLACE_HOTSPARE = 10, // 设置PD为自动替换热备盘
    PD_OPERATION_CAN_SET_DEDICATED_HOTSPARE =
        (PD_OPERATION_SET_DEDICATED_HOTSPARE + SML_ASYNC_OPERATION_OFFSET), // 设置PD为局部热备盘的前台评估
    PD_OPERATION_CAN_SET_AUTO_REPLACE_HOTSPARE =
        (PD_OPERATION_SET_AUTO_REPLACE_HOTSPARE + SML_ASYNC_OPERATION_OFFSET), // 设置PD为自动替换热备盘的前台评估
    PD_OPERATION_CAN_CANCEL_HOTSPARE =
        (PD_OPERATION_CANCEL_HOTSPARE + SML_ASYNC_OPERATION_OFFSET), // 取消PD的热备盘的前台评估
} SML_PD_OPERATION_E;

typedef enum _tag_pd_smart_test_type {
    PD_TEST_SATA_OFFLINE = 0,
    PD_TEST_SATA_SHORT_OFFLINE,
    PD_TEST_SATA_EXTENDED_OFFLINE,
    PD_TEST_SAS_DEFAULT = 100,
    PD_TEST_SAS_BACKGROUND_SHORT,
    PD_TEST_SAS_BACKGROUND_LONG,
} SML_PD_SMART_TEST_TYPE_E;

typedef enum _tag_pd_speed_type {
    PD_SPEED_UNDEFINED,
    PD_SPEED_1P5G,
    PD_SPEED_3G,
    PD_SPEED_6G,
    PD_SPEED_12G,
    PD_SPEED_2P5G,
    PD_SPEED_5G,
    PD_SPEED_8G,
    PD_SPEED_10G,
    PD_SPEED_16G,
    PD_SPEED_20G,
    PD_SPEED_30G,
    PD_SPEED_32G,
    PD_SPEED_40G,
    PD_SPEED_64G,
    PD_SPEED_80G,
    PD_SPEED_96G,
    PD_SPEED_128G,
    PD_SPEED_160G,
    PD_SPEED_256G,
    PD_SPEED_22P5G,

    PD_SPEED_PCIE_2P5G = 0x80,
    PD_SPEED_PCIE_5G,
    PD_SPEED_PCIE_8G,
    PD_SPEED_PCIE_16G,
    PD_SPEED_UNKNOWN = 0XFF,
} SML_PD_SPEED_TYPE_E;


#define ATA_SMART_DATA_LENGTH 512
#define ATA_SMART_THRESHOLD_LENGTH ATA_SMART_DATA_LENGTH

typedef struct _tag_sml_pd_smart_info {
    guint8 valid_flag;
    guint32 last_update_timestamp;
    union {
        struct {
            guint8 smart_data[ATA_SMART_DATA_LENGTH];
            guint8 smart_threshold[ATA_SMART_THRESHOLD_LENGTH];
        } SATADevice;
        struct {
            guint8 ASC;
            guint8 ASCQ;
        } SASDevice;
    };
} SML_PD_SMART_INFO_S;

typedef struct _tag_sml_pd_progress_info {
    guint8 rebuild_state;
    guint8 rebuild_progress;
    guint8 patrol_state;
    guint8 patrol_progress;
} SML_PD_PROGRESS_INFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
