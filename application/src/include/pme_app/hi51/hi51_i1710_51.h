/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 i1710 51 header file
 */
#ifndef __HI1710_51_H__
#define __HI1710_51_H__
#include <hi51_type.h>
#include <hi51_wdg.h>

#ifdef USE_SDCC
#include <mcs51/8051.h>
#else
#include <REG51.h>
#endif

/* Special register define */
SFR_DEFINE(CMD_STATUS_REG, 0x91);
/* [0]: Cmd Valid
   [1]: read/write flag
          1: write,
          0: read
   [2]: access type
          1: access isntruction ram,
          0: access ahb bus
   [7:3]: reserved
*/
SFR_DEFINE(CMD_REG, 0x92);
SFR_DEFINE(ADDR_REG_0, 0x93);
SFR_DEFINE(ADDR_REG_1, 0x94);
SFR_DEFINE(ADDR_REG_2, 0x95);
SFR_DEFINE(ADDR_REG_3, 0x96);

SFR_DEFINE(INT_STATUS_ARM, 0xB9); /* intr status reg from arm */
SFR_DEFINE(INT_STATUS_IP0, 0xBA); /* intr status reg from app */
SFR_DEFINE(INT_STATUS_IP1, 0xBB); /* intr status reg from app */
SFR_DEFINE(INT_STATUS_IP2, 0xBC); /* intr status reg from app */
SFR_DEFINE(INT_STATUS_IP3, 0xBD); /* intr status reg from app */
SFR_DEFINE(MCU_TO_ARM_INT, 0xB1); /* 8051 mcu intr to arm */
SFR_DEFINE(ARM_INTR_MASK, 0xB2);
SFR_DEFINE(INT_MASK_0, 0xB3);
SFR_DEFINE(INT_MASK_1, 0xB4);
SFR_DEFINE(INT_MASK_2, 0xB5);
SFR_DEFINE(INT_MASK_3, 0xB6);

SFR_DEFINE(ARM_INT_CLEAR, 0xC4);  /* clear intr from arm */

SFR_DEFINE(WR_DATA_REG_0, 0x9A);
SFR_DEFINE(WR_DATA_REG_1, 0x9B);
SFR_DEFINE(WR_DATA_REG_2, 0x9C);
SFR_DEFINE(WR_DATA_REG_3, 0x9D);

SFR_DEFINE(RD_DATA_REG_0, 0xA1);
SFR_DEFINE(RD_DATA_REG_1, 0xA2);
SFR_DEFINE(RD_DATA_REG_2, 0xA3);
SFR_DEFINE(RD_DATA_REG_3, 0xA4);

SFR_DEFINE(WDG_CFG, 0xC1);
SFR_DEFINE(WDG_CNT0, 0xC2);
SFR_DEFINE(WDG_CNT1, 0xC3);

#define SET_WDG_ENABLE(enable) do {\
    if (HI51_WDG_ENABLE == (enable)) {\
        WDG_CFG = 1;\
    } else {\
        WDG_CFG = 0;\
    }\
} while (0);

#define WDG_FEED() do {\
    WDG_CNT1 = 0xff;\
    WDG_CNT0 = 0xff;\
} while (0);

#define SET_REG_ADDR(ptr) do {\
    ADDR_REG_3 = *(ptr);\
    (ptr)++;\
    ADDR_REG_2 = *(ptr);\
    (ptr)++;\
    ADDR_REG_1 = *(ptr);\
    (ptr)++;\
    ADDR_REG_0 = *(ptr);\
} while (0);

#define SET_REG_DATA_BE(ptr) do {\
    WR_DATA_REG_0 = *(ptr);\
    (ptr)++;\
    WR_DATA_REG_1 = *(ptr);\
    (ptr)++;\
    WR_DATA_REG_2 = *(ptr);\
    (ptr)++;\
    WR_DATA_REG_3 = *(ptr);\
} while (0);

#define SET_REG_DATA_LE(ptr) do {\
    WR_DATA_REG_3 = *(ptr);\
    (ptr)++;\
    WR_DATA_REG_2 = *(ptr);\
    (ptr)++;\
    WR_DATA_REG_1 = *(ptr);\
    (ptr)++;\
    WR_DATA_REG_0 = *(ptr);\
} while (0);

#define GET_REG_DATA_BE(ptr) do {\
    *(ptr) = RD_DATA_REG_0;\
    (ptr)++;\
    *(ptr) = RD_DATA_REG_1;\
    (ptr)++;\
    *(ptr) = RD_DATA_REG_2;\
    (ptr)++;\
    *(ptr) = RD_DATA_REG_3;\
} while (0);

#define GET_REG_DATA_LE(ptr) do {\
    *(ptr) = RD_DATA_REG_3;\
    (ptr)++;\
    *(ptr) = RD_DATA_REG_2;\
    (ptr)++;\
    *(ptr) = RD_DATA_REG_1;\
    (ptr)++;\
    *(ptr) = RD_DATA_REG_0;\
} while (0);

#define INT_VECTOR_CNT          40

#define INT_VECTOR_A9_INTR_0    0   // A9配置8051寄存器
#define INT_VECTOR_A9_INTR_1    1
#define INT_VECTOR_A9_INTR_2    2
#define INT_VECTOR_A9_INTR_3    3
#define INT_VECTOR_A9_INTR_4    4
#define INT_VECTOR_A9_INTR_5    5
#define INT_VECTOR_A9_INTR_6    6
#define INT_VECTOR_A9_INTR_7    7
#define INT_VECTOR_I2C0_INTR    8   // I2C0中断
#define INT_VECTOR_I2C1_INTR    9   // I2C1中断
#define INT_VECTOR_I2C2_INTR    10  // I2C2中断
#define INT_VECTOR_I2C3_INTR    11  // I2C3中断
#define INT_VECTOR_I2C4_INTR    12  // I2C4中断
#define INT_VECTOR_I2C5_INTR    13  // I2C5中断
#define INT_VECTOR_I2C6_INTR    14  // I2C6中断
#define INT_VECTOR_I2C7_INTR    15  // I2C7中断
#define INT_VECTOR_I2C8_INTR    16  // I2C8中断
#define INT_VECTOR_I2C9_INTR    17  // I2C9中断
#define INT_VECTOR_I2C10_INTR   18  // I2C10中断
#define INT_VECTOR_I2C11_INTR   19  // I2C11中断
#define INT_VECTOR_PECI_INT     20  // PECI中断
#define INT_VECTOR_MAILBOX_INT  21  // Mailbox中断
#define INT_VECTOR_UART2_INTR   22  // UART2中断
#define INT_VECTOR_UART3_INTR   23  // UART3中断
#define INT_VECTOR_UART4_INTR   24  // UART4中断
#define INT_VECTOR_SSP_INTR     25  // SSP中断
#define INT_VECTOR_CAN0_IRQ     26  // CANBUS0中断
#define INT_VECTOR_CAN1_IRQ     27  // CANBUS1中断
#define INT_VECTOR_IPMB0_INT    28  // IPMB0中断
#define INT_VECTOR_IPMB1_INT    29  // IPMB1中断
#define INT_VECTOR_IPMB2_INT    30  // IPMB2中断
#define INT_VECTOR_IPMB3_INT    31  // IPMB3中断
#define INT_VECTOR_FAN_INT      32  // FAN中断
#define INT_VECTOR_ADC_INT      33  // ADC中断
#define INT_VECTOR_REV_0        34  // 保留
#define INT_VECTOR_REV_1        35  // 保留
#define INT_VECTOR_REV_2        36  // 保留
#define INT_VECTOR_REV_3        37  // 保留
#define INT_VECTOR_REV_4        38  // 保留
#define INT_VECTOR_REV_5        39  // 保留

/* 获取中断状态宏函数 */
#define GET_INT_STATUS(int_status) do {\
    (int_status)[0] = INT_STATUS_ARM;\
    (int_status)[1] = INT_STATUS_IP0;\
    (int_status)[2] = INT_STATUS_IP1;\
    (int_status)[3] = INT_STATUS_IP2;\
    (int_status)[4] = INT_STATUS_IP3;\
} while (0);

/* 清中断状态宏函数 */
#define CLR_INT_STATUS(int_status) do {\
    RI = 0;\
    TI = 0;\
    ARM_INT_CLEAR = (int_status)[0];\
    ARM_INT_CLEAR = 0;\
} while (0);

#define GET_INT_MASK(int_mask) do {\
    (int_mask)[0] = ARM_INTR_MASK;\
    (int_mask)[1] = INT_MASK_0;\
    (int_mask)[2] = INT_MASK_1;\
    (int_mask)[3] = INT_MASK_2;\
    (int_mask)[4] = INT_MASK_3;\
} while (0);

#define SET_INT_MASK(int_mask) do {\
    ARM_INTR_MASK = (int_mask)[0];\
    INT_MASK_0 = (int_mask)[1];\
    INT_MASK_1 = (int_mask)[2];\
    INT_MASK_2 = (int_mask)[3];\
    INT_MASK_3 = (int_mask)[4];\
} while (0);

#endif