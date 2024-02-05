


#ifndef __FRU_EVENT_H__
#define __FRU_EVENT_H__
#include "pme/pme.h"
#include "pme_app/pme_app.h"

gint32 event_renew_product_sn_elabel(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 event_renew_product_assettag_elabel(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 event_update_fru_ipmbaddr(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 event_update_fru_locator_record_id(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 event_update_hdd_component(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 event_process_as_status_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);

#endif 
