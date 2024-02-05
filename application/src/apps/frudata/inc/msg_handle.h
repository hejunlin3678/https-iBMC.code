

#ifndef __FRU_MSG_HANDLE_H__
#define __FRU_MSG_HANDLE_H__


#include "pme/pme.h"
#include "pme_app/pme_app.h"

gint32 reboot_request_process(REBOOT_CTRL ctrl);

gint32 add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);

gint32 add_object_complete_callback(guint32 position);

gint32 del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);

gint32 del_object_complete_callback(guint32 position);


#endif 
