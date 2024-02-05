

#include "redfish_provider.h"
#include "redfish_message_system.h"
#include "redfish_provider_system.h"

// CertificatesType类型(redfish标准定义的类型，PEM:单证书, PEMchain:证书链, PKCS7:p7形式单或多证书. 当前只支持PEM)
#define CERTIFICATE_TYPE_SINGLE_PEM "PEM"

// 证书导入、重置方法参数
#define METHOD_PARAM_USAGE_SECUREBOOT           "SecureBoot"
#define METHOD_PARAM_USAGE_BOOT                 "Boot"
#define METHOD_PARAM_DATABASE_DB                "db"
#define METHOD_PARAM_DATABASE_DBX               "dbx"
#define METHOD_PARAM_DATABASE_CERT              "cert"
#define METHOD_PARAM_DATABASE_CRL               "crl"
#define METHOD_PARAM_RESET_TYPE_DELETE          "DeleteAllKeys"
#define METHOD_PARAM_RESET_TYPE_TO_DEFAULT      "ResetAllKeysToDefault"
#define METHOD_PARAM_FILE_PATH                  "None"

// 证书最大长度字符串(与CERT_MAX_SIZE相关)
#define CERT_MAX_SIZE_STR "102400"

LOCAL gint32 get_system_bios_oem_bootcert_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_bios_oem_import_bootcert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_bios_oem_import_bootcrl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_bios_oem_reset_bootcert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_bios_oem_reset_bootcrl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_oem_bootcert_certificates(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_oem_bootcert_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_bios_oem_bootcert_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_bootcert_import_secureboot_cert(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object** o_result_jso);
LOCAL gint32 act_bootcert_reset_secureboot_cert(PROVIDER_PARAS_S* i_paras,
    json_object** o_message_jso, json_object** o_result_jso);

LOCAL const gchar *g_sytem_bios_oem_bootcert_action_array[] = {
    RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CERT,
    RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CRL,
    RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CERT,
    RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CRL,
    RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_SECUREBOOT_CERT,
    RFPROP_BOOT_CERTIFICATES_ACTION_RESET_SECUREBOOT_CERT
};

LOCAL PROPERTY_PROVIDER_S g_system_bios_oem_bootcert_actioninfo_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_system_bios_oem_bootcert_actioninfo_odata_id,
        NULL, NULL, ETAG_FLAG_ENABLE
    }
};

LOCAL PROPERTY_PROVIDER_S g_system_bios_oem_bootcert_provider[] = {
    {
        RFPROP_ODATA_ID,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_system_bios_oem_bootcert_odataid,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL,
        act_system_bios_oem_import_bootcert, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CRL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL,
        act_system_bios_oem_import_bootcrl, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL,
        act_system_bios_oem_reset_bootcert, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CRL,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID, NULL, NULL,
        act_system_bios_oem_reset_bootcrl, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_CERTIFICATES,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_system_bios_oem_bootcert_certificates,
        NULL, NULL, ETAG_FLAG_ENABLE
    },
    {
        RFPROP_ACTIONS,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_READONLY, SYS_LOCKDOWN_NULL,
        get_system_bios_oem_bootcert_actions,
        NULL, NULL, ETAG_FLAG_DISABLE
    },
    {
        RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_SECUREBOOT_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_bootcert_import_secureboot_cert, ETAG_FLAG_DISABLE
    },
    {
        RFPROP_BOOT_CERTIFICATES_ACTION_RESET_SECUREBOOT_CERT,
        MAP_PROPERTY_NULL, MAP_PROPERTY_NULL,
        USERROLE_SECURITYMGNT, SYS_LOCKDOWN_FORBID,
        NULL, NULL,
        act_bootcert_reset_secureboot_cert, ETAG_FLAG_DISABLE
    }
};

LOCAL gint32 process_return_code(gint32 ret_code, json_object **o_message_jso)
{
    switch (ret_code) {
        case SECUREBOOT_METHOD_OK:
            return HTTP_OK;
        case SECUREBOOT_METHOD_EXPIRED_CERT:
            debug_log(DLOG_ERROR, "%s: Certificate expired.", __FUNCTION__);
            (void)create_message_info(MSGID_CERT_EXPIRED, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SECUREBOOT_METHOD_BIOS_STATE_UNSUPPORT:
            debug_log(DLOG_ERROR, "%s: Bios state not allowed to inport.", __FUNCTION__);
            (void)create_message_info(MSGID_BIOS_STATE_NOT_ALLOWED, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        case SECUREBOOT_METHOD_PEM_FORMAT_ERR:
            debug_log(DLOG_ERROR, "%s: Certificate not in pem format.", __FUNCTION__);
            (void)create_message_info(MSGID_BIOS_PEM_FORMAT_ERR, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        case SECUREBOOT_METHOD_STRING_TOO_LONG:
            debug_log(DLOG_ERROR, "%s: Certificate string is too long.", __FUNCTION__);
            (void)create_message_info(MSGID_STRING_VALUE_TOO_LONG, NULL, o_message_jso,
                RFPROP_BIOS_CERTIFICATE_STRING, CERT_MAX_SIZE_STR);
            return HTTP_BAD_REQUEST;
        case SECUREBOOT_METHOD_IMPORT_LIMIT:
            debug_log(DLOG_ERROR, "%s: Imported certificate has exceeded the limitation.", __FUNCTION__);
            (void)create_message_info(MSGID_BIOS_IMPORT_LIMIT_EXCEED, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        default:
            debug_log(DLOG_DEBUG, "%s: Return code(%d) abnormal.", __FUNCTION__, ret_code);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

gint32 system_bios_oem_bootcert_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    guint32 product_ver = 0;
    guchar arm_enable = 0;
    guchar board_type = 0;

    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (!redfish_check_system_uri_valid(i_paras->uri)) {
        debug_log(DLOG_ERROR, "%s: redfish_check_system_uri_valid failed", __FUNCTION__);
        return HTTP_NOT_FOUND;
    }

    
    ret = redfish_get_x86_enable_type(&board_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get board_type failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (board_type != ENABLE) {
        debug_log(DLOG_ERROR, "%s: board_type is disable.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_product_version_num(&product_ver);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_product_version_num failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_func_ability failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (arm_enable == DISABLE && product_ver < PRODUCT_VERSION_V5) {
        debug_log(DLOG_ERROR, "%s: arm_enable: %u, product_ver: %u", __FUNCTION__, arm_enable, product_ver);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *prop_provider = g_system_bios_oem_bootcert_provider;
    *count = G_N_ELEMENTS(g_system_bios_oem_bootcert_provider);

    return RET_OK;
}

LOCAL gint32 check_cert_string(json_object *val_jso, const gchar **cert_string, json_object **o_message_jso)
{
    if (!get_element_str(val_jso, RFPROP_BIOS_CERTIFICATE_STRING, cert_string)) {
        debug_log(DLOG_ERROR, "%s: get certificate string failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (*cert_string != NULL && strlen(*cert_string) > CERT_MAX_SIZE) {
        (void)create_message_info(MSGID_STRING_VALUE_TOO_LONG, NULL, o_message_jso,
            RFPROP_BIOS_CERTIFICATE_STRING, CERT_MAX_SIZE_STR);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}

LOCAL gint32 check_cert_type(json_object *val_jso, const gchar **cert_type, json_object **o_message_jso)
{
    if (!get_element_str(val_jso, RFPROP_BIOS_CERTIFICATE_TYPE, cert_type)) {
        // 请求体没有CertificateType时设置为默认的PEM格式
        debug_log(DLOG_DEBUG, "%s: CertificateType not exisit in request, set to PEM.", __FUNCTION__);
        *cert_type = CERTIFICATE_TYPE_SINGLE_PEM;
        return RET_OK;
    }

    // 当前只支持PME类型
    if (g_strcmp0(*cert_type, CERTIFICATE_TYPE_SINGLE_PEM) != 0) {
        debug_log(DLOG_ERROR, "%s: cert type(%s) is invalid", __FUNCTION__, *cert_type);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}

LOCAL gint32 check_resetkeys_type(json_object *val_jso, const gchar **reset_type, json_object **o_message_jso)
{
    if (!get_element_str(val_jso, RFPROP_BIOS_RESETKEYS_TYPE, reset_type)) {
        debug_log(DLOG_ERROR, "%s: get resetkeys type failed", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (g_strcmp0(*reset_type, METHOD_PARAM_RESET_TYPE_DELETE) != 0 &&
        g_strcmp0(*reset_type, METHOD_PARAM_RESET_TYPE_TO_DEFAULT) != 0) {
        debug_log(DLOG_ERROR, "%s: reset type(%s) is invalid", __FUNCTION__, *reset_type);
        return HTTP_BAD_REQUEST;
    }

    return RET_OK;
}


LOCAL gint32 get_system_bios_oem_bootcert_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    return get_odata_id(i_paras, o_message_jso, o_result_jso, RF_BIOS_OEM_BOOT_CERTIFICATES_ODATA_ID);
}


LOCAL gint32 act_system_bios_oem_import_bootcert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    const gchar *cert_string = NULL;
    const gchar *cert_type = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    if (i_paras == NULL || i_paras->val_jso == NULL || o_message_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = check_cert_string(i_paras->val_jso, &cert_string, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = check_cert_type(i_paras->val_jso, &cert_type, o_message_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_BIOS_CERTIFICATE_TYPE, RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CERT);
        return ret;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_USAGE_BOOT));
    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_DATABASE_CERT));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_string));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_IMPORT_CERT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    return process_return_code(ret, o_message_jso);
}


LOCAL gint32 act_system_bios_oem_import_bootcrl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    const gchar *cert_string = NULL;
    const gchar *cert_type = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    if (i_paras == NULL || i_paras->val_jso == NULL || o_message_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = check_cert_string(i_paras->val_jso, &cert_string, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = check_cert_type(i_paras->val_jso, &cert_type, o_message_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_BIOS_CERTIFICATE_TYPE, RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_BOOT_CRL);
        return ret;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_USAGE_BOOT));
    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_DATABASE_CRL));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_string));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_IMPORT_CERT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    return process_return_code(ret, o_message_jso);
}


LOCAL gint32 act_system_bios_oem_reset_bootcert(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    const gchar *reset_type = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    if (i_paras == NULL || i_paras->val_jso == NULL || o_message_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = check_resetkeys_type(i_paras->val_jso, &reset_type, o_message_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_BIOS_RESETKEYS_TYPE, RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CERT);
        return ret;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_USAGE_BOOT));
    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_DATABASE_CERT));
    input_list = g_slist_append(input_list, g_variant_new_string(reset_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_RESET_CERT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    return process_return_code(ret, o_message_jso);
}


LOCAL gint32 act_system_bios_oem_reset_bootcrl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;
    const gchar *reset_type = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    if (i_paras == NULL || i_paras->val_jso == NULL || o_message_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = check_resetkeys_type(i_paras->val_jso, &reset_type, o_message_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_BIOS_RESETKEYS_TYPE, RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CRL);
        return ret;
    }

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_USAGE_BOOT));
    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_DATABASE_CRL));
    input_list = g_slist_append(input_list, g_variant_new_string(reset_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_BIOS, METHOD_BIOS_RESET_CERT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    return process_return_code(ret, o_message_jso);
}

LOCAL gint32 get_system_bios_oem_bootcert_certificates(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    _cleanup_gvariant_slist_full_ GSList *output_list = NULL;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = dfl_get_object_handle(OBJ_NAME_BIOS, &obj_handle);
    if (ret != DFL_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: dfl_get_object_handle failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dfl_call_class_method(obj_handle, METHOD_BIOS_GET_CERTIFICATES, NULL, NULL, &output_list);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_DEBUG, "%s: dfl_call_class_method failed, ret: %d", __FUNCTION__, ret);
        return process_return_code(ret, o_message_jso);
    }

    const gchar *cert_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    if (cert_str == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: g_variant_get_string failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_tokener_parse(cert_str);
    if (*o_result_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s: json_tokener_parse failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 get_system_bios_oem_bootcert_actions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (i_paras->is_satisfy_privi == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gchar slot[MAX_RSC_ID_LEN] = {0};
    gint32 ret = redfish_get_board_slot(slot, sizeof(slot));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gchar rsc_uri[MAX_URI_LENGTH] = {0};
    ret = sprintf_s(rsc_uri, sizeof(rsc_uri), RF_BIOS_OEM_BOOT_CERTIFICATES_ODATA_ID, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    json_object *action_jso = json_object_new_object();
    if (action_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    guint32 action_cnt = G_N_ELEMENTS(g_sytem_bios_oem_bootcert_action_array);
    for (guint32 i = 0; i < action_cnt; i++) {
        rf_add_action_prop(action_jso, (const gchar*)rsc_uri, g_sytem_bios_oem_bootcert_action_array[i]);
    }

    *o_result_jso = action_jso;
    return HTTP_OK;
}


LOCAL gint32 get_system_bios_oem_bootcert_actioninfo_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gchar slot[MAX_RSC_ID_LEN] = {0};
    (void)redfish_get_board_slot(slot, MAX_RSC_ID_LEN);

    gchar uri[MAX_URI_LENGTH] = {0};
    gint32 ret = sprintf_s(uri, MAX_URI_LENGTH, RF_BIOS_OEM_BOOT_CERTIFICATES_ODATA_ID, slot);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: sprintf_s failed, ret: %d", __FUNCTION__, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gchar action_info_uri[MAX_URI_LENGTH] = {0};
    ret = rf_update_rsc_actioninfo_uri(g_sytem_bios_oem_bootcert_action_array,
        G_N_ELEMENTS(g_sytem_bios_oem_bootcert_action_array), i_paras->uri, uri, action_info_uri, MAX_URI_LENGTH);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: rf_update_rsc_actioninfo_uri failed, uri: %s", __FUNCTION__, i_paras->uri);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_string((const gchar*)action_info_uri);

    return HTTP_OK;
}


gint32 system_bios_oem_bootcert_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    if (i_paras == NULL || prop_provider == NULL || count == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return RET_ERR;
    }

    if (redfish_check_system_uri_valid(i_paras->uri)) {
        *prop_provider = g_system_bios_oem_bootcert_actioninfo_provider;
        *count = G_N_ELEMENTS(g_system_bios_oem_bootcert_actioninfo_provider);
        return RET_OK;
    }

    return HTTP_NOT_FOUND;
}


LOCAL gint32 act_bootcert_import_secureboot_cert(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    const gchar *data_base = NULL;
    const gchar *cert_str = NULL;
    const gchar *cert_type = NULL;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = check_cert_string(i_paras->val_jso, &cert_str, o_message_jso);
    if (ret != RET_OK) {
        return ret;
    }

    ret = check_cert_type(i_paras->val_jso, &cert_type, o_message_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_BIOS_CERTIFICATE_TYPE, RFPROP_BOOT_CERTIFICATES_ACTION_IMPORT_SECUREBOOT_CERT);
        debug_log(DLOG_ERROR, "%s: check_cert_type failed", __FUNCTION__);
        return ret;
    }

    (void)get_element_str(i_paras->val_jso, RFPROP_BIOS_DATABASE, &data_base);
    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_USAGE_SECUREBOOT)); // usage
    input_list = g_slist_append(input_list, g_variant_new_string(data_base));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_str));
    input_list = g_slist_append(input_list, g_variant_new_string(cert_type));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        BIOS_CLASS_NAME, METHOD_BIOS_IMPORT_CERT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    return process_return_code(ret, o_message_jso);
}


LOCAL gint32 act_bootcert_reset_secureboot_cert(PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    const gchar *data_base = NULL;
    const gchar *resetkeys_type = NULL;
    _cleanup_gvariant_slist_full_ GSList *input_list = NULL;

    if (i_paras == NULL || o_message_jso == NULL || o_result_jso == NULL) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if ((i_paras->user_role_privilege & USERROLE_SECURITYMGNT) == 0) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    gint32 ret = check_resetkeys_type(i_paras->val_jso, &resetkeys_type, o_message_jso);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, NULL, o_message_jso,
            RFPROP_BIOS_RESETKEYS_TYPE, RFPROP_BOOT_CERTIFICATES_ACTION_RESET_BOOT_CERT);
        return ret;
    }

    (void)get_element_str(i_paras->val_jso, RFPROP_BIOS_DATABASE, &data_base);

    input_list = g_slist_append(input_list, g_variant_new_string(METHOD_PARAM_USAGE_SECUREBOOT)); // usage
    input_list = g_slist_append(input_list, g_variant_new_string(data_base));
    input_list = g_slist_append(input_list, g_variant_new_string(resetkeys_type));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        BIOS_CLASS_NAME, METHOD_BIOS_RESET_CERT, AUTH_DISABLE, i_paras->user_role_privilege, input_list, NULL);

    return process_return_code(ret, o_message_jso);
}