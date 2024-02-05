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

#ifndef _LOCAL_BUS_H_
#define _LOCAL_BUS_H_

#include "glib.h"

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef struct tag_local_bus_opt_s
{
    guint32 cs;     /*片选, L_BUS_CS_E*/
    guint32 type;   /*0为按字节访问，1为按比特位访问*/
    guint32 offset; /*偏移位置，以片选器件的访问位宽对齐*/
    guint32 len;    /*读/写数据的长度, 单位:字节;大于等于1，小于等于0x1000，以片选器件的访问位宽对齐；
                           按照bit访问时，长度值必须等于位宽的字节数*/
    guint32 mask;   /*比特掩码，1有效，按比特访问时有效，驱动只取与器件位宽一致的位数,其他的比特位不关注*/
    guint8 *data; /*读/写数据*/
}LOCAL_BUS_OPT_S;

enum
{
    L_BUS_CPLD = 0,    /*片选1--->为CPLD*/
    L_BUS_CS2  = 1,    /*片选2*/
    L_BUS_CS3  = 2     /*片选3*/
};

enum
{
    L_BUS_BYTE_ACCESS = 0,    /*按字节读取*/
    L_BUS_BIT_ACCESS  = 1,    /*按bit读取*/
};


/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32  local_bus_init(void);
void    local_bus_clean(void);
gint32  local_bus_read(LOCAL_BUS_OPT_S* l_bus_opt);
gint32  local_bus_write(LOCAL_BUS_OPT_S* l_bus_opt);
gint32 localbus_mmap_init(void);
void localbus_mmap_clean(void);
gint32 localbus_mmap_read(LOCAL_BUS_OPT_S* l_bus_opt);
gint32  localbus_mmap_write(LOCAL_BUS_OPT_S* l_bus_opt);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
