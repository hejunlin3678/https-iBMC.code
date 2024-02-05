


#ifndef _CARD_IPMI_MSG_H_
#define _CARD_IPMI_MSG_H_

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

typedef struct ipmi_hot_oper_status_s {
    guint8 manu_id[3];
    guint8 subcmd;
    guint8 device_id;
    guint8 segment;
    guint8 status[6];
} IPMI_HOT_OPER_STATUS_S;

typedef struct ipmi_hot_oper_ras_msg_s {
    guint8 manu_id[3];
    guint8 subcmd;
    guint8 device_id;
} IPMI_HOT_OPER_RAS_MSG_S;

extern gint32 card_mgmt_send_smm_ipmi_msg(const void *data, guint8 data_len, guint8 retry);
extern gint32 card_mgmt_get_smm_ras_status(const void *data, guint8 data_len, guint8 *response, gsize resp_size,
    guint8 retry);
extern gint32 card_manage_send_ipmi_request(IPMI_REQ_MSG_HEAD_S *header, gconstpointer req_data, gsize resp_size,
    gpointer *response, gboolean wait_response, gint32 channel_type);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif
