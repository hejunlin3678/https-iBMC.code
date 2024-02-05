

#ifndef __RETIMER_DRIVER_H__
#define __RETIMER_DRIVER_H__

#include "retimer_5902_dev.h"
#include "retimer_ds280_dev.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32(*retimer_set_rx_ctle)(OBJ_HANDLE obj_handle, SET_RX_CTLE_DEBUG *rx_ctle, gint8 *data_info, gint32 buf_len);
gint32(*retimer_set_tx_ffe)(OBJ_HANDLE obj_handle, SET_TX_FFE_DEBUG *tx_ffe, gint8 *data_info, gint32 buf_len);
gint32(*retimer_set_reg_info)(OBJ_HANDLE obj_handle, guint32 regAddr, guint32 data_info);
gint32(*retimer_get_reg_info)(OBJ_HANDLE obj_handle, guint32 regAddr, gint8 *data_info, gint32 buf_len);
gint32(*retimer_get_eye_diag)(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32(*retimer_get_rx_ctle)(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32(*retimer_get_tx_ffe)(OBJ_HANDLE obj_handle, guint8 channel_id, gint8 *data_info, gint32 buf_len);
gint32(*retimer_get_waite_flag)(OBJ_HANDLE obj_handle, gint8 *data_info, gint32 buf_len);
gint32(*retimer_get_prbs_error)(OBJ_HANDLE obj_handle, PRBS_ERROR_CHECK *prbs, gint8 *data_info, gint32 buf_len);
gint32(*retimer_prbs_send_recv)(OBJ_HANDLE obj_handle, PRBS_SEND_RECV *prbs);
gint32(*retimer_set_tx_swing)(OBJ_HANDLE obj_handle, SET_TX_SWING *tx_swing);
gint32(*retimer_get_temperature)(OBJ_HANDLE obj_handle, gint16 *temp_data);
gint32(*retimer_get_heartbeat)(OBJ_HANDLE obj_handle);

guint8 retimer_get_type(void);
void retimer_driver_init(guint8 chip_type);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 

#endif
