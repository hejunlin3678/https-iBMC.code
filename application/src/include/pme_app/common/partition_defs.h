/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 系统硬分区管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __PARTITION_DEFINE_H__
#define __PARTITION_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/* 用于区分9032/9016产品的硬分区模式 */
enum {
    PHYSICAL_PARTITION_MODE_RESOURCE = 0x00,
    PHYSICAL_PARTITION_MODE_4P = 0x01,
    PHYSICAL_PARTITION_MODE_8P = 0x02,
    PHYSICAL_PARTITION_MODE_16P = 0x03,
    PHYSICAL_PARTITION_MODE_24P = 0x04,
    PHYSICAL_PARTITION_MODE_32P = 0x05,
    PHYSICAL_PARTITION_MODE_12P = 0x06,
    PHYSICAL_PARTITION_MODE_20P = 0x07,
    INVALID_PHYSICAL_PARTITION_MODE = 0xff,
};

/* 由于需要支持9008V5同时也要用到  */
#define COMPUTER_ID_9032_V1 0x90320000
#define COMPUTER_ID_9008V5_BASE 0x90080100
#define COMPUTER_ID_9008V5_ADVANCE 0x90080101


/* 硬分区配置情况 */
enum {
    PARTITION_4P = 0,
    PARTITION_8P,
    PARTITION_CENTRALIZE, /* 16P, 32P情况 */
    PARTITION_MAX,
};

#define PARTITON_CODE_BASE 3000
#define PARTITON_CODE_SLAVEBMC_NOT_PRESENT 3001   // 从BMC不在位
#define PARTITON_CODE_BMC_NOT_POWEROFF 3002       // 主、从BMC的系统不是下电的
#define PARTITON_CODE_PARTITION_CONF_SAME 3003    // 要配置分区的和当前的配置相同
#define PARTITON_CODE_BMC_VERSION_NOT_SAME 3004   // 主、从BMC的版本不相同
#define PARTITON_CODE_CPLD_VERSION_NOT_SAME 3005  // 主、从BMC的CPLD版本不相同
#define PARTITON_CODE_SLAVE_BMC_AUTH_FAIL 3006    // 从BMC认证失败
#define PARTITON_CODE_SLAVE_BMC_USER_NOT_PRI 3007 // 从BMC的权限需要管理员或者操作员

/* cpld有没有做过硬分区配置 */
enum {
    UNCONFIGURED = 0,
    CONFIGURED,
    CONFIGURED_MAX,
};


/* 节点模式 */
enum {
    NODE_SLAVE = 0,
    NODE_MASTER,
    NODE_MAX,
};

#define PHYSICAL_PARTITION_MAX_NODE_NUM 8


#define EEPROM_PARTITION_OFFSET 0x3f75
#define EEPROM_PARTITION_SIZE 1 /* 1byte */


#pragma pack(1)

/* 配置硬分区布局信息 */
typedef struct ipmi_physical_partition_map_s {
    guint8 node_num;
    guint8 node_slot[PHYSICAL_PARTITION_MAX_NODE_NUM];
} IPMI_PHYSICAL_PARTITION_MAP_S;

typedef struct partition_get_config_req_s {
    guint32 manu_id : 24;
    guint32 subcmd : 8;
    guint8 parameter;
    guint8 block_selector;
    guint8 offset;
    guint8 length;
} PARTITION_GET_CONFIG_REQ_S;

typedef struct partition_get_config_resq_s {
    guint32 completion_code : 8;
    guint32 manu_id : 24;
    guint8 cascading_flag : 1;
    guint8 reserved : 7;
    guint8 partition;
} PARTITION_GET_CONFIG_RESP_S;

typedef struct partition_get_expander_mode_req_s {
    guint32 manu_id : 24;
    guint32 subcmd : 8;
    guint8 parameter_selector;
    guint8 set_selector;
} PARTITION_GET_EXPANDER_MODE_REQ_S;

typedef struct partition_get_expander_mode_resq_s {
    guint32 completion_code : 8;
    guint32 manu_id : 24;
    guint8 mode;
} PARTITION_GET_EXPANDER_MODE_RESP_S;

typedef struct partition_get_partition_config_req_s {
    guint32 manu_id : 24;
    guint32 subcmd : 8;
    guint8 parameter_selector;
    guint8 set_selector;
} PARTITION_GET_PARTITION_CONFIG_REQ_S;

typedef struct partition_set_expander_mode_req_s {
    guint32 manu_id : 24;
    guint32 subcmd : 8;
    guint8 parameter_selector;
    guint8 set_selector;
    guint8 parameter;
} PARTITION_SET_EXPANDER_MODE_REQ_S;

typedef struct partition_set_expander_mode_resq_s {
    guint32 completion_code : 8;
    guint32 manu_id : 24;
} PARTITION_SET_EXPANDER_MODE_RESP_S;

typedef struct partition_set_partition_config_resq_s {
    guint32 completion_code : 8;
    guint32 manu_id : 24;
} PARTITION_SET_PARTITION_CONFIG_RESP_S;

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __PARTITION_DEFINE_H__ */
