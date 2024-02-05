#ifndef __UIP_SENSOR_ALARM_H__
#define __UIP_SENSOR_ALARM_H__

#include <glib.h>

typedef struct tag_event_work_state_s {
#ifdef BD_BIG_ENDIAN
    guint8 ignore_if_disable : 1;
    guint8 auto_re_arm : 1;
    guint8 initial_update_progress : 1;
    // 0=disable,1=enable，这是由硬件访问失败引起的是否禁止扫描
    guint8 disable_access_error : 1;
    guint8 disable_override : 1;
    // 这个是根据本地的实体状态决定传感器是否禁止
    guint8 disable_scanning_local : 1;
    // 0=disable,1=enable，这是由SMM或者SMS发命令禁止上报消息，但未指明是否扫描
    guint8 disable_all : 1;
    // 0=disable ,1=enable,这是由SMM或者SMS发命令禁止的
    guint8 disable_scanning : 1;
#else
    // 0=disable ,1=enable,这是由SMM或者SMS发命令禁止的
    guint8 disable_scanning : 1;
    // 0=disable,1=enable，这是由SMM或者SMS发命令禁止上报消息，但未指明是否扫描
    guint8 disable_all : 1;
    // 这个是根据本地的实体状态决定传感器是否禁止
    guint8 disable_scanning_local : 1;
    guint8 disable_override : 1;
    // 0=disable,1=enable，这是由硬件访问失败引起的是否禁止扫描
    guint8 disable_access_error : 1;
    guint8 initial_update_progress : 1;
    guint8 auto_re_arm : 1;
    guint8 ignore_if_disable : 1;
#endif
} EVENT_WORK_STATE_S;

typedef struct tg_SENSOR_READINGMASK_BIT {
#ifdef BD_BIG_ENDIAN
    guint16 reserved4 : 2;
    guint16 settable_upper_nonrecoverable : 1;
    guint16 settable_upper_critical : 1;
    guint16 settable_upper_noncritical : 1;
    guint16 settable_lower_nonrecoverable : 1;
    guint16 settable_lower_critical : 1;
    guint16 settable_lower_noncritical : 1;
    guint16 reserved3 : 2;
    guint16 readable_upper_nonrecoverable : 1;
    guint16 readable_upper_critical : 1;
    guint16 readable_upper_noncritical : 1;
    guint16 readable_lower_nonrecoverable : 1;
    guint16 readable_lower_critical : 1;
    guint16 readable_lower_noncritical : 1;
#else
    guint16 readable_lower_noncritical : 1;
    guint16 readable_lower_critical : 1;
    guint16 readable_lower_nonrecoverable : 1;
    guint16 readable_upper_noncritical : 1;
    guint16 readable_upper_critical : 1;
    guint16 readable_upper_nonrecoverable : 1;
    guint16 reserved3 : 2;
    guint16 settable_lower_noncritical : 1;
    guint16 settable_lower_critical : 1;
    guint16 settable_lower_nonrecoverable : 1;
    guint16 settable_upper_noncritical : 1;
    guint16 settable_upper_critical : 1;
    guint16 settable_upper_nonrecoverable : 1;
    guint16 reserved4 : 2;
#endif
} SENSOR_READINGMASK_BIT;

typedef struct tg_SENSOR_STATE_BIT {
#ifdef BD_BIG_ENDIAN
    guint16 reserved1                       : 4;
    guint16 assert_nonrecoverable_ugh       : 1;
    guint16 assert_nonrecoverable_ugl       : 1;
    guint16 assert_critical_ugh             : 1;
    guint16 assert_critical_ugl             : 1;
    guint16 assert_noncritical_ugh          : 1;
    guint16 assert_noncritical_ugl          : 1;
    guint16 assert_nonrecoverable_lgh       : 1;
    guint16 assert_nonrecoverable_lgl       : 1;
    guint16 assert_critical_lgh             : 1;
    guint16 assert_critical_lgl             : 1;
    guint16 assert_noncritical_lgh          : 1;
    guint16 assert_noncritical_lgl          : 1;
#else
    guint16 assert_noncritical_lgl          : 1;
    guint16 assert_noncritical_lgh          : 1;
    guint16 assert_critical_lgl             : 1;
    guint16 assert_critical_lgh             : 1;
    guint16 assert_nonrecoverable_lgl       : 1;
    guint16 assert_nonrecoverable_lgh       : 1;
    guint16 assert_noncritical_ugl          : 1;
    guint16 assert_noncritical_ugh          : 1;
    guint16 assert_critical_ugl             : 1;
    guint16 assert_critical_ugh             : 1;
    guint16 assert_nonrecoverable_ugl       : 1;
    guint16 assert_nonrecoverable_ugh       : 1;
    guint16 reserved1                       : 4;
#endif
}SENSOR_STATE_BIT_T;

typedef struct tag_sensor_capability_s {
#ifdef BD_BIG_ENDIAN
    guint8 ignore_support : 1;
    guint8 auto_rearm_support : 1;
    guint8 hysteresis_support : 2;
    guint8 threshold_access_support : 2;
    guint8 event_msg_support : 2;
#else
    guint8 event_msg_support : 2;
    guint8 threshold_access_support : 2;
    guint8 hysteresis_support : 2;
    guint8 auto_rearm_support : 1;
    guint8 ignore_support : 1;
#endif
} SENSOR_CAPABILITY_S;

#endif /* __UIP_SENSOR_ALARM_H__ */
