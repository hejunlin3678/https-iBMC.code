#ifndef __KMC_H__
#define __KMC_H__

#include "wsecv2_cbb.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif/* __cpluscplus */
#endif /* __cpluscplus */

#define MAX_MK_LEN (WSEC_MK_PLAIN_LEN_MAX * 2 + 1)
#define IV_LEN 16
#define MAX_CT_LEN    512
#define MAX_PT_LEN    256
#define MAX_CF_LEN    (10 * 1024)
#define MAX_PF_LEN    (10 * 1024)
#define SSHKEY_FILE_MAXSISE (1024 * 1024)

#define MAX_PLAINTEXT_BUF_SIZE     (1024 * 1024)
#define MAX_CIPHERTEXT_BUF_SIZE    (MAX_PLAINTEXT_BUF_SIZE + 1024) // 密文长度一般比明文多几十字节,增加1k余量

#define KEYSTORE_SHORT_NAME         "datatocheck.dat"
#define KEYSTORE_BKP_SHORT_NAME     "datatocheck_bkp.dat"
#define KEYSTORE_DIR          "/data/etc/cbb"
#define KEYSTORE_BKP_DIR      "/data/etc/cbb/bkp"

#define KEYSTORE          KEYSTORE_DIR"/"KEYSTORE_SHORT_NAME
#define KEYSTORE_BKP      KEYSTORE_BKP_DIR"/"KEYSTORE_BKP_SHORT_NAME

/* 以下KSF路径用于降权限的进程 */
/* redfish */
#define KEYSTORE_REDFISH_SHORT_NAME   "datatocheck_redfish.dat"
#define KEYSTORE_REDFISH_DIR          "/data/etc/cbb_redfish"
#define KEYSTORE_REDFISH    KEYSTORE_REDFISH_DIR"/"KEYSTORE_REDFISH_SHORT_NAME
#define KEYSTORE_REDFISH_BKP    KEYSTORE_REDFISH_DIR"/datatocheck_redfish_bkp.dat"

/* kvm_vmm */
#define KEYSTORE_KVM_SHORT_NAME   "datatocheck_kvm.dat"
#define KEYSTORE_KVM_DIR          "/data/etc/cbb_kvm"
#define KEYSTORE_KVM    KEYSTORE_KVM_DIR"/"KEYSTORE_KVM_SHORT_NAME
#define KEYSTORE_KVM_BKP    KEYSTORE_KVM_DIR"/datatocheck_kvm_bkp.dat"

/* snmd */
#define KEYSTORE_SNMPD_SHORT_NAME   "datatocheck_snmpd.dat"
#define KEYSTORE_SNMPD_DIR          "/data/etc/cbb_snmpd"
#define KEYSTORE_SNMPD     KEYSTORE_SNMPD_DIR"/"KEYSTORE_SNMPD_SHORT_NAME
#define KEYSTORE_SNMPD_BKP    KEYSTORE_SNMPD_DIR"/datatocheck_snmpd_bkp.dat"

/* upgrade */
#define KEYSTORE_UPGRADE_SHORT_NAME   "datatocheck_upgrade.dat"
#define KEYSTORE_UPGRADE_DIR          "/data/etc/cbb_upgrade"
#define SAFE_ORIGIN_KEYSTORE_UPGRADE_DIR    "/data/opt/pme/upgrade"
#define KEYSTORE_UPGRADE     KEYSTORE_UPGRADE_DIR"/"KEYSTORE_UPGRADE_SHORT_NAME
#define KEYSTORE_UPGRADE_BKP    KEYSTORE_UPGRADE_DIR"/datatocheck_upgrade_bkp.dat"
#define ORIGIN_KEYSTORE_UPGRADE "/opt/pme/upgrade/datatocheck_upgrade.dat"
#define SAFE_ORIGIN_KEYSTORE_UPGRADE SAFE_ORIGIN_KEYSTORE_UPGRADE_DIR"/"KEYSTORE_UPGRADE_SHORT_NAME
#define KEYSTORE_UPGRADE_BAK KEYSTORE_UPGRADE".bak"
#define KEYSTORE_UPGRADE_BKP_BAK KEYSTORE_UPGRADE_BKP".bak"
#define UPGRADE_PROFILE_EN SAFE_ORIGIN_KEYSTORE_UPGRADE_DIR"/pme_profile_en"
#define UPGRADE_PROFILE_EN_BAK UPGRADE_PROFILE_EN".bak"

/* sshd */
#define KEYSTORE_SSHD_SHORT_NAME   "datatocheck_sshd.dat"
#define KEYSTORE_SSHD_DIR          "/data/etc/cbb_sshd"
#define KEYSTORE_SSHD      KEYSTORE_SSHD_DIR"/"KEYSTORE_SSHD_SHORT_NAME
#define KEYSTORE_SSHD_BKP    KEYSTORE_SSHD_DIR"/datatocheck_sshd_bkp.dat"

#define KEYSTORE_DEFAULT_USER_DIR "/opt/pme/conf/ssl/datatocheck_default_user.dat"
#define KEYSTORE_DEFAULT_USER_SHORT_NAME "datatocheck_default_user.dat"

#define KEYSTORE_USER_DIR   "/opt/pme/pram/User_datatocheck.dat"
#define KEYSTORE_BKP_USER_DIR   "/opt/pme/pram/User_datatocheck_bkp.dat"

#define MAX_KSF_ACTUAL_PATH_LEN   100

#define KMC_INITIAL_MASTERKEY_ID    0xFFFFFFFF
#define KMC_INVALID_MASTERKEY_ID    0

#define MASTERKEY_ID_IS_VALID(a)    ((a) != KMC_INITIAL_MASTERKEY_ID && (a) != KMC_INVALID_MASTERKEY_ID)

#define MAX_SET_PROP_TRY_COUNT    3

typedef enum {

    FILE_FUNC_SYSTEM,
    FILE_FUNC_FM
    
}FILE_FUNC_TYPE;

typedef enum {

    /* 0、1为默认域，从2开始 */
    DOMAIN_ID_MANUAL = 1,
    DOMAIN_ID_USER_PWD,
    DOMAIN_ID_SNMP_PRIVACY_PWD,
    DOMAIN_ID_SNMP_COMM,
    DOMAIN_ID_TRAP_COMM,
    DOMAIN_ID_SMTP_PWD,
    DOMAIN_ID_VNC_PWD,
    DOMAIN_ID_RD_EVT_SUBSCRIPTION,
    DOMAIN_ID_UPGRADE_FILE,
    DOMAIN_ID_SSHD_HOST_KEY,
    DOMAIN_ID_SSL,
    DOMAIN_ID_LDAP_BIND_PWD,
    DOMAIN_ID_NTP_GROUPKEY_FILE,
    DOMAIN_ID_BIOS,
    DOMAIN_ID_USB_UNCOMPRESS_PWD,
    DOMAIN_ID_SENSITIVE_FILE,
    DOMAIN_ID_BUTT
}DOMAIN_ID;

typedef enum {

    ENCRYPT_TYPE_KMC = 0,
    ENCRYPT_TYPE_REMOTE,
    ENCRYPT_TYPE_LOCAL,
    ENCRYPT_TYPE_INVALID = 255,

}ENCRYPT_TYPE;

typedef struct {
    guint32 domain_id;
    const gchar* obj_name;
    const gchar* obj_wk_prop;
    const gchar* obj_componet_prop;
    const gchar* obj_mk_id_prop;
    const gsize pt_max_len;
    const gsize ct_max_len;
    const gchar* remote_obj_name;
}ENCRYPT_ITEM_S;

extern pthread_mutex_t g_kmc_mutex;

void kmc_get_ksf_actual_path(KmcKsfName *ksf_name);
gint32 kmc_init(WsecUint32 role_type, KmcKsfName *ksf_name, FILE_FUNC_TYPE file_func_type);
void* kmc_refresh_mk_mask(void* arg);
gint32 kmc_reset(KmcKsfName *ksf_name);
gint32 kmc_fill_callback_func(WsecCallbacks *all_callbacks, FILE_FUNC_TYPE file_func_type);
gboolean kmc_check_load_status(void);
gboolean kmc_check_active_mk(guint32 domain_id);
gint32 kmc_sync_local_property(guint32 domain_id, OBJ_HANDLE local_obj_handle);
gint32 kmc_update_redfish_prop(OBJ_HANDLE obj_handle,
                               const gchar* priv_data_prop,
                               guint8 encrypt_type);
gint32 kmc_agent_init_for_modules(void);
void* kmc_agent_load_task(void* arg);
gint32 kmc_agent_load_for_opensource(KmcKsfName *ksf_name);

gint32 kmc_get_decrypted_data(guint32 domain_id, const gchar* ct, gchar* pt, guint32 pt_size, gsize* pt_len);
gint kmc_get_encrypted_data(guint32 domain_id, const gchar* pt, gchar* ct, gsize ct_size, gsize* ct_len, guint8 encrypt_type);
gint32 kmc_update_ciphertext(guint32 domain_id, const gchar* old_ct, gchar* new_ct, gsize new_ct_size, gsize* new_ct_len, guint8 encrypt_type);
gint32 kmc_get_new_ciphertext_property(guint32 domain_id, 
                               OBJ_HANDLE local_obj_handle, 
                               const gchar* local_ct_prop,
                               gchar* new_ct,
                               gsize new_ct_size,
                               gsize* new_ct_len,
                               guint8 encrypt_type);
gint32 kmc_get_decrypted_file(guint32 domain_id, const char *encryptpath, const char *plainfile);
gint32 kmc_get_encrypted_file(guint32 domain_id, const char *plainfile, const char *encryptpath);

gint32 kmc_update_ciphertext_str_prop(guint32 domain_id, 
                              OBJ_HANDLE local_obj_handle, 
                              const gchar* local_ct_prop,
                              guint8 encrypt_type);
gboolean kmc_check_if_masterkey_id_set(guint32 domain_id);
gint32 kmc_get_encrypt_data_if_mm_board(guint32 domain_id, const gchar *plaintext, gchar *ciphertext, gsize ciphertext_size, gsize *ciphertext_len);
gint32 kmc_get_encrypt_item(guint32 domain_id, ENCRYPT_ITEM_S *encrypt_item);
gint32 kmc_read_from_file(guint32 domain_id, const gchar *file_name, gchar *plaintext, gsize plaintext_size, gsize *plaintext_len);
gint32 kmc_write_to_file(guint32 domain_id, const gchar *file_name, const gchar *plaintext, gsize plaintext_len, guint8 encrypt_type);
gint32 kmc_update_ciphertext_file(guint32 domain_id, const gchar* file_name, guint8 encrypt_type);
gboolean is_encrypted_by_kmc(const guint8 *ciphertext);
gint32 redecrypt_pme_profile(gchar *plaintext, gsize plaintext_size, gsize *plaintext_len);
#ifdef __cplusplus
 #if __cplusplus
}
 #endif/* __cpluscplus */
#endif /* __cpluscplus */

#endif
