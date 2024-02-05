/* 
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved. 
* Description: 时间、时区管理相关的定义。
* Author: h00282621 
* Create: 2020-3-10 
* Notes: 用于跨模块使用的相关的定义。
*/

#ifndef __TIME_DEFINE_H__
#define __TIME_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define TIME_ZONE_GMT       "GMT"
#define TIME_ZONE_UTC       "UTC"
#define TIME_UTC_LEN        3
#define TIME_ZONE_FILE      "/dev/shm/OStimezone.infile"
#define LOCALTIME_FILE      "/data/etc/localtime"
#define LOCALTIME_TMP_FILE  "/data/etc/localtime.tmp"
#define TIMEZONE_CGP        "/dev/shm/CGP"
#define ZONEINFO_PATH       "/usr/share/zoneinfo/"

#define TIMEZONE_MIN_LEN    3
#define TIMEZONE_MAX_LEN    48
#define TIMEZONE_STR_LEN    10
#define SEXAGESIMAL         60 /*60进制*/
#define MIN_TIME_OFFSET     -720 /*最小的时间偏移*/
#define MINUS_SIXTY_MINUTES_OFFSET -60 /*60分钟的时间偏移*/
#define MAX_TIME_OFFSET     840 /*最大的时间偏移*/
#define GMT_FORMAT          0 /*GMT的时区格式*/
#define UTC_FORMAT          1 /*UTC的时区格式*/
#define TIME_ZONE_STRING_LEN  TIMEZONE_MAX_LEN

#define TIMEZONE_AREA_NUM   15
#define TIMEZONE_CITY_NUM   200  /* 新增城市后此处越界，改大预留空间 */
#define AREA_CITY_NAME_SZ   30
#define START_CACHE_FS      1
#define STOP_CACHE_FS       0

#define NTP_SYNC_RESULT_SUCCESS     0
#define NTP_SYNC_RESULT_NOT_SUCCESS 1

#define TIME_SRC_SMM 2  /* 时间源为管理板 */

// 板上RTC时间相关
#define LEAP_MONTH_DAY 29
#define YEAR_LEAP_4    4
#define YEAR_LEAP_100  100
#define YEAR_LEAP_400  400
#define YEAR_2099      2099
#define YEAR_2020      2020
#define YEAR_2010      2010
#define YEAR_2000      2000
#define YEAR_1900      1900
#define YEAR_1800      1800
#define MONTH_FEB      2
#define MONTH_DATE_30  30
#define MONTH_DATE_MAX 31
#define MONTH_DATE_28  28
#define MONTH_APIR     4
#define MONTH_JUNE     6
#define MONTH_SEP      9
#define MONTH_NOV      11
#define MONTH_DEC      12
#define MONTH_JAN      1
#define BASE_10        10
#define BASE_16        16
#define MONTH_DATE_MIN 1
#define MAX_MINUTE     59
#define MAX_SECOND     59
#define MAX_HOUR_DAY   23

#define MAX_TIME_ZONE   840
#define MIN_TIME_ZONE   (-720)
#define SECOND_PER_MIN  60
#define SECOND_PER_HOUR 3600
#define SECOND_PER_DAY  (SECOND_PER_HOUR * 24)
#define SECOND_TEN_YEARS    (SECOND_PER_HOUR * 24 * 365 * 10)
#define DAY_PER_YEAR 365


#define TIME_FORMAT_ERR 2
typedef struct tg_BMCCOMMON_RTC_TIME_S {
    guint16 Year; /* 年 2014 */
    guint8 Month; /* 月 1-12 */
    guint8 Date;  /* 日 1-31 */

    guint8 Day;

    guint8 Hour;   /* 时: 24小时制式 */
    guint8 Minute; /* 分: 0-59min */
    guint8 Second; /* 秒: 0-59min */
} BMCCOMMON_RTC_TIME_S;

/* 系统时间 */
typedef struct tg_BMC_SYS_TIME_S {
    guint16 Year; /* 年 2014 */
    guint8 Month; /* 月 1-12 */
    guint8 Date;  /* 日 1-31 */

    guint8 Hour;   /* 时: 24小时制式 */
    guint8 Minute; /* 分: 0-59min */
    guint8 Second; /* 秒: 0-59min */
} BMC_SYS_TIME_S;



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TIME_DEFINE_H__ */
