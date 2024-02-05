/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : power_supply.h
  版 本 号   : 初稿
  部    门   :
  作    者   : zhouyubo@huawei.com
  生成日期   : 2013年7月25日
  最近修改   :
  功能描述   : 电源管理对外接口
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月25日
    作    者   : zhouyubo@huawei.com
    修改内容   : 创建文件

******************************************************************************/
#ifndef POWER_SUPPLY_H
#define POWER_SUPPLY_H

#include <glib.h>
#include "pme/common/mscm_macro.h"
#include "pme_app/hi51/hi51_sys_config.h"
#include "power_supply_err.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define PS_51_HANDLE_INVALID    (-1)


#ifndef ARM64_HI1711_ENABLED
gint32 ps_51_open(gint *handle);
gint32 ps_51_close(gint handle);
guchar ps_get_data_reliability(void);
gint32 ps_51_ctrl(gint handle, guint32 start);
gint32 ps_get_data(gint handle, guint32 reg_offset, guint32 len, void *data_out);
gint32 ps_set_data(gint handle, guint32 reg_offset, guint32 len, void *data_in);
gint32 ps_set_para(gint handle, PS_PARA_S *ps_para);
gint32 ps_get_para(gint handle, PS_PARA_S *ps_para);
gint32 ps_get_monitor(gint handle, PS_MONITOR_S *ps_info);
gint32 ps_get_fru(gint handle, guint8 ps_id, PS_FRU_S *fru_data);
gint32 ps_set_present(gint handle, guint32 present);
gint32 ps_set_flag(gint handle, guint32 ps_id, guint32 flag);
gint32 ps_load_firmware(gint handle, const gchar *file_name);
gint32 ps_load_def_firmware(gint handle);
gint32 ps_dft_test_sram(gint handle);
gint32 c51_i2c_read( C51_RW_REG_INFO_S* addr_info );
gint32 c51_i2c_write( C51_RW_REG_INFO_S* addr_info );
#endif

gint32 ps_set_reg(gint handle, guint32 reg_offset, guint32 datain);
gint32 ps_get_reg(gint handle, guint32 reg_offset, guint32 *datain);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
