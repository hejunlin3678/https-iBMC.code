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
#ifndef __PFLASH_DRV__
#define __PFLASH_DRV__

#include "glib.h"

/* 保留flash文件目录 */
#ifdef ARM_PLATFORM
#define RESERVE_FLASH_PATH  "/opt/pme/pflash/"
#else
#define RESERVE_FLASH_PATH  "/tmp/pflash/"
#endif

/*****************************************************************************
错误码定义
*****************************************************************************/
#define PFLASH_OK       0
#define PFLASH_ELOCK    (-3001)         /* 互斥锁异常 */
#define PFLASH_EOPEN    (-3002)         /* 打开设备文件失败 */
#define PFLASH_EWRITE   (-3003)         /* 写入设备失败 */
#define PFLASH_EERASE   (-3004)         /* 擦除设备失败 */
#define PFLASH_EREAD    (-3005)         /* 读取设备文件失败 */
#define PFLASH_ELSEEK   (-3006)         /* lseek错误 */
#define PFLASH_EEXIST   (-3007)         /* 文件不存在 */
#define PFLASH_ECHECK   (-3008)         /* 文件对比失败 */
#define PFLASH_EPARA    (-3009)         /* 参数错误 */
#define PFLASH_EFULL    (-3010)         /* FLASH满 */
#define PFLASH_EFILE    (-3011)         /* 文件异常*/

#ifdef __cplusplus
#if __cplusplus
    extern "C" {
#endif
#endif /* __cplusplus */


extern gint pflash_recover_file(const gchar *file_name);
extern gint pflash_init(void);
extern gpointer pflash_open(const char *pathname, int flags);
extern gssize pflash_write(gpointer file_hnd, const guint8 *buf, gsize count);
extern gssize pflash_read(gpointer file_hnd, guint8 *buf, gsize count);
extern void pflash_close(gpointer file_hnd);
extern gint pflash_fstat(gpointer file_hnd, gsize *size);
extern gint pflash_get_contents(gpointer file_hnd, guint8 **contents, gsize *length);
gint get_pflash_file(gchar *filename, guint8 **contents, gsize *length);




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


