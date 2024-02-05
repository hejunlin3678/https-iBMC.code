
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 __get_system_infinibands_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infinibands_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infinibands_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infinibands_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_link_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_ipv4(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_ipv6(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_ipv6_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_interface_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_infiniband_related_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 __get_system_ib_prop_string(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_infinibands_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infinibands_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infinibands_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infinibands_odata_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infinibands_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 __get_system_infinibands_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_IBS, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 __get_system_infinibands_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = rf_get_rsc_odata_prop(IBS_ODATA_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 __get_system_infinibands_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    gint32 ib_rsc_count = 0;
    OBJ_HANDLE obj_handle = 0;
    guint32 net_func = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (DFL_OK != ret || NULL == obj_list) {
        goto exit;
    }

    for (node = obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &net_func);
        continue_if_fail(NETDEV_FUNCTYPE_IB & net_func);
        ib_rsc_count++;
    }

exit:
    *o_result_jso = json_object_new_int(ib_rsc_count);
    do_if_fail(NULL == obj_list, g_slist_free(obj_list));

    return HTTP_OK;
}


LOCAL gint32 __get_ib_rsc_memberid(OBJ_HANDLE obj_handle, gchar *member_id, guint32 member_id_len)
{
    int iRet = -1;
    errno_t safe_fun_ret = EOK;
    guint8 port_type = 0;
    gchar os_eth_name[PROP_VAL_LENGTH] = {0};
    gint32 ret = 0;
    gchar card_location[PROP_VAL_LENGTH] = {0};
    gchar device_name[PROP_VAL_LENGTH] = {0};
    guint8 slik_num = 0;
    OBJ_HANDLE netcard_obj = 0;
    OBJ_HANDLE component_obj = 0;

    if (0 == obj_handle || NULL == member_id || member_id_len < 1) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return VOS_ERR;
    }

    (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
    if (ETH_PORT_TYPE_VIRTUAL == port_type) {
        (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_OSETH_NAME, os_eth_name,
            sizeof(os_eth_name));
        safe_fun_ret = strncpy_s(member_id, member_id_len, os_eth_name, member_id_len - 1);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    } else {
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &slik_num);
        ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_obj);
        return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: get %s ref netcard fail", __FUNCTION__, dfl_get_object_name(obj_handle)));

        ret = dfl_get_referenced_object(netcard_obj, PROPERTY_NETCARD_REF_COMPONENT, &component_obj);
        return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: get %s ref component fail", __FUNCTION__, dfl_get_object_name(netcard_obj)));

        (void)dal_get_property_value_string(component_obj, PROPERTY_COMPONENT_LOCATION, card_location,
            sizeof(card_location));
        (void)dal_get_property_value_string(component_obj, PROPERTY_COMPONENT_DEVICE_NAME, device_name,
            sizeof(device_name));
        (void)dal_clear_string_blank(card_location, sizeof(card_location));
        (void)dal_clear_string_blank(device_name, sizeof(device_name));

        iRet =
            snprintf_s(member_id, member_id_len, member_id_len - 1, "%s%sPort%u", card_location, device_name, slik_num);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    }

    return VOS_OK;
}


LOCAL gint32 __get_system_infinibands_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint32 net_func = 0;
    gchar member_url[MAX_URL_LEN] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_array();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: got json_object_new_array failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (DFL_OK != ret || NULL == obj_list) {
        goto exit;
    }

    json_object *member_obj_jso = NULL;

    for (node = obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &net_func);
        continue_if_fail(NETDEV_FUNCTYPE_IB & net_func);

        // 获取IB网口的member id
        ret = __get_ib_rsc_memberid(obj_handle, member_id, sizeof(member_id));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: __get_ib_rsc_memberid failed.", __FUNCTION__));

        ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_IB, member_id, member_url, sizeof(member_url));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed.", __FUNCTION__));

        // 将URL添加进members数组
        member_obj_jso = json_object_new_object();
        continue_do_info_if_expr(NULL == member_obj_jso,
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__));
        json_object_object_add(member_obj_jso, RFPROP_ODATA_ID, json_object_new_string(member_url));
        ret = json_object_array_add(*o_result_jso, member_obj_jso);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: json_object_array_add failed.", __FUNCTION__);
            json_object_put(member_obj_jso));

        obj_handle = 0;
        (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));
        (void)memset_s(member_url, sizeof(member_url), 0, sizeof(member_url));
        member_obj_jso = NULL;
    }

exit:
    do_if_fail(NULL == obj_list, g_slist_free(obj_list));

    return HTTP_OK;
}


gint32 system_infinibands_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar x86_enable = 0;

    ret = redfish_get_x86_enable_type(&x86_enable);
    if (ret != VOS_OK || x86_enable == DISABLE) {
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (TRUE != ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_infinibands_provider;
    *count = G_N_ELEMENTS(g_system_infinibands_provider);

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_system_infiniband_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, BUSY_ETH_ATTRIBUTE_OSETH_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, BUSY_ETH_ATTRIBUTE_OSETH_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_LINK_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_link_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_IPV4_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_ipv4, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_IPV6_ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_ipv6, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ETH_IPV6_GATEWAY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  __get_system_infiniband_ipv6_gateway, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_UUID, MAP_PROPERTY_NULL, BUSY_ETH_ATTRIBUTE_UUID, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_uuid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_INTERFACE_TYPE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_interface_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ETH_RELATED_PORT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, __get_system_infiniband_related_port, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 __get_system_infiniband_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = __get_ib_rsc_memberid(i_paras->obj_handle, member_id, sizeof(member_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get __get_ib_rsc_memberid failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_IB, member_id, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 __get_system_infiniband_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = rf_get_rsc_odata_prop(IB_ODATA_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 __get_system_infiniband_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return __get_system_ib_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return __get_system_ib_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_link_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_system_ethernetinterface_link_status(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_ipv4(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_system_ethernetinterface_ipv4s(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_ipv6(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_system_ethernetinterface_ipv6s(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_ipv6_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_system_ethernetinterface_ipv6_gateway(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return __get_system_ib_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 __get_system_infiniband_interface_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar *interface_type = NULL;
    guint8 port_type = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);

    // 0xff认为是没bma时物理网口的值
    if ((ETH_PORT_TYPE_PHYSICAL == port_type) || (0xff == port_type)) {
        interface_type = RF_PROPERTY_ETH_PORT_TYPE_PHYSICAL;
    } else if (ETH_PORT_TYPE_VIRTUAL == port_type) {
        interface_type = RF_PROPERTY_ETH_PORT_TYPE_VIRTUAL;
    } else {
        debug_log(DLOG_DEBUG, "%s : invalid port type is %d", __FUNCTION__, port_type);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    *o_result_jso = json_object_new_string(interface_type);

    return HTTP_OK;
}


LOCAL gint32 __get_system_infiniband_related_port(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    guint8 port_type = 0;
    OBJ_HANDLE net_card_com = 0;
    OBJ_HANDLE net_card_obj = 0;
    guint8 silk_num = 0;
    gchar location_str[PROP_VAL_LENGTH] = {0};
    gchar device_name_str[PROP_VAL_LENGTH] = {0};
    gchar net_card_memid[PROP_VAL_LENGTH] = {0};
    gchar slot_id[PROP_VAL_LENGTH] = {0};
    gchar url[MAX_URL_LEN] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
    if (ETH_PORT_TYPE_VIRTUAL == port_type) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_referenced_object(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &net_card_obj);
    if (DFL_OK != ret) {
        debug_log(DLOG_DEBUG, "%s:dfl_get_referenced_object failed, prop = %s", __FUNCTION__,
            BUSY_ETH_ATTRIBUTE_REF_NETCARD);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_get_referenced_object(net_card_obj, PROPERTY_NETCARD_REF_COMPONENT, &net_card_com);
    if (DFL_OK != ret) {
        debug_log(DLOG_DEBUG, "%s:dfl_get_referenced_object failed, prop = %s", __FUNCTION__,
            PROPERTY_NETCARD_REF_COMPONENT);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)redfish_get_board_slot(slot_id, sizeof(slot_id));
    (void)dal_get_property_value_string(net_card_com, PROPERTY_COMPONENT_LOCATION, location_str, sizeof(location_str));
    (void)dal_get_property_value_string(net_card_com, PROPERTY_COMPONENT_DEVICE_NAME, device_name_str,
        sizeof(device_name_str));
    (void)dal_clear_string_blank(location_str, sizeof(location_str));
    (void)dal_clear_string_blank(device_name_str, sizeof(device_name_str));
    iRet = snprintf_s(net_card_memid, sizeof(net_card_memid), sizeof(net_card_memid) - 1, "%s%s", location_str,
        device_name_str);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    (void)dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silk_num);

    iRet = snprintf_s(url, sizeof(url), sizeof(url) - 1, CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID, slot_id,
        net_card_memid, silk_num);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    json_object_object_add(*o_result_jso, ODATA_ID, json_object_new_string((const char *)url));

    return HTTP_OK;
}


LOCAL gint32 __get_system_ib_prop_string(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar pme_prop_val[PROP_VAL_LENGTH] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, g_system_infiniband_provider[i_paras->index].pme_prop_name,
        pme_prop_val, sizeof(pme_prop_val));

    *o_result_jso = json_object_new_string(pme_prop_val);

    return HTTP_OK;
}


LOCAL gint32 __redfish_check_ib_uri_valid(const gchar *bond_url, const gchar *url_template, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint32 net_func = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar member_url[MAX_URL_LEN] = {0};
    gchar member_id[PROP_VAL_LENGTH] = {0};
    gint32 result_val = VOS_ERR;
    if (NULL == url_template || NULL == bond_url) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return result_val;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_obj_list);
    if (DFL_OK != ret || NULL == business_obj_list) {
        // 网口不存在属于正常情况，日志打印 INFO 级别
        debug_log(DLOG_INFO, "%s: get %s object list failed.", __FUNCTION__, ETH_CLASS_NAME_BUSY_ETH);
        goto exit;
    }

    for (node = business_obj_list; node; node = g_slist_next(node)) {
        // 非IB网口跳过
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &net_func);
        continue_if_fail(NETDEV_FUNCTYPE_IB & net_func);

        ret = __get_ib_rsc_memberid(obj_handle, member_id, sizeof(member_id));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: __get_ib_rsc_memberid failed.", __FUNCTION__));

        ret = rf_get_rsc_odata_prop(url_template, member_id, member_url, sizeof(member_url));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed.", __FUNCTION__));

        // 和入参url相同，返回值置为VOS_OK
        if (0 == g_ascii_strcasecmp(bond_url, member_url)) {
            do_if_fail(NULL == o_obj_handle, (*o_obj_handle = obj_handle));
            result_val = VOS_OK;
            break;
        }

        net_func = 0;
        (void)memset_s(member_id, sizeof(member_id), 0, sizeof(member_id));
        (void)memset_s(member_url, sizeof(member_url), 0, sizeof(member_url));
    }
exit:
    do_if_fail(VOS_OK == result_val, debug_log(DLOG_ERROR, "%s: bond url is invalid.", __FUNCTION__));
    do_if_fail(NULL == business_obj_list, g_slist_free(business_obj_list));
    return result_val;
}


gint32 system_infiniband_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *url_format = URI_FORMAT_SYS_IB;

    ret = __redfish_check_ib_uri_valid(i_paras->uri, url_format, &obj_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: redfish_check_bond_uri_valid failed,url is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }
    
    i_paras->obj_handle = obj_handle;

    *prop_provider = g_system_infiniband_provider;
    *count = G_N_ELEMENTS(g_system_infiniband_provider);

    return VOS_OK;
}
