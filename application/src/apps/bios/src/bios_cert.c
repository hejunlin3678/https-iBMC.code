
#include "openssl/pem.h"
#include "json/json.h"
#include "glib-2.0/glib.h"
#include "pme/file_manage/file_manage.h"
#include "pme_app/pme_app.h"
#include "bios_cert.h"

// 证书交互文件定义的键值(与BIOS存在协议约定，不可随意更改)
#define INTERACT_KEY_SECUREBOOT            "SecureBoot"
#define INTERACT_KEY_SECUREBOOT_DATABASES  "SecureBootDataBases"
#define INTERACT_KEY_SECUREBOOT_DB         "db"
#define INTERACT_KEY_SECUREBOOT_DBX        "dbx"
#define INTERACT_KEY_UEFI_SIGNATUREOWNER   "UefiSignatureOwner"
#define INTERACT_KEY_BOOT                  "Boot"
#define INTERACT_KEY_CERTIFICATE_STRING    "CertificateString"
#define INTERACT_KEY_CERTIFICATE_TYPE      "CertificateType"
#define INTERACT_KEY_CERTIFICATES          "Certificates"
#define INTERACT_KEY_OPERATION             "Operation"
#define INTERACT_KEY_SIZE                  "Size"
#define INTERACT_KEY_CRL                   "Crl"

// 证书用途
#define CERT_USAGE_SECUREBOOT   INTERACT_KEY_SECUREBOOT
#define CERT_USAGE_BOOT         "Boot"

// DWORD 长度
#define DWORD_LEN               (4)

// 安全启动数据库类型(UEFI标准)
#define SECUREBOOT_DATABASE_DB   INTERACT_KEY_SECUREBOOT_DB
#define SECUREBOOT_DATABASE_DBX  INTERACT_KEY_SECUREBOOT_DBX

// HTTPS BOOT数据库类型(自定义，仅用于BIOS方法)
#define HTTPSBOOT_DATABASE_CERT  "cert"
#define HTTPSBOOT_DATABASE_CRL   "crl"

// 重置类型
#define RESETKEYS_TYPE_DELETE_ALL       "DeleteAllKeys"
#define RESETKEYS_TYPE_RESET_TO_DEFAULT "ResetAllKeysToDefault"

// 证书操作类型
#define CERT_OPERATION_ADD    "Add"
#define CERT_OPERATION_DELETE RESETKEYS_TYPE_DELETE_ALL
#define CERT_OPERATION_RESET  RESETKEYS_TYPE_RESET_TO_DEFAULT

// UefiSignatureOwner格式: 16进制 8-4-4-4-12
#define UEFI_SIG_OWNER_PATTERN "^[a-f0-9]{8}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{4}-[a-f0-9]{12}$"

// CertificatesType类型(redfish标准定义的类型，PEM:单证书, PEMchain:证书链, PKCS7:p7形式单或多证书. 当前只支持PEM)
#define CERTIFICATE_TYPE_SINGLE_PEM "PEM"

// 自定义PEM类型, 仅用于内部建立映射关系
#define PEM_TYPE_SINGLE_PEM     CERTIFICATE_TYPE_SINGLE_PEM
#define PEM_TYPE_X509_CRL       "CRL"

// 证书操作条目数限制(非证书数目)
#define BIOS_MAX_CERT_NUM 9
#define BMC_MAX_CERT_NUM (2 * BIOS_MAX_CERT_NUM)

// 获取证书类方法的json响应体相关宏定义
// HTTPS BOOT证书类别
#define HTTPSBOOT_CERT          "HttpsCert"
#define HTTPSBOOT_CRL           "HttpsCrl"

// 证书详细信息
#define CERT_DETAILS_ISSUER                         "Issuer"
#define CERT_DETAILS_SUBJECT                        "Subject"
#define CERT_DETAILS_COUNTRY                        "Country"
#define CERT_DETAILS_STATE                          "State"
#define CERT_DETAILS_CITY                           "City"
#define CERT_DETAILS_ORGANIZATION                   "Organization"
#define CERT_DETAILS_ORGANIZATION_UNIT              "OrganizationUnit"
#define CERT_DETAILS_COMMON_NAME                    "CommonName"
#define CERT_DETAILS_EMAIL                          "Email"
#define CERT_DETAILS_VALID_NOT_BEFORE               "ValidNotBefore"
#define CERT_DETAILS_VALID_NOT_AFTER                "ValidNotAfter"
#define CERT_DETAILS_SERIAL_NUMBER                  "SerialNumber"
#define CERT_DETAILS_FINGER_PRINT                   "Fingerprint"
#define CERT_DETAILS_FINGER_PRINT_HASH_ALGO         "FingerprintHashAlgorithm"
#define CERT_DETAILS_SIGNATURE_ALGO                 "SignatureAlgorithm"


#define CERT_FINGERPRINT_HASH_ALGO_SHA1             "TPM_ALG_SHA1"
#define CERT_FINGERPRINT_HASH_ALGO_SHA256           "TPM_ALG_SHA256"

// 证书详细信息中的KeyUsage部分
#define CERT_DETAILS_KEY_USAGE                      "KeyUsage"
#define CERT_DETAILS_KEY_USAGE_CRL_SGINING          "CRLSigning"
#define CERT_DETAILS_KEY_USAGE_DATA_ENCIPHERMENT    "DataEncipherment"
#define CERT_DETAILS_KEY_USAGE_DECIPHER_ONLY        "DecipherOnly"
#define CERT_DETAILS_KEY_USAGE_DIGITAL_SIGNATURE    "DigitalSignature"
#define CERT_DETAILS_KEY_USAGE_ENCIPHER_ONLY        "EncipherOnly"
#define CERT_DETAILS_KEY_USAGE_KEY_AGREEMENT        "KeyAgreement"
#define CERT_DETAILS_KEY_USAGE_KEY_CERT_SIGN        "KeyCertSign"
#define CERT_DETAILS_KEY_USAGE_KEY_ENCIPHERMENT     "KeyEncipherment"
#define CERT_DETAILS_KEY_USAGE_NON_REPUDIATION      "NonRepudiation"

typedef struct tag_x509_key_usage {
    guint32 usage;
    const gchar *usage_str;
} X509KeyUsage;
// 支持的证书用途
LOCAL const gchar *g_supported_cert_usage[] = {
    CERT_USAGE_SECUREBOOT,
    CERT_USAGE_BOOT
};

// 支持的证书类型(当前只支持PEM)
LOCAL const gchar *g_supported_cert_type[] = {
    CERTIFICATE_TYPE_SINGLE_PEM
};

// 支持的数据库类型
LOCAL const gchar *g_supported_database[] = {
    SECUREBOOT_DATABASE_DB,
    SECUREBOOT_DATABASE_DBX,
    HTTPSBOOT_DATABASE_CERT,
    HTTPSBOOT_DATABASE_CRL
};

LOCAL const gchar *g_supported_pem_type[] = {
    PEM_TYPE_SINGLE_PEM,
    PEM_TYPE_X509_CRL
};

FM_REGISTER_S g_bios_secureboot_fm[] = {
    {SECUREBOOT_NEW_SHORT_NAME, SECUREBOOT_NEW_JSON_FILE, FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, 0, NULL, NULL},
    {SECUREBOOT_CURRENT_SHORT_NAME, SECUREBOOT_CURRENT_JSON_FILE, FM_PROTECT_POWER_OFF_E, 0, NULL, NULL, 0, NULL, NULL}
};

LOCAL const BOOT_CERT_TYPE_S g_boot_cert_type[] = {
    {BOOT_CERT_TYPE_WHITELIST, "db"},
    {BOOT_CERT_TYPE_BLACKLIST, "dbx"},
    {BOOT_CERT_TYPE_CERT,      "cert"},
    {BOOT_CERT_TYPE_CRL,       "crl"}
};

LOCAL int pem_single_verify(BIO *bio);
LOCAL int pem_crl_verify(BIO *bio);

// pem格式证书校验函数
typedef int (*PEM_VERIFY) (BIO *bio);

typedef struct verify_st {
    const char* type;
    PEM_VERIFY verify_func;
}VERIFY;

// 不同pem类型证书/CRL校验函数列表
LOCAL const VERIFY verify_map[] = {
    {PEM_TYPE_SINGLE_PEM, pem_single_verify},
    {PEM_TYPE_X509_CRL, pem_crl_verify}
};

LOCAL GMutex g_cert_update_mutex;

void init_cert_update_mutex(void)
{
    g_mutex_init(&g_cert_update_mutex);
}

gint32 clear_boot_cert_file(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    if (get_cert_apply_mode() != CERT_APPLY_MODE_MCU_OPCODE) {
        return RET_ERR;
    }
    if (property_name == NULL || property_value == NULL) {
        return RET_ERR;
    }
    gint32 ret;

    guint8 boot_flag = g_variant_get_byte(property_value);
    debug_log(DLOG_DEBUG, "Receive([%s]:%d) change event!", property_name, boot_flag);

    if (boot_flag == SMBIOS_WRITE_FINISH) {
        ret = fm_clear_file(SECUREBOOT_NEW_SHORT_NAME);
        if (ret != FM_OK) {
            debug_log(DLOG_ERROR, "%s: fm_clear_file %s failed", __FUNCTION__, SECUREBOOT_NEW_SHORT_NAME);
            return RET_ERR;
        }
        debug_log(DLOG_DEBUG, "[%s]check bios boot ready and clear boot cert file success.", __FUNCTION__);
    }
    return RET_OK;
}


void bios_secureboot_file_init(void)
{
    if (g_file_test(SECUREBOOT_JSON_FILE_DIR, G_FILE_TEST_IS_DIR) == FALSE) {
        (void)vos_mkdir_recursive(SECUREBOOT_JSON_FILE_DIR);
        (void)chown(SECUREBOOT_JSON_FILE_DIR, ROOT_USER_UID, APPS_USER_GID);
        (void)chmod(SECUREBOOT_JSON_FILE_DIR, S_IRWXU | S_IRGRP | S_IXGRP);
    }

    gint32 ret = fm_init(g_bios_secureboot_fm, G_N_ELEMENTS(g_bios_secureboot_fm));
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: File Manager init failed", __FUNCTION__);
    }
}

LOCAL gint32 check_bios_state(OBJ_HANDLE obj_handle)
{
    guint8 bios_state = 0;
    gint32 ret = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_STARTUP_STATE, &bios_state);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret: %d", __FUNCTION__, ret);
        return SECUREBOOT_METHOD_ERR;
    }

    if (bios_state == BIOS_STARTUP_STATE_OFF || bios_state == BIOS_STARTUP_POST_STAGE_FINISH) {
        
        return SECUREBOOT_METHOD_OK;
    }

    debug_log(DLOG_ERROR, "%s: bios_state: 0x%x", __FUNCTION__, bios_state);
    return SECUREBOOT_METHOD_BIOS_STATE_UNSUPPORT;
}


LOCAL void parse_cert_keyusage(X509 *cert, json_object *keyusage_output_jso)
{
    guint32 usage = X509_get_key_usage(cert);
    if (usage == 0) {
        debug_log(DLOG_ERROR, "%s: no key usage data", __FUNCTION__);
        return;
    }

    X509KeyUsage key[] = {
        {KU_DIGITAL_SIGNATURE, CERT_DETAILS_KEY_USAGE_DIGITAL_SIGNATURE},
        {KU_NON_REPUDIATION, CERT_DETAILS_KEY_USAGE_NON_REPUDIATION},
        {KU_KEY_ENCIPHERMENT, CERT_DETAILS_KEY_USAGE_KEY_ENCIPHERMENT},
        {KU_DATA_ENCIPHERMENT, CERT_DETAILS_KEY_USAGE_DATA_ENCIPHERMENT},
        {KU_KEY_AGREEMENT, CERT_DETAILS_KEY_USAGE_KEY_AGREEMENT},
        {KU_KEY_CERT_SIGN, CERT_DETAILS_KEY_USAGE_KEY_CERT_SIGN},
        {KU_CRL_SIGN, CERT_DETAILS_KEY_USAGE_CRL_SGINING},
        {KU_ENCIPHER_ONLY, CERT_DETAILS_KEY_USAGE_ENCIPHER_ONLY},
        {KU_DECIPHER_ONLY, CERT_DETAILS_KEY_USAGE_DECIPHER_ONLY}
    };

    for (guint32 i = 0; i < G_N_ELEMENTS(key); i++) {
        if (usage & key[i].usage) {
            json_object_array_add(keyusage_output_jso, json_object_new_string(key[i].usage_str));
        }
    }
}

LOCAL void parse_cert_identifier(const gchar *cert_id, const CERT_INFO_T *cert_info, json_object *output_jso)
{
    json_object *cert_id_jso = json_object_new_object();
    if (cert_id_jso == NULL) {
        debug_log(DLOG_ERROR, "%s:json_object_new_object failed, cert_id: %s", __FUNCTION__, cert_id);
        return;
    }

    json_object_object_add(output_jso, cert_id, cert_id_jso);
    json_object_object_add(cert_id_jso, CERT_DETAILS_COUNTRY, json_object_new_string(cert_info->country));
    json_object_object_add(cert_id_jso, CERT_DETAILS_STATE, json_object_new_string(cert_info->state));
    json_object_object_add(cert_id_jso, CERT_DETAILS_CITY, json_object_new_string(cert_info->location));
    json_object_object_add(cert_id_jso, CERT_DETAILS_ORGANIZATION, json_object_new_string(cert_info->org));
    json_object_object_add(cert_id_jso, CERT_DETAILS_ORGANIZATION_UNIT, json_object_new_string(cert_info->org_unit));
    json_object_object_add(cert_id_jso, CERT_DETAILS_COMMON_NAME, json_object_new_string(cert_info->common_name));
    json_object_object_add(cert_id_jso, CERT_DETAILS_EMAIL, json_object_new_string(cert_info->email));
}


LOCAL void parse_cert_details(const gchar *cert_str, json_object *output_jso)
{
    _cleanup_x509_ X509 *cert = NULL;
    CERT_INFO_T issuer = {0};
    CERT_INFO_T subject = {0};

    if (PEM_to_X509(cert_str, &cert, strlen(cert_str)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: PEM_to_X509 failed.", __FUNCTION__);
        return;
    }

    gint32 ret = dal_get_cert_info_from_x509(cert, &issuer, &subject);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_cert_info_from_x509 failed, ret: %d", __FUNCTION__, ret);
        return;
    }

    // Issuer
    parse_cert_identifier(CERT_DETAILS_ISSUER, &issuer, output_jso);
    // Subject
    parse_cert_identifier(CERT_DETAILS_SUBJECT, &subject, output_jso);

    json_object_object_add(output_jso, CERT_DETAILS_VALID_NOT_BEFORE, json_object_new_string(subject.not_before));
    json_object_object_add(output_jso, CERT_DETAILS_VALID_NOT_AFTER, json_object_new_string(subject.not_after));

    // KeyUsage
    json_object *keyusage_array_jso = json_object_new_array();
    if (keyusage_array_jso != NULL) {
        parse_cert_keyusage(cert, keyusage_array_jso);
        json_object_object_add(output_jso, CERT_DETAILS_KEY_USAGE, keyusage_array_jso);
    } else {
        debug_log(DLOG_ERROR, "%s:json_object_new_array failed", __FUNCTION__);
    }

    json_object_object_add(output_jso, CERT_DETAILS_SERIAL_NUMBER, json_object_new_string(subject.serial));
    json_object_object_add(output_jso, CERT_DETAILS_FINGER_PRINT, json_object_new_string(subject.fingerprint_sha256));
    json_object_object_add(output_jso, CERT_DETAILS_FINGER_PRINT_HASH_ALGO,
        json_object_new_string(CERT_FINGERPRINT_HASH_ALGO_SHA256)); // 指纹算法默认采用sha256
    json_object_object_add(output_jso, CERT_DETAILS_SIGNATURE_ALGO, json_object_new_string(subject.sig_algo));
}


LOCAL void parse_cert_details_simple(const gchar *cert_str, json_object *output_jso)
{
    _cleanup_x509_ X509 *cert = NULL;
    CERT_INFO_T issuer = {0};
    CERT_INFO_T subject = {0};

    if (PEM_to_X509(cert_str, &cert, strlen(cert_str)) != RET_OK) {
        debug_log(DLOG_ERROR, "%s: PEM_to_X509 failed.", __FUNCTION__);
        return;
    }

    gint32 ret = dal_get_cert_info_from_x509(cert, &issuer, &subject);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_cert_info_from_x509 failed, ret: %d", __FUNCTION__, ret);
        return;
    }

    // Issuer
    parse_cert_identifier(CERT_DETAILS_ISSUER, &issuer, output_jso);
    // Subject
    parse_cert_identifier(CERT_DETAILS_SUBJECT, &subject, output_jso);
    // ValidNotBefore
    json_object_object_add(output_jso, CERT_DETAILS_VALID_NOT_BEFORE, json_object_new_string(subject.not_before));
    // ValidNotAfter
    json_object_object_add(output_jso, CERT_DETAILS_VALID_NOT_AFTER, json_object_new_string(subject.not_after));
    // SerialNumber
    json_object_object_add(output_jso, CERT_DETAILS_SERIAL_NUMBER, json_object_new_string(subject.serial));
}


LOCAL void foreach_array_parse_cert_properties(const json_object *cert_array_jso, gboolean is_cert,
    json_object *output_array_jso)
{
    gsize certs_cnt = json_object_array_length(cert_array_jso);

    for (gsize i = 0; i < certs_cnt; i++) {
        json_object *cert_jso = json_object_array_get_idx(cert_array_jso, i);
        if (cert_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_array_get_idx %zu failed.", __FUNCTION__, i);
            continue;
        }

        const gchar *cert_str = NULL;
        if (!get_element_str(cert_jso, INTERACT_KEY_CERTIFICATE_STRING, &cert_str)) {
            debug_log(DLOG_ERROR, "%s:get_element_str CertificateString failed, index: %zu", __FUNCTION__, i);
            continue;
        }

        json_object *cert_output_jso = json_object_new_object();
        if (cert_output_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_object failed, index: %zu", __FUNCTION__, i);
            continue;
        }

        const gchar *cert_type = "";
        if (!get_element_str(cert_jso, INTERACT_KEY_CERTIFICATE_TYPE, &cert_type)) {
            debug_log(DLOG_ERROR, "%s:get_element_str CertificateType failed, index: %zu", __FUNCTION__, i);
        }

        if (is_cert && g_strcmp0(cert_type, "PEM") == 0) {
            
            parse_cert_details_simple(cert_str, cert_output_jso);
            json_object_array_add(output_array_jso, cert_output_jso);
            continue;
        }
        json_object_put(cert_output_jso);
    }
}


LOCAL void foreach_array_parse_cert_info(const json_object *cert_array_jso, gboolean is_cert,
    json_object *output_array_jso)
{
    gsize certs_cnt = json_object_array_length(cert_array_jso);

    for (gsize i = 0; i < certs_cnt; i++) {
        json_object *cert_jso = json_object_array_get_idx(cert_array_jso, i);
        if (cert_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_array_get_idx %zu failed.", __FUNCTION__, i);
            continue;
        }

        const gchar *cert_str = NULL;
        if (!get_element_str(cert_jso, INTERACT_KEY_CERTIFICATE_STRING, &cert_str)) {
            debug_log(DLOG_ERROR, "%s:get_element_str CertificateString failed, index: %zu", __FUNCTION__, i);
            continue;
        }

        json_object *cert_output_jso = json_object_new_object();
        if (cert_output_jso == NULL) {
            debug_log(DLOG_ERROR, "%s:json_object_new_object failed, index: %zu", __FUNCTION__, i);
            continue;
        }
        json_object_object_add(cert_output_jso, INTERACT_KEY_CERTIFICATE_STRING, json_object_new_string(cert_str));

        const gchar *cert_type = "";
        if (!get_element_str(cert_jso, INTERACT_KEY_CERTIFICATE_TYPE, &cert_type)) {
            debug_log(DLOG_ERROR, "%s:get_element_str CertificateType failed, index: %zu", __FUNCTION__, i);
        }
        json_object_object_add(cert_output_jso, INTERACT_KEY_CERTIFICATE_TYPE, json_object_new_string(cert_type));

        if (is_cert && g_strcmp0(cert_type, "PEM") == 0) {
            
            parse_cert_details(cert_str, cert_output_jso);
        }

        const gchar *owner = "";
        if (!get_element_str(cert_jso, INTERACT_KEY_UEFI_SIGNATUREOWNER, &owner)) {
            debug_log(DLOG_ERROR, "%s:get_element_str UefiSignatureOwner failed, index: %zu", __FUNCTION__, i);
        }
        json_object_object_add(cert_output_jso, INTERACT_KEY_UEFI_SIGNATUREOWNER, json_object_new_string(owner));
        json_object_array_add(output_array_jso, cert_output_jso);
    }
}


LOCAL gint32 parse_secureboot_database(const json_object *database_jso, const gchar *database_id,
    json_object *output_jso)
{
    json_object *database_array = json_object_new_array();
    if (database_array == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(output_jso, database_id, database_array);

    json_object *db_id_jso = NULL;
    if (!json_object_object_get_ex(database_jso, database_id, &db_id_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex database_id: %s failed", __FUNCTION__, database_id);
        return RET_ERR;
    }

    json_object *cert_array_jso = NULL;
    if (!json_object_object_get_ex(db_id_jso, INTERACT_KEY_CERTIFICATES, &cert_array_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex Certificates failed", __FUNCTION__);
        return RET_ERR;
    }

    foreach_array_parse_cert_info(cert_array_jso, TRUE, database_array);

    return RET_OK;
}


LOCAL gint32 parse_secureboot_cert_info(const json_object *secureboot_jso, json_object *output_cert_jso)
{
    json_object *database_jso = NULL;

    if (!json_object_object_get_ex(secureboot_jso, INTERACT_KEY_SECUREBOOT_DATABASES, &database_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex SecureBootDataBases failed", __FUNCTION__);
        return RET_ERR;
    }

    json_object *secureboot_output_jso = json_object_new_object();
    if (secureboot_output_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(output_cert_jso, INTERACT_KEY_SECUREBOOT, secureboot_output_jso);

    gint32 ret = parse_secureboot_database(database_jso, INTERACT_KEY_SECUREBOOT_DB, secureboot_output_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parse_secureboot_database failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = parse_secureboot_database(database_jso, INTERACT_KEY_SECUREBOOT_DBX, secureboot_output_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parse_secureboot_database failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 parse_boot_cert_properties(const json_object *boot_jso, json_object *output_cert_jso)
{
    // 获取Boot下面的Certificates数组
    json_object *cert_array_jso = NULL;
    if (!json_object_object_get_ex(boot_jso, INTERACT_KEY_CERTIFICATES, &cert_array_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex Certificates failed", __FUNCTION__);
        return RET_ERR;
    }

    // 遍历解析Certificates
    foreach_array_parse_cert_properties(cert_array_jso, TRUE, output_cert_jso);

    return RET_OK;
}


LOCAL gint32 parse_boot_cert_info(const json_object *boot_jso, json_object *output_cert_jso)
{
    json_object *boot_output_jso = json_object_new_object();
    if (boot_output_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_object failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(output_cert_jso, INTERACT_KEY_BOOT, boot_output_jso);

    // 获取Boot下面的Certificates数组
    json_object *cert_array_jso = NULL;
    if (!json_object_object_get_ex(boot_jso, INTERACT_KEY_CERTIFICATES, &cert_array_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex Certificates failed", __FUNCTION__);
        return RET_ERR;
    }

    json_object *https_cert_jso = json_object_new_array();
    if (https_cert_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(boot_output_jso, HTTPSBOOT_CERT, https_cert_jso);

    // 遍历解析Certificates
    foreach_array_parse_cert_info(cert_array_jso, TRUE, https_cert_jso);

    // 获取Boot下面的Crl数组
    json_object *crl_array_jso = NULL;
    if (!json_object_object_get_ex(boot_jso, INTERACT_KEY_CRL, &crl_array_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex Crl failed", __FUNCTION__);
        return RET_ERR;
    }

    json_object *crl_array = json_object_new_array();
    if (crl_array == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_new_array failed", __FUNCTION__);
        return RET_ERR;
    }
    json_object_object_add(boot_output_jso, HTTPSBOOT_CRL, crl_array);

    // 遍历解析Crl
    foreach_array_parse_cert_info(crl_array_jso, FALSE, crl_array);

    return RET_OK;
}


LOCAL gint32 parse_bios_cert_info(const json_object *cur_secureboot_jso, json_object *output_cert_jso)
{
    json_object *secureboot_cert_jso = NULL;

    if (!json_object_object_get_ex(cur_secureboot_jso, INTERACT_KEY_SECUREBOOT, &secureboot_cert_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed", __FUNCTION__);
        return RET_ERR;
    }

    // 解析secureboot证书
    gint32 ret = parse_secureboot_cert_info(secureboot_cert_jso, output_cert_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parse_secureboot_cert_info failed", __FUNCTION__);
        return RET_ERR;
    }

    json_object *boot_cert_jso = NULL;
    if (!json_object_object_get_ex(cur_secureboot_jso, INTERACT_KEY_BOOT, &boot_cert_jso)) {
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex failed", __FUNCTION__);
        return RET_ERR;
    }

    // 解析boot证书
    ret = parse_boot_cert_info(boot_cert_jso, output_cert_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parse_boot_cert_info failed", __FUNCTION__);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 method_bios_get_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "%s: invalid param", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    gint32 ret = check_bios_state(obj_handle);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: check_bios_state failed", __FUNCTION__);
        return ret;
    }

    if (g_file_test(SECUREBOOT_CURRENT_JSON_FILE, G_FILE_TEST_EXISTS) == FALSE ||
        g_file_test(SECUREBOOT_CURRENT_JSON_FILE, G_FILE_TEST_IS_SYMLINK) == TRUE) {
        debug_log(DLOG_DEBUG, "%s: secureboot json does not exist or is invalid.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    _cleanup_json_object_ json_object *cur_secureboot_jso = json_object_from_file(SECUREBOOT_CURRENT_JSON_FILE);
    if (cur_secureboot_jso == NULL) {
        debug_log(DLOG_ERROR, "%s: json_object_from_file failed", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    _cleanup_json_object_ json_object *bios_cert_jso = json_object_new_object();
    ret = parse_bios_cert_info(cur_secureboot_jso, bios_cert_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parse_bios_cert_info failed", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_string(dal_json_object_get_str(bios_cert_jso)));

    return SECUREBOOT_METHOD_OK;
}

gint32 method_bios_get_https_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (output_list == NULL) {
        debug_log(DLOG_ERROR, "[%s] invalid param", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    gint32 ret = check_bios_state(obj_handle);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "[%s] check_bios_state failed", __FUNCTION__);
        return ret;
    }

    if (g_file_test(SECUREBOOT_CURRENT_JSON_FILE, G_FILE_TEST_EXISTS) == FALSE ||
        g_file_test(SECUREBOOT_CURRENT_JSON_FILE, G_FILE_TEST_IS_SYMLINK) == TRUE) {
        debug_log(DLOG_ERROR, "[%s] secureboot json does not exist or is invalid.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    _cleanup_json_object_ json_object *file_obj = json_object_from_file(SECUREBOOT_CURRENT_JSON_FILE);
    if (file_obj == NULL) {
        debug_log(DLOG_ERROR, "[%s] json_object_from_file failed", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    json_object *boot_cert_jso = NULL;
    if (!json_object_object_get_ex(file_obj, INTERACT_KEY_BOOT, &boot_cert_jso)) {
        debug_log(DLOG_ERROR, "[%s] json_object_object_get_ex '%s' failed", __FUNCTION__, INTERACT_KEY_BOOT);
        return SECUREBOOT_METHOD_ERR;
    }

    json_object *output_cert_jso = json_object_new_array();
    // 解析https boot证书
    ret = parse_boot_cert_properties(boot_cert_jso, output_cert_jso);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: parse_boot_cert_properties failed", __FUNCTION__);
        goto EXIT;
    }

    *output_list = g_slist_append(*output_list, g_variant_new_string(dal_json_object_get_str(output_cert_jso)));

EXIT:
    json_object_put(output_cert_jso);
    return ret;
}


LOCAL int pem_single_verify(BIO *bio)
{
    // 尝试加载内容，验证是否符合PEM格式
    _cleanup_x509_ X509 *x509 = PEM_read_bio_X509(bio, NULL, 0, NULL);
    if (x509 == NULL) {
        debug_log(DLOG_ERROR, "%s: PEM format is invalid", __FUNCTION__);
        return SECUREBOOT_METHOD_PEM_FORMAT_ERR;
    }
    
    // 校验证书有效期
    gint32 ret = dal_check_cert_time_x509((const X509*)x509);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Check certificate time failed", __FUNCTION__);
        return SECUREBOOT_METHOD_EXPIRED_CERT;
    }

    return SECUREBOOT_METHOD_OK;
}


LOCAL int pem_crl_verify(BIO *bio)
{
    // 尝试加载内容，验证是否符合PEM格式
    _cleanup_x509_crl_ X509_CRL *x = PEM_read_bio_X509_CRL(bio, NULL, 0, NULL);
    if (x == NULL) {
        debug_log(DLOG_ERROR, "%s: PEM format is invalid", __FUNCTION__);
        return SECUREBOOT_METHOD_PEM_FORMAT_ERR;
    }

    return SECUREBOOT_METHOD_OK;
}

LOCAL gboolean is_supported(const gchar *type, const gchar *support_array[], gsize arr_len)
{
    for (gsize i = 0; i < arr_len; i++) {
        if (g_strcmp0(type, support_array[i]) == 0) {
            return TRUE;
        }
    }
    debug_log(DLOG_ERROR, "%s: Type(%s) not supported", __FUNCTION__, type);
    return FALSE;
}

LOCAL gint32 verify_cert_usage(const gchar *usage)
{
    gboolean supported = is_supported(usage, g_supported_cert_usage, G_N_ELEMENTS(g_supported_cert_usage));
    if (!supported) {
        debug_log(DLOG_ERROR, "%s: Verify cert usage failed.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    return SECUREBOOT_METHOD_OK;
}

LOCAL gint32 verify_database(const gchar *database)
{
    gboolean supported = is_supported(database, g_supported_database, G_N_ELEMENTS(g_supported_database));
    if (!supported) {
        debug_log(DLOG_ERROR, "%s: Verify database failed.", __FUNCTION__);
        return SECUREBOOT_METHOD_INVALID_PARAM;
    }

    return SECUREBOOT_METHOD_OK;
}

LOCAL gint32 verify_cert_type(const gchar *cert_type)
{
    gboolean supported = is_supported(cert_type, g_supported_cert_type, G_N_ELEMENTS(g_supported_cert_type));
    if (!supported) {
        debug_log(DLOG_ERROR, "%s: Verify cert type failed.", __FUNCTION__);
        return SECUREBOOT_METHOD_INVALID_PARAM;
    }

    return SECUREBOOT_METHOD_OK;
}

LOCAL gint32 verify_reset_type(const gchar *reset_type)
{
    if (g_strcmp0(reset_type, CERT_OPERATION_DELETE) != 0 &&
        g_strcmp0(reset_type, CERT_OPERATION_RESET) != 0) {
        debug_log(DLOG_ERROR, "%s: Verify reset type failed.", __FUNCTION__);
        return SECUREBOOT_METHOD_INVALID_PARAM;
    }

    return SECUREBOOT_METHOD_OK;
}

LOCAL gint32 verify_pem_type(const gchar *pem_type)
{
    gboolean supported = is_supported(pem_type, g_supported_pem_type, G_N_ELEMENTS(g_supported_pem_type));
    if (!supported) {
        debug_log(DLOG_ERROR, "%s: Verify pem type failed.", __FUNCTION__);
        // 内部方法参数错误，不对外呈现
        return SECUREBOOT_METHOD_ERR;
    }

    return SECUREBOOT_METHOD_OK;
}


LOCAL gint32 verify_cert_content(const gchar *cert_string, const gchar *pem_type)
{
    gint32 ret = SECUREBOOT_METHOD_ERR; // 下面for循环中条件进不去时默认返回ERR
    if (verify_pem_type(pem_type) != SECUREBOOT_METHOD_OK) {
        return SECUREBOOT_METHOD_ERR;
    }
    // 通过verify_map根据证书类型进行自定义校验
    _cleanup_bio_ BIO *bio = BIO_new(BIO_s_mem());
    BIO_write(bio, cert_string, strlen(cert_string));
    for (gint32 i = 0; i < G_N_ELEMENTS(verify_map); i++) {
        if (g_strcmp0(pem_type, verify_map[i].type) != 0) {
            continue;
        }
        ret = verify_map[i].verify_func(bio);
        if (ret != SECUREBOOT_METHOD_OK) {
            debug_log(DLOG_ERROR, "%s: Verify certificate content failed.", __FUNCTION__);
            return ret;
        }
        break;
    }

    return ret;
}

LOCAL gint32 verify_cert_string(const gchar *cert_string, const gchar *pem_type)
{
    // 校验字符串长度
    size_t len = strlen(cert_string);
    if (len > CERT_MAX_SIZE) {
        return SECUREBOOT_METHOD_STRING_TOO_LONG;
    }

    // 校验字符内容
    return verify_cert_content(cert_string, pem_type);
}

LOCAL gint32 check_import_param(const gchar *usage, const gchar *database, const gchar *cert_string,
                                const gchar *cert_type)
{
    if (usage == NULL || database == NULL || cert_string == NULL ||
        cert_type == NULL) {
        debug_log(DLOG_ERROR, "%s: Input is null.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    // 校验Usage是否合法
    gint32 ret = verify_cert_usage(usage);
    if (ret != SECUREBOOT_METHOD_OK) {
        return ret;
    }

    // 校验database是否合法
    ret = verify_database(database);
    if (ret != SECUREBOOT_METHOD_OK) {
        return ret;
    }

    // 校验cert_type是否合法
    ret = verify_cert_type(cert_type);
    if (ret != SECUREBOOT_METHOD_OK) {
        return ret;
    }

    // db类型为crl时，对应的PEM类型为PEM_TYPE_X509_CRL
    if (g_strcmp0(cert_type, CERTIFICATE_TYPE_SINGLE_PEM) == 0 &&
        g_strcmp0(database, HTTPSBOOT_DATABASE_CRL) == 0) {
        return verify_cert_string(cert_string, PEM_TYPE_X509_CRL);
    }

    // 非CRL场景，pem类型与外部输入的cert_type一致，当前值支持PEM，后续扩展PEMchain、PKCS7等标准格式
    return verify_cert_string(cert_string, cert_type);
}

LOCAL gint32 check_reset_param(const gchar *usage, const gchar *database, const gchar *reset_type)
{
    if (usage == NULL || database == NULL || reset_type == NULL) {
        debug_log(DLOG_ERROR, "%s: Input is null.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    // 校验Usage是否合法
    gint32 ret = verify_cert_usage(usage);
    if (ret != SECUREBOOT_METHOD_OK) {
        return ret;
    }

    // 校验database是否合法
    ret = verify_database(database);
    if (ret != SECUREBOOT_METHOD_OK) {
        return ret;
    }

    // 校验reset_type是否合法
    return verify_reset_type(reset_type);
}


LOCAL gint32 add_cert_object(json_object *cert_array_obj, json_object *add_obj)
{
    // 判断导入项是否超限制
    gsize num = json_object_array_length(cert_array_obj);
    if (num >= BMC_MAX_CERT_NUM) {
        debug_log(DLOG_ERROR, "%s: Imported item number reached limitation, importing is not allowed.", __FUNCTION__);
        return SECUREBOOT_METHOD_IMPORT_LIMIT;
    }

    // 将待添加的obj的加入cert_array_obj
    (void)json_object_array_add(cert_array_obj, add_obj);
    return SECUREBOOT_METHOD_OK;
}


LOCAL gint32 add_reset_object(json_object *cert_array_obj, json_object *add_obj)
{
    // 清空数组
    gsize num = json_object_array_length(cert_array_obj);
    json_object_array_del_idx(cert_array_obj, 0, num);

    // 将待添加的obj的加入cert_array_obj
    (void)json_object_array_add(cert_array_obj, add_obj);
    return SECUREBOOT_METHOD_OK;
}


LOCAL json_object *create_default_file_object(void)
{
    json_object *file_obj = json_object_new_object();
    json_object *secureboot_obj = json_object_new_object();
    json_object *database_obj = json_object_new_object();
    json_object *database_db_obj = json_object_new_object();
    json_object *database_dbx_obj = json_object_new_object();
    json_object *boot_obj = json_object_new_object();

    // 创建secureboot空对象
    (void)json_object_object_add(database_db_obj, INTERACT_KEY_CERTIFICATES, json_object_new_array());
    (void)json_object_object_add(database_dbx_obj, INTERACT_KEY_CERTIFICATES, json_object_new_array());
    (void)json_object_object_add(database_obj, INTERACT_KEY_SECUREBOOT_DB, database_db_obj);
    (void)json_object_object_add(database_obj, INTERACT_KEY_SECUREBOOT_DBX, database_dbx_obj);
    (void)json_object_object_add(secureboot_obj, INTERACT_KEY_SECUREBOOT_DATABASES, database_obj);

    // 创建boot空对象
    (void)json_object_object_add(boot_obj, INTERACT_KEY_CERTIFICATES, json_object_new_array());
    (void)json_object_object_add(boot_obj, INTERACT_KEY_CRL, json_object_new_array());

    // 合并对象
    (void)json_object_object_add(file_obj, INTERACT_KEY_SECUREBOOT, secureboot_obj);
    (void)json_object_object_add(file_obj, INTERACT_KEY_BOOT, boot_obj);

    return file_obj;
}


LOCAL json_object *get_json_file_object(const gchar* json_file)
{
    json_object *file_obj = NULL;

    if (vos_get_file_accessible(json_file) == TRUE &&
        vos_get_file_length(json_file) != 0) {
        // 文件存在，则从文件获取整个json对象
        file_obj = json_object_from_file(json_file);
    } else {
        // 不存在或为空时创建新对象
        file_obj = create_default_file_object();
    }

    return file_obj;
}


LOCAL json_object *get_cert_array_obj(json_object *file_obj, const gchar *usage, const gchar *database)
{
    json_object *usage_obj         = NULL;
    json_object *secureboot_db_obj = NULL;
    json_object *database_obj      = NULL;
    json_object *cert_array_obj    = NULL;
    const char *usage_key          = NULL;

    if (g_strcmp0(usage, CERT_USAGE_SECUREBOOT) == 0) {
        usage_key = INTERACT_KEY_SECUREBOOT;
    } else {
        usage_key = INTERACT_KEY_BOOT;
    }
    (void)json_object_object_get_ex(file_obj, usage_key, &usage_obj);

    if (g_strcmp0(database, SECUREBOOT_DATABASE_DB) == 0 ||
        g_strcmp0(database, SECUREBOOT_DATABASE_DBX) == 0) {
        (void)json_object_object_get_ex(usage_obj, INTERACT_KEY_SECUREBOOT_DATABASES, &secureboot_db_obj);
        (void)json_object_object_get_ex(secureboot_db_obj, database, &database_obj);
        (void)json_object_object_get_ex(database_obj, INTERACT_KEY_CERTIFICATES, &cert_array_obj);
    } else if (g_strcmp0(database, HTTPSBOOT_DATABASE_CERT) == 0) {
        (void)json_object_object_get_ex(usage_obj, INTERACT_KEY_CERTIFICATES, &cert_array_obj);
    } else if (g_strcmp0(database, HTTPSBOOT_DATABASE_CRL) == 0) {
        (void)json_object_object_get_ex(usage_obj, INTERACT_KEY_CRL, &cert_array_obj);
    }

    return cert_array_obj;
}


LOCAL json_object *make_import_item(const gchar *cert_string, const gchar *cert_type)
{
    json_object *item_obj = json_object_new_object();
    (void)json_object_object_add(item_obj, INTERACT_KEY_OPERATION, json_object_new_string(CERT_OPERATION_ADD));
    (void)json_object_object_add(item_obj, INTERACT_KEY_CERTIFICATE_STRING, json_object_new_string(cert_string));
    (void)json_object_object_add(item_obj, INTERACT_KEY_CERTIFICATE_TYPE, json_object_new_string(cert_type));
    (void)json_object_object_add(item_obj, INTERACT_KEY_SIZE, json_object_new_int((gint32)strlen(cert_string)));

    return item_obj;
}


LOCAL json_object *make_update_item(const gchar *cert_string, const gchar *cert_type)
{
    json_object *item_obj = json_object_new_object();
    (void)json_object_object_add(item_obj, INTERACT_KEY_CERTIFICATE_STRING, json_object_new_string(cert_string));
    (void)json_object_object_add(item_obj, INTERACT_KEY_CERTIFICATE_TYPE, json_object_new_string(cert_type));

    return item_obj;
}

LOCAL gint32 write_object_to_file(json_object *file_obj)
{
    FM_FILE_S *fp = fm_fopen(SECUREBOOT_NEW_SHORT_NAME, "w");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: Open bios secureboot file failed.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    const char* json_str = dal_json_object_get_str(file_obj);
    guint32 write_size = fm_fwrite(json_str, 1, strlen(json_str), fp);
    fm_fclose(fp);
    if (write_size != strlen(json_str)) {
        debug_log(DLOG_ERROR, "%s: The size of json string which has been written is error.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    return SECUREBOOT_METHOD_OK;
}

LOCAL gint32 write_object_to_cur_file(json_object *file_obj)
{
    FM_FILE_S *fp = fm_fopen(SECUREBOOT_CURRENT_SHORT_NAME, "w");
    if (fp == NULL) {
        debug_log(DLOG_ERROR, "%s: Open bios secureboot file failed.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    const char* json_str = dal_json_object_get_str(file_obj);
    guint32 write_size = fm_fwrite(json_str, 1, strlen(json_str), fp);
    fm_fclose(fp);
    if (write_size != strlen(json_str)) {
        debug_log(DLOG_ERROR, "%s: The size of json string which has been written is error.", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    return SECUREBOOT_METHOD_OK;
}


LOCAL gint32 cert_array_add_object(const gchar *operation, json_object *cert_array_obj, json_object *add_obj)
{
    // 根据操作类型添加json对象
    gint32 ret;
    if (g_strcmp0(operation, CERT_OPERATION_ADD) == 0) {
        ret = add_cert_object(cert_array_obj, add_obj);
        if (ret != SECUREBOOT_METHOD_OK) {
            debug_log(DLOG_ERROR, "%s: Add certificate json object failed", __FUNCTION__);
            return ret;
        }
    } else {
        ret = add_reset_object(cert_array_obj, add_obj);
        if (ret != SECUREBOOT_METHOD_OK) {
            debug_log(DLOG_ERROR, "%s: Add reset json object failed", __FUNCTION__);
            return ret;
        }
    }

    return ret;
}

LOCAL gint32 import_cert(const gchar *usage, const gchar *database, const gchar *cert_string,
    const gchar *cert_type)
{
    // 获取完整的json对象
    _cleanup_json_object_ json_object *file_obj = get_json_file_object(SECUREBOOT_NEW_JSON_FILE);

    // 获取证书列表的json对象
    json_object *cert_array_obj = get_cert_array_obj(file_obj, usage, database);
    if (json_object_get_type(cert_array_obj) != json_type_array) {
        debug_log(DLOG_ERROR, "%s: cert_array_obj is invalid", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    // 构造待添加的对象
    json_object *add_obj = make_import_item(cert_string, cert_type);

    // 导入待添加对象
    gint32 ret = cert_array_add_object(CERT_OPERATION_ADD, cert_array_obj, add_obj);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Certificate list add json object failed", __FUNCTION__);
        (void)json_object_put(add_obj);
        return ret;
    }

    // 写入文件
    return write_object_to_file(file_obj);
}


LOCAL json_object* make_reset_item(const gchar *reset_type)
{
    json_object *reset_obj = json_object_new_object();
    (void)json_object_object_add(reset_obj, INTERACT_KEY_OPERATION, json_object_new_string(reset_type));

    return reset_obj;
}

LOCAL gint32 reset_cert(const gchar *usage, const gchar *database, const gchar *reset_type)
{
    // 获取完整的json对象
    _cleanup_json_object_ json_object *file_obj = get_json_file_object(SECUREBOOT_NEW_JSON_FILE);

    // 获取证书列表的json对象
    json_object *cert_array_obj = get_cert_array_obj(file_obj, usage, database);
    if (json_object_get_type(cert_array_obj) != json_type_array) {
        debug_log(DLOG_ERROR, "%s: cert_array_obj is invalid", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    // 构造待添加的对象
    json_object *add_obj = make_reset_item(reset_type);

    // 导入待添加对象
    gint32 ret = cert_array_add_object(reset_type, cert_array_obj, add_obj);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Certificate list add json object failed", __FUNCTION__);
        (void)json_object_put(add_obj);
        return ret;
    }

    // 写入文件
    return write_object_to_file(file_obj);
}

LOCAL gint32 update_cert_to_json_inner(json_object *file_obj, const guint8 *buf, guint32 buf_len, guint32 *pos)
{
    if (file_obj == NULL || buf == NULL || buf_len == 0 || pos == NULL || *pos >= buf_len) {
        debug_log(DLOG_ERROR, "[%s]param is invalid.", __FUNCTION__);
        return RET_ERR;
    }

    // 后续根据证书类型，取对象的 Certificates对象
    json_object *cert_array_obj = NULL;

    // 证书类型
    guint8 cert_type = buf[(*pos)++];
    switch (cert_type) {
        case BOOT_CERT_TYPE_WHITELIST: // 0x00:白名单证书
            cert_array_obj = get_cert_array_obj(file_obj, CERT_USAGE_SECUREBOOT, SECUREBOOT_DATABASE_DB);
            break;
        case BOOT_CERT_TYPE_BLACKLIST: // 0x01:黑名单证书
            cert_array_obj = get_cert_array_obj(file_obj, CERT_USAGE_SECUREBOOT, SECUREBOOT_DATABASE_DBX);
            break;
        case BOOT_CERT_TYPE_CERT: // 0x02:https证书
            cert_array_obj = get_cert_array_obj(file_obj, CERT_USAGE_BOOT, HTTPSBOOT_DATABASE_CERT);
            break;
        case BOOT_CERT_TYPE_CRL: // 0x03:https CRL
            cert_array_obj = get_cert_array_obj(file_obj, CERT_USAGE_BOOT, HTTPSBOOT_DATABASE_CRL);
            break;
        default:
            break;
    }

    // 证书长度
    guint32 cert_len = MAKE_DWORD(buf[*pos + 3], buf[*pos + 2], buf[*pos + 1], buf[*pos]); // LSB guint32
    *pos += DWORD_LEN;
    if (cert_array_obj == NULL || json_object_get_type(cert_array_obj) != json_type_array || cert_len >= BUF_SIZE_2K) {
        debug_log(DLOG_ERROR, "[%s]cert_array_obj is invalid, cert len = %d", __FUNCTION__, cert_len);
        return RET_ERR;
    }

    // 证书内容
    gchar *cert_string = (gchar *)g_malloc0((gsize)cert_len + 1);
    if (cert_string == NULL) {
        debug_log(DLOG_ERROR, "[%s]g_malloc0 failed.", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret = memcpy_s(cert_string, cert_len, buf + *pos, cert_len);
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "[%s]memcpy_s fail, ret = %d", __FUNCTION__, ret);
        goto EXIT;
    }
    *pos += cert_len;

    // 更新证书
    json_object *update_obj = make_update_item(cert_string, "PEM");
    ret = add_cert_object(cert_array_obj, update_obj);
    if (ret != SECUREBOOT_METHOD_OK) {
        (void)json_object_put(update_obj);
    }

EXIT:
    g_free(cert_string);
    return ret;
}

LOCAL gint32 update_cert_to_json(json_object *file_obj, const guint8 *buf, guint32 buf_len)
{
    if (file_obj == NULL || buf == NULL || buf_len == 0) {
        debug_log(DLOG_ERROR, "[%s]param is invalid.", __FUNCTION__);
        return RET_ERR;
    }
    gint32 ret = RET_OK;
    guint32 parsed_len = 0;
    // 总证书的个数
    guint8 cert_num = buf[parsed_len++];

    for (guint8 i = 0; i < cert_num; i++) {
        ret = update_cert_to_json_inner(file_obj, buf, buf_len, &parsed_len);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "[%s] deal with cert[%u] failed.", __FUNCTION__, i + 1);
            return ret;
        }
    }

    // 写入文件
    return write_object_to_cur_file(file_obj);
}

LOCAL void update_cert_info(void)
{
    OBJ_HANDLE obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_HOSTMU, 0, &obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]dal_get_object_handle_nth of %s failed.", __FUNCTION__, CLASS_HOSTMU);
        return;
    }

    // 从驱动获取证书信息
    GSList* output_list = NULL;
    ret = dfl_call_class_method(obj_handle, METHOD_HOSTMU_GET_CERT_INFO, NULL, NULL, &output_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]call %s.%s failed. ret = %d", __FUNCTION__,
            CLASS_HOSTMU, METHOD_HOSTMU_GET_CERT_INFO, ret);
        return;
    }

    guint32 buf_len = 0;
    const guint8 *buf = (const guint8*)g_variant_get_fixed_array((GVariant *)g_slist_nth_data(output_list, 0),
        (gsize *)&buf_len, sizeof(guint8));
    debug_log(DLOG_DEBUG, "[%s] buf_len = %u", __FUNCTION__, buf_len);

    // 清空SECUREBOOT_CURRENT_JSON_FILE 文件
    (void)vos_rm_filepath(SECUREBOOT_CURRENT_JSON_FILE);

    // 解析证书信息
    _cleanup_json_object_ json_object *file_obj = get_json_file_object(SECUREBOOT_CURRENT_JSON_FILE);
    ret = update_cert_to_json(file_obj, (const guint8*)buf, buf_len);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "[%s]parse_cert_info failed. ret = %d", __FUNCTION__, ret);
    }

    g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
}

gint32 on_hostmu_bootstage_change(OBJ_HANDLE object_handle, gpointer user_data, gchar *property_name,
    GVariant *property_value)
{
    if (property_value == NULL) {
        return RET_ERR;
    }

    guint8 boot_stage = g_variant_get_byte(property_value);
    if (boot_stage == 1) {
        // 为确保能获取到数据，暂定延时 5 秒
        vos_task_delay(5 * VOS_TICK_PER_SECOND);
        update_cert_info();
    }
    return RET_OK;
}
 
LOCAL gint32 import_upgrade_uploadfile_ssl(const gchar *certstring, guint8 ssl_type, guint8 *ssl_state)
{
    GSList *obj_list = NULL;
    gint32 ret = dfl_get_object_list(CLASS_SOC_BOARD, &obj_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "get bcu list fail, ret=%d", ret);
        return ret;
    }
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    input_list = g_slist_append(input_list, g_variant_new_string(certstring));
    input_list = g_slist_append(input_list, g_variant_new_byte(ssl_type));
    *ssl_state = 0;

    // 加锁保障BMC向MCU传输证书数据时是串行操作
    g_mutex_lock(&g_cert_update_mutex);

    for (GSList *node = obj_list; node != NULL; node = node->next) {
        OBJ_HANDLE bcu_handle = (OBJ_HANDLE)node->data;
        ret = dfl_call_class_method(bcu_handle, METHOD_SOC_BOARD_UPLODE_BOOT_CERT, NULL, input_list, NULL);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: call %s.UploadBootCert fail, ret=%d", __FUNCTION__,
                dfl_get_object_name(bcu_handle), ret);
            goto QUIT;
        }
        vos_task_delay(5000); 
        ret = dfl_call_class_method(bcu_handle, METHOD_SOC_BOARD_GET_BOOT_CERT_STATUS, NULL, NULL, &output_list);
        if (ret != DFL_OK) {
            debug_log(DLOG_ERROR, "%s: call %s.GetCertUpdateStatus fail, ret=%d", __FUNCTION__,
                dfl_get_object_name(bcu_handle), ret);
            goto QUIT;
        }
        guint8 single_state = g_variant_get_byte((GVariant *)g_slist_nth_data(output_list, 0));
        *ssl_state = (single_state > *ssl_state) ? single_state : *ssl_state;
        g_slist_free_full(output_list, (GDestroyNotify)g_variant_unref);
        output_list = NULL;
    }

QUIT:
    g_slist_free(obj_list);
    g_mutex_unlock(&g_cert_update_mutex);
    g_slist_free_full(input_list, (GDestroyNotify)g_variant_unref);
    return ret;
}


LOCAL void print_ssl_status_operation_log(GSList *caller_info, guint8 status, const gchar *usage,
    const gchar *database)
{
    switch (status) {
        case BOOT_CERT_STATUS_NA:
        case BOOT_CERT_STATUS_COMPLETE:
        case BOOT_CERT_STATUS_UPDATE:
            method_operation_log(caller_info, NULL, "Import BIOS %s %s successfully", usage, database);
            break;
        default:
            method_operation_log(caller_info, NULL, "Import BIOS %s %s failed", usage, database);
    }
}

LOCAL guint8 transform_cert_type_to_number(const gchar *database)
{
    for (gsize i = 0; i < G_N_ELEMENTS(g_boot_cert_type); i++) {
        if (!strcmp(g_boot_cert_type[i].attr_name, database)) {
            return g_boot_cert_type[i].attr_id;
        }
    }
    return BOOT_CERT_TYPE_UNKNOWN;
}

guint8 get_cert_apply_mode(void)
{
    guint8 cert_apply = 0;
    OBJ_HANDLE bios_handle;
    gint32 ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &bios_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: get bios handle fail, ret: %d",  __FUNCTION__, ret);
        return CERT_APPLY_MODE_NOT_SUPPORT;
    }

    ret = dal_get_property_value_byte(bios_handle, PROPERTY_BIOS_CERT_APPLY_MODE, &cert_apply);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte failed, ret: %d", __FUNCTION__, ret);
        return CERT_APPLY_MODE_NOT_SUPPORT;
    }

    return cert_apply;
}


gint32 method_bios_import_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, "%s: Input is null", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    const gchar *usage       = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 0), NULL);
    const gchar *database    = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 1), NULL);
    const gchar *cert_string = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 2), NULL);
    const gchar *cert_type   = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 3), NULL);
 
    gint32 ret = check_import_param(usage, database, cert_string, cert_type);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Import BIOS %s %s failed, ret = %d", __FUNCTION__, usage, database, ret);
        method_operation_log(caller_info, NULL, "Import BIOS %s %s failed", usage, database);
        return ret;
    }

    ret = check_bios_state(obj_handle);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: check_bios_state failed", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Import BIOS %s %s failed", usage, database);
        return ret;
    }

    ret = import_cert(usage, database, cert_string, cert_type);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Import BIOS %s %s failed, ret = %d", __FUNCTION__, usage, database, ret);
        method_operation_log(caller_info, NULL, "Import BIOS %s %s failed", usage, database);
        return ret;
    }
    // 如果传进来的内容校验没有问题并且导入成功，则将文件传给MCU
    guint8 ssl_state = BOOT_CERT_STATUS_NA;
    if (get_cert_apply_mode() == CERT_APPLY_MODE_MCU_OPCODE) {
        guint8 transfer_type = transform_cert_type_to_number(database);
        ret = import_upgrade_uploadfile_ssl(cert_string, transfer_type, &ssl_state);
    }
    
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: Import BIOS %s %s failed, ret = %d", __FUNCTION__, usage, database, ret);
        method_operation_log(caller_info, NULL, "Import BIOS %s %s failed", usage, database);
        return ret;
    }
    print_ssl_status_operation_log(caller_info, ssl_state, usage, database);
    return ret;
}


gint32 method_bios_reset_cert(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, "%s: Input is null", __FUNCTION__);
        return SECUREBOOT_METHOD_ERR;
    }

    const gchar *usage      = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 0), NULL);
    const gchar *database   = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 1), NULL);
    const gchar *reset_type = g_variant_get_string((GVariant*)g_slist_nth_data(input_list, 2), NULL);

    gint32 ret = check_reset_param(usage, database, reset_type);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Reset BIOS %s %s failed, ret = %d", __FUNCTION__, usage, database, ret);
        method_operation_log(caller_info, NULL, "Reset BIOS %s %s failed", usage, database);
        return ret;
    }

    ret = check_bios_state(obj_handle);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: check_bios_state failed", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Import BIOS %s %s failed", usage, database);
        return ret;
    }

    ret = reset_cert(usage, database, reset_type);
    if (ret != SECUREBOOT_METHOD_OK) {
        debug_log(DLOG_ERROR, "%s: Reset BIOS %s %s failed", __FUNCTION__, usage, database);
        method_operation_log(caller_info, NULL, "Reset BIOS %s %s failed, ret = %d", usage, database, ret);
        return ret;
    }
    
    method_operation_log(caller_info, NULL, "Reset BIOS %s %s successfully", usage, database);
    return ret;
}


gint32 method_bios_get_secureboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    OBJ_HANDLE mcu_obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_SOC_BOARD, 0, &mcu_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]get HostMU fail, ret=%d", __FUNCTION__, ret);
        return ret;
    }

    // 获取secureboot状态
    ret = dfl_call_class_method(mcu_obj_handle, METHOD_MCU_GET_SECUREBOOT, NULL, NULL, output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]call %s failed, ret=%d", __FUNCTION__, METHOD_MCU_GET_SECUREBOOT, ret);
    }
    return ret;
}


gint32 method_bios_set_secureboot(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
#define SECURE_BOOT_ENABLE      "Enable"
#define SECURE_BOOT_DISABLE     "Disable"
    if (input_list == NULL || caller_info == NULL) {
        debug_log(DLOG_ERROR, "%s: Input is null", __FUNCTION__);
        method_operation_log(caller_info, NULL, "Set BIOS SecureBoot failed, no parameter");
        return RET_ERR;
    }
    guint8 secureboot = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    OBJ_HANDLE socboard_obj_handle = 0;
    gint32 ret = dal_get_object_handle_nth(CLASS_SOC_BOARD, 0, &socboard_obj_handle);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "[%s]get SoCBoard fail, ret=%d", __FUNCTION__, ret);
        method_operation_log(caller_info, NULL, "Set BIOS SecureBoot %s failed",
            (secureboot == 1 ? SECURE_BOOT_ENABLE : SECURE_BOOT_DISABLE));
        return ret;
    }

    ret = dfl_call_class_method(socboard_obj_handle, METHOD_SOC_BOARD_SET_SECUREBOOT, NULL, input_list, output_list);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:call %s fail, ret = %d", __FUNCTION__, METHOD_SOC_BOARD_SET_SECUREBOOT, ret);
        method_operation_log(caller_info, NULL, "Set BIOS SecureBoot %s failed",
            (secureboot == 1 ? SECURE_BOOT_ENABLE : SECURE_BOOT_DISABLE));
        return ret;
    }

    method_operation_log(caller_info, NULL, "Set BIOS SecureBoot %s successfully",
        (secureboot == 1 ? SECURE_BOOT_ENABLE : SECURE_BOOT_DISABLE));
    return RET_OK;
}