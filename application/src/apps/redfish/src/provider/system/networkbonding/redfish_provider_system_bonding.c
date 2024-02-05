
#include "redfish_provider.h"
#include "redfish_provider_system.h"

LOCAL gint32 get_system_bondings_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bondings_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bondings_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bondings_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bonding_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_workmode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_link_monitor_frequency(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bonding_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_bonding_configure(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bondings_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_bondings_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bondings_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bondings_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS_COUNT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bondings_odata_count, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MEMBERS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bondings_members, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_bondings_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BONDINGS, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bondings_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = rf_get_rsc_odata_prop(BONDINGS_ODATA_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bondings_odata_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint16 bond_count = 0;
    guint8 card_type = 0;
    OBJ_HANDLE obj_handle = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &business_obj_list);
    if (DFL_OK != ret || NULL == business_obj_list) {
        // 网口不存在属于正常情况，日志打印 INFO 级别
        debug_log(DLOG_INFO, "%s: get %s object list failed.", __FUNCTION__, ETH_CLASS_NAME_BUSY_ETH);
        goto exit;
    }

    for (node = business_obj_list; node; node = g_slist_next(node)) {
        obj_handle = (OBJ_HANDLE)(node->data);
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
        continue_if_fail(BUSINESSPORT_CARD_TYPE_TEAM == card_type);
        bond_count++;
    }

exit:
    do_if_fail(NULL == business_obj_list, g_slist_free(business_obj_list));
    *o_result_jso = json_object_new_int(bond_count);

    return HTTP_OK;
}


LOCAL gint32 get_system_bondings_members(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint8 card_type = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar member_url[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    json_object *member_obj_jso = NULL;
    if (NULL == o_message_jso || NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_array();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: failed to get json_object_new_array.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
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
        continue_if_fail(BUSINESSPORT_CARD_TYPE_TEAM == card_type);

        // 获取bond网口的URL
        (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));
        ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BONDING, eth_bdf, member_url, sizeof(member_url));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed.", __FUNCTION__));

        // 将URL添加进members数组
        member_obj_jso = json_object_new_object();
        continue_do_info_if_expr(NULL == member_obj_jso,
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed.", __FUNCTION__));
        json_object_object_add(member_obj_jso, RFPROP_ODATA_ID, json_object_new_string((const gchar *)member_url));
        ret = json_object_array_add(*o_result_jso, member_obj_jso);
        continue_do_info_if_fail(VOS_OK == ret,
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


gint32 system_networkbondings_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
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

    *prop_provider = g_system_bondings_provider;
    *count = G_N_ELEMENTS(g_system_bondings_provider);

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_system_bonding_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ID, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_BDF, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_OSETH_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BONDING_TYPE, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_WORK_MODE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_type, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_BONDING_BRIDGE_WORKMODE, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_WORK_MODE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_workmode, NULL, NULL, ETAG_FLAG_ENABLE},
    {RF_PROPERTY_LINK_MONITOR_FREQUENCY, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_LINK_MNTR_FRE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_link_monitor_frequency, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_LINKS, ETH_CLASS_NAME_BUSY_ETH, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_links, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bonding_action, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFACTION_BONDING_CONFIGURE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, NULL, NULL, act_bonding_configure, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_bonding_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    if (NULL == o_message_jso || NULL == o_result_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BONDING, eth_bdf, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bonding_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    ret = rf_get_rsc_odata_prop(BONDING_ODATA_CONTEXT, NULL, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


LOCAL gint32 get_system_bonding_prop_string(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    (void)dal_get_property_value_string(i_paras->obj_handle, g_system_bonding_provider[i_paras->index].pme_prop_name,
        pme_prop_val, sizeof(pme_prop_val));

    
    
    return_val_if_expr(VOS_OK != rf_string_check((const gchar *)pme_prop_val), HTTP_INTERNAL_SERVER_ERROR);
    

    *o_result_jso = json_object_new_string((const gchar *)pme_prop_val);

    return HTTP_OK;
}


LOCAL gint32 get_system_bonding_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_system_bonding_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_bonding_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return get_system_bonding_prop_string(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_bonding_workmode(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_system_bonding_prop_string(i_paras, o_message_jso, o_result_jso);
}

LOCAL gint32 get_system_bonding_type(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gchar work_mode[PROP_VAL_LENGTH] = {0};
    gchar *flag_pointer = NULL;
    gint32 bond_type = 0;
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, g_system_bonding_provider[i_paras->index].pme_prop_name,
        work_mode, sizeof(work_mode));

    // 属性示例 "active-backup 1",redfish只取最后的数字
    flag_pointer = g_strrstr(work_mode, " ");
    if (NULL == flag_pointer) {
        debug_log(DLOG_DEBUG, "%s: invalid bond type is %s", __FUNCTION__, work_mode);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = vos_str2int(flag_pointer + 1, 10, &bond_type, NUM_TPYE_INT32);
    if (VOS_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: %s is not number", __FUNCTION__, flag_pointer + 1);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_int(bond_type);

    return HTTP_OK;
}


LOCAL gint32 get_system_bonding_link_monitor_frequency(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 link_monitor_frequency = 0;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_uint32(i_paras->obj_handle, g_system_bonding_provider[i_paras->index].pme_prop_name,
        &link_monitor_frequency);

    *o_result_jso = json_object_new_int((gint32)link_monitor_frequency);

    return HTTP_OK;
}


LOCAL void get_bond_links_bonded_interfaces(OBJ_HANDLE obj_handle, json_object *links_obj)
{
    json_object *bonded_interfaces = NULL;
    json_object *object_jso = NULL;
    gchar contained_ports[MAX_URL_LEN] = {0};
    gchar **port_array = NULL;
    guint32 port_count = 0;
    guint32 index_val = 0;
    gint32 ret = 0;
    OBJ_HANDLE physical_obj = 0;
    gchar memberid[PROP_VAL_LENGTH] = {0};
    gchar physical_url[MAX_URL_LEN] = {0};

    if (NULL == links_obj || 0 == obj_handle) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return;
    }

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
        debug_log(DLOG_DEBUG, "%s: invalid BondedInterfaces is %s", __FUNCTION__, contained_ports);
        goto exit;
    }

    port_count = g_strv_length(port_array);
    for (index_val = 0; index_val < port_count; index_val++) {
        // 获取物理网口的对象句柄，拼接物理网口的URL
        ret = dal_get_specific_object_string(ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_ATTRIBUTE_OSETH_NAME,
            port_array[index_val], &physical_obj);
        continue_do_info_if_fail(DFL_OK == ret, debug_log(DLOG_ERROR,
            "%s: get businessPort object handle failed, os eth name is %s", __FUNCTION__, port_array[index_val]));
        ret = redfish_get_sys_eth_memberid(physical_obj, memberid, sizeof(memberid));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: redfish_get_sys_eth_memberid failed, index = %d", __FUNCTION__, index_val));
        ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_ETH, memberid, physical_url, sizeof(physical_url));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed, index = %d", __FUNCTION__, index_val));

        // add进数组中
        object_jso = json_object_new_object();
        continue_do_info_if_expr(NULL == object_jso,
            debug_log(DLOG_ERROR, "%s: json_object_new_object failed, index = %d", __FUNCTION__, index_val));
        json_object_object_add(object_jso, ODATA_ID, json_object_new_string((const gchar *)physical_url));
        ret = json_object_array_add(bonded_interfaces, object_jso);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: json_object_array_add failed, index = %d", __FUNCTION__, index_val);
            json_object_put(object_jso));
    }
exit:
    do_if_fail(NULL == port_array, g_strfreev(port_array));
    json_object_object_add(links_obj, RFPROP_BONDED_INTERFACES, bonded_interfaces);
}


LOCAL void get_bond_links_related_interfaces(OBJ_HANDLE obj_handle, json_object *links_obj)
{
    gint32 ret;
    gchar memberid[PROP_VAL_LENGTH] = {0};
    gchar bond_url_eth[MAX_URL_LEN] = {0};
    json_object *related_interfaces = NULL;

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
    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_ETH, memberid, bond_url_eth, sizeof(bond_url_eth));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed", __FUNCTION__);
        goto exit;
    }

    json_object_object_add(related_interfaces, ODATA_ID, json_object_new_string((const gchar *)bond_url_eth));
exit:
    json_object_object_add(links_obj, RFPROP_RELATED_INTERFACE, related_interfaces);
}


LOCAL gint32 get_system_bonding_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    *o_result_jso = json_object_new_object();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    get_bond_links_bonded_interfaces(i_paras->obj_handle, *o_result_jso);
    get_bond_links_related_interfaces(i_paras->obj_handle, *o_result_jso);

    return HTTP_OK;
}


LOCAL gint32 get_system_bonding_action(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_id[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)dal_get_property_value_string(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));
    (void)rf_get_rsc_odata_prop(URI_FORMAT_SYS_BONDING, eth_bdf, odata_id, sizeof(odata_id));

    *o_result_jso = json_object_new_object();
    if (NULL == *o_result_jso) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    rf_add_action_prop(*o_result_jso, (const gchar *)odata_id, RFACTION_BONDING_CONFIGURE);

    return HTTP_OK;
}


LOCAL gint32 act_bonding_configure(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gint32 result_val = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    TASK_MONITOR_INFO_S *act_bond_config_info = NULL;
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    json_object *link_monitor = NULL;
    gchar team_bdf[PROP_VAL_LENGTH] = {0};

    // URL示例:/redfish/v1/Systems/1/NetworkBondings/team00/Actions/NetworkBonding.Configure
    if (NULL == i_paras || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)json_object_object_get_ex(i_paras->val_jso, RF_PROPERTY_LINK_MONITOR_FREQUENCY, &link_monitor);
    if (json_type_int != json_object_get_type(link_monitor)) {
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso, dal_json_object_get_str(link_monitor),
            RF_PROPERTY_LINK_MONITOR_FREQUENCY, RFACTION_BONDING_CONFIGURE);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)json_object_get_int(link_monitor)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_SET_BOND_MNTR_FRE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    switch (ret) {
        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            result_val = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            goto error_exit;

        
        case VOS_ERR_NOTSUPPORT:
            result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, o_message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        default:
            debug_log(DLOG_DEBUG, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                BUSY_ETH_METHOD_CREATE_VLAN, ret);
            result_val = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_BDF, team_bdf, sizeof(team_bdf));
    iRet = snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "Set %s link monitor frequency",
        team_bdf);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    act_bond_config_info = task_info_new_function(i_paras->obj_handle, free_state_flag, BUSY_ETH_ATTRIBUTE_CONFIG_STATE,
        BUSY_ETH_ATTRIBUTE_CONFIG_RESULT, oration_name, i_paras->uri, HTTP_ACTION);
    if (NULL == act_bond_config_info) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    act_bond_config_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, act_bond_config_info, i_paras->val_jso,
        i_paras->uri, o_result_jso);
    if (RF_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(act_bond_config_info);
        result_val = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result_val = HTTP_ACCEPTED;
error_exit:

    if (HTTP_INTERNAL_SERVER_ERROR == result_val && NULL != o_message_jso) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    }

    return result_val;
}


LOCAL gint32 redfish_check_bond_uri_valid(const gchar *bond_url, const gchar *url_template, OBJ_HANDLE *o_obj_handle)
{
    gint32 ret;
    GSList *business_obj_list = NULL;
    GSList *node = NULL;
    guint8 card_type = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar member_url[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    gint32 result_val = VOS_ERR;

    if (NULL == bond_url || NULL == url_template) {
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
        continue_if_fail(BUSINESSPORT_CARD_TYPE_TEAM == card_type);

        // 获取bond网口的URL
        (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));
        ret = rf_get_rsc_odata_prop(url_template, eth_bdf, member_url, sizeof(member_url));
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: rf_get_rsc_odata_prop failed.", __FUNCTION__));

        // 和入参url相同，返回值置为VOS_OK
        if (0 == g_ascii_strcasecmp(bond_url, member_url)) {
            do_if_fail(NULL == o_obj_handle, (*o_obj_handle = obj_handle));
            result_val = VOS_OK;
            break;
        }

        card_type = 0;
        (void)memset_s(eth_bdf, sizeof(eth_bdf), 0, sizeof(eth_bdf));
        (void)memset_s(member_url, sizeof(member_url), 0, sizeof(member_url));
    }
exit:
    do_if_fail(VOS_OK == result_val, debug_log(DLOG_ERROR, "%s: bond url is invalid.", __FUNCTION__));
    do_if_fail(NULL == business_obj_list, g_slist_free(business_obj_list));
    return result_val;
}


gint32 system_networkbonding_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *url_format = NULL;
    const gchar *action_flag = NULL;

    action_flag = g_str_case_rstr((const gchar *)i_paras->uri, ACTION_SEGMENT);
    if (action_flag) {
        url_format = URI_FORMAT_SYS_BONDING_ACTION_URL;
    } else {
        url_format = URI_FORMAT_SYS_BONDING;
    }

    ret = redfish_check_bond_uri_valid(i_paras->uri, url_format, &obj_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: redfish_check_bond_uri_valid failed,url is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }
    
    i_paras->obj_handle = obj_handle;

    *prop_provider = g_system_bonding_provider;
    *count = G_N_ELEMENTS(g_system_bonding_provider);

    return VOS_OK;
}

LOCAL PROPERTY_PROVIDER_S g_system_bonding_acinfo_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bondings_acinfo_odata_id, NULL, NULL, ETAG_FLAG_ENABLE}
};


LOCAL gint32 get_system_bondings_acinfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gchar odata_id[MAX_URL_LEN] = {0};
    gchar eth_bdf[PROP_VAL_LENGTH] = {0};
    gint32 ret;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (TRUE != i_paras->is_satisfy_privi) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    (void)dal_get_property_value_string(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_BDF, eth_bdf, sizeof(eth_bdf));

    ret = rf_get_rsc_odata_prop(URI_FORMAT_SYS_BONDING_ACTION_INFO, eth_bdf, odata_id, sizeof(odata_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get odata_id prop failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar *)odata_id);

    return HTTP_OK;
}


gint32 system_networkbonding_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    ret = redfish_check_bond_uri_valid(i_paras->uri, URI_FORMAT_SYS_BONDING_ACTION_INFO, &obj_handle);
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: redfish_check_bond_uri_valid failed,url is %s", __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }
    
    i_paras->obj_handle = obj_handle;

    *prop_provider = g_system_bonding_acinfo_provider;
    *count = G_N_ELEMENTS(g_system_bonding_acinfo_provider);

    return VOS_OK;
}


LOCAL gint32 check_system_eth_url(const gchar *eth_url_in, OBJ_HANDLE *eth_obj_handle)
{
    int iRet = -1;
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar memberid[PROP_VAL_LENGTH] = {0};
    gchar eth_url[MAX_URL_LEN] = {0};
    guint8 port_type = 0;
    gint32 result_val = VOS_ERR;

    if (NULL == eth_url_in) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return result_val;
    }

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get slot id failed", __FUNCTION__);
        return result_val;
    }

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &obj_list);
    if (DFL_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: get %s obj handle failed", __FUNCTION__, ETH_CLASS_NAME_BUSY_ETH);
        return result_val;
    }

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        obj_handle = (OBJ_HANDLE)obj_node->data;

        
        (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_PORT_TYPE, &port_type);
        continue_if_fail(ETH_PORT_TYPE_PHYSICAL == port_type);

        ret = redfish_get_sys_eth_memberid(obj_handle, memberid, sizeof(memberid));
        continue_if_fail(VOS_OK == ret);

        iRet = snprintf_s(eth_url, sizeof(eth_url), sizeof(eth_url) - 1, URI_FORMAT_SYS_ETH, slot_id, memberid);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        if (0 == g_ascii_strcasecmp(eth_url_in, (const gchar *)eth_url)) {
            do_if_expr(NULL != eth_obj_handle, (*eth_obj_handle = obj_handle));
            result_val = VOS_OK;
            break;
        }
    }
    g_slist_free(obj_list);

    do_if_fail(VOS_OK == result_val, debug_log(DLOG_ERROR, "%s: invalid url is %s", __FUNCTION__, eth_url_in));

    return result_val;
}


LOCAL gint32 fill_create_bond_input_list(json_object *body_jso_checked, GSList **input_list, json_object **message_info)
{
    errno_t safe_fun_ret = EOK;
    gint32 result_val = HTTP_INTERNAL_SERVER_ERROR;
    gint32 ret;
    const gchar *bond_name = NULL;
    gint32 bond_type;
    gint32 mii_link_monitor_frequency;
    json_object *link_jso = NULL;
    json_object *temp_jso = NULL;
    json_object *bonded_interfaces = NULL;
    json_object *odata_id = NULL;
    GSList *list_value = NULL;
    guint32 array_count = 0;
    guint32 i = 0;
    gchar json_pointer[MAX_URL_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gchar ports[MAX_URL_LEN] = {0};
    gchar os_eth_name[PROP_VAL_LENGTH] = {0};
    const gchar *address_origin = PROPERTY_VALUE_DEFAULT_STRING;
    const gchar *link_status = PROPERTY_VALUE_DEFAULT_STRING;
    guint8 is_onboot = G_MAXUINT8;

    if (NULL == body_jso_checked || NULL == input_list || NULL == message_info) {
        debug_log(DLOG_ERROR, "%s: NULL Pointer", __FUNCTION__);
        return result_val;
    }

    (void)get_element_str(body_jso_checked, RFPROP_COMMON_NAME, &bond_name);
    (void)json_object_object_get_ex(body_jso_checked, RFPROP_BONDING_TYPE, &temp_jso);
    if (json_type_int != json_object_get_type(temp_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_BONDING_TYPE, message_info,
            dal_json_object_get_str(temp_jso), RFPROP_BONDING_TYPE);
        return HTTP_BAD_REQUEST;
    }
    bond_type = json_object_get_int(temp_jso);
    temp_jso = NULL;
    (void)json_object_object_get_ex(body_jso_checked, RF_PROPERTY_LINK_MONITOR_FREQUENCY, &temp_jso);
    if (json_type_int != json_object_get_type(temp_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RF_PROPERTY_LINK_MONITOR_FREQUENCY, message_info,
            dal_json_object_get_str(temp_jso), RF_PROPERTY_LINK_MONITOR_FREQUENCY);
        return HTTP_BAD_REQUEST;
    }
    mii_link_monitor_frequency = json_object_get_int(temp_jso);
    temp_jso = NULL;
    (void)json_object_object_get_ex(body_jso_checked, RFPROP_LINKS, &link_jso);

    ret = json_object_object_get_ex(body_jso_checked, RF_PROPERTY_ADDR_ORIGIN, &temp_jso);
    do_if_expr(TRUE == ret, address_origin = dal_json_object_get_str(temp_jso); temp_jso = NULL);

    ret = json_object_object_get_ex(body_jso_checked, RF_PROPERTY_LINK_STATUS, &temp_jso);
    do_if_expr(TRUE == ret, link_status = dal_json_object_get_str(temp_jso); temp_jso = NULL);

    ret = json_object_object_get_ex(body_jso_checked, RF_PROPERTY_IS_ONBOOT, &temp_jso);
    do_if_expr(TRUE == ret, is_onboot = (TRUE == json_object_get_boolean(temp_jso)) ? TRUE : FALSE; temp_jso = NULL);

    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(bond_name));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_byte((guint8)bond_type)); // schema中限制了取值范围
    list_value = g_slist_append(list_value,
        (gpointer)g_variant_new_uint32((guint32)mii_link_monitor_frequency)); // schema中限制了取值范围
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(address_origin));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_string(link_status));
    list_value = g_slist_append(list_value, (gpointer)g_variant_new_byte(is_onboot));
    if (NULL != link_jso) {
        // sizeof(json_pointer)大小为1024，大于格式化后字符串长度，无需判断返回值
        (void)snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1, "%s/%s", RFPROP_LINKS,
            RFPROP_BONDED_INTERFACES);
        // bondedinterfaces不存在，报错
        ret = json_object_object_get_ex(link_jso, RFPROP_BONDED_INTERFACES, &bonded_interfaces);
        goto_label_do_info_if_fail(TRUE == ret, error_exit, result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_PROP_MISSING, json_pointer, message_info, json_pointer));
        // 为空数组报错
        array_count = json_object_array_length(bonded_interfaces);
        goto_label_do_info_if_expr(0 == array_count, error_exit, result_val = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, json_pointer, message_info, json_pointer));

        for (i = 0; i < array_count; i++) {
            // sizeof(json_pointer)大小为1024，大于格式化后字符串长度，无需判断返回值
            (void)snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1, "%s/%s/%u", RFPROP_LINKS,
                RFPROP_BONDED_INTERFACES, i);
            temp_jso = json_object_array_get_idx(bonded_interfaces, i);
            ret = json_object_object_get_ex(temp_jso, RFPROP_ODATA_ID, &odata_id);
            goto_label_do_info_if_fail(TRUE == ret, error_exit, result_val = HTTP_BAD_REQUEST;
                (void)strncat_s(json_pointer, sizeof(json_pointer), RFPROP_ODATA_ID, strlen(RFPROP_ODATA_ID));
                (void)create_message_info(MSGID_PROP_MISSING, json_pointer, message_info, json_pointer));

            ret = check_system_eth_url(dal_json_object_get_str(odata_id), &obj_handle);
            goto_label_do_info_if_fail(VOS_OK == ret, error_exit, result_val = HTTP_BAD_REQUEST;
                (void)snprintf_s(json_pointer, sizeof(json_pointer), sizeof(json_pointer) - 1,
                "The value for %s/%s/%d/%s is invalid", RFPROP_LINKS, RFPROP_BONDED_INTERFACES, i, RFPROP_ODATA_ID);
                (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, message_info, json_pointer));
            (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_OSETH_NAME, os_eth_name,
                sizeof(os_eth_name));
            safe_fun_ret = strncat_s(ports, sizeof(ports), os_eth_name, strlen(os_eth_name));
            do_if_expr(safe_fun_ret != EOK,
                debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            if (i < array_count - 1) {
                safe_fun_ret =
                    strncat_s(ports, sizeof(ports), RF_IPV4_PROP_SPLIT_FLAG, strlen(RF_IPV4_PROP_SPLIT_FLAG));
                do_val_if_expr(safe_fun_ret != EOK,
                    debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
            }
        }
        list_value = g_slist_append(list_value, (gpointer)g_variant_new_string((const gchar *)ports));
    }

    *input_list = list_value;

    return VOS_OK;
error_exit:

    do_if_fail(NULL == list_value, g_slist_free_full(list_value, (GDestroyNotify)g_variant_unref));
    return result_val;
}


gint32 create_network_bonding(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso)
{
    gint32 ret;
    gint32 result = 0;
    json_object *message_jso = NULL;
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    TASK_MONITOR_INFO_S *create_bond_info = NULL;
    OBJ_HANDLE eth_obj = 0;

    // URL示例:  /redfish/v1/Systems/1/NetworkBondings
    if (NULL == i_param || NULL == body_jso_checked || NULL == id || 0 == id_len || NULL == message_array_jso) {
        debug_log(DLOG_DEBUG, "%s: invalid input param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_system_uri_valid(i_param->uri);
    goto_label_do_info_if_expr(TRUE != ret, error_exit,
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result = HTTP_NOT_FOUND);

    // 创建bond的句柄使用任意一个物理网口的句柄就行了，这里暂时使用类的第一个句柄
    ret = dal_get_object_handle_nth(ETH_CLASS_NAME_BUSY_ETH, 0, &eth_obj);
    if (DFL_OK != ret) {
        // 获取失败认为是资源不存在
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result = HTTP_NOT_FOUND;
        goto error_exit;
    }
    debug_log(DLOG_DEBUG, "%s: object_name = %s", __FUNCTION__, dfl_get_object_name(eth_obj));

    
    ret = fill_create_bond_input_list(body_jso_checked, &input_list, &message_jso);
    if (VOS_OK != ret) {
        result = ret;
        goto error_exit;
    }

    
    ret = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, eth_obj,
        ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_CREATE_BOND, AUTH_ENABLE, i_param->user_role_privilege, input_list,
        &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    switch (ret) {
        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            result = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_jso);
            goto error_exit;

        
        case VOS_ERR_NOTSUPPORT:
            result = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, &message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        default:
            debug_log(DLOG_DEBUG, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                BUSY_ETH_METHOD_CREATE_BOND, ret);
            result = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }
    (void)snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "Bond Creation");
    create_bond_info = task_info_new_function(eth_obj, free_state_flag, BUSY_ETH_ATTRIBUTE_CONFIG_STATE,
        BUSY_ETH_ATTRIBUTE_CONFIG_RESULT, oration_name, i_param->uri, HTTP_CREATE);
    if (NULL == create_bond_info) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    create_bond_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, create_bond_info, body_jso_checked,
        i_param->uri, &i_param->val_jso);
    if (RF_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(create_bond_info);
        result = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result = VOS_OK;
error_exit:

    if (HTTP_INTERNAL_SERVER_ERROR == result && NULL != message_array_jso) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
    }

    if (NULL != message_jso) {
        (void)json_object_array_add(message_array_jso, message_jso);
    }

    return result;
}


gint32 delete_network_bonding(PROVIDER_PARAS_S *i_param, json_object *o_message_array_jso)
{
    int iRet;
    gint32 ret;
    gint32 result = 0;
    json_object *message_jso = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    guint32 free_state_flag = 0;
    OBJ_HANDLE eth_obj = 0;
    gchar oration_name[PROP_VAL_LENGTH] = {0};
    TASK_MONITOR_INFO_S *delete_bond_info = NULL;

    // URL示例:  /redfish/v1/Systems/1/NetworkBondings/team00
    if (NULL == i_param || NULL == o_message_array_jso) {
        debug_log(DLOG_DEBUG, "%s: invalid input param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_check_system_uri_valid(i_param->uri);
    if (TRUE != ret) {
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result = HTTP_NOT_FOUND;
        goto error_exit;
    }

    // 删除bond的句柄使用任意一个物理网口的句柄就行了，这里暂时使用类的第一个句柄
    ret = dal_get_object_handle_nth(ETH_CLASS_NAME_BUSY_ETH, 0, &eth_obj);
    if (DFL_OK != ret) {
        // 获取失败认为是资源不存在
        (void)create_message_info(MSGID_RSC_MISSING_URI, NULL, &message_jso, i_param->uri);
        result = HTTP_NOT_FOUND;
        goto error_exit;
    }
    debug_log(DLOG_DEBUG, "%s: object_name = %s", __FUNCTION__, dfl_get_object_name(eth_obj));
    input_list = g_slist_append(input_list, (gpointer)g_variant_new_string(i_param->member_id));
    
    ret = uip_call_class_method_redfish(i_param->is_from_webui, i_param->user_name, i_param->client, eth_obj,
        ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_DELETE_BOND, AUTH_ENABLE, i_param->user_role_privilege, input_list,
        &output_list);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    

    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            result = HTTP_FORBIDDEN;
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &message_jso);
            goto error_exit;

        case VOS_OK:
            free_state_flag = g_variant_get_uint32((GVariant *)g_slist_nth_data(output_list, 0));
            g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
            debug_log(DLOG_DEBUG, "%s: free_state_flag = %d", __FUNCTION__, free_state_flag);
            break;

            
        case VOS_ERR_NOTSUPPORT:
            result = HTTP_BAD_REQUEST;
            (void)create_message_info(MSGID_CONFIG_IP_FAILED, NULL, &message_jso, RF_MSGDIS_BMA_NOT_PRESENT);
            goto error_exit;

        default:
            debug_log(DLOG_DEBUG, "%s:uip_call_class_method_redfish failed,method=%s,ret=%d", __FUNCTION__,
                BUSY_ETH_METHOD_DELETE_BOND, ret);
            result = HTTP_INTERNAL_SERVER_ERROR;
            goto error_exit;
    }

    iRet = snprintf_s(oration_name, sizeof(oration_name), sizeof(oration_name) - 1, "Bond(%s) Deletion",
        i_param->member_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    delete_bond_info = task_info_new_function(eth_obj, free_state_flag, BUSY_ETH_ATTRIBUTE_CONFIG_STATE,
        BUSY_ETH_ATTRIBUTE_CONFIG_RESULT, oration_name, i_param->uri, HTTP_DELETE);
    if (NULL == delete_bond_info) {
        debug_log(DLOG_DEBUG, "%s: task_info_new_function failed", __FUNCTION__);
        result = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }

    delete_bond_info->rsc_privilege = USERROLE_BASICSETTING;
    ret = create_new_task(oration_name, ipinfo_config_status_monitor_fn, delete_bond_info, NULL, i_param->uri,
        &i_param->val_jso);
    if (RF_OK != ret) {
        debug_log(DLOG_DEBUG, "%s: create_new_task failed", __FUNCTION__);
        task_monitor_info_free(delete_bond_info);
        result = HTTP_INTERNAL_SERVER_ERROR;
        goto error_exit;
    }
    result = VOS_OK;
error_exit:

    if (HTTP_INTERNAL_SERVER_ERROR == result && NULL != o_message_array_jso) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
    }

    if (NULL != message_jso) {
        (void)json_object_array_add(o_message_array_jso, message_jso);
    }

    return result;
}
