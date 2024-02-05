/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 ps ina220 header file
 */
#ifndef _HI51_PS_INA220_H_
#define _HI51_PS_INA220_H_

#define INA220_REG_CONFIGURATION    0x00
#define INA220_REG_SHUNT_VOLTAGE    0x01
#define INA220_REG_BUS_VOLTAGE      0x02
#define INA220_REG_POWER            0x03
#define INA220_REG_CURRENT          0x04
#define INA220_REG_CALIBRATION      0x05
/* h00272616  DTS2016010404377 */
#define INA220_INVALID_POWER_VALUE  0xffff

guint16 ina220_get_input_power(guint8 ps_id);
guint16 ina220_get_vin(guint8 ps_id);
guint16 ina220_get_iin(guint8 ps_id);
gint8 ina220_probe(guint8 dev_id);
void ina220_set_refer_cfg(guint8 bus_id, guint8 slave_addr, guint16 cfg);
void ina220_set_calib_cfg(guint8 bus_id, guint8 slave_addr, guint16 calib);
gint32 ina220_get_reg(guint8 bus_id, guint8 addr, guint8 offset, guint16 *val);

#endif