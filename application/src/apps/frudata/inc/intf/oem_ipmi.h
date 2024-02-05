
#ifndef __OEM_IPMI_H__
#define __OEM_IPMI_H__

#include "common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#ifdef DFT_ENABLED
#pragma pack(1)
typedef struct {
    guint8 sub_cmd;
    guint8 cmd_code;
    guint8 device_type;
    guint8 status;
} BMC_SET_SCAN_STATUS_REQ_MSG_S;

typedef struct {
    guint8 sub_cmd;
    guint8 cmd_code;
    guint8 device_type;
} BMC_GET_SCAN_STATUS_REQ_MSG_S;

typedef struct {
    guint8 compcode;
    guint8 status;
} BMC_GET_SCAN_STATUS_RSP_MSG_S;
#pragma pack()
#endif

gint32 ipmi_cmd_get_product_info(gconstpointer pdata, gpointer user_data);
gint32 ipmi_cmd_set_product_info(gconstpointer pdata, gpointer user_data);
gint32 ipmi_cmd_clear_product_info(gconstpointer pdata, gpointer user_data);
gint32 ipmi_cmd_get_clear_state(gconstpointer pdata, gpointer user_data);
gint32 ipmi_set_board_port_speed(gconstpointer msg_data, gpointer user_data);
gint32 ipmi_get_board_port_speed(gconstpointer msg_data, gpointer user_data);
#ifdef DFT_ENABLED
gint32 ipmi_cmd_set_scan_status(gconstpointer req_msg, gpointer user_data);
gint32 ipmi_cmd_get_scan_status(gconstpointer req_msg, gpointer user_data);
#endif
gint32 dft_get_huawei_formate_elabel(const void *msg_data, gpointer user_data);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 
#endif 