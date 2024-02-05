/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : uip_vnc.h
  版 本 号   : 初稿
  作    者   : tangjie 00381753
  生成日期   : 2017年2月24日
  最近修改   :
  功能描述   : uip_vnc.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2017年2月24日
    作    者   : tangjie 00381753
    修改内容   : 创建文件

******************************************************************************/

#ifndef __UIP_VNC_H__
#define __UIP_VNC_H__

#include <glib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * 宏定义                                   *
 *----------------------------------------------*/

/*vnc的使能状态宏*/
#define VNC_DISABLED    0
#define VNC_ENABLED     1




/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

extern gint32 set_vnc_keyboard_layout(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar*  vnc_layout);
extern gint32 set_vnc_ssl_state(const gchar* interface, const gchar* username, const gchar* client_ip, guint8  vnc_ssl_state);
extern gint32 set_vnc_timeout(const gchar* interface, const gchar* username, const gchar* client_ip, gint32  vnc_timeout);
extern gint32 set_vnc_password_validity(const gchar* interface, const gchar* username, const gchar* client_ip, guint32  vnc_validity);
extern gint32 set_vnc_pwd(const gchar* interface, const gchar* username, const gchar* client_ip, const gchar* vnc_key);
extern gint32 set_vnc_pwd_complex_state(const gchar* interface, const gchar* username, const gchar* client_ip, guint8  state);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __UIP_VNC_H__ */

