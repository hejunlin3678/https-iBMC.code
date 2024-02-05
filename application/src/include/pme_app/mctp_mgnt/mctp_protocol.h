/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2021. All rights reserved.
 * Description: MCTP协议层定义
 * History: 2021-02-05 huanghan1@huawei.com 优化代码结构，删掉不使用的定义
 */

#ifndef _MCTP_PROTOCOL_H_
#define _MCTP_PROTOCOL_H_

#ifndef LITTLE_ENDIAN
 #define LITTLE_ENDIAN
#endif

/* MCTP Physical Medium Idendifiers, refers to DSP0239 of DMTF
refers to https://www.dmtf.org/sites/default/files/standards/documents/DSP0239_1.6.0.pdf
0x00  Unspecified
0x01  SMBus 2.0 100 kHz compatible
0x02  SMBus 2.0 + I2C 100 kHz compatible
0x03  I2C 100 kHz compatible (Standard-mode)
0x04  I2C 400 kHz compatible (Fast-mode)
0x05  I2C 1 MHz compatible (Fast-mode Plus)
0x06  I2C 3.4 MHz compatible (High-speed mode)
0x07  Reserved
0x08  PCIe 1.1 compatible
0x09  PCIe 2.0 compatible
0x0A  PCIe 2.1 compatible
0x0B  PCIe 3.0 compatible
0x0C:0x0E  Reserved
0x0F  PCI compatible (PCI 1.0,2.0,2.1,2.2,2.3,3.0,PCI-X 1.0, PCI-X 2.0)
0x10  USB 1.1 compatible
0x11  USB 2.0 compatible
0x12  USB 3.0 compatible
0x13:0x17  Reserved
0x18  NC-SI over RBT (A physical interface based on RMII as defined in DSP0222)
0x20  KCS1 / Legacy (Fixed Address Decoding)
0x21  KCS1 / PCI (Base Class 0xC0 Subclass 0x01)
0x22  Serial Host2 / Legacy (Fixed Address Decoding)
0x23  Serial Host2 / PCI (Base Class 0x07 Subclass 0x00)
0x24  Asynchronous Serial3 (Between MCs and IMDs)
*/
#define PHY_MEDIUM_UNSPECIFIED                     0x00
#define PHY_MEDIUM_SMBUS_2_0_100_KHZ               0x01
#define PHY_MEDIUM_SMBUS_2_0_I2C_100_KHZ           0x02
#define PHY_MEDIUM_I2C_100_KHZ                     0x03
#define PHY_MEDIUM_I2C_400_KHZ                     0x04
#define PHY_MEDIUM_I2C_1_MHZ                       0x05
#define PHY_MEDIUM_I2C_3_4_MHZ                     0x06
#define PHY_MEDIUM_PCIE_1_1                        0x08
#define PHY_MEDIUM_PCIE_2_0                        0x09
#define PHY_MEDIUM_PCIE_2_1                        0x0A
#define PHY_MEDIUM_PCIE_3_0                        0x0B
#define PHY_MEDIUM_PCI                             0x0F
#define PHY_MEDIUM_USB_1_1                         0x10
#define PHY_MEDIUM_USB_2_0                         0x11
#define PHY_MEDIUM_USB_3_0                         0x12
#define PHY_MEDIUM_NCSI_OVER_RBT                   0x18
#define PHY_MEDIUM_KCS1_LEGACY                     0x20
#define PHY_MEDIUM_KCS1_PCI                        0x21
#define PHY_MEDIUM_SERIAL_HOST2_LEGACY             0x22
#define PHY_MEDIUM_SERIAL_HOST2_PCI                0x23
#define PHY_MEDIUM_ASYNCHRONOUS_SERIAL3            0x24


/*
* MCTP Physical Transport Binding Identifiers, refers to DSP0239 of DMTF
*/
#define MCTP_PHY_TRAN_BI_RESERVED                   0x00
#define MCTP_PHY_TRAN_BI_SMBUS                      0x01
#define MCTP_PHY_TRAN_BI_PCIE                       0x02
#define MCTP_PHY_TRAN_BI_USB                        0x03
#define MCTP_PHY_TRAN_BI_KCS                        0x04
#define MCTP_PHY_TRAN_BI_SERIAL                     0x05
#define MCTP_PHY_TRAN_BI_VENDOR                     0xFF

/*
* MCTP Message Types, refers to DSP0239 of DMTF
*/
#define MCTP_MSG_TYPE_CONTROL                   0x0
#define MCTP_MSG_TYPE_PLDM                      0x1
#define MCTP_MSG_TYPE_NCSI                      0x2
#define MCTP_MSG_TYPE_ETHERNET                  0x3
#define MCTP_MSG_TYPE_NVME                      0x4
#define MCTP_MSG_TYPE_VENDOR_PCI                0x7E
#define MCTP_MSG_TYPE_VENDOR_IANA               0x7F

#define MCTP_MSG_TYPE_COUNT                     0x07

/*
* MCTP Control Commands, refers to DSP0236 of DMTF
*/
#define MCTP_CMD_RESERVED                            0x00
#define MCTP_CMD_SET_ENDPOINT_ID                     0x01
#define MCTP_CMD_GET_ENDPOINT_ID                     0x02
#define MCTP_CMD_GET_ENDPOINT_UUID                   0x03
#define MCTP_CMD_GET_MCTP_VERSION_SUPPORT            0x04
#define MCTP_CMD_GET_MESSAGE_TYPE_SUPPORT            0x05
#define MCTP_CMD_GET_VENDOR_DEFINED_MESSAGE_SUPPORT  0x06
#define MCTP_CMD_RESOLVE_ENDPOINT_ID                 0x07
#define MCTP_CMD_ALLOCATE_ENDPOINT_IDS               0x08
#define MCTP_CMD_ROUTING_INFORMATION_UPDATE          0x09
#define MCTP_CMD_GET_ROUTING_TABLE_ENTRIES           0x0A
#define MCTP_CMD_PREPARE_FOR_ENDPOINT_DISCOVERY      0x0B
#define MCTP_CMD_ENDPOINT_DISCOVERY                  0x0C
#define MCTP_CMD_DISCOVERY_NOTIFY                    0x0D
#define MCTP_CMD_GET_NETWORK_ID                      0x0E
#define MCTP_CMD_QUERY_HOP                           0x0F
#define MCTP_CMD_RESOLVE_UUID                        0x10

/*
* Special Endpoint IDs, refers to DSP0236 of DMTF
*/
#define NULL_DST_EID                                0x00
#define NULL_SRC_EID                                0x00
#define BROADCAST_EID                               0xFF

/*
* MCTP Control Message Completion Codes, refers to DSP0236 of DMTF
*/
#define MCTP_CC_SUCCESS                             0x00
#define MCTP_CC_ERROR                               0x01
#define MCTP_CC_ERROR_INVALID_DATA                  0x02
#define MCTP_CC_ERROR_INVALID_LENGTH                0x03
#define MCTP_CC_ERROR_NOT_READY                     0x04
#define MCTP_CC_ERROR_UNSUPPORTED_CMD               0x05
#define MCTP_CC_ERROR_INVALID_PLDM_TYPE             0x20

#define DEF_MSG_TAG     0x0
#define DEF_PKG_SEQ     0x02

#define MCTP_MSG_HEAD_LEN 16
#define MCTP_MAX_PAYLOAD_LEN 256
#define MCTP_MAX_PAYLOAD_EXT_LEN 1024
#define MCTP_MIN_PAYLOAD_LEN 4

/*最小包大小*/
#define MCTP_MIN_PACKET_SIZE (MCTP_MIN_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)

/*最大包大小*/
#define MCTP_MAX_PACKET_SIZE (MCTP_MAX_PAYLOAD_LEN + MCTP_MSG_HEAD_LEN)
#define MCTP_VDM_MSG_LEN (MCTP_MAX_PACKET_SIZE - MCTP_MSG_HEAD_LEN - 1)

#define MCTP_MAX_LOG_DATA_LEN  (MCTP_MAX_PAYLOAD_LEN - MCTP_MSG_HEAD_LEN - 1)

#define MCTP_MSG_EXT_LEN 8192

#pragma pack(1)

/* 以下是根据MCTP over PCI Express Packet Format定义的数据结构 */
typedef struct  _MCTP_HEAD
{
#ifdef  LITTLE_ENDIAN
    guint8 type : 7;     // Identifies the payload format used for higher level protocols
    guint8 ic   : 1;     // Integrity Check bit
#else
    guint8 ic   : 1;
    guint8 type : 7;
#endif
}MCTP_MSG_TYPE_HEAD;

/* 此结构体数据定义与规范有差异，假设规范定义的是数据格式
    BYTE 0-3,
    BYTE 4-7,
    BYTE 8-11,
    BYTE 12-15
    但是，hi1710/hi1711 SDK要求的内存数据格式是
    BYTE 4-7,
    BYTE 0-3,
    BYTE 12-15
    BYTE 8-11,
*/
typedef struct  _pcie_medium_head {
    guint8 msgcode;
#ifdef  LITTLE_ENDIAN
    guint8 vdmcode : 4;
    guint8 pad     : 2;
    guint8 res5    : 2;
#else
    guint8 res5    : 2;
    guint8 pad     : 2;
    guint8 vdmcode : 4;
#endif
    guint16 requestid;
    guint8  len_l;
#ifdef  LITTLE_ENDIAN
    guint8 len_h : 2;
    guint8 res4  : 2;
    guint8 attr  : 2;
    guint8 ep    : 1;
    guint8 td    : 1;
#else
    guint8 td    : 1;
    guint8 ep    : 1;
    guint8 attr  : 2;
    guint8 res4  : 2;
    guint8 len_h : 2;
#endif
#ifdef  LITTLE_ENDIAN
    guint8 res3    : 4;
    guint8 tc      : 3;
    guint8 res2    : 1;
#else
    guint8 res2    : 1;
    guint8 tc      : 3;
    guint8 res3    : 4;
#endif
#ifdef  LITTLE_ENDIAN
    guint8 type    : 5;
    guint8 fmt     : 2;
    guint8 res1    : 1;
#else
    guint8 res1    : 1;
    guint8 fmt     : 2;
    guint8 type    : 5;
#endif
} pcie_medium_head;

typedef struct  _mctp_transport_head {
#ifdef  LITTLE_ENDIAN
    guint8 tag : 3;      // Identifies packets belonging to a particular message
    guint8 to  : 1;      // Tag Owner bit
    guint8 seq : 2;      // Sequence number
    guint8 eom : 1;      // Identifies last packet in message
    guint8 som : 1;      // Identifies first packet in message
#else
    guint8 som : 1;
    guint8 eom : 1;
    guint8 seq : 2;
    guint8 to  : 1;
    guint8 tag : 3;
#endif
    guint8 srceid;        // Logical Address of the message originator
    guint8 desteid;       // Logical Address of the message target
#ifdef  LITTLE_ENDIAN
    guint8 ver  : 4;
    guint8 res1 : 4;
#else
    guint8 res1 : 4;
    guint8 ver  : 4;     // Identifies MCTP transport packet format and fields for a given medium
#endif
} mctp_transport_head;

// 基于PCIe VDM的MCTP发送包格式
typedef struct  _pcie_vdm_head_tx {
    pcie_medium_head    pciemedium;
    mctp_transport_head mcptransport;
    guint16      vdid;
    guint16      targetid;
} pcie_vdm_head_tx;

// 基于PCIe VDM的MCTP接收包格式
typedef struct  _pcie_vdm_head_rx {
    guint8 len_l;
#ifdef  LITTLE_ENDIAN
    guint8 len_h : 2;
    guint8 res1  : 6;
#else
    guint8 res1  : 6;
    guint8 len_h : 2;
#endif
    guint8 res2;
#ifdef LITTLE_ENDIAN
    guint8 type : 5;
    guint8 res3 : 3;
#else
    guint8 res3 : 3;
    guint8 type : 5;
#endif
    guint8 res4;
#ifdef  LITTLE_ENDIAN
    guint8 vdmcode : 4;
    guint8 pad     : 2;
    guint8 res5    : 2;
#else
    guint8 res5    : 2;
    guint8 pad     : 2;
    guint8 vdmcode : 4;
#endif
    guint16 requestid;
    guint16 vdid;
    guint16 targetid;
    mctp_transport_head mcptransport;
} pcie_vdm_head_rx;

/*	类似于UDP /ICMP/TCP协议*/
typedef struct  _mctp_msg_head {
#ifdef LITTLE_ENDIAN
        guint8 instance_id :5;
        guint8 rsv         :1;
        guint8 d_bit       :1;
        guint8 rq          :1;
#else
        guint8 rq          :1;
        guint8 d_bit       :1;
        guint8 rsv         :1;
        guint8 instance_id :5;
#endif
    guint8 com_code;
} mctp_msg_head;

typedef struct _mctp_req {
    pcie_vdm_head_tx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 pad;
} mctp_req;

typedef struct _mctp_rsq_notify {
    pcie_vdm_head_tx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 pad;
}mctp_rsq_notify;

typedef struct _mctp_rsq_uuid {
    pcie_vdm_head_tx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 pad;
}mctp_rsq_uuid;

typedef struct _mctp_rsq_ep_discover {
    pcie_vdm_head_rx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 pad;
}mctp_rsq_ep_discover;

//bmc to me
typedef struct _mctp_rsp_uuid {
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 uuid[16];
}mctp_rsp_uuid;

typedef struct _mctp_rsp_notify {
    mctp_msg_head msg_head;
    guint8 completion_code;
}mctp_rsp_notify;

typedef struct _mctp_rsp_prepare_ep_discovery {
    mctp_msg_head msg_head;
    guint8 completion_code;
}mctp_rsp_prepare_ep_discovery;

typedef struct _mctp_rsp_ep_discover {
    mctp_msg_head msg_head;
    guint8 completion_code;
}mctp_rsp_ep_discover;

typedef struct _bmc_to_net_get_eid {
    pcie_vdm_head_tx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 pad;
}bmc_to_net_get_eid;

typedef struct _net_to_bmc_get_eid {
    pcie_vdm_head_rx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 bmc_eid;
    #ifdef LITTLE_ENDIAN
    guint8 bmc_eid_type : 2;
    unsigned rsv1 : 2;
    guint8 bmc_type : 2;
    guint8 rsv2 : 2;
#else
    guint8 rsv2 : 2;
    guint8 bmc_type : 2;
    unsigned rsv1 : 2;
    guint8 bmc_eid_type : 2;
#endif
    guint8 medium_specific_info;
    guint8 pad;
}net_to_bmc_get_eid;

typedef struct _bmc_to_net_get_uuid {
    pcie_vdm_head_tx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 pad;
}bmc_to_net_get_uuid;

typedef struct _net_to_bmc_get_uuid {
    pcie_vdm_head_rx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 uuid[16];
}net_to_bmc_get_uuid;

typedef struct _mctp_rsq_get_msg_type_support {
    mctp_msg_head msg_head;
    guint8 pad;
}mctp_rsq_get_msg_type_support;

typedef struct _mctp_rsp_get_msg_type_support {
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 num_msg_type;
    guint8 type_support[MCTP_MSG_TYPE_COUNT];
}mctp_rsp_get_msg_type_support;

typedef struct _mctp_rsq_get_eid {
    mctp_msg_head msg_head;
    guint8 pad;
}mctp_rsq_get_eid;

typedef struct _mctp_rsp_get_eid {
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 bmc_eid;
#ifdef LITTLE_ENDIAN
    guint8 bmc_eid_type : 2;
    guint8 rsv1         : 2;
    guint8 bmc_type     : 2;
    guint8 rsv2         : 2;
#else
    guint8 rsv2         : 2;
    guint8 bmc_type     : 2;
    guint8 rsv1         : 2;
    guint8 bmc_eid_type : 2;
#endif
    guint8 medium_specific_info;
    guint8 pad;
}mctp_rsp_get_eid;

typedef struct _mctp_rsq_set_eid {
    mctp_msg_head msg_head;
#ifdef LITTLE_ENDIAN
    guint8 operation : 2;
    guint8 rsv       : 6;
#else
    guint8 rsv       : 6;
    guint8 operation : 2;
#endif
    guint8 assigned_eid;
    guint8 pad[3];
}mctp_rsq_set_eid;

typedef struct _mctp_rsp_set_eid {
    mctp_msg_head msg_head;
    guint8 completion_code;
#ifdef LITTLE_ENDIAN
    guint8 eid_allocate_status : 2;
    guint8 rsv1                : 2;
    guint8 eid_assign_status   : 2;
    guint8 rsv2                : 2;
#else
    guint8 rsv2                : 2;
    guint8 eid_assign_status   : 2;
    guint8 rsv1                : 2;
    guint8 eid_allocate_status : 2;
#endif
    guint8 eid_set;
    guint8 eid_pool_size;
    guint8 pad;
}mctp_rsp_set_eid;

typedef struct _mctp_rsq_get_mctp_version {
    mctp_msg_head msg_head;
    guint8 message_type;
}mctp_rsq_get_mctp_version;

#define MAX_GET_MCTP_VERSION_RSP    0x4

typedef struct _mctp_rsp_get_mctp_version {
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 version_num_entry_count;
    guint8 version_entry[MAX_GET_MCTP_VERSION_RSP];
    guint8 pad[3];
}mctp_rsp_get_mctp_version;
typedef struct _mctp_rsq_route_table {
    mctp_msg_head msg_head;
    guint8 entry_handle;
}mctp_rsq_route_table;

typedef struct _mctp_route_table_entry {
    guint8 size_eid_range;
    guint8 start_eid;
#ifdef LITTLE_ENDIAN
    guint8 port_num             : 5;
    guint8 dymatic_static_entry : 1;
    guint8 entry_type           : 2;
#else
    guint8 entry_type           : 2;
    guint8 dymatic_static_entry : 1;
    guint8 port_num             : 5;
#endif
    guint8 transport_bind_identify;
    guint8 physical_media_type_identify;
    guint8 physical_address_size;
    guint16 physical_adress;
}mctp_route_table_entry;

typedef struct _mctp_rsp_route_table {
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 next_entry;
    guint8 num_returned_entry;
    mctp_route_table_entry route_table_entry[16];
    guint8 pad[2];
}mctp_rsp_route_table;

typedef struct _mctp_rsq_resolve_eid {
    pcie_vdm_head_tx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 target_eid;
}mctp_rsq_resolve_eid;

//me to bmc
typedef struct _mctp_rsp_resolve_eid {
    pcie_vdm_head_rx pcie_vdm_head;
    mctp_msg_head msg_head;
    guint8 completion_code;
    guint8 bridge_eid;
    guint8 bus_num;  // refers to Physical Address Format for MCTP Control Messages in DSP0238
#ifdef LITTLE_ENDIAN
    guint8 function_num : 3;
    guint8 device_num   : 5;
#else
    guint8 device_num   : 5;
    guint8 function_num : 3;
#endif
    guint8 pad;
}mctp_rsp_resolve_eid;

#pragma pack()

#endif
