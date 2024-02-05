/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 monitor header file
 */
#ifndef _HI51_PS_MONITOR_H_
#define _HI51_PS_MONITOR_H_
#include <hi51_system.h>

#ifdef USE_SDCC
void psm_heart_beat_isr(void) __using INTRRUPT_USING;
#else
void psm_heart_beat_isr(void);
#endif
void psm_init(void);
void psm_task_power_monitor(guint8 task_id);
void psm_task_v_i_monitor(guint8 task_id);
void psm_task_rw_reg(guint8 task_id);

#endif