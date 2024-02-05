/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: LED灯管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __LED_DEFINE_H__
#define __LED_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define STOP_INTELLIGENCE_TEST_NUM 0xff /* 停止装备数码管智能测试值 */

#define LED_UID_NUM_ID 4
#define TURN_ON_IDENTIFY_LED 0xFF     /* 点亮UID指示灯 */
#define TURN_OFF_IDENTIFY_LED 0x00    /* 关闭UID指示灯 */
#define RELEASE_OVER_RIDE_STATE 0xFC  /* 释放超驰状态，回归本地模式 */
#define TURN_BLINK_IDENTIFY_LED 0x01  /* 0x01 - 0xFA均为闪烁 */
#define TURN_ON_IDENTIFY_LED_TMP 0x02 /* 临时点亮UID指示灯 */
#define TURN_ON_DEFAULT_TIME 15
#define DEFAULT_DURATION_TIME 0x00    /* 默认持续时间 */
#define DEFAULT_LED_COLOR 0xF         /* 默认LED的颜色 */
#define HEALTH_LED_ID       2

#define LED_COLOR_NUM 7
#define LED_STATE_OFF 0
#define LED_STATE_ON 0xff


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __LED_DEFINE_H__ */
