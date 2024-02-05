
#include "pme/pme.h"
#include "pme_app/pme_app.h"


#include "modbus_device.h"
#include "asset_locator.h"


gint32 modbus_rtu_conn_ctx_init(guchar ch, guint32 slave_addr, modbus_t *ctx)
{
    gint32 ret;
    OBJ_HANDLE obj_modbus;
    modbus_t *mb_ctx = NULL;

    if ((ctx == NULL) || (ch >= MAX_UART_NUM)) {
        debug_log(DLOG_ERROR, "%s failed: input param error", __FUNCTION__);
        return RET_ERR;
    }

    ret = dal_get_specific_object_byte(CLASS_BUS_MODBUS, PROPERTY_MODBUS_UART, ch, &obj_modbus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:failed to find modbus obj for UART%d. ret=%d", __FUNCTION__, ch, ret);
        return RET_ERR;
    }

    ret = dfl_get_binded_data(obj_modbus, (gpointer *)&mb_ctx);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get bind data for UART%d failed. ret=%d", __FUNCTION__, ch, ret);
        return RET_ERR;
    }

    
    if (mb_ctx == NULL) {
        debug_log(DLOG_ERROR, "%s failed: context of channel [%d] has not been initialized", __FUNCTION__, ch);
        return RET_ERR;
    }

    
    ret = memcpy_s((void *)ctx, sizeof(modbus_t), mb_ctx, sizeof(modbus_t));
    if (ret != EOK) {
        debug_log(DLOG_ERROR, "%s failed: copy context info for channel [%d] failed, ret is %d", __FUNCTION__, ch, ret);
        return RET_ERR;
    }

    modbus_set_slave(ctx, slave_addr);

    return RET_OK;
}


LOCAL gint32 mb_common_dev_init(OBJ_HANDLE obj_modbus)
{
    
    return RET_OK;
}


LOCAL gint32 mb_common_dev_start(OBJ_HANDLE obj_modbus)
{
    return RET_OK;
}


LOCAL gint32 modbus_bind_uart_port(OBJ_HANDLE obj_modbus)
{
    gint32 ret;
    guint8 uart = 0;
    guint32 port = 0;
    UART_CONNECT_INFO_S info;

    
    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);
    if (uart < UART3) {
        debug_log(DLOG_ERROR, "%s: Set %s on UART%d failed.(Invalid or occupied)", __FUNCTION__,
            dfl_get_object_name(obj_modbus), uart);
        return RET_ERR;
    }

    
    (void)dal_get_property_value_uint32(obj_modbus, PROPERTY_MODBUS_PORT, &port);

    info.uart = (guint32)uart;
    info.port_or_uart = port;

    ret = uart_connect_init();
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: uart_connect_init failed! ret %d.", __FUNCTION__, ret);
        return RET_ERR;
    }

    return uart_connect_port(&info);
}


LOCAL modbus_t *modbus_new_handler(OBJ_HANDLE obj_modbus)
{
    modbus_t *ctx = NULL;
    guint8 uart = 0;
    guint8 type = 0;
    guint8 mode = 0;
    guint32 speed = 0;

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_UART, &uart);
    (void)dal_get_property_value_uint32(obj_modbus, PROPERTY_MODBUS_SPEED, &speed);
    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_TYPE, &type);
    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_MODE, &mode);

    if (MODBUS_TYPE_RTU == type) {
        
        ctx = modbus_new_rtu(uart, speed);
        if (ctx == NULL) {
            debug_log(DLOG_ERROR, "%s: Unable to allocate modbus RTU node", __FUNCTION__);
            return NULL;
        }

        ctx->type = type; 
        ctx->mode = mode; 

        
        ctx->s = -1;

        return ctx;
    } else {
        debug_log(DLOG_ERROR, "%s: modbus type %d not supported", __FUNCTION__, type);
        return NULL;
    }
}


LOCAL gint32 modbus_open_uart_dev(modbus_t *ctx)
{
    gint32 ret;

    
    if (-1 != ctx->s) {
        (void)modbus_close(ctx);
        maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Modbus close UART port, fd=%d", ctx->s);
    }

    ret = modbus_connect(ctx);
    if (ret == MODBUS_ERR) {
        debug_log(DLOG_ERROR, "%s: modbus_connect failed", __FUNCTION__);
        return RET_ERR;
    }

    if (ctx->s == -1) {
        debug_log(DLOG_ERROR, "%s: bad uart dev fd", __FUNCTION__);
        return RET_ERR;
    }

    maintenance_log_v2(MLOG_INFO, FC__PUBLIC_OK, "Modbus open UART port, fd=%d", ctx->s);

    return RET_OK;
}


LOCAL void modbus_free_priv_data(gpointer modbus_priv_data)
{
    modbus_t *p_priv_data = NULL;

    if (modbus_priv_data == NULL) {
        return;
    }

    p_priv_data = (modbus_t *)modbus_priv_data;

    modbus_free(p_priv_data);

    return;
}


LOCAL gint32 modbus_rtu_init(OBJ_HANDLE obj_modbus)
{
    gint32 ret;
    modbus_t *mb_ctx = NULL;
    const gchar *modbus_name = NULL;

    modbus_name = dfl_get_object_name(obj_modbus);

    
    mb_ctx = modbus_new_handler(obj_modbus);
    if (mb_ctx == NULL) {
        debug_log(DLOG_ERROR, "%s:Failed to create modbus handler for %s", __FUNCTION__, modbus_name);
        return RET_ERR;
    }

    
    ret = modbus_open_uart_dev(mb_ctx);
    if (ret != RET_OK) {
        modbus_free(mb_ctx);
        debug_log(DLOG_ERROR, "%s:Open uart for %s failed.", __FUNCTION__, modbus_name);
        return RET_ERR;
    }

    
    ret = dfl_bind_object_data(obj_modbus, (gpointer)mb_ctx, modbus_free_priv_data);
    if (ret != DFL_OK) {
        modbus_free(mb_ctx);
        debug_log(DLOG_ERROR, "%s: failed to bind private data for %s. ret=%d", __FUNCTION__, modbus_name, ret);
        return RET_ERR;
    }

    return RET_OK;
}


LOCAL gint32 modbus_slave_init(OBJ_HANDLE obj_modbus)
{
    gint32 ret;

    
    ret = mb_common_dev_init(obj_modbus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: MB common dev init failed.", __FUNCTION__);
        return RET_ERR;
    }

    
    (void)asset_locator_init(obj_modbus);

    return RET_OK;
}


LOCAL gint32 modbus_channel_init(OBJ_HANDLE obj_modbus)
{
    gint32 ret;
    guint8 type = MODBUS_TYPE_RTU;
    const gchar *modbus_name = NULL;

    modbus_name = dfl_get_object_name(obj_modbus);

    (void)dal_get_property_value_byte(obj_modbus, PROPERTY_MODBUS_TYPE, &type);

    
    if (MODBUS_TYPE_RTU == type) {
        ret = modbus_bind_uart_port(obj_modbus);
        if (ret != RET_OK) {
            uart_connect_clean();
            debug_log(DLOG_ERROR, "%s: Bind uart and port for %s failed.ret=%d", __FUNCTION__, modbus_name, ret);
            return RET_ERR;
        }

        debug_log(DLOG_INFO, "%s: Bind uart and port for %s successfully", __FUNCTION__, modbus_name);

        ret = modbus_rtu_init(obj_modbus);
        if (ret != RET_OK) {
            return RET_ERR;
        }
    } else {
        debug_log(DLOG_ERROR, "%s:Modbus type %d not supported.", __FUNCTION__, type);
        return RET_ERR;
    }

    return RET_OK;
}


gint32 modbus_init(void)
{
    gint32 ret;
    GSList *modbus_list = NULL;
    GSList *modbus_node = NULL;
    OBJ_HANDLE obj_modbus;

    ret = dfl_get_object_list(CLASS_BUS_MODBUS, &modbus_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get obj list failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (modbus_node = modbus_list; modbus_node != NULL; modbus_node = modbus_node->next) {
        obj_modbus = (OBJ_HANDLE)modbus_node->data;

        ret = modbus_channel_init(obj_modbus);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: MB channel init failed.", __FUNCTION__);
            g_slist_free(modbus_list);
            return RET_ERR;
        }

        ret = modbus_slave_init(obj_modbus);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: MB slave init failed.", __FUNCTION__);
            g_slist_free(modbus_list);
            return RET_ERR;
        }
    }

    g_slist_free(modbus_list);

    return RET_OK;
}


LOCAL gint32 process_each_modbus(OBJ_HANDLE obj_modbus)
{
    gint32 ret;

    
    ret = mb_common_dev_start(obj_modbus);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: MB common dev start failed.", __FUNCTION__);
        return RET_ERR;
    }

    
    asset_locator_start(obj_modbus);

    return RET_OK;
}


gint32 modbus_start(void)
{
    gint32 ret;
    GSList *modbus_list = NULL;
    GSList *modbus_node = NULL;
    OBJ_HANDLE obj_modbus;

    ret = dfl_get_object_list(CLASS_BUS_MODBUS, &modbus_list);
    if (ret != DFL_OK) {
        debug_log(DLOG_ERROR, "%s:get obj list failed. ret=%d", __FUNCTION__, ret);
        return RET_ERR;
    }

    for (modbus_node = modbus_list; modbus_node != NULL; modbus_node = modbus_node->next) {
        obj_modbus = (OBJ_HANDLE)modbus_node->data;
        ret = process_each_modbus(obj_modbus);
        if (ret != RET_OK) {
            debug_log(DLOG_ERROR, "%s: Process each modbus failed.", __FUNCTION__);
            g_slist_free(modbus_list);
            return RET_ERR;
        }
    }

    g_slist_free(modbus_list);

    return RET_OK;
}


gint32 mb_set_debug_state(GSList *input_list)
{
    guchar debug_state = DISABLED;
    guchar ch;
    gint32 ret;
    const gchar *debug_state_str = NULL;

    ch = g_variant_get_byte(g_slist_nth_data(input_list, 0));
    debug_state = g_variant_get_byte(g_slist_nth_data(input_list, 1));
    debug_state_str = (ENABLED == debug_state) ? "Enabled" : "Disabled";

    ret = modbus_set_debug_state(ch, debug_state);

    debug_printf("Set channel [%d] debug state to %s %s.", ch, debug_state_str,
        (ret == RET_OK ? "successfully" : "failed"));

    return RET_OK;
}


gint32 method_modbus_pwr_out_switch(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
    gint32 ret;
    guint8 pwr_out_enable = 0;
    gchar       silk_name[MAX_PROPERTY_VALUE_LEN] = {0};

    if (input_list == NULL) {
        return RET_ERR;
    }

    pwr_out_enable = g_variant_get_byte((GVariant *)g_slist_nth_data(input_list, 0));

    
    ret = dal_set_property_value_byte(obj_handle, PROPERTY_MODBUS_PWR_OUT_ENABLE, pwr_out_enable, DF_SAVE);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s: set prop %s failed. ret=%d", __FUNCTION__, PROPERTY_MODBUS_PWR_OUT_ENABLE, ret);
    }

    (void)dal_get_property_value_string(obj_handle, PROPERTY_MODBUS_SILK_NAME, silk_name, sizeof(silk_name));
    method_operation_log(caller_info, NULL, "Set %s power output to %s %s", silk_name,
        (pwr_out_enable == TRUE) ? "enabled" : "disabled", (ret == RET_OK) ? "successfully" : "failed");

    return ret;
}


gint32 method_modbus_read_input_reg(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list,
    GSList **output_list)
{
#define READ_INPUT_REG_PARA_NUM 3
    gint32 ret;
    guint8 list_length = 0;
    guint8 uart = 0;
    modbus_t ctx = { 0 };
    guint32 slave_addr = 0;
    guint32 reg_addr = 0;
    guint8 reg_num = 0;
    guint16 *reg_buf = NULL;

    if ((obj_handle == 0) || (input_list == NULL) || (output_list == NULL)) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    list_length = g_slist_length(input_list);
    if (list_length != READ_INPUT_REG_PARA_NUM) {
        debug_log(DLOG_ERROR, "%s: input param number %d invalid, expect %d", __FUNCTION__, list_length,
            READ_INPUT_REG_PARA_NUM);
        return RET_ERR;
    }

    slave_addr = g_variant_get_uint32((GVariant *)(g_slist_nth_data(input_list, 0)));
    reg_addr = g_variant_get_uint16((GVariant *)(g_slist_nth_data(input_list, 1)));
    reg_num = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 2)));

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MODBUS_UART, &uart);

    
    ret = modbus_rtu_conn_ctx_init(uart, slave_addr, &ctx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Connect to slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr, ret);
        return ret;
    }

    reg_buf = (guint16 *)g_malloc0(reg_num * sizeof(guint16));
    if (reg_buf == NULL) {
        debug_log(DLOG_ERROR, "%s: malloc for reg_buf failed", __FUNCTION__);
        return RET_ERR;
    }

    ret = modbus_read_input_registers(&ctx, reg_addr, reg_num, reg_buf);
    if (ret == MODBUS_ERR) {
        g_free(reg_buf);
        debug_log(DLOG_ERROR, "%s: read %d reg at offset 0x%04x failed.ret=%d", __FUNCTION__, reg_num, reg_addr, ret);
        return ret;
    }

    
    *output_list = g_slist_append(*output_list,
        g_variant_new_fixed_array(G_VARIANT_TYPE_UINT16, reg_buf, (guint32)reg_num, sizeof(guint16)));

    g_free(reg_buf);

    return RET_OK;
}


gint32 method_modbus_send_cmd(OBJ_HANDLE obj_handle, GSList *caller_info, GSList *input_list, GSList **output_list)
{
    errno_t safe_fun_ret = EOK;
#define INPUT_PARA_VALID_NUM 4
    guint8 list_length = 0;
    guint32 slave_addr = 0;
    guint8 function_code = 0;
    const guint8 *input_data = NULL;
    guint32 input_data_size = 0;
    int rc = 0;
    guint8 uart = 0;
    gint32 ret = 0;
    gsize parm_temp = 0;
    modbus_t ctx = { 0 };
    MB_CMD_REQ_S mb_req = { 0 };
    MB_CMD_RESP_S mb_rsp = { { 0 } };
    guint32 timeout_ms = 0;

    if ((obj_handle == 0) || (input_list == NULL)) {
        debug_log(DLOG_ERROR, "%s: input parameter is not valid", __FUNCTION__);
        return RET_ERR;
    }

    list_length = g_slist_length(input_list);
    if (list_length != INPUT_PARA_VALID_NUM) {
        debug_log(DLOG_ERROR, "%s: input param number %d invalid, expect %d", __FUNCTION__, list_length,
            INPUT_PARA_VALID_NUM);
        return RET_ERR;
    }

    slave_addr = g_variant_get_uint32((GVariant *)(g_slist_nth_data(input_list, 0)));
    function_code = g_variant_get_byte((GVariant *)(g_slist_nth_data(input_list, 1)));
    input_data = (const guint8 *)g_variant_get_fixed_array((GVariant *)(g_slist_nth_data(input_list, 2)), &parm_temp,
        sizeof(guint8));
    timeout_ms = g_variant_get_uint32((GVariant *)(g_slist_nth_data(input_list, 3)));
    if (input_data == NULL || parm_temp == 0) {
        debug_log(DLOG_ERROR, "%s: input data is invalid", __FUNCTION__);
        return RET_ERR;
    }

    
    (void)dal_get_property_value_byte(obj_handle, PROPERTY_MODBUS_UART, &uart);

    
    ret = modbus_rtu_conn_ctx_init(uart, slave_addr, &ctx);
    if (ret != RET_OK) {
        debug_log(DLOG_ERROR, "%s:Connect to slave(0x%02X) failed. ret=%d", __FUNCTION__, slave_addr, ret);
        return ret;
    }

    mb_req.ctx = &ctx;
    input_data_size = (guint32)parm_temp;

    mb_req.data[0] = (guint8)slave_addr;
    mb_req.data[1] = function_code;
    safe_fun_ret = memcpy_s(mb_req.data + 2, sizeof(mb_req.data) - 2, input_data, input_data_size);
    if (safe_fun_ret != EOK) {
        debug_log(DLOG_ERROR, "%s: memcpy_s fail, ret = %d", __FUNCTION__, safe_fun_ret);
        return RET_ERR;
    }

    mb_req.req_len = input_data_size + 2;
    mb_req.timeout_ms = timeout_ms;

    rc = modbus_send_command(&mb_req, &mb_rsp);
    if (rc == MODBUS_ERR) {
        debug_log(DLOG_ERROR, "%s: send modbus command failed. ret=%d", __FUNCTION__, rc);
        return RET_ERR;
    }

    
    if (output_list != NULL) {
        *output_list = g_slist_append(*output_list,
            g_variant_new_fixed_array(G_VARIANT_TYPE_BYTE, mb_rsp.data, (guint32)rc, sizeof(guint8)));
    }

    return RET_OK;
}
