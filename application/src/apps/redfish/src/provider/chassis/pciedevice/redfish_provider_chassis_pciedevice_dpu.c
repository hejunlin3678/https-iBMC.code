

#include "redfish_provider.h"


static void get_dpucard_extend_storage_ipaddr(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    gint32 ret;
    gchar ipv4_addr[IPV4_IP_STR_SIZE + 1] = {0};
    gchar ipv6_addr[IPV6_IP_STR_SIZE + 1] = {0};
    const guint8 *data = NULL;
    gsize data_len = 0;
    json_object *property_jso = NULL;
    GSList* output_list = NULL;

    
    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "input param error.");
        return;
    }

    
    ret = dfl_call_class_method(obj_handle, METHOD_GET_DPU_IP_ADDR, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s call method failed, ret = %d", dfl_get_object_name(obj_handle), ret);
    } else {
        data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
            &data_len, sizeof(guint8));
    }

    if ((gint32)data_len != 22 || data == NULL) { // 22, 应该获取的数据长度
        (void)strcpy_s(ipv4_addr, sizeof(ipv4_addr), "NA");
        (void)strcpy_s(ipv6_addr, sizeof(ipv6_addr), "NA");
        debug_log(DLOG_ERROR, "%s get ip addr failed", dfl_get_object_name(obj_handle));
    } else {
        
        (void)snprintf_s(ipv4_addr, sizeof(ipv4_addr), sizeof(ipv4_addr) - 1, "%d.%d.%d.%d/%d",
            data[0], data[1], data[2], data[3], data[4]); // 0,1,2,3:ipv4, 4:mask
        
        (void)snprintf_s(ipv6_addr, sizeof(ipv6_addr), sizeof(ipv6_addr) - 1,
            "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X/%d",
            data[5], data[6], data[7], data[8], data[9], data[10], // 5,6,7,8,9,10:ipv6
            data[11], data[12], data[13], data[14], data[15], data[16], // 11,12,13,14,15,16:ipv6
            data[17], data[18], data[19], data[20], data[21]); // 17,18,19,20:ipv6, 21:mask
    }
    if (output_list != NULL) {
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    
    property_jso = json_object_new_string(ipv4_addr);
    if (property_jso == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_string for ipv4 fail.");
        return;
    }
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_IP, property_jso);

    
    property_jso = json_object_new_string(ipv6_addr);
    if (property_jso == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_string for ipv6 fail.");
        return;
    }
    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_IPV6, property_jso);
}


static void get_dpucard_extend_storage_port_status(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    gint32 ret;
    guint8 port_status;
    json_object *property_jso = NULL;

    
    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error.", __func__);
        return;
    }

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_DPU_CARD_ETH_LINK_STATUS, &port_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s get port status fail, ret = %d", dfl_get_object_name(obj_handle), ret);
        return;
    }

    
    if (port_status == 0) { // 0, linkup
        property_jso = json_object_new_string("LinkUp");
    } else if (port_status == 1) {  // 1, linkdown
        property_jso = json_object_new_string("LinkDown");
    } else {
        debug_log(DLOG_ERROR, "%s: current link status is unknown.", dfl_get_object_name(obj_handle));
    }

    (void)json_object_object_add(huawei_jso, RFPROP_PCIEDEVICES_STORAGE_MANAGEMENT_PORT_STATUS, property_jso);
}


void get_storage_dpucard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso)
{
    
    
    if (huawei_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error.", __func__);
        return;
    }

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_DPU_CARD_BOARD_NAME,
        RFPROP_PCIEDEVICES_PRODUCT_NAME, huawei_jso);

    
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_DPU_CARD_PCB_VER, RFPROP_PCIEDEVICES_PCB_VERSION, huawei_jso);

     
    (void)get_dpucard_extend_storage_ipaddr(obj_handle, huawei_jso);

    
    (void)get_dpucard_extend_storage_port_status(obj_handle, huawei_jso);
}