

#ifndef __BIOS_H__
#define __BIOS_H__

#include "set_clp.h"
#include "get_clp.h"
#include "bdf.h"
#include "hw_access.h"
#include "file_op.h"
#include "debug.h"
#include "smbios.h"
#include "im_export_config.h"
#include "bios_settings.h"
#include "boot_options.h"
#include "bios_pwd.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define JUDGE_WRITE_PROTECT_VALID(x) ((((x)&0x0f) >= 2) || ((((x) >> 4) & 0x0f) > 0))

#define BIOS_BOOT_VALID_SEG_NUM 16

#define BOOT_OPTION_VALID 0
#define BOOT_OPTION_INVALID 1


#define BIOS_FLAG_IDLE 0x00
#define BIOS_FLAG_DOING 0x01

#define BOOT_OPTION_PARAMETER_VERSION 1

#define BIOS_LOAD_DEFAULT_CONF 0

#define MAX_INTERFACE_NUM 4


#define BIOS_CONFIG_PXE_TIMEOUT_RETRY       "PxeTimeoutRetryControl"

#define BIOS_CLASS_CONFIGVALUE_JSON_FILE_NAME     "/opt/pme/pram/BIOS_ConfigValueFileName"
#define BIOS_CONFIGVALUE_JSON_FILE_NAME     "/data/opt/pme/conf/bios/ConfigValue.json"
#define BIOS_CURRENTVALUE_JSON_FILE_NAME    "/data/opt/pme/conf/bios/currentvalue.json"
#define BIOS_REGISTRY_JSON_FILE_NAME        "/data/opt/pme/conf/bios/registry.json"
#define BIOS_CLASS_COMPONENT_JSON_FILE_NAME     "/opt/pme/pram/BIOS_ComponentVersionFileName"
#define BIOS_COMPONENT_JSON_FILE_NAME        "/data/opt/pme/conf/bios/ComponentVersion.json"
#define TMP_BIOS_SETTING_JSON_FILE_NAME     "/tmp/setting.json"

#define BIOS_COMPONENT_IPMI_LEN 10
#define BIOS_RSP_CONTENT_FRAME_LEN 200
#define BIOS_VER_LEN 16

#pragma pack(1)
typedef struct {
    guint8 comp_code;    
    guint8 manu_id[3];   
    guint8 last_frame;
    guint8 data_len;
    union {
        gchar file_data[BIOS_RSP_CONTENT_FRAME_LEN];
        struct {
            guint32 file_len;
            guint32 crc32;
            gchar bios_version[BIOS_VER_LEN];
        } info_data;
    } content;
} IPMI_BIOS_COMPONENT_VERSION_RESP;
#pragma pack()

enum INTERFACE_CONFIG_TYPE {
    TYPE_GE = 0,
    TYPE_10GE = 1,
    TYPE_40GE = 2,
    TYPE_FC = 3,
    TYPE_FCOE = 4,
    TYPE_RESERVED
};

#define CONSTRUCT_START_OPT_FORMAT(x) ((x) << 2)
#define RESTORE_BOOT_DEV_DEFAULT 0xc3

#define BIOS_DUMP_BOOT_HEAD ("\n------Bios Boot Options Begin------\n")
#define BIOS_DUMP_CHANGE_HEAD ("\n------Bios File Change Begin------\n")
#define BIOS_DUMP_DATA_HEAD ("\n------Bios Data Operation Begin------\n")
#define BIOS_DUMP_DATA_CONT ("%s:\r\ndata_flag:%u,data_offset:%u,data_len:%u\r\n")
#define BIOS_DUMP_CLP_CFG ("\n------CLP Config file Begin------\n")
#define BIOS_DUMP_SMBIOS_CFG ("\n------SmBios Config file Begin------\n")

#define BIOS_GET_BOOT_OPTION_LEN 3

#define DEFAULT_OSCA_PORT 0xff

enum {
    BIOS_OBJ_HANDLE_FUNC = 0,
    SMBIOS_OBJ_HANDLE_FUNC,
    OBJ_HANDLE_FUNC_BUTT
};

typedef struct tg_BIOS_BOOT_OPTION {
    guint8 set_in_progress_reserved : 6;
    guint8 set_in_progress : 2;
    guint8 service_partition_sel;
    guint8 service_partition_scan_reserved : 7;
    guint8 service_partition_scan : 1;
} BIOS_BOOT_OPTION_S;

typedef struct tag_UPDATE_BIOS_STATUS {
    guint8 manufacture[3];
    guint8 sub_cmd;
    guint8 status;
} UPDATE_BIOS_STATUS_S;

typedef struct tag_nic_set_system_info {
    guchar port_select;
    guchar port_type;
} NIC_SET_SYSTEM_INFO;



#define MAX_BIOS_DYNAMIC_CFG_ITEM 256
#define BIOS_JSON_DEMT "PowerSaving"
#define BIOS_DEMT_DESP "DEMT"

typedef enum {
    BIOS_SMI_DATA_EMPTY = 0,
    BIOS_SMI_DATA_READY,
    BIOS_SMI_DATA_FETCHED,
    BIOS_REPORT_CONF_RESULT
} BIOS_SMI_DATA_STATE;





#define INVALID_CHAN_NUM (-1)


gint32 ipmi_cmd_get_property_from_bios_file(gconstpointer req_msg, gpointer user_data);
gint32 ipmi_set_osca_port_type(const void *msg_data, gpointer user_data);
gint32 ipmi_get_osca_port_type(const void *msg_data, gpointer user_data);

gint32 compare_and_copy_clp_config_file_with_lock(gchar *full_file_name);
void resource_init(void);



void get_bios_conf_file_info(guint8 bios_type, gchar *file_path, guint32 file_path_len, gchar *file_name,
    guint32 file_name_len);


extern gint32 _check_current_file(const gchar *devicename);
gint32 read_boot_mode_from_setting_json(guint8 *boot_mode);
gint32 bios_set_boot_mode_prop_value(guint8 boot_mode);
extern guchar g_bios_config_format;
extern const gchar *g_file_property_name[BIOS_FILE_NAME_NUM];
extern BOOT_OPTIONS_S g_boot_options;
extern STRUCT_BIOS_CHANGE_INI_CONFIG g_boot_type_order_array[12];
extern BIOS_FILE_CHANGE_S g_bios_file_change[BIOS_FILE_MAX_SELECTOR];
extern guchar g_bios_setting_file_state;
extern BIOS_DATA_OPERATE_S g_smbios_wr_data_cfg;
extern guint32 g_manufacture_id_bios;
extern BIOS_DATA_OPERATE_S g_bios_rd_data_cfg;
extern BIOS_DATA_OPERATE_S g_bios_wr_data_cfg;
extern BIOS_DATA_OPERATE_S g_bios_imu_wr_data_cfg;
extern BIOS_DATA_OPERATE_S g_upg_fw_data_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
