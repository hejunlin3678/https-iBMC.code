/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Function definition of raidadm lib
 * Author: zhuaiqiang
 * Create: 2021-03-25
 */

#ifndef HOST_TOOLS_LIB_TOOL_LIB_H
#define HOST_TOOLS_LIB_TOOL_LIB_H

#include "lib_api.h"

#define DECIMAL_1000    1000
#define MAX_CHANNAL_NAME_LEN    16
#define MAX_PF_NUM_PER_ADAPTER    2 // 当前芯片最多支持2个PF。 当前默认通过PF0来发送带外消息
#define MAX_OUTBAND_HEAD_LEN 128
#define MAX_EVENT_DESCRIPTION_LEN 128

enum STORE_CHANNAL_TYPE {
    CHANNAL_IN_BAND_SCSI_PF0 = 0,
    CHANNAL_IN_BAND_SCSI_PF1,
    CHANNAL_IN_BAND_NVME_PF0,
    CHANNAL_IN_BAND_NVME_PF1,
    CHANNAL_OUT_BAND_MCTP_PCIE,
    CHANNAL_OUT_BAND_MCTP_I2C,
    CHANNAL_UNKNOWN_TYPE
};

/*****************************************************************************
 Description  : lib对外的命令属性数据结构
*****************************************************************************/
struct lib_cmd_param {
    u8 opcode; // 厂商自定义操作码，用于区分命令读写类型。（1）0xC0: 非读写类命令，无数据传输（2）0xC1：写类型命令，有从主机到芯片的数据
                // （3）0xC2：读类型命令，有从芯片到主机的数据
    u8 channel; // raid卡通道 带内用于选则pf0/pf1，    0: pf0，  1: pf1，带外用于选则  mctp over pcie/I2C，0: mctp over pcie(pf0)，
                // 1: mctp over pcie(pf1)，0xf0  i2c （当前版本不支持），其它值保留
    u8 card_id; // 发送的raid卡编号
    u8 rsvd0; // 预留与lib库之间的控制信息
    u32 rsvd1; // 保留字段（在NVMe中用于传NSID）
    u16 subopcode; // 子操作码，标识具体功能命令
    u16 data_len; // 读或者写的数据长度
    u16 param_len; // 命令参数长度
    u16 rsvd2;
    union { // 命令参数，参数24Bytes内使用param[24]，超过24Btyes使用p_param指向参数buffer。
        u8 cmd_param[LIB_PARAM_SIZE];
        void *p_param;
    };
    void *data; // 读或者写的数据buffer
    u32 timeout_ms; // 命令超时时间，单位：毫秒
    u32 result0; // 保存特性命令执行的结果（比如查询命令，返回小于32bit的结果）
    u32 result1; // 预留，支持返回结果为64bit时，高32bit。
};

/*****************************************************************************
 Description  : lib对外的设备属性数据结构
*****************************************************************************/
struct adapter_descriptor {
    u8 adapter_idx; // 系统内RAID卡编号，业务平台自定义，建议按照0,1,2…进行编号
    u8 pf_num;      // 每张RAID卡使能的PF数量，业务至少保证探测一个PF
    u8 channel;     // 支持的传输通道类型：带内、带外 MCTP PCIe、带外MCTP I2C等。0 ：带内，1： 带外mctp over pcie，2 ： 带外 i2c
    u8 rsvd0;
    union { // 通道地址。
        u8 outband_eid[MAX_PF_NUM_PER_ADAPTER]; // outband_eid:mctp协议的eid地址
        u8 inband_chan[MAX_PF_NUM_PER_ADAPTER]; // 用于填充raid卡在OS系统中的编号
        u16 slav_addr[MAX_PF_NUM_PER_ADAPTER]; // 使用I2C通信的时候，使用的从地址
    }channel_addr;
    u16 phy_addr[MAX_PF_NUM_PER_ADAPTER];
};

/*****************************************************************************
 Description  : lib对外的事件处理数据结构
*****************************************************************************/
struct event_detail {
    u8 adapter_idx;                             // 上报事件的card编号
    u8 event_type;                              // 上报事件的类型
    u16 event_code;                             // 事件码（预留8bit，当前事件消息中用1个字节）
    u16 event_level;                            // 事件级别
    u16 rsv0;
    u64 event_param;                            // 事件参数，不同上报事件的参数结构待定义
    u32 event_seq;                              // 事件序列号（在lib库侧接收到的排序）
    int time_stamp;                             // 接收到event时的时间戳
    char desc[MAX_EVENT_DESCRIPTION_LEN];       // 上报事件描述信息
};

/*****************************************************************************
 Description  : LIB配置数据结构
*****************************************************************************/
struct sysfunc {
    int (*mctp_send)(u8 card_id, u32 req_len, const u8 *req, u32 *rsp_len, u8 *rsp, u32 timeout_us); // 消息发送函数
    int (*syslog)(int level, char *fmt, ...); // 日志接口
    int (*event_hand)(struct event_detail *evt); // 注册事件处理函数
    int (*pd_powercycle)(u8 adapter_idx, u16 enc_id, u16 slot); // 硬盘上下电处理函数
};

struct store_lib_conf {
    struct sysfunc  func_list;  // 注册的系统函数列表，参见注册函数列表
    u32 cmd_timeout_ms;         // 命令超时时间，单位：毫秒
    u32 event_poll_interval;    // 事件查询间隔时间，单位：秒
    u8  adapter_num;            // 系统支持的最大适配器数量，默认填8个
    u8  rsvd0[7];               // 对齐
};

struct event_node {
    struct event_detail evt;
    struct event_node *next;
};

struct event_queue {
    int task_num;
    struct event_node *top;
    struct event_node *tail;
};

typedef int (*BMC_MCTP_SEND_FUNC)(u8 card_id, u32 req_len, const u8 *req, u32 *rsp_len, u8 *rsp, u32 timeout_us);
typedef int (*BMC_SYSLOG_FUNC)(int level, char *fmt, ...);
typedef int (*BMC_EVENT_HAND_FUNC)(struct event_detail *evt);
typedef int (*BMC_PD_POWERCYCLE_FUNC)(u8 adapter_idx, u16 enc_id, u16 slot);

int store_lib_init(struct store_lib_conf *conf);
void store_lib_exit(void);
int store_add_adapter(struct adapter_descriptor *adpater);
int store_clear_interest_event(u8 card_id);
int store_update_interest_event(u8 card_id, struct host_event_info *msg);
int store_register_interest_event(u8 card_id, struct host_event_info *msg);
int store_event_msg_rcv(u8 adapter_idx, u8 *event_msg, int msg_len);
int store_send_cmd(struct lib_cmd_param *cmd_param);

#endif // HOST_TOOLS_LIB_TOOL_LIB_H