/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description: 读写CPLD寄存器接口文件
 * Author: yuanteng 00583122
 * Create: 2019-1-10
 * Notes: 无
 * History: 2019-1-10 yuanteng 00583122 创建文件
 */
#ifndef __DAL_CPLD_H__
#define __DAL_CPLD_H__
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef enum BMC_MUTEX_TYPE {
    BMC_MUTEX_IPC_CPLD_RAM = 0,
    BMC_MUTEX_IPC_DISK_RECV_REG = 1,
    BMC_MUTEX_IPC_DISK_SEND_REG = 2,
    BMC_MUTEX_IPC_SOFTRESET_FLAG = 3,
    BMC_MUTEX_IPC_MCTP_OVER_SMBUS = 4,   /* 网络、cardmanage、mctp模块均会发送命令，增加锁避免冲突 */
    BMC_MUTEX_MAX,        /* 在此之前添加类型 */
    BMC_MUTEX_BUTTY = 200 /* 此枚举最多不要超过200 */
} BMC_MUTEX_TYPE_E;

/* 逻辑访问IIC 相关寄存器 */
typedef enum tagCPLD_REG_E {
    /* 逻辑访问IIC 相关寄存器 */
    CPLD_BMC_RAM_ADDR0            = 0xBA, /* RAM空间访问地址寄存器0 */
    CPLD_BMC_RAM_ADDR1            = 0xBB, /* RAM空间访问地址寄存器1 */
    CPLD_BMC_RAM_DATA             = 0xBC,  /* RAM空间访问数据寄存器 */
    CPLD_IP0_CMD_READY            = 0x200,
    CPLD_IP0_REPAIR               = 0x201,
    CPLD_IP0_CARD_ID              = 0x202,
    CPLD_IP0_FIRST_WR_NUM0        = 0x203,
    CPLD_IP0_FIRST_WR_NUM1        = 0x204,
    CPLD_IP0_SECOND_WR_NUM0       = 0x205,
    CPLD_IP0_READ_NUM             = 0x206,
    CPLD_IP0_STATUS               = 0x207,
    CPLD_IP0_I2C_RAM_ADDR0        = 0x208,
    CPLD_IP0_I2C_RAM_ADDR1        = 0x209,
    CPLD_IP0_I2C_RAM_DATA         = 0x20a,
    CPLD_IP_I2C_TIMEOUT           = 0x241,  /* 通过逻辑访问IIC的超时时间 */
    CPLD_RAM_ADDR_END             = 0xDC00,
}CPLD_REG_SMM_E;

typedef struct devicetype2classname {
    guint8 devicetype;
    gchar *classname;
} DEVICETYPE2CLASSNAME;

#define CPLD_CS_0 0x00    /* CPLD片选0 */
#define CPLD_CS_1 0x01  /* CPLD片选1 */
#define CPLD_BYTE_ACCESS 0x00 /* 按字节来操作CPLD */
#define CPLD_BIT_ACCESS 0x01  /* 按位来操作CPLD */
/* CPLD 读写 MAX 支持 */
#define CPLD_MAX_ADDR (0x3FF)
#define CPLD_IIC_READ_HEADLEN 3
#define CPLD_IIC_READ_BIT     1

#define CPLD_NON_UPGRADING 0        /* cpld不在升级中 */
#define CPLD_IS_UPGRADING  1        /* cpld正在升级中 */
#define CPLD_UPGRADING_UNKNOWN 2    /* cpld升级状态unknown */

typedef enum {
    CPLD_RESULT_OK = 0,
    CPLD_RESULT_NOK = 1,
    CPLD_RESULT_NOT_SUPPORT = 2,
    CPLD_RESULT_TESTING = 3,
} CPLD_SPACE_TEST_RESULT;

gint32 dal_read_cpld_bytes(guint8 chip_id, guint32 offset, guint32 length, guint8 *out);
gint32 dal_write_cpld_bytes(guint8 chip_id, guint32 offset, guint32 length, guint8 *input);
gint32 dal_read_cpld_byte(guint8 chip_id, guint32 offset, guint8* output);
gint32 dal_write_cpld_byte(guint8 chip_id, guint32 offset, guint8 byte);
void dal_set_cpld_ram(guint16 ram_addr, guint8 ram_data);
void dal_get_cpld_ram(guint16 ram_addr, guint8 *ram_data);
gint32 dal_get_cpld_ram_multibytes(guint16 ram_addr, guint32 length, guint8 *ram_data);
gint32 dal_set_cpld_ram_multibytes(guint16 ram_addr, guint32 length, guint8 *ram_data);
gint32 dal_write_cpld_bit(guint8 chip_id, guint32 offset, guint32 bit_num, guint8 bit);
gint32 dal_read_cpld_bit(guint8 chip_id, guint32 offset, guint32 bit_num, guint8 *out);
gint32 dal_process_mutex_sem_P(BMC_MUTEX_TYPE_E members);
gint32 dal_process_mutex_sem_v(BMC_MUTEX_TYPE_E members);
guint8 dal_get_cpld_selftest_status(void);
gint32 dal_get_cpld_upgrading_state(guint8 *state);
guint8 dal_get_cpld_up_used_jtag(void);
gboolean is_cpld_space_testing(void);
gboolean is_cpld_suspend_rw(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DFL_CPLD_H__ */
