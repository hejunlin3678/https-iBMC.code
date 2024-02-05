/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: MCTP PLDM消息相关定义
 * Author: huanghan1@huawei.com
 * Create: 2021-02-05
 */

#ifndef MCTP_PLDM_MSG_H
#define MCTP_PLDM_MSG_H

#define PLDM_MAX_INSTANCE_ID                  31

#define MCTP_PLDM_STATISTICS_LEN              88
#define MCTP_PLDM_RESERVED_LEN                40
#define MCTP_PLDM_PAYLOAD_LEN               1024
#define MCTP_PLDM_LOG_LEN                   1024


#pragma pack(1)

typedef struct tag_mctp_statistic {
    guint8 tx_sn_speed;
    guint8 rx_sn_speed;
    guint8 sn_stage;
    guint8 sn_rsvd;
    guint8 sfp_speed;
    guint8 sfp_absent;
    guint8 sfp_open;
    guint8 sfp_rsvd;
    guint8 work_speed;
    guint8 work_topo;
    guint8 tx_bb_credit;
    guint8 rx_bb_credit;
    guint8 port_status;
    guint8 port_rsvd1;
    guint8 port_rsvd2;
    guint8 _16_gfecram_ecc_1biterr;
    guint8 _16_gfecram_ecc_mulbiterr;
    guint8 _16gfecram_ecc_addr;
    guint8 non32g_txdata_ecc_1biterr;
    guint8 non32g_txdata_ecc_mulbiterr;
    guint8 non32g_txdata_ecc_addr;
    guint8 non32g_rxbuf_ecc_1biterr;
    guint8 non32g_rxbuf_ecc_mulbiterr;
    guint8 non32g_rxbuf_ecc_addr;
    guint8 pub_rxeschfifo_ecc_1biterr;
    guint8 pub_rxeschfifo_ecc_mulbiterr;
    guint8 pub_rxeschfifo_ecc_addr;
    guint8 ua_txafifo_ecc_1biterr;
    guint8 ua_txafifo_ecc_mulbiterr;
    guint8 ua_txafifo_ecc_addr;
    guint8 ua_rxafifo_ecc_1biterr;
    guint8 ua_rxafifo_ecc_mulbiterr;
    guint8 ua_rxafifo_ecc_addr;
    guint8 _32g_fc2ptxfifo_ecc_1biterr;
    guint8 _32g_fc2ptxfifo_ecc_mulbiterr;
    guint8 _32g_fc2ptxfifo_ecc_addr;
    guint8 _32g_fc2prxfifo_ecc_1biterr;
    guint8 _32g_fc2prxfifo_ecc_mulbiterr;
    guint8 _32g_fc2prxfifo_ecc_addr;
    guint8 _32g_pcstxsfifo_ecc_1biterr;
    guint8 _32g_pcstxsfifo_ecc_mulbiterr;
    guint8 _32g_pcstxsfifo_ecc_addr;
    guint8 _32g_pcsrxafifo_ecc_1biterr;
    guint8 _32g_pcsrxafifo_ecc_mulbiterr;
    guint8 _32g_pcsrxafifo_ecc_addr;
    guint8 _32g_rsfec_ecc_1biterr;
    guint8 _32g_rsfec_ecc_mulbiterr;
    guint8 _32g_rsfec_ecc_addr;
    guint8 rsvd0;
    guint8 rsvd1;
    guint8 rsvd2;
    guint8 intr_enter_sneg;
    guint8 intr_bm_rx_fatal;
    guint8 intr_enter_ac;
    guint8 intr_leave_ac;
    guint8 intr_bm_tx_fatal;
    guint8 intr_rx_overflow;
    guint8 intr_tx_overflow;
    guint8 intr_loop_fail;
    guint8 intr_oi0;
    guint8 intr_psm_rcvd_lips;
    guint8 intr_lp_tov_timeout;
    guint8 intr_trans_12_lips;
    guint8 intr_op0;
    guint8 intr_link_fail;
    guint8 intr_rx_fatal_err;
    guint8 intr_tx_fatal_err;
    guint8 intr_pcs_losesync;
    guint8 intr_edec_sync;
    guint8 intr_p0;
    guint8 intr_lpsm_miss_rdy;
    guint8 intr_rcvd_arbf0;
    guint8 intr_rcvd_cls;
    guint8 intr_lpsm_failes_err;
    guint8 intr_lpsm_nocredit_timeout;
    guint8 intr_psm_rx_nocredit_timeout;
    guint8 intr_lpsm_rcvd_psm_os;
    guint8 intr_rcvd_arbff;
    guint8 intr_bf_fifo;
    guint8 intr_bp_fifo;
    guint8 intr_rx_afifo;
    guint8 intr_tx_afifo;
    guint8 intr_rxbf_fif1;
    guint8 intr_tx_datfifo;
    guint8 intr_tx_cmd_fifo;
    guint8 intr_rsvd0;
    guint8 intr_rsvd1;
    guint8 rsvd;
}USER_MCTP_STATISTIC_DATA;
#pragma pack()

typedef enum {
    FC_DEFAULT_WWPN = 0,
    FC_WORK_WWPN,
    FC_DEFAULT_WWNN,
    FC_WORK_WWNN,
    FC_WWN_TYPE_BUTT
} FABRIC_WWN_TYPE;

typedef enum _mctp_pldm_link_status_ {
    MCTP_PLDM_LINK_STATUS_UNKNOW = 0,
    MCTP_PLDM_LINK_STATUS_DISCONNECT = 0x01,
    MCTP_PLDM_LINK_STATUS_CONNECT = 0x02,
} MCTP_PLDM_LINK_STATUS;

typedef struct _FRU_TABLE_INFO_ {
    gchar wwpn[64];
    gchar wwnn[64];
    gchar part_num[64];
    gchar serial_number[64];
    gchar ver[64];
} FRU_TABLE_INFO;

typedef struct {
    guint8 log_cmd;
    guint8 log_type;
    guint8 *progress_bar;
} MCTP_LOG_DATA_S;

gint32 mctp_pldm_get_specified_wwn(OBJ_HANDLE obj_handle, FABRIC_WWN_TYPE wwn_type, gchar *wwn, gsize wwn_len);
gint32 mctp_pldm_get_statistics(OBJ_HANDLE obj_handle, USER_MCTP_STATISTIC_DATA* statis_info, guint32 statis_len);
gint32 mctp_pldm_get_fw_log(OBJ_HANDLE obj_handle, guint8** buf, guint32* buf_len, MCTP_LOG_DATA_S *log_data);
gint32 mctp_pldm_get_temp(OBJ_HANDLE obj_handle, guint16* temperature);
gint32 mctp_pldm_get_linkspeed(OBJ_HANDLE obj_handle, guint16* linkspeed);
gint32 mctp_pldm_get_linkstatus(OBJ_HANDLE obj_handle, guint16* linkstatus);
gint32 mctp_pldm_get_frutable_info(OBJ_HANDLE obj_handle, FRU_TABLE_INFO* table_info);
gint32 mctp_pldm_get_fru_info_qlogic(OBJ_HANDLE obj_handle, FRU_TABLE_INFO* table_info);
gint32 mctp_pldm_get_temp_qlogic(OBJ_HANDLE obj_handle, guint16 *temperature);
gint32 mctp_pldm_get_link_speed_qlogic(OBJ_HANDLE obj_handle, guint16 *linkspeed);
gint32 mctp_pldm_get_link_state_qlogic(OBJ_HANDLE obj_handle, guint16* linkstate);

#endif
