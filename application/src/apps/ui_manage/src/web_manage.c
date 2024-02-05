
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "web_manage.h"
#include "snmp_manage_app.h"

G_LOCK_DEFINE(g_web_service_config_lock);

LOCAL gint32 g_web_server_state = VHOST_START;

LOCAL guchar g_anti_dns_rebind = 0;
LOCAL gchar g_hostname[HOST_NAME_MAX_LEN + 1] = { 0 };
LOCAL gchar g_domainname[DNS_DOMAINNAME_MAX_LEN + 1] = { 0 };

LOCAL gboolean g_is_writing_default_server = FALSE;
void stop_webserver(void);
LOCAL void get_https_configuration(HTTPS_CONF_S *configure);
LOCAL void write_out_service_listen_addr(FM_FILE_S *fp, HTTPS_CONF_S https_conf, gboolean *is_listen_default);
LOCAL void write_inner_service_listen_addr(FM_FILE_S *fp, HTTPS_CONF_S configure, gboolean is_listen_default);
LOCAL void write_service_listen_addr(FM_FILE_S *fp, const gchar *ipaddr, guint16 port, const gchar *eth_name,
    gboolean is_https);

#define HTTPS_TLS_CONFIG_MASK             0x01
#define HTTP_CONFIG_MASK                  0x02
#define HTTPS_CONFIG_MASK                 0x04
#define HTTPS_VERIFY_CLIENT_CONFIG_MASK   0x08
#define SSL_STR_LEN                       64
#define MAX_PLAINTEXT_BUF_SIZE            (1024 * 1024)

guint32 g_webserver_update_flag = 0;

LOCAL void set_webserver_update_flag(guint32 mask)
{
    G_LOCK(g_web_service_config_lock);
    g_webserver_update_flag |= mask;
    G_UNLOCK(g_web_service_config_lock);
}

LOCAL void clear_webserver_update_flag(guint32 mask)
{
    G_LOCK(g_web_service_config_lock);
    g_webserver_update_flag &= ~mask;
    G_UNLOCK(g_web_service_config_lock);
}

LOCAL gboolean get_webserver_update_flag(guint32 mask)
{
    guint32 val;
    G_LOCK(g_web_service_config_lock);
    val = g_webserver_update_flag & mask;
    G_UNLOCK(g_web_service_config_lock);

    return val > 0;
}


LOCAL OBJ_HANDLE get_active_eth_obj(void)
{
    OBJ_HANDLE obj_eth_group = 0;
    OBJ_HANDLE obj_eth = 0;
    guint8 active_port = 0;
    gint32 ret;

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUTTER_GROUP_TYPE,
        &obj_eth_group);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get outter eth group object failed, ret:%d", __FUNCTION__, ret);
        return obj_eth;
    }

    (void)dal_get_property_value_byte(obj_eth_group, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, &active_port);

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_NUM, active_port, &obj_eth);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get active Eth object handle, ret:%d", __FUNCTION__, ret);
    }

    return obj_eth;
}

LOCAL gboolean __ip_changed(OBJ_HANDLE obj_handle, const gchar *obj_name)
{
    guint8 software_type = 0xff;
    (void)dal_get_software_type(&software_type);
    return (check_object_is_specific_out_type(obj_handle, OUTTER_GROUP_TYPE) == RET_OK) ||
        (check_object_is_specific_out_type(obj_handle, VETH_GROUP_TYPE) == RET_OK) ||
        (check_object_is_specific_out_type(obj_handle, MAINT_DEDICATED_ETHERNET) == RET_OK) ||
        (check_object_is_specific_out_type(obj_handle, RACK_INNER_COMM_ETHERNET) == RET_OK) ||
        (check_object_is_specific_out_type(obj_handle, RACK_NODE_INNER_ETHERNET) == RET_OK) ||
        (strcmp(obj_name, OBJECT_CLASS_SMS) == 0) || (strcmp(obj_name, FLOAT_ETHGROUP_OBJ_NAME) == 0) ||
        (strcmp(obj_name, ASM_OBJECT_NAME) == 0 && software_type != MGMT_SOFTWARE_TYPE_PANGEA_MGNT &&
        software_type != MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL) ||
        (obj_handle == get_active_eth_obj()) || (strcmp(obj_name, OBJ_USB_MGMT) == 0);
}

LOCAL gboolean __port_changed(OBJ_HANDLE obj_handle, const gchar *obj_name)
{
    return (strcmp(obj_name, WEBHTTPS_SERVER_PORT_OBJ_NAME) == 0) ||
        (check_object_is_specific_out_type(obj_handle, INNER_GROUP_TYPE) == RET_OK);
}

gint32 ConfigureVirtualHost(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    const gchar *obj_name = NULL;

    if (g_https_enable_flag != SERVICE_START) {
        debug_log(DLOG_DEBUG, "[%s]the web enable flag is %d\n", __FUNCTION__, g_https_enable_flag);
        return RET_OK;
    }

    obj_name = dfl_get_object_name(obj_handle);
    if (obj_name == NULL) {
        debug_log(DLOG_ERROR, "Get object name failed!\n");
        return RET_ERR;
    }

    debug_log(DLOG_DEBUG, "%s: update %s:%s.", __FUNCTION__, obj_name, property_name);

    
    if (strcmp(obj_name, OBJ_SECURITY_ENHANCE) == 0) {
        if (strcmp(property_name, PROPERTY_SECURITY_ANTI_DNS_REBIND) == 0) {
            set_webserver_update_flag(HTTPS_CONFIG_MASK);
        } else {
            set_webserver_update_flag(HTTPS_TLS_CONFIG_MASK);
        }
    }
    
    else if (__ip_changed(obj_handle, obj_name)) {
        set_webserver_update_flag(HTTP_CONFIG_MASK | HTTPS_CONFIG_MASK);
    
    } else if (strcmp(obj_name, MUTUAL_AUTH_OBJECT_NAME) == 0) {
        set_webserver_update_flag(HTTPS_TLS_CONFIG_MASK | HTTPS_VERIFY_CLIENT_CONFIG_MASK);
    
    } else if (__port_changed(obj_handle, obj_name)) {
        set_webserver_update_flag(HTTP_CONFIG_MASK | HTTPS_CONFIG_MASK);
    }
    
    else if (strcmp(obj_name, WEBHTTP_SERVER_PORT_OBJ_NAME) == 0) {
        set_webserver_update_flag(HTTP_CONFIG_MASK);
    } else if (strcmp(obj_name, CLASS_NAME_DNSSETTING) == 0) {
        set_webserver_update_flag(HTTPS_CONFIG_MASK);
    } else if (strcmp(obj_name, BMC_CLASEE_NAME) == 0) {
        set_webserver_update_flag(HTTPS_CONFIG_MASK);
    }

    return RET_OK;
}

gint32 MutualAuthRootCertChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    OBJ_HANDLE ma_handle = 0;
    guchar ma_enabled = ENABLED;

    if (g_https_enable_flag != SERVICE_START) {
        debug_log(DLOG_DEBUG, "[%s]the web enable flag is %d\n", __FUNCTION__, g_https_enable_flag);
        return RET_OK;
    }

    (void)dal_get_object_handle_nth(MUTUAL_AUTH_CLASS_NAME, 0, &ma_handle);
    (void)dal_get_property_value_byte(ma_handle, MUTUAL_AUTH_STATE, &ma_enabled);
    // 双因素未开启,证书操作不重启apache
    if (ma_enabled != ENABLED) {
        return RET_OK;
    }

    if (g_web_server_state == VHOST_START) {
        g_web_server_state = VHOST_RESTART;
    }
    debug_log(DLOG_DEBUG, "[%s]currently g_web_server_state is %d\n", __FUNCTION__, g_web_server_state);
    return RET_OK;
}


gint32 MutualAuthCrlFilePathChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    const gchar *crl_file_path = NULL;
    guchar ma_enabled = ENABLED;
    guchar crl_verify_enabled = ENABLED;
    OBJ_HANDLE ma_handle = 0;

    if (property_value == NULL) {
        return RET_OK;
    }

    crl_file_path = g_variant_get_string(property_value, 0);
    if (crl_file_path == NULL || strlen(crl_file_path) == 0) {
        return RET_OK;
    }

    
    (void)dal_get_object_handle_nth(MUTUAL_AUTH_CLASS_NAME, 0, &ma_handle);
    (void)dal_get_property_value_byte(ma_handle, MUTUAL_AUTH_STATE, &ma_enabled);
    (void)dal_get_property_value_byte(ma_handle, MUTUAL_AUTH_CRL, &crl_verify_enabled);

    if (ma_enabled == ENABLED && crl_verify_enabled == ENABLED) {
        return MutualAuthRootCertChangeCallback(obj_handle, user_data, property_name, property_value);
    }

    return RET_OK;
}


void service_rollback_web_server_configure(void)
{
    gint32 result = 0;
    FM_FILE_S *fp = NULL;
    gchar content[2048] = { 0 };

    fp = fm_fopen(HTTPS_CONF_NAME, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s:open configure failed.\n", __FUNCTION__);
        return;
    }

    (void)snprintf_s(content, sizeof(content), sizeof(content) - 1, "listen %d ssl;\r\n", VHOST_DEFAULT_OUTER_PORT);

    result = fm_fwrite(content, 1, strlen(content), fp);
    if (result != strlen(content)) {
        debug_log(DLOG_ERROR, "%s:write configure failed.\n", __FUNCTION__);
    }

    fm_fclose(fp);
}


LOCAL void set_webserver_log(void)
{
    FILE *fp = NULL;

    if (vos_check_dir(SHM_LOG_PATH) == FALSE) {
        (void)vos_mkdir_recursive(SHM_LOG_PATH);
    }
    (void)chmod(SHM_LOG_PATH, SHM_LOG_FILE_MOD);
    (void)chown(SHM_LOG_PATH, APACHE_UID, APACHE_GID);

    if (vos_check_dir(NGINX_LOG_PATH) == FALSE) {
        (void)vos_mkdir_recursive(NGINX_LOG_PATH);
    }
    (void)chmod(NGINX_LOG_PATH, SHM_LOG_FILE_MOD);
    (void)chown(NGINX_LOG_PATH, APACHE_UID, APACHE_GID);

    if (g_file_test(ACCESS_LOG, G_FILE_TEST_EXISTS) == FALSE) {
        fp = g_fopen(ACCESS_LOG, "w+");
        if (fp == NULL) {
            debug_log(DLOG_ERROR, "%s: create access_log file failed", __FUNCTION__);
        } else {
            (void)fclose(fp);
        }
    }
    (void)chmod(ACCESS_LOG, NGINX_LOG_MOD);
    (void)chown(ACCESS_LOG, APACHE_UID, APACHE_GID);

    if (g_file_test(ACCESS_LOG_BAK, G_FILE_TEST_EXISTS) == TRUE) {
        (void)chmod(ACCESS_LOG_BAK, NGINX_LOG_MOD);
    }

    if (g_file_test(ERROR_LOG, G_FILE_TEST_EXISTS) == FALSE) {
        fp = g_fopen(ERROR_LOG, "w+");
        if (fp == NULL) {
            debug_log(DLOG_ERROR, "%s: create error_log file failed", __FUNCTION__);
        } else {
            (void)fclose(fp);
        }
    }
    (void)chmod(ERROR_LOG, NGINX_LOG_MOD);
    (void)chown(ERROR_LOG, APACHE_UID, APACHE_GID);

    if (g_file_test(ERROR_LOG_BAK, G_FILE_TEST_EXISTS) == TRUE) {
        (void)chmod(ERROR_LOG_BAK, NGINX_LOG_MOD);
    }
}

LOCAL gint32 __write_to_pipe(const gchar *pipe, const gchar *buffer, guint32 length)
{
    gchar *real_path = realpath(pipe, NULL);
    if (real_path == NULL) {
        debug_log(DLOG_ERROR, "%s: check realpath failed", __FUNCTION__);
        return RET_ERR;
    }
    debug_log(DLOG_DEBUG, "ready to write pipe.");
    gint32 fd = open(real_path, O_WRONLY);
    if (fd < 0) {
        debug_log(DLOG_ERROR, "%s: open pipe failed: %d", __FUNCTION__, fd);
        free(real_path);
        real_path = NULL;
        return RET_ERR;
    }
    gint32 write_len = write(fd, buffer, length);
    close(fd);
    if (write_len != length) {
        debug_log(DLOG_ERROR, "%s: write pipe failed", __FUNCTION__);
        free(real_path);
        real_path = NULL;
        return RET_ERR;
    }
    free(real_path);
    real_path = NULL;
    return RET_OK;
}


LOCAL gint32 write_cert_to_pipe(const gchar *buffer, guint32 length)
{
    if (g_anti_dns_rebind) {
        if (__write_to_pipe(DECRYPT_CERT_PER_FOR_DEFAULT_SERVER_PATH, buffer, length) != RET_OK) {
            return RET_ERR;
        }
        if (__write_to_pipe(DECRYPT_CERT_KEY_FOR_DEFAULT_SERVER_PATH, buffer, length) != RET_OK) {
            return RET_ERR;
        }
    }

    if (__write_to_pipe(DECRYPT_CERT_PEM_PATH, buffer, length) != RET_OK) {
        return RET_ERR;
    }
    if (__write_to_pipe(DECRYPT_CERT_KEY_PATH, buffer, length) != RET_OK) {
        return RET_ERR;
    }
    return RET_OK;
}

LOCAL void __remove_fifos(void)
{
    (void)vos_rm_filepath(DECRYPT_CERT_PEM_PATH);
    (void)vos_rm_filepath(DECRYPT_CERT_KEY_PATH);
    if (g_anti_dns_rebind) {
        (void)vos_rm_filepath(DECRYPT_CERT_PER_FOR_DEFAULT_SERVER_PATH);
        (void)vos_rm_filepath(DECRYPT_CERT_KEY_FOR_DEFAULT_SERVER_PATH);
    }
}

LOCAL gint32 __create_fifo(const gchar *fifo)
{
    unlink(fifo);
    if (mkfifo(fifo, NGINX_LOG_MOD) != 0) {
        debug_log(DLOG_ERROR, "%s: dal_pfx2pem_mem failed", __FUNCTION__);
        return RET_ERR;
    }
    (void)chown(fifo, APACHE_UID, APACHE_GID);
    return RET_OK;
}


LOCAL gint32 get_ssl_cert(gchar **plaintext, gsize *plaintext_len)
{
    if (plaintext == NULL || plaintext_len == NULL) {
        debug_log(DLOG_ERROR, "%s: input parm is NULL", __FUNCTION__);
        return RET_ERR;
    }

    *plaintext = (gchar *)g_malloc0(MAX_PLAINTEXT_BUF_SIZE);
    if (*plaintext == NULL) {
        debug_log(DLOG_ERROR, "%s: g_malloc0 failed.", __FUNCTION__);
        return RET_ERR;
    }

    if (dal_pfx2pem_mem(SSL_ENCRYPT_CERT, *plaintext, MAX_PLAINTEXT_BUF_SIZE, plaintext_len) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_pfx2pem_mem failed", __FUNCTION__);
        goto ERR;
    }

    if (__create_fifo(DECRYPT_CERT_PEM_PATH) != RET_OK) {
        goto ERR;
    }
    if (__create_fifo(DECRYPT_CERT_KEY_PATH) != RET_OK) {
        goto ERR;
    }
    if (g_anti_dns_rebind) {
        if (__create_fifo(DECRYPT_CERT_PER_FOR_DEFAULT_SERVER_PATH) != RET_OK) {
            goto ERR;
        }
        if (__create_fifo(DECRYPT_CERT_KEY_FOR_DEFAULT_SERVER_PATH) != RET_OK) {
            goto ERR;
        }
    }

    return RET_OK;

ERR:
    clear_sensitive_info(*plaintext, MAX_PLAINTEXT_BUF_SIZE);
    g_free(*plaintext);
    *plaintext = NULL;
    __remove_fifos();
    return RET_ERR;
}

void stop_webserver(void)
{
    gchar* argv[4] = {"/bin/sh", "-c", NULL, NULL};

    if (is_webserver_alive(WEB_SERVER_PID_FILE) == TRUE) {
        argv[2] = "/bin/ps -ef | /bin/grep nginx | /bin/grep -v grep | "
            "/usr/bin/awk '{print $2}' | xargs /bin/kill -9 > /dev/null 2>&1";
        if (vos_system_s(argv[0], argv) != RET_OK) {
            debug_log(DLOG_ERROR, "[%s]kill nginx failed\n", __FUNCTION__);
        }
    }
}

LOCAL gint32 start_webserver(void)
{
    gint32 ret = RET_OK;
    gchar *plaintext = NULL;
    gsize plaintext_len = 0;
    gchar* argv[4] = {"/bin/sh", "-c", NULL, NULL};

    if (is_webserver_alive(WEB_SERVER_PID_FILE) == FALSE) {
        // 解密证书
        ret = get_ssl_cert(&plaintext, &plaintext_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get_ssl_cert failed.", __FUNCTION__);
            return RET_ERR;
        }

        // 启动服务
        argv[2] = "/opt/pme/web/sbin/nginx > /dev/null 2>&1 &";
        ret = vos_system_s(argv[0], argv);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:start nginx failed.", __FUNCTION__);
            goto __EXIT;
        }

        // 写管道文件
        ret = write_cert_to_pipe(plaintext, plaintext_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:write_cert_key_to_fifo failed.", __FUNCTION__);
            stop_webserver();
        }

        (void)chmod(WEB_SERVER_PID_FILE, (S_IRUSR | S_IWUSR | S_IRGRP));
    }

__EXIT:
    if (plaintext != NULL) {
        clear_sensitive_info(plaintext, plaintext_len);
        g_free(plaintext);
        plaintext = NULL;
        __remove_fifos();
    }

    return ret;
}

LOCAL gint32 restart_webserver(void)
{
    gint32 ret;
    gchar *plaintext = NULL;
    gsize plaintext_len = 0;
    gchar* argv[4] = {"/bin/sh", "-c", NULL, NULL};

    if (is_webserver_alive(WEB_SERVER_PID_FILE) == TRUE) {
        // 解密证书
        ret = get_ssl_cert(&plaintext, &plaintext_len);
        debug_log(DLOG_DEBUG, "%s: get_ssl_cert.", __FUNCTION__);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get_ssl_cert failed", __FUNCTION__);
            return RET_ERR;
        }

        // 重启服务
        argv[2] = "/opt/pme/web/sbin/nginx -s reload > /dev/null 2>&1 &";
        ret = vos_system_s(argv[0], argv);
        if (ret != RET_OK) {
            debug_log(DLOG_DEBUG, "%s: reload nginx failed, ret=%d, attempt to forcibly restart", __FUNCTION__, ret);
            stop_webserver();
            (void)start_webserver();
        }

        // 写管道文件, 传递给nginx -s reload进程
        ret = write_cert_to_pipe(plaintext, plaintext_len);
        debug_log(DLOG_DEBUG, "%s: write_cert_key_to_fifo.", __FUNCTION__);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:write_cert_to_pipe failed for nginx reload process", __FUNCTION__);
            stop_webserver();
            goto __EXIT;
        }

        // 写管道文件, 传递给原nginx进程
        ret = write_cert_to_pipe(plaintext, plaintext_len);
        debug_log(DLOG_DEBUG, "%s: write_cert_key_to_fifo.", __FUNCTION__);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:write_cert_to_pipe failed for nginx", __FUNCTION__);
            stop_webserver();
        }
    } else {
        ret = start_webserver();
    }

__EXIT:
    if (plaintext != NULL) {
        clear_sensitive_info(plaintext, plaintext_len);
        g_free(plaintext);
        plaintext = NULL;
        __remove_fifos();
    }

    return ret;
}


gint32 ConfigureSSLCipherSuite(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
#define MAX_SSL_CIPHER_SUITE_STR_LENTH 1024 
#define SSL_CIPHER_SUITE "ssl_ciphers"      

    gint32 ret = 0;
    gchar **suite_vector = NULL;
    gsize vector_len = 0;
    guint32 i = 0;
    gchar *item_content = NULL;
    gchar *division = ":";

    if (property_name == NULL || property_value == NULL) {
        return RET_ERR;
    }

    item_content = (gchar *)g_malloc0(MAX_SSL_CIPHER_SUITE_STR_LENTH);
    if (item_content == NULL) {
        debug_log(DLOG_ERROR, "%s: failed to allocate memory.", __FUNCTION__);
        return RET_ERR;
    }

    ret = snprintf_s(item_content, MAX_SSL_CIPHER_SUITE_STR_LENTH, MAX_SSL_CIPHER_SUITE_STR_LENTH - 1, "%s ",
        SSL_CIPHER_SUITE);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
    }

    suite_vector = g_variant_dup_strv(property_value, &vector_len);
    if (suite_vector == NULL) {
        debug_log(DLOG_ERROR, "%s: g_variant_dup_strv failed.", __FUNCTION__);
        g_free(item_content);
        return RET_ERR;
    }

    for (i = 0; i < vector_len; i++) {
        debug_log(DLOG_DEBUG, "%s: SSLCipherSuite[%d] = %s", __FUNCTION__, i, suite_vector[i]);

        if (strstr(suite_vector[i], "off") != NULL) {
            continue;
        }

        
        if (strncat_s(item_content, MAX_SSL_CIPHER_SUITE_STR_LENTH, suite_vector[i],
            strlen(suite_vector[i]) - strlen(" on")) != EOK) {
            debug_log(DLOG_ERROR, "%s %d: strncat_s failed.", __FUNCTION__, __LINE__);
            g_free(item_content);
            g_strfreev(suite_vector);
            return RET_ERR;
        }

        if (strncat_s(item_content, MAX_SSL_CIPHER_SUITE_STR_LENTH, division, 1) != EOK) {
            debug_log(DLOG_ERROR, "%s %d: strncat_s failed.", __FUNCTION__, __LINE__);
            g_free(item_content);
            g_strfreev(suite_vector);
            return RET_ERR;
        }
    }
    g_strfreev(suite_vector);

    
    item_content[MAX_SSL_CIPHER_SUITE_STR_LENTH - 1] = '\0';

    
    *(item_content + strlen(item_content) - 1) = ';';

    ret = dal_modify_config_file(HTTPS_CIPHER_SUITE_CONF_SHORT_NAME, SSL_CIPHER_SUITE, item_content);
    g_free(item_content);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to modify config file %s", __FUNCTION__, HTTPS_CIPHER_SUITE_CONF_SHORT_NAME);
        return ret;
    }

    return RET_OK;
}

void init_ssl_cipher_suites(void)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;

    ret = dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    if (ret != RET_OK) {
        return;
    }

    ret = dfl_get_property_value(obj_handle, PROPERTY_SECURITY_SSL_CIPHER_SUITES, &property_value);
    if (ret != DFL_OK) {
        return;
    }

    ret = ConfigureSSLCipherSuite(obj_handle, NULL, PROPERTY_SECURITY_SSL_CIPHER_SUITES, property_value);
    g_variant_unref(property_value);

    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: failed to cover %s.", __FUNCTION__, HTTPS_CIPHER_SUITE_CONF_SHORT_NAME);
    }

    return;
}


LOCAL gboolean valid_ip_change_callback_check(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    gint32 ret;
    gchar class_name[MAX_NAME_SIZE] = { 0 };
    guint8 out_type = 0;

    // 只对EthGroup属性变化监听触发的回调进行检查，其他类属性监听均返回TRUE
    ret = dfl_get_class_name(obj_handle, class_name, sizeof(class_name));
    if (ret != DFL_OK || g_strcmp0(class_name, ETH_CLASS_NAME_ETHGROUP) != 0) {
        return TRUE;
    }

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);

    // 只关注对外管理网口的ActivePort、Type及本地链路地址的变化
    if (g_strcmp0(property_name, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT) == 0 ||
        g_strcmp0(property_name, ETH_GROUP_ATTRIBUTE_TYPE) == 0 ||
        g_strcmp0(property_name, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK) == 0) {
        if (out_type != OUT_ETHERNET) {
            debug_log(DLOG_DEBUG, "%s: %s.%s change is ignored", __FUNCTION__, dfl_get_object_name(obj_handle),
                property_name);
            return FALSE;
        }
    }

    // 机柜内网只使用IPv4，不关注IPv6地址的变化
    if (g_strcmp0(property_name, ETH_GROUP_ATTRIBUTE_IPV6_ADDR) == 0 ||
        g_strcmp0(property_name, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR) == 0) {
        if (out_type == RACK_INNER_COMM_ETHERNET || out_type == RACK_NODE_INNER_ETHERNET) {
            debug_log(DLOG_DEBUG, "%s: %s.%s change is ignored", __FUNCTION__, dfl_get_object_name(obj_handle),
                property_name);
            return FALSE;
        }
    }

    return TRUE;
}


gint32 IpAddrChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    const GVariantType *type = NULL;
    const gchar *prop_str = NULL;
    gsize prop_str_len = 0;
    guint8 prop_uint8;

    
    if (valid_ip_change_callback_check(obj_handle, user_data, property_name, property_value) != TRUE) {
        return VOS_OK;
    }

    // 增加必要的日志打印，记录触发原因，增强可定位性
    type = g_variant_get_type(property_value);
    if (g_variant_type_equal(type, G_VARIANT_TYPE_STRING) == TRUE) {
        prop_str = g_variant_get_string(property_value, &prop_str_len);
        if ((prop_str != NULL) && (strlen(prop_str) != 0)) {
            debug_log(DLOG_INFO, "%s: %s.%s changed into %s", __FUNCTION__, dfl_get_object_name(obj_handle),
                property_name, prop_str);
        } else {
            debug_log(DLOG_INFO, "%s: %s.%s changed into empty", __FUNCTION__, dfl_get_object_name(obj_handle),
                property_name);
        }
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE) == TRUE) {
        prop_uint8 = g_variant_get_byte(property_value);
        debug_log(DLOG_INFO, "%s: %s.%s changed into %d", __FUNCTION__, dfl_get_object_name(obj_handle), property_name,
            prop_uint8);
    } else {
        debug_log(DLOG_INFO, "%s: %s.%s changed", __FUNCTION__, dfl_get_object_name(obj_handle), property_name);
    }

    gint32 ret = ConfigureVirtualHost(obj_handle, user_data, property_name, property_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call ConfigureVirtualHost failed, ret:%d", __FUNCTION__, ret);
    }

    ret = SetSnmpAgentAddr(obj_handle, user_data, property_name, property_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call SetSnmpAgentAddr failed, ret:%d", __FUNCTION__, ret);
    }

    return RET_OK;
}


gint32 HostNameDomainNameDNSRebindChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    const GVariantType *type = NULL;
    const gchar *prop_str = NULL;
    gsize prop_str_len = 0;

    // 增加必要的日志打印，记录触发原因，增强可定位性
    type = g_variant_get_type(property_value);
    if (g_variant_type_equal(type, G_VARIANT_TYPE_STRING)) {
        prop_str = g_variant_get_string(property_value, &prop_str_len);
        if ((prop_str != NULL) && (strlen(prop_str) != 0)) {
            debug_log(DLOG_DEBUG, "%s: %s.%s changed into %s", __FUNCTION__, dfl_get_object_name(obj_handle),
                property_name, prop_str);
        } else {
            debug_log(DLOG_DEBUG, "%s: %s.%s changed into empty", __FUNCTION__, dfl_get_object_name(obj_handle),
                property_name);
        }
    } else if (g_variant_type_equal(type, G_VARIANT_TYPE_BYTE)) {
        debug_log(DLOG_DEBUG, "%s: %s.%s changed into %d", __FUNCTION__, dfl_get_object_name(obj_handle), property_name,
            g_variant_get_byte(property_value));
    } else {
        debug_log(DLOG_DEBUG, "%s: %s.%s changed", __FUNCTION__, dfl_get_object_name(obj_handle), property_name);
    }
    gint32 ret = ConfigureVirtualHost(obj_handle, user_data, property_name, property_value);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: call ConfigureVirtualHost failed, ret:%d", __FUNCTION__, ret);
    }
    return RET_OK;
}


gint32 CertStatusChange(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value)
{
    const gchar *obj_name = NULL;

    obj_name = dfl_get_object_name(obj_handle);
    if (obj_name == NULL) {
        debug_log(DLOG_ERROR, "%s: Get object name failed!\n", __FUNCTION__);
        return RET_ERR;
    }

    
    if (strcmp(obj_name, SSL_OBJECT_OWNER) != 0) {
        return RET_OK;
    }

    debug_log(DLOG_ERROR, "%s: cert status changed", __FUNCTION__);
    g_web_server_state = VHOST_RESTART;
    return RET_OK;
}


gint32 config_https_tls_protocol(void)
{
    guint16 tls_protocol = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar protocol_content[SNMP_CONF_MAXLINE_LEN + 1] = { 0 };
    guint8 mutual_state = 0;
    gboolean isTLSv1_2 = TRUE;
    gboolean isTLSv1_3 = TRUE;

    
    (void)snprintf_s(protocol_content, SNMP_CONF_MAXLINE_LEN, SNMP_CONF_MAXLINE_LEN - 1, "%s", "ssl_protocols ");

    
    gint32 ret = dfl_get_object_handle(CLASS_SECURITY_ENHANCE, &obj_handle);
    if (ret == DFL_OK) {
        ret = dal_get_property_value_uint16(obj_handle, PROPERTY_TLS_VERSION, &tls_protocol);
        
        if ((ret == RET_OK) && ((tls_protocol & TLS_1_2_MASK) == 0)) {
            isTLSv1_2 = FALSE;
        }
    }

    (void)dfl_get_object_handle(MUTUAL_AUTH_OBJECT_NAME, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_STATE, &mutual_state);

    if (mutual_state) {
        isTLSv1_3 = FALSE;
    }

    if (isTLSv1_2 == TRUE) {
        errno_t safe_fun_ret = strcat_s(protocol_content, sizeof(protocol_content), "TLSv1.2 ");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    
    if (isTLSv1_3 == TRUE) {
        errno_t safe_fun_ret = strcat_s(protocol_content, sizeof(protocol_content), "TLSv1.3 ");
        if (safe_fun_ret != EOK) {
            debug_log(DLOG_ERROR, "%s: strncat_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret);
        }
    }

    
    *(protocol_content + strlen(protocol_content) - 1) = ';';

    
    FM_FILE_S *file_handle = fm_fopen(HTTPS_TLS_SHORT_FILE_NAME, "w");
    if (file_handle == NULL) {
        debug_log(DLOG_ERROR, "%s:%d:fail to open %s\n", __FUNCTION__, __LINE__, HTTPS_TLS_SHORT_FILE_NAME);
        return RET_ERR;
    }

    
    
    (void)fchmod(fileno(file_handle->fd), 0640);
    
    ret = fm_fwrite(protocol_content, 1, strlen(protocol_content), file_handle);
    if (ret != strlen(protocol_content)) {
        fm_fclose(file_handle);
        debug_log(DLOG_ERROR, "%s:write file(length:%zu) fail:%d.\n", __FUNCTION__, strlen(protocol_content), ret);
        return RET_ERR;
    }

    fm_fclose(file_handle);
    
    (void)chown(HTTPS_TLS_FILE, APACHE_UID, APACHE_GID);
    

    return RET_OK;
}


LOCAL void get_https_configuration_out_port(HTTPS_CONF_S *configure)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guint8 ip_ver = ENABLE_IPV4_AND_IPV6;

    if (configure == NULL) {
        debug_log(DLOG_ERROR, "input parm error.");
        return;
    }

    (void)dal_get_object_handle_nth(ETH_CLASS_NAME_ETHGROUP, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ip_ver);
    configure->ip_version = ip_ver;

    configure->port_out = VHOST_DEFAULT_OUTER_PORT;

    // 获取外部服务IP地址,如果获取失败则取默认IP
    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        ret = dal_get_multi_bmc_service_ipv4addr(OUTTER_GROUP_TYPE, configure->ip_out_multi[0],
            sizeof(configure->ip_out_multi), &configure->ip_out_num);
        if (ret == RET_OK) {
            ret = dal_get_multi_bmc_service_ipv6addr(OUTTER_GROUP_TYPE, configure->ipv6_out_multi[0],
                sizeof(configure->ipv6_out_multi), &configure->ipv6_out_num);
        }
        debug_log(DLOG_DEBUG, "%s: update https out_port(%d), ipv4num(%d), ip(%s/%s/%s/%s).\n", __FUNCTION__,
            configure->port_out, configure->ip_out_num, configure->ip_out_multi[0], configure->ip_out_multi[0x1],
            configure->ip_out_multi[0x2], configure->ip_out_multi[0x3]);
        debug_log(DLOG_DEBUG, "%s: update https out_port(%d), ipv6num(%d), ip(%s/%s/%s/%s).\n", __FUNCTION__,
            configure->port_out, configure->ipv6_out_num, configure->ipv6_out_multi[0], configure->ipv6_out_multi[0x1],
            configure->ipv6_out_multi[0x2], configure->ipv6_out_multi[0x3]);
    } else {
        ret = dal_get_bmc_service_ipaddr(OUTTER_GROUP_TYPE, configure->ip_out, sizeof(configure->ip_out),
            configure->ipv6_out, sizeof(configure->ipv6_out));
    }
    if (ret != RET_OK) {
        strcpy_s(configure->ip_out, sizeof(configure->ip_out), VHOST_DEFAULT_OUTER_IP);
        strcpy_s(configure->ipv6_out, sizeof(configure->ipv6_out), VHOST_DEFAULT_OUTER_IP);
    }
}


LOCAL void get_https_configuration_inner_port(HTTPS_CONF_S *configure)
{
    gint32 ret;
    if (configure == NULL) {
        debug_log(DLOG_ERROR, "input parm error");
        return;
    }

    configure->port_inner1 = VHOST_DEFAULT_INNER_PORT; // 刀片与MM920/921通信,固定使用40443端口
    configure->port_inner2 = VHOST_DEFAULT_OUTER_PORT; // 刀片与旧版本管理板(如MM910),固定使用443端口

    
    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        ret = dal_get_multi_bmc_service_ipv4addr(INNER_GROUP_TYPE, configure->ip_inner_multi[0],
            sizeof(configure->ip_inner_multi), &configure->ip_inner_num);
        debug_log(DLOG_DEBUG, "%s: update https inner_port(%d), ipv4num(%d), ip(%s/%s/%s/%s).\n", __FUNCTION__,
            configure->port_inner1, configure->ip_inner_num, configure->ip_inner_multi[0x0],
            configure->ip_inner_multi[0x1], configure->ip_inner_multi[0x2], configure->ip_inner_multi[0x3]);
    } else {
        ret = dal_get_bmc_service_ipaddr(INNER_GROUP_TYPE, configure->ip_inner, sizeof(configure->ip_inner), NULL, 0);
    }

    if (ret == RET_OK) {
        configure->state_inner = VHOST_ENABLE;
    } else {
        debug_log(DLOG_ERROR, "%s:get inner ip failed", __FUNCTION__);
    }
}


LOCAL gint32 get_rack_inner_ip_by_chan(guint8 channel, gchar *ipv4, guint32 ipv4_len)
{
    gint32 ret;
    OBJ_HANDLE obj_eth_group = 0;

    if ((ipv4 == NULL) || (ipv4_len < MAX_IPV4_LEN)) {
        debug_log(DLOG_ERROR, "%s: invalid parameters", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_CHANNEL, channel, &obj_eth_group);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get obj by chan=%d failed, ret=%d", __FUNCTION__, channel, ret);
        return ret;
    }

    ret = dal_get_property_value_string(obj_eth_group, ETH_GROUP_ATTRIBUTE_IP_ADDR, ipv4, ipv4_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get ipv4 failed, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    return RET_OK;
}


LOCAL void get_usb_mgmt_network_conf(HTTPS_CONF_S *configure)
{
    OBJ_HANDLE obj_handle = 0;
    gchar ip_addr[MAX_IPV4_LEN] = { 0 };
    gint32 ret;

    
    configure->state_usb_mgmt = VHOST_DISABLE;

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK || obj_handle == 0) {
        debug_log(DLOG_INFO, "%s: get usbmgmt obj failed, ret=%d", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_USB_MGMT_USB_NETWORK_IP_ADDR, ip_addr, sizeof(ip_addr));
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s failed, ret=%d", __FUNCTION__, PROPERTY_USB_MGMT_USB_NETWORK_IP_ADDR, ret);
        return;
    }

    
    if (strlen(ip_addr) == 0) {
        debug_log(DLOG_INFO, "%s: ipaddr is clear", __FUNCTION__);
        return;
    }

    
    ret = vos_ipv4_addr_valid_check((const guchar *)ip_addr);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invaild ipv4_addr %s", __FUNCTION__, ip_addr);
        return;
    }

    ret = strcpy_s(configure->ip_usb_mgmt, sizeof(configure->ip_usb_mgmt), ip_addr);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s: strcpy_s failed, ret=%d", __FUNCTION__, ret);
        return;
    }

    configure->state_usb_mgmt = VHOST_ENABLE;
    return;
}


LOCAL void get_float_ip_http_addr(HTTP_CONF_S *configure)
{
    gint32 ret;

    // 如果是HMM主板,则还需要写入浮动IP地址
    (void)dal_get_software_type(&configure->software_type);
    (void)get_board_active_state(&configure->as_status);
    if (configure->software_type != MGMT_SOFTWARE_TYPE_EM || configure->as_status != ASM_ACTIVE) {
        return;
    }

    ret = dal_get_bmc_service_ipaddr(FLOAT_GROUP_TYPE, configure->ipv4_float, sizeof(configure->ipv4_float),
        configure->ipv6_float, sizeof(configure->ipv6_float));
    if (ret == RET_ERR) {
        debug_log(DLOG_ERROR, "%s: get float ip failed", __FUNCTION__);
    }

    return;
}


LOCAL void get_bma_http_addr(HTTP_CONF_S *configure)
{
    gint32 ret;

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86,
        &configure->host_enable_bma);
    if (configure->host_enable_bma == VHOST_ENABLE) {
        ret = dal_get_inband_net_service_ipaddr(configure->ipv4_bma, sizeof(configure->ipv4_bma), configure->ipv6_bma,
            sizeof(configure->ipv6_bma));
        if (ret == RET_ERR) {
            debug_log(DLOG_ERROR, "%s: get inband channel ipaddr failed", __FUNCTION__);
        }
    }

    return;
}


LOCAL void get_FusionPoD_inner_http_addr(HTTP_CONF_S *configure)
{
    gint32 ret;

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_IRM_HEART_BEAT_ENABLE,
        &configure->irm_heartbeat_enable);
    if ((configure->irm_heartbeat_enable != 0) && (configure->net_type == NET_TYPE_PCIE ||
         configure->net_type == NET_TYPE_CABINET_VLAN)) {
        ret = get_rack_inner_ip_by_chan(RACK_MAIN_CHAN_NUM, configure->ipv4_inner_main,
            sizeof(configure->ipv4_inner_main));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get main rack inner ip failed", __FUNCTION__);
        }

        ret =
            get_rack_inner_ip_by_chan(RACK_BAK_CHAN_NUM, configure->ipv4_inner_bak, sizeof(configure->ipv4_inner_bak));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get backup rack inner ip failed", __FUNCTION__);
        }
    }

    return;
}


LOCAL void get_http_listen_out_addr(HTTP_CONF_S *configure)
{
    gint32 ret;

    
    configure->support_multi_enable = dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN);
    if (configure->support_multi_enable) {
        ret = dal_get_multi_bmc_service_ipv4addr(OUTTER_GROUP_TYPE, configure->ipv4_multi_out[0],
            sizeof(configure->ipv4_multi_out), &configure->ipv4_num_out);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get out service ipaddrv4 failed(%d)", __FUNCTION__, ret);
            return;
        }

        ret = dal_get_multi_bmc_service_ipv6addr(OUTTER_GROUP_TYPE, configure->ipv6_multi_out[0],
            sizeof(configure->ipv6_multi_out), &configure->ipv6_num_out);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get out service ipaddrv6 failed(%d)", __FUNCTION__, ret);
            return;
        }

        if ((configure->ipv4_num_out == 0 && configure->ipv6_num_out == 0) ||
            (configure->ipv4_num_out > MAX_VHOST_IP_NUM) || (configure->ipv6_num_out > MAX_VHOST_IP_NUM)) {
            debug_log(DLOG_ERROR, "%s: ipv4 and ipv6 len is error %d %d", __FUNCTION__, configure->ipv4_num_out,
                configure->ipv6_num_out);
        }
    } else {
        
        ret = dal_get_bmc_service_ipaddr(OUTTER_GROUP_TYPE, configure->ipv4_out, sizeof(configure->ipv4_out),
            configure->ipv6_out, sizeof(configure->ipv6_out));
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get out service ipaddr failed(%d)", __FUNCTION__, ret);
            return;
        }

        if (strlen(configure->ipv4_out) == 0 && strlen(configure->ipv6_out) == 0) {
            debug_log(DLOG_ERROR, "%s: ipv4 and ipv6 len is zero", __FUNCTION__);
        }
    }

    return;
}


LOCAL void get_usb_mgmt_http_listen_out_addr(gchar *ip_addr, gint32 ip_addr_len)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    ret = dal_get_object_handle_nth(CLASS_USB_MGMT, 0, &obj_handle);
    if (ret != RET_OK || obj_handle == 0) {
        debug_log(DLOG_INFO, "%s: get usbmgmt obj failed, ret=%d", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_property_value_string(obj_handle, PROPERTY_USB_MGMT_USB_NETWORK_IP_ADDR, ip_addr, ip_addr_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s failed, ret=%d", __FUNCTION__, PROPERTY_USB_MGMT_USB_NETWORK_IP_ADDR, ret);
        return;
    }

    return;
}


LOCAL gint32 get_http_https_port_state(HTTP_CONF_S *configure)
{
    gint32 retv;
    guchar HTTPSEnable = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    const guint16 *default_port_array = NULL;
    gsize default_port_count = 0;

    retv = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_HTTPS, &HTTPSEnable);
    if (retv == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_HTTPSEnable_status fail.\n", __FUNCTION__, __LINE__);
        return RET_ERR;
    }

    
    retv = dfl_get_object_handle(WEBHTTP_SERVER_PORT_OBJ_NAME, &obj_handle);
    if ((HTTPSEnable == SERVICE_STATE_ENABLE) && (retv == RET_OK) && (obj_handle > 0)) {
        (void)dal_get_property_value_byte(obj_handle, SERVER_PORT_ATTRIBUTE_STATE, &configure->state);
        retv = dfl_get_property_value(obj_handle, SERVER_PORT_ATTRIBUTE_PORT, &property_value);
        if (retv == DFL_OK && property_value != NULL) {
            default_port_array =
                (const guint16 *)g_variant_get_fixed_array(property_value, &default_port_count, sizeof(guint16));
            if (default_port_array != NULL) {
                configure->http_port = default_port_array[0];
            }
            g_variant_unref(property_value);
        }
    }

    
    retv = dfl_get_object_handle(WEBHTTPS_SERVER_PORT_OBJ_NAME, &obj_handle);
    if ((retv == RET_OK) && (obj_handle > 0)) {
        retv = dfl_get_property_value(obj_handle, SERVER_PORT_ATTRIBUTE_PORT, &property_value);
        if (retv == DFL_OK && property_value != NULL) {
            default_port_array =
                (const guint16 *)g_variant_get_fixed_array(property_value, &default_port_count, sizeof(guint16));
            if (default_port_array != NULL) {
                configure->https_port = default_port_array[0];
            }
            g_variant_unref(property_value);
        }
    }

    return RET_OK;
}


LOCAL gint32 get_http_configuration(HTTP_CONF_S *configure)
{
    gint32 retv = 0;
    OBJ_HANDLE group_obj;

    
    (void)memset_s(configure, sizeof(HTTP_CONF_S), 0, sizeof(HTTP_CONF_S));

    retv = get_http_https_port_state(configure);
    if (retv == RET_ERR) {
        return RET_ERR;
    }
    // 检查当前ipv6是否处于SLACC模式, 检查失败或者处于该模式则监听任意地址
    configure->is_slaac = TRUE;
    retv = is_ipv6_slaac_mode(&configure->is_slaac);
    if (retv == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get ipv6_slaac_mode fail.\n", __FUNCTION__, __LINE__);
    }

    get_local_link_addr_and_ethname(configure->ip_local_link, sizeof(configure->ip_local_link), configure->eth_name,
        sizeof(configure->eth_name));

    
    get_usb_mgmt_http_listen_out_addr(configure->usb_ip_addr, sizeof(configure->usb_ip_addr));

    
    get_http_listen_out_addr(configure);

    // 获取对BMA服务的监听地址
    get_bma_http_addr(configure);

    // 获取对外管理网口类型
    retv =
        dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUT_ETHERNET, &group_obj);
    if (retv == RET_OK) {
        (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_TYPE, &configure->net_type);
    }

    get_FusionPoD_inner_http_addr(configure);

    
    if (configure->net_type != NET_TYPE_DEDICATED) {
        retv = dal_get_bmc_service_ipaddr(MAINT_DEDICATED_ETHERNET, configure->ip_maint, sizeof(configure->ip_maint),
            NULL, 0);
        if (retv == RET_ERR) {
            debug_log(DLOG_ERROR, "%s, %d: MAINT_DEDICATED_ETHERNET fail.\n", __FUNCTION__, __LINE__);
        }
    }

    get_float_ip_http_addr(configure);
    return RET_OK;
}


LOCAL void get_https_configuration(HTTPS_CONF_S *configure)
{
    gint32 retv = 0;
    OBJ_HANDLE obj_handle = 0;
    GVariant *property_value = NULL;
    const guint16 *default_port_array = NULL;
    gsize default_port_count = 0;
    guchar HTTPSEnable = 0;
    guchar software_type = 0;
    guchar as_status = ASM_ACTIVE;
    OBJ_HANDLE group_obj;
    guchar net_type = NET_TYPE_DEDICATED;
    guint8 irm_heartbeat_enable = 0;

    (void)memset_s(configure, sizeof(HTTPS_CONF_S), 0, sizeof(HTTPS_CONF_S));

    
    retv = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_HTTPS, &HTTPSEnable);
    if (retv == RET_ERR) {
        debug_log(DLOG_ERROR, "%s, %d: get_WEBEnable_status fail.\n", __FUNCTION__, __LINE__);
        return;
    }

    if (HTTPSEnable == SERVICE_STATE_DISABLE) {
        return;
    }

    
    get_https_configuration_out_port(configure);
    get_local_link_addr_and_ethname(configure->ip_local_link, sizeof(configure->ip_local_link), configure->eth_name,
        sizeof(configure->eth_name));

    retv = dfl_get_object_handle(WEBHTTPS_SERVER_PORT_OBJ_NAME, &obj_handle);
    if ((retv == RET_OK) && (obj_handle > 0)) {
        
        (void)dal_get_property_value_byte(obj_handle, SERVER_PORT_ATTRIBUTE_STATE, &configure->state_out);
        retv = dfl_get_property_value(obj_handle, SERVER_PORT_ATTRIBUTE_PORT, &property_value);
        if (retv == RET_OK && property_value != NULL) {
            default_port_array = (const guint16 *)g_variant_get_fixed_array(property_value, &default_port_count,
                sizeof(*default_port_array));
            configure->port_out = default_port_array[0];
            g_variant_unref(property_value);
        }
    }

    
    get_https_configuration_inner_port(configure);

    (void)dal_get_software_type(&software_type);
    (void)get_board_active_state(&as_status);
    if ((software_type == MGMT_SOFTWARE_TYPE_EM) && (as_status == ASM_ACTIVE)) {
        // 如果是主用板, 则还需获取浮动IP
        retv = dal_get_bmc_service_ipaddr(FLOAT_GROUP_TYPE, configure->ip_float, sizeof(configure->ip_float),
            configure->ipv6_float, sizeof(configure->ipv6_float));
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "%s:get float ip failed", __FUNCTION__);
        }
        debug_log(DLOG_ERROR, "%s, update https float_port(%d), ip(%s) ipv6(%s).\n", __FUNCTION__, configure->port_out,
            configure->ip_float, configure->ipv6_float);
    }

    // 获取对外管理网口类型
    retv =
        dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUT_ETHERNET, &group_obj);
    if (retv == RET_OK) {
        (void)dal_get_property_value_byte(group_obj, ETH_GROUP_ATTRIBUTE_TYPE, &net_type);
    }

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_IRM_HEART_BEAT_ENABLE, &irm_heartbeat_enable);
    if ((irm_heartbeat_enable != 0) && (net_type == NET_TYPE_PCIE || net_type == NET_TYPE_CABINET_VLAN)) {
        configure->state_rack_inner = VHOST_ENABLE;
        
        (void)get_rack_inner_ip_by_chan(RACK_MAIN_CHAN_NUM, configure->ip_rack_inner, sizeof(configure->ip_rack_inner));

        (void)get_rack_inner_ip_by_chan(RACK_BAK_CHAN_NUM, configure->ip_rack_inner_bak,
            sizeof(configure->ip_rack_inner_bak));
    }

    
    if (net_type != NET_TYPE_DEDICATED) {
        retv = dal_get_bmc_service_ipaddr(MAINT_DEDICATED_ETHERNET, configure->ip_maint, sizeof(configure->ip_maint),
            NULL, 0);
        if (retv == RET_OK) {
            // 获取近端运维口成功获取时维护标志位，表示需要将近端运维IP加入https服务侦听地址列表
            configure->state_maint = VHOST_ENABLE;
        }
    }

    
    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_X86, &configure->state_bma);
    
    if (configure->state_bma == VHOST_ENABLE) {
        configure->port_bma = VHOST_DEFAULT_SMS_PORT;
        retv = dal_get_inband_net_service_ipaddr(configure->ip_bma, sizeof(configure->ip_bma), configure->ipv6_bma,
            sizeof(configure->ipv6_bma));
        if (retv != RET_OK) {
            debug_log(DLOG_ERROR, "%s: get inband channel ipaddr failed", __FUNCTION__);
            return;
        }
        if (vos_ipv4_addr_valid_check((guchar *)configure->ip_bma) != RET_OK) {
            (void)memset_s(configure->ip_bma, sizeof(configure->ip_bma), 0, sizeof(configure->ip_bma));
        }
    }

    
    get_usb_mgmt_network_conf(configure);
}

LOCAL void write_bma_https_listen(FM_FILE_S *fp, const HTTPS_CONF_S *configure)
{
    guint8 channel_type;

    channel_type =
        (g_strcmp0((const gchar *)configure->ipv6_bma, VHOST_DEFAULT_SMS_CDEV_IPADDR) == 0) ? PCIE_CDEV : PCIE_VETH;
    if (configure->state_bma == VHOST_ENABLE && configure->port_bma != configure->port_out) {
        if (channel_type == PCIE_CDEV) {
            write_service_listen_addr(fp, (const gchar *)configure->ipv6_bma, configure->port_bma, NULL, TRUE);
        } else {
            write_service_listen_addr(fp, (const gchar *)configure->ipv6_bma, configure->port_bma, "veth", TRUE);
            if (vos_ipv4_addr_valid_check((const guchar *)configure->ip_bma) == RET_OK) {
                write_service_listen_addr(fp, (const gchar *)configure->ip_bma, configure->port_bma, NULL, TRUE);
            }
        }
    }
}

LOCAL void __write_return_403_directive(FM_FILE_S *fp)
{
    const gchar *directive = "return 403;\r\n}";
    gint32 ret = fm_fwrite(directive, 1, strlen(directive), fp);
    if (ret != strlen(directive)) {
        debug_log(DLOG_ERROR, "%s: write failed, ret: %d.", __FUNCTION__, ret);
    }
}

LOCAL void __write_https_defaule_server_name_directive(FM_FILE_S *fp)
{
    const gchar *server_name = "server_name _;\r\n";
    gint32 ret = fm_fwrite(server_name, 1, strlen(server_name), fp);
    if (ret != strlen(server_name)) {
        debug_log(DLOG_ERROR, "%s: write failed, ret: %d.", __FUNCTION__, ret);
    }
}

LOCAL void __write_https_server_name_directive(FM_FILE_S *fp)
{
    OBJ_HANDLE obj_handle_bmc = 0;
    OBJ_HANDLE obj_handle_dns = 0;
    gchar hostname[HOST_NAME_MAX_LEN + 1] = { 0 };
    gchar domainname[DNS_DOMAINNAME_MAX_LEN + 1] = { 0 };
    gchar buffer[BUFFER_SIZE_2K] = { 0 };
    const gchar *perl_ipv4_reg = "\"~^(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})$\"";
    const gchar *perl_ipv6_veth_reg = "\"~^FE80\"";
    const gchar *perl_ipv6_reg = "\"~^\\[(((([\\da-f]{1,4}):){7}([\\da-f]{1,4}))|(((([\\da-f]{1,4}):){1,7}:)|"
        "((([\\da-f]{1,4}):){6}:([\\da-f]{1,4}))|"
        "((([\\da-f]{1,4}):){5}:(([\\da-f]{1,4}):)?([\\da-f]{1,4}))|"
        "((([\\da-f]{1,4}):){4}:(([\\da-f]{1,4}):){0,2}([\\da-f]{1,4}))|"
        "((([\\da-f]{1,4}):){3}:(([\\da-f]{1,4}):){0,3}([\\da-f]{1,4}))|"
        "((([\\da-f]{1,4}):){2}:(([\\da-f]{1,4}):){0,4}([\\da-f]{1,4}))|"
        "((([\\da-f]{1,4}):){1}:(([\\da-f]{1,4}):){0,5}([\\da-f]{1,4}))|"
        "(::(([\\da-f]{1,4}):){0,6}([\\da-f]{1,4}))|(::([\\da-f]{1,4})?))|"
        "(((([\\da-f]{1,4}):){6}(((1?[1-9]?\\d)|(10\\d)|"
        "(2[0-4]\\d)|(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))|"
        "((([\\da-f]{1,4}):){5}:(((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|"
        "(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))|"
        "((([\\da-f]{1,4}):){4}:(([\\da-f]{1,4}):)?(((1?[1-9]?\\d)|(10\\d)|"
        "(2[0-4]\\d)|(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))|"
        "((([\\da-f]{1,4}):){3}:(([\\da-f]{1,4}):){0,2}(((1?[1-9]?\\d)|(10\\d)|"
        "(2[0-4]\\d)|(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))|"
        "((([\\da-f]{1,4}):){2}:(([\\da-f]{1,4}):){0,3}(((1?[1-9]?\\d)|(10\\d)|"
        "(2[0-4]\\d)|(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))|"
        "(([\\da-f]{1,4})::(([\\da-f]{1,4}):){0,4}(((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|"
        "(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))|"
        "(::(([\\da-f]{1,4}):){0,5}(((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|"
        "(25[0-5]))\\.){3}((1?[1-9]?\\d)|(10\\d)|(2[0-4]\\d)|(25[0-5])))))\\]$\"";

    
    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle_bmc);
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle_dns);
    
    (void)dal_get_property_value_string(obj_handle_bmc, BMC_HOST_NAME, hostname, sizeof(hostname));
    (void)dal_get_property_value_string(obj_handle_dns, PROPERTY_DNSSETTING_DOMAINNAME, domainname, sizeof(domainname));

    if (!g_anti_dns_rebind) {
        __write_https_defaule_server_name_directive(fp);
        return;
    }

    if (snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "server_name %s %s%s%s %s %s %s;\r\n", hostname,
        hostname, (strlen(domainname) == 0) ? "" : ".", domainname, perl_ipv4_reg, perl_ipv6_veth_reg,
        perl_ipv6_reg) <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed.", __FUNCTION__);
        return;
    }

    if (fm_fwrite(buffer, 1, strlen(buffer), fp) != strlen(buffer)) {
        debug_log(DLOG_ERROR, "%s: write failed.", __FUNCTION__);
    }
}

LOCAL void __write_include_ssl_cert_path_directive(FM_FILE_S *fp)
{
    gchar buffer[BUFFER_SIZE_1K] = { 0 };
    gint32 ret = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "include %s;\r\n", HTTPS_SSL_CERT_PATH_CONF);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret： %d", __FUNCTION__, ret);
        return;
    }
    ret = fm_fwrite(buffer, 1, strlen(buffer), fp);
    if (ret != strlen(buffer)) {
        debug_log(DLOG_ERROR, "%s: write failed, ret: %d.", __FUNCTION__, ret);
    }
}

LOCAL void __write_https_server(FM_FILE_S *fp, const HTTPS_CONF_S *configure)
{
    gboolean is_listen_default = FALSE;

    gint32 ret = fm_fwrite(SERVER_BEGAIN, 1, strlen(SERVER_BEGAIN), fp);
    if (ret != strlen(SERVER_BEGAIN)) {
        debug_log(DLOG_ERROR, "%s:write https server begain fail.", __FUNCTION__);
    }

    
    write_out_service_listen_addr(fp, *configure, &is_listen_default);
    // 如果外部服务端口跟内部端口一致(40443),且已经监听默认IP, 则不用写入对BMA和HMM服务的地址和虚拟主机信息
    if (is_listen_default == TRUE && configure->port_out == VHOST_DEFAULT_INNER_PORT) {
        write_inner_service_listen_addr(fp, *configure, is_listen_default);
    } else {
        
        write_bma_https_listen(fp, configure);

        // 4. 内部IP监听
        write_inner_service_listen_addr(fp, *configure, is_listen_default);
    }

    if (g_is_writing_default_server) {
        __write_https_defaule_server_name_directive(fp);
        __write_include_ssl_cert_path_directive(fp);
        __write_return_403_directive(fp);
        return;
    }
    __write_https_server_name_directive(fp);
    ret = fm_fwrite(SERVER_END_HTTPS, 1, strlen(SERVER_END_HTTPS), fp);
    if (ret != strlen(SERVER_END_HTTPS)) {
        debug_log(DLOG_ERROR, "%s:write fail.", __FUNCTION__);
    }
}

LOCAL gboolean __get_change_flag(guchar anti_dns_rebind, const gchar *hostname, const gchar *domainname)
{
    if (anti_dns_rebind != g_anti_dns_rebind) {
        return TRUE;
    }
    if (!anti_dns_rebind) {
        return FALSE;
    }
    if (strcmp(g_hostname, hostname) != 0 || strcmp(g_domainname, domainname) != 0) {
        return TRUE;
    }
    return FALSE;
}

LOCAL gboolean __anti_dns_rebind_hostname_domainname_should_change(void)
{
    OBJ_HANDLE obj_handle = 0;
    guchar anti_dns_rebind = 0;
    gchar hostname[HOST_NAME_MAX_LEN + 1] = { 0 };
    gchar domainname[DNS_DOMAINNAME_MAX_LEN + 1] = { 0 };

    (void)dal_get_object_handle_nth(CLASS_SECURITY_ENHANCE, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_ANTI_DNS_REBIND, &anti_dns_rebind);
    (void)dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    (void)dal_get_property_value_string(obj_handle, BMC_HOST_NAME, hostname, sizeof(hostname));
    (void)dal_get_object_handle_nth(CLASS_NAME_DNSSETTING, 0, &obj_handle);
    (void)dal_get_property_value_string(obj_handle, PROPERTY_DNSSETTING_DOMAINNAME, domainname, sizeof(domainname));

    gboolean flag = __get_change_flag(anti_dns_rebind, hostname, domainname);

    g_anti_dns_rebind = anti_dns_rebind;
    (void)memcpy_s(g_hostname, sizeof(g_hostname), hostname, sizeof(hostname));
    (void)memcpy_s(g_domainname, sizeof(g_domainname), domainname, sizeof(domainname));
    return flag;
}


void update_https_configuration(void)
{
    FM_FILE_S *fp = NULL;
    HTTPS_CONF_S configure;
    static HTTPS_CONF_S previous_configure = { 0 };

    get_https_configuration(&configure);

    gboolean configure_changed = memcmp(&configure, &previous_configure, sizeof(configure)) != 0;
    (void)memcpy_s(&previous_configure, sizeof(previous_configure), &configure, sizeof(configure));
    gboolean rebind_hostname_domainname_changed = __anti_dns_rebind_hostname_domainname_should_change();
    if (!configure_changed && !rebind_hostname_domainname_changed) {
        debug_log(DLOG_ERROR, "Nothing to be done for https configuration.");
        return;
    }

    
    fp = fm_fopen(HTTPS_CONF_NAME, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open conf file failed.");
        return;
    }

    if (configure.state_out + configure.state_inner + configure.state_bma + configure.state_rack_inner +
        configure.state_maint + configure.state_usb_mgmt ==
        SERVICE_STATE_DISABLE) {
        debug_log(DLOG_DEBUG, "%s: go to exit", __FUNCTION__);
        g_web_server_state = VHOST_STOP;
        fm_fclose(fp);
        return;
    }

    if (g_anti_dns_rebind) {
        g_is_writing_default_server = TRUE;
        debug_log(DLOG_DEBUG, "write default server");
        __write_https_server(fp, &configure);
    }
    debug_log(DLOG_DEBUG, "write server");
    g_is_writing_default_server = FALSE;
    __write_https_server(fp, &configure);

    fm_fclose(fp);
    if (g_web_server_state == VHOST_START) {
        g_web_server_state = VHOST_RESTART;
    } else if (g_web_server_state == VHOST_STOP) {
        g_web_server_state = VHOST_START;
    }
}


LOCAL void write_usb_mgmt_http_listen_out_addr(FM_FILE_S *fp, HTTP_CONF_S *configure)
{
    gint32 ret;

    
    if (strlen(configure->usb_ip_addr) == 0) {
        debug_log(DLOG_INFO, "%s: ipaddr is clear", __FUNCTION__);
        return;
    }

    
    ret = vos_ipv4_addr_valid_check((const guchar *)configure->usb_ip_addr);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: invaild ipv4_addr %s", __FUNCTION__, configure->usb_ip_addr);
        return;
    }

    write_service_listen_addr(fp, configure->usb_ip_addr, configure->http_port, NULL, FALSE);

    return;
}


LOCAL gint32 write_http_listen_out_addr(FM_FILE_S *fp, HTTP_CONF_S *configure)
{
    guchar i = 0;

    
    write_service_listen_addr(fp, (const gchar *)configure->ip_local_link, configure->http_port,
        (const gchar *)configure->eth_name, FALSE);

    
    write_usb_mgmt_http_listen_out_addr(fp, configure);

    
    if (configure->support_multi_enable) {
        if ((configure->ipv4_num_out == 0 && configure->ipv6_num_out == 0) ||
            (configure->ipv4_num_out > MAX_VHOST_IP_NUM) || (configure->ipv6_num_out > MAX_VHOST_IP_NUM)) {
            debug_log(DLOG_ERROR, "%s: ipv4 and ipv6 len is error %d %d", __FUNCTION__, configure->ipv4_num_out,
                configure->ipv6_num_out);
            return RET_ERR;
        }

        
        for (i = 0; i < configure->ipv4_num_out; i++) {
            write_service_listen_addr(fp, (const gchar *)configure->ipv4_multi_out[i], configure->http_port, NULL,
                FALSE);
        }

        for (i = 0; i < configure->ipv6_num_out; i++) {
            write_service_listen_addr(fp, (const gchar *)configure->ipv6_multi_out[i], configure->http_port, NULL,
                FALSE);
        }
    } else {
        
        if (strlen(configure->ipv4_out) == 0 && strlen(configure->ipv6_out) == 0) {
            debug_log(DLOG_ERROR, "%s: ipv4 and ipv6 len is zero", __FUNCTION__);
            return RET_ERR;
        }

        write_service_listen_addr(fp, (const gchar *)configure->ipv4_out, configure->http_port, NULL, FALSE);
        write_service_listen_addr(fp, (const gchar *)configure->ipv6_out, configure->http_port, NULL, FALSE);
    }

    return RET_OK;
}


gint32 is_ipv6_slaac_mode(gboolean *is_slaac)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar **ipv6_slaac_addr = NULL;
    gsize ip_cnt = 0;
    gsize i;

    if (is_slaac == NULL) {
        debug_log(DLOG_ERROR, "%s:NULL pointer", __FUNCTION__);
        return RET_ERR;
    }
    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUTTER_GROUP_TYPE,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get ethgroup object handle failed, ret:%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    ret = dal_get_property_value_strv(obj_handle, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR, &ipv6_slaac_addr, &ip_cnt);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get property %s failed, ret:%d", __FUNCTION__, ETH_GROUP_ATTRIBUTE_SLAAC_IPV6_ADDR,
            ret);
        return RET_ERR;
    }

    *is_slaac = FALSE;
    for (i = 0; i < ip_cnt; i++) {
        if (ipv6_slaac_addr[i] != NULL && strlen(ipv6_slaac_addr[i]) != 0) {
            *is_slaac = TRUE;
            break;
        }
    }

    if (ipv6_slaac_addr != NULL) {
        g_strfreev(ipv6_slaac_addr);
    }
    return RET_OK;
}

LOCAL void write_FusionPoD_inner_http_listen(FM_FILE_S *fp, HTTP_CONF_S *configure)
{
    
    if ((configure->irm_heartbeat_enable != 0) && (configure->net_type == NET_TYPE_PCIE ||
         configure->net_type == NET_TYPE_CABINET_VLAN)) {
        if (strlen(configure->ipv4_inner_main)) {
            write_service_listen_addr(fp, configure->ipv4_inner_main, configure->http_port, NULL, FALSE);
        }

        if (strlen(configure->ipv4_inner_bak)) {
            write_service_listen_addr(fp, configure->ipv4_inner_bak, configure->http_port, NULL, FALSE);
        }
    }
    return;
}

LOCAL void write_bma_http_listen(FM_FILE_S *fp, HTTP_CONF_S *configure)
{
    if (configure->host_enable_bma == VHOST_ENABLE) {
        if (strlen(configure->ipv4_bma) > 0) {
            write_service_listen_addr(fp, configure->ipv4_bma, configure->http_port, NULL, FALSE);
        }
        if (strlen(configure->ipv6_bma) > 0) {
            write_service_listen_addr(fp, configure->ipv6_bma, configure->http_port,
                ((g_strcmp0((const gchar *)configure->ipv6_bma, VHOST_DEFAULT_SMS_IPADDR) == 0) ? "veth" : NULL),
                FALSE);
        }
    }
    return;
}

LOCAL void write_float_ip_http_listen(FM_FILE_S *fp, HTTP_CONF_S *configure)
{
    // 如果是HMM主板,则还需要写入浮动IP地址
    if (configure->software_type != MGMT_SOFTWARE_TYPE_EM || configure->as_status != ASM_ACTIVE) {
        return;
    }

    if (strlen(configure->ipv4_float) > 0) {
        debug_log(DLOG_DEBUG, "%s: update http float_port(%d), ipv4(%s).\n", __FUNCTION__, configure->http_port,
            configure->ipv4_float);
        write_service_listen_addr(fp, (const gchar *)configure->ipv4_float, configure->http_port, NULL, FALSE);
    }
    if (strlen(configure->ipv6_float) > 0) {
        debug_log(DLOG_DEBUG, "%s: update http float_port(%d), ipv6(%s).\n", __FUNCTION__, configure->http_port,
            configure->ipv6_float);
        write_service_listen_addr(fp, (const gchar *)configure->ipv6_float, configure->http_port, NULL, FALSE);
    }

    return;
}


LOCAL void write_http_listen_addr(HTTP_CONF_S *configure)
{
    gint32 ret;
    FM_FILE_S *fp = NULL;
    gchar buffer[MAX_BUFF_SIZE] = { 0 };

    fp = fm_fopen(HTTP_CONF_NAME, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: fm_open failed", __FUNCTION__);
        return;
    }

    if (configure->state == VHOST_DISABLE) {
        fm_fclose(fp);
        return;
    }

    ret = fm_fwrite(SERVER_BEGAIN, 1, strlen(SERVER_BEGAIN), fp);
    if (ret != strlen(SERVER_BEGAIN)) {
        debug_log(DLOG_ERROR, "%s:write http server begain fail.", __FUNCTION__);
    }

    // 检查当前ipv6是否处于SLACC模式, 检查失败或者处于该模式则监听任意地址
    if (configure->is_slaac == TRUE) {
        write_service_listen_addr(fp, VHOST_DEFAULT_OUTER_IP, configure->http_port, NULL, FALSE);
        goto EXIT;
    }

    // 写入外部服务的监听地址
    ret = write_http_listen_out_addr(fp, configure);
    if (ret != RET_OK) {
        write_service_listen_addr(fp, VHOST_DEFAULT_OUTER_IP, configure->http_port, NULL, FALSE);
        goto EXIT;
    }

    // 写入对BMA服务的监听地址
    write_bma_http_listen(fp, configure);

    write_FusionPoD_inner_http_listen(fp, configure);

    
    if (configure->net_type != NET_TYPE_DEDICATED) {
        if (strlen(configure->ip_maint)) {
            // 获取近端运维口成功获取时维护标志位，表示需要将近端运维IP加入http服务侦听地址列表
            write_service_listen_addr(fp, configure->ip_maint, configure->http_port, NULL, FALSE);
        }
    }

    write_float_ip_http_listen(fp, configure);

EXIT:
    ret = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, SERVER_END_HTTP, configure->https_port);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
    }
    ret = fm_fwrite(buffer, 1, strlen(buffer), fp);
    if (ret != strlen(buffer)) {
        debug_log(DLOG_ERROR, "%s:write http server end fail.", __FUNCTION__);
    }
    fm_fclose(fp);
}

void update_http_configuration(void)
{
    
    gint32 ret;
    HTTP_CONF_S configure;
    static HTTP_CONF_S previous_configure = { 0 };

    ret = get_http_configuration(&configure);
    if (ret == RET_ERR || memcmp(&configure, &previous_configure, sizeof(configure)) == 0) {
        debug_log(DLOG_ERROR, "Nothing to be done for http configuration.");
        return;
    }
    (void)memcpy_s(&previous_configure, sizeof(previous_configure), &configure, sizeof(configure));

    
    write_http_listen_addr(&configure);
    g_web_server_state = (g_web_server_state == VHOST_START) ? VHOST_RESTART : g_web_server_state;
}


gint32 check_object_is_specific_out_type(OBJ_HANDLE obj_handle, guint8 type)
{
    gchar class_name[MAX_NAME_SIZE] = { 0 };
    gint32 ret = RET_OK;
    guint8 out_type = 0xFF;

    if (obj_handle == OBJ_HANDLE_COMMON) {
        debug_log(DLOG_DEBUG, "%s: input param error", __FUNCTION__);
        return RET_ERR;
    }

    ret = dfl_get_class_name(obj_handle, class_name, MAX_NAME_SIZE);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s: dfl_get_class_name failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (g_strcmp0(class_name, ETH_CLASS_NAME_ETHGROUP) != 0) {
        debug_log(DLOG_DEBUG, "%s: class name %s not match", __FUNCTION__, class_name);
        return RET_ERR;
    }

    ret = dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_OUT_TYPE, &out_type);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get prop value failed, ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    if (type == out_type) {
        return RET_OK;
    }

    return RET_ERR;
}


void config_https_verify_client(void)
{
    gint32 retv = 0;
    FM_FILE_S *fp = NULL;
    gchar content[BUFFER_SIZE_2K] = { 0 };
    const gchar *ssl_file = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 mutual_enable = 0;
    guint8 mutual_ocsp = 0;
    guint8 mutual_crl = 0;

    
    retv = dal_get_object_handle_nth(MUTUAL_AUTH_CLASS_NAME, 0, &obj_handle);
    if (retv != RET_OK) {
        debug_log(DLOG_ERROR, "get MutualAuthentication object handle failed, ret:%d.", retv);
        return;
    }

    (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_STATE, &mutual_enable);
    (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_OCSP, &mutual_ocsp);
    (void)dal_get_property_value_byte(obj_handle, MUTUAL_AUTH_CRL, &mutual_crl);

    // CA文件不存在，双因素使能会导致web服务异常
    if (!vos_get_file_accessible(VERIFY_ROOT_CERT) && mutual_enable) {
        mutual_enable = 0;
        mutual_crl = 0;
        debug_log(DLOG_ERROR, "no CA list file, mutual auth can't be enabled.");
    }

    if (mutual_enable == 0) {
        mutual_crl = 0;
    }
    // SSL CRL文件存在,使用SSL CRL文件，不存在使用SSL CRT文件
    ssl_file = vos_get_file_accessible(VERIFY_CRL_CERT) ? VIRTUAL_HOST_CRL_CONF : VIRTUAL_HOST_CRT_CONF;

    retv = snprintf_s(content, BUFFER_SIZE_2K, BUFFER_SIZE_2K - 1, VIRTUAL_HOST_EXT, (mutual_enable ? "on" : "off"),
        (mutual_enable ? VIRTUAL_HOST_CA_CONF : ""), (mutual_ocsp ? "on" : "off"), (mutual_crl ? ssl_file : ""));
    if (retv <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d\n", __FUNCTION__, retv);
    }

    fp = fm_fopen(HTTPS_VERIFY_CLIENT_CONF_NAME, "w+");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "open conf file failed.");
        return;
    }

    retv = fm_fwrite(content, 1, strlen(content), fp);
    if (retv != strlen(content)) {
        fm_fclose(fp);
        debug_log(DLOG_ERROR, "%s:write file(length:%zu) fail:%d.\n", __FUNCTION__, strlen(content), retv);
        return;
    }

    fm_fclose(fp);
    (void)chown(HTTPS_VERIFY_CLIENT_CONF, APACHE_UID, APACHE_GID);

    if (g_web_server_state == VHOST_START) {
        g_web_server_state = VHOST_RESTART;
    }
    debug_log(DLOG_DEBUG, "[%s]currently g_web_server_state is %d\n", __FUNCTION__, g_web_server_state);
}

LOCAL gboolean is_link_local_addr(const gchar *ipv6Str)
{
    struct in6_addr addr_info;
    gint32 ret;

    ret = inet_pton(AF_INET6, (const gchar *)ipv6Str, &addr_info);
    if (ret != 1) {
        return FALSE;
    }

    if (IN6_IS_ADDR_LINKLOCAL(&addr_info) == TRUE) {
        return TRUE;
    }

    return FALSE;
}


void get_local_link_addr_and_ethname(gchar *ip_addr, guint32 ip_len, gchar *eth_name, guint32 ethname_len)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guint8 active_port = 0;
    errno_t secure_rv;
    guint32 vlan_state = 0;
    guint16 vlan_id = 0;
    OBJ_HANDLE obj_eth = 0;

    if (ip_addr == NULL || ip_len == 0 || eth_name == NULL || ethname_len == 0) {
        return;
    }

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETHGROUP, ETH_GROUP_ATTRIBUTE_OUT_TYPE, OUTTER_GROUP_TYPE,
        &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get outter eth group object failed, ret:%d", __FUNCTION__, ret);
        return;
    }

    (void)dal_get_property_value_string(obj_handle, ETH_GROUP_ATTRIBUTE_IPV6_LOCAL_LINK, ip_addr, ip_len);
    if (is_link_local_addr(ip_addr) == FALSE) {
        goto EXIT;
    }

    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_ACTIVE_PORT, &active_port);

    ret = dal_get_specific_object_byte(ETH_CLASS_NAME_ETH, ETH_ATTRIBUTE_NUM, active_port, &obj_eth);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:get active Eth object handle, ret:%d", __FUNCTION__, ret);
        goto EXIT;
    }

    (void)dal_get_property_value_uint32(obj_eth, ETH_ATTRIBUTE_VLAN_STATE, &vlan_state);
    (void)dal_get_property_value_uint16(obj_eth, ETH_ATTRIBUTE_VLAN_ID, &vlan_id);

    if (vlan_state != 0 && vlan_id != 0) {
        secure_rv = snprintf_s(eth_name, ethname_len, ethname_len - 1, "eth%u.%u", active_port, vlan_id);
    } else {
        secure_rv = snprintf_s(eth_name, ethname_len, ethname_len - 1, "eth%u", active_port);
    }

    if (secure_rv > 0) {
        return;
    }

EXIT:
    secure_rv = memset_s(ip_addr, ip_len, 0, ip_len);
    if (secure_rv != EOK) {
        debug_log(DLOG_ERROR, "%s: call memset_s failed, ret:%d", __FUNCTION__, secure_rv);
    }
}


LOCAL gint32 is_listen_at_default_ip(guchar *software_type)
{
    
    *software_type = 0;
    gint32 ret;

    ret = dal_get_software_type(software_type);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get software_type error, ret = %d", __FUNCTION__, ret);
        return RET_ERR;
    }
    if (*software_type == MGMT_SOFTWARE_TYPE_PANGEA_ALANTIC_CTRL ||
        *software_type == MGMT_SOFTWARE_TYPE_PANGEA_PACIFIC_CTRL || *software_type == MGMT_SOFTWARE_TYPE_PANGEA_MGNT) {
        return RET_OK;
    }
    return RET_ERR;
}


LOCAL void check_is_out_service_listen_default(HTTPS_CONF_S https_conf, gboolean *is_listen_default)
{
    gint32 ret;
    gboolean is_slaac;

    if (https_conf.state_out != VHOST_ENABLE || is_listen_default == NULL) {
        return;
    }

    // 检查ipv6地址是否开启SLAAC模式,若检查失败或已开启,则监听所有地址
    ret = is_ipv6_slaac_mode(&is_slaac);
    if (ret != RET_OK || is_slaac == TRUE) {
        *is_listen_default = TRUE;
        return;
    }

    // 如果外部服务IP有一个为默认IP或IP均为空, 则直接监听默认IP(一般异常场景才会出现此种情况)
    if (g_strcmp0(https_conf.ip_out, VHOST_DEFAULT_OUTER_IP) == 0 ||
        g_strcmp0(https_conf.ipv6_out, VHOST_DEFAULT_OUTER_IP) == 0) {
        *is_listen_default = TRUE;
    }

    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        if (https_conf.ip_out_num == 0 && https_conf.ipv6_out_num == 0) {
            *is_listen_default = TRUE;
        }
    } else {
        if (strlen(https_conf.ip_out) == 0 && strlen(https_conf.ipv6_out) == 0) {
            *is_listen_default = TRUE;
        }
    }

    guint8 software_type = 0;
    if (is_listen_at_default_ip(&software_type) == RET_OK) {
        *is_listen_default = TRUE;
    }
    debug_log(DLOG_DEBUG, "%s: result(%d).", __FUNCTION__, *is_listen_default);
}

LOCAL void __concatenate_ssl_str(gchar *ssl_str, gint32 ssl_str_len, gboolean is_https)
{
    const gchar *default_server = g_is_writing_default_server ? " default_server" : "";
    const gchar *ssl = is_https ? " ssl" : "";
    const gchar *reuseport = (!g_is_writing_default_server) ? " reuseport" : "";

    gint32 ret = snprintf_s(ssl_str, ssl_str_len, ssl_str_len - 1, "%s%s%s", default_server, ssl, reuseport);
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: snprintf_s failed, ret: %d", __FUNCTION__, ret);
    }
}


LOCAL void write_service_listen_addr(FM_FILE_S *fp, const gchar *ipaddr, guint16 port, const gchar *eth_name,
    gboolean is_https)
{
    gint32 ret = 0;
    gint32 is_ipv6 = 0;
    gchar buffer[BUFFER_SIZE_1K] = { 0 };
    gchar ssl_str[SSL_STR_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    guint8 ip_ver = ENABLE_IPV4_AND_IPV6;

    debug_log(DLOG_DEBUG, "%s: port(%d), ip(%s)", __FUNCTION__, port, ipaddr);

    if (ipaddr == NULL || strlen(ipaddr) == 0) {
        return;
    }

    if (strstr(ipaddr, ":") != NULL) {
        is_ipv6 = 1;
    }

    __concatenate_ssl_str(ssl_str, SSL_STR_LEN, is_https);

    (void)dal_get_object_handle_nth(ETH_CLASS_NAME_ETHGROUP, 0, &obj_handle);
    (void)dal_get_property_value_byte(obj_handle, ETH_GROUP_ATTRIBUTE_IP_VERSION, &ip_ver);

    // 链路本地地址
    if (eth_name != NULL) {
        ret = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "listen [%s%%%s]:%u%s;\r\n", ipaddr, eth_name,
            port, ssl_str);
        goto DONE;
    }

    // 全地址
    if (g_strcmp0(ipaddr, VHOST_DEFAULT_OUTER_IP) == 0) {
        if (ip_ver == ENABLE_IPV4_ONLY || ip_ver == ENABLE_IPV4_AND_IPV6) {
            ret = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, "listen *:%u%s;\r\n", port, ssl_str);
        }

        if (ip_ver == ENABLE_IPV6_ONLY || ip_ver == ENABLE_IPV4_AND_IPV6) {
            ret += snprintf_s(buffer + strlen(buffer), sizeof(buffer) - strlen(buffer),
                sizeof(buffer) - strlen(buffer) - 1, "listen [::]:%u%s;\r\n", port, ssl_str);
        }

        goto DONE;
    }

    // IP地址
    if ((is_ipv6 && ip_ver == ENABLE_IPV4_ONLY) || (!is_ipv6 && ip_ver == ENABLE_IPV6_ONLY)) {
        return;
    }
    ret = snprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1,
        is_ipv6 ? "listen [%s]:%u%s;\r\n" : "listen %s:%u%s;\r\n", ipaddr, port, ssl_str);

DONE:
    if (ret <= 0) {
        debug_log(DLOG_ERROR, "%s: buffer snprintf_s fail, ret = %d\n", __FUNCTION__, ret);
    }
    ret = fm_fwrite(buffer, 1, strlen(buffer), fp);
    if (ret != strlen(buffer)) {
        debug_log(DLOG_ERROR, "%s:write file(length:%zu) fail:%d", __FUNCTION__, strlen(buffer), ret);
    }
}


LOCAL void write_out_service_listen_addr(FM_FILE_S *fp, HTTPS_CONF_S https_conf, gboolean *is_listen_default)
{
    guint32 i;
    guchar software_type = 0;
    guchar as_status = ASM_ACTIVE;

    if (https_conf.state_out != VHOST_ENABLE || fp == NULL || is_listen_default == NULL) {
        return;
    }

    // 带内BOB服务访问web需要访问443端口
    if (https_conf.state_bma == VHOST_ENABLE) {
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_bma, https_conf.port_out, NULL, TRUE);
        write_service_listen_addr(fp, (const gchar *)https_conf.ipv6_bma, https_conf.port_out,
            strcmp((const gchar *)https_conf.ipv6_bma, VHOST_DEFAULT_SMS_IPADDR) == 0 ? "veth" : NULL, TRUE);
    }

    check_is_out_service_listen_default(https_conf, is_listen_default);

    if (*is_listen_default == TRUE) {
        write_service_listen_addr(fp, VHOST_DEFAULT_OUTER_IP, https_conf.port_out, NULL, TRUE);
        return;
    }

    if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
        for (i = 0; i < https_conf.ip_out_num; i++) {
            write_service_listen_addr(fp, (const gchar *)https_conf.ip_out_multi[i], https_conf.port_out, NULL, TRUE);
        }

        for (i = 0; i < https_conf.ipv6_out_num; i++) {
            write_service_listen_addr(fp, (const gchar *)https_conf.ipv6_out_multi[i], https_conf.port_out, NULL, TRUE);
        }
    } else {
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_out, https_conf.port_out, NULL, TRUE);
        write_service_listen_addr(fp, (const gchar *)https_conf.ipv6_out, https_conf.port_out, NULL, TRUE);
    }

    // 若为PoD产品，则尝试添加柜内通信IP的侦听配置
    if (https_conf.state_rack_inner == VHOST_ENABLE) {
        debug_log(DLOG_INFO, "add %s, %s to https list", https_conf.ip_rack_inner, https_conf.ip_rack_inner_bak);
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_rack_inner, https_conf.port_out, NULL, TRUE);
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_rack_inner_bak, https_conf.port_out, NULL, TRUE);
    }

    // 若近端维护IP可用，则尝试添加侦听配置
    if (https_conf.state_maint == VHOST_ENABLE) {
        debug_log(DLOG_INFO, "%s: add %s to https listen list", __FUNCTION__, https_conf.ip_maint);
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_maint, https_conf.port_out, NULL, TRUE);
    }

    (void)dal_get_software_type(&software_type);
    (void)get_board_active_state(&as_status);
    if ((software_type == MGMT_SOFTWARE_TYPE_EM) && (as_status == ASM_ACTIVE)) {
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_float, https_conf.port_out, NULL, TRUE);
        write_service_listen_addr(fp, (const gchar *)https_conf.ipv6_float, https_conf.port_out, NULL, TRUE);
    }

    
    write_service_listen_addr(fp, (const gchar *)https_conf.ip_local_link, https_conf.port_out,
        (const gchar *)https_conf.eth_name, TRUE);

    
    if (https_conf.state_usb_mgmt == VHOST_ENABLE) {
        write_service_listen_addr(fp, (const gchar *)https_conf.ip_usb_mgmt, https_conf.port_out, NULL, TRUE);
    }

    *is_listen_default = FALSE;
}


LOCAL void write_inner_service_listen_addr(FM_FILE_S *fp, HTTPS_CONF_S configure, gboolean is_listen_default)
{
    guint32 i;
    guchar software_type = MGMT_SOFTWARE_TYPE_BLADE;

    if ((configure.state_inner == VHOST_DISABLE) || (fp == NULL) ||
        (strlen(configure.ip_inner) + configure.ip_inner_num == 0)) {
        return;
    }

    (void)dal_get_software_type(&software_type);

    // 由于v5刀片与新旧版本管理板通信端口号不同,因此内部IP需同时监听443(与旧版本HMM通信端口)和40443(与新版本HMM通信端口)两个端口
    // 监听40443端口
    if (is_listen_default == FALSE || configure.port_out != configure.port_inner1) {
        if (dal_is_support_eth_define_specific_propery(PROPERTY_SUPPORT_MULTI_VLAN)) {
            for (i = 0; i < configure.ip_inner_num; i++) {
                write_service_listen_addr(fp, (const gchar *)configure.ip_inner_multi[i], configure.port_inner1, NULL,
                    TRUE);
            }

            
            return;
        } else {
            write_service_listen_addr(fp, (const gchar *)configure.ip_inner, configure.port_inner1, NULL, TRUE);
        }
    }

    // 监听443端口(EM无需监听)
    if (software_type != MGMT_SOFTWARE_TYPE_EM) {
        if (is_listen_default == FALSE || configure.port_out != configure.port_inner2) {
            write_service_listen_addr(fp, (const gchar *)configure.ip_inner, configure.port_inner2, NULL, TRUE);
        }
    }
}

gboolean is_webserver_alive(const gchar *pid_file)
{
    gint32 ret;
    FILE *fp = NULL;
    guint32 webserver_pid = 0;
    gchar buf[MAX_INFO_LEN] = { 0 };
    gchar path[MAX_INFO_LEN] = { 0 };
    size_t buf_len;

    gchar *real_path = realpath(pid_file, NULL);
    if (real_path == NULL) {
        debug_log(DLOG_ERROR, "%s: check realpath failed", __FUNCTION__);
        return FALSE;
    }

    fp = fopen(real_path, "r");
    g_free(real_path);
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s %d: open webserver pid file failed", __FUNCTION__, __LINE__);
        return FALSE;
    }

    if (vos_fgets_s(buf, sizeof(buf) - 1, fp) == NULL) {
        debug_log(DLOG_ERROR, "%s %d: read webserver pid file failed", __FUNCTION__, __LINE__);
        (void)fclose(fp);
        return FALSE;
    }

    (void)fclose(fp);
    buf_len = strlen(buf);
    if (buf_len == 0) {
        debug_log(DLOG_ERROR, "%s : get buf len failed", __FUNCTION__);
        return FALSE;
    }
    buf[buf_len - 1] = '\0'; // 清除末尾换行符

    ret = vos_str2int(buf, 10, &webserver_pid, NUM_TPYE_UINT32);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : vos_str2int failed", __FUNCTION__);
        return FALSE;
    }

    (void)sprintf_s(path, sizeof(path), "/proc/%u", webserver_pid);
    if (vos_check_dir(path) == FALSE) {
        (void)vos_rm_filepath(pid_file);
        debug_log(DLOG_ERROR, "%s : not find proc dir", __FUNCTION__);
        return FALSE;
    }

    return TRUE;
}

void enable_disable_web_service(void)
{
    gboolean flag;
    if (g_https_enable_flag != SERVICE_START) {
        return;
    }

    debug_log(DLOG_DEBUG, "g_webserver_update_flag = %d", g_webserver_update_flag);

    set_webserver_log();

    flag = get_webserver_update_flag(HTTPS_TLS_CONFIG_MASK);
    if (flag == TRUE) {
        clear_webserver_update_flag(HTTPS_TLS_CONFIG_MASK);
        (void)config_https_tls_protocol();
    }

    flag = get_webserver_update_flag(HTTP_CONFIG_MASK);
    if (flag == TRUE) {
        clear_webserver_update_flag(HTTP_CONFIG_MASK);
        update_http_configuration();
    }

    flag = get_webserver_update_flag(HTTPS_CONFIG_MASK);
    if (flag == TRUE) {
        clear_webserver_update_flag(HTTPS_CONFIG_MASK);
        update_https_configuration();
    }

    flag = get_webserver_update_flag(HTTPS_VERIFY_CLIENT_CONFIG_MASK);
    if (flag == TRUE) {
        clear_webserver_update_flag(HTTPS_VERIFY_CLIENT_CONFIG_MASK);
        config_https_verify_client();
    }

    switch (g_web_server_state) {
        case VHOST_START:
            start_webserver();
            break;

        case VHOST_RESTART:
            if (restart_webserver() == RET_OK) {
                g_web_server_state = VHOST_START;
            }
            break;

        case VHOST_STOP:
            stop_webserver();
            break;
    }
}
