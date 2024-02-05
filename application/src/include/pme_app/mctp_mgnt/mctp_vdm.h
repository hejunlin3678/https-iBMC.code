/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MCTP PCIe VDM Transport Binding相关定义
 * Author: huanghan1@huawei.com
 * Create: 2021-02-05
 */

#ifndef MCTP_VDM_MSG_H
#define MCTP_VDM_MSG_H

#define MCTP_PCIE_VDM_HDR_OFFSET 0
#define MCTP_PCIE_TRAN_HDR_OFFSET 12
#define MCTP_PCIE_PAYLOAD_OFFSET 16

#define MCTP_VDM_HEADER_LEN 16
#define MCTP_OVER_PCIE_HEADER_LEN   17

/*
* MCTP over PCIe Packet Formats, refers to DSP0238 of DMTF
*/
#define MCTP_HEAD_VERSION                           0x01 /* header version */

#define ROUTE_TO_ROOT                               0x10 /* Type and Routing */
#define ROUTE_BY_ID                                 0x12
#define BROADCAST_FROM_RC                           0x13

#define DEF_PCIE_FMT     0x03
#define DEF_PCIE_TYPE    0x12
#define DEF_PCIE_TC      0x00
#define DEF_PCIE_TD      0x00
#define DEF_PCIE_EP      0x00
#define DEF_PCIE_ATTR    0x1
#define DEF_PCIE_LEN     0x80
#define DEF_PCIE_REQ     0x0000
#define DEF_PCIE_PAD     0x00
#define DEF_PCIE_VDMCODE 0x00
#define DEF_PCIE_MSGCODE 0x7f
#define DEF_PCIE_TARGET  0x0000
#define DEF_PCIE_VDID    0x1ab4

#define SOM_TRUE        0x01
#define SOM_FASLE       0x00

#define EOM_TRUE        0x01
#define EOM_FALSE       0x00

#define RQ_TRUE         0x01
#define RQ_FALSE        0x00

#define TO_TRUE         0x01
#define TO_FALSE        0x00

#endif

