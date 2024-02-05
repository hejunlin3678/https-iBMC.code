

#include "redfish_provider.h"
#include "redfish_util.h"
#include "redfish_provider_cert_update.h"

#define RFACTION_CMP_CONFIG                    "CMPConfig"
#define RF_CMPCFG_CASERVER_ADDR                "CAServerAddr"
#define RF_CMPCFG_CASERVER_CMPPATH             "CAServerCMPPath"
#define RF_CMPCFG_CASERVER_PORT                "CAServerPort"
#define RF_CMPCFG_TLSAUTHTYPE                  "TLSAuthType"
#define RF_CMPCFG_AUTOUPDATE_ENABLED           "AutoUpdateEnabled"
#define RF_CMPCFG_CMPCFG_SUBJECTINFO           "SubjectInfo"
#define RF_CMPCFG_COUNTRY                      "Country"
#define RF_CMPCFG_STATE                        "State"
#define RF_CMPCFG_LOCATION                     "Location"
#define RF_CMPCFG_ORGNAME                      "OrgName"
#define RF_CMPCFG_ORGUNIT                      "OrgUnit"
#define RF_CMPCFG_COMMONNAME                   "CommonName"
#define RF_CMPCFG_INTERNALNAME                 "InternalName"
#define RF_CMPCFG_EMAIL                        "Email"

#define RF_CMPCFG_COUNTRY_WHOLE                  "CMPConfig/SubjectInfo/Country"
#define RF_CMPCFG_COMMONNAME_WHOLE               "CMPConfig/SubjectInfo/CommonName"

#define RF_CMPCFG_COUNTRY_REGEX "^[A-Za-z]{2}$"
#define RF_CMPCFG_COMMONNAME_REGEX "^[0-9a-zA-Z_ \\-\\.]{1,64}$"


#define PROPERTY_CERT_UPDATE_SERVICE_CA_CERT_PATH "/tmp/ca_server_ca.pem"

#define PROPERTY_CERT_UPDATE_SERVICE_CRL_PATH "/tmp/ca_server_ca.crl"

#define PROPERTY_CERT_UPDATE_SERVICE_CLIENT_CERT_PATH "/tmp/ca_server_client_cert.pfx"

#define IMPORT_CA_SERVER_CRT_TASK_NAME "remote ca server ca cert import"
#define IMPORT_CA_SERVER_CRL_TASK_NAME "remote ca server crl import"
#define IMPORT_CA_SERVER_CLIENT_TASK_NAME "remote ca server client cert import"
#define IMPORT_CA_SERVER_CA_CERT_TRANSFER_FILEINFO_CODE 0x37
#define IMPORT_CA_SERVER_CRL_TRANSFER_FILEINFO_CODE 0x38
#define IMPORT_CA_SERVER_CLIENT_CERT_TRANSFER_FILEINFO_CODE 0x39
#define RFACTION_CERT_UPDATE_SERVICE_IMPORT_CA_CERT "CertUpdateService.ImportCA"
#define RFACTION_CERT_UPDATE_SERVICE_IMPORT_CRL "CertUpdateService.ImportCrl"
#define RFACTION_CERT_UPDATE_SERVICE_DELETE_CRL "CertUpdateService.DeleteCrl"
#define RFACTION_CERT_UPDATE_SERVICE_IMPORT_CLIENT_CERT "CertUpdateService.ImportClientCert"
#define CA_SERVER_CA_CERT_CHAIN_IFNO "CACertChainInfo"
#define CA_SERVER_CLIENT_CERT_CHAIN_IFNO "ClientCertChainInfo"
#define RPPROP_CLIENT_CERT "ClientCert"
#define CERT_PASSWORD_MIN_LEN_STR "0"
#define CERT_PASSWORD_MAX_LEN_STR "32"

#define RFACTION_UPDATE_CERT_ACTION "CertUpdateService.UpdateCertFromCA"

#define MSGID_CERT_UPDATE_FAILED "CertUpdateFailed"
typedef struct {
    const gchar *prop_name;
    gint32 (*set_prop)(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso);
    gint32 (*get_prop)(OBJ_HANDLE obj_handle, json_object **o_result_jso);
} CMP_CONFIG_HANDLE_METHOD;

LOCAL gint32 get_cert_updateservice_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_cert_update_cmp_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 set_cert_update_cmp_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 set_cmpcfg_caserver_addr(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso);
LOCAL gint32 get_cmpcfg_caserver_addr(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 set_cmpcfg_caserver_port(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso);
LOCAL gint32 get_cmpcfg_caserver_port(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 set_cmpcfg_caserver_cmppath(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso);
LOCAL gint32 get_cmpcfg_caserver_cmppath(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 set_cmpcfg_tlsauthtype(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso);
LOCAL gint32 get_cmpcfg_tlsauthtype(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 set_cmpcfg_auto_update_state(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso);
LOCAL gint32 get_cmpcfg_auto_update_state(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 set_cmpcfg_subjectinfo(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle,
    json_object **o_message_jso);
LOCAL gint32 get_cmpcfg_subjectinfo(OBJ_HANDLE obj_handle, json_object **o_result_jso);
LOCAL gint32 get_cert_update_srv_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_cert_update_srv_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_cert_update_svc_import_ca_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_cert_update_svc_ca_cert_info(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 act_cert_update_svc_import_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_cert_update_svc_delete_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_cert_update_svc_import_client_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_cert_update_svc_client_cert_info(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 trigger_cert_update_entry(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);


LOCAL PROPERTY_PROVIDER_S g_cert_update_service_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_cert_updateservice_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
	{
        RFPROP_ACTIONS,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_cert_update_srv_actions,
        NULL,
        NULL,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_CMP_CONFIG,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_FORBID,
        get_cert_update_cmp_config,
        set_cert_update_cmp_config,
        NULL,
        ETAG_FLAG_ENABLE
    },
	{
        CA_SERVER_CA_CERT_CHAIN_IFNO,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_cert_update_svc_ca_cert_info,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        CA_SERVER_CLIENT_CERT_CHAIN_IFNO,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_cert_update_svc_client_cert_info,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    },
    {
        RFACTION_CERT_UPDATE_SERVICE_IMPORT_CA_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_cert_update_svc_import_ca_cert_entry,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_CERT_UPDATE_SERVICE_IMPORT_CRL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_cert_update_svc_import_crl_entry,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_CERT_UPDATE_SERVICE_DELETE_CRL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_cert_update_svc_delete_crl,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_CERT_UPDATE_SERVICE_IMPORT_CLIENT_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        act_cert_update_svc_import_client_cert_entry,
        ETAG_FLAG_DISABLE
    },
    {
        RFACTION_UPDATE_CERT_ACTION,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT,
        SYS_LOCKDOWN_FORBID,
        NULL,
        NULL,
        trigger_cert_update_entry,
        ETAG_FLAG_DISABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_cert_update_svc_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL,
        MAP_PROPERTY_NULL,
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_cert_update_srv_actioninfo_odata_id,
        NULL, NULL,
        ETAG_FLAG_ENABLE
    }
};

LOCAL const gchar* g_cert_update_svc_action_array[] = {
    RFACTION_CERT_UPDATE_SERVICE_IMPORT_CA_CERT,
    RFACTION_CERT_UPDATE_SERVICE_IMPORT_CRL,
    RFACTION_CERT_UPDATE_SERVICE_DELETE_CRL,
    RFACTION_CERT_UPDATE_SERVICE_IMPORT_CLIENT_CERT,
    RFACTION_UPDATE_CERT_ACTION
};

// 证书更新配置属性设置和查询处理函数
LOCAL const CMP_CONFIG_HANDLE_METHOD g_cmp_config_method[] = {
    {RF_CMPCFG_CASERVER_ADDR, set_cmpcfg_caserver_addr, get_cmpcfg_caserver_addr},
    {RF_CMPCFG_CASERVER_PORT, set_cmpcfg_caserver_port, get_cmpcfg_caserver_port},
    {RF_CMPCFG_CASERVER_CMPPATH, set_cmpcfg_caserver_cmppath, get_cmpcfg_caserver_cmppath},
    {RF_CMPCFG_TLSAUTHTYPE, set_cmpcfg_tlsauthtype, get_cmpcfg_tlsauthtype},
    {RF_CMPCFG_AUTOUPDATE_ENABLED, set_cmpcfg_auto_update_state, get_cmpcfg_auto_update_state},
    {RF_CMPCFG_CMPCFG_SUBJECTINFO, set_cmpcfg_subjectinfo, get_cmpcfg_subjectinfo},
};


LOCAL gint32 set_cmpcfg_caserver_addr(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    const gchar *caserver_addr = NULL;
    GSList *input_list = NULL;

    (void)get_element_str(i_paras->val_jso, RF_CMPCFG_CASERVER_ADDR, &caserver_addr);
    gint32 ret = dal_check_net_addr_valid(caserver_addr, NULL);
    if (ret != NET_ADDR_OK) {
        debug_log(DLOG_ERROR, "%s: CA server address is invalid.", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_CASERVER_ADDR,
            o_message_jso, caserver_addr, RFACTION_CMP_CONFIG "/" RF_CMPCFG_CASERVER_ADDR);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(caserver_addr));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_SET_CMPCFG_CASERVER_ADDR, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value(%s) failed. ret:%d", __FUNCTION__, RF_CMPCFG_CASERVER_ADDR, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_CASERVER_ADDR, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_cmpcfg_caserver_port(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    gint32 port = 0;
    GSList *input_list = NULL;

    (void)get_element_int(i_paras->val_jso, RF_CMPCFG_CASERVER_PORT, &port);
    input_list = g_slist_append(input_list, g_variant_new_uint32((guint32)port));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_SET_CMPCFG_CASERVER_PORT, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value(%s) failed. ret:%d", __FUNCTION__, RF_CMPCFG_CASERVER_PORT, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_CASERVER_PORT, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_cmpcfg_caserver_cmppath(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    const gchar *caserver_cmppath = NULL;
    GSList *input_list = NULL;

    (void)get_element_str(i_paras->val_jso, RF_CMPCFG_CASERVER_CMPPATH, &caserver_cmppath);

    input_list = g_slist_append(input_list, g_variant_new_string(caserver_cmppath));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_SET_CMPCFG_CASERVER_CMPPATH, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value(%s) failed. ret:%d", __FUNCTION__,
            RF_CMPCFG_CASERVER_CMPPATH, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_CASERVER_CMPPATH,
            o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_cmpcfg_tlsauthtype(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    const gchar *auth_type = 0;
    GSList *input_list = NULL;
    guint8 tls_authtype = 0;

    (void)get_element_str(i_paras->val_jso, RF_CMPCFG_TLSAUTHTYPE, &auth_type);

    // CA服务器认证方式：单向:1、双向:2
    if (g_strcmp0(auth_type, "one-way") == 0) {
        tls_authtype = 1;
    } else {
        tls_authtype = 2;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(tls_authtype));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_SET_CMPCFG_TLS_AUTHTYPE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value(%s) failed. ret:%d", __FUNCTION__, RF_CMPCFG_TLSAUTHTYPE, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_TLSAUTHTYPE, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 set_cmpcfg_auto_update_state(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    json_bool auto_update_state = 0;
    GSList *input_list = NULL;

    (void)get_element_boolean(i_paras->val_jso, RF_CMPCFG_AUTOUPDATE_ENABLED, &auto_update_state);

    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)auto_update_state));
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_SET_CMPCFG_AUTOUPDATE_ENABLED, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value(%s) failed. ret:%d", __FUNCTION__, RF_CMPCFG_AUTOUPDATE_ENABLED, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_AUTOUPDATE_ENABLED, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}

LOCAL void get_cmpcfg_method_inputlist(json_object *subject_jso, OBJ_HANDLE obj_handle, GSList **input_list)
{
    const gchar *property_val = NULL;
    gchar history_property[PROPERTY_LEN] = {0};

    const gchar* property_map[] = {RF_CMPCFG_COUNTRY, RF_CMPCFG_STATE, RF_CMPCFG_LOCATION, RF_CMPCFG_ORGNAME,
        RF_CMPCFG_ORGUNIT, RF_CMPCFG_COMMONNAME, RF_CMPCFG_INTERNALNAME, RF_CMPCFG_EMAIL};

    for (gsize i = 0; i < G_N_ELEMENTS(property_map); i++) {
        (void)get_element_str(subject_jso, property_map[i], &property_val);

        if (property_val == NULL) {
            (void)dal_get_property_value_string(obj_handle, property_map[i], history_property,
                PROPERTY_LEN);
            *input_list = g_slist_append(*input_list, g_variant_new_string(history_property));
        } else {
            *input_list = g_slist_append(*input_list, g_variant_new_string(property_val));
        }

        property_val = NULL;
    }
}

LOCAL gint32 check_required_property_valid(json_object *subject_jso, json_object **o_message_jso)
{
    const gchar *country = NULL;
    const gchar *common_name = NULL;

    // country和common_name是必填项，获取不到就认为异常
    json_bool is_ok = get_element_str(subject_jso, RF_CMPCFG_COUNTRY, &country);
    if (is_ok != TRUE) {
        (void)create_message_info(MSGID_PROP_MISSING, RF_CMPCFG_COUNTRY_WHOLE, o_message_jso,
            RF_CMPCFG_COUNTRY_WHOLE);
        return HTTP_BAD_REQUEST;
    }

    is_ok = get_element_str(subject_jso, RF_CMPCFG_COMMONNAME, &common_name);
    if (is_ok != TRUE) {
        (void)create_message_info(MSGID_PROP_MISSING, RF_CMPCFG_COMMONNAME_WHOLE, o_message_jso,
            RF_CMPCFG_COMMONNAME_WHOLE);
        return HTTP_BAD_REQUEST;
    }

    if (g_regex_match_simple(RF_CMPCFG_COUNTRY_REGEX, country, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RF_CMPCFG_COUNTRY_WHOLE, o_message_jso,
            country, RF_CMPCFG_COUNTRY_WHOLE);
        return HTTP_BAD_REQUEST;
    }

    if (g_regex_match_simple(RF_CMPCFG_COMMONNAME_REGEX, common_name, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0) != TRUE) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RF_CMPCFG_COMMONNAME_WHOLE, o_message_jso,
                common_name, RF_CMPCFG_COMMONNAME_WHOLE);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}


LOCAL gint32 set_cmpcfg_subjectinfo(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object **o_message_jso)
{
    GSList *input_list = NULL;
    json_object *subject_jso = NULL;
    (void)json_object_object_get_ex(i_paras->val_jso, RF_CMPCFG_CMPCFG_SUBJECTINFO, &subject_jso);

    gint32 ret = check_required_property_valid(subject_jso, o_message_jso);
    if (ret != HTTP_OK) {
        debug_log(DLOG_ERROR, "%s: required property is invalid.", __FUNCTION__);
        return ret;
    }

    get_cmpcfg_method_inputlist(subject_jso, obj_handle, &input_list);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_SET_CMPCFG_SUBJECTINFO, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property value(%s) failed. ret:%d", __FUNCTION__, RF_CMPCFG_CMPCFG_SUBJECTINFO, ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, RFACTION_CMP_CONFIG "/" RF_CMPCFG_CMPCFG_SUBJECTINFO, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_cmpcfg_caserver_addr(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    gchar ca_addr[PROPERTY_LEN] = {0};

    gint32 ret = dal_get_property_value_string(obj_handle, RF_CMPCFG_CASERVER_ADDR, ca_addr, PROPERTY_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get ca_addr failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_string((const gchar *)ca_addr);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "%s: create the json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_cmpcfg_caserver_port(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    guint32 port = 0;

    gint32 ret = dal_get_property_value_uint32(obj_handle, RF_CMPCFG_CASERVER_PORT, &port);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get port failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_int((gint32)port);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "%s: create the json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_cmpcfg_caserver_cmppath(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    gchar cmp_path[PROPERTY_LEN] = {0};

    gint32 ret = dal_get_property_value_string(obj_handle, RF_CMPCFG_CASERVER_CMPPATH, cmp_path, PROPERTY_LEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get CMP path failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_string((const gchar *)cmp_path);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "%s: create the json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_cmpcfg_tlsauthtype(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    guint8 tls_authtype = 0;

    gint32 ret = dal_get_property_value_byte(obj_handle, RF_CMPCFG_TLSAUTHTYPE, &tls_authtype);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get auth_type failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    // CA服务器认证方式：单向:1、双向:2
    if (tls_authtype == 1) {
        *o_result_jso = json_object_new_string("one-way");
    } else {
        *o_result_jso = json_object_new_string("two-way");
    }

    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "%s: create the json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_cmpcfg_auto_update_state(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    guint8 auto_update_status = 0;

    gint32 ret = dal_get_property_value_byte(obj_handle, RF_CMPCFG_AUTOUPDATE_ENABLED, &auto_update_status);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get auto update status failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    *o_result_jso = json_object_new_boolean(auto_update_status);
    if ((*o_result_jso) == NULL) {
        debug_log(DLOG_ERROR, "%s: create the json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_cmpcfg_subjectinfo(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: create json object failed.", __FUNCTION__);
        return RET_ERR;
    }

    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_COUNTRY, RF_CMPCFG_COUNTRY, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_STATE, RF_CMPCFG_STATE, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_LOCATION, RF_CMPCFG_LOCATION, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_ORGNAME, RF_CMPCFG_ORGNAME, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_ORGUNIT, RF_CMPCFG_ORGUNIT, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_COMMONNAME, RF_CMPCFG_COMMONNAME, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_INTERALNAME, RF_CMPCFG_INTERNALNAME, *o_result_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CMPCFG_EMAIL, RF_CMPCFG_EMAIL, *o_result_jso);

    return RET_OK;
}

LOCAL gint32 set_cmp_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso)
{
    gint32 ret;
    json_object *prop_jso = NULL;
    json_object *message = NULL;
    gint32 result = RET_ERR;
    OBJ_HANDLE obj_handle = 0;

    ret = dal_get_object_handle_nth(CLASS_NAME_CMP_CONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    *o_message_jso = json_object_new_array();
    for (guint32 prop_index = 0; prop_index < G_N_ELEMENTS(g_cmp_config_method); prop_index++) {
        if (json_object_object_get_ex(i_paras->val_jso, g_cmp_config_method[prop_index].prop_name,
            &prop_jso) != TRUE) {
            continue;
        }

        if (g_cmp_config_method[prop_index].set_prop == NULL) {
            debug_log(DLOG_DEBUG, "%s: the function pointer is null. (property: %s)", __FUNCTION__,
                g_cmp_config_method[prop_index].prop_name);
            continue;
        }

        ret = g_cmp_config_method[prop_index].set_prop(i_paras, obj_handle, &message);
        if (ret != HTTP_OK) {
            (void)json_object_array_add(*o_message_jso, message);
            continue;
        }

        // patch操作有一个设置ok，整体返回ok
        result = RET_OK;
    }

    return result;
}

LOCAL gint32 set_cert_update_cmp_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret = HTTP_BAD_REQUEST;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((USERROLE_SECURITYMGNT & (i_paras->user_role_privilege)) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, RFACTION_CMP_CONFIG, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = set_cmp_config(i_paras, o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set property(CMPConfig) failed. (ret:%d)", __FUNCTION__, ret);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}

LOCAL gint32 get_cmp_config_prop(PROVIDER_PARAS_S *i_paras, json_object *cmp_json)
{
    guint32 prop_index;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;

    gint32 ret = dal_get_object_handle_nth(CLASS_NAME_CMP_CONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed (ret:%d)", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (prop_index = 0; prop_index < G_N_ELEMENTS(g_cmp_config_method); prop_index++) {
        if (g_cmp_config_method[prop_index].get_prop == NULL) {
            debug_log(DLOG_DEBUG, "%s: the function pointer is null. (property: %s)", __FUNCTION__,
                g_cmp_config_method[prop_index].prop_name);
            continue;
        }

        ret = g_cmp_config_method[prop_index].get_prop(obj_handle, &val_jso);
        if (ret != RET_OK) {
            json_object_object_add(cmp_json, g_cmp_config_method[prop_index].prop_name, NULL);
        } else {
            json_object_object_add(cmp_json, g_cmp_config_method[prop_index].prop_name, val_jso);
        }

        val_jso = NULL;
    }

    return RET_OK;
}


LOCAL gint32 get_cert_update_cmp_config(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    json_object *cmp_jso = json_object_new_object();
    if (cmp_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: create json object failed.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = get_cmp_config_prop(i_paras, cmp_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "cert update service get properties failed.");
        json_object_put(cmp_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = cmp_jso;
    return HTTP_OK;
}


LOCAL gint32 trigger_cert_update_entry(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    if ((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_CMP_CONFIG, 0, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret(%d).", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_CMP_CONFIG, METHOD_ACT_CERT_UPDATE, AUTH_DISABLE, i_paras->user_role_privilege, NULL, NULL);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update cert failed. ret:%d", __FUNCTION__, ret);
        (void)create_message_info(MSGID_CERT_UPDATE_FAILED, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }

    return HTTP_OK;
}


LOCAL gint32 process_import_local_ca_cert(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    
    if (strlen(file_path) >= MAX_FILEPATH_LENGTH) {
        debug_log(DLOG_ERROR, "%s: file path length is over %d or file is oversized", __FUNCTION__,
            MAX_FILEPATH_LENGTH);
        ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        goto exit;
    }

    if (vos_get_file_length(file_path) > CERT_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: file is oversized", __FUNCTION__);
        ret = IMPORT_CERT_FILE_LEN_EXCEED_ERR;
        goto exit;
    }

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);

    
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));

    ret = uip_call_class_method_redfish((guint8)from_webui_flag, user_name, client, 0, CLASS_CA_SERVER_CACERT,
        CA_SERVER_CACERT_METHOD_IMPORT_CACERT, AUTH_DISABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);

    ret = parse_import_ca_cert_result(ret, o_message_jso);
exit:
    if (ret != HTTP_OK) {
        (void)proxy_delete_file(0, NULL, NULL, file_path);
    }
    return ret;
}


LOCAL gint32 ca_cert_remote_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;

    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_ca_cert, monitor_fn_data,
        message_obj);
    return ret;
}


LOCAL gint32 process_import_remote_ca_cert(PROVIDER_PARAS_S *i_paras, const gchar *remote_uri,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *cert_import_data = NULL;
    json_object *body_jso = NULL;

    ret =
        rf_start_file_download(i_paras, remote_uri, IMPORT_CA_SERVER_CA_CERT_TRANSFER_FILEINFO_CODE, o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: start file download fail", __FUNCTION__);
        return ret;
    }

    cert_import_data = cert_import_info_new(i_paras, import_cert_set_common_userdata_fn);
    if (cert_import_data == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    cert_import_data->task_progress = TASK_NO_PROGRESS; 
    cert_import_data->rsc_privilege = USERROLE_SECURITYMGNT;

    body_jso = i_paras->val_jso;
    ret = create_new_task(IMPORT_CA_SERVER_CRT_TASK_NAME, ca_cert_remote_import_status_monitor, cert_import_data,
        body_jso, i_paras->uri, o_result_jso);
    if (ret != RF_OK) {
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__);
        task_monitor_info_free(cert_import_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_ACCEPTED;
}


LOCAL gint32 act_cert_update_svc_import_ca_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    IMPORT_CERT_PARAMS_S params;

    
    if ((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = is_import_cert_or_crl_task_exist(ca_cert_remote_import_status_monitor, i_paras, FILE_TYPE_ROOT_CERT,
        o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    set_import_cert_params(&params, FILE_TYPE_ROOT_CERT, import_cert_set_common_userdata_fn,
        process_import_local_ca_cert, process_import_remote_ca_cert, PROPERTY_CERT_UPDATE_SERVICE_CA_CERT_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        return ret;
    }

    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_CERT_IMPORT_FAILED, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import root cert fail, ret:%d", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}


LOCAL gint32 act_cert_update_svc_import_ca_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_cert_update_svc_import_ca_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_OK && ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL void get_single_cert_comon_info(OBJ_HANDLE obj_handle, json_object *cert_info_jso)
{
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_ISSUEBY,
        PROPERTY_SECURITY_SERVICE_ISSUE_BY, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_ISSUETO,
        PROPERTY_SECURITY_SERVICE_ISSUE_TO, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_VALIDFROM,
        PROPERTY_SECURITY_SERVICE_VALID_FROM, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_VALIDTO,
        PROPERTY_SECURITY_SERVICE_VALID_TO, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_SN,
        PROPERTY_SECURITY_SERVICE_SERIAL_NUMBER, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_SIGALGORITHM,
        PROPERTY_SECURITY_SERVICE_SIG_ALGO, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_KEYUSAGE,
        PROPERTY_SECURITY_SERVICE_KEY_USAGE, cert_info_jso);
    (void)rf_add_property_jso_uint32(obj_handle, PROPERTY_CA_SERVER_CACERT_KEYLENGTH,
        PROPERTY_SECURITY_SERVICE_KEY_LENGTH, cert_info_jso);
    (void)rf_add_property_jso_string(obj_handle, PROPERTY_CA_SERVER_CACERT_FINGERPRINT,
        PROPERTY_CA_SERVER_CACERT_FINGERPRINT, cert_info_jso);
}


LOCAL void get_single_ca_cert_info(OBJ_HANDLE obj_handle, json_object *cert_info_jso)
{
    gchar cert_crl_path[MAX_FILEPATH_LENGTH] = {0};
    gchar crl_start_time[CERT_ITEM_LEN_128] = {0};
    gchar crl_expire_time[CERT_ITEM_LEN_128] = {0};

    
    get_single_cert_comon_info(obj_handle, cert_info_jso);

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CA_SERVER_CACERT_CRL_FILEPATH, cert_crl_path,
        sizeof(cert_crl_path));
    if (g_strcmp0(cert_crl_path, "") != 0) {
        json_object_object_add(cert_info_jso, PROPERTY_SECURITY_SERVICE_IS_IMPORT_CRL, json_object_new_boolean(TRUE));
    } else {
        json_object_object_add(cert_info_jso, PROPERTY_SECURITY_SERVICE_IS_IMPORT_CRL, json_object_new_boolean(FALSE));
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CA_SERVER_CACERT_CRL_STARTTIME, crl_start_time,
        sizeof(crl_start_time));
    if (g_strcmp0(crl_start_time, "") != 0) {
        json_object_object_add(cert_info_jso, PROPPETY_SECURITY_SERVICE_CRL_VALID_FROM,
            json_object_new_string(crl_start_time));
    } else {
        json_object_object_add(cert_info_jso, PROPPETY_SECURITY_SERVICE_CRL_VALID_FROM, NULL);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_CA_SERVER_CACERT_CRL_EXPIRETIME, crl_expire_time,
        sizeof(crl_expire_time));
    if (g_strcmp0(crl_expire_time, "") != 0) {
        json_object_object_add(cert_info_jso, PROPPETY_SECURITY_SERVICE_CRL_VALID_TO,
            json_object_new_string(crl_expire_time));
    } else {
        json_object_object_add(cert_info_jso, PROPPETY_SECURITY_SERVICE_CRL_VALID_TO, NULL);
    }

    return;
}


LOCAL gint32 get_cert_update_svc_ca_cert_info(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    gint32 ret;
    json_object *cert_info_jso = NULL;
    OBJ_HANDLE obj_handle;
    gchar cert_sn[CERT_ITEM_LEN_64] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: the parameter is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 用户权限判断: 需满足只读权限，否则返回403，无权限访问
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 获取CA证书对象句柄
    ret = dal_get_object_handle_nth(CLASS_CA_SERVER_CACERT, 0, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get object handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取序列号，确认证书是否已导入
    (void)dal_get_property_value_string(obj_handle, PROPERTY_CA_SERVER_CACERT_SN, cert_sn, sizeof(cert_sn));
    if (strlen(cert_sn) == 0) {
        return HTTP_OK;
    }

    // 创建新的对象
    cert_info_jso = json_object_new_object();
    if (cert_info_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 获取CA服务器CA证书及吊销列表属性值，并添加到对象中
    get_single_ca_cert_info(obj_handle, cert_info_jso);
    json_object_object_add(*o_result_jso, RPPROP_SERVER_CERT, cert_info_jso);

    return HTTP_OK;
}


LOCAL gint32 get_cert_update_svc_client_cert_info(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso)
{
    json_object *cert_info_jso = NULL;
    OBJ_HANDLE obj_handle = 0;
    gchar cert_sn[CERT_ITEM_LEN_64] = {0};

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: the parameter is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // 用户权限判断: 需满足只读权限，否则返回403，无权限访问
    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    // 获取CA证书对象句柄
    gint32 ret = dal_get_object_handle_nth(CLASS_CA_SERVER_CLIENTCERT, 0, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get object handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s json_object_new_object fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取序列号，确认证书是否已导入
    (void)dal_get_property_value_string(obj_handle, PROPERTY_CA_SERVER_CLIENTCERT_SN, cert_sn, sizeof(cert_sn));
    if (strlen(cert_sn) == 0) {
        return HTTP_OK;
    }

    // 创建新的对象
    cert_info_jso = json_object_new_object();
    if (cert_info_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取CA服务器客户端信息，并添加到json对象中，暂不支持证书链信息的查询
    get_single_cert_comon_info(obj_handle, cert_info_jso);
    json_object_object_add(*o_result_jso, RPPROP_CLIENT_CERT, cert_info_jso);

    return HTTP_OK;
}


LOCAL gint32 get_cert_update_srv_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guint32 i;
    guint32 len;
    gint32 ret;
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    gchar rsc_uri[MAX_URI_LENGTH] = {0};
    json_object *action_jso = NULL;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: the parameter is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot error. ret is %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = snprintf_s(rsc_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_CERTUPDATE_SVC_URI, slot_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret is %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    action_jso = json_object_new_object();
    if (action_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc new object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    len = G_N_ELEMENTS(g_cert_update_svc_action_array);
    for (i = 0; i < len; i++) {
        rf_add_action_prop(action_jso, (const gchar*)rsc_uri, g_cert_update_svc_action_array[i]);
    }

    *o_result_jso = action_jso;
    return HTTP_OK;
}


LOCAL gint32 get_cert_update_srv_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar action_info_uri[MAX_URI_LENGTH] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: the parameter is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    (void)redfish_get_board_slot(slot_id, MAX_RSC_ID_LEN);

    ret = snprintf_s(uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, MANAGER_CERTUPDATE_SVC_URI, slot_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret is %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = rf_update_rsc_actioninfo_uri(g_cert_update_svc_action_array, G_N_ELEMENTS(g_cert_update_svc_action_array),
        i_paras->uri, uri, action_info_uri, MAX_URI_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: update action info uri (%s) failed", __FUNCTION__, i_paras->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)action_info_uri);

    return HTTP_OK;
}

LOCAL gint32 get_cert_updateservice_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, MANAGER_CERTUPDATE_SVC_URI);
}


LOCAL gint32 process_import_local_crl_cert(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso)
{
    gint32 ret;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;
    GSList *input_list = NULL;
    gint32 http_code = HTTP_OK;

    if (strlen(file_path) >= MAX_FILEPATH_LENGTH) {
        debug_log(DLOG_ERROR, "%s: the length of file path is over %d", __FUNCTION__, MAX_FILEPATH_LENGTH);
        ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        goto EXIT;
    }

    if (vos_get_file_length(file_path) > MAX_CRL_FILE_LEN) {
        debug_log(DLOG_ERROR, "%s: file is large than %d", __FUNCTION__, MAX_FILEPATH_LENGTH);
        ret =  IMPORT_CERT_FILE_LEN_EXCEED_ERR;
        goto EXIT;
    }

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);

    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    ret = uip_call_class_method_redfish((guint8)from_webui_flag, user_name, client, 0, CLASS_CA_SERVER_CACERT,
        CA_SERVER_CACERT_METHOD_IMPORT_CRL, AUTH_DISABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call remote method(%s) fail, ret = %d.", __FUNCTION__,
            CA_SERVER_CACERT_METHOD_IMPORT_CRL, ret);
    }

    parse_import_crl_common_ret_code(ret, o_message_jso, &http_code);
    ret = http_code;

EXIT:
    if (ret != HTTP_OK) {
        (void)proxy_delete_file(0, NULL, NULL, file_path);
    }

    return ret;
}


LOCAL gint32 crl_remote_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;

    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_crl_cert, monitor_fn_data,
        message_obj);
    return ret;
}


LOCAL gint32 process_import_remote_crl_cert(PROVIDER_PARAS_S *i_paras, const gchar *uri, json_object **o_message_jso,
    json_object **o_result_jso)
{
    TASK_MONITOR_INFO_S *import_crl_data = NULL;
    json_object *body_jso = NULL;

    gint32 ret = rf_start_file_download(i_paras, uri, IMPORT_CA_SERVER_CRL_TRANSFER_FILEINFO_CODE, o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: start crl file download fail", __FUNCTION__);
        return ret;
    }

    import_crl_data = cert_import_info_new(i_paras, import_cert_set_common_userdata_fn);
    if (import_crl_data == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    import_crl_data->task_progress = TASK_NO_PROGRESS;  
    import_crl_data->rsc_privilege = USERROLE_SECURITYMGNT;
    body_jso = i_paras->val_jso;

    ret = create_new_task(IMPORT_CA_SERVER_CRL_TASK_NAME, crl_remote_import_status_monitor, import_crl_data, body_jso,
        i_paras->uri, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create new task failed", __FUNCTION__);
        task_monitor_info_free(import_crl_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_ACCEPTED;
}


LOCAL gint32 act_cert_update_svc_import_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    IMPORT_CERT_PARAMS_S params = {0};

    
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: the input parameter is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = is_import_cert_or_crl_task_exist(crl_remote_import_status_monitor, i_paras, FILE_TYPE_CRL,
        o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    
    set_import_cert_params(&params, FILE_TYPE_CRL, import_cert_set_common_userdata_fn, process_import_local_crl_cert,
        process_import_remote_crl_cert, PROPERTY_CERT_UPDATE_SERVICE_CRL_PATH);

    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        return ret;
    }

    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_CRL_IMPORT_FAILED, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import crl fail, ret = %d.", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}

LOCAL gint32 act_cert_update_svc_import_crl_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_cert_update_svc_import_crl(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_OK && ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
    }
    return ret;
}


LOCAL gint32 act_cert_update_svc_delete_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    gint32 ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        i_paras->obj_handle, CLASS_CA_SERVER_CACERT, CA_SERVER_CACERT_METHOD_DELETE_CRL, AUTH_ENABLE,
        i_paras->user_role_privilege, NULL, NULL);
    if (ret == RET_OK) {
        ret = HTTP_OK;
    } else if (ret == VOS_ERR_MA_CRL_NOT_EXIST) {
        (void)create_message_info(MSGID_CRL_NOT_EXISTED, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: CA CRL not exist, ret:%d", __FUNCTION__, ret);
        ret = HTTP_BAD_REQUEST;
    } else {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: delete CRL to CA fail, ret:%d", __FUNCTION__, ret);
        ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    return ret;
}


LOCAL void import_client_cert_set_param_fn(PROVIDER_PARAS_S *i_paras, json_object *user_data)
{
    json_object* jso_pwd = NULL;
    gint32 ret;

    import_cert_set_common_userdata_fn(i_paras, user_data);

    json_bool ret_bool = json_object_object_get_ex(i_paras->val_jso, RFACTION_PARAM_PWD, &jso_pwd);
    if (ret_bool == TRUE) {
        ret = json_object_object_add(user_data, RFACTION_PARAM_PWD, json_object_get(jso_pwd));
    } else {  // 不输入密码，表示空密码
        ret = json_object_object_add(user_data, RFACTION_PARAM_PWD, json_object_new_string(""));
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: add the password to user data failed.", __FUNCTION__);
    }
}

LOCAL gint32 parse_import_client_cert_result(gint32 ret, json_object **o_message_jso)
{
    switch (ret) {
        case RET_OK:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;
        case RET_ERR:
        case VOS_ERR_MA_FILE_PATH_INVALILD:
        case VOS_ERR_MA_FILE_LEN_EXCEED:
            (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case VOS_ERR_MA_FORMAT_ERR:
            (void)create_message_info(MSGID_CERTIFICATE_FORMAT_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case VOS_ERR_MA_KEY_USAGE_CERT_SIGN_ERR:
        case VOS_ERR_MA_BASIC_CONSTRAINTS_ERR:
            (void)create_message_info(MSGID_CLICERT_CONSTRAINTS_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SSL_WEAK_CA_SHA1:
            (void)create_message_info(MSGID_CA_MD_TOO_WEAK, NULL, o_message_jso, "SHA1");
            return HTTP_BAD_REQUEST;
        case SSL_WEAK_CA_MD:
            (void)create_message_info(MSGID_CA_MD_TOO_WEAK, NULL, o_message_jso, "MD5");
            return HTTP_BAD_REQUEST;
        case VOS_ERR_MA_CERT_EXPIRE:
        case SSL_CACERT_OVERDUED:
            (void)create_message_info(MSGID_CERT_EXPIRED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case VOS_ERR_MA_PWD_LEN_INVALID:
            (void)create_message_info(MSGID_INVALID_PSWD_LEN, RFACTION_PARAM_PWD, o_message_jso,
                CERT_PASSWORD_MIN_LEN_STR, CERT_PASSWORD_MAX_LEN_STR);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 process_import_local_client_cert(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    const gchar *user_name = NULL;
    const gchar *client = NULL;
    const gchar *pwd = NULL;
    gint32 user_priv = 0;
    gint32 from_webui_flag = REDFISH_REQ_FROM_OTHER_CLIENT;

    
    if (strlen(file_path) >= MAX_FILEPATH_LENGTH) {
        debug_log(DLOG_ERROR, "%s: file path length is over %d or file is oversized", __FUNCTION__,
            MAX_FILEPATH_LENGTH);
        ret = IMPORT_CERT_INVALID_FILEPATH_ERR;
        goto EXIT;
    }

    if (vos_get_file_length(file_path) > CERT_MAX_SIZE) {
        debug_log(DLOG_ERROR, "%s: file is oversized", __FUNCTION__);
        ret = IMPORT_CERT_FILE_LEN_EXCEED_ERR;
        goto EXIT;
    }

    
    import_cert_get_common_userdata_fn(&user_name, &client, &user_priv, &from_webui_flag, user_data);
    (void)get_element_str(user_data, RFACTION_PARAM_PWD, &pwd);

    
    input_list = g_slist_append(input_list, g_variant_new_string(file_path));
    input_list = g_slist_append(input_list, g_variant_new_string(pwd));

    ret = uip_call_class_method_redfish((guint8)from_webui_flag, user_name, client, 0, CLASS_CA_SERVER_CLIENTCERT,
        CA_SERVER_CLIENTCERT_METHOD_IMPORT_CLIENTCERT, AUTH_DISABLE, user_priv, input_list, NULL);
    uip_free_gvariant_list(input_list);

    ret = parse_import_client_cert_result(ret, o_message_jso);

EXIT:
    json_object_clear_string(user_data, RFACTION_PARAM_PWD);

    if (ret != HTTP_OK) {
        (void)proxy_delete_file(0, NULL, NULL, file_path);
    }
    return ret;
}


LOCAL gint32 client_cert_remote_import_status_monitor(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj)
{
    gint32 ret;

    ret = import_cert_common_status_monitor(origin_obj_path, body_jso, process_import_local_client_cert,
        monitor_fn_data, message_obj);
    return ret;
}


LOCAL gint32 process_import_remote_client_cert(PROVIDER_PARAS_S *i_paras, const gchar *remote_uri,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    TASK_MONITOR_INFO_S *cert_import_data = NULL;
    json_object *body_jso = NULL;

    ret = rf_start_file_download(i_paras, remote_uri, IMPORT_CA_SERVER_CLIENT_CERT_TRANSFER_FILEINFO_CODE,
        o_message_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: start file download fail", __FUNCTION__);
        return ret;
    }

    cert_import_data = cert_import_info_new(i_paras, import_client_cert_set_param_fn);
    if (cert_import_data == NULL) {
        debug_log(DLOG_ERROR, "%s: alloc a new cert_import info failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    cert_import_data->task_progress = TASK_NO_PROGRESS; 
    cert_import_data->rsc_privilege = USERROLE_SECURITYMGNT;

    body_jso = i_paras->val_jso;
    ret = create_new_task(IMPORT_CA_SERVER_CLIENT_TASK_NAME, client_cert_remote_import_status_monitor, cert_import_data,
        body_jso, i_paras->uri, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: create new task failed.", __FUNCTION__);
        task_monitor_info_free(cert_import_data);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_ACCEPTED;
}


LOCAL gint32 act_cert_update_svc_import_client_cert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    IMPORT_CERT_PARAMS_S params;

    
    if ((i_paras == NULL) || (o_message_jso == NULL) || (o_result_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s the parameter is invalid.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    
    ret = is_import_cert_or_crl_task_exist(client_cert_remote_import_status_monitor, i_paras, FILE_TYPE_CLIENT_CERT,
        o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    set_import_cert_params(&params, FILE_TYPE_CLIENT_CERT, import_client_cert_set_param_fn,
        process_import_local_client_cert, process_import_remote_client_cert,
        PROPERTY_CERT_UPDATE_SERVICE_CLIENT_CERT_PATH);
    ret = process_cert_or_crl_import(i_paras, o_message_jso, o_result_jso, &params);
    
    if (ret == HTTP_OK || ret == HTTP_ACCEPTED || *o_message_jso != NULL) {
        return ret;
    }

    switch (ret) {
        case IMPORT_CERT_URI_DISMATCH_ERR:
        case IMPORT_CERT_FILE_LEN_EXCEED_ERR:
        case IMPORT_CERT_INVALID_FILEPATH_ERR:
            (void)create_message_info(MSGID_ENCRYPTED_CERT_IMPORT_FAILED, NULL, o_message_jso);
            ret = HTTP_BAD_REQUEST;
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s: import client cert fail, ret = %d.", __FUNCTION__, ret);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            break;
    }

    return ret;
}

LOCAL gint32 act_cert_update_svc_import_client_cert_entry(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret = act_cert_update_svc_import_client_cert(i_paras, o_message_jso, o_result_jso);
    if (ret != HTTP_OK && ret != HTTP_ACCEPTED && i_paras != NULL) {
        delete_user_specific_file(i_paras, i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_CONTENT);
        json_object_clear_string(i_paras->val_jso, RFACTION_PARAM_PWD);
    }
    return ret;
}


gint32 cert_update_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean b_ret;

    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    b_ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (b_ret != TRUE) {
        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_cert_update_service_provider;
    *count = G_N_ELEMENTS(g_cert_update_service_provider);
    return RET_OK;
}


gint32 cert_update_svc_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gboolean b_ret;

    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    b_ret = redfish_check_manager_uri_valid(i_paras->uri);
    if (b_ret == TRUE) {
        *prop_provider = g_cert_update_svc_actioninfo_provider;
        *count = G_N_ELEMENTS(g_cert_update_svc_actioninfo_provider);
        return RET_OK;
    } else {
        return HTTP_NOT_FOUND;
    }
}