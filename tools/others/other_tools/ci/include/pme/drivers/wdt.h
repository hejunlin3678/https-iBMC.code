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

#ifndef _WDT_H_
#define _WDT_H_

#include "glib.h"

#define WDT_MODE_USER           0
#define WDT_MODE_KERNEL         1

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32  wdt_mode(guint8 mode);
gint32  stop_wdt(void);
gint32  clear_wdt(void);
gint32 abnormal_reset(void);
gint32 wait_abnormal_reset(void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
