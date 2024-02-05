/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : public_sdr.h
  版 本 号   : 初稿
  部    门   : BMC
  作    者   : wangpenghui wwx382232
  生成日期   : 2018年4月17日
  最近修改   :
  功能描述   : SDR公共头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2018年4月17日
    作    者   : wangpenghui wwx382232
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#ifndef __PUBLIC_SDRDEV_H__
#define __PUBLIC_SDRDEV_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


/* BEGIN: Modified by wangpenghui wwx382232, 2018/4/16 10:23:5   问题单号:DTS2018041600753 */

/* SDR的最大长度，必须比各类SDR的长度长 */
#define SDR_MAX_LEN                 64

/* SDR名字的长度值 */
#define SDR_NAME_LENGTH             16

#define SDR_TYPE_THRESHOLD_SENSOR 1

#pragma pack(1)     /* 强制单字节对齐 */


/*sdr type = 01h Full Sensor Record*/
typedef struct tag_sdr_type01_s
{
    /*record key bytes*/
    guint8  owner_id;           //bit7:1 ipmb slave addr, bit0 not used
    guint8  owner_lun;          //bit7:4 channel num, bit3:0 not used there
    guint8  sensor_num;         //Unique number identifying the sensor

    /*record body bytes*/
    guint8  entity_id;          //not used
    guint8  entity_instance;    //not used
    guint8  sensor_init;        //sensor initialization state
    guint8  sensor_cap;         //sensor capabilities
    guint8  sensor_type;        //sensor type: temp=0x01 volt=0x02..
    guint8  event_reading_type_code;
    guint16 assert_lower_reading_mask;      // 原命名 assertion_event_lower_threshold_reading_mask
    guint16 deassert_upper_reading_mask;    //  原命名 deassertion_event_upper_threshold_reading_mask
    guint16 discrete_reading_mask;  //  原命名 discrete_reading_settable_readable_threshold_mask;
    guint8  sensor_units_1;
    guint8  sensor_units_2;
    guint8  sensor_units_3;
    guint8  linearization;
    guint8  M;
    guint8  M_tolerance;
    guint8  B;
    guint8  B_accuracy;
    guint8  accuracy_exp;
    guint8  R_B_exp;
    guint8  analog_flags;
    guint8  nominal_reading;
    guint8  normal_max;
    guint8  normal_min;
    guint8  max_reading;
    guint8  min_reading;
    guint8  upper_non_recoverable_threshold;
    guint8  upper_critical_threshold;
    guint8  upper_non_critical_threshold;
    guint8  lower_non_recoverable_threshold;
    guint8  lower_critical_threshold;
    guint8  lower_non_critical_threshold;
    guint8 positive_threshold_hysteresis;   //  原命名 positive_going_threshold_hysteresis
    guint8 negative_threshold_hysteresis;   //  原命名 negative_going_threshold_hysteresis
    guint8  reserved1;
    guint8  reserved2;
    guint8  oem;
    guint8  id_string_type_len;
    gchar   id_string[SDR_NAME_LENGTH];
} SDR_TYPE01_S;

/*sdr type = 02h Compact Sensor Record*/
typedef struct tag_sdr_type02_s
{
    /*record key bytes*/
    guint8  owner_id;           //bit7:1 ipmb slave addr, bit0 not used
    guint8  owner_lun;          //bit7:4 channel num, bit3:0 not used there
    guint8  sensor_num;         //Unique number identifying the sensor

    /*record body bytes*/
    guint8  entity_id;          //not used
    guint8  entity_instance;    //not used
    guint8  sensor_init;        //sensor initialization state
    guint8  sensor_cap;         //sensor capabilities
    guint8  sensor_type;        //sensor type: temp=0x01 volt=0x02..
    guint8  event_reading_type_code;
    guint16 assert_lower_reading_mask;      // 原命名 assertion_event_lower_threshold_reading_mask
    guint16 deassert_upper_reading_mask;    //  原命名 deassertion_event_upper_threshold_reading_mask
    guint16 discrete_reading_mask;  //  原命名 discrete_reading_settable_readable_threshold_mask;
    guint8  sensor_units_1;
    guint8  sensor_units_2;
    guint8  sensor_units_3;
    guint16 Sensor_Record_Sharing;
    guint8 positive_threshold_hysteresis;   //  原命名 positive_going_threshold_hysteresis
    guint8 negative_threshold_hysteresis;   //  原命名 negative_going_threshold_hysteresis
    guint8  reserved1;
    guint8  reserved2;
    guint8  reserved3;
    guint8  oem;
    guint8  id_string_type_len;
    gchar   id_string[SDR_NAME_LENGTH];
} SDR_TYPE02_S;

/*Entity Association Record - SDR Type 08h*/
typedef struct tag_sdr_type08_s
{
    /*record key bytes*/
    guint8  container_entity_id;
    guint8  container_entity_instance;
    guint8  flags;
    guint8  entity1_id;
    guint8  entity1_instance;
    guint8  entity2_id;
    guint8  entity2_instance;
    guint8  entity3_id;
    guint8  entity3_instance;
    guint8  entity4_id;
    guint8  entity4_instance;
} SDR_TYPE08_S;

typedef struct tag_sdr_type09_s
{
    /*record key bytes*/
    guint8  container_entity_id;
    guint8  container_entity_instance;
    guint8  container_entity_devaddr;
    guint8  container_entity_devchannel;
    guint8  flags;

    guint8  contained_entity1_devaddr;
    guint8  contained_entity1_devchannel;
    guint8  entity1_id;
    guint8  entity1_instance;

    guint8  contained_entity2_devaddr;
    guint8  contained_entity2_devchannel;
    guint8  entity2_id;
    guint8  entity2_instance;

    guint8  contained_entity3_devaddr;
    guint8  contained_entity3_devchannel;
    guint8  entity3_id;
    guint8  entity3_instance;

    guint8  contained_entity4_devaddr;
    guint8  contained_entity4_devchannel;
    guint8  entity4_id;
    guint8  entity4_instance;
} SDR_TYPE09_S;

/*sdr type = 10h Generic Device Locator Record*/
typedef struct tag_sdr_type10_s
{
#ifdef BD_BIG_ENDIAN
    /*record key bytes*/
    guint8  device_access_addr    : 7;    //msb at first
    guint8  reserved1             : 1;

    guint8  device_slave_addr     : 7;
    guint8  chan_bit3             : 1;

    guint8  chan_bit0_2           : 3;
    guint8  LUN                   : 2;
    guint8  pri_bus_id            : 3;
#else
    guint8  reserved1             : 1;
    guint8  device_access_addr    : 7;

    guint8  chan_bit3             : 1;
    guint8  device_slave_addr     : 7;

    guint8  pri_bus_id            : 3;
    guint8  LUN                   : 2;
    guint8  chan_bit0_2           : 3;
#endif

    /*record body bytes*/
    guint8  addr_span;
    guint8  reserved2;
    guint8  device_type;
    guint8  device_type_modifier;
    guint8  entity_id;
    guint8  entity_instance;
    guint8  oem;
    guint8  id_string_type_len;
    gchar   id_string[SDR_NAME_LENGTH];
} SDR_TYPE10_S;

/*sdr type = 11h FRU Device Locator Record*/
typedef struct tag_sdr_type11_s
{
    /*record key bytes*/
    guint8  device_access_addr;
    guint8  fru_device_id_device_slave_addr;

#ifdef BD_BIG_ENDIAN
    guint8  logical_device : 1;
    guint8  reserved       : 2;
    guint8  lun            : 2;
    guint8  prviate_bus_id : 3;
#else
    guint8  prviate_bus_id : 3;
    guint8  lun            : 2;
    guint8  reserved       : 2;
    guint8  logical_device : 1;
#endif

    // guint8 logical_physical_access_LUN_bus_id;
    guint8  channel_num;

    /*record body bytes*/
    guint8  reserved1;
    guint8  device_type;
    guint8  device_type_modifier;
    guint8  fru_entity_id;
    guint8  fru_entity_instance;
    guint8  oem;
    guint8  id_string_type_len;
    gchar   id_string[SDR_NAME_LENGTH];
} SDR_TYPE11_S;

/*sdr type = 12h Management Controller Device Locator Record*/
typedef struct tag_sdr_type12_s
{
    /*record key bytes*/
    guint8  device_slave_addr;
    guint8  channel_num;

    /*record body bytes*/
    guint8  power_state_notification_global_initialization;
    guint8  device_cap;
    guint8  reserved1;
    guint8  reserved2;
    guint8  reserved3;
    guint8  entity_id;
    guint8  entity_instance;
    guint8  oem;
    guint8  id_string_type_len;
    gchar   id_string[SDR_NAME_LENGTH];
} SDR_TYPE12_S;


/*sdr type = 13h Management Controller Confirmation Record*/
typedef struct tag_sdr_type13_s
{
    /*record key bytes*/
    guint8  device_slave_addr;
    guint8  device_id;
    guint8  channel_num_device_revision;

    /*record body bytes*/
    guint8  firmware_revision_1;
    guint8  firmware_revision_2;
    guint8  ipmi_revision;
    guint8  manufacturer_id[3];
    guint8  product_id[2];
    guint8  device_guid[16];
    guint8  reserved1;
    guint8  reserved2;
    guint8  reserved3;
    guint8  entity_id;
    guint8  entity_instance;
    guint8  oem;
    guint8  id_string_type_len;
    gchar   id_string[SDR_NAME_LENGTH];
} SDR_TYPE13_S;

/*sdr type = 14h BMC Message Channel Info Record*/
typedef struct tag_sdr_type14_s
{
    /*record body bytes*/
    guint8  message_channel_0_info;
    guint8  message_channel_1_info;
    guint8  message_channel_2_info;
    guint8  message_channel_3_info;
    guint8  message_channel_4_info;
    guint8  message_channel_5_info;
    guint8  message_channel_6_info;
    guint8  message_channel_7_info;
    guint8  message_interrupt_type;
    guint8  event_message_buffer_interrupt_type;
    guint8  reserved1;
} SDR_TYPE14_S;
/* BEGIN: Added by h00191524, 2014/11/13   PN:DTS2014102206334*/
/*注释掉rec_sub_type成员，扩展oem_data使之符合ipmi 2.0规范*/
typedef struct tag_sdr_typec0_s
{
    guint8 man_id_0;               // Manufacturer ID low byte, huawei is 0xdb
    guint8 man_id_1;               // Manufacturer ID mid byte, huawei is 0x07
    guint8 man_id_2;               // Manufacturer ID high byte, huawei is 0x00
    //guint8 rec_sub_type;           // sdr record sub type
    guint8 oem_data[56];
} SDR_TYPEC0_S;
/* END:   Added by h00191524, 2014/11/13 */

typedef union tag_sdr_data_s
{
    SDR_TYPE01_S   sdr_type_01;
    SDR_TYPE02_S   sdr_type_02;
    SDR_TYPE08_S   sdr_type_08;
    SDR_TYPE09_S   sdr_type_09;
    SDR_TYPE10_S   sdr_type_10;
    SDR_TYPE11_S   sdr_type_11;
    SDR_TYPE12_S   sdr_type_12;
    SDR_TYPE13_S   sdr_type_13;
    SDR_TYPE14_S   sdr_type_14;
    SDR_TYPEC0_S   sdr_type_c0;
} SDR_DATA_S;

typedef struct tag_sdr_record_s
{
    guint8     record_id_l;     // 传感器记录ID低字节
    guint8     record_id_h;     // 传感器记录ID高字节
    guint8     version;         // SDR 版本
    guint8     type;            // 记录类型
    guint8     len;             // 记录长度
    SDR_DATA_S sdr_data;        // SDR 实际数据存储区
} SDR_RECORD_S;

/* 请求: IPMI_GET_DEVICE_SDR命令字结构体 */
typedef struct tag_ipmimsg_get_sdr_s
{
    guint8 reserve_id_l;
    guint8 reserve_id_h;
    guint8 record_id_l;
    guint8 record_id_h;
    guint8 offset;
    guint8 read_len;
} IPMIMSG_GET_SDR_S;


/* 响应: IPMI_GET_DEVICE_SDR命令字结构体 */
typedef struct tag_ipmimsg_get_sdr_resp_s
{
    guint8 comp_code;
    guint8 next_record_id_l;
    guint8 next_record_id_h;
    guint8 buf[SDR_MAX_LEN];
} IPMIMSG_GET_SDR_RESP_S;


/* 响应: IPMI_RESERVE_DEVICE_SDR_REPOSITORY命令字结构体 */
typedef struct tag_ipmimsg_reverse_sdr_repository_resp_s
{
    guint8 comp_code;
    guint8 reserve_id_l;
    guint8 reserve_id_h;
} IPMIMSG_REVERSE_SDR_REPOSITORY_RESP_S;


/* 响应: IPMI_GET_SDR_REPOSITORY_INFO命令字结构体 */
typedef struct tag_ipmimsg_get_sdr_repository_info_resp_s
{
    guint8 comp_code;
    guint8 sdr_ver;

    guint8 rec_cnt_l;
    guint8 rec_cnt_h;

    guint8 free_space_l;
    guint8 free_space_h;

    guint8 add_timestamp0;
    guint8 add_timestamp1;
    guint8 add_timestamp2;
    guint8 add_timestamp3;

    guint8 del_timestamp0;
    guint8 del_timestamp1;
    guint8 del_timestamp2;
    guint8 del_timestamp3;

    guint8 op_support;
} IPMIMSG_GET_SDR_REPOSITORY_INFO_RESP_S;


/* 响应: IPMI_GET_SDR_REPOSITORY_ALLOCATION_INFO命令字结构体 */    
typedef struct tag_ipmimsg_get_sdr_repository_alloc_info_resp_s
{
    guint8 comp_code;

    guint8 unit_cnt_l;
    guint8 unit_cnt_h;

    guint8 unit_size_l;
    guint8 unit_size_h;

    guint8 free_unit_l;
    guint8 free_unit_h;

    guint8 free_block_l;
    guint8 free_block_h;

    guint8 max_unit_size;
} IPMIMSG_GET_SDR_REPOSITORY_ALLOC_INFO_RESP_S;


/* 响应: IPMI_GET_DEVICE_SDR_INFO命令字结构体 */
typedef struct tag_ipmimsg_get_device_sdr_info_resp_s
{
    guint8 comp_code;
    guint8 cnt;

#ifdef BD_BIG_ENDIAN
    guint8 dynamic_sensor  :1;    // msb at first
    guint8 reserved1       :3;
    guint8 lun             :4;
#else
    guint8 lun             :4;
    guint8 reserved1       :3;
    guint8 dynamic_sensor  :1;    // msb at first
#endif
    guint32 population;
} IPMBMSG_GET_DEVICE_SDR_INFO_RESP_S;


/* 请求:  IPMI_PARTIAL_ADD_SDR命令字结构体 */
typedef struct tag_ipmimsg_partial_add_sdr_s
{
    guint8 reserve_id_l;
    guint8 reserve_id_h;
    guint8 record_id_l;
    guint8 record_id_h;
    guint8 offset;

#ifdef BD_BIG_ENDIAN
    guint8 reserved           : 4;
    guint8 in_progress        : 4;
#else
    guint8 in_progress        : 4;
    guint8 reserved           : 4;
#endif

    guint8 buf[1];
} IPMIMSG_PARTIAL_ADD_SDR_S;


/* 响应： IPMI_PARTIAL_ADD_SDR命令字结构体 */
typedef struct tag_ipmimsg_partial_add_sdr_resp_s
{
    guint8 comp_code;
    guint8 record_id_l;
    guint8 record_id_h;
} IPMIMSG_PARTIAL_ADD_SDR_RESP_S;


/* 请求: IPMI_CLEAR_SDR_REPOSITORY命令字结构体 */
typedef struct tag_ipmimsg_clr_sdr_s
{
    guint8 reserve_id_l;
    guint8 reserve_id_h;
    guint8 flag_C;
    guint8 flag_L;
    guint8 flag_R;
    guint8 op;
} IPMIMSG_CLR_SDR_S;

/* 响应： IPMI_CLEAR_SDR_REPOSITORY命令字结构体 */
typedef struct tag_ipmimsg_clr_sdr_resp_s
{
    guint8 comp_code;

#ifdef BD_BIG_ENDIAN
    guint8 reserved : 4;
    guint8 status   : 4; // msb at first
#else
    guint8 status   : 4;
    guint8 reserved : 4;
#endif
} IPMIMSG_CLR_SDR_RESP_S;

/* 响应： IPMI_GET_SDR_REPOSITORY_TIME命令字结构体 */
typedef struct tag_ipmimsg_get_sdr_time_resp_s 
    {
    guint8 comp_code;
    guint8 t0;         // low byte
    guint8 t1;
    guint8 t2;
    guint8 t3;
} IPMIMSG_GET_SDR_TIME_RESP_S;


#pragma pack()     /*恢复字节对齐方式 */
/* END:   Modified by wangpenghui wwx382232, 2018/4/16 10:23:10 */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __PUBLIC_SDRDEV_H__ */