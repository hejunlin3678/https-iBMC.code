

#ifndef __RESOURCE_CHANNEL_H__
#define __RESOURCE_CHANNEL_H__

#include "pme/pme.h"
#include "pme_app/pme_app.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define RESOURCE_SUB_CMD_OFFSET 4
#define RESOURCE_ID_OFFSET 5
#define RESOURCE_FLAG_OFFSET 6


#define UPLOAD_OPERATE_INITIATE 0xAA
#define UPLOAD_OPERATE_STATUS 0x00


#define DOWNLOAD_OPERATE_INITIATE 0xAA
#define DOWNLOAD_OPERATE_NAME 0x55
#define DOWNLOAD_OPERATE_STATUS 0x00

#define UPLOAD_MIN_HEAD_LEN 8
#define MAX_FILE_PATH_LEN 128
#define RES_MAX_FILE_NAME_LEN 64
#define MAX_LOOKUP_TIMES 3
#define DESP_DATA_LEN 5

typedef struct tg_FILE_TRANS_INFO {
    guint8 resource_id;
    gchar src_ip[MAX_FILE_PATH_LEN];
    gchar src_path[MAX_FILE_PATH_LEN];
    gchar dest_path[MAX_FILE_PATH_LEN];
    gchar request_msg[IPMI_MSG_MAX_PAYLOAD_LEN + IPMI_MSG_HEAD_LEN];
} FILE_TRANS_INFO, *PFILE_TRANS_INFO;


#define GET_CONFIG_FILE_STATUS 0x01
#define GET_SHA256_VALUE 0X02
#define SHA256_CHECKSUM_LENGTH 32

typedef struct config_file_info {
    guint8 resource_id;
    guint8 status;
    guint8 icv_value_sha256[SHA256_CHECKSUM_LENGTH];
} STRUCT_CONFIG_FILE_INFO;

#define DEBUG_INFO_FILE_NAME_LEN 64

#define START_COMMAND 0XAA
#define GET_STATUS 0X00
#define COMMAND_IN_PROGRESS 0X00
#define COMMAND_COMPLETE 0X01
#define COMMAND_FAIL 0X02

#define RESOURCE_BMC_DEBUG_INFO_PATH "/tmp/"
#define RESOURCE_SOL_LOG_PATH RESOURCE_BMC_DEBUG_INFO_PATH
#define RESOURCE_SOL_LOG_FILE "systemcom.tar"
#define RESOURCE_BLACKBOX_PATH RESOURCE_BMC_DEBUG_INFO_PATH
#define RESOURCE_BLACKBOX_FILE "blackbox.tar"
#define RESOURCE_BIOS_PROFILE_PATH RESOURCE_BMC_DEBUG_INFO_PATH
#define RESOURCE_BIOS_CLP_CONFIG_FILE "ClpConfig0.ini"
#define RESOURCE_BIOS_CLP_RESPONSE_FILE "ClpResponse0.ini"

#define RESOURCE_POWER_VIEW_PATH "/opt/pme/pram/"
#define RESOURCE_POWER_VIEW_FILE "powerview.txt" // "ps_smm_view.dat"

// 文件下载完成码定义
#define COMP_FILE_NOT_EXIT 0x01
#define COMP_FILE_TRANS_FAIL 0x02
#define COMP_FILE_TRANS_TIMEOUT 0x03

#define SMM_INNER_ETHERNET 1
#define NET_SEPERATE_CHAR "."

extern GMutex g_clpconfig_mutex;
extern guint8 g_is_clpconfig_from_bt;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 
