
#include "redfish_provider.h"

#define NETWORK_TYPE_ERR (-2)

#define IPMI_PORT1_ERROR (-1)
#define IPMI_PORT2_ERROR (-2)
#define IPMI_PORT_ERROR_ALL (-9)
#define IPMI_PORT_ERROR_INTERNAL (-99)



typedef enum {
    CHECK_PORT = 0,
    CHECK_RANGE,
    CHECK_PROTOCOL
} RF_CHECK_TYPE_E;


LOCAL gint32 get_networkprotocol_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_http(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_https(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_ssh(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_ipmi(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_snmp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_kvmip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_vmm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_ssdp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_template(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint32 i_id_num);


LOCAL gint32 get_networkprotocol_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso); // 更新odataid的值


LOCAL gint32 set_networkprotocol_http(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_https(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_ssh(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_ipmi(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_snmp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_kvmip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_vmm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_ssdp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_data_template(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gchar *prop, gchar *method);
LOCAL gint32 set_networkprotocol_template(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint32 i_id_num, gchar *protocol_name);
LOCAL gint32 get_networkprotocol_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_networkprotocol_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_networkprotocol_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_rmcp_server_state(guchar from_webui_flag, const gchar *username, const gchar *ip);

LOCAL PROPERTY_PROVIDER_S g_networkprotocol_provider[] = {
    {RFPROP_NETWORKPROTOCOL_HOSTNAME, BMC_CLASEE_NAME, PROPERTY_BMC_HOSTNAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_networkprotocol_hostname, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_FQDN, CLASS_NAME_DNSSETTING, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_networkprotocol_fqdn, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_HTTP, SERVER_PORT_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_http, set_networkprotocol_http, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_HTTPS, SERVER_PORT_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_https, set_networkprotocol_https, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_SSH, SERVER_PORT_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_ssh, set_networkprotocol_ssh, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_IPMI, RMCP_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_ipmi, set_networkprotocol_ipmi, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_SNMP, CLASS_SNMP, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_snmp, set_networkprotocol_snmp, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_KVMIP, KVM_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_kvmip, set_networkprotocol_kvmip, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_VMM, VMM_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_vmm, set_networkprotocol_vmm, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_NETWORKPROTOCOL_SSDP, CLASS_EX_PORT_CONFIG, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_ssdp, set_networkprotocol_ssdp, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_networkprotocol_odataid, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_networkprotocol_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_COMMON_OEM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_networkprotocol_oem, set_networkprotocol_oem, NULL, ETAG_FLAG_ENABLE}
    
};


LOCAL gint32 get_networkprotocol_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
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

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1, MANAGERS_NETPROCOL_METADATA,
        slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(odata_context);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}



LOCAL void rf_check_ret(RF_CHECK_TYPE_E check_type, gint32 ret, gint32 *ret_result, gchar *protocol_json,
    json_object **o_message_jso, json_object *message, json_object *obj_json)
{
    const gchar* value_str = dal_json_object_get_str(obj_json);

    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            *ret_result = HTTP_OK;
            return;

            
        case COMP_CODE_INVALID_CMD:
            create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, protocol_json, &message, protocol_json);
            *ret_result = (*ret_result == HTTP_OK) ? HTTP_OK : HTTP_NOT_IMPLEMENTED;
            break;
            

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, protocol_json, &message, protocol_json);
            break;

        case COMP_CODE_OUTOF_RANGE:
            if (CHECK_PORT == check_type) {
                create_message_info(MSGID_PORTID_NOT_MODIFIED, protocol_json, &message, value_str, protocol_json);
            } else {
                create_message_info(MSGID_PROP_NOT_IN_LIST, protocol_json, &message, value_str, protocol_json);
            }
            break;

        case RF_ERROR: 
            if (CHECK_PORT == check_type) {
                create_message_info(MSGID_PORTID_NOT_MODIFIED, protocol_json, &message, value_str, protocol_json);
            } else if (CHECK_PROTOCOL == check_type) {
                create_message_info(MSGID_PROP_NOT_IN_LIST, protocol_json, &message, value_str, protocol_json);
            } else {
                create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            }
            break;

        case NETWORK_TYPE_ERR:

            if (CHECK_PROTOCOL == check_type) {
                create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
                *ret_result = (*ret_result == HTTP_OK) ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;
            }
            break;

        default: 
            debug_log(DLOG_ERROR, "%s, set %s return = %d.", __FUNCTION__, protocol_json, ret);
            
            if (dal_is_rpc_exception(ret) == TRUE) {
                create_message_info(MSGID_SERVICE_RESTART_IN_PROGRESS, protocol_json, &message);
            } else {
                create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
                *ret_result = (*ret_result == HTTP_OK) ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;
            }
    }

    if (message != NULL) {
        json_object_array_add(*o_message_jso, message);
    }
}


LOCAL gint32 get_networkprotocol_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar HostName[HOST_NAME_MAX_LEN + 1] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(g_networkprotocol_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, g_networkprotocol_provider[i_paras->index].pme_prop_name, HostName,
        sizeof(HostName));

    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get hostname property fail.\n", __FUNCTION__, __LINE__));
    *o_result_jso = json_object_new_string(HostName);

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_fqdn(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar HostName[HOST_NAME_MAX_LEN + 1] = {0};
    gchar DomainName[DNS_DOMAINNAME_MAX_LEN + 1] = {0};
    gchar fqdn[HOST_NAME_MAX_LEN + DNS_DOMAINNAME_MAX_LEN + 2] = {0};
    guchar NetConfigEnable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_NETCONFIG, &NetConfigEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_NetConfigEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr((SERVICE_NETCONFIG_ENABLE != NetConfigEnable), HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_BMC_HOSTNAME, HostName, sizeof(HostName));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get hostname property fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_DOMAINNAME, DomainName, sizeof(DomainName));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get domainname property fail.\n", __FUNCTION__, __LINE__));

    
    iRet = snprintf_s(fqdn, sizeof(fqdn), sizeof(fqdn) - 1, "%s.%s", HostName, DomainName);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    *o_result_jso = json_object_new_string(fqdn);

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_http(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_networkprotocol_template(i_paras, o_message_jso, o_result_jso, SERVICE_CONFIGURATION_WEB_HTTP);
    return ret;
}


LOCAL gint32 get_networkprotocol_https(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = get_networkprotocol_template(i_paras, o_message_jso, o_result_jso, SERVICE_CONFIGURATION_WEB_HTTPS);
    return ret;
}


LOCAL gint32 get_networkprotocol_ssh(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    ret = get_networkprotocol_template(i_paras, o_message_jso, o_result_jso, SERVICE_CONFIGURATION_SSH);
    return ret;
}


LOCAL gint32 get_networkprotocol_ipmi(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar port_state = 0;
    guint16 port_data = 0;
    guchar IPMILanEnable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_IPMI, &IPMILanEnable);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_snmpenable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr(SERVICE_STATE_DISABLE == IPMILanEnable, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(RMCP_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_LAN_PLUS_STATE, &port_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_RMCP_PORT1, &port_data);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(0 != port_data, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_object();

    if (1 == port_state) {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(FALSE));
    }

    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(port_data));

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_snmp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar port_state = 0;
    gint32 port_data = 0;
    guchar snmpenable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SNMPD, &snmpenable);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_snmpenable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr(SERVICE_STATE_DISABLE == snmpenable, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SNMP_STATE, &port_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, PROPERTY_SNMP_PORTID, &port_data);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(0 != port_data, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_object();

    if (1 == port_state) {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(FALSE));
    }

    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(port_data));

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_kvmip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar port_state = 0;
    gint32 port_data = 0;
    guchar KVMEnable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &KVMEnable);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_snmpenable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr(SERVICE_STATE_DISABLE == KVMEnable, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    ret = dal_get_object_handle_nth(PROXY_KVM_CLASS_NAME, 0, &obj_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, KVM_PROPERTY_STATE, &port_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, KVM_PROPERTY_PORT, &port_data);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(0 != port_data, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_object();

    if (1 == port_state) {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(FALSE));
    }

    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(port_data));

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_vmm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar port_state = 0;
    gint32 port_data = 0;
    guchar KVMEnable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &KVMEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_snmpenable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr((SERVICE_STATE_DISABLE == KVMEnable), HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    
    ret = dal_get_object_handle_nth(PROXY_VMM_CLASS_NAME, 0, &obj_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, VMM_PROPERTY_STATE, &port_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_int32(obj_handle, VMM_PROPERTY_PORT, &port_data);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(0 != port_data, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_object();

    if (1 == port_state) {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(FALSE));
    }

    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(port_data));

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_ssdp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar service_mode = 0;
    guint32 data = 0;
    gchar scope[256] = {0};
    guchar SSDPEnable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_SSDP, &SSDPEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_SSDPEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr((SERVICE_STATE_DISABLE == SSDPEnable), HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    

    ret = dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_object();

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SSDP_CONFIG_MODE, &service_mode);
    if (ret == VOS_OK) {
        if (service_mode & (SERVICE_STATE_ENABLE << SSDP_RECEIVE_BIT)) {
            json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE,
                json_object_new_boolean(SERVICE_STATE_ENABLE));
        } else {
            json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE,
                json_object_new_boolean(SERVICE_STATE_DISABLE));
        }
    } else {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, NULL);
    }

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SSDP_CONFIG_PORT, &data);
    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID,
        (VOS_OK != ret || 0 == data) ? NULL : json_object_new_int(data));
    data = 0;

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SSDP_CONFIG_INTERVAL, &data);
    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_NMIS,
        (VOS_OK != ret) ? NULL : json_object_new_int(data));
    data = 0;

    
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SSDP_CONFIG_MULTICAST_TTL, &data);
    json_object_object_add(*o_result_jso, PROPERTY_SSDP_CONFIG_MULTICAST_TTL,
        (VOS_OK != ret || 0 == data) ? NULL : json_object_new_int(data));

    

    (void)dal_get_property_value_string(obj_handle, PROPERTY_SSDP_CONFIG_IPV6ADDR, scope, sizeof(scope));
    return_val_do_info_if_expr(0 == g_strcmp0(scope, SSDP_IPV6_LINK), HTTP_OK,
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_NIPS,
        json_object_new_string(SSDP_IPV6_SCOPE_LINK)));
    return_val_do_info_if_expr(0 == g_strcmp0(scope, SSDP_IPV6_SITE), HTTP_OK,
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_NIPS,
        json_object_new_string(SSDP_IPV6_SCOPE_SITE)));
    return_val_do_info_if_expr(0 == g_strcmp0(scope, SSDP_IPV6_ORG), HTTP_OK,
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_NIPS,
        json_object_new_string(SSDP_IPV6_SCOPE_ORG)));
    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_NIPS, NULL);

    return HTTP_OK;
}


LOCAL gint32 get_networkprotocol_template(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint32 i_id_num)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar port_state = 0;
    GVariant *property_data = NULL;
    const guint16 *port_data = NULL;
    gsize len = 0;
    guchar HTTPSEnable = 0;
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_HTTPS, &HTTPSEnable);
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get_httpsenable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_expr((SERVICE_STATE_DISABLE == HTTPSEnable), HTTP_INTERNAL_SERVER_ERROR);
    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: systems ethernetinterface link get board_type fail.\n", __FUNCTION__, __LINE__));

    if (BOARD_SWITCH == board_type) {
        return_val_if_expr((SERVICE_CONFIGURATION_WEB_HTTPS != i_id_num), HTTP_INTERNAL_SERVER_ERROR);
    }

    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret =
        dal_get_specific_object_uint32(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_SERVER_ID, i_id_num, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, SERVER_PORT_ATTRIBUTE_STATE, &port_state);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    ret = dfl_get_property_value(obj_handle, SERVER_PORT_ATTRIBUTE_PORT, &property_data);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    return_val_do_info_if_fail(NULL != property_data, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data NULL pointer.\n", __FUNCTION__, __LINE__));

    port_data = (const guint16 *)g_variant_get_fixed_array(property_data, &len, sizeof(guint16));

    
    *o_result_jso = json_object_new_object();

    if (1 == port_state) {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(FALSE));
    }

    json_object_object_add(*o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(*port_data));

    g_variant_unref(property_data);

    return HTTP_OK;
}


LOCAL void _get_vnc_info(json_object *huawei)
{
    json_object *vnc = NULL;
    guint8 vnc_enabled = 0xff;
    guint8 vnc_supported = 0;
    OBJ_HANDLE obj_handle = 0;
    json_object *property = NULL;
    gint32 vnc_port = 0;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_VNC, &vnc_supported);
    goto_label_do_info_if_fail(1 == vnc_supported, exit,
        debug_log(DLOG_MASS, "%s: the product not support vnc", __FUNCTION__));

    vnc = json_object_new_object();
    goto_label_do_info_if_expr(NULL == vnc, exit, debug_log(DLOG_ERROR, "%s: alloc vnc mem fail", __FUNCTION__));

    (void)dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &obj_handle);

    // 查询vnc是否开启
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_VNC_ENABLE_STATE, &vnc_enabled);

    if (0 == vnc_enabled || 1 == vnc_enabled) {
        property = json_object_new_boolean(vnc_enabled);
    }

    json_object_object_add(vnc, RFPROP_NETWORKPROTOCOL_STATE, property);
    property = NULL;

    // 查询vnc端口号
    (void)dal_get_property_value_int32(obj_handle, PROPERTY_VNC_PORT, &vnc_port);

    if (0 != vnc_port) {
        property = json_object_new_int((gint32)vnc_port); // 端口号最大65535，可以强转成有符号数
    }

    json_object_object_add(vnc, RFPROP_NETWORKPROTOCOL_PORTID, property);

exit:
    json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_VNC, vnc);
}



LOCAL gint32 _get_ipmi_info(json_object *huawei)
{
    json_object *ipmi_jso = NULL;
    guint8 rmcp_enabled = 0xff;
    guint8 rmcp_plus_enabled = 0xff;
    OBJ_HANDLE obj_handle = 0;
    gint32 ipmi_port1 = 0;
    gint32 ipmi_port2 = 0;
    gint32 ret;

    ipmi_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == ipmi_jso, VOS_ERR,
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_IPMI, NULL);
        debug_log(DLOG_INFO, "%s: alloc ipmi_jso mem failed", __FUNCTION__));

    ret = dal_get_object_handle_nth(RMCP_CLASS_NAME, 0, &obj_handle);
    
    return_val_do_info_if_expr(VOS_ERR == ret, ret, json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_IPMI, NULL);
        json_object_put(ipmi_jso); debug_log(DLOG_MASS, "%s: get RmcpConfig handle failed.ret=%d", __FUNCTION__, ret));
    
    // 查询 RMCP 使能状态
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LAN_STATE, &rmcp_enabled);

    if (0 == rmcp_enabled || 1 == rmcp_enabled) {
        json_object_object_add(ipmi_jso, RFPROP_RMCP_ENALBED, json_object_new_boolean(rmcp_enabled));
    } else {
        json_object_object_add(ipmi_jso, RFPROP_RMCP_ENALBED, NULL);
    }

    // 查询 RMCP_PLUS 使能状态
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_LAN_PLUS_STATE, &rmcp_plus_enabled);

    if (0 == rmcp_plus_enabled || 1 == rmcp_plus_enabled) {
        json_object_object_add(ipmi_jso, RFPROP_RMCP_PLUS_ENABLED, json_object_new_boolean(rmcp_plus_enabled));
    } else {
        json_object_object_add(ipmi_jso, RFPROP_RMCP_PLUS_ENABLED, NULL);
    }

    // 查询 ipmi port1 端口号
    (void)dal_get_property_value_int32(obj_handle, PROPERTY_RMCP_PORT1, &ipmi_port1);

    if (0 != ipmi_port1) {
        json_object_object_add(ipmi_jso, RFPROP_IPMI_PORT1, json_object_new_int(ipmi_port1));
    } else {
        json_object_object_add(ipmi_jso, RFPROP_IPMI_PORT1, NULL);
    }

    // 查询 ipmi port2 端口号
    (void)dal_get_property_value_int32(obj_handle, PROPERTY_RMCP_PORT2, &ipmi_port2);

    if (0 != ipmi_port2) {
        json_object_object_add(ipmi_jso, RFPROP_IPMI_PORT2, json_object_new_int(ipmi_port2));
    } else {
        json_object_object_add(ipmi_jso, RFPROP_IPMI_PORT2, NULL);
    }

    json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_IPMI, ipmi_jso);
    return VOS_OK;
}


LOCAL gint32 _get_nat_info(json_object *huawei)
{
    json_object *nat_jso = NULL;
    guint8 nat_supported = 0xff;
    OBJ_HANDLE obj_handle = 0;
    gint32 nat_port = 0;
    gint32 ret;

    nat_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == nat_jso, VOS_ERR,
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_NAT, NULL);
        debug_log(DLOG_INFO, "%s: alloc nat_jso mem failed", __FUNCTION__));

    ret = dal_get_specific_object_uint32(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_SERVER_ID,
        SERVICE_CONFIGURATION_HMMSSHNAT, &obj_handle);
    
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_NAT, NULL);
        json_object_put(nat_jso); debug_log(DLOG_MASS, "%s: get PortConfig handle failed.ret=%d", __FUNCTION__, ret));
    

    // 查询 NAT 使能状态
    (void)dal_get_property_value_byte(obj_handle, SERVER_PORT_ATTRIBUTE_STATE, &nat_supported);

    if (0 == nat_supported || 1 == nat_supported) {
        json_object_object_add(nat_jso, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(nat_supported));
    } else {
        json_object_object_add(nat_jso, RFPROP_NETWORKPROTOCOL_STATE, NULL);
    }

    // 查询 NAT 端口号
    (void)dal_get_property_value_int32(obj_handle, SERVER_PORT_ATTRIBUTE_PORT, &nat_port);

    if (0 != nat_port) {
        json_object_object_add(nat_jso, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(nat_port));
    } else {
        json_object_object_add(nat_jso, RFPROP_NETWORKPROTOCOL_PORTID, NULL);
    }

    json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_NAT, nat_jso);
    return VOS_OK;
}


LOCAL gint32 _get_video_info(json_object *huawei)
{
    json_object *video = NULL;
    guint8 video_supported = 0xff;
    OBJ_HANDLE obj_handle = 0;
    gint32 video_port = 0;
    gint32 ret;

    video = json_object_new_object();
    return_val_do_info_if_expr(NULL == video, VOS_ERR,
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_VIDEO, NULL);
        debug_log(DLOG_INFO, "%s: alloc video mem failed", __FUNCTION__));

    ret = dal_get_object_handle_nth(VIDEO_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK,
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_VIDEO, NULL);
        json_object_put(video); debug_log(DLOG_MASS, "%s: get Video handle failed.ret=%d", __FUNCTION__, ret));

    // 查询 video 使能状态
    ret = dal_get_property_value_byte(obj_handle, VIDEO_PROPERTY_STATE, &video_supported);
    return_val_do_info_if_expr(VOS_ERR == ret, ret, json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_VIDEO, video);
        debug_log(DLOG_MASS, "%s: get Video handle failed", __FUNCTION__));

    if (0 == video_supported || 1 == video_supported) {
        json_object_object_add(video, RFPROP_NETWORKPROTOCOL_STATE, json_object_new_boolean(video_supported));
    } else {
        json_object_object_add(video, RFPROP_NETWORKPROTOCOL_STATE, NULL);
    }

    // 查询video端口号
    (void)dal_get_property_value_int32(obj_handle, VIDEO_PROPERTY_PORT, &video_port);

    if (0 != video_port) {
        json_object_object_add(video, RFPROP_NETWORKPROTOCOL_PORTID, json_object_new_int(video_port));
    } else {
        json_object_object_add(video, RFPROP_NETWORKPROTOCOL_PORTID, NULL);
    }

    json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_VIDEO, video);
    return VOS_OK;
}



LOCAL gint32 _get_ssdp_notify_enabled(json_object *huawei)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 ssdp_mode = 0;
    json_object *ssdp_jso = NULL;

    (void)dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SSDP_CONFIG_MODE, &ssdp_mode);
    return_val_do_info_if_expr(VOS_ERR == ret, ret, json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_SSDP, NULL);
        debug_log(DLOG_MASS, "%s: get ssdp_mode failed, ret is %d", __FUNCTION__, ret));

    ssdp_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == ssdp_jso, VOS_ERR,
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_SSDP, NULL);
        debug_log(DLOG_ERROR, "%s: new ssdp_jso failed.", __FUNCTION__));

    
    if (ssdp_mode & (SERVICE_STATE_ENABLE << SSDP_SEND_BIT)) {
        json_object_object_add(ssdp_jso, RFPROP_NETWORKPROTOCOL_NOTIFY_ENABLED,
            json_object_new_boolean(SERVICE_STATE_ENABLE));
    } else {
        json_object_object_add(ssdp_jso, RFPROP_NETWORKPROTOCOL_NOTIFY_ENABLED,
            json_object_new_boolean(SERVICE_STATE_DISABLE));
    }

    json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_SSDP, ssdp_jso);
    return VOS_OK;
}


LOCAL gint32 _get_ssdp_info(json_object *huawei)
{
    (void)_get_ssdp_notify_enabled(huawei);

    return VOS_OK;
}


LOCAL gint32 get_networkprotocol_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei = NULL;
    guint8 vnc_supported = 0;
    
    guint8 ipmi_supported = 0;
    

    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));

    huawei = json_object_new_object();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_OK,
        debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__);
        json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei));

    json_object_object_add(*o_result_jso, RFPROP_COMMON_HUAWEI, huawei);
    // 判断产品是否支持VNC
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_VNC, &vnc_supported);

    if (1 == vnc_supported) {
        // 查询VNC信息
        _get_vnc_info(huawei);
    } else {
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_VNC, NULL);
    }

    
    // 获取 VIDEO 信息
    (void)_get_video_info(huawei);
    // 获取NAT 信息
    (void)_get_nat_info(huawei);
    // 判断产品是否支持IPMI
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_IPMI, &ipmi_supported);

    if (1 == ipmi_supported) {
        // 查询IPMI信息
        (void)_get_ipmi_info(huawei);
    } else {
        json_object_object_add(huawei, RFPROP_NETWORKPROTOCOL_IPMI, NULL);
    }

    

    
    (void)_get_ssdp_info(huawei);
    

    return HTTP_OK;
}


LOCAL gint32 set_networkprotocol_http(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = set_networkprotocol_template(i_paras, o_message_jso, o_result_jso, SERVICE_CONFIGURATION_WEB_HTTP,
        RFPROP_NETWORKPROTOCOL_HTTP);
    return ret;
}


LOCAL gint32 set_networkprotocol_https(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;

    ret = set_networkprotocol_template(i_paras, o_message_jso, o_result_jso, SERVICE_CONFIGURATION_WEB_HTTPS,
        RFPROP_NETWORKPROTOCOL_HTTPS);
    return ret;
}


LOCAL gint32 set_networkprotocol_ssh(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    ret = set_networkprotocol_template(i_paras, o_message_jso, o_result_jso, SERVICE_CONFIGURATION_SSH,
        RFPROP_NETWORKPROTOCOL_SSH);
    return ret;
}

LOCAL gint32 get_networkprotocol_ipmi_state(json_object *val_jso, gchar *prop_name, gchar *message_name,
    json_object **state_jso, json_object **o_message_jso)
{
    json_object *message = NULL;
    json_bool jso_ret;

    jso_ret = json_object_object_get_ex(val_jso, prop_name, state_jso);
    if (jso_ret != TRUE) {
        return HTTP_BAD_REQUEST;
    }

    
    if (json_object_get_type(*state_jso) == json_type_null) {
        create_message_info(MSGID_PROP_TYPE_ERR, message_name, &message, RF_VALUE_NULL, message_name);
        json_object_array_add(*o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }
    return RET_OK;
}

LOCAL gint32 set_networkprotocol_ipmi_state(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, gchar *mathod_name,
    json_object *state_jso)
{
    gint32 ret;
    gboolean state;
    guint8 state_value = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    state = json_object_get_boolean(state_jso);
    if (state == TRUE) {
        state_value = 1;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(state_value));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        RMCP_CLASS_NAME, mathod_name, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);
    return ret;
}

LOCAL gint32 get_networkprotocol_ipmi_port(json_object *val_jso, guint16 *port_value, json_object **obj_json,
    json_object **o_message_jso)
{
    json_object *message = NULL;
    gint32 port_data;
    json_bool jso_ret;

    jso_ret = json_object_object_get_ex(val_jso, RFPROP_NETWORKPROTOCOL_PORTID, obj_json);
    if (jso_ret != TRUE) {
        return HTTP_BAD_REQUEST;
    }

    if (json_object_get_type(*obj_json) != json_type_int) {
        create_message_info(MSGID_PROP_TYPE_ERR, "IPMI/Port", &message,
            ((json_object_get_type(*obj_json) == json_type_null) ? RF_VALUE_NULL : dal_json_object_get_str(*obj_json)),
            "IPMI/Port");
        json_object_array_add(*o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }

    port_data = json_object_get_int(*obj_json);
    if ((port_data < PORT_MIN_VALUE) || (port_data > PORT_MAX_VALUE)) {
        create_message_info(MSGID_PORTID_NOT_MODIFIED, "IPMI/Port", &message, dal_json_object_get_str(*obj_json),
            "IPMI/Port");
        json_object_array_add(*o_message_jso, message);
        return HTTP_BAD_REQUEST;
    }

    *port_value = (guint16)port_data;
    return RET_OK;
}

LOCAL gint32 set_networkprotocol_ipmi_port(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, gint16 port_value)
{
    gint32 ret;
    guint16 port_data2 = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    
    ret = dal_get_property_value_uint16(obj_handle, PROPERTY_RMCP_PORT2, &port_data2);
    if (ret != RET_OK || port_data2 == 0) {
        debug_log(DLOG_ERROR, "%s: get property_data failed, ret = %d.", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    input_list = g_slist_append(input_list, g_variant_new_byte(RMCP_PORT_SAVE));
    input_list = g_slist_append(input_list, g_variant_new_uint16(port_value));
    input_list = g_slist_append(input_list, g_variant_new_uint16(port_data2));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        obj_handle, RMCP_CLASS_NAME, METHOD_SET_RMCP_PORT, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    return ret;
}


LOCAL gint32 set_networkprotocol_ipmi(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = RET_OK;
    gint32 ret_result = HTTP_BAD_REQUEST;
    OBJ_HANDLE obj_handle = 0;
    json_object *obj_json = NULL;
    guint16 port_value = 0;
    json_object *message = NULL;
    gboolean set_protocol_state = FALSE;

    
    ret = rf_check_para_and_priv(i_paras, o_message_jso, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: check param and priv failed, ret= %d.", __FUNCTION__, ret);
        return ret;
    }

    
    ret = dal_get_object_handle_nth(RMCP_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth failed, ret = %d.", __FUNCTION__, ret);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_message_jso = json_object_new_array();
    if (*o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed.", __FUNCTION__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = get_networkprotocol_ipmi_state(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_STATE, "IPMI/ProtocolEnabled",
        &obj_json, o_message_jso);
    if (ret == RET_OK) {
        set_protocol_state = TRUE;
        ret = set_networkprotocol_ipmi_state(i_paras, obj_handle, METHOD_SET_LANPLUS_STATE, obj_json);
        rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, "IPMI/ProtocolEnabled", o_message_jso, message, obj_json);
    }

    
    obj_json = NULL;
    ret = get_networkprotocol_ipmi_port(i_paras->val_jso, &port_value, &obj_json, o_message_jso);
    if (ret == RET_OK) {
        ret = set_networkprotocol_ipmi_port(i_paras, obj_handle, port_value);
        rf_check_ret(CHECK_PORT, ret, &ret_result, "IPMI/Port", o_message_jso, message, obj_json);
        if (ret == RET_OK) {
            
            return ret_result;
        }
    }

    
    if (set_protocol_state == TRUE) {
        set_rmcp_server_state(i_paras->is_from_webui, i_paras->user_name, i_paras->client);
    }

    return ret_result;
}


LOCAL gint32 set_networkprotocol_snmp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret_state;
    gint32 ret_port;
    gint32 ret_result = HTTP_BAD_REQUEST;
    gint32 flags = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *obj_json = NULL;
    guchar port_state = 0;
    guint32 port_data = 0;
    json_object *message = NULL;
    gint32 ret = 0;
    
    return_val_do_info_if_fail(o_message_jso && VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    *o_message_jso = json_object_new_array();

    
    ret_port = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_PORTID, &obj_json);
    if (ret_port) {
        

        do_val_if_fail(!(json_type_int != json_object_get_type(obj_json)),
            (void)create_message_info(MSGID_PROP_TYPE_ERR, "SNMP/Port", &message,
            (json_object_get_type(obj_json) == json_type_null) ? RF_VALUE_NULL : dal_json_object_get_str(obj_json),
            "SNMP/Port");
            (void)json_object_array_add(*o_message_jso, message); flags = 1);

        if (1 != flags) {
            port_data = json_object_get_int(obj_json);
            if ((port_data < PORT_MIN_VALUE) || (port_data > PORT_MAX_VALUE)) {
                (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, "SNMP/Port", &message,
                    dal_json_object_get_str(obj_json), "SNMP/Port");
                (void)json_object_array_add(*o_message_jso, message);
                obj_json = NULL;
            } else {
                
                ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_handle);
                return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                    debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));
                
                input_list = g_slist_append(input_list, g_variant_new_int32(port_data));
                ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    obj_handle, CLASS_SNMP, METHOD_SNMP_SETSNMPPORTID, AUTH_ENABLE, i_paras->user_role_privilege,
                    input_list, NULL);

                uip_free_gvariant_list(input_list);
                input_list = NULL;

                (void)rf_check_ret(CHECK_PORT, ret, &ret_result, "SNMP/Port", o_message_jso, message, obj_json);

                obj_json = NULL;
            }
        }
    }

    flags = 0;

    
    ret_state = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_STATE, &obj_json);
    if (ret_state) {
        

        do_val_if_fail(!(NULL == obj_json), (void)create_message_info(MSGID_PROP_TYPE_ERR, "SNMP/ProtocolEnabled",
            &message, RF_VALUE_NULL, "SNMP/ProtocolEnabled");
            (void)json_object_array_add(*o_message_jso, message); flags = 1);

        if (1 != flags) {
            port_state = json_object_get_boolean(obj_json);
            
            ret = dal_get_object_handle_nth(CLASS_SNMP, 0, &obj_handle);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));
            
            input_list = g_slist_append(input_list, g_variant_new_byte(port_state));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                CLASS_SNMP, METHOD_SNMP_SETSNMPSTATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

            uip_free_gvariant_list(input_list);

            (void)rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, "SNMP/ProtocolEnabled", o_message_jso, message,
                obj_json);
        }
    }

    
    return ret_result;
}


LOCAL gint32 set_networkprotocol_kvmip(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret_result = HTTP_BAD_REQUEST;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *obj_json = NULL;
    json_object *message = NULL;
    
    return_val_do_info_if_fail(o_message_jso && VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    *o_message_jso = json_object_new_array();

    guint8 kvm_enabled = DISABLE;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (kvm_enabled == DISABLE) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_NETWORKPROTOCOL_KVMIP, &message,
            RF_VALUE_NULL, RFPROP_NETWORKPROTOCOL_KVMIP);
        (void)json_object_array_add(*o_message_jso, message);
        return HTTP_NOT_IMPLEMENTED;
    }

    gint32 ret = dal_get_object_handle_nth(PROXY_KVM_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get o_obj_handle fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret_port = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_PORTID, &obj_json);
    if (ret_port) {
        

        if (json_object_get_type(obj_json) != json_type_int) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, "KVMIP/Port", &message,
                json_object_get_type(obj_json) == json_type_null ? RF_VALUE_NULL : json_object_get_string(obj_json),
                "KVMIP/Port");
                (void)json_object_array_add(*o_message_jso, message);
        } else {
            guint32 port_data = json_object_get_int(obj_json);
            if ((port_data < PORT_MIN_VALUE) || (port_data > PORT_MAX_VALUE)) {
                (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, "KVMIP/Port", &message,
                    dal_json_object_get_str(obj_json), "KVMIP/Port");
                (void)json_object_array_add(*o_message_jso, message);
            } else {
                
                input_list = g_slist_append(input_list, g_variant_new_int32(port_data));
                ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    obj_handle, PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_PORT, AUTH_ENABLE, i_paras->user_role_privilege,
                    input_list, NULL);

                uip_free_gvariant_list(input_list);
                input_list = NULL;

                rf_check_ret(CHECK_PORT, ret, &ret_result, "KVMIP/Port", o_message_jso, message, obj_json);
            }
        }
    }

    obj_json = NULL;
    
    gint32 ret_state = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_STATE, &obj_json);
    if (ret_state) {
        

        if (obj_json == NULL) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, "KVMIP/ProtocolEnabled",
                &message, RF_VALUE_NULL, "KVMIP/ProtocolEnabled");
            (void)json_object_array_add(*o_message_jso, message);
        } else {
            guchar port_state = json_object_get_boolean(obj_json);
            
            input_list = g_slist_append(input_list, g_variant_new_byte(port_state));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
                NULL);

            uip_free_gvariant_list(input_list);

            rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, "KVMIP/ProtocolEnabled", o_message_jso, message, obj_json);
        }
    }

    
    return ret_result;
}


LOCAL gint32 set_networkprotocol_vmm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret_result = HTTP_BAD_REQUEST;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *obj_json = NULL;
    json_object *message = NULL;

    
    return_val_do_info_if_fail(o_message_jso && VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    *o_message_jso = json_object_new_array();

    guint8 kvm_enabled = DISABLE;
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_KVM, &kvm_enabled);
    if (kvm_enabled == DISABLE) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_NETWORKPROTOCOL_VMM, &message,
            RF_VALUE_NULL, RFPROP_NETWORKPROTOCOL_VMM);
        (void)json_object_array_add(*o_message_jso, message);
        return HTTP_NOT_IMPLEMENTED;
    }
    gint32 ret = dal_get_object_handle_nth(PROXY_VMM_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get o_obj_handle fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret_port = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_PORTID, &obj_json);
    if (ret_port) {
        

        if (json_object_get_type(obj_json) != json_type_int) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, "VirtualMedia/Port", &message,
                json_object_get_type(obj_json) == json_type_null ? RF_VALUE_NULL : dal_json_object_get_str(obj_json),
                "VirtualMedia/Port");
            (void)json_object_array_add(*o_message_jso, message);
        } else {
            guint32 port_data = json_object_get_int(obj_json);
            if ((port_data < PORT_MIN_VALUE) || (port_data > PORT_MAX_VALUE)) {
                (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, "VirtualMedia/Port", &message,
                    dal_json_object_get_str(obj_json), "VirtualMedia/Port");
                (void)json_object_array_add(*o_message_jso, message);
            } else {
                
                input_list = g_slist_append(input_list, g_variant_new_int32(port_data));
                ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    obj_handle, PROXY_VMM_CLASS_NAME, VMM_METHOD_SET_PORT, AUTH_ENABLE, i_paras->user_role_privilege,
                    input_list, NULL);

                uip_free_gvariant_list(input_list);
                input_list = NULL;
                rf_check_ret(CHECK_PORT, ret, &ret_result, "VirtualMedia/Port", o_message_jso, message, obj_json);
            }
        }
    }

    obj_json = NULL;
    
    gint32 ret_state = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_STATE, &obj_json);
    if (ret_state) {
        
        if (obj_json == NULL) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR,
                "VirtualMedia/ProtocolEnabled", &message, RF_VALUE_NULL, "VirtualMedia/ProtocolEnabled");
            (void)json_object_array_add(*o_message_jso, message);
        } else {
            guchar port_state = json_object_get_boolean(obj_json);
            
            input_list = g_slist_append(input_list, g_variant_new_byte(port_state));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                PROXY_VMM_CLASS_NAME, VMM_METHOD_SET_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
                NULL);

            uip_free_gvariant_list(input_list);
            rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, "VirtualMedia/ProtocolEnabled", o_message_jso, message,
                obj_json);
        }
    }

    
    return ret_result;
}



LOCAL gint32 set_networkprotocol_ssdp(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = 0;
    gint32 ret_result = HTTP_BAD_REQUEST;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *obj_json = NULL;
    guchar port_state = 0;
    guchar mode = 0;
    gchar protocol_json[100] = {0};
    const gchar *scope = NULL;
    json_object *message = NULL;
    gint32 flags = 0;

    
    return_val_do_info_if_fail(o_message_jso && VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    *o_message_jso = json_object_new_array();

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_STATE, &obj_json)) {
        

        (void)memset_s(protocol_json, sizeof(protocol_json), 0, sizeof(protocol_json));
        // sizeof(protocol_json)大小为100，大于格式化后字符串长度，无需判读返回值
        (void)snprintf_s(protocol_json, sizeof(protocol_json), sizeof(protocol_json) - 1, "%s/%s",
            RFPROP_NETWORKPROTOCOL_SSDP, RFPROP_NETWORKPROTOCOL_STATE);
        do_val_if_fail(json_object_get_type(obj_json) != json_type_null, flags = 1;
            (void)create_message_info(MSGID_PROP_TYPE_ERR, protocol_json, &message, RF_VALUE_NULL, protocol_json);
            (void)json_object_array_add(*o_message_jso, message); message = NULL);

        if (1 != flags) {
            port_state = json_object_get_int(obj_json);

            
            (void)dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
            ret = dal_get_property_value_byte(obj_handle, PROPERTY_SSDP_CONFIG_MODE, &mode);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: get prop mode fail.\n", __FUNCTION__, __LINE__));

            if (SERVICE_STATE_ENABLE == port_state) {
                mode = mode | (SERVICE_STATE_ENABLE << SSDP_RECEIVE_BIT);
            } else {
                
                mode = mode & ((SERVICE_STATE_DISABLE << SSDP_RECEIVE_BIT) + 1);
            }

            
            input_list = g_slist_append(input_list, g_variant_new_byte(mode));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                CLASS_SSDP_CONFIG, METHOD_SSDP_CONFIG_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
                NULL);
            uip_free_gvariant_list(input_list);
            input_list = NULL;

            (void)rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, protocol_json, o_message_jso, message, obj_json);
        }
    }

    flags = 0;
    obj_json = NULL;

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_PORTID, &obj_json) == TRUE) {
        ret = set_data_template(i_paras, o_message_jso, o_result_jso, RFPROP_NETWORKPROTOCOL_PORTID,
            METHOD_SSDP_CONFIG_PORT);
        (void)rf_check_ret(CHECK_PORT, ret, &ret_result, "SSDP/Port", o_message_jso, message, obj_json);
    }

    obj_json = NULL;

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_NMIS, &obj_json) == TRUE) {
        ret = set_data_template(i_paras, o_message_jso, o_result_jso, RFPROP_NETWORKPROTOCOL_NMIS,
            METHOD_SSDP_CONFIG_INTERVAL);
        (void)rf_check_ret(CHECK_RANGE, ret, &ret_result, "SSDP/NotifyMulticastIntervalSeconds", o_message_jso, message,
            obj_json);
    }

    obj_json = NULL;

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_NTTL, &obj_json) == TRUE) {
        ret = set_data_template(i_paras, o_message_jso, o_result_jso, RFPROP_NETWORKPROTOCOL_NTTL,
            METHOD_SSDP_CONFIG_TTL);
        (void)rf_check_ret(CHECK_RANGE, ret, &ret_result, "SSDP/NotifyTTL", o_message_jso, message, obj_json);
    }

    obj_json = NULL;

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_NIPS, &obj_json)) {
        (void)memset_s(protocol_json, sizeof(protocol_json), 0, sizeof(protocol_json));
        // sizeof(protocol_json)大小为100，大于格式化后字符创长度，无需判断返回值
        (void)snprintf_s(protocol_json, sizeof(protocol_json), sizeof(protocol_json) - 1, "%s/%s",
            RFPROP_NETWORKPROTOCOL_SSDP, RFPROP_NETWORKPROTOCOL_NIPS);

        do_val_if_fail(!(json_type_string != json_object_get_type(obj_json)),
            (void)create_message_info(MSGID_PROP_TYPE_ERR, protocol_json, &message,
            json_object_get_type(obj_json) == json_type_null ? RF_VALUE_NULL : dal_json_object_get_str(obj_json),
            protocol_json); flags = 1;
            (void)json_object_array_add(*o_message_jso, message); message = NULL);

        if (0 == flags) {
            
            ret = dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

            scope = dal_json_object_get_str(obj_json);

            
            input_list = g_slist_append(input_list, g_variant_new_string(scope));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                CLASS_SSDP_CONFIG, METHOD_SSDP_CONFIG_SCOPE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
                NULL);
            uip_free_gvariant_list(input_list);

            (void)rf_check_ret(CHECK_RANGE, ret, &ret_result, protocol_json, o_message_jso, message, obj_json);
        }
    }

    
    return ret_result;
}



LOCAL gint32 set_data_template(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gchar *prop, gchar *method)
{
    int iRet;
    gint32 ret;
    gchar protocol_json[100] = {0};
    json_object *obj_json = NULL;
    json_object *message = NULL;
    guint32 data;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;

    ret = json_object_object_get_ex(i_paras->val_jso, prop, &obj_json);
    do_if_fail(ret == TRUE, return VOS_OK); // 此处不要改为HTTP_OK

    iRet = snprintf_s(protocol_json, sizeof(protocol_json), sizeof(protocol_json) - 1, "%s/%s",
        RFPROP_NETWORKPROTOCOL_SSDP, prop);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    do_val_if_fail(!(json_type_int != json_object_get_type(obj_json)),
        (void)create_message_info(MSGID_PROP_TYPE_ERR, protocol_json, &message,
        json_object_get_type(obj_json) == json_type_null ? RF_VALUE_NULL : dal_json_object_get_str(obj_json),
        protocol_json);
        (void)json_object_array_add(*o_message_jso, message); return NETWORK_TYPE_ERR);

    data = json_object_get_int(obj_json);

    
    ret = dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    input_list = g_slist_append(input_list, g_variant_new_uint32(data));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SSDP_CONFIG, method, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret;
}



LOCAL gint32 set_networkprotocol_template(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, guint32 i_id_num, gchar *protocol_name)
{
    int iRet = -1;

    gint32 ret = 0;
    gint32 ret_state;
    gint32 ret_port;
    gint32 ret_result = HTTP_BAD_REQUEST;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    json_object *obj_json = NULL;
    guchar port_state = 0;
    guint32 port_data = 0;
    gchar protocol_json[100] = {0};
    json_object *message = NULL;
    gint32 flags = 0;
    guchar HTTPSEnable = 0;
    guchar board_type = 0;

    
    return_val_do_info_if_fail(o_message_jso && VOS_OK == provider_paras_check(i_paras), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    *o_message_jso = json_object_new_array();

    
    ret_port = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_PORTID, &obj_json);
    if (ret_port) {
        iRet = snprintf_s(protocol_json, sizeof(protocol_json), sizeof(protocol_json) - 1, "%s/%s", protocol_name,
            RFPROP_NETWORKPROTOCOL_PORTID);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        
        
        ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_HTTPS, &HTTPSEnable);
        return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get_httpsenable_status fail.\n", __FUNCTION__, __LINE__));
        ret = redfish_get_board_type(&board_type);
        if ((SERVICE_STATE_ENABLE == HTTPSEnable) && (BOARD_SWITCH == board_type)) {
            do_val_if_expr((SERVICE_CONFIGURATION_WEB_HTTPS != i_id_num), ret_result = HTTP_NOT_IMPLEMENTED;
                (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, protocol_json, &message, protocol_json);
                (void)json_object_array_add(*o_message_jso, message); flags = 1);
        }

        
        do_val_if_fail(!(json_type_int != json_object_get_type(obj_json)),
            (void)create_message_info(MSGID_PROP_TYPE_ERR, protocol_json, &message,
            json_object_get_type(obj_json) == json_type_null ? RF_VALUE_NULL : dal_json_object_get_str(obj_json),
            protocol_json);
            (void)json_object_array_add(*o_message_jso, message); flags = 1);

        if (1 != flags) {
            port_data = json_object_get_int(obj_json);
            if ((port_data < PORT_MIN_VALUE) || (port_data > PORT_MAX_VALUE)) {
                (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, protocol_json, &message,
                    dal_json_object_get_str(obj_json), protocol_json);
                (void)json_object_array_add(*o_message_jso, message);
                obj_json = NULL;
            } else {
                
                ret = dal_get_specific_object_uint32(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_SERVER_ID, i_id_num,
                    &obj_handle);
                return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                    debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

                
                input_list = g_slist_append(input_list, g_variant_new_uint16(port_data));
                ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
                    obj_handle, SERVER_PORT_CLASS_NAME, SERVER_PORT_METHOD_SET_PORT, AUTH_ENABLE,
                    i_paras->user_role_privilege, input_list, NULL);
                uip_free_gvariant_list(input_list);
                input_list = NULL;

                (void)rf_check_ret(CHECK_PORT, ret, &ret_result, protocol_json, o_message_jso, message, obj_json);

                obj_json = NULL;
            }
        }
    }

    flags = 0;
    
    ret_state = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORKPROTOCOL_STATE, &obj_json);
    if (ret_state) {
        
        (void)memset_s(protocol_json, sizeof(protocol_json), 0, sizeof(protocol_json));
        iRet = snprintf_s(protocol_json, sizeof(protocol_json), sizeof(protocol_json) - 1, "%s/%s", protocol_name,
            RFPROP_NETWORKPROTOCOL_STATE);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        
        
        ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_HTTPS, &HTTPSEnable);
        return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get_httpsenable_status fail.\n", __FUNCTION__, __LINE__));
        ret = redfish_get_board_type(&board_type);
        if ((SERVICE_STATE_ENABLE == HTTPSEnable) && (BOARD_SWITCH == board_type)) {
            do_val_if_expr((SERVICE_CONFIGURATION_WEB_HTTPS != i_id_num), ret_result = HTTP_NOT_IMPLEMENTED;
                (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, protocol_json, &message, protocol_json);
                (void)json_object_array_add(*o_message_jso, message); flags = 1);
        }

        

        do_val_if_fail(!(NULL == obj_json),
            (void)create_message_info(MSGID_PROP_TYPE_ERR, protocol_json, &message, RF_VALUE_NULL, protocol_json);
            (void)json_object_array_add(*o_message_jso, message); flags = 1);

        if (1 != flags) {
            port_state = json_object_get_boolean(obj_json);

            
            ret = dal_get_specific_object_uint32(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_SERVER_ID, i_id_num,
                &obj_handle);
            return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
                debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

            
            input_list = g_slist_append(input_list, g_variant_new_byte(port_state));
            ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                SERVER_PORT_CLASS_NAME, SERVER_PORT_METHOD_SET_STATE, AUTH_ENABLE, i_paras->user_role_privilege,
                input_list, NULL);
            uip_free_gvariant_list(input_list);

            (void)rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, protocol_json, o_message_jso, message, obj_json);
        }
    }

    
    return ret_result;
}


LOCAL gint32 _set_vnc_state(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    gint32 vnc_enabled;
    GSList *input_list = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    gint32 ret_result = HTTP_BAD_REQUEST;

    // 内部接口不做入参检测
    vnc_enabled = json_object_get_boolean(prop);

    input_list = g_slist_append(input_list,
        g_variant_new_byte((guchar)vnc_enabled)); // vnc_enabled只可能为0或者1，可以强转为guchar

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, VNC_CLASS_NAME,
        METHOD_VNC_SET_ENABLE_STATE, AUTH_DISABLE, 0, input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    (void)rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, PROPERTY_MANAGER_VNCPORTENABLED, &message_array, message_jso,
        prop);
    return ret_result;
}


LOCAL gint32 _set_vnc_port(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    GSList *input_list = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    gint32 vnc_port;
    gint32 ret_result = HTTP_BAD_REQUEST;

    // 内部接口不做入参检测
    // 小数可能传进来，返回类型错误
    
    if (json_type_int != json_object_get_type(prop)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_VNC_PORT, &message_jso, dal_json_object_get_str(prop),
            ERROR_VNC_PORT);
        (void)json_object_array_add(message_array, message_jso);
        return ret_result;
    }

    
    vnc_port = json_object_get_int(prop);
    input_list = g_slist_append(input_list, g_variant_new_int32(vnc_port));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, VNC_CLASS_NAME,
        METHOD_VNC_SET_PORT, AUTH_DISABLE, 0, input_list, NULL);

    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    do_if_expr((CHECK_VNC_PORT_EXIST_ERR == ret || CHECK_VNC_PORT_RANGE_ERR == ret), (ret = RF_ERROR));
    (void)rf_check_ret(CHECK_PORT, ret, &ret_result, ERROR_VNC_PORT, &message_array, message_jso, prop);
    return ret_result;
}


LOCAL gint32 _set_vnc_info(PROVIDER_PARAS_S *i_paras, json_object *property, json_object *message_array)
{
    gint32 result = HTTP_BAD_REQUEST;
    json_object *prop = NULL;
    guint8 vnc_supported = 0;
    json_object *message_jso = NULL;
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 flags = 0;

    // 内部接口，不做入参检测

    // 产品不支持VNC，返回501
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_VNC, &vnc_supported);
    return_val_do_info_if_fail(1 == vnc_supported, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ERROR_VNC, &message_jso, ERROR_VNC);
        (void)json_object_array_add(message_array, message_jso));

    // 不具有KVM远程控制权限，返回权限不足
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_VNC, &message_jso, ERROR_VNC);
        (void)json_object_array_add(message_array, message_jso));

    // 获取VNC对象句柄
    (void)dal_get_object_handle_nth(VNC_CLASS_NAME, 0, &obj_handle);
    i_paras->obj_handle = obj_handle;

    // 设置vnc使能
    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_STATE, &prop)) {
        
        do_val_if_expr(NULL == prop, (void)create_message_info(MSGID_PROP_TYPE_ERR, PROPERTY_MANAGER_VNCPORTENABLED,
            &message_jso, RF_VALUE_NULL, PROPERTY_MANAGER_VNCPORTENABLED);
            (void)json_object_array_add(message_array, message_jso); flags = 1);

        if (0 == flags) {
            ret = _set_vnc_state(i_paras, prop, message_array);
            do_if_expr(HTTP_OK == ret, (result = HTTP_OK));
            ret = 0;
        }

        
    }

    prop = NULL;

    // 设置vnc端口号
    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_PORTID, &prop)) {
        ret = _set_vnc_port(i_paras, prop, message_array);
        do_if_expr(HTTP_OK == ret, (result = HTTP_OK));
    }

    return result;
}



LOCAL gint32 ipmi_port_check(OBJ_HANDLE rmcp_handle, guint16 *port1, guint16 *port2)
{
    GSList *p_input_list = NULL;
    gint32 ret_val;
    gint32 ret_result = VOS_OK;
    OBJ_HANDLE obj_handle = 0;

    return_val_if_expr((0 == rmcp_handle || NULL == port1 || NULL == port2), IPMI_PORT_ERROR_INTERNAL);
    ret_val = dfl_get_object_handle(SSH_SERVER_PORT_OBJ_NAME, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret_val, IPMI_PORT_ERROR_INTERNAL,
        debug_log(DLOG_INFO, "Can't find object %s. Ret is %d\n", "SSH", ret_val));

    
    p_input_list = g_slist_append(p_input_list, g_variant_new_uint16(*port1));
    p_input_list = g_slist_append(p_input_list, g_variant_new_byte(IPMI_SERVICE_PORT1_INDEX));

    p_input_list = g_slist_append(p_input_list, g_variant_new_uint16(*port2));
    p_input_list = g_slist_append(p_input_list, g_variant_new_byte(IPMI_SERVICE_PORT2_INDEX));

    ret_val = dfl_call_class_method(obj_handle, SERVER_PORT_METHOD_PORT_CHECK, NULL, p_input_list, NULL);
    g_slist_free_full(p_input_list, (GDestroyNotify)g_variant_unref);
    if (VOS_OK == ret_val) {
        return VOS_OK;
    }

    p_input_list = NULL;
    p_input_list = g_slist_append(p_input_list, g_variant_new_uint16(*port1));
    p_input_list = g_slist_append(p_input_list, g_variant_new_byte(IPMI_SERVICE_PORT1_INDEX));
    ret_val = dfl_call_class_method(obj_handle, SERVER_PORT_METHOD_PORT_CHECK, NULL, p_input_list, NULL);

    do_info_if_true(VOS_OK != ret_val, ret_result = IPMI_PORT1_ERROR;
        (void)dal_get_property_value_uint16(rmcp_handle, PROPERTY_RMCP_PORT1, port1););

    g_slist_free_full(p_input_list, (GDestroyNotify)g_variant_unref);
    p_input_list = NULL;
    p_input_list = g_slist_append(p_input_list, g_variant_new_uint16(*port2));
    p_input_list = g_slist_append(p_input_list, g_variant_new_byte(IPMI_SERVICE_PORT2_INDEX));
    ret_val = dfl_call_class_method(obj_handle, SERVER_PORT_METHOD_PORT_CHECK, NULL, p_input_list, NULL);

    do_info_if_true(VOS_OK != ret_val,
        (ret_result == IPMI_PORT1_ERROR) ? (ret_result = IPMI_PORT_ERROR_ALL) : (ret_result = IPMI_PORT2_ERROR);
        (void)dal_get_property_value_uint16(rmcp_handle, PROPERTY_RMCP_PORT2, port2););

    g_slist_free_full(p_input_list, (GDestroyNotify)g_variant_unref);
    return ret_result;
}





LOCAL gint32 _set_ipmi_port(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    GSList *input_list = NULL;
    GSList *caller_info = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    json_object *prop_port1 = NULL;
    json_object *prop_port2 = NULL;
    guint16 ipmi_port1 = 0;
    guint16 ipmi_port2 = 0;
    guint8 port_miss_times = 0;
    guint8 port1_invalid = 0;
    guint8 port2_invalid = 0;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret_result = HTTP_BAD_REQUEST;
    gint32 check_ret;
#define PORT_SAVE_ONLY 2

    // 内部接口不做入参检测
    ret = dal_get_object_handle_nth(RMCP_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, ret,
        debug_log(DLOG_MASS, "%s: get RmcpConfig handle failed.ret=%d", __FUNCTION__, ret));

    if (json_object_object_get_ex(prop, RFPROP_IPMI_PORT1, &prop_port1)) {
        // 小数可能传进来，返回类型错误
        if (json_type_int != json_object_get_type(prop_port1)) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_IPMI_PORT_1, &message_jso,
                dal_json_object_get_str(prop_port1), ERROR_IPMI_PORT_1);
            (void)json_object_array_add(message_array, message_jso);
            (void)dal_get_property_value_uint16(obj_handle, PROPERTY_RMCP_PORT1, &ipmi_port1);
            port1_invalid++;
        } else {
            ipmi_port1 = (guint16)json_object_get_int(prop_port1);
        }
    } else {
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_RMCP_PORT1, &ipmi_port1);
        port_miss_times++;
    }

    if (json_object_object_get_ex(prop, RFPROP_IPMI_PORT2, &prop_port2)) {
        // 小数可能传进来，返回类型错误
        if (json_type_int != json_object_get_type(prop_port2)) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_IPMI_PORT_2, &message_jso,
                dal_json_object_get_str(prop_port2), ERROR_IPMI_PORT_2);
            (void)json_object_array_add(message_array, message_jso);
            (void)dal_get_property_value_uint16(obj_handle, PROPERTY_RMCP_PORT2, &ipmi_port2);
            port2_invalid++;
        } else {
            ipmi_port2 = (guint16)json_object_get_int(prop_port2);
        }
    } else {
        (void)dal_get_property_value_uint16(obj_handle, PROPERTY_RMCP_PORT2, &ipmi_port2);
        port_miss_times++;
    }
    
    if (port1_invalid + port2_invalid + port_miss_times > 1) {
        return ret_result;
    }

    
    check_ret = ipmi_port_check(obj_handle, &ipmi_port1, &ipmi_port2);
    if (VOS_OK != check_ret) {
        caller_info =
            g_slist_append(caller_info, g_variant_new_string(dal_rf_get_op_log_module_name(i_paras->is_from_webui)));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->user_name));
        caller_info = g_slist_append(caller_info, g_variant_new_string(i_paras->client));
        proxy_method_operation_log(caller_info, MODULE_NAME_REDFISH, "Check IPMI LAN service port failed");
        g_slist_free_full(caller_info, (GDestroyNotify)g_variant_unref);
    }
    
    debug_log(DLOG_DEBUG, "%s:port check ret=%d, ipmi_port1=%d, ipmi_port2=%d", __FUNCTION__, check_ret, ipmi_port1,
        ipmi_port2);
    if (VOS_OK != check_ret && port1_invalid + port2_invalid + port_miss_times == 1) {
        return_val_do_info_if_expr(IPMI_PORT1_ERROR == check_ret, ret_result,
            (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, ERROR_IPMI_PORT_1, &message_jso,
            dal_json_object_get_str(prop), ERROR_IPMI_PORT_1);
            (void)json_object_array_add(message_array, message_jso););

        return_val_do_info_if_expr(IPMI_PORT2_ERROR == check_ret, ret_result,
            (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, ERROR_IPMI_PORT_2, &message_jso,
            dal_json_object_get_str(prop), ERROR_IPMI_PORT_2);
            (void)json_object_array_add(message_array, message_jso););
    }

    return_val_do_info_if_expr(IPMI_PORT_ERROR_ALL == check_ret, ret_result,
        (void)create_message_info(MSGID_PORTID_NOT_MODIFIED, ERROR_IPMI_PORT_1, &message_jso,
        dal_json_object_get_str(prop), ERROR_IPMI_PORT_1);
        (void)json_object_array_add(message_array, message_jso); (void)create_message_info(MSGID_PORTID_NOT_MODIFIED,
        ERROR_IPMI_PORT_2, &message_jso, dal_json_object_get_str(prop), ERROR_IPMI_PORT_2);
        (void)json_object_array_add(message_array, message_jso););

    return_val_do_info_if_expr(IPMI_PORT_ERROR_INTERNAL == check_ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(message_array, message_jso););

    input_list = g_slist_append(input_list, g_variant_new_byte(PORT_SAVE_ONLY));
    input_list = g_slist_append(input_list, g_variant_new_uint16(ipmi_port1));
    input_list = g_slist_append(input_list, g_variant_new_uint16(ipmi_port2));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0, RMCP_CLASS_NAME,
        METHOD_SET_RMCP_PORT, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s, uip return = %d.", __FUNCTION__, ret);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    if (VOS_OK == ret) {
        ret_result = HTTP_OK;
        do_info_if_true((IPMI_PORT1_ERROR == check_ret), (void)rf_check_ret(CHECK_PORT, COMP_CODE_OUTOF_RANGE,
            &ret_result, ERROR_IPMI_PORT_1, &message_array, message_jso, prop););
        do_info_if_true((IPMI_PORT2_ERROR == check_ret), (void)rf_check_ret(CHECK_PORT, COMP_CODE_OUTOF_RANGE,
            &ret_result, ERROR_IPMI_PORT_2, &message_array, message_jso, prop););
    } else {
        (void)rf_check_ret(CHECK_PORT, ret, &ret_result, ERROR_IPMI_PORT_1, &message_array, message_jso, prop);
        (void)rf_check_ret(CHECK_PORT, ret, &ret_result, ERROR_IPMI_PORT_2, &message_array, message_jso, prop);
    }

    return ret_result;
}


LOCAL gint32 _set_ipmi_info(PROVIDER_PARAS_S *i_paras, json_object *property, json_object *message_array)
{
    gint32 result = HTTP_BAD_REQUEST;
    json_object *prop = NULL;
    json_object *message = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 ipmi_supported = 0;
    gboolean set_state = FALSE;

    // 内部接口，不做入参检测

    // 产品不支持IPMI，返回501
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_IPMI, &ipmi_supported);
    return_val_do_info_if_fail(1 == ipmi_supported, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ERROR_IPMI, &message, ERROR_IPMI);
        (void)json_object_array_add(message_array, message));

    // 不具有权限，返回权限不足
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_IPMI, &message, ERROR_IPMI);
        (void)json_object_array_add(message_array, message));

    // 获取IPMI 对象句柄
    ret = dal_get_object_handle_nth(RMCP_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_SVC_TEMP_UNAVALIBLE, ERROR_IPMI, &message, ERROR_IPMI);
        (void)json_object_array_add(message_array, message));

    i_paras->obj_handle = obj_handle;

    // 设置IPMI LAN 1.5 使能
    ret = get_networkprotocol_ipmi_state(property, RFPROP_RMCP_ENALBED, ERROR_IPMI_RMCP_ENABLED, &prop, &message_array);
    if (ret == RET_OK) {
        set_state = TRUE;
        ret = set_networkprotocol_ipmi_state(i_paras, obj_handle, METHOD_SET_LAN_STATE, prop);
        rf_check_ret(CHECK_PROTOCOL, ret, &result, ERROR_IPMI_RMCP_ENABLED, &message_array, message, prop);
    }

    // 设置IPMI LAN 2.0 使能
    prop = NULL;
    ret = get_networkprotocol_ipmi_state(property, RFPROP_RMCP_PLUS_ENABLED, ERROR_IPMI_RMCP_PLUS_ENABLED, &prop,
        &message_array);
    if (ret == RET_OK) {
        set_state = TRUE;
        ret = set_networkprotocol_ipmi_state(i_paras, obj_handle, METHOD_SET_LANPLUS_STATE, prop);
        rf_check_ret(CHECK_PROTOCOL, ret, &result, ERROR_IPMI_RMCP_PLUS_ENABLED, &message_array, message, prop);
    }

    // 设置IPMI port1 和port2 端口号
    ret = _set_ipmi_port(i_paras, property, message_array);
    if (ret == HTTP_OK) {
        return HTTP_OK;
    }

    
    if (set_state == TRUE)  {
        set_rmcp_server_state(i_paras->is_from_webui, i_paras->user_name, i_paras->client);
    }

    return result;
}


LOCAL gint32 _set_nat_state(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    json_bool nat_enabled;
    guchar nat_enable = 0;
    GSList *input_list = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    gint32 ret_result = HTTP_BAD_REQUEST;

    // 内部接口不做入参检测
    nat_enabled = json_object_get_boolean(prop);
    do_if_expr((TRUE == nat_enabled), (nat_enable = 1));
    input_list = g_slist_append(input_list, g_variant_new_byte(nat_enable));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, SERVER_PORT_CLASS_NAME, SERVER_PORT_METHOD_SET_STATE, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s, uip return = %d.", __FUNCTION__, ret);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    (void)rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, ERROR_NAT_ENABLED, &message_array, message_jso, prop);
    return ret_result;
}


LOCAL gint32 _set_nat_port(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    GSList *input_list = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    guint16 nat_port;
    gint32 ret_result = HTTP_BAD_REQUEST;

    // 内部接口不做入参检测
    // 小数可能传进来，返回类型错误
    if (json_type_int != json_object_get_type(prop)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_NAT_PORT, &message_jso, dal_json_object_get_str(prop),
            ERROR_NAT_PORT);
        (void)json_object_array_add(message_array, message_jso);
        return ret_result;
    }

    nat_port = (guint16)json_object_get_int(prop);
    input_list = g_slist_append(input_list, g_variant_new_uint16(nat_port));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, SERVER_PORT_CLASS_NAME, SERVER_PORT_METHOD_SET_PORT, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s, uip return = %d.", __FUNCTION__, ret);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    (void)rf_check_ret(CHECK_PORT, ret, &ret_result, ERROR_NAT_PORT, &message_array, message_jso, prop);
    return ret_result;
}


LOCAL gint32 _set_nat_info(PROVIDER_PARAS_S *i_paras, json_object *property, json_object *message_array)
{
    gint32 result = HTTP_BAD_REQUEST;
    json_object *prop = NULL;
    json_object *message_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 flags = 0;

    // 内部接口，不做入参检测

    // 不具有权限，返回权限不足
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_NAT, &message_jso, ERROR_NAT);
        (void)json_object_array_add(message_array, message_jso));

    // 获取NAT 对象句柄
    ret = dal_get_specific_object_uint32(SERVER_PORT_CLASS_NAME, SERVER_PORT_ATTRIBUTE_SERVER_ID,
        SERVICE_CONFIGURATION_HMMSSHNAT, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_SVC_TEMP_UNAVALIBLE, ERROR_NAT, &message_jso, ERROR_NAT);
        (void)json_object_array_add(message_array, message_jso));

    i_paras->obj_handle = obj_handle;

    // 设置NAT 使能
    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_STATE, &prop)) {
        do_val_if_expr(NULL == prop, (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_NAT_ENABLED, &message_jso,
            RF_VALUE_NULL, ERROR_NAT_ENABLED);
            (void)json_object_array_add(message_array, message_jso); flags = 1);

        if (0 == flags) {
            ret = _set_nat_state(i_paras, prop, message_array);
            do_if_expr(HTTP_OK == ret, (result = HTTP_OK));
            ret = 0;
        }
    }

    prop = NULL;

    // 设置NAT 端口号
    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_PORTID, &prop)) {
        ret = _set_nat_port(i_paras, prop, message_array);
        do_if_expr(HTTP_OK == ret, (result = HTTP_OK));
    }

    return result;
}


LOCAL gint32 _set_video_state(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    json_bool video_enabled;
    guchar video_enable = 0;
    GSList *input_list = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    gint32 ret_result = HTTP_BAD_REQUEST;

    // 内部接口不做入参检测
    video_enabled = json_object_get_boolean(prop);
    do_if_expr((TRUE == video_enabled), (video_enable = 1));
    input_list = g_slist_append(input_list, g_variant_new_byte(video_enable));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        PROXY_VIDEO_CLASS_NAME, VIDEO_METHOD_SET_ENABLE, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s, uip return = %d.", __FUNCTION__, ret);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    (void)rf_check_ret(CHECK_PROTOCOL, ret, &ret_result, ERROR_VIDEO_ENABLED, &message_array, message_jso, prop);
    return ret_result;
}


LOCAL gint32 _set_video_port(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    GSList *input_list = NULL;
    gint32 ret;
    json_object *message_jso = NULL;
    gint32 video_port;
    gint32 ret_result = HTTP_BAD_REQUEST;

    // 内部接口不做入参检测
    // 小数可能传进来，返回类型错误
    if (json_type_int != json_object_get_type(prop)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_VIDEO_PORT, &message_jso, dal_json_object_get_str(prop),
            ERROR_VIDEO_PORT);
        (void)json_object_array_add(message_array, message_jso);
        return ret_result;
    }

    video_port = json_object_get_int(prop);
    input_list = g_slist_append(input_list, g_variant_new_int32(video_port));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        PROXY_VIDEO_CLASS_NAME, VIDEO_METHOD_SET_PORT, AUTH_DISABLE, 0, input_list, NULL);
    debug_log(DLOG_DEBUG, "%s, uip return = %d.", __FUNCTION__, ret);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    do_if_expr((VOS_ERR == ret), (ret = RF_ERROR));
    (void)rf_check_ret(CHECK_PORT, ret, &ret_result, ERROR_VIDEO_PORT, &message_array, message_jso, prop);
    return ret_result;
}


LOCAL gint32 _set_video_info(PROVIDER_PARAS_S *i_paras, json_object *property, json_object *message_array)
{
    gint32 result = HTTP_BAD_REQUEST;
    json_object *prop = NULL;
    json_object *message_jso = NULL;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gint32 flags = 0;

    // 内部接口，不做入参检测

    // 不具有权限，返回权限不足
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_SECURITYMGNT, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_VIDEO, &message_jso, ERROR_VIDEO);
        (void)json_object_array_add(message_array, message_jso));

    // 获取Video对象句柄
    ret = dal_get_object_handle_nth(VIDEO_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_SVC_TEMP_UNAVALIBLE, ERROR_VIDEO, &message_jso, ERROR_VIDEO);
        (void)json_object_array_add(message_array, message_jso));

    i_paras->obj_handle = obj_handle;

    // 设置video使能
    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_STATE, &prop)) {
        do_val_if_expr(NULL == prop, (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_VIDEO_ENABLED, &message_jso,
            RF_VALUE_NULL, ERROR_VIDEO_ENABLED);
            (void)json_object_array_add(message_array, message_jso); flags = 1);

        if (0 == flags) {
            ret = _set_video_state(i_paras, prop, message_array);
            do_if_expr(HTTP_OK == ret, (result = HTTP_OK));
            ret = 0;
        }
    }

    prop = NULL;

    // 设置video端口号
    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_PORTID, &prop)) {
        ret = _set_video_port(i_paras, prop, message_array);
        do_if_expr(HTTP_OK == ret, (result = HTTP_OK));
    }

    return result;
}



LOCAL gint32 _set_ssdp_notify_enabled(PROVIDER_PARAS_S *i_paras, json_object *prop, json_object *message_array)
{
    gint32 ret_result = HTTP_BAD_REQUEST;
    gint32 ret;
    json_object *message_jso = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint32 interval = 0;
    guchar notify_enabled;
    guchar mode = 0;
    GSList *input_list = NULL;
    // 内部接口，不做入参检测

    return_val_do_info_if_expr(NULL == prop, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_TYPE_ERR, ERROR_SSDP_NOTIFY_ENABLED, &message_jso, RF_VALUE_NULL,
        ERROR_SSDP_NOTIFY_ENABLED);
        (void)json_object_array_add(message_array, message_jso));
    notify_enabled = json_object_get_boolean(prop);

    (void)dal_get_object_handle_nth(CLASS_SSDP_CONFIG, 0, &obj_handle);
    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_SSDP_CONFIG_INTERVAL, &interval);
    return_val_do_info_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get interval fail, ret is %d.", __FUNCTION__, ret));

    return_val_do_info_if_expr(interval == 0 && notify_enabled == TRUE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_MULTICAST_INTERVAL_IS_ZERO, NULL, &message_jso);
        (void)json_object_array_add(message_array, message_jso));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SSDP_CONFIG_MODE, &mode);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get prop mode fail.\n", __FUNCTION__, __LINE__));

    if (SERVICE_STATE_ENABLE == notify_enabled) {
        mode = mode | (SERVICE_STATE_ENABLE << SSDP_SEND_BIT);
    } else {
        
        mode = mode & ((SERVICE_STATE_DISABLE << SSDP_SEND_BIT) + 2);
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(mode));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SSDP_CONFIG, METHOD_SSDP_CONFIG_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    (void)rf_check_ret(CHECK_RANGE, ret, &ret_result, ERROR_SSDP_NOTIFY_ENABLED, &message_array, message_jso, prop);

    return ret_result;
}


LOCAL gint32 _set_ssdp_info(PROVIDER_PARAS_S *i_paras, json_object *property, json_object *message_array)
{
    gint32 ret_result = HTTP_BAD_REQUEST;
    json_object *prop = NULL;
    // 内部接口，不做入参检测

    if (json_object_object_get_ex(property, RFPROP_NETWORKPROTOCOL_NOTIFY_ENABLED, &prop)) {
        ret_result = _set_ssdp_notify_enabled(i_paras, prop, message_array);
    }

    return ret_result;
}


LOCAL gint32 set_networkprotocol_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    json_object *huawei = NULL;
    json_object *property = NULL;
    gint32 result = HTTP_BAD_REQUEST;
    
    gint32 ret = 0;
    

    return_val_do_info_if_expr((provider_paras_check(i_paras) != VOS_OK) || (o_message_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s : NULL pointer", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei);
    
    return_val_if_expr((0 == g_strcmp0(dal_json_object_to_json_string(i_paras->val_jso), JSON_NULL_OBJECT_STR)) ||
        (0 == g_strcmp0(dal_json_object_to_json_string(huawei), JSON_NULL_OBJECT_STR)),
        HTTP_BAD_REQUEST);

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_message_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s : alloc message array mem fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 设置VNC信息
    if (json_object_object_get_ex(huawei, RFPROP_NETWORKPROTOCOL_VNC, &property)) {
        ret = _set_vnc_info(i_paras, property, *o_message_jso);
        do_if_expr((HTTP_OK == ret), (result = ret));
    }

    
    // 设置Video信息
    if (json_object_object_get_ex(huawei, RFPROP_NETWORKPROTOCOL_VIDEO, &property)) {
        ret = _set_video_info(i_paras, property, *o_message_jso);
        do_if_expr((HTTP_OK == ret), (result = ret));
        debug_log(DLOG_DEBUG, "%s : set video ret = %d", __FUNCTION__, ret);
    }

    // 设置NAT 信息
    if (json_object_object_get_ex(huawei, RFPROP_NETWORKPROTOCOL_NAT, &property)) {
        ret = _set_nat_info(i_paras, property, *o_message_jso);
        do_if_expr((HTTP_OK == ret), (result = ret));
        debug_log(DLOG_DEBUG, "%s : set nat ret = %d", __FUNCTION__, ret);
    }

    // 设置IPMI 信息
    if (json_object_object_get_ex(huawei, RFPROP_NETWORKPROTOCOL_IPMI, &property)) {
        ret = _set_ipmi_info(i_paras, property, *o_message_jso);
        do_if_expr((HTTP_OK == ret), (result = ret));
        debug_log(DLOG_DEBUG, "%s : set ipmi ret = %d", __FUNCTION__, ret);
    }

    

    
    // 设置SSDP 信息
    if (json_object_object_get_ex(huawei, RFPROP_NETWORKPROTOCOL_SSDP, &property)) {
        ret = _set_ssdp_info(i_paras, property, *o_message_jso);
        do_if_expr((HTTP_OK == ret), (result = ret));
        debug_log(DLOG_DEBUG, "%s : set ssdp ret = %d", __FUNCTION__, ret);
    }
    

    return result;
}


gint32 networkprotocol_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gint32 ret;
    guchar NetConfigEnable = 0;

    
    return_val_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count), HTTP_INTERNAL_SERVER_ERROR);

    
    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_NETCONFIG, &NetConfigEnable);
    return_val_do_info_if_expr((VOS_ERR == ret), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s, %d: get_NetConfigEnable_status fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail((SERVICE_NETCONFIG_ENABLE == NetConfigEnable), HTTP_NOT_FOUND);
    

    ret = redfish_check_manager_uri_valid(i_paras->uri); // 校验用户输入的URI是否有效
    if (TRUE == ret) {
        *prop_provider = g_networkprotocol_provider;
        *count = sizeof(g_networkprotocol_provider) / sizeof(PROPERTY_PROVIDER_S);
        return VOS_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}


LOCAL gint32 get_networkprotocol_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;

    gint32 ret;
    gchar change_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    iRet = snprintf_s(change_uri, sizeof(change_uri), sizeof(change_uri) - 1, RFPROP_MANAGER_NETPRO, slot_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string(change_uri);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 set_rmcp_server_state(guchar from_webui_flag, const gchar *username, const gchar *ip)
{
    gint32 ret;
    guint8 state;
    guint8 state2;
    guint32 port;
    guint32 port2;
    OBJ_HANDLE rmcp_obj_handle = 0;
    GSList *input_list = NULL;
    GSList *property_name_list = NULL;
    GSList *property_value_list = NULL;

    return_val_if_expr(NULL == username || NULL == ip, VOS_ERR);
    if (VOS_OK != dfl_get_object_handle(RMCP_OBJ_NAME, &rmcp_obj_handle)) {
        debug_log(DLOG_INFO, "%s:Get rmcp object failed.", __FUNCTION__);
        return VOS_ERR;
    }

    property_name_list = g_slist_append(property_name_list, PROPERTY_LAN_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_LAN_PLUS_STATE);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_PORT1);
    property_name_list = g_slist_append(property_name_list, PROPERTY_RMCP_PORT2);
    ret = dfl_multiget_property_value(rmcp_obj_handle, property_name_list, &property_value_list);

    g_slist_free(property_name_list);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s:Get rmcp property value failed:%d.", __FUNCTION__, ret);
        return ret;
    }

    state = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 0));
    state2 = g_variant_get_byte((GVariant *)g_slist_nth_data(property_value_list, 1));
    port = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 2));
    port2 = g_variant_get_uint16((GVariant *)g_slist_nth_data(property_value_list, 3));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(state | state2));
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port));
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)port2));

    
    ret = uip_call_class_method_redfish(from_webui_flag, username, ip, 0, RMCP_CLASS_NAME, METHOD_SET_RMCP_PORT, 0,
        AUTH_DISABLE, input_list, NULL);
    if (VOS_OK != ret) {
        debug_log(DLOG_INFO, "%s:uip returned:%d.", __FUNCTION__, ret);
    }
    

    g_slist_free_full(property_value_list, (GDestroyNotify)g_variant_unref);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    return ret;
}
