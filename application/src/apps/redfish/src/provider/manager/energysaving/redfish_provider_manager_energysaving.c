
#include "redfish_provider.h"

#define ACTIVE_PSU_NAME_LEN 5
#define ACTIVE_PSU_MASK_BIT_WIDTH 32
#define MASTER_SPARE_MODE "Failover"
#define LOAD_BALANCE_MODE "Sharing"

typedef struct tag_energysaving_cfg_info {
    guint8 demt_enabled;
    guint8 power_mode;
    guint32 active_psu_mask;
} ENERGY_SAVING_CFG_S;

LOCAL gint32 check_param_and_privi(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, json_object **o_result_jso)
{
    
    if (o_message_jso == NULL || o_result_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_expr(i_paras->is_satisfy_privi == 0, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    return VOS_OK;
}


LOCAL gint32 create_cfg_powermode_fail_message(gint32 ret_val, json_object **obj_msg, const gchar *prop_name)
{
    gchar prop_str[MAX_DES_LENGTH] = {0};
    gint32 ret;

    return_val_do_info_if_expr(obj_msg == NULL || prop_name == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: NULL pointer", __FUNCTION__));

    ret = snprintf_s(prop_str, MAX_DES_LENGTH, MAX_DES_LENGTH - 1, "%s/%s", prop_name,
        RFPROP_MANAGER_ENERGY_SAVING_POWER_CONTROL);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s err", __FUNCTION__));

    switch (ret_val) {
        case ERR_ACTIVE_PS_ONT_PST:
            (void)create_message_info(MSGID_POWER_ACTIVE_PS_NOT_PST, (const gchar *)prop_str, obj_msg, "PSU", "PSU");
            break;

        case ERR_NO_STANDBY_PS_PST:
            (void)create_message_info(MSGID_POWER_NO_STANDBY_PS_PST, (const gchar *)prop_str, obj_msg, "PSU", "PSU");
            break;

        case ERR_ACTIVE_PS_ONT_HLS:
            (void)create_message_info(MSGID_POWER_ACTIVE_PS_NOT_HLS, (const gchar *)prop_str, obj_msg, "PSU", "PSU");
            break;

        case ERR_CURRENT_POWER_HIGN:
            (void)create_message_info(MSGID_POWER_CURRENT_POWER_HIGN, (const gchar *)prop_str, obj_msg, "PSU", "PSU");
            break;

        case ERR_ACTIVE_PS_NUM_NE:
            (void)create_message_info(MSGID_POWER_ACTIVE_PS_NUM_NE, (const gchar *)prop_str, obj_msg, "PSU", "PSU");
            break;

        case ERR_PS_NOT_SUPPORT:
            (void)create_message_info(MSGID_POWER_PS_NOT_SUPPORT, (const gchar *)prop_str, obj_msg, "PSU", "PSU");
            break;

        case ERR_ACTIVE_STANDBY_MODE_UNSUPPORT:
            (void)create_message_info(MSGID_POWER_ACTIVE_STANDBY_MODE_UNSUPPORT, (const gchar *)prop_str, obj_msg,
                "PSU");
            break;

        case ERR_NAR_VOLT_ERROR:
            (void)create_message_info(MSGID_POWER_NAR_VOLTAGE_NOT_MATCH, (const gchar *)prop_str, obj_msg, "PSU");
            break;

        case ERR_NAR_PS_NOT_SUPPORT:
            (void)create_message_info(MSGID_POWER_PS_NOT_SUPPORT_NAR, NULL, obj_msg);
            break;

        default: 
            debug_log(DLOG_ERROR, "%s: ret = %d", __FUNCTION__, ret);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, obj_msg);
            return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_BAD_REQUEST;
}


LOCAL gint32 check_energy_saving_method_result(const gint32 ret_value, json_object **message_jso,
    const gchar *prop_name, const gchar *set_str)
{
    gchar prop_str[MAX_DES_LENGTH] = {0};
    gint32 ret;

    
    if (ret_value >= ERR_NAR_PS_NOT_SUPPORT && ret_value <= ERR_ACTIVE_PS_ONT_PST) {
        
        return create_cfg_powermode_fail_message(ret_value, message_jso, prop_name);
    }

    ret = snprintf_s(prop_str, MAX_DES_LENGTH, MAX_DES_LENGTH - 1, "%s/%s", prop_name,
        RFPROP_MANAGER_ENERGY_SAVING_DEMT_ENABLED);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s err", __FUNCTION__));

    
    switch (ret_value) {
        case VOS_OK:
        case RFERR_SUCCESS:
            return HTTP_OK;
        case BIOS_ERROR_ENERGY_SAVING_POWER_MODE_NOT_ALLOW:
            (void)create_message_info(MSGID_ENERGY_SAVING_POWER_MODE_NOT_ALLOWED, NULL, message_jso);
            return HTTP_BAD_REQUEST;
        case BIOS_ERROR_ENERGY_SAVING_ENABLED_NOT_ALLOWED:
            (void)create_message_info(MSGID_ENERGY_SAVING_ENABLED_NOT_ALLOWED, NULL, message_jso);
            return HTTP_BAD_REQUEST;
        case BIOS_ERROR_ENERGY_SAVING_CFG_NOT_ALLOWED:
            (void)create_message_info(MSGID_ENERGY_SAVING_NOT_ALLOWED, NULL, message_jso);
            return HTTP_BAD_REQUEST;
        case RFERR_WRONG_PARAM: 
            (void)create_message_info(MSGID_PROP_FORMAT_ERR, prop_name, message_jso, set_str, prop_name);
            return HTTP_BAD_REQUEST;
        case RFERR_INSUFFICIENT_PRIVILEGE: 
            (void)create_message_info(MSGID_PROP_MODIFY_NEED_PRIV, prop_name, message_jso, prop_name);
            return HTTP_FORBIDDEN;
        case BIOS_ERROR_STATUS_UNSUPPORTED:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)prop_str, message_jso,
                (const gchar *)prop_str);
            return HTTP_BAD_REQUEST;
        case BIOS_ERROR_PRODUCT_UNSUPPORTED:
            (void)create_message_info(MSGID_PROP_MODIFY_UNSUPPORT, (const gchar *)prop_str, message_jso,
                (const gchar *)prop_str);
            return HTTP_NOT_IMPLEMENTED;
        case BIOS_ERROR_BIOS_STATUS_UNSUPPORTED:
            (void)create_message_info(MSGID_BIOS_STATUS_UNSUPPORTED, (const gchar *)prop_str, message_jso,
                (const gchar *)prop_str);
            return HTTP_BAD_REQUEST;
        case BIOS_ERROR_ENERGY_SAVING_DISABLED:
            (void)create_message_info(MSGID_BIOS_ENERGY_SAVING_DISABLED, prop_name, message_jso, prop_name);
            return HTTP_BAD_REQUEST;
        case BIOS_ERROR_LICENSE_NOT_INSTALLED:
            (void)create_message_info(MSGID_LICENSE_NOT_INSTALL, NULL, message_jso);
            return HTTP_BAD_REQUEST;
        default: 
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, message_jso);
            debug_log(DLOG_ERROR, "%s: call method fail %d.", __FUNCTION__, ret);
            return HTTP_INTERNAL_SERVER_ERROR;
    }
}

LOCAL gint32 analysize_ouput_result(json_object **message_array, GSList *output_list, const gchar *property_des)
{
#define ENERGY_SAVING_CFG_ITEM_NUM 2 

    gint32 ret;
    gint32 ret_value;
    gint32 ret_val = HTTP_OK;
    GVariant *property_value = NULL;
    json_object *tmp_obj = NULL;

    
    guint8 output_num = g_slist_length(output_list);

    
    for (guint8 i = 0; i < output_num; i++) {
        property_value = (GVariant *)g_slist_nth_data(output_list, i);
        ret_value = g_variant_get_int32(property_value);

        ret = check_energy_saving_method_result(ret_value, &tmp_obj, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_CONFIG,
            property_des);
        do_info_if_true(ret != HTTP_OK, ret_val = ret; json_object_array_add(*message_array, tmp_obj));
    }

    
    return_val_if_expr(output_num < ENERGY_SAVING_CFG_ITEM_NUM, HTTP_OK);

    return ret_val;
}

LOCAL gint32 get_manager_energysaving_odata_id(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar slot[MAX_RSC_ID_LEN] = { 0 };
    gchar uri[MAX_URI_LENGTH] = { 0 };

    
    ret = check_param_and_privi(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    ret = redfish_get_board_slot(slot, sizeof(slot));
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s, %d: function  return err of redfish_slot_id.", __FUNCTION__, __LINE__));

    ret = snprintf_s(uri, sizeof(uri), sizeof(uri) - 1, ENERGY_SAVING_URI_ODATAID, slot);
    do_val_if_expr(ret <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s fail, ret = %d", __FUNCTION__, ret));
    *o_result_jso = json_object_new_string((const char *)uri);

    if (*o_result_jso == NULL) {
        debug_log(DLOG_ERROR, "%s, json_object_new_string fail.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    return HTTP_OK;
}


LOCAL gint32 is_support_energysaving_service(void)
{
    gint32 ret;
    guchar support_flag = 0;

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        &support_flag);
    return_val_do_info_if_expr((ret != VOS_OK), HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: get %s fail(%d).", __FUNCTION__, PROPERTY_PME_SERVICECONFIG_ENERGY_SAVING_SUPPORT,
        ret));
    return_val_if_expr((support_flag != SERVICE_NETCONFIG_ENABLE), HTTP_NOT_FOUND);

    return VOS_OK;
}


LOCAL gint32 get_property_energysaving_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    guchar property_value = 0;

    
    ret = check_param_and_privi(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get_object_handle fail(%d).\n", __FUNCTION__, ret));

    
    ret = dal_get_property_value_byte(obj_handle, PROPERTY_ENERGY_SAVING_ENABLE, &property_value);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get property_data fail(%d).\n", __FUNCTION__, ret));

    
    *o_result_jso = json_object_new_boolean(property_value);
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_boolean fail.\n", __FUNCTION__));
    return HTTP_OK;
}


LOCAL gint32 set_property_energysaving_enable(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar value_log[MAX_RSC_ID_LEN] = { 0 };
    OBJ_HANDLE obj_handle = 0;
    json_bool property_state;
    GSList *input_list = NULL;

    
    ret = check_param_and_privi(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    property_state = json_object_get_boolean(i_paras->val_jso);

    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get_object_handle fail(%d).\n", __FUNCTION__, ret));
    
    input_list = g_slist_append(input_list, g_variant_new_byte((guchar)property_state));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_ENERGY_SAVING, METHOD_ENERGY_SAVING_SET_ENABLE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    // sizeof(value_log)大小为64，大于格式化后字符串长度，无需判断返回值
    (void)snprintf_s(value_log, sizeof(value_log), sizeof(value_log) - 1, "%d", property_state);

    return check_energy_saving_method_result(ret, o_message_jso, RFPROP_MANAGER_ENERGY_SAVING_ENABLED, value_log);
}


LOCAL gint32 get_property_energysaving_deactive_threshold(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE object_handle = 0;
    guint16 property = 0;
    json_object *json_obj = NULL;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    
    return_val_do_info_if_expr(i_paras->is_satisfy_privi == 0, HTTP_FORBIDDEN,
        (void)create_message_info(MSGID_INSUFFICIENT_PRIVILEGE, NULL, o_message_jso));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    // 获取句柄信息
    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &object_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth return %d", __FUNCTION__, ret));

    
    ret = dal_get_property_value_uint16(object_handle, PROPERTY_ENERGY_SAVING_LOGIC_CPU_USAGE_THRE, &property);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL;
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_uint16 retuen %d\n", __FUNCTION__, ret));

    
    return_val_do_info_if_expr(property == INVALID_VAL, HTTP_OK,
        json_object_object_add(*o_result_jso, RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE, NULL));

    json_obj = json_object_new_int(property);
    return_val_do_info_if_expr(json_obj == NULL, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s: json_object_new_int fail\n", __FUNCTION__));

    json_object_object_add(*o_result_jso, RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE, json_obj);
    json_obj = NULL;

    return HTTP_OK;
}


LOCAL gint32 set_property_energysaving_deactive_threshold(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gchar value_log[MAX_RSC_ID_LEN] = {0};
    gchar prop_name[MAX_RSC_ID_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;
    gint32 property;
    GSList *input_list = NULL;
    json_object *tmp_obj = NULL;
    json_bool jso_ret;

    
    ret = check_param_and_privi(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    return_val_if_expr(json_object_object_length(i_paras->val_jso) == 0, HTTP_BAD_REQUEST);

    
    jso_ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE, &tmp_obj);
    if ((jso_ret != TRUE) || (tmp_obj == NULL)) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE);
        return HTTP_BAD_REQUEST;
    }

    
    if (json_object_get_type(tmp_obj) != json_type_int) {
        (void)create_message_info(MSGID_PROP_TYPE_ERR, RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE, o_message_jso,
            dal_json_object_get_str(i_paras->val_jso), RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE);
        return HTTP_BAD_REQUEST;
    }

    
    property = json_object_get_int(tmp_obj);

    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: get_object_handle fail(%d).\n", __FUNCTION__, ret));

    
    input_list = g_slist_append(input_list, g_variant_new_uint16((guint16)property));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_ENERGY_SAVING, METHOD_ENERGY_SAVING_LOGIC_CPU_UASGE_THRE, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);

    uip_free_gvariant_list(input_list);

    
    (void)snprintf_truncated_s(value_log, sizeof(value_log), "%d", property);

    (void)snprintf_truncated_s(prop_name, sizeof(prop_name), "%s/%s", RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_THRE,
        RFPROP_MANAGER_ENERGY_SAVING_CPU_USAGE_THRE);

    return check_energy_saving_method_result(ret, o_message_jso, prop_name, value_log);
}


LOCAL void get_energy_saving_active_psu_cfg(GSList *handle_list, guint8 power_mode, json_object *power_jso)
{
    gint32 ret;
    guint8 i;
    guint8 property = 0;
    guint8 ps_num = 0;
    gchar psu_name[MAX_PROP_NAME] = {0};
    OBJ_HANDLE obj_handle = 0;
    guint32 active_ps_mask = CAPACITY_DEFAULT_VAL;
    GSList *handle_list_node = NULL;
    gchar cfg_name[MAX_RSC_NAME_LEN] = {0};

    
    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle return %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &ps_num);
    return_do_info_if_expr(ret != VOS_OK || ps_num == 0,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM property return %d", __FUNCTION__, ret));

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_POWER, &property);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: dal_get_func_ability rerturn %d", __FUNCTION__, ret));

    for (handle_list_node = handle_list; handle_list_node != NULL; handle_list_node = handle_list_node->next) {
        obj_handle = (OBJ_HANDLE)(uintptr_t)handle_list_node->data;

        ret = dal_get_property_value_string(obj_handle, PROPERTY_ENERGY_SAVING_CFG_NAME, cfg_name, sizeof(cfg_name));
        continue_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "%s: get property_data fail(%d)\n", __FUNCTION__, ret));

        
        if (strcmp(cfg_name, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_PSU) == 0) {
            ret = dal_get_property_value_uint32(obj_handle, PROPERTY_ENERGY_SAVING_CFG_VALUE, &active_ps_mask);
            return_do_info_if_expr(ret != VOS_OK,
                debug_log(DLOG_ERROR, "%s: get property_data fail(%d).\n", __FUNCTION__, ret));
            break;
        }
    }

    
    return_do_info_if_expr(active_ps_mask == CAPACITY_DEFAULT_VAL,
        debug_log(DLOG_ERROR, "%s: no find active psu", __FUNCTION__));

    
    
    if (property != ENABLE || power_mode == POWER_CFG_LOAD_BALANCE) {
        active_ps_mask = 0;
        for (i = 0; i < ps_num; i++) {
            active_ps_mask |= (guint32)1 << i;
        }
    } else {
        
        active_ps_mask = ~active_ps_mask;
    }

    for (i = 0; i < ACTIVE_PSU_MASK_BIT_WIDTH; i++) {
        if (active_ps_mask & ((guint32)1 << i)) {
            
            (void)memset_s(psu_name, sizeof(psu_name), 0, sizeof(psu_name));
            //  sizeof(psu_name) 大小为32，大于格式化后字符串长度，无需判断返回值
            (void)snprintf_s(psu_name, sizeof(psu_name), sizeof(psu_name) - 1, POWER_SUPPLY_UNIT_NAME "%d", i + 1);

            json_object_array_add(power_jso, json_object_new_string((const gchar*)psu_name));
        }
    }
}


LOCAL gint32 get_energy_saving_power_mode_cfg(GSList *handle_list, guint8 *power_mode, json_object *power_jso)
{
    gint32 ret;
    guint8 property = 0;
    OBJ_HANDLE obj_handle = 0;
    GSList *handle_list_node = NULL;
    gchar cfg_name[MAX_RSC_NAME_LEN] = {0};

    
    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle return %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &property);
    return_val_do_info_if_expr(ret != VOS_OK || property == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM property return %d", __FUNCTION__, ret));

    
    ret = dal_get_func_ability(CLASS_NAME_PME_SERVICECONFIG, PROTERY_PME_SERVICECONFIG_POWER, &property);
    return_val_do_info_if_expr((ret != VOS_OK), VOS_ERR,
        debug_log(DLOG_ERROR, "%s: dal PROTERY_PME_SERVICECONFIG_POWER ability rerturn %d", __FUNCTION__, ret));

    *power_mode = INVALID_VAL;
    for (handle_list_node = handle_list; handle_list_node != NULL; handle_list_node = handle_list_node->next) {
        obj_handle = (OBJ_HANDLE)(uintptr_t)handle_list_node->data;

        ret = dal_get_property_value_string(obj_handle, PROPERTY_ENERGY_SAVING_CFG_NAME, cfg_name, sizeof(cfg_name));
        continue_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_string return %d\n", __FUNCTION__, ret));

        
        if (strcmp(cfg_name, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE) == 0) {
            ret = dal_get_property_value_byte(obj_handle, PROPERTY_ENERGY_SAVING_CFG_VALUE, power_mode);
            return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
                debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte return %d", __FUNCTION__, ret));
            break;
        }
    }

    
    if (property == 0 && *power_mode == POWER_CFG_MASTER_SPARE) {
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE,
            json_object_new_string(LOAD_BALANCE_MODE));
    } else if (*power_mode == POWER_CFG_MASTER_SPARE) {
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE,
            json_object_new_string(MASTER_SPARE_MODE));
    } else if (*power_mode == POWER_CFG_LOAD_BALANCE) {
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE,
            json_object_new_string(LOAD_BALANCE_MODE));
    } else {
        debug_log(DLOG_DEBUG, "%s: unkown power mode(%d)\n", __FUNCTION__, *power_mode);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL void get_energy_saving_power_control_attribute(GSList *handle_list, json_object *result_jso)
{
    gint32 ret;
    guint8 power_mode = INVALID_VAL;
    json_object *power_jso = NULL;
    json_object *psu_jso = NULL;

    
    power_jso = json_object_new_object();
    json_object_object_add(result_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_CONTROL, power_jso);
    return_do_info_if_expr(power_jso == NULL, debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    
    psu_jso = json_object_new_array();
    json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_PSU, psu_jso);
    do_if_expr(psu_jso == NULL, debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__));

    
    ret = get_energy_saving_power_mode_cfg(handle_list, &power_mode, power_jso);
    return_do_info_if_expr(ret != VOS_OK,
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE, NULL));

    return_if_expr(psu_jso == NULL);

    get_energy_saving_active_psu_cfg(handle_list, power_mode, psu_jso);
}


LOCAL void get_energy_saving_demt_cfg(GSList *handle_list, json_object *result_jso)
{
    GSList *handle_list_node = NULL;
    OBJ_HANDLE obj_handle;
    gchar cfg_name[MAX_RSC_NAME_LEN] = {0};
    guint8 demt_value = INVALID_VAL;
    gint32 ret;

    for (handle_list_node = handle_list; handle_list_node != NULL; handle_list_node = handle_list_node->next) {
        obj_handle = (OBJ_HANDLE)(uintptr_t)handle_list_node->data;

        ret = dal_get_property_value_string(obj_handle, PROPERTY_ENERGY_SAVING_CFG_NAME, cfg_name, sizeof(cfg_name));
        continue_do_info_if_expr(ret != VOS_OK,
            debug_log(DLOG_ERROR, "%s: dal_get_property_value_string return %d\n", __FUNCTION__, ret));

        
        if (strcmp(cfg_name, RFPROP_MANAGER_ENERGY_SAVING_DEMT_ENABLED) == 0) {
            ret = dal_get_property_value_byte(obj_handle, PROPERTY_ENERGY_SAVING_CFG_VALUE, &demt_value);
            return_do_info_if_expr(ret != VOS_OK,
                debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte return %d", __FUNCTION__, ret));
            break;
        }
    }

    
    if (demt_value == INVALID_VAL) {
        json_object_object_add(result_jso, (const gchar*)cfg_name, NULL);
    } else {
        json_object_object_add(result_jso, (const gchar*)cfg_name, json_object_new_boolean((json_bool)demt_value));
    }
}


LOCAL void get_energy_saving_cfg_attribute(OBJ_HANDLE obj_handle, json_object **o_result_jso,
    const gchar *property_name)
{
    gint32 ret;
    GSList *ref_handle_list = NULL;
    guint32 ref_obj_count = 0;

    
    ret = dal_get_ref_object_handle_list(obj_handle, property_name, &ref_handle_list, &ref_obj_count);
    return_do_info_if_expr((ret != VOS_OK || ref_obj_count == 0),
        debug_log(DLOG_ERROR, "%s: dal_get_ref_object_handle_list %s return %d", __FUNCTION__, property_name, ret));

    
    get_energy_saving_demt_cfg(ref_handle_list, *o_result_jso);

    
    get_energy_saving_power_control_attribute(ref_handle_list, *o_result_jso);
    g_slist_free(ref_handle_list);
}


LOCAL gint32 get_property_energysaving_active_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    // 获取句柄信息
    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR, (void)json_object_put(*o_result_jso);
        *o_result_jso = NULL; debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth %s return %d", __FUNCTION__,
        CLASS_NAME_ENERGY_SAVING, ret));

    
    get_energy_saving_cfg_attribute(obj_handle, o_result_jso, PROPERTY_ENERGY_SAVING_ACTIVE_CFG);

    return HTTP_OK;
}


LOCAL gint32 get_energy_saving_actual_power_mode_cfg(json_object *power_jso)
{
    gint32 ret;
    guint8 property = 0;
    OBJ_HANDLE obj_handle = 0;

    
    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle return %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &property);
    return_val_do_info_if_expr(ret != VOS_OK || property == 0, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM property return %d", __FUNCTION__, ret));

    
    ret = dfl_get_object_handle(OBJ_NAME_AMMETER, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle fail", __FUNCTION__));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_ACTUAL_POWER_MODE, &property);
    return_val_do_info_if_expr(ret != VOS_OK, VOS_ERR,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_ACTUAL_POWER_MODE fail", __FUNCTION__));

    if (property == POWER_CFG_MASTER_SPARE) {
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE,
            json_object_new_string(MASTER_SPARE_MODE));
    } else if (property == POWER_CFG_LOAD_BALANCE) {
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE,
            json_object_new_string(LOAD_BALANCE_MODE));
    } else {
        debug_log(DLOG_ERROR, "%s: unkown power mode(%d)\n", __FUNCTION__, property);
        return VOS_ERR;
    }

    return VOS_OK;
}


LOCAL void get_energy_saving_actual_active_psu(json_object *power_jso)
{
    gint32 ret;
    guint8 psu_num = 0;
    guint8 i;
    guint8 uint8_value = 0;
    gchar device_name[POWER_STR_MAX_LEN] = {0};
    OBJ_HANDLE obj_handle = 0;

    return_do_info_if_expr(power_jso == NULL, debug_log(DLOG_ERROR, "%s: NULL param", __FUNCTION__));

    
    ret = dfl_get_object_handle(OBJ_PRODUCT_COMPONENT, &obj_handle);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get OBJ_PRODUCT_COMPONENT handle return %d", __FUNCTION__, ret));

    ret = dal_get_property_value_byte(obj_handle, PROPERTY_COMPOENT_PS_NUM, &psu_num);
    return_do_info_if_expr(ret != VOS_OK,
        debug_log(DLOG_ERROR, "%s: get PROPERTY_COMPOENT_PS_NUM return %d", __FUNCTION__, ret));

    for (i = 0; i < psu_num; i++) {
        ret = get_ps_handle_by_slot(i, &obj_handle);
        continue_if_expr(ret != VOS_OK);

        
        ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_PRESENCE, &uint8_value);
        continue_if_expr((ret != VOS_OK) || (uint8_value == 0));

        
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_ACTUAL_ACTIVE, &uint8_value);
        continue_if_expr(ret != VOS_OK || uint8_value != RF_POWER_CONTROL_PSU_ACTIVE_POWER);

        
        ret = dal_get_property_value_string(obj_handle, PROTERY_PS_DEVICENAME, device_name, sizeof(device_name));
        if (ret != RET_OK) {
            continue;
        }
        
        json_object_array_add(power_jso, json_object_new_string((const gchar*)device_name));
    }
}


LOCAL gint32 get_property_energysaving_actual_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;
    json_object *tmp_jso = NULL;
    json_object *power_jso = NULL;
    json_object *psu_jso = NULL;
    guint8 uint8_value = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    
    ret = dal_get_object_handle_nth(BIOS_CLASS_NAME, 0, &obj_handle);
    if (ret == VOS_OK) {
        ret = dal_get_property_value_byte(obj_handle, PROPERTY_BIOS_DEMT_CONFIG_ITEM, &uint8_value);
        do_info_if_true(ret == VOS_OK, tmp_jso = json_object_new_boolean((json_bool)uint8_value);
            json_object_object_add(*o_result_jso, RFPROP_MANAGER_ENERGY_SAVING_DEMT_ENABLED, tmp_jso));
    }

    
    power_jso = json_object_new_object();
    json_object_object_add(*o_result_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_CONTROL, power_jso);
    return_val_do_info_if_expr(power_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    
    psu_jso = json_object_new_array();
    json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_PSU, psu_jso);
    do_if_expr(psu_jso == NULL, debug_log(DLOG_ERROR, "%s: json_object_new_array fail", __FUNCTION__));

    
    ret = get_energy_saving_actual_power_mode_cfg(power_jso);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        json_object_object_add(power_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE, NULL));

    get_energy_saving_actual_active_psu(psu_jso);

    return HTTP_OK;
}


LOCAL gint32 get_property_energysaving_cancel_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    OBJ_HANDLE obj_handle = 0;

    
    if (o_result_jso == NULL || o_message_jso == NULL || provider_paras_check(i_paras) != VOS_OK) {
        debug_log(DLOG_ERROR, "%s: NULL pointer.", __FUNCTION__);
        return HTTP_INTERNAL_SERVER_ERROR;
    }

    // 获取句柄信息
    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth %s return %d", __FUNCTION__, CLASS_NAME_ENERGY_SAVING,
        ret));

    *o_result_jso = json_object_new_object();
    return_val_do_info_if_expr(*o_result_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_object fail", __FUNCTION__));

    
    get_energy_saving_cfg_attribute(obj_handle, o_result_jso, PROPERTY_ENERGY_SAVING_CANCEL_CFG);

    return HTTP_OK;
}

LOCAL gint32 add_active_psu_mask(json_object **o_message_jso, const gchar *property_name, const gchar *active_psu,
    const size_t arry_index, guint32 *active_psu_mask)
{
    gchar error_str[MAX_STRBUF_LEN] = {0};
    gint32 ret;
    guint8 psu_id = 0;
    OBJ_HANDLE obj_handle = 0;

    ret =
        snprintf_s(error_str, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s/%zu: %s", property_name, arry_index, active_psu);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    
    ret = dal_get_specific_object_string(CLASS_NAME_PS, PROTERY_PS_DEVICENAME, active_psu, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: dal_get_specific_object_string return %d,%s param err", __FUNCTION__, ret,
        error_str);
        (void)create_message_info(MSGID_PROP_FORMAT_ERR, property_name, o_message_jso, (const gchar *)error_str,
        property_name));

    ret = dal_get_property_value_byte(obj_handle, PROTERY_PS_INDEX, &psu_id);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_property_value_byte return %d", __FUNCTION__, ret);
        (void)(void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    *active_psu_mask &= (~((guint32)1 << psu_id));
    return VOS_OK;
}

LOCAL gint32 check_array_jso_valid(json_object *property_jso, size_t *array_len)
{
    size_t i;
    const gchar *jso_str = NULL;

    
    *array_len = json_object_array_length(property_jso);
    if (*array_len <= 0 || *array_len > PS_DEV_CNT_MAX) {
        debug_log(DLOG_ERROR, "%s: json_object_array_length = 0", __FUNCTION__);
        return RET_ERR;
    }

    for (i = 0; i < *array_len; i++) {
        jso_str = dal_json_object_get_str(json_object_array_get_idx(property_jso, i));
        // 有效的成员为非空对象
        if ((g_strcmp0(jso_str, JSON_NULL_OBJECT_STR) != 0) && (strlen(jso_str) != 0)) {
            return VOS_OK;
        }
    }
    return VOS_ERR;
}

LOCAL gint32 get_active_psu_cfg_from_json(json_object *power_obj, json_object **o_message_jso,
    const gchar *rf_prop_name, ENERGY_SAVING_CFG_S *energy_saving_cfg)
{
    json_bool jso_ret;
    json_object *tmp_obj = NULL;
    json_object *jso_value = NULL;
    size_t arr_len = 0;
    size_t i;
    const gchar *jso_str = NULL;
    gchar psu_name[MAX_STRBUF_LEN] = {0};
    gint32 ret;
    gint32 error_index = 0;
    gchar error_str[MAX_STRBUF_LEN] = {0};
    gchar property_name[MAX_STRBUF_LEN] = {0};

    ret = snprintf_s(property_name, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s/%s/%s", rf_prop_name,
        RFPROP_MANAGER_ENERGY_SAVING_POWER_CONTROL, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_PSU);
    return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
        (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

    
    jso_ret = json_object_object_get_ex(power_obj, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_PSU, &tmp_obj);
    return_val_do_info_if_expr(jso_ret != TRUE || tmp_obj == NULL, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: json_object_object_get_ex error", __FUNCTION__);
        (void)create_message_info(MSGID_PROP_ITEM_MISSING, (const gchar *)property_name, o_message_jso,
        (const gchar *)property_name));

    
    ret = check_array_jso_valid(tmp_obj, &arr_len);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_BAD_REQUEST,
        debug_log(DLOG_ERROR, "%s: check_array_valid fail", __FUNCTION__);
        (void)create_message_info(MSGID_ARRAY_REQUIRED_ITEM, (const char *)property_name, o_message_jso,
        (const char *)property_name));

    
    ret = json_array_duplicate_check(tmp_obj, &error_index);
    if (ret == RF_OK) {
        ret = snprintf_s(error_str, MAX_STRBUF_LEN, MAX_STRBUF_LEN - 1, "%s/%d", property_name, error_index);
        return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

        (void)create_message_info(MSGID_PROP_ITEM_DUPLICATE, (const gchar *)error_str, o_message_jso,
            (const gchar *)error_str);
        return HTTP_BAD_REQUEST;
    }

    for (i = 0; i < arr_len; i++) {
        jso_value = json_object_array_get_idx(tmp_obj, i);
        return_val_if_expr(jso_value == NULL, VOS_ERR);

        jso_str = dal_json_object_get_str(jso_value);
        return_val_do_info_if_expr(jso_str == NULL, HTTP_INTERNAL_SERVER_ERROR,
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

        
        if (g_strcmp0(jso_str, JSON_NULL_OBJECT_STR) == 0 || strlen(jso_str) == 0) {
            continue;
        }

        (void)memset_s(psu_name, sizeof(psu_name), 0, sizeof(psu_name));
        ret = snprintf_s(psu_name, sizeof(psu_name), sizeof(psu_name) - 1, "%s", jso_str);
        return_val_do_info_if_expr(ret <= 0, HTTP_INTERNAL_SERVER_ERROR,
            debug_log(DLOG_ERROR, "%s: snprintf_s fail", __FUNCTION__);
            (void)create_message_info(MSGID_INTERNAL_ERR, NULL, o_message_jso));

        ret = add_active_psu_mask(o_message_jso, property_name, psu_name, i, &energy_saving_cfg->active_psu_mask);
        return_val_if_expr(ret != VOS_OK, ret);
    }
    return VOS_OK;
}


LOCAL gint32 get_cfg_vaule_form_json(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso, const gchar *prop_name,
    ENERGY_SAVING_CFG_S *energy_saving_cfg)
{
    json_bool jso_ret;
    json_object *tmp_obj = NULL;
    json_object *power_obj = NULL;
    json_bool demt_state;
    gint32 ret = VOS_OK;
    const gchar *powermode_str = NULL;

    
    energy_saving_cfg->demt_enabled = INVALID_VAL;
    energy_saving_cfg->power_mode = INVALID_VAL;
    energy_saving_cfg->active_psu_mask = CAPACITY_DEFAULT_VAL;

    
    jso_ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ENERGY_SAVING_DEMT_ENABLED, &tmp_obj);
    if ((jso_ret == TRUE) && (tmp_obj != NULL)) {
        demt_state = json_object_get_boolean(tmp_obj);
        energy_saving_cfg->demt_enabled = (guint8)demt_state;
    }

    
    jso_ret = json_object_object_get_ex(i_paras->val_jso, RFPROP_MANAGER_ENERGY_SAVING_POWER_CONTROL, &power_obj);
    if ((jso_ret == TRUE) && (power_obj != NULL)) {
        tmp_obj = NULL;

        
        jso_ret = json_object_object_get_ex(power_obj, RFPROP_MANAGER_ENERGY_SAVING_POWER_MODE, &tmp_obj);
        if ((jso_ret == TRUE) && (tmp_obj != NULL)) {
            powermode_str = dal_json_object_get_str(tmp_obj);
        }

        if (powermode_str != NULL) {
            if (strcmp(powermode_str, MASTER_SPARE_MODE) == 0) {
                energy_saving_cfg->power_mode = POWER_CFG_MASTER_SPARE;

                
                ret = get_active_psu_cfg_from_json(power_obj, o_message_jso, prop_name, energy_saving_cfg);
            } else if (strcmp(powermode_str, LOAD_BALANCE_MODE) == 0) {
                energy_saving_cfg->power_mode = POWER_CFG_LOAD_BALANCE;
                energy_saving_cfg->active_psu_mask = 0;
            }
        }
    }

    return ret;
}


LOCAL gint32 set_property_energysaving_active_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    gchar log_str[MAX_RSC_NAME_LEN] = {0};
    GSList *input_list = NULL;
    GSList *output_list = NULL;
    json_object *tmp_obj = NULL;
    ENERGY_SAVING_CFG_S energy_saving_cfg;

    
    ret = check_param_and_privi(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    return_val_if_expr(json_object_object_length(i_paras->val_jso) == 0, HTTP_BAD_REQUEST);

    ret =
        get_cfg_vaule_form_json(i_paras, o_message_jso, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_CONFIG, &energy_saving_cfg);
    return_val_if_expr(ret != VOS_OK, ret);

    *o_message_jso = json_object_new_array();
    return_val_do_info_if_expr(*o_message_jso == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: json_object_new_array fail\n", __FUNCTION__));

    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth return %d.", __FUNCTION__, ret));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(energy_saving_cfg.demt_enabled));
    input_list = g_slist_append(input_list, g_variant_new_byte(energy_saving_cfg.power_mode));
    input_list = g_slist_append(input_list, g_variant_new_uint32(energy_saving_cfg.active_psu_mask));
    return_val_do_info_if_expr(input_list == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: g_slist_append fail.", __FUNCTION__));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_ENERGY_SAVING, METHOD_ENERGY_SAVING_ACTIVE_CFG, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, &output_list);
    uip_free_gvariant_list(input_list);

    ret_val = snprintf_s(log_str, sizeof(log_str), sizeof(log_str) - 1, "DEMTEnabled: %d, PowerMode: %s",
        energy_saving_cfg.demt_enabled,
        (energy_saving_cfg.power_mode == POWER_CFG_MASTER_SPARE) ? MASTER_SPARE_MODE : LOAD_BALANCE_MODE);
    do_if_expr(ret_val <= 0, debug_log(DLOG_ERROR, "%s: snprintf_s %d fail.", __FUNCTION__, ret_val));

    if (ret == VOS_OK) {
        ret_val = analysize_ouput_result(o_message_jso, output_list, (const gchar *)log_str);
    } else {
        ret_val = check_energy_saving_method_result(ret, &tmp_obj, RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_CONFIG, log_str);
        json_object_array_add(*o_message_jso, tmp_obj);
    }

    uip_free_gvariant_list(output_list);
    return ret_val;
}


LOCAL gint32 set_property_energysaving_cancel_cfg(PROVIDER_PARAS_S *i_paras, json_object **o_message_jso,
    json_object **o_result_jso)
{
    gint32 ret;
    gint32 ret_val;
    OBJ_HANDLE obj_handle = 0;
    gchar log_str[MAX_RSC_NAME_LEN] = {0};
    GSList *input_list = NULL;
    ENERGY_SAVING_CFG_S energy_saving_cfg;

    
    ret = check_param_and_privi(i_paras, o_message_jso, o_result_jso);
    return_val_if_expr(ret != VOS_OK, ret);

    
    return_val_if_expr(json_object_object_length(i_paras->val_jso) == 0, HTTP_BAD_REQUEST);

    ret = get_cfg_vaule_form_json(i_paras, o_message_jso, RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_CONFIG,
        &energy_saving_cfg);
    return_val_if_expr(ret != VOS_OK, ret);

    ret = dal_get_object_handle_nth(CLASS_NAME_ENERGY_SAVING, 0, &obj_handle);
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: dal_get_object_handle_nth return %d.", __FUNCTION__, ret));

    
    input_list = g_slist_append(input_list, g_variant_new_byte(energy_saving_cfg.demt_enabled));
    input_list = g_slist_append(input_list, g_variant_new_byte(energy_saving_cfg.power_mode));
    input_list = g_slist_append(input_list, g_variant_new_uint32(energy_saving_cfg.active_psu_mask));
    return_val_do_info_if_expr(input_list == NULL, HTTP_INTERNAL_SERVER_ERROR,
        debug_log(DLOG_ERROR, "%s: g_slist_append fail.", __FUNCTION__));

    ret = uip_call_class_method_redfish(i_paras->is_from_webui, i_paras->user_name, i_paras->client, obj_handle,
        CLASS_NAME_ENERGY_SAVING, METHOD_ENERGY_SAVING_CANCEL_CFG, AUTH_ENABLE, i_paras->user_role_privilege,
        input_list, NULL);
    uip_free_gvariant_list(input_list);

    
    (void)snprintf_s(log_str, sizeof(log_str), sizeof(log_str) - 1, "DEMT: %d, PowerMode: %d",
        energy_saving_cfg.demt_enabled, energy_saving_cfg.power_mode);
    ret_val =
        check_energy_saving_method_result(ret, o_message_jso, RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_CONFIG, log_str);

    return ret_val;
}

LOCAL PROPERTY_PROVIDER_S g_manager_energysaving_provider[] = {
    {RFPROP_ODATA_ID, MAP_PROPERTY_NULL, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_NULL, get_manager_energysaving_odata_id, NULL, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ENERGY_SAVING_ENABLED, CLASS_NAME_ENERGY_SAVING, PROPERTY_ENERGY_SAVING_ENABLE, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_energysaving_enable, set_property_energysaving_enable, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_THRE, CLASS_NAME_ENERGY_SAVING, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_energysaving_deactive_threshold, set_property_energysaving_deactive_threshold, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ENERGY_SAVING_DEACTIVE_CONFIG, CLASS_NAME_ENERGY_SAVING, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_energysaving_cancel_cfg, set_property_energysaving_cancel_cfg, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ENERGY_SAVING_ACTIVE_CONFIG, CLASS_NAME_ENERGY_SAVING, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_energysaving_active_cfg, set_property_energysaving_active_cfg, NULL, ETAG_FLAG_ENABLE},
    {RFPROP_MANAGER_ENERGY_SAVING_ACTUAL_CONFIG, CLASS_NAME_ENERGY_SAVING, MAP_PROPERTY_NULL, USERROLE_READONLY, SYS_LOCKDOWN_FORBID, get_property_energysaving_actual_cfg, NULL, NULL, ETAG_FLAG_ENABLE}
};


gint32 manager_energysaving_provider_entry(PROVIDER_PARAS_S *i_paras, PROPERTY_PROVIDER_S **prop_provider,
    guint32 *count)
{
    gint32 ret;

    
    return_val_if_expr((i_paras == NULL) || (prop_provider == NULL) || (count == NULL), HTTP_INTERNAL_SERVER_ERROR);

    
    ret = is_support_energysaving_service();
    return_val_do_info_if_expr(ret != VOS_OK, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: cannot support energy saving service ", __FUNCTION__));

    
    ret = redfish_check_manager_uri_valid(i_paras->uri);
    return_val_do_info_if_expr(ret != TRUE, HTTP_NOT_FOUND,
        debug_log(DLOG_ERROR, "%s: invalid uri is %s", __FUNCTION__, i_paras->uri));

    *prop_provider = g_manager_energysaving_provider;
    *count = sizeof(g_manager_energysaving_provider) / sizeof(PROPERTY_PROVIDER_S);
    return VOS_OK;
}
