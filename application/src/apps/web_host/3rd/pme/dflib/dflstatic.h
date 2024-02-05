/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description: app应用模块代码模板中使用的函数，只能静态调用
               用户app模块代码中请不要使用下述函数接口,不能在运行过程中再次调用
 * Author: h00168999
 * Create: 2018/12/22
 * Notes: dflstatic.h
 * History:
 */

#ifndef __DFLSTATIC_H__
#define __DFLSTATIC_H__

#include "dflapi.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/*
 * Description: 静态注册模块
 * Notes: module_name全部由字母组成，不含空格
 */
extern gint32 static_register_module(const gchar *module_name);

/*
 * Description: app启动rpc服务  创建模块初始化启动线程
 */
extern void static_start_rpc_service(void);

/*
 * Description: 记录初始化函数地址，并创建线程启动
 */
typedef gint32 (*MODULE_INIT_FUNC)(void);
extern gint32 static_module_init(gint order, MODULE_INIT_FUNC handler);

/*
 * Description: 注册交叉初始化回调函数
 */
extern gint32 static_module_cross_init(MODULE_INIT_FUNC handler);

/*
 * Description: 记录模块启动运行函数
 */
typedef gint32 (*MODULE_START_FUNC)(void);
extern gint32 static_module_start(MODULE_START_FUNC handler, gint32 delay);

typedef gint32 (*DEBUG_CMD_FUNC)(GSList *input_list);
typedef struct tag_debug_cmd {
    // 调试命令名称
    const gchar *debug_cmd_name;
    // 调试命令回调函数
    DEBUG_CMD_FUNC debug_fun;
    // 参数列表  同GVariantClass, 例如"uin"表示3个参数，依次为uint32,int32,int16
    const gchar *parameter_str;
    // 调试命令描述帮助
    const gchar *description;
} DEBUG_CMD_S;

/*
 * Description: 注册DEBUG命令字的回调处理函数
 */
extern gint32 static_register_debug_cmd(gsize size, DEBUG_CMD_S *debug_cmd_table);

/*
 * Description: 静态注册当前模块的类
 */
extern gint32 static_declaration_class(const gchar *class_name);

/*
 * Description: 静态注册类方法 ，需要先注册类
 */
typedef gint32 (*CLASS_METHOD_FUNC)(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 static_declaration_class_method(const gchar *class_name, const gchar *method_name,
    CLASS_METHOD_FUNC method);

/*
 * Description: 注册DUMP命令字的回调处理函数
 */
typedef gint32 (*DUMP_FUNC)(const gchar *path);
extern gint32 static_register_dump_handler(DUMP_FUNC handler);

typedef gint32 (*IPMI_CMD_FUNC)(gconstpointer pdata, gpointer user_data);
typedef struct tag_ipmi_data {
    // ipmi命令网络功能码
    guint8 netfn;
    // ipmi命令字
    guint8 cmd;
    // 过滤字符串
    const gchar *filter;
    // 命令权限
    guint8 privilege;
    // 命令属于哪个功能
    const gchar *role_func;
    // ipmi命令数据区(data)最大长度
    guint8 max_length;
    // ipmi命令数据区最小长度
    guint8 min_length;
    // ipmi命令处理回调函数
    IPMI_CMD_FUNC ipmi_handler;
    // user data
    gpointer user_data;
} IPMI_CMD_S;

/*
 * Description: 注册APP模块支持的IPMI请求消息
 */
extern gint32 static_implementation_ipmicmd(gsize tbl_size, IPMI_CMD_S *ipmi_cmd_table);

/*
 * Description: 模块静态注册完成
 *              注册完成后从管理进程复制对象到本地、复制class tree到本地、复制路由表到本地
 */
extern gint32 static_complete_module_register(void);

/*
 * Description: BMC控制器重启的处理函数
 */
typedef enum {
    // 运行状态，无复位请求
    REBOOT_NONE,
    // 准备复位
    REBOOT_PREPARE,
    // 执行复位
    REBOOT_PERFORM,
    // 取消复位
    REBOOT_CANCEL,
    // 强制复位
    REBOOT_FORCE,
} REBOOT_CTRL;

typedef gint32 (*REBOOT_FUNC)(REBOOT_CTRL rc);
extern gint32 static_register_reboot_handler(REBOOT_FUNC handler);

/*
 * Description: 静态注册对象属性变更的处理函数表
 */
extern gint32 static_listen_class_event(const gchar *class_name, const gchar *property_name, PropertyEventFunc handler,
    gpointer user_data);


/*
 * Description: 注册对象添加的回调函数
 *              本函数已经通过模板宏封装，无需直接调用此函数
 */
typedef gint32 (*AddOjbFunc)(const gchar *class_name, OBJ_HANDLE object_handle);
extern gint32 dfl_add_object_handler(AddOjbFunc handler);

/*
 * Description: 注册对象删除的回调函数
 *              本函数已经通过模板宏封装，无需直接调用此函数
 */
typedef gint32 (*DelObjFunc)(const gchar *class_name, OBJ_HANDLE object_handle);
extern gint32 dfl_del_object_handler(DelObjFunc handler);

/*
 * Description: 注册对象添加完成的回调函数
 *              本函数已经通过模板宏封装，无需直接调用此函数
 */
typedef gint32 (*AddObjCompleteFunc)(guint32 position);
extern gint32 dfl_addobj_complete_handler(AddObjCompleteFunc handler);

/*
 * Description: 注册对象删除完成的回调函数
 *              本函数已经通过模板宏封装，无需直接调用此函数
 */
typedef gint32 (*DelObjCompleteFunc)(guint32 position);
extern gint32 dfl_delobj_complete_handler(DelObjCompleteFunc handler);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DFLSTATIC_H__ */
