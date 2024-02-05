/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2021. All rights reserved.
 * Description: 部件升级相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __UPGRADE_DEFINE_H__
#define __UPGRADE_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */


/* ********************************
COMP_CONFIG     0x00    0x01
COMP_APP        0x01    0x02
COMP_BIOS       0x02    0x04
COMP_CFG        0x03    0x08
COMP_BOOT       0x04    0x10
COMP_CPLD       0x05    0x20
COMP_CPLD_HDD_BACKPLANE 0x06 0x40
COMP_IPMC       0x07    0x80
********************************* */
#define COMP_CONFIG 0x00             // 升级所需配置文件包support.tar.gz
#define COMP_APP 0x01                // app区升级
#define COMP_BIOS 0x02               // BIOS升级
#define COMP_CFG 0x03                // CFG升级
#define COMP_BOOT 0X04               // uboot升级
#define COMP_CPLD 0x05               // CPLD升级
#define UPGRADE_CPLD_SPACE_TEST_IDEX 0x11       // cpld全资源检测
#define COMP_CPLD_HDD_BACKPLANE 0x06 // 硬盘背板CPLD升级(前置)
#define COMP_IPMC 0x07               // BMC根文件系统升级(cramfs)
#define COMP_2488H_CPU 0x08          // 2488H CPU扣板CPLD
#define COMP_8100_CPU 0x08           // 8100 CPU板
#define COMP_9032_CPU 0x08
#define COMP_8100_FIO 0x09        // 8100 前IO板
#define COMP_9032_FIO 0x09        // 9032前IO板
#define COMP_ATLAS_NPU_BOARD 0x09 // NPU板CPLD升级组件ID
#define COMP_CPLD_NPU_BOARD_IDEX 0x0f // NPU板CPLD升级组件扩展ID
#define COMP_8100_BIOS 0x0a       // 8100 BIOS双镜像升级
#define COMP_9032_BIOS 0x0a
#define COMP_8100_BIO 0x0b             // 8100 后IO板
#define COMP_LCD 0x0c                  // LCD升级
#define COMP_RAID 0x0d                 // RAID升级
#define COMP_FAN 0x0e                  // FAN升级
#define COMP_8100_HDD_BACKPLANE_B 0x0f // 8100 24盘硬盘背板
#define COMP_9032_HDD_BACKPLANE_B 0x0f
#define COMP_RH5885_HDD_BACKPLANE_CD 0x0f // RH5885V3-HV3 新硬盘背板CPLD 0x7d 0x7e
#define COMP_8100_HDD_BACKPLANE_C 0x10
#define COMP_9032_HDD_BACKPLANE_C 0x10
#define COMP_WBD 0x11
#define COMP_9032_BIO 0x12                 // 9032后IO板
#define COMP_RETIMER_FW 0x13                  // 9032 NC板
#define COMP_DFT_MODE 0x14                 // 装备模式标识升级,用于深度恢复出厂配置
#define COMP_8100_HDD_BACKPLANE_D 0x15     // 8100 8盘硬盘背板
#define COMP_CPLD_HDD_BACKPLANE_EXP 0x16   // 扩展的硬盘背板
#define COMP_CPLD_182X_EXT_IDEX 0xf  // 182X延申板CPLD升级扩展ID
#define COMP_POWER 0x17                    // 电源固件升级
#define COMP_CPLD_HDD_BACKPLANE_OTHER 0x18 // 硬盘背板CPLD升级(包含内置和后置)
#define COMP_CPLD_HDD_COMPONENT_IDEX 0xfffffffd
#define COMP_IPMC_EXT4 0x19   // BMC根文件系统升级(ext4fs)
#define COMP_CPLD_BY_I2C 0x1A // RAID卡CPLD升级(通过I2C)
#define COMP_I2C_CPLD 0x1B    // VRD CPLD升级(通过 I2C)
#define COMP_PRR_FW 0x1C      // PRR固件升级
#define COMP_MCU_FW 0x1D      // 板卡MCU固件升级
#define COMP_PUB_FW 0x1E      // 共享FRU升级

#define UPGRADE_COMPONENTIDEX_FRUD 127

// 共用硬盘背板升级流程(COMP_CPLD_HDD_BACKPLANE_OTHER)的部件扩展ID
#define COMP_CPLD_CONVERGE_BOARD_IDEX 0x10 // Atlas 800D G1水平汇聚管理板(XML配置中已使用1,2,3,0xfffffffd, 都是用于不同类型的硬盘背板)

#define UPGRADE_COMPONENTIDEX_CARD_XML 128

#define REMOTE_UPGRADE_URI_MAX_LEN 1024

/* 主备同步升级状态 */
enum {
    BMC_SYNC_STATE_NONE = 0,            // 未进行主备同步
    BMC_SYNC_STATE_UPGRADING,           // 第一个分区升级中
    BMC_SYNC_STATE_UPGRADE_COMPLETE,    // 第一个分区升级成功
    BMC_SYNC_STATE_SYNC,                // 同步另一个分区中
    BMC_SYNC_STATE_SYNC_COMPLETE,       // 同步另一个分区完成
    BMC_SYNC_STATE_COMPLETE,            // 两个分区均升级完成
    BMC_SYNC_STATE_TRANS_TRANSFERRING,  // 文件传输中
    BMC_SYNC_STATE_TRANS_FAILED,        // 文件传输失败
    BMC_SYNC_STATE_UPGRADE_FAILED,      // 第一个分区升级失败
    BMC_SYNC_STATE_SYN_FAILED,          // 另一分区同步失败
    BMC_SYNC_STATE_MAX_VAL,
};

#define BMC_AUTO_VALID 1
#define BMC_MANUAL_VALID 0
#define REBOOT_TO_VALID_BMC 1
#define WAIT_FOR_MANUAL_REBOOT 0
#define NO_RESET_FLAG 0xff

#define BMC_ACTIVE_MODE_RESET 0
#define BMC_ACTIVE_MODE_IMMEDIATELY 1
#define BMC_ACTIVE_MODE_RESET_OS 2
#define BMC_ACTIVE_MODE_RESET_SERVER 3
#define BMC_ACTIVE_MODE_RESET_MM 4

#define BIOS_ACTIVE_MODE_NONE 0
#define BIOS_ACTIVE_MODE_UPGRADE_ONLY 0xff

#define BIOS_ACTIVE_MODE_UPGRADE_ONLY_STR "None"

#define BIOS_COMPONENT_INFO_KEY "ComponentInfo"
#define BIOS_COMPONENT_NAME_KEY "Name"
#define BIOS_COMPONENT_BITMAP_KEY "BitMap"

#define ACTIVE_MODE_STR_LEN    32
typedef struct tagACTIVE_MODE_STR2INT_S {
    gchar active_mode_str[ACTIVE_MODE_STR_LEN];
    guint8 active_mode;
}ACTIVE_MODE_STR2INT_S;

/* 远程升级模式 */
enum {
    REMOTE_UPDATE_MODE_DEFAULT = 0,   // 普通升级模式
    REMOTE_UPDATE_MODE_SYNC           // 主备同步升级
};

/* 生效CPLD的过程中不同操作的返回错误码 */
#define SMM_CPLD_VALID_OK 0x00            // 生效成功
#define SMM_CPLD_VALID_NO_NEED 0x01       // 不需要生效
#define SMM_CPLD_VALID_FAILED 0x02        // 生效失败
#define SMM_CPLD_VALID_REBOOT_FAILED 0x03 // 生效成功，但BMC或EM复位失败

#define CACHE_BIOS_HPM_FILE "/data/upgrade/bios.hpm"
#define CACHE_BIOS_COMP_FILE "/data/upgrade/bios.tar.gz"
#define CACHE_BMC_SYNC_HPM_FILE "/data/upgrade/bmc_sync.hpm"
#define CACHE_UPGRADE_DIR "/data/upgrade"

#define CACHE_UPGRADE_DIR "/data/upgrade"
#define CACHE_CPLD_RESUME_HPM_FILE "/data/upgrade/cpld_resume.hpm"

#ifdef ARM64_HI1711_ENABLED
#define PFR_BIOS_BACKUP_DIR "/data/backup/bios"
#define PFR_BIOS_TMP_DIR "/data/backup/bios/tmp"
#define PFR_BIOS_GOLD_DIR "/data/backup/bios/gold"
#define PFR_BIOS_TMP_FILE "/data/backup/bios/tmp/bios.hpm"
#define PFR_BIOS_GOLD_FILE "/data/backup/bios/gold/bios.hpm"
#define PFR_BIOS_RESUME_FILE  "/opt/pme/conf/biosresume/bios.hpm"
#define PFR_TMP_BIOS_RESUME_FILE  "/tmp/bios_resume.hpm"
#endif


#define UPGRADE_RESUME_BUCKUP_CPLD_DIR "/data/backup/cpld" /* 自愈hpm包临时备份路径 */
#define UPGRADE_RESUME_BUCKUP_TMP_CPLD_DIR "/data/backup/cpld/tmp" /* 自愈hpm包临时备份路径 */
#define UPGRADE_RESUME_BUCKUP_GOLD_CPLD_DIR "/data/backup/cpld/gold" /* 自愈hpm包备份路径 */
#define UPGRADE_RESUME_BUCKUP_TMP_CPLD_FILE "/data/backup/cpld/tmp/cpld_resume.hpm" /* 临时自愈hpm包 */
#define UPGRADE_RESUME_BUCKUP_GOLD_CPLD_FILE "/data/backup/cpld/gold/cpld_resume.hpm" /* 自愈hpm包 */

#define RESUME_FILENAME_MAX_LEN 64
#define CONFIG_FILE_NAME "update.cfg"

/* 单板AC下电再上电触发BIOS生效后的OS上下电策略 */
/* 生效后保持下电 */
#define DONOTHING_AFTER_UP_BIOS 0
/* 生效后执行上电 */
#define POWER_ON_AFTER_UP_BIOS 1
/* 生效后执行通电策略 */
#define PWR_REST_POLICY_AFTER_UP_BIOS 2


/* AC后执行通电策略 */
#define PWR_REST_AFTER_AC 0
/* AC后执行下电 */
#define POWER_OFF_AFTER_AC 1
/* AC后执行上电 */
#define POWER_ON_AFTER_AC 2

/* VRD生效方式 */
#define TYPE_CHIP_RESET_VALIDATE_VRD 0
#define TYPE_AC_POWER_OFF_VALIDATE_VRD 1
#define TYPE_CHIP_RESET_AND_NO_SETFLAG 0xFF

/* VRD成功标志 */
#define VALIDATE_VRD_NONE_SUCCESS 0
#define VALIDATE_VRD_SUCCESS 1

/* powercycle flag */
#define DO_DEFAULT_AFTER_VALIDATE_VRD 0
#define DO_POWER_ON_AFTER_VALIDATE_VRD 1

/* VRD版本变更标志 */
#define VRD_VERSION_DEFAULT 0
#define VRD_VERSION_CHANGED 1

#define VRD_CONFIG_FILE_LEN 128

#define VRD_UPGRADTED 1
#define VRD_UPGRADING 2
#define VRD_WAIT_UPGRADE 3      /* 等待BMC重启后，VRD再升级 */

#define FORCE_UP_BIOS_START 1
#define FORCE_UP_BIOS_FINISH 0

#define TYPE_CPLD_LOAD 0
#define TYPE_CPLD_VALID 1

#define CPLD_NO_IMMED_ACTIVE    0      /* 先load, 后台等待下电再refresh */
#define CPLD_IMMED_ACTIVE       1      /* load，立即refresh */
#define CPLD_ONLY_ACTIVE        2      /* 不load, 仅refresh */

#define UP_DETAIL_RESULT_MAX_LEN 1024

#define UPDATE_MODE_NONE 0
#define UPDATE_MODE_PREPARE 1
#define UPDATE_MODE_PROCESS 2
#define UPDATE_MODE_ACTIVITY 3

/* 常见错误: 2-19 不能随便修改,可以添加,因为涉及到UI显示问题 */
#define UP_COMM_FILE_NOT_EXIST 2                /* 升级文件不存在 */
#define UP_COMM_FILE_ERROR 3                    /* 升级文件错误，如格式错误，校验码错误等 */
#define UP_COMM_LITTLE_MEM 4                    /* 升级所需的内存不足 */
#define UP_COMM_POWER_TATUS_ERR 8               /* 升级时依赖的电源状态错误，比如全区升级ME+BIOS时，电源要下电 */
#define UP_COMM_FIRMWARE_VER_MISMATCH 9         /* 固件版本不匹配（版本不允许回退） */
#define UP_COMM_REMOTE_POWER_TATUS_ERR 11       /* 升级时依赖的远端电源状态错误 */
#define UP_COMM_FIRMWARE_FILE_MISMATCH 12       /* 固件文件不匹配，比如校验BoardID失败 */
#define UP_COMM_FIRMWARE_ERR_WRONG_ME_STATUS 13 /* 错误的ME状态 */
#define UP_COMM_FIRMWARE_UNKNOWN_ERR 14         /* 写固件失败 */
#define UP_COMM_FIRMWARE_ERR_UPGRADE_FAILED 15  /* 不支持升级 */
#define UP_COMM_STANDBY_FORBIDDEN_ERR 16        /* 当前是备板，禁止升级部件 */
#define UP_LOCAL_SUCCESS_EXTRA_ERR 17           /* 本地EM升级成功，extra操作中远程EM升级失败 */
#define UP_EM_EXTRA_CHECK_FAILED 18             /* EM升级前的额外检查失败（升级风扇、LCD等机框部件前检查是否主用板） */
#define UP_SHELF_FAN_ERR 19                     /* EM升级机框风扇失败 */
#define UP_COMM_MAIN_MAX 20                     /* 其它，升级内部处理错误码，上层不需关心，可查看日志具体什么错误 */
#define UP_SHELF_FAN_PARTLY_OK 21               /* 机框风扇部分升级成功（除了不在位和不需要升级的） */
#define UP_PSU_INNER_ERR 22                     /* 升级电源内部错误,上层不需关心，可查看日志具体什么错误 */
#define UP_PSU_UNSUPPORT_ERR 23                 /* 当前电源状态不支持升级 */
#define UP_PSU_NUM_ERR 24                       /* 除升级电源外其他健康电源不能够支撑整机功率 */
#define UP_PSU_HPM_ERR 25                       /* 电源不支持固件升级或电源型号与升级文件不匹配 */
#define UP_PSU_NOT_UP_ERR 26                    /* 电源未升级 */
#define UP_COMM_WHITEBRAND_VER_MISMATCH 27      /* 白牌包和Web版本不匹配 */
#define UP_COMM_FIRMWARE_ERR_RETRY 28           /* 设备上电中，暂时不能执行固件升级 */
#define UP_COMM_FIRMWARE_EARLY_ERR       70     /* 待升级版本过低 */
#define UP_PSU_SECONDARY_FORBIDDEN_ERR   0xfc   /* VSMM当前是从节点，不支持PSU升级 */
#define UP_FAN_SAME_VERSION 0xfd                /* MM920 风扇升级错误 */
#define UP_FAN_NOT_PRESENT 0xfe                 /* MM920 风扇升级错误 */
#define UP_FAN_UNKNOWN_ERR 0xff                 /* MM920 风扇升级错误 */

#define BIT(x) (1 << (x))
#define UP_SUCCESS 0
#define UP_ERR_HW_ACCESS BIT(7)
#define UP_ERR_HW_STOP BIT(6)
#define UP_ERR_INTER BIT(5)

typedef enum tagCARD_MCU_TYPE {
    CARD_MCU_TYPE_GW,
    CARD_MCU_TYPE_HC,
    CARD_MCU_TYPE_SD5000 = 5,
} CARD_MCU_TYPE_E;

enum {
    UPGRADE_COMPLETED = 0x00,
    UPGRADE_IN_PROGRESS = 0x01,
    UPGRADE_FAIL = 0x03,
    UPGRADE_MIDDLE = 0x55,
    UPGRADE_INVALID = 0xFF
};


#define FW_CLASSIFICATIONS_BACKPLANE_CPLD 6
#define FW_CLASSIFICATIONS_CPLD 3
#define FW_CLASSIFICATIONS_BMC 1
#define FW_CLASSIFICATIONS_FAN COMP_FAN /* 此处定义参考代码的引用情况，具体来源无法追溯 */

#define PS_VALID_FORCE_UPGRADE_FLAG 2 // 有条件强制电源升级标志位(4电源规格涉及1+1，2+2场景)
#define PS_FORCE_UPGRADE_FLAG 1
#define PS_NORMAL_UPGRADE_FLAG 0


#define DOWNLOAD_OPTION_BIOS 0x01
#define FORCE_UPGRADE_BIOS 0x01
#define NORMAL_UPGRADE_BIOS 0
#define FORCE_UPGRADE_CPLD 0x01
#define NORMAL_UPGRADE_CPLD 0

#define MAX_FILE_SIZE_1M (1024 * 1024)
#define MAX_FILE_SIZE_2M (2 * 1024 * 1024)
#define MAX_FILE_SIZE_25M (25 * 1024 * 1024)
#define MAX_FILE_SIZE_90M (90 * 1024 * 1024)
#define MAX_FILE_SIZE_2K (2 * 1024)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UPGRADE_DEFINE_H__ */
