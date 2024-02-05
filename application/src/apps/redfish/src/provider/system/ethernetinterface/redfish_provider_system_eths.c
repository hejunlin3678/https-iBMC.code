

#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_ethernetinterfaces_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterfaces_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL gint32 get_system_ethernetinterfaces_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_ethernetinterfaces_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL PROPERTY_PROVIDER_S g_system_eths_provider[] = {
    {RFPROP_MEMBERS_COUNT, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_GROUP_ATTRIBUTE_MAC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterfaces_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_GROUP_ATTRIBUTE_MAC, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterfaces_members, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterfaces_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterfaces_odata_context, NULL, NULL, ETAG_FLAG_ENABLE}
};

LOCAL gint32 get_system_ethernetinterfaces_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, SYSTEMS_ETHS_METADATA, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_ethernetinterfaces_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));

    
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));
    

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, RFPROP_SYSTEM_ETHS, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterfaces_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint32 net_dev_function = 0;
    guint8 port_type = 0;
    gint32 ret = 0;
    OBJ_HANDLE referencd_handle = 0;
    guchar virtual_flag = NOT_AVAILABLE;
    gchar member_id[PROP_VAL_LENGTH] = { 0 };
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    

    
    (void)dfl_get_object_list(g_system_eths_provider[i_paras->index].pme_class_name, &obj_list);
    

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        net_dev_function = 0;
        
        virtual_flag = NOT_AVAILABLE;
        
        // 以太网显示
        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE,
            &net_dev_function);
        
        
        
        continue_if_expr(0 == (net_dev_function & NETDEV_FUNCTYPE_ETHERNET));
        
        
        
        continue_if_expr(is_lom_hide_phy_port((OBJ_HANDLE)obj_node->data) == TRUE);

        // 类型既不是物理网口也不是逻辑网口，过滤掉
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
        // 认为0xff是没bma的情况下，物理网口的值
        continue_if_expr((ETH_PORT_TYPE_VIRTUAL != port_type) && (ETH_PORT_TYPE_PHYSICAL != port_type) &&
            (0xff != port_type));

        
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &referencd_handle);
        
        if (VOS_OK == ret) {
            ret = dal_get_property_value_byte(referencd_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
            continue_do_info_if_expr(VOS_OK != ret, debug_log(DLOG_ERROR, "%s : get virtual flag fail ", __FUNCTION__));
        }
        
        ret = redfish_get_sys_eth_memberid((OBJ_HANDLE)obj_node->data, member_id, PROP_VAL_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG, "%s : get member id fail ", __FUNCTION__));

        
        if (AVAILABLE != virtual_flag) {
            count++;
        }
        
    }

    
    *o_result_jso = json_object_new_int(count);
    g_slist_free(obj_list);
    return HTTP_OK;
}


LOCAL gint32 formate_port_member_id(OBJ_HANDLE obj_handle, gchar *memberid, gint32 mem_id_len)
{
    gint32 ret;
    OBJ_HANDLE netcard_obj;
    OBJ_HANDLE component_obj;
    guint8 slik_num = 0;
    gchar card_location[PROP_VAL_LENGTH] = {0};
    gchar device_name[PROP_VAL_LENGTH] = {0};

    ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: get %s ref netcard fail", __FUNCTION__, dfl_get_object_name(obj_handle));
        return ret;
    }

    ret = dfl_get_referenced_object(netcard_obj, PROPERTY_NETCARD_REF_COMPONENT, &component_obj);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get %s ref component fail", __FUNCTION__, dfl_get_object_name(netcard_obj));
        return ret;
    }

    (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &slik_num);
    (void)dal_get_property_value_string(component_obj, PROPERTY_COMPONENT_LOCATION, card_location, PROP_VAL_LENGTH);
    (void)dal_get_property_value_string(component_obj, PROPERTY_COMPONENT_DEVICE_NAME, device_name, PROP_VAL_LENGTH);
    (void)dal_clear_string_blank(card_location, sizeof(card_location));
    (void)dal_clear_string_blank(device_name, sizeof(device_name));

    ret = snprintf_s(memberid, mem_id_len, mem_id_len - 1, "%s%sPort%u", card_location, device_name, slik_num);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

gint32 redfish_get_sys_eth_memberid(OBJ_HANDLE obj_handle, gchar *memberid, gint32 mem_id_len)
{
    // member id生成规则
    // 物理网口:  (关联网卡在Component中对象的Location+DeviceName)和port+SlikNum。示例:mainboardNIC1Port1
    // 逻辑网口:  OSEthName属性    示例:bond0
    guint8 port_type = 0;
    gchar os_eth_name[PROP_VAL_LENGTH] = {0};
    errno_t str_ret = EOK;
    guint8 slik_num = 0;
    guint8 card_type = 0xff;

    return_val_do_info_if_expr(NULL == memberid, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);

    if (ETH_PORT_TYPE_VIRTUAL == port_type) {
        
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        if (BUSINESSPORT_CARD_TYPE_TEAM == card_type || BUSINESSPORT_CARD_TYPE_BRIDGE == card_type) {
            // team/bridge网口对象中BDF属性保存的为BMA上报的逻辑编号，对应BMA资源中URLId属性
            // 此处用此作为URI为与龙哥讨论一致结论，避免OS上读到的真实网口名称存在空格、中文等特殊字符
            (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, os_eth_name, PROP_VAL_LENGTH);
        } else {
            (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_OSETH_NAME, os_eth_name,
                PROP_VAL_LENGTH);
        }

        
        if (!g_strcmp0(os_eth_name, PROPERTY_VALUE_DEFAULT_STRING)) {
            return RET_ERR;
        }
        
        str_ret = strncpy_s(memberid, mem_id_len, os_eth_name, PROP_VAL_LENGTH - 1);
        
        return_val_do_info_if_fail(EOK == str_ret, VOS_ERR, debug_log(DLOG_ERROR, "%s: strncpy_s fail", __FUNCTION__));

        return VOS_OK;
    } else if ((ETH_PORT_TYPE_PHYSICAL == port_type) || (0xff == port_type)) { // 0xff出现在没bma的情况下
        
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &slik_num);
        // 对于丝印号是255 的物理网口(只可能是bma报过来的)，member id也用 os eth name
        if (0xff == slik_num) {
            (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_OSETH_NAME, os_eth_name,
                PROP_VAL_LENGTH);
            
            if (!g_strcmp0(os_eth_name, PROPERTY_VALUE_DEFAULT_STRING)) {
                return RET_ERR;
            }

            str_ret = strncpy_s(memberid, mem_id_len, os_eth_name, PROP_VAL_LENGTH - 1);
            
            return_val_do_info_if_fail(EOK == str_ret, VOS_ERR,
                debug_log(DLOG_ERROR, "%s: strncpy_s fail", __FUNCTION__));

            return VOS_OK;
        }
        
        return formate_port_member_id(obj_handle, memberid, mem_id_len);
    } else {
        debug_log(DLOG_ERROR, "%s: invalid port type : %d", __FUNCTION__, port_type);
    }

    return VOS_ERR;
}


LOCAL gint32 get_system_ethernetinterfaces_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet = -1;
    gint32 ret = 0;
    gchar eth_uri[ETHS_URI_LEN + MACADDRESS_LEN + 2] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    guint32 net_dev_function = 0;
    gchar member_id[PROP_VAL_LENGTH] = {0};
    guint8 port_type = 0;
    OBJ_HANDLE referencd_handle = 0;
    guchar virtual_flag = NOT_AVAILABLE;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    (void)dfl_get_object_list(g_system_eths_provider[i_paras->index].pme_class_name, &obj_list);
    

    
    *o_result_jso = json_object_new_array();
    
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "%s:json_object_new_array error", __FUNCTION__));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        virtual_flag = NOT_AVAILABLE;
        
        
        
        // 以太网显示
        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE,
            &net_dev_function);
        
        
        continue_if_expr(0 == (net_dev_function & NETDEV_FUNCTYPE_ETHERNET));
        
        
        

        continue_if_expr(is_lom_hide_phy_port((OBJ_HANDLE)obj_node->data) == TRUE);

        // 类型既不是物理网口也不是逻辑网口，过滤掉
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
        // 认为0xff是没bma的情况下，物理网口的值
        continue_if_expr((ETH_PORT_TYPE_VIRTUAL != port_type) && (ETH_PORT_TYPE_PHYSICAL != port_type) &&
            (0xff != port_type));
        
        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &referencd_handle);
        
        if (VOS_OK == ret) {
            ret = dal_get_property_value_byte(referencd_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
            continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s : get virtual flag fail ", __FUNCTION__));
        }
        
        if (AVAILABLE != virtual_flag) {
            ret = redfish_get_sys_eth_memberid((OBJ_HANDLE)obj_node->data, member_id, PROP_VAL_LENGTH);
            continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s : get member id fail ", __FUNCTION__));
            
            
            ret = redfish_get_board_slot(slot, sizeof(slot));
            iRet = snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, URI_FORMAT_SYS_ETH, slot, member_id);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            
            obj_jso = json_object_new_object();

            
            return_val_do_info_if_expr(
                NULL == obj_jso, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso); *o_result_jso = NULL;
                g_slist_free(obj_list); debug_log(DLOG_ERROR, "%s:json_object_new_object error", __FUNCTION__));
            
            json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(eth_uri));
            ret = json_object_array_add(*o_result_jso, obj_jso);
            
            return_val_do_info_if_expr(0 != ret, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
                json_object_put(obj_jso); *o_result_jso = NULL; g_slist_free(obj_list);
                debug_log(DLOG_ERROR, "%s:json_object_array_add error", __FUNCTION__));

            
            obj_jso = NULL;
        }
        
    }

    g_slist_free(obj_list);
    return HTTP_OK;
}


gint32 redfish_mac_addr2uriid(gchar *mac_addr, guint8 addr_len)
{
    gint32 i, j;
    if (NULL == mac_addr || addr_len == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    for (i = 0, j = 0; mac_addr[i] != '\0'; i++) {
        if (mac_addr[i] != ':') {
            mac_addr[j++] = mac_addr[i];
        }
    }

    mac_addr[j] = '\0';
    return VOS_OK;
}


gint32 system_ethernetinterfaces_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guchar board_type = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    return_val_if_fail(DISABLE != board_type, HTTP_NOT_FOUND);
    

    
    if (redfish_check_system_uri_valid(i_paras->uri)) {
        

        *prop_provider = g_system_eths_provider;
        *count = sizeof(g_system_eths_provider) / sizeof(PROPERTY_PROVIDER_S);

        return VOS_OK;
    }

    return HTTP_NOT_FOUND;
}
