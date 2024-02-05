

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "ipmc_shelf_common.h"
#include "uip.h"
#include "ipmc_public.h"
#include "ipmctypedef.h"
#include "ipmcgetcommand.h"


gint32 ipmc_get_serialdir(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    gchar src_name[MAX_PROPERTY_VALUE_LEN] = {0};
    gchar dest_name[MAX_PROPERTY_VALUE_LEN] = {0};
    guint8 serial_num = 0;
    guint8 connect_status = 0;
    guint8 availability = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_NAME_SERIAL_MANAGEMENT, &obj_list);
    if (ret != DFL_OK || g_slist_last(obj_list) == 0) {
        g_printf("Get serial management object list failed.\r\n");
        return FALSE;
    }

    g_printf("Current connected serial direction : \n");
    g_printf("%-10s    %-20s    %-20s\r\n", "Num", "Source", "Destination");

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)obj_note->data;
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_INDEX, &serial_num);
        ret += dal_get_property_value_string(obj_handle, PROPERTY_SRC_SERIAL_NAME, src_name, sizeof(src_name));
        ret += dal_get_property_value_string(obj_handle, PROPERTY_DEST_SERIAL_NAME, dest_name, sizeof(dest_name));
        ret += dal_get_property_value_byte(obj_handle, PROPERTY_CONNECT_STATUS, &connect_status);
        ret += dal_get_property_value_byte(obj_handle, PROPERTY_AVAILABILITY, &availability);
        if (ret != DFL_OK) {
            g_printf("Get serial management information failed.\r\n");
            g_slist_free(obj_list);
            return FALSE;
        }

        if (connect_status == 1 && availability == 1) {
            g_printf("%-10d    %-20s    %-20s\r\n", serial_num, src_name, dest_name);
        }
    }

    g_slist_free(obj_list);
    return TRUE;
}


gint32 ipmc_get_serialnode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 node_id;
    guint8 max_node_id = 0;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get %s object  failed.\r\n", CLASS_NAME_CHASSIS);
        return RET_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_PANEL_SERIAL_SW_NODE_ID, &node_id);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_MAX_NODE_INDEX, &max_node_id);

    if (node_id < 0 || node_id > max_node_id) {
        g_printf("Panel serial port is empty and is not connected to any node.\r\n");
        return RET_ERR;
    }
    
    if (node_id == 0) {
        g_printf("Connected to panel serial port: Management Board Node.\r\n");
    } else {
        g_printf("Connected to panel serial port: NODE%d.\r\n", node_id);
    }

    return RET_OK;
}