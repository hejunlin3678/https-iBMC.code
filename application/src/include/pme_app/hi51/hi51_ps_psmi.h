/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 ps psmi header file
 */
#ifndef _HI51_PS_PSMI_H_
#define _HI51_PS_PSMI_H_

#ifndef __HI51_FOR_ARM__
/* PSMIµçÔ´¼Ä´æÆ÷µØÖ· */
#define PSMI_REG_ADDR_SHUTDOWN_EVENT    0x04
#define PSMI_REG_ADDR_PROTOCOL          0x98
#define PSMI_REG_ADDR_INPUT_VOLTAGE     0X08
#define PSMI_REG_ADDR_INPUT_CURRENT     0X0A
#define PSMI_REG_ADDR_INPUT_POWER       0X0C
#define PSMI_REG_ADDR_OUTPUT_VOLTAGE    0X0E
#define PSMI_REG_ADDR_OUTPUT_CURRENT    0X10
#define PSMI_REG_ADDR_OUTPUT_POWER      0X12
#define PSMI_REG_ADDR_INLET_TEMPERATURE 0X1A
#define PSMI_REG_ADDR_CONTROL           0X3A

guint16 psmi_get_input_power(guint8 ps_id);
guint16 psmi_get_output_power(guint8 ps_id);
guint16 psmi_get_vin(guint8 ps_id);
guint16 psmi_get_vout(guint8 ps_id);
guint16 psmi_get_iin(guint8 ps_id);
guint16 psmi_get_iout(guint8 ps_id);
guint32 psmi_get_health_event(guint8 ps_id);
guint16 psmi_get_env_temp(guint8 ps_id);
gint8 psmi_probe(guint8 dev_id);
guint8 psmi_check_input_loss(guint32 event);
#endif

#endif