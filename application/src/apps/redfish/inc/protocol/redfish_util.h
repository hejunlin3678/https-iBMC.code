

#ifndef __REDFISH_UTIL_H__
#define __REDFISH_UTIL_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"
#include "redfish_defs.h"

#include "glib.h"
#include "json.h"

#ifdef _cplusplus
extern "C" {
#endif

#define PRODUCT_CONFIG_PATH "/data/opt/pme" 

#define REDFISH_PATH_DEFINE(redfish_relative_path) (PRODUCT_CONFIG_PATH redfish_relative_path)

#define RSC_CFG_FILE_PATH "/redfish/v1/redfish_template/rsc_cfg.json"

#define CONFG_FILE_SCHEMASTORE_EN "/data/opt/pme/conf/redfish/v1/schemastore/en"


typedef struct tag_str_map {
    const char *str1;
    const char *str2;
} str_map_s;

typedef struct tag_int2str_map {
    gint32 int_val;
    const gchar *str_val;
} INT2STR_MAP_S;

#define MAX_DATETIME_LEN 128

#define MAX_SEGMENT_CHARACTER_NUM 128

#define MAX_NUM_BUF_LEN 16

#define USER_AUTH_TYPE_LOCAL 0

#define APPEND_BUFFER_LEN 16

typedef enum {
    RF_OK,                              
    RF_FAILED,                          
    RF_PARAM_ERROR,                     
    RF_PROP_UNKNOWN,                    
    RF_PROP_TYPE_ERR,                   
    RF_PROP_FORMAT_ERR,                 
    RF_PROP_NOT_IN_LIST,                
    RF_PROP_MISSING,                    
    RF_REQUEST_NOT_ALLOWED,             
    RF_PROP_READONLY,                   
    RF_CREATE_OK,                       
    RF_CREATE_FAILED,                   
    RF_SESSION_LIMIT_EXCEEDED,          
    RF_ACCOUNT_PASSWORD_EXPIRED,        
    RF_AUTH_FAILED,                     
    RF_RPC_CONN_FAILED,                 
    RF_USER_LOCKED,                     
    RF_USER_IS_NOT_MANAGER,             
    RF_RSC_LIMIT_EXCEEDED,              
    RF_RSC_ALREADY_EXIST,               
    RF_RSC_IN_USE,                      
    RF_RSC_NOT_FOUND,                   
    RF_DELETE_OK,                       
    RF_DELETE_FAILED,                   
    RF_EVT_SUB_LIMIT_EXCEEDED,          
    RF_UNSUPPORTED_JSON_FORMAT_DATA,    
    RF_MALFORMED_JSON,                  
    RF_ETAG_MATCH_FAILED,               
    RF_QUERY_NOT_SUPPORTED,             
    RF_QUERY_TYPE_ERROR,                
    RF_QUERY_OUTOF_RANGE,               
    RF_QUERY_FORMAT_ERROR,              
    RF_ACTION_NOT_SUPPORTED,            
    RF_NO_VALIED_SESSION,               
    RF_PATCH_FAILED,                    
    RF_TASK_LIMIT_EXCEED,               
    RF_ACTION_FAILED,                   
    RF_CREATE_NOT_ALLOWED,              
    RF_NOT_IMPLEMENTED,                 
    RF_FILE_TRANSFER_PROTOCAL_MISMATCH, 
    RF_PROP_NAME_INVALID_FORMAT,        
    RF_ARRAY_ERROR,                     
    RF_INSUFFICIENT_PRIV,               
    RF_ARRAY_REQUIRED_ITEM, 
    RF_IGNORE_CUR_OPERATION_RESULT, 
} rf_retvalue;

typedef enum CREATE_MSG_RETVALUE {
    RF_CREATE_MSG = 0,
    RF_PARAM_ERR,
    RF_GET_JSO_FROM_BASE_ERR, 
    RF_GET_ODATA_ERR,
    RF_CREATE_MSG_ID_ERR,
    RF_CREATE_REL_PROP_ERR,
    RF_CREATE_MSG_ERR,
    RF_CREATE_ARG_ARRAY_ERR,
    RF_GET_SEV_ERR,
    RF_GET_RESOLUTION_ERR,
    RF_CONVERGE_MSG_ERR
} CREATE_MSG_RETVALUE_ENUM;



typedef enum {
    READ_HISTORY_UINT16 = 0, 
    READ_HISTORY_DOUBLE      
} READ_HISTORY_TYPE;


typedef struct {
    gchar client[SESSION_IP_LEN];
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    gchar uri[MAX_URI_LENGTH]; 
    guchar is_from_webui;
    guint8 user_role_privilege;
    json_object* param_jso;
} EXPORT_ACTION_ARGS;

typedef gint32 (*get_status_handler)(json_object **message_obj, guint8 *progress);

typedef gint32 (*generate_handler)(EXPORT_ACTION_ARGS *action_args, json_object **o_message_jso);

typedef struct tag_export_transaction_info {
    guint8 type;       // 导出类型, 0代表导出到本地，1代表导出到远端
    guint8 state;      // 导出阶段, 0代表生成文件阶段，1代表传输文件阶段
    guint8 permission; // 导出权限
    guint8 need_del;   // 源文件是否需要删除
    gint32 file_id;
    gchar file_category[NAME_LEN + 1];              // 导出文件的种类
    gchar content[MAX_URI_LENGTH];                  // 目的文件路径或远端URI
    gchar src[MAX_FILEPATH_LENGTH];                 // 源文件路径
    gchar client[SESSION_IP_LEN];                   // 客户端
    gchar session_id[SESSION_ID_LEN + 1];           // 当前会话
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1]; // 用户名
    generate_handler generate_file;                 // 生成文件并处理出错的接口
    get_status_handler get_status;                  // 生成文件进度查询接口
    guchar from_webui_flag;
    guchar auth_type;
} EXPORT_TRANSACTION_INFO_S;

extern const char *get_mapped_str(const char *str, str_map_s *str_map, int size);


extern int digital_str_bits(const char *token_begin);

extern gint32 fill_odata_head_info(const char *odata_id_str, json_object *rsc_jso);



extern int get_str_array_index_str(const char **str_array, int array_len, const char *str, char *index_str,
    int index_buf_len);


json_object *get_rsc_allowed_operation(const char *rsc_path);


int num_format_string_translate(const char *num_format_str, int arg_num, gchar **format_str_out);


gboolean property_format_string(json_object *jso, const char *prop_key, ...);


gint32 json_custom_get_elem_str(json_object *i_jso, const gchar *i_element_name, gchar *o_str, guint32 i_str_len);

extern const gchar *g_str_case_rstr(const gchar *haystack, const gchar *needle);

extern gint32 get_rsc_class(const char *original_path, char *rsc_class_name, int class_name_len);


extern int split_path(char *path, guint32 path_size, char *segments[]);

extern gint32 get_str_val_from_map_info(INT2STR_MAP_S *map_info, guint32 size, gint int_val, const gchar **str_val);

extern gint32 str_delete_char(gchar *str, gchar c);

extern gint32 string_type_json_arr_to_variant(json_object *arr_jso, GVariant **var);

extern gint32 fixed_size_type_json_arr_to_variant(json_object *arr_jso, GVariant **var);

extern gint32 rf_set_json_array_to_obj_handle(OBJ_HANDLE obj_handle, const gchar *prop, json_object *arr_jso,
    DF_OPTIONS options);

extern gint32 rf_obj_handle_data_to_rsc_data(OBJ_HANDLE obj_handle, const gchar *obj_prop_name,
    const gchar *rsc_prop_name, json_object *rsc_jso);

extern gint32 rf_obj_handle_data_trans_to_rsc_data(OBJ_HANDLE obj_handle, const gchar *obj_prop_name,
    const gchar *rsc_prop_name, json_object *rsc_jso);

extern gint32 string_v_variant_to_json_data(GVariant *var, json_object **arr_jso);

extern gint32 rf_get_json_data_from_variant(GVariant *var, json_object **jso);

extern gint32 array_variant_to_json_data(GVariant *var, json_object **arr_jso);

extern gint32 rf_get_json_array_from_variant(GVariant *var, json_object **arr_jso);

extern void redfish_trim_uri(gchar *uri, guint32 uri_size);

extern void rf_trim_https_uri(gchar *uri, guint32 uri_size);

extern void rf_gen_json_object_etag(json_object *jso, gchar *etag, guint32 etag_len);

extern gint32 rf_get_sp_json_file(guint8 file_id, const gchar *file_name, json_object **obj);

extern gint32 rf_set_sp_json_file(guint8 file_id, const gchar *file_name, json_object *obj);

extern gint32 read_history_data_from_file(gchar *file_path, gint32 num_of_labels, gchar **labels,
    json_object **o_result_jso, READ_HISTORY_TYPE history_type);

extern gint32 get_universal_provider_odata_info(void *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    const gchar *uri_template);

extern gint32 get_file_lines(const gchar *log_path, gint32 *count);

extern gint32 find_fullfilename_from_directory(const gchar *directory_path, const gchar *prefix_file_name,
    gchar *full_file_name, gsize length);

extern gint32 create_export_transaction(void *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    EXPORT_TRANSACTION_INFO_S *export_info);

extern void record_method_operation_log(guchar from_webui_flag, gboolean export_success, gchar *user_name,
    gchar *client, gchar *file_category);
void record_update_schema_method_operation_log(guchar from_webui_flag, const gchar *user_name, const gchar *client,
    gboolean update_success);

extern gint32 redfish_sp_schema_get_bmc_file(const gchar *file_name, gchar *bmc_file_name, gint32 file_name_length);

extern gint32 shelf_managers_systems_sort_by_odataid(const void *j1, const void *j2);

void get_dir_from_path(const gchar *path, gchar *dir, guint32 dir_length);
gboolean is_valid_file_transfer_protocol(const gchar *uri);

gint32 set_rsp_body_content(const gchar *content, gchar **o_rsp_body_str);
void add_val_str_to_rsp_body(const gchar *key, const gchar *val, gboolean is_last_prop, gchar *buffer,
    guint32 buffer_len);

json_object *get_resource_cfg_json(const gchar* relative_file_name);
gint32 rf_get_property_value_string_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso);
gint32 rf_get_property_value_int32_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso);
gint32 rf_get_property_value_int16_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso);
gint32 rf_get_property_value_uint32_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso);
gint32 rf_get_property_value_uint16_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso);
gint32 rf_get_property_value_uint8_as_jso(OBJ_HANDLE obj_handle, const gchar *prop, json_object **jso);

#ifdef _cplusplus
}
#endif 

#endif 
