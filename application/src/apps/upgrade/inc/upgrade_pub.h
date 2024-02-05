
#ifndef __UPGRADE_PUB_H__
#define __UPGRADE_PUB_H__

#include "error_code.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 


#define ROLLBACK_SIGN 65535 


#define PFN_INVALID_RET_VAL (-1)
#define FILE_OP_RET_EXCEP_VAL (-1)

#define UPGRADE_SCRIPT_PATH ("/opt/pme/pram/up_cfg")
#define UPGRADE_BEFORE_ACTION ("/opt/pme/pram/up_cfg/beforeaction.sh")
#define UPGRADE_AFTER_ACTION ("/opt/pme/pram/up_cfg/afteraction.sh")

#define PFN_PATH_PARTITION ("/")

#define UPGRADE_PROFILE_CIPHER "/data/opt/pme/upgrade/pme_profile_en"
#define UPGRADE_PROFILE_ORIGIN "/opt/pme/upgrade/pme_profile_en"
#define UPGRADE_PROFILE_CIPHER_BAK "/data/opt/pme/upgrade/pme_profile_en.bak"


#define UPGRADE_DEFAULT_PATH ("/dev/shm/upgrade")
#define UPGRADE_FIXED_FILE_INFO (".hpm")
#define UPGRADE_CONF_FILE ("/dev/shm/upgrade/config.tar.gz")
#define UPGRADE_DEFAULT_PATH_FILE ("/dev/shm/upgrade/*")
#define UPGRADE_CFGFILEPATH "/dev/shm/upgrade/CfgFileList.conf"
#define UPGRADE_AFTER_FILEPATH "/dev/shm/upgrade/afteraction.sh"
#define UPGRADE_BEFORE_FILEPATH "/dev/shm/upgrade/beforeaction.sh"
#define UPGRADE_FIRSTBOOT_FILEPATH "/dev/shm/upgrade/firstboot.sh"
#define UPGRADE_UPDATE_FILEPATH "/dev/shm/upgrade/update.cfg"


#define VRD_FW_UPGRADE_TMP_PATH "/tmp/vrd_resume.hpm"               
#define VRD_FW_DEFAULT_DIR      "/opt/pme/conf/vrd_mgnt"            
#define VRD_FW_TMP_SAVE_DIR_TIANCHI     "/data/upgrade/up_cfg/vrd_fw"       
#define VRD_FW_TMP_SAVE_DIR     "/opt/pme/pram/up_cfg/vrd_fw"
#define VRD_FW_UPGRADE_DIR      "/dev/shm/upgrade/vrd_fw"           
#define VRD_FW_BACKUP_DIR       "/data/backup/vrd"                 

#define UPGRADE_CACHE_BIOS_HPM_FILE CACHE_BIOS_HPM_FILE
#define UPGRADE_RESUME_TMP_CPLD_FILE ("/tmp/cpld_resume.hpm") 
#define UPGRADE_RESUME_TMP_PUB_CPLD_FILE ("/tmp/pcr_cpld.hpm")    

#define PUB_CPLD_RESUEM_CACHE_FILE  "/data/cpld/pub_cpld.hpm"
#define UP_OP_LOG_START_CONFLICT "Refuse upgrade when bios is upgrading\r\n"

#define DOWNLOAD_BIOS_LOG_FAILED "Download BIOS failed\r\n"
#define UPGRADE_CLPD_VALID_FILE_DIR "/opt/pme/pram/up_cfg"
#define KB(x) ((x) / 1024)
#define PERCENTAGE(x, total) (((x)*100) / (total))

#define JUDGE_KEY_SEARCH_VALID(x, y) ((0 == (x)) || (0 == (y)))

#define BUFFSIZE (4 * 1024)

#define MAX_COMMAND_LEN 256

#define PFN_NOT_INIT 0
#define PFN_INITED 1
#define TEMP_BUFF_SIZE 128
#define UPDATE_CPLD_FLAG1 (1) // CPLD需要生效
#define UPDATE_CPLD_FLAG0 (0) // CPLD不需要生效


#define PFN_POWER_LOOKUP_TIMES 600
#define PFN_FORCE_POWER_OFF 0
#define PFN_POWER_ON 1
#define PFN_SPECIAL_POWER_OFF 2

#define PFN_AUTO_PERM_DELAY_TIME 5000 // unit:ms

#define UPGRADE_LOOK_DELAY_TIME 3000 // unit:ms
#define UPGRADE_VRD_VALID_DELAY_TIME 100 // unit:ms

// 加解密相关
#define UP_DECRYPT_HEADER_SIZE 256
#define UP_DECRYPT_DATA_SIZE_MAX 32
#define UP_DECRYPT_DATA_SIZE_MIN 17
#define UP_DECRYPT_KEY_SIZE 16
#define DECRYPT_BUFSIZE (10 * 1024 + UP_DECRYPT_DATA_SIZE_MAX) 

#define PREUPGRADE_FLAG 0
#define UPGRADED_FLAG 1
#define PREROLLBACK_FLAG 2

#define BUFSIZE (10 * 1024) 


#define BUFSIZE_64K (64 * 1024) 


#define FLASH_COPY_SIZE (4 * 1024)


#define OPER_BUTTON_LOCK 1
#define NOT_OPER_BUTTON_LOCK 0

#define HPM_FILE 0
#define COMPONENT_FILE 1

#define RH8100_SINGLE_SYS 1

#define RH8100_MASTER_BMC 1
#define RH8100_SLAVE_BMC 0

#define PRODUCT_NAME_RH8100 "RH8100"
#define PRODUCT_NAME_9032 "9032"
#define PRODUCT_NAME_8100 "8100"
#define PRODUCT_NAME_9008 "9008"


#define WILDCARD_COMPONENT_BOARD_ID 65535
#define INVALIDE_COMPONENT_BOARD_ID 0
#define INVALIDE_COMPONENT_IDEX 0
#define WILDCARD_COMPONENT_IDEX 0xfffffffe



#define UP_CPLD_TASK_NAME ("CpldUp")


#define MAX_RESUME_FILE_LEN 255

#define MAX_DEVICE_ID_CFG_CNT 32 // 允许在update.cfg中配置DeviceIDList的ID最大个数

#define MAX_DEVICE_ID_CFG_CNT 32 // 允许在update.cfg中配置DeviceIDList的ID最大个数


#define DELAY_MAX_TIMES 20

#define LOG_FLUSH_DELAY_MS 3000

#define MAX_COMPONENT_NUM 31

// 组件类型
#define M3_UBOOT_COMPONENT_TYPE 0
#define ROOTFS_HEAD_COMPONENT_TYPE 1
#define ROOTFS_IMG_COMPONENT_TYPE 2
#define CMS_UBOOT_COMPONENT_TYPE 4

// 组件签名类型
#define HUAWEI_MODE 0

#define TEMP_GPP_REGION_LEN 0x18000000 // gpp大小为384M

// 待升级区各数据写入偏移
#define ROOTFS_HEADER_START_OFFSET (TEMP_GPP_REGION_LEN - 0x400000)
#define M3_UBOOT_HEADER_START_OFFSET (TEMP_GPP_REGION_LEN - 0x300000)
#define TOP_IMGE_HEADER_START_OFFSET (TEMP_GPP_REGION_LEN - 512)
#define UBOOT_HEADER_START_OFFSET (TEMP_GPP_REGION_LEN - 0x240000)
#define FIRMWARE_TYPE_M3_L0 0
#define FIRMWARE_TYPE_M3_L1 1
#define FIRMWARE_TYPE_BMC_BOOTLOADER 2
#define FIRMWARE_TYPE_BMC_ROOTFS_HEADER 3

#define INVAILD_SECTION_TYPE 0
#define SECTION_TYPE_FW 4
#define SECTION_TYPE_ROOT_CERT 16

#define ONCE_WRITE_LEN 1024
#define MAX_ROOTFS_HEADER_LEN 0x100000      // 1M
#define MAX_M3_UBOOT_COMPONENT_LEN 0x200000 // 2M

#define L0_FW_UPGRADE_FLAG 0x01
#define L1_FW_UPGRADE_FLAG 0x02
#define UBOOT_FW_UPGRADE_FLAG 0x04
#define ROOTFS_FW_UPGRADE_FLAG 0x08
#define ALL_FW_UPGRADE_FLAG 0x0F

#define ALIGNMENT_SIZE 2048
#define MAX_FLASH_WR_COUNT 3

#define CMS_MAX_SECTION_LENGTH 8196
#define MAX_SECTION_NUMBER 256

#define SUBIMAGEHEADER_OFFSET (1024 * 768)
#define MMC_BLK_BOOT0_FILE "/dev/mmcblk0boot0"

#define COMPONENT_VENDOR_LENGTH 8
#define COMPONENT_TYPE_LENGTH 2
#define COMPONENT_NAME_LENGTH 16

#define UBOOT_SIGN_TYPE_SOC 0
#define UBOOT_SIGN_TYPE_CMS 1

#define HASH_SIZE 32
#define HASH_STR_BUFF_SIZE (HASH_SIZE * 3)

#define IS_CLEARLOG_WBD 1
#define NOT_CLEARLOG_WBD 0


typedef enum {
    
    POWER_ON_FIRMWARE_ACTIVE_INDEX = 0,       // BMC主动下电场景，升级CPLD、VRD需要AC生效
    POWER_ON_STRATEGY_INDEX = 31        // 上电开机策略index，该bit为0代表执行通电开机策略，1代表不执行通电开机策略
} UNIT_POWER_ON_SCENE_IDX_T;


typedef struct {
    const gchar *component_type;
    const gchar *component_type_name;
} COMPTYPE_MAP;

typedef struct {
    guint32 section_type;
    guint32 section_offset;
    guint32 section_length;
} SECTION_S;

typedef struct {
    guint32 preamble;       // 0x55aa55aa
    guint32 file_len;       // = Customer CRL Section Offset + Section Length;
    guint16 firmware_type;  // always be 3
    guint16 section_number; // always be 7
    guint8 reserved[64];
    SECTION_S section[7];
    guint32 head_magic; // 0x33cc33cc
} SUBIMAGEHEADER_S;

typedef struct {
    guint32 preamble;       // 0x55aa55aa
    guint32 file_len;       // = Customer CRL Section Offset + Section Length;
    guint16 firmware_type;  // always be 3
    guint16 section_number; // always be 7
    guint8 ver_active[4];
    guint8 build_time_active[6];
    guint16 ext_section_num;
    guint32 sign_type;
    guint8 usr_data[48];
    SECTION_S section[7];
    guint32 head_magic; // 0x33cc33cc
} UBOOT_SUBIMAGEHEADER_S;

typedef struct {
    guint32 component_type;
    guint32 component_offset;
    guint32 component_length;
    guint32 reserved;
} COMPONET_S;

typedef struct { // 512byte
    guint8 component_num;
    guint8 reserved[15];
    COMPONET_S component[31];
} TOPIMAGEHEADER_S;

typedef void (*fp_set_component_upgrade_value)(gint32 value);
typedef gint32 (*fp_get_component_upgrade_value)(void);

typedef void (*fp_set_component_upgrade_ctrl)(guint8 value);
typedef guint8 (*fp_get_component_upgrade_ctrl)(void);


typedef void *(*fp_get_component_upgrade_parameter)(void);


typedef gint32 (*fp_action_upgrade_data)(gint32 value);

typedef void (*fp_set_boot_flag_value)(gint32 uboot_flag, gint32 value);

typedef gint32 (*fp_upgrade_cpld)(gsize length, gpointer in_data);


typedef gboolean (*fp_get_flag)(void);

typedef void (*fp_set_flag)(gboolean);


typedef void (*fp_set_firmware_package_ver)(const gchar *ver);

typedef void (*VRD_UPGRADE_FUNC)(gint8 *flag, guint32 component_idEx, gboolean is_vrd_valid_by_ac);
typedef void (*VRD_UPGRADE_CONF_FUNC)(gint8 *flag);
typedef void (*VRD_UPGRADE_VER_FUNC)(void);
typedef gint32 (*VRD_UPDATE_VER_FUNC)(OBJ_HANDLE vrd_handle);

typedef gint32 (*VRD_VERSION_CHECK_FUNC)(OBJ_HANDLE handle, gint8 *flag, guint32 type);


typedef void (*fp_init_comp_upgrade_detail_result_code)(void);
typedef void (*fp_set_comp_upgrade_detail_result_code)(guint8 result_id, guint8 slot_id, gint32 upgrade_result);
typedef void (*fp_get_comp_upgrade_detail_result_code)(guint8 result_id, guint8 *slot_id, gint32 *upgrade_result);
typedef void (*fp_set_comp_upgrade_detail_result_num)(guint8 result_code_num);
typedef guint8 (*fp_get_comp_upgrade_detail_result_num)(void);


typedef void (*fp_get_chip_infos)(guint8 *chip_nums, guint8 *chip_supply_mode, guint8 *chip_file_nums);
typedef void (*fp_set_chip_file_nums)(guint8 chip_file_nums);

enum {
    BOOT_ORDER_MTD3 = 0,
    BOOT_ORDER_MTD4,
    BOOT_AREA_MAX_CNT
};

enum {
    UPGRADE_IPMC_BOOTFLAG_IV = 0,
    UPGRADE_IPMC_BOOTFLAG_V
};

enum {
    UPGRADE_PROGRESS_INIT = 0,
    UPGRADE_PROGRESS_BEGIN = 10,
    UPGRADE_PROGRESS_FIRST = 30,
    UPGRADE_PROGRESS_MIDDLE = 50,
    UPGRADE_PROGRESS_SECOND = 60,
    UPGRADE_PROGRESS_THIRD = 80,
    UPGRADE_PROGRESS_UPER_LIMIT = 99,
    UPGRADE_PROGRESS_END = 100
};

enum {
    UPGRADE_IPMC_REBOOT_NO = 0,
    UPGRADE_IPMC_REBOOT_YES
};

enum {
    UPGRADE_ACTION_NOP = 0x00,
    UPGRADE_ACTION_FORCE,
    UPGRADE_ACTION_RESET,
    UPGRADE_ACTION_BUTT
};

enum {
    UPGRADE_BUS_BRIDGE = 0,
    UPGRADE_BUS_PME
};

enum {
    // 写生效文件复位
    VALID_CLPD_BY_REBOOT = 0,
    // 写生效文件，AC
    VALID_CLPD_BY_ACLOST,
    // 不写生效文件，AC
    VALID_CLPD_BY_ACLOST_2,
    // 直接AC，如果失败的话，写生效文件，再次AC
    VALID_CLPD_BY_ACLOST_3
};



enum {
    HPM_UP_OEM = 0,
    HPM_UP_IPMI,
    HPM_UP_SYNC, // 主备同步时触发的备区同步升级任务
    HPM_UP_DEFAULT
};

typedef enum {
    PME_UP_TYPE_CFG = 0,
    PME_UP_TYPE_DATA,
    PME_UP_TYPE_IPMC,
    PME_UP_TYPE_BUTT
} PME_UP_TYPE_E;

typedef struct tag_UP_FILE_NAME {
    gchar meth_up_file_name[MAX_FILEPATH_LENGTH];
    gchar meth_inter_name[MAX_FILEPATH_LENGTH];
    gchar meth_usr_name[LDAP_USER_NAME_MAX_LEN + LDAP_USERDOMAIN_MAX_LEN + 2];
    gchar meth_cli_name[MAX_FILEPATH_LENGTH];
    gchar ipmi_up_file_name[MAX_FILEPATH_LENGTH];
    gchar ipmi_req_msg[MAX_POSSIBLE_IPMI_FRAME_LEN];
} UP_FILE_NAME_S;

typedef struct tag_UPGRADE_FILE_INFO {
    const gchar *file_name;
    glong file_offset;
    glong file_len;
    
    glong bios_base_addr;
    glong bios_end_addr;
    
    guint8 preserve_mac;
} UPGRADE_FILE_INFO_S;


// 定义存放升级任务参数的结构
typedef struct tag_UPGRADE_PARAM {
    gint32 up_src;                                  
    gint32 fruid;                                   
    guint32 offset;                                 
    guint32 length;                                 
    guint32 component_id;                           
    gchar upgrade_file_name[FILE_NAME_BUFFER_SIZE]; 
    guint8 type; 

    
    guint32 component_idEx;
    guint32 device_id;     
    guint32 sub_device_id; 
    
    
    guint16 *device_id_list; 
    guint8 device_id_cnt;    
    
    
    guint16 component_board_id_list[MAX_DEVICE_ID_CFG_CNT]; 
    guint8 component_board_id_cnt;
    
    gchar uid[MAX_UID_LENGTH * 10]; 
    guint8 current_file_index;  
    guint8 firmware_cnt;        
    guint32 revision;           
} UPGRADE_PARAM_S;

typedef struct tag_UPGRADE_CALLBACK_FUNC {
    fp_set_component_upgrade_value set_comp_status;
    fp_get_component_upgrade_value get_comp_status;

    fp_set_component_upgrade_value set_comp_percent;
    fp_get_component_upgrade_value get_comp_percent;

    fp_set_component_upgrade_value set_comp_mode;
    fp_get_component_upgrade_value get_comp_mode;

    fp_set_component_upgrade_value set_comp_errcode;
    fp_get_component_upgrade_value get_comp_errcode;

    fp_set_component_upgrade_value set_union_mcu_upgrade_final_flag;
    fp_get_component_upgrade_value get_union_mcu_upgrade_final_flag;

    fp_set_component_upgrade_value set_comp_boot_order;
    fp_get_component_upgrade_value get_comp_boot_order;

    fp_set_component_upgrade_value set_host_status;
    fp_get_component_upgrade_value get_host_status;

    fp_set_boot_flag_value set_comp_boot_flag;
    fp_action_upgrade_data get_comp_boot_flag;

    fp_set_component_upgrade_value set_comp_never_boot;
    fp_get_component_upgrade_value get_comp_never_boot;

    fp_set_component_upgrade_value set_comp_res_flag;
    fp_get_component_upgrade_value get_comp_res_flag;

    fp_set_component_upgrade_ctrl set_comp_auto_flag;
    fp_get_component_upgrade_ctrl get_comp_auto_flag;

    fp_set_component_upgrade_ctrl set_comp_action_finish;
    fp_get_component_upgrade_ctrl get_comp_action_finish;

    fp_get_component_upgrade_value finish_upgrade;

    fp_action_upgrade_data set_image_ver;

    fp_set_component_upgrade_ctrl set_bios_upgrade_status;

    fp_get_component_upgrade_ctrl get_upgrade_mode_parameter;

    fp_set_component_upgrade_ctrl set_bios_owner;
    fp_get_component_upgrade_ctrl get_bios_owner;

    fp_set_component_upgrade_ctrl set_pme_cpld_flag;
    fp_get_component_upgrade_ctrl get_pme_cpld_flag;

    fp_set_component_upgrade_ctrl set_back_cpld_flag;
    fp_get_component_upgrade_ctrl get_back_cpld_flag;

    fp_upgrade_cpld set_cpld_upgrade;
    fp_upgrade_cpld set_cpld_validate;

    fp_get_component_upgrade_ctrl get_chassis_power_status;

    fp_get_component_upgrade_ctrl get_dft_mode;
    fp_action_upgrade_data set_power_status;

    fp_set_component_upgrade_ctrl set_cpld_valid_flag;
    fp_get_component_upgrade_ctrl get_valid_cpld_way;
    fp_get_component_upgrade_parameter get_upgrade_parameter;

    fp_get_flag get_bmc_reboot_flag_after_upgrade;
    fp_set_flag set_bmc_reboot_flag_after_upgrade;
    
    fp_get_component_upgrade_ctrl get_bios_need_poweroff_flag;
    fp_get_component_upgrade_value get_bios_need_poweroff_time;
    

    fp_get_flag get_monitor_power_off_flag;
    fp_set_flag set_monitor_power_off_flag;

    fp_get_flag get_cache_upgrade_file_flag;
    fp_set_flag set_cache_upgrade_file_flag;

    
    fp_get_flag get_filter_up_bios_flag;
    fp_set_flag set_filter_up_bios_flag;
    

    
    fp_set_firmware_package_ver set_firmware_package_ver;
    

    
    fp_get_flag get_cache_upgrade_file_result;
    fp_set_flag set_cache_upgrade_file_result;
    

    
    fp_set_firmware_package_ver bakup_cur_component_ver;
    

    
    fp_init_comp_upgrade_detail_result_code init_upgrade_detail_result_code;
    fp_set_comp_upgrade_detail_result_code set_upgrade_detail_result_code;
    fp_get_comp_upgrade_detail_result_code get_upgrade_detail_result_code;
    fp_set_comp_upgrade_detail_result_num set_upgrade_detail_result_num;
    fp_get_component_upgrade_ctrl get_upgrade_detail_result_num;
    
    fp_get_chip_infos get_upgrade_cpld_chip_infos;
    fp_set_chip_file_nums set_upgrade_cpld_file_nums;

    fp_set_component_upgrade_ctrl set_wbd_clearlog_flag;
} UPGRADE_CALLBACK_FUNC_S;

typedef struct tag_UPGRADE_TYPE_INFO {
    guint8 type;         // 升级文件的类型 : HPM_FILE表示hpm包，COMPONENT_FILE表示组件文件
    gint32 component_id; // 如果type是hpm包则不涉及，如果是组建文件则表示该组建的ID
    gchar str_image_path[MAX_FILEPATH_LENGTH];
} UPGRADE_TYPE_INFO;

typedef gint32 (*fp_start_component_upgrade)(UPGRADE_PARAM_S *upgrade_param);
typedef gint32 (*fp_upgrade_init)(UPGRADE_CALLBACK_FUNC_S *fp_callback_func);
typedef gint32 (*fp_upgrade_spi_extra)(OBJ_HANDLE obj_handle);


typedef struct tag_DOWNLOAD_CALLBACK_FUNC {
    fp_set_component_upgrade_ctrl set_bios_owner;
    fp_get_component_upgrade_ctrl get_bios_owner;
    fp_set_component_upgrade_ctrl set_bios_upgrade_status;
} DOWNLOAD_CALLBACK_FUNC_S;


typedef struct fan_upgrade_channel_info {
    guint16 upgrade_device_array[32]; 
    const gchar *file_path;           
    guint8 device_count;              
    guint8 channel_id;                
} FAN_UPGRADE_CHANNEL_INFO_S;


gint32 upgrade_reboot_remote_bmc(void);



typedef gint32 (*fp_download_func)(DOWNLOAD_CALLBACK_FUNC_S *call_func, const gchar *filename, gint32 filesize);

gint32 download_bios(DOWNLOAD_CALLBACK_FUNC_S *call_func, const gchar *filename, gint32 filesize);


gint32 upgrade_init(UPGRADE_CALLBACK_FUNC_S *fp_callback_func);
gint32 start_component_upgrade(UPGRADE_PARAM_S *upgrade_param);

void upgrade_reboot_bmc(guint32 param);
guint32 check_is_need_aclost(guint32 component_id);
gboolean add_execute_privilege(const gchar *file_path);
gint32 extract_tar_file(const gchar *infilename, const gint32 offset, const guint32 length, const gchar *newfilename);
gint32 extract_file_to_buffer(gchar *infilename, const gint32 offset, const guint32 length, guchar *buffer,
    guint32 buffer_len);
void compose_integrate_file_name(gchar *path_name, gchar *file_name, gchar *out_path, gint32 path_len);
gboolean check_dir(gchar *dirpath);
void error_process(UPGRADE_CALLBACK_FUNC_S *call_back, gint32 error, const gchar *rm_file_name,
    const gchar *caller_name, guint32 caller_line);
void set_action_on_finish(UPGRADE_CALLBACK_FUNC_S *call_back, guint8 action);
void auto_perform_upgrade_action(UPGRADE_CALLBACK_FUNC_S *call_back, guint8 flag);
void pub_action_before_load(void);
void pub_action_after_load(guint32 component_id);
gint32 file_copy_by_cfg(gchar *cfgfilepath, gchar *sourcepath);

guint8 get_me_upgrade_flag(void);
void set_me_upgrade_flag(guint8 flag);
gint32 upgrade_reboot_handle(REBOOT_CTRL rc);

gint32 mtd_flash_copy_ex(UPGRADE_CALLBACK_FUNC_S *call_back, const gchar *devicename, UPGRADE_PARAM_S *file_info,
    PME_UP_TYPE_E up_type);
gint32 get_current_boot_order(UPGRADE_CALLBACK_FUNC_S *call_back);
const gchar *get_up_firmware_name(guint32 comp_id);
gint32 check_hpm_file_name_valid(const gchar *file_name, gint32 file_len, gchar *name_buffer);
void pfn_save_last_boot_order(guint8 flag);
gint32 emmc_flash_copy_ex(UPGRADE_CALLBACK_FUNC_S *call_back, const gchar *devicename, UPGRADE_PARAM_S *file_info,
    PME_UP_TYPE_E up_type);
gint32 get_partner_mode_enable(guint32 *partner_mode_enable);

gint32 get_node_info(guint8 *partition, guint8 *node_mode);
gint32 get_product_name(gchar *product_name, guint32 product_name_len);
gint32 get_remote_upgrade_status(guint8 *state, gint32 *error_code);
gint32 get_decrypt_aes_key(gint32 file_handle, glong offset_pos, gchar *key, gsize key_size, gsize *key_len);
gint32 get_decrypt_data_from_hpm(guint8 *out_buf, gsize out_buf_size, gint32 *out_len, const guint8 *in_buf,
    gint32 in_len, gchar *key, gsize key_len);

gint32 decrypt_src_file_to_dest_file(const gchar *src_file, const gchar *dest_file);
void collect_ipmi_log_info(gchar *msg_buf, const void *request_msg, guint32 msg_buf_len);
void pfn_set_rollback_flag(guint8 flag_type);
gint32 upgrade_module_add_object_callback(const gchar *class_name, OBJ_HANDLE object_handle);
gint32 update_firmware_version_info(OBJ_HANDLE object_handle, gpointer user_data);

void callback_func_init(UPGRADE_CALLBACK_FUNC_S *func);



gint32 get_bmc_rollback_flag(void);



guint8 pfn_get_cmc_dft_mode(void);


gint32 upgrade_get_remote_property_value(const gchar *object_name, GSList *input_list, GSList **output_list);
gint32 upgrade_call_remote_class_method(const gchar *object_name, const gchar *method_name, GSList *user_data_list,
    GSList *input_list, GSList **output_list);

gint32 upgrade_get_remote_bmc_ip(gchar *remote_ip, guint ip_len);
gint32 get_file_len(FILE *stream, guint32 *pFileLen);
void set_bmc_rollback_flag_r(gint32 value, gint32 line, const gchar *func);
#define set_bmc_rollback_flag(value) set_bmc_rollback_flag_r(value, __LINE__, __FUNCTION__)

gint32 deal_with_update_bp_extern_action(GVariant *array_obj_name_gvar);

gint32 remap_board_id(gint32 cfg_board_id, guint8 xml_id);
gint32 copy_resume_file(guint8 backup_mode, gchar *target_file_path, guint32 path_max_len);
gint32 get_cpld_backup_mode(guint8 *backup_mode);
gint32 get_cpld_resume_enable(guint8 *resume_enable);
gint32 set_forbid_allow_power_on(guint8 val, guint8 mask, guint8 mode);

gint32 get_component_type_name(const gchar *component_type, gchar *component_name, guint32 name_buff_size);
gint32 upgrade_set_asm_flag(guint32 val, guint32 delay_sec);
gint32 check_product_pangea_v6(guint8 *is_pangea);


#ifdef ARM64_HI1711_ENABLED
gint32 emmc_flash_write_temp_gpp(UPGRADE_CALLBACK_FUNC_S *call_back, UPGRADE_PARAM_S *file_info);
#ifdef DFT_ENABLED
gint32 emmc_flash_sync_tmp_to_gold_gpp(UPGRADE_CALLBACK_FUNC_S *call_back);
#endif
#endif

void bmc_achieve_ac_cycle(void);

gint32 do_bp_extern_action(OBJ_HANDLE handle, gint8 input_data);

gint32 get_decrypt_key(const gchar *data_key, gchar *key, gsize key_size, gsize *key_len);
gint32 is_cpld_valid_file_exist(void);
void modify_temp_file_permission(void);
gint32 get_comp_tar_filepath(gchar *filepath, size_t file_path_size, const gchar *comp_tar_file);
guint8 pfn_get_dft_mode(void);
guint8 pfn_bios_upgrade_mode(void);
gint32 read_plain_text(gchar *plaintext, gsize buff_size, gsize *text_len);
gint32 rollback_masterkey(void);
gint32 backup_masterkey(void);
void upgrade_firmware_extern_action(OBJ_HANDLE handle, gint8 input_data);
void pull_pfr_hard_reset_me(void);
gboolean get_sync_upgrade_enable(void);
gint32 update_vrd_expected_version(guint32 exp_ver);
gint32 generate_exp_ver_file(void);

gint32 cpld_space_test_func(void);
#ifndef ARM64_HI1711_ENABLED
void kill_apps(void);
void resume_apps(gboolean is_reset);
void get_more_avialable_memory(void);
void restore_memory(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
