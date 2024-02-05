/******************************************************************************

                  版权所有 (C), 2016-2021, 华为技术有限公司

 ******************************************************************************
  文 件 名   : sml_public.h
  版 本 号   : 初稿
  作    者   : huanghan (h00282621)
  生成日期   : 2016年2月27日
  最近修改   :
  功能描述   : SML内部用作缓存的数据类型，内部接口, 主要是华为自定义的数据结构，
               且与厂商提供的接口无关的定义
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月27日
    作    者   : huanghan (h00282621)
    修改内容   : 创建文件

******************************************************************************/
#ifndef __SML_PUBLIC_H__
#define __SML_PUBLIC_H__

#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml_common.h"
#include "pme_app/smlib/sml_public_ctrl.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#pragma pack(1)

#define ATA_SMART_UPDATE_CYCLE 3600  // 1小时

#define PD_HEALTH_CODE_SLOW_DISK       BIT(3)

/* BEGIN: Added by Yangshigui YWX386910, 2017/4/19   问题单号:DTS2017041911858 */
enum {
    PD_WARNNING_INFO_FW_STATE,         // fw_state
    PD_WARNNING_INFO_CAPACITY,         // coerced_size
    PD_WARNNING_INFO_MEDIA_ERR_COUNT,  // media_err_count
    PD_WARNNING_INFO_PREFAIL_COUNT,    // prefail_count
    MAX_PD_WARNNING_INFO
};

#define MAX_WARNNING_INFO_DEBOUNCE_COUNT 3    // 告警信息防抖次数
#define RAID_INFO_DEBOUNCE_DELAY         2000 // 防抖时再次从RAID卡读取的延时
/* END:   Added by Yangshigui YWX386910, 2017/4/19 */

typedef struct _tag_sml_pd_basic_info {
    gint32 identify_status;  // sata盘厂商和型号版本获取成功标志
    guint16 device_id;
    guint16 sequence_num;
    guint16 health;
    guint8 fde_capable;
    guint8 fw_state;
    guint8 power_state;
    guint8 scsi_dev_type;
    guint8 device_speed;
    guint8 link_speed;
    guint16 encl_device_id;
    guint8 slot_num;
    guint8 media_type;
    guint8 interface_type;
    guint8 temperature;
    guint8 boot_priority;
    gchar sas_addr1[SML_SAS_ADDR_LENGTH];
    gchar sas_addr2[SML_SAS_ADDR_LENGTH];
    gchar serial_num[SML_PD_SN_LENGTH];
    gchar model[SML_PD_MODEL_LENGTH];
    gchar firmware_version[SML_PD_FW_VERSION_LENGTH];
    gchar manufacturer[SML_PD_MANUFACTURER_LENGTH];
    guint32 vendor_id;
    guint32 coerced_size;  // 单位MB
    guint32 media_err_count;
    guint32 other_err_count;
    guint32 prefail_count;
    guint8 hot_spare;
    guint8 remnant_media_wearout;
    guint16 rotation_speed;
    SML_PD_PROGRESS_INFO_S proginfo;
    guint8  form_factor;

    // 以下为创建LD时需要的物理盘信息
    guint8 dev_not_supported;  // 设置是否支持
    guint8 fw_state_raw;       // 未经转换的直接从RAID卡读出来的物理盘固件状态
    guint8 is_foreign;         // 物理盘上是否包含外部配置信息
    guint64 coerced_size_blk;  // 以block为单位的物理盘容量，直接从RAID卡读出，未做任何转换
    /* BEGIN: Added by Yangshigui YWX386910, 2017/1/13   问题单号:DTS2017010501008 */
    guint16 block_size;  // block size，典型的有512 bytes/4K bytes
    /* END:   Added by Yangshigui YWX386910, 2017/1/13 */
    /* BEGIN: Added by h00371221, 2017/12/25   AR.SR.SFEA02109385.002.001 */
    guint32 power_on_hours;
    guint16 halflife;
    guint32 last_update_timestamp;
    /* END:    Added by h00371221, 2017/12/25 */
    guint8 bootable;
} SML_PD_BASIC_INFO_S;

typedef struct _tag_sml_pd_info {
    guint32 last_update_timestamp;
    SML_PD_BASIC_INFO_S pdinfo;
    SML_PD_SMART_INFO_S smartinfo;
    /* BEGIN: Added by Yangshigui YWX386910, 2017/4/19   问题单号:DTS2017041911858 */
    // 标记物理盘相关的告警数据是否已经确定并上报给上层APP,避免确定了的告警信息还进行不必要的防抖处理
    guint8 pd_warnig_info_reported[MAX_PD_WARNNING_INFO];
    /* END:   Added by Yangshigui YWX386910, 2017/4/19 */
} SML_PD_INFO_S;

typedef struct _tag_sml_pd_list {
    guint32 last_update_timestamp;
    guint16 pd_count;
    guint8 reserved[2];
    guint16 device_ids[SML_MAX_PHYSICAL_DRIVES];
    guint8 slot_num[SML_MAX_PHYSICAL_DRIVES];
    /* BEGIN: Modified by gwx455466, 2017/9/4 14:35:35   问题单号:DTS2017083101891 */
    guint16 enclosure_ids[SML_MAX_PHYSICAL_DRIVES];
    /* END:   Modified by gwx455466, 2017/9/4 14:35:39 */
} SML_PD_LIST_S;

typedef struct _tag_sml_ld_list {
    guint32 last_update_timestamp;
    guint16 ld_count;
    guint8 reserved[2];
    guint16 target_ids[SML_MAX_LOGIC_DRIVES];
} SML_LD_LIST_S;

/* BEGIN: Modified by zhanglei wx382755, 2017/12/2   PN:AR.SR.SFEA02109379.004.006 */
typedef struct _tag_sml_ld_progress_info {
    // 当前是否在fgi过程中
    guint8 current_fgi_state;
    // fgi进度，百分比
    guint8 fgi;
    // 当前是否在重构过程中
    guint8 rebuild_state;
    // 重构进度，百分比
    guint8 rebuild_progress;
} SML_LD_PROGRESS_INFO_S;
/* END:   Modified by zhanglei wx382755, 2017/12/2 */

typedef struct _tag_sml_ld_basic_info {
    guint32 last_update_timestamp;
    guint8 ld_warnig_info_reported;  // 标记逻辑盘告警信息是否已经上报
    guint8 reserved0;  // fill pad to align this structrue with 4bytes
    guint16 target_id;
    gchar name[SML_LD_NAME_LENGTH];
    guint8 drive_state;
    guint8 raid_level;
    guint8 default_read_policy;
    guint8 default_write_policy;
    guint8 default_cache_policy;
    guint8 current_read_policy;
    guint8 current_write_policy;
    guint8 current_cache_policy;
    guint8 access_policy;
    guint8 disk_cache_policy;
    guint8 init_state;  // 初始化操作方式，创建逻辑盘时指定的参数
    guint8 consistent_check;
    guint8 span_depth;
    guint8 num_drive_per_span;
    guint8 bgi_enabled;
    guint8 bootable;
    guint8 is_sscd;
    guint8 is_epd;
    guint8 strip_size;
    guint8 accelerator;
    guint32 size;
    guint32 max_resizeable_size;  // 以MB为单位
    guint8 boot_priority;
    guint8 reserved;  // fill pad to align this structrue with 4bytes
    guint8 cache_line_size;
    guint8 array_count;
    guint16 ref_array[SML_MAX_SPAN_DEPTH];
    SML_LD_PROGRESS_INFO_S progress_info;

    // sscd关联的普通ld列表，只有sscd才有这个信息
    guint16 sscd_ld_count;
    guint16 sscd_ld_list[SML_MAX_LOGIC_DRIVES];
    guint8 reserved3[3];   // fill pad to align this structrue with 4bytes
    // 逻辑盘专用热备盘列表
    guint8 spare_count;
    guint16 spare_pd_ids[SML_MAX_PHYSICAL_DRIVES];
    guint8 spare_pd_slots[SML_MAX_PHYSICAL_DRIVES];
    /* BEGIN: Modified by gwx455466, 2017/9/4 14:35:35   问题单号:DTS2017083101891 */
    guint16 spare_pd_enclosure_ids[SML_MAX_PHYSICAL_DRIVES];
    /* END:   Modified by gwx455466, 2017/9/4 14:35:39 */
} SML_LD_BASIC_INFO_S;

typedef struct _tag_sml_ld_info {
    SML_LD_BASIC_INFO_S ldinfo;
    SML_PD_LIST_S pd_in_ld;
} SML_LD_INFO_S;

typedef struct _tag_sml_bbu_status {
    guint32 last_update_timestamp;
    /* BEGIN: Added by Yangshigui YWX386910, 2017/4/19   问题单号:DTS2017041911858 */
    guint8 bbu_warnig_info_reported;  // 标记BBU告警是否已经上报过
    /* END:   Added by Yangshigui YWX386910, 2017/4/19 */
    guint8 present;
    /** BEGIN: Added by cwx290152, 2017/11/21   PN:DTS2017101812710 */
    guint16 temperature;
    /** END:   Added by cwx290152, 2017/11/21 */
    gchar type[SML_BBU_TYPE_LENGTH];
#ifdef BD_BIG_ENDIAN
    guint32 reserved : 22;
    guint32 failed : 1;
    guint32 no_space : 1;
    guint32 remaining_capacity_low : 1;
    guint32 predictive_failure : 1;
    guint32 learn_cycle_timeout : 1;
    guint32 learn_cycle_failed : 1;
    guint32 replacepack : 1;
    guint32 temperature_high : 1;
    guint32 voltage_low : 1;
    guint32 pack_missing : 1;
#else
    guint32 pack_missing : 1;
    guint32 voltage_low : 1;
    guint32 temperature_high : 1;
    guint32 replacepack : 1;
    guint32 learn_cycle_failed : 1;
    guint32 learn_cycle_timeout : 1;
    guint32 predictive_failure : 1;
    guint32 remaining_capacity_low : 1;
    guint32 no_space : 1;
    guint32 failed : 1;
    guint32 reserved : 22;
#endif
    guint8 present_updated;  // 标记上电后，BBU在位是否更新过，下电会清0
    guint8 reserved1[3];
} SML_BBU_STATUS_S;

// Refer to LSI storelib definition
typedef struct _tag_sml_sasphy_error_count {
    guint32 invalid_dword_count;
    guint32 loss_dword_sync_count;
    guint32 phy_reset_problem_count;
    guint32 running_disparity_error_count;
} SML_SASPHY_ERR_COUNT_S;

typedef struct _tag_sml_sasphy_error_info {
    guint32 phy_id;
    guint64 exp_sas_addr;
    SML_SASPHY_ERR_COUNT_S phy_err_count;
} SML_SASPHY_ERR_INFO_S;

typedef struct _tag_sml_ctrl_sasphy_info {
    guint32 last_update_timestamp;
    guint32 collect_timestamp;
    guint8 phy_count;
    guint8 reserved[3];
    SML_SASPHY_ERR_COUNT_S phy_err[SML_MAX_SAS_PHY_PER_CTRL];
} SML_SASPHY_INFO_S;

typedef struct _tag_sml_ctrl_expander_info {
    guint8 expander_count;
    guint8 reserved[3];
} SML_EXPANDER_INFO_S;

// Expander中的PHY误码信息
typedef struct _tag_sml_expander_sasphy_info {
    guint8 phy_count;
    SML_SASPHY_ERR_COUNT_S phy_err[SML_MAX_SAS_PHY_PER_EXPANDER];
} SML_EXP_SASPHY_INFO_S;

// RAID卡下的Expander PHY误码信息
typedef struct _tag_sml_ctrl_expander_sasphy_info {
    guint32 collect_timestamp;
    guint8 expander_count;
    SML_EXP_SASPHY_INFO_S expander_phy[SML_MAX_EXPANDER_PER_CONTROLLER];
} SML_CTRL_EXP_SASPHY_INFO_S;

// RAID卡下的所有expander信息
typedef struct _tag_sml_ctrl_expander_list_info {
    gint32 count;                                        // raid卡下exp数量
    guint64 sas_addr_list[SML_MAX_EXPANDER_PER_CONTROLLER]; // exp的sas地址
    guint8 phy_count_list[SML_MAX_EXPANDER_PER_CONTROLLER]; // exp的phy数量
} SML_CTRL_EXP_LIST_INFO_S;

// PHY误码的历史信息，用于PHY误码诊断
typedef struct _tag_sml_phy_diag_history_info {
    GList *ctrl_phy_list;  // 链表元素为 SML_SASPHY_INFO_S
    GList *exp_phy_list;   // 链表元素为 SML_CTRL_EXP_SASPHY_INFO_S
} SML_PHY_DIAG_HISTORY_INFO_S;

// PHY误码诊断的拓扑信息
typedef struct _tag_sml_phy_diag_info {
    // 诊断标志，0为不诊断，1为诊断
    guint8 diag_mark;
    // phy类型标识，0为下行phy，1为上行phy(主要用于区分Expander的PHY)
    guint8 phy_type;
    // 连接设备类型，0为No device，1为硬盘，2为Encl，3为RAID卡，4为其他设备
    guint8 connect_type;
} SML_PHY_DIAG_INFO_S;

typedef enum _tag_sml_phy_connect_type {
    SML_PHY_CONNECT_NO_DEVICE = 0,
    SML_PHY_CONNECT_END_DEVICE,
    SML_PHY_CONNECT_ENCL_DEVICE,
    SML_PHY_CONNECT_RAID_DEVICE,
    SML_PHY_CONNECT_OTHER_DEVICE
} SML_PHY_CONNECT_TYPE_E;

typedef enum _tag_sml_phy_type {
    SML_PHY_TYPE_DOWN = 0,
    SML_PHY_TYPE_UP
} SML_PHY_TYPE_E;

typedef struct _tag_sml_exp_phy_diag_info {
    guint64 sas_addr;          // SAS地址，用于记录该Expander是否在拓扑中已经遍历过
    guint8 diag_mark;          // 用于标记该Expander是否在诊断链路上
    guint8 virtual_phy_count;  // 虚拟phy数目
    guint8 exp_level;          // exp的层级，用于记录exp级联的层级
    guint8 phy_count;
    SML_PHY_DIAG_INFO_S phy_info[SML_MAX_SAS_PHY_PER_EXPANDER];
} SML_EXP_PHY_DIAG_INFO_S;

typedef struct _tag_sml_ctrl_phy_diag_topo_info {
    guint8 ctrl_phy_count;
    SML_PHY_DIAG_INFO_S ctrl_phy_info[SML_MAX_SAS_PHY_PER_CTRL];

    guint8 exp_count;
    SML_EXP_PHY_DIAG_INFO_S exp_info[SML_MAX_EXPANDER_PER_CONTROLLER];
} SML_CTRL_PHY_DIAG_TOPO_INFO_S;


typedef struct _tag_sml_ctrl_sas_addr_info {
    guint32 last_update_timestamp;
    gchar addr[SML_SAS_ADDR_LENGTH];
} SML_SAS_ADDR_S;

/* BEGIN: Added by Yangshigui YWX386910, 2016/11/8   问题单号:AR-0000264511-001-003 */
typedef struct _tag_sml_array_list {
    guint32 last_update_timestamp;
    guint16 array_count;
    guint8 reserved[2];
    guint16 array_refs[SML_MAX_ARRAY];
} SML_ARRAY_LIST_S;

typedef struct _tag_smlib_array_info {
    guint32 last_update_timestamp;
    guint32 used_space;                                        // Array上已用空间，单位MB
    guint32 total_free_space;                                  // Array上总的空闲空间，单位MB
    guint32 free_blocks_count;                                 // Array上空闲块的数量
    guint32 free_blocks_space[SML_MAX_HOLES_IN_ARRAY];         // Array上可用于创建逻辑盘的空闲容量，单位MB
    guint8 reserved1[3];                                       // fill pad to align this structrue with 4bytes
    guint8 ld_count;                                           // Array上已经建立的逻辑盘个数
    guint16 ld_ids[SML_MAX_LOGICAL_DRIVES_PER_ARRAY];           // Array上逻辑盘ID列表
    guint8 reserved2[3];                                       // fill pad to align this structrue with 4bytes
    guint8 pd_count;                                           // Array包含的物理硬盘个数
    guint16 pd_ids[SML_MAX_PHYSICAL_DRIVES_PER_ARRAY];         // 包含的物理硬盘的device id列表
    guint8 pd_slots[SML_MAX_PHYSICAL_DRIVES_PER_ARRAY];        // 包含的物理硬盘的槽位号(slot)列表
    /* BEGIN: Modified by gwx455466, 2017/9/5 17:23:20   问题单号:DTS2017083101891 */
    guint16 pd_enclosures[SML_MAX_PHYSICAL_DRIVES_PER_ARRAY];  // 包含的物理硬盘的EID列表
    /* END:   Modified by gwx455466, 2017/9/5 17:23:23 */
} SML_ARRAY_INFO_S;
/* END:   Added by Yangshigui YWX386910, 2016/11/8 */

typedef struct _tag_sml_ctrl_info {
    SML_CTRL_BASIC_INFO_S ctrl;
    SML_CTRL_HEALTH_S health;
    SML_SAS_ADDR_S sas_addr;
    SML_BBU_STATUS_S bbu;
    SML_SASPHY_INFO_S phy;
    SML_EXPANDER_INFO_S expander;
    SML_LD_LIST_S ldlist;
    SML_PD_LIST_S pdlist;
    SML_LD_INFO_S ld[SML_MAX_LOGIC_DRIVES];
    SML_PD_INFO_S pd[SML_MAX_PHYSICAL_DRIVES];
    /* BEGIN: Added by Yangshigui YWX386910, 2016/11/8   问题单号:AR-0000264511-001-003 */
    SML_ARRAY_LIST_S arraylist;
    SML_ARRAY_INFO_S array[SML_MAX_ARRAY];
    /* END:   Added by Yangshigui YWX386910, 2016/11/8 */

    SML_PHY_DIAG_HISTORY_INFO_S phy_history;  // PHY误码诊断历史数据

    /* BEGIN: Added by Yangshigui YWX386910, 2017/1/11   问题单号:DTS2017010501008 */
    // 刷新数据优化
    /*
    xxx_refresh_ctrl数组说明
    数组下标为逻辑盘/Array ID (0 ~ 255)，物理盘刷新控制的下标为物理盘在缓存中的索引值
    bit3 ~ bit0: 刷新方式(策略)
                 0000 - 按默认方式刷新(按超时的方式刷新)
                 0001 - 强制从RAID控制器刷新
                 0010 - 强制直接从全局缓存读取数据
    bit7 ~ bit4: 当前策略持续次数，当为0时自动转换为按默认方式刷新
 */
    guint8 ld_refresh_ctrl[SML_MAX_NUM_OF_IDS];
    guint8 pd_in_ld_refresh_ctrl[SML_MAX_NUM_OF_IDS];
    guint8 pd_refresh_ctrl[SML_MAX_NUM_OF_IDS];
    guint8 array_refresh_ctrl[SML_MAX_NUM_OF_IDS];

    // 记录上次写物理盘SMART信息的时间戳
    guint32 last_smart_dump_timestamp;
    /* END:   Added by Yangshigui YWX386910, 2017/1/11 */
} SML_CTRL_INFO_S;

/* PD状态设置参数 */
typedef struct tag_sml_pd_state_param {
    guint8 state;
    guint8 support_epd;
}SML_PD_STATE_PARAM_S;

/* BEGIN: Added by Yangshigui YWX386910, 2016/11/9   问题单号:AR-0000264511-003-001 ~  AR-0000264511-003-008 */
// ------------在新的Array上创建逻辑盘
typedef struct tag_sml_create_ld_on_new_array {
    guint8 span_depth;                        // span的个数
    guint8 num_drive_per_span;                // 每个span的硬盘数
    guint16 pd_sel[SML_MAX_PHYSICAL_DRIVES];  // 选择的硬盘列表，总的硬盘数=span的个数x每个span的硬盘数
    guint8 raid_level;                        // RAID级别:0/1/5/6/10/50/60/1ADM/10ADM

    SML_CREATE_LD_COMMON_PROPERTIES_S props;  // 公共属性参数

    SML_CREATE_LD_RETURN_PARAM_S ret_param;  // 返回参数

    // 创建过程中需要的额外信息,由sml内部获取填充，非用户输入信息
    SML_CREATE_LD_PD_INFO_S pd_infos[SML_MAX_PHYSICAL_DRIVES];
    SML_CREATE_LD_CTRL_INFO_S ctrl_info;
} SML_CREATE_LD_ON_NEW_ARRAY_S;

// ------------在现有Array上创建逻辑盘
typedef struct tag_sml_create_ld_on_existed_array {
    guint16 array_ref;                        // 存在空闲空间的Array ID
    guint8 block_index;                       // 用户选择的空闲块，未选择时为0xFF
    guint8 raid_level;                        // RAID级别:0/1/5/6/10/50/60/1ADM/10ADM
    guint8 span_depth;                        // span的个数
    SML_CREATE_LD_COMMON_PROPERTIES_S props;  // 公共属性参数

    SML_CREATE_LD_RETURN_PARAM_S ret_param;  // 返回参数

    // 创建过程中需要的额外信息,由sml内部获取填充，非用户输入信息
    SML_CREATE_LD_CTRL_INFO_S ctrl_info;
    guint16 pd_block_size;  // array上物理盘的block size
} SML_CREATE_LD_ON_EXISTED_ARRAY_S;

// ------------使用SSD硬盘创建CacheCade逻辑盘
typedef struct tag_sml_create_cachecade_ld {
    guint8 pd_count;                          // 选择的SSD硬盘数
    guint16 pd_sel[SML_MAX_PHYSICAL_DRIVES];  // 选择的SSD硬盘列表
    guint8 raid_level;                        // RAID级别:0/1/5
    gchar ld_name[SML_LD_NAME_LENGTH];        // 逻辑盘的名称，以'\0'结束的ASCII字符串
    guint8 write_policy;                      // 写策略 Write Through/Write Back/Write caching ok if bad BBU
    guint16 array_ref;                        // 存在空闲空间的Array ID
    guint32 capacity;                         // 容量大小, MB为单位
    guint16 target_ld_id;                     // 待关联的普通逻辑盘
    guint8 cache_line_size;                   // 缓存行大小
    SML_CREATE_LD_RETURN_PARAM_S ret_param;  // 返回参数

    // 创建过程中需要的额外信息,由sml内部获取填充，非用户输入信息
    SML_CREATE_LD_PD_INFO_S pd_infos[SML_MAX_PHYSICAL_DRIVES];
    SML_CREATE_LD_CTRL_INFO_S ctrl_info;
} SML_CREATE_CACHECADE_LD_S;

// ------------LD属性设置项
typedef struct tag_sml_ld_set_properties {
    guint32 setting_options;            // 指示哪些设置项为有效值, bit0:ld_name/bit1:read_policy ...
    gchar ld_name[SML_LD_NAME_LENGTH];  // 逻辑盘名称
    guint8 read_policy;                 // 读策略 Read Ahead/No Read Ahead/ Adaptive Read Ahead
    guint8 write_policy;                // 写策略 Write Through/Write Back/Write caching ok if bad BBU
    guint8 io_policy;                   // IO策略 Direct IO/Cached IO
    guint8 access_policy;               // 访问策略 Read Write/Read Only/Blocked
    guint8 disk_cache_policy;           // 硬盘缓存策略 Unchanged/Enable/Disable
    guint8 bgi_enable;                  // 后台初始化使能
    guint8 accelerator;                 // 加速方法
    guint32 capacity;                   // 容量
    guint8 strip_size;                  // 条带大小

    // 以下参数为根据实际逻辑盘的实际状况确定，并非用户输入参数
    gint32 ssd_in_ld;  // 指示逻辑盘中是否包含SSD硬盘
} SML_LD_SET_PROPERTIES_S;
/* END:   Added by Yangshigui YWX386910, 2016/11/9 */

// ------------逻辑盘SSD Caching Enable设置
typedef struct tag_sml_ld_set_ssd_caching_enable {
    guint8 enable;  // SSD Caching使能

    // 以下参数为根据实际逻辑盘的实际状况确定，并非用户输入参数
    guint8 sscd_count;                             // 当前控制器下所有CacheCade LD的个数
    guint16 sscd_target_ids[SML_MAX_LOGIC_DRIVES];  // CacheCade LD的ID列表
} SML_LD_SET_SSCD_CACHING_ENABLE_S;

#define ASSERT_BIT_IF_TRUE(x, y, z) \
    do {                            \
        if (x) {                    \
            (y) |= BIT(z);          \
        } else {                    \
            (y) &= ~BIT(z);         \
        }                           \
    } while (0)

/* BEGIN: Modified by Yangshigui YWX386910, 2017/6/30   问题单号:SF-0000276589 */
#define SML_CTRL_ID_VALID_BIT(x) ((x)&0x000000FF)
/* END:   Modified by Yangshigui YWX386910, 2017/6/30 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

