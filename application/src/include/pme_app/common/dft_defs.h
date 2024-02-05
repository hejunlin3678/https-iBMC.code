/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 装备测试相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __DFT_DEFINE_H__
#define __DFT_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define IN_DFT_MODE 1 // 是否处于装备模式标识 0：非装备模式 1：装备模式
#define MAX_DIEID_LEN 256
#define  MAX_DIEID_NUM              8
#define  MAX_DIEID_LOCATION         16
#define  MAX_DIEID_NAME             16
#define  MAX_DIEID_VER              32


/* ***************DFT器件可测试性类型定义************** */
#define DFT_TYPE_SELFTEST 1           /* 可以直接自检，由APP提供检验结果 */
#define DFT_TYPE_REQUIRE_ANTECEDENT 2 /* 需要人工干预的前提条件，比如需要接环回头、需要设置屏幕等，由各APP提供测试结果 */
#define DFT_TYPE_PRESS_TEST 3         /* 拷机测试项 */
#define DFT_TYPE_MANUAL_INTERVENTION 4 /* 需要人工检查结果，比如LED灯需要人工查看是否正常，需要由装备人员确定检测结果 */
#define DFT_TYPE_REQUIRE_OPERATION 5   /* 需要人工操作，如button 、LCD */
#define DFT_TYPE_REQUIRE_INTERACT 6    /* 需要与装备进行交互，需要装备侧软件比对数据来确定测试结果 */
#define DFT_TYPE_INVALID 255           /* 无效的测试项，不进行注册 */

/* ***********************DFT ID 定义****************** */
/* 1到32：自检测试器件项，可测试类型为1 */
#define DFT_LM75 1
#define DFT_LM80 2
#define DFT_PCA9555 3
#define DFT_PCA9545 4
#define DFT_PCA9517 5
#define DFT_INA220 6
#define DFT_BUTTON_CELL 7
#define DFT_PS 8
#define DFT_ME 9
#define DFT_SD_SAMPLE_TEST 10
#define DFT_FLASH 11
#define DFT_EEPROM 12
#define DFT_DDR3 13
#define DFT_SRAM 14
#define DFT_CPLD 15
#define DFT_CPU_VOLT 16
#define DFT_MEM_VOLT 17
#define DFT_12V0_VOLT 18
#define DFT_5V0_VOLT 19
#define DFT_3V3_VOLT 20
#define DFT_SD_PRESENCE 21
#define DFT_CARD_PRESENCE 22
#define DFT_LM96080 23
#define DFT_MASS_STRG_PST 24
#define DFT_MASS_STRG_RW 25
#define DFT_HI1710_TEST 26
#define DFT_CLK_TEST 27 // for 9032
#define DFT_GE_IF_TEST 28
#define DFT_LSW_PORT_TEST 29
#define DFT_NAND_FLASH 28
#define DFT_FPGA_TEST 30
#define DFT_CPLD_MANUFAC_IDE 31

/* 33到64：自检测试总线项，可测试类型为1 */
#define DFT_I2C 33
#define DFT_LPC 34
#define DFT_IPMB 35
#define DFT_LOCALBUS 36
#define DFT_PCIE 37
#define DFT_PWM 38
#define DFT_SWITCH9032_TEST 39 // for 9032
#define DFT_CPLD_POST_TEST 40  // for 9032
#define DFT_PECI_TEST 41
#define DFT_USB_PORT_SWITCH_TEST 42
#define DFT_MM_CLOCK 43 // EM时钟信号自检
#define DFT_HEART_BEAT_LOOP_TEST  46   /* 心跳环回测试 */
// 通用的外部器件自检测试，通过写外部硬件寄存器启动/停止自检测试，再从外部寄存器中读取自检测试结果
#define DFT_GENERAL_EXT_HW_TEST 50

/* 65到96：需要前置条件的测试项，可测试类型为2 */
#define DFT_KEYBOARD_TEST 65
#define DFT_CD_TEST 66
#define DFT_FP_TEST 67
#define DFT_VCE_TEST 68
#define DFT_JTAG 69
#define DFT_SPI 70
#define DFT_BMC_UART 71
#define DFT_LCD_UART 72
#define DFT_NSCI 73
#define DFT_ETHERNET_PORT 74
#define DFT_VOLTAGE_TEST 75
#define DFT_PPR_EEP_TEST 76
#define DFT_JUMPER_SELFTEST 78
#define DFT_FCLINK_TEST 79
#define DFT_M8PB_FW_LOAD 80
#define DFT_LAN_SWITCH_TEST 81
#define DFT_XCUJ_RESV_CHAN_TEST 82
#define DFT_FAN_TEST 83
#define DFT_HW_CHAN_TEST 84
#define DFT_NI_TEST 85             // for 9032
#define DFT_NC_SERIAL_LOOP_TEST 86 // for 9032
#define DFT_NI_EYE_TEST 87         // for 9032
#define DFT_PHY_PORT_LOOP_TEST 87
#define DFT_FRU_RESET_TEST 88
#define DFT_IO_READ_TEST 89
#define DFT_IO_WRITE_TEST 90
#define DFT_NI_LINK_BMC_TO_BMC_TEST 91
#define DFT_USB_TEST 92
#define DFT_TYPE_C_USB_TEST 94
#define DFT_OPTICAL_MODULE_TEST 95 // 光模块在位检测


#define DFT_RS485_LOOP_TEST 43                  // 智能盒子RS485串口环回测试项ID
#define FC_DFT 0x44                           /* 自定义功能码：装备测试相关 */
#define FC_DFT_SUB_SWITCH 0x01                /* 子功能码：MCU相关的DFT测试启停开关 */
#define FC_DFT_SUB_GET_RESULT 0x02            /* 子功能码：获取MCU DFT的测试结果 */
#define DFT_SLAVE_DEFAULT_ADDR 0xF7           /* DFT默认从机地址 */
#define MODBUS_SERIAL_TEST_SUB_FUNCODE 0x04     // RS485串口环回装备测试子功能码
#define MODBUS_FIXTURE_ID_TEST_SUB_FUNCODE 0x05 // 夹具ID测试项子功能码
#define MODBUS_SUB_FUNCODE_INDEX 4              // modbus命令子功能索引
#define MODBUS_HEAD_LEN 2                       // modbus命令头部长度
#define MODBUS_RSP_MIN_LEN 3                    // modbus命令响应最小长度


/* 97到128：拷机测试项，可测试性类型为3 */
#define DFT_SD_CARD_OVERALL_TEST 97
#define DFT_VDDQ_VOLT_TEST 98
#define DFT_VDDN_VOLT_TEST 99
#define DFT_AVS_VOLT_TEST 100


/* 129到160：需要人工检查结果的测试项，可测试性类型为4 */
#define DFT_PHYSICAL_LED_TEST 129
#define DFT_LAMP_TUBE_TEST 130
#define DFT_LED_PANEL_TEST 131
#define DFT_PHYSICAL_LED_INTELLIGENCE_TEST 132 // LED指示灯防呆测试
#define DFT_LAMP_TUBE_INTELLIGENCE_TEST 133    // 数码管防呆测试
#define DFT_FAN_LED_TEST 134


/* 161到192，195：需要人工操作测试项，可测试性类型为5 */
#define DFT_HANDLE 161
#define DFT_UID_BUTTON_PRESS 162
#define DFT_POWER_BUTTON_PRESS 163
#define DFT_LCD_SELFTEST 164
#define DFT_LCD_CALIBRATE 165
#define DFT_LCD_LAMP 166
#define DFT_LCD_BACK_LIGHT 167
#define DFT_LCD_DEAD_PIXEL 168
#define DFT_LCD_PRESENCE 169
#define DFT_BUTTON_PRESS 170
#define DFT_SWITCH_TEST 171
#define DFT_MCU_TEST 172 // MCU管理的PCIE卡的装备测试
#define DFT_MAG_VALVE 173 /* 漏液电磁阀开关测试 */
#define DFT_LEAKING_ALARM 174 /* 漏液检测告警测试 */
#define DFT_HIGH_TEMP_PROTECT 175 /* NPU过温保护信号测试 */

#define DFT_CHASSIS_COVER_TEST 195
#define DFT_ENCLOSURE_PULL_TEST 196
#define DFT_PANGEA_TEST 197

/* ****************DFT ITEM类名定义****************** */
#define DFT_ITEM_CLASS "DftItem"

/* ****************DFT ITEM属性名定义**************** */
#define DFT_ID "DftId"
#define SLOT_ID "Slot"                   // 槽位号索引
#define DEV_ID "DevNum"                  // 设备编号
#define TEST_TYPE "TestabilityType"      // 可测试性类型
#define ITEM_NAME "ItemName"             // 测试项名称
#define PROMPT_READY "PromptReady"       // 测试启动前准备提示
#define PROMPT_COMPLETE "PromptComplete" // 测试启动后交互提示
#define REF_OBJECT "RefObject"           // 关联对象
#define PROCESS_TIME "ProcessTime" // 本测试项操作完成时间（如dftid为71的BMC串口环回就表示串口切换加打流时间，
                                   // dftid为87的端口环回测试表示的是环回测试配置模式需要的时间，XCU需要打流的时间不包括在里面）
#define INPUT_DATA "InputData"
#define DFT_ACTION "DftAction"


/* ****************DFT 测试状态定义**************** */
#ifndef COMP_CODE_COMPLETE
#define COMP_CODE_COMPLETE 0x00
#endif
#ifndef COMP_CODE_TESTING
#define COMP_CODE_TESTING 0x80
#endif
#ifndef COMP_CODE_UNSTART
#define COMP_CODE_UNSTART 0x81
#endif

/* *****************DFT 测试命令定义*************** */
typedef enum tag_dft_test_cmd {
    TEST_STOP = 0,   /* 停止测试 */
    TEST_START,      /* 开始测试 */
    TEST_GET_RESULT, /* 获取测试结果 */
} DFT_TEST_CMD_E;

/* *****************DFT 测试结果定义*************** */
typedef enum tag_dft_test_result {
    TEST_SUCCEED = 0, /* 测试成功 */
    TEST_FAILED,      /* 测试失败 */
    TEST_NON = 0xFF,  /* 不涉及 */
} DFT_TEST_RESULT_E;

/* *****************识别FT模式方法***************** */
typedef enum tag_FT_identify_type {
    NO_FT_IDENTIFY_TYPE = 0,                    /* 不识别 */
    ETH_LOOP_FT_IDENTIFY_TYPE = 1,              /* 通过网口环回识别FT装备模式 */
    SERIEAL_LOOP_FT_IDENTIFY_TYPE = 2,          /* 通过串口环回识别FT装备模式 */
    LSW_LOOP_FT_IDENTIFY_TYPE = 3,              /* 通过LSW 端口环回识别装备模式 */
    CONVERGE_SERIEAL_LOOP_FT_IDENTIFY_TYPE = 4, /* 通过汇聚面板串口环回识别FT装备模式 */
} FT_IDENTIFY_TYPE_E;

enum {
    SELF_TEST_NO_ERROR = 0x55,
    SELF_TEST_NOT_IMPLEMENT,
    SELF_TEST_CORRUPT_INACCESS,
    SELF_TEST_FATAL_HW_ERR
};

/* 装备测试版本校验的类型 */
#define VERSION_TYPE_FABRIC 24

#pragma pack(1)
typedef struct tg_IPMIMSG_GET_SELF_TEST_RESULTS_RESP {
    guint8 comp_code;
    guint8 result;

    union {
        guint8 byte;
        struct tagBit {
#ifdef BD_BIG_ENDIAN
            guint8 sel_connot_accsee : 1; // msb at first
            guint8 sdr_connot_accsee : 1;
            guint8 fru_connot_access : 1;
            guint8 ipmb_line_no_response : 1; // 0
            guint8 sdr_empty : 1;
            guint8 fru_internal_use_area_corrupted : 1;     // 0
            guint8 controller_update_boot_fw_corrupted : 1; // 0
            guint8 controller_operational_fw_corrupted : 1; // 0
#else
            guint8 controller_operational_fw_corrupted : 1; // 0
            guint8 controller_update_boot_fw_corrupted : 1; // 0
            guint8 fru_internal_use_area_corrupted : 1;     // 0
            guint8 sdr_empty : 1;
            guint8 ipmb_line_no_response : 1; // 0
            guint8 fru_connot_access : 1;
            guint8 sdr_connot_accsee : 1;
            guint8 sel_connot_accsee : 1; //
#endif
        } BIT_;
    } test_result;
} IPMIMSG_GET_SELF_TEST_RESULTS_RESP_S;
#pragma pack()


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __DFT_DEFINE_H__ */
