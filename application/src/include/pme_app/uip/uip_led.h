#ifndef __UIP_LED_H__
#define __UIP_LED_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FRU_LED_SUPPORT_BLUE 2
#define FRU_LED_SUPPORT_RED 4
#define FRU_LED_SUPPORT_GREEN 8
#define FRU_LED_SUPPORT_AMBER 16
#define FRU_LED_SUPPORT_ORANGE 32
#define FRU_LED_SUPPORT_WHITE 64

#define FRU_LED_COLOR_RESERVED 0
#define FRU_LED_COLOR_BLUE 1
#define FRU_LED_COLOR_RED 2
#define FRU_LED_COLOR_GREEN 3
#define FRU_LED_COLOR_AMBER 4
#define FRU_LED_COLOR_ORANGE 5
#define FRU_LED_COLOR_WHITE 6

enum UIP_LED_IDENTIFY
{
    UIP_LED_OFF,    //0-Ãð
    UIP_LED_ON,     //1-ÁÁ
    UIP_LED_BLINK,  //2-ÉÁË¸
    UIP_LED_BUTT,
};

extern gint32 uip_set_uid_led_state(gchar state, gchar timeout);

#ifdef __cplusplus
}
#endif
#endif

