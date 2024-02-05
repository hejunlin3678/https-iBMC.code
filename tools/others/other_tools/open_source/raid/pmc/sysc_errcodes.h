/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: 错误码定义
 * Author: huangleisheng 00475287
 * Create: 2021年3月1日
 */

#ifndef _SC_ERRCODES_H_
#define _SC_ERRCODES_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SC_SUCCESS               0
#define SC_FAILED                0xFFFFFFFF   // 内部错误，用户不感知。正常情况下不会发生，如有必要再新增具体的错误码供调用者处理。

#define SC_ERR_CODE_BASE              0x8000
#define SC_CTRL_ERR_CODE_BASE         (SC_ERR_CODE_BASE + 0x100)
#define SC_LD_ERR_CODE_BASE           (SC_ERR_CODE_BASE + 0x200)
#define SC_PD_ERR_CODE_BASE           (SC_ERR_CODE_BASE + 0x300)
#define SC_CONFIG_ERR_CODE_BASE       (SC_ERR_CODE_BASE + 0x400)
#define SC_ARRAY_ERR_CODE_BASE        (SC_ERR_CODE_BASE + 0x500)

/* 系统级 */
#define SC_ERR_INVALID_CMD                 (SC_ERR_CODE_BASE + 0x01)
#define SC_ERR_INVALID_CMD_TYPE            (SC_ERR_CODE_BASE + 0x02)
#define SC_ERR_NULL_DATA                   (SC_ERR_CODE_BASE + 0x03)
#define SC_ERR_CDA_NOT_INIT                (SC_ERR_CODE_BASE + 0x04)
#define SC_ERR_CANNOT_ALLOC_MEM            (SC_ERR_CODE_BASE + 0x05)
#define SC_ERR_DATA_INVALID                (SC_ERR_CODE_BASE + 0x06)

/* 控制器 */
#define SC_ERR_INVALID_CTRL_INDEX          (SC_CTRL_ERR_CODE_BASE + 0x01)   // 控制器下标无效
#define SC_ERR_CTRL_STATUS_NOT_OK          (SC_CTRL_ERR_CODE_BASE + 0x02)   // 控制器状态不正常
#define SC_ERR_CTRL_OPER_NOT_SUPPORTED     (SC_CTRL_ERR_CODE_BASE + 0x03)   // 控制器操作不支持
#define SC_ERR_CTRL_BBU_STATUS_ABNORMAL    (SC_CTRL_ERR_CODE_BASE + 0x04)   // 控制器的BBU状态不正常
#define SC_ERR_CTRL_NO_EDITABLE_LD         (SC_CTRL_ERR_CODE_BASE + 0x05)   // 控制器没有可配置的逻辑盘
#define SC_ERR_CTRL_RCP_NOT_IN_RANGE       (SC_CTRL_ERR_CODE_BASE + 0x06)   // 控制器读缓存百分比不在有效范围

/* 逻辑盘 */
#define SC_ERR_LD_INVALID                  (SC_LD_ERR_CODE_BASE + 0x01)   // 逻辑盘无效
#define SC_ERR_LD_OPERATION_NOT_SUPPORT    (SC_LD_ERR_CODE_BASE + 0x02)   // 逻辑盘操作不支持
#define SC_ERR_LD_INIT_IN_PROGRESS         (SC_LD_ERR_CODE_BASE + 0x03)   // 逻辑盘正在初始化
#define SC_ERR_LD_PROPERTY_SET_NOT_ALLOWED (SC_LD_ERR_CODE_BASE + 0x04)   // maxCache操作不支持
#define SC_ERR_LD_SIZE_SHRINK_NOT_ALLOWED  (SC_LD_ERR_CODE_BASE + 0x05)   // 逻辑盘容量不允许收缩
#define SC_ERR_LD_NOT_SSD                  (SC_LD_ERR_CODE_BASE + 0x06)   // 逻辑盘成员盘非SSD
#define SC_ERR_LD_STATE_UNSUPPORTED_TO_SET (SC_LD_ERR_CODE_BASE + 0x07)   // 当前逻辑盘状态不支持指定的设置操作

/* 物理盘 */
#define SC_ERR_PD_INVALID                  (SC_PD_ERR_CODE_BASE + 0x01)   // 物理盘无效
#define SC_ERR_PD_OPERATION_NOT_SUPPORT    (SC_PD_ERR_CODE_BASE + 0x02)   // 物理盘操作不支持
#define SC_ERR_PD_MAKESPARE_NOT_ALLOWED    (SC_PD_ERR_CODE_BASE + 0x03)   // 物理盘不允许设置热备
#define SC_ERR_PD_SPARE_FOR_RAID0_LD       (SC_PD_ERR_CODE_BASE + 0x04)   // 试图将物理盘设置为RAID0逻辑盘的热备盘
#define SC_ERR_PD_STATE_UNSUPPORTED_TO_SET (SC_PD_ERR_CODE_BASE + 0x05)   // 当前物理盘状态不支持指定的设置操作
#define SC_ERR_PD_SPARE_SDD_HDD_MIXED      (SC_PD_ERR_CODE_BASE + 0x06)   // 作为热备的物理盘与逻辑盘成员盘为不同介质的硬盘(SSD/HDD)

/* 配置相关 */
#define SC_ERR_CONFIG_OPERATION_NOT_SUPPORTED   (SC_CONFIG_ERR_CODE_BASE + 0x01)   // RAID配置操作不支持
#define SC_ERR_MAX_LD_NUM_REACHED               (SC_CONFIG_ERR_CODE_BASE + 0x02)   // 达到了最大支持的逻辑盘个数
#define SC_ERR_DATA_DRIVE_INVALID               (SC_CONFIG_ERR_CODE_BASE + 0x03)   // 数据物理盘无效
#define SC_ERR_DUPLICATE_DRIVES_IN_LIST         (SC_CONFIG_ERR_CODE_BASE + 0x04)   // 列表中有重复物理盘
#define SC_ERR_DRIVE_NOT_UNASSIGNED             (SC_CONFIG_ERR_CODE_BASE + 0x05)   // 硬盘状态不是未分配
#define SC_ERR_DRIVE_TOO_SMALL                  (SC_CONFIG_ERR_CODE_BASE + 0x06)   // 数据物理盘容量太小
#define SC_ERR_DRIVE_TYPE_NOT_MATCH             (SC_CONFIG_ERR_CODE_BASE + 0x07)   // 硬盘类型不匹配
#define SC_ERR_DRIVE_BLOCK_SIZE_INCOMPATIBLE    (SC_CONFIG_ERR_CODE_BASE + 0x08)   // 硬盘块大小不兼容
#define SC_ERR_NO_AVAILABLE_PHYSICAL_SPACE      (SC_CONFIG_ERR_CODE_BASE + 0x09)   // 没有可用的物理空间
#define SC_ERR_INVALID_RAID_LEVEL               (SC_CONFIG_ERR_CODE_BASE + 0x0A)   // 无效的RAID级别
#define SC_ERR_CAPACITY_TOO_SMALL               (SC_CONFIG_ERR_CODE_BASE + 0x0B)   // 容量太小
#define SC_ERR_CAPACITY_TOO_LARGE               (SC_CONFIG_ERR_CODE_BASE + 0x0C)   // 容量太大
#define SC_ERR_CONFIG_PRESENT                   (SC_CONFIG_ERR_CODE_BASE + 0x0D)   // 配置存在
#define SC_ERR_ASSOCIATED_LD_SIZE_OUT_OF_RANGE  (SC_CONFIG_ERR_CODE_BASE + 0x0E)   // 被关联的普通逻辑盘容量超范围

/* 硬盘阵列相关 */
#define SC_ERR_ARRAY_INVALID                   (SC_ARRAY_ERR_CODE_BASE + 0x01)   // 阵列无效

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif