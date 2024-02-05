

#ifndef __BMC_H__
#define __BMC_H__

#include <curl/curl.h>
#include <syslog.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define VER_MAX_LEN 64 // 版本最大的长度
#define VALUE_LEN 1024
#define BMC_TEMP_BUF_LEN 128
#define MAX_MAC_ADDR_LENS 30
#ifdef ARM64_HI1711_ENABLED
#define IBMC_VERSION_VR 0x20 //  1711使用新的V版本号
#else
#define IBMC_VERSION_VR 0x10 // GetDeviceID 返回值byte15，用于区分V2和V3版本
#endif

#define IPMI_IMU_GET_DATA    0x98
#define IPMI_IMU_GET_OS_TIME 0x1a

#define BMC_OBJECT_HANDLE_NAME 0x01010101
#define BMC_OBJECT_CONTACT_NAME 0x01010102

#define IPMI_VERIFY_H2P_INFO_RESP_DATA_LOAD_FAIL	0x01
#define IPMI_VERIFY_H2P_INFO_RESP_DATA_VERIFY_FAIL	0x02

#define IPMI_GET_EFUSE_TYPE_NOPARTNER				0x00
#define IPMI_GET_EFUSE_TYPE_PARTNER			        0x01

#define IPMI_GET_EFUSE_STATE_UNKNOW                    0xFF
#define IPMI_GET_EFUSE_STATE_NOEFUSE                   0x00
#define IPMI_GET_EFUSE_STATE_EFUSEON                   0x01

#define BMC_OPEN_BOX_SUB_CMD 0x05
#define BMC_OPEN_BOX_DEV_ID 0x09
#define BMC_CLRPW_SIG_DETECT_DELAY 500 // 查询bmc_clear_pw管脚信号时间间隔ms

#define BMC_SYNC_TIME_DELAY 6000
#define CERT_CHECK_TIME (200 * 1000)
#define CERT_CHECK_TIME_DELAY (2 * 60 * 1000)


#define TIME_ZONE_FULL_INIT         1
#define TIME_ZONE_PARTIAL_INIT      2
#define TIME_ZONE_INIT_WITHOUT_DST  3 // init阶段不执行夏令时初始化

#define BMC_CONTACT_COPYRIGHT_LEN 255
#define SNMP_SYS_CONTACT_LEN 255
#define CONTACT_OFFICAL_WEB_LEN 255

#define BMC_VAILD_TIME      0x386D4380UL 
#define BMC_NODE_NUM        8
#define BMC_GET_SEL_TIME    0x48
#define RESQ_IPMI_SIZE      148 

#define BMC_CPLD_VER_REG_NAME ("CpldVerReg")
#define BMC_GET_BCD_MAJOR(x) (((x) >> 4) & 0x0f)
#define BMC_GET_BCD_MINOR(x) ((x)&0x0f)

#define BMC_MAX_MONTH_NUM 12
#define BMC_GUID_DATA_OFFSET 4

#define BOARD_OBJ_SLOT_ID_FAULT 1
#define BOARD_OBJ_HW_ADDR_FAULT 2
#define BOARD_OBJ_BOARD_ID_FAULT 3
#define BMC_BOARD_ID_BIT_INDEX 4
#define ONE_DAY_MILESECOND (24 * 60 * 60 * 1000UL)

#define MAX_SYS_INFO_BLOCK 16
#define MAX_SHARE_DATA_INFO_BLOCK 256
#define SYS_INFO_BLOCK_LEN 16

#define LANGUAGE_REPEAT_ERR 2

#define GET_OEM_SUPPORT_STATE(kvm, sol, aps, car, fwt, mac, ver) \
    (guint8)(((kvm) << 7) | ((sol) << 6) | ((aps) << 5) | ((car) << 4) \
    | ((fwt) << 3) | ((mac) << 2) | ((ver) << 1))

#define BMC_CONST_BCD_FORMAT(x, y) (((x)&0x0f) | (((y)&0x0f) << 4))

#define POWERCAPPING_CLASS_ID 14
#define PC_ENABLE_PROPERTY_ID 1
#define PC_LIMITVALUE_PROPERTY_ID 2
#define PC_FAILACTION_PROPERTY_ID 3

#define RAIDCONTROLLER_CLASS_ID 78
#define USER_IP_OFFSET 2
#define UUID_PER_MODE_POWER_OFF 1
#define UUID_PER_MODE_RESET     0

typedef struct {
    const gchar *prop_name;
    guint8 supported_bit;
} CUSTOM_COOLING_COM_BIT;

typedef struct {
    guint16 prop_id;
    guint8 supported_bit;
} CUSTOM_COOLING_COM_BIT_INT;

typedef struct {
    guint16 index;
    guint8 supported_bit;
} CUSTOM_ENV_COOLING_COM_BIT;

// 文件操作
#define WEB_CUSTOM_FILE ("/data/opt/pme/web/conf/web_custom.xml")
#define WEB_DOC_PIC ("/data/opt/pme/web/htdocs/theme/custom/img/code1.png")
#define WEB_STYLE_FILE "/data/opt/pme/web/custom/style_config.json"

#define ALL_CONFIG_IN_PROCESS 1
#define ALL_CONFIG_COMPELETE 0

#define SUPPORT_SCAN_ONLY_HDD 1
#define SUPPORT_SCAN_HDD_AND_SSD 2

#define BMC 0
#define BIOS 1

#define NO_NEED_POWER_ON_AFTER_BOOT 0
#define NEED_POWER_ON_AFTER_BOOT    1

#define BMC_STRING_LEN 128
#define COMPONENT_NAME_BMC "BMC"
#define COMPONENT_NAME_RAID "RAID"

#define VMD_ENABLED 1
#define VMD_DISABLED 0

#define CACHE_RAID_XML_FILE "/data/etc/raid.xml"
#define SESSION_TOKEN_LENGTH_UPDATE_FLAG_PATH "/data/var/first_update.flag"
// 因为格式化的时候是大写，正则里面去掉小写字母，只保留大写
#define UUID_FORMAT_REGEX "^[0-9A-F]{8}-[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{4}-[0-9A-F]{12}$"


typedef enum {
    HOTPLUG_SURPRISE_DISABLED = 0,
    HOTPLUG_SURPRISE_ENABLED = 1
} HOTPLUG_SURPRISE_TYPE;
#define HOTPLUG_SURPRISE_STATUS(x) ((x) == HOTPLUG_SURPRISE_ENABLED ? "enabled" : "disabled")
#define CACHE_RAID_XML_FILE "/data/etc/raid.xml"
#define HOTPLUG_SURPRISE_VALUE_OFFSET 6

#pragma pack(1) 


typedef struct {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
} IPMIMSG_OEM_SET_RESP_S;


typedef struct {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 data;                    
} IPMIMSG_OEM_GET_RESP_S;



typedef struct tag_BMC_DDR_ECC_ERR_GET_S {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint32 single_ecc_count;       
    guint32 mutil_ecc_count;        
} BMC_DDR_ECC_ERR_GET_S;
typedef struct {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 data;                    
} IPMIMSG_GET_UUID_SAVE_MODE_RESP_S;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint16 selector;               
} IPMIMSG_GET_UUID_SAVE_MODE_REQ_S;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint8 selector;                
    guint8 data;                    
} IPMIMSG_SET_UUID_SAVE_MODE_REQ_S;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint16 selector;                
} IPMIMSG_GET_DST_ENABLE_REQ_S;

typedef struct {
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 sub_cmd;                 
    guint16 selector;                
    guint16 data_length;             
    guint8 data;                    
} IPMIMSG_SET_DST_ENABLE_REQ_S;


#pragma pack()

// --BTD，直接引用驱动头文件--end
#define MAX_SLOT_ID 0x20 // 最大槽位号
#define BMC_CHASSIS_FRU_ID 0

enum {
    BMC_SUCCESS = 0,
    BMC_FAIL
};

enum {
    BMC_LAST = 0,
    BMC_MIDDLE
};

enum {
    REGE_DEV_GUID_INIT = 0xaa,
    REGE_DEV_GUID_GET_STATUS = 0x00
};

enum {
    REGE_GUID_IN_PROGRESS = 0,
    REGE_GUID_COMPLETED
};

typedef struct tag_PME_GENERATE_GUID {
    guint8 fruid;
    guint8 flag_G;
    guint8 flag_U;
    guint8 flag_I;
    guint8 flag_D;
    guint8 op;
} PME_GENERATE_GUID_S;

typedef struct tag_SYNC_RTC_BIOS {
    guint8 manufacture[3];
    guint8 sub_cmd;
    guint8 valid_flag;
    guint8 year_h;
    guint8 year_l;
    guint8 month;
    guint8 day;
    guint8 hour;
    guint8 minute;
    guint8 second;
} SYNC_RTC_BIOS_S;

typedef struct tag_PME_OPEN_BOX {
    guint8 manufacture[3];
    guint8 sub_cmd;
    guint8 device_id;
    guint8 event;
    guint8 device_status[6];
} PME_OPEN_BOX_S;

typedef struct tag_PME_SET_PMC_CONF {
    guint8 manufacture[3];
    guint8 sub_cmd;
    guint8 param_select;
    guint8 data_flag;
    guint8 offset;
    guint8 length;
    /*lint -e1501*/
    guint8 data[0];
} PME_SET_IPMC_CONF_S;

typedef struct tag_PME_GET_PMC_CONF {
    guint8 manufacture[3];
    guint8 sub_cmd;
    guint8 param_select;
    guint8 offset;
    guint8 length;
} PME_GET_IPMC_CONF_S;

typedef struct tag_GET_ADDR_INFO_MSG {
    guint8 picmg_id;
    guint8 fru_dev_id;
    guint8 addr_key_type;
    guint8 addr_key;
    guint8 site_type;
} GET_ADDR_INFO_MSG_S;

typedef struct tag_PME_LOCATION_REQ_MSG {
    guint8 manufacture[3];
    guint8 sub_cmd;
} PME_LOCATION_REQ_MSG_S;

typedef struct tag_PME_SET_LOCATION_REQ_MSG {
    PME_LOCATION_REQ_MSG_S msg_head;
    guint8 last_data;
    guint8 write_offset;
    guint8 write_len;
    guint8 write_data;
} PME_SET_LOCATION_REQ_MSG_S;

typedef struct tag_PME_GET_LOCATION_REQ_MSG {
    PME_LOCATION_REQ_MSG_S msg_head;
    guint8 read_offset;
    guint8 read_len;
} PME_GET_LOCATION_REQ_MSG_S;

typedef struct {
    guint8 manufacture[MANUFAC_ID_LEN];
    guint8 sub_cmd;
} BMC_RESET_CAUSE_REQ_MSG_S;

typedef struct {
    guint8 compcode;
    guint8 manufacture[MANUFAC_ID_LEN];
    guint8 reset_cause;
} BMC_RESET_CAUSE_RSP_MSG_S;

typedef struct tag_IPMIMSG_GET_DEVICE_ID_RESP {
    guint8 comp_code;
    guint8 device_id;
    guint8 device_revision;
    guint8 fw_revision1;
    guint8 fw_revision2;
    guint8 ipmi_version;
    guint8 additional_device;
    guint8 manufacturer_id0;
    guint8 manufacturer_id1;
    guint8 manufacturer_id2;
    guint8 product_id_l;
    guint8 product_id_h;
    guint8 board_id_l;
    guint8 board_id_h;
    guint8 reserved[2];
} IPMIMSG_GET_DEVICE_ID_RESP_S;

typedef union tag_ADDITION_DEV_SUUPORT {
    guint8 byte;
    struct {
#ifdef BD_BIG_ENDIAN
        guint8 ChassisDevice : 1;
        guint8 Bridge : 1;
        guint8 IPMBEventGenerator : 1;
        guint8 IPMBEventReceiver : 1;
        guint8 FRUInventoryDevice : 1;
        guint8 SELDevice : 1;
        guint8 SDRRepositoryDevice : 1;
        guint8 SensorDevice : 1;
#else
        guint8 SensorDevice : 1;
        guint8 SDRRepositoryDevice : 1;
        guint8 SELDevice : 1;
        guint8 FRUInventoryDevice : 1;
        guint8 IPMBEventReceiver : 1;
        guint8 IPMBEventGenerator : 1;
        guint8 Bridge : 1;
        guint8 ChassisDevice : 1;
#endif
    } _BIT;
} ADDITION_DEV_SUPPORT_S;

typedef struct tag_IPMIMSG_GET_DEVICE_GUID_RESP {
    guint8 comp_code;
    guint8 uuid[BMC_GUID_LEN + 1];
} IPMIMSG_GET_DEVICE_GUID_RESP_S;

typedef struct tag_BMCGL_ENABLE {
#ifdef BD_BIG_ENDIAN
    guint8 oem2_enable : 1;
    guint8 oem1_enable : 1;
    guint8 oem0_enable : 1;
    guint8 reserved : 1;
    guint8 enable_sysevt_logsel : 1;
    guint8 enable_evt_msgbuf : 1;
    guint8 enable_evt_msgful_ipt : 1;
    guint8 enable_revmsg_ipt : 1;
#else
    guint8 enable_revmsg_ipt : 1;
    guint8 enable_evt_msgful_ipt : 1;
    guint8 enable_evt_msgbuf : 1;
    guint8 enable_sysevt_logsel : 1;
    guint8 reserved : 1;
    guint8 oem0_enable : 1;
    guint8 oem1_enable : 1;
    guint8 oem2_enable : 1;
#endif
} BMCGL_ENABLE_S;

typedef union tag_IPMIMSG_SET_BMCGL_ENABLE {
    guint8 byte;
    BMCGL_ENABLE_S bmc_enable;
} IPMIMSG_SET_BMCGL_ENABLE_S;

typedef struct tag_IPMIMSG_GET_BMCGL_ENABLE_RESP {
    guint8 comp_code;
    BMCGL_ENABLE_S bmc_enable;
} IPMIMSG_GET_BMCGL_ENABLE_RESP_S;

// BMC 全局设置标志
typedef struct tag_BMC_GLOBAL_ENABLE {
    // oem2, oem1, oem0 主要用于外部软件的设置、查询、软件之间的信号量
    IPMIMSG_SET_BMCGL_ENABLE_S global_enable;

    guint8 support_evt_msgbuf : 1;
    guint8 support_revmsg_ipt : 1;
    guint8 reserved2 : 6;
} BMC_GLOBAL_ENABLE_S;

typedef struct tag_IPMIMSG_GET_H2P_VERIFY_INFO_RESP {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 verify_code;             
} IPMIMSG_GET_H2P_VERIFY_INFO_RESP_S;

typedef struct tag_IPMIMSG_GET_EFUSE_STATE_RESP {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 efuse_state;             
} IPMIMSG_GET_EFUSE_STATE_RESP_S;

typedef struct tag_IPMIMSG_GET_EFUSE_TYPE_RESP {
    guint8 comp_code;               
    guint8 manu_id[MANUFAC_ID_LEN]; 
    guint8 efuse_type;              
} IPMIMSG_GET_EFUSE_TYPE_RESP_S;

typedef struct tg_IPMIMSG_GET_CHASSIS_CAP_RESP_S {
    guint8 comp_code;
    guint8 chas_cap_flag;
    guint8 chas_fruinfo_addr;
    guint8 chas_sdr_addr;
    guint8 chas_sel_addr;
    guint8 chas_sysmgm_addr;
    guint8 bridge_device_addr;
} IPMIMSG_GET_CHASSIS_CAP_RESP_S;

typedef struct tg_IPMIMSG_GET_CHASSIS_STATE_RESP_S {
    guint8 comp_code;

#ifdef BD_BIG_ENDIAN
    guint8 restore_policy : 2;
    guint8 control_fault : 1;
    guint8 power_fault : 1;
    guint8 interlock : 1;
    guint8 overload : 1;
    guint8 power_state : 1;
#else
    guint8 power_state : 1;
    guint8 overload : 1;
    guint8 interlock : 1;
    guint8 power_fault : 1;
    guint8 control_fault : 1;
    guint8 restore_policy : 2;
#endif
    guint8 last_pwr_evt;
    guint8 misc_chas_state;
    guint8 ftpal_button_cap;
} IPMIMSG_GET_CHASSIS_STATE_RESP_S;

enum {
    CHASSIS_IDENTIFY_STATE_OFF = 0,
    CHASSIS_IDENTIFY_STATE_ON = 0x20,
    CHASSIS_IDENTIFY_STATE_TIMED = 0x10
};

typedef struct tag_BMC_AUXFW_INFO_RESP_MSG {
    guint8 comp_code;
    guint8 manufacture_id0;
    guint8 manufacture_id1;
    guint8 manufacture_id2;
    guint8 bmc_platform_ver;
    guint8 ecoft_enable;
} BMC_AUXFW_INFO_RESP_MSG_S;
typedef union _PRODUCT_UNIQUE_ID_ {
    struct {
#ifdef BIG_ENDIAN
        guint32 product_uid;
        guint32 vendor_id;
        
#else
        guint32 vendor_id;
        guint32 product_uid;
#endif
    } split;
    guint64 real_produc_uid;
} PRODUCT_UID;

gint32 set_lsw_chip_port(gint32 smm_id);
extern gint32 get_on_board_rtc_time_method(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 rollback_ssl_workkey(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 uninstall_language_resource(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_blade_manage_mode(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 bmc_set_uuid(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
extern gint32 update_bmc_build_time(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 bmc_global_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
extern gint32 bmc_global_add_object_complete_callback(guint32 position);
extern gint32 bmc_global_del_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
extern gint32 bmc_global_del_object_complete_callback(guint32 position);
extern gint32 get_time_src_from_cfg(guint8 *ptime_src);
gint32 method_set_product_disks_maxtemp(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_service_startpoint(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_service_lifespan(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_product_name(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_set_product_uid(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_set_product_version(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_set_new_pro_name(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 clear_tmp_file(gchar *reserved_hpm);
gint32 ipmi_get_hotplug_surprise(const void *req_msg, gpointer user_data);
gint32 ipmi_set_hotplug_surprise(const void *req_msg, gpointer user_data);
gint32 ipmi_set_service_lifespan(const void *req_msg, gpointer user_data);
gint32 ipmi_get_service_lifespan(const void *req_msg, gpointer user_data);
gint32 ipmi_get_first_power_on_time(const void *req_msg, gpointer user_data);
#ifdef DFT_ENABLED
gint32 ipmi_set_first_power_on_time(const void *req_msg, gpointer user_data);
#endif
extern gint32 get_mm_optical_module_info(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
extern gint32 process_as_status_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
extern gint32 property_sync_method(OBJ_HANDLE handler, GSList *caller, GSList *input, GSList **output);
extern gint32 sync_time_or_timezone_to_blade(guint8 set_param);
gint32 method_update_product_alias_by_coolingmedium(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 ipmi_command_get_oem_version_info(const void *req_msg, gpointer user_data);
gint32 bmc_get_h2p_verify_info(gconstpointer req_msg, gpointer user_data);
gint32 bmc_get_efuse_type(gconstpointer req_msg, gpointer user_data);
gint32 bmc_get_efuse_state(gconstpointer req_msg, gpointer user_data);
void refresh_cert_overdue_status(guint8 offset, guint8 id, gchar *not_before, gchar *not_after);
gint32 ipmi_set_custom_alarm(const void *msg_data, gpointer user_data);
gint32 ipmi_get_custom_alarm_status(const void *msg_data, gpointer user_data);
gint32 ipmi_set_custom_manufacturer(const void *req_msg, gpointer user_data);
gint32 ipmi_get_custom_manufacturer(const void *req_msg, gpointer user_data);
void bus_manage_init(void);
gint32 ipmi_get_custom_property(const void* req_msg, gpointer user_data);
gint32 ipmi_set_custom_property(const void* req_msg, gpointer user_data);
#ifdef ARM64_HI1711_ENABLED
extern gint32 debug_print_trng(GSList *input_list);
#define SYS_INFO_DEVNAME                "/dev/sys_info"
#define SYS_INFO_IOC_MAGIC              's'
#define SYS_INFO_GET_CHIP_INFO     _IOR(SYS_INFO_IOC_MAGIC, 26, int)
#define BMC_CHIP_VERSION_V110_NUM 0x110
#define CHIP_VERSION_LEN_MAX 16
typedef struct chip_version_info {
    guint32 chip_name;
    guint32 chip_version;
} CHIP_VERSION_INFO;
#endif
void detect_bmc_clear_pw_sig(void *param);
gint32 period_per_init(void);
void bmc_period_task(void);
void reboot_bmc_self(guint32 param);
gint32 set_guid_property_value(void);
BMC_GLOBAL_ENABLE_S *get_bmc_gl_enable_addr(void);
void set_bmc_guid_value_state(guint8 state);
IPMIMSG_GET_DEVICE_GUID_RESP_S get_bmc_guid_value(void);
gint32 refresh_string_expression_value(OBJ_HANDLE handle, gpointer user_data);
void task_string_expression_refresh(void *param);
gint32 init_bmc_per_info(void);
void uuid_mutex_init(void);
gint32 create_tmpdir_sem(void);
void bmc_ipmi_chan_detect(void);

#ifdef DFT_ENABLED
gint32 chassis_cover_dft_init(OBJ_HANDLE handle, gpointer data);
#endif
gint32 set_first_power_on_time(guint32 year, guint32 month, guint32 day);

gint32 method_set_configuration_model(OBJ_HANDLE product, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_set_cmcc_version(OBJ_HANDLE product, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_set_cucc_version(OBJ_HANDLE product, GSList* caller_info,
    GSList* input_list, GSList** output_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif



#endif

