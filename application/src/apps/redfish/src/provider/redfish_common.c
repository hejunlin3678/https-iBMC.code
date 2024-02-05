

#include "redfish_provider.h"
#include "redfish_http.h"
#include "redfish_util.h"


void parse_single_value(gboolean reading_success, json_object* obj_json, 
    gchar* value_str, gchar value, gchar* str_value)
{
    if (obj_json == NULL || value_str == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return;
    }

    if (!reading_success) {
        json_object_object_add(obj_json, value_str, NULL);
    } else {
        if (NULL == str_value) {
            if (value) {
                json_object_object_add(obj_json, value_str, json_object_new_boolean(TRUE));
            } else {
                json_object_object_add(obj_json, value_str, json_object_new_boolean(FALSE));
            }
        } else {
            json_object_object_add(obj_json, value_str, json_object_new_string(str_value));
        }
    }
    return;
}


gint32 is_support_mutual_auth(json_object **o_message_jso, const gchar *property_name)
{
    guchar product_auth_enable = 0;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    if (o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = dfl_get_object_handle(BMC_PRODUCT_NAME_APP, &obj_handle);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, property_name, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get %s obj_handle fail, ret:%d", __FUNCTION__, BMC_PRODUCT_NAME_APP, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, &product_auth_enable);
    if (ret != RET_OK) {
        (void)create_message_info(MSGID_INTERNAL_ERR, property_name, o_message_jso);
        debug_log(DLOG_ERROR, "%s: get property %s fail, ret: %d", __FUNCTION__,
            PROPERTY_MUTUAL_AUTHENTICATION_ENABLE, ret);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    if (product_auth_enable == FALSE) {
        (void)create_message_info(MSGID_CERTIFICATE_2FA_NOT_SUPPORTED, property_name, o_message_jso);
        return HTTP_NOT_IMPLEMENTED;
    }
    return RET_OK;
}


void cert_import_info_free_func(void *monitor_data)
{
    if (monitor_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return;
    }

    json_object *jso = (json_object *)monitor_data;

    (void)json_object_put(jso);
    return;
}


void import_cert_set_common_userdata_fn(PROVIDER_PARAS_S *i_paras, json_object *user_data)
{
    if (i_paras == NULL || user_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return;
    }

    json_object_object_add(user_data, RF_LOG_USER_NAME, json_object_new_string(i_paras->user_name));
    json_object_object_add(user_data, RF_LOG_USER_IP, json_object_new_string(i_paras->client));
    json_object_object_add(user_data, RF_USER_PRIV, json_object_new_int(i_paras->user_role_privilege));
    json_object_object_add(user_data, RF_USERDATA_FROM_WEBUI_FLAG, json_object_new_int(i_paras->is_from_webui));
    return;
}


void import_cert_get_common_userdata_fn(const gchar **user_name, const gchar **client, gint32 *user_priv,
    gint32 *from_webui_flag, json_object *user_data)
{
    if (user_name == NULL || client == NULL || user_priv == NULL || from_webui_flag == NULL || user_data == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return;
    }
    (void)get_element_str(user_data, RF_LOG_USER_NAME, user_name);
    (void)get_element_str(user_data, RF_LOG_USER_IP, client);
    (void)get_element_int(user_data, RF_USER_PRIV, user_priv);
    (void)get_element_int(user_data, RF_USERDATA_FROM_WEBUI_FLAG, from_webui_flag);
    return;
}


gint32 check_cert_import_by_uri(const gchar *str_uri, IMPORT_CERT_FILE_TYPE file_type, gboolean *is_local_import)
{
    gboolean b_ret;
    gint32 ret;
    const gchar *uri_regex_template = NULL;
    if (str_uri == NULL || is_local_import == NULL) {
        debug_log(DLOG_ERROR, "%s: input param invalid", __FUNCTION__);
        return RET_ERR;
    }

    switch (file_type) {
        case FILE_TYPE_ROOT_CERT:
            uri_regex_template = SERVER_CERT_IMPORT_REGEX;
            break;
        case FILE_TYPE_CRL:
        case FILE_TYPE_LDAP_CRL:
        case FILE_TYPE_SYSLOG_CRL:
            uri_regex_template = CRL_IMPORT_REGEX;
            break;
        case FILE_TYPE_CLIENT_CERT:
            uri_regex_template = CUSTOM_CERT_IMPORT_REGEX;
            break;
        default:
            debug_log(DLOG_ERROR, "%s: invalid inport file type:%d", __FUNCTION__, file_type);
            return RET_ERR;
    }

    b_ret = g_regex_match_simple(uri_regex_template, str_uri, G_REGEX_OPTIMIZE, (GRegexMatchFlags)0);
    if (b_ret == FALSE) {
        return IMPORT_CERT_URI_DISMATCH_ERR;
    }

    if (str_uri[0] != '/') {
        
        *is_local_import = FALSE;
        return RET_OK;
    } else {
        // 以本地的方式导入双因素根证书
        *is_local_import = TRUE;
        ret = dal_check_real_path(str_uri);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s :Cert file real_path is illegal! return %d\r\n", __FUNCTION__, ret);
            return IMPORT_CERT_INVALID_FILEPATH_ERR;
        }
    }
    return RET_OK;
}


gint32 json_array_is_empty(json_object *array_json)
{
    gint32 len;
    gint32 array_index;
    gint32 array_null_count = 0;
    json_object *item_jso = NULL;

    len = json_object_array_length(array_json);

    for (array_index = 0; array_index < len; array_index++) {
        item_jso = json_object_array_get_idx(array_json, array_index);
        if (item_jso == NULL) {
            debug_log(DLOG_ERROR, "%s, %d : item_jso is null, array_index %d", __FUNCTION__, __LINE__, array_index);
            continue;
        }
        
        if (json_object_object_length(item_jso) == 0 && item_jso->_userdata == NULL) {
            array_null_count++;
        }
        
    }

    if (len == array_null_count) {
        return RET_ERR;
    }

    return RET_OK;
}

gint32 get_ps_handle_by_slot(guint8 slot_id, OBJ_HANDLE *obj_handle)
{
    gchar device_name[POWER_STR_MAX_LEN] = {0};
    if (dal_is_pangea_pacific_series() == FALSE) {
        slot_id += 1;
    }

    gint32 ret = sprintf_s(device_name, sizeof(device_name), POWER_SUPPLY_UNIT_NAME "%u", slot_id);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "snprintf_s fail ret[%d]", ret);
        return RET_ERR;
    }

    return dal_get_specific_object_string(CLASS_NAME_PS, PROTERY_PS_DEVICENAME, (const gchar *)device_name,
        obj_handle);
}