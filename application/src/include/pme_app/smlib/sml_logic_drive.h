/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description: logic drive头文件
 * Author: wangliang 00247157
 * Create: 2021-2-18
 * Notes:
 * History:2021-2-18 内容拆分自sml_base.h
 */

#ifndef __SML_LOGIC_DRIVE_H__
#define __SML_LOGIC_DRIVE_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SML_LD_NAME_LENGTH 128
/* BEGIN: Modified by fwx469456, 2017/12/08 14:28:37   问题单号:DTS2017120509584   */
#define SML_CTRL_SUPPORTED_RAIDLEVELS_LENGTH 100
/* END:   Modified by fwx469456, 2017/12/08 14:28:50 */
#define SML_LD_MIN_SIZE 64 // MB

#define SML_LD_CAPACITY_UNIT_MB 0
#define SML_LD_CAPACITY_UNIT_GB 1
#define SML_LD_CAPACITY_UNIT_TB 2

#define SML_LD_DEFAULT_CAPACITY_IN_MB 0xFFFFFFFF // 创建逻辑盘可以指定默认值，后台使用最大可用空间来创建
#define SML_LD_MAX_CAPACITY_IN_MB     0xFFFFFFFE // 最大可能的逻辑盘容量值

#define SML_ERR_SHORT_BRIEF_LEN 128

#define LD_PROP_SETTING_NAME              BIT(0)
#define LD_PROP_SETTING_READ_POLICY       BIT(1)
#define LD_PROP_SETTING_WRITE_POLICY      BIT(2)
#define LD_PROP_SETTING_IO_POLICY         BIT(3)
#define LD_PROP_SETTING_ACCESS_POLICY     BIT(4)
#define LD_PROP_SETTING_DISK_CACHE_POLICY BIT(5)
#define LD_PROP_SETTING_BGI_ENABLE        BIT(6)

typedef enum tag_logic_drive_raid_level {
    RAID_LEVEL_0  = 0,
    RAID_LEVEL_1  = 1,
    RAID_LEVEL_2  = 2,
    RAID_LEVEL_3  = 3,
    RAID_LEVEL_4  = 4,
    RAID_LEVEL_5  = 5,
    RAID_LEVEL_6  = 6,
    RAID_LEVEL_10 = 10,
    RAID_LEVEL_1ADM = 11,
    RAID_LEVEL_10ADM = 12,
    RAID_LEVEL_1TRIPLE = 13,
    RAID_LEVEL_10TRIPLE = 14,
    RAID_LEVEL_1E = 17,
    RAID_LEVEL_20 = 20,
    RAID_LEVEL_30 = 30,
    RAID_LEVEL_40 = 40,
    RAID_LEVEL_50 = 50,
    RAID_LEVEL_60 = 60,
    /* BEGIN: 2019-7-5 tianpeng twx572344 PN:DTS2019051509803 */
    RAID_LEVEL_DELETED = 254,
    /* END:   2019-7-5 tianpeng twx572344 PN:DTS2019051509803 */
    RAID_LEVEL_UNKNOWN = 255
} SML_LOGIC_DRIVE_RAID_LEVEL_E;

typedef enum tag_primary_raid_level {
    PRL_0 = 0,
    PRL_1 = 1,
    PRL_5 = 5,
    PRL_6 = 6
} PRIMARY_RAID_LEVEL_E;

typedef enum tag_raid_level_qualifier {
    RLQ_0 = 0,
    RLQ_3 = 3,
} RAID_LEVEL_QUALIFIER_E;

typedef enum tag_secondary_raid_level {
    SRL_0 = 0,
    SRL_3 = 3,
} SECONDARY_RAID_LEVEL_E;

typedef enum tag_ld_state {
    LD_STATE_OFFLINE            = 0,
    LD_STATE_PARTIALLY_DEGRADED = 1,
    LD_STATE_DEGRADED           = 2,
    LD_STATE_OPTIMAL            = 3,
    LD_STATE_FAILED             = 4,
    LD_STATE_NOT_CONFIGURED,
    LD_STATE_INTERIM_RECOVERY,
    LD_STATE_READY_FOR_RECOVERY,
    LD_STATE_RECOVERYING,
    LD_STATE_WRONG_DRIVE_REPLACED,
    LD_STATE_DRVIE_IMPROPERLY_CONNECTED = 10,
    LD_STATE_EXPANDING,
    LD_STATE_NOT_YET_AVAILABLE,
    LD_STATE_QUEUED_FOR_EXPANSION,
    LD_STATE_DISABLED_FROM_SCSIID_CONFLICT,
    LD_STATE_EJECTED,
    LD_STATE_ERASE_IN_PROGRESS,
    LD_STATE_UNUSED,
    LD_STATE_READY_TO_PERFORM_PREDICTIVE_SPARE_REBUILD,
    LD_STATE_RPI_IN_PROGRESS,
    LD_STATE_RPI_QUEUED = 20,
    LD_STATE_ENCRYPTED_VOLUME_WITHOUT_KEY,
    LD_STATE_ENCRYPTION_MIGRATION,
    LD_STATE_ENCRYPTED_VOLUME_REKEYING,
    LD_STATE_ENCRYPTED_VOLUME_ENCRYPTION_OFF,
    LD_STATE_VOLUME_ENCODE_REQUESTED,
    LD_STATE_ENCRYPTED_VOLUME_REKEY_REQUESTED,
    LD_STATE_UNSUPPORTED_ON_THIS_CONTROLLER,
    LD_STATE_NOT_FORMATTED, // 未格式化
    LD_STATE_FORMATTING, // 正在格式化
    LD_STATE_SANITIZING = 30, // 正在进行数据销毁
    LD_STATE_INITIALIZING,
    LD_STATE_INITIALIZEFAIL,
    LD_STATE_DELETING,
    LD_STATE_DELETEFAIL,
    LD_STATE_WRITE_PROTECT,
    LD_STATE_UNKNOWN = 0xFF,
} SML_LD_STATE_E;

typedef enum tag_ld_state_level {
    LEVEL_NORMAL = 0,
    LEVEL_WARNING,
    LEVEL_CRITICAL,
    LEVEL_UNKNOWN = 0xFF,
} SML_LD_STATE_LEVEL_E;

typedef enum tag_ld_read_policy {
    LD_CACHE_NO_READ_AHEAD = 0,
    LD_CACHE_READ_AHEAD,
    LD_CACHE_READ_AHEAD_ADAPTIVE
} SML_LD_READ_POLICY_E;

typedef enum tag_ld_write_policy {
    LD_CACHE_WRITE_THROUGH = 0,
    LD_CACHE_WRITE_BACK,
    LD_CACHE_WRITE_CACHE_IF_BAD_BBU,
    LD_CACHE_WRITE_BUTT,  // 不能超过2个bit表示的最大值3
    LD_CACHE_READ_ONLY
} SML_LD_WRITE_POLICY_E;

typedef enum tag_ld_cache_policy {
    LD_CACHE_CACHED_IO = 0,
    LD_CACHE_DIRECT_IO
} SML_LD_CACHE_POLICY_E;

typedef enum tag_ld_access_policy {
    LD_ACCESS_RW = 0,
    LD_ACCESS_READ_ONLY,
    LD_ACCESS_BLOCKED,
    LD_ACCESS_HIDDEN
} SML_LD_ACCESS_POLICY_E;

typedef enum tag_disk_cache_policy {
    PD_CACHE_UNCHANGED = 0,
    PD_CACHE_ENABLE,
    PD_CACHE_DISABLE
} SML_DISK_CACHE_POLICY_E;

typedef enum _tag_ld_init_state {
    LD_INIT_STATE_NO_INIT    = 0,
    LD_INIT_STATE_QUICK_INIT = 1,
    LD_INIT_STATE_FULL_INIT  = 2,
    LD_INIT_STATE_RPI = 3,  // Rapid-Parity-Initialization
    LD_INIT_STATE_OPO = 4,  // SSD Over-Provisioning Optimization
    LD_INIT_STATE_FRONT = 5,  // 前台
    LD_INIT_STATE_BACKGROUND = 6,  // 后台,
    LD_INIT_STATE_BUTT,
} SML_LD_INIT_STATE_E;

typedef enum _tag_ld_accelerator {
    LD_ACCELERATOR_UNKNOWN = 0,
    LD_ACCELERATOR_NONE,
    LD_ACCELERATOR_CACHE,
    LD_ACCELERATOR_IOBYPASS,
    LD_ACCELERATOR_MAXCACHE,
} SML_LD_ACCELERATOR_E;

typedef enum _tag_ld_cache_line_size {
    LD_CACHE_LINE_SIZE_64K = 1,
    LD_CACHE_LINE_SIZE_256K = 4,
} SML_LD_CACHE_LINE_SIZE_E;

typedef enum _tag_ld_operation {
    LD_OPERATION_DELETE = 0,                  // 删除逻辑盘
    LD_OPERATION_SET_BOOTABLE = 1,            // 设置逻辑盘为启动盘
    LD_OPERATION_SET_PROPERTIES = 2,          // 设置逻辑盘属性值
    LD_OPERATION_SET_SSCD_CACHING_ENABLE = 3, // 设置关联到所有CacheCade逻辑盘或取消所有关联
    /* BEGIN: Modified by zhanglei wx382755, 2017/11/20   PN:zhanglei_fgi */
    LD_OPERATION_START_FGI = 4, // 开始FGI
    LD_OPERATION_CANCEL_FGI = 5 // 取消FGI
    /* END:   Modified by zhanglei wx382755, 2017/11/20 */
} SML_LD_OPERATION_E;

typedef enum _tag_config_operation {
    CONFIG_OPERATION_CREATE_LD_ON_NEW_ARRAY = 0,     // 在新的Array上创建LD
    CONFIG_OPERATION_CREATE_LD_ON_EXISTED_ARRAY = 1, // 在现有的Array上创建LD
    CONFIG_OPERATION_CREATE_CACHECADE_LD = 2,        // 创建CacheCade LD
} SML_CONFIG_OPERATION_E;

// ------------创建逻辑盘的公共属性参数
typedef struct tag_sml_create_ld_common_properties {
    guint32 capacity;                  // 容量大小, MB为单位
    gchar ld_name[SML_LD_NAME_LENGTH]; // 逻辑盘的名称，以'\0'结束的ASCII字符串
    guint8 strip_size;                 // 条带大小 64K/128K/256K/512K/1024K(7~11)
    guint8 read_policy;                // 读策略 Read Ahead/No Read Ahead/ Adaptive Read Ahead
    guint8 write_policy;               // 写策略 Write Through/Write Back/Write caching ok if bad BBU
    guint8 io_policy;                  // IO策略 Direct IO/Cached IO
    guint8 access_policy;              // 访问策略 Read Write/Read Only/Blocked
    guint8 disk_cache_policy;          // 硬盘缓存策略 Unchanged/Enable/Disable
    guint8 init_state;                 // 初始化状态 No Init/Quick Init/Full Init
    guint8 accelerator;
} SML_CREATE_LD_COMMON_PROPERTIES_S;

// ------------创建逻辑盘返回的参数
typedef struct tag_sml_create_ld_return_param {
    gint32 err_code;                                    // 创建逻辑盘返回的错误码
    gchar err_short_brief[SML_ERR_SHORT_BRIEF_LEN + 1]; // 错误简短描述
    guint16 ld_target_id;                                // 新创建的逻辑盘ID，创建成功才可用
    guint8 array_count;                                 // 创建使用的Array个数，创建成功才可用
    guint16 array_refs[SML_MAX_SPAN_DEPTH]; // 创建使用的Array ID，可能为新建，也可能是已有的Array，创建成功才可用
} SML_CREATE_LD_RETURN_PARAM_S;

// ------------创建逻辑盘所需的物理盘信息
typedef struct tag_sml_create_ld_pd_info {
    guint16 device_id;        // 物理盘ID
    guint16 seq_num;          // 物理盘sequence number
    guint8 scsi_dev_type;     // 设备scsci类型
    guint8 media_type;        // 媒介类型
    guint8 interface_type;    // 接口类型
    guint8 fw_state;          // 固件状态
    guint8 not_supported;     // 物理设备是否支持
    guint8 is_foreign;        // 是否存在外部配置
    guint64 coerced_size_blk; // 对齐后的物理盘容量，单位block
    guint16 block_size;       // 物理盘block大小
} SML_CREATE_LD_PD_INFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
