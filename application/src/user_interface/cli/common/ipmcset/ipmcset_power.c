

#include "ipmcset_power.h"

#define DEFAULT_PS_NUM 2
#define OP_POWER_OFF 0
#define OP_POWER_ON 1
#define OP_FORCED_POWER_OFF 2


gint32 setcommand_privilege_judge(guchar privilege, const gchar *rolepriv)
{
    if (ipmc_privilege_judge(privilege, rolepriv) == FALSE) {
        return FALSE;
    }

    return TRUE;
}


gint32 check_fan_control_support(void)
{
    guint8 node_mode = 0;
    guint8 ctrl_node;
    gchar custom_name[LOG_SPECIAL_STRING_LEN_MAX] = {"iBMC"};

    
    if (dal_get_custom_string_name(custom_name, BMC_CUSTOM_LOGIBMC_NAME, sizeof(custom_name)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_custom_string_name fail.", __FUNCTION__);
    }

    if (dal_get_func_ability(CLASS_PARTITION, NODE_MODE, &node_mode) == RET_OK) {
        if (node_mode != 1) {
            g_printf("Perform the operation on the master(HFC-2) %s.\r\n", custom_name);
            return RET_ERR;
        }
    }

    if (is_control_by_other_node(&ctrl_node) == TRUE) {
        g_printf("The fan is set to manual mode by the node %d.\r\n", ctrl_node);
        return RET_ERR;
    }

    return RET_OK;
}


void get_fan_slot_range(guint8 *min_fan_slot, guint8 *max_fan_slot)
{
    gint32 ret;
    guint8 cur_fan_slot = 0;
    GSList *fan_obj_list = NULL;
    GSList *fan_obj_node = NULL;

    ret = dfl_get_object_list(CLASS_FANCLSEE, &fan_obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get fan obj list failed. ret=%d", __FUNCTION__, ret);
        return;
    }

    *max_fan_slot = 0;
    *min_fan_slot = INVALID_DATA_BYTE;
    for (fan_obj_node = fan_obj_list; fan_obj_node; fan_obj_node = fan_obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)fan_obj_node->data, FAN_SLOT, &cur_fan_slot);
        *min_fan_slot = cur_fan_slot < *min_fan_slot ? cur_fan_slot : *min_fan_slot;
        *max_fan_slot = cur_fan_slot > *max_fan_slot ? cur_fan_slot : *max_fan_slot;
    }
    g_slist_free(fan_obj_list);

    return;
}


gint32 get_fanmode_and_fanlevel(guint8 fanid, guint8 fan_level)
{
    gint32 ret;
    gint32 mode = 0;
    guint32 timeout = 0;
    guchar level = 0;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_MODE);
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_TIMEOUT);
    property_name_list = g_slist_append(property_name_list, COOLING_PROPERTY_LEVEL);
    ret = uip_multiget_object_property(COOLINGCLASS, COOLINGCLASS, property_name_list, &property_value);
    if (ret != RET_OK) {
        g_printf("Get fan mode and level failed.\r\n");
        g_slist_free(property_name_list);
        return FALSE;
    } else {
        
        mode = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
        timeout = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 1));
        level = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 2));

        switch (mode) {
            case AUTO_FAN_MODE:
                g_printf("Current Mode           :   Auto \r\n");
                break;

            case MANUAL_FAN_MODE:
                g_printf("Current Mode           :   manual, timeout %u seconds.\r\n", timeout);
                break;

            default:
                break;
        }

        if (level == 0xfe) {
            g_printf("Fan has been placed in \"Emergency Shut Down\" by the Shelf Manager\n");
        } else if (level <= 0x64) {
            if (fanid == 0xff) {
            
                g_printf("Global Manual Fan Level:   %d%%\r\n", fan_level);
            }
        }
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return TRUE;
}


gint32 get_maximum_blade_power_capping_value(OBJ_HANDLE current_handle, guint16 *remaind_value)
{
    gint32 result;
    OBJ_HANDLE object_handle;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    guint8 presence = 0;
    guint8 state = 0;
    guint16 extra_value = 0;
    guint16 blade_cap_max = 0;
    guint16 blade_value = 0;

    guint16 blade_current_power = 0;
    guint16 blade_current_power_total = 0;

    guint16 base_value = 0;
    guchar MgmtSoftWareType = 0;
    guint32 ps_total_power = 0;
    guint16 shelf_capping_value = 0;                 
    guint16 blade_capping_value_setted_manually = 0; 
    OBJ_HANDLE syspower_object_handle = 0;

    (void)dal_get_software_type(&MgmtSoftWareType);

    if (remaind_value == NULL) {
        return RET_ERR;
    }

    
    result = dfl_get_object_list(CLASS_NAME_SHELF_POWER_CAPPING, &handle_list);
    if (result != DFL_OK) {
        return result;
    }

    object_handle = (OBJ_HANDLE)handle_list->data;
    g_slist_free(handle_list);
    handle_list = NULL;

    
    if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
        
        property_name_list = g_slist_append(property_name_list, PROTERY_SHELF_POWER_CAPPING_VALUE);
        property_name_list = g_slist_append(property_name_list, PROTERY_SHELF_POWER_CAPPING_MAX_EXTRA_VALUE);
        result = dfl_multiget_property_value(object_handle, property_name_list, &property_value_list);
        if (result != DFL_OK) {
            g_slist_free(property_name_list);
            return result;
        }

        *remaind_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 0));
        extra_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 1));

        g_slist_free(property_name_list);
        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
        property_value_list = NULL;

        
        if (*remaind_value <= extra_value) {
            return RET_ERR;
        }

        *remaind_value = *remaind_value - extra_value;
    } else {
        (void)dal_get_property_value_uint16(object_handle, PROTERY_SHELF_POWER_CAPPING_VALUE, &shelf_capping_value);
    }
    

    
    result = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &handle_list);
    if (result != DFL_OK) {
        return result;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        
        if (is_compute_node(object_handle) == FALSE) {
            continue;
        }
        
        
        presence = 0;
        state = 0;
        blade_value = 0;
        base_value = 0;

        
        (void)dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
        
        (void)dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_POWER_CAPPING_MANUAL_STATE, &state);
        (void)dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_VALUE, &blade_value);
        (void)dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE, &base_value);
        
        (void)dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_CURRENT_POWER,
            &blade_current_power);
        

        

        
        if (presence == BLADE_PRESENCE) {
            blade_current_power_total += blade_current_power;
            
            if (object_handle == current_handle) {
                continue;
            }

            if (state == SHELF_PWRCAP_STATUS_ENABLED) {
                
                if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
                    if (*remaind_value < blade_value) {
                        result = RET_ERR;
                        break;
                    }

                    *remaind_value -= blade_value;
                } else {
                    blade_capping_value_setted_manually += blade_value;
                }
                
            } else {
                if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
                    if (*remaind_value < base_value) {
                        result = RET_ERR;
                        break;
                    }
                    *remaind_value -= base_value;
                }
            }
        }
    }

    
    if (MgmtSoftWareType == MGMT_SOFTWARE_TYPE_EM) {
        
        (void)dfl_get_object_handle(OBJ_NAME_AMMETER, &syspower_object_handle);
        dal_get_property_value_uint32(syspower_object_handle, PROTERY_PSTOTAL_POWER, &ps_total_power);

        extra_value = ps_total_power - blade_current_power_total;
        if (shelf_capping_value < extra_value + blade_capping_value_setted_manually) {
            
            *remaind_value |= 0x8000;
            
            result = RET_ERR;
        } else {
            *remaind_value = shelf_capping_value - (extra_value + blade_capping_value_setted_manually);
            result = RET_OK;
        }
    }
    

    g_slist_free(handle_list);

    if (result == RET_OK) {
        if (dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_MAX_CAP_POWER, &blade_cap_max) ==
            RET_OK) {
            *remaind_value = *remaind_value > blade_cap_max ? blade_cap_max : *remaind_value;
        }
    }

    return result;
}


gint32 is_continue_operate(void)
{
    gint32 input;

    g_printf("WARNING: The operation may have many adverse effects.\r\n");
    input = check_the_input_operation(CLI_COMFIRM_STRING);
    return input;
}


gint32 get_minimum_power_capping_value(guint16 *demand_value, guint16 *minimum_value)
{
    gint32 result;
    OBJ_HANDLE object_handle = 0;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    GVariant *property_value = NULL;
    guint8 presence = 0;
    guint8 state = 0;
    guint16 blade_value = 0;
    guint16 base_value = 0;
    guint16 current_power = 0;
    guint16 total_current_power = 0;
    guint16 extra_value = 0;

    guchar MgmtSoftWareType = 0;
    OBJ_HANDLE syspower_object_handle = 0;
    guint32 ps_total_power = 0;

    (void)dal_get_software_type(&MgmtSoftWareType);
    if (demand_value == NULL) {
        return RET_ERR;
    }

    
    result = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &handle_list);
    if (result != DFL_OK) {
        return result;
    }

    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        object_handle = (OBJ_HANDLE)list_item->data;
        
        if (is_compute_node(object_handle) == FALSE) {
            continue;
        }
        
        property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_MANUAL_STATE);
        property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_VALUE);
        property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE);
        result = dfl_multiget_property_value(object_handle, property_name_list, &property_value_list);
        if (result != DFL_OK) {
            g_slist_free(property_name_list);
            property_name_list = NULL;
            continue;
        }

        state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
        blade_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 1));
        base_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        g_slist_free(property_name_list);
        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        property_name_list = NULL;
        property_value_list = NULL;

        
        (void)dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
        
        
        if (presence == BLADE_PRESENCE) {
            if (state == SHELF_PWRCAP_STATUS_ENABLED) {
                if (blade_value < USHRT_MAX - *demand_value) {
                    *demand_value += blade_value;
                    *minimum_value += blade_value;
                } else {
                    g_slist_free(handle_list);
                    return RET_ERR;
                }
            } else {
                if (base_value < USHRT_MAX - *demand_value) {
                    *demand_value += base_value;
                } else {
                    g_slist_free(handle_list);
                    return RET_ERR;
                }
            }

            
            (void)dal_get_property_value_uint16(object_handle, PROTERY_IPMBETH_POWER_CAPPING_CURRENT_POWER,
                &current_power);

            total_current_power += current_power;
            
        }
    }

    g_slist_free(handle_list);
    handle_list = NULL;

    
    if (MgmtSoftWareType != MGMT_SOFTWARE_TYPE_EM) {
        
        result = dfl_get_object_list(CLASS_NAME_SHELF_POWER_CAPPING, &handle_list);
        if (result != DFL_OK) {
            return result;
        }

        object_handle = (OBJ_HANDLE)handle_list->data;
        g_slist_free(handle_list);

        result = dfl_get_property_value(object_handle, PROTERY_SHELF_POWER_CAPPING_MAX_EXTRA_VALUE, &property_value);
        if (result != DFL_OK) {
            return result;
        }

        extra_value = g_variant_get_uint16(property_value);
        g_variant_unref(property_value);

        if (extra_value > USHRT_MAX - *demand_value) {
            return RET_ERR;
        }
    } else {
        
        (void)dfl_get_object_handle(OBJ_NAME_AMMETER, &syspower_object_handle);
        dal_get_property_value_uint32(syspower_object_handle, PROTERY_PSTOTAL_POWER, &ps_total_power);

        if (ps_total_power > total_current_power) {
            extra_value = ps_total_power - total_current_power;
        } else {
            return RET_ERR;
        }
    }
    

    *demand_value += extra_value;
    *minimum_value += extra_value;

    return RET_OK;
}


gint32 get_component_ps_max_num(guint8 *max_ps_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(OBJ_PRODUCT_COMPONENT, 0, &obj_handle);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, max_ps_num);
    } else {
        *max_ps_num = DEFAULT_PS_NUM;
    }

    return RET_OK;
}


guint8 get_powercontrol_normal_and_redundancy(void)
{
    gint32 ret;
    guint8 uint8_value = 0;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj_list for CLASS_NAME_AMMETER fail", __FUNCTION__);
        return uint8_value;
    }

    ret = dal_get_property_value_byte(obj_handle, PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE, &uint8_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get PROTERY_SYSPOWER_NORMAL_AND_REDUNDANCY_ENABLE fail", __FUNCTION__);
        return uint8_value;
    }
    return uint8_value;
}

void print_set_psu_workmode_errinfo(gint32 ret)
{
    if (ret == ERR_ACTIVE_PS_ONT_PST) {
        g_printf("At least one active PSU is not detected.\r\n");
    } else if (ret == ERR_NO_STANDBY_PS_PST) {
        g_printf("No standby PSU is detected.\r\n");
    } else if (ret == ERR_ACTIVE_PS_ONT_HLS) {
        g_printf("The active or standby PSU is unhealthy.\r\n");
    } else if (ret == ERR_CURRENT_POWER_HIGN) {
        g_printf("The current power exceeds 75%% of the power ratings of all active PSUs.\r\n");
    } else if (ret == ERR_ACTIVE_PS_NUM_NE) {
        g_printf("Active PSUs are insufficient.\r\n");
    } else if (ret == ERR_PS_NOT_SUPPORT) {
        g_printf("At least one PSU does not support.\r\n");
    } else if (ret == ERR_ACTIVE_STANDBY_MODE_UNSUPPORT) {
        g_printf("The configuration dose not support.\r\n");
    } else {
        g_printf("Unknown error.\r\n");
        debug_log(DLOG_ERROR, "Unknown error %d.", ret);
    }

    return;
}


gint32 ipmc_set_pcinfo_value_check_value(const gint32 value, const guint16 actual_min_value, const guint16 min_value,
    const guint16 max_value)
{
    
    if (value < actual_min_value) {
        g_printf("Set shelf power capping value failed. The value is lower than the minimum value allowed: (%d).\r\n",
            actual_min_value);
        return FALSE;
    }
    

    
    if (value > max_value) {
        g_printf("Set shelf power capping value failed. Reference range from [%d, %d].\r\n", min_value, max_value);
        return FALSE;
    } else if (value < min_value) {
        g_printf("Reference range from [%d, %d].\r\n", min_value, max_value);
        g_printf(
            "The value for Shelf Power Cap is lower than the lower limit of the range, power capping may fail.\r\n");
    } else {
        g_printf("The value for Shelf Power Cap is in the range of %d-%d.\r\n", min_value, max_value);
        g_printf("The lower limit for Shelf Power Cap is the minimum value allowed. However, if the servers are "
            "equipped with high-power PCIe devices such as GPUs and SSDs and Shelf Power Cap is set to a value "
            "close to the lower limit, power capping may fail.\r\n");
    }
    return TRUE;
    
}

LOCAL void print_set_powerstate_warning_info(gint32 operate)
{
    switch (operate) {
        case OP_POWER_OFF:
            g_printf("WARNING: The operation may have many adverse effects.\r\n");
            break;
        case OP_POWER_ON:
            g_printf("WARNING: The operation may have many adverse effects.\r\n");
            break;
        case OP_FORCED_POWER_OFF:
            g_printf("WARNING: A forced power-off may damage programs or unsaved data of the server.\r\n");
            break;
        default:
            debug_log(DLOG_ERROR, "Unknown operate %d.", operate);
            break;
    }
}

gint32 is_continue_powerstate_operate(gint32 operate)
{
    gint32 input;

    print_set_powerstate_warning_info(operate);
    input = check_the_input_operation(CLI_COMFIRM_STRING);
    return input;
}

LOCAL void print_set_frucontrol_warning_info(gint32 operate)
{
    switch (operate) {
        case FRU_RESET:
            g_printf("WARNING: A forced restart may damage programs or unsaved data of the server.\r\n");
            break;
        case FRU_GRACE_REBOOT:
            g_printf("WARNING: A forced power cycle may damage programs or unsaved data of the server.\r\n");
            break;
        default:
            debug_log(DLOG_ERROR, "Unknown operate %d.", operate);
            break;
    }
}

gint32 is_continue_frucontrol_operate(gint32 operate)
{
    gint32 input;

    print_set_frucontrol_warning_info(operate);
    input = check_the_input_operation(CLI_COMFIRM_STRING);
    return input;
}