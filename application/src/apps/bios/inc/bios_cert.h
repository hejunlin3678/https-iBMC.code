

#ifndef BIOS_CERT_H
#define BIOS_CERT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

// 证书相关文件路径
#define SECUREBOOT_JSON_FILE_DIR        "/data/opt/pme/conf/bios/secureboot"
#define SECUREBOOT_NEW_JSON_FILE        "/data/opt/pme/conf/bios/secureboot/new_secureboot.json"
#define SECUREBOOT_CURRENT_JSON_FILE    "/data/opt/pme/conf/bios/secureboot/current_secureboot.json"
#define SECUREBOOT_NEW_SHORT_NAME       "new_secureboot.json"
#define SECUREBOOT_CURRENT_SHORT_NAME   "current_secureboot.json"

#define MAX_BOOT_CERT_TYPE_LEN   64
#define BOOT_CERT_TYPE_WHITELIST 0
#define BOOT_CERT_TYPE_BLACKLIST 1
#define BOOT_CERT_TYPE_CERT      2
#define BOOT_CERT_TYPE_CRL       3
#define BOOT_CERT_TYPE_UNKNOWN   255

#define BUF_SIZE_2K 2048
typedef struct boot_cert_type {
    guint8 attr_id;
    const gchar attr_name[MAX_BOOT_CERT_TYPE_LEN];
} BOOT_CERT_TYPE_S;

gint32 clear_boot_cert_file(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
void bios_secureboot_file_init(void);
guint8 get_cert_apply_mode(void);
gint32 method_bios_get_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_bios_get_https_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_bios_import_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_bios_reset_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_bios_get_secureboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 method_bios_set_secureboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);
gint32 on_hostmu_bootstage_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value);
void init_cert_update_mutex(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif 