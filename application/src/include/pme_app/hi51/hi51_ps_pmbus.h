/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 ps pmbus header file
 */
#ifndef _HI51_PS_PMBUS_H_
#define _HI51_PS_PMBUS_H_

#include <hi51_sys_config.h>

/*
 * PMBUS电源寄存器地址
 * https://pmbus.org/Assets/PDFS/Public/PMBus_Specification_Part_II_Rev_1-1_20070205.pdf
 * Table 26. Command Summary
 */
#define PMBUS_CMD_VOUT_MODE             0x20
#define PMBUS_CMD_STATUS_BYTE           0x78
#define PMBUS_CMD_STATUS_WORD           0x79
#define PMBUS_CMD_STATUS_VOUT           0x7A
#define PMBUS_CMD_STATUS_IOUT           0x7B
#define PMBUS_CMD_STATUS_INPUT          0x7C
#define PMBUS_CMD_STATUS_TEMPERATURE    0x7D
#define PMBUS_CMD_STATUS_CML            0x7E
#define PMBUS_CMD_STATUS_OTHER          0x7F
#define PMBUS_CMD_STATUS_MFR_SPECIFIC   0x80
#define PMBUS_CMD_STATUS_FANS_1_2       0x81
#define PMBUS_CMD_READ_VIN              0x88
#define PMBUS_CMD_READ_IIN              0x89
#define PMBUS_CMD_READ_12VOUT           0x8B
#define PMBUS_CMD_READ_12VIOUT          0x8C
#define PMBUS_CMD_READ_TEMPERATURE_1    0x8D
#define PMBUS_CMD_READ_TEMPERATURE_2    0x8E
#define PMBUS_CMD_READ_TEMPERATURE_3    0x8F
#define PMBUS_CMD_READ_POUT             0x96
#define PMBUS_CMD_READ_PIN              0x97
#define PMBUS_CMD_PMBUS_REVISON         0x98
#define PMBUS_CMD_MFR_ID                0x99
#define PMBUS_CMD_MFR_MODEL             0x9A
#define PMBUS_CMD_MFR_REVISION          0x9B
#define PMBUS_CMD_MFR_DATE              0x9D
#define PMBUS_CMD_MFR_SERIAL            0x9E
#define PMBUS_CMD_INPUT_TYPE            0xDF
#define PMBUS_CMD_POWER_TYPE            0xCF
#define PMBUS_CMD_EVENT                 0xD0
#define PMBUS_CMD_PART_NUMBER           0xDE
#define PMBUS_CMD_PS_RATE               0x31
#define PMBUS_5500_CMD_INPUT_TYPE       0xD8
#define PMBUS_5500_CMD_PS_RATE          0xA7

#define IS_PMBUS_CMD_READ_I(cmd) ((cmd) == PMBUS_CMD_READ_IIN || (cmd) == PMBUS_CMD_READ_12VIOUT)

guint16 pmbus_get_output_power(guint8 ps_id);
guint16 pmbus_get_input_power(guint8 ps_id);
guint16 pmbus_get_vin(guint8 ps_id);
guint16 pmbus_get_vout(guint8 ps_id, guint8 dev_type);
guint16 pmbus_get_iin(guint8 ps_id);
guint16 pmbus_get_iout(guint8 ps_id);
guint32 pmbus_get_health_event(guint8 ps_id, guint8 dev_type);
guint16 pmbus_get_env_temp(guint8 ps_id);
guint16 pmbus_get_chip_temp(guint8 ps_id, guint8 cmd);
guint8 pmbus_probe(guint8 dev_id);
guint8 pmbus_check_input_loss(guint32 event);

#endif