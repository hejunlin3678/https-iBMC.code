


#ifndef __FRU_COMPONENT_SDI_H__
#define __FRU_COMPONENT_SDI_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"

gint32 get_sdi_set_ip_status(guint8 device_type, guint8 device_num, guint8 *status, guint32 *ipaddr, guint16 *vlan);

gint32 get_sdi_os_loaded_info(guint8 device_type, guint8 device_num, guint8 *os_loaded);

gint32 get_sdi_boot_order_info(guint8 device_type, guint8 device_num, guint8 *boot_order, guint8 *vaild_type);

gint32 get_sdi_storage_ip_info(guint8 device_type, guint8 device_num, guint8 *status, guint32 *ipaddr, guint16 *vlan);

gint32 get_sdi_reset_linkage_info(guint8 device_type, guint8 device_num, guint8 *reset_linkage_mode);


#endif 
