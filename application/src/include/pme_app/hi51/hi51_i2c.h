/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 i2c header file
 */
#ifndef _HI51_I2C_H_
#define _HI51_I2C_H_

#define I2C_SPEED_SET_100K  0
#define I2C_SPEED_SET_400K  1
#define I2C_SPEED_SET_3M4   2

#ifdef _IN_51_

#define I2C_DELAY_UNIT      100

#define I2C_WAIT_TIMEOUT    1000

extern guint32 g_i2c_rx_off;
void switch_9545_channel(guint8 dev_id);
void i2c_init(void);
gint8 i2c_read_msg(guint8 bus_id, guint8 addr, guint8 tx_len, guint8 rx_len, guint16 time_out);
gint8 i2c_write_msg(guint8 bus_id, guint8 addr, guint16 tx_len, guint16 data_len, guint16 time_out);
gint8 pmbus_read_block(guint8 bus_id, guint8 addr, guint8 tx_len, guint16 time_out);
extern __idata guint16 g_i2c_tmoutcnt;

#endif /* end of _IN_51_ */

#endif