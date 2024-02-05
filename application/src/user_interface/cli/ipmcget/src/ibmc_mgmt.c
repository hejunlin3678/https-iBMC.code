

#include "ipmcget_ibmc_mgmt.h"
#include "ipmcget_sys_mgmt.h"


gint32 ipmc_get_rollbackstatus(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar status = 0;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d rollbackstatus\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_NAME_UPGRADE, OBJ_NAME_UPGRADE, PROTERY_ROLL_STATE, &value);
    if (ret != RET_OK) {
        g_printf("Get rollback status failed.\r\n");
        return FALSE;
    } else {
        status = g_variant_get_byte(value);
        
        
        
        switch (status) {
            case COMP_CODE_HPM_ROLL_INPOSSIABLE:
                g_printf("Haven't performed rollback!\r\n");
                break;

            case COMP_CODE_HPM_ROLL_FAILURE:
                g_printf("Last rollback failed!\r\n");
                break;

            case COMP_CODE_HPM_ROLL_SUCCESS:
                g_printf("Last rollback success!\r\n");
                break;

            case COMP_CODE_HPM_ROLL_IN_PROGRESS:
                g_printf("Rollback is in progress!\r\n");
                break;

            default:
                g_printf("Rollback status invalid!\r\n");
                g_variant_unref(value);
                return FALSE;
        }
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_locationID(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    const gchar *locationid = NULL;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d locationid\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_NAME_BOARD, OBJ_NAME_BOARD, PROPERTY_BOARD_LOCATIONID, &value);
    if (ret != RET_OK) {
        g_printf("Get Location ID failed.\r\n ");
        return FALSE;
    } else {
        locationid = g_variant_get_string(value, 0);
        g_printf("Location ID is:%s\r\n", locationid);
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_chassis_number(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint32 chassis_num;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget %s-d chassisnum\r\n", help_default_name);
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_NAME_PRODUCT, OBJ_NAME_PRODUCT, PROPERTY_PRODUCT_CHASSISNUM, &value);
    if (ret != RET_OK) {
        g_printf("Get chassis number failed.\r\n ");
        return FALSE;
    }

    chassis_num = g_variant_get_uint32(value);
    g_printf("Chassis Number : %d\r\n", chassis_num);
    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_bbu_module_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -d bbuinfo\r\n");
        return RET_ERR;
    }

    ret = dfl_foreach_object(CLASS_BBU_MODULE_NAME, print_bbu_module_info, NULL, NULL);
    if (ret != DFL_OK) {
        return ret;
    }

    return RET_OK;
}


gint32 ipmc_get_ascend_work_mode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -d npuworkmode\r\n");
        return RET_ERR;
    }

    (void)dfl_foreach_object(CLASS_NPU, ascend_mode_info, NULL, NULL);

    return RET_OK;
}


gint32 ipmc_get_usbmgmt_info(unsigned char privilege, const gchar *rolepriv, unsigned char argc, char **argv,
    char *target)
{
    OBJ_HANDLE obj_handle;
    gint32 ret;
    guchar is_board_present;
    guchar is_enable;
    guchar device_status;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Usage: ipmcget -t usbmgmt -d info\r\n");
        return FALSE;
    }

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Get USB management information failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_PRESENCE, &is_board_present);
    if (ret != RET_OK) {
        g_printf("Get USB management information failed.\r\n");
        return FALSE;
    }
    if (is_board_present == FALSE) {
        g_printf("The mounting ear with %s directly connected management interface is not present.\r\n", custom_name);
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_ENABLE, &is_enable);
    if (ret != RET_OK) {
        g_printf("Get USB management information failed.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USB_MGMT_DEVICE_STATUS, &device_status);
    if (ret != RET_OK) {
        g_printf("Get USB management information failed.\r\n");
        return FALSE;
    }
    g_printf("Service State         : %s\r\n", is_enable == TRUE ? "enabled" : "disabled");
    g_printf("USB Device Presence   : %s\r\n", device_status == 0 ? "absent" : "present");

    return TRUE;
}


gint32 ipmc_get_otm_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 max_port_num = 0;
    guint32 port_id;
    guint8 is_local_flag = 1;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }

    gint32 ret = get_port_adapter_max_num_and_flg(&is_local_flag, &max_port_num);
    if (ret != RET_OK) {
        g_printf("Get OTM information failed.\r\n");
        return RET_ERR;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t otm -d info -v <Option>\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall - Show all OTM devices.\r\n");
        if (max_port_num >= 1) {
            g_printf("\tID - Show only one port OTM device which matches with this ID (0 - %d).\r\n", max_port_num - 1);
        } else {
            g_printf("\tID - Show only one port OTM device which matches with this ID (0 - %d).\r\n", max_port_num);
        }
        return RET_ERR;
    }

    if (!strcmp(argv[1], "all")) {
        for (port_id = 0; port_id < max_port_num; port_id++) {
            print_otm_manufacture_info(is_local_flag, port_id);
            g_printf("\r\n");
        }
    } else {
        if (str_to_int(argv[1]) == IPMC_ERROR) {
            g_printf("Invalid OTM Port ID.\r\n");
            return RET_ERR;
        }

        port_id = (guint32)str_to_int(argv[1]);
        if (port_id >= max_port_num) {
            g_printf("Invalid OTM Port ID.\r\n");
            return RET_ERR;
        }
        print_otm_manufacture_info(is_local_flag, port_id);
    }
    return RET_OK;
}


gint32 ipmc_get_otm_status(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 max_port_num = 0;
    guint32 port_id;
    guint8 is_local_flag = 1;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return RET_ERR;
    }

    gint32 ret = get_port_adapter_max_num_and_flg(&is_local_flag, &max_port_num);
    if (ret != RET_OK) {
        g_printf("Get OTM information failed.\r\n");
        return RET_ERR;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t otm -d status -v <Option>\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall - Show all OTM devices.\r\n");
        if (max_port_num >= 1) {
            g_printf("\tID - Show only one port OTM device which matches with this ID (0 - %d).\r\n", max_port_num - 1);
        } else {
            g_printf("\tID - Show only one port OTM device which matches with this ID (0 - %d).\r\n", max_port_num); 
        }
        return RET_ERR;
    }

    if (!strcmp(argv[1], "all")) {
        for (port_id = 0; port_id < max_port_num; port_id++) {
            print_otm_hardware_status(is_local_flag, port_id);
            g_printf("\r\n");
        }
    } else {
        if (str_to_int(argv[1]) == IPMC_ERROR) {
            g_printf("Invalid OTM Port ID.\r\n");
            return RET_ERR;
        }
        port_id = (guint32)str_to_int(argv[1]);
        if (port_id >= max_port_num) {
            g_printf("Invalid OTM Port ID.\r\n");
            return RET_ERR;
        }
        print_otm_hardware_status(is_local_flag, port_id);
    }

    return RET_OK;
}


gint32 ipmc_get_rack_asset_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    guint32 ret;
    OBJ_HANDLE obj_rack_asset;
    guint8 asset_mgmt_status = ASSET_MGMT_SVC_OFF;
    guint8 u_count;
    guint8 u_count_used;
    guint8 direction;
    guint32 rated_power;
    guint32 load_capacity;
    guint32 depth;
    guint32 width;
    guint32 height;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset);
    if (ret != RET_OK) {
        g_printf("Rack level asset management not supported.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_rack_asset, PROPERTY_RACK_ASSET_MGMT_SVC_STATUS, &asset_mgmt_status);
    if (ret != RET_OK) {
        g_printf("Failed to query asset management service status.\r\n");
        return FALSE;
    }

    
    if (asset_mgmt_status != ASSET_MGMT_SVC_ON) {
        g_printf("Asset management service not ready.\r\n");
        return FALSE;
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_U_COUNT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_U_COUNT_USED);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_DIRECTION);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_RATED_POWER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_LOAD_CAPACITY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_DEPTH_MM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_WIDTH_MM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_HEIGHT_MM);
    ret = dfl_multiget_property_value(obj_rack_asset, property_name_list, &property_value_list);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return FALSE;
    }

    u_count = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
    u_count_used = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));
    direction = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 2));
    rated_power = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 3));
    load_capacity = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 4));
    depth = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 5));
    width = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 6));
    height = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 7));

    g_printf("%s\n", "Rack Level Asset Management Information");
    g_printf("----------------------------------------------------------------------\n");
    g_printf("%-40s : %d\n", "Unit Number Total", u_count);
    g_printf("%-40s : %d\n", "Unit Number Used", u_count_used);
    g_printf("%-40s : %s\n", "Unit Number Direction", (direction == 0) ? "From Bottom Up" : "From Top Down");
    if (rated_power != 0) {
        g_printf("%-40s : %d\n", "Rated Power(Watts)", rated_power);
    }
    if (load_capacity != 0) {
        g_printf("%-40s : %d\n", "Load Capacity(kg)", load_capacity);
    }
    if (depth != 0) {
        g_printf("%-40s : %d\n", "Depth(mm)", depth);
    }
    if (width != 0) {
        g_printf("%-40s : %d\n", "Width(mm)", width);
    }
    if (height != 0) {
        g_printf("%-40s : %d\n", "Height(mm)", height);
    }
    g_printf("----------------------------------------------------------------------\n");

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value_list);
    property_name_list = NULL;
    property_value_list = NULL;

    
    ret = print_rack_location_info(obj_rack_asset);
    return ret;
}


gint32 ipmc_get_unit_asset_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_rack_asset;
    guint8 asset_mgmt_status = ASSET_MGMT_SVC_OFF;
    gint32 unit_num = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    
    if ((argc != 2) || (strcmp(argv[0], "-v"))) {
        g_printf("Usage: ipmcget -t asset -d unitinfo -v <Option>\r\n");
        g_printf("Option:\r\n");
        g_printf("\tall  - Show all unit info in this Rack Management Controller.\r\n");
        g_printf("\tnumber  - Show specific unit info which matches with this unit number.\r\n");
        return FALSE;
    }

    ret = dal_get_object_handle_nth(CLASS_RACK_ASSET_MGMT, 0, &obj_rack_asset);
    if (ret != RET_OK) {
        g_printf("Rack level asset management not supported.\r\n");
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_rack_asset, PROPERTY_RACK_ASSET_MGMT_SVC_STATUS, &asset_mgmt_status);
    if (ret != RET_OK) {
        g_printf("Failed to query asset management service status.\r\n");
        return FALSE;
    }

    
    if (asset_mgmt_status != ASSET_MGMT_SVC_ON) {
        g_printf("Asset management service not ready.\r\n");
        return FALSE;
    }

    if (!strcmp(argv[1], "all")) {
        process_unit_info(PARAM_DEFAULT_VALUE);
    } else {
        unit_num = str_to_int(argv[1]);
        if (unit_num <= 0) {
            g_printf("Invalid unit number\r\n");
            return FALSE;
        }

        process_unit_info(unit_num);
    }

    return TRUE;
}


gint32 ipmc_get_system_time(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret = RET_OK;
    struct tm local_time = { 0 };
    gint16 time_offset = 0;
    gchar *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    gchar time_zone[64] = {0};

    memset_s(&local_time, sizeof(local_time), 0, sizeof(local_time));

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }
    if (ipmc_get_timeoffset(&time_offset) != RET_OK) {
        g_printf("Get system time failed.\r\n");
        return FALSE;
    }
    if (ipmc_get_timezone(time_zone, sizeof(time_zone)) != RET_OK) {
        g_printf("Get system time failed.\r\n");
        return FALSE;
    }

    time_t cur_time = time(0);
    if (cur_time == 0) {
        g_printf("Get system time failed.\r\n ");
        return FALSE;
    } else {
        
        ret = dal_localtime_r((time_t *)&cur_time, &local_time); 
        if (ret != RET_OK) {
            g_printf("Get system time failed.\r\n ");
            return FALSE;
        }
        g_printf("%04d-%02d-%02d %s %02d:%02d:%02d ", (1900 + local_time.tm_year), (1 + local_time.tm_mon),
            local_time.tm_mday, wday[local_time.tm_wday], local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
    }

    if (strncasecmp(time_zone, TIME_ZONE_UTC, strlen(TIME_ZONE_UTC)) == 0 ||
        strncasecmp(time_zone, TIME_ZONE_GMT, strlen(TIME_ZONE_GMT)) == 0) {
        g_printf("%s \r\n", time_zone);
    } else {
        g_printf("%s", time_zone);
        print_time_offset(time_offset);
    }

    return TRUE;
}
