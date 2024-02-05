


#ifndef __MODBUS_DEVICE_H__
#define __MODBUS_DEVICE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif 

gint32 modbus_rtu_conn_ctx_init(guchar ch, guint32 slave_addr, modbus_t *ctx);
gint32 modbus_init(void);
gint32 modbus_start(void);
gint32 mb_set_debug_state(GSList *input_list);
gint32 method_modbus_pwr_out_switch(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_modbus_read_input_reg(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list);
gint32 method_modbus_send_cmd(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif 


#endif
