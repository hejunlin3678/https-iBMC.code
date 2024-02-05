/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: controller头文件
 * Author: wangliang 00247157
 * Create: 2021-2-18
 * Notes:
 * History:2021-2-18 内容拆分自sml_base.h
 */

#ifndef __SML_CONTROLLER_H__
#define __SML_CONTROLLER_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define BIT(x) (1 << (x))

#define CTRL_WCP_DEFAULT_STR     "Default"
#define CTRL_WCP_ENABLE_STR      "Enabled"
#define CTRL_WCP_DISABLE_STR     "Disabled"
#define CTRL_WCP_UNKNOWN_STR     "N/A"

#define CTRL_MODE_RAID_STR     "RAID"
#define CTRL_MODE_HBA_STR      "HBA"
#define CTRL_MODE_JBOD_STR     "JBOD"
#define CTRL_MODE_MIXED_STR    "Mixed"
#define CTRL_MODE_UNKNOWN_STR  "N/A"

// SML库管理的RAID控制器/LD/PD的最大支持数
#define SML_MAX_RAID_CONTROLLER           8
#define SML_MAX_SAS_PHY_PER_CTRL          40
#define SML_MAX_LOGIC_DRIVES              240
#define SML_MAX_SPAN_DEPTH                16
#define SML_MAX_PHYSICAL_DRIVES           256
#define SML_MAX_ARRAY                     128
#define SML_MAX_LOGICAL_DRIVES_PER_ARRAY  16
#define SML_MAX_PHYSICAL_DRIVES_PER_ARRAY 32
#define SML_MAX_NUM_OF_IDS                256
#define SML_MAX_HOLES_IN_ARRAY            32
#define SML_MAX_SAS_PHY_PER_EXPANDER      100  // 每个Expander下的最大PHY个数
#define SML_MAX_EXPANDER_PER_CONTROLLER   4    // 每个RAID卡可连接的最大Expander个数
#define SML_MAX_BOOTABLE_DEVICES          8
#define SML_MAX_LOGIC_DRIVES_BY_HOT_SPARE 64 // 每个RAID卡可支持最大逻辑盘个数

#define SML_CTRL_NAME_LENGTH              100
#define SML_CTRL_FW_VERSION_LENGTH        128
#define SML_CTRL_NVDATA_VERSION_LENGTH    64
#define SML_CTRL_SN_LEN                   64
#define SML_SAS_ADDR_LENGTH               32
#define SML_BBU_TYPE_LENGTH               16
#define SML_CTRL_HW_REV_LENGTH            32

/* BEGIN: Modified by zwx382233, 2017/5/18 16:53:47   问题单号:DTS2017050904064  */
#define SML_RAID_LEVEL_0        "RAID0"
#define SML_RAID_LEVEL_1        "RAID1"
#define SML_RAID_LEVEL_5        "RAID5"
#define SML_RAID_LEVEL_6        "RAID6"
#define SML_RAID_LEVEL_10       "RAID10"
#define SML_RAID_LEVEL_50       "RAID50"
#define SML_RAID_LEVEL_60       "RAID60"
#define SML_RAID_LEVEL_1ADM     "RAID1(ADM)"
#define SML_RAID_LEVEL_10ADM    "RAID10(ADM)"
#define SML_RAID_LEVEL_1TRIPLE  "RAID1Triple"
#define SML_RAID_LEVEL_10TRIPLE "RAID10Triple"
/* END:   Modified by zwx382233, 2017/5/18 16:53:52 */

#define BIT_OFFSET_MEMORY_CORRECTABLE_ERROR 1
#define BIT_OFFSET_MEMORY_UNCORRECTABLE_ERROR 2
#define BIT_OFFSET_ECC_ERROR 3
#define BIT_OFFSET_COMMUNICATION_LOST 4

#define SML_BOOT_PRIORITY_NONE        "None"
#define SML_BOOT_PRIORITY_PRIMARY     "Primary"
#define SML_BOOT_PRIORITY_SECONDARY   "Secondary"
#define SML_BOOT_PRIORITY_ALL         "All"

#define LSI_STORELIB_TYPE_VALID_BIT(ctrl_id) (((ctrl_id)&0xFF000000) >> 24)
#define LSI_CTRL_ID_RESVED_BIT_MASK(ctrl_id) ((ctrl_id) & (~0x007F0000))
#define HISTORE_TYPE_RAID 0
#define HISTORE_TYPE_HBA  1
#define VOLUME_TYPE_PHYSICAL_DRIVE_BIT 0x10000000
#define VOLUME_TYPE_LOGICAL_DRIVE_BIT  0x20000000

#define SL_LD_NAME_LENGTH       16      /* 博通卡逻辑盘最大长度15 */
#define SC_LD_NAME_LENGTH       65      /* PMC卡逻辑盘最大长度64 */
#define HS_LD_NAME_LENGTH       21      /* 自研卡逻辑盘最大长度20 */

#define CTRL_CC_RATE_LOW_STR    "Low"
#define CTRL_CC_RATE_MIDDLE_STR "Medium"
#define CTRL_CC_RATE_HIGH_STR   "High"
#define CTRL_CC_STATUS_ON       "On"
#define CTRL_CC_STATUS_OFF      "Off"
#define CTRL_CC_DISABLE         0
#define CTRL_CC_ENABLE          1
#define CTRL_CC_SET             2
#define CTRL_CC_PERIOD_MIN      24      /* 1天即24小时 */
#define CTRL_CC_PERIOD_MAX      1440    /* 60天即1440小时 */
#define CTRL_CC_DELAY_MAX       24      /* 最长为24小时 */
#define CTRL_CC_MASK_PERIOD     BIT(0)
#define CTRL_CC_MASK_RATE       BIT(1)
#define CTRL_CC_MASK_REPAIR     BIT(2)
#define CTRL_CC_MASK_DELAY      BIT(3)
#define CTRL_CC_STR_LEN 255

#define EVENT_CODE_UNEXPECTED_SENSE 113
#define EVENT_CODE_COMMAND_TIMEOUT 267
#define EVENT_CODE_RESET_TYPE 268
#define EVENT_CODE_POWER_RESET 331

typedef union tag_sml_event_detail {
    guint8 b[256]; // 256是参考结构体MR_EVT_DETAIL的大小
    struct {
        guint32 code;
        guint8 slot_num;
    } pd_event;
} SML_EVENT_DETAIL;

typedef enum _tag_ctrl_ccheck_rate {
    CTRL_CCHECK_RATE_LOW     = 1,
    CTRL_CCHECK_RATE_MIDDLE  = 2,
    CTRL_CCHECK_RATE_HIGH    = 3,
} SML_CTRL_CCHECK_RATE;

typedef enum _tag_ctrl_param_index {
    CTRL_PARAM_INDEX_FIR     = 0,
    CTRL_PARAM_INDEX_SEC     = 1,
    CTRL_PARAM_INDEX_THI     = 2,
    CTRL_PARAM_INDEX_FOU     = 3,
    CTRL_PARAM_INDEX_FIF     = 4,
    CTRL_PARAM_INDEX_SIX     = 5,
} SML_CTRL_PARAM_INDEX;

typedef enum _tag_sml_ctrl_vender_index {
    VENDER_LSI,
    VENDER_PMC,
    VENDER_HUAWEI,
#ifdef ITEST_ENABLED
    VENDER_ITEST_SIMULATE, // used for itest
#endif
} SML_CTRL_VENDER_INDEX;

typedef enum tag_controller_type_id {
    LSI_3108_WITH_MR,
    LSI_3108_WITH_IR,
    LSI_3108_WITH_IT,
    LSI_3008_WITH_MR,
    LSI_3008_WITH_IR,
    LSI_3008_WITH_IT,
    /* BEGIN: Added by Yangshigui YWX386910, 2017/4/6   问题单号:SR-0000276589-001 */
    LSI_3508_WITH_MR,
    LSI_3516_WITH_MR,
    LSI_3408_WITH_IMR,
    LSI_3416_WITH_IMR,
    LSI_3408_WITH_IT,
    LSI_3416_WITH_IT,
    /* END:   Added by Yangshigui YWX386910, 2016/4/6 */
    /* BEGIN: Added by Yangshigui YWX386910, 2017/10/28   问题单号:AR.SR.SFEA02109385.001.001/002 */
    LSI_3004_WITH_IMR,
    /* END:   Added by Yangshigui YWX386910, 2017/10/28 */
    LSI_3008_WITH_IT_PCIE,
    LSI_3908_WITH_MR,
    LSI_3916_WITH_MR,
    LSI_3808_WITH_MR,
    LSI_3816_WITH_IMR,
    LSI_3808_WITH_IMR = 18,
    PMC_3152_8I_SMART_RAID = 64,
    PMC_2100_8I_SMART_HBA = 65,
    HI1880_SP186_M_16i = 96,
    HI1880_SP186_M_32i = 97,
    HI1880_SP186_M_40i = 98,
    HI1880_SP686C_M_16i_2G = 99,
    HI1880_SP686C_M_16i_4G = 100,
    HI1880_SP686C_MH_32i_4G = 101,
    HI1880_SP686C_M_40i_2G = 102,
    HI1880_SP686C_M_40i_4G = 103,
    HI1880_SPR120 = 104,
    HI1880_SPR130 = 105,
    HI1880_SPR140 = 106,
    HI1880_SPR180 = 107,
    HI1880_SPR185 = 108,
    HI1880_SPR190 = 109,
    HI1880_SPR1A0 = 110,
    HI1880_SPR1A5 = 111,
    HI1880_SPR110 = 112,
    HI1880_SP186_M_8i = 113,
#ifdef ITEST_ENABLED
    ITEST_SIMULATE = 254,
#endif
    INVALID_CONTROLLER_TYPE = 255
} SML_CTRL_TYPE_ID_E;

typedef struct tag_controller {
    gint8 type_id;
    gint8 vendor_id;
    gint32 controller_id;
} SML_CTRL_ID;

typedef enum _tag_sml_ctrl_init_state {
    INIT_UNKNOWN_STATE = 0,
    INIT_IN_PROGRESS,
    INIT_DONE,
    INIT_FAILED,
} SML_CTRL_INIT_STATE;

typedef enum _tag_ctrl_device_interface {
    CTRL_DEVICE_INTERFACE_SPI,
    CTRL_DEVICE_INTERFACE_SAS_3G,
    CTRL_DEVICE_INTERFACE_SATA_1_5G,
    CTRL_DEVICE_INTERFACE_SATA_3G,
    CTRL_DEVICE_INTERFACE_SAS_6G,
    CTRL_DEVICE_INTERFACE_SAS_12G,
    CTRL_DEVICE_INTERFACE_SAS_32G,
    CTRL_DEVICE_INTERFACE_SAS_64G,
    CTRL_DEVICE_INTERFACE_SATA_32G,
    CTRL_DEVICE_INTERFACE_SATA_64G,
    CTRL_DEVICE_INTERFACE_INVALID = 255,
} SML_CTRL_DEVICE_INTERFACE_E;

typedef enum _tag_ctrl_bios_boot_mode {
    CTRL_BIOS_BOOT_MODE_SOE, // Stop On Errors
    CTRL_BIOS_BOOT_MODE_POE, // Pause On Errors
    CTRL_BIOS_BOOT_MODE_IOE, // Ignore On Errors
    CTRL_BIOS_BOOT_MODE_HSM, // Headless Safe Mode
    CTRL_BIOS_BOOT_MODE_INVALID = 255,
} SML_CTRL_BIOS_BOOT_MODE_E;

typedef enum _tag_volume_boot_priority {
    BOOT_PRIORITY_NONE = 0,
    BOOT_PRIORITY_PRIMARY,
    BOOT_PRIORITY_SECONDARY,
    BOOT_PRIORITY_ALL,
    BOOT_PRIORITY_INVALID = 0xFF,
} SML_VOLUME_BOOT_PRIORITY;

typedef enum _tag_ctrl_drive_write_cache_policy {
    CTRL_DRIVE_WCP_DEFAULT = 0,
    CTRL_DRIVE_WCP_ENABLED = 1,
    CTRL_DRIVE_WCP_DISABLED = 2,
    CTRL_DRIVE_WCP_INVALID = 3,
} SML_CTRL_DRIVE_WRITE_CACHE_POLICY;

typedef enum _tag_ctrl_drive_wcp {
    CTRL_DRIVE_WCP_CONFIG = 0,
    CTRL_DRIVE_WCP_UNCONFIG = 1,
    CTRL_DRIVE_WCP_HBA = 2,
    CTRL_DRIVE_WCP_INV = 3,
} SML_CTRL_DRIVE_WCP;

typedef enum _tag_ctrl_operation {
    CTRL_OPERATION_DISABLE_COPYBACK      = 0, // 设置CopyBack是否禁止
    CTRL_OPERATION_ENABLE_SMART_COPYBACK = 1, // 设置是否使能SMART CopyBack功能
    CTRL_OPERATION_ENABLE_JBOD           = 2, // 设置是否使能JBOD功能
    CTRL_OPERATION_RESTORE_DEFAULT       = 3, // 控制器恢复默认设定值
    CTRL_OPERATION_SET_BIOS_BOOT_MODE    = 4, // 控制器BIOS启动模式，参考storelib枚举类型MR_BOOT_MODE
    CTRL_OPERATION_SET_PERSONALITY_MODE  = 5, // 设置raid卡的控制模式，当前有raid模式和jbod模式
    CTRL_OPERATION_SET_MAINTAIN_PDFAIL_HISTORY = 6, // 设置是否在重启过程中启用对故障PD的跟踪、是否对不在位的故障PD点灯
    CTRL_OPERATION_IMPORT_FOREIGN_CONFIG = 7, // 导入外部配置
    CTRL_OPERATION_DUMP_LOG              = 8, // 收集日志
    CTRL_OPERATION_ENABLE_NBWC           = 9,
    CTRL_OPERATION_READ_CACHE_POLICY     = 10,
    CTRL_OPERATION_WCP_VALUE             = 11,
    CTRL_OPERATION_ENABLE_CC             = 12, // 设置/开始/停止一致性校验功能
    CTRL_OPERATION_CLEAR_FOREIGN_CONFIG  = 13, // 清除外部配置
} SML_CTRL_OPERATION_E;

typedef enum _tag_ctrl_mode {
    CTRL_MODE_RAID    = 0,
    CTRL_MODE_HBA     = 1,
    CTRL_MODE_JBOD    = 2,
    CTRL_MODE_MIXED   = 3,
    CTRL_MODE_UNKNOWN = 0xff, // 获取失败时候的默认值
} SML_CTRL_MODE;

typedef enum _tag_ctrl_pcie_width {
    PCIE_LINK_WIDTH_X1     = 1,
    PCIE_LINK_WIDTH_X2     = 2,
    PCIE_LINK_WIDTH_X4     = 4,
    PCIE_LINK_WIDTH_X8     = 8,
    PCIE_LINK_WIDTH_X16    = 16,
    PCIE_LINK_WIDTH_UNKNOW = 0xff, // 获取失败时候的默认值
} SML_CTRL_PCIE_WIDTH;

typedef enum _tag_ctrl_memory_size {
    CTRL_MRMORY_SIZE_256MB     = 256,
    CTRL_MRMORY_SIZE_512MB     = 512,
    CTRL_MRMORY_SIZE_1GB       = 1024,
    CTRL_MRMORY_SIZE_2GB       = 2048,
    CTRL_MRMORY_SIZE_4GB       = 4096,
    CTRL_MRMORY_SIZE_8GB       = 8192,
    CTRL_MRMORY_SIZE_16GB      = 16384,
    CTRL_MEMORY_SIZE_UNKNOW    = 0xffff
} SML_CTRL_MEMORY_SIZE;

typedef enum _tag_ctrl_info_type {
    CTRL_INFO_TYPE_CTRL             = BIT(0),
    CTRL_INFO_TYPE_HEALTH           = BIT(1),
    CTRL_INFO_TYPE_SAS_ADDR         = BIT(2),
    CTRL_INFO_TYPE_BBU              = BIT(3),
    CTRL_INFO_TYPE_PHY              = BIT(4),
    CTRL_INFO_TYPE_LDLIST           = BIT(5),
    CTRL_INFO_TYPE_PDLIST           = BIT(6),
    CTRL_INFO_TYPE_LD               = BIT(7),
    CTRL_INFO_TYPE_PD               = BIT(8),
    CTRL_INFO_TYPE_ARRAYLIST        = BIT(9),
    CTRL_INFO_TYPE_ARRAY            = BIT(10),
} SML_CTRL_INFO_TYPE_E;

typedef enum _tag_ctrl_log_type {
    CTRL_AP_LOG_TYPE = 0,
    CTRL_IMU_LOG_TYPE,
    CTRL_AP_LOG_INDEX,
    CTRL_IMU_LOG_INDEX,
    CTRL_AP_LASTWORD_TYPE,
    CTRL_DUMP_FILE_TYPE,
    CTRL_FLASH_DUMP_FILE_TYPE,
    CTRL_NAND_LOG0_FILE_TYPE,
    CTRL_NAND_LOG1_FILE_TYPE,
    CTRL_LOG_LAST_CRASH_DUMP,
    CTRL_LOG_SERIAL_OUTPUT,
    CTRL_LOG_TYPE_BUTT = 0xFF,
} SML_CTRL_LOG_TYPE_E;

// ------------创建逻辑盘所需的控制器信息
typedef struct tag_sml_create_ld_ctrl_info {
    guint8 max_lds_per_array;
    guint8 max_lds;
    guint8 allow_mix_ssd_hdd;
    guint8 allow_ssd_mix;

    guint8 raid0_supported;
    guint8 raid1_supported;
    guint8 raid5_supported;
    guint8 raid6_supported;
    guint8 raid10_supported;
    guint8 raid50_supported;
    guint8 raid60_supported;

    guint8 ssc_raid0_unsupported;
    guint8 ssc_raid1_supported;
    guint8 ssc_raid5_supported;

    guint8 min_pd_raid0;
    guint16 max_pd_raid0;
    guint8 min_pd_raid1;
    guint16 max_pd_raid1;
    guint8 min_pd_raid5;
    guint16 max_pd_raid5;
    guint8 min_pd_raid6;
    guint16 max_pd_raid6;
    guint8 min_pd_raid10;
    guint16 max_pd_raid10;
    guint8 min_pd_raid50;
    guint16 max_pd_raid50;
    guint8 min_pd_raid60;
    guint16 max_pd_raid60;

    guint8 min_strip;
    guint8 max_strip;

    guint8 support_epd;
} SML_CREATE_LD_CTRL_INFO_S;

/* BEGIN: Added by Yangshigui YWX386910, 2017/4/23   问题单号:DTS2017032104172 */
typedef union _tag_ctrl_option1 {
    guint32 opt1_val;
    struct {
#ifdef BD_BIG_ENDIAN
        guint32 access_policy_support : 1;
        guint32 access_policy_default : 2;
        guint32 access_policy_opt_reserved : 2;
        guint32 access_blocked_opt : 1;
        guint32 access_read_only_opt : 1;
        guint32 access_rw_opt : 1;

        guint32 io_policy_support : 1;
        guint32 io_policy_default : 2;
        guint32 io_policy_opt_reserved : 3;
        guint32 direct_io_opt : 1;
        guint32 cached_io_opt : 1;

        guint32 write_policy_support : 1;
        guint32 write_policy_default : 2;
        guint32 write_policy_opt_reserved : 2;
        guint32 write_back_if_bad_bbu_opt : 1;
        guint32 write_back_opt : 1;
        guint32 write_through_opt : 1;

        guint32 read_policy_support : 1;
        guint32 read_policy_default : 2;
        guint32 read_policy_opt_reserved : 3;
        guint32 read_ahead_opt : 1;
        guint32 no_read_ahead_opt : 1;
#else
        guint32 no_read_ahead_opt : 1;
        guint32 read_ahead_opt : 1;
        guint32 read_policy_opt_reserved : 3;
        guint32 read_policy_default : 2;
        guint32 read_policy_support : 1;

        guint32 write_through_opt : 1;
        guint32 write_back_opt : 1;
        guint32 write_back_if_bad_bbu_opt : 1;
        guint32 write_policy_opt_reserved : 2;
        guint32 write_policy_default : 2;
        guint32 write_policy_support : 1;

        guint32 cached_io_opt : 1;
        guint32 direct_io_opt : 1;
        guint32 io_policy_opt_reserved : 3;
        guint32 io_policy_default : 2;
        guint32 io_policy_support : 1;

        guint32 access_rw_opt : 1;
        guint32 access_read_only_opt : 1;
        guint32 access_blocked_opt : 1;
        guint32 access_policy_opt_reserved : 2;
        guint32 access_policy_default : 2;
        guint32 access_policy_support : 1;
#endif
    } opt1_div;
} SML_CTRL_OPTION1_S;

typedef union _tag_ctrl_option2 {
    guint32 opt2_val;
    struct {
#ifdef BD_BIG_ENDIAN
        guint32 reserved2 : 3;
        guint32 ctrl_support_mixed : 1;
        guint32 ctrl_support_hba : 1;
        guint32 ctrl_support_jbod : 1;
        guint32 ctrl_support_raid : 1;
        guint32 ctrl_support_mode_set : 1;

        guint32 reserved1 : 4;
        guint32 ctrl_support_jbod_state : 1;
        guint32 ctrl_support_epd : 1;
        guint32 ctrl_support_crypto_erase : 1;
        guint32 ctrl_temp_src_internal : 1;

        guint32 reserved0 : 8;

        guint32 pd_cache_policy_support : 1;
        guint32 pd_cache_policy_default : 2;
        guint32 pd_cache_opt_reserved : 2;
        guint32 pd_cache_disable_opt : 1;
        guint32 pd_cache_enable_opt : 1;
        guint32 pd_cache_unchanged_opt : 1;
#else
        /*
         Byte0，物理盘缓存策略（每个bit值，1表示支持，0表示不支持）
                bit0：物理盘缓存策略 – Unchanged（预设）
                bit1：物理盘缓存策略 – Enable（预设）
                bit2：物理盘缓存策略 – Disable（预设）
                bit3-4：保留
                bit5-6：默认值的bit位
                bit7：物理盘缓存策略 – 是否支持修改（从RAID卡获取）
        */
        guint32 pd_cache_unchanged_opt : 1;
        guint32 pd_cache_enable_opt : 1;
        guint32 pd_cache_disable_opt : 1;
        guint32 pd_cache_opt_reserved : 2;
        guint32 pd_cache_policy_default : 2;
        guint32 pd_cache_policy_support : 1;

        guint32 reserved0 : 8;

        /*
         Byte2: 综合特性
            bit0:  是否从控制器获取温度，1表示从控制器获取温度，0表示不从控制器获取温度
            bit1:  是否支持硬盘加密擦除
            bit2:  是否支持创建、删除EPD
            bit3:  是否支持JBOD功能
            bit4-7: 保留
        */
        guint32 ctrl_temp_src_internal : 1;
        guint32 ctrl_support_crypto_erase : 1;
        guint32 ctrl_support_epd : 1;
        guint32 ctrl_support_jbod_state : 1;
        guint32 reserved1 : 4;

        /*
         Byte3: 工作模式
            bit0:  是否支持设置工作模式
            bit1:  是否支持RAID工作模式
            bit2:  是否支持JBOD工作模式
            bit3:  是否支持HBA工作模式
            bit4:  是否支持Mixed工作模式
            bit5-7: 保留
        */
        guint32 ctrl_support_mode_set : 1;
        guint32 ctrl_support_raid : 1;
        guint32 ctrl_support_jbod : 1;
        guint32 ctrl_support_hba : 1;
        guint32 ctrl_support_mixed : 1;
        guint32 reserved2 : 3;
#endif
    } opt2_div;
} SML_CTRL_OPTION2_S;

typedef struct _tag_sml_ctrl_wcp {
    guint8 type;
    guint8 policy;
}SML_CTRL_WCP;
typedef union _tag_ctrl_option3 {
    guint32 opt3_val;
    struct {
#ifdef BD_BIG_ENDIAN
        guint32 reserved1 : 13;
        guint32 raid10triple_support_opt : 1;
        guint32 raid1triple_support_opt : 1;
        guint32 raid10adm_support_opt : 1;

        guint32 raid1adm_support_opt : 1;
        guint32 raid60_support_opt : 1;
        guint32 raid50_support_opt : 1;
        guint32 raid10_support_opt : 1;
        guint32 raid6_support_opt : 1;
        guint32 raid5_support_opt : 1;
        guint32 raid1_support_opt : 1;
        guint32 raid0_support_opt : 1;

        guint32 reserved0 : 2;
        guint32 hba_drive_wcp : 2;
        guint32 unconfigured_drive_wcp : 2;
        guint32 configured_drive_wcp : 2;
#else
        guint32 configured_drive_wcp : 2;
        guint32 unconfigured_drive_wcp : 2;
        guint32 hba_drive_wcp : 2;
        guint32 reserved0 : 2;

        guint32 raid0_support_opt : 1;
        guint32 raid1_support_opt : 1;
        guint32 raid5_support_opt : 1;
        guint32 raid6_support_opt : 1;
        guint32 raid10_support_opt : 1;
        guint32 raid50_support_opt : 1;
        guint32 raid60_support_opt : 1;
        guint32 raid1adm_support_opt : 1;

        guint32 raid10adm_support_opt : 1;
        guint32 raid1triple_support_opt : 1;
        guint32 raid10triple_support_opt : 1;
        guint32 reserved1 : 13;
#endif
    } opt3_div;
} SML_CTRL_OPTION3_S;

/* END:   Added by Yangshigui YWX386910, 2017/4/23 */
typedef gint32 (*HANDLE_EVENT_INFO_FUNC)(SML_EVENT_DETAIL *evt_detail);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
