/*******************************************************************
              Copyright 2008 - 2013, Huawei Tech. Co., Ltd.
                      ALL RIGHTS RESERVED
                          
Filename      : dflerr.h
Author        : s00120510
Creation time : 2013/7/18
Description   : dflib 错误码定义

Version       : 1.0
********************************************************************/



#ifndef __DFLERR_H__
#define __DFLERR_H__

#include "pme/common/mscm_vos.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

// 正常
#define DFL_OK                           (VOS_OK)
// 未定义错误
#define DFL_ERR                          (VOS_ERR)

// 软件运行错误
#define ERRCODE_RUN_ERR                  (-2000)
// 参数错误
#define ERRCODE_WRONG_PARAMETER          (-2001)
// 类未注册
#define ERRCODE_CLASS_NOT_REGISTERED     (-2002)
// 类没有找到
#define ERRCODE_CLASS_NOT_FOUND          (-2003)
// 指定模块没有找到
#define ERRCODE_MODULE_NOT_FOUND         (-2004)
// 指定的方法没有找到
#define ERRCODE_METHOD_NOT_FOUND         (-2005)
// 从管理进程复制对象失败
#define ERRCODE_COPY_OBJECT_FAILED       (-2006)
// 类、模块、方法等重复注册
#define ERRCODE_DUPLICATE_REGISTERED     (-2007)
// 没有找到路由
#define ERRCODE_ROUTE_NOT_FOUND          (-2008)
// 对象不存在
#define ERRCODE_OBJECT_NOT_EXIST         (-2009)
// 属性不存在
#define ERRCODE_PROPERTY_NOT_EXIST       (-2010)
// 存取器不存在
#define ERRCODE_ACCESSOR_NOT_EXIST       (-2011)
// 数据类型不匹配
#define ERRCODE_TYPE_MISMATCH            (-2012)
// 不支持的数据类型
#define ERRCODE_TYPE_NOTSUPPORT          (-2013)
// 远程对象只读，不可写
#define ERRCODE_READONLY                 (-2014)
// 信号量锁定失败
#define ERRCODE_LOCK_FAILED              (-2015)
// 消息过长
#define ERRCODE_MSG_TOO_BIG              (-2016)
// APP无法复位
#define ERRCODE_REBOOT_FAIL              (-2017)
// 未初始化
#define ERRCODE_NOT_INITED               (-2018)
// 状态不正确
#define ERRCODE_WRONG_STATE              (-2019)
// 超时
#define ERRCODE_TIMEOUT                  (-2020)
// 无资源
#define ERRCODE_NO_RESOURCE              (-2021)
//Connector 已有动态对象
#define ERRCODE_OBJECT_CREATED          (-2022)
//xml文件清理失败
#define ERRCODE_REMOVE_FAILED             (-2023)
//xml文件创建失败
#define ERRCODE_CREATED_FAILED             (-2024)




#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif



