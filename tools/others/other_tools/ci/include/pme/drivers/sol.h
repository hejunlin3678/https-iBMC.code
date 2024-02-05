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

#ifndef _SOL_H_
#define _SOL_H_

#include "glib.h"

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef struct tag_sol_data_s
{
    guint32 cur;                /* HOST发送数据的方向，0表示获取HOST发的数据，1 表示获取HOST收的数据 */
    gchar*  read_buf;           /* 存放读取的数据 */
    guint32 length;             /* 读取数据的长度，《=4M，且长度必须为4的倍数 */
} SOL_DATA_S;

typedef struct tag_sol_data_pos_s
{
    guint32 pos_start; /* 物理起始地址 */
    guint32 pos_offset; /* 相对起始地址的偏移 */
} SOL_DATA_POS_S;

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32  sol_init(void);
void sol_clean(void);
gint32  sol_enable(gint32 enable);
gint32  sol_read(SOL_DATA_S* data);
gint32 sol_get_pos(SOL_DATA_POS_S *pos);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
