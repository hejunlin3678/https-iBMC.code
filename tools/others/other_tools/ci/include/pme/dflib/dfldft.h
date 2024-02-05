/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : dfldft.h
  版 本 号   : 初稿
  部    门   : 
  作    者   : s00120510
  生成日期   : 2013年9月17日
  最近修改   :
  功能描述   : 框架提供给公共服务模块--DFT模块的api函数--其他模块请不要调用
  函数列表   :
  修改历史   :
  1.日    期   : 2013年9月17日
    作    者   : s00120510
    修改内容   : 创建文件

******************************************************************************/


#ifndef __DFLDFT_H__
#define __DFLDFT_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

// dft 项注册内容
typedef struct tag_dft_item
{
    guint32 dft_id;
    // 板内槽位号 [0:3]表示当前层扣卡的槽位, [4:7]表示上一层单板所在槽位，[8:11]表示上上一层单板所在槽位
    // 最多三层，软件认为不应该超过3层叠加的扣卡
    // slot_id=0 在底板，非底板槽位号从1开始
    guint32 slot_id;
    guint32 dev_id;
    guint32 item_index;
    guint32 rpc_port;
}DFT_TEST_ITEM_S;

typedef gint32 (*DftRegisterFunc)(gsize size, DFT_TEST_ITEM_S *test_item);
/*****************************************************************
Parameters    :  
Return        :    
Description   :  DFT模块注册自己的回调函数: 处理app注册测试项的请求
*****************************************************************/
extern gint32 dfl_dft_register_handler(DftRegisterFunc func);


/*****************************************************************
Parameters    :  rpc_port
              index
              command
Return        :    
Description   :  DFT 模块调用。启动、停止测试、获取结果
*****************************************************************/
extern gint32 dfl_dft_control(guint32 rpc_port, guint32 index, guint32 command, 
    gsize para_size, gconstpointer para_data,   DFT_RESULT_S *result);





#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __DFLDFT_H__ */

