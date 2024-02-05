/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2019-2019. All rights reserved.
 * Description: lsw public头文件
 * Author: m00446762
 * Create: 2019/10/30
 */

#ifndef __LSW_PUBLIC_H__
#define __LSW_PUBLIC_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define LSW_MAX_LPORT_NUM 0xff

#define LSW_CENTER_RPC_SERVER_PORT 51001
#define LSW_LOCAL_RPC_SERVER_PORT 51002
#define LSW_MAX_PLANE_NAME_LEN 32
#define LSW_MAX_SLOT_ID 48
#define LSW_NODE_VALID 0x5a
#define LSW_NODE_INVALID 0xff
#define LSW_NODE_UNSTEADY 0x55
#define LSW_PORT_NAME_MAX_LEN 32
#define LSW_LINK_DOWN 0
#define LSW_LINK_UP 1
#define LSW_LINK_INVALID 0xff
#define IPMB_OBJECT_SMM1 "SMM1"
#define IPMB_OBJECT_SMM2 "SMM2"
#define SMM1_SLOT_ID 37
#define SMM2_SLOT_ID 38
#define BLADE1_SLOT_ID 1
#define BLADE2_SLOT_ID 2
#define SLOT_ID_ANY 0xff
#define LSW_LINKSCAN_INTERVAL_TIME 300
#define LSW_CONFIG_VERIFY_DELAY    (60)
#define LSW_MAX_VLAN_ID            4095
#define LSW_LOCAL_LINK_SLIP_WINDOW_MASK  0x07
#define LSW_LOCAL_PORT_STATISTIC_CNT  12
#define LSW_LOCAL_PORT_SPECIAL_CNT  4
#define LSW_LOCAL_PORT_SPEEDSTRAMS  1000

#define LSW_LOCAL_PORT_AUTONEG_ENABLE   1
#define LSW_LOCAL_PORT_AUTONEG_DISABLE  0
#define LSW_LOCAL_PORT_DUPLEX_FULL      1
#define LSW_LOCAL_PORT_DUPLEX_HALF      0
#define LSW_LOCAL_PORT_STATUS_INVALID   2
#define LSW_LOCAL_PORT_STATUS_ENABLE    1
#define LSW_LOCAL_PORT_STATUS_DISABLE   0
#define LSW_OUT_PANEL_PORT              1

/* 端口link down时，redfish中端口的自协商和全双工模式不可见 */
#define LSW_AUTONEG_INVISIABLE   2
#define LSW_DUPLEX_INVISIABLE    2
#define LSW_SPEED_INVISIABLE     "invisiable"

#define LSW_REDFISH_PORT_WORK_RATE_NUM      6
#define LSW_CTRL_PORT_10M_SPEED             "10(M)"
#define LSW_CTRL_PORT_100M_SPEED            "100(M)"
#define LSW_CTRL_PORT_1000M_SPEED           "1(G)"
#define LSW_CTRL_PORT_10G_SPEED             "10(G)"
#define LSW_CTRL_PORT_25G_SPEED             "25(G)"
#define LSW_CTRL_PORT_100G_SPEED            "100(G)"

/* 背板类型定义 */
#define LSW_BACK_PLANE_TYPE_4U       0  /* 0b000 4U背板 */
#define LSW_BACK_PLANE_TYPE_8U       2  /* 0b010 8U背板 */
#define LSW_BACK_PLANE_TYPE_EQUIP    7  /* 0b111 装备背板 */

#define LSW_DFT_MODE                 1  /* 装备模式 */
#define LSW_NORMAL_MODE              0  /* 正常模式 */

#define SPECIAL_PORT_SLOT_POSITION   3
#define LSW_CENTER_LOCAL_REG_DIF   1
#define LSW_CENTER_LOCAL_XML_DIF   2

#define LSW_GET_CMD_IPMI_OEM 0x60
#define LSW_SET_CMD_IPMI_OEM 0x61

/* LSW GET类命令 */
#define LSW_IPMI_GET_NODE_STATUS            0x01
#define LSW_IPMI_GET_PORT_STATISTIC         0x02
#define LSW_IPMI_GET_PORT_INFO              0x03
#define LSW_IPMI_GET_BROAD_ONLINE_STATUS    0x04
#define LSW_IPMI_M_PATH_REQUEST             0x05
#define LSW_IPMI_GET_BROAD_PORT_STATUS      0x06
#define LSW_IPMI_HANDLE_NODE_LINK_STATUS    0x07
#define LSW_IPMI_GET_BACKPLANE_TYPE         0x08
#define LSW_IPMI_GET_NODE_VAILD_STATUS_TYPE 0x09
#define LSW_IPMI_GET_ALL_NODE_STATUS        0x0a
#define LSW_IPMI_GET_ITF_PORT_MODE          0x0b
#define LSW_IPMI_GET_MESH_HEALTH_STATUS     0x0c

#define LSW_IPMI_DATA_COLLECTION            0x0d
#define LSW_IPMI_GET_FILE_TRANSFER_STATUS   0x11

/* LSW SET类命令 */
#define LSW_IPMI_UPDATE_LINK_STATUS         0x01
#define LSW_IPMI_SET_PORT_PVID              0x03
#define LSW_IPMI_SET_PORT_LOOP_MODE         0x04 /* 未使用 */
#define LSW_IPMI_SET_PORT_TRANS_RULE        0x05 /* 未使用 */
#define LSW_IPMI_SET_PORT_SUPPRESSION       0x06 /* 未使用 */
#define LSW_IPMI_SET_PORT_ENABLESTATUS      0x07
#define LSW_IPMI_SET_PORT_FECSTATUS         0x08
#define LSW_IPMI_UPDATE_PORT_INFO           0x09
#define LSW_IPMI_UPDATE_CHIP_STATUS         0x0a
#define LSW_IPMI_SET_ITF_PORT_MODE          0x0b
#define LSW_IPMI_UPDATE_HEART_BEAT          0x0c
#define LSW_IPMI_START_FILE_TRANSFER        0x11

#define LSW_IPMI_NODE_MSG_LEN 6
#define LSW_IPMI_SET_MTU_MSG_LEN 41
#define LSW_IPMI_SET_ENABLE_MSG_LEN 41
#define LSW_IPMI_GET_STATISTIC_LEN 37
#define LSW_IPMI_GET_PORT_INFO_LEN 37
#define LSW_IPMI_SET_PVID_MSG_LEN 39
#define LSW_IPMI_SET_PORT_FEC_MSG_LEN 14
#define LSW_IPMI_UPDATE_LINK_STATUS_MSG_LEN 39
#define INTERAL_OEM_CMD 0x93
#define LSW_IPMI_GET_ONLINE_BY_SLOT_MSG_LEN 20
#define LSW_IPMI_M_PATH_REQUEST_LEN 35
#define LSW_IPMI_GET_PORT_STATUS_MSG_LEN 13
#define LSW_IPMI_HANDLE_LINK_STATUS_LEN 24
#define LSW_IPMI_BACKPLANE_TYPE_LEN 5
#define LSW_IPMI_NODE_VAILD_STATUS_MSG_LEN 12
#define LSW_IPMI_GET_NODE_STATUS_MSG_LEN 20
#define LSW_IPMI_GET_FILE_TRANSFER_STATUS_MSG_LEN 5

#define LSW_IPMI_GET_MESH_STATUS_MSG_LEN 5

#define LSW_IPMI_GET_ITF_PORT_MODE_MSG_LEN 5
#define LSW_IPMI_SET_ITF_PORT_MODE_MSG_LEN 6

#define LSW_CHIP_POWER_DOWN 0xA0  /* 带内芯片处于下电状态 */
#define THREE_PARAMRTER 3
#define FOUR_PARAMRTER 4
#define FIVE_PARAMRTER 5
#define SIX_PARAMRTER 6
#define SEVEN_PARAMRTER 7

#define IPMI_REQUEST_LEN 255

#define DELAY_1S 1000
#define RPC_RETRY_TIMES 3
#define BMC_IP_BASEADDR 128

#define LSW_M_PATH_VERSION 0x20 /* 多路径功能版本号，用于中继特性不兼容的版本间的互斥 */

#define LSW_CHIP_START_UNKNOWN 0
#define LSW_CHIP_WARM_START    1
#define LSW_CHIP_COLD_START    2

#define CHIP_INIT         2
#define CHIP_POWER_ON     1
#define CHIP_POWER_DOWN   0

#define BOARD_TCE_4U_SMM  0x01
#define BOARD_TCE_8U_SMM  0x02
#define BOARD_TCE_2U2     0x03
#define BOARDID_CN221SV2  0x56
#define BOARDID_CM221S    0x34

#define RET_ERR_NOT_EXIST (-2)

typedef enum tag_LSW_M_PATH_RUN_MODE_E {
    LSW_M_PATH_DISABLE = 0,    /* 关闭mesh多路径中继 */
    LSW_M_PATH_LINK_DOWN_MODE,      /* 运行mesh多路径中继，中继方式使用linkdown状态 */
    LSW_M_PATH_LINK_FAULT_MODE,     /* 运行mesh多路径中继，中继方式使用linkfault状态：包含linkdown和发包检测 */
    LSW_M_PATH_RUN_MODE_INVALID
} LSW_M_PATH_RUN_MODE_E;

typedef enum {
    MESH_STATUS_OK = 0,
    MESH_STATUS_FAULTY,
    MESH_CHANNEL_FAULTY,
    MESH_CONFIG_FAULTY,
    MESH_STATUS_MAX,
}LSW_MESH_STATUS;

typedef enum {
    CONF_VERIFY_STATUS_OK = 0,
    CONF_VERIFY_STATUS_FAULTY,
    CONF_VERIFY_STATUS_MAX,
}LSW_M_CONF_VERIFY_STATUS;

typedef enum {
    PORT_MODE_100G = 1, /* 模式1：8*25G端口不组trunk，2*100G端口组为一组trunk */
    PORT_MODE_25G,      /* 模式2：8*25G组为两组trunk，2*100G端口不组trunk */
    PORT_MODE_400G,     /* 模式3：400GE大带宽模式，8*25G组为两组trunk */
    PORT_MODE_MAX
} LSW_ITF_PORT_MODE;

typedef enum {
    LSW_OFF = 0,
    LSW_ON,
    LSW_INIT,
} LSW_FLAG;

/* 构造ipmi应答消息,00 07 db对应华为内部地址 */
#define LSW_IPMI_RESP_MSG(resp_data, status) do {\
    (resp_data)[0] = (status);\
    (resp_data)[1] = 0xdb;\
    (resp_data)[2] = 0x07;\
    (resp_data)[3] = 0x00;\
} while(0)

/* 构造center到local的ipmi请求头 */
#define LSW_IPMI_REQ_HEAD(request_header, msg_len, slot_id) do {\
    (request_header).lun = 0;\
    (request_header).netfn = NETFN_OEM_REQ;\
    (request_header).cmd = INTERAL_OEM_CMD;\
    (request_header).src_len = (msg_len);\
    (request_header).slot_id = (slot_id);\
} while(0)

/* lsw请求的共用ipmi消息体 */
#define LSW_IPMI_REQ_BODY(req_data, sub_cmd, para_sel) do {\
    (req_data)[0] = 0xdb;\
    (req_data)[1] = 0x07;\
    (req_data)[2] = 0x00;\
    (req_data)[3] = (sub_cmd);\
    (req_data)[4] = (para_sel);\
} while(0)

/* 构造local到center的ipmi请求头 */
#define LSW_LOCAL_IPMI_REQ_HEAD(req_msg_header, msg_len) do {\
    (req_msg_header).target_type = IPMI_SMM;\
    (req_msg_header).target_instance = 0;\
    (req_msg_header).lun = 0;\
    (req_msg_header).netfn = NETFN_OEM_REQ;\
    (req_msg_header).cmd = INTERAL_OEM_CMD;\
    (req_msg_header).src_len = (msg_len);\
} while(0)

typedef struct lsw_port_info_stru {
    guint32 port_duplex;
    guint32 port_autonego;
    gchar port_work_rate[LSW_PORT_NAME_MAX_LEN];
    guint32 max_pkt_len;
    guint32 pvid;
    guint32 loop_mode;
    guint32 link_status;
    guint32 enable_status;
} LSW_PORT_INFO;

typedef struct lsw_port_statistic_stru {
    guint64 rx_octets_total;
    guint64 rx_pkt_unicast;
    guint64 rx_pkt_multicast;
    guint64 rx_pkt_broadcast;
    guint64 tx_octets_total;
    guint64 tx_pkt_unicast;
    guint64 tx_pkt_multicast;
    guint64 tx_pkt_broadcast;
    guint64 rx_mac_drop_cnt;
    guint64 rx_adpt_s2p_drop_cnt;
    guint64 rx_swb_dpi_drop_cnt;
    guint64 tx_adpt_p2s_drop_cnt;
    guint64 tx_pkt_err_all;
    guint64 tx_mac_drop_cnt;
    guint64 rx_pkt_symbol_err;
    guint64 rx_pkt_fcs_err;
} LSW_PORT_STATISTIC;

typedef struct lsw_ipmi_head_msg {
    guint8 lun;
    guint8 netfn;
    guint8 slot_id;
    guint8 cmd;
    guint8 src_len;
    guint8* data;
} LSW_IPMI_HEAD_MSG;

typedef struct {
    SEMID *sem_id;
    gchar *sem_name;
    gchar err_ret_flag; /* 0标识最开始初始化阶段信号量创建失败不会返回失败, 需要在后面使用之前判断锁是否为NULL */
} LSW_SEM_INFO;

typedef gint32 (*lsw_ipmi_handler)(guint8 slot_id, const gchar* obj_name, GSList* input_list, GSList** output_list);

typedef struct lsw_method_resource {
    const gchar method_name[32];
    lsw_ipmi_handler handler;
} LSW_IPMI_METHOD_RESOURCE;

extern void lsw_get_flag(guint32 set_value, guint32 reg_value, guint32 xml_value, guint8 *flag);
extern gint32 lsw_check_spec_param_validity(GSList* input_list, GSList** output_list);
extern gint32 lsw_get_rpc_server_ip(gchar *ip);
extern gint32 lsw_get_ethgroup_handle(OBJ_HANDLE* ethgroup_handle);
extern gint32 get_host_slot_id(guint8* slot_id);
extern guint32 lsw_get_rpc_server_port(void);
extern gint32 lsw_get_board_presence_by_slotid(guint8 slot_id, guint8 *presence);
extern gint32 lsw_get_ipmb_obj_handle_by_slotid(guint8 slot_id, OBJ_HANDLE* obj_handle);
extern gint32 lsw_public_get_ft_mode_status(guint8 *ft_mode);
extern gint32 lsw_public_get_back_plane_type(guint32 *type);
extern gint32 lsw_ipmi_send_msg_to_blade(LSW_IPMI_HEAD_MSG* header, gpointer* response);
extern const gchar* lsw_get_inner_portname(const gchar *port_name);
extern const gchar* lsw_get_real_portname(const gchar *port_name);
extern gint32 lsw_ipmi_check_src_legality(const void* msg_data, guint8 req_len,
                                          const guint8** req_data, guint8 *complete_code);

extern gint32 lsw_get_boardtype(guint32 *board_type);
extern gint32 lsw_get_main_boardid(guint32 *main_board_id);
gint32 lsw_read_accessor_byte(const gchar *acc_name, guint8 *p_byte_val);
gint32 lsw_write_accessor_byte(const gchar *acc_name, guint8 byte_val);
gint32 lsw_get_mesh_health_status(guint8 *health_status);
gint32 lsw_init_sem(LSW_SEM_INFO *sem_list, guint32 sem_num);
gint32 lsw_read_accessor_byte(const gchar *acc_name, guint8 *p_byte_val);
gint32 lsw_write_accessor_byte(const gchar *acc_name, guint8 byte_val);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __LSW_PUBLIC_H__ */
