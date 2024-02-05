/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : peci.h
  版 本 号   : 初稿
  作    者   : h00168999
  生成日期   : 2013年8月2日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2014年6月9日
    作    者   : h00168999
    修改内容   : 创建文件

******************************************************************************/

#ifndef _PECI_H_
#define _PECI_H_

#include "glib.h"

/*****************************************************************************
对外宏定义
*****************************************************************************/
#define PECI_MAX_DATA_NUM   32   /*读写的最大数据字节数*/

/*****************************************************************************
对外接口
*****************************************************************************/
extern gint32 peci_init(void);
extern gint32 peci_write_read(guint8 wr_data[PECI_MAX_DATA_NUM], guint32 wr_len,
                                     guint8 rd_data[PECI_MAX_DATA_NUM], guint32* rd_len);

#endif

