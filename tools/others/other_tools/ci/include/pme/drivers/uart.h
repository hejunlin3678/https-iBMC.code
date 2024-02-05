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

#ifndef _UART_H_
#define _UART_H_

#include "glib.h"

/*****************************************************************************
对外提供的宏定义
 ******************************************************************************/
#define PORT0 0
#define PORT1 1
#define PORT2 2
#define PORT3 3
#define PORT4 4
#define PORT5 5
#define PORT6 6
#define PORT7 7

#define UART0 0
#define UART1 1
#define UART2 2
#define UART3 3
#define UART4 4
#define UART5 5
#define UART6 6
#define UART7 7

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef struct tag_uart_connect_info_s
{
    guint32 uart;           /* uart 0~4   (具体可用连接需遵循Hi1710用户手册) */
    guint32 port_or_uart;   /* 端口0~4 */
} UART_CONNECT_INFO_S;

typedef struct uart_x86_baud_allinfo_s
{
    guint32 x86_uart_chan;     /*必须是0或者1*/
    guint32 uart_baud;         /*返回波特率信息*/
    guint32 * baud_constant_list;  /*指定x86波特率的范围值*/
}UART_X86_BAUD_INFO_S;

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32  uart_connect_init(void);
void uart_connect_clean(void);
gint32  uart_connect_uart(UART_CONNECT_INFO_S* info);
gint32  uart_connect_port(UART_CONNECT_INFO_S* info);
gint32 uart_init(guint8 chan, guint32 baud);
void uart_close(guint8 chan);
gint32 uart_read(guint8 chan, gchar* obuffer, guint32 max_read_len);
gint32 uart_write(guint8 chan, gchar* ibuffer, guint32 len);
gint32 get_current_baud(gint32 uart_index);
void update_uart_baud(gint32 uart_index, guint8 connect_uart);
void update_uart_baud_v2(guint8 chan, guint32 uart_baud);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
