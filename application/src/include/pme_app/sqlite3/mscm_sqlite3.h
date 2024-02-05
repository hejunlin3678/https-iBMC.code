/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : mscm_sqlite3.h
  版 本 号   : 初稿
  作    者   : zhenggenqiang 207985
  生成日期   : 2013年8月21日
  最近修改   :
  功能描述   : db_transaction.c 的外部头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月21日
    作    者   : zhenggenqiang 207985
    修改内容   : 创建文件

******************************************************************************/
#ifndef _MSCM_SQLITE3_H_
#define _MSCM_SQLITE3_H_

/* SQLITE3开源头文件 */
#include "sqlite3.h"

#include <stdio.h>
#include <glib.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

/* SQLITE数据库文件的权限 */
#define SQLITE_DB_FILE_RIGHT (S_IRUSR | S_IWUSR | S_IROTH | S_IRGRP | S_IWOTH | S_IWGRP)
#define SQLITE_DB_FILE_RIGHT_LITE  (S_IRUSR | S_IWUSR | S_IRGRP)

#define COMP_TYPE_CPU                   "CPU"
#define COMP_TYPE_MEMORY                "Memory"
#define COMP_TYPE_HARDDISK              "Disk"
#define COMP_TYPE_PSU                   "PSU"
#define COMP_TYPE_FAN                   "Fan"
#define COMP_TYPE_MAINBOARD             "Mainboard"
#define COMP_TYPE_SYSTEM                "System"
#define COMP_TYPE_CHASSIS               "Chassis"

#define TAG_TABLE_CLASS                 "classification"
#define TAG_TABLE_IDENT                 "identification"

#define METRIC_TYPE_TEMP                "Temperature"
#define METRIC_TYPE_VOLTAGE             "Voltage"
#define METRIC_TYPE_CURRENT             "Current"
#define METRIC_TYPE_PERFORMANCE         "Performance Indicator"
#define METRIC_TYPE_HEALTH              "Health Status Indicator"
#define METRIC_TYPE_LOG                 "Log"
#define METRIC_TYPE_SMART               "Smart"

enum DA_REPORT_METRIC_TYPE
{
    INVALIDTYPE,
    VOLTAGE,
    TEMPERATURE,
    PERFORMANCE,
    HEALTH,
    CURRENT,
    LOG,
    SMART
};

typedef void (*RepairFunc)(void);

typedef struct tag_dev_da_database
{
    gchar       db_path[MAX_FILEPATH_LENGTH + 1];   // 设备数据采集的数据库路径
    gchar       item_db_path[MAX_FILEPATH_LENGTH + 1];  // item.db数据库文件路径
    gchar       data_acq_db_path[MAX_FILEPATH_LENGTH + 1];  // data_acq.db数据库文件路径
    gchar       data_acq_anoth_db_path[MAX_FILEPATH_LENGTH + 1];    // data_acq_another.db数据库文件路径
    gint32      item_db_lock;
    gulong      item_db_sem_id;
    sqlite3*    item_sqlite_db;
    gint32      data_acq_db_lock;
    gulong      data_acq_db_sem_id;
    sqlite3*    data_acq_sqlite_db;
    sqlite3*    data_acq_anoth_sqlite_db;
    sqlite3*    data_acq_mem_sqlite_db;  // 内存数据库句柄
    sqlite3*    active_data_db;         // 活动数据库句柄
    guint64     data_acq_datapoint_id;  // 保存当前最大datapointid值
    guint64     cur_datapoint_count;    // 当前数据库中总的数据点条数
    guint64     max_item_id;            // 最大数据采集项id，仅iRM中使用
    gulong      min_timestamp;          // 内存数据库中的最小时间戳(Flash数据库中的最大时间戳)，仅iRM中使用
    gulong      max_timestamp;          // 数据库中的最大时间戳，仅iRM中使用
    gchar       active_db_path[MAX_FILEPATH_LENGTH + 1];   // 设备活动数据库符号链接的路径，仅iRM中使用
    RepairFunc  item_db_repair_func;
} DEV_DA_DB_S;

// 查找条件
typedef struct get_data_conditions
{
    guchar      Component;
    guchar      MetricType;
    gulong      StartTime;
    gulong      EndTime;
} GET_CONDITIONS;

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern void db_sqlite_lock(gint32 handle);
extern void db_sqlite_unlock(gint32 handle);
extern gint32 db_begin_transaction(sqlite3* sqlite_db, gchar** err_msg);
extern gint32 db_commit_transaction(sqlite3* sqlite_db, gchar** err_msg);
extern gint32 db_rollback(sqlite3* sqlite_db, gchar** err_msg);
extern gint32 db_execute_sql_data(sqlite3* sqlite_db, const gchar* sql_buf, gchar** err_msg);
extern gint32 db_execute_sql_data_with_callback(sqlite3* sqlite_db, const gchar* sql_buf, sqlite3_callback callback, void* pArg, gchar** err_msg);
extern gint32 db_sqlite_close(sqlite3* sqlite_db);
extern gint32 db_sqlite_open(const gchar* sql_db_name, sqlite3** sqlite_db);
extern void db_free(gchar* msg);

extern gint32 dam_acquisition_info_reinit(void);
extern void _repair_daml_db_integrity(const gchar* db_path);
extern gint32 _da_db_lock_init(DEV_DA_DB_S* device_db);
extern void _da_db_lock(gint32 sqlite_lock, gulong sem_id);
extern void _da_db_unlock(gint32 sqlite_lock, gulong sem_id);
extern gint32 db_create_table(sqlite3* db_handle, gchar* table, gchar* data_buf);
extern gint32 get_db_size(sqlite3* db_handle, guint32* db_size);
extern gint32 _da_main_db_attach(DEV_DA_DB_S* device_db, const gchar* active_db, const gchar* sub_active_db);
extern gint32 _da_main_db_detach(DEV_DA_DB_S* device_db);
extern void _get_table_count(sqlite3* db_handle, gchar* table, guint64* count);
extern void get_cur_data_count(DEV_DA_DB_S* device_db);
gint32 get_db_size(sqlite3* db_handle, guint32* db_size);
extern void get_main_sub_db(DEV_DA_DB_S* device_db, gchar* active_db, guint32 act_db_path_len, gchar* sub_active_db, guint32 sub_act_db_path_len);
extern void _repair_db_for_sync(DEV_DA_DB_S* device_db);
extern gint32 translate_device_type(const gchar* type_string, guint8* dev_type);
extern gint32 translate_metric_type(const gchar* type_string, guint8* metric_type);
extern const gchar* _dam_get_metric_type_string(guchar metric_type);
extern guchar long_time_data_check(GET_CONDITIONS* conditions);
extern gint32 get_table_max_min_val(sqlite3* db_handle, const gchar* table, const gchar* key, guint64* out_value, guint8 cmd_flag);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
