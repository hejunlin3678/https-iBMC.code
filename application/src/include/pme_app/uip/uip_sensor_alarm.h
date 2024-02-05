/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : uip_sensor_alarm.h
  版 本 号   : 初稿
  作    者   : zhenggenqiang 207985
  生成日期   : 2013年7月13日
  最近修改   :
  功能描述   : sensor.c 的头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月13日
    作    者   : zhenggenqiang 207985
    修改内容   : 创建文件

******************************************************************************/

#ifndef __UIP_SENSOR_ALARM_H__
#define __UIP_SENSOR_ALARM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/*----------------------------------------------*
 * 宏定义                                   *
 *----------------------------------------------*/

/* 传感器访问能力定义 */
#define SENSOR_CAP_NOSPECIFIED          0
#define SENSOR_CAP_READABLE             1
#define SENSOR_CAP_READABLE_SETTABLE    2
#define SENSOR_CAP_FIXED                3
 
/* BEGIN: Added by z00355831, 2016/11/08   PN:  DTS2016110801517   */
/* 传感器事件描述信息长度 */
#define EVENT_DESC_LENGTH               1024
/* END:   Added by z00355831, 2016/11/08 */

/* 传感器事件描述重新解析的分隔符 */
#define EVENT_DESC_SPLIT_DATA2_FLG  ("$2")
#define EVENT_DESC_SPLIT_DATA3_FLG  ("$3")

/*----------------------------------------------*
 * 结构体定义                                   *
 *----------------------------------------------*/
/* 定义system类别的SEL格式中的事件格式 */
typedef struct tag_event_msg_s
{
    guint8 event_msg_ver;                
    guint8 sensor_type;
    guint8 sensor_no;

#ifdef BD_BIG_ENDIAN
    guint8 event_dir             : 1;
    guint8 event_type            : 7;
#else
    guint8 event_type            : 7;
    guint8 event_dir             : 1;
#endif

    guint8 event_data_1;
    guint8 event_data_2;
    guint8 event_data_3;
} EVENT_MSG_S;


// 定义system类别的SEL格式
typedef struct tag_syssel_data_s
{
    guint8 generator_id_l;
    guint8 generator_id_h;

    EVENT_MSG_S event_msg;
} SYSSEL_DATA_S;

// 定义SEL格式
typedef struct tag_sel_record_s
{
    guint8 record_id_l;
    guint8 record_id_h;
    guint8 type;

    guint8 time_stamp0;
    guint8 time_stamp1;
    guint8 time_stamp2;
    guint8 time_stamp3;

    SYSSEL_DATA_S syssel_data;
} SEL_RECORD_S;

// 定义SEL+SENSOR_NAME格式
typedef struct tag_sel_sensor_name_s
{    
    gchar  sensor_name[MAX_NAME_SIZE];  /* 传感器名称 */
    SEL_RECORD_S sel_record;            /* SEL事件记录 */
} SEL_SENSOR_NAME_S;


typedef struct tg_SENSOR_STATE_BIT
{
#ifdef BD_BIG_ENDIAN
    guint16 reserved1                       : 4;
    guint16 assert_nonrecoverable_ugh       : 1;    //upper going high
    guint16 assert_nonrecoverable_ugl       : 1;    //upper going low
    guint16 assert_critical_ugh             : 1;    //upper going high
    guint16 assert_critical_ugl             : 1;    //upper going low
    guint16 assert_noncritical_ugh          : 1;    //upper going high
    guint16 assert_noncritical_ugl          : 1;    //upper going low
    guint16 assert_nonrecoverable_lgh       : 1;    //lower going high
    guint16 assert_nonrecoverable_lgl       : 1;    //lower going low
    guint16 assert_critical_lgh             : 1;    //lower going high
    guint16 assert_critical_lgl             : 1;    //lower going low
    guint16 assert_noncritical_lgh          : 1;    //lower going high
    guint16 assert_noncritical_lgl          : 1;    //lower going low
#else
    guint16 assert_noncritical_lgl          : 1;    //lower going low
    guint16 assert_noncritical_lgh          : 1;    //lower going high
    guint16 assert_critical_lgl             : 1;    //lower going low
    guint16 assert_critical_lgh             : 1;    //lower going high
    guint16 assert_nonrecoverable_lgl       : 1;    //lower going low
    guint16 assert_nonrecoverable_lgh       : 1;    //lower going high
    guint16 assert_noncritical_ugl          : 1;    //upper going low
    guint16 assert_noncritical_ugh          : 1;    //upper going high
    guint16 assert_critical_ugl             : 1;    //upper going low
    guint16 assert_critical_ugh             : 1;    //upper going high
    guint16 assert_nonrecoverable_ugl       : 1;    //upper going low
    guint16 assert_nonrecoverable_ugh       : 1;    //upper going high
    guint16 reserved1                       : 4;
#endif
}SENSOR_STATE_BIT_T;
 
typedef struct tg_SENSOR_READINGMASK_BIT
{
#ifdef BD_BIG_ENDIAN

    guint16 reserved4                     : 2;
    guint16 settable_upper_nonrecoverable : 1;
    guint16 settable_upper_critical       : 1;
    guint16 settable_upper_noncritical    : 1;
    guint16 settable_lower_nonrecoverable : 1;
    guint16 settable_lower_critical       : 1;
    guint16 settable_lower_noncritical    : 1;
    guint16 reserved3                     : 2;
    guint16 readable_upper_nonrecoverable : 1;
    guint16 readable_upper_critical       : 1;
    guint16 readable_upper_noncritical    : 1;
    guint16 readable_lower_nonrecoverable : 1;
    guint16 readable_lower_critical       : 1;
    guint16 readable_lower_noncritical    : 1;
#else
    guint16 readable_lower_noncritical    : 1;
    guint16 readable_lower_critical       : 1;
    guint16 readable_lower_nonrecoverable : 1;
    guint16 readable_upper_noncritical    : 1;
    guint16 readable_upper_critical       : 1;
    guint16 readable_upper_nonrecoverable : 1;
    guint16 reserved3                     : 2;
    guint16 settable_lower_noncritical    : 1;
    guint16 settable_lower_critical       : 1;
    guint16 settable_lower_nonrecoverable : 1;
    guint16 settable_upper_noncritical    : 1;
    guint16 settable_upper_critical       : 1;
    guint16 settable_upper_nonrecoverable : 1;
    guint16 reserved4                     : 2;
#endif
} SENSOR_READINGMASK_BIT;

typedef struct tag_event_work_state_s
{
#ifdef BD_BIG_ENDIAN
    guint8 ignore_if_disable               : 1;
    guint8 auto_re_arm                     : 1;
    guint8 initial_update_progress         : 1;
    guint8 disable_access_error            : 1;    //0=disable ,1=enable，这是由硬件访问失败引起的是否禁止扫描
    guint8 disable_override                : 1;
    guint8 disable_scanning_local          : 1;    // 这个是根据本地的实体状态决定传感器是否禁止
    guint8 disable_all                     : 1;    //0=disable ,1=enable，这是由SMM或者SMS发命令禁止上报消息，但未指明是否扫描
    guint8 disable_scanning                : 1;    //0=disable ,1=enable, 这是由SMM或者SMS发命令禁止的
#else
    guint8 disable_scanning                : 1;    //0=disable ,1=enable, 这是由SMM或者SMS发命令禁止的
    guint8 disable_all                     : 1;    //0=disable ,1=enable，这是由SMM或者SMS发命令禁止上报消息，但未指明是否扫描
    guint8 disable_scanning_local          : 1;    // 这个是根据本地的实体状态决定传感器是否禁止
    guint8 disable_override                : 1;
    guint8 disable_access_error            : 1;    //0=disable ,1=enable，这是由硬件访问失败引起的是否禁止扫描
    guint8 initial_update_progress         : 1;
    guint8 auto_re_arm                     : 1;
    guint8 ignore_if_disable               : 1;
#endif
} EVENT_WORK_STATE_S;

typedef struct tag_sensor_capability_s
{
#ifdef BD_BIG_ENDIAN
    guint8 ignore_support                : 1;
    guint8 auto_rearm_support            : 1;
    guint8 hysteresis_support            : 2;
    guint8 threshold_access_support      : 2;
    guint8 event_msg_support             : 2;
#else
    guint8 event_msg_support             : 2;
    guint8 threshold_access_support      : 2;
    guint8 hysteresis_support            : 2;
    guint8 auto_rearm_support            : 1;
    guint8 ignore_support                : 1;
#endif
} SENSOR_CAPABILITY_S;


/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern gint32 uip_remake_event_desc(const EVENT_MSG_S* event_msg, gchar* event_desc);
extern gint32 uip_get_component_info(guint8 type, guint32* present_count, guint32* component_count, guint32* health);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __UIP_SENSOR_ALARM_H__ */



