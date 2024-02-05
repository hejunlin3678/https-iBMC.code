/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : adc_base.h
  版 本 号   : 初稿
  部    门   :
  作    者   : licheng
  生成日期   : 2013年10月28日
  最近修改   :
  功能描述   : adc驱动头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年10月28日
    作    者   : licheng
    修改内容   : 创建文件

******************************************************************************/

#ifndef _ADC_BASE_H_
#define _ADC_BASE_H_

#include "glib.h"

/* 参考电压源 */
typedef enum adc_volt_src_e
{
	EXTERNAL_VOLT,  /* 外部参考电压 */
	INTERNAL_VOLT   /* 内部参考电压 */
} VOLT_REF_SRC;

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
        extern "C" {
#endif
#endif /* __cplusplus */

extern gint32 adc_init(guint scan_rate, VOLT_REF_SRC volt_ref_src, guint volt_ref_val);
extern void adc_clean(void);
extern gint32 adc_read(gint chan_id, guint32 *val);


#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif /* __cplusplus */


#endif


