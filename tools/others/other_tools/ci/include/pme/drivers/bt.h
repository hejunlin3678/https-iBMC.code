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

#ifndef _BT_H_
#define _BT_H_

#include "glib.h"

/*****************************************************************************
对外提供的I2C参数宏定义
 ******************************************************************************/
#define MAX_BT_MSG_SIZE (248 + 4)

/*****************************************************************************
数据结构定义
*****************************************************************************/
#pragma pack(1)
    typedef struct  tag_bt_buffer_info_s
    {
        guint32 timeout;        /* 超时时间，范围[0-65535]，单位jiffies */
        gchar*  buffer;
        guint32 len;            /* 范围[0-MAX_BT_MSG_SIZE] */
        gint32  reserved;
    } BT_BUFFER_INFO_S;
#pragma pack()


/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32  bt_init(void);
void bt_clean(void);
gint32  bt_read(BT_BUFFER_INFO_S* info);
gint32  bt_write(BT_BUFFER_INFO_S* info);
gint32  bt_setatn(gint32 enable);
gint32 cpld_bt_init(void);
void cpld_bt_clean(void);
gint32 cpld_bt_read(BT_BUFFER_INFO_S* info);
gint32 cpld_bt_write(BT_BUFFER_INFO_S* info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
