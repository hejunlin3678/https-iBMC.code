
#ifndef __WEBMANAGER_H__
#define __WEBMANAGER_H__

#include "pme_app/pme_app.h"

#define WEB_SERVER_PID_FILE "/opt/pme/web/logs/nginx.pid"
#define MAX_VHOST_IP_NUM 8
#define SHM_LOG_PATH "/dev/shm/log"
#define NGINX_LOG_PATH "/dev/shm/log/web"
#define ACCESS_LOG "/dev/shm/log/web/access_log"
#define ACCESS_LOG_BAK "/dev/shm/log/web/access_log.1"
#define ERROR_LOG "/dev/shm/log/web/error_log"
#define ERROR_LOG_BAK "/dev/shm/log/web/error_log.1"
#define SHM_LOG_FILE_MOD 0755
#define NGINX_LOG_MOD 0600

#define HTTPS_TLS_SHORT_FILE_NAME "ssl_protocol"
#define HTTPS_TLS_FILE "/data/opt/pme/web/conf/nginx_ssl_protocol.conf"
#define HTTPS_SSL_CERT_PATH_CONF "nginx_https_defualt_server_ext.conf"

#define BUFFER_SIZE_1K 1024
#define BUFFER_SIZE_2K 2048

typedef struct tag_http_conf_s {
    guint8 state;
    guint16 http_port;
    guint16 https_port;
    gboolean is_slaac;
    gchar ip_local_link[MAX_IPV6_LEN];
    gchar eth_name[STRING_LEN_MAX];
    gchar usb_ip_addr[MAX_IPV4_LEN];

    gchar ipv4_out[MAX_IPV4_LEN];
    gchar ipv6_out[MAX_IPV6_LEN];
    guint8 support_multi_enable;
    gchar ipv4_multi_out[MAX_VHOST_IP_NUM][MAX_IPV4_LEN];
    gchar ipv6_multi_out[MAX_VHOST_IP_NUM][MAX_IPV6_LEN];
    guchar ipv4_num_out;
    guchar ipv6_num_out;

    guchar host_enable_bma;
    gchar ipv4_bma[MAX_IPV4_LEN];
    gchar ipv6_bma[MAX_IPV6_LEN];

    guchar net_type;
    gchar ipv4_inner_main[MAX_IPV4_LEN];
    gchar ipv4_inner_bak[MAX_IPV4_LEN];
    guint8 irm_heartbeat_enable;

    gchar ip_maint[VHOST_IP_LEN]; // 近端运维IP

    guchar software_type;
    guchar as_status;
    gchar ipv4_float[MAX_IPV4_LEN];
    gchar ipv6_float[MAX_IPV6_LEN];
} HTTP_CONF_S;

typedef struct tag_https_conf_s {
    guint8 mutual_enable;
    guint8 mutual_ocsp;
    guint8 state_out;
    guint8 state_bma;
    guint8 state_inner;
    guint8 state_rack_inner; // FusionPoD柜内通信支持状态
    guint8 state_maint;      // 近端运维网口支持状态
    guint8 state_usb_mgmt;   // USB管理网口支持状态
    guint16 port_out;
    guint16 port_bma;
    guint16 port_inner1; // 与新版本HMM(HMM920/HMM921)通信的内部端口
    guint16 port_inner2; // 与旧版本HMM通信的内部端口

    gchar ip_out_multi[MAX_VHOST_IP_NUM][MAX_IPV4_LEN]; 
    guchar ip_out_num;

    gchar ip_out[VHOST_IP_LEN]; 
    gchar ip_bma[VHOST_IP_LEN];

    gchar ip_inner_multi[MAX_VHOST_IP_NUM][MAX_IPV4_LEN]; 
    guchar ip_inner_num;

    gchar ip_inner[VHOST_IP_LEN];          
    gchar ip_rack_inner[VHOST_IP_LEN];     // FusionPoD柜内通信主路IP
    gchar ip_rack_inner_bak[VHOST_IP_LEN]; // FusionPoD柜内通信备路IP
    gchar ip_maint[VHOST_IP_LEN];          // 近端运维IP
    gchar ip_usb_mgmt[VHOST_IP_LEN];       // USB管理网口IP
    guint8 mutual_crl;

    gchar ipv6_out_multi[MAX_VHOST_IP_NUM][MAX_IPV6_LEN]; 
    guchar ipv6_out_num;

    gchar ipv6_out[VHOST_IP_LEN]; 
    gchar ipv6_bma[VHOST_IP_LEN];
    gchar ip_float[VHOST_IP_LEN];
    gchar ipv6_float[VHOST_IP_LEN];
    gchar ip_local_link[MAX_IPV6_LEN];
    gchar eth_name[STRING_LEN_MAX];
    guchar web_enable;
    guint8 ip_version;
} HTTPS_CONF_S;



gint32 ConfigureVirtualHost(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
gint32 MutualAuthRootCertChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 MutualAuthCrlFilePathChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 HostNameDomainNameDNSRebindChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);


void service_rollback_web_server_configure(void);

void stop_webserver(void);
gint32 IpAddrChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
gint32 NetTypeChangeCallback(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
gint32 CertStatusChange(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
gint32 ConfigureSSLCipherSuite(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
void init_ssl_cipher_suites(void);
gint32 check_object_is_specific_out_type(OBJ_HANDLE obj_handle, guint8 type);
void get_local_link_addr_and_ethname(gchar *ip_addr, guint32 ip_len, gchar *eth_name, guint32 ethname_len);
void update_http_configuration(void);
gint32 config_https_tls_protocol(void);
void update_https_configuration(void);
void config_https_verify_client(void);
void enable_disable_web_service(void);
gint32 is_ipv6_slaac_mode(gboolean *is_slaac);
gboolean is_webserver_alive(const gchar *pid_file);
#endif