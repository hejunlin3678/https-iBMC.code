/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : platform_hi1710.h
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

#ifndef _PWM_H_
#define _PWM_H_

#include "glib.h"

/*****************************************************************************
宏定义
*****************************************************************************/
#define MAX_PWM_CHAN_NUM        12
#define MAX_RATIO_PERCENT       1000

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

void pwm_clean(void);
gint32  pwm_read(guint32 chanel_num, guint32* ratio);
gint32  pwm_write(guint32 chanel_num, guint32 ratio);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif