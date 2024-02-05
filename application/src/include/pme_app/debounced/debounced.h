/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : debounce.h
  版 本 号   : 初稿
  部    门   :
  作    者   : licheng
  生成日期   : 2014年4月4日
  最近修改   :
  功能描述   : 去抖动头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2014年4月4日
    作    者   : licheng
    修改内容   : 创建文件

******************************************************************************/

#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

#include "pme_app/pme_app.h"

#ifdef __cplusplus
 #if __cplusplus
    extern "C" {
 #endif
#endif /* __cplusplus */

#define DBD_OK  0
#define DBD_EPARA    DEF_ERROR_NUMBER(ERR_MODULE_ID_SCAN, -1)
#define DBD_EINTER   DEF_ERROR_NUMBER(ERR_MODULE_ID_SCAN, -2)


/* 去抖实例基类 */
typedef struct dbd_base_instance_s
{
    gchar class_nm[MAX_NAME_SIZE];
} DBD_BASE_INSTANCE;

extern DBD_BASE_INSTANCE* dbd_create_instance(OBJ_HANDLE dbd_cfg_obj_hnd, guint8 default_val);
extern void dbd_release_instance(DBD_BASE_INSTANCE *dbd_base_inst);
extern gint32 dbd_get_debounced_val(DBD_BASE_INSTANCE* dbd_base_inst, guint8 new_val, guint8 *dbd_val);
extern void dbd_clear_debounced_val(DBD_BASE_INSTANCE* dbd_base_inst, guint8 default_value);

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif

