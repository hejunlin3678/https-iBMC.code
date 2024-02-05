

#include "ipmcget_power.h"

LOCAL void __get_max_ps_num(guint8 *max_ps_num)
{
#define DEFAULT_PSU_NUM 2
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    ret = dal_get_object_handle_nth(OBJ_PRODUCT_COMPONENT, 0, &obj_handle);
    if (ret == RET_OK) {
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, max_ps_num);
    } else {
        *max_ps_num = DEFAULT_PSU_NUM;
    }
    return;
}

void get_ps_wokmode(void)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 actual_power_mode = 0;
    GSList *ps_obj_list = NULL;
    guchar workmode;
    guchar psId;
    guint8 expected_power_mode = 0;
    guint32 expected_active_mask = 0;
    guint8 max_ps_num = 0;
    guint8 deep_sleep = 0;
    OBJ_HANDLE service_handle = 0;
    guint8 deep_sleep_enable = 0;

    
    (void)dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &service_handle);
    (void)dal_get_property_value_byte(service_handle, PROTERY_PME_SERVICECONFIG_DEEP_SLEEP_ENABLE, &deep_sleep_enable);
    

    __get_max_ps_num(&max_ps_num);

    
    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_AMMETER, 0, &obj_handle);
    if (ret != RET_OK) {
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_ACTUAL_POWER_MODE, &actual_power_mode);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_EXPECTED_POWER_MODE, &expected_power_mode);
    (void)dal_get_property_value_byte(obj_handle, PROTERY_SYSPOWER_DEEP_SLEEP_MODE_ENABLE, &deep_sleep);

    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_EXPECTED_ACTIVE, &expected_active_mask);
    g_printf("\r\n%-23s :\r\n", "Current PSU WorkMode");
    g_printf("%-23s :\r\n", "Actual PSU Status");

    if (actual_power_mode == POWER_CFG_MASTER_SPARE) {
        g_printf("%-23s : Active/Standby\r\n", "    Work  Mode");
        ret = dfl_get_object_list(CLASS_NAME_PS, &ps_obj_list);
        if (ret != DFL_OK || g_slist_length(ps_obj_list) == 0) {
            g_printf("        Get ps info failed\r\n");
            return;
        }

        for (GSList *obj_note = ps_obj_list; obj_note; obj_note = obj_note->next) {
            guint8 sourcetype = 0;
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROTERY_PS_INDEX, &psId);
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_ACTUAL_ACTIVE, &workmode);
            (void)dal_get_property_value_byte((OBJ_HANDLE)obj_note->data, PROPERTY_SOURCE_TYPE, &sourcetype);
            if (sourcetype == 0) {
                continue;
            }
            if (workmode == POWER_CFG_ACTIVE_POWER) {
                g_printf("%-23s : PS%d\r\n", "    Active PSU",
                    (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) ? (psId) : (psId + 1));
            }
        }

        g_slist_free(ps_obj_list);
    } else {
        g_printf("%-23s : Load Balancing\r\n", "    Work Mode");
    }

    g_printf("%-23s :\r\n", "Predicted PSU Status");

    if (expected_power_mode == POWER_CFG_MASTER_SPARE) {
        g_printf("%-23s : Active/Standby\r\n", "    Work Mode");

        for (guint8 i = 0; i < max_ps_num; i++) {
            if (((1 << i) & expected_active_mask) == 0) {
                g_printf("%-23s : PS%d\r\n", "    Active PSU",
                    (dal_match_product_id(PRODUCT_ID_PANGEA_V6)) ? (i) : (i + 1));
            }
        }
    } else {
        g_printf("%-23s : Load Balancing\r\n", "    Work Mode");
    }
    if (deep_sleep_enable == DEEP_SLEEP_MODE_ENABLED) {
        
        g_printf("\r\n%-23s : %s\r\n", "Hibernate Status",
            (deep_sleep == DEEP_SLEEP_MODE_ENABLED) ? "Enabled" : "Disabled");
        
    }
}

gint32 get_blade_power_capping_info(void)
{
    OBJ_HANDLE object_handle = 0;
    GSList *handle_list = NULL;
    GSList *list_item = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    gint32 node_num = 1;
    guint8 presence = 0;
    gchar print_buff[MAX_FILE_NAME_LEN] = {0};

    gint32 ret = dfl_get_object_list(CLASS_NAME_IPMBETH_BLADE, &handle_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get object ipmbeth blade object list failed: %d.\n ", ret);
        return ret;
    }

    g_printf("\nBlades Power Capping Info:\n");
    g_printf("   %-8s%-10s%-14s%-13s%-14s%-12s%-15s%-15s\n", "Blade", "Presence", "FailedAction", "ManualState",
        "CappingState", "Setting(W)", "LimitPower(W)", "CurrentPower(W)");

    property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_MANUAL_STATE);
    property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_VALUE);
    property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_BASE_VALUE);
    property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_CURRENT_POWER);
    property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_BLADE_FAILACTION);
    property_name_list = g_slist_append(property_name_list, PROTERY_IPMBETH_POWER_CAPPING_ENABLE);
    for (list_item = handle_list; list_item; list_item = g_slist_next(list_item)) {
        vos_task_delay(10UL);
        object_handle = (OBJ_HANDLE)list_item->data;
        
        if (is_compute_node(object_handle) == FALSE) {
            continue;
        }
        
        ret = dfl_multiget_property_value(object_handle, property_name_list, &property_value_list);
        if (ret != DFL_OK) {
            node_num++;
            continue;
        }

        guint8 state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
        guint16 value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 1));
        guint16 base_value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
        guint16 current_pwr = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 3));
        guint8 fail_action = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 4));
        guint8 pwrcap_state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 5));

        g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
        property_value_list = NULL;

        
        (void)dal_get_property_value_byte(object_handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
        
        if (presence == 1) {
            
            if (snprintf_s(print_buff, sizeof(print_buff), sizeof(print_buff) - 1,
                "   blade%-3d%-10s%-14s%-13s%-14s%-12u%-15u%-6u\n", node_num, "Presence",
                ((fail_action == 1) ? "PowerOff" : "NoAction"), ((state == 1) ? "enabled" : "disabled"),
                ((pwrcap_state == 1) ? "enabled" : "disabled"), value, base_value, current_pwr) < 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
            }
        } else {
            if (snprintf_s(print_buff, sizeof(print_buff), sizeof(print_buff) - 1, "   blade%-3d%-10s\n", node_num,
                "Absence") < 0) {
                debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
            }
        }

        
        g_printf("%s", print_buff);

        node_num++;
    }

    g_slist_free(handle_list);
    g_slist_free(property_name_list);

    return RET_OK;
}

LOCAL gint compare_psu_id(gconstpointer a, gconstpointer b)
{
    guint8 psu_a_id = 0;
    guint8 psu_b_id = 0;
    (void)dal_get_property_value_byte((OBJ_HANDLE)a, PROTERY_PS_INDEX, &psu_a_id);
    (void)dal_get_property_value_byte((OBJ_HANDLE)b, PROTERY_PS_INDEX, &psu_b_id);
    if (psu_a_id < psu_b_id) {
        return -1;
    } else if (psu_a_id > psu_b_id) {
        return 1;
    }
    return 0;
}


gint32 get_psu_supply_source(gchar *ret_str, guint16 len)
{
#define POWER_STR_MAX_LEN 10
#define MAX_LEN 30
    gint32 ret;
    GSList *obj_psu_handle_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar psu_name[POWER_STR_MAX_LEN] = {0};
    gchar one_str[MAX_LEN] = {0};
    guint8 channel = 0;

    ret = dfl_get_object_list(CLASS_NAME_PS, &obj_psu_handle_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s get psu object list error:%d", __FUNCTION__, ret);
        return ret;
    }

    obj_psu_handle_list = g_slist_sort(obj_psu_handle_list, compare_psu_id);
    for (node = obj_psu_handle_list; node != NULL; node = node->next) {
        obj_handle = (OBJ_HANDLE)node->data;

        (void)memset_s(one_str, MAX_LEN, 0, MAX_LEN);
        (void)memset_s(psu_name, POWER_STR_MAX_LEN, 0, POWER_STR_MAX_LEN);
        (void)dal_get_property_value_string(obj_handle, PROTERY_PS_DEVICENAME, psu_name, POWER_STR_MAX_LEN);

        ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_POWERSUPPLY_CHANNEL, &channel);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s dal_get_property_value_byte error, psu is %s, ret %d", __FUNCTION__, psu_name,
                ret);
            ret = snprintf_s(one_str, MAX_LEN, MAX_LEN - 1, "%-5s : Query failed\n", psu_name);
        } else {
            ret = snprintf_s(one_str, MAX_LEN, MAX_LEN - 1, "%-5s : %u\n", psu_name, channel);
        }
        if (ret <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, psu is %s ret = %d", __FUNCTION__, psu_name, ret);
            g_slist_free(obj_psu_handle_list);
            return RET_ERR;
        }

        ret = strcat_s(ret_str, len, one_str);
        if (ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strcat_s fail, ret = %d", __FUNCTION__, ret);
            g_slist_free(obj_psu_handle_list);
            return RET_ERR;
        }
    }

    g_slist_free(obj_psu_handle_list);
    return RET_OK;
}
gboolean check_print_ps_version_condition(void)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar enable_state = 0;
    guint8 as_status = ACTIVE_STATE;

    ret = dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PME_SERVICECONFIG_PS_UPGRADE, &enable_state);
    if (ret != RET_OK) {
        return FALSE;
    }
    if (enable_state == 0) {
        return FALSE;
    }

    if (dal_check_if_mm_board()) {
        ret = get_board_active_state(&as_status);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s]: get_board_active_status failed, ret = %d.", __FUNCTION__, ret);
            return FALSE;
        }
        if (as_status == STANDBY_STATE) {
            return FALSE;
        }
    }

    return TRUE;
}