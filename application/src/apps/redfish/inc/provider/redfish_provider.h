

#ifndef __REDFISH_PROVIDER_H__
#define __REDFISH_PROVIDER_H__

#include <json.h>
#include <sys/file.h>

#include "pme_app/pme_app.h"
#include "pme_app/uip/uip.h"
#include "pme_app/uip/uip_network_config.h"
#include "redfish_message.h"

#include "pme_app/smlib/sml_errcodes.h"
#include "pme_app/smlib/sml_base.h"
#include "pme_app/smlib/sml.h"
#include "open_source/raid/lsi/mfistat.h"

#include "redfish_provider_resource.h"
#include "json_patch_util.h"
#include "property_method_app.h"
#include "redfish_util.h"
#include "open_source/raid/lsi/mfistat.h"
#include "pme_app/common/public_redfish.h"
#ifdef ARM64_HI1711_ENABLED
#include <errno.h> //解决1711 arm64无法找errno变量
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 




// 资源公共属性
#define MAX_CLASS_NAME 32
#define MAX_PROP_NAME 32
#define MAX_JSON_PROP_NAME 128

// 对象空句柄
#define OBJ_HANDLE_NULL 0


#define MAX_PROP_DEPTH 5

#define MAX_URI_LEN 256

#define IMPORT_CONFILE_ITEM_REGEX_LEN 512
#define TAG_TEMP_FILE_REQUIRED_ITEM 1
#define TAG_TEMP_FILE_NOT_REQUIRED_ITEM 0

#define AVAILABLE 1
#define NOT_AVAILABLE 0

#define BUSSINESS_PORT_PFID "PfId"
#define BUSSINESS_PORT "Port"
#define BUSSINESS_PORT_PERMANENT_MAC "PermanentMac"
#define BUSSINESS_PORT_PFMACINFO "PfMacInfo"
#define BUSSINESS_PORT_PFINFO_MAX_CNT 32

// 1711 SMM板内固件为6个：ActiveUboot、BackupUboot、ActiveBMC、BackupBMC、AvailableBMC、CPLD
#ifdef ARM64_HI1711_ENABLED
#define SMM_BOARDFW_CNT                          6
#else
#define SMM_BOARDFW_CNT                          5
#endif
#define ACCURACY_FORMAT_REGEX "%.[0-9]f"
#define ACCURACY_FORMAT_3F "%.3f"
#define AUTH_TYPE_PERMIT_DINED         3

#define RF_MIN_FLOAT_NUMBER 0.0000000000000001

// 设置mac地址方法的下发通道类型
#define SET_ACTUAL_MAC_ADDR_BY_MCTP 0

// 设置RFID标签信息及资产条SN信息的操作类型
#define MANDATORY_PROPERTY 1
#define OPTIONAL_PROPERTY 2

#define BOARD_ENCLO_SLOT_FORMAT      "Enclosure"
#define CHASSIS_SHARED_RESOURCES_ENLOSURE      "enclosure"
#define PCIE_ALARM_LIGHT_ON            "Lit"
#define PCIE_ALARM_LIGHT_OFF           "Off"
#define LOCAL_CONTROLOR                0

#define COMPONENT_DEVICENAME_MSG0 0
#define COMPONENT_DEVICENAME_MSG1 1
#define COMPONENT_DEVICENAME_MSG3 3
#define STR_LEN_2                 2
#define BIOS_COMP_ID_EX_ATLAS 0x81
#define BIOS_COMP_ID_EX_1601 0x82


#define NEED_TWO_PARAMETER 2
#define NEED_ONE_PARAMETER 1
#define NOT_NEED_PARAMETER 0





typedef struct web_user_last_login_info_S {
    guint32 user_last_login_time;
    gchar user_last_login_ip[SESSION_IP_LEN + 1];
    guint8 request_is_create;
} WEB_USER_LAST_LOGIN_INFO_S;


// provider input paras sturct: get/set/action均使用该结构体传入参数(视需要再扩展)
typedef struct tag_provider_paras {
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1]; // 用户名
    gchar client[SESSION_IP_LEN];                   // 客户端
    gchar uri[MAX_URI_LENGTH];                      // URI路径
    gchar session_id[SESSION_ID_LEN + 1];           // 当前会话
    gchar member_id[MAX_MEM_ID_LEN];                // 访问集合member时使用，通常为uri末端部分
    gint32 index;                                   // 索引号(provider数组中索引号）
    gint32 skip;
    gint32 top;
    guint8 is_satisfy_privi; // privileged_flag 用户角色权限满足标志，由上层鉴权后传入
    guint8 is_from_webui;
    
    guint8 user_role_privilege; // 当前用户角色权限,用于patch操作时的class method鉴权.
    
    gchar user_roleid[USER_ROLEID_MAX_LEN + 1];
    json_object *val_jso;
    
    json_object *user_data;
    
    json_object *custom_header;
    
    json_object *delete_data;
    

    
    guchar auth_type;    
    

    
    OBJ_HANDLE obj_handle;

    
    WEB_USER_LAST_LOGIN_INFO_S web_user_last_login_info;
    

    
    GSList *parent_obj_handle_list;
    GHashTable *uri_params; // uri参数哈希表
} PROVIDER_PARAS_S;

typedef gint32 (*property_handler_t)(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

typedef struct tag_property_provider {
    
    const char property_name[MAX_JSON_PROP_NAME];

    
    gchar pme_class_name[MAX_CLASS_NAME];

    
    gchar pme_prop_name[MAX_PROP_NAME];

    
    guint16 require_privilege;

    
    guint8 syslock_allow;

    
    property_handler_t pfn_get;

    
    property_handler_t pfn_set;

    
    property_handler_t pfn_action;

    
    guint8 etag_flag;
} PROPERTY_PROVIDER_S;

typedef struct import_confile_item_format {
    const char *utag_item;
    const char *column;
    guint8 check_null_flag;
    const char regex[IMPORT_CONFILE_ITEM_REGEX_LEN];
} CONFILE_ITEM_FORMAT_S;



typedef struct {
    PROVIDER_PARAS_S *i_paras;
    json_object *o_message_jso;
    OBJ_HANDLE handle;
    json_object *arr_elem_jso; 
    gint32 arr_index;          
    gint32 set_succ_num;       
} SET_POWER_LIMIT_INFO_S, SET_POWER_REDUNDANCY_INFO_S;


typedef struct {
    
    gint32 error_code;
    
    gchar error_type[MAX_PROP_NAME];
    
    guint32 parameter_cnt;
} SET_POWER_REDUNDANCY_RETURN_MESS;

typedef struct {
    const char *property_name;
    json_type property_type;
    gint32 operate_type;
    gint64 lower_limit;
    gint64 upper_limit;
    const char *transfer_key;
} SET_UNIT_RFID_INFO_S;


typedef gint32 (*rsc_get_func)(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

typedef gint32 (*create_func)(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso);
typedef gint32 (*delete_func)(PROVIDER_PARAS_S *i_param, json_object *message_array_jso);
typedef gint32 (*generate_create_headers)(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso);
typedef gint32 (*generate_delete_headers)(PROVIDER_PARAS_S *i_param, json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso);

typedef gint32 (*etag_del_prop_func)(json_object *object);

typedef gint32 (*get_provider_info_function)(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **provider, guint32 *count);

typedef gint32 (*get_component_uri_func)(OBJ_HANDLE component_obj, const gchar *uri_format, const void *user_data,
    gchar *component_uri, guint32 uri_len);

typedef void (*get_eth_info_func)(OBJ_HANDLE, guchar, json_object*);
typedef enum {
    PROVIDER_URI_PATTERN_DEFAULT_MODE = 0, // 默认值：无需关注是否为通配的情况，即不存在资源URI被多条Provider正则匹配到的情况
    PROVIDER_URI_SPECICAL_PATTERN_MODE, // URI为特定模式匹配:相对于通用配置，只满足特定的情况
    PROVIDER_URI_WILDCARD_PATTERN_MODE // URI为通配模式:特定情况也符合此处通用的匹配
} PROVIDER_URI_PATTERN_FLAG;

typedef struct tag_resource_provider_s {
    // 资源URI的通配模式,模式中必须包含^$开始结束符号,均采用小写字母
    const char uri_pattern[MAX_URI_LENGTH];

    
    PROVIDER_URI_PATTERN_FLAG provider_uri_pattern_flag;

    // 资源配置文件对应文件夹路径(不需index.json之类的后缀),均采用小写字母
    const char resource_path[MAX_URI_LENGTH];
    // 获取类的provider表及元素个数，存在设置的属性时，此成员必须存在
    get_provider_info_function resource_provider_fn;

    
    const rsc_get_func do_get;

    
    etag_del_prop_func etag_del_prop;
} RESOURCE_PROVIDER_S;

typedef struct tag_rsc_opr_info {
    const gchar *uri;
    const guchar syslock_allow;
    const create_func do_create;
    const delete_func do_delete;
    const generate_create_headers do_generate_create_headers;
    const generate_delete_headers do_generate_delete_headers;
} REDFISH_OPR_INFO_S;

typedef struct _tag_component_to_uri_info {
    guint8 component_device_type;
    const gchar *uri_format;
    get_component_uri_func uri_get_func;
} COMPONENT_TO_URI_INFO_S;


typedef struct special_prop {
    gint32 num;
    const gchar *prop[MAX_PROP_DEPTH];
} SPECIAL_PROP_S;





typedef enum {
    CREATE_LD_ON_NEW_ARRAY = 0,
    ADD_LD_ON_EXIST_ARRAY,
    CREATE_LD_AS_CACHECADE
} CREATE_LD_TYPE_E;


typedef struct tag_depend_info_s {
    gchar depend_name[PROPERTY_NAME_LENGTH];
    gchar depend_prop[PROPERTY_NAME_LENGTH];
    json_object *depend_value;
    gchar prop_relation;
} DEPEND_INFO;



typedef struct {
    OBJ_HANDLE object_handle;
    CREATE_LD_TYPE_E create_type;
    guint8 ctrl_id;
    guint8 type_id;
    guint8 raid_level; 
    gchar str_raid_level[MAX_RAID_LEVEL_LEN];
    guint8 disk_id_array[MAX_PD_ID_NUM];
    guint32 disk_num; 

    guint8 span_num;                    
    gchar volume_name[SML_LD_NAME_LENGTH]; 
    guint32 volume_capacity;            
    guint32 strip_size;                 
    guint8 strip_size_as_para;          

    guint8 read_policy;
    guint8 write_policy;
    guint8 io_policy;
    guint8 access_policy;
    guint8 disk_cache_policy;
    guint8 init_mode;
    guint16 array_id; 
    guint8 block_index;
    guint8 accelerator;
    guint8 cache_line_size;
    guint16 associated_ld;
} CREATE_INFO_S;

typedef struct {
    gchar event_code[PROPERTY_NAME_LENGTH];
    gchar event_code_high[PROPERTY_NAME_LENGTH];
    gchar event_code_low[PROPERTY_NAME_LENGTH];
    gint32 subobject_index;
    gchar action_type[PROPERTY_NAME_LENGTH];
} PRECISE_ALARM_S;


typedef struct tag_shield_alarm_s {
    gchar event_code[PROPERTY_NAME_LENGTH];
    gchar event_code_high[PROPERTY_NAME_LENGTH];
    gchar event_code_low[PROPERTY_NAME_LENGTH];
    guint8 shield_state;
} SHIELD_ALARM_S;


typedef struct {
    OBJ_HANDLE object_handle;
    guint16 volume_id;
} DEL_INFO_S;


typedef enum {
    STAT_OK = 0,
    STAT_IN_PROGRESS,
    STAT_FAILED,
    STAT_TASK_EXPCEPTION
} CREAT_OR_DEL_VOL_STAT_E;


typedef struct {
    CREAT_OR_DEL_VOL_STAT_E status;
    gint32 err_code;
    gchar res_id[MAX_VOL_STR_LEN]; 
} CREAT_OR_DEL_VOL_RES_S;


typedef struct {
    union {
        CREATE_INFO_S create_info;
        DEL_INFO_S del_info;
    } creat_or_del_info;
    CREAT_OR_DEL_VOL_RES_S result;
    PROVIDER_PARAS_S provider_paras;
} CREAT_OR_DEL_VOL_PARAS_S;


typedef struct sel_data {
    gchar severity[SELINFO_SEVERITY_LEN];          // 级别
    gchar subject_type[SELINFO_SUBJECJ_RTPE_LEN];  // 事件主体类型
    gchar event_desc[SELINFO_DESC_LEN];            // 事件描述字符串
    gchar event_code[RFPROP_SEL_EVENTCODELEN];     // 事件码
    gchar old_event_code[RFPROP_SEL_EVENTCODELEN]; // 旧事件码
    gchar creat_time[SELINFO_CREATE_TIME];         // 创建时间
    gchar trigmode[SELINFO_TRIGMODE_LEN];          // 触发方式
    gchar eventsubject[SELINFO_DESC_LEN];          // 事件主体类型
    
    gchar sn[SELINFO_DESC_LEN]; // 事件主体序列号
    gchar pn[SELINFO_DESC_LEN]; // 事件主体部件号
    
} SAVE_SEL_DATA;

typedef struct {
    guint8 user_role_privilege;
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    gchar client[SESSION_IP_LEN];
    OBJ_HANDLE obj_handle;
    guchar from_webui_flag;
} DISCONNECT_NEED_INFO;


typedef struct {
    gint32 board_type;
    gchar board_name[MAX_PROP_NAME];
    gchar prop_max_num[MAX_PROP_NAME];
} BOARD_PROPERTY;



typedef void (*pcie_extend_info_handler_t)(OBJ_HANDLE obj_handle, json_object *huawei_jso);

typedef struct pcie_extend_info_provider {
    const gchar class_name[MAX_CLASS_NAME + 1];
    pcie_extend_info_handler_t pfn_get;
} PCIE_EXTEND_INFO_PROVIDER;

typedef struct {
    const gchar *prop_name;
    gint32 (*set_prop)(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso);
} PCIE_HUAWEI_HANDLE_METHOD;

typedef struct get_sdicard_boot_option {
    guint8 boot_order;
    gchar *boot_option;
} GET_SDICARD_BOOT_OPTION;

typedef struct get_sdicard_boot_order_valid_type {
    guint8 valid_type;
    gchar *valid_type_str;
} GET_SDICARD_BOOT_VALID_TYPE;

typedef struct _tag_rdf_node_info {
    
    const char *data;

    
    GNode *found_node;
} rdf_node_info_s;


typedef struct {
    guint8 user_role_privilege;
    gchar usburi[PSLOT_URI_LEN];
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    gchar client[SESSION_IP_LEN];
    OBJ_HANDLE obj_handle;
    gint32 check_usb_info;
    gint32 usb_timeout;
    gint32 usb_type;
    guchar from_webui_flag;
} USBSTICK_CONNECT_NEED_INFO;

typedef struct update_sp_schema_need_info {
    guint8 user_role_privilege;
    guint8 is_from_webui;
    gchar uri[PSLOT_URI_LEN];
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    gchar client[SESSION_IP_LEN];
    OBJ_HANDLE obj_handle;
    gint32 update_timeout;
    gint32 update_process;
    gchar checksum_val[PROP_VAL_LENGTH + 1];
    gchar file_name[MAX_FILEPATH_LENGTH + 1];
} UPDATE_SP_SCHEMA_NEED_INFO;


typedef struct {
    gchar user_name[SESSION_USER_NAME_MAX_LEN + 1];
    guint8 user_role_privilege;
    gchar client[SESSION_IP_LEN];
    OBJ_HANDLE obj_handle;
    gint32 filter_process;
    guint8 filter_status;
    guint8 is_from_webui;
    gchar file_name[MAX_FILEPATH_LENGTH];
    json_object *body_jso;
} DATA_FILTERINF_NEED_INFO;



typedef void (*import_cert_set_userdata_fn)(PROVIDER_PARAS_S *, json_object *);


typedef gint32 (*process_local_cert_import_fn)(const gchar *file_path, json_object *user_data,
    json_object **o_message_jso);


typedef gint32 (*process_remote_cert_import_fn)(PROVIDER_PARAS_S *i_paras, const gchar *remote_uri,
    json_object **o_message_jso, json_object **o_result_jso);


typedef struct _import_cert_params {
    
    IMPORT_CERT_FILE_TYPE file_type;
    
    import_cert_set_userdata_fn set_userdata_fn;
    
    process_local_cert_import_fn local_cert_import_fn;
    
    process_remote_cert_import_fn remote_cert_import_fn;
    
    gchar *tmp_file_path;
} IMPORT_CERT_PARAMS_S;

typedef enum tag_alarm_led_state {
    ALARM_LIGHT_OFF = 0x00, 
    ALARM_LIGHT_ON,         
    ALARM_LIGHT_BUTT = 0xff
} ALARM_LED_STATE_E;

#define RF_ADD_ODATA_ID_MEMBER(a, s) do {                           \
    json_object *o = json_object_new_object();                      \
    json_object_object_add(o, ODATA_ID, json_object_new_string(s)); \
    (void)json_object_array_add(a, o);                              \
} while (0)

property_handler_t find_property_provider(const PROPERTY_PROVIDER_S *provider, guint32 provider_count,
    const char *property_name, guint32 *provider_index);

void get_event_level(guint8 level, json_object **json_object_level);
gboolean is_kerberos_support(void);
void rf_provider_paras_free(PROVIDER_PARAS_S *provider_paras);



gint32 redfish_get_format_time(PROVIDER_PARAS_S *i_paras, guint32 *p_datetime, gint16 *p_timezone);
gint32 redfish_fill_format_time(gchar *str_buf, guint32 str_len, guint32 *p_datetime, gint16 *p_timezone);
gint32 redfish_get_property_value_uint16(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
gint32 redfish_get_property_value_string(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
gint32 redfish_set_property_value_uint16(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso, const gchar *method_name, guint8 user_role_privilege);



gint32 get_drive_oem_physicaldiskstate(OBJ_HANDLE obj_handle, json_object *o_huawei_jso);
void get_oem_raid_level(OBJ_HANDLE obj_handle, json_object *result);
void redfish_storage_get_related_array_info(OBJ_HANDLE dri_obj_handle, json_object *result_jso,
    json_object *logical_jso);
gint32 get_storage_logical_rsc(OBJ_HANDLE obj_handle, json_object *result_data);
void get_drive_id(OBJ_HANDLE dri_obj_handle, gchar *drives_id_str, gsize str_len);
gint32 redfish_system_storage_drive_string(const gchar *redfish_provider_name, OBJ_HANDLE i_obj_handle,
    json_object **o_result_jso);
gint32 get_drive_mediatype(OBJ_HANDLE dri_obj_handle, gchar *str_buf, gsize buf_len);
gint32 get_drive_oem_driveid(OBJ_HANDLE obj_handle, json_object *o_huawei_jso);

void get_storage_is_support_volume(OBJ_HANDLE obj_handle, json_object *huawei);
void get_oem_storage_controller_mode(OBJ_HANDLE obj_handle, json_object *huawei);
gint32 redfish_alarm_severities_change(guchar *data, gchar *str_alarm, gint32 length);

gint32 redfish_judge_mntportcab_valid(guint8 net_type);

gint32 SetSessionserviceTimeout(const OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 provider_paras_check(PROVIDER_PARAS_S *i_paras);
gint32 accountservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 sessionservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 session_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 session_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 session_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 manager_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_ethernetinterfaces_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_ethernetinterface_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_lldpservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_usbmgmt_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_energysaving_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_nic_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 check_param_and_privi_validity(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 check_redfish_call_method_result(const gint32 ret, json_object **o_message_jso, const gchar *prop_name,
    const gchar *set_str);
gint32 is_support_usbmgmt_service(void);
void get_nic_fqdn(OBJ_HANDLE eth_group_obj, json_object *o_rsc_jso);
gint32 get_nic_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 redfish_set_ip_version(OBJ_HANDLE obj_handle, json_object *property_jso, json_object **message_array,
    PROVIDER_PARAS_S *i_paras);
gint32 redfish_set_dns_mode(json_object *property_jso, json_object **message_array, PROVIDER_PARAS_S *i_paras);
void redfish_get_dns_address_origin(json_object **huawei);
void redfish_get_ip_version(OBJ_HANDLE obj_handle, json_object **huawei);
gint32 redfish_subnet_mask_check(const gchar *mask);
gint32 manager_nic_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_nic_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 manager_syslog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_syslog_import_root_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_syslog_import_client_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_syslog_import_crl_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_syslog_delete_crl_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_syslog_submit_test_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
void monitor_info_free_func(void *monitor_data);
gint32 parse_file_transfer_err_code(gint32 transfer_status, json_object **message_obj);
gint32 manager_smtp_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_smtp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 networkprotocol_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 copy_tmp_to_des(const gchar *original_file_path, const gchar *dest_file_path, json_object *user_data,
    json_object **o_message_jso);
gint32 system_processor_view_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_process_view_etag_del_property(json_object *object);
gint32 system_memory_view_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 system_restart_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 system_restartos_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 systems_delete_etag_prop(json_object *object);
gint32 system_processors_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 system_processor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

void get_system_processor_gpu_provider(PROPERTY_PROVIDER_S **gpu_provider, guint32 *arr_len);


gint32 system_processors_usage_rate_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 system_ethernetinterfaces_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_eth_etag_del_property(json_object *object);
gint32 system_ethernetinterface_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_ethernetinterface_vlans_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_ethernetinterface_vlan_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 system_storage_lds_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_storage_ld_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 system_storage_ld_init_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_digital_warranty_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 accounts_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 account_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 account_provider_oem_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 account_provider_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 manager_provider_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 chassis_provider_thermal_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 chassis_power_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 chassis_enc_power_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);

gint32 power_history_data_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 power_collect_history_data_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 power_set_psu_supply_source_actioninfo_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 power_reset_data_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 power_reset_statistics_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 redfish_mac_addr2uriid(gchar *mac_addr, guint8 addr_len);
gint32 redfish_get_sys_eth_memberid(OBJ_HANDLE obj_handle, gchar *memberid, gint32 mem_id_len);

gint32 serviceroot_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 roles_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 role_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 redfish_roleid_int2str(guint8 i_role_id, gchar *o_userrole_str, guint32 str_size);
gint32 redfish_role_comparison(const gchar *i_user_roleid, const guchar uri_roleid);

gint32 get_resource_provider(const gchar *uri, RESOURCE_PROVIDER_S **resource_provider);
gint32 sync_up_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 activate_biosinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 storage_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_resource_status_property(guint8 *resource_health, guint8 *resource_HealthRollup, const gchar *resource_state,
    json_object **status_obj, guint8 in_oem);

gboolean is_virtual_eth_port(OBJ_HANDLE busi_obj);
void get_system_eth_link(OBJ_HANDLE obj_handle, json_object *huawei_jso, const gchar *str_template);
gint32 get_network_port_url(OBJ_HANDLE busi_obj, const gchar *slot_id, const gchar *url_template, gchar *netport_url,
    guint32 buf_len);

gint32 get_chassis_pciefunction_oem_associatedresource(OBJ_HANDLE i_obj_handle, json_object **o_json_prop);


void chassis_get_sdcard_odata_id(json_object *o_drives_odata_id);
gboolean redfish_check_volume_uri_effective(const gchar *i_uri, guint8 *o_controller_id, OBJ_HANDLE *o_obj_handle);


gint32 get_object_obj_location_devicename(OBJ_HANDLE obj_handle, gchar *o_loc_dev, gint32 length);
gint32 redfish_get_drives_location_devicename(OBJ_HANDLE obj_handle, gchar *o_loc_dev, gint32 length);


gint32 chassis_board_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gboolean redfish_check_board_uri_effective(gchar *i_memberid, OBJ_HANDLE *o_obj_handle, OBJ_HANDLE chassis_handle);


gint32 get_chassis_board_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);


gint32 chassis_pciedevices_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider,
    guint32* count);
gint32 chassis_pciedevice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_pciefunction_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 redfish_get_pcie_component_obj_handle(OBJ_HANDLE i_obj_handle, OBJ_HANDLE *o_handle);
void redfish_get_chassis_pciedevice_odata_id(json_object *o_result_jso, const char *class_type);
gint32 get_chassis_pciedevices_function_odata_id(OBJ_HANDLE chassis_handle, OBJ_HANDLE i_pcie_handle,
    json_object *o_result_jso);
gint32 redfish_check_pciecard_netcard_get_handle(OBJ_HANDLE i_pcie_handle, OBJ_HANDLE *o_netcard_handle);

gint32 get_opr_fn_info(const gchar *identifier, REDFISH_OPR_INFO_S **opr_fn_info);
gboolean is_opr_fn_uri(const gchar *identifier);
gint32 check_string_val_effective(const char *val);
gint32 redfish_get_ps_verison(OBJ_HANDLE obj_handle, char *ver, gint32 ver_len);
gint32 redfish_get_object_verison(const gchar *class_name, OBJ_HANDLE obj_handle, const gchar *prop_name, gchar *ver,
    gint32 ver_len);
gint32 redfish_get_firmware_version(OBJ_HANDLE obj_handle, gchar *ver, gint32 ver_len);
gint32 thermal_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 thermal_inlet_history_temperature_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 chassis_location_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 chassis_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 redfish_get_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

gint32 redfish_set_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

gint32 redfish_strip_to_int(gchar *str, guint32 str_size);

gint32 system_provider_logservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 system_provider_logservice_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_provider_logservice_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_provider_logservice_collect_sel_action_info_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 system_provider_logservice_query_sel_log_action_info_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 system_provider_logservice_hostlog_hostlog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_provider_logservice_hostlog_action_info_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 transform_capacity_to_intmb(gchar *memory_capacity, guint32 len, guint32 *num);
gint32 rfprocessor_maxspeed_to_intmhz(gchar *speed, guint32 len);
gint32 rfprocessor_get_sensor_reading(OBJ_HANDLE obj_handle, gdouble *pchReading);
gint32 chassis_control_led_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
void get_chassis_oem_powersupply_summary(json_object **oem_tem);
gint32 chassis_set_unit_rfid_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 chassis_set_device_installed_status_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_export_template_file_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 chassis_import_config_file_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 chassis_set_dimensions_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 chassis_boards_arcard_control_actioninfo_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 system_memory_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 system_memorys_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 system_memory_metrics_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gboolean rfmemory_type_is_dcpmm(OBJ_HANDLE obj_handle);


gint32 system_memory_usage_rate_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 registrystore_bios_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 registries_bios_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_bios_registry_file_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);


gint32 system_network_usage_rate_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 system_networkinterfaces_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_networkinterface_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_networkports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_networkadapters_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_networkadapter_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_networkports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_networkport_etag_del_property(json_object *object);
gint32 chassis_networkport_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_opticalmodule_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 chassis_direct_opticalmodule_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 chassis_direct_opticalmodule_list_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);

gint32 chassis_opticalmodule_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 get_chassis_networkadapter_technology_type(const gchar *net_card_obj_name, guint32 *func_type);

gint32 firmwareinventories_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 firmwareinventory_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);




gint32 sms_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 sms_refresh_bma_action_info_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);




gint32 snmp_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 snmp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


void rf_init_rsc_provider_tree_info(void);

gint32 managers_provider_switch_profiles_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 manager_licsrv_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 licsrv_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
void is_disk_support_crypto_erase(OBJ_HANDLE obj_handle, guint8 *crypto_erase_enabled);
void get_chassis_related_drives_member(json_object* o_result_jso);
gint32 chassis_drives_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);

gint32 chassis_drive_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 chassis_drive_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 redfish_check_every_drives_get_handle(const gchar *class_name, const gchar *i_uri, OBJ_HANDLE *o_handle);
gint32 redfish_action_format_check(const gchar *str_action, struct json_object *jso_allowable_values,
    struct json_object *jso_body, json_object **o_message_jso);

gint32 task_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 get_manager_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 set_manager_timezone(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 check_para_effective(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_task_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_task_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_task_svc_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_odata_id_object(gchar *pre_uri, gchar *next_uri, json_object **object);

gint32 redfish_get_board_type(guchar *board_type);
gint32 redfish_get_board_slot(gchar *slot_string, gint32 slot_len);
gboolean redfish_check_chassis_uri_valid(gchar *uri_string, OBJ_HANDLE *component_handle);

gboolean rf_check_chassis_uri_valid_allow_node_enc(gchar *uri_string, OBJ_HANDLE *component_handle);

gboolean redfish_check_manager_uri_valid(gchar *uri_string);
gboolean check_chassis_uri_pangea_enc(const gchar *uri_string);
gboolean redfish_check_chassis_rack_uri_valid(gchar *uri_string, OBJ_HANDLE *obj_handle);

gboolean redfish_check_smm_manager_uri_valid(gchar *uri_string);
gboolean redfish_check_smm_chassis_uri_valid(gchar *uri_string);
gint32 redfish_get_smm_health(guint8 *health);
void get_ethernetinterface_id(OBJ_HANDLE obj_handle, guint8 out_type, gchar *ethernetinterface_id, guint32 lenth);
gint32 get_manager_defaultip_group(OBJ_HANDLE *obj_handle);
gint32 system_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 chassis_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_overview_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 collection_storage_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 dataacquisition_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_data_acquisition_svc_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_msg_jso,
    gchar **o_rsp_body_str);
gint32 rack_data_acquisition_cfg_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_file_transfer_progress(gint32 *progress);

gint32 get_data_acquisition_report(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 dataacquisition_report_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 update_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 https_cert_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


#ifdef ARM64_HI1711_ENABLED
gint32 dice_cert_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 dice_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
#endif


gint32 security_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 security_srv_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 chassis_power_etag_del_property(json_object *object);
gint32 manager_etag_del_property(json_object *object);
gint32 processor_etag_del_property(json_object *object);

gint32 create_account(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso);
gint32 generate_account_response_headers(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso);
gint32 delete_account(PROVIDER_PARAS_S *i_param, json_object *message_array_jso);
gint32 generate_del_account_response_headers(PROVIDER_PARAS_S *i_param, json_object **header_array_jso,
    const gchar *uri, json_object *message_array_jso);

gint32 create_session(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 o_id_len,
    json_object *o_message_array_jso);
gint32 generate_session_response_headers(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso);
gint32 delete_session(PROVIDER_PARAS_S *i_param, json_object *message_array_jso);
gint32 generate_del_session_response_headers(PROVIDER_PARAS_S *i_param, json_object **header_array_jso,
    const gchar *uri, json_object *message_array_jso);

gint32 get_provider_param_from_session(const char *session_id, PROVIDER_PARAS_S *o_provider_param);

gint32 verify_account_and_get_info(guchar from_webui_flag, const gchar *user_name, const gchar *password,
    const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id, guint32 role_id_len,
    gchar *o_real_user, guint32 real_user_len);

gint32 verify_local_account_and_get_info(guchar from_webui_flag, const gchar *user_name, const gchar *password,
    const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id, guint32 role_id_len,
    gchar *o_real_user, guint32 real_user_len);

gint32 verify_special_ldap_account_and_get_info(guchar from_webui_flag, guint8 domain, const gchar *user_name,
    const gchar *password, const gchar *client, guchar *o_auth_type, guchar *o_user_id, guchar *o_pri, gchar *o_role_id,
    guint32 role_id_len, gchar *o_real_user, guint32 real_user_len);
gint32 create_volume(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso);
gint32 generate_create_rsc_response_headers_task(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso);
gint32 delete_volume(PROVIDER_PARAS_S *i_param, json_object *o_message_array_jso);

gint32 generate_del_response_headers_task(PROVIDER_PARAS_S *i_param, json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso);
gint32 ld_get_uri(OBJ_HANDLE obj_handle, gchar *o_ld_uri, gint32 uri_len);


gint32 ld_get_member_id(OBJ_HANDLE obj_handle, gchar *o_member_id, gint32 member_id_len, guint8 *raid_id);


gint32 redfish_check_storages_ld_uri_effective(const gchar *uri, OBJ_HANDLE *obj_handle, gboolean is_action_info);
gint32 check_delete_property_by_class(json_object *jso, const gchar *prop_path, json_bool *delete_flage);

gint32 system_provider_logcollection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_provider_log_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 generate_post_rsc_etag(PROVIDER_PARAS_S *i_param, const gchar *member_id, json_object **o_header_array_jso,
    json_object *message_array_jso);

gint32 generate_rsc_create_response_headers(PROVIDER_PARAS_S *i_param, gchar *id, json_object **header_array_jso,
    json_object *message_array_jso);

gint32 evt_svc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 evt_subscription_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 get_evt_svc_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_evt_subscription_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso,
    json_object *o_err_array_jso, gchar **o_rsp_body_str);

gint32 get_evt_subscription_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 generate_del_common_response_headers(PROVIDER_PARAS_S *i_param, json_object **header_array_jso, const gchar *uri,
    json_object *message_array_jso);

gint32 create_subscription_entry(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso);

gint32 delete_subscription(PROVIDER_PARAS_S *i_param, json_object *message_array_jso);

const gchar *get_rf_event_health_string(guchar event_type, guchar severity_level);

gint32 server_identity_source_trans(guchar *source_index, gchar *str_buf, guint32 str_buf_len,
    IDENTITY_TRANS_DIRECTION direction);

gint32 rf_validate_rsc_exist(const gchar *rsc_uri, json_object **rsc_jso);

gint32 task_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 task_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 evt_subscription_col_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 get_location_devicename(OBJ_HANDLE component_handle, gchar *string_value, gint32 string_value_len);
gint32 get_networkports_members_count(OBJ_HANDLE netcard_obj, json_object **o_result_jso, gint32 *count);
gint32 get_networkport_number(OBJ_HANDLE busi_obj, gchar *port_num, guint buf_len);

gint32 get_network_members_count(json_object **o_result_jso, gchar *uri_template, gint32 *count);
gint32 check_network_adapter_url(const gchar *network_adapter_id, OBJ_HANDLE *netcard_obj);
gint32 get_network_adapter_id(OBJ_HANDLE netcard_obj, gchar *network_id, guint32 buf_len);


gint32 provider_get_prop_values(PROVIDER_PARAS_S *provider_param, json_object **o_resource_jso,
    json_object *o_message_array_jso, const PROPERTY_PROVIDER_S *provider, const guint32 count);

gint32 rsc_del_none_etag_affected_property(json_object *object, SPECIAL_PROP_S *prop_config, guint32 config_count);

void get_chassis_oem_drive_summary(json_object **oem_tem);


gint32 redfish_check_uri_type(const gchar *uri, gint32 *type);

gint32 chassis_thermal_etag_del_property(json_object *object);

gint32 system_memory_etag_del_property(json_object *object);

gint32 chassis_etag_del_property(json_object *object);

gint32 chassis_pciedevice_etag_del_property(json_object *object);

gint32 chassis_drive_etag_del_property(json_object *object);

gint32 chassis_opticalmodule_etag_del_property(json_object *object);

gint32 registry_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 registry_rsc_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 get_registry_file_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_event_registry_file_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_event_lang_version(gchar** major_ver, gchar** minor_ver, gchar** aux_ver);

gint32 accountservice_provider_import_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 accountservice_provider_delete_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 task_monitor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_task_monitor_id_value(const gchar *uri, gint32 *monitor_id);
gint32 get_task_monitor_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
json_object *monitor_create_location(void);

gint32 rf_save_import_content_to_file(PROVIDER_PARAS_S *i_paras, const gchar *import_type_str, const gchar *content_str,
    gint32 cert_format_type, gchar *file_path, guint32 file_path_len, guchar *cert_store_type,
    json_object **o_message_jso);

gint32 manager_provider_vmmaction_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_provider_vmmcollection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_vmm_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_provider_usbstick_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 ldapservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 ldapservice_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 ldapservice_controller_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 ldapservice_controller_provider_import_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 ldapservice_controller_provider_import_crl_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 kerberosservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 kerberosservice_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 kerberos_controller_provider_import_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 kerberos_controller_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 get_set_function_normal_ret(PROVIDER_PARAS_S *i_paras, gint32 ret, json_object **o_message_jso,
    const gchar *key_str, const gchar *value_str);
gint32 get_set_function_complex_ret(PROVIDER_PARAS_S *i_paras, gint32 ret, json_object **o_message_jso,
    const gchar *key_str, const gchar *value_str);
void get_operation_result(gint32 ret, gboolean *is_success, gint32 *ret_code);

GHashTable *url_hash_table_init(void);
void url_hash_table_destroy(GHashTable *url_hash_table);
gint32 url_hash_table_insert(GHashTable *url_hash_table, const gchar *key, json_object *rsc_jso);
json_object *url_hash_table_lookup(GHashTable *url_hash_table, const gchar *key);

gint32 fill_rsc_id_for_uri_pattern(const gchar *uri_pattern, GSList **uri_list, GHashTable *hash_table);

gint32 get_privilege_map_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 accountservice_provider_import_crl_actioninfo_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 rf_resolve_action_name_from_rsc_jso(json_object *resource_jso, const gchar *action_path,
    gchar *resolved_action_name, guint32 action_name_len);

gint32 rf_validate_action_uri(const gchar *action_uri, GHashTable *hash_table);

gint32 workrecord_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 managers_provider_diagnosticservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_logservice_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_logservice_operatelog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_logservice_runlog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_logservice_securitylog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_operatelog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_runlog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_provider_securitylog_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_operatelog_item_details(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_runlog_item_details(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_securitylog_item_details(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 manager_spservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_raids_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_raid_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_osinstalls_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_osinstall_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_updates_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_update_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_results_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_result_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_cfgs_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_cfg_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 manager_sp_up_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_deviceinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
void make_sp_error_message(gint32 *ret_val, gchar *url, json_object **message_jso);
gint32 manager_sp_diagnose_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_diagnose_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_sp_diagnose_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_diagnose_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 create_sp_diagnose_conf(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso);
gint32 manager_sp_drive_erase_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_sp_drive_erase_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_sp_drive_erase_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso,
    json_object *o_err_array_jso, gchar **o_rsp_body_str);
gint32 get_sp_drive_erase_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 create_sp_drive_erase_conf(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso);
gint32 get_sp_file_uri_members(PROVIDER_PARAS_S *i_paras, gchar *format_uri, guchar pathid, json_object *json_obj);
gint32 get_sp_file_data(PROVIDER_PARAS_S *i_paras, guchar pathid, gchar *filename, json_object *json_obj);
gint32 get_sp_collection_odata_context_id(json_object *o_rsc_jso, gchar *metadata, gchar *odataid);
gint32 get_sp_memberid_odata_context_id(json_object *o_rsc_jso, gchar *metadata, gchar *odataid, gchar *memberid);
gint32 get_sp_pcie_memberid_id(json_object *o_rsc_jso, PROVIDER_PARAS_S *i_paras);
gint32 create_sp_item_template(PROVIDER_PARAS_S *i_paras, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    const gchar *filename, gint32 pathid);
void write_sp_task_desc(PROVIDER_PARAS_S* i_paras, const gchar* file_content);
gint32 create_sp_osinstall_id(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso);
gint32 create_sp_raid_id(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *o_id, guint32 id_len,
    json_object *o_message_array_jso);

gint32 get_sp_raids_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 manager_sp_raid_currents_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_sp_raid_currents_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 manager_sp_raid_current_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_spraid_export_config_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 manager_update_sp_schema_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 get_sp_raid_current_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_raid_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_osinstalls_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_osinstall_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_updates_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
void get_sp_result_in_mem(json_object *jso_dst);
gint32 create_sp_result_monitor_task(void);
gint32 get_sp_results_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_result_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_cfgs_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_cfg_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_sp_deviceinfo_file_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_sp_osinstall_status(PROVIDER_PARAS_S *i_paras);

gint32 check_power_state_on(void);
gboolean check_pangea_node_to_enc(OBJ_HANDLE component_handle);

gint32 rf_support_sp_service(void);

gint32 _sp_get_connect_state(void);


gboolean redfish_check_board_memberid(OBJ_HANDLE obj_handle, guint8 i_type_id, const gchar *i_memberid);


gint32 managers_provider_ntpservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 managers_provider_kvmservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_kvm_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 managers_provider_ntpaction_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 parse_file_transfer_err_code(gint32 transfer_status, json_object **message_obj);
gint32 verify_ip_protocol_enabled(OBJ_HANDLE group_obj, const gchar *ip_ver);
gint32 verify_ip_mode(OBJ_HANDLE group_obj, const gchar *ip_ver);
gint32 provider_jsonschemas_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 rf_check_array_length_valid(json_object *array, gint32 limit_length, json_object **message_jso,
    const gchar *json_pointer);

gboolean check_array_object(json_object *object);

void rf_check_property_deleted(json_object *value_jso, const gchar *property_str, gboolean *flag);
gint32 rf_check_file_path(const gchar *value_str);

void rf_add_action_prop(json_object *action_prop_jso, const gchar *rsc_uri, const gchar *action_name);

gint32 https_cert_actioninfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 storage_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
void get_free_blocks_space(OBJ_HANDLE array_obj_handle, guint8 span_depth, json_object *array_jso);
gint32 system_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);

gint32 rf_get_actioninfo_uri(const gchar *rsc_uri, const gchar *action_name, gchar *actioninfo_uri, guint32 uri_len);

gint32 rf_update_rsc_actioninfo_uri(const gchar **rsc_action_array, guint32 action_count,
    const gchar *original_action_info_uri, const gchar *rsc_uri, gchar *formatted_actioninfo_uri, guint32 uri_len);

gint32 rf_bios_get_registryversion(gchar *value, guint32 value_len, gint32 flag);
gint32 return_value_judgment(gint32 arr_subscript, gint32 *return_array);
gint32 managers_provider_vncservice_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 rf_get_property_encrypted_data(const gchar *property_plain_data, GVariant **encrypted_data_var);

gint32 rf_get_property_plain_data(GVariant *encrypted_data_var, gchar **plain_data);
gint32 rf_get_regex_match_result_nth(const gchar *pattern, const gchar *string, guchar position, gchar **result);

gint32 get_chassis_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 rf_get_chassis_component_info_by_type(guchar component_type, CHASSIS_COMPONENT_INFO *chassis_component_info);

gint32 rf_gen_chassis_component_id(const gchar *rsc_id, OBJ_HANDLE component_handle, gchar *chassis_id_buf,
    guint32 buf_len);

gint32 rf_get_object_location_handle(guint32 recursive_depth, OBJ_HANDLE obj_handle, OBJ_HANDLE *location_handle);

gint32 rf_get_chassis_component_info_by_id(const gchar *chassis_id, CHASSIS_COMPONENT_INFO *chassis_component_info,
    guchar *rsc_index);

void rf_add_property_jso_string(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_double(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_byte(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_uint16(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_uint16_hex(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_int16(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_prop_json_double(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso, gchar *format);
void rf_add_property_jso_uint32(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_bool(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

void rf_add_property_jso_presence(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

gint32 rf_update_rsc_component_info(OBJ_HANDLE obj_handle, json_object *rsc_jso);

gint32 get_chassis_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 get_rack_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);

gint32 rf_string_check(const gchar *data);

gint pcie_connector_compare_func(gconstpointer a, gconstpointer b);

gboolean redfish_check_chassis_pciedevice_uri_effective(const gchar *rsc_uri, OBJ_HANDLE *o_obj_handle);

gboolean redfish_check_pciedevice_location_devicename(const gchar *location_devicename, OBJ_HANDLE *o_obj_handle);

void gen_enclosure_contains_chassis_array(json_object *uri_array_jso);


gint32 check_enclosure_component_type(OBJ_HANDLE chassis_handle, gboolean is_support_node_enclosure);
gint32 thermal_clear_history_temp_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 get_system_memorys_count(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_manager_bmc_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
void get_system_overview_present_fan_num(json_object **o_result_jso);
void get_system_overview_inlet_temp_info(json_object *o_result_jso);
void get_system_overview_fansummary(json_object **o_result_jso);
gint32 get_system_overview_systems(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_system_overview_chassis(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_system_overview_managers(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_common_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 system_overview_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
void get_system_overview_power(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
void get_system_overview_mac(json_object **o_result_jso);
gint32 find_chassis_component_handle_with_id(const gchar *chassis_id, OBJ_HANDLE *component_handle);

gint32 redfish_get_support_sys_power_type(guchar* enable_type);

gint32 redfish_get_x86_enable_type(guchar *enable_type);

gint32 rf_gen_enc_contained_component_chassis_slot(gchar *chassis_slot, guint32 max_slot_len);

gint32 rf_gen_component_uri(OBJ_HANDLE component_handle, gchar *rsc_uri, guint32 rsc_uri_len);

gint32 set_fru_product_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guchar fru_id,
    json_object **o_result_jso);


gint32 fabric_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 pcie_fabric_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_pcie_fabric_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 config_composition_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 zone_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_zone_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 endpoint_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_endpoint_collection_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 zone_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_zone_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 endpoint_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_endpoint_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 rf_check_rsc_uri_valid_with_property_list(const gchar *rsc_id, const gchar *class_name, GSList *property_list,
    OBJ_HANDLE *obj_handle);
gint32 get_pci_id_property_uint16(OBJ_HANDLE obj_handle, gchar *property_name, json_object **o_result_jso);



gint32 peripheral_fw_inventory_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_peripheral_fw_inventory_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);



gint32 rf_get_elabel_handle_of_fru(OBJ_HANDLE fru_obj_handle, OBJ_HANDLE *elabel_obj_handle);



gint32 check_logservice_hostlog_service_support(const gchar *session_id);

gint32 rf_inner_session_id_check(const gchar *session_id);


gint32 sp_info_cache(const char *sp_para_name, const char *sp_para_path, json_object *body_jso_checked);



gint32 system_network_bridge_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_network_bridge_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 rf_get_rsc_odata_prop(const gchar *uri_template, const gchar *rsc_id, gchar *o_odata_prop,
    guint32 o_odata_prop_len);
gint32 system_networkbondings_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_networkbonding_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 create_vlan(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso);
gint32 delete_vlan(PROVIDER_PARAS_S *i_param, json_object *o_message_array_jso);
gint32 ipinfo_config_status_monitor_fn(const char *origin_obj_path, json_object *body_jso,
    TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
TASK_MONITOR_INFO_S *task_info_new_function(OBJ_HANDLE obj_handle, guint32 free_state_flag, const gchar *state_prop,
    const gchar *result_prop, gchar *operation_name, gchar *rsc_url, const gchar *rf_method);
gint32 system_vlan_acinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_networkbonding_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 create_network_bonding(PROVIDER_PARAS_S *i_param, json_object *body_jso_checked, gchar *id, guint32 id_len,
    json_object *message_array_jso);
gint32 delete_network_bonding(PROVIDER_PARAS_S *i_param, json_object *o_message_array_jso);
gint32 system_infinibands_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 system_infiniband_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 system_ethernetinterface_acinfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);

gint32 get_chassis_pciefunction_oem_associatedresource(OBJ_HANDLE i_obj_handle, json_object **o_json_prop);


gint32 get_system_ethernetinterface_link_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_system_ethernetinterface_ipv4s(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_system_ethernetinterface_ipv6s(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_system_ethernetinterface_ipv6_gateway(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gchar *uri_string);
gint32 get_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    gchar *uri_string);
gint32 chassis_discretesensor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_thresholdsensor_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 threshold_sensor_etag_del_property(json_object *object);


gint32 get_chassis_sensor_threshold(OBJ_HANDLE handle, const gchar *property_name, json_object *obj_json,
    const gchar *rf_property, guint16 readable, SENSOR_CAPABILITY_S *sensor_cap);
gint32 redfish_prase_severity_str_to_jso(const gchar *alarm_severity_str, json_object **o_result_jso);
gint32 redfish_get_severity_from_jso(json_object *jso_obj, gchar *alarm_severity_str, size_t buffer_len);


gboolean check_is_support_node_enclosure(guint32 product_id, guchar board_type);
gboolean is_node_enc_rsc(OBJ_HANDLE component_handle, guchar *board_type, guint32 *product_id, guchar *component_type);



gint32 get_fdmservice_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 manager_fdmservice_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 fdm_event_reports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 fdm_regenerate_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 fdm_remove_alarm_action_info_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 check_fdm_enabled(void);
gint32 manager_fdmservice_etag_del_property(json_object *object);



gint32 chassis_provider_unit_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_chassis_unit_rsc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);



gint32 get_rack_mgnt_service(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_session_agent(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gboolean rf_support_rack_mgnt_agent_service(void);
gint32 rack_mgnt_service_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 rack_mgnt_session_agent_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 rack_mgnt_set_dev_token_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);


gint32 rack_mgnt_period_pre_pwr_capping_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 rack_mgnt_set_pwr_capping_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 rack_mgnt_power_agent_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_power_agent(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 rack_power_agent_etag_del_property(json_object *object);




gint32 get_network_agent(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 rack_mgnt_network_agent_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 rack_mgnt_set_dev_ipinfo_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras,
    PROPERTY_PROVIDER_S **prop_provider, guint32 *count);


gint32 proxy_copy_file(guchar from_webui_flag, const gchar *user_name, const gchar *client, const gchar *src_file,
    const gchar *dest_file, guint32 user_id, guint32 group_id, guint32 delete_flag);
gint32 proxy_modify_file_mode(guchar from_webui_flag, const gchar *user_name, const gchar *client,
    const gchar *dest_file, guint32 user_id, guint32 group_id);
void proxy_revert_file_mode_on_exit(const gchar *dest_file, guint32 user_id, guint32 group_id);
gint32 proxy_delete_file(guchar from_webui_flag, const gchar *user_name, const gchar *client, const gchar *dest_file);
gint32 proxy_create_file(const gchar *dest_file, const gchar *create_flag, guint32 user_id, guint32 group_id,
    guint32 file_mode);
gint32 manager_provider_usbstick_ibma_actinfo_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 manager_provider_usbstick_ibma_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 get_usbstick_ibma_service_support_state(void);
gint32 redfish_usbstickcontrol_disconnect(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 redfish_usbstickcontrol_connect(OBJ_HANDLE obj_handle, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_manager_usbstick_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_usbstick_mediatype(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 act_usbstick_control_oem_process(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 rf_check_para_and_priv(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 rf_get_product_id(guint32 *product_id);
gint32 rf_check_uri_endpoint_member(const char *uri_endpoint, const gchar *class_name, const gchar *property_name,
    guint32 out_len, gchar *property_value);
gint32 get_ums_server_flag(guint8 *ums_server_flag);
gint32 get_installable_bma_state(void);
gint32 chassis_enclosure_power_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 smm_chassis_enclosure_hdd_drawer_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 chassis_enclosure_power_etag_del_property(json_object *object);
gint32 smm_chassis_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 smm_chassis_enclosure_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 smm_chassis_hmm_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 smm_get_chassis_bmc(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 smm_chassis_enclosure_thermal_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 smm_chassis_enclosure_thermal_rsc(PROVIDER_PARAS_S* i_paras, json_object* o_rsc_jso,
                                         json_object* o_err_array_jso, gchar** o_rsp_body_str);
gint32 smm_chassis_enclosure_thermal_etag_del_property(json_object *object);
gint32 smm_get_systems_collection(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 smm_get_managers_collection(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 smm_account_node_provider_collection_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
void get_smm_name_from_redfish_uri(const gchar *prefix, const gchar *uri, gchar *smm_name, gint32 buffer_len);
gint32 smm_manager_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 smm_get_overview(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 redfish_ipmi_send_msg_to_blade(IPMIMSG_BLADE_T *header, gsize resp_size, gpointer *response,
    gboolean wait_response);
gint32 redfish_get_blade_bmc_ip_config_by_cmd(guint8 slave_addr, guint8 net_func, guint8 cmd, guint8 filter,
    gchar *result, guint16 result_len);
gint32 smm_get_ipv4_config(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 smm_get_ipv6_config(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
const gchar *get_redfish_health_level(guint8 severity_level);
gint32 redfish_get_blade_health(guint8 slave_addr, guint8 *health);
gint32 redfish_health_count_to_level(guint32 minor, guint32 major, guint32 critical, guint8 *health);
void get_manager_oem_system_manager_info(json_object **obj);
void get_manager_oem_product_id(json_object** oem_property);
gint32 set_system_manager_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
#ifdef ARM64_HI1711_ENABLED
gint32 lanswitch_collection_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count);
gint32 lanswitch_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 lanswitch_ports_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 lanswitch_port_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 lanswitch_port_enable_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 chassis_switch_port_provider_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 chassis_switch_port_enable_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 rf_check_mesh_resource_legality(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 rf_check_blade_name(gchar* uri);
gint32 rf_check_mesh_name(gchar* uri);
gint32 rf_check_switch_name(gchar* uri);
gint32 rf_check_port_id_and_type(OBJ_HANDLE object_handle, const gchar *switch_name);
gint32 rf_check_lsw_resource_legality(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 rf_check_lsw_blade_name(gchar *uri, const gchar **blade_name);
gint32 rf_check_lsw_port_by_blade(OBJ_HANDLE port_handle, const gchar *blade_name);
gint32 rf_check_format_blade_name(PROVIDER_PARAS_S *i_paras, gchar *blade_name_format, gsize max_blade_name_len);
gint32 rf_get_uri_info_by_position(gchar *uri, guint32 position, gchar *uri_name, gint32 uri_len);
gint32 rf_check_lsw_port_by_position(OBJ_HANDLE object_handle, gchar *port_name, gsize max_port_name_len);
gint32 rf_format_const_string_upper_head(const gchar *src_string, guint32 src_length, gchar *dst_string,
    guint32 dst_length);
gint32 chassis_switches_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 chassis_switch_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
gint32 chassis_switch_ports_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** provider, guint32* count);
#endif

void cert_import_info_free_func(void *monitor_data);
gint32 add_all_action_prop(json_object *action_obj, const gchar *action_name, const gchar *slot, const gchar *target,
    const gchar *actioninfo);
gint32 get_logservice_datetime(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_logservice_time_offset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
void get_value_unc_uc_unr_lnc_lc_lnr(OBJ_HANDLE obj_node, json_object *obj_json, gchar *value_str);
void get_temperature_status(OBJ_HANDLE obj_node, json_object *obj_json);
gint32 get_fru_elabel_handle(guint32 position, OBJ_HANDLE *elabel_handle);
gint32 set_system_manager_info(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

const gchar *get_chassis_pciedevices_cardtype(OBJ_HANDLE pcie_handle);
guint8 redfish_get_pciedevice_component_type(OBJ_HANDLE obj_handle);
gint32 import_cert_common_status_monitor(const char *origin_obj_path, json_object *body_jso,
    process_local_cert_import_fn local_import_fn, TASK_MONITOR_INFO_S *monitor_fn_data, json_object **message_obj);
TASK_MONITOR_INFO_S *cert_import_info_new(PROVIDER_PARAS_S *i_paras, import_cert_set_userdata_fn set_userdata_fn);
void set_import_cert_params(IMPORT_CERT_PARAMS_S *param, IMPORT_CERT_FILE_TYPE file_type,
    import_cert_set_userdata_fn set_userdata_fn, process_local_cert_import_fn local_import_fn,
    process_remote_cert_import_fn remote_ipmort_fn, gchar *tmp_file_path);

gint32 is_import_cert_or_crl_task_exist(task_status_monitor_fn monitor_fn, PROVIDER_PARAS_S *i_paras,
    IMPORT_CERT_FILE_TYPE file_type, json_object **o_message_jso);

gint32 rf_start_file_download(PROVIDER_PARAS_S *i_paras, const gchar *remote_uri, gint32 transferfileinfo_code,
    json_object **o_message_jso);

gint32 process_cert_or_crl_import(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    IMPORT_CERT_PARAMS_S *import_cert_params);
void import_cert_set_common_userdata_fn(PROVIDER_PARAS_S *i_paras, json_object *user_data);
void import_cert_get_common_userdata_fn(const gchar **user_name, const gchar **client, gint32 *user_priv,
    gint32 *from_webui_flag, json_object *user_data);

void parse_import_crl_common_ret_code(gint32 ret_code, json_object **o_message_jso, gint32 *http_code);
gint32 check_root_cert_id(json_object *request_body, json_object **o_message_jso, const gchar *class_name,
    OBJ_HANDLE *cert_handle);
gint32 is_support_mutual_auth(json_object **o_message_jso, const gchar *property_name);
gint32 act_import_mutual_auth_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 act_delete_mutual_auth_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

gint32 get_controller_id(const char *uri, guchar *controller_id, OBJ_HANDLE *obj_handle);
gint32 get_controller_crl_enabled(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_controller_crl_start_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_controller_crl_expire_time(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
gint32 get_ldap_controller_action_info_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso, const gchar *actioninfo_uri_prifix, const gchar *actioninfo_uri_postfix);
gint32 act_import_ldap_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 act_delete_ldap_crl(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 import_ldap_cert_check_param(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso,
    guchar *controller_id, OBJ_HANDLE *obj_handle);
gint32 ret_value_judge(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint32 ret);
gint32 set_indicator_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
gint32 get_indicator_led_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

PROPERTY_PROVIDER_S *get_self_provider_service(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S* provider,
    gsize lens);
gint32 get_health_and_uid_status(PROPERTY_PROVIDER_S* provider, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
gint32 set_health_and_uid_status(PROPERTY_PROVIDER_S* provider, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso);
gint32 chassis_bbus_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 chassis_bbu_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count);
gint32 get_thermal_src(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str);
gint32 get_thermal_fans(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso);
const gchar *get_chassis_pciedevices_cardtype(OBJ_HANDLE i_pcie_handle);
guint8 redfish_get_pciedevice_component_type(OBJ_HANDLE i_obj_handle);
gboolean is_lom_hide_phy_port(OBJ_HANDLE businessport_obj);
void format_xpu_member_id(gchar *member_id, gsize str_len);
void get_npu_temp_or_power(OBJ_HANDLE obj_handle, const gchar *prop_name, const gchar *rf_prop,
    json_object *o_result_jso);
void get_npu_ecc_info(OBJ_HANDLE obj_handle, json_object* o_result_jso);
void get_npu_memory_info(OBJ_HANDLE obj_handle, json_object* o_result_jso);
void get_npu_eth_info(OBJ_HANDLE obj_handle, json_object* o_result_jso);

void get_processor_cache_size(OBJ_HANDLE obj_handle, const gchar *prop_name, const gchar *rf_prop_name,
    json_object *o_result_jso);

void get_system_overview_battery(json_object **o_result_jso);
gint32 rf_gen_rsc_id_with_location_and_devicename(OBJ_HANDLE object_handle, gchar *output_str, guint32 str_len);

gboolean is_the_request_from_rack_inner_device(const gchar *client);
void update_irm_inner_access_node_timestamp(const gchar *client, RF_INTERFACE_TYPE_E type);
gint32 check_cert_import_by_uri(const gchar *str_uri, IMPORT_CERT_FILE_TYPE file_type, gboolean *is_local_import);
gint32 check_cert_import_by_text(const gchar *content_text, IMPORT_CERT_PARAMS_S *params, json_object **o_message_jso);
void parse_single_value(gboolean reading_success, json_object *obj_json, gchar *value_str, gchar value,
    gchar *str_value);
gboolean check_user_priv_for_query_task(PROVIDER_PARAS_S *i_paras, guint8 task_need_priv, const gchar *task_owner);
void get_sel_message_id(OBJ_HANDLE evt_handle, json_object **messageid_object);

gint32 is_valid_integer_jso_or_null(json_object* jso, const gchar* prop_name,
    gint64 lower_limit, gint64 upper_limit, json_object** o_message_jso);
gint32 json_array_is_empty(json_object *array_json);
gint32 manager_provider_fpcservice_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** provider, guint32* count);
gint32 manager_provider_fpcservice_memory_entry(PROVIDER_PARAS_S* i_paras,
    PROPERTY_PROVIDER_S** prop_provider, guint32* count);

gint32 format_cpu_model_by_cmcc(gchar *in_model, gchar *out_model, guint32 out_size);
void get_oem_prop_threshold_uint16(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);
void get_xpu_devicelocator_position(OBJ_HANDLE obj_handle, json_object* huawei_jso);
void get_npu_utl_oem_huawei(OBJ_HANDLE obj_handle, json_object* huawei_jso);
gint32 construct_npu_name(OBJ_HANDLE npu_handle, gchar *npu_name, size_t npu_name_len);
void delete_user_specific_file(PROVIDER_PARAS_S *i_paras, json_object *vak_jso, gchar* prop_name);
gint32 gen_firmwareinventory_retimer_rsc_id(OBJ_HANDLE object_handle, gchar *prefix, gchar *output_str,
                                             guint32 str_len, guint8 type);
gint32 start_sp_firmware_upgrade_monitor(OBJ_HANDLE object_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 check_pangea_uri_in_blade(const gchar* pattern, const gchar* string);
guint32 set_huawei_chassis_id(PROVIDER_PARAS_S* i_paras, json_object* o_message_jso, json_object* huawei_obj);
gint32 chassis_led_group_provider_entry(PROVIDER_PARAS_S* i_paras, PROPERTY_PROVIDER_S** prop_provider, guint32* count);
void get_storage_dpucard_extend_info(OBJ_HANDLE obj_handle, json_object *huawei_jso);
gint32 parse_import_ca_cert_result(gint32 ret, json_object **o_message_jso);
gint32 __call_file_trasfer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, gint32 *result, gint32 file_id);
gint32 get_ld_id_from_uri(guint8 controller_id, const gchar *i_obj_json_string, guint16 *ld_id);
gint32 get_soc_board_firm_memberid(OBJ_HANDLE obj_handle, gchar *buf, guint32 buf_len);
gboolean is_valid_board_firmver(OBJ_HANDLE obj_handle, const guint32 comp_idx);
gboolean is_mainboard_bios_supported(void);
gboolean is_imu_valid(void);
gint32 get_io_board_firm_memberid(OBJ_HANDLE firm_handle, gchar *buf, guint32 buf_len);
gint32 get_mcu_board_firm_memberid(OBJ_HANDLE firm_handle, gchar *buf, guint32 buf_len);

gboolean is_force_restart_enabled(void);
gint32 get_ps_handle_by_slot(guint8 slot_id, OBJ_HANDLE *obj_handle);
gint32 get_power_updatebale(const gchar *member_id, gchar *class_name, OBJ_HANDLE resource_obj, guchar *byte_value);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
