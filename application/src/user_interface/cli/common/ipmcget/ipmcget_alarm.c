

#include "ipmcget_alarm.h"

const gchar* syslog_eventsource[] = {
    "operationlogs",
    "securitylogs",
    "eventlogs",
};


LOCAL void prase_sensor_alarm_value(guint16 reading_mask, gdouble lnr, gdouble lc, gdouble lnc, gdouble unc, gdouble uc,
    gdouble unr)
{
    SENSOR_READINGMASK_BIT *sensor_reading_mask = NULL;
    sensor_reading_mask = (SENSOR_READINGMASK_BIT *)&reading_mask;
    if (sensor_reading_mask->readable_lower_nonrecoverable) {
        g_printf("| %-10.3f ", lnr);
    } else {
        g_printf("| %-10s ", "na");
    }

    if (sensor_reading_mask->readable_lower_critical) {
        g_printf("| %-10.3f ", lc);
    } else {
        g_printf("| %-10s ", "na");
    }

    if (sensor_reading_mask->readable_lower_noncritical) {
        g_printf("| %-10.3f ", lnc);
    } else {
        g_printf("| %-10s ", "na");
    }

    if (sensor_reading_mask->readable_upper_noncritical) {
        g_printf("| %-8.3f ", unc);
    } else {
        g_printf("| %-8s ", "na");
    }

    if (sensor_reading_mask->readable_upper_critical) {
        g_printf("| %-10.3f ", uc);
    } else {
        g_printf("| %-10s ", "na");
    }

    if (sensor_reading_mask->readable_upper_nonrecoverable) {
        g_printf("| %-10.3f ", unr);
    } else {
        g_printf("| %-10s ", "na");
    }

    return;
}

LOCAL gint32 parse_sensor_status(guint16 status, gchar *status_buff)
{
#define SENSOR_STATUS_NR "nr"
#define SENSOR_STATUS_CR "cr"
#define SENSOR_STATUS_NC "nc"
#define SENSOT_STATUS_OK "ok"

    if (status_buff == NULL) {
        return RET_ERR;
    }

    SENSOR_STATE_BIT_T *sensor_state = (SENSOR_STATE_BIT_T *)&status;
    gchar *state = SENSOT_STATUS_OK;

    if (sensor_state->assert_nonrecoverable_lgl || sensor_state->assert_nonrecoverable_ugh) {
        state = SENSOR_STATUS_NR;
    } else if (sensor_state->assert_critical_lgl || sensor_state->assert_critical_ugh) {
        state = SENSOR_STATUS_CR;
    } else if (sensor_state->assert_noncritical_lgl || sensor_state->assert_noncritical_ugh) {
        state = SENSOR_STATUS_NC;
    }

    if (snprintf_s(status_buff, SENSOR_STATUS_LEN + 1, SENSOR_STATUS_LEN, "%s", state) < 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 print_analog_sensor_info(OBJ_HANDLE obj_handle, gpointer data)
{
    gint32 ret;
    guchar reading_mask;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    SENSOR_CAPABILITY_S *sensor_cap = NULL;
    const gchar *name = NULL;
    const gchar *unit = NULL;
    guint16 status;
    guint8 reading_status;
    guint8 capabilities;
    guint8 number;
    gchar status_buff[SENSOR_STATUS_LEN + 1] = {0};
    gdouble value;
    gdouble lnr;
    gdouble lc;
    gdouble lnc;
    gdouble unc;
    gdouble uc;
    gdouble unr;
    gdouble phys;
    gdouble nhys;

    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_NUMBER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_SENSOR_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_READING_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_UNIT_STR);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_ASSERT_STATUS);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_LNR_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_LC_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_LNC_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_UNC_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_UC_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_UNR_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_P_HYS_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_N_HYS_CONVERT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_READING_MASK);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THS_SENSOR_READING_STATUS);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_CAPABILITIES);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    
    number = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), NULL);
    value = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 2));
    unit = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), NULL);
    status = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 4));
    lnr = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 5));
    lc = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 6));
    lnc = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 7));
    unc = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 8));
    uc = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 9));
    unr = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 10));
    phys = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 11));
    nhys = g_variant_get_double((GVariant *)g_slist_nth_data(property_value, 12));
    reading_mask = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 13));
    reading_status = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 14));
    capabilities = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 15));
    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;
    sensor_cap = (SENSOR_CAPABILITY_S *)&capabilities;
    ret = parse_sensor_status(status, status_buff);
    if (ret != RET_OK) {
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        return RET_ERR;
    }

    g_printf("0x%-8x | %-16s ", number, name);

    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all ||
        !sensor_reading_status->initial_update_progress 
        || !sensor_reading_status->disable_access_error) {
        g_printf("| %-10s | %-12s | %-6s ", "na", unit, "na");
    } else {
        g_printf("| %-10.3f | %-12s | %-6s ", value, unit, status_buff);
    }

    if (!((sensor_cap->threshold_access_support == SENSOR_CAP_READABLE_SETTABLE) ||
        (sensor_cap->threshold_access_support == SENSOR_CAP_READABLE))) {
        g_printf("| %-10s | %-10s | %-10s | %-8s | %-10s | %-10s | %-6s | %-6s", "na", "na", "na", "na", "na", "na",
            "na", "na");
    } else {
        prase_sensor_alarm_value(reading_mask, lnr, lc, lnc, unc, uc, unr);

        if (!((sensor_cap->hysteresis_support == SENSOR_CAP_READABLE_SETTABLE) ||
            (sensor_cap->hysteresis_support == SENSOR_CAP_READABLE))) {
            g_printf("| %-6s | %-6s ", "na", "na");
        } else {
            g_printf("| %-6.3f | %-6.3f", phys, nhys);
        }
    }

    g_printf("\n");

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return RET_OK;
}


gint32 print_discrete_sensor_info(OBJ_HANDLE obj_handle, gpointer data)
{
    gint32 ret;
    const gchar *name = NULL;
    guint16 status;
    guint16 value;
    guint8 reading_status;
    guint8 number;
    EVENT_WORK_STATE_S *sensor_reading_status = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_NUMBER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_SENSOR_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_READING_RAW);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THR_SENSOR_ASSERT_STATUS);
    property_name_list = g_slist_append(property_name_list, PROPERTY_THS_SENSOR_READING_STATUS);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    
    number = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 1), NULL);
    value = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 2));
    status = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value, 3));
    reading_status = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 4));
    sensor_reading_status = (EVENT_WORK_STATE_S *)&reading_status;

    g_printf("0x%-8x | %-16s ", number, name);

    if (!sensor_reading_status->disable_scanning || !sensor_reading_status->disable_scanning_local ||
        !sensor_reading_status->disable_all ||
        !sensor_reading_status->initial_update_progress 
        || !sensor_reading_status->disable_access_error || (value & 0xFF00) != 0) {
        g_printf("| %-10s | %-12s | %-6s ", "na", "discrete", "na");
    } else {
        g_printf("| 0x%-8x | %-12s | 0x%-4x ", (value & 0xFF), "discrete", status);
    }

    g_printf("| %-10s | %-10s | %-10s | %-8s | %-10s | %-10s | %-6s | %-6s", "na", "na", "na", "na", "na", "na", "na",
        "na");
    g_printf("\n");
    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return RET_OK;
}


gint32 print_trapitem_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    guint8 indexnum;
    guchar state;
    gchar trapitem_state[TRAPITEM_STATE_LEN + 1] = {0};
    guint32 portnum;
    const gchar *dest_ipaddr = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value = NULL;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_TRAP_ITEM_INDEX_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_TRAP_ITEM_ENABLE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_TRAP_ITEM_DEST_IPPORT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_TRAP_ITEM_DEST_IPADDR);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value);
    if (ret != DFL_OK) {
        g_printf("Get trapitem information failed.");
        g_slist_free(property_name_list);
        return RET_ERR;
    }

    indexnum = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 0));
    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value, 1));
    portnum = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value, 2));
    dest_ipaddr = g_variant_get_string((GVariant *)g_slist_nth_data(property_value, 3), 0);
    if (state == 0) {
        
        snprintf_s(trapitem_state, TRAPITEM_STATE_LEN + 1, TRAPITEM_STATE_LEN, "%s", "disabled");
    } else if (state == 1) {
        snprintf_s(trapitem_state, TRAPITEM_STATE_LEN + 1, TRAPITEM_STATE_LEN, "%s", "enabled");
    }
    
    else {
        g_printf("TrapItem state invalid.\r\n");
        g_slist_free(property_name_list);
        uip_free_gvariant_list(property_value);
        return RET_ERR;
    }

    g_printf("%-15d | %-10s | %-10d | %-20s\r\n", indexnum + 1, trapitem_state, portnum, dest_ipaddr);

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value);
    return RET_OK;
}


gint32 get_syslog_state(void)
{
    guint8 state = 0;
    OBJ_HANDLE obj_handle = 0;

    if (dal_get_object_handle_nth(CLASS_SYSLOG_CONFIG, 0, &obj_handle) != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SYSLOG_MGNT_ENABLE_STATE, &state);
    if (state) {
        g_printf("syslog state: enabled\r\n");
    } else {
        g_printf("syslog state: disabled\r\n");
    }

    return TRUE;
}


gint32 get_syslog_dest_state(gint32 dest)
{
    guchar state = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = dal_get_specific_object_byte(CLASS_SYSLOG_ITEM_CONFIG, PROPERTY_REMOTE_SYSLOG_INDEX, dest, &obj_handle);
    if (ret != RET_OK) {
        g_printf("Unknown error.\r\n");
        return FALSE;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REMOTE_SYSLOG_ENABLE_STATE, &state);

    // 打印需要
    dest = dest + 1;

    if (state == 0x01) {
        g_printf("syslog dest%d state: enabled\r\n", dest);
    } else if (state == 0x00) {
        g_printf("syslog dest%d state: disabled\r\n", dest);
    } else {
        g_printf("syslog dest%d state: unknown\r\n", dest);
    }

    return TRUE;
}


gint32 print_syslogitem_info(OBJ_HANDLE obj_handle, gpointer user_data)
{
    errno_t safe_fun_ret = EOK;
    guint8 indexnum = 0;
    guchar state = 0;
    gchar trapitem_state[TRAPITEM_STATE_LEN + 1] = {0};
    guint32 port = 0;
    gchar dest_ipaddr[256] = {0};
    guint32 src_mask = 0;
    gchar src_description[512] = {0};
    gint32 i = 0;

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REMOTE_SYSLOG_INDEX, &indexnum);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_REMOTE_SYSLOG_ENABLE_STATE, &state);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_REMOTE_SYSLOG_DEST_ADDR, dest_ipaddr, sizeof(dest_ipaddr));
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_REMOTE_SYSLOG_DEST_PORT, &port);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_REMOTE_SYSLOG_LOG_SRC_MASK, &src_mask);

    if (state == 0) {
        
        snprintf_s(trapitem_state, TRAPITEM_STATE_LEN + 1, TRAPITEM_STATE_LEN, "%s", "disabled");
    } else if (state == 1) {
        snprintf_s(trapitem_state, TRAPITEM_STATE_LEN + 1, TRAPITEM_STATE_LEN, "%s", "enabled");
    } else {
        g_printf("state invalid.\r\n");
        return RET_ERR;
    }

    if (src_mask == 0) {
        snprintf_s(src_description, sizeof(src_description), sizeof(src_description) - 1, "none");
        
    } else {
        for (i = 0; i < 3; i++) {
            if (src_mask & (0x01 << i)) {
                safe_fun_ret = strncat_s(src_description, sizeof(src_description), syslog_eventsource[i],
                    strlen(syslog_eventsource[i]));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
                safe_fun_ret = strncat_s(src_description, sizeof(src_description), " ", strlen(" "));
                if (safe_fun_ret != EOK) {
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
                }
            }
        }
    }

    g_printf("%-15d | %-10s | %-10d | %-20s | %-35s\r\n", indexnum + 1, trapitem_state, port, dest_ipaddr,
        src_description);

    return RET_OK;
}


gint32 get_trap_state(void)
{
    gint32 ret;
    guchar state = 0;
    GVariant *value = NULL;

    ret = uip_get_object_property(CLASS_TRAP_CONFIG, OBJECT_TRAP_CONFIG, PROPERTY_TRAP_ENABLE, &value);
    if (ret != RET_OK) {
        g_printf("Get trap state failed.\r\n ");
        return FALSE;
    } else {
        state = g_variant_get_byte(value);
        if (state) {
            g_printf("trap state : enabled\r\n");
        } else {
            g_printf("trap state : disabled\r\n");
        }
    }

    g_variant_unref(value);
    return TRUE;
}


gint32 get_trap_dest_state(gint32 dest)
{
    gint32 iRet;
    gint32 ret;
    guchar state = 0;
    GVariant *value = NULL;
    gchar obj_name[OBJ_NAME_MAX_LEN] = {0};

    
    iRet = snprintf_s(obj_name, OBJ_NAME_MAX_LEN, OBJ_NAME_MAX_LEN - 1, "%s_%d", CLASS_TRAP_ITEM_CONFIG, dest - 1);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    
    ret = uip_get_object_property(CLASS_TRAP_ITEM_CONFIG, obj_name, PROPERTY_TRAP_ITEM_ENABLE, &value);
    if (ret != RET_OK) {
        g_printf("Get trap dest%d state failed.\r\n", dest);
        return FALSE;
    } else {
        state = g_variant_get_byte(value);
        if (state == 0x01) {
            g_printf("trap dest%d state : enabled\r\n", dest);
        } else if (state == 0x00) {
            g_printf("trap dest%d state : disabled\r\n", dest);
        } else {
            g_printf("trap dest%d state : unknown\r\n", dest);
        }
    }

    g_variant_unref(value);
    return TRUE;
}
