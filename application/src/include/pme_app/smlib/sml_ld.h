/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2021. All rights reserved.
 * Description: 逻辑盘相关对外提供的数据和接口
 * Author: 汪军锋
 * Create: 2021-2-9
 * Notes: 无
 * History: 2021-2-9 汪军锋 wwx921845 从sml.h拆分出该头文件
 */
#ifndef SML_LD_H
#define SML_LD_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define LD_PROP_SETTING_NAME              BIT(0)
#define LD_PROP_SETTING_READ_POLICY       BIT(1)
#define LD_PROP_SETTING_WRITE_POLICY      BIT(2)
#define LD_PROP_SETTING_IO_POLICY         BIT(3)
#define LD_PROP_SETTING_ACCESS_POLICY     BIT(4)
#define LD_PROP_SETTING_DISK_CACHE_POLICY BIT(5)
#define LD_PROP_SETTING_BGI_ENABLE        BIT(6)
#define LD_PROP_SETTING_ACCELERATOR       BIT(7)
#define LD_PROP_SETTING_CAPACITY_SIZE     BIT(8)
#define LD_PROP_SETTING_STRIP_SIZE        BIT(9)

#pragma pack(1)

/***************************************************************/
/* 变量名称前缀 i - input,表示在调用对应的接口时此变量是输入参数 */
/* 变量名称前缀 o - output,表示在调用对应的接口时此变量是是输出参数 */
/**************************************************************/

// ------------------名称
typedef struct tag_sml_ld_name {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_ld_target_id;    // LD的目标ID，唯一标识
    gchar  o_name[SML_LD_NAME_LENGTH];
} SML_LD_NAME_S;

// -----------------健康状态
typedef struct tag_sml_ld_health_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_health_state;
    /*
    0: MR_LD_STATE_OFFLINE
    1: MR_LD_STATE_PARTIALLY_DEGRADED
    2: MR_LD_STATE_DEGRADED
    3: MR_LD_STATE_OPTIMAL
    */
} SML_LD_HEALTH_STATUS_S;

// ------------------运行状态
typedef struct tag_sml_ld_state {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_state;
    /*
    0: MR_LD_STATE_OFFLINE
    1: MR_LD_STATE_PARTIALLY_DEGRADED
    2: MR_LD_STATE_DEGRADED
    3: MR_LD_STATE_OPTIMAL
    */
} SML_LD_STATE_S;

// ----------------RAID级别
typedef struct tag_sml_ld_raid_level {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_raid_level;
    /*
    0: RAID0
    1: RAID1
    2: RAID2
    3: RAID3
    4: RAID4
    5: RAID5
    6: RAID6
    10: RAID10
    17: RAID1E
    20: RAID20
    30: RAID30
    40: RAID40
    50: RAID50
    60: RAID60
    255: Unknown
    other: reserved
    */
} SML_LD_RAID_LEVEL_S;

// ----------------读策略
typedef struct tag_sml_ld_read_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_default_read_policy;
    /*
    0 : LD_CACHE_NO_READ_AHEAD,
    1 : LD_CACHE_READ_AHEAD,
    2 : LD_CACHE_ADAPTIVE_READ_ACHEAD
    */
    guint8  o_current_read_policy;
} SML_LD_READ_POLICY_S;

// ---------------写策略
typedef struct tag_sml_ld_write_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_default_write_policy;
    /*
    0 : LD_CACHE_WRITE_THROUGH,
    1 : LD_CACHE_WRITE_BACK,
    2 : LD_CACHE_WRITE_CACHE_IF_BAD_BBU
    3 : LD_CACHE_READ_ONLY
    */
    guint8  o_current_write_policy;
} SML_LD_WRITE_POLICY_S;

// ----------访问策略
typedef struct tag_sml_ld_access_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_access_policy;
} SML_LD_ACCESS_POLICY_S;

// -------------磁条大小
typedef struct tag_sml_ld_stripe_size {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_strip_size;     // 0=512, 1=1K, 2=2K, 3=4K, 7=64K, 11=1MB
} SML_LD_STRIP_SIZE_S;

// -------------容量
typedef struct tag_sml_ld_capacity_size {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint32 o_capacity_size;
    guint32 o_max_resizeable_size; // LD最大可设置的容量
} SML_LD_CAPACITY_SIZE_S;

// ----------物理盘cache策略
typedef struct tag_sml_ld_cache_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_default_cache_policy;
    /*
     0 : LD_CACHE_CACHED_IO,
     1 : LD_CACHE_DIRECT_IO
    */
    guint8  o_current_cache_policy;
    guint8  o_disk_cache_policy;
    /*
     0 : MR_PD_CACHE_UNCHANGED      // don't change disk cache policy - use disk's default
     1 : MR_PD_CACHE_ENABLE         // enable disk write cache
     2 : MR_PD_CACHE_DISABLE        // disable disk write cache
    */
} SML_LD_CACHE_POLICY_S;

/* BEGIN: Added by Yangshigui YWX386910, 2016/11/25   问题单号:DTS2016112111294 */
// -----------逻辑盘初始化操作方式
typedef struct tag_sml_ld_init_state {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_init_state;
    /*
    INIT_STATE_NO_INIT = 0,
    INIT_STATE_QUICK_INIT = 1,
    INIT_STATE_FULL_INIT = 2,
    */
} SML_LD_INIT_STATE_S;
/* END:   Added by Yangshigui YWX386910, 2016/11/25 */

// ---------是否在检查一致性
typedef struct tag_sml_ld_consistency_state {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_cc; // check consistency is in progress
} SML_LD_CC_STATE_S;

// -----------成员盘列表
typedef struct tag_sml_ld_pd_list {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint16 o_pd_count;
    guint16 o_pd_device_id[SML_MAX_PHYSICAL_DRIVES];
    guint8  o_pd_slot_num[SML_MAX_PHYSICAL_DRIVES];
    /* BEGIN: Modified by gwx455466, 2017/9/4 14:35:35   问题单号:DTS2017083101891  */
    guint16 o_pd_enclosure_id[SML_MAX_PHYSICAL_DRIVES];
    /* END:   Modified by gwx455466, 2017/9/4 14:35:39 */
} SML_LD_PD_LIST_S;

// -----------关联的array列表
typedef struct tag_sml_ld_ref_array {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint16 o_ref_array[SML_MAX_SPAN_DEPTH];
} SML_LD_REF_ARRAY_S;

// -----------Spandepth
typedef struct tag_sml_ld_span_depth {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_span_depth;
} SML_LD_SPAN_DEPTH_S;

// ------------Number drivers of per span
typedef struct tag_sml_ld_span_num_drivers {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_span_num_drivers;
} SML_LD_SPAN_NUM_DRIVERS_S;

// ----------- BGI使能状态
typedef struct tag_sml_ld_bgi_state {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_bgi_state;
} SML_LD_BGI_STATE_S;

// -----------启动盘优先级
typedef struct tag_sml_ld_bootable {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_bootable;
    guint8 o_boot_priority;    // LD的启动优先级
} SML_LD_BOOT_PRIORITY_S;

// -----------是否是CacheCade逻辑盘
typedef struct tag_sml_ld_is_sscd {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_target_id;    // LD的目标ID，唯一标识
    guint8  o_is_sscd;
} SML_LD_IS_SSCD_S;

typedef struct tag_sml_ld_accelerator {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_ld_target_id;     // LD的目标ID，唯一标识
    guint8  o_accelerator;
} SML_LD_ACCELERATOR_S;

typedef struct tag_sml_ld_cache_line_size {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_ld_target_id;     // LD的目标ID，唯一标识
    guint8  o_cache_line_size;
} SML_LD_CACHE_LINE_SIZE_S;

typedef struct tag_sml_ld_rebuild_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_ld_target_id;     // LD的目标ID，唯一标识
    guint8  o_is_rebuild;
    guint8  o_rebuid_progress;
} SML_LD_REBUILD_STATUS_S;

// ------------获取指定CacheCade LD关联的逻辑盘ID列表
typedef struct tag_sml_sscd_associated_ld_list {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_sscd_id; // CacheCade逻辑盘ID
    guint8  o_ld_count; // 关联的逻辑盘个数
    guint16  o_ld_ids[SML_MAX_LOGIC_DRIVES]; // 关联的逻辑盘ID列表
} SML_SSCD_ASSOCIATED_LD_LIST_S;

typedef struct tag_sml_ld_associated_sscd_list {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_ld_id; // 逻辑盘ID
    guint8  o_sscd_count; // 关联的CacheCade盘个数
    guint16  o_sscd_ids[SML_MAX_LOGIC_DRIVES]; // 关联的CacheCade盘ID列表
} SML_LD_ASSOCIATED_SSCD_LIST_S;

// ------------设置普通逻辑盘是否关联到CacheCade逻辑盘
typedef struct tag_sml_ld_sscd_caching_enable {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  o_sscd_caching_enable; // 是否有关联到cachecade LD
} SML_LD_SSCD_CACHING_ENABLE_S;

// ------------在新的Array上创建逻辑盘
typedef struct tag_sml_raid_on_new_array_param {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  i_span_depth; // span的个数
    guint8  i_num_drive_per_span; // 每个span的硬盘数
    guint16 i_pd_sel[SML_MAX_PHYSICAL_DRIVES]; // 选择的硬盘列表，总的硬盘数=span的个数x每个span的硬盘数
    guint8  i_raid_level; // RAID级别:0/1/5/6/10/50/60

    SML_CREATE_LD_COMMON_PROPERTIES_S i_props; // 创建时用到的逻辑盘属性

    SML_CREATE_LD_RETURN_PARAM_S o_ret_param; // 返回参数
} SML_RAID_ON_NEW_ARRAY_PARAM_S;

// ------------在现有Array上创建逻辑盘
typedef struct tag_sml_raid_on_existed_array_param {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_array_ref; // 存在空闲空间的Array ID
    guint8  i_block_index;
    guint8  i_raid_level;
    guint8  i_span_depth;
    SML_CREATE_LD_COMMON_PROPERTIES_S i_props; // 创建时用到的逻辑盘属性

    SML_CREATE_LD_RETURN_PARAM_S o_ret_param; // 返回参数
} SML_RAID_ON_EXISTED_ARRAY_PARAM_S;

// ------------使用SSD硬盘创建CacheCade逻辑盘
typedef struct tag_sml_raid_cachecade_param {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  i_pd_count; // 选择的SSD硬盘数
    guint16 i_pd_sel[SML_MAX_PHYSICAL_DRIVES]; // 选择的SSD硬盘列表
    guint8  i_raid_level; // RAID级别:0/1/5
    gchar   i_ld_name[SML_LD_NAME_LENGTH]; // 逻辑盘的名称，以'\0'结束的ASCII字符串
    guint8  i_write_policy; // 写策略 Write Through/Write Back/Write caching ok if bad BBU
    guint16 i_array_ref;
    guint32 i_capacity; // 容量大小, MB为单位
    guint16 i_associated_ld;
    guint8  i_cache_line_size;

    SML_CREATE_LD_RETURN_PARAM_S o_ret_param; // 返回参数
} SML_RAID_CACHECADE_PARAM_S;


// ------------删除指定的逻辑盘/设置指定逻辑盘为启动盘
typedef struct tag_sml_ld_target {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 指定的逻辑盘ID
    gpointer i_param_ptr; // 扩展参数指针（如指定启动优先级）
    guint32 i_param_size; // 扩展参数大小
} SML_LD_TARGET_S;

// ------------设置逻辑盘名称
typedef struct tag_sml_ld_set_name {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 要设置名称的逻辑盘ID
    gchar   i_ld_name[SML_LD_NAME_LENGTH]; // 逻辑盘的名称，以'\0'结束的ASCII字符串
} SML_LD_SET_NANE_S;

// ------------设置逻辑盘读策略
typedef struct tag_sml_ld_set_read_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_read_policy; // 读策略
    /*
    0 : LD_CACHE_NO_READ_AHEAD,
    1 : LD_CACHE_READ_AHEAD,
    2 : LD_CACHE_ADAPTIVE_READ_ACHEAD
    */
} SML_LD_SET_READ_POLICY_S;

// ------------设置逻辑盘写策略
typedef struct tag_sml_ld_set_write_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_write_policy; // 写策略
    /*
    0 : LD_CACHE_WRITE_THROUGH,
    1 : LD_CACHE_WRITE_BACK,
    2 : LD_CACHE_WRITE_CACHE_IF_BAD_BBU
    */
} SML_LD_SET_WRITE_POLICY_S;

// ------------设置逻辑盘IO策略
typedef struct tag_sml_ld_set_io_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_io_policy; // IO策略
    /*
     0 : LD_CACHE_CACHED_IO,
     1 : LD_CACHE_DIRECT_IO
    */
} SML_LD_SET_IO_POLICY_S;

// ------------设置逻辑盘访问策略
typedef struct tag_sml_ld_set_access_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_access_policy; // 访问策略
    /*
    0:Read Write/1:Read Only/2:Blocked
    */
} SML_LD_SET_ACCESS_POLICY_S;

// ------------设置逻辑盘的物理盘缓存策略
typedef struct tag_sml_ld_set_disk_cache_policy {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_disk_cache_policy; // 物理盘缓存策略
    /*
     0 : MR_PD_CACHE_UNCHANGED      // don't change disk cache policy - use disk's default
     1 : MR_PD_CACHE_ENABLE         // enable disk write cache
     2 : MR_PD_CACHE_DISABLE        // disable disk write cache
    */
} SML_LD_SET_DISK_CACHE_POLICY_S;

// ------------设置逻辑盘后台初始化使能
typedef struct tag_sml_ld_set_bgi_enable {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_bgi_enable; // 后台初始化使能 1:enable 0:disable
} SML_LD_SET_BGI_ENABLE_S;

typedef struct tag_sml_ld_set_accelerator {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_target_id; // 逻辑盘ID
    guint8 i_accelerator; // 加速方法 0:None 1: controller cache 2: IO bypass
} SML_LD_SET_ACCELERATOR_S;

typedef struct tag_sml_ld_set_capacity_size {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_target_id; // 逻辑盘ID
    guint32 i_capacity_size;
} SML_LD_SET_CAPACITY_SIZE_S;

typedef struct tag_sml_ld_set_strip_size {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_target_id; // 逻辑盘ID
    guint8 i_strip_size;
} SML_LD_SET_STRIP_SIZE_S;

/* BEGIN: Modified by zhanglei wx382755, 2017/12/2   PN:AR.SR.SFEA02109379.004.006 */
// ------------开始逻辑盘FGI
typedef struct tag_sml_ld_start_fgi {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  fgi_type; // foreground初始化类型 1:quick init 2:full init
} SML_LD_START_FGI_S;

typedef struct tag_sml_ld_get_fgi_progress {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  fgi_progress; // foreground初始化进度，百分比
} SML_LD_GET_FGI_PROGRESS_S;

typedef struct tag_sml_ld_get_current_fgi_state {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  current_fgi_state; // 是否在foreground初始化中
} SML_LD_GET_CURRENT_FGI_STATE_S;
/* END:   Modified by zhanglei wx382755, 2017/12/2 */

// ------------设置逻辑盘关联/取消关联到指定的CacheCade逻辑盘
typedef struct tag_sml_ld_set_cachecade_association {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  i_target_id; // 逻辑盘ID
    guint8  i_associate; // 是否关联 1:关联 0:不关联 如果关联，则设置关联上所有的CacheCade LD
} SML_LD_SET_CACHECADE_ASSOCIATION_S;

/*
 * Description: APP通过SML获取逻辑盘指定状态的描述和告警级别
*/
const gchar* sml_ld_state2str(guint8 state, guint8 *level);

/*
 * Description: APP通过SML获取逻辑盘指定RAID级别的描述
*/
const gchar* sml_ld_level2str(guint8 level);

/*
 * Description: APP通过SML获取指定CacheCade LD关联的逻辑盘列表
 * History: 2016-11-3 yangshigui (wx386910) 新生成函数
*/
gint32 sml_get_sscd_associated_ld_list(SML_SSCD_ASSOCIATED_LD_LIST_S* ld);

/*
 * Description: APP通过SML获取指定逻辑盘关联的CacheCade LD列表
*/
gint32 sml_get_ld_associated_sscd_list(SML_LD_ASSOCIATED_SSCD_LIST_S* ld);

/*
 * Description: APP通过SML获取指定LD是否关联了CacheCade LD
 * History: 2016-11-18 yangshigui (wx386910) 新生成函数
*/
gint32 sml_get_ld_sscd_caching_enable(SML_LD_SSCD_CACHING_ENABLE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的局部热备PD列表
 * History: 2016-11-23 yangshigui 新生成函数
*/
gint32 sml_get_ld_spared_pd_list(SML_LD_PD_LIST_S* ld);

/*
 * Description: APP通过SML获取逻辑盘的名称
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_name(SML_LD_NAME_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD健康状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_health_status(SML_LD_HEALTH_STATUS_S* ld);

/*
 * Description: APP向SML获取指定RAID控制的LD状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_state(SML_LD_STATE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的RAID级别
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_raid_level(SML_LD_RAID_LEVEL_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD读策略
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_read_policy(SML_LD_READ_POLICY_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD写策略
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_write_policy(SML_LD_WRITE_POLICY_S* ld);

/*
 * Description: APP通过SML获取逻辑盘访问策略
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_access_policy(SML_LD_ACCESS_POLICY_S* ld);

/*
 * Description: APP通过SML获取逻辑盘的初始化操作方式
 * History: 2016-11-26 yangshigui (ywx386910) 新生成函数
*/
gint32 sml_get_ld_init_state(SML_LD_INIT_STATE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD cache策略，包含DiskCache
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_cache_policy(SML_LD_CACHE_POLICY_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的条带大小
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_strip_size(SML_LD_STRIP_SIZE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的容量大小
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_capacity_size(SML_LD_CAPACITY_SIZE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的一致性校验状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_cc_state(SML_LD_CC_STATE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的PD列表
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_pd_list(SML_LD_PD_LIST_S* ld);

/*
 * Description: APP通过SML获取逻辑盘关联的Array
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_ref_array(SML_LD_REF_ARRAY_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的Span depth
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_span_depth(SML_LD_SPAN_DEPTH_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的每个Span的Drive数
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_num_drive_per_span(SML_LD_SPAN_NUM_DRIVERS_S* ld);

/*
 * Description: APP通过SML获取逻辑盘的BGI使能状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_bgi_state(SML_LD_BGI_STATE_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD是否是启动盘
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_boot_priority(SML_LD_BOOT_PRIORITY_S* ld);

/*
 * Description: APP通过SML获取逻辑盘是否是CacheCade逻辑盘
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ld_issscd(SML_LD_IS_SSCD_S* ld);

/*
 * Description: APP向SML获取指定RAID控制器的LD的FGI进度
 * History: 2017-11-22 zhanglei wx382755 新生成函数
*/
gint32 sml_get_ld_fgi_progress(SML_LD_GET_FGI_PROGRESS_S* param);

/*
 * Description: APP向SML获取指定RAID控制器的LD的当前FGI状态
 * History: 2017-11-22 zhanglei wx382755 新生成函数
*/
gint32 sml_get_ld_current_fgi_state(SML_LD_GET_CURRENT_FGI_STATE_S* param);

/*
 * Description: APP向SML获取指定RAID控制器的LD的加速方法
 */
gint32 sml_get_ld_accelerator(SML_LD_ACCELERATOR_S* param);

/*
 * Description: APP向SML获取指定RAID控制器的LD的缓存行大小
 */
gint32 sml_get_ld_cache_line_size(SML_LD_CACHE_LINE_SIZE_S* param);

/*
 * Description: APP向SML获取指定RAID控制器的LD的重构状态
 */
gint32 sml_get_ld_rebuild_status(SML_LD_REBUILD_STATUS_S* param);

/*
 * Description: APP通过SML执行在新的阵列上创建逻辑盘的命令
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_create_ld_on_new_array(SML_RAID_ON_NEW_ARRAY_PARAM_S* config);

/*
 * Description: APP通过SML执行在现有阵列上创建逻辑盘的命令
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_create_ld_on_existed_array(SML_RAID_ON_EXISTED_ARRAY_PARAM_S* config);

/*
 * Description: APP通过SML执行使用SSD硬盘创建CacheCade逻辑盘的命令
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_create_ld_as_cachecade(SML_RAID_CACHECADE_PARAM_S* config);

/*
 * Description: APP通过SML执行删除指定逻辑盘的命令
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_delete(SML_LD_TARGET_S* ld);

/*
 * Description: APP通过SML执行开始指定逻辑盘FGI的命令
 * History: 2017-11-21 zhanglei wx382755 新生成函数
*/
gint32 sml_start_ld_fgi(SML_LD_START_FGI_S* start_fgi);

/*
 * Description: APP通过SML执行取消指定逻辑盘FGI的命令
 * History: 2017-11-21 zhanglei wx382755 新生成函数
*/
gint32 sml_cancel_ld_fgi(SML_LD_TARGET_S* ld);

/*
 * Description: APP通过SML执行设置指定逻辑盘为启动盘的命令
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_boot_priority(SML_LD_TARGET_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的名称
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_name(SML_LD_SET_NANE_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的读策略
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_read_policy(SML_LD_SET_READ_POLICY_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的写策略
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_write_policy(SML_LD_SET_WRITE_POLICY_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的IO策略
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_io_policy(SML_LD_SET_IO_POLICY_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的访问策略
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_access_policy(SML_LD_SET_ACCESS_POLICY_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的物理盘缓存策略
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_disk_cache_policy(SML_LD_SET_DISK_CACHE_POLICY_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的后台初始化使能值
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_bgi_enable(SML_LD_SET_BGI_ENABLE_S* ld);

/*
 * Description: APP通过SML设置指定逻辑盘的加速方法
*/
gint32 sml_set_ld_accelerator(SML_LD_SET_ACCELERATOR_S *ld);

/*
 * Description: APP通过SML设置指定逻辑盘的容量
*/
gint32 sml_set_ld_capacity_size(SML_LD_SET_CAPACITY_SIZE_S *ld);

/*
 * Description: APP通过SML设置指定逻辑盘的条带大小
*/
gint32 sml_set_ld_strip_size(SML_LD_SET_STRIP_SIZE_S *ld);

/*
 * Description: APP通过SML设置指定逻辑盘是否关联到指定的CacheCade逻辑盘
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_set_ld_cachecade_association(SML_LD_SET_CACHECADE_ASSOCIATION_S* ld);

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
