

#ifndef __REDFISH_HTTP_H__
#define __REDFISH_HTTP_H__

#include <json.h>
#include "redfish_defs.h"
#include "redfish_provider.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define REDFISH_OBJECT ("RedfishObject")
#define ALLOWED_OPERATION ("AllowedOperation")


#define RESP_HEADER_ODATA_VERSION "OData-Version"
#define RESP_HEADER_CONTENT_TYPE "Content-Type"
#define RESP_HEADER_ETAG "ETag"
#define RESP_HEADER_SERVER "Server"
#define RESP_HEADER_SETCOOKIE "Set-Cookie"
#define RESP_HEADER_LINK ("Link")
#define RESP_HEADER_LOCATION ("Location")
#define RESP_HEADER_ALLOW ("Allow")
#define RESP_HEADER_X_AUTH_TOKEN ("X-Auth-Token")
#define RESP_HEADER_FROM ("From")
#define RESP_HEADER_TOKEN ("Token")
#define RESP_HEADER_ALLOW_ORIGIN ("Access-Control-Allow-Origin")
#define REQUEST_SMS ("SmsId")
#define REQUEST_SERVICE_ROOT ("ServiceRoot")

#define JSON_SCHEMA_MEMBERS_COUNT "Members@odata.count"
#define JSON_SCHEMA_MEMBERS_NEXT_LINK "Members@odata.nextLink"
#define JSON_SCHEMA_MEMBERS_MAX 32
#define QUERY_TOP "$top"
#define QUERY_SKIP "$skip"

#define QUERY_EXPAND "$expand"
#define QUERY_EXPAND_LEVELS "($levels"
#define MULTI_KEY_MAX_NUM 3 // 支持 $ 开头的组合key值最大个数
#define URL_REVERSE_BRACKETS ")"
#define EXPAND_ASTERISK '*'
#define EXPAND_TILDE '~'
#define EXPAND_PERIOD '.'
#define NAVIGATION_PROP_OBJ_LEN 1


#define CONFG_FILE_FLASH "/opt/pme/extern/redfish/redfish.tar.bz2"

#define CONFG_FILE_PREFIX "/data/opt/pme/conf"
#define CONFG_FILE_PATH_OLD "/opt/pme/pram/redfish"
#define CONFG_FILE_PATH "/data/opt/pme/conf/redfish"
#define CONFG_FILE_SUFFIX "index.json"
#define CONFG_FILE_SCHEMASTORE_EN "/data/opt/pme/conf/redfish/v1/schemastore/en"

#define METADATA_URI_PREFIX "/redfish/v1/$metadata"
#define METADATA_URI_PREFIX2 "/redfish/v1/$metadata#"
#define LINK_PREFIX "/redfish/v1/SchemaStore/en"
#define SCHEMA_MEMBER_SUFFIX "template/index.json"

#define SCHEMA_PREFIX "/redfish/v1/jsonschemas/"

#define SCHEMASTORE_PREFIX "/redfish/v1/schemastore"
#define REGISTRY_STORE_PREFIX "/redfish/v1/registrystore/messages"
#define METADATA_PREFIX "/redfish/v1/metadata"
#define ODATA_PREFIX "/redfish/v1/odata"
#define REGISTRIES_PREFIX "/redfish/v1/registries"
#define SMS_PREFIX "/redfish/v1/sms/"


#define SESSIONS_PREFIX "/redfish/v1/sessionservice/sessions"



#define DATA_ACQ_REPORT_PREFIX "/redfish/v1/dataacquisitionservice/dataacquisitionreport"
#define COMPONENT_TYPE "componenttype"
#define METRIC_TYPE "metrictype"
#define START_TIME "starttime"
#define END_TIME "endtime"
#define SER_NUM "serialnumber"


#define USERNAME "UserName"
#define PASSWORD "Password"
#define ROLEID "RoleId"
#undef DOMAIN
#define DOMAIN "Domain"
#define RFPROP_COPYRIGHT "copyright"
#define COLLECTION "Collection"

#define ALLOW_GET 0x01
#define ALLOW_PATCH 0x02
#define ALLOW_POST 0x04
#define ALLOW_DELETE 0x08
#define ALLOW_ALL 0x0F




#define MAX_IPADDR_LEN 128
#define MAX_METHOD_NAME_LEN 16
#define MAX_ACTION_NAME_LEN 256
#define MAX_PARAM_LEN 64 // query parameter name length
#define MAX_STATUS_TITLE_LEN 64


#define MAX_KEYS_NUM 5


#define TOKEN_KEY 23
#define TOKEN_LEN 40
#define MAX_ROLEID_LENGTH 31

#define RF_MAX_USER_NAME_LEN 32
#define MAX_PW_LEN 20
#define MAX_NAME_LEN 32
#define MAX_PATH_LEN 256
#define MAX_SESSION_ID_LEN 32
#define HANDLE_LOCAL (-1)
#define HEAD_INDEX_OFFSET 0x1234

#define URL_CODE_CHAR_LEN 3
#define PWD_URL_CODE_CHAR_NUM 33


#define sms_base_error(info, code, message, ...) do {                                             \
        debug_log(DLOG_DEBUG, "[%s]: [base.CODE] %d, [MESSAGE] %s", __FUNCTION__, code, message); \
        sms_base_message(info, code, message, __VA_ARGS__);                                       \
    } while (0)

#define sms_ibmc_error(info, code, message, ...) do {                                             \
        debug_log(DLOG_DEBUG, "[%s]: [ibmc.CODE] %d, [MESSAGE] %s", __FUNCTION__, code, message); \
        sms_ibmc_message(info, code, message, __VA_ARGS__);                                       \
    } while (0)




typedef struct tag_sms_info_s {
    guint8 id;
    OBJ_HANDLE sms_obj_handle;
    gchar host[MAX_IPADDR_LEN];
    json_object *jso_white_list;
    struct tag_sms_info_s *next;

    gchar method[STRING_LEN_MAX];
    gchar session_id[SESSION_ID_LEN + 1];
    gchar relative_uri[MAX_URI_LENGTH];
    gchar redirect_uri[MAX_URI_LENGTH];

    json_object *jso_request;
    json_object *jso_response;

    gint32 resp_code;
} SMS_INFO_S;


typedef enum tag_query_param {
    SKIP_QUERY,
    TOP_QUERY
} QUERY_PARAM;

typedef enum tag_expand_query_param {
    EXPAND_INIT, 
    ASTERISK,    
    PERIOD,      
    TILDE,       
    MAX_TYPE
} EXPAND_QUERY_PARAM;


typedef enum tag_skip_top_param_status {
    PARA_INIT = 0x0000, 
    SKIP_EXIT = 1 << 0, 
    TOP_EXIT = 1 << 1,  
    MAX_VALUE           
} SKIP_TOP_PARA_STATUS;



typedef struct tag_param_info {
    gchar key[MAX_PARAM_LEN];
    gchar value[MAX_PARAM_LEN];
} PARAM_INFO_S;

typedef struct tag_http_header {
    gchar header_title[MAX_HEADER_TITLE_LEN];
    gchar header_content[MAX_HEADER_CONTENT_LEN];
} HTTP_HEADER_S;

typedef struct tag_http_header_index {
    HTTP_HEADER_S *from;
    HTTP_HEADER_S *origin;
    HTTP_HEADER_S *token;
    HTTP_HEADER_S *accept;
    HTTP_HEADER_S *cookie;
    HTTP_HEADER_S *if_match;
    HTTP_HEADER_S *if_none_match;
    HTTP_HEADER_S *basic_auth;
    HTTP_HEADER_S *reauth_key;
    HTTP_HEADER_S *content_type;
    HTTP_HEADER_S *x_auth_token;
    HTTP_HEADER_S *odata_version;
} HTTP_HEADER_INDEX_S;

typedef struct tag_http_response_status {
    gint32 status_code;
    gchar title[MAX_STATUS_TITLE_LEN];
} RESP_STATUS_S;

// http请求字符串解析后内容
typedef struct tag_request_info {
    gchar relative_uri[MAX_URI_LENGTH];
    gchar uri_lower[MAX_URI_LENGTH];
    gchar resource_path[MAX_PATH_LENGTH];
    gchar member_id[MAX_MEM_ID_LEN];
    gchar request_method[MAX_METHOD_NAME_LEN];
    METHOD_TYPE_E method_type;
    gchar request_ip[MAX_IPADDR_LEN];
    gchar request_body[MAX_REQUEST_BODY_LEN];
    HTTP_HEADER_S headers[MAX_HEADER_NUM];
    HTTP_HEADER_INDEX_S headers_index;
    PARAM_INFO_S query_params[MAX_KEYS_NUM];
    gint32 query_param_num;
    gchar action_name[MAX_ACTION_NAME_LEN];
    gint32 allow_opr;
    GHashTable *uri_params;            // 解析后的uri参数哈希表
    RESOURCE_PROVIDER_S *res_provider; // 资源的回调处理信息
} REQUEST_INFO_S;

// http响应字符串解析后内容
typedef struct tag_resp_info {
    HTTP_HEADER_S headers[MAX_HEADER_NUM];
    guint8 resp_head_num;
    gchar resp_body[MAX_RESPONSE_STRING_LEN];
    RESP_STATUS_S status;
} RESPONSE_INFO_S;



typedef gint32 (*http_handler)(REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *i_param, HTTP_RESPONSE_DATA_S *response);

typedef struct tag_http_request_handler {
    METHOD_TYPE_E method_type;
    http_handler handler;
} REQUEST_HANDLER_S;

typedef struct tag_authenticated_free_uri {
    METHOD_TYPE_E method_type;
    const gchar *uri;
} AUTH_FREE_S;

typedef struct tag_reauthenticated_uri {
    METHOD_TYPE_E method_type;
    const gchar *uri;
    const gchar *property;
    const gchar *operation_name;
} RE_AUTH_S;


extern gint32 method_process_http_request(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern void redfish_http_init(void);
extern gint32 generate_common_response_headers(json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso);
gint32 get_res_info_from_uri(REQUEST_INFO_S *i_request_info, PROVIDER_PARAS_S *provider_param,
    json_object *message_array_jso);
extern gint32 generate_get_response_body(const REQUEST_INFO_S *i_request_info, PROVIDER_PARAS_S *i_param,
    json_object **o_resource_jso, json_object *o_message_array_jso);
gint32 generate_etag_response_header(const gchar *uri_lower, json_object *i_redfish_jso,
    RESOURCE_PROVIDER_S *resource_provider, json_object **o_header_jso);
extern gint32 generate_response_status(gint32 response_code, json_object **status_jso, json_object *message_array_jso);

extern gint32 verify_xauthtoken_get_provider_param(const REQUEST_INFO_S *request_info,
    PROVIDER_PARAS_S *provider_param);
extern gint32 verify_basic_auth_get_provider_param(const REQUEST_INFO_S *request_info, PROVIDER_PARAS_S *provider_param,
    json_object **message_info_jso);

extern void add_monitor_location_header(json_object *rsc_jso, json_object *header_array_jso);
extern gint32 method_redirect_http_request(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 set_default_white_list(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);

gint32 sms_base_message(SMS_INFO_S *info, gint32 resp_code, const gchar *msg_id, const gchar *related_property, ...);
gint32 sms_ibmc_message(SMS_INFO_S *info, gint32 resp_code, const gchar *msg_id, const gchar *related_property, ...);
gint32 method_redirect_http_request(const OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 ConfigureSmsHost(OBJ_HANDLE obj_handle, gpointer user_data, gchar *property_name, GVariant *property_value);
void free_request_info(REQUEST_INFO_S *request_info);

gint32 check_uri_shield_condition(REQUEST_INFO_S *request_info, HTTP_RESPONSE_DATA_S *response);
extern void redfish_system_lockdown_init(void);
extern gint32 redfish_system_lockdown_check(const guint8 allow);
extern gint32 redfish_system_lockdown_state_changed(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);

void generate_redfish_resp_data_from_jso(json_object *response_jso, HTTP_RESPONSE_DATA_S *response);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
