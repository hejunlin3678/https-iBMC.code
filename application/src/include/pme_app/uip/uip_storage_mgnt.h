/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : uip_storage_mgnt.h
  版 本 号   : 初稿
  作    者   : huanghan (h00282621)
  生成日期   : 2016年12月10日
  最近修改   :
  功能描述   : 声明UI接口解析存储设备配置命令参数的接口
  函数列表   :
  修改历史   :
  1.日    期   : 2016年12月10日
    作    者   : huanghan (h00282621)
    修改内容   : 创建文件

******************************************************************************/

#ifndef __UIP_STORAGE_MGNT_H__
#define __UIP_STORAGE_MGNT_H__

#include <glib.h>
#include <glib/gprintf.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UIP_LD_OPTION_ARRAY_ID              "array"
#define UIP_LD_OPTION_RAID_LEVEL            "rl"
#define UIP_LD_OPTION_PD_LIST               "pd"
#define UIP_LD_OPTION_SPAN_DEPTH            "sc"
#define UIP_LD_OPTION_CACHECADE_FLAG        "cachecade"
#define UIP_LD_OPTION_NAME                  "name"
#define UIP_LD_OPTION_SIZE                  "size"
#define UIP_LD_OPTION_STRIP_SIZE            "ss"
#define UIP_LD_OPTION_READ_POLICY           "rp"
#define UIP_LD_OPTION_WRITE_POLICY          "wp"
#define UIP_LD_OPTION_IO_POLICY             "iop"
#define UIP_LD_OPTION_ACCESS_POLICY         "ap"
#define UIP_LD_OPTION_DISK_CACHE_POLICY     "dcp"
#define UIP_LD_OPTION_INIT_TYPE             "init"
#define UIP_LD_OPTION_ACCELERATION_METHOD   "acc"
#define UIP_LD_OPTION_ASSOCIATED_LD         "associatedld"
#define UIP_LD_OPTION_CACHE_LINE_SIZE       "cls"
#define UIP_LD_OPTION_BGI_STATE             "bgi"
#define UIP_LD_OPTION_CACHECADE_STATE       "sscd"
#define UIP_LD_OPTION_BOOTABLE_FLAG         "boot"
#define UIP_LD_OPTION_BLOCK_FLAG            "block"

#define UIP_RAID_CONTROLLER_OPTION_COPYBACK_STATE             "cb"
#define UIP_RAID_CONTROLLER_OPTION_SMARTER_COPYBACK_STATE     "smartercb"
#define UIP_RAID_CONTROLLER_OPTION_JBOD_STATE                 "jbod"
#define UIP_RAID_CONTROLLER_OPTION_RESTORE                    "restore"
#define UIP_RAID_CONTROLLER_OPTION_MODE                       "mode"

#define UIP_PD_OPTION_FW_STATE              "state"
#define UIP_PD_OPTION_HOT_SPARE_STATE       "hotspare"
#define UIP_PD_OPTION_SPARE_LD              "ld"
#define UIP_PD_OPTION_LOCATE_STATE          "locate"
#define UIP_PD_OPTION_LOCATE_DURATION       "duration"
#define UIP_PD_OPTION_CRYPTO_ERASE		    "cryptoerase"
#define UIP_PD_OPTION_BOOTABLE_FLAG         "boot"

#define UIP_USER_OPTION_LOGIN_INTERFACE		"ui"
#define UIP_USER_OPTION_PASSWORD		"up"

/* BEGIN: Added by huanghan (h00282621), 2016/12/10   问题单号:AR-0000267460-001*/
gint32 uip_parse_ld_raid_level(gchar* arg, guint8* raid_level);
gint32 uip_parse_ld_array_id(gchar* arg, guint32* array_id);
gint32 uip_parse_ld_pd_list(gchar* arg, guint8** pd_list, guint32* pd_count);
gint32 uip_parse_ld_name(gchar* arg, gchar* buf, guint32 buf_size);
gint32 uip_parse_ld_span_depth(gchar* arg, guint8* span_depth);
gint32 uip_parse_ld_size(gchar* arg, guint32* size, guint8* size_unit);
gint32 uip_parse_ld_strip_size(gchar* arg, guint8* strip_size);
gint32 uip_parse_ld_read_policy(gchar* arg, guint8* read_policy);
gint32 uip_parse_ld_write_policy(gchar* arg, guint8* write_policy);
gint32 uip_parse_ld_io_policy(gchar* arg, guint8* io_policy);
gint32 uip_parse_ld_access_policy(gchar* arg, guint8* access_policy);
gint32 uip_parse_ld_disk_cache_policy(gchar* arg, guint8* disk_cache_policy);
gint32 uip_parse_ld_init_type(gchar* arg, guint8* init_type);
gint32 uip_parse_ld_block_index(gchar* arg, guint8* block_index);
gint32 uip_parse_ld_accelerator(gchar *arg, guint8 *accelerator);
gint32 uip_parse_associated_ld(gchar *arg, guint16 *associated_ld);
gint32 uip_parse_cache_line_size(gchar *arg, guint8 *cache_line_size);
gint32 uip_parse_ld_bgi_state(gchar* arg, guint8* bgi_state);
gint32 uip_parse_boot_priority(gchar* arg, guint8* boot_param);
gint32 uip_parse_ld_sscd_caching_state(gchar* arg, guint8* ssdc_state);
gint32 uip_parse_ctrl_config_state(gchar* arg, guint8* state);
gint32 uip_parse_ctrl_config_mode(gchar *arg, guint8 *mode);
gint32 uip_parse_pd_state(gchar* arg, guint8* state);
gint32 uip_parse_pd_hotspare_state(gchar* arg, guint8* state);
gint32 uip_parse_pd_hotspare_ld(gchar* arg, guint32* ld);
gint32 uip_parse_pd_locate_state(gchar* arg, guint8* state);
gint32 uip_parse_pd_locate_duration(gchar* arg, guint32* duration);
/* END:   Added by huanghan (h00282621), 2016/12/10 */

#ifdef __cplusplus
}
#endif

#endif /* __UIP_STORAGE_MGNT_H__ */

