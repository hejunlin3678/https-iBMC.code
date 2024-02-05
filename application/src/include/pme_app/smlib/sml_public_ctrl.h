/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: 有关CTRL的结构体
 * Author:
 * Create: 2022-3-24
 * Notes: 无
 * History: 2022-3-24  从sml_public.h拆分出该头文件
 */
#ifndef SML_PUBLIC_CTRL_H
#define SML_PUBLIC_CTRL_H
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml_common.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#pragma pack(1)

typedef struct _tag_sml_ctrl_health {
    guint32 last_update_timestamp;
    /* BEGIN: Added by Yangshigui YWX386910, 2017/4/19   问题单号:DTS2017041911858 */
    guint8 ctrl_nvram_error_reported; // 标记是否上报过控制器nvram error错误
    guint8 clk_err;
    /* END:   Added by Yangshigui YWX386910, 2017/4/19 */
    guint16 health_status;
    guint16 hw_err;
    guint8 power_err;
    guint8 capacity_err;
    guint8 flash_err;
    // err_change 用来记录新增五个故障的状态变化，防止log刷屏
    guint8 err_change;
    guint16 reserved;
} SML_CTRL_HEALTH_S;

/* BEGIN: Added by Yangshigui YWX386910, 2017/1/11   问题单号:DTS2017010501008 */
typedef struct _tag_sml_ctrl_health_data {
    /* BEGIN: Added by Yangshigui YWX386910, 2017/4/19   问题单号:DTS2017041911858 */
    guint8 ctrl_nvram_error_reported; // 标记是否上报过控制器nvram error错误
    /* END:   Added by Yangshigui YWX386910, 2017/4/19 */
    guint16 ctrl_nvram_err_count;
    guint8 bbu_present;
    // 以下为新增五个故障
    guint16 ctrl_hw_err;
    guint8 ctrl_clk_err;
    guint8 ctrl_power_err;
    guint8 ctrl_capacity_err;
    guint8 ctrl_flash_err;
} SML_CTRL_HEALTH_DATA_S;
/* END:   Added by Yangshigui YWX386910, 2017/1/11 */

typedef struct _tag_sml_ctrl_operations {
    struct {
#ifdef BD_BIG_ENDIAN
        guint32 reserved : 19;
        guint32 hba_drive_wcp : 2;
        guint32 unconfigured_drive_wcp : 2;
        guint32 configured_drive_wcp : 2;
        guint32 support_epd : 1;
        guint32 support_mixed : 1;
        guint32 support_hba : 1;
        guint32 support_raid : 1;
        guint32 support_mode_set : 1;
        guint32 support_jbod_state : 1;
        guint32 support_jbod : 1;
#else
        guint32 support_jbod : 1;
        guint32 support_jbod_state : 1;
        guint32 support_mode_set : 1;
        guint32 support_raid : 1;
        guint32 support_hba : 1;
        guint32 support_mixed : 1;
        guint32 support_epd : 1;
        guint32 configured_drive_wcp : 2;
        guint32 unconfigured_drive_wcp : 2;
        guint32 hba_drive_wcp : 2;
        guint32 reserved : 19;
#endif
    } ctrl_operations;

    /* BEGIN: Modified by Yangshigui YWX386910, 2017/4/5   问题单号:DTS2017032104172 */
    struct {
#ifdef BD_BIG_ENDIAN
        guint32 reserved : 25;
        guint32 default_write_policy : 2;
        guint32 support_disk_cache_policy : 1;
        guint32 support_access_policy : 1;
        guint32 support_io_policy : 1;
        guint32 support_write_policy : 1;
        guint32 support_read_policy : 1;
#else
        guint32 support_read_policy : 1;
        guint32 support_write_policy : 1;
        guint32 support_io_policy : 1;
        guint32 support_access_policy : 1;
        guint32 support_disk_cache_policy : 1;
        guint32 default_write_policy : 2;
        guint32 reserved : 25;
#endif
    } ld_operations;
    /* END:   Modified by Yangshigui YWX386910, 2017/4/5 */

    struct {
#ifdef BD_BIG_ENDIAN
        /* BEGIN  Added by zwx566676, AR.SR.SFEA02130924.051.001 加密盘安全擦除 , 2018/9/27 */
        guint32 reserved : 30;
        guint32 support_crypto_erase : 1;
        /* END:   Added by zwx566676, 2018/9/27 */
        guint32 support_temperature : 1;
#else
        guint32 support_temperature : 1;
        /* BEGIN  Added by zwx566676, AR.SR.SFEA02130924.051.001 加密盘安全擦除 , 2018/9/27 */
        guint32 support_crypto_erase : 1;
        guint32 reserved : 30;
        /* END:   Added by zwx566676, 2018/9/27 */
#endif
    } pd_operations;
} SML_CTRL_OPERATIONS_S;

typedef struct _tag_sml_ctrl_properties {
#ifdef BD_BIG_ENDIAN
    guint32 reserved : 29;
    guint32 jbod_enabled : 1;
    guint32 smarter_copyback_enabled : 1;
    guint32 copyback_enabled : 1;

#else
    guint32 copyback_enabled : 1;
    guint32 smarter_copyback_enabled : 1;
    guint32 jbod_enabled : 1;
    guint32 reserved : 29;
#endif
} SML_CTRL_PROPERTIES_S;

typedef struct _tag_sml_ctrl_basic_info {
    guint32 last_update_timestamp;
    SML_CTRL_PROPERTIES_S properties;
    SML_CTRL_OPERATIONS_S operations;
    gchar ctrl_name[SML_CTRL_NAME_LENGTH];
    gchar ctrl_sn[SML_CTRL_SN_LEN];
    gchar fw_version[SML_CTRL_FW_VERSION_LENGTH];
    gchar nvdata_version[SML_CTRL_NVDATA_VERSION_LENGTH];
    gchar hardware_revision[SML_CTRL_HW_REV_LENGTH];
    guint16 ld_present_conut;
    guint16 ld_degraded_count;
    guint16 ld_offline_count;
    guint16 pd_present_count;
    guint16 pddisk_present_count;
    guint16 pddisk_prefail_count;
    guint16 pddisk_fail_count;
    guint16 memory_size;
    guint16 memory_ecc_count;
    guint16 memory_ecc_bucket_size;
    guint16 memory_ce_count;  // correctable error
    guint16 memory_uce_count; // uncorrectable error

    // 以下增加的为创建逻辑盘过程中需要的控制器信息
    guint16 max_pd_raid0;
    guint16 max_pd_raid1;
    guint16 max_pd_raid5;
    guint16 max_pd_raid6;
    guint16 max_pd_raid10;
    guint16 max_pd_raid50;
    guint16 max_pd_raid60;

    guint8 max_lds_per_array;
    guint8 max_lds;

    guint8 allow_mix_ssd_hdd;
    guint8 allow_ssd_mix;

    guint8 raid0_supported;
    guint8 raid1_supported;
    guint8 raid5_supported;
    guint8 raid6_supported;
    guint8 raid10_supported;
    guint8 raid50_supported;
    guint8 raid60_supported;
    guint8 raid1adm_supported;
    guint8 raid10adm_supported;
    guint8 raid1triple_supported;
    guint8 raid10triple_supported;

    guint8 ssc_raid0_unsupported;
    guint8 ssc_raid1_supported;
    guint8 ssc_raid5_supported;

    guint8 min_pd_raid0;
    guint8 min_pd_raid1;
    guint8 min_pd_raid5;
    guint8 min_pd_raid6;
    guint8 min_pd_raid10;
    guint8 min_pd_raid50;
    guint8 min_pd_raid60;

    guint8 ctrl_temp;
    guint8 mode;
    guint8 spare_activation_mode; // spare activation mode
    guint8 pcie_link_width;
    guint8 nobattery_write_cache;
    guint8 read_cache_percent;
    guint8 write_cache_policy;
    guint8 ctrl_warnig_info_reported; // 标记控制器告警信息是否已经上报
    guint8 cache_pinned;
    guint8 maint_pd_fail_history;
    guint8 device_interface;
    guint8 min_strip;
    guint8 max_strip;

    // 以下为一致性校验所涉及的控制器信息
    guint8 consis_check_enabled;
    guint16 consis_check_period;
    guint8 consis_check_rate;
    guint8 consis_check_repair;
    guint8 consis_check_status;
    guint16 consis_check_totalvd;
    guint16 consis_check_completedvd;
    guint32 consis_check_delay;

    guint8 rescerved[2]; // 四字节对齐预留字段
    /* 注意：此数据结构需要保持四字节对齐 */
} SML_CTRL_BASIC_INFO_S;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif