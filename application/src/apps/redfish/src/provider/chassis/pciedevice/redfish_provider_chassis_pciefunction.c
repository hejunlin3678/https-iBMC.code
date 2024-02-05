
#include "redfish_provider.h"

LOCAL gint32 get_chassis_pciefunction_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_deviced(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_vendorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_subsystemid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_subsystemvendorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_deviceclass(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunction_functionid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_json_jso);
LOCAL gint32 get_pcieaddrinfo_prop_value(OBJ_HANDLE i_obj_handle, gchar *property_name, guint8 *o_property_value);

LOCAL PROPERTY_PROVIDER_S g_chassis_pciefunction_provider[] = {
    {RFPROP_ODATA_CONTEXT,          MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID,               MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID,              MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME,            MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_name, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_PCIEFUNCTION_DEVICEID, CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_DID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_deviced, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_PCIEFUNCTION_FUNCTIONID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_functionid, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_PCIEFUNCTION_VENDORID,  CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_VID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_vendorid, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_PCIEFUNCTION_SUBSYSTEMID,  CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_SUBDID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_subsystemid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEFUNCTION_SUBSYSTEMVENDORID,  CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_SUBVID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_subsystemvendorid, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_PCIEFUNCTION_DEVICECLASS,  CLASS_PCIECARD_NAME, PROPERTY_PCIECARD_VID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_deviceclass, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_OEM,             MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_oem_property, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PCIEDEVICES_LINKS,          MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunction_links, NULL, NULL, ETAG_FLAG_ENABLE}
};

gint32 get_pci_id_property_uint16(OBJ_HANDLE obj_handle, gchar *property_name, json_object **o_result_jso)
{
    gint32 ret;
    guint16 property_value = 0;
    gchar str_val[MAX_RSC_ID_LEN] = {0};
    
    guint16 vendor_id = 0;
    

    ret = dal_get_property_value_uint16(obj_handle, property_name, &property_value);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get uint16 property val fail.", __FUNCTION__, __LINE__));

    
    if (DRIVE_SMART_INVALID_U16_VALUE == property_value) {
        debug_log(DLOG_MASS, "%s, %d: get property val is invalid.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    

    
    (void)dal_get_property_value_uint16(obj_handle, PROPERTY_PCIECARD_VID, &vendor_id);
    return_val_do_info_if_expr((0 == vendor_id) && (0 == property_value), HTTP_OK, (*o_result_jso = NULL));
    

    ret = snprintf_s(str_val, sizeof(str_val), sizeof(str_val) - 1, "0x%04x", property_value);
    return_val_do_info_if_expr(0 >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(str_val);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_MASS, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 _get_chassis_and_pciedevice_id(GSList *parent_handle_list, gchar *chassis_id, guint32 chassis_id_len,
    gchar *pcie_device_id, guint32 pcie_device_id_len)
{
    OBJ_HANDLE chassis_handle;
    OBJ_HANDLE pcie_card_handle;
    guchar pcie_card_slot = INVALID_VAL;
    gint32 ret;
    guint8 compont_type;

    
    chassis_handle = (OBJ_HANDLE)g_slist_nth_data(parent_handle_list, 0);
    ret = rf_gen_chassis_component_id(NULL, chassis_handle, chassis_id, chassis_id_len);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get board slot id fail.", __FUNCTION__, __LINE__));

    pcie_card_handle = (OBJ_HANDLE)g_slist_nth_data(parent_handle_list, 1);
    return_val_do_info_if_fail(0 != pcie_card_handle, VOS_ERR,
        debug_log(DLOG_ERROR, "cann't get pcie card handle for parent handle list"));

    compont_type = redfish_get_pciedevice_component_type(pcie_card_handle);
    
    ret = dal_get_property_value_byte(pcie_card_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, &pcie_card_slot);
    if (ret != RET_OK || pcie_card_slot == INVALID_VAL) {
        (void)dal_get_property_value_byte(pcie_card_handle, PROPERTY_PCIE_CARD_SLOT_ID, &pcie_card_slot);
    }
    ret = snprintf_s(pcie_device_id, pcie_device_id_len, pcie_device_id_len - 1, "%s%u",
        compont_type == COMPONENT_TYPE_OCP ? RF_OCPCARD_PREFIX : RF_PCIECARD_PREFIX, pcie_card_slot);
    return_val_do_info_if_fail(ret > 0, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: location devicename fail .", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_chassis_pciefunction_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar str_context[MAX_STRBUF_LEN] = {0};
    gchar pcie_memberid[MAX_STRBUF_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = _get_chassis_and_pciedevice_id(i_paras->parent_obj_handle_list, slot_id, MAX_RSC_ID_LEN, pcie_memberid,
        MAX_STRBUF_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed: cann't get chassis or pciedevice id", __FUNCTION__));

    ret = snprintf_s(str_context, sizeof(str_context), sizeof(str_context) - 1, RFPROP_PCIEFUNCTION_METADATA, slot_id,
        pcie_memberid);
    do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(str_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciefunction_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar odata_id[MAX_STRBUF_LEN] = {0};
    gchar pcie_memberid[MAX_STRBUF_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = _get_chassis_and_pciedevice_id(i_paras->parent_obj_handle_list, slot_id, MAX_RSC_ID_LEN, pcie_memberid,
        MAX_STRBUF_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s failed: cann't get chassis or pciedevice id", __FUNCTION__));

    ret = snprintf_s(odata_id, sizeof(odata_id), sizeof(odata_id) - 1, URI_FORMAT_PCIEFUNCTION_URI_STRING, slot_id,
        pcie_memberid, i_paras->member_id);
    do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciefunction_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_string(i_paras->member_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_pciefunction_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar name[MAX_STRBUF_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = snprintf_s(name, sizeof(name), sizeof(name) - 1, "%s%s", RFPROP_PCIEFUNCTION_NAME, i_paras->member_id);
    do_if_expr(0 >= ret, debug_log(DLOG_MASS, "%s %d:snprintf_s fail.", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_string(name);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciefunction_deviced(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return get_pci_id_property_uint16(i_paras->obj_handle,
        g_chassis_pciefunction_provider[i_paras->index].pme_prop_name, o_result_jso);
}

LOCAL gint32 get_chassis_pciefunction_vendorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return get_pci_id_property_uint16(i_paras->obj_handle,
        g_chassis_pciefunction_provider[i_paras->index].pme_prop_name, o_result_jso);
}

LOCAL gint32 get_chassis_pciefunction_subsystemid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return get_pci_id_property_uint16(i_paras->obj_handle,
        g_chassis_pciefunction_provider[i_paras->index].pme_prop_name, o_result_jso);
}

LOCAL gint32 get_chassis_pciefunction_subsystemvendorid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return get_pci_id_property_uint16(i_paras->obj_handle,
        g_chassis_pciefunction_provider[i_paras->index].pme_prop_name, o_result_jso);
}

LOCAL gint32 get_chassis_pciefunction_deviceclass(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 functionclass = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_property_value_byte(i_paras->obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property string value fail.", __FUNCTION__, __LINE__));

    switch (functionclass) {
            
        case PCIECARD_FUNCTION_UNKNOWN:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_OTHER);
            break;

        case PCIECARD_FUNCTION_RAID:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_STORAGE_CONTROLLER);
            break;

        case PCIECARD_FUNCTION_NETCARD:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_NETWORK_CONTROLLER);
            break;

        case PCIECARD_FUNCTION_GPU:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_DISPLAY_CONTROLLER);
            break;

        case PCIECARD_FUNCTION_STORAGE:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_UNCLASSIFIED_DEVICE);
            break;

        case PCIECARD_FUNCTION_SDI:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_INTELLIGENT_CONTROLLER);
            break;

        case PCIECARD_FUNCTION_ACCELERATE:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_PROCESSING_ACCELERATORS);
            break;

        case PCIECARD_FUNCTION_PCIE_RISER:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_OTHER);
            break;

        
        case PCIECARD_FUNCTION_FPGA:
        case PCIECARD_FUNCTION_NPU:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_PROCESSING_ACCELERATORS);
            break;
            
        case PCIECARD_FUNCTION_STORAGE_DPU:
            *o_result_jso = json_object_new_string(RFPROP_PCIEFUNCTION_INTELLIGENT_CONTROLLER);
            break;

        default:
            debug_log(DLOG_MASS, "%s, %d: get function class value is error.", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_pciefunction_functionid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_json_jso)
{
    debug_log(DLOG_INFO, "get_chassis_pciefunction_functionid");
    gint32 ret;
    guint8 function_id = 0;
    // 检查入参,若为空指针则返回内部错误,不填充返回消息体
    if (NULL == o_message_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 检查用户权限：若用户不满足只读权限，则返回403
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 查询function_id
    ret = get_pcieaddrinfo_prop_value(i_paras->obj_handle, PROPERTY_PCIE_FUNCTION, &function_id);
    do_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: get pciefunction function number fail.", __FUNCTION__, __LINE__));

    // 检查function_id是否为0xff,是则返回null
    return_val_if_expr(function_id == INVALID_DATA_BYTE, HTTP_INTERNAL_SERVER_ERROR);

    // 按照schema定义,function_id返回int型整数，因此需要将uint8强制转换成int32
    *o_json_jso = json_object_new_int((gint32)function_id);

    do_if_expr(*o_json_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);
    return HTTP_OK;
}


gint32 get_chassis_pciefunction_oem_associatedresource(OBJ_HANDLE i_obj_handle, json_object **o_json_prop)
{
    gint32 ret;
    guint8 resource_num = 0;
    gchar resource_name[MAX_STRBUF_LEN] = {0};
    guint32 product_id;

    ret = dal_get_property_value_byte(i_obj_handle, PROPERTY_PCIECARD_CPU_ID, &resource_num);
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "%s: get property byte value fail", __FUNCTION__);
        return RET_ERR;
    }

    
    if (resource_num == 0) {
        debug_log(DLOG_MASS, "%s: resource ID is invalid", __FUNCTION__);
        return VOS_ERR;
    } else if (resource_num == RESOURCE_ID_PCIE_SWTICH) {
        
        *o_json_prop = json_object_new_string(RFPROP_PCIEFUNCTION_PCIESWITCH);
    } else if (resource_num == RESOURCE_ID_PCH) {
        
        *o_json_prop = json_object_new_string(RFPROP_PCIEFUNCTION_PCH);
    } else if (resource_num == RESOURCE_ID_CPU1_AND_CPU2) { 
        *o_json_prop = json_object_new_string(RESOURCE_ID_PCIE_CARD_BELONG_TO_CPU1_AND_CPU2_STR);
    } else if (resource_num == RESOURCE_ID_PCIE_CARD_BELONG_TO_ALL_CPU) {
        *o_json_prop = json_object_new_string(RESOURCE_ID_PCIE_CARD_BELONG_TO_ALL_CPU_STR);
    } else {
        ret = dal_get_product_id(&product_id);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "Get software type failed, ret:%d.", ret);
            return ret;
        }
        resource_num = (product_id == PRODUCT_ID_PANGEA_V6) ? (resource_num - 1) : resource_num;

        ret = snprintf_s(resource_name, sizeof(resource_name), sizeof(resource_name) - 1, RFPROP_PCIEFUNCTION_CPUNAME,
            resource_num);
        if (ret <= 0) {
            debug_log(DLOG_MASS, "%s: snprintf_s fail.", __FUNCTION__);
            return VOS_ERR;
        }

        *o_json_prop = json_object_new_string((const char *)resource_name);
    }
    if (*o_json_prop == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 check_netcard_port_bdf_number(OBJ_HANDLE i_obj_handle, guint8 i_bdf_number, gboolean *check_flag)
{
    gint32 ret;
    OBJ_HANDLE netcard_handle = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 bdfnum = 0;
    gchar ref_card_name[MAX_RSC_ID_LEN] = {0};

    
    ret = redfish_check_pciecard_netcard_get_handle(i_obj_handle, &netcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get object handle fail.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get object list fail.", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_string((OBJ_HANDLE)(obj_node->data), BUSY_ETH_ATTRIBUTE_REF_NETCARD, ref_card_name,
            sizeof(ref_card_name));
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s:get ref card fail.", __FUNCTION__));

        if (0 == g_strcmp0(dfl_get_object_name(netcard_handle), ref_card_name)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)(obj_node->data), PROPERTY_BDF_NO, &bdfnum);
            do_if_fail(VOS_OK == ret, continue);
            if (i_bdf_number == bdfnum) {
                *check_flag = TRUE;
                g_slist_free(obj_list);
                return VOS_OK;
            }
        }
    }
    g_slist_free(obj_list);
    return VOS_ERR;
}


LOCAL gint32 get_pcieaddrinfo_prop_value(OBJ_HANDLE i_obj_handle, gchar *property_name, guint8 *o_property_value)
{
    gint32 ret;
    guint8 slot = 0;
    gboolean result_flag = FALSE;
    gboolean check_flag = FALSE;
    guint8 addrinfo_bdf = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 component_type = 0;
    guint8 addrinfo_slot = 0;
    guint8 bdf_number = 0;
    guint8 functionclass = 0;
    guint8 object_component_type;

    
    if (property_name == NULL) {
        debug_log(DLOG_MASS, "%s, %d: input parameter is invalid fail.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    ret = dal_get_property_value_byte(i_obj_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property string value fail.", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(i_obj_handle, PROPETRY_PCIECARD_SLOT, &slot);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property byte value fail.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_PCIE_ADDR_INFO, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property byte value fail.", __FUNCTION__, __LINE__));
    object_component_type = redfish_get_pciedevice_component_type(i_obj_handle);

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_COMPONENT_TYPE, &component_type);
        do_if_fail(VOS_OK == ret, continue);

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_PCIE_SLOT_ID, &addrinfo_slot);
        do_if_fail(VOS_OK == ret, continue);

        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_BDF_NO, &addrinfo_bdf);
        do_if_fail(VOS_OK == ret, continue);

        if ((object_component_type == component_type) && (slot == addrinfo_slot)) {
            
            if (PCIECARD_FUNCTION_NETCARD == functionclass) {
                ret = check_netcard_port_bdf_number(i_obj_handle, addrinfo_bdf, &check_flag);
                do_if_fail(VOS_OK == ret, continue);
                do_if_expr(FALSE == check_flag, continue);

                ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, property_name, &bdf_number);
                do_if_expr(VOS_OK == ret, result_flag = TRUE; break);
            }
            
            ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, property_name, &bdf_number);
            do_if_expr(VOS_OK == ret, result_flag = TRUE; break);
        }
    }
    g_slist_free(obj_list);

    if (result_flag) {
        *o_property_value = bdf_number;
    }
    return VOS_OK;
}


LOCAL gint32 get_chassis_pciefunction_oem_rootbdf(OBJ_HANDLE i_obj_handle, gchar *property_name,
    json_object **o_json_prop)
{
    gint32 ret;
    gchar string_bdf[MAX_STRBUF_LEN] = {0};
    guint8 property_value = 0;

    ret = get_pcieaddrinfo_prop_value(i_obj_handle, property_name, &property_value);
    if (VOS_OK == ret) {
        ret = snprintf_s(string_bdf, sizeof(string_bdf), sizeof(string_bdf) - 1, "0x%02x", property_value);
        return_val_do_info_if_fail(0 < ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__));

        *o_json_prop = json_object_new_string(string_bdf);
        do_if_fail(NULL != *o_json_prop, return VOS_ERR);
    } else {
        debug_log(DLOG_MASS, "%s, %d: get bdf number fail.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }
    return VOS_OK;
}

LOCAL void get_pciefunction_device_bdf(OBJ_HANDLE obj_handle, json_object *json_huawei)
{
    guint8 u8_bus = G_MAXUINT8;
    guint8 u8_device = G_MAXUINT8;
    guint8 u8_function = G_MAXUINT8;
    json_object *device_bdf = NULL;
    json_object *bus_jso = NULL;
    json_object *device_jso = NULL;
    json_object *function_jso = NULL;
    gchar       str_bus[BDF_STR_LEN + 1] = {0};
    gchar       str_device[BDF_STR_LEN + 1] = {0};
    gchar       str_function[BDF_STR_LEN + 1] = {0};
    gint32 ret_val = 0;

    if (NULL == json_huawei) {
        return;
    }

    device_bdf = json_object_new_object();
    json_object_object_add(json_huawei, RFPROP_PCIEFUNCTION_DEVICE_BDF, device_bdf);
    if (NULL == device_bdf) {
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_BUS_NUM, &u8_bus);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_DEV_NUM, &u8_device);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_FUN_NUM, &u8_function);
    
    if (G_MAXUINT8 != u8_bus || G_MAXUINT8 != u8_device || G_MAXUINT8 != u8_function) {
        ret_val = snprintf_s(str_bus, sizeof(str_bus), sizeof(str_bus) - 1, "0x%02x", u8_bus);
        do_if_expr(BDF_STR_LEN == ret_val, (bus_jso = json_object_new_string(str_bus)));

        ret_val = snprintf_s(str_device, sizeof(str_device), sizeof(str_device) - 1, "0x%02x", u8_device);
        do_if_expr(BDF_STR_LEN == ret_val, (device_jso = json_object_new_string(str_device)));

        ret_val = snprintf_s(str_function, sizeof(str_function), sizeof(str_function) - 1, "0x%02x", u8_function);
        do_if_expr(BDF_STR_LEN == ret_val, (function_jso = json_object_new_string(str_function)));
    }
    json_object_object_add(device_bdf, RFPROP_PCIEFUNCTION_D_BUS, bus_jso);
    json_object_object_add(device_bdf, RFPROP_PCIEFUNCTION_D_DEVICE, device_jso);
    json_object_object_add(device_bdf, RFPROP_PCIEFUNCTION_D_FUNCTION, function_jso);

    return;
}

LOCAL void get_ap_chassis_pciefunction_oem_rootbdf(OBJ_HANDLE pcie_handle, json_object *json_str)
{
    gint32 ret;
    OBJ_HANDLE ctrl_handle = 0;
    guint8 u8_bus = G_MAXUINT8;
    guint8 u8_device = G_MAXUINT8;
    guint8 u8_function = G_MAXUINT8;
    gchar       str_bus[BDF_STR_LEN + 1] = {0};
    gchar       str_device[BDF_STR_LEN + 1] = {0};
    gchar       str_func[BDF_STR_LEN + 1] = {0};
    ret = dal_get_specific_object_position(pcie_handle, CLASS_RAID_CONTROLLER_NAME, &ctrl_handle);
    if (ret != RET_OK) {
        (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_BUS, NULL);
        (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_DEVICE, NULL);
        (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_FUNCTION, NULL);
        debug_log(DLOG_DEBUG, "%s:get raidcontroller handle failed,ret:%d", __FUNCTION__, ret);
        return;
    }
    ret = dal_get_property_value_byte(ctrl_handle, PROPERTY_RAID_CONTROLLER_PCI_BUS, &u8_bus);
    if (ret == RET_OK) {
        ret = snprintf_s(str_bus, sizeof(str_bus), sizeof(str_bus) - 1, "0x%02x", u8_bus);
        if (ret > 0) {
            (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_BUS, json_object_new_string(str_bus));
        } else {
            (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_BUS, NULL);
            debug_log(DLOG_DEBUG, "%s:get raidcontroller busnum failed,ret:%d", __FUNCTION__, ret);
        }
    }
    
    ret = dal_get_property_value_byte(ctrl_handle, PROPERTY_RAID_CONTROLLER_PCI_DEVICE, &u8_device);
    if (ret == RET_OK) {
        ret = snprintf_s(str_device, sizeof(str_device), sizeof(str_device) - 1, "0x%02x", u8_device);
        if (ret > 0) {
            (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_DEVICE, json_object_new_string(str_device));
        } else {
            (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_DEVICE, NULL);
            debug_log(DLOG_DEBUG, "%s:get raidcontroller devicenum failed,ret:%d", __FUNCTION__, ret);
        }
    }

    ret = dal_get_property_value_byte(ctrl_handle, PROPERTY_RAID_CONTROLLER_PCI_FUNCTION, &u8_function);
    if (ret == RET_OK) {
        ret = snprintf_s(str_func, sizeof(str_func), sizeof(str_func) - 1, "0x%02x", u8_function);
        if (ret > 0) {
            (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_FUNCTION, json_object_new_string(str_func));
        } else {
            (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_FUNCTION, NULL);
            debug_log(DLOG_DEBUG, "%s:get raidcontroller func failed,ret:%d", __FUNCTION__, ret);
        }
    }
    return;
}


LOCAL void get_chassis_pciefunction_oem_rootbdf_condition(OBJ_HANDLE pcie_handle, json_object *json_str,
    json_object **json_prop)
{
    gint32 ret;
    
    if (dal_check_board_special_me_type() != RET_OK) {
        
        ret = get_chassis_pciefunction_oem_rootbdf(pcie_handle, PROPERTY_PCIE_BUS, json_prop);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s: get pciefunction device bus failed", __FUNCTION__);
        }
        (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_BUS, *json_prop);
        *json_prop = NULL;

        
        ret = get_chassis_pciefunction_oem_rootbdf(pcie_handle, PROPERTY_PCIE_DEVICE, json_prop);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s: get pciefunction device number failed", __FUNCTION__);
        }
        (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_DEVICE, *json_prop);
        *json_prop = NULL;

        
        ret = get_chassis_pciefunction_oem_rootbdf(pcie_handle, PROPERTY_PCIE_FUNCTION, json_prop);
        if (ret != RET_OK) {
            debug_log(DLOG_MASS, "%s: get pciefunction function number failed", __FUNCTION__);
        }
        (void)json_object_object_add(json_str, RFPROP_PCIEFUNCTION_FUNCTION, *json_prop);
    } else {
        get_ap_chassis_pciefunction_oem_rootbdf(pcie_handle, json_str);
    }
    return;
}

LOCAL gint32 get_chassis_pciefunction_oem_property(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *json_huawei = NULL;
    json_object *jso_prop = NULL;
    
    guchar x86_enabled = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enabled);
    return_val_if_expr(DISABLE == x86_enabled, HTTP_INTERNAL_SERVER_ERROR);
    

    json_huawei = json_object_new_object();
    return_val_do_info_if_fail(NULL != json_huawei, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: json object new object fail.", __FUNCTION__, __LINE__));

    
    ret = get_chassis_pciefunction_oem_associatedresource(i_paras->obj_handle, &jso_prop);
    do_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get pciefunction associated resource.", __FUNCTION__, __LINE__));
    (void)json_object_object_add(json_huawei, RFPROP_PCIEFUNCTION_CONNECTEDCPU, jso_prop);
    jso_prop = NULL;

    get_chassis_pciefunction_oem_rootbdf_condition(i_paras->obj_handle, json_huawei, &jso_prop);

    get_pciefunction_device_bdf(i_paras->obj_handle, json_huawei);

    
    rf_add_property_jso_string(i_paras->obj_handle, PROPETRY_PCIECARD_LINK_WIDTH_ABILITY,
        RFPROP_PCIEFUNCTION_PCIE_LINK_WIDTH_ABILITY, json_huawei);
    rf_add_property_jso_string(i_paras->obj_handle, PROPETRY_PCIECARD_LINK_SPEED_ABILITY,
        RFPROP_PCIEFUNCTION_PCIE_LINK_SPEED_ABILITY, json_huawei);
    rf_add_property_jso_string(i_paras->obj_handle, PROPETRY_PCIECARD_LINK_WIDTH, RFPROP_PCIEFUNCTION_PCIE_LINK_WIDTH,
        json_huawei);
    rf_add_property_jso_string(i_paras->obj_handle, PROPETRY_PCIECARD_LINK_SPEED, RFPROP_PCIEFUNCTION_PCIE_LINK_SPEED,
        json_huawei);
    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != json_huawei, HTTP_INTERNAL_SERVER_ERROR, json_object_put(json_huawei);
        debug_log(DLOG_MASS, "%s, %d: json object new object fail.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, json_huawei);
    return HTTP_OK;
}


LOCAL void redfish_get_pciefunction_pciedevice_links(GSList *parent_handle_list, json_object *o_json_prop)
{
    gint32 ret;
    json_object *json_prop = NULL;
    gchar pcie_memberid[MAX_STRBUF_LEN] = {0};
    gchar str_uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};

    ret = _get_chassis_and_pciedevice_id(parent_handle_list, slot, MAX_RSC_ID_LEN + 1, pcie_memberid, MAX_STRBUF_LEN);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s failed:cann't get chassis or pciedevice rsc id", __FUNCTION__));

    ret = snprintf_s(str_uri, sizeof(str_uri), sizeof(str_uri) - 1, URI_FORMAT_CHASSIS_PCIEDEVICES_URI, slot,
        pcie_memberid);
    return_do_info_if_fail(0 < ret, debug_log(DLOG_MASS, "%s,%d:snprintf_s fail.", __FUNCTION__, __LINE__));

    json_prop = json_object_new_string(str_uri);
    return_do_info_if_fail(NULL != json_prop,
        debug_log(DLOG_MASS, "%s, %d: json object new object fail.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(o_json_prop, RFPROP_ODATA_ID, json_prop);
    return;
}


LOCAL gint32 redfish_get_pciefunction_card_add(json_object *o_json_prop, gchar *uri_str, guint8 check_flag)
{
    gint32 ret;
    json_object *json_array = NULL;
    json_object *json_obj = NULL;
    json_object *json_prop = NULL;

    if (NULL == uri_str) {
        debug_log(DLOG_MASS, "%s, %d: input parameter is invalid fail.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    json_array = json_object_new_array();
    return_val_do_info_if_fail(NULL != json_array, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: json object new array fail.", __FUNCTION__, __LINE__));

    json_obj = json_object_new_object();
    return_val_do_info_if_fail(NULL != json_obj, VOS_ERR, json_object_put(json_array);
        debug_log(DLOG_MASS, "%s, %d: json object new array fail.", __FUNCTION__, __LINE__));

    json_prop = json_object_new_string(uri_str);
    return_val_do_info_if_fail(NULL != json_prop, VOS_ERR, json_object_put(json_array); json_object_put(json_obj);
        debug_log(DLOG_ERROR, "%s, %d: json object new array fail.", __FUNCTION__, __LINE__));

    (void)json_object_object_add(json_obj, RFPROP_ODATA_ID, json_prop);

    ret = json_object_array_add(json_array, json_obj);
    do_val_if_fail(0 == ret, json_object_put(json_obj);
        debug_log(DLOG_MASS, "%s, %d: json object array add fail.", __FUNCTION__, __LINE__));

    switch (check_flag) {
        case RFPROP_PCIEFUNCTION_PCIE_RAID:
            (void)json_object_object_add(o_json_prop, RFPROP_STORAGE_CONTROLLERS, json_array);
            break;

        case RFPROP_PCIEFUNCTION_PCIE_SSD:
            (void)json_object_object_add(o_json_prop, RFPROP_CHASSIS_DRIVES, json_array);
            break;

        default:
            json_object_put(json_array);
            return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL void redfish_get_netcard_chipcard_odata(OBJ_HANDLE obj_handle, gchar *slot, gchar *member_id,
    json_object *o_json_prop)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *json_array = NULL;
    json_object *json_obj = NULL;
    json_object *json_prop = NULL;
    OBJ_HANDLE netcard_handle = 0;
    gchar ref_card_name[MAX_RSC_ID_LEN] = {0};
    guint8 BDFNumber = 0;
    guint8 uri_bdfnumber = 0;
    gchar port_memberid[MAX_RSC_ID_LEN] = {0};
    gchar uri_str[MAX_URI_LENGTH] = {0};
    guint32 net_function_type = 0;

    
    ret = redfish_check_pciecard_netcard_get_handle(obj_handle, &netcard_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get object handle fail.", __FUNCTION__, __LINE__));

    ret = vos_str2int(member_id, 10, &uri_bdfnumber, NUM_TPYE_INT32);
    return_do_info_if_expr(VOS_ERR == ret, debug_log(DLOG_MASS, "%s,%d:vos_str2int fail", __FUNCTION__, __LINE__));

    json_array = json_object_new_array();
    return_do_info_if_fail(NULL != json_array,
        debug_log(DLOG_MASS, "%s, %d: json object new array fail.", __FUNCTION__, __LINE__));

    
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, json_object_put(json_array);
        debug_log(DLOG_MASS, "%s, %d: get object list fail.", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_string((OBJ_HANDLE)(obj_node->data), BUSY_ETH_ATTRIBUTE_REF_NETCARD, ref_card_name,
            sizeof(ref_card_name));
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s:get ref card fail.", __FUNCTION__));

        
        ret = dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE,
            &net_function_type);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s:get function type fail.", __FUNCTION__));
        

        
        continue_if_expr((0 == (net_function_type & NETDEV_FUNCTYPE_ETHERNET)) &&
            (0 == (net_function_type & NETDEV_FUNCTYPE_IB)));
        

        if (0 == g_strcmp0(dfl_get_object_name(netcard_handle), ref_card_name)) {
            
            ret = dal_get_property_value_byte((OBJ_HANDLE)(obj_node->data), PROPERTY_BDF_NO, &BDFNumber);
            BDFNumber += 1;
            do_if_fail(BDFNumber == uri_bdfnumber, continue);

            (void)memset_s(port_memberid, sizeof(port_memberid), 0, sizeof(port_memberid));

            ret = redfish_get_sys_eth_memberid((OBJ_HANDLE)(obj_node->data), port_memberid, sizeof(port_memberid));
            do_if_fail(VOS_OK == ret, continue);

            
            
            if (0 == (net_function_type & NETDEV_FUNCTYPE_ETHERNET)) {
                ret = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, URI_FORMAT_SYS_IB, slot, port_memberid);
                do_if_fail(0 < ret, debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__); continue);
            } else {
                ret =
                    snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, URI_FORMAT_SYS_ETH, slot, port_memberid);
                do_if_fail(0 < ret, debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__); continue);
            }
            

            json_prop = json_object_new_string(uri_str);
            do_if_fail(NULL != json_prop, continue);

            json_obj = json_object_new_object();
            do_if_fail(NULL != json_obj, json_object_put(json_prop); continue);

            (void)json_object_object_add(json_obj, RFPROP_ODATA_ID, json_prop);

            ret = json_object_array_add(json_array, json_obj);
            do_val_if_fail(0 == ret, json_object_put(json_obj));
            json_obj = NULL;
            json_prop = NULL;
        }
    }
    g_slist_free(obj_list);

    
    return_do_info_if_expr(0 == json_object_array_length(json_array), json_object_put(json_array));
    

    (void)json_object_object_add(o_json_prop, RF_RESOURCE_ETHERNETINTERFACES, json_array);
    return;
}

LOCAL gint32 redfish_get_pciefunction_card_links(GSList *parent_handle_list, gchar *member_id, json_object *o_json_prop)
{
    gint32 ret;
    guint8 check_flag = 0;
    gchar slave_name[MAX_STRBUF_LEN] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar uri_str[MAX_URI_LENGTH] = {0};
    gchar loc_dev[MAX_STRBUF_LEN] = {0};
    OBJ_HANDLE chassis_handle;
    OBJ_HANDLE pcie_card_handle;
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE controller_handle = 0;
    OBJ_HANDLE hdd_handle = 0;
    guint8 controller_id = 0;
    guint8 functionclass = 0;
    gchar com_devicename[MAX_STRBUF_LEN] = {0};
    guchar x86_enable = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enable);
    return_val_if_fail(ENABLED == x86_enable, VOS_ERR);

    chassis_handle = (OBJ_HANDLE)g_slist_nth_data(parent_handle_list, 0);
    ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot, MAX_RSC_ID_LEN + 1);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    pcie_card_handle = (OBJ_HANDLE)g_slist_nth_data(parent_handle_list, 1);
    ret =
        dal_get_property_value_string(pcie_card_handle, PROPERTY_PCIE_CARD_SLAVE_CARD, slave_name, sizeof(slave_name));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get slave card value fail.", __FUNCTION__, __LINE__));

    
    if (0 == g_strcmp0(slave_name, CLASS_M2TRANSFORMCARD)) {
        debug_log(DLOG_MASS, "%s, %d: get slave card value is invalid.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    ret = dal_get_property_value_byte(pcie_card_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s, %d: get property string value fail.", __FUNCTION__, __LINE__));

    
    if (PCIECARD_FUNCTION_NETCARD == functionclass) {
        
        
        
        
        

        
        (void)redfish_get_netcard_chipcard_odata(pcie_card_handle, (gchar *)slot, member_id, o_json_prop);
        return VOS_OK;
    } else if (PCIECARD_FUNCTION_STORAGE == functionclass) {
        ret = redfish_get_pcie_component_obj_handle(pcie_card_handle, &component_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get component obj handle fail.", __FUNCTION__, __LINE__));

        
        
        ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, com_devicename,
            sizeof(com_devicename));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

        
        ret = dal_get_specific_object_string(CLASS_HDD_NAME, PROPERTY_HDD_DEVICENAME, com_devicename, &hdd_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get hdd_handle fail.", __FUNCTION__, __LINE__));

        
        ret = get_object_obj_location_devicename(component_handle, loc_dev, sizeof(loc_dev));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get component location devicename fail.", __FUNCTION__, __LINE__));

        ret = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, URI_FORMAT_CHASSIS_DRIVES_URI, slot, loc_dev);
        return_val_do_info_if_fail(0 < ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__));
        check_flag = RFPROP_PCIEFUNCTION_PCIE_SSD;
    } else if (PCIECARD_FUNCTION_RAID == functionclass) {
        ret = redfish_get_pcie_component_obj_handle(pcie_card_handle, &component_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get slave card value fail.", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, com_devicename,
            sizeof(com_devicename));
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get property value fail.", __FUNCTION__, __LINE__));

        ret = dal_get_specific_object_string(CLASS_RAID_CONTROLLER_NAME, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME,
            com_devicename, &controller_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get object handle fail.", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_byte(controller_handle, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: get property byte value fail.", __FUNCTION__, __LINE__));

        ret = snprintf_s(uri_str, sizeof(uri_str), sizeof(uri_str) - 1, STORAGE_CONTROLLERS_ODATA_ID_VALUE, slot,
            controller_id, controller_id);
        return_val_do_info_if_fail(0 < ret, VOS_ERR,
            debug_log(DLOG_MASS, "%s, %d: snprintf_s fail.", __FUNCTION__, __LINE__));
        check_flag = RFPROP_PCIEFUNCTION_PCIE_RAID;
    } else {
        debug_log(DLOG_MASS, "%s, %d: get slave card value is invalid.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = redfish_get_pciefunction_card_add(o_json_prop, uri_str, check_flag);
    if (VOS_OK != ret) {
        debug_log(DLOG_MASS, "%s, %d: json object array fail.", __FUNCTION__, __LINE__);
    }

    return VOS_OK;
}


LOCAL gint32 get_chassis_pciefunction_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *jso_pciedevice_odata_id = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_MASS, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    jso_pciedevice_odata_id = json_object_new_object();
    return_val_do_info_if_fail(NULL != jso_pciedevice_odata_id, HTTP_INTERNAL_SERVER_ERROR,
        json_object_put(*o_result_jso);
        *o_result_jso = NULL);

    (void)redfish_get_pciefunction_pciedevice_links(i_paras->parent_obj_handle_list, jso_pciedevice_odata_id);
    (void)json_object_object_add(*o_result_jso, RFPROP_CHASSIS_PCIEDEVICES, jso_pciedevice_odata_id);

    ret = redfish_get_pciefunction_card_links(i_paras->parent_obj_handle_list, i_paras->member_id, *o_result_jso);
    do_if_fail(VOS_OK == ret,
        debug_log(DLOG_MASS, "%s, %d: get card function odata id fail.\n", __FUNCTION__, __LINE__));
    return HTTP_OK;
}

LOCAL gboolean check_pcie_netcard_uri_effective(const gchar *slot, OBJ_HANDLE pcie_handle, const gchar *i_uri,
    const gchar *pcie_memberid)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 bdfnum = 0;
    const gchar *object_name = NULL;
    gchar ref_card_name[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE netcard_handle = 0;

    
    ret = redfish_check_pciecard_netcard_get_handle(pcie_handle, &netcard_handle);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_MASS, "%s, %d: get object handle fail.", __FUNCTION__, __LINE__));

    
    object_name = dfl_get_object_name(netcard_handle);
    if (!object_name) {
        debug_log(DLOG_MASS, "%s, %d: get object name fail.", __FUNCTION__, __LINE__);
        return FALSE;
    }

    
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_MASS, "%s, %d: get object list fail.", __FUNCTION__, __LINE__));

    
    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_string((OBJ_HANDLE)(obj_node->data), BUSY_ETH_ATTRIBUTE_REF_NETCARD, ref_card_name,
            sizeof(ref_card_name));
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_MASS, "%s:get ref card fail.", __FUNCTION__));

        if (0 == g_strcmp0(object_name, ref_card_name)) {
            ret = dal_get_property_value_byte((OBJ_HANDLE)(obj_node->data), PROPERTY_BDF_NO, &bdfnum);
            do_if_fail(VOS_OK == ret, continue);
            
            bdfnum += 1;

            gchar function_uri[MAX_STRBUF_LEN] = {0};
            ret = snprintf_s(function_uri, sizeof(function_uri), sizeof(function_uri) - 1,
                URI_FORMAT_PCIEFUNCTION_URI_NUMBER, slot, pcie_memberid, bdfnum);
            do_if_fail(0 < ret, debug_log(DLOG_MASS, "%s, %d : snprintf_s fail.", __FUNCTION__, __LINE__); continue);

            if (0 == g_ascii_strcasecmp(i_uri, function_uri)) {
                g_slist_free(obj_list);
                return TRUE;
            }
        }
    }

    g_slist_free(obj_list);
    return FALSE;
}

LOCAL gboolean check_uri_effective(OBJ_HANDLE pcie_card_handle, const gchar *i_uri, const gchar *slot,
    const gchar *pcie_memberid, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    gchar function_uri[MAX_STRBUF_LEN] = {0};
    guint8 functionclass = PCIECARD_FUNCTION_UNKNOWN;

    
    ret = dal_get_property_value_byte(pcie_card_handle, PROPERTY_PCIECARD_FUNCTIONCLASS, &functionclass);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get property value for %s failed, ret is %d.", PROPERTY_PCIECARD_FUNCTIONCLASS, ret);
        return FALSE;
    }

    
    if (functionclass == PCIECARD_FUNCTION_NETCARD) {
        gboolean result = check_pcie_netcard_uri_effective((const gchar *)slot, pcie_card_handle, i_uri, pcie_memberid);
        if (result == TRUE) {
            *o_obj_handle = pcie_card_handle;
        }
        return result;
    }

    
    ret = snprintf_s(function_uri, sizeof(function_uri), sizeof(function_uri) - 1,
        URI_FORMAT_PCIEFUNCTION_URI_NUMBER, slot, pcie_memberid, RFPROP_PCIEFUNCTION_MEMBERID);
    if (ret <= EOK) {
        debug_log(DLOG_ERROR, "%s:snprintf_s fail, ret is %d.", __FUNCTION__, ret);
        return FALSE;
    }

    
    if (g_ascii_strcasecmp(i_uri, function_uri) != 0) {
        return FALSE;
    }

    *o_obj_handle = pcie_card_handle;
    return TRUE;
}

LOCAL gboolean redfish_check_chassis_pciefunction_uri_effective(gchar *i_uri, GSList **parent_handle_list,
    OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    gboolean result;
    gchar pcie_memberid[MAX_RSC_ID_LEN] = {0};
    guint8 pcie_presence = 0;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    OBJ_HANDLE chassis_handle = 0;
    OBJ_HANDLE pcie_card_handle = 0;
    guchar pcie_card_slot = INVALID_VAL;
    const gchar *card_prefix_ptr = NULL;

    return_val_do_info_if_fail((NULL != i_uri) && (NULL != parent_handle_list) && (NULL != o_obj_handle), FALSE,
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    result = redfish_check_chassis_uri_valid(i_uri, &chassis_handle);
    return_val_do_info_if_fail(TRUE == result, result, debug_log(DLOG_ERROR, "check chassis uri failed"));

    
    return_val_if_fail(VOS_OK == check_enclosure_component_type(chassis_handle, FALSE), FALSE);
    

    *parent_handle_list = g_slist_append(*parent_handle_list, (gpointer)chassis_handle);

    ret = rf_gen_chassis_component_id(NULL, chassis_handle, slot, MAX_RSC_NAME_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, FALSE,
        debug_log(DLOG_MASS, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    result = redfish_check_chassis_pciedevice_uri_effective(i_uri, &pcie_card_handle);
    return_val_do_info_if_fail(TRUE == result, result,
        debug_log(DLOG_ERROR, "%s failed: check pcie card resource failed", __FUNCTION__));

    *parent_handle_list = g_slist_append(*parent_handle_list, (gpointer)pcie_card_handle);

    ret = dal_get_property_value_byte(pcie_card_handle, PROPETRY_PCIECARD_PRESENCE, &pcie_presence);
    
    return_val_do_info_if_fail((VOS_OK == ret) && (1 == pcie_presence), FALSE,
        debug_log(DLOG_DEBUG, "get pcie card function failed, pcie card resource absent"));
    
    

    card_prefix_ptr = get_chassis_pciedevices_cardtype(pcie_card_handle);

    (void)dal_get_silk_id(pcie_card_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, PROPETRY_PCIECARD_SLOT, &pcie_card_slot);

    ret = snprintf_s(pcie_memberid, MAX_RSC_ID_LEN, MAX_RSC_ID_LEN - 1, "%s%u", card_prefix_ptr, pcie_card_slot);
    return_val_do_info_if_fail(ret > 0, FALSE, debug_log(DLOG_ERROR, "format pcie card id failed, ret is %d", ret));

    return check_uri_effective(pcie_card_handle, i_uri, slot, pcie_memberid, o_obj_handle);
}


gint32 chassis_pciefunction_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    if (redfish_check_chassis_pciefunction_uri_effective(i_paras->uri, &(i_paras->parent_obj_handle_list),
        &(i_paras->obj_handle))) {
        *prop_provider = g_chassis_pciefunction_provider;
        *count = sizeof(g_chassis_pciefunction_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}
