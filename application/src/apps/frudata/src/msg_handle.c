

#include "msg_handle.h"
#include "prepare_fru.h"

gint32 reboot_request_process(REBOOT_CTRL ctrl)
{
    if (ctrl == REBOOT_FORCE || ctrl == REBOOT_PERFORM) {
        check_eeprom_write_status();
        (void)per_exit();
        return RET_OK;
    }

    if (ctrl == REBOOT_CANCEL) {
        return RET_OK;
    }

    return RET_OK;
}

gint32 add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    if (class_name == NULL) {
        return RET_ERR;
    }

    frudata_add_object_callback(class_name, object_handle);

    return RET_OK;
}

gint32 add_object_complete_callback(guint32 position)
{
    (void)frudata_add_object_complete_callback(position);
    return RET_OK;
}

gint32 del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle)
{
    return frudata_del_object_callback(class_name, object_handle);
}

gint32 del_object_complete_callback(guint32 position)
{
    (void)frudata_del_object_complete_callback(position);
    return RET_OK;
}
