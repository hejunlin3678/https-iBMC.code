/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : usb.h
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

#ifndef _USB_H_
#define _USB_H_

#include "glib.h"

/*****************************************************************************
对外提供的宏定义
 ******************************************************************************/
#define	USB_FUNC_KEYBOARD		0x01
#define	USB_FUNC_MOUSE			0x02
#define	USB_FUNC_CDROM			0x04
#define	USB_FUNC_FLOPPY			0x08
#define   USB_FUNC_STORAGE              0x10

#define USB_FUNC_NUM_MAX    8
#define USB_DEV_NUM_MAX     4

#define USB_DEV_NOTCONNECTED    0
#define USB_DEV_CONNECTED         1

#define	MAX_TIMEOUT_US					1000

#define	HWUSB_DEVRESET		-1
#define	HWUSB_ERESTARTSYS	-2
#define	HWUSB_EINVAL		-3
#define	HWUSB_ETIMEOUT		-4
#define	HWUSB_EFAULT		-5


/*****************************************************************************
数据结构定义
*****************************************************************************/

typedef struct tag_usb_connect_info_s
{
    guint32 usb_id; /*要做复合设备的USB口*/
    guint32 fn_cnt; /*此USB口要做的功能设备数量，最多一个口支持8个*/
    guint32 *fn_list; /*此USB口要支持的功能列表数组指针*/
    guint32 *fn_id; /*连接成功后，按照fn_list的顺序返回对应设备ID数组*/
    guint32 option; /*连接选项，0表示仅连接操作，1表示复位连接重连*/
    guint32 reserved[1];   /*保留扩展字段*/
} USB_CONNECT_INFO_S;

typedef struct tag_usb_disconnect_info_s
{
    guint32 usb_id; /*要做复合设备的USB口*/
    guint32 fn_cnt; /*断开连接的功能设备数（扩展保留，1711不支持，不使用）*/
    guint32 *fn_id; /*断开连接的fucntion id数组指针（扩展保留，1711不支持，不使用）*/
    guint32 reserved[2];   /*保留扩展字段*/
} USB_DISCONNECT_INFO_S;


typedef struct tag_usb_buffer_info_s
{
    guint32 usb_id; /*要做复合设备的USB口*/  
    guint32 fn_id;  /*要读取/写入数据function功能设备ID（由connect接口返回的fn_id）*/
    guint8 *buf; /* 需要读取/写入的数据buffer*/
    guint32 len; /* 需要读取/写入的数据长度*/
    guint32 timeout; /*读取/写入请求超时时间，为0，表示不等待，立即返回
    其他，表示单位为us的等待时间，该值超过MAX_TIMEOUT_US时自动截短*/
    guint8 overflow;  /*在接收过程中，出现发生数据溢出的情况
                        当接收缓冲区不足以存放接收到的数据时，该位为1（键鼠设备不使用）*/
    guint8 splited; /*  在接收过程中，该请求对应的数据中包含非完整包的情况
                        当底层一个包被分割到2个不同的请求中时，该位为1
                        （键鼠设备不使用）*/
    guint16 reserved; /*保留字段*/
    guint32 result; /*请求结果，*/
} USB_BUF_INFO_S;

typedef struct tag_usb_drd_ctrl_s
{
        
    guint32 usb_id; /*要做复合设备的USB口*/ 
    guint32 role;  /*USB口角色：  0 - DEVICE 1 - HOST*/
    guint32 reserved[2]; /*保留字段*/
} USB_DRD_CTRL_S;

#define CPLD_INT_SRC_REG_ID     0 /*用于判断是否是USB角色切换中断的寄存器ID*/
#define CPLD_USB_ROLE_REG_ID   1/*用于判断USB角色的状态寄存器ID*/
#define CPLD_USB_DRD_REG_CNT  2/*USB DRD相关CPLD寄存器个数*/

typedef struct tag_usb_drd_cpld_s
{
    guint8 bus_id;  /*CPLD连接的总线ID（1711对应localbus片选）*/
    guint32 reg_offset[CPLD_USB_DRD_REG_CNT]; /*各个寄存器偏移 */
} USB_DRD_CPLD_S;

/*****************************************************************************
kernel
*****************************************************************************/
typedef struct tag_usb_comp_conn{
    unsigned int usb_id;     /*复合设备USB控制器ID*/
    unsigned int fn_cnt;     /*复合设备功能数，最多支持8个*/
    unsigned int fn_list[USB_FUNC_NUM_MAX];      /*复合设备功能列表数组*/
    //unsigned int fn_id[USB_FUNC_NUM_MAX];      /*复合设备功能id 数组*/
} USB_COMP_CONNECT_S;

typedef struct tag_usb_comp_state{
    unsigned int usb_id;     /*复合设备USB控制器ID*/
    unsigned int conn_state;      /*复合设备连接状态，0表示未连接，1表示已连接*/
    unsigned int fn_cnt;     /*复合设备功能数，最多支持8个*/
    unsigned int fn_list[USB_FUNC_NUM_MAX];      /*复合设备功能列表数组*/
} USB_COMP_STATE_S;

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32 usb_compound_init(void);
void usb_compound_exit(void);
gint32 usb_compound_connect(USB_CONNECT_INFO_S* info);
gint32 usb_compound_disconnect(USB_DISCONNECT_INFO_S* info);
gint32 usb_compound_read(USB_BUF_INFO_S* info);
gint32 usb_compound_write(USB_BUF_INFO_S* info);
gint32 usb_compound_state_get(guint32 usb_id, guint32 fn_id, guint32 *state);
gint32 usb_drd_set_role(USB_DRD_CTRL_S* ctrl);
gint32 usb_drd_get_role(USB_DRD_CTRL_S* ctrl);
gint32 usb_drd_set_cpld_info (USB_DRD_CPLD_S * info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
