/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : mdio.h
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

#ifndef _MDIO_H_
#define _MDIO_H_

#include "glib.h"

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */


#define QD_SMI_BUSY				0x8000
#define QD_SMI_MODE				0x1000
#define QD_SMI_MODE_BIT			12
#define QD_SMI_OP_BIT			10
#define QD_SMI_OP_SIZE			2
#define QD_SMI_DEV_ADDR_BIT		5
#define QD_SMI_DEV_ADDR_SIZE	5
#define QD_SMI_REG_ADDR_BIT		0
#define QD_SMI_REG_ADDR_SIZE	5

#define QD_SMI_CLAUSE45			0
#define QD_SMI_CLAUSE22			1

#define QD_SMI_WRITE			0x01
#define QD_SMI_READ				0x02


gint32  mdio_write(guint32 chan_id, guint32 phy_addr, guint32 reg_addr, guint32 value);
gint32  mdio_read(guint32 chan_id, guint32 phy_addr, guint32 reg_addr, guint32 * value);
gint32  mdio_init(void);

gint32  mdio45_read(guint32 chan_id, guint32 phy_addr, guint32 reg_addr, guint32 * value);
gint32  mdio45_write(guint32 chan_id, guint32 phy_addr, guint32 reg_addr, guint32 value);
gint32  mdio45_init(guint32 cpld_cs, guint32 base_addr);




gint32 mdio_multi_read(guint32 lsw_addr, guint32 phy_num,  guint32 phy_reg, guint32 *value);
gint32 mdio_multi_write( guint32 lsw_addr, guint32 phy_num, guint32 phy_reg, guint32 data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
