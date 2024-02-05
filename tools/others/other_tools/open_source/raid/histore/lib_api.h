/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: cmd main process
 * Create: 2020-12-9
 * Notes: none
 * History: 2020-11-13 create
 */
#ifndef INCLUDE_OPEN_API_LIB_API_H
#define INCLUDE_OPEN_API_LIB_API_H

// 命令涉及数据结构默认采用8字节对齐
#pragma pack(push)
#pragma pack(8)

#ifndef u8
typedef unsigned char u8;
#endif

#ifndef u16
typedef unsigned short u16;
#endif

#ifndef u32
typedef unsigned int u32;
#endif

#ifndef u64
typedef unsigned long long u64;
#endif

#define CMD_TYPE_SET 0x00
#define CMD_TYPE_WRITE 0x01
#define CMD_TYPE_READ 0x02
#define CMD_TYPE_MASK 0x03
#define CMD_MAGIC_FLAG 0xAABB

#define ADM_RAID_CMD_ERR 0xC0
#define USER_CMD_TIMEOUT_MS 600000          // 部分命令需要擦除flash耗时较长（比如升级），暂定10分钟，不指定驱动默认1分钟超时
#define USER_CMD_DEFAULT_TIMEOUT_MS 180000   // lib库设置默认超时三分钟
#define PD_UPGRADE_TIMEOUT_MS 120000         // 物理盘固件升级超时时间
#define LIB_PARAM_SIZE 24
#define IOCTL_TIME_OUT 0  // 60s
#define MAX_VERIFY_VD_PRRIOD 60
#define WORD_TO_DWORD 4
#define DISK_UPGRADE_OFFSET 3
#define MIN_VD_NAME_LEN 4

#define FIRMWARE_NAME_LEN 12     /* the max length of firmware name */
#define MAX_MODEL_NAME_LEN 40    /* the max length of model name */
#define MAX_RG_NAME_LEN 32       /* the max length of raid group name */
#define MAX_LUN_NAME_LEN 32      /* the max length of lun name */
#define CMD_MEMB_DISK_MAX_NUM 80U
#define MAX_RG_HOTSPARE_DISK_NUM 8
#define ADM_MAX_DATA_SIZE 0x1000U
#define HOST_MAX_LUN_NAME_LEN 20    // BMC界面对外展示lun名称最大允许长度

#define CMD_MAX_EXP_NUM 70
#define MAX_PHY_NUM 50
#define MAX_INCONSISTENT_STRIPE_NUM 100
#define ADM_MAX_FILE_NAME_LENGTH 255
#define CMD_MAX_FOREIGN_NUM 256
#define MASK_OF_STRUCT_INDEX_FIRST 1 // used for ADM_CMD_SET_CONSIS_CHECK
#define MASK_OF_STRUCT_INDEX_SECOND 2
#define MASK_OF_STRUCT_INDEX_THIRD 4
#define MASK_OF_STRUCT_INDEX_FOURTH 8

#define ADM_SCSI_CDB_MAX_LEN 32
#define ADM_SCSI_CDB_SENSE_MAX_LEN 96
#define ADM_SCSI_CDB_SENSE_LEN32 32
#define SET_CDW_NUM(U32A, U8_ARR)                                                   \
    {                                                                               \
        U32A = U8_ARR[0] << 24 | U8_ARR[1] << 16 | U8_ARR[2] << 8 | U8_ARR[3] << 0; \
    }

#define ADM_RAID_SET 0xc0
#define ADM_RAID_WRITE 0xc1
#define ADM_RAID_READ 0xc2
#define ADM_RAID_PARAM_WRITE 0xc3
#define ADM_RAID_READ_FROM_CQE 0xc4

#define ADM_RAID_INVALID_VALUE16 0xFFFF

#define MAX_ASYNC_EVENT_NUM 4
#define STOR_MAX_ASYNC_EVENT_NUM 4
#define NVME_MAX_ASYNC_EVENT_NUM 1
#define ADM_RAID_INVALID_PROGRESS 0xFF
#define DEV_DIR_IN_LINUX "/dev/"

#define JSON_C_TO_STRING_TAB 10 // 测试得出此值，自带宏失效，无法得到想要结果
#define LAST_MAJOR_VERSION      3
#define LAST_SUB_VERSION        0
#define LAST_FULL_VERSION       ((LAST_MAJOR_VERSION << 8) + LAST_SUB_VERSION)
#define GET_MAJOR_VERSION(val)  (((val) >> 8) & 0xff)
#define GET_SUB_VERSION(val)    ((val) & 0xff)
#define GET_BUF_LEN_ALIG8(val)  (((val) & 0x7) ? ((((val) >> 3) << 3) + 8) : (val))
#define MAX_VD_NAME_LEN 24

/* bit offset used for SUBOPCODE:ADM_CMD_SET_PATROLREAD */
enum adm_patrolread_bits {
    PERIOD_BIT,
    RATE_BIT,
    DISK_NUM_BIT,
    DELAY_BIT,
    BUTT_BIT
};

/* bit offset used for ccheck parameter mask */
enum adm_ccheck_bits {
    CCPERIOD_BIT,
    CCSPEED_BIT,
    CCREPAIR_MODE_BIT,
    CCDELAY_BIT,
    CCHECK_BUTT_BIT
};

/* bit offset used for kick slowdisk cycles parameter mask */
enum adm_kick_slowdisks_bits {
    KICK_PERIOD_BIT,
    KICK_SUM_BIT,
    KICK_REFRESH_BIT
};

/*
 * 和单盘进度相关的bit位，bit0:重构，bit1:巡检，bit2:回拷，bit3:擦除
 * 详见cmd_show_disk的action成员变量
 */
enum adm_disk_progress_bits {
    ADM_DISK_RECONSTRUCTION_BIT,
    ADM_DISK_PATROLREAD_BIT,
    ADM_DISK_COPYBACK_BIT,
    ADM_DISK_SANITIZE_BIT,
    ADM_DISK_BUTT_BIT
};

/* bit offset */
enum task_type_bit_val {
    DISK_TASK_RECONSTRUCTION = 0x1 << ADM_DISK_RECONSTRUCTION_BIT,
    DISK_TASK_PATROLREAD =     0x1 << ADM_DISK_PATROLREAD_BIT,
    DISK_TASK_COPYBACK =       0x1 << ADM_DISK_COPYBACK_BIT,
    DISK_TASK_SANITIZE =       0x1 << ADM_DISK_SANITIZE_BIT,
};

/* set disk online or offline in struct cmd_disk_on_offline */
enum adm_disk_online_offline {
    ADM_DISK_OFFLINE,
    ADM_DISK_ONLINE
};

enum sc_code {
    SC_CHECK_SENSE = 1, /* 命令执行失败，主机侧需要查看sense buffer */
    SC_FW_ERR,      /* 命令执行失败 */
    SC_ABT_NOT_FOUND, /* io abort时卡侧没有找到需要abort的io */
    SC_PERMISSION, /* 本控已经处于被切主的状态，阻挡的命令都回复无权限 */
    SC_RETRY, /* 芯片处于复位模式中，所有被阻挡命令都回复需要重试 */
    SC_LEFT_IO, /* 切主命令，对控还有部分盘没有排空完 */
    SC_ABORTED_WITHOUT_NCQ,  /* 标记被abort的IO, 并且无NCQ TAG数据 */
    SC_ABORTED_WITH_NCQ /* 标记被abort的IO, 并且携带NCQ TAG数据 */
};

enum admin_channel {
    ADM_PCIE,
    ADM_MCTP_OVER_PCIE,
    ADM_I2C_SMBUS
};

enum sct_type {
    SCT_RAID = 0x07,
};

enum raid_aen_info {
    TIME_SYNC,
    PHY_EVENT,
    DISK_EVENT,
    CHIP_EVENT
};

enum raid_aen_type {
    AEN_RAID = 0x07,
};

enum adm_cache_del_lun_method {
    ADM_CACHE_METHOD_DEFAULT,
    ADM_CACHE_METHOD_FORCE,
    ADM_CACHE_METHOD_DISCARD_CACHE // 舍弃未下刷的脏数据
};

/* Begin 与raid相关枚举变量定义，后期修改代码时，需要对齐定义 */
enum user_input_raid_level {
    USER_INPUT_RAID0_LEVEL = 0,
    USER_INPUT_RAID1_LEVEL = 1,
    USER_INPUT_RAID5_LEVEL = 5,
    USER_INPUT_RAID6_LEVEL = 6,
    USER_INPUT_RAID10_LEVEL = 10,
    USER_INPUT_RAID50_LEVEL = 50,
    USER_INPUT_RAID60_LEVEL = 60,
};
enum raid_group_level {
    CMD_RAID_LEVEL_RAID0 = 0,
    CMD_RAID_LEVEL_RAID1,
    CMD_RAID_LEVEL_RAID5,
    CMD_RAID_LEVEL_RAID6,
    CMD_RAID_LEVEL_RAID10,
    CMD_RAID_LEVEL_RAID50,
    CMD_RAID_LEVEL_RAID60,
    CMD_RAID_LEVEL_BUTT,
    CMD_RAID_DEFAULT_VALUE = 255
};
enum adm_raid_status {
    ADM_RAID_STATUS_NORMAL = 1,
    ADM_RAID_STATUS_FAULT,
    ADM_RAID_STATUS_DEGRADE,
    ADM_RAID_STATUS_BUTT
};
enum adm_disk_status {
    ADM_DISK_STATUS_NORMAL = 1,
    ADM_DISK_STATUS_FAULT,
    ADM_DISK_STATUS_RECONSTRUCTING,
    ADM_DISK_STATUS_RECONSTRUCTED,
    ADM_DISK_STATUS_COPYBACKING,
    ADM_DISK_STATUS_FREESPARE,
    ADM_DISK_STATUS_USEDSPARE,
    ADM_DISK_STATUS_PRECOPYING,
    ADM_DISK_STATUS_DIAGNOSING,
    ADM_DISK_STATUS_BUTT
};

enum adm_pd_status {
    ADM_PD_STATUS_ONLINE = 1,
    ADM_PD_STATUS_OFFLINE,
    ADM_PD_STATUS_CFG_FAULT,
    ADM_PD_STATUS_UCFG_GOOD,
    ADM_PD_STATUS_UCFG_FAULT,
    ADM_PD_STATUS_HOT_SPARE,
    ADM_PD_STATUS_FOREIGN,
    ADM_PD_STATUS_DIAGNOSING,
    ADM_PD_STATUS_RECONSTRUCTING,
    ADM_PD_STATUS_COPYBACK,
    ADM_PD_STATUS_INCOMPAT,
    ADM_PD_STATUS_UNSUPPORTE,
    ADM_PD_STATUS_UNKNOWN,
    ADM_PD_STATUS_BUTT,
};

enum adm_cut_capacity {
    ADM_MIN_CC = 512,
    ADM_MAX_CC = 1024
};
enum adm_susize {
    ADM_SUSZ_DEFAULT = 0,
    ADM_SUSZ_32 = 32,
    ADM_SUSZ_64 = 64,
    ADM_SUSZ_128 = 128,
    ADM_SUSZ_256 = 256,
    ADM_SUSZ_512 = 512,
    ADM_SUSZ_1024 = 1024
};
enum adm_write_type {
    ADM_WB = 1,
    ADM_WT,
    ADM_WBE
};
enum adm_set_vd_attr_type {
    VD_ATTR_WCACHE,
    VD_ATTR_RCACHE,
    VD_ATTR_NAME,
    VD_ATTR_MAP_TYPE,
    VD_ATTR_PF,
    VD_ATTR_ACCESS
};
enum adm_read_type {
    ADM_NO_AHEAD,
    ADM_AHEAD
};
enum adm_init_type {
    ADM_INIT_FAST,
    ADM_INIT_BACKGROUND,
    ADM_INIT_FRONT
};

enum adm_disk_cahe_switch {
    ADM_DISK_CAHE_SWITCH_OFF,
    ADM_DISK_CAHE_SWITCH_ON,
    ADM_DISK_CAHE_DEFAULT
};

enum adm_disk_delete_sanitize {
    ADM_DELETE_LUN_SANITIZE_OFF,
    ADM_DELETE_LUN_SANITIZE_ON
};

enum adm_lun_freq {
    ADM_LUN_FREQ_COLD = 0,
    ADM_LUN_FREQ_HOT,
    ADM_LUN_FREQ_NORMAL,
    ADM_LUN_FREQ_BUTT
};

enum adm_map_type {
    ADM_RAID_SCSI,
    ADM_RAID_NVME
};

enum adm_lun_status {
    ADM_LUN_STATUS_NORMAL = 1, /* 正常 */
    ADM_LUN_STATUS_FAULT = 2,  /* 故障 */
    ADM_LUN_STATUS_DEGRADE = 3,
    ADM_LUN_STATUS_NOTFORMATTED = 4,   /* 未格式化 */
    ADM_LUN_STATUS_FORMATTING = 5,     /* 正在格式化 */
    ADM_LUN_STATUS_SANITIZING = 6,     /* 正在进行数据销毁 */
    ADM_LUN_STATUS_INITIALIZING = 7,   /* ThinLun初始化状态 */
    ADM_LUN_STATUS_INITIALIZEFAIL = 8, /* ThinLun初始化失败状态 */
    ADM_LUN_STATUS_DELETING = 9,       /* ThinLun删除状态 */
    ADM_LUN_STATUS_DELETEFAIL = 10,    /* ThinLun删除中失败 */
    ADM_LUN_STATUS_WRITE_PROTECT = 11  /* ThinLun只读状态 */
};

enum adm_bgtask_type {
    ADM_BGTASK_TYPE_RW_FGR,       /* 读写前台IO ，这个值不会取 */
    ADM_BGTASK_TYPE_COPYBACK,     /* 回拷后台IO */
    ADM_BGTASK_TYPE_PRECOPY,      /* 冗余拷贝后台IO */
    ADM_BGTASK_TYPE_BGINIT,       /* 格式化后台IO */
    ADM_BGTASK_TYPE_REBUILD,      /* 重构后台IO */
    ADM_BGTASK_TYPE_SANITIZE,     /* 数据销毁后台IO */
    ADM_BGTASK_TYPE_EXPANDRG,     /* 动态特性后台IO */
    ADM_BGTASK_TYPE_CONSIS_CHECK, /* 一致性校验 */
    ADM_BGTASK_TYPE_PATROLREAD,   /* 巡检 */
    ADM_BGTASK_TYPE_FRONT_INIT,   /* 前台格式化 */
};

enum adm_card_bgtask_op_type {
    ADM_BGTASK_STOP_TYPE,
    ADM_BGTASK_START_TYPE,
    ADM_BGTASK_SET_TYPE
};

enum adm_bgtask_rate_speed {
    ADM_BGTASK_RATE_LOW = 1,
    ADM_BGTASK_RATE_MIDDLE,
    ADM_BGTASK_RATE_HIGH
};
enum adm_vd_repair_type { // 注意:此开关值与一般含义相反
    ADM_AUTOCC_REPAIRE_AUTO, // 输出显示为:on (值为0)
    ADM_AUTOCC_REPAIRE_MANU
};

enum adm_back_rginfo_ground_type {
    ADM_RAID_GROUP_REBUILD,
    ADM_RAID_GROUP_COPYBACK,
    ADM_RAID_GROUP_PROCOPY,
    ADM_RAID_GROUP_EXPANDRG
};

enum adm_is_rg_copyback_all {
    ADM_RG_COPYBACK_ONE,
    ADM_RG_COPYBACK_ALL
};

enum adm_vd_access_policy {
    ADM_VD_ACCESS_READ_WRITE, // 可读可写
    ADM_VD_ACCESS_READ_ONLY,  // 只读
    ADM_VD_ACCESS_BUTT
};

/* End 与raid相关枚举变量定义，后期修改代码时，需要对齐定义 */

/* Begin 与后端相关枚举变量定义，后期修改代码时，需要对齐定义 */
enum adm_pdlist_intf {
    ADM_DEVICE_TYPE_SAS = 1,
    ADM_DEVICE_TYPE_EXP = 2,
    ADM_DEVICE_TYPE_SATA = 8,
    ADM_DEVICE_TYPE_PLANE = 9,
    ADM_DEVICE_TYPE_NVME = 10,
    ADM_DEVICE_TYPE_UNKNOW,
    ADM_DEVICE_TYPE_BUTT
};

enum adm_pdlist_type {
    ADM_DISK_MEDIUM_TYPE_HDD = 0,
    ADM_DISK_MEDIUM_TYPE_SSD = 1,
    ADM_DISK_MEDIUM_TYPE_UNKNOWN
};

enum adm_pdlist_status {
    ADM_DISK_STATE_NEW,
    ADM_DISK_STATE_GOOD,
    ADM_DISK_STATE_BAD,
    ADM_DISK_STATE_OUT,
    ADM_DISK_STATE_INVALID,
    ADM_DISK_STATE_OFFLINE,
    ADM_DISK_STATE_PREFAIL,
    ADM_DISK_STATE_DISKSLOW,
    ADM_DISK_STATE_FOREIGN,
    ADM_DISK_STATE_INCOMPATIBLE,
    ADM_DISK_STATE_UNSUPPORTED,
    ADM_DISK_STATE_DIAGNOSE,
    ADM_DISK_STATE_PENDING,
    ADM_DISK_STATE_BUTT
};

enum adm_sas_link_speed {
    ADM_SAS_LINK_RATE_FREE = 0,   // no link
    ADM_SAS_LINK_RATE_1_5 = 8,    // 1.5G
    ADM_SAS_LINK_RATE_3_0 = 9,    // 3.0G
    ADM_SAS_LINK_RATE_6_0 = 10,   // 6.0G
    ADM_SAS_LINK_RATE_12_0 = 11,  // 12G
    ADM_SAS_LINK_RATE_24_0 = 12,  // 24G
    ADM_SAS_LINK_RATE_BUTT
};

enum adm_disk_size {
    ADM_DISK_SIZE_TYPE_NOT_REPORT = 0,
    ADM_DISK_SIZE_TYPE_525 = 1,  // 5.25inch
    ADM_DISK_SIZE_TYPE_35,       // 3.5inch
    ADM_DISK_SIZE_TYPE_25,       // 2.5inch
    ADM_DISK_SIZE_TYPE_18,       // 1.8inch
    ADM_DISK_SIZE_TYPE_LESS18,   // less than 1.8inch
    ADM_DISK_SIZE_TYPE_UNKNOWN
};

enum adm_disk_type {
    ADM_DISK_TYPE_FREE = 1,         /* 自由盘 */
    ADM_DISK_TYPE_MEMBER = 2,       /* 成员盘 */
    ADM_DISK_TYPE_SPARE = 3,        /* 热备盘 */
    ADM_DISK_TYPE_SSDC = 4,         /* SSD CACHE成员盘 */
    ADM_DISK_TYPE_PASS_THROUGH = 5, /* 直通盘 */
    ADM_DISK_TYPE_SMR = 6,          /* 只能当做硬盘使用，无法组在raid中使用 */
    ADM_DISK_TYPE_RAW_DISK = 7,     /* 裸盘 */
    ADM_DISK_TYPE_SED = 8,          /* 加密盘 */
    ADM_DISK_TYPE_BUTT
};

enum adm_halflife_tag {
    ADM_NO_HALF_LIFE,
    ADM_HAVE_HALF_LIFE
};

enum adm_lun_map_type {
    ADM_MAP_TYPE_ST,
    ADM_MAP_TYPE_NT
};

enum adm_lun_type {
    ADM_LUN_TYPE_NORMAL,
    ADM_LUN_TYPE_PRIVATE,
    ADM_LUN_TYPE_EXPAND
};

enum adm_sas_phy_func {
    ADM_SAS_PHY_HARD_RESET = 1,
    ADM_SAS_PHY_LINK_RESET,
    ADM_SAS_PHY_DISABLE,
    ADM_SAS_PHY_ENABLE,
    ADM_SAS_PHY_SPEED,
    ADM_SAS_PHY_ENABLELOOP,
    ADM_SAS_PHY_DISABLELOOP,
    ADM_SAS_PHY_BUTT
};

enum adm_phy_enable {
    ADM_PHY_DISABLE,
    ADM_PHY_ENABLE
};

enum adm_bde_device_type {
    /* these are SAS protocol defined (attached device type field) */
    ADM_SAS_PHY_UNUSED = 0,
    ADM_SAS_END_DEVICE = 1,
    ADM_SAS_EDGE_EXPANDER_DEVICE = 2,
    ADM_SAS_SATA_DEV = 3,
    ADM_SAS_SATA_PENDING = 4,
    ADM_SAS_SES_DEVICE = 5,
    ADM_BDE_NVME_DEVICE = 6,
    ADM_BDE_DEVICE_BUTT
};

enum adm_sanitize_lun {
    ADM_SANITIZE_SIMPLE = 1,  /* 擦一轮 */
    ADM_SANITIZE_NORMAL = 3,  /* 擦三轮 */
    ADM_SANITIZE_THOROUGH = 9 /* 擦九轮 */
};

enum adm_sanitize_mode {
    ADM_SANITIZE_OVERWRITE = 1,
    ADM_SANITIZE_BLOCKERASE = 2,
    ADM_SANITIZE_CRYPTOERASE = 3,
};

enum adm_precopy_switch {
    ADM_PRECOPY_SWITCH_ON = 1,
    ADM_PRECOPY_SWITCH_OFF
};

enum adm_chgegycpbsw_switch {
    ADM_CHGEGYCPBSW_ON = 1,
    ADM_CHGEGYCPBSW_OFF,
};

enum adm_diskbst_type {
    ADM_DISKBST_PHYSICAL,
    ADM_DISKBST_LOGICAL
};

enum adm_pcie_link_state {
    PCIE_STATE_LINKDOWN,
    PCIE_STATE_LINKUP
};

enum adm_pcie_link_speed {
    PCIE_LINK_SPEED_GEN1 = 1,
    PCIE_LINK_SPEED_GEN2,
    PCIE_LINK_SPEED_GEN3,
    PCIE_LINK_SPEED_GEN4
};

enum adm_disk_secerass_times {
    ADM_ERASE_TIME_SIMPLE = 1,
    ADM_ERASE_TIME_NORMAL = 3,
    ADM_ERASE_TIME_THOROUGH = 9
};

enum adm_raid_report_pf {
    ADM_RAID_PF0,
    ADM_RAID_PF1,
    ADM_RAID_AUTO
};

enum adm_dif_switch_status {
    ADM_DIF_SWITCH_OFF,
    ADM_DIF_SWITCH_ON
};

enum boot_pinnedcache_switch {
    SET_BOOT_PINNEDCACHE_OFF = 0,
    SET_BOOT_PINNEDCACHE_ON = 1
};

enum adm_scsi_cdb_len_teype {
    ADM_SCSI_CDB_LEN_SIX_BYTE = 6,
    ADM_SCSI_CDB_LEN_TEN_BYTE = 10,
    ADM_SCSI_CDB_LEN_TWELVE_BYTE = 12,
    ADM_SCSI_CDB_LEN_SIXTEEN_BYTE = 16,
    ADM_SCSI_CDB_LEN_THIRTY_TWO_BYTE = 32
};

enum adm_ldlist_sed_disk_type {
    ADM_LDLIST_NOT_SED_DISK,
    ADM_LDLIST_SED_DISK
};

enum adm_flash_test_type {
    ADM_CMD_FLASH_TEST_NAND,
    ADM_CMD_FLASH_TEST_NOR
};

enum adm_general_switch_value {
    ADM_GENERAL_SWITCH_OFF,
    ADM_GENERAL_SWITCH_ON
};

enum adm_card_passthru_type_value {
    ADM_CARD_PASSTHRU_OFF,
    ADM_CARD_PASSTHRU_JBOD,
    ADM_CARD_PASSTHRU_RAWDRIVE
};

enum adm_vd_sec_type_value {
    ADM_VD_SEC_TYPE_512N,
    ADM_VD_SEC_TYPE_512E,
    ADM_VD_SEC_TYPE_4KN,
    ADM_VD_SEC_TYPE_520,
    ADM_VD_SEC_TYPE_512_8,
    ADM_VD_SEC_TYPE_4160,
    ADM_VD_SEC_TYPE_4096_8_56,
    ADM_VD_SEC_TYPE_4096_56_8
};

enum adm_vd_sanitize_result {
    ADM_SANITIZE_RESULT_NONE = 1,
    ADM_SANITIZE_RESULT_RUNNING,
    ADM_SANITIZE_RESULT_COMPLETE,
    ADM_SANITIZE_RESULT_FAILED
};
enum adm_disk_physical_status {
    CMD_DISK_STATE_NEW,           // 初始状态
    CMD_DISK_STATE_GOOD,          // 用户可用的状态
    CMD_DISK_STATE_BAD,           // 故障盘
    CMD_DISK_STATE_OUTING,        // 正在拔盘
    CMD_DISK_STATE_OFFLINE,       // 暂时保留
    CMD_DISK_STATE_ONLINE,        // 暂时保留
    CMD_DISK_STATE_PREFAIL,       // 预失效盘
    CMD_DISK_STATE_DISKSLOW,      // 慢盘
    CMD_DISK_STATE_FOREIGN,       // raid配置兼容但是不是本地raid组的盘
    CMD_DISK_STATE_INCOMPATIBLE,  // raid配置不兼容的盘
    CMD_DISK_STATE_UNSUPPORTED,   // 不支持的盘
    CMD_DISK_STATE_DIAGNOSE,      // 处在诊断状态的盘
    CMD_DISK_STATE_PENDING,       // 等待上报的盘
    CMD_DISK_STATE_BUTT
};

enum adm_set_boot_dev {
    CMD_BOOT_OFF,
    CMD_BOOT_ON
};

enum adm_show_boot_type {
    CMD_BOOT_TYPE_VD,
    CMD_BOOT_TYPE_PD
};

enum adm_new_switch_value {
    ADM_NEW_SWITCH_ON = 1,
    ADM_NEW_SWITCH_OFF
};

enum adm_hot_spare_status {
    HOT_SPARE_NON = 0,      // 非热备盘
    HOT_SPARE_GLOBAL,       // 全局热备盘
    HOT_SPARE_LOCAL,        // 局部热备盘
    HOT_SPARE_INVALID
};
/* End 与后端相关枚举变量定义，后期修改代码时，需要对齐定义 */

/* 区分需要记录操作日志的命令，频繁调用的命令建议去掉 */
#define CMD_NEED_PRINT(x) ((x) != ADM_CMD_GET_UART_LOG && (x) != ADM_CMD_UPDATE_FW \
                          && (x) != ADM_CMD_SEND_UART_LOG && (x) != ADM_CMD_SHOW_DISK \
                          && (x) != ADM_CMD_GET_LOG && (x) != ADM_CMD_GET_DUMP \
                          && (x) != ADM_CMD_TRANSPORT_FILE && (x) != ADM_CMD_GET_CTRL_INFO \
                          && (x) != ADM_CMD_GET_CTRL_STATUS && (x) != ADM_CMD_SHOW_PASSTHRU_TYPE \
                          && (x) != ADM_CMD_GET_SC_INFO && (x) != ADM_CMD_SHOW_EXP_PHY_STATUS \
                          && (x) != ADM_CMD_SHOW_VDS_ID && (x) != ADM_CMD_SHOW_VD_INFO \
                          && (x) != ADM_CMD_SHOW_RG_INFO && (x) != ADM_CMD_SHOW_PDS_ID \
                          && (x) != ADM_CMD_SHOW_RGS_ID)

/*****************************************************************************
 Description  : subopcode  共占11位
                高2位用于区分命令类型:
                00 : 商用类
                01 : dfx
                10 : fanout + 保留（用于dfx和ddi命令扩充）
                11 : ddi
                针对商用类命令，次高两位用于区分命令对象:
                00 : card类型
                01 : rg 和 vd类型()
                10 : disk类型
                11 : 系统类 + 保留
*****************************************************************************/
enum {
    ADM_CMD_UPDATE_FW = 0x0037,            // 55
    ADM_CMD_ACTIVE_FW = 0x0038,            // 56
    ADM_CMD_TRANSPORT_FILE = 0x0039,       // 57
    ADM_CMD_GET_FW_DATA = 0x003A,          // 58
    ADM_CMD_GET_UART_LOG = 0x003B,         // 59
    ADM_CMD_SEND_UART_LOG = 0x003C,        // 60
    ADM_CMD_HDTEST_FLASH = 0x003D,         // 61
    ADM_CMD_HDTEST_SRAM = 0x003E,          // 62
    ADM_CMD_HDTEST_SEC = 0x003F,           // 63
    ADM_CMD_SHOW_EYE_DIAGRAM = 0x0040,     // 64
    ADM_CMD_SDS_SET_DFE = 0x0041,          // 65
    ADM_CMD_SDS_SHOW_DFE = 0x0042,         // 66
    ADM_CMD_SDS_SET_CTLE = 0x0043,         // 67
    ADM_CMD_SDS_SHOW_CTLE = 0x0044,        // 68
    ADM_CMD_SDS_SET_FFE = 0x0045,          // 69
    ADM_CMD_SDS_SHOW_FFE = 0x0046,         // 70
    ADM_CMD_SDS_SET_SWING = 0x0047,        // 71
    ADM_CMD_SDS_SHOW_SWING = 0x0048,       // 72
    ADM_CMD_SHOW_PCIE_LINK = 0x0049,       // 73
    ADM_CMD_SHOW_PCIE_LTSSM = 0x004A,      // 74
    ADM_CMD_CLEAR_PCIE_LTSSM = 0x004B,     // 75
    ADM_CMD_SET_TX_MARGIN = 0x004C,        // 76
    ADM_CMD_SHOW_DIEID = 0x004E,           // 78
    ADM_CMD_SGPIO_LIGHT = 0x004F,          // 79
    ADM_CMD_SHOW_LINK_ERROR = 0x0050,      // 80
    ADM_CMD_CLEAN_LINK_ERROR = 0x0051,     // 81
    ADM_CMD_CLEAR_NAND = 0x0052,           // 82
    ADM_CMD_SRAM_ECC_INFO = 0x0053,        // 83
    ADM_CMD_DDR_INFO = 0x0054,             // 84
    ADM_CMD_SHOW_EFUSE = 0x0055,           // 85
    ADM_CMD_SET_EFUSE = 0x0056,            // 86
    ADM_CMD_SHOW_CHIP_ADC = 0x0057,        // 87
    ADM_CMD_SHOW_TEMP = 0x0058,            // 88
    ADM_CMD_SET_SAS_ADDR = 0x0059,         // 89
    ADM_CMD_SET_ELECTRON_LABEL = 0x005A,   // 90
    ADM_CMD_SET_LOOP_TYPE = 0x005B,        // 91
    ADM_CMD_SHOW_LOOP_TYPE = 0x005C,       // 92
    ADM_CMD_SHOW_PRBS_STA = 0x005D,        // 93
    ADM_CMD_SET_PRBS = 0x005E,             // 94
    ADM_CMD_SHOW_PRBS_TYPE = 0x005F,       // 95
    ADM_CMD_SHOW_VOL = 0x0060,             // 96
    ADM_CMD_SHOW_KEY_INFO = 0x0061,        // 97
    ADM_CMD_SHOW_CDR = 0x0062,             // 98
    ADM_CMD_SET_CDR = 0x0063,              // 99
    ADM_CMD_SET_PN = 0x0064,               // 100
    ADM_CMD_SHOW_PN = 0x0065,              // 101
    ADM_CMD_SHOW_FW_VERSION = 0x0066,      // 102
    ADM_CMD_GET_LOG = 0x0067,              // 103
    ADM_CMD_SET_BP_INJECT_DIE = 0x0068,    // 104
    ADM_CMD_SET_BP_DQS_DEFLECT = 0x0069,   // 105
    ADM_CMD_SET_MBIST = 0x006A,            // 106
    ADM_CMD_SHOW_MBIST = 0x006B,           // 107
    ADM_CMD_SET_BP_NAND = 0x006C,          // 108
    ADM_CMD_SET_EXMBIST = 0x006D,          // 109
    ADM_CMD_SHOW_EXMBIST = 0x006F,         // 111
    ADM_CMD_SHOW_ELECTRON_LABEL = 0x0070,  // 112
    ADM_CMD_CLEAR_LOG = 0x0071,            // 113
    ADM_CMD_SET_SELF_LEARN = 0x0072,       // 114
    ADM_CMD_SHOW_SM_CAP_V = 0x0073,        // 115
    ADM_CMD_SET_PRBS_CTRL = 0x0074,        // 116
    ADM_CMD_SET_BP_FS_V = 0x0075,          // 117
    ADM_CMD_SET_BP_FUN_V = 0x0076,         // 118
    ADM_CMD_GET_BP_INFO_V = 0x0077,        // 119
    ADM_CMD_CLEAR_EQUIP_CFG = 0x0078,      // 120
    ADM_CMD_GET_BP_METADATA_V = 0x0079,    // 121
    ADM_CMD_SET_TSP_FREQ = 0x007A,         // 122
    ADM_CMD_SHOW_TSP_FREQ = 0x007B,        // 123

    /* card对象类型命令 */
    ADM_CMD_GET_PDLIST_NUM = 0x501,              // 1281
    ADM_CMD_SHOW_PDLIST = 0x502,                 // 1282
    ADM_CMD_SET_BGRATE = 0x503,                  // 1283
    ADM_CMD_SHOW_BGTASK = 0x504,                 // 1284
    ADM_CMD_SET_COPYBACK_SW = 0x505,             // 1285
    ADM_CMD_SHOW_COPYBACK_SW = 0x506,            // 1286
    ADM_CMD_SET_PRECOPY_SW = 0x507,              // 1287
    ADM_CMD_SHOW_PRECOPY_SW = 0x508,             // 1288
    ADM_CMD_SET_PATROLREAD = 0x509,              // 1289
    ADM_CMD_SHOW_PATROLREAD = 0x50a,             // 1290
    ADM_CMD_SET_EHSP_SW = 0x50b,                 // 1291
    ADM_CMD_SHOW_EHSP_SW = 0x50c,                // 1292
    ADM_CMD_SET_PASSTHRU_TYPE = 0x50d,           // 1293
    ADM_CMD_SHOW_PASSTHRU_TYPE = 0x50e,          // 1294
    ADM_CMD_SET_DDMA = 0x50f,                    // 1295
    ADM_CMD_SHOW_DDMA = 0x510,                   // 1296
    ADM_CMD_SHOW_BOOT = 0x511,                   // 1297
    ADM_CMD_CLEAR_FNCFG = 0x512,                 // 1298
    ADM_CMD_IMPORT_FNCFG = 0x513,                // 1299
    ADM_CMD_SHOW_FNCFG = 0x514,                  // 1300
    ADM_CMD_SHOW_FNCFGLIST = 0x515,              // 1301
    ADM_CMD_SHOW_BST = 0x516,                    // 1302
    ADM_CMD_GET_CTRL_INFO = 0x517,               // 1303
    ADM_CMD_RESET_CFG = 0x518,                   // 1304
    ADM_CMD_SHOW_PDS_ID = 0x51b,                 // 1307
    ADM_CMD_SHOW_PCIE_ERROR = 0x51c,             // 1308
    ADM_CMD_CLEAR_PCIE_ERROR = 0x51d,            // 1309
    ADM_CMD_SCSI_PASSTHROUGH = 0x51e,            // 1310
    ADM_CMD_NVMe_PASSTHROUGH = 0x51f,            // 1311
    ADM_CMD_GET_CTRL_STATUS = 0x520,             // 1312
    ADM_CMD_GET_SC_INFO = 0x521,                 // 1313
    ADM_CMD_GET_BST_NUM = 0x522,                 // 1314
    ADM_CMD_SET_CONSIS_CHECK = 0x523,            // 1315
    ADM_CMD_SHOW_CONSIS_CHECK = 0x524,           // 1316
    ADM_CMD_SET_SMART_POLLING = 0x525,           // 1317
    ADM_CMD_SHOW_SMART_POLLING = 0x526,          // 1318
    ADM_CMD_SET_BOOT_PINNEDCACHE = 0x527,        // 1319
    ADM_CMD_SHOW_BOOT_PINNEDCACHE = 0x528,       // 1320
    ADM_CMD_I2C_DATATRANCE = 0x529,              // 1321
    ADM_CMD_SHOW_REG = 0x52a,                    // 1322
    ADM_CMD_SET_REG = 0x52b,                     // 1323
    ADM_CMD_SET_GLBCFG = 0x52c,                  // 1324
    ADM_CMD_SHOW_PHY_CFG = 0x52d,                // 1325
    ADM_CMD_SHOW_ALLREG = 0x52e,                 // 1326
    ADM_CMD_CLEAR_NORFLASH = 0x52f,              // 1327
    ADM_CMD_DETE_FLASH = 0x530,                  // 1328
    ADM_CMD_SET_POWERSAVE = 0x532,               // 1330
    ADM_CMD_SET_CAP_LEARN = 0x533,               // 1331
    ADM_CMD_SHOW_SELFLEARN = 0x534,              // 1332
    ADM_CMD_SET_ALARMTEMP = 0x535,               // 1333
    ADM_CMD_SET_SELFLEARN_CYCLE = 0x536,         // 1334
    ADM_CMD_QUERY_ACTIVE_TYPE = 0x537,           // 1335
    ADM_CMD_RESET_DEFAULT_CFG = 0x538,           // 1336
    ADM_CMD_SHOW_RAID_GLB_CFG = 0x539,           // 1337
    ADM_CMD_REGISTER_HOST_EVENT = 0x53a,         // 1338
    ADM_CMD_NOTIFY_POWERCYCLE_RESULT = 0x53b,    // 1339
    ADM_CMD_SET_BACKUPCAP = 0x53c,               // 1340
    ADM_CMD_GET_DUMP = 0x53d,                    // 1341
    ADM_CMD_SET_DUMP = 0x53e,                    // 1342
    ADM_CMD_SHOW_DDR_EYE = 0x53f,                // 1343
    ADM_CMD_SET_RETENTION = 0x540,               // 1344
    ADM_CMD_SET_REPORT_DRIVE_NUM = 0x541,        // 1345
    ADM_CMD_SHOW_PINNEDCACHE_LIST = 0x542,       // 1346
    ADM_CMD_SHOW_CAP_LEARN = 0x543,              // 1347
    ADM_CMD_SET_LOG_LEVEL = 0x544,               // 1348
    ADM_CMD_SHOW_LOG_LEVEL = 0x545,              // 1349
    ADM_CMD_SET_KICK_DISK_CYCLE = 0x546,         // 1350
    ADM_CMD_USER_RECOVERY = 0x547,               // 1351
    ADM_CMD_USER_RECOVERY_PROCESS = 0x548,       // 1352
    ADM_CMD_USER_RECOVERY_SET_DEFAULT = 0x549,   // 1353
    ADM_CMD_USER_RECOVERY_SHOW_DEFAULT = 0x54a,  // 1354
    ADM_CMD_USER_RECOVERY_CLEAR_ERROR = 0x54b,   // 1355
    ADM_CMD_SET_DRIVER_FC = 0x54c,               // 1356
    ADM_CMD_SHOW_DRIVER_FC = 0x54d,              // 1357
    ADM_CMD_SET_DPHSTARTMOD = 0x54e,             // 1358
    ADM_CMD_SHOW_DPHSTARTMOD = 0x54f,            // 1359
    ADM_CMD_GET_NAND_LOG = 0x550,                // 1360
    ADM_CMD_SET_NAND_LOG_IDX = 0x551,            // 1361
    ADM_CMD_SET_DHA_SW = 0x552,                  // 1362
    ADM_CMD_SHOW_DHA_SW = 0x553,                 // 1363
    ADM_CMD_SHOW_KICK_DRIVE_STATUS = 0x554,      // 1364
    ADM_CMD_SHOW_DRIVER_FC_EXPAND = 0x555,       // 1365
    ADM_CMD_SET_MERGE = 0x556,                   // 1366
    ADM_CMD_SHOW_MERGE = 0x557,                  // 1367
    ADM_CMD_SET_QUEUE_DEPTH = 0x558,             // 1368
    ADM_CMD_SHOW_QUEUE_DEPTH = 0x559,            // 1369
    ADM_CMD_SET_VD_FC = 0x55a,                   // 1370
    ADM_CMD_SHOW_VD_FC = 0x55b,                  // 1371
    ADM_CMD_SET_VD_MERGE = 0x55c,                // 1372
    ADM_CMD_SHOW_VD_MERGE = 0x55d,               // 1373
    ADM_CMD_SHOW_LOAD_FW_VERSION = 0x55e,        // 1374

    /* rg 对象类型命令 */
    ADM_CMD_CREATE_RG = 0x600,             // 1536
    ADM_CMD_DELETE_RG = 0x601,             // 1537
    ADM_CMD_GET_RG_NUM = 0x602,            // 1538
    ADM_CMD_SHOW_RGLIST = 0x603,           // 1539
    ADM_CMD_SHOW_RG_INFO = 0x604,          // 1540
    ADM_CMD_EXPAND_RG_DISK = 0x605,        // 1541
    ADM_CMD_SHOW_RG_EXP_PROGRESS = 0x606,  // 1542
    ADM_CMD_SHOW_MEMBER_DISKS = 0x607,     // 1543
    ADM_CMD_SHOW_RGS_ID = 0x608,           // 1544
    ADM_CMD_MEMBER_DISK_NUM = 0x609,       // 1545
    ADM_CMD_SET_RG_VD_PDCACHE = 0x60A,     // 1546

    /* vd 对象类型命令 */
    ADM_CMD_CREATE_VD = 0x700,              // 1792
    ADM_CMD_DELETE_VD = 0x701,              // 1793
    ADM_CMD_GET_VDLIST_NUM = 0x702,         // 1794
    ADM_CMD_SHOW_VDLIST = 0x703,            // 1795
    ADM_CMD_SHOW_VD_INFO = 0x704,           // 1796
    ADM_CMD_EXPAND_VD_SIZE = 0x706,         // 1798
    ADM_CMD_SET_VD_CONSIS_CHECK = 0x707,    // 1799
    ADM_CMD_SHOW_VD_CONSIS_CHECK = 0x708,   // 1800
    ADM_CMD_SET_VD_SANITIZE = 0x709,        // 1801
    ADM_CMD_SHOW_VD_SANITIZE = 0x70A,       // 1802
    ADM_CMD_SET_VD_BOOT = 0x70B,            // 1803
    ADM_CMD_SET_VD_ATTR = 0x70C,            // 1804
    ADM_CMD_SHOW_VD_INCONSISTENT = 0x70D,   // 1805
    ADM_CMD_SHOW_VDS_ID = 0x70E,            // 1806
    ADM_CMD_DELETE_VD_PINNEDCACHE = 0x70F,  // 1807

    /* disk对象类型命令 */
    ADM_CMD_GET_DISK_DID = 0x800,          // 2048
    ADM_CMD_SET_DISK_PASSTHRU = 0x801,     // 2049
    ADM_CMD_SET_DISK_BOOT = 0x802,         // 2050
    ADM_CMD_SET_DISK_HSP = 0x803,          // 2051
    ADM_CMD_SET_DISK_ON_OFFLINE = 0x804,   // 2052
    ADM_CMD_SET_DISK_FREE = 0x805,         // 2053
    ADM_CMD_SHOW_DISK = 0x806,             // 2054
    ADM_CMD_SET_DISK_PATROLREAD = 0x807,   // 2055
    ADM_CMD_SHOW_DISK_PATROLREAD = 0x808,  // 2056

    ADM_CMD_SET_DISK_PHY = 0x80A,            // 2058
    ADM_CMD_SHOW_PHY_STATUS = 0x80B,         // 2059
    ADM_CMD_SHOW_DISK_PHY_EEROR = 0x80C,     // 2060
    ADM_CMD_SET_DISK_QUEUE_DEPTH = 0x80D,    // 2061
    ADM_CMD_SHOW_DISK_IO_SUM = 0x80E,        // 2062
    ADM_CMD_SET_DISK_SANITIZE = 0x80F,       // 2063
    ADM_CMD_SET_DISK_ERASE = 0x810,          // 2064
    ADM_CMD_SHOW_SANITIZE_PROGRESS = 0x811,  // 2065
    ADM_CMD_SHOW_DISK_ERASE = 0x812,         // 2066
    ADM_CMD_SET_DISK_CACHE = 0x813,          // 2067
    ADM_CMD_SHOW_DISK_CACHE = 0x814,         // 2068
    ADM_CMD_SET_DISK_LED = 0x815,            // 2069
    ADM_CMD_ACTIVE_DISK_FW = 0x816,          // 2070
    ADM_CMD_GET_DISK_SMART_INFO = 0x817,     // 2071
    ADM_CMD_GET_DISKTYPE = 0x818,            // 2072
    ADM_CMD_SET_DISK_LOOPBACK = 0x819,       // 2073
    ADM_CMD_CLEAR_SAS_PHYERROR = 0x81a,      // 2074
    ADM_CMD_SET_NCQ = 0x81b,                 // 2075
    ADM_CMD_SET_DISK_POWERSAVE = 0x81c,      // 2076
    ADM_CMD_SET_SLOWDISK = 0x81d,            // 2077
    ADM_CMD_SET_PHY_CFG = 0x81e,             // 2078
    ADM_CMD_SHOW_SSD_WEAR = 0x81f,           // 2079
    ADM_CMD_SET_HOTSPARE_SLOT = 0x820,       // 2080
    ADM_CMD_SHOW_HOTSPARE_SLOT = 0x821,      // 2081
    ADM_CMD_SHOW_SLOWDISK = 0x822,           // 2082
    ADM_CMD_SET_SLOWDISK_EX = 0x823,         // 2083
    ADM_CMD_SHOW_SLOWDISK_EX = 0x824,        // 2084

    /* 框类对象（共用disk的字空间） */
    ADM_CMD_SHOW_EXP_PHY_STATUS = 0x8f0,  // = 2288
};

#define ADM_RAID_NVME_DOWNLOAD_IMAG_OPCODE (0x11)

/*****************************************************************************
 Description  : 管理命令的管理通道的ioctl数据结构，64+12字节
*****************************************************************************/
struct adm_ioctl_sqe_r64 {
    u8 opcode;  // 厂商自定义操作码
    u8 flags;   // 包含PSDT和FUSE，用于选择PRP/SGL以及融合命令标志，该字段由驱动进行选择
    u16 rsvd0;  // 保留
    u32 nsid;   // namespace ID
    union {
        struct {
            u16 subopcode;  // 标识具体的一条命令
            u16 rsvd1;      // 标识具体的一条命令
        } info_0;
        u32 cdw2;
    };
    union {
        struct {
            u16 data_len;  // 数据传输时用于指定数据长度
            u16 param_len;
        } info_1;
        u32 cdw3;
    };
    u64 metadata;  // 元数据
    u64 addr;      // 数据地址
    u32 metadata_len;
    u32 data_len; // 用于兼容nvme公版驱动，公版驱动会使用此值做dma
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
    u32 timeout_ms;
    u32 result0;
    u32 result1;
};

// 用于兼容nvme result字段只有32位的驱动版本
struct adm_ioctl_sqe_r32 {
    u8 opcode;  // 厂商自定义操作码
    u8 flags;   // 包含PSDT和FUSE，用于选择PRP/SGL以及融合命令标志，该字段由驱动进行选择
    u16 rsvd0;  // 保留
    u32 nsid;   // namespace ID
    union {
        struct {
            u16 subopcode;  // 标识具体的一条命令
            u16 rsvd1;      // 标识具体的一条命令
        } info_0;
        u32 cdw2;
    };
    union {
        struct {
            u16 data_len;  // 数据传输时用于指定数据长度
            u16 param_len;
        } info_1;
        u32 cdw3;
    };
    u64 metadata;  // 元数据
    u64 addr;      // 数据地址
    u32 metadata_len;
    u32 data_len; // 用于兼容nvme公版驱动，公版驱动会使用此值做dma
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
    u32 timeout_ms;
    u32 result0;
};

/*****************************************************************************
 Description  : 管理命令的io通道的ioctl数据结构，128+12字节
*****************************************************************************/
struct adm_ioctl_scsi_pthru {
    u8  opcode;
    u8  flags;
    u16 rsvd0;
    u32 nsid;  // cdw1
    union {
        struct {
            u16 res_sense_len; // cdb透传使用，当前默认96字节
            u8  cdb_len;
            u8  rsvd0;
        } info_0;
        u32 cdw2;
    };
    union {
        struct {
            u16 subopcode;
            u16 rsvd1;
        } info_1;
        u32 cdw3;
    };
    union {
        struct {
            u16 rsvd;
            u16 param_len; // cdw10 ~ cdw23 ,cdb透传不使用
        } info_2;
        u32 cdw4;
    };
    u32 cdw5;
    u64 addr;
    u64 prp2;
    union {
        struct {
            u16 eid;
            u16 sid;
        } info_3;
        u32 cdw10;
    };
    union {
        struct {
            u16 did;
            u8  did_flag;
            u8  rsvd2;
        } info_4;
        u32 cdw11;
    };
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 data_len; // buff_len
    u32 cdw16;
    u32 cdw17;
    u32 cdw18;
    u32 cdw19;
    u32 cdw20;
    u32 cdw21;
    u32 cdw22;
    u32 cdw23;
    u64 sense_addr;
    u32 cdw26[4];
    u32 timeout_ms;
    u32 result0;        // BSG 不支持 返回result字段
    u32 result1;
};

#define ADM_IOCTL_CODE_SQER64 _IOWR('N', 0x41, struct adm_ioctl_sqe_r64)
#define ADM_IOCTL_CODE_SQER32 _IOWR('N', 0x41, struct adm_ioctl_sqe_r32)
#define ADM_IOCTL_CODE_SCSI_PTHRU _IOWR('N', 0x42, struct adm_ioctl_scsi_pthru)

#define ADM_BSG_MSGCODE_SCSI_PTHRU  1    // 标识与通过BSG交互时传递的命令类型为scsi直通命令，一般管理命令默认为0
// BSG ioctl 封装结构体
struct bsg_ioctl_cmd {
    u32 msgcode;    // 替代原IOCTL命令字作用，BSG只有一个公用命令字
    u32 control;    // 按bit定义（控制sg_io_v4 response 内容填Sense（bit0=1原定义，但本卡交互无此结构）还是result（bit0=0）
    union {
        struct adm_ioctl_sqe_r64   ioctl_r64;      // 交互结构体result字段64位
        struct adm_ioctl_scsi_pthru ioctl_pthru;    // 透传scsi交互结构体
    };
};

/* ***************************************************************
 *Description  : Hifan Tool Param Define Here
 *************************************************************** */
#define HIFAN_IOCTL_CMD _IOWR('N', 0x41, struct adm_ioctl_sqe_r64)
#define MAX_FRAGMENT_LEN (1024 * 4)

#ifdef PAGE_SIZE
#undef PAGE_SIZE
#endif
#define PAGE_SIZE (1024 * 4)

#define HIFAN_MAX_FILE_NAME_LENGTH (256)
#define HIFAN_MAX_FILE_DIR_LEN (512)

/* subcode:11bit,高两位区分命令类型 10:fanout 预留+ 保留 */
enum hifan_cli_cmd_id {
    /* normal cmd */
    CMD_SYS_HELP = ((0x2 << 9) | 0x0), /* num 0x2 9 0x0 used to compute fanout cmd id */
    CMD_CARD_INFO,
    CMD_CARD_VERSION,
    CMD_REG_OPER,
    CMD_PORT_INFO,
    /* dma cmd */
    CMD_LOAD_FILE_ATTR = ((0x2 << 9) | 0x20), /* num 0x2 9 0x20 used to compute fanout cmd id */
    CMD_LOAD_FILE,
    CMD_FW_UPDATE_ATTR,
    CMD_FW_UPDATE,
    CMD_FW_LOG,
    CMD_DMA_DEBUG = ((0x2 << 9) | 0xFF), /* num 0x2 9 0xFF used to compute fanout cmd id */
    CMD_MAX_CNT = 0xFFFFFFFF,
};
#define is_dma_cmd(sub_cmd) \
    ((sub_cmd == CMD_FW_UPDATE) || (sub_cmd == CMD_FW_LOG) || (sub_cmd == CMD_LOAD_FILE) || (sub_cmd == CMD_DMA_DEBUG))

/* Define cmd name here */
#define HIFAN_CMD_SPACE "    "
#define HIFAN_TOOL_NAME "hifanadm"
#define CLI_CMD_HELP "help"
#define CLI_CMD_INFO "info"
#define CLI_CMD_VERSION "version"
#define CLI_CMD_REG_OPER "reg_oper"
#define CLI_CMD_PORT_INFO "port_info"
#define CLI_CMD_DOWN_LOAD "download"
#define CLI_CMD_FW_UPDATE "fw_update"
#define CLI_CMD_FW_LOG "fw_log"
#define CLI_CMD_DMA_DEBUG "dma_debug"

/* Define cmd param struct here */
#define CARD_NUM_PER_CHAN 10
struct hifan_cmd_card_info {
    u32 rsvd[2];
};
struct hifan_cmd_card_info_rsp {
    u8 card_num;
    u32 card_idx[CARD_NUM_PER_CHAN];
};

#define CARD_VERSION_LEN 16
struct hifan_cmd_card_version {
    u32 rsvd[2];
};
struct hifan_cmd_card_version_rsp {
    u8 drv_ver[CARD_VERSION_LEN];
    u8 fw_ver[CARD_VERSION_LEN];
    u8 pcie_ver[CARD_VERSION_LEN];
};

struct hifan_cmd_fw_log {
    u32 resv[2];
};
struct hifan_cmd_fw_log_rsp {
    u32 data[MAX_FRAGMENT_LEN];
    u32 data_len;
    u32 log_size;
};

#define MBOX_REG_READ (0)
#define MBOX_REG_WRITE (1)
struct hifan_cmd_reg_oper {
    /* (1) reg read:  opcode-0, reg: 0xxx, reg_val: any not use
       (2) reg write: opcode-1, reg: 0xxx, reg_val: 0xxxx */
    u32 opcode;
    u32 reg;
    u32 reg_val;
};
struct hifan_cmd_reg_oper_rsp {
    /* (1) reg read:  reg: read, reg_value: register val
       (2) reg write: not used */
    u32 reg;
    u32 reg_val;
};

#define LIMIT_NUM 100
struct hifan_cmd_reg_dump {
    u32 start_reg;
    u32 end_reg;
};
struct hifan_cmd_reg_dump_rsp {
    u32 num;
    u32 reg[LIMIT_NUM];
    u32 val[LIMIT_NUM];
};

#define MAX_PCIE_PORT_ID 16
struct hifan_cmd_port_info {
    /* bit0~5 define in pcie module
       port0-15 is rp, core0 port0-15 in chip
       port16 is ep, core1 port0 in chip */
    u32 pcie_port_id : 4;    // port1-15
    u32 pcie_port_mode : 1;  // 1:pcie-ep, 0:pcie-rp,disk
    u32 resv : 27;
};
struct hifan_cmd_port_info_rsp {
    u32 pcie_mode;   /* PCIe mode, RC/EP */
    u32 port_en;     /* disable or enable */
    u32 link_status; /* linkstatus, 0-link-down 1-link-up */
    u32 port_width;  /* link speed, Gen 1.0/2.0/3.0/4.0 */
    u32 port_gen;    /* linkstatus, 0-link-down 1-link-up */
};

struct hifan_cmd_common {
    u32 resv[5]; /* 最大能传递20byte 参数 */
};

/*****************************************************************************
 Description  : download file cmd for dfx
 Function     : download文件的操作
 command      : toolname cx download_dfx -f filepath
 Direction    : cmd 每次传输文件最大为0x1000，
*****************************************************************************/
struct hifan_load_file_param {
    struct {
        u8 is_file_attr : 1;  // 传输数据是否是文件属性数据，1，文件属性数据，0，文件内容数据，
        u8 is_last : 1;       // 是否为最后一段数据，1，是，0 ，不是
        u8 rsvd : 6;          // 保留数据
    } info;
    u8 rsvd0;
    u16 send_cnt;      // 当前数据是第几片数据 ，start with 0;
    u32 current_size;  // 本次传输数据的大小byte
    u32 file_tag;      // 文件特定的随机数tag
    u32 rsvd1;
};

struct hifan_load_file_attr {
    u32 total_size;  // 标识文件大小byte
    u8 file_name[HIFAN_MAX_FILE_NAME_LENGTH];
    u32 file_tag;  // 每传出一个文件时生成的随机数tag
    u32 file_crc;
};

struct hifan_cmd_dma_debug {
    u32 opt;
    u64 addr;
};

struct hifan_tool_param {
    u32 fanout_id;
    u32 opcode;
    void *addr;
    u32 data_len;
    union {
        struct hifan_cmd_common common;
        struct hifan_cmd_card_info card_info;
        struct hifan_cmd_card_version card_version;
        struct hifan_cmd_fw_log fw_log;
        struct hifan_cmd_reg_oper reg_oper;
        struct hifan_cmd_reg_dump reg_dump;
        struct hifan_load_file_param file_param;
        struct hifan_cmd_port_info port_info;
        struct hifan_cmd_dma_debug dma_debug;
    };
};

/*****************************************************************************
 Description  : lib NVMe_PASSTHROUGH命令数据结构 从lib_cmd_param->p_param中获取
*****************************************************************************/
struct nvme_pt_cmd {
    u8 opcode;  // 厂商自定义操作码
    u8 flags;   // 包含PSDT和FUSE，用于选择PRP/SGL以及融合命令标志，该字段由驱动进行选择
    u16 rsvd0;
    u32 nsid;  // namespace ID
    u32 cdw2;
    u32 cdw3;
    u64 metadata;      // 元数据
    u64 addr;          // 数据地址 prp1
    u32 metadata_len;  // prp2_parat_1
    u32 data_len;      // prp2_part_2
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
};

/*****************************************************************************
 Description  : nvme cmd
 Function     : NVMe_PASSTHROUGH命令
 command      : toolname cx nvme --opcode u8 --nsid u32 -f save_data_file --cdw10 F0,F1,F2,F3
                --cdw11 F0,F1,F2,F3 --cdw12 F0,F1,F2,F3 --cdw13 F0,F1,F2,F3 --cdw14 F0,F1,F2,F3
                --cdw15 F0,F1,F2,F3
 Direction    : opcode: ADM_CMD_NVMe_PASSTHROUGH
*****************************************************************************/
struct nvme_cmd_param {
    u8 opcode;                                  //
    u8 type;                                    // cmd_type
    u16 data_len;                               // 返回数据长度
    u32 nsid;                                   // namespace id
    u8 file_path[ADM_MAX_FILE_NAME_LENGTH];  // nvme 命令返回的数据保存文件, 将分配的内存地址赋值给 nvme_pt_cmd->addr
    u32 cdw10;
    u32 cdw11;
    u32 cdw12;
    u32 cdw13;
    u32 cdw14;
    u32 cdw15;
};

#define CDB_DISK_SCSI_IMAGE_DOWNLOAD_OPCODE 0x3B
#define CDB_DISK_SCSI_IMAGE_SERVER_ACTION 0xE

struct scsi_update_pthru_cdb {
    u8 opcode;  // 厂商自定义操作码
    u8 model;
    u8 buffer_id;
    u8 buff_offset[3];
    u8 transfer_length[3];
    u8 control;
};

/* card commands */

/*****************************************************************************
 Description  : copy back status
 command      : add
 Direction      : none
*****************************************************************************/
struct cmd_rg_copyback_status {  // 设置和查询共用
    u8 is_all_raidgroup;         // 0 指定raid组   1卡上所有raid组
    u8 copyback_status;          // 0 or 1
};


enum pdlist_dif_size {
    PDLIST_DIF_SIZE_0 = 0,
    PDLIST_DIF_SIZE_8,
    PDLIST_DIF_SIZE_8_56,
    PDLIST_DIF_SIZE_56_8,
    PDLIST_DIF_SIZE_NA
};

enum pdlist_dif_type {
    PDLIST_DIF_TYPE_NA = 0,
    PDLIST_DIF_TYPE_1 = 1,
    PDLIST_DIF_TYPE_2 = 2,
    PDLIST_DIF_TYPE_3 = 3
};

enum pdlist_sp_type {
    PDLIST_SP_TYPE_NA = 0,
    PDLIST_SP_TYPE_1 = 1,
    PDLIST_SP_TYPE_2 = 2
};

/*****************************************************************************
 Description  :
 Function     : 显示expander列表信息
 Command      : toolname cx explist show
 Direction    : read
*****************************************************************************/
struct cmd_show_explist {
    struct {
        u16 exp_num;  // 查询到的expander数
        u16 rsvd[3];  // 字节对齐
    } head;
    struct {
        u16 enc_id;  // 框号
        u8 slot_id;  // 槽位号
        u8 level;    // 级数，expander级联，软件最大支持6级
        u32 rsvd;    // 字节对齐
    } expinfo[CMD_MAX_EXP_NUM];
};


/*****************************************************************************
 Description  : set lun attribute
 Function     : 设置lun三个参数
 command      : toolname cx:vdx set [-name name] [-w {WB/WT/WBE}] [-r {No_Ahead|Ahead}]
 Direction    : set
*****************************************************************************/
struct cmd_set_lun {
    char name[MAX_LUN_NAME_LEN];
    u16 read_type;   // （1）No_Ahead （2）Ahead
    u16 write_type;  // （1）WB，（2）WT ，（3）WBE
};

/*****************************************************************************
 Description  : set lun_disk_write_cache
 Function     : 设置lun成员盘写cache功能
 command      : raidadm cx vbx writecache on/off
 Direction    : set
*****************************************************************************/
struct cmd_disk_write_cache {
    u16 lun_id;
    u16 switcher;  // 0:off    1:on
};

enum adm_disk_active {
    ADM_ACTIVE_DISK_FW_STOP_IO,
    ADM_ACTIVE_DISK_FW_RUN_IO
};

struct adm_sas_smart_attr {
    u32 asc;
    u32 ascq;
    u32 fru;
    u32 rsvd;
};

struct adm_sata_smart_attr {
    u16 smart_id;
    u16 value;
    u16 worst;
    u16 thresh;
    u8 type;
    u8 updated;
};

struct adm_nvme_smart_attr {
    u16 smart_id;
    u16 rsvd;
    u64 value[2];
};

enum adm_smart_type {
    ADM_SMART_DISK_SAS,
    ADM_SMART_DISK_SATA,
    ADM_SMART_DISK_NVME
};

#define ADM_MAX_SATA_SMART_NUM 64
#define ADM_MAX_NVME_SMART_NUM 64
#define ADM_SMART_DISK_MODEL_MAX_STR_LEN   41
#define ADM_SMART_DISK_VER_MAX_STR_LEN 32

enum adm_sata_attr_title {
    ADM_VENDOR_SPECIFIC_SMART,  // Vendor Specific SMART Attributes with Thresholds
    ADM_MAN_MADE_SMART          // Man-made SMART Attributes with Thresholds
};

struct adm_sata_smart_attr_all {
    struct adm_sata_smart_attr sata_smart[ADM_MAX_SATA_SMART_NUM];
    u16 flag[ADM_MAX_SATA_SMART_NUM];
    union {
        u64 raw_val;
        u8 raw_value[8];
    } raw_union[ADM_MAX_SATA_SMART_NUM];
    u8 model[ADM_SMART_DISK_MODEL_MAX_STR_LEN];  // 磁盘型号，包含字符串结束符
    u8 rsv[7];
    u8 fw_version[ADM_SMART_DISK_VER_MAX_STR_LEN];  // 磁盘固件版本
};
/*****************************************************************************
 Description  : GET DISK SMART INFO
 Function     : 获取对应硬盘的smart信息
 command      :
 Direction    : subopcode: ADM_CMD_GET_DISK_SMART_INFO
*****************************************************************************/
#define SMART_INFO_MAGIC 0xAABB
struct adm_smart_info {
    u8 type;        // 标注返回的是哪种盘属性 adm_smart_type
    u8 title_type;  // Vend 1,
    u16 attr_num;   // 返回的条目数量
    u16 magic;      // SMART_INFO_MAGIC,兼容老版本显示
    u8 rsv[2];
    union {
        struct adm_sas_smart_attr sas_smart;
        struct adm_sata_smart_attr_all sata_smart_all;
        struct adm_nvme_smart_attr nvme_smart[ADM_MAX_NVME_SMART_NUM];
    };
};

#define ADM_RAID_SELECT_MASTER 1
#define ADM_RAID_SWITCH_MASTER 2

#define MT_MSG_DATA_LEN 28
/* multihost状态变更信息 */
struct mt_status_msg {
    int msgid;
    u8 data[28]; /* 保留使用 */
};

enum ssd_max_io_size {
    SSD_MAX_IO_128K = 0x80,
    SSD_MAX_IO_256K = 0x100,
    SSD_MAX_IO_1M = 0x400
};

/* dm表传给主机侧的信息 */
struct host_dev_table {
    u32 device_id;
    u16 host_target;
    u8 host_channel;
    u8 host_lun_num;
    u8 boot : 1;
    u8 pt : 1;
    u8 disktype : 3;
    u8 rawdisk : 1;
    u8 rsvd0 : 2;
    u8 valid : 1;  /* host driver use */
    u8 change : 1; /* if lun changed, such as capacity */
    u8 rsvd1 : 6;
    u16 max_io_size_kb;
};

/*****************************************************************************
 Description  : raid 卡全局配置查询
 Direction    : subopcode: ADM_CMD_SHOW_RAID_GLB_CFG
*****************************************************************************/
struct cmd_show_glb_cfg {
    u8 precopy_sw;
    u8 precopy_rate;
    u8 copyback_sw;
    u8 copyback_rate;

    u8 ehsp_sw;
    u8 ddma_sw;
    u8 rsvd;
    u8 powersave_sw;
    u32 powersave_wait_time;

    u8 ccheck_sw;
    u8 ccheck_repair_sw;
    u8 ccheck_rate;
    u8 ccheck_period;

    u8 patrolread_sw;
    u8 patrolread_rate;
    u8 disk_parall_num;
    u8 patrolread_period;

    u8 bginit_rate;
    u8 rebuild_rate;
    u8 sanitize_rate;
    u8 expandrg_rate;
    u8 rsvd1[8];
};

struct raid_set_card_dif {
    u8 opcode;
    u8 flags;
    u16 command_id;
    u32 device_id;  // 主机可见的设备ID（NSID），0xffffffff表示对已创建的所有设备起作用
    u32 rsvd0[9];
    u32 sub_opcode : 8;
    u32 rsvd1 : 24;
    u8 enable;  // 0 关闭，1 开启
    u8 rsvd2[3];
    u32 rsvd3[3];
};

struct set_wstrip_rinsert {
    u8 opcode;
    u8 flags;
    u16 command_id;
    u32 device_id;  // 主机可见的设备ID（NSID）
    u32 rsvd0[9];
    u32 sub_opcode : 8;
    u32 rsvd1 : 24;
    u8 enable;  // 0 恢复盘LBAF，1 设置盘LBAF+DIF
    u8 rsvd2[3];
    u32 rsvd3[3];
};

enum sal_abt_type {
    SAL_ABORT_IO,
    SAL_ABORT_ADMIN,
};

struct raid_eh_param {
    union {
        struct {
            u16 cmd_id;
            u16 sq_id;
            u8 is_admin; /* 0:数据IO,1:管理命令 */
            u8 rsv[3];
        } io_abt;
        struct {
            u8 type;
            u8 rsv[7];
        } dev_abt;
        struct {
            u8 type;
            u8 rsv[7];
        } pf_abt;
        struct {
            u8 ctrl_id;
            u8 rsv[7];
        } ctrl_rst;
    };
};

struct sal_reset_nvme_ctrl {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u8 reset_type;
    u8 rsv3;
    u16 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
    u32 rsv9;
};

struct sal_set_pcie_port_state {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u8 state;
    u8 rsv3;
    u16 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
    u32 rsv9;
};

struct sal_get_pcie_port_list {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv3;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
};

struct sal_get_pcie_port_info {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv3;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
};

struct sal_get_ctrl_list {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 rsv1;
    u16 rsv2;
    u32 rsv3;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
    u32 rsv9;
};

struct sal_nvme_pt {
    u8 opcode;
    u8 fuse : 2;
    u8 rsv0 : 4;
    u8 psdt : 2;
    u16 cmd_id;
    u32 ns_id;
    u8 sub_opcode;
    u8 ctrl_ns_id; /* 对应盘上的nsid */
    u8 slot_id;
    u8 es_id;
    u32 rsv2;
    u64 mptr;
    u64 prp1;
    u64 prp2;
    u32 rsv3;
    u32 rsv4;
    u32 rsv5;
    u32 rsv6;
    u32 rsv7;
    u32 rsv8;
};

struct fe_stor_sqe {
    u8 opcode;
    u8 flags;
    u16 command_id;
    u32 device_id;
    u64 in_prp;     // 输入参数 小于4K
    u64 out_prp;    // 输出参数 小于4K
    u64 data_prp1;  // 数据 小于1M
    u64 data_prp2;
    u32 in_len : 16;   // 输入参数长度
    u32 out_len : 16;  // 输出参数长度
    u32 sub_opcode : 8;
    u32 data_len : 24;  // 数据长度
    u32 cdw12[4];
};

struct sal_switch_master {
    u16 tmout;
};

#define SAL_DISK_EVENT_AEN 0x57

enum {
    NOTIFY_DISK_OPCODE = 0x1,
    NOTIFY_SATA_DISK_ERR = 0x2
};

struct sal_disk_event {
    u8 opcode;
    u8 rsvd[3];
    union {
        u32 status;
        struct {
            u32 cq_info[4];
            u32 logic_id;
            u32 rsv2[1];
        };
    };
};

struct sal_tmf_command {
    u8 tmf_type;
    u16 cmd_id;
    u16 sq_id;
    u8 is_admin;
};

enum raid_sal_opcode {
    RAID_SAL_NO_WR = 0xd0,
    RAID_SAL_WRITE = 0xd1,
    RAID_SAL_READ = 0xd2,
    RAID_SAL_EH = 0xd4
};

enum raid_sal_sub_opcode {
    /* 0xd0/0xd1/0xd2 */
    RAID_SAL_OPEN_PHY = 0X1,
    RAID_SAL_CLOSE_PHY = 0X2,
    RAID_SAL_LINK_RESET = 0X3,
    RAID_SAL_HARD_RESET = 0X4,
    RAID_SAL_NOTIFY = 0X5,
    RAID_SAL_ADD_DEVICE = 0X6,
    RAID_SAL_REMOVE_DEVICE = 0X7,
    RAID_SAL_COMMON_OPT = 0X8,
    RAID_SAL_SSP_MSG = 0X9,
    RAID_SAL_STP_MSG = 0XA,
    RAID_SAL_SMP_MSG = 0XB,
    RAID_SAL_MGMT_MSG = 0XC,
    RAID_SAL_QUERY_DEV_CNT = 0XD,  // 查询双控还未注销的盘数量
    RAID_SAL_LOCATE_STATUS = 0xE,
    RAID_SAL_CTL_LED_STATUS = 0xF,
    RAID_SAL_EQUIP_OPEN_PHY = 0x10,
    RAID_SAL_QUERY_DEVICE = 0x11,
    RAID_SAL_QUERY_PF_DEVICE = 0x12,
    RAID_SAL_REMOVE_PF_DEVICE = 0x13,
    RAID_SAL_CFG_STREAM = 0x14,
    RAID_SAL_SET_DEVICE_STAT = 0x15,
    RAID_SAL_QUERY_DEVICE_STAT = 0x16,

    RAID_BIST_START = 0x20,
    RAID_BIST_RX_EN = 0x21,
    RAID_BIST_TX_EN = 0x22,
    RAID_BIST_CLOSE = 0x23,
    RAID_IN_LOOPBACK_OPEN = 0x24,
    RAID_IN_LOOPBACK_CLOSE = 0x25,
    RAID_OUT_LOOPBACK_OPEN = 0x26,
    RAID_OUT_LOOPBACK_CLOSE = 0x27,
    RAID_GET_ERROR_CNT = 0x28,
    RAID_GET_PHY_LINKRATE = 0x29,
    RAID_OPERATE_FFE = 0x2A,
    RAID_GET_CHIP_STATE = 0x2B,
    RAID_GET_PHY_STATE = 0x2C,

    RAID_SAL_SWITCH_MASTER = 0x80,
    RAID_SAL_ENABLE_PF = 0x81,
    RAID_SAL_CARD_DIF = 0x82,
    RAID_SAL_WSTRIP_RINSERT = 0x83,
    // Task Management Function
    RAID_SAL_TMF_CMD = 0x84,
    RAID_SAL_GPIO_CFG = 0x85,

    /* 0xd4 */
    RAID_SAL_IO_ABORT = 0x1,
    RAID_SAL_DEVICE_ABORT = 0x2,
    RAID_SAL_PF_ABORT = 0x3,
    RAID_SAL_DEVICE_RESET = 0x4,
    RAID_SAL_CTRL_RESET = 0x5,
    RAID_SAL_PRE_SOFT_RESET = 0x6,
    RAID_SAL_SOFT_RESET = 0x7,
    RAID_SAL_AEN_ABORT = 0x8
};

/*****************************************************************************
 Description  : 固件冷升级
 command      : toolname cx update fw file=xx bitmap=x
 Direction    : ADM_RAID_WRITE subopcode: ADM_CMD_UPDATE_FW 0x37
*****************************************************************************/
struct cmd_update_firmware {
    u16 seq;          // 分片序号，每个组件都从0开始
    u16 tgt_bit_map;  // 待升级组件的bitmap
    u8 fw_type;       // 当前组件类型, update_tgt_type_e
    u8 algo_type;     // 算法类型
    u8 trans_flag;    // 当前分片类型(起始、中部、尾部、全部)
    u8 finish_flag;
    u32 tgt_len;        // 当前组件不包含padding的总长度
    u32 data_crc;       // 当前传输片的数据CRC32结果
    u32 cur_slice_len;  // 当前传输片的数据长度(大小限制  RAID:256KB; HBA:64KB，考虑到nvme驱动每次传输最大4K，固定为4K)
};

/*****************************************************************************
 Description  : 固件激活
 command      : toolname cx active fw bitmap=x force={0|1} type={0|1}
 Direction    : ADM_RAID_SET subopcode: ADM_CMD_ACTIVE_FW 0x38
*****************************************************************************/
struct cmd_active_firmware {
    u16 act_bitmap;  //  待升级组件的bitmap
    u8 is_force;     /* force为1代表强制激活,0:正常激活 */
    u8 type;         /* active_type_e type=0   上电激活 type=1   立即激活 */
};

/*****************************************************************************
 Description  : 向指定卡写入指定文件内容
 command      : toolname cx transport file name=xx offset=x
 Direction    : ADM_RAID_SET
*****************************************************************************/
struct cmd_transport_file {
    u8 trans_flag;  // 当前分片类型(前3位分别表示起始、中部、尾部、单片能传完起始和尾部置位)
    u8 rsvd;
    u16 cur_slice_len;  // 当前传输片的数据长度(大小限制  RAID:256KB; HBA:64KB，考虑到nvme驱动每次传输最大4K，固定为4K)
    u32 seq;            // 分片序号，从0开始每次递增1
    u32 slice_crc;      // 当前传输片的数据CRC32
    u32 file_len;       // 传输文件的大小
    u32 offset;         // 文件需要被写入的偏移地址
};

#define UART_REDIRECT_MAX_LOG_SIZE 128
/*****************************************************************************
 Description  : 新增主机工具，卡侧串口日志重定向
*****************************************************************************/
struct cmd_uart_redirect {
    u16 char_num;  // 实际接收和发送的字符数
    u16 rsvd;
    char uart_data[UART_REDIRECT_MAX_LOG_SIZE];  // 接收和发送的字符数据
};

/*****************************************************************************
 Description  : 获取卡侧内存数据（RTOSCK没有文件系统，无法获取文件）
 command      : toolname cx get fwdata start_addr=xx end_offset=xx save_path=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_GET_FW_DATA
*****************************************************************************/
enum fwdata_blk_type {
    FIRST_BLK,
    LAST_BLK,
    OTHER_BLK
};

struct cmd_fwdata_param {
    u64 start_addr;
    struct {
        u32 rsvd : 6;
        u32 blk_type : 2;       // 本次传输数据块类型:第一块/最后一块/其他
        u32 cur_blk_size : 24;  // 本次传输数据的大小byte
    } data_info;
    u32 tag;  // 文件特定的随机数tag
};

/*****************************************************************************
 Description  : 眼图查询
 command      : toolname cx show eye_diagram id=x
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_EYE_DIAGRAM
*****************************************************************************/
struct sds_com_phy_para {
    u8 phy_sds_id;  // 0 - 47
    u8 rsvd[3];
};

struct sds_four_eye_diagram {
    int eye_top; /* 顺序固定为:上下左右 */
    int eye_bot;
    int eye_left;
    int eye_right;
};

/*****************************************************************************
Description  : TX预加重测试参数(set和show)
command      : toolname cx set sds_ffe id=x param=x,x,x,x,x
               toolname cx show sds_ffe id=x
Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_FFE(将通过data传递一部分数据)
               ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SHOW_FFE
*****************************************************************************/
struct sds_tx_fir_data {
    char fir_pre2;
    char fir_pre1;
    unsigned char fir_main;
    char fir_post1;
    char fir_post2;
    unsigned char rsvd[3];
};

/*****************************************************************************
Description  : RX ctle(set和show)
command      : toolname cx set sds_ctle id=x g=x,x,x,x b=x,x,x z=x,x,x s=x,x,x c=x,x,x r=x,x,x l=x,x p=x,x
               toolname cx show sds_ctle id=x
Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_CTLE
               ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SHOW_CTLE
*****************************************************************************/
struct sds_ctle_data {
    u8 ctleactgn1;
    u8 ctleactgn2;
    u8 ctleactgn3;
    u8 ctleactgn4;
    u8 ctlebst1;
    u8 ctlebst2;
    u8 ctlebst3;
    u8 ctleza1;
    u8 ctleza2;
    u8 ctleza3;
    u8 ctlesqh1;
    u8 ctlesqh2;
    u8 ctlesqh3;
    u8 ctlecmband1;
    u8 ctlecmband2;
    u8 ctlecmband3;
    u8 ctlermband1;
    u8 ctlermband2;
    u8 ctlermband3;
    u8 rxctlerefsel;      // 不传递
    u8 ctlepassgn;        // 不传递
    u8 ctlehfzen1;        // 不传递
    u8 ctlehfzen2;        // 不传递
    u8 ctlehfzen3;        // 不传递
    u16 rxctleibiastune;  // 不传递
    u8 ctle_eq1_lunpk_enb;
    u8 ctle_gn1_lunpk_enb;
    u8 ctlepassattn;
    u8 ctlepasstune;
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : RX dfe(set和show)
 command      : toolname cx set sds_dfe id=x param=x,x,x,x,,,  20 : TAP1cen、TAP1EDG、TAP2~9、FTAP1~6
                toolname cx show sds_dfe id=x
 Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_DFE
                ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SHOW_DFE
*****************************************************************************/
struct sds_rx_dfe {
    /* 20 : TAP1cen、TAP1EDG、TAP2~9、FTAP1~6 */
    int tap_value[20];
};

/*****************************************************************************
 Description  : RX dfe(set和show)
 command      : toolname cx set sds_dfe id=x param=x,x,x,x,,,  20 : TAP1cen、TAP1EDG、TAP2~9、FTAP1~6
                toolname cx show sds_dfe id=x
 Direction    : ADM_RAID_WRITE SUBOPCODE:ADM_CMD_SDS_SET_SWING
                ADM_RAID_READ SUBOPCODE:ADM_CMD_SDS_SET_SWING
*****************************************************************************/
struct sds_tx_swing {
    u32 swing;
};

/*****************************************************************************
 Description  : PCIE上行ep链路查询
 command      : toolname cx show pcie_ep_link
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PCIE_LINK
                ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_LINK_ERROR
*****************************************************************************/
struct pcie_ep_port {
    u32 port_id;
};

struct pcie_ep_state_get_rsp {
    u8 state;   // state:link状态:1 建链，0不建链
    u8 speed;   // speed:1~4代表gen1到gen4
    u8 porten;  // porten:0,是不使能，其他状态NA，1使能，其他数据才有效；
    u8 width;   // width:显示数字,直接显示
};

/*****************************************************************************
 Description  : PCIE 状态机查询
                状态清除
 command      : toolname cx show pcie_ltssm
                toolname cx clear pcie_ltssm
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PCIE_LTSSM
                ADM_RAID_SET  SUBOPCODE:ADM_CMD_CLEAR_PCIE_LTSSM
*****************************************************************************/
#define LTSSM_TRACE_MAXNR 64
union pcie_ltssm_trace1 {
    struct {
        u32 ltssm_st : 6;                                     /* [5..0] */
        u32 duration_counter : 4;                             /* [9..6] */
        u32 mac_cur_link_speed : 2;                           /* [11..10] */
        u32 train_bit_map : 8;                                /* [19..12] */
        u32 rxl0s_st : 2;                                     /* [21..20] */
        u32 any_change_pipe_req : 1;                          /* [22] */
        u32 rcv_eios : 1;                                     /* [23] */
        u32 dl_retrain : 1;                                   /* [24] */
        u32 all_phy_rxeleidle_or_rx_skp_interval_timeout : 1; /* [25] */
        u32 directed_speed_change : 1;                        /* [26] */
        u32 any_det_eieos_ts : 1;                             /* [27] */
        u32 rxl0s_to_recovery : 1;                            /* [28] */
        u32 any_lane_rcv_speed_change : 1;                    /* [29] */
        u32 changed_speed_recovery : 1;                       /* [30] */
        u32 suceessful_speed_negotiation : 1;                 /* [31] */
    } bits;
    u32 val;
};

union pcie_ltssm_trace2 {
    struct {
        u32 train_bit_map : 16; /* [15..0] */
        u32 txdetrx : 16;       /* [31..16] */
    } bits;
    u32 val;
};

struct pcie_ltssm_rsp {
    u8 port_id;
    u8 trace_mode;
    u8 roll;
    u8 count;
    union pcie_ltssm_trace1 trace1[LTSSM_TRACE_MAXNR];
    union pcie_ltssm_trace2 trace2[LTSSM_TRACE_MAXNR];
};

/*****************************************************************************
 Description  : PCIE 设置tx_margin
 command      : toolname cx set pcie_tx_margin id=x margin=x
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_TX_MARGIN
*****************************************************************************/
struct pcie_tx_margin {
    u32 port_id;
    u32 tx_margin;
};

/*****************************************************************************
 Description  : 对Nand/Nor flash 下发命令，自验后返回结果
 command      : toolname cx set flashtest type={nand|nor}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_HDTEST_FLASH
*****************************************************************************/
struct cmd_flash_test {
    u8 type;  // 0:ADM_CMD_FLASH_TEST_NAND 1:ADM_CMD_FLASH_TEST_NOR
    u8 rsvd[3];
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bp_inject_die die=xx block=xx len=xx
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BP_INJECT_DIE
*****************************************************************************/
struct cmd_bp_inject_die_unavail {
    u32 die;
    u32 block;
    u32 len;
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bp_nand [type=xx] [addr=xx] [ch=xx] [ce=xx]
                [lun=xx] [plane=xx] [block=xx] [page=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BP_NAND
*****************************************************************************/
struct cmd_bp_nand_operation {
    u32 op_type;
    u64 addr;
    u32 ch;
    u32 ce;
    u32 lun;
    u32 plane;
    u32 block;
    u32 page;
};

enum CMD_OP_TYPE_E {
    CMD_NAND_ERASE,
    CMD_NAND_RAW_WRITE,
    CMD_NAND_DATA_WRITE,
    CMD_NAND_RAW_READ,
    CMD_NAND_DATA_READ,
    CMD_NAND_ALL,
    CMD_NAND_INJECT_UNC,
    CMD_NAND_UID,
    CMD_NAND_OP_BUTT
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bpfs [type = xx]
                [name=xx] [len=xx] [path=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:
*****************************************************************************/
enum CMD_SET_FS_TYPE_E {
    FS_INIT,
    FS_OPEN,
    FS_CLEAN,
    FS_WRITE,
    FS_BUTT
};

struct cmd_bp_fs_operation {
    u32 type;
    u32 name;
    u32 seek;
    u32 skip;
    u32 size;
    u32 trans_len;
};

struct cmd_fs_size {
    u32 expect_size;
    u32 real_size;
};

/*****************************************************************************
 Description  :
 command      : toolname cx get bpfs_info [type = xx]
                [name=xx] [len=xx] [path=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:
*****************************************************************************/
enum CMD_GET_BP_TYPE_E {
    FS_READ,
    BP_META,
    BP_FS,
    DATA_READ,
    BUTT
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bpfun [type=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:
*****************************************************************************/
typedef enum {
    BP_INIT,
    BP_RESTORE,
    BP_CLEAN,
    BP_BACKUP,
    FUN_BUTT
} CMD_FUN_TYPE_E;

struct cmd_bp_hba_function {
    u8 type;
    u8 rsv[3];
};

/*****************************************************************************
 Description  :
 command      : toolname cx get metadata addr=xx path=xx
 Direction    : ADM_RAID_GET SUBOPCODE:
*****************************************************************************/
typedef enum {
    CMD_GET_INFO,
    CMD_GET_DATA,
} CMD_METD_TYPE_E;

typedef struct {
    u16 load_finish;    // 回拷结束标志
    u16 data_valid;     // 数据回拷有效性
    u32 phy_ppn;        // 数据的物理ppn
    int ret;            // ppn回拷结果
    int res;            // ppn回拷结果
} cmd_ret_info;

typedef struct {
    u32 sn;
    u32 data_len;
    cmd_ret_info main;   // 主区
    cmd_ret_info back;   // 备区
} cmd_meta_result;

typedef struct {
    u32 start_flg;       // 起始标志
    u32 cmd_type;        // 命令类型
    u64 sn;              // sn号防命令重入
    u64 addr;            // 回拷地址
} cmd_metadata_info_req;

/*****************************************************************************
 Description  :
 command      : toolname cx set bp_dqs_deflect dir={left|right}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BP_DQS_DEFLECT
*****************************************************************************/
struct cmd_bp_dqs_deflect_dir {
    u8 dir;
    u8 rsv[3];
};

enum CMD_DQS_DEFLECT_DIR {
    CMD_DQS_DEFLECT_LEFT,
    CMD_DQS_DEFLECT_RIGHT
};

#define CHIP_NUM_MAX 5
#define CHIP_DIEID_WORD_NUM 5
#define CHIP_NAME_LEN_MAX 7
#define CHIP_VERSION_LEN_MAX 8
#define CHIP_LOCATION_LEN_MAX 8
/*****************************************************************************
 Description  : 获取芯片dieid数据
 command      : toolname cx show dieid
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_DIEID
*****************************************************************************/
struct chip_dieid {
    u8 chip_id;
    u8 chip_name[CHIP_NAME_LEN_MAX];
    u8 chip_version[CHIP_VERSION_LEN_MAX];
    u8 chip_location[CHIP_LOCATION_LEN_MAX];
    u32 dieid_data[CHIP_DIEID_WORD_NUM];
};
struct cmd_chip_dieid {
    u32 chip_num;
    struct chip_dieid chip_data[CHIP_NUM_MAX];
};

/*****************************************************************************
 Description  : 公有定位结构体
*****************************************************************************/
struct cmd_rg_id {
    u16 rg_id;
    u8 rsvd[2];
};

struct cmd_vd_id {
    u16 vd_id;
    u8 rsvd[2];
};

struct cmd_expander_id {
    u16 exp_id;
    u8 rsvd[2];
};

enum disk_locate_flag_value {
    DISK_LOC_ENC_SLOT,
    DISK_LOC_DID
};

struct disk_location {
    u16 enc_id;   // 框号
    u16 slot_id;  // 槽位号
};

struct multi_disk_location {
    u16 enc_id;   //     框号
    u16 slot_id;  //     槽位号
    u16 did;      //     逻辑号
    u8 flag;      //     0:DISK_LOC_ENC_SLOT 1:DISK_LOC_DID, 用于下发时确认本次定位方式
    u8 rsvd;
};

/*****************************************************************************
 Description  : 获取物理磁盘列表
 command      : toolname cx show pdlist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PDLIST
*****************************************************************************/
struct cmd_pdlist_idx {
    u16 start_idx;  // 本次获取磁盘信息起始index
    u16 count;      // 查询数量
    u32 rsvd;       // 对齐预留
};

struct cmd_pdlist_entry {
    u16 enc_id;     // 磁盘框号
    u16 slot_id;    // 磁盘槽位号
    u8 interface;   // 物理盘的接口类型，SAS EXP,SATA,PLANE,NVME
    u8 media_type;  // 磁盘介质类型: HDD,SSD
    u8 logi_type;
    u8 logi_status;
    u16 did;  // 磁盘ID
    u8 physi_status;
    u8 link_speed;     // 实际连接速度
    u32 lbs;           // 物理盘逻辑扇区大小
    u32 pbs;           // 物理盘物理扇区大小
    u64 raw_capacity;  // 物理磁盘原始容量
    u8 model[40 + 1];  // 磁盘型号，包含字符串结束符
    u8 sed_disk;       // 物理盘是否为加密盘:YES,NO
    u8 dif_info;       // Bit3-bit0:dif类型，   Bit7-bit4:  dif size
    u8 sp;             // 物理盘起转状态 spin down/spin up
    u8 fw[32];         // 固件版本信息
    u8 halflife;
    u8 support_flag;    // 磁盘支持的能力   bit0:fua支持, bit1:是否慢盘, bit2:是否为smr盘
    u8 rsvd0[2];
};
#define CMD_PDS_MAX_NUM 256U
#define CMD_PDLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_pdlist_entry))
/*****************************************************************************
 Description  : 物理磁盘列表查询结果
 command      : toolname cx show pdlist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_PDLIST_NUM
*****************************************************************************/
struct cmd_show_pdlist {
    u16 num;                                             // 磁盘数量
    u16 rsvd0;                                           // 对齐预留
    u32 rsvd1[15];                                       // 预留
    struct cmd_pdlist_entry disks[CMD_PDLIST_ONCE_NUM];  // 单个4K页面最多能获取的entry(含头)
};

/*****************************************************************************
 Description  : 获取物理磁盘数量
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_PDLIST_NUM
*****************************************************************************/
struct cmd_disk_num {
    u16 num;     // 在位磁盘数量
    u8 rsvd[2];  // 对齐预留
};

/*****************************************************************************
 Description  : 获取磁盘的详细信息
 command      : toolname cx:ex:sx show
 Direction    : opcode=0xC2  subopcode=0x47
*****************************************************************************/
struct cmd_show_disk {
    u16 enc_id;
    u16 slot_id;
    u16 did;
    u16 asso_enc_id;
    u16 asso_slot_id;
    u16 asso_did;
    u8 rg_id;
    u8 rg_level;           // 硬盘所在raid组级别
    u8 logi_type;          // 逻辑类型
    u8 logi_status;        // 物理盘状态
    u64 rg_capacity;       // Disk归属于raid组的容量大小
    u64 disk_WWN;          // 盘标识,16进制展示
    u8 hibernated_status;  // 硬盘休眠状态，
    u8 hsp_status;         // 热备状态，0:非热备盘；1:全局热备；2:局部热备
    u8 is_boot;            // 是否启动盘
    u8 interface;          // 硬盘接口类型，
    u16 physi_id;
    u8 media_type;
    u8 physi_status;
    u32 lbs;
    u32 pbs;
    u8 disk_max_speed;  // 盘的最大支持速率
    u8 device_speed;    // 盘的协商速率；
    u8 is_sed;          // 是否为加密盘
    u8 dif_info;
    u8 connected_port_id;  // 连接的端口号
    u8 linked_port_speed;  // 如果直连盘，是与盘的协商速率；若是expander，则是卡与expander的协商速率
    u8 disk_cache;
    u8 drive_temperature;  // 硬盘当前温度
    u8 halflife;
    u8 media_error_count;
    u8 other_error_count;
    u8 sed_enable;   // 是否设置密码
    u8 drive_size;   // 硬盘尺寸，
    u8 locate_led;   // 1 on； 0 off
    u8 ncq_support;  // 硬盘队列支持与否
    u8 ncq_enable;   // 硬盘队列启用或者禁用
    u64 raw_capacity;
    u64 sas_address;
    u64 power_on_time[2];
    u8 model[40 + 1];
    u8 vendor[32];
    u8 serial_num[32];
    u8 fw_version[32];
    u16 rotation_rate;  // 0:未上报, 1 non-rotating medium 非旋转介质, 其他值表示转速
    u8 wearout;         // SSD盘寿命预测
    u8 action;          // bit0:重构，bit1:巡检，bit2:回拷，bit3:擦除
    u16 left_time;      // 任务剩余时间
    u8 progress;        // 任务进度，0-100
    u8 support_flag;    // 磁盘支持的能力   bit0:fua支持, bit1:是否慢盘，bit2:是否为smr盘
};

/*****************************************************************************
 Description  : 设置单个磁盘的巡检开关和参数
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_DISK_PATROLREAD
*****************************************************************************/
struct cmd_set_disk_patrolread {
    struct multi_disk_location loc;
    u8 sw;  // 0: off  1:on （start）
    u8 rate;
    u8 rsvd[2];
    u32 delay;  // 单位小时
};

/*****************************************************************************
 Description  : 查询单个磁盘的巡检状态
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_DISK_PATROLREAD
*****************************************************************************/
struct cmd_show_disk_patrolread {
    u16 left_time;  // 剩余预估时间，单位，min
    u8 rate;
    u8 run_status;
    u8 progress;
    u8 delay;   // 延迟时间，单位h
    u16 magic;  // CMD_MAGIC_FLAG 兼容之前版本，定义标志
};

/*****************************************************************************
 Description  : 显示卡\expander特定端口的phy信息
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PHY_STATUS
*****************************************************************************/
struct cmd_show_phy_entry {
    u8 phy_id;
    u8 phy_enable;
    u8 link_speed;  // 端口下行速率
    u8 attach_dev_type;
    u32 rsvd0;
    u64 attach_sas_addr;
    u64 invalid_dword_cont;
    u64 run_disparity_err_cont;
    u64 loss_of_dword_syn_cont;
    u64 phy_reset_pro_cont;
    u32 rsvd[2];
};

/*****************************************************************************
 Description  : 显示盘侧的误码信息
 command      : toolname cx:ex:sx show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_DISK_PHY_EEROR
*****************************************************************************/
struct cmd_show_disk_sas_error {
    u64 invalid_dword_cont;
    u64 run_disparity_err_cont;
    u64 loss_of_dword_syn_cont;
    u64 phy_reset_pro_cont;
};

#define MAX_SATA_PHY_ERR_EVENT 16
struct cmd_show_disk_sata_error {
    u64 err_val[MAX_SATA_PHY_ERR_EVENT];
};

struct cmd_show_disk_error {
    u8 disk_type;
    u8 dev_present;
    u8 link_speed;  // 协商速率
    union {
        struct cmd_show_disk_sas_error sas_error;
        struct cmd_show_disk_sata_error sata_error;
    };
};

/*****************************************************************************
 Description  : 显示某个expander的phy状态
 command      : toolname cx:ex show phy
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_EXP_PHY_STATUS
*****************************************************************************/
struct cmd_show_exp_phy_status {
    u8 phy_num;  // 该expander的phy口数量
    u8 rsvd[7];
    struct cmd_show_phy_entry phys[MAX_PHY_NUM];
};

/*****************************************************************************
 Description  : 查询单磁盘安全擦除进度
 command      : toolname cx:ex:sx show sanitize
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SHOW_SANITIZE_PROGRESS
*****************************************************************************/
struct cmd_sanitize_progress {
    u8 progress;  // 0-100
    u16 left_time;
    u8 rsvd[5];
};

/*****************************************************************************
 Description  : 设置磁盘安全擦除
 command      : toolname cx:ex:sx start sanitize mode={cryptoerase|overwrite|blockerase}
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_SANITIZE
*****************************************************************************/
struct cmd_set_disk_sanitize {
    struct multi_disk_location loc;
    u8 mode;        // 擦除模式, overwrite - 1, blockerase - 2, cryptoerase - 3
    u8 rsvd[7];     // 对齐预留
};

/*****************************************************************************
 Description  : 设置磁盘安全擦除
 command      : toolname cx:ex:sx start erase mode={simple| normal| thorough} [pa=x] [pb=x]
 command      : toolname cx:ex:sx stop erase
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_ERASE
*****************************************************************************/
struct cmd_set_disk_erase {
    struct multi_disk_location loc;
    u8 sw;          // 打开或关闭 1 ADM_NEW_SWITCH_ON 开，2 关
    u8 mode;        // simple -1 | normal -2| thorough -3
    u8 pattern[2];  // 擦除数据
    u8 rsvd[12];    // 对齐预留
};

/*****************************************************************************
 Description  : 设置硬盘灯的状态
 command      : toolname cx:ex:sx set led
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_LED
*****************************************************************************/
struct cmd_set_disk_led {
    struct multi_disk_location loc;
    u8 led_type;  // 点灯类型 Locate:1 /err:0
    u8 sw;        // 0:关；1:开
    u8 rsvd[2];   // 对齐预留
};

enum cmd_disk_led_type {
    CMD_LED_TYPE_ERR,
    CMD_LED_TYPE_LOCATE
};

enum cmd_led_light_state {
    CMD_LED_STATUES_OFF,
    CMD_LED_STATUES_ON
};

/*****************************************************************************
 Description  : 获取磁盘did
 command      : toolname cx:ex:sx show did
 Direction    : opcode=READ  subopcode=ADM_CMD_GET_DISK_DID
*****************************************************************************/
struct cmd_disk_did {
    u16 did;
    u16 rsvd;
};
#define ADM_CREATE_RG_BIT_PDCACHE_UNSUP 1 /* LUN创建PDcache同步，有不支持的盘，通过result0的第二个bit返回 */
/*****************************************************************************
 Description  : 创建逻辑盘
 command      : toolname cx create vd
 Direction    : opcode=0xC0  subopcode=ADM_CMD_CREATE_VD
*****************************************************************************/
struct cmd_create_vd {
    u8 name[MAX_VD_NAME_LEN];  // 名称，包含结束符
    u8 rsvd1[8];
    u64 size;  // 逻辑盘大小，单位 MB，输入数据必须是整数，取值范围:0MB到RAID组剩余容量；全F表示全部剩余容量）
    u32 strip_unit_size;  // 分条大小，单位KB，默认值32KB，enum adm_susize
    u8 init;  // 初始化类型，enum adm_init_type,fast:快速初始化（默认值），front:前台初始化，background:后台初始化
    u8 cold_hot;  // VD冷热属性: 2:默认值，无冷热属性，1:热lun，0:冷LUN
    u8 wcache;  // VD写cache策略。取值为:（1）WB 写回，（2）WT 写穿（默认值），（3）WBE 强制写回；
    u8 rcache;    // VD预读策略。取值为:（0）No_Ahead 不预读（默认值），（1）Ahead 预读
    u8 map_type;  // SCSI: ADM_RAID_SCSI(0) NVMe : ADM_RAID_NVME(1)
    u8 rg_id;  // RAID组ID，最多支持64个RAID组。当创建完第一个RG之后，可以通过指定RAID组来添加LUN
    u8 rg_id_used;       // ture: rd_id used, false: rg_id unused
    u8 level;            // vd所属RAID组级别。取值为enum raid_group_level
    u8 member_disk_num;  // 成员物理磁盘数量,
    u8 sg_disk_num;      // RAID子组的成员盘数量
    u8 disk_cache_sw;    // VD所属硬盘写Cache开关:enum adm_disk_cahe_switch
    u8 pf;               // AUTO = 2; PF0 = 0; PF1 = 1;如果在指定RAID组上创建，则PF属性跟随raid组的属性（即1个RAID上，所有的VD都上报到同一个  Port）
    u8 locationtype;  // 0: 使用EID/SID定位 1 :使用DID定位盘
    u8 rsvd[7];
    union {
        struct disk_location disks[CMD_MEMB_DISK_MAX_NUM];  // 成员盘位置信息。
                                                                    // 如果location_type = 0, 则使用struct disk_location,
                                                                    // 否则使用u 16 did[80];
        u16 did[CMD_MEMB_DISK_MAX_NUM];
    };
};

/*****************************************************************************
 Description  : 删除逻辑盘
 command      : toolname cx:vdx delete
 Direction    : opcode=0xC0  subopcode=ADM_CMD_DELETE_VD
*****************************************************************************/
struct cmd_del_vd {
    struct cmd_vd_id loc;
    u8 discardcache;  // 0: default 1: force 2: discardcache
    u8 rsvd[3];       // 对齐预留
};

/*****************************************************************************
 Description  : 逻辑盘列表展示
 command      : toolname cx show vdlist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_VDLIST
*****************************************************************************/
struct cmd_vd_list_idx {
    u16 start_idx;  // 起始vd index
    u16 count;      // 查询的最大数量
    u32 rsvd;       // 对齐预留
};

struct cmd_vdlist_entry {
    u8 rg_id;
    u8 vd_status;
    u8 map_type;
    u8 vd_type;
    u16 vd_id;
    u16 su_size;
    u8 vd_name[32];
    u64 capacity;
    u32 sec_size;
    u8 cold_hot;
    u8 rcache;
    u8 wcache;
    u8 pf;
    u8 realwcache;
    u8 access_policy;  // 读写属性 enum adm_vd_access_policy
    u16 magic;         // 工具版本兼容魔术字段
    u8 rsvd[4];        // 对齐预留
};
#define CMD_VDS_MAX_NUM 512U     // 当前卡侧raid模式最大为512，hba模式为4
#define CMD_VDLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_vdlist_entry))
struct cmd_show_vdlist {
    u16 num;                                           // 返回总数
    u16 rsvd0;                                         // 对齐预留
    u32 rsvd1[15];                                     // 预留
    struct cmd_vdlist_entry vds[CMD_VDLIST_ONCE_NUM];  // 单个4K页面最多能获取的entry
};

/*****************************************************************************
 Description  : 获取逻辑盘数量
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_VDLIST_NUM
*****************************************************************************/
struct cmd_get_vd_num {
    u16 num;     // 逻辑盘vd数量
    u8 rsvd[2];  // 对齐预留
};

/*****************************************************************************
 Description  : 开关raid卡回拷
 command      : toolname cx set copyback
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_COPYBACK_SW
                opcode=0xC0  subopcode=ADM_CMD_SHOW_COPYBACK_SW
*****************************************************************************/
struct cmd_copyback_status {
    u8 copyback_status;  // 开关状态，0关，1开
    u8 rsvd[3];          // 对齐预留
};

/*****************************************************************************
 Description  : 查询、设置bootwithpinnedcache开关
 command      : toolname cx set bootwithpinnedcache sw={on|off}
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_BOOT_PINNEDCACHE
                opcode=0xC0  subopcode=ADM_CMD_SHOW_BOOT_PINNEDCACHE
*****************************************************************************/
struct cmd_set_pinnedcache {
    u8 sw;               // 开关状态，0关，1开
    u8 rsvd[3];          // 对齐预留
};

/*****************************************************************************
 Description  : 公用开关参数，适合单一obj定位方式
*****************************************************************************/
struct cmd_comn_switch_status {
    u32 rsvd0;     // 供单一obj使用
    u8 sw_status;  // 开关状态，0关，1开
    u8 rsvd[3];    // 对齐预留
};

/*****************************************************************************
 Description  : 预拷贝开关
 command      : toolname cx set precopy
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_PRECOPY_SW
                opcode=0xC0  subopcode=ADM_CMD_SHOW_PRECOPY_SW
*****************************************************************************/
struct cmd_precopy_status {
    u8 precopy_status;  // 开关状态，0关，1开
    u8 rsvd[3];         // 对齐预留
};

/*****************************************************************************
 Description  : 查询系统巡检状态
 command      : toolname cx show patrolread
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PATROLREAD
*****************************************************************************/
struct cmd_show_patrolread {
    u8 period;            // 周期，单位:天，取值范围[1:60]
    u8 sw;                // 开关状态
    u8 rate;              // 速率，low|middle|high
    u8 disk_num;          // 并行做的盘数量
    u8 run_status;        // 是否正在做巡检,run和stop
    u8 finish_disks_sum;  // 做完巡检的盘数量
    u8 run_disks_sum;     // 正在巡检的盘数量
    u8 delay;             // 延迟时间，单位h
    u16 magic;            // CMD_MAGIC_FLAG 兼容之前版本，定义标志
    u8 rsv[2];            // 对齐预留
};

/*****************************************************************************
 Description  : 紧急热备开关状态
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_EHSP_SW
                opcode=0xC2  subopcode=ADM_CMD_SHOW_EHSP_SW
*****************************************************************************/
struct cmd_emergency_hsp {
    u8 ehsp_status;  // 开关状态，0:关；1:开
    u8 rsvd[3];      // 对齐预留
};

/*****************************************************************************
 Description  : raid卡整体直通类型
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_PASSTHRU_TYPE
                opcode=0xC2  subopcode=ADM_CMD_SHOW_PASSTHRU_TYPE
*****************************************************************************/
struct cmd_passthru_type {
    u8 passthru_type;  // 0:关 ADM_CARD_PASSTHRU_OFF；1:JBOD ADM_CARD_PASSTHRU_JBOD 2:RAWDRIVE ADM_CARD_PASSTHRU_RAWDRIVE
    u8 rsvd[3];        //  对齐预留
};

/*****************************************************************************
 Description  : DDMA开关
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SET_DDMA
                opcode=0xC2  subopcode=ADM_CMD_SHOW_DDMA
*****************************************************************************/
struct cmd_ddma {
    u8 ddma_status;  //  0:关；1:开
    u8 rsvd[3];      //  对齐预留
};

/*****************************************************************************
 Description  : 显示raid卡内设置为启动盘的相关盘信息
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_BOOT
*****************************************************************************/
struct cmd_show_boot_entry {
    u16 id;   //  vid或DID
    u8 type;  //  vd/pd
    u8 rsvd0;
    struct disk_location disk;  //  物理磁盘类型使用
    u8 rsvd1[4];
};

#define BOOT_ENTRY_MAX 20
struct cmd_show_boot {
    u16 num;   // 启动盘数量
    u16 rsvd;  // 对齐预留
    struct cmd_show_boot_entry disks[BOOT_ENTRY_MAX];
};

/*****************************************************************************
 Description  : 查询当前系统的单个外来配置信息
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_FNCFG
                opcode=0xC2  subopcode=ADM_CMD_CLEAR_FNCFG
                opcode=0xC2  subopcode=ADM_CMD_IMPORT_FNCFG
*****************************************************************************/
struct cmd_fncfg_param {
    u64 rg_guid;    // 索引 all (全F) / guid 通过raid组唯一标识guid定位外来配置
    u64 rsvd[2];
};

/*****************************************************************************
 Description  : 查询当前系统的所有详细外来配置信息
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_FNCFG
*****************************************************************************/
struct cmd_show_fncfg_entry {
    u16 enc;         //  当前foreign配置生效的硬盘框号
    u16 slot;        //  当前foreign配置生效的硬盘槽位号
    u8 status;       //  当前配置磁盘的状态
    u8 type;         //  当前磁盘的类型，free,member,spare,PaThrough,RawDisk
    u8 rg_id;        //  当前foreign配置磁盘所在的raid组ID
    u8 rg_level;     //  当前foreign配置磁盘所在的raid组等级
    u8 raid_status;  //  当前foreign配置磁盘所在的raid组状态，normal,fault,degrade
    u8 rsvd[3];
    u64 rg_guid;     //  所属raid组的唯一标识
    u16 vd_count;    //  所属raid组创建的逻辑盘数量
    u16 pd_count;    //  所属raid组使用的物理盘数量
    u8 rsvd1[16];
};

/*****************************************************************************
 Description  : 查询当前系统的所有外来配置信息列表
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_FNCFGLIST
*****************************************************************************/
#define CMD_FNCFGLIST_MAX_NUM 240U
#define CMD_FNCFGLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64U) / sizeof(struct cmd_show_fncfg_entry))
struct cmd_fncfglist_idx {
    u16 start_idx;      // 本次获取信息起始index
    u16 count;          // 查询数量
    u32 rsvd;           // 对齐预留
};

struct cmd_show_fncfglist {
    u16 num;                                                // 返回数量
    u16 rsvd0;                                              // 对齐预留
    u32 rsvd1[15];                                          // 预留
    struct cmd_show_fncfg_entry cfgs[CMD_FNCFGLIST_ONCE_NUM];
};

/*****************************************************************************
 Description  : BMC使用，一次获取所有物理盘的框号槽位号和逻辑号，不获取详细信息
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_PDS_ID
*****************************************************************************/
struct pds_disk_location {
    u16 enc_id;                         // 框号
    u16 slot_id;                        // 槽位号
    u16 did;                            // 逻辑号
    u16 rsvd;
};

#define PDS_ID_NODE_NUM         256
struct cmd_pds_id {
    struct pds_disk_location ids[PDS_ID_NODE_NUM];
    u16 num;                            //  当前接入磁盘数量
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : 获取后台任务信息
 command      : toolname cx show bgtask
 Direction    : opcode=ADM_RAID_READ  subopcode=ADM_CMD_SHOW_BGTASK
*****************************************************************************/
#define CMD_MAX_REALTIME_BGTASK_NUM 32
struct bgtask_info {
    u8 type;
    u8 progress;  // 任务运行进度,其余显示百分比
    u8 rate;
    u8 rsvd0;  // 对齐预留
    u16 vd_id;
    u16 time_left;
    u8 rsvd1[4];  // 对齐预留
};

struct cmd_show_bgtask {
    u8 sw;
    u8 task_num;
    u8 rsvd[6];  // 对齐预留
    struct bgtask_info bgtask[CMD_MAX_REALTIME_BGTASK_NUM];
};

/*****************************************************************************
 Description  : raid组列表展示
 command      : toolname cx show rglist
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_RGLIST
*****************************************************************************/
struct cmd_rglist_idx {
    u16 start_idx;  // 起始rg index
    u16 count;      // 本次获取磁盘数量
    u32 rsvd;       // 对齐预留
};

struct cmd_rglist_entry {
    u8 name[32];         // 名称
    u8 rg_id;            // raid组ID
    u8 level;            // raid组等级
    u8 status;           // raid组状态
    u8 member_disk_num;  // 成员盘总数
    u64 total_capacity;  // raid组总容量
    u64 free_capacity;   // raid组剩余容量
    u8 rsvd[8];  // 对齐预留
};

#define CMD_RGS_MAX_NUM 64U     // 当前卡侧raid模式最大为64，hba模式为4
#define CMD_RGLIST_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_rglist_entry))
struct cmd_show_rglist {
    u16 num;
    u16 rsvd0;                                         // 对齐预留
    u32 rsvd1[15];                                     // 对齐预留
    struct cmd_rglist_entry rgs[CMD_RGLIST_ONCE_NUM];  // 单个4K页面最多能获取的entry
};

/*****************************************************************************
 Description  : 显示当前raid组的详细信息
 command      : toolname cx:rgx show
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_RG_INFO
*****************************************************************************/
struct rg_block_info {
    u64 capacity;   // 容量
    u64 start_lba;  // 在单个物理盘上扇区起始位置
    u64 end_lba;    // 在单个物理盘上扇区结束位置
    u16 vd_id;      // vd的ID信息(当使用的时候有效)
    u8 used_flag;   // 是否空闲 0:空闲，1:used
    u8 rsvd;
};

struct member_disk_location {
    u16 enc_id;   // 框号
    u16 slot_id;  // 槽位号
    u16 did;      // 磁盘id
    u8 sg_index;  // 子组索引 （对于热备盘 0xff表示无效）
    u8 rsvd0;
};

#define BLOCK_INFO_MAX 36
struct cmd_rg_info {
    u8 rg_id;            // Raid组ID
    u8 level;            // Raid级别
    u8 status;           // Raid组状态
    u8 sg_num;           // 子组数量
    u8 sg_disk_num;      // 子组磁盘数
    u8 member_disk_num;  // 成员盘数量
    u8 copyback_sw;      // 回拷开关
    u8 disk_cache_sw;    // on off deault
    u16 vd_list[32];                                                         // vd列表
    u32 sec_size;                                                            // 扇区大小（B）
    u16 aligned_size;                                                        // 对齐大小（MB）
    u16 vd_num;                                                              // VD数量
    u8 name[32];                                                             // Raid组名称
    u64 total_capacity;                                                      // 总容量
    u64 free_capacity;                                                       // 空闲容量
    u64 sg_free_capacity;                                                    // 子组剩余容量，单位B
    u16 rsvd2[3];
    u8 hsp_num;                                                              // 热备盘数量
    u8 background_task;                                                      // 当前后台任务
    struct member_disk_location disks[CMD_MEMB_DISK_MAX_NUM];        // 成员盘信息
    struct member_disk_location hsp_disks[MAX_RG_HOTSPARE_DISK_NUM];  // 成员盘信息
    u8 free_block_num;                                                       // raid组上空闲区间容量
    u8 block_num;                   // raid组上的block数量（由于创建和删除VD，在RAID组上留下的非连续空间）
    u8 sec_type;                          // 扇区类型，enum adm_vd_sec_type_value
    u8 rsvd0;
    struct rg_block_info block_info[BLOCK_INFO_MAX];  // RG 上的block信息
    u32 rsvd1[5];                         // 预留
};

/*****************************************************************************
 Description  : 显示逻辑盘VD的详细信息
 command      : toolname cx:vdx show
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_VD_INFO
*****************************************************************************/
struct vd_block_info {
    u16 id;                // lun id信息
    u16 next_id;           //  下一个lun id信息（这个id和VD是否是统一编号）
    u16 rsvd[2];           // 对齐保留
    u64 capacity;          // 每个lun的容量
    u64 start_lba_ondisk;  // 在单个物理盘上的起始LBA地址
    u64 sec_num_ondisk;    // 在单个物理盘上的扇区数量
};

#define BLOCK_NUM_MAX 16
struct cmd_vd_info {
    u8 name[32];  // vd 名称
    u16 id;
    u8 rg_id;
    u8 rg_level;
    u8 sg_num;       // 子组数量
    u8 sg_disk_num;  // 每个子组磁盘数量
    u8 vd_status;
    u8 vd_type;   // NORMAL_LUN = 0,PRIVATE_LUN = 1, EXPAND_LUN = 2
    u8 cold_hot;  // VD冷热属性:（1）2:默认值，无冷热属性，（2）1:热lun，（3）0:冷LUN
    u8 map_type;  // ST NT NA
    u8 config_port;                       // 配置上报PF类型
    u8 work_port;                         // 当前上报的PF类型
    u16 stripe_size;                      // 条带大小，KB
    u16 su_size;                          // 分条大小，KB
    u64 capacity;                         // 单位MB。
    u32 sec_size;                         // 扇区大小
    u8 sec_type;                          // 扇区类型，enum adm_vd_sec_type_value
    u8 rcache;                            // No_Ahead Ahead NA
    u8 wcache;                            // 当前配置状态: WB WT WBE NA
    u8 real_wcache;                       // 实际工作状态: WB WT WBE NA
    u8 init_rate;                         // high medium low
    u8 pr_rate;                           // patrolread high medium low
    u8 init_progress;                     // 0-100
    u8 sanitize_progress;                 // 0-100
    struct vd_block_info block_info[BLOCK_NUM_MAX];  // 1个VD最多可以扩展7次，预留到16个
    u8 block_num;                         // vd的数据块数量，normal 只有1个。expander LUN可能有多个.
    u8 dirty_cache;                       // no,yes
    u8 is_boot;                           // 是否设置为启动逻辑盘
    u8 access_policy;                     // 读写属性 enum adm_vd_access_policy
    u8 rsvd[2];                           // 对齐预留
    u16 magic;                            // 工具版本兼容魔术字段
    u64 wwn_low;                          // LUN的WWN低8个字节，由厂商自定义
    u64 wwn_high;  // LUN的WWN高8个字节，最高4bit表示NAA类型，剩余60bit表示IEEE中厂商ID
};

/*****************************************************************************
 Description  : 显示RG或者VD的成员盘总数
 Direction    : opcode=ADM_RAID_READ_FROM_CQE  subopcode=ADM_CMD_MEMBER_DISK_NUM
*****************************************************************************/
enum cmd_member_id_type {
    CMD_RG_MEMBER_TYPE,
    CMD_VD_MEMBER_TYPE
};

struct cmd_member_id {
    u16 id;   // RG 和 VD通用，通过type区分
    u8 type;  // 0:CMD_RG_MEMBER_TYPE 1:CMD_VD_MEMBER_TYPE
    u8 rsvd;
};
/*****************************************************************************
 Description  : 显示RG或VD的成员盘信息
 command      : toolname cx:vdx / cx:rgx show pdarray
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_MEMBER_DISKS
*****************************************************************************/
struct cmd_member_disk_idx {
    struct cmd_member_id loc;
    u16 start_idx;  // 本次获取磁盘信息起始index,范围:0-max_pdlist_n-1
    u16 count;      // 查询数量
    u32 rsvd;       // 对齐预留
};

struct cmd_member_disk_entry {
    u16 enc_id;
    u16 slot_id;
    u16 did;
    u16 asso_enc_id;   //  关联热备盘框位号
    u16 asso_slot_id;  //  关联热备盘槽位号
    u16 asso_did;
    u64 capacity;  //  磁盘总容量，B
    u32 sec_size;  //  扇区大小，B
    u8 model[40 + 1];
    u8 intf_type;
    u8 media_type;
    u8 logi_status;
    u8 asso_status;
    u8 rsvd[3];
};
#define CMD_BST_MAX_NUM 8192U
#define CMD_MEMB_DISKS_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64) / sizeof(struct cmd_member_disk_entry))
struct cmd_show_member_disks {
    u16 num;
    u16 rsvd0;                                                    // 对齐预留
    u32 rsvd1[15];                                                // 64字节
    struct cmd_member_disk_entry disks[CMD_MEMB_DISKS_ONCE_NUM];  // 单个4K页面最多能获取的entry
};

#define RGS_ID_NODE_NUM         128
/*****************************************************************************
 Description  : BMC使用，一次获取所有raid组的id，不获取详细信息
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_SHOW_RGS_ID
*****************************************************************************/
struct cmd_rgs_id {
    u16 ids[RGS_ID_NODE_NUM];
    u16 num;  //  当前含有的raid组数量
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : 设置逻辑盘VD为启动盘
 command      : toolname cx:vdx set bootdrive
 Direction    : opcode=0xC0  subopcode=0x2A
*****************************************************************************/
struct cmd_vd_boot {
    struct cmd_vd_id loc;
    u8 boot_status;  // 0:关；1:开
    u8 rsvd[3];      // 对齐预留
};

/*****************************************************************************
 Description  : 设置逻辑盘属性
 command      : toolname cx:vdx set attr
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_VD_ATTR
*****************************************************************************/
struct cmd_set_vd_attr {
    struct cmd_vd_id loc;
    u8 type;     // 逻辑盘属性类型，0:wcache/1:rcache/2:name/3:map_type/4:pf/5:access
    u8 rsvd[3];  // 对齐保留
    union {
        u32 value;    // 属性值，非名称属性时有效
        u8 name[MAX_VD_NAME_LEN];  // 逻辑盘名称，属性类型为名称时有效
    };
    u8 rsvd1[8];
};

/*****************************************************************************
 Description  : 设置局部热备盘或者全局热备盘
 command      : toolname cx:ex:sx set hotspare
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_HSP
*****************************************************************************/
struct cmd_disk_hotspare {
    struct multi_disk_location loc;
    u8 hsp_type;  // 0 局部热备盘，1 全局 热备盘 2 槽位热备
    u8 rg_id;     // 局部热备时使用
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : 设置槽位热备
 command      : toolname cx:ex:sx set hotspare
 Direction    : opcode=0xC0  subopcode=ADM_CMD_SET_DISK_HSP
*****************************************************************************/
struct cmd_hspslot_param {
    struct multi_disk_location loc;
    u8 sw;        // 关 0:ADM_GENERAL_SWITCH_OFF  开 1:ADM_GENERAL_SWITCH_ON
    u8 rsvd[7];
};

/*****************************************************************************
 Description  : 查询槽位热备列表
 command      : toolname cx show hspslot
 Direction    : opcode=ADM_RAID_READ  subopcode=ADM_CMD_SHOW_BGTASK
*****************************************************************************/
#define CMD_MAX_HSPSLOTS_NUM 511
struct cmd_show_hspslot {
    u8 num;
    u8 rsvd[7];
    struct {
        u16 enc;
        u16 slot;
        u8 rsvd1[4];
    } slots[CMD_MAX_HSPSLOTS_NUM]; // 511，总4K
};

struct sense_info {
    u8 sense_len;     // sense 长度，<=96个字节
    u8 sense_offset;  // 数据偏移
    u16 ret;          // 命令执行结果
};

/*****************************************************************************
 Description  : 设置单磁盘为直通盘
 command      : toolname cx:ex:sx set passthru
 Direction    : opcode=ADM_RAID_SET  subopcode=ADM_CMD_SET_DISK_PASSTHRU
*****************************************************************************/
struct cmd_disk_passthru {
    struct multi_disk_location loc;
    u8 map_type;  // 针对nvme盘的映射类型:SCSI或NVMe。预留接口，支持双PF和nvme盘后使用
    u8 pf;        // 硬盘归属的PF,预留接口，支持双PF后使用
    u8 rsvd[2];   // 对齐预留
};

/*****************************************************************************
 Description  : 设置磁盘为启动盘
 command      : toolname cx:ex:sx set bootdrive
 Direction    : opcode=0xC0  subopcode=0x43 ADM_CMD_SET_DISK_BOOT
*****************************************************************************/
struct cmd_disk_boot {
    struct multi_disk_location loc;
    u8 boot_status;  // 0:关；1:开
    u8 rsvd[3];      // 对齐预留
};

/*****************************************************************************
 Description  : 设置物理盘上下线
 command      : toolname cx:ex:sx set online/offline
 Direction    : opcode=0xC0  subopcode=KSTORE_CMD_SET_DISK_ON_OFFLINE
*****************************************************************************/
struct cmd_disk_on_offline {
    struct multi_disk_location loc;
    u8 online_status;  // ADM_DISK_ONLINE: online，ADM_DISK_OFFLINE: offline
    u8 rsvd[3];        // 对齐预留
};

/*****************************************************************************
 Description  : 设置/查询全局裸盘开关状态
 Command      : toolname cx set/show rawdisk
 Direction    : opcode=0xC0  subopcode=0x17/0x18
*****************************************************************************/
struct cmd_glb_rawdisk {
    u8 rawdisk_state;  // 0:关；1:开
    u8 rsvd[3];        // 对齐预留
};

/*****************************************************************************
 Description  : 设置/查询全局DDMA开关状态
 command      : toolname cx set/show ddma
 Direction    : opcode=0xC0  subopcode=0x19/0x1A
*****************************************************************************/
struct cmd_glb_ddma {
    u8 ddma_state;  // 0:关；1:开
    u8 rsvd[3];     // 对齐预留
};

/*****************************************************************************
 Description  : SCSI协议命令透传
 command      :
 Direction    : opcode=0xC0/0xC1/0xC2  subopcode=ADM_CMD_SCSI_PASSTHROUGH
*****************************************************************************/
struct cmd_scsi_passthrough {
    struct multi_disk_location loc;
    u8 lun;               // 目标盘上的lun id，当前产品只支持单盘单lun,目前不使用
    u8 cdb_len;           // cdb命令的长度
    u8 sense_buffer_len;  // sense buffer的大小，建议96个字节
    u8 rsvd0;             // 保留字段
    u8 cdb[32];           // cdb命令字
    u32 rsvd1;            // 保留字节对齐
    u8 *sense_buffer;     // sense_buffer 数据保存地址
};

/*****************************************************************************
 Description  : 获取当前控制器的配置信息和能力信息，hba与raid不同.
 command      :
 Direction    : opcode=0xC2  subopcode=0x17 ADM_CMD_GET_CTRL_INFO
*****************************************************************************/
struct cmd_controller_info {
    u16 msg_len;                    // 当前消息有效长度
    u8  ctrl_mod;                   /*  表示当前板卡的工作模式
                                        0: HBA
                                        1: RAID
                                        2: HBA for Storage 存储模式HBA卡
                                        3: JOBD（链接JBOD框的扩张卡）
                                        4: other
                                    */
    u8  rgtask;                     /*  raid组相关后台支持:
                                        bit0:重构
                                        bit1 预拷贝
                                        bit2 回拷
                                        bit3 巡检
                                        bit4 扩盘 （当前版本不支持）
                                    */
    u8  mod_support;                /*  bit0: raid是否支持
                                        bit1:直通模式是否支持
                                        bit2:裸盘模式是否支持
                                        其它保留
                                    */
    u8 raid_level;                  /*  支持的raid级别，1表示支持，0表示不支持
                                        bit0 raid0
                                        bit1 raid1
                                        bit2 raid10
                                        bit3 raid5
                                        bit4 raid50
                                        bit5 raid6
                                        bit6 raid60
                                    */
    u8  expander_support;           // 支持的expander数量，0 表示当前版本不支持对接expander
    u8  rsv;                        // 保留
    u16 max_pdnum;                  // 支持最大物理盘数量
    u16 max_hddnum;                 // 支持最大HDD盘数量 0不支持
    u16 max_ssdnum;                 // 支持最大SSD盘数量 0不支持
    u16 max_nvmenum;                // 支持最大NVMe盘数量 0不支持
    u8  min_hddnum_level0;          // raid0最小物理盘数(hdd)
    u8  max_hddnum_level0;          // raid0最大物理盘数(hdd)
    u8  min_hddnum_level1;          // raid1最小物理盘数(hdd)
    u8  max_hddnum_level1;          // raid1最大物理盘数(hdd)
    u8  min_hddnum_level5;          // raid5最小物理盘数(hdd)
    u8  max_hddnum_level5;          // raid5最大物理盘数(hdd)
    u8  min_hddnum_level6;          // raid6最小物理盘数(hdd)
    u8  max_hddnum_level6;          // raid6最大物理盘数(hdd)
    u8  min_hddnum_level10;         // raid10最小物理盘数(hdd)
    u8  max_hddnum_level10;         // raid10最大物理盘数(hdd)
    u8  max_hdd_subrgnum_level10;   // raid10子组数量(hdd)
    u8  min_hddnum_level50;         // raid50最小物理盘数(hdd)
    u8  max_hddnum_level50;         // raid50最大物理盘数(hdd)
    u8  max_hddsubrgnum_level50;    // raid50子组数量(hdd)
    u8  min_hddnum_level60;         // raid10最小物理盘数(hdd)
    u8  max_hddnum_level60;         // raid10最大物理盘数(hdd)
    u8  max_hddsubrgnum_level60;    // raid10子组数量(hdd)
    u8  pd_op_support;              // bit0 物理盘安全擦除 sanitize
                                    // bit1 物理盘安全擦除 erase
                                    // bit2 例测 smart_polling
    u16 vd_op_support;              // 逻辑盘操作能力
                                    /*  bit0 坏道修复
                                        bit1 扩lun，hba不支持
                                        bit2 创建
                                        bit3 删除
                                        bit4 前台初始化
                                        bit5 后台初始化
                                        bit6 快速初始化
                                        bit7 安全擦除
                                        bit8 分条一致性校验
                                    */
    u8  min_ssdnum_level0;          // raid0最小物理盘数(ssd)
    u8  max_ssdnum_level0;          // raid0最大物理盘数(ssd)
    u8  min_ssdnum_level1;          // raid1最小物理盘数(hdd)
    u8  max_ssdnum_level1;          // raid1最大物理盘数(hdd)
    u8  min_ssdnum_level5;          // raid5最小物理盘数(hdd)
    u8  max_ssdnum_level5;          // raid5最大物理盘数(hdd)
    u8  min_ssdnum_level6;          // raid6最小物理盘数(hdd)
    u8  max_ssdnum_level6;          // raid6最大物理盘数(hdd)
    u8  min_ssdnum_level10;         // raid10最小物理盘数(hdd)
    u8  max_ssdnum_level10;         // raid10最大物理盘数(hdd)
    u8  max_ssd_subrgnum_level10;   // raid10子组数量(hdd)
    u8  min_ssdnum_level50;         // raid50最小物理盘数(hdd)
    u8  max_ssdnum_level50;         // raid50最大物理盘数(hdd)
    u8  max_ssdsubrgnum_level50;    // raid50子组数量(hdd)
    u8  min_ssdnum_level60;         // raid10最小物理盘数(hdd)
    u8  max_ssdnum_level60;         // raid10最大物理盘数(hdd)
    u8  max_ssdsubrgnum_level60;    // raid10子组数量(hdd)
    u8  max_expandlun;              // 每个vd支持的最大扩展lun数量
    u8  max_vdnum;                  // 支持的最大vd数量(64个)
    u8  spares_support;             /*  是否支持热备
                                        bit0:全局热备支持
                                        bit1:局部热备支持
                                        bit2:紧急热备支持
                                    */
    u8  hsp_per_raid;               // 每个raid组支持的最大局部热备盘数量(2个)
    u8  max_globhps;                // 全局热备盘数量
    u8  foreign_import;             // 外来配置导入:1表示支持，0表示不支持；bit0 : 支持外来配置导入，其它位保留
    u8  phy_number;                 // 控制器使用的最大phy数量（40个，可能屏蔽不用）
    u8  drive_support;              // 支持的驱动器类型；bit0:SCSI 设备，bit1:NVMe设备
    u8  rsvpf[3];                   // 保留
    u8  pf_number;                  // pf支持的数量（当前芯片最多支持2个）；    1:支持1个PF，    2: 支持2个PF
    u8  pf_type;                    /*  pf工作模式
                                        00:未使能
                                        01:SCSI模式
                                        02:NVME模式
                                        bit0-bit3: PF0工作模式
                                        bit4-bit7: PF1工作模式
                                    */
    u8  ep_pcie_width;              // 上行PCIE端口属性（最大）:    1 x4，    2 x8，3 x16
    u8  ep_pcie_speed;              // PCIe4.0 16G
    u32 rsvd0;                      // 保留
    u16 vdnum_per_rg;               // 每个raid组最大创建的vd数量
    u16 rg_num;                     // raid组最大个数
    u8  bp_support;                 // 是否支持备电模块:    bit0:备电模块是否支持
    u8  rsv_bp[3];                  // 保留
    u8  rsv0[4];                    // 保留
    u8  sas_ctl_speed;              // sas控制器支持的速率 12G
    u8  pcie_rp_speed;              // 支持下行pcie设备速率，pcie4.0 16G
    u8  rsvd1[2];                   // 保留
    u32 max_hddstripe_size;         // 最大条带大小(K)   (最大80M）
    u32 max_ssdstripe_size;         // 最大条带大小(K)   (最大16M）
    u16 min_hddstripe_size;         // 最小条带大小(K)   32 表示32K
    u16 min_ssdstripe_size;         // 最小条带大小(K)   32 表示32K
    u16 max_su_size;                // 最大条带单元大小(K)  1024 (1M)
    u16 min_su_size;                // 最小条带单元大小(K)  32
    u8  pad[];                      // 填充到128字节
};

#define CONTROLLER_STATUS_BOARD_NAME_LEN 24
/*****************************************************************************
 Description  : 获取当前控制器的状态信息
 command      :
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_CTRL_STATUS
*****************************************************************************/
struct cmd_controller_status {
    u16 msg_len;                // 当前信息有效长度
    u16 vid;                    // pcie vendor id
    u16 did;                    // pcie device ID
    u16 svid;                   // pcie subsystem Vendor ID
    u16 ssid;                   // subsystem ID
    u8 bus;                     // pci总线号（0）
    u8 device;                  // pci设备号（0）
    u8 pf0_enable;              // 当pf0使能时,呈现为1个设备
    u8 pf1_enable;              // 当pf1使能时,呈现为2个设备
    u8 ep_pcie_width;           // 协商宽度:1,2,4,8,16  （代表x1，x2，x4，x8）
    u8 ep_pcie_speed;           // 当前协商速率
    u64 sas_address;            // 控制器的sas地址
    char chip_name[16];         // 显示芯片名称 40bytes
    char chip_version[8];       // 芯片版本信息 （V100）
    u8 die_id[20];              // 显示芯片ID (20位的16进制数) （0）
    u8 rsv[4];
    char board_sn[32];          // 预留31个字节（当前20个字节）,生产阶段加载，从barcode提取 （0）
    char fw_version0[16];       // 分区0的FW版本 //预留15个字节 （1.5.00）
    char fw_version1[16];       // 分区1的FW版本 （1.5.01）136bytes
    u8 current_fw;              // 当前运行分区: 0 分区0， 1:分区1 （0）
    u8 ctrl_mod;                // 控制器当前工作模式 RAID、HBA
    short chip_tempature;       // -40 ?C 125
    u16 disk_num;               // 在位的物理盘数（开关phy或online、offline）
    u16 vd_num;                 // 逻辑盘数量
    u16 vd_degrade_num;         // 降级的vd数量
    u16 vd_fault_num;           // 故障的vd数量
    u16 rg_num;                 // raid组数量
    u16 disk_prefail_num;       // 盘预失效数量
    u16 disk_fail_num;          // 盘失效数量
    u16 ddr_size;               // 单位MB
    u16 ddr_ecc_count;          // DDR统计值
    u16 ddr_ecc_bucket_size;    // 可纠正错误门限 160bytes
    u16 ddr_ce_count;
    u16 ddr_uce_count;          // 如果检测到不可纠正错误，增加，(包括多bit ECC)，大概率这时系统回挂死
    u16 sram_ecc_count;         // 系统中sram 单bit ecc错误统计
    u16 sram_uce_count;         // sram不可纠正错误统计
    u8 cache_pinned;            // 是否有pinned cache （cache提供状态）（0）
    u8 need_check;              // Errors need fw to check
    u16 board_hw_err;           /* 内部模块检测故障（导致IO停止）
                                 * bit0: TSP异常
                                 * bit1: NAC异常
                                 * bit2: FW异常
                                 * bit3: IMU异常
                                 * bit4: 备电失败
                                 * bit5: SOC_FATAL
                                 * bit6: DDR_MBIT_ERR
                                 * bit7: SRAM_ERR
                                 * bit8: DDRC_ERR
                                 * bit9: RDE_ERR
                                 * bit10: SAS_CONTROLLER_ERR
                                 * bit11 - bit15: rsved */
    u8 board_clk_err;           /* 内部模块时钟故障
                                 * bit0: PCIe EP参考时钟故障
                                 * bit1: PCIe RP参考时钟故障
                                 * bit2: SAS 端口参考时钟故障
                                 * bit3 - bit7: rsved */
    u8 board_power_err;         /* bit0: 电源芯片异常（SYH664）工作状态
                                 * bit1: 内部电源芯片输出电压过高
                                 * bit2: 内部电源芯片输出电压过低
                                 * bit3 - bit7: rsved */
    u8 board_capacity_err;      /* 内部备电模块电容故障
                                 * bit0: 芯片内部模块检测错误
                                 * bit1: 超级电容工作电压过低
                                 * bit2: 超级电容工作电压过高
                                 * bit3: 小电容容值异常 (S_CAPACITY_ERR)
                                 * bit4: 超级电容容值异常
                                 * bit5: 小电容工作电压过低
                                 * bit6: 小电容工作电压过高
                                 * bit7: rsved */
    u8 board_flash_err;         /* 内部flash模块故障
                                 * bit0: SPI Nor Flash故障
                                 * bit1: SPI Nand Flash故障 (hba bp_Flash_err)
                                 * bit2: NFC Nand Flash故障 (raid bp_Flash_err)
                                 * bit3 - bit7: rsed */
    char board_name[CONTROLLER_STATUS_BOARD_NAME_LEN];        // 单板型号名称
};

/*****************************************************************************
 Description  : 获取超级电容的状态信息
 command      : toolname cx:sc show supercap
 Direction    : opcode=0xC2  subopcode=ADM_CMD_GET_SC_INFO
*****************************************************************************/
struct cmd_supercapacitor_info {
    u8 voltage;                   // 电容模组电压（单位0.1V）
    u8 pcb_version;               // 电容模组PCB版本号（0000 0000=A；0000 0001=B）
    char temperature;             // 电容模组温度（返回摄氏度）
    char alarm_temp;              // 温度门限
    u32 design_capacity;          // 电容模组设计容量(0.01F为单位)
    char module_version[8];       // 电容模组版本号(000001 ~ 065535)
    char cap_version[8];          // 超级电容版本号(ABCD),当前使用4个字节
    char device_name[16];         // 电容模组设备名
    char vendor_name[8];          // 电容模组厂商名简写
    char cap_sn[16];              // 超级电容SN序列号
    char device_sn[16];           // 电容模组SN号
    char cap_manufacture[12];     // 超级电容出厂日期
    char device_manufacture[12];  // 电容模组生产日期
    char pack_assm_no[8];         // 电容模组打包序列号
    char pcb_assm_no[16];         // 电容模组PCB装配号
    u8 present;                   // 在位状态(0-不在位，1-在位)
    u8 learn_status;              /* 自学习状态:
                         0: 正在自学习（bit位置1有效）
                         1: 自学习成功（bit位置1有效）
                         2: 自学习失败（bit位置1有效）
                         3: 自学习超时（bit位置1有效）
                         全0:没有自学习
                     */
    u8 vendor_id;                 // 电容模组厂商ID（显示为01）
    u8 flash_erase_status;  // flash擦除状态（0-没有擦除，1-擦除成功，2-擦除失败，3-正在擦除）
    u8 sc_status;           /* 各个bit表示状态:
                      0: mos_status   电容MOS的打开和关闭状态（1-打开）
                      1: high_temp_alarm电容模组温度过高（1有效）
                      2:low_vol_alarm 电容模组电压过低（1有效）
                      3:low_capacity_alarm    电容模组剩余容量低（1有效）
                      4: replace_required 需要更换（1有效）
                      5: nospace_offload   没有空间备份cache（1有效）
                      6: pack_missing 电容不在位（1有效）
                      7: 保留
                      说明:bit 2和bit 3在自学习时不会置位，自学习完成后根据结果置位
                  */
    u8 status;              // 备电模块总体状态（0-FAILED，1-OK）
    u8 signal_alarm;        // 电容iic通道异常 (1-FAILED, 0-OK)
    u8 rsvd0[9];           // 保留字段，后续扩充，默认填0
};

enum cmd_channal_type {
    CHANNAL_TYPE_INBAND_SCSI = 0,
    CHANNAL_TYPE_OUTBAND_PCIE,
    CHANNAL_TYPE_OUTBAND_I2C,
    CHANNAL_TYPE_INBAND_NVME,
    CHANNAL_TYPE_INVALID,
};

enum cmd_pf_type {
    PF_TYPE_PF0 = 0,
    PF_TYPE_PF1,
    PF_TYPE_INVALID,
};

struct outband_cmd_req_header {
    u8 opcode;              // 操作码
    u8 cmd_flags;           // 预留lib库和fw件传递控制信息
    u16 cid;                // 消息序列号，用于将命令与CQE对应，由lib库分配
    u16 subopcode;          // 子命令字，用于定义具体管理功能
    u8 obj_flag;            // 标记obj的类型，当前主要用于区分是用EID/SID表示物理盘，还是DID表示物理盘
    u8 Rsvd1;
    u16 obj;                // 用于描述对象的id（包括磁盘ID，VD 的ID， RG的ID）
    u16 obj_ex;             // 如果使用EID/SID表示物理盘时，这里填充EID
    u16 param_len;          // 指定参数长度
    u16 data_len;           // 指定数据长度（不超过4K）
    u16 data_offset;        // 数据相对 opcode开始算的起始位置
    u16 version;            // 用于lib库和固件版本匹配
    u32 timeout_ms;         // 命令超时时间，单位:毫秒
    u8 data[0];             /* 填充参数和数据。考虑到和带内命令字兼容，如果参数长度小于20，则可以带有数据字段，否则不能带有数据字段
                                数据的起始地址，按照8字节对齐，如果不对齐，使用pad填充，如果总长度不是四字节对齐，填充pad（0）对齐
                            */
};

struct outband_cmd_rsp_header {
    u32 cqedw0;             // 返回命令特殊的返回结果（如果查询信息小于32bit，则使用这个字段返回结果）
    u32 cqedw1;             // 预留（为了兼容NVMe开源驱动，有的版本只能返回32bit值给用户态，因此这个字段尽量不使用）
    u16 data_len;           // 指定数据长度（不超过4K）
    u16 data_offset;        // 数据相对 cqedw0开始算的起始位置
    u16 cid;                // 消息序列号，用于将命令与CQE对应，由lib库分配
    u16 status;             // 返回命令通用的执行状态（成功，失败）
    u8 data[0];
};

#define MAX_HOST_EVENT_NUMBER 14
struct event_info {
    u8 type;                // 关注的事件类型
    u8 level;               // 关注的事件最低级别（比如关注的最低级别是info，更高级别的warning，error也会上报）
} __attribute__((packed));

struct host_event_info {
    u8 eid;                 // 带外mctp对应的eid
    u8 event_num;           // 关注的事件数量，设置events的数量
    u16 rsv;
    struct event_info events[MAX_HOST_EVENT_NUMBER];    // 关注的具体事件
};

struct host_powercycle_result {
    u16 enc_id;     // 框号
    u16 slot_id;    // 槽位号
    int ret;        // 下电再上电是否成功，0:成功，非0:失败
};

/*****************************************************************************
 Description  : set raid bgtask speed
 command      : toolname cx set bgrate function={rebuild|sanitize|bginit|expandrg|copyback|precopy
                                                 |precopy|consistencycheck(ccheck)|patrolread|all
                                        rate={high|medium|low}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_BGRATE
*****************************************************************************/
struct cmd_set_bgrate {
    u8 bg_type;
    u8 rate;
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : 显示逻辑盘一致性校验信息
 command      : toolname cx:vdx show ccheck (consistencycheck)
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_CONSISTENCY_CHECK
*****************************************************************************/
struct cmd_get_vd_cc {
    u16 vd_id;
    u8 sw;
    u8 period;
    u8 rate;
    u8 repair;
    u8 run_status;
    u8 progress;
    u16 left_time;
    u8 rsv;
    u8 delay;   // 延迟时间，单位h
    u16 magic;  // CMD_MAGIC_FLAG 兼容之前版本，定义标志
};

/*****************************************************************************
 Description  : 显示raid卡一致性校验信息
 command      : toolname cx show ccheck (consistencycheck)
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_CONSISTENCY_CHECK
*****************************************************************************/
struct cmd_get_rd_cc {
    u8 sw;           // 0为off 1为on
    u8 period;       // 周期  :单位为day      取值范围[1:60] Period
    u8 rate;         // ADM_BGTASK_RATE_LOW = 1 ADM_BGTASK_RATE_MIDDLE = 2 ADM_BGTASK_RATE_HIGH = 3
    u8 repair;       // ADM_AUTOCC_REPAIRE_AUTO =0显示为on      ADM_AUTOCC_REPAIRE_MANU = 1 显示为off
    u8 run_status;   // 0 stop     1 run
    u8 total_vd;     // 需要校验的总数量
    u8 complete_vd;  // 已经完成的数量
    u8 delay;        // 延迟时间，单位h
    u16 magic;       // CMD_MAGIC_FLAG 兼容之前版本，定义标志
    u8 rsvd[54];     // 总 64
};

/*****************************************************************************
 Description  : 开始\停止逻辑盘一致性校验信息
 command      : toolname cx:vdx start ccheck delay=x
                toolname cx:vdx stop ccheck
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_VD_CONSIS_CHECK
*****************************************************************************/
struct cmd_set_vd_cc {
    struct cmd_vd_id loc;
    u8 sw;      // 0: off (stop)  1:on （start） 2: set
    u8 rsvd0[3];
    u32 delay;  // 任务的等待开始时间，单位小时
    u8 rsvd[4];
};

/*****************************************************************************
 Description  : 设置/开始/停止卡级一致性校验
 command      : toolname cx start ccheck period=x repair={on|off} rate={low|medium|high}
                toolname cx stop ccheck
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_CONSIS_CHECK
*****************************************************************************/
struct cmd_set_cc {
    u8 sw;      // 0: off (stop)  1:on （start） 2: set
    u8 period;  // 周期  :单位为day      取值范围[1:60] Period
    u8 rate;    // ADM_BGTASK_RATE_LOW = 1 ADM_BGTASK_RATE_MIDDLE = 2 ADM_BGTASK_RATE_HIGH = 3
    u8 repair;  // ADM_AUTOCC_REPAIRE_AUTO =0显示为on      ADM_AUTOCC_REPAIRE_MANU = 1 显示为off
    u32 delay;  // 任务的等待开始时间，单位小时
    u8 mask;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : 开启raid卡自动巡检
 command      : toolname cx start patrolread period=x drivenum=x rate={low|medium|high}
                toolname cx stop patrolread
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_PATROLREAD
*****************************************************************************/
struct cmd_start_patrolread {
    u8 sw;        // 0: off (stop)  1:on （start） 2: set
    u8 period;    // 周期，单位:天，取值范围[1:60]
    u8 rate;      // ADM_BGTASK_RATE_LOW = 1 ADM_BGTASK_RATE_MIDDLE = 2 ADM_BGTASK_RATE_HIGH = 3
    u8 disk_num;  // 磁盘同时巡检的硬盘数量，1-8；默认4。
    u32 delay;    // 任务的等待开始时间，单位小时
    u8 mask;      // bit控制本次命令传递相关参数值是否有效，按顺序将bit位置1有效。用于保留用户历史参数设置，进行增量修改。
    u8 rsvd[3];   // 对齐预留
};

/*****************************************************************************
 Description  : 设置紧急热备开关状态
 command      : toolname cx set ehsp sw={on|off}
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_EHSP
*****************************************************************************/
struct cmd_glb_ehsp {
    u8 ehsp_status;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : 设置后台任务开关状态
 command      : toolname cx set bgtask sw={on|off}
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_SET_BGTASK
*****************************************************************************/
struct cmd_glb_bgtask {
    u8 bgtask_status;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : 扩展vd容量
 command      : toolname cx:vdx expand size=x
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_EXPAND_VD_SIZE
*****************************************************************************/
struct cmd_expand_vd {
    struct cmd_vd_id loc;
    u64 capacity;  // 扩展容量MB单位
    u32 rsvd;
};

/*****************************************************************************
 Description  : 扩展rg磁盘数量
 command      : toolname cx:rgx expand {drives=ex:sx,ey:sy OR drives=x,y (dids)}
 Direction    : ADM_RAID_SET  SUBOPCODE:ADM_CMD_EXPAND_RG_DISK
*****************************************************************************/
struct cmd_expand_rg_disk {
    struct cmd_rg_id loc;
    u16 num;
    u8 locate_type;
    u8 rsvd;
    union {  // 如果location_type为0,则使用struct disk_location,否则使用u16 did[80]
        struct disk_location disks[CMD_MEMB_DISK_MAX_NUM];
        u16 did[CMD_MEMB_DISK_MAX_NUM];
    };
};

/*****************************************************************************
 Description  : 扩盘进度查询
 command      : toolname cx:rgx show expand
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_RG_EXP_PROGRESS
*****************************************************************************/
struct cmd_expand_disk_info {
    u16 rg_id;
    u16 left_time;  //  剩余预估时间，单位，min
    u8 progress;    //  0-100
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : 对逻辑盘进行安全擦除
 command      : toolname cx:vdx start sanitize mode={simple|normal|thorough}
                toolname cx:vdx stop sanitize
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_VD_SANITIZE
*****************************************************************************/
struct cmd_set_vd_sanitize {
    struct cmd_vd_id loc;
    u8 sw;
    u8 mode;  // simple擦一轮normal 擦三轮 thorough 执行九轮
    u8 pattern[2];
    u8 is_valid;
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : 查询逻辑盘VD安全擦除信息
 command      : toolname cx:vdx show sanitize
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_SANITIZE
*****************************************************************************/
struct cmd_vd_sanitize_info {
    u16 id;
    u8 status;
    u8 rate;
    u8 sanitize_times;
    u8 current_step; /* delete */
    u8 progress;
    u8 rsvd0;
    u8 name[31 + 1];
    u64 capacity;
    u16 left_time;
    u8 sanitize_result;
    u8 rsvd1;
};

/*****************************************************************************
 Description  : 查询逻辑盘VD的分条不一致信息
 command      : toolname cx:vdx show inconsistent
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_INCONSISTENT
*****************************************************************************/
struct cmd_vd_inconsistent_info {
    u32 stripe_num;
    struct {
        u16 vd_id;
        u16 rsvd;
        u32 stripe_id;
        u64 start_lba;
        u64 len;
    } stripe[MAX_INCONSISTENT_STRIPE_NUM];  // 100
};

/*****************************************************************************
 Description  : 查询存储设备坏块标记数量
 command      : toolname cx/cx:ex:sx show bst slba=xx (512, 512kB,512M, 512G) num=xx
 Direction    : SUBOPCODE:ADM_CMD_SHOW_BST
*****************************************************************************/

struct cmd_bst_num_param {
    struct multi_disk_location loc;  // 在获取所有bst时，复用单盘bst的获取方式。只是将flag置一，将did置为全f.
    u64 start_lba;
    u32 len;
};

struct cmd_bst_data_idx {
    struct multi_disk_location loc;  // 在获取所有bst时，复用单盘bst的获取方式。只是将flag置一，将did置为全f.
    u64 start_lba;
    u32 len;
    u16 start_idx;
    u16 count;
};

struct cmd_bst_data_entry {
    u16 did;
    u64 start_lba;  // 开始扇区位置
    u32 nlb;        // 扇区数量
    u8 type;        // 1:Logical     0:Physical
    u8 rsvd[3];
    u32 index;
};
#define CMD_BST_MAX_NUM 8192U
#define CMD_BST_DATA_ONCE_NUM ((ADM_MAX_DATA_SIZE - 64U) / sizeof(struct cmd_bst_data_entry))
struct cmd_show_bst {
    u16 num;
    u16 rsvd0;
    u32 rsvd1[15];
    struct cmd_bst_data_entry bsts[CMD_BST_DATA_ONCE_NUM];
};

/*****************************************************************************
 Description  : 设置硬盘例测周期
 command      : toolname cx:ex:sx set routine sw={on|off}
                toolname cx show jbod/rawdrive
 Direction    : opcode=0xC0  subopcode=0x15/0x16 ADM_CMD_SET_PASSTHRU_TYPE
                                                 ADM_CMD_SHOW_PASSTHRU_TYPE
*****************************************************************************/
struct cmd_glb_passthru_type {
    u8 passthru_type;  // 0:关；1:JBOD模式 2:RAWDISK模式
    u8 rsvd[3];        // 对齐预留
};

/*****************************************************************************
 Description  : SMART扫盘周期设置和查询
 command      : toolname cx set routine_cycle period=x
 Direction    : opcode:SET  ADM_CMD_SET_SMART_POLLING
                            ADM_CMD_SHOW_SMART_POLLING
*****************************************************************************/
struct cmd_smart_polling {
    u32 period;  // 例测时间,单位s，默认值:60
    u32 rsvd;
};

/*****************************************************************************
 Description  : 设置phy口属性
 Command      : toolname cx:ex:sx set phy func={hardreset|linkreset|enable|disable|speed}
 Direction    : none  ADM_CMD_SET_DISK_PHY
*****************************************************************************/
struct cmd_set_phy {
    struct multi_disk_location loc;
    u8 func;  // 功能函数对应的序号
    u8 speed;
    u8 rsvd[6];
};

/*****************************************************************************
 Description  : 保存phy口属性
 Command      : toolname cx:ex:sx set phy_cfg sw={enable|disable} [sp=speed(12G)]
 Direction    : none  ADM_CMD_SET_PHY_CFG
*****************************************************************************/
struct cmd_set_phy_cfg {
    struct multi_disk_location loc;
    u8 sw;
    u8 speed;
    u8 rsvd[6];
};

/*****************************************************************************
 Description  : 查询 phy cfg
 Command      : toolname cx show phy_cfg
 Direction    : none  ADM_CMD_SHOW_PHY_CFG
*****************************************************************************/
typedef struct _flash_phy_cfg_info { /* 12B */
    u8 flag;  // 1已配置，0xff未配置
    u8 slot_id;
    u8 phy_link_rate;
    u8 phy_switch;
    u32 rsv[2];
} flash_phy_cfg_info;

/*****************************************************************************
 Description  : 设置磁盘队列深度
 Command      : toolname cx:ex:sx set phy func={hardreset|linkreset|enable|disable|speed}
 Direction    : none  ADM_CMD_SET_DISK_QUEUE_DEPTH
*****************************************************************************/
struct cmd_set_disk_queue_depth {
    struct multi_disk_location loc;
    u16 depth;
    u16 rsvd;
};

/*****************************************************************************
 Description  : 获取硬盘IO个数统计
 command      : toolname cx:ex:sx diskIO
 Direction    : opcode: ADM_CMD_SHOW_DISK_IO_SUM
*****************************************************************************/
struct cmd_show_disk_io_sum {
    u64 io_sum;
    u64 failed_io_sum;
    u8 rsvd[4];
};

/*****************************************************************************
 Description  : 设置、查询硬盘的写cache状态
 command      : toolname cx:ex:sx diskIO
 Direction    : opcode:  ADM_CMD_SET_DISK_CACHE
                         ADM_CMD_SHOW_DISK_CACHE
*****************************************************************************/
struct cmd_disk_cache {
    struct multi_disk_location loc;
    u8 cache_status;
    u8 rsvd[3];
};

#define VDS_ID_NODE_NUM         128
/*****************************************************************************
 Description  : BMC使用，一次获取所有逻辑盘vd的id
 command      : toolname cx:ex:sx diskIO
 Direction    : opcode:  ADM_CMD_SHOW_VDS_ID
*****************************************************************************/
struct cmd_vds_id {
    u16 ids[VDS_ID_NODE_NUM];
    u16 num;  // 当前含有的逻辑盘数量
    u8 rsvd[2];
};

/*****************************************************************************
 Description  : pcie链路误码查询
 command      : toolname cx
 Direction    : opcode:
*****************************************************************************/
union pcie_mac_int_status {
    struct {
        u32 reg_linkup_intr_status : 1;                  /* [0] */
        u32 reg_linkdown_intr_status : 1;                /* [1] */
        u32 reg_enter_l0_intr_status : 1;                /* [2] */
        u32 reg_leave_l0_intr_status : 1;                /* [3] */
        u32 reg_enter_disable_intr_status : 1;           /* [4] */
        u32 reg_leave_disable_intr_status : 1;           /* [5] */
        u32 reg_hot_reset_intr_status : 1;               /* [6] */
        u32 rxl0s_to_recovery_intr_status : 1;           /* [7] */
        u32 reg_phystatus_timeout_intr_status : 1;       /* [8] */
        u32 reg_deskew_unlock_intr_status : 1;           /* [9] */
        u32 reg_symbol_unlock_intr_status : 1;           /* [10] */
        u32 reg_loopback_unlock_intr_status : 1;         /* [11] */
        u32 reg_deskew_fifo_overflow_intr_status : 1;    /* [12] */
        u32 reg_enter_l1_timeout_intr_status : 1;        /* [13] */
        u32 reg_enter_l2_timeout_intr_status : 1;        /* [14] */
        u32 reg_enter_recovery_lock_intr_status : 1;     /* [15] */
        u32 reg_enter_recovery_cfg_intr_status : 1;      /* [16] */
        u32 reg_enter_eq_p3_intr_status : 1;             /* [17] */
        u32 pl_eco_rsv_intr_status : 4;                  /* [21:18] */
        u32 ltssm_tracer_sram_ecc_1bit_err : 1;          /* [22] */
        u32 ltssm_tracer_sram_ecc_2bit_err : 1;          /* [23] */
        u32 reg_enter_g1_recovery_speed_intr_status : 1; /* [24] */
        u32 reg_enter_g2_recovery_speed_intr_status : 1; /* [25] */
        u32 reg_enter_g3_recovery_speed_intr_status : 1; /* [26] */
        u32 reg_ltssm_intr_status : 1;                   /* [27] */
        u32 reg_enter_lpbk_lock_intr_status : 1;         /* [28] */
        u32 reg_enter_g4_recovery_speed_intr_status : 1; /* [29] */
        u32 reserved : 2;                                /* [31:30] */
    } bits;
    u32 val;
};  // 0x054

union pcie_mac_reg_test_counter {
    struct {
        u32 phy_lane_err_counter : 16; /* [15:0] */
        u32 reserved : 16;             /* [31:16] */
    } bits;
    u32 val;
};  // 0x05c

union pcie_mac_reg_debug_pipe2 {
    struct {
        u32 mac_powerdown_1 : 24; /* [23:0] */
        u32 mac_ltssm_st : 6;     /* [29:24] */
        u32 reserved : 2;         /* [31:30] */
    } bits;
    u32 val;
};  // 0x068

union pcie_mac_reg_debug_pipe3 {
    struct {
        u32 phy_phystatus : 16; /* [15:0] */
        u32 mac_ltssm_st : 6;   /* [21:16] */
        u32 reserved : 10;      /* [31:22] */
    } bits;
    u32 val;
};  // 0x06c

union pcie_mac_symbol_unlock_counter {
    struct {
        u32 symbol_unlock_counter : 16; /* [15:0] */
        u32 reserved : 16;              /* [31:16] */
    } bits;
    u32 val;
};  // 0x08c

union pcie_mac_reg_debug_pipe10 {
    struct {
        u32 phy_rxdata_valid : 16; /* [15:0] */
        u32 mac_ltssm_st : 6;      /* [21:16] */
        u32 reserved : 10;         /* [31:22] */
    } bits;
    u32 val;
};  // 0x0254

union pcie_mac_pcs_rx_err_cnt {
    struct {
        u32 pcs_rx_err_cnt : 16; /* [15:0] */
        u32 reserved : 16;       /* [31:16] */
    } bits;
    u32 val;
};  // 0x02BC

union pcie_mac_level_l0_info {
    struct {
        u32 up_retrain_redo_req : 1;     /* [0] */
        u32 dp_redp_eq_enter_rcv : 1;    /* [1] */
        u32 cfg_link_disable : 1;        /* [2] */
        u32 enter_loop_back : 1;         /* [3] */
        u32 framing_err_retrain : 1;     /* [4] */
        u32 dl_retran : 1;               /* [5] */
        u32 skp_infter_idle_timeout : 1; /* [6] */
        u32 hot_reset : 1;               /* [7] */
        u32 cfg_retrain_req : 1;         /* [8] */
        u32 reg_retrain_req : 1;         /* [9] */
        u32 det_eieos_128b130b : 1;      /* [10] */
        u32 det_ts_train : 1;            /* [11] */
        u32 direct_speed_change : 1;     /* [12] */
        u32 tl_ap_req_link_down : 1;     /* [13] */
        u32 dl_req_link_down : 1;        /* [14] */
        u32 enter_l2_req : 1;            /* [15] */
        u32 enter_l1_req : 1;            /* [16] */
        u32 enter_l0s_req : 1;           /* [17] */
        u32 enter_l1l2_timeout : 1;      /* [18] */
        u32 rcv_eios : 1;                /* [19] */
        u32 resvered : 1;                /* [31:20] */
    } bits;
    u32 val;
};  // 0x3b4

union pcie_apb_lane_error_status0 {
    struct {
        u32 symbol_unlock_err_status : 16; /* [15:0] */
        u32 pcs_rcv_err_status : 16;       /* [31:16] */
    } bits;
    u32 val;
};  // 0x3bc

union pcie_apb_lane_error_status1 {
    struct {
        u32 phy_lane_err_status : 16;           /* [15:0] */
        u32 loopback_link_data_err_status : 16; /* [31:16] */
    } bits;
    u32 val;
};  // 0x3c0

union pcie_dl_dfx_lcrc_err {
    struct {
        u32 dl_lcrc_err_num : 8; /* [7:0] */
        u32 resvered : 24;       /* [31:8] */
    } bits;
    u32 val;
};  // 0x50

union pcie_dl_dfx_dcrc_err {
    struct {
        u32 dl_dcrc_err_num : 8; /* [7:0] */
        u32 resvered : 24;       /* [31:8] */
    } bits;
    u32 val;
};  // 0x54

union pcie_dl_retry_cnt {
    struct {
        u32 dfx_retry_cnt : 16; /* [15:0] */
        u32 resvered : 16;      /* [31:16] */
    } bits;
    u32 val;
};  // 0xb8

union pcie_dl_mac_retry_cnt {
    struct {
        u32 dl_mac_retrain_cnt : 8; /* [7:0] */
        u32 resvered : 24;          /* [31:8] */
    } bits;
    u32 val;
};  // 0xd4

union pcie_dl_rx_nak_count {
    struct {
        u32 rx_nak_count : 16; /* [15:0] */
        u32 resvered : 16;     /* [31:16] */
    } bits;
    u32 val;
};  // 0x00F8

union pcie_cfg_cor_err_status {
    struct {
        u32 rx_err_st : 1;                 /* [0] */
        u32 reserved0 : 5;                 /* [5:1] */
        u32 bad_tlp_st : 1;                /* [6] */
        u32 bad_dllp_st : 1;               /* [7] */
        u32 reply_num_rollover_st : 1;     /* [8] */
        u32 reserved1 : 3;                 /* [11:9] */
        u32 reply_timer_timout_st : 1;     /* [12] */
        u32 advisory_non_fatal_err_st : 1; /* [13] */
        u32 cor_int_err_st : 1;            /* [14] */
        u32 header_log_overflow_st : 1;    /* [15] */
        u32 reserved2 : 16;                /* [31:16] */
    } bits;
    u32 val;
};  // 0x110

struct cmd_pcie_link_error {
    union pcie_mac_int_status mac_int_status;
    union pcie_mac_reg_test_counter test_counter;
    union pcie_mac_reg_debug_pipe2 dbg_pipe2;
    union pcie_mac_reg_debug_pipe3 dbg_pipe3;
    union pcie_mac_symbol_unlock_counter sys_unlock;
    union pcie_mac_reg_debug_pipe10 dbg_pipe10;
    union pcie_mac_pcs_rx_err_cnt pcs_rx_err;
    union pcie_mac_level_l0_info mac_level_10_info;
    union pcie_apb_lane_error_status0 apb_lane_err_st0;
    union pcie_apb_lane_error_status1 apb_lane_err_st1;
    union pcie_dl_dfx_lcrc_err lcrc;
    union pcie_dl_dfx_dcrc_err dcrc;
    union pcie_dl_retry_cnt dl_retry;
    union pcie_dl_mac_retry_cnt dl_mac_retry;
    union pcie_dl_rx_nak_count rx_nak_count;
    union pcie_cfg_cor_err_status cfg_cor_err;
};

#define MAX_EXMBIST_BURST 2
#define MAX_EXMBIST_FAULT_RECORD 10

typedef union _U_EXMBIST_ROWCOL {
    struct {
        u32 row : 18;    // 出错Row
        u32 col : 10;    // 出错Col
        u32 bank : 4;      // [31:28] 出错Bank
    } bits;
    /* Define an unsigned member */
    u32    value;
}U_EXMBIST_ROWCOL;
typedef union _U_EXMBIST_ERRINFO {
    struct {
        u32 type : 2;      // [1:0] 表示错误类型 00 -> 无故障, 01 -> CE, 10 -> UCE
        u32 socket : 2;    // [3:2] 出错Socket 00 -> Socket0, 01 -> Socket1
        u32 dimm : 1;      // [4] 出错DIMM 0 -> DIMM0, 1-> DIMM1
        u32 rank : 2;      // [6:5] 出错Rank 0 -> Rank0, 1-> Rank1
        u32 channel : 3;   // [9:7] 000 -> Ch0, ... ,110 -> Ch6
        u32 dramWidth : 2; // [11:10] 内存位宽
        u32 alo : 5;       // [16:12] 出错算法
        u32 patternId : 5; // [21:17] 出错pattern
        u32 reserve : 10;   // [31:22] 保留位
    } bits;
    /* Define an unsigned member */
    u32    value;
}U_EXMBIST_ERRINFO;
typedef struct _exmbist_error_recode {
    u64 err_bit[MAX_EXMBIST_BURST];   // 出错bit位
    U_EXMBIST_ROWCOL row_col;
    U_EXMBIST_ERRINFO err_info;
    u16 err_cnt;                      // 错误计数
    u8 err_burst[MAX_EXMBIST_BURST];  // 出错Burst
    u8 temperature;                   // 温度
    u8 rsv[3];
}exmbist_error_recode;

struct exmbist_sum {
    u32 idx;  // 记录 record 当前的 idx
    u32 rsv1;
    exmbist_error_recode fault_record[MAX_EXMBIST_FAULT_RECORD];
    u8 status;  // 4:需要测 3:Running 2:fail, 1:pass, 0:默认
    u8 cur_pattern; // 记录当前处于什么pattern阶段，for debug
    u8 cur_algorithm; // 记录当前处于什么算法阶段, for debug
    u8 rsv2;
    u32 magic;
};

typedef struct exmbist_sum hw_exmbist_sum;

struct set_exmbist_param {
    u8 status;  // 4:需要测 3:Running 2:fail, 1:pass, 0:默认
    u8 rsv[3];
};

enum set_exmbist_status {
    EXMBIST_STATUS_DEFALUT = 0,
    EXMBIST_STATUS_PASS,
    EXMBIST_STATUS_FAIL,
    EXMBIST_STATUS_RUNNING,
    EXMBIST_STATUS_TEST,
};

/*****************************************************************************
 Description  : 获取ecc信息
 command      : toolname cx show ecc_info
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SRAM_ECC_INFO
*****************************************************************************/
struct sram_ecc_info {
    u16 peri_single_ecc;
    u16 peri_mutil_ecc;
    u16 imu_single_ecc;
    u16 imu_mutil_ecc;
};

/*****************************************************************************
 Description  : 获取ddr信息
 command      : toolname cx show ddr_info
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_DDR_INFO
*****************************************************************************/
struct cmd_ddr_info_rslt {
    u8 channel;
    u8 ddr_size;
    u8 ddr_num;
    u8 ddr_total_size;
};

enum ddr_info_total_size {
    DDR_INFO_TOTAL_SIZE_256MB = 0,
    DDR_INFO_TOTAL_SIZE_512MB,
    DDR_INFO_TOTAL_SIZE_1GB,
    DDR_INFO_TOTAL_SIZE_2GB,
    DDR_INFO_TOTAL_SIZE_4GB,
    DDR_INFO_TOTAL_SIZE_8GB,
    DDR_INFO_TOTAL_SIZE_16GB,
    DDR_INFO_TOTAL_SIZE_BUTT
};

enum ddr_info_size {
    DDR_INFO_SIZE_256MB = 0,
    DDR_INFO_SIZE_512MB,
    DDR_INFO_SIZE_1GB,
    DDR_INFO_SIZE_2GB
};

/*****************************************************************************
 Description  : 读写芯片efuse数据
 command      : toolname cx show efuse id=x index=x count=x
 command      : toolname cx set efuse file=xxx
 Direction    : SUBOPCODE:ADM_CMD_SHOW_EFUSE ADM_CMD_SET_EFUSE
*****************************************************************************/
struct efuse_param_s {
    u8 efuse_id;
    u8 efuse_index;
    u8 efuse_count;
    u8 rsv;
};

#define CHIP_ADC_CHANNEL 5
struct cmd_chip_adc {
    int adc_val[CHIP_ADC_CHANNEL];
};

struct cmd_chip_tsensor {
    int cpu_temperature;
};

/*****************************************************************************
 Description  : 配置sas地址
 command      : toolname cx set sas addr=xxx
 Direction    : SUBOPCODE:ADM_CMD_SET_SAS_ADDR
*****************************************************************************/
struct cmd_sas_addr {
    u64 sas_addr;
};

/*****************************************************************************
 Description  : 配置电子标签eLable
 command      : toolname cx set elable file=xxx
 Direction    : SUBOPCODE:ADM_CMD_SET_ELECTRON_LABEL
*****************************************************************************/
struct cmd_electron_label {
    u8 board_type[32];
    u8 board_sn[32];
    u8 item[32];
    u8 description[300];
    u8 manufactured[32];
    u8 vendor_name[32];
    u8 issue_number[32];
    u8 clei_code[32];
    u8 bom[32];
    u8 model[32];
    u8 pcb[32];
};

struct sds_com_con_para {
    u8 phy_sds_id;
    u8 lane_num;  // 从phy_sds_id开始连续获取的lane_num个
};
struct cmd_sds_com_con_para {
    struct sds_com_con_para con_para;
    u8 rsv[2];
};

/*****************************************************************************
 Description  : 环回类型设置
 command      : toolname cx set loop id=xx num=xx type=xx
                type:0-ALL_DISABLE,1-PARALLEL_TX_TO_RX,2-PARALLEL_RX_TO_TX,
                3-SERIAL_TX_TO_RX,4-SERIAL_RX_TO_TX,5-SERIAL_PRE_DRV_TX_TO_RX
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_LOOP_TYPE
*****************************************************************************/
enum CMD_SERDES_LOOPBACK_TYPE {
    CMD_SERDES_LOOPBACL_ALL_DISABLE = 0,             /* <去使能所有环回 */
    CMD_SERDES_LOOPBACK_PARALLEL_TX_TO_RX = 1,       /* <Serdes并行接口内环 */
    CMD_SERDES_LOOPBACK_PARALLEL_RX_TO_TX = 2,       /* <Serdes并行接口外环 */
    CMD_SERDES_LOOPBACK_SERIAL_TX_TO_RX = 3,         /* <Serdes串行接口内环 */
    CMD_SERDES_LOOPBACK_SERIAL_RX_TO_TX = 4,         /* <芯片不支持，串行接口外环 */
    CMD_SERDES_LOOPBACK_SERIAL_PRE_DRV_TX_TO_RX = 5, /* <Serdes串行接口内环(不需端接) ，仅H30有效 */
    CMD_SERDES_LOOPBACK_END
};
struct cmd_sds_set_loop_para {
    struct sds_com_con_para con_para;
    u8 loop_type;
    u8 rsv;
};

struct cmd_sds_set_loop_rlt {
    u64 rlt_bit_mask;  // 对应bit为1代表对应physdsid执行失败，仅对传下来的id有效
};

/*****************************************************************************
 Description  : 环回类型获取
 command      : toolname cx show loop_type id=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_LOOP_TYPE
*****************************************************************************/
struct cmd_sds_get_loop_type {
    u8 loop_type;
    u8 rsv[3];
};

/*****************************************************************************
 Description  : 获取prbs状态
 command      : toolname cx show prbs_sta id=xx num=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PRBS_STA
*****************************************************************************/
struct cmd_serdes_prbs_status_s {
    u32 check_en;     /* <prbs检查使能 */
    u32 check_type;   /* <prbs检查类型 */
    u32 error_status; /* <prbs检查发现错误 */
    u64 error_cnt;    /* <prbs检查发现错误次数 */
    u32 alos_status;  /* <0:未los     1:los */
};

/*****************************************************************************
 Description  : 设置prbs
 command      : toolname cx set prbs id=xx num=xx dir={tx|rx|tx_rx} type={disable|prbs7|prbs9|prbs10|
 prbs11|prbs15|prbs20|prbs23|prbs31|prbs58|prbs63|define} [data0=xx data1=xx data2=xx]
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_PRBS ADM_CMD_SET_PRBS_CTRL
*****************************************************************************/
enum cmd_serdes_prbs_type {
    CMD_SERDES_PRBS_DISABLE = 0,
    CMD_SERDES_PRBS_PRBS7 = 1,  /* <X^7+X^6+1 */
    CMD_SERDES_PRBS_PRBS9 = 2,  /* <X^9+X^5+1 */
    CMD_SERDES_PRBS_PRBS10 = 3, /* <X^10+X^7+1 */
    CMD_SERDES_PRBS_PRBS11 = 4, /* <X^11+X^9+1 */
    CMD_SERDES_PRBS_PRBS15 = 5, /* <X^15+X^14+1 */
    CMD_SERDES_PRBS_PRBS20 = 6, /* <X^20+X^17+1 */
    CMD_SERDES_PRBS_PRBS23 = 7, /* <X^23+X^28+1 */
    CMD_SERDES_PRBS_PRBS31 = 8, /* <X^31+X^28+1 */
    CMD_SERDES_PRBS_PRBS58 = 9,
    CMD_SERDES_PRBS_PRBS63 = 10,
    CMD_SERDES_PRBS_USER_DEFINE = 11, /* <RX USER_DEFINE   H16V200: RX Pattern0101 */
    CMD_SERDES_PRBS_END
};
struct cmd_sds_set_prbs {
    struct sds_com_con_para con_para;
    u8 dir;  // 0-tx,1-rx,2(rsv)-tx and rx
    u8 prbs_type;
    u32 cus_data[3];  // 只有在prbs_type为usrdefine时才有效
};

/*****************************************************************************
 Description  : 查询prbs类型
 command      : toolname cx show prbs_type id=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PRBS_TYPE
*****************************************************************************/
struct cmd_sds_prbs_rlt {
    u16 tx_prbs_type;
    u16 rx_prbs_type;
};

/*****************************************************************************
 Description  : 电压获取
 command      : toolname cx show vol id=xx type={ss|cs|ds|cs_hvcc|cs_lvcc|ds_hvcc|all}
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_VOL
*****************************************************************************/
enum cmd_adc_type {
    CMD_ADC_TYPE_SS = 0,      /* <ss电压 */
    CMD_ADC_TYPE_CS = 1,      /* <cs电压 */
    CMD_ADC_TYPE_DS = 2,      /* <ds电压 */
    CMD_ADC_TYPE_CS_HVCC = 3, /* <HVCC电压 */
    CMD_ADC_TYPE_CS_LVCC = 4, /* <LVCC电压 */
    CMD_ADC_TYPE_DS_HVCC = 5, /* <HVCC电压 */
    CMD_ADC_TYPE_ALL = 6,     /* <电压 */
    CMD_ADC_TYPE_END
};
struct cmd_sds_vol_para {
    struct sds_com_phy_para com_para;
    u8 adc_type;
    u8 rsv[3];
};

struct cmd_sds_vol_rlt {
    u16 vol_mv;
    u8 rsv[2];
};

/*****************************************************************************
 Description  : keyinfo获取 返回num个
 command      : toolname cx show keyinfo id=xx num=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_KEY_INFO
*****************************************************************************/
struct cmd_sds_key_info_data {
    struct sds_tx_fir_data ffe;
    struct sds_ctle_data ctle_cfg;
    struct sds_rx_dfe tap_cfg;
    u32 alos;
    u32 dsapi12;
    u32 dsapi13;
};

/*****************************************************************************
 Description  : cdr获取 cdr设置
 command      : toolname cx show cdr id=xx
 command      : toolname cx set cdr id=xx num=xx cphs=xx ephs=xx phs=xx freq=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_CDR
                ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_CDR
*****************************************************************************/
struct cmd_serdes_cdr {
    u8 cphsofst;
    u8 ephsofst;
    u8 phsgn;
    u8 freqgn_cdrmode;
};

/*****************************************************************************
 Description  : 设置pn
 command      : toolname cx set pn id=xx num=xx dir={tx|rx|tx_rx} pn_val={0|1}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_PN
*****************************************************************************/
struct cmd_sds_set_pn {
    struct sds_com_con_para con_para;
    u8 dir;     // 0-tx,1-rx,2-tx and rx
    u8 pn_val;  // 0 or 1
};

/*****************************************************************************
 Description  : 获取pn
 command      : toolname cx show pn id=xx
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_PN
*****************************************************************************/
struct cmd_sds_pn_rlt {
    u16 tx_pn_val;
    u16 rx_pn_val;
};

/*****************************************************************************
 Description  : 获取固件版本信息
 command      : toolname cx show fw_version
 Direction    : SUBOPCODE:ADM_CMD_SHOW_FW_VERSION
*****************************************************************************/
#define FW_VERSION_COUNT    10
#define FW_VERSION_VERS_MAX 16
#define FW_VERSION_TIME_MAX 20
struct fw_version_s {
    u8 build_vers[FW_VERSION_VERS_MAX];
    u8 build_time[FW_VERSION_TIME_MAX];
};
struct version_query_s {
    struct fw_version_s region_master[FW_VERSION_COUNT];
    struct fw_version_s region_slave[FW_VERSION_COUNT];
    u8 master_vers[FW_VERSION_VERS_MAX];
    u8 slave_vers[FW_VERSION_VERS_MAX];
};

/*****************************************************************************
 Description  : 获取加载的固件版本信息
 command      : toolname cx show inactivefw
 Direction    : SUBOPCODE:ADM_CMD_SHOW_LOAD_FW_VERSION
*****************************************************************************/
struct load_version_query_s {
    struct fw_version_s region_load[FW_VERSION_COUNT];
    u32 valid_status;  // 0:无效，1:有效
};

/*****************************************************************************
 Description  : 设置log level
 command      : toolname cx set loglevel val={fatal|error|warn|print|info|debug}
 Direction    : ADM_RAID_SET SUBOPCODE:ADM_CMD_SET_LOG_LEVEL
*****************************************************************************/
struct cmd_set_log_level {
    u16 log_level;
    u16 rsv;
};

/*****************************************************************************
 Description  : 获取log level
 command      : toolname cx show loglevel
 Direction    : ADM_RAID_READ SUBOPCODE:ADM_CMD_SHOW_LOG_LEVEL
*****************************************************************************/
struct cmd_get_log_level {
    u16 log_level;
    u16 rsv;
};

/*****************************************************************************
 Description  : 获取raid卡日志信息
 command      : toolname cx get log path=xxx
 Direction    : SUBOPCODE:ADM_CMD_GET_LOG
*****************************************************************************/
enum log_file_type {
    AP_LOG_TYPE,
    IMU_LOG_TYPE,
    AP_LOG_INDEX,
    IMU_LOG_INDEX,
    AP_LASTWORD_TYPE,
    LOG_BUFF_TYPE
};

struct cmd_log_info_req {
    u8 collect_type;
    u8 flag;  // 当前分片类型(前3位分别表示起始bit0、中部bit1、尾部bit2、单片能传完起始和尾部置位)
    u16 req_seq;
    u32 req_offset;
    u32 req_len;
};

struct log_rsp_head {
    u32 total_size;
    u32 max_pay_load;  // 单次支持最大返回的数据长度
    u8 collect_type;
    u8 rsv;
    u16 seq;
    u32 rsp_len;
    u32 data_slice_crc;
};

#define CHIP_MAX_PAY_LOAD_CAP 64 * 1024
struct cmd_log_info_data {
    struct log_rsp_head rsp_head;
    u8 data[CHIP_MAX_PAY_LOAD_CAP];  // 占位并不实际申请内存
};

#define LOG_MGT_MAGIC   0x1b1c1d1e  // imu，ap，apkey日志的magic
#define FLASH_CNT_VALID 0x12345678
#define LOG_TAG_ID      0x7fff

enum LOG_TAG_TYPE {
    LOG_MAGIC_TAG,
    LOG_VER_TAG
};

typedef struct {
    u32 tag_type;
    u32 area;
    u32 magic;
    u32 rsv;
} magic_tag;  // user_val

typedef struct {
    u32 tag_type;
    u32 area;
    u16 ver[4];
} version_tag;  // user_val

/*****************************************************************************
 Description  :
 command      : toolname cx set mbist [flag=xx]
                toolname cx show mbist
 Direction    : SUBOPCODE:ADM_CMD_SET_MBIST
                SUBOPCODE:ADM_CMD_SHOW_MBIST
*****************************************************************************/
#define MBIST_REQ_MAGIC     0x5a5a5a5a
struct cmd_mbist_request {
    u8 verify_flag;
    u8 rsved[3];
};

struct cmd_mbist_status {
    u8 fail_count;
    u8 pass_count;
    u8 rsved[2];
};

/*****************************************************************************
 Description  :
 command      : toolname cx show ddr_eye_diagram
 Direction    : SUBOPCODE:ADM_CMD_SHOW_DDR_EYE
*****************************************************************************/
#define DDR_CHANNEL_NUM 2
#define DDR_BIT_NUM 72
struct ddr_eye_bit {
    short RxDly[2];
    short RxVrf[2];
    short TxDly[2];
    short TxVrf[2];
};

struct cmd_ddr_eye_info {
    struct ddr_eye_bit ddr_eye_bit[DDR_CHANNEL_NUM][DDR_BIT_NUM];  // DDR两个channel, 各72bit
};

/*****************************************************************************
 Description  :
 command      : toolname cx set loopback sw={on|off} phyid={0-39} rate=[1.5|3|6|12]
 codetype=[0|1|2|3|4|5|6|7|8|9|10] mode=[0|1]
 Direction    : SUBOPCODE:ADM_CMD_SET_DISK_LOOPBACK
*****************************************************************************/

enum loop_back_mode {
    LOOP_BACK_MODE_IN = 0,
    LOOP_BACK_MODE_OUT,
};

enum loop_back_type {
    LOOP_BACK_TYPE_PRBS7 = 0,
    LOOP_BACK_TYPE_PRBS23,
    LOOP_BACK_TYPE_PRBS31,
    LOOP_BACK_TYPE_JTPAT,
    LOOP_BACK_TYPE_CJTPAT,
    LOOP_BACK_TYPE_SCRAMBED_0,
    LOOP_BACK_TYPE_TRAIN,
    LOOP_BACK_TYPE_TRAIN_DONE,
    LOOP_BACK_TYPE_HFTP,
    LOOP_BACK_TYPE_MFTP,
    LOOP_BACK_TYPE_LFTP,
};

enum loop_back_rate {
    LOOP_BACK_RATE_1P5G = 8,
    LOOP_BACK_RATE_3G,
    LOOP_BACK_RATE_6G,
    LOOP_BACK_RATE_12G,
};

struct cmd_start_disk_loopback {
    u8 sw;  // 0关 1开
    u8 phyid;
    u8 rsv[2];
    u32 rate;       // 速率
    u32 code_type;  // 码型
    u32 mode;       // 0:内环 1 外环
};

struct cmd_loopback_rslt {
    u8 sw;  // 0关 1开
    u8 rsv[3];
    u32 code_type;
    u32 status;
};

/*****************************************************************************
 Description  : SAS DISK Firmware actice
 command      : SAS EXPANDER，PCIE SW固件激活是否需要阻挡IO
 Direction    : opcode:ADM_CMD_ACTIVE_DISK_FW;
*****************************************************************************/
struct adm_disk_fw_active {
    struct multi_disk_location loc;
    u16 io_break;  // 1:不中断IO业务，0:中断IO业务
    u16 rsvd;
};

/*****************************************************************************
 Description  :
 command      : toolname cx:ex:sx set ncq sw={on|off}
 Direction    : subopcode:ADM_CMD_SET_NCQ;
*****************************************************************************/
struct cmd_set_ncq {
    struct multi_disk_location loc;
    u32 sw;  // 0关 1开
};

enum set_slow_disk_type {
    SET_SLOW_DISK_TYPE_RAID,
    SET_SLOW_DISK_TYPE_RAWDRIVE
};
/*****************************************************************************
 Description  :
 command      : toolname cx set slowdrive type={raid|rawdrive} [sw={on|off}] [ratio=x] [media={ssd|hdd}]
                [susize={256|1024}] [l1=x] [l2=x] [l3=x] [default]
 Direction    : subopcode:ADM_CMD_SET_SLOWDISK;
*****************************************************************************/
struct cmd_set_slow_disk {
    u8 sw;           // 0 关 1开 2default
    u8 type;         // enum set_slow_disk_type
    u16 su_size;     // ADM_SUSZ_256, ADM_SUSZ_1024, 0 unknown
    u8 ratio;        // 0 unknown
    u8 default_set;  // 0 unknown, 1 set default
    u8 media;        // 0 hdd, 1 ssd, 2 unknown
    u8 rsv[3];
    u16 l1;  // 0 unknown
    u16 l2;  // 0 unknown
    u16 l3;  // 0 unknown
};

struct raid_threshold {
    u8 media;  // 0 hdd, 1 ssd, 2 unknown
    u8 rsv[3];
    u16 su_size;  // ADM_SUSZ_256, ADM_SUSZ_1024, 0 unknown
    u16 l1;       // 0 unknown
    u16 l2;       // 0 unknown
    u16 l3;       // 0 unknown
};

struct rawdrive_threshold {
    u8 media;  // 0 hdd, 1 ssd, 2 unknown
    u8 rsv;
    u16 l2;  // 0 unknown
};

struct slow_disk_config {
    u8 ratio;  // 只存在于raid
    u8 rsv[3];
    struct raid_threshold raid_info[3];
    struct rawdrive_threshold rawdrive_info[2];
};

/*****************************************************************************
 Description  :
 command      : toolname cx show slowdrive
 Direction    : subopcode:ADM_CMD_SHOW_SLOWDISK;
*****************************************************************************/
struct cmd_show_slow_disk {
    u8 raid_sw;  // 0关 1开
    u8 raw_sw;   // 0关 1开
    u8 rsv[2];
    struct slow_disk_config current_config;
    struct slow_disk_config next_config;
};

/*****************************************************************************
 Description  :
 command      : toolname cx set bpselflearn time=xx volta_vc=xx volta_vc_to_vd=xx cap=xx
 Direction    : subopcode:ADM_CMD_SET_SELF_LEARN;
*****************************************************************************/
struct cmd_bp_cap_operation {
    u32 va_time;
    u32 volta_vc;
    u32 volta_vc_to_vd;
    u32 cap_i_ma;
};

struct cmd_bp_cap_learn_time {
    u32 total_time;  // 单位分钟
};

struct cmd_bp_cap_retrive {
    u64 super_cap_esr;  // super_cap_esr/100
    u64 super_cap_cf;   // super_cap_cf/100
    u8 status;          // 学习状态
    u8 rsv;
    u16 cap_voltage;    // 高档位电压值 mV
};

struct cmd_set_backupcap {
    u64 backup_cap_cf;
};

/*****************************************************************************
 Description  :
 command      : toolname cx show smcapv
 Direction    : subopcode:ADM_CMD_SHOW_SM_CAP_V;
*****************************************************************************/
struct cmd_bp_samll_cap_volta {
    u32 small_cap_volta; //   small_cap_volta/1000 V
};

struct cmd_map_ppn_rslt {
    u8 ch[2];
    u8 ce[2];
    u8 die[2];
    u8 plane[2];
    u16 page[2];
    u16 block[2];
};

/*****************************************************************************
 Description  : dump所有寄存器
 command      : toolname cx show allreg type={sds|fe|bi|cache|raid|all}
                [macroid=0-7|0xff] outfile=xx
 Direction    : subopcode:ADM_CMD_SHOW_ALLREG;
*****************************************************************************/
enum {
    GET_DUMP_SUB_TYPE,       // 根据主type获取支持子type
    GET_SUB_TYPE_ADDR_INFO,  // 返回地址 num、以及title
    GET_REG_DATA_BY_ADDR,    // 根据地址的长度分批次获取
};

enum reg_dump_type {
    DUMP_SDS_REG,
    DUMP_FE_REG,
    DUMP_BE_REG,
    DUMP_CACHE_REG,
    DUMP_RAID_REG,
    DUMP_SOC_REG,
    DUMP_BUTT
};

typedef struct {
    u8 macro_id;  // SDS专用0-7 代表macroid 0xff代表全部 macroid
    u8 rsv[3];
} sds_type_req;

typedef struct {
    u8 rsv[4];
} extra_type_req;

typedef struct {
    u64 reg_start_addr;
    u32 reg_num;
} reg_req_info;

#pragma pack(4)
struct cmd_reg_dump_param {
    u8 op_type;
    u8 rsv1;
    u16 reg_type;
    union {
        extra_type_req extra_req;
        struct {
            u16 sub_type;
        } sub_addr_req;
        struct {
            reg_req_info reg_req;
            u16 sub_type;
        } addr_data_req;
        u64 data[2];
    };
};
#pragma pack()

#define MAX_DUMP_SUB_TYPE_NUM 512  // 根据需要扩大
struct dump_sub_type_info {
    u16 sub_type_num;
    u16 dump_sub_type[MAX_DUMP_SUB_TYPE_NUM];
};

struct dump_sub_type_addr_info {
    u8 sub_title[32];
    reg_req_info reg_req;
};

/*****************************************************************************
 Description  : 设置硬盘的省电模式
 command      : toolname cx set powersave sw={on|off} [time=xx]
 Direction    : subopcode:ADM_CMD_SET_POWERSAVE;
*****************************************************************************/
struct cmd_set_powersave {
    u32 time;       // [30,120] 分钟
    u8 sw;          // 0关 1开
    u8 rsvd[3];
};

/*****************************************************************************
 Description  :
 command      : toolname cx:ex:sx set powersave sw={on|off} [pc=xx] [pcmod=xx]
 Direction    : subopcode:ADM_CMD_SET_DISK_POWERSAVE;
*****************************************************************************/
struct cmd_set_power_save {
    struct multi_disk_location loc;
    u32 sw;      // 0关 1开
    u32 pc;      // 2          3
    u32 pc_mod;  // 0 1 2      1
};

/*****************************************************************************
 Description  : 查询超级电容自学习信息
 command      : toolname cx:sc show selflearn
 Direction    : subopcode:ADM_CMD_SHOW_SELFLEARN;
*****************************************************************************/
struct cmd_bp_selfleran_rslt {
    u8 present;           // 在位状态(0-不在位，1-在位)
    u8 last_learn_status; /* 上次自学习完成状态 自学习状态:
                        0: 正在自学习（bit位置1有效）
                        1: 自学习成功（bit位置1有效）
                        2: 自学习失败（bit位置1有效）
                        3: 自学习超时（bit位置1有效）
                        全0:没有自学习
                    */
    u8 rsv[2];
    u32 learn_cycle;      // relearn周期，单位天
    u64 last_learn_time;  // 上次自学习时间
    u64 next_learn_time;  // 下次relearn开始时间，秒级时间戳
};

/*****************************************************************************
 Description  : 设置超级电容自学习周期
 command      : toolname cx:sc set learn cycle=xx starttime=yyyy-MM-dd,hh:mm:ss
 Direction    : subopcode:ADM_CMD_SET_SELFLEARN_CYCLE;
*****************************************************************************/
struct cmd_bp_selfleran_cycle {
    u32 learn_cycle;      // relearn周期，单位天
    u32 next_learn_time;       // 单位:秒
};

/*****************************************************************************
 Description  : 设置备电模块温度门限
 command      : toolname cx:sc set alarm temp=xx
 Direction    : subopcode:ADM_CMD_SET_ALARMTEMP;
*****************************************************************************/
struct cmd_set_bp_alarmtemp {
    int temperature;
};

/*****************************************************************************
 Description  : 设置UEFI向BIOS最大上报磁盘数量
 command      :
 Direction    : subopcode:ADM_CMD_SET_REPORT_DRIVE_NUM;
*****************************************************************************/
struct cmd_set_report_drive_num {
    u8 max_drive_for_uefi;
    u8 rsvd[3];
};

typedef union {
    /* Define the struct bits */
    struct {
        /* DWORD0 */
        unsigned int event_type      : 3;   /* [0..2] 自定义消息 */
        unsigned int rsv             : 5;   /* [3..7] 预留 */
        unsigned int event_code      : 8;   /* [8..15] 事件码 */
        unsigned int event_location  : 4;   /* [16..19] 事件位置 */
        unsigned int event_level     : 4;   /* [20..23] 事件级别 */
        unsigned int parame0         : 8;   /* [24..31] 参数0 */
        /* DWORD1 */
        unsigned int parame1         : 32;  /* 参数1 */
    } bits;

    unsigned long long result;
} event_report_info;

#define EVENT_VENDOR_SPECIFIC    0x7

enum event_code {
    FLASH_ERR = 139,
    CPU_OVERTEMP = 140,
    CPU_OVERTEMP_ALARM,
    CPU_TEMP_REVER,
    SDS_DOWN_REFCLK_WARN = 143,
    SDS_DOWN_REFCLK_RECOVER = 144,
    PCIE_CTRL_WARN = 145,
    PCIE_CTRL_RECOVER = 146
};

enum event_location {
    EVENT_CONTROLLER = 0,
    EVENT_PD,
    EVENT_VD,
    EVENT_RD,
    EVENT_ENCL,
    EVENT_SC
};

enum event_level {
    EVENT_FATAL = 0,
    EVENT_CRITICAL,
    EVENT_WARNING,
    EVENT_INFO,
};

enum file_type {
    DUMP_FILE_TYPE,      // cx get dump path=xx [type=0] 从norflash取
    DUMP_RAM_FILE_TYPE,  // cx get dump path=xx [type=1] 从ram取，如果不支持ram，则从norflash取
    NAND_FILE_TYPE       // cx get nandlog path=xx [id=x]
};

/*****************************************************************************
 Description  : 收集dump命令
 command      : toolname cx get dump path=xx
 Direction    : SUBOPCODE:ADM_CMD_GET_DUMP
*****************************************************************************/
#define REG_DUMP_MAGIC  0x5b5c5d5e
#define DATA_DUMP_MAGIC 0x5e5f5051
#define RAS_DUMP_TYPE 0x1
#define REG_DUMP_TYPE 0x2
struct reg_dump_header {
    u32 magic;
    u8 type;
    u8 rsv1[3];
    u64 time_ms;
};

/*****************************************************************************
 Description  : nand老化测试
 command      : toolname cx set retention turn=x
 Direction    : SUBOPCODE:ADM_CMD_SET_RETENTION
******************************************************************************/
typedef enum {
    NO_CAUSE_LOG_T = 0,
    CAUSE_UNC_T,
    CAUSE_PSF_T,
    CAUSE_ESF_T,
    CAUSE_PSF_UNC_T,
    RD_RISK_FLAG_T,
    HIGH_ERROR_BIT_T,
    ERASE_TIMEOUT_T,
    PROG_TIMEOUT_T,
    OTHER_CAUSE_T
} GBB_CAUSE_T;

struct bp_gbb_log {
    u8 turn;
    u8 ch;
    u8 ce;
    u8 lun;
    u8 plane;
    u8  rsv;
    u16 block;
    u16 page;
    u16 temperature;
    GBB_CAUSE_T cause;
};

struct bp_retention_info {
    u8 success;
    u8 rsv[3];
    u32 data_len;
};

struct bp_retention_req {
    u8 idx;
    u8 rsv[3];
    u32 data_len;
};

/*****************************************************************************
 Description  : 查询存在pinnedcache的vd
 command      : toolname cx show pinnedcache
 Direction    : SUBOPCODE:ADM_CMD_SHOW_PINNEDCACHE_LIST
*****************************************************************************/

struct cmd_pinnedcache_info {
    u8 vdid;
    u8 status;
};

#define PINNEDCACHE_LIST_MAX 127
struct cmd_pinnedcache_list {
    u32 cnt;
    struct cmd_pinnedcache_info info[PINNEDCACHE_LIST_MAX];
};

/*****************************************************************************
 Description  : 设置踢盘周期
 command      : toolname cx set kickdrive [period=x(1-90)day] [sum=x(1-8)]
 Direction    : SUBOPCODE:ADM_CMD_SET_KICK_DISK_CYCLE
*****************************************************************************/
struct cmd_set_kickdisk {
    u8 period;   // MASK_OF_STRUCT_INDEX_FIRST
    u8 sum;      // MASK_OF_STRUCT_INDEX_SECOND
    u8 rsvd;
    u8 mask;
    u32 rsvd1[3];
};

/**
 * @brief 查询当前慢盘踢盘状况
 * @command toolname cx show kickdrive
 * @subopcode ADM_CMD_SHOW_KICK_DRIVE_STATUS
 */
struct cmd_show_kickdrive {
    u8 period; /* 当前慢盘踢盘重置周期 */
    u8 sum; /* 当前周期慢盘踢盘允许总数 */
    u8 kick_sum; /* 当期已踢盘总数 */
    u8 rsvd[53];
    u64 reset_remaining_time; /* 慢盘踢盘总数重置剩余时间，单位：min */
};

/*****************************************************************************
 Description  : 用户恢复卡异常的操作
 command      :
 Direction    : SUBOPCODE:ADM_CMD_USER_RECOVERY ADM_CMD_USER_RECOVERY_PROCESS
*****************************************************************************/
enum {
    RECOVER_WRITE_HOLE = 1,         // WRITE_HOLE_CLEAR
    RECOVER_PINNED_CACHE = 2,       // PINNED_CACHE_CLEAR
    RECOVER_BAD_CARD = 3,           // NONE
    RECOVER_RESTORE_DATA = 4,       // RESTORE_DATA | DROP_DATA
    RECOVER_DROP_DATA = 5,          // DROP_DATA
    RECOVER_RESETTING_REQUIRED = 6, // RESETTING_REQUIRED
    MAX_RECOVERY_TYPE
};

enum {
    WRITE_HOLE_CLEAR = 1,
    PINNED_CACHE_CLEAR = 2,
    RESTORE_DATA = 3,
    DROP_DATA = 4,
};

struct cmd_user_recovery {
    u16 opcode;
    u16 operation;
    u8 param[20];
};

struct cmd_user_recovery_default {
    u32 default_on;
};

/*****************************************************************************
 Description  : 配置裸盘的流控门限（fc -> flow control)
 command      : toolname cx set drivefc mode={high|low} [media=hdd|ssd] 默认hdd
                toolname cx set vdfc mode={high|low|medium}
 Direction    : SUBOPCODE:ADM_CMD_SET_DRIVER_FC, ADM_CMD_SET_VD_FC
*****************************************************************************/
enum drivefc_mode {
    DRIVEFC_LOW = 0,
    DRIVEFC_HIGH = 1,
    DRIVEFC_MEDIUM = 2,
    DRIVEFC_MAX
};
struct cmd_set_drive_fc {
    u8 drivefc_mode;
    u8 media;  // 0 hdd, 1 ssd
    u8 rsv[2];
};
struct cmd_drive_fc_cfg {
    u32 current_mode;
    u32 next_mode;
    u32 rsv[6];
};

/*****************************************************************************
 Description  : 查询裸盘的流控门限（fc -> flow control) 新增查询信息
 command      : toolname cx show drivefc
 Direction    : SUBOPCODE:ADM_CMD_SHOW_DRIVER_FC_EXPAND
*****************************************************************************/
struct cmd_show_drive_fc_rslt {
    struct cmd_drive_fc_cfg config[8];  // 0 hdd, 1 ssd 其他预留
};

/*****************************************************************************
 Description  : 查询vd的流控门限（fc -> flow control) 新增查询信息
 command      : toolname cx show vdfc
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_FC
*****************************************************************************/
struct cmd_show_vd_fc_cfg {
    struct cmd_drive_fc_cfg config[8];  // 0 vd 其他预留
};

/*****************************************************************************
 Description  : 配置收集nandlog的索引
 command      :
 Direction    : SUBOPCODE:ADM_CMD_SET_NAND_LOG_IDX
*****************************************************************************/
struct cmd_set_nand_log_idx {
    u8 idx;  // 0xFF为全部收集
    u8 rsv[3];
};

enum area_type {
    APLOG_ATTR,
    APKEYLOG_ATTR,
    IMULOG_ATTR,
    DUMP_ATTR,
    LASTWORD_ATTR,
    AREA_ATTR_NUM
};
struct nandlog_desc {
    u8 area_type;  // enum area_type
    u8 rsv[3];
    u32 area_len;  // 区域的长度
};
#define NANDLOG_DESC_MAX_LEN 7
#define NANDLOG_MAGIC        0x5A5B5C5D
struct nandlog_bin_header {
    u32 nand_log_magic;  // NANDLOG_MAGIC
    u8 nand_log_cnt;     // 一个nandlog.bin的nandlog的数量
    u8 rsv[2];
    u8 desc_valid_num;  // 每一份nandlog内容的种类
    struct nandlog_desc desc[NANDLOG_DESC_MAX_LEN];
};

#define OPER_FLASH_LOG_MAGIC 0x1b1c1d1e
typedef struct {
    u32 magic;  // OPER_FLASH_LOG_MAGIC
    u32 rsv[7];
} flash_log_header;

/*****************************************************************************
 Description  : 设置芯片在SAS链路上请求分发的模式
 command      : toolname cx set dphstartmod  [sas0={0|1}]  [sas1={0|1}] [sas2={0|1}]  [sas3={0|1} ] [sas4={0|1}]
 Direction    : SUBOPCODE:ADM_CMD_SET_DPHSTARTMOD
*****************************************************************************/
#define SAS_IP_MAX 5
struct cmd_set_dph_mod_para {
    u8 sas_dph_mod[SAS_IP_MAX];
    u8 mask;  // bit0-bit4表示用户配置sas0-sas4
    u8 rsv0[2];
};

/*****************************************************************************
 Description  : 查询芯片在SAS链路上请求分发的模式
 command      : toolname cx show dphstartmod
 Direction    : SUBOPCODE:ADM_CMD_SHOW_DPHSTARTMOD
*****************************************************************************/
struct cmd_show_dph_mod_rslt {
    u8 cur_sas_ip_num;            // 当前有效sas ip数量
    u8 rsv0[3];
    u8 cur_dph_mod[SAS_IP_MAX];   // 当前配置
    u8 rsv1[3];
    u8 next_dph_mod[SAS_IP_MAX];  // 下次生效配置
    u8 rsv2[3];
};

struct cmd_rg_vd_pdcache_param {
    struct cmd_member_id loc;
    u8 pdcache; // 值定义见enum adm_disk_cahe_switch
    u8 rsvd[3];
};

/*****************************************************************************
 Description  : 设置dha开关
 command      : toolname cx set dha sw={on|off}
 Direction    : SUBOPCODE:ADM_CMD_SET_DHA_SW
*****************************************************************************/
struct cmd_set_dha_sw {
    u8 dha_sw;  // 0:close 1:open
    u8 rsv[3];
};

/*****************************************************************************
 Description  : 设置quedepth
 command      : toolname cx set quedepth type={rawdrive|raid} [sashdd=xx] [sasssd=xx] [satahdd=xx] [satassd=xx]
 Direction    : SUBOPCODE:ADM_CMD_SET_QUEUE_DEPTH
*****************************************************************************/
enum que_depth_type {
    QUE_DEPTH_TYPE_RAID,
    QUE_DEPTH_TYPE_RAWDRIVE,
    QUE_DEPTH_TYPE_BUTT
};

struct cmd_que_depth_cfg {
    u8 sas_hdd;
    u8 sas_ssd;
    u8 sata_hdd;
    u8 sata_ssd;
    u8 rsv[4];
};

struct cmd_set_que_depth {
    u8 type;  // enum que_depth_type
    u8 bitmask;  // 支持用户同时设置一个或多个参数，对应bit为1表示用户设置，为0表示用户未设置
                 // bit0: sas_hdd, bit1: sas_ssd, bit2: sata_hdd, bit3: sata_ssd
    u8 rsv[2];
    struct cmd_que_depth_cfg set_cfg;
};

/*****************************************************************************
 Description  : 查询quedepth
 command      : toolname cx show quedepth
 Direction    : SUBOPCODE:ADM_CMD_SHOW_QUEUE_DEPTH
*****************************************************************************/
struct cmd_show_que_depth {
    struct cmd_que_depth_cfg current_cfg[QUE_DEPTH_TYPE_BUTT];
    struct cmd_que_depth_cfg next_cfg[QUE_DEPTH_TYPE_BUTT];
    u32 rsv[16];  // 预留
};

/*****************************************************************************
 Description  : 配置io合并参数
 command      : toolname cx set merge [sw=on|off] [timeout=default|x] [count=default|x]
 Direction    : SUBOPCODE:ADM_CMD_SET_MERGE
*****************************************************************************/
struct cmd_set_merge_cfg {
    u8 sw;  // 0:off 1:on
    u8 rsv[7];
    u32 timeout;  // U32_MAX:default  单位：us
    u32 count;    // U32_MAX:default
    u32 bitmask;  // bit0:sw, bit1:timeout, bit2:count
};

/*****************************************************************************
 Description  : 查询io合并参数
 command      : toolname cx show merge
 Direction    : SUBOPCODE:ADM_CMD_SHOW_MERGE
*****************************************************************************/
struct cmd_show_merge_cfg {
    u8 sw;  // 0:off 1:on
    u8 rsv[7];
    u32 timeout;
    u32 count;
    u32 rsv2[4];
};

/*****************************************************************************
 Description  : 配置vd io合并参数
 command      : toolname cx set vdmerge vd=x [stripe_len=default|x] [start_len=default|x] [merge_len=default|x]
 Direction    : SUBOPCODE:ADM_CMD_SET_VD_MERGE
*****************************************************************************/
struct cmd_vd_merge_cfg {
    u8 vdid;
    u8 rsv[3];
    u32 stripe_len;  // U32_MAX:default 单位：4kb
    u32 start_len;   // U32_MAX:default 单位：4kb
    u32 merge_len;   // U32_MAX:default 单位：4kb
};

struct cmd_set_vd_merge_cfg {
    u8 bitmask;  // bit0:stripe_len ,bit1: start_len, bit2:merge_len
    u8 rsv[3];
    struct cmd_vd_merge_cfg set_cfg;
};

/*****************************************************************************
 Description  : 查询vd io合并参数
 command      : toolname cx show vdmerge [vd=x|all]
 Direction    : SUBOPCODE:ADM_CMD_SHOW_VD_MERGE
*****************************************************************************/
struct cmd_show_vd_merge_param {
    u8 vdid;  // U8_MAX:all
    u8 rsv[3];
};

#define VD_MAX_NUM 127
struct cmd_show_vd_merge_cfg {
    u8 vd_num;  // 查询到的vd数量
    u8 rsv[3];
    struct cmd_vd_merge_cfg show_cfg[VD_MAX_NUM];
};

/*****************************************************************************
 Description  : set tsp freq
 command      : toolname cx set tspfreq mode={low|high}
 Direction    : SUBOPCODE:ADM_CMD_SET_TSP_FREQ
*****************************************************************************/
enum tsp_freq_mode {
    TSP_FREQ_LOW = 0,  /* 426MHz */
    TSP_FREQ_HIGH = 1, /* 800MHz */
    TSP_FREQ_BUTT
};

struct cmd_set_tsp {
    u32 freq_mode;
};

/*****************************************************************************
 Description  : show tsp freq
 command      : toolname cx show tspfreq
 Direction    : SUBOPCODE:ADM_CMD_SHOW_TSP_FREQ
*****************************************************************************/
struct cmd_show_tsp {
    u32 cur_freq_mode;
    u32 next_freq_mode;
};

#pragma pack(pop)
#endif  // INCLUDE_OPEN_API_LIB_API_H
