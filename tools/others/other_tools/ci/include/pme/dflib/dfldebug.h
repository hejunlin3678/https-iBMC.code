/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : dfldebug.h
  版 本 号   : 初稿
  部    门   : 
  作    者   : s00120510
  生成日期   : 2013年8月14日
  最近修改   :
  功能描述   : 框架提供给公共服务模块--debug模块的api函数--其他模块请不要调用
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月14日
    作    者   : s00120510
    修改内容   : 创建文件

******************************************************************************/


#ifndef __DFLDEBUG_H__
#define __DFLDEBUG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


// -------------------------- 以下函数仅供debug模块调用，其他模块!不能!调用 --------------------------------
/*****************************************************************
Parameters    :  module_name
              cmdlist 返回 List<DEBUG_CMDLIST_S*>
Return        :    
Description   :  返回指定模块的debug命令列表
*****************************************************************/
extern gint32 dfl_get_debug_cmdlist(const gchar *module_name, GSList**cmdlist);

#define MAX_HELP_INFO_SIZE 128
typedef struct tag_debug_cmdlist
{
    guint32 index;
    // debug cmd
    gchar name[MAX_NAME_SIZE];
    // debug help
    gchar help[MAX_HELP_INFO_SIZE];
    // parameter type(GVariantType) list
    //GSList type_list;
}DEBUG_CMDLIST_S;

/*****************************************************************
Parameters    :  cmd_node
Return        :    
Description   :  释放dfl_get_debug_cmdlist函数返回的List<DEBUG_CMDLIST_S*>节点
*****************************************************************/
extern void dfl_free_cmd_node(DEBUG_CMDLIST_S *cmd_node);

/*****************************************************************
Parameters    :  module_list  返回 List<gchar*>
Return        :    
Description   :  获得模块名称列表
*****************************************************************/

extern gint32 dfl_get_module_list(GSList**module_list);

/*****************************************************************
Parameters    :  handler
Return        :    
Description   :  框架跟踪模块间消息、ipmi命令收发，并通过trace handler发送给debug模块
*****************************************************************/
typedef gint32 (*TRACE_FUNC)(const char* fmt, ...);
extern gint32 dfl_trace_handler(TRACE_FUNC handler);

/*****************************************************************
Parameters    :  module_name
              cmd_name
              para_list 返回 List<GVariantType*>
Return        :    
Description   : 返回指定模块的某个命令的参数列表 
*****************************************************************/
extern gint32 dfl_get_debug_cmd_paralist(const gchar* module_name, const gchar* cmd_name, GSList**para_list);

/*****************************************************************
Parameters    :  module_name
              cmd_name
              input_list
Return        :    
Description   : 向app发送调试命令 
*****************************************************************/
extern gint32 dfl_call_debug_cmd(const gchar *target_module, const gchar* cmd_name, GSList *input_list);

/*****************************************************************
Parameters    :  module_name
              path
Return        :    
Description   : 导出模块的维护信息数据 
*****************************************************************/
extern gint32 dfl_module_dump(const gchar* module_name, const gchar* path);

/*****************************************************************
Parameters    :  obj_handle
                 property_name
                 property_value
                 options
Return        :    
Description   : 调试接口: 设置对象的属性并广播属性变更事件 
*****************************************************************/

extern gint32 dfl_override_property_value( OBJ_HANDLE obj_handle, 
    const gchar* property_name, GVariant* property_value , DF_OPTIONS options); 


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DFLDEBUG_H__ */

