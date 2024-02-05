/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : sml_base.h
  版 本 号   : 初稿
  作    者   : huanghan (h00282621)
  生成日期   : 2016年2月25日
  最近修改   :
  功能描述   : 华为自研的存储设备管理库(Storage Management Library)的基本信息
  函数列表   :
  修改历史   :
  1.日    期   : 2016年2月25日
    作    者   : huanghan (h00282621)
    修改内容   : 创建文件

******************************************************************************/
#ifndef __SML_BASE_H__
#define __SML_BASE_H__

#include "sml_controller.h"
#include "sml_logic_drive.h"
#include "sml_physical_drive.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef offsetof
#define offsetof(type,m) ((size_t)&(((type *)0)->m))
#endif

#define PD_ATTACHED_CONTROLLER_PCH     "PCH"
#define STORAGE_INFO_INVALID_STRING    "N/A"
#define STORAGE_INFO_INVALID_BYTE      0xFF
#define STORAGE_INFO_INVALID_WORD      0xFFFF
#define STORAGE_INFO_INVALID_DWORD     0xFFFFFFFF
#define STORAGE_INFO_INVALID_QWORD     0xFFFFFFFFFFFFFFFF

typedef enum tag_oob_interface_type {
    OOB_TYPE_OVER_I2C = 0,
    OOB_TYPE_OVER_PCIE,
    OOB_TYPE_BUTT,
} OOB_INTERFACE_TYPE_E;

typedef gint32 (*I2C_WRITE_FUNC)(guint8 obj_index, guint8* pWritebuf, guint8 write_length);
typedef gint32 (*I2C_WRITEREAD_FUNC)(guint8 obj_index, guint8* pWritebuf, guint8 write_length, guint8* pReadbuf, guint8 read_length);
typedef gint32 (*MCTP_WRITEREAD_FUNC)(guint8 obj_index, guint32 request_length, const guint8 *request,
    guint32 *response_length, guint8 *response, guint32 timeout);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif

