

#include "redfish_provider.h"

#define OCP_CARD_UNKNOWN          0x3

LOCAL gint32 get_chassis_networkadapter_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_technology(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object *json_array);
LOCAL gint32 get_chassis_networkadapter_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkadapter_networkports(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_chassis_networkadapter_oem_configuration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_chassis_networkadapter_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_chassis_networkadapter_provider[] = {
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {CHASSIS_NETWORK_ADAPTER_MANU, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {CHASSIS_NETWORK_ADAPTER_MODEL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_model, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_STATUS, CLASS_NETCARD_NAME, BUSY_NETCARD_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPORTS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_networkports, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_VOLUMES_OEM, RF_VALUE_NULL, RF_VALUE_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_chassis_networkadapter_oem, set_chassis_networkadapter_oem, NULL, ETAG_FLAG_ENABLE},
    {CHASSIS_NETWORK_ADAPTER_CONTROLLER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkadapter_controllers, NULL, NULL, ETAG_FLAG_ENABLE}
};
typedef struct {
    
    guint8 reg_val;
    
    gchar led_state_name[MAX_CLASS_NAME];
} PROPERTY_OCP_LEDSTATE;

LOCAL PROPERTY_OCP_LEDSTATE g_ocp_led_state[] = {
    {OCP_CARD_REMOVEABLE, "Removable"},
    {OCP_CARD_INSERTABLE, "Insertable"},
    {OCP_CARD_INOPERABLE, "Inoperable"},
    {OCP_CARD_UNKNOWN, "Unknown"}
};

LOCAL void get_pcie_slot_by_component(OBJ_HANDLE component_handle, guint8 *slot_id)
{
    guint8 comp_type;
    gchar *card_class_name = NULL;
    OBJ_HANDLE card_handle = INVALID_OBJ_HANDLE;

    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICENUM, slot_id);
    (void)dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_DEVICE_TYPE, &comp_type);
    switch (comp_type) {
        case COMPONENT_TYPE_PCIE_CARD:
            card_class_name = CLASS_PCIE_CARD;
            break;
        case COMPONENT_TYPE_OCP:
            card_class_name = CLASS_OCP_CARD;
            break;
        default:
            return;
    }

    (void)dal_get_specific_object_byte(card_class_name, PROPETRY_PCIECARD_SLOT, *slot_id, &card_handle);
    (void)dal_get_silk_id(card_handle, PROPERTY_PCIE_CARD_LOGIC_SLOT, PROPETRY_PCIECARD_SLOT, slot_id);
    return;
}


LOCAL void add_property_value_string_network_adapters(json_object *object_name, const gchar *add_name,
    const gchar *string_value, gint32 ret)
{
    if (ret != RET_OK || g_strcmp0(string_value, "N/A") == 0) {
        json_object_object_add(object_name, add_name, NULL);
    } else {
        json_object_object_add(object_name, add_name, json_object_new_string(string_value));
    }
}


LOCAL void add_property_slotid_network_adapters(json_object *object_name, OBJ_HANDLE card_handle, OBJ_HANDLE comphandle)
{
    gint32 ret;
    guint8 slot_id;
    guint8 card_type = 0;

    ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_CARDTYPE, &card_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, get %s crdType fail.\n", __FUNCTION__, dfl_get_object_name(card_handle));
        json_object_object_add(object_name, CHASSIS_NETWORK_ADAPTER_SLOTID, NULL);
        json_object_object_add(object_name, CHASSIS_NETWORK_ADAPTER_SLOT_NUM, NULL);
        return;
    }

    if (card_type == NET_TYPE_PCIE) {
        get_pcie_slot_by_component(comphandle, &slot_id);
        json_object_object_add(object_name, CHASSIS_NETWORK_ADAPTER_SLOTID, json_object_new_int(slot_id));
        json_object_object_add(object_name, CHASSIS_NETWORK_ADAPTER_SLOT_NUM, json_object_new_int(slot_id));
    } else {
        json_object_object_add(object_name, CHASSIS_NETWORK_ADAPTER_SLOTID, NULL);
        json_object_object_add(object_name, CHASSIS_NETWORK_ADAPTER_SLOT_NUM, NULL);
    }
}


LOCAL gint32 get_chassis_networkadapter_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret_val;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret_val = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    if (ret_val != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)string_value);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gint32 ret;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, CHASSIS_NETWORK_ADAPTERS_MEMBER_ID, slot, string_value);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)uri);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, CHASSIS_NETWORK_ADAPTER_METADATA, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string((const gchar *)uri);
    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar manufacturer[FWINV_STR_MAX_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, BUSY_NETCARD_CHIP_MANU, manufacturer, FWINV_STR_MAX_LEN);
    if (ret != RET_OK || g_strcmp0(manufacturer, "N/A") == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        *o_result_jso = json_object_new_string((const gchar *)manufacturer);
    }

    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar model[FWINV_STR_MAX_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_string(i_paras->obj_handle, BUSY_NETCARD_MODEL, model, FWINV_STR_MAX_LEN);
    if (ret != RET_OK || g_strcmp0(model, "N/A") == 0) {
        return HTTP_INTERNAL_SERVER_ERROR;
    } else {
        *o_result_jso = json_object_new_string((const gchar *)model);
    }

    if (*o_result_jso == NULL) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 health = 0;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_property_value_byte(i_paras->obj_handle, BUSY_NETCARD_HEALTH, &health);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get health status fail.\n", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)get_resource_status_property(&health, NULL, STRING_CONSET_VALUE_ENABLE, o_result_jso, FALSE);
    return HTTP_OK;
}


gint32 get_chassis_networkadapter_technology_type(const gchar *net_card_obj_name, guint32 *func_type)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar ref_netcard[FWINV_STR_MAX_LEN] = {0};
    guint32 port_type = 0;

    if (net_card_obj_name == NULL || func_type == 0) {
        return RET_ERR;
    }

    
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get BusinessPort object list failed.");
        return RET_ERR;
    }

    
    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_REF_NETCARD, ref_netcard,
            FWINV_STR_MAX_LEN);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d:  dal_get_property_value_string fail\n", __FUNCTION__, __LINE__);
            g_slist_free(obj_list);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
        if (g_ascii_strcasecmp(ref_netcard, net_card_obj_name) != 0) {
            continue;
        }
        
        ret =
            dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_SUPPORT_FUNCTYPE, &port_type);
        
        // 一个获取失败不退出，continue
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s,dal_get_property_value_uint32 fail,ret = %d, port = %s", __FUNCTION__, ret,
                dfl_get_object_name((OBJ_HANDLE)obj_node->data));
            continue;
        }
        

        *func_type |= port_type;
    }

    g_slist_free(obj_list);

    return RET_OK;
}


LOCAL gint32 get_chassis_networkadapter_technology(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object *json_array)
{
    const gchar *net_card_obj_name = NULL;
    gint32 ret = 0xff;
    guint32 func_type = 0;

    
    net_card_obj_name = dfl_get_object_name(obj_handle);

    (void)get_chassis_networkadapter_technology_type(net_card_obj_name, &func_type);

    if (func_type == NETCARD_BUSIPORT_TYPE_UNKNOWN) {
        return RET_OK;
    }

    if (func_type & NETCARD_BUSIPORT_TYPE_ETH) {
        ret = json_object_array_add(json_array, json_object_new_string(RFPROP_ETHERNET));
        return_val_do_info_if_fail(ret == RET_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: json_object_array_add fail.", __FUNCTION__));
    }

    if (func_type & NETCARD_BUSIPORT_TYPE_FC) {
        ret = json_object_array_add(json_array, json_object_new_string(RFPROP_FIBRE_CHANNEL));
        return_val_do_info_if_fail(ret == RET_OK, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s:  json_object_array_add fail.", __FUNCTION__));
    }

    if (func_type & NETCARD_BUSIPORT_TYPE_ISCSI) {
        ret = json_object_array_add(json_array, json_object_new_string(RFPROP_ISCSI));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d:  json_object_array_add fail\n", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    if (func_type & NETCARD_BUSIPORT_TYPE_FCOE) {
        ret = json_object_array_add(json_array, json_object_new_string(RFPROP_FCOE));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d:  json_object_array_add fail\n", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    if (func_type & NETCARD_BUSIPORT_TYPE_OPA) {
        ret = json_object_array_add(json_array, json_object_new_string(RFPROP_OPA));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d:  json_object_array_add fail\n", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    if (func_type & NETCARD_BUSIPORT_TYPE_IB) {
        ret = json_object_array_add(json_array, json_object_new_string(RFPROP_IB));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s, %d:  json_object_array_add fail\n", __FUNCTION__, __LINE__);
            return HTTP_INTERNAL_SERVER_ERROR;
        }
    }

    return RET_OK;
}


LOCAL gint32 get_chassis_networkadapter_configuration(PROVIDER_PARAS_S *i_paras, guint16 boardid, json_object *huawei,
    const gchar *devicename, OBJ_HANDLE card_handle)
{
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *response = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar board_type = 0;
    guint32 product_num = 0;
    guchar arm_enable = 0;
    json_object *response_json = NULL;
    const gchar *object_name = NULL;

    if (provider_paras_check(i_paras) != RET_OK || huawei == NULL || devicename == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    
    ret = dal_get_product_version_num(&product_num);
    return_val_do_info_if_fail(ret == RET_OK, RET_ERR, debug_log(DLOG_ERROR, "%s: get board_type fail.", __FUNCTION__));
    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(ret == RET_OK, RET_ERR, debug_log(DLOG_ERROR, "%s: get board_type fail.", __FUNCTION__));
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);

    // 网卡配置查询只支持V5刀片和机架
    if (board_type == DISABLE || ((arm_enable == DISABLE) && (product_num != PRODUCT_VERSION_V5))) {
        return RET_ERR;
    }

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get obj_handle fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    object_name = dfl_get_object_name(card_handle);

    
    input_list = g_slist_append(input_list, g_variant_new_uint16(boardid));
    input_list = g_slist_append(input_list, g_variant_new_string(devicename));
    input_list = g_slist_append(input_list, g_variant_new_string(object_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BIOS_CLASS_NAME, METHOD_BIOS_GET_CLP_RESP, 0, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    
    if (ret != RET_OK) {
        debug_log(DLOG_MASS, "call method %s failed, ret is %d", METHOD_BIOS_GET_CLP_RESP, ret);
        return RET_ERR;
    }
    response = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);

    response_json = json_tokener_parse(response);
    if (response_json == NULL) {
        uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s: json_tokener_parse fail.\n", __FUNCTION__);
        return RET_ERR;
    }

    json_object_object_add(huawei, RF_MEZZ_CONFIGURATION, response_json);

    uip_free_gvariant_list(output_list);

    return RET_OK;
}

LOCAL gint32 get_chassis_networkadapter_pcb_version(json_object *huawei, OBJ_HANDLE component_handle)
{
    OBJ_HANDLE fru_handle = 0;
    guchar fru_id = 0;
    gchar pcbversion[MAX_STRBUF_LEN] = {0};
    gint32 ret = dal_get_property_value_byte(component_handle, PROPERTY_COMPONENT_FRUID, &fru_id);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: Get FruId failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &fru_handle);
    if (ret == DFL_OK) {
        ret = dal_get_property_value_string(fru_handle, PROPERTY_FRU_PCB_VERSION, pcbversion, sizeof(pcbversion));
        if (ret != DFL_OK || check_string_val_effective(pcbversion) != VOS_OK) {
            debug_log(DLOG_DEBUG, "%s: Get PCB version from fru failed, ret = %d", __FUNCTION__, ret);
            goto EXIT;
        }

        json_object_object_add(huawei, RFPROP_BOARD_PCBVERSION, json_object_new_string((const gchar*)pcbversion));
        return VOS_OK;
    }

    OBJ_HANDLE pcie_handle = 0;
    ret = dal_get_specific_object_position(component_handle, CLASS_PCIE_CARD, &pcie_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: Get pcie card handle failed, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }
    ret = dal_get_property_value_string(pcie_handle, PROPERTY_PCIE_CARD_PCB_VER, pcbversion, sizeof(pcbversion));
    if (ret != DFL_OK || check_string_val_effective(pcbversion) != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s: Get PCB version from pciecard fail, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }
    json_object_object_add(huawei, RFPROP_BOARD_PCBVERSION, json_object_new_string((const gchar*)pcbversion));
    return VOS_OK;
EXIT:
    json_object_object_add(huawei, RFPROP_BOARD_PCBVERSION, NULL);
    return VOS_ERR;
}


LOCAL gint32 get_chassis_networkadapter_associatedresource(json_object *huawei, OBJ_HANDLE card_handle)
{
    json_object *resid_obj = NULL;
    gint32 ret_val;
    if (huawei == NULL || card_handle == 0) {
        return RET_ERR;
    }

    
    ret_val = get_chassis_pciefunction_oem_associatedresource(card_handle, &resid_obj);
    if (ret_val != RET_OK) {
        json_object_object_add(huawei, RFPROP_PCIEFUNCTION_CONNECTEDCPU, NULL);
        return RET_OK;
    }
    

    json_object_object_add(huawei, RFPROP_PCIEFUNCTION_CONNECTEDCPU, resid_obj);

    return RET_OK;
}


LOCAL gint32 get_chassis_networkadapter_virtual_ethernetInterfaces(json_object *huawei, OBJ_HANDLE card_handle)
{
    int iRet = -1;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint32 net_dev_function = 0;
    guint8 port_type = 0;

    gchar card_name[MAX_RSC_NAME_LEN] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};
    gchar eth_uri[PROP_VAL_MAX_LENGTH] = {0};

    json_object *array_obj = json_object_new_array();
    return_val_if_expr(array_obj == NULL, RET_ERR);

    json_object *node_obj = NULL;

    if (huawei == NULL || card_handle == 0) {
        json_object_put(array_obj);
        return RET_ERR;
    }
    (void)redfish_get_board_slot(slot, sizeof(slot));

    (void)dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        (void)memset_s(card_name, MAX_RSC_NAME_LEN, 0, MAX_RSC_NAME_LEN);

        (void)dal_get_property_value_uint32((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE,
            &net_dev_function);

        if ((net_dev_function & NETDEV_FUNCTYPE_ETHERNET) == 0) {
            continue;
        }

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
        // 认为0xff是没bma的情况下，物理网口的值
        // 只需要虚拟的
        if (port_type != ETH_PORT_TYPE_VIRTUAL) {
            continue;
        }

        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_ETH_ATTRIBUTE_REF_NETCARD, card_name,
            MAX_RSC_NAME_LEN);

        if (g_strcmp0(card_name, dfl_get_object_name(card_handle)) != 0) {
            continue;
        }

        (void)redfish_get_sys_eth_memberid((OBJ_HANDLE)obj_node->data, member_id, PROP_VAL_LENGTH);
        iRet = snprintf_s(eth_uri, sizeof(eth_uri), sizeof(eth_uri) - 1, URI_FORMAT_SYS_ETH, slot, member_id);
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }

        node_obj = json_object_new_object();
        if (node_obj == NULL) {
            continue;
        }

        json_object_object_add(node_obj, ODATA_ID, json_object_new_string((const gchar *)eth_uri));

        json_object_array_add(array_obj, node_obj);
    }

    g_slist_free(obj_list);

    json_object_object_add(huawei, RFPROP_NETCARD_VIRTUAL_ETHERNET_INTERFACES, array_obj);

    return RET_OK;
}


LOCAL void rf_add_ledstate_jso(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL;
    guchar prop_val;
    guint32 i;
    if ((property == NULL) || (rf_property == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return;
    }
    if (obj_handle == 0) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }
    
    ret = dal_get_property_value_byte(obj_handle, property, &prop_val);
    if (ret != VOS_OK) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle), property,
            ret);
        return;
    }
    
    if (prop_val == INVALID_DATA_BYTE) {
        json_object_object_add(o_result_jso, rf_property, property_jso);
        return;
    }

    for (i = 0; i < sizeof(g_ocp_led_state) / sizeof(PROPERTY_OCP_LEDSTATE); i++) {
        if (prop_val == g_ocp_led_state[i].reg_val) {
            property_jso = json_object_new_string(g_ocp_led_state[i].led_state_name);
            if (property_jso == NULL) {
                debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__);
            }
            debug_log(DLOG_DEBUG, "property_name is %s provider found", g_ocp_led_state[i].led_state_name);
            break;
        }
    }
    json_object_object_add(o_result_jso, rf_property, property_jso);
    return;
}

LOCAL gint32 get_chassis_networkadapter_sid_vid(OBJ_HANDLE card_handle, json_object *resource_jso)
{
    if (resource_jso == NULL || card_handle == 0) {
        return RET_ERR;
    }

    json_object *pci_id_val_jso = NULL;
    
    get_pci_id_property_uint16(card_handle, PROPERTY_NETCARD_VENDER_ID, &pci_id_val_jso);
    json_object_object_add(resource_jso, PROPERTY_DEVICE_OEM_VID, pci_id_val_jso);

    
    pci_id_val_jso = NULL;
    get_pci_id_property_uint16(card_handle, PROPERTY_NETCARD_DEVICE_ID, &pci_id_val_jso);
    json_object_object_add(resource_jso, PROPERTY_DEVICE_OEM_DID, pci_id_val_jso);

    
    pci_id_val_jso = NULL;
    get_pci_id_property_uint16(card_handle, PROPERTY_NETCARD_SUB_VENDER_VID, &pci_id_val_jso);
    json_object_object_add(resource_jso, PROPERTY_DEVICE_OEM_SUB_VID, pci_id_val_jso);

    
    pci_id_val_jso = NULL;
    get_pci_id_property_uint16(card_handle, PROPERTY_NETCARD_SUB_DEVICE_ID, &pci_id_val_jso);
    json_object_object_add(resource_jso, PROPERTY_DEVICE_OEM_SUB_DID, pci_id_val_jso);

    return RET_OK;
}

LOCAL void get_chassis_networkadapter_component_uid(OBJ_HANDLE component_handle, json_object *oem_data)
{
    gchar component_uid[STRING_LEN_MAX] = {0};
    (void)dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_UNIQUE_ID,
        component_uid, sizeof(component_uid));

    json_object* uid_jso = dal_check_component_uid_valid((const gchar*)component_uid) != TRUE ? NULL :
        json_object_new_string((const gchar*)component_uid);
    (void)json_object_object_add(oem_data, RFPROP_BOARD_COMPONENTUID, uid_jso);
}


LOCAL gint32 get_chassis_networkadapter_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    json_object *huawei = NULL;
    gchar drivername[FWINV_STR_MAX_LEN] = {0};
    gchar driverversion[FWINV_STR_MAX_LEN] = {0};
    gchar manu[FWINV_STR_MAX_LEN] = {0};
    gchar model[FWINV_STR_MAX_LEN] = {0};
    gchar name[FWINV_STR_MAX_LEN] = {0};
    gchar devicename[FWINV_STR_MAX_LEN] = {0};
    gchar holder[FWINV_STR_MAX_LEN] = {0};
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar rootBDF[FWINV_STR_MAX_LEN + 1] = {0};
    gchar firmware_version[FWINV_STR_MAX_LEN] = {0};
    OBJ_HANDLE card_handle = 0;
    OBJ_HANDLE component_handle = 0;
    guint16 board_id = 0;
    gchar boardIdHexStr[MAX_STRBUF_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    card_handle = i_paras->obj_handle;

    ret = dfl_get_referenced_object(card_handle, PROPERTY_NETCARD_REF_COMPONENT, &component_handle);
    return_val_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR);

    huawei = json_object_new_object();
    return_val_do_info_if_expr(NULL == huawei, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(card_handle, PROPERTY_NETCARD_NAME, name, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_NAME, name, ret);

    
    ret = dal_get_property_value_string(card_handle, BUSY_NETCARD_DRIVE_NAME, drivername, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_DRIVER_NAME, drivername, ret);

    
    ret = dal_get_property_value_string(card_handle, BUSY_NETCARD_DRIVE_VERSION, driverversion, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_DRIVER_VERSION, driverversion, ret);

    
    ret = dal_get_property_value_string(card_handle, BUSY_NETCARD_MANU, manu, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_CARDMANU, manu, ret);

    
    ret = dal_get_property_value_string(card_handle, PROPERTY_NETCARD_DESC, model, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_CARDMODEL, model, ret);

    
    add_property_slotid_network_adapters(huawei, card_handle, component_handle);

    
    
    ret =
        dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, devicename, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_DEVICE_LOCATOR, devicename, ret);

    
    ret = dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_LOCATION, holder, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_POSITION, holder, ret);
    

    
    
    json_object *technology = json_object_new_array();
    
    if (NULL != technology) {
        ret = get_chassis_networkadapter_technology(i_paras, card_handle, technology);
        // 获取失败不退出
        do_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s:get_chassis_networkadapter_technology fail\n", __FUNCTION__));
    }

    
    json_object_object_add(huawei, CHASSIS_NETWORK_ADAPTER_NETWORK_TECHNOLOGY, technology);
    

    
    ret = dal_get_property_value_string(card_handle, PROPERTY_NETCARD_ROOT_BDF, rootBDF, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_ROOT_BDF, rootBDF, ret);
    

    // 获取网卡固件信息
    ret = dal_get_property_value_string(card_handle, BUSY_NETCARD_FIRM_VERSION, firmware_version, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(huawei, CHASSIS_NETWORK_ADAPTER_FIRMWARE_VERSION, firmware_version, ret);

    // 获取网卡四元组信息
    ret = get_chassis_networkadapter_sid_vid(card_handle, huawei);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get netcard vender id failed, ret = %d", __FUNCTION__, ret);
    }

    
    ret = dal_get_property_value_uint16(component_handle, PROPERTY_COMPONENT_BOARDID, &board_id);
    do_if_expr(ret != DFL_OK, debug_log(DLOG_ERROR, "%s: get board id failed, ret = %d", __FUNCTION__,
        ret)); 
    

    
    if (ret == DFL_OK && board_id != 0 && board_id != INVALID_DATA_WORD) {
        (void)json_object_object_add(huawei, RFPROP_BOARD_BOARDID, json_object_new_int(board_id));
        
        ret = snprintf_s(boardIdHexStr, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "0x%04x", board_id);
        if (0 >= ret) {
            debug_log(DLOG_MASS, "%s %d : snprintf_s fail.", __FUNCTION__, __LINE__);
            (void)json_object_object_add(huawei, RFPROP_BOARD_BOARDIDHEX, NULL);
        } else {
            (void)json_object_object_add(huawei, RFPROP_BOARD_BOARDIDHEX, json_object_new_string(boardIdHexStr));
        }
        ret =
            get_chassis_networkadapter_configuration(i_paras, board_id, huawei, (const gchar *)devicename, card_handle);
        do_if_fail(VOS_OK == ret, json_object_object_add(huawei, RF_MEZZ_CONFIGURATION, NULL);
            debug_log(DLOG_MASS, "%s:get_chassis_networkadapter_configuration  fail\n", __FUNCTION__));
    } else {
        json_object_object_add(huawei, RF_MEZZ_CONFIGURATION, NULL);
        (void)json_object_object_add(huawei, RFPROP_BOARD_BOARDID, NULL);
        (void)json_object_object_add(huawei, RFPROP_BOARD_BOARDIDHEX, NULL);
        
    }

    get_chassis_networkadapter_component_uid(component_handle, huawei);

    (void)get_chassis_networkadapter_pcb_version(huawei, component_handle);

    rf_add_property_jso_bool(card_handle, PROPERTY_NETCARD_MCTP_SUPPORT, RFPROP_NETCARD_MCTP_SUPPORT, huawei);
    rf_add_property_jso_bool(card_handle, PROPERTY_NETCARD_HOTPLUG_SUPPORT, RFPROP_NETCARD_HOTPLUG_ENABLE, huawei);
    rf_add_property_jso_bool(card_handle, PROPERTY_NETCARD_PREPLUG_STATE, RFPROP_NETCARD_HOTPLUG_STATE, huawei);
    rf_add_ledstate_jso(card_handle, PROPERTY_NETCARD_HOTPLUG_CONTROL, RFPROP_NETCARD_HOTPLUG_CONTROL, huawei);
    (void)get_chassis_networkadapter_associatedresource(huawei, card_handle);

    (void)get_chassis_networkadapter_virtual_ethernetInterfaces(huawei, card_handle);
    

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__);
        (void)json_object_put(huawei));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_MANUFACTURER, huawei);

    return HTTP_OK;
}

LOCAL void create_error_message(const gchar *error_data, json_object **o_message_jso, json_object *deleted_jso)
{
    json_object *error_jso = NULL;
    json_object *tmp_jso = NULL;
    json_object *del_array = NULL;
    const gchar *message_id_str = NULL;
    const gchar *prop_str = NULL;
    const gchar *val_str = NULL;
    const gchar *rel_prop_str = NULL;
    const gchar *rel_val_str = NULL;
    const gchar *tmp_str = NULL;
    size_t i;

    if (error_data == NULL || o_message_jso == NULL || deleted_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return;
    }

    error_jso = json_tokener_parse(error_data);
    if (error_jso == NULL) {
        debug_log(DLOG_DEBUG, "%s: json_tokener_parse failed.", __FUNCTION__);
        return;
    }

    if (!json_object_object_get_ex(error_jso, MESSAGE_ID_KEY, &tmp_jso)) {
        debug_log(DLOG_DEBUG, "%s: No key %s is found.", __FUNCTION__, MESSAGE_ID_KEY);
        json_object_put(error_jso);
        return;
    }
    message_id_str = dal_json_object_get_str(tmp_jso);
    if (json_object_object_get_ex(error_jso, RFPROP_NETWORK_ADAPTER_ERROR_PROP, &tmp_jso)) {
        prop_str = dal_json_object_get_str(tmp_jso);
    }

    tmp_jso = NULL;
    if (json_object_object_get_ex(error_jso, RFPROP_NETWORK_ADAPTER_ERROR_VAL, &tmp_jso)) {
        val_str = dal_json_object_get_str(tmp_jso);
    }

    tmp_jso = NULL;
    if (json_object_object_get_ex(error_jso, RFPROP_NETWORK_ADAPTER_ERROR_REL_PROP, &tmp_jso)) {
        rel_prop_str = dal_json_object_get_str(tmp_jso);
    }

    tmp_jso = NULL;
    if (json_object_object_get_ex(error_jso, RFPROP_NETWORK_ADAPTER_ERROR_REL_VAL, &tmp_jso)) {
        rel_val_str = dal_json_object_get_str(tmp_jso);
    }

    if (g_ascii_strncasecmp(message_id_str, MSGID_PROP_MISSING, strlen(MSGID_PROP_MISSING)) == 0) {
        if (prop_str != NULL) {
            
            // 查找该属性是否被框架删除
            if (json_object_object_get_ex(deleted_jso, PROP_DELETED_PROPS, &del_array)) {
                for (i = 0; i < json_object_array_length(del_array); i++) {
                    tmp_jso = json_object_array_get_idx(del_array, i);
                    tmp_str = dal_json_object_get_str(tmp_jso);
                    if (strlen(prop_str) == strlen(tmp_str + 2)) {
                        if (g_ascii_strcasecmp(tmp_str + 2, prop_str) == 0) {
                            json_object_put(error_jso);
                            return;
                        }
                    }
                }
            }
            
            create_message_info(message_id_str, prop_str, o_message_jso, prop_str);
        }
    } else if (g_ascii_strncasecmp(message_id_str, MSGID_PROP_MODIFY_UNSUPPORT, strlen(MSGID_PROP_MODIFY_UNSUPPORT)) ==
        0) {
        create_message_info(message_id_str, RFPROP_NETWORK_ADAPTER_CONFIG, o_message_jso,
            RFPROP_NETWORK_ADAPTER_CONFIG);
    } else if (g_ascii_strncasecmp(message_id_str, MSGID_PF_TARGET_EXCEED, strlen(MSGID_PF_TARGET_EXCEED)) == 0 ||
        g_ascii_strncasecmp(message_id_str, MSGID_PORT_INSUFFICIENT, strlen(MSGID_PORT_INSUFFICIENT)) == 0 ||
        g_ascii_strncasecmp(message_id_str, MSGID_PROP_UNKNOWN, strlen(MSGID_PROP_UNKNOWN)) == 0 ||
        g_ascii_strncasecmp(message_id_str, MSGID_PROPERTY_VALUE_ERR, strlen(MSGID_PROPERTY_VALUE_ERR)) == 0 || 
        g_ascii_strncasecmp(message_id_str, MSGID_CHIP_RULE_CHECK_FAIL, strlen(MSGID_CHIP_RULE_CHECK_FAIL)) == 0) {
        if (prop_str != NULL) {
            create_message_info(message_id_str, prop_str, o_message_jso, prop_str);
        }
    }  else if (g_ascii_strncasecmp(message_id_str, MSGID_PROP_NOT_IN_LIST, strlen(MSGID_PROP_NOT_IN_LIST)) == 0 ||
        g_ascii_strncasecmp(message_id_str, MSGID_PROP_TYPE_ERR, strlen(MSGID_PROP_TYPE_ERR)) == 0) {
        if (prop_str != NULL && val_str != NULL) {
            create_message_info(message_id_str, prop_str, o_message_jso, val_str, prop_str);
        }
    } else if (g_ascii_strncasecmp(message_id_str, MSGID_MODE_NOT_SUPPORT, strlen(MSGID_MODE_NOT_SUPPORT)) == 0) {
        if (prop_str != NULL && rel_prop_str != NULL && rel_val_str != NULL) {
            create_message_info(message_id_str, prop_str, o_message_jso, rel_prop_str, rel_val_str, prop_str);
        }
    } else if (g_ascii_strncasecmp(message_id_str, MSGID_QINQ_VLANID_ERROR, strlen(MSGID_QINQ_VLANID_ERROR)) == 0) {
        
        if (prop_str != NULL) {
            create_message_info(message_id_str, prop_str, o_message_jso, rel_val_str, prop_str, val_str);
        }
        
    } else if (g_ascii_strncasecmp(message_id_str, MSGID_BANDWITH_ERROR, strlen(MSGID_BANDWITH_ERROR)) == 0) {
        
        if (prop_str != NULL && rel_prop_str != NULL) {
            create_message_info(message_id_str, prop_str, o_message_jso, prop_str, rel_prop_str);
        }
        
    } else {
        if (g_ascii_strncasecmp(message_id_str, MSGID_BOOTPROTOCOL_NOT_FOUND_IN_PFS,
            strlen(MSGID_BOOTPROTOCOL_NOT_FOUND_IN_PFS)) == 0 &&
            prop_str != NULL) {
            create_message_info(message_id_str, prop_str, o_message_jso, prop_str);
            json_object_put(error_jso);
            return;
        }
        debug_log(DLOG_ERROR, "%s: message_id_str:%s", __FUNCTION__, message_id_str);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }

    json_object_put(error_jso);
}


LOCAL gint32 set_chassis_networkadapter_oem_configuration(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint16 board_id = 0;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE component_handle = 0;
    OBJ_HANDLE card_handle;
    guint8 port_count = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    json_object *huawei_obj = NULL;
    json_object *config_obj = NULL;
    const gchar *config_str = NULL;
    gchar           devicename[FWINV_STR_MAX_LEN] = {0};
    guchar board_type = 0;
    guint32 product_num = 0;
    guchar arm_enable = 0;
    const gchar *error_message_str = NULL;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d : NULL Pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    
    ret = dal_get_product_version_num(&product_num);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);

    // 网卡配置设置只支持V5刀片和机架
    if (DISABLE == board_type || ((DISABLE == arm_enable) && (product_num != PRODUCT_VERSION_V5))) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_NETWORK_ADAPTER_CONFIG, o_message_jso,
            RFPROP_NETWORK_ADAPTER_CONFIG);
        
        debug_log(DLOG_DEBUG, "%s, %d: unsupport modify.\n", __FUNCTION__, __LINE__);
        
        return HTTP_NOT_IMPLEMENTED;
    }

    
    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei_obj);
    return_val_do_info_if_expr((0 == g_strcmp0(dal_json_object_to_json_string(i_paras->val_jso), JSON_NULL_OBJECT_STR))
        || (0 == g_strcmp0(dal_json_object_to_json_string(huawei_obj), JSON_NULL_OBJECT_STR)),
        HTTP_BAD_REQUEST, debug_log(DLOG_DEBUG, "%s: Get object failed.", __FUNCTION__));

    return_val_do_info_if_fail(TRUE ==
        json_object_object_get_ex(huawei_obj, RFPROP_NETWORK_ADAPTER_CONFIG, &config_obj),
        HTTP_BAD_REQUEST, debug_log(DLOG_DEBUG, "%s, %d: Get Configuration object failed.", __FUNCTION__, __LINE__));

    card_handle = i_paras->obj_handle;
    ret = dfl_get_referenced_object(card_handle, PROPERTY_NETCARD_REF_COMPONENT, &component_handle);
    return_val_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR);
    
    if (json_object_object_get_ex(config_obj, RFPROP_NETWORK_ADAPTER_CONFIG_EFFECTIVE, NULL)) {
        (void)create_message_info(MSGID_PROP_NOT_WRITABLE, RFPROP_NETWORK_ADAPTER_CONFIG_EFFECTIVE, o_message_jso,
            RFPROP_NETWORK_ADAPTER_CONFIG_EFFECTIVE);
        debug_log(DLOG_DEBUG, "%s:Effective property is read-only.", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    
    config_str = dal_json_object_to_json_string(config_obj);

    ret = dal_get_property_value_uint16(component_handle, PROPERTY_COMPONENT_BOARDID, &board_id);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: board_id get error.", __FUNCTION__, __LINE__));

    ret =
        dal_get_property_value_string(component_handle, PROPERTY_COMPONENT_DEVICE_NAME, devicename, FWINV_STR_MAX_LEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: devicename get error.", __FUNCTION__, __LINE__));

    if (!g_ascii_strncasecmp(devicename, NET_TYPE_LOM_STR, strlen(NET_TYPE_LOM_STR))) {
        ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_PORTNUM, &port_count);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_DEBUG, "%s, %d: port count get error.", __FUNCTION__, __LINE__));
    }
    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_DEBUG, "%s:find object fail.", __FUNCTION__));

    
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_uint16(board_id));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(devicename));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(config_str));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(port_count));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BIOS_CLASS_NAME, METHOD_BIOS_SET_CLP_CONF, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "call method %s failed, ret is %d", METHOD_BIOS_SET_CLP_CONF, ret));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "call method %s failed, ret is %d", METHOD_BIOS_SET_CLP_CONF, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        uip_free_gvariant_list(output_list));

    
    if (NULL != output_list) {
        error_message_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
        debug_log(DLOG_DEBUG, "error info is %s", error_message_str);
        create_error_message(error_message_str, o_message_jso, i_paras->delete_data);
        uip_free_gvariant_list(output_list);
        return HTTP_BAD_REQUEST;
    }

    uip_free_gvariant_list(output_list);

    return HTTP_OK;
}

LOCAL gint32 chassis_networkadapter_param_check(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **huawei_obj, gint32 *error_code)
{
    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s : NULL Pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        *error_code = HTTP_INTERNAL_SERVER_ERROR;
        return VOS_ERR;
    }

    if (g_strcmp0(dal_json_object_to_json_string(i_paras->val_jso), JSON_NULL_OBJECT_STR) == 0) {
        debug_log(DLOG_DEBUG, "%s : empty request.", __FUNCTION__);
        *error_code = HTTP_BAD_REQUEST;
        return VOS_ERR;
    }
    if (!json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, huawei_obj)) {
        debug_log(DLOG_DEBUG, "%s : get huawei object failed.", __FUNCTION__);
        *error_code = HTTP_BAD_REQUEST;
        return VOS_ERR;
    }
    if (g_strcmp0(dal_json_object_to_json_string(*huawei_obj), JSON_NULL_OBJECT_STR) == 0) {
        debug_log(DLOG_DEBUG, "%s : huawei object is null.", __FUNCTION__);
        *error_code = HTTP_BAD_REQUEST;
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 set_chassis_networkadapter_oem_preplug(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei_obj = NULL;
    json_object *preplug_obj = NULL;
    guint8 pre_plug_state;
    gint32 ret;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    const gchar *error_message_str = NULL;
    gint32 error_code = 0;

    ret = chassis_networkadapter_param_check(i_paras, o_message_jso, &huawei_obj, &error_code);
    if (ret != VOS_OK) {
        return error_code;
    }

    if (!json_object_object_get_ex(huawei_obj, RFPROP_NETCARD_HOTPLUG_STATE, &preplug_obj)) {
        debug_log(DLOG_DEBUG, "%s : %s object is null.", __FUNCTION__, RFPROP_NETCARD_HOTPLUG_STATE);
        return HTTP_OK;
    }
    pre_plug_state = (json_object_get_boolean(preplug_obj) == TRUE) ? 0 : 1;
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_byte(pre_plug_state));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_NETCARD_NAME, METHOD_SET_NETCARD_PRE_PLUG_STATE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);

    uip_free_gvariant_list(input_list);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "call method %s failed, ret is %d", METHOD_SET_NETCARD_PRE_PLUG_STATE, ret);
        create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_NETCARD_HOTPLUG_STATE, o_message_jso,
            RFPROP_NETCARD_HOTPLUG_STATE);
        if (output_list != NULL) {
            uip_free_gvariant_list(output_list);
        }
        return HTTP_BAD_REQUEST;
    }

    if (output_list != NULL) {
        error_message_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
        debug_log(DLOG_DEBUG, "error info is %s", error_message_str);
        create_error_message(error_message_str, o_message_jso, i_paras->delete_data);
        uip_free_gvariant_list(output_list);
    }

    return HTTP_OK;
}


LOCAL gint32 set_chassis_networkadapter_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 p_ret;
    gint32 c_ret;
    guint32 product_num = 0;
    json_object *preplug_msg = NULL;
    json_object *configuration_msg = NULL;
    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_DEBUG, "%s : NULL Pointer.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!(i_paras->user_role_privilege & USERROLE_BASICSETTING)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, "Oem/Huawei", o_message_jso, "Oem/Huawei");
        return HTTP_FORBIDDEN;
    }
    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "json_object_new_array failed.");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    p_ret = set_chassis_networkadapter_oem_preplug(i_paras, &preplug_msg, o_result_jso);
    if (preplug_msg != NULL) {
        (void)json_object_array_add(*o_message_jso, preplug_msg);
    }
    (void)dal_get_product_version_num(&product_num);
    if (product_num > PRODUCT_VERSION_V5) {
        return p_ret;
    }
    c_ret = set_chassis_networkadapter_oem_configuration(i_paras, &configuration_msg, o_result_jso);
    if (configuration_msg != NULL) {
        (void)json_object_array_add(*o_message_jso, configuration_msg);
    }
    if (p_ret == HTTP_OK || c_ret == HTTP_OK) {
        return HTTP_OK;
    }
    return c_ret;
}


LOCAL gint32 get_chassis_networkadapter_controllers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar firm_version[FWINV_STR_MAX_LEN] = {0};
    guint8 port_count = 0;
    guint8 card_type = 0;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    OBJ_HANDLE card_handle;
    OBJ_HANDLE component_handle = 0;
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    
    guint8 slot_id = 0;
    gchar               rsc_id[MAX_RSC_ID_LEN] = {0};
    

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__));

    card_handle = i_paras->obj_handle;
    ret = dfl_get_referenced_object(card_handle, PROPERTY_NETCARD_REF_COMPONENT, &component_handle);
    return_val_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR);

    _cleanup_json_object_ json_object *controllers = json_object_new_object();
    return_val_do_info_if_expr(NULL == controllers, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(card_handle, BUSY_NETCARD_FIRM_VERSION, firm_version, FWINV_STR_MAX_LEN);
    add_property_value_string_network_adapters(controllers, CHASSIS_NETWORK_ADAPTER_FIRM_VERSION, firm_version, ret);

    
    ret = dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_PORTNUM, &port_count);
    _cleanup_json_object_ json_object *controller_capabilities = json_object_new_object();
    return_val_do_info_if_expr(NULL == controller_capabilities, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));

    json_object_object_add(controller_capabilities, CHASSIS_NETWORK_ADAPTER_PORTCOUNT,
        (ret == RET_OK) ? json_object_new_int(port_count) : NULL);
    json_object_object_add(controllers, CHASSIS_NETWORK_ADAPTER_CONTROLLER_CAPA, TAKE_PTR(controller_capabilities));

    
    _cleanup_json_object_ json_object *controller_link = json_object_new_object();
    return_val_do_info_if_expr(NULL == controller_link, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_property_value_byte(card_handle, PROPERTY_NETCARD_CARDTYPE, &card_type);
    if (PCIE_NCSI_TYPE == card_type) {
        json_object_object_add(controller_link, CHASSIS_NETWORK_PCIEDS_ODATA_COUNT, json_object_new_int(1));

        
        get_pcie_slot_by_component(component_handle, &slot_id);

        ret = snprintf_s(rsc_id, sizeof(rsc_id), sizeof(rsc_id) - 1, "%s%u", RF_PCIECARD_PREFIX, slot_id);
        return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "format pcie card id failed, ret is %d", ret));

        ret = snprintf_s(string_value, sizeof(string_value), sizeof(string_value) - 1, CHASSIS_PCIEDS_MEMBERID, slot,
            rsc_id);
        return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));
        

        _cleanup_json_object_ json_object *obj_jso = json_object_new_object();
        return_val_do_info_if_expr(NULL == obj_jso, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__));
        json_object_object_add(obj_jso, RFPROP_ODATA_ID, json_object_new_string(string_value));

        _cleanup_json_object_ json_object *pcie_devices = json_object_new_array();
        return_val_do_info_if_expr(NULL == pcie_devices, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__));
        ret = json_object_array_add(pcie_devices, TAKE_PTR(obj_jso));
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_array_add error.", __FUNCTION__, __LINE__));
        json_object_object_add(controller_link, RFPROP_PCIEDS, TAKE_PTR(pcie_devices));
    }
    _cleanup_json_object_ json_object *networkports = json_object_new_array();
    return_val_do_info_if_expr(NULL == networkports, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__));
    (void)get_networkports_members_count(i_paras->obj_handle, &networkports, NULL);
    json_object_object_add(controller_link, SYSTEMS_NETWORK_PORTS_ODATA_COUNT,
        json_object_new_int((gint32)json_object_array_length(networkports)));
    json_object_object_add(controller_link, RFPROP_NETWORKPORTS, TAKE_PTR(networkports));
    json_object_object_add(controllers, RFPROP_LINKS, TAKE_PTR(controller_link));

    _cleanup_json_object_ json_object *result_jso = json_object_new_array();
    if (result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: json object new array", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = json_object_array_add(result_jso, TAKE_PTR(controllers));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_array_add error.", __FUNCTION__, __LINE__));
    
    *o_result_jso = TAKE_PTR(result_jso);
    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkadapter_networkports(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar string_value[FWINV_STR_MAX_LEN] = {0};
    gchar netports_url[MAX_URL_LEN] = {0};

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s, %d: get slot id error.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = get_network_adapter_id(i_paras->obj_handle, string_value, sizeof(string_value));
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(netports_url, sizeof(netports_url), sizeof(netports_url) - 1,
        SYSTEMS_NETWORK_ADAPTERS_NETWORKPORTS, slot, string_value);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    json_object_object_add(*o_result_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)netports_url));

    return HTTP_OK;
}


gint32 check_network_adapter_url(const gchar *network_adapter_id, OBJ_HANDLE *netcard_obj)
{
    gint32 ret_val;
    GSList *netcard_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE ref_com_handle = 0;
    guint8 virtual_flag = 0;
    gchar       member_id[MAX_RSC_ID_LEN] = {0};

    ret_val = dfl_get_object_list(CLASS_NETCARD_NAME, &netcard_list);
    if (ret_val != DFL_OK || netcard_list == NULL) {
        return RET_ERR;
    }

    for (node = netcard_list; node != NULL; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)(node->data);

        (void)dal_get_property_value_byte(obj_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
        if (virtual_flag == 1) { // 1 表示虚拟的网卡
            virtual_flag = 0;
            continue;
        }

        ret_val = dfl_get_referenced_object(obj_handle, PROPERTY_NETCARD_REF_COMPONENT, &ref_com_handle);
        if (ret_val != DFL_OK) {
            continue;
        }

        ret_val = get_location_devicename(ref_com_handle, member_id, sizeof(member_id));
        if (ret_val != RET_OK) {
            continue;
        }

        if (strcasecmp(member_id, network_adapter_id) == 0) {
            *netcard_obj = obj_handle;
            break;
        }
    }
    g_slist_free(netcard_list);

    if (node) {
        ret_val = RET_OK;
    } else {
        ret_val = RET_ERR;
    }

    return ret_val;
}


gint32 chassis_networkadapter_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    
    gint32 ret;
    gboolean bool_ret;
    OBJ_HANDLE chassis_handle;
    guchar x86_enabled = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enabled);
    if (x86_enabled != ENABLE) {
        return HTTP_NOT_FOUND;
    }

    
    bool_ret = redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle);
    if (!bool_ret) {
        return HTTP_NOT_FOUND;
    }
    

    
    ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    ret = check_network_adapter_url(i_paras->member_id, &(i_paras->obj_handle));
    if (ret != RET_OK) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_chassis_networkadapter_provider;
    *count = sizeof(g_chassis_networkadapter_provider) / sizeof(PROPERTY_PROVIDER_S);

    

    return RET_OK;
}
