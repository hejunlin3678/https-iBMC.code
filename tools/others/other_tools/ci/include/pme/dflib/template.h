/*******************************************************************
              Copyright 2008 - 2013, Huawei Tech. Co., Ltd.
                      ALL RIGHTS RESERVED

Filename      : template.h
Author        : s00120510
Creation time : 2013/7/3
Description   :

Version       : 1.0
********************************************************************/
#ifndef __TEMPLATE_H__
#define __TEMPLATE_H__


#include "dflib.h"
#include "pme/debug/maint_debug_so.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

#define BEGIN_MODULE_DECLARATION(module) \
    static const gchar *_app_module_name = module; \
    static IPMI_CMD_S *_ipmi_cmd_table = NULL; \
    static guint32    _ipmi_cmd_table_size = 0; \
    static DEBUG_CMD_S *_debug_cmd_table = NULL; \
    static guint32    _debug_cmd_table_size = 0; \
    static gint32 _module_start_delay = 0; \
    static gint32 _app_init(void) \
    { \
        gint32 result; \
        result=static_register_module(module); \
        if(DFL_OK != result) \
        {debug_log(DLOG_ERROR, "register module %s failed.errcode:%d\r\n", module, result); return result;} \
        result=init_md_so(); \
        if(DFL_OK != result) \
        {debug_log(DLOG_ERROR, "init_md_so failed.errcode:%d\r\n", result); return result;}

#define DELAY_BEFORE_START(delay) _module_start_delay = delay;

#define DECLARATION_CLASS(classname) \
    result = static_declaration_class(classname); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR, "declaration class %s failed.errcode:%d\r\n", classname, result); return result;}

#define DECLARATION_CLASS_METHOD(c,m,fun) \
    result = static_declaration_class_method(c,m,fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR, "declaration method %s.%s failed.errcode:%d\r\n",c,m, result); return result;}

#define DECLARATION_REBOOT_PROCESS(fun) \
    result = static_register_reboot_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register reboot handler failed.errcode:%d\r\n", result); return result;}

#define ON_ADD_OBJECT(fun)  \
    result = dfl_add_object_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register add object handler failed.errcode:%d\r\n", result); return result;}

#define ON_ADD_OBJECT_COMPLETE(fun)  \
    result = dfl_addobj_complete_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register add object complete handler failed.errcode:%d\r\n", result); return result;}    

#define ON_DEL_OBJECT(fun)  \
    result = dfl_del_object_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register del object handler failed.errcode:%d\r\n", result); return result;}

#define ON_DEL_OBJECT_COMPLETE(fun)  \
    result = dfl_delobj_complete_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register del object complete handler failed.errcode:%d\r\n", result); return result;}    

#define ON_DUMP(fun) \
    result = static_register_dump_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register dump handler failed.errcode:%d\r\n", result); return result;}


#define ON_CLASS_PROPERTY_EVENT(c, p, fun, d) \
    result = static_listen_class_event(c, p, fun, d); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register property(%s.%s) event handler failed.errcode:%d\r\n",c,p, result); return result;}


#define ON_IPMI_REQUEST(fun) \
    result = dfl_register_ipmi_agent(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register ipmi send request handler failed.errcode:%d\r\n",result); return result;}

#define ON_IPMI_RESPONSE(fun) \
    result = dfl_ipmi_response_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register ipmi send response handler failed.errcode:%d\r\n",result); return result;}

#define ON_IPMI_PROXY(fun) \
    result = dfl_ipmi_proxy_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register ipmi proxy handler failed.errcode:%d\r\n",result); return result;}

#define ON_DFT_REGISTER(fun) \
    result = dfl_dft_register_handler(fun); \
    if(DFL_OK != result) \
    {debug_log(DLOG_ERROR,"register dft register handler failed.errcode:%d\r\n",result); return result;}



#define END_MODULE_DECLARATION()   return DFL_OK;}

#define BEGIN_IPMI_COMMAND() IPMI_CMD_S _ipmi_cmd[]={
#define ADD_IPMI_COMMAND(n,c,f,p,max,min,fun,d)  {n,c,f,p,"",max,min,fun,d},
#define ADD_IPMI_COMMAND_V2(n,c,f,p,r,max,min,fun,d)  {n,c,f,p,r,max,min,fun,d},
#define END_IPMI_COMMAND() }; \
    __attribute__((constructor)) static void _init_ipmi_cmd_table(void) \
    { \
    _ipmi_cmd_table = _ipmi_cmd; \
    _ipmi_cmd_table_size = sizeof(_ipmi_cmd)/sizeof(_ipmi_cmd[0]); \
    } //lint -e528    该函数会被自动调用,故屏蔽此告警

#define BEGIN_DEUBG_COMMAND() DEBUG_CMD_S _debug_cmd[]={
#define ADD_DEBUG_COMMAND(n,fun,plist,d)  {n,fun,plist,d},
#define END_DEBUG_COMMAND() }; \
    __attribute__((constructor)) static void _init_debug_cmd_table(void) \
    { \
    _debug_cmd_table = _debug_cmd; \
    _debug_cmd_table_size = sizeof(_debug_cmd)/sizeof(_debug_cmd[0]); \
    }//lint -e528    该函数会被自动调用,故屏蔽此告警

#if (defined(UNIT_TEST) && defined(MOD_MAIN))
#define MAIN_FUNC_NAME	MOD_MAIN
extern int MOD_MAIN(int argc, char *argv[]);
#else
#define MAIN_FUNC_NAME	main
#endif

#define BEGIN_MODULE_MAIN() \
    int MAIN_FUNC_NAME(int argc, char *argv[]) \
    { \
        gint32 result; \
        result = _app_init(); \
        if (DFL_OK != result) {return result;} \
        if (_ipmi_cmd_table_size) \
        { \
            result = static_implementation_ipmicmd(_ipmi_cmd_table_size, _ipmi_cmd_table); \
            if(DFL_OK != result) \
            {debug_log(DLOG_ERROR, "register ipmi command failed.errcode:%d\r\n",result); return result;} \
        } \
        if (_debug_cmd_table_size) \
        { \
            result = static_register_debug_cmd(_debug_cmd_table_size, _debug_cmd_table); \
            if(DFL_OK != result) \
            {debug_log(DLOG_ERROR, "register debug cmd failed.errcode:%d\r\n",result); return result;} \
        } \
        result = static_complete_module_register(); \
        if(DFL_OK != result) \
        {debug_log(DLOG_ERROR,"get object/class/route info failed.errcode:%d\r\n",result); return result;}
        
#define MODULE_ORDER_INIT(order, init) \
        result = static_module_init(order, init); \
        if(DFL_OK != result) \
        { \
            debug_log(DLOG_ERROR, "module %s init failed.errcode:%d\r\n",_app_module_name,result); \
            return result; \
        }

#define MODULE_INIT(init) MODULE_ORDER_INIT(MAX_ORDER_INIT_NUM, init)

#define MODULE_CROSS_INIT(init) \
        result = static_module_cross_init(init); \
        if(DFL_OK != result) \
        { \
            debug_log(DLOG_ERROR, "module %s cross init failed.errcode:%d\r\n",_app_module_name,result); \
            return result; \
        }

#define MODULE_START(start) \
        result = static_module_start(start, _module_start_delay); \
        if(DFL_OK != result) \
        { \
            debug_log(DLOG_ERROR, "module %s start failed.errcode:%d\r\n",_app_module_name,result); \
            return result; \
        }

#if (defined(UNIT_TEST) && defined(MOD_MAIN))
#define END_MODULE_MAIN() \
        return VOS_OK; \
    }
#else
#define END_MODULE_MAIN() \
        static_start_rpc_service(); \
        return 0; \
    }    
#endif

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif

