/*
* Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
* Description: LCD显示相关的定义。
* Author: h00282621
* Create: 2020-3-10
* Notes: 用于跨模块使用的相关的定义。
*/


#ifndef __LCD_DEFINE_H__
#define __LCD_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define  LCD_VERSION_ADDR 0x0000            /* LCD版本信息地址 */

#define  REAL_VERSION_LEN  4                /* 实际版本信息长度 */

/* RGB颜色定义 */
#define  BLACK 0x0000                       /* 黑色：      0,   0,   0 */
#define  NAVY 0x000F                        /* 深蓝色：    0,   0, 128 */
#define  DGREEN 0x03E0                      /* 深绿色：    0, 128,   0 */
#define  DCYAN 0x03EF                       /* 深青色：    0, 128, 128 */
#define  MAROON 0x7800                      /* 深红色：  128,   0,   0 */
#define  PURPLE 0x780F                      /* 紫色：    128,   0, 128 */
#define  OLIVE 0x7BE0                       /* 橄榄绿：  128, 128,   0 */
#define  LGRAY 0xC618                       /* 灰白色：  192, 192, 192 */
#define  DGRAY 0x7BEF                       /* 深灰色：  128, 128, 128 */
#define  BLUE 0x001F                        /* 蓝色：      0,   0, 255 */
#define  GREEN 0x07E0                       /* 绿色：      0, 255,   0 */
#define  CYAN 0x07FF                        /* 青色：      0, 255, 255 */
#define  RED 0xF800                         /* 红色：    255,   0,   0 */
#define  MAGENTA 0xF81F                     /* 品红：    255,   0, 255 */
#define  YELLOW 0xFFE0                      /* 黄色：    255, 255,   0 */
#define  ORANGE 0xFD20                      /* 橙色：    255, 165,   0 */
#define  WHITE 0xFFFF                       /* 白色：    255, 255, 255 */

#define  ARIAL_ASC_12x12 1                  /* 12x12  ARIAL ASCII字符 */
#define  ARIAL_ASC_14x14 2                  /* 14x14  ARIAL ASCII字符 */
#define  ARIAL_ASC_16x16 3                  /* 16x16  ARIAL ASCII字符 */
#define  ARIAL_ASC_18x18 4                  /* 18x18  ARIAL ASCII字符 */

#define  ARIAL_BOLD_ASC_12x12 5             /* 12x12 BOLD ARIAL ASCII字符 */
#define  ARIAL_BOLD_ASC_14x14 6             /* 14x14 BOLD ARIAL ASCII字符 */
#define  ARIAL_BOLD_ASC_16x16 7             /* 16x16 BOLD ARIAL ASCII字符 */
#define  ARIAL_BOLD_ASC_18x18 8             /* 18x18 BOLD ARIAL ASCII字符 */

#define  ASC_6x8 8                          /* 6x8 ASCII字符     */
#define  GBK_12x12 12                       /* 12x12 GBK扩展码   */
#define  GBK_16x16 16                       /* 16x16 GBK扩展码   */
#define  GBK2132_24x24 24                   /* 24x24 GBK2132内码 */
#define  GBK2132_32x32 32                   /* 32x32 GBK2132内码 */

#define  ICON_ID_MAX 409                    /* 内存图标序号最大值 */
#define  CURSOR_WIDE_MAX 32                 /* 光标宽度最大值 */
#define  CURSOR_HEIGHT_MAX 32               /* 光标高度最大值 */
#define  PICTURE_ID_MAX 65534               /* 内存图片序号最大值  */
#define  LIGHT_TIME_MAX 256                 /* 最大背光时间，单位：秒 */
#define  LIGHT_PWMLEVEL_MAX 100             /* PWM可调节的最大背光亮度 */



#define LCD_BUSY 1                         /* 智能显示终端正忙 */
#define LCD_FREE 0                         /* 智能显示终端空闲 */

#define KEYVALID 1                         /* 按键有效标志 */
#define KEYINVALID 0                       /* 按键无效标志 */

#define TOUCH_INVALID 0                    /* 触摸无效 */
#define TOUCH_DOWN 1                       /* 触摸按下标志 */
#define TOUCH_UP 2                         /* 触摸抬起标志 */
#define TOUCH_MOVE 3                       /* 触摸移动标志 */

#define LCD_XADDR_MAX 320                  /* 液晶X轴坐标最大值 */
#define LCD_YADDR_MAX 240                  /* 液晶Y轴坐标最大值 */

#define LCD_TEXTLABEL_ID_MAX 7280          /* 文本标签ID最大值 */
#define LCD_XY_MSG_LEN 10                  /* 坐标消息长度 */
#define LCD_DEVICE_INFO_MSG_LEN 6          /* 上报的器件信息消息长度 */
#define LCD_DEVICE_CHECK_MSG_LEN 11        /* 上报的器件自检消息长度 */
#define LCD_DOWN_LOAD_MSG_LEN 11           /* 启动下载资源文件消息长度 */
#define LCD_SHAKE_HAND_MSG_LEN 6           /* 上电握手消息长度  */

#define LCD_RX_MSG_LEN_MAX 256             /* 最大的接收消息长度 */
#define LCD_RX_MSG_NUM_MAX 32              /* 最大的接收消息个数 */
#define LCD_TX_MSG_LEN_MAX 1024            /* 最大的发送消息长度 */


#define  USER_FONT_LIB 1                   /* 按用户字库显示字符 */


#define LCD_CMD_HEAD 0xAA                  /*命令头*/
#define LCD_SHAKE_RSP 0x55                 /*握手响应*/
#define LCD_CMD_TOUCH_UP 0x72              /*触摸笔抬起时，终端最后一次采集的坐标数据上传指令*/
#define LCD_CMD_TOUCH_DOWN 0x73            /*触摸笔按下时，终端采集的坐标数据命令*/
#define LCD_CMD_DEVICE_INFO 0xA0           /*器件信息读取*/
#define LCD_CMD_DOWN_LOAD 0xF2             /*下载资源文件*/
#define LCD_CMD_FLASH_READ 0x91            /*下载资源文件*/
#define LCD_CMD_SHAKE_HAND 0x00            /*上电握手信息*/
#define LCD_CMD_PARA_DEVICE_CHECK 0x00     /*器件信息读取,器件自检参数*/
#define LCD_CMD_PARA_POS_DEVICE_CHECK 0x3  /*器件信息读取,器件自检参数位置*/



#define MAX_UART_FIFO  128

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __LCD_DEFINE_H__ */
