/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : sys_info.h
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

#ifndef _SYS_INFO_H_
#define _SYS_INFO_H_

#include "glib.h"

#define SDK_VER_LEN                     64
#define UBOOT_VER_LEN                   32

/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

enum
{
    CONFIG_256M_MEM,
    CONFIG_512M_MEM,
    CONFIG_RESERVED
} ;

gint32  get_core_temp(gint32* core_temp);
gint32  get_reset_type(gint32* reset_type);
gint32  get_sys_jiffies(guint32* sys_jif);
gint32  get_startup_times(guint32* startup_times);
gint32  clear_startup_times(void);
gint32  get_sdk_ver(gchar version[SDK_VER_LEN], gchar build_time[SDK_VER_LEN], gint32 ver_length, gint32 time_length);
gint32  get_mother_board_id(guint8* board_id);
gint32  get_board_pcb_version(guint8* pcb_ver);
gint32  get_board_hw_platform(guint8* hw_platform);
gint32  get_pcie_addr(guint32* pcie_addr, guint32* pcie_size);
gint32  get_pram_reserve_addr(guint32* pram_addr, guint32* pram_size);
gint32  set_pcie1_flag(guint32 flag);
gint32  get_ddr_self_test_result(guint32* result);
gint32 get_uboot_file_ver(guint8* uboot_file_buf, guint32 uboot_file_len,
    gchar uboot_version[UBOOT_VER_LEN], guint32 uboot_ver_len);
gint32 get_uboot_ver(guint32 uboot_num, gchar uboot_version[UBOOT_VER_LEN]);
gint32 get_cur_uboot_num(guint32* cur_uboot_num);
gint32  get_io_drive_config( guint8 id, guint8* value );
gint32  set_io_drive_config( guint8 id, guint8 value );
gint32  get_io_slew_config( guint8 id, guint8* value );
gint32  set_io_slew_config( guint8 id, guint8 value );
gint32 get_uboot_build_time(guint32 uboot_num, gchar build_time[UBOOT_VER_LEN]);
gint32  get_memory_type(guint8* mem_type);
/* BEGIN: Added by lingmingqiang, 2016/12/6   需求单号:RM-201610116373*/
gint32 set_vga_display_status(guint32 status);
/* END:   Added by lingmingqiang, 2016/12/6 */
/* BEGIN: Add by lingmingqiang, 2017/2/8 RM-201702075496*/
gint32 get_rsvmem_free_support(gint32* support_flag);
/* END:   Modified by lingmingqiang, 2017/2/8 */
gint32 get_product_type(guint32 *product_type);
gint32 get_bmc_reset_flag(guint32 *bmc_reset_flag);
gint32 get_hi1710_serdes(unsigned char type, unsigned char lane_id, unsigned char *drv_value, unsigned char *emp_post, unsigned char *rx_eq);
gint32 set_hi1710_serdes(unsigned char type, unsigned char lane_id, unsigned char drv_value, unsigned char emp_post, unsigned char rx_eq);
gint32  set_localbus_bitwide_and_offset( guint8 cs, guint8 cf_addr_offset, guint8 bit_wide);
gint32  set_cd_param(void);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif
