/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2021. All rights reserved.
 * Description: 带外管理相关对外提供的数据结构和接口
 * Author: 汪军锋
 * Create: 2021-2-9
 * Notes: 无
 * History: 2021-2-9 汪军锋 wwx921845 从sml.h拆分出该头文件
 */
#ifndef SML_OOB_H
#define SML_OOB_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define SML_ADD_CTRL         0
#define SML_REGISTER_CTRL    1
#define SML_DEL_CTRL         2
#define SML_UNREGISTER_CTRL  3
#define SML_MAX_STR_BUF_SIZE 64
#define SML_MAX_DETAIL_DESC_SIZE 512

#pragma pack(1)

/***************************************************************/
/* 设置类操作使用到的数据结构接声明 */
/* 变量名称前缀 i - input,表示在调用对应的接口时此变量是输入参数 */
/* 变量名称前缀 o - output,表示在调用对应的接口时此变量是是输出参数 */
/**************************************************************/

// ------------对PD执行操作
typedef struct tag_sml_pd_operation {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    guint16 i_device_id; // PD的设备ID，唯一标识
    guint16 i_operation;
    gpointer i_param_ptr;
    guint32 i_param_size;
} SML_PD_OPERTATION_S;
/*
i_operation取值:
PD_OPERATION_LOCATE                  = 0,
PD_OPERATION_STOP_LOCATE             = 1,
PD_OPERATION_SET_GLOBAL_HOTSPARE     = 2, //设置PD为全局热备盘
PD_OPERATION_SET_DEDICATED_HOTSPARE  = 3, //设置PD为局部热备盘
PD_OPERATION_CANCEL_HOTSPARE         = 4, //取消PD的热备盘设置
PD_OPERATION_SET_STATE               = 5, //设置PD的固件状态Online/Offline/JBOD/UG，状态与SML_PD_STATE_E类型对应

设置为PD_OPERATION_LOCATE
i_param_ptr为指向guint8*，参数为硬盘定位持续时间

设置为PD_OPERATION_SET_DEDICATED_HOTSPARE
i_param_ptr为指向guint8*，参数为要设置热备的逻辑盘ID

设置为PD_OPERATION_SET_STATE
i_param_ptr为指向guint8*，参数为硬盘要设置的状态,
6:PD_STATE_ONLINE 3:PD_STATE_OFFLINE 8:PD_STATE_SYSTEM(JBOD) 0:PD_STATE_UNCONFIGURED_GOOD(UG)
*/

// ------------对RAID控制器执行操作
typedef struct tag_sml_ctrl_operation {
    guint8  i_controller_index; // RAID控制器的索引号，从0开始
    /*
    i_operation                                        i_param_ptr
    CTRL_OPERATION_DISABLE_COPYBACK      = 0,          guint8* disable 1:禁止copyback 0:使能copyback
    CTRL_OPERATION_ENABLE_SMART_COPYBACK = 1,          guint8* enable  1:使能SMART copyback 0:禁止SMART copyback
    CTRL_OPERATION_ENABLE_JBOD           = 2,          guint8* enable  1:使能JBOD 0:禁止JBOD
    CTRL_OPERATION_RESTORE_DEFAULT       = 3,          无参数
    CTRL_OPERATION_SET_BIOS_BOOT_MODE    = 4,          guint8* 参考SML_CTRL_BIOS_BOOT_MODE_E
    */
    guint16 i_operation;
    gpointer i_param_ptr;
    guint32 i_param_size;
} SML_CTRL_OPERTATION_S;

typedef struct _tag_sml_ctrl_personality_mode {
    guint8 mode;
    guint8 profile_id;
}SML_CTRL_PERSONALITY_MODE;

typedef struct _tag_sml_ctrl_log_data {
    guint8 log_type;
    guint8* log_buffer;
    gsize log_length;
} SML_CTRL_LOG_DATA_S;

typedef struct _tag_sml_ctrl_set_consis_check_param {
    guint8 sw;
    guint16 period;
    guint8 rate;
    guint8 repair;
    guint32 delay;
    guint8 mask;
} SML_CTRL_SET_CONSIS_CHECK_PARAM;

#pragma pack()

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif
