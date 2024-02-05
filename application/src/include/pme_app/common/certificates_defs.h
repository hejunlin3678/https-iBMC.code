/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 证书管理相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __CERTIFICATES_DEFINE_H__
#define __CERTIFICATES_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define CSR_NEW_AES_ENCRYPT_TYPE 0
#define CSR_AES_DECRYPT_TYPE 1
#define CSR_OLD_AES_DECRYPT_TYPE 2
#define CSR_NEW_AES_DECRYPT_TYPE 3

#define OLD_AES_128_ENCRYPT_DECRYPT_TYPE 0
#define OLD_AES_256_ENCRYPT_DECRYPT_TYPE 1


#define DEFAULT_CA_FILE "/opt/pme/web/conf/ssl.key/A-server.pem"
#define CUSTOMIZE_CERT_FILE "/data/opt/pme/conf/customize"
#define CSR_PRIVATE_KEY "/data/opt/pme/conf/server.key"
#define TMP_PUBLIC_KEY "/dev/shm/t_cert.pem"
#define CSR_FILE "/data/share/csr/server.csr"
#define TMP_CSR_FILE "/tmp/server.csr"
#define CSR_PATH "/data/share/csr/"
#define CSR_CFG_FILE "/data/opt/pme/conf/ssl/csr.conf"
#define TMP_SERVER_PEM_FILE "/dev/shm/server_tmp.pem"
#define TMP_CUSTOM_CERT_PFX_FILE "/dev/shm/custom_cert_tmp.pfx"
#define RS_TMP_SERVER_PEM_FILE "/tmp/server_tmp.pem"
#define CUSTOM_CERT_DEFAULT_PATH "/dev/shm/custom_cert.pfx"
#define CUSTOM_CERT_PEM_DEFAULT_PATH "/dev/shm/custom_cert.pem"

/* customize.pfx说明(加密) */
/* 1、用户导入的证书 */
/* 2、白牌定制时的证书，类似用户导入的证书 */
/* 3、BMC动态生成的RSA算法证书(旧版本生成，新版本不生成，但旧版本升级到新版本的时候会保留) */
/* 4、安全红线3.0之后通过生产在PKI平台申请的设备唯一证书 */
#define SSL_ENCRYPT_CUSTOMIZE_CERT "/data/opt/pme/conf/customize.pfx"
#define SSL_ENCRYPT_CSR_PRIVATE_KEY "/data/opt/pme/conf/server_enc.key"
#define SAVE_IV_PASS_FP "/opt/pme/conf/ssl/piv.conf"

/* 预置证书秘钥piv.conf的备份 */
#define SAVE_IV_PASS_BACKUP_FP "/data/opt/pme/conf/ssl/piv.conf"

/* A-server.pfx说明(加密) */
/* 1、旧版本(3.28之前)，自签名证书(实际上已经没用，因为旧版本会动态生成证书) */
/* 2、新版本(3.28及之后)，PKI申请的RSA加密算法证书 */
#define SSL_ENCRYPT_BACKUP_CERT "/opt/pme/web/conf/ssl.key/A-server.pfx"

/* 预置证书 A-server.pfx的备份 */
#define SSL_ENCRYPT_BACKUP_BACKUP_CERT "/data/opt/pme/conf/ssl/B-server.pfx"

/* A-server-ECC.pfx说明(加密) */
/* 1、旧版本(3.28之前)，不存在 */
/* 2、新版本(3.28及之后)，PKI申请的ECC加密算法证书 */
#define SSL_ENCRYPT_BACKUP_ECC_CERT "/opt/pme/web/conf/ssl.key/A-server-ECC.pfx"

/* 预置证书 A-server-ECC.pfx的备份 */
#define SSL_ENCRYPT_BACKUP_ECC_BACKUP_CERT "/data/opt/pme/conf/ssl/B-server-ECC.pfx"

/* server.pfx说明(加密) */
/* BMC当前使用的证书 */
#define SSL_ENCRYPT_CERT "/data/opt/pme/web/conf/ssl.key/server.pfx"

#define CSR_EXPORT_FILE_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.(csr|php)$"
#define SERVER_CERT_IMPORT_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.(crt|cer|pem)$"
#define LDAP_CERT_IMPORT_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.(crt|cer|cert|pem)$"
#define CRL_IMPORT_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.crl$"
#define KRB_KEYTABLE_IMPORT_REGEX "^(/tmp/.{1,246})\\.(keytab)$"
#define CUSTOM_CERT_IMPORT_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.(p12|pfx)$"
#define LICENSE_FILE_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.(xml|dat|XML|DAT)$"
#define NTPKEY_FILE_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,250})$"
#define HTTPS_TRUST_CERT_IMPORT_REGEX "^(/tmp/.{1,246})\\.(crt|cer|pem)$"
#define IMPORT_CONFIGURE_REGEX "^((https|sftp|nfs|cifs|scp)://.{1,1000}|/tmp/.{1,246})\\.xml$"

#define LDAP_CACERT_UPLOAD_SUCCESS 0
#define LDAP_CACERT_UPLOAD_FAILED 2
#define LDAP_CACERT_TOO_BIG 1
#define LDAP_CACERT_FORMAT_FAILED 3
#define LDAP_CACERT_OVERDUED 4
#define CERT_CHAIN_LDAP_MAX 10

#define CSR_CFG_GROUPNAME "SSL"
#define MAX_SERVER_CERT_FILE_LEN (100 * 1024)    /* 证书最大大小100k */
#define MAX_CRL_FILE_LEN (100 * 1024)            /* CRL文件最大大小100k */
#define MAX_ENCRYPTED_CERT_FILE_LEN (100 * 1024) /* 自定义证书大小限定100KB */


/* **************导入失败类型************** */
#define IMPORT_OK 0
#define IMPORT_FAIL 1
#define NO_MORE_MEM 2
#define FILE_MISMATCH 3
#define CUSTOM_CERT_IMPORT_FAIL 4
#define IMPORT_COMMON_ERR 5 /* 证书导入通用错误 */
#define CA_MD_TOO_WEAK 7    /* 证书加密算法太弱(MD5) */
#define SSL_CACERT_OVERDUED 8
#define CA_SHA1_TOO_WEAK 9


#define SSL_PARAMETER_LEN2 2
#define SSL_PARAMETER_LEN64 64
#define SSL_PARAMETER_LEN128 128
#define SSL_INFO_LEN256 256

#define LDAP_SERVER1 1
#define LDAP_SERVER2 2
#define LDAP_SERVER3 3
#define LDAP_SERVER4 4
#define LDAP_SERVER5 5
#define LDAP_SERVER6 6
#define LDAP_CERT_LEN512 512


/* 证书类型
0: 保留
1: 服务器证书
2: 中间证书
3: 根证书
*/
#define CERT_TYPE_RESERVED 0
#define CERT_TYPE_SERVER 1
#define CERT_TYPE_INTERMEDIATE 2
#define CERT_TYPE_ROOT 3
#define CERT_TYPE_RESERVED_STR "Reserved"
#define CERT_TYPE_SERVER_STR "Server Certificate"
#define CERT_TYPE_INTERMEDIATE_STR "Intermediate Certificate"
#define CERT_TYPE_ROOT_STR "Root Certificate"
#define CERT_INFO_STR_REGEX "CN=(.{0,}), OU=(.{0,}), O=(.{0,}), L=(.{0,}), S=(.{0,}), C=(.{0,})"
#define CERT_INFO_MATCH_COUNT 7    /* 字符串本身+6个匹配部分 */
#define MAX_CERT_CHAIN_LEVEL 10    /* 最大支持10级证书链 */
#define MAX_CERT_NAME_INFO_LEN 256 /* 使用者或颁发者名称最大长度 */

#define CERT_KIND_SERVER_CERT 0              /* 服务器证书 */
#define CERT_KIND_CERT_CHAIN 1               /* 证书链 */
#define CERT_CHAIN_SSL_OBJ_INITIAL_CERT_ID 4 /* 证书链相关SSL对象，CertId从4开始 */


#define MAX_COMPONENT_STR_LEN 32
#define MAX_CSR_FILE_LEN (1024 * 10)

/* 0 表示未开启生成或生成成功，1 表示正在生成中，2 表示CSR生成失败 */
#define CSR_GEN_READY_OR_OK 0
#define CSR_GENERATING 1
#define CSR_GEN_FAILED 2

#define MK_UPDATE_READY_OR_OK 0
#define MK_UPDATING 1
#define MK_UPDATED 2
#define MK_UPDATE_FAILED 3

#define DECRYPT_CERT_PATH "/data/opt/pme/web/conf/ssl.key/server.pem"
#define DECRYPT_CERT_PEM_PATH "/data/opt/pme/web/conf/ssl.key/cert.pem"
#define DECRYPT_CERT_KEY_PATH "/data/opt/pme/web/conf/ssl.key/cert.key"
#define DECRYPT_CERT_PER_FOR_DEFAULT_SERVER_PATH "/data/opt/pme/web/conf/ssl.key/cert1.pem"
#define DECRYPT_CERT_KEY_FOR_DEFAULT_SERVER_PATH "/data/opt/pme/web/conf/ssl.key/cert1.key"
#define LDAP_CACERT_PATH "/data/opt/pme/conf/ssl/ldap_cacert.cert"
#define LDAP_CACERT_PATH2 "/data/opt/pme/conf/ssl/ldap_cacert2.cert"
#define LDAP_CACERT_PATH3 "/data/opt/pme/conf/ssl/ldap_cacert3.cert"
#define LDAP_CACERT_PATH4 "/data/opt/pme/conf/ssl/ldap_cacert4.cert"
#define LDAP_CACERT_PATH5 "/data/opt/pme/conf/ssl/ldap_cacert5.cert"
#define LDAP_CACERT_PATH6 "/data/opt/pme/conf/ssl/ldap_cacert6.cert"
#define LDAP_CACERT_NO 6
#define SYSLOG_FILE_PATH_CA "/data/etc/syslog-ng/ca.d/ca.crt"
#define SYSLOG_FILE_PATH_CERT_DIR "/data/etc/syslog-ng/cert.d"
#define SYSLOG_FILE_PATH_CERT "/data/etc/syslog-ng/cert.d/client.pem"

#define LDAP_CRL_PATH1 "/data/opt/pme/conf/ssl/ldap1.crl"
#define LDAP_CRL_PATH2 "/data/opt/pme/conf/ssl/ldap2.crl"
#define LDAP_CRL_PATH3 "/data/opt/pme/conf/ssl/ldap3.crl"
#define LDAP_CRL_PATH4 "/data/opt/pme/conf/ssl/ldap4.crl"
#define LDAP_CRL_PATH5 "/data/opt/pme/conf/ssl/ldap5.crl"
#define LDAP_CRL_PATH6 "/data/opt/pme/conf/ssl/ldap6.crl"

#define LDAP_CERT_SOFT_LINK_PATH1 "/data/opt/pme/conf/ssl/ldap1_ca.d"
#define LDAP_CERT_SOFT_LINK_PATH2 "/data/opt/pme/conf/ssl/ldap2_ca.d"
#define LDAP_CERT_SOFT_LINK_PATH3 "/data/opt/pme/conf/ssl/ldap3_ca.d"
#define LDAP_CERT_SOFT_LINK_PATH4 "/data/opt/pme/conf/ssl/ldap4_ca.d"
#define LDAP_CERT_SOFT_LINK_PATH5 "/data/opt/pme/conf/ssl/ldap5_ca.d"
#define LDAP_CERT_SOFT_LINK_PATH6 "/data/opt/pme/conf/ssl/ldap6_ca.d"

#define KRB_KEYTABLE_PATH "/data/opt/pme/conf/ssl/kerberos.keytab"
#define KRB_PW_KEYTABLE_PATH "/data/opt/pme/conf/ssl/kerberos_pw.keytab"
#define KRB_CONF_PATH "/data/opt/pme/conf/ssl/kerberos.conf"
#define KRB_ENC_KEYTABLE_PATH "/data/opt/pme/conf/ssl/kerberos.pfx"
#define KRB_CCACHE_FILE_DIR "/data/opt/pme/conf/ssl"
#define KRB_CCACHE_FILE_PREFIX "krb5cc_"
#define KRB_RCACHE_FILE_DIR "/data/var/tmp"
#define KRB_RCACHE_FILE_PREFIX "HTTP_"

#define KRB_CONF_SHORT_FILE_NAME "kerberos.conf"

// 证书过期判断设置为7-180天
#define CERT_OVERDUE_TIME_MAX 180
#define CERT_OVERDUE_TIME_MIN 7

#ifdef UNIT_TEST
#ifdef ETC_SYSCTL_CONF_FILE
#undef ETC_SYSCTL_CONF_FILE
#endif
#define ETC_SYSCTL_CONF_FILE "./file2"
#else
#define ETC_SYSCTL_CONF_FILE "/etc/sysctl.conf"
#endif


/* *********导入CRL远程方法的错误返回码******* */
enum import_crl_errorcode_e {
    IMPORT_CRL_ERR_FORMAT_ERR = 0x100,   /* 格式错误，无法解析出CRL对象 */
    IMPORT_CRL_ERR_ISSUER_VERIFY_FAIL,   /* 进行CRL发行者签名校验失败 */
    IMPORT_CRL_ERR_ROOT_CERT_NOT_IMPORT, /* 对应根证书未导入的错误 */
    IMPORT_CRL_ERR_ISSUING_DATE_INVALID,
    IMPORT_CRL_ERR_NOT_SUPPORT_CRL_SIGN  /* 根证书不支持吊销列表签名 */
};

#define DELETE_CRL_ERR_CRL_NOT_EXIST  0x110   /* 无CRL证书 */

/* ***************SSL证书状态**************** */
enum ssl_cert_state_e {
    SSL_CERT_UNINIT = 0, // ssl 证书未初始化
    SSL_CERT_INITED,     // ssl 证书已初始化
    SSL_CERT_REINIT,     // ssl 证书重新初始化
};

/* **********证书过期告警对象ID列表****** */
enum CERT_MONITOR_ID_TBL {
    SYSLOG_ROOT_CERT_MONITOR_ID = 0,
    SYSLOG_CLIENT_CERT_MONITOR_ID = 1,
    LDAP_CERT_MONITOR1_1_ID = 2, // LDAP证书1_1的Certmonitor的Id为2
    LDAP_CERT_MONITOR2_1_ID = 3, // LDAP证书2_1的Certmonitor的Id为3
    LDAP_CERT_MONITOR3_1_ID = 4, // LDAP证书3_1的Certmonitor的Id为4
    LDAP_CERT_MONITOR4_1_ID = 5, // LDAP证书4_1的Certmonitor的Id为5
    LDAP_CERT_MONITOR5_1_ID = 6, // LDAP证书5_1的Certmonitor的Id为6
    LDAP_CERT_MONITOR6_1_ID = 7, // LDAP证书6_1的Certmonitor的Id为7
    SSL_CERT_MONITOR4_ID = 11, // SSL证书Certmonitor的Id从11开始
    SSL_CERT_MONITOR5_ID,
    SSL_CERT_MONITOR6_ID,
    SSL_CERT_MONITOR7_ID,
    SSL_CERT_MONITOR8_ID,
    SSL_CERT_MONITOR9_ID,
    SSL_CERT_MONITOR10_ID,
    SSL_CERT_MONITOR11_ID,
    SSL_CERT_MONITOR12_ID,
    SSL_CERT_MONITOR13_ID = 20, // SSL证书Certmonitor的Id到20结束
    ROOT_CERT_MONITOR1_ID = 21, // 根证书Certmonitor的Id从21开始
    ROOT_CERT_MONITOR2_ID,
    ROOT_CERT_MONITOR3_ID,
    ROOT_CERT_MONITOR4_ID,
    ROOT_CERT_MONITOR5_ID,
    ROOT_CERT_MONITOR6_ID,
    ROOT_CERT_MONITOR7_ID,
    ROOT_CERT_MONITOR8_ID,
    ROOT_CERT_MONITOR9_ID,
    ROOT_CERT_MONITOR10_ID,
    ROOT_CERT_MONITOR11_ID,
    ROOT_CERT_MONITOR12_ID,
    ROOT_CERT_MONITOR13_ID,
    ROOT_CERT_MONITOR14_ID,
    ROOT_CERT_MONITOR15_ID,
    ROOT_CERT_MONITOR16_ID = 36, // 根证书Certmonitor的Id到36结束
    CLIENT_CERT_MONITOR1_ID = 37, // 客户端证书Certmonitor的Id从37开始
    CLIENT_CERT_MONITOR2_ID,
    CLIENT_CERT_MONITOR3_ID,
    CLIENT_CERT_MONITOR4_ID,
    CLIENT_CERT_MONITOR5_ID,
    CLIENT_CERT_MONITOR6_ID,
    CLIENT_CERT_MONITOR7_ID,
    CLIENT_CERT_MONITOR8_ID,
    CLIENT_CERT_MONITOR9_ID,
    CLIENT_CERT_MONITOR10_ID,
    CLIENT_CERT_MONITOR11_ID,
    CLIENT_CERT_MONITOR12_ID,
    CLIENT_CERT_MONITOR13_ID,
    CLIENT_CERT_MONITOR14_ID,
    CLIENT_CERT_MONITOR15_ID,
    CLIENT_CERT_MONITOR16_ID,
    CLIENT_CERT_MONITOR17_ID = 53, // 客户端证书Certmonitor的Id到53结束
    LDAP_CERT_MONITOR1_2_ID = 54, // LDAP证书1_2~1_8Certmonitor的Id从54开始
    LDAP_CERT_MONITOR1_3_ID,
    LDAP_CERT_MONITOR1_4_ID,
    LDAP_CERT_MONITOR1_5_ID,
    LDAP_CERT_MONITOR1_6_ID,
    LDAP_CERT_MONITOR1_7_ID,
    LDAP_CERT_MONITOR1_8_ID,
    LDAP_CERT_MONITOR1_9_ID,
    LDAP_CERT_MONITOR1_10_ID = 62, // LDAP证书1_2~1_10Certmonitor的Id到62结束
    LDAP_CERT_MONITOR2_2_ID = 63, // LDAP证书2_2~2_10Certmonitor的Id从63开始
    LDAP_CERT_MONITOR2_3_ID,
    LDAP_CERT_MONITOR2_4_ID,
    LDAP_CERT_MONITOR2_5_ID,
    LDAP_CERT_MONITOR2_6_ID,
    LDAP_CERT_MONITOR2_7_ID,
    LDAP_CERT_MONITOR2_8_ID,
    LDAP_CERT_MONITOR2_9_ID,
    LDAP_CERT_MONITOR2_10_ID = 71, // LDAP证书2_2~2_10Certmonitor的Id到71结束
    LDAP_CERT_MONITOR3_2_ID = 72, // LDAP证书3_2~3_10Certmonitor的Id从72开始
    LDAP_CERT_MONITOR3_3_ID,
    LDAP_CERT_MONITOR3_4_ID,
    LDAP_CERT_MONITOR3_5_ID,
    LDAP_CERT_MONITOR3_6_ID,
    LDAP_CERT_MONITOR3_7_ID,
    LDAP_CERT_MONITOR3_8_ID,
    LDAP_CERT_MONITOR3_9_ID,
    LDAP_CERT_MONITOR3_10_ID = 80, // LDAP证书3_2~3_10Certmonitor的Id到80结束
    LDAP_CERT_MONITOR4_2_ID = 81, // LDAP证书4_2~4_10Certmonitor的Id从81开始
    LDAP_CERT_MONITOR4_3_ID,
    LDAP_CERT_MONITOR4_4_ID,
    LDAP_CERT_MONITOR4_5_ID,
    LDAP_CERT_MONITOR4_6_ID,
    LDAP_CERT_MONITOR4_7_ID,
    LDAP_CERT_MONITOR4_8_ID,
    LDAP_CERT_MONITOR4_9_ID,
    LDAP_CERT_MONITOR4_10_ID = 89, // LDAP证书4_2~4_10Certmonitor的Id到89结束
    LDAP_CERT_MONITOR5_2_ID = 90, // LDAP证书5_2~5_10Certmonitor的Id从90开始
    LDAP_CERT_MONITOR5_3_ID,
    LDAP_CERT_MONITOR5_4_ID,
    LDAP_CERT_MONITOR5_5_ID,
    LDAP_CERT_MONITOR5_6_ID,
    LDAP_CERT_MONITOR5_7_ID,
    LDAP_CERT_MONITOR5_8_ID,
    LDAP_CERT_MONITOR5_9_ID,
    LDAP_CERT_MONITOR5_10_ID = 98, // LDAP证书5_2~5_10Certmonitor的Id到98结束
    LDAP_CERT_MONITOR6_2_ID = 99, // LDAP证书6_2~6_10Certmonitor的Id从99开始
    LDAP_CERT_MONITOR6_3_ID,
    LDAP_CERT_MONITOR6_4_ID,
    LDAP_CERT_MONITOR6_5_ID,
    LDAP_CERT_MONITOR6_6_ID,
    LDAP_CERT_MONITOR6_7_ID,
    LDAP_CERT_MONITOR6_8_ID,
    LDAP_CERT_MONITOR6_9_ID,
    LDAP_CERT_MONITOR6_10_ID = 107, // LDAP证书6_2~6_10Certmonitor的Id到107结束
    BIOS_SECUREBOOT_CERT_MONITOR_ID = 108, // BIOS安全启动证书Certmonitor的Id为108
    REMOTE_HTTPS_CA_CERT_MONITOR1_ID = 109, // CA证书Certmonitor的Id从109开始
    REMOTE_HTTPS_CA_CERT_MONITOR2_ID,
    REMOTE_HTTPS_CA_CERT_MONITOR3_ID,
    REMOTE_HTTPS_CA_CERT_MONITOR4_ID,
    REMOTE_HTTPS_CA_CERT_MONITOR5_ID,
    REMOTE_HTTPS_CA_CERT_MONITOR6_ID,
    REMOTE_HTTPS_CA_CERT_MONITOR7_ID,
    REMOTE_HTTPS_CA_CERT_MONITOR8_ID = 116, // CA证书Certmonitor的Id到116结束
    REMOTE_HTTPS_CA_CRL_MONITOR1_ID = 117, // CA吊销列表Certmonitor的Id从117开始
    REMOTE_HTTPS_CA_CRL_MONITOR2_ID,
    REMOTE_HTTPS_CA_CRL_MONITOR3_ID,
    REMOTE_HTTPS_CA_CRL_MONITOR4_ID,
    REMOTE_HTTPS_CA_CRL_MONITOR5_ID,
    REMOTE_HTTPS_CA_CRL_MONITOR6_ID,
    REMOTE_HTTPS_CA_CRL_MONITOR7_ID,
    REMOTE_HTTPS_CA_CRL_MONITOR8_ID = 124, // CA吊销列表Certmonitor的Id到124结束
    DICE_CERT_MONITOR_ID = 125, // DICE证书Certmonitor的Id为125
    LDAP_CRL_MONITOR1_ID = 126, // LDAP吊销列表Certmonitor的Id从126开始
    LDAP_CRL_MONITOR2_ID,
    LDAP_CRL_MONITOR3_ID,
    LDAP_CRL_MONITOR4_ID,
    LDAP_CRL_MONITOR5_ID,
    LDAP_CRL_MONITOR6_ID = 131, // LDAP吊销列表Certmonitor的Id到131结束
    SYSLOG_ROOT_CRL_MONITOR_ID = 132, // SYSLOG吊销列表Certmonitor的Id为132
    ROOT_CRL_MONITOR1_ID = 133, // 根证书吊销列表Certmonitor的Id从133开始
    ROOT_CRL_MONITOR2_ID,
    ROOT_CRL_MONITOR3_ID,
    ROOT_CRL_MONITOR4_ID,
    ROOT_CRL_MONITOR5_ID,
    ROOT_CRL_MONITOR6_ID,
    ROOT_CRL_MONITOR7_ID,
    ROOT_CRL_MONITOR8_ID,
    ROOT_CRL_MONITOR9_ID,
    ROOT_CRL_MONITOR10_ID,
    ROOT_CRL_MONITOR11_ID,
    ROOT_CRL_MONITOR12_ID,
    ROOT_CRL_MONITOR13_ID,
    ROOT_CRL_MONITOR14_ID,
    ROOT_CRL_MONITOR15_ID,
    ROOT_CRL_MONITOR16_ID = 148, // 根证书吊销列表Certmonitor的Id到148结束
    CA_SERVER_CA_CERT_MONITOR_ID = 149, // CA服务器的CA证书Certmonitor的Id为149
    CA_SERVER_CRL_MONITOR_ID = 150,  // CA服务器的证书吊销列表Certmonitor的Id为150
    CA_SERVER_CLIENT_CERT_MONITOR_ID = 151,  // CA服务器的客户端证书Certmonitor的Id为151
    CERT_MONITOR_MAX_ID = 255 // CertMonitor最多为255个
};
/* 证书过期告警对象ID偏移量 */
#define SSL_CERT_MONITOR_OFFSET (SSL_CERT_MONITOR4_ID - 4)   // 证书ID从4开始的SSL证书才配置了证书过期检测的功能
#define ROOT_CERT_MONITOR_OFFSET (ROOT_CERT_MONITOR1_ID - 1) // 证书过期告警对象ID偏移量为对应第一个证书的ID减1
#define CLIENT_CERT_MONITOR_OFFSET (CLIENT_CERT_MONITOR1_ID - 1)
#define REMOTE_HTTPS_CA_CERT_MONITOR_OFFSET (REMOTE_HTTPS_CA_CERT_MONITOR1_ID - 1)
#define REMOTE_HTTPS_CRL_CERT_MONITOR_OFFSET (REMOTE_HTTPS_CA_CRL_MONITOR1_ID - 1)
#define CA_SERVER_CA_CERT_MONITOR_OFFSET (CA_SERVER_CA_CERT_MONITOR_ID - 1)
#define CA_SERVER_CRL_MONITOR_OFFSET (CA_SERVER_CRL_MONITOR_ID - 1)
#define CA_SERVER_CLIENT_CERT_MONITOR_OFFSET (CA_SERVER_CLIENT_CERT_MONITOR_ID - 1)
#define LDAP_CRL_CERT_MONITOR_OFFSET (LDAP_CRL_MONITOR1_ID - 1)
#define ROOT_CRL_CERT_MONITOR_OFFSET (ROOT_CRL_MONITOR1_ID - 1)

// dice证书链导出状态错误码; 同时作为进度, 大于50
enum dice_export_err_code {
    DICE_PRECHECK_FAILED = 54,
    DICE_GET_CERT_0_FAILED = 55,
    DICE_GET_CERT_1_FAILED = 56,
    DICE_GET_LAST_CERT_FAILED = 57,
    DICE_TRANSFER_FILE_FAILED = 58,
};
#define ACT_COMPLETE 0
#define ACT_FAILED 1
#define ACT_IN_PROCESS 2


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __CERTIFICATES_DEFINE_H__ */
