/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2021. All rights reserved.
 * Description: 控制器相关对外提供的数据和接口
 * Author: 汪军锋
 * Create: 2021-2-9
 * Notes: 无
 * History: 2021-2-9 汪军锋 wwx921845 从sml.h拆分出该头文件
 */
#ifndef SML_CTRL_H
#define SML_CRTL_H

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

// -----------------注册RAID管理时需要的数据
typedef struct tag_sml_register_ctrl_oob_info {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  i_controller_typeid; // RAID控制器的类型编号，如 0 : LSI3008 with MR
    union {
        struct {
            guint8  eid;
            guint16 phy_addr;
            MCTP_WRITEREAD_FUNC mctp_writeread_func;
        } over_mctp;
        struct {
            I2C_WRITE_FUNC i2c_write_func;         // 注册的I2CWrite回调接口
            I2C_WRITEREAD_FUNC i2c_writeread_func; // 注册的I2CWriteRead回调接口
        } over_i2c;
    } register_info;
    guint8  i_oob_operate;
    guint8  i_oob_interface_type;
    HANDLE_EVENT_INFO_FUNC handle_event_info_func; // 注册的event回调接口
} SML_CTRL_OOB_INFO_S;

// -----------------控制器产品名称
typedef struct tag_sml_ctrl_name {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    gchar   o_ctrl_name[SML_CTRL_NAME_LENGTH];
} SML_CTRL_NAME_S;

// serialnumber
typedef struct {
    guint8  i_ctrl_idx;
    gchar   o_ctrl_sn[SML_CTRL_SN_LEN];
} SML_CTRL_SN_S;

// -----------------固件版本
typedef struct tag_sml_ctrl_fw_version {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    gchar   o_fw_version[SML_CTRL_FW_VERSION_LENGTH];
} SML_CTRL_FIRMWARE_VERSION_S;

// -----------------热备激活模式
typedef struct tag_sml_ctrl_spare_activation_mode {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint8 o_spare_activation_mode;
    /*
    0: Spare Activation mode is type Failure
    1: Spare Activation mode is type Predictive
    255：No configuration: spare activation mode is invalid
    */
} SML_CTRL_SPARE_ACTIVATION_MODE;

// -----------------PCIe带宽
typedef struct tag_sml_ctrl_pcie_link_width {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint8 o_pcie_link_width;
} SML_CTRL_PCIE_LINK_WIDTH;

// -----------------无电池写缓存设置
typedef struct tag_sml_ctrl_no_battery_write_cache {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint8 o_no_battery_write_cache;
    /*
    0: kControllerNBWCEnabled
    1: kControllerNBWCDisabled
    255：kControllerNBWCInvalid
    */
} SML_CTRL_NO_BATTERY_WRITE_CACHE;

// -----------------硬件版本
typedef struct tag_sml_ctrl_hardware_revision {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    gchar o_hardware_revision[SML_CTRL_HW_REV_LENGTH];
} SML_CTRL_HARDWARE_REVISION;

// -----------------读缓存百分比
typedef struct tag_sml_ctrl_read_cache_percent {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint8 o_read_cache_percent;
} SML_CTRL_READ_CACHE_PERCENT;

typedef struct tag_sml_ctrl_write_cache_policy {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint8 o_write_cache_policy;
} SML_CTRL_WRITE_CACHE_POLICY;

// -----------------NvDATA的版本
typedef struct tag_sml_ctrl_nvdata_version {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    gchar   o_nvdata_version[SML_CTRL_NVDATA_VERSION_LENGTH];
} SML_CTRL_NVDATA_VERSION_S;

// -----------------内存大小
typedef struct tag_sml_ctrl_memory_size {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  o_memory_size;    // in MB
} SML_CTRL_MEMORY_SIZE_S;

// -----------------DDR ECC计数
typedef struct tag_sml_ctrl_ddr_ecc_count {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  o_ecc_count;
} SML_CTRL_DDR_ECC_COUNT_S;

// -----------------SAS地址
typedef struct tag_sml_ctrl_sas_addr {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    gchar   o_sas_addr[SML_SAS_ADDR_LENGTH];
} SML_CTRL_SAS_ADDR_S;

// -----------------SAS速率
typedef struct tag_sml_ctrl_interface_speed {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_if_type;
    /*
    0: SPI
    1: SAS_3G
    2: SATA_1_5G
    3: SATA_3G
    4: SAS_6G
    5: SAS_12G
    255：unknown
    */
} SML_CTRL_INTERFACE_TYPE_S;


// -----------------SAS PHY的信息，不作为接口参数使用
typedef struct tag_sml_sas_phy_info {
    guint8  phy_id;
    guint32 invalid_dword_count;
    guint32 loss_dword_sync_count;
    guint32 phy_reset_problem_count;
    guint32 running_disparity_error_count;
} SML_SAS_PHY_INFO_S;

// -----------------SAS PHY错误统计
typedef struct tag_sml_ctrl_sas_phy_err_count {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  i_force_update; // 设置为1表示强制从RAID卡获取PHY误码数据，0表示采用默认的带缓存机制的策略
    guint8  o_phy_count;
    SML_SAS_PHY_INFO_S  o_phy[SML_MAX_SAS_PHY_PER_CTRL];
} SML_CTRL_SAS_PHY_ERR_S;

// -----------------Expander的PHY误码信息
typedef struct tag_sml_expander_sas_phy_info {
    guint8  phy_count;
    SML_SAS_PHY_INFO_S  phy_info[SML_MAX_SAS_PHY_PER_EXPANDER];
} SML_EXP_SAS_PHY_INFO_S;

// -----------------RAID卡下的Expander的PHY误码信息
typedef struct tag_sml_ctrl_expander_sas_phy_info {
    guint8  i_controller_index;
    guint8  o_expander_count;
    SML_EXP_SAS_PHY_INFO_S  o_expander_phy[SML_MAX_EXPANDER_PER_CONTROLLER];
} SML_CTRL_EXP_SAS_PHY_INFO_S;

// -----------------RAID控制器是否有cache pinned
typedef struct tag_sml_ctrl_cache_pinned {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_is_cache_pinned;
    /*
    0: no cache pinned
    1：cache pinned
    other: reserved
    */
} SML_CTRL_CACHE_PINNED_STATE_S;

// -----------------RAID控制器是否开启硬盘故障记忆
typedef struct tag_sml_ctrl_maint_pd_fail_history {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_maint_pd_fail_history_enable;
    /*
    0: disabled
    1：enabled
    other: reserved
    */
} SML_CTRL_PD_FAIL_HISTORY_STATE_S;

// -----------------RAID控制器支持的条带大小范围
typedef struct tag_sml_ctrl_strip_size_options {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_min_strip_support;
    guint8  o_max_strip_support;
} SML_CTRL_STRIP_SIZE_OPTIONS_S;

// -----------------RAID控制器的属性信息
typedef struct tag_sml_ctrl_properties_state {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_copyback_enabled;
    guint8  o_smarter_copyback_enabled;
    guint8  o_jbod_enabled;
    guint8  o_consis_check_enabled;
    guint16  o_consis_check_period;
    guint8  o_consis_check_rate;
    guint8  o_consis_check_repair;
    guint8  o_consis_check_status;
    guint16  o_consis_check_totalvd;
    guint16  o_consis_check_completedvd;
    guint32 o_consis_check_delay;
} SML_CTRL_PROPERTIES_STATE_S;

// -----------------RAID的健康状态
typedef struct tag_sml_ctrl_health_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16  o_health_status_code;
    /*
    bit0: 1- 内存有可纠正错误
    bit1: 1- 内存有不可纠正错误
    bit2: 1- 内存有ECC错误达到阈值
    bit3: 1- NVRAM有不可纠正错误
    bit4: reserved
    bit5: 1- pg信号异常
    bit6: 1- 内部模块检测故障（导致IO停止）
    bit7: 1- 内部模块参考时钟故障
    bit8: 1- 内部电源电压故障
    bit9: 1- 内部备电模块电容故障
    bit10: 1- 内部flash模块故障
    bit11-15: reserved
    */
    guint16  o_hw_err;
    guint8  o_clk_err;
    guint8  o_power_err;
    guint8  o_capacity_err;
    guint8  o_flash_err;
    guint8  o_err_change;
} SML_CTRL_HEALTH_STATUS_S;

// -----------------RAID的BBU状态
typedef struct tag_sml_ctrl_bbu_status {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_bbu_present;
    /** BEGIN: Added by cwx290152, 2017/11/21   PN:DTS2017101812710 */
    guint16 o_temperature;
    /** END:   Added by cwx290152, 2017/11/21 */
    gchar   o_bbu_type[SML_BBU_TYPE_LENGTH];
#ifdef BD_BIG_ENDIAN
    guint32  o_reserved : 22;
    guint32  o_failed : 1;
    guint32  o_no_space : 1;
    guint32  o_remaining_capacity_low : 1;
    guint32  o_predictive_failure : 1;
    guint32  o_learn_cycle_timeout : 1;
    guint32  o_learn_cycle_failed : 1;
    guint32  o_replace_pack : 1;
    guint32  o_temperature_high : 1;
    guint32  o_voltage_low : 1;
    guint32  o_pack_missing : 1;
#else
    guint32  o_pack_missing : 1;
    guint32  o_voltage_low : 1;
    guint32  o_temperature_high : 1;
    guint32  o_replace_pack : 1;
    guint32  o_learn_cycle_failed : 1;
    guint32  o_learn_cycle_timeout : 1;
    guint32  o_predictive_failure : 1;
    guint32  o_remaining_capacity_low : 1;
    guint32  o_no_space : 1;
    guint32  o_failed : 1;
    guint32  o_reserved : 22;
#endif
} SML_CTRL_BBU_STATUS_S;

// -----------------RAID模式
typedef struct tag_sml_ctrl_mode {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_mode;
    /*
    0: non RAID(HBA)
    1：RAID
    other: reserved
    */
} SML_CTRL_MODE_S;

/* BEGIN: Added by Yangshigui YWX386910, 2017/4/11   问题单号:AR-0000276589-001-005 */
// -----------------RAID控制器温度
typedef struct tag_sml_ctrl_temp {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_ctrl_temp; // 控制器温度，摄氏度，255为无效值
} SML_CTRL_TEMP_S;
/* END:   Added by Yangshigui YWX386910, 2017/4/11 */

// -----------------RAID控制器管理的LD/PD总数
typedef struct tag_sml_ctrl_ld_pd_count {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 o_ld_present_count;
    guint16 o_ld_degraded_count;
    guint16 o_ld_offline_count;
    guint16 o_pd_present_count;
    guint16 o_pd_disk_present_count;
    guint16 o_pd_disk_predfailure_count;
    guint16 o_pd_disk_failed_count;
} SML_CTRL_LD_PD_COUNT_S;

// -----------------RAID控制器管理的LD列表
typedef struct tag_sml_ctrl_ld_list {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 o_ld_count;
    guint16 o_ld_targetIDs[SML_MAX_LOGIC_DRIVES];
} SML_CTRL_LD_LIST_S;

// -----------------RAID控制器管理的PD列表
typedef struct tag_sml_ctrl_pd_list {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 o_pd_count;
    guint16 o_pd_deviceIDs[SML_MAX_PHYSICAL_DRIVES];
    guint8  o_pd_slot_num[SML_MAX_PHYSICAL_DRIVES];
    /* BEGIN: Modified by gwx455466, 2017/9/4 14:35:35   问题单号:DTS2017083101891  */
    guint16 o_pd_enclosure_id[SML_MAX_PHYSICAL_DRIVES];
    /* END:   Modified by gwx455466, 2017/9/4 14:35:39 */
} SML_CTRL_PD_LIST_S;

// -----------------RAID控制器启动模式
typedef struct tag_sml_ctrl_boot_mode {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_boot_mode;
    /*
    0:CTRL_BIOS_BOOT_MODE_SOE,  Stop On Errors
    1:CTRL_BIOS_BOOT_MODE_POE,  Pause On Errors
    2:CTRL_BIOS_BOOT_MODE_IOE,  Ignore On Errors
    3:CTRL_BIOS_BOOT_MODE_HSM,  Headless Safe Mode
    255:CTRL_BIOS_BOOT_MODE_INVALID
    */
} SML_CTRL_BOOT_MODE_S;

typedef struct tag_sml_ctrl_boot_devices {
    guint8 i_controller_index; // RAID控制器的索引号，从0开始
    guint32 o_bootable_devices[SML_MAX_BOOTABLE_DEVICES];
} SML_CTRL_BOOTABLE_DEVICES_S;

/* Yangshigui YWX386910, 2016/11/8   问题单号:AR-0000264511-001-003 */
// ------------控制器Array列表
typedef struct tag_sml_ctrl_array_list {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint8  o_array_count; // 获取到的Array个数
    guint16 o_array_refs[SML_MAX_ARRAY]; // Array ID列表
} SML_CTRL_ARRAY_LIST_S;

// ------------获取指定Array的信息
typedef struct tag_sml_ctrl_array_info {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_array_ref; // 要获取信息的Array ID
    guint32 o_used_space; // 已用空间，单位MB
    guint32 o_total_free_space; // 可用空间之和，单位MB
    guint32 o_free_blocks_count; // 空闲块的数量
    guint32 o_free_blocks_space[SML_MAX_HOLES_IN_ARRAY]; // 每个空闲块的容量，单位MB
    guint8  o_ld_count; // Array上的逻辑盘数
    guint16  o_ld_ids[SML_MAX_LOGICAL_DRIVES_PER_ARRAY]; // Array上逻辑盘ID列表
    guint8  o_pd_count; // 组成这个Array的物理盘个数
    guint16 o_pd_ids[SML_MAX_PHYSICAL_DRIVES_PER_ARRAY]; // 组成这个Array的物理盘ID列表
    guint8  o_pd_slots[SML_MAX_PHYSICAL_DRIVES_PER_ARRAY]; // 组成这个Array的物理盘槽位号列表
    guint16 o_pd_enclosure[SML_MAX_PHYSICAL_DRIVES_PER_ARRAY]; // 组成这个Array的物理盘槽位号列表
} SML_CTRL_ARRAY_INFO_S;
/* END:   Added by Yangshigui YWX386910, 2016/11/8 */

/* BEGIN: Added by Yangshigui YWX386910, 2017/4/5   问题单号:DTS2017032104172 */
// ------------获取控制器支持的操作选项的信息
typedef struct tag_sml_ctrl_support_operations {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始

    struct {
#ifdef BD_BIG_ENDIAN
        guint32 o_reserved : 8;
        guint32 o_hba_drive_wcp : 2;
        guint32 o_unconfigured_drive_wcp : 2;
        guint32 o_configured_drive_wcp : 2;
        guint32 o_support_epd : 1;
        guint32 o_support_mixed : 1;
        guint32 o_support_jbod : 1;
        guint32 o_support_hba : 1;
        guint32 o_support_raid : 1;
        guint32 o_support_mode_set : 1;
        guint32 o_support_jbod_state : 1;
        guint32 o_support_raid10triple : 1;
        guint32 o_support_raid1triple : 1;
        guint32 o_support_raid10adm : 1;
        guint32 o_support_raid1adm : 1;
        guint32 o_support_raid60 : 1;
        guint32 o_support_raid50 : 1;
        guint32 o_support_raid10 : 1;
        guint32 o_support_raid6 : 1;
        guint32 o_support_raid5 : 1;
        guint32 o_support_raid1 : 1;
        guint32 o_support_raid0 : 1;
#else
        guint32 o_support_raid0 : 1;
        guint32 o_support_raid1 : 1;
        guint32 o_support_raid5 : 1;
        guint32 o_support_raid6 : 1;
        guint32 o_support_raid10 : 1;
        guint32 o_support_raid50 : 1;
        guint32 o_support_raid60 : 1;
        guint32 o_support_raid1adm : 1;
        guint32 o_support_raid10adm : 1;
        guint32 o_support_raid1triple : 1;
        guint32 o_support_raid10triple : 1;
        guint32 o_support_jbod_state : 1;
        guint32 o_support_mode_set : 1;
        guint32 o_support_raid : 1;
        guint32 o_support_hba : 1;
        guint32 o_support_jbod : 1;
        guint32 o_support_mixed : 1;
        guint32 o_support_epd : 1;
        guint32 o_configured_drive_wcp : 2;
        guint32 o_unconfigured_drive_wcp : 2;
        guint32 o_hba_drive_wcp : 2;
        guint32 o_reserved : 8;
#endif
    } ctrl_operations;

    /* BEGIN: Modified by Yangshigui YWX386910, 2017/4/5   问题单号:DTS2017032104172 */
    struct {
#ifdef BD_BIG_ENDIAN
        guint32 o_reserved : 25;
        guint32 o_default_write_policy : 2;
        guint32 o_support_disk_cache_policy : 1;
        guint32 o_support_access_policy : 1;
        guint32 o_support_io_policy : 1;
        guint32 o_support_write_policy : 1;
        guint32 o_support_read_policy : 1;
#else
        guint32 o_support_read_policy : 1;
        guint32 o_support_write_policy : 1;
        guint32 o_support_io_policy : 1;
        guint32 o_support_access_policy : 1;
        guint32 o_support_disk_cache_policy : 1;
        guint32 o_default_write_policy : 2;
        guint32 o_reserved : 25;
#endif
    } ld_operations;
    /* END:   Modified by Yangshigui YWX386910, 2017/4/5 */

    struct {
#ifdef BD_BIG_ENDIAN
        /* BEGIN  Added by zwx566676, AR.SR.SFEA02130924.051.001 加密盘安全擦除 , 2018/9/27  */
        guint32 o_reserved : 30;
        guint32 o_support_crypto_erase : 1;
        /* END:   Added by zwx566676, 2018/9/27 */
        guint32 o_support_temperature : 1;
#else
        guint32 o_support_temperature : 1;
        /* BEGIN  Added by zwx566676, AR.SR.SFEA02130924.051.001 加密盘安全擦除 , 2018/9/27  */
        guint32 o_support_crypto_erase : 1;
        guint32 o_reserved : 30;
        /* END:   Added by zwx566676, 2018/9/27 */
#endif
    } pd_operations;
} SML_CTRL_SUPPORT_OPERATIONS_S;

/*
 * Description: APP向SML注册RAID控制器的管理信息，SML会初始化对应的全局数据，并
                根据注册信息调用第三方的初始化接口
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_register_controller(SML_CTRL_OOB_INFO_S* ctrl);

/*
 * Description: APP通知SML退出RAID控制器的管理，SML会清除全局数据，并调用第三方的
                清理接口
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_unregister_controller(SML_CTRL_OOB_INFO_S* ctrl);

/*
 * Description: APP根据需要调用SML接口刷新第三方软件lib中的缓存数据
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_update_cache(guint8 ctrl_index);

/*
 * Description: APP调用该接口清空缓存的控制器信息，在退出带外管理时调用
 * History: 2018-05-8 zhanglei zwx382755  新生成函数
*/
void sml_clear_all_controller_info(void);

/*
 * Description: 模拟在诊断信息链表中插入Expander通信故障诊断事件
 * History: 2019-01-09 x00453037 新生成函数
*/
gint32 sml_mock_ctrl_event(guint8 ctrl_index, guint8 mock_count,
    SML_MOCK_CTRL_EVENT_S* mock_event);

/*
 * Description: 模拟获取phy诊断的拓扑结构信息
 * History: 2019-01-26 x00453037 新生成函数
*/
gint32 sml_mock_get_phy_topo_info(guint8 ctrl_index, guint16 pd_device_id);

/*
 * Description: 模拟从文件中增加phy误码历史数据
 * History: 2019-01-29 x00453037 新生成函数
*/
gint32 sml_mock_add_phy_err(guint8 ctrl_index, const gchar* file_path);

/*
 * Description: 获取RAID控制器的初始化状态
 * History: 2016-02-26 r00355870 新生成函数 DTS2020010312097
*/
guint8 sml_get_ctrl_init_state(guint8 ctrl_index);

/*
 * Description: 获取RAID控制器的名称
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_name(SML_CTRL_NAME_S* ctrl);

/*
 * Description : 获取RAID控制器的serial number
 */
gint32 sml_get_ctrl_sn(SML_CTRL_SN_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的固件版本信息
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_fw_version(SML_CTRL_FIRMWARE_VERSION_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的配置数据版本
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_nvdata_version(SML_CTRL_NVDATA_VERSION_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的内存大小
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_memory_size(SML_CTRL_MEMORY_SIZE_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的内存ECC统计
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_ecc_count(SML_CTRL_DDR_ECC_COUNT_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的SAS地址
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_sas_addr(SML_CTRL_SAS_ADDR_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的接口类型，如SAS_6G,SAS_12G等
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_device_interface(SML_CTRL_INTERFACE_TYPE_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的PHY错误统计
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_sas_phy_err_count(SML_CTRL_SAS_PHY_ERR_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器下的Expander的PHY错误统计
 * History: 2018-10-31 x00453037 新生成函数 AR.SR.SFEA02130924.009.011
*/
gint32 sml_get_ctrl_exp_sas_phy_err_count(SML_CTRL_EXP_SAS_PHY_INFO_S* expander);

/*
 * Description: APP向SML获取RAID控制器是否有cache pinned
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_cache_pinned(SML_CTRL_CACHE_PINNED_STATE_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器是否开启硬盘故障记忆
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_pd_fail_history(SML_CTRL_PD_FAIL_HISTORY_STATE_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的健康状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_health_status(SML_CTRL_HEALTH_STATUS_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的BBU状态
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_bbu_status(SML_CTRL_BBU_STATUS_S* ctrl);

/*
 * Description: APP向SML获取控制器的模式
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_mode(SML_CTRL_MODE_S* ctrl);

/*
 * Description: APP向SML获取控制器的温度
 * History: 2017-04-11 yangshigui ywx386910 新生成函数
*/
gint32 sml_get_ctrl_temp(SML_CTRL_TEMP_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的LD和PD总数
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_ld_and_pd_count(SML_CTRL_LD_PD_COUNT_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的LD列表
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_ld_list(SML_CTRL_LD_LIST_S* ctrl);

/*
 * Description: APP向SML获取RAID控制器的PD列表
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_pd_list(SML_CTRL_PD_LIST_S* ctrl);

/*
 * Description: 获取RAID控制器支持的条带大小范围
 * History: 2016-02-26 huanghan 新生成函数
*/
gint32 sml_get_ctrl_strip_szie_options(SML_CTRL_STRIP_SIZE_OPTIONS_S* ctrl);

/*
 * Description: APP通过SML获取控制器的属性
 * History: 2016-11-03 yangshigui (wx386910) 新生成函数
*/
gint32 sml_get_ctrl_properties(SML_CTRL_PROPERTIES_STATE_S* ctrl);

/*
 * Description: APP通过SML获取指定RAID控制器下的array列表
 * History: 2016-11-03 yangshigui (wx386910) 新生成函数
*/
gint32 sml_get_ctrl_array_list(SML_CTRL_ARRAY_LIST_S* ctrl);

/*
 * Description: APP通过SML获取指定RAID控制器的BIOS启动模式
 * History: 2018-06-27 renlele r00355870 新生成函数 DTS2018062710957
*/
gint32 sml_get_ctrl_boot_mode(SML_CTRL_BOOT_MODE_S* ctrl);

/*
 * Description: APP通过SML获取指定RAID控制器下的特定Array的信息
 * History: 2017-04-05 yangshigui (wx386910) 新生成函数
*/
gint32 sml_get_ctrl_support_operations(SML_CTRL_SUPPORT_OPERATIONS_S* ctrl);

/*
 * Description: APP通过SML获取指定RAID控制器的启动盘
 */
gint32 sml_get_ctrl_boot_devices(SML_CTRL_BOOTABLE_DEVICES_S *boot_devices);

/*
 * Description: APP通过SML获取指定array的信息
 * History: 2016-11-3 yangshigui (wx386910) 新生成函数
*/
gint32 sml_get_array_info(SML_CTRL_ARRAY_INFO_S* array);

guint8 sml_raid_ctrl_mode_str2num(const gchar* name);
const gchar* sml_ctrl_wcp_type_num2str(guint8 type);
guint8 sml_raid_ctrl_wcp_str2num(const gchar* name);
const gchar* sml_raid_ctrl_mode_num2str(guint8 mode);
const gchar* sml_ctrl_drive_wcp2str(guint8 wcp);
gint32 sml_get_ctrl_spare_activation_mode(SML_CTRL_SPARE_ACTIVATION_MODE *ctrl);
gint32 sml_get_ctrl_hardware_revision(SML_CTRL_HARDWARE_REVISION *ctrl);
gint32 sml_get_ctrl_pcie_link_width(SML_CTRL_PCIE_LINK_WIDTH *ctrl);
gint32 sml_get_ctrl_no_battery_write_cache(SML_CTRL_NO_BATTERY_WRITE_CACHE *ctrl);
gint32 sml_get_ctrl_read_cache_percent(SML_CTRL_READ_CACHE_PERCENT *ctrl);

/*
 * Description: 启动优先级从字符串转数字
 */
guint8 sml_ctrl_boot_priority_str2num(const gchar* str);

/*
 * Description: 启动优先级从数字转字符串
 */
const gchar* sml_ctrl_boot_priority_num2str(guint8 priority);

/*
 * Description: 一致性校验速度从字符串转数字
 * History: 2022年5月16日  luyizou (l00651878)  新生成函数
*/
guint8 sml_ctrl_ccheck_rate_str2num(const gchar* speed);

/*
 * Description: 一致性校验速度从数字转字符串
 * History: 2022年5月16日  luyizou (l00651878)  新生成函数
*/
const gchar* sml_ctrl_ccheck_rate_num2str(guint8 rate);

/*
 * Description: 一致性校验运行状态从字符串转数字
 * History: 2022年5月16日  luyizou (l00651878)  新生成函数
*/
guint8 sml_ctrl_ccheck_status_str2num(const gchar* status_s);

/*
 * Description: 一致性校验运行状态从数字转字符串
 * History: 2022年5月16日  luyizou (l00651878)  新生成函数
*/
const gchar* sml_ctrl_ccheck_status_num2str(guint8 status);

/*
 * Description: APP通过SML对PD执行操作
 * History: 2016-11-3 yangshigui wx386910 新生成函数
*/
gint32 sml_ctrl_operation(SML_CTRL_OPERTATION_S* ctrl);

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
