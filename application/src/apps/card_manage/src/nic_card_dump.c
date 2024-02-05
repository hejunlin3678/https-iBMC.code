


#include "pme_app/pme_app.h"


#include "pcie_card.h"


LOCAL gint32 print_nic_optical_module_head_flie(FILE *fp, const gchar *title)
{
    gint32 ret = fprintf(fp, "%s\n", title);
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s:fprintf failed.", __FUNCTION__);
        return RET_ERR;
    }
 
    ret = fprintf(fp,
        "%-32s | %-12s | %-8s | %-12s | %-12s | %-14s | %-18s | %-30s | %-18s\n",
        "Net Card Name", "Port Name", "BMA Id", "Present", "Vendor Name", "Serial Number",
        "Mac Addr", "Actual Mac", "Link Status");
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s:fprintf failed.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}
 

LOCAL gint32 get_optical_module_info(OBJ_HANDLE obj_handle, FILE *fp, OBJ_HANDLE net_card_handle)
{
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;
    gchar port_name[PROP_VAL_LENGTH] = {0};
    const gchar *bma_id = NULL;
    guint8 present = 0;
    const gchar *vendor_name = NULL;
    const gchar *serial_number = NULL;
    gchar mac_addr[PROP_VAL_LENGTH] = {0};
    gchar actual_mac[PROP_VAL_LENGTH] = {0};
    guint8 link_status = 0;
    OBJ_HANDLE port_handle = 0;
 
    
    gint32 ret = dfl_get_referenced_object(obj_handle, PROPERTY_OPT_MDL_REF_PORT_OBJ, &port_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s:dfl_get_referenced_object %s failed", __FUNCTION__, PROPERTY_OPT_MDL_REF_PORT_OBJ);
        return RET_ERR;
    }
    (void)dal_get_property_value_string(port_handle, BUSY_ETH_ATTRIBUTE_NAME, port_name, sizeof(port_name));
    (void)dal_get_property_value_string(port_handle, BUSY_ETH_GROUP_ATTRIBUTE_MAC, mac_addr, sizeof(mac_addr));
    (void)dal_get_property_value_string(port_handle, BUSY_ETH_ATTRIBUTE_ACTUAL_MAC_ADDR, actual_mac,
        sizeof(actual_mac));
    (void)dal_get_property_value_byte(port_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &link_status);
 
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_BMAID);
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_PRESENT);
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_VENDOR_NAME);
    property_name_list = g_slist_append(property_name_list, PROPERTY_OPT_MDL_SERIAL_NUMBER);
 
    ret = dfl_multiget_property_value(obj_handle, property_name_list, &property_value_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_multiget_property_value failed", __FUNCTION__);
        goto EXIT;
    }
    
    bma_id = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 0), NULL);
    present = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));
    vendor_name = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 2), NULL);     // 2代表对应列表索引
    serial_number = g_variant_get_string((GVariant *)g_slist_nth_data(property_value_list, 3), NULL);   // 3代表对应列表索引
 
    ret = fprintf(fp, "%-32s | %-12s | %-8s | %-12u | %-12s | %-14s | %-18s | %-30s | %-18u\n",
        dfl_get_object_name(net_card_handle), port_name, bma_id, present, vendor_name, serial_number,
        mac_addr, actual_mac, link_status);
 
EXIT:
    g_slist_free(property_name_list);
    property_name_list = NULL;
    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    property_value_list = NULL;
    if (ret < 0) {
        debug_log(DLOG_ERROR, "%s:fprintf failed.", __FUNCTION__);
        return RET_ERR;
    }
    return RET_OK;
}
 

LOCAL gint32 nic_get_optical_module_info(FILE *fp, const gchar *class_name)
{
    GSList *net_card_list = NULL;
    GSList *card_node = NULL;
    OBJ_HANDLE net_card_handle = 0;
    guint8 card_type = 0;
    GSList *opt_node = NULL;
    GSList *optical_module_list = NULL;
    
    gint32 ret = dfl_get_object_list(CLASS_NETCARD_NAME, &net_card_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:dfl_get_object_list CLASS_NETCARD_NAME fail(Ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }
 
    for (card_node = net_card_list; card_node; card_node = card_node->next) {
        net_card_handle = (OBJ_HANDLE)card_node->data;
        
        (void)dal_get_property_value_byte(net_card_handle, PROPERTY_NETCARD_CARDTYPE, &card_type);
        if (card_type != NET_TYPE_LOM) {
            continue;
        }
 
        
        ret = dal_get_object_list_position(net_card_handle, class_name, &optical_module_list);
 
        
        for (opt_node = optical_module_list; ret == RET_OK && opt_node; opt_node = g_slist_next(opt_node)) {
            ret = get_optical_module_info((OBJ_HANDLE)opt_node->data, fp, net_card_handle);
        }
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get optical module info or optical_module_list failed, class name is:%s",
                __FUNCTION__, class_name);
        }
 
        g_slist_free(optical_module_list);
        optical_module_list = NULL;
    }
    (void)fprintf(fp, "\n\n");
 
    g_slist_free(net_card_list);
    net_card_list = NULL;
    return ret;
}
 

gint32 nic_card_dump_info(const gchar *class_name, const gchar *title, const gchar *path)
{
    FILE *fp = NULL;
    gchar file_name[NIC_CARD_DUMPINFO_MAX_LEN + 1] = {0};
 
    if (path == NULL || class_name == NULL || title == NULL) {
        debug_log(DLOG_ERROR, "%s:get path or class_name or title failed.", __FUNCTION__);
        return RET_ERR;
    }
 
    
    gint32 ret = snprintf_s(file_name, NIC_CARD_DUMPINFO_MAX_LEN + 1, NIC_CARD_DUMPINFO_MAX_LEN, "%s/card_info", path);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
 
    fp = dal_fopen_check_realpath(file_name, "a+", file_name);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s:Open card_info failed", __FUNCTION__);
        return RET_ERR;
    }
 
    (void)fchmod(fileno(fp), COMMON_DUMP_FILE_UMASK);
    
    ret = print_nic_optical_module_head_flie(fp, title);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:print_nic_optical_module_head_flie failed, class_name is:%s",
            __FUNCTION__, class_name);
        (void)fclose(fp);
        return RET_ERR;
    }
 
    ret = nic_get_optical_module_info(fp, class_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:nic_get_optical_module_info failed, class_name is:%s", __FUNCTION__, class_name);
        (void)fclose(fp);
        return RET_ERR;
    }
 
    (void)fclose(fp);
    return RET_OK;
}
