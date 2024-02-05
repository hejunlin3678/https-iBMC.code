

#ifndef __WDT2_IPMI_H__
#define __WDT2_IPMI_H__

gint32 wdt_ipmi_get_timer(const void *req_msg, gpointer user_data);
gint32 wdt_ipmi_reset_timer(const void *req_msg, gpointer user_data);
gint32 wdt_ipmi_set_timer(const void *req_msg, gpointer user_data);
#endif 