/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MCTP NCSI消息相关定义
 * Author: huanghan1@huawei.com
 * Create: 2021-02-05
 */

#ifndef MCTP_NCSI_MSG_H
#define MCTP_NCSI_MSG_H


#define MCTP_NCSI_OEM_MANU_ID_MELLANOX           0x8119
#define MCTP_NCSI_OEM_MANU_ID_BJ                 0xE789
#define MCTP_NCSI_OEM_MANU_ID_INTEL              0x157
#define MCTP_PLDM_OEM_MANU_ID_EMULEX             0x0000113D
#define MCTP_PLDM_OEM_MANU_ID_QLOGIC             0x00000F21
#define MCTP_NCSI_OEM_MANU_ID_WX                 0x57584B4A

#define BJ_MANUFACTURE_ID                        0x89E70000

// 1822
#define MCTP_NCSI_OEM_MANU_ID_HUAWEI							0x000007DB
#define MCTP_NCSI_INVAILD_DATA									0x7ffd
#define MCTP_NCSI_NOT_PRESENT								    0x7ffe
#define MCTP_NCSI_READ_TEMP_FAIL								0x7fff
#define MCTP_NCSI_ERROR_CODE_MAX_NUM   							11
#define MCTP_NCSI_GET_RSP_INVAILD								0xff
#define MCTP_NCSI_MAX_STRING_LENGTH                             64
#define MCTP_NCSI_GET_MAC_ADDR_REQ_SUB_CMD						0x00
#define MCTP_NCSI_GET_DEFAULT_MAC_ADDR_REQ_SUB_CMD				0x04
#define MCTP_NCSI_GET_CONTROLLER_TEMP_REQ_SUM_CMD				0x0A
#define MCTP_NCSI_GET_OPTICAL_TEMP_REQ_SUM_CMD					0x0B
#define MCTP_NCSI_GET_SDI5_0_LOG                                0x12
#define MCTP_NCSI_GET_DRIVER_NAME_LEN							65
#define MCTP_NCSI_GET_DRIVER_VERSION_LEN						16
#define MCTP_NCSI_PART_NUM_LEN									17
#define MCTP_NCSI_DCBX_FIELD_LEN								8
#define MCTP_NCSI_OPTICAL_FIELD_LEN								16

#define MCTP_NCSI_ERROR_CODE_MAX_COUNT							11
#define MCTP_NCSI_MAX_IP_ADDR_COUNT								8
#define MCTP_NCSI_MAX_IP_ADDR_LEN								16

#define MCTP_NCSI_PACKET_MIN_LEN           64
#define MCTP_NCSI_PACKET_HEADER_LEN         16

// 1822 LLDP TX功能使能状态
enum {
    PORT_LLDP_TX_DISABLE = 0,
    PORT_LLDP_TX_ENABLE = 1,
    PORT_LLDP_TX_ENABLE_INVALID = 2
};

#pragma pack(1)
// NCSI common struct definition
typedef struct tag_mctp_ncsi_ctl_packet_header {
    guint8 mc_id;
    guint8 header_revision;
    guint8 reserved;
    guint8 iid;
    guint8 packet_type;
#ifdef BD_BIG_ENDIAN
    guint8 package_id : 3;
    guint8 channel_id : 5;
#else
    guint8 channel_id : 5;
    guint8 package_id : 3;
#endif
#ifdef BD_BIG_ENDIAN
    guint8 reserved1 : 4;
    guint8 payload_len_hi : 4;
#else
    guint8 payload_len_hi : 4;
    guint8 reserved1 : 4;
#endif
    guint8 payload_len_lo;
    gint32 reserved2;
    gint32 reserved3;
} MCTP_NCSI_CTL_PACKET_HEADER;

typedef struct tag_mctp_ncsi_packet {
    MCTP_NCSI_CTL_PACKET_HEADER packet_head;
    guint8     payload[MCTP_NCSI_PACKET_MIN_LEN * 4 - MCTP_NCSI_PACKET_HEADER_LEN - 4];
} MCTP_NCSI_PACKET;

typedef struct tg_mctp_ncsi_link_status_field_rsp {
#ifdef BD_BIG_ENDIAN
    guint32 reserved3 : 8;
    guint32 reserved2 : 2;
    guint32 oem_link_speed : 1;
    guint32 serdes_link : 1;
    guint32 link_partner8 : 2;
    guint32 rx_flow_control : 1;
    guint32 tx_flow_control : 1;
    guint32 link_partner7 : 1;
    guint32 link_partner6 : 1;
    guint32 link_partner5 : 1;
    guint32 link_partner4 : 1;
    guint32 link_partner3 : 1;
    guint32 link_partner2 : 1;
    guint32 link_partner1 : 1;
    guint32 channel_available : 1;    /* NCSI通道可用标志 */

    guint32 parallel_detection : 1;
    guint32 negotiate_complete : 1;
    guint32 negotiate_flag : 1;
    guint32 speed_duplex : 4;
    guint32 link_flag : 1;
#else
    guint32 reserved3 : 8;

    guint32 tx_flow_control : 1;
    guint32 rx_flow_control : 1;
    guint32 link_partner8 : 2;
    guint32 serdes_link : 1;
    guint32 oem_link_speed : 1;
    guint32 reserved2 : 2;

    guint32 channel_available : 1;    /* NCSI通道可用标志 */
    guint32 link_partner1 : 1;
    guint32 link_partner2 : 1;
    guint32 link_partner3 : 1;
    guint32 link_partner4 : 1;
    guint32 link_partner5 : 1;
    guint32 link_partner6 : 1;
    guint32 link_partner7 : 1;

    guint32 link_flag : 1;
    guint32 speed_duplex : 4;
    guint32 negotiate_flag : 1;
    guint32 negotiate_complete : 1;
    guint32 parallel_detection : 1;
#endif
}MCTP_NCSI_LINK_STATUS_FIELD_RSP;

typedef struct tag_ctrl_packet_stat {
    guint last_cnt_ms;
    guint last_cnt_ls;
    unsigned long long recv_byte;
    unsigned long long trans_byte;
    unsigned long long total_unicast_packets_received;
    unsigned long long total_multicast_packets_received;
    unsigned long long total_broadcast_packets_received;
    unsigned long long total_unicast_packets_transmitted;
    unsigned long long total_multicast_packets_transmitted;
    unsigned long long total_broadcast_packets_transmitted;
    guint fcs_receive_errors;
    guint alignment_errors;
    guint false_carrier_detections;
    guint runt_packets_received;
    guint jabber_packets_received;
    guint pause_xon_frames_received;
    guint pause_xoff_frames_received;
    guint pause_xon_frames_transmitted;
    guint pause_xoff_frames_transmitted;
    guint single_collision_transmit_frames;
    guint multiple_collision_transmit_frames;
    guint late_collision_frames;
    guint excessive_collision_frames;
    guint control_frames_received;
    guint frames_received_64_byte;
    guint frames_received_65_127_byte;
    guint frames_received_128_255_byte;
    guint frames_received_256_511_byte;
    guint frames_received_512_1023_byte;
    guint frames_received_1024_1522_byte;
    guint frames_received_1523_9022_byte;
    guint frames_transmitted_64_byte;
    guint frames_transmitted_65_127_byte;
    guint frames_transmitted_128_255_byte;
    guint frames_transmitted_256_511_byte;
    guint frames_transmitted_512_1023_byte;
    guint frames_transmitted_1024_1522_byte;
    guint frames_transmitted_1523_9022_byte;
    unsigned long long valid_bytes_received;
    guint error_runt_packets_received;
    guint error_jabber_packets_received;
}CTRL_PACKET_STAT;

typedef struct tag_mctp_ncsi_get_packet_statis_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    CTRL_PACKET_STAT packet_statisitc;
    guint32 check_sum;
}MCTP_NCSI_GET_PACKET_STATIS_RSP_PAYLOAD;

typedef struct tag_mctp_ncsi_get_info_public_field {
    guint32 manufacture_id;
    guint8 command_rev;
    guint8 command_id;
    guint8 sub_command;
    guint8 reserved;
} MCTP_NCSI_GET_INTO_PUBLIC_FIELD;

typedef struct tag_mctp_ncsi_get_info_req_cmmd {
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint32 check_sum;
} MCTP_NCSI_GET_INTO_REQ_CMMD_HW;

typedef struct tag_mctp_ncsi_get_sdi_log_req_cmmd {
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint32 offset;
    guint32 length;
    guint32 check_sum;
} MCTP_NCSI_GET_SDI_LOG_REQ_CMMD_HW;

typedef struct tg_mctp_ncsi_get_sdi_rsp_rsp_payload {
    guint16 rsp_code;
    guint16 reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8* data;
} MCTP_NCSI_GET_SDI_LOG_RSP_PAYLOAD;

typedef struct _tg_ncsi_mctp_get_sdi_log_ {
    MCTP_NCSI_CTL_PACKET_HEADER packet_head;
    MCTP_NCSI_GET_SDI_LOG_RSP_PAYLOAD payload;
} MCTP_NCSI_GET_SDI_LOG_RSP;

typedef struct tag_mctp_ncsi_get_optical_info_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8 part_num[MCTP_NCSI_OPTICAL_FIELD_LEN];
    guint8 vendor[MCTP_NCSI_OPTICAL_FIELD_LEN];
    guint8 serial_num[MCTP_NCSI_OPTICAL_FIELD_LEN];
    guint8 identifier;
    guint8 type;
    guint8 connector;
    guint8 reserved_2;
    guint16 transfer_distance;
    guint16 wave_length;
    guint16 working_temp;
    guint16 working_voltage;
    guint16 working_tx_bias;
    guint16 working_tx_power;
    guint16 working_rx_power;
    guint16 warning_low_temp_threshold;
    guint16 warning_high_temp_threshold;
    guint16 warning_tx_power_threshold;
    guint16 warning_rx_power_threshold;
    guint16 alarm_low_temp_threshold;
    guint16 alarm_high_temp_threshold;
    guint16 alarm_tx_power_threshold;
    guint16 alarm_rx_power_threshold;
    guint8 rx_los_state;
    guint8 tx_full_state;
    guint8 reserved_3[24];
    guint32 check_sum;
} MCTP_NCSI_GET_OPTICAL_INFO_RSP_PAYLOAD;

// DCBX status
typedef struct tag_mctp_ncsi_get_dcbx_status_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8 cos2_up[MCTP_NCSI_DCBX_FIELD_LEN];
    guint8 up_pgid[MCTP_NCSI_DCBX_FIELD_LEN];
    guint8 pgpct[MCTP_NCSI_DCBX_FIELD_LEN];
    guint8 strict[MCTP_NCSI_DCBX_FIELD_LEN];
    guint8 pfcmap;
    gchar reserved2[3];
    guint32 check_sum;
} MCTP_NCSI_GET_DCBX_STATUS_RSP_PAYLOAD;

// GET ERROR CODE
typedef struct tag_mctp_ncsi_get_error_code_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8 health_status;
    guint8 error_count;
    guint16 error_code[MCTP_NCSI_ERROR_CODE_MAX_COUNT];
    guint32 check_sum;
} MCTP_NCSI_GET_ERROR_CODE_RSP_PAYLOAD;

// GET IP ADDR
typedef struct tag_mctp_ncsi_ip_addr_info {
    guint8 ip_addr[MCTP_NCSI_MAX_IP_ADDR_LEN];
    guint8 prefix;
    guint8 reserved[3];
}MCTP_NCSI_IP_ADDR_INFO;

typedef struct tag_mctp_ncsi_get_ip_addr_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint16 ip_addr_count;
    guint8 bitmap;
    guint8 reserved_1;
    MCTP_NCSI_IP_ADDR_INFO ip_addr_array[MCTP_NCSI_MAX_IP_ADDR_COUNT];
    guint32 check_sum;
} MCTP_NCSI_GET_IP_ADDR_RSP_PAYLOAD;

typedef struct tag_mctp_ncsi_get_lldp_capability_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8 lldp_capability;
    guint8 reserved[3];
    guint32 check_sum;
}MCTP_NCSI_GET_LLDP_CAPABILITY_RSP_PAYLOAD;

typedef struct tag_mctp_ncsi_enable_lldp_capability_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint32 check_sum;
}MCTP_NCSI_ENABLE_LLDP_CAPABILITY_RSP_PAYLOAD;

typedef struct tg_mctp_ncsi_set_lldp_nb_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8 reserved[3];
    guint8 lldp_nb;
    guint32 check_sum;
}MCTP_NCSI_SET_LLDP_NB_RSP_PAYLOAD;

typedef struct tg_mctp_ncsi_get_lldp_nb_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint8 reserved[3];
    guint8 lldp_nb;
    guint32 check_sum;
} MCTP_NCSI_GET_LLDP_NB_RSP_PAYLOAD;

typedef struct tg_mctp_ncsi_enable_lldp_nb_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint32 check_sum;
} MCTP_NCSI_ENABLE_LLDP_NB_RSP_PAYLOAD;

typedef struct tg_mctp_ncsi_reset_nic_rsp_payload {
    gushort rsp_code;
    gushort reason_code;
    MCTP_NCSI_GET_INTO_PUBLIC_FIELD public_cmd;
    guint32 check_sum;
} MCTP_NCSI_RESET_NIC_RSP_PAYLOAD;
#pragma pack()


gint32 mctp_ncsi_get_link_status(OBJ_HANDLE obj_handle, MCTP_NCSI_LINK_STATUS_FIELD_RSP* link_status_info);
gint32 mctp_ncsi_get_firm_ver(OBJ_HANDLE obj_handle, gchar* firm_ver, guint8 firm_len, guint16 *vid_did,
    guint16 vid_did_len);
gint32 mctp_ncsi_get_packet_statis(OBJ_HANDLE obj_handle, MCTP_NCSI_GET_PACKET_STATIS_RSP_PAYLOAD* pst_packet_rsp);
gint32 mctp_ncsi_clear_init_state(OBJ_HANDLE obj_handle);
gint32 mctp_ncsi_common_debug_cmd(OBJ_HANDLE obj_handle, guchar pack_type,
                                  guchar* payload_data, guint32 data_len,
                                  guchar* recv_data, guint32 recv_data_len);
gint32 mctp_ncsi_get_bdf_info_hw(OBJ_HANDLE obj_handle, guint8* bus, guint8* device, guint8* function);
gint32 mctp_ncsi_get_pcie_interface_abiliy_hw(OBJ_HANDLE obj_handle, guint8* link_width, guint8* link_speed);
gint32 mctp_ncsi_get_pcie_interface_status_hw(OBJ_HANDLE obj_handle, guint8* link_width, guint8* link_speed);
gint32 mctp_ncsi_get_temp_hw(OBJ_HANDLE obj_handle, guint8 sub_cmd, guint16* temperature);
gint32 mctp_ncsi_get_sdi_card_log_hw(OBJ_HANDLE port_handle, guint8 log_type, guint32 offset,
    gchar* buffer, guint32 size);
gint32 mctp_ncsi_get_dcbx_status_hw(OBJ_HANDLE obj_handle, MCTP_NCSI_GET_DCBX_STATUS_RSP_PAYLOAD* pst_packet_rsp);
gint32 mctp_ncsi_get_optical_module_info_hw(OBJ_HANDLE obj_handle,
                                            MCTP_NCSI_GET_OPTICAL_INFO_RSP_PAYLOAD* pst_packet_rsp);
gint32 mctp_ncsi_get_error_code_hw(OBJ_HANDLE obj_handle, guint8* health_status, guint8* error_num,
                                   guint16* error_code, guint8 len);
gint32 mctp_ncsi_get_mac_addr(OBJ_HANDLE obj_handle, gchar* mac_addr, guint8 mac_len,
                              guint8 sub_cmd, guint16* mac_addr_num);

gint32 mctp_ncsi_set_mac_addr(OBJ_HANDLE obj_handle, gchar* mac_addr, guint32 mac_len);
gint32 mctp_ncsi_get_temp_intel(OBJ_HANDLE obj_handle, guint16* temperature);

gint32 mctp_ncsi_get_lldp_capability_hw(OBJ_HANDLE obj_handle, guint8* lldp_capability);
gint32 mctp_ncsi_enable_lldp_capability_hw(OBJ_HANDLE obj_handle);

gint32 mctp_ncsi_set_lldp_tx_capability_hw(OBJ_HANDLE obj_handle, gboolean status);
gint32 mctp_ncsi_get_lldp_tx_capability_hw(OBJ_HANDLE obj_handle, guint8 *lldp_capability);
gint32 mctp_ncsi_get_lldp_nb_mellanox(OBJ_HANDLE obj_handle, guint8 *lldp_nb);
gint32 mctp_ncsi_get_temp_mlx(OBJ_HANDLE obj_handle, guint16* temperature);
gint32 mctp_ncsi_set_lldp_nb_mellanox(OBJ_HANDLE obj_handle, guint8 lldp_nb);
gint32 mctp_ncsi_reset_nic_mellanox(OBJ_HANDLE obj_handle, guint8 mode);
gint32 mctp_ncsi_get_default_mac_addr_mlnx(OBJ_HANDLE obj_handle, gchar *mac_addr, guint8 mac_len);
gint32 mctp_ncsi_get_sn_wx(OBJ_HANDLE obj_handle, gchar *sn, guint8 sn_len);
gint32 mctp_ncsi_get_temp_wx(OBJ_HANDLE obj_handle, guint16 *temperature);
gint32 mctp_ncsi_get_lldp_wx(OBJ_HANDLE obj_handle, guint8 *lldp_nb);
gint32 mctp_ncsi_set_lldp_wx(OBJ_HANDLE obj_handle, guint8 lldp_nb);
#endif
