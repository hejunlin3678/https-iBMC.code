

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#include "ipmc_shelf_common.h"
#include "uip.h"
#include "ipmc_public.h"
#include "ipmctypedef.h"
#include "ipmcsetcommand.h"

typedef struct {
    gint32 status;
    const gchar *description;
} STATUS_DESC_INFO;


LOCAL void print_serialdir_helpinfo(void)
{
#define SERIAL_UNAVAILABILITY 0
#define SERIAL_NOT_SHOWING 2
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    guint8 serial_num = 0;
    guint8 availability = 0;
    guint8 src_id = 0;
    guint8 dst_id = 0;
    gchar src_name[MAX_PROPERTY_VALUE_LEN] = {0};
    gchar dest_name[MAX_PROPERTY_VALUE_LEN] = {0};

    g_printf("Usage: ipmcset -d serialdir -v <option>\r\n");
    g_printf("Options are:\r\n");

    gint32 ret = dfl_get_object_list(CLASS_NAME_SERIAL_MANAGEMENT, &obj_list);
    if (ret != DFL_OK || g_slist_last(obj_list) == 0) {
        return;
    }

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        OBJ_HANDLE obj_handle = (OBJ_HANDLE)obj_note->data;
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_INDEX, &serial_num);
        ret += dal_get_property_value_byte(obj_handle, PROPERTY_AVAILABILITY, &availability);
        ret += dal_get_property_value_byte(obj_handle, PROPERTY_SRC_SERIAL_ID, &src_id);
        ret += dal_get_property_value_byte(obj_handle, PROPERTY_DEST_SERIAL_ID, &dst_id);
        ret += dal_get_property_value_string(obj_handle, PROPERTY_SRC_SERIAL_NAME, src_name, sizeof(src_name));
        ret += dal_get_property_value_string(obj_handle, PROPERTY_DEST_SERIAL_NAME, dest_name, sizeof(dest_name));
        if (ret != DFL_OK) {
            g_printf("Get serial management information failed.\r\n");
            g_slist_free(obj_list);
            return;
        }

         
        if (availability != SERIAL_UNAVAILABILITY && availability != SERIAL_NOT_SHOWING &&
            ((SERIAL_SELECTOR_HOST_SERIAL != src_id) || (SERIAL_SELECTOR_BMC_DEBUG != dst_id))) {
            g_printf(" %-3d     Serial switch from %s to %s\r\n", serial_num, src_name, dest_name);
        }
    }

    g_slist_free(obj_list);
    return;
}

LOCAL gint32 get_serial_management_handle(guint8 serial_num, OBJ_HANDLE *serial_handle)
{
    gint32 ret;
    guint8 availability = 0xff;
    guint8 src_id = 0;
    guint8 dst_id = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_specific_object_byte(CLASS_NAME_SERIAL_MANAGEMENT, PROPERTY_INDEX, serial_num, &obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_AVAILABILITY, &availability);
    if ((ret != RET_OK) || (availability == 0)) {
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SRC_SERIAL_ID, &src_id);
    ret += dal_get_property_value_byte(obj_handle, PROPERTY_DEST_SERIAL_ID, &dst_id);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    if ((src_id == SERIAL_SELECTOR_HOST_SERIAL) && (dst_id == SERIAL_SELECTOR_BMC_DEBUG)) {
        return RET_ERR;
    }

    *serial_handle = obj_handle;
    return RET_OK;
}


gint32 ipmc_set_serialdir(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 serial_num = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if ((argc != 2) || (strcmp(argv[0], "-v"))) { 
        print_serialdir_helpinfo();
        return FALSE;
    }

    if ((serial_num = str_to_int(argv[1])) == IPMC_ERROR) {
        print_serialdir_helpinfo();
        return FALSE;
    }

    if (serial_num > 0xFF) {
        print_serialdir_helpinfo();
        return FALSE;
    }

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        
    }

    ret = get_serial_management_handle((guint8)serial_num, &obj_handle);
    if (ret != RET_OK) {
        print_serialdir_helpinfo();
        return FALSE;
    }

    
    ret = uip_call_class_method(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
        CLASS_NAME_SERIAL_MANAGEMENT, dfl_get_object_name(obj_handle), METHOD_SET_SERIAL_DIRECTION, NULL, NULL);
    
    if (ret == VOS_ERR_NOTSUPPORT) {
        print_serialdir_helpinfo();
        return FALSE;
    }

    
    if (ret != RET_OK) {
        g_printf("Set serial port direction failed.\r\n");
        return FALSE;
    }

    g_printf("Set serial port direction successfully.\r\n");
    return TRUE;
}

LOCAL gint32 set_serialnode(OBJ_HANDLE obj_handle, guint8 node_id)
{
    gint32 ret;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    STATUS_DESC_INFO desc[] = {
        {CHASSIS_SET_SERIAL_NODE_INVALID_ID, "Invalid Node ID.\r\n"},
        {CHASSIS_SET_SERIAL_NODE_NOT_PRESENT, "The node is not present.\r\n"}
    };

    if (get_username_ip(username, ip, sizeof(ip)) != RET_OK) {
        snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_byte(node_id));
    input_list = g_slist_append(input_list, g_variant_new_uint32(DF_SAVE));
    ret= uip_call_class_method_with_handle(OPERATOR_LOG_CLI, username, ip, CLASS_NAME_CHASSIS,
        obj_handle, METHOD_CHASSIS_SET_PANEL_SERIAL_SW_NODE_ID, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call %s failed, ret = %d",
            __FUNCTION__, METHOD_CHASSIS_SET_PANEL_SERIAL_SW_NODE_ID, ret);
    }

    for (gsize i = 0; i < ARRAY_SIZE(desc); ++i) {
        if (ret == desc[i].status) {
            g_printf("%s", desc[i].description);
        }
    }

    return ret;
}


LOCAL void print_serialnode_helpinfo(void)
{
#define NODE_PRESENT 1
    g_printf("Usage: ipmcset -d serialnode -v <nodeid>\r\n");
    OBJ_HANDLE obj_handle = 0;
    guint32 node_present = 0;
    guint8 max_node_idx = 0;
    guint8 software_type = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Get Chassis object failed, err code %d", __func__, ret);
        return;
    }
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_CHASSIS_CHASSIS_NODE_PRESENT, &node_present);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Get ChassisNodePresent property failed, err code %d", __func__, ret);
        return;
    }
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_MAX_NODE_INDEX, &max_node_idx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s Get NodeIndex property failed, err code %d", __func__, ret);
        return;
    }

    g_printf("Options are:\r\n");
    (void)dal_get_software_type(&software_type);
    if (software_type == MGMT_SOFTWARE_TYPE_RM) {
        g_printf(" %-3d     Serial Port switch to Management Board Node\r\n", 0);
    }
    for (guint8 idx = 0; idx < max_node_idx; idx++) {
        guint8 present = (node_present >> idx) & 0x01;
        if (present == NODE_PRESENT) {
            g_printf(" %-3d     Serial Port switch to Node%u\r\n", idx + 1, idx + 1);
        }
    }
    return;
}


gint32 ipmc_set_serialnode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    gint32 node_id;
    OBJ_HANDLE obj_handle = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 2 || strcmp(argv[0], "-v")) { 
        print_serialnode_helpinfo();
        return FALSE;
    }

    if ((node_id = str_to_int(argv[1])) == IPMC_ERROR) {
        print_serialnode_helpinfo();
        return FALSE;
    }

    if (node_id > 0xFF) {
        print_serialnode_helpinfo();
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0, &obj_handle);
    if (ret != RET_OK) {
        print_serialnode_helpinfo();
        return FALSE;
    }
        
    ret = set_serialnode(obj_handle, (guint8)node_id);
    if (node_id == 0) {
        g_printf("Set panel serial port to Management Board Node %s.\r\n",
            (ret == RET_OK) ? "successfully" : "failed");
    } else {
        g_printf("Set panel serial port to NODE%s %s.\r\n",
            argv[1], (ret == RET_OK) ? "successfully" : "failed");
    }

    return ret == RET_OK ? TRUE : FALSE;
}