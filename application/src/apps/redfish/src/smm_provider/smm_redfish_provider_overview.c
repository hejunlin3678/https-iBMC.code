
#include "redfish_provider.h"
#include "redfish_forward.h"


const gchar *get_redfish_health_level(guint8 severity_level)
{
    const gchar*    health_string[] = {RF_STATUS_OK, RF_STATUS_WARNING, RF_STATUS_WARNING, RF_STATUS_CRITICAL};

    if (severity_level > HEALTH_CRITICAL) {
        debug_log(DLOG_ERROR, "unknown health severity level:%d", severity_level);
        return NULL;
    }

    if (severity_level >= G_N_ELEMENTS(health_string)) {
        debug_log(DLOG_ERROR, "wrong event type with severity level, severity level:%d", severity_level);
        return NULL;
    }

    return health_string[severity_level];
}


LOCAL gint32 get_one_component_info(OBJ_HANDLE handle, gpointer user_data)
{
    errno_t safe_fun_ret;
    json_object *components_obj = (json_object *)user_data;
    json_object *component_obj = NULL;
    guint8 blade_type = 0;
    guint8 hotswap = 0;
    guint8 width = 0;
    guint8 managed = 0;
    guint8 presence = 0;
    const gchar *blade_name = NULL;
    guint8 smm_slave_addr = 0;
    guint8 health_level = 0;
    guint8 slave_addr = 0;
    gint32 ret = VOS_ERR;
    const gchar *redfish_health_str = NULL;
    gchar blade_name_f[MAX_NAME_SIZE] = { 0 };

    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_PRESENCE, &presence);
    if (presence != BLADE_PRESENCE) {
        return VOS_OK;
    }
    blade_name = dfl_get_object_name(handle);
    safe_fun_ret = strncpy_s(blade_name_f, sizeof(blade_name_f), blade_name, sizeof(blade_name_f) - 1);
    do_val_if_expr(safe_fun_ret != EOK,
        debug_log(DLOG_ERROR, "%s: strncpy_s fail, ret = %d\n", __FUNCTION__, safe_fun_ret));

    component_obj = json_object_new_object();
    goto_label_do_info_if_fail(component_obj, FAIL_EXIT,
        debug_log(DLOG_ERROR, "%s, %d: json_object_new_object fail.\n", __FUNCTION__, __LINE__););

    
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_SLAVEADDR, &slave_addr);
    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_BLADETYPE, &blade_type);
    if (blade_type == 0x03) { // 管理板
        ret = get_current_smm_slave_addr(&smm_slave_addr);
        goto_label_do_info_if_expr(VOS_OK != ret, FAIL_EXIT,
            debug_log(DLOG_ERROR, "[%s]get_current_smm_slave_addr failed.", __func__));
        if (smm_slave_addr == slave_addr) { // 本板
            (void)redfish_get_smm_health(&health_level);
        } else {
            (void)redfish_get_blade_health(slave_addr, &health_level); // other smm
        }
        blade_name_f[0] = 'H'; // SMM1 to HMM1
    } else {
        (void)redfish_get_blade_health(slave_addr, &health_level); // blade / swi
        format_string_as_upper_head(blade_name_f, strlen(blade_name_f));
    }
    redfish_health_str = get_redfish_health_level(health_level);

    (void)dal_get_property_value_byte(handle, PROTERY_IPMBETH_BLADE_HOTSWAP, &hotswap);
    (void)dal_get_property_value_byte(handle, PROPERTY_IPMBETH_BOARD_WIDTH_TYPE, &width);
    (void)dal_get_property_value_byte(handle, PROPERTY_IPMBETH_BOARD_MANAGED, &managed);

    json_object_object_add(component_obj, RFPROP_OVERVIEW_ID, json_object_new_string((const gchar *)blade_name_f));
    json_object_object_add(component_obj, RFPROP_OVERVIEW_HOTSWAPSTATE, json_object_new_int(hotswap));
    json_object_object_add(component_obj, RFPROP_OVERVIEW_HEALTH,
        redfish_health_str ? json_object_new_string(redfish_health_str) : NULL);
    json_object_object_add(component_obj, RFPROP_OVERVIEW_MANAGEDBYMM,
        managed ? json_object_new_boolean(managed - 1) : NULL);
    (void)convert_width_height(width, component_obj);
    json_object_array_add(components_obj, component_obj);
    return VOS_OK;
FAIL_EXIT:
    json_object_put(component_obj);
    return VOS_OK; // 返回ok继续循环
}


gint32 smm_get_overview(PROVIDER_PARAS_S *i_paras, json_object *o_rsc_jso, json_object *o_err_array_jso,
    gchar **o_rsp_body_str)
{
    json_object *msg_obj = NULL;
    json_object *components_obj = NULL;
    
    if (NULL == o_rsc_jso || NULL == o_err_array_jso || VOS_OK != provider_paras_check(i_paras)) {
        debug_log(DLOG_ERROR, "[%s] NULL pointer.", __func__);
        create_message_info(MSGID_INTERNAL_ERR, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
        return HTTP_INTERNAL_SERVER_ERROR;
    }
    
    return_val_do_info_if_fail(i_paras->user_role_privilege & USERROLE_READONLY, HTTP_FORBIDDEN, {
        create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, &msg_obj);
        json_object_array_add(o_err_array_jso, msg_obj);
    });

    components_obj = json_object_new_array();
    (void)dfl_foreach_object(CLASS_NAME_IPMBETH_BLADE, get_one_component_info, components_obj, NULL);
    json_object_object_add(o_rsc_jso, RFPROP_OVERVIEW_COMPONENTS, components_obj);

    return VOS_OK;
}