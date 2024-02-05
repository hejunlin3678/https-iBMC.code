


#ifndef __FRU_COMPONENT_MEZZ_H__
#define __FRU_COMPONENT_MEZZ_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "common.h"

gint32 renew_mezz_mac(FRU_PRIV_PROPERTY_S *fru_priv);

gint32 read_mezz_card_mac_addr(OBJ_HANDLE object_handle, gchar *property_name, guchar (*mac_addr)[MAX_MAC_STR_LEN],
    guint8 *mac_len, guint32 mac_addr_num);

gint32 get_mezz_pcie_connect_info(guchar device_type, guint8 device_num, guchar *resp_data, gint32 data_size,
    gint32 read_offset, guint8 read_len, guint8 *end_flag, guint8 *resp_data_len);

void clear_mezz_mac(FRU_PRIV_PROPERTY_S *fru_priv);


#endif 
