/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 pmbus 8100 header file
 */
#ifndef _HI51_PMBUS_8100_H_
#define _HI51_PMBUS_8100_H_

#ifdef _IN_51_

#ifndef __HI51_FOR_ARM__
guint8 pmbus_8100_get_raw_byte(guint8 bus_id, guint8 slave, guint8 command);
guint8 pmbus_8100_set_raw_byte(guint8 bus_id, guint8 slave, guint8 command);
guint16 pmbus_8100_get_raw_word(guint8 bus_id, guint8 slave, guint8 command);
guint16 pmbus_8100_get_linear_11(guint8 bus_id, guint8 slave, guint8 command);
void pmbus_8100_get_block(guint8 bus_id, guint8 slave, guint8 command, guint32 buf_offset);
guint16 pmbus_8100_get_linear_16(guint8 bus_id, guint8 slave, guint8 command);
extern __idata guint32 g_pmbus_8100_crc_error_cnt;
#endif /*  */

#endif

#endif