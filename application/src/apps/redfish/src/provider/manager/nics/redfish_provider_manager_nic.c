

#include <arpa/inet.h>
#include "redfish_provider.h"

LOCAL gint32 _get_type_portnum_from_memberid(const gchar *memberid, guint8 *card_type, guint8 *port_num);
LOCAL gint32 _nic_rsc_id_compare(gconstpointer a, gconstpointer b);
LOCAL gint32 _nic_type_compare(gconstpointer a, gconstpointer b);
LOCAL gint32 _set_nic_interface_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_nic_host_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_vlan(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_nic_ipv4_static(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_dhcpv4(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_ipv6_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_nic_ipv6_static(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_ipv6_static_gw(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_nic_dhcpv6(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_name_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 _set_nic_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gboolean is_cardtype_ncsi_enable(OBJ_HANDLE port_cap_obj, guint8 card_type);

LOCAL PROPERTY_PROVIDER_S g_manager_nic_provider[] = {
    {RFPROP_NIC_INTERFACE_ENABLED, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_interface_enabled, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_BMC_HOSTNAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_host_name, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_BMC_FQDN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_fqdn, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_VLAN, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_vlan, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV4ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_ipv4_addresses, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_ipv4_static, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NIC_DHCPV4, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_dhcpv4, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV6_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_ipv6_addresses, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_ipv6_static, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_ipv6_static_gw, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NIC_DHCPV6, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_dhcpv6, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ETH_STATIC_NAMESERVER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_name_server, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, _set_nic_oem, NULL, ETAG_FLAG_ENABLE}
};

#define DEDICATED_ID "DedicatedPort%u"
#define AGGREGATION_ID "AggregationPort1"
#define LOM_ID "LOMPort%u"

#define LOM2_ID "FlexIOPort%u"

#define PCIE_ID "PCIePort%u"
#define OCP_ID "OCPPort%u"
#define OCP2_ID "OCP2Port%u"
#define LOW_DEDICATED_FLAG "dedicated"
#define LOW_AGGREGATION_FLAG "aggregation"
#define LOW_LOM_FLAG "lom"

#define LOW_LOM2_FLAG "flexio"

#define LOW_PCIE_FLAG "pcie"
#define LOW_OCP_FLAG "ocp"
#define LOW_OCP2_FLAG "ocp2"
#define LOW_PORT_FLAG "port"

#define DEDICATED_NAME "Manager Dedicated Network Interface Port%u"
#define AGGREGATION_NAME "Manager Aggregation Network Interface Port1"
#define SHARED_NAME "Manager %s Network Interface Port%u"

enum {
    PROP_EXIST,    // 属性通过了协议层的检查
    PROP_NOT_EXIST // 属性不存在
};

typedef struct _tag_nic_id_prop {
    guint8 card_type;
    guint8 eth_num;  // 共享网口有效
    guint8 port_num; // 共享网口有效
    gchar nic_rsc_id[MAX_RSC_ID_LEN];
} NIC_ID_PROP;


LOCAL void _add_nic_rsc_url_to_members(const gchar *slot_string, GSList *nic_rsc_list, json_object *members_array)
{
    GSList *node = NULL;
    NIC_ID_PROP *nic_id_prop = NULL;
    gint32 ret_val = 0;
    gchar       nic_url[MAX_RSC_URI_LEN] = {0};

    for (node = nic_rsc_list; node; node = g_slist_next(node)) {
        nic_id_prop = (NIC_ID_PROP *)(node->data);
        (void)memset_s(nic_url, sizeof(nic_url), 0, sizeof(nic_url));
        ret_val = snprintf_s(nic_url, sizeof(nic_url), sizeof(nic_url) - 1, URI_FORMAT_MANAGER_NIC, slot_string,
            nic_id_prop->nic_rsc_id);
        if (ret_val > 0) {
            RF_ADD_ODATA_ID_MEMBER(members_array, nic_url);
        }
    }

    return;
}


LOCAL gint32 _get_nic_from_eth(OBJ_HANDLE object_handle, gpointer user_data)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_val = 0;
    guint8 eth_type = 0;
    guint8 out_type = 0;
    GSList **nic_rsc_list = (GSList **)user_data;
    GSList *tmp_list = NULL;
    NIC_ID_PROP nic_rsc_prop_tmp = { 0 };
    guint8 rsc_id = 0;

    (void)dal_get_property_value_byte(object_handle, ETH_ATTRIBUTE_OUT_TYPE, &out_type);

    if (out_type != OUT_ETHERNET) {
        return DFL_OK;
    }

    (void)dal_get_property_value_byte(object_handle, ETH_ATTRIBUTE_TYPE, &eth_type);

    if (eth_type == NET_TYPE_DEDICATED) {
        // OSCA 可能会有2个专用网口满足条件，EthGroup只用使用其中的一个，若链表中已存在一个专用，跳过对本对象的处理
        nic_rsc_prop_tmp.card_type = NET_TYPE_DEDICATED;
        tmp_list = g_slist_find_custom(*nic_rsc_list, &nic_rsc_prop_tmp, _nic_type_compare);
        if (tmp_list) {
            return DFL_OK;
        }
    }

    NIC_ID_PROP *nic_id_prop = (NIC_ID_PROP *)g_malloc0(sizeof(NIC_ID_PROP));
    return_val_do_info_if_expr(nic_id_prop == NULL, DFL_OK, debug_log(DLOG_ERROR, "%s:g_malloc0 failed", __FUNCTION__));

    nic_id_prop->card_type = eth_type;
    if ((eth_type == NET_TYPE_DEDICATED) || (eth_type == NET_TYPE_CABINET_VLAN)) {
        if (dal_get_specific_object_byte(CLASS_NAME_ETH_FUNCTION, PROPERTY_FUNCTION_TYPE,
            FN_TYPE_OUTTER_OM, &obj_handle) == RET_OK) {
            rsc_id = dal_eth_get_cur_plane_eth_num();
        } else if (dal_get_specific_object_byte(ETH_CLASS_NAME_MGNTPORTCAP, PROPERTY_MNGTPORTCAP_PCIEMGNT_ENABLE,
            ENABLED, &obj_handle) == VOS_OK) {
            
            (void)dal_get_property_value_byte(object_handle, ETH_ATTRIBUTE_ETH_ID, &rsc_id);
        } else {
            rsc_id = 1;
        }

        ret_val = snprintf_s(nic_id_prop->nic_rsc_id, sizeof(nic_id_prop->nic_rsc_id),
            sizeof(nic_id_prop->nic_rsc_id) - 1, DEDICATED_ID, rsc_id);
        do_if_expr(ret_val <= 0,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s Dedicated id", ret_val));
    } else if (eth_type == NET_TYPE_AGGREGATION) {
        if (redfish_judge_mntportcab_valid(NET_TYPE_AGGREGATION) != RET_OK) {
            g_free(nic_id_prop);
            return RET_OK;
        }

        (void)strncpy_s(nic_id_prop->nic_rsc_id, sizeof(nic_id_prop->nic_rsc_id), AGGREGATION_ID,
            strlen(AGGREGATION_ID));
    } else {
        g_free(nic_id_prop);
        nic_id_prop = NULL;
    }

    if (nic_id_prop) {
        *nic_rsc_list = g_slist_append(*nic_rsc_list, nic_id_prop);
    }

    return DFL_OK;
}


LOCAL void _get_nic_id(OBJ_HANDLE object_handle, guint8 card_type, guint8 silk_num, NIC_ID_PROP *nic_id_prop)
{
    gint32 ret_val;
    const gchar *nic_id_template = NULL;

    if (card_type == NET_TYPE_LOM) {
        nic_id_template = LOM_ID;
    } else if (card_type == NET_TYPE_LOM2) {
        nic_id_template = LOM2_ID;
    } else if (card_type == NET_TYPE_PCIE) {
        nic_id_template = PCIE_ID;
    } else if (card_type == NET_TYPE_OCP_CARD) {
        nic_id_template = OCP_ID;
    } else if (card_type == NET_TYPE_OCP2_CARD) {
        nic_id_template = OCP2_ID;
    } else {
        return;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret_val = snprintf_s(nic_id_prop->nic_rsc_id, sizeof(nic_id_prop->nic_rsc_id), sizeof(nic_id_prop->nic_rsc_id) - 1,
        nic_id_template, silk_num);
#pragma GCC diagnostic pop
    do_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s Shared id", ret_val));

    return;
}


LOCAL gboolean is_cardtype_ncsi_enable(OBJ_HANDLE port_cap_obj, guint8 card_type)
{
    guchar enable_state = 0;
    guchar enabled = 1;

    if (card_type == NET_TYPE_LOM) {
        (void)dal_get_property_value_byte(port_cap_obj, PROPERTY_MNGTPORTCAP_LOMNCSIENABLE, &enable_state);
        return_val_if_expr(enable_state != enabled, FALSE);
    } else if (card_type == NET_TYPE_LOM2) {
        (void)dal_get_property_value_byte(port_cap_obj, PROPERTY_MNGTPORTCAP_LOM2NCSIENABLE, &enable_state);
        return_val_if_expr(enable_state != enabled, FALSE);
    } else if (card_type == NET_TYPE_PCIE) {
        (void)dal_get_property_value_byte(port_cap_obj, PROPERTY_MNGTPORTCAP_PCIENCSIENABLE, &enable_state);
        return_val_if_expr(enable_state != enabled, FALSE);
    } else if (card_type == NET_TYPE_OCP_CARD) {
        (void)dal_get_property_value_byte(port_cap_obj, PROPERTY_MNGTPORTCAP_OCPENABLE, &enable_state);
        return_val_if_expr(enable_state != enabled, FALSE);
    } else if (card_type == NET_TYPE_OCP2_CARD) {
        (void)dal_get_property_value_byte(port_cap_obj, PROPERTY_MNGTPORTCAP_OCP2ENABLE, &enable_state);
        return_val_if_expr(enable_state != enabled, FALSE);
    } else {
        // card_type为其他类型直接返回错误
        return FALSE;
    }

    return TRUE;
}


LOCAL gint32 _get_nic_from_businessport(OBJ_HANDLE object_handle, gpointer user_data)
{
    NIC_ID_PROP *nic_id_prop = NULL;
    GSList **nic_rsc_list = (GSList **)user_data;
    static OBJ_HANDLE port_cap_obj = 0;
    guint8 card_type = 0;
    guint8 silk_num = 0;
    OBJ_HANDLE net_card_obj = 0;
    guint8 ncsi_supported = 0;
    guint8 available_flag = 0;

    (void)dal_get_property_value_byte(object_handle, BUSY_ETH_ATTRIBUTE_AVAILABLEFLAG, &available_flag);
    return_val_if_expr(available_flag == AVAILABLE_FLAG_VIRTUAL, DFL_OK);
    if (port_cap_obj == 0) {
        (void)dal_get_object_handle_nth(ETH_CLASS_NAME_MGNTPORTCAP, 0, &port_cap_obj);
    }

    (void)dal_get_property_value_byte(object_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
    (void)dal_get_property_value_byte(object_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk_num);
    (void)dfl_get_referenced_object(object_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &net_card_obj);
    (void)dal_get_property_value_byte(net_card_obj, PROPERTY_NETCARD_NCSI_SUPPORTED, &ncsi_supported);
    return_val_if_expr(ncsi_supported != ENABLE, DFL_OK);

    nic_id_prop = (NIC_ID_PROP *)g_malloc0(sizeof(NIC_ID_PROP));
    return_val_if_expr(nic_id_prop == NULL, DFL_OK);

    nic_id_prop->card_type = card_type;

    if (is_cardtype_ncsi_enable(port_cap_obj, card_type) == FALSE) {
        goto EXIT_FREE_MEM;
    }

    _get_nic_id(net_card_obj, card_type, silk_num, nic_id_prop);
    (void)dal_get_property_value_byte(object_handle, BUSY_ETH_ATTRIBUTE_ETH_NUM, &(nic_id_prop->eth_num));
    (void)dal_get_property_value_byte(object_handle, BUSY_ETH_ATTRIBUTE_PORT_NUM, &(nic_id_prop->port_num));

    *nic_rsc_list = g_slist_append(*nic_rsc_list, nic_id_prop);

    return DFL_OK;
EXIT_FREE_MEM:
    g_free(nic_id_prop);

    return DFL_OK;
}


LOCAL void _fill_nic_rsc_list(GSList **nic_rsc_list)
{
    gint32 ret_val;

    if (nic_rsc_list == NULL) {
        debug_log(DLOG_ERROR, ERROR_LOG_NULL_POINTER, __FUNCTION__);
        return;
    }

    ret_val = dfl_foreach_object(ETH_CLASS_NAME_ETH, _get_nic_from_eth, nic_rsc_list, NULL);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "dfl_foreach_object Eth", ret_val);
    }

    ret_val = dfl_foreach_object(ETH_CLASS_NAME_BUSY_ETH, _get_nic_from_businessport, nic_rsc_list, NULL);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_DEBUG, ERROR_LOG_WITH_CODE, __FUNCTION__, "dfl_foreach_object BusinessPort", ret_val);
    }

    return;
}


gint32 get_nic_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    json_object *members_array = NULL;
    gint32 members_count;
    GSList *nic_rsc_list = NULL;
    gint32 ret_val;
    gchar       slot_string[MAX_RSC_ID_LEN] = {0};
    gchar       url_temp[MAX_RSC_URI_LEN] = {0};

    ret_val = redfish_get_board_slot(slot_string, sizeof(slot_string));
    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "get board slot", ret_val);
        return RF_FAILED;
    }

    ret_val = snprintf_s(url_temp, sizeof(url_temp), sizeof(url_temp) - 1, URI_FORMAT_MANAGER_NICS, slot_string);
    if (ret_val > 0) {
        json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)url_temp));
    }

    members_array = json_object_new_array();
    if (members_array) {
        _fill_nic_rsc_list(&nic_rsc_list);
        _add_nic_rsc_url_to_members(slot_string, nic_rsc_list, members_array);

        g_slist_free_full(nic_rsc_list, g_free);
    }
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS, members_array);
    members_count = (members_array != NULL) ? (gint32)json_object_array_length(members_array) : 0;
    json_object_object_add(o_rsc_jso, RFPROP_MEMBERS_COUNT, json_object_new_int(members_count));

    return RF_OK;
}


gint32 manager_nic_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret_val;

    ret_val = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_if_expr(ret_val != TRUE, HTTP_NOT_FOUND);

    *prop_provider = NULL;
    *count = 0;

    return VOS_OK;
}


LOCAL gint32 _set_nic_interface_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint8 netmode_flag = 0;
    gint32 ret_val = RF_PROP_TYPE_ERR;
    gint32 ret_code = HTTP_BAD_REQUEST;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE uip_handle = i_paras->obj_handle;
    GSList *input_list = NULL;
    guint8 card_type = 0;
    guint8 port_num = 0;

    if (json_object_get_type(*o_result_jso) != json_type_boolean) {
        goto EXIT_WITH_MSG;
    }

    ret_val = dal_eth_get_out_type_object(OUTTER_GROUP_TYPE, &obj_handle);
    goto_label_do_info_if_expr(ret_val != DFL_OK, EXIT_WITH_MSG, (ret_val = HTTP_INTERNAL_SERVER_ERROR));

    json_bool json_bool_val = json_object_get_boolean(*o_result_jso);
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &netmode_flag);
 
    
    goto_label_do_info_if_expr(netmode_flag == RF_UNSUPPORT_ADAPTIVE_NET_MODE || json_bool_val != TRUE, EXIT_WITH_MSG,
        (ret_val = HTTP_NOT_IMPLEMENTED));

    (void)_get_type_portnum_from_memberid(i_paras->member_id, &card_type, &port_num);
    input_list = g_slist_append(input_list, g_variant_new_byte(card_type));
    if ((dal_check_ar_card_support()) || (card_type == NET_TYPE_LOM || card_type == NET_TYPE_LOM2 ||
        card_type == NET_TYPE_PCIE || card_type == NET_TYPE_OCP_CARD || card_type == NET_TYPE_OCP2_CARD)) {
        // 如果是共享网口，还需要设置 将丝印加入链表
        input_list = g_slist_append(input_list, g_variant_new_byte(port_num));
    }

    
    if (uip_handle == 0) {
        uip_handle = obj_handle;
    }

    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        uip_handle, CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_ACTIVE_PORT, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call EthGroup.SetActivePort", ret_val);
    }
EXIT_WITH_MSG:
    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case REDFISH_ETH_UNSUPPORT:
            // 非手动模式
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_AUTO_NETMODE, RFPROP_NIC_INTERFACE_ENABLED,
                o_message_jso);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_NIC_INTERFACE_ENABLED, o_message_jso,
                RFPROP_NIC_INTERFACE_ENABLED);
            break;

        case COMP_CODE_INVALID_CMD:
        case HTTP_NOT_IMPLEMENTED:
            ret_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_NIC_INTERFACE_ENABLED, o_message_jso,
                RFPROP_NIC_INTERFACE_ENABLED);
            break;

        case RF_PROP_TYPE_ERR: // 接口只会碰到NULL类型错误，所以第一个参数默认写成 null
            (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_NIC_INTERFACE_ENABLED, o_message_jso, RF_NULL_STR,
                RFPROP_NIC_INTERFACE_ENABLED);
            break;

        default:
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_host_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val = 0;
    gint32 ret_code = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, ERROR_LOG_NULL_POINTER, __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_BMC_HOSTNAME, o_message_jso));

    if (json_object_get_type(*o_result_jso) != json_type_string) {
        ret_val = RF_PROP_TYPE_ERR;
        goto EXIT_WITH_MSG;
    }

    ret_val = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    if (ret_val != DFL_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "dal_get_object_handle_nth BMC", ret_val);
        goto EXIT_WITH_MSG;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(dal_json_object_get_str(*o_result_jso)));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_CLASEE_NAME, METHOD_BMC_HOSTNAME, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call BMC.SetHostName", ret_val);
    }

EXIT_WITH_MSG:
    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_BMC_HOSTNAME, o_message_jso,
                RFPROP_MANAGER_BMC_HOSTNAME);
            break;

        case REDFISH_VOS_ERROR:
            ret_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_INVALID_HOSTNAME, RFPROP_MANAGER_BMC_HOSTNAME, o_message_jso);
            break;

        case RF_PROP_TYPE_ERR: // 接口只会碰到NULL类型错误，所以第一个参数默认写成 null
            ret_code = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_BMC_HOSTNAME, o_message_jso, RF_NULL_STR,
                RFPROP_MANAGER_BMC_HOSTNAME);
            break;

        default:
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    const gchar *fqdn_const = NULL;
    gchar       fqdn[DNS_DOMAINNAME_MAX_LEN + HOST_NAME_MAX_LEN + 2] = {0};
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    gchar *domain = NULL;
    OBJ_HANDLE bmc_obj = 0;
    OBJ_HANDLE ddns_obj = 0;
    GSList *input_list = NULL;
    guint8 dns_mode = G_MAXUINT8;

    return_val_do_info_if_expr(i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, ERROR_LOG_NULL_POINTER, __FUNCTION__));
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_BMC_FQDN, o_message_jso));
    goto_label_do_info_if_expr(json_object_get_type(*o_result_jso) != json_type_string, EXIT_WITH_MSG,
        (ret_val = RF_PROP_TYPE_ERR));

    fqdn_const = dal_json_object_get_str(*o_result_jso);
    goto_label_do_info_if_expr(strlen(fqdn_const) > DNS_DOMAINNAME_MAX_LEN + HOST_NAME_MAX_LEN + 1, EXIT_WITH_MSG,
        (ret_val = RF_PROP_FORMAT_ERR));

    ret_val = strncpy_s(fqdn, sizeof(fqdn), fqdn_const, strlen(fqdn_const));
    if (ret_val != EOK) {
        ret_val = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s fqdn", ret_val);
        goto EXIT_WITH_MSG;
    }

    // 查找第一个点，分割主机名和域名
    domain = strstr(fqdn, DOT_STR);
    // 不含   .
    goto_label_do_info_if_expr(domain == NULL, EXIT_WITH_MSG, (ret_val = RF_PROP_FORMAT_ERR));
    *domain = 0;
    domain += 1;

    goto_label_do_info_if_expr(strlen(domain) > DNS_DOMAINNAME_MAX_LEN, EXIT_WITH_MSG, (ret_val = RF_PROP_FORMAT_ERR));

    ret_val = dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &ddns_obj);
    ret_val += dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &bmc_obj);
    if (ret_val != DFL_OK) {
        ret_val = HTTP_INTERNAL_SERVER_ERROR;
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "dal_get_object_handle_nth", ret_val);
        goto EXIT_WITH_MSG;
    }

    (void)dal_get_property_value_byte(ddns_obj, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
    goto_label_do_info_if_expr(dns_mode != DNS_ADD_ORIGIN_MANUAL, EXIT_WITH_MSG, (ret_val = REDFISH_ETH_UNSUPPORT));

    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)fqdn));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, bmc_obj,
        BMC_CLASEE_NAME, METHOD_BMC_HOSTNAME, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;
    if (ret_val != VOS_OK) {
        do_if_expr(ret_val != RFERR_INSUFFICIENT_PRIVILEGE && ret_val != COMP_CODE_INVALID_CMD,
            (ret_val = RF_PROP_FORMAT_ERR));
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call BMC.SetHostName", ret_val);
        goto EXIT_WITH_MSG;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(domain));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ddns_obj,
        CLASS_NAME_DNSSETTING, METHOD_DNSSETTING_DOMAINNAME, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call DNSSetting.SetDomainName", ret_val);
    }

EXIT_WITH_MSG:
    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RF_PROP_TYPE_ERR:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_BMC_FQDN, o_message_jso, RF_NULL_STR,
                RFPROP_MANAGER_BMC_FQDN);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
                RFPROP_MANAGER_BMC_FQDN);
            break;

        case RF_PROP_FORMAT_ERR:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_MANAGER_BMC_FQDN, o_message_jso, fqdn_const,
                RFPROP_MANAGER_BMC_FQDN);
            break;

        case REDFISH_ETH_UNSUPPORT:
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_AUTO_DNS, RFPROP_MANAGER_BMC_FQDN, o_message_jso);
            break;

        case COMP_CODE_INVALID_CMD:
            ret_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_BMC_FQDN, o_message_jso,
                RFPROP_MANAGER_BMC_FQDN);
            break;

        default:
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            break;
    }

    return ret_code;
}


LOCAL gint32 _create_vlan_message(gint32 ret_val, OBJ_HANDLE obj_handle, const gchar *id_json_p,
    json_object **message_obj)
{
    gint32 ret_code = HTTP_BAD_REQUEST;
    gchar       vlan_id_str[5] = {0}; // vlan id 最大4位数
    gchar       min_vlan_id_str[5] = {0};
    gchar       max_vlan_id_str[5] = {0};
    guint16 vlan_id = 0;
    gint32 ret = 0;

    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case COMP_CODE_INVALID_CMD:
        case HTTP_NOT_IMPLEMENTED:
            ret_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_VLAN, message_obj,
                RFPROP_MANAGER_ETH_VLAN);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_VLAN, message_obj,
                RFPROP_MANAGER_ETH_VLAN);
            break;

        case REDFISH_ETH_ERR: // 分析代码，此错误正常只可能存在 vlan id不在范围内的情况，生成vlan id错误消息
            ret = snprintf_s(vlan_id_str, sizeof(vlan_id_str), sizeof(vlan_id_str) - 1, "%u", vlan_id);
            do_if_expr(ret <= 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s vlan_id", ret_val));
            (void)dal_get_property_value_uint16(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_MIN, &vlan_id);
            ret = snprintf_s(min_vlan_id_str, sizeof(min_vlan_id_str), sizeof(min_vlan_id_str) - 1, "%u", vlan_id);
            do_if_expr(ret <= 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s min vlan_id", ret_val));
            (void)dal_get_property_value_uint16(obj_handle, ETH_GROUP_ATTRIBUTE_VLAN_MAX, &vlan_id);
            ret = snprintf_s(max_vlan_id_str, sizeof(max_vlan_id_str), sizeof(max_vlan_id_str) - 1, "%u", vlan_id);
            do_if_expr(ret <= 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s max vlan_id", ret_val));
            (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, id_json_p, message_obj, (const gchar *)vlan_id_str,
                id_json_p, (const gchar *)min_vlan_id_str, (const gchar *)max_vlan_id_str);
            break;

        default:
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_obj);
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_vlan(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val = 0;
    gint32 ret_code = HTTP_BAD_REQUEST;
    guint8 state_exit = PROP_NOT_EXIST;
    guint8 id_exit = PROP_NOT_EXIST;
    json_object *state_jso = NULL;
    json_object *id_jso = NULL;
    json_object *message_obj = NULL;
    const gchar *state_json_p = RFPROP_MANAGER_ETH_VLAN "/" RFPROP_MANAGER_ETH_VLAN_STATE;
    const gchar *id_json_p = RFPROP_MANAGER_ETH_VLAN "/" RFPROP_MANAGER_ETH_VLAN_ID;
    const gchar *json_str_val = RF_NULL_STR;
    json_bool json_bool_val;
    guint8 net_mode_flag = 1;
    GSList *input_list = NULL;
    guint16 vlan_id = 0;
    guint32 vlan_state = 0;
    guint8 prop_exist_count = 0;

    return_val_do_info_if_expr(i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, ERROR_LOG_NULL_POINTER, __FUNCTION__));
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_BMC_FQDN, o_message_jso));
    return_val_if_expr(json_object_object_length(*o_result_jso) == 0,
        HTTP_BAD_REQUEST); 

    // 如果存在相应的字段，则将存在标志设置为 存在
    json_bool_val = json_object_object_get_ex(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_STATE, &state_jso);
    do_if_expr(json_bool_val == TRUE, state_exit = PROP_EXIST; prop_exist_count++; json_bool_val = FALSE);
    json_bool_val = json_object_object_get_ex(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_ID, &id_jso);
    do_if_expr(json_bool_val == TRUE, id_exit = PROP_EXIST; prop_exist_count++; json_bool_val = FALSE);

    *o_message_jso = json_object_new_array();
    goto_label_do_info_if_expr(*o_message_jso == NULL, EXIT_WITH_MSG,
        debug_log(DLOG_ERROR, "%s, new *o_message_jso json array failed", __FUNCTION__);
        ret_val = HTTP_INTERNAL_SERVER_ERROR);

    // 判断json类型
    do_if_expr(state_exit == PROP_EXIST && json_object_get_type(state_jso) != json_type_boolean,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, state_json_p, &message_obj, RF_NULL_STR, state_json_p);
        (void)json_object_array_add(*o_message_jso, message_obj); message_obj = NULL;
        state_exit = PROP_NOT_EXIST); // 属性出错了，状态设置为不存在

    do_if_expr(id_exit == PROP_EXIST && json_object_get_type(id_jso) != json_type_int,
        json_str_val = (id_jso != NULL) ? dal_json_object_get_str(id_jso) : json_str_val;
        (void)create_message_info(MSGID_PROP_TYPE_ERR, id_json_p, &message_obj, json_str_val, id_json_p);
        (void)json_object_array_add(*o_message_jso, message_obj); message_obj = NULL;
        id_exit = PROP_NOT_EXIST); // 属性出错了，状态设置为不存在

    // 存在的属性均产生了错误返回 400
    return_val_if_expr(json_object_array_length(*o_message_jso) >= prop_exist_count, ret_code);

    // 判断是否支持VLAN设置
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_NET_MODE_FLAG, &net_mode_flag);
    goto_label_do_info_if_expr(net_mode_flag == RF_UNSUPPORT_ADAPTIVE_NET_MODE, EXIT_WITH_MSG,
        (ret_val = HTTP_NOT_IMPLEMENTED));

    // VLANEnable 和 VLANId 有3种 可能性。1: 2个都存在；2：只有VLANEnable存在；3：只有VLANId存在
    if (state_exit == PROP_EXIST && id_exit == PROP_EXIST) {
        (void)get_element_boolean(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_STATE, &json_bool_val);
        do_if_expr(json_bool_val == TRUE, (vlan_state = ENABLE));

        
        if (vlan_state != ENABLE) {
            create_message_info(MSGID_VLAN_INFO_CONFLICT, id_json_p, &message_obj);
            json_object_array_add(*o_message_jso, message_obj);
            return HTTP_BAD_REQUEST;
        }

        (void)get_element_int(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_ID, &ret_val);
        
        vlan_id = (guint16)ret_val;

        input_list = g_slist_append(input_list, g_variant_new_uint16(vlan_id));
        input_list = g_slist_append(input_list, g_variant_new_uint32(vlan_state));
    } else if (state_exit == PROP_EXIST && id_exit == PROP_NOT_EXIST) {
        // 用户只输入了 VLANEnable, 获取 后台的  vlan id属性
        (void)get_element_boolean(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_STATE, &json_bool_val);
        do_if_expr(json_bool_val == TRUE, (vlan_state = ENABLE));
        ret_val = dal_get_property_value_uint16(i_paras->obj_handle, PROPERTY_ETHGROUP_VLANID, &vlan_id);
        goto_label_do_info_if_expr(ret_val != DFL_OK, EXIT_WITH_MSG,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "get vlan id", ret_val));

        input_list = g_slist_append(input_list, g_variant_new_uint16(vlan_id));
        input_list = g_slist_append(input_list, g_variant_new_uint32(vlan_state));
    } else if (state_exit == PROP_NOT_EXIST && id_exit == PROP_EXIST) {
        // 后台方法支持 只传 vlan id
        (void)get_element_int(*o_result_jso, RFPROP_MANAGER_ETH_VLAN_ID, &ret_val);
        vlan_id = (guint16)ret_val;

        input_list = g_slist_append(input_list, g_variant_new_uint16(vlan_id));
        
        input_list = g_slist_append(input_list, g_variant_new_uint32(ENABLE));
    } else {
        // 不应该出现这种情况
        ret_code = HTTP_INTERNAL_SERVER_ERROR;
        goto EXIT_WITH_MSG;
    }

    ret_val =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_VLAN_ID, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
EXIT_WITH_MSG:

    ret_code = _create_vlan_message(ret_val, i_paras->obj_handle, id_json_p, &message_obj);
    if (message_obj) {
        ret_val = json_object_array_add(*o_message_jso, message_obj);
        do_if_expr(ret_val != 0, debug_log(DLOG_ERROR, "%s, add message_obj to *o_message_jso failed", __FUNCTION__);
            json_object_put(message_obj));
    }

    return ret_code;
}


LOCAL gint32 _set_nic_ipv4_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    (void)create_message_info(MSGID_PROP_NOT_WRITABLE, RFPROP_MANAGER_ETH_IPV4ADDR, o_message_jso,
        RFPROP_MANAGER_ETH_IPV4ADDR);
    return HTTP_BAD_REQUEST;
}


LOCAL gint32 _check_dhcpv4_and_static_ip(json_object *request_body)
{
    gint32 ret_val;
    json_object *dhcp_obj_jso = NULL;
    json_object *static_ip_jso = NULL;
    json_bool dhcp_state = FALSE;

    ret_val = json_object_object_get_ex(request_body, RFPROP_NIC_DHCPV4, &dhcp_obj_jso);
    if (ret_val == TRUE) {
        (void)get_element_boolean(dhcp_obj_jso, RFPROP_NIC_DHCP_ENABLED, &dhcp_state);
        ret_val = json_object_object_get_ex(request_body, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, &static_ip_jso);
        if (ret_val == TRUE && dhcp_state == TRUE) { // DHCPEnabled 为TRUE ，且 IPv4StaticAddresses 存在
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


LOCAL gint32 _get_prop_from_ipv4_obj(json_object *ipv4_obj, const gchar *prop_name, const gchar *json_pointer,
    gchar *prop_buf, guint32 buf_size, json_object *message_arr)
{
    gint32 ret_val;
    json_object *prop_jso = NULL;
    json_object *message_jso = NULL;
    const gchar *prop_char = NULL;
    const gchar *msg_id = NULL;

    ret_val = json_object_object_get_ex(ipv4_obj, prop_name, &prop_jso);
    return_val_if_expr(ret_val != TRUE, VOS_ERR);

    // 判断类型
    if (json_object_get_type(prop_jso) != json_type_string) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, json_pointer, &message_jso, RF_NULL_STR,
            json_pointer); // 目前接口层的类型不对只可能是  NULL
        ret_val = json_object_array_add(message_arr, message_jso);
        do_if_expr(ret_val != 0,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add message_jso to message_arr", ret_val);
            json_object_put(message_jso));
        return VOS_ERR;
    }

    prop_char = dal_json_object_get_str(prop_jso);
    if (0 == g_strcmp0(prop_name, RFPROP_MANAGER_ETH_IPADDR)) {
        ret_val = vos_ipv4_addr_valid_check((const guchar *)prop_char);
        msg_id = MSGID_INVALID_IPV4ADDRESS;
    } else if (0 == g_strcmp0(prop_name, RFPROP_MANAGER_ETH_IPMASK)) {
        ret_val = redfish_subnet_mask_check(prop_char);
        msg_id = MSGID_INVALID_SUBNETMASK;
    } else if (0 == g_strcmp0(prop_name, RFPROP_MANAGER_ETH_IPGATEWAY)) {
        ret_val = vos_ipv4_addr_valid_check((const guchar *)prop_char);
        msg_id = MSGID_INVALID_IPV4GATEWAY;
    } else {
        return VOS_ERR;
    }

    if (ret_val != 0) {
        (void)create_message_info(msg_id, json_pointer, &message_jso, prop_char, json_pointer);
        ret_val = json_object_array_add(message_arr, message_jso);
        do_if_expr(ret_val != 0,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add message_jso to message_arr", ret_val);
            json_object_put(message_jso));
        return VOS_ERR;
    }

    ret_val = strncpy_s(prop_buf, buf_size, prop_char, strlen(prop_char));
    if (ret_val != EOK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s prop_char", ret_val);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 _eth_judge_ipv4_ip_gateway(const gchar *ipv4_addr, const gchar *eth_sub_mask, const gchar *eth_gateway)
{
    gboolean is_true;
    struct in_addr temp = { 0 };
    guint32 ip;
    guint32 mask;
    guint32 gateway;

    if (inet_pton(AF_INET, ipv4_addr, (void *)&temp) != 1) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "inet_pton ipv4_addr", errno);
        return VOS_ERR;
    }
    ip = temp.s_addr;

    if (inet_pton(AF_INET, eth_sub_mask, (void *)&temp) != 1) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "inet_pton eth_sub_mask", errno);
        return VOS_ERR;
    }
    mask = temp.s_addr;

    if (inet_pton(AF_INET, eth_gateway, (void *)&temp) != 1) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "inet_pton eth_gateway", errno);
        return VOS_ERR;
    }
    gateway = temp.s_addr;

    is_true = ((ip & mask) != (gateway & mask));           // IP和网关的，网络号不同返回 1
    is_true = is_true || ((gateway & (~mask)) == 0);       // 网关的主机号为 0，即网络地址，返回 1
    is_true = is_true || (0xFFFFFFFF == (gateway | mask)); // 网关的主机号为全1，即广播地址，返回 1
    is_true = is_true && (0 != gateway); // 上面3个条件成立，且 网关不是  0时  最终结果才是                 1

    if (is_true) {
        debug_log(DLOG_ERROR, "%s: gateway address not valid", __FUNCTION__);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gint32 _fill_ipv4_input_list(OBJ_HANDLE obj_handle, json_object *ipv4_obj, GSList **input_list,
    json_object *message_arr)
{
    gchar       address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar       netmask[IPV4_MASK_STR_SIZE + 1] = {0};
    gchar       gateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    gint32 ret_val;
    GSList *i_input_list = NULL;
    guint8 log_flag = 0;
    const gchar *address_json_p = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR;
    const gchar *netmask_json_p = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPMASK;
    const gchar *gateway_json_p = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPGATEWAY;
    json_object *message_jso = NULL;
    guint8 ip_mode = 0;

    if (json_object_object_length(ipv4_obj) == 0) {
        return VOS_ERR;
    }

    ret_val = _get_prop_from_ipv4_obj(ipv4_obj, RFPROP_MANAGER_ETH_IPADDR, address_json_p, address, sizeof(address),
        message_arr);
    if (ret_val == VOS_OK) {
        log_flag |= BIT(0);
    } else {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_ETHGROUP_IPADDR, address, sizeof(address));
    }

    ret_val = _get_prop_from_ipv4_obj(ipv4_obj, RFPROP_MANAGER_ETH_IPMASK, netmask_json_p, netmask, sizeof(netmask),
        message_arr);
    if (ret_val == VOS_OK) {
        log_flag |= BIT(1);
    } else {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_ETHGROUP_SUBMASK, netmask, sizeof(netmask));
    }

    ret_val = _get_prop_from_ipv4_obj(ipv4_obj, RFPROP_MANAGER_ETH_IPGATEWAY, gateway_json_p, gateway, sizeof(gateway),
        message_arr);
    if (ret_val == VOS_OK) {
        log_flag |= BIT(2);
    } else {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_ETHGROUP_GATEWAY, gateway, sizeof(gateway));
    }

    if (log_flag == 0) {
        return VOS_ERR;
    }

    // 最后还需要校验 IP和网关是否同网段
    if (log_flag & BIT(2)) { // 用户有输入网关才校验
        ret_val = _eth_judge_ipv4_ip_gateway((const gchar *)address, (const gchar *)netmask, (const gchar *)gateway);
        if (ret_val != VOS_OK) {
            (void)create_message_info(MSGID_INVALID_IPV4GATEWAY, gateway_json_p, &message_jso, (const gchar *)gateway,
                gateway_json_p);
            ret_val = json_object_array_add(message_arr, message_jso);
            do_if_expr(ret_val != 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add message_jso to message_arr", ret_val);
                json_object_put(message_jso));
            return VOS_ERR;
        }
    }

    // 若当前已经是DHCP模式，不允许单独设置子网掩码和网关，带上IP就可以设置
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_ETHGROUP_IPMODE, &ip_mode);
    if (ip_mode == IP_DHCP_MODE && (log_flag & BIT(0)) == 0) {
        // 参考    IPAddresses.v1_1_0.json  描述，生成只读的错误消息
        do_if_expr(log_flag & BIT(1), (void)create_message_info(MSGID_PROP_NOT_WRITABLE, netmask_json_p, &message_jso,
            netmask_json_p);
            (void)json_object_array_add(message_arr, message_jso));
        do_if_expr(log_flag & BIT(2), (void)create_message_info(MSGID_PROP_NOT_WRITABLE, gateway_json_p, &message_jso,
            gateway_json_p);
            (void)json_object_array_add(message_arr, message_jso));

        return VOS_ERR;
    }

    i_input_list = g_slist_append(i_input_list, g_variant_new_string((const gchar *)address));
    i_input_list = g_slist_append(i_input_list, g_variant_new_string((const gchar *)netmask));
    i_input_list = g_slist_append(i_input_list, g_variant_new_string((const gchar *)gateway));
    i_input_list = g_slist_append(i_input_list, g_variant_new_byte(log_flag));

    *input_list = i_input_list;

    return VOS_OK;
}


LOCAL gint32 set_nic_ipv4_static_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, gint32 array_len)
{
    gint32 result;
    GSList *input_list = NULL;
    json_object *value_ipaddrs = NULL; 
    json_object *message_jso = NULL;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *ip = NULL;

    
    value_ipaddrs = json_object_array_get_idx(i_paras->val_jso, 0);
    if (json_object_object_length(value_ipaddrs) == 0 && array_len > 1) {
        debug_log(DLOG_DEBUG, "%s: start object == NULL\n", __FUNCTION__);
        return HTTP_OK;
    }

    result = _fill_ipv4_input_list(i_paras->obj_handle, json_object_array_get_idx(*o_result_jso, 0), &input_list,
        *o_message_jso);
    return_val_if_expr(result != VOS_OK, ret_code);

    result = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_MASK_GATEWAY, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (result) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN; // 权限不足只产生一个消息
            json_object_put(*o_message_jso);
            *o_message_jso = NULL;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, o_message_jso,
                RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR);
            break;

        case COMP_CODE_INVALID_CMD:
            ret_code = HTTP_NOT_IMPLEMENTED;
            json_object_put(*o_message_jso);
            *o_message_jso = NULL;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, o_message_jso,
                RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR);
            break;

        case ETH_PARAM_IP_CONFLICT_ERROR:
            (void)create_message_info(MSGID_IPV4_ADDRESS_CONFLCT,
                RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR, &message_jso);
            result = json_object_array_add(*o_message_jso, message_jso);
            do_if_expr(result != 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add message_jso to message_arr", result);
                json_object_put(message_jso));
            break;
        case ERROR_PARAMETER_INVALID:
            json_object_object_get_ex(value_ipaddrs, RFPROP_MANAGER_ETH_IPADDR, &ip);
            (void)create_message_info(MSGID_INVALID_IPV4ADDRESS,
                RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR, &message_jso,
                dal_json_object_get_str(ip), RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR);
            result = json_object_array_add(*o_message_jso, message_jso);
            do_if_expr(result != 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add message_jso to message_arr", result);
                json_object_put(message_jso));
            break;
        default:
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call EthGroup.SetIpMaskGateway", result);
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            result = json_object_array_add(*o_message_jso, message_jso);
            do_if_expr(result != 0,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add message_jso to message_arr", result);
                json_object_put(message_jso));
            break;
    }

    return ret_code;
}


LOCAL gint32 set_nic_backup_ipv4_static_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar       address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar       netmask[IPV4_MASK_STR_SIZE + 1] = {0};
    gchar       gateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    gint32 ret_val;
    guint8 log_flag = 0;
    const gchar *address_json_p = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/1/" RFPROP_MANAGER_ETH_IPADDR;
    const gchar *netmask_json_p = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/1/" RFPROP_MANAGER_ETH_IPMASK;
    const gchar *gateway_json_p = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/1/" RFPROP_MANAGER_ETH_IPGATEWAY;
    const gchar *address_json = RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR "/1/";
    json_object *message_jso = NULL;
    gint32 response_code = HTTP_BAD_REQUEST;
    json_object *ipv4_obj = NULL;

    ipv4_obj = json_object_array_get_idx(*o_result_jso, 1);
    if (json_object_object_length(ipv4_obj) == 0) {
        return HTTP_OK;
    }

    
    ret_val = _get_prop_from_ipv4_obj(ipv4_obj, RFPROP_MANAGER_ETH_IPADDR, address_json_p, address, sizeof(address),
        *o_message_jso);
    if (ret_val == VOS_OK) {
        log_flag |= BIT(0);
    }

    
    ret_val = _get_prop_from_ipv4_obj(ipv4_obj, RFPROP_MANAGER_ETH_IPMASK, netmask_json_p, netmask, sizeof(netmask),
        *o_message_jso);
    if (ret_val == VOS_OK) {
        log_flag |= BIT(1);
    }

    
    ret_val = _get_prop_from_ipv4_obj(ipv4_obj, RFPROP_MANAGER_ETH_IPGATEWAY, gateway_json_p, gateway, sizeof(gateway),
        *o_message_jso);
    if (ret_val == VOS_OK) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, gateway_json_p, &message_jso, gateway_json_p);
        (void)json_object_array_add(*o_message_jso, message_jso);
        return HTTP_NOT_IMPLEMENTED;
    }

    
    return_val_do_info_if_expr(0x3 != log_flag, response_code, (response_code = HTTP_BAD_REQUEST));
    GSList *input_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)address));
    input_list = g_slist_append(input_list, g_variant_new_string((const gchar *)netmask));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_BACKUP_IP_MAKS, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    if (VOS_OK == ret_val) {
        response_code = HTTP_OK;
    } else if (COMP_CODE_INVALID_CMD == ret_val) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, &message_jso,
            RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR);
        (void)json_object_array_add(*o_message_jso, message_jso);
        return HTTP_NOT_IMPLEMENTED;
    } else if (ERROR_PARAMETER_INVALID == ret_val) {
        (void)create_message_info(MSGID_INVALID_IPV4ADDRESS, address_json_p, &message_jso, (const gchar *)address,
            address_json_p);
        (void)json_object_array_add(*o_message_jso, message_jso);
    } else {
        (void)create_message_info(MSGID_IPV4_ADDRESS_CONFLCT, address_json, &message_jso);
        (void)json_object_array_add(*o_message_jso, message_jso);
    }

    return response_code;
}


LOCAL gint32 _set_nic_ipv4_static(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    guint8 ipver = 0;
    gint32 array_len;

    return_val_do_info_if_expr(i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, ERROR_LOG_NULL_POINTER, __FUNCTION__));
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, o_message_jso));
    // 判断是会否存在 DHCP和IP地址同时下发的场景
    ret_val = _check_dhcpv4_and_static_ip(i_paras->user_data);
    return_val_do_info_if_expr(ret_val == VOS_OK, ret_code,
        (void)create_message_info(MSGID_IPV4_INFO_CONFLICT_DHCP, NULL, o_message_jso));

    // IPv4 协议未使能返回错误
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    return_val_do_info_if_expr(ipver != ENABLE_IPV4_ONLY && ipver != ENABLE_IPV4_AND_IPV6, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, o_message_jso,
        RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, "IPv4"));

    // 数组有效性检查
    ret_val = rf_check_array_length_valid(*o_result_jso, 2, o_message_jso, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR);
    return_val_if_expr(ret_val != VOS_OK, ret_code);

    *o_message_jso = json_object_new_array();
    return_val_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    array_len = json_object_array_length(i_paras->val_jso);

    
    ret_val = set_nic_ipv4_static_addresses(i_paras, o_message_jso, o_result_jso, array_len);
    if (ret_val == HTTP_OK && array_len == 1) {
        ret_code = HTTP_OK;
    }

    
    if (2 == array_len) {
        ret_val = set_nic_backup_ipv4_static_addresses(i_paras, o_message_jso, o_result_jso);
        if (ret_val == HTTP_OK) {
            ret_code = HTTP_OK;
        }
    }

    return ret_code;
}


LOCAL gint32 _set_nic_dhcpv4_enable(PROVIDER_PARAS_S *i_paras, json_object **message_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *dhcp_enable_jso = NULL;
    const gchar *dhcpv4_enable_p = RFPROP_NIC_DHCPV4 "/" RFPROP_NIC_DHCP_ENABLED;
    guint8 ip_mode;
    json_bool json_bool_val;
    GSList *input_list = NULL;
    guint8 ipver = 0;

    ret_val = _check_dhcpv4_and_static_ip(i_paras->user_data);
    if (ret_val == VOS_OK) {
        return ret_code;
    }

    ret_val = json_object_object_get_ex(i_paras->val_jso, RFPROP_NIC_DHCP_ENABLED, &dhcp_enable_jso);
    if (ret_val != TRUE) {
        return ret_code;
    }

    // IPv4 协议未使能返回错误
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    return_val_do_info_if_expr(ipver != ENABLE_IPV4_ONLY && ipver != ENABLE_IPV4_AND_IPV6, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, dhcpv4_enable_p, message_jso, dhcpv4_enable_p,
        "IPv4"));

    if (json_object_get_type(dhcp_enable_jso) != json_type_boolean) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, dhcpv4_enable_p, message_jso, RF_NULL_STR, dhcpv4_enable_p);
        return ret_code;
    }

    json_bool_val = json_object_get_boolean(dhcp_enable_jso);
    ip_mode = (json_bool_val == TRUE) ? IP_DHCP_MODE : IP_STATIC_MODE;

    input_list = g_slist_append(input_list, g_variant_new_byte(ip_mode));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV4_MODE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call EthGroup.SetIpMode", ret_val);
    }

    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, dhcpv4_enable_p, message_jso, dhcpv4_enable_p);
            break;

        case COMP_CODE_INVALID_CMD:
            ret_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, dhcpv4_enable_p, message_jso, dhcpv4_enable_p);
            break;

        default:
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_dns_mode_ipver(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE ddns_obj, guint8 dns_mode, guint8 dns_ip_ver,
    const gchar *json_p, json_object **message_jso)
{
    guint8 ip_ver = 0;
    guint8 ipv4_mode = 0;
    guint8 ipv6_mode = 0;
    gint32 ret_val = 0;
    gint32 ret_code = HTTP_BAD_REQUEST;
    GSList *input_list = NULL;

    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ip_ver);

    if (dns_ip_ver == DNS_IP_VER_IPV4) {
        (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IP_MODE, &ipv4_mode);
        return_val_do_info_if_expr(ip_ver != ENABLE_IPV4_ONLY && ip_ver != ENABLE_IPV4_AND_IPV6, ret_code,
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, json_p, message_jso, json_p, IP_VER_IPV4));

        return_val_do_info_if_expr(ipv4_mode != IP_DHCP_MODE, ret_code,
            (void)create_message_info(MSGID_STATIC_IP_MODE, json_p, message_jso, IP_VER_IPV4));
    } else {
        // 设置DNS版本为IPv6
        (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6_mode);
        return_val_do_info_if_expr(ip_ver != ENABLE_IPV6_ONLY && ip_ver != ENABLE_IPV4_AND_IPV6, ret_code,
            (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, json_p, message_jso, json_p, IP_VER_IPV6));

        return_val_do_info_if_expr(ipv6_mode != IP_DHCP_MODE, ret_code,
            (void)create_message_info(MSGID_STATIC_IP_MODE, json_p, message_jso, IP_VER_IPV6));
    }

    if (dns_ip_ver != G_MAXUINT8) {
        input_list = g_slist_append(input_list, g_variant_new_byte(dns_ip_ver));
        ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ddns_obj,
            CLASS_NAME_DNSSETTING, METHOD_DNSSETTING_DNSIPVER, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
            NULL);
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        goto_label_do_info_if_expr(ret_val != VOS_OK, EXIT_ERROR_MSG,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call DNSSetting.SetDdnsIPVer", ret_val));
    }

    if (dns_mode != G_MAXUINT8) {
        input_list = g_slist_append(input_list, g_variant_new_byte(dns_mode));
        ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, ddns_obj,
            CLASS_NAME_DNSSETTING, METHOD_DNSSETTING_DNSMODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
            NULL);
        uip_free_gvariant_list(input_list);
        goto_label_do_info_if_expr(ret_val != VOS_OK, EXIT_ERROR_MSG,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call DNSSetting.SetDdnsMode", ret_val));
    }
EXIT_ERROR_MSG:
    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, json_p, message_jso, json_p);
            ret_code = HTTP_FORBIDDEN;
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_dhcp_use_for_dns(PROVIDER_PARAS_S *i_paras, const gchar *object_name, json_object **message_jso)
{
    

    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_bool json_bool_val;
    json_object *use_for_dns_jso = NULL;
    const gchar *dhcpv4_dns_p = RFPROP_NIC_DHCPV4 "/" RFPROP_NIC_USE_DNS_SERVER;
    const gchar *dhcpv6_dns_p = RFPROP_NIC_DHCPV6 "/" RFPROP_NIC_USE_DNS_SERVER;
    const gchar *prop_json_p = dhcpv4_dns_p;
    OBJ_HANDLE ddhs_obj = 0;
    guint8 dns_ipver = 0;
    guint8 to_set_ip_ver = DNS_IP_VER_IPV4;

    ret_val = json_object_object_get_ex(i_paras->val_jso, RFPROP_NIC_USE_DNS_SERVER, &use_for_dns_jso);
    if (ret_val != TRUE) {
        // 用户未输入 UseDNSServers，返回 400，不生成消息
        return ret_code;
    }

    do_if_expr(0 == g_strcmp0(object_name, RFPROP_NIC_DHCPV6), prop_json_p = dhcpv6_dns_p;
        to_set_ip_ver = DNS_IP_VER_IPV6);
    if (json_object_get_type(use_for_dns_jso) != json_type_boolean) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_json_p, message_jso, RF_NULL_STR, prop_json_p);
        return ret_code;
    }

    ret_val = dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &ddhs_obj);
    if (ret_val != DFL_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "get DNSSetting object handle", ret_val);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)dal_get_property_value_byte(ddhs_obj, PROPERTY_DNSSETTING_DNSIPVER, &dns_ipver);

    json_bool_val = json_object_get_boolean(use_for_dns_jso);
    if (json_bool_val == TRUE) {
        //  当前是IPv4对象，需要设置的IP版本就是IPv4
        //  当前是IPv6对象，需要设置的IP版本就是IPv6
        ret_val = _set_dns_mode_ipver(i_paras, ddhs_obj, DNS_ADD_ORIGIN_AUTO, 
            to_set_ip_ver, prop_json_p, message_jso);
        ret_code = ret_val;   
    } else {
        // 设置为FALSE时，当前的IP版本不是 要设置的IP版本，直接返回，相同，设置当前DNS模式为 手动
        return_val_if_expr(to_set_ip_ver != dns_ipver, HTTP_OK);
        ret_val = _set_dns_mode_ipver(i_paras, ddhs_obj, DNS_ADD_ORIGIN_MANUAL, 
            G_MAXUINT8, prop_json_p, message_jso);
        ret_code = ret_val;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_dhcpv4(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *dhcpv4_message = NULL;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, ERROR_LOG_NULL_POINTER, __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_NIC_DHCPV4, o_message_jso));
    return_val_if_expr(json_object_object_length(*o_result_jso) == 0, ret_code);

    *o_message_jso = json_object_new_array();
    return_val_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret_val = _set_nic_dhcpv4_enable(i_paras, &dhcpv4_message);
    goto_if_expr(ret_val == HTTP_FORBIDDEN, EXIT_ADD_MESSAGE);
    if (dhcpv4_message) {
        ret_val = json_object_array_add(*o_message_jso, dhcpv4_message);
        do_if_expr(ret_val != 0, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add dhcp enable message_jso to message_arr", ret_val);
            json_object_put(dhcpv4_message));
        dhcpv4_message = NULL;
    } else {
        // 有成功就返回 200
        do_if_expr(ret_val == HTTP_OK, (ret_code = HTTP_OK));
    }

    ret_val = _set_nic_dhcp_use_for_dns(i_paras, RFPROP_NIC_DHCPV4, &dhcpv4_message);
    goto_if_expr(ret_val == HTTP_FORBIDDEN, EXIT_ADD_MESSAGE);
    if (dhcpv4_message) {
        ret_val = json_object_array_add(*o_message_jso, dhcpv4_message);
        do_if_expr(ret_val != 0, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add dhcp use for dns message_jso to message_arr", ret_val);
            json_object_put(dhcpv4_message));
        dhcpv4_message = NULL;
    } else {
        // 有成功就返回 200
        do_if_expr(ret_val == HTTP_OK, (ret_code = HTTP_OK));
    }

EXIT_ADD_MESSAGE:
    if (dhcpv4_message) {
        ret_val = json_object_array_add(*o_message_jso, dhcpv4_message);
        do_if_expr(ret_val != 0,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add dhcpv4 message_jso to message_arr", ret_val);
            json_object_put(dhcpv4_message));
    }

    return ret_code;
}


LOCAL gint32 _check_dhcpv6_and_static_ip(json_object *request_body)
{
    gint32 ret_val;
    json_object *dhcp_obj_jso = NULL;
    json_object *ip_arr_jso = 0;
    const char *op_mode = NULL;
    json_bool ip_exit = FALSE;
    json_bool gateway_exit = FALSE;

    ret_val = json_object_object_get_ex(request_body, RFPROP_NIC_DHCPV6, &dhcp_obj_jso);
    if (ret_val == TRUE) {
        (void)get_element_str(dhcp_obj_jso, RFPROP_NIC_OPERATING_MODE, &op_mode);
        ip_exit = json_object_object_get_ex(request_body, RFPROP_MANAGER_ETH_IPV6_ADDR, &ip_arr_jso);
        ret_val = json_object_object_get_ex(request_body, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, &ip_arr_jso);
        gateway_exit = json_object_object_get_ex(request_body, RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS, &ip_arr_jso);
        if (0 == g_strcmp0(op_mode, NIC_OPERATING_MODE_STATEFUL) && (ip_exit || ret_val || gateway_exit)) {
            return VOS_OK;
        }
    }

    return VOS_ERR;
}


LOCAL gint32 _ipv6_common_check(PROVIDER_PARAS_S *i_paras, const gchar *ipv6_json_p, json_object **o_message_jso,
    json_object **ipv6_obj)
{
    gint32 ret_val;
    guint8 ipver = 0;

    ret_val = _check_dhcpv6_and_static_ip(i_paras->user_data);
    return_val_if_expr(ret_val == VOS_OK, HTTP_BAD_REQUEST);

    // IPv6 协议未使能返回错误
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    return_val_do_info_if_expr(ipver != ENABLE_IPV6_ONLY && ipver != ENABLE_IPV4_AND_IPV6, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, ipv6_json_p, o_message_jso, ipv6_json_p, "IPv6"));

    // 数组有效性检查
    ret_val = rf_check_array_length_valid(i_paras->val_jso, 1, o_message_jso, ipv6_json_p);
    return_val_if_expr(ret_val != VOS_OK, HTTP_BAD_REQUEST);

    *ipv6_obj = json_object_array_get_idx(i_paras->val_jso, 0);
    return_val_if_expr(json_object_object_length(*ipv6_obj) <= 0, HTTP_BAD_REQUEST);

    return ret_val;
}


LOCAL gint32 _check_ipv6_address_conflict_with_gateway(OBJ_HANDLE obj_handle, const gchar *method, const gchar *val_str,
    const gchar *josn_p, json_object **message_jso)
{
    gchar       current_ip[MAX_IPV6_LEN] = {0};
    const gchar *arg_3 = NULL;

    if (g_strcmp0(method, ETH_GROUP_METHOD_SET_IPV6_ADDR) == 0) {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_ETHGROUP_IPV6GATEWAY, current_ip, sizeof(current_ip));
        arg_3 = RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS "/0/" RFPROP_MANAGER_ETH_IPADDR;
    } else if (g_strcmp0(method, ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY) == 0) {
        (void)dal_get_property_value_string(obj_handle, PROPERTY_ETHGROUP_IPV6ADDR, current_ip, sizeof(current_ip));
        arg_3 = RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR;
    } else {
        return VOS_ERR;
    }

    if (g_strcmp0(current_ip, val_str) == 0) {
        (void)create_message_info(MSGID_IPV6_ADDRESS_CONFLICT_GATEWAY, josn_p, message_jso, val_str, josn_p, arg_3);
        return VOS_OK;
    }

    return VOS_ERR;
}


LOCAL gint32 _set_nic_ipv6_address_info(PROVIDER_PARAS_S *i_paras, json_object *ipv6_obj, const gchar *prop_name,
    const gchar *ipv6_json_p, const gchar *method_name, json_type json_type_val, json_object **message_jso)
{
    gint32 ret_val;
    json_object *ipv6_prop_json = NULL;
    gint32 ret_code = HTTP_BAD_REQUEST;
    const gchar *prop_val_string = NULL;
    guint8 prop_val_byte = 0;
    GSList *input_list = NULL;
    const gchar *msgid_val_err = MSGID_INVALID_IPV6ADDRESS;
    gchar       array_name[PROP_VAL_LENGTH] = {0};
    gchar *slash_point = NULL;

    ret_val = json_object_object_get_ex(ipv6_obj, prop_name, &ipv6_prop_json);
    return_val_if_expr(ret_val != TRUE, ret_code);

    if (json_object_get_type(ipv6_prop_json) != json_type_val) {
        prop_val_string = (ipv6_prop_json == NULL) ? RF_NULL_STR : dal_json_object_get_str(ipv6_prop_json);
        (void)create_message_info(MSGID_PROP_TYPE_ERR, ipv6_json_p, message_jso, prop_val_string, ipv6_json_p);
        return ret_code;
    }

    if (json_type_val == json_type_string) {
        prop_val_string = dal_json_object_get_str(ipv6_prop_json);
        input_list = g_slist_append(input_list, g_variant_new_string(prop_val_string));
    } else if (json_type_val == json_type_int) {
        prop_val_byte = (guint8)json_object_get_int(ipv6_prop_json); // schema中有范围限制
        input_list = g_slist_append(input_list, g_variant_new_byte(prop_val_byte));
    } else {
        return ret_code;
    }

    // 网关和IP地址相同，生成 网关和IP冲突的消息
    ret_val = _check_ipv6_address_conflict_with_gateway(i_paras->obj_handle, method_name, prop_val_string, ipv6_json_p,
        message_jso);
    return_val_do_info_if_expr(ret_val == VOS_OK, ret_code, uip_free_gvariant_list(input_list));

    ret_val =
        uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->obj_handle,
        CLASS_NAME_ETHGROUP, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call EthGroup.method_name", ret_val);
    }

    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            ret_val = strncpy_s(array_name, sizeof(array_name), ipv6_json_p, strlen(ipv6_json_p));
            do_if_expr(ret_val != EOK,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s ipv6_json_p", ret_val));
            slash_point = strstr(array_name, SLASH_FLAG_STR);
            do_if_expr(slash_point, (*slash_point = 0));
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, (const gchar *)array_name, message_jso,
                (const gchar *)array_name);
            break;

        case COMP_CODE_INVALID_CMD:
            ret_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ipv6_json_p, message_jso, ipv6_json_p);
            break;

        case REDFISH_ETH_ERR:
        case REDFISH_ETH_PARAM_NOT_VALIID:
            do_if_expr(0 == g_strcmp0(method_name, ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY),
                (msgid_val_err = MSGID_INVALID_IPV6GATEWAY));
            (void)create_message_info(msgid_val_err, ipv6_json_p, message_jso, prop_val_string, ipv6_json_p);
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, ipv6_json_p, message_jso);
            ret_val = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_ipv6_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *ipv6_obj = NULL;
    const gchar *address_json_p = RFPROP_MANAGER_ETH_IPV6_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR;
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_ETH_IPV6_ADDR, o_message_jso));
    ret_val = _ipv6_common_check(i_paras, RFPROP_MANAGER_ETH_IPV6_ADDR, o_message_jso, &ipv6_obj);
    return_val_if_expr(ret_val != VOS_OK, ret_code);

    ret_code = _set_nic_ipv6_address_info(i_paras, ipv6_obj, RFPROP_MANAGER_ETH_IPADDR, address_json_p,
        ETH_GROUP_METHOD_SET_IPV6_ADDR, json_type_string, o_message_jso);

    return ret_code;
}


LOCAL gint32 _set_nic_ipv6_static(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *ipv6_obj = NULL;
    json_object *message_jso = NULL;
    const gchar *address_json_p = RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPADDR;
    const gchar *prefix_json_p = RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR "/0/" RFPROP_MANAGER_ETH_IPV6_PREFIXLEN;
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, o_message_jso));
    ret_val = _ipv6_common_check(i_paras, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, o_message_jso, &ipv6_obj);
    return_val_if_expr(ret_val != VOS_OK, ret_code);

    *o_message_jso = json_object_new_array();
    return_val_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret_val = _set_nic_ipv6_address_info(i_paras, ipv6_obj, RFPROP_MANAGER_ETH_IPADDR, address_json_p,
        ETH_GROUP_METHOD_SET_IPV6_ADDR, json_type_string, &message_jso);
    goto_if_expr(ret_val == HTTP_FORBIDDEN, EXIT_ADD_MESSAGE);
    if (message_jso) {
        ret_val = json_object_array_add(*o_message_jso, message_jso);
        do_if_expr(ret_val != VOS_OK, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add static ipv6 address message_jso to message_arr", ret_val);
            json_object_put(message_jso));
        message_jso = NULL;
    } else {
        // 有成功就返回 200
        do_if_expr(ret_val == HTTP_OK, (ret_code = HTTP_OK));
    }

    ret_val = _set_nic_ipv6_address_info(i_paras, ipv6_obj, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN, prefix_json_p,
        ETH_GROUP_METHOD_SET_IPV6_PREFIX, json_type_int, &message_jso);
    goto_if_expr(ret_val == HTTP_FORBIDDEN, EXIT_ADD_MESSAGE);
    if (message_jso) {
        ret_val = json_object_array_add(*o_message_jso, message_jso);
        do_if_expr(ret_val != VOS_OK, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add static ipv6 prefix message_jso to message_arr", ret_val);
            json_object_put(message_jso));
        message_jso = NULL;
    } else {
        // 有成功就返回 200
        do_if_expr(ret_val == HTTP_OK, (ret_code = HTTP_OK));
    }

EXIT_ADD_MESSAGE:
    if (message_jso) {
        ret_val = json_object_array_add(*o_message_jso, message_jso);
        do_if_expr(ret_val != VOS_OK, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add ipv6 static message_jso to message_arr", ret_val);
            json_object_put(message_jso));
    }

    return ret_code;
}


LOCAL gint32 _set_nic_ipv6_static_gw(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *ipv6_obj = NULL;
    const gchar *address_json_p = RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS "/0/" RFPROP_MANAGER_ETH_IPADDR;

    ret_val = _ipv6_common_check(i_paras, RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS, o_message_jso, &ipv6_obj);
    return_val_if_expr(ret_val != VOS_OK, ret_code);

    ret_code = _set_nic_ipv6_address_info(i_paras, ipv6_obj, RFPROP_MANAGER_ETH_IPADDR, address_json_p,
        ETH_GROUP_METHOD_SET_IPV6_DEFAULT_GATEWAY, json_type_string, o_message_jso);

    return ret_code;
}


LOCAL gint32 _set_nic_dhcpv6_op_mode(PROVIDER_PARAS_S *i_paras, json_object **message_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *op_mode_jso = 0;
    const gchar *op_mode_str = NULL;
    const gchar *op_mode_json_p = RFPROP_NIC_DHCPV6 "/" RFPROP_NIC_OPERATING_MODE;
    guint8 ipv6_mode = 0;
    GSList *input_list = NULL;
    guint8 ipver = 0;

    ret_val = _check_dhcpv6_and_static_ip(i_paras->user_data);
    return_val_do_info_if_expr(ret_val == VOS_OK, ret_code,
        (void)create_message_info(MSGID_IPV6_INFO_CONFLICT_DHCPV6, NULL, message_jso));

    ret_val = json_object_object_get_ex(i_paras->val_jso, RFPROP_NIC_OPERATING_MODE, &op_mode_jso);
    if (ret_val != TRUE) {
        return ret_code;
    }

    if (json_object_get_type(op_mode_jso) != json_type_string) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, op_mode_json_p, message_jso, RF_NULL_STR, op_mode_json_p);
        return ret_code;
    }

    op_mode_str = dal_json_object_get_str(op_mode_jso);
    if (0 == g_strcmp0(op_mode_str, NIC_OPERATING_MODE_STATEFUL)) {
        ipv6_mode = IP_DHCP_MODE;
    } else if (0 == g_strcmp0(op_mode_str, NIC_OPERATING_MODE_DISABLED)) {
        ipv6_mode = IP_STATIC_MODE;
    }

    if (ipv6_mode == 0) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, op_mode_json_p, message_jso, op_mode_str, op_mode_json_p);
        return ret_code;
    }

    // IPv6 协议未使能返回错误
    (void)dal_get_property_value_byte(i_paras->obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ipver);
    return_val_do_info_if_expr(ipver != ENABLE_IPV6_ONLY && ipver != ENABLE_IPV4_AND_IPV6, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_IPVER, op_mode_json_p, message_jso, op_mode_json_p, "IPv6"));

    input_list = g_slist_append(input_list, g_variant_new_byte(ipv6_mode));
    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_NAME_ETHGROUP, ETH_GROUP_METHOD_SET_IPV6_MODE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "call EthGroup.SetIpv6Mode", ret_val);
    }

    switch (ret_val) {
        case VOS_OK:
            ret_code = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            ret_code = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, op_mode_json_p, message_jso, op_mode_json_p);
            break;

        case COMP_CODE_INVALID_CMD:
            ret_code = HTTP_NOT_IMPLEMENTED;
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, op_mode_json_p, message_jso, op_mode_json_p);
            break;

        default:
            ret_code = HTTP_INTERNAL_SERVER_ERROR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            break;
    }

    return ret_code;
}


LOCAL gint32 _set_nic_dhcpv6(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;
    json_object *dhcpv6_message = NULL;
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_NIC_DHCPV6, o_message_jso));
    return_val_if_expr(json_object_object_length(*o_result_jso) == 0, ret_code);

    *o_message_jso = json_object_new_array();
    return_val_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    ret_val = _set_nic_dhcpv6_op_mode(i_paras, &dhcpv6_message);
    goto_if_expr(ret_val == HTTP_FORBIDDEN, EXIT_ADD_MESSAGE);
    if (dhcpv6_message) {
        ret_val = json_object_array_add(*o_message_jso, dhcpv6_message);
        do_if_expr(ret_val != 0, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add dhcp operate mode message_jso to message_arr", ret_val);
            json_object_put(dhcpv6_message));
        dhcpv6_message = NULL;
    } else {
        // 有成功就返回 200
        do_if_expr(ret_val == HTTP_OK, (ret_code = HTTP_OK));
    }

    ret_val = _set_nic_dhcp_use_for_dns(i_paras, RFPROP_NIC_DHCPV6, &dhcpv6_message);
    goto_if_expr(ret_val == HTTP_FORBIDDEN, EXIT_ADD_MESSAGE);
    if (dhcpv6_message) {
        ret_val = json_object_array_add(*o_message_jso, dhcpv6_message);
        do_if_expr(ret_val != 0, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__,
            "add dhcp use for dns message_jso to message_arr", ret_val);
            json_object_put(dhcpv6_message));
        dhcpv6_message = NULL;
    } else {
        // 有成功就返回 200
        do_if_expr(ret_val == HTTP_OK, (ret_code = HTTP_OK));
    }

EXIT_ADD_MESSAGE:
    if (dhcpv6_message) {
        ret_val = json_object_array_add(*o_message_jso, dhcpv6_message);
        do_if_expr(ret_val != 0,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "add dhcpv6 message_jso to message_arr", ret_val);
            json_object_put(dhcpv6_message));
    }

    return ret_code;
}


LOCAL gint32 _set_nic_name_server(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guint32 arr_len, i;
    guint8 empty_count = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 dns_mode = 0;
    json_object *array_item = NULL;
    json_object *message_jso = NULL;
    const gchar *name_address = NULL;
    json_type item_type = json_type_null;
    gint32 ret_code = 0;
    GSList *input_list = NULL;
    gint32 ret_val;
    const gchar *method_name = METHOD_DNSSETTING_PRIMARYDOMAIN;
    gchar           json_p[PROP_VAL_LENGTH] = {0};

    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_MANAGER_ETH_STATIC_NAMESERVER, o_message_jso));

    arr_len = json_object_array_length(*o_result_jso);
    return_val_do_info_if_expr(arr_len != DNS_SERVER_CNT, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_WRONG_ARRAY_LENGTH, RFPROP_MANAGER_ETH_STATIC_NAMESERVER, o_message_jso,
        "3"));

    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
    
    return_val_do_info_if_expr(dns_mode == DNS_ADD_ORIGIN_AUTO, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MODIFY_FAILED_WITH_AUTO_DNS, RFPROP_MANAGER_ETH_STATIC_NAMESERVER,
        o_message_jso));

    *o_message_jso = json_object_new_array();
    return_val_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    for (i = 0; i < arr_len; i++) {
        name_address = NULL;
        array_item = json_object_array_get_idx(*o_result_jso, i);
        item_type = json_object_get_type(array_item);
        if (item_type == json_type_null) {
            // null 表示清空
            name_address = "";
        } else if (item_type == json_type_object) {
            empty_count += 1;
            continue;
        } else {
            name_address = dal_json_object_get_str(array_item);
        }

        do_if_expr(i == ALTERNATE_DNS_SERVER_INDEX, (method_name = METHOD_DNSSETTING_BACKUPDOMAIN));
        do_if_expr(i == TERTIARY_DNS_SERVER_INDEX, (method_name = METHOD_DNSSETTING_TERTIARYDOMAIN));

        input_list = g_slist_append(input_list, g_variant_new_string(name_address));
        ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_NAME_DNSSETTING, method_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
        uip_free_gvariant_list(input_list);
        input_list = NULL;
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: call DNSSetting.%s failed, ret = %d", __FUNCTION__, method_name, ret_val));
        switch (ret_val) {
            case VOS_OK:
                ret_code = HTTP_OK;
                break;

            case RFERR_INSUFFICIENT_PRIVILEGE:
                json_object_put(*o_message_jso);
                *o_message_jso = NULL;
                (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_MANAGER_ETH_STATIC_NAMESERVER,
                    o_message_jso, RFPROP_MANAGER_ETH_STATIC_NAMESERVER);
                return HTTP_FORBIDDEN;

            default:
                (void)memset_s(json_p, sizeof(json_p), 0, sizeof(json_p));
                ret_val = snprintf_s(json_p, sizeof(json_p), sizeof(json_p) - 1, "%s/%u",
                    RFPROP_MANAGER_ETH_STATIC_NAMESERVER, i);
                do_if_expr(ret_val <= 0,
                    debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s json_p", ret_val));
                (void)create_message_info(MSGID_INVALID_NAMESERVER, (const gchar *)json_p, &message_jso,
                    (const gchar *)name_address, (const gchar *)json_p);
                ret_val = json_object_array_add(*o_message_jso, message_jso);
                do_if_expr(ret_val != 0,
                    debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "json_object_array_add", ret_val);
                    json_object_put(message_jso));
                break;
        }
    }

    return_val_do_info_if_expr((empty_count == DNS_SERVER_CNT), HTTP_BAD_REQUEST, json_object_put(*o_message_jso);
        *o_message_jso = NULL; (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM,
        RFPROP_MANAGER_ETH_STATIC_NAMESERVER, o_message_jso, RFPROP_MANAGER_ETH_STATIC_NAMESERVER););

    return ret_code;
}


LOCAL gint32 _set_nic_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei = NULL;
    json_object *prop_jso = NULL;
    gint32 ret_val;
    gint32 ret_code = HTTP_BAD_REQUEST;

    if (json_object_object_length(*o_result_jso) == 0) {
        return HTTP_BAD_REQUEST;
    }
    return_val_do_info_if_expr(i_paras->obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_UNENABLED_NETWORK_PORT, RFPROP_OEM, o_message_jso));
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_OEM, o_message_jso, RFPROP_OEM));

    ret_val = json_object_object_get_ex(*o_result_jso, RFPROP_OEM_HUAWEI, &huawei);
    return_val_if_expr(ret_val != TRUE, HTTP_BAD_REQUEST);

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_array fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret_val = json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_IPVER, &prop_jso);
    if (ret_val == TRUE) {
        ret_val = redfish_set_ip_version(i_paras->obj_handle, prop_jso, o_message_jso, i_paras);
        do_if_expr(ret_val == VOS_OK, (ret_code = HTTP_OK));
        prop_jso = NULL;
    }

    ret_val = json_object_object_get_ex(huawei, RFPROP_MANAGER_ETH_OEM_DNS_MODE, &prop_jso);
    if (ret_val == TRUE) {
        ret_val = redfish_set_dns_mode(prop_jso, o_message_jso, i_paras);
        do_if_expr(ret_val == VOS_OK, (ret_code = HTTP_OK));
    }

    return ret_code;
}


LOCAL gint32 _get_type_portnum_from_memberid(const gchar *memberid, guint8 *card_type, guint8 *port_num)
{
    _cleanup_gfree_ gchar *low_member_id = NULL;
    gchar *number_flag = NULL;
    gint32 ret_val;

    if (card_type == NULL || port_num == NULL || memberid == NULL) {
        return RET_ERR;
    }

    low_member_id = g_ascii_strdown(memberid, strlen(memberid));
    if (low_member_id == NULL) {
        return RET_ERR;
    }

    number_flag = strstr(low_member_id, LOW_PORT_FLAG);
    if (number_flag == NULL) {
        return VOS_ERR;
    }
    number_flag += strlen(LOW_PORT_FLAG);

    ret_val = vos_str2int(number_flag, 10, port_num, NUM_TPYE_UCHAR);
    if (ret_val != VOS_OK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "vos_str2int port id", ret_val);
        return VOS_ERR;
    }

    if (strstr(low_member_id, LOW_DEDICATED_FLAG)) {
        *card_type = NET_TYPE_DEDICATED;
    } else if (strstr(low_member_id, LOW_AGGREGATION_FLAG)) {
        *card_type = NET_TYPE_AGGREGATION;
    } else if (strstr(low_member_id, LOW_LOM2_FLAG)) {
        *card_type = NET_TYPE_LOM2;
    } else if (strstr(low_member_id, LOW_LOM_FLAG)) {
        *card_type = NET_TYPE_LOM;
    } else if (strstr(low_member_id, LOW_PCIE_FLAG)) {
        *card_type = NET_TYPE_PCIE;
    } else if (strstr(low_member_id, LOW_OCP2_FLAG)) {
        
        *card_type = NET_TYPE_OCP2_CARD;
    } else if (strstr(low_member_id, LOW_OCP_FLAG)) { 
        *card_type = NET_TYPE_OCP_CARD;
    } else {
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL void get_card_name(guint8 card_type, OBJ_HANDLE busi_obj_handle, gchar **card_name, gchar *com_device_name,
    guint32 name_len)
{
    gint32 ret_val;
    OBJ_HANDLE net_obj_handle = 0;
    OBJ_HANDLE com_obj_handle = 0;
    guint8 virtual_flag = 0;

    ret_val = dfl_get_referenced_object(busi_obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &net_obj_handle);
    if (ret_val != DFL_OK) {
        return;
    }
    (void)dal_get_property_value_byte(net_obj_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
    
    if (card_type == NET_TYPE_PCIE && virtual_flag == 1) {
        ret_val = strncpy_s(com_device_name, name_len, NET_TYPE_PCIE_STR, strlen(NET_TYPE_PCIE_STR));
        if (ret_val != EOK) {
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s", ret_val);
        }
        *card_name = com_device_name;
    } else if (card_type == NET_TYPE_OCP_CARD && virtual_flag == 1) {
        ret_val = strncpy_s(com_device_name, name_len, NET_TYPE_OCP_STR, strlen(NET_TYPE_OCP_STR));
        if (ret_val != EOK) {
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s", ret_val);
        }
        *card_name = com_device_name;
    } else if (card_type == NET_TYPE_OCP2_CARD && virtual_flag == 1) {
        ret_val = strncpy_s(com_device_name, name_len, NET_TYPE_OCP2_STR, strlen(NET_TYPE_OCP2_STR));
        if (ret_val != EOK) {
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s", ret_val);
        }
        *card_name = com_device_name;
    } else {
        ret_val = dfl_get_referenced_object(net_obj_handle, PROPERTY_NETCARD_REF_COMPONENT, &com_obj_handle);
        if (ret_val != DFL_OK) {
            return;
        }
        (void)dal_get_property_value_string(com_obj_handle, PROPERTY_COMPONENT_DEVICE_NAME, com_device_name, name_len);
        *card_name = dal_clear_string_blank(com_device_name, name_len);
    }
}


LOCAL void _get_nic_rsc_name(guint8 card_type, guint8 port_num, gchar *nic_name_buf, guint32 nic_name_len)
{
    GSList *busi_list = NULL;
    GSList *node = NULL;
    gint32 ret_val;
    guint8 i_card_type = 0;
    guint8 i_silk_num = 0;
    OBJ_HANDLE busi_obj_handle = 0;
    gchar       com_device_name[PROP_VAL_LENGTH] = {0};
    gchar *card_name_pointer = NULL;

    return_do_info_if_expr(nic_name_len == 0,
        debug_log(DLOG_ERROR, "%s failed:input param error, nic_name_len is 0.", __FUNCTION__));

    ret_val = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &busi_list);
    return_if_expr(ret_val != DFL_OK);

    for (node = busi_list; node; node = g_slist_next(node)) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)(node->data), BUSY_ETH_ATTRIBUTE_CARD_TYPE, &i_card_type);
        (void)dal_get_property_value_byte((OBJ_HANDLE)(node->data), BUSY_ETH_ATTRIBUTE_SILK_NUM, &i_silk_num);

        if ((i_card_type == card_type) && (i_silk_num == port_num)) {
            busi_obj_handle = (OBJ_HANDLE)(node->data);
            break;
        }
    }
    g_slist_free(busi_list);

    if (busi_obj_handle == 0) {
        return;
    }

    get_card_name(card_type, busi_obj_handle, &card_name_pointer, com_device_name, PROP_VAL_LENGTH);

    ret_val = snprintf_s(nic_name_buf, nic_name_len, nic_name_len - 1, SHARED_NAME, card_name_pointer, port_num);
    do_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s name", ret_val))

        return;
}


LOCAL void get_dedicate_name_and_template(guint8 port_number, guint8 card_type, gchar *final_name, guint32 name_len,
    const gchar **member_id_template)
{
    gint32 ret_val;
    if (card_type != NET_TYPE_DEDICATED) {
        return;
    }
    *member_id_template = DEDICATED_ID;
    ret_val = snprintf_s(final_name, name_len, name_len - 1, DEDICATED_NAME, port_number);
    do_if_expr(ret_val <= 0,
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s dedicated name", ret_val));
    return;
}


LOCAL void get_aggregation_name_and_id(guint8 card_type, gchar *final_name, guint32 name_len, gchar *final_member_id,
    guint32 id_len)
{
    gint32 ret_val;
    if (card_type != NET_TYPE_AGGREGATION) {
        return;
    }
    ret_val = strncpy_s(final_member_id, id_len, AGGREGATION_ID, strlen(AGGREGATION_ID));
    do_if_expr(ret_val != EOK,
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s aggregation id", ret_val));
    ret_val = strncpy_s(final_name, name_len, AGGREGATION_NAME, strlen(AGGREGATION_NAME));
    do_if_expr(ret_val != EOK,
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s aggregation name", ret_val));
    return;
}


LOCAL void get_other_cardtype_template(guint8 card_type, const gchar **member_id_template)
{
    if (card_type == NET_TYPE_LOM) {
        *member_id_template = LOM_ID;
    } else if (card_type == NET_TYPE_LOM2) {
        *member_id_template = LOM2_ID;
    } else if (card_type == NET_TYPE_PCIE) {
        *member_id_template = PCIE_ID;
    } else if (card_type == NET_TYPE_OCP_CARD) {
        *member_id_template = OCP_ID;
    } else if (card_type == NET_TYPE_OCP2_CARD) {
        *member_id_template = OCP2_ID;
    } else {
        return;
    }
}


LOCAL void _get_nic_odata_prop(const gchar *memberid, json_object *o_rsc_jso)
{
    gchar odata_prop_buf[MAX_RSC_URI_LEN] = {0};
    gint32 ret_val;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar final_member_id[MAX_RSC_ID_LEN] = {0};
    gchar final_name[MAX_RSC_URI_LEN] = {0};
    const gchar *member_id_template = NULL;
    guint8 port_number = 0;
    guint8 card_type = 0;

    ret_val = _get_type_portnum_from_memberid(memberid, &card_type, &port_number);
    return_if_expr(ret_val != VOS_OK);

    get_dedicate_name_and_template(port_number, card_type, final_name, MAX_RSC_URI_LEN, &member_id_template);
    get_aggregation_name_and_id(card_type, final_name, MAX_RSC_URI_LEN, final_member_id, MAX_RSC_ID_LEN);
    get_other_cardtype_template(card_type, &member_id_template);

    if (strlen(final_member_id) == 0 && member_id_template != NULL) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
        ret_val = snprintf_s(final_member_id, sizeof(final_member_id), sizeof(final_member_id) - 1, member_id_template,
            port_number);
#pragma GCC diagnostic pop
        do_if_expr(ret_val <= 0,
            debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "snprintf_s member id", ret_val));
    }
    (void)redfish_get_board_slot(slot_id, sizeof(slot_id));
    ret_val = snprintf_s(odata_prop_buf, sizeof(odata_prop_buf), sizeof(odata_prop_buf) - 1, URI_FORMAT_MANAGER_NIC,
        slot_id, final_member_id);
    if (ret_val > 0) {
        json_object_object_add(o_rsc_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)odata_prop_buf));
    }
    json_object_object_add(o_rsc_jso, RFPROP_ID, json_object_new_string((const gchar *)final_member_id));
    if (strlen(final_name) == 0) {
        _get_nic_rsc_name(card_type, port_number, final_name, sizeof(final_name));
    }
    json_object_object_add(o_rsc_jso, RFPROP_NAME, json_object_new_string((const gchar *)final_name));
    return;
}


LOCAL void _get_nic_interface_enable(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    json_bool bool_val = (eth_group_obj == 0) ? FALSE : TRUE;

    json_object_object_add(o_rsc_jso, RFPROP_NIC_INTERFACE_ENABLED, json_object_new_boolean(bool_val));

    return;
}


LOCAL void _get_nic_mac(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar   mac_addr[MACADDRESS_LEN + 1] = {0};

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, PROPERTY_ETHGROUP_MACADDR, mac_addr, sizeof(mac_addr));
    }
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_MACADDR,
        strlen(mac_addr) ? json_object_new_string((const gchar *)mac_addr) : NULL);

    return;
}


LOCAL void _get_nic_host_name(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar       host_name[HOST_NAME_MAX_LEN + 1] = {0};
    OBJ_HANDLE bmc_obj_handle = 0;

    if (eth_group_obj > 0) {
        (void)dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &bmc_obj_handle);
        (void)dal_get_property_value_string(bmc_obj_handle, PROPERTY_BMC_HOSTNAME, host_name, sizeof(host_name));
    }
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_BMC_HOSTNAME,
        strlen(host_name) ? json_object_new_string((const gchar *)host_name) : NULL);

    return;
}


void get_nic_fqdn(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar       host_name[HOST_NAME_MAX_LEN + 1] = {0};
    gchar       domain_name[DNS_DOMAINNAME_MAX_LEN + 1] = {0};
    gchar       fqdn[DNS_DOMAINNAME_MAX_LEN + HOST_NAME_MAX_LEN + 2] = {0}; // +2的原因是，一个是点，一个是字符串结束符
    gint32 ret_val = 0;
    json_object *fqdn_jso = NULL;

    if (eth_group_obj > 0) {
        (void)dal_get_object_handle_nth(CLASS_NAME_BMC, 0, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_BMC_HOSTNAME, host_name, sizeof(host_name));
        obj_handle = 0;
        (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_DOMAINNAME, domain_name,
            sizeof(domain_name));
    }

    if (strlen(host_name)) {
        ret_val = strncpy_s(fqdn, sizeof(fqdn), host_name, strlen(host_name));
        do_if_expr(ret_val != EOK, debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s", ret_val));

        if (strlen(domain_name) > 0) { // 如果域名有效，拼接域名
            ret_val = strncat_s(fqdn, sizeof(fqdn), ".", strlen("."));
            do_if_expr(ret_val != EOK,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncat_s .", ret_val));
            ret_val = strncat_s(fqdn, sizeof(fqdn), (const gchar *)domain_name, strlen(domain_name));
            do_if_expr(ret_val != EOK,
                debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncat_s domain_name", ret_val));
        }
        fqdn_jso = json_object_new_string((const gchar *)fqdn);
    }
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_BMC_FQDN, fqdn_jso);

    return;
}


LOCAL void _get_nic_vlan_info(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    json_object *vlan_jso = NULL;
    guint8 net_mode_flag = DISABLE;
    guint32 vlan_state = G_MAXUINT8;
    guint16 vlan_id = G_MAXUINT16;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_byte(eth_group_obj, PROPERTY_ETHGROUP_NETMODEFLAG, &net_mode_flag);
        (void)dal_get_ethgroup_vlan_info(eth_group_obj, &vlan_state, &vlan_id);
    }

    if (net_mode_flag == ENABLE) {
        vlan_jso = json_object_new_object();
        if (vlan_jso) {
            json_object_object_add(vlan_jso, RFPROP_MANAGER_ETH_VLAN_STATE,
                (vlan_state == ENABLE || vlan_state == DISABLE) ? json_object_new_boolean(vlan_state) : NULL);
            json_object_object_add(vlan_jso, RFPROP_MANAGER_ETH_VLAN_ID,
                (vlan_id <= RF_MAX_VLAN_ID) ? json_object_new_int(vlan_id) : NULL);
        }
    }
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_VLAN, vlan_jso);

    return;
}


LOCAL void _get_nic_ipv4_addresses(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar       address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar       subnetmask[IPV4_MASK_STR_SIZE + 1] = {0};
    guint8 ipv4_mode = 0;
    const gchar *ipv4_mode_str = NULL;
    gchar       gateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    json_object *ipv4_array_jso = NULL;
    json_object *ipv4_obj_jso = NULL;
    gint32 ret_val;
    guint8 virtual_net_status = DISABLE;
    gchar default_addr[IPV4_IP_STR_SIZE + 1] = {0};
    gchar default_mask[IPV4_MASK_STR_SIZE + 1] = {0};
    GSList *output_list = NULL;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IP_ADDR, address, sizeof(address));
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_SUB_MASK, subnetmask,
            sizeof(subnetmask));
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IP_MODE, &ipv4_mode);
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_GATEWAY, gateway, sizeof(gateway));
    }
    ipv4_mode_str = (ipv4_mode == IP_STATIC_MODE) ? IP_MODE_STATIC_STRING :
                                                    ((ipv4_mode == IP_DHCP_MODE) ? IP_MODE_DHCP_STRING : NULL);

    ipv4_array_jso = json_object_new_array();
    goto_if_expr(ipv4_array_jso == NULL || ipv4_mode_str == NULL,
        EXIT_ADD_TO_RSC); // IP模式为空，认为该NIC不是当前管理网口
    ipv4_obj_jso = json_object_new_object();
    goto_if_expr(ipv4_obj_jso == NULL, EXIT_ADD_TO_RSC);
    ret_val = json_object_array_add(ipv4_array_jso, ipv4_obj_jso);
    goto_label_do_info_if_expr(ret_val != 0, EXIT_ADD_TO_RSC, json_object_put(ipv4_obj_jso));

    json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPADDR,
        strlen((const gchar *)address) ? json_object_new_string((const gchar *)address) : NULL);
    json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPMASK,
        strlen((const gchar *)subnetmask) ? json_object_new_string((const gchar *)subnetmask) : NULL);
    json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPMODE,
        (ipv4_mode_str != NULL) ? json_object_new_string(ipv4_mode_str) : NULL);
    json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPGATEWAY,
        strlen(gateway) ? json_object_new_string((const gchar *)gateway) : NULL);

    
    ret_val = dfl_call_class_method(eth_group_obj, ETH_GROUP_METHOD_VIRTUAL_NETWORK_STATUS, NULL, NULL, &output_list);
    if (VOS_OK == ret_val) {
        virtual_net_status = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        if (ENABLE == virtual_net_status) {
            json_object *obj_json_virtual_ipv4 = NULL;

            
            ipv4_mode_str = IP_MODE_STATIC_STRING;
            obj_json_virtual_ipv4 = json_object_new_object();
            goto_if_expr(obj_json_virtual_ipv4 == NULL, EXIT_ADD_TO_RSC);
            (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_BACKUP_IP_ADDR, default_addr,
                sizeof(default_addr));
            (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_BACKUP_SUB_MASK, default_mask,
                sizeof(default_mask));

            json_object_object_add(obj_json_virtual_ipv4, RFPROP_MANAGER_ETH_IPADDR,
                0 == strlen(default_addr) ? NULL : json_object_new_string((const gchar *)default_addr));
            json_object_object_add(obj_json_virtual_ipv4, RFPROP_MANAGER_ETH_IPMASK,
                0 == strlen(default_mask) ? NULL : json_object_new_string((const gchar *)default_mask));
            json_object_object_add(obj_json_virtual_ipv4, RFPROP_MANAGER_ETH_IPMODE,
                json_object_new_string(ipv4_mode_str));
            json_object_array_add(ipv4_array_jso, obj_json_virtual_ipv4);
        }
    }

EXIT_ADD_TO_RSC:
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_IPV4ADDR, ipv4_array_jso);

    return;
}


LOCAL void _get_nic_ipv4_static_addresses(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar       address[IPV4_IP_STR_SIZE + 1] = {0};
    gchar       subnetmask[IPV4_MASK_STR_SIZE + 1] = {0};
    guint8 ipv4_mode = 0;
    gchar       gateway[IPV4_GATEWAY_STR_SIZE + 1] = {0};
    json_object *ipv4_array_jso = NULL;
    json_object *ipv4_obj_jso = NULL;
    gint32 ret_val;
    gchar default_addr[IPV4_IP_STR_SIZE + 1] = {0};
    gchar default_mask[IPV4_MASK_STR_SIZE + 1] = {0};

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IP_ADDR, address, sizeof(address));
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_SUB_MASK, subnetmask,
            sizeof(subnetmask));
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IP_MODE, &ipv4_mode);
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_GATEWAY, gateway, sizeof(gateway));
    }

    ipv4_array_jso = json_object_new_array();
    goto_if_expr(ipv4_array_jso == NULL || (IP_STATIC_MODE != ipv4_mode && IP_DHCP_MODE != ipv4_mode),
        EXIT_ADD_TO_RSC); // 非静态模式，返回空数组
    ipv4_obj_jso = json_object_new_object();
    goto_if_expr(ipv4_obj_jso == NULL, EXIT_ADD_TO_RSC);
    ret_val = json_object_array_add(ipv4_array_jso, ipv4_obj_jso);
    goto_label_do_info_if_expr(ret_val != 0, EXIT_ADD_TO_RSC, json_object_put(ipv4_obj_jso));
    if (IP_STATIC_MODE == ipv4_mode) {
        json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPADDR,
            strlen((const gchar *)address) ? json_object_new_string((const gchar *)address) : NULL);
        json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPMASK,
            strlen((const gchar *)subnetmask) ? json_object_new_string((const gchar *)subnetmask) : NULL);
        json_object_object_add(ipv4_obj_jso, RFPROP_MANAGER_ETH_IPGATEWAY,
            strlen((const gchar *)gateway) ? json_object_new_string((const gchar *)gateway) : NULL);
    }

    
    ret_val = dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_BACKUP_IP_ADDR, default_addr,
        sizeof(default_addr));
    if (VOS_OK == ret_val) {
        if (0 != strlen(default_addr)) {
            json_object *obj_json_virtual_ipv4 = NULL;

            
            obj_json_virtual_ipv4 = json_object_new_object();
            goto_if_expr(obj_json_virtual_ipv4 == NULL, EXIT_ADD_TO_RSC);
            (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_BACKUP_SUB_MASK, default_mask,
                sizeof(default_mask));

            json_object_object_add(obj_json_virtual_ipv4, RFPROP_MANAGER_ETH_IPADDR,
                0 == strlen(default_addr) ? NULL : json_object_new_string((const gchar *)default_addr));
            json_object_object_add(obj_json_virtual_ipv4, RFPROP_MANAGER_ETH_IPMASK,
                0 == strlen(default_mask) ? NULL : json_object_new_string((const gchar *)default_mask));
            json_object_array_add(ipv4_array_jso, obj_json_virtual_ipv4);
        }
    }

EXIT_ADD_TO_RSC:
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_IPV4_STATIC_ADDR, ipv4_array_jso);

    return;
}


LOCAL void _get_nic_dhcpv4(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    json_object *dhcpv4_jso = NULL;
    json_bool json_bool_val = FALSE;
    guint8 ipv4_mode = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 dns_mode = 0;
    guint8 dns_ip_ver = 0;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IP_MODE, &ipv4_mode);
        (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSIPVER, &dns_ip_ver);
    }

    dhcpv4_jso = json_object_new_object();
    if (dhcpv4_jso) { // 非当前管理网口都是false
        json_bool_val = (ipv4_mode == IP_DHCP_MODE) ? TRUE : FALSE;
        json_object_object_add(dhcpv4_jso, RFPROP_NIC_DHCP_ENABLED, json_object_new_boolean(json_bool_val));

        json_bool_val = (dns_mode == DNS_ADD_ORIGIN_AUTO && dns_ip_ver == DNS_IP_VER_IPV4) ? TRUE : FALSE;
        json_object_object_add(dhcpv4_jso, RFPROP_NIC_USE_DNS_SERVER, json_object_new_boolean(json_bool_val));
    }
    json_object_object_add(o_rsc_jso, RFPROP_NIC_DHCPV4, dhcpv4_jso);

    return;
}


LOCAL void _get_nic_ipv6_addresses(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gint32 ret_val;
    json_object *ipv6_array_jso = NULL;
    json_object *ipv6_obj_jso = NULL;
    gchar           address[IPV6_IP_STR_SIZE + 1] = {0};
    gchar           local_address[IPV6_IP_STR_SIZE + 1] = {0};
    const gchar *ipv6_mode_str = NULL;
    json_object *com_obj_jso = NULL;
    const gchar **ip_addr = NULL;
    const guint8 *prefix = NULL;
    gsize prefix_len = 0;
    gsize i;
    guint8 ipv6_mode = 0;
    guint8 ipv6_prefix = 0;
    GVariant *ipv6_slaac_addr = NULL;
    GVariant *ipv6_slaac_prefix = NULL;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, address, sizeof(address));
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6_mode);
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_PREFIX, &ipv6_prefix);
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK, local_address,
            sizeof(local_address));
        (void)dfl_get_property_value(eth_group_obj, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR, &ipv6_slaac_addr);
        (void)dfl_get_property_value(eth_group_obj, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_PREFIX, &ipv6_slaac_prefix);
    }

    ipv6_array_jso = json_object_new_array();
    goto_if_expr(ipv6_array_jso == NULL || ipv6_mode == 0, EXIT_ADD_TO_RSC);
    ipv6_obj_jso = json_object_new_object();
    goto_if_expr(ipv6_obj_jso == NULL, EXIT_ADD_TO_RSC);
    ret_val = json_object_array_add(ipv6_array_jso, ipv6_obj_jso);
    goto_label_do_info_if_expr(ret_val != 0, EXIT_ADD_TO_RSC, json_object_put(ipv6_obj_jso));

    // 添加可手动配置的IPv6地址
    json_object_object_add(ipv6_obj_jso, RFPROP_MANAGER_ETH_IPADDR,
        strlen((const gchar *)address) ? json_object_new_string((const gchar *)address) : NULL);
    json_object_object_add(ipv6_obj_jso, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN,
        (ipv6_prefix > 0 && ipv6_prefix <= 128) ? json_object_new_int(ipv6_prefix) :
                                                  NULL); // IPv6地址128bit,前缀长度范围 1-128
    ipv6_mode_str =
        (ipv6_mode == IP_STATIC_MODE) ? IP_MODE_STATIC_STRING : ((ipv6_mode == IP_DHCP_MODE) ? IPV6_MODE_DHCP : NULL);
    json_object_object_add(ipv6_obj_jso, RFPROP_MANAGER_ETH_IPMODE,
        (ipv6_mode_str != NULL) ? json_object_new_string(ipv6_mode_str) : NULL);

    // 添加链路本地地址
    com_obj_jso = json_object_new_object();
    goto_if_expr(com_obj_jso == NULL, EXIT_ADD_TO_RSC);
    ret_val = json_object_array_add(ipv6_array_jso, com_obj_jso);
    goto_label_do_info_if_expr(ret_val != 0, EXIT_ADD_TO_RSC, json_object_put(com_obj_jso));

    json_object_object_add(com_obj_jso, RFPROP_MANAGER_ETH_IPADDR,
        strlen(local_address) ? json_object_new_string((const gchar *)local_address) : NULL);
    json_object_object_add(com_obj_jso, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN,
        json_object_new_int(DEFAULT_IPV6_PREFIX_LEN));
    json_object_object_add(com_obj_jso, RFPROP_MANAGER_ETH_IPMODE, json_object_new_string(IPV6_MODE_LINKLOCAL));

    // 添加SLAAC地址
    ip_addr = g_variant_get_strv(ipv6_slaac_addr, NULL); // 地址和前缀的数组长度是相同的
    prefix = (const guint8 *)g_variant_get_fixed_array(ipv6_slaac_prefix, &prefix_len, sizeof(guint8));
    goto_if_expr(ip_addr == NULL || prefix == NULL, EXIT_ADD_TO_RSC);

    for (i = 0; i < prefix_len; i++) {
        continue_if_expr(strlen(ip_addr[i]) == 0);
        com_obj_jso = json_object_new_object();
        continue_if_expr(com_obj_jso == NULL);

        json_object_object_add(com_obj_jso, RFPROP_MANAGER_ETH_IPADDR, json_object_new_string(ip_addr[i]));
        json_object_object_add(com_obj_jso, RFPROP_MANAGER_ETH_IPMODE, json_object_new_string(IPV6_MODE_SLAAC));
        json_object_object_add(com_obj_jso, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN,
            (prefix[i] > 0 && prefix[i] <= 128) ? json_object_new_int(prefix[i]) : NULL);

        ret_val = json_object_array_add(ipv6_array_jso, com_obj_jso);
        do_if_expr(ret_val != 0, debug_log(DLOG_ERROR,
            "%s: add com_obj_jso to ipv6_array_jso failed, i = %" G_GSIZE_FORMAT, __FUNCTION__, i);
            json_object_put(com_obj_jso));
    }

EXIT_ADD_TO_RSC:
    if (ip_addr != NULL) {
        g_free(ip_addr);
    }
    do_if_expr(ipv6_slaac_addr, g_variant_unref(ipv6_slaac_addr));
    do_if_expr(ipv6_slaac_prefix, g_variant_unref(ipv6_slaac_prefix));
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_IPV6_ADDR, ipv6_array_jso);

    return;
}


LOCAL void _get_nic_ipv6_static_addresses(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    json_object *ipv6_static_array_jso = NULL;
    json_object *ipv6_static_obj_jso = NULL;
    gint32 ret_val;
    gchar       address[IPV6_IP_STR_SIZE + 1] = {0};
    guint8 ipv6_mode = 0;
    guint8 ipv6_prefix = 0;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_ADDR, address, sizeof(address));
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6_mode);
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_PREFIX, &ipv6_prefix);
    }

    ipv6_static_array_jso = json_object_new_array();
    goto_if_expr(ipv6_static_array_jso == NULL || ipv6_mode != IP_STATIC_MODE, EXIT_ADD_TO_RSC);
    ipv6_static_obj_jso = json_object_new_object();
    goto_if_expr(ipv6_static_obj_jso == NULL, EXIT_ADD_TO_RSC);
    ret_val = json_object_array_add(ipv6_static_array_jso, ipv6_static_obj_jso);
    goto_label_do_info_if_expr(ret_val != 0, EXIT_ADD_TO_RSC, json_object_put(ipv6_static_obj_jso));

    json_object_object_add(ipv6_static_obj_jso, RFPROP_MANAGER_ETH_IPADDR,
        strlen(address) ? json_object_new_string((const gchar *)address) : NULL);
    json_object_object_add(ipv6_static_obj_jso, RFPROP_MANAGER_ETH_IPV6_PREFIXLEN,
        (ipv6_prefix > 0 && ipv6_prefix <= 128) ? json_object_new_int(ipv6_prefix) :
                                                  NULL); // IPv6地址128bit,前缀长度范围 1-128

EXIT_ADD_TO_RSC:
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_IPV6_STATIC_ADDR, ipv6_static_array_jso);

    return;
}


LOCAL void _get_nic_ipv6_gateway(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar   gateway[IPV6_GATEWAY_STR_SIZE + 1] = {0};

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, gateway, sizeof(gateway));
    }
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_IPV6_GATEWAY,
        strlen(gateway) ? json_object_new_string((const gchar *)gateway) : NULL);

    return;
}


LOCAL void _get_nic_ipv6_static_gateway(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    json_object *ipv6_gw_static_array_jso = NULL;
    json_object *ipv6_gw_static_obj_jso = NULL;
    gint32 ret_val;
    gchar       gateway[IPV6_GATEWAY_STR_SIZE + 1] = {0};
    guint8 ipv6_mode = 0;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_string(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_GATEWAY, gateway, sizeof(gateway));
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6_mode);
    }

    ipv6_gw_static_array_jso = json_object_new_array();
    goto_if_expr(ipv6_gw_static_array_jso == NULL || ipv6_mode != IP_STATIC_MODE, EXIT_ADD_TO_RSC);
    ipv6_gw_static_obj_jso = json_object_new_object();
    goto_if_expr(ipv6_gw_static_obj_jso == NULL, EXIT_ADD_TO_RSC);
    ret_val = json_object_array_add(ipv6_gw_static_array_jso, ipv6_gw_static_obj_jso);
    goto_label_do_info_if_expr(ret_val != 0, EXIT_ADD_TO_RSC, json_object_put(ipv6_gw_static_obj_jso));

    json_object_object_add(ipv6_gw_static_obj_jso, RFPROP_MANAGER_ETH_IPADDR,
        strlen((const gchar *)gateway) ? json_object_new_string((const gchar *)gateway) : NULL);

EXIT_ADD_TO_RSC:
    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_IPV6_STATIC_GATEWAYS, ipv6_gw_static_array_jso);

    return;
}


LOCAL void _get_nic_dhcpv6(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    const gchar *op_mode_str = NULL;
    json_object *dhcpv6_jso = NULL;
    json_bool json_bool_val = FALSE;
    OBJ_HANDLE obj_handle = 0;
    guint8 dns_mode = 0;
    guint8 dns_ip_ver = 0;
    guint8 ipv6_mode = 0;

    if (eth_group_obj > 0) {
        (void)dal_get_property_value_byte(eth_group_obj, ETH_GROUP_ATTRIBUTE_IPV6_MODE, &ipv6_mode);
        (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSIPVER, &dns_ip_ver);
    }

    dhcpv6_jso = json_object_new_object();
    if (dhcpv6_jso) {
        
        
        // BMC的 DHCPv6会配置IPv6地址，因此是 Stateful
        op_mode_str = (ipv6_mode == IP_DHCP_MODE) ? NIC_OPERATING_MODE_STATEFUL : NIC_OPERATING_MODE_DISABLED;
        json_object_object_add(dhcpv6_jso, RFPROP_NIC_OPERATING_MODE,
            op_mode_str != NULL ? json_object_new_string(op_mode_str) : NULL);

        json_bool_val = (dns_mode == DNS_ADD_ORIGIN_AUTO && dns_ip_ver == DNS_IP_VER_IPV6) ? TRUE : FALSE;
        json_object_object_add(dhcpv6_jso, RFPROP_NIC_USE_DNS_SERVER, json_object_new_boolean(json_bool_val));
    }
    json_object_object_add(o_rsc_jso, RFPROP_NIC_DHCPV6, dhcpv6_jso);

    return;
}


LOCAL void _get_nic_name_server(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar       main_domain[DNS_DOMAINSERVER_MAX_LEN + 1] = {0};
    gchar       backup_domain[DNS_DOMAINSERVER_MAX_LEN + 1] = {0};
    gchar       tertiary_domain[DNS_DOMAINSERVER_MAX_LEN + 1] = {0};
    json_object *name_server_arr_jso = NULL;
    json_object *main_name_server_jso = NULL;
    guint8 dns_mode = G_MAXUINT8;
    json_object *back_name_server_jso = NULL;
    json_object *tertiary_name_server_jso = NULL;
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;

    if (eth_group_obj > 0) {
        (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_BACKUPDOMAIN, backup_domain,
            sizeof(backup_domain));
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_TERTIARYDOMAIN, tertiary_domain,
            sizeof(tertiary_domain));
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_PRIMARYDOMAIN, main_domain,
            sizeof(main_domain));
    }

    name_server_arr_jso = json_object_new_array();
    if (name_server_arr_jso && dns_mode != G_MAXUINT8) {
        main_name_server_jso = strlen(main_domain) ? json_object_new_string((const gchar *)main_domain) : NULL;
        ret_val = json_object_array_add(name_server_arr_jso, main_name_server_jso);
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: add main_name_server_jso to name_server_arr_jso failed", __FUNCTION__);
            json_object_put(main_name_server_jso));

        back_name_server_jso = strlen(backup_domain) ? json_object_new_string((const gchar *)backup_domain) : NULL;
        ret_val = json_object_array_add(name_server_arr_jso, back_name_server_jso);
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: add back_name_server_jso to name_server_arr_jso failed", __FUNCTION__);
            json_object_put(back_name_server_jso));

        tertiary_name_server_jso =
            strlen(tertiary_domain) ? json_object_new_string((const gchar *)tertiary_domain) : NULL;
        ret_val = json_object_array_add(name_server_arr_jso, tertiary_name_server_jso);
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: add tertiary_name_server_jso to name_server_arr_jso failed", __FUNCTION__);
            json_object_put(tertiary_name_server_jso));
    }

    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_NAMESERVER, name_server_arr_jso);

    return;
}


LOCAL void _get_nic_static_name_server(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    gchar       main_domain[DNS_DOMAINSERVER_MAX_LEN + 1] = {0};
    gchar       backup_domain[DNS_DOMAINSERVER_MAX_LEN + 1] = {0};
    gchar       tertiary_domain[DNS_DOMAINSERVER_MAX_LEN + 1] = {0};
    json_object *static_name_server_arr = NULL;
    json_object *main_name_server_jso = NULL;
    json_object *back_name_server_jso = NULL;
    json_object *tertiary_name_server_jso = NULL;
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    guint8 dns_mode = G_MAXUINT8;

    if (eth_group_obj > 0) {
        (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_DNSSETTING_DNSMODE, &dns_mode);
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_PRIMARYDOMAIN, main_domain,
            sizeof(main_domain));
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_BACKUPDOMAIN, backup_domain,
            sizeof(backup_domain));
        (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_TERTIARYDOMAIN, tertiary_domain,
            sizeof(tertiary_domain));
    }

    static_name_server_arr = json_object_new_array();
    if (static_name_server_arr && dns_mode != G_MAXUINT8) {
        main_name_server_jso = (dns_mode == DNS_ADD_ORIGIN_MANUAL && strlen(main_domain)) ?
            json_object_new_string((const gchar *)main_domain) :
            NULL;
        ret_val = json_object_array_add(static_name_server_arr, main_name_server_jso);
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: add main_name_server_jso to name_server_arr_jso failed", __FUNCTION__);
            json_object_put(main_name_server_jso));

        back_name_server_jso = (dns_mode == DNS_ADD_ORIGIN_MANUAL && strlen(backup_domain)) ?
            json_object_new_string((const gchar *)backup_domain) :
            NULL;
        ret_val = json_object_array_add(static_name_server_arr, back_name_server_jso);
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: add back_name_server_jso to name_server_arr_jso failed", __FUNCTION__);
            json_object_put(back_name_server_jso));

        tertiary_name_server_jso = (dns_mode == DNS_ADD_ORIGIN_MANUAL && strlen(tertiary_domain)) ?
            json_object_new_string((const gchar *)tertiary_domain) :
            NULL;
        ret_val = json_object_array_add(static_name_server_arr, tertiary_name_server_jso);
        do_if_expr(ret_val != VOS_OK,
            debug_log(DLOG_ERROR, "%s: add tertiary_name_server_jso to name_server_arr_jso failed", __FUNCTION__);
            json_object_put(tertiary_name_server_jso));
    }

    json_object_object_add(o_rsc_jso, RFPROP_MANAGER_ETH_STATIC_NAMESERVER, static_name_server_arr);

    return;
}


LOCAL void _get_nic_oem(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso)
{
    json_object *oem = NULL;
    json_object *huawei = NULL;

    oem = json_object_new_object();
    if (oem) {
        huawei = json_object_new_object();
        if (huawei) {
            if (eth_group_obj == 0) {
                json_object_object_add(huawei, RFPROP_MANAGER_ETH_OEM_IPVER, NULL);
                json_object_object_add(huawei, RFPROP_MANAGER_ETH_OEM_DNS_MODE, NULL);
            } else {
                // 添加 IPVersion
                redfish_get_ip_version(eth_group_obj, &huawei);

                // 添加 DNSAddressOrigin
                redfish_get_dns_address_origin(&huawei);
            }
        }
        json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
    }
    json_object_object_add(o_rsc_jso, RFPROP_OEM, oem);

    return;
}


LOCAL void _get_nic_links(gchar *memberid, json_object *o_rsc_jso)
{
    gchar eth_url[MAX_RSC_URI_LEN] = {0};
    json_object *links = NULL;
    json_object *oem = NULL;
    json_object *huawei = NULL;
    json_object *ethernet_interface = NULL;
    json_object *odata_id = NULL;
    gint32 ret_val = 0;
    guint8 card_type = 0;
    guint8 port_num = 0;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    links = json_object_new_object(); // Links
    json_object_object_add(o_rsc_jso, RFPROP_LINKS, links);
    return_if_expr(links == NULL);
    oem = json_object_new_object(); // Oem
    json_object_object_add(links, RFPROP_OEM, oem);
    return_if_expr(oem == NULL);
    huawei = json_object_new_object(); // Huawei
    json_object_object_add(oem, RFPROP_OEM_HUAWEI, huawei);
    return_if_expr(huawei == NULL);
    ethernet_interface = json_object_new_object(); // EthernetInterface
    json_object_object_add(huawei, RF_CLASS_ETHERNETINTERFACE, ethernet_interface);
    return_if_expr(ethernet_interface == NULL);
    (void)_get_type_portnum_from_memberid(memberid, &card_type, &port_num);
    (void)redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (card_type == NET_TYPE_LOM || card_type == NET_TYPE_LOM2 || card_type == NET_TYPE_PCIE ||
        card_type == NET_TYPE_OCP_CARD || card_type == NET_TYPE_OCP2_CARD) {
        ret_val = snprintf_s(eth_url, sizeof(eth_url), sizeof(eth_url) - 1, RFPROP_SYSTEM_ETHS, slot_id);
        return_if_expr(ret_val <= 0);
        odata_id = json_object_new_string((const gchar *)eth_url);
    }
    json_object_object_add(ethernet_interface, RFPROP_ODATA_ID, odata_id);

    return;
}


LOCAL gint32 _nic_rsc_id_compare(gconstpointer a, gconstpointer b)
{
    const NIC_ID_PROP *list_data = (const NIC_ID_PROP *)a;
    const NIC_ID_PROP *insert_data = (const NIC_ID_PROP *)b;

    return (0 == strcasecmp(list_data->nic_rsc_id, insert_data->nic_rsc_id)) ? 0 : -1;
}

LOCAL gint32 _nic_type_compare(gconstpointer a, gconstpointer b)
{
    const NIC_ID_PROP *list_data = (const NIC_ID_PROP *)a;
    const NIC_ID_PROP *insert_data = (const NIC_ID_PROP *)b;

    if (list_data == NULL) {
        return -1;
    } else if (insert_data == NULL) {
        return 1;
    } else {
        return list_data->card_type - insert_data->card_type;
    }
}


LOCAL gint32 _check_nic_memberid_valid(gchar *member_id, OBJ_HANDLE *eth_group_obj)
{
    GSList *nic_id_list = NULL;
    GSList *cur_id_list = NULL;
    NIC_ID_PROP i_nic_id_prop = { 0, 0, 0, { 0 } };
    gint32 ret_val;
    gint32 ret_code = VOS_ERR;
    OBJ_HANDLE obj_handle = 0;
    guint8 net_type = 0;
    guint8 eth_num = 0;
    guint8 ncsi_port = 0;
    NIC_ID_PROP *found_nic_prop = NULL;

    return_val_if_expr(eth_group_obj == NULL, ret_code);

    ret_val = strncpy_s(i_nic_id_prop.nic_rsc_id, sizeof(i_nic_id_prop.nic_rsc_id), member_id, strlen(member_id));
    if (ret_val != EOK) {
        debug_log(DLOG_ERROR, ERROR_LOG_WITH_CODE, __FUNCTION__, "strncpy_s member_id", ret_val);
        return VOS_ERR;
    }

    _fill_nic_rsc_list(&nic_id_list);

    cur_id_list = g_slist_find_custom(nic_id_list, &i_nic_id_prop, _nic_rsc_id_compare);
    if (cur_id_list == NULL) {
        debug_log(DLOG_DEBUG, "%s: find %s in nic list failed", __FUNCTION__, member_id);
        goto EXIT_FREE_MEM;
    }
    found_nic_prop = (NIC_ID_PROP *)(cur_id_list->data);

    *eth_group_obj = 0;
    ret_code = RET_OK; // 代码走到这里，说明member id 校验已经通过了，下面代码  是判断该网口是不是当前的网口
    ret_val = dal_eth_get_out_type_object(OUTTER_GROUP_TYPE, &obj_handle);
    goto_if_expr(ret_val != DFL_OK, EXIT_FREE_MEM);

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_TYPE, &net_type);

    if (net_type == NET_TYPE_DEDICATED || net_type == NET_TYPE_AGGREGATION || (net_type == NET_TYPE_CABINET_VLAN)) {
        do_if_expr(net_type == found_nic_prop->card_type, (*eth_group_obj = obj_handle));
    } else if (net_type == NET_TYPE_LOM || net_type == NET_TYPE_LOM2 || net_type == NET_TYPE_PCIE ||
        net_type == NET_TYPE_OCP_CARD || net_type == NET_TYPE_OCP2_CARD) {
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_NCSI_PORT, &ncsi_port);
        (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, &eth_num);
        do_if_expr(net_type == found_nic_prop->card_type && ncsi_port == found_nic_prop->port_num &&
            eth_num == found_nic_prop->eth_num,
            (*eth_group_obj = obj_handle));
    }

EXIT_FREE_MEM:
    g_slist_free_full(nic_id_list, g_free);

    return ret_code;
}


gint32 get_nic_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    // 刷新 @odata.id  , Id, Name
    _get_nic_odata_prop(i_paras->member_id, o_rsc_jso);

    // 刷新 InterfaceEnabled
    _get_nic_interface_enable(i_paras->obj_handle, o_rsc_jso);

    // 刷新 PermanentMACAddress
    _get_nic_mac(i_paras->obj_handle, o_rsc_jso);

    // 刷新 HostName
    _get_nic_host_name(i_paras->obj_handle, o_rsc_jso);

    // 刷新 FQDN
    get_nic_fqdn(i_paras->obj_handle, o_rsc_jso);

    // 刷新 VLAN
    _get_nic_vlan_info(i_paras->obj_handle, o_rsc_jso);

    // 刷新 IPv4Addresses
    _get_nic_ipv4_addresses(i_paras->obj_handle, o_rsc_jso);

    // 刷新 IPv4StaticAddresses
    _get_nic_ipv4_static_addresses(i_paras->obj_handle, o_rsc_jso);

    // 刷新 DHCPv4
    _get_nic_dhcpv4(i_paras->obj_handle, o_rsc_jso);

    // 刷新 IPv6Addresses
    _get_nic_ipv6_addresses(i_paras->obj_handle, o_rsc_jso);

    // 刷新 IPv6StaticAddresses
    _get_nic_ipv6_static_addresses(i_paras->obj_handle, o_rsc_jso);

    // 刷新 IPv6DefaultGateway
    _get_nic_ipv6_gateway(i_paras->obj_handle, o_rsc_jso);

    // 刷新 IPv6StaticDefaultGateways
    _get_nic_ipv6_static_gateway(i_paras->obj_handle, o_rsc_jso);

    // 刷新 DHCPv6
    _get_nic_dhcpv6(i_paras->obj_handle, o_rsc_jso);

    // 刷新 NameServers
    _get_nic_name_server(i_paras->obj_handle, o_rsc_jso);

    // 刷新 StaticNameServers
    _get_nic_static_name_server(i_paras->obj_handle, o_rsc_jso);

    // 刷新 Oem
    _get_nic_oem(i_paras->obj_handle, o_rsc_jso);

    // 刷新 Links
    _get_nic_links(i_paras->member_id, o_rsc_jso);

    return RF_OK;
}


gint32 manager_nic_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret_val;

    ret_val = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_if_expr(ret_val != TRUE, HTTP_NOT_FOUND);

    ret_val = _check_nic_memberid_valid(i_paras->member_id, &(i_paras->obj_handle));
    return_val_if_expr(ret_val != VOS_OK, HTTP_NOT_FOUND);

    *prop_provider = g_manager_nic_provider;
    *count = G_N_ELEMENTS(g_manager_nic_provider);

    return VOS_OK;
}
