
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_bridge_collection_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bridge_collection_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bridge_collection_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bridge_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_bridge_collection_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_collection_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_collection_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_collection_odata_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_collection_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_bridge_collection_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BRIDGE_COLLECTION, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bridge_collection_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = rf_get_rsc_odata_prop(BRIDGE_COLLECT_ODATA_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bridge_collection_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint16 bond_count = 0;
    guint8 card_type = 0;
    OBJ_HANDLE obj_handle = 0;

    return_val_do_info_if_expr(NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_expr(TRUE != i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_obj_list);
    if (DFL_OK != ret || NULL == business_obj_list) {
        // 网口不存在属于正常情况，日志打印 INFO 级别
        debug_log(DLOG_INFO, "%s: get %s object list failed.", __FUNCTION__, ETH_CLASS_NAME_BUSY_ETH);
        goto exit;
    }

    for (node = business_obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        continue_if_fail(BUSINESSPORT_CARD_TYPE_BRIDGE == card_type);
        bond_count++;
    }

exit:
    do_if_fail(NULL == business_obj_list, g_slist_free(business_obj_list));
    *o_result_jso = json_object_new_int(bond_count);

    return HTTP_OK;
}


LOCAL gint32 get_system_bridge_collection_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 retVal;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint8 card_type = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar member_url[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    json_object *member_obj_jso = NULL;

    return_val_do_info_if_expr(NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    return_val_do_info_if_expr(TRUE != i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();

    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed.", __FUNCTION__));

    retVal = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_obj_list);
    if (DFL_OK != retVal || NULL == business_obj_list) {
        // 网口不存在属于正常情况，日志打印 INFO 级别
        debug_log(DLOG_INFO, "%s: get %s object list failed.", __FUNCTION__, ETH_CLASS_NAME_BUSY_ETH);
        goto exit;
    }

    for (node = business_obj_list; node; node = g_slist_next(node)) {
        // 非bond网口跳过
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        continue_if_fail(BUSINESSPORT_CARD_TYPE_BRIDGE == card_type);

        // 获取bond网口的URL
        (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));
        retVal = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BRIDGE, eth_bdf, member_url, sizeof(member_url));
        continue_do_info_if_fail(VOS_OK == retVal,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed.", __FUNCTION__));

        // 将URL添加进members数组
        member_obj_jso = json_object_new_object();
        continue_do_info_if_expr(NULL == member_obj_jso,
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__));
        json_object_object_add(member_obj_jso, RFPROP_ODATA_ID, json_object_new_string(member_url));
        retVal = json_object_array_add(*o_result_jso, member_obj_jso);
        continue_do_info_if_fail(VOS_OK == retVal,
            debug_log(DLOG_ERROR, "%s: json_object_array_add failed.", __FUNCTION__);
            json_object_put(member_obj_jso));

        card_type = 0;
        (void)memset_s(eth_bdf, sizeof(eth_bdf), 0, sizeof(eth_bdf));
        (void)memset_s(member_url, sizeof(member_url), 0, sizeof(member_url));
    }

exit:
    do_if_fail(NULL == business_obj_list, g_slist_free(business_obj_list));
    return HTTP_OK;
}


gint32 system_network_bridge_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guint8 enable_type = 0;

    ret = redfish_get_x86_enable_type(&enable_type);
    if (VOS_OK != ret || DISABLE == enable_type) {
        return HTTP_NOT_FOUND;
    }

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (TRUE != ret) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_bridge_collection_provider;
    *count = G_N_ELEMENTS(g_system_bridge_collection_provider);

    return VOS_OK;
}

LOCAL gint32 get_system_bridge_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bridge_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bridge_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bridge_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bridge_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_bridge_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ID, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_BDF, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_OSETH_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LINKS, ETH_CLASS_NAME_BUSY_ETH, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bridge_links, NULL, NULL, ETAG_FLAG_ENABLE},
};


LOCAL gint32 get_system_bridge_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BRIDGE, eth_bdf, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bridge_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = rf_get_rsc_odata_prop(BRIDGE_ODATA_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string(odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bridge_prop_string(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    (void)dal_get_property_value_string(i_paras->obj_handle, g_system_bridge_provider[i_paras->index].pme_prop_name,
        pme_prop_val, sizeof(pme_prop_val));

    
    return_val_if_expr(VOS_OK != rf_string_check(pme_prop_val), HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(pme_prop_val);

    return HTTP_OK;
}


LOCAL gint32 get_system_bridge_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_system_bridge_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_bridge_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_system_bridge_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL void get_bridge_links_bonded_interfaces(OBJ_HANDLE obj_handle, json_object *links_obj)
{
    gchar contained_ports[MAX_URL_LEN] = {0};
    gchar **port_array = NULL;
    guint32 port_count = 0;
    guint32 index_val = 0;
    gint32 retVal = 0;
    OBJ_HANDLE physical_obj = 0;
    gchar memberid[PROP_VAL_LENGTH] = {0};
    gchar physical_url[MAX_URL_LEN] = {0};
    json_object *bonded_interfaces = NULL;
    json_object *object_jso = NULL;

    return_do_info_if_expr(NULL == links_obj || 0 == obj_handle,
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    bonded_interfaces = json_object_new_array();
    if (NULL == bonded_interfaces) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
        goto exit;
    }

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_CONTAINED_PORTS, contained_ports,
        sizeof(contained_ports));
    goto_label_do_info_if_expr(0 == g_strcmp0(contained_ports, PROPERTY_VALUE_DEFAULT_STRING), exit,
        debug_log(DLOG_DEBUG, "%s: not bonded interface", __FUNCTION__));

    // 属性示例  enp2s0f2;enp2s0f3 ， 无效时为N/A
    port_array = g_strsplit(contained_ports, ";", 0);
    if (NULL == port_array) {
        debug_log(DLOG_DEBUG, "%s: invalid BondedInterfaces with %s", __FUNCTION__, contained_ports);
        goto exit;
    }

    port_count = g_strv_length(port_array);

    for (index_val = 0; index_val < port_count; index_val++) {
        // 获取物理网口的对象句柄，拼接物理网口的URL
        retVal = dal_get_specific_object_string(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_OSETH_NAME,
            port_array[index_val], &physical_obj);
        continue_do_info_if_fail(DFL_OK == retVal, debug_log(DLOG_DEBUG,
            "%s: get businessPort object handle failed, os eth name is %s", __FUNCTION__, port_array[index_val]));
        retVal = redfish_get_sys_eth_memberid(physical_obj, memberid, sizeof(memberid));
        continue_do_info_if_fail(VOS_OK == retVal,
            debug_log(DLOG_ERROR, "%s: redfish_get_sys_eth_memberid failed, index = %d", __FUNCTION__, index_val));
        retVal = rf_get_rsc_odata_prop(URI_FORMAT_SYS_ETH, memberid, physical_url, sizeof(physical_url));
        continue_do_info_if_fail(VOS_OK == retVal,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed, index = %d", __FUNCTION__, index_val));

        // add进数组中
        object_jso = json_object_new_object();
        continue_do_info_if_expr(NULL == object_jso,
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed, index = %d", __FUNCTION__, index_val));
        json_object_object_add(object_jso, ODATA_ID, json_object_new_string(physical_url));
        retVal = json_object_array_add(bonded_interfaces, object_jso);
        continue_do_info_if_fail(VOS_OK == retVal,
            debug_log(DLOG_ERROR, "%s: json_object_array_add failed, index = %d", __FUNCTION__, index_val);
            json_object_put(object_jso));
    }

exit:
    do_if_fail(NULL == port_array, g_strfreev(port_array));
    json_object_object_add(links_obj, RFPROP_BRIDGE_INTERFACES, bonded_interfaces);
}


LOCAL void get_bridge_links_related_interfaces(OBJ_HANDLE obj_handle, json_object *links_obj)
{
    gchar memberid[PROP_VAL_LENGTH] = {0};
    gchar bond_url_eth[MAX_URL_LEN] = {0};
    json_object *related_interfaces = NULL;
    gint32 retVal;
    if (NULL == links_obj || 0 == obj_handle) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return;
    }

    related_interfaces = json_object_new_object();
    if (NULL == related_interfaces) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        goto exit;
    }

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, memberid, sizeof(memberid));
    retVal = rf_get_rsc_odata_prop(URI_FORMAT_SYS_ETH, memberid, bond_url_eth, sizeof(bond_url_eth));
    if (VOS_OK != retVal) {
        debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed", __FUNCTION__);
        goto exit;
    }

    json_object_object_add(related_interfaces, ODATA_ID, json_object_new_string(bond_url_eth));
exit:
    json_object_object_add(links_obj, RFPROP_RELATED_INTERFACE, related_interfaces);
}


LOCAL gint32 get_system_bridge_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return_val_do_info_if_expr(NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();

    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_bridge_links_bonded_interfaces(i_paras->obj_handle, *o_result_jso);
    get_bridge_links_related_interfaces(i_paras->obj_handle, *o_result_jso);

    return HTTP_OK;
}

LOCAL gint32 redfish_check_bridge_uri_valid(const gchar *bridge_url, const gchar *url_template,
    OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint8 card_type = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar member_url[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    gint32 result_val = VOS_ERR;

    if (NULL == bridge_url || NULL == url_template) {
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
        // 非bond网口跳过
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        continue_if_fail(BUSINESSPORT_CARD_TYPE_BRIDGE == card_type);

        // 获取bond网口的URL
        (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));
        ret = rf_get_rsc_odata_prop(url_template, eth_bdf, member_url, sizeof(member_url));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed.", __FUNCTION__));

        // 和入参url相同，返回值置为VOS_OK
        if (0 == g_ascii_strcasecmp(bridge_url, member_url)) {
            do_if_fail(NULL == o_obj_handle, (*o_obj_handle = obj_handle));
            result_val = VOS_OK;
            break;
        }

        card_type = 0;
        (void)memset_s(eth_bdf, sizeof(eth_bdf), 0, sizeof(eth_bdf));
        (void)memset_s(member_url, sizeof(member_url), 0, sizeof(member_url));
    }

exit:
    do_if_fail(VOS_OK == result_val, debug_log(DLOG_ERROR, "%s: url is invalid.", __FUNCTION__));
    do_if_fail(NULL == business_obj_list, g_slist_free(business_obj_list));
    return result_val;
}


gint32 system_network_bridge_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *url_format = NULL;

    url_format = URI_FORMAT_SYS_BRIDGE;

    ret = redfish_check_bridge_uri_valid(i_paras->uri, url_format, &obj_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: redfish_check_bond_uri_valid failed,url is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    
    i_paras->obj_handle = obj_handle;

    *prop_provider = g_system_bridge_provider;
    *count = G_N_ELEMENTS(g_system_bridge_provider);

    return VOS_OK;
}
