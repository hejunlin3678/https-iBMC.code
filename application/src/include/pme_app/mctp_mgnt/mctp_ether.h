/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MCTP以太报文消息相关定义
 * Author: huanghan1@huawei.com
 * Create: 2021-02-05
 */

#ifndef MCTP_ETHER_MSG_H
#define MCTP_ETHER_MSG_H

#define MCTP_ETHER_PACKET_HEADER_LEN          12

/* TLV type from section 8.4.1 of IEEE 802.1AB 9~126 reversed */
#define END_OF_LLDPDU_TLV       0 /* MANDATORY */
#define CHASSIS_ID_TLV          1 /* MANDATORY */
#define PORT_ID_TLV             2 /* MANDATORY */
#define TIME_TO_LIVE_TLV        3 /* MANDATORY */
#define PORT_DESCRIPTION_TLV    4 /* OPTIONAL  */
#define SYSTEM_NAME_TLV         5 /* OPTIONAL  */
#define SYSTEM_DESCRIPTION_TLV  6 /* OPTIONAL  */
#define SYSTEM_CAPABILITIES_TLV 7 /* OPTIONAL  */
#define MANAGEMENT_ADDRESS_TLV  8 /* OPTIONAL  */
#define ORG_SPECIFIC_TLV        127 /* OPTIONAL */

/* chassis ID subtype section 8.5.2.2 of IEEE 802.1AB 8~255 reversed */
#define CHASSIS_ID_REVERSED          0
#define CHASSIS_ID_CHASSIS_COMPONENT 1
#define CHASSIS_ID_INTERFACE_ALIAS   2
#define CHASSIS_ID_PORT_COMPONENT    3
#define CHASSIS_ID_MAC_ADDRESS       4
#define CHASSIS_ID_NETWORK_ADDRESS   5
#define CHASSIS_ID_INTERFACE_NAME    6
#define CHASSIS_ID_LOCALLY_ASSIGNED  7

/* port ID subtype section8.5.3.2 802.1AB 8~255 reversed */
#define PORT_ID_REVERSED         0
#define PORT_ID_INTERFACE_ALIAS  1
#define PORT_ID_PORT_COMPONENT   2
#define PORT_ID_MAC_ADDRESS      3
#define PORT_ID_NETWORK_ADDRESS  4
#define PORT_ID_INTERFACE_NAME   5
#define PORT_ID_AGENT_CIRCUIT_ID 6
#define PORT_ID_LOCALLY_ASSIGNED 7

/* IEEE802.1 org TLV Define */
#define IEEE_SUBTYPE_PORT_VLAN_ID               1
#define IEEE_SUBTYPE_PORT_AND_PROTOCOL_VLAN_ID  2
#define IEEE_SUBTYPE_VLAN_NAME                  3
#define IEEE_SUBTYPE_PROTOCOL_IDENTITY          4
#define IEEE_SUBTYPE_VID_USAGE_DIGEST           5
#define IEEE_SUBTYPE_MANAGEMENT_VID             6
#define IEEE_SUBTYPE_LINK_AGGREGATION           7

// LLDP控制报文通过NCSI over MCTP发送给网卡， LLDP数据报文通过以Ether over MCTP传给BMC
#define MCTP_LLDP_ETHERTYPE         0x88CC

#define LLDP_MAX_TLV_NUM            64
#define LLDP_BEGIN_RESERVED_TLV     9
#define LLDP_END_RESERVED_TLV       126
#define LLDP_MAX_INFO_LEN           512

#define LLDP_NOT_SUPPORT 0
#define LLDP_SUPPORT 1

#define PORT_LLDP_ENABLE    1
#define PORT_LLDP_DISABLE   0

#define MCTP_ETHER_RESP_SUB_CMD      0
#define MCTP_ETHER_RESP_CMD_TYPE     0
#define MCTP_ETHER_MAX_PAYLOAD_LEN   2048
#define MCTP_ETHER_MAX_PACKET_SIZE (MCTP_ETHER_MAX_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)

#define TLV_TTL_INDEX           2
#define TLV_INFO_START_INDEX    1

#define MCTP_LLDP_TTL_MAX_VAL        0xFFFF

#pragma pack(1)
typedef struct tag_mctp_ether_get_info_public_field {
    guint32 manufacture_id;
    guint8 command_type;
    guint8 sub_command;
    guint8 reserved;
    guint8 port_id;
}MCTP_ETHER_GET_INFO_PUBLIC_FIELD;
typedef struct tag_mctp_ether_ctl_packet_header {
    MCTP_ETHER_GET_INFO_PUBLIC_FIELD eth_public_cmd;
    guint8 dest_addr[6];
    guint8 src_addr[6];
    guint16 ether_type;
}MCTP_ETHER_CTL_PACKET_HEADER;

typedef struct tag_lldp_tlv {
    guint8 type;
    guint16 length;
    guint8 info_string[LLDP_MAX_INFO_LEN];
}LLDP_TLV_S;

typedef struct tg_mctp_ether_packet {
    MCTP_ETHER_CTL_PACKET_HEADER packet_head;
    guint8 lldp_payload[0]; // 表示LLDP中TLV数据集合
}MCTP_ETHER_PACKET;

#define LLDP_PAYLOAD_MAX_BUF_LEN    (MCTP_ETHER_MAX_PACKET_SIZE - sizeof(MCTP_ETHER_PACKET))

typedef struct mctp_lldp_port {
    guint16 port_lldp_ttl;
    TIMERID port_ptid;
}MCTP_LLDP_PORT;
#pragma pack()

void ethernet_mctp_init(void);
gint32 mctp_ether_recv_lldp(guint8 card_eid, guint8* port_id, LLDP_TLV_S* lldp_port_info_array,
                            guint32 port_info_len, guint16* port_info_num);

gint32 parse_lldp_tlv(guint8 *frame, guint32 frame_max_len, LLDP_TLV_S *lldp_port_info_array,
    guint32 port_info_array_size, guint16 *port_info_num);

gint32 delete_ttl_hash_item(OBJ_HANDLE busy_eth_obj);
#endif

