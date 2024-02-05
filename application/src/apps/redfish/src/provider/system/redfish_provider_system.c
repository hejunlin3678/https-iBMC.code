
#include "redfish_provider.h"
#include "redfish_message_system.h"
#include "redfish_provider_system.h"

#define SET_P_STATE 0
#define SET_T_STATE 1

#define RF_SYSTEM_VALUE_MAXLEN 128
#define RF_SYSTEM_BOSVERSION_MAXLEN 128
#define RF_SYSTEM_CPU_ID_MAXLEN 128
#define RF_SYSTEM_RESPOND_MAXLEN 256


#define RF_SYSTEM_OSVERSION_MAXLEN 128


#define RF_SYSTEM_BOOT_NONE "None"
#define RF_SYSTEM_BOOT_PXE "Pxe"
#define RF_SYSTEM_BOOT_HDD "Hdd"
#define RF_SYSTEM_BOOT_CD "Cd"
#define RF_SYSTEM_BOOT_BIOS "BiosSetup"
#define RF_SYSTEM_BOOT_FLOPPY "Floppy"

#define RF_SYSTEM_POST_STATE_POWER_OFF "PowerOff"
#define RF_SYSTEM_POST_STATE_IN_POST "InPost"
#define RF_SYSTEM_POST_STATE_DISC_COMPLETE "InPostDiscoveryComplete"
#define RF_SYSTEM_POST_STATE_FINISHED "FinishedPost"

#define RF_SYSTEM_HOSTINGROLE_BACK "Switch"
#define RF_SYSTEM_HOSTINGROLE_NO_BACK "ApplicationServer"


#define MAX_SLOT_NUMBER 255 // SlotNumber可能为255，防止出错
#define RECORD_FLAG 1

#define INET6_ADDRSTRLEN 46

#define MIN_SILKNUM 1

// 不走通电开机策略的场景
#define RFPROP_POWER_ON_SCENE_FIRMWARE_ACTIVE "FirmwareActive"      // BMC主动下电场景，升级CPLD、VRD需要AC生效


typedef enum {
    
    POWER_ON_FIRMWARE_ACTIVE_INDEX = 0,       // BMC主动下电场景，升级CPLD、VRD需要AC生效
    POWER_ON_STRATEGY_INDEX = 31        // 上电开机策略index，该bit为0代表执行通电开机策略，1代表不执行通电开机策略
} UNIT_POWER_ON_SCENE_IDX_T;

typedef struct power_on_strategy_exception {
    const gchar *strategy_exception_scene;
    guint8 strategy_exception_scene_idx;
} POWER_ON_STRATEGY_EXCEPTION;

typedef struct swi_ipv6_addr_info_str_s {
    gchar ipv6_address[INET6_ADDRSTRLEN]; // ipv6地址
    guint8 addr_prefix_len;               // ipv6地址前缀长度
} SWI_IPV6_ADDR_INFO_STR_S;

typedef struct swi_ipv6_route_info_str_s {
    gchar ipv6_address[INET6_ADDRSTRLEN]; // ipv6地址
    guint8 addr_prefix_len;               // ipv6地址前缀长度
    gchar ipv6_gateway[INET6_ADDRSTRLEN]; // ipv6 gateway
} SWI_IPV6_ROUTE_INFO_STR_S;

typedef gint32 (*set_oem_prop_handler)(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);

typedef struct {
    const gchar *rf_prop;
    set_oem_prop_handler handler;
} OEM_SET_PROP_INFO;

#define NPU_ABILITY_TYPE_LENGTH 15
#define NPU_ABILITY_VALUE_MAX 65535
#define NPU_ABILITY_ARRAY_NUM 20

LOCAL const gchar system_allowable_values[] =
    "{\"ResetType\": [\"On\", \"ForceOff\", \"GracefulShutdown\", \"ForceRestart\", \"Nmi\", \"ForcePowerCycle\"]}";
LOCAL gint32 get_system_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_system_hostingrole(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_system_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL void get_system_health_led_state(json_object* o_result_jso);
LOCAL gint32 get_system_powerstate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL guint8 rfsystem_get_switch_plane_powerstatus(guchar *power_state);
LOCAL gint32 get_system_biosversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_processorsummary(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_memorysummary(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_boot(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_system_boot(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 set_system_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_system_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_ethernetinterfaces(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_processors(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_memory(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_networkinterfaces(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_system_storage(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_bios(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_system_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_systems_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_systems_action_target(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 redfish_create_uri(const gchar *only_uri, gchar *change_uri, gint32 change_urilen);
LOCAL gint32 redfish_make_array_uri(gchar *link_val, const gchar *only_uri, gchar *nature, json_object *change_uri);
LOCAL gint32 act_system_frucontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 act_system_clear_network_usagerate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_system_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_power_delay_mode_and_count(OBJ_HANDLE obj_handle, json_object *o_result_jso);
LOCAL gint32 set_system_oem_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 act_system_add_swi_ipv6addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_del_swi_ipv6addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_add_swi_ipv6route(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 act_system_del_swi_ipv6route(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL void rf_add_mem_free(OBJ_HANDLE obj_handle, json_object *o_result_jso);

LOCAL gint32 get_system_huawei(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso);



LOCAL gint32 get_system_logservice(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_system_trustedmodules(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 redfish_set_oem_utilisethreshold(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array, guchar type);
LOCAL gint32 redfish_set_oem_gracefulpowerofftimeoutperiod(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);

LOCAL gint32 redfish_set_oem_hostname_sync_enable(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);
LOCAL gint32 set_system_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_system_huawei_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *out_val, guint32 len,
    json_object *o_result_jso, const gchar *resource_name);
LOCAL void set_system_oem_huawei_para(gint32 *return_array, gint32 *arr_subscript, gint32 ret, gint32 len);

LOCAL gint32 redfish_set_oem_productalias(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);
LOCAL gchar *rfsystem_to_system_trust_state(guchar truststate_byte);
LOCAL gint32 redfish_set_oem_gracefulpowerofftimeoutenabled(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);
LOCAL gint32 redfish_set_oem_configuration_model(PROVIDER_PARAS_S *paras,
    json_object *property_jso, json_object** msg_array);
LOCAL gint32 redfish_set_oem_cmcc_version(PROVIDER_PARAS_S *paras, json_object* property_jso, json_object** msg_array);
LOCAL gint32 redfish_set_oem_cucc_version(PROVIDER_PARAS_S *paras, json_object* property_jso, json_object** msg_array);
LOCAL gint32 get_system_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL void get_oem_prop_cpuptlimit(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso);
LOCAL void get_oem_prop_cpupt(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso);
LOCAL void get_oem_prop_panelpowerbuttonenabled(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);
LOCAL void get_oem_prop_powerrestorepolicy(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);
LOCAL void get_oem_prop_powerontrategyexceptions(OBJ_HANDLE obj_handle, const gchar *property, 
                                                 const gchar *rf_property,json_object *o_result_jso);
LOCAL void get_oem_partitionlists_object(json_object *huawei);
LOCAL void get_oem_prop_powerofftimeoutenabled(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);

LOCAL gint32 get_chassis_pciedevice(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 get_chassis_pciefunctions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 get_switch_plane_object(const gchar *switch_type, OBJ_HANDLE *obj_handle);
LOCAL void get_oem_bootup_sequence(json_object *huawei, PROVIDER_PARAS_S *i_paras);
LOCAL void get_manager_oem_kvm_enable(json_object **oem_property);
LOCAL gint32 set_system_local_kvm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);

LOCAL gint32 get_system_partnum(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso);
LOCAL gint32 redfish_set_oem_cpupt(PROVIDER_PARAS_S *i_paras, json_object *property_p_jso, json_object *property_t_jso,
    json_object **message_array, guchar type);
LOCAL void get_oem_prop_cpupandt(PROVIDER_PARAS_S *i_paras, guint8 *p_state, guint8 *t_state);


LOCAL gint32 get_memory_history_usage_rate_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_memory_history_usage_rate_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_memory_history_usage_rate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_network_history_usage_rate_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_network_history_usage_rate_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);
LOCAL gint32 get_network_history_usage_rate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso);

LOCAL gint32 create_common_return_message(gint32 ret_val, json_object **message_array, const gchar *prop_name);


LOCAL void redfish_get_energy_mode_enable(json_object *o_result_jso);
LOCAL void redfish_get_bios_dynamic_config_info(json_object *o_result_jso);
LOCAL gint32 set_bios_dynamic_cfg(PROVIDER_PARAS_S *i_paras, json_object **message_array, GSList *input_list);
LOCAL gint32 set_bios_energy_mode_status(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);
LOCAL gint32 set_bios_dynamic_cfg_info(PROVIDER_PARAS_S *i_paras, json_object **message_array,
    json_object *bios_energy_cfg_jso);
LOCAL gint32 get_demt_set_cfg_info(json_object *bios_energy_cfg_jso, json_object **message_array, guint8 *demt_cfg_info,
    guint32 demt_cfg_size, guint8 *demt_cfg_len, guint8 *same_cfg_flag);
LOCAL gint32 analysize_ouput_cfg_result(json_object **message_array, GSList *output_list);
LOCAL void redfish_get_intelligent_energy_saving_info(json_object *o_result_jso);
LOCAL void redfish_set_intelligent_energy_saving_info(json_object *huawei, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, gint32 *return_array, gint32 *return_array_index, guint32 return_array_size);
LOCAL gint32 create_return_message(gint32 ret_val, json_object **message_array, const gchar *prop_name);
LOCAL gint32 check_unsupported_product_type(void);


LOCAL void get_oem_prop_leakpolicy(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso);
LOCAL gint32 redfish_set_oem_leak_stragety(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array);
LOCAL void get_oem_prop_leakdetsupport(guint8 leakdet_support, const gchar *rf_property, json_object *o_result_jso);

LOCAL PROPERTY_PROVIDER_S g_system_provider[] = {
    
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_ASSETTAG, CLASS_ELABEL, PROPERTY_ELABEL_PRODUCT_ASSET_TAG, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_assettag, set_system_assettag, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_MANUFACTURER, CLASS_ELABEL, PROPERTY_ELABEL_PRODUCT_MFG_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_manufacturer, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_MODEL, CLASS_ELABEL, PROPERTY_ELABEL_PRODUCT_NAME, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_model, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_HOSTNAME, CLASS_COMPUTER_SYSTEM, PROPERTY_COMPUTER_SYSTEM_HOSTNAME, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_hostname, set_system_hostname, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_SERIALNUMBER, CLASS_ELABEL, PROPERTY_ELABEL_PRODUCT_SN, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_serialnumber, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_SYSTEM_UUID, BMC_CLASEE_NAME, BMC_DEV_GUID_NAME, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_uuid, redfish_set_uuid, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_SYSTEM_HOSTINGROLE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_hostingrole, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_STATUS, CLASS_WARNING, PROPERTY_WARNING_HEALTH_STATUS, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_status, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_POWERSTATE, CLASS_PAYLOAD, PROPERTY_PAYLOAD_CHASSPWR_STATE, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_powerstate, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_BIOSVERSION, BIOS_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_biosversion, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_PROCESSORSUMMARY, CLASS_CPU, PROPERTY_CPU_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processorsummary, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_MEMORYSUMMARY, CLASS_MEMORY, PROPERTY_MEM_HEALTH, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memorysummary, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_BOOT, BIOS_CLASS_NAME, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_boot, set_system_boot, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_RESET, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_system_reset, ETAG_FLAG_ENABLE},
    {RF_RESOURCE_ETHERNETINTERFACES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_ethernetinterfaces, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_PROCESSORS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_processors, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_MEMORY, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_memory, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_TRUSTEDMODULES, CLASS_SECURITY_MODULE, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_trustedmodules, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_INTERFACES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_networkinterfaces, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_SYSTEM_STORAGE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_storage, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_BIOS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_bios, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_COMMON_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_id, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFPROP_CHASSIS_PCIEDEVICES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciedevice, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_CHASSIS_PCIEFUNCTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_chassis_pciefunctions, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    
    {RFPROP_SYSTEM_LINKS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_links, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_ACTIONS, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_systems_action_target, NULL, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_SYSTEM_FRUCONTROL, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_system_frucontrol, ETAG_FLAG_ENABLE},
    {RFPROP_CLEAR_NETWORK_USAGERATE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_BASICSETTING, SYS_LOCKDOWN_FORBID, NULL, NULL, act_system_clear_network_usagerate, ETAG_FLAG_ENABLE},
    
    {RFPROP_FWINV_OEM, CLASS_COMPUTER_SYSTEM, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_system_oem, set_system_oem_huawei, NULL, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_SYSTEM_ADD_SWI_IPV6ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_system_add_swi_ipv6addr, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_DEL_SWI_IPV6ADDR, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_system_del_swi_ipv6addr, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_ADD_SWI_IPV6ROUTE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_system_add_swi_ipv6route, ETAG_FLAG_ENABLE},
    {RFPROP_SYSTEM_DEL_SWI_IPV6ROUTE, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_POWERMGNT, SYS_LOCKDOWN_ALLOW, NULL, NULL, act_system_del_swi_ipv6route, ETAG_FLAG_ENABLE},
    
    
    {RFPROP_SYSTEM_LOGSERVICES, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_system_logservice, NULL, NULL, ETAG_FLAG_ENABLE},
    

    
    {RFPROP_PART_NUMBER, CLASS_ELABEL, PROPERTY_ELABEL_PRODUCT_PN, USERROLE_READONLY, SYS_LOCKDOWN_NULL,  get_system_partnum, NULL, NULL, ETAG_FLAG_ENABLE},
    
    {RFACTION_SYSTEM_LOCAL_KVM, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_KVMMGNT, SYS_LOCKDOWN_FORBID, NULL, set_system_local_kvm, NULL, ETAG_FLAG_ENABLE}
};

#pragma pack(1)
typedef struct _tag_bios_dynamic_cfg_item_info {
    guint8 cfg_item_number;
    gchar cfg_ref_prop_name[BIOS_CFG_ITEM_DATA_MAX_LEN];
} BIOS_CFG_REF_PROP_INFO;
#pragma pack()


LOCAL BIOS_CFG_REF_PROP_INFO g_bios_cfg_ref_prop_name_tbl[] = {
    {BIOS_DYNAMIC_INVALID_ITEM,                 BIOS_CFG_DESP_NULL},
                                                                /*lint -save -e651 -e64*/
    {BIOS_DYNAMIC_DEMT,                             ERROR_PROP_DEMT_CFG_ITEM},
                                                                /*lint -restore */
    {BIOS_DYNAMIC_UFS,                                 BIOS_CFG_DESP_NULL},
    {BIOS_DYNAMIC_ENERGY_PERF_BIAS_CFG, BIOS_CFG_DESP_NULL},
    {BIOS_DYNAMIC_PL1_POWER_LIMIT,          BIOS_CFG_DESP_NULL},
    {BIOS_DYNAMIC_PL2_POWER_LIMIT,          BIOS_CFG_DESP_NULL},
    {BIOS_DYNAMIC_PL1_TIME_WINDOWS,      BIOS_CFG_DESP_NULL},
    {BIOS_DYNAMIC_PL2_TIME_WINDOWS,      BIOS_CFG_DESP_NULL},
    {BIOS_DYNAMIC_LOADLINE_SWITCH,         BIOS_CFG_DESP_NULL}
};


LOCAL gint32 check_unsupported_product_type(void)
{
    gint32 ret_val;
    OBJ_HANDLE pmeservice_handle = 0;

    gchar ro[MAX_NAME_SIZE] = {0};
    gchar rp[MAX_NAME_SIZE] = {0};

    
    (void)dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &pmeservice_handle);
    ret_val = dfl_get_referenced_property(pmeservice_handle, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT, ro, rp,
        MAX_NAME_SIZE, MAX_NAME_SIZE);
    return_val_do_info_if_expr(VOS_OK == ret_val, ret_val,
        debug_log(DLOG_INFO, "%s : PMEService energy_saving prop rp %s.%s, type support", __FUNCTION__, ro, rp));

    return BIOS_ERROR_PRODUCT_UNSUPPORTED;
}


LOCAL gint32 create_return_message(gint32 ret_val, json_object **message_array, const gchar *prop_name)
{
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    json_object *message_jso = NULL;

    return_val_do_info_if_expr(NULL == message_array || NULL == prop_name, method_ret,
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    switch (ret_val) {
        case VOS_OK:
            method_ret = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_FORBIDDEN;
            break;

        case BIOS_ERROR_STATUS_UNSUPPORTED:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_BAD_REQUEST;
            break;

        case BIOS_ERROR_PRODUCT_UNSUPPORTED:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_NOT_IMPLEMENTED;
            break;

        case BIOS_ERROR_BIOS_STATUS_UNSUPPORTED:
            (void)create_message_info(MSGID_BIOS_STATUS_UNSUPPORTED, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_BAD_REQUEST;
            break;

        case BIOS_ERROR_ENERGY_SAVING_DISABLED:
            (void)create_message_info(MSGID_BIOS_ENERGY_SAVING_DISABLED, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_BAD_REQUEST;
            break;

        case BIOS_ERROR_LICENSE_NOT_INSTALLED:
            (void)create_message_info(MSGID_LICENSE_NOT_INSTALL, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_BAD_REQUEST;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set bios energy saving info failed, return %d", __FUNCTION__, ret_val);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL void redfish_get_energy_mode_enable(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *energy_mode_jso = NULL;
    guint8 energy_mode_state = BIOS_ENERGY_MODE_INVALID;

    return_do_info_if_expr(NULL == o_result_jso, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_DEBUG, "%s: get Bios object handle fail", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_ENERGY_MODE_ENABLE, &energy_mode_state);
    goto_label_do_info_if_expr(energy_mode_state >= BIOS_ENERGY_MODE_INVALID, exit,
        debug_log(DLOG_DEBUG, "%s: invalid Bios energy mode value is %d", __FUNCTION__, energy_mode_state));

    energy_mode_jso = json_object_new_boolean(energy_mode_state);

exit:

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_BIOS_ENERGY_MODE_ENABLED, energy_mode_jso);

    return;
}


LOCAL void redfish_get_bios_dynamic_config_info(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *bios_dynamic_cfg_obj = NULL;
    guint8 demt_cfg_value = BIOS_DEMT_CFG_INVALID;

    return_do_info_if_expr(NULL == o_result_jso, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    bios_dynamic_cfg_obj = json_object_new_object();
    goto_label_do_info_if_fail(NULL != bios_dynamic_cfg_obj, exit,
        debug_log(DLOG_ERROR, "%s: call json_object_new_object failed", __FUNCTION__));

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_DEBUG, "%s: get Bios object handle fail", __FUNCTION__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_DEMT_CONFIG_ITEM, &demt_cfg_value);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get Bios DEMT cfg value failed", __FUNCTION__));
    json_object_object_add(bios_dynamic_cfg_obj, RFPROP_SYSTEM_BIOS_DEMT_CFG_PARA,
        demt_cfg_value >= BIOS_DEMT_CFG_INVALID ? NULL : json_object_new_boolean(demt_cfg_value));

exit:
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_BIOS_ENERGY_CONFIGURATION, bios_dynamic_cfg_obj);

    return;
}


LOCAL void redfish_get_intelligent_energy_saving_info(json_object *o_result_jso)
{
    guint8 energy_saving_support = 0;

    return_do_info_if_expr(NULL == o_result_jso, debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        &energy_saving_support);
    return_do_info_if_fail(1 == energy_saving_support,
        debug_log(DLOG_DEBUG, "%s : this resource not enabled by license", __FUNCTION__));

    // 查询BIOS动态配置功能使能属性
    redfish_get_energy_mode_enable(o_result_jso);

    // 查询BIOS动态配置信息
    redfish_get_bios_dynamic_config_info(o_result_jso);

    return;
}


LOCAL gint32 set_bios_energy_mode_status(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret_val;
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    guint8 energy_mode_status;
    json_bool energy_mode_status_int;
    GSList *input_list = NULL;
    const gchar *prop_name = ERROR_PROP_ENERGY_SAVING_ENABLED;
    OBJ_HANDLE bios_handle = 0;

    if (NULL == message_array || NULL == property_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return method_ret;
    }

    
    ret_val = check_unsupported_product_type();
    goto_label_do_info_if_fail(VOS_OK == ret_val, exit,
        debug_log(DLOG_DEBUG, "%s: product is not supported", __FUNCTION__));

    ret_val = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    goto_label_do_info_if_fail(VOS_OK == ret_val, exit,
        debug_log(DLOG_DEBUG, "%s: get bios energy saving mode failed, ret is %d", __FUNCTION__, ret_val));

    energy_mode_status_int = json_object_get_boolean(property_jso);
    if (TRUE == energy_mode_status_int) {
        energy_mode_status = ENABLED;
    } else {
        energy_mode_status = DISABLED;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(energy_mode_status));

    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, bios_handle,
        CLASS_NAME_BIOS, METHOD_SET_BIOS_ENERGY_MODE_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

exit:
    method_ret = create_return_message(ret_val, message_array, prop_name);
    return method_ret;
}


LOCAL gint32 analysize_ouput_cfg_result(json_object **message_array, GSList *output_list)
{
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    json_object *message_jso = NULL;
    const guint8 *output_data = 0;
    gchar *cfg_ref_prop_name = NULL;
    guint8 cfg_failed_item_num = 0;
    gsize list_index;
    gsize output_data_len = 0;

    if (NULL == message_array || NULL == output_list) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return method_ret;
    }

    output_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0), 
        &output_data_len, sizeof(guint8));
    return_val_do_info_if_expr((0 == output_data_len) || (output_data_len % 2 != 0) || (NULL == output_data),
        method_ret, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s: output data len %"G_GSIZE_FORMAT" is not valid", __FUNCTION__, output_data_len));

    for (list_index = 0; list_index < output_data_len; list_index += BIOS_INDEX_INTERVAL_LEN) {
        message_jso = NULL;
        continue_if_expr(BIOS_CFG_ITEM_SUCCESS == output_data[list_index + 1]);

        cfg_ref_prop_name = (g_strcmp0(g_bios_cfg_ref_prop_name_tbl[output_data[list_index]].cfg_ref_prop_name,
            BIOS_CFG_DESP_NULL) == 0) ?
            NULL :
            g_bios_cfg_ref_prop_name_tbl[output_data[list_index]].cfg_ref_prop_name;
        if (NULL != cfg_ref_prop_name) {
            (void)create_message_info(MSGID_INTERNAL_ERR, cfg_ref_prop_name, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
        }

        cfg_failed_item_num++;
    }

    // 有一个配置项配置成功，即返回成功
    if (cfg_failed_item_num < output_data_len / 2) {
        method_ret = HTTP_OK;
    }

    return method_ret;
}


LOCAL gint32 set_bios_dynamic_cfg(PROVIDER_PARAS_S *i_paras, json_object **message_array, GSList *input_list)
{
    GSList *output_list = NULL;
    OBJ_HANDLE bios_handle = 0;
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    json_object *message_jso = NULL;
    gint32 ret_val;

    if (NULL == message_array || NULL == input_list || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__);
        return method_ret;
    }

    ret_val = dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    return_val_do_info_if_fail(VOS_OK == ret_val, method_ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso); debug_log(DLOG_DEBUG,
        "%s failed: cann't get object handle for class [%s], ret is %d", __FUNCTION__, CLASS_NAME_BIOS, ret_val));

    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, bios_handle,
        CLASS_NAME_BIOS, METHOD_SET_BIOS_DYNAMIC_CONFIGURATION, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        &output_list);
    if (VOS_OK == ret_val) {
        method_ret = analysize_ouput_cfg_result(message_array, output_list);
    } else {
        method_ret = create_return_message(ret_val, message_array, ERROR_PROP_CFG_ITEM);
    }

    uip_free_gvariant_list(output_list);
    return method_ret;
}


LOCAL gint32 get_demt_set_cfg_info(json_object *bios_energy_cfg_jso, json_object **message_array, guint8 *demt_cfg_info,
    guint32 demt_cfg_size, guint8 *demt_cfg_len, guint8 *same_cfg_flag)
{
#define DEMT_CFG_INFO_LEN 3

    json_object *property_jso = NULL;
    json_object *message_temp_jso = NULL;
    guint8 demt_set_value;
    json_bool prop_exist;
    guint8 demt_cfg_offset = 0;
    OBJ_HANDLE bios_handle = 0;
    guint8 demt_cfg_save_value = 0;
    json_bool demt_status_int;

    if (NULL == message_array || NULL == bios_energy_cfg_jso || NULL == demt_cfg_info || NULL == demt_cfg_len) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    prop_exist = json_object_object_get_ex(bios_energy_cfg_jso, RFPROP_SYSTEM_BIOS_DEMT_CFG_PARA, &property_jso);
    return_val_do_info_if_expr(FALSE == prop_exist, HTTP_OK,
        debug_log(DLOG_DEBUG, "%s: demt cfg para is not in cfg list", __FUNCTION__));

    demt_status_int = json_object_get_boolean(property_jso);
    if (TRUE == demt_status_int) {
        demt_set_value = ENABLED;
    } else {
        demt_set_value = DISABLED;
    }

    
    return_val_do_info_if_fail(demt_cfg_size >= DEMT_CFG_INFO_LEN, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
        (void)json_object_array_add(*message_array, message_temp_jso); debug_log(DLOG_ERROR,
        "%s: demt cfg info len %d excceed max size %d", __FUNCTION__, DEMT_CFG_INFO_LEN, demt_cfg_size));

    // DEMT配置出参封装(TLV)
    (void)memset_s(demt_cfg_info, demt_cfg_size, 0, demt_cfg_size);
    demt_cfg_info[demt_cfg_offset++] = BIOS_DYNAMIC_DEMT;
    demt_cfg_info[demt_cfg_offset++] = sizeof(guint8);
    demt_cfg_info[demt_cfg_offset++] = demt_set_value;

    *demt_cfg_len = DEMT_CFG_INFO_LEN;

    // 判断下发配置是否与BMC本地保存一致
    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &bios_handle);
    (void)dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_DEMT_CONFIG_ITEM, &demt_cfg_save_value);
    if (demt_cfg_save_value == demt_set_value) {
        *same_cfg_flag = 1;
    }

    return HTTP_OK;
}


LOCAL gint32 set_bios_dynamic_cfg_info(PROVIDER_PARAS_S *i_paras, json_object **message_array,
    json_object *bios_energy_cfg_jso)
{
    errno_t safe_fun_ret = EOK;
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    guint8 bios_cfg_num = 0;
    guint32 bios_cfg_total_length = 0;
    guint32 bios_cfg_offset = 0;
    guint8 *bios_cfg_info = NULL;
    GSList *input_list = NULL;
    json_object *message_jso = NULL;

    guint8 demt_cfg_info[BIOS_CFG_ITEM_DATA_MAX_LEN] = {0};
    guint8 demt_cfg_len = 0;
    guint8 same_cfg_num = 0;  // 表示下发配置与BMC保存一致的配置个数
    guint8 same_cfg_flag = 0; // 标识下发的配置项是否与BMC保存一致

    if (NULL == message_array || NULL == bios_energy_cfg_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return method_ret;
    }

    
    method_ret = check_unsupported_product_type();
    return_val_do_info_if_fail(VOS_OK == method_ret, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, ERROR_PROP_CFG_ITEM, &message_jso, ERROR_PROP_CFG_ITEM);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_DEBUG, "%s: product is not supported", __FUNCTION__));

    
    if (0 == g_strcmp0(JSON_NULL_OBJECT_STR, dal_json_object_get_str(bios_energy_cfg_jso))) {
        debug_log(DLOG_DEBUG, "%s: bios cfg obj is empty", __FUNCTION__);
        return HTTP_BAD_REQUEST;
    }

    
    method_ret = get_demt_set_cfg_info(bios_energy_cfg_jso, message_array, demt_cfg_info, sizeof(demt_cfg_info),
        &demt_cfg_len, &same_cfg_flag);
    return_val_do_info_if_fail(HTTP_OK == method_ret, method_ret,
        debug_log(DLOG_ERROR, "%s: call get_demt_set_cfg_info failed", __FUNCTION__));
    do_if_expr(demt_cfg_len > 0, bios_cfg_num++; bios_cfg_total_length += demt_cfg_len);
    do_if_expr(same_cfg_flag == 1, same_cfg_num++; same_cfg_flag = 0);

    
    bios_cfg_info = (guint8 *)g_malloc0(sizeof(guint8) * bios_cfg_total_length);
    return_val_do_info_if_fail(NULL != bios_cfg_info, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso); debug_log(DLOG_ERROR,
        "%s: g_malloc0 failed, bios_cfg_total_length is %d", __FUNCTION__, bios_cfg_total_length));

    if (demt_cfg_len > 0) {
        safe_fun_ret = memcpy_s(bios_cfg_info + bios_cfg_offset, bios_cfg_total_length, demt_cfg_info, demt_cfg_len);
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    }

    
    
    if (bios_cfg_num == same_cfg_num) {
        bios_cfg_num = same_cfg_num | BIOS_CFG_ITEM_SAME_FLAG_OFFSET;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(bios_cfg_num));
    input_list = g_slist_append(input_list,
        (gpointer)g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, bios_cfg_info, bios_cfg_total_length, sizeof(guint8)));
    method_ret = set_bios_dynamic_cfg(i_paras, message_array, input_list);

    uip_free_gvariant_list(input_list);
    g_free(bios_cfg_info);

    return method_ret;
}


LOCAL void redfish_set_intelligent_energy_saving_info(json_object *huawei, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, gint32 *return_array, gint32 *return_array_index, guint32 return_array_size)
{
    gint32 ret = 0;
    json_object *property_jso = NULL;

    return_do_info_if_expr(NULL == huawei || NULL == i_paras || NULL == o_message_jso || NULL == return_array ||
        NULL == return_array_index,
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_BIOS_ENERGY_MODE_ENABLED, &property_jso)) {
        ret = set_bios_energy_mode_status(i_paras, property_jso, o_message_jso);
        // codex修改
        return_do_info_if_expr((guint32)(*return_array_index) >= return_array_size,
            debug_log(DLOG_ERROR, "%s: return array index %d out of range return array size %d", __FUNCTION__,
            (*return_array_index), return_array_size));
        return_array[*return_array_index] = ret;
        (*return_array_index)++;
    }

    property_jso = NULL;

    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_BIOS_ENERGY_CONFIGURATION, &property_jso)) {
        ret = set_bios_dynamic_cfg_info(i_paras, o_message_jso, property_jso);
        return_do_info_if_expr((guint32)(*return_array_index) >= return_array_size,
            debug_log(DLOG_ERROR, "%s: return array index %d out of range return array size %d", __FUNCTION__,
            (*return_array_index), return_array_size));
        return_array[*return_array_index] = ret;
        (*return_array_index)++;
    }

    return;
}


LOCAL gint32 create_common_return_message(gint32 ret_val, json_object **message_array, const gchar *prop_name)
{
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    json_object *message_jso = NULL;

    return_val_do_info_if_expr(message_array == NULL || prop_name == NULL, method_ret,
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    switch (ret_val) {
        case VOS_OK:
            method_ret = HTTP_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_FORBIDDEN;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set failed, return %d", __FUNCTION__, ret_val);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL gchar *rfsystem_startenable_to_string(guchar start_flag)
{
    if (EFFECTIVE_ONCE == start_flag) {
        return "Once";
    } else if (EFFECTIVE_NONE == start_flag) {
        return "Disabled";
    } else if (EFFECTIVE_FOREVER == start_flag) {
        return "Continuous";
    } else {
        return NULL;
    }
}


LOCAL gint32 rfsystem_start_flag_to_uchar(const gchar *start_flag, guchar *out_flag)
{
    if (g_strcmp0("Disabled", start_flag) == 0) {
        *out_flag = EFFECTIVE_NONE;
    } else if (g_strcmp0("Once", start_flag) == 0) {
        *out_flag = EFFECTIVE_ONCE;
    } else if (g_strcmp0("Continuous", start_flag) == 0) {
        *out_flag = EFFECTIVE_FOREVER;
    } else {
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gchar *rfsystem_startoption_to_string(guchar start_option)
{
    switch (start_option) {
        case 0:
            return RF_SYSTEM_BOOT_NONE;

        case FORCE_PEX:
            return RF_SYSTEM_BOOT_PXE;

        case FORCE_HARD_DRIVE:
            return RF_SYSTEM_BOOT_HDD;

        case FORCE_CD_DVD:
            return RF_SYSTEM_BOOT_CD;

        case FORCE_BIOS_SETUP:
            return RF_SYSTEM_BOOT_BIOS;

        case FORCE_FLOPPY_REMOVABLE_MEDIA:
            return RF_SYSTEM_BOOT_FLOPPY;

        default:
            return NULL;
    }
}

LOCAL gint32 rfsystem_bootmode_to_uchar(const gchar *start_flag, guchar *out_flag)
{
    
    if (g_strcmp0(RFPROP_SYSTEM_BOOT_MODE_LEGACY, start_flag) == 0) {
        *out_flag = BIOS_BOOT_LEGACY;
    } else if (g_strcmp0(RFPROP_SYSTEM_BOOT_MODE_UEFI, start_flag) == 0) {
        
        *out_flag = BIOS_BOOT_UEFI;
    } else {
        return VOS_ERR;
    }

    return VOS_OK;
}

LOCAL gchar *rfsystem_bootmode_to_string(guchar boot_mode)
{
    switch (boot_mode) {
        case BIOS_BOOT_LEGACY:
            return RFPROP_SYSTEM_BOOT_MODE_LEGACY;

        case BIOS_BOOT_UEFI:
            return RFPROP_SYSTEM_BOOT_MODE_UEFI;

        default:
            return NULL;
    }
}


LOCAL gint32 rfsystem_startoption_to_uchar(const gchar *start_option, guchar *boot_set)
{
    if (g_strcmp0(RF_SYSTEM_BOOT_NONE, start_option) == 0) {
        *boot_set = 0;
    } else if (g_strcmp0(RF_SYSTEM_BOOT_PXE, start_option) == 0) {
        *boot_set = FORCE_PEX;
    } else if (g_strcmp0(RF_SYSTEM_BOOT_HDD, start_option) == 0) {
        *boot_set = FORCE_HARD_DRIVE;
    } else if (g_strcmp0(RF_SYSTEM_BOOT_CD, start_option) == 0) {
        *boot_set = FORCE_CD_DVD;
    } else if (g_strcmp0(RF_SYSTEM_BOOT_BIOS, start_option) == 0) {
        *boot_set = FORCE_BIOS_SETUP;
    } else if (g_strcmp0(RF_SYSTEM_BOOT_FLOPPY, start_option) == 0) {
        *boot_set = FORCE_FLOPPY_REMOVABLE_MEDIA;
    } else {
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL gchar *rfsystem_powerstate_to_string(guchar powerstatel_byte)
{
    if (1 == powerstatel_byte) {
        return "On";
    } else if (0 == powerstatel_byte) {
        return "Off";
    } else {
        return NULL;
    }
}


LOCAL gchar *rfsystem_powerstate_to_system_state(guchar powerstatel_byte)
{
    if (1 == powerstatel_byte) {
        
        return "Enabled";
        
    } else if (0 == powerstatel_byte) {
        return "Disabled";
    } else {
        return "\0";
    }
}


LOCAL gchar *rfsystem_to_system_trust_state(guchar truststate_byte)
{
    if (1 == truststate_byte) {
        return "Enabled";
    } else if (0 == truststate_byte) {
        return "Absent";
    } else {
        return "\0";
    }
}


LOCAL guint32 rfsystem_memory_to_intgb(gchar *memory, guint32 len)
{
    guint32 num = 0;

    if (NULL == memory || len < 2) {
        return num;
    }

    if (*(memory + len - 1) == 'G' || *(memory + len - 1) == 'g' || *(memory + len - 2) == 'G' ||
        *(memory + len - 2) == 'g') {
        if (redfish_strip_to_int(memory, len) != VOS_OK) {
            return num;
        }

        if (vos_str2int(memory, 10, &num, NUM_TPYE_UINT32)) {
            return 0;
        }
    } else if (*(memory + len - 1) == 'M' || *(memory + len - 1) == 'm' || *(memory + len - 2) == 'M' ||
        *(memory + len - 2) == 'm') {
        if (redfish_strip_to_int(memory, len) != VOS_OK) {
            return num;
        }

        if (vos_str2int(memory, 10, &num, NUM_TPYE_UINT32)) {
            return 0;
        }

        num = num / 1024;
    }

    return num;
}


LOCAL void bios_add_supported_start_option(OBJ_HANDLE obj_handle, json_object *object)
{
    guint32 supported_option = 0;
    if (dal_get_property_value_uint32(obj_handle, PROPERTY_BIOS_SUPPORTED_START_OPTION, &supported_option) != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios.SupportedStartOption failed", __FUNCTION__);
        return;
    }
    const gchar *start_option_list[] = {
        RF_SYSTEM_BOOT_NONE, RF_SYSTEM_BOOT_PXE, RF_SYSTEM_BOOT_HDD, NA_STR, NA_STR, // 0, 1, 2, 3, 4
        RF_SYSTEM_BOOT_CD, RF_SYSTEM_BOOT_BIOS, // 5, 6
        NA_STR, NA_STR, NA_STR, NA_STR, NA_STR, NA_STR, NA_STR, NA_STR, RF_SYSTEM_BOOT_FLOPPY // 7~15
    };
    json_object *obj_jso = json_object_new_array();
    for (gsize i = 0; i < G_N_ELEMENTS(start_option_list); i++) {
        if (((guint32)0x1 << i) & supported_option) {
            json_object_array_add(obj_jso, json_object_new_string(start_option_list[i]));
        }
    }
    json_object_object_add(object, RFPROP_SYSTEM_BOOT_TARGET_ALLOW, obj_jso);
}


LOCAL PROPERTY_PROVIDER_S *rfsystem_get_self_provider(PROVIDER_PARAS_S *i_paras)
{
    return_val_if_fail((NULL != i_paras) && (i_paras->index >= 0) &&
        (i_paras->index < (gint32)(sizeof(g_system_provider) / sizeof(PROPERTY_PROVIDER_S))),
        (PROPERTY_PROVIDER_S *)NULL);
    return &g_system_provider[i_paras->index];
}


LOCAL gint32 rfsystem_get_property_string(PROPERTY_PROVIDER_S *self, PROVIDER_PARAS_S *i_paras,
    json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    gchar value_string[PROP_VAL_MAX_LENGTH + 1] = {0};

    
    if (NULL == self || NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(self->pme_class_name, 0, &obj_handle);
    
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, self->pme_prop_name, value_string, sizeof(value_string));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    if (rf_string_check(value_string) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string(value_string);

    return HTTP_OK;
}



LOCAL gint32 rfsystem_boot_set_startoption(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    const gchar *start_option_p, OBJ_HANDLE obj_handle)
{
    guchar bootset = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    json_object *message_temp_jso = NULL;
    gchar respond_value[RF_SYSTEM_RESPOND_MAXLEN] = {0};
    const gchar *target = "Boot/BootSourceOverrideTarget";

    if (NULL == i_paras || NULL == o_message_jso || NULL == start_option_p) {
        return VOS_ERR;
    }

    gint32 ret = rfsystem_startoption_to_uchar(start_option_p, &bootset);
    if (ret != VOS_OK) {
        (void)memset_s(respond_value, RF_SYSTEM_RESPOND_MAXLEN, 0, RF_SYSTEM_RESPOND_MAXLEN);
        errno_t safe_fun_ret = memcpy_s(respond_value, sizeof(respond_value), target, strlen(target));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, respond_value, &message_temp_jso, start_option_p,
            respond_value);
        (void)json_object_array_add(o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(bootset));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_NAME_BIOS, METHOD_BIOS_BOOTOPTION, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
            &output_list);
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);

        
        switch (ret) {
            case VOS_OK:
            case RFERR_SUCCESS:
                return HTTP_OK;

            case BIOS_ERROR_INVALID_INPUT_PARA:
                (void)create_message_info(MSGID_PROP_NOT_IN_LIST, respond_value, &message_temp_jso, start_option_p,
                    respond_value);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_BAD_REQUEST;

            case RFERR_INSUFFICIENT_PRIVILEGE: 
                (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, target, &message_temp_jso, target);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_FORBIDDEN;

            default: 
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
}



LOCAL gint32 rfsystem_boot_set_start_flag(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso,
    const gchar *start_flag_p, OBJ_HANDLE obj_handle)
{
    errno_t safe_fun_ret = EOK;
    gint32 ret;
    guchar stat_flag = 0;
    GSList *input_list = NULL;
    GSList *output_list = NULL;

    json_object *message_temp_jso = NULL;
    gchar respond_value[RF_SYSTEM_RESPOND_MAXLEN] = {0};
    const gchar *enable = "Boot/BootSourceOverrideEnabled";

    if (NULL == i_paras || NULL == o_message_jso || NULL == start_flag_p) {
        return VOS_ERR;
    }

    ret = rfsystem_start_flag_to_uchar(start_flag_p, &stat_flag);
    if (ret != VOS_OK) {
        (void)memset_s(respond_value, RF_SYSTEM_RESPOND_MAXLEN, 0, RF_SYSTEM_RESPOND_MAXLEN);
        safe_fun_ret = memcpy_s(respond_value, sizeof(respond_value), enable, strlen(enable));
        do_val_if_expr(safe_fun_ret != EOK,
            debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, respond_value, &message_temp_jso, start_flag_p,
            respond_value);
        (void)json_object_array_add(o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(stat_flag));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_NAME_BIOS, METHOD_BIOS_BOOTOPTION_FLAG, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
            &output_list);
        uip_free_gvariant_list(input_list);
        uip_free_gvariant_list(output_list);

        
        switch (ret) {
            case VOS_OK:
            case RFERR_SUCCESS:
                return HTTP_OK;

            case RFERR_INSUFFICIENT_PRIVILEGE: 
                (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, enable, &message_temp_jso, enable);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_FORBIDDEN;

            default: 
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
}


LOCAL gint32 rfsystem_set_boot_mode(PROVIDER_PARAS_S *i_paras, json_object *o_message_jso, const gchar *boot_mode_p,
    OBJ_HANDLE obj_handle)
{
    gint32 ret;
    guchar boot_mode = 0;
    GSList *input_list = NULL;
    json_object *message_temp_jso = NULL;
    const gchar *mode = "Boot/BootSourceOverrideMode";

    if (NULL == i_paras || NULL == o_message_jso || NULL == boot_mode_p) {
        return VOS_ERR;
    }

    ret = rfsystem_bootmode_to_uchar(boot_mode_p, &boot_mode);
    if (ret != VOS_OK) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, mode, &message_temp_jso, boot_mode_p, mode);
        (void)json_object_array_add(o_message_jso, message_temp_jso);
        return HTTP_BAD_REQUEST;
    } else {
        input_list = g_slist_append(input_list, g_variant_new_byte(boot_mode));

        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_NAME_BIOS, METHOD_SET_BIOS_BOOT_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
        uip_free_gvariant_list(input_list);

        switch (ret) {
            case VOS_OK:
                return HTTP_OK;

            case RFERR_INSUFFICIENT_PRIVILEGE: 
                (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, mode, &message_temp_jso, mode);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_FORBIDDEN;

            default: 
                (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_temp_jso);
                (void)json_object_array_add(o_message_jso, message_temp_jso);
                return HTTP_INTERNAL_SERVER_ERROR;
        }
    }
}


LOCAL gint32 get_system_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH] = {0};
    gchar slot[MAX_RSC_NAME_LEN] = {0};

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(VOS_ERR == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:redfish_get_board_slot fail", __FUNCTION__));

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYSTEM, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s:json_object_new_string fail", __FUNCTION__));
    

    return HTTP_OK;
}


LOCAL gint32 get_customized_asset_tag_by_cmcc(struct json_object **asset)
{
    gint32 ret;
    gchar serial_number[RF_SYSTEM_VALUE_MAXLEN] = {0};
    gchar asset_tag[RF_SYSTEM_VALUE_MAXLEN] = {0};
    OBJ_HANDLE elabel_handle = 0;
    OBJ_HANDLE fru_handle = 0;
 
    
    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, 0, &fru_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_get_specific_object_byte(%s) failed: ret %d.", CLASS_FRU, ret);
        return RET_ERR;
    }
    
    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &elabel_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dfl_get_referenced_object(%s) handle failed: ret %d.", PROPERTY_FRU_ELABEL_RO, ret);
        return RET_ERR;
    }

    
    ret = dal_get_property_value_string(elabel_handle,
        PROPERTY_ELABEL_PRODUCT_ASSET_TAG, asset_tag, RF_SYSTEM_VALUE_MAXLEN);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string %s failed, ret %d.",
            PROPERTY_ELABEL_PRODUCT_ASSET_TAG, ret);
        return RET_ERR;
    }

    if (strlen(asset_tag) != 0) {
        *asset = json_object_new_string(asset_tag);
        return RET_OK;
    }

    
    ret = dal_get_property_value_string(elabel_handle,
        PROPERTY_ELABEL_PRODUCT_SN, serial_number, RF_SYSTEM_VALUE_MAXLEN);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "dal_get_property_value_string %s failed: ret %d.", PROPERTY_ELABEL_PRODUCT_SN, ret);
        return RET_ERR;
    }

    
    if (rf_string_check(serial_number) != VOS_OK) {
        debug_log(DLOG_DEBUG, "serial_number %s check error", serial_number);
        *asset = json_object_new_string(asset_tag);
        return RET_OK;
    }
    *asset = json_object_new_string(serial_number);
    return RET_OK;
}

LOCAL gint32 get_system_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32  ret;
    gboolean is_customized = FALSE;

    is_customized = dal_is_customized_by_cmcc();
    debug_log(DLOG_DEBUG, " get_system_assettag is_customized = %d", is_customized);

    if (!is_customized) {
        return rfsystem_get_property_string(rfsystem_get_self_provider(i_paras), i_paras, o_message_jso, o_result_jso);
    }
    ret = get_customized_asset_tag_by_cmcc(o_result_jso);
    if (ret != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    return HTTP_OK;
}


LOCAL gint32 get_system_manufacturer(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return rfsystem_get_property_string(rfsystem_get_self_provider(i_paras), i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_model(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return rfsystem_get_property_string(rfsystem_get_self_provider(i_paras), i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_serialnumber(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return rfsystem_get_property_string(rfsystem_get_self_provider(i_paras), i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_uuid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return redfish_get_uuid(i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_hostingrole(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    guchar board_type = 0;
    json_object *obj_json = NULL;
    guchar x86_enable = DISABLE;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_array fail.\n", __FUNCTION__, __LINE__));

    (void)redfish_get_x86_enable_type(&x86_enable);

    if (BOARD_SWITCH == board_type) {
        obj_json = json_object_new_string(RF_SYSTEM_HOSTINGROLE_BACK);
        return_val_do_info_if_fail(NULL != obj_json, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail.\n", __FUNCTION__, __LINE__));
    } else if (DISABLE != x86_enable) {
        obj_json = json_object_new_string(RF_SYSTEM_HOSTINGROLE_NO_BACK);
        return_val_do_info_if_fail(NULL != obj_json, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: json_object_new_string fail.\n", __FUNCTION__, __LINE__));
    } else {
        return HTTP_OK;
    }

    ret = json_object_array_add(*o_result_jso, obj_json);
    do_val_if_expr(0 != ret, json_object_put(obj_json));

    return HTTP_OK;
}


LOCAL gint32 get_system_status(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guchar health = 0;
    guchar powerstatel_byte = 0;
    gchar *rf_state = NULL;
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    gint32 ret = dal_get_object_handle_nth(CLASS_WARNING, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_WARNING_HEALTH_STATUS, &health);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));

    if (BOARD_SWITCH != board_type) {
        
        ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, &powerstatel_byte);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    } else {
        ret = rfsystem_get_switch_plane_powerstatus(&powerstatel_byte);
        do_val_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, %d: rfsystem_get_switch_plane_powerstatus fail.\n", __FUNCTION__, __LINE__));
    }

    
    
    rf_state = rfsystem_powerstate_to_system_state(powerstatel_byte);

    
    get_resource_status_property(&health, NULL, rf_state, o_result_jso, FALSE);

    return HTTP_OK;
}


LOCAL void get_system_health_led_state(json_object* o_result_jso)
{
    OBJ_HANDLE led_handle;
    guint8 led_state;
    json_object* led_jso = NULL;

    
    gint32 ret = dfl_get_object_handle(SYSTEM_HEALTHY_LED, &led_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_DEBUG, "cann't get object handle for %s", SYSTEM_HEALTHY_LED);
        return;
    }
    
    
    ret = dal_get_property_value_byte(led_handle, STATE, &led_state);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get led state value failed", __FUNCTION__);
        return;
    }

    if (led_state == SYSTEM_HEALTH_LED_OFF_VALUE) {
        led_jso = json_object_new_string(RF_LED_OFF);
    } else if (led_state == SYSTEM_HEALTH_LED_LIT_VALUE) {
        led_jso = json_object_new_string(RF_LED_LIT);
    } else {
        led_jso = json_object_new_string(RF_LED_BLINKING);
    }
    ret = json_object_object_add(o_result_jso, RFPROP_SYSTEM_HEALTH_LED_STATE, led_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: add HealthLedState resource failed", __FUNCTION__);
    }
    return;
}


gint32 rf_get_system_powerstate(guchar *power_state)
{
    guchar board_type = 0;
    OBJ_HANDLE obj_handle = 0;

    return_val_if_expr(power_state == NULL, VOS_ERR);
    gint32 ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));

    if (BOARD_SWITCH == board_type) {
        ret = rfsystem_get_switch_plane_powerstatus(power_state);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_DEBUG, "%s, %d: rfsystem_get_switch_plane_powerstatus fail.\n", __FUNCTION__, __LINE__));
    } else {
        
        ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_CHASSPWR_STATE, power_state);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));
    }

    return VOS_OK;
}


LOCAL gint32 get_system_powerstate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar powerstatel_byte = 0;
    gchar *powerstatel_p = NULL;

    
    if (NULL == o_result_jso || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    gint32 ret = rf_get_system_powerstate(&powerstatel_byte);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: call rf_get_system_powerstate fail", __FUNCTION__, __LINE__));

    
    powerstatel_p = rfsystem_powerstate_to_string(powerstatel_byte);
    return_val_do_info_if_fail(powerstatel_p != NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s, %d: fail.\n", __FUNCTION__, __LINE__));

    
    *o_result_jso = json_object_new_string(powerstatel_p);

    return HTTP_OK;
}


LOCAL guint8 rfsystem_get_switch_plane_powerstatus(guchar *power_state)
{
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    guchar i;

    for (i = BASE_FRU_ID; i <= FC_FRU_ID; i++) {
        ret_val = dal_get_specific_object_byte(CLASS_FRUPAYLOAD, PROPERTY_FRU_PAYLOAD_FRUID, i, &obj_handle);
        break_if_expr(VOS_OK == ret_val);
    }

    return_val_do_info_if_fail(0 != obj_handle, VOS_ERR, *power_state = INVALID_VAL;
        debug_log(DLOG_DEBUG, "invalid obj_handle"));

    ret_val = dal_get_property_value_byte(obj_handle, PROPERTY_FRUPAYLOAD_POWERSTATE, power_state);
    return_val_do_info_if_fail(VOS_OK == ret_val, VOS_ERR,
        debug_log(DLOG_ERROR, "Get payload power state failed: %d.\n", ret_val));

    return VOS_OK;
}


LOCAL gint32 get_system_partnum(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return rfsystem_get_property_string(rfsystem_get_self_provider(i_paras), i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 get_system_biosversion(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    gchar version_string[RF_SYSTEM_BOSVERSION_MAXLEN];
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    // 某些机型不展示bios版本，如Atlas800DG1
    if (!is_mainboard_bios_supported()) {
        return HTTP_OK;
    }
    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_string(obj_handle, BMC_BIOS_VER_NAME, version_string, RF_SYSTEM_BOSVERSION_MAXLEN);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    if (rf_string_check(version_string) != VOS_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string(version_string);

    return HTTP_OK;
}


LOCAL gint32 get_status_for_count_is_zero(json_object **status_obj, gboolean in_oem)
{
    json_object *huawei = NULL;
    json_object *oem = NULL;

    *status_obj = json_object_new_object();
    return_val_do_info_if_expr(*status_obj == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d: new object fail.", __FUNCTION__, __LINE__));

    if (in_oem) {
        json_object_object_add(*status_obj, RFPROP_SEL_SEVERITY, NULL);
    } else {
        huawei = json_object_new_object();
        return_val_do_info_if_expr(huawei == NULL, VOS_ERR, json_object_put(*status_obj);
            debug_log(DLOG_ERROR, "%s, %d: new object object fail.", __FUNCTION__, __LINE__));
        oem = json_object_new_object();
        return_val_do_info_if_expr(oem == NULL, VOS_ERR, json_object_put(*status_obj); json_object_put(huawei);
            debug_log(DLOG_ERROR, "%s, %d: new oem object fail.", __FUNCTION__, __LINE__));
        json_object_object_add(huawei, RFPROP_SEL_SEVERITY, NULL);
        json_object_object_add(oem, RFPROP_COMMON_HUAWEI, huawei);
        json_object_object_add(*status_obj, RFPROP_COMMON_OEM, oem);
    }

    json_object_object_add(*status_obj, RFPROP_HEALTH_ROLLUP, json_object_new_string(LOGENTRY_SEVERITY_OK));
    return VOS_OK;
}


LOCAL gint32 get_system_processorsummary(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 count = 0;
    guchar cpu_id = 0;
    guchar cpu_presence = 0;
    guint8 cpu_invisible = 0;
    guchar cpu_health = 0;
    guchar cpu_health_temp = 0;
    gchar  cpu_mode_string[RF_SYSTEM_CPU_ID_MAXLEN] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *status_obj_jso = NULL;
    guchar board_type = 0;
    guchar power_state = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: processorsummary get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_list(CLASS_CPU, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        cpu_id = 0;
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
        do_if_expr((ret != RET_OK) || (cpu_id == 0), continue);
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_INVISIBLE, &cpu_invisible);
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_PRESENCE, &cpu_presence);
        do_if_expr((ret != RET_OK) || (cpu_presence == 0) || (cpu_invisible == 1), continue);

        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_HEALTH, &cpu_health_temp);

        if (obj_node == obj_list) {
            (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_MODEL, cpu_mode_string,
                RF_SYSTEM_CPU_ID_MAXLEN);
        }

        count++;
        cpu_health = cpu_health >= cpu_health_temp ? cpu_health : cpu_health_temp;
    }

    
    ret = rf_get_system_powerstate(&power_state);
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s, %d: gey system power state fail", __FUNCTION__, __LINE__));
    // power_state取值为表示上电
    do_val_if_expr(power_state != 1, (cpu_health = INVALID_DATA_BYTE));

    
    ret = rf_string_check(cpu_mode_string);

    
    *o_result_jso = json_object_new_object();
    guint8 in_oem = i_paras->is_from_webui ? TRUE : FALSE;
    
    if (count > 0) {
        get_resource_status_property(NULL, &cpu_health, NULL, &status_obj_jso, in_oem);
    } else {
        (void)get_status_for_count_is_zero(&status_obj_jso, in_oem);
    }
    
    json_object_object_add(*o_result_jso, RFPROP_COMMON_COUNT, json_object_new_int(count));
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_PROCESSOR_MODE,
        ret == VOS_OK ? json_object_new_string(cpu_mode_string) : NULL);
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    g_slist_free(obj_list);

    return HTTP_OK;
}


LOCAL gint32 get_system_memorysummary(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 total_memory = 0;
    guchar mem_health = 0;
    guchar mem_health_temp = 0;
    gchar  memory[20] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    json_object *status_obj_jso = NULL;
    guchar board_type = 0;
    guchar mem_presence = 0;
    gint32 count = 0;
    guchar power_state = 0;
    guint8 in_oem;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: memorysummary get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dfl_get_object_list(CLASS_MEMORY, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_PRESENCE, &mem_presence);
        continue_if_expr(mem_presence == 0);
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_CAPACITY, memory, sizeof(memory));
        total_memory = total_memory + rfsystem_memory_to_intgb(memory, strlen(memory));
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_MEM_HEALTH, &mem_health_temp);
        mem_health = mem_health >= mem_health_temp ? mem_health : mem_health_temp;
        count++;
    }

    
    ret = rf_get_system_powerstate(&power_state);
    do_val_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s, %d: gey system power state fail", __FUNCTION__, __LINE__));
    // power_state取值为表示上电
    do_val_if_expr(power_state != 1, (mem_health = INVALID_DATA_BYTE));

    
    
    *o_result_jso = json_object_new_object();
    in_oem = i_paras->is_from_webui ? TRUE : FALSE;
    
    if (count > 0) {
        get_resource_status_property(NULL, &mem_health, NULL, &status_obj_jso, in_oem);
    } else {
        (void)get_status_for_count_is_zero(&status_obj_jso, in_oem);
    }
    
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_MEMORY_TOTAL, json_object_new_int(total_memory));
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    g_slist_free(obj_list);

    return HTTP_OK;
}


LOCAL gint32 get_system_boot(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guchar boot_mode = 0;
    guchar start_option = 0;
    guchar start_flag = 0;
    gchar *boot_mode_p = NULL;
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get o_obj_handle fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_NAME, &start_option);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    ret = dal_get_property_value_byte(obj_handle, BIOS_START_OPTION_FLAG_NAME, &start_flag);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get property_data fail.\n", __FUNCTION__, __LINE__));

    
    
    
    
    ret = dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE, &boot_mode);
    do_if_true((DFL_OK == ret), (boot_mode_p = rfsystem_bootmode_to_string(boot_mode)));
    
    

    
    gchar *start_flag_p = rfsystem_startenable_to_string(start_flag);
    gchar *start_option_p = rfsystem_startoption_to_string(start_option);

    
    *o_result_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_BOOT_TARGET,
        start_option_p == NULL ? NULL : json_object_new_string(start_option_p));
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_BOOT_ENABLED,
        start_flag_p == NULL ? NULL : json_object_new_string(start_flag_p));
    json_object_object_add(*o_result_jso, RFPROP_SYSTEM_BOOT_MODE,
        boot_mode_p == NULL ? NULL : json_object_new_string(boot_mode_p));
    bios_add_supported_start_option(obj_handle, *o_result_jso);
    
    return HTTP_OK;
}


LOCAL gint32 get_system_ethernetinterfaces(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RF_RESOURCE_ETHERNETINTERFACES, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_processors(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_PROCESSORS, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_memory(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_MEMORY, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_networkinterfaces(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_INTERFACES, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_system_storage(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_STORAGES, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 get_system_bios(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    guchar board_type = 0;
    guint32 product_ver = 0;
    guchar arm_enable = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);
    

    // 非v5单板不显示bios导航
    ret = dal_get_product_version_num(&product_ver);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: dal_get_product_version_num fail.\n", __FUNCTION__, __LINE__));

    
    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
    // arm产品支持通过redfish配置bios setup菜单
    return_val_if_expr(((DISABLE == arm_enable) && (PRODUCT_VERSION_V5 > product_ver)), HTTP_INTERNAL_SERVER_ERROR);
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_BIOS, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}

LOCAL gint32 get_chassis_pciedevice(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: json object new array fail.\n", __FUNCTION__, __LINE__));

    (void)redfish_get_chassis_pciedevice_odata_id(*o_result_jso, CLASS_PCIECARD_NAME);
    (void)redfish_get_chassis_pciedevice_odata_id(*o_result_jso, CLASS_OCP_CARD);
    return HTTP_OK;
}

LOCAL void get_chassis_pciefunctions_by_cardtype(const char *card_type, json_object *o_result_jso)
{
    GSList *obj_list = NULL;
    guint8 pcie_presence = 0;
    OBJ_HANDLE chassis_comp_handle = 0;

    
    gint32 ret = dfl_get_object_list(card_type, &obj_list);
    
    if (ret != VOS_OK) {
        debug_log(DLOG_INFO, "%s, %d: get obj lsit fail, card_type(%s)", __FUNCTION__, __LINE__, card_type);
        return;
    }

    for (GSList *obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = rf_get_object_location_handle(0, (OBJ_HANDLE)obj_node->data, &chassis_comp_handle);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "cann't get location handle for %s", dfl_get_object_name((OBJ_HANDLE)obj_node->data));
            g_slist_free(obj_list);
            return;
        }
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPETRY_PCIECARD_PRESENCE, &pcie_presence);
        if (ret != VOS_OK || pcie_presence == 0) {
            continue;
        }

        (void)get_chassis_pciedevices_function_odata_id(chassis_comp_handle, (OBJ_HANDLE)obj_node->data, o_result_jso);
    }
    g_slist_free(obj_list);

    return;
}


LOCAL gint32 get_chassis_pciefunctions(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_MASS, "%s, %d: json object new array fail.\n", __FUNCTION__, __LINE__));
    get_chassis_pciefunctions_by_cardtype(CLASS_PCIECARD_NAME, *o_result_jso);
    get_chassis_pciefunctions_by_cardtype(CLASS_OCP_CARD, *o_result_jso);
    return HTTP_OK;
}

LOCAL gint32 get_system_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gchar slot_str[MAX_RSC_ID_LEN] = {0};

    
    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != o_result_jso) && (NULL != i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error", __FUNCTION__));
    

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = redfish_get_board_slot(slot_str, sizeof(slot_str));
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    *o_result_jso = json_object_new_string(slot_str);
    do_if_expr(*o_result_jso == NULL, return HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL gint32 set_system_boot(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 ret_http = 0;

    OBJ_HANDLE obj_handle = 0;
    const gchar *boot_mode_p = NULL;
    const gchar *start_option_p = NULL;
    const gchar *start_startflag_p = NULL;
    json_object *obj_json_option = NULL;
    json_object *obj_json_enable = NULL;
    json_object *obj_json_mode = NULL;
    json_object *message_temp_jso = NULL;
    const gchar *mode = "Boot/BootSourceOverrideMode";
    const gchar *enable = "Boot/BootSourceOverrideEnabled";
    const gchar *target = "Boot/BootSourceOverrideTarget";
    guchar board_type = 0;
    guint32 product_num = 0;
    guchar arm_enable = 0;
    guint8 boot_mode_support_flag = 0;

    
    return_val_do_info_if_expr((o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    
    ret = dal_get_object_handle_nth(g_system_provider[i_paras->index].pme_class_name, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    *o_message_jso = json_object_new_array();

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_BOOT_MODE, &obj_json_mode) == TRUE) {
        
        
        ret = dal_get_product_version_num(&product_num);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
        
        ret = redfish_get_x86_enable_type(&board_type);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
        
        (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enable);
        (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &boot_mode_support_flag);

        // arm产品支持通过redfish配置bios setup菜单
        if (board_type == DISABLE || ((arm_enable == DISABLE) && (product_num < PRODUCT_VERSION_V5)) ||
            boot_mode_support_flag != ENABLE) {
            
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, mode, &message_temp_jso, mode);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            message_temp_jso = NULL;
            ret_http = HTTP_NOT_IMPLEMENTED;
        } else if (json_type_string != json_object_get_type(obj_json_mode)) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, mode, &message_temp_jso,
                obj_json_mode == NULL ? RF_VALUE_NULL : dal_json_object_get_str(obj_json_mode), mode);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_http = HTTP_BAD_REQUEST;
        } else {
            boot_mode_p = dal_json_object_get_str(obj_json_mode);
            ret_http = rfsystem_set_boot_mode(i_paras, *o_message_jso, boot_mode_p, obj_handle);
        }

        
    }

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_BOOT_ENABLED, &obj_json_enable) == TRUE) {
        
        ret = redfish_get_x86_enable_type(&board_type);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));

        if (DISABLE == board_type) {
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, enable, &message_temp_jso, enable);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            message_temp_jso = NULL;
            ret_http = HTTP_NOT_IMPLEMENTED;
        }
        
        else if (json_type_string != json_object_get_type(obj_json_enable)) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, enable, &message_temp_jso,
                obj_json_enable == NULL ? RF_VALUE_NULL : dal_json_object_get_str(obj_json_enable), enable);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_http = HTTP_BAD_REQUEST;
        } else {
            start_startflag_p = dal_json_object_get_str(obj_json_enable);
            ret_http = rfsystem_boot_set_start_flag(i_paras, *o_message_jso, start_startflag_p, obj_handle);
        }
    }

    
    if (json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_BOOT_TARGET, &obj_json_option) == TRUE) {
        
        ret = redfish_get_x86_enable_type(&board_type);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));

        if (DISABLE == board_type) {
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, target, &message_temp_jso, target);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_http = HTTP_NOT_IMPLEMENTED;
        }
        
        else if (json_type_string != json_object_get_type(obj_json_option)) {
            (void)create_message_info(MSGID_PROP_TYPE_ERR, target, &message_temp_jso,
                obj_json_option == NULL ? RF_VALUE_NULL : dal_json_object_get_str(obj_json_option), target);
            (void)json_object_array_add(*o_message_jso, message_temp_jso);
            ret_http = HTTP_BAD_REQUEST;
        } else {
            start_option_p = dal_json_object_get_str(obj_json_option);
            ret = rfsystem_boot_set_startoption(i_paras, *o_message_jso, start_option_p, obj_handle);
            ret_http = ret_http == HTTP_OK ? HTTP_OK : ret;
        }
    }

    return ret_http;
}


gint32 set_fru_product_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, guchar fru_id,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const gchar *assettag_p = NULL;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE fru_handle = 0;

    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_BASICSETTING, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSTEM_ASSETTAG, o_message_jso,
        RFPROP_SYSTEM_ASSETTAG));

    ret = dal_get_specific_object_byte(CLASS_FRU, PROPERTY_FRU_ID, fru_id, &fru_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        debug_log(DLOG_ERROR, "get object handle with [%s:%d] for class [%s] failed, ret is %d", PROPERTY_FRU_ID,
        fru_id, CLASS_FRU, ret));

    
    ret = dfl_get_referenced_object(fru_handle, PROPERTY_FRU_ELABEL_RO, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, RFPROP_ASSET_TAG, o_message_jso, RFPROP_ASSET_TAG));

    assettag_p = (i_paras->val_jso == NULL ? "\0" : dal_json_object_get_str(i_paras->val_jso));
    if (NULL == assettag_p || strlen(assettag_p) > FRU_MAX_ELABEL_LEN) {
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_SYSTEM_ASSETTAG, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_SYSTEM_ASSETTAG);
        return HTTP_BAD_REQUEST;
    }

    input_list = g_slist_append(input_list, g_variant_new_string(assettag_p));
    input_list = g_slist_append(input_list, g_variant_new_byte(fru_id));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_ELABEL, METHOD_SET_PRODUCT_ASSETTAG, AUTH_ENABLE, i_paras->user_role_privilege, input_list, &output_list);
    debug_log(DLOG_INFO, "%s:set property value end, ret is %d", __FUNCTION__, ret);

    uip_free_gvariant_list(input_list);
    uip_free_gvariant_list(output_list);

    
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SYSTEM_ASSETTAG, o_message_jso,
                RFPROP_SYSTEM_ASSETTAG);
            return HTTP_FORBIDDEN;

        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s failed, ret is %d", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 set_system_assettag(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return set_fru_product_assettag(i_paras, o_message_jso, 0, o_result_jso);
}


LOCAL gint32 do_system_reset(guchar from_webui_flag, const gchar *i_username, const gchar *i_client, guint8 i_userpriv,
    const gchar *i_reset_type)
{
    gint32 ret;

    return_val_do_info_if_fail(NULL != i_reset_type, VOS_ERR,
        debug_log(DLOG_DEBUG, "[%s]: param check err", __FUNCTION__));
    debug_log(DLOG_DEBUG, "[%s]: %s  \"ResetType\": \"%s\"", __FUNCTION__, ">>>>>>>>System reset<<<<<<<<<",
        i_reset_type);

    
    if (VOS_OK == g_strcmp0(RF_ACTION_ON, i_reset_type)) {
        ret = uip_call_class_method_redfish(from_webui_flag, i_username, i_client, OBJ_HANDLE_NULL, CLASS_PAYLOAD,
            METHOD_PWR_ON, AUTH_ENABLE, i_userpriv, NULL, NULL);
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FORCEOFF, i_reset_type)) {
        ret = uip_call_class_method_redfish(from_webui_flag, i_username, i_client, OBJ_HANDLE_NULL, CLASS_PAYLOAD,
            METHOD_FORCE_PWR_OFF, AUTH_ENABLE, i_userpriv, NULL, NULL);
    } else if (VOS_OK == g_strcmp0(RF_ACTION_GRACEFULSHUTDOWN, i_reset_type)) {
        ret = uip_call_class_method_redfish(from_webui_flag, i_username, i_client, OBJ_HANDLE_NULL, CLASS_PAYLOAD,
            METHOD_PWR_OFF, AUTH_ENABLE, i_userpriv, NULL, NULL);
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FORCERESTART, i_reset_type)) {
        ret = uip_call_class_method_redfish(from_webui_flag, i_username, i_client, OBJ_HANDLE_NULL, CLASS_FRUPAYLOAD,
            METHOD_COLD_RESET, AUTH_ENABLE, i_userpriv, NULL, NULL);
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FORCE_POWER_CYCLE, i_reset_type)) {
        ret = uip_call_class_method_redfish(from_webui_flag, i_username, i_client, OBJ_HANDLE_NULL, CLASS_FRUPAYLOAD,
            METHOD_GRACE_REBOOT, AUTH_ENABLE, i_userpriv, NULL, NULL);
    } else if (VOS_OK == g_strcmp0(RF_ACTION_NMI, i_reset_type)) {
        ret = uip_call_class_method_redfish(from_webui_flag, i_username, i_client, OBJ_HANDLE_NULL, CLASS_PAYLOAD,
            METHOD_DIAG_INTR, AUTH_ENABLE, i_userpriv, NULL, NULL);
    } else {
        ret = HTTP_INTERNAL_SERVER_ERROR;
    }
    

    do_info_if_true(VOS_OK != ret, debug_log(DLOG_DEBUG, "%s fail , ret: %d  ", __FUNCTION__, ret));

    return ret;
}


LOCAL gint32 act_system_reset(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    struct json_object *jso_allowable_values = NULL;
    const gchar *str_value = NULL;

    guchar board_type = 0;

    debug_log(DLOG_DEBUG, "[%s]: act_system_reset", __FUNCTION__);

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    debug_log(DLOG_DEBUG, "[%s]: param check ok", __FUNCTION__);

    
    ret = redfish_get_support_sys_power_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));

    if (board_type != ENABLE) {
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, RFPROP_SYSTEM_RESET, o_message_jso, RFPROP_SYSTEM_RESET);
        return HTTP_NOT_IMPLEMENTED;
    }

    

    jso_allowable_values = json_tokener_parse(system_allowable_values);

    ret = redfish_action_format_check(RFPROP_SYSTEM_RESET, jso_allowable_values, i_paras->val_jso, o_message_jso);

    json_object_put(jso_allowable_values);

    return_val_if_fail((HTTP_OK == ret), ret);

    str_value = dal_json_object_get_str(ex_json_object_object_get_val(i_paras->val_jso));

    
    ret = do_system_reset(i_paras->is_from_webui, i_paras->user_name, i_paras->client, i_paras->user_role_privilege,
        str_value);
    // 成功
    return_val_do_info_if_expr((VOS_OK == ret), HTTP_OK, (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso));
    

    // 无权限
    return_val_do_info_if_expr((RFERR_INSUFFICIENT_PRIVILEGE == ret), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    return_val_do_info_if_expr((COMP_CODE_CMD_INVALID == ret), HTTP_METHOD_NOT_ALLOWED,
        (void)create_message_info(MSGID_RESET_FAILED_BY_BBU, NULL, o_message_jso, str_value));

    // BIOS正在升级
    
    return_val_do_info_if_expr((POWER_ON_FAILED_BY_BIOS_UP == ret), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_RESET_FAILED, NULL, o_message_jso, str_value));
    

    // VRD正在升级
    return_val_do_info_if_expr((POWER_ON_FAILED_BY_VRD_UP == ret), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_RESET_FAILED_BY_VRD, NULL, o_message_jso, str_value));

    
    debug_log(DLOG_ERROR, "Reset failed, error code :%#x", ret);
    (void)create_message_info(MSGID_RESET_NOT_ALLOWED, NULL, o_message_jso, str_value);

    return HTTP_BAD_REQUEST;
    
}


LOCAL guint8 redfish_frucontrol_choose(const gchar *i_control_type)
{
    if (VOS_OK == g_strcmp0(RF_ACTION_FRU_POWERON, i_control_type)) {
        return FRU_CONTROL_POWER_ON;
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FRU_POWEROFF, i_control_type)) {
        return FRU_CONTROL_POWER_OFF;
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FRU_POWERRESET, i_control_type)) {
        return FRU_CONTROL_COLD_RESET;
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FRU_POWERCYCLE, i_control_type)) {
        return FRU_CONTROL_GRACEFUL_REBOOT;
    } else if (VOS_OK == g_strcmp0(RF_ACTION_FRU_POWERDIAGRESET, i_control_type)) {
        return FRU_CONTROL_ISSUEDIAGNOSTICINTERRUPT;
    }

    return 0xff;
}


LOCAL gint32 parse_fru_control_result(gint32 ret_code, const gchar *action, const gchar *fruid,
    json_object **o_message_jso)
{
    switch (ret_code) {
        case COMP_CODE_SUCCESS:
            (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
            return HTTP_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
            return HTTP_FORBIDDEN;
        
        case COMP_CODE_CMD_INVALID:
            (void)create_message_info(MSGID_RESET_FAILED_BY_BBU, NULL, o_message_jso, action);
            return HTTP_METHOD_NOT_ALLOWED;
        // BIOS或CPLD正在升级
        case POWER_ON_FAILED_BY_BIOS_UP:
            (void)create_message_info(MSGID_ACT_FAILED_BY_BIOS_UPGRADE, NULL, o_message_jso, action);
            return HTTP_BAD_REQUEST;
        case POWER_ON_FAILED_BY_CPLD_UP:
            (void)create_message_info(MSGID_ACT_FAILED_BY_CPLD_UPGRADE, NULL, o_message_jso, action);
            return HTTP_BAD_REQUEST;
        case POWER_ON_FAILED_BY_VRD_UP:
            (void)create_message_info(MSGID_ACT_FAILED_BY_VRD_UPGRADE, NULL, o_message_jso, action);
            return HTTP_BAD_REQUEST;
        // 系统未上电
        case COMP_CODE_STATUS_INVALID:
            (void)create_message_info(MSGID_ACT_FAILED_BY_POWER_OFF, NULL, o_message_jso, action);
            return HTTP_BAD_REQUEST;
        // 不支持动作
        case COMP_CODE_UNSUPPORT:
            (void)create_message_info(MSGID_CMD_NOT_SUPPORTED, RFPROP_FRUCONTROL_TYPE, o_message_jso,
                RFPROP_FRUCONTROL_TYPE, fruid, action);
            return HTTP_NOT_IMPLEMENTED;
        // 系统忙
        case COMP_CODE_BUSY:
            (void)create_message_info(MSGID_RSC_IN_USE, NULL, o_message_jso);
            return HTTP_BAD_REQUEST;
        // Fru不存在
        case COMP_CODE_OUTOF_RANGE:
            (void)create_message_info(MSGID_FRU_NOT_EXIST, RFPROP_FRUID, o_message_jso, fruid);
            return HTTP_BAD_REQUEST;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 act_system_frucontrol(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;
    guchar result;
    json_object *obj_json = NULL;
    const gchar *actions = NULL;
    gint32 fru_id;
    gchar fruid[10] = {0};

    
    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_FRUID, &obj_json);
    return_val_do_info_if_expr(ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SYSTEM_FRUCTL, RFPROP_FRUID));

    
    
    return_val_do_info_if_expr(json_type_int != json_object_get_type(obj_json), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso,
            dal_json_object_to_json_string(obj_json), RFPROP_FRUID, RFPROP_SYSTEM_FRUCONTROL));
    
    fru_id = json_object_get_int(obj_json);

    
    return_val_do_info_if_expr(fru_id < 0 || fru_id > G_MAXUINT8, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_TYPE_ERR, NULL, o_message_jso,
            dal_json_object_to_json_string(obj_json), RFPROP_FRUID, RFPROP_SYSTEM_FRUCONTROL));
    

    obj_json = NULL;

    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_FRUCONTROL_TYPE, &obj_json);
    return_val_do_info_if_expr(ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SYSTEM_FRUCTL,
        RFPROP_FRUCONTROL_TYPE));
    actions = dal_json_object_get_str(obj_json);
    result = redfish_frucontrol_choose(actions);
    return_val_do_info_if_expr(0xff == result, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, RFPROP_FRUCONTROL_TYPE, o_message_jso, actions,
        RFPROP_FRUCONTROL_TYPE));

    input_list = g_slist_append(input_list, g_variant_new_byte(result));
    input_list = g_slist_append(input_list, g_variant_new_byte((guint8)fru_id));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_FRUPAYLOAD, METHOD_FRU_CONTROL, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    debug_log(DLOG_DEBUG, "%s, %d: %s,%d,%d .\n", __FUNCTION__, __LINE__, actions, fru_id, ret);

    uip_free_gvariant_list(input_list);
    (void)snprintf_s(fruid, sizeof(fruid), sizeof(fruid) - 1, "%d", fru_id);
    return parse_fru_control_result(ret, actions, (const gchar *)fruid, o_message_jso);
}


gboolean is_force_restart_enabled(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret =  dal_get_object_handle_nth(CLASS_NAME_STORAGE_ARB, 0, &obj_handle);
    if (ret != RET_OK) {
        return FALSE;
    }
    guint8 fso_enable = 0;
    dal_get_property_value_byte(obj_handle, PROPERTY_ARB_FORCE_RESTART_ENABLED, &fso_enable);
    if (fso_enable != 1) {
        return FALSE;
    }
    return TRUE;
}


LOCAL gint32 _parse_i_paras_swi_ipv6addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, GSList **input_list)
{
    json_object *obj_json = NULL;
    const gchar *fru_type = NULL;
    const gchar *ipv6_addr = NULL;
    guchar board_type = BOARD_OTHER;

    return_val_do_info_if_expr(input_list == NULL, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    
    gint32 ret = redfish_get_board_type(&board_type);
    return_val_do_info_if_expr((board_type != BOARD_SWITCH), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, o_message_jso, RFPROP_SYSTEM_ADD_SWI_IPV6ADDR);
        debug_log(DLOG_ERROR, "board type %d not supported, ret is %d !\r\n", board_type, ret));

    // 解析FruType字段
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_FRU_TYPE, &obj_json);
    return_val_do_info_if_expr(ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, NULL, o_message_jso, RFPROP_SYSTEM_FRU_TYPE));

    fru_type = dal_json_object_get_str(obj_json);
    obj_json = NULL;
    // 解析Address字段
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_IPV6ADDR, &obj_json);
    return_val_do_info_if_expr(ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SYSTEM_ADD_SWI_IPV6ADDR,
            RFPROP_SYSTEM_IPV6ADDR));
    ipv6_addr = dal_json_object_get_str(obj_json);
    obj_json = NULL;
    // 解析PrefixLength字段
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_IPV6PREFIX, &obj_json);
    return_val_do_info_if_expr(ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SYSTEM_ADD_SWI_IPV6ADDR,
            RFPROP_SYSTEM_IPV6PREFIX));
    gint32 ipv6_prefix = json_object_get_int(obj_json);

    // 将相关字段填入*input_lis，准备下发给switch模块的方法；
    *input_list = g_slist_append(*input_list, g_variant_new_string(fru_type));
    *input_list = g_slist_append(*input_list, g_variant_new_string(ipv6_addr));
    *input_list = g_slist_append(*input_list, g_variant_new_int32(ipv6_prefix));
    debug_log(DLOG_DEBUG, "%s: %s, %s, %d, %d \n", __FUNCTION__, fru_type, ipv6_addr, ipv6_prefix, ret);

    return HTTP_OK;
}


LOCAL gint32 create_swi_ipv6_return_message(json_object **o_message_jso, gint32 ret, const gchar *err_info)
{
    return_val_do_info_if_expr((o_message_jso == NULL) || (err_info == NULL), HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    debug_log(DLOG_DEBUG, "%s: err_info %s, ret %d\n", __FUNCTION__, err_info, ret);

    // 成功
    return_val_do_info_if_expr((ret == COMP_CODE_SUCCESS), HTTP_OK,
        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso));

    // 参数错误FruType
    return_val_do_info_if_expr((ret == SET_SWITCH_PLANE_PARA_INVALID), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_NOT_SUPPORTED, err_info, o_message_jso, RFPROP_SYSTEM_FRU_TYPE,
        err_info));

    // 参数错误IP地址错误；
    return_val_do_info_if_expr((ret == SET_SWITCH_PLANE_IP_FORMAT_ERR), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_IPV6ADDRESS, err_info, o_message_jso, RFPROP_SYSTEM_IPV6ADDR,
        err_info));

    // 参数错误前缀错误；
    return_val_do_info_if_expr((ret == SET_SWITCH_PLANE_MASK_FORMAT_ERR), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_SUBNETMASK, err_info, o_message_jso, RFPROP_SYSTEM_IPV6PREFIX,
        err_info));

    // 参数错误gateway错误；
    return_val_do_info_if_expr((ret == SET_SWITCH_PLANE_GW_FORMAT_ERR), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_IPV6GATEWAY, err_info, o_message_jso, RFPROP_SYSTEM_IPV6GATEWAY,
        err_info));
    // 无权限
    return_val_do_info_if_expr((ret == RFERR_INSUFFICIENT_PRIVILEGE), HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));
    // 交换板返回错误的响应
    if (ret == SWITCH_PLANE_RESPONE_ERR) {
        (void)create_message_info(MSGID_SWITCH_BOARD_SET_FAIL, NULL, o_message_jso);
        return HTTP_BAD_REQUEST;
    }
    // 内部错误
    (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
    return HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 act_system_add_swi_ipv6addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((o_message_jso == NULL) || (i_paras == NULL) || (i_paras->val_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    // 解析输入参数；
    ret = _parse_i_paras_swi_ipv6addr(i_paras, o_message_jso, &input_list);
    return_val_do_info_if_expr(ret != HTTP_OK, HTTP_BAD_REQUEST, uip_free_gvariant_list(input_list));

    // 发送消息调用设置函数；
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_ADD_IPV6_ADDR, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    // 处理发送消息的返回码；
    ret = create_swi_ipv6_return_message(o_message_jso, ret, RFPROP_SYSTEM_ADD_SWI_IPV6ADDR);
    return ret;
}


LOCAL gint32 act_system_del_swi_ipv6addr(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    GSList *input_list = NULL;

    return_val_do_info_if_expr((o_message_jso == NULL) || (i_paras == NULL) || (i_paras->val_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    // 解析输入参数；
    ret = _parse_i_paras_swi_ipv6addr(i_paras, o_message_jso, &input_list);
    return_val_do_info_if_expr(ret != HTTP_OK, HTTP_BAD_REQUEST, uip_free_gvariant_list(input_list));

    // 发送消息调用设置函数；
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_DEL_IPV6_ADDR, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    // 处理发送消息的返回码；
    ret = create_swi_ipv6_return_message(o_message_jso, ret, RFPROP_SYSTEM_DEL_SWI_IPV6ADDR);
    return ret;
}


LOCAL gint32 _parse_i_paras_swi_ipv6route(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, GSList **input_list)
{
    json_object *obj_json = NULL;
    const gchar *ipv6_gateway = NULL;

    return_val_do_info_if_expr(input_list == NULL, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    // 解析输入参数；
    gint32 ret = _parse_i_paras_swi_ipv6addr(i_paras, o_message_jso, input_list);
    return_val_if_expr(ret != HTTP_OK, HTTP_BAD_REQUEST);

    obj_json = NULL;
    // 解析Gateway字段
    ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_SYSTEM_IPV6GATEWAY, &obj_json);
    return_val_do_info_if_expr(ret == FALSE, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_SYSTEM_ADD_SWI_IPV6ROUTE,
            RFPROP_SYSTEM_IPV6GATEWAY));
    ipv6_gateway = dal_json_object_get_str(obj_json);

    // 将相关字段填入*input_lis，准备下发给switch模块的方法；
    *input_list = g_slist_append(*input_list, g_variant_new_string(ipv6_gateway));

    debug_log(DLOG_DEBUG, "%s, %s, %d\n", __FUNCTION__, ipv6_gateway, ret);

    return HTTP_OK;
}


LOCAL gint32 act_system_add_swi_ipv6route(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;
    gint32 ret;

    return_val_do_info_if_expr((o_message_jso == NULL) || (i_paras == NULL) || (i_paras->val_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    // 解析输入参数；
    ret = _parse_i_paras_swi_ipv6route(i_paras, o_message_jso, &input_list);
    return_val_do_info_if_expr(ret != HTTP_OK, HTTP_BAD_REQUEST, uip_free_gvariant_list(input_list));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_ADD_IPV6_ROUTE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    // 处理发送消息的返回码；
    ret = create_swi_ipv6_return_message(o_message_jso, ret, RFPROP_SYSTEM_ADD_SWI_IPV6ROUTE);
    return ret;
}

LOCAL gint32 act_system_del_swi_ipv6route(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    GSList *input_list = NULL;
    gint32 ret;
    return_val_do_info_if_expr((o_message_jso == NULL) || (i_paras == NULL) || (i_paras->val_jso == NULL),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));
    // 解析输入参数；
    ret = _parse_i_paras_swi_ipv6route(i_paras, o_message_jso, &input_list);
    return_val_do_info_if_expr(ret != HTTP_OK, HTTP_BAD_REQUEST, uip_free_gvariant_list(input_list));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_NULL,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_DEL_IPV6_ROUTE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    // 处理发送消息的返回码；
    ret = create_swi_ipv6_return_message(o_message_jso, ret, RFPROP_SYSTEM_DEL_SWI_IPV6ROUTE);
    return ret;
}


LOCAL gint32 act_system_clear_network_usagerate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    json_object *obj_json = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *wave_title = NULL;
    gchar card_wave_title[TMP_STR_LEN] = {0};

    return_val_do_info_if_fail((NULL != o_message_jso) && (NULL != i_paras) && (NULL != i_paras->val_jso),
        HTTP_INTERNAL_SERVER_ERROR, (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    gint32 ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_NETWORK_BWU_WAVE_TITLE, &obj_json);
    return_val_do_info_if_expr(FALSE == ret, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_ACT_PARA_MISSING, NULL, o_message_jso, RFPROP_CLEAR_NETWORK_USAGE,
        RFPROP_NETWORK_BWU_WAVE_TITLE));

    wave_title = dal_json_object_get_str(obj_json);
    obj_json = NULL;

    
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    return_val_do_info_if_expr(((VOS_OK != ret) || (NULL == obj_list)), HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        (void)dal_get_property_value_string((OBJ_HANDLE)obj_node->data, BUSY_NETCARD_BWU_WAVE_TITLE, card_wave_title,
            sizeof(card_wave_title));
        if (strcmp(wave_title, card_wave_title) == 0) {
            obj_handle = (OBJ_HANDLE)obj_node->data;
            break;
        }
    }

    g_slist_free(obj_list);
    return_val_do_info_if_expr(obj_handle == 0, HTTP_BAD_REQUEST,
        (void)create_message_info(MSGID_INVALID_ACTION_PARAM_VALUE, NULL, o_message_jso, wave_title,
        BUSY_NETCARD_BWU_WAVE_TITLE));

    
    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NETCARD_NAME, BUSY_NETCARD_METHOD_CLEAR_BWU_WAVE, AUTH_ENABLE, i_paras->user_role_privilege, NULL, NULL);
    
    if (COMP_CODE_SUCCESS == ret) {
        (void)create_message_info(MSGID_SUCCESS, NULL, o_message_jso);
        ret = HTTP_OK;
    } else if (RFERR_INSUFFICIENT_PRIVILEGE == ret) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        ret = HTTP_FORBIDDEN;
    } else {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
        ret = HTTP_INTERNAL_SERVER_ERROR;
    }

    return ret;
}



LOCAL gint32 redfish_create_uri(const gchar *only_uri, gchar *change_uri, gint32 change_urilen)
{
    int iRet;
    gint32 ret;
    gchar pslot[MAX_RSC_NAME_LEN] = {0};

    if (NULL == only_uri || NULL == change_uri) {
        return VOS_ERR;
    }

    
    ret = redfish_get_board_slot(pslot, MAX_RSC_NAME_LEN);
    if (VOS_OK != ret) {
        return VOS_ERR;
    }

    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    iRet = snprintf_s(change_uri, change_urilen, change_urilen - 1, only_uri, pslot);
#pragma GCC diagnostic pop
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    return VOS_OK;
}


LOCAL gint32 redfish_make_array_uri(gchar *link_val, const gchar *only_uri, gchar *nature, json_object *change_uri)
{
    gchar out_uri[MAX_URI_LENGTH] = {0};
    gint32 ret;
    json_object *array_uri = NULL;
    json_object *object_uri = NULL;

    if (NULL == only_uri || NULL == link_val || NULL == nature) {
        return VOS_ERR;
    }

    array_uri = json_object_new_array();
    return_val_do_info_if_fail(NULL != array_uri, VOS_ERR,
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    object_uri = json_object_new_object();
    return_val_do_info_if_fail(NULL != object_uri, VOS_ERR, json_object_put(array_uri);
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    
    ret = redfish_create_uri(only_uri, out_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, json_object_put(array_uri); json_object_put(object_uri);
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    
    json_object_object_add(object_uri, nature, json_object_new_string(out_uri));
    
    ret = json_object_array_add(array_uri, object_uri);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, json_object_put(array_uri); json_object_put(object_uri);
        debug_log(DLOG_ERROR, "%s,%d:   memory alloc fail. ", __FUNCTION__, __LINE__));

    json_object_object_add(change_uri, link_val, array_uri);

    return VOS_OK;
}


LOCAL gint32 get_systems_links(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    gchar array1_uri[MAX_URI_LENGTH] = "/redfish/v1/Chassis/%s";
    gchar array2_uri[MAX_URI_LENGTH] = "/redfish/v1/Managers/%s";
    

    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    *o_result_jso = json_object_new_object();
    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    ret = redfish_make_array_uri(RFPROP_SYSTEM_CHASSIS, array1_uri, RFPROP_ODATA_ID, *o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s,%d:    redfish_make_array_uri return err. ", __FUNCTION__, __LINE__));
    ret = redfish_make_array_uri(RFPROP_SYSTEM_MANAGERS, array2_uri, RFPROP_ODATA_ID, *o_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s,%d:    redfish_make_array_uri return err. ", __FUNCTION__, __LINE__));
    return HTTP_OK;
}


LOCAL gint32 get_systems_action_target(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar               system_uri[MAX_URI_LENGTH] = {0};
    gchar               slot_id[MAX_RSC_NAME_LEN] = {0};
    json_object *oem_jso = NULL;
    json_object *hw_jso = NULL;
    guchar board_type = BOARD_OTHER;

    
    if (VOS_OK != provider_paras_check(i_paras) || NULL == o_message_jso || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s %d", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    (void)redfish_get_board_slot(slot_id, MAX_RSC_NAME_LEN);

    ret = snprintf_s(system_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_SYSTEM, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    
    *o_result_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:    json_object_new_object fail. ", __FUNCTION__, __LINE__));

    
    rf_add_action_prop(*o_result_jso, system_uri, RFPROP_SYSTEM_RESET);

    oem_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != oem_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "alloc new json object failed"));

    hw_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != hw_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "alloc new json object failed");
        (void)json_object_put(oem_jso));

    
    rf_add_action_prop(hw_jso, system_uri, RFPROP_SYSTEM_FRUCONTROL);
    rf_add_action_prop(hw_jso, system_uri, RFPROP_CLEAR_NETWORK_USAGERATE);
    
    ret = redfish_get_board_type(&board_type);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "get board type failed, ret is %d", ret));
    if (board_type == BOARD_SWITCH) {
        rf_add_action_prop(hw_jso, system_uri, RFPROP_SYSTEM_ADD_SWI_IPV6ADDR);
        rf_add_action_prop(hw_jso, system_uri, RFPROP_SYSTEM_DEL_SWI_IPV6ADDR);
        rf_add_action_prop(hw_jso, system_uri, RFPROP_SYSTEM_ADD_SWI_IPV6ROUTE);
        rf_add_action_prop(hw_jso, system_uri, RFPROP_SYSTEM_DEL_SWI_IPV6ROUTE);
    }
    json_object_object_add(oem_jso, RFPROP_COMMON_HUAWEI, hw_jso);
    json_object_object_add(*o_result_jso, RFPROP_COMMON_OEM, oem_jso);

    return HTTP_OK;
}


LOCAL gint32 get_system_oem(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    // 变量定义
    gint32 ret;
    json_object *huawei_result_jso;

    // 变量初始化
    ret = 0;
    huawei_result_jso = NULL;

    // 入参检查: 500 错误
    
    if (NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras) || NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    

    // 入参检查: 403 错误
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    huawei_result_jso = json_object_new_object();
    return_val_do_info_if_fail((NULL != huawei_result_jso), HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d 'json_object_new_object fail' ", __FUNCTION__, __LINE__));

    // 获取"Huawei" 的值
    ret = get_system_huawei(i_paras, huawei_result_jso);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_result_jso);
        debug_log(DLOG_MASS, "%s, %d: get obj_list fail.\n", __FUNCTION__, __LINE__));

    // 将获得的结果添加入o_result_jso
    (*o_result_jso) = json_object_new_object();
    return_val_do_info_if_fail((NULL != *o_result_jso), HTTP_INTERNAL_SERVER_ERROR, json_object_put(huawei_result_jso);
        debug_log(DLOG_ERROR, "%s, %d 'json_object_new_object fail' ", __FUNCTION__, __LINE__));

    // 获取"OEM"的值
    json_object_object_add(*o_result_jso, RFPROP_FWINV_HUAWEI, huawei_result_jso);

    return HTTP_OK;
}


LOCAL void get_ipinfo_from_output_list(json_object *object, GSList *output)
{
    const gchar *temp_str = NULL;
    json_object *ip_value = NULL;

    return_do_info_if_expr((NULL == object) || (NULL == output),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    temp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output, 0), NULL);
    // 交换平面没有配置IP信息，返回0.0.0.0   转换为null
    if (g_strcmp0(RFPROP_INVALID_IP_ADDR, temp_str)) {
        ip_value = json_object_new_string(temp_str);
    }

    json_object_object_add(object, RF_PROPERTY_ADDR, ip_value);
    temp_str = NULL;
    ip_value = NULL;

    temp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output, 1), NULL);
    if (g_strcmp0(RFPROP_INVALID_IP_ADDR, temp_str)) {
        ip_value = json_object_new_string(temp_str);
    }

    json_object_object_add(object, RF_PROPERTY_SUB_MASK, ip_value);
    temp_str = NULL;
    ip_value = NULL;

    temp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output, 2), NULL);
    if (g_strcmp0(RFPROP_INVALID_IP_ADDR, temp_str)) {
        ip_value = json_object_new_string(temp_str);
    }

    json_object_object_add(object, RF_PROPERTY_GATEWAY, ip_value);

    return;
}


LOCAL void get_ipv6addr_from_output_list(gint32 result, json_object *object, GSList *output)
{
    json_object *obj_array = NULL;
    json_object *node_array = NULL;
    GVariant *property_value = NULL;
    gsize temp_size = 0;
    const SWI_IPV6_ADDR_INFO_STR_S *p_swi_ipv6_addr_info = NULL;

    return_do_info_if_expr(object == NULL, debug_log(DLOG_ERROR, "%s:object NULL pointer", __FUNCTION__));
    obj_array = json_object_new_array();
    return_do_info_if_expr(obj_array == NULL, debug_log(DLOG_ERROR, "%s: json_object_new_array fail\n", __FUNCTION__));

    // 如果去交换板查询不成功，交换板版本不支持或其他，返回空数组；
    return_do_info_if_expr((result != VOS_OK), json_object_object_add(object, RF_PROPERTY_IPV6_ADDR, obj_array));

    return_do_info_if_expr(output == NULL, (void)json_object_put(obj_array);
        debug_log(DLOG_ERROR, "%s:output NULL pointer", __FUNCTION__));

    // 获取第一个参数，代表有多少个IPV6地址,如果没有配置，就不显示；
    guint32 ipv6_addr_count = g_variant_get_int32((GVariant *)g_slist_nth_data(output, 0));
    debug_log(DLOG_DEBUG, "%s, %d ", __FUNCTION__, ipv6_addr_count);
    ipv6_addr_count = (ipv6_addr_count > MAX_SWI_IPV6_GROUP_NUM) ? MAX_SWI_IPV6_GROUP_NUM : ipv6_addr_count;

    if (ipv6_addr_count != 0) {
        // 获取IPV6地址相关的数组；
        property_value = (GVariant *)g_slist_nth_data(output, 1);
        p_swi_ipv6_addr_info =
            (const SWI_IPV6_ADDR_INFO_STR_S *)g_variant_get_fixed_array(property_value, &temp_size, sizeof(guint8));
        debug_log(DLOG_DEBUG, "%s, temp_size %" G_GSIZE_FORMAT " ", __FUNCTION__, temp_size);
        return_do_info_if_expr(p_swi_ipv6_addr_info == NULL, (void)json_object_put(obj_array);
            debug_log(DLOG_ERROR, "%s: p_swi_ipv6_addr_info = NULL", __FUNCTION__));

        // 获取每一个IPV6地址，添加到对象中；
        for (guint32 i = 0; i < ipv6_addr_count; i++) {
            debug_log(DLOG_INFO, "%s: ipv6_addr_str %s, addr_prefix_len%d", __FUNCTION__,
                p_swi_ipv6_addr_info[i].ipv6_address, p_swi_ipv6_addr_info[i].addr_prefix_len);

            node_array = json_object_new_object();
            continue_do_info_if_expr(node_array == NULL,
                debug_log(DLOG_ERROR, "json_object_new_object node_array fail\n"));

            json_object_object_add(node_array, RF_PROPERTY_ADDR,
                json_object_new_string(p_swi_ipv6_addr_info[i].ipv6_address));
            json_object_object_add(node_array, RF_PROPERTY_PREF_LEN,
                json_object_new_int(p_swi_ipv6_addr_info[i].addr_prefix_len));
            // 将新的对象添加到数组中；
            json_object_array_add(obj_array, node_array);
            node_array = NULL;
        }
    }
    json_object_object_add(object, RF_PROPERTY_IPV6_ADDR, obj_array);
    return;
}


LOCAL void get_ipv6route_from_output_list(gint32 result, json_object *object, GSList *output)
{
    json_object *obj_array = NULL;
    json_object *node_array = NULL;
    GVariant *property_value = NULL;
    gsize temp_size = 0;
    const SWI_IPV6_ROUTE_INFO_STR_S *p_swi_ipv6_route_info = NULL;

    return_do_info_if_expr(object == NULL, debug_log(DLOG_ERROR, "%s:object NULL pointer", __FUNCTION__));
    obj_array = json_object_new_array();
    return_do_info_if_expr(obj_array == NULL, debug_log(DLOG_ERROR, "%s: json_object_new_array fail\n", __FUNCTION__));

    // 如果去交换板查询不成功，交换板版本不支持或其他，返回空数组；
    return_do_info_if_expr((result != VOS_OK), json_object_object_add(object, RF_PROPERTY_IPV6_ROUTE, obj_array));

    return_do_info_if_expr(output == NULL, (void)json_object_put(obj_array);
        debug_log(DLOG_ERROR, "%s:output NULL pointer", __FUNCTION__));

    // 获取第一个参数，代表有多少个IPV6地址；
    guint32 ipv6_route_count = g_variant_get_int32((GVariant *)g_slist_nth_data(output, 0));
    debug_log(DLOG_DEBUG, "%s, %d ", __FUNCTION__, ipv6_route_count);
    ipv6_route_count = (ipv6_route_count > MAX_SWI_IPV6_GROUP_NUM) ? MAX_SWI_IPV6_GROUP_NUM : ipv6_route_count;

    if (ipv6_route_count != 0) {
        // 获取IPV6地址相关的数组；
        property_value = (GVariant *)g_slist_nth_data(output, 1);
        p_swi_ipv6_route_info =
            (const SWI_IPV6_ROUTE_INFO_STR_S *)g_variant_get_fixed_array(property_value, &temp_size, sizeof(guint8));
        debug_log(DLOG_DEBUG, "%s, %" G_GSIZE_FORMAT " ", __FUNCTION__, temp_size);
        return_do_info_if_expr(p_swi_ipv6_route_info == NULL, (void)json_object_put(obj_array);
            debug_log(DLOG_ERROR, "%s: p_swi_ipv6_route_info = NULL", __FUNCTION__));

        // 获取每一个IPV6路由，添加到对象中；
        for (guint32 i = 0; i < ipv6_route_count; i++) {
            debug_log(DLOG_INFO, "%s:ipv6_addr_str %s, ipv6_gateway_str %s, addr_prefix_len %d", __FUNCTION__,
                p_swi_ipv6_route_info[i].ipv6_address, p_swi_ipv6_route_info[i].ipv6_gateway,
                p_swi_ipv6_route_info[i].addr_prefix_len);

            node_array = json_object_new_object();
            continue_do_info_if_expr(node_array == NULL,
                debug_log(DLOG_ERROR, "json_object_new_object node_array fail\n"));

            json_object_object_add(node_array, RF_PROPERTY_ADDR,
                json_object_new_string(p_swi_ipv6_route_info[i].ipv6_address));
            json_object_object_add(node_array, RF_PROPERTY_PREF_LEN,
                json_object_new_int(p_swi_ipv6_route_info[i].addr_prefix_len));
            json_object_object_add(node_array, RF_PROPERTY_GATEWAY,
                json_object_new_string(p_swi_ipv6_route_info[i].ipv6_gateway));

            // 将新的对象添加到数组中；
            json_object_array_add(obj_array, node_array);
            node_array = NULL;
        }
    }
    json_object_object_add(object, RF_PROPERTY_IPV6_ROUTE, obj_array);
    return;
}


LOCAL void get_each_switch_ipinfo(PROVIDER_PARAS_S *i_paras, json_object *switch_ip_obj, const gchar *switch_type)
{
    json_object *object = NULL;
    GSList *output_list = NULL;
    GSList *ipv6_addr_output_list = NULL;
    GSList *ipv6_route_output_list = NULL;

    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    return_do_info_if_expr((switch_ip_obj == NULL) || (i_paras == NULL),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 没有对象，显示null
    ret = get_switch_plane_object(switch_type, &obj_handle);
    goto_label_do_info_if_fail(ret == VOS_OK, exit,
        debug_log(DLOG_MASS, "%s: get_switch_plane_object fail' ", __FUNCTION__));

    // 调方法，查询IP信息
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_GET_IP_MASK_GW, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &output_list);

    // 调用方法失败不返回，继续查后面的内容；
    
    debug_log(DLOG_DEBUG, "%s: get %s ip info return %d", __FUNCTION__, switch_type, ret);
    

    object = json_object_new_object();
    goto_label_do_info_if_expr(NULL == object, exit,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail' ", __FUNCTION__));

    // 将出参链表中的值add进json对象，如果失败，保持之前的逻辑，不添加IPV4的内容；
    if (ret == VOS_OK) {
        get_ipinfo_from_output_list(object, output_list);
    }

    // 调方法，查询IPV6信息
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_GET_IPV6_ADDR, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &ipv6_addr_output_list);
    debug_log(DLOG_DEBUG, "%s: get %s ipv6info ret %d ", __FUNCTION__, switch_type, ret);

    // 调用方法失败，不返回，添加空数组，继续查后面的内容；
    // 将出参链表中的值add进json对象
    get_ipv6addr_from_output_list(ret, object, ipv6_addr_output_list);

    // 调方法，查询IPV6 路由信息
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_GET_IPV6_ROUTE, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &ipv6_route_output_list);
    debug_log(DLOG_DEBUG, "%s: get %s ipv6routeinfo ret %d ", __FUNCTION__, switch_type, ret);

    // 调用方法失败，不返回，添加空数组，继续查后面的内容；
    // 将出参链表中的值add进json对象
    get_ipv6route_from_output_list(ret, object, ipv6_route_output_list);

exit:

    // 该接口中有判空
    uip_free_gvariant_list(output_list);
    uip_free_gvariant_list(ipv6_addr_output_list);
    uip_free_gvariant_list(ipv6_route_output_list);

    json_object_object_add(switch_ip_obj, switch_type, object);

    return;
}


LOCAL void get_each_switch_stacking_status(PROVIDER_PARAS_S *i_paras, json_object *switch_stacking_obj,
    const gchar *switch_type)
{
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *stacking_status = NULL;
    gint8 temp_val;
    gint32 ret;
    const gchar* temp_str[] = { "None",
                                "Master",
                                "Standby",
                                "Slave",
                              };

    return_do_info_if_expr((NULL == switch_stacking_obj) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 没有对象，显示null
    ret = get_switch_plane_object(switch_type, &obj_handle);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_MASS, "%s: get_switch_plane_object fail' ", __FUNCTION__));

    // 查询堆叠
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_GET_STACKING_STATUS, AUTH_ENABLE, i_paras->user_role_privilege,
        NULL, &output_list);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_DEBUG, "%s: get %s stacking info return %d", __FUNCTION__, switch_type, ret));

    temp_val = (gint8)g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    if (temp_val < SWITCH_STACKING_ROLE_NUM && temp_val >= 0) {
        stacking_status = json_object_new_string(temp_str[temp_val]);
    }

exit:
    // 该接口中有判空
    uip_free_gvariant_list(output_list);
    json_object_object_add(switch_stacking_obj, RFPROP_SYSTEM_SWITCH_STACKING_STATUS, stacking_status);

    return;
}


LOCAL void get_each_switch_version_info(PROVIDER_PARAS_S *i_paras, json_object *switch_version_obj,
    const gchar *switch_type)
{
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    const gchar *temp_str = NULL;
    json_object *version_info = NULL;
    gint32 ret;

    return_do_info_if_expr((NULL == switch_version_obj) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 没有对象，显示null
    ret = get_switch_plane_object(switch_type, &obj_handle);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_MASS, "%s: get_switch_plane_object fail' ", __FUNCTION__));

    // call method 查询版本信息
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_GET_VERSION, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &output_list);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_DEBUG, "%s: get %s version info return %d", __FUNCTION__, switch_type, ret));

    temp_str = g_variant_get_string((GVariant *)g_slist_nth_data(output_list, 0), NULL);
    version_info = json_object_new_string(temp_str);

exit:
    // 该接口中有判空
    uip_free_gvariant_list(output_list);
    json_object_object_add(switch_version_obj, RFPROP_SYSTEM_SWITCH_VERSION_INFO, version_info);

    return;
}


LOCAL void redfish_get_oem_switch_ipinfo(json_object *json_obj, PROVIDER_PARAS_S *i_paras)
{
    json_object *switch_ip_obj = NULL;

    return_do_info_if_expr((NULL == json_obj) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    switch_ip_obj = json_object_new_object();
    if (NULL != switch_ip_obj) {
        // 获取base的IP信息
        get_each_switch_ipinfo(i_paras, switch_ip_obj, RFPROP_SYSTEM_SWITCH_BASE);

        // 获取fabric的IP信息
        get_each_switch_ipinfo(i_paras, switch_ip_obj, RFPROP_SYSTEM_SWITCH_FABRIC);

        // 获取FC的IP信息
        get_each_switch_ipinfo(i_paras, switch_ip_obj, RFPROP_SYSTEM_SWITCH_FC);
    }

    json_object_object_add(json_obj, RFPROP_SYSTEM_SWITCH_IPINFO, switch_ip_obj);

    return;
}


LOCAL void redfish_get_oem_switch_base_info(json_object *json_obj, PROVIDER_PARAS_S *i_paras)
{
    json_object *switch_base_obj = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    return_do_info_if_expr((NULL == json_obj) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 没有对象，显示null
    ret = get_switch_plane_object(RFPROP_SYSTEM_SWITCH_BASE, &obj_handle);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_MASS, "%s: get switch base plane object fail.", __FUNCTION__));

    switch_base_obj = json_object_new_object();
    if (NULL != switch_base_obj) {
        get_each_switch_version_info(i_paras, switch_base_obj, RFPROP_SYSTEM_SWITCH_BASE);
        get_each_switch_stacking_status(i_paras, switch_base_obj, RFPROP_SYSTEM_SWITCH_BASE);
    }

exit:
    json_object_object_add(json_obj, RFPROP_SYSTEM_SWITCH_BASE, switch_base_obj);

    return;
}


LOCAL void redfish_get_oem_switch_fabric_info(json_object *json_obj, PROVIDER_PARAS_S *i_paras)
{
    json_object *switch_fabric_obj = NULL;
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;

    return_do_info_if_expr((NULL == json_obj) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    // 没有对象，显示null
    ret = get_switch_plane_object(RFPROP_SYSTEM_SWITCH_FABRIC, &obj_handle);
    goto_label_do_info_if_expr(VOS_OK != ret, exit,
        debug_log(DLOG_MASS, "%s: get switch fabric plane object fail.", __FUNCTION__));

    switch_fabric_obj = json_object_new_object();
    if (NULL != switch_fabric_obj) {
        get_each_switch_version_info(i_paras, switch_fabric_obj, RFPROP_SYSTEM_SWITCH_FABRIC);
        get_each_switch_stacking_status(i_paras, switch_fabric_obj, RFPROP_SYSTEM_SWITCH_FABRIC);
    }

exit:
    json_object_object_add(json_obj, RFPROP_SYSTEM_SWITCH_FABRIC, switch_fabric_obj);

    return;
}


LOCAL void redfish_get_oem_switch_info(json_object *json_obj, PROVIDER_PARAS_S *i_paras)
{
    json_object *switch_info_obj = NULL;

    return_do_info_if_expr((NULL == json_obj) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    switch_info_obj = json_object_new_object();
    if (NULL != switch_info_obj) {
        redfish_get_oem_switch_base_info(switch_info_obj, i_paras);
        redfish_get_oem_switch_fabric_info(switch_info_obj, i_paras);
    }

    json_object_object_add(json_obj, RFPROP_SYSTEM_SWITCH_INFO, switch_info_obj);

    return;
}


LOCAL void get_slot_bit_map(OBJ_HANDLE product_handle, json_object *o_result_jso)
{
    gint32 ret;
    guint32 count = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 board_widthtype = 0xff;

    return_do_info_if_expr(NULL == o_result_jso, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    
    ret = dfl_get_object_count(CLASS_WIDTH_BITMAP, &count);
    return_do_info_if_expr((DFL_OK != ret) || (0 == count),
        debug_log(DLOG_MASS, "%s: WidthBitMap object not exit.", __FUNCTION__);
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_SLOTWIDTH, NULL));

    // 先根据 product里面的 BoardWidthType确定使用哪个对象的值
    (void)dal_get_property_value_byte(product_handle, PROPERTY_BOARD_WIDTHTYPE, &board_widthtype);

    (void)dal_get_specific_object_byte(CLASS_WIDTH_BITMAP, PROPERTY_WIDTH_TYPE, board_widthtype, &obj_handle);
    

    
    rf_add_property_jso_uint32(obj_handle, PROPERTY_BITMAP_VALUE, RFPROP_SYSTEM_COMPUTERSYSTEM_SLOTWIDTH, o_result_jso);
}


LOCAL void redfish_get_oem_pch(json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_MAINBOARD_NAME, 0, &obj_handle);

    rf_add_property_jso_string(obj_handle, PROPERTY_MAINBOARD_PCHMODEL, RFPROP_SYSTEM_PCH_MODEL, o_result_jso);
}

LOCAL gint32 redfish_get_storage_health_by_handle(OBJ_HANDLE obj_handle, guchar *health_out)
{
    gint32 ret = 0;
    gchar object_name[STORAGE_CTRL_NAME_LENGTH] = {0};
    gchar property_name[STORAGE_CTRL_NAME_LENGTH] = {0};
    guchar health = 0;

    if (NULL == health_out) {
        debug_log(DLOG_ERROR, "%s, %d: input error.\n", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    
    if (VOS_OK == dfl_get_referenced_property(obj_handle, PROPERTY_RAID_CONTROLLER_TYPE, object_name, property_name,
        STORAGE_CTRL_NAME_LENGTH, STORAGE_CTRL_NAME_LENGTH)) {
        ret = dfl_get_object_handle(object_name, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get raid obj_handle fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_RAIDCARD_HEALTH, &health);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get raid card health property fail.\n", __FUNCTION__, __LINE__));
    }
    
    else if (VOS_OK == dfl_get_referenced_property(obj_handle, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME, object_name,
        property_name, STORAGE_CTRL_NAME_LENGTH, STORAGE_CTRL_NAME_LENGTH)) {
        ret = dfl_get_object_handle(object_name, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get ComRaidCard obj_handle fail.\n", __FUNCTION__, __LINE__));

        ret = dfl_get_referenced_property(obj_handle, PROPERTY_COMPONENT_DEVICENUM, object_name, property_name,
            STORAGE_CTRL_NAME_LENGTH, STORAGE_CTRL_NAME_LENGTH);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get PcieCard object name fail.\n", __FUNCTION__, __LINE__));

        ret = dfl_get_object_handle(object_name, &obj_handle);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get PcieCard obj_handle fail.\n", __FUNCTION__, __LINE__));

        ret = dal_get_property_value_byte(obj_handle, PROPERTY_PCIECARD_HEALTH, &health);
        return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_MASS, "%s, %d: get PcieCard health property fail.\n", __FUNCTION__, __LINE__));
    }

    *health_out = health;

    return VOS_OK;
}


LOCAL void redfish_get_oem_storage_summary(json_object *o_result_jso)
{
    gint32 ret = 0;
    guchar controller_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar raid_health = 0;
    guchar sd_health = 0;
    guchar health = 0;
    OBJ_HANDLE sd_handle = 0;
    guchar sd_presence = 0;
    gint32 count = 0;
    json_object *status_obj_jso = NULL;
    json_object *storage_sum = NULL;

    
    if (NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    // 获取RAID 控制器
    (void)dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &controller_id);
        do_if_expr(((VOS_OK != ret) || (0xFF == controller_id)), continue);

        (void)redfish_get_storage_health_by_handle((OBJ_HANDLE)obj_node->data, &raid_health);

        health = raid_health > health ? raid_health : health;
        count++;
    }

    
    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
    obj_list = NULL;
    

    // 获取 SD 卡存储器的
    (void)dfl_get_object_list(CLASS_RAID_CHIP_NAME, &obj_list);

    if (NULL != obj_list) {
        // 只会存在一个SD 控制器
        sd_handle = (OBJ_HANDLE)g_slist_nth_data(obj_list, 0);
        ret = dal_get_property_value_byte(sd_handle, PROPETRY_PRESENT, &sd_presence);
        if (VOS_OK == ret && 0 != sd_presence) {
            (void)dal_get_property_value_byte(sd_handle, PROPETRY_SDRAID_HEALTH, &sd_health);
            do_val_if_expr(DRIVE_INVALID_VALUE != sd_health, (health = sd_health > health ? sd_health : health);
                count++;);
        }
    }

    
    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));
    

    
    storage_sum = json_object_new_object();
    json_object_object_add(storage_sum, RFPROP_COMMON_COUNT, json_object_new_int(count));
    do_if_expr(count > 0, get_resource_status_property(NULL, &health, NULL, &status_obj_jso, TRUE));

    json_object_object_add(storage_sum, RFPROP_MANAGER_STATUS, status_obj_jso);
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_STORAGE_SUMMARY, storage_sum);
}


LOCAL void redfish_get_oem_logical_drive_summary(json_object *o_result_jso)
{
    gint32 count = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guchar ld_health = 0;
    guchar health = 0;
    guchar health_num = 0;

    json_object *status_obj_jso = NULL;
    json_object *logical_drive_sum = NULL;
    guint8 ld_raid_level = 0;

    
    if (NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    // 不存在逻辑盘时会返回错误，这个忽略，按0来处理
    (void)dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);

    for (obj_node = obj_list; obj_node; obj_node = g_slist_next(obj_node)) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        continue_if_expr(RAID_LEVEL_DELETED == ld_raid_level);
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_HEALTH, &health_num);

        
        guint8 level = LEVEL_UNKNOWN;
        (void)sml_ld_state2str(health_num, &level);
        if (level == LEVEL_NORMAL) {
            ld_health = RF_STATUS_COUNT_OK;
        } else if (level == LEVEL_WARNING || level == LEVEL_CRITICAL) {
            ld_health = RF_STATUS_COUNT_WARNING;
        } else {
            debug_log(DLOG_DEBUG, "%s: invalid health is %d", __FUNCTION__, health_num);
        }

        health = ld_health > health ? ld_health : health;
        count++;
    }

    do_val_if_expr(NULL != obj_list, g_slist_free(obj_list));

    
    logical_drive_sum = json_object_new_object();
    json_object_object_add(logical_drive_sum, RFPROP_COMMON_COUNT, json_object_new_int(count));

    do_if_expr(count > 0, get_resource_status_property(NULL, &health, NULL, &status_obj_jso, TRUE));

    json_object_object_add(logical_drive_sum, RFPROP_MANAGER_STATUS, status_obj_jso);
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_STORAGE_LOGICAL_DRIVE_SUMMARY, logical_drive_sum);
}


LOCAL guchar redfish_get_rollhealth_by_summary(json_object *summary, guchar *health_val)
{
    json_object *status = NULL;
    json_object *health = NULL;
    gint32 ret;
    const gchar *health_str = 0;
    guchar ret_val = 0;

    return_val_if_expr(NULL == summary, VOS_ERR);

    ret = json_object_object_get_ex(summary, RFPROP_MANAGER_STATUS, &status);
    return_val_do_info_if_expr(TRUE != ret || NULL == status, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: json_object_object_get_ex, get status error", __FUNCTION__));

    ret = json_object_object_get_ex(status, RFPROP_SEL_SEVERITY, &health);
    return_val_do_info_if_expr(TRUE != ret || NULL == health, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex, get Severity error", __FUNCTION__));

    health_str = dal_json_object_get_str(health);
    if (NULL != health_str) {
        if (0 == g_strcmp0(health_str, INFORMATIONAL_STRING)) {
            ret_val = RF_STATUS_COUNT_OK;
        } else if (0 == g_strcmp0(health_str, MINOR_STRING)) {
            ret_val = RF_STATUS_COUNT_WARNING;
        } else if (0 == g_strcmp0(health_str, RF_STATUS_MAJOR)) {
            ret_val = RF_STATUS_COUNT_MAJOR;
        } else if (0 == g_strcmp0(health_str, RF_STATUS_CRITICAL)) {
            ret_val = RF_STATUS_COUNT_CRITICAL;
        }
    }
    *health_val = ret_val;
    return VOS_OK;
}


LOCAL void redfish_get_oem_storage_subsystem_summary(json_object *o_result_jso)
{
    json_object *logical_drive_obj = NULL;
    json_object *drive_obj = NULL;
    json_object *storage_obj = NULL;

    json_object *all_obj = NULL;
    json_object *status_obj_jso = NULL;
    json_object *subsystem_sum = NULL;

    guchar logical_drive_health = 0;
    guchar drive_health = 0;
    guchar storage_health = 0;

    guchar health = 0;
    guchar exist_flag = 0;
    gint32 ret = 0;

    
    if (NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    all_obj = json_object_new_object();
    return_do_info_if_expr(NULL == all_obj, debug_log(DLOG_ERROR, "%s: json_object_new_object error", __FUNCTION__));

    // 获取逻辑盘,有申请内存，加在all_obj
    redfish_get_oem_logical_drive_summary(all_obj);

    (void)json_object_object_get_ex(all_obj, RFPROP_SYSTEM_STORAGE_LOGICAL_DRIVE_SUMMARY, &logical_drive_obj);
    if (NULL != logical_drive_obj) {
        ret = redfish_get_rollhealth_by_summary(logical_drive_obj, &logical_drive_health);
        do_if_expr(VOS_OK == ret, exist_flag++);
        health = logical_drive_health > health ? logical_drive_health : health;
    }

    // 获取RAID 控制器,有申请内存，加在all_obj
    redfish_get_oem_storage_summary(all_obj);
    (void)json_object_object_get_ex(all_obj, RFPROP_SYSTEM_STORAGE_SUMMARY, &storage_obj);
    if (NULL != storage_obj) {
        ret = redfish_get_rollhealth_by_summary(storage_obj, &storage_health);
        do_if_expr(VOS_OK == ret, exist_flag++);
        health = storage_health > health ? storage_health : health;
    }
    // 释放all_obj，包含逻辑盘,RAID 控制器
    json_object_put(all_obj);

    // 获取硬盘,有申请内存，drive_obj 传出
    get_chassis_oem_drive_summary(&drive_obj);

    if (NULL != drive_obj) {
        ret = redfish_get_rollhealth_by_summary(drive_obj, &drive_health);
        do_if_expr(VOS_OK == ret, exist_flag++);
        // 释放drive_obj
        json_object_put(drive_obj);
        health = drive_health > health ? drive_health : health;
    }
    return_do_info_if_expr(exist_flag == 0,
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_STORAGE_SUBSYSTEM_SUMMARY, NULL));
    subsystem_sum = json_object_new_object();
    get_resource_status_property(NULL, &health, NULL, &status_obj_jso, TRUE);
    json_object_object_add(subsystem_sum, RFPROP_MANAGER_STATUS, status_obj_jso);

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_STORAGE_SUBSYSTEM_SUMMARY, subsystem_sum);
}


LOCAL gint32 get_oem_storage_component_json_object(const gchar *name, const gchar *odata_id, json_object **o_result_jso)
{
    json_object *name_jso = NULL;
    json_object *link_jso = NULL;
    json_object *odata_id_jso = NULL;
    json_object *component_jso = NULL;

    
    return_val_do_info_if_fail(NULL != name && NULL != o_result_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    component_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != component_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: new component_jso fail, name is %s.", __func__, name));

    
    name_jso = json_object_new_string(name);
    return_val_do_info_if_fail(NULL != name_jso, VOS_ERR, json_object_put(component_jso);
        debug_log(DLOG_ERROR, "%s: new name_jso fail, name is %s.", __func__, name));
    json_object_object_add(component_jso, NAME_KEY, name_jso);

    if (NULL != odata_id) {
        
        link_jso = json_object_new_object();
        return_val_do_info_if_fail(NULL != link_jso, VOS_ERR, json_object_put(component_jso);
            debug_log(DLOG_ERROR, "%s: new link_jso fail, name is %s.", __func__, name));
        json_object_object_add(component_jso, RFPROP_SYSTEM_STORAGE_LINK, link_jso);

        
        odata_id_jso = json_object_new_string(odata_id);
        return_val_do_info_if_fail(NULL != odata_id_jso, VOS_ERR, json_object_put(component_jso);
            debug_log(DLOG_ERROR, "%s: new odata_id_jso fail, odata_id is %s.", __func__, odata_id));
        json_object_object_add(link_jso, RFPROP_ODATA_ID, odata_id_jso);
    }

    *o_result_jso = component_jso;
    return VOS_OK;
}


LOCAL void add_disk_support_crypto_erase(OBJ_HANDLE hdd_handle, json_object *drive_object_jso)
{
    guint8 support_earse = G_MAXUINT8;
    json_object *support_earse_jso = NULL;

    return_if_expr(NULL == drive_object_jso);

    is_disk_support_crypto_erase(hdd_handle, &support_earse);

    if (G_MAXUINT8 != support_earse) {
        support_earse_jso = json_object_new_boolean(support_earse);
    }
    json_object_object_add(drive_object_jso, PROPERTY_DEVICE_OEM_CRYPTO_ERASE_SUPPORTED, support_earse_jso);

    return;
}


LOCAL gint32 get_oem_storage_drive_json_object(OBJ_HANDLE hdd_handle, json_object **o_result_jso)
{
    int iRet;
    errno_t safe_fun_ret;
    gint32 ret;
    gchar board_slot[MAX_RSC_ID_LEN + 1] = {0};
    gchar hdd_device_name[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    gchar hdd_name[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    gchar hdd_physical_location[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    gchar hdd_rsc_id[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    gchar hdd_odata_id[MAX_URI_LENGTH + 1] = {0};
    json_object *hdd_jso = NULL;

    
    ret = redfish_get_board_slot(board_slot, sizeof(board_slot));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get board slot fail(%d).", __func__, ret));

    
    ret = dal_get_property_value_string(hdd_handle, PROPERTY_HDD_DEVICENAME, hdd_device_name, STORAGE_CTRL_INFO_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop DeviceName fail(%d).", __func__, ret));
    
    safe_fun_ret = strncpy_s(hdd_name, sizeof(hdd_name), hdd_device_name, sizeof(hdd_name) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));
    

    
    ret = dal_get_property_value_string(hdd_handle, PROPERTY_HDD_PYSICAL_LOCATION, hdd_physical_location,
        STORAGE_CTRL_INFO_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get prop PhysicalLocation fail(%d).", __func__, ret));

    
    dal_clear_string_blank(hdd_device_name, sizeof(hdd_device_name));
    dal_clear_string_blank(hdd_physical_location, sizeof(hdd_physical_location));

    
    iRet = snprintf_s(hdd_rsc_id, STORAGE_CTRL_INFO_LENGTH + 1, STORAGE_CTRL_INFO_LENGTH, "%s%s", hdd_physical_location,
        hdd_device_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
    iRet =
        snprintf_s(hdd_odata_id, MAX_URI_LENGTH + 1, MAX_URI_LENGTH, URI_FORMAT_CHASSIS_DRIVE, board_slot, hdd_rsc_id);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    ret = get_oem_storage_component_json_object(hdd_name, hdd_odata_id, &hdd_jso);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get hdd_jso fail(%d).", __func__, ret));

    
    add_disk_support_crypto_erase(hdd_handle, hdd_jso);
    

    *o_result_jso = hdd_jso;
    return VOS_OK;
}


LOCAL void get_oem_storage_pcie_drive_for_storage(json_object *hdds_arr_jso)
{
    gint32 ret;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint8 hdd_presence = 0;
    json_object *pcie_hdd_jso = NULL;

    
    return_do_info_if_fail(NULL != hdds_arr_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dfl_get_object_list(CLASS_PCIEHDD_NAME, &obj_list);
    
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s: get object list of class PCIeDisk fail(%d).", __func__, ret));
    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop Presence fail(%d).", __func__, ret));
        continue_if_fail(1 == hdd_presence);

        
        get_oem_storage_drive_json_object((OBJ_HANDLE)obj_node->data, &pcie_hdd_jso);

        
        json_object_array_add(hdds_arr_jso, pcie_hdd_jso);
        pcie_hdd_jso = NULL;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
}


LOCAL void get_oem_storage_physical_drive_for_storage(json_object *hdds_arr_jso)
{
    gint32 ret;
    OBJ_HANDLE hdd_back_plane_obj_handle = 0;
    OBJ_HANDLE obj_handle = 0;
    guint8 start_slot = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar hdd_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    guint8 hdd_presence = 0;
    guint8 hdd_id = 0;
    json_object *hdd_jso = NULL;

    
    return_do_info_if_fail(NULL != hdds_arr_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dal_get_object_handle_nth(CLASS_HDDBACKPLANE_NAME, 0, &hdd_back_plane_obj_handle);
    if (VOS_OK == ret) {
        ret = dal_get_property_value_byte(hdd_back_plane_obj_handle, PROPERTY_HDDBACKPLANE_STARTSLOT, &start_slot);
        do_if_fail(VOS_OK == ret, start_slot = 0;
            debug_log(DLOG_INFO, "%s: get prop StartSlot for HDDBackplane object fail(%d).", __func__, ret));
    } else {
        start_slot = 0;
        debug_log(DLOG_INFO, "%s: get obj_handle for HDDBackplane class fail(%d).", __func__, ret);
    }

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s: get object list of class Hdd fail(%d).", __func__, ret));
    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop Presence fail(%d).", __func__, ret));
        continue_if_fail(1 == hdd_presence);

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ID, &hdd_id);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));
        continue_if_fail(hdd_id >= start_slot);

        
        
        (void)memset_s(hdd_ref_raid_controller, sizeof(hdd_ref_raid_controller), 0, sizeof(hdd_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            hdd_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for Hdd object fail(%d).", __func__, ret));
        
        
        if (strlen(hdd_ref_raid_controller) > 0 && g_strcmp0(hdd_ref_raid_controller, "PCH") != 0) {
            ret = dfl_get_object_handle(hdd_ref_raid_controller, &obj_handle);
            if (ret == DFL_OK) {
                continue;
            }
        }
        
        
        get_oem_storage_drive_json_object((OBJ_HANDLE)obj_node->data, &hdd_jso);

        
        json_object_array_add(hdds_arr_jso, hdd_jso);
        hdd_jso = NULL;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
}


LOCAL void get_oem_storage_physical_drive_for_raid_controller(OBJ_HANDLE raid_controller_obj_handle,
    json_object *raid_controller_jso, gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size,
    gint32 *hdd_slot_numbers, gint32 hdd_max_size)
{
    gint32 ret;
    OBJ_HANDLE hdd_back_plane_obj_handle = 0;
    guint8 start_slot = 0;
    const gchar *raid_controller_obj_name = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar hdd_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    guint8 hdd_presence = 0;
    guint8 hdd_id = 0;
    json_object *hdd_jso = NULL;
    json_object *hdds_arr_jso = NULL;

    
    return_do_info_if_fail(NULL != raid_controller_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dal_get_object_handle_nth(CLASS_HDDBACKPLANE_NAME, 0, &hdd_back_plane_obj_handle);
    if (VOS_OK == ret) {
        ret = dal_get_property_value_byte(hdd_back_plane_obj_handle, PROPERTY_HDDBACKPLANE_STARTSLOT, &start_slot);
        do_if_fail(VOS_OK == ret, start_slot = 0;
            debug_log(DLOG_INFO, "%s: get prop StartSlot for HDDBackplane object fail(%d).", __func__, ret));
    } else {
        start_slot = 0;
        debug_log(DLOG_INFO, "%s: get obj_handle for HDDBackplane class fail(%d).", __func__, ret);
    }

    
    raid_controller_obj_name = dfl_get_object_name(raid_controller_obj_handle);
    return_do_info_if_fail(NULL != raid_controller_obj_name,
        debug_log(DLOG_ERROR, "%s: get name of RaidController object fail.", __func__));

    
    hdds_arr_jso = json_object_new_array();
    return_do_info_if_fail(NULL != hdds_arr_jso, debug_log(DLOG_ERROR, "%s: new hdds_arr_jso fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    
    return_do_info_if_fail(VOS_OK == ret, json_object_put(hdds_arr_jso);
        debug_log(DLOG_DEBUG, "%s: get object list of class Hdd fail(%d).", __func__, ret));
    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)memset_s(hdd_ref_raid_controller, sizeof(hdd_ref_raid_controller), 0, sizeof(hdd_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            hdd_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for Hdd object fail(%d).", __func__, ret));
        continue_if_fail(VOS_OK == g_strcmp0(hdd_ref_raid_controller, raid_controller_obj_name));

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop Presence fail(%d).", __func__, ret));
        continue_if_fail(1 == hdd_presence);

        
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ID, &hdd_id);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));
        continue_if_fail(hdd_id >= start_slot && hdd_id < hotspare_hdd_max_size && hdd_id < hdd_max_size &&
            0 == span_hotspare_hdd_slot_numbers[hdd_id] && 0 == hdd_slot_numbers[hdd_id]);
        

        
        get_oem_storage_drive_json_object((OBJ_HANDLE)obj_node->data, &hdd_jso);

        
        json_object_array_add(hdds_arr_jso, hdd_jso);
        hdd_jso = NULL;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    
    return_do_info_if_fail(json_object_array_length(hdds_arr_jso) > 0, json_object_put(hdds_arr_jso);
        debug_log(DLOG_INFO, "%s: no physical drive associated with the raid controller directly.", __func__));
    json_object_object_add(raid_controller_jso, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, hdds_arr_jso);
}


LOCAL void get_oem_storage_physical_drive_for_logical_drive(OBJ_HANDLE logical_drive_obj_handle, json_object *ld_jso,
    gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size, gint32 *hdd_slot_numbers, gint32 hdd_max_size)
{
    gint32 ret;
    OBJ_HANDLE hdd_back_plane_obj_handle = 0;
    guint8 start_slot = 0;
    gchar ld_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    GVariant *ld_participated_pd_slots = NULL;
    const guchar *ld_participated_pd_slots_array = NULL;
    guint32 slots_array_len;
    GVariant *ld_participated_pd_enclosures = NULL;
    const guint16 *ld_participated_pd_enclosures_array = NULL;
    guint32 enclosures_array_len;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint32 index_num = 0;
    gchar hdd_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    guint8 hdd_presence = 0;
    guint8 hdd_id = 0;
    guint8 hdd_slot_number = 0;
    guint16 hdd_enclosure_device_id = 0;
    json_object *hdd_jso = NULL;
    json_object *hdds_arr_jso = NULL;
    gsize parm_temp = 0;

    
    return_do_info_if_fail(NULL != ld_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dal_get_object_handle_nth(CLASS_HDDBACKPLANE_NAME, 0, &hdd_back_plane_obj_handle);
    if (VOS_OK == ret) {
        ret = dal_get_property_value_byte(hdd_back_plane_obj_handle, PROPERTY_HDDBACKPLANE_STARTSLOT, &start_slot);
        do_if_fail(VOS_OK == ret, start_slot = 0;
            debug_log(DLOG_INFO, "%s: get prop StartSlot for HDDBackplane object fail(%d).", __func__, ret));
    } else {
        start_slot = 0;
        debug_log(DLOG_INFO, "%s: get obj_handle for HDDBackplane class fail(%d).", __func__, ret);
    }

    
    ret = dal_get_property_value_string(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
        ld_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for LogicalDrive object fail(%d).", __func__, ret));

    
    ret = dfl_get_property_value(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_SLOT,
        &ld_participated_pd_slots);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get prop ParticipatedPDSlot fail(%d).", __func__, ret));

    ld_participated_pd_slots_array =
        (const guchar *)g_variant_get_fixed_array(ld_participated_pd_slots, &parm_temp, sizeof(guchar));
    slots_array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != ld_participated_pd_slots_array, exit,
        debug_log(DLOG_ERROR, "%s: get ld_participated_pd_slots_array fail.", __func__));

    
    ret = dfl_get_property_value(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_PARTICIPATED_PD_ENCLOSURE,
        &ld_participated_pd_enclosures);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get prop ParticipatedPDEnclosures fail(%d).", __func__, ret));

    ld_participated_pd_enclosures_array =
        (const guint16 *)g_variant_get_fixed_array(ld_participated_pd_enclosures, &parm_temp, sizeof(guint16));
    enclosures_array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != ld_participated_pd_enclosures_array, exit,
        debug_log(DLOG_ERROR, "%s: get ld_participated_pd_enclosures_array fail.", __func__));

    
    hdds_arr_jso = json_object_new_array();
    goto_label_do_info_if_fail(NULL != hdds_arr_jso, exit,
        debug_log(DLOG_ERROR, "%s: new hdds_arr_jso fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    
    goto_label_do_info_if_fail(VOS_OK == ret, exit, json_object_put(hdds_arr_jso);
        debug_log(DLOG_DEBUG, "%s: get object list of class Hdd fail(%d).", __func__, ret));
    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)memset_s(hdd_ref_raid_controller, sizeof(hdd_ref_raid_controller), 0, sizeof(hdd_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            hdd_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_DEBUG, "%s: get prop RefRAIDController for Hdd object fail(%d).", __func__, ret));
        continue_if_fail(VOS_OK == g_strcmp0(hdd_ref_raid_controller, ld_ref_raid_controller));

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop Presence fail(%d).", __func__, ret));
        continue_if_fail(1 == hdd_presence);

        
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ID, &hdd_id);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));
        continue_if_fail(hdd_id >= start_slot && hdd_id < hotspare_hdd_max_size && hdd_id < hdd_max_size &&
            0 == span_hotspare_hdd_slot_numbers[hdd_id]);
        

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_SLOT_NUMBER, &hdd_slot_number);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop SlotNumber fail(%d).", __func__, ret));

        
        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ENCLOSURE_DEVICE_ID,
            &hdd_enclosure_device_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop EnclosureDeviceId fail(%d).", __func__, ret));

        
        for (index_num = 0; index_num < slots_array_len && index_num < enclosures_array_len; index_num++) {
            if (hdd_slot_number == ld_participated_pd_slots_array[index_num] &&
                hdd_enclosure_device_id == ld_participated_pd_enclosures_array[index_num]) {
                break;
            }
        }

        
        continue_if_fail(index_num < slots_array_len && index_num < enclosures_array_len);

        
        get_oem_storage_drive_json_object((OBJ_HANDLE)obj_node->data, &hdd_jso);

        
        
        hdd_slot_numbers[hdd_id] = RECORD_FLAG;
        

        
        json_object_array_add(hdds_arr_jso, hdd_jso);
        hdd_jso = NULL;
    }

    
    goto_label_do_info_if_fail(json_object_array_length(hdds_arr_jso) > 0, exit, json_object_put(hdds_arr_jso);
        debug_log(DLOG_INFO, "%s: no physical drive associated with the logical drive directly.", __func__));
    json_object_object_add(ld_jso, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, hdds_arr_jso);

exit:
    
    do_if_expr(NULL != ld_participated_pd_slots, g_variant_unref(ld_participated_pd_slots));
    do_if_expr(NULL != ld_participated_pd_enclosures, g_variant_unref(ld_participated_pd_enclosures));
    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
}


LOCAL void get_oem_storage_physical_drive_for_hot_spare(OBJ_HANDLE logical_drive_obj_handle, json_object *hot_spare_jso,
    gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size)
{
    gint32 ret;
    gchar ld_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    GVariant *ld_dedicated_spared_pd_slots = NULL;
    const guchar *ld_dedicated_spared_pd_slots_array = NULL;
    guint32 slots_array_len;
    GVariant *ld_dedicated_spared_pd_enclosures = NULL;
    const guint16 *ld_dedicated_spared_pd_enclosures_array = NULL;
    guint32 enclosures_array_len;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint32 index_num = 0;
    gchar hdd_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    guint8 hdd_slot_number = 0;
    guint8 hdd_id_num = 0;
    guint8 hdd_presence = 0;
    guint16 hdd_enclosure_device_id = 0;
    json_object *hdd_jso = NULL;
    json_object *hdds_arr_jso = NULL;
    gsize parm_temp = 0;
    
    return_do_info_if_fail(NULL != hot_spare_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dal_get_property_value_string(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
        ld_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for LogicalDrive object fail(%d).", __func__, ret));

    
    ret = dfl_get_property_value(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_SLOT,
        &ld_dedicated_spared_pd_slots);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get prop DedicatedSparedPDSlots fail(%d).", __func__, ret));

    ld_dedicated_spared_pd_slots_array =
        (const guchar *)g_variant_get_fixed_array(ld_dedicated_spared_pd_slots, &parm_temp, sizeof(guchar));
    slots_array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != ld_dedicated_spared_pd_slots_array, exit,
        debug_log(DLOG_ERROR, "%s: get ld_dedicated_spared_pd_slots_array fail.", __func__));

    
    ret = dfl_get_property_value(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_ENCLOSURE,
        &ld_dedicated_spared_pd_enclosures);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get prop DedicatedSparedPDEnclosures fail(%d).", __func__, ret));

    ld_dedicated_spared_pd_enclosures_array =
        (const guint16 *)g_variant_get_fixed_array(ld_dedicated_spared_pd_enclosures, &parm_temp, sizeof(guint16));
    enclosures_array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != ld_dedicated_spared_pd_enclosures_array, exit,
        debug_log(DLOG_ERROR, "%s: get ld_dedicated_spared_pd_enclosures_array fail.", __func__));

    
    hdds_arr_jso = json_object_new_array();
    goto_label_do_info_if_fail(NULL != hdds_arr_jso, exit,
        debug_log(DLOG_ERROR, "%s: new hdds_arr_jso fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    goto_label_do_info_if_fail(VOS_OK == ret, exit, json_object_put(hdds_arr_jso);
        debug_log(DLOG_ERROR, "%s: failed to get object list of class Hdd(%d).", __func__, ret));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)memset_s(hdd_ref_raid_controller, sizeof(hdd_ref_raid_controller), 0, sizeof(hdd_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            hdd_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for Hdd object fail(%d).", __func__, ret));
        continue_if_fail(VOS_OK == g_strcmp0(hdd_ref_raid_controller, ld_ref_raid_controller));

        
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ID, &hdd_id_num);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));
        continue_if_fail(hdd_id_num < hotspare_hdd_max_size);
        

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop Presence fail(%d).", __func__, ret));
        continue_if_fail(1 == hdd_presence);

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_SLOT_NUMBER, &hdd_slot_number);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop SlotNumber fail(%d).", __func__, ret));

        
        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ENCLOSURE_DEVICE_ID,
            &hdd_enclosure_device_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop EnclosureDeviceId fail(%d).", __func__, ret));

        
        for (index_num = 0; index_num < slots_array_len && index_num < enclosures_array_len; index_num++) {
            if (hdd_slot_number == ld_dedicated_spared_pd_slots_array[index_num] &&
                hdd_enclosure_device_id == ld_dedicated_spared_pd_enclosures_array[index_num]) {
                break;
            }
        }

        
        continue_if_fail(index_num < slots_array_len && index_num < enclosures_array_len);

        
        get_oem_storage_drive_json_object((OBJ_HANDLE)obj_node->data, &hdd_jso);

        
        
        span_hotspare_hdd_slot_numbers[hdd_id_num] = RECORD_FLAG;
        

        
        json_object_array_add(hdds_arr_jso, hdd_jso);
        hdd_jso = NULL;
    }

    
    json_object_object_add(hot_spare_jso, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, hdds_arr_jso);

exit:
    
    do_if_expr(NULL != ld_dedicated_spared_pd_slots, g_variant_unref(ld_dedicated_spared_pd_slots));
    do_if_expr(NULL != ld_dedicated_spared_pd_enclosures, g_variant_unref(ld_dedicated_spared_pd_enclosures));
    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
}


LOCAL void get_oem_storage_hot_spare(OBJ_HANDLE logical_drive_obj_handle, json_object *ld_jso,
    gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size)
{
    gint32 ret;
    GVariant *ld_dedicated_spared_pd_slots = NULL;
    const guchar *ld_dedicated_spared_pd_slots_array = NULL;
    gsize slots_array_len = 0;
    json_object *hot_spare_jso = NULL;

    
    return_do_info_if_fail(NULL != ld_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dfl_get_property_value(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_DEDICATED_SPARED_PD_SLOT,
        &ld_dedicated_spared_pd_slots);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get prop DedicatedSparedPDSlots fail(%d).", __func__, ret));

    ld_dedicated_spared_pd_slots_array =
        (const guchar *)g_variant_get_fixed_array(ld_dedicated_spared_pd_slots, &slots_array_len, sizeof(guchar));
    return_do_info_if_fail(NULL != ld_dedicated_spared_pd_slots_array, g_variant_unref(ld_dedicated_spared_pd_slots);
        debug_log(DLOG_ERROR, "%s: get ld_dedicated_spared_pd_slots_array fail.", __func__));

    
    
    return_do_info_if_fail(0 != slots_array_len && 255 != ld_dedicated_spared_pd_slots_array[0],
        g_variant_unref(ld_dedicated_spared_pd_slots);
        ld_dedicated_spared_pd_slots = NULL;
        debug_log(DLOG_INFO, "%s: no hot spare associated with the logical drive.", __func__));
    
    g_variant_unref(ld_dedicated_spared_pd_slots);

    
    ret = get_oem_storage_component_json_object(RFPROP_SYSTEM_STORAGE_HOT_SPARE_NAME, NULL, &hot_spare_jso);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get hot_spare_jso fail(%d).", __func__, ret));

    
    get_oem_storage_physical_drive_for_hot_spare(logical_drive_obj_handle, hot_spare_jso,
        span_hotspare_hdd_slot_numbers, hotspare_hdd_max_size);

    
    json_object_object_add(ld_jso, RFPROP_SYSTEM_STORAGE_HOT_SPARE, hot_spare_jso);
}


LOCAL void get_oem_storage_physical_drive_for_span(OBJ_HANDLE disk_array_obj_handle, json_object *span_obj,
    gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size)
{
    gint32 ret;
    gchar disk_array_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    GVariant *disk_array_ref_pd_slots = NULL;
    const guchar *disk_array_ref_pd_slots_array = NULL;
    guint32 slots_array_len;
    GVariant *disk_array_ref_pd_enclosures = NULL;
    const guint16 *disk_array_ref_pd_enclosures_array = NULL;
    guint32 enclosures_array_len;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    guint32 index_num = 0;
    gchar hdd_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    guint8 hdd_presence = 0;
    guint8 hdd_slot_number = 0;
    guint8 hdd_id = 0;
    guint16 hdd_enclosure_device_id = 0;
    json_object *hdd_jso = NULL;
    json_object *hdds_arr_jso = NULL;
    gsize parm_temp = 0;

    
    return_do_info_if_fail(NULL != span_obj, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = dal_get_property_value_string(disk_array_obj_handle, PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
        disk_array_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for DiskArray object fail(%d).", __func__, ret));

    
    ret = dfl_get_property_value(disk_array_obj_handle, PROPERTY_DISK_ARRAY_REF_PD_SLOTS, &disk_array_ref_pd_slots);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get prop RefPDSlots fail(%d).", __func__, ret));

    disk_array_ref_pd_slots_array =
        (const guchar *)g_variant_get_fixed_array(disk_array_ref_pd_slots, &parm_temp, sizeof(guchar));
    slots_array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != disk_array_ref_pd_slots_array, exit,
        debug_log(DLOG_ERROR, "%s: get disk_array_ref_pd_slots_array fail.", __func__));

    
    goto_label_do_info_if_fail(0 != slots_array_len && 255 != disk_array_ref_pd_slots_array[0], exit,
        debug_log(DLOG_ERROR, "%s: prop RefPDSlots error.", __func__));

    
    ret = dfl_get_property_value(disk_array_obj_handle, PROPERTY_DISK_ARRAY_REF_PD_ENCLOSURES,
        &disk_array_ref_pd_enclosures);
    goto_label_do_info_if_fail(VOS_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get prop RefPDEnclosures fail(%d).", __func__, ret));

    disk_array_ref_pd_enclosures_array =
        (const guint16 *)g_variant_get_fixed_array(disk_array_ref_pd_enclosures, &parm_temp, sizeof(guint16));
    enclosures_array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != disk_array_ref_pd_enclosures_array, exit,
        debug_log(DLOG_ERROR, "%s: get disk_array_ref_pd_enclosures_array fail.", __func__));

    
    hdds_arr_jso = json_object_new_array();
    goto_label_do_info_if_fail(NULL != hdds_arr_jso, exit,
        debug_log(DLOG_ERROR, "%s: new hdds_arr_jso fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_HDD_NAME, &obj_list);
    
    goto_label_do_info_if_fail(VOS_OK == ret, exit, json_object_put(hdds_arr_jso);
        debug_log(DLOG_DEBUG, "%s: get object list of class Hdd fail(%d).", __func__, ret));
    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)memset_s(hdd_ref_raid_controller, sizeof(hdd_ref_raid_controller), 0, sizeof(hdd_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_REF_RAID_CONTROLLER,
            hdd_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for Hdd object fail(%d).", __func__, ret));
        continue_if_fail(VOS_OK == g_strcmp0(hdd_ref_raid_controller, disk_array_ref_raid_controller));

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_PRESENCE, &hdd_presence);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop Presence fail(%d).", __func__, ret));
        continue_if_fail(1 == hdd_presence);

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_SLOT_NUMBER, &hdd_slot_number);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop SlotNumber fail(%d).", __func__, ret));

        
        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ENCLOSURE_DEVICE_ID,
            &hdd_enclosure_device_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop EnclosureDeviceId fail(%d).", __func__, ret));

        
        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_HDD_ID, &hdd_id);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));
        continue_if_fail(hdd_id < hotspare_hdd_max_size);
        

        
        for (index_num = 0; index_num < slots_array_len && index_num < enclosures_array_len; index_num++) {
            if (hdd_slot_number == disk_array_ref_pd_slots_array[index_num] &&
                hdd_enclosure_device_id == disk_array_ref_pd_enclosures_array[index_num]) {
                break;
            }
        }

        
        continue_if_fail(index_num < slots_array_len && index_num < enclosures_array_len);

        
        get_oem_storage_drive_json_object((OBJ_HANDLE)obj_node->data, &hdd_jso);

        
        
        span_hotspare_hdd_slot_numbers[hdd_id] = RECORD_FLAG;
        

        
        json_object_array_add(hdds_arr_jso, hdd_jso);
        hdd_jso = NULL;
    }

    
    json_object_object_add(span_obj, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, hdds_arr_jso);

exit:
    
    do_if_expr(NULL != disk_array_ref_pd_slots, g_variant_unref(disk_array_ref_pd_slots));
    do_if_expr(NULL != disk_array_ref_pd_enclosures, g_variant_unref(disk_array_ref_pd_enclosures));
    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
}


LOCAL void get_oem_storage_spans(OBJ_HANDLE logical_drive_obj_handle, const gchar *ld_odata_id, json_object *ld_jso,
    gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size)
{
    int iRet = -1;
    guint8 ld_span_depth = 0;
    gchar ld_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    GVariant *ld_ref_array_list = NULL;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar disk_array_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    guint16 disk_array_id = 0;
    guint32 index_num = 0;
    gint32 span_num = 0;
    gchar span_name[STORAGE_CTRL_NAME_LENGTH + 1] = {0};
    json_object *span_jso = NULL;
    gsize parm_temp = 0;

    
    return_do_info_if_fail(NULL != ld_jso, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    gint32 ret =
        dal_get_property_value_byte(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_SPAN_DEPTH, &ld_span_depth);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop SpanDepth fail(%d).", __func__, ret));
    return_do_info_if_expr(ld_span_depth < 2 || ld_span_depth == 255, debug_log(DLOG_INFO,
        "%s: prop SpanDepth is %d, no spans associated with the logical drive.", __func__, ld_span_depth));

    
    ret = dal_get_property_value_string(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
        ld_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for LogicalDrive object fail(%d).", __func__, ret));

    
    ret = dfl_get_property_value(logical_drive_obj_handle, PROPERTY_LOGICAL_DRIVE_REF_ARRAY, &ld_ref_array_list);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop RefArray fail(%d).", __func__, ret));

    const guint16 *ld_ref_array =
        (const guint16 *)g_variant_get_fixed_array(ld_ref_array_list, &parm_temp, sizeof(guint16));
    guint32 array_len = (guint32)parm_temp;
    goto_label_do_info_if_fail(NULL != ld_ref_array, exit,
        debug_log(DLOG_ERROR, "%s: get ld_ref_array fail.", __func__));

    
    json_object *spans_arr_jso = json_object_new_array();
    goto_label_do_info_if_fail(NULL != spans_arr_jso, exit,
        debug_log(DLOG_ERROR, "%s: new spans_arr_jso fail.", __func__));

    
    ret = dal_get_array_list_by_ctrl(ld_ref_raid_controller, &obj_list);
    
    goto_label_do_info_if_fail(VOS_OK == ret, exit, json_object_put(spans_arr_jso);
        debug_log(DLOG_DEBUG, "%s: get object list of class DiskArray fail(%d).", __func__, ret));
    
    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)memset_s(disk_array_ref_raid_controller, sizeof(disk_array_ref_raid_controller), 0,
            sizeof(disk_array_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_DISK_ARRAY_REF_RAID_CONTROLLER,
            disk_array_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop RefRAIDController for DiskArray object fail(%d).", __func__, ret));
        continue_if_fail(VOS_OK == g_strcmp0(disk_array_ref_raid_controller, ld_ref_raid_controller));

        
        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_DISK_ARRAY_ID, &disk_array_id);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop ArrayId fail(%d).", __func__, ret));

        for (index_num = 0; index_num < array_len; index_num++) {
            if (disk_array_id == ld_ref_array[index_num]) {
                break;
            }
        }

        
        continue_if_fail(index_num < array_len);

        
        (void)memset_s(span_name, sizeof(span_name), 0, sizeof(span_name));
        iRet =
            snprintf_s(span_name, sizeof(span_name), sizeof(span_name) - 1, RFPROP_SYSTEM_STORAGE_SPAN_NAME, span_num);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
        span_num++;

        
        ret = get_oem_storage_component_json_object(span_name, ld_odata_id, &span_jso);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get span_jso fail(%d).", __func__, ret));

        
        get_oem_storage_physical_drive_for_span((OBJ_HANDLE)obj_node->data, span_jso, span_hotspare_hdd_slot_numbers,
            hotspare_hdd_max_size);

        
        json_object_array_add(spans_arr_jso, span_jso);
        span_jso = NULL;
    }

    
    json_object_object_add(ld_jso, RFPROP_VOLUME_DRIVE_SPANS, spans_arr_jso);

exit:
    
    do_if_expr(NULL != ld_ref_array_list, g_variant_unref(ld_ref_array_list));
    do_if_expr(NULL != obj_list, g_slist_free(obj_list));
}


LOCAL void get_oem_storage_logical_drive(OBJ_HANDLE raid_controller_obj_handle, json_object *raid_controller_jso,
    gint32 *span_hotspare_hdd_slot_numbers, gint32 hotspare_hdd_max_size, gint32 *hdd_slot_numbers, gint32 hdd_max_size)
{
    gint32 ret;
    gchar board_slot[MAX_RSC_ID_LEN + 1] = {0};
    const gchar *raid_controller_obj_name = NULL;
    guint8 raid_controller_id = 0;
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar ld_ref_raid_controller[STORAGE_CTRL_INFO_LENGTH + 1] = {0};
    gchar ld_name[STORAGE_CTRL_NAME_LENGTH + 1] = {0};
    guint16 ld_target_id = 0;
    gchar ld_odata_id[MAX_URI_LENGTH + 1] = {0};
    json_object *ld_jso = NULL;
    json_object *ld_arr_jso = NULL;
    guint8 ld_raid_level = 0;

    
    return_do_info_if_fail((NULL != raid_controller_jso) && (MAX_SLOT_NUMBER + 1 >= hotspare_hdd_max_size) &&
        (MAX_SLOT_NUMBER + 1 >= hdd_max_size),
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = redfish_get_board_slot(board_slot, sizeof(board_slot));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get board slot fail(%d).", __func__, ret));

    
    raid_controller_obj_name = dfl_get_object_name(raid_controller_obj_handle);
    return_do_info_if_fail(NULL != raid_controller_obj_name,
        debug_log(DLOG_ERROR, "%s: get name of RaidController object fail.", __func__));

    ret = dal_get_property_value_byte(raid_controller_obj_handle, PROPERTY_RAID_CONTROLLER_ID, &raid_controller_id);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));

    
    ld_arr_jso = json_object_new_array();
    return_do_info_if_fail(NULL != ld_arr_jso, debug_log(DLOG_ERROR, "%s: new ld_arr_jso fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_LOGICAL_DRIVE_NAME, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, json_object_put(ld_arr_jso);
        debug_log(DLOG_INFO, "%s: get object list of class LogicalDrive fail(%d).", __func__, ret));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_RAID_LEVEL,
            &ld_raid_level);
        continue_if_expr(RAID_LEVEL_DELETED == ld_raid_level);
        

        
        (void)memset_s(ld_ref_raid_controller, sizeof(ld_ref_raid_controller), 0, sizeof(ld_ref_raid_controller));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_REF_RAID_CONTROLLER,
            ld_ref_raid_controller, STORAGE_CTRL_INFO_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop RefRAIDController fail(%d).", __func__, ret));
        continue_if_fail(VOS_OK == g_strcmp0(ld_ref_raid_controller, raid_controller_obj_name));

        
        ret =
            dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_LOGICAL_DRIVE_TARGET_ID, &ld_target_id);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop TargetID fail(%d).", __func__, ret));

        (void)memset_s(ld_name, sizeof(ld_name), 0, sizeof(ld_name));
        int iRet = snprintf_s(ld_name, sizeof(ld_name), sizeof(ld_name) - 1, RFPROP_SYSTEM_STORAGE_LOGICAL_DRIVE_NAME,
            ld_target_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        (void)memset_s(ld_odata_id, sizeof(ld_odata_id), 0, sizeof(ld_odata_id));
        iRet = snprintf_s(ld_odata_id, MAX_URI_LENGTH + 1, MAX_URI_LENGTH, RFPROP_SYSTEM_REVOLUME, board_slot,
            raid_controller_id, ld_target_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        
        ret = get_oem_storage_component_json_object(ld_name, ld_odata_id, &ld_jso);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get logical_drive_jso fail(%d).", __func__, ret));

        
        get_oem_storage_spans((OBJ_HANDLE)obj_node->data, ld_odata_id, ld_jso, span_hotspare_hdd_slot_numbers,
            hotspare_hdd_max_size);
        get_oem_storage_hot_spare((OBJ_HANDLE)obj_node->data, ld_jso, span_hotspare_hdd_slot_numbers,
            hotspare_hdd_max_size);
        get_oem_storage_physical_drive_for_logical_drive((OBJ_HANDLE)obj_node->data, ld_jso,
            span_hotspare_hdd_slot_numbers, hotspare_hdd_max_size, hdd_slot_numbers, hdd_max_size);

        
        json_object_array_add(ld_arr_jso, ld_jso);
        ld_jso = NULL;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    
    return_do_info_if_fail(json_object_array_length(ld_arr_jso) > 0, json_object_put(ld_arr_jso);
        debug_log(DLOG_INFO, "%s: no logical drive associated with raid controller.", __func__));
    json_object_object_add(raid_controller_jso, RF_LINKS_VOLUMES, ld_arr_jso);
}


LOCAL void get_oem_storage_raid_controllers(json_object *summary_obj)
{
    int iRet = -1;
    gint32 ret;
    gchar board_slot[MAX_RSC_ID_LEN + 1] = {0};
    GSList *obj_list = NULL;
    GSList *obj_node = NULL;
    gchar raid_controller_name[STORAGE_CTRL_NAME_LENGTH + 1] = {0};
    guint8 raid_controller_id = 0;
    gchar odata_id[MAX_URI_LENGTH + 1] = {0};
    json_object *raid_controller_jso = NULL;
    json_object *raid_controllers_arr_jso = NULL;
    int hdd_slot_numbers[MAX_SLOT_NUMBER + 1] = {0};
    int span_hotspare_hdd_slot_numbers[MAX_SLOT_NUMBER + 1] = {0};

    
    return_do_info_if_fail(NULL != summary_obj, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    ret = redfish_get_board_slot(board_slot, sizeof(board_slot));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get board slot fail(%d).", __func__, ret));

    
    raid_controllers_arr_jso = json_object_new_array();
    return_do_info_if_fail(NULL != raid_controllers_arr_jso,
        debug_log(DLOG_ERROR, "%s: new raid_controllers_arr_jso fail.", __func__));

    
    ret = dfl_get_object_list(CLASS_RAID_CONTROLLER_NAME, &obj_list);
    return_do_info_if_fail(VOS_OK == ret, json_object_put(raid_controllers_arr_jso);
        debug_log(DLOG_INFO, "%s: get object list of class RaidController fail(%d).", __func__, ret));

    
    for (obj_node = obj_list; obj_node; obj_node = obj_node->next) {
        
        (void)memset_s(raid_controller_name, sizeof(raid_controller_name), 0, sizeof(raid_controller_name));
        ret = dal_get_property_value_string((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_COMPONENT_NAME,
            raid_controller_name, STORAGE_CTRL_NAME_LENGTH);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get prop ComponentName fail(%d).", __func__, ret));

        
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_RAID_CONTROLLER_ID, &raid_controller_id);
        continue_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: get prop Id fail(%d).", __func__, ret));

        (void)memset_s(odata_id, sizeof(odata_id), 0, sizeof(odata_id));
        iRet = snprintf_s(odata_id, MAX_URI_LENGTH + 1, MAX_URI_LENGTH, SYSTEMS_STORAGE_CONTROLLER_URI, board_slot,
            raid_controller_id);
        do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

        
        ret = get_oem_storage_component_json_object(raid_controller_name, odata_id, &raid_controller_jso);
        continue_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s: get raid_controller_jso fail(%d).", __func__, ret));

        
        get_storage_is_support_volume((OBJ_HANDLE)obj_node->data, raid_controller_jso);
        
        get_oem_storage_controller_mode((OBJ_HANDLE)obj_node->data, raid_controller_jso);
        
        
        (void)memset_s(span_hotspare_hdd_slot_numbers, sizeof(span_hotspare_hdd_slot_numbers), 0,
            sizeof(span_hotspare_hdd_slot_numbers));
        (void)memset_s(hdd_slot_numbers, sizeof(hdd_slot_numbers), 0, sizeof(hdd_slot_numbers));
        get_oem_storage_logical_drive((OBJ_HANDLE)obj_node->data, raid_controller_jso, span_hotspare_hdd_slot_numbers,
            MAX_SLOT_NUMBER + 1, hdd_slot_numbers, MAX_SLOT_NUMBER + 1);
        get_oem_storage_physical_drive_for_raid_controller((OBJ_HANDLE)obj_node->data, raid_controller_jso,
            span_hotspare_hdd_slot_numbers, MAX_SLOT_NUMBER + 1, hdd_slot_numbers, MAX_SLOT_NUMBER + 1);
        

        
        json_object_array_add(raid_controllers_arr_jso, raid_controller_jso);
        raid_controller_jso = NULL;
    }

    do_if_expr(NULL != obj_list, g_slist_free(obj_list));

    
    json_object_object_add(summary_obj, RFPROP_SYSTEM_STORAGE_RAID_CONTROLLERS, raid_controllers_arr_jso);
}


LOCAL void redfish_get_oem_storage_views_summary(json_object *o_result_jso)
{
    json_object *summary_obj = NULL;
    json_object *drives_arr_jso = NULL;

    
    return_do_info_if_fail(o_result_jso != NULL, debug_log(DLOG_ERROR, "%s: Input param error.", __func__));

    
    summary_obj = json_object_new_object();
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_STORAGE_VIEWS_SUMMARY, summary_obj);

    
    get_oem_storage_raid_controllers(summary_obj);

    
    drives_arr_jso = json_object_new_array();
    return_do_info_if_fail(drives_arr_jso != NULL,
        debug_log(DLOG_ERROR, "%s: new json array as drives_arr_jso fail.", __FUNCTION__));

    get_oem_storage_physical_drive_for_storage(drives_arr_jso);
    get_oem_storage_pcie_drive_for_storage(drives_arr_jso);
    if (json_object_array_length(drives_arr_jso) > 0) {
        json_object_object_add(summary_obj, RFPROP_VOLUME_DRIVE_SPAN_DRIVES, drives_arr_jso);
    } else {
        json_object_put(drives_arr_jso);
    }
    
}

LOCAL void redfish_sys_oem_get_post_state(json_object *oem_json)
{
    gint32 ret;
    json_object *post_state = NULL;
    const char *post_state_str = NULL;
    OBJ_HANDLE bios = INVALID_OBJ_HANDLE;
    OBJ_HANDLE payload = INVALID_OBJ_HANDLE;
    guchar bios_status = 0;
    guchar boot_stage = 0;
    guchar power_state = 0;

    ret = dfl_get_object_handle(OBJ_NAME_BIOS, &bios);
    if (ret != DFL_OK) { 
        goto ERR_OUT;
    }

    ret = dal_get_property_value_byte(bios, PROPERTY_BIOS_STARTUP_STATE, &bios_status);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Bios get SystemStartupState failed[%d]", __FUNCTION__, ret);
        goto ERR_OUT;
    }
    if (bios_status == BIOS_STARTUP_POST_STAGE_FINISH) {
        post_state_str = RF_SYSTEM_POST_STATE_FINISHED;
        goto OUT;
    }

    ret = dal_get_property_value_byte(bios, PROPERTY_BIOS_BOOT_STAGE, &boot_stage);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Bios get BiosBootStage failed[%d]", __FUNCTION__, ret);
        goto ERR_OUT;
    }
    if (boot_stage == BIOS_BOOT_STAGE_PCIE_INFO_REPORTED) {
        post_state_str = RF_SYSTEM_POST_STATE_DISC_COMPLETE;
        goto OUT;
    }

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s dal_get_object_handle_nth(%s) failed[%d]", __FUNCTION__, CLASS_PAYLOAD, ret);
        goto ERR_OUT;
    }
    ret = dal_get_property_value_byte(payload, PROPERTY_PAYLOAD_CHASSPWR_STATE, &power_state);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s Payload get ChassisPowerState failed[%d]", __FUNCTION__, ret);
        goto ERR_OUT;
    }
    post_state_str = (power_state == HARD_POWER_ON) ? RF_SYSTEM_POST_STATE_IN_POST : RF_SYSTEM_POST_STATE_POWER_OFF;

OUT:
    post_state = json_object_new_string(post_state_str);
    if (post_state == NULL) {
        debug_log(DLOG_ERROR, "%s json_object_new_string %s failed", __FUNCTION__, post_state_str);
    }

ERR_OUT:
    json_object_object_add(oem_json, RFPROP_SYSTEM_OEM_POST_STATE, post_state);
}


LOCAL void get_manager_oem_kvm_enable(json_object **oem_property)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret;
    guchar kvm_state = 0;

    
    ret = dal_get_object_handle_nth(PROXY_KVM_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s dal_get_object_handle_nth fail. ret = %d", __FUNCTION__, ret);
        return;
    }

    ret = dal_get_property_value_byte(obj_handle, KVM_PROPERTY_LOCAL_KVM_STATE, &kvm_state);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s dal_get_property_value_string fail.", __FUNCTION__));

    if ((1 == kvm_state) || (0 == kvm_state)) {
        *oem_property = json_object_new_boolean(kvm_state);
        do_val_if_fail(NULL != *oem_property,
            debug_log(DLOG_ERROR, "%s dal_get_property_value_string fail.", __FUNCTION__));
    } else {
        
        debug_log(DLOG_DEBUG, "%s kvm_state value is out of range.", __FUNCTION__);
        
    }

    return;
}


LOCAL void redfish_get_boot_mode_sw_disp(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *js_bt_md_sw_disp = NULL;
    guint8 bt_md_sw_disp = 0;

    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: Input param error.", __func__);
        return;
    }
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios object handle failed", __FUNCTION__);
        goto EXIT;
    }
    ret = dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SW_ENABLE, &bt_md_sw_disp);
    if (ret != DFL_OK) {
        debug_log(DLOG_DEBUG, "%s: get Bios mode switch display status failed", __FUNCTION__);
        goto EXIT;
    }
    if (bt_md_sw_disp != BIOS_BOOT_MODE_SW_ENABLE_ON && bt_md_sw_disp != BIOS_BOOT_MODE_SW_ENABLE_OFF) {
        debug_log(DLOG_DEBUG, "%s: invalid Bios mode switch display status: %d", __FUNCTION__, bt_md_sw_disp);
        goto EXIT;
    }
    js_bt_md_sw_disp = json_object_new_boolean((gint32)bt_md_sw_disp);
EXIT:
    json_object_object_add(o_result_jso, RFPROP_BIOS_BT_MD_IPMI_SW_DISP, js_bt_md_sw_disp);
}


LOCAL void redfish_get_boot_mode_switch(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *boot_mode_swi_jso = NULL;
    guint8 boot_mode_swi = 0;

    if (NULL == o_result_jso) {
        return;
    }

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    goto_label_do_info_if_fail(DFL_OK == ret, exit,
        debug_log(DLOG_DEBUG, "%s: get Bios object handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SW, &boot_mode_swi);
    goto_label_do_info_if_fail(DFL_OK == ret, exit,
        debug_log(DLOG_ERROR, "%s: get Bios mode switch fail", __FUNCTION__));

    goto_label_do_info_if_expr((BIOS_BOOT_MODE_SW_ON != boot_mode_swi) && (BIOS_BOOT_MODE_SW_OFF != boot_mode_swi),
        exit, debug_log(DLOG_DEBUG, "%s: invalid Bios mode switch value is %d", __FUNCTION__, boot_mode_swi));

    boot_mode_swi_jso = json_object_new_boolean((gint32)boot_mode_swi);

exit:

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_BOOT_MODE_CHANGE_ENABLED, boot_mode_swi_jso);
}


LOCAL gint32 get_oem_odata_id_object(const gchar *url_prefix, const gchar *resc_name, json_object *huawei)
{
    int iRet;
    gint32 ret;
    gchar sys_url[MAX_URL_LEN] = {0};
    gchar slot_id[MAX_RSC_ID_LEN] = {0};
    json_object *resc_obj = NULL;

    if (NULL == url_prefix || NULL == resc_name || NULL == huawei) {
        debug_log(DLOG_ERROR, "%s: null pointer", __FUNCTION__);
        return VOS_ERR;
    }

    ret = redfish_get_board_slot(slot_id, sizeof(slot_id));
    if (VOS_OK != ret) {
        debug_log(DLOG_ERROR, "%s: get slot id failed", __FUNCTION__);
        return VOS_ERR;
    }

    iRet = snprintf_s(sys_url, sizeof(sys_url), sizeof(sys_url) - 1, "%s%s/%s", url_prefix, slot_id, resc_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    resc_obj = json_object_new_object();
    if (NULL == resc_obj) {
        debug_log(DLOG_ERROR, "%s: alloc resc obj memory failed", __FUNCTION__);
        return VOS_ERR;
    }

    json_object_object_add(huawei, resc_name, resc_obj);
    json_object_object_add(resc_obj, ODATA_ID, json_object_new_string(sys_url));

    return VOS_OK;
}


LOCAL void rf_add_mem_free(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guint32 ret;
    gdouble total_mem = 0;
    gdouble MemUsage = 0;
    gdouble SystemMemoryFreeGiB;

    return_do_info_if_expr((NULL == o_result_jso),
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE, NULL);
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));
    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE, NULL);
        return;
    }

    
    ret = dal_get_property_value_double(obj_handle, PROPERTY_COMPUTER_SYSTEM_TOTALSYSMEM, &total_mem);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    ret = dal_get_property_value_double(obj_handle, PROPERTY_COMPUTER_SYSTEM_MEMUSAGE, &MemUsage);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));
    return_do_info_if_expr(G_MAXUINT16 == MemUsage,
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE, NULL));
    // 计算剩余内存Free
    SystemMemoryFreeGiB = total_mem * (1 - MemUsage / 100);
    
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE,
        ex_json_object_new_double(SystemMemoryFreeGiB, "%.2f"));
}


LOCAL const gchar *rf_hs_2_powerstate(guint8 hs)
{
    const gchar *power_state = NULL;

    switch (hs) {
        case FRU_HS_STATE_M1:
        case FRU_HS_STATE_M2: // 上电请求
            power_state = RFPROP_VALUE_SYSTEM_POWER_OFF;
            break;

        case FRU_HS_STATE_M3:
            power_state = RFPROP_VALUE_SYSTEM_POWERING_ON;
            break;

        case FRU_HS_STATE_M4:
        case FRU_HS_STATE_M5: // 下电请求
            power_state = RFPROP_VALUE_SYSTEM_POWER_ON;
            break;

        case FRU_HS_STATE_M6:
            power_state = RFPROP_VALUE_SYSTEM_POWERING_OFF;
            break;

        case FRU_HS_STATE_M0: // 未插入
        // case FRU_HOTSWAP_M7://通信丢失
        default:
            break;
    }

    return power_state;
}


LOCAL const gchar *rf_fruid_2_type(guint8 fruid)
{
    const gchar *fru_type = NULL;

    switch (fruid) {
        case OS_FRU_ID:
            fru_type = RFPROP_VALUE_SYSTEM_OS;
            break;

        case BASE_FRU_ID:
            fru_type = RFPROP_SYSTEM_SWITCH_BASE;
            break;

        case FABRIC_FRU_ID:
            fru_type = RFPROP_SYSTEM_SWITCH_FABRIC;
            break;

        case FC_FRU_ID:
            fru_type = RFPROP_SYSTEM_SWITCH_FC;
            break;

        default:
            debug_log(DLOG_DEBUG, "%s:invalid fru id is %d", __FUNCTION__, fruid);
    }

    return fru_type;
}


LOCAL gint32 get_each_hotswap(OBJ_HANDLE object_handle, gpointer user_data)
{
    json_object *present_fru_jso = (json_object *)user_data;
    guint8 current_state = G_MAXUINT8;
    guint8 fru_id = 0;
    json_object *pf_jso_object = NULL;
    const gchar *power_state = NULL;
    json_object *power_state_jso = NULL;
    json_object *fru_type_jso = NULL;
    const gchar *fru_type = NULL;

    return_val_if_expr(NULL == present_fru_jso, VOS_OK);

    (void)dal_get_property_value_byte(object_handle, PROPERTY_HS_CRU_STATE, &current_state);

    if (current_state >= FRU_HS_STATE_M1 && current_state <= FRU_HS_STATE_M6) {
        pf_jso_object = json_object_new_object();
        return_val_if_expr(NULL == pf_jso_object, VOS_OK);

        (void)dal_get_property_value_byte(object_handle, PROPERTY_HS_FRUID, &fru_id);
        fru_type = rf_fruid_2_type(fru_id);
        if (NULL != fru_type) {
            fru_type_jso = json_object_new_string(fru_type);
        }
        json_object_object_add(pf_jso_object, RFPROP_SYSTEM_FRU_TYPE, fru_type_jso);

        power_state = rf_hs_2_powerstate(current_state);
        if (NULL != power_state) {
            power_state_jso = json_object_new_string(power_state);
        }
        json_object_object_add(pf_jso_object, RFPROP_SYSTEM_POWERSTATE, power_state_jso);

        json_object_array_add(present_fru_jso, pf_jso_object);
    }

    return VOS_OK;
}


LOCAL void redfish_get_present_fru(json_object *huawei)
{
    json_object *present_fru_jso = NULL;
    gint32 ret_val = 0;

    return_if_expr(NULL == huawei);

    present_fru_jso = json_object_new_array();
    if (NULL == present_fru_jso) {
        goto exit;
    }

    ret_val = dfl_foreach_object(CLASS_HOTSWAP, get_each_hotswap, present_fru_jso, NULL);
    do_if_fail(DFL_OK == ret_val, debug_log(DLOG_DEBUG, "%s: dfl_foreach_object fail", __FUNCTION__));

exit:
    json_object_object_add(huawei, RFPROP_SYSTEM_PRESENT_FRUS, present_fru_jso);
}


LOCAL gint32 get_wol_supported(void)
{
    OBJ_HANDLE pme_service_obj = 0;
    guint8 wol_conf_state = G_MAXUINT8;

    (void)dal_get_object_handle_nth(CLASS_NAME_PME_SERVICECONFIG, 0, &pme_service_obj);
    (void)dal_get_property_value_byte(pme_service_obj, PROTERY_PME_SERVICECONFIG_WOL, &wol_conf_state);

    return_val_if_fail(ENABLED == wol_conf_state, VOS_ERR);

    return VOS_OK;
}


LOCAL void get_wol_status(OBJ_HANDLE obj_handle, json_object *huawei)
{
    json_object *wol_state = NULL;
    guint8 wol_status = G_MAXUINT8;
    gint32 ret;

    ret = get_wol_supported();
    if (VOS_OK != ret) {
        goto exit;
    }

    (void)dal_get_property_value_byte(obj_handle, PROPERTY_WOL_STATE, &wol_status);
    if (ENABLED != wol_status && DISABLED != wol_status) {
        goto exit;
    }

    wol_state = json_object_new_boolean(wol_status);
exit:
    json_object_object_add(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_WOL_STATUS, wol_state);
}


LOCAL gint32 get_system_huawei_info(OBJ_HANDLE obj_handle, const gchar *property_name, gchar *out_val, guint32 len,
    json_object *o_result_jso, const gchar *resource_name)
{
    gint32 ret;
    json_object *result_jso = NULL;

    ret = dal_get_property_value_string(obj_handle, property_name, out_val, len);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "get_system_info: get %s.%s failed, ret:%d", dfl_get_object_name(obj_handle),
            property_name, ret));
    
    if (rf_string_check(out_val) == VOS_OK) {
        
        result_jso = json_object_new_string(out_val);
        do_if_expr(result_jso == NULL, debug_log(DLOG_ERROR, "get_system_info: json_object_new_string failed"));
    }
    json_object_object_add(o_result_jso, resource_name, result_jso);
    return VOS_OK;
}

LOCAL void _rf_add_system_health_summary_info(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE warning_handle = OBJ_HANDLE_NULL;
    json_object *health_summary_jso = NULL;

    ret = dal_get_object_handle_nth(CLASS_NAME_WARNING, 0, &warning_handle);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_DEBUG,
        "%s %d: cann't get object handle for class [%s], ret is %d", __FUNCTION__, __LINE__, CLASS_NAME_WARNING, ret));

    health_summary_jso = json_object_new_object();
    return_do_info_if_fail(NULL != health_summary_jso,
        debug_log(DLOG_ERROR, "%s %d: cann't alloc new json object", __FUNCTION__, __LINE__));

    rf_add_property_jso_uint32(warning_handle, PROTERY_WARNING_CRITICAL, RFPROP_SYSTEM_CRITICAL_ALARM_CNT,
        health_summary_jso);
    rf_add_property_jso_uint32(warning_handle, PROTERY_WARNING_MAJOR, RFPROP_SYSTEM_MAJOR_ALARM_CNT,
        health_summary_jso);
    rf_add_property_jso_uint32(warning_handle, PROTERY_WARNING_MINOR, RFPROP_SYSTEM_MINOR_ALARM_CNT,
        health_summary_jso);

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_HEALTH_SUMMARY, health_summary_jso);

    return;
}

gint32 get_bma_connect_state(PROVIDER_PARAS_S *i_paras)

{
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guchar bma_connect_state = SMS_BMA_GLOBAL_STATE_NOTOK;

    gint32 ret = dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    return_val_if_expr(VOS_OK != ret, VOS_ERR);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SMS, METHOD_SMS_GET_BMA_CONNECT_STATE, AUTH_DISABLE, i_paras->user_role_privilege, NULL, &output_list);
    if (VOS_OK == ret) {
        bma_connect_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
    }

    return_val_do_info_if_expr(SMS_BMA_GLOBAL_STATE_NOTOK == bma_connect_state, VOS_ERR,
        debug_log(DLOG_INFO, "%s, %d: me type is SMbus-PECI and BMA unconnected.", __FUNCTION__, __LINE__));

    return VOS_OK;
}


LOCAL gint32 get_system_huawei_payload(json_object *o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    do_if_fail(ret == VOS_OK, debug_log(DLOG_DEBUG, "%d: 'get object handle fail.'\n", __LINE__));

    
    get_oem_prop_powerofftimeoutenabled(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_EN,
        RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_ENABLED, o_result_jso);
    rf_add_property_jso_uint32(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM, RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT,
        o_result_jso);
    rf_add_property_jso_uint32(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_DEF,
        RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_DEF, o_result_jso);
    rf_add_property_jso_uint32(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_MAX,
        RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_MAX, o_result_jso);
    rf_add_property_jso_uint32(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_MIN,
        RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_MIN, o_result_jso);

    
    get_oem_prop_panelpowerbuttonenabled(obj_handle, PROPERTY_PAYLOAD_PWR_BTN_LOCK,
        RFPROP_SYSTEM_COMPUTERSYSTEM_PANELPOWERBUTTONENABLED, o_result_jso);
    
    get_oem_prop_powerrestorepolicy(obj_handle, PROPERTY_POWER_REST_POLICY,
        RFPROP_SYSTEM_COMPUTERSYSTEM_SYSTEMSTATEAFTERPOWERSUPPLIED, o_result_jso);
    
    get_oem_prop_powerontrategyexceptions(obj_handle, PROPTERY_POWER_ON_STRATEGY_EXCEPTIONS,
        RFPROP_SYSTEM_COMPUTERSYSTEM_POWERONSTRATEGYEXCEPTIONS, o_result_jso);
    // 获取延迟上电配置
    ret = get_power_delay_mode_and_count(obj_handle, o_result_jso);
    do_if_fail(ret == VOS_OK, debug_log(DLOG_DEBUG, "get_power_delay_mode_and_count fail."));
    return VOS_OK;
}


LOCAL gint32 get_kernel_os_version(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso, OBJ_HANDLE *obj_handle)
{
    gint32 ret;
    gchar version_string[RF_SYSTEM_OSVERSION_MAXLEN];       // osversion 值
    gchar kernalversion_string[RF_SYSTEM_OSVERSION_MAXLEN]; // kernalversion 值
    json_object *enable_value_result_jso = NULL;
    guchar enable_value;

    return_val_if_expr(i_paras == NULL || o_result_jso == NULL, VOS_ERR);

    // 变量初始化
    memset_s(version_string, sizeof(version_string), 0, sizeof(version_string)); // 字符数组初始化
    memset_s(kernalversion_string, sizeof(kernalversion_string), 0, sizeof(kernalversion_string)); // 字符数组初始化

    

    // 获取类"ComputerSystem" 对象的句柄
    // COMPUTERSYSTEM_CLASS_NAME
    ret = dal_get_object_handle_nth(g_system_provider[i_paras->index].pme_class_name, 0, obj_handle);
    if (ret == VOS_OK) {
        // 获取类对象中属性"OSVersion"的值
        // "OSVersion"的的宏定义为:  PROPERTY_COMPUTER_SYSTEM_OSVERSION
        ret = get_system_huawei_info(*obj_handle, PROPERTY_COMPUTER_SYSTEM_OSVERSION, version_string,
            RF_SYSTEM_OSVERSION_MAXLEN, o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_OSVERSION);
        return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
            debug_log(DLOG_ERROR, "%d: 'get property value fail.'\n", __LINE__));

        // 获取类对象中属性"KernalVersion"的值
        // "KernalVersion" 的的宏定义为:  PROPERTY_COMPUTER_SYSTEM_KERNEL_VERSION
        ret = get_system_huawei_info(*obj_handle, PROPERTY_COMPUTER_SYSTEM_KERNEL_VERSION, kernalversion_string,
            RF_SYSTEM_OSVERSION_MAXLEN, o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_KERNALVERSION);
        return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%d: 'get property value fail.'\n", __LINE__));
        
        ret = dal_get_property_value_byte(*obj_handle, PROPERTY_COMPUTER_SYSTEM_SYNC_ENABLE, &enable_value);
        return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
            debug_log(DLOG_ERROR, "%d: 'get property %s value fail.'\n", __LINE__,
            PROPERTY_COMPUTER_SYSTEM_SYNC_ENABLE));

        
        enable_value_result_jso = json_object_new_boolean(enable_value);
        do_if_expr(enable_value_result_jso == NULL,
            debug_log(DLOG_ERROR, "%d 'json_object_new_string fail' ", __LINE__));
    }

    
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_SYNCENABLE, enable_value_result_jso);
    

    return VOS_OK;
}


LOCAL void get_oem_sms_property(json_object *o_result_jso)
{
    OBJ_HANDLE sms_handle = 0; // 类ComputerSystem 的对象的句柄

    
    (void)dal_get_object_handle_nth(CLASS_SMS, 0, &sms_handle);
    rf_add_property_jso_string(sms_handle, PROPERTY_SMS_FIRMWAREVERSION, RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_SERVICE,
        o_result_jso);
    rf_add_property_jso_string(sms_handle, PROPERTY_SMS_STATUS, RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_STATUS, o_result_jso);
    rf_add_property_jso_string(sms_handle, PROPERTY_SMS_VNICDEIVEVERSION, RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_DRIVER,
        o_result_jso);
    rf_add_property_jso_bool(sms_handle, PROPERTY_SMS_ENABLED, RFPROP_SYSTEM_COMPUTERSYSTEM_SMS_ENABLED, o_result_jso);

    return;
}

LOCAL void get_url_if_x86_enable(json_object *o_result_jso)
{
    gint32 ret;
    guchar x86_enable = 0;

    if (o_result_jso == NULL) {
        return;
    }

    ret = redfish_get_x86_enable_type(&x86_enable);
    if (ret != VOS_OK || x86_enable == DISABLE) {
        return;
    }

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_IB, o_result_jso);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get ib url failed", __FUNCTION__));

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_NB, o_result_jso);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get bond url failed", __FUNCTION__));

    
    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_NBRIDGE, o_result_jso);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get bridge url failed", __FUNCTION__));
    

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_PROCESSORSIMPLEVIEW, o_result_jso);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get process view url failed", __FUNCTION__));

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_MEMORYSIMPLEVIEW, o_result_jso);
    do_if_expr(ret != VOS_OK, debug_log(DLOG_ERROR, "%s: get memory view url failed", __FUNCTION__));
}

LOCAL void get_url_if_frso_enable(json_object *o_result_jso)
{
    gint32 ret;
    if (o_result_jso == NULL) {
        return;
    }
    if (is_force_restart_enabled() != TRUE) {
        return;
    }

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, "forcerestartservice", o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get %s url failed", __FUNCTION__, "forcerestartservice");
    }
}


LOCAL void get_system_cpu_total_cores(json_object* o_result_jso)
{
    gint32 ret;
    GSList* obj_list = NULL;
    GSList* obj_node = NULL;
    guchar cpu_id;
    guchar cpu_presence;
    guint16 cpu_core_quantity;
    guint32 cpu_total_cores = 0;

    ret = dfl_get_object_list(CLASS_CPU, &obj_list);
    if (ret != VOS_OK || g_slist_length(obj_list) == 0) {
        debug_log(DLOG_DEBUG, "%s: get CPU obj_list failed", __FUNCTION__);
        return;
    }

    for (obj_node = obj_list; obj_node != NULL; obj_node = obj_node->next) {
        cpu_id = 0;
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_PHYSIC_ID, &cpu_id);
        if (ret != VOS_OK || cpu_id == 0) {
            debug_log(DLOG_ERROR, "%s: get cpu object list failed", __FUNCTION__);
            continue;
        }
        
        cpu_presence = 0;
        ret = dal_get_property_value_byte((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_PRESENCE, &cpu_presence);
        if (ret != VOS_OK || cpu_presence == 0) {
            debug_log(DLOG_ERROR, "%s: get cpu presence failed", __FUNCTION__);
            continue;
        }
        
        cpu_core_quantity = 0;
        ret = dal_get_property_value_uint16((OBJ_HANDLE)obj_node->data, PROPERTY_CPU_CORE_COUNT, &cpu_core_quantity);
        if (ret != VOS_OK) {
            debug_log(DLOG_ERROR, "%s: get cpu core quantity failed", __FUNCTION__);
            continue;
        }
        cpu_total_cores = cpu_total_cores + cpu_core_quantity;
    }

    g_slist_free(obj_list);
    json_object* cpu_total_cores_jso = NULL;
    cpu_total_cores_jso = json_object_new_int(cpu_total_cores);
    if (cpu_total_cores_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_int failed", __FUNCTION__);
    }
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTOTALCORES, cpu_total_cores_jso);
    return;
}

LOCAL void get_config_model(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    if (dal_is_customized_by_cmcc() || dal_is_customized_by_cucc()) {
        rf_add_property_jso_string(obj_handle, PROPERTY_PRODUCT_CONFIG_MODEL,
            RFPROP_SYSTEM_COMPUTERSYSTEM_CONFIGMODEL, o_result_jso);
    }
}

LOCAL void get_cmcc_version(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    if (dal_is_customized_by_cmcc()) {
        rf_add_property_jso_string(obj_handle, PROPERTY_PRODUCT_CMCC_VERSION,
            RFPROP_SYSTEM_COMPUTERSYSTEM_CMCC_VERSION, o_result_jso);
    }
}

LOCAL void get_cucc_version(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    if (dal_is_customized_by_cucc()) {
        rf_add_property_jso_string(obj_handle, PROPERTY_PRODUCT_CUCC_VERSION,
            RFPROP_SYSTEM_COMPUTERSYSTEM_CUCC_VERSION, o_result_jso);
    }
}

LOCAL void get_process_memory_history(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
    gint32 ret;

    ret = get_process_memory_history_supported(i_paras);
    if (ret != VOS_OK) {
        return;
    }

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_PROCESSORS_HISTORY_USAGE_RATE, o_result_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_INFO, "%s:get ProcessorsHistoryUsageRate url failed", __FUNCTION__);
    }

    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_MEMORY_HISTORY_USAGE_RATE, o_result_jso);
    if (ret != VOS_OK) {
        debug_log(DLOG_INFO, "%s:get MemoryHistoryUsageRate url failed", __FUNCTION__);
    }
}

LOCAL void add_npu_ability_type_value_json_object(json_object* o_result_jso, const gchar* type, 
    gdouble value)
{
    gint32 ret;
    json_object* js_type_value = NULL;
    json_object* js_type = NULL;
    json_object* js_value = NULL;

    js_type = json_object_new_string(type);
    if (js_type == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail.", __FUNCTION__);
        return;
    }
    js_value = ex_json_object_new_double(value, ACCURACY_FORMAT_3F);
    if (js_value == NULL) {
        debug_log(DLOG_ERROR, "%s: ex_json_object_new_double fail.", __FUNCTION__);
        json_object_put(js_type);
        return;
    }
    js_type_value = json_object_new_object();
    if (js_type_value == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail.", __FUNCTION__);
        json_object_put(js_type);
        json_object_put(js_value);
        return;
    }
	
    json_object_object_add(js_type_value, RFPROP_NPU_ABILITY_TYPE, js_type);
    json_object_object_add(js_type_value, RFPROP_NPU_ABILITY_VALUE, js_value);
        
    ret = json_object_array_add(o_result_jso, js_type_value);
    if (ret != RET_OK) {
        (void)json_object_put(js_type_value);
        debug_log(DLOG_ERROR, "%s: json array add fail.", __FUNCTION__);
    }
}
LOCAL void get_npu_ability_type_value(json_object* o_result_jso, OBJ_HANDLE obj_handle)
{
    gint32 ret;
    gchar **str_array = NULL;
    gsize str_array_len;
    guint32 i;
    gdouble value_arry[NPU_ABILITY_ARRAY_NUM] = {0};
    gsize   value_arry_len;

    ret = dal_get_property_value_strv(obj_handle, PROPERTY_NPU_ABILITY_TYPE_ARRAY, &str_array, &str_array_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get NpuAbility.TypeArray fail", __FUNCTION__);
        return;
    }
    ret = dal_get_property_value_doublev(obj_handle, PROPERTY_NPU_ABILITY_VALUE_ARRAY, value_arry,
        sizeof(value_arry), &value_arry_len);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get NpuAbility.ValueArray fail", __FUNCTION__);
        g_strfreev(str_array);
        return;
    }

    for (i = 0; i < str_array_len && i < value_arry_len; i++) {
        size_t type_len = strlen(str_array[i]);
        if (type_len == 0 || type_len >= NPU_ABILITY_TYPE_LENGTH) {
            continue;
        }
        add_npu_ability_type_value_json_object(o_result_jso, str_array[i], value_arry[i]);
    }
    g_strfreev(str_array);
}

LOCAL void redfish_get_npu_ability(json_object* o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle;
    json_object* js_ai_npu_ability = NULL;

    if (o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__);
        return;
    }

    ret = dal_get_object_handle_nth(CLASS_NPU_ABILITY, 0, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s: get NpuAbility object handle failed", __FUNCTION__);
        return;
    }

    js_ai_npu_ability = json_object_new_array();
    if (js_ai_npu_ability != NULL) {
        get_npu_ability_type_value(js_ai_npu_ability, obj_handle);
    } else {
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__);
    }

    json_object_object_add(o_result_jso, RFPROP_NPU_ABILITY, js_ai_npu_ability);
}


LOCAL void get_system_product_bmc_bios(json_object* o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0,  &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle fail");
    }
    get_wol_status(obj_handle, o_result_jso);
    
    get_slot_bit_map(obj_handle, o_result_jso);
    get_config_model(obj_handle, o_result_jso);
    get_cmcc_version(obj_handle, o_result_jso);
    get_cucc_version(obj_handle, o_result_jso);
    
    rf_add_property_jso_string(obj_handle, PROPERTY_PRODUCT_ALIAS, RFPROP_SYSTEM_COMPUTERSYSTEM_PRODUCTALIAS,
        o_result_jso);
    
    rf_add_property_jso_string(obj_handle, BMC_PRODUCT_VER_NAME, RFPROP_SYSTEM_COMPUTERSYSTEM_PRODUCTVERSION,
        o_result_jso);
    
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0,  &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get object handle fail");
    }
    rf_add_property_jso_string(obj_handle, PROPERTY_BMC_SYSTEMNAME, RFPROP_SERVICEROOT_PRODUCT_NAME, o_result_jso);
    rf_add_property_jso_string(obj_handle, PROPERTY_BMC_DEV_SERIAL, RFPROP_SYSTEM_DEV_SERIALNUMBER, o_result_jso);

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0,  &obj_handle);
    if (ret == RET_OK) {
        rf_add_property_jso_string(obj_handle, BMC_TEEOS_VER_NAME, RFPROP_SYSTEM_TEEOSVERSION, o_result_jso);
    }
}


LOCAL void get_system_chassis(json_object* o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar prop_val = 0;
    guchar max_node_index = 0;
    guchar mgmt_software_type = 0xFF;

    
    ret = dal_get_object_handle_nth(CLASS_NAME_CHASSIS, 0,  &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get object handle fail.");
    }

    
    rf_add_property_jso_string(obj_handle, PROPERTY_CHASSIS_OWNER_ID, RFPROP_SYSTEM_COMPUTERSYSTEM_DEVICEOWNERID, 
        o_result_jso);

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_MAX_NODE_INDEX, &max_node_index);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get property value fail.");
    }

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_CHASSIS_NODE_INDEX, &prop_val);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "get property value fail.");
    }

     
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_DEVICESLOTID, 
        (max_node_index != 0) && (prop_val > max_node_index) ? NULL : json_object_new_int(prop_val));
    
    (void)dal_get_software_type(&mgmt_software_type);
    if ((mgmt_software_type == MGMT_SOFTWARE_TYPE_BLADE) || (mgmt_software_type == MGMT_SOFTWARE_TYPE_CHASSIS_SINGLE)) {
        
        ret = dal_get_object_handle_nth(CLASS_ELABEL, 0,  &obj_handle);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: get object handle fail, ret=%d.", __FUNCTION__, ret);
        }

        if (mgmt_software_type == MGMT_SOFTWARE_TYPE_BLADE) {
            
            rf_add_property_jso_string(obj_handle, PROPERTY_ELABEL_CHASSIS_SN, RFPROP_ENCLOSURE_SERIAL_NUM, 
                o_result_jso);
        } else {
            
            rf_add_property_jso_string(obj_handle, PROPERTY_ELABEL_PRODUCT_SN, RFPROP_ENCLOSURE_SERIAL_NUM, 
                o_result_jso);
        }
    }

    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0,  &obj_handle);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: get object handle failed, ret=%d", __FUNCTION__, ret);
    }
    
    rf_add_property_jso_byte(obj_handle, PROPERTY_STORAGE_CONFIG_READY, RFPROP_SYSTEM_COMPUTERSYSTEM_RAID_CFG_READY, 
        o_result_jso);
}

LOCAL void get_thermaltrip_property(json_object *o_result_jso)
{
    OBJ_HANDLE payload_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_DEBUG, "%s: get payload handle failed!", __FUNCTION__);
        return;
    }
    rf_add_property_jso_bool(payload_handle, PROPERTY_THERMAL_TRIP_POWERPOLICY,
        RFPROP_POWER_ON_AFTER_CPU_THERMALTRIP, o_result_jso);
    rf_add_property_jso_uint32(payload_handle, PROPERTY_THERMAL_TRIP_TIMEDELAY,
        RFPROP_DELAY_SECONDS_AFTER_CPU_THERMALTRIP, o_result_jso);
}

LOCAL void get_leakdetect_property(json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;
    guint8 leakdet_support = dal_get_leakdetect_support();
    get_oem_prop_leakdetsupport(leakdet_support, RFPROP_SYSTEM_COMPUTERSYSTEM_LEAKDET_SUPPORT, o_result_jso);
    if (leakdet_support == PME_SERVICE_SUPPORT) {
        (void)dal_get_object_handle_nth(COOLINGCLASS, 0, &obj_handle);
        get_oem_prop_leakpolicy(obj_handle, PROPERTY_COOLING_LEAK_POLICY, RFPROP_SYSTEM_COMPUTERSYSTEM_LEAK_STRATEGY,
            o_result_jso);
    }
}


LOCAL gint32 get_system_huawei(PROVIDER_PARAS_S *i_paras, json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;                // 类ComputerSystem 的对象的句柄
    json_object *local_kvm_result_jso = NULL; // kernalversion 值的 json形式

    
    _rf_add_system_health_summary_info(o_result_jso);

    
    gint32 ret = get_kernel_os_version(i_paras, o_result_jso, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: Failed to get kernel or OS version.", __FUNCTION__));

    
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_MAJOR_VERSION,
        RFPROP_SYSTEM_COMPUTERSYSTEM_MAJOROSVERSION, o_result_jso);
    
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_MINOR_VERSION,
        RFPROP_SYSTEM_COMPUTERSYSTEM_MINOROSVERSION, o_result_jso);
    
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_SP_MAJOR_VERSION,
        RFPROP_SYSTEM_COMPUTERSYSTEM_MAJORPATCHVERSION, o_result_jso);
    
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_SP_MINOR_VERSION,
        RFPROP_SYSTEM_COMPUTERSYSTEM_MINORPATCHVERSION, o_result_jso);
    
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_DOMAIN, RFPROP_SYSTEM_COMPUTERSYSTEM_DOMAINNAME,
        o_result_jso);
    
    rf_add_property_jso_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_DESCRIPTION,
        RFPROP_SYSTEM_COMPUTERSYSTEM_HOSTDESCRIPTION, o_result_jso);

    
    
    rf_add_prop_json_double(obj_handle, PROPERTY_COMPUTER_SYSTEM_BUFFERMEM, RFPROP_SYSTEM_COMPUTERSYSTEM_BUFFER,
        o_result_jso, NULL);
    
    rf_add_prop_json_double(obj_handle, PROPERTY_COMPUTER_SYSTEM_CACHEDMEM, RFPROP_SYSTEM_COMPUTERSYSTEM_CACHED,
        o_result_jso, NULL);
    
    rf_add_mem_free(obj_handle, o_result_jso);

    
    get_oem_sms_property(o_result_jso);
    

    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_COMPUTER_SYSTEM_BANDWIDTH_USAGETHRE,
        RFPROP_SYSTEM_COMPUTERSYSTEM_NETBANDWIDTHTHRESHOLD, o_result_jso);

    
    (void)get_manager_oem_kvm_enable(&local_kvm_result_jso);
    (void)json_object_object_add(o_result_jso, RFPROP_KVM_LOCAL_STATE, local_kvm_result_jso);

    get_thermaltrip_property(o_result_jso);

    
    get_oem_partitionlists_object(o_result_jso);

    // 后插板，获取对象句柄失败，不返回
    obj_handle = OBJ_HANDLE_COMMON;
    (void)dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);

    
    
    get_oem_prop_threshold_uint16(obj_handle, PROPERTY_MEINFO_CPU_UTILISE, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUUSAGE,
        o_result_jso);
    
    get_oem_prop_threshold_uint16(obj_handle, PROPERTY_MEINFO_MEM_UTILISE, RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYUSAGE,
        o_result_jso);
    json_object *cpu_power_obj = json_object_new_object();

    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_MEINFO_CPU_UTILISE_THRESHOLD,
        RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTHRESHOLD, o_result_jso);
    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_MEINFO_MEM_UTILISE_THRESHOLD,
        RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYTHRESHOLD, o_result_jso);
    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_MEINFO_DISK_UTILISE_THRESHOLD,
        RFPROP_SYSTEM_COMPUTERSYSTEM_HDDTHRESHOLD, o_result_jso);
    if (cpu_power_obj != NULL) {
        
        get_oem_prop_cpuptlimit(i_paras, obj_handle, cpu_power_obj);
        
        get_oem_prop_cpupt(i_paras, obj_handle, cpu_power_obj);
    }
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPOWERADJUSTMENT, cpu_power_obj);
    
    
    get_system_cpu_total_cores(o_result_jso);

    // 获取payload对象的属性
    ret = get_system_huawei_payload(o_result_jso);
    do_if_fail(ret == VOS_OK, debug_log(DLOG_DEBUG, "get_system_payload fail."));

    get_leakdetect_property(o_result_jso);

    
    get_system_product_bmc_bios(o_result_jso);

    
    get_system_chassis(o_result_jso);
    
    // 查询启动顺序(只支持V3)
    get_oem_bootup_sequence(o_result_jso, i_paras);

    redfish_sys_oem_get_post_state(o_result_jso);

    // 获取系统健康灯状态
    get_system_health_led_state(o_result_jso);

    // 查询交换平面
    redfish_get_oem_switch_ipinfo(o_result_jso, i_paras);
    
    redfish_get_oem_switch_info(o_result_jso, i_paras);

    
    redfish_get_present_fru(o_result_jso);

    // 查询PCH的值
    redfish_get_oem_pch(o_result_jso);

    
    redfish_get_oem_storage_summary(o_result_jso);

    
    redfish_get_oem_logical_drive_summary(o_result_jso);
    redfish_get_oem_storage_subsystem_summary(o_result_jso);

    
    redfish_get_oem_storage_views_summary(o_result_jso);

    
    redfish_get_boot_mode_sw_disp(o_result_jso);
    redfish_get_boot_mode_switch(o_result_jso);

    
    
    redfish_get_intelligent_energy_saving_info(o_result_jso);

    get_url_if_x86_enable(o_result_jso);
    redfish_get_npu_ability(o_result_jso);
    get_url_if_frso_enable(o_result_jso);
    
    ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_DIGITALWARRANTY, o_result_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get digital warranty url failed", __FUNCTION__);
    }

    get_process_memory_history(i_paras, o_result_jso);

    ret = get_bma_connect_state(i_paras);
    if (ret == VOS_OK) {
        ret = get_oem_odata_id_object(RF_SYSTEMS_URI, RFPROP_NETWORK_HISTORY_USAGE_RATE, o_result_jso);
        do_if_fail(ret == VOS_OK, debug_log(DLOG_INFO, "%s:get NetworkHistoryUsageRate url failed", __FUNCTION__));
    }

    return VOS_OK;
}


LOCAL gint32 get_power_delay_mode_and_count(OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    gint32 ret;
    guint8 delay_mode;

    guint32 delay_count;
    double delay_seconds;
    gchar delay_mode_str[PROP_VAL_LENGTH] = {0};
    json_object *delay_mode_jso = NULL;

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_PAYLOAD_PWRDELAY_MODE, &delay_mode);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYMODE, NULL);
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
        PROPERTY_PAYLOAD_PWRDELAY_MODE, ret));

    ret = dal_delaymode_int2str(delay_mode, delay_mode_str, sizeof(delay_mode_str));
    if (ret == VOS_OK) {
        delay_mode_jso = json_object_new_string(delay_mode_str);
    }
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYMODE, delay_mode_jso);

    ret = dal_get_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_PWRDELAY_COUNT, &delay_count);
    return_val_do_info_if_fail(ret == VOS_OK, VOS_ERR,
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYSECONDS, NULL);
        debug_log(DLOG_ERROR, "%s: get %s.%s failed, ret=%d.", __FUNCTION__, dfl_get_object_name(obj_handle),
        PROPERTY_PAYLOAD_PWRDELAY_COUNT, ret));
    if (delay_count > MAX_POWER_DELAY_COUNT) {
        delay_seconds = MAX_POWER_DELAY_SECONDS;
    } else {
        delay_seconds = delay_count / DOUBLE_TEN;
    }
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYSECONDS,
        ex_json_object_new_double(delay_seconds, "%.1f"));
    return VOS_OK;
}


LOCAL gint32 get_system_logservice(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    ret = get_odata_id_object(RF_SYSTEMS_URI, RFPROP_SYSTEM_LOGSERVICES, o_result_jso);
    return_val_if_expr(VOS_OK != ret, HTTP_INTERNAL_SERVER_ERROR);

    return HTTP_OK;
}


LOCAL const gchar *boot_seq_int2str(guint8 boot_order)
{
    const gchar *boot_order_str = NULL;

    if (BOOT_ORDER_TPYE_HDD == boot_order) {
        boot_order_str = RFPROP_SYSTEM_BOOTUPSEQUENCE_HDD;
    } else if (BOOT_ORDER_TPYE_CD == boot_order) {
        boot_order_str = RFPROP_SYSTEM_BOOTUPSEQUENCE_CD;
    } else if (BOOT_ORDER_TPYE_PXE == boot_order) {
        boot_order_str = RFPROP_SYSTEM_BOOTUPSEQUENCE_PXE;
    } else if (BOOT_ORDER_TPYE_OTHERS == boot_order) {
        boot_order_str = RFPROP_SYSTEM_BOOTUPSEQUENCE_OTHER;
    } else {
        debug_log(DLOG_DEBUG, "%s: invalid boot order value is %d", __FUNCTION__, boot_order);
    }

    return boot_order_str;
}


LOCAL void get_bootup_seq_from_output(json_object *bootup_sequence, GSList *output)
{
    guint32 i;
    const guint8 *boot_order = NULL;
    guint32 element_num;
    const gchar *boot_order_str = NULL;
    json_object *boot_order_jso = NULL;
    gint32 ret = 0;
    gsize parm_temp = 0;

    return_do_info_if_expr((NULL == bootup_sequence) || (NULL == output),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    boot_order =
        (const guint8 *)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output, 0), &parm_temp, sizeof(guint8));
    element_num = (guint32)parm_temp;

    
    return_do_info_if_fail(NULL != boot_order, debug_log(DLOG_ERROR, "%s: boot_order is NULL", __FUNCTION__));
    

    return_do_info_if_fail(BIOS_BOOT_DEVICE_NUM == element_num,
        debug_log(DLOG_DEBUG, "%s: invalid bootup number fail", __FUNCTION__));

    for (i = 0; i < element_num; i++) {
        boot_order_str = boot_seq_int2str(boot_order[i]);
        return_do_info_if_expr(NULL == boot_order_str,
            debug_log(DLOG_ERROR, "%s: boot_seq_int2str fail", __FUNCTION__));

        boot_order_jso = json_object_new_string(boot_order_str);
        return_do_info_if_expr(NULL == boot_order_jso,
            debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

        ret = json_object_array_add(bootup_sequence, boot_order_jso);
        return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: json_object_array_add fail", __FUNCTION__);
            (void)json_object_put(boot_order_jso));
    }

    return;
}


LOCAL void get_oem_bootup_sequence(json_object *huawei, PROVIDER_PARAS_S *i_paras)
{
    guint32 ret = 0;
    json_object *bootup_sequence = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 bios_order_format = 0;

    return_do_info_if_expr((NULL == huawei) || (NULL == i_paras),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    (void)dal_get_object_handle_nth(CLASS_NAME_BIOS, 0, &obj_handle);
    // 没有bios 对象，不显示启动顺序
    return_if_expr(0 == obj_handle);

    // 不支持的产品，不显示启动顺序
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_BOIS_ORDER_FORMAT, &bios_order_format);
    return_if_expr(0xff == bios_order_format);

    bootup_sequence = json_object_new_array();
    if (NULL != bootup_sequence) {
        // 调用方法查询启动顺序
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            CLASS_NAME_BIOS, METHOD_GET_BOOT_ORDER, AUTH_ENABLE, i_paras->user_role_privilege, NULL, &output_list);
        if (VOS_OK == ret) {
            get_bootup_seq_from_output(bootup_sequence, output_list);
        } else {
            debug_log(DLOG_DEBUG, "%s: get bootup dequence return %d", __FUNCTION__, ret);
        }

        uip_free_gvariant_list(output_list);
    } else {
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__);
    }

    json_object_object_add(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_BOOTUPSEQUENCE, bootup_sequence);

    return;
}



LOCAL void get_system_trustedmodules_oem(OBJ_HANDLE obj_handle, json_object **o_result_jso)
{
    gint32 ret;
    gchar temp_buf[SECURITY_MODULE_MAX_STRING_LENGTH] = {0};
    json_object *obj_oem = NULL;
    json_object *obj_huawei = NULL;
    guint8 selftest_result = 0;

    
    if (NULL == o_result_jso) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return;
    }

    obj_huawei = json_object_new_object();
    return_do_info_if_fail(NULL != obj_huawei, debug_log(DLOG_ERROR, "%s: new vendor object failed.", __FUNCTION__));

    obj_oem = json_object_new_object();
    return_do_info_if_fail(NULL != obj_oem, debug_log(DLOG_ERROR, "%s, %d: obj_oem is NULL.", __FUNCTION__, __LINE__);
        json_object_put(obj_huawei));

    json_object_object_add(obj_oem, RFPROP_OEM_HUAWEI, obj_huawei);
    json_object_object_add(*o_result_jso, RFPROP_OEM, obj_oem);

    
    (void)memset_s(temp_buf, sizeof(temp_buf), 0, sizeof(temp_buf));
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE, temp_buf,
        sizeof(temp_buf));
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: get SpecificationType fail, ret %d.\n", __FUNCTION__, __LINE__, ret));
    ret = rf_string_check(temp_buf);
    json_object_object_add(obj_huawei, RFPROP_SYSTEM_TRUSTEDMODULES_PROTOCOLTYPE,
        VOS_OK == ret ? json_object_new_string(temp_buf) : NULL);

    (void)memset_s(temp_buf, sizeof(temp_buf), 0, sizeof(temp_buf));
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION, temp_buf,
        sizeof(temp_buf));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s, %d: get %s fail, ret %d.\n", __FUNCTION__,
        __LINE__, PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION, ret));
    ret = rf_string_check(temp_buf);
    json_object_object_add(obj_huawei, RFPROP_SYSTEM_TRUSTEDMODULES_SPECIFICATIONVERSION,
        VOS_OK == ret ? json_object_new_string(temp_buf) : NULL);

    (void)memset_s(temp_buf, sizeof(temp_buf), 0, sizeof(temp_buf));
    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_MANUFACTURER_NAME, temp_buf,
        sizeof(temp_buf));
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s, %d: get %s fail, ret %d.\n", __FUNCTION__,
        __LINE__, PROPERTY_SECURITY_MODULE_MANUFACTURER_NAME, ret));
    ret = rf_string_check(temp_buf);
    json_object_object_add(obj_huawei, RFPROP_SYSTEM_TRUSTEDMODULES_MANUFACTURERNAME,
        VOS_OK == ret ? json_object_new_string(temp_buf) : NULL);

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_MODULE_SELF_TEST_RESULT, &selftest_result);
    return_do_info_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s, %d: get %s fail, ret %d.\n", __FUNCTION__,
        __LINE__, PROPERTY_SECURITY_MODULE_SELF_TEST_RESULT, ret));
    
    json_object_object_add(obj_huawei, RFPROP_SYSTEM_TRUSTEDMODULES_SELFTESTRESULT,
        INVALID_DATA_BYTE != selftest_result ?
        (selftest_result == ENABLE ? json_object_new_string(OK_STRING) : json_object_new_string(CRITICAL_STRING)) :
        NULL);
}



LOCAL gint32 get_single_trustedmodule(OBJ_HANDLE obj_handle, gpointer user_data)
{
    gint32 ret;
    gint32 ret_type;
    guchar trus_health = 0;
    guchar trus_state = 0;
    gchar *trus_state_s = NULL;
    gchar fm_version[SECURITY_MODULE_MAX_STRING_LENGTH] = {0};
    const gchar *fm_type = NULL;
    gchar specification_version[SECURITY_MODULE_MAX_STRING_LENGTH + 50] = {0};
    gchar type[SECURITY_MODULE_MAX_STRING_LENGTH + 10] = {0};
    json_object *status_obj_jso = NULL;
    gint32 ret_fw_ver;
    guchar health_stu;
    json_object *o_result_jso = (json_object *)user_data;
    json_object *object_jso = NULL;

    return_val_do_info_if_expr(NULL == o_result_jso, VOS_OK,
        debug_log(DLOG_ERROR, "%s: user_data is null pointer", __FUNCTION__));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_MANUFACTURER_VERSION, fm_version,
        SECURITY_MODULE_MAX_STRING_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s: get %s fail, ret = %d", __FUNCTION__, PROPERTY_SECURITY_MODULE_MANUFACTURER_VERSION,
        ret));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION,
        specification_version, sizeof(specification_version));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s: get %s fail, ret = %d", __FUNCTION__, PROPERTY_SECURITY_MODULE_SPECIFICATION_VERSION,
        ret));

    ret = dal_get_property_value_string(obj_handle, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE, type, sizeof(type));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s: get %s fail, ret = %d", __FUNCTION__, PROPERTY_SECURITY_MODULE_SPECIFICATION_TYPE,
        ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_MODULE_SELF_TEST_RESULT, &trus_health);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s: get %s fail, ret = %d", __FUNCTION__, PROPERTY_SECURITY_MODULE_SELF_TEST_RESULT,
        ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_SECURITY_MODULE_PRESENCE, &trus_state);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_OK,
        debug_log(DLOG_ERROR, "%s: get %s fail, ret = %d", __FUNCTION__, PROPERTY_SECURITY_MODULE_PRESENCE, ret));

    ret_fw_ver = rf_string_check(fm_version);
    ret_type = rf_string_check(type);
    trus_state_s = rfsystem_to_system_trust_state(trus_state);
    if (VOS_OK == g_strcmp0(RFPROP_SYSTEM_TRUSTEDMODULES_TPM, type) &&
        VOS_OK == g_strcmp0(RFPROP_SYSTEM_TRUSTEDMODULES_1_2, specification_version)) {
        fm_type = RFPROP_SYSTEM_TRUSTEDMODULES_TPM_1_2;
    } else if (VOS_OK == g_strcmp0(RFPROP_SYSTEM_TRUSTEDMODULES_TPM, type) &&
        VOS_OK == g_strcmp0(RFPROP_SYSTEM_TRUSTEDMODULES_2_0, specification_version)) {
        fm_type = RFPROP_SYSTEM_TRUSTEDMODULES_TPM_2_0;
    } else if (VOS_OK == g_strcmp0(RFPROP_SYSTEM_TRUSTEDMODULES_TCM, type) &&
        VOS_OK == g_strcmp0(RFPROP_SYSTEM_TRUSTEDMODULES_1_0, specification_version)) {
        fm_type = RFPROP_SYSTEM_TRUSTEDMODULES_TCM_1_0;
    } else {
        ret_type = VOS_ERR;
    }

    object_jso = json_object_new_object();
    return_val_do_info_if_expr(NULL == object_jso, VOS_OK,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    ret = json_object_array_add(o_result_jso, object_jso);
    return_val_do_info_if_expr(VOS_OK != ret, VOS_OK, json_object_put(object_jso);
        debug_log(DLOG_ERROR, "%s: json_object_array_add fail", __FUNCTION__));

    
    switch (trus_health) {
        case 0:
            health_stu = 1;
            break;

        case 1:
            health_stu = 0;
            break;

        default:
            health_stu = trus_health;
            break;
    }
    (void)get_resource_status_property(&health_stu, NULL, trus_state_s, &status_obj_jso, FALSE);
    
    json_object_object_add(object_jso, RFPROP_SYSTEM_TRUSTEDMODULES_FMVER,
        ret_fw_ver == VOS_OK ? json_object_new_string(fm_version) : NULL);
    json_object_object_add(object_jso, RFPROP_SYSTEM_TRUSTEDMODULES_INTERFACETYPE,
        ret_type == VOS_OK ? json_object_new_string(fm_type) : NULL);
    json_object_object_add(object_jso, RFPROP_SYSTEM_SUMMARY_STATUS, status_obj_jso);

    get_system_trustedmodules_oem(obj_handle, &object_jso);

    return VOS_OK;
}


LOCAL gint32 get_system_trustedmodules(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    guchar board_type = 0;

    
    if (NULL == o_result_jso || NULL == o_message_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    gint32 ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: get board_type fail.\n", __FUNCTION__, __LINE__));
    return_val_if_fail(DISABLE != board_type, HTTP_INTERNAL_SERVER_ERROR);

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_expr(NULL == *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__));

    ret = dfl_foreach_object(CLASS_SECURITY_MODULE, get_single_trustedmodule, *o_result_jso,
        NULL); // 失败了返回200，结果为空数组
    return_val_do_info_if_fail(DFL_OK == ret, HTTP_OK,
        debug_log(DLOG_MASS, "%s: dfl_foreach_object %s failed, ret = %d", __FUNCTION__, CLASS_SECURITY_MODULE, ret));

    return HTTP_OK;
}


LOCAL void get_oem_prop_cpupt(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guint32 ret;
    GSList *output_list = NULL;
    guint8 p_state;
    guint8 t_state;
    guint8 x86_enable = DISABLE;

    return_do_info_if_expr((NULL == o_result_jso), debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);

    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    return_do_info_if_expr(dal_match_product_id(PRODUCT_ID_TCE),
        debug_log(DLOG_MASS, "%s:TCE do not support get cpupt", __FUNCTION__));

    (void)redfish_get_x86_enable_type(&x86_enable);
    if (x86_enable == DISABLE) {
        debug_log(DLOG_MASS, "%s: do not support get cpupt", __FUNCTION__);
        return;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_GETPT, AUTH_ENABLE, i_paras->user_role_privilege, NULL, &output_list);

    return_do_info_if_expr(VOS_OK != ret, uip_free_gvariant_list(output_list);
        debug_log(DLOG_DEBUG, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    p_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    t_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));

    uip_free_gvariant_list(output_list);

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPSTATE, json_object_new_int(p_state));
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTSTATE, json_object_new_int(t_state));

    return;
}


LOCAL void get_oem_prop_cpuptlimit(PROVIDER_PARAS_S *i_paras, OBJ_HANDLE obj_handle, json_object *o_result_jso)
{
    guint32 ret;
    GSList *output_list = NULL;
    guint8 p_limit;
    guint8 t_limit;
    guint8 x86_enable = DISABLE;

    return_do_info_if_expr((NULL == o_result_jso), debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);
    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_MASS, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    return_do_info_if_expr(dal_match_product_id(PRODUCT_ID_TCE),
        debug_log(DLOG_MASS, "%s:TCE do not support get cpupt", __FUNCTION__));

    (void)redfish_get_x86_enable_type(&x86_enable);
    if (x86_enable == DISABLE) {
        debug_log(DLOG_MASS, "%s: do not support get cpupt", __FUNCTION__);
        return;
    }

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_GETPTLIMT, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &output_list);

    return_do_info_if_expr(VOS_OK != ret, uip_free_gvariant_list(output_list);
        debug_log(DLOG_DEBUG, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    p_limit = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    t_limit = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
    uip_free_gvariant_list(output_list);
    if (p_limit == 0) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPLIMIT, NULL);
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPLIMIT, json_object_new_int(p_limit - 1));
    }

    if (t_limit == 0) {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTLIMIT, NULL);
    } else {
        json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTLIMIT, json_object_new_int(t_limit - 1));
    }

    return;
}


LOCAL void get_oem_prop_powerofftimeoutenabled(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL;
    guint32 prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    ret = dal_get_property_value_uint32(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    property_jso = json_object_new_boolean(prop_val);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);
}


LOCAL void get_oem_prop_panelpowerbuttonenabled(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    guchar prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    
    ret = dal_get_property_value_byte(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    
    property_jso = json_object_new_boolean(1 - prop_val);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_boolean fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


LOCAL void get_oem_prop_powerrestorepolicy(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    guchar prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    
    ret = dal_get_property_value_byte(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    
    if (prop_val == POWER_RESTORE_POLICY_ALWAYS_OFF) {
        property_jso = json_object_new_string(RFPROP_SYSTEM_POWER_POLICY_STAY_OFF);
    } else if (prop_val == POWER_RESTORE_POLICY_PREVIOUS) {
        property_jso = json_object_new_string(RFPROP_SYSTEM_POWER_POLICY_PREVIOUS_STATE);
    } else if (prop_val == POWER_RESTORE_POLICY_ALWAYS_ON) {
        property_jso = json_object_new_string(RFPROP_SYSTEM_POWER_POLICY_TURN_ON);
    } else if (prop_val == POWER_RESTORE_POLICY_ALWAYS_ON_RO) {
        property_jso = json_object_new_string(RFPROP_SYSTEM_POWER_POLICY_TURN_ON_RO);
    } else {
        property_jso = NULL;
    }

    do_if_expr(NULL == property_jso, debug_log(DLOG_DEBUG, "%s:json_object_new_int fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}

LOCAL void get_oem_prop_powerontrategyexceptions(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    guint32 exceptions = 0;
    POWER_ON_STRATEGY_EXCEPTION power_on_strategy_exception[] = {
        {RFPROP_POWER_ON_SCENE_FIRMWARE_ACTIVE, POWER_ON_FIRMWARE_ACTIVE_INDEX}
    };
    guint8 j;

    if ((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso)) {
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__);
        return;
    }

    
    ret = dal_get_property_value_uint32(obj_handle, property, &exceptions);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get property value fail", __FUNCTION__);
    }
    (void)json_object_object_add(o_result_jso, rf_property, NULL);

    
    property_jso = json_object_new_array();
    for (j = 0; j < G_N_ELEMENTS(power_on_strategy_exception); j++) {
        if (GET_BIT_GUINT32_VALUE(exceptions, power_on_strategy_exception[j].strategy_exception_scene_idx)) {
            (void)json_object_array_add(property_jso,
                json_object_new_string(power_on_strategy_exception[j].strategy_exception_scene));
        }
    }

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


void get_oem_prop_threshold_uint16(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    guint32 ret;
    json_object *property_jso = NULL; // 属性 值的 json格式
    guint16 prop_val = 0;

    return_do_info_if_expr((NULL == property) || (NULL == rf_property) || (NULL == o_result_jso),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    if (0 == obj_handle) {
        json_object_object_add(o_result_jso, rf_property, NULL);
        return;
    }

    
    ret = dal_get_property_value_uint16(obj_handle, property, &prop_val);
    return_do_info_if_fail(VOS_OK == ret, json_object_object_add(o_result_jso, rf_property, NULL);
        debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

    
    return_do_info_if_expr(prop_val > 100, json_object_object_add(o_result_jso, rf_property, NULL));
    
    
    property_jso = json_object_new_int(prop_val);
    do_if_expr(NULL == property_jso, debug_log(DLOG_ERROR, "%s:json_object_new_int fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


LOCAL void get_oem_partitionlists_object(json_object *huawei)
{
    guint32 ret;
    json_object *jso_array = NULL;
    json_object *obj_json_partition = NULL;
    gchar prop_val[PROP_VAL_MAX_LENGTH] = {0};
    json_object *prop = NULL;
    OBJ_HANDLE obj_handle = 0;
    gdouble val = 0;
    guchar value = 0;
    GSList *list = NULL;
    GSList *node = NULL;

    jso_array = json_object_new_array();
    return_do_info_if_expr(NULL == jso_array, debug_log(DLOG_ERROR, "%s : json_object_new_array fail", __FUNCTION__);
        json_object_object_add(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONLIST, NULL));

    
    ret = dfl_get_object_list(CLASS_OS_DRIVE_PARTITION, &list);
    // 没有bma是没有该类，失败只记debug日志
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_DEBUG, "%s, %d: 'get object list fail.'\n", __FUNCTION__, __LINE__);
        json_object_object_add(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONLIST, jso_array););

    for (node = list; node; node = g_slist_next(node)) {
        obj_json_partition = json_object_new_object();
        continue_do_info_if_fail((NULL != obj_json_partition),
            debug_log(DLOG_ERROR, "%s : json_object_new_object fail", __FUNCTION__));
        obj_handle = (OBJ_HANDLE)node->data;

        
        (void)dal_get_property_value_string(obj_handle, PROPERTY_OS_DRIVE_OS_DRIVE_NAME, prop_val, PROP_VAL_MAX_LENGTH);

        if (g_strcmp0(prop_val, PROPERTY_VALUE_DEFAULT_STRING)) {
            prop = json_object_new_string(prop_val);
            do_info_if_true(NULL == prop, debug_log(DLOG_ERROR, "%s : json_object_new_string fail", __FUNCTION__));
        }

        json_object_object_add(obj_json_partition, RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONNAME, prop);
        prop = NULL;

        
        (void)dal_get_property_value_byte(obj_handle, PROPERTY_OS_DRIVE_USAGE, &value);

        if (value != 0xFF) {
            prop = json_object_new_int(value);
            do_info_if_true(NULL == prop, debug_log(DLOG_ERROR, "%s : json_object_new_int fail", __FUNCTION__));
        }

        json_object_object_add(obj_json_partition, RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONUSAGE, prop);
        prop = NULL;

        
        ret = dal_get_property_value_double(obj_handle, PROPERTY_OS_DRIVE_PARTITION_USEDGB, &val);
        if (val != 0xFFFF) {
            prop = ex_json_object_new_double(val, "%.2f");
            do_info_if_true((NULL == prop), debug_log(DLOG_ERROR, "%s : ex_json_object_new_double fail", __FUNCTION__));
        }

        json_object_object_add(obj_json_partition, RFPROP_SYSTEM_COMPUTERSYSTEM_USAGECAPACITY, prop);
        prop = NULL;

        
        ret = dal_get_property_value_double(obj_handle, PROPERTY_OS_DRIVE_PARTITION_TOTALGB, &val);
        if (val != 0xFFFF) {
            prop = ex_json_object_new_double(val, "%.2f");
            do_info_if_true((NULL == prop), debug_log(DLOG_ERROR, "%s : ex_json_object_new_double fail", __FUNCTION__));
        }

        json_object_object_add(obj_json_partition, RFPROP_SYSTEM_COMPUTERSYSTEM_TOTALCAPACITY, prop);

        ret = json_object_array_add(jso_array, obj_json_partition);
        do_info_if_true(VOS_OK != ret, json_object_put(obj_json_partition);
            debug_log(DLOG_ERROR, "%s : json_object_array_add fail", __FUNCTION__));
    }

    g_slist_free(list);
    json_object_object_add(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONLIST, jso_array);
    return;
}


LOCAL gint32 get_system_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    return rfsystem_get_property_string(rfsystem_get_self_provider(i_paras), i_paras, o_message_jso, o_result_jso);
}


LOCAL gint32 set_system_hostname(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    const char *value_string = NULL;
    GSList *input_list = NULL;

    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_expr(((i_paras->user_role_privilege) & (USERROLE_BASICSETTING)) == 0, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SERVICEROOT_HOST_NAME, o_message_jso,
        RFPROP_SERVICEROOT_HOST_NAME));
    value_string = dal_json_object_get_str(i_paras->val_jso);
    ret = dal_get_object_handle_nth(CLASS_COMPUTER_SYSTEM, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_PROP_ITEM_MISSING, RFPROP_SERVICEROOT_HOST_NAME, o_message_jso,
        RFPROP_SERVICEROOT_HOST_NAME));
    
    input_list = g_slist_append(input_list, g_variant_new_string(value_string));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_COMPUTER_SYSTEM, METHOD_COMSYS_SET_HOSTNAME, AUTH_DISABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;
        
        case ERRCODE_MSG_TOO_BIG:
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, RFPROP_SERVICEROOT_HOST_NAME, o_message_jso, value_string,
                RFPROP_SERVICEROOT_HOST_NAME);
            debug_log(DLOG_ERROR, "%s, %d: failed, ret is %d", __FUNCTION__, __LINE__, ret);
            return HTTP_BAD_REQUEST;
        
        
        case ERRCODE_TYPE_MISMATCH:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_SERVICEROOT_HOST_NAME, o_message_jso, value_string,
                RFPROP_SERVICEROOT_HOST_NAME);
            debug_log(DLOG_ERROR, "%s, %d: failed, ret is %d", __FUNCTION__, __LINE__, ret);
            return HTTP_BAD_REQUEST;
        
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_SERVICEROOT_HOST_NAME, o_message_jso,
                RFPROP_SERVICEROOT_HOST_NAME);
            return HTTP_FORBIDDEN;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso);
            debug_log(DLOG_ERROR, "%s, %d: failed, ret is %d", __FUNCTION__, __LINE__, ret);
            return HTTP_BAD_REQUEST;
    }
}

LOCAL gint32 redfish_set_oem_configuration_model(PROVIDER_PARAS_S* para,
    json_object* property_jso, json_object** msg_array)
{
    gint32 ret;
    const gchar* config_modle_str = NULL;
    const gchar* prop_err = "Oem/Huawei/"RFPROP_SYSTEM_COMPUTERSYSTEM_CONFIGMODEL;
    json_object* message_jso = NULL;
    GSList* input_list = NULL;
    OBJ_HANDLE product = 0;

    if (!dal_is_customized_by_cmcc() && !dal_is_customized_by_cucc()) {
        debug_log(DLOG_INFO, "%s : not supported", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_UNKNOWN, prop_err, &message_jso, prop_err);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product);
    if (ret != VOS_OK) {
        debug_log(DLOG_INFO, "%s : get PRODUCT object handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_ITEM_MISSING, prop_err, &message_jso, prop_err);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    config_modle_str = dal_json_object_get_str(property_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(config_modle_str));
    ret = uip_call_class_method_redfish(para->is_from_webui, para->user_name, para->client, product,
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_CONFIG_MODEL, AUTH_ENABLE, para->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;
        case ERRCODE_TYPE_MISMATCH:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_err, &message_jso, config_modle_str, prop_err);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, type mismatched, ret is %d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, insufficient privilege, ret is %d", __FUNCTION__, ret);
            return HTTP_FORBIDDEN;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, unknown err, ret is %d", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 redfish_set_oem_cmcc_version(PROVIDER_PARAS_S* para, json_object* property_jso, json_object** msg_array)
{
    gint32 ret;
    const gchar* cmcc_version_str = NULL;
    const gchar* prop_err = "Oem/Huawei/"RFPROP_SYSTEM_COMPUTERSYSTEM_CMCC_VERSION;
    json_object* message_jso = NULL;
    GSList* input_list = NULL;
    OBJ_HANDLE product = 0;

    if (!dal_is_customized_by_cmcc()) {
        debug_log(DLOG_ERROR, "%s : not supported", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_UNKNOWN, prop_err, &message_jso, prop_err);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : get PRODUCT object handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_ITEM_MISSING, prop_err, &message_jso, prop_err);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    cmcc_version_str = dal_json_object_get_str(property_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(cmcc_version_str));
    ret = uip_call_class_method_redfish(para->is_from_webui, para->user_name, para->client, product,
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_CMCC_VERSION, AUTH_ENABLE, para->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return  HTTP_OK;
        case ERRCODE_TYPE_MISMATCH:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_err, &message_jso, cmcc_version_str, prop_err);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, type mismatched, ret is %d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, insufficient privilege, ret is %d", __FUNCTION__, ret);
            return HTTP_FORBIDDEN;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, unknown err, ret is %d", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 redfish_set_oem_cucc_version(PROVIDER_PARAS_S* para, json_object* property_jso, json_object** msg_array)
{
    gint32 ret;
    const gchar* cucc_version_str = NULL;
    const gchar* prop_err = "Oem/Huawei/"RFPROP_SYSTEM_COMPUTERSYSTEM_CUCC_VERSION;
    json_object* message_jso = NULL;
    GSList* input_list = NULL;
    OBJ_HANDLE product = 0;

    if (!dal_is_customized_by_cucc()) {
        debug_log(DLOG_ERROR, "%s : not supported", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_UNKNOWN, prop_err, &message_jso, prop_err);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_PRODUCT, 0, &product);
    if (ret != VOS_OK) {
        debug_log(DLOG_ERROR, "%s : get PRODUCT object handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_ITEM_MISSING, prop_err, &message_jso, prop_err);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    cucc_version_str = json_object_get_string(property_jso);
    input_list = g_slist_append(input_list, g_variant_new_string(cucc_version_str));
    ret = uip_call_class_method_redfish(para->is_from_webui, para->user_name, para->client, product,
        CLASS_NAME_PRODUCT, METHOD_PRODUCT_SET_CUCC_VERSION, AUTH_ENABLE, para->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return  HTTP_OK;
        case ERRCODE_TYPE_MISMATCH:
            (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_err, &message_jso, cucc_version_str, prop_err);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, type mismatched, ret is %d", __FUNCTION__, ret);
            return HTTP_BAD_REQUEST;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, insufficient privilege, ret is %d", __FUNCTION__, ret);
            return HTTP_FORBIDDEN;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*msg_array, message_jso);
            debug_log(DLOG_ERROR, "%s: failed, unknown err, ret is %d", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}


LOCAL gint32 redfish_set_oem_power_restore_policy(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    const gchar *poweron_policy_str = NULL;
    guint8 poweron_policy = 0xff;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_err = "Oem/Huawei/PowerOnStrategy";

    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras) || (NULL == property_jso)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    poweron_policy_str = dal_json_object_get_str(property_jso);
    if (0 == g_strcmp0(poweron_policy_str, RFPROP_SYSTEM_POWER_POLICY_STAY_OFF)) {
        poweron_policy = POWER_RESTORE_POLICY_ALWAYS_OFF;
    } else if (0 == g_strcmp0(poweron_policy_str, RFPROP_SYSTEM_POWER_POLICY_TURN_ON)) {
        poweron_policy = POWER_RESTORE_POLICY_ALWAYS_ON;
    } else if (0 == g_strcmp0(poweron_policy_str, RFPROP_SYSTEM_POWER_POLICY_PREVIOUS_STATE)) {
        poweron_policy = POWER_RESTORE_POLICY_PREVIOUS;
    } else {
        debug_log(DLOG_ERROR, "%s : invalid power estore policy is %s.", __FUNCTION__, poweron_policy_str);
        return VOS_ERR;
    }

    // 获取payload对象句柄
    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : get payload object handle fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(poweron_policy));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PAYLOAD, METHOD_SET_PWR_REST_POLICY, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        case VOS_OK:
            return VOS_OK;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return VOS_ERR;
}

LOCAL void power_on_strategy_exceptions_parse(json_object* json_obj, guint32* exceps)
{
    gint32 len = 0;
    guint32 i;
    guint32 j;
    json_object *temp_jso = NULL;
    const gchar* scene_str = NULL;
    POWER_ON_STRATEGY_EXCEPTION power_on_strategy_exception[] = {
        {RFPROP_POWER_ON_SCENE_FIRMWARE_ACTIVE, POWER_ON_FIRMWARE_ACTIVE_INDEX}
    };

    len = json_object_array_length(json_obj);
    for (i = 0; i < len; i++) {
        temp_jso = json_object_array_get_idx(json_obj, i);
        scene_str = NULL;
        scene_str = dal_json_object_get_str(temp_jso);
        if (scene_str == NULL) {
            continue;
        }
        for (j = 0; j < G_N_ELEMENTS(power_on_strategy_exception); j++) {
            if (g_strcmp0(scene_str, power_on_strategy_exception[j].strategy_exception_scene) == 0) {
                *exceps = SET_BIT_GUINT32_VALUE(*exceps, power_on_strategy_exception[j].strategy_exception_scene_idx);
                break;
            }
        }
    }
}

LOCAL gint32 redfish_set_oem_power_on_strategy_exceptions(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    GSList* input_list = NULL;
    const gchar* prop_str = "Oem/Huawei/PowerOnStrategyExceptions";
    guint32 exceps = 0;
 
    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : get payload object handle fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));
 
    power_on_strategy_exceptions_parse(property_jso, &exceps);
 
    input_list = g_slist_append(input_list, g_variant_new_uint32(exceps));
 
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
                                        CLASS_PAYLOAD, METHOD_SET_POWER_ON_STRATEGY_EXCEPTIONS, AUTH_ENABLE,
                                        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
 
    switch (ret) {
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_str, &message_jso, prop_str);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        case VOS_OK:
            return VOS_OK;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
            debug_log(DLOG_ERROR, "%s : uip_call_class_method_redfish fail, ret = %d", __FUNCTION__, ret);
    }

    return VOS_ERR;
}


LOCAL gint32 redfish_set_oem_power_restore_delay_mode(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    gint32 method_ret;
    const gchar *delay_mode_str = NULL;
    guint8 delay_mode = 0xff;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_mode_err = "Oem/Huawei/PowerRestoreDelayMode";

    if (message_array == NULL || provider_paras_check(i_paras) != VOS_OK || (property_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    delay_mode_str = dal_json_object_get_str(property_jso);
    ret = dal_delaymode_str2int(delay_mode_str, &delay_mode);
    if (ret == VOS_ERR) {
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, prop_mode_err, &message_jso, delay_mode_str, prop_mode_err);
        (void)json_object_array_add(*message_array, message_jso);
        return VOS_ERR;
    }

    // 获取payload对象句柄
    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(ret == DFL_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : get payload object handle fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(delay_mode));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PAYLOAD, METHOD_SET_PWR_DELAY_MODE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    method_ret = create_common_return_message(ret, message_array, prop_mode_err);
    return method_ret;
}


LOCAL gint32 redfish_set_oem_power_restore_delay_count(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    gint32 method_ret;
    guint32 delay_count;
    double delay_seconds;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_count_err = "Oem/Huawei/PowerRestoreDelaySeconds";
    const gchar *delay_string = NULL;
    const gchar *pchr_fname = NULL;

    if (message_array == NULL || provider_paras_check(i_paras) != VOS_OK || (property_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    delay_string = dal_json_object_get_str(property_jso);
    pchr_fname = strrchr(delay_string, '.');
    if (pchr_fname != NULL) {
        return_val_do_info_if_expr(
            strlen(pchr_fname) > 2, VOS_ERR, debug_log(DLOG_ERROR, "%s : input param illegal.", __FUNCTION__); (
            void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_count_err, &message_jso, delay_string, prop_count_err);
            (void)json_object_array_add(*message_array, message_jso));
    }
    delay_seconds = json_object_get_double(property_jso);
    delay_count = (guint32)(delay_seconds * INT_TEN);

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(ret == DFL_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : get payload object handle fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));

    input_list = g_slist_append(input_list, g_variant_new_uint32(delay_count));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PAYLOAD, METHOD_SET_PWR_DELAY_COUNT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    method_ret = create_common_return_message(ret, message_array, prop_count_err);
    return method_ret;
}


LOCAL gint32 redfish_set_oem_panel_power_button_enabled(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    guint8 power_button;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_error = "Oem/Huawei/PanelPowerButtonEnabled";

    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras) || (NULL == property_jso)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    power_button = json_object_get_boolean(property_jso) ? 0 : 1;

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : get payload object handle fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));

    input_list = g_slist_append(input_list, g_variant_new_byte(power_button));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PAYLOAD, METHOD_SET_PWR_BTN_LOCK, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_error, &message_jso, prop_error);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return VOS_ERR;
}


LOCAL gint32 get_switch_plane_object(const gchar *switch_type, OBJ_HANDLE *obj_handle)
{
    guint8 fru_id = 0;
    gint32 ret;

    return_val_do_info_if_expr(NULL == obj_handle, VOS_ERR, debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (0 == g_strcmp0(switch_type, RFPROP_SYSTEM_SWITCH_BASE)) {
        fru_id = BASE_FRU_ID;
    } else if (0 == g_strcmp0(switch_type, RFPROP_SYSTEM_SWITCH_FABRIC)) {
        fru_id = FABRIC_FRU_ID;
    } else if (0 == g_strcmp0(switch_type, RFPROP_SYSTEM_SWITCH_FC)) {
        fru_id = FC_FRU_ID;
    } else {
        debug_log(DLOG_ERROR, "%s: invalid switch type is %s.", __FUNCTION__, switch_type);
        return VOS_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_SWITCH_PLANE_INFO_MGNT, PROPERTY_SWITCH_PLANE_FRU_ID, fru_id, obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_MASS, "%s: do not have %s", __FUNCTION__, switch_type));

    return VOS_OK;
}


LOCAL gint32 set_switch_ip(PROVIDER_PARAS_S *i_paras, json_object *property, json_object **message_array,
    const gchar *switch_type)
{
    int iRet;
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    GSList *input_list = NULL;
    const gchar *address = NULL;
    const gchar *mask = NULL;
    const gchar *gateway = NULL;
    json_object *message = NULL;
    gchar error_string[PROP_VAL_LENGTH] = {0};
    gchar error[PROP_VAL_LENGTH] = {0};

    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras) || (NULL == property) ||
        (NULL == switch_type)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    // 失败不影响程序正常运行
    iRet = snprintf_s(error, sizeof(error), sizeof(error) - 1, "Oem/Huawei/SwitchIpInfo/%s", switch_type);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    ret = get_switch_plane_object(switch_type, &obj_handle);
    // 没有对象，生成一个FRU不存在的消息
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_DEBUG, "%s: get switch plane object handle fail", __FUNCTION__);
        (void)create_message_info(MSGID_FRU_NOT_EXIST, error_string, &message, switch_type);
        (void)json_object_array_add(*message_array, message));

    // 如果查到接口里的是空对象(原因是，协议层删除了错误属性)，直接返回，不生成消息
    if (0 == g_strcmp0(dal_json_object_get_str(property), JSON_NULL_OBJECT_STR)) {
        return VOS_ERR;
    }

    // 没有传值，给一个默认值
    ret = get_element_str(property, RF_PROPERTY_ADDR, &address);
    do_info_if_true(FALSE == ret, (address = ""));

    ret = get_element_str(property, RF_PROPERTY_SUB_MASK, &mask);
    do_info_if_true(FALSE == ret, (mask = ""));

    ret = get_element_str(property, RF_PROPERTY_GATEWAY, &gateway);
    do_info_if_true(FALSE == ret, (gateway = ""));

    // 在schema里面限制长度(7,15)，IP最短是7，最长是15

    input_list = g_slist_append(input_list, g_variant_new_string(address));
    input_list = g_slist_append(input_list, g_variant_new_string(mask));
    input_list = g_slist_append(input_list, g_variant_new_string(gateway));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_SWITCH_PLANE_INFO_MGNT, SWITCH_METHOD_SET_IP_MASK_GW, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            // 权限不足
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, error, &message, error);
            (void)json_object_array_add(*message_array, message);

            break;

        case SET_SWITCH_PLANE_IP_FORMAT_ERR:
            // IP错误
            iRet = snprintf_s(error_string, sizeof(error_string), sizeof(error_string) - 1,
                "Oem/Huawei/SwitchIpInfo/%s/Address", switch_type);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            (void)create_message_info(MSGID_INVALID_IPV4ADDRESS, error_string, &message, address, error_string);
            (void)json_object_array_add(*message_array, message);

            break;

        case SET_SWITCH_PLANE_MASK_FORMAT_ERR:
            // 掩码错误
            iRet = snprintf_s(error_string, sizeof(error_string), sizeof(error_string) - 1,
                "Oem/Huawei/SwitchIpInfo/%s/SubnetMask", switch_type);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            (void)create_message_info(MSGID_INVALID_SUBNETMASK, error_string, &message, mask, error_string);
            (void)json_object_array_add(*message_array, message);

            break;

        case SET_SWITCH_PLANE_GW_FORMAT_ERR:
            // 网关错误
            iRet = snprintf_s(error_string, sizeof(error_string), sizeof(error_string) - 1,
                "Oem/Huawei/SwitchIpInfo/%s/Gateway", switch_type);
            do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));
            (void)create_message_info(MSGID_INVALID_IPV4GATEWAY, error_string, &message, gateway, error_string);
            (void)json_object_array_add(*message_array, message);

            break;

            
        case SWITCH_PLANE_NO_RESPONE:
        case SWITCH_PLANE_RESPONE_ERR:
            // (base ,fabric,fc)交换平面下电、刚上电、或者连续设置FC 的IP
            (void)create_message_info(MSGID_FAILED_SET_IP_INFO, error, &message, switch_type);
            (void)json_object_array_add(*message_array, message);

            break;

            
        default:
            debug_log(DLOG_DEBUG, "%s: set %s ipinfo return %d", __FUNCTION__, switch_type, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return VOS_ERR;
}


LOCAL gint32 redfish_set_oem_switch_ipinfo(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    json_object *property = NULL;
    gint32 result = VOS_ERR;
    const gchar *switch_err = "Oem/Huawei/SwitchIpInfo";
    json_object *message_jso = NULL;
    guint8 board_type = 0;

    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras) || (NULL == property_jso)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return result;
    }

    ret = redfish_get_board_type(&board_type);
    if (VOS_OK != ret) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        return result;
    }

    // 非后插板报501
    if (BOARD_SWITCH != board_type) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, switch_err, &message_jso, switch_err);
        (void)json_object_array_add(*message_array, message_jso);
        
        return HTTP_NOT_IMPLEMENTED;
        
    }

    // 设置base的IP信息
    ret = json_object_object_get_ex(property_jso, RFPROP_SYSTEM_SWITCH_BASE, &property);
    if (TRUE == ret) {
        ret = set_switch_ip(i_paras, property, message_array, RFPROP_SYSTEM_SWITCH_BASE);
        do_info_if_true(VOS_OK == ret, (result = VOS_OK));
    }
    property = NULL;

    // 设置fabric的IP信息
    ret = json_object_object_get_ex(property_jso, RFPROP_SYSTEM_SWITCH_FABRIC, &property);
    if (TRUE == ret) {
        ret = set_switch_ip(i_paras, property, message_array, RFPROP_SYSTEM_SWITCH_FABRIC);
        do_info_if_true(VOS_OK == ret, (result = VOS_OK));
    }
    property = NULL;

    // 设置fc的IP信息
    ret = json_object_object_get_ex(property_jso, RFPROP_SYSTEM_SWITCH_FC, &property);
    if (TRUE == ret) {
        ret = set_switch_ip(i_paras, property, message_array, RFPROP_SYSTEM_SWITCH_FC);
        do_info_if_true(VOS_OK == ret, (result = VOS_OK));
    }

    return result;
}


LOCAL void get_boot_int_from_string(const gchar *boot_string, guint8 *boot_int)
{
    return_do_info_if_expr((NULL == boot_int) || (NULL == boot_string),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    if (0 == g_strcmp0(RFPROP_SYSTEM_BOOTUPSEQUENCE_HDD, boot_string)) {
        *boot_int = BOOT_ORDER_TPYE_HDD;
    } else if (0 == g_strcmp0(RFPROP_SYSTEM_BOOTUPSEQUENCE_CD, boot_string)) {
        *boot_int = BOOT_ORDER_TPYE_CD;
    } else if (0 == g_strcmp0(RFPROP_SYSTEM_BOOTUPSEQUENCE_PXE, boot_string)) {
        *boot_int = BOOT_ORDER_TPYE_PXE;
    } else {
        // 协议层会拦截异常值
        *boot_int = BOOT_ORDER_TPYE_OTHERS;
    }

    return;
}


LOCAL void get_valid_array_len(json_object *property_jso, gint32 *array_len)
{
    return_do_info_if_expr((NULL == property_jso) || (NULL == array_len),
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__));

    gint32 len = json_object_array_length(property_jso);
    gint32 i;

    *array_len = 0;

    for (i = 0; i < len; i++) {
        // 有效的成员为非空对象
        if (g_strcmp0(dal_json_object_get_str(json_object_array_get_idx(property_jso, i)), JSON_NULL_OBJECT_STR)) {
            (*array_len)++;
        }
    }
}


LOCAL gint32 redfish_set_oem_boot_sequence(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 array_len = 0;
    const gchar *error_mes = "Oem/Huawei/BootupSequence";
    json_object *message = NULL;
    gint32 ret;
    gint32 error_index = 0;
    gchar item_error[PROP_VAL_LENGTH] = {0};
    GVariant *boot_property = NULL;
    guint8 boot_int[BIOS_BOOT_DEVICE_NUM] = {0};
    gint32 i;
    GSList *input_list = NULL;
    gchar array_len_string[PROP_VAL_MAX_LENGTH] = {0};

    // 有效长度不等于4，报错
    get_valid_array_len(property_jso, &array_len);

    if (BIOS_BOOT_DEVICE_NUM != array_len) {
        // 生成长度错误的消息
        (void)snprintf_s(array_len_string, sizeof(array_len_string), sizeof(array_len_string) - 1, "%d",
            BIOS_BOOT_DEVICE_NUM);
        (void)create_message_info(MSGID_PROP_WRONG_ARRAY_LENGTH, error_mes, &message, array_len_string);
        (void)json_object_array_add(*message_array, message);

        return VOS_ERR;
    }

    // 存在重复属性报错
    ret = json_array_duplicate_check(property_jso, &error_index);
    if (VOS_OK == ret) {
        ret = snprintf_s(item_error, sizeof(item_error), sizeof(item_error) - 1, "Oem/Huawei/BootupSequence/%d",
            error_index);
        do_if_fail(ret > 0, debug_log(DLOG_ERROR, "%s : snprintf_s fail", __FUNCTION__));

        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, item_error, &message, item_error);
        (void)json_object_array_add(*message_array, message);

        return VOS_ERR;
    }

    // 从json array 中解析启动顺序
    for (i = 0; i < BIOS_BOOT_DEVICE_NUM; i++) {
        get_boot_int_from_string(dal_json_object_get_str(json_object_array_get_idx(property_jso, i)), &boot_int[i]);
    }

    boot_property = g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, boot_int, BIOS_BOOT_DEVICE_NUM, sizeof(guint8));
    input_list = g_slist_append(input_list, boot_property);

    // 调用底层方法设置启动顺序
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, OBJ_HANDLE_COMMON,
        BIOS_CLASS_NAME, METHOD_SET_BOOT_ORDER, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RF_ERROR:
            // 返回-1，认为是当前产品不支持设置启动顺序
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, error_mes, &message, error_mes);
            (void)json_object_array_add(*message_array, message);
            
            return HTTP_NOT_IMPLEMENTED;
            

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, error_mes, &message, error_mes);
            (void)json_object_array_add(*message_array, message);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set boot order return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return VOS_ERR;
}



LOCAL gint32 set_system_local_kvm(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *val_jso = NULL;
    json_object *o_message_jso_temp = NULL;
    json_bool value_bool;
    guint32 value_byte;
    GSList *input_list = NULL;
    const gchar* prop_name = "Oem/Huawei/LocalKvmEnabled";

    
    // 只读权限
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_KVMMGNT, VOS_ERR,
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &o_message_jso_temp, prop_name);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp));

    
    val_jso = (*o_result_jso);                     // 本地LVM使能的值json_object 格式)
    value_bool = json_object_get_boolean(val_jso); // (bool格式)
    if (value_bool == TRUE) {
        value_byte = 1;
    } else {
        value_byte = 0;
    }

    debug_log(DLOG_DEBUG, " value_byte  :%d", value_byte);

    
    // 3.1 获得BMC的句柄
    ret = dal_get_object_handle_nth(PROXY_KVM_CLASS_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
        (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
        debug_log(DLOG_ERROR, "%s:find object fail.", __FUNCTION__));

    
    // 4.1 填写入参
    input_list = g_slist_append(input_list, g_variant_new_byte(value_byte));

    
    // 4.2 调用UIP函数
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        PROXY_KVM_CLASS_NAME, KVM_METHOD_SET_LOCAL_KVM_STATE, AUTH_DISABLE, AUTH_PRIV_NONE, input_list, NULL);
    

    do_val_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "call method %s failed, ret is %d", KVM_METHOD_SET_LOCAL_KVM_STATE, ret));

    // 4.3 UIP函数调用成功

    // 对类方法的返回值进行判断
    switch (ret) {
        case VOS_OK:
            ret = VOS_OK;
            break;

        default:
            ret = VOS_ERR;
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &o_message_jso_temp);
            (void)json_object_array_add(*o_message_jso, o_message_jso_temp);
            debug_log(DLOG_ERROR, "kvm failure :%d", ret);
            break;
    }

    
    uip_free_gvariant_list(input_list);

    
    return ret;
}


LOCAL gint32 set_system_boot_mode_switch(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    // 内部接口不做入参检测
    gint32 ret;
    guint8 boot_mode_swi;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    guint8 bios_boot_mode_supported = 0;
    json_object *message = NULL;

    (void)dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);

    // 在schema中，说明了，不支持设置启动模式时，为只读属性
    (void)dal_get_property_value_byte(obj_handle, BIOS_BOOT_MODE_SUPPORT_FLAG, &bios_boot_mode_supported);
    return_val_do_info_if_fail(bios_boot_mode_supported, VOS_ERR,
        (void)create_message_info(MSGID_PROP_NOT_WRITABLE, ERROR_PROP_BOOT_MODE_CHANGE_ENABLED, &message,
        ERROR_PROP_BOOT_MODE_CHANGE_ENABLED);
        (void)json_object_array_add(*message_array, message));

    boot_mode_swi = (guint8)json_object_get_boolean(property_jso);

    input_list = g_slist_append(input_list, g_variant_new_byte(boot_mode_swi));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BIOS_CLASS_NAME, METHOD_SET_BIOS_BOOT_MODE_SW, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);

    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, ERROR_PROP_BOOT_MODE_CHANGE_ENABLED, &message,
                ERROR_PROP_BOOT_MODE_CHANGE_ENABLED);
            (void)json_object_array_add(*message_array, message);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set boot mode switch return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message);
            (void)json_object_array_add(*message_array, message);
    }

    return VOS_ERR;
}


LOCAL gint32 set_system_net_bandwidth_usagethre(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    gint32 method_ret = VOS_ERR;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    gint32 threshold;
    const gchar *prop_name = "Oem/Huawei/NetBandwidthThresholdPercent";

    
    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return method_ret;
    }

    
    ret = dal_get_object_handle_nth(CLASS_COMPUTER_SYSTEM, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, method_ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    
    if (json_type_int != json_object_get_type(property_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name, &message_jso, dal_json_object_get_str(property_jso),
            prop_name);
        (void)json_object_array_add(*message_array, message_jso);
        return VOS_ERR;
    }

    threshold = json_object_get_int(property_jso);
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)threshold));

    
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_COMPUTER_SYSTEM, METHOD_COMSYS_SET_BANDWIDTH_USAGETHRE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            method_ret = VOS_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set threshold value return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL gint32 set_system_wol_status(PROVIDER_PARAS_S *i_paras, json_object *property_jso, json_object **message_array)
{
    gint32 method_ret = HTTP_INTERNAL_SERVER_ERROR;
    gint32 ret_val;
    const gchar *prop_name = "Oem/Huawei/WakeOnLANStatus";
    json_object *message_jso = NULL;
    json_bool wol_status_int;
    guint8 wol_status_byte;
    GSList *input_list = NULL;

    if (NULL == message_array || NULL == property_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return method_ret;
    }

    ret_val = get_wol_supported();
    return_val_do_info_if_fail(VOS_OK == ret_val, HTTP_NOT_IMPLEMENTED,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name, &message_jso, prop_name);
        (void)json_object_array_add(*message_array, message_jso));

    wol_status_int = json_object_get_boolean(property_jso);
    if (TRUE == wol_status_int) {
        wol_status_byte = ENABLED;
    } else {
        wol_status_byte = DISABLED;
    }

    input_list = g_slist_append(input_list, g_variant_new_byte(wol_status_byte));

    ret_val = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, 0,
        BMC_PRODUCT_NAME_APP, METHOD_SET_WOL_STATE, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret_val) {
        case VOS_OK:
            method_ret = VOS_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, &message_jso, prop_name);
            (void)json_object_array_add(*message_array, message_jso);
            method_ret = HTTP_FORBIDDEN;
            break;

        default:
            debug_log(DLOG_ERROR, "%s: set wol return %d", __FUNCTION__, ret_val);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL void set_system_oem_huawei_power_restore(PROVIDER_PARAS_S *i_paras, json_object *huawei,
    json_object **o_message_jso, gint32 *return_array, gint32 *arr_subscript)
{
    gint32 ret;
    json_object *property_jso = NULL;
    // 设置通电开机策略
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_SYSTEMSTATEAFTERPOWERSUPPLIED, &property_jso)) {
        ret = redfish_set_oem_power_restore_policy(i_paras, property_jso, o_message_jso);
        
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
        
    }

    // 设置上电延迟模式
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYMODE, &property_jso)) {
        ret = redfish_set_oem_power_restore_delay_mode(i_paras, property_jso, o_message_jso);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }

    // 设置上电延迟时间
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERRESTORE_DELAYSECONDS, &property_jso)) {
        ret = redfish_set_oem_power_restore_delay_count(i_paras, property_jso, o_message_jso);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }

    //设置通电开机策略例外场景
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_POWERONSTRATEGYEXCEPTIONS, &property_jso)) {
        ret = redfish_set_oem_power_on_strategy_exceptions(i_paras, property_jso, o_message_jso);
        
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
        
    }
}


LOCAL void set_system_oem_huawei_para(gint32 *return_array, gint32 *arr_subscript, gint32 ret, gint32 len)
{
    if (*arr_subscript >= len) {
        debug_log(DLOG_ERROR, "%d:set array failed ", __LINE__);
        return;
    }
    return_array[*arr_subscript] = ret;
    (*arr_subscript)++;
    return;
}


LOCAL gint32 set_oem_cpu_pt_state(json_object *huawei, PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    gint32 *return_array, gint32 arr_sz, gint32 *arr_subscript)
{
    gint32 ret = VOS_OK;
    guint8 cpu_p_state = 0;
    guint8 cpu_t_state = 0;
    json_object *property_cpu_jso = NULL;
    json_object *property_cpu_power_jso = NULL;
    json_object *property_jso = NULL;

    return_val_if_expr(return_array == NULL || arr_subscript == NULL, VOS_ERR);

    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPOWERADJUSTMENT, &property_cpu_power_jso)) {
        // 先获取原有的工作频率和空闲时间
        get_oem_prop_cpupandt(i_paras, &cpu_p_state, &cpu_t_state);

        // 设置CPU的工作频率
        if (json_object_object_get_ex(property_cpu_power_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPSTATE, &property_jso)) {
            property_cpu_jso = json_object_new_int(cpu_t_state);
            ret = redfish_set_oem_cpupt(i_paras, property_jso, property_cpu_jso, o_message_jso, SET_P_STATE);
            json_object_put(property_cpu_jso);
            return_val_if_expr((*arr_subscript) >= arr_sz, VOS_ERR);
            return_array[*arr_subscript] = ret;
            *arr_subscript = (*arr_subscript) + 1;

            if (ret == VOS_OK) {
                debug_log(DLOG_ERROR, "setp ret=%d\n", ret);
                cpu_p_state = json_object_get_int(property_jso);
            }
        }

        property_cpu_jso = NULL;
        property_jso = NULL;

        // 设置CPU的空闲时间
        if (json_object_object_get_ex(property_cpu_power_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTSTATE, &property_jso)) {
            property_cpu_jso = json_object_new_int(cpu_p_state);
            ret = redfish_set_oem_cpupt(i_paras, property_cpu_jso, property_jso, o_message_jso, SET_T_STATE);
            debug_log(DLOG_ERROR, "sett ret=%d\n", ret);
            json_object_put(property_cpu_jso);
            return_val_if_expr((*arr_subscript) >= arr_sz, VOS_ERR);
            return_array[*arr_subscript] = ret;
            *arr_subscript = (*arr_subscript) + 1;
        }
    }
    return VOS_OK;
}

LOCAL void set_system_oem_huawei_threshold(PROVIDER_PARAS_S *i_paras, json_object *huawei,
    json_object **o_message_jso, gint32 *return_array, gint32 *arr_subscript)
{
    gint32 ret;
    json_object *property_jso = NULL;
    
    
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTHRESHOLD, &property_jso)) {
        ret = redfish_set_oem_utilisethreshold(i_paras, property_jso, o_message_jso, SET_CPU_UTILISE_TYPE);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }
    property_jso = NULL;

    // 设置内存门限值
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYTHRESHOLD, &property_jso)) {
        ret = redfish_set_oem_utilisethreshold(i_paras, property_jso, o_message_jso, SET_MEM_UTILISE_TYPE);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }
    property_jso = NULL;

    // 设置硬盘使用门限
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_HDDTHRESHOLD, &property_jso)) {
        ret = redfish_set_oem_utilisethreshold(i_paras, property_jso, o_message_jso, SET_DISK_UTILISE_TYPE);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }
}

#define NPU_ABILITY_ARRAY_EXCEED_MAXNUM 1
#define NPU_ABILITY_PROP_TYPE_MISSING 2
#define NPU_ABILITY_PROP_VALUE_MISSING 3
#define NPU_ABILITY_NOT_SUPPORTED 4
#define NPU_ABILITY_TYPE_OUT_OF_RANGE 11
#define NPU_ABILITY_VALUE_OUT_OF_RANGE 37
#define NPU_ABILITY_PROP_PATH "Oem/Huawei/NpuAbility"

LOCAL void create_set_oem_npu_ability_message_info(json_object** msg_array, gint32 result, const guint8 arr_idx)
{
#define PARAM_STR_LEN 50
    json_object* message_jso = NULL;
    gchar param_str[PARAM_STR_LEN] = {0};
    
    switch (result) {
        case NPU_ABILITY_TYPE_OUT_OF_RANGE:
            (void)snprintf_s(param_str, sizeof(param_str), sizeof(param_str) - 1, "%s/%u/%s",
                NPU_ABILITY_PROP_PATH, arr_idx, RFPROP_NPU_ABILITY_TYPE);
            (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, NULL, &message_jso, param_str);
            break;
        case NPU_ABILITY_VALUE_OUT_OF_RANGE:
            (void)snprintf_s(param_str, sizeof(param_str), sizeof(param_str) - 1, "%s/%u/%s",
                NPU_ABILITY_PROP_PATH, arr_idx, RFPROP_NPU_ABILITY_VALUE);
            (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, NULL, &message_jso, param_str);
            break;
        case NPU_ABILITY_ARRAY_EXCEED_MAXNUM:
            (void)create_message_info(MSGID_RECORDS_EXC_MAXNUM, NULL, &message_jso);
            break;
        case NPU_ABILITY_PROP_TYPE_MISSING:
            (void)snprintf_s(param_str, sizeof(param_str), sizeof(param_str) - 1, "%s/%u/%s",
                NPU_ABILITY_PROP_PATH, arr_idx, RFPROP_NPU_ABILITY_TYPE);
            (void)create_message_info(MSGID_PROP_ITEM_MISSING, NULL, &message_jso, param_str);
            break;
        case NPU_ABILITY_PROP_VALUE_MISSING:
            (void)snprintf_s(param_str, sizeof(param_str), sizeof(param_str) - 1, "%s/%u/%s",
                NPU_ABILITY_PROP_PATH, arr_idx, RFPROP_NPU_ABILITY_VALUE);
            (void)create_message_info(MSGID_PROP_ITEM_MISSING, NULL, &message_jso, param_str);
            break;
        default:
            (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &message_jso, METHOD_NPU_ABILITY_SET_NPU_ABILITY);
    }
    (void)json_object_array_add(*msg_array, message_jso);
}

LOCAL gint32 redfish_set_oem_npu_ability(PROVIDER_PARAS_S* para,
    json_object* type_jso, json_object* value_jso, json_object** msg_array, const guint8 arry_idx)
{
    gint32 ret;
    gdouble value;
    const char* type = NULL;
    json_object* message_jso = NULL;
    GSList* input_list = NULL;
    OBJ_HANDLE obj_handle;

    ret = dal_get_object_handle_nth(CLASS_NPU_ABILITY, 0, &obj_handle);
    if (ret != DFL_OK) {
        debug_log(DLOG_INFO, "%s : get NpuAbility object handle failed", __FUNCTION__);
        (void)create_message_info(MSGID_ACT_NOT_SUPPORTED, NULL, &message_jso, METHOD_NPU_ABILITY_SET_NPU_ABILITY);
        (void)json_object_array_add(*msg_array, message_jso);
        return HTTP_BAD_REQUEST;
    }

    type  = dal_json_object_get_str(type_jso);
    value = json_object_get_double(value_jso);
    
    input_list = g_slist_append(input_list, g_variant_new_string(type));
    input_list = g_slist_append(input_list, g_variant_new_double(value));

    ret = uip_call_class_method_redfish(para->is_from_webui, para->user_name, para->client, obj_handle,
        CLASS_NPU_ABILITY, METHOD_NPU_ABILITY_SET_NPU_ABILITY, AUTH_ENABLE,
        para->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    if (ret != RET_OK) {
        if (ret != NPU_ABILITY_TYPE_OUT_OF_RANGE && ret != NPU_ABILITY_VALUE_OUT_OF_RANGE) {
            ret = NPU_ABILITY_NOT_SUPPORTED;
        }
        create_set_oem_npu_ability_message_info(msg_array, ret, arry_idx);
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}


LOCAL gint32 check_system_oem_huawei_npu_ability(const json_object *array_jso, json_object** msg_array)
{
    gint32 ret = RET_OK;
    gsize jso_array_len;
    json_object *value_jso = NULL;
    json_object *type_jso = NULL;
    json_object *npu_ability_jso = NULL;
    json_bool ret_json;
    gdouble value;
    const char* type = NULL;
    guint8 i = 0;
    
    jso_array_len = json_object_array_length(array_jso);
    if (jso_array_len > NPU_ABILITY_ARRAY_NUM) {
        ret = NPU_ABILITY_ARRAY_EXCEED_MAXNUM;
        goto EXIT;
    }

    for (i = 0; i < jso_array_len; i++) {
        npu_ability_jso = json_object_array_get_idx(array_jso, i);  
        ret_json = json_object_object_get_ex(npu_ability_jso, RFPROP_NPU_ABILITY_TYPE, &type_jso);
        if (ret_json == FALSE) {
            ret = NPU_ABILITY_PROP_TYPE_MISSING;
            goto EXIT;
        }
        type  = dal_json_object_get_str(type_jso);
        if (type == NULL || strlen(type) >= NPU_ABILITY_TYPE_LENGTH || strlen(type) == 0) {
            ret = NPU_ABILITY_TYPE_OUT_OF_RANGE;
            goto EXIT;
        }
        ret_json = json_object_object_get_ex(npu_ability_jso, RFPROP_NPU_ABILITY_VALUE, &value_jso);
        if (ret_json == FALSE) {
            ret = NPU_ABILITY_PROP_VALUE_MISSING;
            goto EXIT;
        }
        value = json_object_get_double(value_jso);
        if (value < 0 || value > NPU_ABILITY_VALUE_MAX) {
            ret = NPU_ABILITY_VALUE_OUT_OF_RANGE;
            goto EXIT;
        }
    }
EXIT:
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s : check_system_oem_huawei_npu_ability fail.", __FUNCTION__);
        create_set_oem_npu_ability_message_info(msg_array, ret, i);
        return HTTP_BAD_REQUEST;
    }
    return HTTP_OK;
}

LOCAL void set_system_oem_huawei_npu_ability(PROVIDER_PARAS_S *i_paras, json_object *huawei,
    json_object **o_message_jso, gint32 *return_array, gint32 *arr_subscript)
{
    gint32 ret;
    json_object *array_jso = NULL;
    json_object *npu_ability_jso = NULL;
    json_object *value_jso = NULL;
    json_object *type_jso = NULL;
    gsize jso_array_len;
    gsize i;
    json_bool ret_json;

    if (json_object_object_get_ex(huawei, RFPROP_NPU_ABILITY, &array_jso) == FALSE) {
        return;
    }
    ret = check_system_oem_huawei_npu_ability(array_jso, o_message_jso);
    if (ret != HTTP_OK) {
        goto EXIT;
    }
    
    jso_array_len = json_object_array_length(array_jso);
    for (i = 0; i < jso_array_len; i++) {
        npu_ability_jso = json_object_array_get_idx(array_jso, i);
        ret_json = json_object_object_get_ex(npu_ability_jso, RFPROP_NPU_ABILITY_TYPE, &type_jso);
        if (ret_json == FALSE) {
            create_set_oem_npu_ability_message_info(o_message_jso, NPU_ABILITY_PROP_TYPE_MISSING, i);
            ret = HTTP_BAD_REQUEST;
            break;
        }
   
        ret_json = json_object_object_get_ex(npu_ability_jso, RFPROP_NPU_ABILITY_VALUE, &value_jso);
        if (ret_json == FALSE) {
            create_set_oem_npu_ability_message_info(o_message_jso, NPU_ABILITY_PROP_VALUE_MISSING, i);
            ret = HTTP_BAD_REQUEST;
            break;
        }
        
        ret = redfish_set_oem_npu_ability(i_paras, type_jso, value_jso, o_message_jso, i);
        if (ret != HTTP_OK) {
            break;
        }
    }
EXIT:
    set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
}

LOCAL void set_system_oem_huawei_by_customized(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object* huawei, gint32* return_array, gint32* arr_subscript)
{
    json_object* property_jso = NULL;
    gint32 ret = 0;
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_CONFIGMODEL, &property_jso)) {
        ret = redfish_set_oem_configuration_model(i_paras, property_jso, o_message_jso);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }

    property_jso = NULL;
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_CMCC_VERSION, &property_jso)) {
        ret = redfish_set_oem_cmcc_version(i_paras, property_jso, o_message_jso);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }

    property_jso = NULL;
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_CUCC_VERSION, &property_jso)) {
        ret = redfish_set_oem_cucc_version(i_paras, property_jso, o_message_jso);
        set_system_oem_huawei_para(return_array, arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }
}

LOCAL gint32 set_oem_thermaltrip_pwrpolicy(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    
    if (!(i_paras->user_role_privilege & USERROLE_POWERMGNT)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_POWER_ON_AFTER_CPU_THERMALTRIP_URI,
            message_array, RFPROP_POWER_ON_AFTER_CPU_THERMALTRIP);
        return HTTP_FORBIDDEN;
    }
    GSList *input_list = NULL;
    OBJ_HANDLE payload_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: payload object does not exist!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    guint8 pwr_policy_val = (guint8)json_object_get_boolean(property_jso);

    input_list = g_slist_append(input_list, g_variant_new_byte(pwr_policy_val));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        payload_handle, CLASS_PAYLOAD, METHOD_SET_THERMAL_TRIP_POWERPOLICY, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret == RET_OK ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;
}

LOCAL gint32 set_oem_thermaltrip_timedelay(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    
    if (!(i_paras->user_role_privilege & USERROLE_POWERMGNT)) {
        (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, RFPROP_DELAY_SECONDS_AFTER_CPU_THERMALTRIP_URI,
            message_array, RFPROP_DELAY_SECONDS_AFTER_CPU_THERMALTRIP);
        return HTTP_FORBIDDEN;
    }
    GSList *input_list = NULL;
    OBJ_HANDLE payload_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &payload_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: payload object does not exist!", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    // schema文件规定范围为0-1800，无效值会被拦截，因此由int32转换为uint32无风险
    guint32 time_delay_val = (guint32)json_object_get_int(property_jso);

    input_list = g_slist_append(input_list, g_variant_new_uint32(time_delay_val));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client,
        payload_handle, CLASS_PAYLOAD, METHOD_SET_THERMAL_TRIP_TIMEDELAY, AUTH_DISABLE,
        i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    return ret == RET_OK ? HTTP_OK : HTTP_INTERNAL_SERVER_ERROR;
}

OEM_SET_PROP_INFO g_oem_set_prop_info_list[] = {
    
    {RFPROP_SYSTEM_COMPUTERSYSTEM_LEAK_STRATEGY,            redfish_set_oem_leak_stragety},

    // 设置面板电源按钮使能
    {RFPROP_SYSTEM_COMPUTERSYSTEM_PANELPOWERBUTTONENABLED,  redfish_set_oem_panel_power_button_enabled},

    
    {RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT_ENABLED,  redfish_set_oem_gracefulpowerofftimeoutenabled},

    
    {RFPROP_SYSTEM_COMPUTERSYSTEM_POWEROFFTIMEOUT,          redfish_set_oem_gracefulpowerofftimeoutperiod},

    
    {RFPROP_SYSTEM_SWITCH_IPINFO,                           redfish_set_oem_switch_ipinfo},

    // 设置系统启动顺序，仅V3支持
    {RFPROP_SYSTEM_COMPUTERSYSTEM_BOOTUPSEQUENCE,           redfish_set_oem_boot_sequence},

    // 设置产品别名
    {RFPROP_SYSTEM_COMPUTERSYSTEM_PRODUCTALIAS,             redfish_set_oem_productalias},

    // 设置启动模式
    {RFPROP_SYSTEM_BOOT_MODE_CHANGE_ENABLED,                set_system_boot_mode_switch},

    // 设置网卡的带宽门限
    {RFPROP_SYSTEM_COMPUTERSYSTEM_NETBANDWIDTHTHRESHOLD,    set_system_net_bandwidth_usagethre},

    // 设置WoL状态
    {RFPROP_SYSTEM_COMPUTERSYSTEM_WOL_STATUS,               set_system_wol_status},

    // 设置CPU过温下电策略使能以及过温后下电再上电间隔秒数
    {RFPROP_POWER_ON_AFTER_CPU_THERMALTRIP,                 set_oem_thermaltrip_pwrpolicy},

    // 设置CPU过温下电再上电间隔秒数
    {RFPROP_DELAY_SECONDS_AFTER_CPU_THERMALTRIP,            set_oem_thermaltrip_timedelay},
};


LOCAL gint32 set_system_oem_huawei(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    gint32 result;
    json_object *huawei = NULL;
    json_object *property_jso = NULL;
    
    gint32 return_array[NUMBER_SYSTEM_ATTRIBUTES_OEM] = {0};
    gint32 arr_subscript = 0;

    
    
    return_val_do_info_if_expr(o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK,
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%d: NULL pointer.", __LINE__));

    (void)json_object_object_get_ex(i_paras->val_jso, RFPROP_COMMON_HUAWEI, &huawei);
    
    return_val_if_expr((0 == g_strcmp0(dal_json_object_to_json_string(i_paras->val_jso), JSON_NULL_OBJECT_STR)) ||
        (0 == g_strcmp0(dal_json_object_to_json_string(huawei), JSON_NULL_OBJECT_STR)),
        HTTP_BAD_REQUEST);

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "json_object_new_array fail");
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_COMPUTERSYSTEM_SYNCENABLE, &property_jso)) {
        ret = redfish_set_oem_hostname_sync_enable(i_paras, property_jso, o_message_jso);
        set_system_oem_huawei_para(return_array, &arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
    }
    set_system_oem_huawei_by_customized(i_paras, o_message_jso, huawei, return_array, &arr_subscript);
    // 设置阈值
    set_system_oem_huawei_threshold(i_paras, huawei, o_message_jso, return_array, &arr_subscript);

    
    ret = set_oem_cpu_pt_state(huawei, i_paras, o_message_jso, return_array, sizeof(return_array), &arr_subscript);
    do_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s : Failed to set oem cpu PT state, ret = %d", __FUNCTION__, ret));

    // 设置通电开机策略
    // 通电开机设置
    set_system_oem_huawei_power_restore(i_paras, huawei, o_message_jso, return_array, &arr_subscript);

    property_jso = NULL;

    for (gsize i = 0; i < G_N_ELEMENTS(g_oem_set_prop_info_list); i++) {
        if (json_object_object_get_ex(huawei, g_oem_set_prop_info_list[i].rf_prop, &property_jso)) {
            gint32 ret = g_oem_set_prop_info_list[i].handler(i_paras, property_jso, o_message_jso);
            set_system_oem_huawei_para(return_array, &arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
        }
        property_jso = NULL;
    }

    // 设置本地KVM使能的状态
    if (json_object_object_get_ex(huawei, RFACTION_SYSTEM_LOCAL_KVM, &property_jso)) {
        ret = set_system_local_kvm(i_paras, o_message_jso, &property_jso);
        
        set_system_oem_huawei_para(return_array, &arr_subscript, ret, NUMBER_SYSTEM_ATTRIBUTES_OEM);
        
    }
    property_jso = NULL;

    
    property_jso = NULL;
    if (json_object_object_get_ex(huawei, RFPROP_SYSTEM_BIOS_ENERGY_MODE_ENABLED, &property_jso) ||
        json_object_object_get_ex(huawei, RFPROP_SYSTEM_BIOS_ENERGY_CONFIGURATION, &property_jso)) {
        redfish_set_intelligent_energy_saving_info(huawei, i_paras, o_message_jso, return_array, &arr_subscript,
            sizeof(return_array));
    }
    
    set_system_oem_huawei_npu_ability(i_paras, huawei, o_message_jso, return_array, &arr_subscript);
    
    
    result = return_value_judgment(arr_subscript, return_array);
    
    return result;
}

LOCAL void get_oem_prop_cpupandt(PROVIDER_PARAS_S *i_paras, guint8 *p_state, guint8 *t_state)
{
    guint32 ret;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;

    return_do_info_if_expr(VOS_OK != provider_paras_check(i_paras) || (NULL == p_state) || (NULL == t_state),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);

    return_do_info_if_expr(VOS_OK != ret,
        debug_log(DLOG_ERROR, "%s, %d:  get object handle  fail", __FUNCTION__, __LINE__));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_GETPT, AUTH_ENABLE, i_paras->user_role_privilege, NULL, &output_list);

    return_do_info_if_expr(VOS_OK != ret, uip_free_gvariant_list(output_list);
        debug_log(DLOG_ERROR, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    *p_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    *t_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));

    uip_free_gvariant_list(output_list);

    return;
}


LOCAL gint32 redfish_set_oem_cpupt(PROVIDER_PARAS_S *i_paras, json_object *property_p_jso, json_object *property_t_jso,
    json_object **message_array, guchar type)
{
    gint32 ret;
    gint32 method_ret = VOS_ERR;
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    gulong running_time = 0;
    guint8 p_state;
    guint8 t_state;
    guint8 p_limit;
    guint8 t_limit;
    guint8 cpu_p_state = 0;
    guint8 cpu_t_state = 0;
    const gchar* prop_name[] = {"Oem/Huawei/CPUPowerAdjustment/CPUPState", "Oem/Huawei/CPUPowerAdjustment/CPUTState"};
    guint8 arm_enabled = 0;

    
    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return method_ret;
    }

    if ((SET_P_STATE != type) && (SET_T_STATE != type)) {
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        return HTTP_NOT_IMPLEMENTED;
    }

    (void)dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_ARM, &arm_enabled);

    if (arm_enabled) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name[type], &message_jso, prop_name[type]);
        (void)json_object_array_add(*message_array, message_jso);
        return HTTP_NOT_IMPLEMENTED;
    }

    ret = dal_get_object_handle_nth(CLASS_NAME_POWERCAPPING, 0, &obj_handle);
    return_val_do_info_if_expr(VOS_OK != ret, method_ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    // 类型不为int返回类型错误
    if (json_type_int != json_object_get_type(property_p_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name[type], &message_jso,
            dal_json_object_get_str(property_p_jso), prop_name[type]);
        (void)json_object_array_add(*message_array, message_jso);

        return VOS_ERR;
    }

    // 类型不为int返回类型错误
    if (json_type_int != json_object_get_type(property_t_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name[type], &message_jso,
            dal_json_object_get_str(property_t_jso), prop_name[type]);
        (void)json_object_array_add(*message_array, message_jso);

        return VOS_ERR;
    }

    p_state = json_object_get_int(property_p_jso);

    t_state = json_object_get_int(property_t_jso);

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_GETPTLIMT, AUTH_ENABLE, i_paras->user_role_privilege, NULL,
        &output_list);
    return_val_do_info_if_expr(VOS_OK != ret, method_ret, uip_free_gvariant_list(output_list);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d:  ret : %d  fail", __FUNCTION__, __LINE__, ret));

    p_limit = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
    t_limit = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 1));
    uip_free_gvariant_list(output_list);

    if ((SET_P_STATE == type && p_limit == 0) || (SET_T_STATE == type && t_limit == 0)) {
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name[type], &message_jso, prop_name[type]);
        (void)json_object_array_add(*message_array, message_jso);
        return method_ret;
    }

    return_val_do_info_if_expr(p_state > p_limit - 1 || t_state > t_limit - 1, method_ret,
        (void)create_message_info(MSGID_VALUE_OUT_OF_RANGE, NULL, &message_jso, prop_name[type]);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'state id out of range.'\n", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_byte(p_state));
    input_list = g_slist_append(input_list, g_variant_new_byte(t_state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_POWERCAPPING, METHOD_POWERCAP_SETPT, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            while (1) {
                vos_task_delay(100);
                running_time += 1;
                get_oem_prop_cpupandt(i_paras, &cpu_p_state, &cpu_t_state);

                if ((cpu_p_state == p_state && cpu_t_state == t_state) || running_time > 50) {
                    break;
                }
            }

            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name[type], &message_jso, prop_name[type]);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        case CPU_PT_STATE_POWER_OFF:
            debug_log(DLOG_DEBUG, "%s: power off cannot set pt state %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_name[type], &message_jso, prop_name[type]);
            (void)json_object_array_add(*message_array, message_jso);
            break;
        case CPU_PT_STATE_OUTOF_RANGE:
            debug_log(DLOG_DEBUG, "%s: out of range return %d", __FUNCTION__, ret);
            if (SET_P_STATE == type) {
                (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, prop_name[type], &message_jso,
                    dal_json_object_get_str(property_p_jso), prop_name[type], 0, p_limit);
            } else {
                (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, prop_name[type], &message_jso,
                    dal_json_object_get_str(property_t_jso), prop_name[type], 0, t_limit);
            }

            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: redfish_set_oem_cpupt return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, prop_name[type], &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL gint32 redfish_set_oem_utilisethreshold(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array, guchar type)
{
    gint32 ret;
    gint32 method_ret = VOS_ERR;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    gint32 threshold;
    
    const gchar* prop_name[] = {"Oem/Huawei/CPUThresholdPercent", "Oem/Huawei/MemoryThresholdPercent", "Oem/Huawei/HardDiskThresholdPercent"};
    

    
    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return method_ret;
    }

    
    ret = dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, method_ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    // 类型不为int返回类型错误
    if (json_type_int != json_object_get_type(property_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_name[type], &message_jso,
            dal_json_object_get_str(property_jso), prop_name[type]);
        (void)json_object_array_add(*message_array, message_jso);

        return VOS_ERR;
    }

    threshold = json_object_get_int(property_jso);

    
    return_val_do_info_if_expr(threshold < 0 || threshold > G_MAXUINT16, method_ret,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));
    

    input_list = g_slist_append(input_list, g_variant_new_byte(type));
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)threshold));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_ME_INFO, METHOD_MEINFO_UTILISE_THRESHOLD, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            method_ret = VOS_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name[type], &message_jso, prop_name[type]);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set threshold value return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL gint32 redfish_set_oem_productalias(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    gint32 method_ret = VOS_ERR;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    gchar product_alias[PROP_VAL_LENGTH + 1] = {0};
    const gchar *prop_err = "Oem/Huawei/ProductAlias";
    const gchar *prop_json = NULL;
    errno_t str_ret;

    
    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    // 在schema里面限制长度	(0-64)
    prop_json = dal_json_object_get_str(property_jso);

    // 可以设置空字符串
    str_ret = strncpy_s(product_alias, sizeof(product_alias), prop_json, sizeof(product_alias) - 1);
    return_val_do_info_if_fail(str_ret == EOK, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'snprintf_s fail.'\n", __FUNCTION__, __LINE__));

    // 中文返回格式错误
    ret = dal_check_string_is_valid_ascii(product_alias);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_err, &message_jso, prop_json, prop_err);
        (void)json_object_array_add(*message_array, message_jso));

    
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    input_list = g_slist_append(input_list, g_variant_new_string(prop_json));

    // 调用方法
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        BMC_PRODUCT_NAME_APP, BMC_METHOD_SETPRODUCTALIAS, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);

    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            method_ret = VOS_OK;
            break;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: redfish_set_oem_productalias return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return method_ret;
}


LOCAL gint32 redfish_set_oem_gracefulpowerofftimeoutperiod(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_err = "Oem/Huawei/SafePowerOffTimoutSeconds";
    guint32 time_int;
    guint32 max_time = 0;
    guint32 min_time = 0;
    gchar max_value[PROP_VAL_LENGTH] = {0};
    gchar min_value[PROP_VAL_LENGTH] = {0};

    
    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso);
        debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));

    // 类型不为int返回类型错误
    if (json_type_int != json_object_get_type(property_jso)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, prop_err, &message_jso, dal_json_object_get_str(property_jso),
            prop_err);
        (void)json_object_array_add(*message_array, message_jso);

        return VOS_ERR;
    }

    time_int = (guint32)json_object_get_int(property_jso);

    input_list = g_slist_append(input_list, g_variant_new_uint32(time_int));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PAYLOAD, METHOD_SET_PWR_OFF_TM, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*message_array, message_jso);
            break;

            // 返回-1，代表时间不在最大值和最小值范围内
        case RF_ERROR:
            // 定义一个消息，要体现设置的值、最大值、最小值
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_MAX, &max_time);
            (void)dal_get_property_value_uint32(obj_handle, PROPERTY_PAYLOAD_PWROFF_TM_MIN, &min_time);
            (void)snprintf_s(max_value, sizeof(max_value), sizeof(max_value) - 1, "%u", max_time);
            (void)snprintf_s(min_value, sizeof(min_value), sizeof(min_value) - 1, "%u", min_time);
            (void)create_message_info(MSGID_PROP_VALUE_OUTOFRANGE, prop_err, &message_jso,
                dal_json_object_get_str(property_jso), prop_err, min_value, max_value);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: redfish_set_oem_gracefulpowerofftimeoutperiod return %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return VOS_ERR;
}


LOCAL gint32 redfish_set_oem_hostname_sync_enable(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_err = "Oem/Huawei/HostnameSyncEnabled";
    guchar prop_name;

    if (message_array == NULL || provider_paras_check(i_paras) != VOS_OK || (property_jso == NULL)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }
    ret = dal_get_object_handle_nth(CLASS_COMPUTER_SYSTEM, 0, &obj_handle);
    return_val_do_info_if_expr(ret != DFL_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s, %d : get ComputerSystem object handle fail, ret is %d .", __FUNCTION__, __LINE__,
        ret);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));

    prop_name = json_object_get_boolean(property_jso) ? ENABLE : DISABLE;
    input_list = g_slist_append(input_list, g_variant_new_byte(prop_name));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_COMPUTER_SYSTEM, METHOD_COMSYS_SET_SYNC_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege, input_list,
        NULL);
    uip_free_gvariant_list(input_list);

    switch (ret) {
        case VOS_OK:
            return VOS_OK;
        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*message_array, message_jso);
            break;
        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }
    return VOS_ERR;
}


LOCAL gint32 redfish_set_oem_gracefulpowerofftimeoutenabled(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle = 0;
    json_object *message_jso = NULL;
    const gchar *prop_err = "Oem/Huawei/SafePowerOffTimeoutEnabled";

    if (NULL == message_array || VOS_OK != provider_paras_check(i_paras) || (NULL == property_jso)) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return VOS_ERR;
    }

    ret = dal_get_object_handle_nth(CLASS_PAYLOAD, 0, &obj_handle);
    return_val_do_info_if_fail(DFL_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s : get payload object handle fail.", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
        (void)json_object_array_add(*message_array, message_jso));

    input_list = g_slist_append(input_list, g_variant_new_uint32(json_object_get_boolean(property_jso)));
    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_PAYLOAD, METHOD_SET_PWR_OFF_TM_EN, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
    uip_free_gvariant_list(input_list);
    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            debug_log(DLOG_DEBUG, "%s: Property Modification Need Privilege,return value is %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            (void)json_object_array_add(*message_array, message_jso);
            break;

        default:
            debug_log(DLOG_DEBUG, "%s: set time out enabled failed", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            (void)json_object_array_add(*message_array, message_jso);
    }

    return VOS_ERR;
}


gint32 system_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    
    gboolean ret;

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (ret == FALSE) {
        debug_log(DLOG_ERROR, "%s: leave system_provider_entry HTTP_NOT_FOUND,url = %s",
            __FUNCTION__, i_paras->uri);
        return HTTP_NOT_FOUND;
    }

    

    *prop_provider = g_system_provider;
    *count = sizeof(g_system_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}


gint32 systems_delete_etag_prop(json_object *object)
{
    SPECIAL_PROP_S value[] = {
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUUSAGE, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYUSAGE, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_PARTITIONLIST, NULL, NULL}},
        
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_BUFFER, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_CACHED, NULL, NULL}},
        {3, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_FREE, NULL, NULL}},
        
        {4, {RFPROP_COMMON_OEM, RFPROP_COMMON_HUAWEI, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPOWERADJUSTMENT, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUPSTATE, NULL}},
        {0, {NULL, NULL, NULL, NULL, NULL}}
    };

    return rsc_del_none_etag_affected_property(object, value, G_N_ELEMENTS(value));
}


LOCAL gint32 get_system_actioninfo_odataid(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    const gchar*         system_action_info_array[] = {
        RFPROP_SYSTEM_RESET,
        RFPROP_SYSTEM_FRUCONTROL
    };
    gint32 ret;
    gchar               system_uri[MAX_URI_LENGTH] = {0};
    gchar               slot_id[MAX_RSC_NAME_LEN] = {0};
    gchar               actioninfo_uri[MAX_URI_LENGTH] = {0};

    (void)redfish_get_board_slot(slot_id, MAX_RSC_NAME_LEN);

    ret = snprintf_s(system_uri, MAX_URI_LENGTH, MAX_URI_LENGTH - 1, URI_FORMAT_SYSTEM, slot_id);
    return_val_do_info_if_fail(ret > 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "format string failed, ret is %d", ret));

    ret = rf_update_rsc_actioninfo_uri(system_action_info_array, G_N_ELEMENTS(system_action_info_array), i_paras->uri,
        system_uri, actioninfo_uri, MAX_URI_LENGTH);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "update action info uri (%s) failed", i_paras->uri));

    *o_result_jso = json_object_new_string(actioninfo_uri);

    return HTTP_OK;
}

LOCAL PROPERTY_PROVIDER_S g_system_action_info_provider[] = {
    {
        RFPROP_ODATA_ID,
        "", "",
        USERROLE_READONLY,
        SYS_LOCKDOWN_NULL,
        get_system_actioninfo_odataid,
        NULL,
        NULL,
        ETAG_FLAG_ENABLE
    }
};


gint32 system_actioninfo_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider, guint32 *count)
{
    gboolean ret;

    return_val_do_info_if_fail((NULL != i_paras) && (NULL != prop_provider) && (NULL != count),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s failed:input param error", __FUNCTION__));

    ret = redfish_check_system_uri_valid(i_paras->uri);
    if (!ret) {
        debug_log(DLOG_ERROR, "%s %d: rsc at %s doesn't exist", __FUNCTION__, __LINE__, i_paras->uri);

        return HTTP_NOT_FOUND;
    }

    *prop_provider = g_system_action_info_provider;
    *count = sizeof(g_system_action_info_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL PROPERTY_PROVIDER_S g_system_memory_usage_rate_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_memory_history_usage_rate_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_memory_history_usage_rate_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_memory_history_usage_rate, NULL, NULL, ETAG_FLAG_DISABLE}
};



LOCAL gint32 get_memory_history_usage_rate_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot) - 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYS_MEMORY_HISTORY_USAGE_RATE, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));
    return HTTP_OK;
}


LOCAL gint32 get_memory_history_usage_rate_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
        SYSTEMS_MEMORY_HISTORY_USAGE_RATE_METADATA, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_memory_history_usage_rate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar file_path[MAX_FILE_PATH_LENGTH + 1] = {0};
    gchar* labels[] = {RFPROP_MEMORY_UTILISE};

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    iRet = snprintf_s(file_path, MAX_FILE_PATH_LENGTH + 1, MAX_FILE_PATH_LENGTH, "%s%s", OPT_PME_PRAM_FOLDER,
        MEMORY_HISTORY_USAGE_RATE_FILE);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    
    
    ret = read_history_data_from_file(file_path, sizeof(labels) / sizeof(char *), labels, o_result_jso,
        READ_HISTORY_UINT16);
    

    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_DEBUG, "%s: read_history_data_from_file fail, ret is %d.", __FUNCTION__, ret));

    return HTTP_OK;
}


LOCAL PROPERTY_PROVIDER_S g_system_network_usage_rate_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_network_history_usage_rate_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_ODATA_CONTEXT, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_network_history_usage_rate_odata_context, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_HISTORY_DATA, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_network_history_usage_rate, NULL, NULL, ETAG_FLAG_DISABLE}
};



LOCAL gint32 get_network_history_usage_rate_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar uri[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot) - 1);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    iRet = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, URI_FORMAT_SYS_NETWORK_HISTORY_USAGE_RATE, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)uri);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_network_history_usage_rate_odata_context(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    int iRet;
    gint32 ret;
    gchar odata_context[MAX_URI_LENGTH + 1] = {0};
    gchar slot[MAX_RSC_NAME_LEN + 1] = {0};

    
    return_val_do_info_if_fail(NULL != o_result_jso && NULL != o_message_jso && VOS_OK == provider_paras_check(i_paras),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: redfish_get_board_slot fail.", __FUNCTION__));

    iRet = snprintf_s(odata_context, sizeof(odata_context), sizeof(odata_context) - 1,
        SYSTEMS_NETWORK_HISTORY_USAGE_RATE_METADATA, slot);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    *o_result_jso = json_object_new_string((const char *)odata_context);
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_string fail", __FUNCTION__));

    return HTTP_OK;
}


LOCAL gint32 get_businessport_silknum(OBJ_HANDLE netcard_handle, OBJ_HANDLE port_handle, gint32 labels_cnt,
    OBJ_HANDLE *port_handle_array)
{
    gint32 ret;
    OBJ_HANDLE ref_netcard_handle = 0;
    guchar silknum = INVALID_DATA_BYTE;

    ret = dfl_get_referenced_object(port_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_netcard_handle);
    return_val_if_expr(ret != VOS_OK, VOS_OK);

    if (ref_netcard_handle == netcard_handle) {
        ret = dal_get_property_value_byte(port_handle, BUSY_ETH_ATTRIBUTE_SILK_NUM, &silknum);
        return_val_if_expr(ret != VOS_OK, VOS_OK);
        return_val_do_info_if_expr(silknum < MIN_SILKNUM, VOS_ERR,
            debug_log(DLOG_ERROR, "%s: get property %s.silknum is invaild(silknum = %d).", __FUNCTION__,
            dfl_get_object_name(port_handle), silknum));

        do_if_expr(silknum <= labels_cnt, (port_handle_array[silknum - 1] = port_handle));
    }
    return VOS_OK;
}

LOCAL gint32 get_netcard_current_usage_rate(OBJ_HANDLE netcard_handle, gint32 labels_cnt, gchar **labels,
    json_object **result_jso)
{
    gint32 ret;
    gint32 result = VOS_OK;
    GSList *port_obj_list = NULL;
    GSList *port_obj_node = NULL;
    OBJ_HANDLE *port_handle_array = NULL;
    gint32 i;
    json_object *jso_obj = NULL;
    guint16 usage_rate = INVALID_DATA_WORD;

    return_val_if_expr(netcard_handle == 0 || labels_cnt <= 0 || labels == NULL || result_jso == NULL, VOS_ERR);
    *result_jso = json_object_new_object();
    return_val_do_info_if_expr(*result_jso == NULL, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: call json_object_new_array fail", __FUNCTION__));

    port_handle_array = (OBJ_HANDLE *)g_malloc0(labels_cnt * sizeof(OBJ_HANDLE));
    goto_label_do_info_if_expr(port_handle_array == NULL, exit, result = VOS_ERR;
        debug_log(DLOG_ERROR, "%s:g_malloc0 port_handle_array fail", __FUNCTION__));
    
    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &port_obj_list);
    goto_label_do_info_if_expr(ret != VOS_OK || port_obj_list == NULL, exit, result = VOS_ERR;
        debug_log(DLOG_ERROR, "%s: get port obj handle list fail", __FUNCTION__));

    
    for (port_obj_node = port_obj_list; port_obj_node; port_obj_node = port_obj_node->next) {
        ret = get_businessport_silknum(netcard_handle, (OBJ_HANDLE)port_obj_node->data, labels_cnt, port_handle_array);
        goto_label_do_info_if_expr(ret != VOS_OK, exit, result = VOS_ERR;
            debug_log(DLOG_ERROR, "%s:get silknum fail!", __FUNCTION__));
    }

    
    for (i = 0; i < labels_cnt; i++) {
        jso_obj = NULL;
        if (port_handle_array[i] != 0) {
            ret = dal_get_property_value_uint16(port_handle_array[i], BUSY_ETH_ATTRIBUTE_BANDWIDTH_USAGE, &usage_rate);
            continue_do_info_if_expr(ret != VOS_OK, json_object_object_add(*result_jso, (const gchar *)labels[i], NULL);
                debug_log(DLOG_ERROR, "%s:get port bandwidth usage fail, ret:%d", __FUNCTION__, ret));
            
            do_if_expr(usage_rate <= 100 * 100,
                (jso_obj = ex_json_object_new_double((((gdouble)usage_rate) / 100), "%.2f")));
        }

        
        json_object_object_add(*result_jso, (const gchar *)labels[i], jso_obj);
    }

exit:
    do_if_expr(port_handle_array != NULL, (g_free(port_handle_array)));
    do_info_if_true(result != VOS_OK, json_object_put(*result_jso); *result_jso = NULL);
    do_if_expr(port_obj_list != NULL, (g_slist_free(port_obj_list)));
    return result;
}


LOCAL gint32 get_netcard_history_usage_rate_with_handle(OBJ_HANDLE obj_handle, json_object **item_jso)
{
    int iRet;
    gint32 ret;
    gint32 i;
    gchar wave_title[PROP_VAL_MAX_LENGTH + 1] = {0};
    gchar file_name[MAX_FILE_PATH_LENGTH + 1] = {0};
    gchar file_path[MAX_FILE_PATH_LENGTH + 1] = {0};
    guint8 num_of_ports = 0;
    gchar **labels = NULL;
    json_object *wave_title_jso = NULL;
    json_object *data_arr_jso = NULL;
    json_object *current_usage_jso = NULL;

    
    return_val_do_info_if_fail(NULL != item_jso, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    ret = dal_get_property_value_string(obj_handle, BUSY_NETCARD_BWU_WAVE_TITLE, wave_title, sizeof(wave_title));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get wave_title fail, ret is %d.", __FUNCTION__, ret));

    
    ret = dal_get_property_value_string(obj_handle, BUSY_NETCARD_BWU_WAVE_FILENAME, file_name, sizeof(file_name));
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get num of ports fail, ret is %d.", __FUNCTION__, ret));

    
    if (0 == strcasecmp(file_name, INVALID_DATA_STRING)) {
        debug_log(DLOG_DEBUG, "%s: %s is invalid", __FUNCTION__, BUSY_NETCARD_BWU_WAVE_FILENAME);
        return VOS_ERR;
    }

    
    iRet =
        snprintf_s(file_path, MAX_FILE_PATH_LENGTH + 1, MAX_FILE_PATH_LENGTH, "%s%s", OPT_PME_PRAM_FOLDER, file_name);
    do_val_if_expr(iRet <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, iRet));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_NETCARD_PORTNUM, &num_of_ports);
    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get num of ports fail, ret is %d.", __FUNCTION__, ret));

    
    
    labels = (gchar **)g_malloc0((num_of_ports + 1) * sizeof(gchar *));
    return_val_do_info_if_fail(NULL != labels, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: g_malloc0 labels fail.", __FUNCTION__));

    
    for (i = 0; i < num_of_ports; i++) {
        labels[i] = (gchar *)g_malloc0((MAX_LABEL_LENGTH + 1) * sizeof(gchar));
        return_val_do_info_if_fail(NULL != labels[i], VOS_ERR, g_strfreev(labels);
            debug_log(DLOG_ERROR, "%s: g_malloc0 labels[%d] fail.", __FUNCTION__, i));

        (void)snprintf_s(labels[i], MAX_LABEL_LENGTH + 1, MAX_LABEL_LENGTH, "Port%dUtilisePercents", (i + 1));
    }

    
    
    
    ret = read_history_data_from_file(file_path, num_of_ports, labels, &data_arr_jso, READ_HISTORY_DOUBLE);
    

    return_val_do_info_if_fail(VOS_OK == ret, VOS_ERR, g_strfreev(labels);
        debug_log(DLOG_DEBUG, "%s: read_history_data_from_file fail, ret is %d.", __FUNCTION__, ret));

    
    (void)get_netcard_current_usage_rate(obj_handle, num_of_ports, labels, &current_usage_jso);
    
    g_strfreev(labels);

    
    *item_jso = json_object_new_object();
    wave_title_jso = json_object_new_string(wave_title);
    json_object_object_add(*item_jso, RFPROP_NETWORK_BWU_WAVE_TITLE, wave_title_jso);
    json_object_object_add(*item_jso, RFPROP_CURRENT_NETWORK_UTILISE, current_usage_jso);
    json_object_object_add(*item_jso, RFPROP_NETWORK_UTILISE, data_arr_jso);

    return VOS_OK;
}


LOCAL guint32 get_netcard_busiport_type(OBJ_HANDLE card_handle)
{
    gint32 ret = RET_OK;
    GSList *busi_list = NULL;
    GSList *busi_node = NULL;
    OBJ_HANDLE busi_handle = 0;
    OBJ_HANDLE ref_handle = 0;
    guint32 func_type;
    guint32 card_busi_type = NETCARD_BUSIPORT_TYPE_UNKNOWN;

    ret = dfl_get_object_list(ETH_CLASS_NAME_BUSY_ETH, &busi_list);
    if (ret != DFL_OK || busi_list == NULL) {
        debug_log(DLOG_DEBUG, "%s:get busi_list failed", __FUNCTION__);
        return card_busi_type;
    }

    for (busi_node = busi_list; busi_node; busi_node = busi_node->next) {
        busi_handle = (OBJ_HANDLE)busi_node->data;

        ret = dfl_get_referenced_object(busi_handle, BUSY_ETH_ATTRIBUTE_REF_NETCARD, &ref_handle);
        
        if (ret != DFL_OK || card_handle != ref_handle) {
            continue;
        }
        func_type = 0;
        ret = dal_get_property_value_uint32(busi_handle, BUSY_ETH_ATTRIBUTE_SUPPORT_FUNCTYPE, &func_type);
        if (ret != RET_OK) {
            continue;
        }
        if (func_type & NETDEV_FUNCTYPE_FC) {
            card_busi_type |= NETCARD_BUSIPORT_TYPE_FC;
        } else if (func_type & NETDEV_FUNCTYPE_ETHERNET) {
            card_busi_type |= NETCARD_BUSIPORT_TYPE_ETH;
        } else if (func_type & NETDEV_FUNCTYPE_IB) {
            card_busi_type |= NETCARD_BUSIPORT_TYPE_IB;
        }
    }

    g_slist_free(busi_list);
    return card_busi_type;
}


LOCAL gint32 get_network_history_usage_rate(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    guchar virtual_flag = 0;
    GSList *obj_list = NULL;
    GSList *obj_list_iter = NULL;
    OBJ_HANDLE iter_handle = 0;
    json_object *arr_jso = NULL;
    json_object *item_jso = NULL;
    guint32 type;

    
    return_val_do_info_if_fail((NULL != i_paras) && (NULL != o_message_jso) && (NULL != o_result_jso),
        HTTP_INTERNAL_SERVER_ERROR, debug_log(DLOG_ERROR, "%s: input param error.", __FUNCTION__));

    
    ret = dfl_get_object_list(CLASS_NETCARD_NAME, &obj_list);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get class NetCard obj_list fail, ret is %d.", __FUNCTION__, ret));

    
    arr_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != arr_jso, HTTP_INTERNAL_SERVER_ERROR, g_slist_free(obj_list);
        debug_log(DLOG_ERROR, "%s: new data_arr fail.", __FUNCTION__));

    
    for (obj_list_iter = obj_list; NULL != obj_list_iter; obj_list_iter = obj_list_iter->next) {
        
        iter_handle = (OBJ_HANDLE)obj_list_iter->data;

        
        ret = dal_get_property_value_byte(iter_handle, PROPERTY_NETCARD_VIRTUAL_FLAG, &virtual_flag);
        continue_do_info_if_expr(((ret == VOS_OK) && (virtual_flag == AVAILABLE)),
            debug_log(DLOG_DEBUG, "%s: this is virtual eth card.", __FUNCTION__));
        
        if (g_strrstr(dfl_get_object_name(iter_handle), DYNAMIC_OBJ_ETH_CARD) != NULL) {
            debug_log(DLOG_DEBUG, "%s: dynamic virtual netcard, not support historical usage rate.", __FUNCTION__);
            continue;
        }
        type = get_netcard_busiport_type(iter_handle);
        if ((type == NETCARD_BUSIPORT_TYPE_FC) || (type == NETCARD_BUSIPORT_TYPE_UNKNOWN)) {
            debug_log(DLOG_DEBUG, "%s: fc or unkonwn netcard, not support historical usage rate", __FUNCTION__);
            continue; 
        }
        
        ret = get_netcard_history_usage_rate_with_handle(iter_handle, &item_jso);
        continue_do_info_if_fail((VOS_OK == ret) && (NULL != item_jso),
            debug_log(DLOG_DEBUG, "%s: get_netcard_history_usage_rate fail.", __FUNCTION__));

        ret = json_object_array_add(arr_jso, item_jso);
        do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s: json_object_array_add fail.", __FUNCTION__));

        item_jso = NULL;
    }

    g_slist_free(obj_list);
    *o_result_jso = arr_jso;
    return HTTP_OK;
}


gint32 system_memory_usage_rate_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    gboolean bool_ret;
    guchar board_type = 0;

    
    ret = redfish_get_x86_enable_type(&board_type);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: redfish_get_x86_enable_type fail, ret is %d.", __FUNCTION__, ret));
    return_val_do_info_if_fail(ENABLE == board_type, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s: board_type is disable.", __FUNCTION__));
    
    ret = get_process_memory_history_supported(i_paras);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    
    
    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_do_info_if_fail(TRUE == bool_ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: leave system_memory_usage_rate_provider_entry HTTP_NOT_FOUND", __FUNCTION__));

    *prop_provider = g_system_memory_usage_rate_provider;
    *count = sizeof(g_system_memory_usage_rate_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


gint32 system_network_usage_rate_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;
    gboolean bool_ret;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dal_get_object_handle_nth(CLASS_NETCARD_NAME, 0, &obj_handle);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_NOT_FOUND,
        debug_log(DLOG_DEBUG, "%s: get NetCard obj_handle fail, ret is %d.", __FUNCTION__, ret));

    
    bool_ret = redfish_check_system_uri_valid(i_paras->uri);
    return_val_do_info_if_fail(TRUE == bool_ret, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: leave system_network_usage_rate_provider_entry HTTP_NOT_FOUND.", __FUNCTION__));
    
    ret = get_bma_connect_state(i_paras);
    return_val_if_fail(VOS_OK == ret, HTTP_NOT_FOUND);
    
    *prop_provider = g_system_network_usage_rate_provider;
    *count = sizeof(g_system_network_usage_rate_provider) / sizeof(PROPERTY_PROVIDER_S);

    return VOS_OK;
}


LOCAL void get_system_version(json_object *o_result_jso)
{
    // 变量定义
    gint32 ret;                                             // 返回值
    OBJ_HANDLE obj_handle = 0;                              // 类ComputerSystem 的对象的句柄
    gchar version_string[RF_SYSTEM_OSVERSION_MAXLEN];       // osversion 值
    gchar kernalversion_string[RF_SYSTEM_OSVERSION_MAXLEN]; // kernalversion 值
    json_object *osversion_result_jso = NULL;               // osversion 值的 json形式
    json_object *kernalversion_result_jso = NULL;           // kernalversion 值的 json形式

    // 变量初始化
    (void)memset_s(version_string, sizeof(version_string), 0, sizeof(version_string)); // 字符数组初始化
    (void)memset_s(kernalversion_string, sizeof(kernalversion_string), 0,
        sizeof(kernalversion_string)); // 字符数组初始化

    // 获取类"ComputerSystem" 对象的句柄
    // COMPUTERSYSTEM_CLASS_NAME
    ret = dal_get_object_handle_nth(CLASS_COMPUTER_SYSTEM, 0, &obj_handle);
    if (VOS_OK == ret) {
        // 获取类对象中属性"OSVersion"的值
        // "OSVersion"的的宏定义为:  PROPERTY_COMPUTER_SYSTEM_OSVERSION
        ret = dal_get_property_value_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_OSVERSION, version_string,
            RF_SYSTEM_OSVERSION_MAXLEN);
        return_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

        // 获取类对象中属性"KernalVersion"的值
        // "KernalVersion" 的的宏定义为:  PROPERTY_COMPUTER_SYSTEM_KERNEL_VERSION
        ret = dal_get_property_value_string(obj_handle, PROPERTY_COMPUTER_SYSTEM_KERNEL_VERSION, kernalversion_string,
            RF_SYSTEM_OSVERSION_MAXLEN);
        return_do_info_if_fail(VOS_OK == ret,
            debug_log(DLOG_ERROR, "%s, %d: 'get property value fail.'\n", __FUNCTION__, __LINE__));

        
        if (rf_string_check(kernalversion_string) == VOS_OK) {
            // 将属性"KernalVersion"的字符串值转换成为json的string类型
            kernalversion_result_jso = json_object_new_string(kernalversion_string);
            do_if_expr(NULL == kernalversion_result_jso,
                debug_log(DLOG_ERROR, "%s, %d 'json_object_new_string fail' ", __FUNCTION__, __LINE__));
        }

        
        if (rf_string_check(version_string) == VOS_OK) {
            // 将属性"OSVersion"的字符串值转换成为json的string类型
            osversion_result_jso = json_object_new_string(version_string);
            do_if_expr(NULL == osversion_result_jso,
                debug_log(DLOG_ERROR, "%s, %d 'json_object_new_string fail' ", __FUNCTION__, __LINE__));
        }
    }

    

    // 将属性"osversion" 和"Kernalversion"加入到 *o_result_jso 分配的空间中
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_OSVERSION, osversion_result_jso); // "OSVersion"
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_COMPUTERSYSTEM_KERNALVERSION,
        kernalversion_result_jso); // ""KernalVersion"
    

    return;
}


LOCAL void get_system_product(json_object *o_result_jso)
{
    gint32 ret;      // 返回值
    OBJ_HANDLE obj_handle = 0; // 类ComputerSystem 的对象的句柄

    
    ret = dal_get_object_handle_nth(BMC_PRODUCT_NAME_APP, 0, &obj_handle);
    return_do_info_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: get object handle fail, ret:%d\n", __FUNCTION__, __LINE__, ret));

    rf_add_property_jso_string(obj_handle, PROPERTY_PRODUCT_ALIAS, RFPROP_SYSTEM_COMPUTERSYSTEM_PRODUCTALIAS,
        o_result_jso);

    
    rf_add_property_jso_string(obj_handle, BMC_PRODUCT_PICTURE, RFPROP_SERVICEROOT_PRODUCT_PIC, o_result_jso);

    
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    do_if_fail(VOS_OK == ret,
        debug_log(DLOG_ERROR, "%s, %d: get object handle fail, ret:%d\n", __FUNCTION__, __LINE__, ret));

    rf_add_property_jso_string(obj_handle, PROPERTY_BMC_DEV_SERIAL, RFPROP_SYSTEM_DEV_SERIALNUMBER, o_result_jso);

    return;
}


LOCAL void add_cpu_memory_disk_usage_info(json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;

    // 后插板，获取对象句柄失败，不返回
    (void)dal_get_object_handle_nth(CLASS_ME_INFO, 0, &obj_handle);
    
    get_oem_prop_threshold_uint16(obj_handle, PROPERTY_MEINFO_CPU_UTILISE, RFPROP_SYSTEM_COMPUTERSYSTEM_CPUUSAGE,
        o_result_jso);

    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_MEINFO_CPU_UTILISE_THRESHOLD,
        RFPROP_SYSTEM_COMPUTERSYSTEM_CPUTHRESHOLD, o_result_jso);

    
    get_oem_prop_threshold_uint16(obj_handle, PROPERTY_MEINFO_MEM_UTILISE, RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYUSAGE,
        o_result_jso);

    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_MEINFO_MEM_UTILISE_THRESHOLD,
        RFPROP_SYSTEM_COMPUTERSYSTEM_MEMORYTHRESHOLD, o_result_jso);

    
    rf_add_property_jso_uint16(obj_handle, PROPERTY_MEINFO_DISK_UTILISE_THRESHOLD,
        RFPROP_SYSTEM_COMPUTERSYSTEM_HDDTHRESHOLD, o_result_jso);
}


LOCAL void add_prop_power_state(json_object *o_result_jso)
{
    gint32 ret;
    guchar power_state_byte = 0;
    gchar *power_state_str = NULL;

    ret = rf_get_system_powerstate(&power_state_byte);
    if (ret != VOS_OK) {
        goto ERR_EXIT;
    }

    power_state_str = rfsystem_powerstate_to_string(power_state_byte);
    if (power_state_str == NULL) {
        goto ERR_EXIT;
    }

    json_object_object_add(o_result_jso, RFPROP_SYSTEM_POWERSTATE,
        json_object_new_string((const gchar *)power_state_str));
    return;

ERR_EXIT:
    json_object_object_add(o_result_jso, RFPROP_SYSTEM_POWERSTATE, NULL);
    return;
}


LOCAL void add_prop_iBMA_running_status(json_object *o_result_jso)
{
    OBJ_HANDLE obj_handle = 0;

    (void)dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    rf_add_property_jso_string(obj_handle, PROPERTY_SMS_STATUS, RFPROP_SYSTEM_COMPUTERSYSTEM_IBMA_STATUS, o_result_jso);
}


LOCAL gint32 get_system_teeosversion(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso, json_object** o_result_jso)
{
    gint32 ret = 0;
    OBJ_HANDLE obj_handle = 0;
    gchar version_string[RF_SYSTEM_BOSVERSION_MAXLEN];

    
    if (o_result_jso == NULL || o_message_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    if (!(i_paras->is_satisfy_privi)) {
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso);
        return HTTP_FORBIDDEN;
    }

    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get o_obj_handle fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    ret = dal_get_property_value_string(obj_handle, BMC_TEEOS_VER_NAME, version_string, RF_SYSTEM_BOSVERSION_MAXLEN);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    if (rf_string_check(version_string) != RET_OK) {
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    *o_result_jso = json_object_new_string(version_string);

    return HTTP_OK;
}


LOCAL void get_bios_and_teeos_version(PROVIDER_PARAS_S* i_paras, json_object** o_message_jso,
    json_object *obj_jso)
{
    gint32 ret;
    json_object *obj_property = NULL;
    
    ret = get_system_biosversion(i_paras, o_message_jso, &obj_property);
    if (ret != HTTP_OK) {
        debug_log(DLOG_INFO,
            "%s: function  return err of get_system_biosversion.", __FUNCTION__);
    }
    json_object_object_add(obj_jso, RFPROP_SYSTEM_BIOSVERSION, obj_property);

    obj_property = NULL;
    
    ret = get_system_teeosversion(i_paras, o_message_jso, &obj_property);
    if (ret != HTTP_OK) {
        debug_log(DLOG_INFO,
            "%s: function  return err of get_system_teeosversion.", __FUNCTION__);
    }
    json_object_object_add(obj_jso, RFPROP_SYSTEM_TEEOSVERSION, obj_property);
}


gint32 get_system_overview_systems(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    gint32 ret;
    json_object *obj_jso = NULL;
    json_object *obj_property = NULL;
    OBJ_HANDLE obj_handle = 0;

    
    return_val_do_info_if_expr(NULL == o_message_jso || NULL == o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: NULL pointer.", __FUNCTION__, __LINE__));

    ret = provider_paras_check(i_paras);
    return_val_do_info_if_fail(VOS_OK == ret, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d: paras check fail", __FUNCTION__, __LINE__));

    
    return_val_do_info_if_fail(i_paras->is_satisfy_privi, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_array();
    return_val_do_info_if_fail(NULL != *o_result_jso, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s,%d:   json_object_new_array fail. ", __FUNCTION__, __LINE__));

    
    obj_jso = json_object_new_object();
    return_val_do_info_if_fail(NULL != obj_jso, HTTP_INTERNAL_SERVER_ERROR, json_object_put(*o_result_jso);
        debug_log(DLOG_ERROR, "%s,%d:   json_object_new_object fail. ", __FUNCTION__, __LINE__));

    
    ret = get_common_id(i_paras, o_message_jso, &obj_jso);
    do_if_expr(HTTP_OK != ret,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of get_common_id.", __FUNCTION__, __LINE__));

    get_bios_and_teeos_version(i_paras, o_message_jso, obj_jso);

    
    get_system_version(obj_jso);

    
    get_system_product(obj_jso);

    
    ret = dal_get_object_handle_nth(BMC_CLASEE_NAME, 0, &obj_handle);
    do_if_fail(VOS_OK == ret, debug_log(DLOG_ERROR, "%s, %d: 'get object handle fail.'\n", __FUNCTION__, __LINE__));
    rf_add_property_jso_string(obj_handle, PROPERTY_BMC_SYSTEMNAME, RFPROP_SERVICEROOT_PRODUCT_NAME, obj_jso);

    add_cpu_memory_disk_usage_info(obj_jso);

    
    ret = get_system_memorys_count(i_paras, o_message_jso, &obj_property);
    do_if_expr(HTTP_OK != ret,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of get_system_memorys_count.", __FUNCTION__, __LINE__));
    json_object_object_add(obj_jso, RFPROP_SYSTEM_OVERVIEW_MEMORY_COUNT, obj_property);

    obj_handle = OBJ_HANDLE_COMMON;
    (void)dal_get_object_handle_nth(CLASS_SMS, 0, &obj_handle);
    rf_add_property_jso_bool(obj_handle, PROPERTY_SMS_ENABLED, RFPROP_SYSTEM_COMPUTERSYSTEM_SMS_ENABLED, obj_jso);

    
    redfish_get_oem_logical_drive_summary(obj_jso);

    
    redfish_get_oem_storage_summary(obj_jso);

    
    _rf_add_system_health_summary_info(obj_jso);

    
    obj_property = NULL;
    ret = get_system_processorsummary(i_paras, o_message_jso, &obj_property);
    do_val_if_expr(HTTP_OK != ret,
        debug_log(DLOG_INFO, "%s, %d: function  return err of get_system_processorsummary.", __FUNCTION__, __LINE__));
    json_object_object_add(obj_jso, RFPROP_SYSTEM_PROCESSORSUMMARY, obj_property);

    
    obj_property = NULL;
    ret = get_system_memorysummary(i_paras, o_message_jso, &obj_property);
    do_val_if_expr(HTTP_OK != ret,
        debug_log(DLOG_INFO, "%s, %d: function  return err of get_system_memorysummary.", __FUNCTION__, __LINE__));
    json_object_object_add(obj_jso, RFPROP_SYSTEM_MEMORYSUMMARY, obj_property);

    
    get_oem_partitionlists_object(obj_jso);

    
    add_prop_power_state(obj_jso);

    
    add_prop_iBMA_running_status(obj_jso);

    (void)json_object_array_add(*o_result_jso, obj_jso);

    return HTTP_OK;
}



LOCAL void get_oem_prop_leakpolicy(OBJ_HANDLE obj_handle, const gchar *property, const gchar *rf_property,
    json_object *o_result_jso)
{
    json_object *property_jso = NULL;
    guchar prop_val = 0;

    return_do_info_if_expr((property == NULL) || (rf_property == NULL) || (o_result_jso == NULL),
        debug_log(DLOG_ERROR, "%s:input param error", __FUNCTION__));

    (void)dal_get_property_value_byte(obj_handle, property, &prop_val);
    if (prop_val == KEEP_PWR_STATUS_AFTER_LEAKAGE) {
        property_jso = json_object_new_string(RFPROP_SYSTEM_LEAK_STRATEGY_MANUAL_PWROFF);
    } else if (prop_val == POWER_OFF_AFTER_LEAKAGE) {
        property_jso = json_object_new_string(RFPROP_SYSTEM_LEAK_STRATEGY_AUTO_PWROFF);
    } else {
        property_jso = NULL;
    }

    do_if_expr(property_jso == NULL, debug_log(DLOG_DEBUG, "%s:json_object_new_int fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}


LOCAL gint32 redfish_set_oem_leak_stragety(PROVIDER_PARAS_S *i_paras, json_object *property_jso,
    json_object **message_array)
{
    gint32 ret = HTTP_BAD_REQUEST;
    const gchar *str_leak_stragety = NULL;
    guint8 leak_stragety = KEEP_PWR_STATUS_AFTER_LEAKAGE;
    GSList *input_list = NULL;
    OBJ_HANDLE obj_handle;
    json_object *message_jso = NULL;
    const gchar *prop_err = "Oem/Huawei/LeakStrategy";
    guint8 leakdet_support;

    return_val_do_info_if_expr((message_array == NULL) || (provider_paras_check(i_paras) != VOS_OK) ||
        (property_jso == NULL),
        VOS_ERR, debug_log(DLOG_ERROR, "%s : Input parameter error", __FUNCTION__));

    leakdet_support = dal_get_leakdetect_support();
    goto_label_do_info_if_expr(leakdet_support == PME_SERVICE_UNSUPPORT, func_end,
        (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_err, &message_jso, prop_err));

    str_leak_stragety = dal_json_object_get_str(property_jso);
    if (g_strcmp0(str_leak_stragety, RFPROP_SYSTEM_LEAK_STRATEGY_MANUAL_PWROFF) == 0) {
        leak_stragety = KEEP_PWR_STATUS_AFTER_LEAKAGE;
    } else if (g_strcmp0(str_leak_stragety, RFPROP_SYSTEM_LEAK_STRATEGY_AUTO_PWROFF) == 0) {
        leak_stragety = POWER_OFF_AFTER_LEAKAGE;
    } else {
        debug_log(DLOG_ERROR, "%s : invalid leak stragety is %s", __FUNCTION__, str_leak_stragety);
        (void)create_message_info(MSGID_PROP_NOT_IN_LIST, NULL, &message_jso, str_leak_stragety, prop_err);
        goto func_end;
    }

    ret = dfl_get_object_handle(COOLINGCLASS, &obj_handle);
    if (ret == VOS_OK) {
        input_list = g_slist_append(input_list, g_variant_new_byte(leak_stragety));
        ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
            COOLINGCLASS, METHOD_COOLING_SET_LEAK_POLICY, AUTH_ENABLE, i_paras->user_role_privilege, input_list, NULL);
        uip_free_gvariant_list(input_list);
    }
    switch (ret) {
        case VOS_OK:
            return VOS_OK;

        case RFERR_INSUFFICIENT_PRIVILEGE:
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_err, &message_jso, prop_err);
            ret = HTTP_FORBIDDEN;
            goto func_end;

        case COMP_CODE_STATUS_INVALID:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, prop_err, &message_jso, prop_err);
            ret = HTTP_BAD_REQUEST;
            goto func_end;

        default:
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, &message_jso);
            ret = HTTP_INTERNAL_SERVER_ERROR;
            goto func_end;
    }

func_end:
    (void)json_object_array_add(*message_array, message_jso);
    return ret;
}


LOCAL void get_oem_prop_leakdetsupport(guint8 leakdet_support, const gchar *rf_property, json_object *o_result_jso)
{
    json_object *property_jso = NULL;
    return_do_info_if_expr((rf_property == NULL) || (o_result_jso == NULL),
        debug_log(DLOG_ERROR, "%s : input param error", __FUNCTION__));

    property_jso = json_object_new_boolean(leakdet_support);
    do_if_expr(property_jso == NULL, debug_log(DLOG_ERROR, "%s : json_object_new_boolean fail", __FUNCTION__));

    json_object_object_add(o_result_jso, rf_property, property_jso);

    return;
}
