/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : mtd_drivers.h
  版 本 号   : 初稿
  作    者   : hejun 00168999
  生成日期   : 2013年11月23日
  最近修改   :
  功能描述   : mtd_drivers头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月25日
    作    者   : hejun 00168999
    修改内容   : 创建文件

******************************************************************************/
#ifndef __MTD_DRIVERS__
#define __MTD_DRIVERS__

#include "glib.h"

/*****************************************************************************
错误码定义
*****************************************************************************/
#define MTD_WRITE_OK          0

#define ERR_OPEN_SRC_FILE     (-3001)         /* 打开源文件失败 */
#define ERR_OPEN_DEV_FILE     (-3002)         /* 打开设备文件失败 */
#define ERR_READ_SRC_FILE     (-3003)         /* 读取源文件失败 */
#define ERR_WRITE_DEV_FILE    (-3004)         /* 写入设备失败 */
#define ERR_ERASE_DEV_FILE    (-3005)         /* 擦除设备失败 */
#define ERR_PARAMETER_ERR     (-3006)         /* 参数错误 */
#define ERR_GET_SRC_FILE_INFO (-3007)         /* 获取源文件参数错误 */
#define ERR_GET_DEV_FILE_INFO (-3008)         /* 获取设备文件参数错误 */
#define ERR_LSEEK             (-3009)         /* lseek错误 */
#define ERR_REWIND            (-3010)         /* rewind错误 */
#define ERR_READ_DEV_FILE     (-3011)         /* 读取设备文件失败 */
#define ERR_CHECK_FAIL        (-3012)         /* 文件对比失败 */


/*****************************************************************************
数据结构定义
*****************************************************************************/
typedef gint32 (*GET_UPG_PERCENT_FUNC)(guint8 percent);


/*****************************************************************************
对外接口函数定义, 添加对C++的支持
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */

gint32 mtd_write(const gchar* devicename, const gchar * sourcefilename, GET_UPG_PERCENT_FUNC get_percent,
                        guint32 offset, guint32 size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

