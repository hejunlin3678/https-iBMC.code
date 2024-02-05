

#include <netinet/in.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pme_app/uip/uip_network_config.h>
#include "pme_app/pme_app.h"
#include "pme_app/uip/uip_object_interface.h"
#include "ipmcgetcommand.h"
#include "ipmc_public.h"
#include "ipmc_shelf_common.h"

gint32 ipmc_lsw_get_ctrl_portstatistics(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv,
                                        gchar* target)
{
    gint32 ret;
    const gchar* stat_buf = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList* input_list = NULL;
    GSList* output_list = NULL;

    
    ret = ipmc_privilege_judge(privilege, rolepriv);
    if (ret == FALSE) {
        return FALSE;
    }

    if (argc != NUM_2 || (strcmp(argv[0], "-v"))) {
        goto OUT_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("get obj handle failed, ret = %d\r\n", ret);
        return FALSE;
    }

    if (g_utf8_validate(argv[1], strlen(argv[1]), NULL) == FALSE) {
        goto OUT_ERROR;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(argv[1]));
    ret = dfl_call_class_method(obj_handle, METHOD_CP_GET_STATIS, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) { 
        goto OUT_ERROR;
    }

    stat_buf = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    g_printf("%s", stat_buf);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;
    return TRUE;

OUT_ERROR:
    if (dal_check_if_mm_board() == TRUE) {
        g_printf("Usage: ipmcget [-l smm] -t lsw -d portstatistics -v <portname>\r\n");
    } else {
        g_printf("Usage: ipmcget -t lsw -d portstatistics -v <portname>\r\n");
    }
    return FALSE;
}
gint32 ipmc_lsw_get_ctrl_l2table(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar* table_buf = NULL;
    gchar* tmp_buf = NULL;

    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        goto OUT_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("obj list not found, ret = %d\r\n", ret);
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_CP_GET_L2_TABLE, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        goto OUT_ERROR;
    }

    table_buf = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    tmp_buf = (gchar*)g_malloc0(strlen(table_buf) + 1);
    if (tmp_buf == NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        output_list = NULL;
        g_printf("memory is not enough\r\n");
        return FALSE;
    }

    ret = strcpy_s(tmp_buf, strlen(table_buf) + 1, table_buf);
    uip_free_gvariant_list(output_list);
    if (ret != RET_OK) {
        g_free(tmp_buf); 
        tmp_buf = NULL; 
        g_printf("strcpy_s failed\r\n");
        return FALSE;
    }

    ipmc_get_lsw_print(tmp_buf);
    g_free(tmp_buf);
    tmp_buf = NULL;

    return TRUE;
OUT_ERROR:
    if (dal_check_if_mm_board() == TRUE) {
        g_printf("Usage: ipmcget [-l smm] -t lsw -d l2table\r\n");
    } else {
        g_printf("Usage: ipmcget -t lsw -d l2table\r\n");
    }
    return FALSE;
}

gint32 ipmc_lsw_get_ctrl_portinfo(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    const gchar* port_info_buf = NULL;
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        goto OUT_ERROR;
    }

    ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("l2table obj list not found, ret = %d\r\n", ret);
        return FALSE;
    }
    g_printf("Port Id  | Port Name    | Link     | Port Enable  | Duplex   | Speed(Mbps)  | Autonego\r\n");

    ret = dfl_call_class_method(obj_handle, METHOD_CP_GET_PORT_INFO, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        goto OUT_ERROR;
    }

    port_info_buf = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    g_printf("%s", port_info_buf);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;

    return TRUE;
OUT_ERROR:
    if (dal_check_if_mm_board() == TRUE) {
        g_printf("Usage: ipmcget [-l smm] -t lsw -d portinfo\r\n");
    } else {
        g_printf("Usage: ipmcget -t lsw -d portinfo\r\n");
    }
    return FALSE;
}

gint32 ipmc_lsw_get_ctrl_chipinfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    const gchar *chip_info_buf = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        if (dal_check_if_mm_board() == TRUE) {
            g_printf("Usage: ipmcget [-l smm] -t lsw -d chipinfo\r\n");
        } else {
            g_printf("Usage: ipmcget -t lsw -d chipinfo\r\n");
        }
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("get obj handle of %s failed, ret=%d\r\n", CLASS_LSW_PUBLIC_ATTR_LOCAL_CP, ret);
        return FALSE;
    }
    ret = dfl_call_class_method(obj_handle, METHOD_CP_GET_CHIP_INFO, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        g_printf("dfl_call_class_method failed, ret=%d\r\n", ret);
        return FALSE;
    }
    chip_info_buf = g_variant_get_string((GVariant*)g_slist_nth_data(output_list, 0), NULL);
    g_printf("%s\r\n", chip_info_buf);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;

    return TRUE;
}

gint32 ipmc_lsw_get_data_l2table(guchar privilege, const gchar* rolepriv, guchar argc,
                                 gchar** argv, gchar* target)
{   
    gint32 ret;
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar* table_buf = NULL;
    gchar* tmp_l2table_buf = NULL;
    gchar *class_name = CLASS_LSW_PUBLIC_ATTR_LOCAL_DP;
    gchar *class_method = METHOD_DATA_L2; 

    if (ipmc_preconditions_no_argc_judge(privilege, rolepriv, argc) == FALSE) {
        return FALSE;
    }

    ret = dal_get_object_handle_nth((const gchar *)class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("get obj handle failed, ret = %d\r\n", ret);
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, (const gchar *)class_method, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        g_printf("Get l2table info failed.\r\n");
        return FALSE;
    }

    table_buf = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    tmp_l2table_buf = (gchar*)g_malloc0(strlen(table_buf) + 1);
    if (tmp_l2table_buf == NULL) {
        uip_free_gvariant_list(output_list); 
        output_list = NULL; 
        g_printf("g_malloc0 failed\r\n");
        return FALSE;
    }

    ret = strcpy_s(tmp_l2table_buf, strlen(table_buf) + 1, table_buf);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;
    if (ret != RET_OK) {
        g_free(tmp_l2table_buf);
        tmp_l2table_buf = NULL; 
        g_printf("strcpy_s failed\r\n");
        return FALSE;
    }

    ipmc_get_lsw_print(tmp_l2table_buf);
    g_free(tmp_l2table_buf);
    tmp_l2table_buf = NULL;

    return TRUE;
}

gint32 ipmc_lsw_get_data_portinfo(guchar privilege, const gchar* rolepriv, guchar argc,
                                  gchar** argv, gchar* target)
{
    gint32 ret;
    guint32 logic_port;
    guint32 phy_port;
    guint32 speed;
    guint32 link_status;
    guint32 port_enable;
    guint32 is_autonego; 
    const gchar* port_name = NULL;
    GSList* input_list = NULL;
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar *class_name = CLASS_LSW_PUBLIC_ATTR_LOCAL_DP;
    gchar *class_method = METHOD_DATA_INFO; 

    if (ipmc_preconditions_no_argc_judge(privilege, rolepriv, argc) == FALSE) {
        return FALSE;
    }

    ret = dal_get_object_handle_nth((const gchar *)class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("obj list not found, ret = %d\r\n", ret);
        return FALSE;
    }
    g_printf("Port Id  | Port Name  | Link     | Port Enable  | Duplex   | Speed(Gbps)  | Autonego\r\n");

    for (logic_port = 0; logic_port < LSW_CHIP_LOGIC_PORT_MAX; logic_port++) {
        input_list = g_slist_append(input_list, g_variant_new_uint32(logic_port));
        ret = dfl_call_class_method(obj_handle, (const gchar *)class_method, NULL, input_list, &output_list);
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        input_list = NULL;
        if (ret != DFL_OK) {
            continue;
        }

        phy_port = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, NUM_0));
        port_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, NUM_1), NULL);
        link_status = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, NUM_2));
        port_enable = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, NUM_3));
        speed = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, NUM_4));
        is_autonego = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, NUM_5));

        g_printf("%-9d| %-11s| %-9s| %-13s| %-9s| %-13d| %-10s\r\n", phy_port, port_name,
                 ((link_status == 0) ? "Down" : "Up"), ((port_enable == 0) ? "Disable" : "Enable"),
                 "Full", speed, ((is_autonego == 0) ? "Disable" : "Enable"));

        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        output_list = NULL;
    }

    return TRUE;
}

gint32 ipmc_lsw_get_data_portstatistics(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv,
                                        gchar* target)
{
    gint32 ret;
    const gchar* stat_buf = NULL;
    OBJ_HANDLE obj_handle = 0;
    GSList* output_list = NULL;
    GSList* input_list = NULL;
    gchar *class_name = CLASS_LSW_PUBLIC_ATTR_LOCAL_DP;
    gchar *class_method = METHOD_DATA_STATIS;

    
    ret = ipmc_privilege_judge(privilege, rolepriv);
    if (ret == FALSE) {
        return FALSE;
    }

    if (argc != NUM_2 || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t <mesh0> -d portstatistics -v <portname>\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth((const gchar *)class_name, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("get obj_handle fail, ret = %d\r\n", ret);
        return FALSE;
    }

    if (g_utf8_validate(argv[1], strlen(argv[1]), NULL) == FALSE) {
        g_printf("Usage: ipmcget -t <mesh0> -d portstatistics -v <portname>\r\n");
        return FALSE;
    }
    input_list = g_slist_append(input_list, g_variant_new_string(argv[1]));
    ret = dfl_call_class_method(obj_handle, (const gchar *)class_method, NULL, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    input_list = NULL;
    if (ret != DFL_OK) {
        g_printf("Get port statistic failed.\r\n");
        return FALSE;
    }

    stat_buf = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    g_printf("%s", stat_buf);

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;

    return TRUE;
}

gint32 ipmc_lsw_get_mesh_multiplepath(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    guint32 mpath_mode;
    GSList* output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar *class_name = CLASS_LSW_PUBLIC_ATTR_LOCAL_DP;
    gchar *class_method = METHOD_DATA_GET_MESH_MPATH;

    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t <mesh0> -d multiplepath\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth((const gchar *)class_name, 0, &obj_handle);
    if (ret != VOS_OK) {
        g_printf("get obj_handle fail, ret = %d\r\n", ret);
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, (const gchar *)class_method, NULL, NULL, &output_list);
    if (ret != VOS_OK) {
        g_printf("dfl_call_class_method, ret = %d\r\n", ret);
        return FALSE;
    }
    mpath_mode = g_variant_get_uint32((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;
    if ((mpath_mode != ENABLE) && (mpath_mode != DISABLE)) {
        g_printf("path mode(%u) is invalid!\r\n", mpath_mode);
        return FALSE;
    }
    g_printf("The current mode : %s\r\n", (mpath_mode == ENABLE) ? "enabled" : "disabled");

    return TRUE;
}

gint32 ipmc_lsw_get_mesh_itfportmode(guchar privilege, const gchar* rolepriv, guchar argc, gchar** argv, gchar* target)
{
    gint32 ret;
    guint8 port_mode;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar *port_desc[] = { "2*100GE", "8*25GE", "8*25GE + 2*100GE" };

    
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -t mesh -d itfportmode\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_LSW_NODE_MANAGE_DP, 0, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("get obj_handle fail, ret = %d\r\n", ret);
        return FALSE;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_LSW_GET_ITF_PORT_MODE, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        g_printf("dfl_call_class_method, ret = %d\r\n", ret);
        return FALSE;
    }

    port_mode = g_variant_get_byte((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    output_list = NULL;
    if ((port_mode == 0) || (port_mode >= PORT_MODE_MAX)) {
        g_printf("port mode(%u) is invalid!\r\n", port_mode);
        return FALSE;
    }
    g_printf("The current mode : %u (%s)\r\n", port_mode, port_desc[port_mode - 1]);

    return TRUE;
}
