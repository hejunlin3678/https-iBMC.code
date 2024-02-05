/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 系统事件相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */


#ifndef __EVENT_DEFINE_H__
#define __EVENT_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MAX_DESC_PARA_NUM 6
#define MAX_SEL_TIME_LEN 256
#define MAX_ADINFO_LENGTH 256
#define MAX_EVENT_SUBJECT_LEN 256

#define EVENT_LANG_VER_FORMAT "%d.%02d.%02d"

#define EVENT_TYPE_SYS 0     // 系统事件
#define EVENT_TYPE_MAINT 1   // 维护事件
#define EVENT_TYPE_RUNNING 2 // 运行事件
#define EVENT_TYPE_DFT 3     // 装备事件

#define EVENT_POLICY_DEFAULT_CYCLE 1000 // 默认1000ms扫描周期
#define EVENT_POLICY_LISTEN_TYPE 0      // 侦听型

#define EVENT_MONITOR_MODE_HIGH_BIT_MASK 0xF0 // mode 的高4bit掩码
#define EVENT_MONITOR_MODE_LOW_BIT_MASK 0x0F  // mode 的低4bit掩码

/* Mode的低4bit定义 */
#define EVENT_MONITOR_MODE_NONE 0    // 不监控
#define EVENT_MONITOR_MODE_AUTO 1    // 本地自动扫描(正常监控)
#define EVENT_MONITOR_MODE_MANUAL 2  // 手动模拟
#define EVENT_MONITOR_MODE_RECOVER 3 // 手动触发恢复事件
#define EVENT_MONITOR_MODE_THIRD 5   // 第三方控制(State属性由本地或BMC外部模块直接更新，监控任务不更新)
#define EVENT_MONITOR_MODE_CONFIRM 6 // 手动确认
#define EVENT_MONITOR_MODE_MAX_NUM 7

/* Mode的高4bit+低4bit组合定义 */
#define EVENT_MONITOR_MODE_REARM 0x10          // rearm(重复上报)(收到重报命令重报)
#define EVENT_MONITOR_MODE_HEALTH_RECOVER 0x20 // 健康事件恢复
#define EVENT_MONITOR_MODE_TEST 0x30           // 测试事件，复用触发模式变量
#define EVENT_MONITOR_MANUAL_AND_REARM 0x12    // 模拟+重报

#define EVENT_MONITOR_TRIGMODE_LEN 35

#define OPERATOR_LT   1                                    // <
#define OPERATOR_LE   2                                    // <=
#define OPERATOR_GT   3                                    // >
#define OPERATOR_GE   4                                    // >=
#define OPERATOR_EQ   5                                    // =
#define OPERATOR_NE   6                                    // !=
#define EVENT_MONITOR_OPERATOR_RISING_EDGE 7               // 0 -> 1
#define EVENT_MONITOR_OPERATOR_FALLING_EDGE 8              // 1 -> 0
#define EVENT_MONITOR_OPERATOR_FAILLING_EDGE_RECOVERABLE 9 /* 1->0 产生；0->1恢复 */

#define EVENT_RESOURCE_HMM_STANDBY 1

#define EVENT_OBJ_STATE_UNKNOWN 0xFF      // 未知状态
#define EVENT_OBJ_STATE_IGNORE_FIRST 0xFE // 忽略第一次
#define EVENT_OBJ_STATE_ASSERT 1          // 产生告警
#define EVENT_OBJ_STATE_DEASSERT 0        // 消除告警

#define EVENT_OBJ_CAN_NOT_DEASSERT 0 //  不能恢复
#define EVENT_OBJ_CAN_DEASSERT 1     //  能恢复

#define EVENT_LANGUAGE_FILE "event_lang_template_v2.xml"
#define EVENT_LANGUAGE_FILE_PATH "/data/opt/pme/conf/language"
#define EVENT_LANGUAGE_DEFAULT_NODE_CONTENT "N/A"
#define EVENT_LANGUAGE_MAJOR_VER_NODE "MAJOR_VERSION"
#define EVENT_LANGUAGE_MINOR_VER_NODE "MINOR_VERSION"
#define EVENT_LANGUAGE_AUX_VER_NODE "AUX_VERSION"
#define EVENT_LANGUAGE_LANG_NODE "LanguageSet"
#define EVENT_LANGUAGE_DESC_NODE "EventDescription"
#define EVENT_LANGUAGE_SUGG_NODE "EventSuggestion"

#define EVENT_LANGUAGE_ITEM_NAME_PREFIX "lang_"
#define EVENT_LANGUAGE_ITEM_NAME_SUFFIX "_str"
#define EVENT_LANGUAGE_CONTENT_MAX_LENGTH 1024
#define EVENT_EXT_INFO_SPLIT_CHAR 0xBA
#define EVENT_OLD_OR_NEW_FLAG "event_old"
#define XPATH_STRING_MAX_LENGTH 256

#define EVENT_LANGUAGE_EN "en" // 英文
#define EVENT_LANGUAGE_ZH "zh" // 中文
#define EVENT_LANGUAGE_JP "jp" // 日语
#define EVENT_LANGUAGE_DE "de" // 德语
#define EVENT_LANGUAGE_FR "fr" // 法语

#define LANGUAGE_NULL_OR_EMPTY (-2001)
#define LANGUAGE_ONLY_ONE_LANGUAGE (-2002)
#define LANGUAGE_NO_UNINSTALLED_ZH_OR_EN (-2003)
#define LANGUAGE_NOT_INSTALLED (-2004)
#define LANGUAGE_NOT_SUPPORT (-2005)

#define EVENT_PARAM_OBJECT_TYPE_ID "ObjectTypeId"
#define EVENT_PARAM_OBJECT_TYPE_LABEL "ObjectTypeLabel"
#define EVENT_PARAM_LANG_TYPE "langType"
#define EVENT_PARAM_SEL_LIMIT "selLimit"
#define EVENT_PARAM_SEL_LAST_ID "selLastId"
#define EVENT_PARAM_SEL_SEARCH_STR "selSearchStr"
#define EVENT_PARAM_SEL_LEVEL "selLevel"
#define EVENT_PARAM_SEL_ENTITY "selEntity"
#define EVENT_PARAM_SEL_BEGIN_TIME "selBeginTime"
#define EVENT_PARAM_SEL_END_TIME "selEndTime"

#define EVENT_ERR_SEL_LIMIT_INVALID (-4001)
#define EVENT_ERR_SEL_LAST_ID_INVALID (-4002)
#define EVENT_ERR_SEL_SEARCH_STR_INVALID (-4003)
#define EVENT_ERR_SEL_LEVEL_INVALID (-4004)
#define EVENT_ERR_SEL_ENTITY_INVALID (-4005)
#define EVENT_ERR_SEL_BEGIN_TIME_INVALID (-4006)
#define EVENT_ERR_SEL_END_TIME_INVALID (-4007)
#define EVENT_ERR_SEL_OFFSET_INVALID (-4008)

#define INVALID_EVENT_RECORD_ID 0
#define INVALID_EVENT_CODE_VAL 0x8001     // 不合理的eventcode
#define INVALID_SUBJECT_INDEX_VAL 0x8002  // 不合理的subjectindex值
#define INVALID_INPUT_PARAMETER 0x8003    // 输入参数错误
#define INTERNAL_PROCESS_ERROR 0x8004     // 内部处理错误
#define SUBJECT_EVENT_OUT_OF_RANGE 0x8005 // 事件主体类型超出定义范围
#define INTERNAL_PROCESS_QUEUE_BUSY 0x8006 // 正在处理告警模式任务


#define PARAM_FLAG "flag"
#define PARAM_DEFAULT_VALUE 255
#define PARAM_DEFAULT_VALUE_ZERO 0
#define PARAM_DEFAULT_VALUE_EMPTY_STR ""

#define EVENT_ALARM_LEVEL_NUM 4
#define EVENT_SEVERITY_CRITICAL 3 // 紧急
#define EVENT_SEVERITY_MAJOR 2    // 严重
#define EVENT_SEVERITY_MINOR 1    // 轻微
#define EVENT_SEVERITY_NORMAL 0   // 正常
#define EVENT_SEVERITY_ALL 255    // 主体类型(全部)

#define SUBJECT_TYPE_UNKNOWN "UNKNOWN" // 超出BMC事件主体类型定义范围


#define MSG_ARG_DELIMITER ";;"
#define EVT_SUBJECT_DELIMITER "--"
#define EO_LOG_SEL_CSV_TAR_NAME ("/tmp/sel.tar")

#define EVT_CODE_COMP_TYPE_BIT_OFFSET 24

#define EVT_CHANNEL_FILTER_SNMP_TRAP (BIT(2))
#define EVT_CHANNEL_FILTER_SMTP (BIT(3))
#define EVT_CHANNEL_SYSLOG (BIT(4))
#define EVT_CHANNEL_RESTFUL (BIT(5))
#define EVT_CHANNEL_ALL 0xff
#define REARM_SEQ_ALL 0

#define SEL_SQLITE_DB_FILE ("/opt/pme/pram/sel.db")
#define SEL_SQLITE_SYNC_FILE ("/opt/pme/pram/sel_db_sync")
#define SEL_STR_MAXLENGTH 350

#define EO_SQLITE_DB_FILE ("/opt/pme/pram/eo.db")
#define EO_SQLITE_SYNC_FILE ("/opt/pme/pram/eo_db_sync")
#define EO_SQLITE_SYNC_FILE_RIGHT (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define EO_SQLITE_TABLE_NAME ("event_table")
#define EO_SQLITE_MONITOR_TABLE_NAME ("monitor_table")

#define EVENT_OFFSET_VAL 20               // 每次请求的数据个数
#define COMPONENT_SPECIAL_DEVICE_NUM 0xFE // 特殊用途的component device num 起始号

typedef enum _tag_event_extern_info_type {
    EVENT_EXT_DESCRIPTION,
    EVENT_EXT_SUGGESTION
} EVENT_EXT_INFO_TYPE;

typedef struct event_monitor_struct {
    gchar MonitorName[OBJ_NAME_LENGTH + 1];
    gchar SubjectObj[OBJ_NAME_LENGTH + 1];
    gchar InformationObj[OBJ_NAME_LENGTH + 1];
    gchar PolicyObj[OBJ_NAME_LENGTH + 1];
    gchar SensorObj[OBJ_NAME_LENGTH + 1];
    gchar EventFRU[OBJ_NAME_LENGTH + 1];
    gchar Mode[OBJ_NAME_LENGTH + 1];
    gchar SavedMode[OBJ_NAME_LENGTH + 1];
    gchar Time[OBJ_NAME_LENGTH + 1];
    gchar State[OBJ_NAME_LENGTH + 1];
    gchar ScannedCount[OBJ_NAME_LENGTH + 1];
    gchar Reading[PROP_VAL_LENGTH + 1];
    gchar SavedReading[OBJ_NAME_LENGTH + 1];
    gchar ReadingMask[OBJ_NAME_LENGTH + 1];
    gchar Threshold[PROP_VAL_LENGTH + 1];
    gchar Hysteresis[OBJ_NAME_LENGTH + 1];
    gchar Operator[OBJ_NAME_LENGTH + 1];
    gchar AddInfo[MAX_DESC_PARA_NUM][PROP_VAL_LENGTH + 1];
    gchar DescriptionDynamic[MAX_DESC_PARA_NUM][PROP_VAL_LENGTH + 1];
    gchar SuggestionDynamic[MAX_DESC_PARA_NUM][PROP_VAL_LENGTH + 1];
} EVENT_MONITOR_INFO;

#pragma pack(1)
typedef struct tag_msg_packet { // 用于模块间传递数据时封装（平台事件转发到SMM_MGNT/告警事件转发到SMM_MGNT）
    guint8 msg_type;
    guint8 addr;
    guint32 data_length;
    guint8 data[1];
} MSG_PACKET;
#pragma pack()

#define MSG_PACKET_HEADER_LENGTH (sizeof(MSG_PACKET) - 1)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __EVENT_DEFINE_H__ */
