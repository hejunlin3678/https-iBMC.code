/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2021. All rights reserved.
 * Description: 物理盘相关对外提供的数据结构和接口
 * Author: 汪军锋
 * Create: 2021-2-9
 * Notes: 无
 * History: 2021-2-9 汪军锋 wwx921845 从sml.h拆分出该头文件
 */
#ifndef SML_PD_H
#define SML_PD_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#pragma pack(1)

/***************************************************************/
/* 变量名称前缀 i - input,表示在调用对应的接口时此变量是输入参数 */
/* 变量名称前缀 o - output,表示在调用对应的接口时此变量是是输出参数 */
/**************************************************************/

// ------------PD的健康状态
typedef struct tag_sml_pd_health_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint16 o_health_status_code;
    /*
    bit0: 1- media error
    bit1: 1- predictive fail
    bit2: 1- other error
    bit3: 1- io deterioration
    bit4: 1- drive not ready
    bit5: 1- drive functional
    bit6: 1- reset not required
    bit7: 1- port0 pcie link active
    other: reserved
    */
} SML_PD_HEALTH_STATUS_S;

//  ------------PD是否是FDE(Full Disk Encryption)
typedef struct tag_sml_pd_fde_capable {
    guint8  i_controller_index;  // RAID控制器的索引号，从0开始
    guint16 i_device_id;  // PD的设备ID，唯一标识
    guint8  o_fde_capable;
    /*
    1=FDE drive, 0=non-FDE
    */
} SML_PD_FDE_CAPABLE_S;

// ------------PD的FW状态
typedef struct tag_sml_pd_fw_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint16 o_fw_state;
    /*
    0x00 :Unconfigured good drive
    0x01 :Unconfigured bad drive
    0x02 :Hot spare drive
    0x10 :Configured - good drive (data invalid)
    0x11 :Configured - bad drive (data invalid)
    0x14 :Configured - drive is rebuilding
    0x18 :Configured - drive is online
    0x20 :drive is getting copied
    0x40 :drive is exposed and controlled by host
    0x80 :UnConfigured - shielded
    0x82 :Hot Spare - shielded
    0x90 :Configured - shielded
    */
} SML_PD_FW_STATE_S;

// ------------PD的SN
typedef struct tag_sml_pd_sn {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    gchar  o_pd_sn[SML_PD_SN_LENGTH];
    gchar  previous_sn[SML_PD_SN_LENGTH];
} SML_PD_SN_S;

// ------------PD的型号
typedef struct tag_sml_pd_model {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    gchar   o_pd_model[SML_PD_MODEL_LENGTH];
} SML_PD_MODEL_S;

// ------------PD的FW版本
typedef struct tag_sml_fw_version {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    gchar   o_pd_fw_version[SML_PD_FW_VERSION_LENGTH];
} SML_PD_FW_VERSION_S;

// ------------PD的容量大小
typedef struct tag_sml_pd_capacity {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint32 o_capcity_size;
    guint16 o_block_size; // 硬盘块大小
} SML_PD_CAPACITY_S;

// ------------PD的媒介类型
typedef struct tag_sml_pd_media_type {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_media_type;
} SML_PD_MEDIA_TYPE_S;

// ------------PD的接口类型
typedef struct tag_sml_pd_interface_type {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_if_type;
    /*
    0:Unknown
    1:parallel SCSI,
    2:SAS
    3:SATA
    4:FC
    other: reserved
    */
} SML_PD_INTERFACE_TYPE_S;

// ------------PD的支持的最大速率
typedef struct tag_sml_pd_device_speed {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_device_speed;
    /*
    0: MR_PD_SPEED_UNKNOWN
    1: MR_PD_SPEED_1p5G
    2: MR_PD_SPEED_3G
    3: MR_PD_SPEED_6G
    4: MR_PD_SPEED_12G
    other: reserved
    */
} SML_PD_DEVICE_SPEED_S;

// ------------PD的Link速率
typedef struct tag_sml_pd_link_speed {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_link_speed;
} SML_PD_LINK_SPEED_S;

// ------------PD的厂商
typedef struct tag_sml_pd_manufacturer {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    gchar   o_manufacturer[SML_PD_MANUFACTURER_LENGTH];
} SML_PD_MANUFACTURER_S;
// ------------PD的厂商ID
typedef struct tag_sml_pd_verdorid {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint32  o_vendorid;
} SML_PD_VENDORID_S;

// ------------PD的SAS地址
typedef struct tag_sml_pd_sas_addr {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    gchar   o_sas_addr1[SML_SAS_ADDR_LENGTH];
    gchar   o_sas_addr2[SML_SAS_ADDR_LENGTH];
} SML_PD_SAS_ADDR_S;

// ------------PD的热备状态
typedef struct tag_sml_pd_hot_spare_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_hot_spare;
    /*
    0：no hot spare
    1：global hot spare
    2：dedicated spare
    other: reserved
    */
} SML_PD_HOT_SPARE_STATUS_S;

// ------------PD的重构状态
typedef struct tag_sml_pd_rebuild_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_is_rebuild;
    guint8  o_rebuid_progress;
} SML_PD_REBUILD_STATUS_S;

// ------------PD的巡检状态
typedef struct tag_sml_pd_patrol_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_is_patrol;
    guint8  o_patrol_progress;
} SML_PD_PATROL_STATUS_S;

// ------------PD的巡检状态
typedef struct tag_sml_pd_power_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_power_state;
    /*
    0: device is active (spun up)
    1: device is stopped (spun down)
    0xff: device is transitioning between power states
    */
} SML_PD_POWER_STATE_S;

// ------------PD的温度
typedef struct tag_sml_pd_temperature {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_temperature;
} SML_PD_TEMPERATURE_S;

// ------------PD的SMART预告警状态
typedef struct tag_sml_pd_smart_prefail {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_smart_prefail;
    /*
    0 - no prefail
    1 - has prefail
    */
} SML_PD_SMART_PREFAIL_S;

// ------------PD的SSD剩余使用寿命
typedef struct tag_sml_pd_ssd_lifeleft {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint8  o_ssd_remnant_wearout; // 剩余擦写次数的百分比
    /*
    0 - 100: percentage of life left
    255 : not support
    */
} SML_PD_SSD_REMNANT_MEDIA_WEAROUT_S;


/* BEGIN: Added by h00371221, 2017/12/25   AR.SR.SFEA02109385.002.001 */
// ------------PD的SSD累计通电时间
typedef struct tag_sml_pd_ssd_power_on_hours {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint32  o_ssd_power_on_hours; //
} SML_PD_SSD_POWER_ON_HOURS_S;
/* END:    Added by h00371221, 2017/12/25 */

// ------------PD的错误统计
typedef struct tag_sml_pd_error_count {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint32 o_media_err_count;
    guint32 o_other_err_count;
    guint32 o_predictive_fail_count;
} SML_PD_ERR_COUNT_S;

// PD的转速
typedef struct {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id;        // PD的设备ID，唯一标识
    guint16 o_rotation_speed;
} SML_PD_ROTATION_SPEED;

// PD的尺寸
typedef struct {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id;        // PD的设备ID，唯一标识
    guint8  o_form_factor;
} SML_PD_FORM_FACTOR;

// ------------Dump PD SMART info
typedef struct tag_sml_pd_smart_dump {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    gchar   i_file_path[PATH_MAX];
    gchar   i_file_short_name[NAME_MAX];
} SML_PD_SMART_DUMP_S;

typedef struct tag_sml_pd_log_data {
    SML_PD_LOG_TYPE_E log_type;   // 日志类型
    union {
        guint16 sectors; // SATA 盘日志数据长度按sector个数计算
        guint32 bytes;   // SAS 盘日志数据长度按byte计算
    }max_raw_data_size;
    gint32  result;
    guint32 scsi_status_code;   // 收集失败时的SCSI Status Code
    guint32 data_length; // 原始数据长度
    guint8* data;        // 动态分配的内存，存放日志的原始数据
}SML_PD_LOG_DATA_S;

typedef struct tag_sml_pd_log {
    guint8  i_controller_index;
    guint16 i_device_id;
    SML_PD_LOG_DATA_S log;
}SML_PD_LOG_S;

/* BEGIN: Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表 */
typedef struct tag_sml_pd_smart_collect_info {
    guint8  i_controller_index;
    guint16 i_device_id;
    /* 此处无法引用到SML_PD_SMART_INFO_S的定义，定义为通用指针 */
    SML_PD_SMART_INFO_S smart_info;
}SML_PD_SMART_COLLECT_INFO_S;
/* END:   Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表 */

// ----------------硬盘故障诊断
typedef struct tag_sml_pd_fault_analysis {
    guint8 i_controller_index;
    guint16 i_pd_device_id;
    guint16 i_encl_device_id;
    guint8 i_pd_error;  // 用于链路诊断
    union {
        struct {
#ifndef BD_BIG_ENDIAN
            guint32 sense_error : 1;
            guint32 encl_comm_error : 1;
            guint32 ctrl_encl_link_error : 1;
            guint32 encl_pd_link_error : 1;
            guint32 ctrl_pd_link_error : 1;
            // expander到expander的链路，共三个bit位，第n位bit表示离硬盘的第n级encl_ecnl链路有问题
            guint32 encl_encl_link_error : 3;
            guint32 pd_smart_log_error : 1;    // error标志,任何SMART属性(除了crc错误)或标准日志诊断出异常则标记
            guint32 pd_crc_error : 1;          // crc错误时需要结合链路诊断特殊处理
            guint32 pd_passthru_cmd_fail : 1;  // 硬盘直通命令发送失败，需要结合链路诊断特殊处理
            guint32 pd_short_dst_error : 1;    // 硬盘short dst检测失败
            guint32 reserved : 20;
#else
            guint32 reserved : 20;
            guint32 pd_short_dst_error : 1;
            guint32 pd_passthru_cmd_fail : 1;
            guint32 pd_crc_error : 1;
            guint32 pd_smart_log_error : 1;
            guint32 encl_encl_link_error : 3;
            guint32 ctrl_pd_link_error : 1;
            guint32 encl_pd_link_error : 1;
            guint32 ctrl_encl_link_error : 1;
            guint32 encl_comm_error : 1;
            guint32 sense_error : 1;
#endif
        }o_fault_bitmap;
        guint32 o_fault_dword;
    };
    // sas 接口的华为和记忆硬盘在匹配sense 故障的时候，通过此数组传递厂商信息，
    // 此数组同时用于记录维护日志的信息。
    gchar io_buf[SML_MAX_DETAIL_DESC_SIZE];
}SML_PD_FAULT_ANALYSIS;

// -----------启动盘优先级
typedef struct tag_sml_pd_bootable {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id;    // PD的设备ID，唯一标识
    guint8 o_bootable;
    guint8 o_boot_priority;  // 启动盘优先级
} SML_PD_BOOT_PRIORITY_S;

/*
 * Description: APP向SML获取指定RAID控制器的PD的健康状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_health_status(SML_PD_HEALTH_STATUS_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的FDE能力
 * History: 2019-03-03 zwx382755 新生成函数
*/
gint32 sml_get_pd_fde_capable(SML_PD_FDE_CAPABLE_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的firmware状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_fw_state(SML_PD_FW_STATE_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的SN
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_sn(SML_PD_SN_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的型号
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_model(SML_PD_MODEL_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的Firmware版本
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_fw_version(SML_PD_FW_VERSION_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的容量
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_capacity(SML_PD_CAPACITY_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的媒介类型
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_media_type(SML_PD_MEDIA_TYPE_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的接口类型，e.g. SATA,SAS,etc
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_interface_type(SML_PD_INTERFACE_TYPE_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的支持的最大速率
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_device_speed(SML_PD_DEVICE_SPEED_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的协商的速率
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_link_speed(SML_PD_LINK_SPEED_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的厂商名称
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_manufacturer(SML_PD_MANUFACTURER_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的厂商ID
 * History: 2019-02-26 chenshihao cwx398307 新生成函数
*/
gint32 sml_get_pd_vendorid(SML_PD_VENDORID_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的SAS地址
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_sas_addr(SML_PD_SAS_ADDR_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的热备状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_hot_spare_status(SML_PD_HOT_SPARE_STATUS_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的重建状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_rebuild_status(SML_PD_REBUILD_STATUS_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的巡检状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_patrol_status(SML_PD_PATROL_STATUS_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的电源状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_power_state(SML_PD_POWER_STATE_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的温度
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_temperature(SML_PD_TEMPERATURE_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的剩余使用寿命，仅对SSD有效
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_ssd_remnant_wearout(SML_PD_SSD_REMNANT_MEDIA_WEAROUT_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的累计通电时间，仅对SSD有效
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_ssd_power_on_hours(SML_PD_SSD_POWER_ON_HOURS_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的SMART预告警状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_smart_prefail(SML_PD_SMART_PREFAIL_S* pd);

/*
 * Description: APP向SML获取指定RAID控制器的PD的错误统计
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_pd_err_count(SML_PD_ERR_COUNT_S* pd);

/*
 * Description: APP向SML获取指定硬盘的转速
 * History：2020-10-19 xwx949252 UADP1159373 新增
 */
gint32 sml_get_pd_rotation_speed(SML_PD_ROTATION_SPEED* pd);

/*
 * Description : APP向SML获取指定硬盘的尺寸
 * History：2020-02-26 zhanglei (zwx382755) UADP602577 新增
 */
gint32 sml_get_pd_form_factor(SML_PD_FORM_FACTOR* pd);

/*
 * Description: APP通过SML获取PD的日志数据
 * History: 2018-01-16 huanghan 新生成函数
*/
gint32 sml_get_pd_log(SML_PD_LOG_S* pd);

/*
 * Description: APP向SML dump指定RAID控制器的PD SMART信息
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_dump_pd_smart_info(SML_PD_SMART_DUMP_S* pd);

/*
 * Description: APP通过SML对PD执行操作
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_pd_operation(SML_PD_OPERTATION_S* pd);

/*
 Description: APP通过SML设置硬盘固件状态
 */
gint32 sml_set_pd_state(guint8 controller_index, guint16 device_id, guint8 state);

/*
 Description: APP通过SML设置硬盘巡检状态
 */
gint32 sml_set_pd_patrol_state(guint8 ctrl_id, guint16 did, guint8 patrol_state);

/*
 * Description: APP通过SML检查pd的sense code，判断是否需要上报故障
 * History: 2019-02-15 huangleisheng 00475287 新生成函数
*/
gboolean sml_check_pd_sense_code(SCSI_SENSE_DISECT_S *sense_info);

/* BEGIN: Added by 00356691 zhongqiu, 2018/5/19   PN:AR.SR.SFEA02130925.003.001 支持采集的数据列表 */
gint32 sml_collect_hdd_smart_data(SML_PD_SMART_COLLECT_INFO_S* pd);

/*
 * Description: 打印ATA smart 信息到buf
 * History: 2017-05-9 gwx455466 DTS2017050906926
*/
void sml_parse_ata_smart_attr_raw_value(gchar* buf, guint32 buf_size, ATA_SMART_ATTRIBUTE_S* smart_attr,
    guint8* format);

/*
 * Description: 获取SMART Attribute的名称
 * History: 2018-01-10 huanghan h00282621 新生成函数
*/
const gchar* sml_get_ata_smart_attr_name(guint8 attr_id);

/*
 * Description: APP调用该接口诊断链路PHY误码故障
 * History: 2019-01-24 x00453037 新生成函数
*/
gint32 sml_diagnose_link_phy_error(SML_PD_FAULT_ANALYSIS* diag_info, guint32 serial_threshold,
    guint32 recent_threshold);

/*
 * Description: 诊断Expander与RAID卡之间的通信故障
 * History: 2019-01-7 x00453037 新生成函数
*/
gint32 sml_diagnose_encl_comm_error(SML_PD_FAULT_ANALYSIS* diag_info);

/*
 * Description: APP调用该接口诊断硬盘的sense error
 * History: 2019-01-10 x00453037 新生成函数
*/
gint32 sml_diagnose_pd_sense_error(SML_PD_FAULT_ANALYSIS* diag_info);

/*
 * Description: APP向SML获取指定RAID控制器PD的启动盘优先级
 */
gint32 sml_get_pd_boot_priority(SML_PD_BOOT_PRIORITY_S *pd);

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
