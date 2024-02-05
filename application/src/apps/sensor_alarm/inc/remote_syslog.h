

#ifndef SENSOR_REMOTE_SYSLOG_H
#define SENSOR_REMOTE_SYSLOG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

extern gint32 method_syslog_set_enable_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_msg_format(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_msg_identity(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_msg_level_mask(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_auth_type(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_import_cert(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_get_cert_status(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_get_cert_info(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_msg_protocol(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_net_protocol(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_set_src_mask(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_import_crl(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_syslog_delete_crl(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

extern gint32 method_rsyslog_set_dest_addr(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_rsyslog_set_dest_port(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_rsyslog_set_enable_state(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 method_rsyslog_set_test_trigger(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern void syslog_restart_delay(gint32 delay_ms);
extern gint32 syslog_identity_handler(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern void syslog_init(void);
gint32 generate_remote_syslog_conf(void);


extern gint32 property_sync_syslog_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 property_sync_syslog_conf(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);

void syslog_check_domain_name_valid(void);

#define SYSLOG_MGNT_ENABLE 1
#define SYSLOG_MGNT_DISABLE 0


typedef enum {
    SYSLOG_MGNT_MSG_FORMAT_CUSTOM = 0,
    SYSLOG_MGNT_MSG_FORMAT_RFC3164 = 1
} SYSLOG_MSG_FORMAT_TYPE;

#define SYSLOG_MGNT_SRC_OP 0x01
#define SYSLOG_MGNT_SRC_SEC 0x02
#define SYSLOG_MGNT_SRC_SEL 0x04


typedef enum {
    SYSLOG_MGNT_MSG_IDEN_SN = 0x01,
    SYSLOG_MGNT_MSG_IDEN_ASSETTAG = 0x02,
    SYSLOG_MGNT_MSG_IDEN_HOSTNAME = 0x03
} SYSLOG_MSG_IDENTITY_TYPE;
#define SYSLOG_MGNT_MSG_IDEN_CHASSIS_NAME 0x04
#define SYSLOG_MGNT_MSG_IDEN_CHASSIS_LOCATION 0x05
#define SYSLOG_MGNT_MSG_IDEN_CHASSIS_SERIAL_NUMBER 0x06


#define SYSLOG_MGNT_LEVEL_CRITICAL 0x08
#define SYSLOG_MGNT_LEVEL_MAJOR 0x04
#define SYSLOG_MGNT_LEVEL_MINOR 0x02
#define SYSLOG_MGNT_LEVEL_NORMAL 0x01


#define SYSLOG_MSG_PROTOCOL_BSD 1
#define SYSLOG_MSG_PROTOCOL_IETF 2

#define SYSLOG_MSG_FMT_STR(x) ((x) ? "rfc3164" : "custom")

#define REMOTE_MAX_ADDR_LEN 128

#define SYSLOG_MGNT_CONF_MAX_SIZE 16384 

typedef struct tag_dest_net_parm {
    guint8 net_protocol;
    guint32 port;
    gchar ip_addr[HOST_ADDR_MAX_LEN + 1];
    guint8 ip_mode;
} DEST_NET_PARM;
#define SYSLOG_DEST_NET_PTL(x) ((x) == SYSLOG_MGNT_NET_PTL_UDP ? "udp" : "tcp")
#define SYSLOG_DEST_NET_MODE(x) ((x) == HOST_ADDR_TYPE_IPV6 ? "6" : "")
#define REMOTE_OSS_SERVER "d_remote_oss_network_server"
#define REMOTE_OSS_SERVER_EXT "d_remote_oss_network_server_ext"
#define REMOTE_TEMPLATE "template(t_r1)"
#define REMOTE_TEMPLATE_EXT "template(t_r2)"
#define REMOTE_PERSIST_OSS "d_remote_oss"
#define REMOTE_PERSIST_OSS_EXT "d_remote_oss_ext"

// #oss log static
#define SOURCE_OPERATOR "source(s_sys); filter(f_operate);"
#define SOURCE_SECURITY "source(s_sys); filter(f_auth);"
#define SOURCE_SEL "source(s_sys); filter(f_syslog_test); filter(f_sel); filter(f_sel_severity);"

#define FILTER_SEL_MSG "filter(f_sel);"
#define FILTER_SEL_SEVE "filter(f_sel_severity);"
#define FILTER_SEL_SEVE_HEAD "filter f_sel_severity { "
#define FILTER_SEL_MATCH_NORMAL "match(\",Normal,\" value(\"MESSAGE\"))"
#define FILTER_SEL_MATCH_MINOR "match(\",Minor,\" value(\"MESSAGE\"))"
#define FILTER_SEL_MATCH_MAJOR "match(\",Major,\" value(\"MESSAGE\"))"
#define FILTER_SEL_MATCH_CRITICAL "match(\",Critical,\" value(\"MESSAGE\"))"

#define FILTER_BUF_LEN 1024

#define REMOTE_SYSLOG_CONF_PATH "/data/etc/remotelog.conf" 
#define SYSLOG_TMP_PEM_PATH "/data/etc/syslog_client.pem"
#define CERT_ITEM_BUFF_LEN 512

#define SYSLOG_FILE_PATH_CA_DIR "/data/etc/syslog-ng/ca.d"
#define SYSLOG_FILE_PATH_CA "/data/etc/syslog-ng/ca.d/ca.crt"
#define SYSLOG_FILE_PATH_KEY_DIR "/data/etc/syslog-ng/key.d"
#define SYSLOG_FILE_PATH_KEY "/data/etc/syslog-ng/key.d/client.pem"
#define SYSLOG_FILE_PATH_CERT_DIR "/data/etc/syslog-ng/cert.d"
#define SYSLOG_FILE_PATH_CERT "/data/etc/syslog-ng/cert.d/client.pem"
#define SYSLOG_FILE_PATH_CRL_DIR "/data/etc/syslog-ng/crl.d"

#define SYSLOG_DST_ROOT_CA "ca_dir(\"/data/etc/syslog-ng/ca.d\")"
#define SYSLOG_DST_CLIENT_CERT "cert_file(\"/data/etc/syslog-ng/cert.d/client.pem\")"
#define SYSLOG_DST_CLIENT_KEY "key_file(\"/data/etc/syslog-ng/key.d/client.pem\")"
#define SYSLOG_DST_CRL_PATH "crl_dir(\"/data/etc/syslog-ng/crl.d\")"

#define LOG_DIR "/var/log/pme"


#define SYSLOG_CONFIG_FORMAT_FILTER_HEAD "#test log static\r\n"
#define SYSLOG_CONFIG_FORMAT_FILTER_SERVER_MATCH "filter f_t%d { match(\"server %d test\" value(\"MESSAGE\"));};\r\n"
#define SYSLOG_CONFIG_FORMAT_FILTER_TAIL "\r\nfilter f_operate { facility(local5) and level(info); };\r\n"          \
        "filter f_sel { match(\",Asserted,\" value(\"MESSAGE\")) or match(\",Deasserted,\" value(\"MESSAGE\"));};\r\n"
#define SYSLOG_CONFIG_FORMAT_DESTINATION_ITEM \
    "log{source(s_sys);filter(f_syslog_test);filter(f_t%d);destination(d_remote_oss_network_server%d);};\r\n" 
#define SYSLOG_CONFIG_CUSTOM_FMT "template t_r1{template(\"<$PRI> %s %s %s: ${MSG}\\n\");template_escape(no);};\r\n" \
        "template t_r2{template(\"<$PRI> %s %s %s: ${ISODATE} ${HOST} "                 \
        "${MSGHDR}${MSG}\\n\");template_escape(no);};\r\n%s"   \
        "%s\r\n%s\r\n" 
#define SYSLOG_CONFIG_RFC3164_FMT                                                                      \
    "template t_r1{template(\"<$PRI>${DATE} ${HOST} ${MSGHDR} ${MSG}\\n\");template_escape(no);};\r\n" \
        "template t_r2{template(\"<$PRI>${DATE} ${HOST} ${MSGHDR} "                                    \
        "${MSG}\\n\");template_escape(no);};\r\n%s"                          \
        "%s\r\n%s\r\n"
        
typedef struct tag_syslog_buf_info {
    gchar *gen_buf;
    gint32 buf_len;
} R_SYS_BUFF_INFO_ST;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif
