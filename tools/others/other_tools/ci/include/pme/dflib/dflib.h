/*******************************************************************
              Copyright 2008 - 2013, Huawei Tech. Co., Ltd.
                      ALL RIGHTS RESERVED
                          
Filename      : dflib.h
Author        : s00120510
Creation time : 2013/7/3
Description   : 分布式框架库头文件

Version       : 1.0
********************************************************************/

#ifndef __DFLIB_H__
#define __DFLIB_H__

#include "pme/dflib/dflerr.h"
#include "dflapi.h"
#include "dflstatic.h"
#include "dflipmi.h"
#include "dfldebug.h"
#include "dfldft.h"
#include "dfhas.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */


// -------------------------- 以下函数仅供框架内部使用 --------------------------------
/*****************************************************************
Parameters    :  module_name
                 method_name
                 method
                 para_list GSList<GVariantType*>
Return        :    
Description   :  注册模块方法
*****************************************************************/
typedef gint32 (*MODULE_METHOD_FUNC)(GSList *input_list, GSList**output_list);
extern gint32 dfl_declaration_module_method( const gchar* module_name, 
    const gchar* method_name, MODULE_METHOD_FUNC method, GSList *input_para_list, GSList *output_para_list);

/*****************************************************************
Parameters    :  module_name
              method_name
              input_list   GSList<GVariant*>
              output_list  GSList<GVariant*>
Return        :    
Description   :  调用其他模块方法
*****************************************************************/
extern gint32 dfl_call_module_method(const gchar *module_name, 
    const gchar *method_name, GSList *input_list, GSList**output_list);

/*****************************************************************
Parameters    :  module_name
              method_name
              para_list  返回 List<GVariantType*>
Return        :    
Description   : 获取模块方法入参参数列表 
*****************************************************************/
extern gint32 dfl_get_module_method_paralist(const gchar* module_name, 
    const gchar* method_name, GSList**para_list);



#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif


