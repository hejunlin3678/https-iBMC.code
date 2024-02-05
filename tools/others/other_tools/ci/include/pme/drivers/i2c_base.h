/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : platform_hi1710.h
  版 本 号   : 初稿
  作    者   : w00206813
  生成日期   : 2013年8月2日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月2日
    作    者   : w00206813
    修改内容   : 创建文件

******************************************************************************/

#ifndef _I2C_H_
#define _I2C_H_

#include "glib.h"

/*****************************************************************************
对外提供的I2C参数宏定义
 ******************************************************************************/
#define I2C_CLK_100K        100
#define I2C_CLK_400K        400
#define I2C_CLK_3400K       3400
#define I2C_SCLHIGH_MAX     65535
#define I2C_SCLHIGH_MIN     6
#define I2C_SCLLOW_MIN      8
#define I2C_CHANEL_0        0
#define I2C_MAX_CHANEL      12

#define I2C_MAX_OFFSET_WIDTH        4
#define I2C_MAX_BYTE_READ_NUM       4
#define I2C_MAX_WRITE_LENGTH    1100

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef struct tag_i2c_init_s
{
    guint32 drv_id;                     /*I2C控制器的编号，取值范围为0~11*/
    guint32 speed;                      /*I2C传输速率设置(100K/400K)?II2C_CLK_100K/I2C_CLK_400K/I2C_CLK_3400K   */
    guint32 is_slave;                   /*是否为从模式(主从只能为一种)*/
    guint32 address;                    /*从模式的地址 */
    guint32 sda_hold_ratio;             /*SDA信号在SCL信号下降沿之后的Hold时间长度占SCL低电平的比率
                                          取值范围(0--100),不赋值默认为0；*/
} I2C_INIT_S;

typedef struct tag_i2c_write_s
{
    guint32 drv_id;               /*I2C控制器的编号，取值范围为0~11*/
    guint8* tx_buffer;            /*写命令 包括 从设备地址 +  偏移地址 +  数据 */
    guint32 length;               /*写命令的长度*/
    guint32 time_out_cnt;         /*写超时时间 */
} I2C_WRITE_S;

typedef struct tag_i2c_read_s
{
    guint32 rv_id;               /*I2C控制器的编号，取值范围为0~11*/
    guint8* tx_buffer;           /*读操作发送的数据 (地址(1byte高7位) + 其他数据),其他数据可以没有*/
    guint32 tx_len;              /*发送命令中数据长度( 地址 + 其他数据 的字节数 )*/
    guint8* read_buf;            /*回传数据buffer*/
    guint32 length;              /*读出数据的长度*/
    guint32 time_out_cnt;        /*读超时时间 ms*/
    guint32 re_read_cnt;         /*重读次数 默认为3，失败时重读三次*/
} I2C_READ_S;

#ifndef __ARM32_HI1710__            /* 1711的接口定义 */
struct i2c_slave_memory_rdwr {
    guint32 drv_id; /* 驱动ID, 与总线ID保持一致 */
    guint32 offset; /* 读写偏移 */
    guint32 size; /* 读写长度 */
    guint32 reserved; /* 保留扩展字段 */
    guint8 *buff; /* 写入或者读出的数据 */
} __attribute__ ((aligned(sizeof(guint32))));
 
struct i2c_depl {
    guint32 drv_id; /* 驱动ID, 与总线ID保持一致 */
    guint32 flags; /* 初始化属性 */
    guint32 sda_hold; /* SDA信号hold延迟周期数 */
    guint32 scl_high; /* SCL 高电平周期数 */
    guint32 scl_low; /* SCL 低电平周期数 */
    guint32 scl_low_timeout; /* SCL 低电平超时周期数 */
    guint32 sda_low_timeout; /* SDL 低电平超时周期数 */
    guint16 slave_address; /* 总线初始化为slave时，控制器的地址 */
    guint16 clk_strech_timeout; /* 保留扩展字段 */
    guint64 slave_dlen;  /* 总线初始化为slave时，控制器使用的内存的长度，单位字节，必须是8的整数倍 */
    guint64 extension; /* 保留扩展字段 */
} __attribute__ ((aligned(sizeof(guint32))));

struct i2c_slave_err_code {
    guint32 drv_id;         /* 驱动ID, 与总线ID保持一致 */
    guint32 size;           /* 错误码总长度 */
    guint32 enable;         /* 是否返回错误码 */
    guint8 *buff;           /* 错误码缓存 */
} __attribute__((aligned(sizeof(guint32))));

typedef struct i2c_slave_memory_rdwr I2C_SLAVE_CACHE_RDWR_S;
typedef struct i2c_slave_err_code I2C_SLAVE_ERR_CODE_S;
typedef struct i2c_depl DEV_I2C_DEPLOY;
 
#endif       /* 1711的接口定义 */

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32 i2c_init(guint8 bus_id, guint8 speed, guint8 mode, guint8 addr);
void i2c_clean(void);
gint32  i2c_read(I2C_READ_S* read_data);
gint32  i2c_write(I2C_WRITE_S* write_data);
gint32  i2c_reset(guint8 bus_id);
gint32 i2c_proxy_access(GSList* cmd_list, GSList** output_list);
#ifndef __ARM32_HI1710__            /* 1711的接口定义 */
gint32 i2c_slave_cache_write(I2C_SLAVE_CACHE_RDWR_S *cache_wr);
gint32 i2c_slave_cache_read(I2C_SLAVE_CACHE_RDWR_S *cache_rd);
gint32 i2c_deploy(DEV_I2C_DEPLOY *deploy);
#endif       /* 1711的接口定义 */
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
