
#include "snmp_user_method.h"
#include "snmp_manage_app.h"
#include "keyGen.h"
#include "snmp_method.h"

typedef struct snmp_user_info_data {
    gchar engineid[SNMP_ENGINEID_MAX_LEN + 1];
    gchar snmp_auth[SNMP_AUTH_LEN + 1];
    gchar pwd[SNMP_HEXPASSWORD_MAX_LEN + 1];
    gchar snmp_priv[SNMP_AUTH_LEN + 1];
    gchar privacy_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1];
} SnmpUserInfoData;

const gchar* snmp_auth_str[] =
{
    ".1.3.6.1.6.3.10.1.1.2", // md5
    ".1.3.6.1.6.3.10.1.1.3", // sha
    ".1.3.6.1.6.3.10.1.1.3", // sha1
    ".1.3.6.1.6.3.10.1.1.5", // sha256
    ".1.3.6.1.6.3.10.1.1.6", // sha384
    ".1.3.6.1.6.3.10.1.1.7"  // sha512
};

const gchar* snmp_priv_str[] =
{
    ".1.3.6.1.6.3.10.1.2.1",  // none
    ".1.3.6.1.6.3.10.1.2.2",  // des
    ".1.3.6.1.6.3.10.1.2.4",  // aes
    ".1.3.6.1.4.1.14832.1.4", // aes256
};

#define SNMP_RWUSER_ADMIN_LEN 3
#define SNMP_RWUSER_ROLE_INDEX 4

LOCAL gint32 snmp_fgetent(FM_FILE_S *stream, SNMP_USER_S *user_ptr)
{
    gchar *end = NULL;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };

    if (stream == NULL || user_ptr == NULL) {
        return RET_ERR;
    }

    
    gchar *p = fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, stream);
    if (p == NULL) {
        return RET_ERR;
    }

    while (p) {
        
        if (strstr(buf, SNMP_RWUSER)) {
            
            gchar *substr = strtok_s(p, " ", &end);
            
            if (substr && (strcmp(substr, SNMP_RWUSER) == 0)) {
                user_ptr->user_priv = SNMP_RW_USER;
                substr = strtok_s(end, " ", &end);
                if (substr == NULL) {
                    return RET_ERR;
                }
                (void)strncpy_s(user_ptr->username, sizeof(user_ptr->username), substr, sizeof(user_ptr->username) - 1);
                break;
            }
        } else if (strstr(buf, SNMP_ROUSER)) {
            
            gchar *substr = strtok_s(p, " ", &end);
            
            if (substr && (strcmp(substr, SNMP_ROUSER) == 0)) {
                user_ptr->user_priv = SNMP_RO_USER;
                substr = strtok_s(end, " ", &end);
                if (substr == NULL) {
                    return RET_ERR;
                }
                (void)strncpy_s(user_ptr->username, sizeof(user_ptr->username), substr, sizeof(user_ptr->username) - 1);
            }
        } else if (strstr(buf, SNMP_GROUP)) {
            
            gchar *substr = strtok_s(p, " ", &end);
            
            if (substr && (strcmp(substr, SNMP_GROUP) == 0)) {
                substr = strtok_s(end, " ", &end);
                if (substr == NULL) {
                    return RET_ERR;
                }
                (void)strncpy_s(user_ptr->user_group, sizeof(user_ptr->user_group), substr,
                    sizeof(user_ptr->user_group) - 1);
                break;
            }
        }

        
        p = fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, stream);
    }

    if (p) {
        return RET_OK;
    }

    return RET_ERR;
}


LOCAL gint32 snmp_fgetnam(const gchar *username, SNMP_USER_S *snmpuser_ptr)
{
    FM_FILE_S *stream = NULL;

    
    if (username == NULL || strlen(username) == 0 || snmpuser_ptr == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:Input NULL pointer\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    g_mutex_lock(&g_snmp_mutex);

    
    stream = fm_fopen(SNMP_SHORT_FILE_NAME, "r");
    if (stream == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    
    while (snmp_fgetent(stream, snmpuser_ptr) != RET_ERR) {
        
        if ((strlen(username) == strlen((gchar *)snmpuser_ptr->username))) {
            if (memcmp(snmpuser_ptr->username, username, strlen(username)) == 0) {
                fm_fclose(stream);
                g_mutex_unlock(&g_snmp_mutex);
                return RET_OK;
            }
        }

        (void)memset_s(snmpuser_ptr, sizeof(SNMP_USER_S), 0, sizeof(SNMP_USER_S));
    }

    
    fm_fclose(stream);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_ERR;
}


gint32 snmp_check_username_exist(const gchar *username)
{
    gint32 ret = 0;
    SNMP_USER_S snmpuser_ptr;
    (void)memset_s(&snmpuser_ptr, sizeof(SNMP_USER_S), 0, sizeof(SNMP_USER_S));

    
    ret = snmp_fgetnam(username, &snmpuser_ptr);
    if (ret != RET_OK) {
        return SNMP_USERNAME_NOT_EXIST;
    }

    return SNMP_USERNAME_EXIST;
}


gint32 AddSnmpUserName(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gchar *user_name = NULL;
    gint32 ret = 0;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }

    if (get_snmpv3_status() == SNMP_STATE_DISABLE) {
        debug_log(DLOG_INFO, "[%s] snmpv3 is disabled, config won't be written.", __FUNCTION__);
        return RET_OK;
    }
    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:User_name is NULL.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (strlen(user_name) > SNMP_USERNAME_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:%d:User_name length error.", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    
    if (snmp_check_username_exist(user_name) == SNMP_USERNAME_EXIST) {
        
        free_mem_set_null(user_name);
        return SNMP_USERNAME_EXIST;
    }

    ret = snmp_add_username_info((const gchar *)user_name);
    free_mem_set_null(user_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:add user info to snmp file failed, ret %d.", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL gint32 __is_match_user_v3info(gchar *buf, gint32 buf_size, const gchar *user_name)
{
    gchar *p_config = NULL;
    gchar *end = NULL;
    if ((buf == NULL) || (buf_size <= 0)) {
        return RET_ERR;
    }
    p_config = strtok_s(buf, " ", &end);
    if (p_config == NULL) {
        return RET_ERR;
    }
    // 匹配配置文件中v3用户第一行用户名信息 rwuser Administrator priv
    if ((strcmp(p_config, SNMP_RWUSER) == 0) || (strcmp(p_config, SNMP_ROUSER) == 0)) {
        p_config = strtok_s(end, " ", &end);
        if ((p_config != NULL) && ((user_name == NULL) || (strcmp(p_config, user_name) == 0))) {
            return RET_OK;
        }
        return RET_ERR;
    }
    // 匹配配置文件中v3用户第二行用户名信息 group gmyuser usm Administrator
    if (strcmp(p_config, SNMP_GROUP) == 0) {
        p_config = strtok_s(end, " ", &end);
        if (p_config == NULL) {
            return RET_ERR;
        }
        p_config = strtok_s(end, " ", &end);
        if ((p_config == NULL) || (strcmp(p_config, "usm") != 0)) {
            return RET_ERR;
        }
        p_config = strtok_s(end, " ", &end);
        if ((p_config != NULL) && ((user_name == NULL) || (strcmp(p_config, user_name) == 0))) {
            return RET_OK;
        }
    }
    return RET_ERR;
}


gint32 del_snmp_conf_from_file(const gchar *user_name, guint8 *match_flag)
{
    FM_FILE_S *fp = NULL;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 p_start = 0;
    gint32 p_end = 0;
    gint32 del_len = 0;
    gint32 ret = 0;
    size_t last_ch_index;

    g_mutex_lock(&g_snmp_mutex);

    
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        last_ch_index = strlen(buf) - 1;
        if (last_ch_index >= SNMP_CONF_MAXLINE_LEN) { 
            p_start = fm_ftell(fp);
            continue;
        }
        buf[last_ch_index] = 0; 
        if (__is_match_user_v3info(buf, sizeof(buf), user_name) == RET_OK) {
            p_end = fm_ftell(fp);
            del_len = p_end - p_start;
            *match_flag = TRUE;
            break;
        }
        
        p_start = fm_ftell(fp);
    }

    ret = fm_delete((gsize)del_len, (gsize)p_start, fp);
    if (ret != FM_OK) {
        debug_log(DLOG_ERROR, "Call fm_delete failed,ret = %d", ret);
        fm_fclose(fp);
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_OK;
}


gint32 snmp_del_username_from_file(const gchar *user_name)
{
    guint8 match_flag;
    gint32 ret;
    do {
        match_flag = FALSE;
        ret = del_snmp_conf_from_file(user_name, &match_flag);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: del_snmp_conf_from_file fail, ret = %d", __FUNCTION__, ret);
            return RET_ERR;
        }
    } while (match_flag == TRUE);
    return RET_OK;
}


gint32 DelSnmpUserName(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gchar *user_name = NULL;
    gint32 ret = 0;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }

    if (get_snmpv3_status() == SNMP_STATE_DISABLE) {
        debug_log(DLOG_INFO, "[%s] snmpv3 is disabled, config won't be wrote.", __FUNCTION__);
        return RET_OK;
    }

    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:User_name is NULL.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    if (strlen(user_name) > SNMP_USERNAME_MAX_LEN) {
        debug_log(DLOG_ERROR, "%s:%d:User_name length error.", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    
    if (snmp_check_username_exist(user_name) != SNMP_USERNAME_EXIST) {
        
        debug_log(DLOG_ERROR, "%s:%d:user_name is not exist.", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return SNMP_USERNAME_NOT_EXIST;
    }

    ret = snmp_del_username_from_file(user_name);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:snmp_del_username_from_file failed.", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    free_mem_set_null(user_name);
    return RET_OK;
}

LOCAL void delete_invalidchar(gchar *string_tmp)
{
    gint i = 0;

    
    if (string_tmp == NULL) {
        return;
    }

    for (i = 0; i < strlen(string_tmp); i++) {
        if (string_tmp[i] == '\n') {
            string_tmp[i] = '\0';
            break;
        }
    }

    return;
}


LOCAL gint32 get_snmp_info(gchar *username, guint32 username_len, gchar *password, gchar *privacy_password,
    gchar *engineid, gint32 engineid_size, gchar *hex_name, gint32 hex_name_size, gchar *hex_passwd, gint32 snmp_auth,
    gchar *hex_privacy_passwd, gint32 snmp_priv)
{
    errno_t safe_fun_ret = EOK;
    FM_FILE_S *old_stream = NULL;
    FM_FILE_S *new_stream = NULL;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar *tmp = NULL;
    SNMP_ADDITION_DATA snmp_data = { 0 };
    gchar authkey[SNMP_AUTHKYT_MAX_LEN + 1] = { 0 };
    gchar privkey[SNMP_AUTHKYT_MAX_LEN + 1] = { 0 };
    gint32 ret = 0;
    gint32 key_len = 0;
    gboolean find_flag = FALSE;

    // 判断鉴权算法类型，并返回鉴权密文长度
    key_len = sc_get_properlength1(snmp_auth, 0);
    if (key_len <= 0) {
        return RET_ERR;
    }

    

    
    
    g_mutex_lock(&g_snmp_mutex);
    old_stream = fm_fopen(SNMP_SHORT_FILE_NAME, "r");
    if (old_stream != NULL) {
        
        find_flag = TRUE;

        while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, old_stream) != NULL) {
            if ((strstr(buf, "oldEngineID") != NULL) || (strstr(buf, "oldEngineBakID") != NULL)) {
                tmp = strstr(buf, " ");
                break;
            }

            (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
        }

        fm_fclose(old_stream);
    }

    
    if ((find_flag == FALSE) || (tmp == NULL)) {
        
        new_stream = fm_fopen(SNMP_SHORT_FILE_NAME, "r");
        if (new_stream == NULL) {
            debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_ERR;
        }

        
        while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, new_stream) != NULL) {
            
            if (strstr(buf, "oldEngineBakID") != NULL) {
                tmp = strstr(buf, " ");
                break;
            }

            (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
        }

        
        fm_fclose(new_stream);
    }

    g_mutex_unlock(&g_snmp_mutex);

    
    if (tmp == NULL) {
        return RET_ERR;
    }

    
    delete_invalidchar(tmp);
    if (strlen(tmp) == 0 || (guint32)strlen(tmp) > engineid_size) {
        debug_log(DLOG_ERROR, "%s failed: param error, strlen(tmp) is illegal.", __FUNCTION__);
        return RET_ERR;
    }

    safe_fun_ret = memmove_s(engineid, engineid_size, tmp + 1, strlen(tmp) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memmove_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    dal_trim_string(engineid, engineid_size);

    
    dal_convert_hex_to_string((guchar *)(engineid + SNMP_HEX_PREFIX_LEN), (guchar *)snmp_data.str_engineID,
        sizeof(snmp_data.str_engineID));
    snmp_data.snmp_auth = snmp_auth;
    snmp_data.pwd_key_type = SNMP_AUTH_TYPE;
    
    ret = get_snmp_pwd_key(username, password, &snmp_data, authkey, sizeof(authkey));
    
    convert_bytes_to_string((guchar *)authkey, key_len, (guchar *)hex_passwd);
    clear_sensitive_info(authkey, sizeof(authkey));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Failed to get auth key", __FUNCTION__);
        return RET_ERR;
    }

    snmp_data.pwd_key_type = SNMP_PRIV_TYPE;
    
    ret = get_snmp_pwd_key(username, privacy_password, &snmp_data, privkey, sizeof(privkey));
    
    convert_bytes_to_string((guchar *)privkey, key_len, (guchar *)hex_privacy_passwd);
    clear_sensitive_info(privkey, sizeof(privkey));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Failed to get priv key", __FUNCTION__);
        return RET_ERR;
    }

    
    dal_convert_string_to_hex((guchar *)username, username_len, (guchar *)hex_name, hex_name_size);
    safe_fun_ret = strncat_s(hex_name, hex_name_size, "00", strlen("00"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 snmp_get_auth_protocol(const gchar *user_name, guint8 *auth_protocol)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, user_name, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get user(%s) object handle failed, %d!", __FUNCTION__, user_name, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_SNMPAUTHPROTOCOL, auth_protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s.%s failed, ret %d", __FUNCTION__, dfl_get_object_name(obj_handle),
            PROPERTY_USER_SNMPAUTHPROTOCOL, ret);
        return ret;
    }

    return ret;
}


LOCAL gint32 snmp_get_priv_protocol(const gchar *user_name, guint8 *priv_protocol)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = 0;

    ret = dal_get_specific_object_string(CLASS_USER, PROPERTY_USER_NAME, user_name, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get user(%s) object handle failed, %d!", __FUNCTION__, user_name, ret);
        return ret;
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_USER_SNMPPRIVACYPROTOCOL, priv_protocol);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get %s.%s failed, ret %d", __FUNCTION__, dfl_get_object_name(obj_handle),
            PROPERTY_USER_SNMPPRIVACYPROTOCOL, ret);
        return ret;
    }

    return RET_OK;
}


gint32 SetSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 iRet = -1;
    gchar *user_pwd = NULL;
    gsize password_len = 0x00;
    gchar *ptr_privacy_pwd = NULL;
    gsize privacy_password_len = 0x00;
    gint32 ret = 0;
    FM_FILE_S *stream = NULL;
    gchar engineid[SNMP_ENGINEID_MAX_LEN + 1] = { 0 };
    gchar hex_name[2 * SNMP_USERNAME_MAX_LEN + 2 + 2 + 1] = { 0 };
    gchar hex_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1] = { 0 };
    gchar hex_priv_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 p_start = 0;
    gint32 p_end = 0;
    gint32 old_buf_len = 0;
    gint32 new_buf_len = 0;
    gchar pwd[SNMP_MAX_PASSWORD_LEN + 1] = { 0 };
    gchar privacy_pwd[SNMP_MAX_PASSWORD_LEN + 1] = { 0 };
    gint32 found = 0;
    guint8 snmp_auth = 0;
    guint8 snmp_priv = 1;
    gchar **str_array = NULL;
    guint8 str_length = 0;
    gsize username_len = 0;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }

    gchar *user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), &username_len);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!");
        return RET_ERR;
    }

    user_pwd = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 1), &password_len);
    if (user_pwd == NULL) {
        debug_log(DLOG_ERROR, "Input NULL auth password pointer!");
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    errno_t safe_fun_ret = strncpy_s(pwd, sizeof(pwd), user_pwd, sizeof(pwd) - 1);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    clear_sensitive_info(user_pwd, password_len);

    ptr_privacy_pwd = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 2), &privacy_password_len);
    if (ptr_privacy_pwd == NULL) {
        debug_log(DLOG_ERROR, "Input NULL privacy password pointer!");
        (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
        free_mem_set_null(user_name);
        free_mem_set_null(user_pwd);
        return RET_ERR;
    }
    safe_fun_ret = strncpy_s(privacy_pwd, SNMP_MAX_PASSWORD_LEN + 1, ptr_privacy_pwd, SNMP_MAX_PASSWORD_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    clear_sensitive_info(ptr_privacy_pwd, privacy_password_len);

    ret = snmp_get_auth_protocol((const gchar *)user_name, &snmp_auth);
    if ((ret != RET_OK) || (snmp_auth > USE_SHA512)) {
        debug_log(DLOG_ERROR, "%s:snmp_get_auth_protocol failed!\n", __FUNCTION__);
        (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
        clear_sensitive_info(privacy_pwd, sizeof(privacy_pwd));
        goto ERROR_EXIT;
    }

    ret = snmp_get_priv_protocol((const gchar *)user_name, &snmp_priv);
    if ((ret != RET_OK) || (snmp_priv > USE_AES256)) {
        (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
        clear_sensitive_info(privacy_pwd, sizeof(privacy_pwd));
        debug_log(DLOG_ERROR, "%s:snmp_get_priv_protocol failed, ret %d!", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    
    ret = get_snmp_info(user_name, (guint32)username_len, pwd, privacy_pwd, engineid, sizeof(engineid), hex_name,
        sizeof(hex_name), hex_pwd, (gint32)snmp_auth, hex_priv_pwd, (gint32)snmp_priv);

    
    (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
    clear_sensitive_info(privacy_pwd, sizeof(privacy_pwd));

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:get_snmp_info failed!\n", __FUNCTION__, __LINE__);
        clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
        clear_sensitive_info(hex_priv_pwd, sizeof(hex_priv_pwd));
        goto ERROR_EXIT;
    }

    

    
    g_mutex_lock(&g_snmp_mutex);
    stream = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (stream == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
        clear_sensitive_info(hex_priv_pwd, sizeof(hex_priv_pwd));
        goto ERROR_EXIT;
    }

    
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, stream) != NULL) {
        
        if (strstr(buf, (gchar *)hex_name) != NULL) {
            old_buf_len = strlen(buf); // 用户信息长度
            str_array = g_strsplit(buf, " ", SNMP_TOKENS_PER_LINE);
            if (str_array == NULL) {
                debug_log(DLOG_ERROR, "Prase snmp file failed!\n");
                continue;
            }

            str_length = g_strv_length(str_array);
            if (str_length != SNMP_TOKENS_PER_LINE) {
                debug_log(DLOG_DEBUG, "This line is not in the correct format!");
                g_strfreev(str_array);
                continue;
            }

            
            if (str_array[SNMP_USERNAMEHEX_INDEX_LINE] == NULL ||
                strlen(str_array[SNMP_USERNAMEHEX_INDEX_LINE]) < 2 ||
                g_strcmp0(str_array[SNMP_USERNAMEHEX_INDEX_LINE] + 2, hex_name) != 0) {
                (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
                g_strfreev(str_array);
                continue;
            }

            g_strfreev(str_array);
            found = 1;
            p_end = fm_ftell(stream);      // 文件内部指针的读取位置
            p_start = p_end - old_buf_len; // 开始位置
            break;
        }

        (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
    }

    (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
    
    iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "usmUser 1 3 %s 0x%s 0x%s NULL %s 0x%s %s 0x%s \"\"\n",
        engineid, hex_name, hex_name, snmp_auth_str[snmp_auth], hex_pwd, snmp_priv_str[snmp_priv], hex_priv_pwd);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    new_buf_len = strlen(buf);
    clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
    clear_sensitive_info(hex_priv_pwd, sizeof(hex_priv_pwd));

    
    if (found == 0) {
        p_start = fm_ftell(stream);
        ret = fm_insert(buf, (gsize)new_buf_len, p_start, stream);
    }
    
    else {
        ret = fm_modify(buf, (gsize)new_buf_len, (gsize)p_start, (gsize)old_buf_len, stream);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Call method fail! found:%d", __FUNCTION__, found);
    }

    
    fm_fclose(stream);
    g_mutex_unlock(&g_snmp_mutex);
    free_mem_set_null(user_name);
    free_mem_set_null(user_pwd);
    free_mem_set_null(ptr_privacy_pwd);
    return ret;

ERROR_EXIT:
    free_mem_set_null(user_name);
    free_mem_set_null(user_pwd);
    free_mem_set_null(ptr_privacy_pwd);
    return RET_ERR;
}

LOCAL gint32 snmp_compare_oldengineid(const gchar *user_oldengineid)
{
    gint32 ret = 0;
    const gchar *oldengineid = NULL;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;

    if (user_oldengineid == NULL) {
        return RET_ERR;
    }

    ret = dfl_get_object_handle(OBJ_NAME_SNMP, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP handle failed!\n");
        return RET_ERR;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_SNMP_OLDENGINEID, &property_value);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "Get SNMP oldengineid failed!");
        return RET_ERR;
    }

    oldengineid = g_variant_get_string(property_value, 0);
    if (oldengineid == NULL) {
        debug_log(DLOG_ERROR, "g_variant_get_string() failed!\n");
        g_variant_unref(property_value);
        return RET_ERR;
    }

    if (strlen(oldengineid) != strlen(user_oldengineid) ||
        strncmp(user_oldengineid, oldengineid, strlen(oldengineid))) {
        
        g_variant_unref(property_value);
        return RET_ERR;
    }

    g_variant_unref(property_value);

    return RET_OK;
}


LOCAL gint32 snmp_strcmp_s(gchar *auth_info_from_file, guint32 size, gchar *auth_info)
{
    guint8 ret = 0;
    gint32 i = 0;
    gint32 j = 0;
    gint32 new_len = 0;

    if (auth_info_from_file == NULL || auth_info == NULL) {
        return RET_ERR;
    }

    new_len = strlen(auth_info);
    if (new_len > size - 1) {
        return RET_ERR;
    }

    for (i = 0; i < new_len + 1; i++, j++) {
        ret |= (guint8)auth_info_from_file[j] ^ (guint8)auth_info[i];

        if (!auth_info_from_file[j]) {
            j = 0;
        }
    }

    if (ret != 0) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 snmp_compare_auth_info(gchar *auth_info_from_file, guint32 size, gchar *auth_info)
{
    if (auth_info_from_file == NULL || auth_info == NULL) {
        return RET_ERR;
    }

    if (snmp_strcmp_s(auth_info_from_file, size, auth_info) != 0) {
        return RET_ERR;
    } else {
        return RET_OK;
    }
}

LOCAL void __copy_info_from_file(SnmpUserInfoData *user_info, gchar **str_array, guint32 str_length)
{
    errno_t safe_fun_ret;

    if (str_length != SNMP_TOKENS_PER_LINE) {
        debug_log(DLOG_DEBUG, "This line is not in the correct format!");
        return;
    }

    
    safe_fun_ret = strncpy_s(user_info->engineid, sizeof(user_info->engineid), str_array[SNMP_OLDENGINEID_INDEX_LINE],
        SNMP_ENGINEID_MAX_LEN);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    safe_fun_ret = strncpy_s(user_info->snmp_auth, sizeof(user_info->snmp_auth),
        str_array[SNMP_AUTHPROCOTOL_INDEX_LINE], SNMP_AUTH_LEN);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    safe_fun_ret =
        strncpy_s(user_info->pwd, sizeof(user_info->pwd), str_array[SNMP_USERPASSWDHEX_LINE], SNMP_HEXPASSWORD_MAX_LEN);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    safe_fun_ret = strncpy_s(user_info->snmp_priv, sizeof(user_info->snmp_priv),
        str_array[SNMP_PRIVPROCOTOL_INDEX_LINE], SNMP_AUTH_LEN);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    safe_fun_ret = strncpy_s(user_info->privacy_pwd, sizeof(user_info->privacy_pwd), str_array[SNMP_PRIVPASSWDHEX_LINE],
        SNMP_HEXPASSWORD_MAX_LEN);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }
}


LOCAL gint32 __get_info_from_conf(gchar *user_name, gsize username_len, SnmpUserInfoData *user_info)
{
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar hex_name[2 * SNMP_USERNAME_MAX_LEN + 2 + 2 + 1] = { 0 };
    gchar **str_array = NULL;

    g_mutex_lock(&g_snmp_mutex);

    
    FM_FILE_S *stream = fm_fopen(SNMP_SHORT_FILE_NAME, "r");
    if (stream == NULL) {
        debug_log(DLOG_ERROR, "%s:fail to open %s", __FUNCTION__, SNMP_SHORT_FILE_NAME);
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    
    dal_convert_string_to_hex((guchar *)user_name, (guint32)username_len, (guchar *)hex_name, sizeof(hex_name));
    errno_t safe_fun_ret = strncat_s(hex_name, sizeof(hex_name), "00", strlen("00"));
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, stream) != NULL) {
        if (strstr(buf, (gchar *)hex_name) != NULL) {
            str_array = g_strsplit(buf, " ", SNMP_TOKENS_PER_LINE);
            if (str_array == NULL) {
                debug_log(DLOG_ERROR, "Parse snmp file failed!");
                continue;
            }

            guint8 str_length = g_strv_length(str_array);
            if (str_length != SNMP_TOKENS_PER_LINE) {
                debug_log(DLOG_DEBUG, "This line is not in the correct format!");
                g_strfreev(str_array);
                continue;
            }

            
            if (str_array[SNMP_USERNAMEHEX_INDEX_LINE] == NULL ||
                strlen(str_array[SNMP_USERNAMEHEX_INDEX_LINE]) < 2 ||
                g_strcmp0(str_array[SNMP_USERNAMEHEX_INDEX_LINE] + 2, hex_name) != 0) {
                g_strfreev(str_array);
                continue;
            }

            __copy_info_from_file(user_info, str_array, str_length);

            g_strfreev(str_array);
            break;
        }

        (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
    }

    
    fm_fclose(stream);
    g_mutex_unlock(&g_snmp_mutex);

    return RET_OK;
}

LOCAL gint32 __check_snmp_auth_priv_info(SnmpUserInfoData user_info, guint8 snmp_auth, guint8 snmp_priv)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar snmp_auth_info[SNMP_AUTH_LEN + 1] = { 0 };
    gchar snmp_priv_info[SNMP_AUTH_LEN + 1] = { 0 };

    safe_fun_ret =
        strncpy_s(snmp_auth_info, sizeof(snmp_auth_info), snmp_auth_str[snmp_auth], sizeof(snmp_auth_info) - 1);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    safe_fun_ret =
        strncpy_s(snmp_priv_info, sizeof(snmp_priv_info), snmp_priv_str[snmp_priv], sizeof(snmp_priv_info) - 1);
    if (safe_fun_ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
    }

    
    ret = snmp_compare_auth_info((gchar *)user_info.snmp_auth, (guint32)sizeof(user_info.snmp_auth), snmp_auth_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: snmp_compare_auth_info snmp_auth fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    
    ret = snmp_compare_auth_info((gchar *)user_info.snmp_priv, (guint32)sizeof(user_info.snmp_priv), snmp_priv_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: snmp_compare_auth_info snmp_priv fail, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __check_snmp_pwd_info(SnmpUserInfoData user_info, GSList *input_list, const gchar *user_name,
    guint32 key_len, guint8 snmp_auth)
{
    gint32 ret;
    gchar hex_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1] = { 0 };
    gchar authkey[SNMP_ENGINEID_MAX_LEN + 1] = { 0 };
    gchar *user_pwd = NULL;
    gsize password_len = 0x00;
    SNMP_ADDITION_DATA snmp_data = { 0 };

    
    dal_convert_hex_to_string((guchar *)(user_info.engineid + SNMP_HEX_PREFIX_LEN), (guchar *)snmp_data.str_engineID,
        sizeof(snmp_data.str_engineID));

    snmp_data.snmp_auth = snmp_auth;
    snmp_data.pwd_key_type = SNMP_AUTH_TYPE;

    user_pwd = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 1), &password_len);
    if (user_pwd == NULL) {
        debug_log(DLOG_ERROR, "%s:Input NULL pointer!", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = get_snmp_pwd_key(user_name, user_pwd, &snmp_data, authkey, sizeof(authkey));
    clear_sensitive_info(user_pwd, password_len);
    free_mem_set_null(user_pwd);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get snmp passwd key failed!", __FUNCTION__);
        clear_sensitive_info(authkey, sizeof(authkey));
        return RET_ERR;
    }

    
    convert_bytes_to_string((guchar *)authkey, key_len, (guchar *)hex_pwd);
    clear_sensitive_info(authkey, sizeof(authkey));

    
    ret = snmp_compare_auth_info((gchar *)(user_info.pwd + 2), (guint32)(sizeof(user_info.pwd) - 2), (gchar *)hex_pwd);
    clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:snmp_compare_auth_info snmp passwd failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __check_snmp_priv_pwd_info(SnmpUserInfoData user_info, GSList *input_list, const gchar *user_name,
    guint32 key_len, guint8 snmp_auth)
{
    gint32 ret;
    gchar *ptr_privacy_pwd = NULL;
    gsize privacy_password_len = 0x00;
    gchar privkey[SNMP_ENGINEID_MAX_LEN + 1] = { 0 };
    gchar hex_privacy_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1] = { 0 };
    SNMP_ADDITION_DATA snmp_data = { 0 };

    
    dal_convert_hex_to_string((guchar *)(user_info.engineid + SNMP_HEX_PREFIX_LEN), (guchar *)snmp_data.str_engineID,
        sizeof(snmp_data.str_engineID));

    snmp_data.snmp_auth = snmp_auth;
    snmp_data.pwd_key_type = SNMP_PRIV_TYPE;

    ptr_privacy_pwd = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 2), &privacy_password_len);
    if (ptr_privacy_pwd == NULL) {
        debug_log(DLOG_ERROR, "%s:Input NULL pointer!", __FUNCTION__);
        return RET_ERR;
    }

    // 加密密码
    ret = get_snmp_pwd_key(user_name, ptr_privacy_pwd, &snmp_data, privkey, sizeof(privkey));
    clear_sensitive_info(ptr_privacy_pwd, privacy_password_len);
    free_mem_set_null(ptr_privacy_pwd);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get snmp privacy passwd key failed!", __FUNCTION__);
        clear_sensitive_info(privkey, sizeof(privkey));
        return RET_ERR;
    }

    
    convert_bytes_to_string((guchar *)privkey, key_len, (guchar *)hex_privacy_pwd);
    clear_sensitive_info(privkey, sizeof(privkey));

    
    ret = snmp_compare_auth_info((gchar *)(user_info.privacy_pwd + 2), (guint32)(sizeof(user_info.privacy_pwd) - 2),
        (gchar *)hex_privacy_pwd);
    clear_sensitive_info(hex_privacy_pwd, sizeof(hex_privacy_pwd));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:snmp_compare_auth_info snmp privacy passwd failed, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __get_snmp_protocol_info(const gchar *user_name, guint8 *snmp_auth, guint8 *snmp_priv, guint32 *key_len)
{
    gint32 ret;

    ret = snmp_get_auth_protocol(user_name, snmp_auth);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:snmp_get_auth_protocol failed!", __FUNCTION__);
        return RET_ERR;
    }

    ret = snmp_get_priv_protocol(user_name, snmp_priv);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:snmp_get_priv_protocol failed!", __FUNCTION__);
        return RET_ERR;
    }

    // 判断鉴权算法类型，并返回HASH密文长度
    *key_len = sc_get_properlength1(*snmp_auth, 0);
    if (*key_len <= 0) {
        debug_log(DLOG_ERROR, "%s:Key lenth error!", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}

LOCAL gint32 __check_snmp_engineid(SnmpUserInfoData user_info)
{
    gint32 ret;

    if (user_info.engineid[0] == '\0') {
        debug_log(DLOG_ERROR, "%s:Get engineid failed!", __FUNCTION__);
        return RET_ERR;
    }

    
    ret = snmp_compare_oldengineid(user_info.engineid);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Compare oldengineid failed!", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 CompareSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    guint32 key_len = 0;
    guint8 snmp_auth = 0;
    guint8 snmp_priv = 1;
    gsize username_len = 0;
    SnmpUserInfoData user_info = { 0 };

    if (input_list == NULL) {
        return RET_ERR;
    }

    gchar *user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), &username_len);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s:Input NULL pointer!", __FUNCTION__);
        return RET_ERR;
    }

    gint32 ret = __get_snmp_protocol_info(user_name, &snmp_auth, &snmp_priv, &key_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __get_snmp_protocol_info fail, ret = %d", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    ret = __get_info_from_conf(user_name, username_len, &user_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __get_info_from_conf fail, ret = %d", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    ret = __check_snmp_engineid(user_info);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __check_snmp_engineid fail, ret = %d", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    ret = __check_snmp_pwd_info(user_info, input_list, user_name, key_len, snmp_auth);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __check_snmp_pwd_info fail, ret = %d", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    ret = __check_snmp_priv_pwd_info(user_info, input_list, user_name, key_len, snmp_auth);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __check_snmp_priv_pwd_info fail, ret = %d", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    ret = __check_snmp_auth_priv_info(user_info, snmp_auth, snmp_priv);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: __check_snmp_auth_priv_info fail, ret = %d", __FUNCTION__, ret);
        goto ERROR_EXIT;
    }

    free_mem_set_null(user_name);
    return RET_OK;

ERROR_EXIT:
    free_mem_set_null(user_name);
    return RET_ERR;
}


gint32 AddSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 iRet = -1;
    errno_t safe_fun_ret = EOK;

    FM_FILE_S *stream = NULL;
    gchar engineid[SNMP_ENGINEID_MAX_LEN + 1] = { 0 };
    gchar hex_name[2 * SNMP_USERNAME_MAX_LEN + 2 + 2 + 1] = { 0 };
    gchar hex_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1] = { 0 };
    gchar hex_priv_pwd[SNMP_HEXPASSWORD_MAX_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 ret;
    gchar *user_name = NULL;
    gchar *user_pwd = NULL;
    gchar *ptr_privacy_pwd = NULL;
    gsize password_len = 0x00;
    gsize privacy_password_len = 0x00;
    gchar pwd[SNMP_MAX_PASSWORD_LEN + 1] = { 0 };
    gchar privacy_pwd[SNMP_MAX_PASSWORD_LEN + 1] = { 0 };
    guint8 snmp_auth = 0;
    guint8 snmp_priv = 1;
    gint32 new_buf_len = 0;
    gsize username_len = 0;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }

    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), &username_len);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:Input NULL pointer!", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    user_pwd = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 1), &password_len);
    if (user_pwd == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:Input NULL pointer!", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return RET_ERR;
    }
    safe_fun_ret = strncpy_s(pwd, sizeof(pwd), user_pwd, SNMP_MAX_PASSWORD_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    clear_sensitive_info(user_pwd, password_len);

    ptr_privacy_pwd = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 2), &privacy_password_len);
    if (ptr_privacy_pwd == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:Input NULL pointer!", __FUNCTION__, __LINE__);
        (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
        free_mem_set_null(user_name);
        free_mem_set_null(user_pwd);
        return RET_ERR;
    }
    safe_fun_ret = strncpy_s(privacy_pwd, sizeof(privacy_pwd), ptr_privacy_pwd, SNMP_MAX_PASSWORD_LEN);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }
    clear_sensitive_info(ptr_privacy_pwd, privacy_password_len);

    ret = snmp_get_auth_protocol((const gchar *)user_name, &snmp_auth);
    if ((ret != RET_OK) || (snmp_auth > USE_SHA512)) {
        debug_log(DLOG_ERROR, "%s:%d:snmp_get_auth_protocol failed!\n", __FUNCTION__, __LINE__);
        
        (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
        clear_sensitive_info(privacy_pwd, sizeof(privacy_pwd));
        goto ERROR_EXIT;
    }

    ret = snmp_get_priv_protocol((const gchar *)user_name, &snmp_priv);
    if ((ret != RET_OK) || (snmp_priv > USE_AES256)) {
        debug_log(DLOG_ERROR, "%s:%d:snmp_get_priv_protocol failed!\n", __FUNCTION__, __LINE__);
        
        (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
        clear_sensitive_info(privacy_pwd, sizeof(privacy_pwd));
        goto ERROR_EXIT;
    }

    
    ret = get_snmp_info(user_name, (guint32)username_len, pwd, privacy_pwd, engineid, sizeof(engineid), hex_name,
        sizeof(hex_name), hex_pwd, (gint32)snmp_auth, hex_priv_pwd, (gint32)snmp_priv);
    
    (void)memset_s(pwd, sizeof(pwd), 0, sizeof(pwd));
    clear_sensitive_info(privacy_pwd, sizeof(privacy_pwd));

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:get_snmp_info failed!\n", __FUNCTION__, __LINE__);
        clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
        clear_sensitive_info(hex_priv_pwd, sizeof(hex_priv_pwd));
        goto ERROR_EXIT;
    }

    g_mutex_lock(&g_snmp_mutex);
    stream = fm_fopen(SNMP_SHORT_FILE_NAME, "a+");
    if (stream == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
        clear_sensitive_info(hex_priv_pwd, sizeof(hex_priv_pwd));
        goto ERROR_EXIT;
    }

    
    (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);

    
    iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "usmUser 1 3 %s 0x%s 0x%s NULL %s 0x%s %s 0x%s \"\"\n",
        engineid, hex_name, hex_name, snmp_auth_str[snmp_auth], hex_pwd, snmp_priv_str[snmp_priv], hex_priv_pwd);
    if (iRet <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
    }
    clear_sensitive_info(hex_pwd, sizeof(hex_pwd));
    clear_sensitive_info(hex_priv_pwd, sizeof(hex_priv_pwd));
    new_buf_len = strlen(buf);
    ret = fm_fwrite(buf, 1, new_buf_len, stream);
    if (ret != new_buf_len) {
        debug_log(DLOG_ERROR, "Call fm_fwrite failed\n");
        fm_fclose(stream);
        g_mutex_unlock(&g_snmp_mutex);
        goto ERROR_EXIT;
    }

    
    fm_fclose(stream);
    g_mutex_unlock(&g_snmp_mutex);
    free_mem_set_null(user_name);
    free_mem_set_null(user_pwd);
    free_mem_set_null(ptr_privacy_pwd);
    return RET_OK;

ERROR_EXIT:
    free_mem_set_null(user_name);
    free_mem_set_null(user_pwd);
    free_mem_set_null(ptr_privacy_pwd);
    return RET_ERR;
}


LOCAL gint32 trim_null_char(gchar *buffer, guint32 max_buffer_len, FM_FILE_S *stream, gint32 p_start, gchar **trimmed)
{
    guint32 start = 0;
    gint32 p_end;
    gint32 del_len;
    gint32 ret;

    // 第一个非0字符位置
    while (buffer[start] == 0 && start < max_buffer_len) {
        start++;
    }
    if (start == max_buffer_len || buffer[start] == '\n') {
        // 全是空字符，删除
        ret = fm_delete(start + 1, p_start, stream);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "fm_delete failed, ret = %d", ret);
        }
        ret = fm_fseek(stream, p_start, SEEK_SET);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "fm_fseek failed, ret = %d", ret);
        }
        debug_log(DLOG_DEBUG, "All null chars");
        return RET_ERR;
    }

    *trimmed = buffer + start;
    p_end = fm_ftell(stream);
    del_len = p_end - p_start;
    // 无论是否成功，都将文件指针后移
    ret = fm_modify(*trimmed, (gsize)strlen(*trimmed), (gsize)p_start, (gsize)del_len, stream);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "fm_modify failed, ret = %d", ret);
    }
    ret = fm_fseek(stream, p_start + strlen(*trimmed), SEEK_SET);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "fm_fseek failed, ret = %d", ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 get_user_privilege(gchar **str_array, guint8 str_length, UI_MANAGE_USER_S *snmp_user_info, gint32 *user_id)
{
    if (strcmp(str_array[0], SNMP_RWUSER) == 0 || strcmp(str_array[0], SNMP_ROUSER) == 0) {
        
        if (str_length == SNMP_RWUSER_ADMIN_LEN) {
            snmp_user_info[*user_id].privilege[1] = RMCP_ADMIN;
        } else {
            if (strcmp(str_array[SNMP_RWUSER_ROLE_INDEX], VIEW_OPERATOR) == 0) {
                snmp_user_info[*user_id].privilege[1] = RMCP_OPERATOR;
            } else if (strcmp(str_array[SNMP_RWUSER_ROLE_INDEX], VIEW_USER) == 0) {
                snmp_user_info[*user_id].privilege[1] = RMCP_USER;
            } else {
                return RET_ERR;
            }
        }
        (void)strncpy_s(snmp_user_info[*user_id].user_name, SNMP_MAX_NAME_LEN + 1, str_array[1], SNMP_MAX_NAME_LEN);
        snmp_user_info[*user_id].enable_state = 1;
        *user_id = *user_id + 1;
    }

    return RET_OK;
}


gint32 GetSnmpUserInfo(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    FM_FILE_S *fp = NULL;
    gchar buf[SNMP_CONF_MAXLINE_UNTRIMMED + 1] = { 0 };
    gint32 i = 0;
    gint32 count = 0;
    gint32 sump_user_max_num_count = 2 * SNMP_USER_MAX_NUM;
    UI_MANAGE_USER_S *snmp_user_info = (UI_MANAGE_USER_S *)g_malloc0(sump_user_max_num_count *
        sizeof(UI_MANAGE_USER_S));
    if (snmp_user_info == NULL) {
        debug_log(DLOG_ERROR, "priv_data malloc is NULL\n");
        return RET_ERR;
    }
    gchar **str_array = NULL;
    guint8 str_length = 0;
    gint32 p_start = 0;
    gchar *trimmed = NULL;
    gint32 ret;

    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Get SnmpUserInfo failed, output_list can not be NULL.\r\n");
        g_free(snmp_user_info);
        return RET_ERR;
    }

    (void)memset_s(snmp_user_info, sizeof(UI_MANAGE_USER_S) * sump_user_max_num_count, 0,
        sizeof(UI_MANAGE_USER_S) * sump_user_max_num_count);

    g_mutex_lock(&g_snmp_mutex);

    
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        g_mutex_unlock(&g_snmp_mutex);
        g_free(snmp_user_info);
        return RET_ERR;
    }

    
    while ((fm_fgets(buf, SNMP_CONF_MAXLINE_UNTRIMMED + 1, fp) != NULL) && (i < 2 * SNMP_USER_MAX_NUM)) {
        trimmed = buf;
        if (buf[0] == 0) {
            ret = trim_null_char(buf, SNMP_CONF_MAXLINE_UNTRIMMED + 1, fp, p_start, &trimmed);
            if (ret != RET_OK) {
                debug_log(DLOG_DEBUG, "trim null characters failed.");
            }
        }

        dal_trim_string(trimmed, SNMP_CONF_MAXLINE_UNTRIMMED + 1);

        str_array = g_strsplit(trimmed, " ", 5);
        if (str_array == NULL) {
            goto NEXT_LINE;
        }

        str_length = g_strv_length(str_array);
        if (str_length != 3 && str_length != 5) {
            (void)memset_s(buf, SNMP_CONF_MAXLINE_UNTRIMMED + 1, 0, SNMP_CONF_MAXLINE_UNTRIMMED + 1);
            g_strfreev(str_array);
            goto NEXT_LINE;
        }

        ret = get_user_privilege(str_array, str_length, snmp_user_info, &i);
        if (ret != RET_OK) {
            (void)memset_s(buf, SNMP_CONF_MAXLINE_UNTRIMMED + 1, 0, SNMP_CONF_MAXLINE_UNTRIMMED + 1);
            g_strfreev(str_array);
            goto NEXT_LINE;
        }

        (void)memset_s(buf, SNMP_CONF_MAXLINE_UNTRIMMED + 1, 0, SNMP_CONF_MAXLINE_UNTRIMMED + 1);
        g_strfreev(str_array);

    NEXT_LINE:
        p_start = fm_ftell(fp);
        if (p_start < 0) {
            debug_log(DLOG_ERROR, "%s: fm_ftell failed.", __FUNCTION__);
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            g_free(snmp_user_info);
            return RET_ERR;
        }
    }

    
    count = i;

    
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);

    *output_list = g_slist_append(*output_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, snmp_user_info,
        sizeof(UI_MANAGE_USER_S) * count, sizeof(guint8)));
    *output_list = g_slist_append(*output_list, g_variant_new_int32(count));
    g_free(snmp_user_info);
    return RET_OK;
}

gint32 GetSnmpUserHexInfo(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 i = 0;
    gint sump_user_max_num_count = 2 * SNMP_USER_MAX_NUM;

    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "Get SnmpUserHexInfo failed, output_list can not be NULL");
        return RET_ERR;
    }

    UI_MANAGE_USER_S *snmp_user_info = (UI_MANAGE_USER_S *)g_malloc(sump_user_max_num_count * sizeof(UI_MANAGE_USER_S));
    if (snmp_user_info == NULL) {
        debug_log(DLOG_ERROR, "priv_data malloc is NULL\n");
        return RET_ERR;
    }

    (void)memset_s(snmp_user_info, sizeof(UI_MANAGE_USER_S) * sump_user_max_num_count, 0,
        sizeof(UI_MANAGE_USER_S) * sump_user_max_num_count);

    
    g_mutex_lock(&g_snmp_mutex);
    FM_FILE_S *stream = fm_fopen(SNMP_SHORT_FILE_NAME, "r");
    if (stream == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        g_free(snmp_user_info);
        return RET_ERR;
    }

    while ((fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, stream) != NULL) && (i < 2 * SNMP_USER_MAX_NUM)) {
        gchar **str_array = g_strsplit(buf, " ", SNMP_TOKENS_PER_LINE);
        if (str_array == NULL) {
            continue;
        }

        guint8 str_length = g_strv_length(str_array);
        if (str_length != SNMP_TOKENS_PER_LINE) {
            g_strfreev(str_array);
            continue;
        }

        if (strcmp(str_array[0], "usmUser") != 0 || str_array[SNMP_USERNAMEHEX_INDEX_LINE] == NULL ||
            strlen(str_array[SNMP_USERNAMEHEX_INDEX_LINE]) < SNMP_HEX_PREFIX_LEN) {
            g_strfreev(str_array);
            continue;
        }

        dal_convert_hex_to_string((guchar *)(str_array[SNMP_USERNAMEHEX_INDEX_LINE] + 2),
            (guchar *)snmp_user_info[i].user_name, sizeof(snmp_user_info[i].user_name));
        g_strfreev(str_array);
        (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
        i++;
    }

    
    gint32 count = i;

    
    fm_fclose(stream);
    g_mutex_unlock(&g_snmp_mutex);

    if (count == 0) {
        debug_log(DLOG_DEBUG, "no user in snmpd.conf file\r\n");
        g_free(snmp_user_info);
        return RET_ERR;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, snmp_user_info,
        sizeof(UI_MANAGE_USER_S) * count, sizeof(guint8)));
    *output_list = g_slist_append(*output_list, g_variant_new_int32(count));
    g_free(snmp_user_info);
    return RET_OK;
}


LOCAL gint32 find_spec_snmp_user_line(const gchar* hex_name, const gchar* line_buff)
{
    _cleanup_gstrv_ gchar **str_array = g_strsplit(line_buff, " ", SNMP_TOKENS_PER_LINE);
    if (str_array == NULL) {
        debug_log(DLOG_ERROR, "%s: prase snmp file fail", __FUNCTION__);
        return RET_ERR;
    }

    guint str_length = g_strv_length(str_array);
    if (str_length != SNMP_TOKENS_PER_LINE) {
        debug_log(DLOG_ERROR, "%s: This line is not in the correct format", __FUNCTION__);
        return RET_ERR;
    }

    gchar* user_name = str_array[SNMP_USERNAMEHEX_INDEX_LINE];
    if (strlen(user_name) < 2) { // nmpd.conf中hex_name的格式为0x313200，比较时需要屏蔽前2个字符"0x"
        debug_log(DLOG_ERROR, "%s: hex_user_name_format error", __FUNCTION__);
        return RET_ERR;
    }
    // 蔽前2个字符"0x"
    if (g_strcmp0(user_name + 2, hex_name) != 0) {
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 snmp_del_passwd_from_file(gchar *username, guint32 username_len)
{
    errno_t safe_fun_ret = EOK;
    FM_FILE_S *fp = NULL;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 found = 0;
    gint32 p_start = 0;
    gint32 p_end = 0;
    gint32 buf_len = 0;
    gint32 ret;
    gchar hex_name[2 * SNMP_USERNAME_MAX_LEN + 2 + 2 + 1] = { 0 };

    

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    dal_convert_string_to_hex((guchar *)username, username_len, (guchar *)hex_name, sizeof(hex_name));
    safe_fun_ret = strncat_s(hex_name, sizeof(hex_name), "00", strlen("00"));
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
    }

    (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(buf, hex_name) != NULL) {
            buf_len = strlen(buf);

            if (find_spec_snmp_user_line(hex_name, buf) != RET_OK) {
                (void)memset_s(buf, 1 + SNMP_CONF_MAXLINE_LEN, 0, SNMP_CONF_MAXLINE_LEN + 1);
                continue;
            }

            found = 1;
            p_end = fm_ftell(fp);
            p_start = p_end - buf_len;
            break;
        }

        (void)memset_s(buf, SNMP_CONF_MAXLINE_LEN + 1, 0, SNMP_CONF_MAXLINE_LEN + 1);
    }

    if (found == 0) {
        (void)fm_ftell(fp);
    } else {
        ret = fm_delete((gsize)buf_len, (gsize)p_start, fp);
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "Call fm_delete failed!\n");
            fm_fclose(fp);
            g_mutex_unlock(&g_snmp_mutex);
            return RET_ERR;
        }
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_OK;
}


gint32 DelSnmpUserPassword(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret = 0;
    gchar *user_name = NULL;
    gsize username_len = 0;
    if (input_list == NULL) {
        debug_log(DLOG_ERROR, "Input_list can not be NULL.\r\n");
        return RET_ERR;
    }

    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), &username_len);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:User_name is NULL.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    ret = snmp_del_passwd_from_file(user_name, (guint32)username_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:%d:snmp_del_passwd_from_file failed.", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    free_mem_set_null(user_name);
    return RET_OK;
}


gint32 DeleteNoAccessSnmpUser(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    FM_FILE_S *fp = NULL;
    gchar *user_name = NULL;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar *p = NULL;
    gint32 p_start = 0;
    gint32 p_end = 0;
    gint32 del_len = 0;
    gint32 ret = 0;
    gchar *pch = NULL;
    gchar *end = NULL;

    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:User_name is NULL.", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    g_mutex_lock(&g_snmp_mutex);

    
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:%d:snmpuser_fgetnam failed.", __FUNCTION__, __LINE__);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    
    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        p = strtok_s(buf, " ", &end);
        if (p && (strcmp(p, SNMP_RWUSER) == 0)) {
            p = strtok_s(end, " ", &end);
            if (p && (strlen(p) == strlen(user_name))) {
                if (memcmp(p, user_name, strlen(p)) == 0) {
                    
                    pch = fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp);
                    if (pch == NULL) {
                        fm_fclose(fp);
                        g_mutex_unlock(&g_snmp_mutex);
                        free_mem_set_null(user_name);
                        return RET_ERR;
                    }

                    p_end = fm_ftell(fp);
                    del_len = p_end - p_start;
                    break;
                }
            }
        }

        p_start = fm_ftell(fp);
    }

    ret = fm_delete(del_len, p_start, fp);
    if (ret == RET_ERR) {
        fm_fclose(fp);
        debug_log(DLOG_ERROR, "Call fm_delete failed!\n");
        g_mutex_unlock(&g_snmp_mutex);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    free_mem_set_null(user_name);
    return RET_OK;
}


LOCAL gint32 get_snmp_delete_length(const gchar *user_name, FM_FILE_S *fp, gint32 *p_start, gint32 *del_len,
    const gchar *file_user_name)
{
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 p_end = 0;
    const gint32 success_flag = 2;
    gchar *pch = NULL;

    if (file_user_name && (strlen(file_user_name) == strlen(user_name)) &&
        (memcmp(file_user_name, user_name, strlen(file_user_name)) == 0)) {
        
        pch = fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp);
        if (pch == NULL) {
            return RET_ERR;
        }
        p_end = fm_ftell(fp);
        *del_len = p_end - *p_start;
        return success_flag;
    }
    return RET_OK;
}


#define SUCCESS_FLAG 2
LOCAL gint32 get_snmp_file_info(const gchar *user_name, FM_FILE_S *fp, gint32 *p_start, gint32 *del_len)
{
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar *end = NULL;
    gchar *p = NULL;
    gint32 ret;

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        p = strtok_s(buf, " ", &end);
        if (p && (strcmp(p, SNMP_RWUSER) == 0 || strcmp(p, SNMP_ROUSER) == 0)) {
            p = strtok_s(end, " ", &end);
            ret = get_snmp_delete_length(user_name, fp, p_start, del_len, p);
            if (ret == RET_ERR) {
                return RET_ERR;
            }
            if (ret == SUCCESS_FLAG) {
                break;
            }
        }

        *p_start = fm_ftell(fp);
    }
    return RET_OK;
}


LOCAL gint32 snmp_add_privilege_info(const gchar *user_name, guint8 user_priv)
{
    gint32 iRet = -1;
    FM_FILE_S *fp = NULL;
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gint32 p_start = 0;
    gint32 del_len = 0;
    gint32 ret;

    g_mutex_lock(&g_snmp_mutex);
    
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    ret = get_snmp_file_info(user_name, fp, &p_start, &del_len);
    if (ret != RET_OK) {
        fm_fclose(fp);
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }

    if (user_priv != RMCP_ADMIN && user_priv != RMCP_OPERATOR && user_priv != RMCP_USER) {
        ret = fm_delete(del_len, p_start, fp);
    } else {
        if (user_priv == RMCP_ADMIN) {
            iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "rwuser %s priv\ngroup gmyuser usm %s\n", user_name,
                user_name);
        } else if (user_priv == RMCP_OPERATOR) {
            iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "rwuser %s priv -V %s\ngroup %s usm %s\n", user_name,
                VIEW_OPERATOR, GROUP_OPERATOR, user_name);
        } else if (user_priv == RMCP_USER) {
            iRet = snprintf_s(buf, sizeof(buf), sizeof(buf) - 1, "rwuser %s priv -V %s\ngroup %s usm %s\n", user_name,
                VIEW_USER, GROUP_USER, user_name);
        }
        if (iRet <= 0) {
            debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet);
        }
        ret = fm_modify(buf, strlen(buf), p_start, del_len, fp);
    }

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "Call fm_delete/fm_modify failed, ret = %d", ret);
        fm_fclose(fp);
        g_mutex_unlock(&g_snmp_mutex);
        return RET_ERR;
    }
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);
    return RET_OK;
}


gint32 SetSnmpUserPrivilege(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gchar *user_name = NULL;
    guint8 user_priv;
    gint32 ret;

    
    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "Input NULL pointer!");
        return RET_ERR;
    }

    if (get_snmpv3_status() == SNMP_STATE_DISABLE) {
        debug_log(DLOG_INFO, "[%s] snmpv3 is disabled, config won't be wrote.", __FUNCTION__);
        free_mem_set_null(user_name);
        return RET_OK;
    }
    user_priv = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 1));
    
    
    if (strlen(user_name) > SNMP_USERNAME_MAX_LEN) {
        debug_log(DLOG_ERROR, "User name length error!");
        free_mem_set_null(user_name);
        return RET_ERR;
    }
    
    if (snmp_check_username_exist(user_name) != SNMP_USERNAME_EXIST) {
        debug_log(DLOG_ERROR, "User name is not exist!");
        free_mem_set_null(user_name);
        return RET_ERR;
    }
    
    if (!((user_priv >= RMCP_USER && user_priv <= RMCP_ADMIN) || (user_priv == 15))) {
        debug_log(DLOG_ERROR, "User privilege error!");
        free_mem_set_null(user_name);
        return RET_ERR;
    }
    ret = snmp_add_privilege_info(user_name, user_priv);
    free_mem_set_null(user_name);
    return ret;
}


LOCAL gint32 replace_hexname_of_buffer(const gchar *input_buf, const gchar *old_hex_name, const gchar *new_hex_name,
    gchar *output_buf, guint32 output_len)
{
    gint32 ret;

    if (output_len == 0) { 
        debug_log(DLOG_ERROR, "%s invalid output_len", __FUNCTION__);
        return RET_ERR;
    }

    gchar **str_array = g_strsplit(input_buf, " ", SNMP_TOKENS_PER_LINE);
    if (str_array == NULL) {
        debug_log(DLOG_ERROR, "Prase snmp file failed!");
        return RET_ERR;
    }

    if (g_strv_length(str_array) != SNMP_TOKENS_PER_LINE) {
        debug_log(DLOG_ERROR, "This line is not in the correct format!");
        g_strfreev(str_array);
        return RET_ERR;
    }

    
    if (str_array[SNMP_USERNAMEHEX_INDEX_LINE] == NULL ||
        strlen(str_array[SNMP_USERNAMEHEX_INDEX_LINE]) < SNMP_HEX_PREFIX_LEN ||
        strcmp(old_hex_name, str_array[SNMP_USERNAMEHEX_INDEX_LINE] + SNMP_HEX_PREFIX_LEN) != 0) {
        g_strfreev(str_array);
        return RET_ERR;
    }

    
    ret = snprintf_s(output_buf, output_len, output_len - 1, "usmUser 1 3 %s 0x%s 0x%s NULL %s %s %s %s \"\"\n",
        str_array[SNMP_OLDENGINEID_INDEX_LINE], new_hex_name, new_hex_name, str_array[SNMP_AUTHPROCOTOL_INDEX_LINE],
        str_array[SNMP_USERPASSWDHEX_LINE], str_array[SNMP_PRIVPROCOTOL_INDEX_LINE],
        str_array[SNMP_PRIVPASSWDHEX_LINE]);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret);
        g_strfreev(str_array);
        return RET_ERR;
    }
    g_strfreev(str_array);
    return RET_OK;
}


LOCAL gint32 snmp_update_pwd_by_rename(gchar *old_user_name, gint32 old_len, gchar *new_user_name, gint32 new_len)
{
    errno_t safe_fun_ret;
    gint32 ret;
    gchar old_hex_name[SNMP_HEX_USERNAME_LEN + 1] = { 0 };
    gchar new_hex_name[SNMP_HEX_USERNAME_LEN + 1] = { 0 };
    gchar buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    gchar new_buf[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    FM_FILE_S *fp = NULL;

    dal_convert_string_to_hex((guchar *)old_user_name, (guint32)old_len, (guchar *)old_hex_name, sizeof(old_hex_name));
    safe_fun_ret = strcat_s(old_hex_name, sizeof(old_hex_name), "00");
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }
    dal_convert_string_to_hex((guchar *)new_user_name, (guint32)new_len, (guchar *)new_hex_name, sizeof(new_hex_name));
    safe_fun_ret = strcat_s(new_hex_name, sizeof(new_hex_name), "00");
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    g_mutex_lock(&g_snmp_mutex);
    fp = fm_fopen(SNMP_SHORT_FILE_NAME, "r+");
    if (fp == NULL) {
        g_mutex_unlock(&g_snmp_mutex);
        debug_log(DLOG_ERROR, "%s:fail to open %s", __FUNCTION__, SNMP_SHORT_FILE_NAME);
        return RET_ERR;
    }

    while (fm_fgets(buf, SNMP_CONF_MAXLINE_LEN + 1, fp) != NULL) {
        if (strstr(buf, old_hex_name) != NULL) {
            ret = replace_hexname_of_buffer(buf, old_hex_name, new_hex_name, new_buf, sizeof(new_buf));
            if (ret == RET_OK) { 
                break;
            }
        }
        (void)memset_s(buf, sizeof(buf), 0, sizeof(buf));
    }

    
    (void)fm_fseek(fp, 0, SEEK_END);
    ret = fm_fwrite(new_buf, sizeof(gchar), sizeof(new_buf), fp);
    fm_fclose(fp);
    g_mutex_unlock(&g_snmp_mutex);

    if (ret != sizeof(new_buf)) {
        debug_log(DLOG_ERROR, "fm_fwrite failed, expect size %zu, actual size %d", sizeof(new_buf), ret);
        return RET_ERR;
    }
    return RET_OK;
}


gint32 UpdateSnmpUserPasswordByRename(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    gchar *old_user_name = NULL;
    gchar *user_name = NULL;

    if (input_list == NULL || g_slist_length(input_list) != 2) { 
        debug_log(DLOG_ERROR, "%s:input_list is NULL, or input len invalid", __FUNCTION__);
        return RET_ERR;
    }

    old_user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 0), NULL);
    if (old_user_name == NULL) {
        debug_log(DLOG_ERROR, "%s: old user name is NULL", __FUNCTION__);
        return RET_ERR;
    }
    user_name = g_variant_dup_string((GVariant *)g_slist_nth_data(input_list, 1), NULL);
    if (user_name == NULL) {
        debug_log(DLOG_ERROR, "%s: user name is NULL", __FUNCTION__);
        free_mem_set_null(old_user_name);
        return RET_ERR;
    }

    if (strlen(old_user_name) == 0 || strlen(user_name) == 0) {
        debug_log(DLOG_ERROR, "%s: user name is invalid, (%s->%s)", __FUNCTION__, old_user_name, user_name);
        free_mem_set_null(old_user_name);
        free_mem_set_null(user_name);
        return RET_ERR;
    }

    if (strcmp(old_user_name, user_name) == 0) {
        debug_log(DLOG_ERROR, "%s: two user name(%s) is same", __FUNCTION__, old_user_name);
        free_mem_set_null(old_user_name);
        free_mem_set_null(user_name);
        return RET_OK;
    }

    ret = snmp_update_pwd_by_rename(old_user_name, strlen(old_user_name), user_name, strlen(user_name));
    debug_log(DLOG_INFO, "%s: update pwd, rename(%s->%s), ret %d", __FUNCTION__, old_user_name, user_name, ret);

    free_mem_set_null(old_user_name);
    free_mem_set_null(user_name);
    return ret;
}
