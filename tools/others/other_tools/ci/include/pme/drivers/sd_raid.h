/******************************************************************************

                  版权所有 (C), 2001-2013, 华为技术有限公司

******************************************************************************
  文 件 名   : sd_raid.h
  版 本 号   : 初稿
  作    者   : h00168999
  生成日期   : 2013年8月2日
  最近修改   :
  功能描述   : hi1710平台SDK用户程序接口
  函数列表   :
  修改历史   :
  1.日    期   : 2014年6月9日
    作    者   : h00168999
    修改内容   : 创建文件

******************************************************************************/

#ifndef _SD_RAID_H_
#define _SD_RAID_H_

#include "glib.h"

/*****************************************************************************
对外宏定义
*****************************************************************************/
#define SD_RAID_HEALTHY                 0
#define SD_RAID_UNHEALTHY               1

#define SD_0                            0
#define SD_1                            1
#define MAX_SUPPORT_SD_NUM              2

#define MAX_FX3S_PARTITION_NUM          4

#define SD_OWNER_PME                    0
#define SD_OWNER_X86                    1

#define MAX_FX3S_FIRMWARE_LEN           (1 * 1024 * 1024)
#define MAX_FW_VER_LEN                  32

#define UNIT_M                          (1024 * 1024)

#define NOT_PRESENT                     0
#define PRESENT                         1

#define VENDOR_LEN			20
#define CID_LENTH                     16
#define SID_LENTH                     16


typedef struct tag_fx3s_manual_status_s
{
	guint8 present[MAX_SUPPORT_SD_NUM];/*在位信息，0表示不在位，状态信息仅在SD在位是有效*/
	guint8 partition_count[MAX_SUPPORT_SD_NUM];/* SD卡上的分区个数*/
	guint32 partition_size[MAX_SUPPORT_SD_NUM][MAX_FX3S_PARTITION_NUM];/*SD卡上的分区大小(单位为block)*/
	guint32 sync_mode[MAX_SUPPORT_SD_NUM];/*同步模式，0表示自动模式，1表示手动模式*/
	guint32 device_mode[MAX_SUPPORT_SD_NUM];/*0:undefined; 1:primary; 2:secondary*/
	guint8 scope[MAX_SUPPORT_SD_NUM];
	guint8 dirty_partitions[MAX_SUPPORT_SD_NUM];/* Bitmap of dirty partitions */
	guint8 stale_partitions[MAX_SUPPORT_SD_NUM];/* Bitmap of stale partitions */	
} FX3S_QUERY_STATUS_MANUAL_S;


/*****************************************************************************
对外接口
*****************************************************************************/
extern gint32  get_fx3s_version(gchar version[MAX_FW_VER_LEN]);
extern gint32  get_fx3s_sd_pres(guint8 port, guint8 * pst);
extern gint32  get_fx3s_sd_volume(guint8 port, guint64 * vol);
extern gint32  update_fx3s_fw(guint8 * fw_buffer, guint32 fw_len);
extern gint32  get_fx3s_health_state(guint8 port, guint8 * health_st);
extern gint32  get_fx3s_sd_raid_vendor(gint8 raid_vendor[VENDOR_LEN], gint32 length);
extern gint32  get_fx3s_sd_cid_info(guint8 port, guint8 cid[CID_LENTH]);
extern gint32  get_fx3s_sd_raid_status(guint8* num, guint8* synced_card, guint8 status[MAX_FX3S_PARTITION_NUM]);
extern gint32  creat_fx3s_partition(guint8 num, guint32 size[MAX_FX3S_PARTITION_NUM],
                                     guint8 enable[MAX_FX3S_PARTITION_NUM],
                                     guint8 writable[MAX_FX3S_PARTITION_NUM]);
extern gint32 set_fx3s_part_visible(guint8 num, guint8 enable);
extern gint32 set_fx3s_part_writable(guint8 num, guint8 enable);
extern gint32 set_fx3s_sd_primary(guint32 primary_sd);
extern gint32 set_fx3s_sd_read_err_thre(guint32 err_thre);
extern gint32 set_fx3s_sd_write_err_thre(guint32 err_thre);
extern gint32 set_fx3s_sd_rw_ctrl(guint32 rw_ctrl);
extern gint32 reset_sd_raid(void);
extern void clear_sd_raid_fd(void);
/* BEGIN: Added by lingmingqiang, 2016/12/9   RM-201607063688 for manual sync*/
extern gint32 creat_fx3s_partition_manual(guint8 num, guint32 size[MAX_FX3S_PARTITION_NUM],
                                    guint8 enable[MAX_FX3S_PARTITION_NUM],
                                    guint8 writable[MAX_FX3S_PARTITION_NUM]);
extern gint32 start_fx3s_sync(guint8 part_num);
extern gint32 set_fx3s_sync_dir(guint8 dir);
extern gint32 get_fx3s_status_manual(FX3S_QUERY_STATUS_MANUAL_S* fx3s_query_status);
/* END:   Added by lingmingqiang, 2016/12/9 */
extern gint32 get_fx3s_sd_raid_metadata_state(guint32 *state);
extern gint32 get_fx3s_sd_sid_info(guint8 port, guint8 sid[SID_LENTH]);

#endif

