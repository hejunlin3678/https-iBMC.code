/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 pmbus 8100 header file
 */
#ifndef _HI51_PS_PMBUS_8100_H_
#define _HI51_PS_PMBUS_8100_H_

#ifndef __HI51_FOR_ARM__
/* PMBUSµçÔ´¼Ä´æÆ÷µØÖ· */
#define PMBUS_8100_CMD_CLEAR_FAULTS          0x03
#define PMBUS_8100_CMD_STATUS_BYTE           0x78
#define PMBUS_8100_CMD_STATUS_WORD           0x79
#define PMBUS_8100_CMD_STATUS_VOUT           0x7A
#define PMBUS_8100_CMD_STATUS_IOUT           0x7B
#define PMBUS_8100_CMD_STATUS_INPUT          0x7C
#define PMBUS_8100_CMD_STATUS_TEMPERATURE    0x7D
#define PMBUS_8100_CMD_STATUS_CML            0x7E
#define PMBUS_8100_CMD_STATUS_OTHER          0x7F
#define PMBUS_8100_CMD_STATUS_MFR_SPECIFIC   0x80
#define PMBUS_8100_CMD_STATUS_FANS_1_2       0x81
#define PMBUS_8100_CMD_READ_VIN              0x88
#define PMBUS_8100_CMD_READ_12VOUT           0x8B
#define PMBUS_8100_CMD_READ_12VIOUT          0x8C
#define PMBUS_8100_CMD_READ_TEMPERATURE_1    0x8D
#define PMBUS_8100_CMD_READ_TEMPERATURE_2    0x8E
#define PMBUS_8100_CMD_READ_POUT             0x96
#define PMBUS_8100_CMD_READ_PIN              0x97
#define PMBUS_8100_CMD_PMBUS_REVISON         0x98
#define PMBUS_8100_CMD_MFR_ID                0x99
#define PMBUS_8100_CMD_MFR_MODEL             0x9A
#define PMBUS_8100_CMD_MFR_REVISION          0x9B
#define PMBUS_8100_CMD_MFR_DATE              0x9D
#define PMBUS_8100_CMD_MFR_SERIAL            0x9E
#define PMBUS_8100_CMD_INPUT_TYPE            0xDF
#define PMBUS_8100_CMD_EVENT                 0xD0
#define PMBUS_8100_CMD_PART_NUMBER           0xDE

guint16 pmbus_8100_get_output_power(guint8 ps_id);
guint16 pmbus_8100_get_input_power(guint8 ps_id);
guint16 pmbus_8100_get_vin(guint8 ps_id);
guint16 pmbus_8100_get_vout(guint8 ps_id);
guint16 pmbus_8100_get_iin(guint8 ps_id);
guint16 pmbus_8100_get_iout(guint8 ps_id);
guint32 pmbus_8100_get_health_event(guint8 ps_id);
guint16 pmbus_8100_get_env_temp(guint8 ps_id);
guint16 pmbus_8100_get_chip_temp(guint8 ps_id);
gint8 pmbus_8100_probe(guint8 dev_id);
guint8 pmbus_8100_check_input_loss(guint32 event);
void pmbus_8100_get_input_type(guint8 ps_id);

extern __idata guint32 g_pmbus_8100_health_get_cnt;
#endif /* __HI51_FOR_ARM__ */

#endif