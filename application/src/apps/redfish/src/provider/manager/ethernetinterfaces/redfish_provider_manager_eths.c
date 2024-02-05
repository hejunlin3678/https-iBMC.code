

#include "redfish_provider.h"

LOCAL gint32 get_smm_manager_ethernetinterfaces_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_smm_manager_ethernetinterfaces_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_manager_ethernetinterfaces_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterfaces_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterfaces_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_manager_ethernetinterfaces_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_manager_eths_provider[] = {
    {RFPROP_MEMBERS_COUNT, ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_MAC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterfaces_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_MAC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterfaces_members, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterfaces_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_ethernetinterfaces_odata_context, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_manager_ethernetinterfaces_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret = 0;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    guint8 software_type = 0;

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_software_type(&software_type);
    if ((MGMT_SOFTWARE_TYPE_EM == software_type) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) {
        get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, slot_id, sizeof(slot_id));
    } else {
        
        ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));
    }
    
    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, MANAGERS_ETHS_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_manager_ethernetinterfaces_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret = 0;
    gchar slot[MAX_RSC_ID_LEN] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    guint8 software_type = 0;

    if (NULL == o_message_jso || NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_software_type(&software_type);
    if ((MGMT_SOFTWARE_TYPE_EM == software_type) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) { // 管理板
        get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, slot, sizeof(slot));
    } else {
        
        ret = redfish_get_board_slot(slot, sizeof(slot));
        return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);
    }
    
    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_ETHS, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL gint32 get_smm_manager_ethernetinterfaces_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    guint8 smm_index = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    guint32 presence = 0;
    gint32 count = 0;

    get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
    // 获取本板对象句柄
    ret = dal_get_object_handle_nth(CLASS_SMM, 0, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object of class %s failed, err code %d", __func__, CLASS_SMM, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 获取本板索引号
    (void)dal_get_property_value_byte(handle, PROPERTY_INDEX, &smm_index);
    if (smm_index == smm_name[3] - '0') { // 读取本板网口数目，本板为主，网口数目为2
        count = 2;
    } else { // 备用板在位则网口数为1，不在位则为0
        (void)get_other_board_present(&presence);
        if (presence == BLADE_PRESENCE) {
            count = 1;
        }
    }
    *o_result_jso = json_object_new_int(count);
    return HTTP_OK;
}


LOCAL gint32 get_manager_ethernetinterfaces_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 count = 0;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 out_type = 0;
    guint8 software_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    (void)dal_get_software_type(&software_type);
    
    
    if ((software_type == MGMT_SOFTWARE_TYPE_EM) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) {
        ret = get_smm_manager_ethernetinterfaces_count(i_paras, o_message_jso, o_result_jso);
        return ret;
    }


    
    ret = dfl_get_object_list(g_manager_eths_provider[i_paras->index].pme_class_name, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);
        continue_if_expr((out_type != OUTTER_GROUP_TYPE) && (out_type != FLOATIP_ETHERNET));
        (void)memset_s(mac_addr, sizeof(mac_addr), 0, sizeof(mac_addr));
        
        get_ethernetinterface_id((OBJ_HANDLE)obj_node->data, out_type, mac_addr, sizeof(mac_addr));
        do_if_expr(0 == strlen(mac_addr), continue);

        count++;
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 get_smm_manager_ethernetinterfaces_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gchar smm_name[MAX_NAME_SIZE] = { 0 };
    guint8 smm_index = 0;
    OBJ_HANDLE handle = 0;
    gint32 ret;
    guint32 presence = 0;
    json_object *interface_obj = NULL;
    gchar uri[MAX_URI_LEN] = { 0 };

    // URI : /redfish/v1/Managers/HMM[12]/EthernetInterfaces
    get_smm_name_from_redfish_uri(RF_MANAGERS_URI, i_paras->uri, smm_name, sizeof(smm_name));
    ret = dal_get_object_handle_nth(CLASS_SMM, 0, &handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "[%s] Get object of class %s failed, err code %d", __func__, CLASS_SMM, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 获取本板索引号
    (void)dal_get_property_value_byte(handle, PROPERTY_INDEX, &smm_index);
    // 添加网口成员
    if (smm_index == smm_name[3] - '0') { // 读取本板网口数目，本板为主，包含静态网口和浮动网口
        // Static
        interface_obj = json_object_new_object();
        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_ETH, smm_name, "StaticEth2");
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        json_object_object_add(interface_obj, RFPROP_ODATA_ID, json_object_new_string(uri));
        json_object_array_add(*o_result_jso, interface_obj);
        // Float
        interface_obj = json_object_new_object();
        iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_ETH, smm_name, "FloatEth2");
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        json_object_object_add(interface_obj, RFPROP_ODATA_ID, json_object_new_string(uri));
        json_object_array_add(*o_result_jso, interface_obj);
    } else { // 备用板在位则有静态网口，否则为空
        (void)get_other_board_present(&presence);
        if (presence == BLADE_PRESENCE) {
            // Static
            interface_obj = json_object_new_object();
            iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_MANAGER_ETH, smm_name, "StaticEth2");
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            json_object_object_add(interface_obj, RFPROP_ODATA_ID, json_object_new_string(uri));
            json_object_array_add(*o_result_jso, interface_obj);
        }
    }
    return HTTP_OK;
}
LOCAL gboolean is_outter_group_obj(OBJ_HANDLE obj_handle)
{
    gint32 ret;
    OBJ_HANDLE outter_group_obj = 0;

    ret = dal_eth_get_out_type_object(OUTTER_GROUP_TYPE, &outter_group_obj);
    if (ret != RET_OK) {
        return FALSE;
    }

    if (obj_handle != outter_group_obj) {
        return FALSE;
    }

    return TRUE;
}


void get_ethernetinterface_id(OBJ_HANDLE obj_handle, guint8 out_type, gchar *ethernetinterface_id, guint32 lenth)
{
    gint32 retv = VOS_OK;
    guint32 vlan_state = 0;
    guint16 vlan_id = 0;
    guint8 mac_source_port = 0;
    guint8 mgnt_plane = 0;
    guint8 as_status = 0xff;

    
    if (out_type == FLOATIP_ETHERNET) {
        (void)get_board_active_state(&as_status);
        if (as_status == ASM_ACTIVE) {
            do_if_expr((lenth < 1), return );
            retv = snprintf_s(ethernetinterface_id, lenth, lenth - 1, MGNT_FLOAT_IP_STR);
            do_val_if_expr(retv <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s1 fail, ret = %d", __FUNCTION__, retv));
        }
        return;
    }

    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_MAC_SOURCE_PORT, &mac_source_port);
        (void)dal_get_property_value_uint32(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_STATE, &vlan_state);
        (void)dal_get_property_value_uint16(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_ID, &vlan_id);

        do_if_expr((lenth < 1), return );
        
        if (is_outter_group_obj(obj_handle)) {
            retv = snprintf_s(ethernetinterface_id, lenth, lenth - 1, MGNT_DEFAULT_IP_STR);
            do_val_if_expr(retv <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s2 fail, ret = %d", __FUNCTION__, retv));
            return;
        }

        
        mgnt_plane = mac_source_port - 1;

        if (vlan_state == FALSE) {
            retv = snprintf_s(ethernetinterface_id, lenth, lenth - 1, MGNT_PLANE_STR, mgnt_plane);
            do_val_if_expr(retv <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s3 fail, ret = %d", __FUNCTION__, retv));
        } else {
            
            retv = snprintf_s(ethernetinterface_id, lenth, lenth - 1, MGNT_PLANE_STR_VLAN, mgnt_plane, vlan_id);
            do_val_if_expr(retv <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s4 fail, ret = %d", __FUNCTION__, retv));
        }
    } else {
        (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_MAC, ethernetinterface_id, lenth);
        do_if_expr(0 == strlen(ethernetinterface_id), return );

        
        (void)redfish_mac_addr2uriid(ethernetinterface_id, lenth);
    }
    return;
}


LOCAL gint32 get_manager_ethernetinterfaces_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};
    gchar eth_uri[ETHS_URI_LEN + MACADDRESS_LEN + 2] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;
    guint8 out_type = 0;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    guint8 software_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();
    (void)dal_get_software_type(&software_type);
    
    if ((MGMT_SOFTWARE_TYPE_EM == software_type) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) {
        ret = get_smm_manager_ethernetinterfaces_members(i_paras, o_message_jso, o_result_jso);
        return ret;
    }

    
    ret = dfl_get_object_list(g_manager_eths_provider[i_paras->index].pme_class_name, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));
    
    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);
        continue_if_expr_true((out_type != OUTTER_GROUP_TYPE) && (out_type != FLOATIP_ETHERNET)); 
        (void)memset_s(mac_addr, sizeof(mac_addr), 0, sizeof(mac_addr));
        
        get_ethernetinterface_id((OBJ_HANDLE)obj_node->data, out_type, mac_addr, sizeof(mac_addr));
        do_if_expr(0 == strlen(mac_addr), continue);
        
        iRet = snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, RFPROP_MANAGER_ETH, slot, mac_addr);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        obj_jso = json_object_new_object();
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(eth_uri));
        
        json_object_array_add(*o_result_jso, obj_jso);
        obj_jso = NULL;
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}

gint32 manager_ethernetinterfaces_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean ret;
    guchar NetConfigEnable = 0;
    guint8 software_type = 0;

    
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_NETCONFIG, &NetConfigEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d: get_NetConfigEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail((SERVICE_NETCONFIG_ENABLE == NetConfigEnable), HTTP_NOT_FOUND);


    (void)dal_get_software_type(&software_type);
    if ((MGMT_SOFTWARE_TYPE_EM == software_type) &&
        (!dal_is_support_eth_function_specific_propery(PROPERTY_VLAN_STATUS, 0))) {
        ret = redfish_check_smm_manager_uri_valid(i_paras->uri);
    } else {
        ret = redfish_check_manager_uri_valid(i_paras->uri);
    }
    
    if (0 == ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_manager_eths_provider;
    *count = sizeof(g_manager_eths_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


void get_system_overview_mac(json_object **o_result_jso)
{
    gint32 ret;
    gchar mac_addr[MACADDRESS_LEN + 1] = {0};
    OBJ_HANDLE obj_handle = 0;

    guint8 software_type = 0;

    (void)dal_get_software_type(&software_type);
    if (MGMT_SOFTWARE_TYPE_EM == software_type) {
        *o_result_jso = NULL;
        return;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUTTER_GROUP_TYPE,
        &obj_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: dal_get_specific_object_byte fail.", __FUNCTION__, __LINE__));

    (void)memset_s(mac_addr, sizeof(mac_addr), 0, sizeof(mac_addr));

    
    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_MAC, mac_addr, sizeof(mac_addr));
    *o_result_jso = json_object_new_string(mac_addr);

    return;
}
