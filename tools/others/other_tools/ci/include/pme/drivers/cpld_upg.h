/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : cpld_upg.h
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

#ifndef _CPLD_UPG_H_
#define _CPLD_UPG_H_

#include "glib.h"

/*****************************************************************************
对外数据结构接口函数定义
*****************************************************************************/
#define CPLD_UPGRADE_LOAD_MODE  0
#define CPLD_UPGRADE_RST_MODE   1
#define MAX_SCAN_CHAIN_SIZE     6

/*****************************************************************************
对外数据结构接口函数定义
*****************************************************************************/
typedef struct tag_cpld_upg_file_s
{
    guint8  mode;
    guint32 product_id;     /*cpld文件id*/
    guint32 board_id;       /*board id,保留*/
    guint32 pcb;            /*pcb,保留*/
    guint32 component_id;   /*组件ID,0=底板cpld，1=背板cpld*/
    guint32 length;
    guint8* buf;
} CPLD_UPG_FILE_S;

typedef struct cpld_device {
    guint8 select;
    guint8 cpld_id;
    guint32 device_count;
    guint32 device_id[MAX_SCAN_CHAIN_SIZE];
} CPLD_DEVICE_S;

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32  cpld_reset(void);
gint32  cpld_upg_write(guint8 * buf, guint32 length, guint32 component_id, guint32 product_id);
gint32  cpld_upg_init(guint8 tck, guint8 tms, guint8 tdo, guint8 tdi);	
gint32  config_cpld_upg_reg(guint8 offset, guint8 tck, guint8 tms, guint8 tdo, guint8 tdi);
gint32  cpld_reset_init(guint8 rst);
gint32  cpld_spi_reset(void);
gint32  cpld_upg_init_v2(void);
gint32 cpld_get_id(CPLD_DEVICE_S *cpld_device_s);
gint32 cpld_set_num(guint32 cpld_num);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
