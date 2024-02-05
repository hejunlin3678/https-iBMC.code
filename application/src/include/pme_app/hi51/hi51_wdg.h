/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 wdg header file
 */
#ifndef _HI51_WDG_H_
#define _HI51_WDG_H_

#ifdef _IN_51_
#include <hi51_type.h>
#else
#include <glib.h>
#endif

#define HI51_WDG_ENABLE  1
#define HI51_WDG_DISABLE 0

void hi51_wdg_feed(void);

#endif