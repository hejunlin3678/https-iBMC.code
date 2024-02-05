/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 命令行相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __CLI_DEFINE_H__
#define __CLI_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define OBVIOUS_CMD 0x01
#define UNOBVIOUS_CMD 0x00


#define HIDE_TYPE_NULL 0
#define HIDE_TYPE_PROP 1
#define HIDE_TYPE_OBJ 2
#define HIDE_TYPE_OBJ_NUM 3
#define HIDE_TYPE_PROP_AND_ACTIVE 5
#define HIDE_TYPE_PROP_REVERSE_AND_ACTIVE 6
#define HIDE_TYPE_OBJ_NUM_AND_ACTIVE 7

#define OPTION_STRING "l:t:d:hT:"
#define CLI_COMFIRM_STRING "Do you want to continue?[Y/N]:"

#define PARAMETER_LEN 64
#define MAX_OPERATE_LOG_LEN 511

#define DELAY_TIME 500

#ifdef ARM64_HI1711_ENABLED
#define UART2_NAME "/dev/ttyS0"
#else
#define UART2_NAME "/dev/ttyAMA0"
#endif

// 命令执行错误码
typedef enum err_type_e {
    FUNCTION_NOT_SUPPORTED = 241,
    MISSING_REQUIRED_TARGET = 242,
    UNRECOGNIZED_OEM_EXTENSION = 243,
    QUEUE_FULL = 244,
    REQUIRED_OPTION_MISSING,
    INVALID_TARGET,
    OPTION_NOT_SUPPORTED,
    MISSING_ARGUMENT,
    OUTPUT_FORMAT_NOT_SUPPORTED,
    INVALID_ARGUMENT,
    INVALID_OPTION,
    COMMAND_SYNTAX_ERROR,
    COMMAND_NOT_RECOGNIZED,
    COMMAND_NOT_SUPPORTED,
    COMMAND_ERROR_UNSPECIFIED = 255
} ERR_TYPE_E;

#pragma pack(1)

typedef struct read_property_t {
    char *name;
    char *alias;
    char value[MAX_PROPERTY_VALUE_LEN];
} READ_PROPERTY_T, *PREAD_PROPERTY_T;

#pragma pack()

struct helpinfo {
    const gchar *name;
    const gchar *desc;
    guchar privilege;
    const gchar *rolepriv; /* 依赖用户角色的某个权限，如KVM权限 */
    guchar syslock_allow;  /* 系统锁定控制：0：禁止，1：允许 */
    guchar flag;
    guint8 HideType; /* 屏蔽类型 */
                     /* 0 :  不依赖 */
                     /* 1 : 依赖属性 */
                     /* 2 : 依赖对象是否存在 */
                     /* 3 : 对象个数 */
                     /* 5 : 依赖属性同时判断是否为主板 */
                     /* 6 : 反向依赖属性同时判断是否为主板 */
    const gchar *DepClassName;    /* 类名 */
    guint32 DepObjectNum;         /* 依赖的对象个数 */
    const gchar *DepPropertyName; /* 属性 */
    const gchar *DepPropetyValue; /* 属性值 */
    /* original function pointer */
    gint32 (*func)(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target);
    /* new function pointer called by shelf management software */
    gint32 (*func_l)(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target,
        gchar *location);
};

struct helpinfoplus {
    struct helpinfo *help_info;
};

struct helpinfoplus_plus {
    struct helpinfoplus *help_infoplus;
};


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CLI_DEFINE_H__ */
