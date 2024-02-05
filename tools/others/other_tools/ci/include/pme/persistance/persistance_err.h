/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : persistance.h
  版 本 号   : 初稿
  部    门   : 
  作    者   : zhouyubo@huawei.com
  生成日期   : 2013年7月24日
  最近修改   :
  功能描述   : 持久化对外头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月24日
    作    者   : zhouyubo@huawei.com
    修改内容   : 创建文件

******************************************************************************/
#ifndef PERSISTANCE_ERR_H
#define PERSISTANCE_ERR_H

#include <glib.h>
#include <pme/common/mscm_vos.h>

#ifdef __cplusplus  
#if __cplusplus
extern "C" {  
#endif
#endif /* __cplusplus */ 

typedef enum tag_per_error
{
    /* 正常功能性返回 */
    PER_OK = VOS_OK,                              
    PER_NEW_ITEM,                            /* 有新持久化项目 */
    
    /* 错误码返回 */
    PER_ERROR_PARA_0_INVALID = DEF_ERROR_NUMBER(ERR_MODULE_ID_PER, 0),        /* 第1参数无效 */
    PER_ERROR_PARA_1_INVALID,                /* 第2参数无效 */
    PER_ERROR_PARA_2_INVALID,                /* 第3参数无效 */
    PER_ERROR_PARA_3_INVALID,                /* 第4参数无效 */
    PER_ERROR_PARA_4_INVALID,                /* 第5参数无效 */
    PER_ERROR_PARA_5_INVALID,                /* 第6参数无效 */
    PER_ERROR_PARA_6_INVALID,                /* 第7参数无效 */
    PER_ERROR_PARA_OTHER_INVALID,            /* 其他输入参数（比如从全局变量输入的参数）无效 */
    PER_ERROR_MALLOC_FAIL,                   /* 资源分配失败 */
    PER_ERROR_FILE_INIT_FAIL,                /* 文件管理初始化失败 */
    PER_ERROR_PARSER_INIT_FAIL,              /* 文件解析器初始化失败 */
    PER_ERROR_CFG_ITEM_INIT_FAIL,            /* 单个配置项初始化失败 */
    PER_ERROR_CFG_ITEM_NOT_INITED,           /* 单个配置项尚未初始化 */
    PER_ERROR_CFG_INIT_FAIL,                 /* 单个配置项尚未初始化 */
    PER_ERROR_FILE_OPEN_FAIL,                /* 文件打开失败 */
    PER_ERROR_FILE_LOCK_FAIL,                /* 文件加锁失败 */
    PER_ERROR_FILE_WRITE_FAIL,               /* 文件加锁失败 */
    PER_ERROR_DATA_INVALID,                  /* 文件CRC校验数据丢失 */
    PER_ERROR_CFG_INPORT_FAIL,               /* 配置导入失败 */
    PER_ERROR_CLEAR_CFG_FLAG,                /* 清除配置标志失败 */
}PER_ERROR_E;


#ifdef __cplusplus  
#if __cplusplus
}  
#endif
#endif /* __cplusplus */  

#endif

