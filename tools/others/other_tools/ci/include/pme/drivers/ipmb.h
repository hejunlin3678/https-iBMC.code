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

#ifndef _IPMB_H_
#define _IPMB_H_

#include "glib.h"

/*****************************************************************************
对外提供的宏定义
 ******************************************************************************/
#define MAX_IPMB_MSG_LEN 255     /* 读写数据的最大长度 */
#define MAX_IPMB_NUM 4           /* IPMB通道数 */

#define IPMB_BUS_BUSY 0X04
#define IPMB_SDAL_ERR 0X80 
#define IPMB_SCKL_ERR 0X40
#define IPMB_SDAH_ERR 0X20
#define IPMB_SCKH_ERR 0X10

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef struct tag_ipmb_init_s
{
    guint32 bus;                 /* IPMB通道号,取值范围：0---3 */
    guint32 speed;               /* IPMB速率，分为100K和400K两种（取值为100、400） */
    guint32 slave_addr;          /* IPMB地址，长度取值范围小于256 */
} IPMB_INIT_S;

typedef struct tag_ipmb_msg_s
{
    guint32 bus;                /* IPMB通道号,取值范围：0---3 */
    guint32 len;                /* 读或者写的长度<=255byte */
    guint32 time_out_count;     /* 读或者写等待的时间，时间单位为毫秒 */
    guint8* buf;                /* 存放读回来的数据或者写数据 */
} IPMB_MSG_S;

typedef struct tag_ipmb_addr_s
{
    guint32 bus;                /* IPMB通道号,取值范围：0---3 */
    guint32 slave_addr;         /* IPMB的从设备地址（BMC地址），地址大小<=127 */
} IPMB_ADDR_S;

typedef struct tag_ipmb_status_s
{
    guint32 bus;                /* IPMB通道号,取值范围：0---3 */
    guint8  status;             /* IPMB的状态，status上的每个位表示IPMB的各自状态 */
} IPMB_STATUS_S;

typedef struct tag_ipmb_check_s
{
    guint32 timeout;
    guint8  mask_read[MAX_IPMB_NUM];
    guint8  data_ready[MAX_IPMB_NUM];
    guint32 reserved;
} IPMB_CHECK_S;

typedef struct tag_ipmb_enable_s
{
    guint32 bus;              /* IPMB通道号,取值范围：0---3 */
    guint32 enable;           /* 0,不使能;1,使能 */
} IPMB_ENABLE_S;


/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */
gint32  ipmb_init(IPMB_INIT_S* init_data, guint8 count);
void ipmb_clean(void);
gint32  ipmb_read(IPMB_MSG_S* msg);
gint32  ipmb_write(IPMB_MSG_S* msg);
gint32 ipmb_reset(guint32 bus);
gint32 ipmb_get_enable(IPMB_ENABLE_S* enable);
gint32 ipmb_set_enable(IPMB_ENABLE_S* enable);
gint32  ipmb_get_addr(IPMB_ADDR_S* addr);
gint32  ipmb_set_addr(IPMB_ADDR_S* addr);
gint32  ipmb_get_status(IPMB_STATUS_S* status);
gint32  ipmb_check_readable(IPMB_CHECK_S* info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
