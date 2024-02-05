/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2020. All rights reserved.
 * Description: 文件传输相关的定义。
 * Author: h00282621
 * Create: 2020-3-10
 * Notes: 用于跨模块使用的相关的定义。
 */

#ifndef __FILE_TRANS_DEFINE_H__
#define __FILE_TRANS_DEFINE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define FILE_CATEGORY_SERIAL_PORT_DATA "serial port data"
#define FILE_CATEGORY_BLACKBOX_DATA "blackbox data"
#define FILE_CATEGORY_OPERATE_LOG "operate log"
#define FILE_CATEGORY_RUN_LOG "run log"
#define FILE_CATEGORY_SECURITY_LOG "security log"
#define FILE_CATEGORY_POWEROFF_VIDEO "poweroff video"
#define FILE_CATEGORY_OSRESET_VIDEO "osreset video"
#define FILE_CATEGORY_CATERROR_VIDEO "caterror video"
#define FILE_CATEGORY_SELLOG "system event log"
#define FILE_CATEGORY_POWER_HISTORY_DATA "power history data"
#define FILE_CATEGORY_NPU_LOG "NPU log"

#define FILE_URL_HTTPS_HEAD "https:"
#define FILE_URL_SFTP_HEAD "sftp:"
#define FILE_URL_NFS_HEAD "nfs:"
#define FILE_URL_CIFS_HEAD "cifs:"
#define FILE_URL_SCP_HEAD "scp:"

#define URI_TYPE_LOCAL 0
#define URI_TYPE_REMOTE 1

#define FILE_TYPE_LOCAL 0
#define FILE_TYPE_URI 1
#define FILE_TYPE_TEXT 2

#define INVALID_FILE_PATH 0x01

#define CLIENT_IP_MAX_LEN 64
#define INTERFACE_MAX_LEN 64

#define FILE_ID_DUMP 0x3
#define FILE_ID_SOL 0x6
#define FILE_ID_BLACKBOX 0x7
#define FILE_ID_SSH 0x0d
#define FILE_ID_CERTIFICATE 0x0e
#define FILE_ID_CONFIG 0x0f
#define FILE_ID_CONFIG_EXPORT_IMPORT 0x0F // 与上一行等价
#define FILE_ID_CSR 0x10
#define FILE_ID_CLIENT_CER 0x14

#define FILE_ID_SMM_UPGRADE 0x22
#define FILE_ID_SEL_PACKET_TAR 0x23
#define FILE_ID_HISTORY_DATA_SCV 0x24
#define FILE_ID_OPERATELOG 0x25
#define FILE_ID_RUNLOG 0x26
#define FILE_ID_SECURITYLOG 0x27
#define FILE_ID_POWEROFF 0x28
#define FILE_ID_OSRESET 0x29
#define FILE_ID_CATERROR 0x2a
#define FILE_ID_LICENSE 0x2b

#ifdef ARM64_HI1711_ENABLED
#define FILE_ID_DICE_CSR 0x2c
#define FILE_ID_DICE_CERT0 0x2d
#define FILE_ID_DICE_CERT1 0x2e
#define FILE_ID_DICE_CERT2 0x2f
#endif

#define FILE_ID_NPU_LOG 0x35

#define CODE_CONVERT_USERPSWD 0
#define CODE_CONVERT_PATH 1
#define MOUNT_LOCAL_FILEPATH "/dev/shm/share"
#define MOUNT_PATH_MODE 0750

#define MAX_URL_LEN 1024  // 网络地址字符串最大长度
#define MAX_PORTNUM_LEN 5 // 网络地址中的端口信息最大长度，0-65535,最多5个字符

#define UPLOADING_FILE 4
#define UPLOAD_FILE_OK 5
#define UPLOAD_FILE_ERR 6

#define UNKNOWN_ERROR -1
#define DOWNLOAD_UNAVAILABLE -101
#define FILENAME_TOO_LONG -102
#define FILELEN_OUTOF_MAX -103
#define SET_UPDATEFLAG_ERROR -104
#define CURL_RUN_ERROR -105
#define UPLOAD_UNAVAILABLE -106
#define UPLOAD_FILE_UNAVAILABLE -107
#define TRANSFILE_UNSUPPORTED_PROTOCOL -108
#define TRANSFILE_URL_FORMAT_ERR -109
#define UMS_BUSY_ERROR                          -110
#define MOUNT_FILE_UNAVAILABLE                  -111
#define CMS_AUTH_ERROR                          -112
#define IMAGEURI_IS_ILLEGAL                     -113
#define UMS_TRANSFER_TIMEOUT                    -114
#define SP_IS_BUSY                              -115
#define UPGRADE_SP_ERROR                        -116
#define FIND_CLASS_ERROR                        -117
#define MOUNT_UMS_UNAVAILABLE                   -118
#define SP_OF_X86_NEEDED                        -119
#define SP_OF_ARM_NEEDED                        -120
#define IN_UPGRADE_PROGRESS -121
#define CURL_PEER_SSL_VERIFY_FAILED -122
#define NO_PERMISSION_TO_OPERATE_FILE -123
#define IBMA_UPGRADE_INVALID_PACAKE   -124
#define IBMA_OF_ARM_NEEDED -125
#define IBMA_OF_X86_NEEDED -126
#define UMS_ERR_EXIT_UPGRADE_TASK -127
typedef struct tag_url_code_s {
    gchar c;
    gchar *s;
} URL_CODE_S;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FILE_TRANS_DEFINE_H__ */
