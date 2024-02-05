
#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "pme_app/mctp_mgnt/mctp_usr_api.h"
#include "redfish_provider.h"

LOCAL gint32 get_chassis_networkport_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_silknum(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_linkstatus(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_chassis_networkport_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _get_chassis_networkport_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 _set_chassis_networkport_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);


LOCAL gint32 __get_chassis_eth_statistics(OBJ_HANDLE obj_handle, json_object *huawei);



LOCAL gint32 get_chassis_networkport_active_linktechnology(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);


LOCAL PROPERTY_PROVIDER_S g_chassis_networkport_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_NAME, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_name, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_id, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_PORT_ACTIVE_LINK_TECHNOLOGY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_active_linktechnology, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_STATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PORT_NUMBER, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_silknum, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PORT_LINKSTATUS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_linkstatus, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_PORT_ADDRESSES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_networkport_addresses, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, _get_chassis_networkport_oem, _set_chassis_networkport_oem, NULL, ETAG_FLAG_ENABLE},
    
};


gint32 chassis_networkport_etag_del_property(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_ETH_PORT_STATISTIC, RF_PROPERTY_WORK_SPEED, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_ETH_PORT_STATISTIC, RF_PROPERTY_TX_SN_SPEED, NULL}},
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_ETH_PORT_STATISTIC, RF_PROPERTY_RX_SN_SPEED, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };
    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


LOCAL gint32 get_chassis_networkport_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE netcard_obj = 0;
    gchar dl_string_value[FWINV_STR_MAX_LEN] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = dfl_get_referenced_object(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_obj);
    return_val_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = get_network_adapter_id(netcard_obj, dl_string_value, sizeof(dl_string_value));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, CHASSIS_NETWORK_PORT_MEMBERID,
        slot_id, dl_string_value);
    return_val_do_info_if_expr(VOS_OK >= ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : snprintf_s uri fail", __FUNCTION__));

    *o_result_jso = json_object_new_string((const gchar *)odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkport_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar odata_id[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    return_val_do_info_if_fail(
        (NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras)),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    ret = get_network_port_url(i_paras->obj_handle, slot_id, CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID_S, odata_id,
        sizeof(odata_id));
    return_val_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string((const gchar *)odata_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 trans_networkport_name(char* str, gint32 buf_size)
{
    GSList* card_list = NULL;
    gint32 id;
    gint32 ret;

    ret = dfl_get_object_list(CLASS_PANGEA_PCIE_CARD, &card_list); 
    if (ret != RET_OK) {
        return RET_OK;
    }

    if (card_list != NULL) {
        g_slist_free(card_list);
        card_list = NULL;
    }

    ret = vos_str2int(str, 10, &id, NUM_TPYE_INT32); 
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "trans str to int failed, ret=%d.", ret);
        return ret;
    }

    if (id <= 0) {
        return RET_OK;
    }

    (void)snprintf_s(str, buf_size, buf_size - 1, "%d", id - 1); 
    return RET_OK;
}


LOCAL gint32 get_chassis_networkport_name(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret;
    ret = trans_networkport_name(i_paras->member_id, MAX_MEM_ID_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Transform network port failed, ret=%d.", ret);
    }

    
    *o_result_jso = json_object_new_string(i_paras->member_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_networkport_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_string(i_paras->member_id);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkport_active_linktechnology(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint32 func_type = 0;
    json_object *ret_json_val = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 查询网络设备功能类型
    ret = dal_get_property_value_uint32(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);
    return_val_do_info_if_fail(ret == VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get property NetDevFunctionType fail.\n", __FUNCTION__, __LINE__));

    
    if (func_type & NETCARD_BUSIPORT_TYPE_ETH) {
        ret_json_val = json_object_new_string(RFPROP_ETHERNET);
    } else if (func_type & NETCARD_BUSIPORT_TYPE_FC) {
        ret_json_val = json_object_new_string(RFPROP_FIBRE_CHANNEL);
    } else if (func_type & NETCARD_BUSIPORT_TYPE_IB) {
        ret_json_val = json_object_new_string(RFPROP_INFINI_BAND);
    } else {
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = ret_json_val;
    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkport_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar available_flag = 0;
    const gchar *state = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_AVAILABLEFLAG, &available_flag);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get value for prop AvailableFlag fail(%d)", __func__, ret));

    
    state = (AVAILABLE_FLAG_DISABLED == available_flag) ? DISABLED_STRING : ENABLED_STRING;
    (void)get_resource_status_property(NULL, NULL, state, o_result_jso, FALSE);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkport_linkstatus(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guint8 link_status = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_LINK_STATUS, &link_status);
    return_val_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR);

    
    if (LINK_DOWN == link_status) {
        *o_result_jso = json_object_new_string("Down");
    } else if (LINK_UP == link_status) {
        *o_result_jso = json_object_new_string("Up");
    } else {
        
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkport_silknum(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar card_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    
    ret = dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_CARD_TYPE, &card_type);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get netport card type fail, ret:%d", __FUNCTION__, ret));
    return_val_do_info_if_expr(card_type == BUSINESSPORT_CARD_TYPE_VIR, HTTP_OK, (*o_result_jso = NULL));

    *o_result_jso = json_object_new_string(i_paras->member_id);
    
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_chassis_networkport_addresses(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar addresses[FWINV_STR_MAX_LEN] = {0};
    gchar wwpn[FWINV_STR_MAX_LEN] = {0};
    gchar wwnn[FWINV_STR_MAX_LEN] = {0};
    guint32 func_type = 0;
    OBJ_HANDLE fc_handle = 0;
    const gchar *mac_prop = BUSY_ETH_GROUP_ATTRIBUTE_MAC;

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = dal_get_property_value_uint32(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);
    return_val_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_array();
    return_val_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR);

    if (0 != (func_type & NETDEV_FUNCTYPE_FC)) {
        ret = dfl_get_referenced_object(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fc_handle);
        return_val_do_info_if_expr(ret != DFL_OK, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
            *o_result_jso = NULL);

        ret = dal_get_property_value_string(fc_handle, PROPERTY_FC_CARD_PORT_WWPN, wwpn, FWINV_STR_MAX_LEN);
        do_if_expr(VOS_OK == ret && g_strcmp0(wwpn, "N/A") && g_strcmp0(wwpn, "00:00:00:00:00:00"),
            json_object_array_add(*o_result_jso, json_object_new_string((const gchar *)wwpn)));

        ret = dal_get_property_value_string(fc_handle, PROPERTY_FC_CARD_PORT_WWNN, wwnn, FWINV_STR_MAX_LEN);
        do_if_expr(VOS_OK == ret && g_strcmp0(wwnn, "N/A") && g_strcmp0(wwnn, "00:00:00:00:00:00"),
            json_object_array_add(*o_result_jso, json_object_new_string((const gchar *)wwnn)));
    } else if ((0 != (func_type & NETDEV_FUNCTYPE_ETHERNET)) || (0 != (func_type & NETDEV_FUNCTYPE_IB))) {
        
        if ((func_type & NETDEV_FUNCTYPE_IB) != 0) {
            mac_prop = BUSY_ETH_ATTRIBUTE_ACTUAL_MAC_ADDR;
        }
        ret = dal_get_property_value_string(i_paras->obj_handle, mac_prop, addresses, FWINV_STR_MAX_LEN);
        do_if_expr(VOS_OK == ret && g_strcmp0(addresses, "N/A") && g_strcmp0(addresses, "00:00:00:00:00:00"),
            json_object_array_add(*o_result_jso, json_object_new_string((const gchar *)addresses)));
    }

    return HTTP_OK;
}


LOCAL void _redfish_get_interface_type(OBJ_HANDLE obj_handle, json_object *huawei)
{
    json_object *interface_type_jso = NULL;
    guint8 interface_type = 0xff;
    const gchar *interface_type_str = NULL;

    goto_label_do_info_if_expr(0 == obj_handle || NULL == huawei, EXIT,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_IFTYPE, &interface_type);

    if (PORT_TYPE_OPTICALPORT == interface_type) {
        interface_type_str = RFVALUE_OPTICALPORT;
    } else if (PORT_TYPE_ELETICALPORT == interface_type) {
        interface_type_str = RFVALUE_ELETICALPORT;
    } else {
        goto EXIT;
    }

    interface_type_jso = json_object_new_string(interface_type_str);
EXIT:
    json_object_object_add(huawei, CHASSIS_NETWORK_PORT_TYPE, interface_type_jso);
}


#define MAXSPEED_STR_MAX_LEN 32
LOCAL void _redfish_get_interface_maxspeed(OBJ_HANDLE obj_handle, json_object* huawei)
{
    json_object* interface_speed_jso = NULL;
    gchar speed[MAXSPEED_STR_MAX_LEN] = {0};
    gchar tmp[MAXSPEED_STR_MAX_LEN] = {0};
    guint32 maxspeed = 0;
    guint32 port_type = 0;
    gint32 ret;

    if (obj_handle == 0 || huawei == NULL) {
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__);
        goto EXIT;
    }

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_INTER_SPEED, speed, MAXSPEED_STR_MAX_LEN);

    if (strlen(speed) == 0 && !g_strcmp0(speed, PROPERTY_VALUE_DEFAULT_STRING)) {
        debug_log(DLOG_ERROR, "%s : str len = 0 or N/A", __FUNCTION__);
        goto EXIT;
    }

    // InterfaceSpeed 标准格式：10GE、10G、56G/100G
    // 是否含有非法字符或不包含G
    if (strchr(speed, ',') || strchr(speed, '*') || !strchr(speed, 'G')) {
        debug_log(DLOG_ERROR, "%s : prop contains ',' or '*', Speed str = %s", __FUNCTION__, speed);
        goto EXIT;
    }

    ret = dal_get_interface_maxspeed_str2int(speed, &maxspeed);
    if (ret == RET_ERR) {
        goto EXIT;
    }

    if (dal_is_customized_by_cmcc()) {
        ret = snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%dMb/s", maxspeed * 1000);  // 1000单位换算
    } else {
        (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &port_type);
        if (port_type == NETCARD_BUSIPORT_TYPE_ETH) {  // 以太网卡
            ret = snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%uGE", maxspeed);
        } else {   // 非以太网卡
            ret = snprintf_s(tmp, sizeof(tmp), sizeof(tmp) - 1, "%uG", maxspeed);
        }
    }

    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s : snprintf_s err ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }

    interface_speed_jso = json_object_new_string(tmp);
EXIT:
    json_object_object_add(huawei, CHASSIS_NETWORK_PORT_SPEED, interface_speed_jso);
}


LOCAL gint32 _redfish_get_businessport_portname(OBJ_HANDLE obj_handle, json_object *huawei)
{
    gint32 ret;
    guint32 func_type = 0;
    OBJ_HANDLE fc_handle = 0;

    return_val_do_info_if_fail(NULL != huawei, VOS_ERR, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    ret = dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop NetDevFuncType fail(%d).", __func__, ret));

    if (NETCARD_BUSIPORT_TYPE_FC == func_type) {
        ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fc_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: get object handle of reference slave port fail(%d).", __func__, ret));

        (void)rf_add_property_jso_string(fc_handle, PROPERTY_FC_CARD_PORT_PORT_NAME, PROTERY_PORT_NAME, huawei);
    } else {
        (void)rf_add_property_jso_string(obj_handle, BUSY_ETH_ATTRIBUTE_OSETH_NAME, PROTERY_PORT_NAME, huawei);
    }

    return VOS_OK;
}


LOCAL gint32 _redfish_get_businessport_interfacetype(OBJ_HANDLE obj_handle, json_object *huawei)
{
    gint32 ret;
    guint32 func_type = 0;
    OBJ_HANDLE fc_handle = 0;

    return_val_do_info_if_fail(NULL != huawei, VOS_ERR, debug_log(DLOG_ERROR, "%s: input param error.", __func__));

    ret = dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop NetDevFuncType fail(%d).", __func__, ret));

    if (NETCARD_BUSIPORT_TYPE_FC == func_type) {
        ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fc_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s: get object handle of reference slave port fail(%d).", __func__, ret));

        (void)rf_add_property_jso_string(fc_handle, PROPERTY_FC_CARD_PORT_PORT_TYPE, RFPROP_INTERFACE_TYPE, huawei);
    } else {
        json_object_object_add(huawei, RFPROP_INTERFACE_TYPE, NULL);
    }

    return VOS_OK;
}


LOCAL void _redfish_get_opticalmodule_link(PROVIDER_PARAS_S *i_paras, json_object *huawei)
{
    json_object *opticalmodule = NULL;
    json_object *odata_id = NULL;
    gchar uri[MAX_URI_LENGTH] = {0};
    gint32 ret;
    OBJ_HANDLE op_handle = 0;
    guint8 present = 0;
    guint8 if_type = 0xff;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    return_do_info_if_expr(VOS_OK != provider_paras_check(i_paras) || NULL == huawei,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    ret = dal_get_specific_object_string(CLASS_OPTICAL_MODULE, PROPERTY_OPT_MDL_REF_PORT_OBJ,
        dfl_get_object_name(i_paras->obj_handle), &op_handle);
    if (ret != DFL_OK) {
        ret = dal_get_specific_object_string(CLASS_VIRTUAL_OPTICAL_MODULE, PROPERTY_OPT_MDL_REF_PORT_OBJ,
            dfl_get_object_name(i_paras->obj_handle), &op_handle);
    }

    return_do_info_if_fail(ret == DFL_OK,
        debug_log(DLOG_DEBUG, "%s : get optical modle object handle by businessport fail.", __FUNCTION__));

    (void)dal_get_property_value_byte(op_handle, PROPERTY_OPT_MDL_PRESENT, &present);

    (void)dal_get_property_value_byte(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_IFTYPE, &if_type);
    // 非光口或者光模块不在位不显示
    return_do_info_if_fail((if_type == PORT_TYPE_OPTICALPORT) && (present == 1),
        debug_log(DLOG_DEBUG, "%s : not support optical modle or not present.", __FUNCTION__));

    opticalmodule = json_object_new_object();
    return_do_info_if_expr(opticalmodule == NULL, json_object_object_add(huawei, RFRESC_OPTICAL_MODULE, opticalmodule);
        debug_log(DLOG_ERROR, "%s : new optical modle object fail.", __FUNCTION__));
    json_object_object_add(huawei, RFRESC_OPTICAL_MODULE, opticalmodule);

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_if_expr(ret != VOS_OK);
    ret = get_network_port_url(i_paras->obj_handle, slot_id, URI_FORMAT_CHASSIS_OPTICAL_MODULE_S, uri, sizeof(uri));
    if (ret == VOS_OK) {
        odata_id = json_object_new_string((const char *)uri);
    }

    json_object_object_add(opticalmodule, RFPROP_ODATA_ID, odata_id);
}


LOCAL void _redfish_get_businessport_bdf(OBJ_HANDLE obj_handle, json_object *huawei)
{
#define BDF_STR_MAX_LEN 32

    json_object *bdf_jso = NULL;
    gchar bdf_str[BDF_STR_MAX_LEN + 1] = {0};

    goto_label_do_info_if_expr(0 == obj_handle || NULL == huawei, EXIT,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_BDF, bdf_str, BDF_STR_MAX_LEN);

    if (0 != strlen(bdf_str) && g_strcmp0(bdf_str, PROPERTY_VALUE_DEFAULT_STRING)) {
        bdf_jso = json_object_new_string((const gchar *)bdf_str);
    }

EXIT:
    json_object_object_add(huawei, RFPROP_PORT_BDF, bdf_jso);
}

LOCAL void __redfish_get_businessport_lldp(OBJ_HANDLE obj_handle, json_object *json_obj)
{
#define LLDP_NOT_SUPPORT        0

    json_object *lldp_jso = NULL;
    gint32 ret;
    OBJ_HANDLE card_handle = 0;
    guint8 lldp_support;
    guint8 lldp_enabled;

    if (obj_handle == 0 || json_obj == NULL) {
        json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, lldp_jso);
        return;
    }

    lldp_jso = json_object_new_object();
    if (lldp_jso == NULL) {
        json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, NULL);
        return;
    }

    ret = dal_get_specific_object_position(obj_handle, CLASS_NETCARD_NAME, &card_handle);
    if (ret != DFL_OK) {
        json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, NULL);
        json_object_put(lldp_jso);
        return;
    }

    ret = dal_get_property_value_byte(card_handle, PORPERTY_NETCARD_LLDP_SUPPORTED, &lldp_support);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get lldp supported fail", __FUNCTION__);
        json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, NULL);
        json_object_put(lldp_jso);
        return;
    }

    if (lldp_support == LLDP_NOT_SUPPORT) {
        debug_log(DLOG_DEBUG, "%s: netcard not support lldp", __FUNCTION__);
        json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, NULL);
        json_object_put(lldp_jso);
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, BUSY_ETH_ATTRIBUTE_LLDP_ENABLE_MODE, &lldp_enabled);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: failed to get lldp enable flag", __FUNCTION__);
        json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, NULL);
        json_object_put(lldp_jso);
        return;
    }

    json_object_object_add(lldp_jso, RFPROP_PORT_LLDP_ENABLE, json_object_new_boolean(lldp_enabled));
    json_object_object_add(lldp_jso, RFPROP_PORT_LLDP_WORKMODE, json_object_new_string(RFPROP_PORT_LLDP_TX_WORKMODE));
    json_object_object_add(json_obj, RFPROP_PORT_LLDP_SERVICE, lldp_jso);
    return;
}

/*****************************************************************************
 函 数 名  : _redfish_get_businessport_firmware_version
 功能描述  : 获取网口固件版本信息
 输入参数  : OBJ_HANDLE obj_handle
             json_object* huawei
 输出参数  : 无
 返 回 值  : LOCAL

 修改历史      :
  1.日    期   : 2018年8月21日
    作    者   : jwx372839
    修改内容   : 新生成函数

**************************************************************************** */
LOCAL void _redfish_get_businessport_firmware_version(OBJ_HANDLE obj_handle, json_object *huawei)
{
    json_object *fw_ver_jso = NULL;
    gchar fw_ver_str[PROP_VAL_LENGTH] = {0};

    goto_label_do_info_if_expr(0 == obj_handle || NULL == huawei, EXIT,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_FIRM_VERSION, fw_ver_str, PROP_VAL_LENGTH);

    if (0 != strlen(fw_ver_str) && g_strcmp0(fw_ver_str, PROPERTY_VALUE_DEFAULT_STRING)) {
        fw_ver_jso = json_object_new_string((const gchar *)fw_ver_str);
    }

EXIT:
    json_object_object_add(huawei, RFPROP_PORT_FIRM_VERSION, fw_ver_jso);
}


LOCAL void _redfish_get_businessport_driver_version(OBJ_HANDLE obj_handle, json_object *huawei)
{
    json_object *driver_ver_jso = NULL;
    gchar driver_ver_str[PROP_VAL_LENGTH] = {0};

    goto_label_do_info_if_expr(0 == obj_handle || NULL == huawei, EXIT,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_DRIVER_VERSION, driver_ver_str, PROP_VAL_LENGTH);

    if (0 != strlen(driver_ver_str) && g_strcmp0(driver_ver_str, PROPERTY_VALUE_DEFAULT_STRING)) {
        driver_ver_jso = json_object_new_string((const gchar *)driver_ver_str);
    }

EXIT:
    json_object_object_add(huawei, RFPROP_PORT_DRIVER_VERSION, driver_ver_jso);
}


LOCAL void _redfish_get_businessport_driver_name(OBJ_HANDLE obj_handle, json_object *huawei)
{
    json_object *driver_name_jso = NULL;
    gchar driver_name_str[PROP_VAL_LENGTH] = {0};

    goto_label_do_info_if_expr(0 == obj_handle || NULL == huawei, EXIT,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    (void)dal_get_property_value_string(obj_handle, BUSY_ETH_ATTRIBUTE_DRIVER_NAME, driver_name_str, PROP_VAL_LENGTH);

    if (0 != strlen(driver_name_str) && g_strcmp0(driver_name_str, PROPERTY_VALUE_DEFAULT_STRING)) {
        driver_name_jso = json_object_new_string((const gchar *)driver_name_str);
    }

EXIT:
    json_object_object_add(huawei, RFPROP_PORT_DRIVER_NAME, driver_name_jso);
}


LOCAL void _redfish_get_eth_link(PROVIDER_PARAS_S *i_paras, json_object *huawei)
{
    guint32 port_type = 0;
    gint32 ret;

    return_if_expr(NULL == huawei);

    ret = dal_get_property_value_uint32(i_paras->obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &port_type);
    if (VOS_OK == ret) {
        if (NETCARD_BUSIPORT_TYPE_IB == port_type) {
            get_system_eth_link(i_paras->obj_handle, huawei, URI_FORMAT_SYS_IB);
        } else if (NETCARD_BUSIPORT_TYPE_ETH == port_type) {
            get_system_eth_link(i_paras->obj_handle, huawei, URI_FORMAT_SYS_ETH);
        } else {
            json_object_object_add(huawei, RFPROP_ETH_RELATED_PORT, NULL);
        }
    } else {
        json_object_object_add(huawei, RFPROP_ETH_RELATED_PORT, NULL);
    }
}


LOCAL void _redfish_get_fc_port_state(OBJ_HANDLE obj_handle, json_object *result_jso)
{
    gint32 ret;
    guint32 func_type = 0;
    OBJ_HANDLE fiber_handle = 0;

    if (result_jso == NULL) {
        return;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get property failed, ret(%d).\n", __FUNCTION__, ret);
        return;
    }

    
    if (!(func_type & NETCARD_BUSIPORT_TYPE_FC)) {
        return;
    }

    
    ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fiber_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get fiber channel handle failed, ret(%d), object_name(%s).\n", __FUNCTION__, ret,
            dfl_get_object_name(obj_handle));
        return;
    }

    
    rf_add_property_jso_string(fiber_handle, PROPERTY_FC_CARD_PORT_LINK_STATUS, RFPROP_PORT_FC_PORT_STATE, result_jso);
}


LOCAL gint32 _redfish_get_businessport_technology(OBJ_HANDLE obj_handle, json_object *huawei)
{
    guint32 func_type = 0;

    return_val_if_expr((NULL == huawei), VOS_ERR);

    (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);

    if (func_type & NETCARD_BUSIPORT_TYPE_ETH) {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, json_object_new_string(RFPROP_ETHERNET));
    } else if (func_type & NETCARD_BUSIPORT_TYPE_FC) {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, json_object_new_string(RFPROP_FIBRE_CHANNEL));
    } else if (func_type & NETCARD_BUSIPORT_TYPE_ISCSI) {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, json_object_new_string(RFPROP_ISCSI));
    } else if (func_type & NETCARD_BUSIPORT_TYPE_FCOE) {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, json_object_new_string(RFPROP_FCOE));
    } else if (func_type & NETCARD_BUSIPORT_TYPE_OPA) {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, json_object_new_string(RFPROP_OPA));
    } else if (func_type & NETCARD_BUSIPORT_TYPE_IB) {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, json_object_new_string(RFPROP_IB));
    } else {
        json_object_object_add(huawei, RFPROP_PORT_TECHNOLOGY, NULL);
    }

    return VOS_OK;
}


LOCAL gint32 _redfish_get_businessport_fcid(OBJ_HANDLE obj_handle, json_object *huawei)
{
    gint32 ret = 0xff;
    guint32 func_type = 0;
    OBJ_HANDLE fc_handle = 0;
    gchar fcid[FWINV_STR_MAX_LEN] = {0};

    return_val_if_expr((NULL == huawei), VOS_ERR);

    (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);

    if (NETCARD_BUSIPORT_TYPE_FC == func_type) {
        ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fc_handle);
        if (VOS_OK == ret) {
            (void)dal_get_property_value_string(fc_handle, PROPERTY_FC_CARD_PORT_FCID, fcid, FWINV_STR_MAX_LEN);
            json_object_object_add(huawei, RFPROP_PORT_FC_ID, json_object_new_string((const gchar *)fcid));

            return VOS_OK;
        }
    }

    json_object_object_add(huawei, RFPROP_PORT_FC_ID, NULL);

    return VOS_OK;
}


LOCAL gint32 _redfish_get_businessport_speed(OBJ_HANDLE obj_handle, json_object *huawei)
{
    gint32 ret = 0xff;
    guint32 func_type = 0;
    OBJ_HANDLE fc_handle = 0;
    guint32 speed = 0;

    return_val_if_expr((NULL == huawei), VOS_ERR);

    (void)dal_get_property_value_uint32(obj_handle, BUSY_ETH_ATTRIBUTE_NETDEV_FUNCTYPE, &func_type);

    if (NETCARD_BUSIPORT_TYPE_FC == func_type) {
        ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fc_handle);
        if (VOS_OK == ret) {
            (void)dal_get_property_value_uint32(fc_handle, PROPERTY_FC_CARD_PORT_LINK_SPEED, &speed);

            if (RF_INFO_INVALID_DWORD != speed) {
                json_object_object_add(huawei, RFPROP_PORT_SPEED, json_object_new_int(speed));

                return VOS_OK;
            }
        }
    }
    json_object_object_add(huawei, RFPROP_PORT_SPEED, NULL);

    return VOS_OK;
}


LOCAL gint32 _get_chassis_networkport_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *huawei = NULL;

    return_val_do_info_if_fail(
        (o_result_jso != NULL && o_message_jso != NULL && provider_paras_check(i_paras) == VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR);

    huawei = json_object_new_object(); // 返回OK是为了显示huawei为null
    return_val_do_info_if_expr(huawei == NULL, HTTP_OK,
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);

    // 查询接口类型(光口或者电口)
    _redfish_get_interface_type(i_paras->obj_handle, huawei);

    // 查询接口最大速率
    _redfish_get_interface_maxspeed(i_paras->obj_handle, huawei);

    
    _redfish_get_businessport_portname(i_paras->obj_handle, huawei);
    _redfish_get_businessport_interfacetype(i_paras->obj_handle, huawei);
    

    
    _redfish_get_businessport_bdf(i_paras->obj_handle, huawei);
    

    // 获取lldp状态
    __redfish_get_businessport_lldp(i_paras->obj_handle, huawei);
    
    // 获取网口固件版本、驱动版本、驱动名称3个属性，对双芯片网卡，不同网口可能各有各的驱动
    // 原网卡资源的属性不变，以免出现上游兼容性问题，网口处新增接口解决该问题
    _redfish_get_businessport_firmware_version(i_paras->obj_handle, huawei);
    _redfish_get_businessport_driver_version(i_paras->obj_handle, huawei);
    _redfish_get_businessport_driver_name(i_paras->obj_handle, huawei);

    
    
    (void)_redfish_get_businessport_technology(i_paras->obj_handle, huawei);

    _redfish_get_businessport_fcid(i_paras->obj_handle, huawei);

    
    _redfish_get_fc_port_state(i_paras->obj_handle, huawei);

    _redfish_get_businessport_speed(i_paras->obj_handle, huawei);

    _redfish_get_eth_link(i_paras, huawei);
    

    // 显示光模块的导航
    _redfish_get_opticalmodule_link(i_paras, huawei);

    
    __get_chassis_eth_statistics(i_paras->obj_handle, huawei);
    

    return HTTP_OK;
}


gint32 get_network_port_url(OBJ_HANDLE busi_obj, const gchar *slot_id, const gchar *url_template, gchar *netport_url,
    guint32 buf_len)
{
    gint32 ret_val;
    OBJ_HANDLE netcard_obj = 0;
    OBJ_HANDLE ref_com_handle = 0;
    gchar       netcard_id[MAX_RSC_ID_LEN] = {0};
    guint8 virtual_flag = 0;
    gchar       port_num[PROP_VAL_LENGTH] = {0};

    return_val_if_expr(slot_id == NULL || url_template == NULL || netport_url == NULL || buf_len < 1, VOS_ERR);

    // 虚拟网口不生成url
    return_val_if_expr(is_virtual_eth_port(busi_obj), VOS_ERR);

    ret_val = dfl_get_referenced_object(busi_obj, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &netcard_obj);
    return_val_if_expr(ret_val != DFL_OK, VOS_ERR);
    (void)dal_get_property_value_byte(netcard_obj, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
    return_val_if_expr(virtual_flag == 1, VOS_ERR);
    ret_val = dfl_get_referenced_object(netcard_obj, PROPERTY_NETCARD_REF_COMPONENT, &ref_com_handle);
    return_val_if_expr(ret_val != DFL_OK, VOS_ERR);
    ret_val = get_location_devicename(ref_com_handle, netcard_id, sizeof(netcard_id));
    return_val_if_expr(ret_val != VOS_OK, VOS_ERR);
    ret_val = get_networkport_number(busi_obj, port_num, sizeof(port_num));
    return_val_if_expr(ret_val != VOS_OK, VOS_ERR);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    ret_val = snprintf_s(netport_url, buf_len, buf_len - 1, url_template, slot_id, netcard_id, port_num);
#pragma GCC diagnostic pop
    return_val_if_expr(ret_val <= 0, VOS_ERR);

    return VOS_OK;
}


LOCAL gint32 set_chassis_networkport_lldpenable(PROVIDER_PARAS_S* i_paras, json_object* val_jso,
    json_object** o_message_jso)
{
    GSList  *input_list = NULL;
    gint32  ret;
    const gchar* prop_name = "Oem/Huawei/LldpService/LldpEnabled";
    gboolean lldp_enable;

    lldp_enable = json_object_get_boolean(val_jso);
    input_list = g_slist_append(input_list, g_variant_new_byte(lldp_enable));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, ETH_CLASS_NAME_BUSY_ETH, BUSY_ETH_METHOD_SET_PORT_LLDP_ENABLE, AUTH_ENABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    input_list = NULL;

    switch (ret) {
        case VOS_OK:
            return HTTP_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, o_message_jso, prop_name);
            break;
        case MCTP_ERR_PCIE_CHAN_DISABLED:
            (void)create_message_info(MSGID_PCIE_PORT_NOT_ENABLED_ERR, NULL, o_message_jso);
            break;
        default:
            (void)create_message_info(MSGID_CONFIG_PORT_LLDP_ERR, NULL, o_message_jso);
            break;
    }

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 set_chassis_networkport_lldpservice(PROVIDER_PARAS_S* i_paras, json_object* val_jso,
    json_object** o_message_jso)
{
    json_object *data_jso = NULL;
    gint32 ret_val = HTTP_BAD_REQUEST;
    gint32 ret;
    json_object* o_message_jso_tmp = NULL;

    *o_message_jso = json_object_new_array();
    if (json_object_object_get_ex(val_jso, RFPROP_PORT_LLDP_ENABLE, &data_jso) == TRUE) {
        ret = set_chassis_networkport_lldpenable(i_paras, data_jso, &o_message_jso_tmp);
        if (ret == HTTP_OK) {
            ret_val = HTTP_OK;
        }

        if (o_message_jso_tmp != NULL) {
            json_object_array_add(*o_message_jso, o_message_jso_tmp);
            o_message_jso_tmp = NULL;
        }
    }

    return ret_val;
}


LOCAL gint32 set_chassis_networkport_oem_data(PROVIDER_PARAS_S* i_paras, json_object* oem_data_jso,
    json_object** o_message_jso)
{
    json_object *data_jso = NULL;
    gint32 ret_val = HTTP_BAD_REQUEST;
    gint32 ret;
    json_object* o_message_jso_tmp = NULL;

    *o_message_jso = json_object_new_array();
    if (json_object_object_get_ex(oem_data_jso, RFPROP_PORT_LLDP_SERVICE, &data_jso) == TRUE) {
        ret = set_chassis_networkport_lldpservice(i_paras, data_jso, &o_message_jso_tmp);
        if (ret == HTTP_OK) {
            ret_val = HTTP_OK;
        }

        if (json_object_get_type(o_message_jso_tmp) == json_type_array) {
            gint32 msg_index;
            gint32 array_len = json_object_array_length(o_message_jso_tmp);

            for (msg_index = 0; msg_index < array_len; msg_index++) {
                (void)json_object_array_add(*o_message_jso,
                    json_object_get(json_object_array_get_idx(o_message_jso_tmp, msg_index)));
            }
        }
        json_object_put(o_message_jso_tmp);
        o_message_jso_tmp = NULL;
    }

    return ret_val;
}


LOCAL gint32 _set_chassis_networkport_oem(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    json_object *manu_obj = NULL;
    json_bool ret_bool;

    
    if (o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret_bool = json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &manu_obj);
    if (ret_bool != TRUE) {
        return HTTP_BAD_REQUEST;
    }

    return set_chassis_networkport_oem_data(i_paras, manu_obj, o_message_jso);
}

/*****************************************************************************
 函 数 名  : redfish_check_networkport_uri_valid
 功能描述  : 网络端口URI合法性校验
 输入参数  : const gchar *netport_url
 输出参数  : 无

 修改历史      :
  1.日    期   : 2017年3月16日
    作    者   : awx381331
    修改内容   : 新生成函数

  2.日    期   : 2017年8月16日
    作    者   : zwx382755
    修改内容   : SREA02109352-001-002   增加对象句柄出参

**************************************************************************** */
LOCAL gint32 redfish_check_networkport_uri_valid(const gchar *netport_url, OBJ_HANDLE *business_port_obj)
{
    gint32 ret_val;
    gchar       slot_id[MAX_RSC_ID_LEN] = {0};
    GSList *busi_obj_list = NULL;
    GSList *node = NULL;
    OBJ_HANDLE busi_obj = 0;
    gchar       url_val[MAX_URL_LEN] = {0};

    return_val_if_expr(NULL == business_port_obj, VOS_ERR);

    ret_val = redfish_get_board_slot(slot_id, sizeof(slot_id));
    return_val_if_expr(ret_val != VOS_OK, VOS_ERR);

    ret_val = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &busi_obj_list);
    return_val_if_expr(ret_val != DFL_OK || busi_obj_list == NULL, VOS_ERR);

    for (node = busi_obj_list; node; node = g_slist_next(node)) {
        busi_obj = (OBJ_HANDLE)(node->data);
        continue_if_expr(is_lom_hide_phy_port((OBJ_HANDLE)node->data) == TRUE);
        ret_val = get_network_port_url(busi_obj, slot_id, CHASSIS_NETWORK_ADAPTERS_PORTS_MEMBERID_S, url_val,
            sizeof(url_val));
        continue_if_expr(ret_val != VOS_OK);

        if (0 == strcasecmp(netport_url, url_val)) {
            *business_port_obj = busi_obj;
            break;
        }
    }
    g_slist_free(busi_obj_list);

    if (node == NULL) {
        return VOS_ERR;
    }

    return VOS_OK;
}

gint32 chassis_networkport_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    gboolean bool_ret;
    OBJ_HANDLE obj_handle = 0;
    OBJ_HANDLE chassis_handle;
    
    guchar x86_enabled = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &x86_enabled);
    return_val_if_fail(ENABLE == x86_enabled, HTTP_NOT_FOUND);

    
    bool_ret = redfish_check_chassis_uri_valid(i_paras->uri, &chassis_handle);
    do_if_expr(!bool_ret, return HTTP_NOT_FOUND);
    

    
    ret = check_enclosure_component_type(chassis_handle, FALSE);
    
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);

    
    ret = redfish_check_networkport_uri_valid(i_paras->uri, &obj_handle);
    do_if_expr(VOS_OK != ret, return HTTP_NOT_FOUND);
    i_paras->obj_handle = obj_handle;
    

    *prop_provider = g_chassis_networkport_provider;
    *count = sizeof(g_chassis_networkport_provider) / sizeof(PROPERTY_PROVIDER_S);

    

    return VOS_OK;
}


LOCAL gint32 __get_chassis_eth_statistics(OBJ_HANDLE obj_handle, json_object *huawei)
{
    OBJ_HANDLE fiber_channel_port = 0;
    json_object *related_port_info = NULL;
    json_object *property_jso = NULL; // 属性 值的 json格式
    gchar prop_val[PROP_VAL_LENGTH] = {0};
    guint8 prop_val_byte = 0;

    goto_label_do_info_if_expr(0 == obj_handle || NULL == huawei, exit,
        debug_log(DLOG_ERROR, "%s : NULL pointer.", __FUNCTION__));

    gint32 ret = dfl_get_referenced_object(obj_handle, BUSY_ETH_ATTRIBUTE_REF_SLAVEPORT, &fiber_channel_port);
    if (DFL_OK != ret) {
        // 没有引用的就没有网口统计信息
        goto exit;
    }

    related_port_info = json_object_new_object();
    if (NULL == related_port_info) {
        debug_log(DLOG_ERROR, "%s: alloc memory failed", __FUNCTION__);
        goto exit;
    }

    rf_add_property_jso_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_TX_SN_SPEED, RF_PROPERTY_TX_SN_SPEED,
        related_port_info);
    rf_add_property_jso_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_RX_SN_SPEED, RF_PROPERTY_RX_SN_SPEED,
        related_port_info);

    
    ret = dal_get_property_value_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_SN_STAGE, &prop_val_byte);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        json_object_object_add(related_port_info, RF_PROPERTY_SN_STAGE, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    
    if ((SN_STAGE_WAITING_SIGNAL_INT0 == prop_val_byte) || (SN_STAGE_WAITING_SIGNAL_INT1 == prop_val_byte)) {
        property_jso = json_object_new_string(SN_STAGE_WAITING_SIGNAL);
    } else if ((SN_STAGE_NEGOTIATING_INT2 == prop_val_byte) || (SN_STAGE_NEGOTIATING_INT3 == prop_val_byte) ||
        (SN_STAGE_NEGOTIATING_INT4 == prop_val_byte)) {
        property_jso = json_object_new_string(SN_STAGE_NEGOTIATING);
    } else if (SN_STAGE_NEGOTIATION_COMPLETED_INT == prop_val_byte) {
        property_jso = json_object_new_string(SN_STAGE_NEGOTIATION_COMPLETED);
    }
    json_object_object_add(related_port_info, RF_PROPERTY_SN_STAGE, property_jso);

    
    ret = dal_get_property_value_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_SFP_OPEN, &prop_val_byte);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        json_object_object_add(related_port_info, RF_PROPERTY_SFP_OPEN, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    property_jso = NULL;
    
    if ((OPTICAL_MODULE_ENABLED == prop_val_byte) || (OPTICAL_MODULE_DISABLED == prop_val_byte)) {
        property_jso = json_object_new_boolean(prop_val_byte);
        do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__));
    }
    json_object_object_add(related_port_info, RF_PROPERTY_SFP_OPEN, property_jso);

    
    ret = dal_get_property_value_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_WORK_SPEED, &prop_val_byte);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        json_object_object_add(related_port_info, RF_PROPERTY_WORK_SPEED, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));
    property_jso = NULL;
    
    if ((INVALID_DATA_BYTE != prop_val_byte) && (ZERO_DATA != prop_val_byte)) {
        property_jso = json_object_new_int(prop_val_byte);
        do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__));
    }
    json_object_object_add(related_port_info, RF_PROPERTY_WORK_SPEED, property_jso);

    
    ret = dal_get_property_value_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_WORK_TOPO, &prop_val_byte);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        json_object_object_add(related_port_info, RF_PROPERTY_WORK_TOPO, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    (void)memset_s(prop_val, PROP_VAL_LENGTH, 0, PROP_VAL_LENGTH);
    
    if (prop_val_byte == WORK_MODE_LOOP_INT) {
        (void)strcpy_s(prop_val, PROP_VAL_LENGTH, WORK_MODE_LOOP);
    } else if (prop_val_byte == WORK_MODE_NON_LOOP_INT) {
        (void)strcpy_s(prop_val, PROP_VAL_LENGTH, WORK_MODE_NON_LOOP);
    }
    if (strlen(prop_val)) {
        property_jso = json_object_new_string((const gchar *)prop_val);
        do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
        json_object_object_add(related_port_info, RF_PROPERTY_WORK_TOPO, property_jso);
    } else {
        json_object_object_add(related_port_info, RF_PROPERTY_WORK_TOPO, NULL);
    }

    rf_add_property_jso_uint16(fiber_channel_port, PROPERTY_FC_CARD_PORT_TX_BB_CREDIT, RF_PROPERTY_TX_BB_CREDIT,
        related_port_info);
    rf_add_property_jso_uint16(fiber_channel_port, PROPERTY_FC_CARD_PORT_RX_BB_CREDIT, RF_PROPERTY_RX_BB_CREDIT,
        related_port_info);

    
    ret = dal_get_property_value_byte(fiber_channel_port, PROPERTY_FC_CARD_PORT_PORT_STATUS, &prop_val_byte);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        json_object_object_add(related_port_info, RF_PROPERTY_PORT_STATUS, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    property_jso = NULL;
    
    if (prop_val_byte == PORT_STATUS_LINK_UP_INT) {
        property_jso = json_object_new_string(PORT_STATUS_LINK_UP);
    } else if (prop_val_byte == PORT_STATUS_DISABLED_INT) {
        property_jso = json_object_new_string(PORT_STATUS_DISABLED_STR);
    } else if (prop_val_byte == PORT_STATUS_WAITING_SIGNAL_INT) {
        property_jso = json_object_new_string(PORT_STATUS_WAITING_SIGNAL);
    }
    json_object_object_add(related_port_info, RF_PROPERTY_PORT_STATUS, property_jso);

exit:

    json_object_object_add(huawei, RFPROP_ETH_PORT_STATISTIC, related_port_info);

    return HTTP_OK;
}
