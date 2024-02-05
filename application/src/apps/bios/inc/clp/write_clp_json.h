
#ifndef WRITE_CLP_JSON_H
#define WRITE_CLP_JSON_H

#include "pme_app/pme_app.h"
#include "bios_ipmi.h"
#include "pme_app/xmlparser/xmlparser.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define LINE_MAX_LEN 0xFF
#define MAX_LEN_32 32
#define MAX_LEN_64 64
#define MAX_LEN_128 128

#define _check_pf_vlan_trunkrange _check_pfs_vlan_trunkrange
#define PORT_PF_MAX 4
#define TARGET_MAX_NUM 8
// 实测网卡返回给BMC的QinqVlanTrunkRange最多为127字节，超过127字节的部分会被截断，限制了BMC传入QinqVlanTrunkRange最多127字节
#define   QINQ_VLAN_TRUNK_RANGE_MAX_LEN     127
// QinqVlanTrunkRange最多127字节，加上"set pf0 QinqVlanTrunkRange="27字节，总共最长为127+27=155字节，留点余量到160
#define   PF_CONFIG_MAX_LEN                 160
 
typedef struct pf_config {
    guchar pf_id;
    gchar Protocol[MAX_LEN_32 + 1];
    guchar MinBandwidth;
    guchar MaxBandwidth;
    gchar PermanentAddress[MAX_LEN_32 + 1];
    gchar iScsiAddress[MAX_LEN_32 + 1];
    gchar FCoEFipMACAddress[MAX_LEN_32 + 1];
    gchar FCoEWWNN[MAX_LEN_32 + 1];
    gchar FCoEWWPN[MAX_LEN_32 + 1];
    gchar QinqVlanMode[MAX_LEN_32 + 1];
    gint16 QinqVlanId;
    guchar QinqVlanPriority;
    gchar QinqVlanTrunkRange[QINQ_VLAN_TRUNK_RANGE_MAX_LEN + 1];
} PF_CONFIG;

typedef struct fcoe_boot_target {
    guchar target_id;
    guchar TargetEnable;
    gchar FCoEWWPN[MAX_LEN_32 + 1];
    gint16 bootlun;
} FCOE_BOOT_TARGET;

typedef struct tag_clp_set {
    guint16 board_id;
    gint32 clp_flag;
} CLP_SET_S;

typedef struct port_config {
    guchar SRIOVEnabled;
    guchar BootEnable;
    gchar BootProtocol[MAX_LEN_32 + 1];
    guchar BootToTarget;
    PF_CONFIG NparPF[PORT_PF_MAX];
    FCOE_BOOT_TARGET FCoEBootTarget[TARGET_MAX_NUM];
    gint16 VlanId;
    guchar SANBootEnable;
    gchar LinkConfig[MAX_LEN_32 + 1];
    gchar SerDesConfig[MAX_LEN_32 + 1];
    guchar Band_sum;
    guchar FCoE_count;
    guchar iSCSI_count;
} PORT_CONFIG;

typedef struct net_chip_config {
    gchar MultifunctionMode[MAX_LEN_32];
} NET_CHIP_CONFIG;

typedef struct card_info {
    guint32 vid;
    guint32 did;
} CARD_INFO;

typedef struct head_info {
    gchar devicename[MAX_PROPERTY_VALUE_LEN];
    guchar chipid;
    guchar portid;
} HEAD_INFO;

typedef struct clp_response {
    guint32 board_id;
    CARD_INFO cardinfo;
    HEAD_INFO headinfo;
    NET_CHIP_CONFIG chipconfig;
    PORT_CONFIG portconfig;
} CLP_RESPONSE_UNIT;

gint32 _json_set_and_rules_check(const gchar *devicename, const gchar *config_data, json_object **error_info,
    GSList **config_list, CLP_SET_S clp_set);

gint32 _set_boottotargets_config(json_object *targets_json, CLP_RESPONSE_UNIT *config_unit_p, json_object **error_info,
    const gchar *targets_name, gint32 port_index);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif 