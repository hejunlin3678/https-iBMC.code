

#include "ipmcget_ibmc_mgmt.h"

#define MIN_SFP_POWER_VALUE 1
#define MAX_SFP_POWER_VALUE 65535

#define TEMP_COEFF 256
#define VOLT_COEFF 10000.0
#define ELEC_COEFF 500.0


gint32 print_bbu_module_info(OBJ_HANDLE obj_handle, gpointer data)
{
    guint8 bbu_id = 0;
    guint8 bbu_health = 0;
    gchar firmware_version[PARAMETER_LEN] = {0};
    guint8 per_rem_cap = 0;
    guint32 remain_cap = 0;
    guint32 full_cap = 0;
    guint8 work_status = 0;
    guint32 work_time = 0;
    gchar batt_manu_date[PARAMETER_LEN] = {0};
    gchar batt_model[PARAMETER_LEN] = {0};
    gchar batt_sn[PARAMETER_LEN] = {0};
    gchar batt_manu[PARAMETER_LEN] = {0};
    guint8 m2_status = 0;

    gchar* bbu_health_str[] = {
        "Normal",
        "Minor",
        "Major",
        "Critical",
        "Unknown",
    };

    gchar* bbu_status_str[] = {
        "OFFLINE",
        "INIT",
        "IDLE",
        "ON_CHARGE",
        "CHARGE_FULL",
        "ON_DISCHARGE",
        "ONLINE_TEST",
        "PROTECT",
        "SHUT_DOWN",
        "UNDEFINED",
    };

    gchar* bbu_m2_status_str[] = {
        "Absent/Absent",
        "Present/Absent",
        "Absent/Present",
        "Present/Present",
        "Unknown",
    };

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BBU_MODULE_ID, &bbu_id);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BBU_MODULE_HEALTH, &bbu_health);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BBU_MODULE_FIRMWARE_VER, firmware_version, PARAMETER_LEN);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BBU_MODULE_PER_REMAIN_CAP, &per_rem_cap);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_BBU_MODULE_REMAIN_CAP, &remain_cap);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_BBU_MODULE_FULL_CAP, &full_cap);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BBU_MODULE_WORK_STATUS, &work_status);
    (void)dal_get_property_value_uint32(obj_handle, PROPERTY_BBU_MODULE_WORK_TIME, &work_time);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BBU_MODULE_BATT_MANU_DATE, batt_manu_date, PARAMETER_LEN);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BBU_MODULE_BATT_MODEL, batt_model, PARAMETER_LEN);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BBU_MODULE_BATT_SN, batt_sn, PARAMETER_LEN);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_BBU_MODULE_BATT_MANU, batt_manu, PARAMETER_LEN);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BBU_MODULE_M2_STATUS, &m2_status);

    if (bbu_health >= (sizeof(bbu_health_str) / sizeof(bbu_health_str[0]))) {
        bbu_health = (sizeof(bbu_health_str) / sizeof(bbu_health_str[0])) - 1;
    }

    if (work_status >= (sizeof(bbu_status_str) / sizeof(bbu_status_str[0]))) {
        work_status = (sizeof(bbu_status_str) / sizeof(bbu_status_str[0])) - 1;
    }

    if (m2_status >= (sizeof(bbu_m2_status_str) / sizeof(bbu_m2_status_str[0]))) {
        m2_status = (sizeof(bbu_m2_status_str) / sizeof(bbu_m2_status_str[0])) - 1;
    }

    g_printf("------------------- BBU MODULE INFO -------------------\r\n");
    g_printf("%-30s: %d\r\n", "BBU ID", bbu_id);
    g_printf("%-30s: %s\r\n", "BBU Health", bbu_health_str[bbu_health]);
    g_printf("%-30s: %s\r\n", "Firmware Version", firmware_version);
    g_printf("%-30s: %d\r\n", "Remain Capacity(%)", per_rem_cap);
    g_printf("%-30s: %d\r\n", "Remain Capacity(mWh)", remain_cap);
    g_printf("%-30s: %d\r\n", "Full Capacity(mWh)", full_cap);
    g_printf("%-30s: %s\r\n", "Work Status", bbu_status_str[work_status]);
    g_printf("%-30s: %d\r\n", "Work Time(h)", work_time);
    g_printf("%-30s: %s\r\n", "Battery Manufacturing Date", batt_manu_date);
    g_printf("%-30s: %s\r\n", "Battery Model", batt_model);
    g_printf("%-30s: %s\r\n", "Battery SN", batt_sn);
    g_printf("%-30s: %s\r\n", "Battery Manufacturer", batt_manu);
    g_printf("%-30s: %s\r\n", "M.2_1/M.2_2", bbu_m2_status_str[m2_status]);

    return RET_OK;
}


LOCAL gint32 get_remote_rpc_data(gchar *target_ip, guint16 *target_port)
{
    guint8 current_slot = 0;
    OBJ_HANDLE bmc_obj_handle = 0;
    OBJ_HANDLE rpc_obj_handle = 0;
    gint32 ret;
    guint8 rpc_status = 0;

    (void)dfl_get_object_handle(BMC_BOARD_OBJ_NAME, &bmc_obj_handle);

    (void)dal_get_property_value_byte(bmc_obj_handle, BMC_SLOT_ID_NAME, &current_slot);

    ret = dal_get_specific_object_byte(CLASS_REMOTE_RPC_INFO, PROPERTY_REMOTE_RPC_INFO_INDEX, current_slot - 1,
        &rpc_obj_handle);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    (void)dal_get_property_value_uint16(rpc_obj_handle, PROPERTY_REMOTE_RPC_INFO_TARGET_PORT, target_port);

    (void)dal_get_property_value_string(rpc_obj_handle, PROPERTY_REMOTE_RPC_INFO_TARGET_IP, target_ip, MAX_IPV4_LEN);

    (void)dal_get_property_value_byte(rpc_obj_handle, PROPERTY_REMOTE_RPC_INFO_STATUS, &rpc_status);

    if (rpc_status != ENABLE) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_remote_otm_obj_list(const gchar *class_name, GSList **output_list)
{
    gint32 ret;
    guint16 target_port = 0;
    GSList *input_list = NULL;
    gchar               target_ip[MAX_IPV4_LEN] = {0};

    ret = get_remote_rpc_data(target_ip, &target_port);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(class_name));

    ret = dfl_call_remote_class_method2(target_ip, target_port, OBJ_NAME_DATA_SYNC_SERVER_CONFIG,
        METHOD_GET_OBJ_NAME_LIST, NULL, input_list, output_list);
    if (ret != DFL_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}


LOCAL gint32 get_remote_special_otm_obj_name(guint8 port_id, gchar **obj_name)
{
    gint32 ret;
    guint16 target_port = 0;
    gchar               target_ip[MAX_IPV4_LEN] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    gsize len = 0;

    ret = get_remote_rpc_data(target_ip, &target_port);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(CLASS_PORT_ADAPTER));
    input_list = g_slist_append(input_list, g_variant_new_string(PROPERTY_PORT_ADAPTER_PORT_NO));
    input_list = g_slist_append(input_list, g_variant_new_byte(port_id));

    ret = dfl_call_remote_class_method2(target_ip, target_port, OBJ_NAME_DATA_SYNC_SERVER_CONFIG,
        METHOD_GET_SPECIAL_OBJECT_BYTE, NULL, input_list, &output_list);
    if (ret != DFL_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    *obj_name = g_variant_dup_string((GVariant *)g_slist_nth_data(output_list, 0), &len);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    return RET_OK;
}


LOCAL gint32 get_remote_otm_info_list(const gchar *obj_name, const gchar *method_name, GSList **output_list,
    guint8 port_id)
{
    gint32 ret;
    guint8 port_presence;
    guint16 target_port = 0;
    gchar               target_ip[MAX_IPV4_LEN] = {0};
    GSList *input_list = NULL;
    GSList *prop_output_list = NULL;

    ret = get_remote_rpc_data(target_ip, &target_port);
    if (ret != RET_OK) {
        return RET_ERR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(PROPERTY_PORT_ADAPTER_PRESENT));
    ret = dfl_get_remote_property_value2(target_ip, target_port, obj_name, input_list, &prop_output_list);
    if (ret != DFL_OK) {
        g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
        return RET_ERR;
    }

    port_presence = g_variant_get_byte((GVariant *)g_slist_nth_data(prop_output_list, 0));
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(prop_output_list, (GDestroyNotify)g_variant_unref);
    if (port_presence == MEDIA_NOT_PRESENT) {
        g_printf("OTM Port%d not present.\r\n", port_id);
        
        return COMP_MEDIA_NOT_PRESENT;
        
    }

    ret = dfl_call_remote_class_method2(target_ip, target_port, obj_name, method_name, NULL, NULL, output_list);
    
    if (ret != DFL_OK) {
        return ret;
    }
    
    return RET_OK;
}


gint32 get_port_adapter_max_num_and_flg(guint8 *flag, guint32* max_port_num)
{
    GSList *obj_list = NULL;
    gint32 ret;

    if (flag == NULL || max_port_num == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_count(CLASS_PORT_ADAPTER, max_port_num);
    if (ret == DFL_OK && *max_port_num != 0) {
        *flag = CURRENT_IS_LOCAL;
        return RET_OK;
    }

    ret = get_remote_otm_obj_list(CLASS_PORT_ADAPTER, &obj_list);
    if (ret == RET_OK) {
        gsize parm_temp = 0;
        const gchar **object = NULL;

        object = g_variant_get_strv((GVariant *)g_slist_nth_data(obj_list, 0), &parm_temp);
        if (object != NULL) {
            g_free(object);
        }
        *flag = CURRENT_IS_REMOTE;
        *max_port_num = (guint32)parm_temp;
        g_slist_free_full(obj_list, (GDestroyNotify)g_variant_unref);
        return RET_OK;
    }

    return RET_ERR;
}


void print_otm_manufacture_info(guint8 is_local_flag, guint8 port_id)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 port_presence = 0;
    GSList *output_list = NULL;
    const gchar *vendor_name = NULL;
    const gchar *vendor_pn = NULL;
    const gchar *vendor_sn = NULL;
    const gchar *vendor_oui = NULL;
    const gchar *vendor_rev = NULL;
    const gchar *otm_date = NULL;
    const gchar *t_code = NULL;
    gchar *obj_name = NULL;

    if (is_local_flag) {
        ret = dal_get_specific_object_byte(CLASS_PORT_ADAPTER, PROPERTY_PORT_ADAPTER_PORT_NO, port_id, &obj_handle);
        if (ret != RET_OK) {
            goto print_fail_info;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PORT_ADAPTER_PRESENT, &port_presence);
        if (port_presence == MEDIA_NOT_PRESENT) {
            g_printf("OTM Port%d not present.\r\n", port_id);
            return;
        }

        ret = uip_call_class_method(NULL, NULL, NULL, CLASS_PORT_ADAPTER, dfl_get_object_name(obj_handle),
            METHOD_GET_PORT_MANUFACTURE_INFO, NULL, &output_list);
    } else {
        ret = get_remote_special_otm_obj_name(port_id, &obj_name);
        if (ret != RET_OK) {
            goto print_fail_info;
        }

        ret = get_remote_otm_info_list(obj_name, METHOD_GET_PORT_MANUFACTURE_INFO, &output_list, port_id);
        if (obj_name != NULL) {
            g_free(obj_name);
        }
        
        if (ret == COMP_MEDIA_NOT_PRESENT) {
            return;
        }
        
    }
print_fail_info:
    if (ret != RET_OK) {
        g_printf("Get OTM Port%d information failed.\r\n", port_id);
        if (output_list != NULL) {
            uip_free_gvariant_list(output_list);
        }
        return;
    }

    vendor_name = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    vendor_pn = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 1), NULL);
    vendor_sn = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 2), NULL);
    vendor_oui = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 3), NULL);
    vendor_rev = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 4), NULL);
    otm_date = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 5), NULL);
    t_code = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 6), NULL);

    
    g_printf("OTM Port%d info:\r\n", port_id);
    g_printf("VendorName   : %s\r\n", vendor_name);
    g_printf("VendorPN     : %s\r\n", vendor_pn);
    g_printf("VendorSN     : %s\r\n", vendor_sn);
    g_printf("VendorOUI    : %s\r\n", vendor_oui);
    g_printf("VendorREV    : %s\r\n", vendor_rev);
    g_printf("Data         : %s\r\n", otm_date);
    g_printf("T_Code       : %s\r\n", t_code);

    uip_free_gvariant_list(output_list);
    return;
}


void print_otm_hardware_status(guint8 is_local_flag, guint8 port_id)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 port_presence = 0;
    GSList *output_list = NULL;
    guint16 temperature = 0;
    guint16 vcc = 0;
    guint16 tx_bias = 0;
    guint16 tx_power = 0;
    guint16 rx_power = 0;
    gdouble converted_vcc = 0.0;
    gdouble converted_tx_bias = 0.0;
    gdouble converted_tx_power = 0;
    gdouble converted_rx_power = 0;
    gchar string_tx_power[256] = "invalid";
    gchar string_rx_power[256] = "invalid";
    gchar *obj_name = NULL;

    if (is_local_flag) {
        ret = dal_get_specific_object_byte(CLASS_PORT_ADAPTER, PROPERTY_PORT_ADAPTER_PORT_NO, port_id, &obj_handle);
        if (ret != RET_OK) {
            goto print_fail_info;
        }

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_PORT_ADAPTER_PRESENT, &port_presence);
        if (port_presence == MEDIA_NOT_PRESENT) {
            g_printf("OTM Port%d not present.\r\n", port_id);
            return;
        }

        ret = uip_call_class_method(NULL, NULL, NULL, CLASS_PORT_ADAPTER, dfl_get_object_name(obj_handle),
            METHOD_GET_HARDWARE_STATUS, NULL, &output_list);
    } else {
        ret = get_remote_special_otm_obj_name(port_id, &obj_name);
        if (ret != RET_OK) {
            goto print_fail_info;
        }

        ret = get_remote_otm_info_list(obj_name, METHOD_GET_HARDWARE_STATUS, &output_list, port_id);
        if (obj_name != NULL) {
            g_free(obj_name);
        }

        
        if (ret == COMP_MEDIA_NOT_PRESENT) {
            return;
        }
        
    }
    
    if (ret == COMP_CODE_PARA_NOT_SUPPORT_REAL_INFO) {
        g_printf("Not support to query port%d cable hardware status.\r\n", port_id);
        return;
    }
    
print_fail_info:
    if (ret != RET_OK) {
        g_printf("Get OTM port%d information failed.\r\n", port_id);
        if (output_list != NULL) {
            uip_free_gvariant_list(output_list);
        }
        return;
    }

    temperature = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 0));
    vcc = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 1));
    tx_bias = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 2));
    tx_power = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 3));
    rx_power = g_variant_get_uint16((GVariant *)g_slist_nth_data(output_list, 4));

    
    temperature = temperature / TEMP_COEFF;

    
    converted_vcc = vcc / VOLT_COEFF;

    
    converted_tx_bias = tx_bias / ELEC_COEFF;

    
    if (tx_power > MIN_SFP_POWER_VALUE && tx_power < MAX_SFP_POWER_VALUE) {
        
        converted_tx_power = tx_power / 10000.0;
        
        converted_tx_power = 10.0 * (log(converted_tx_power) / log(10.0));
        (void)snprintf_s(string_tx_power, sizeof(string_tx_power), sizeof(string_tx_power) - 1, "%.2f",
            converted_tx_power);
    }

    
    if (rx_power > MIN_SFP_POWER_VALUE && rx_power < MAX_SFP_POWER_VALUE) {
        
        converted_rx_power = rx_power / 10000.0;
        
        converted_rx_power = 10.0 * (log(converted_rx_power) / log(10.0));

        (void)snprintf_s(string_rx_power, sizeof(string_rx_power), sizeof(string_rx_power) - 1, "%.2f",
            converted_rx_power);
    }

    
    g_printf("OTM port%d data:\r\n", port_id);
    g_printf("Temp: %d Celius, Vcc: %.2f V, BIAS: %.2f mA, TXPower: %s dBm, RXPower: %s dBM\r\n", temperature,
        converted_vcc, converted_tx_bias, string_tx_power, string_rx_power);

    uip_free_gvariant_list(output_list);

    return;
}


gint32 print_rack_location_info(OBJ_HANDLE obj_rack_asset)
{
    gint32 ret;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    const gchar *country = NULL;
    const gchar *territory = NULL;
    const gchar *city = NULL;
    const gchar *street = NULL;
    const gchar *location = NULL;
    const gchar *post_code = NULL;
    const gchar *house_num = NULL;
    const gchar *building = NULL;
    const gchar *floor = NULL;
    const gchar *room = NULL;
    const gchar *row = NULL;
    const gchar *rack = NULL;

    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_COUNTRY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_TERRITORY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_CITY);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_STREET);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_LOCATION_INFO);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_HOUSE_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_BUILDING);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_FLOOR);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_ROOM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_POST_CODE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_ROW);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RACK_ASSET_MGMT_RACK);
    ret = dfl_multiget_property_value(obj_rack_asset, property_name_list, &property_value_list);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return FALSE;
    }

    country = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), NULL);
    territory = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), NULL);
    city = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), NULL);
    street = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 3), NULL);
    location = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 4), NULL);
    house_num = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 5), NULL);
    building = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 6), NULL);
    floor = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 7), NULL);
    room = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 8), NULL);
    post_code = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 9), NULL);
    row = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 10), NULL);
    rack = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 11), NULL);

    
    g_printf("%s\n", "Location");
    g_printf("----------------------------------------------------------------------\n");
    if ((strcmp("", country) != 0) && (strcmp(INVALID_DATA_STRING, country) != 0)) {
        g_printf("%-40s : %s\n", "Conutry", country);
    }
    if ((strcmp("", territory) != 0) && (strcmp(INVALID_DATA_STRING, territory) != 0)) {
        g_printf("%-40s : %s\n", "Territory", territory);
    }
    if (strcmp("", city) != 0) {
        g_printf("%-40s : %s\n", "City", city);
    }
    if ((strcmp("", street) != 0) && (strcmp(INVALID_DATA_STRING, street) != 0)) {
        g_printf("%-40s : %s\n", "Street", street);
    }
    if ((strcmp("", location) != 0) && (strcmp(INVALID_DATA_STRING, location) != 0)) {
        g_printf("%-40s : %s\n", "Additional Location", location);
    }
    if ((strcmp("", house_num) != 0) && (strcmp(INVALID_DATA_STRING, house_num) != 0)) {
        g_printf("%-40s : %s\n", "HouseNumber", house_num);
    }
    if (strcmp("", building) != 0) {
        g_printf("%-40s : %s\n", "Building", building);
    }
    if (strcmp("", floor) != 0) {
        g_printf("%-40s : %s\n", "Floor", floor);
    }
    if (strcmp("", room) != 0) {
        g_printf("%-40s : %s\n", "Room", room);
    }
    if ((strcmp("", post_code) != 0) && (strcmp(INVALID_DATA_STRING, post_code) != 0)) {
        g_printf("%-40s : %s\n", "PostCode", post_code);
    }
    g_printf("----------------------------------------------------------------------\n");

    
    g_printf("%s\n", "Placement");
    g_printf("----------------------------------------------------------------------\n");
    if (strcmp("", row) != 0) {
        g_printf("%-40s : %s\n", "Row", row);
    }
    if (strcmp("", rack) != 0) {
        g_printf("%-40s : %s\n", "Rack", rack);
    }
    g_printf("----------------------------------------------------------------------\n");

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value_list);
    property_name_list = NULL;
    property_value_list = NULL;

    return TRUE;
}

LOCAL void print_rfid_extend_filed(const gchar *extend_json_str)
{
    json_object *tag_info = NULL;
    const gchar *val_str = NULL;

    tag_info = json_tokener_parse(extend_json_str);
    if (tag_info == NULL) {
        return;
    }

#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
    json_object_object_foreach0(tag_info, key_str, val)
    {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#pragma GCC diagnostic pop
#endif
        val_str = dal_json_object_get_str(val);
        if ((strcmp("", val_str) != 0) && (strcmp(INVALID_DATA_STRING, val_str) != 0)) {
            g_printf("%-40s : %s\n", key_str, val_str);
        }
    }

    json_object_put(tag_info);
}


LOCAL void print_unit_asset_info(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guint8 u_num = 0;
    guint8 detected = 0;
    guint8 height;
    guint8 life;
    guint32 weight;
    guint32 power;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    const gchar *color = NULL;
    const gchar *state = NULL;
    const gchar *model = NULL;
    const gchar *type = NULL;
    const gchar *sn = NULL;
    const gchar *manu = NULL;
    const gchar *time = NULL;
    const gchar *owner = NULL;
    const gchar *part = NULL;
    const gchar *extend = NULL;
    gchar           uid[PARAMETER_LEN] = {0};

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_UNIT_INFO_U_NUM, &u_num);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_UNIT_INFO_DETECTED, &detected);

    g_printf("----------------------------------------------------------------------\n");
    
    if (detected != 1) {
        g_printf("Unit number %d has no tag detected.\r\n", u_num);
        return;
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_INDICATOR_COLOR);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_INDICATOR_LED);
    
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_U_HEIGHT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_DEV_MODEL);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_DEV_TYPE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_MANUFACTURER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_SER_NUM);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return;
    }

    color = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), NULL);
    state = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 1), NULL);
    height = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 2));
    model = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 3), NULL);
    type = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 4), NULL);
    manu = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 5), NULL);
    sn = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 6), NULL);

    
    (void)dal_get_byte_array_with_separator(obj_handle, PROPERTY_UNIT_INFO_UID, ":", uid, sizeof(uid));

    g_printf("%-40s : %d\n", "Unit Number", u_num);
    g_printf("%-40s : %s\n", "Indicator Color", (strcmp("", color) == 0) ? INVALID_DATA_STRING : color);
    g_printf("%-40s : %s\n", "Indicator State", (strcmp("", state) == 0) ? INVALID_DATA_STRING : state);
    g_printf("%-40s : %d\n", "Height(U)", (height == 0) ? 1 : height);
    g_printf("%-40s : %s\n", "RFID Tag UID", (strcmp("", uid) == 0) ? INVALID_DATA_STRING : uid);
    g_printf("%-40s : %s\n", "Device Model", (strcmp("", model) == 0) ? INVALID_DATA_STRING : model);
    g_printf("%-40s : %s\n", "Device Type", (strcmp("", type) == 0) ? INVALID_DATA_STRING : type);
    g_printf("%-40s : %s\n", "Manufacturer", (strcmp("", manu) == 0) ? INVALID_DATA_STRING : manu);
    g_printf("%-40s : %s\n", "Serial Number", (strcmp("", sn) == 0) ? INVALID_DATA_STRING : sn);

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value_list);
    property_name_list = NULL;
    property_value_list = NULL;

    
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_PART_NUM);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_LIFE_CYCLE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_CHK_IN_TIME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_WEIGHT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_RATED_POWER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_ASSET_OWNER);
    property_name_list = g_slist_append(property_name_list, PROPERTY_UNIT_INFO_EXTEND_FIELD);
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    if (ret != DFL_OK) {
        g_slist_free(property_name_list);
        return;
    }

    part = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), NULL);
    life = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));
    time = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), NULL);
    weight = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 3));
    power = g_variant_get_uint32((GVariant *)g_slist_nth_data(property_value_list, 4));
    owner = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 5), NULL);
    extend = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 6), NULL);

    
    if ((strcmp("", part) != 0) && (strcmp(INVALID_DATA_STRING, part) != 0)) {
        g_printf("%-40s : %s\n", "Part Number", part);
    }
    if ((life != 0xFF) && (life != 0)) {
        g_printf("%-40s : %d\n", "Asset Life Cycle(Years)", life);
    }
    if ((strcmp("", time) != 0) && (strcmp(INVALID_DATA_STRING, time) != 0)) {
        g_printf("%-40s : %s\n", "Start Use From", time);
    }
    if (weight != 0) {
        g_printf("%-40s : %d\n", "Weight(Kg)", weight);
    }
    if (power != 0) {
        g_printf("%-40s : %d\n", "Rated Power(Watts)", power);
    }
    if ((strcmp("", owner) != 0) && (strcmp(INVALID_DATA_STRING, owner) != 0)) {
        g_printf("%-40s : %s\n", "Asset Owner", owner);
    }
    if ((strcmp("", extend) != 0) && (strcmp(INVALID_DATA_STRING, extend) != 0)) {
        print_rfid_extend_filed(extend);
    }

    g_slist_free(property_name_list);
    uip_free_gvariant_list(property_value_list);
    property_name_list = NULL;
    property_value_list = NULL;

    return;
}


void process_unit_info(guint8 unit_num)
{
    gint32 ret;
    guint8 prop_u_num = 0;
    guint8 match_flag = FALSE;
    GSList *node = NULL;
    GSList *obj_list = NULL;

    ret = dfl_get_object_list(CLASS_UNIT_INFO, &obj_list);
    if (ret != DFL_OK || obj_list == NULL) {
        g_printf("Not unit locator device found.\r\n");
        return;
    }

    if (unit_num == PARAM_DEFAULT_VALUE) {
        for (node = obj_list; node; node = g_slist_next(node)) {
            print_unit_asset_info((OBJ_HANDLE)(node->data));
        }
    } else {
        for (node = obj_list; node; node = g_slist_next(node)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)(node->data), PROPERTY_UNIT_INFO_U_NUM, &prop_u_num);
            if ((ret == RET_OK) && (prop_u_num == unit_num)) {
                match_flag = TRUE;
                print_unit_asset_info((OBJ_HANDLE)(node->data));
                break;
            }
        }

        if (match_flag == FALSE) {
            g_printf("Invalid unit number : %d\r\n", unit_num);
        }
    }

    g_slist_free(obj_list);

    return;
}


gint32 ipmc_get_timezone(gchar *time_zone, guint32 sz)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    const gchar *tz_tmp = NULL;
    GVariant *value = NULL;

    if (time_zone == NULL) {
        return RET_ERR;
    }

    ret = uip_get_object_property(CLASS_NAME_BMC, OBJ_NAME_BMC, PROPERTY_BMC_TIMEZONE_STR, &value);
    if (ret != RET_OK) {
        return RET_ERR;
    } else {
        tz_tmp = g_variant_get_string(value, NULL);
        if (sz == 0) {
            g_variant_unref(value);
            debug_log(DLOG_ERROR, "%s failed:input param error, sz is 0.", __FUNCTION__);
            return RET_ERR;
        }
        memset_s(time_zone, sz, 0, sz);
        safe_fun_ret = strncpy_s(time_zone, sz, tz_tmp, sz - 1);
        if (safe_fun_ret != EOK) {
            g_variant_unref(value);
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
            return RET_ERR;
        }
    }

    g_variant_unref(value);

    return RET_OK;
}

void print_time_offset(gint16 timeOffset)
{
    if (timeOffset == 0) {
        g_printf("(UTC) \r\n");
    } else {
        if ((timeOffset < 0) && (timeOffset > MINUS_SIXTY_MINUTES_OFFSET)) {
            g_printf("(UTC-00:%02d) \r\n", abs(timeOffset % SEXAGESIMAL));
        } else {
            g_printf("(UTC%+03d:%02d) \r\n", (timeOffset / SEXAGESIMAL), abs(timeOffset % SEXAGESIMAL));
        }
    }
    return;
}
