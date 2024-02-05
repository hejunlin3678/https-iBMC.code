
#include "redfish_provider.h"

LOCAL gint32 get_chassis_networkadapters_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapters_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapters_odata_id_networkadapters(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapters_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapters_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapters_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_networkadapters_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapters_odata_id_networkadapters, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapters_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapters_members, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapters_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_VOLUMES_OEM, RF_VALUE_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapters_oem, NULL, NULL, ETAG_FLAG_ENABLE},
    
};

typedef struct _NetCardInfo_ {
    guint8 card_type;
    guint8 slot_id;

    gchar card_name[PROPERTY_NAME_LENGTH];
    gchar devicename[PROPERTY_NAME_LENGTH];
    gchar uri[MAX_URI_LENGTH];    
}NetCardInfo;


LOCAL gint32 gslist_compare_callback(gconstpointer constpoint_a, gconstpointer constpoint_b) 
{
    const NetCardInfo* compare_info_1 = NULL;
    const NetCardInfo* compare_info_2 = NULL;

    if (constpoint_a == NULL || constpoint_b == NULL) {
        return FALSE;
    }

    compare_info_1 = (const NetCardInfo*) constpoint_a;
    compare_info_2 = (const NetCardInfo*) constpoint_b;

    // 卡1 是板载卡
    if (compare_info_1->card_type == NET_TYPE_LOM) {
        if (compare_info_2->card_type == NET_TYPE_LOM) {
            goto slot_compare; // 两张卡都是板载卡，按槽位号排序
        }
        // 卡1是板载卡，卡2不是板载卡的情况下，不需要交换
        return FALSE;
    }

    // 卡1 是灵活插卡
    if (compare_info_1->card_type == NET_TYPE_LOM2) {
        if (compare_info_2->card_type == NET_TYPE_LOM) { // 卡2是板载卡
            return TRUE;
        } else if (compare_info_2->card_type == NET_TYPE_LOM2) {
            goto slot_compare;
        }

        return FALSE;
    }
    
    if (compare_info_2->card_type == NET_TYPE_LOM || compare_info_2->card_type == NET_TYPE_LOM2) {
        return TRUE;
    }

slot_compare: 
    if (compare_info_1->slot_id > compare_info_2->slot_id) {
        
        return TRUE;
    }
  
    // 槽位号一致的情况下，比较卡类型，按类型进行排序
    if (compare_info_1->slot_id == compare_info_2->slot_id && 
        compare_info_1->card_type > compare_info_2->card_type) {
        return TRUE;
    }

    return FALSE;
}


LOCAL void get_chassis_fcnetcard_info(OBJ_HANDLE handle, gchar* slot, json_object** fcnetcard_jso)
{
    gint32 ret;
    OBJ_HANDLE component_handle = 0;
    gchar location_devicename[PROPERTY_NAME_LENGTH] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar card_name[PROPERTY_NAME_LENGTH] = {0};
    gchar component_name[PROPERTY_NAME_LENGTH] = {0};

    
    ret = dfl_get_referenced_object(handle, PROPERTY_NETCARD_REF_COMPONENT, &component_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_referenced_object: %s", __FUNCTION__, PROPERTY_NETCARD_REF_COMPONENT);
        return;
    }

    
    (void)get_location_devicename(component_handle, location_devicename, PROPERTY_NAME_LENGTH);
    
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, CHASSIS_NETWORK_ADAPTERS_MEMBER_ID, slot, location_devicename);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    
    ret = dal_get_property_value_string(handle, PROPERTY_NETCARD_NAME, card_name, sizeof(card_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string:%s", __FUNCTION__, PROPERTY_NETCARD_NAME);
        return;
    }

    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, component_name, 
        PROPERTY_NAME_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string:%s", __FUNCTION__, PROPERTY_COMPONENT_DEVICE_NAME);
        return;
    }

    json_object* node_jso = json_object_new_object();
    if (node_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: new json object failed", __FUNCTION__);
        return;
    }

    
    json_object_object_add(node_jso, CHASSIS_NETWORK_ADAPTER_NAME, json_object_new_string((const gchar*)card_name));
    json_object_object_add(node_jso, CHASSIS_NETWORK_ADAPTER_DEVICE_LOCATOR,
        json_object_new_string((const gchar*)component_name));
    json_object_object_add(node_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar*)uri));

    json_object_array_add(*fcnetcard_jso, node_jso);

    return;
}


LOCAL gint32 get_netcard_property(OBJ_HANDLE handle, NetCardInfo* netcard_data)
{
    gint32 ret;
    OBJ_HANDLE com_handle = 0;

    // 获取卡名称
    ret = dal_get_property_value_string(handle, PROPERTY_NETCARD_NAME, netcard_data->card_name,
        sizeof(netcard_data->card_name));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string:%s", __FUNCTION__, PROPERTY_NETCARD_NAME);
        return RET_ERR;
    }

    // 获取网卡CardTye 
    ret = dal_get_property_value_byte(handle, PROPERTY_NETCARD_CARDTYPE, &(netcard_data->card_type));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte:%s.", __FUNCTION__, PROPERTY_NETCARD_CARDTYPE);
        return RET_ERR;
    }

    // 获取网卡槽位号 
    ret = dal_get_property_value_byte(handle, PROPERTY_NETCARD_SLOT_ID, &(netcard_data->slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte:%s.", __FUNCTION__, PROPERTY_NETCARD_SLOT_ID);
        return RET_ERR;
    }  

    ret = dfl_get_referenced_object(handle, PROPERTY_NETCARD_REF_COMPONENT, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_referenced_object: %s", __FUNCTION__, PROPERTY_NETCARD_REF_COMPONENT);
        return RET_ERR;
    }

    ret = dal_get_property_value_string(com_handle, PROPERTY_COMPONENT_DEVICE_NAME, netcard_data->devicename,
        sizeof(netcard_data->devicename));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte:%s.", __FUNCTION__, PROPERTY_COMPONENT_DEVICE_NAME);
        return RET_ERR;
    }   
    return RET_OK;  
}


LOCAL gint32 get_chassis_netcard_info(OBJ_HANDLE handle, gchar* slot, NetCardInfo** netcard_info)
{
    gint32 ret;
    OBJ_HANDLE com_handle = 0;
    gchar component_devicename[PROPERTY_NAME_LENGTH] = {0};
    gchar uri[MAX_URI_LENGTH] = {0};
    NetCardInfo* netcard_data = NULL;

    
    ret = dfl_get_referenced_object(handle, PROPERTY_NETCARD_REF_COMPONENT, &com_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_referenced_object: %s", __FUNCTION__, PROPERTY_NETCARD_REF_COMPONENT);
        return RET_ERR;
    }

    
    (void)get_location_devicename(com_handle, component_devicename, PROPERTY_NAME_LENGTH);
    
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, CHASSIS_NETWORK_ADAPTERS_MEMBER_ID, slot, component_devicename);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
    }

    netcard_data = (NetCardInfo*)g_malloc0(sizeof(NetCardInfo));
    if (netcard_data == NULL) {
        debug_log(DLOG_ERROR, "%s: malloc failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = get_netcard_property(handle, netcard_data);
    if (ret != RET_OK) {
        g_free(netcard_data);
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string:%s", __FUNCTION__, PROPERTY_NETCARD_NAME);
        return RET_ERR;
    }

    ret = snprintf_s(netcard_data->uri, sizeof(netcard_data->uri), sizeof(netcard_data->uri) - 1, "%s", uri);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed", __FUNCTION__);
    }

    *netcard_info = netcard_data;
   
    return RET_OK;
}


LOCAL void generate_net_card_json_array(GSList* netcard_list, json_object** netcard_jso)
{
    GSList* obj_node = NULL;
    NetCardInfo *netcard_list_data = NULL;
    json_object* node_obj_jso = NULL;
    
    for (obj_node = netcard_list; obj_node != NULL; obj_node = obj_node->next) {
        netcard_list_data = (NetCardInfo*)obj_node->data;
        if (netcard_list_data == NULL) {
            continue;
        }

        node_obj_jso = json_object_new_object();
        if (node_obj_jso == NULL) {
            debug_log(DLOG_ERROR, "%s: new json object failed", __FUNCTION__);
            continue;
        }

        json_object_object_add(node_obj_jso, CHASSIS_NETWORK_ADAPTER_NAME, 
            json_object_new_string(netcard_list_data->card_name));
        json_object_object_add(node_obj_jso, CHASSIS_NETWORK_ADAPTER_DEVICE_LOCATOR, 
            json_object_new_string(netcard_list_data->devicename));
        json_object_object_add(node_obj_jso, RFPROP_ODATA_ID, json_object_new_string(netcard_list_data->uri));
                
        json_object_array_add(*netcard_jso, node_obj_jso);
    }

    return;
}


LOCAL void get_chassis_networkadapters_netcard(gchar* slot, json_object** netcard_jso, json_object** fcnetcard_jso)
{
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    guchar virtual_flag;
    gint32 ret;
    const gchar* card_obj_name = NULL;
    guint32 type = 0;
    GSList* netcard_list = NULL;
    NetCardInfo *netcard_list_data = NULL;

    (void)dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        
        virtual_flag = NOT_AVAILABLE;
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
        if (ret != RET_OK || virtual_flag == AVAILABLE) {
            continue;
        }
        
        
        card_obj_name = dfl_get_object_name((OBJ_HANDLE)obj_node->data);
        type = 0;
        (void)get_chassis_networkadapter_technology_type(card_obj_name, &type);

        
        if ((type & NETCARD_BUSIPORT_TYPE_FC) != 0) {
            get_chassis_fcnetcard_info((OBJ_HANDLE)obj_node->data, slot, fcnetcard_jso);
        } else {
            netcard_list_data = NULL;
            ret = get_chassis_netcard_info((OBJ_HANDLE)obj_node->data, slot, &netcard_list_data);
            if (ret != RET_OK) {
                continue;
            }
            netcard_list = g_slist_insert_sorted(netcard_list, netcard_list_data, gslist_compare_callback);
        }
    }
    
    g_slist_free(obj_list);
    
    generate_net_card_json_array(netcard_list, netcard_jso);
    
    g_slist_free_full(netcard_list, g_free);
    
    return;
}


LOCAL gint32 get_chassis_networkadapters_oem_property(json_object* huawei) 
{    
    json_object* fcnetcard_jso = NULL;
    json_object* netcard_jso = NULL;
    gint32 ret;
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    
    if (huawei == NULL) {
        return RET_ERR;
    }

    fcnetcard_jso = json_object_new_array();
    if (fcnetcard_jso == NULL) {
        return RET_ERR;
    }

    netcard_jso = json_object_new_array();
    if (netcard_jso == NULL) {
        json_object_put(fcnetcard_jso);
        return RET_ERR;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        json_object_object_add(huawei, NETWORK_TYPE_NET, netcard_jso);
        json_object_object_add(huawei, NETWORK_TYPE_FC, fcnetcard_jso);
        return RET_OK;
    }

    
    get_chassis_networkadapters_netcard((gchar*)slot, &netcard_jso, &fcnetcard_jso);
    
    json_object_object_add(huawei, NETWORK_TYPE_NET, netcard_jso);
    json_object_object_add(huawei, NETWORK_TYPE_FC, fcnetcard_jso);

    return RET_OK;
}


LOCAL gint32 get_chassis_networkadapters_oem_property_team_bridge(json_object *huawei)
{
    int iRet = -1;
    gint32 ret;
    json_object *node_obj = NULL;
    json_object *team_jso = NULL;
    json_object *bridge_jso = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 card_type = 0;

    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};
    gchar eth_uri[PROP_VAL_MAX_LENGTH] = {0};
    gchar eth_name[FWINV_STR_MAX_LEN] = {0};

    return_val_if_expr(NULL == huawei, VOS_ERR);

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    bridge_jso = json_object_new_array();
    return_val_if_expr(NULL == bridge_jso, VOS_ERR);
    json_object_object_add(huawei, NETWORK_TYPE_BRIDGE, bridge_jso);

    team_jso = json_object_new_array();
    return_val_if_expr(NULL == team_jso, VOS_ERR);
    json_object_object_add(huawei, NETWORK_TYPE_TEAM, team_jso);

    (void)dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        continue_if_expr(BUSINESSPORT_CARD_TYPE_TEAM != card_type && BUSINESSPORT_CARD_TYPE_BRIDGE != card_type);

        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_OSETH_NAME, eth_name,
            FWINV_STR_MAX_LEN);
        continue_if_expr(VOS_OK != ret);

        (void)redfish_get_sys_eth_memberid((OBJ_HANDLE)obj_node->data, member_id, PROP_VAL_LENGTH);
        (void)redfish_get_board_slot(slot, sizeof(slot));

        node_obj = NULL;
        node_obj = json_object_new_object();
        continue_if_expr(NULL == node_obj);

        json_object_object_add(node_obj, CHASSIS_NETWORK_ADAPTER_NAME, json_object_new_string(eth_name));
        json_object_object_add(node_obj, CHASSIS_NETWORK_ADAPTER_DEVICE_LOCATOR, json_object_new_string(eth_name));
        if (card_type == BUSINESSPORT_CARD_TYPE_BRIDGE) {
            iRet = snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, URI_FORMAT_SYS_BRIDGE, slot, member_id);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            json_object_object_add(node_obj, RFPROP_ODATA_ID, json_object_new_string(eth_uri));
            json_object_array_add(bridge_jso, node_obj);
        } else {
            iRet = snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, URI_FORMAT_SYS_BONDING, slot, member_id);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            json_object_object_add(node_obj, RFPROP_ODATA_ID, json_object_new_string(eth_uri));
            json_object_array_add(team_jso, node_obj);
        }
    }

    g_slist_free(obj_list);

    return VOS_OK;
}



LOCAL gint32 get_chassis_networkadapters_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    huawei = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);

    (void)get_chassis_networkadapters_oem_property(huawei);

    (void)get_chassis_networkadapters_oem_property_team_bridge(huawei);

    return HTTP_OK;
}

gint32 get_network_members_count(json_object **o_result_jso, gchar *uri_template, gint32 *count)
{
    gchar       slot[MAX_RSC_ID_LEN] = {0};
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *obj_jso = NULL;
    gchar       network_url[MAX_URL_LEN] = {0};
    gchar       member_id[MAX_RSC_ID_LEN] = {0};
    guint8 virtual_flag = 0;
    OBJ_HANDLE ref_com_handle = 0;
    gint32 netcard_count = 0;

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    
    return_val_do_info_if_expr(VOS_OK != ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: get %s object list failed, ret:%d", __FUNCTION__, CLASS_NETCARD_NAME, ret));
    

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
        if (virtual_flag == 1) { // 1 表示虚拟的网卡
            virtual_flag = 0;
            continue;
        }

        ret = dfl_get_referenced_object((OBJ_HANDLE)obj_node->data, PROPERTY_NETCARD_REF_COMPONENT, &ref_com_handle);
        continue_if_expr(ret != DFL_OK);

        if (o_result_jso && uri_template) {
            ret = get_location_devicename(ref_com_handle, member_id, sizeof(member_id));
            continue_if_expr(ret != VOS_OK);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
            ret = snprintf_s(network_url, sizeof(network_url), sizeof(network_url) - 1, uri_template, slot, member_id);
#pragma GCC diagnostic pop
            continue_if_expr(ret <= 0);

            obj_jso = json_object_new_object();
            continue_if_expr(obj_jso == NULL);
            json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(network_url));
            ret = json_object_array_add(*o_result_jso, obj_jso);
            continue_do_info_if_expr(ret != VOS_OK, json_object_put(obj_jso));

            (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));
            (void)memset_s(network_url, sizeof(network_url), 0, sizeof(network_url));
        }
        netcard_count++;
    }
    g_slist_free(obj_list);

    if (count) {
        *count = netcard_count;
    }

    return VOS_OK;
}


LOCAL gint32 get_chassis_networkadapters_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
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

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, CHASSIS_NETWORK_ADAPTERS_METADATA,
        slot_id);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_networkadapters_odata_id_networkadapters(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
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

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, CHASSIS_NETWORK_ADAPTERS, slot);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string(uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapters_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)get_network_members_count(NULL, NULL, &count);

    *o_result_jso = json_object_new_int(count);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapters_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail\n.", __FUNCTION__, __LINE__));

    (void)get_network_members_count(o_result_jso, CHASSIS_NETWORK_ADAPTERS_MEMBER_ID, NULL);

    return HTTP_OK;
}


gint32 chassis_networkadapters_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    OBJ_HANDLE chassis_handle;
    guchar x86_enabled = 0;
    gint32 ret;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enabled);
    return_val_if_fail(ENABLE == x86_enabled, HTTP_NOT_FOUND);

    if (FALSE == redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle)) {
        return HTTP_NOT_FOUND;
    }

    
    ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    *prop_provider = g_chassis_networkadapters_provider;
    *count = sizeof(g_chassis_networkadapters_provider) / sizeof(PROPERTY_PROVIDER_S);

    

    return VOS_OK;
}
