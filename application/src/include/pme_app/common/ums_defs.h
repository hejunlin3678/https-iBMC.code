/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 板载USB设备管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __UMS_DEFINE_H__
#define __UMS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

// U盘 flash mtd字符串
#ifdef ARM64_HI1711_ENABLED
#define MTD_FLASH_DEV_C "/dev/mmcblk0p2"
#else
#define MTD_FLASH_DEV_C "/dev/mmcblk0p4"
#endif
#define UMS_MAX_CFG_FILE_LEN (2 * 1024 * 1024)

#define UMS_X86_BMC_SHARE 0x1 // 临时挂载， x86与BMC 共享，只在BIOS 未启动完成时可用
#define UMS_X86_MONOPOLY 0x11 // 正式挂载， x86独占挂载

typedef enum _tagUMS_TRANS_ERR_E {
    UMS_TRANS_ERR_FILE_TOO_LARGE = -4,   /* 文件大小超限 */
    UMS_TRANS_ERR_NO_SPACE = -3,         /* 总容量分开 */
    UMS_TRANS_ERR_FILE_CNT_LIMITED = -2, /* 文件个数超50 */
    UMS_TRANS_ERR_UNKOWN = -1,           /* 未知错误 */

} UMS_TRANS_ERR_E;

typedef enum _tagUMS_WR_ERR_E {
    UMS_WR_ERR_BUSY = -30,        /* x86正在读写u盘，BMC不能访问 */
    UMS_WR_ERR_IN_UPGRADING = -29, /* 正在通过BMC升级SP，不能访问U盘 */
    UMS_WR_ERR_BMC_NOT_SUPPORT = -28,   /* BMC 不支持 USB Mass Storage */
    UMS_WR_ERR_CAN_NOT_GET_LOCK = -27    /* 其他任务正在写盘，无法获取锁 */
} UMS_WR_ERR_E;
typedef enum _tagUMS_LOCATION {
    UMS_LOCATION_LOCAL = 0,
    UMS_LOCATION_REMOTE = 1
} UMS_LOCATION;
typedef enum _tagUMS_MOUNT_TYPE {
    UMS_MOUNT_TYPE_TEMPORARY = 0,
    UMS_MOUNT_TYPE_FORMAL = 1,
    UMS_MOUNT_TYPE_SET_SERVICE_FLAG = 255
} UMS_MOUNT_TYPE;
typedef enum _tagUMS_TRANS_FILE_ID_E {
    UMS_TRANS_FILE_ID_SP_RESULT = 0,    /*  */
    UMS_TRANS_FILE_ID_SP_DIALOG = 1,    /*  */
    UMS_TRANS_FILE_ID_SP_OSINSTALL = 2, /* /data/sp/spforbmc/operate/sposinstall/$id.json */
    UMS_TRANS_FILE_ID_SP_FWUPDATE = 3,  /* /data/sp/spforbmc/operate/spfwupdate/xx.xx */
    UMS_TRANS_FILE_ID_SP_NETDEV = 4,    /* /data/sp/spforbmc/operate/ spnetdev/ $id.json */
    UMS_TRANS_FILE_ID_SP_RAID = 5,      /* /data/sp/spforbmc/operate/ spraid/ $id.json */
    UMS_TRANS_FILE_ID_SP_CFG = 6,       /*  */
    UMS_TRANS_FILE_ID_SP_INFO = 7,      /*  */
    UMS_TRANS_FILE_ID_SP_SCHEMA = 8,    /*  */
    UMS_TRANS_FILE_ID_SP_RAID_EXPORT = 9, /* /data/sp/spforbmc/operate/spraid/export/location+devicename_current_cfg.json */
    UMS_TRANS_FILE_ID_SP_DIAGNOSE = 10,
    UMS_TRANS_FILE_ID_SP_DRIVE_ERASE = 11,
    UMS_TRANS_FILE_ID_CNT
} UMS_TRANS_FILE_ID_E;
typedef enum _tagUMS_IBMA_FILE_ID {
    UMS_IBMA_FILE_ID_LINUX = 0,
    UMS_IBMA_FILE_ID_ALL = 1,   
    UMS_IBMA_FILE_ID_CNT
} UMS_IBMA_FILE_ID_E;
// 文件读写 路径白名单
#define UMS_TRANS_PATH_SP_RESULT "/data/sp/spforbmc/operate"
#define UMS_TRANS_PATH_SP_DIALOG "/data/sp/spforbmc/operate/spdialog"
#define UMS_TRANS_PATH_SP_OSINSTALL "/data/sp/spforbmc/operate/sposinstall"
#define UMS_TRANS_PATH_SP_FWUPDATE "/data/sp/spforbmc/operate/spfwupdate"
#define UMS_TRANS_PATH_SP_NETDEV "/data/sp/spforbmc/operate/spnetdev"
#define UMS_TRANS_PATH_SP_RAID "/data/sp/spforbmc/operate/spraid"
#define UMS_TRANS_PATH_SP_CFG "/data/sp/spforbmc/operate/spcfg"
#define UMS_TRANS_PATH_SP_INFO "/data/sp/spforbmc/spinfo"
#define UMS_TRANS_PATH_SP_SCHEMA "/data/sp/spforbmc/spschema"
#define UMS_TRANS_PATH_SP_RAID_EXPORT "/data/sp/spforbmc/operate/spraid/export"
#define UMS_TRANS_PATH_SP_DIAGNOSE "/data/sp/spforbmc/operate/spdiagnose"
#define UMS_TRANS_PATH_SP_DRIVE_ERASE "/data/sp/spforbmc/operate/spdriveerase"

#define UMS_TRANS_PATH_ARR      \
    {                                   \
        UMS_TRANS_PATH_SP_RESULT,       \
		UMS_TRANS_PATH_SP_DIALOG,       \
		UMS_TRANS_PATH_SP_OSINSTALL,    \
		UMS_TRANS_PATH_SP_FWUPDATE,     \
        UMS_TRANS_PATH_SP_NETDEV,       \
		UMS_TRANS_PATH_SP_RAID,         \
		UMS_TRANS_PATH_SP_CFG,          \
		UMS_TRANS_PATH_SP_INFO,         \
        UMS_TRANS_PATH_SP_SCHEMA,       \
		UMS_TRANS_PATH_SP_RAID_EXPORT,  \
		UMS_TRANS_PATH_SP_DIAGNOSE,     \
        UMS_TRANS_PATH_SP_DRIVE_ERASE,  \
    }

#define UMS_FILEPATH "/data/sp" // U盘 文件路径
#define UMS_UPDATEINFO "/data/sp/updateinfo"
#define ISO_UPDATEINFO "/data/sp_update/iso/updateinfo"
#define UMS_SP_FULL_INI_PATH "/data/sp/updateinfo/full.ini"

#define UMS_UP_MODE_FULL "Full"
#define UMS_UP_MODE_RECOVER "Recover"
#define UMS_UP_MODE_APP "APP"
#define UMS_UP_MODE_DRIVER "Driver"
#define UMS_UP_MODE_AUTO "Auto"

#define SP_TASK_DESC_FILE_NAME "sptask.json"  // SP任务描述文件
#define HAVE_NO_SP_TASK "{\"SpTaskExist\": false}"  // 无SP任务时，描述文件初始化内容
#define HAVE_SP_TASK "{\"SpTaskExist\": true}"      // 存在SP任务时，描述文件更新内容

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UMS_DEFINE_H__ */
