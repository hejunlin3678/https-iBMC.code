/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2008-2022. All rights reserved.
 * Description : ft CPU 获取温度头文件
 * Author : zwx1079372
 * Create : 2021年9月10日
 * Notes : NA
 * History : 2021年9月10日 创建文件
 *
 */
 
#ifndef __FTCPUTEMP_H__
#define __FTCPUTEMP_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern gint32 get_ftcpu_temp(guint8 bus_id, guint8 speed, guint32 address, guint16* temp);
extern gint32 get_postcode(guint8 bus_id, guint8 speed, guint32 address, guint16* code);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
