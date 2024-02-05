/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : record_wave.h
  版 本 号   : 初稿
  部    门   : 
  作    者   : zhouyubo@huawei.com
  生成日期   : 2014年1月8日
  最近修改   :
  功能描述   : 录波库头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年1月8日
    作    者   : zhouyubo@huawei.com
    修改内容   : 创建文件

******************************************************************************/

#include <pme/common/mscm_macro.h>
#include <pme/pme.h>
#include <glib.h>

#ifndef RECORD_WAVE_H
#define RECORD_WAVE_H

#ifdef __cplusplus  
extern "C" {  
#endif /* __cplusplus */ 

#define RECORD_WAVE_BUF_MULTIPLES  2 //最大记录条目倍数
#define MAX_WAVE_NAME_LEN  64  //最大录播文件名长度

typedef struct tag_wave_cfg_s
{
    gchar *wave_name;
    gchar *web_view_path; /* WEB视图路径 */
    gchar *smm_view_path; /* SMM视图路径 */
    guint32  max_record_cnt; /* 最大记录数 */
    guint32  max_item_cnt; /* 最大记录数 */
    guint32  max_interval; /* 记录最大间隔时间 */    
    guint32  smm_item;     /* SMM记录项序号 */
    gchar    **item_label; /* 项目名称 */
}WAVE_CFG_S;

typedef struct tag_wave_record_s
{
    guint32 cur_time; /* 录波记录时间: 说明，不一定会精确以该时间记录，而是以*/
    guint32 item_cnt; /* 项目数 */
    guint32 *record_data; /* 录波记录数据 */
}WAVE_RECORD_S;

typedef struct tag_wave_header_s
{
    guint32  first_id; /* 第一条 */
    guint32  next_id; /* 下一条 */
    guint32  last_time; /* 最近一次记录时间 */
} WAVE_HEADER_S;

typedef struct tag_wave_s
{
    struct tag_wave_header_s header; /* 数据块头 */
    gchar* web_view_path; /* WEB视图路径 */
    gchar* smm_view_path; /* SMM板视图格式存储路径 */
    guint32  max_record_cnt; /* 最大记录数 */
    guint32  max_view_cnt;  /* 最大展示的记录数 */
    guint32  max_item_cnt; /* 最大条目数 */
    guint32  max_interval; /* 记录最大间隔时间 */
    gchar* table_head; /* 表头 */
    guint32* record_buf; /* 数据缓存区 */
    gchar wave_name[MAX_WAVE_NAME_LEN];
    gboolean loaded; /* 已经加载标志 */
    guint32 smm_item; /* SMM板交互所取的ITEM */
}WAVE_S;

typedef struct tag_statistic_s
{
    guint32 cur_val;
    guint32 max_val;
    guint32 min_val;
    gdouble aver_val;
    guint32 aver_cnt;
    guint32 max_time;
    guint32 min_time;
    guint32 start_time;
    guint32 cur_time;
}STAT_S;

extern guint32 wave_get_real_record_cnt(WAVE_S *wave_handle);
extern void wave_create_web_view(WAVE_S *wave_handle);
extern void wave_create_smm_view(WAVE_S *wave_handle);
extern WAVE_S *wave_init(WAVE_CFG_S *wave_cfg);
extern gboolean wave_record(WAVE_S *wave_handle, WAVE_RECORD_S *record_info);
extern void wave_exit(WAVE_S *wave_handle);
extern gint32 wave_clear(WAVE_S *wave_handle);
extern void wave_remove(WAVE_S *wave_handle);
extern gint32 wave_record_rollback(WAVE_S *wave_handle);

extern STAT_S *stat_init(void);
extern void stat_caculate(STAT_S *stat_handle, guint32 value, guint32 cur_time);
extern void stat_clear(STAT_S *stat_handle);
extern guint32 stat_get_cur(STAT_S *stat_handle);
extern guint32 stat_get_max(STAT_S *stat_handle);
extern guint32 stat_get_min(STAT_S *stat_handle);
extern guint32 stat_get_aver(STAT_S *stat_handle);
extern guint32 stat_get_aver(STAT_S *stat_handle);
extern guint32 stat_get_max_time(STAT_S *stat_handle);
extern guint32 stat_get_min_time(STAT_S *stat_handle);
extern guint32 stat_get_start_time(STAT_S *stat_handle);
extern guint32 stat_get_handle_len(void);
gboolean wave_load_record(WAVE_S *wave_handle);

#ifdef __cplusplus  
#if __cplusplus
}  
#endif
#endif /* __cplusplus */  

#endif

