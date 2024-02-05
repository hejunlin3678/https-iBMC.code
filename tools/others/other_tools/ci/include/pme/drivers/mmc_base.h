/******************************************************************************

                  版权所有 (C), 2008-2017, 华为技术有限公司

******************************************************************************
  文 件 名   : mmc_base.h
  版 本 号   : 初稿
  作    者   : w00305139
  生成日期   : 2017年8月7日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2017年8月7日
    作    者   : w00305139
    修改内容   : 创建文件

******************************************************************************/

#ifndef _MMC_BASE_H_
#define _MMC_BASE_H_

#include "glib.h"

/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef enum MMC_REG_TYPE
{
	MMC_EXT_CSD,	//emmc ext_csd寄存器
	MMC_CID,		//emmc cid寄存器
	MMC_CSD 		//emmc csd寄存器
}MMC_REG_TYPE_E;

typedef struct MMC_REG_INFO
{
	MMC_REG_TYPE_E mmc_reg_type_v;//emmc颗粒寄存器类型
	unsigned int mmc_reg_offset;  //emmc颗粒寄存器的偏移(除了est_csd寄存器其他两个是0)
	unsigned int length;          //需要获取的emmc颗粒寄存器的字节长度
	unsigned char *buf;           //获取的数据
}MMC_REG_INFO_S;

typedef struct MMC_WRITE_ALLINFO
{
    unsigned int mmc_clear_flag;    //是否清除前一次的统计数据(时间和写入量)
    unsigned long write_time;       // 统计的总的时间
	unsigned long long write_byte;  // 在统计总的时间范围内的写入量
}MMC_WRITE_ALLINFO_S;


/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32 mmc_init(void);
gint32 mmc_read(MMC_REG_INFO_S* mmc_opt_s);
gint32 mmc_get_hynix_health_report(unsigned char *hynix_ext_csd, unsigned int ext_csd_length);// 长度只能传512
gint32 mmc_get_tbw(MMC_WRITE_ALLINFO_S* mmc_write_allinfo_s);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
