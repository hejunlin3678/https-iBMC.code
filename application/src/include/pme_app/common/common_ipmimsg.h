/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : common_ipmimsg.h
  版 本 号   : 初稿
  作    者   : d00384307
  生成日期   : 2019年2月19日
  最近修改   :
  功能描述   : IPMI消息结构体头文件

该文件原则上是用来放置跨模块使用的ipmi消息结构体

  1.日    期   : 2019年2月19日
    作    者   : d00384307
    修改内容   : 创建文件

******************************************************************************/
#ifndef __COMMON_IPMIMSG_H__
#define __COMMON_IPMIMSG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif      /* __cplusplus */

#define IPMI_SUBCMD_GET_HEALTH_EVENT 0x37
#define IPMI_CMD_BMC_CONFIG_DATA_LEN_OFFSET1 7  // 小端，高位字节
#define IPMI_CMD_BMC_CONFIG_DATA_LEN_OFFSET2 6  // 小端，低位字节
#define IPMI_CMD_BMC_CONFIG_DATA_OFFSET 8

#pragma pack(1)
typedef struct _tag_event_ipmi_rearm_req
{
    guint8 huawei_iana_id[3];        // 0x0007DB, LSB first
    guint8 sub_cmd;
    guint8 reserved[4];
} EVENT_IPMI_REARM_REQ;

typedef struct _tag_event_ipmi_rearm_rsp
{
    guint8 completion_code;
    guint8 huawei_iana_id[3];        // 0x0007DB, LSB first
} EVENT_IPMI_REARM_RSP;

typedef struct _tag_event_ipmi_get_entry_req
{
    guint8  huawei_iana_id[3];        // 0x0007DB, LSB first
    guint8  sub_cmd;
    guint8  event_type;
    guint8  event_severity;
    guint8  event_format;
    guint16 record_id;
    guint16 read_offset;
    guint8  read_length;
} EVENT_IPMI_GET_ENTRY_REQ;

#define EVENT_TIMESTAMP_BUFFER_LENGTH 20
typedef struct _tag_event_entry
{
    guint32 new_event_code;
    guint32 old_event_code;
    guint8  event_severity;
    guint8  event_trigger_mode;
    gchar   event_timestamp[EVENT_TIMESTAMP_BUFFER_LENGTH];
    guint8  event_content[1];
} EVENT_ENTRY;
#define EVENT_ENTRY_HEADER_SIZE (sizeof(EVENT_ENTRY) - sizeof(guint8))

typedef struct _tag_event_ipmi_get_entry_rsp
{
    guint8  completion_code;
    guint8  huawei_iana_id[3];        // 0x0007DB, LSB first
    guint16 next_record_id;
#ifdef BD_BIG_ENDIAN
    guint8  reserved: 7;
    guint8  cascading: 1;
#else
    guint8  cascading: 1;
    guint8  reserved: 7;
#endif
    guint8  reserved_byte;
    EVENT_ENTRY  event;
} EVENT_IPMI_GET_ENTRY_RSP;
#define EVENT_IPMI_GET_ENTRY_RSP_HEADER_SIZE (sizeof(EVENT_IPMI_GET_ENTRY_RSP) - sizeof(EVENT_ENTRY))

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif  /* __cplusplus */

#endif  /* __COMMON_IPMIMSG_H__ */
