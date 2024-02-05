/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : gpio.h
  版 本 号   : 初稿
  作    者   : h00168999
  生成日期   : 2013年8月2日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月2日
    作    者   : h00168999
    修改内容   : 创建文件

******************************************************************************/

#ifndef _GPIO_BASE_H_
#define _GPIO_BASE_H_

#include "glib.h"

/*****************************************************************************
宏定义
*****************************************************************************/
#define GPIO_MAX_GROUP_NUM        5

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32 gpio_init( guint32 gpio_num, guint8 direction );
gint32  gpio_read(guint8 gpio_num, guint8* gpio_level, guint8 direction);
gint32  gpio_write(guint32 gpio_num, guint8 gpio_level);
gint32 gpio_set_int(guint32 gpio_int_num, guint8 int_level);
gint32 gpio_get_int(guint32 gpio_int_num, guint32 timeout, guint32* value, guint32 rsv);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
