#ifndef _MCTP_USR_API_H_
#define _MCTP_USR_API_H_

#include "pme_app/mctp_mgnt/mctp_protocol.h"
#include "pme_app/mctp_mgnt/mctp_control.h"
#include "pme_app/mctp_mgnt/mctp_pldm.h"
#include "pme_app/mctp_mgnt/mctp_ncsi.h"
#include "pme_app/mctp_mgnt/mctp_ether.h"
#include "pme_app/mctp_mgnt/mctp_vdm.h"


/* 厂商自定义错误码 */
#define MCTP_ERR_PCIE_CHAN_DISABLED  2 // BMC的PCIe端口没有使能
#define MCTP_ERR_NOT_SUPPORT         3 // 部件不支持MCTP

/* 设备名 */
#define MCTP_DEV_NAME "/dev/mctp"

// 读数据的最大超时时间，单位us
#ifdef MCTP_TIMEOUT_MAX
#undef MCTP_TIMEOUT_MAX // Hi1711平台PME定义了同名宏，APP层使用以APP层定义的宏为准
#define MCTP_TIMEOUT_MAX  (10 * 60 * 1000 *1000)
#endif
#define MCTP_TIMEOUT_NONE  (~0U)

#define WAIT_RESPONSE  1
#define NO_WAIT_RESPONSE  0

#define MCTP_ENABLE 1
#define MCTP_DISABLE 0

#define MCTP_DEFAULT_TIMEOUT  2000000   // 2s

typedef enum {
    MCTP_TABLE_REFRESH,
    MCTP_TABLE_CHANGE,
    MCTP_TABLE_OK
} MCTP_TABLE_STATUS;

#define MCTP_HASH_TABLE_MAX_SIZE  200
#define MCTP_ENDPOINT_DEFAULT_PCIE (NULL)

#pragma pack(1)
typedef struct mctp_init_info_s {
    guint32 process_tag;
    guint8 msg_type_num;
    guint8 msg_type[16];
    guint8 msg_src_eid;
    guint16 phy_addr;
} MCTP_REGISTRY_S;

typedef struct {
    unsigned long long packets_in;          // 收发个数
    unsigned long long packets_out;         // 收发个数，统计的是写发送FIFO的次数
    unsigned long long packets_out_reg;     // 收发个数，统计的是MCTP的发包统计寄存器的值
    unsigned long long packets_drop_crc;     // 因CRC错误丢包个数
    unsigned long long packets_drop_len;     // 因接受长度错误错误丢包个数
    unsigned long long packets_drop_fifo;    // 因MCTP硬件接受FIFO溢出丢包个数
    unsigned long long packets_drop_fifo1;   // 因上层数据FIFO溢出丢包个数
} MCTP_STATISTICS;

typedef struct {
    guint8 msg_type;
    guint32 pid;
    guint32 process_tag;
    guint32 queue_cnt;
    gint32 eid;
} MCTP_CONFIG_INFO;

#define MAX_QUERY_NUM            24
typedef struct {
    MCTP_CONFIG_INFO config_info[MAX_QUERY_NUM];
    unsigned int count;
} QUERY_CONFIG_INFO;

#pragma pack()

typedef enum {
    MCTP_MEDIUM_SMBUS = 0x02,
    MCTP_MEDIUM_PCIE_VDM = 0x0F,
    MCTP_MEDIUM_SMBUS_OEM_MASTER = 0x80,
    MCTP_MEDIUM_NULL = 255
} MCTP_MEDIUM_T;

typedef struct {
    OBJ_HANDLE chip_handle;
    guint8 eid;
    guint8 medium;
    guint16 phyaddr;
} MCTP_ENDPOINT_T;

typedef struct {
    // 协议层传bmc_eid, bmc_phy_addr,me_eid, me_phy_addr给app
    guint8 bmc_eid;
    guint16 bmc_phy_addr;
    guint8 me_eid;
    guint16 me_phy_addr;
    // app传数据给协议层
    guint8 target_eid;
    guint16 target_phy_addr;
    guint8 msg_type;
    guint8 msg_tag;
    guint8 pkt_seq;
    guint8 instance_id;
    guint8 res0;
    guint8 command_code;
    guint16 timeout;   /* 每个报文的超时时间可能不一样，此处预留 */
    guint32 res1;
    guint16 data_len;  /* PCIE_VDM_HDR + PCIE_VDM_DATA的长度  */
    guint16 expect_len; // 预期接收长度

    /* Header:16 payload:大小为256Btyes的整数倍 */
    guint8* data;
} MCTP_MSG_ST, *PMCTP_MSG_ST;

gint32 mctp_transport_init(void);
gint32 mctp_pcie_transport_register(MCTP_REGISTRY_S *registry_info);
guint32 mctp_process_tag_generate(const char* key);
gint32 mctp_get_eid_config(QUERY_CONFIG_INFO *mctp_config);
gint32 mctp_ncsi_init(void);

/* Send request and receive response of all kind of MCTP message */
gint32 mctp_send_request(MCTP_ENDPOINT_T *ep, PMCTP_MSG_ST req_msg, PMCTP_MSG_ST res_msg,
    guint8 wait_response, guint32 timeout);

/* Receive MCTP Control Message Request */
gint32 mctp_recv_request(PMCTP_MSG_ST pMsg);
/* Send MCTP Control Message Response */
gint32 mctp_send_response(PMCTP_MSG_ST pMsg);

gint32 mctp_vdm_engine_reinit(void);
gint32 mctp_ether_msg_recv(PMCTP_MSG_ST res_msg);
gint32 mctp_get_msg_statistics(MCTP_STATISTICS * mctp_static);
gint32 mctp_make_endpoint(OBJ_HANDLE netcard_handle, MCTP_ENDPOINT_T *ep);
gint32 mctp_pcie_trans_print_file(FILE *fp);
#endif

