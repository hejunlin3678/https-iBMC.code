
#ifndef __RETIMER_DEBUG_H__
#define __RETIMER_DEBUG_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

#define EYE_DIAG_ITEM_NUM   4
#define EYE_DIAG_OFFSET     4
#define EYE_DIAG_DATA_LEN   12
#define CTLE_ITEM_NUM       10
#define CTLE_DATA_OFFSET    4
#define CTLE_DATA_TOTAL_LEN 14
#define FFE_ITEM_MAX_NUM    5
#define FFE_DATA_OFFSET     4
#define FFE_DATA_TOTAL_LEN  14

gint32 retimer_set_rx_ctle_proc(GSList *input_list);
gint32 retimer_set_tx_ffe_proc(GSList *input_list);
gint32 retimer_set_reg_info_proc(GSList *input_list);
gint32 retimer_get_reg_info_proc(GSList *input_list);
gint32 retimer_get_eye_diag_proc(GSList *input_list);
gint32 retimer_get_rx_ctle_proc(GSList *input_list);
gint32 retimer_get_tx_ffe_proc(GSList *input_list);
gint32 retimer_set_channel_proc(GSList *input_list);
gint32 retimer_get_idle_proc(GSList *input_list);
gint32 retimer_set_prbs_send_recv_proc(GSList *input_list);
gint32 retimer_get_prbs_error_check_proc(GSList *input_list);
gint32 retimer_set_tx_swing_proc(GSList *input_list);
gint32 retimer_turn_routine_on_or_off_proc(GSList *input_list);
gint32 method_retimer_reg_read_write(GSList *input_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif 
