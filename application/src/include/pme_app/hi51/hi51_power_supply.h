/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2001-2020. All rights reserved.
 * Description: hi51 power supply header file
 */
#ifndef _HI51_POWER_SUPPLY_H_
#define _HI51_POWER_SUPPLY_H_

#define PS_POWER_FROM_INVALID   0xFF    /* 未知功率来源 */
#define PS_POWER_FROM_CHIP      0       /* 从INA220获取功率信息 */
#define PS_POWER_FROM_PSU       1       /* 从电源模块获取功率信息 */

#define PS_PROTOCOL_INVALID     0xFF    /* 未知电源协议类型 */
#define PS_PROTOCOL_PSMI        0       /* PSMI */
#define PS_PROTOCOL_PMBUS       1       /* PMBUS */
#define PS_PROTOCOL_INA220      2       /* INA220 */
#define PS_PROTOCOL_PMBUS_5500  8       /* PMBUS 5500 专用 */
#define PS_PROTOCOL_PMBUS_8100  9       /* PMBUS 8100 专用 */

#define PS_CMD_GET_POWER      0X10
#define PS_CMD_GET_EVENT      0X11

typedef struct TAG_PS_EVENT_BIT_S {
    guint8 failure : 1;
    guint8 over_voltage : 1;
    guint8 over_current : 1;
    guint8 over_temperature : 1;
    guint8 input_loss : 1;
    guint8 rev3 : 1;
    guint8 fan1_failure : 1;
    guint8 fan2_failure : 1;
} PS_EVENT_BIT_S;

#define PS_EVENT_RESERVED_BYTE 3
typedef union TAG_PS_EVENT_S {
    struct {
        struct TAG_PS_EVENT_BIT_S event;
        guint8 rev[PS_EVENT_RESERVED_BYTE];
    } bitval;

    guint32 u32val;
} PS_EVENT_S;

typedef union TAG_PS_EVENT_LOCAL_S {
    struct TAG_PS_EVENT_BIT_S event;
    guint8 u8val;
} PS_EVENT_LOCAL_S;

#define PS_FLAG_RESERVED_BYTE 3
typedef struct TAG_PS_FLAG_BIT_S {
    guint8 output : 1;
    guint8 intput : 1;
    /* Modified by w00213782, PN:DTS2014111407928 */
    guint8 clear_alarm : 1;
    guint8 rev1 : 5;
    guint8 rev[PS_FLAG_RESERVED_BYTE];
} PS_FLAG_BIT_S;

typedef union TAG_PS_FLAG_S {
    struct TAG_PS_FLAG_BIT_S flag;
    guint32 u32val;
} PS_FLAG_S;

#ifdef _IN_51_

guint8 get_ps_id_from_dev_id(guint8 dev_id);
guint16 ps_get_power_default(guint8 ps_id);
guint32 ps_get_event_default(guint8 ps_id);
void ps_get_addr_info(guint8 ps_id, guint8 *slave, guint8 *bus_id);
guint8 ps_get_dev_type(guint8 ps_id);
guint16 ps_get_power_in(guint8 ps_id);
guint16 ps_get_power_out(guint8 ps_id);
guint16 ps_get_env_temp(guint8 ps_id);
guint16 ps_get_chip_temp(guint8 ps_id, guint8 cmd);
guint32 ps_get_event(guint8 ps_id);
guint8 ps_check_input_loss(guint8 ps_id, guint32 event);
guint16 ps_get_vin(guint8 ps_id);
guint16 ps_get_vout(guint8 ps_id);
guint16 ps_get_i_in(guint8 ps_id);
guint16 ps_get_i_out(guint8 ps_id);
#ifndef __HI51_FOR_ARM__
guint32 ps_get_input_type(guint8 ps_id);
#endif
void ps_exit_default(guint8 dev_id);
void ps_init(void);

#endif /* end of _IN_51_ */

#endif