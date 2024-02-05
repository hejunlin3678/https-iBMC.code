/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MCTP控制消息相关定义
 * Author: huanghan1@huawei.com
 * Create: 2021-02-05
 */

#ifndef MCTP_CONTROL_MSG_H
#define MCTP_CONTROL_MSG_H

#define D_TRUE          0x1
#define D_FALSE         0x0

/* Control Commands data length definitions */
#define DISCOVER_NOTIFY_DATA_LEN                    0x14
#define DISCOVER_NOTIFY_PAYLOAD_DWORD               0x01

#define PREPARE_DISCOVER_DATA_LEN                   0x14
#define PREPARE_DISCOVER_PAYLOAD_DWORD              0x01

#define EP_DISCOVER_DATA_LEN                        0x14
#define EP_DISCOVER_PAYLOAD_DWORD                   0x01

#define GET_EP_EID_RSP_DATA_LEN                     0x18
#define GET_EP_EID_RSP_PAYLOAD_DWORD                0x02
#define GET_EP_EID_RSP_PAD_LEN                      0x01

#define GET_EP_EID_RSQ_DATA_LEN                     0x14
#define GET_EP_EID_RSQ_PAYLOAD_DWORD                0x01
#define GET_EP_EID_RSQ_PAD_LEN                      0x01

#define SET_EP_EID_DATA_LEN                         0x18
#define SET_EP_EID_PAYLOAD_DWORD                    0x02
#define SET_EP_EID_RSP_PAD_LEN                      0x01

#define SET_EID                                     0x00
#define FORCE_EID                                   0x01
#define RESET_EID                                   0x02
#define SET_DISCOVERD_FLAG                          0x03
#define SET_EP_EID_DATA_RAW_LEN                     0x04 // 从IID开始统计的命令长度
#define EID_ASSIGN_ACCEPTED                         0x00
#define EID_ASSIGN_REJECTED                         0x01

#define ROUTE_TABLE_RSQ_DATA_LEN                     0x14
#define ROUTE_TABLE_RSQ_PAYLOAD_DWORD                0x01
#define ROUTE_TABLE_RSQ_PAD_LEN                      0x00

#define RESOLVE_EID_RSQ_DATA_LEN                     0x14
#define RESOLVE_EID_RSQ_PAYLOAD_DWORD                0x01
#define RESOLVE_EID_RSQ_PAD_LEN                      0x00

#define GET_UUID_RSQ_DATA_LEN                        0x14
#define GET_UUID_RSQ_PAYLOAD_DWORD                   0x01
#define GET_UUID_RSQ_PAD_LEN                         0x01

#define GET_MCTP_VERSION_RSQ_DATA_LEN                0x14
#define GET_MCTP_VERSION_PAYLOAD_DWORD               0x01
#define GET_MCTP_VERSION_RSQ_PAD_LEN                 0x00

#define GET_MCTP_MSG_TYPE_SUPPORT_RSQ_DATA_LEN       0x14
#define GET_MCTP_MSG_TYPE_SUPPORT_DWORD              0x01
#define GET_MCTP_MSG_TYPE_SUPPORT_RSQ_PAD_LEN        0x00

#define UN_DISCOVER     0x0
#define DISCOVER        0x1
#define NOT_YET_ASSIGNED    0x00

#endif

