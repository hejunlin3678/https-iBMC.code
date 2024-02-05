/******************************************************************************

                  版权所有 (C), 2008-2009, 华为技术有限公司

 ******************************************************************************
  文 件 名   : maint_debug_error.h
  版 本 号   : 初稿
  部    门   :
  作    者   : licheng
  生成日期   : 2013年8月15日
  最近修改   :
  功能描述   : 维护调测模块错误码
  函数列表   :
  修改历史   :
  1.日    期   : 2013年8月15日
    作    者   : licheng
    修改内容   : 创建文件

******************************************************************************/


#ifndef __MD_ERROR_H__
#define __MD_ERROR_H__

#ifdef __cplusplus
 #if __cplusplus
    extern "C" {
 #endif
#endif /* __cplusplus */

#include <glib.h>
#include "pme/common/mscm_err.h"

#define MD_OK       0
#define MD_EPARA    DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -1)
#define MD_EINTER   DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -2)
#define MD_ESOCK_CLOSE   DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -3)
#define MD_EMSGFMT  DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -4)
#define MD_ECMD_INVALID    DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -5)
#define MD_ECMD_EXE DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -6)
#define MD_EBUSY    DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -7)
#define MD_EPRIVILEGE   DEF_ERROR_NUMBER(ERR_MODULE_ID_MD, -8)

#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif

