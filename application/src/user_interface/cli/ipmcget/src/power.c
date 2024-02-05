

#include "ipmcget_power.h"

LOCAL guint8 get_max_fan_num(void)
{
    OBJ_HANDLE product_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_PRODUCT_COMPONENT, 0, &product_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get product handle failed, ret = %d", __FUNCTION__, ret);
        return 0;
    }

    guint8 fan_num = 0;
    ret = dal_get_property_value_byte(product_handle, PROPERTY_COMPOENT_FAN_NUM, &fan_num);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get fan num fail, ret = %d", __FUNCTION__, ret);
        return 0;
    }
    return fan_num;
}


LOCAL guint8 get_fan_curr_ctrl_flag(void)
{
    guint8 flag = TRUE;   
    GSList *output_list = NULL;
    OBJ_HANDLE cooling_handle = 0;
    
    gint32 ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != RET_OK) {
        return flag;
    }

    ret = dfl_call_class_method(cooling_handle, METHOD_COOLING_GET_FAN_CTRL_FLAG, NULL, NULL, &output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Call GetFanCtrlFlag method failed, ret %d", ret);
        return flag;
    }

    flag = g_variant_get_byte((GVariant*)g_slist_nth_data(output_list, 0));
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);

    return flag;
}


LOCAL gint32 get_fanlevel_by_active_board(GSList **output_list)
{
    gint32 ret;
    OBJ_HANDLE handle = 0;

    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Get object handle failed, ret %d", ret);
        return ret;
    }

    if (dal_board_comm_by_can()) {  //  使用CAN总线的产品通过CAN总线转发
        ret = dal_forward_method_to_active_by_can(handle,
            METHOD_COOLING_GET_MANUAL_LEVEL, NULL, NULL, output_list, 500);  
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get fanlevel by canbus remote method failed, ret %d", ret);
        }
    } else {
        ret = call_other_smm_method(dfl_get_object_name(handle), METHOD_COOLING_GET_MANUAL_LEVEL, NULL, NULL,
            output_list);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "Get fanlevel by smm remote method failed, ret %d", ret);
        }
    }

    return ret;
}

LOCAL gint32 print_manual_faninfo(void)
{
#define FAN_LEVEL_SEP_MASK  0x3

    GSList *output_list = NULL;
    gchar username[PARAMETER_LEN + 1] = {0};
    gchar ip[PARAMETER_LEN + 1] = {0};

    gint32 ret = get_username_ip(username, ip, sizeof(ip));
    if (ret != RET_OK) {
        (void)snprintf_s(username, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
        (void)snprintf_s(ip, PARAMETER_LEN + 1, PARAMETER_LEN, "%s", "unknown");
    }

    OBJ_HANDLE cooling_handle = 0;
    ret = dal_get_object_handle_nth(COOLINGCLASS, 0, &cooling_handle);
    if (ret != DFL_OK) {
        goto EXIT;
    }
    
    if (get_fan_curr_ctrl_flag() == FALSE) {
        ret = get_fanlevel_by_active_board(&output_list);
    } else {
        ret = uip_call_class_method_with_handle(OPERATOR_LOG_CLI, (const gchar *)username, (const gchar *)ip,
            COOLINGCLASS, cooling_handle, METHOD_COOLING_GET_MANUAL_LEVEL, NULL, &output_list);
    }

    if (ret != RET_OK || output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: call METHOD_COOLING_GET_MANUAL_LEVEL failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    gsize len = 0;
    const guint16 *data = (const guint16 *)g_variant_get_fixed_array(
        (GVariant *)g_slist_nth_data(output_list, 0), &len, sizeof(guint16));
    if (data == NULL || len == 0 || len > get_max_fan_num()) {
        uip_free_gvariant_list(output_list);
        goto EXIT;
    }

    g_printf("Manual fan level:");

    for (guint32 i = 0; i < len; i++) {
        gchar *sep = ((i & FAN_LEVEL_SEP_MASK) == 0) ? "\r\n" : ", ";
        g_printf("%sFan%u: %u", sep, (data[i] >> BYTE_BITS_NUM) & 0xff, data[i] & 0xff);
    }
    g_printf("\r\n");
    uip_free_gvariant_list(output_list);
    return RET_OK;
EXIT:
    g_printf("Get manual fan level failed.\r\n");
    return RET_ERR;
}


gint32 ipmc_get_faninfo(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar mode = 0;
    guchar ctrl_node;
    guint32 timeout = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_MODE);
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_TIMEOUT);
    ret = uip_multiget_object_property(COOLINGCLASS, COOLINGCLASS, property_name_list, &property_value);
    if (ret != RET_OK) {
        g_printf("Get fan mode and fan level failed.\r\n");
        g_slist_free(property_name_list);
        return FALSE;
    }
    
    mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    timeout = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 1));

    switch (mode) {
        case AUTO_FAN_MODE:
            g_printf("Current mode: auto\r\n");
            break;

        case MANUAL_FAN_MODE:
            if (is_control_by_other_node(&ctrl_node) == TRUE) {
                g_printf("Current mode: manual.\r\n");
                g_printf("The fan is set to manual mode by the node %d.\r\n", ctrl_node);
            } else {
                g_printf("Current mode: manual, timeout %u seconds.\r\n", timeout);
                ret = print_manual_faninfo();
            }
            break;

        default:
            g_printf("fan mode data invalid.\r\n");
            break;
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return (ret == RET_OK) ? TRUE : FALSE;
}


LOCAL void get_ps_slot(OBJ_HANDLE handle, guint8* slot_id)
{
    (void)dal_get_property_value_byte(handle, PROTERY_PS_SLOT_ID, slot_id);
    if (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) {
        *slot_id -= 1;
    }
}

LOCAL void ipmc_get_ps_workmode(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 PowerActiveStandbyEnable = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_POWER, &PowerActiveStandbyEnable);

        if (PowerActiveStandbyEnable == ENABLE) {
            get_ps_wokmode();
        }
    }
    return;
}


gint32 ipmc_get_psu_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_note = NULL;
    const gchar *input_mode_string = NULL;
    guint8 slot_id = 0;
    guint16 irate = 0;
    guint8 input_mode = 0;
    gchar ps_manufacturer[PS_MAX_LEN + 1] = {0};
    gchar ps_modelname[PS_MAX_LEN + 1] = {0};
    gchar ps_version[PS_MAX_LEN + 1] = {0};
    gchar ps_sn[PS_VERSION_LEN] = {0};
    guint8 visable = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_list(CLASS_NAME_PS, &obj_list);
    if (ret != DFL_OK || g_slist_last(obj_list) == 0) {
        g_printf("Get the PSU object list failed.\r\n");
        return FALSE;
    }

    
    obj_list = g_slist_sort(obj_list, dal_compare_psu_slot_id);
    
    g_printf("Current PSU Information : \n");
    
    g_printf("%-5s  %-15s  %-20s  %-30s  %-32s  %-12s  %-8s\r\n", "Slot", "Manufacturer", "Type", "SN", "Version",
        "Rated Power", "InputMode");
    

    for (obj_note = obj_list; obj_note; obj_note = obj_note->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROTERY_PS_VISABLE, &visable);
        if (visable != ENABLE) {
            continue;
        }

        get_ps_slot((OBJ_HANDLE)obj_note->data, &slot_id);
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROTERY_PS_MANUFACTURER, ps_manufacturer,
            sizeof(ps_manufacturer));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROTERY_PS_MODEL, ps_modelname,
            sizeof(ps_modelname));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROTERY_PS_SN, ps_sn, sizeof(ps_sn));
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_note->data, PROTERY_PS_VERSION, ps_version,
            sizeof(ps_version));
        (void)dal_get_property_value_uint16((OBJ_HANDLE)obj_note->data, PROTERY_PS_RATING, &irate);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROTERY_PS_INPUTMODE, &input_mode);

        if (input_mode == PROPERTY_AC_MODE) {
            input_mode_string = STRING_AC_MODE;
        } else if (input_mode == PROPERTY_DC_MODE) {
            input_mode_string = STRING_DC_MODE;
        }
        
        else if (input_mode == PROPERTY_ACDC_MODE) {
            input_mode_string = STRING_ACDC_MODE;
        }
        
            input_mode_string = STRING_NA_MODE;
        }

        
        dal_trim_string(ps_manufacturer, PS_MAX_LEN + 1);
        dal_trim_string(ps_modelname, PS_MAX_LEN + 1);
        dal_trim_string(ps_sn, PS_VERSION_LEN);
        dal_trim_string(ps_version, PS_MAX_LEN + 1);
        
        g_printf("%-5d  %-15s  %-20s  %-30s  %-32s  %-12d  %-8s\r\n", slot_id, ps_manufacturer, ps_modelname, ps_sn,
            ps_version, irate, input_mode_string);
        
        
    }

    g_slist_free(obj_list);

    ipmc_get_ps_workmode();

    return TRUE;
}


gint32 ipmc_get_powerstate(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guint8 power_state;
    guint8 hotswap_state;
    GVariant *value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Usage: ipmcget -d powerstate\r\n");
        return FALSE;
    }

    ret = uip_get_object_property(CLASS_CHASSISPAYLOAD, OBJECT_CHASSISPAYLOAD, PROPERTY_PAYLOAD_CHASSPWR_STATE, &value);
    if (ret != RET_OK) {
        g_printf("Get power state failed.\r\n ");
        return FALSE;
    }

    power_state = g_variant_get_byte(value);
    g_printf("Power state   : %s\r\n", power_state ? "On" : "Off");
    g_variant_unref(value);

    value = NULL;
    ret = uip_get_object_property(CLASS_HOTSWAP, OBJECT_HOTSWAP, PROPERTY_HS_CRU_STATE, &value);
    if (ret != RET_OK) {
        g_printf("Get hot swap state failed.\r\n ");
        return FALSE;
    }

    hotswap_state = g_variant_get_byte(value);
    g_printf("Hotswap state : M%d\r\n", hotswap_state);
    g_variant_unref(value);
    return TRUE;
}


gint32 ipmc_get_poweronpermit_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar autopoweron;
    const gchar *ip = NULL;
    guint32 port;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    OBJ_HANDLE obj_handle;
    GSList *obj_list = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

     
    ret = dfl_get_object_list(CLASS_STATELESS, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Get object failed.\n");
        return RET_ERR;
    }

    
    obj_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
    g_slist_free(obj_list);

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_AUTO_POWER_ON);
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_SYS_MANAGER_IP);
    property_name_list = g_slist_append(property_name_list, PROPERTY_STATELESS_SYS_MANAGER_PORT);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get poweronpermit information failed.\n");
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    autopoweron = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    ip = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), 0);
    port = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 2));

    g_printf("State         : %s\n", (autopoweron == FALSE) ? "enabled" : "disabled");
    g_printf("ManagerIP     : %s\n", ip);
    g_printf("ManagerPort   : %d\n", port);

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);

    return TRUE;
}


gint32 ipmc_get_pwroff_policy_after_leakage(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    guint8 pwroff_policy = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    if (ret != DFL_OK) {
        g_printf("Get power-off strategy after leakage failed.\r\n");
        return FALSE;
    }
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_COOLING_LEAK_POLICY, &pwroff_policy);
    if (pwroff_policy == KEEP_PWR_STATUS_AFTER_LEAKAGE) {
        g_printf("Manual power-off after leakage.\r\n");
    } else if (pwroff_policy == POWER_OFF_AFTER_LEAKAGE) {
        g_printf("Auto power-off after leakage.\r\n");
    }

    return TRUE;
}


gint32 ipmc_get_powercapping_info(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    OBJ_HANDLE shelf_handle = 0;
    OBJ_HANDLE mspp_handle = 0;
    guint8 mode = 0;
    guint8 enable = 0;
    guint8 threshold = 0;
    guint8 active_status = 0;
    guint16 value = 0;
    guint16 current_power = 0;
    guint8 MSPPEnable_state = 0;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc) {
        g_printf("Usage: ipmcget %s-t powercapping -d info\r\n", need_l_shelf);
        return FALSE;
    }

    
    
    ret = dal_get_object_handle_nth(CLASS_NAME_SHELF_POWER_CAPPING, 0, &shelf_handle);
    if (ret != RET_OK) {
        g_printf("Get power capping info failed.\r\n ");
        return FALSE;
    }
    (void)dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_MODE, &mode);
    (void)dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_ENABLE, &enable);
    (void)dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_THRESHOLD, &threshold);
    (void)dal_get_property_value_uint16(shelf_handle, PROTERY_SHELF_POWER_CAPPING_VALUE, &value);
    
    (void)dal_get_property_value_uint16(shelf_handle, PROTERY_SHELF_POWER_CAPPING_CURRENT_POWER, &current_power);
    
    (void)dal_get_property_value_byte(shelf_handle, PROTERY_SHELF_POWER_CAPPING_ACTIVE_STATE, &active_status);

    g_printf("Shelf Power Capping Info:\r\n");
    g_printf("   Mode           : %s\r\n", (mode == 0) ? "Equal" : ((mode == 1) ? "Manual" : "Proportion"));
    g_printf("   Enable         : %s\r\n", (enable) ? "Enabled" : "Disabled");
    g_printf("   Value          : %dW\r\n", value);
    g_printf("   Threshold      : %d%%\r\n", threshold);
    g_printf("   Current Power  : %dW\r\n", current_power);
    if (enable == 1) {
        g_printf("   Active Status  : %s\r\n", (active_status) ? "Enabled" : "Disabled");
    }

    
    // 如果MSPP类存在，则显示MSPP的使能状态
    ret = dal_get_object_handle_nth(CLASS_NAME_MSPP, 0, &mspp_handle);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(mspp_handle, PROTERY_MSPP_ENABLE, &MSPPEnable_state);
        g_printf("   MSPP           : %s\r\n", (MSPPEnable_state) ? "Enabled" : "Disabled");
    }

    

    
    ret = get_blade_power_capping_info();
    if (ret != RET_OK) {
        g_printf("Get blade power capping info failed.\r\n ");
        return FALSE;
    }

    return TRUE;
}

gint32 ipmc_get_fan_smartmode(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
    gint32 ret;
    guchar mode = 0;
    guint32 smartmode = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;
    gchar* smartmode_dsp[4] = { "Energy saving", "Low noise", "High performance", "Custom" };

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_MODE);
    property_name_list = g_slist_append(property_name_list, COOLING_THERMALLEVEL);
    ret = uip_multiget_object_property(COOLINGCLASS, COOLINGCLASS, property_name_list, &property_value);
    if (ret != RET_OK) {
        g_printf("Get fan mode and smart mode failed.\r\n");
        g_slist_free(property_name_list);
        return FALSE;
    } else {
        
        mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        smartmode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));

        switch (mode) {
            case MANUAL_FAN_MODE:
                g_printf("Current fan control mode: Manual\r\n");
                break;

            case AUTO_FAN_MODE:
                g_printf("Current mode: %s\r\n", smartmode_dsp[smartmode & 0x3]);
                break;

            default:
                g_printf("fan mode data invalid.\r\n");
                break;
        }
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return TRUE;
}


gint32 ipmc_get_psu_supply_source(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv, gchar *target)
{
#define MAX_LEN 30
#define MAX_PSU_NUM 24
    gint32 ret;
    gchar ret_str[MAX_LEN * MAX_PSU_NUM] = {0};

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }
    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    ret = get_psu_supply_source(ret_str, MAX_LEN * MAX_PSU_NUM);
    if (ret != RET_OK) {
        g_printf("Failed to get PSU power source.\r\n");
        return FALSE;
    }

    g_printf("************************************** \r\n");
    g_printf("0 indicates the input A power supply.  \r\n");
    g_printf("1 indicates the input B power supply.  \r\n");
    g_printf("************************************** \r\n");
    g_printf("The PSU power source:\n");
    g_printf("%s \r\n", ret_str);

    return TRUE;
}


gint32 ipmc_get_gracefulshutdowntimeout(guchar privilege, const gchar *rolepriv, guchar argc, gchar **argv,
    gchar *target)
{
    gint32 ret;
    guint32 timeout;
    guint8 timeout_state;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    if (argc != 0) {
        g_printf("Too many parameters.\r\n");
        return FALSE;
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_PAYLOAD_PWROFF_TM_EN);
    property_name_list = g_slist_append(property_name_list, PROPERTY_PAYLOAD_PWROFF_TM);

    ret =
        uip_multiget_object_property(CLASS_CHASSISPAYLOAD, OBJECT_CHASSISPAYLOAD, property_name_list, &property_value);
    g_slist_free(property_name_list);
    if (ret != RET_OK) {
        g_printf("Get shutdown timeout failed.\r\n");
        return FALSE;
    }

    timeout_state = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 0));
    timeout = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 1));
    g_printf("Graceful shutdown timeout state :    %s\r\n", timeout_state ? "enabled" : "disabled");

    if (timeout_state != 0) {
        g_printf("Graceful shutdown timeout value :    %d s\r\n", timeout);
    }

    uip_free_gvariant_list(property_value);
    return TRUE;
}
