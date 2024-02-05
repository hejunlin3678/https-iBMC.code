/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : dflipmi.h
  版 本 号   : 初稿
  部    门   : 
  作    者   : s00120510
  生成日期   : 2013年8月14日
  最近修改   :
  功能描述   : 框架提供给公共服务模块--IPMI模块的api函数--其他模块请不要调用
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月14日
    作    者   : s00120510
    修改内容   : 创建文件

******************************************************************************/


#ifndef __DFLIPMI_H__
#define __DFLIPMI_H__

#include "pme/dfrpc/df_rpc.h"


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


// -------------------------- 以下函数仅供IPMI模块调用，其他模块!不能!调用 --------------------------------
/*****************************************************************
Parameters    :  target_que
              handler_index
              size
              req_data
Return        :    
Description   : 分发IPMI命令，ipmi模块接收到外部命令，分发给app 
*****************************************************************/
extern gint32 dfl_distribute_ipmicmd(guint32 target_que, guint32 handler_index, gsize req_size, void *req_data);

/*****************************************************************
Parameters    :  size
              cmd_table
Return        :    
Description   : 从管理进程获得所有模块注册的IPMI命令表 
*****************************************************************/
extern gint32 dfl_get_ipmicmd_table(gsize* size, IPMI_REG_DATA_S **cmd_table);

/*****************************************************************
Parameters    :  ipmi_request_handler
Return        :    
Description   :  注册IPMI模块的回调函数，该函数接收APP的IPMI消息并对外发送，并返回响应
*****************************************************************/
extern gint32 dfl_register_ipmi_agent(send_ipmi_request_ptr ipmi_request_handler);

/*****************************************************************
Parameters    :  handler
Return        :    
Description   :  注册响应发送处理函数，app向外发送响应，由ipmi模块负责发送
*****************************************************************/
extern gint32 dfl_ipmi_response_handler(send_ipmi_response_ptr handler);

/*****************************************************************
Parameters    :  handler
Return        :    
Description   :  用于ipmi模块注册转发ipmi命令接口
*****************************************************************/
typedef gint32 (*IpmiProxyFunc)(gsize req_size, gconstpointer request);
extern gint32 dfl_ipmi_proxy_handler(IpmiProxyFunc handler);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DFLIPMI_H__ */

